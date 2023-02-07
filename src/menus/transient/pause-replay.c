/// \file  menus/transient/pause-replay.c
/// \brief Replay popup menu

#include "../../k_menu.h"
#include "../../s_sound.h"
#include "../../p_tick.h" // leveltime
#include "../../i_time.h"
#include "../../r_main.h" // R_ExecuteSetViewSize
#include "../../p_local.h" // P_InitCameraCmd
#include "../../d_main.h" // D_StartTitle

static void M_PlaybackTick(void);

menuitem_t PAUSE_PlaybackMenu[] =
{
	{IT_CALL   | IT_STRING, "Hide Menu",			NULL, "M_PHIDE",	{.routine = M_SelectableClearMenus},	  0, 0},

	{IT_CALL   | IT_STRING, "Rewind",				NULL, "M_PREW",		{.routine = M_PlaybackRewind},			 20, 0},
	{IT_CALL   | IT_STRING, "Pause",				NULL, "M_PPAUSE",	{.routine = M_PlaybackPause},			 36, 0},
	{IT_CALL   | IT_STRING, "Fast-Forward",			NULL, "M_PFFWD",	{.routine = M_PlaybackFastForward},		 52, 0},
	{IT_CALL   | IT_STRING, "Backup Frame",			NULL, "M_PSTEPB",	{.routine = M_PlaybackRewind},			 20, 0},
	{IT_CALL   | IT_STRING, "Resume",				NULL, "M_PRESUM",	{.routine = M_PlaybackPause},			 36, 0},
	{IT_CALL   | IT_STRING, "Advance Frame",		NULL, "M_PFADV",	{.routine = M_PlaybackAdvance},			 52, 0},

	{IT_ARROWS | IT_STRING, "View Count",			NULL, "M_PVIEWS",	{.routine = M_PlaybackSetViews},		 72, 0},
	{IT_ARROWS | IT_STRING, "Viewpoint",			NULL, "M_PNVIEW",	{.routine = M_PlaybackAdjustView},		 88, 0},
	{IT_ARROWS | IT_STRING, "Viewpoint 2",			NULL, "M_PNVIEW",	{.routine = M_PlaybackAdjustView},		104, 0},
	{IT_ARROWS | IT_STRING, "Viewpoint 3",			NULL, "M_PNVIEW",	{.routine = M_PlaybackAdjustView},		120, 0},
	{IT_ARROWS | IT_STRING, "Viewpoint 4",			NULL, "M_PNVIEW",	{.routine = M_PlaybackAdjustView},		136, 0},

	{IT_CALL   | IT_STRING, "Toggle Free Camera",	NULL, "M_PVIEWS",	{.routine = M_PlaybackToggleFreecam},	156, 0},
	{IT_CALL   | IT_STRING, "Stop Playback",		NULL, "M_PEXIT",	{.routine = M_PlaybackQuit},			172, 0},
};

menu_t PAUSE_PlaybackMenuDef = {
	sizeof (PAUSE_PlaybackMenu) / sizeof (menuitem_t),
	NULL,
	0,
	PAUSE_PlaybackMenu,
	BASEVIDWIDTH/2 - 88, 2,
	0, 0,
	NULL,
	0, 0,
	M_DrawPlaybackMenu,
	M_PlaybackTick,
	NULL,
	NULL,
	NULL
};

void M_EndModeAttackRun(void)
{
	G_CheckDemoStatus(); // Cancel recording

	Command_ExitGame_f(); // Clear a bunch of state

	modeattacking = ATTACKING_NONE; // Kept until now because of Command_ExitGame_f

	if (demo.title == true)
	{
		D_StartTitle();
	}
	else
	{
		D_ClearState();
		M_StartControlPanel();
	}
}

// Replay Playback Menu

tic_t playback_last_menu_interaction_leveltime = 0;

