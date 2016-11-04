/*
s_backend.c - sound hardware output
Copyright (C) 2009 Uncle Mike

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
#if XASH_SOUND == SOUND_TIZEN

#include "sound.h"

#include <media/audio_io.h>

#define SAMPLE_16BIT_SHIFT		1
#define SECONDARY_BUFFER_SIZE		0x10000

/*
=======================================================================
Global variables. Must be visible to window-procedure function
so it can unlock and free the data block after it has been played.
=======================================================================
*/
extern convar_t		*s_primary;
extern dma_t			dma;

static audio_out_h tizenAudio;
static int bufferSize;

//static qboolean	snd_firsttime = true;
//static qboolean	primary_format_set;

static void Tizen_PrintAudioError_( const char *function, int error, const char *msg )
{
	if( error == AUDIO_IO_ERROR_INVALID_PARAMETER )
	{
		Msg( "Audio: %s(%s): Invalid parameter\n", function, msg );
	}
	else if( error == AUDIO_IO_ERROR_OUT_OF_MEMORY )
	{
		Msg( "Audio: %s(%s): Out of memory\n", function, msg );
	}
	else if( error == AUDIO_IO_ERROR_DEVICE_NOT_OPENED )
	{
		Msg( "Audio: %s(%s): Device not opened\n", function, msg );
	}
	else if( error == AUDIO_IO_ERROR_SOUND_POLICY )
	{
		Msg( "Audio: %s(%s): Policy error\n", function, msg );
	}
	else
	{
		Msg( "Audio: %s(%s): Error code %i\n", error );
	}
}

#define Tizen_PrintError( x )         Tizen_PrintAudioError_( __FUNCTION__, ( x ), "" )
#define Tizen_PrintMsgError( x, msg ) Tizen_PrintAudioError_( __FUNCTION__, ( x ), ( msg ) )

/*
==================
SNDDMA_Init

Try to find a sound device to mix for.
Returns false if nothing is found.
==================
*/
qboolean SNDDMA_Init( void *hInst )
{
	int ret;	
	
	ret = audio_out_create( SOUND_DMA_SPEED,
		AUDIO_CHANNEL_STEREO, AUDIO_SAMPLE_TYPE_S16_LE, 
		SOUND_TYPE_MEDIA, &tizenAudio );
	
	if( ret )
	{
		Tizen_PrintError( ret );
		goto fail;
	}
	
	audio_out_get_buffer_size( tizenAudio, &bufferSize );
	
	dma.format.speed = SOUND_DMA_SPEED;
	dma.format.channels = 2;
	dma.format.width = 2;
	dma.samples = 0x8000 * dma.format.channels;
	dma.buffer = Z_Malloc(dma.samples * 2);
	dma.samplepos = 0;
	dma.sampleframes = dma.samples / dma.format.channels;
	
	dma.initialized = true;
	
	Con_Printf( "Using Tizen audio driver: %d Hz\n", SOUND_DMA_SPEED );
	
	return true;	
fail:
	SNDDMA_Shutdown();
	return false;
		
}

/*
==============
SNDDMA_GetDMAPos

return the current sample position (in mono samples read)
inside the recirculating dma buffer, so the mixing code will know
how many sample are required to fill it up.
===============
*/
int SNDDMA_GetDMAPos( void )
{
	return dma.samplepos;
}

/*
==============
SNDDMA_GetSoundtime

update global soundtime
===============
*/
int SNDDMA_GetSoundtime( void )
{
	static int	buffers, oldsamplepos;
	int		samplepos, fullsamples;
	
	fullsamples = dma.samples / 2;

	// it is possible to miscount buffers
	// if it has wrapped twice between
	// calls to S_Update.  Oh well.
	samplepos = SNDDMA_GetDMAPos();

	if( samplepos < oldsamplepos )
	{
		buffers++; // buffer wrapped

		if( paintedtime > 0x40000000 )
		{	
			// time to chop things off to avoid 32 bit limits
			buffers = 0;
			paintedtime = fullsamples;
			S_StopAllSounds();
		}
	}

	oldsamplepos = samplepos;

	return (buffers * fullsamples + samplepos / 2);
}

/*
==============
SNDDMA_BeginPainting

Makes sure dma.buffer is valid
===============
*/
void SNDDMA_BeginPainting( void )
{
	int ret = audio_out_prepare( tizenAudio );
	
	if( ret ) 
		Tizen_PrintError( ret );
}

/*
==============
SNDDMA_Submit

Send sound to device if buffer isn't really the dma buffer
Also unlocks the dsound buffer
===============
*/
void SNDDMA_Submit( void )
{
	int ret;
	int size = dma.samples << 1;
	int pos = dma.samplepos << 1;
	int wrapped = pos + bufferSize - size;

	if (wrapped < 0) 
	{
		audio_out_write(tizenAudio, dma.buffer + pos, bufferSize);
		dma.samplepos += bufferSize >> 1;
	} 
	else 
	{
		int remaining = size - pos;
		audio_out_write(tizenAudio, dma.buffer + pos, remaining );
		audio_out_write(tizenAudio, dma.buffer, wrapped );
		dma.samplepos = wrapped >> 1;
	}
	
	ret = audio_out_unprepare( tizenAudio );
	if( ret ) Tizen_PrintError( ret );
}

/*
==============
SNDDMA_Shutdown

Reset the sound device for exiting
===============
*/
void SNDDMA_Shutdown( void )
{
	int ret;
	
	Msg( "Shutting down Tizen audio.\n" );
	dma.initialized = false;

	ret = audio_out_destroy( tizenAudio );
	if( ret ) 
		Tizen_PrintError( ret );

	if( dma.buffer )
	{
		 Mem_Free( dma.buffer );
		 dma.buffer = NULL;
	}
}

/*
===========
S_PrintDeviceName
===========
*/
void S_PrintDeviceName( void )
{
	Msg( "Audio: Tizen\n" );
}


/*
===========
S_Activate
Called when the main window gains or loses focus.
The window have been destroyed and recreated
between a deactivate and an activate.
===========
*/
void S_Activate( qboolean active )
{
}
#endif
