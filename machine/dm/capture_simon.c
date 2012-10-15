/*
 * demolition man
 * capture_simon.c
 *
 * written by James Cardona
 *
 * Location Description:
 * mode started by claw switch capture_simon
 *
 * Scoring Description: (original game)
 * The ball is dropped down a tunnel and served to the
 * right ramp just like the Underground. The seven
 * major shots are lit for 5 mil each for three shots.
 * A 25 million award for completing all three shots for a total of 40 million

 * Scoring Description: (my rules)
 * same as above except:
 * major shots are lit for 15 mil each for three shots.
 * A 50 million award for completing all three shots for a total of 95 million.
 *
 * estimate of average capture simon mode score: 30 million to 95 million
 *
 */
/* CALLSET_SECTION (capture_simon, __machine3__) */

#include <freewpc.h>
#include "dm/global_constants.h"
#include "clawmagnet.h"
#include <diverter.h> //autogenerated by divhold.ct
#include "capture_simon.h"

//constants
const U8 			CAP_SIM_EASY_GOAL 	= 3;
const U8 			CAP_SIM_MED_GOAL 	= 4;
const U8 			CAP_SIM_HARD_GOAL 	= 5;
const U8 			CAP_SIM_GOAL_INCREMENT = 1;
const U8 			CAP_SIM_GOAL_MAX 	= 10;

//local variables
U8			capture_simon_SoundCounter;
U8 			capture_simon_mode_shots_made;
U8 			capture_simon_mode_shots_goal;
__local__ U8 			capture_simon_modes_achieved;
__local__ U8 			capture_simon_modes_completed;
U8			capture_simon_mode_timer;
score_t 	capture_simon_mode_score;
score_t 	capture_simon_mode_last_score;
score_t 	capture_simon_mode_next_score;
score_t 	capture_simon_mode_score_total_score;


//external variables

//internally called function prototypes  --external found at protos.h
void capture_simon_reset (void);
void capture_simon_player_reset (void);
void capture_simon_effect_deff(void);
void capture_simon_mode_init (void);
void capture_simon_mode_expire (void);
void capture_simon_mode_exit (void);
void cap_simon_choose_random_flag_set(void);

/****************************************************************************
 * mode definition structure
 ***************************************************************************/
struct timed_mode_ops capture_simon_mode = {
	DEFAULT_MODE,
	.init = capture_simon_mode_init,
	.exit = capture_simon_mode_exit,
	.gid = GID_CAPTURE_SIMON_MODE_RUNNING,
	.music = MUS_MD_CAPTURE_SIMON,
	.deff_starting = DEFF_CAPTURE_SIMON_START_EFFECT,
	.deff_running = DEFF_CAPTURE_SIMON_EFFECT,
//	.deff_ending = DEFF_CAPTURE_SIMON_END_EFFECT,
	.prio = PRI_GAME_MODE4,
	.init_timer = 23,
	.timer = &capture_simon_mode_timer,
	.grace_timer = 2,
//	.pause = system_timer_pause,
};



/****************************************************************************
 * initialize  and exit
 ***************************************************************************/
void capture_simon_reset (void) {
	flag_off (FLAG_IS_CAPSIM_SIDERAMP_ACTIVATED);
	flag_off (FLAG_IS_CAPSIM_LEFTRAMP_ACTIVATED);
	flag_off (FLAG_IS_CAPSIM_RIGHTRAMP_ACTIVATED);
	flag_off (FLAG_IS_CAPSIM_UNDER_ACTIVATED);
	flag_off (FLAG_IS_CAPSIM_CENTERRAMP_ACTIVATED);
	flag_off (FLAG_IS_CAPSIM_LEFTORB_ACTIVATED);
	flag_off (FLAG_IS_CAPSIM_RIGHTORB_ACTIVATED);
}//end of function


