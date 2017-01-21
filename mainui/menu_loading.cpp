#include "extdll.h"
#include "basemenu.h"
#include "keydefs.h"
#include "menufont.h"	// built-in menu font
#include "utils.h"
#include "menu_btnsbmp_table.h"
//CR
#include "ui_title_anim.h"

CLoadingProgressBar g_LoadingProgressBar;

/*
==========================
UI_StartLoadingProgressBar
==========================
*/
void UI_StartLoadingProgressBar( const char *type, int progressPoints )
{
	;
}

/*
==========================
UI_ContinueLoadingProgressBar
==========================
*/
void UI_ContinueLoadingProgressBar( const char *type, int progressPoint, float fraction )
{
	;
}

/*
==========================
UI_SetLoadingProgressBarStatusText
==========================
*/
void UI_SetLoadingProgressBarStatusText( const char *text )
{
	;
}

/*
==========================
UI_StopLoadingProgressBar
==========================
*/
void UI_StopLoadingProgressBar( void )
{
	g_LoadingProgressBar.Reset();
}

/*
==========================
UI_SetSecondaryProgressBar
==========================
*/
void UI_SetSecondaryProgressBar( float progress )
{
	g_LoadingProgressBar.m_flSecondaryProgress = progress;
}

/*
==========================
UI_SetSecondaryProgressBarText
==========================
*/
void UI_SetSecondaryProgressBarText( const char *text )
{
	;
}

/*
==========================
UI_OnDisconnectFromServer
==========================
*/
void UI_OnDisconnectFromServer( int reason, const char *text )
{
	g_LoadingProgressBar.Reset();
}

CLoadingProgressBar::CLoadingProgressBar() :
	m_iProgressPoints(0), m_iProgressPoint(0),
	m_iDisconnectReason(REASON_UNKNOWN),
	m_flFrac(0), m_flSecondaryProgress(0),
	m_bActive(false),
	m_szType(NULL), m_szStatus(NULL), m_szSecondaryStatus(NULL), m_szDisconnectReasonText(NULL)
{
}

CLoadingProgressBar::~CLoadingProgressBar()
{
	Reset();
}

void CLoadingProgressBar::Reset()
{
	// expected to be allocated
	delete[] m_szDisconnectReasonText;
	delete[] m_szSecondaryStatus;
	delete[] m_szStatus;
	delete[] m_szType;

	m_bActive = false;
}

void CLoadingProgressBar::Draw()
{
	;
}
