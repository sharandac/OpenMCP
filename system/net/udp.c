/***************************************************************************
 *            udp.c
 *
 *  Mon Jul 31 21:46:47 2006
 *  Copyright  2006  Dirk Broßwick
 *  Email: sharandac@snafu.de
 *
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
 * \ingroup network
 * \addtogroup UDP Der UDP Stack fuer Mikrocontroller (udp.c)
 * \code #include "udp.h" \endcode
 *
 * \par Uebersicht
 * Der UDP-Stack für Mikrocontroller. Behandelt komplett den UDP-Stack
 * mit Verbindungsaufbau und Abbau.
 *
 * @{
 */

/**
 *
 * \author Dirk Broßwick
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "config.h"
#include "system/math/math.h"
#include "arp.h"
#include "ethernet.h"
#include "ip.h"
#include "udp.h"
#include "endian.h"

struct UDP_SOCKET UDP_sockettable[ MAX_UDP_CONNECTIONS ];
// struct UDP_SOCKET * UDP_socket;

/* -----------------------------------------------------------------------------------------------------------*/
/*! Hier findet die Initialisierung des UDP-Stack statt um einen definiertenausgangzustand zu haben.
 * \retval  NONE
 */
/* -----------------------------------------------------------------------------------------------------------*/
void UDP_init( void )
{
	int i;
	
	for( i = 0 ; i < MAX_UDP_CONNECTIONS ; i++ )
		UDP_CloseSocket( i );
}
	
/* -----------------------------------------------------------------------------------------------------------*/
/*! Hier findet die Bearbeitung des Packetes statt welches ein UDP-Packet enthaelt. Es wird versucht, die 
 * Verbindung zuzuordnen, wenn dies nicht moeglich ist, wird hier abgebrochen.
 * Danach wird der Inhalt dem Socket zugeordnet und Daten in den Puffer des Benutzers kopiert.
 * \warning Zu lange UDP-Packete werden abgeschnitten.
 * \param 	packet_lenght	Gibt die Packetgroesse in Byte an die das Packet lang ist.
 * \param	ethernetbuffer	Zeiger auf das Packet.
 * \return  NONE
 */
/* -----------------------------------------------------------------------------------------------------------*/
void udp( int packet_lenght, char * ethernetbuffer )
{
	int i, socket , Offset;

	struct IP_header * IP_packet;		// IP_struct anlegen
	IP_packet = ( struct IP_header *) &ethernetbuffer[ETHERNET_HEADER_LENGTH];
	struct UDP_header * UDP_packet;		// TCP_struct anlegen
	UDP_packet = ( struct UDP_header *) &ethernetbuffer[ETHERNET_HEADER_LENGTH + ((IP_packet->IP_Version_Headerlen & 0x0f) * 4 )];

#ifdef _DEBUG_
	printf_P( PSTR("UDP-Packet empfangen") , packet_lenght );
#endif

	socket = UDP_GetSocket( ethernetbuffer );

	// Socket zugeordnet ?
	if ( socket != UDP_SOCKET_ERROR )
	{
		// Hat Socket Daten ?
		if ( ( ntohs( UDP_packet->UDP_Datalenght ) - UDP_HEADER_LENGHT ) != 0 )
		{
			// Offset für UDP-Daten im Ethernetframe berechnen
			Offset = ETHERNET_HEADER_LENGTH + ((IP_packet->IP_Version_Headerlen & 0x0f) * 4 ) + UDP_HEADER_LENGHT;

			if( UDP_sockettable[ socket ].UDPcallback != NULL )
			{
				// ruf mal die Callback auf und übergebe den Puffer und deren länge
				UDP_sockettable[ socket ].UDPcallback( &ethernetbuffer[ Offset ], ntohs( UDP_packet->UDP_Datalenght ) - UDP_HEADER_LENGHT );
				UDP_sockettable[ socket ].Socketstate = UDP_SOCKET_READY;
				UDP_sockettable[ socket ].DestinationPort = 0 ;
				UDP_sockettable[ socket ].DestinationIP = 0 ;
				UDP_sockettable[ socket ].Bufferfill = 0;
			}
			else
			{
				// Socket schon mit daten belegt ?
				if ( UDP_sockettable[ socket ].Socketstate == UDP_SOCKET_READY )
				{
					// Größe der Daten limitieren auf Puffergröße
					i = MIN( ntohs( UDP_packet->UDP_Datalenght ) - UDP_HEADER_LENGHT , UDP_sockettable[ socket ].Bufferlenght );
					// Größe der Daten eintragen
					UDP_sockettable[ socket ].Bufferfill = i;
					// Socket als belegt melden
					UDP_sockettable[ socket ].Socketstate = UDP_SOCKET_BUSY;
					// Daten kopieren, aber flott
					memcpy( UDP_sockettable[ socket ].Recivebuffer , &ethernetbuffer[ Offset ] , i );
					/*
					 while ( i-- )
					 {
						 UDP_sockettable[ socket ].Recivebuffer[i] = ethernetbuffer[ Offset + i ];
				}
				*/
				}
			}
		}
	}
	return;
}

