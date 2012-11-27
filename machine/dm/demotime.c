/*
 * demolition man
 * demotime.c
 * 
 * written by James Cardona
 *
 * demotime multiball
 *
 *
 * */
/* CALLSET_SECTION (demotime, __machine2__) */


#include <freewpc.h>
#include "dm/global_constants.h"
#include <diverter.h> //autogenerated by divhold.ct

//constants
const U8	DEMOTIME_EASY_GOAL = 5;
const U8	DEMOTIME_GOAL_INCREMENT = 3;

//local variables
__boolean	demotime_start_music;
score_t		demotime_score;
U8			demotime_jackpot_shots_made;
U8			demotime_MessageCounter;
U8			demotime_counter;
U8			demotime_goal;


//external variables
extern U8			NumBallsFrozen; //from lock_freeze_mbstart.c

//internally called function prototypes  --external found at protos.h
void demotime_player_reset (void);



/****************************************************************************
 * multiball definition structure
 ***************************************************************************/
struct mb_mode_ops demotime_mode = {
	DEFAULT_MBMODE,
	//.update = , /* The update callback is invoked whenever the state of the multiball changes. */
	.music = MUS_MB_B,
	.deff_starting = DEFF_DEMOTIME_START_EFFECT,
	.deff_running = DEFF_DEMOTIME_EFFECT,
	//.deff_ending = ,
//.active_task = 				//	default => .active_task = mb_mode_active_task,
	.prio = PRI_MULTIBALL,		//default => .prio = PRI_NULL,
//.grace_period = 				//default => .grace_period = 500ms
};
//task_gid_t gid_running;
//task_gid_t gid_in_grace;



/****************************************************************************
 * initialize  and exit
 ***************************************************************************/
void demotime_player_reset (void) {
	demotime_goal = DEMOTIME_EASY_GOAL;
	flag_off(FLAG_IS_DEMOTIME_ENABLED);
	flag_off(FLAG_IS_DEMOTIME_RUNNING);
	score_zero(demotime_score);
	demotime_jackpot_shots_made = 0; //these need to be zeroed in before we enter the mode so bonus doesn't fake trigger
	demotime_start_music = FALSE;
	lamp_tristate_off (LM_DEMO_TIME);
}//end of function



CALLSET_ENTRY (demotime, start_player) { demotime_player_reset(); }



/****************************************************************************
 * external event listeners
 ****************************************************************************/
CALLSET_ENTRY (demotime, music_refresh)  {
	if (demotime_start_music)				music_request (LOW_HORN, PRI_GAME_QUICK8);
	else 									mb_mode_music_refresh (&demotime_mode);
}//end of function



CALLSET_ENTRY (demotime, display_update) { mb_mode_display_update (&demotime_mode); }



CALLSET_ENTRY (demotime, end_ball) {
	if (flag_test(FLAG_IS_DEMOTIME_RUNNING)) {
		mb_mode_end_ball (&demotime_mode);
		jackpot_reset();
		lamp_tristate_off (LM_DEMO_TIME);
		flag_off(FLAG_IS_DEMOTIME_RUNNING);
		flag_off(FLAG_IS_DEMOTIME_ENABLED);
		if (flag_test(FLAG_IS_R_RAMP_CLAWREADY) ) 	rramp_clawready_on();
		else										rramp_clawready_off();
	}
}//end of function



//puts in grace period if set, otherwise ends mode
CALLSET_ENTRY (demotime, single_ball_play) {
	if (flag_test(FLAG_IS_DEMOTIME_RUNNING)) {
		mb_mode_end_ball (&demotime_mode);
		jackpot_reset();
		lamp_tristate_off (LM_DEMO_TIME);
		flag_off(FLAG_IS_DEMOTIME_RUNNING);
		flag_off(FLAG_IS_DEMOTIME_ENABLED);
		if (flag_test(FLAG_IS_R_RAMP_CLAWREADY) ) 	rramp_clawready_on();
		else										rramp_clawready_off();
	}//end of if
}//end of function




