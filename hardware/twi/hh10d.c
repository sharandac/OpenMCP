/***************************************************************************
 *            hh10D.c
 *
 *  Sun Aug  7 18:27:25 2011
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

#include <avr/interrupt.h>
#include <avr/io.h>
#include "hardware/twi/twi.h"
#include "hardware/timer1/timer1.h"

#include "config.h"

#if defined( HH10D )

#include "hh10d.h"

static HH10D_STATE = 0;

void HH10D_init( void )
{
	if ( TWI_SendAddress( HH10D_TWI_ADDR , TWI_READ ) == TRUE )
	{
		HH10D_STATE = 1;
	}
	TWI_SendStop();
}

int	HH10D_meas()
{
	int offset, sens, i,freq = 0;
	char temp_sreg;

	if ( TWI_SendAddress( HH10D_TWI_ADDR, TWI_WRITE ) == TRUE )
	{
		TWI_Write ( 10 );
		TWI_SendStop();
		TWI_SendAddress( HH10D_TWI_ADDR, TWI_READ );
		sens = TWI_ReadAck() << 8;
		sens |= TWI_ReadAck();
		offset = TWI_ReadAck() << 8;
		offset |= TWI_ReadNack();
	}
	TWI_SendStop();
	
	temp_sreg = SREG;
	cli();

	while( bit_is_set( TIFR1 , OCF1A) );
	TIFR1 = (1<<OCF1A);

	for ( i = 0 ; i < 100 ; )
	{
		if( bit_is_set( TIFR1 , ICF1 ) )
		{	
			TIFR1 = (1<<ICF1);
			freq++;
		}
		if ( bit_is_set( TIFR1 , OCF1A) )
		{
			i++;
			TIFR1 = (1<<OCF1A);
		}
	}
	
	SREG = temp_sreg;
	return( ( offset - freq ) / 10 );
//	return ( ( offset - freq  ) / (4096 / sens ) );
//	return ( freq );
}

#endif