/* -----------------------------------------------------------------------------------------------------------*/
/*!\brief Registriert ein Socket in den die Daten fuer ein Verbindung gehalten werden um die ausgehenden und einghenden UDP-Packet zuzuordnen.
 * \param 	IP					Die IP-Adresse des Zielhost.
 * \param	DestinationPort		Der Zielpot des Zielhost mit den verbunden werden soll. Der Sourcport wird automatisch eingestellt. Zu beachten ist das bei einer Verbindn zu Port 67 der Sourceport auf 68 eingestellt wird.
 * \param	Bufferlenght		Groesse des Datenpuffer der vom Benutzer bereitgestellt wird. Hier werden die eingegenden UDP-Daten kopiert. Dieser Puffer sollte entsprechend der Verwendung dimensioniert sein.
 * \param	UDP_Recivebuffer	Zieger auf den Puffer der vom Benutzer bereitgestellt wird.
 * \return  Beim erfolgreichen anlegen eines Socket wird die Socketnummer zurueck gegeben. Im Fehlerfall 0xffff.
 */
/* -----------------------------------------------------------------------------------------------------------*/
int UDP_RegisterSocket( long IP, unsigned int DestinationPort, int Bufferlenght, char * UDP_Recivebuffer)
{
//	struct IP_CONFIG ip_config;
//	ip_getconfig( &ip_config );
	
	int socket,i;
	char temp;
	
	temp = SREG;
	cli();
	
	socket = UDP_Getfreesocket();

	if ( socket != UDP_SOCKET_ERROR )
	{
		UDP_sockettable[socket].Socketstate = UDP_SOCKET_READY;
		UDP_sockettable[socket].DestinationPort = DestinationPort ;
		// wenn Zielport Bootps(67) dann Sourceport auf Bootpc(68) setzen um kommunikation mit DHCP-Server zu ermöglichen
		if ( DestinationPort == 67 )
			UDP_sockettable[socket].SourcePort = 68 ;
		else
			UDP_sockettable[socket].SourcePort =~ DestinationPort;

		UDP_sockettable[socket].DestinationIP = IP;
		UDP_sockettable[socket].Bufferfill = 0;
		UDP_sockettable[socket].Bufferlenght = Bufferlenght;
		UDP_sockettable[socket].Recivebuffer = UDP_Recivebuffer;
		UDP_sockettable[socket].UDPcallback = NULL;

		// if an broadcast ?
		if ( IP == 0xffffffff ) 
		{
			for( i = 0 ; i < 6 ; i++ )
			{
				UDP_sockettable[socket].MACadress[i] = 0xff;
			}
		}
		// need MAC-address ?
//		else if ( IS_ADDR_IN_MY_SUBNET( IP, ip_config.Netmask ) )
		else if ( IS_ADDR_IN_MY_SUBNET( IP, Netmask ) )
		{
			if ( IS_BROADCAST_ADDR( IP, Netmask ) )
			{
				for( i = 0 ; i < 6 ; i++ )
				{
					UDP_sockettable[socket].MACadress[i] = 0xff;
				}
			}
			else
			{
				SREG = temp;
				GetIP2MAC( IP, UDP_sockettable[socket].MACadress );
				temp = SREG;
				cli();
			}
		}
#if defined(MULTICAST)
		// Check if we got a multicast address.
		// Remember the IP address is already in network byte order, so we check
		// the lowest byte for the multicast IP address bits.
		else if ( (IP & 0xf0) == 0xe0 )
		{
			// Yes, we have an IP multicast, set MAC to multicast.
			// First chop the IP address into bytes.
			union IP_ADDRESS IPnum;
			IPnum.IP = IP;

			UDP_sockettable[socket].MACadress[0] = 0x01;
			UDP_sockettable[socket].MACadress[1] = 0x00;
			UDP_sockettable[socket].MACadress[2] = 0x5e;

			// Conforming to the rules the 24th bit must be set to 0.
			// The remaining 23 bits are mapped as they are.
			UDP_sockettable[socket].MACadress[3] =  IPnum.IPbyte[1] & 0x7f;
			UDP_sockettable[socket].MACadress[4] =  IPnum.IPbyte[2];
			UDP_sockettable[socket].MACadress[5] =  IPnum.IPbyte[3];
		}
#endif
		else
		{
			SREG = temp;
			GetIP2MAC( Gateway, UDP_sockettable[socket].MACadress );
			temp = SREG;
			cli();
		}
	}
	
	SREG = temp;
	return( socket );
}

