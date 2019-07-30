/***************************************************************************
 *            ds1307.c
 *
 * Controller	: ATmega644p (Clock: 20000 Mhz-internal)
 * Compiler		: AVR-GCC (winAVR with AVRStudio)
 * Version 		: 2.4
 * Author		: CC K-H Legat
 * Date			: 18 Januar 2011
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
#include <avr/pgmspace.h>
#include <stdio.h>

/**
 * \ingroup TWI
 * \addtogroup DS1307 Stellt Funktionen zum auslesen eines DS1307 RTC-IC bereit.
 *
 * @{
 */

/**
 * \author CC K-H Legat
 */

#include "config.h"

#if defined(TWI)

#include "ds1307.h"
#include "system/math/math.h"
#include "hardware/twi/twi.h"
#include "system/clock/clock.h"

/**
 * \brief Function to Read the Time from the TWI RTC and bring back to Caller
 * \param	Time	Pointer to a struct.
 * \retval	int		DS1307_OK or DS1307_ERROR.
 */
int DS1307_GetTime( struct TIME * Timestruct )
{
	struct DS1307_MEM DS1307;
	int i;
	char * DS1307_data;
	int retval = DS1307_ERROR;

	// baue Verbindung zum DS1307 auf
	if ( TWI_SendAddress( DS1307_ADDR , TWI_WRITE ) == TRUE )
	{
		DS1307_data = (char *)&DS1307;

		// Setze den Read/WritePointer
		TWI_Write( 0 );
		// Verbindung beenden
		TWI_SendStop();

		// Oeffne neue Verbindung
		if ( TWI_SendAddress( DS1307_ADDR , TWI_READ ) == TRUE )
		{
			// lese die RTC ein
			for ( i = 0 ; i < sizeof( struct DS1307_MEM ) ; i++ )
			{
				* DS1307_data++ = TWI_ReadAck();
				if ( i == ( sizeof( struct DS1307_MEM ) - 1 ) )
				{
					* DS1307_data++ = TWI_ReadNack() ;
				}
			}
			// schließe Verbindung
			TWI_SendStop();

			// hole die aktuelle Zeit vom Controller
			CLOCK_GetTime( Timestruct );

			// setze die Zeit vom DS1307
			Timestruct->ms = 0;
			Timestruct->ss = bcd2bin( DS1307.RTC_SECONDS );
			Timestruct->mm = bcd2bin( DS1307.RTC_MINUTES );
			Timestruct->hh = bcd2bin( DS1307.RTC_HOURS );
			Timestruct->WW = bcd2bin( DS1307.RTC_DAY );
			Timestruct->DD = bcd2bin( DS1307.RTC_DATE );
			Timestruct->MM = bcd2bin( DS1307.RTC_MONTH );
			Timestruct->YY = bcd2bin( DS1307.RTC_YEAR ) + 2000 ;

			// berechne die UTC
			CLOCK_encode_time( Timestruct );

			// alles ok
			retval = DS1307_OK;
		}
/*
		 else
		{
			printf_P(PSTR("RTC Read failed.."));
		}
*/
	}
/*
	 else
	{
		printf_P(PSTR("RTC sendAddress1 failed.."));
	}
*/
	return( retval );
}

/**
 * \brief Function to update the DS1307 for next writing
 * \param	Time	Pointer to a struct.
 * \retval	int		DS1307_OK or DS1307_ERROR.
 */
int DS1307_SetTime( struct TIME * Timestruct )
{
	struct DS1307_MEM DS1307;

	int i;
	char * DS1307_data;
	int retval = DS1307_ERROR;

	DS1307.RTC_SECONDS = bin2bcd( Timestruct->ss ) | DS1307_CH;
	DS1307.RTC_MINUTES = bin2bcd( Timestruct->mm );
	DS1307.RTC_HOURS = bin2bcd( Timestruct->hh ) | DS1307_24H ;
	DS1307.RTC_DAY = bin2bcd( Timestruct->WW );
	DS1307.RTC_DATE = bin2bcd( Timestruct->DD );
	DS1307.RTC_MONTH = bin2bcd( Timestruct->MM );
	DS1307.RTC_YEAR = bin2bcd( (char) ( Timestruct->YY - 2000 ) );

	if ( TWI_SendAddress( DS1307_ADDR , TWI_WRITE ) == TRUE )
	{
		DS1307_data = (char *)&DS1307;

		TWI_Write( 0 );

		for ( i = 0 ; i < sizeof( struct DS1307_MEM ) ; i++ )
		{
			TWI_Write( * DS1307_data++ );
		}
		TWI_SendStop();

		retval = DS1307_OK;
	}
/*
	 else
	{
		printf_P(PSTR("RTC Write failed.."));

		return 1;
	}
*/
	return( retval );
}

#endif 

/**
 * @}
 */

 