void capture_simon_player_reset (void) {
	capture_simon_reset();
	capture_simon_modes_achieved = 0;
	capture_simon_modes_completed = 0;//these need to be zeroed in before we enter the mode so bonus doesn't fake trigger
	capture_simon_mode_shots_made = 0;
	capture_simon_mode_shots_goal = CAP_SIM_EASY_GOAL;
	score_zero(capture_simon_mode_score_total_score);
	capture_simon_SoundCounter = 0;
}//end of function



void capture_simon_mode_init (void) {
			//the claw mode can expire on its own and since it is a lower priority it will not display
			//callset_invoke (end_claw_mode); // this seemed to cause occasional crashes
			clawmagnet_off ();
			flag_off(FLAG_IS_BALL_ON_CLAW);
			flipper_enable ();
	sound_start (ST_SPEECH, SPCH_UNDER_ARREST, SL_3S, PRI_GAME_QUICK1);
	capture_simon_mode_shots_made = 0;
	++capture_simon_modes_achieved;
	//flash lamp for a time
	lamp_tristate_flash(LM_CLAW_CAPTURE_SIMON);
	task_sleep(TIME_500MS);
	lamp_tristate_on(LM_CLAW_CAPTURE_SIMON);
	score_zero(capture_simon_mode_score);
	score_zero(capture_simon_mode_last_score);
	score_zero(capture_simon_mode_next_score);
	switch (capture_simon_modes_achieved ){
		case 1: score_add(capture_simon_mode_next_score, score_table[SC_15M]); break;
		case 2: score_add(capture_simon_mode_next_score, score_table[SC_20M]); break;
		case 3: score_add(capture_simon_mode_next_score, score_table[SC_25M]); break;
		default: score_add(capture_simon_mode_next_score, score_table[SC_25M]);
	}//end of switch
	//this pause is here to give sounds time to finish
	task_sleep(TIME_500MS);
	task_sleep(TIME_500MS);
	task_sleep(TIME_500MS);
	sound_start (ST_SPEECH, SPCH_SO_SCARED, SL_4S, PRI_GAME_QUICK1);
	cap_simon_choose_random_flag_set();
	all_arrow_update();
	diverter_stop();//defined in divhold2.ct
}//end of function




void capture_simon_mode_expire (void) {
	flag_off (FLAG_IS_CAPSIM_SIDERAMP_ACTIVATED);
	flag_off (FLAG_IS_CAPSIM_LEFTRAMP_ACTIVATED);
	flag_off (FLAG_IS_CAPSIM_RIGHTRAMP_ACTIVATED);
	flag_off (FLAG_IS_CAPSIM_UNDER_ACTIVATED);
	flag_off (FLAG_IS_CAPSIM_CENTERRAMP_ACTIVATED);
	flag_off (FLAG_IS_CAPSIM_LEFTORB_ACTIVATED);
	flag_off (FLAG_IS_CAPSIM_RIGHTORB_ACTIVATED);
	all_arrow_update();
	if (flag_test(FLAG_IS_R_RAMP_CLAWREADY) ) 	rramp_clawready_on();
	else										rramp_clawready_off();
	capture_simon_mode_shots_goal += CAP_SIM_GOAL_INCREMENT;//increment goal for next time
}//end of function


void capture_simon_mode_exit (void) { capture_simon_mode_expire();}


/****************************************************************************
 * external event listeners
 ****************************************************************************/
CALLSET_ENTRY (capture_simon, music_refresh)  	{ timed_mode_music_refresh (&capture_simon_mode); }
CALLSET_ENTRY (capture_simon, end_ball) 		{ if (timed_mode_running_p(&capture_simon_mode) ) timed_mode_end (&capture_simon_mode); }
CALLSET_ENTRY (capture_simon, display_update) 	{ timed_mode_display_update (&capture_simon_mode); }

CALLSET_ENTRY (capture_simon, start_player) 	{ capture_simon_player_reset(); }
CALLSET_ENTRY (capture_simon, start_ball) 		{ capture_simon_reset(); }




/****************************************************************************
 * body
 *
 ***************************************************************************/
