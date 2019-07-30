/***************************************************************************
 *  rfm12.c
 *
 *  Sun Apr 17 17:41:35 2011
 *  Based on Firmware version 2.0.1 by Jürgen Eckert
 *  -> http://www.mikrocontroller.net/articles/AVR_RFM12
 *	Ported to OpenMcp by Jan Wissel 
 *	OpenMcp, by  Dirk Broßwick
 *  <sharandac@snafu.de>
 *  for documentation see:
 *  http://www.mikrocontroller.net/articles/RFM12 
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
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "hardware/spi/spi_core.h"
#include "hardware/ext_int/ext_int.h"
#include "hardware/led/led_core.h"
#include "system/clock/clock.h"
#include "config.h"
#include "system/thread/thread.h"

#if defined( RFM12 )

#include "rfm12.h"

//used by interrupt routine
struct RFM12_stati RFM12_status;
volatile unsigned char RFM12_Index = 0;
unsigned char RFM12_Data[RFM12_DataLength+10];	// +10 == paket overhead

#define RFM_PACKET_SIZE 8


void rftransmit(void)
{	
    unsigned char RFMdata=0;
    unsigned char RFMpacket[RFM_PACKET_SIZE];
    unsigned char i;   

	for(i=0;i<RFM_PACKET_SIZE;i++){
        RFMpacket[i]=RFMdata;
        RFMdata++;
    }
 
    if ( RFM12_txstart(RFMpacket, RFM_PACKET_SIZE) != 0 )
		LED_toggle(2);
}

void rfrecieve(void)
{
    unsigned char RFMpacket[RFM_PACKET_SIZE], temp;
	int i;
	
	temp = RFM12_rxfinish( RFMpacket );
	if ( temp != 255 && temp != 254 )
	{
		printf_P( PSTR("RFM12 %d Bytes recieve."), temp );
		for ( i = 0 ; i < RFM_PACKET_SIZE ; i++)
			printf_P( PSTR("%02x ") , RFMpacket[ i ] );
		if ( RFM12_rxstart() != 0 )
			printf_P( PSTR("RFM12 error"));
		printf_P( PSTR("\r\n"));
	}
}


//interrupt service routine
void RFM12_isr( void )
{
	LED_on(2);
	
	if(RFM12_status.Rx)
	{
		if(RFM12_Index < RFM12_DataLength)
		{
			//no complete packet received, read FIFO, store in data
			RFM12_Data[RFM12_Index++] = RFM12_trans(0xB000) & 0x00FF;
		}
		else
		{
			//packet completely received, turn off RX
			RFM12_trans(0x8208);
			RFM12_status.Rx = 0;
		}
		if(RFM12_Index >= RFM12_Data[0] + 3)
		{	
			//EOT
			//enough data in package?
			//note: will fail if Data[0] was received incorrectly
			//anyway, does not matter, crc will detect that.
			//end RX and signalize new packet
			RFM12_trans(0x8208);
			RFM12_status.Rx = 0;
			RFM12_status.New = 1;
		}
	}
	else if(RFM12_status.Tx)
	{
		//transmit next byte
		RFM12_trans(0xB800 | RFM12_Data[RFM12_Index]);

		if(!RFM12_Index)
		{
			//index=0? -> end TX
			RFM12_status.Tx = 0;
			RFM12_trans(0x8208);		// TX off
		}
		else
		{
			RFM12_Index--;
		}
	}
	else
	{
		RFM12_trans(0x0000);			//dummy read
	}

	LED_off(2);
}


void RFM12_init( void )
{
	char tmp_sreg;	

    // save SREG and diable interrupts
	tmp_sreg = SREG;
	cli();
	
	// CS_Pin as Output, RFM12 deselect
	RFM12_CS_PORT |= (1<<RFM12_CS_PIN );
	RFM12_CS_DDR |= (1<<RFM12_CS_PIN );

	// Interrupt-Pin as Input
	RFM12_IRQ_PORT &= ~(1<<RFM12_IRQ_PIN );
	RFM12_IRQ_DDR &= ~(1<<RFM12_IRQ_PIN );

	// SPI start
	SPI_init( RFM12_SPI_BUS_NUM );

	RFM12_trans(0xC0E0);			// AVR CLK: 10MHz
	RFM12_trans(0x80D7);			// Enable FIFO
	RFM12_trans(0xC2AB);			// Data Filter: internal
	RFM12_trans(0xCA81);			// Set FIFO mode
	RFM12_trans(0xE000);			// disable wakeuptimer
	RFM12_trans(0xC800);			// disable low duty cycle
	RFM12_trans(0xC4F7);			// AFC settings: autotuning: -10kHz...+7,5kHz

	RFM12_trans(0x0000);
	
	RFM12_status.Rx = 0;
	RFM12_status.Tx = 0;
	RFM12_status.New = 0;

    RFM12_config();
    // Register RFM12 as interruptsource
	EXTINT_set ( RFM12_IRQ_EXINT_NUM , SENSE_LOW , RFM12_isr );                      

	SREG = tmp_sreg;
	
//	CLOCK_RegisterCallbackFunction ( rftransmit, SECOUND );
}

unsigned char RFM12_rxstart( void )
{
	if(RFM12_status.New)
		return(1);			//buffer not yet empty
	if(RFM12_status.Tx)
		return(2);			//tx in action
	if(RFM12_status.Rx)
		return(3);			//rx already in action

	RFM12_trans(0x82C8);		// RX on
	RFM12_trans(0xCA81);		// set FIFO mode
	RFM12_trans(0xCA83);		// enable FIFO
	
	//rf12_trans(0x0000);	//clear int

	RFM12_Index = 0;
	RFM12_status.Rx = 1;

	//MCUCR |= (1<<ISC01);
	//GICR |= (1<<INT0);
	return(0);				//all went fine
}


unsigned char RFM12_rxfinish( unsigned char *data )
{
	unsigned int crc, crc_chk = 0;
	unsigned char i;

	if( RFM12_status.Rx )
	{
		return( 255 );				//not finished yet
	}
	
	if( !RFM12_status.New )
	{
		return( 254 );				//old buffer
	}
	
	for( i = 0 ; i<RFM12_Data[0] + 1 ; i++ )
	{
		crc_chk = crcUpdate( crc_chk, RFM12_Data[ i ] );
	}
	
	crc = RFM12_Data[ i++ ];
	crc |= RFM12_Data[ i ] << 8;
	RFM12_status.New = 0;

/*	if( crc != crc_chk )
	{
		return( 253 );				//crc err -or- strsize
	}
	else
*/ 	{
		for( i = 0 ; i<RFM12_Data[ 0 ] ; i++ )
		{
			data[ i ] = RFM12_Data[ i+1 ];
		}
		return( RFM12_Data[ 0 ] );			//strsize
	}
}


