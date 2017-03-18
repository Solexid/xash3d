/*
SpinControl.h - spin selector
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
#include "SpinControl.h"
#include "utils.h"

/*
=================
CMenuSpinControl::Init
=================
*/
void CMenuSpinControl::VidInit( void )
{
	if( !m_szName ) m_szName = "";	// this is also the text displayed

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

	UI_ScaleCoords( NULL, NULL, &m_iCharWidth, &m_iCharHeight );

	if(!( m_iFlags & (QMF_LEFT_JUSTIFY|QMF_CENTER_JUSTIFY|QMF_RIGHT_JUSTIFY)))
		m_iFlags |= QMF_LEFT_JUSTIFY;

	if( !m_iColor ) m_iColor = uiColorHelp;
	if( !m_iFocusColor ) m_iFocusColor = uiPromptTextColor;
	if( !this->leftArrow ) this->leftArrow = UI_LEFTARROW;
	if( !this->leftArrowFocus ) this->leftArrowFocus = UI_LEFTARROWFOCUS;
	if( !this->rightArrow ) this->rightArrow = UI_RIGHTARROW;
	if( !this->rightArrowFocus ) this->rightArrowFocus = UI_RIGHTARROWFOCUS;

	// scale the center box
	m_iX2 = m_iX;
	m_iY2 = m_iY;
	m_iWidth2 = m_iWidth;
	m_iHeight2 = m_iHeight;
	UI_ScaleCoords( &m_iX2, &m_iY2, &m_iWidth2, &m_iHeight2 );

	// extend the width so it has room for the arrows
	m_iWidth += (m_iHeight * 3);

	// calculate new X for the control
	m_iX -= (m_iHeight + (m_iHeight/2));

	UI_ScaleCoords( &m_iX, &m_iY, &m_iWidth, &m_iHeight );
}

/*
=================
CMenuSpinControl::Key
=================
*/
const char *CMenuSpinControl::Key( int key, int down )
{
	const char	*sound = 0;
	int		arrowWidth, arrowHeight, leftX, leftY, rightX, rightY;

	if( !down ) return uiSoundNull;

	switch( key )
	{
	case K_MOUSE1:
	case K_MOUSE2:
		if( !( m_iFlags & QMF_HASMOUSEFOCUS ))
			break;

		// calculate size and position for the arrows
		arrowWidth = m_iHeight + (UI_OUTLINE_WIDTH * 2);
		arrowHeight = m_iHeight + (UI_OUTLINE_WIDTH * 2);

		leftX = m_iX + UI_OUTLINE_WIDTH;
		leftY = m_iY - UI_OUTLINE_WIDTH;
		rightX = m_iX + (m_iWidth - arrowWidth) - UI_OUTLINE_WIDTH;
		rightY = m_iY - UI_OUTLINE_WIDTH;

		// now see if either left or right arrow has focus
		if( UI_CursorInRect( leftX, leftY, arrowWidth, arrowHeight ))
		{
			if( this->curValue > this->minValue )
			{
				this->curValue -= this->range;
				if( this->curValue < this->minValue )
					this->curValue = this->minValue;
				sound = uiSoundMove;
			}
			else sound = uiSoundBuzz;
		}
		else if( UI_CursorInRect( rightX, rightY, arrowWidth, arrowHeight ))
		{
			if( this->curValue < this->maxValue )
			{
				this->curValue += this->range;
				if( this->curValue > this->maxValue )
					this->curValue = this->maxValue;
				sound = uiSoundMove;
			}
			else sound = uiSoundBuzz;
		}
		break;
	case K_LEFTARROW:
	case K_KP_LEFTARROW:
		if( m_iFlags & QMF_MOUSEONLY )
			break;
		if( this->curValue > this->minValue )
		{
			this->curValue -= this->range;
			if( this->curValue < this->minValue )
				this->curValue = this->minValue;
			sound = uiSoundMove;
		}
		else sound = uiSoundBuzz;
		break;
	case K_RIGHTARROW:
	case K_KP_RIGHTARROW:
		if( m_iFlags & QMF_MOUSEONLY )
			break;

		if( this->curValue < this->maxValue )
		{
			this->curValue += this->range;
			if( this->curValue > this->maxValue )
				this->curValue = this->maxValue;
			sound = uiSoundMove;
		}
		else sound = uiSoundBuzz;
		break;
	}

	if( sound && ( m_iFlags & QMF_SILENT ))
		sound = uiSoundNull;

	if( sound )
	{
		if( sound != uiSoundBuzz )
			_Event( QM_CHANGED );
	}
	return sound;
}

