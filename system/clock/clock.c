//***************************************************************************
//*            clock.c
//*
//*  Sat Jun  3 23:01:42 2006
//*  Copyright  2006  Dirk Broßwick
//*  Email: sharandac@snafu.de
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

/**
 * \ingroup system
 * \addtogroup CLOCK Die Clockfunktion für den Microcontroller als Zeitbasis (clock.c)
 *
 * \date	03-06-2008: Neuen Code hinzugefügt, geht jetzt schneller, die Callbacks und 
 *			Counter werden nur einmal durchsucht pro Tick.
 * \date	04-06-2008: Lastverteilung in den Callbacks eingebaut, geht jetzt wunderbar, es wird pro Tick
 *			nur eine Callback aufgerufen für Resolutions größer gleich Sekunde, die anderen werden
 *			als Executionbit markiert und in den folgenden Tick ausgeführt.
 * \date	05-01-2008: Die Uhrzeit kann jetzt mit Hilfe eines Struct geholt werden, sollte in zukunft benutzt
 *			werden da nicht mehr auf die Globalen Variablen zugegriffen werden muss.
 * \date	05-14-2008: Delayfunktion hinzugefügt.
 * \date	02-18-2010: Unixzeit decoding hinzugefügt.
 * \date	09-26-2010: Unixzeit encoding hinzugefügt.
 *
 * @{
 */

/**
 *
 * Die Clockfunktion für den Microcontroller als Zeitbasis (clock.c)
 *
 * \par Uebersicht
 *		Stellt funktionen bereit um eine genaue Zeit zu realisieren und funktionen
 *		um Zeitgesteuert eigene Funktionen die man hinterlegt aufzurufen
 * \author Dirk Broßwick
 *
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

#include "hardware/timer1/timer1.h"
#include "system/clock/clock.h"
	
struct CALLBACK Clock_CallBack_Table [ MAX_CLOCK_CALLBACKS ]; 

struct COUNTER Counter_Table [ MAX_CLOCK_COUNTDOWNTIMER ];

struct TIME time;
/*-----------------------------------------------------------------------------------------------------------*/
/*! \brief Initialisiert die System-Clock und registriert ihn auf Timer1 mit 1/100s als Callbackfunktion.
 */
/*------------------------------------------------------------------------------------------------------------*/
void CLOCK_init(void)
	{
		
		unsigned int i;
		
		// Alle Callbackeinträge löschen
		for ( i = 0 ; i < MAX_CLOCK_CALLBACKS ; i++ ) 
			{
				Clock_CallBack_Table[i].CallbackFunc = NULL;
				Clock_CallBack_Table[i].Resolution = NO_USE;
			}
			
		for ( i = 0 ; i < MAX_CLOCK_COUNTDOWNTIMER ; i++ ) 
			{
				Counter_Table[i].Counter = 0;
				Counter_Table[i].Resolution = NO_USE;
			}
			
		time.uptime=0;
		time.time=0;
		time.timezone=0;

		CLOCK_decode_time( &time );
		
		// Clocksource init
		timer1_init( 100 , 0);
		// clock_tick registrieren als Callbackfunktion
		timer1_RegisterCallbackFunction( CLOCK_tick );
		
	}
	
/*-----------------------------------------------------------------------------------------------------------*/
/*! \brief Die ISR der Clock, hier werden die Uhrzeit und Callbackeinträge abgearbeitet
 */
/*------------------------------------------------------------------------------------------------------------*/
void CLOCK_tick( void )
	{			
		unsigned char state;
		unsigned int i;
		time.ms++;
		state = MSECOUND;
		if ( time.ms == 100 )
		{
			time.ms = 0;
			time.ss++;
			time.uptime++;
			time.time++;
			state = SECOUND;
			if ( time.ss == 60 )
			{
				time.ss = 0;
				time.mm++;
				state = MINUTE;
				if ( time.mm == 60 )
				{
					time.mm = 0;
					time.hh++;
					state = HOUR;
					CLOCK_decode_time( &time );

					if ( time.hh == 24 )
					{
						time.hh = 0;
						state = DAY;
					}
				}
			}
		}
				
		// alle Callbacks durchgehen und alles was größer Resolution MSECOUND als Execution markieren für spätere ausführung
		for ( i = 0; i < MAX_CLOCK_CALLBACKS ; i++ )
		{
			if ( Clock_CallBack_Table[i].Resolution <= state )
			{
				if ( state >= SECOUND )
					Clock_CallBack_Table[i].Execution = Executionbit;
				else
				{
					Clock_CallBack_Table[i].Execution = NonExecutionbit;
					Clock_CallBack_Table[i].CallbackFunc();
				}
			}
		}

		// Hier werden die als Execution markierten Callbacks ausgeführt, je durchlauf einer
		for ( i = 0; i < MAX_CLOCK_CALLBACKS ; i++ )
		{
			if ( Clock_CallBack_Table[i].Execution == Executionbit )
			{
				Clock_CallBack_Table[i].Execution = NonExecutionbit;
				Clock_CallBack_Table[i].CallbackFunc();
				break;
			}
		}
		
		for ( i = 0 ; i < MAX_CLOCK_COUNTDOWNTIMER ; i++ )
		{
			if ( Counter_Table[i].Resolution <= state && Counter_Table[i].Counter != 0 )
			{
				Counter_Table[i].Counter--;
			}
		}
	}

