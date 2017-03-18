/*
Field.h - edit field
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
#include "Field.h"
#include "utils.h"

/*
=================
CMenuField::Init
=================
*/
void CMenuField::VidInit( void )
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
		if( m_iCharWidth < 1 ) m_iCharWidth = UI_MED_CHAR_WIDTH;
		if( m_iCharHeight < 1 ) m_iCharHeight = UI_MED_CHAR_HEIGHT;
	}

	UI_ScaleCoords( NULL, NULL, &m_iCharWidth, &m_iCharHeight );

	if( !(m_iFlags & (QMF_LEFT_JUSTIFY|QMF_CENTER_JUSTIFY|QMF_RIGHT_JUSTIFY)))
		m_iFlags |= QMF_LEFT_JUSTIFY;

	if( !m_iColor ) m_iColor = uiInputTextColor;
	if( !m_iFocusColor ) m_iFocusColor = uiInputTextColor;

	this->maxLength++;
	if( this->maxLength <= 1 || this->maxLength >= UI_MAX_FIELD_LINE )
		this->maxLength = UI_MAX_FIELD_LINE - 1;

	UI_ScaleCoords( &m_iX, &m_iY, &m_iWidth, &m_iHeight );

	// calculate number of visible characters
	widthInChars = (m_iWidth / m_iCharWidth);

	this->cursor = strlen( this->buffer );
}

/*
================
CMenuField::_Event
================
*/
void CMenuField::_Event( int ev )
{
	switch( ev )
	{
	case QM_LOSTFOCUS: EngFuncs::EnableTextInput( false ); break;
	case QM_GOTFOCUS: EngFuncs::EnableTextInput( true ); break;
	}

	CMenuBaseItem::_Event( ev );
}

/*
================
CMenuField::Paste
================
*/
void CMenuField::Paste( void )
{
	char	*str;
	int	pasteLen, i;

	str = EngFuncs::GetClipboardData ();
	if( !str ) return;

	// send as if typed, so insert / overstrike works properly
	pasteLen = strlen( str );
	for( i = 0; i < pasteLen; i++ )
		UI_CharEvent( str[i] );
	FREE( str );
}

/*
================
CMenuField::Clear
================
*/
void CMenuField::Clear( void )
{
	memset( this->buffer, 0, UI_MAX_FIELD_LINE );
	this->cursor = 0;
	this->scroll = 0;
}


/*
=================
CMenuField::Key
=================
*/
const char *CMenuField::Key( int key, int down )
{
	int	len;

	if( !down ) return 0;

	// clipboard paste
	if((( key == K_INS ) || ( key == K_KP_INS )) && EngFuncs::EngFuncs::KEY_IsDown( K_SHIFT ))
	{
		Paste();
		return 0;
	}

	len = strlen( this->buffer );

	if( key == K_INS )
	{
		// toggle overstrike mode
		EngFuncs::KEY_SetOverstrike( !EngFuncs::KEY_GetOverstrike( ));
		return uiSoundNull; // handled
	}

	// previous character
	if( key == K_LEFTARROW )
	{
		if( this->cursor > 0 ) this->cursor = EngFuncs::UtfMoveLeft( this->buffer, this->cursor );
		if( this->cursor < this->scroll ) this->scroll--;
		return uiSoundNull;
	}

	// next character
	if( key == K_RIGHTARROW )
	{
		if( this->cursor < len ) this->cursor = EngFuncs::UtfMoveRight( this->buffer, this->cursor, len );
		if( this->cursor >= this->scroll + widthInChars && this->cursor <= len )
			this->scroll++;
		return uiSoundNull;
	}

	// first character
	if( key == K_HOME )
	{
		this->cursor = 0;
		return uiSoundNull;
	}

	// last character
	if( key == K_END )
	{
		this->cursor = len;
		return uiSoundNull;
	}

	if( key == K_BACKSPACE )
	{
		if( this->cursor > 0 )
		{
			int pos = EngFuncs::UtfMoveLeft( this->buffer, this->cursor );
			memmove( this->buffer + pos, this->buffer + this->cursor, len - this->cursor + 1 );
			this->cursor = pos;
			if( this->scroll ) this->scroll--;
		}
	}
	if( key == K_DEL )
	{
		if( this->cursor < len )
			memmove( this->buffer + this->cursor, this->buffer + this->cursor + 1, len - this->cursor );
	}

	if( key == K_MOUSE1 )
	{
		float y = m_iY;

		if( y > ScreenHeight - m_iHeight - 40 )
			y = ScreenHeight - m_iHeight - 15;

		if( UI_CursorInRect( m_iX, y, m_iWidth, m_iHeight ) )
		{
			int charpos = (uiStatic.cursorX - m_iX) / m_iCharWidth;
			this->cursor = this->scroll + charpos;
			if( charpos == 0 && this->scroll )
				this->scroll--;
			if( charpos == widthInChars && this->scroll < len - 1 )
				this->scroll++;
			if( this->scroll > len )
				this->scroll = len;
			if( this->cursor > len )
				this->cursor = len;
		}
	}

	_Event( QM_CHANGED );
	return 0;
}

