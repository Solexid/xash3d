
#pragma once
#ifndef TIZEN_MAIN_H
#define TIZEN_MAIN_H

#include <app.h>
#include <Elementary.h>
#include <system_settings.h>
#include <efl_extension.h>
#include <dlog.h>

#ifdef  LOG_TAG
#undef  LOG_TAG
#endif
#define LOG_TAG "Xash3D"

#if !defined(PACKAGE)
#define PACKAGE "in.celest.xash3d.hl"
#endif

void Tizen_GetScreenRes( int *w, int *h );

#endif /* TIZEN_MAIN_H */