/*-----------------------------------------------------------------------------------------------------------*/
/*!\brief Registriert einen CountdownTimer.
 * \param	pFunc		Zeiger auf die Funktion die aufgerufen werden soll
 * \param	Resolution	Gibt die Auflösung an mit der die Callbackfunktion aufgerufen werden soll
 *						mögliche Paramter: MSECOUND,SECOUND,MINUTE,HOUR und DAY
 * \return	TRUE oder FALSE
 */
/*------------------------------------------------------------------------------------------------------------*/
char CLOCK_RegisterCallbackFunction( CLOCK_CALLBACK_FUNC pFunc, unsigned char Resolution )
	{
		unsigned char i;
		
		timer1_stop();

		for ( i = 0 ; i < MAX_CLOCK_CALLBACKS ; i++ ) 
		{
			if ( Clock_CallBack_Table[i].CallbackFunc == pFunc )
			{
				timer1_free();
				return CLOCK_OK;
			}
		}
		
		for ( i = 0 ; i < MAX_CLOCK_CALLBACKS ; i++ )
		{
			if ( Clock_CallBack_Table[i].CallbackFunc == NULL )
			{
				// zuerst Pointer zu Funktion setzen und Resolution setzen zum scharf machen
				// dies ist nötig da der zugrif auf Resolution atomar ist
				Clock_CallBack_Table[i].CallbackFunc = pFunc;
				Clock_CallBack_Table[i].Resolution = Resolution;
				timer1_free();
				return CLOCK_OK;
			}
		}
		timer1_free();
		return CLOCK_FAILED;
	}
	
/*-----------------------------------------------------------------------------------------------------------*/
/*!\brief Löscht einen registrierte Callbackfunktion.
 * \param	pFunc		Zeiger auf die Funktion die aufgerufen werden soll
 * \return	TRUE oder FALSE
 */
/*------------------------------------------------------------------------------------------------------------*/
char CLOCK_RemoveCallbackFunction( CLOCK_CALLBACK_FUNC pFunc )
	{
		unsigned char counter;
		
		timer1_stop();

		for ( counter = 0 ; counter < MAX_CLOCK_CALLBACKS ; counter++ )
		{
			if ( Clock_CallBack_Table[ counter ].CallbackFunc == pFunc )
			{
				// zuerst Resolution löschen zum sperren und dann Pointer löschen
				// dies ist nötig da der zugrif auf Resolution atomar ist
				Clock_CallBack_Table[ counter ].Resolution = NO_USE;
				Clock_CallBack_Table[ counter ].CallbackFunc = NULL;
				timer1_free();
				return CLOCK_OK;
			}
		}
		timer1_free();
		return CLOCK_FAILED;
	}
	
/*-----------------------------------------------------------------------------------------------------------*/
/*!\brief Registriert einen Counterdown-zähler.
 * \code
 * unsigned char timer;
 *
 * #define TIMEOUT 1000
 *		
 * timer = CLOCK_RegisterCoundowntimer();
 * CLOCK_SetCountdownTimer( timer , TIMEOUT, MSEC );
 *
 * while ( 1 )
 * {
 *		if ( Get_Key() && ( CLOCK_GetCountdownTimer( timer ) != 0 ) )
 *		{
 *			CLOCK_ReleaseCountdownTimer( timer );
 *			return( Okay );
 *		}
 *		if ( CLOCK_GetCountdownTimer( timer ) == 0 ) 
 *		{
 *			CLOCK_ReleaseCountdownTimer( timer );
 *			return( Error );
 *		}
 * }
 * \endcode
 * \return	Die Nummer des Countdowntimer oder FALSE
 */
