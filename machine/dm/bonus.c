/*
 * bonus.c
 */
/* CALLSET_SECTION (bonus, __machine3__) */



#include <freewpc.h>
#include <eb.h>
#include <status.h>
#include "dm/global_constants.h"

//constants

//local variables
score_t total_bonus;	/* Total bonus score */
score_t bonus_scored;	/* Temp variable used to calculate bonus per item etc */
bool buttons_held;

//external variables
extern U8 		car_crash_shots_made; 				//from car_crash.c

extern U8 		car_chase_mode_shots_made;			//from car_chase.c
extern U8 		car_chase_modes_achieved;
extern U8 		car_chase_modes_completed;

extern U8 		capture_simon_mode_shots_made;		//from capture_simon.c
extern U8 		capture_simon_modes_achieved;
extern U8 		capture_simon_modes_completed;

extern U8 		prison_break_mode_shots_made;		//from prison_break.c
extern U8 		prison_break_modes_achieved;

extern U8 		acmag_mode_shots_made;				//from acmag.c
extern U8 		acmag_modes_achieved;

extern U8 		explode_mode_shots_made;			//from EXPLODE.c
extern score_t 	explode_mode_score;

extern U8 		superjets_modes_achieved;			//from superjets.c
extern U8 		superjets_mode_shots_made;

extern U8		standupFrenzy_modes_achieved;		//from standupfrenzy.c
extern U8 		standupFrenzyNumHits;
//

extern U8 		combo_counter;						//from combos.c
extern U8		fortress_jackpot_shots_made;

extern U8 		rollover_bonus_multiplier; 			//from rollovers.c

extern U8		fortress_jackpot_shots_made;
extern U8		demotime_jackpot_shots_made;
extern U8		wasteland_jackpot_shots_made;
extern U8		cryoprison_jackpot_shots_made;

extern U8		demotime_jackpot_shots_made;

extern U8 		huxley_mode_shots_made;

extern U8		back_in_the_fridge_shots_made;





//internally called function prototypes  --external found at protos.h
void bonus_button_monitor (void);
void bonus_sched_transition (void);
void bonus_pause (void);
bool check_for_big_score (void);
bool check_for_puny_score (void);

/****************************************************************************
 * body
 ****************************************************************************/

CALLSET_ENTRY (bonus, bonus){
	deff_start (DEFF_BONUS);
//	leff_start (LEFF_BONUS);
//	task_sleep_sec (1);
//	while (deff_get_active() == DEFF_BONUS)  task_sleep (TIME_500MS);
//	leff_stop (LEFF_BONUS);
}//end of function