static void M_PlaybackTick(void)
{
	INT16 i;

	if (leveltime - playback_last_menu_interaction_leveltime >= 6*TICRATE)
		playback_last_menu_interaction_leveltime = leveltime - 6*TICRATE;

	// Toggle items
	if (paused && !demo.rewinding)
	{
		PAUSE_PlaybackMenu[playback_pause].status = PAUSE_PlaybackMenu[playback_fastforward].status = PAUSE_PlaybackMenu[playback_rewind].status = IT_DISABLED;
		PAUSE_PlaybackMenu[playback_resume].status = PAUSE_PlaybackMenu[playback_advanceframe].status = PAUSE_PlaybackMenu[playback_backframe].status = IT_CALL|IT_STRING;

		if (itemOn >= playback_rewind && itemOn <= playback_fastforward)
			itemOn += playback_backframe - playback_rewind;
	}
	else
	{
		PAUSE_PlaybackMenu[playback_pause].status = PAUSE_PlaybackMenu[playback_fastforward].status = PAUSE_PlaybackMenu[playback_rewind].status = IT_CALL|IT_STRING;
		PAUSE_PlaybackMenu[playback_resume].status = PAUSE_PlaybackMenu[playback_advanceframe].status = PAUSE_PlaybackMenu[playback_backframe].status = IT_DISABLED;

		if (itemOn >= playback_backframe && itemOn <= playback_advanceframe)
			itemOn -= playback_backframe - playback_rewind;
	}

	if (modeattacking)
	{
		for (i = playback_viewcount; i <= playback_view4; i++)
			PAUSE_PlaybackMenu[i].status = IT_DISABLED;

		//PAUSE_PlaybackMenu[playback_moreoptions].mvar1 = 72;
		//PAUSE_PlaybackMenu[playback_quit].mvar1 = 88;
		PAUSE_PlaybackMenu[playback_quit].mvar1 = 72;

		//currentMenu->x = BASEVIDWIDTH/2 - 52;
		currentMenu->x = BASEVIDWIDTH/2 - 44;
	}
	else
	{
		PAUSE_PlaybackMenu[playback_viewcount].status = IT_ARROWS|IT_STRING;

		for (i = 0; i <= r_splitscreen; i++)
			PAUSE_PlaybackMenu[playback_view1+i].status = IT_ARROWS|IT_STRING;
		for (i = r_splitscreen+1; i < 4; i++)
			PAUSE_PlaybackMenu[playback_view1+i].status = IT_DISABLED;

		//PAUSE_PlaybackMenu[playback_moreoptions].mvar1 = 156;
		//PAUSE_PlaybackMenu[playback_quit].mvar1 = 172;
		PAUSE_PlaybackMenu[playback_quit].mvar1 = 156;

		//currentMenu->x = BASEVIDWIDTH/2 - 94;
		currentMenu->x = BASEVIDWIDTH/2 - 88;
	}
}

void M_SetPlaybackMenuPointer(void)
{
	itemOn = playback_pause;
}

void M_PlaybackRewind(INT32 choice)
{
	static tic_t lastconfirmtime;

	(void)choice;

	if (!demo.rewinding)
	{
		if (paused)
		{
			G_ConfirmRewind(leveltime-1);
			paused = true;
			S_PauseAudio();
		}
		else
			demo.rewinding = paused = true;
	}
	else if (lastconfirmtime + TICRATE/2 < I_GetTime())
	{
		lastconfirmtime = I_GetTime();
		G_ConfirmRewind(leveltime);
	}

	CV_SetValue(&cv_playbackspeed, 1);
}

void M_PlaybackPause(INT32 choice)
{
	(void)choice;

	paused = !paused;

	if (demo.rewinding)
	{
		G_ConfirmRewind(leveltime);
		paused = true;
		S_PauseAudio();
	}
	else if (paused)
		S_PauseAudio();
	else
		S_ResumeAudio();

	CV_SetValue(&cv_playbackspeed, 1);
}

void M_PlaybackFastForward(INT32 choice)
{
	(void)choice;

	if (demo.rewinding)
	{
		G_ConfirmRewind(leveltime);
		paused = false;
		S_ResumeAudio();
	}
	CV_SetValue(&cv_playbackspeed, cv_playbackspeed.value == 1 ? 4 : 1);
}

void M_PlaybackAdvance(INT32 choice)
{
	(void)choice;

	paused = false;
	TryRunTics(1);
	paused = true;
}

void M_PlaybackSetViews(INT32 choice)
{
	if (choice > 0)
	{
		if (r_splitscreen < 3)
			G_AdjustView(r_splitscreen + 2, 0, true);
	}
	else if (r_splitscreen)
	{
		if (choice == 0)
		{
			r_splitscreen--;
		}
		else
		{
			r_splitscreen = 0;
		}
		R_ExecuteSetViewSize();
	}
}

void M_PlaybackAdjustView(INT32 choice)
{
	G_AdjustView(itemOn - playback_viewcount, (choice > 0) ? 1 : -1, true);
}

// this one's rather tricky
void M_PlaybackToggleFreecam(INT32 choice)
{
	(void)choice;
	M_ClearMenus(true);

	// remove splitscreen:
	splitscreen = 0;
	R_ExecuteSetViewSize();

	P_InitCameraCmd();	// init camera controls
	if (!demo.freecam)	// toggle on
	{
		demo.freecam = true;
		democam.cam = &camera[0];	// this is rather useful
	}
	else	// toggle off
	{
		demo.freecam = false;
		// reset democam vars:
		democam.cam = NULL;
		//democam.turnheld = false;
		democam.keyboardlook = false;	// reset only these. localangle / aiming gets set before the cam does anything anyway
	}
}

void M_PlaybackQuit(INT32 choice)
{
	(void)choice;
	G_StopDemo();

	if (demo.inreplayhut)
		M_StartControlPanel();
	else if (modeattacking)
		M_EndModeAttackRun();
	else
		D_StartTitle();
}
