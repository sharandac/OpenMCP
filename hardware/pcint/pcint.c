/***************************************************************************
 *            pcint.c
 *
 *  Wed Sep 30 11:29:22 2009
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

#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

/**
 * \ingroup Hardware
 * \addtogroup PCINT Interface um die PinChange Interrupts zu benutzen (pcint.c)
 *
 * Mit diesen Modul können die PinChange Interrupts genutzt werden.
 *
 * @{
 */

/**
 * \file
 *
 * \author Dirk Broßwick
 */

#include "config.h"

#if defined(PC_INT)

#include "pcint.h"

struct PCINT_STRUCT PCINT_table[ 4 ];

/**
 * \brief Hier wird der PCINT Initialisiert.
 * \param		NONE
 * \return		NONE
 */
void PCINT_init ( void )
{
	int i;

	char sreg_tmp;
	sreg_tmp = SREG;    /* Sichern */
	cli();

	for ( i = 0 ; i < 4 ; i++ )
		PCINT_table[ i ].FUNCTION = NULL;

	SREG = sreg_tmp;
}

/**
 * \brief Setzt für einen PC_INT die Callbackfunktion.
 * \param		PCINT		Nummer des PCINT.
 * \param		pFunc		Pointer auf die Callbackfunktion.
 * \return		0, alles Okay.
 */
int PCINT_set ( unsigned char PCINT, PCINT_CALLBACK_FUNC pFunc )
{
	char sreg_tmp;

	sreg_tmp = SREG;    /* Sichern */
	cli();

	// CallbackFunc eintragen
	PCINT_table[ PCINT ].FUNCTION = pFunc ;

	SREG = sreg_tmp;
	
	return( 0 );
}

/**
 * \brief Gibt den PCINT frei, so das der Interrupt ausgelöst wird.
 * \param		PCINT		Nummer des PCINT.
 * \return		0, alles Okay
 */
int PCINT_enablePCINT( unsigned char PCINT )
{
	PCICR |= ( 1<<PCINT );
	return( 0 );
}

/**
 * \brief Sperrt den PCINT, so das der Interrupt nicht mehr ausgelöst wird.
 * \param		PCINT		Nummer des PCINT.
 * \return		0, alles Okay
 */
int PCINT_disablePCINT( unsigned char PCINT )
{
	PCICR &= ~( 1<<PCINT );
	return( 0 );
}

/**
 * \brief Gibt einen einzelnen Pin für PCINT frei.
 * \param		PIN		Nummer des Pin der Freigegeben werden soll.
 * \param		PCINT	Nummer des PCINT interrupt auf den der Pin liegt.
 * \return		0, alles Okay
 */
int PCINT_enablePIN( unsigned char PIN, unsigned char PCINT )
{
	PCICR |= ( 1<<PCINT );

	switch( PCINT )
	{
		case 0:			PCMSK0 |= ( 1<<PIN );
						break;
		case 1:			PCMSK1 |= ( 1<<PIN );
						break;
		case 2:			PCMSK2 |= ( 1<<PIN );
						break;
		case 3:			PCMSK3 |= ( 1<<PIN );
						break;
	}

	return( 0 );
}

/**
 * \brief Sperrt einen einzelnen Pin für PCINT.
 * \param		PIN		Nummer des Pin der gersperrt werden soll.
 * \param		PCINT	Nummer des PCINT interrupt auf den der Pin liegt.
 * \return		0, alles Okay
 */
int PCINT_disablePIN( unsigned char PIN, unsigned char PCINT )
{
	switch( PCINT )
	{
		case 0:			PCMSK0 &= ~( 1<<PIN );
						break;
		case 1:			PCMSK1 &= ~( 1<<PIN );
						break;
		case 2:			PCMSK2 &= ~( 1<<PIN );
						break;
		case 3:			PCMSK3 &= ~( 1<<PIN );
						break;
	}

	return( 0 );
}


/**
 * \brief Hier wird der PCINT Initialisiert.
 * \param		NONE
 * \return		NONE
 */
ISR(PCINT0_vect)
{
	PCIFR |= ( 1<<0 );
	if ( PCINT_table[ 0 ].FUNCTION != NULL )
		PCINT_table[ 0 ].FUNCTION();
}

ISR(PCINT1_vect)
{
	PCIFR |= ( 1<<1 );
	if ( PCINT_table[ 1 ].FUNCTION != NULL )
		PCINT_table[ 1 ].FUNCTION();
}

ISR(PCINT2_vect)
{
	PCIFR |= ( 1<<2 );
	if ( PCINT_table[ 2 ].FUNCTION != NULL )
		PCINT_table[ 2 ].FUNCTION();
}

ISR(PCINT3_vect)
{
	PCIFR |= ( 1<<3 );
	if ( PCINT_table[ 3 ].FUNCTION != NULL )
		PCINT_table[ 3 ].FUNCTION();
}

/**
 * @}
 */

#endif
