/*! \file led_core.c \brief Stellt die LED Funkionalitaet bereit */
//***************************************************************************
//*            led_core.c
//*
//*  Sat Jun  3 23:01:42 2006
//*  Copyright  2006  User
//*  Email
//****************************************************************************/
///	\ingroup Hardware
///	\defgroup LED Stellt die LED Funkionalitaet bereit (led_core.c)
///	\code #include "led_core.h" \endcode
///	\par Uebersicht
///		Stellt Funktionen bereit um LED über ein definiertes Interface anzusprechen.
//****************************************************************************/
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
//@{

#include <stdio.h>
#include <avr/pgmspace.h>

#include "config.h"

#if defined(LED)

#include "hardware/gpio/gpio_core.h"
#include "led_core.h"

#if defined(OpenMCP)

	#define		LED_INVERT
	const char LED_DATA[] PROGMEM = { PORTD_4 , PORTD_5 , PORTD_6 };

#elif defined(AVRNETIO)

	const char LED_DATA[] PROGMEM = { PORTD_2 , PORTD_4 , PORTD_6 };

#elif defined(myAVR)

	const char LED_DATA[] PROGMEM = { PORTC_4 , PORTC_5 };

#elif defined(XPLAIN)

	const char LED_DATA[] PROGMEM = { PORTF_0 , PORTF_1 , PORTF_2 , PORTF_3 , PORTF_4 , PORTF_5 , PORTF_6 , PORTF_7 };

#elif defined(ATXM2)

//	const char LED_DATA[] PROGMEM = { PORTF_4 , PORTF_5 , PORTF_6 , PORTF_7 };
//	const char LED_DATA[] PROGMEM = { PORTC_0 , PORTC_1};
//	const char LED_DATA[] PROGMEM = { PORTF_0 };

#else

	#error "LED not supported on this Hardware."

#endif

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Hier wird die LED_Core initialisiert.
 * \param	NONE
 * \return	NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
void LED_init( void )
{
	int i;

	for( i = 0 ; i < sizeof( LED_DATA ) ; i++ )
	{
		GPIO_setdirection( GPIO_OUT , pgm_read_byte( &LED_DATA[ i ] ) );
		LED_off( i );
	}
	return;
}

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Schalten eine LED ein.
 * \param	LED_index	Die Nummer der LED.
 * \return	NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
void LED_on ( char LED_index )
{
	if ( LED_index >= sizeof( LED_DATA ) )
	    return;
	#ifndef LED_INVERT
		GPIO_setPin( pgm_read_byte( &LED_DATA[ (int)LED_index ] ) );
	#else
		GPIO_clearPin( pgm_read_byte( &LED_DATA[ (int)LED_index ] ) );
	#endif
}

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Schaltet eine LED aus.
 * \param	LED_index	Die Nummer der LED.
 * \return	NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
void LED_off ( char LED_index )
{
	if ( LED_index >= sizeof( LED_DATA ) )
	    return;
#ifndef LED_INVERT
	GPIO_clearPin( pgm_read_byte( &LED_DATA[ (int)LED_index ] ) );
#else
	GPIO_setPin( pgm_read_byte( &LED_DATA[ (int)LED_index ] ) );
#endif
}

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Toggelt eine LED.
 * \param	LED_index	Die Nummer der LED.
 * \return	NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
void LED_toggle ( char LED_index )
{
	if ( LED_index >= sizeof( LED_DATA ) )
	    return;

#ifndef LED_INVERT
	if ( GPIO_getPin( pgm_read_byte( &LED_DATA[ (int)LED_index ] ) ) != 0 )
		LED_off( LED_index );
	else
		LED_on( LED_index );
#else
	if ( GPIO_getPin( pgm_read_byte( &LED_DATA[ (int)LED_index ] ) ) != 0 )
		LED_on( LED_index );
	else
		LED_off( LED_index );
#endif
}

#endif
