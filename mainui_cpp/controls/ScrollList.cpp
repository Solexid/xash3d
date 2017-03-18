/*
ScrollList.h - animated button with picture
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
#include "ScrollList.h"
#include "utils.h"


/*
=================
menuScrollList_t::Init
=================
*/
void CMenuScrollList::VidInit( )
{
	if( !m_szName ) m_szName = "";

	m_iCharWidth = 1;
	m_iCharHeight = 1;
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

	if( m_iCharWidth < 1 ) m_iCharWidth = 1;
	if( m_iCharHeight < 1 ) m_iCharHeight = 1;

	if(!(m_iFlags & (QMF_LEFT_JUSTIFY|QMF_CENTER_JUSTIFY|QMF_RIGHT_JUSTIFY)))
		m_iFlags |= QMF_LEFT_JUSTIFY;

	if( !m_iColor ) m_iColor = uiPromptTextColor;
	if( !m_iFocusColor ) m_iFocusColor = uiPromptFocusColor;
	if( !upArrow ) upArrow = UI_UPARROW;
	if( !upArrowFocus ) upArrowFocus = UI_UPARROWFOCUS;
	if( !downArrow ) downArrow = UI_DOWNARROW;
	if( !downArrowFocus ) downArrowFocus = UI_DOWNARROWFOCUS;

//	curItem = 0;
	topItem = 0;
	numItems = 0;
	highlight = -1;

	// count number of items
	while( itemNames[numItems] )
		numItems++;

	// scale the center box
	m_iX2 = m_iX;
	m_iY2 = m_iY;
	m_iWidth2 = m_iWidth;
	m_iHeight2 = m_iHeight;
	UI_ScaleCoords( &m_iX2, &m_iY2, &m_iWidth2, &m_iHeight2 );

	// calculate number of visible rows
	numRows = (m_iHeight2 / m_iCharHeight) - 2;
	if( numRows > numItems ) numRows = numItems;

	// extend the height so it has room for the arrows
	m_iHeight2 += (m_iWidth / 4);

	// calculate new Y for the control
	m_iY -= (m_iWidth / 8);

	UI_ScaleCoords( &m_iX, &m_iY, &m_iWidth, &m_iHeight );
}
/*
=================
menuScrollList_t::Key
=================
*/
const char *CMenuScrollList::Key( int key, int down )
{
	const char	*sound = 0;
	int		arrowWidth, arrowHeight, upX, upY, downX, downY;
	int		i, y;
	bool noscroll = false;

	if( !down )
	{
		this->scrollBarSliding = false;
		return uiSoundNull;
	}

	switch( key )
	{
	case K_MOUSE1:
		noscroll = true; // don't scroll to current when mouse used
		if(!( m_iFlags & QMF_HASMOUSEFOCUS ))
			break;

		// use fixed size for arrows
		arrowWidth = 24;
		arrowHeight = 24;

		UI_ScaleCoords( NULL, NULL, &arrowWidth, &arrowHeight );

		// glue with right top and right bottom corners
		upX = m_iX2 + m_iWidth2 - arrowWidth;
		upY = m_iY2 + UI_OUTLINE_WIDTH;
		downX = m_iX2 + m_iWidth2 - arrowWidth;
		downY = m_iY2 + (m_iHeight2 - arrowHeight) - UI_OUTLINE_WIDTH;

		// ADAMIX
		if( UI_CursorInRect( upX, upY + arrowHeight, arrowWidth, this->scrollBarY - upY - arrowHeight ) ||
			  UI_CursorInRect( upX, this->scrollBarY + this->scrollBarHeight , arrowWidth, downY - ( this->scrollBarY + this->scrollBarHeight ) ) )
		{
			this->scrollBarSliding = true;
			//break;
		}
		// ADAMIX END

		// Now see if either up or down has focus
		if( UI_CursorInRect( upX, upY, arrowWidth, arrowHeight ))
		{
			if( this->topItem > 5 )
			{
				this->topItem-=5;
				sound = uiSoundMove;
			}
			else
			{
				this->topItem = 0;
				sound = uiSoundBuzz;
			}
			break;
		}
		else if( UI_CursorInRect( downX, downY, arrowWidth, arrowHeight ))
		{
			if( this->topItem < this->numItems - this->numRows - 5 )
			{
				this->topItem+=5;
				sound = uiSoundMove;
			}
			else
			{
				this->topItem = this->numItems - this->numRows;
				sound = uiSoundBuzz;
			}
			break;
		}

		// see if an item has been selected
		y = m_iY2 + m_iCharHeight;
		for( i = this->topItem; i < this->topItem + this->numRows; i++, y += m_iCharHeight )
		{
			if( !this->itemNames[i] )
				break; // done

			if( UI_CursorInRect( m_iX, y, m_iWidth - arrowWidth, m_iCharHeight ))
			{
				this->curItem = i;
				sound = uiSoundNull;
				break;
			}
		}
		break;
	case K_HOME:
	case K_KP_HOME:
		if( this->curItem != 0 )
		{
			this->curItem = 0;
			sound = uiSoundMove;
		}
		else sound = uiSoundBuzz;
		break;
	case K_END:
	case K_KP_END:
		if( this->curItem != this->numItems - 1 )
		{
			this->curItem = this->numItems - 1;
			sound = uiSoundMove;
		}
		else sound = uiSoundBuzz;
		break;
	case K_PGUP:
	case K_KP_PGUP:
		if( this->curItem != 0 )
		{
			this->curItem -= 2;
			if( this->curItem < 0 )
				this->curItem = 0;
			sound = uiSoundMove;
		}
		else sound = uiSoundBuzz;
		break;
	case K_PGDN:
	case K_KP_PGDN:
		if( this->curItem != this->numItems - 1 )
		{
			this->curItem += 2;
			if( this->curItem > this->numItems - 1 )
				this->curItem = this->numItems - 1;
			sound = uiSoundMove;
		}
		else sound = uiSoundBuzz;
		break;
	case K_UPARROW:
	case K_KP_UPARROW:
	case K_MWHEELUP:
		if( this->curItem != 0 )
		{
			this->curItem--;
			sound = uiSoundMove;
		}
		else sound = uiSoundBuzz;
		break;
	case K_DOWNARROW:
	case K_MWHEELDOWN:
		if( this->numItems > 0 && this->curItem != this->numItems - 1 )
		{
			this->curItem++;
			sound = uiSoundMove;
		}
		else sound = uiSoundBuzz;
		break;
	}
	if( !noscroll )
	{
		if( this->curItem < this->topItem )
			this->topItem = this->curItem;
		if( this->curItem > this->topItem + this->numRows - 1 )
			this->topItem = this->curItem - this->numRows + 1;
		if( this->topItem < 0 ) this->topItem = 0;
		if( this->topItem > this->numItems - this->numRows )
			this->topItem = this->numItems - this->numRows;
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
menuScrollList_t::Draw
=================
*/
void CMenuScrollList::Draw( )
{
	int	justify = 0;
	int	shadow;
	int	i, x, y, w, h;
	int	selColor = 0xFF503818; // Red 80, Green 56, Blue 24, Alpha 255
	int	arrowWidth, arrowHeight, upX, upY, downX, downY;
	int	upFocus, downFocus, scrollbarFocus;

	if( m_iFlags & QMF_LEFT_JUSTIFY )
		justify = 0;
	else if( m_iFlags & QMF_CENTER_JUSTIFY )
		justify = 1;
	else if( m_iFlags & QMF_RIGHT_JUSTIFY )
		justify = 2;

	shadow = (m_iFlags & QMF_DROPSHADOW);

	// use fixed size for arrows
	arrowWidth = 24;
	arrowHeight = 24;

	UI_ScaleCoords( NULL, NULL, &arrowWidth, &arrowHeight );

	x = m_iX2;
	y = m_iY2;
	w = m_iWidth2;
	h = m_iHeight2;

	if( !this->background )
	{
		// draw the opaque outlinebox first
		UI_FillRect( x, y, w, h, uiColorBlack );
	}

	// hightlight the selected item
	if( !( m_iFlags & QMF_GRAYED ))
	{
		y = m_iY2 + m_iCharHeight;
		for( i = this->topItem; i < this->topItem + this->numRows; i++, y += m_iCharHeight )
		{
			if( !this->itemNames[i] )
				break;		// Done
			// hightlighted item
			if( i == this->highlight )
			{
				UI_FillRect( m_iX, y, m_iWidth - arrowWidth, m_iCharHeight, 0xFF383838 );
			}

			if( i == this->curItem )
			{
				UI_FillRect( m_iX, y, m_iWidth - arrowWidth, m_iCharHeight, selColor );
			}
		}
	}

	if( this->background )
	{
		// get size and position for the center box
		UI_DrawPic( x, y, w, h, uiColorWhite, this->background );
	}
	else
	{

		int color;

		if( m_iFlags & QMF_HIGHLIGHTIFFOCUS && m_iFlags & QMF_HASKEYBOARDFOCUS )
			color = uiInputTextColor;
		else
			color = uiInputFgColor;

		x = m_iX2 - UI_OUTLINE_WIDTH;
		y = m_iY2;
		w = UI_OUTLINE_WIDTH;
		h = m_iHeight2;

		// draw left
		UI_FillRect( x, y, w, h, color );

		x = m_iX2 + m_iWidth2;
		y = m_iY2;
		w = UI_OUTLINE_WIDTH;
		h = m_iHeight2;

		// draw right
		UI_FillRect( x, y, w, h, color );

		x = m_iX2;
		y = m_iY2;
		w = m_iWidth2 + UI_OUTLINE_WIDTH;
		h = UI_OUTLINE_WIDTH;

		// draw top
		UI_FillRect( x, y, w, h, color );

		// draw bottom
		x = m_iX2;
		y = m_iY2 + m_iHeight2 - UI_OUTLINE_WIDTH;
		w = m_iWidth2 + UI_OUTLINE_WIDTH;
		h = UI_OUTLINE_WIDTH;

		UI_FillRect( x, y, w, h, color );
	}

	// glue with right top and right bottom corners
	upX = m_iX2 + m_iWidth2 - arrowWidth;
	upY = m_iY2 + UI_OUTLINE_WIDTH;
	downX = m_iX2 + m_iWidth2 - arrowWidth;
	downY = m_iY2 + (m_iHeight2 - arrowHeight) - UI_OUTLINE_WIDTH;

	float step = (this->numItems <= 1 ) ? 1 : (downY - upY - arrowHeight) / (float)(this->numItems - 1);

	if( cursorDown && !this->scrollBarSliding )
	{
		if( UI_CursorInRect( m_iX2, m_iY2, m_iWidth2 - arrowWidth, m_iHeight2 ))
		{
			static float ac_y = 0;
			ac_y += cursorDY;
			cursorDY = 0;
			if( ac_y > m_iCharHeight / 2.0f )
			{
				this->topItem -= ac_y/ m_iCharHeight - 0.5;
				if( this->topItem < 0 )
					this->topItem = 0;
				ac_y = 0;
			}
			if( ac_y < -m_iCharHeight / 2.0f )
			{
				this->topItem -= ac_y/ m_iCharHeight - 0.5 ;
				if( this->topItem > this->numItems - this->numRows )
					this->topItem = this->numItems - this->numRows;
				ac_y = 0;
			}
		}
		else if( UI_CursorInRect( this->scrollBarX, this->scrollBarY, this->scrollBarWidth, this->scrollBarHeight ))
		{
			static float ac_y = 0;
			ac_y += cursorDY;
			cursorDY = 0;
			if( ac_y < -step )
			{
				this->topItem += ac_y / step + 0.5;
				if( this->topItem < 0 )
					this->topItem = 0;
				ac_y = 0;
			}
			if( ac_y > step )
			{
				this->topItem += ac_y / step + 0.5;
				if( this->topItem > this->numItems - this->numRows )
					this->topItem = this->numItems - this->numRows;
				ac_y = 0;
			}
		}

	}

	// draw the arrows base
	UI_FillRect( upX, upY + arrowHeight, arrowWidth, downY - upY - arrowHeight, uiInputFgColor );


	// ADAMIX
	this->scrollBarX = upX + m_iCharHeight/4;
	this->scrollBarWidth = arrowWidth - m_iCharHeight/4;

	if(((downY - upY - arrowHeight) - (((this->numItems-1)*m_iCharHeight)/2)) < 2)
	{
		this->scrollBarHeight = (downY - upY - arrowHeight) - (step * (this->numItems - this->numRows));
		this->scrollBarY = upY + arrowHeight + (step*this->topItem);
	}
	else
	{
		this->scrollBarHeight = downY - upY - arrowHeight - (((this->numItems- this->numRows) * m_iCharHeight) / 2);
		this->scrollBarY = upY + arrowHeight + (((this->topItem) * m_iCharHeight)/2);
	}

	if( this->scrollBarSliding )
	{
		int dist = uiStatic.cursorY - this->scrollBarY - (this->scrollBarHeight>>1);

		if((((dist / 2) > (m_iCharHeight / 2)) || ((dist / 2) < (m_iCharHeight / 2))) && this->topItem <= (this->numItems - this->numRows ) && this->topItem >= 0)
		{
			//_Event( QM_CHANGED );

			if((dist / 2) > ( m_iCharHeight / 2 ) && this->topItem < ( this->numItems - this->numRows - 1 ))
			{
				this->topItem++;
			}

			if((dist / 2) < -(m_iCharHeight / 2) && this->topItem > 0 )
			{
				this->topItem--;
			}
		}

		//this->topItem = this->curItem - this->numRows + 1;
		if( this->topItem < 0 ) this->topItem = 0;
		if( this->topItem > ( this->numItems - this->numRows - 1 ))
			this->topItem = this->numItems - this->numRows - 1;
	}

	if( this->scrollBarSliding )
	{
		// Draw scrollbar background
		UI_FillRect ( this->scrollBarX, upY + arrowHeight, this->scrollBarWidth, downY - upY - arrowHeight, uiColorBlack);
	}

	// ADAMIX END
	// draw the arrows
	if( m_iFlags & QMF_GRAYED )
	{
		UI_DrawPic( upX, upY, arrowWidth, arrowHeight, uiColorDkGrey, this->upArrow );
		UI_DrawPic( downX, downY, arrowWidth, arrowHeight, uiColorDkGrey, this->downArrow );
	}
	else
	{
		scrollbarFocus = UI_CursorInRect( this->scrollBarX, this->scrollBarY, this->scrollBarWidth, this->scrollBarHeight );

		// special case if we sliding but lost focus
		if( this->scrollBarSliding ) scrollbarFocus = true;

		// Draw scrollbar itself
		UI_FillRect( this->scrollBarX, this->scrollBarY, this->scrollBarWidth, this->scrollBarHeight, scrollbarFocus ? uiInputTextColor : uiColorBlack );

		if(this != m_pParent->ItemAtCursor())
		{
			UI_DrawPic( upX, upY, arrowWidth, arrowHeight, uiColorWhite, this->upArrow );
			UI_DrawPic( downX, downY, arrowWidth, arrowHeight, uiColorWhite, this->downArrow );
		}
		else
		{
			// see which arrow has the mouse focus
			upFocus = UI_CursorInRect( upX, upY, arrowWidth, arrowHeight );
			downFocus = UI_CursorInRect( downX, downY, arrowWidth, arrowHeight );

			if(!( m_iFlags & QMF_FOCUSBEHIND ))
			{
				UI_DrawPic( upX, upY, arrowWidth, arrowHeight, uiColorWhite, this->upArrow );
				UI_DrawPic( downX, downY, arrowWidth, arrowHeight, uiColorWhite, this->downArrow );
			}

			if( m_iFlags & QMF_HIGHLIGHTIFFOCUS )
			{
				UI_DrawPic( upX, upY, arrowWidth, arrowHeight, uiColorWhite, (upFocus) ? this->upArrowFocus : this->upArrow );
				UI_DrawPic( downX, downY, arrowWidth, arrowHeight, uiColorWhite, (downFocus) ? this->downArrowFocus : this->downArrow );
			}
			else if( m_iFlags & QMF_PULSEIFFOCUS )
			{
				int	color;

				color = PackAlpha( m_iColor, 255 * (0.5 + 0.5 * sin( (float)uiStatic.realTime / UI_PULSE_DIVISOR )));

				UI_DrawPic( upX, upY, arrowWidth, arrowHeight, (upFocus) ? color : m_iColor, (upFocus) ? this->upArrowFocus : this->upArrow );
				UI_DrawPic( downX, downY, arrowWidth, arrowHeight, (downFocus) ? color : m_iColor, (downFocus) ? this->downArrowFocus : this->downArrow );
			}

			if( m_iFlags & QMF_FOCUSBEHIND )
			{
				UI_DrawPic( upX, upY, arrowWidth, arrowHeight, m_iColor, this->upArrow );
				UI_DrawPic( downX, downY, arrowWidth, arrowHeight, m_iColor, this->downArrow );
			}
		}
	}

	// Draw the list
	x = m_iX2;
	w = m_iWidth2;
	h = m_iCharHeight;
	y = m_iY2 + m_iCharHeight;

	// prevent the columns out of rectangle bounds
	EngFuncs::PIC_EnableScissor( x, y, m_iWidth - arrowWidth - uiStatic.outlineWidth, m_iHeight );

	for( i = this->topItem; i < this->topItem + this->numRows; i++, y += m_iCharHeight )
	{
		if( !this->itemNames[i] )
			break;	// done

		if( m_iFlags & QMF_GRAYED )
		{
			UI_DrawString( x, y, w, h, this->itemNames[i], uiColorDkGrey, true, m_iCharWidth, m_iCharHeight, justify, shadow );
			continue;	// grayed
		}

		if( i != this->curItem )
		{
			UI_DrawString( x, y, w, h, this->itemNames[i], m_iColor, false, m_iCharWidth, m_iCharHeight, justify, shadow );
			continue;	// no focus
		}

		if(!( m_iFlags & QMF_FOCUSBEHIND ))
			UI_DrawString( x, y, w, h, this->itemNames[i], m_iColor, false, m_iCharWidth, m_iCharHeight, justify, shadow );

		if( m_iFlags & QMF_HIGHLIGHTIFFOCUS )
			UI_DrawString( x, y, w, h, this->itemNames[i], m_iFocusColor, false, m_iCharWidth, m_iCharHeight, justify, shadow );
		else if( m_iFlags & QMF_PULSEIFFOCUS )
		{
			int	color;

			color = PackAlpha( m_iColor, 255 * (0.5 + 0.5 * sin( (float)uiStatic.realTime / UI_PULSE_DIVISOR )));

			UI_DrawString( x, y, w, h, this->itemNames[i], color, false, m_iCharWidth, m_iCharHeight, justify, shadow );
		}

		if( m_iFlags & QMF_FOCUSBEHIND )
			UI_DrawString( x, y, w, h, this->itemNames[i], m_iColor, false, m_iCharWidth, m_iCharHeight, justify, shadow );
	}

	EngFuncs::PIC_DisableScissor();
}
