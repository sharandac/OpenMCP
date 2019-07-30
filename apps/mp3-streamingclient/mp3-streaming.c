/*! \file mp3-streaming.c \brief MP3-Streamingengine zum streamen von MP3 */
//***************************************************************************
//*            mp3-streaming.c
//*
//*  Sat May  10 21:07:42 2008
//*  Copyright  2008  Dirk Broßwick
//*  Email: sharandac@snafu.de
//****************************************************************************/
///	\ingroup software
///	\defgroup mp3stream Tsumani MP3-Streamingengine (mp3-streaming.c)
///	\code #include "mp3-streaming.h" \endcode
///	\par Uebersicht
/// 	Die Tsunami MP3-streamingengine zum streamen von mp3 über das netzwerk.
/// 	Die Engine kümmert sich um das Puffern des MP3-Stream der von einem Shoutcast-
/// 	Streamingserver gesendet wird.
//****************************************************************************/
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
//@{
#include <avr/pgmspace.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/io.h>

#include "config.h"

#if defined(HTTPSERVER_STREAM)

#include "mp3-streaming.h"
#include "mp3-clientserver.h"
#include "streaminfo.h"

	#include "hardware/vs10xx/vs10xx.h"
	#include "hardware/vs10xx/vs10xx_buffer.h"

#include "system/buffer/fifo.h"
#include "system/clock/clock.h"
#include "system/math/math.h"
#include "system/net/tcp.h"
#include "system/net/dns.h"
#include "system/net/ethernet.h"
#include "system/stdout/stdout.h"
#include "system/thread/thread.h"
#include "system/shell/shell.h"

volatile int MP3_SOCKET = SOCKET_ERROR;
volatile unsigned long totalbytes = 0;
volatile unsigned int vol = 255;

volatile unsigned long metaintcounter = 0;
volatile unsigned int metainfolen = 0;
volatile unsigned char metainfo = 1;
volatile unsigned int metaint = 0;
volatile unsigned char weakstream = 2;
volatile unsigned char autoreplay = 1;
volatile int mp3timer;

volatile unsigned char verboselevel = 0;

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Initialisiert die streamingengine.
 * \param 	NONE
 * \return	NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
void mp3client_init( void )
{
	MP3_SOCKET = SOCKET_ERROR;
	
	mp3timer = CLOCK_RegisterCountdowntimer();
	if ( mp3timer == CLOCK_FAILED ) return;
	
	THREAD_RegisterThread( mp3client_thread, PSTR("mp3-streaming"));
	printf_P( PSTR("Tsunami v0.2.0 MP3-Streamingengine gestartet \r\n$Id: mp3-streaming.c 534 2015-01-18 15:52:02Z sharan $.\r\n"));

#if defined(SHELL)
	SHELL_RegisterCMD( cmd_mp3stream ,PSTR("stream") );
#endif

}

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Schick die MP3-Daten zum VS10xx
 * \param 	NONE
 * \return	NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
void mp3client_stream( int socket, int fifo )
{		
	int vs10xx_fifo = streambuffer_getfifo();
	int len;

	// größe für den Transfer ermitteln
	len = Get_FIFOrestsize( vs10xx_fifo );

	if ( Get_Bytes_in_FIFO( fifo ) <= len )
	{
		len = Get_Bytes_in_FIFO( fifo ) ;
	}
	
	// sind metainformationen mit dabei ? Wenn ja größe bis zur nächsten metainfo ermitteln
	if( metainfo == 1 )
	{
		if ( ( metaintcounter + len ) >= metaint )
		{
			len = metaint - metaintcounter;
		}
	}

	metaintcounter = metaintcounter + len;

	// Daten kopieren und in den VS10xx-buffer kopieren
	streambuffer_fillfifo( fifo, len);
	
	if ( metainfo == 1 )
	{
		// Metainfo erreicht ?
		if( metaintcounter == metaint )
		{
			if ( Get_Bytes_in_FIFO( fifo ) > ( Read_Byte_ahead_FIFO( fifo, 0 ) * 16 ) )
			{
				mp3client_readmetainfo( fifo );
				metaintcounter = 0;
			}
		}
	}
	streambuffer_setsource( stream_http );
}

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Die MP3-streamingengine, sie sollte in einer Mainloop in regelmässigen Abständen aufgerufen werden,
 * wobei die Intervalle nicht zu knapp gewählt werden sollten, damit sie noch richtig arbeitet.
 * \param 	NONE
 * \return	NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
