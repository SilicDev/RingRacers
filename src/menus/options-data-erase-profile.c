/// \file  menus/options-data-erase-profile.c
/// \brief Erase Profile Menu

#include "../k_menu.h"
#include "../s_sound.h"
#include "../f_finale.h"

menuitem_t OPTIONS_DataProfileErase[] =
{
	{IT_NOTHING | IT_KEYHANDLER, NULL, NULL, NULL, {.routine = M_HandleProfileErase}, 0, 0},
};

menu_t OPTIONS_DataProfileEraseDef = {
	sizeof (OPTIONS_DataProfileErase) / sizeof (menuitem_t),
	&OPTIONS_DataEraseDef,
	0,
	OPTIONS_DataProfileErase,
	48, 80,
	SKINCOLOR_BLUEBERRY, 0,
	2, 5,
	M_DrawProfileErase,
	M_OptionsTick,
	NULL,
	NULL,
	NULL
};

// Check if we have any profile loaded.
void M_CheckProfileData(INT32 choice)
{
	UINT8 np = PR_GetNumProfiles();
	(void) choice;

	if (np < 2)
	{
		S_StartSound(NULL, sfx_s3k7b);
		M_StartMessage("There are no custom profiles.\n\nPress (B)", NULL, MM_NOTHING);
		return;
	}

	optionsmenu.eraseprofilen = 1;
	M_SetupNextMenu(&OPTIONS_DataProfileEraseDef, false);
}

static void M_EraseProfileResponse(INT32 choice)
{
	if (choice == MA_YES)
	{
		S_StartSound(NULL, sfx_itrole); // bweh heh heh

		PR_DeleteProfile(optionsmenu.eraseprofilen);

		// Did we bust our current profile..!?
		if (cv_currprofile.value == -1)
		{
			F_StartIntro();
			M_ClearMenus(true);
		}
		else if (optionsmenu.eraseprofilen > PR_GetNumProfiles()-1)
		{
			optionsmenu.eraseprofilen--;
		}
	}
}

void M_HandleProfileErase(INT32 choice)
{
	const UINT8 pid = 0;
	const UINT8 np = PR_GetNumProfiles()-1;
	(void) choice;

	if (menucmd[pid].dpad_ud > 0)
	{
		S_StartSound(NULL, sfx_s3k5b);
		optionsmenu.eraseprofilen++;

		if (optionsmenu.eraseprofilen > np)
			optionsmenu.eraseprofilen = 1;

		M_SetMenuDelay(pid);
	}
	else if (menucmd[pid].dpad_ud < 0)
	{
		S_StartSound(NULL, sfx_s3k5b);

		if (optionsmenu.eraseprofilen == 1)
			optionsmenu.eraseprofilen = np;
		else
			optionsmenu.eraseprofilen--;

		M_SetMenuDelay(pid);
	}
	else if (M_MenuBackPressed(pid))
	{
		M_GoBack(0);
		M_SetMenuDelay(pid);
	}
	else if (M_MenuConfirmPressed(pid))
	{
		if (optionsmenu.eraseprofilen == cv_currprofile.value)
			M_StartMessage("Your ""\x85""current profile""\x80"" will be erased.\nAre you sure you want to proceed?\nDeleting this profile will also\nreturn you to the title screen.\n\nPress (A) to confirm or (B) to cancel", FUNCPTRCAST(M_EraseProfileResponse), MM_YESNO);
		else
			M_StartMessage("This profile will be erased.\nAre you sure you want to proceed?\n\nPress (A) to confirm or (B) to cancel", FUNCPTRCAST(M_EraseProfileResponse), MM_YESNO);

		M_SetMenuDelay(pid);
	}
}