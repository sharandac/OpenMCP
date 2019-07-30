/*!\file timer0.c \brief Stellt den Timer0 im CTC mode bereit */
//***************************************************************************
//*            timer0.c
//*
//*  Mon Jul 31 21:46:47 2006
//*  Copyright  2006 Dirk Broßwick
//*  Email: sharandac@snafu.de
//****************************************************************************/
///	\ingroup Hardware
///	\defgroup timer0 Stellt den Timer0 bereit (timer0.c)
///	\code #include "timer0.h" \endcode
///	\par Uebersicht
///		Stellt den Timer0 im CTC mode bereit der in festgelegten intervallen einen
///		Interrupt auslöst.
//****************************************************************************/
//@{
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
 
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stddef.h>
#include "timer0.h"

TIMER0_CALLBACK_FUNC TIMER0_CallbackFunc[ MAX_TIMER0_CALLBACKS ];

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Initialisiert den Timer0 im CTC-Mode.
 * \param 	Hz		Die anzahl der aufrufe in der Sekunde.
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
void timer0_init( int Hz )
{
		unsigned char i;
		
		// Alle Callbackeinträge löschen
		for ( i = 0 ; i < MAX_TIMER0_CALLBACKS ; i++ )
		{
			TIMER0_CallbackFunc[i] = NULL;
		}
#if defined(__AVR_XMEGA__)

	TIMER0.CNT = 0x00;
	/*Timer-Top-Wert in diesem Fall führt der Timer etwa jede Sekunde 
	einen Interrupt durch*/
	TIMER0.PER = ( F_CPU / ( 8l * Hz ) ); // Hz bei 32MHz
	/*8 Vorteiler*/
	TIMER0.CTRLA = TC_CLKSEL_DIV8_gc ;
	/*Timer im Normalmodus*/
	TIMER0.CTRLB = 0x00;
	/*Interrupt konfigurieren*/
	TIMER0.INTCTRLA = 0x01;
	
#else
	#if defined(__AVR_ATmega2561__) || defined(__AVR_ATmega644__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega1284P__) 
			// Timer0 einstellungen setzen
			TCCR0A = ( 1<<WGM01 ); // CTC mode setzen
			OCR0A  = ( F_CPU / ( 256UL * Hz ) ); // Hz bei 20MHz
			TCCR0B = ( 1<<CS02 ) | ( 0<<CS01 ) | ( 0<<CS00 ); // Prescaler 256
			TIMSK0 = ( 1<<OCIE0A );  // Compare Match A Interupt freigegben
	#elif defined(__AVR_AT90CAN128__)
			// Timer0 einstellungen setzen
			TCCR0A = ( 1<<WGM01 ) | ( 1<<CS02 ) | ( 0<<CS01 ) | ( 0<<CS00 ); // Prescaler 256, CTC mode setzen
			OCR0A  = ( F_CPU / ( 256 * Hz ) ); // Hz bei 20MHz
			TIMSK0 = ( 1<<OCIE0A );  // Compare Match A Interupt freigegben
	#endif
#endif
	
}
	
/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Stoppt den Timer0.
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
void timer0_stop(void)
	{
#if defined(__AVR_XMEGA__)
		TIMER0.INTCTRLA = 0x0;
#else
		TIMSK0 &= ~( 1<<OCIE0A );  // Compare Match A Interupt sperren
#endif
	}

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Gibt den Timer0 frei.
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
void timer0_free(void)
	{
#if defined(__AVR_XMEGA__)
		TIMER0.INTCTRLA = 0x01;
#else
		TIMSK0 |= ( 1<<OCIE0A );  // Compare Match A Interupt sperren		
#endif
	}

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Gibt den Timer0 frei.
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
#if defined(__AVR_XMEGA__)
	ISR(TCC0_OVF_vect)
#else
	#if defined(__AVR_ATmega2561__) || defined(__AVR_ATmega644__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega1284P__) 
		ISR( TIMER0_COMPA_vect )
	#elif defined(__AVR_AT90CAN128__)
		ISR( TIMER0_COMP_vect )
	#endif
#endif
{
	unsigned char i;
	for ( i = 0 ; i < MAX_TIMER0_CALLBACKS ; i++ )
	{
		if ( TIMER0_CallbackFunc[i] != NULL )
		{
			TIMER0_CallbackFunc[i]();
		}
	}
}
	
/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Hinterlegt eine Callbackfunktion für den Timer0.
 * \param	pFunc		Zeiger auf die Aufzurufende Funktion.
 * \return	ErrorCode	TRUE oder FALSE.
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
unsigned char timer0_RegisterCallbackFunction( TIMER0_CALLBACK_FUNC pFunc )
	{
		unsigned char i;
		
		for ( i = 0 ; i < MAX_TIMER0_CALLBACKS ; i++ ) 
		{
			if ( TIMER0_CallbackFunc[i] == pFunc )
				return TRUE;
		}
		
		for ( i = 0 ; i < MAX_TIMER0_CALLBACKS ; i++ )
		{
			if ( TIMER0_CallbackFunc[i] == NULL )
			{
				TIMER0_CallbackFunc[i] = pFunc;
				return TRUE;
			}
		}
		return FALSE;
	}

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Löscht die Callbackfunktion für den Timer0.
 * \param	pFunc		Zeiger auf die Aufzurufende Funktion.
 * \return	ErrorCode	TRUE oder FALSE.
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
unsigned char timer0_RemoveCallbackFunction( TIMER0_CALLBACK_FUNC pFunc )
	{
		unsigned char i;
		
		for ( i = 0 ; i < MAX_TIMER0_CALLBACKS ; i++ )
		{
			if ( TIMER0_CallbackFunc[i] == pFunc )
			{
				TIMER0_CallbackFunc[i] = NULL;
				return TRUE;
			}
		}
		return FALSE;
	}
//@}
