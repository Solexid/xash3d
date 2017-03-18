/*
YesNoMessageBox.h - simple generic message box
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
#ifndef MENU_GENERICMSGBOX_H
#define MENU_GENERICMSGBOX_H

#include "PicButton.h"
#include "Action.h"

class CMenuYesNoMessageBox : public CMenuAction
{
public:
	virtual void Init();
	virtual void VidInit();
	virtual void Draw();
	void SetMessage( const char *msg );
	void SetPositiveButton( const char *msg, int buttonPic, void *extra = NULL );
	void SetNegativeButton( const char *msg, int buttonPic, void *extra = NULL );
	void ToggleVisibility();

	EventCallback onPositive;
	EventCallback onNegative;
private:
	void *positiveExtra, *negativeExtra;
	CMenuAction		dlgMessage1;
	CMenuPicButton	yes;
	CMenuPicButton	no;
};

#endif // MENU_GENERICMSGBOX_H
