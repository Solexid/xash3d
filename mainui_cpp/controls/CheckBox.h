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

#pragma once
#ifndef MENU_CHECKBOX_H
#define MENU_CHECKBOX_H

#define UI_CHECKBOX_EMPTY		"gfx/shell/cb_empty"
#define UI_CHECKBOX_GRAYED		"gfx/shell/cb_disabled"
#define UI_CHECKBOX_FOCUS		"gfx/shell/cb_over"
#define UI_CHECKBOX_PRESSED		"gfx/shell/cb_down"
#define UI_CHECKBOX_ENABLED		"gfx/shell/cb_checked"

class CMenuCheckBox : public CMenuBaseItem
{
public:
	virtual void VidInit( void );
	virtual const char * Key( int key, int down );
	virtual void Draw( void );

	int		enabled;
	const char	*emptyPic;
	const char	*focusPic;	// can be replaced with pressPic manually
	const char	*checkPic;
	const char	*grayedPic;	// when QMF_GRAYED is set

};

#endif // MENU_CHECKBOX_H