CALLSET_ENTRY (capture_simon, sw_claw_capture_simon) {
	timed_mode_begin (&capture_simon_mode);//start mode
}//end of function



 //center ramp shot made during capture_simon mode
void capture_simon_made(void) {
	++capture_simon_mode_shots_made;
	score_zero(capture_simon_mode_last_score);
	switch (capture_simon_modes_achieved ){
		case 1:
			score (SC_5M);
			score_add(capture_simon_mode_last_score, score_table[SC_5M]);
			break;
		case 2:
			//2nd time we are in capture_simon - score differently
			score (SC_10M);
			score_add(capture_simon_mode_last_score, score_table[SC_10M]);
			break;
		case 3:
			//3rd time we are in capture_simon - score differently
			score (SC_15M);
			score_add(capture_simon_mode_last_score, score_table[SC_15M]);
			break;
		default:
			//all cases past 3rd time we are in capture_simon
			score (SC_15M);
			score_add(capture_simon_mode_last_score, score_table[SC_15M]);
			break;
	}//end of switch
	score_add (capture_simon_mode_score, capture_simon_mode_last_score);
	score_add (capture_simon_mode_score_total_score, capture_simon_mode_last_score);

	if (capture_simon_mode_shots_made < capture_simon_mode_shots_goal ) {
			cap_simon_choose_random_flag_set();
			all_arrow_update();
			deff_start (DEFF_CAPTURE_SIMON_HIT_EFFECT);

	}	//IF FINAL CAPTURE SIMON SHOT MADE
	else {
		score (SC_25M);
		deff_start (DEFF_CAPTURE_SIMON_END_EFFECT);
		capture_simon_mode_exit();
		}
}//end of function


/****************************************************************************
 * DMD display and sound effects
 ****************************************************************************/
void capture_simon_start_effect_deff(void) {
	dmd_alloc_low_clean ();
	font_render_string_center (&font_bitoutline, DMD_MIDDLE_X, DMD_BIG_CY_Top, "CAPTURE");
	font_render_string_center (&font_bitoutline, DMD_MIDDLE_X, DMD_BIG_CY_Bot, "SIMON");
	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
}//end of mode_effect_deff