/****************************************************************************
 * body
 *
 ***************************************************************************/
//MAKING ANY OF THE CLAW MODE SHOTS CALLS THIS
void demotime_increment (void) {
	if (++demotime_counter >= demotime_goal) {
		flag_on(FLAG_IS_DEMOTIME_ENABLED);
		deff_start (DEFF_DEMOTIME_INFO_EFFECT);//under /kernel/deff.c
		lamp_tristate_flash (LM_DEMO_TIME);
	}
}//end of function



//ONCE ENABLED IT IS STARTED BY A UNDERGROUND SHOT
void demotime_start(void) {
	demotime_start_music = TRUE;
	flag_on(FLAG_IS_DEMOTIME_RUNNING);
	flag_off(FLAG_IS_DEMOTIME_ENABLED);
	mb_mode_start(&demotime_mode);
	diverter_stop();//defined in divhold2.ct

	//LIGHTS
	lamp_tristate_on (LM_DEMO_TIME);

	//reset all start criteria for next time
	demotime_goal += (DEMOTIME_GOAL_INCREMENT);
	demotime_counter = 0;

	set_all_jackpots();
	//serve balls
//	U8 i;
//	for (i = 1; i < 5; i++) {
		serve_ball_auto(); //add one ball to the playfield - NOT technically a multiball since doesn't change global ball count
	//	set_ball_count (2);  //this also works unless the number is >2 then set_ball_count_task at serve.c breaks horribly
//		task_sleep (TIME_1S + TIME_500MS);
//	}//end of loop
}//end of function



//jackpot shot
void demotime_jackpot_made(void) {
	score_add (demotime_score, score_table[DEMOTIME_MB_SCORE]);
	score (DEMOTIME_MB_SCORE);
	deff_start (DEFF_DEMOTIME_JACKPOT_EFFECT);
}//end of function



/****************************************************************************
 * DMD display and sound effects
 ****************************************************************************/
void demotime_animation_display_effect (U16 start_frame, U16 end_frame){
	U16 fno;
	for (fno = start_frame; fno <= end_frame; fno += 2) {
		dmd_alloc_pair ();
		frame_draw(fno);
		dmd_show2 ();
		task_sleep (TIME_100MS);
	}//end of inner loop
}



void demotime_frame_bitfade_fast (U16 frame){
	dmd_sched_transition (&trans_bitfade_fast);
	dmd_alloc_pair ();
	frame_draw(frame);
	dmd_show2 ();
	task_sleep (TIME_100MS);
}




void demotime_frame_with_words_display_steel_effect (U16 frame, U8 x, U8 y, char *words){
	dmd_alloc_pair_clean ();// Clean both pages
	dmd_map_overlay ();
	dmd_clean_page_low ();
	font_render_string_center (&font_steel, x, y, words);
	dmd_text_outline ();
	dmd_alloc_pair ();
	frame_draw(frame);
	dmd_overlay_outline ();
	dmd_show2 ();
	task_sleep (TIME_100MS);
}



void demotime_info_effect_deff(void) {
	dmd_clean_page_high ();
	dmd_clean_page_low ();
	font_render_string_center (&font_var5, DMD_MIDDLE_X, DMD_SMALL_CY_1, "SHOOT SUBWAY");
	font_render_string_center (&font_var5, DMD_MIDDLE_X, DMD_SMALL_CY_2, "FOR");

	font_render_string_center (&font_fipps, DMD_MIDDLE_X, DMD_BIG_CY_Bot, "DEMOLITION TIME");
	dmd_show_low();
	task_sleep_sec (4);
	deff_exit ();
}//end of mode_effect_deff




