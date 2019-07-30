/***************************************************************************
 *            cmd_adx.c
 *
 *  Thu Nov  5 17:02:56 2009
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

/**
 * \ingroup modules
 * \addtogroup cmd_adc Shell- und CGI-Interface um fuer den adc (cmd_adc.c)
 *
 * @{
 */

/**
 * \file
 *
 */
 
#include <avr/pgmspace.h>
#include <avr/version.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "config.h"

#if defined(ANALOG) 

#include "hardware/adc/adc.h"
#include "hardware/gpio/gpio_out.h"
#include "system/config/eeconfig.h"

#include "system/shell/shell.h"
#include "cmd_waterpump.h"

const char WATERPUMP_P[] PROGMEM = { "WATERPUMP" };

void init_cmd_waterpump( void )
{
#if defined(SHELL)
	SHELL_RegisterCMD( cmd_waterpump, PSTR("waterpump") );
#endif
}

int cmd_waterpump( int argc, char ** argv )
{
	char string[16];
	int trigger=512,temp=0,res=0;

	if ( readConfig_P( WATERPUMP_P, string ) != -1 )
		trigger = atoi( string );
	
	GPIO_out_set( 9 );
	_delay_ms(1000);
	res = ADC_GetValue( 7 );
	GPIO_out_clear( 9 );

	temp = ADC_mVolt( res + 1 , 5000 );
	printf_P(PSTR("Messwert: %d, V=%d.%03dVolt, trigger bei %d).\r\n"),res,temp / 1000, temp % 1000, trigger );

	if( argc <= 1 )
	{
		if ( res > trigger )
		{
			printf_P(PSTR("Pumpe ein für 5sek.\r\n"));
			GPIO_out_set( 8 );
			_delay_ms(5000);
			GPIO_out_clear( 8 );
		}
		else
		{
			printf_P(PSTR("Kein Wasser nötig.\r\n"));
		}
	}
	
	return(0);
}

#endif

/**
 * @}
 */
