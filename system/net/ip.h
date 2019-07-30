/*!\file ip.h \brief Stellt die IP Funktionalitaet bereit*/
//***************************************************************************
//*            ip.h
//*
//*  Mon Jul 31 21:46:47 2007
//*  Copyright  2007  sharandac
//*  Email
///	\ingroup network
///	\defgroup IP Der IP Stack fuer Mikrocontroller (ip.h)
///	\code #include "arp.h" \endcode
///	\code #include "ethernet.h" \endcode
///	\code #include "ip.h" \endcode
///	\code #include "icmp.h" \endcode
///	\code #include "udp.h" \endcode
///	\code #include "tcp.h" \endcode
///	\par Uebersicht
///		Der IP-Stack für Mikrocontroller. Behandelt die Verarbeitung der eingehenden IP
/// Packete und reicht diese weiter an UDP, TCP und ICMP.
//****************************************************************************/
/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
//@{

#ifndef __IP_H__

	#define __IP_H__


// berechnet die Broadcast-Adresse bei gegebener IP-Adresse und Netzmaske
#define CALC_BROADCAST_ADDR( ip, mask ) ( ip | ~mask )

// Testet, ob eine Adresse die Broadcast-Adresse is (zu einer Netzwerkmaske)
#define IS_BROADCAST_ADDR( ip, mask ) ( ( ip & ~mask ) == ~mask)

// Schaut ob Ziel-IP in diesen Subnet liegt 
#define IS_ADDR_IN_MY_SUBNET( ip, mask ) ( ( ip & mask ) == ( myIP & mask ) )

	extern long myIP;
	extern long Netmask;
	extern long Gateway;
	extern long DNSserver;

/*
 struct IP_CONFIG {
		long myIP;
		long Netmask;
		long Gateway;
		long DNSserver;
	};
*/

	void ip( int packet_lenght , char * buffer );
	void MakeIPheader( long SourceIP, char Protocoll, int Datalenght , char * ethernetbuffer );
	long strtoip( char * buffer );
	char * iptostr( long IP, char * strIP );
/*
	void ip_setconfig( struct IP_CONFIG * ip_config );
	void ip_getconfig( struct IP_CONFIG * ip_config );
*/

	#define IP_HEADER_LENGHT 20

	// Convert dot-notation IP address into 32-bit word. Example: IPDOT(192l,168l,1l,1l)
	#define IPDOT( d, c, b, a ) ((a<<24)|(b<<16)|(c<<8)|(d))

	union IP_ADDRESS {
		char IPbyte[4];
		long IP;
	};

	#define FRAGMENTOFFSET_bm		0x3fff

	struct IP_header{
		unsigned char IP_Version_Headerlen;
		unsigned char IP_TOS;
		unsigned int IP_Totallenght;
		unsigned int IP_Identification;
		unsigned int IP_Flags_Fragmentoffset;
		unsigned char IP_TTL;
		unsigned char IP_Protocol;
		unsigned int IP_Headerchecksum;
		unsigned long IP_SourceIP;
		unsigned long IP_DestinationIP;
	};
	
	#define PROTO_ICMP 0x01
	#define PROTO_TCP 0x06
	#define PROTO_UDP 0x11
	

#endif
//@}
