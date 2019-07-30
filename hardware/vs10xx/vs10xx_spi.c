/***************************************************************************
 *            vs10xx_spi.c
 *
 *  Thu Dec 15 19:18:51 2011
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
 
#include <avr/io.h>
#include <stdio.h>
#include <avr/pgmspace.h>

#include "config.h"

#if defined(VS10XX)

	#include "vs10xx_spi.h"
	#include "system/clock/clock.h"

#if !defined(__AVR_XMEGA__)
	#include "hardware/spi/spi_core.h"

	static int SPI_BUSID = -1;
#endif

// #define DEBUG

void VS10XX_SPI_init( void )
{
#if defined(__AVR_XMEGA__)
	MOSI_PORT.DIRSET = ( 1<<MOSI );
	MISO_PORT.DIRCLR = ( 1<<MISO );
	SCK_PORT.DIRSET = ( 1<<SCK );
	DCLK_PORT.DIRSET = ( 1<<DCLK );

	SCK_PORT.OUTCLR = ( 1<<SCK );
	DCLK_PORT.OUTCLR = ( 1<<DCLK );

	VS10XX_deselect;
	VS10XX_deselect_bsync;
#else
	SPI_BUSID = 0;
	SPI_init( SPI_BUSID );
#endif

}

char VS10XX_SPI_send_data( char data )
{
#if defined(__AVR_XMEGA__)
	unsigned char i;

#if defined(DEBUG)
	printf_P( PSTR("\r\nSPI-Data-Send: "));
#endif
		
	for( i = 0 ; i < 8 ; i++ )
	{

		// Kieck mal ob MSB 1 ist
		if ( ( data & 0x80 ) > 0 )
		{
#if defined(DEBUG)
			printf_P( PSTR("1 "));
#endif
			MOSI_PORT.OUTSET = ( 1<<MOSI );
		}
		else
		{
#if defined(DEBUG)
			printf_P( PSTR("0 "));
#endif
			MOSI_PORT.OUTCLR = ( 1<<MOSI );
		}										

		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		// SCK auf High setzen
		DCLK_PORT.OUTSET = ( 1<<DCLK );
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		data <<= 1;
		// SCK auf Low setzen
		DCLK_PORT.OUTCLR = ( 1<<DCLK );
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");

	}

	return ( 0 );

#else
	return ( SPI_ReadWrite( SPI_BUSID, data ) );
#endif
}

char VS10XX_SPI_send_cmd( char data )
{
#if defined(__AVR_XMEGA__)
	unsigned char i;
	char misobyte = 0;
	
#if defined(DEBUG)
	printf_P( PSTR("\r\nSPI-CMD-Send: "));
#endif
		
	for( i = 0 ; i < 8 ; i++ )
	{
		// Kieck mal ob MSB 1 ist
		if ( ( data & 0x80 ) > 0 )
		{
#if defined(DEBUG)
			printf_P( PSTR("1"));
#endif
			MOSI_PORT.OUTSET = ( 1<<MOSI );
		}
		else
		{
#if defined(DEBUG)
			printf_P( PSTR("0"));
#endif
			MOSI_PORT.OUTCLR = ( 1<<MOSI );
		}
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		// SCK auf High setzen
		SCK_PORT.OUTSET = ( 1<<SCK );
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");

		data <<= 1;
		misobyte <<= 1;
		
		if ( ( MISO_PORT.IN & ( 1 << MISO ) ) > 0 )
		{
#if defined(DEBUG)
			printf_P( PSTR("1 "));
#endif
			misobyte |= 0x01;
		}
		else
		{
#if defined(DEBUG)
			printf_P( PSTR("0 "));
#endif
		}
		
		// SCK auf High setzen
		SCK_PORT.OUTCLR = ( 1<<SCK );
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
	}

	return ( misobyte );

#else
	return ( SPI_ReadWrite( SPI_BUSID, data ) );
#endif
}

#endif