void bonus_deff (void) {
	in_bonus = TRUE;
//	music_disable();
//			task_kill_gid (GID_MUSIC_REFRESH);
//			music_off ();

	task_sleep (TIME_100MS);	/* Wait a bit so the previous music_stop doesn't kill the sounds */
	sound_start1 (ST_ANY, BONUS_SHORT);
	score_zero (total_bonus);	/* Clear the bonus score */
	/* Show Initial bonus screen */
	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed10, DMD_MIDDLE_X, DMD_BIG_CY_Cent, "BONUS");
	dmd_sched_transition (&trans_scroll_down);
	dmd_show_low ();
	task_sleep_sec (1);

	task_recreate_gid (GID_BONUS_BUTTON_MONITOR, bonus_button_monitor); /* Start a task to monitor the buttons */

	/**
	 * *car crash scoring and display *
	 * **/
	if (car_crash_shots_made > 0) {
				sound_start1 (ST_ANY, BONUS_SHORT);
			dmd_alloc_low_clean ();
			score_zero (bonus_scored);
			score_add (bonus_scored, score_table[SC_100K]);
			score_mul (bonus_scored, car_crash_shots_made);
			score_add (total_bonus, bonus_scored);

			sprintf_score (bonus_scored);
			font_render_string_center (&font_fixed10, DMD_MIDDLE_X, DMD_BIG_CY_Cent, sprintf_buffer);
			sprintf ("CAR CRASH");
			font_render_string_center (&font_var5, DMD_MIDDLE_X, DMD_SMALL_CY_1, sprintf_buffer);
			bonus_sched_transition ();
			dmd_show_low ();
			bonus_pause ();
	}/***end of car crash scoring and display ***/



	/**
	 * *car chase scoring and display *
	 * **/
	if (car_chase_modes_achieved > 0) {
				sound_start1 (ST_ANY, BONUS_SHORT);
			dmd_alloc_low_clean ();
			score_zero (bonus_scored);
			score_add (bonus_scored, score_table[SC_100K]);
			score_mul (bonus_scored, car_chase_mode_shots_made);
			score_add (total_bonus, bonus_scored);

			score_zero (bonus_scored);
			score_add (bonus_scored, score_table[SC_1M]);
			score_mul (bonus_scored, car_chase_modes_achieved);
			score_add (total_bonus, bonus_scored);

			sprintf_score (bonus_scored);
			font_render_string_center (&font_fixed10, DMD_MIDDLE_X, DMD_BIG_CY_Cent, sprintf_buffer);
			sprintf ("CAR CHASE");
			font_render_string_center (&font_var5, DMD_MIDDLE_X, DMD_SMALL_CY_1, sprintf_buffer);
			bonus_sched_transition ();
			dmd_show_low ();
			bonus_pause ();
	}/***end of car chase scoring and display ***/



	/**
	 * *capture_simon scoring and display *
	 * **/
	if (capture_simon_modes_achieved > 0) {
				sound_start1 (ST_ANY, BONUS_SHORT);
			dmd_alloc_low_clean ();
			score_zero (bonus_scored);
			score_add (bonus_scored, score_table[SC_100K]);
			score_mul (bonus_scored, capture_simon_mode_shots_made);
			score_add (total_bonus, bonus_scored);

			score_zero (bonus_scored);
			score_add (bonus_scored, score_table[SC_1M]);
			score_mul (bonus_scored, capture_simon_modes_achieved);
			score_add (total_bonus, bonus_scored);

			score_zero (bonus_scored);
			score_add (bonus_scored, score_table[SC_5M]);
			score_mul (bonus_scored, capture_simon_modes_completed);
			score_add (total_bonus, bonus_scored);

			sprintf_score (bonus_scored);
			font_render_string_center (&font_fixed10, DMD_MIDDLE_X, DMD_BIG_CY_Cent, sprintf_buffer);
			sprintf ("CAPTURE SIMON");
			font_render_string_center (&font_var5, DMD_MIDDLE_X, DMD_SMALL_CY_1, sprintf_buffer);
			bonus_sched_transition ();
			dmd_show_low ();
			bonus_pause ();
	}/***end of capture_simon scoring and display ***/



	/**
	 * *prison_break scoring and display *
	 * **/
	if (prison_break_modes_achieved > 0) {
				sound_start1 (ST_ANY, BONUS_SHORT);
			dmd_alloc_low_clean ();
			score_zero (bonus_scored);
			score_add (bonus_scored, score_table[SC_100K]);
			score_mul (bonus_scored, prison_break_mode_shots_made);
			score_add (total_bonus, bonus_scored);

			score_zero (bonus_scored);
			score_add (bonus_scored, score_table[SC_1M]);
			score_mul (bonus_scored, prison_break_modes_achieved);
			score_add (total_bonus, bonus_scored);

			sprintf_score (bonus_scored);
			font_render_string_center (&font_fixed10, DMD_MIDDLE_X, DMD_BIG_CY_Cent, sprintf_buffer);
			sprintf ("BREAKOUT");
			font_render_string_center (&font_var5, DMD_MIDDLE_X, DMD_SMALL_CY_1, sprintf_buffer);
			bonus_sched_transition ();
			dmd_show_low ();
			bonus_pause ();
	}/***end of prison_break scoring and display ***/

	/**
	 * *acmag scoring and display *
	 * **/
	if (acmag_modes_achieved > 0) {
				sound_start1 (ST_ANY, BONUS_SHORT);
			dmd_alloc_low_clean ();
			score_zero (bonus_scored);
			score_add (bonus_scored, score_table[SC_100K]);
			score_mul (bonus_scored, acmag_mode_shots_made);
			score_add (total_bonus, bonus_scored);

			score_zero (bonus_scored);
			score_add (bonus_scored, score_table[SC_1M]);
			score_mul (bonus_scored, acmag_modes_achieved);
			score_add (total_bonus, bonus_scored);

			sprintf_score (bonus_scored);
			font_render_string_center (&font_fixed10, DMD_MIDDLE_X, DMD_BIG_CY_Cent, sprintf_buffer);
			sprintf ("ACMAG");
			font_render_string_center (&font_var5, DMD_MIDDLE_X, DMD_SMALL_CY_1, sprintf_buffer);
			bonus_sched_transition ();
			dmd_show_low ();
			bonus_pause ();
	}/***end of acmag scoring and display ***/

	/**
	 * *explode scoring and display *
	 * **/
	if (explode_mode_shots_made > 0) {
			sound_start1 (ST_ANY, BONUS_SHORT);
			dmd_alloc_low_clean ();
			score_zero (bonus_scored);
			score_add (bonus_scored, score_table[SC_100K]);
			score_mul (bonus_scored, explode_mode_shots_made);
			score_add (bonus_scored, score_table[SC_1M]);
			score_add (total_bonus, bonus_scored);

			sprintf_score (bonus_scored);
			font_render_string_center (&font_fixed10, DMD_MIDDLE_X, DMD_BIG_CY_Cent, sprintf_buffer);
			sprintf ("EXPLODE");
			font_render_string_center (&font_var5, DMD_MIDDLE_X, DMD_SMALL_CY_1, sprintf_buffer);
			bonus_sched_transition ();
			dmd_show_low ();
			bonus_pause ();
	}/***end of explode scoring and display ***/

	/**
	 * *superjets scoring and display *
	 * **/
	if (superjets_modes_achieved > 0) {
			sound_start1 (ST_ANY, BONUS_SHORT);
			dmd_alloc_low_clean ();
			score_zero (bonus_scored);
			score_add (bonus_scored, score_table[SC_100K]);
			score_mul (bonus_scored, superjets_mode_shots_made);
			score_add (total_bonus, bonus_scored);

			score_zero (bonus_scored);
			score_add (bonus_scored, score_table[SC_1M]);
			score_mul (bonus_scored, superjets_modes_achieved);
			score_add (total_bonus, bonus_scored);

			sprintf_score (bonus_scored);
			font_render_string_center (&font_fixed10, DMD_MIDDLE_X, DMD_BIG_CY_Cent, sprintf_buffer);
			sprintf ("SUPERJETS");
			font_render_string_center (&font_var5, DMD_MIDDLE_X, DMD_SMALL_CY_1, sprintf_buffer);
			bonus_sched_transition ();
			dmd_show_low ();
			bonus_pause ();
	}/***end of superjets scoring and display ***/



	/**
	 * *standupFrenzy scoring and display *
	 * **/
	if (standupFrenzy_modes_achieved > 0) {
			sound_start1 (ST_ANY, BONUS_SHORT);
			dmd_alloc_low_clean ();
			score_zero (bonus_scored);
			score_add (bonus_scored, score_table[SC_100K]);
			score_mul (bonus_scored, standupFrenzyNumHits);
			score_add (total_bonus, bonus_scored);

			score_zero (bonus_scored);
			score_add (bonus_scored, score_table[SC_1M]);
			score_mul (bonus_scored, standupFrenzy_modes_achieved);
			score_add (total_bonus, bonus_scored);

			sprintf_score (bonus_scored);
			font_render_string_center (&font_fixed10, DMD_MIDDLE_X, DMD_BIG_CY_Cent, sprintf_buffer);
			sprintf ("FRENZY");
			font_render_string_center (&font_var5, DMD_MIDDLE_X, DMD_SMALL_CY_1, sprintf_buffer);
			bonus_sched_transition ();
			dmd_show_low ();
			bonus_pause ();
	}/***end of standupFrenzy scoring and display ***/




	/**
	 * *fortress scoring and display *
	 * **/
	if (fortress_jackpot_shots_made > 0) {
			sound_start1 (ST_ANY, BONUS_SHORT);
			dmd_alloc_low_clean ();
			score_zero (bonus_scored);
			score_add (bonus_scored, score_table[SC_100K]);
			score_mul (bonus_scored, fortress_jackpot_shots_made);
			score_add (total_bonus, bonus_scored);

			sprintf_score (bonus_scored);
			font_render_string_center (&font_fixed10, DMD_MIDDLE_X, DMD_BIG_CY_Cent, sprintf_buffer);
			sprintf ("FORTRESS");
			font_render_string_center (&font_var5, DMD_MIDDLE_X, DMD_SMALL_CY_1, sprintf_buffer);
			bonus_sched_transition ();
			dmd_show_low ();
			bonus_pause ();
	}/***end of fortress scoring and display ***/




	/**
	 * *demotime scoring and display *
	 * **/
	if (demotime_jackpot_shots_made > 0) {
			sound_start1 (ST_ANY, BONUS_SHORT);
			dmd_alloc_low_clean ();
			score_zero (bonus_scored);
			score_add (bonus_scored, score_table[SC_100K]);
			score_mul (bonus_scored, demotime_jackpot_shots_made);
			score_add (total_bonus, bonus_scored);

			sprintf_score (bonus_scored);
			font_render_string_center (&font_fixed10, DMD_MIDDLE_X, DMD_BIG_CY_Cent, sprintf_buffer);
			sprintf ("demotime");
			font_render_string_center (&font_var5, DMD_MIDDLE_X, DMD_SMALL_CY_1, sprintf_buffer);
			bonus_sched_transition ();
			dmd_show_low ();
			bonus_pause ();
	}/***end of demotime_ scoring and display ***/




	/**
	 * *wasteland scoring and display *
	 * **/
	if (wasteland_jackpot_shots_made > 0) {
			sound_start1 (ST_ANY, BONUS_SHORT);
			dmd_alloc_low_clean ();
			score_zero (bonus_scored);
			score_add (bonus_scored, score_table[SC_100K]);
			score_mul (bonus_scored, wasteland_jackpot_shots_made);
			score_add (total_bonus, bonus_scored);

			sprintf_score (bonus_scored);
			font_render_string_center (&font_fixed10, DMD_MIDDLE_X, DMD_BIG_CY_Cent, sprintf_buffer);
			sprintf ("WASTELAND");
			font_render_string_center (&font_var5, DMD_MIDDLE_X, DMD_SMALL_CY_1, sprintf_buffer);
			bonus_sched_transition ();
			dmd_show_low ();
			bonus_pause ();
	}/***end of wasteland_ scoring and display ***/





	/**
	 * *cryoprison scoring and display *
	 * **/
	if (cryoprison_jackpot_shots_made > 0) {
			sound_start1 (ST_ANY, BONUS_SHORT);
			dmd_alloc_low_clean ();
			score_zero (bonus_scored);
			score_add (bonus_scored, score_table[SC_100K]);
			score_mul (bonus_scored, cryoprison_jackpot_shots_made);
			score_add (total_bonus, bonus_scored);

			sprintf_score (bonus_scored);
			font_render_string_center (&font_fixed10, DMD_MIDDLE_X, DMD_BIG_CY_Cent, sprintf_buffer);
			sprintf ("CRYOPRISON");
			font_render_string_center (&font_var5, DMD_MIDDLE_X, DMD_SMALL_CY_1, sprintf_buffer);
			bonus_sched_transition ();
			dmd_show_low ();
			bonus_pause ();
	}/***end of cryoprison_ scoring and display ***/




	/**
	 * *demotime scoring and display *
	 * **/
	if (demotime_jackpot_shots_made > 0) {
			sound_start1 (ST_ANY, BONUS_SHORT);
			dmd_alloc_low_clean ();
			score_zero (bonus_scored);
			score_add (bonus_scored, score_table[SC_100K]);
			score_mul (bonus_scored, demotime_jackpot_shots_made);
			score_add (total_bonus, bonus_scored);

			sprintf_score (bonus_scored);
			font_render_string_center (&font_fixed10, DMD_MIDDLE_X, DMD_BIG_CY_Cent, sprintf_buffer);
			sprintf ("DEMOLITION TIME");
			font_render_string_center (&font_var5, DMD_MIDDLE_X, DMD_SMALL_CY_1, sprintf_buffer);
			bonus_sched_transition ();
			dmd_show_low ();
			bonus_pause ();
	}/***end of demotime_ scoring and display ***/





	/**
	 * *huxley scoring and display *
	 * **/
	if (huxley_mode_shots_made > 0) {
			sound_start1 (ST_ANY, BONUS_SHORT);
			dmd_alloc_low_clean ();
			score_zero (bonus_scored);
			score_add (bonus_scored, score_table[SC_100K]);
			score_mul (bonus_scored, huxley_mode_shots_made);
			score_add (total_bonus, bonus_scored);

			sprintf_score (bonus_scored);
			font_render_string_center (&font_fixed10, DMD_MIDDLE_X, DMD_BIG_CY_Cent, sprintf_buffer);
			sprintf ("HUXLEY");
			font_render_string_center (&font_var5, DMD_MIDDLE_X, DMD_SMALL_CY_1, sprintf_buffer);
			bonus_sched_transition ();
			dmd_show_low ();
			bonus_pause ();
	}/***end of huxley scoring and display ***/





	/**
	 * *back_in_the_fridge scoring and display *
	 * **/
	if (back_in_the_fridge_shots_made > 0) {
			sound_start1 (ST_ANY, BONUS_SHORT);
			dmd_alloc_low_clean ();
			score_zero (bonus_scored);
			score_add (bonus_scored, score_table[SC_100K]);
			score_mul (bonus_scored, back_in_the_fridge_shots_made);
			score_add (total_bonus, bonus_scored);

			sprintf_score (bonus_scored);
			font_render_string_center (&font_fixed10, DMD_MIDDLE_X, DMD_BIG_CY_Cent, sprintf_buffer);
			sprintf ("FRIDGE");
			font_render_string_center (&font_var5, DMD_MIDDLE_X, DMD_SMALL_CY_1, sprintf_buffer);
			bonus_sched_transition ();
			dmd_show_low ();
			bonus_pause ();
	}/***end of back_in_the_fridge scoring and display ***/




	/* Show final score */
	sound_start1 (ST_ANY, BONUS_LONG);

	score_long (total_bonus); // Add to total bonus to player score
	dmd_alloc_low_clean ();
	scores_draw ();
	dmd_sched_transition (&trans_scroll_up);
	dmd_show_low ();
	task_kill_gid (GID_BONUS_BUTTON_MONITOR);
	task_sleep_sec (1);
