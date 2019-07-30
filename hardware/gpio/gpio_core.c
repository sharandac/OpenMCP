/***************************************************************************
 *            gpio_core.c
 *
 *  Sat Oct  2 19:28:41 2010
 *  Copyright  2010  sharan
 *  <sharan@<host>>
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
#include <stdio.h>
#include <stdlib.h>

#include "config.h"

#include "gpio_core.h"
#include "gpio_in.h"
/**
 * \ingroup Hardware
 * \addtogroup GPIO_HAL HAL für die GPIO
 * \brief HAL für die GPIO
 *
 * @{
 */

/**
 * \file
 *
 * \author Dirk Broßwick
 */

void GPIO_init( void )
{
	GPIO_in_init();
}

/**
 * \brief Setzt die Datenrichtung für einen Pin.
 * \param	PIN		Nummer des Pin.
 */
int GPIO_setdirection( char direction, char PIN )
{
	char returnval = GPIO_ERROR;
	char PORT = PIN >> 3;
	PIN &= 0x7;

//	printf_P( PSTR("Port: %c , Pin: %d\r\n"),  PORT + 'A', PIN );
	         
	if ( PIN > MAX_GPIO_PIN )
		return( returnval );

	switch( PORT )
	{
#if defined(__AVR_XMEGA__)
	#if MAX_GPIO_PORTS > 0
			case 0:		if( direction == GPIO_OUT )
							PORTA.DIRSET = ( 1<<PIN );
						else if( direction == GPIO_IN )
							PORTA.DIRCLR = ( 1<<PIN );
						break;
	#endif
	#if MAX_GPIO_PORTS > 1
			case 1:		if( direction == GPIO_OUT )
							PORTB.DIRSET = ( 1<<PIN );
						else if( direction == GPIO_IN )
							PORTB.DIRCLR = ( 1<<PIN );
						break;
	#endif
	#if MAX_GPIO_PORTS > 2
			case 2:		if( direction == GPIO_OUT )
							PORTC.DIRSET = ( 1<<PIN );
						else if( direction == GPIO_IN )
							PORTC.DIRCLR = ( 1<<PIN );
						break;
	#endif
	#if MAX_GPIO_PORTS > 3
			case 3:		if( direction == GPIO_OUT )
							PORTD.DIRSET = ( 1<<PIN );
						else if( direction == GPIO_IN )
							PORTD.DIRCLR = ( 1<<PIN );
						break;
	#endif
	#if MAX_GPIO_PORTS > 4
			case 4:		if( direction == GPIO_OUT )
							PORTE.DIRSET = ( 1<<PIN );
						else if( direction == GPIO_IN )
							PORTE.DIRCLR = ( 1<<PIN );
						break;
	#endif
	#if MAX_GPIO_PORTS > 5
			case 5:		if( direction == GPIO_OUT )
							PORTF.DIRSET = ( 1<<PIN );
						else if( direction == GPIO_IN )
							PORTF.DIRCLR = ( 1<<PIN );
						break;
	#endif
#else
	#if MAX_GPIO_PORTS > 0
			case 0:		if( direction == GPIO_OUT )
							DDRA |= ( 1<<PIN );
						else if( direction == GPIO_IN )
						DDRA &= ~( 1<<PIN );
						break;
	#endif
	#if MAX_GPIO_PORTS > 1
			case 1:		if( direction == GPIO_OUT )
							DDRB |= ( 1<<PIN );
						else if( direction == GPIO_IN )
							DDRB &= ~( 1<<PIN );
						break;
	#endif
	#if MAX_GPIO_PORTS > 2
			case 2:		if( direction == GPIO_OUT )
							DDRC |= ( 1<<PIN );
						else if( direction == GPIO_IN )
							DDRC &= ~( 1<<PIN );
						break;
	#endif
	#if MAX_GPIO_PORTS > 3
			case 3:		if( direction == GPIO_OUT )
							DDRD |= ( 1<<PIN );
						else if( direction == GPIO_IN )
							DDRD &= ~( 1<<PIN );
						break;
	#endif
	#if MAX_GPIO_PORTS > 4
			case 4:		if( direction == GPIO_OUT )
							DDRE |= ( 1<<PIN );
						else if( direction == GPIO_IN )
							DDRE &= ~( 1<<PIN );
						break;
	#endif
	#if MAX_GPIO_PORTS > 5
			case 5:		if( direction == GPIO_OUT )
							DDRF |= ( 1<<PIN );
						else if( direction == GPIO_IN )
							DDRF &= ~( 1<<PIN );
						break;
	#endif
#endif
	}

	return( returnval );
}

/**
 * \brief Liest die Datenrichtung für einen Pin.
 * \param	PIN		Nummer des Pin.
 */
