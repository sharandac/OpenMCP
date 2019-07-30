/***************************************************************************
 *            xmega_spi.c.c
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

#include <avr/io.h>
#include "xmega_spi.h"

#include <avr/pgmspace.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#if defined(__AVR_XMEGA__)

void xmega_spi_init( int SPI_ID )
{
	// Set SPI baseaddress errechnen
	SPI_t * spi = (void *) ( 0x08c0 | ( SPI_ID << 8 ) );
	// Set PORT baseaddress errechnen
	PORT_t * port = (void *) ( 0x0600 | ( ( SPI_ID + 2 ) << 5 ) );

	/* MOSI and SCK as output. */
	port->OUTSET |= SPI_MOSI_bm | SPI_SCK_bm | SPI_SS_bm ;
	port->DIRSET |= SPI_MOSI_bm | SPI_SCK_bm | SPI_SS_bm ;
	/* MISO as Input. */
	port->DIRCLR = SPI_MISO_bm;

	spi->CTRL = SPI_ENABLE_bm | SPI_MASTER_bm ;
//	spi->CTRL = SPI_CLK2X_bm | SPI_ENABLE_bm | SPI_MASTER_bm;
}

char xmega_spi_ReadWrite( int SPI_ID, char Data )
{
	// Set SPI baseaddress errechnen
	SPI_t * spi = (void *) ( 0x08c0 | ( SPI_ID << 8 ) );

	// daten senden
	spi->DATA = Data;
	// auf fertig warten
	while( !(spi->STATUS & SPI_IF_bm ) );
	// daten zurueckgeben
	return( spi->DATA );
	
}

void xmega_spi_WriteBlock( int SPI_ID, char * Block, int len )
{
	// Set SPI baseaddress errechnen
	SPI_t * spi = (void *) ( 0x08c0 | ( SPI_ID << 8 ) );
	char data;

	// ersten Wert senden
	spi->DATA = *Block++;
	len--;

	while( len )
	{
		// Wert schon mal in Register holen, schneller da der Wert jetzt in einem Register steht und nicht mehr aus dem RAM geholt werden muss
		// nachdem das senden des vorherigen Wertes fertig ist,
		data = *Block++;
		// warten auf fertig
		while( !(spi->STATUS & SPI_IF_bm ) );
		// Wert aus Register senden
		spi->DATA = data;
		// Counter erhöhen
		len--;
	}
	while( !(spi->STATUS & SPI_IF_bm ) );

	return;
}

void xmega_spi_ReadBlock( int SPI_ID, char * Block, int len )
{
	// Set SPI baseaddress errechnen
	SPI_t * spi = (void *) ( 0x08c0 | ( SPI_ID << 8 ) );
	char data;
	
	// dummy wert senden
	spi->DATA = 0x00;
	// auf fertig warten
	while( len )
	{
		len--;
		// warten auf fertig
		while( !(spi->STATUS & SPI_IF_bm ) );
		// Dummy Wert senden
		spi->DATA = 0x00;
		// Daten einlesen
		data = spi->DATA;
		// speichern
		*Block++ = data;
	}
	while( !(spi->STATUS & SPI_IF_bm ) );

	return;
}

#endif
