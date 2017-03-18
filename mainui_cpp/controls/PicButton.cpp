/*
PicButton.h - animated button with picture
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
#include "Bitmap.h"
#include "PicButton.h"
#include "utils.h"
/*
=================
CMenuPicButton::Init
=================
*/
void CMenuPicButton::VidInit( void )
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

	if(!( m_iFlags & ( QMF_LEFT_JUSTIFY|QMF_CENTER_JUSTIFY|QMF_RIGHT_JUSTIFY )))
		m_iFlags |= QMF_LEFT_JUSTIFY;

	if( !m_iColor ) m_iColor = uiPromptTextColor;
	if( !m_iFocusColor ) m_iFocusColor = uiPromptFocusColor;

	if( m_iWidth < 1 || m_iHeight < 1 )
	{
		if( m_iWidth < 1 )
			m_iWidth = m_iCharWidth * strlen( m_szName );

		if( m_iHeight < 1 )
			m_iHeight = m_iCharHeight * 1.5;
	}

	UI_ScaleCoords( &m_iX, &m_iY, &m_iWidth, &m_iHeight );
	UI_ScaleCoords( NULL, NULL, &m_iCharWidth, &m_iCharHeight );
}

/*
=================
CMenuPicButton::Key
=================
*/
const char *CMenuPicButton::Key( int key, int down )
{
	const char	*sound = 0;

	switch( key )
	{
	case K_MOUSE1:
		if(!( m_iFlags & QMF_HASMOUSEFOCUS ))
			break;
		sound = uiSoundLaunch;
		break;
	case K_ENTER:
	case K_KP_ENTER:
	case K_AUX1:
		if( m_iFlags & QMF_MOUSEONLY )
			break;
		sound = uiSoundLaunch;
	}
	if( sound && ( m_iFlags & QMF_SILENT ))
		sound = uiSoundNull;

	if( sound )
	{
		if( m_iFlags & QMF_ACT_ONRELEASE )
		{
			int event;
			if( down )
			{
				event = QM_PRESSED;
				m_bPressed = true;
			}
			else
				event = QM_ACTIVATED;

			TACheckMenuDepth();
			_Event( event );
			SetTitleAnim( AS_TO_TITLE );
		}
		else if( down )
		{
			_Event( QM_ACTIVATED );
		}
	}

	return sound;
}