int GPIO_getdirection( char PIN )
{
	char returnval = GPIO_ERROR;
	char PORT = PIN >> 3;
	PIN &= 0x7;
	
	if ( PIN > MAX_GPIO_PIN )
		return( returnval );

	switch( PORT )
	{
#if defined(__AVR_XMEGA__)
	#if MAX_GPIO_PORTS > 0
			case 0:		returnval = PORTA.DIR & ( 1<<PIN );
						break;
	#endif
	#if MAX_GPIO_PORTS > 1
			case 1:		returnval = PORTB.DIR & ( 1<<PIN );
						break;
	#endif
	#if MAX_GPIO_PORTS > 2
			case 2:		returnval = PORTC.DIR & ( 1<<PIN );
						break;
	#endif
	#if MAX_GPIO_PORTS > 3
			case 3:		returnval = PORTD.DIR & ( 1<<PIN );
						break;
	#endif
	#if MAX_GPIO_PORTS > 4
			case 4:		returnval = PORTE.DIR & ( 1<<PIN );
						break;
	#endif
	#if MAX_GPIO_PORTS > 5
			case 5:		returnval = PORTF.DIR & ( 1<<PIN );
						break;
	#endif
#else
	#if MAX_GPIO_PORTS > 0
			case 0:		returnval = DDRA & ( 1<<PIN );
						break;
	#endif
	#if MAX_GPIO_PORTS > 1
			case 1:		returnval = DDRB & ( 1<<PIN );
						break;
	#endif
	#if MAX_GPIO_PORTS > 2
			case 2:		returnval = DDRC & ( 1<<PIN );
						break;
	#endif
	#if MAX_GPIO_PORTS > 3
			case 3:		returnval = DDRD & ( 1<<PIN );
						break;
	#endif
	#if MAX_GPIO_PORTS > 4
			case 4:		returnval = DDRE & ( 1<<PIN );
						break;
	#endif
	#if MAX_GPIO_PORTS > 5
			case 5:		returnval = DDRF & ( 1<<PIN );
						break;
	#endif
#endif
	}
	return( returnval );
}

/**
 * \brief Setzt einen Pin auf High.
 * \param	PIN		Nummer des Pin.
 */
int GPIO_setPin( char PIN )
{
	char returnval = GPIO_ERROR;
	char PORT = PIN >> 3;
	PIN &= 0x7;
	
	if ( PIN > MAX_GPIO_PIN )
		return( returnval );

	switch( PORT )
	{
#if defined(__AVR_XMEGA__)
	#if MAX_GPIO_PORTS > 0
			case 0:		PORTA.OUTSET = ( 1<<PIN );
						break;
	#endif
	#if MAX_GPIO_PORTS > 1
			case 1:		PORTB.OUTSET = ( 1<<PIN );
						break;
	#endif
	#if MAX_GPIO_PORTS > 2
			case 2:		PORTC.OUTSET = ( 1<<PIN );
						break;
	#endif
	#if MAX_GPIO_PORTS > 3
			case 3:		PORTD.OUTSET = ( 1<<PIN );
						break;
	#endif
	#if MAX_GPIO_PORTS > 4
			case 4:		PORTE.OUTSET = ( 1<<PIN );
						break;
	#endif
	#if MAX_GPIO_PORTS > 5
			case 5:		PORTF.OUTSET = ( 1<<PIN );
						break;
	#endif
#else
	#if MAX_GPIO_PORTS > 0
			case 0:		PORTA |= ( 1<<PIN );
						break;
	#endif
	#if MAX_GPIO_PORTS > 1
			case 1:		PORTB |= ( 1<<PIN );
						break;
	#endif
	#if MAX_GPIO_PORTS > 2
			case 2:		PORTC |= ( 1<<PIN );
						break;
	#endif
	#if MAX_GPIO_PORTS > 3
			case 3:		PORTD |= ( 1<<PIN );
						break;
	#endif
	#if MAX_GPIO_PORTS > 4
			case 4:		PORTE |= ( 1<<PIN );
						break;
	#endif
	#if MAX_GPIO_PORTS > 5
			case 5:		PORTF |= ( 1<<PIN );
						break;
	#endif
#endif
	}
	return( returnval );

}

/**
 * \brief Setzt einen Pin auf LOW.
 * \param	PIN		Nummer des Pin.
 */
