/***************************************************************************
 *            xmega_spi.c.h
 *
 *  Sun May 16 18:14:04 2010
 *  Copyright  2010  Dirk Broßwick
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
 
#ifndef _XMEGA_SPI_H
	#define _XMEGA_SPI_H

	#define SPI_SS_bm             0x10 /*!< \brief Bit mask for the SS pin. */
	#define SPI_MOSI_bm           0x20 /*!< \brief Bit mask for the MOSI pin. */
	#define SPI_MISO_bm           0x40 /*!< \brief Bit mask for the MISO pin. */
	#define SPI_SCK_bm            0x80 /*!< \brief Bit mask for the SCK pin. */

	void xmega_spi_init( int SPI_ID );
	char xmega_spi_ReadWrite( int SPI_ID, char Data );
	void xmega_spi_WriteBlock( int SPI_ID, char * Block, int len );
	void xmega_spi_ReadBlock( int SPI_ID, char * Block, int len );

#endif
