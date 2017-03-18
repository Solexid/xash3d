/*
Bitmap.cpp - bitmap menu item
Copyright (C) 2010 Uncle Mike
Copyright (C) 2017 a1batross

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#include "extdll.h"
#include "basemenu.h"
#include "Bitmap.h"
#include "PicButton.h" // GetTitleTransFraction
#include "utils.h"

/*
=================
CMenuBitmap::Init
=================
*/
void CMenuBitmap::VidInit( )
{
	if( !m_szName ) m_szName = "";
	if( !m_szFocusPic ) m_szFocusPic = m_szPic;
	if( !m_iColor ) m_iColor = uiColorWhite;

	UI_ScaleCoords( &m_iX, &m_iY, &m_iWidth, &m_iHeight );
}

/*
=================
CMenuBitmap::Key
=================
*/
const char *CMenuBitmap::Key( int key, int down )
{
	const char	*sound = 0;

	switch( key )
	{
	case K_MOUSE1:
		if(!( m_iFlags & QMF_HASMOUSEFOCUS ))
			break;
		sound = uiSoundLaunch;
		break;
	case K_ENTER:
	case K_KP_ENTER:
	case K_AUX1:
		//if( !down ) return sound;
		if( m_iFlags & QMF_MOUSEONLY )
			break;
		sound = uiSoundLaunch;
		break;
	}
	if( sound && ( m_iFlags & QMF_SILENT ))
		sound = uiSoundNull;

	if( m_iFlags & QMF_ACT_ONRELEASE )
	{
		if( sound )
		{
			int	event;

			if( down )
			{
				event = QM_PRESSED;
				m_bPressed = true;
			}
			else event = QM_ACTIVATED;
			_Event( event );
		}
	}
	else if( down )
	{
		if( sound )
			_Event( QM_ACTIVATED );
	}

	return sound;
}

/*
=================
CMenuBitmap::Draw
=================
*/
void CMenuBitmap::Draw( void )
{
	if( m_iFlags & QMF_GRAYED )
	{
		UI_DrawPic( m_iX, m_iY, m_iWidth, m_iHeight, uiColorDkGrey, m_szPic );
		return; // grayed
	}

	if(( m_iFlags & QMF_MOUSEONLY ) && !( m_iFlags & QMF_HASMOUSEFOCUS ))
	{
		UI_DrawPic( m_iX, m_iY, m_iWidth, m_iHeight, m_iColor, m_szPic );
		return; // no focus
	}

	if(this != m_pParent->ItemAtCursor())
	{
		// UNDONE: only inactive bitmaps supported
		if( m_iFlags & QMF_DRAW_ADDITIVE )
			UI_DrawPicAdditive( m_iX, m_iY, m_iWidth, m_iHeight, m_iColor, m_szPic );
		else UI_DrawPic( m_iX, m_iY, m_iWidth, m_iHeight, m_iColor, m_szPic );
		return; // no focus
	}

	if( this->m_bPressed )
		UI_DrawPic( m_iX, m_iY, m_iWidth, m_iHeight, m_iColor, m_szPressPic );
	else if(!( m_iFlags & QMF_FOCUSBEHIND ))
		UI_DrawPic( m_iX, m_iY, m_iWidth, m_iHeight, m_iColor, m_szPic );
	if( m_iFlags & QMF_HIGHLIGHTIFFOCUS )
		UI_DrawPic( m_iX, m_iY, m_iWidth, m_iHeight, m_iColor, m_szFocusPic );
	else if( m_iFlags & QMF_PULSEIFFOCUS )
	{
		int	color;

		color = PackAlpha( m_iColor, 255 * (0.5 + 0.5 * sin( (float)uiStatic.realTime / UI_PULSE_DIVISOR )));
		UI_DrawPic( m_iX, m_iY, m_iWidth, m_iHeight, color, m_szFocusPic );
	}

	if( m_iFlags & QMF_FOCUSBEHIND )
		UI_DrawPic( m_iX, m_iY, m_iWidth, m_iHeight, m_iColor, m_szPic );
}

/*
=================
CMenuBackgroundBitmap::Draw
=================
*/
void CMenuBackgroundBitmap::Draw()
{
	if (!uiStatic.m_fHaveSteamBackground)
	{
		UI_DrawPic( 0, 0, ScreenWidth, ScreenHeight, uiColorWhite, m_szPic );
		return;
	}

	int xpos, ypos;
	float xScale, yScale;

	// work out scaling factors
	xScale = ScreenWidth / uiStatic.m_flTotalWidth;
	yScale = xScale;

	// iterate and draw all the background pieces
	ypos = 0;
	for (int y = 0; y < BACKGROUND_ROWS; y++)
	{
		xpos = 0;
		for (int x = 0; x < BACKGROUND_COLUMNS; x++)
		{
			bimage_t &bimage = uiStatic.m_SteamBackground[y][x];

			int dx = (int)ceil(xpos * xScale);
			int dy = (int)ceil(ypos * yScale);
			int dw = (int)ceil(bimage.width * xScale);
			int dt = (int)ceil(bimage.height * yScale);

			if (x == 0) dx = 0;
			if (y == 0) dy = 0;

			EngFuncs::EngFuncs::PIC_Set( bimage.hImage, 255, 255, 255, 255 );
			EngFuncs::EngFuncs::PIC_Draw( dx, dy, dw, dt );
			xpos += bimage.width;
		}
		ypos += uiStatic.m_SteamBackground[y][0].height;
	}
}

void CMenuBannerBitmap::Draw()
{
	// don't draw banners until transition is done
#ifdef TA_ALT_MODE
	if( CMenuPicButton::GetTitleTransFraction() != 10 ) return;
#else
	if( CMenuPicButton::GetTitleTransFraction() < 1.0f ) return;
#endif

	CMenuBitmap::Draw();
}

void CMenuBannerBitmap::VidInit()
{
	if( !m_bOverrideRect )
		SetRect( UI_BANNER_POSX, UI_BANNER_POSY, UI_BANNER_WIDTH, UI_BANNER_HEIGHT );

	CMenuBitmap::VidInit();
}
