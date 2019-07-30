/***************************************************************************
 *            cmd_heater.c
 *
 *  Do Februar 26 19:11:47 2015
 *  Copyright  2015  Dirk Broßwick
 *  <sharandac@snafu.de>
 ****************************************************************************/
/*
 * cmd_heater.c
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
 * \ingroup modules
 * \addtogroup cmd_heater Steuerinface zum ansteuern einer Heizung (cmd_heater.c)
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
#include "system/shell/shell.h"
#include "system/config/eeconfig.h"
#include "hardware/heater/heater.h"

#include "cmd_heater.h"

void init_cmd_heater( void )
{
	HEATER_init();
	HEATER_work();
	
	SHELL_RegisterCMD( cmd_heater, PSTR("heater"));
	cgi_RegisterCGI( cgi_heater, PSTR("heater.cgi"));
}

int cmd_heater( int argc, char ** argv )
{
	if ( argc <= 3 )
	{
		if ( !strcmp_P( argv[1], PSTR("set") ) )
		{
			HEATER_set( atoi( argv[2] ) );
		}
		if ( !strcmp_P( argv[1], PSTR("on") ) )
		{
			HEATER_on();
		}
		if ( !strcmp_P( argv[1], PSTR("off") ) )
		{
			HEATER_off();
		}
		if ( !strcmp_P( argv[1], PSTR("work") ) )
		{
			HEATER_work();
		}
		if ( !strcmp_P( argv[1], PSTR("save") ) )
		{
			HEATER_save();
		}
		if ( !strcmp_P( argv[1], PSTR("stats") ) )
		{
			printf_P(PSTR("min = %ld, destination = %d, heateing = %d\r\n"),HEATER_get_time(), HEATER_get_temp(), HEATER_get_status() );
		}
	}
	else
	{
		printf_P(PSTR("heater <set n|on|off|work|save|stats> \r\n"));
	}
	return(0);
}


void cgi_heater( void * pStruct )
{
	struct HTTP_REQUEST * http_request;
	http_request = (struct HTTP_REQUEST *) pStruct;
		
	cgi_PrintHttpheaderStart();

	if ( PharseCheckName_P( http_request, PSTR("HEATER_TEMP") ) )
	{
		changeConfig_P( PSTR("HEATER_TEMP"), http_request->argvalue[ PharseGetValue_P ( http_request, PSTR("HEATER_TEMP") ) ] );
		HEATER_set( atoi( http_request->argvalue[ PharseGetValue_P ( http_request, PSTR("HEATER_TEMP") ) ] ) );
	}

		printf_P( PSTR(	"<form action=\"heater.cgi\">"
					   	"<table border=\"0\" cellpadding=\"5\" cellspacing=\"0\">"
						"<tr>"
					   	"<td align=\"right\">Solltemperatur</td>"
						"<td><input name=\"HEATER_TEMP\" type=\"text\" size=\"30\" value=\"%d\" maxlength=\"3\"></td>"
  						"</tr>"
						"<td></td><td><input type=\"submit\" value=\" Einstellung &Uuml;bernehmen \"></td>"
  						"</tr>"
					   	"</table>"
						"</form>"), HEATER_get_temp() );	

	cgi_PrintHttpheaderEnd();
}

#endif

/**
 * @}
 */