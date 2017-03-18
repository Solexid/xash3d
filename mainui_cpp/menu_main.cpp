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
#include "Action.h"
#include "Bitmap.h"
#include "PicButton.h"
#include "YesNoMessageBox.h"
#include "utils.h"
#include "keydefs.h"
#include "menu_btnsbmp_table.h"
#include "menu_strings.h"

#define ART_MINIMIZE_N	"gfx/shell/min_n"
#define ART_MINIMIZE_F	"gfx/shell/min_f"
#define ART_MINIMIZE_D	"gfx/shell/min_d"
#define ART_CLOSEBTN_N	"gfx/shell/cls_n"
#define ART_CLOSEBTN_F	"gfx/shell/cls_f"
#define ART_CLOSEBTN_D	"gfx/shell/cls_d"

class CMenuMain: public CMenuFramework
{
public:
	virtual const char *Key( int key, int down );
	virtual void Activate( );

	void QuitDialog( void );
	void PromptDialog( void );
	void ShowPrompt( void ( *action )( void ), const char *message );

	CMenuPicButton	console;
private:
	virtual void _Init();
	virtual void _VidInit( );

	class CMenuMainBackground : public CMenuBackgroundBitmap
	{
	public:
		virtual void Draw();
	} background;
	CMenuPicButton	resumeGame;
	CMenuPicButton	disconnect;
	CMenuPicButton	newGame;
	CMenuPicButton	hazardCourse;
	CMenuPicButton	configuration;
	CMenuPicButton	saveRestore;
	CMenuPicButton	multiPlayer;
	CMenuPicButton	customGame;
	CMenuPicButton	previews;
	CMenuPicButton	quit;

	// buttons on top right. Maybe should be drawn if fullscreen == 1?
	CMenuBitmap	minimizeBtn;
	CMenuBitmap	quitButton;

	// quit dialog
	CMenuYesNoMessageBox quitDialog;
	CMenuYesNoMessageBox disconnectDialog;

	bool bTrainMap;
	bool bCustomGame;
};

static CMenuMain uiMain;

/*
=================
UI_Background_Ownerdraw
=================
*/
void CMenuMain::CMenuMainBackground::Draw( )
{
	// map has background
	if( EngFuncs::GetCvarFloat( "cl_background" ))
		return;

	CMenuBackgroundBitmap::Draw();

	if( uiStatic.m_fHaveSteamBackground || uiStatic.m_fDisableLogo )
		return; // no logos for steam background

	if( EngFuncs::GetLogoLength() <= 0.05f || EngFuncs::GetLogoWidth() <= 32 )
		return;	// don't draw stub logo (GoldSrc rules)

	float	logoWidth, logoHeight, logoPosY;
	float	scaleX, scaleY;

	scaleX = ScreenWidth / 640.0f;
	scaleY = ScreenHeight / 480.0f;

	logoWidth = EngFuncs::GetLogoWidth() * scaleX;
	logoHeight = EngFuncs::GetLogoHeight() * scaleY;
	logoPosY = 70 * scaleY;	// 70 it's empirically determined value (magic number)

	EngFuncs::DrawLogo( "logo.avi", 0, logoPosY, logoWidth, logoHeight );
}

void CMenuMain::QuitDialog( void )
{
	// toggle main menu between active\inactive
	// show\hide quit dialog
	console.Flags() ^= QMF_INACTIVE;
	resumeGame.Flags() ^= QMF_INACTIVE;
	disconnect.Flags() ^= QMF_INACTIVE;
	newGame.Flags() ^= QMF_INACTIVE;
	hazardCourse.Flags() ^= QMF_INACTIVE;
	saveRestore.Flags() ^= QMF_INACTIVE;
	configuration.Flags() ^= QMF_INACTIVE;
	multiPlayer.Flags() ^= QMF_INACTIVE;
	customGame.Flags() ^= QMF_INACTIVE;
	previews.Flags() ^= QMF_INACTIVE;
	quit.Flags() ^= QMF_INACTIVE;
	minimizeBtn.Flags() ^= QMF_INACTIVE;
	quitButton.Flags() ^= QMF_INACTIVE;

	quitDialog.ToggleVisibility();
}

