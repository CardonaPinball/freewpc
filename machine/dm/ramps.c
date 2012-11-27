/*
 * demolition man
 * ramps.c
 *
 * written by James Cardona
 *
 * Location Description:
 * There are 4 ramps total
 *
 * Left Ramp: Directly to the right of the left loop is this ramp which always
 * feeds to the left inlane. It can be lit for "Quick Freeze" and the "Car Chase" round.
 * Lights are, from bottom up, Quick Freeze, Car Chase, Explode, Jackpot, Combo arrow.
 *
 * Center Ramp (the ACMag): Located at the very top of the playfield, it is
 * a wide ramp (three pinball widths wide) that is triggered only by a gate.
 * Balls drop immediately and roll either down the left loop or into
 * the MTL rollovers. The center ramp has a Jackpot lamp and a Combo arrow.
 * The Center Ramp is bounded on the left by the Upper Rebound and on the right
 * by the left jet bumper.
 *
 * Side Ramp: This is a small, twisting, and difficult ramp just below the
 * Computer. It is used for the skill shot. The Side Ramp has a jackpot
 * lamp and a Combo Arrow.
 *
 * Right Ramp: This is a tall ramp similar to the right ramp in Judge Dredd.
 * It has a diverter which normally sends the ball to the right inlane,
 * but it can send the ball to the Cryo-Claw when that feature is available.
 * It's also used during the Car Chase round, and to score Super Jackpots.
 * Lights are, from bottom up, Claw Ready, Car Chase, Explode, Jackpot, Combo
 * Arrow. There is a Super Jackpot light mounted over the ramp.
 *
 *
 * Scoring Description: (original game)
 * Ramps score depending on mode running as follows:
 * basic score - no mode running: ramp 100k enter - 250k exit
 * explode running: scores only on left and right ramps- 5 to 15 million depending on time left
 * car crash running: scores only on left and right ramps- 10 million
 * car chase in orig is not a mode but single lights that can be lit on either L or R ramp
 * MB quick freeze - right inlane activates quick freeze light from left ramp - shoot left ramp to freeze.

 * TODO: Multiball: any ramp may be lit for jackpot shot - award of 20 mill
 * TODO: combos - 4million first combo and extra 2 mill for each successive combo
 * TODO: capture simon: all arrows flash and remain for easiest lev - shoot 3 arrows 5 mill each
 * first 2, last one 25 mill
 *
 * TODO: right ramp to access claw - 1 million
 *
 * Scoring Description: (my rules)
 * same as original except:
 * car chase is a mode here (not in original) --L or R ramp for xx million shot --handled in carchase.c
 *
 *
 */

#include <freewpc.h>
#include <diverter.h> //autogenerated by divhold.ct
#include "dm/global_constants.h"

//constants
const U8 		RAMPS_EASY_GOAL = 5;
const U8 		RAMPS_PREDEFINED_GOAL_INCREMENT = 1;
const U8 		RAMPS_GOAL_STEP = 5;
const U8 		RAMPS_GOAL_MAX = 50;
const U8 		RampsTotalNumOfSounds = 18; //num between 0 and 24 == 25 total
const sound_code_t rampSoundsArray[] = {	SPCH_NICE_SHOOTING_SLY, 	SPCH_NICE_SHOT_SLY,
											SPCH_GREAT_SHOT_SLY, SPCH_GREAT_SHOOTING, 		SPCH_NOW_THATS_WHAT_I_CALL_SHOOTING,
											SPCH_OHH_YEAH1, 	SPCH_OHH_YEAH2, 			SPCH_OUTSTANDING_SLY,
											SPCH_NICE, 			SPCH_VERY_NICE, 			SPCH_NICE_MOVE_SLY,
											SPCH_GOOD_ONE, 		SPCH_GREAT_SHOT, 			SPCH_GOOD_SHOT,
											SPCH_NICE_SHOT, 	SPCH_NICE_SHOOTING, 	SPCH_NICE_SHOOTING,
											SPCH_EXCELLENT };

