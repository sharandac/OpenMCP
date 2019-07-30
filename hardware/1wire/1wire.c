/***************************************************************************
 *            1wire.c
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

/**
 *
 * \ingroup Hardware
 * \addtogroup ONE_WIRE Interface für den 1-Wirebus (1wire.c)
 *
 * @{
 */

/**
 *
 * \author Dirk Broßwick
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#if defined(__AVR_XMEGA__)
	#include <system/clock/delay_x.h>
#else
	#include "util/delay.h"
#endif
#include "system/math/crc8.h"
#include "config.h"

#if defined(ONEWIRE)

#include "1wire.h"

const char FAMILY_10[] PROGMEM = "DS18S20";
const char FAMILY_22[] PROGMEM = "DS1822";
const char FAMILY_28[] PROGMEM = "DS18B20";
const char FAMILY_UNKOWN[] PROGMEM = "unkown family code";

FAMILY family[ ] = {
	{ 0x10, FAMILY_10 } ,
//	{ 0x1B, FAMILY_1B } ,
	{ 0x22, FAMILY_22 } ,
	{ 0x28, FAMILY_28 } ,
//	{ 0x29, FAMILY_29 } ,
//	{ 0x2c, FAMILY_2C } ,
	{ 0, FAMILY_UNKOWN }
};

/**
 * \brief Resetet alle 1-Wire Geräte auf dem Bus.
 * \param	NONE
 * \return  NONE
 */
char ONEWIRE_reset( void )
{	char sreg_tmp;
	char Error;

	// Interrups sperren
	sreg_tmp = SREG;
	cli();

	// DS Ausgang setzen auf Low	DQ_OUT &= ~(1<<DQ_PIN);
	DQ_DDR |= 1<<DQ_PIN;

	// 480µs warten für Reset
	_delay_us( 480 );

	// DS auf Eingang setzen
	DQ_DDR &= ~(1<<DQ_PIN);
	
	// warten
	_delay_us( 90 );
	
	// immer noch Power ? Dann Fehler
	Error = DQ_IN & ( 1<<DQ_PIN );
		
	// Warten
	_delay_us( 380 );
	
	if( ( DQ_IN & ( 1<<DQ_PIN ) ) == 0 )
		Error = 1;

	SREG = sreg_tmp;

	return Error;
}

/**
 * \brief Sendet/Empfängt einzelne Bits auf dem 1-Wirebus.
 * \param	BIT		Einzelnes Bit welches gesendet werden soll.
 * \return  0 oder 1
 */
char ONEWIRE_bitio( char BIT )
{
	char sreg_tmp;
	sreg_tmp = SREG;
	cli();
	
	DQ_DDR |= 1<<DQ_PIN;
	_delay_us( 1 );

	if( BIT )
		DQ_DDR &= ~(1<<DQ_PIN);
	
	_delay_us( 14 );
	
	if( (DQ_IN & ( 1<<DQ_PIN ) ) == 0 )
		BIT = 0;
	
	_delay_us( 45 );

	DQ_DDR &= ~( 1<<DQ_PIN );
	
	_delay_us( 1 );

	SREG = sreg_tmp;

	return BIT;

}

/**
 * \brief Schreibt ein Byte auf den 1-Wirebus
 * \param	BYTE	Byte welches gesendet werden soll.
 * \return  Echo des gesendeten Bytes.
 */
unsigned int ONEWIRE_writebyte( unsigned char BYTE )
{
	char i = 8, j;

	do{
		j = ONEWIRE_bitio( BYTE & 1 );
		BYTE >>= 1;
		if( j )
			BYTE |= 0x80;
	}while( --i );
  
	return BYTE;
}

/**
 * \brief Resetet alle 1-Wire Geräte auf dem Bus.
 * \return  Empfangendes Byte.
 */
unsigned int ONEWIRE_readbyte( void )
{
  return ONEWIRE_writebyte( 0xFF );
}


/**
 * \brief Schaltest Spannung auf den Datenpin um alle Geräte am Bus zu Versorgen.
 * \param	NONE
 * \return  NONE
 */
void ONEWIRE_ParasitepowerOn( void )
{
	DQ_OUT |= 1<< DQ_PIN;
	DQ_DDR |= 1<< DQ_PIN;
}

/**
 * \brief Sucht nach 1-Wire-Geräten auf dem Bus.
 */
char ONEWIRE_searchrom( char diff, char * id )
{	
	char j, next_diff, b;

	int i;
	if( ONEWIRE_reset() )	
		return PRESENCE_ERR;

	ONEWIRE_writebyte( SEARCH_ROM );
	next_diff = LAST_DEVICE;

	i = 64;
	do{
		j = 8;
		do{
		b = ONEWIRE_bitio( 1 );
		if( ONEWIRE_bitio( 1 ) )
		{			if( b )
				return DATA_ERR;
  		}
		else
		{			if( !b )
			{
				if( diff > i || ((*id & 1) && diff != i) )
				{
					b = 1;
					next_diff = i;
				}
			}		}
 
				
		ONEWIRE_bitio( b );
		*id >>= 1;
		if( b )			*id |= 0x80;
  		i--;
		}while( --j );
		

		id++;
	
	}while( i );

	// Pointer wieder auf den Anfang der ID stellen
	id = id - 8;

	// CRC8 check der ID
	if ( ONEWIRE_checkID ( id ) != LAST_DEVICE )
		next_diff = ID_ERR;
	
	return next_diff;
}

/**
 * \brief Sendet ein Befehl an den 1-Wireteilnehmer mit einer bestimmten ID
 * \param	command		Befehl der gesendet werden soll.
 * \param	id			Pointer auf die 64Bit ID.
 * \return  NONE
 */
void ONEWIRE_command( char command, char * id )
{
	char i;
	
	ONEWIRE_reset();

	if( id )
	{		
	    ONEWIRE_writebyte( MATCH_ROM );
		i = 8;
		do{
			ONEWIRE_writebyte( * id );
  			id++;
		}while( --i );
	}
	else
	{
		ONEWIRE_writebyte( SKIP_ROM );
	}
	ONEWIRE_writebyte( command );
}

/**
 * \brief Liefert eine Beschreibung zum 1-Wireteilnehmer wenn Typ bekannt ist.
 * \param	id		Ersten Byte der id.
 * \return  Pointer auf einen String im Flash mit der Beschreibung.
 */
const char * ONEWIRE_getfamilycode2string( char id )
{
	int i = 0;
	
	while ( family[ i ].id != 0 )
	{
		if ( family[ i ].id == id ) break;
		i++;
	}
	return( family[ i ].familystring );
}


/**
 * \brief Prüft die CRC der ID des 1-Wire Gerätes.
 * \param	ID		Pointer auf die ID		
 * \return  LAST_DEVICE für Ok und ID_ERR bei Fehler
 */
char ONEWIRE_checkID( char * ID )
{
	char retval = ID_ERR;

	if ( crc8( ID , 7 ) == ID[7])
		retval = LAST_DEVICE;
		
	return( retval );
}

#endif

/**
 * @}
 */
