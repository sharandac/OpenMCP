/***************************************************************************
 *            zg2100.c
 *
 *  Tue Sep  3 18:42:42 2013
 *  Copyright  2013  Dirk Broßwick
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

#include "config.h"

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include <avr/io.h>

#if defined USE_ZG2100

#include "zg2100_com.h"
#include "zg2100_hardware.h"
#include "zg2100_driver.h"
#include "zg2100_def.h"

#if defined(__AVR_XMEGA__)
	#include <system/clock/delay_x.h>
#else
	#include "util/delay.h"
#endif

#include "hardware/spi/spi_core.h"
#include "system/net/endian.h"

#define SOFTSPI
// #define ZG2100_DEBUG

static char cmd[ ZG2100_CMD_SIZE ];

void zg2100_com_init( void )
{
#if defined(SOFTSPI)
	/* MOSI and SCK as output. */
	ZG2100_PORT.OUTSET = (1<<ZG2100_MOSI | 1<<ZG2100_SCK );
	ZG2100_PORT.DIRSET = (1<<ZG2100_MOSI | 1<<ZG2100_SCK );
	/* MISO as Input. */
	ZG2100_PORT.DIRCLR = 1<<ZG2100_MISO;
#else
	SPI_init( ZG2100_SPI_ID );
#endif
	ZG2100_PORT.DIRSET = ( 1<<ZG2100_RESET | 1<<ZG2100_HIBERNATE | 1<<ZG2100_CS );
	ZG2100_PORT.OUTSET = ( 1<<ZG2100_RESET | 1<<ZG2100_CS );

	ZG2100_PORT.DIRCLR = ( 1<<ZG2100_INT );
	ZG2100_PORT.OUTCLR = ( 1<<ZG2100_INT | 1<<ZG2100_HIBERNATE );

	ZG2100_PORT.OUTCLR = ( 1<<ZG2100_RESET );
	_delay_ms( 250 );
	ZG2100_PORT.OUTSET = ( 1<<ZG2100_RESET );
	_delay_ms( 250 );
}

int zg2100_read_register( int addr )
{
	int len, * temp_reg;
	int result;

	len = ZG2100_REG_LEN( addr );

	cmd[0] = ( char )( ZG2100_CMD_REG_READ | addr );
	memset( &cmd[1], 0, len );

	zg2100_trans( cmd, len + 1, true, ZG2100_RW );

	if ( len == 1 )
	{
		result = cmd[1];
	}
	else
	{
		// caste char-pointer von cmd[1] auf int
		temp_reg = (int*)&cmd[1];
		result = htons( *temp_reg );
}
	return( result );
}

void zg2100_write_register( int addr, int reg )
{
	int len, * temp_reg;

	len = ZG2100_REG_LEN( addr );
	cmd[0] = ( char )( ZG2100_CMD_REG_WRITE | addr );

	if ( len == 1 )
	{
		cmd[1] = reg;
	}
	else
	{
		// caste char-pointer von cmd[1] auf int
		temp_reg = (int*)&cmd[1];
		*temp_reg = htons( reg );
	}
	
	zg2100_trans( cmd, len + 1, true, ZG2100_WRITE );	
}

int zg2100_indexregister_read( int addr )
{
	zg2100_write_register( ZG2100_REG_IREG_ADDR, addr );
	return( zg2100_read_register(ZG2100_REG_IREG_DATA ) );
}

void zg2100_indexregister_write( int addr , int reg )
{
	zg2100_write_register( ZG2100_REG_IREG_ADDR, addr );
	zg2100_write_register( ZG2100_REG_IREG_DATA, reg );	
}


void zg2100_fifo_read( char fifo, char * type, char * subtype, char * buffer, int len )
{
	cmd[0] = (fifo==ZG2100_FIFO_DATA)?ZG2100_CMD_FIFO_RD_DATA:ZG2100_CMD_FIFO_RD_MGMT;
	memset( &cmd[1], 0 , 2 );
	zg2100_trans( cmd, 3, false, ZG2100_RW );
	*type = cmd[1];
	*subtype = cmd[2];

	zg2100_trans( buffer, len - 2 , true, ZG2100_READ );
	cmd[0] = ZG2100_CMD_FIFO_RD_DONE;
	zg2100_trans( cmd, 1 , true, ZG2100_WRITE );
}

