/***************************************************************************
 *            key.c
 *
 *  Sun Sep 11 17:18:16 2011
 *  Copyright  2011  Dirk Broßwick
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

#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "config.h"

/**
 *
 * \ingroup Hardware
 * \addtogroup KEYBOARD Interface um Tasten am Controller einzubinden (keyboard.c).
 *
 * @{
 */

/**
 *
 * \author Dirk Broßwick
 *
 */

#if defined( KEYBOARD )

#include "keyboard.h"
#include "hardware/gpio/gpio_core.h"
#include "system/buffer/fifo.h"
#include "system/clock/clock.h"

#if defined(ATXM2)

	#define		KEYBOARD_INVERSE
	#define 	KEYBOARD_PULLUP
	const char 	KEYBOARD_DATA[] PROGMEM = { PORTF_0, PORTF_1, PORTF_2, PORTF_3 };
	const char 	KEYBOARD_CHAR[] PROGMEM = { 's', 'c', 'f', 'e' };
	char 		KEYBOARD_time[ sizeof ( KEYBOARD_DATA ) ];

#endif

char KEYBOARD_Buffer[ KEYBOARD_Bufferlen ];

int KEYBOARD_fifo;

/**
 * \brief Initialisiert die Tasten.
 * \param	NONE
 * \return  NONE
 */
void KEYBOARD_init( void )
{
	int i;

	for ( i = 0 ; i < sizeof( KEYBOARD_DATA ) ; i++ )
	{
		GPIO_setdirection( GPIO_IN , pgm_read_byte( &KEYBOARD_DATA[ i ] ) );
#if defined( KEYBOARD_PULLUP )
		GPIO_setPullUp( pgm_read_byte( &KEYBOARD_DATA[ i ] ) );
#endif
		KEYBOARD_time[ i ] = 0;
	}
	
	KEYBOARD_fifo = Get_FIFO( KEYBOARD_Buffer, KEYBOARD_Bufferlen );
	CLOCK_RegisterCallbackFunction( KEYBOARD_Interrupt, MSECOUND );
}

/**
 * \brief Die Interruptroutine zur Auswerten der Tasten.
 * \param	NONE
 * \return  NONE
 */
void KEYBOARD_Interrupt( void )
{
	int i;

	for( i = 0 ; i < sizeof( KEYBOARD_DATA ) ; i++ )
	{
		// checken ob Taster gedrueckt, wenn ja, Timer hoch zaehlen sonst auf 0 setzen
		if ( GPIO_getPin( pgm_read_byte( &KEYBOARD_DATA[ i ] ) ) )
		{
#if defined( KEYBOARD_INVERSE )
			KEYBOARD_time[ i ] = 0;
#else
			KEYBOARD_time[ i ]++;			
#endif
		}
		else
		{
#if defined( KEYBOARD_INVERSE )
			KEYBOARD_time[ i ]++;
#else
			KEYBOARD_time[ i ] = 0;			
#endif
		}

		// wenn gedrueckte Taste entdeckt, dann Zeichen ab in den Puffer
		if ( KEYBOARD_time[ i ] != 0 )
		{
			if ( KEYBOARD_time[ i ] == 1 )
			{
				Put_Byte_in_FIFO ( KEYBOARD_fifo, pgm_read_byte( &KEYBOARD_CHAR[ i ] ) );
			}
			// Wenn Taste laenger gedrueckt, warten auf DELAY-Wert
			else if ( KEYBOARD_time [ i ] == KEYBOARD_DELAY )
			{
				// Taste speichern
				Put_Byte_in_FIFO ( KEYBOARD_fifo, pgm_read_byte( &KEYBOARD_CHAR[ i ] ) );
				// Vom aktuellen Wert den Repeatwert abziehen
				KEYBOARD_time [ i ] = KEYBOARD_time[ i ] - KEYBOARD_REPEAT;
			}
		}
	}
}

/**
 * \brief Holt eine Tasten aus den Tastenpuffer
 * \param	NONE
 * \return  Keyvalue or EOF
 */
int KEYBOARD_GetKey( void )
{
	int retval = EOF;
	
	if ( Get_Bytes_in_FIFO ( KEYBOARD_fifo ) != 0 )
		retval = Get_Byte_from_FIFO( KEYBOARD_fifo );

	return( retval );
}

/**
 * \brief Löscht den Tastenpuffer
 * \param	NONE
 * \return  NONE
 */
void KEYBOARD_Flush( void )
{
	Flush_FIFO( KEYBOARD_fifo );
}

#endif // KEYBOARD

/**
 * @}
 */