/* -----------------------------------------------------------------------------------------------------------*/
/*!\brief Registriert ein ListenSocket der auf einen bestimmten Port lauschen tut.
 * \param	Port				Der Port auf den gelauscht werden soll.
 * \param	Bufferlenght		Groesse des Datenpuffer der vom Benutzer bereitgestellt wird. Hier werden die eingegenden UDP-Daten kopiert. Dieser Puffer sollte entsprechend der Verwendung dimensioniert sein.
 * \param	UDP_Recivebuffer	Zieger auf den Puffer der vom Benutzer bereitgestellt wird.
 * \return  Beim erfolgreichen anlegen eines ListenSocket wird die Socketnummer zurueck gegeben. Im Fehlerfall 0xffff.
 */
/* -----------------------------------------------------------------------------------------------------------*/
int UDP_ListenOnPort( unsigned int Port, int Bufferlenght, char * UDP_Recivebuffer)
{
	int socket;
	char temp;
	
	temp = SREG;
	cli();

	socket = UDP_Getfreesocket();

	if ( socket != UDP_SOCKET_ERROR )
	{
		UDP_sockettable[socket].Socketstate = UDP_SOCKET_READY;
		UDP_sockettable[socket].DestinationPort = 0 ;
		UDP_sockettable[socket].SourcePort = Port ;
		UDP_sockettable[socket].DestinationIP = 0 ;
		UDP_sockettable[socket].Bufferfill = 0;
		UDP_sockettable[socket].Bufferlenght = Bufferlenght;
		UDP_sockettable[socket].Recivebuffer = UDP_Recivebuffer;
		UDP_sockettable[socket].UDPcallback = NULL;
	}
	
	SREG = temp;
	return( socket );
}

/* -----------------------------------------------------------------------------------------------------------*/
/*!\brief Sendet ein UDP-Packet an einen Host.
 * \code
 * // ein Simples Beispiel für das Senden einen UDP-Packetes
 * #define Bufferlen 100
 * #define DestinationPort 53
 * #define IP 0x12345678
 *
 * unsigned char buffer[ Bufferlen ];
 * unsigned int socket;
 *
 * // Socket Registrieren
 * socket = UDP_RegisterSocket( IP, DestinationPort, Bufferlen, buffer );
 * // Socket erfolgreich geoeffnet ?
 * if ( socket == 0xffff ) 
 *           return( error );
 * // UDP-Packet senden
 * UDP_SendPacket( socket , Bufferlen, buffer );
 * // Socket schliessen
 * UDP_CloseSocket( socket );
 * \endcode
 * \param 	socket			Die Socketnummer ueber die das Packet gesendet wird.
 * \param	Datalenght		Gibt die Datenlaenge der Daten in Byte an die gesendet werden sollen.
 * \param	UDP_Databuffer  Zeifer auf den Datenpuffer der gesendet werden soll.
 * \return  Bei einem Fehler beim versenden wird ungleich 0 zurueckgegeben, sonst 0.
 * \sa UDP_RegisterSocket , UDP_GetSocketState
 */