const U8 		missedTotalNumOfSounds =11;
const sound_code_t missedRampSoundsArray[] = {SPCH_TOO_BAD, 		SPCH_PATHETIC,		SPCH_HATE_WHEN_THAT_HAPPENS,
											SPCH_OHH_NO,		SPCH_BAD_AIM,		SPCH_DULCH,
											SPCH_WES_LAUGH_SHORT1, SPCH_SIMON_SNICKER, SPCH_WES_LAUGH_SHORT2,
											SPCH_WES_LAUGH_SHORT3, 			SPCH_JUST_LOVELY };

//local variables
U8	ramp_SoundCounter;
U8 	left_ramp_counter;
U8 	right_ramp_counter;
U8 	center_ramp_counter;
U8 	side_ramp_counter;
U8 	all_ramp_counter;
U8 		left_ramp_goal;
U8 		right_ramp_goal;
U8 		center_ramp_goal;
U8 		side_ramp_goal;
U8 		all_ramp_goal;
__boolean 	left_Ramp_CarChase_activated;
__boolean 	left_Ramp_Explode_activated;
__boolean 		right_Ramp_CarChase_activated;
__boolean 		right_Ramp_Explode_activated;

//external variables

//prototypes
void ramps_reset (void);
void right_ramp_task (void);
void right_ramp_goal_award (void);
void left_ramp_task (void);
void left_ramp_goal_award (void);
void center_ramp_goal_award (void);
void ramps_flasher (char * flasher);
void side_ramp_task (void);
void side_ramp_goal_award (void);
void ramp_sounds (void);
void missed_ramp_sounds (void);
void left_ramp_made(void);
void right_ramp_made(void);
void side_ramp_made(void);


/****************************************************************************
 * initialize  and exit
 ***************************************************************************/
void ramps_reset (void) {
	left_ramp_counter = 0;
	right_ramp_counter = 0;
	center_ramp_counter = 0;
	side_ramp_counter = 0;
	all_ramp_counter = 0;
		left_Ramp_CarChase_activated = FALSE;
		left_Ramp_Explode_activated = FALSE;
	right_Ramp_CarChase_activated = FALSE;
	right_Ramp_Explode_activated = FALSE;
	if (flag_test(FLAG_IS_R_RAMP_CLAWREADY) ) 	callset_invoke (rramp_clawready_on);
	else														callset_invoke (rramp_clawready_off);
}//end of function

void players_ramps_reset (void) {
	ramp_SoundCounter = 0;
	right_ramp_goal = RAMPS_EASY_GOAL;
	left_ramp_goal = RAMPS_EASY_GOAL;
	center_ramp_goal = RAMPS_EASY_GOAL;
	side_ramp_goal = RAMPS_EASY_GOAL;
	all_ramp_goal = (RAMPS_EASY_GOAL * 5);
	flag_off(FLAG_IS_LRAMP_QUICKFREEZE_ACTIVATED);
	flag_off(FLAG_IS_R_RAMP_CLAWREADY);
	ramps_reset();
}//end of function

CALLSET_ENTRY (ramps, start_player) { players_ramps_reset(); }
CALLSET_ENTRY (ramps, start_ball) { ramps_reset(); }
CALLSET_ENTRY (ramps, end_ball) {}



/****************************************************************************
 * playfield lights and flags -- MULTIPLE RAMPS AT SAME TIME
 ***************************************************************************/
//called from eyeball_explode.c for explode mode start
void activate_explode_inserts_ramps(void) {
	left_Ramp_Explode_activated = TRUE;
	right_Ramp_Explode_activated = TRUE;
	lamp_tristate_flash (LM_LEFT_RAMP_EXPLODE);
	lamp_tristate_flash (LM_RIGHT_RAMP_EXPLODE);
}//end of function

void deactivate_explode_inserts_ramps(void) {
	left_Ramp_Explode_activated = FALSE;
	right_Ramp_Explode_activated = FALSE;
	lamp_tristate_off (LM_LEFT_RAMP_EXPLODE);
	lamp_tristate_off (LM_RIGHT_RAMP_EXPLODE);
}//end of function


//called from carcrash.c for carcrash mode start
void carchase_mode_on(void) {
	left_Ramp_CarChase_activated = TRUE;
	right_Ramp_CarChase_activated = TRUE;
	lamp_tristate_flash (LM_LEFT_RAMP_CAR_CHASE);
	lamp_tristate_flash (LM_RIGHT_RAMP_CAR_CHASE);
}//end of function

