/*
 * demolition man
 * acmag.c
 *
 * written by James Cardona
 *
 * Location Description:
 * mode started by claw switch acmag -points scored by center ramp.
 *
 * Scoring Description: (original game)
 * counts up as time goes down from 5 million to 12.5 million, typically
 * about a 20 second mode.  Only shot that scores is the very wide
 * center ramp
 *
 * Scoring Description: (my rules)
 * same as above except:
 * scoring is higher to balance with other modes
 * 2nd and 3rd acmag mode score higher to encourage doing mode more than once
 *
 * estimate of average acmag mode score: 60 million to 125 million
 *
 *
 *
 */
/* CALLSET_SECTION (acmag, __machine__) */



#include <freewpc.h>
#include "dm/global_constants.h"
#include "clawmagnet.h"

//constants

//local variables
U8 						acmag_mode_shots_made;
__local__ U8 			acmag_modes_achieved;
U8			acmag_mode_timer;
score_t 	acmag_mode_score;
score_t 	acmag_mode_next_score;
score_t 	acmag_mode_score_total_score;

//external variables

//internally called function prototypes  --external found at protos.h
void acmag_reset (void);
void acmag_player_reset (void);
void acmag_effect_deff(void);
void acmag_mode_init (void);
void acmag_mode_expire (void);
void acmag_mode_exit (void);
void star_draw (void);
void show_text_on_stars (U8 times);


/****************************************************************************
 * mode definition structure
 ***************************************************************************/
struct timed_mode_ops acmag_mode = {
	DEFAULT_MODE,
	.init = acmag_mode_init,
	.exit = acmag_mode_exit,
	.gid = GID_ACMAG_MODE_RUNNING,
	.music = MUS_ACMAG,
	.deff_starting = DEFF_ACMAG_START_EFFECT,
	.deff_running = DEFF_ACMAG_EFFECT,
//	.deff_ending = DEFF_ACMAG_END_EFFECT,
	.prio = PRI_GAME_MODE2,
	.init_timer = 33,
	.timer = &acmag_mode_timer,
	.grace_timer = 2, //default is 2
//	.pause = system_timer_pause,
};



/****************************************************************************
 * initialize  and exit
 ***************************************************************************/
void acmag_reset (void) {
	flag_off (FLAG_IS_ACMAG_RUNNING);
}//end of function



void acmag_player_reset (void) {
	acmag_reset();
	acmag_modes_achieved = 0;
	acmag_mode_shots_made = 0;
	score_zero(acmag_mode_score_total_score);
}//end of function



void acmag_mode_init (void) {
			//the claw mode can expire on its own and since it is a lower priority it will not display
			//callset_invoke (end_claw_mode); // this seemed to cause occasional crashes
			clawmagnet_off ();
			flag_off(FLAG_IS_BALL_ON_CLAW);
			flipper_enable ();
			ballsave_add_time (10);
	acmag_mode_shots_made = 0;
	flag_on (FLAG_IS_ACMAG_RUNNING);
	center_ramp_arrow_update();
	++acmag_modes_achieved;
	sound_start (ST_SPEECH, SPCH_ACMAG_ACTIVATED, SL_4S, PRI_GAME_QUICK5);
	//flash lamp for a time
	lamp_tristate_flash(LM_CLAW_ACMAG);
	task_sleep(TIME_500MS);
	lamp_tristate_on(LM_CLAW_ACMAG);
	score_zero(acmag_mode_score);
	score_zero(acmag_mode_next_score);
	switch (acmag_modes_achieved ){
		case 1: score_add(acmag_mode_next_score, score_table[ACMAG_HIT_SCORE_1]); break;
		case 2: score_add(acmag_mode_next_score, score_table[ACMAG_HIT_SCORE_2]); break;
		default:
		case 3: score_add(acmag_mode_next_score, score_table[ACMAG_HIT_SCORE_3]); break;
	}//end of switch
}//end of function



void acmag_mode_expire (void) {
	flag_off (FLAG_IS_ACMAG_RUNNING);
	center_ramp_arrow_update();
}//end of function


