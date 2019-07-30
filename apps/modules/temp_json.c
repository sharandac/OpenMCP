/***************************************************************************
 *            temp_json.c
 *
 *  Sun Dec 20 00:51:51 2009
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
#include <avr/pgmspace.h>
#include "config.h"
#include "system/thread/thread.h"
#include "system/clock/clock.h"
#include "apps/httpd/cgibin/cgi-bin.h"
#include "apps/httpd/httpd2.h"
#include "apps/httpd/httpd2_pharse.h"
#include "temp_json.h"
#include "system/sensor/temp.h"
#include "system/config/eeconfig.h"
#include "system/nano_DB/logger.h"
#include "system/nano_DB/highchart.h"

#if defined(MMC) && defined(TEMP_LOGGER)

// #define DEBUG

static char lastmm;
const char Sensor_P[] PROGMEM = "sensor";
const char Templogfilename_P[] PROGMEM = "templog_%02d.ndb";

/**
 * \ingroup modules
 * \addtogroup Templogger Temploggermodul (temp_json.c)
 *
 * Dieses Modul loggt die Temperatur von Sensoren die im EEprom hinterlegt
 * sind und speichert sie in einer nanoDB auf SD-Card.
 *
 * @{
 */

/**
 * \file
 * Loggt die Temperatur.
 *
 * \author Dirk Broßwick
 */

/**
 * \brief Initialisiert das Temploggermodul und Registriert das CGI-Interface.
 * \return	void
 */
void temp_json_init( void )
{
	lastmm = 0;

	THREAD_RegisterThread( temp_json_thread, PSTR("Temp Logger"));
	cgi_RegisterCGI( temp_json_cgi, PSTR("temp.json"));
	cgi_RegisterCGI( temp_logger_cgi, PSTR("templogger.cgi"));
	temp_json_thread();
}

/**
 * \brief Der Temp_Json Thread, wird zyklisch aufgerufen.
 * \return	void
 */
void temp_json_thread( void )
{
	int retval = LOGGER_ERROR;
	int Temp=0x8000;
	static char Sensor=0;
	char FILENAME[32];
	
	struct TEMP_JSON temp_json;
	temp_json.logger_entry.DataSize = sizeof( temp_json.temp );
	
	// Zeit holen
	struct TIME nowtime;
	CLOCK_GetTime ( &nowtime );

	// Update ?
	if ( ( nowtime.mm % 10 == 0 ) && ( lastmm != nowtime.mm ) )
	{
		Sensor = 0;
		lastmm = nowtime.mm ;
	}

	if( Sensor < TEMP_MAX_SENSORS )
	{
		Temp = TEMP_readtemp( Sensor );

		if ( Temp != TEMP_ERROR )
		{
			// erzeuge mal einen Dummydatensatz
			temp_logger_clean( &temp_json );
			// Dateinamen erzeigen zum Sensor
			sprintf_P( FILENAME, Templogfilename_P , Sensor );
			// Versuche letzten Datensatz zu holen
			retval = LOGGER_getlastDBEntry ( &temp_json.logger_entry , FILENAME );
			// Wenn letzter Eintrag nicht lesbar, neue NanoDB erzeugen
			if ( retval == LOGGER_ERROR )
			{
#if defined( DEBUG )
				printf_P( PSTR("Last DBEntry failed. Make new nano_DB %s.\r\n"), FILENAME );
#endif
				// Neue NanoDB erzeugen
				retval = LOGGER_addDBentry( &temp_json.logger_entry , FILENAME );
			}
			// Datenbankeintrag erzeugt oder gelesen ?
			if ( retval == LOGGER_OK )
			{
					// letzer Eintrag von heute?
				if ( ! ( temp_json.logger_entry.time.YY == nowtime.YY && temp_json.logger_entry.time.MM == nowtime.MM && temp_json.logger_entry.time.DD == nowtime.DD ) )
				{
#if defined( DEBUG )
				printf_P(PSTR("Add new DBEntry in %s.\r\n"), FILENAME );
#endif
					// Neue NanoDB erzeugen
					temp_logger_clean( &temp_json );
					retval = LOGGER_addDBentry( &temp_json.logger_entry , FILENAME );
				}
			}
#if defined( DEBUG )
			else
				printf_P(PSTR("Make new nano_DB %s failed.\r\n"), FILENAME );
#endif
					
			// alles Ok bis hier hin
			if ( retval == LOGGER_OK )
			{
#if defined( DEBUG )
				printf_P(PSTR("Write actual DBEntry in %s.\r\n"), FILENAME );
#endif
					// Temp speichern
				temp_json.temp[ ( nowtime.hh * 6 ) + ( nowtime.mm / 10 ) ] = Temp;
				// und sichern in Datenbank
				LOGGER_writeDBEntry( &temp_json.logger_entry, FILENAME );
			}
		}
		Sensor++;
	}
}


