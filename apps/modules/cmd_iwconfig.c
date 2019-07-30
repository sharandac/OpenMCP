/***************************************************************************
 *            cmd_ifconfig.c
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
 * \addtogroup cmd_ifconfig Shell- und CGI-Interface zu ifconfig (cmd_ifconfig.c)
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
#include "system/shell/shell.h"

#include "cmd_iwconfig.h"

#if defined USE_ZG2100

#include "hardware/network/zg2100/zg2100_driver.h"

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Registriert das Shell- und CGI-Interface zu ifconfig.
 * \param 	NONE
 * \return	NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
void init_cmd_iwconfig( void )
{
#if defined(SHELL)
	SHELL_RegisterCMD( cmd_connect, PSTR("scan"));
#endif
}

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Das Shell-Interface für zum Anzeigen der IP-config.
 * \param 	argc		Anzahl der Argumente
 * \param 	**argc		Pointer auf die Argumente
 * \return	NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
int cmd_connect( int argc, char ** argv )
{
//	printf_P(PSTR("init ZG2100\r\n"));
//	zg2100_drv_init();

	printf_P(PSTR("starte scan ... \r\n"));
	zg2100_start_scan();
	
	return(0);
}

#endif

/**
 * @}
 */