void carchase_mode_off(void) {
	left_Ramp_CarChase_activated = FALSE;
	right_Ramp_CarChase_activated = FALSE;
	lamp_tristate_off (LM_LEFT_RAMP_CAR_CHASE);
	lamp_tristate_off (LM_RIGHT_RAMP_CAR_CHASE);
}//end of function


/****************************************************************************
 * playfield lights and flags - LEFT RAMP
 ***************************************************************************/
// called by right inlane switch --see inlanes.c
// or called by certain number of standups made --see standupfrenzy.c
void activate_left_ramp_quickfreeze(void) {
	flag_on(FLAG_IS_LRAMP_QUICKFREEZE_ACTIVATED);
	lamp_tristate_flash (LM_QUICK_FREEZE);
}//end of function

void deactivate_left_ramp_quickfreeze(void) {
	flag_off(FLAG_IS_LRAMP_QUICKFREEZE_ACTIVATED);
	lamp_tristate_off (LM_QUICK_FREEZE);
}//end of function

void lramp_jackpot_light_on(void) {
	flag_on(FLAG_IS_L_RAMP_JACKPOT_ACTIVATED);
	lamp_tristate_on (LM_LEFT_RAMP_JACKPOT);
}//end of function

void lramp_jackpot_light_off(void) {
	flag_off(FLAG_IS_L_RAMP_JACKPOT_ACTIVATED);
	lamp_tristate_off (LM_LEFT_RAMP_JACKPOT);
}//end of function

void lramp_arrow_light_on(void) {
	flag_on (FLAG_IS_L_RAMP_ARROW_ACTIVATED);
	lamp_tristate_on (LM_LEFT_RAMP_ARROW);
}//end of function

void lramp_arrow_light_off(void) {
	flag_off (FLAG_IS_L_RAMP_ARROW_ACTIVATED);
	lamp_tristate_off (LM_LEFT_RAMP_ARROW);
}//end of function


/****************************************************************************
 * playfield lights and flags - RIGHT RAMP
 ***************************************************************************/
void rramp_jackpot_light_on(void) {
	flag_on(FLAG_IS_R_RAMP_JACKPOT_ACTIVATED);
	lamp_tristate_on (LM_RIGHT_RAMP_JACKPOT);
}//end of function

void rramp_jackpot_light_off(void) {
	flag_off(FLAG_IS_R_RAMP_JACKPOT_ACTIVATED);
	lamp_tristate_off (LM_RIGHT_RAMP_JACKPOT);
}//end of function

void rramp_arrow_light_on(void) {
	flag_on (FLAG_IS_R_RAMP_ARROW_ACTIVATED);
	lamp_tristate_on (LM_RIGHT_RAMP_ARROW);
}//end of function

void rramp_arrow_light_off(void) {
	flag_off (FLAG_IS_R_RAMP_ARROW_ACTIVATED);
	lamp_tristate_off (LM_RIGHT_RAMP_ARROW);
}//end of function

//called by left inlane --see inlanes.c
void rramp_clawready_on(void) {
	flag_on(FLAG_IS_R_RAMP_CLAWREADY);
	lamp_tristate_on (LM_CLAW_READY);
	flasher_pulse (FLASH_DIVERTER_FLASHER);
	diverter_start();//defined in divhold2.ct
}//end of function

//called by claw after ball is passed to it from elevator
void rramp_clawready_off(void) {
	flag_off(FLAG_IS_R_RAMP_CLAWREADY);
	lamp_tristate_off (LM_CLAW_READY);
	flasher_pulse (FLASH_DIVERTER_FLASHER);
	diverter_stop(); //defined in divhold2.ct
}//end of function


/****************************************************************************
 * playfield lights and flags - CENTER RAMP
 ***************************************************************************/
void cramp_jackpot_light_on(void) {
	flag_on(FLAG_IS_C_RAMP_JACKPOT_ACTIVATED);
	lamp_tristate_on (LM_CENTER_RAMP_JACKPOT);
}//end of function

void cramp_jackpot_light_off(void) {
	flag_off(FLAG_IS_C_RAMP_JACKPOT_ACTIVATED);
	lamp_tristate_off (LM_CENTER_RAMP_JACKPOT);
}//end of function

