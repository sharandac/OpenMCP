/***************************************************************************
 *            gpio_out.c
 *
 *  Mon Mar  2 03:36:12 2009
 *  Copyright  2009  Dirk Broßwick
 *  <sharandac@snafu.de>
 ****************************************************************************/
/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <stdlib.h>
#include "config.h"

/**
 * \ingroup GPIO_HAL
 * \addtogroup GPIO_OUT Die GPIO-Funktionen für Ausgänge (gpio_out.c)
 *
 * @{
 */

/**
 * \author Dirk Broßwick
 */

#if defined(DIGITAL_IO)

#include "gpio_core.h"
#include "gpio_out.h"

#if defined(AVRNETIO)

	const char GPIO_OUT_DATA[] PROGMEM = { PORTC_0, PORTC_1, PORTC_2, PORTC_3, PORTC_4, PORTC_5, PORTC_6, PORTC_7 , PORTA_6, PORTA_5 };

#elif defined(EtherSense)

	const char GPIO_OUT_DATA[] PROGMEM = { PORTC_4, PORTC_5, PORTC_6, PORTC_7, PORTD_0, PORTD_1, PORTD_2, PORTD_3, PORTD_4, PORTD_5 };

#elif defined(myAVR)

	const char GPIO_OUT_DATA[] PROGMEM = { PORTA_0, PORTA_1, PORTA_2 };

#elif defined(XPLAIN)

	const char GPIO_OUT_DATA[] PROGMEM = { PORTE_0, PORTE_1, PORTE_2, PORTE_3, PORTE_4, PORTE_5, PORTE_6, PORTE_7 };

#elif defined(ATXM2)

	const char GPIO_OUT_DATA[] PROGMEM = { PORTD_0, PORTD_1, PORTD_2, PORTD_3, PORTD_4, PORTD_5, PORTD_6, PORTD_7 };

#endif

/**
 * \brief Initialisiert die Digitalen Ausgänge
 */
void GPIO_out_init( void )
{
	int i;

	for ( i = 0 ; i < sizeof( GPIO_OUT_DATA ) ; i++ )
	{
		GPIO_setdirection( GPIO_OUT , pgm_read_byte( &GPIO_OUT_DATA[ i ] ) );
	}
}

/**
 * \brief Setzt einen Ausgang auf High.
 * \param 	PIN		Pinnummer welcher auf High gesetzt werden soll
 */
void GPIO_out_set( char pin )
{
	if ( pin >= sizeof( GPIO_OUT_DATA ) )
	    return;
	GPIO_setPin( pgm_read_byte( &GPIO_OUT_DATA[ (int)pin ] ) );
}

/**
 * \brief Setzt einen Ausgang auf Low.
 * \param 	PIN		Pinnummer welcher auf Low gesetzt werden soll
 */
void GPIO_out_clear( char pin )
{
	if ( pin >= sizeof( GPIO_OUT_DATA ) )
	    return;
	GPIO_clearPin( pgm_read_byte( &GPIO_OUT_DATA[ (int)pin ] ) );
}

/**
 * \brief Liest den Status eines Ausganges ein.
 * \param	PIN		Pinnummer des Ausganges der eingelesen werden soll.
 */
char GPIO_out_state( char pin )
{
	char returnval=0;
	
	if ( pin >= sizeof( GPIO_OUT_DATA ) )
		return(0);
	
	if ( GPIO_getPin( pgm_read_byte( &GPIO_OUT_DATA[ (int)pin ] ) ) != 0 )
		returnval = 1;
	
	return( returnval );
}

/**
 * \brief Gibt die Anzahl der GPIO Ausgänge zurück.
 */
char GPIO_out_max( void )
{
	return( sizeof( GPIO_OUT_DATA ) );
}

/**
 * \brief Gibt zu einem Pin den realen Pin zurück.
 * \param	PIN		Pinnummer.
 */
char GPIO_out_Port( char pin )
{
	if ( pin >= sizeof( GPIO_OUT_DATA ) )
		return(0);

	return( pgm_read_byte( &GPIO_OUT_DATA[ (int)pin ] ) );
}
#endif
/**
 * @}
 */
