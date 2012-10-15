/*
 * demolition man
 * museum.c
 * 
 * written by James Cardona
 *
 * handles the lock freezes and the trigger of multiballs
 *
 *
 * */
/* CALLSET_SECTION (museum, __machine3__) */


#include <freewpc.h>
#include "dm/global_constants.h"

//local variables
score_t		museum_score;
U8			museum_jackpot_shots_made;
U8			museum_MessageCounter;
__boolean	museum_start_music;

//external variables
extern U8			NumBallsFrozen; //from lock_freeze_mbstart.c

//internally called function prototypes  --external found at protos.h
void museum_player_reset (void);
void museum_start_effect_deff(void);
void museum_jackpot_effect1_deff(void);
void museum_jackpot_effect2_deff(void);
void museum_jackpot_effect3_deff(void);
void museum_jackpot_effect(void);
void museum_effect_deff (void);

/****************************************************************************
 * multiball definition structure
 ***************************************************************************/
struct mb_mode_ops museum_mode = {
	DEFAULT_MBMODE,
	//.update = , /* The update callback is invoked whenever the state of the multiball changes. */
	.music = MUS_MB,
	.deff_starting = DEFF_MUSEUM_START_EFFECT,
	.deff_running = DEFF_MUSEUM_EFFECT,
	//.deff_ending = ,
//.active_task = 				//	default => .active_task = mb_mode_active_task,
	.prio = PRI_MULTIBALL,		//default => .prio = PRI_NULL,
//.grace_period = 				//default => .grace_period = 500ms
};
//task_gid_t gid_running;
//task_gid_t gid_in_grace;


/****************************************************************************
 * initialize  and exit
 ***************************************************************************/
void museum_player_reset (void) {
	flag_off(FLAG_IS_MUSEUM_ACTIVATED);
	score_zero(museum_score);
	museum_jackpot_shots_made = 0; //these need to be zeroed in before we enter the mode so bonus doesn't fake trigger
	museum_start_music = FALSE;
}//end of function



CALLSET_ENTRY (museum, start_player) { museum_player_reset(); }

/****************************************************************************
 * external event listeners
 ****************************************************************************/
CALLSET_ENTRY (museum, music_refresh)  {
	if (museum_start_music)				music_request (HELICOPTER, PRI_GAME_QUICK8);
	else 									mb_mode_music_refresh (&museum_mode);
}//end of function

CALLSET_ENTRY (museum, display_update) { mb_mode_display_update (&museum_mode); }

CALLSET_ENTRY (museum, end_ball) {
	if (flag_test(FLAG_IS_MUSEUM_ACTIVATED)) {
		mb_mode_end_ball (&museum_mode);
		jackpot_reset();
	}
}//end of function

//puts in grace period if set
CALLSET_ENTRY (museum, single_ball_play) {
	if (flag_test(FLAG_IS_MUSEUM_ACTIVATED)) {
//		mb_mode_single_ball (&museum_mode);
		mb_mode_end_ball (&museum_mode);
		jackpot_reset();
	}
}//end of function

/****************************************************************************
 * body
 *
 ***************************************************************************/
void museum_start(void) {
	museum_start_music = TRUE; //for to play the helicopter instead of the music
	//LIGHTS
			lamp_tristate_flash(LM_MUSEUM_MULTIBALL);
			task_sleep (TIME_2S);
			lamp_tristate_on (LM_MUSEUM_MULTIBALL);
			lamp_tristate_off (LM_FREEZE_1);
			lamp_tristate_off (LM_FREEZE_2);
			lamp_tristate_off (LM_FREEZE_3);
			lamp_tristate_off (LM_FREEZE_4);
	//SOUNDS
			U8 	museum_SoundCounter;
			museum_SoundCounter = random_scaled(2);//from kernal/random.c - pick number from 0 to 2
			if (museum_SoundCounter == 0)
				sound_start (ST_SPEECH, SPCH_SOMETHING_RIGHT_PREV_LIFE, SL_4S, PRI_GAME_QUICK5);
			else
				sound_start (ST_SPEECH, SPCH_SEND_MANIAC, SL_4S, PRI_GAME_QUICK5);
	task_sleep (TIME_3S);
	museum_start_music = FALSE; //for to kill the music
	flag_on(FLAG_IS_MUSEUM_ACTIVATED);
	mb_mode_start(&museum_mode);

	choose_random_jackpot();//randomize which jackpot shot to make
	set_ball_count (2); //TODO: NumBallsFrozen  --1ST BALL RELEASED AT TOP_POPPER.C
	multiball_started();//reset all MB start criteria for next time
}//end of function



