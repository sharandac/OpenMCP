/***************************************************************************
 *            vs10xx_buffer.c
 *
 *  Mon Feb  6 18:30:48 2012
 *  Copyright  2012  Dirk Broßwick
 *  <sharandac@snafu.de>
 ****************************************************************************/

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */

/**
 * \ingroup VS10xx
 * \addtogroup softbuffer Support fuer den VS10xx. (vs10xx_buffer.c)
 *
 * Software buffering für den VS10xx.
 *
 * @{
 */

/**
 * \file
 *
 * \author Dirk Broßwick
 */

#include <avr/pgmspace.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/io.h>

#include "system/buffer/fifo.h"
#include "system/clock/clock.h"

#include "config.h"

#if defined(VS10XX)

#include "vs10xx.h"
#include "vs10xx_buffer.h"

static char vs10xx_buffer[ 1024 * 12 ];
volatile unsigned char vs10xx_fifo = -1;
static char stream_source = -1;
static char slowstart = 0;
static int timer=0, frames = 0, fps = 0;
static long bandbreite = 0, band = 0;
static long bytecounter = 0;

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Initialisiert den Software-buffer.
 * \return	NONE		None
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
void streambuffer_init( void )
{
	// Register FIFO
	vs10xx_fifo = Get_FIFO ( vs10xx_buffer, sizeof( vs10xx_buffer ) );

	if ( vs10xx_fifo == FIFO_ERROR )
	{
		printf_P(PSTR("FIFO-Error"));
		while(1);
	}
	
	Flush_FIFO( vs10xx_fifo );
	// Callback eintragen
	CLOCK_RegisterCallbackFunction( streambuffer_irq , MSECOUND );
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Interruptroutine zum befüllen des VS10XX
 * \return	NONE		None
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
void streambuffer_irq( void )
{	
	static int counter = 0;

	counter++;

	if ( counter == 100 )
	{
		fps = frames;
		frames = 0;
		counter = 0;
		band = ( band + ( bandbreite * 32 * 8 ) ) / 2;
		bandbreite = 0;
	}

	// check buffer to set slowstart
	if ( Get_FIFOrestsize( vs10xx_fifo ) < ( sizeof( vs10xx_buffer ) / 2 ) )
	{
		slowstart = 1;
	}
	else if ( Get_Bytes_in_FIFO( vs10xx_fifo ) < ( sizeof( vs10xx_buffer ) / 8 ) )
	{
		slowstart = 0;
	}
	
	int blocks = 0;

	// push data into decoder
	if ( slowstart == 1 )
	{
		blocks = VS10xx_flush_from_FIFO( vs10xx_fifo );
		bandbreite = bandbreite + blocks;
	}
	
	// prevent clicks on startup
	if ( timer != 0 )
	{
		if ( blocks == 0 )
			timer--;
		else
			if ( timer <= 100 )
			timer++;

		if ( timer == 0 )
		{
			slowstart = 0;

			Flush_FIFO( vs10xx_fifo );
			stream_source = -1;
			bytecounter = 0;
//			VS10xx_reset();
		}
	}
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Schiebt Daten in den Softwarebuffer.
 * \param 	buffer		Pointer auf den Buffer.
 * \paran	len			Länge der im Buffer befindlichen Daten.
 * \return	NONE		None
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
void streambuffer_fill( char * buffer, int len )
{	
	frames++;
	
	// push data into FIFO
	if ( Put_Block_in_FIFO( vs10xx_fifo , len, buffer ) != FIFO_ERROR )
	{
		bytecounter = bytecounter + len;
		// reload timer
		timer = 100;
	}
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Schiebt Daten in den Softwarebuffer.
 * \param 	buffer		Pointer auf den Buffer.
 * \paran	len			Länge der im Buffer befindlichen Daten.
 * \return	NONE		None
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
void streambuffer_fillfifo( int fifo, int len )
{
	frames++;
	
	// push data into FIFO
	if ( Get_FIFO_to_FIFO( fifo, len , vs10xx_fifo ) != FIFO_ERROR )
	{
		bytecounter = bytecounter + len;
		// reload timer
		timer = 100;
	}
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Gibt die anzahl der zugriff auf den Softwarebuffer in der letzten Sekunden zurück
 * \return	fps		fps
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
int streambuffer_getbandwidth ( void )
{
	return( band / 800 );
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Gibt die anzahl der zugriff auf den Softwarebuffer in der letzten Sekunden zurück
 * \return	fps		fps
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
int streambuffer_getfps ( void )
{
	return( fps );
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Gibt den aktuellen Füllzustand des Softwarebuffer zurück.
 * \param 	len			Größe auf die bezug genommen werden soll
 * \return	len			Füllzustand
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
int streambuffer_get ( int len )
{
	return( Get_Bytes_in_FIFO( vs10xx_fifo ) / ( sizeof( vs10xx_buffer ) / len ) );
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Gibt den Status zurück ob gerade Daten abgespielt werden
 * \return	NONE		None
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
int streambuffer_getstate ( void )
{
	return( timer );
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Gibt den Status zurück ob gerade Daten abgespielt werden
 * \return	NONE		None
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
int streambuffer_getfifo ( void )
{
	return( vs10xx_fifo );
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Gibt die Adresse den Buffer im Speicher zurück
 * \return	char *		Pointer auf den speicher.
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
char * streambuffer_getbase ( void )
{
	return( vs10xx_buffer );
}

void streambuffer_setsource( char source )
{
	stream_source = source;
}

char streambuffer_getsource( void )
{
	return( stream_source );
}

long streambuffer_getcounter( void )
{
	return( bytecounter );
}
#endif // VS10XX

/**
 * @}
**/

