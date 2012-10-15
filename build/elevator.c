
/* This file was autogenerated by 'ctemp' from  for class duty. */

/***********************************************************************
 * elevator.c
 ***********************************************************************/

#include <freewpc.h>
#include "elevator.h"

__fastram__ elevator_state_t elevator_state;
__fastram__ U8 elevator_timer;
U8 elevator_speed;

__boolean elevator_timer;


void elevator_move_monitor (void) {
	sol_enable (SOL_ELEVATOR_MOTOR);
	task_sleep_sec (4);
	sol_disable (SOL_ELEVATOR_MOTOR);
	elevator_state = ELEVATOR_OFF;
	task_exit ();
}


void elevator_stop (void) {
	dbprintf ("Stopping\n");
	task_kill_gid (GID_ELEVATOR_MONITOR);
	elevator_state = ELEVATOR_OFF;
	sol_disable (SOL_ELEVATOR_MOTOR);
}



void elevator_move_up (void) {
	disable_interrupts ();
	task_create_gid1 (GID_ELEVATOR_MONITOR, elevator_move_monitor);
	elevator_timer = 1;
	sol_enable (SOL_ELEVATOR_MOTOR);
	if (elevator_state != ELEVATOR_UP)
		elevator_state = ELEVATOR_UP;
	enable_interrupts ();
}



void elevator_move_down (void) {
	disable_interrupts ();
	task_create_gid1 (GID_ELEVATOR_MONITOR, elevator_move_monitor);
	elevator_timer = 1;
	sol_enable (SOL_ELEVATOR_MOTOR);
	if (elevator_state != ELEVATOR_DOWN) 
		elevator_state = ELEVATOR_DOWN;
	enable_interrupts ();
}



CALLSET_ENTRY (elevator, SW_ELEVATOR_HOLD) {
	if (elevator_state == ELEVATOR_UP)
		elevator_stop ();
}



CALLSET_ENTRY (elevator, SW_ELEVATOR_INDEX) {
	if (elevator_state == ELEVATOR_DOWN)
		elevator_stop ();
}



void elevator_bump_monitor (void) {
	sol_enable (SOL_ELEVATOR_MOTOR);
	task_sleep(TIME_66MS);
	sol_disable (SOL_ELEVATOR_MOTOR);
	elevator_state = ELEVATOR_OFF;
	task_exit ();
}



void elevator_bump_up (void) {
	disable_interrupts ();
	task_create_gid1 (GID_ELEVATOR_B_MONITOR, elevator_bump_monitor);
	elevator_timer = 1;
	sol_enable (SOL_ELEVATOR_MOTOR);
	if (elevator_state != ELEVATOR_UP)
		elevator_state = ELEVATOR_UP;
	enable_interrupts ();
}



void elevator_bump_down (void) {
	disable_interrupts ();
	task_create_gid1 (GID_ELEVATOR_B_MONITOR, elevator_bump_monitor);
	elevator_timer = 1;
	sol_enable (SOL_ELEVATOR_MOTOR);
	if (elevator_state != ELEVATOR_DOWN) 
		elevator_state = ELEVATOR_DOWN;
	enable_interrupts ();
}



CALLSET_ENTRY (elevator, tilt, test_start) {
	elevator_state = ELEVATOR_OFF;
	elevator_stop ();
}



CALLSET_ENTRY (elevator, amode_start) {
	elevator_state = ELEVATOR_INIT;
	if (ELEVATOR_DOWN == ELEVATOR_DOWN) 	elevator_move_down();
	else 									elevator_move_up();
}




CALLSET_ENTRY (elevator, init) {
	elevator_state = ELEVATOR_OFF;
	elevator_set_speed (ELEVATOR_DUTY_25);
}













/* vim: set filetype=c: */