/* -----------------------------------------------------------------------------------------------------------*/
int UDP_SendPacket( int socket, int Datalenght, char * UDP_Databuffer )
{
	if ( socket >= MAX_UDP_CONNECTIONS ) return( UDP_SOCKET_ERROR );
		
	LockEthernet();
	
	char * ethernetbuffer;
	ethernetbuffer = (char*) __builtin_alloca (( size_t ) ETHERNET_HEADER_LENGTH + IP_HEADER_LENGHT + UDP_HEADER_LENGHT + Datalenght );

	struct IP_header * IP_packet;		// IP_struct anlegen
	IP_packet = ( struct IP_header *) &ethernetbuffer[ETHERNET_HEADER_LENGTH];
	IP_packet->IP_Version_Headerlen = 0x45; // Standart IPv4 und Headerlenght 20byte
		
	int offset = ETHERNET_HEADER_LENGTH + ((IP_packet->IP_Version_Headerlen & 0x0f) * 4 ) + UDP_HEADER_LENGHT;

	memcpy( &ethernetbuffer[ offset ], UDP_Databuffer, Datalenght );
//	for ( i = 0 ; i < Datalenght ; i++ ) ethernetbuffer[ offset + i ] = UDP_Databuffer[ i ];
	
	MakeIPheader( UDP_sockettable[socket].DestinationIP, PROTO_UDP, UDP_HEADER_LENGHT + Datalenght , ethernetbuffer );
	MakeUDPheader( socket, Datalenght, ethernetbuffer );
	MakeETHheader( UDP_sockettable[socket].MACadress, ethernetbuffer );
	sendEthernetframe( ETHERNET_HEADER_LENGTH + IP_HEADER_LENGHT + UDP_HEADER_LENGHT + Datalenght, ethernetbuffer);

	FreeEthernet();
	
	return(0);
}

/* -----------------------------------------------------------------------------------------------------------*/
/*!\brief Gibt den Socketstatus aus.
 * \param	socket	Die Socketnummer vom abzufragen Socket.
 * \return  Den Socketstatus.
 */
/* -----------------------------------------------------------------------------------------------------------*/
int UDP_GetSocketState( int socket )
	{
		int state = 0;
		char temp;
		
		if ( socket >= MAX_UDP_CONNECTIONS )
			return( UDP_SOCKET_ERROR );

		temp = SREG;
		cli();

		state = UDP_sockettable[ socket ].Socketstate;

		SREG = temp;
		
		return( state );
	}

/* -----------------------------------------------------------------------------------------------------------*/
/*!\brief Gibt die Anzahl der Byte aus die sich im Puffer befinden. Diese Abfrage macht nur sinn in Verbindung mit
 * UDP_GetSocketState nachdem ein UDP-Packet empfangen worden ist und der Status fuer das auf UDP_SOCKET_BUSY steht.
 * Danach werden bis zur Freigabe durch UDP_FreeBuffer keine Daten auf den Socket mehr angenommen
 * \param	socket		Die Socketnummer vom abzufragen Socket.
 * \return  Anzahl der Byte im Puffer.
 *\sa UDP_GetSocketState, UDP_FreeBuffer
 */
/* -----------------------------------------------------------------------------------------------------------*/
int UDP_GetByteInBuffer( int socket )
	{
		
		char temp;
		int size=0;

		if ( socket >= MAX_UDP_CONNECTIONS )
			return( UDP_SOCKET_ERROR );
		
		temp = SREG;
		cli();

		size = UDP_sockettable[ socket ].Bufferfill;
		
		SREG = temp;
		
		return ( size );
	}

/* -----------------------------------------------------------------------------------------------------------*/
/*!\brief Gibt den UDP-Puffer wieder zum empfang frei. Danach werden wieder UDP-Daten angenommen und in den Puffer kopiert.
 * \param	socket		Die Socketnummer die freigegeben werden soll.
 * \return	NONE
 */
/* -----------------------------------------------------------------------------------------------------------*/
int UDP_RegisterCallback( int socket, UDP_Callback pFunc )
	{
		char temp;

		if ( socket >= MAX_UDP_CONNECTIONS ) return( UDP_SOCKET_ERROR );

		temp = SREG;
		cli();
		
		UDP_sockettable[ socket ].UDPcallback = pFunc;

		SREG = temp;

		return( 0 );
	}
	
/* -----------------------------------------------------------------------------------------------------------*/
/*!\brief Gibt den UDP-Puffer, welche für einer UDP-Verbindung zugeordnet ist, wieder zum empfang frei.
 * \param	socket		Die Socketnummer die freigegeben werden soll.
 * \return	NONE
 */
