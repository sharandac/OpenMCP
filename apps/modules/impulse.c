/***************************************************************************
 *            impulse.c
 *
 *  Tue Nov 17 18:31:24 2009
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "config.h"

/**
 * \ingroup modules
 * \addtogroup Impulslogger Impulsloggermodul (impulse.c)
 *
 * Dieses Modul loggt die die Impulse an einem Eingang
 * und speichert sie in einer nanoDB auf SD-Card.
 *
 * @{
 */

/**
 *
 * \author Dirk Broßwick
 *
 */

#if defined(IMPULSCOUNTER)

#include "impulse.h"
#include "apps/httpd/cgibin/cgi-bin.h" 
#include "apps/httpd/httpd2.h" 
#include "apps/httpd/httpd2_pharse.h" 
#include "system/thread/thread.h"
#include "system/clock/clock.h"
#include "system/config/eeconfig.h"
#include "system/nano_DB/highchart.h"

// der PinChange Treiber
#if defined(PC_INT)
	#include "hardware/pcint/pcint.h"
#else
	#error "Bitte PC_INT in der config.h aktivieren"
#endif

const char IMPULS_PRESCALER_P[] PROGMEM = { "IMPULS_PRESCALER" };
const char IMPULS_COUNTER_P[] PROGMEM = { "IMPULS_COUNTER" };
const char IMPULS_AUTOSAVE_P[] PROGMEM = { "IMPULS_AUTOSAVE" };
const char IMPULS_UNIT_P[] PROGMEM = { "IMPULS_UNIT" };
const char AUTOSAVE_P[] PROGMEM = { "AUTOSAVE" };
const char UNIT_P[] PROGMEM = { "UNIT" };
const char PRESCALER_P[] PROGMEM = { "PRESCALER" };
const char impulslogfilename_P[] PROGMEM = { "impuls.log" };

static char lastmm;
static long IMPULS_COUNTER;
static long OLDIMPULS_COUNTER;

/**
 * \brief Initialisiert das Impulsloggermodul und Registriert das CGI-Interface.
 * \return	void
 */
void IMPULS_init( void )
{
	char String[16];
	char sreg_tmp;

	lastmm = 0;
	
	IMPULS_setCounter( 0 );

	if ( readConfig_P( IMPULS_COUNTER_P, String ) != -1 )
		IMPULS_setCounter( atol( String ) );

	OLDIMPULS_COUNTER = IMPULS_getCounter();
	
	sreg_tmp = SREG;	/* Sichern */
	cli();

	PCINT_set ( CounterPort, IMPULS_Interrupt );	
	PCINT_enablePIN( CounterPin, CounterPort );
	PCINT_enablePCINT( CounterPort );
	
	SREG = sreg_tmp;

#if defined(HTTPSERVER)
	cgi_RegisterCGI( cgi_impuls, PSTR("impuls.cgi") );
	cgi_RegisterCGI( IMPULS_logger_cgi, PSTR("impulslogger.cgi"));
#endif
	THREAD_RegisterThread( IMPULS_thread, PSTR("Impulse Logger"));

	IMPULS_thread();
}

/**
 * \brief Der Impulslogger-Threads, wird zyklisch aufgerufen.
 * \return	void
 */
void IMPULS_thread( void )
{
	int retval = LOGGER_ERROR;
	char FILENAME[32];
	char String[16];
	static char logrun=0;
	
	struct IMPULS_JSON impuls_json;
	impuls_json.logger_entry.DataSize = sizeof( impuls_json.impuls );
	
	// Zeit holen
	struct TIME nowtime;
	CLOCK_GetTime ( &nowtime );

	if ( ( nowtime.mm % 10 == 0 ) && ( lastmm != nowtime.mm ) )
	{
		logrun = 0;
		lastmm = nowtime.mm ;
	}

	if ( logrun == 0 )
	{
		// Impulse alle 30 min im EEprom sichern
		if ( readConfig_P( IMPULS_AUTOSAVE_P, String ) != -1 )
		{
			if ( !strcmp_P( String, PSTR("on") ) && ( nowtime.mm % 30 == 0 ) )
			{
				IMPULS_saveAll();
			}
		}
		// erzeuge mal einen Dummydatensatz
		IMPULS_clean( &impuls_json );
		// Dateinamen erzeigen zum Sensor
		sprintf_P( FILENAME, impulslogfilename_P );
		// Versuche letzten Datensatz zu holen
		retval = LOGGER_getlastDBEntry ( &impuls_json.logger_entry , FILENAME );
		// Wenn letzter Eintrag nicht lesbar, neue NanoDB erzeugen
		if ( retval == LOGGER_ERROR )
		{
			// Neue NanoDB erzeugen
			retval = LOGGER_addDBentry( &impuls_json.logger_entry , FILENAME );
		}
		// Datenbankeintrag erzeugt oder gelesen ?
		if ( retval == LOGGER_OK )
		{
			// letzer Eintrag von heute?
			if ( ! ( impuls_json.logger_entry.time.YY == nowtime.YY && impuls_json.logger_entry.time.MM == nowtime.MM && impuls_json.logger_entry.time.DD == nowtime.DD ) )
			{
				// Neue NanoDB erzeugen
				IMPULS_clean( &impuls_json );
				retval = LOGGER_addDBentry( &impuls_json.logger_entry , FILENAME );
			}
		}
		// alles Ok bis hier hin ?
		if ( retval == LOGGER_OK )
		{
			impuls_json.impuls[ ( nowtime.hh * 6 ) + ( nowtime.mm / 10 ) ] = (int) ( IMPULS_getCounter() - OLDIMPULS_COUNTER );
			OLDIMPULS_COUNTER = IMPULS_getCounter();
			// und sichern in Datenbank
			LOGGER_writeDBEntry( &impuls_json.logger_entry, FILENAME );
		}
		logrun = 1;
	}
}