void mp3client_thread( void )
{	

}

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Diese Funktion dient zum Starten des MP3-Stream, damit diese dem stream dann im mp3-client_thread abarbeiten
 * kann.
 * \param 	streamingPROTO	Pointer auf einen String für das Protokoll, Standard ist http.
 * \param	streamingURL	Pointer auf den DNS-namen oder die IP als String.
 * \param	streamingIP		Die IP-Adresse des Streamingserver(shoutcastserver).
 * \param	streamingPORT	Der TCP-Port auf dem der Streamingserver angesprochen werden soll.
 * \param	streamingFILE	Pointer der auf einen String zeigt der das File auf den Streamingserver zeigt.
 * \param	socket			TCP-handle um Parameter der Aushandlung auf den TCP-Socket auszugeben ( z.B. Handle einer offenen Telnetsession )
 * \return	NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
void mp3client_startstreaming( char * streamingPROTO, char * streamingURL, long streamingIP, int streamingPORT, char * streamingFILE )
{
	
	unsigned int TEMP_MP3_SOCKET = SOCKET_ERROR;
	int REQUESTCODE;
	struct STDOUT oldSTDOUT;

	STDOUT_save( &oldSTDOUT );

	weakstream = 2;
	
	// mindestens IP und PORT gesetzt ?
	if ( streamingIP == 0  || streamingPORT == 0 ) return;

	// schon ein stream offen ? Wenn ja killen
	if ( MP3_SOCKET != SOCKET_ERROR )
	{
		printf_P( PSTR("Highjack current stream and kill them on Socket %d\r\n"), MP3_SOCKET );
		mp3client_stopstreaming ( );
	}
	printf_P( PSTR("Connect to %s"), streamingURL );
	
	// neue Verbindung auf machen
	TEMP_MP3_SOCKET = Connect2IP( streamingIP , streamingPORT );
				
	// neue Verbindung geöffnet ?
	if ( TEMP_MP3_SOCKET != SOCKET_ERROR )
	{
		printf_P( PSTR(" connected ;-D\r\n"), streamingURL );
	}
	else
	{
		printf_P( PSTR(" fail to connect ;-(\r\n"), streamingURL );
	}
	
	STDOUT_Flush();
	
	// Wenn Verbindung geöffnet, weiter weiter :-)
	if ( TEMP_MP3_SOCKET != SOCKET_ERROR )
	{
		metainfo = 1;
		STDOUT_set( _TCP, TEMP_MP3_SOCKET );
		// Request senden
		printf_P( PSTR("GET /%s HTTP/1.1\r\n" \
					   "Host: %s\r\n" \
					   "User-Agent: mp3-streamingclient v0.1a (ccc-berlin)\r\n" \
					   "Icy-MetaData: %d\r\n" \
					   "Keep-Alive: \r\n" \
					   "Connection: TE, Keep-Alive\r\n" \
					   "TE: trailers\r\n\r\n" ), streamingFILE , streamingFILE , metainfo );

		STDOUT_restore( &oldSTDOUT );

		printf_P( PSTR("Request on TCP-Socket %d\r\n"), TEMP_MP3_SOCKET );
		
		// Stream setzen
		MP3_SOCKET = TEMP_MP3_SOCKET;
						
		STDOUT_set( NONE, 0 );
		
		REQUESTCODE = mp3client_pharseheader( MP3_SOCKET );

		STDOUT_restore( &oldSTDOUT );
		
		if ( REQUESTCODE != 200 && REQUESTCODE != 0 )
		{
			switch ( REQUESTCODE )
			{
				case -1:		printf_P( PSTR("Timeout:") );
								break;
				case 400:		printf_P( PSTR("Requestcode: %d Bad Request:"), REQUESTCODE );
								break;
				case 401:		printf_P( PSTR("Requestcode: %d Unauthorized:"), REQUESTCODE );
								break;
				case 403:		printf_P( PSTR("Requestcode: %d Forbidden:"), REQUESTCODE );
								break;
				case 404:		printf_P( PSTR("Requestcode: %d Not Found:"), REQUESTCODE );
								break;
				case 405:		printf_P( PSTR("Requestcode: %d Method Not Allowed:"), REQUESTCODE );
								break;
				case 500: 		printf_P( PSTR("Requestcode: %d Internal Server Error:"), REQUESTCODE );			
								break;
				default:		printf_P( PSTR("Requestcode: %d Unknow:"), REQUESTCODE );			
								break;
			}
			
			printf_P( PSTR("Stream failed\r\n") );
			STDOUT_Flush();
			MP3_SOCKET = SOCKET_ERROR;
			return;
		}
		// wenn alles schick mit dem stream dann:
		printf_P( PSTR("Metaint: %u\r\n"), metaint );
		printf_P( PSTR("Requestcode: %d - Stream okay, Tsunami can highjack them totaly\r\n"), REQUESTCODE );
	}
	metaintcounter = 0;
	RegisterTCPCallBack( MP3_SOCKET , mp3client_stream );
	
	STDOUT_Flush();

}

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Hält einen gerade abgespielten MP3-Stream an.
 * \param	socket			TCP-handle um Parameter der Aushandlung auf den TCP-Socket auszugeben ( z.B. Handle einer offenen Telnetsession )
 * \return	NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
