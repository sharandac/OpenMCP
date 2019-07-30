/***************************************************************************
 *            gpio_in.c
 *
 *  Mon Mar  2 03:36:12 2009
 *  Copyright  2009  Dirk Broßwick
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
 
#include <avr/io.h>
#include <avr/pgmspace.h>

/**
 * \ingroup GPIO_HAL
 * \addtogroup GPIO_IN Die GPIO-Funktionen für Eingänge (gpio_in.c)
 *
 * @{
 */

/**
 * \author Dirk Broßwick
 */

#include "config.h"

#if defined(DIGITAL_IO)

#include "gpio_in.h"
#include "gpio_core.h"

#if defined(AVRNETIO)

	const char GPIO_IN_DATA[] PROGMEM = { PORTD_3, PORTD_5, PORTD_7, PORTB_0, PORTB_3, PORTA_0, PORTA_1, PORTA_2, PORTA_3, PORTA_4, PORTA_7 };

#elif defined(myAVR)

	const char GPIO_IN_DATA[] PROGMEM = { PORTC_5, PORTC_6 };

#elif defined(EtherSense)

	const char GPIO_IN_DATA[] PROGMEM = { PORTC_2, PORTC_3 };

#elif defined(XPLAIN)

	#define 	GPIO_IN_PULLUP
	const char GPIO_IN_DATA[] PROGMEM = { PORTF_0, PORTF_1, PORTF_2, PORTF_3, PORTF_4, PORTF_5, PORTF_6, PORTF_7 };

#elif defined(ATXM2)

	#define 	GPIO_IN_PULLUP
//	const char GPIO_IN_DATA[] PROGMEM = { PORTF_0, PORTF_1, PORTF_2, PORTF_3 };
	const char GPIO_IN_DATA[] PROGMEM = { PORTC_2, PORTC_3, PORTC_4 };

#endif

/**
 * \brief Initialisiert die Digitalen Einghänge
 */
void GPIO_in_init( void )
{
	int i;

	for ( i = 0 ; i < sizeof( GPIO_IN_DATA ) ; i++ )
	{
		GPIO_setdirection( GPIO_IN , pgm_read_byte( &GPIO_IN_DATA[ i ] ) );
#if defined( GPIO_IN_PULLUP )
		GPIO_setPullUp( pgm_read_byte( &GPIO_IN_DATA[ i ] ) );
#endif
	}
}

/**
 * \brief Liest den digitalen Eingang aus.
 * \param Pinnummer des Eingangs
 */
char GPIO_in_state( int pin )
{
	if ( pin >= sizeof( GPIO_IN_DATA ) )
	    return(0);
	return( GPIO_getPin( pgm_read_byte( &GPIO_IN_DATA[ (int)pin ] ) ) );
}

/**
 * \brief Gibt die Anzahl der GPIO Eingänge zurück.
 */
char GPIO_in_max( void )
{
	return( sizeof( GPIO_IN_DATA ) );
}

/**
 * \brief Gibt zu einem Pin den realen Pin zurück.
 * \param	PIN		Pinnummer.
 */
char GPIO_in_Port( char pin )
{
	if ( pin >= sizeof( GPIO_IN_DATA ) )
		return(0);

	return( pgm_read_byte( &GPIO_IN_DATA[ (int)pin ] ) );
}
#endif

/**
 * @}
 */
