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
#include "ScrollList.h"
#include "utils.h"
#include "keydefs.h"
#include "menu_btnsbmp_table.h"

#define ART_BANNER_INET		"gfx/shell/head_inetgames"
#define ART_BANNER_LAN		"gfx/shell/head_lan"

class CMenuServerBrowser: public CMenuFramework
{
public:
	virtual const char *Key( int key, int down );
	virtual void Draw();

	void SetLANOnly( bool lanOnly ) { m_bLanOnly = lanOnly; }
private:
	virtual void _Init();
	virtual void _VidInit();

	static void PromptDialog( CMenuBaseItem *pSelf, void *pExtra );
	void GetGamesList( void );
	static void JoinGame( CMenuBaseItem *pSelf, void *pExtra );

	char		gameDescription[UI_MAX_SERVERS][256];
	char		*gameDescriptionPtr[UI_MAX_SERVERS];

	CMenuBackgroundBitmap background;
	CMenuBannerBitmap banner;

	CMenuPicButton joinGame;
	CMenuPicButton createGame;
	CMenuPicButton gameInfo;
	CMenuPicButton refresh;
	CMenuPicButton done;

	CMenuYesNoMessageBox msgBox;
	CMenuScrollList	gameList;
	CMenuAction	hintMessage;

	char		hintText[MAX_HINT_TEXT];
	int		refreshTime;

	bool m_bLanOnly;
};

static CMenuServerBrowser	uiServerBrowser;

void CMenuServerBrowser::PromptDialog( CMenuBaseItem *pSelf, void *pExtra )
{
	CMenuServerBrowser *parent = (CMenuServerBrowser *)pSelf->Parent();

	// toggle main menu between active\inactive
	// show\hide quit dialog
	parent->joinGame.Flags() ^= QMF_INACTIVE;
	parent->createGame.Flags() ^= QMF_INACTIVE;
	parent->gameInfo.Flags() ^= QMF_INACTIVE;
	parent->refresh.Flags() ^= QMF_INACTIVE;
	parent->done.Flags() ^= QMF_INACTIVE;
	parent->gameList.Flags() ^= QMF_INACTIVE;

	parent->msgBox.ToggleVisibility();
}

/*
=================
CMenuServerBrowser::KeyFunc
=================
*/
const char *CMenuServerBrowser::Key( int key, int down )
{
	if( down && key == K_ESCAPE && !( msgBox.Flags() & QMF_HIDDEN ))
	{
		PromptDialog( &msgBox, NULL );
		return uiSoundNull;
	}
	return CMenuFramework::Key( key, down );
}

/*
=================
CMenuServerBrowser::GetGamesList
=================
*/
void CMenuServerBrowser::GetGamesList( void )
{
	int		i;
	const char	*info;

	for( i = 0; i < uiStatic.numServers; i++ )
	{
		if( i >= UI_MAX_SERVERS )
			break;
		info = uiStatic.serverNames[i];

		gameDescription[i][0] = 0; // mark this string as empty

		StringConcat( gameDescription[i], Info_ValueForKey( info, "host" ), QMSB_GAME_LENGTH );
		AddSpaces( gameDescription[i], QMSB_GAME_LENGTH );

		StringConcat( gameDescription[i], Info_ValueForKey( info, "map" ), QMSB_MAPNAME_LENGTH );
		AddSpaces( gameDescription[i], QMSB_MAPNAME_LENGTH );

		StringConcat( gameDescription[i], Info_ValueForKey( info, "numcl" ), QMSB_MAXCL_LENGTH );
		StringConcat( gameDescription[i], "\\", QMSB_MAXCL_LENGTH );
		StringConcat( gameDescription[i], Info_ValueForKey( info, "maxcl" ), QMSB_MAXCL_LENGTH );
		AddSpaces( gameDescription[i], QMSB_MAXCL_LENGTH );

		char ping[10];
		snprintf( ping, 10, "%.f ms", uiStatic.serverPings[i] * 1000 );
		StringConcat( gameDescription[i], ping, QMSB_PING_LENGTH );
		AddSpaces( gameDescription[i], QMSB_PING_LENGTH );

		gameDescriptionPtr[i] = gameDescription[i];
	}

	for( ; i < UI_MAX_SERVERS; i++ )
		gameDescriptionPtr[i] = NULL;

	gameList.itemNames = (const char **)gameDescriptionPtr;
	gameList.numItems = 0; // reset it
	gameList.curItem = 0; // reset it

	if( !gameList.CharHeight() )
		return; // to avoid divide integer by zero

	// count number of items
	while( gameList.itemNames[gameList.numItems] )
		gameList.numItems++;

	// calculate number of visible rows
	int h = gameList.Height();
	UI_ScaleCoords( NULL, NULL, NULL, &h );

	gameList.numRows = (h / gameList.CharHeight()) - 2;
	if( gameList.numRows > gameList.numItems ) gameList.numRows = gameList.numItems;

	if( uiStatic.numServers )
		joinGame.Flags() &= ~QMF_GRAYED;
}

