/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "extdll.h"
#include "basemenu.h"
#include "Bitmap.h"
#include "PicButton.h"
#include "YesNoMessageBox.h"
#include "utils.h"
#include "keydefs.h"
#include "menu_btnsbmp_table.h"
#include "menu_strings.h"

#define ART_BANNER		"gfx/shell/head_newgame"

class CMenuNewGame : public CMenuFramework
{
public:
	virtual const char *Key( int key, int down );

private:
	virtual void _Init();
	virtual void _VidInit();

	static void StartGameCb( CMenuBaseItem *pSelf, void *pExtra );
	static void PromptDialogCb( CMenuBaseItem *pSelf, void *pExtra );

	CMenuBackgroundBitmap background;
	CMenuBannerBitmap     banner;
	CMenuPicButton        easy;
	CMenuPicButton        medium;
	CMenuPicButton        hard;
	CMenuPicButton        cancel;

	CMenuYesNoMessageBox  msgBox;

	float skill;
};

static CMenuNewGame	uiNewGame;

/*
=================
CMenuNewGame::StartGame
=================
*/
void CMenuNewGame::StartGameCb( CMenuBaseItem *pSelf, void *pExtra )
{
	CMenuNewGame *parent = (CMenuNewGame *)pSelf->Parent();

	if( EngFuncs::GetCvarFloat( "host_serverstate" ) && EngFuncs::GetCvarFloat( "maxplayers" ) > 1 )
		EngFuncs::HostEndGame( "end of the game" );

	EngFuncs::CvarSetValue( "skill", parent->skill );
	EngFuncs::CvarSetValue( "deathmatch", 0.0f );
	EngFuncs::CvarSetValue( "teamplay", 0.0f );
	EngFuncs::CvarSetValue( "pausable", 1.0f ); // singleplayer is always allowing pause
	EngFuncs::CvarSetValue( "maxplayers", 1.0f );
	EngFuncs::CvarSetValue( "coop", 0.0f );

	EngFuncs::PlayBackgroundTrack( NULL, NULL );

	EngFuncs::ClientCmd( FALSE, "newgame\n" );
}

void CMenuNewGame::PromptDialogCb( CMenuBaseItem *pSelf, void *pExtra )
{
	CMenuNewGame *parent = (CMenuNewGame *)pSelf->Parent();

	if ( !CL_IsActive() )
	{
		StartGameCb( pSelf, pExtra );
		return;
	}

	parent->skill = (int)pExtra;

	parent->easy.Flags() ^= QMF_INACTIVE;
	parent->medium.Flags() ^= QMF_INACTIVE;
	parent->hard.Flags() ^= QMF_INACTIVE;
	parent->cancel.Flags() ^= QMF_INACTIVE;

	parent->msgBox.ToggleVisibility();
}

/*
=================
CMenuNewGame::Key
=================
*/
const char *CMenuNewGame::Key( int key, int down )
{
	if( down && key == K_ESCAPE && !( msgBox.Flags() & QMF_HIDDEN ))
	{
		skill = 0;	// clear skill
		return uiSoundNull;
	}
	return CMenuFramework::Key( key, down );
}

/*
=================
CMenuNewGame::Init
=================
*/
void CMenuNewGame::_Init( void )
{
	// memset( &uiNewGame, 0, sizeof( CMenuNewGame ));

	banner.SetPicture( ART_BANNER );
	
	easy.SetNameAndStatus( "Easy", MenuStrings[HINT_SKILL_EASY] );
	easy.SetPicture( PC_EASY );
	easy.EventCallbackExtra() = (void*)1;
	easy.onActivated = PromptDialogCb;

	medium.SetNameAndStatus( "Medium", MenuStrings[HINT_SKILL_NORMAL] );
	medium.SetPicture( PC_MEDIUM );
	medium.EventCallbackExtra() = (void*)2;
	medium.onActivated = PromptDialogCb;

	hard.SetNameAndStatus( "Difficult", MenuStrings[HINT_SKILL_HARD] );
	hard.SetPicture( PC_DIFFICULT );
	hard.EventCallbackExtra() = (void*)3;
	hard.onActivated = PromptDialogCb;

	cancel.SetNameAndStatus("Cancel", "Go back to the main menu");
	cancel.SetPicture( PC_CANCEL );
	cancel.onActivated = PopMenuCb;

	msgBox.SetMessage( MenuStrings[HINT_RESTART_GAME] );
	msgBox.SetPositiveButton( "Ok", PC_OK );
	msgBox.SetNegativeButton( "Cancel", PC_CANCEL );
	msgBox.onPositive = StartGameCb;

	AddItem( background );
	AddItem( banner );
	AddItem( easy );
	AddItem( medium );
	AddItem( hard );
	AddItem( cancel );
	AddItem( msgBox );
}

void CMenuNewGame::_VidInit( void )
{
	easy.SetCoord( 72, 230 );
	medium.SetCoord( 72, 280 );
	hard.SetCoord( 72, 330 );
	cancel.SetCoord( 72, 380 );
}

/*
=================
UI_NewGame_Precache
=================
*/
void UI_NewGame_Precache( void )
{
	EngFuncs::PIC_Load( ART_BACKGROUND );
	EngFuncs::PIC_Load( ART_BANNER );
}

/*
=================
UI_NewGame_Menu
=================
*/
void UI_NewGame_Menu( void )
{
	// completely ignore save\load menus for multiplayer_only
	if( gMenu.m_gameinfo.gamemode == GAME_MULTIPLAYER_ONLY || !EngFuncs::CheckGameDll() )
		return;

	if( !uiNewGame.WasInit() )
		uiNewGame.Init();
	uiNewGame.VidInit();

	uiNewGame.PushMenu( );
}
