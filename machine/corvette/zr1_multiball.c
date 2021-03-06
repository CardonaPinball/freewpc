/*
 * Copyright 2010 by Dominic Clifton <me@dominicclifton.name>
 *
 * This file is part of FreeWPC.
 *
 * FreeWPC is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * FreeWPC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FreeWPC; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 * @TODO sonny_jim says use kick_request instead of dev_enter.
 *
 * @TODO It's possible to light the lock by doing a right orbit twice very quickly
 *       use event following instead of timed task or just adjust the timer value?
 *
 * @TODO find out why task sleep between ball unlocks doesn't work.
 *       The player needs enough time after the first ball it launched to hit
 *       the inner loop and attempt a shot on the skid pad.
 *       currently the second ball is launched too quickly and gets it the way
 *
 * @TODO use ZR1 flasher so the player knows when and where the balls are going to come from.
 *
 * @TODO reset all other players' locked_ball count to 0 at multiball start
 *       I'ts not fair on other players, but no way round it as balls are ejected from ZR1 lock...
 */

#include <freewpc.h>
#include <corvette/zr1.h>
#include <zr1_low_rev_gate.h>

__local__ U8 lock_count;

score_t zr1_mb_jackpot_value;

void zr1_ball_locked_deff (void)
{
	dmd_alloc_low_clean ();
	dmd_draw_border (dmd_low_buffer);
	sprintf ("BALL %d LOCKED", lock_count);
	font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);
	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
}

void zr1_mb_lit_deff (void)
{
	speech_start(SPCH_HEAD_FOR_THE_LT5, SL_3S);
	dmd_alloc_low_clean ();
	dmd_draw_border (dmd_low_buffer);
	font_render_string_center (&font_term6, 64, 9, "SHOOT ENGINE");
	font_render_string_center (&font_term6, 64, 21, "FOR ZR1 MULTIBALL");
	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
}

void zr1_mb_start_deff (void)
{
	sprintf ("ZR1 MULTIBALL");
	flash_and_exit_deff (30, TIME_100MS);
}


void zr1_mb_running_deff (void)
{
	for (;;)
	{
		score_update_start ();
		dmd_alloc_pair ();
		dmd_clean_page_low ();
		font_render_string_center (&font_var5, 64, 5, "MULTIBALL");
		sprintf_current_score ();
		font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);
		dmd_copy_low_to_high ();

		font_render_string_center (&font_var5, 64, 27, "SHOOT JACKPOTS");

		dmd_show_low ();
		do {
			task_sleep (TIME_100MS);
			dmd_show_other ();
			task_sleep (TIME_100MS);
			dmd_show_other ();
		} while (!score_update_required ());
	}
}


void zr1_mb_trq_jp_deff (void)
{
	sprintf ("TORQUE JACKPOT");
	flash_and_exit_deff (20, TIME_100MS);
}

void zr1_mb_hp_jp_deff (void)
{
	sprintf ("H.P. JACKPOT");
	flash_and_exit_deff (20, TIME_100MS);
}

void zr1_mb_reset (void)
{
	lock_count = 0;
	flag_on (FLAG_ZR1_MULTIBALL_LITE_LOCK_LIT);
	flag_off (FLAG_ZR1_MULTIBALL_LOCK_LIT);
	flag_off (FLAG_ZR1_MULTIBALL_RUNNING);
	flag_off (FLAG_TORQUE_JACKPOT_LIT);
	flag_off (FLAG_HORSEPOWER_JACKPOT_LIT);
}

void zr1_mb_award_lite_lock (void)
{
	if (!flag_test (FLAG_ZR1_MULTIBALL_LITE_LOCK_LIT)) {
		return;
	}

	flag_on (FLAG_ZR1_MULTIBALL_LOCK_LIT);
	flag_off (FLAG_ZR1_MULTIBALL_LITE_LOCK_LIT);

	deff_start (DEFF_ZR1_MB_LIT);
}

void zr1_mb_light_horsepower_jackpot (void)
{
	flag_on (FLAG_HORSEPOWER_JACKPOT_LIT);
}

void zr1_mb_light_torque_jackpot (void)
{
	flag_on (FLAG_TORQUE_JACKPOT_LIT);
}