void demotime_start_effect_deff(void) {
	U16 fno;
	sound_start (ST_SAMPLE, EXPLOSION, SL_2S, PRI_GAME_QUICK5);
	sound_start (ST_SPEECH, SPCH_YOU_AND_ME, SL_4S, PRI_GAME_QUICK5);

	demotime_animation_display_effect (IMG_FIGHT_B_START, IMG_FIGHT_B_END);
	dmd_map_overlay ();
	dmd_clean_page_low ();
	font_render_string_center (&font_steel, DMD_MIDDLE_X - 20, DMD_BIG_CY_Top, "FORTRESS");
	dmd_text_outline ();
	dmd_alloc_pair ();
	frame_draw(IMG_FIGHT_B_END);
	dmd_overlay_outline ();
	dmd_show2 ();
	task_sleep (TIME_500MS);

	sound_start (ST_SAMPLE, EXPLOSION, SL_2S, PRI_GAME_QUICK5);

	demotime_frame_bitfade_fast(IMG_MUSEUM_C1_START);

	sound_start (ST_SPEECH, SPCH_MUST_HAVE_HURT, SL_4S, PRI_GAME_QUICK5);

	demotime_animation_display_effect (IMG_MUSEUM_C1_START, IMG_MUSEUM_C2_END);
	demotime_frame_bitfade_fast(IMG_MUSEUM_D2_START);
	demotime_animation_display_effect (IMG_MUSEUM_D2_START, IMG_MUSEUM_D2_END);
	demotime_frame_bitfade_fast(IMG_SIMON_D_START);
	demotime_animation_display_effect (IMG_SIMON_D_START, IMG_SIMON_D_END);
	dmd_map_overlay ();
	dmd_clean_page_low ();
	font_render_string_center (&font_steel, DMD_MIDDLE_X - 20, DMD_BIG_CY_Bot, "MUSEUM");
	dmd_text_outline ();
	dmd_alloc_pair ();
	frame_draw(IMG_SIMON_D_END);
	dmd_overlay_outline ();
	dmd_show2 ();
	task_sleep (TIME_500MS);

	sound_start (ST_SAMPLE, EXPLOSION, SL_2S, PRI_GAME_QUICK5);

	demotime_frame_bitfade_fast(IMG_WASTELAND_A1_START);

	sound_start (ST_SPEECH, SPCH_IN_THIS_CENTURY, SL_4S, PRI_GAME_QUICK5);

	demotime_animation_display_effect (IMG_WASTELAND_A1_START, IMG_WASTELAND_A1_END);
	demotime_frame_bitfade_fast(IMG_WASTELAND_A3_START);
	for (fno = IMG_WASTELAND_A3_START; fno <= IMG_WASTELAND_A3_END; fno += 2) {
		dmd_map_overlay ();
		dmd_clean_page_low ();
		font_render_string_center (&font_steel, DMD_MIDDLE_X + 10, DMD_BIG_CY_Bot, "WASTELAND");
		dmd_text_outline ();
		dmd_alloc_pair ();
		frame_draw(fno);
		dmd_overlay_outline ();
		dmd_show2 ();
		task_sleep (TIME_100MS);
	}//end of inner loop
	task_sleep (TIME_500MS);

	sound_start (ST_SAMPLE, EXPLOSION, SL_2S, PRI_GAME_QUICK5);

	demotime_frame_bitfade_fast(IMG_CAPSIMON_C_START);

	sound_start (ST_SPEECH, SPCH_JOHN_SPARTAN, SL_4S, PRI_GAME_QUICK5);

	for (fno = IMG_CAPSIMON_C_START; fno <= IMG_CAPSIMON_C_END; fno += 2) {
		dmd_map_overlay ();
		dmd_clean_page_low ();
		font_render_string_center (&font_steel, DMD_MIDDLE_X - 20, DMD_BIG_CY_Top, "CRYO");
		font_render_string_center (&font_steel, DMD_MIDDLE_X - 20, DMD_BIG_CY_Bot, "PRISON");
		dmd_text_outline ();
		dmd_alloc_pair ();
		frame_draw(fno);
		dmd_overlay_outline ();
		dmd_show2 ();
		task_sleep (TIME_100MS);
	}//end of inner loop
	task_sleep (TIME_500MS);

	sound_start (ST_SAMPLE, EXPLOSION, SL_2S, PRI_GAME_QUICK5);

	demotime_frame_bitfade_fast(IMG_JAIL_A_START);
	demotime_animation_display_effect (IMG_JAIL_A_START, IMG_JAIL_A_END);

	sound_start (ST_SPEECH, SPCH_DEMOLITION_MAN, SL_4S, PRI_GAME_QUICK5);

	dmd_map_overlay ();
	dmd_clean_page_low ();
	font_render_string_center (&font_steel, DMD_MIDDLE_X - 20, DMD_BIG_CY_Top, "DEMOLITION");
	font_render_string_center (&font_steel, DMD_MIDDLE_X - 20, DMD_BIG_CY_Bot, "TIME");
	dmd_text_outline ();
	dmd_alloc_pair ();
	frame_draw(IMG_JAIL_A_END);
	dmd_overlay_outline ();
	dmd_show2 ();
	task_sleep (TIME_500MS);

	demotime_start_music = FALSE; //kill the start music

	deff_exit ();
}//end of mode_effect_deff



