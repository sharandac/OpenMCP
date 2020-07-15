/***************************************************************************
 *            spi_1.c
 *
 *  Mon Jul 31 21:46:47 2006
 *  Copyright  2006  Dirk BroÃŸwick
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
 
#include <avr/io.h>
#include "spi_1.h"

#include "config.h"

#if !defined(__AVR_XMEGA__)

void SPI1_init( void )
{
#if defined(__AVR_ATmega2561__)
		UBRR0 = 0;
		/* Setting the XCKn port pin as output, enables master mode. */
		SPI_DDR |= ( 1 << XCK1 );
		/* Set MSPI mode of operation and SPI data mode 0. */
		UCSR0C = (1<<UMSEL01)|(1<<UMSEL00);
		/* Enable receiver and transmitter. */
		UCSR0B = (1<<RXEN0)|(1<<TXEN0);
		/* Set baud rate. */
		/* IMPORTANT: The Baud Rate must be set after the transmitter is enabled */
		UBRR0 = 0;
#elif defined(__AVR_ATmega644P__) || defined(__AVR_ATmega1284P__)
		UBRR1 = 0;
		/* Setting the XCKn port pin as output, enables master mode. */
		SPI_DDR |= ( 1 << XCK1 );
		/* Set MSPI mode of operation and SPI data mode 0. */
		UCSR1C = (1<<UMSEL11)|(1<<UMSEL10);
		/* Enable receiver and transmitter. */
		UCSR1B = (1<<RXEN1)|(1<<TXEN1);
		/* Set baud rate. */
		/* IMPORTANT: The Baud Rate must be set after the transmitter is enabled */
		UBRR1 = 0;
#endif
}

char SPI1_ReadWrite( char Data)
{
#if defined(__AVR_ATmega2561__)
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)) );
	/* Put data into buffer, sends the data */
	UDR0 = Data;
	/* Wait for data to be received */
	while ( !(UCSR0A & (1<<RXC0)) );
	/* Get and return received data from buffer */
	return UDR0;
#elif defined(__AVR_ATmega644P__) || defined(__AVR_ATmega1284P__)
	/* Wait for empty transmit buffer */
	while ( !( UCSR1A & (1<<UDRE1)) );
	/* Put data into buffer, sends the data */
	UDR1 = Data;
	/* Wait for data to be received */
	while ( !(UCSR1A & (1<<RXC1)) );
	/* Get and return received data from buffer */
	return UDR1;
#else
	return(-1);
#endif
	
}

void SPI1_FastMem2Write( char * buffer, int Datalenght )
{	
#if defined(__AVR_ATmega2561__)
	int Counter = 0;
	char data;

	// erten Wert senden
	UDR0 = buffer[ Counter++ ];
	while( Counter < Datalenght )
	{
		// Wert schon mal in Register holen, schneller da der Wert jetzt in einem Register steht und nicht mehr aus dem RAM geholt werden muss
		// nachdem das senden des vorherigen Wertes fertig ist,
		data = buffer[ Counter ];
		// warten auf fertig
		while ( !( UCSR0A & (1<<UDRE0)) );
		// Wert aus Register senden
		UDR0 = data;
		Counter++;
	}
	while ( !(UCSR0A & (1<<RXC0)) );
#elif defined(__AVR_ATmega644P__) || defined(__AVR_ATmega1284P__)
	int Counter = 0;
	char data;

	// erten Wert senden
	UDR1 = buffer[ Counter++ ];
	while( Counter < Datalenght )
	{
		// Wert schon mal in Register holen, schneller da der Wert jetzt in einem Register steht und nicht mehr aus dem RAM geholt werden muss
		// nachdem das senden des vorherigen Wertes fertig ist,
		data = buffer[ Counter ];
		// warten auf fertig
		while ( !( UCSR1A & (1<<UDRE1)) );
		// Wert aus Register senden
		UDR1 = data;
		Counter++;
	}
	while ( !( UCSR1A & (1<<UDRE1)) );
//	while ( !(UCSR1A & (1<<RXC1)) );
#endif
	return;
}

void SPI1_FastRead2Mem( char * buffer, int Datalenght )
{	
#if defined(__AVR_ATmega2561__)
	int Counter = 0;
	char data;
	
	// 20200715 dl6lr copy length bytes from spi, fix by one error
	while( Counter < Datalenght )
	{
		// warten auf fertig
		while ( !( UCSR0A & (1<<UDRE0)) );
		// dummywrite
		UDR0 = 0xFF;

		while ( !(UCSR0A & (1<<RXC0)) );
		// Daten einlesen
		data = UDR0;

		// speichern
		buffer[ Counter++ ] = data;
		// buffer[ Counter++ ] = SPI1_ReadWrite( 0x00 );
	}

	// warten auf fertig
	while ( !( UCSR0A & (1<<UDRE0)) );
//	while ( !(UCSR0A & (1<<RXC0)) );
#elif defined(__AVR_ATmega644P__) || defined(__AVR_ATmega1284P__)
	int Counter = 0;
	char data;

	// 20200715 dl6lr copy length bytes from spi, fix by one error
	while( Counter < Datalenght )
	{
		// warten auf fertig
		while ( !( UCSR1A & (1<<UDRE1)) );
		// dummywrite
		UDR1 = 0x00;

		while ( !(UCSR1A & (1<<RXC1)) );
		// Daten einlesen
		data = UDR1;

		// speichern
		buffer[ Counter++ ] = data;
		// buffer[ Counter++ ] = SPI1_ReadWrite( 0x00 );
	}
	// warten auf fertig
//	while ( !( UCSR1A & (1<<UDRE1)) );
//	while ( !(UCSR1A & (1<<RXC1)) );
#endif
	return;
}

#endif
