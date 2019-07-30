/***************************************************************************
 *            cmd_mon.c
 *
 *  Tue Aug  9 09:37:13 2011
 *  Copyright  2011  Dirk Broßwick
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
#include <avr/version.h>
#include <avr/eeprom.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "config.h"

#if defined ( EtherSense )

#include "hardware/twi/hh10d.h"
#include "cmd_mon.h"
#include "hardware/adc/adc.h"
#include "hardware/gpio/gpio_core.h"

#include "system/net/arp.h"
#include "system/net/ip.h"
#include "system/net/dns.h"
#include "system/net/ethernet.h"
#include "system/stdout/stdout.h"
#include "system/config/eeconfig.h"
#include "system/shell/shell.h"
#include "apps/httpd/cgibin/cgi-bin.h"
#include "apps/httpd/httpd2.h"
#include "apps/httpd/httpd2_pharse.h"

int Rt [] = { 2882, 2302, 1851, 1498, 1220, 1000, 824, 683, 568, 476, 400 , 338, 287, 245, 209, 180, 155, 135, 117, 102, 89 };
int t  [] = {   0,     5,   10,   15,   20,   25,  30,  35,  40,  45,  50 ,  55,  60,  65,  70,  75,  80,  85,  90,  95,100 };


void MON_init ( void )
{
	GPIO_setPullUp( PORTA_1 );
	GPIO_setPullUp( PORTA_2 );
	cgi_RegisterCGI( cgi_mon, PSTR("mon.cgi") );
}

void cgi_mon( void * pStruct )
{
	int temp, i_bias, resist, i;
	temp = ADC_mVolt( ADC_GetValue( 0 ) + 1 , 5000 );

	// Strom in uA
	i_bias = ( 5000 - temp ) / 68;

	// Widerstand in 110Ohm
	resist = ( temp * 10 ) / i_bias ;

	for ( i = 0 ; i < sizeof (Rt) ; i++ )
	{
		if ( Rt[ i ] < resist )
		{
			temp = ( Rt[i-1] - Rt[i] ) / 5 ;
			temp = ( resist - Rt[i] ) / temp ;
			temp = t[i] - temp;
			
			break;
		}
	}
	
	printf_P( PSTR( "MAC: %02x:%02x:%02x:%02x:%02x:%02x\r\n"),mymac[0],mymac[1],mymac[2],mymac[3],mymac[4],mymac[5]);	
	printf_P( PSTR( "humidity: %d %%\r\n"), HH10D_meas() );
	printf_P( PSTR( "temperature: %d C, %d.%02d kOhm\r\n"), temp, resist / 100 , resist % 100 );
	printf_P( PSTR( "door switch 1: %d\r\n"), GPIO_getPin( PORTC_2) );	
	printf_P( PSTR( "door switch 2: %d\r\n"), GPIO_getPin( PORTC_3) );
	printf_P( PSTR( "water adc 1: %d\r\n"), ADC_GetValue( 1 ) );
	printf_P( PSTR( "water adc 2: %d\r\n"), ADC_GetValue( 2 ) );
}

#endif
