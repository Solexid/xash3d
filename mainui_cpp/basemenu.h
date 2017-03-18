/*
basemenu.h - menu basic header 
Copyright (C) 2010 Uncle Mike

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#ifndef BASEMENU_H
#define BASEMENU_H
#include "netadr.h"
#include "keydefs.h"

// engine constants
enum
{
	GAME_NORMAL = 0,
	GAME_SINGLEPLAYER_ONLY,
	GAME_MULTIPLAYER_ONLY
};

enum
{
	KEY_CONSOLE = 0,
	KEY_GAME,
	KEY_MENU
};

#define CS_SIZE			64	// size of one config string
#define CS_TIME			16	// size of time string

// color strings
#define ColorIndex( c )		((( c ) - '0' ) & 7 )
#define IsColorString( p )		( p && *( p ) == '^' && *(( p ) + 1) && *(( p ) + 1) >= '0' && *(( p ) + 1 ) <= '9' )

#define UI_MAX_MENUDEPTH		8
#define UI_MAX_MENUITEMS		64

#define UI_PULSE_DIVISOR		75
#define UI_BLINK_TIME		250
#define UI_BLINK_MASK		499

#define UI_SMALL_CHAR_WIDTH		10
#define UI_SMALL_CHAR_HEIGHT	20

#define UI_MED_CHAR_WIDTH		18
#define UI_MED_CHAR_HEIGHT		26

#define UI_BIG_CHAR_WIDTH		20
#define UI_BIG_CHAR_HEIGHT		40

#define UI_OUTLINE_WIDTH		uiStatic.outlineWidth	// outline thickness

#define UI_MAXGAMES			900	// slots for savegame/demos
#define UI_MAX_SERVERS		256
#define UI_MAX_BGMAPS		32

#define MAX_HINT_TEXT		512


// menu buttons dims
#define UI_BUTTONS_WIDTH		240
#define UI_BUTTONS_HEIGHT		40
#define UI_BUTTON_CHARWIDTH		14	// empirically determined value

#define UI_LEFTARROW		"gfx/shell/larrowdefault"
#define UI_LEFTARROWFOCUS		"gfx/shell/larrowflyover"
#define UI_LEFTARROWPRESSED		"gfx/shell/larrowpressed"
#define UI_RIGHTARROW		"gfx/shell/rarrowdefault"
#define UI_RIGHTARROWFOCUS		"gfx/shell/rarrowflyover"
#define UI_RIGHTARROWPRESSED		"gfx/shell/rarrowpressed"
#define UI_UPARROW			"gfx/shell/uparrowd"
#define UI_UPARROWFOCUS		"gfx/shell/uparrowf"
#define UI_UPARROWPRESSED		"gfx/shell/uparrowp"
#define UI_DOWNARROW		"gfx/shell/dnarrowd"
#define UI_DOWNARROWFOCUS		"gfx/shell/dnarrowf"
#define UI_DOWNARROWPRESSED		"gfx/shell/dnarrowp"

// Generic flags
#define BIT( x ) ( 1U << x )
enum
{
	QMF_LEFT_JUSTIFY       = BIT( 0 ),
	QMF_CENTER_JUSTIFY     = BIT( 1 ),
	QMF_RIGHT_JUSTIFY      = BIT( 2 ),
	QMF_GRAYED             = BIT( 3 ), // Grays and disables
	QMF_INACTIVE           = BIT( 4 ), // Disables any input
	QMF_HIDDEN             = BIT( 5 ), // Doesn't draw
	QMF_NUMBERSONLY        = BIT( 6 ), // Edit field is only numbers
	QMF_LOWERCASE          = BIT( 7 ), // Edit field is all lower case
	QMF_UPPERCASE          = BIT( 8 ), // Edit field is all upper case
	QMF_DRAW_ADDITIVE      = BIT( 9 ), // enable additive for this bitmap
	QMF_PULSEIFFOCUS       = BIT( 10 ),
	QMF_HIGHLIGHTIFFOCUS   = BIT( 11 ),
	QMF_SMALLFONT          = BIT( 12 ),
	QMF_BIGFONT            = BIT( 13 ),
	QMF_DROPSHADOW         = BIT( 14 ),
	QMF_SILENT             = BIT( 15 ), // Don't play sounds
	QMF_HASMOUSEFOCUS      = BIT( 16 ),
	QMF_MOUSEONLY          = BIT( 17 ), // Only mouse input allowed
	QMF_FOCUSBEHIND        = BIT( 18 ), // Focus draws behind normal item
	QMF_NOTIFY             = BIT( 19 ), // draw notify at right screen side
	QMF_ACT_ONRELEASE      = BIT( 20 ), // call Key_Event when button is released
	QMF_ALLOW_COLORSTRINGS = BIT( 21 ), // allow colorstring in MENU_FIELD
	QMF_HIDEINPUT          = BIT( 22 ), // used for "password" field
	QMF_HASKEYBOARDFOCUS   = BIT( 23 )
};

enum menuEvent_e
{
	QM_GOTFOCUS = 1,
	QM_LOSTFOCUS,
	QM_ACTIVATED,
	QM_CHANGED,
	QM_PRESSED
};

// Server browser
#define QMSB_GAME_LENGTH	25
#define QMSB_MAPNAME_LENGTH	20+QMSB_GAME_LENGTH
#define QMSB_MAXCL_LENGTH	10+QMSB_MAPNAME_LENGTH
#define QMSB_PING_LENGTH    10+QMSB_MAXCL_LENGTH

// Use these macros to set EventCallback, if no function pointer is available
// SET_EVENT must be followed by event code and closed by END_EVENT
// SET_EVENT_THIS can be used for setting event member inside it's class
// SET_EVENT_VOID can be used for setting void(void) function as event.
#ifdef MY_COMPILER_SUCKS
#define SET_EVENT( item, callback ) struct __##item##callback {\
	static void callback( CMenuBaseItem *pSelf, void *pExtra )
#define END_EVENT( item, callback ) }; (item).callback = __##item##callback::callback;

#define SET_EVENT_THIS( callback ) SET_EVENT( this, callback )
#define END_EVENT_THIS( callback ) }; this->callback = __this##callback::callback;

#define SET_EVENT_VOID( item, callback, func ) struct __##item##callback {\
	static void callback( CMenuBaseItem *pSelf, void *pExtra ) { (func)(); } };\
	(item).callback = __##item##callback::callback
#define SET_EVENT_THIS_VOID( callback, func ) struct __this##callback {\
	static void callback( CMenuBaseItem *pSelf, void *pExtra ) { (func)(); } };\
	this->callback = __this##callback::callback

#define SET_EVENT_CMD( item, callback, cmd, execute_now ) \
	struct __##item##callback \
	{\
		static void callback( CMenuBaseItem *pSelf, void *pExtra ) \
		{ \
			EngFuncs::ClientCmd( (execute_now), (cmd) );
		} \
	};\
	(item).callback = __##item##callback::callback
#define SET_EVENT_THIS_CMD( callback, cmd, execute_now ) \
	struct __this##callback \
	{\
		static void callback( CMenuBaseItem *pSelf, void *pExtra ) \
		{ \
			EngFuncs::ClientCmd( (execute_now), (cmd) );
		} \
	}; \
	this->callback = __this##callback::callback
#else
#define SET_EVENT( item, callback ) (item).callback = [](CMenuBaseItem *pSelf, void *pExtra)
#define END_EVENT( item, callback ) ;

#define SET_EVENT_THIS( callback ) SET_EVENT( (*this), callback )
#define END_EVENT_THIS( callback ) END_EVENT( (*this), callback )

#define SET_EVENT_VOID( item, callback, func ) \
	(item).callback = [](CMenuBaseItem *pSelf, void *pExtra) \
	{ \
		(func)(); \
	}
#define SET_EVENT_THIS_VOID( item, callback, func ) SET_EVENT_VOID( (*this), callback, func )

#define SET_EVENT_CMD( item, callback, cmd, execute_now ) \
	(item).callback = [](CMenuBaseItem *pSelf, void *pExtra) \
	{ \
		EngFuncs::ClientCmd( (execute_now), (cmd) ); \
	}
#define SET_EVENT_THIS_CMD( callback, cmd, execute_now ) SET_EVENT_CMD( (*this), callback, cmd, execute_now )

#endif

class CMenuBaseItem;

class CMenuFramework
{
public:
	CMenuFramework() : cursor(-1), cursorPrev(-1), items(), numItems(), initialized(false) {}

	void Init( void );
	void VidInit( void );

	virtual void Draw( void );
	virtual const char *Key( int key, int down );
	virtual void Activate( void ){}
	virtual void Char( int key );
	virtual void MouseMove( int x, int y );

	void AddItem( CMenuBaseItem &item );
	void CursorMoved( void );
	void SetCursor( int newCursor, bool notify = true );
	void SetCursorToItem( CMenuBaseItem *item, bool notify = true );

	CMenuBaseItem *ItemAtCursor( void );

	void AdjustCursor( int dir );

	void PushMenu( void );
	void PopMenu( void );

	static void PopMenuCb( CMenuBaseItem *pSelf, void *pExtra );

	inline int GetCursor() const { return cursor; }
	inline int GetCursorPrev() const { return cursorPrev; }
	inline int ItemCount() const { return numItems; }
	inline bool WasInit() const { return initialized; }

private:
	virtual void _Init() {}
	virtual void _VidInit() {}
	void VidInitItems();
	bool initialized;
	int cursor;
	int cursorPrev;

	CMenuBaseItem *items[UI_MAX_MENUITEMS];
	int numItems;
};

class CMenuBaseItem
{
public:
	friend class CMenuFramework;

	virtual void Init( void ){}
	virtual void VidInit( void ){}
	virtual const char *Key( int key, int down ){ return 0; }
	virtual void Draw( void ){}
	virtual void Char( int key ){}
	virtual ~CMenuBaseItem(){}

	const char *Activate( void );
	bool IsCurrentSelected( void );

#ifndef MY_COMPILER_SUCKS_DICKS
	typedef std::function<void(CMenuBaseItem *, void*)> EventCallback;
#else
	typedef void (*EventCallback)(CMenuBaseItem *, void *);
#endif

	EventCallback onGotFocus;
	EventCallback onLostFocus;
	EventCallback onActivated;
	EventCallback onChanged;
	EventCallback onPressed;

	void SetCoord( int x, int y )                { m_iX = x, m_iY = y; }
	void SetSize( int w, int h )                 { m_iWidth = w, m_iHeight = h; }
	void SetRect( int x, int y, int w, int h )   { SetCoord( x, y ); SetSize( w, h ); }
	void SetCharSize( int w, int h	)            { m_iCharWidth = w; m_iCharHeight = h; }
	void SetName( const char *name )             { m_szName = name; }
	void SetStatusText( const char *status )     { m_szStatusText = status; }
	void SetNameAndStatus( const char *name, const char *status ) { SetName( name ); SetStatusText( status ); }
	void SetColor( int color )                   { m_iColor = color; }
	void SetFocusColor( int color )              { m_iFocusColor = color; }

	CMenuFramework* Parent() { return m_pParent; }
	const char*	Name() { return m_szName; }
	const char* StatusText() { return m_szStatusText; }
	unsigned int &Flags() { return m_iFlags; }
	int &XPos() { return m_iX; }
	int &YPos() { return m_iY; }
	int &Width() { return m_iWidth; }
	int &Height() { return m_iHeight; }
	int &Color() { return m_iColor; }
	int &FocusColor() { return m_iFocusColor; }
	int &CharWidth() { return m_iCharWidth; }
	int &CharHeight() { return m_iCharHeight; }

	void *&EventCallbackExtra() { return m_pEventCallbackExtra; }

protected:
	CMenuFramework	*m_pParent;
	void *m_pEventCallbackExtra;

	const char	*m_szName;
	const char	*m_szStatusText;

	unsigned int	m_iFlags;

	int		m_iX, m_iY;
	int		m_iWidth, m_iHeight;

	int		m_iX2, m_iY2;
	int		m_iWidth2, m_iHeight2;

	bool	m_bPressed;
	int		m_iLastFocusTime;

	int		m_iColor;
	int		m_iFocusColor;
	int		m_iCharWidth;
	int		m_iCharHeight;

	// calls specific EventCallback
	virtual void _Event( int ev );
};

// =====================================================================
// Main menu interface

extern cvar_t	*ui_precache;
extern cvar_t	*ui_showmodels;

#define BACKGROUND_ROWS	3
#define BACKGROUND_COLUMNS	4

typedef struct
{
	HIMAGE	hImage;
	int	width;
	int	height;
} bimage_t;

typedef struct
{
	CMenuFramework	*menuActive;
	CMenuFramework	*menuStack[UI_MAX_MENUDEPTH];
	int				menuDepth;

	netadr_t serverAddresses[UI_MAX_SERVERS];
	char	serverNames[UI_MAX_SERVERS][256];
	float	serverPings[UI_MAX_SERVERS];
	int		serversRefreshTime;
	int		numServers;
	int		updateServers;	// true is receive new info about servers

	char	bgmaps[UI_MAX_BGMAPS][80];
	int		bgmapcount;

	HIMAGE	hFont;		// mainfont

	// handle steam background images
	bimage_t m_SteamBackground[BACKGROUND_ROWS][BACKGROUND_COLUMNS];
	float	m_flTotalWidth;
	float	m_flTotalHeight;
	bool	m_fHaveSteamBackground;
	bool	m_fDisableLogo;
	bool	m_fDemosPlayed;
	int		m_iOldMenuDepth;

	float	scaleX;
	float	scaleY;
	int		outlineWidth;
	int		sliderWidth;

	int		cursorX;
	int		cursorY;
	int		realTime;
	int		firstDraw;
	float	enterSound;
	int		mouseInRect;
	int		hideCursor;
	int		visible;
	int		framecount;	// how many frames menu visible
	int		initialized;

	// btns_main.bmp stuff
	HIMAGE	buttonsPics[71];	// FIXME: replace with PC_BUTTONCOUNT

	int		buttons_width;	// btns_main.bmp global width
	int		buttons_height;	// per one button with all states (inactive, focus, pressed) 

	int		buttons_draw_width;	// scaled image what we drawing
	int		buttons_draw_height;
	int		width;
} uiStatic_t;

extern float	cursorDY;			// use for touch scroll
extern bool cursorDown;
extern uiStatic_t		uiStatic;

#define DLG_X ((uiStatic.width - 640) / 2 - 192) // Dialogs are 640px in width

extern const char		*uiSoundIn;
extern const char		*uiSoundOut;
extern const char		*uiSoundKey;
extern const char		*uiSoundRemoveKey;
extern const char		*uiSoundLaunch;
extern const char		*uiSoundBuzz;
extern const char		*uiSoundGlow;
extern const char		*uiSoundMove;
extern const char		*uiSoundNull;

extern int	uiColorHelp;
extern int	uiPromptBgColor;
extern int	uiPromptTextColor;
extern int	uiPromptFocusColor;
extern int	uiInputTextColor;
extern int	uiInputBgColor;
extern int	uiInputFgColor;

extern int	uiColorWhite;
extern int	uiColorDkGrey;
extern int	uiColorBlack;

void UI_ScaleCoords( int *x, int *y, int *w, int *h );
int UI_CursorInRect( int x, int y, int w, int h );
void UI_DrawPic( int x, int y, int w, int h, const int color, const char *pic );
void UI_DrawPicAdditive( int x, int y, int w, int h, const int color, const char *pic );
void UI_DrawPicTrans( int x, int y, int width, int height, const int color, const char *pic );
void UI_DrawPicHoles( int x, int y, int width, int height, const int color, const char *pic );
void UI_FillRect( int x, int y, int w, int h, const int color );
#define UI_DrawRectangle( x, y, w, h, color ) UI_DrawRectangleExt( x, y, w, h, color, uiStatic.outlineWidth )
void UI_DrawRectangleExt( int in_x, int in_y, int in_w, int in_h, const int color, int outlineWidth );
void UI_DrawString( int x, int y, int w, int h, const char *str, const int col, int forceCol, int charW, int charH, int justify, int shadow );
void UI_StartSound( const char *sound );
void UI_LoadBmpButtons( void );

void UI_RefreshInternetServerList( void );
void UI_RefreshServerList( void );
int UI_CreditsActive( void );
void UI_DrawFinalCredits( void );

void UI_CloseMenu( void );

// Precache
void UI_Main_Precache( void );
void UI_NewGame_Precache( void );
void UI_LoadGame_Precache( void );
void UI_SaveGame_Precache( void );
void UI_SaveLoad_Precache( void );
void UI_MultiPlayer_Precache( void );
void UI_Options_Precache( void );
void UI_InternetGames_Precache( void );
void UI_LanGame_Precache( void );
void UI_PlayerSetup_Precache( void );
void UI_Controls_Precache( void );
void UI_AdvControls_Precache( void );
void UI_GameOptions_Precache( void );
void UI_CreateGame_Precache( void );
void UI_Audio_Precache( void );
void UI_Video_Precache( void );
void UI_VidOptions_Precache( void );
void UI_VidModes_Precache( void );
void UI_CustomGame_Precache( void );
void UI_Credits_Precache( void );
void UI_GoToSite_Precache( void );
void UI_Touch_Precache( void );
void UI_TouchOptions_Precache( void );
void UI_TouchButtons_Precache( void );
void UI_TouchEdit_Precache( void );
void UI_FileDialog_Precache( void );
void UI_GamePad_Precache( void );

// Menus
void UI_Main_Menu( void );
void UI_NewGame_Menu( void );
void UI_LoadGame_Menu( void );
void UI_SaveGame_Menu( void );
void UI_SaveLoad_Menu( void );
void UI_MultiPlayer_Menu( void );
void UI_Options_Menu( void );
void UI_InternetGames_Menu( void );
void UI_LanGame_Menu( void );
void UI_PlayerSetup_Menu( void );
void UI_Controls_Menu( void );
void UI_AdvControls_Menu( void );
void UI_GameOptions_Menu( void );
void UI_CreateGame_Menu( void );
void UI_Audio_Menu( void );
void UI_Video_Menu( void );
void UI_VidOptions_Menu( void );
void UI_VidModes_Menu( void );
void UI_CustomGame_Menu( void );
void UI_Credits_Menu( void );
void UI_Touch_Menu( void );
void UI_TouchOptions_Menu( void );
void UI_TouchButtons_Menu( void );
void UI_TouchEdit_Menu( void );
void UI_FileDialog_Menu( void );
void UI_TouchButtons_AddButtonToList( const char *name, const char *texture, const char *command, unsigned char *color, int flags );
void UI_TouchButtons_GetButtonList();
void UI_GamePad_Menu( void );
//
//-----------------------------------------------------
//
class CMenu
{
public:
	// Game information
	GAMEINFO		m_gameinfo;
};

typedef struct
{
	char patterns[32][256];
	int npatterns;
	char result[256];
	bool valid;
	void ( * callback )( bool success );
	bool preview;
} uiFileDialogGlobal_t;

extern uiFileDialogGlobal_t uiFileDialogGlobal;

extern CMenu gMenu;

#endif // BASEMENU_H