/*------------------------------------------------------------------------------------------------------------*/
int CLOCK_RegisterCountdowntimer( void )
	{
		int counter ;
		
		timer1_stop();
		
		for ( counter = 0 ; counter < MAX_CLOCK_COUNTDOWNTIMER ; counter++ )
		{
			if ( Counter_Table[ counter ].Resolution == NO_USE )
			{
				Counter_Table[ counter ].Counter = 0;
				timer1_free();
				return ( counter ) ;
			}
		}

		timer1_free();

		return( CLOCK_FAILED );		
	}

/*-----------------------------------------------------------------------------------------------------------*/
/*!\brief Set einen startwert für einen Counterdown-zähler.
 * \param	counter		Die Counternummer der benutzt werden soll, dieser sollte vorher mit CLOCK_RegisterCoundowntimer
 *						ermittelt worden sein.
 * \param	value		Der Wert ab den gegen 0 gezählt werden soll.
 * \param	Resolution	Gibt die Auflösung an mit der die Callbackfunktion aufgerufen werden soll
 *						mögliche Paramter: MSECOUND,SECOUND,MINUTE,HOUR und DAY
 * \return	Die Nummer des Countdowntimer oder FALSE
 */
/*------------------------------------------------------------------------------------------------------------*/
void CLOCK_SetCountdownTimer( int counter, unsigned int value, unsigned char Resolution )
	{
		timer1_stop();

		Counter_Table[ counter ].Counter = value;
		Counter_Table[ counter ].Resolution = Resolution;

		timer1_free();
	}
	
/*-----------------------------------------------------------------------------------------------------------*/
/*!\brief Holt den aktuellen Zählerstand für einen Counterdown-zähler.
 * \param	counter		Die Counternummer der benutzt werden soll, dieser sollte vorher mit CLOCK_RegisterCoundowntimer
 *						ermittelt worden sein.
 * \return	Value		Der Zählerstand
 */
/*------------------------------------------------------------------------------------------------------------*/
unsigned int CLOCK_GetCountdownTimer( int counter )
	{
		unsigned int value;

		timer1_stop();

		value = Counter_Table[ counter ].Counter;

		timer1_free();

		return ( value );
	}

/*-----------------------------------------------------------------------------------------------------------*/
/*!\brief Gibt einen Counter wieder zur Benutzung frei.
 * \param	counter		Die Counternummer die benutzt werden soll, dieser sollte vorher mit CLOCK_RegisterCoundowntimer
 *						ermittelt worden sein.
 * \return	NONE
 */
/*------------------------------------------------------------------------------------------------------------*/	
void CLOCK_ReleaseCountdownTimer( int counter )
	{
		Counter_Table[ counter ].Resolution = NO_USE ;
	}

/*-----------------------------------------------------------------------------------------------------------*/
/*!\brief Holt die Aktuelle Uhrzeit und speichert sie in der übergebenen Struktur.
 * \param	Timestruct		Pointer auf die Struct in der die Uhrzeit abgelegt werden soll.
 * \return	NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
int CLOCK_GetTime( struct TIME * Timestruct )
	{
		timer1_stop();

		memcpy( Timestruct, &time, sizeof(struct TIME) );
	
		timer1_free();
		
		return( 0 );
	}

/*-----------------------------------------------------------------------------------------------------------*/
/*!\brief Setzt die Aktuelle Uhrzeit.
 * \param	Timestruct		Pointer auf die Struct in der die Uhrzeit abgelegt ist.
 * \return	NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
int CLOCK_SetTime( struct TIME * Timestruct )
	{
		timer1_stop();

		memcpy( &time, Timestruct, sizeof(struct TIME) );
		
		timer1_free();
		
		return( 0 );
	}

/*-----------------------------------------------------------------------------------------------------------*/
/**
 * \brief Delay was sonst.
 * \param	ms			Warte einfach eine Zeit in ms.
 * \return	NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
void CLOCK_delay(unsigned int ms) 
{
	unsigned char counter;
		
	counter = CLOCK_RegisterCountdowntimer();
	CLOCK_SetCountdownTimer( counter , ms/10 , MSECOUND );

	while ( 1 )
		{
			if ( CLOCK_GetCountdownTimer( counter ) == 0 ) 
			{
				CLOCK_ReleaseCountdownTimer( counter );
				return;
			}
		}	
}

const char wday_str[] PROGMEM = "So\0Mo\0Di\0Mi\0Do\0Fr\0Sa";

/**
 * \brief Gibt fuer den Aktuellen Monat die Anzahl der Tage aus.
 * \param	isleapyear		Ist ein Schaltjahr?
 * \param	month			Der Monat der abgefragt werden soll.
 * \return	DAYS			Anzahl der Tage fuer den Monat.
 */
