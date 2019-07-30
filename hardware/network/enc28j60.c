//*****************************************************************************
//
// File Name	: 'enc28j60.c'
// Title		: Microchip ENC28J60 Ethernet Interface Driver
// Author		: Pascal Stang (c)2005
// Created		: 9/22/2005
// Revised		: 9/22/2005
// Version		: 0.1
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
//*****************************************************************************
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
 * \ingroup Hardware
 * \addtogroup enc28j60 Microchip ENC28J60 Ethernet Interface Driver. (enc28j60.c)
 *
 * \par Overview
 *		This driver provides initialization and transmit/receive
 * functions for the Microchip ENC28J60 10Mb Ethernet Controller and PHY.
 * This chip is novel in that it is a full MAC+PHY interface all in a 28-pin
 * chip, using an SPI interface to the host processor.
 * @{
 */

/**
 * \file
 *
 * \author Dirk Broßwick
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "enc28j60.h"

#include "hardware/spi/spi_core.h"
#include "system/net/ethernet.h"
#include "system/clock/delay_x.h"

#include "config.h"

#if defined(EXTINT)
	#include "hardware/ext_int/ext_int.h"
#endif

#if defined(myAVR)

	#if defined(LED)
		#include "hardware/led/led_core.h"
	#else
		#error "Bitte LED aktivieren!"
	#endif

	#if defined(PC_INT)
		#include "hardware/pcint/pcint.h"
	#else
		#error "Bitte PC_INT aktivieren!"
	#endif
#endif

unsigned char Enc28j60Bank;
unsigned int NextPacketPtr;

//*********************************************************************************************************
//
// Setzt die MAC-Adressse im Enc28j60
//
//*********************************************************************************************************
void nicSetMacAddress( char * MAC)
{
	// write MAC address
	// NOTE: MAC address in ENC28J60 is byte-backward
  	enc28j60Write(MAADR5, MAC[ 0 ] );
	enc28j60Write(MAADR4, MAC[ 1 ] );
	enc28j60Write(MAADR3, MAC[ 2 ] );
	enc28j60Write(MAADR2, MAC[ 3 ] );
	enc28j60Write(MAADR1, MAC[ 4 ] );
	enc28j60Write(MAADR0, MAC[ 5 ] );
}

//*********************************************************************************************************
//
// Sende Rad Command
//
//*********************************************************************************************************
char enc28j60ReadOp( char op, char address)
{
	char temp_sreg;
	temp_sreg = SREG;
	cli();

	unsigned char data;
	// CS aktive setzen
#if defined(__AVR_XMEGA__)
	ENC28J60_CONTROL_PORT.OUTCLR = ( 1<<ENC28J60_CONTROL_CS );
#else
	ENC28J60_CONTROL_PORT &= ~( 1<<ENC28J60_CONTROL_CS );
#endif
	// lesecomando schreiben
	data = SPI_ReadWrite( SPIBUS, op | (address & ADDR_MASK) );
	// dummy senden um ergebnis zu erhalten
	data = SPI_ReadWrite( SPIBUS, 0x00 );
	// dummy read machen
	if ( address & 0x80 )
		data = SPI_ReadWrite( SPIBUS, 0x00 );
	// CS wieder freigeben
	_delay_us( 1 );
#if defined(__AVR_XMEGA__)
	ENC28J60_CONTROL_PORT.OUTSET = (1<<ENC28J60_CONTROL_CS);
#else
	ENC28J60_CONTROL_PORT |= (1<<ENC28J60_CONTROL_CS);
#endif

	SREG = temp_sreg;

	return data;
}

//*********************************************************************************************************
//
// Sende Write Command
//
//*********************************************************************************************************
void enc28j60WriteOp( char op, char address, char data)
{
	char temp_sreg;
	temp_sreg = SREG;
	cli();
	
	// CS aktive setzen
#if defined(__AVR_XMEGA__)
	ENC28J60_CONTROL_PORT.OUTCLR = ( 1<<ENC28J60_CONTROL_CS );
#else
	ENC28J60_CONTROL_PORT &= ~( 1<<ENC28J60_CONTROL_CS );
#endif
	// schreibcomando senden
	SPI_ReadWrite( SPIBUS, op | (address & ADDR_MASK) );
	// daten senden
	SPI_ReadWrite( SPIBUS, data );
	// CS wieder freigeben
	_delay_us( 1 );
#if defined(__AVR_XMEGA__)
	ENC28J60_CONTROL_PORT.OUTSET = (1<<ENC28J60_CONTROL_CS);
#else
	ENC28J60_CONTROL_PORT |= (1<<ENC28J60_CONTROL_CS);
#endif

	SREG = temp_sreg;
}

//*********************************************************************************************************
//
// Buffer einlesen
//
//*********************************************************************************************************
void enc28j60ReadBuffer( int len, char * data)
{
	char temp_sreg;
	temp_sreg = SREG;
	cli();

	// assert CS
#if defined(__AVR_XMEGA__)
	ENC28J60_CONTROL_PORT.OUTCLR = ( 1<<ENC28J60_CONTROL_CS );
#else
	ENC28J60_CONTROL_PORT &= ~( 1<<ENC28J60_CONTROL_CS );
#endif
	// issue read command
	SPI_ReadWrite( SPIBUS, ENC28J60_READ_BUF_MEM );

	SPI_ReadBlock( SPIBUS, data, len);
/*	while(len--)
	{
		// read data
		*data++ = SPI1_ReadWrite( 0x00 );
	} 
*/
	// release CS
	_delay_us( 1 );
#if defined(__AVR_XMEGA__)
	ENC28J60_CONTROL_PORT.OUTSET = (1<<ENC28J60_CONTROL_CS);
#else
	ENC28J60_CONTROL_PORT |= (1<<ENC28J60_CONTROL_CS);
#endif
	
	SREG = temp_sreg;
}

