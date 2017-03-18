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

#pragma once
#ifndef MENU_SLIDER_H
#define MENU_SLIDER_H
#define UI_SLIDER_MAIN		"gfx/shell/slider"

class CMenuSlider : public CMenuBaseItem
{
public:
	virtual void VidInit( void );
	virtual const char * Key( int key, int down );
	virtual void Draw( void );

	float		minValue;
	float		maxValue;
	float		curValue;
	float		drawStep;
	int		numSteps;
	float		range;
	int		keepSlider;	// when mouse button is holds
};

#endif // MENU_SLIDER_H