//	task_kill_gid (GID_BONUS_TALKING);

	/*make verbal comments on final score*/
	U8 BSoundCounter;
	BSoundCounter = random_scaled(5);
	if (check_for_puny_score () ) {
			if (BSoundCounter == 0)	sound_start1 (ST_ANY, SPCH_PATHETIC);
			if (BSoundCounter == 1)	sound_start1 (ST_ANY, SPCH_SIMON_LAUGH_LONG);
			if (BSoundCounter == 2)	sound_start1 (ST_ANY, SPCH_SIMON_SNICKER);
			if (BSoundCounter == 3)	sound_start1 (ST_ANY, SPCH_PATHETIC);
			if (BSoundCounter == 4)	sound_start1 (ST_ANY, SPCH_HATE_WHEN_THAT_HAPPENS);
	}
	else if (check_for_big_score () ) {
			if (BSoundCounter == 0)	sound_start1 (ST_ANY, SPCH_BEST_DAY_OF_MY_LIFE);
			if (BSoundCounter == 1)	sound_start1 (ST_ANY, SPCH_HOW_NICE);
			if (BSoundCounter == 2)	sound_start1 (ST_ANY, SPCH_LOVE_THIS_GUY);
			if (BSoundCounter == 3)	sound_start1 (ST_ANY, SPCH_SOMETHING_RIGHT_PREV_LIFE);
			if (BSoundCounter == 4)	sound_start1 (ST_ANY, SPCH_OUTSTANDING);
	}
	task_sleep_sec (2);
	in_bonus = FALSE;
	//this will start second half of end_ball routine in kernal/game.c
	//or if tilted will reset tilt at common/tilt.c
	callset_invoke (bonus_complete);
	deff_exit ();
}//END OF FUNCTION




