/***************************************************************************
 *            udp-stream.c
 *
 *  Mon Nov 28 19:37:36 2011
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


/**
 * \ingroup software
 * \addtogroup udp_stream MP3-streaming über UDP. (udp_stream.c)
 *
 * Mit diesen Treiber kann der VS10xx per UDP angesprochen werden.
 *
 * @{
 */

/**
 * \file
 *
 * \author Dirk Broßwick
 */

#include <avr/pgmspace.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include "system/net/udp.h"
#include "system/thread/thread.h"
#include "system/net/endian.h"

#include "config.h"

#if defined(VS10XX)

#include "udp-stream.h"
#include "streaminfo.h"
#include "hardware/vs10xx/vs10xx_buffer.h"

static char udp_streambuffer[1];
static int udp_mp3_socket = UDP_SOCKET_ERROR;

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Init um den Streamingdienst zu starten.
 * \return	NONE		None
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
void udp_stream_init( void )
{		
	// Register UDP-Socket
	udp_mp3_socket = UDP_ListenOnPort( UDP_STREAM_PORT, 0, udp_streambuffer );
	// Register UDP-Callback
	UDP_RegisterCallback( udp_mp3_socket, udp_stream_callback );
	printf_P(PSTR("UDP-Stream start\r\n"));
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Die UDP-Callback für eingehende Packete
 * \param 	buffer		Pointer auf den Buffer.
 * \paran	len			Länge der im Buffer befindlichen Daten.
 * \return	NONE		None
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
void udp_stream_callback( char * buffer, int len )
{

	static long sequencenumber = 0;
	
	struct STREAM_PACKET *stream_packet;
	stream_packet = (struct STREAM_PACKET *) buffer;

	switch( stream_packet->packet_type )
	{
		case	data:		
							{
								struct STREAM_DATAPACKET *stream_datapacket;
								stream_datapacket = (struct STREAM_DATAPACKET *) stream_packet->data;

								sequencenumber = ntohl(stream_datapacket->sequencenumber );
								streambuffer_fill( stream_datapacket->data, ntohs( stream_datapacket->len ) );
								streambuffer_setsource( stream_udp );
								break;
							}
		case	info:
							{
								struct STREAM_INFOPACKET *stream_infopacket;
								stream_infopacket = (struct STREAM_INFOPACKET *) stream_packet->data;

								switch( stream_infopacket->infotype )
								{
									case	title:	streaminfo_updatetitle( stream_infopacket->data );
													break;
									case	source:	streaminfo_updatesource( stream_infopacket->data );
													break;
									default:		break;
								}
								break;
							}
		case	init:
							{
								struct STREAM_INITPACKET *stream_initpacket;
								stream_initpacket = (struct STREAM_INITPACKET *) stream_packet->data;
								sequencenumber = 0;
								
								break;
							}
		case 	relay:
							{
								UDP_SendPacket( udp_mp3_socket, len, buffer );
								break;
							}
		default:			break;
	}


//	streambuffer_fill( buffer, len );
}

#endif // VS10XX

/**
 * @}
**/