void cramp_arrow_light_on(void) {
	flag_on (FLAG_IS_C_RAMP_ARROW_ACTIVATED);
	lamp_tristate_on (LM_CENTER_RAMP_ARROW);
}//end of function

void cramp_arrow_light_off(void) {
	flag_off (FLAG_IS_C_RAMP_ARROW_ACTIVATED);
	lamp_tristate_off (LM_CENTER_RAMP_ARROW);
}//end of function


/****************************************************************************
 * playfield lights and flags - SIDE RAMP
 ***************************************************************************/
void sramp_jackpot_light_on(void) {
	flag_on(FLAG_IS_S_RAMP_JACKPOT_ACTIVATED);
	lamp_tristate_on (LM_SIDE_RAMP_JACKPOT);
}//end of function

void sramp_jackpot_light_off(void) {
	flag_off(FLAG_IS_S_RAMP_JACKPOT_ACTIVATED);
	lamp_tristate_off (LM_SIDE_RAMP_JACKPOT);
}//end of function

void sramp_arrow_light_on(void) {
	flag_on (FLAG_IS_S_RAMP_ARROW_ACTIVATED);
	lamp_tristate_on (LM_SIDE_RAMP_ARROW);
}//end of function

void sramp_arrow_light_off(void) {
	flag_off (FLAG_IS_S_RAMP_ARROW_ACTIVATED);
	lamp_tristate_off (LM_SIDE_RAMP_ARROW);
}//end of function

/****************************************************************************
 *
 * left ramp
 *
 ***************************************************************************/
void left_ramp_task (void) { task_sleep_sec(2); task_exit(); }


CALLSET_ENTRY (ramps, sw_left_ramp_enter) {
	task_create_gid1 (GID_LEFT_RAMP_ENTERED, left_ramp_task);
	flasher_pulse (FLASH_LEFT_RAMP_UP_FLASHER); //FLASH followed by name of flasher in caps
	score (RAMPS_ENTER_SCORE);
	if (flag_test (FLAG_IS_CARCHASE_MODE_RUNNING) ) sound_start (ST_SAMPLE, CAR_SKID, SL_3S, PRI_GAME_QUICK1);
}//end of function


CALLSET_ENTRY (ramps, sw_left_ramp_exit) {
	if ( task_kill_gid(GID_LEFT_RAMP_ENTERED) ) left_ramp_made();
}//end of function


void left_ramp_made(void) {
	++left_ramp_counter;
	++all_ramp_counter;
	score (RAMPS_MADE_SCORE);
	flasher_pulse (FLASH_LEFT_RAMP_UP_FLASHER); //FLASH followed by name of flasher in caps
	task_sleep (TIME_100MS);
	if (flag_test(FLAG_IS_L_RAMP_JACKPOT_ACTIVATED) ) 				score_jackpot();
	else if (flag_test(FLAG_IS_HUXLEY_RUNNING) )					huxley_mode_shot_made();
	else if (flag_test (FLAG_IS_PBREAK_RUNNING) )  					prison_break_made();
	else if (flag_test (FLAG_IS_CAPSIM_LEFTRAMP_ACTIVATED) )		capture_simon_made();
	else if (flag_test (FLAG_IS_EXPLODE_MODE_RUNNING) ) 			explode_made(); //goto eyeball_explode.c for scoring
	else if (flag_test (FLAG_IS_CARCHASE_MODE_RUNNING) )			car_chase_ramp_made(); //goto carchase.c for scoring
	else if (flag_test(FLAG_IS_LRAMP_QUICKFREEZE_ACTIVATED) )	 	increment_freeze(); //goto lock_freeze_mbstart.c
	else if (flag_test(FLAG_IS_COMBOS_KILLED) ) 					combo_init();
	else if ( flag_test(FLAG_IS_COMBO_LEFTRAMP_ACTIVATED) ) 		combo_hit();

	//if not in a mode then perform normal sounds and display effects
	if (	!flag_test (FLAG_IS_EXPLODE_MODE_RUNNING)
		&& 	!flag_test (FLAG_IS_CARCHASE_MODE_RUNNING)
		&& 	!flag_test (FLAG_IS_CAPSIM_LEFTRAMP_ACTIVATED) )
				ramp_sounds();
	// TODO: normal display effects call
	if (left_ramp_counter == left_ramp_goal)  left_ramp_goal_award();
}//end of function