/*
=================
CMenuSpinControl::Draw
=================
*/
void CMenuSpinControl::Draw( void )
{
	int	justify = 0;
	int	shadow;
	int	x, y, w, h;
	int	arrowWidth, arrowHeight, leftX, leftY, rightX, rightY;
	int	leftFocus, rightFocus;

	if( m_iFlags & QMF_LEFT_JUSTIFY )
		justify = 0;
	else if( m_iFlags & QMF_CENTER_JUSTIFY )
		justify = 1;
	else if( m_iFlags & QMF_RIGHT_JUSTIFY )
		justify = 2;

	shadow = (m_iFlags & QMF_DROPSHADOW);

	// calculate size and position for the arrows
	arrowWidth = m_iHeight + (UI_OUTLINE_WIDTH * 2);
	arrowHeight = m_iHeight + (UI_OUTLINE_WIDTH * 2);

	leftX = m_iX + UI_OUTLINE_WIDTH;
	leftY = m_iY - UI_OUTLINE_WIDTH;
	rightX = m_iX + (m_iWidth - arrowWidth) - UI_OUTLINE_WIDTH;
	rightY = m_iY - UI_OUTLINE_WIDTH;

	// get size and position for the center box
	w = m_iWidth2;
	h = m_iHeight2;
	x = m_iX2;
	y = m_iY2;

	if( this->background )
	{
		UI_DrawPic( x, y, w, h, uiColorWhite, this->background );
	}
	else
	{
		// draw the background
		UI_FillRect( x, y, w, h, uiColorBlack );

		// draw the rectangle
		UI_DrawRectangle( x, y, w, h, uiInputFgColor );
	}

	if( m_iFlags & QMF_GRAYED )
	{
		UI_DrawString( x, y, w, h, m_szName, uiColorDkGrey, true, m_iCharWidth, m_iCharHeight, justify, shadow );
		UI_DrawPic( leftX, leftY, arrowWidth, arrowHeight, uiColorDkGrey, this->leftArrow );
		UI_DrawPic( rightX, rightY, arrowWidth, arrowHeight, uiColorDkGrey, this->rightArrow );
		return; // grayed
	}

	if(this != m_pParent->ItemAtCursor())
	{
		UI_DrawString(x, y, w, h, m_szName, m_iColor, false, m_iCharWidth, m_iCharHeight, justify, shadow );
		UI_DrawPic(leftX, leftY, arrowWidth, arrowHeight, m_iColor, this->leftArrow);
		UI_DrawPic(rightX, rightY, arrowWidth, arrowHeight, m_iColor, this->rightArrow);
		return;		// No focus
	}

	// see which arrow has the mouse focus
	leftFocus = UI_CursorInRect( leftX, leftY, arrowWidth, arrowHeight );
	rightFocus = UI_CursorInRect( rightX, rightY, arrowWidth, arrowHeight );

	if( !( m_iFlags & QMF_FOCUSBEHIND ))
	{
		UI_DrawString( x, y, w, h, m_szName, m_iColor, false, m_iCharWidth, m_iCharHeight, justify, shadow );
		UI_DrawPic( leftX, leftY, arrowWidth, arrowHeight, m_iColor, this->leftArrow );
		UI_DrawPic( rightX, rightY, arrowWidth, arrowHeight, m_iColor, this->rightArrow );
	}

	if( m_iFlags & QMF_HIGHLIGHTIFFOCUS )
	{
		UI_DrawString( x, y, w, h, m_szName, m_iFocusColor, false, m_iCharWidth, m_iCharHeight, justify, shadow );
		UI_DrawPic( leftX, leftY, arrowWidth, arrowHeight, m_iColor, (leftFocus) ? this->leftArrowFocus : this->leftArrow );
		UI_DrawPic( rightX, rightY, arrowWidth, arrowHeight, m_iColor, (rightFocus) ? this->rightArrowFocus : this->rightArrow );
	}
	else if( m_iFlags & QMF_PULSEIFFOCUS )
	{
		int	color;

		color = PackAlpha( m_iColor, 255 * (0.5 + 0.5 * sin( (float)uiStatic.realTime / UI_PULSE_DIVISOR )));

		UI_DrawString( x, y, w, h, m_szName, color, false, m_iCharWidth, m_iCharHeight, justify, shadow );
		UI_DrawPic( leftX, leftY, arrowWidth, arrowHeight, (leftFocus) ? color : m_iColor, (leftFocus) ? this->leftArrowFocus : this->leftArrow );
		UI_DrawPic( rightX, rightY, arrowWidth, arrowHeight, (rightFocus) ? color : m_iColor, (rightFocus) ? this->rightArrowFocus : this->rightArrow );
	}

	if( m_iFlags & QMF_FOCUSBEHIND )
	{
		UI_DrawString( x, y, w, h, m_szName, m_iColor, false, m_iCharWidth, m_iCharHeight, justify, shadow );
		UI_DrawPic( leftX, leftY, arrowWidth, arrowHeight, m_iColor, this->leftArrow );
		UI_DrawPic( rightX, rightY, arrowWidth, arrowHeight, m_iColor, this->rightArrow );
	}
}
