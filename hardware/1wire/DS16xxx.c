/***************************************************************************
 *            DS16xxx.c
 *
 *  Wed Dec 23 17:04:05 2009
 *  Copyright  2009  Dirk Broßwick
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

/**
 *
 * \ingroup ONE_WIRE
 * \addtogroup DS16XXX Stellt Funktionen zum auslesen der DS16XXX Familie bereit.
 *
 * @{
 */

/**
 *
 * \author Dirk Broßwick
 *
 */

#include "system/clock/clock.h"
#include "config.h"

#if defined(ONEWIRE)

#include "1wire.h"
#include "DS16xxx.h"
#include "system/sensor/temp.h"

/**
 * \brief Liest einen DS16XXX aus.
 * \param	id	Pointer auf die 64Bit ID des DS16XXX Tempsensors.
 * \return	Temperatur im 8.8 Format oder TEMP_ERROR
 */
int DS16xxx_readtemp( char * id )
{
	int Temp;	
	
	// Wenn Temperatursensor, dann Messung starten und Temperatur ausgeben
	if( * id == 0x28 || * id == 0x22 || * id == 0x10 )
	{
		ONEWIRE_command( CONVERT_T, id );
		ONEWIRE_ParasitepowerOn( );
		CLOCK_delay( 1000 );
		ONEWIRE_command( READ , id );
		Temp = ONEWIRE_readbyte( );
		Temp |= ( ONEWIRE_readbyte( ) << 8 );

		if( * id == 0x10 )			// 9 -> 12 bit
    		Temp <<= 3;

		Temp = ( Temp << 4 );
	}
	else
	{
		Temp = TEMP_ERROR;
	}
	return( Temp );
}

#endif

/**
 * @}
 */

