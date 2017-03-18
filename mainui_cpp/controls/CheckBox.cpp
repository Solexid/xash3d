/*
CheckBox.h - checkbox
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
#include "CheckBox.h"
#include "utils.h"

/*
=================
CMenuCheckBox::Init
=================
*/
void CMenuCheckBox::VidInit( void )
{
	if( !m_szName ) m_szName = "";

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
		if( m_iCharWidth < 1 ) m_iCharWidth = 12;
		if( m_iCharHeight < 1 ) m_iCharHeight = 24;
	}

	UI_ScaleCoords( NULL, NULL, &m_iCharWidth, &m_iCharHeight );

	if(!(m_iFlags & (QMF_LEFT_JUSTIFY|QMF_CENTER_JUSTIFY|QMF_RIGHT_JUSTIFY)))
		m_iFlags |= QMF_LEFT_JUSTIFY;

	if( !this->emptyPic ) this->emptyPic = UI_CHECKBOX_EMPTY;
	if( !this->focusPic ) this->focusPic = UI_CHECKBOX_FOCUS;
	if( !this->checkPic ) this->checkPic = UI_CHECKBOX_ENABLED;
	if( !this->grayedPic ) this->grayedPic = UI_CHECKBOX_GRAYED;
	if( !m_iColor ) m_iColor = uiColorWhite;
	if( !m_iFocusColor ) m_iFocusColor = uiColorWhite;

	if( !m_iWidth ) m_iWidth = 32;
	if( !m_iHeight ) m_iHeight = 32;

	UI_ScaleCoords( &m_iX, &m_iY, &m_iWidth, &m_iHeight );
}

/*
=================
CMenuCheckBox::Key
=================
*/
const char *CMenuCheckBox::Key( int key, int down )
{
	const char	*sound = 0;

	switch( key )
	{
	case K_MOUSE1:
		if(!( m_iFlags & QMF_HASMOUSEFOCUS ))
			break;
		sound = uiSoundGlow;
		break;
	case K_ENTER:
	case K_KP_ENTER:
	case K_AUX1:
		//if( !down ) return sound;
		if( m_iFlags & QMF_MOUSEONLY )
			break;
		sound = uiSoundGlow;
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
			else event = QM_CHANGED;
			if( !down ) this->enabled = !this->enabled;	// apply on release
			_Event( event );
		}
	}
	else if( down )
	{
		if( sound )
		{
			this->enabled = !this->enabled;
			_Event( QM_CHANGED );
		}
	}
	return sound;
}

/*
=================
CMenuCheckBox::Draw
=================
*/
void CMenuCheckBox::Draw( void )
{
	int	justify;
	int	shadow;
	int	textOffset, y;

	if( m_iFlags & QMF_LEFT_JUSTIFY )
		justify = 0;
	else if( m_iFlags & QMF_CENTER_JUSTIFY )
		justify = 1;
	else if( m_iFlags & QMF_RIGHT_JUSTIFY )
		justify = 2;

	shadow = (m_iFlags & QMF_DROPSHADOW);

	y = m_iY + (m_iHeight>>2);
	textOffset = m_iX + (m_iWidth * 1.7f);
	UI_DrawString( textOffset, y, strlen( m_szName ) * m_iCharWidth, m_iCharHeight, m_szName, uiColorHelp, true, m_iCharWidth, m_iCharHeight, justify, shadow );

	if( m_szStatusText && m_iFlags & QMF_NOTIFY )
	{
		int	charW, charH;
		int	x, w;

		charW = UI_SMALL_CHAR_WIDTH;
		charH = UI_SMALL_CHAR_HEIGHT;

		UI_ScaleCoords( NULL, NULL, &charW, &charH );

		x = 250;
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
		UI_DrawPic( m_iX, m_iY, m_iWidth, m_iHeight, uiColorWhite, this->grayedPic );
		return; // grayed
	}

	if(( m_iFlags & QMF_MOUSEONLY ) && !( m_iFlags & QMF_HASMOUSEFOCUS )
	   || ( this != m_pParent->ItemAtCursor() ) )
	{
		if( !this->enabled )
			UI_DrawPic( m_iX, m_iY, m_iWidth, m_iHeight, m_iColor, this->emptyPic );
		else UI_DrawPic( m_iX, m_iY, m_iWidth, m_iHeight, m_iColor, this->checkPic );
		return; // no focus
	}

	if( m_iFlags & QMF_HIGHLIGHTIFFOCUS && !this->enabled )
	{
		UI_DrawPic( m_iX, m_iY, m_iWidth, m_iHeight, m_iFocusColor, this->focusPic );
	}
	else if( !this->enabled )
	{
		UI_DrawPic( m_iX, m_iY, m_iWidth, m_iHeight, m_iColor, this->emptyPic );
	}
	else if( m_iFlags & QMF_HIGHLIGHTIFFOCUS )
	{
		// use two textures for it. Second is just focus texture, slightly orange. Looks pretty.
		UI_DrawPic( m_iX, m_iY, m_iWidth, m_iHeight, m_iColor, UI_CHECKBOX_PRESSED );
		UI_DrawPicAdditive( m_iX, m_iY, m_iWidth, m_iHeight, uiInputTextColor, this->focusPic );
	}
	else
	{
		UI_DrawPic( m_iX, m_iY, m_iWidth, m_iHeight, m_iColor, this->checkPic );
	}
}
