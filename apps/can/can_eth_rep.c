/***************************************************************************
 *            can.c
 *
 *  Tue Nov 30 01:11:46 2010
 *  Copyright  2010  sharan
 *  <sharan@<host>>
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
#include <stdio.h>
#include <avr/pgmspace.h>
#include "system/thread/thread.h"
#include "system/net/udp.h"
#include "system/net/ip.h"

#include "config.h"

#if defined(CAN)

#include "can_eth_rep.h"

static int CANETHREP_UDP_Socket;
static char CANETHREP_UDPBuffer[ CANETHREP_UDP_Bufferlen ];
static long CANETHREP_Counter = 0;

void CANETHERNET_Callback( char * Buffer, int Bufferlen )
{
	// tue was blödes
	CANETHREP_Counter++;
}

void CANETHREP_init( void )
{
	// Socket aufmachen
	CANETHREP_UDP_Socket = UDP_ListenOnPort( CANETHREP_UDPPORT, CANETHREP_UDP_Bufferlen, CANETHREP_UDPBuffer );
	printf_P( PSTR("CAN-Ethernet repeater started on port %u\r\n"), CANETHREP_UDPPORT);
	// Callback eintragen, wenn keine Callback einfach auskommentieren
	UDP_RegisterCallback( CANETHREP_UDP_Socket, CANETHERNET_Callback );
	// Thread eintragen
	THREAD_RegisterThread( CANETHREP_thread, PSTR("CAN-Ethernet repeater"));
}

void CANETHREP_thread( void )
{
	printf_P( PSTR("Counter: %ld\r"), CANETHREP_Counter );
	// printf_P( PSTR("sock state %x\r\n"), UDP_GetSocketState( UDP_Socket ) );
	if ( UDP_GetSocketState( CANETHREP_UDP_Socket ) == UDP_SOCKET_BUSY )
	{
		CANETHREP_Counter++;
		printf_P( PSTR("UDP ") );
		int i;
		for ( i = 0; i < UDP_GetByteInBuffer( CANETHREP_UDP_Socket ); i++ )
		{
			printf_P( PSTR("%x "), CANETHREP_UDPBuffer[i] );
		}
		printf_P( PSTR(" auf Socket %d"), CANETHREP_UDP_Socket );

		UDP_BackToListen( CANETHREP_UDP_Socket );

		printf_P( PSTR(" neuer Socket %d\r\n"), CANETHREP_UDP_Socket );
	}
}

#endif
