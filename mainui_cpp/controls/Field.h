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

#pragma once
#ifndef MENU_FIELD_H
#define MENU_FIELD_H

#define UI_MAX_FIELD_LINE		256

class CMenuField : public CMenuBaseItem
{
public:
	virtual void VidInit( void );
	virtual const char * Key( int key, int down );
	virtual void Draw( void );
	virtual void Char( int key );

	const char	*background;
	int		maxLength;		// can't be more than UI_MAX_FIELD_LINE
	char		buffer[UI_MAX_FIELD_LINE];
	int		widthInChars;
	int		cursor;
	int		scroll;
	void Paste();
	void Clear();
protected:
	virtual void _Event( int ev );
};

#endif // MENU_FIELD_H