//*********************************************************************************************************
//
// Buffer schreiben
//
//*********************************************************************************************************
void enc28j60WriteBuffer( int len, char * data)
{
	char temp_sreg;
	temp_sreg = SREG;
	cli();

	// assert CS
#if defined(__AVR_XMEGA__)
	ENC28J60_CONTROL_PORT.OUTCLR = ( 1<<ENC28J60_CONTROL_CS );
#else
	ENC28J60_CONTROL_PORT &= ~( 1<<ENC28J60_CONTROL_CS );
#endif
	
	// issue write command
	SPI_ReadWrite( SPIBUS, ENC28J60_WRITE_BUF_MEM );

//	SPI1_FastMem2Write( data, len );
	while(len--)
	{
		// write data
		SPI_ReadWrite( SPIBUS, *data++ );
	}
	// release CS
	_delay_us( 1 );
#if defined(__AVR_XMEGA__)
	ENC28J60_CONTROL_PORT.OUTSET = (1<<ENC28J60_CONTROL_CS);
#else
	ENC28J60_CONTROL_PORT |= (1<<ENC28J60_CONTROL_CS);
#endif
	SREG = temp_sreg;
}

//*********************************************************************************************************
//
// 
//
//*********************************************************************************************************
void enc28j60SetBank(char address)
{
	// set the bank (if needed)
	if((address & BANK_MASK) != Enc28j60Bank)
	{
		// set the bank
		enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, (ECON1_BSEL1|ECON1_BSEL0));
		enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, (address & BANK_MASK)>>5);
		Enc28j60Bank = (address & BANK_MASK);
	}
}

//*********************************************************************************************************
//
// 
//
//*********************************************************************************************************
char enc28j60Read( char address)
{
	// set the bank
	enc28j60SetBank(address);
	// do the read
	return enc28j60ReadOp(ENC28J60_READ_CTRL_REG, address);
}

//*********************************************************************************************************
//
// 
//
//*********************************************************************************************************
void enc28j60Write( char address, char data)
{
	// set the bank
	enc28j60SetBank(address);
	// do the write
	enc28j60WriteOp(ENC28J60_WRITE_CTRL_REG, address, data);
}

//*********************************************************************************************************
//
// 
//
//*********************************************************************************************************
int enc28j60PhyRead( char address)
{
     int data;
 
     // Set the right address and start the register read operation
     enc28j60Write(MIREGADR, address);
     enc28j60Write(MICMD, MICMD_MIIRD);
 
    // wait until the PHY read completes
     while(enc28j60Read(MISTAT) & MISTAT_BUSY);
 
     // quit reading
     enc28j60Write(MICMD, 0x00);
     
     // get data value
     data  = enc28j60Read(MIRDL);
     data |= enc28j60Read(MIRDH)<<8;
     // return the data
     return data;
}

//*********************************************************************************************************
//
// 
//
//*********************************************************************************************************
void enc28j60PhyWrite( char address, int data)
{
	// set the PHY register address
	enc28j60Write(MIREGADR, address);

	// write the PHY data
	enc28j60Write(MIWRL, data);
	enc28j60Write(MIWRH, data>>8);

	// wait until the PHY write completes
	while(enc28j60Read(MISTAT) & MISTAT_BUSY);
}

