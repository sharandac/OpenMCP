/*!\file timer1.c \brief Stellt den Timer1 im CTC mode bereit */
//***************************************************************************
//*            timer1.c
//*
//*  Mon Jul 31 21:46:47 2006
//*  Copyright  2006 Dirk Broßwick
//*  Email: sharandac@snafu.de
//****************************************************************************/
///	\ingroup Hardware
///	\defgroup timer1 Stellt den Timer1 bereit (timer1.c)
///	\code #include "timer1.h" \endcode
///	\par Uebersicht
///		Stellt den Timer1 im CTC mode bereit der in festgelegten intervallen einen
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

#include "timer1.h"

TIMER1_CALLBACK_FUNC TIMER1_CallbackFunc[ MAX_TIMER1_CALLBACKS ];
 
/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Initialisiert den Timer1 im CTC-Mode.
 * \param 	Hz		Die anzahl der aufrufe in der Sekunde.
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
void timer1_init( unsigned int Hz, unsigned int timedrift )
{
	unsigned char i;

	// Alle Callbackeinträge löschen
	for ( i = 0 ; i < MAX_TIMER1_CALLBACKS ; i++ )
		TIMER1_CallbackFunc[i] = NULL;

#if defined(__AVR_XMEGA__)
	
	TIMER1.CNT = 0x00;
   /*Timer-Top-Wert in diesem Fall führt der Timer etwa jede Sekunde 
	einen Interrupt durch*/
	TIMER1.PER = ( F_CPU / ( 8UL * Hz ) ); // Hz bei 32MHz
	/*1024 Vorteiler*/
	TIMER1.CTRLA = TC_CLKSEL_DIV8_gc ;
	/*Timer im Normalmodus*/
	TIMER1.CTRLB = 0x00;
	/*Interrupt konfigurieren*/
	TIMER1.INTCTRLA = 0x01;
	
#else
	
	// Timer0 einstellungen setzen
	TCCR1B = ( 1<<WGM12 ) | ( 0<<CS12 ) | ( 1<<CS11 ) | ( 0<<CS10 ); // CTC mode setzen, Prescaler 1024
	OCR1A  = ( F_CPU / ( 8 * Hz ) ) - timedrift ; // 100Hz bei 16MHz
	TIMSK1 = ( 1<<OCIE1A );  // Compare Match A Interupt freigegben

#endif
	
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Stoppt den Timer1.
 * \param	NONE
 */
/* -----------------------------------------------------------------------------------------------------------*/
void timer1_stop(void)
	{
#if defined(__AVR_XMEGA__)
	TIMER1.INTCTRLA = 0x0;
#else
	TIMSK1 &= ~( 1<<OCIE1A );  // Compare Match A Interupt sperren		
#endif
	}
			
/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Gibt den Timer1 frei.
 * \param	NONE
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
void timer1_free(void)
{
#if defined(__AVR_XMEGA__)
	TIMER1.INTCTRLA = 0x01;
#else
	TIMSK1 |= ( 1<<OCIE1A );  // Compare Match A Interupt freigeben
#endif
}


#if defined(__AVR_XMEGA__)
	ISR(TCC1_OVF_vect)
#else
	ISR( TIMER1_COMPA_vect )
#endif
{
	unsigned char i;
	for ( i = 0 ; i < MAX_TIMER1_CALLBACKS ; i++ ) if ( TIMER1_CallbackFunc[i] != NULL ) TIMER1_CallbackFunc[i]();
}
	
/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Hinterlegt eine Callbackfunktion für den Timer1.
 * \param	pFunc		Zeiger auf die Aufzurufende Funktion.
 * \return	ErrorCode	TRUE oder FALSE.
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
unsigned char timer1_RegisterCallbackFunction( TIMER1_CALLBACK_FUNC pFunc )
	{
		unsigned char i;
		
		for ( i = 0 ; i < MAX_TIMER1_CALLBACKS ; i++ ) 
		{
			if ( TIMER1_CallbackFunc[i] == pFunc )
				return TRUE;
		}
		
		for ( i = 0 ; i < MAX_TIMER1_CALLBACKS ; i++ )
		{
			if ( TIMER1_CallbackFunc[i] == NULL )
			{
				TIMER1_CallbackFunc[i] = pFunc;
				return TRUE;
			}
		}
		return FALSE;
	}

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Hinterlegt eine Callbackfunktion für den Timer1.
 * \param	pFunc		Zeiger auf die Aufzurufende Funktion.
 * \return	ErrorCode	TRUE oder FALSE.
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
unsigned char timer1_RemoveCallbackFunction( TIMER1_CALLBACK_FUNC pFunc )
	{
		unsigned char i;
		
		for ( i = 0 ; i < MAX_TIMER1_CALLBACKS ; i++ )
		{
			if ( TIMER1_CallbackFunc[i] == pFunc )
			{
				TIMER1_CallbackFunc[i] = NULL;
				return TRUE;
			}
		}
		return FALSE;
	}

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Wartet N Counterticks vom TCNT1. Ein Tick ist abhängig vom Prescaler. Bei 16MHz ist ein Tick 0.5µs lang.
 * \param   Counter		Anzahl der Ticks die gewartet werden soll.
 * \return	NONE		None
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
void timer1_wait( int counter )
{	
	int TickCounter = 0;
	char OldTick;

#if defined(__AVR_XMEGA__)
	OldTick = TIMER1.CNT;
	
	while( counter != TickCounter )
	{
		if ( OldTick != TIMER1.CNT )
		{
			OldTick = TIMER1.CNT;
			TickCounter++;
		}
	}
#else
	OldTick = TCNT1L;
	
	while( counter != TickCounter )
	{
		if ( OldTick != TCNT1L )
		{
			OldTick = TCNT1L;
			TickCounter++;
		}
	}
#endif

	return;
}

unsigned int timer1_getcounter( void )
{
#if defined(__AVR_XMEGA__)
	return( TIMER1.CNT );
#else
	return( TCNT1 );
#endif
}

unsigned int timer1_gettimerbase( void )
{
#if defined(__AVR_XMEGA__)
	return( TIMER1.PER );
#else
	return( OCR1A );
#endif
}
//@}
