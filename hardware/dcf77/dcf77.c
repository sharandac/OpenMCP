/***************************************************************************
 *            dcf77.c
 *
 *  Tue Jul 27 03:15:55 2010
 *  Copyright  2010  Dirk Broßwick
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
#include <stdio.h>
#include <avr/pgmspace.h>
#include <util/parity.h> 

#include "config.h"

/**
 * \ingroup Hardware
 * \addtogroup DCF77 DCF77-Empfangsmodul (dcf77.c)
 *
 * @{
 */

/**
 * \file
 * Stellt die Systemuhr nach DCF77. Als Signal für ein beliebiger Eingangspin gebraucht.
 * Das Eingangsisgnal kann beliebige polarität haben.
 *
 * \author Dirk Broßwick
 */

#if defined(DCF77)

#include "dcf77.h"
#include "system/math/math.h"
#include "system/clock/clock.h"
#if defined(LED)
	#include "hardware/led/led_core.h"
#endif
#if defined(DEBUG)
	#include "system/stdout/stdout.h"
#endif

// #define DEBUG
// #define SPIKEFILTER


volatile char DCF77_state = DCF77_STATE_WAITFORSYNC;

/**
 * \brief Initialisiert das DCF77 Modul
 */
void DCF77_init( void )
{
	// Port Auf eingfang schalten
	DCF77_DDR &= ~( 1<< DCF77_PIN );
	// Callback eintragen
	CLOCK_RegisterCallbackFunction( DCF77_work, MSECOUND );
}

/**
 * \brief DCF77 Signalauswertung
 */