void capture_simon_hit_effect_deff(void) {
	dmd_alloc_low_clean ();
	//
	//	DRAW HAND CUFFS
	//
	capture_simon_mode_shots_made++; //for testing only

	if (capture_simon_mode_shots_made % 3 == 0)		{
		sound_start (ST_SPEECH, SPCH_WES_LAUGH1, SL_4S, PRI_GAME_QUICK5);
		bitmap_blit (cuffs_1_bits, 0, 0);
	}
	if (capture_simon_mode_shots_made % 3 == 2)	{
		sound_start (ST_SPEECH, SPCH_WES_LAUGH2, SL_4S, PRI_GAME_QUICK5);
		bitmap_blit (cuffs_3_bits, 0, 10);
	}
	if (capture_simon_mode_shots_made % 3 == 1)	{
		sound_start (ST_SPEECH, SPCH_WES_LAUGH3, SL_4S, PRI_GAME_QUICK5);
		bitmap_blit (cuffs_2_bits, 80, 0);
		font_render_string_center (&font_bitoutline, 40, DMD_BIG_CY_Top, "CAUGHT");
		sprintf_score (capture_simon_mode_last_score);
		font_render_string_center (&font_bitoutline, 40, DMD_BIG_CY_Bot, sprintf_buffer);
	}
	else {
		font_render_string_center (&font_bitoutline, 88, DMD_BIG_CY_Top, "CAUGHT");
		sprintf_score (capture_simon_mode_last_score);
		font_render_string_center (&font_bitoutline, 88, DMD_BIG_CY_Bot, sprintf_buffer);
	}
	dmd_show_low ();
	task_sleep_sec (2);

	//
	//	DRAW FOOT STEPS RUNNING AWAY
	//
	if (	capture_simon_mode_shots_made % 3 == 0
		||	capture_simon_mode_shots_made % 3 == 2) {
				dmd_clean_page_low ();
				bitmap_blit (footstep_left_2_bits, 104, 18);
				dmd_show_low ();
				sound_start (ST_EFFECT, TOINK1, SL_1S, PRI_GAME_QUICK5);
				task_sleep (TIME_200MS);
				bitmap_blit (footstep_left_1_bits, 76, 6);
				sound_start (ST_EFFECT, TOINK2, SL_1S, PRI_GAME_QUICK5);
				dmd_show_low ();
				task_sleep (TIME_200MS);
						bitmap_blit (footstep_left_2_bits, 48, 18);
						dmd_show_low ();
						sound_start (ST_EFFECT, TOINK1, SL_1S, PRI_GAME_QUICK5);
						task_sleep (TIME_200MS);
						bitmap_blit (footstep_left_1_bits, 20, 6);
						sound_start (ST_EFFECT, TOINK2, SL_1S, PRI_GAME_QUICK5);
						task_sleep (TIME_200MS);
						dmd_show_low ();
				task_sleep_sec (1);
	}
	else {
		dmd_clean_page_low ();
		bitmap_blit (footstep_right_2_bits, 0, 18);
		dmd_show_low ();
		sound_start (ST_EFFECT, TOINK1, SL_1S, PRI_GAME_QUICK5);
		task_sleep (TIME_200MS);
		bitmap_blit (footstep_right_1_bits, 28, 6);
		sound_start (ST_EFFECT, TOINK2, SL_1S, PRI_GAME_QUICK5);
		dmd_show_low ();
		task_sleep (TIME_200MS);
				bitmap_blit (footstep_right_2_bits, 56, 18);
				dmd_show_low ();
				sound_start (ST_EFFECT, TOINK1, SL_1S, PRI_GAME_QUICK5);
				task_sleep (TIME_200MS);
				bitmap_blit (footstep_right_1_bits, 84, 6);
				sound_start (ST_EFFECT, TOINK2, SL_1S, PRI_GAME_QUICK5);
				dmd_show_low ();
				task_sleep (TIME_200MS);
		task_sleep_sec (1);
}//end of else
	deff_exit ();
}//end of mode_effect_deff



void capture_simon_effect_deff(void) {
	U8 i = 0;
	__boolean TOGGLE = FALSE;
	for (;;) {
		dmd_alloc_low_clean ();
//		dmd_sched_transition (&trans_scroll_right);
		if (++i % 5 == 0) {//change TOGGLE once per second
				if (TOGGLE) {
					TOGGLE = FALSE;
					font_render_string_center (&font_bitoutline, DMD_MIDDLE_X, DMD_BIG_CY_Top, "SIMON");
				}//end of inner if
				else {
					TOGGLE = TRUE;
					font_render_string_center (&font_bitoutline, DMD_MIDDLE_X, DMD_BIG_CY_Top, "CAPTURE");
				}//end of else
		}//end of if (++i % 5 == 0)
		sprintf ("%d SEC LEFT,  %d HIT", capture_simon_mode_timer, capture_simon_mode_shots_made);
		font_render_string_center (&font_var5, DMD_MIDDLE_X, DMD_SMALL_CY_4, sprintf_buffer);
		sprintf_score (capture_simon_mode_next_score);
		font_render_string_center (&font_var5, DMD_MIDDLE_X, DMD_SMALL_CY_5, sprintf_buffer);
		dmd_show_low ();
		task_sleep (TIME_200MS);
	}//END OF ENDLESS LOOP
}//end of mode_effect_deff



void capture_simon_end_effect_deff(void) {
	dmd_alloc_low_clean ();
	font_render_string_center (&font_bitoutline, DMD_MIDDLE_X, DMD_BIG_CY_Top, "SIMON");
	sprintf("CAUGHT!!");
	font_render_string_center (&font_bitoutline, DMD_MIDDLE_X, DMD_BIG_CY_Bot, sprintf_buffer);
	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
	}//end of mode_effect_deff