/*
=================
CMenuPicButton::Draw
=================
*/
void CMenuPicButton::Draw( )
{
	int state = BUTTON_NOFOCUS;

	if( m_iFlags & (QMF_HASMOUSEFOCUS|QMF_HASKEYBOARDFOCUS))
		state = BUTTON_FOCUS;

	// make sure what cursor in rect
	if( m_bPressed && cursorDown )
		state = BUTTON_PRESSED;
	else m_bPressed = false;

	if( m_szStatusText && m_iFlags & QMF_NOTIFY )
	{
		int	charW, charH;
		int	x, w;

		charW = UI_SMALL_CHAR_WIDTH;
		charH = UI_SMALL_CHAR_HEIGHT;

		UI_ScaleCoords( NULL, NULL, &charW, &charH );

		x = 290;
		w = UI_SMALL_CHAR_WIDTH * strlen( m_szStatusText );
		UI_ScaleCoords( &x, NULL, &w, NULL );
		x += m_iX;

		int	r, g, b;

		UnpackRGB( r, g, b, uiColorHelp );
		EngFuncs::DrawSetTextColor( r, g, b );
		EngFuncs::DrawConsoleString( x, m_iY, m_szStatusText );
	}

	if( this->pic )
	{
		int r, g, b, a;

		UnpackRGB( r, g, b, m_iFlags & QMF_GRAYED ? uiColorDkGrey : uiColorWhite );

		wrect_t rects[]=
		{
		{ 0, uiStatic.buttons_width, 0, 26 },
		{ 0, uiStatic.buttons_width, 26, 52 },
		{ 0, uiStatic.buttons_width, 52, 78 }
		};

		EngFuncs::PIC_Set( this->pic, r, g, b, 255 );
		EngFuncs::PIC_EnableScissor( m_iX, m_iY, uiStatic.buttons_draw_width, uiStatic.buttons_draw_height - 2 );
		EngFuncs::PIC_DrawAdditive( m_iX, m_iY, uiStatic.buttons_draw_width, uiStatic.buttons_draw_height, &rects[state] );

		a = (512 - (uiStatic.realTime - this->m_iLastFocusTime)) >> 1;

		if( state == BUTTON_NOFOCUS && a > 0 )
		{
			EngFuncs::PIC_Set( this->pic, r, g, b, a );
			EngFuncs::PIC_DrawAdditive( m_iX, m_iY, uiStatic.buttons_draw_width, uiStatic.buttons_draw_height, &rects[BUTTON_FOCUS] );
		}
		EngFuncs::PIC_DisableScissor();
	}
	else
	{
		int	justify = 0;
		int	shadow;

		if( m_iFlags & QMF_LEFT_JUSTIFY )
			justify = 0;
		else if( m_iFlags & QMF_CENTER_JUSTIFY )
			justify = 1;
		else if( m_iFlags & QMF_RIGHT_JUSTIFY )
			justify = 2;

		shadow = (m_iFlags & QMF_DROPSHADOW);

		if( m_iFlags & QMF_GRAYED )
		{
			UI_DrawString( m_iX, m_iY, m_iWidth, m_iHeight, m_szName, uiColorDkGrey, true, m_iCharWidth, m_iCharHeight, justify, shadow );
			return; // grayed
		}

		if(this != m_pParent->ItemAtCursor())
		{
			UI_DrawString( m_iX, m_iY, m_iWidth, m_iHeight, m_szName, m_iColor, false, m_iCharWidth, m_iCharHeight, justify, shadow );
			return; // no focus
		}

		if(!( m_iFlags & QMF_FOCUSBEHIND ))
			UI_DrawString( m_iX, m_iY, m_iWidth, m_iHeight, m_szName, m_iColor, false, m_iCharWidth, m_iCharHeight, justify, shadow );

		if( m_iFlags & QMF_HIGHLIGHTIFFOCUS )
			UI_DrawString( m_iX, m_iY, m_iWidth, m_iHeight, m_szName, m_iFocusColor, false, m_iCharWidth, m_iCharHeight, justify, shadow );
		else if( m_iFlags & QMF_PULSEIFFOCUS )
		{
			int	color;

			color = PackAlpha( m_iColor, 255 * (0.5 + 0.5 * sin( (float)uiStatic.realTime / UI_PULSE_DIVISOR )));

			UI_DrawString( m_iX, m_iY, m_iWidth, m_iHeight, m_szName, color, false, m_iCharWidth, m_iCharHeight, justify, shadow );
		}

		if( m_iFlags & QMF_FOCUSBEHIND )
			UI_DrawString( m_iX, m_iY, m_iWidth, m_iHeight, m_szName, m_iColor, false, m_iCharWidth, m_iCharHeight, justify, shadow );
	}
}

void CMenuPicButton::SetPicture( int ID )
{
	if( ID < 0 || ID > PC_BUTTONCOUNT )
		return; // bad id

#if 0	// too different results on various games. disabled
	width = PicButtonWidth( ID ) * UI_BUTTON_CHARWIDTH;
#else
	m_iWidth = UI_BUTTONS_WIDTH;
#endif
	m_iHeight = UI_BUTTONS_HEIGHT;

	pic = uiStatic.buttonsPics[ID];
	button_id = ID;

	if( pic ) // text buttons not use it
		m_iFlags |= QMF_ACT_ONRELEASE;
}

// ============================ Animations ===========================
#define BANNER_X_FIX	-16
#define BANNER_Y_FIX	-20

// Title Transition Time period
#define TTT_PERIOD		200.0f

CMenuPicButton *ButtonStack[UI_MAX_MENUDEPTH];
int		ButtonStackDepth;

CMenuPicButton::Quad	CMenuPicButton::TitleLerpQuads[2];
int		CMenuPicButton::transition_state = CMenuPicButton::AS_TO_TITLE;
HIMAGE	CMenuPicButton::TransPic = 0;
bool	CMenuPicButton::hold_button_stack = false;
int		CMenuPicButton::transition_initial_time;


void CMenuPicButton::TACheckMenuDepth( void )
{
	PreClickDepth = uiStatic.menuDepth;
}