unsigned char RFM12_txstart( unsigned char *data, unsigned char size )
{
	unsigned char i, l;
	unsigned int crc;

	if( RFM12_status.Tx )
	{
		return( 2 );			//tx in action
	}
	if( RFM12_status.Rx )
	{
		return( 3 );			//rx already in action
	}
	if( size > RFM12_DataLength )
	{
		return( 4 );			//str to big to transmit
	}
	
	RFM12_status.Tx = 1;
	RFM12_Index = size + 9;			//act -10 

	i = RFM12_Index;				
	RFM12_Data[ i-- ] = 0xAA;
	RFM12_Data[ i-- ] = 0xAA;
	RFM12_Data[ i-- ] = 0xAA;
	RFM12_Data[ i-- ] = 0x2D;
	RFM12_Data[ i-- ] = 0xD4;
	RFM12_Data[ i-- ] = size;
	crc = crcUpdate( 0 , size );
	for( l = 0 ; l<size ; l++ )
	{
		RFM12_Data[ i-- ] = data[ l ];
		crc = crcUpdate( crc, data[ l ] );
	}	
	RFM12_Data[ i-- ] = (crc & 0x00FF);
	RFM12_Data[ i-- ] = (crc >> 8);
	RFM12_Data[ i-- ] = 0xAA;
	RFM12_Data[ i-- ] = 0xAA;

	RFM12_trans(0x8238);			// TX on
	return( 0 );				//all went fine
}