/*
=================
CMenuServerBrowser::JoinGame
=================
*/
void CMenuServerBrowser::JoinGame( CMenuBaseItem *pSelf, void *pExtra )
{
	CMenuServerBrowser *parent = (CMenuServerBrowser *)pSelf->Parent();

	// close dialog
	if( !(parent->msgBox.Flags() & QMF_HIDDEN ) )
		PromptDialog( pSelf, pExtra );

	if( parent->gameDescription[parent->gameList.curItem][0] == 0 )
		return;

	EngFuncs::ClientJoin( uiStatic.serverAddresses[parent->gameList.curItem] );
	// prevent refresh durning connect
	parent->refreshTime = uiStatic.realTime + 999999999;
}

/*
=================
UI_Background_Ownerdraw
=================
*/
void CMenuServerBrowser::Draw( void )
{
	CMenuFramework::Draw();

	if( uiStatic.realTime > refreshTime )
	{
		refreshTime = uiStatic.realTime + 20000; // refresh every 10 secs
		if( m_bLanOnly ) UI_RefreshServerList();
		else UI_RefreshInternetServerList();
	}

	// serverinfo has been changed update display
	if( uiStatic.updateServers )
	{
		GetGamesList ();
		uiStatic.updateServers = false;
	}
}

/*
=================
CMenuServerBrowser::Init
=================
*/
void CMenuServerBrowser::_Init( void )
{
	// memset( &uiServerBrowser, 0, sizeof( CMenuServerBrowser ));

	StringConcat( hintText, "Name", QMSB_GAME_LENGTH );
	AddSpaces( hintText, QMSB_GAME_LENGTH );
	StringConcat( hintText, "Map", QMSB_MAPNAME_LENGTH );
	AddSpaces( hintText, QMSB_MAPNAME_LENGTH );
	StringConcat( hintText, "Players", QMSB_MAXCL_LENGTH );
	AddSpaces( hintText, QMSB_MAXCL_LENGTH );
	StringConcat( hintText, "Ping", QMSB_PING_LENGTH );
	AddSpaces( hintText, QMSB_PING_LENGTH );


	joinGame.Flags() = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_GRAYED;
	joinGame.SetNameAndStatus( "Join game", "Join to selected game" );
	joinGame.SetPicture( PC_JOIN_GAME );
	SET_EVENT( joinGame, onActivated )
	{
		if( CL_IsActive() ) PromptDialog( pSelf, pExtra );
		else JoinGame( pSelf, pExtra );
	}
	END_EVENT( joinGame, onActivated )

	createGame.Flags() = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW;
	createGame.SetNameAndStatus( "Create game", "Create new Internet game" );
	createGame.SetPicture( PC_CREATE_GAME );
	SET_EVENT( createGame, onActivated )
	{
		if( ((CMenuServerBrowser*)pSelf->Parent())->m_bLanOnly )
			EngFuncs::CvarSetValue( "public", 0.0f );
		else EngFuncs::CvarSetValue( "public", 1.0f );

		// UI_CreateGame_Menu();
	}
	END_EVENT( createGame, onActivated )

	gameInfo.Flags() = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_GRAYED;
	gameInfo.SetNameAndStatus( "View game info", "Get detail game info" );
	gameInfo.SetPicture( PC_VIEW_GAME_INFO );
	// TODO: implement!

	refresh.Flags() = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW;
	refresh.SetNameAndStatus( "Refresh", "Refresh servers list" );
	refresh.SetPicture( PC_REFRESH );

	done.Flags() = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW;
	done.SetNameAndStatus( "Done", "Return to main menu" );
	done.onActivated = PopMenuCb;
	done.SetPicture( PC_DONE );

	msgBox.Flags() = QMF_INACTIVE|QMF_HIDDEN;

	msgBox.SetMessage( "Join a network game will exit\nany current game, OK to exit?" );
	msgBox.SetPositiveButton( "Ok", PC_OK );
	msgBox.SetNegativeButton( "Cancel", PC_CANCEL );
	msgBox.HighlightChoice( true );
	msgBox.onPositive = JoinGame;
	msgBox.onNegative = PromptDialog;

	hintMessage.Flags() = QMF_INACTIVE|QMF_SMALLFONT;
	hintMessage.SetColor( uiColorHelp );
	hintMessage.SetName( hintText );

	gameList.Flags() = QMF_HIGHLIGHTIFFOCUS|QMF_SMALLFONT;
	gameList.itemNames = (const char **)gameDescriptionPtr;

	// server.dll needs for reading savefiles or startup newgame
	if( !EngFuncs::CheckGameDll( ))
		createGame.Flags() |= QMF_GRAYED;	// server.dll is missed - remote servers only


	AddItem( background );
	AddItem( banner );
	AddItem( joinGame );
	AddItem( createGame );
	AddItem( gameInfo );
	AddItem( refresh );
	AddItem( done );
	AddItem( hintMessage );
	AddItem( gameList );
	AddItem( msgBox );

}

