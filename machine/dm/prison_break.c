/*
 * demolition man
 * prison_break.c
 *
 * written by James Cardona
 *
 * Location Description:
 * mode started by claw switch Prison Break
 *
 * Scoring Description: (original game)
 * This is an award/shoot mode that's very quick. The ball is dropped above the upper left
 * flipper and you are awarded 15 million. A reflex shot into the
 * Underground doubles this, a reflex shot into the side ramp triples this.
 * Prison Break also lights Car Chase which is an alternating ramp award
 * very similar to PayBack Time on T2. Awards are 5 million per successful ramp.
 *
 * Scoring Description: (my rules)
 * I do not like this mode and plan on completely changing it to something else,
 * as it is right now we are using prison_break style scoring on only side ramp and underground shots
 *
 */
/* CALLSET_SECTION (prison_break, __machine3__) */

#include <freewpc.h>
#include "dm/global_constants.h"
#include "clawmagnet.h"

//constants
const U8 			PRISON_BREAK_EASY_GOAL 	= 3;
const U8 			PRISON_BREAK_MED_GOAL 	= 4;
const U8 			PRISON_BREAK_HARD_GOAL 	= 5;
const U8 			PRISON_BREAK_GOAL_INCREMENT = 1;
const U8 			PRISON_BREAK_GOAL_MAX 	= 10;

//local variables
U8 			prison_break_mode_shots_made; //number of shots made this mode
__local__ U8 			prison_break_modes_achieved;
U8			prison_break_mode_timer;
score_t 	prison_break_mode_score; //score for this mode only
score_t 	prison_break_mode_last_score;
score_t 	prison_break_mode_next_score;
score_t 	prison_break_mode_score_total_score;

//external variables

//internally called function prototypes  --external found at protos.h
void prison_break_reset (void);
void prison_break_player_reset (void);
void prison_break_effect_deff(void);
void prison_break_mode_init (void);
void prison_break_mode_expire (void);
void prison_break_mode_exit (void);

/****************************************************************************
 * mode definition structure
 ***************************************************************************/
struct timed_mode_ops prison_break_mode = {
	DEFAULT_MODE,
	.init = prison_break_mode_init,
	.exit = prison_break_mode_exit,
	.gid = GID_PRISON_BREAK_MODE_RUNNING,
	.music = MUS_MD_CRYO_PRISON_BREAKOUT,
	.deff_starting = DEFF_PRISON_BREAK_START_EFFECT,
	.deff_running = DEFF_PRISON_BREAK_EFFECT,
//	.deff_ending = DEFF_prison_break_END_EFFECT,
	.prio = PRI_GAME_MODE6,
	.init_timer = 23,
	.timer = &prison_break_mode_timer,
	.grace_timer = 2,
//	.pause = system_timer_pause,
};


/****************************************************************************
 * initialize  and exit
 ***************************************************************************/
void prison_break_reset (void) {
	flag_off (FLAG_IS_PBREAK_ACTIVATED);
}//end of function



void prison_break_player_reset (void) {
	prison_break_reset();
	prison_break_modes_achieved = 0;
	prison_break_mode_shots_made = 0;
	score_zero(prison_break_mode_score_total_score);
}//end of function



void prison_break_mode_init (void) {
			//the claw mode can expire on its own and since it is a lower priority it will not display
			//callset_invoke (end_claw_mode); // this seemed to cause occasional crashes
			clawmagnet_off ();
			flag_off(FLAG_IS_BALL_ON_CLAW);
			flipper_enable ();
	prison_break_mode_shots_made = 0;
	++prison_break_modes_achieved;
	sound_start (ST_SPEECH, SPCH_CRYO_PRISON_BREAKOUT, SL_4S, PRI_GAME_QUICK5);
	//flash lamp for a time
	lamp_tristate_flash(LM_CLAW_PRISON_BREAK);
	task_sleep(TIME_500MS);
	lamp_tristate_on(LM_CLAW_PRISON_BREAK);
	score_zero(prison_break_mode_score);
	score_zero(prison_break_mode_last_score);
	score_add(prison_break_mode_last_score, score_table[SC_500K]);

	score_zero(prison_break_mode_next_score);
	switch (prison_break_modes_achieved) {
		case 1: score(SC_15M); 	score_add(prison_break_mode_score, score_table[SC_15M]);	break;
		case 2: score(SC_20M);  score_add(prison_break_mode_score, score_table[SC_20M]);	break;
		default: score(SC_25M);  score_add(prison_break_mode_score, score_table[SC_25M]);
	}//end of switch
	flag_on (FLAG_IS_PBREAK_ACTIVATED);
}//end of function



