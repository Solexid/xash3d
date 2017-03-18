/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "extdll.h"
#include "basemenu.h"
#include "Bitmap.h"
#include "PicButton.h"
#include "YesNoMessageBox.h"
#include "utils.h"
#include "keydefs.h"
#include "menu_btnsbmp_table.h"

#define ART_BANNER	     	"gfx/shell/head_config"

class CMenuOptions: public CMenuFramework
{
public:
	virtual const char *Key( int key, int down );

private:
	virtual void _Init( void );
	virtual void _VidInit( void );
	static void CheckUpdatesDialog( CMenuBaseItem *pSelf, void *pExtra );

	CMenuBackgroundBitmap	background;
	CMenuBannerBitmap	banner;

	CMenuPicButton	controls;
	CMenuPicButton	audio;
	CMenuPicButton	video;
	CMenuPicButton	touch;
	CMenuPicButton	update;
	CMenuPicButton  gamepad;
	CMenuPicButton	done;

	// update dialog
	CMenuYesNoMessageBox msgBox;

	const char *m_szUpdateUrl;
};

static CMenuOptions	uiOptions;


void CMenuOptions::CheckUpdatesDialog(CMenuBaseItem *pSelf , void *pExtra)
{
	CMenuOptions *parent = (CMenuOptions*)pSelf->Parent();

	// toggle configuration menu between active\inactive
	// show\hide CheckUpdates dialog
	parent->controls.Flags() ^= QMF_INACTIVE;
	parent->audio.Flags() ^= QMF_INACTIVE;
	parent->video.Flags() ^= QMF_INACTIVE;
	parent->touch.Flags() ^= QMF_INACTIVE;
	parent->gamepad.Flags() ^= QMF_INACTIVE;
	parent->update.Flags() ^= QMF_INACTIVE;
	parent->done.Flags() ^= QMF_INACTIVE;

	parent->msgBox.ToggleVisibility( );
}

/*
=================
CMenuOptions::KeyFunc
=================
*/
const char *CMenuOptions::Key( int key, int down )
{
	/*if( down && key == K_ESCAPE && done.Flags() & QMF_INACTIVE )
	{
		CheckUpdatesDialog( &done, NULL );	// cancel 'check updates' dialog
		return uiSoundNull;
	}*/
	return CMenuFramework::Key( key, down );
}


/*
=================
CMenuOptions::Init
=================
*/
void CMenuOptions::_Init( void )
{
	// memset( &uiOptions, 0, sizeof( CMenuOptions ));

	banner.SetPicture( ART_BANNER );

	controls.SetNameAndStatus( "Controls", "Change keyboard and mouse settings" );
	controls.SetPicture( PC_CONTROLS );
	// SET_EVENT_VOID( controls, onActivated, UI_Controls_Menu );

	audio.SetNameAndStatus( "Audio", "Change sound volume and quality" );
	audio.SetPicture( PC_AUDIO );
	// SET_EVENT_VOID( audio, onActivated, UI_Audio_Menu );

	video.SetNameAndStatus( "Video", "Change screen size, video mode and gamma" );
	video.SetPicture( PC_VIDEO );
	// SET_EVENT_VOID( video, onActivated, UI_Video_Menu );

	touch.SetNameAndStatus( "Touch", "Change touch settings and buttons" );
	touch.SetPicture( "gfx/shell/btn_touch" );
	// SET_EVENT_VOID( touch, onActivated, UI_Touch_Menu );

	gamepad.SetNameAndStatus( "Gamepad", "Change gamepad axis and button settings" );
	gamepad.SetPicture( "gfx/shell/btn_gamepad" );
	// SET_EVENT_VOID( gamepad, onActivated, UI_Gamepad_Menu );

	update.SetNameAndStatus( "Update", "Download the latest version of Xash3D engine" );
	update.SetPicture( PC_UPDATE );
	update.onActivated = CheckUpdatesDialog;

	done.SetNameAndStatus( "Done", "Go back to the Main menu" );
	done.SetPicture( PC_DONE );
	done.onActivated = PopMenuCb;

	msgBox.SetMessage( "Check the Internet for updates?" );
	msgBox.SetPositiveButton( "Ok", PC_OK );
	msgBox.SetNegativeButton( "Cancel", PC_CANCEL );
	SET_EVENT( msgBox, onPositive )
	{
		EngFuncs::ShellExecute( ((CMenuOptions*)pSelf->Parent())->m_szUpdateUrl, NULL, TRUE );
	}
	END_EVENT( update, onPositive )

	if( gMenu.m_gameinfo.update_url[0] != 0 )
		m_szUpdateUrl = gMenu.m_gameinfo.update_url;
	else
		m_szUpdateUrl = "https://github.com/FWGS/xash3d/releases/latest";

	AddItem( background );
	AddItem( banner );
	AddItem( controls );
	AddItem( audio );
	AddItem( video );
	AddItem( touch );
	AddItem( gamepad );
	AddItem( update );
	AddItem( done );
	AddItem( msgBox );

}

void CMenuOptions::_VidInit( void )
{
	controls.SetCoord( 72, 230 );
	audio.SetCoord( 72, 280 );
	video.SetCoord( 72, 330 );
	touch.SetCoord( 72, 380 );
	gamepad.SetCoord( 72, 430 );
	update.SetCoord( 72, 480 );
	done.SetCoord( 72, 530 );
}

/*
=================
CMenuOptions::Precache
=================
*/
void UI_Options_Precache( void )
{
	EngFuncs::PIC_Load( ART_BACKGROUND );
	EngFuncs::PIC_Load( ART_BANNER );
}

/*
=================
CMenuOptions::Menu
=================
*/
void UI_Options_Menu( void )
{
	UI_Options_Precache();

	uiOptions.Init();
	uiOptions.VidInit();

	uiOptions.PushMenu();
}
