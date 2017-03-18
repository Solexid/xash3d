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

#pragma once
#ifndef MENU_SPINCONTROL_H
#define MENU_SPINCONTROL_H

class CMenuSpinControl : public CMenuBaseItem
{
public:
	virtual void VidInit( void );
	virtual const char * Key( int key, int down );
	virtual void Draw( void );

	const char	*background;
	const char	*leftArrow;
	const char	*rightArrow;
	const char	*leftArrowFocus;
	const char	*rightArrowFocus;
	float		minValue;
	float		maxValue;
	float		curValue;
	float		range;
};

#endif // MENU_SPINCONTROL_H