void demotime_jackpot_sounds_task(void) {
	U8 			demotime_MessageCounter;
	demotime_MessageCounter = random_scaled(3);
	switch (demotime_MessageCounter) {
		case 0: 	sound_start (ST_SPEECH, SPCH_TRESPASSING, SL_2S, PRI_GAME_QUICK5); break;
		case 1: 	sound_start (ST_SPEECH, SPCH_ARREST, SL_2S, PRI_GAME_QUICK5); break;
		case 2: 	sound_start (ST_SPEECH, SPCH_DREAMIN_KILLING_YOU, SL_2S, PRI_GAME_QUICK5); break;
	}//end of switch
	task_sleep (TIME_500MS);
	sound_start (ST_EFFECT, GUNSHOT_MUFFLED, SL_1S, PRI_GAME_QUICK5);
	task_sleep (TIME_200MS);
	sound_start (ST_EFFECT, GUNSHOT_MUFFLED, SL_1S, PRI_GAME_QUICK5);
	task_sleep (TIME_200MS);
	sound_start (ST_EFFECT, GUNSHOT_MUFFLED, SL_1S, PRI_GAME_QUICK5);
	task_sleep (TIME_200MS);
	sound_start (ST_EFFECT, GUNSHOT, SL_2S, PRI_GAME_QUICK5);
	task_sleep (TIME_500MS);
	switch (demotime_MessageCounter) {
		case 0: 	sound_start (ST_SPEECH, SPCH_JOHN_SCREAM, SL_2S, PRI_GAME_QUICK5); break;
		case 1: 	sound_start (ST_SPEECH, SPCH_DONT_WORRY, SL_2S, PRI_GAME_QUICK5); break;
		case 2: 	sound_start (ST_SPEECH, SPCH_KEEP_DREAMING, SL_2S, PRI_GAME_QUICK5); break;
	}//end of switch
	task_exit();
}//end of mode_effect_deff