//*********************************************************************************************************
//
// 
//
//*********************************************************************************************************
int enc28j60GetRev( void )
{
	// read die RevID
	return( enc28j60Read( EREVID ) );
}
//*********************************************************************************************************
//
// Initialiesiert den ENC28J60
//
//*********************************************************************************************************
void enc28j60Init(void)
{
	// initialize I/O

	// schau mal ob SPI schon laufen tut, wenn starten
	// if ( SPI_GetInitState() == SPI_NOT_INIT ) 
	// delay(10);
		
	SPI_init( SPIBUS );

#if defined(__AVR_XMEGA__)
	ENC28J60_CONTROL_PORT.DIRSET = (1<<ENC28J60_CONTROL_CS);
	ENC28J60_CONTROL_PORT.OUTSET = (1<<ENC28J60_CONTROL_CS);
#else
	ENC28J60_CONTROL_DDR |= 1<<ENC28J60_CONTROL_CS;
	ENC28J60_CONTROL_PORT |= 1<<ENC28J60_CONTROL_CS;
#endif

#if defined(ENC28J60_RESET_PORT)
	#if defined(__AVR_XMEGA__)
		ENC28J60_RESET_PORT.DIRSET = (1<<ENC28J60_RESET_PIN);
		ENC28J60_RESET_PORT.OUTCLR = (1<<ENC28J60_RESET_PIN);
	#else
		ENC28J60_RESET_DDR |= 1<<ENC28J60_RESET_PIN;
		ENC28J60_RESET_PORT &= ~(1<<ENC28J60_RESET_PIN);
	#endif

	_delay_ms( 10 );
	
	#if defined(__AVR_XMEGA__)
		ENC28J60_RESET_PORT.OUTSET = (1<<ENC28J60_RESET_PIN);
	#else
		ENC28J60_RESET_PORT |= 1<<ENC28J60_RESET_PIN;
	#endif
	
	_delay_ms( 10 );
#endif

	// perform system reset
	enc28j60WriteOp(ENC28J60_SOFT_RESET, 0, ENC28J60_SOFT_RESET);
	// check CLKRDY bit to see if reset is complete

	while(!(enc28j60Read(ESTAT) & ESTAT_CLKRDY));
	
	// do bank 0 stuff
	// initialize receive buffer
	// 16-bit transfers, must write low byte first
	// set receive buffer start address
	NextPacketPtr = RXSTART_INIT;
	enc28j60Write(ERXSTL, RXSTART_INIT&0xFF);
	enc28j60Write(ERXSTH, RXSTART_INIT>>8);
	// set receive pointer address
	enc28j60Write(ERXRDPTL, RXSTART_INIT&0xFF);
	enc28j60Write(ERXRDPTH, RXSTART_INIT>>8);
	// set receive buffer end
	// ERXND defaults to 0x1FFF (end of ram)
	enc28j60Write(ERXNDL, RXSTOP_INIT&0xFF);
	enc28j60Write(ERXNDH, RXSTOP_INIT>>8);
	// set transmit buffer start
	// ETXST defaults to 0x0000 (beginnging of ram)
	enc28j60Write(ETXSTL, TXSTART_INIT&0xFF);
	enc28j60Write(ETXSTH, TXSTART_INIT>>8);

	// enable Multicast, Unicast and Broadcast		
#if defined(MULTICAST)
	enc28j60Write(ERXFCON, ERXFCON_UCEN | ERXFCON_CRCEN | ERXFCON_MCEN | ERXFCON_BCEN);
#else
	enc28j60Write(ERXFCON, ERXFCON_UCEN | ERXFCON_CRCEN | ERXFCON_BCEN);
#endif
	
	// do bank 2 stuff
	// enable MAC receive
	enc28j60Write(MACON1, MACON1_MARXEN|MACON1_TXPAUS|MACON1_RXPAUS);
	// bring MAC out of reset
	enc28j60Write(MACON2, 0x00);
	// bring MAC out of reset
	enc28j60Write(MACLCON1, 0x03);
	// enable automatic padding and CRC operations
	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, MACON3, MACON3_PADCFG0|MACON3_TXCRCEN|MACON3_FRMLNEN);


	// set inter-frame gap (non-back-to-back)
	enc28j60Write(MAIPGL, 0x12);
	enc28j60Write(MAIPGH, 0x0C);
	// set inter-frame gap (back-to-back)
	enc28j60Write(MABBIPG, 0x12);
	// Set the maximum packet size which the controller will accept
	enc28j60Write(MAMXFLL, MAX_FRAMELEN&0xFF);
	enc28j60Write(MAMXFLH, MAX_FRAMELEN>>8);

	// no loopback of transmitted frames
	enc28j60PhyWrite(PHCON2, PHCON2_HDLDIS);

	// switch to bank 0
	enc28j60SetBank(ECON1);
	// enable interrutps
	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, EIE, EIE_INTIE|EIE_PKTIE);
	// enable packet reception
	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);
	// LED initialization
	enc28j60PhyWrite(PHLCON, 0x0476);

	// ENC Initialisieren //

