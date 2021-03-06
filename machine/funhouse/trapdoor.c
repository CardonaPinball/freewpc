/*
 * Copyright 2009 by Brian Dominy <brian@oddchange.com>
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

#include <freewpc.h>

enum trap_door_state {
	CLOSED = 0,
	OPEN,
	UNKNOWN,
	DISABLED,
} trap_door_state;

__local__ U8 trap_door_bonuses;

const score_id_t trap_door_bonus_scores[] = {
	SC_250K, SC_500K, SC_750K
};
const struct generic_ladder trap_door_bonus_score_rule = {
	3,
	trap_door_bonus_scores,
	&trap_door_bonuses,
};



static enum trap_door_state trap_door_decide (void)
{
	if (in_live_game)
	{
		if (flag_test (FLAG_JACKPOT_LIT) || frenzy_lit_p ())
			return OPEN;
		else
			return CLOSED;
	}
	else
	{
		return CLOSED;
	}
}


static void trap_door_pulse (void)
{
	if (trap_door_state == CLOSED)
	{
		sol_request (SOL_TRAP_DOOR_CLOSE);
		if (in_live_game && valid_playfield)
			sample_start (SND_BOOM1, SL_1S);
	}
	else
		sol_request (SOL_TRAP_DOOR_OPEN);

	task_sleep (TIME_500MS);
	/* ODO - read 'closed' sw. to see if it worked OK */
	task_exit ();
}


CALLSET_ENTRY (trapdoor, device_update)
{
	enum trap_door_state decision = trap_door_decide ();
	if (decision != trap_door_state && trap_door_state != DISABLED)
	{
		trap_door_state = decision;
		task_recreate_gid (GID_TRAP_DOOR_PULSE, trap_door_pulse);
	}
}


CALLSET_ENTRY (trapdoor, ball_search)
{
	trap_door_state = DISABLED;
	sol_request (SOL_TRAP_DOOR_CLOSE);
	task_sleep_sec (2);
	sol_request (SOL_TRAP_DOOR_OPEN);
	task_sleep_sec (2);
	trap_door_state = OPEN;
	trapdoor_device_update ();
}


CALLSET_ENTRY (trapdoor, start_player)
{
	trap_door_bonuses = 0;
}

