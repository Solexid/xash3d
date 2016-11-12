/*
gl_vid_android.c - nanogl video backend
Copyright (C) 2016 mittorn

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/



#include "common.h"
#if XASH_VIDEO == VIDEO_TIZEN
#include "client.h"
#include "gl_local.h"
#include "mod_local.h"
#include "input.h"
#include <system_info.h>
#include <Evas_GL.h>
#include <GL/nanogl.h>
#include "gl_vidnt.h"


typedef enum
{
	rserr_ok,
	rserr_invalid_fullscreen,
	rserr_invalid_mode,
	rserr_unknown
} rserr_t;

/*
=================
GL_GetProcAddress
=================
*/
void *GL_GetProcAddress( const char *name )
{
	void *func = nanoGL_GetProcAddress(name);

	if(!func)
	{
		MsgDev(D_ERROR, "Error: GL_GetProcAddress failed for %s", name);
	}
	return func;
}


void GL_InitExtensions( void )
{
	// initialize gl extensions
	GL_CheckExtension( "OpenGL 1.1.0", NULL, NULL, GL_OPENGL_110 );

	// get our various GL strings
	glConfig.vendor_string = pglGetString( GL_VENDOR );
	glConfig.renderer_string = pglGetString( GL_RENDERER );
	glConfig.version_string = pglGetString( GL_VERSION );
	glConfig.extensions_string = pglGetString( GL_EXTENSIONS );
	MsgDev( D_INFO, "Video: %s\n", glConfig.renderer_string );

	// initalize until base opengl functions loaded

	GL_SetExtension( GL_DRAW_RANGEELEMENTS_EXT, false );
	GL_SetExtension( GL_ARB_MULTITEXTURE, false );
	GL_SetExtension( GL_ENV_COMBINE_EXT, false );
	GL_SetExtension( GL_DOT3_ARB_EXT, false );
	GL_SetExtension( GL_TEXTURE_3D_EXT, false );
	GL_SetExtension( GL_SGIS_MIPMAPS_EXT, true ); // gles specs

	// hardware cubemaps
	GL_CheckExtension( "GL_OES_texture_cube_map", NULL, "gl_texture_cubemap", GL_TEXTURECUBEMAP_EXT );

	if( GL_Support( GL_TEXTURECUBEMAP_EXT ))
	{
		pglGetIntegerv( GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB, &glConfig.max_cubemap_size );
	}
	GL_SetExtension( GL_ARB_SEAMLESS_CUBEMAP, false );

	GL_SetExtension( GL_EXT_POINTPARAMETERS, false );
	GL_CheckExtension( "GL_OES_texture_npot", NULL, "gl_texture_npot", GL_ARB_TEXTURE_NPOT_EXT );

	GL_SetExtension( GL_TEXTURE_COMPRESSION_EXT, false );
	GL_SetExtension( GL_CUSTOM_VERTEX_ARRAY_EXT, false );
	GL_SetExtension( GL_CLAMPTOEDGE_EXT, true ); // by gles1 specs
	GL_SetExtension( GL_ANISOTROPY_EXT, false );
	GL_SetExtension( GL_TEXTURE_LODBIAS, false );
	GL_SetExtension( GL_CLAMP_TEXBORDER_EXT, false );
	GL_SetExtension( GL_BLEND_MINMAX_EXT, false );
	GL_SetExtension( GL_BLEND_SUBTRACT_EXT, false );
	GL_SetExtension( GL_SEPARATESTENCIL_EXT, false );
	GL_SetExtension( GL_STENCILTWOSIDE_EXT, false );
	GL_SetExtension( GL_ARB_VERTEX_BUFFER_OBJECT_EXT, false );
	GL_SetExtension( GL_TEXTURE_ENV_ADD_EXT,false  );
	GL_SetExtension( GL_SHADER_OBJECTS_EXT, false );
	GL_SetExtension( GL_SHADER_GLSL100_EXT, false );
	GL_SetExtension( GL_VERTEX_SHADER_EXT,false );
	GL_SetExtension( GL_FRAGMENT_SHADER_EXT, false );
	GL_SetExtension( GL_SHADOW_EXT, false );
	GL_SetExtension( GL_ARB_DEPTH_FLOAT_EXT, false );
	GL_SetExtension( GL_OCCLUSION_QUERIES_EXT,false );
	GL_CheckExtension( "GL_OES_depth_texture", NULL, "gl_depthtexture", GL_DEPTH_TEXTURE );

	glConfig.texRectangle = glConfig.max_2d_rectangle_size = 0; // no rectangle

	glConfig.max_2d_texture_size = 0;
	pglGetIntegerv( GL_MAX_TEXTURE_SIZE, &glConfig.max_2d_texture_size );
	if( glConfig.max_2d_texture_size <= 0 ) glConfig.max_2d_texture_size = 256;

	Cvar_Get( "gl_max_texture_size", "0", CVAR_INIT, "opengl texture max dims" );
	Cvar_Set( "gl_max_texture_size", va( "%i", glConfig.max_2d_texture_size ));

	pglGetIntegerv( GL_MAX_VERTEX_UNIFORM_COMPONENTS_ARB, &glConfig.max_vertex_uniforms );
	pglGetIntegerv( GL_MAX_VERTEX_ATTRIBS_ARB, &glConfig.max_vertex_attribs );

	// MCD has buffering issues
	if(Q_strstr( glConfig.renderer_string, "gdi" ))
		Cvar_SetFloat( "gl_finish", 1 );

	Cvar_Set( "gl_anisotropy", va( "%f", bound( 0, gl_texture_anisotropy->value, glConfig.max_texture_anisotropy )));

	// software mipmap generator does wrong result with NPOT textures ...
	if( !GL_Support( GL_SGIS_MIPMAPS_EXT ))
		GL_SetExtension( GL_ARB_TEXTURE_NPOT_EXT, false );

	if( GL_Support( GL_TEXTURE_COMPRESSION_EXT ))
		Image_AddCmdFlags( IL_DDS_HARDWARE );

	glw_state.initialized = true;

	tr.framecount = tr.visframecount = 1;
}