#if defined(myAVR)
	PCINT_init();
	PCINT_set( ENC28J60_INT, enc28j60PacketReceive );
#endif

#if defined(OpenMCP) || defined(AVRNETIO) || defined(UPP) || defined( EtherSense )
	EXTINT_set ( ENC28J60_INT , SENSE_LOW , enc28j60PacketReceive );
#endif

#if defined(XPLAIN) || defined(ATXM2)
	EXTINT_set ( &ENC28J60_INT, ENC28J60_INT_PIN, SENSE_LOW , enc28j60PacketReceive );
#endif
}

//*********************************************************************************************************
//
// Sendet ein Packet
//
//*********************************************************************************************************
void enc28j60PacketSend( int len, char* packet)
{
	// wait for a previous transmitted frame
	while ( (enc28j60Read(ECON1) & ECON1_TXRTS) != 0 );

	// Set the write pointer to start of transmit buffer area
	enc28j60Write(EWRPTL, ( unsigned char ) TXSTART_INIT );
	enc28j60Write(EWRPTH, TXSTART_INIT>>8);
	
	// Set the TXND pointer to correspond to the packet size given
	enc28j60Write(ETXNDL, (TXSTART_INIT+len));
	enc28j60Write(ETXNDH, (TXSTART_INIT+len)>>8);

	// write per-packet control byte
	enc28j60WriteOp(ENC28J60_WRITE_BUF_MEM, 0, 0x00);

	// copy the packet into the transmit buffer
	enc28j60WriteBuffer(len, packet);
	
	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRST);
	enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRST);
	
	// send the contents of the transmit buffer onto the network
	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);
	// wait for transmission complete
//	while ( (enc28j60Read(ECON1) & ECON1_TXRTS) != 0 );

	// send the contents of the transmit buffer onto the network
//	enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRTS);
}

//*********************************************************************************************************
//
// Hole ein empfangendes Packet
//
//*********************************************************************************************************
int enc28j60PacketReceiveLenght( void )
	{
	int len;

	enc28j60Write(ERDPTL, (NextPacketPtr));
	enc28j60Write(ERDPTH, (NextPacketPtr)>>8);

	// read the packet length
	len  = enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0);
	len |= enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0)<<8;
	
	return( len );
	}


//*********************************************************************************************************
//
// Hole ein empfangendes Packet
//
//*********************************************************************************************************
void enc28j60PacketReceive( void )
{
	char packet[ MAX_FRAMELEN ];
	int rxstat, rs,re;;
	int len;

#if defined(myAVR) && defined(LED)
	LED_on(1);
#endif

	while( 1 )
	{
		// check if a packet has been received and buffered
		if( !(enc28j60Read(EIR) & EIR_PKTIF) )
			if (enc28j60Read(EPKTCNT) == 0)
			{
#if defined(myAVR) && defined(LED)
				LED_off(1);
#endif
				break;
			}

		// Set the read pointer to the start of the received packet
		enc28j60Write(ERDPTL, (NextPacketPtr));
		enc28j60Write(ERDPTH, (NextPacketPtr)>>8);

		// read the next packet pointer
		NextPacketPtr  = enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0);
		NextPacketPtr |= enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0)<<8;

		// read the packet length
		len  = enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0);
		len |= enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0)<<8;

		// read the receive status
		rxstat  = enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0);
		rxstat |= enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0)<<8;

		// limit retrieve length
		// len = MIN(len, maxlen);
		// When len bigger than maxlen, ignore the packet und read next packetptr
		if ( len > MAX_FRAMELEN ) 
		{
			enc28j60Write(ERXRDPTL, (NextPacketPtr));
			enc28j60Write(ERXRDPTH, (NextPacketPtr)>>8);
			enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);
			return;
		}
		// copy the packet from the receive buffer
		enc28j60ReadBuffer(len, packet);

		// an implementation of Errata B1 Section #13
		rs = enc28j60Read(ERXSTH);
		rs <<= 8;
		rs |= enc28j60Read(ERXSTL);
		re = enc28j60Read(ERXNDH);
		re <<= 8;
		re |= enc28j60Read(ERXNDL);
		if (NextPacketPtr - 1 < rs || NextPacketPtr - 1 > re)
		{
			enc28j60Write(ERXRDPTL, (re));
			enc28j60Write(ERXRDPTH, (re)>>8);
		}
		else
		{
			enc28j60Write(ERXRDPTL, (NextPacketPtr-1));
			enc28j60Write(ERXRDPTH, (NextPacketPtr-1)>>8);
		}

		// decrement the packet counter indicate we are done with this packet
		enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);

		ethernet_rx( packet, len );
	}
}

