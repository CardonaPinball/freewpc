/*
 * demolition man
 * inlanes.c
 * 
 * Location Description:
 * inlanes are lanes that feed to flippers as opposed to outlanes which feed
 * to the drain
 *
 * Scoring Description: (original game)
 * an unlit or lit lane score 5770
 * if lit, you recieve whatever the light says - access claw on left
 * will move the claw diverter to open it - when the diverter moves, the
 * light goes out
 * on the right inlane is quick freeze, getting this inlane will then
 * light quick freeze light on left ramp and to freeze a ball you must then shoot
 * the left ramp.  The light quick freeze inlane light does not go out until
 * the left ramp is shot and the ball is frozen
 *
 * access claw can be lit by completing MTL rollovers,
 * TODO: random award from top hole,
 * TODO: random award from computer (subway shot) or
 * TODO: completing certain number of arrow shots
 *
 * Depending on machine settings, One, Two, or Three sets of
 * standup targets must be completed to light Quick Freeze, depending on
 * whether Quick Freeze is set to easy, medium, or hard.
 *
 * Scoring Description: (my rules)
 * same as above
 *
 *
 * */

#include <freewpc.h>

//local variables
__boolean 	left_inlane_Access_Claw_activated;
__boolean 	right_inlane_Light_Quick_Freeze_activated;

//prototypes


/****************************************************************************
 * initialize  and exit
 ***************************************************************************/
void inlanes_reset (void) {
	left_inlane_Access_Claw_activated = FALSE;
	lamp_tristate_off(LM_ACCESS_CLAW);
	right_inlane_Light_Quick_Freeze_activated = FALSE;
	lamp_tristate_off(LM_LIGHT_QUICK_FREEZE);
}//end of reset

CALLSET_ENTRY (inlanes, start_player) {  inlanes_reset(); }
CALLSET_ENTRY (inlanes, start_ball) { inlanes_reset(); }


/****************************************************************************
 * left inlane
 *
 * access claw can be lit by completing MTL rollovers, random award from top hole,
 * random award from computer (subway shot) or completing certain number of
 * arrow shots
 ***************************************************************************/
CALLSET_ENTRY (inlanes, Access_Claw_Light_On) {
	left_inlane_Access_Claw_activated = TRUE;
	lamp_tristate_flash(LM_ACCESS_CLAW);
	task_sleep (TIME_300MS);
	lamp_tristate_on (LM_ACCESS_CLAW);
	}

CALLSET_ENTRY (inlanes, Access_Claw_Light_Off) {
	left_inlane_Access_Claw_activated = FALSE;
	lamp_tristate_flash(LM_ACCESS_CLAW);
	task_sleep (TIME_300MS);
	lamp_tristate_off (LM_ACCESS_CLAW);
	}

CALLSET_ENTRY (inlanes, sw_left_inlane) {
	score(SC_5770);
	if (left_inlane_Access_Claw_activated)	{
		callset_invoke(RRamp_ClawReady_On);
		callset_invoke(Access_Claw_Light_Off);
		sound_start (ST_SPEECH, SPCH_CRYOCLAW_ACTIVATED, SL_1S, PRI_GAME_QUICK5);
		}
	else
		sound_start (ST_SAMPLE, EXPLOSION, SL_1S, PRI_GAME_QUICK5);
}


/****************************************************************************
 * right inlane
 *
 * quick freeze can be lit by  hitting One, Two, or Three sets of
 * standup targets.  see standupfrenzy.c
 ***************************************************************************/
CALLSET_ENTRY (inlanes, light_quick_freeze_light_on) {
	right_inlane_Light_Quick_Freeze_activated = TRUE;
	lamp_tristate_flash(LM_LIGHT_QUICK_FREEZE);
	task_sleep (TIME_300MS);
	lamp_tristate_on (LM_LIGHT_QUICK_FREEZE);
	}

CALLSET_ENTRY (inlanes, light_quick_freeze_light_off) {
	right_inlane_Light_Quick_Freeze_activated = FALSE;
	lamp_tristate_flash(LM_LIGHT_QUICK_FREEZE);
	task_sleep (TIME_300MS);
	lamp_tristate_off (LM_LIGHT_QUICK_FREEZE);
	}

CALLSET_ENTRY (inlanes, sw_right_inlane) {
	score(SC_5770);
	sound_start (ST_SPEECH, SPCH_QUICK_FREEZE_ACTIVATED, SL_1S, PRI_GAME_QUICK5);
	if (right_inlane_Light_Quick_Freeze_activated) callset_invoke(Activate_left_Ramp_QuickFreeze);
	}
