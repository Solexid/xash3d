/*
Action.cpp - simple label with background item
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
#include "Action.h"
#include "utils.h"

/*
=================
CMenuAction::Init
=================
*/
void CMenuAction::VidInit( )
{
	if( !m_szName ) m_szName = ""; // this is also the text displayed

	if( m_iFlags & QMF_BIGFONT )
	{
		m_iCharWidth = UI_BIG_CHAR_WIDTH;
		m_iCharHeight = UI_BIG_CHAR_HEIGHT;
	}
	else if( m_iFlags & QMF_SMALLFONT )
	{
		m_iCharWidth = UI_SMALL_CHAR_WIDTH;
		m_iCharHeight = UI_SMALL_CHAR_HEIGHT;
	}
	else
	{
		if( m_iCharWidth < 1 ) m_iCharWidth = UI_MED_CHAR_WIDTH;
		if( m_iCharHeight < 1 ) m_iCharHeight = UI_MED_CHAR_HEIGHT;
	}

	if(!( m_iFlags & ( QMF_LEFT_JUSTIFY|QMF_CENTER_JUSTIFY|QMF_RIGHT_JUSTIFY )))
		m_iFlags |= QMF_LEFT_JUSTIFY;

	if( !m_iColor ) m_iColor = uiPromptTextColor;
	if( !m_iFocusColor ) m_iFocusColor = uiPromptFocusColor;

	if( m_iWidth < 1 || m_iHeight < 1 )
	{
		if( this->background )
		{
			HIMAGE handle = EngFuncs::PIC_Load( this->background );
			m_iWidth = EngFuncs::PIC_Width( handle );
			m_iHeight = EngFuncs::PIC_Height( handle );
		}
		else
		{
			if( m_iWidth < 1 )
				m_iWidth = m_iCharWidth * strlen( m_szName );

			if( m_iHeight < 1 )
				m_iHeight = m_iCharHeight * 1.5;
		}
	}

	UI_ScaleCoords( NULL, NULL, &m_iCharWidth, &m_iCharHeight );
	UI_ScaleCoords( &m_iX, &m_iY, &m_iWidth, &m_iHeight );
}

/*
=================
CMenuAction::Key
=================
*/
const char *CMenuAction::Key( int key, int down )
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
CMenuAction::Draw
=================
*/
void CMenuAction::Draw( )
{
	int	justify = 0;
	int	shadow;

	if( m_iFlags & QMF_LEFT_JUSTIFY )
		justify = 0;
	else if( m_iFlags & QMF_CENTER_JUSTIFY )
		justify = 1;
	else if( m_iFlags & QMF_RIGHT_JUSTIFY )
		justify = 2;

	shadow = (m_iFlags & QMF_DROPSHADOW);

	if( this->background )
		UI_DrawPic( m_iX, m_iY, m_iWidth, m_iHeight, uiColorWhite, this->background );

	if( m_szStatusText && m_iFlags & QMF_NOTIFY )
	{
		int	charW, charH;
		int	x, w;

		charW = UI_SMALL_CHAR_WIDTH;
		charH = UI_SMALL_CHAR_HEIGHT;

		UI_ScaleCoords( NULL, NULL, &charW, &charH );

		x = 290;
		w = UI_SMALL_CHAR_WIDTH * strlen( m_szStatusText );
		UI_ScaleCoords( &x, NULL, &w, NULL );
		x += m_iX;

		int	r, g, b;

		UnpackRGB( r, g, b, uiColorHelp );
		EngFuncs::DrawSetTextColor( r, g, b );
		EngFuncs::DrawConsoleString( x, m_iY, m_szStatusText );
	}

	if( m_iFlags & QMF_GRAYED )
	{
		UI_DrawString( m_iX, m_iY, m_iWidth, m_iHeight, m_szName, uiColorDkGrey, true, m_iCharWidth, m_iCharHeight, justify, shadow );
		return; // grayed
	}

	if(this != m_pParent->ItemAtCursor())
	{
		UI_DrawString( m_iX, m_iY, m_iWidth, m_iHeight, m_szName, m_iColor, false, m_iCharWidth, m_iCharHeight, justify, shadow );
		return; // no focus
	}

	if(!( m_iFlags & QMF_FOCUSBEHIND ))
		UI_DrawString( m_iX, m_iY, m_iWidth, m_iHeight, m_szName, m_iColor, false, m_iCharWidth, m_iCharHeight, justify, shadow );

	if( m_iFlags & QMF_HIGHLIGHTIFFOCUS )
		UI_DrawString( m_iX, m_iY, m_iWidth, m_iHeight, m_szName, m_iFocusColor, false, m_iCharWidth, m_iCharHeight, justify, shadow );
	else if( m_iFlags & QMF_PULSEIFFOCUS )
	{
		int	color;

		color = PackAlpha( m_iColor, 255 * (0.5 + 0.5 * sin( (float)uiStatic.realTime / UI_PULSE_DIVISOR )));

		UI_DrawString( m_iX, m_iY, m_iWidth, m_iHeight, m_szName, color, false, m_iCharWidth, m_iCharHeight, justify, shadow );
	}

	if( m_iFlags & QMF_FOCUSBEHIND )
		UI_DrawString( m_iX, m_iY, m_iWidth, m_iHeight, m_szName, m_iColor, false, m_iCharWidth, m_iCharHeight, justify, shadow );
}