int GPIO_clearPin( char PIN )
{
	char returnval = GPIO_ERROR;
	char PORT = PIN >> 3;
	PIN &= 0x7;
	
	if ( PIN > MAX_GPIO_PIN )
		return( returnval );

	switch( PORT )
	{
#if defined(__AVR_XMEGA__)
	#if MAX_GPIO_PORTS > 0
			case 0:		PORTA.OUTCLR = ( 1<<PIN );
						break;
	#endif
	#if MAX_GPIO_PORTS > 1
			case 1:		PORTB.OUTCLR = ( 1<<PIN );
						break;
	#endif
	#if MAX_GPIO_PORTS > 2
			case 2:		PORTC.OUTCLR = ( 1<<PIN );
						break;
	#endif
	#if MAX_GPIO_PORTS > 3
			case 3:		PORTD.OUTCLR = ( 1<<PIN );
						break;
	#endif
	#if MAX_GPIO_PORTS > 4
			case 4:		PORTE.OUTCLR = ( 1<<PIN );
						break;
	#endif
	#if MAX_GPIO_PORTS > 5
			case 5:		PORTF.OUTCLR = ( 1<<PIN );
						break;
	#endif
#else
	#if MAX_GPIO_PORTS > 0
			case 0:		PORTA &= ~( 1<<PIN );
						break;
	#endif
	#if MAX_GPIO_PORTS > 1
			case 1:		PORTB &= ~( 1<<PIN );
						break;
	#endif
	#if MAX_GPIO_PORTS > 2
			case 2:		PORTC &= ~( 1<<PIN );
						break;
	#endif
	#if MAX_GPIO_PORTS > 3
			case 3:		PORTD &= ~( 1<<PIN );
						break;
	#endif
	#if MAX_GPIO_PORTS > 4
			case 4:		PORTE &= ~( 1<<PIN );
						break;
	#endif
	#if MAX_GPIO_PORTS > 5
			case 5:		PORTF &= ~( 1<<PIN );
						break;
	#endif
#endif
	}
	return( returnval );
}

/**
 * \brief Liest einen Pin ein.
 * \param	PIN		Nummer des Pin.
 * \retval	0 = LOW ; !0 = HIGH
 */
int GPIO_getPin( char PIN )
{
	char returnval = GPIO_ERROR;
	char PORT = PIN >> 3;
	PIN &= 0x7;

	if ( PIN > MAX_GPIO_PIN )
		return( returnval );
	
	switch( PORT )
	{
#if defined(__AVR_XMEGA__)
	#if MAX_GPIO_PORTS > 0
			case 0:		returnval = PORTA.IN & ( 1<<PIN );
						break;
	#endif
	#if MAX_GPIO_PORTS > 1
			case 1:		returnval = PORTB.IN & ( 1<<PIN );
						break;
	#endif
	#if MAX_GPIO_PORTS > 2
			case 2:		returnval = PORTC.IN & ( 1<<PIN );
						break;
	#endif
	#if MAX_GPIO_PORTS > 3
			case 3:		returnval = PORTD.IN & ( 1<<PIN );
						break;
	#endif
	#if MAX_GPIO_PORTS > 4
			case 4:		returnval = PORTE.IN & ( 1<<PIN );
						break;
	#endif
	#if MAX_GPIO_PORTS > 5
			case 5:		returnval = PORTF.IN & ( 1<<PIN );
						break;
	#endif
#else
	#if MAX_GPIO_PORTS > 0
			case 0:		returnval = PINA & ( 1<<PIN );
						break;
	#endif
	#if MAX_GPIO_PORTS > 1
			case 1:		returnval = PINB & ( 1<<PIN );
						break;
	#endif
	#if MAX_GPIO_PORTS > 2
			case 2:		returnval = PINC & ( 1<<PIN );
						break;
	#endif
	#if MAX_GPIO_PORTS > 3
			case 3:		returnval = PIND & ( 1<<PIN );
						break;
	#endif
	#if MAX_GPIO_PORTS > 4
			case 4:		returnval = PINE & ( 1<<PIN );
						break;
	#endif
	#if MAX_GPIO_PORTS > 5
			case 5:		returnval = PINF & ( 1<<PIN );
						break;
	#endif
#endif
		default:	break;
	}
	if ( returnval != 0 )
		returnval = 1;
	return( returnval );
}

/**
 * \brief Aktiviert die internen Pullups.
 * \param	PIN		Nummer des Pin.
 * \retval	GPIO_OK oder GPIO_ERROR
 */
int GPIO_setPullUp( char PIN )
{
	char returnval = GPIO_ERROR;

	if( GPIO_getdirection( PIN ) == 0 )
	{
#if defined(__AVR_XMEGA__)
		char * port = (char *) &PORTA;
				
		PORT_t * Port = (PORT_t *)(port + ( PIN >> 3 ) * 0x20);
		PIN &= 0x07;

		*(&Port->PIN0CTRL + PIN ) &= ~0x38;
		*(&Port->PIN0CTRL + PIN ) |= 0x18;
		
#else
		GPIO_setPin( PIN );
#endif
		returnval = GPIO_OK;
	}
	return( returnval );
}


/**
 * \brief Deaktiviert die internen Pullups.
 * \param	PIN		Nummer des Pin.
 * \retval	GPIO_OK oder GPIO_ERROR
 */
int GPIO_clearPullUp( char PIN )
{
	char returnval = GPIO_ERROR;

	if( GPIO_getdirection( PIN ) == 0 )
	{
#if defined(__AVR_XMEGA__)
		char * port = (char *) &PORTA;

		PORT_t * Port = (PORT_t*)port + ( PIN >> 3 ) * 0x20;
		PIN &= 0x7;

		*(&Port->PIN0CTRL + PIN ) &= ~0x38;
#else
		GPIO_clearPin( PIN );
#endif
		returnval = GPIO_OK;
	}
	return( returnval );
}

/**
 * @}
 */