void demotime_jackpot_effect_deff(void) {
	U8 			demotime_MessageCounter;
	demotime_MessageCounter = random_scaled(8);
	dmd_clean_page_high ();//
	dmd_clean_page_low ();//

	task_create_gid1 (GID_DEMOTIME_JACKPOT_SOUND, demotime_jackpot_sounds_task);

	switch (demotime_MessageCounter) {
	//FROM FORTRESS
		default:
		case 0:
			demotime_animation_display_effect (IMG_FORTRESS_A_START, IMG_FORTRESS_A_END);
			demotime_frame_with_words_display_steel_effect (IMG_FORTRESS_A_END, DMD_MIDDLE_X + 20, DMD_BIG_CY_Top, "JACKPOT");
			break;
		case 1:
			demotime_animation_display_effect (IMG_FORTRESS_B_START, IMG_FORTRESS_B_END);
			demotime_frame_with_words_display_steel_effect (IMG_FORTRESS_B_END, DMD_MIDDLE_X + 20, DMD_BIG_CY_Top, "JACKPOT");
			break;

	//FROM MUSEUM
		case 2:
			demotime_animation_display_effect (IMG_MUSEUM_C1_START, IMG_MUSEUM_C2_END);
			demotime_frame_with_words_display_steel_effect (IMG_MUSEUM_C2_END, DMD_MIDDLE_X + 20, DMD_BIG_CY_Top, "JACKPOT");
			break;
		case 3:
			demotime_animation_display_effect (IMG_MUSEUM_D1_START, IMG_MUSEUM_D3_END);
			demotime_frame_with_words_display_steel_effect (IMG_MUSEUM_D3_END, DMD_MIDDLE_X + 20, DMD_BIG_CY_Top, "JACKPOT");
			break;

	//FROM CRYOPRISON
		case 4:
			demotime_animation_display_effect (IMG_CRYOPRISON_E1_START, IMG_CRYOPRISON_E2_END);
			demotime_frame_with_words_display_steel_effect (IMG_CRYOPRISON_E2_END, DMD_MIDDLE_X + 20, DMD_BIG_CY_Top, "JACKPOT");
			break;

	//FROM WASTELAND
		case 5:
			demotime_animation_display_effect (IMG_WASTELAND_B1_START, IMG_WASTELAND_B1_END);
			demotime_frame_with_words_display_steel_effect (IMG_WASTELAND_B1_END, DMD_MIDDLE_X + 20, DMD_BIG_CY_Top, "JACKPOT");
			break;
		case 6:
			demotime_animation_display_effect (IMG_WASTELAND_A1_START, IMG_WASTELAND_A2_END);
			demotime_frame_with_words_display_steel_effect (IMG_WASTELAND_A2_END, DMD_MIDDLE_X + 20, DMD_BIG_CY_Top, "JACKPOT");
			break;
		}//end of switch
	task_sleep_sec (1);
	deff_exit ();
}//end of mode_effect_deff




	//the image is 4 color in 2 panes, so we must increment by 2's not 1's
	U8 demotime_effect_deff_table[] = {	0, 2, 0, 2, 4, 2, 4, 2, 0, 0,
										2, 4, 6, 8, 6, 8, 4, 2, 0, 6,
										2, 6, 8, 4, 6, 8, 2, 0, 2, 0, 4, 2 };

	void demotime_effect_deff (void) {
		U8 demotime_display_counter = 0;
		__boolean TOGGLE = FALSE;

		dmd_alloc_pair_clean();

		for (;;) {
				dmd_map_overlay ();
				dmd_clean_page_low ();
				dmd_draw_thin_border (dmd_low_buffer);
				font_render_string_center (&font_steel, DMD_MIDDLE_X, DMD_BIG_CY_Top, "DEMOTIME");
				if (demotime_display_counter % 10 != 0) { //draw for 9/10 and blank for 1/10
							if (TOGGLE) 	sprintf ("15 MILLION");
							else 			sprintf ("JACKPOT LIT"); }
				else 						sprintf ("");

				font_render_string_center (&font_steel, DMD_MIDDLE_X, DMD_BIG_CY_Bot, sprintf_buffer);

				dmd_text_outline ();
				dmd_alloc_pair ();
				frame_draw(IMG_FORTRESS_RUN_0 + demotime_effect_deff_table[demotime_display_counter]);
				dmd_overlay_outline ();
				dmd_show2 ();
			task_sleep (TIME_200MS);

			demotime_display_counter++;
			if (demotime_display_counter >= 30) demotime_display_counter  = 0;
			if (demotime_display_counter % 10 == 0) { if (TOGGLE) TOGGLE = FALSE; else TOGGLE = TRUE; }//change TOGGLE once per second
		}//END OF ENDLESS LOOP
	}//end of function

