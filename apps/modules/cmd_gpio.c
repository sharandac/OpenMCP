/***************************************************************************
 *            cmd_gpio.c
 *
 *  Fr Feb  17 17:02:56 2012
 *  Copyright  2012  Dirk Broßwick
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
 *
 * \ingroup modules
 * \addtogroup cmd_gpio Shell- und CGI-Interface zu GPIO (cmd_gpio.c)
 *
 * @{
 */

/**
 *
 * \author Dirk Broßwick
 *
 */

#include <avr/pgmspace.h>
#include <stdio.h>
#include <avr/version.h>
#include <avr/eeprom.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "config.h"

#if defined(DIGITAL_IO)

#include "hardware/gpio/gpio_in.h"
#include "hardware/gpio/gpio_out.h"
#include "apps/httpd/cgibin/cgi-bin.h"
#include "apps/httpd/httpd2.h"
#include "apps/httpd/httpd2_pharse.h"
#include "system/shell/shell.h"
#include "cmd_gpio.h"

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Registriert das Shell- und CGI-Interface zu GPIO.
 * \param 	NONE
 * \return	NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
void init_cmd_gpio( void )
{
#if defined(SHELL)
	SHELL_RegisterCMD( cmd_gpio, PSTR("gpio_out"));
#endif
#if defined(HTTPSERVER_DIGITAL_IO)
	cgi_RegisterCGI( cgi_dio_in, PSTR("dio_in.cgi"));
	cgi_RegisterCGI( cgi_dio_out, PSTR("dio_out.cgi"));
#endif
}

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Das Shell-Interface zu GPIO. 
 * \param 	argc		Anzahl der Argumente
 * \param 	**argc		Pointer auf die Argumente
 * \return	NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
int cmd_gpio( int argc, char ** argv )
{
	int i;
	
	i = atoi( argv[ 2 ] );
	if ( !( i < GPIO_out_max() && i >= 0) )	
	{
		printf_P( PSTR("Error, %d not valid.\r\n"), i );
		return( 0 );
	}
	
	if ( !strcmp_P( argv[ 1 ], PSTR("get") ) )
	{
	}
	if ( !strcmp_P( argv[ 1 ], PSTR("set") ) )
	{
		GPIO_out_set( i );
	}
	if ( !strcmp_P( argv[ 1 ], PSTR("toggle") ) )
	{
		if ( GPIO_out_state( i ) != 0 )
			GPIO_out_clear( i );
		else
			GPIO_out_set( i );
	}
	if ( !strcmp_P( argv[ 1 ], PSTR("clear") ) )
	{
		GPIO_out_clear( i );
	}
	else
		printf_P( PSTR("gpio_out <get|set|toggle|clear> <0..%d>\r\n"), GPIO_out_max() );	

	printf_P( PSTR("GPIO (%d) = %d\r\n"), i, GPIO_out_state( i ) );

	return(0);
	
}

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Das CGI-Interface für zum Anzeigen der Digitalen Eingänge.
 * \param 	pStruct	Struktur auf den HTTP_Request
 * \return	NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
void cgi_dio_in( void * pStruct )
{
	char i;
	
	cgi_PrintHttpheaderStart();

	printf_P( PSTR(	"<form action=\"dio_out.cgi\">"
					"<table border=\"0\" cellpadding=\"5\" cellspacing=\"0\">" ) );

	for( i = 0 ; i < GPIO_in_max() ; i++ )
	{
		printf_P( PSTR(	"<tr>"
					   	"<td align=\"right\">Port%c%d =</td>" ), ( GPIO_in_Port(i) >> 3 ) + 'A', GPIO_in_Port(i) % 8 );

		if ( GPIO_in_state( i ) != 0 )
		{
			printf_P( PSTR( "<td>On</td>") ); 
		}
		else
		{
			printf_P( PSTR( "<td>Off</td>") ); 
		}
		printf_P( PSTR(	"</td>"
 						"</tr>") );
	}
	
	printf_P( PSTR(	"</table>"
					"</form>" ));
	
	cgi_PrintHttpheaderEnd();
}

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Das CGI-Interface für zum Steuern der Digitalen Ausgänge
 * \param 	pStruct	Struktur auf den HTTP_Request
 * \return	NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
void cgi_dio_out( void * pStruct )
{
	struct HTTP_REQUEST * http_request;
	http_request = (struct HTTP_REQUEST *) pStruct;
	
	char i;
	char gpio[10];
	
	if ( http_request->argc != 0 )
	{
		for( i = 0 ; i < GPIO_out_max() ; i++ )
		{
			sprintf_P( gpio, PSTR("GPIO%d"), i );
			if ( PharseCheckName( http_request, gpio ) ) GPIO_out_set( i );
			else GPIO_out_clear( i );
		}
	}

	cgi_PrintHttpheaderStart();
	
	printf_P( PSTR(	"<form action=\"dio_out.cgi\">"
					"<table border=\"0\" cellpadding=\"5\" cellspacing=\"0\">" ) );

	for( i = 0 ; i < GPIO_out_max() ; i++ )
	{
		printf_P( PSTR(	"<tr>"
						"<td align=\"right\">Port%c%d =</td>" ), ( GPIO_out_Port(i) >> 3 ) + 'A', GPIO_out_Port(i) % 8 );

		printf_P( PSTR( "<td><input type=\"checkbox\" name=\"GPIO%d\" "),i);
		
		if ( GPIO_out_state( i ) != 0 )
		{
			printf_P( PSTR( "checked" ), i); 
		}

		printf_P( PSTR(	"></td>"
 						"</tr>") );
	}

	printf_P( PSTR(	"<tr>"
					"<td></td><td><input type=\"submit\" name=\"SUB\"></td>"
					"</tr>"
				   	"</table>"
					"</form>" ));

	cgi_PrintHttpheaderEnd();
}
#endif

/**
 * @}
 */
