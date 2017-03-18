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
#include "YesNoMessageBox.h"
#include "utils.h"
#include "menu_btnsbmp_table.h"
#include "keydefs.h"

#define ART_BANNER			"gfx/shell/head_multi"

class CMenuMultiplayer : public CMenuFramework
{
public:
	virtual const char *Key(int key, int down);

private:
	virtual void _Init();
	virtual void _VidInit();
	static void PredictDialog( CMenuBaseItem *pSelf, void *pExtra );

	CMenuBackgroundBitmap	background;
	CMenuBannerBitmap		banner;

	CMenuPicButton	internetGames;
	CMenuPicButton	spectateGames;
	CMenuPicButton	LANGame;
	CMenuPicButton	Customize;	// playersetup
	CMenuPicButton	Controls;
	CMenuPicButton	done;

	// prompt dialog
	CMenuYesNoMessageBox msgBox;
};

static CMenuMultiplayer	uiMultiPlayer;


void CMenuMultiplayer::PredictDialog( CMenuBaseItem *pSelf, void *pExtra )
{
	CMenuMultiplayer *parent = (CMenuMultiplayer*)pSelf->Parent();

	// toggle main menu between active\inactive
	// show\hide remove dialog
	parent->internetGames.Flags() ^= QMF_INACTIVE;
	parent->spectateGames.Flags() ^= QMF_INACTIVE;
	parent->LANGame.Flags() ^= QMF_INACTIVE;
	parent->Customize.Flags() ^= QMF_INACTIVE;
	parent->Controls.Flags() ^= QMF_INACTIVE;
	parent->done.Flags() ^= QMF_INACTIVE;

	parent->msgBox.ToggleVisibility();
}


/*
=================
UI_Multiplayer_KeyFunc
=================
*/
const char *CMenuMultiplayer::Key( int key, int down )
{
	if( down && key == K_ESCAPE && !( msgBox.Flags() & QMF_HIDDEN ))
	{
		PredictDialog( &msgBox, NULL );
		return uiSoundNull;
	}
	return CMenuFramework::Key( key, down );
}


/*
=================
CMenuMultiplayer::Init
=================
*/
void CMenuMultiplayer::_Init( void )
{
	// memset( &uiMultiPlayer, 0, sizeof( CMenuMultiplayer ));

	banner.SetPicture( ART_BANNER );

	internetGames.Flags() = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_NOTIFY;
	internetGames.SetNameAndStatus( "Internet game", "View list of a game internet servers and join the one of your choice" );
	internetGames.SetPicture( PC_INET_GAME );
	SET_EVENT_VOID( internetGames, onActivated, UI_InternetGames_Menu );

	spectateGames.Flags() = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_NOTIFY|QMF_GRAYED;
	spectateGames.SetNameAndStatus( "Spectate game", "Spectate internet games" );
	spectateGames.SetPicture( PC_SPECTATE_GAMES );
	// TODO: implement
	// SET_EVENT_VOID( spectateGames, onActivated, UI_SpectateGames_Menu );

	LANGame.Flags() = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_NOTIFY;
	LANGame.SetNameAndStatus( "LAN game", "Set up the game on the local area network" );
	LANGame.SetPicture( PC_LAN_GAME );
	SET_EVENT_VOID( LANGame, onActivated, UI_LanGame_Menu );

	Customize.Flags() = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_NOTIFY;
	Customize.SetNameAndStatus( "Customize", "Choose your player name, and select visual options for your character" );
	Customize.SetPicture( PC_CUSTOMIZE );
	// SET_EVENT_VOID( Customize, onActivated, UI_PlayerSetup_Menu );

	Controls.Flags() = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_NOTIFY;
	Controls.SetNameAndStatus( "Controls", "Change keyboard and mouse settings" );
	Controls.SetPicture( PC_CONTROLS );
	// SET_EVENT_VOID( Controls, onActivated, UI_Controls_Menu );

	done.Flags() = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_NOTIFY;
	done.SetNameAndStatus( "Done", "Go back to the Main menu" );
	done.SetPicture( PC_DONE );
	done.onActivated = PopMenuCb;

	msgBox.SetMessage( "It is recomended to enable\nclient movement prediction\nPress OK to enable it now\nOr enable it later in\n^5(Multiplayer/Customize)");
	msgBox.SetPositiveButton( "Ok", PC_OK );
	msgBox.SetNegativeButton( "Cancel", PC_CANCEL );
	msgBox.HighlightChoice( true );
	SET_EVENT( msgBox, onPositive )
	{
		EngFuncs::CvarSetValue( "cl_predict", 1.0f );
		EngFuncs::CvarSetValue( "menu_mp_firsttime", 0.0f );
		CMenuMultiplayer::PredictDialog( pSelf, pExtra );
	}
	END_EVENT( msgBox, onPositive )

	SET_EVENT( msgBox, onNegative )
	{
		EngFuncs::CvarSetValue( "menu_mp_firsttime", 0.0f );
		CMenuMultiplayer::PredictDialog( pSelf, pExtra );
	}
	END_EVENT( msgBox, onNegative )

	AddItem( background );
	AddItem( banner );
	AddItem( internetGames );
	AddItem( spectateGames );
	AddItem( LANGame );
	AddItem( Customize );
	AddItem( Controls );
	AddItem( done );
	AddItem( msgBox );

	//if( EngFuncs::GetCvarFloat( "menu_mp_firsttime" ) && !EngFuncs::GetCvarFloat( "cl_predict" ) )
	if( 1 )
		PredictDialog( &msgBox, NULL );
}

void CMenuMultiplayer::_VidInit()
{
	internetGames.SetCoord( 72, 230 );
	spectateGames.SetCoord( 72, 280 );
	LANGame.SetCoord( 72, 330 );
	Customize.SetCoord( 72, 380 );
	Controls.SetCoord( 72, 430 );
	done.SetCoord( 72, 480 );
}

/*
=================
CMenuMultiplayer::Precache
=================
*/
void UI_MultiPlayer_Precache( void )
{
	EngFuncs::PIC_Load( ART_BACKGROUND );
	EngFuncs::PIC_Load( ART_BANNER );
}

/*
=================
UI_MultiPlayer_Menu
=================
*/
void UI_MultiPlayer_Menu( void )
{
	if ( gMenu.m_gameinfo.gamemode == GAME_SINGLEPLAYER_ONLY )
		return;

	UI_MultiPlayer_Precache();
	uiMultiPlayer.Init();
	uiMultiPlayer.VidInit();

	uiMultiPlayer.PushMenu();
}
