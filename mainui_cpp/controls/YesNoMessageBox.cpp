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

#include "extdll.h"
#include "basemenu.h"
#include "Action.h"
#include "PicButton.h"
#include "YesNoMessageBox.h"
#include "utils.h"

static void ToggleVisibilityCb( CMenuBaseItem *pSelf, void *pExtra );

/*
==============
CMenuYesNoMessageBox::Init
==============
*/
void CMenuYesNoMessageBox::Init( void )
{
	dlgMessage1.Flags() = QMF_INACTIVE|QMF_DROPSHADOW|QMF_HIDDEN|QMF_CENTER_JUSTIFY;
	yes.Flags() = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_HIDDEN;
	no.Flags() = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_HIDDEN;
	Flags() = QMF_INACTIVE|QMF_HIDDEN;

	yes.EventCallbackExtra() = no.EventCallbackExtra() = this;
	yes.EnableTransitions( false );
	no.EnableTransitions( false );

	SET_EVENT( yes, onActivated )
	{
		CMenuYesNoMessageBox *msgBox = (CMenuYesNoMessageBox*)pExtra;
		msgBox->onPositive( msgBox, msgBox->positiveExtra );
	}
	END_EVENT( yes, onActivated )

	SET_EVENT( no, onActivated )
	{
		CMenuYesNoMessageBox *msgBox = (CMenuYesNoMessageBox*)pExtra;
		msgBox->onNegative( msgBox, msgBox->negativeExtra );
	}
	END_EVENT( no, onActivated )

	if( !onNegative )
		onNegative = ToggleVisibilityCb;

	if( !onPositive )
		onPositive = ToggleVisibilityCb;

	m_pParent->AddItem( dlgMessage1 );
	m_pParent->AddItem( yes );
	m_pParent->AddItem( no );
}

/*
==============
CMenuYesNoMessageBox::VidInit
==============
*/
void CMenuYesNoMessageBox::VidInit( void )
{
	dlgMessage1.SetRect( DLG_X + 192, 280, 640, 256 );
	yes.SetRect( DLG_X + 380, 460, UI_BUTTONS_WIDTH / 2, UI_BUTTONS_HEIGHT );
	no.SetRect( DLG_X + 530, 460, UI_BUTTONS_WIDTH / 2, UI_BUTTONS_HEIGHT );
	SetRect( DLG_X + 192, 256, 640, 256 );

	CMenuAction::VidInit(); // setup rect properly
}

/*
==============
CMenuYesNoMessageBox::Draw
==============
*/
void CMenuYesNoMessageBox::Draw( void )
{
	UI_FillRect( m_iX, m_iY, m_iWidth, m_iHeight, uiPromptBgColor );
}

/*
==============
CMenuYesNoMessageBox::SetMessage
==============
*/
void CMenuYesNoMessageBox::SetMessage( const char *msg )
{
	dlgMessage1.SetName( msg );
}

/*
==============
CMenuYesNoMessageBox::SetPositiveButton
==============
*/
void CMenuYesNoMessageBox::SetPositiveButton( const char *msg, int buttonPic, void *extra )
{
	yes.SetName( msg );
	yes.SetPicture( buttonPic );
	positiveExtra = extra;
}

/*
==============
CMenuYesNoMessageBox::SetNegativeButton
==============
*/
void CMenuYesNoMessageBox::SetNegativeButton(const char *msg, int buttonPic, void *extra)
{
	no.SetName( msg );
	no.SetPicture( buttonPic );
}

/*
==============
CMenuYesNoMessageBox::ToggleVisibility
==============
*/
void CMenuYesNoMessageBox::ToggleVisibility( void )
{
	yes.Flags() ^= QMF_HIDDEN;
	no.Flags() ^= QMF_HIDDEN;
	dlgMessage1.Flags() ^= QMF_HIDDEN;
	m_iFlags ^= QMF_HIDDEN;
}

/*
==============
CMenuYesNoMessageBox::ToggleVisibilityCb
==============
*/
static void ToggleVisibilityCb( CMenuBaseItem *pSelf, void *pExtra )
{
	CMenuYesNoMessageBox *msgBox = (CMenuYesNoMessageBox*)pSelf;
	msgBox->ToggleVisibility();
}