void mp3client_stopstreaming( void )
{
	// stream offen ?
	if ( MP3_SOCKET != SOCKET_ERROR )
		CloseTCPSocket ( MP3_SOCKET );

	// Meldung ausgeben
	printf_P( PSTR("Connection closed.\r\n") );
	
	STDOUT_Flush();
	
	// socket ungültig setzen
	MP3_SOCKET = SOCKET_ERROR;
}

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Toggelt das Verboselevel auf der RS232.
 * \param	NONE
 * \return	level	Das Verboselevel 0 oder 1.
 */
/*------------------------------------------------------------------------------------------------------------*/
unsigned char mp3client_setmetainfo( void )
{
	// Wenn verbose auf 0 dann auf 1 setzen, sonst umgekehrt
	if ( metainfo == 0 )
		metainfo = 1;
	else
		metainfo = 0;
	
	// Level zurückgeben
	return( metainfo );
}

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Toggelt das Verboselevel auf der RS232.
 * \param	NONE
 * \return	level	Das Verboselevel 0 oder 1.
 */
/*------------------------------------------------------------------------------------------------------------*/
void mp3client_autoplay( char autoplay )
{
	autoplay = autoreplay;
}

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Toggelt das Verboselevel auf der RS232.
 * \param	NONE
 * \return	level	Das Verboselevel 0 oder 1.
 */
/*------------------------------------------------------------------------------------------------------------*/
unsigned char mp3client_setverboselevel( void )
{
	// Wenn verbose auf 0 dann auf 1 setzen, sonst umgekehrt
	if ( verboselevel == 0 )
		verboselevel = 1;
	else
		verboselevel = 0;
	
	// Level zurückgeben
	return( verboselevel );
}

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Leist den HTTP-header.
 * \param	socket	Der TCP-Socket von wo die HTTP-header lesen werden soll und speichert Infos zum Stream.
 * \return	REQUEST Der Returncode auf dem HTTP-header.
 */
/*------------------------------------------------------------------------------------------------------------*/
int mp3client_pharseheader ( unsigned int socket )
{
	char pharsebuffer[256], Data;
	int pharsebufferpos=0;
	int REQUEST = 0;

	metaint = 0;

	if ( verboselevel != 0 ) printf_P( PSTR("\r\n\r\nGet Headerinformation\r\n---------------------\r\n") );

	while( 1 )
	{

		while ( GetBytesInSocketData( MP3_SOCKET ) == 0 && CLOCK_GetCountdownTimer( mp3timer ) != 0)
		{
			if ( CLOCK_GetCountdownTimer( mp3timer ) == 0 )
				return( -1 );			
		}
		
		Data = GetByteFromSocketData ( socket );
 		if ( verboselevel != 0 ) printf_P( PSTR("%c"), Data);
		
		if ( CLOCK_GetCountdownTimer( mp3timer ) == 0 )
			return( -1 );
		
		if ( Data != 0x0a )
		{
			if ( pharsebufferpos < 255 )
			{
				if ( Data != 0x0d )
				{
					pharsebuffer[ pharsebufferpos++ ] = Data;
					pharsebuffer[ pharsebufferpos ] = '\0';
				}
			}
			if ( Data == 0x0d )
			{
				if ( !memcmp( &pharsebuffer[0] , "ICY" , 3 ) )
				{
					REQUEST = atoi( &pharsebuffer[4] );
					if ( REQUEST != 200 ) return( REQUEST );
				}
				else if ( !memcmp( &pharsebuffer[0] , "HTTP/1.1" , 8 ) )
				{
					REQUEST = atoi( &pharsebuffer[9] );
					if ( REQUEST != 200 ) return( REQUEST );
				}
				else if ( !memcmp( &pharsebuffer[0] , "icy-metaint:" , 11 ) )
				{
					metaint = atoi( &pharsebuffer[12] );
/*					if ( metaint == -32768 )
					{
						metaint = 0;
						metainfo = 0;
					}
*/				}
				else if ( !memcmp( &pharsebuffer[0] , "icy-name" , 8 ) )
				{
					mp3clientupdateNAME( &pharsebuffer[10] );
				}
				else if ( !memcmp( &pharsebuffer[0] , "icy-url" , 7 ) )
				{
					mp3clientupdateMETAURL( &pharsebuffer[9] );
				}
				else if ( !memcmp( &pharsebuffer[0] , "icy-genre" , 9 ) )
				{
					mp3clientupdateGENRE( &pharsebuffer[11] );
				}
				else if ( !memcmp( &pharsebuffer[0] , "icy-br" , 6 ) )
				{
					mp3clientupdateBITRATE( atoi( &pharsebuffer[8] ) );
				}

				if ( pharsebuffer[0] == '\0' )
				{
					Data = GetByteFromSocketData ( socket );
					break;
				}
				else
				{
					pharsebufferpos = 0;
					pharsebuffer[0] = '\0';
				}
					
			}
		}
	}
	
	if ( verboselevel != 0 ) printf_P( PSTR("----------\r\nHeader End\r\n") );

	return( REQUEST );
}

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Liest die Metainfo aus dem Stream aus und speichert ihn ab.
 * \param	socket		Der TCP-Socket von wo aus die Metainfo gelesen werden soll
 * \param	metainfolen	Länge der Metainfo in Bytes.
 * \return	NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
