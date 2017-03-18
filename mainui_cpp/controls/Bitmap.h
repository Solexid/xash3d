/*
Bitmap.h - bitmap menu item
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
#ifndef MENU_BITMAP_H
#define MENU_BITMAP_H

#define ART_BACKGROUND		"gfx/shell/splash"

class CMenuBitmap : public CMenuBaseItem
{
public:
	virtual void VidInit( void );
	virtual const char * Key( int key, int down );
	virtual void Draw( void );
	void SetPicture( const char *pic, const char *focusPic = NULL, const char *pressPic = NULL)
	{
		m_szPic = pic;
		m_szFocusPic = focusPic;
		m_szPressPic = pressPic;
	}

protected:
	const char	*m_szPic;
	const char	*m_szFocusPic;
	const char	*m_szPressPic;
};

// menu banners used fiexed rectangle (virtual screenspace at 640x480)
#define UI_BANNER_POSX		72
#define UI_BANNER_POSY		72
#define UI_BANNER_WIDTH		736
#define UI_BANNER_HEIGHT		128

class CMenuBannerBitmap : public CMenuBitmap
{
public:
	CMenuBannerBitmap() : m_bOverrideRect( false )
	{
		Flags() = QMF_INACTIVE|QMF_DRAW_ADDITIVE;
	}
	virtual void Draw( void );
	virtual void VidInit( void );

	void OverrideRect( bool enable ) { m_bOverrideRect = enable; }
private:
	bool m_bOverrideRect;
	bool m_bTransitionSet;
};

class CMenuBackgroundBitmap: public CMenuBitmap
{
public:
	CMenuBackgroundBitmap() : CMenuBitmap()
	{
		SetPicture( ART_BACKGROUND );
		Flags() = QMF_INACTIVE;
	}
	virtual void Draw( void );
};

#endif // MENU_BITMAP_H