void acmag_mode_exit (void) { acmag_mode_expire();}


/****************************************************************************
 * external event listeners
 ****************************************************************************/
CALLSET_ENTRY (acmag, music_refresh)  	{ timed_mode_music_refresh (&acmag_mode); }
CALLSET_ENTRY (acmag, end_ball) 		{ if (timed_mode_running_p(&acmag_mode) ) timed_mode_end (&acmag_mode); }
CALLSET_ENTRY (acmag, display_update) 	{ timed_mode_display_update (&acmag_mode); }

CALLSET_ENTRY (acmag, start_player) 	{ acmag_player_reset(); }
CALLSET_ENTRY (acmag, start_ball) 		{ acmag_reset(); }




/****************************************************************************
 *
 * body
 *
 ***************************************************************************/
CALLSET_ENTRY (acmag, sw_claw_acmag) {
	demotime_increment();
	timed_mode_begin (&acmag_mode);//start mode
}//end of function


 //center ramp shot made during acmag mode
void acmag_made(void) {
	++acmag_mode_shots_made;
	sound_start (ST_SAMPLE, EXPLOSION, SL_2S, PRI_GAME_QUICK5);
	//flash lamp for a time
	lamp_tristate_flash(LM_CENTER_RAMP_MIDDLE);
	lamp_tristate_flash(LM_CENTER_RAMP_OUTER);
	lamp_tristate_flash(LM_CENTER_RAMP_INNER);
	task_sleep(TIME_1S);
	lamp_tristate_off(LM_CENTER_RAMP_MIDDLE);
	lamp_tristate_off(LM_CENTER_RAMP_OUTER);
	lamp_tristate_off(LM_CENTER_RAMP_INNER);

	switch (acmag_modes_achieved ){
		case 1:
			score (ACMAG_HIT_SCORE_1);
			score_add (acmag_mode_score, score_table[ACMAG_HIT_SCORE_1]);
			score_add (acmag_mode_score_total_score, score_table[ACMAG_HIT_SCORE_1]);
			break;
		case 2:
			//2nd time we are in acmag - score differently
			score (ACMAG_HIT_SCORE_2);
			score_add (acmag_mode_score, score_table[ACMAG_HIT_SCORE_2]);
			score_add (acmag_mode_score_total_score, score_table[ACMAG_HIT_SCORE_2]);
			break;
		default:
		case 3:
			//3rd time we are in acmag - score differently
			score (ACMAG_HIT_SCORE_3);
			score_add (acmag_mode_score, score_table[ACMAG_HIT_SCORE_3]);
			score_add (acmag_mode_score_total_score, score_table[ACMAG_HIT_SCORE_3]);
			break;
	}//end of switch
	deff_start (DEFF_ACMAG_HIT_EFFECT);//under /kernel/deff.c
}//end of function



/****************************************************************************
 *
 * DMD display and sound effects
 *
 ****************************************************************************/
void acmag_start_effect_deff(void) {
	dmd_map_overlay ();
	dmd_clean_page_high ();
	dmd_clean_page_low ();
	dmd_draw_thin_border (dmd_low_buffer);
	font_render_string_center (&font_steel, DMD_MIDDLE_X, DMD_BIG_CY_Top, "ACMAG");
	font_render_string_center (&font_term6, DMD_MIDDLE_X, DMD_BIG_CY_Bot, "CENTER RAMP");
	show_text_on_stars (40); //about 4 seconds
	deff_exit ();
}//end of mode_effect_deff



void acmag_hit_effect_deff(void) {
	dmd_map_overlay ();
	dmd_clean_page_high ();
	dmd_clean_page_low ();
	dmd_draw_thin_border (dmd_low_buffer);
//	font_render_string_center (&font_steel, DMD_MIDDLE_X, DMD_BIG_CY_Top, "ACMAG");
	sprintf_score (acmag_mode_next_score);
	font_render_string_center (&font_fixed10, DMD_MIDDLE_X, DMD_BIG_CY_Cent, sprintf_buffer);
	show_text_on_stars (20);
	deff_exit ();
}//end of mode_effect_deff