/*
=================
GL_CheckExtension
=================
*/
void GL_CheckExtension( const char *name, const dllfunc_t *funcs, const char *cvarname, int r_ext )
{
	const dllfunc_t	*func;
	convar_t		*parm;

	MsgDev( D_NOTE, "GL_CheckExtension: %s ", name );

	if( cvarname )
	{
		// system config disable extensions
		parm = Cvar_Get( cvarname, "1", CVAR_GLCONFIG, va( "enable or disable %s", name ));

		if( parm->integer == 0 || ( gl_extensions->integer == 0 && r_ext != GL_OPENGL_110 ))
		{
			MsgDev( D_NOTE, "- disabled\n" );
			GL_SetExtension( r_ext, 0 );
			return; // nothing to process at
		}
		GL_SetExtension( r_ext, 1 );
	}

	if(( name[2] == '_' || name[3] == '_' ) && !Q_strstr( glConfig.extensions_string, name ))
	{
		GL_SetExtension( r_ext, false );	// update render info
		MsgDev( D_NOTE, "- ^1failed\n" );
		return;
	}


	GL_SetExtension( r_ext, true ); // predict extension state

	if( GL_Support( r_ext ))
		MsgDev( D_NOTE, "- ^2enabled\n" );
	else MsgDev( D_NOTE, "- ^1failed\n" );
}
/*
===============
GL_UpdateGammaRamp
===============
*/
void GL_UpdateGammaRamp( void )
{
}

/*
===============
GL_UpdateSwapInterval
===============
*/
void GL_UpdateSwapInterval( void )
{
	if( gl_swapInterval->modified )
	{
		gl_swapInterval->modified = false;
		// EVAS TODO
	}
}

/*
===============
GL_ContextError
===============
*/
static void GL_ContextError( void )
{
}

/*
==================
GL_SetupAttributes
==================
*/
void GL_SetupAttributes( ) { }

/*
=================
GL_CreateContext
=================
*/
qboolean GL_CreateContext( void )
{
	nanoGL_Init();

	return true;
}

/*
=================
GL_UpdateContext
=================
*/
qboolean GL_UpdateContext( void )
{

	return true;
}

/*
=================
GL_DeleteContext
=================
*/
qboolean GL_DeleteContext( void )
{

	return false;
}

uint VID_EnumerateInstances( void )
{
	return 1;
}

