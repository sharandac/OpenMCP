/***************************************************************************
 *            cmd_ping.c
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
 * \addtogroup cmd_ping Shell-Interface zu ping (cmd_ping.c)
 *
 * @{
 */

/**
 *
 * \author Dirk Broßwick
 *
 */
 
#include <avr/pgmspace.h>
#include <avr/version.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "config.h"

#if defined(DNS)

#include "system/net/ip.h"
#include "system/net/dns.h"
#include "system/net/icmp.h"
#include "system/shell/shell.h"
#include "cmd_ping.h"
#include "system/stdout/stdout.h"

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Registriert das Shell- und CGI-Interface zu Ping.
 * \param 	NONE
 * \return	NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
void init_cmd_ping( void )
{
#if defined(SHELL)
	SHELL_RegisterCMD( cmd_ping, PSTR("ping"));
#endif
}

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Das Shell-Interface zu Ping. 
 * \param 	argc		Anzahl der Argumente
 * \param 	**argc		Pointer auf die Argumente
 * \return	NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
int cmd_ping( int argc, char ** argv )
{
	
	long ip;
	int counter;
	
	if ( argc == 2 )
	{
		ip = strtoip( argv[1] );

		if ( ip == 0 )
		{
			ip = DNS_ResolveName( argv[ 1 ] );
			if ( ip == DNS_NO_ANSWER )
			{
				printf_P( PSTR("unknown host\r\n"));
				return( 0 );
			}
		}

		counter = icmp_send(ip);
		
		if ( counter != -1 )
			printf_P(PSTR("icmp-request to %s = %dms\r\n"),argv[1], counter );
		else
			printf_P(PSTR("icmp-request error, host (%s) not response.\r\n"),argv[1] );
	}
	else
		printf_P( PSTR("ping <ip>\r\n"));

	return(0);
}

#endif

/**
 * @}
 */