/* -----------------------------------------------------------------------------------------------------------*/
int UDP_FreeBuffer( int socket )
	{
		char temp;

		if ( socket >= MAX_UDP_CONNECTIONS ) return( UDP_SOCKET_ERROR );

		temp = SREG;
		cli();
		
		UDP_sockettable[ socket ].Bufferfill = 0;
		UDP_sockettable[ socket ].Socketstate = UDP_SOCKET_READY;

		SREG = temp;

		return( 0 );
	}
	
/* -----------------------------------------------------------------------------------------------------------*/
/*!\brief Gibt den UDP-Puffer, welcher keiner UDP-Verbindung zugeordnet ist, wieder zum empfang frei.
 * \param	socket		Die Socketnummer die freigegeben werden soll.
 * \return	NONE
 */
/* -----------------------------------------------------------------------------------------------------------*/
int UDP_BackToListen( int socket )
	{
		char temp;

		if ( socket >= MAX_UDP_CONNECTIONS ) return( UDP_SOCKET_ERROR );

		temp = SREG;
		cli();
		
		UDP_sockettable[ socket ].Bufferfill = 0;
		UDP_sockettable[ socket ].Socketstate = UDP_SOCKET_READY;
		UDP_sockettable[ socket ].DestinationPort = 0 ;
		UDP_sockettable[ socket ].DestinationIP = 0 ;

		SREG = temp;

		return( 0 );
	}

/* -----------------------------------------------------------------------------------------------------------*/
/*!\brief Gibt das Socket wieder frei und beendet die Verbindung.
 * Alle UDP-Packet die dann von diesen Socket empfangen werden, werden verworfen.
 * \param	socket		Die Socketnummer die geschlossen werden soll.
 * \return	Es wird beim erfolgreichen schliessen der Socket 0 zurueck gegeben, sonst 0xffff.
 */
/* -----------------------------------------------------------------------------------------------------------*/
int UDP_CloseSocket( int socket )
	{
		char temp;

		if ( socket >= MAX_UDP_CONNECTIONS ) return( UDP_SOCKET_ERROR );
			
		temp = SREG;
		cli();

		if ( UDP_sockettable[socket].Socketstate != UDP_SOCKET_NOT_USE )
		{
			UDP_sockettable[socket].Socketstate = UDP_SOCKET_NOT_USE;
			UDP_sockettable[socket].DestinationIP = 0;
			UDP_sockettable[socket].DestinationPort = 0;
			UDP_sockettable[socket].SourcePort = 0;
			UDP_sockettable[socket].Bufferfill = 0;
			UDP_sockettable[socket].Bufferlenght = 0;
			UDP_sockettable[socket].Recivebuffer = NULL;
			UDP_sockettable[socket].UDPcallback = NULL;
		}
		
		SREG = temp;
		return( 0 );
	}

/* -----------------------------------------------------------------------------------------------------------*/
/*!\brief wählt den richtigen Socketeintrag setzt TCP_socket auf diesen eintrag
 */
