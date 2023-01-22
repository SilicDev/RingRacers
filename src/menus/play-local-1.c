/// \file  menus/play-local-1.c
/// \brief Local Play, gamemode selection menu

#include "../k_menu.h"
#include "../m_cond.h" // Condition Sets

menuitem_t PLAY_GamemodesMenu[] =
{
	{IT_STRING | IT_CALL, "Race", "A contest to see who's the fastest of them all!",
		NULL, {.routine = M_SetupRaceMenu}, 0, 0},

	{IT_STRING | IT_CALL, "Battle", "It's last kart standing in this free-for-all!",
		"MENIMG00", {.routine = M_LevelSelectInit}, 0, GT_BATTLE},

	{IT_STRING | IT_CALL, "Capsules", "Bust up all of the capsules in record time!",
		NULL, {.routine = M_LevelSelectInit}, 1, GT_BATTLE},

	{IT_STRING | IT_CALL, "Special", "Strike your target and secure the prize!",
		NULL, {.routine = M_LevelSelectInit}, 1, GT_SPECIAL},

	{IT_STRING | IT_CALL, "Back", NULL, NULL, {.routine = M_GoBack}, 0, 0},
};

menu_t PLAY_GamemodesDef = KARTGAMEMODEMENU(PLAY_GamemodesMenu, &PLAY_MainDef);

void M_SetupGametypeMenu(INT32 choice)
{
	(void)choice;

	PLAY_GamemodesDef.prevMenu = currentMenu;

	// Battle and Capsules (and Special) disabled
	PLAY_GamemodesMenu[1].status = IT_DISABLED;
	PLAY_GamemodesMenu[2].status = IT_DISABLED;
	PLAY_GamemodesMenu[3].status = IT_DISABLED;

	if (cv_splitplayers.value > 1)
	{
		// Re-add Battle
		PLAY_GamemodesMenu[1].status = IT_STRING | IT_CALL;
	}
	else
	{
		boolean anyunlocked = false;

		if (M_SecretUnlocked(SECRET_BREAKTHECAPSULES, true))
		{
			// Re-add Capsules
			PLAY_GamemodesMenu[2].status = IT_STRING | IT_CALL;
			anyunlocked = true;
		}

		if (M_SecretUnlocked(SECRET_SPECIALATTACK, true))
		{
			// Re-add Special
			PLAY_GamemodesMenu[3].status = IT_STRING | IT_CALL;
			anyunlocked = true;
		}

		if (!anyunlocked)
		{
			// Only one non-Back entry, let's skip straight to Race.
			M_SetupRaceMenu(-1);
			return;
		}
	}

	M_SetupNextMenu(&PLAY_GamemodesDef, false);
}