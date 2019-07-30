/***************************************************************************
 *            icmp.c
 *
 *  Sat Jun  3 18:53:45 2006
 *  Copyright  2006  Dirk Broßwick
 *  Email: sharandac@snafu.de
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
 * \ingroup network
 * \addtogroup ICMP Stellt Funktionen für ICMP/Ping bereit. (icmp.c)
 * \code #include "icmp.h" \endcode
 * \par Uebersicht
 *		Stellt Funktionen für ICMP/Ping bereit.
 * @{
 */

/**
 * \file
 *
 * \author Dirk Broßwick
 */

#include "arp.h"
#include "ethernet.h"
#include "ip.h"
#include "icmp.h"
#include "system/math/checksum.h"
#include "endian.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#if defined(__AVR_XMEGA__)
	#include <system/clock/delay_x.h>
#else
	#include "util/delay.h"
#endif

volatile char ICMP_Replaystate = ICMP_Unknow;

/*!\brief Verarbeitet ein ICMP-Packet
 * \param 	packet_lenght		Länge des ICMP-Packetes in Bytes
 * \param 	*buffer				Zeiger auf den Puffer mit dem ICMP-Packet
 * \return	NONE
 */
void icmp( int packet_lenght, char *buffer)
{
	int i;
	int checksum;

	struct ETH_header *ETH_packet; 		// ETH_struct anlegen
	ETH_packet = (struct ETH_header *)&buffer[0];
	struct IP_header *IP_packet;		// IP_struct anlegen
	IP_packet = ( struct IP_header *)&buffer[ETHERNET_HEADER_LENGTH];
	struct ICMP_header *ICMP_packet;	// ICMP_struct anlegen
	ICMP_packet = ( struct ICMP_header *)&buffer[ETHERNET_HEADER_LENGTH + ((IP_packet->IP_Version_Headerlen & 0x0f) * 4 )];

	switch ( ICMP_packet->ICMP_type )
	{
		case ICMP_EchoRequest:		//IP_header unbauen zum versenden 
									IP_packet->IP_DestinationIP = IP_packet->IP_SourceIP;
									IP_packet->IP_SourceIP = myIP;
									//IP_header checksummer ausrechnen
									IP_packet->IP_Headerchecksum = 0x0;
									checksum = Checksum_16( &buffer[14], ((IP_packet->IP_Version_Headerlen & 0x0f) * 4 ) );
									IP_packet->IP_Headerchecksum = htons ( checksum );
									ICMP_packet->ICMP_type = ICMP_EchoReplay; // auf reply einstellen
									ICMP_packet->ICMP_code = 0x00; 
									//Simple ICMP Checksummenbildung, die Idee stammt von
									//Simon, siehe http://avr.auctionant.de/
									if ( ICMP_packet->ICMP_checksumByteOne >  0xFF-0x08 )
										ICMP_packet->ICMP_checksumByteTwo++;
									ICMP_packet->ICMP_checksumByteOne+=0x08;
									// Ethernetframe bauen
									for(i = 0; i < 6 ; i++ )
									{
										ETH_packet->ETH_destMac[i] = ETH_packet->ETH_sourceMac[i];	
										ETH_packet->ETH_sourceMac[i] = mymac[i];
									}
									// und ab die post
									sendEthernetframe( packet_lenght, buffer); // packet_lenght - 4 weil der Controller die checksumme selber berechnet
									break;
		case ICMP_EchoReplay:		if ( ICMP_packet->ICMP_Identifierer == 0xac1d && ICMP_Replaystate == ICMP_WaitForReplay )
										ICMP_Replaystate = ICMP_ReplayOkay;
									break;
	}
}

/*!\brief Sendet einen ICMP-Request und wartet auf Antwort
 * \param 	IP		IP-Adresse die angepingt werden soll
 * \return	Zeit in Millisekunden für die Antwort oder -1
 */
int icmp_send( long IP )
{
	int i;
	char MACadress[6];
	int counter = 0, checksum;
	
	char * ethernetbuffer;
	ethernetbuffer = (char*) __builtin_alloca (( size_t ) ETHERNET_HEADER_LENGTH + IP_HEADER_LENGHT + ICMP_HEADER_LENGHT );

//	struct ETH_header *ETH_packet; 		// ETH_struct anlegen
//	ETH_packet = (struct ETH_header *)&ethernetbuffer;

	struct IP_header *IP_packet;		// IP_struct anlegen
	IP_packet = ( struct IP_header *)&ethernetbuffer[ETHERNET_HEADER_LENGTH];
	IP_packet->IP_Version_Headerlen = 0x45; // Standart IPv4 und Headerlenght 20byte

	struct ICMP_header *ICMP_packet;	// ICMP_struct anlegen
	ICMP_packet = ( struct ICMP_header *)&ethernetbuffer[ETHERNET_HEADER_LENGTH + IP_HEADER_LENGHT ];

	// Arp-request senden und schaun ob ip vorhanden
	if ( IS_ADDR_IN_MY_SUBNET( IP, Netmask ) )
	{
		if ( IS_BROADCAST_ADDR( IP, Netmask ) ) 
		{
			for( i = 0 ; i < 6 ; i++ ) MACadress[i] = 0xff;
		}
		else if ( GetIP2MAC( IP, MACadress ) == NO_ARP_ANSWER ) return ( -1 );
	}
	else if ( GetIP2MAC( Gateway , MACadress ) == NO_ARP_ANSWER ) return ( -1 );

	// ICMP_Replaystate setzen um auf Antwort zu warten
	ICMP_Replaystate = ICMP_WaitForReplay;

	// IP-Header basteln
	MakeIPheader( IP, PROTO_ICMP , 8 , ethernetbuffer );

	// ICMP-Header basteln
	ICMP_packet->ICMP_type = ICMP_EchoRequest; // auf request einstellen
	ICMP_packet->ICMP_code = 0x00; 
	ICMP_packet->ICMP_checksumByteOne = 0;
	ICMP_packet->ICMP_checksumByteTwo = 0;
	ICMP_packet->ICMP_Identifierer = 0xac1d;
	ICMP_packet->ICMP_Seqnumber = 0;

	checksum = htons( Checksum_16( (char*)ICMP_packet, ICMP_HEADER_LENGHT ) );
	ICMP_packet->ICMP_checksumByteOne = checksum & 0xff;
	ICMP_packet->ICMP_checksumByteTwo = ( checksum & 0xff00 ) >> 8;

	// Ethernetheader basteln und senden
	MakeETHheader( MACadress, ethernetbuffer );
	sendEthernetframe( ETHERNET_HEADER_LENGTH + IP_HEADER_LENGHT + ICMP_HEADER_LENGHT, ethernetbuffer);

	// Zeit messen für die Antwort und bei 1000 abbrechen
	while( counter < 1000 )
	{
		if ( ICMP_Replaystate == ICMP_ReplayOkay )
			break;
		counter++;
		_delay_ms(1);
	}

	ICMP_Replaystate = ICMP_Unknow;
	
	if ( counter == 1000 )
		return(-1);

	return(counter);
}

/**
 * @}
 */
