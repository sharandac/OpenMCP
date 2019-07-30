/***************************************************************************
 *            1wire.h
 *
 *  Mon Apr 27 23:28:56 2009
 *  Copyright  2009  Dirk Broßwick
 *  <sharandac@snafu.de>
 *
 * Der Code ist in Anlehnung an Peter Dannegger seinem Code geschrieben.
 * Veröffentlicht unter http://www.mikrocontroller.net/topic/14792 .
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

#ifndef _1WIRE_H
	#define _1WIRE_H

	#include <avr/pgmspace.h>

#if defined(OpenMCP)
	#error "1-Wire wird für diese Hardwareplatform nicht unterstützt, bitte in der config.h abwählen!"
#endif

#if defined(myAVR)
		#define DQ_PIN	PB7
		#define DQ_IN	PINB
		#define DQ_OUT	PORTB
		#define DQ_DDR	DDRB
#elif defined(AVRNETIO)
		#define DQ_PIN	PB1
		#define DQ_IN	PINB
		#define DQ_OUT	PORTB
		#define DQ_DDR	DDRB
#elif defined(ATXM2)
		#define DQ_PIN	0
		#define DQ_IN	PORTC.IN
		#define DQ_OUT	PORTC.OUT
		#define DQ_DDR	PORTC.DIR
#endif

	#define	ONEWIRE_RESET			480
	#define	ONEWIRE_RESET_PAUSE		90
	#define	ONEWIRE_RESET_DELAY		390
	#define	ONEWIRE_BITIO_ZERO		1
	#define	ONEWIRE_BITIO_ONE		14
	#define	ONEWIRE_BITIO_PAUSE		45

	#define MATCH_ROM		0x55
	#define SKIP_ROM		0xCC
	#define	SEARCH_ROM		0xF0
	#define CONVERT_T		0x44
	#define READ			0xBE
	#define WRITE			0x4E
	#define EE_WRITE		0x48
	#define EE_RECALL		0xB8

	#define	SEARCH_FIRST	0x40
	#define	PRESENCE_ERR	0xFF
	#define	DATA_ERR		0xFE
	#define	ID_ERR			0xFD
	#define LAST_DEVICE		0x00


	char ONEWIRE_reset( void );
	char ONEWIRE_bitio( char BIT );
	unsigned int ONEWIRE_writebyte( unsigned char BYTE );
	unsigned int ONEWIRE_readbyte( void );
	void ONEWIRE_ParasitepowerOn( void );
	char ONEWIRE_searchrom( char diff, char * id );
	void ONEWIRE_command( char command, char * id );
	char ONEWIRE_checkID( char * ID );
	const char * ONEWIRE_getfamilycode2string( char id );

	typedef struct {
		const char	id;
		const char 	* familystring;
	} const FAMILY ;

#endif /* 1WIRE_H */