void CMenuMain::PromptDialog( void )
{
	// toggle main menu between active\inactive
	// show\hide quit dialog
	console.Flags() ^= QMF_INACTIVE;
	resumeGame.Flags() ^= QMF_INACTIVE;
	disconnect.Flags() ^= QMF_INACTIVE;
	newGame.Flags() ^= QMF_INACTIVE;
	hazardCourse.Flags() ^= QMF_INACTIVE;
	saveRestore.Flags() ^= QMF_INACTIVE;
	configuration.Flags() ^= QMF_INACTIVE;
	multiPlayer.Flags() ^= QMF_INACTIVE;
	customGame.Flags() ^= QMF_INACTIVE;
	previews.Flags() ^= QMF_INACTIVE;
	quit.Flags() ^= QMF_INACTIVE;
	minimizeBtn.Flags() ^= QMF_INACTIVE;
	quitButton.Flags() ^= QMF_INACTIVE;

	disconnectDialog.ToggleVisibility();
}

/*
=================
UI_Main_KeyFunc
=================
*/
const char *CMenuMain::Key( int key, int down )
{
	if( down && key == K_ESCAPE )
	{
		if ( CL_IsActive( ))
		{
			if( !(quitDialog.Flags() & QMF_HIDDEN ) )
				QuitDialog();
			else if( !(disconnectDialog.Flags() & QMF_HIDDEN ) )
				PromptDialog();
			else
				UI_CloseMenu();
		}
		else QuitDialog();
		return uiSoundNull;
	}
	return CMenuFramework::Key( key, down );
}

/*
=================
UI_Main_ActivateFunc
=================
*/
void CMenuMain::Activate( void )
{
	if ( !CL_IsActive( ))
	{
		uiMain.resumeGame.Flags() |= QMF_HIDDEN;
		uiMain.disconnect.Flags() |= QMF_HIDDEN;
	}

	if( gpGlobals->developer )
	{
		uiMain.console.YPos() = CL_IsActive() ? 130 : 230;
		UI_ScaleCoords( NULL, &uiMain.console.YPos(), NULL, NULL );
	}
}

/*
=================
UI_Main_HazardCourse
=================
*/
static void UI_Main_HazardCourse( CMenuBaseItem*, void* )
{
	if( EngFuncs::GetCvarFloat( "host_serverstate" ) && EngFuncs::GetCvarFloat( "maxplayers" ) > 1 )
		EngFuncs::HostEndGame( "end of the game" );

	EngFuncs::CvarSetValue( "skill", 1.0f );
	EngFuncs::CvarSetValue( "deathmatch", 0.0f );
	EngFuncs::CvarSetValue( "teamplay", 0.0f );
	EngFuncs::CvarSetValue( "pausable", 1.0f ); // singleplayer is always allowing pause
	EngFuncs::CvarSetValue( "coop", 0.0f );

	EngFuncs::PlayBackgroundTrack( NULL, NULL );

	EngFuncs::ClientCmd( FALSE, "hazardcourse\n" );
}

static void UI_Main_Disconnect( CMenuBaseItem*, void* )
{
	if( EngFuncs::GetCvarFloat( "host_serverstate" ) )
		EngFuncs::ClientCmd( TRUE, "endgame;wait;wait;wait;menu_options;menu_main\n");
	else
		EngFuncs::ClientCmd( TRUE, "cmd disconnect;wait;wait;wait;menu_options;menu_main\n");
	UI_Main_Menu();
}