void DCF77_work( void )
{
	static char Timeswitch=0, mm=0, hh=0, DD=0, WW=0, MM=0, YY=0;
	static int counter = 0;
	static char Parity, Databit=0, *Data_p = NULL , bitpos=0, state ;
	char Port;

#if defined(DEBUG)
	// Struktur anlegen für STDOUT
	struct STDOUT oldstream;

	// alte Struktur sichern
	STDOUT_save( &oldstream );

	STDOUT_set( RS232, 0);
#endif

#if defined(SPIKEFILTER)
	static int DCF77_bit = 0;
	
	Port = DCF77_PORT & ( 1<<DCF77_PIN );
	
	if ( ( Port != 0 ) && ( DCF77_bit <= 5 ) )
		DCF77_bit++;
	else if( ( Port == 0 ) && ( DCF77_bit > 0) )
		DCF77_bit--;

	if ( DCF77_bit > 2 )
		Port = 1;
	else
		Port = 0;	
#else	
	Port = DCF77_PORT & ( 1<<DCF77_PIN );
#endif

	// Wenn eine Flanke erkannt worden ist mal schaun ...
	if ( state ^ Port )
	{	
		// ab den 16 Bit - SUMMERTIME, MEZ, MESZ, SWITCHSECOUND
		if ( bitpos == 16 )
		{
			Databit = 0;
			Data_p = &Timeswitch;
		}
		// ab den 21 Bit - Minute
		else if ( bitpos == 21 )
		{
			Parity = 0;
			Databit = 0;
			Data_p = &mm;
		}
		// ab den 29 Bit - Stunde
		else if(bitpos == 29 )
		{
			* Data_p = bcd2bin( * Data_p );
			Parity = 0;
			Databit = 0;
			Data_p = &hh;
		}
		// ab den 36 Bit - Datum / Tag
		else if(bitpos == 36 )
		{
			* Data_p = bcd2bin( * Data_p );
			Parity = 0;
			Databit = 0;
			Data_p = &DD;
		}
		// ab den 42 Bit - Wochentag
		else if(bitpos == 42 )
		{
			* Data_p = bcd2bin( * Data_p );
			Databit = 0;
			Data_p = &WW;
		}
		// ab den 45 Bit - Monat
		else if(bitpos == 45 )
		{
			* Data_p = bcd2bin( * Data_p );
			Databit = 0;
			Data_p = &MM;
		}
		// ab den 50 Bit - Jahr
		else if(bitpos == 50 )
		{
			* Data_p = bcd2bin( * Data_p );
			Databit = 0;
			Data_p = &YY;
		}
		else if(bitpos == 58 )
		{
			* Data_p = bcd2bin( * Data_p );
		}


		// Impulszeiten abfragen und zuordnen
#if defined(LED) && defined(AVRNETIO)
		LED_toggle(1);
#endif
		
		// 0 Impuls
		if ( counter < ( DCF77_IMPULSE_0 + DCF77_IMPULSE_TOLERANZ ) && counter > ( DCF77_IMPULSE_0 - DCF77_IMPULSE_TOLERANZ ) )
		{
			// Ist gerade das Parity Bit übertragen worden?
			if ( bitpos == 28 || bitpos == 35 || bitpos == 58 )
			{
#if defined(DEBUG)
				printf_P( PSTR("P(%1d)"), Parity & 0x01 );
#endif
				// Parity kontrollieren
				if ( ( Parity & 0x01 ) != 0 )
				{
					DCF77_state = DCF77_STATE_PARITYERROR;
#if defined(DEBUG)
					printf_P( PSTR("Parity Error.%02x"), Parity );
#endif
				}
			}
			// Wenn nicht das bit in den Datenblock schreiben
			else
			{
				*Data_p &= ~( 1 << Databit );
				Databit++;
			}
#if defined(DEBUG)
			printf_P( PSTR("0") );
#endif
			bitpos++;
		}
		// 1 Impuls
		else if ( counter < ( DCF77_IMPULSE_1 + DCF77_IMPULSE_TOLERANZ ) && counter > ( DCF77_IMPULSE_1 - DCF77_IMPULSE_TOLERANZ ) )
		{	
			// Ist gerade das Parity Bit übertragen worden?
			if ( bitpos == 28 || bitpos == 35 || bitpos == 58 )
			{
#if defined(DEBUG)
				printf_P( PSTR("P(%1d)"), Parity & 0x01 );
#endif
				if ( ( Parity & 0x01 ) != 1 )
				{
					DCF77_state = DCF77_STATE_PARITYERROR;
#if defined(DEBUG)
					printf_P( PSTR("Parity Error.%02x"), Parity );
#endif
				}
			}
			// Wenn nicht das bit in den Datenblock schreiben
			else
			{
				*Data_p |= ( 1 << Databit );
				Databit++;
			}
#if defined(DEBUG)
			printf_P( PSTR("1") );
#endif
			Parity++;
			bitpos++;
		}
		// Pause zwischen den Datenbits
		else if ( counter < ( DCF77_IMPULSE_PAUSE + DCF77_IMPULSE_TOLERANZ * 2 ) && counter > ( DCF77_IMPULSE_PAUSE - DCF77_IMPULSE_TOLERANZ * 2 ) )
		{
		}
		// Minitenimpuls
		else if ( counter < ( DCF77_IMPULSE_START + DCF77_IMPULSE_TOLERANZ * 2 ) && counter > ( DCF77_IMPULSE_START - DCF77_IMPULSE_TOLERANZ * 2 ) )
		{
			// War das letzte DCf77-Frame komplett okay ?
			if ( DCF77_state == DCF77_STATE_RECEIVE && bitpos == 59 )
			{
				// Uhrzeit holen
				struct TIME time;
				CLOCK_GetTime( &time );

				// DCF77 Uhrzeit eintragen
				time.ms=0;
				time.ss=0;
				time.mm=mm;
				time.hh=hh;
				time.DD=DD;
				time.WW=WW % 7;
				time.MM=MM;
				time.YY=2000 + YY;

				// DCF77 Uhrzeit nach UTC umrechnen
				CLOCK_encode_time( &time );

				// Wenn Sommerzeit dann mal 1h abziehen
				if ( Timeswitch & 0x02 )
					time.time -= 3600l;

				// Und neue Uhrzeit setzen
				CLOCK_SetTime( &time );
				
#if defined(DEBUG)
				printf_P( PSTR(" %02d:%02d %02d/%02d/%d\r\nS"), time.hh,time.mm,time.DD,time.MM,time.YY );
#endif
			}
			else
			{
#if defined(DEBUG)
				printf_P( PSTR("error\r\nS"));
#endif
			}

			// Alles in den Ausgangszustand bringen
			DCF77_state = DCF77_STATE_RECEIVE;
			bitpos=0;			
			hh=0;
			mm=0;
			DD=0;
			WW=0;
			MM=0;
			YY=0;
		}
		else
		{
#if defined(DEBUG)
			printf_P( PSTR("E"));
#endif
			DCF77_state = DCF77_STATE_FRAMEERROR;
		}
		// Flankencounter neu starten
		counter = 0;
	}

	// Flankencounter um 10ms erhöhen
	counter += 10;
	// Portzustand speichern für nächsten durchgang um die Flanke zu erkennen
	state = Port;	

#if defined(DEBUG)
	// alte Struktur sichern
	STDOUT_restore( &oldstream );
#endif

}
#endif

/**
 * @}
 */