void zr1_mb_start_task( void )
{
	flag_off (FLAG_ZR1_MULTIBALL_LOCK_LIT);

	zr1_set_shake_speed(ZR1_SHAKE_SPEED_MEDIUM);
	zr1_shake();

	deff_start (DEFF_ZR1_MB_START);
	flasher_pulse(FLASH_ZR1_UNDERSIDE);
	task_sleep_sec (1);
	flasher_pulse(FLASH_ZR1_UNDERSIDE);
	task_sleep_sec (1);
	flasher_pulse(FLASH_ZR1_UNDERSIDE);
	task_sleep_sec (1);
	flasher_pulse(FLASH_ZR1_UNDERSIDE);
	task_sleep_sec (1);
	flasher_pulse(FLASH_ZR1_UNDERSIDE);
	task_sleep_sec (1);

	zr1_center();

	flag_on (FLAG_ZR1_MULTIBALL_RUNNING);
	zr1_mb_light_torque_jackpot ();
	zr1_mb_light_horsepower_jackpot ();

	// start unlocking balls

	device_unlock_ball (device_entry (DEVNO_ZR1_POPPER));

	task_sleep_sec (3);
	device_unlock_ball (device_entry (DEVNO_ZR1_POPPER));

	task_sleep_sec (3);

	// third ball kicked out by returning from callset zr1_multiball dev_zr1_popper_enter
	task_exit ();
}

void zr1_mb_start (void)
{
	task_create_gid1 (GID_ZR1_MB_START_TASK, zr1_mb_start_task);
}

static void jackpot_check (void) {
	if (!flag_test(FLAG_ZR1_MULTIBALL_RUNNING)) {
		return;
	}

	if (flag_test (FLAG_HORSEPOWER_JACKPOT_LIT) || flag_test (FLAG_TORQUE_JACKPOT_LIT)) {
		return; // jackpots still lit - player must collect both to relight jackpots
	}

	zr1_mb_light_torque_jackpot();
	zr1_mb_light_horsepower_jackpot();


	// TODO see if both hit very quickly, if so award super jackpot.
}

static void zr1_mb_award_horsepower_jackpot (void)
{
	if (!flag_test (FLAG_HORSEPOWER_JACKPOT_LIT)) {
		return;
	}
	flag_off (FLAG_HORSEPOWER_JACKPOT_LIT);

	sound_start (ST_SPEECH, SPCH_HORSEPOWER_JACKPOT, SL_2S, PRI_JACKPOT);
	score (SC_25M);
	/* TODO increase horsepower jackpot value */

	deff_start (DEFF_ZR1_MB_HP_JP);

	jackpot_check();
}


static void zr1_mb_award_torque_jackpot (void)
{
	if (!flag_test (FLAG_TORQUE_JACKPOT_LIT)) {
		return;
	}
	flag_off (FLAG_TORQUE_JACKPOT_LIT);

	sound_start (ST_SPEECH, SPCH_TORQUE_JACKPOT, SL_2S, PRI_JACKPOT);
	score (SC_25M);
	/* TODO increase torque jackpot value */

	deff_start (DEFF_ZR1_MB_TRQ_JP);

	jackpot_check();
}

static void zr1_mb_award_lock ( void )
{
	if (!flag_test (FLAG_ZR1_MULTIBALL_LOCK_LIT)) {
		return;
	}

	lock_count++;

	if (lock_count == 3) {
		zr1_mb_start();
		return;
	}

	sound_start (ST_SAMPLE, SND_DITTY_07, SL_4S, PRI_GAME_QUICK5);
	deff_start (DEFF_ZR1_BALL_LOCKED);

	U8 unlock_from_trough = (device_recount (device_entry (DEVNO_ZR1_POPPER)) >= lock_count);

	if (unlock_from_trough) {
		// locking a ball in the engine causes a new ball to be placed in the trough
		device_lock_ball (device_entry (DEVNO_ZR1_POPPER));
	}
}

CALLSET_ENTRY (zr1_multiball, left_orbit_shot, left_orbit_to_rollover_shot)
{
	if (flag_test (FLAG_ZR1_MULTIBALL_RUNNING)) {
		return;
	}

	if (flag_test (FLAG_ZR1_MULTIBALL_LITE_LOCK_LIT)) {
		zr1_mb_award_lite_lock();
	}
}