void acmag_effect_deff(void) {
	U8 i = 0;
	U8 j = 0;
	__boolean TOGGLE = FALSE;
	__boolean TOGGLE_BOTTOM = FALSE;

	for (;;) {
		dmd_map_overlay ();
		dmd_clean_page_high ();
		dmd_clean_page_low ();
		dmd_draw_thin_border (dmd_low_buffer);

		sprintf_score(current_score);
		font_render_string_center (&font_var5, DMD_MIDDLE_X, DMD_SMALL_CY_1, sprintf_buffer);

		font_render_string_center (&font_steel, DMD_MIDDLE_X, DMD_BIG_CY_Top + 2, "ACMAG");

		if (++i % 3 == 0) { if (TOGGLE) TOGGLE = FALSE; else TOGGLE = TRUE; }//change TOGGLE once per n seconds
		if (++j % 6 == 0) { if (TOGGLE_BOTTOM) TOGGLE_BOTTOM = FALSE; else TOGGLE_BOTTOM = TRUE; }//change TOGGLE once per n seconds

		if (i % 20 != 0) { //draw for 4/5 and blank for 1/5
					if (TOGGLE) {
						sprintf ("%d SEC", acmag_mode_timer);
						font_render_string_center (&font_var5, DMD_MIDDLE_X, DMD_SMALL_CY_3 + 3, sprintf_buffer);
							}//end of if (TOGGLE)
					else {
						sprintf ("%d HIT", acmag_mode_shots_made);
						font_render_string_center (&font_var5, DMD_MIDDLE_X, DMD_SMALL_CY_3 + 3, sprintf_buffer);
						}//end of else
		}//end of if (i % 5 != 0)

		if (TOGGLE_BOTTOM)	font_render_string_center (&font_term6, DMD_MIDDLE_X, DMD_MED_CY_3, "SHOOT CENTER RAMP");
		else			{	sprintf_score (acmag_mode_next_score);
							font_render_string_center (&font_term6, DMD_MIDDLE_X, DMD_MED_CY_3 + 2, sprintf_buffer); }

		show_text_on_stars (8); //about 800 ms
	}//END OF ENDLESS LOOP
}//end of mode_effect_deff



#define MAX_STARS 12
#define MAX_STATE 4
struct star_state {
	U8 time;
	U8 state;
	U8 x;
	U8 y;
} star_states[MAX_STARS];

static const U8 star_bitmaps[] = {
	3, 3, 0, 0, 0,
	3, 3, 0, 2, 0,
	3, 3, 0, 2, 0,
	3, 3, 2, 5, 2,
	3, 3, 2, 7, 2,
	3, 3, 2, 7, 2,
};

void star_draw (void) {
	U8 n;
	for (n = 0; n < MAX_STARS; n++) {
		struct star_state *s = &star_states[n];
		if (s->time) {
			//bitmap_erase_asm (...);
			bitmap_blit2 (star_bitmaps + s->state * 5, s->x, s->y);

			s->time--;

			if (random () < 128)
				;
			if (s->state == MAX_STATE)  s->state--;
			else if (s->state == 0)		s->state++;
			else if (random () < 192)	s->state++;
			else						s->state--;
		}//end of if s->time
		else {
			if (random () < 64) {
				s->time = 4 + random_scaled (8);
				s->x = 4 + random_scaled (120);
				s->y = 2 + random_scaled (24);
				s->state = 0;
			}//end of if (random () < 64
		}//end of else
	}//end of for loop
}//end of function



void show_text_on_stars (U8 times) {
	U8 n;
	for (n = 0; n < times; n++) {
		dmd_dup_mapped ();//allocate new space but make it a copy of what is on DMD now
		dmd_overlay_onto_color ();//mono overlay onto current color page
		star_draw ();
		dmd_show2 ();//shows a 4 color image
		task_sleep (TIME_100MS);
		dmd_map_overlay ();/** Map a consecutive display page pair into windows 0 & 1 */
	}
	dmd_alloc_pair_clean ();
}

