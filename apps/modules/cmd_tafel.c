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

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/version.h>
#include <avr/eeprom.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "config.h"

#if defined(LEDTAFEL) || defined(LEDTAFEL_PAR)

#include "system/stdout/stdout.h"
#if defined(LEDTAFEL)
#include "hardware/led-tafel/led_tafel.h"
#endif
#if defined(LEDTAFEL_PAR)
#include "hardware/led-tafel/led_tafel-par.h"
#endif
#include "apps/telnet/telnet.h"
#include "apps/httpd/cgibin/cgi-bin.h"
#include "apps/httpd/httpd2.h"
#include "apps/httpd/httpd2_pharse.h"
#include "system/net/udp.h"
#include "system/thread/thread.h"
#include "hardware/led/led_core.h"
#include "cmd_tafel.h"

int TAFEL_UDP_Socket;

// make an static UDPbuffer
// warning, do not use an stack-allocated buffer! it will be damage the udp-packet
static char TAFEL_UDPBuffer[ TAFEL_UDP_Bufferlen ];

void TAFEL_callback( char * Buffer, int Bufferlen );

void TAFEL_callback( char * Buffer, int Bufferlen )
{

	struct TAFEL_CMD_HEADER * TAFELCMD;
	TAFELCMD = ( struct TAFEL_CMD_HEADER * ) Buffer;

	LED_on(2);

	if ( TAFELCMD->TAFEL_CMD == read_lum_raw || TAFELCMD->TAFEL_CMD == read_raw )			
	{
		LEDTAFEL_process_cmd( TAFELCMD );
		UDP_SendPacket( TAFEL_UDP_Socket, Bufferlen, Buffer );
	}
	else if ( TAFELCMD->TAFEL_CMD == graphic_direct )
	{
//		UDP_SendPacket( TAFEL_UDP_Socket, Bufferlen, Buffer );
		LEDTAFEL_process_cmd( TAFELCMD );
	}
	else
	{
//		UDP_SendPacket( TAFEL_UDP_Socket, sizeof( TAFELCMD ), Buffer );
		LEDTAFEL_process_cmd( TAFELCMD );
	}
	
	LED_off(2);
}

void init_cmd_tafel( void )
{
	#if defined(HTTPSERVER) && defined(LEDTAFEL)
		cgi_RegisterCGI( cgi_tafel, PSTR("tafel.cgi"));
	#endif
	#if defined(UDP)
		TAFEL_UDP_Socket = UDP_ListenOnPort( TAFEL_UDPPORT, TAFEL_UDP_Bufferlen, TAFEL_UDPBuffer );
		UDP_RegisterCallback( TAFEL_UDP_Socket, TAFEL_callback );
	#endif
}

#if defined(HTTPSERVER) && defined(LEDTAFEL)
void cgi_tafel( void * pStruct )
{
	struct HTTP_REQUEST * http_request;
	http_request = (struct HTTP_REQUEST *) pStruct;

	struct STDOUT oldstream;

	unsigned int Zeile = 1,Spalte = 1;

	if ( PharseCheckName_P( http_request, PSTR("spalte") ) )
		Spalte = atol( http_request->argvalue[ PharseGetValue_P( http_request, PSTR("spalte") ) ] );
		
	if ( PharseCheckName_P( http_request, PSTR("zeile") ) )
		Zeile = atol( http_request->argvalue[ PharseGetValue_P( http_request, PSTR("zeile") ) ] );

	printf_P( PSTR(		"<HTML>\r\n"
						"<HEAD>\r\n"
						"<TITLE>LED-Tafel</TITLE>\r\n"
						"</HEAD>\r\n"
						"<BODY>\r\n"));
	printf_P( PSTR(		"<form action=\"tafel.cgi\" method=\"get\" accept-charset=\"ISO-8859-1\">"
					    "Zeile<input name=\"zeile\" size=\"2\" value=\"%d\">"
					    "Spalte<input name=\"spalte\" size=\"2\" value=\"%d\">"
					    "Text<input name=\"text\" size=\"56\">"
						"<p><input type=\"submit\" value=\"Abschicken\"></p></form>"
					   	"<form action=\"tafel.cgi?clr\" method=\"get\" accept-charset=\"ISO-8859-1\">"
					    "<p><input type=\"submit\" name=\"clr\" value=\"clr\"></p></form>"
						"</BODY>\r\n"
						"</HTML>\r\n"
						"\r\n"), LEDTAFEL_getRow(), LEDTAFEL_getLine() );

	if( PharseCheckName_P( http_request, PSTR("clr") ) )
	{
		LEDTAFEL_initialize_display();
	}
	else if( PharseCheckName_P( http_request, PSTR("text") ) )
	{			

		STDOUT_save( &oldstream );
		STDOUT_set( TAFEL, 0);

		LEDTAFEL_selectMatrix( Zeile, Spalte );
		printf_P( PSTR("%s"), http_request->argvalue[ PharseGetValue_P( http_request, PSTR("text") ) ] );

		STDOUT_restore( &oldstream );
	}
	STDOUT_Flush();	
}
#endif

#endif