/*
=================
CMenuField::Char
=================
*/
void CMenuField::Char( int key )
{
	int	len;

	if( key == 'v' - 'a' + 1 )
	{
		// ctrl-v is paste
		Paste();
		return;
	}

	if( key == 'c' - 'a' + 1 )
	{
		// ctrl-c clears the field
		Clear( );
		return;
	}

	len = strlen( this->buffer );

	if( key == 'a' - 'a' + 1 )
	{
		// ctrl-a is home
		this->cursor = 0;
		this->scroll = 0;
		return;
	}

	if( key == 'e' - 'a' + 1 )
	{
		// ctrl-e is end
		this->cursor = len;
		this->scroll = this->cursor - widthInChars;
		return;
	}

	// ignore any other non printable chars
	//if( key < 32 ) return;

	if( key == '^' && !( m_iFlags & QMF_ALLOW_COLORSTRINGS ))
	{
		// ignore color key-symbol
		return;
	}

	if( m_iFlags & QMF_NUMBERSONLY )
	{
		if( key < '0' || key > '9' )
			return;
	}

	if( m_iFlags & QMF_LOWERCASE )
		key = tolower( key );
	else if( m_iFlags & QMF_UPPERCASE )
		key = toupper( key );

	if( EngFuncs::KEY_GetOverstrike( ))
	{
		if( this->cursor == this->maxLength - 1 ) return;
		this->buffer[this->cursor] = key;
		this->cursor++;
	}
	else
	{
		// insert mode
		if( len == this->maxLength - 1 ) return; // all full
		memmove( this->buffer + this->cursor + 1, this->buffer + this->cursor, len + 1 - this->cursor );
		this->buffer[this->cursor] = key;
		this->cursor++;
	}

	if( this->cursor >= widthInChars ) this->scroll++;
	if( this->cursor == len + 1 ) this->buffer[this->cursor] = 0;

	_Event( QM_CHANGED );
}

