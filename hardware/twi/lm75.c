/***************************************************************************
 *            lm75.c
 *
 *  Wed Dec 23 18:00:02 2009
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

#include "config.h"

/**
 * \ingroup TWI
 * \addtogroup LM75 Stellt Funktionen zum auslesen eines LM75-Sensor bereit.
 *
 * @{
 */

/**
 * \author Dirk Broßwick
 */

#if defined(TWI)

#include "twi.h"
#include "lm75.h"
#include "system/sensor/temp.h"

/**
 * \brief Liest einen LM75 Sensor aus.
 * \param	id	Slaveadresse des LM75 Sensors.
 * \return	Temperatur im 8.8 Format oder TEMP_ERROR
 */
int LM75_readtemp( char * id )
{
	int Temp;

	Temp = TEMP_ERROR;

	if ( * id > 0x47 && * id < 0x50 )
	{
		if ( TWI_SendAddress( * id , TWI_WRITE ) == TRUE )
		{
			TWI_Write( 0 );
			TWI_SendStop();
			
			Temp = 0;
			
			TWI_SendAddress( * id , TWI_READ );
			Temp = ( TWI_ReadAck() << 8 );
			Temp |= ( TWI_ReadNack() );
			TWI_SendStop();
		}
	}
	return( Temp );
}
#endif

/**
 * @}
 */
