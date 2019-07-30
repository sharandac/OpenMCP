/***************************************************************************
 *            vs10xx_spi.h
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

#ifndef _VS10XX_SPI

	#define _VS10XX_SPI

	char VS10XX_SPI_init();
	char VS10XX_SPI_send_data( char );
	char VS10XX_SPI_send_cmd( char );

	// VS10XX Port
	#define VS10XX_SPI_PORT_PORT	PORTC
	#define VS10XX_SPI_PORT_PIN		PINC
	#define VS10XX_SPI_PORT_DDR		DDRC

	// Pinning VS10XX nach Xmega
	#define VS10XX_SPI_MISO		0
	#define VS10XX_SPI_DCLK		1	
	#define VS10XX_SPI_SCK		2
	#define VS10XX_SPI_MOSI		3
	#define VS10XX_SPI_CS		4
	#define VS10XX_SPI_RESET	5
	#define VS10XX_SPI_DREQ		6
	#define VS10XX_SPI_BSYNC	7

#endif // _VS10XX_SPI