/****************************************************************************
 *
 * bonus helper functions
 *
 ****************************************************************************/
/* Speed up the bonus if flipper buttons are pressed */
void bonus_button_monitor (void) {
	buttons_held = FALSE;
	for (;;) {
		if (	switch_poll_logical (SW_LEFT_BUTTON)
			||	switch_poll_logical (SW_RIGHT_BUTTON)
			||	switch_poll_logical (SW_U_L_FLIPPER_BUTTON)
			||	switch_poll_logical (SW_U_R_FLIPPER_BUTTON) )
					buttons_held = TRUE;
		else 		buttons_held = FALSE;
		task_sleep (TIME_200MS);
	}//end of for loop
}//end of function





/* Function so we can call two different transistions
 * depending on whether the buttons were pressed */
void bonus_sched_transition (void) {
	if (buttons_held == TRUE)
		dmd_sched_transition (&trans_scroll_down_fast);
	else
		dmd_sched_transition (&trans_scroll_down);
}//end of function




void bonus_pause (void) {
	if (buttons_held) 	task_sleep (TIME_100MS);
	else 				task_sleep_sec (1);
}//end of function






/* Used to announce if the player has played well */
bool check_for_big_score (void) {
	if (score_compare (total_bonus, score_table[SC_5M]) == 1) 	return TRUE;
			else 												return FALSE;
}//end of function






//returns 1 if s1 > s2
bool check_for_puny_score (void) {
	if (score_compare (score_table[SC_5M], total_bonus) == 1)  	return TRUE;
			else												return FALSE;
}//end of function






/* See if it's the players last ball */
/*
bool check_if_last_ball_for_multiplayer (void)
{
	if (ball_up == system_config.balls_per_game && num_players > 1 && extra_balls == 0)
		return TRUE;
	else
		return FALSE;
}
*/




/*
bool check_if_last_ball_of_multiplayer_game (void)
{
	if (ball_up == system_config.balls_per_game && player_up == num_players &&
		num_players > 1 && extra_balls == 0)
		return TRUE;
	else
		return FALSE;
}
*/
