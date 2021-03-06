/*
 * demolition man
 * jackpots.c
 *
 * written by James Cardona
 *
 *
 */


#include <freewpc.h>
#include "dm/global_constants.h"



//constants
const U8 lramp_mask = 0x1;
const U8 rramp_mask = 0x2;
const U8 sramp_mask = 0x4;
const U8 cramp_mask = 0x8;
const U8 lloop_mask = 0x10; //since this is hex, 10h = 16d
const U8 rloop_mask = 0x20; //since this is hex, 20h = 32d
const U8 ugrnd_mask = 0x40; //since this is hex, 40h = 64d

//local variables

//external variables

//prototypes

/****************************************************************************
 * initialize  and exit
 ***************************************************************************/
void jackpot_reset (void) {
	lramp_jackpot_light_off();
	rramp_jackpot_light_off();
	sramp_jackpot_light_off();
	cramp_jackpot_light_off();
	underground_jackpot_light_off();
	ll_jackpot_light_off();
	rl_jackpot_light_off();
	end_super_jackpot_reminder();
}//end of function



CALLSET_ENTRY (jackpot, start_player, start_ball) 		{ jackpot_reset(); }





/****************************************************************************
 * body
 *
 ****************************************************************************/
void score_jackpot(void) {
	if 		(flag_test(FLAG_IS_FORTRESS_MB_RUNNING) )  		fortress_jackpot_made();
	else if (flag_test(FLAG_IS_MUSEUM_MB_RUNNING) )  		museum_jackpot_made();
	else if (flag_test(FLAG_IS_CRYOPRISON_MB_RUNNING) )  	cryoprison_jackpot_made();
	else if (flag_test(FLAG_IS_WASTELAND_MB_RUNNING) )  	wasteland_jackpot_made();
	else if (flag_test(FLAG_IS_DEMOTIME_RUNNING) )  		demotime_jackpot_made();
}//END OF FUNCTION



void score_super_jackpot (void) {
	if 		(flag_test(FLAG_IS_FORTRESS_MB_RUNNING) )  		fortress_award_super_jackpot();
	else if (flag_test(FLAG_IS_MUSEUM_MB_RUNNING) )  		museum_award_super_jackpot();
	else if (flag_test(FLAG_IS_CRYOPRISON_MB_RUNNING) )  	cryoprison_award_super_jackpot();
	else if (flag_test(FLAG_IS_WASTELAND_MB_RUNNING) )  	wasteland_award_super_jackpot();

	end_super_jackpot_reminder();
}//end of function




void choose_single_jackpot(void) {
	U8 random_chooser;
	random_chooser = random_scaled(6); // choose random # from 0 to 6
	U8 mask = 1;//make a 1 bit flag so that only 1 jackpot is lit
	mask <<= random_chooser;//slide the bit over a random # of times - could be 0
	//turn on the appropriate jackpot
	//TODO: handle jackpots already picked
	if (lramp_mask & mask) lramp_jackpot_light_on();
	if (rramp_mask & mask) rramp_jackpot_light_on();
	if (sramp_mask & mask) sramp_jackpot_light_on();
	if (cramp_mask & mask) cramp_jackpot_light_on();
	if (lloop_mask & mask) ll_jackpot_light_on();
	if (rloop_mask & mask) rl_jackpot_light_on();
	if (ugrnd_mask & mask) underground_jackpot_light_on();
}//end of function



void choose_random_jackpot(void) {
	jackpot_reset ();
	task_sleep (TIME_33MS);
	choose_single_jackpot();
	all_arrow_update(); // at arrow_handler.c
}//end of function



//the most we will choose here is 3
void choose_multiple_random_jackpot(U8 numOfJackpots) {
	jackpot_reset ();
	task_sleep (TIME_33MS);
	if (numOfJackpots > 5) set_all_jackpots ();
	else {
		U8 i;
		for (i = 0; i < numOfJackpots; i++) {
			choose_single_jackpot();
		}//end of loop
		all_arrow_update(); // at arrow_handler.c
	}//end of else
}//end of function



//this only used for wizard modes  --- CURRENTLY ONLY DEMO TIME
void set_all_jackpots (void) {
	lramp_jackpot_light_on();
	rramp_jackpot_light_on();
	sramp_jackpot_light_on();
	cramp_jackpot_light_on();
	ll_jackpot_light_on();
	rl_jackpot_light_on();
	underground_jackpot_light_on();
	all_arrow_update(); // at arrow_handler.c
}//end of function