void VID_StartupGamma( void )
{
	// Device supports gamma anyway, but cannot do anything with it.
	fs_offset_t	gamma_size;
	byte	*savedGamma;
	size_t	gammaTypeSize = sizeof(glState.stateRamp);

	// init gamma ramp
	Q_memset( glState.stateRamp, 0, gammaTypeSize);

	// force to set cvar
	Cvar_FullSet( "gl_ignorehwgamma", "1", CVAR_GLCONFIG );

	glConfig.deviceSupportsGamma = false;	// even if supported!
	BuildGammaTable( vid_gamma->value, vid_texgamma->value );
	MsgDev( D_NOTE, "VID_StartupGamma: software gamma initialized\n" );
}

void VID_RestoreGamma( void )
{
	// no hardware gamma
}

qboolean VID_CreateWindow( int width, int height, qboolean fullscreen )
{
	return true;
}

void VID_DestroyWindow( void )
{
	if( glState.fullScreen )
	{
		glState.fullScreen = false;
	}
}

/*
==================
R_ChangeDisplaySettingsFast

Change window size fastly to custom values, without setting vid mode
==================
*/
void R_ChangeDisplaySettingsFast( int width, int height )
{
	//Cvar_SetFloat("vid_mode", VID_NOMODE);
	Cvar_SetFloat("width", width);
	Cvar_SetFloat("height", height);

	glState.width = width;
	glState.height = height;

	glState.wideScreen = true; // V_AdjustFov will check for widescreen

	SCR_VidInit();
}


rserr_t R_ChangeDisplaySettings( int width, int height, qboolean fullscreen )
{
#ifdef XASH_SDL
	SDL_DisplayMode displayMode;

	SDL_GetCurrentDisplayMode(0, &displayMode);
#ifdef __ANDROID__
	width = displayMode.w;
	height = displayMode.h;
	fullscreen = false;
#endif
	R_SaveVideoMode( width, height );

	// check our desktop attributes
	glw_state.desktopBitsPixel = SDL_BITSPERPIXEL(displayMode.format);
	glw_state.desktopWidth = displayMode.w;
	glw_state.desktopHeight = displayMode.h;

	glState.fullScreen = fullscreen;
	glState.wideScreen = true; // V_AdjustFov will check for widescreen

	if(!host.hWnd)
	{
		if( !VID_CreateWindow( width, height, fullscreen ) )
			return rserr_invalid_mode;
	}
#ifndef __ANDROID__
else if( fullscreen )
	{
		SDL_DisplayMode want, got;

		want.w = width;
		want.h = height;
		want.driverdata = NULL;
		want.format = want.refresh_rate = 0; // don't care

		if( !SDL_GetClosestDisplayMode(0, &want, &got) )
			return rserr_invalid_mode;

		MsgDev(D_NOTE, "Got closest display mode: %ix%i@%i\n", got.w, got.h, got.refresh_rate);

		if( ( SDL_GetWindowFlags(host.hWnd) & SDL_WINDOW_FULLSCREEN ) == SDL_WINDOW_FULLSCREEN)
			if( SDL_SetWindowFullscreen(host.hWnd, 0) == -1 )
				return rserr_invalid_fullscreen;

		if( SDL_SetWindowDisplayMode(host.hWnd, &got) )
			return rserr_invalid_mode;

		if( SDL_SetWindowFullscreen(host.hWnd, SDL_WINDOW_FULLSCREEN) == -1 )
			return rserr_invalid_fullscreen;

		R_ChangeDisplaySettingsFast( got.w, got.h );
	}
	else
	{
		if( SDL_SetWindowFullscreen(host.hWnd, 0) )
			return rserr_invalid_fullscreen;
		SDL_SetWindowSize(host.hWnd, width, height);
		R_ChangeDisplaySettingsFast( width, height );
	}
#endif
#endif // XASH_SDL
	return rserr_ok;
}



/*
==================
VID_SetMode

Set the described video mode
==================
*/
qboolean VID_SetMode( void )
{

	return true;
}

#ifndef EGL_LIB
#define EGL_LIB NULL
#endif

/*
==================
R_Init_OpenGL
==================
*/
qboolean R_Init_OpenGL( void )
{
	return VID_SetMode();
}


/*
==================
R_Free_OpenGL
==================
*/
void R_Free_OpenGL( void )
{
	VID_RestoreGamma ();

	GL_DeleteContext ();

	VID_DestroyWindow ();
	// now all extensions are disabled
	Q_memset( glConfig.extension, 0, sizeof( glConfig.extension[0] ) * GL_EXTCOUNT );
	glw_state.initialized = false;
}
#endif // XASH_VIDEO