void mp3client_readmetainfo( int fifo )
{
	char data,buffer[257];
	int i,x=0;
	long metainfolenght;

	metainfolenght = Get_Byte_from_FIFO( fifo ) * 16;
	
/*	if ( metainfolenght < 256 )
	{
		char *title,*URL;

		Get_Block_from_FIFO( fifo , metainfolenght , buffer );

		 buffer[256] = '\0';
		data = buffer;
		title = data;

		while( *data != ';')
		{
			data++;
		}

		*data = '\0';
		data++;
		URL = data;

		data = title;
		while( *data != '=' )
		{
			data++;
		}
		*data = '\0';
		data++;

		if ( strcmp_P( title , PSTR("StreamTitle") ) == 0 )
		{
			mp3clientupdateMETATITLE( data );
			streaminfo_updatetitle( data );
		}

		data = URL;
		while( *data != '=' )
		{
			data++;
		}
		*data = '\0';
		data++;

		if ( strcmp_P( URL, PSTR("StreamURL") ) == 0 )
		{
			mp3clientupdateMETAURL( data );
			streaminfo_updatesource( data );
		}

	}
	else
	{
		for( i = 0 ; i < metainfolenght ; i++ )
		{
			Get_Byte_from_FIFO( fifo );
		}
	}
*/
	
	weakstream = 0;

	if ( verboselevel != 0 ) printf_P( PSTR("\r%c[2K"),27);
	
	for ( i = 0 ; i < metainfolenght ; i++ )
	{
		data = Get_Byte_from_FIFO( fifo );
		if ( verboselevel != 0 ) printf_P( PSTR("%c"), data );

		// Info auseinander bauen
		if ( data == ';' )
		{
			buffer[x] = '\0';
			if ( !memcmp( &buffer[0] , "StreamTitle=" , 11 ) )
			{
				if ( strlen( &buffer[12] ) != 0 )
				{	
					mp3clientupdateMETATITLE( &buffer[12] );
					streaminfo_updatetitle( &buffer[12] );
				}
				else
				{
					weakstream++;
					streaminfo_updatetitle("unknown");
				}
			}
			if ( !memcmp( &buffer[0] , "StreamUrl=" , 9 ) )
			{
				if ( strlen( &buffer[10] ) != 0 )
				{
					mp3clientupdateMETAURL( &buffer[10] );
					streaminfo_updatesource( &buffer[10] );
				}
				else
				{
					weakstream++;
					streaminfo_updatesource("shoutcast");
				}
			}
			x = 0;
		}
		else if ( data == '\'' )
		{
		}
		else
		{
			if ( x < 256 )
			{
				buffer[x] = data;
				buffer[x+1] = '\0';
				x++;
			}
		}
	}
	if ( verboselevel != 0 ) printf_P( PSTR("\r\n"));

}
#endif

//@}
