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

#pragma once
#ifndef MENU_SCROLLLIST_H
#define MENU_SCROLLLIST_H

class CMenuScrollList : public CMenuBaseItem
{
public:
	virtual void VidInit( void );
	virtual const char *Key( int key, int down );
	virtual void Draw( void );

	const char	*background;
	const char	*upArrow;
	const char	*upArrowFocus;
	const char	*downArrow;
	const char	*downArrowFocus;
	const char	**itemNames;
	int		numItems;
	int		curItem;
	int		topItem;
	int		numRows;
// scrollbar stuff // ADAMIX
	int		scrollBarX;
	int		scrollBarY;
	int		scrollBarWidth;
	int		scrollBarHeight;
	int		scrollBarSliding;
// highlight // mittorn
	int		highlight;
};

#endif // MENU_SCROLLLIST_H