/*
=================
CMenuField::Draw
=================
*/
void CMenuField::Draw( void )
{
	int	justify = 0;
	int	shadow;
	char	text[UI_MAX_FIELD_LINE];
	int	len, drawLen, prestep;
	int	cursor, x, textHeight;
	char	cursor_char[3];
	float y = m_iY;

	if( y > ScreenHeight - m_iHeight - 40 )
	{
		if(this == m_pParent->ItemAtCursor())
			y = ScreenHeight - m_iHeight - 15;
		else
			return;
	}

	if( m_iFlags & QMF_LEFT_JUSTIFY )
		justify = 0;
	else if( m_iFlags & QMF_CENTER_JUSTIFY )
		justify = 1;
	else if( m_iFlags & QMF_RIGHT_JUSTIFY )
		justify = 2;

	shadow = (m_iFlags & QMF_DROPSHADOW);

	cursor_char[1] = '\0';
	if( EngFuncs::KEY_GetOverstrike( ))
		cursor_char[0] = 11;
	else cursor_char[0] = 95;

	drawLen = widthInChars;
	len = strlen( this->buffer ) + 1;

	// guarantee that cursor will be visible
	if( len <= drawLen )
	{
		prestep = 0;
	}
	else
	{
		if( this->scroll + drawLen > len )
		{
			this->scroll = len - drawLen;
			if( this->scroll < 0 ) this->scroll = 0;
		}
		prestep = this->scroll;
	}

	if( prestep + drawLen > len )
		drawLen = len - prestep;

	// extract <drawLen> characters from the field at <prestep>
	if( drawLen >= UI_MAX_FIELD_LINE )
		Host_Error( "CMenuField::Draw: drawLen >= UI_MAX_FIELD_LINE\n" );

	memcpy( text, this->buffer + prestep, drawLen );
	text[drawLen] = 0;

	if( m_iFlags & QMF_HIDEINPUT )
	{
		for( int i = 0; i < drawLen; i++ )
			if( text[i] ) text[i] = '*';
	}

	// find cursor position
	x = drawLen - (ColorStrlen( text ) + 1 );
	if( x < 0 ) x = 0;
	cursor = ( this->cursor - prestep - x );
	if( cursor < 0 ) cursor = 0;

	if( justify == 0 ) x = m_iX;
	else if( justify == 1 )
		x = m_iX + ((m_iWidth - (ColorStrlen( text ) * m_iCharWidth )) / 2 );
	else if( justify == 2 )
		x = m_iX + (m_iWidth - (ColorStrlen( text ) * m_iCharWidth ));

	if( this->background )
	{
		UI_DrawPic( m_iX, y, m_iWidth, m_iHeight, uiColorWhite, this->background );
	}
	else
	{
		// draw the background
		UI_FillRect( m_iX, y, m_iWidth, m_iHeight, uiInputBgColor );

		// draw the rectangle
		UI_DrawRectangle( m_iX, y, m_iWidth, m_iHeight, uiInputFgColor );
	}

	textHeight = y - (m_iCharHeight * 1.5f);
	UI_DrawString( m_iX, textHeight, m_iWidth, m_iCharHeight, m_szName, uiColorHelp, true, m_iCharWidth, m_iCharHeight, 0, shadow );

	if( m_iFlags & QMF_GRAYED )
	{
		UI_DrawString( m_iX, y, m_iWidth, m_iHeight, text, uiColorDkGrey, true, m_iCharWidth, m_iCharHeight, justify, shadow );
		return; // grayed
	}

	if(this != m_pParent->ItemAtCursor())
	{
		UI_DrawString( m_iX, y, m_iWidth, m_iHeight, text, m_iColor, false, m_iCharWidth, m_iCharHeight, justify, shadow );
		return; // no focus
	}

	if( !( m_iFlags & QMF_FOCUSBEHIND ))
	{
		UI_DrawString( m_iX, y, m_iWidth, m_iHeight, text, m_iColor, false, m_iCharWidth, m_iCharHeight, justify, shadow );

		if(( uiStatic.realTime & 499 ) < 250 )
			UI_DrawString( x + (cursor * m_iCharWidth), y, m_iCharWidth, m_iHeight, cursor_char, m_iColor, true, m_iCharWidth, m_iCharHeight, 0, shadow );
	}

	if( m_iFlags & QMF_HIGHLIGHTIFFOCUS )
	{
		UI_DrawString( m_iX, y, m_iWidth, m_iHeight, text, m_iFocusColor, false, m_iCharWidth, m_iCharHeight, justify, shadow );

		if(( uiStatic.realTime & 499 ) < 250 )
			UI_DrawString( x + (cursor * m_iCharWidth), y, m_iCharWidth, m_iHeight, cursor_char, m_iFocusColor, true, m_iCharWidth, m_iCharHeight, 0, shadow );
	}
	else if( m_iFlags & QMF_PULSEIFFOCUS )
	{
		int	color;

		color = PackAlpha( m_iColor, 255 * (0.5 + 0.5 * sin( (float)uiStatic.realTime / UI_PULSE_DIVISOR )));
		UI_DrawString( m_iX, y, m_iWidth, m_iHeight, text, color, false, m_iCharWidth, m_iCharHeight, justify, shadow );

		if(( uiStatic.realTime & 499 ) < 250 )
			UI_DrawString( x + (cursor * m_iCharWidth), y, m_iCharWidth, m_iHeight, cursor_char, color, true, m_iCharWidth, m_iCharHeight, 0, shadow );
	}

	if( m_iFlags & QMF_FOCUSBEHIND )
	{
		UI_DrawString( m_iX, y, m_iWidth, m_iHeight, text, m_iColor, false, m_iCharWidth, m_iCharHeight, justify, shadow );

		if(( uiStatic.realTime & 499 ) < 250 )
			UI_DrawString( x + (cursor * m_iCharWidth), y, m_iCharWidth, m_iHeight, cursor_char, m_iColor, true, m_iCharWidth, m_iCharHeight, 0, shadow );
	}
}