void left_ramp_goal_award (void) {
	//sound_start (ST_SAMPLE, EXPLOSION, SL_1S, PRI_GAME_QUICK5);
	left_ramp_counter = 0;
	if (left_ramp_goal < RAMPS_GOAL_MAX)  left_ramp_goal += RAMPS_GOAL_STEP;
}//end of function



/****************************************************************************
 *
 * right ramp
 *
 ***************************************************************************/
void right_ramp_task (void) { task_sleep_sec(2); task_exit(); }


CALLSET_ENTRY (ramps, sw_right_ramp_enter) {
	task_create_gid1 (GID_RIGHT_RAMP_ENTERED, right_ramp_task);
	flasher_pulse (FLASH_RIGHT_RAMP_UP_FLASHER); //FLASH followed by name of flasher in caps
	score (RAMPS_ENTER_SCORE);
	if (flag_test (FLAG_IS_CARCHASE_MODE_RUNNING) ) sound_start (ST_SAMPLE, CAR_SKID, SL_3S, PRI_GAME_QUICK1);
}//end of function


CALLSET_ENTRY (ramps, sw_right_ramp_exit) {
	if ( task_kill_gid(GID_RIGHT_RAMP_ENTERED) ) right_ramp_made();
}//end of function


void right_ramp_made(void) {
	++right_ramp_counter;
	++all_ramp_counter;
	score (RAMPS_MADE_SCORE);
	flasher_pulse (FLASH_RIGHT_RAMP_UP_FLASHER); //FLASH followed by name of flasher in caps
	task_sleep (TIME_100MS);
	if (flag_test(FLAG_IS_R_RAMP_JACKPOT_ACTIVATED) ) 			score_jackpot();
	else if (flag_test(FLAG_IS_HUXLEY_RUNNING) )				huxley_mode_shot_made();
	else if (flag_test (FLAG_IS_PBREAK_RUNNING) ) 				prison_break_made();
	else if (flag_test (FLAG_IS_CAPSIM_RIGHTRAMP_ACTIVATED) )	capture_simon_made();
	else if (flag_test (FLAG_IS_EXPLODE_MODE_RUNNING) ) 		explode_made(); //goto eyeball_explode.c for scoring
	else if (flag_test (FLAG_IS_CARCHASE_MODE_RUNNING) )		car_chase_ramp_made(); //goto carchase.c for scoring
	else if (flag_test(FLAG_IS_COMBOS_KILLED) ) 				combo_init();
	else if ( flag_test(FLAG_IS_COMBO_RIGHTRAMP_ACTIVATED) ) 	combo_hit();

	//if not in a mode then perform normal sounds and display effects
	if (	!flag_test (FLAG_IS_EXPLODE_MODE_RUNNING)
		&& 	!flag_test (FLAG_IS_CARCHASE_MODE_RUNNING)
		&& 	!flag_test (FLAG_IS_CAPSIM_RIGHTRAMP_ACTIVATED) )
				ramp_sounds();
	// TODO: normal display effects
	if (right_ramp_counter == right_ramp_goal)  right_ramp_goal_award ();
}//end of function


void right_ramp_goal_award (void) {
	right_ramp_counter = 0;
	if (right_ramp_goal < RAMPS_GOAL_MAX)  right_ramp_goal += RAMPS_GOAL_STEP;
}//end of function



/****************************************************************************
 *
 * center ramp
 * note: center ramp has only 1 switch
 *
 ***************************************************************************/