unsigned char CLOCK_monthlen(unsigned char isleapyear,unsigned char month)
{
    if (month == 1)
    {
        return (28+isleapyear);
    }

    if (month > 6)
    {
        month--;
    }

    if (month %2 == 1)
    {
        return (30);
    }

    return (31);
}

/*------------------------------------------------------------------------------------------------------------*/
/**
 * \brief Wandelt Unixzeit in normale Zeit
 * \param	Timestruct	Pointer auf eine struct TIME und wandelt dort Unixzeit in Zeit, Datum u.s.w.
 * \return	NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
void CLOCK_decode_time( struct TIME * Timestruct )
{
	unsigned long dayclock;
	unsigned int  dayno;
	unsigned char summertime;
	unsigned long UTC;

	UTC = Timestruct->time;

	Timestruct->time += Timestruct->timezone * 3600l;

	Timestruct->YY = EPOCH_YR;
	dayclock = ( Timestruct->time ) % SECS_DAY;
	dayno    = ( Timestruct->time ) / SECS_DAY;

	Timestruct->ss = dayclock % 60UL;
	Timestruct->mm = (dayclock % 3600UL) / 60;
	Timestruct->hh   = dayclock / 3600UL;
	Timestruct->WW   = ( dayno + 1 ) % 7;

	while (dayno >= YEARSIZE( Timestruct->YY ) )
	{
		dayno -= YEARSIZE( Timestruct->YY );
		Timestruct->YY++;
	}

	Timestruct->MM = 0;
	while ( dayno >= CLOCK_monthlen( LEAPYEAR( Timestruct->YY ) , Timestruct->MM ) )
	{
		dayno -= CLOCK_monthlen(LEAPYEAR( Timestruct->YY ) , Timestruct->MM );
		Timestruct->MM++;
	}
	Timestruct->MM++;
	Timestruct->DD  = dayno+1;

	// Summertime
	summertime = 1;
	if ( Timestruct->MM < 3 || Timestruct->MM > 10)     // month 1, 2, 11, 12
	{
		summertime = 0;                          // -> Winter
	}

	if ( ( Timestruct->DD - Timestruct->WW >= 25 ) && ( Timestruct->WW || Timestruct->hh >= 2) )
	{                              // after last Sunday 2:00
		if ( Timestruct->MM == 10 )        // October -> Winter
		{
			summertime = 0;
		}
	}
	else
	{                              // before last Sunday 2:00
		if ( Timestruct->MM == 3)        // March -> Winter
		{
			summertime = 0;
		}
	}

	if (summertime)
	{
		Timestruct->hh++;              // add one hour
		if ( Timestruct->hh == 24)
		{                        // next day
			Timestruct->hh = 0;
			Timestruct->WW++;            // next weekday
			if ( Timestruct->WW == 7 )
			{
				Timestruct->WW = 0;
			}
			if ( Timestruct->DD == CLOCK_monthlen( LEAPYEAR( Timestruct->YY ) , Timestruct->MM ) )
			{                // next month
				Timestruct->DD = 0;
				Timestruct->MM++;
			}
			Timestruct->DD++;
		}
	}

	Timestruct->time = UTC;
}

/*------------------------------------------------------------------------------------------------------------*/
/**
 * \brief Wandelt normale Zeit in Unixzeit
 * \param	Timestruct	Pointer auf eine struct TIME und wandelt dort die Zeit, Datum u.s.w. in Unixzeit
 * \return	NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
void CLOCK_encode_time( struct TIME * Timestruct )
{
	unsigned int y;
	unsigned char m;

	Timestruct->time = 0;

	// klapper mal die Jahre ab
	for( y = EPOCH_YR ; y < Timestruct->YY; y ++ )
	{
		Timestruct->time += SECS_DAY * YEARSIZE(y);
	}

	// Monate abklappern
	for( m = 1 ; m < Timestruct->MM ; m++ )
	{
		Timestruct->time += CLOCK_monthlen( LEAPYEAR( Timestruct->YY ) , m ) * SECS_DAY;
	}
	
	// Tage abklappern
	Timestruct->time += ( Timestruct->DD - 1 ) * SECS_DAY;
	// Uhrzeit abklappern
	Timestruct->time += ( Timestruct->ss + (Timestruct->mm * 60UL ) + ( Timestruct->hh * 60UL * 60UL ) );

	Timestruct->time -= Timestruct->timezone * 3600l;
}

/**
 * @}
 */