void CMenuPicButton::PopPButtonStack()
{
	if( hold_button_stack ) return;

	if( ButtonStack[ButtonStackDepth] )
		ButtonStack[ButtonStackDepth]->SetTitleAnim( AS_TO_BUTTON );

	if( ButtonStackDepth )
		ButtonStackDepth--;
}

void CMenuPicButton::PushPButtonStack()
{
	if( ButtonStack[ButtonStackDepth] == this )
		return;

	ButtonStack[++ButtonStackDepth] = this;
}

float CMenuPicButton::GetTitleTransFraction( void )
{
	float fraction = (float)(uiStatic.realTime - transition_initial_time ) / TTT_PERIOD;

	if( fraction > 1.0f )
		fraction = 1.0f;

	return fraction;
}


CMenuPicButton::Quad CMenuPicButton::LerpQuad( Quad a, Quad b, float frac )
{
	Quad c;
	c.x = a.x + (b.x - a.x) * frac;
	c.y = a.y + (b.y - a.y) * frac;
	c.lx = a.lx + (b.lx - a.lx) * frac;
	c.ly = a.ly + (b.ly - a.ly) * frac;

	return c;
}

// TODO: Find CMenuBannerBitmap in next menu page and correct
void CMenuPicButton::SetupTitleQuad()
{
	TitleLerpQuads[1].x  = UI_BANNER_POSX * ScreenHeight / 768;
	TitleLerpQuads[1].y  = UI_BANNER_POSY * ScreenHeight / 768;
	TitleLerpQuads[1].lx = UI_BANNER_WIDTH * ScreenHeight / 768;
	TitleLerpQuads[1].ly = UI_BANNER_HEIGHT * ScreenHeight / 768;
}

void CMenuPicButton::DrawTitleAnim()
{
	SetupTitleQuad();

	if( !TransPic ) return;

	wrect_t r = { 0, uiStatic.buttons_width, 26, 51 };

	float frac = GetTitleTransFraction();/*(sin(gpGlobals->time*4)+1)/2*/;

#ifdef TA_ALT_MODE
	if( frac == 1 && transition_state == AS_TO_BUTTON )
		return;
#else
	if( frac == 1 )
		return;
#endif

	Quad c;

	int f_idx = (transition_state == AS_TO_TITLE) ? 0 : 1;
	int s_idx = (transition_state == AS_TO_TITLE) ? 1 : 0;

	c = LerpQuad( TitleLerpQuads[f_idx], TitleLerpQuads[s_idx], frac );

	EngFuncs::PIC_Set( TransPic, 255, 255, 255 );
	EngFuncs::PIC_DrawAdditive( c.x, c.y, c.lx, c.ly, &r );
}

void CMenuPicButton::SetTitleAnim( int anim_state )
{
	CMenuPicButton *button = this;

	// skip buttons which don't call new menu
	if( PreClickDepth == uiStatic.menuDepth && anim_state == AS_TO_TITLE )
		return;

	// replace cancel\done button with button which called this menu
	if( PreClickDepth > uiStatic.menuDepth && anim_state == AS_TO_TITLE )
	{
		anim_state = AS_TO_BUTTON;

		// HACK HACK HACK
		if( ButtonStack[ButtonStackDepth + 1] )
			button = ButtonStack[ButtonStackDepth+1];
	}

	// don't reset anim if dialog buttons pressed
	//if( button->id == ID_YES || button->id == ID_NO )
	//	return;

	if( !button->enableTrans )
		return;

	if( anim_state == AS_TO_TITLE )
		PushPButtonStack();

	transition_state = anim_state;

	TitleLerpQuads[0].x = XPos();
	TitleLerpQuads[0].y = YPos();
	TitleLerpQuads[0].lx = Width();
	TitleLerpQuads[0].ly = Height();

	transition_initial_time = uiStatic.realTime;
	TransPic = pic;
}

void CMenuPicButton::InitTitleAnim()
{
	memset( TitleLerpQuads, 0, sizeof( CMenuPicButton::Quad ) * 2 );

	SetupTitleQuad();

	ButtonStackDepth = 0;
	memset( ButtonStack, 0, sizeof( ButtonStack ));
}

void CMenuPicButton::ClearButtonStack( void )
{
	ButtonStackDepth = 0;
	memset( ButtonStack, 0, sizeof( ButtonStack ));
}
