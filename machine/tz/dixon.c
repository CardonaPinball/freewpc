/*
 * Copyright 2011 by Ewan Meadows <sonny_jim@hotmail.com>
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

/* CALLSET_SECTION (dixon, __machine2__) */
#include <freewpc.h>

/*
 * Anti-cradling mechanism, as suggested by Philip Dixon
 * During multiball, if the player holds the ball on a flipper for longer than 5
 * seconds, the flipper is momentarily disabled, until the player releases the
 * button.
 */

bool flippers_dixoned;

static void dixon_the_flippers (void)
{
	sound_send (SND_WITH_THE_DEVIL);
	deff_start (DEFF_ANTI_CRADLE);
	flippers_dixoned = TRUE;
	flipper_disable ();
	task_sleep (TIME_500MS);
	task_exit ();
}

/* Monitor the switch */
static inline void anti_cradle_monitor (U8 switch_number, U8 timeout)
{
	while (switch_poll_logical (switch_number)
		&& in_live_game)
	{
		task_sleep (TIME_500MS);
		if (--timeout == 0)
		{
			if (!task_find_gid (GID_DIXON_THE_FLIPPERS))
				task_create_gid (GID_DIXON_THE_FLIPPERS, dixon_the_flippers);
			break;
		}
		/* Warn the player 2 seconds before disabling*/
		else if (timeout == 4)
		{
			sound_send (SND_TILT_WARNING);
			deff_start (DEFF_TILT_WARNING);
		}
		else if (timeout == 6 || timeout == 2)
			sound_send (SND_TILT_WARNING);
	}
}

static bool anti_cradle_enabled (void)
{
	if (in_live_game && !single_ball_play ()
#ifdef CONFIG_MUTE_AND_PAUSE
			&& !task_find_gid (GID_MUTE_AND_PAUSE)
#endif
			&& feature_config.dixon_anti_cradle == YES)
		return TRUE;
	else
		return FALSE;
}

CALLSET_ENTRY (dixon, init, end_game, start_ball, end_ball, single_ball_play)
{
	task_kill_gid (GID_DIXON_THE_FLIPPERS);
	task_kill_gid (GID_ANTI_CRADLE);
	flippers_dixoned = FALSE;
}

static bool check_for_dixoned_flippers (void)
{
	if (flippers_dixoned && !task_find_gid (GID_DIXON_THE_FLIPPERS))
	{
		flippers_dixoned = FALSE;
		flipper_enable ();
		return TRUE;
	}
	else
		return FALSE;
}

static void anti_cradle_shooter_task (void)
{
	anti_cradle_monitor (SW_SHOOTER, 20);
	task_exit ();
}

static void anti_cradle_left_task (void)
{
	anti_cradle_monitor (SW_LEFT_BUTTON, 10);
	task_exit ();
}

static void anti_cradle_right_task (void)
{
	anti_cradle_monitor (SW_RIGHT_BUTTON, 10);
	task_exit ();
}

CALLSET_ENTRY (dixon, sw_shooter)
{
//	if (!check_for_dixoned_flippers ()&& anti_cradle_enabled ())
//		task_recreate_gid (GID_ANTI_CRADLE, anti_cradle_shooter_task);
}

CALLSET_ENTRY (dixon, sw_left_button)
{
	if (!check_for_dixoned_flippers ()&& anti_cradle_enabled ())
		task_create_gid (GID_ANTI_CRADLE, anti_cradle_left_task);
}

CALLSET_ENTRY (dixon, sw_right_button)
{
	if (!check_for_dixoned_flippers ()&& anti_cradle_enabled ())
		task_create_gid (GID_ANTI_CRADLE, anti_cradle_right_task);
}
