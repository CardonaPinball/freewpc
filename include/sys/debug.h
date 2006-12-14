/*
 * Copyright 2006 by Brian Dominy <brian@oddchange.com>
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

#ifndef _SYS_DEBUG_H
#define _SYS_DEBUG_H

/* Include ASIC address definitions */
#include <wpc.h>

extern inline U8 wpc_debug_get_status (void)
{
	return wpc_asic_read (WPC_DEBUG_CONTROL_PORT);
}

extern inline U8 wpc_debug_write_ready (void)
{
	return wpc_debug_get_status () & 0x1;
}

extern inline U8 wpc_debug_read_ready (void)
{
	return wpc_debug_get_status () & 0x2;
}

extern inline void wpc_debug_write (U8 data)
{
	wpc_asic_write (WPC_DEBUG_DATA_PORT, data);
}

extern inline U8 wpc_debug_read (void)
{
	return wpc_asic_read (WPC_DEBUG_DATA_PORT);
}

#pragma GCC poison db_puti db_putc

enum wpc_debugger_request {
	WPC_DBREQ_TASK_DUMP=0x80,
};

enum wpc_debugger_response {
	WPC_DBRSP_CYCLECOUNT=0x80,
	WPC_DBRSP_STACK_POINTER,
	WPC_DBRSP_TASK_CREATE,
	WPC_DBRSP_IDLE_RUN,
};

void db_init (void);
void db_dump_switches (void);
void db_idle_task (void);
void msgbox (void);

#endif /* _SYS_DEBUG_H */