/**
 * \brief Holt die Anzahl der Impulse
 * \return	long	Anzahl der Impulse.
 */
long IMPULS_getCounter( void )
{
	long counter_temp;

	char temp = SREG;
	cli();
	
	counter_temp = IMPULS_COUNTER;

	SREG = temp;
	
	return( counter_temp );
}

/**
 * \brief Setzt die Anzahl der Impulse
 * \param	long	Anzahl der Impulse.
 */
void IMPULS_setCounter( long Impulse )
{
	char temp = SREG;
	cli();
	
	IMPULS_COUNTER = Impulse;
	
	SREG = temp;
}

/**
 * \brief Sichert den aktuellen Impulscounter im Configspeicher.
 */
void IMPULS_saveAll( void )
{
	char String[16];

	char temp = SREG;
	cli();
	
	ltoa( IMPULS_COUNTER , String, 10 );
	
	SREG = temp;

	changeConfig_P( IMPULS_COUNTER_P, String );
}

/**
 * \brief Setzt den Prescaler fuer den Impulscounter.
 * \param	long	Prescaler.
 */
void IMPULS_setPrescaler( long Prescaler )
{
	char String[16];

	ltoa( Prescaler, String, 10 );
	changeConfig_P( IMPULS_PRESCALER_P, String );
	
}

/**
 * \brief Holt den Prescaler fuer den Impulscounter.
 * \retval	long	Prescaler.
 */
long IMPULS_getPrescaler( void )
{
	long temp;
	char String[16];

	temp = 1;
	
	if ( readConfig_P( IMPULS_PRESCALER_P, String ) != -1 )
		temp = atol( String );

	return( temp );
}

/**
 * \brief Setzt die Einheit fuer den Impulscounter.
 * \param	char *	Einheit als String.
 */
void IMPULS_setUnit( char * Unit )
{
	changeConfig_P( IMPULS_UNIT_P, Unit );
}

/**
 * \brief Holt die Einheit fuer den Impulscounter.
 * \param	char *	Zeiger auf einen Leeren String.
 * \retval	char *	Zeiger auf den String.
 */
char * IMPULS_getUnit( char * Unit )
{
	if ( readConfig_P( IMPULS_UNIT_P, Unit ) != -1 )
		return( Unit );

	return( NULL );
}

void IMPULS_Interrupt( void )
{
	static char PRESCALE = 0;

	if ( PRESCALE == 0 )
	{
		IMPULS_COUNTER++;
		PRESCALE = 1;
	}
	else
		PRESCALE = 0;
}


/**
 * \brief Das CGI-Interface für zum Anzeigen der Konfigurationsdaten im EEProm. 
 * \param 	pStruct		Pointer auf den HTTP_Request Struktur
 * \return	NONE
 */
