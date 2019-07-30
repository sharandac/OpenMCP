/***************************************************************************
 *            cmd_eemem.c
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
 * \addtogroup cmd_eemem Shell- und CGI-Interface zu eemem (cmd_eemem.c)
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

#include "apps/httpd/cgibin/cgi-bin.h"
#include "apps/httpd/httpd2.h"
#include "apps/httpd/httpd2_pharse.h"
#include "system/shell/shell.h"
#include "system/config/eeconfig.h"

#include "cmd_eemem.h"

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Registriert das Shell- und CGI-Interface zu eemem.
 * \param 	NONE
 * \return	NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
void init_cmd_eemem( void )
{
#if defined(SHELL)
	SHELL_RegisterCMD( cmd_eemem, PSTR("eemem"));
#endif
#if defined(HTTPSERVER_EEMEM)
	cgi_RegisterCGI( cgi_eemem, PSTR("eemem.cgi"));
#endif
}

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Das Shell-Interface für zum Anzeigen der Konfigurationsdaten im EEProm. 
 * \param 	argc		Anzahl der Argumente
 * \param 	**argc		Pointer auf die Argumente
 * \return	NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
int cmd_eemem( int argc, char ** argv )
{
	int ERROR = -1;
	char string[ 32 ];

	
	if ( argc >= 2 )
	{
		if ( !strcmp_P( argv[1], PSTR("protect") ) )
		{
			strcpy_P( string, PSTR("ON"));
			changeConfig_P( PSTR("WRITE_PROTECT"), string );
			setprotectConfig( PROTECT );
			ERROR = 0; 
		}
		else if ( !strcmp_P( argv[1], PSTR("unprotect") ) )
		{
			setprotectConfig( UNPROTECT );
			strcpy_P( string, PSTR("OFF"));
			changeConfig_P( PSTR("WRITE_PROTECT"), string );
			printf_P( PSTR("unprotectet\r\n"));
			ERROR = 0; 
		}
		else if ( !strcmp_P( argv[1], PSTR("add") ) )
		{
			if ( changeConfig( argv[2], argv[3] ) == 1 )
				ERROR = 0; 
		}
		else if ( !strcmp_P( argv[1], PSTR("del") ) )
		{
			if ( deleteConfig( argv[2] ) == 1 )
				ERROR = 0; 
		}
		else if ( !strcmp_P( argv[1], PSTR("print") ) )
		{
			PrintConfig();
			ERROR = 0; 
		}
		else if ( !strcmp_P( argv[1], PSTR("clean") ) )
		{
			makeConfig();
			ERROR = 0; 
		}
	}

	if ( ERROR == -1 )
		printf_P( PSTR("eemem <protect>|<unprotect> <add [name val]> <del [name]> <print> <clean>\r\n"));

	return(0);
}

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Das CGI-Interface für zum Anzeigen der Konfigurationsdaten im EEProm. 
 * \param 	pStruct	Struktur auf den HTTP_Request
 * \return	NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
void cgi_eemem( void * pStruct )
{
	cgi_PrintHttpheaderStart();

	printf_P( PSTR(	"<pre>"));

	PrintConfig ();

	printf_P( PSTR( "</pre>" ));

	cgi_PrintHttpheaderEnd();

}

/**
 * @}
 */

