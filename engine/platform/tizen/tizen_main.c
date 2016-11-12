/*
tizen_main.c -- Tizen EFL backend
Copyright (C) 2016 a1batross

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#include "nanogl.h" //use NanoGL
#include <pthread.h>
#include "common.h"
#include "input.h"
#include "joyinput.h"
#include "touch.h"
#include "client.h"
#include "tizen_main.h"

static const int s_android_scantokey[] =
{
	0,				K_LEFTARROW,	K_RIGHTARROW,	K_AUX26,		K_ESCAPE,		// 0
	K_AUX26,		K_AUX25,		'0',			'1',			'2',			// 5
	'3',			'4',			'5',			'6',			'7',			// 10
	'8',			'9',			'*',			'#',			K_UPARROW,		// 15
	K_DOWNARROW,	K_LEFTARROW,	K_RIGHTARROW,	K_ENTER,		K_AUX32,		// 20
	K_AUX31,		K_AUX29,		K_AUX28,		K_AUX27,		'a',			// 25
	'b',			'c',			'd',			'e',			'f',			// 30
	'g',			'h',			'i',			'j',			'k',			// 35
	'l',			'm',			'n',			'o',			'p',			// 40
	'q',			'r',			's',			't',			'u',			// 45
	'v',			'w',			'x',			'y',			'z',			// 50
	',',			'.',			K_ALT,			K_ALT,			K_SHIFT,		// 55
	K_SHIFT,		K_TAB,			K_SPACE,		0,				0,				// 60
	0,				K_ENTER,		K_BACKSPACE,	'`',			'-',			// 65
	'=',			'[',			']',			'\\',			';',			// 70
	'\'',			'/',			'@',			K_KP_NUMLOCK,	0,				// 75
	0,				'+',			'`',			0,				0,				// 80
	0,				0,				0,				0,				0,				// 85
	0,				0,				K_PGUP,			K_PGDN,			0,				// 90
	0,				K_A_BUTTON,		K_B_BUTTON,		K_C_BUTTON,		K_X_BUTTON,		// 95
	K_Y_BUTTON,		K_Z_BUTTON,		K_LSHOULDER,	K_RSHOULDER,	K_LTRIGGER,		// 100
	K_RTRIGGER,		K_LSTICK,		K_RSTICK,		K_ESCAPE,		K_ESCAPE,		// 105
	0,				K_ESCAPE,		K_DEL,			K_CTRL,			K_CTRL,		// 110
	K_CAPSLOCK,		0,	0,				0,				0,				// 115
	0,				K_PAUSE,		K_HOME,			K_END,			K_INS,			// 120
	0,				0,				0,				0,				0,				// 125
	0,				K_F1,			K_F2,			K_F3,			K_F4,			// 130
	K_F5,			K_F6,			K_F7,			K_F8,			K_F9,			// 135
	K_F10,			K_F11,			K_F12,			K_KP_NUMLOCK,		K_KP_INS,			// 140
	K_KP_END,		K_KP_DOWNARROW,	K_KP_PGDN,		K_KP_LEFTARROW,	K_KP_5,			// 145
	K_KP_RIGHTARROW,K_KP_HOME,		K_KP_UPARROW,		K_KP_PGUP,		K_KP_SLASH,		// 150
	0,		K_KP_MINUS,		K_KP_PLUS,		K_KP_DEL,			',',			// 155
	K_KP_ENTER,		'=',		'(',			')'
};


#define ANDROID_MAX_EVENTS 64
#define MAX_FINGERS 10

typedef enum event_type
{
	event_touch_down,
	event_touch_up,
	event_touch_move,
	event_key_down,
	event_key_up,
	event_set_pause,
	event_resize,
	event_joyhat,
	event_joyball,
	event_joybutton,
	event_joyaxis,
	event_joyadd,
	event_joyremove
} eventtype_t;

typedef struct touchevent_s
{
	float x;
	float y;
	float dx;
	float dy;
} touchevent_t;

typedef struct joyball_s
{
	short xrel;
	short yrel;
	byte ball;
} joyball_t;

typedef struct joyhat_s
{
	byte hat;
	byte key;
} joyhat_t;

typedef struct joyaxis_s
{
	short val;
	byte axis;
} joyaxis_t;

typedef struct joybutton_s
{
	int down;
	byte button;
} joybutton_t;

typedef struct keyevent_s
{
	int code;
} keyevent_t;

typedef struct event_s
{
	eventtype_t type;
	int arg;
	union
	{
		touchevent_t touch;
		joyhat_t hat;
		joyball_t ball;
		joyaxis_t axis;
		joybutton_t button;
		keyevent_t key;
	};
} event_t;

typedef struct finger_s
{
	float x, y;
	qboolean down;
} finger_t;

static struct {
	pthread_mutex_t mutex; // this mutex is locked while not running frame, used for events synchronization
	pthread_mutex_t framemutex; // this mutex is locked while engine is running and unlocked while it reading events, used for pause in background.
	event_t queue[ANDROID_MAX_EVENTS];
	volatile int count;
	finger_t fingers[MAX_FINGERS];
	char inputtext[256];
} events = { PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER };


typedef struct appdata
{
	Evas_Object *win;
	Evas_Object *img;
	
	Evas_GL *evasgl;
	Evas_GL_API *glapi;
	Evas_GL_Surface *sfc;
	Evas_GL_Context *ctx;
	Evas_GL_Config *cfg;
	Evas_Coord sfc_w, sfc_h;
	
	double oldtime;
	double newtime;
} appdata_s;

static pthread_t thread_id;

static void
win_delete_request_cb(void *data, Evas_Object *obj, void *event_info)
{
	ui_app_exit();
}

static void
win_back_cb(void *data, Evas_Object *obj, void *event_info)
{
	appdata_s *ad = data;
	/* Let window go to hide state. */
	elm_win_lower(ad->win);
}