unsigned char RFM12_txfinished( void )
{
	if(RFM12_status.Tx)
	{
		return( 255 );			//not yet finished
	}
	return(0);
}

void RFM12_allstop( void )
{
	//GICR &= ~(1<<INT0);		//disable int0	
	RFM12_status.Rx = 0;
	RFM12_status.Tx = 0;
	RFM12_status.New = 0;
	RFM12_trans(0x8208);		//shutdown all
	RFM12_trans(0x0000);		//dummy read
}

//-------------------------------------------------------------------------//
// internal used initialisation functions. 
// usually not called from elsewhere
void RFM12_config( void )
{
	RFM12_setfreq( RFM12FREQ( 433.92 ) );				// Send/ Receivefrequenzy 433,92MHz 
    RFM12_setbandwidth( RxBW200 , LNA_6 , RSSI_103 );	// 200kHz Bandwidth, -6dB Amplification, DRSSI threshold: -79dBm
    RFM12_setbaud( RFM12_BAUDRATE );					// 19200 baud
    RFM12_setpower( PWRdB_6 , TxBW120 );				// 1mW Power, 120kHz Frequenzyshift
}

void RFM12_setfreq(unsigned short freq)
{	
    if (freq<96)				// 430,2400MHz
	{
		freq=96;
	}
	else if (freq>3903)			// 439,7575MHz
	{
		freq=3903;
	}
	RFM12_trans(0xA000|freq);
}

void RFM12_setbandwidth( unsigned char bandwidth , unsigned char gain , unsigned char drssi )
{
	RFM12_trans( 0x9400 | ((bandwidth&7)<<5) | ((gain&3)<<3) | (drssi&7) );
}

void RFM12_setbaud(unsigned short baud)
{
	if ( baud<663 )
	{
		return;
	}
	if (baud<5400)					
	{
		// Baudrate= 344827,58621/(R+1)/(1+CS*7)
		RFM12_trans( 0xC680 | ( ( 43104/baud ) -1 ) );
	}
	else
	{
		RFM12_trans( 0xC600 | ( ( 344828UL/baud ) -1 ) );
	}
}


void RFM12_setpower( unsigned char power , unsigned char mod )
{	
	RFM12_trans( 0x9800 | (power&7) | ((mod&15)<<4) );
}

unsigned int crcUpdate(unsigned int crc, unsigned char serialData)
{
    unsigned int tmp;
    unsigned char j;

	tmp = serialData << 8;

	for (j=0; j<8; j++)
	{
        if((crc^tmp) & 0x8000)
		{
			crc = (crc<<1) ^ 0x1021;
		}
		else
		{
			crc = crc << 1;
		}
		tmp = tmp << 1;
    }
	return crc;
}

//-------------------------------------------------------------------------//
// hardware level functions
unsigned int RFM12_trans( unsigned int CMD )
{
    char tmp_sreg;
	int retval;
	CONVERTW val;

	val.w = CMD;
	
	//save SReg, clear Interrupts 
	tmp_sreg = SREG;
    cli();
    
	RFM12_select();
	
//    retval = ( SPI_ReadWrite( RFM12_SPI_BUS_NUM , ( CMD>>8 ) & 0xff  ) ) << 8 ;

	SPDR = val.b[1];
	while(!(SPSR & (1<<SPIF)));
	val.b[1]=SPDR;
	
//    retval |= SPI_ReadWrite( RFM12_SPI_BUS_NUM , CMD & 0xff );

	SPDR = val.b[0];
	while(!(SPSR & (1<<SPIF)));
	val.b[0]=SPDR;

	RFM12_deselect();

	// restore interrupts
    SREG = tmp_sreg;

	return( retval );
}

void RFM12_select( void ){
	RFM12_CS_PORT &= ~( 1<<RFM12_CS_PIN );
}

void RFM12_deselect( void ){
	RFM12_CS_PORT |= ( 1<<RFM12_CS_PIN );
}


#endif