void cgi_impuls( void * pStruct )
{
	long PreScaler;
	char UnitString[16];
	char SaveString[16];

	struct HTTP_REQUEST * http_request;
	http_request = (struct HTTP_REQUEST *) pStruct;

	PreScaler = IMPULS_getPrescaler();

	if ( IMPULS_getUnit( UnitString ) == NULL )
		UnitString[0] = '\0';

	cgi_PrintHttpheaderStart();

	if ( http_request->argc == 0 )
	{
		printf_P( PSTR("Zaehler: %ld,%03ld %s (%ld Impulse)<BR><BR>"
		               "<a href=\"impuls.cgi?config\">Eingestellungen</a>"), IMPULS_getCounter() / PreScaler, ( 1000 * ( IMPULS_getCounter() % PreScaler ) ) / PreScaler, UnitString, IMPULS_getCounter() );
	}
	else 
	{
		if( PharseCheckName_P( http_request, PSTR("config") ) )
		{
			if ( readConfig_P( IMPULS_AUTOSAVE_P, SaveString ) != -1 )
			{
				if ( !strcmp_P( SaveString, PSTR("on") ) )
					sprintf_P( SaveString, PSTR("checked"));
			}
			else
			{
				SaveString[0] = '\0';
			}

			printf_P( PSTR(	"<form action=\"impuls.cgi\">"
			               "<table border=\"0\" cellpadding=\"5\" cellspacing=\"0\">"
			               "<tr>"
			               "<td align=\"right\">AutoSave aktivieren</td>"
			               "<td><input type=\"checkbox\" name=\"AUTOSAVE\" value=\"on\" %s >"
			               "</td>"
			               "<tr>"
			               "<td align=\"right\">PreScaler</td>"
			               "<td><input name=\"PRESCALER\" type=\"text\" size=\"10\" value=\"%ld\" maxlength=\"10\"></td>"
			               "</tr>"
			               "<tr>"
			               "<td align=\"right\">Einheit</td>"
			               "<td><input name=\"UNIT\" type=\"text\" size=\"8\" value=\"%s\" maxlength=\"8\"></td>"
			               "</tr>"
			               "<tr>"
			               "<td></td><td><input type=\"submit\" value=\" Einstellung &Uuml;bernehmen \"></td>"
			               "</tr>"
			               "</table>"
			               "</form>" ) , SaveString, PreScaler, UnitString );
		}
		else
		{
			if ( PharseCheckName_P( http_request, PRESCALER_P ) )
			{
				changeConfig_P( IMPULS_PRESCALER_P, http_request->argvalue[ PharseGetValue_P ( http_request, PRESCALER_P ) ] );
			}
			if ( PharseCheckName_P( http_request, UNIT_P ) )
			{
				changeConfig_P( IMPULS_UNIT_P, http_request->argvalue[ PharseGetValue_P ( http_request, UNIT_P ) ] );
			}

			if ( PharseCheckName_P( http_request, AUTOSAVE_P ) )
			{
				changeConfig_P( IMPULS_AUTOSAVE_P, http_request->argvalue[ PharseGetValue_P ( http_request, AUTOSAVE_P ) ] );
			}
			else
			{
				changeConfig_P( IMPULS_AUTOSAVE_P, "off" );	
			}

			printf_P( PSTR("Einstellungen uebernommen!"));
		}
	}

	cgi_PrintHttpheaderEnd();
}

/**
 * \brief Gibt die Impulse als JSON aus.
 * \return	NONE
 */
long IMPULS_json_print( void )
{
	int i;
	char FILENAME[32];
	long PreScaler;
	long totalimpuls=0;
	
	// Struct erzeugen
	struct IMPULS_JSON impuls_json;

	impuls_json.logger_entry.DataSize = sizeof( impuls_json.impuls );

	PreScaler = IMPULS_getPrescaler();
	
	sprintf_P( FILENAME, impulslogfilename_P );

	IMPULS_clean( &impuls_json );
	LOGGER_getlastDBEntry ( &impuls_json.logger_entry, FILENAME );

	printf_P( PSTR("{ name: \"test\" ,data: [") );

	for ( i = 0 ; i < IMPULS_MAX ; i++ )
	{
		if ( i > 0 )
			printf_P( PSTR(","));

		if ( impuls_json.impuls[ i ] == IMPULS_ERROR )
		{
			printf_P( PSTR("null ") );
		}
		else
		{
			printf_P( PSTR("%ld.%03ld "), impuls_json.impuls[ i ] / PreScaler, ( 1000 * ( impuls_json.impuls[ i ] % PreScaler ) ) / PreScaler );
		}
	}
	printf_P( PSTR("]},"));

	return( totalimpuls );
}

/** 
 * \brief Gibt die Impulse als JSON aus.
 * \param	pStruct		Pointer auf den HTTP_Request Struktur.
 * \return	NONE
 */
void IMPULS_logger_cgi( void * pStruct )
{
	char ChartYName[32], ChartHeadline[32], ChartUnit[32];

	IMPULS_getUnit( ChartUnit );
	sprintf_P( ChartYName, PSTR("Verbrauch (%s)"), ChartUnit );
	sprintf_P( ChartHeadline, PSTR("Verlauf") );

	struct TIME nowtime;
	CLOCK_GetTime ( &nowtime );

	printf_P( HIGHCHART_part_1() , ChartHeadline, nowtime.YY, nowtime.MM-1, nowtime.DD, ChartYName, ChartUnit );
	IMPULS_json_print();
	printf_P( HIGHCHART_part_2() );
}

/** 
 * \brief Setzt ein Impulsloggerdtaensatz zurück.
 * \param	impuls_json		Pointer auf eine IMPULS_JSON Struktur.
 * \return	NONE
 */
void IMPULS_clean( struct IMPULS_JSON * impuls_json )
{
	int i;

	CLOCK_GetTime ( &impuls_json->logger_entry.time );

	for( i = 0 ; i < IMPULS_MAX ; i++ )
	{
		impuls_json->impuls[ i ] = IMPULS_ERROR;
	}
}
#endif

/**
 * @}
 */