CALLSET_ENTRY (ramps, sw_center_ramp) {
	++center_ramp_counter;
	++all_ramp_counter;
	score (RAMPS_MADE_SCORE);

	if (flag_test(FLAG_IS_C_RAMP_JACKPOT_ACTIVATED) ) 			score_jackpot();
	else if (flag_test(FLAG_IS_HUXLEY_RUNNING) )				huxley_mode_shot_made();
	else if (flag_test(FLAG_IS_ACMAG_RUNNING) ) 				acmag_made();
	else if (flag_test (FLAG_IS_CAPSIM_CENTERRAMP_ACTIVATED) )	capture_simon_made();
	else if (flag_test (FLAG_IS_PBREAK_RUNNING) ) 				prison_break_made();
	else if (flag_test(FLAG_IS_COMBOS_KILLED) ) 				combo_init();
	else if ( flag_test(FLAG_IS_COMBO_CENTERRAMP_ACTIVATED) ) 	combo_hit();

	if (!flag_test (FLAG_IS_PBREAK_RUNNING) ) 					ramp_sounds();

	lamp_tristate_flash(LM_CENTER_RAMP_MIDDLE);
	lamp_tristate_flash(LM_CENTER_RAMP_OUTER);
	lamp_tristate_flash(LM_CENTER_RAMP_INNER);
	task_sleep (TIME_100MS);
	lamp_tristate_off(LM_CENTER_RAMP_MIDDLE);
	lamp_tristate_off(LM_CENTER_RAMP_OUTER);
	lamp_tristate_off(LM_CENTER_RAMP_INNER);

	if (center_ramp_counter == center_ramp_goal)  center_ramp_goal_award ();
}//end of function



void center_ramp_goal_award (void) {
	//sound_start (ST_SAMPLE, EXPLOSION, SL_1S, PRI_GAME_QUICK5);
	center_ramp_counter = 0;
	if (center_ramp_goal < RAMPS_GOAL_MAX)  center_ramp_goal += RAMPS_GOAL_STEP;
}//end of function



/****************************************************************************
 *
 * side ramp
 *
 ***************************************************************************/
void side_ramp_task (void) { task_sleep_sec(2); task_exit(); }


CALLSET_ENTRY (ramps, sw_side_ramp_enter) {
	task_create_gid1 (GID_SIDE_RAMP_ENTERED, side_ramp_task);
	flasher_pulse (FLASH_SIDE_RAMP_FLASHER); //FLASH followed by name of flasher in caps
	task_sleep (TIME_100MS);
	score (RAMPS_ENTER_SCORE);
}//end of function


CALLSET_ENTRY (ramps, sw_side_ramp_exit) {
	if ( task_kill_gid(GID_SIDE_RAMP_ENTERED) ) side_ramp_made();
}//end of function


void side_ramp_made(void) {
	++side_ramp_counter;
	++all_ramp_counter;
	score (RAMPS_MADE_SCORE);
	flasher_pulse (FLASH_SIDE_RAMP_FLASHER); //FLASH followed by name of flasher in caps
	task_sleep (TIME_100MS);

	if (flag_test(FLAG_IS_S_RAMP_JACKPOT_ACTIVATED) ) 				score_jackpot();
	else if (flag_test (FLAG_IS_CAPSIM_SIDERAMP_ACTIVATED) )		capture_simon_made();
	else if (flag_test(FLAG_IS_HUXLEY_RUNNING) )					huxley_mode_shot_made();
	else if (flag_test (FLAG_IS_PBREAK_RUNNING) ) 					prison_break_made();
	else if (flag_test(FLAG_IS_COMBOS_KILLED) ) 					combo_init();
	else if ( flag_test(FLAG_IS_COMBO_SIDERAMP_ACTIVATED) ) 		combo_hit();

	ramp_sounds();

	if (side_ramp_counter == side_ramp_goal)  side_ramp_goal_award();
}//end of function


void side_ramp_goal_award (void) {
	side_ramp_counter = 0;
	if (side_ramp_goal < RAMPS_GOAL_MAX)  side_ramp_goal += RAMPS_GOAL_STEP;
}//end of function



/****************************************************************************
 * sound effects
 ****************************************************************************/
void ramp_sounds (void) {
	ramp_SoundCounter = random_scaled(RampsTotalNumOfSounds);//from kernal/random.c
	sound_start (ST_SPEECH, rampSoundsArray[ramp_SoundCounter], SL_4S, PRI_GAME_QUICK5);
}//end of function

void missed_ramp_sounds (void) {
	ramp_SoundCounter = random_scaled(missedTotalNumOfSounds);//from kernal/random.c
	sound_start (ST_SPEECH, missedRampSoundsArray[ramp_SoundCounter], SL_4S, PRI_GAME_QUICK2);
}//end of function



