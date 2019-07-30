/***************************************************************************
 *            ethernet.c
 *
 *  Sat Jun  3 17:25:42 2006
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
 * \addtogroup ethernet Stellt Funktionen zum senden und empfangen über Ethernet bereit. (ethernet.c)
 * \code #include "ethernet.h" \endcode
 * @{
 */

/**
 * \file
 *
 * \author Dirk Broßwick
 */

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <stdio.h>
#include <string.h>

#include "config.h"

#include "hardware/network/zg2100/zg2100_driver.h"
#include "hardware/network/enc28j60.h"
#include "ethernet.h"
#include "arp.h"
#include "ip.h"
#include "tcp.h"
#include "endian.h"
#include "system/config/eeconfig.h"
#include "system/stdout/stdout.h"

#if defined(myAVR) && defined(LED)
	#include "hardware/led/led_core.h"
#endif
#if defined(LCD)
	#include "hardware/lcd/lcd.h"
#endif

char mymac[6] = { ENC28J60_MAC0,ENC28J60_MAC1,ENC28J60_MAC2,ENC28J60_MAC3,ENC28J60_MAC4,ENC28J60_MAC5 };
unsigned long PacketCounter;
unsigned long ByteCounter;
static char eth_state = ETH_LOCK;
unsigned long eth_state_error = 0;

/*!\brief Empfängt ein Ethernetpacket
 * \param 	*packet		Zeiger auf das Ethernetframe
 * \param 	len			Länge des Ethernetframe in Bytes
 * \return	NONE
 */
void ethernet_rx( char * packet, int len)
{
	PacketCounter++;
	ByteCounter = ByteCounter + len;
	
	struct ETH_header *ETH_packet; 		//ETH_struc anlegen
	ETH_packet = (struct ETH_header *) packet; 
	switch ( ntohs( ETH_packet->ETH_typefield ) ) // welcher type ist gesetzt 
	{
		case 0x0806:
#ifdef _DEBUG_
			printf_P( PSTR("-->> ARP\r\n") );
#endif
				arp( len , packet );
			break;
		case 0x0800:		
#ifdef _DEBUG_
			printf_P( PSTR("-->> IP\r\n") );										
#endif
				ip( len , packet );
			break;
	}
}

/*!\brief Sendet ein Ethernetframe
 * \param 	packet_lenght		Länge des Ethernetframe in Bytes
 * \param 	*ethernetbuffer		Zeiger auf das Ethernetframe	 
 * \return	NONE
 */
void sendEthernetframe( int packet_lenght, char *ethernetbuffer)
{
#if defined(myAVR) && defined(LED)
	LED_on(1);
#endif
	
	PacketCounter++;
	ByteCounter = ByteCounter + packet_lenght;
#if defined(USE_ZG2100)
	zg2100_tx( ethernetbuffer, packet_lenght );
#else
	enc28j60PacketSend( packet_lenght, ethernetbuffer );
#endif
	
#if defined(myAVR) && defined(LED)
	LED_off(1);
#endif
}

/*!\brief Baut den passenden Ethernetheader
 * \param 	*MACadresse			Zeiger auf die Macadresse
 * \param 	*ethernetbuffer		Zeiger auf das Ethernetframe	 
 * \return	NONE
 */
void MakeETHheader( char * MACadress , char * ethernetbuffer )
{
	struct ETH_header *ETH_packet; 		// ETH_struct anlegen
	ETH_packet = (struct ETH_header *) ethernetbuffer;

	int i;			

	ETH_packet->ETH_typefield = 0x0008;
	
	for ( i = 0 ; i < 6 ; i++ ) 
	{
		ETH_packet->ETH_sourceMac[i] = mymac[i];			
		ETH_packet->ETH_destMac[i] = MACadress[i];
	}
	return;		
}

/*!\brief Sperrt das Ethernetinterface
 * \param 	NONE
 * \return	NONE
 */
void LockEthernet( void )
{
	if( eth_state == ETH_FREE )
	{
		char sreg_tmp;
		sreg_tmp = SREG;    /* Sichern */
		cli();

		eth_state = ETH_LOCK;
		LockTCP();

#if defined(ZG2100)
		zg2100_lock();
#else
		enc28j60_lockethernet();
#endif
		
		SREG = sreg_tmp;
	}
	else
		eth_state_error++;
}

/*!\brief Gibt das Ethernetinterface frei
 * \param 	NONE
 * \return	NONE
 */
void FreeEthernet( void )
{
	if( eth_state == ETH_LOCK )
	{
		char sreg_tmp;
		sreg_tmp = SREG;    /* Sichern */
		cli();

		eth_state = ETH_FREE;

#if defined(USE_ZG2100)
		zg2100_free();
#else
		enc28j60_freeethernet();
#endif
		
		FreeTCP();
		SREG = sreg_tmp;
	}
	else
		eth_state_error++;
}

/*!\brief Initialisiert das Ethernetinterface
 * \param 	NONE
 * \return	NONE
 */
void EthernetInit( void )
{
	
#if defined(USE_ZG2100)
	char SSID[32];
	char PASS[32];

	// WLAN ZG2100 initialisieren
	zg2100_drv_init();
	zg2100_getmac( mymac );

	printf_P( PSTR("ZG2100 initialisiert, link:"));

	// Wlan Zugangsdaten aus Konfigspeicher einlesen
	readConfig_P( PSTR("WLAN_SSID"), SSID );
	readConfig_P( PSTR("&WLAN_PASS"), PASS );

#if defined(LCD)
	int len=0;

	STDOUT_set( _LCD, 0);
	len = strlen(SSID);

	if ( len >= 16 ) 
	{
		len = 16;
	}
	
	LCD_setXY( 0, 8-(len/2), 6 );
	printf_P( PSTR("%s"),SSID);
	STDOUT_set( RS232, 0);
#endif
	
	// mit WLAN verbinden
	zg2100_set_ssid( SSID );
	zg2100_set_wpa_key( PASS );

	LCD_setXY( 0, 0, 4 );	
	// Verbindung erfogreich?
	if( zg2100_connect(1,0xff) == 1)
		printf_P(PSTR(" established"));
	else
		printf_P(PSTR(" failed"));

#else
	// Ethernet ENC28j60 initialisieren und verbinden
	enc28j60Init();
	nicSetMacAddress( mymac );
	printf_P( PSTR("ENC28j60 initialisiert") );
	enc28j60EnableFullDuplex();
#endif

	printf_P(PSTR(" ( HW-Add: %02x:%02x:%02x:%02x:%02x:%02x )\r\n"), mymac[ 0 ] , mymac[ 1 ] , mymac[ 2 ] , mymac[ 3 ] , mymac[ 4 ] , mymac[ 5 ] );
	// gibt Ethernet frei
	FreeEthernet();
}

/**
 * @}
 */
