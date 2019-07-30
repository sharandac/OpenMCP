/***************************************************************************
 *            spi_2.h
 *
 *  Mon Jul 31 21:46:47 2006
 *  Copyright  2006  Dirk Broßwick
 *  Email: sharandac@snafu
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
 
#ifndef _SPI_2_H
	#define _SPI_2_H

	#include <avr/io.h>

	void SPI2_init( void );
	char SPI2_ReadWrite( char Data );
	void SPI2_FastMem2Write( char * buffer, int Datalenght );
	void SPI2_FastRead2Mem( char * buffer, int Datalenght );


#if defined(__AVR_AT90CAN128__)

	#define SPI2_PORT		PORTE
	#define SPI2_DDR		DDRE
	#define SPI2_PIN		PINE
	#define MISO2			PE0
	#define MOSI2			PE1
	#define SCK2			PE2

#elif defined(__AVR_ATmega2561__)

	#define SPI2_PORT		PORTB
	#define SPI2_DDR		DDRB
	#define SPI2_PIN		PINB
	#define MISO2			PB2
	#define MOSI2			PB3
	#define SCK2			PB1

#elif defined(__AVR_ATmega644__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega1284P__) 

	#define SPI2_PORT		PORTD
	#define SPI2_DDR		DDRD
	#define SPI2_PIN		PIND
	#define MISO2			PD4
	#define MOSI2			PD2
	#define SCK2			PD5

#endif

#endif /* _SPI_2_H */