/****************************************************************************
 *
 * randomize flag chosen
 *
 * we are using a very stupid and in-elegant
 * way of randomness - basically, we have a number
 * of predefined sets of flags and we randomly
 * pick one of those sets
 ****************************************************************************/
void cap_simon_choose_random_flag_set(void) {
	U8 random_chooser;
	random_chooser = random_scaled(5);
	switch (random_chooser) {
	case 0:
		flag_on 	(FLAG_IS_CAPSIM_SIDERAMP_ACTIVATED);
		flag_on 	(FLAG_IS_CAPSIM_LEFTRAMP_ACTIVATED);
		flag_off 	(FLAG_IS_CAPSIM_RIGHTRAMP_ACTIVATED);
		flag_off 	(FLAG_IS_CAPSIM_UNDER_ACTIVATED);
		flag_off 	(FLAG_IS_CAPSIM_CENTERRAMP_ACTIVATED);
		flag_on 	(FLAG_IS_CAPSIM_LEFTORB_ACTIVATED);
		flag_off 	(FLAG_IS_CAPSIM_RIGHTORB_ACTIVATED);
		break;
	case 1:
		flag_on 	(FLAG_IS_CAPSIM_SIDERAMP_ACTIVATED);
		flag_on 	(FLAG_IS_CAPSIM_LEFTRAMP_ACTIVATED);
		flag_off 	(FLAG_IS_CAPSIM_RIGHTRAMP_ACTIVATED);
		flag_off 	(FLAG_IS_CAPSIM_UNDER_ACTIVATED);
		flag_off 	(FLAG_IS_CAPSIM_CENTERRAMP_ACTIVATED);
		flag_off 	(FLAG_IS_CAPSIM_LEFTORB_ACTIVATED);
		flag_on 	(FLAG_IS_CAPSIM_RIGHTORB_ACTIVATED);
		break;
	case 2:
		flag_off 	(FLAG_IS_CAPSIM_SIDERAMP_ACTIVATED);
		flag_off 	(FLAG_IS_CAPSIM_LEFTRAMP_ACTIVATED);
		flag_on 	(FLAG_IS_CAPSIM_RIGHTRAMP_ACTIVATED);
		flag_on 	(FLAG_IS_CAPSIM_UNDER_ACTIVATED);
		flag_on 	(FLAG_IS_CAPSIM_CENTERRAMP_ACTIVATED);
		flag_off 	(FLAG_IS_CAPSIM_LEFTORB_ACTIVATED);
		flag_off 	(FLAG_IS_CAPSIM_RIGHTORB_ACTIVATED);
		break;
	case 3:
		flag_off 	(FLAG_IS_CAPSIM_SIDERAMP_ACTIVATED);
		flag_off	(FLAG_IS_CAPSIM_LEFTRAMP_ACTIVATED);
		flag_on 	(FLAG_IS_CAPSIM_RIGHTRAMP_ACTIVATED);
		flag_on 	(FLAG_IS_CAPSIM_UNDER_ACTIVATED);
		flag_off 	(FLAG_IS_CAPSIM_CENTERRAMP_ACTIVATED);
		flag_off 	(FLAG_IS_CAPSIM_LEFTORB_ACTIVATED);
		flag_on 	(FLAG_IS_CAPSIM_RIGHTORB_ACTIVATED);
		break;
	case 4:
		flag_on 	(FLAG_IS_CAPSIM_SIDERAMP_ACTIVATED);
		flag_off 	(FLAG_IS_CAPSIM_LEFTRAMP_ACTIVATED);
		flag_off 	(FLAG_IS_CAPSIM_RIGHTRAMP_ACTIVATED);
		flag_off 	(FLAG_IS_CAPSIM_UNDER_ACTIVATED);
		flag_on 	(FLAG_IS_CAPSIM_CENTERRAMP_ACTIVATED);
		flag_on 	(FLAG_IS_CAPSIM_LEFTORB_ACTIVATED);
		flag_off 	(FLAG_IS_CAPSIM_RIGHTORB_ACTIVATED);
		break;
	}//end of switch
}//end of function