/*------------------------------------------------------------------------------------------------------------*/
int UDP_GetSocket( char * ethernetbuffer )
	{
		struct ETH_header * ETH_packet; 		// ETH_struct anlegen
		ETH_packet = (struct ETH_header *) ethernetbuffer;
		struct IP_header * IP_packet;		// IP_struct anlegen
		IP_packet = ( struct IP_header *) &ethernetbuffer[ETHERNET_HEADER_LENGTH];
		struct UDP_header * UDP_packet;		// TCP_struct anlegen
		UDP_packet = ( struct UDP_header *) &ethernetbuffer[ETHERNET_HEADER_LENGTH + ((IP_packet->IP_Version_Headerlen & 0x0f) * 4 )];

		int socket = UDP_SOCKET_ERROR,i;
		char temp;

		temp = SREG;
		cli();

		for ( i = 0 ; i < MAX_UDP_CONNECTIONS ; i++ ) 
		{
			if ( UDP_sockettable[ i ].SourcePort == ntohs( UDP_packet->UDP_DestinationPort ) )
			{
				if ( UDP_sockettable[ i ].DestinationPort == ntohs( UDP_packet->UDP_SourcePort ) )
				{
					socket = i;
					break;
				}
				else if ( UDP_sockettable[ i ].DestinationPort == 0 && UDP_sockettable[ i ].DestinationIP == 0 )
				{
					// Socket komplettieren
					UDP_sockettable[ i ].DestinationIP = IP_packet->IP_SourceIP;
					UDP_sockettable[ i ].DestinationPort = ntohs( UDP_packet->UDP_SourcePort );
					socket = i;
					memcpy( UDP_sockettable[ socket ].MACadress, ETH_packet->ETH_sourceMac , 6 );
					break;
				}
			}
		}
/*
		{
			 //Normal zuordnung
			if ( UDP_sockettable[ i ].DestinationPort == ntohs( UDP_packet->UDP_SourcePort )
			    && UDP_sockettable[ i ].SourcePort == ntohs( UDP_packet->UDP_DestinationPort )
			    && UDP_sockettable[ i ].DestinationIP == IP_packet->IP_SourceIP )
			{
				socket = i;
				break;
			}
			// BootP sonderstellung einräumen
			else if ( UDP_sockettable[ i ].DestinationPort == ntohs( UDP_packet->UDP_SourcePort )
			    && UDP_sockettable[ i ].SourcePort == ntohs( UDP_packet->UDP_DestinationPort ) )
			{
				socket = i;
				break;
			}
			// Ist Packet auf einen Listen-socket eingetroffen ?	
			else if ( UDP_sockettable[ i ].SourcePort == ntohs( UDP_packet->UDP_DestinationPort )
			    && UDP_sockettable[ i ].DestinationPort == 0
			    && UDP_sockettable[ i ].DestinationIP == 0 )
			{
				// Socket komplettieren
				UDP_sockettable[ i ].DestinationIP = IP_packet->IP_SourceIP;
				UDP_sockettable[ i ].DestinationPort = ntohs( UDP_packet->UDP_SourcePort );
				socket = i;
				memcpy( UDP_sockettable[ socket ].MACadress, ETH_packet->ETH_sourceMac , 6 );
				break;
			}
		}
*/
		SREG = temp;
		return( socket );
	}
	
/* -----------------------------------------------------------------------------------------------------------*/
/*!\brief Bastelt den UDP-header
 * \param	socket		Nummer des Socket
 * \param	Datalenght	Länge des Datanpacketes
 * \param	ethernetbuffer	Zeiger auf den Ethernetbuffer
 */
/*------------------------------------------------------------------------------------------------------------*/
int MakeUDPheader( int socket, int Datalenght, char * ethernetbuffer )
{
	if ( socket >= MAX_UDP_CONNECTIONS ) return( UDP_SOCKET_ERROR );
	
	struct IP_header * IP_packet;		// IP_struct anlegen
	IP_packet = ( struct IP_header *) &ethernetbuffer[ETHERNET_HEADER_LENGTH];
	struct UDP_header * UDP_packet;		// TCP_struct anlegen
	UDP_packet = ( struct UDP_header *) &ethernetbuffer[ETHERNET_HEADER_LENGTH + ((IP_packet->IP_Version_Headerlen & 0x0f) * 4 )];		
		
	UDP_packet->UDP_DestinationPort = htons( UDP_sockettable[socket].DestinationPort );
	UDP_packet->UDP_SourcePort = htons( UDP_sockettable[socket].SourcePort );
	UDP_packet->UDP_Checksum = 0;
	UDP_packet->UDP_Datalenght = htons( 8 + Datalenght );
	
	return(0);
}

/* -----------------------------------------------------------------------------------------------------------*/
/*!\brief Holt den nächsten freien Socket
 * \return	Gibt die Nummer eines Freien Socket zurück oder UDP_SOCKET_ERROR
 */
/*------------------------------------------------------------------------------------------------------------*/
int UDP_Getfreesocket( void )
	{
		int socket = UDP_SOCKET_ERROR, i;
		char temp;

		temp = SREG;
		cli();

		for ( i = 0 ; i < MAX_UDP_CONNECTIONS ; i++ ) 
		{
			if ( UDP_sockettable[ i ].Socketstate == UDP_SOCKET_NOT_USE )
			{
				socket = i;
				break;
			}
		}
		
		SREG = temp;
		return( socket );
	}

/**
 * @}
**/