/*
=================
CMenuServerBrowser::VidInit
=================
*/
void CMenuServerBrowser::_VidInit()
{
	memset( gameDescription, 0, sizeof( gameDescription ));
	memset( gameDescriptionPtr, 0, sizeof( gameDescriptionPtr ));

	if( m_bLanOnly )
	{
		banner.SetPicture( ART_BANNER_LAN );
		createGame.SetStatusText( "Create new LAN game" );
		SET_EVENT_VOID( refresh, onActivated, UI_RefreshServerList );
	}
	else
	{
		banner.SetPicture( ART_BANNER_INET );
		createGame.SetStatusText( "Create new Internet game" );
		SET_EVENT_VOID( refresh, onActivated, UI_RefreshInternetServerList );
	}

	joinGame.SetCoord( 72, 230 );
	createGame.SetCoord( 72, 280 );
	gameInfo.SetCoord( 72, 330 );
	refresh.SetCoord( 72, 380 );
	done.SetCoord( 72, 430 );

	hintMessage.SetCoord( 340, 225 );
	gameList.SetRect( 340, 255, 660, 240 );

	refreshTime = uiStatic.realTime + 500; // delay before update 0.5 sec
}

/*
=================
CMenuServerBrowser::Precache
=================
*/
void UI_InternetGames_Precache( void )
{
	EngFuncs::PIC_Load( ART_BACKGROUND );
	EngFuncs::PIC_Load( ART_BANNER_INET );
	EngFuncs::PIC_Load( ART_BANNER_LAN );
}

/*
=================
CMenuServerBrowser::Menu
=================
*/
void UI_ServerBrowser_Menu( void )
{
	if ( gMenu.m_gameinfo.gamemode == GAME_SINGLEPLAYER_ONLY )
		return;

	// stop demos to allow network sockets to open
	if ( gpGlobals->demoplayback && EngFuncs::GetCvarFloat( "cl_background" ))
	{
		uiStatic.m_iOldMenuDepth = uiStatic.menuDepth;
		EngFuncs::ClientCmd( FALSE, "stop\n" );
		uiStatic.m_fDemosPlayed = true;
	}

	UI_InternetGames_Precache();
	uiServerBrowser.Init();
	uiServerBrowser.VidInit();

	uiServerBrowser.PushMenu();
}

void UI_InternetGames_Menu( void )
{
	uiServerBrowser.SetLANOnly( false );

	UI_ServerBrowser_Menu();
}

void UI_LanGame_Menu( void )
{
	uiServerBrowser.SetLANOnly( true );

	UI_ServerBrowser_Menu();
}

void UI_LanGame_Precache( void )
{
	UI_InternetGames_Precache();
}