//jackpot shot
void museum_jackpot_made(void) {
	score_add (museum_score, score_table[SC_15M]);
	U8 	museum_SoundCounter;
	museum_SoundCounter = random_scaled(2);//from kernal/random.c - pick number from 0 to 2
	if (++museum_jackpot_shots_made % 2 == 0) {
			if (museum_SoundCounter == 0) 	sound_start (ST_SPEECH, SPCH_DOUBLE_JACKPOT_WES, SL_4S, PRI_GAME_QUICK5);
			else 								sound_start (ST_SPEECH, SPCH_DOUBLE_JACKPOT_SLY, SL_4S, PRI_GAME_QUICK5);
	}
	else {
			if (museum_SoundCounter == 0) 	sound_start (ST_SPEECH, SPCH_AHHHGGG, SL_4S, PRI_GAME_QUICK5);
			else 								sound_start (ST_SPEECH, SPCH_JACKPOT, SL_4S, PRI_GAME_QUICK5);
	}
	score (SC_15M);
	museum_jackpot_effect();//under /kernel/deff.c
}//end of function



/****************************************************************************
 * DMD display and sound effects
 ****************************************************************************/
void museum_start_effect_deff(void) {
		U8 count = 8;
		dmd_alloc_pair_clean ();
		font_render_string_center (&font_amiga4ever, DMD_MIDDLE_X, DMD_BIG_CY_Top, "MUSEUM");
		font_render_string_center (&font_amiga4ever, DMD_MIDDLE_X, DMD_BIG_CY_Bot, "MULTIBALL");
		/* low = text, high = blank */
		while (--count > 0) {
			dmd_show2 ();
			task_sleep (TIME_100MS);
					dmd_flip_low_high ();
					dmd_show2 ();
					task_sleep (TIME_100MS);
			dmd_show_high ();
			task_sleep (TIME_200MS);
					dmd_show2 ();
					task_sleep (TIME_100MS);
					dmd_flip_low_high ();
		}//END OF WHILE LOOP
		deff_exit ();
}//end of mode_effect_deff



void museum_jackpot_effect(void) {
	switch (++museum_MessageCounter % 3) {
		case 0:  deff_start (DEFF_MUSEUM_JACKPOT1_EFFECT); break;
		case 1:  deff_start (DEFF_MUSEUM_JACKPOT2_EFFECT); break;
		case 2:  deff_start (DEFF_MUSEUM_JACKPOT3_EFFECT); break;
		case 3:  deff_start (DEFF_MUSEUM_JACKPOT4_EFFECT); break;
		default: deff_start (DEFF_MUSEUM_JACKPOT1_EFFECT); break;
		}//end of switch
}//end of FUNCTION



void museum_jackpot1_effect_deff(void) {
	U8 i;
	for (i=1; i < 8; i++) {			//SPELL OUT LETTRS
			dmd_alloc_low_clean ();
			sprintf ("JACKPOT");
			if (i < 7)
				sprintf_buffer[i] = '\0';
			font_render_string_center (&font_amiga4ever, 64, 16, sprintf_buffer);
			dmd_show_low ();
			task_sleep (TIME_300MS);
	}
	task_sleep (TIME_500MS);
	deff_exit ();
}//end of mode_effect_deff



void museum_jackpot2_effect_deff(void) {
	U8 i;
	for (i=0; i < 8; i++) {
		dmd_sched_transition (&trans_scroll_up);
		dmd_alloc_low_clean ();
		font_render_string_center (&font_amiga4ever, 64, 16, "JACKPOT");
		dmd_show_low ();
	}
	task_sleep (TIME_500MS);
	deff_exit ();
}//end of mode_effect_deff



void museum_jackpot3_effect_deff(void) {
	U8 i;
	for (i=0; i < 8; i++) {
		dmd_sched_transition (&trans_scroll_up);
		dmd_alloc_low_clean ();
		font_render_string_center (&font_amiga4ever, 64, 8, "JACKPOT");
		font_render_string_center (&font_amiga4ever, 64, 24, "JACKPOT");
		dmd_show_low ();
	}
	task_sleep (TIME_500MS);
	deff_exit ();
}//end of mode_effect_deff



void museum_jackpot4_effect_deff(void) {
	dmd_alloc_pair ();
	dmd_clean_page_low ();
	font_render_string_center (&font_amiga4ever, 64, 16, "JACKPOT");
	dmd_copy_low_to_high ();
	dmd_show_low ();
	dmd_invert_page (dmd_low_buffer);
	deff_swap_low_high (25, TIME_100MS);
	task_sleep (TIME_500MS);
	deff_exit ();
}//end of mode_effect_deff



void museum_effect_deff (void) {
	for (;;) {
		dmd_alloc_low_clean ();
		font_render_string_center (&font_amiga4ever, DMD_MIDDLE_X, DMD_BIG_CY_Top, "MUSEUM");
		sprintf ("%d JACKPOTS", museum_jackpot_shots_made);
		font_render_string_center (&font_var5, DMD_MIDDLE_X, DMD_SMALL_CY_4, sprintf_buffer);
		dmd_show_low ();
		task_sleep (TIME_200MS);
	}//END OF ENDLESS LOOP
}//end of function