void prison_break_mode_expire (void) {
	flag_off (FLAG_IS_PBREAK_ACTIVATED);
}//end of function


void prison_break_mode_exit (void) { prison_break_mode_expire();}


/****************************************************************************
 * external event listeners
 ****************************************************************************/
CALLSET_ENTRY (prison_break, music_refresh)  	{ timed_mode_music_refresh (&prison_break_mode); }
CALLSET_ENTRY (prison_break, end_ball) 		{ if (timed_mode_running_p(&prison_break_mode) ) timed_mode_end (&prison_break_mode); }
CALLSET_ENTRY (prison_break, display_update) 	{ timed_mode_display_update (&prison_break_mode); }

CALLSET_ENTRY (prison_break, start_player) 	{ prison_break_player_reset(); }
CALLSET_ENTRY (prison_break, start_ball) 		{ prison_break_reset(); }




/****************************************************************************
 * body
 *
 ***************************************************************************/
CALLSET_ENTRY (prison_break, sw_claw_prison_break) {
	timed_mode_begin (&prison_break_mode);//start mode
}//end of function



void prison_break_made (void) {
	++prison_break_mode_shots_made;
	score_add(prison_break_mode_score, prison_break_mode_last_score);
	score_add (prison_break_mode_score_total_score, prison_break_mode_last_score);
	score_long (prison_break_mode_last_score);
	deff_start (DEFF_PRISON_BREAK_HIT_EFFECT);//under /kernel/deff.c
}//end of function




/****************************************************************************
 * DMD display effects
 ****************************************************************************/
void prison_break_start_effect_deff(void) {
	U8 i;
	for (i=1; i < 9; i++) {			//SPELL OUT LETTeRS
			dmd_alloc_low_clean ();
			sprintf ("BREAKOUT");
			if (i < 9)
				sprintf_buffer[i] = '\0';
			font_render_string_center (&font_v5prc, DMD_MIDDLE_X, DMD_BIG_CY_Cent, sprintf_buffer);
			dmd_show_low ();
			task_sleep (TIME_300MS);
	}
	task_sleep_sec (1);
	deff_exit ();
}//end of mode_effect_deff



void prison_break_hit_effect_deff(void) {
	dmd_alloc_pair ();
	dmd_clean_page_low ();
	sound_start (ST_SAMPLE, EXPLOSION, SL_2S, PRI_GAME_QUICK5);
	font_render_string_center (&font_v5prc, DMD_MIDDLE_X, DMD_BIG_CY_Top, "BREAKOUT");
	sprintf_score (prison_break_mode_last_score);
	font_render_string_center (&font_term6, DMD_MIDDLE_X, DMD_BIG_CY_Bot, sprintf_buffer);
	dmd_copy_low_to_high ();
	dmd_show_low ();
	dmd_invert_page (dmd_low_buffer);
	deff_swap_low_high (10, TIME_100MS);
	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
}//end of mode_effect_deff




void prison_break_effect_deff(void) {
	for (;;) {
		dmd_alloc_pair_clean();
		sprintf("BREAKOUT");
		sprintf_buffer[1 + (prison_break_mode_shots_made * 2)] = '\0';
		font_render_string_center (&font_v5prc, DMD_MIDDLE_X, DMD_BIG_CY_Top, sprintf_buffer);
		sprintf ("%d SEC LEFT,  %d HIT", prison_break_mode_timer, prison_break_mode_shots_made);
		font_render_string_center (&font_var5, DMD_MIDDLE_X, DMD_SMALL_CY_3, sprintf_buffer);
		sprintf_score (prison_break_mode_score);
		font_render_string_center (&font_var5, DMD_MIDDLE_X, DMD_SMALL_CY_4, sprintf_buffer);
		dmd_show_low ();
		task_sleep (TIME_500MS);
	}//END OF ENDLESS LOOP
}//end of mode_effect_deff





void prison_break_end_effect_deff(void) {
	dmd_alloc_pair_clean();
	font_render_string_center (&font_v5prc, DMD_MIDDLE_X, DMD_BIG_CY_Top, "BROKE OUT");
	sprintf_score (prison_break_mode_score);
	font_render_string_center (&font_v5prc, DMD_MIDDLE_X, DMD_BIG_CY_Bot, sprintf_buffer);
	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
	}//end of mode_effect_deff
