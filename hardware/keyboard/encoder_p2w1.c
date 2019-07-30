/***************************************************************************
 *            encoder_p2w1.c
 *
 *  Thu Mar 15 13:40:45 2012
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
 * \addtogroup ENCODER Interface um einen Drehencoder am Controller einzubinden (encoder_p2w1.c).
 *
 * @{
 */

/**
 *
 * \author Dirk Broßwick
 *
 */

#if defined( ENCODER_P2W1 )

#include "encoder_p2w1.h"
#include "hardware/gpio/gpio_core.h"
#include "system/buffer/fifo.h"
#include "system/clock/clock.h"

#if defined(ATXM2)

	#define		ENCODER_P2W1_INVERSE
	#define 	ENCODER_P2W1_PULLUP
	const char 	ENCODER_P2W1_PHASE[] PROGMEM = { PORTC_2, PORTC_3 };
	const char 	ENCODER_P2W1_DATA[] PROGMEM = { PORTC_4 };
	const char 	ENCODER_P2W1_CHAR[] PROGMEM = { '\r' };
	char 		ENCODER_P2W1_time[ sizeof ( ENCODER_P2W1_DATA ) ];

#endif

char ENCODER_P2W1_Buffer[ ENCODER_P2W1_Bufferlen ];
int ENCODER_P2W1_fifo;

// Dekodertabelle für wackeligen Rastpunkt
// halbe Auflösung
const char ENCODER_P2W1_table[] PROGMEM = {0,0,'a',0,0,0,0,'s','s',0,0,0,0,'a',0,0};    
 
// Dekodertabelle für normale Drehgeber
// volle Auflösung
//const char ENCODER_P2W1_table[16] PROGMEM = {0,1,-1,0,-1,0,0,1,1,0,0,-1,0,-1,1,0};   

void ENCODER_P2W1_init( void )
{
	int i;

	for ( i = 0 ; i < sizeof( ENCODER_P2W1_DATA ) ; i++ )
	{
		GPIO_setdirection( GPIO_IN , pgm_read_byte( &ENCODER_P2W1_DATA[ i ] ) );
#if defined( ENCODER_P2W1_PULLUP )
		GPIO_setPullUp( pgm_read_byte( &ENCODER_P2W1_DATA[ i ] ) );
#endif
		ENCODER_P2W1_time[ i ] = 0;
	}
	
	ENCODER_P2W1_fifo = Get_FIFO( ENCODER_P2W1_Buffer, ENCODER_P2W1_Bufferlen );
	CLOCK_RegisterCallbackFunction( ENCODER_P2W1_Interrupt, MSECOUND );
}

/**
 * \brief Die Interruptroutine zur Auswerten der Tasten.
 * \param	NONE
 * \return  NONE
 */
void ENCODER_P2W1_Interrupt( void )
{
	static int last = 0;
	char enc_delta;
	int i;

	for( i = 0 ; i < sizeof( ENCODER_P2W1_DATA ) ; i++ )
	{
		// checken ob Taster gedrueckt, wenn ja, Timer hoch zaehlen sonst auf 0 setzen
		if ( GPIO_getPin( pgm_read_byte( &ENCODER_P2W1_DATA[ i ] ) ) )
		{
#if defined( ENCODER_P2W1_INVERSE )
			ENCODER_P2W1_time[ i ] = 0;
#else
			ENCODER_P2W1_time[ i ]++;			
#endif
		}
		else
		{
#if defined( ENCODER_P2W1_INVERSE )
			ENCODER_P2W1_time[ i ]++;
#else
			ENCODER_P2W1_time[ i ] = 0;			
#endif
		}

		// wenn gedrueckte Taste entdeckt, dann Zeichen ab in den Puffer
		if ( ENCODER_P2W1_time[ i ] != 0 )
		{
			if ( ENCODER_P2W1_time[ i ] == 1 )
			{
				Put_Byte_in_FIFO ( ENCODER_P2W1_fifo, pgm_read_byte( &ENCODER_P2W1_CHAR[ i ] ) );
			}
			// Wenn Taste laenger gedrueckt, warten auf DELAY-Wert
			else if ( ENCODER_P2W1_time [ i ] == ENCODER_P2W1_DELAY )
			{
				// Taste speichern
				Put_Byte_in_FIFO ( ENCODER_P2W1_fifo, pgm_read_byte( &ENCODER_P2W1_CHAR[ i ] ) );
				// Vom aktuellen Wert den Repeatwert abziehen
				ENCODER_P2W1_time [ i ] = ENCODER_P2W1_time[ i ] - ENCODER_P2W1_REPEAT;
			}
		}
	}
	
    last = (last << 2)  & 0x0F;
    if ( GPIO_getPin( pgm_read_byte( &ENCODER_P2W1_PHASE[ 0 ] ) ) )
		last |=2;
    if ( GPIO_getPin( pgm_read_byte( &ENCODER_P2W1_PHASE[ 1 ] ) ) )
		last |=1;

	enc_delta = pgm_read_byte( &ENCODER_P2W1_table[last] );

	if ( enc_delta )
	{
				// Taste speichern
				Put_Byte_in_FIFO ( ENCODER_P2W1_fifo, enc_delta );
	}
	
}


/**
 * \brief Holt eine Tasten aus den Tastenpuffer
 * \param	NONE
 * \return  Keyvalue or EOF
 */
int ENCODER_P2W1_GetKey( void )
{
	int retval = EOF;
	
	if ( Get_Bytes_in_FIFO ( ENCODER_P2W1_fifo ) != 0 )
		retval = Get_Byte_from_FIFO( ENCODER_P2W1_fifo );

	return( retval );
}

/**
 * \brief Löscht den Tastenpuffer
 * \param	NONE
 * \return  NONE
 */
void ENCODER_P2W1_Flush( void )
{
	Flush_FIFO( ENCODER_P2W1_fifo );
}

#endif

/**
 * @}
 */