static void
img_pixels_get_cb( void *data, Evas_Object *obj )
{
	appdata_s *ad = data;
	
	Evas_GL_API *gl = ad->glapi;
	
	
	evas_gl_make_current( ad->evasgl, ad->sfc, ad->ctx );
	
	ad->newtime = Sys_DoubleTime();
	
	Host_Frame( ad->newtime - ad->oldtime );
	nanoGL_Flush( );
	
	ad->oldtime = ad->newtime;
}

static void
setup_gles( appdata_s *ad )
{
	/* Window */
	/* Create and initialize elm_win.
	   elm_win is mandatory to manipulate window. */
	
	elm_config_accel_preference_set("opengl");
	
	ad->win = elm_win_util_standard_add(PACKAGE, PACKAGE);
	elm_win_autodel_set(ad->win, EINA_TRUE);

	if (elm_win_wm_rotation_supported_get(ad->win)) {
		int rots[4] = { 0, 90, 180, 270 };
		elm_win_wm_rotation_available_rotations_set(ad->win, (const int *)(&rots), 4);
	}

	evas_object_smart_callback_add(ad->win, "delete,request", win_delete_request_cb, NULL);
	eext_object_event_callback_add(ad->win, EEXT_CALLBACK_BACK, win_back_cb, ad);

	/* Conformant */
	/* Create and initialize elm_conformant.
	   elm_conformant is mandatory for base gui to have proper size
	   when indicator or virtual keypad is visible. */
	/*ad->conform = elm_conformant_add(ad->win);
	elm_win_indicator_mode_set(ad->win, ELM_WIN_INDICATOR_SHOW);
	elm_win_indicator_opacity_set(ad->win, ELM_WIN_INDICATOR_OPAQUE);
	evas_object_size_hint_weight_set(ad->conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(ad->win, ad->conform);
	evas_object_show(ad->conform);*/
			
	ad->cfg = evas_gl_config_new( );
	ad->cfg->color_format = EVAS_GL_RGBA_8888;
	ad->cfg->depth_bits   = EVAS_GL_DEPTH_BIT_24;
	ad->cfg->stencil_bits = EVAS_GL_STENCIL_NONE;
	ad->cfg->options_bits = EVAS_GL_OPTIONS_NONE;
	
	evas_object_geometry_get( ad->win, NULL, NULL, &ad->sfc_w, &ad->sfc_h );
	ad->sfc = evas_gl_surface_create( ad->evasgl, ad->cfg, ad->sfc_w, ad->sfc_h );
	
	ad->ctx = evas_gl_context_version_create( ad->evasgl, NULL, EVAS_GL_GLES_1_X );
	
	ad->glapi = evas_gl_api_get( ad->evasgl );
	
	ad->img = evas_object_image_filled_add( evas_object_evas_get( ad->win ) );
	evas_object_image_pixels_get_callback_set( ad->img, img_pixels_get_cb, ad );
	
	/* Show window after base gui is set up */
	evas_object_show(ad->win);
}

static void
run_xash( appdata_s *ad )
{
	char *progname = "xash";
	char *argv[] = { "xash", "-dev", "5" };
	int argc = 3;
	
	Host_Main( argc, argv, progname, FALSE, NULL );
}

static bool
app_create(void *data)
{
	/* Hook to take necessary actions before main event loop starts
		Initialize UI resources and application's data
		If this function returns true, the main loop of application starts
		If this function returns false, the application is terminated */
	appdata_s *ad = data;

	setup_gles( ad );
	run_xash( ad );

	return true;
}

static void
app_control(app_control_h app_control, void *data)
{
	/* Handle the launch request. */
}

static void
app_pause(void *data)
{
	/* Take necessary actions when application becomes invisible. */
}

static void
app_resume(void *data)
{
	/* Take necessary actions when application becomes visible. */
}

static void
app_terminate(void *data)
{
	appdata_s *ad = data;
	/* Release all resources. */
	
	evas_gl_context_destroy( ad->evasgl, ad->ctx );
	ad->ctx = NULL;
	
	evas_gl_surface_destroy( ad->evasgl, ad->sfc );
	ad->sfc = NULL;
	
	evas_gl_config_free( ad->cfg );
	ad->cfg = NULL;
	
	evas_gl_free( ad->evasgl );
	ad->evasgl = NULL;
}

static void
ui_app_lang_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LANGUAGE_CHANGED*/
	char *locale = NULL;
	system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE, &locale);
	elm_language_set(locale);
	free(locale);
	return;
}

static void
ui_app_orient_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_DEVICE_ORIENTATION_CHANGED*/
	return;
}

static void
ui_app_region_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_REGION_FORMAT_CHANGED*/
}

static void
ui_app_low_battery(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LOW_BATTERY*/
}

static void
ui_app_low_memory(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LOW_MEMORY*/
}

int
main(int argc, char *argv[])
{
	appdata_s ad = {0,};
	int ret = 0;

	ui_app_lifecycle_callback_s event_callback = {0,};
	app_event_handler_h handlers[5] = {NULL, };

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;
	event_callback.pause = app_pause;
	event_callback.resume = app_resume;
	event_callback.app_control = app_control;

	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY, ui_app_low_battery, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY], APP_EVENT_LOW_MEMORY, ui_app_low_memory, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED], APP_EVENT_DEVICE_ORIENTATION_CHANGED, ui_app_orient_changed, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, ui_app_lang_changed, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED], APP_EVENT_REGION_FORMAT_CHANGED, ui_app_region_changed, &ad);

	ret = ui_app_main(argc, argv, &event_callback, &ad);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "app_main() is failed. err = %d", ret);
	}

	return ret;
}