/**
 * \brief Das CGI-Interface. Gibt ein Json für einen Tag aus.
 * \param 	PStruct		Structur vom HTTP-Server mit allen Datem vom Request,.
 * \return  void
 */
void temp_json_cgi( void * pStruct )
{
	char Sensor=0;
	
	struct HTTP_REQUEST * http_request;
	http_request = (struct HTTP_REQUEST *) pStruct;

	if( PharseCheckName_P( http_request, Sensor_P ) )
	{
		Sensor = (char) atoi( http_request->argvalue[ PharseGetValue_P ( http_request, Sensor_P ) ] );
	}

	temp_json_print( Sensor );
}

/**
 * \brief Gibt fuer einen Sensor ein Json-Objekt zurueck.
 * \param 	Sensor		Sensor der Ausgelesen werden soll.
 * \return  void
 */
void temp_json_print( char Sensor )
{
	int i;
	char * TEMPNAME ,TEMPSENSOR[64];
	char FILENAME[32];
	
	// Struct erzeugen
	struct TEMP_JSON temp_json;

	if ( TEMP_getSensorConfig( Sensor, TEMPSENSOR ) != NULL )
	{
		// Dateinamen für den Sensor erzeugen
		sprintf_P( FILENAME, Templogfilename_P , Sensor );
		// Datengröße setzen
		temp_json.logger_entry.DataSize = sizeof( temp_json.temp );

		temp_logger_clean( &temp_json );
		// Letzten Datenbankeintrag holen b.z.w. neuen erzeugen wenn alter von einem anderen Tag ist
		LOGGER_getlastDBEntry ( &temp_json.logger_entry, FILENAME );

		TEMPNAME = TEMPSENSOR;	
		TEMPNAME = TEMPNAME + strlen( TEMPNAME ) -1 ;
			
		while( *TEMPNAME != ';' )
		{
			TEMPNAME--;
		}
		TEMPNAME++;
				
		printf_P( PSTR("{name:'%s',data: ["), TEMPNAME );

		for ( i = 0 ; i < TEMP_MAX ; i++ )
		{
			if ( i > 0 )
				printf_P( PSTR(","));

			if ( TEMP_Temp2String( temp_json.temp[ i ] , TEMPSENSOR ) == NULL )
				printf_P( PSTR("null ") );
			else
				printf_P( PSTR("%s "), TEMPSENSOR );
		}
		printf_P( PSTR("]},"));
	}
}

 /**
 * \brief Das CGI-Interface. Erzeugt einen netten Graphen.
 * \param 	PStruct		Structur vom HTTP-Server mit allen Datem vom Request,.
 * \return  void
 */
void temp_logger_cgi( void * pStruct )
{
	char Sensor, ChartYName[32], ChartHeadline[32], ChartUnit[32];

	sprintf_P( ChartUnit, PSTR("&deg;C") );
	sprintf_P( ChartYName, PSTR("Temperatur (%s)"), ChartUnit );
	sprintf_P( ChartHeadline, PSTR("Temperaturverlauf") );
	
	struct HTTP_REQUEST * http_request;
	http_request = (struct HTTP_REQUEST *) pStruct;

	struct TIME nowtime;

	CLOCK_GetTime ( &nowtime );

	printf_P( HIGHCHART_part_1() , ChartHeadline, nowtime.YY, nowtime.MM-1, nowtime.DD, ChartYName, ChartUnit );

	if( PharseCheckName_P( http_request, Sensor_P ) )
	{
		Sensor = atoi( http_request->argvalue[ PharseGetValue_P ( http_request, Sensor_P ) ] );
		temp_json_print( Sensor );
	}
	else
	{
		for ( Sensor = 0 ; Sensor < TEMP_MAX_SENSORS ; Sensor++ )
			temp_json_print( Sensor );
	}
	
	printf_P( HIGHCHART_part_2() );
}

/**
 * \brief Löscht und initialisiert das Struct TEMP_JSON
 * \return  void
 */
void temp_logger_clean( struct TEMP_JSON * temp_json )
{
	int i;

	CLOCK_GetTime (  &temp_json->logger_entry.time );

	for( i = 0 ; i < TEMP_MAX ; i++ )
	{
		temp_json->temp[ i ] = TEMP_ERROR;
	}
}

#endif

/**
 * @}
 */