char enc28j60Linkcheck(void)
{
	if(enc28j60PhyRead(0x01) & PHSTAT1_LLSTAT)
	{
		return ( LINK_OK );
	}
	else
	{
		return ( LINK_DOWN );
	}
}

void enc28j60EnableFullDuplex( void )
{
	// setup duplex ----------------------
	// Disable receive logic and abort any packets currently being transmitted
	enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRTS|ECON1_RXEN);

	unsigned int temp;
	// Set the PHY to the proper duplex mode
	temp = enc28j60PhyRead(PHCON1);
	temp &= ~PHCON1_PDPXMD;
	enc28j60PhyWrite(PHCON1, temp);
	// Set the MAC to the proper duplex mode
	temp = enc28j60Read(MACON3);
	temp &= ~MACON3_FULDPX;
	enc28j60Write(MACON3, temp);

	// Set the back-to-back inter-packet gap time to IEEE specified 
	// requirements.  The meaning of the MABBIPG value changes with the duplex
	// state, so it must be updated in this function.
	// In full duplex, 0x15 represents 9.6us; 0x12 is 9.6us in half duplex
	enc28j60Write(MABBIPG, 0x15 );    
	enc28j60Write(MAIPGL, 0x12);
	enc28j60Write(MAIPGH, 0x0C);    

	// Reenable receive logic
	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);

	return;
}

void enc28j60EnableHalfDuplex( void )
{
	// setup duplex ----------------------
	// Disable receive logic and abort any packets currently being transmitted
	enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRTS|ECON1_RXEN);

	int temp;
	// Set the PHY to the proper duplex mode
	temp = enc28j60PhyRead(PHCON1);
	temp |= PHCON1_PDPXMD;
	enc28j60PhyWrite(PHCON1, temp);
	// Set the MAC to the proper duplex mode
	temp = enc28j60Read(MACON3);
	temp |= MACON3_FULDPX;
	enc28j60Write(MACON3, temp);

	// Set the back-to-back inter-packet gap time to IEEE specified 
	// requirements.  The meaning of the MABBIPG value changes with the duplex
	// state, so it must be updated in this function.
	// In full duplex, 0x15 represents 9.6us; 0x12 is 9.6us in half duplex
	enc28j60Write(MABBIPG, 0x12 );    
	enc28j60Write(MAIPGL, 0x12);
	enc28j60Write(MAIPGH, 0x0C);    

	// Reenable receive logic
	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);

	return;
}

void enc28j60_freeethernet( void )
{
#if defined(OpenMCP) || defined(UPP)
		EXTINT_free ( ENC28J60_INT );
#endif

#if defined(XPLAIN) || defined(ATXM2)
		EXTINT_free ( &ENC28J60_INT, ENC28J60_INT_PIN );
#endif

#if defined(AVRNETIO) || defined( EtherSense )
		EXTINT_free ( ENC28J60_INT );
#endif

#if defined(myAVR)
		PCINT_enablePIN( ENC28J60_INT_PIN, ENC28J60_INT );
		PCINT_enablePCINT( ENC28J60_INT );
#endif
}

void enc28j60_lockethernet( void )
{
#if defined(OpenMCP) || defined(UPP)
		EXTINT_block( ENC28J60_INT );
#endif

#if defined(XPLAIN) || defined(ATXM2)
		EXTINT_block ( &ENC28J60_INT, ENC28J60_INT_PIN );
#endif

#if defined(AVRNETIO) || defined( EtherSense )
		EXTINT_block( ENC28J60_INT );
#endif

#if defined(myAVR)
		PCINT_disablePCINT( ENC28J60_INT );
#endif
}

/**
 * @}
 */
