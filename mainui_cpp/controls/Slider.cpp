/*
Slider.h - slider
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
#include "Slider.h"
#include "utils.h"

/*
=================
CMenuSlider::Init
=================
*/
void CMenuSlider::VidInit(  )
{
	if( !m_szName ) m_szName = "";	// this is also the text displayed

	if( !m_iWidth ) m_iWidth = 200;
	if( !m_iHeight) m_iHeight = 4;
	if( !m_iColor ) m_iColor = uiColorWhite;
	if( !m_iFocusColor ) m_iFocusColor = uiColorWhite;
	if( !this->range ) this->range = 1.0f;
	if( this->range < 0.05f ) this->range = 0.05f;

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

	// scale the center box
	m_iX2 = m_iX;
	m_iY2 = m_iY;
	m_iWidth2 = m_iWidth / 5.0f;
	m_iHeight2 = 4;

	UI_ScaleCoords( &m_iX2, &m_iY2, &m_iWidth2, &m_iHeight2 );
	UI_ScaleCoords( &m_iX, &m_iY, &m_iWidth, &m_iHeight );

	m_iY -= uiStatic.sliderWidth;
	m_iHeight += uiStatic.sliderWidth * 2;
	m_iY2 -= uiStatic.sliderWidth;

	this->numSteps = (this->maxValue - this->minValue) / this->range + 1;
	this->drawStep = (float)(m_iWidth - m_iWidth2) / (float)this->numSteps;
}

/*
=================
CMenuSlider::Key
=================
*/
const char *CMenuSlider::Key( int key, int down )
{
	int	sliderX;

	if( !down )
	{
		if( this->keepSlider )
		{
			// tell menu about changes
			_Event( QM_CHANGED );
			this->keepSlider = false; // button released
		}
		return uiSoundNull;
	}

	switch( key )
	{
	case K_MOUSE1:
		this->keepSlider = false;
		if( !UI_CursorInRect( m_iX, m_iY - 20, m_iWidth, m_iHeight + 40 ) )
			return uiSoundNull;

		// find the current slider position
		sliderX = m_iX2 + (this->drawStep * (this->curValue / this->range));
		this->keepSlider = true;
		int	dist, numSteps;

		// immediately move slider into specified place
		dist = uiStatic.cursorX - m_iX2 - (m_iWidth2>>2);
		numSteps = round(dist / this->drawStep);
		this->curValue = bound( this->minValue, numSteps * this->range, this->maxValue );

		// tell menu about changes
		_Event( QM_CHANGED );

		return uiSoundNull;
		break;
	case K_LEFTARROW:
		this->curValue -= this->range;

		if( this->curValue < this->minValue )
		{
			this->curValue = this->minValue;
			return uiSoundBuzz;
		}

		// tell menu about changes
		_Event( QM_CHANGED );

		return uiSoundKey;
		break;
	case K_RIGHTARROW:
		this->curValue += this->range;

		if( this->curValue > this->maxValue )
		{
			this->curValue = this->maxValue;
			return uiSoundBuzz;
		}

		// tell menu about changes
		_Event( QM_CHANGED );

		return uiSoundKey;
		break;
	}

	return 0;
}

/*
=================
CMenuSlider::Draw
=================
*/
void CMenuSlider::Draw( void )
{
	int	justify = 0;
	int	shadow;
	int	textHeight, sliderX;

	if( m_iFlags & QMF_LEFT_JUSTIFY )
		justify = 0;
	else if( m_iFlags & QMF_CENTER_JUSTIFY )
		justify = 1;
	else if( m_iFlags & QMF_RIGHT_JUSTIFY )
		justify = 2;

	shadow = (m_iFlags & QMF_DROPSHADOW);

	if( this->keepSlider )
	{
		if( !UI_CursorInRect( m_iX, m_iY - 40, m_iWidth, m_iHeight + 80 ) )
			this->keepSlider = false;
		else
		{
			int	dist, numSteps;

			// move slider follow the holded mouse button
			dist = uiStatic.cursorX - m_iX2 - (m_iWidth2>>2);
			numSteps = round(dist / this->drawStep);
			this->curValue = bound( this->minValue, numSteps * this->range, this->maxValue );

			// tell menu about changes
			_Event( QM_CHANGED );
		}
	}

	// keep value in range
	this->curValue = bound( this->minValue, this->curValue, this->maxValue );

	// calc slider position
	sliderX = m_iX2 + (this->drawStep * (this->curValue / this->range)); // TODO: fix behaviour when values goes negative
	//sliderX = bound( m_iX2, sliderX, m_iX2 + m_iWidth - uiStatic.sliderWidth);

	UI_DrawRectangleExt( m_iX, m_iY + uiStatic.sliderWidth, m_iWidth, m_iHeight2, uiInputBgColor, uiStatic.sliderWidth );
	if( m_iFlags & QMF_HIGHLIGHTIFFOCUS && this == m_pParent->ItemAtCursor())
		UI_DrawPic( sliderX, m_iY2, m_iWidth2, m_iHeight, uiColorHelp, UI_SLIDER_MAIN );
	else
		UI_DrawPic( sliderX, m_iY2, m_iWidth2, m_iHeight, uiColorWhite, UI_SLIDER_MAIN );


	textHeight = m_iY - (m_iCharHeight * 1.5f);
	UI_DrawString( m_iX, textHeight, m_iWidth, m_iCharHeight, m_szName, uiColorHelp, true, m_iCharWidth, m_iCharHeight, justify, shadow );
}