void CMenuMain::_Init( void )
{
	if( gMenu.m_gameinfo.trainmap[0] && stricmp( gMenu.m_gameinfo.trainmap, gMenu.m_gameinfo.startmap ) != 0 )
		bTrainMap = true;
	else bTrainMap = false;

	if( EngFuncs::GetCvarFloat( "host_allow_changegame" ))
		bCustomGame = true;
	else bCustomGame = false;

	EngFuncs::PrecacheLogo( "logo.avi" );

	// console
	console.SetNameAndStatus( "Console", "Show console" );
	SET_EVENT( console, onActivated )
	{
		UI_SetActiveMenu( FALSE );
		EngFuncs::KEY_SetDest( KEY_CONSOLE );
	}
	END_EVENT( console, onActivated )

	resumeGame.SetNameAndStatus( "Resume Game", MenuStrings[HINT_RESUME_GAME] );
	SET_EVENT_VOID( resumeGame, onActivated, UI_CloseMenu );

	disconnect.SetNameAndStatus( "Disconnect", "Disconnect from server" );
	SET_EVENT( disconnect, onActivated )
	{
		uiMain.disconnectDialog.SetMessage( "Really disconnect?" );
		uiMain.disconnectDialog.onPositive = UI_Main_Disconnect;
		uiMain.PromptDialog();
	}
	END_EVENT( disconnect, onActivated )

	newGame.SetNameAndStatus( "New Game", MenuStrings[HINT_NEWGAME] );
	SET_EVENT_VOID( newGame, onActivated, UI_NewGame_Menu );

	hazardCourse.SetNameAndStatus( "Hazard Course", MenuStrings[HINT_HAZARD_COURSE] );
	SET_EVENT( hazardCourse, onActivated )
	{
		if( CL_IsActive() )
		{
			uiMain.disconnectDialog.SetMessage( MenuStrings[HINT_RESTART_HZ] );
			uiMain.disconnectDialog.onPositive = UI_Main_HazardCourse;
			uiMain.PromptDialog();
		}
		else UI_Main_HazardCourse( &uiMain.hazardCourse, NULL );
	}
	END_EVENT( hazardCourse, onActivated )

	multiPlayer.SetNameAndStatus( "Multiplayer", MenuStrings[HINT_MULTIPLAYER] );
	// multiPlayer.onActivated = UI_MultiPlayer_Menu

	configuration.SetNameAndStatus( "Configuration", MenuStrings[HINT_CONFIGURATION] );
	// configuration.onActivated = UI_Options_Menu;

	SET_EVENT( saveRestore, onActivated )
	{
		/*if( CL_IsActive() )
			UI_SaveLoad_Menu();
		else UI_LoadGame_Menu();*/
	}
	END_EVENT( saveRestore, onActivated )

	customGame.SetNameAndStatus( "Custom Game", MenuStrings[HINT_CUSTOM_GAME] );
	// customGame.onActivated = UI_CustomGame_Menu;

	previews.SetNameAndStatus( "Previews", MenuStrings[ HINT_PREVIEWS_TEXT ] );
	SET_EVENT( previews, onActivated )
	{
		EngFuncs::ShellExecute( MenuStrings[HINT_PREVIEWS_CMD], NULL, false );
	}
	END_EVENT( previews, onActivated )

	quit.SetNameAndStatus( "Quit", MenuStrings[HINT_QUIT] );
	SET_EVENT_VOID( quit, onActivated, uiMain.QuitDialog );

	SET_EVENT_VOID( quitButton, onActivated, uiMain.QuitDialog );

	SET_EVENT( minimizeBtn, onActivated )
	{
		EngFuncs::ClientCmd( FALSE, "minimize\n" );
	}
	END_EVENT( minimizeBtn, onActivated )

	SET_EVENT( quitDialog, onPositive )
	{
		EngFuncs::ClientCmd( FALSE, "quit\n" );
	}
	END_EVENT( quitDialog, onPositive )
	quitDialog.SetPositiveButton( "Ok", PC_OK );
	quitDialog.SetNegativeButton("Cancel", PC_CANCEL);

	AddItem( background );

	if ( gpGlobals->developer )
		AddItem( console );

	AddItem( disconnect );
	AddItem( resumeGame );
	AddItem( newGame );

	if ( bTrainMap )
		AddItem( hazardCourse );

	AddItem( saveRestore );
	AddItem( configuration );
	AddItem( multiPlayer );

	if ( bCustomGame )
		AddItem( customGame );

	AddItem( previews );
	AddItem( quit );
	AddItem( minimizeBtn );
	AddItem( quitButton );
	AddItem( quitDialog );
	AddItem( disconnectDialog );
}

