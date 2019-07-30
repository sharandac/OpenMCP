/*!\file network.c \brief Init des Netzwerkteils */
/****************************************************************************
 *            network.c
 *
 *  Sat Feb 28 03:26:56 2009
 *  Copyright  2009  Dirk Broßwick
 *  Email sharandac(at)snafu.de
///	\ingroup network
///	\code #include "arp.h" \endcode
///	\code #include "ethernet.h" \endcode
///	\code #include "ip.h" \endcode
///	\code #include "icmp.h" \endcode
///	\code #include "dns.h" \endcode
///	\code #include "dhcp.h" \endcode
///	\code #include "ntp.h" \endcode
///	\code #include "udp.h" \endcode
///	\code #include "tcp.h" \endcode
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
//@{
#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <avr/version.h>

#include "config.h"

#include "system/stdout/stdout.h"
#include "hardware/network/enc28j60.h"
#include "system/net/ethernet.h"
#include "system/net/ip.h"
#include "system/net/arp.h"
#include "system/clock/clock.h"
#include "system/config/eeconfig.h"
#include "system/string/string.h"

#if defined(LED)
	#include "hardware/led/led_core.h"
#endif
#if defined(UDP)
	#include "system/net/udp.h"
	#if defined(DHCP)
		#include "system/net/dhcpc.h"
	#endif
	#if defined(NTP)
		#include "system/net/ntp.h"
	#endif
#endif
#if defined(TCP)
	#include "system/net/tcp.h"
#endif

const char IP_P[] PROGMEM = "IP";
const char MASK_P[] PROGMEM = "MASK";
const char DNS_P[] PROGMEM = "DNS";
const char GATE_P[] PROGMEM = "GATE";

const char DHCP_OKAY_P[] PROGMEM = "DHCP-Config geholt\r\n";
const char DHCP_FAIL_P[] PROGMEM = "DHCP-Config Fehlgeschlagen\r\n";
const char DHCP_TRY_P[] PROGMEM = " |-> Versuche DHCP-Config zu holen. ";


void network_init( void )
{
	// struct für die Zeit anlegen
	struct TIME Time;
	
	char ip[32];
	char timedif = 0;
	
	// Ethernet starten
	if (  checkConfigName_P( PSTR("MAC") ) != -1 ) 
	{
		readConfig_P ( PSTR("MAC"), ip );
		strtobin( ip, mymac, 12 );
	}

	// Ethernet starten
	EthernetInit();

	// ARP starten
	ARP_INIT ();
	printf_P( PSTR("-+-> ARP initialisiert\r\n"));
	
	#if defined(UDP)
		// UDP starten
		UDP_init();
		printf_P( PSTR(" |-> UDP (Tornado-engine) initialisiert\r\n"));
	#endif
		
	#if defined(TCP)
		// tcp starten
		tcp_init();
		printf_P( PSTR(" |-> TCP (Hurrican-engine) initialisiert\r\n"));
	#endif

#if defined(DHCP) && defined(UDP)

	if (  checkConfigName_P( PSTR("DHCP") ) == -1 ) 
	{
		// DHCP-Config holen
		printf_P( DHCP_TRY_P );
		if ( !DHCP_GetConfig () )
		{
			printf_P( DHCP_OKAY_P);
		}
		else 
		{
			printf_P( DHCP_FAIL_P);
		}
	}
	else
	{
		readConfig_P ( PSTR("DHCP"), ip );
		if ( !strcmp_P( ip, PSTR("on") ) )
		{
			// DHCP-Config holen
			printf_P( DHCP_TRY_P );
			if ( !DHCP_GetConfig () ) printf_P( DHCP_OKAY_P );
			else 
			{
				printf_P( DHCP_FAIL_P );
				#ifdef READ_CONFIG
					readConfig_P ( IP_P, ip );
					myIP = strtoip( ip );
					readConfig_P ( MASK_P, ip );
					Netmask = strtoip( ip );
					readConfig_P ( GATE_P, ip );
					Gateway = strtoip( ip );
					#ifdef DNS
						readConfig_P ( DNS_P, ip );
						DNSserver = strtoip( ip );
					#endif
				#endif
			}
		}
		#ifdef READ_CONFIG
		else
		{
			if ( readConfig_P ( IP_P, ip ) != -1)
				myIP = strtoip( ip );
			if ( readConfig_P ( MASK_P, ip ) != -1)
				Netmask = strtoip( ip );
			if ( readConfig_P ( GATE_P, ip ) != -1)
				Gateway = strtoip( ip );
			#ifdef DNS
				if ( readConfig_P ( DNS_P, ip ) != -1)
					DNSserver = strtoip( ip );
			#endif
		}
		#endif
	}
#endif
	
#if !defined(DHCP)
	#ifdef READ_CONFIG
		if ( readConfig_P ( IP_P, ip ) != -1)
			myIP = strtoip( ip );
		if ( readConfig_P ( MASK_P, ip ) != -1)
			Netmask = strtoip( ip );
		if ( readConfig_P ( GATE_P, ip ) != -1)
			Gateway = strtoip( ip );
		#ifdef DNS
			if ( readConfig_P ( DNS_P, ip ) != -1)
				DNSserver = strtoip( ip );
		#endif
	#endif
#endif
	
	
	printf_P( PSTR(	" |   IP     : %s\r\n"), iptostr( myIP, ip ));
	printf_P( PSTR(	" |   Netmask: %s\r\n"), iptostr( Netmask, ip ));
	printf_P( PSTR(	" |   Gateway: %s\r\n"), iptostr( Gateway, ip ) );
	#ifdef DNS
		printf_P( PSTR(" |   DNS    : %s\r\n"), iptostr( DNSserver, ip ) );
	#endif
	
		
#if defined(NTP) && defined(UDP)
	// Uhr einstellen
	if( readConfig_P( PSTR("NTP"), ip ) != -1 )
	{
		if ( !strcmp_P( ip, PSTR("on") ) )
		{
			printf_P( PSTR(" |-> NTP-Server Zeit aktualisieren:"));
			if( readConfig_P ( PSTR("UTCZONE"), ip ) != -1 )
			{
				timedif = (char)atoi( ip );
				readConfig_P ( PSTR("NTPSERVER"), ip );
				if( NTP_GetTime( 0 , ip, timedif ) == NTP_OK )
				{
					CLOCK_GetTime ( &Time );
					printf_P( PSTR(" Zeit: %02d:%02d:%02d.%02d\r\n"),Time.hh,Time.mm,Time.ss,Time.ms);
				}
				else
					printf_P( PSTR(" fehlgeschlagen\r\n"));
			}
			else
				printf_P( PSTR(" fehlgeschlagen\r\n"));
		}
	}
#endif

#if defined(LED)
	#if defined(OpenMCP)
		LED_on(2);
	#endif
#endif
	
#if defined(LCD)
	STDOUT_set( _LCD, 0);	
	printf_P( PSTR("\r\n%s"), iptostr( myIP, ip ) );
	STDOUT_set( RS232, 0);	
#endif

}
//@}