CALLSET_BOOL_ENTRY (zr1_multiball, dev_zr1_popper_kick_request)
{
	if (flag_test (FLAG_ZR1_MULTIBALL_RUNNING)) {
		return TRUE;
	}

	if (!task_find_gid (GID_ZR1_MB_START_TASK)) {
		return TRUE;
	}

	 // hold the kickout for a bit.
	return FALSE;
}

CALLSET_ENTRY (zr1_multiball, dev_zr1_popper_kick_attempt) {
	// TODO use custom flasher timings
	flasher_pulse(FLASH_ZR1_RAMP);
	task_sleep(TIME_100MS);
	flasher_pulse(FLASH_ZR1_RAMP);
	task_sleep(TIME_100MS);
	flasher_pulse(FLASH_ZR1_RAMP);
	task_sleep(TIME_100MS);
	flasher_pulse(FLASH_ZR1_RAMP);
	task_sleep(TIME_100MS);
}

CALLSET_ENTRY (zr1_multiball, dev_zr1_popper_kick_success) {
	if (flag_test(FLAG_ZR1_MULTIBALL_RUNNING)) {
		return;
	}

	sound_start (ST_SAMPLE, SND_BEEP_BEEP, SL_2S, PRI_GAME_QUICK5);
}

CALLSET_ENTRY (zr1_multiball, dev_zr1_popper_enter) {
	zr1_mb_award_lock ();
}

CALLSET_ENTRY (zr1_multiball, skid_pad_shot)
{
	if (!flag_test (FLAG_ZR1_MULTIBALL_RUNNING)) {
		return;
	}
	zr1_mb_award_torque_jackpot ();
}

CALLSET_ENTRY (zr1_multiball, inner_loop_shot)
{
	if (!flag_test (FLAG_ZR1_MULTIBALL_RUNNING)) {
		return;
	}

	zr1_mb_award_horsepower_jackpot ();
}

CALLSET_ENTRY (zr1_multiball, lamp_update)
{
	lamp_flash_if (LM_LITE_LOCK, flag_test (FLAG_ZR1_MULTIBALL_LITE_LOCK_LIT));
	lamp_flash_if (LM_ZR1_RAMP_LOCK, flag_test (FLAG_ZR1_MULTIBALL_LOCK_LIT));
	lamp_flash_if (LM_INNER_LOOP_JACKPOT, flag_test (FLAG_HORSEPOWER_JACKPOT_LIT));
	lamp_flash_if (LM_SKID_PAD_JACKPOT, flag_test (FLAG_TORQUE_JACKPOT_LIT));
}

CALLSET_ENTRY (zr1_multiball, device_update)
{
	// TODO other modes will want to change (open) the up rev gate, perform suitable mode check here (e.g. if mode set then bail and let mode control it instead)

	// close the ZR1 upper 'rev' gate when zr1 multiball lock is lit
	if (flag_test (FLAG_ZR1_MULTIBALL_LOCK_LIT)) {
		flag_off (FLAG_ZR1_UP_REV_GATE_OPENED);
	} else {
		flag_on (FLAG_ZR1_UP_REV_GATE_OPENED);
	}
}

CALLSET_ENTRY (zr1_multiball, music_refresh)
{
	if (flag_test (FLAG_ZR1_MULTIBALL_RUNNING))
	{
		music_request (MUS_MULTIBALL, PRI_GAME_MODE3);
	}
}

CALLSET_ENTRY (zr1_multiball, display_update)
{
	if (flag_test (FLAG_ZR1_MULTIBALL_RUNNING))
		deff_start_bg (DEFF_ZR1_MB_RUNNING, PRI_GAME_MODE3);
}

CALLSET_ENTRY (zr1_multiball, start_player, single_ball_play)
{
	zr1_mb_reset ();
}

CALLSET_ENTRY (zr1_multiball, start_ball) {
	// update the locked_balls count in case another player started a multiball
	U8 zr1_ball_count = device_recount (device_entry (DEVNO_ZR1_POPPER));
	if (lock_count > zr1_ball_count) {
		lock_count = zr1_ball_count;
	}
}
