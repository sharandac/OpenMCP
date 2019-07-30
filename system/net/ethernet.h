/***************************************************************************
 *            ethernet.h
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
 * \file
 *
 * \author Dirk Broßwick
 */

#ifndef __ETHERNET_H__
	
	#define __ETHERNET_H__
	
	extern char mymac[6];
	extern unsigned long PacketCounter;
	extern unsigned long ByteCounter;
	extern unsigned long eth_state_error;

	void ethernet_rx( char * packet, int len);
	unsigned int getEthernetframe( int maxlen, char *buffer);
	void MakeETHheader( char * MACadress , char * buffer );
	void sendEthernetframe( int packet_lenght, char *buffer);
	void EthernetInit( void );
	void LockEthernet( void );
	void FreeEthernet( void );
	void alive( void );

	#define	ETH_LOCK	1
	#define	ETH_FREE	0

	#define ETHERNET_MIN_PACKET_LENGTH	0x3C
	#define ETHERNET_HEADER_LENGTH		14

	struct ETH_header {
		unsigned char ETH_destMac[6];	
		unsigned char ETH_sourceMac[6];
		unsigned int ETH_typefield;
	};

#endif

/**
 * @}
 */