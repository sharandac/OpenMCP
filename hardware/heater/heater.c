/***************************************************************************
 *            heater.c
 *
 *  Do Februar 26 19:31:28 2015
 *  Copyright  2015  Dirk Broßwick
 *  <sharandac@snafu.de>
 ****************************************************************************/
/*
 * heater.c
 *
 * Copyright (C) 2015 - Dirk Broßwick
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * \ingroup hardware
 * \addtogroup heater Steuert eine Heizung an (heater.c)
 *
 * @{
 */

/**
 * \file
 *
 * \author Dirk Broßwick
 */

#include <avr/pgmspace.h>
#include <avr/version.h>
#include <avr/eeprom.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "config.h"

#if defined(HEATERCONTROL)

#include "apps/httpd/cgibin/cgi-bin.h"
#include "apps/httpd/httpd2.h"
#include "apps/httpd/httpd2_pharse.h"
#include "system/config/eeconfig.h"
#include "system/sensor/temp.h"
#include "hardware/gpio/gpio_out.h"

#include "heater.h"

static int heating_temp_offset = 0;
static long heating_time_mm = 0;
static long heating_offtime_mm = 0;
static char heating = HEATER_OFF;
static int destination_temp = 19;

const char HEATER_TEMP[] PROGMEM = "HEATER_TEMP";
const char HEATER_TEMP_OFFSET[] PROGMEM = "HEATER_TEMP_OFFSET";
const char HEATER_TEMP_MM[] PROGMEM = "HEATER_TEMP_MM";
const char HEATER_OFFTEMP_MM[] PROGMEM = "HEATER_OFFTEMP_MM";

void HEATER_init( void )
{
	char string[16];

	if ( readConfig_P( HEATER_TEMP, string ) != -1 )
			HEATER_set( atoi( string ) );

	if ( readConfig_P( HEATER_TEMP_OFFSET, string ) != -1 )
			heating_temp_offset = atoi( string );

	if ( readConfig_P( HEATER_TEMP_MM, string ) != -1 )
			heating_time_mm = atol( string );

	if ( readConfig_P( HEATER_OFFTEMP_MM, string ) != -1 )
			heating_offtime_mm = atol( string );
}

void HEATER_work( void )
{
	int temp;
	char string[16];
	
	// temeratur einlesen
	temp = TEMP_readtemp( 0 ) ;
	
	if ( heating == HEATER_ON )
		heating_time_mm++;
	else
		heating_offtime_mm++;

	if ( temp != TEMP_ERROR )
	{
		temp = temp + heating_temp_offset;		
		temp = temp>>8;

		if ( !( heating_time_mm%100 ) || !( heating_offtime_mm%100 )  )
		{
			sprintf_P(string, PSTR("%ld"),heating_offtime_mm );
			changeConfig_P( HEATER_OFFTEMP_MM, string );
			sprintf_P(string, PSTR("%ld"),heating_time_mm );
			changeConfig_P( HEATER_TEMP_MM, string );
		}

		if ( temp < destination_temp )
		{
			HEATER_on();
		}
		else if( temp >= destination_temp )
		{
			HEATER_off();
		}
	}
	else
	{
		HEATER_off();
	}
	
}

void HEATER_save( void )
{
	char string[16];
	
	sprintf_P(string, PSTR("%ld"),heating_offtime_mm );
	changeConfig_P( HEATER_OFFTEMP_MM, string );
	sprintf_P(string, PSTR("%ld"),heating_time_mm );
	changeConfig_P( HEATER_TEMP_MM, string );
}

void HEATER_set( int temp )
{
	char string[16];

	destination_temp = temp;

	sprintf_P(string, PSTR("%d"),destination_temp );
	changeConfig_P( HEATER_TEMP, string );
}

void HEATER_on( void )
{
	GPIO_out_clear( HEATER_PIN1 );
	GPIO_out_clear( HEATER_PIN2 );
	heating = HEATER_ON;	 	
}

void HEATER_off( void )
{
	GPIO_out_set( HEATER_PIN1 );
	GPIO_out_set( HEATER_PIN2 );
	heating = HEATER_OFF;	 	
}

long HEATER_get_time( void )
{
	return( heating_time_mm );
}

int HEATER_get_temp( void )
{
	return( destination_temp );
}

char HEATER_get_status( void )
{
	return( heating );
}

#endif

/**
 * @}
 */