void zg2100_fifo_write( char fifo, char type, char subtype, char * buffer, int len, bool start, bool stop )
{
	if( start )
	{
		cmd[0] = (fifo==ZG2100_FIFO_DATA)?ZG2100_CMD_FIFO_WR_DATA:ZG2100_CMD_FIFO_WR_MGMT;
		cmd[1] = type;
		cmd[2] = subtype;
		zg2100_trans( cmd, 3 , false, ZG2100_WRITE );
	}

	zg2100_trans( buffer, len, true, ZG2100_WRITE );

	if( stop )
	{
		cmd[0] = ZG2100_CMD_FIFO_WR_DONE;
		zg2100_trans( cmd, 1 , true, ZG2100_WRITE );
	}
	
}

void zg2100_trans( char * buffer, int len, bool chipsel, char direction )
{
	int i;

#ifdef ZG2100_DEBUG
	if ( direction != ZG2100_READ )
	{
		printf_P( PSTR("dump out[%d]: "),len);
		for( i = 0 ; i < len ; i++ )
			printf_P( PSTR("%02x "), buffer[i]);

		if ( chipsel )
		{
			printf_P( PSTR("!cs"));
		}
		printf_P( PSTR("\r\n"));
	}
#endif
		
	ZG2100_PORT.OUTCLR = ( 1<<ZG2100_CS );

	switch( direction )
	{
		case ZG2100_READ:
#if defined(SOFTSPI)
						for( i = 0 ; i < len ; i++)
							buffer[i] = zg2100_soft_spi( 0 );
#else
						SPI_ReadBlock( ZG2100_SPI_ID, buffer, len );
#endif		
			
						break;
		case ZG2100_WRITE:	
#if defined(SOFTSPI)
						for( i = 0 ; i < len ; i++)
							zg2100_soft_spi( buffer[i] );
				
#else
						SPI_WriteBlock( ZG2100_SPI_ID, buffer, len );
#endif			
			break;
		case ZG2100_RW:
		default:		for( i = 0 ; i < len ; i++ )
#if defined(SOFTSPI)
							buffer[i] = zg2100_soft_spi( buffer[i] );
#else
							buffer[i] = SPI_ReadWrite( ZG2100_SPI_ID, buffer[i] );
#endif			
						break;
	}

	if ( chipsel )
	{
		ZG2100_PORT.OUTSET = ( 1<<ZG2100_CS );
	}

#ifdef ZG2100_DEBUG
	if ( direction != ZG2100_WRITE )
	{
			printf_P( PSTR("dump  in[%d]: "),len);
		for( i = 0 ; i < len ; i++ )
			printf_P( PSTR("%02x "), buffer[i]);

		if ( chipsel )
		{
			printf_P( PSTR("!cs"));
		}

		printf_P( PSTR("\r\n"));
	}
#endif


}

char zg2100_soft_spi( char byte )
{
	unsigned char i;
	char misobyte = 0;
			
	for( i = 0 ; i < 8 ; i++ )
	{
		// Kieck mal ob MSB 1 ist
		if ( ( byte & 0x80 ) > 0 )
			ZG2100_PORT.OUTSET = ( 1<<ZG2100_MOSI );
		else
			ZG2100_PORT.OUTCLR = ( 1<<ZG2100_MOSI );

		// SCK auf High setzen
		ZG2100_PORT.OUTSET = ( 1<<ZG2100_SCK );

		byte <<= 1;
		misobyte <<= 1;
		
		if ( ( ZG2100_PORT.IN & ( 1 <<ZG2100_MISO ) ) > 0 )
			misobyte |= 0x01;
		
		// SCK auf High setzen
		ZG2100_PORT.OUTCLR = ( 1<<ZG2100_SCK );
	}

	return ( misobyte );
}

#endif