/*
=================
UI_Main_Init
=================
*/
void CMenuMain::_VidInit( void )
{
	background.Flags() = QMF_INACTIVE;

	console.Flags() = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW;
	console.SetCoord( 72, CL_IsActive() ? 130: 230 );
	console.SetPicture( PC_CONSOLE );

	resumeGame.Flags() = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_NOTIFY;
	resumeGame.SetCoord( 72, 230 );
	resumeGame.SetPicture( PC_RESUME_GAME );

	disconnect.Flags() = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_NOTIFY;
	disconnect.SetCoord( 72, 180 );
	disconnect.SetPicture( PC_DISCONNECT );

	newGame.Flags() = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_NOTIFY;
	newGame.SetCoord( 72, 280 );
	newGame.SetPicture( PC_NEW_GAME );

	if ( gMenu.m_gameinfo.gamemode == GAME_MULTIPLAYER_ONLY || !strlen( gMenu.m_gameinfo.startmap ))
		newGame.Flags() |= QMF_GRAYED;

	hazardCourse.Flags() = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_NOTIFY;
	hazardCourse.SetCoord( 72, 330 );
	hazardCourse.SetPicture( PC_HAZARD_COURSE );

	saveRestore.Flags() = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_NOTIFY;

	// server.dll needs for reading savefiles or startup newgame
	if( !EngFuncs::CheckGameDll( ))
	{
		saveRestore.Flags() |= QMF_GRAYED;
		hazardCourse.Flags() |= QMF_GRAYED;
		newGame.Flags() |= QMF_GRAYED;
	}

	if( CL_IsActive( ))
	{
		saveRestore.SetName( "Save\\Load Game" );
		saveRestore.SetStatusText( MenuStrings[HINT_SAVELOADGAME] );
		saveRestore.SetPicture( PC_SAVE_LOAD_GAME);
	}
	else
	{
		saveRestore.SetName( "Load Game" );
		saveRestore.SetStatusText( MenuStrings[HINT_LOADGAME] );
		resumeGame.Flags() |= QMF_HIDDEN;
		disconnect.Flags() |= QMF_HIDDEN;
		saveRestore.SetPicture( PC_LOAD_GAME );
	}

	saveRestore.SetCoord( 72, bTrainMap ? 380 : 330 );

	configuration.Flags() = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_NOTIFY;
	configuration.SetCoord( 72, bTrainMap ? 430 : 380 );
	configuration.SetPicture( PC_CONFIG );

	multiPlayer.Flags() = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_NOTIFY;
	multiPlayer.SetCoord( 72, bTrainMap ? 480 : 430 );
	multiPlayer.SetPicture( PC_MULTIPLAYER );

	if ( gMenu.m_gameinfo.gamemode == GAME_SINGLEPLAYER_ONLY )
		multiPlayer.Flags() |= QMF_GRAYED;

	if ( gMenu.m_gameinfo.gamemode == GAME_MULTIPLAYER_ONLY )
	{
		saveRestore.Flags() |= QMF_GRAYED;
		hazardCourse.Flags() |= QMF_GRAYED;
	}

	customGame.Flags() = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_NOTIFY;
	customGame.SetCoord( 72, bTrainMap ? 530 : 480 );
	customGame.SetPicture( PC_CUSTOM_GAME );

	previews.Flags() = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_NOTIFY;
	previews.SetCoord( 72, (bCustomGame) ? (bTrainMap ? 580 : 530) : (bTrainMap ? 530 : 480) );

	// too short execute string - not a real command
	if( strlen( MenuStrings[HINT_PREVIEWS_CMD] ) <= 3 )
		previews.Flags() |= QMF_GRAYED;

	previews.SetPicture( PC_PREVIEWS );

	quit.Flags() = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_NOTIFY;
	quit.SetCoord( 72, (bCustomGame) ? (bTrainMap ? 630 : 580) : (bTrainMap ? 580 : 530));
	quit.SetPicture( PC_QUIT );

	minimizeBtn.Flags() = QMF_HIGHLIGHTIFFOCUS|QMF_MOUSEONLY|QMF_ACT_ONRELEASE;
	minimizeBtn.SetRect( uiStatic.width - 72, 13, 32, 32 );
	minimizeBtn.SetPicture( ART_MINIMIZE_N, ART_MINIMIZE_F, ART_MINIMIZE_D );

	quitButton.Flags() = QMF_HIGHLIGHTIFFOCUS|QMF_MOUSEONLY|QMF_ACT_ONRELEASE;
	quitButton.SetRect( uiStatic.width - 36, 13, 32, 32 );
	quitButton.SetPicture( ART_CLOSEBTN_N, ART_CLOSEBTN_F, ART_CLOSEBTN_D );

	if( CL_IsActive() )
		quitDialog.SetMessage( MenuStrings[HINT_QUIT_ACTIVE] );
	else
		quitDialog.SetMessage( MenuStrings[HINT_QUIT] );
}

/*
=================
UI_Main_Precache
=================
*/
void UI_Main_Precache( void )
{
	EngFuncs::PIC_Load( ART_BACKGROUND );
	EngFuncs::PIC_Load( ART_MINIMIZE_N );
	EngFuncs::PIC_Load( ART_MINIMIZE_F );
	EngFuncs::PIC_Load( ART_MINIMIZE_D );
	EngFuncs::PIC_Load( ART_CLOSEBTN_N );
	EngFuncs::PIC_Load( ART_CLOSEBTN_F );
	EngFuncs::PIC_Load( ART_CLOSEBTN_D );

	// precache .avi file and get logo width and height
	EngFuncs::PrecacheLogo( "logo.avi" );
}

/*
=================
UI_Main_Menu
=================
*/
void UI_Main_Menu( void )
{
	UI_Main_Precache();

	uiMain.Init();
	uiMain.VidInit();

	uiMain.PushMenu();
}
