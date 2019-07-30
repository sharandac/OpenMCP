/***************************************************************************
 *            twi.c
 *
 *  Tue Oct  6 22:29:59 2009
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <compat/twi.h>
#include <util/delay.h>

/**
 * \ingroup Hardware
 * \addtogroup TWI Stellt Funktioen für den TWI-Bus(I2C) bereit (twi.c)
 * \date	12-05-2010: TWI neu implementiert, das alte hatte so seine Fehler die ich nicht
 *			tolerieren konnte.
 * \date	12-06-2010: So, noch eine bisschen Foo um den Bus zu erkennen ob der tut.
 *
 * @{
 */

/**
 *
 * \author Dirk Broßwick
 *
 */

#include "config.h"

#if defined(TWI)

#include "twi.h"

volatile char twi_status = FALSE;

/**
 * \brief Initialisiert den TWI-Bus
 */
char TWI_Init ( long TWI_Bitrate )
{
	int i;
	
	TWSR = 0;								/* no prescaler */
	TWBR = ((F_CPU/TWI_Bitrate)-16)/2;

//	TWBR = ( ( F_CPU / ( 2 * TWI_Bitrate ) ) - 5 ) ;
	
	if (TWBR > 11)
	{
		// sende eine START condition zum testen
		TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);

		for ( i = 0 ; i < TWITIMEOUT ; i++ )
		{
			if ( TWCR & ( 1<<TWINT ) )
			{
				twi_status = TRUE;
				TWCR = 0;
				break;
			}
			_delay_us( 10 );
		}
	}
	return twi_status;
}

/**
 * \brief Öffnet den TWI-Bus für eine bestimmte Slaveadresse lesend/schreibend.
 * \param	Address		Adresse des TWI-Slaveadresse
 * \param	TWI_RW		Lesend oder schreibend (TWI_READ or TWI_WRITE)
 * \return	TRUE oder FALSE
 */
char TWI_SendAddress ( char Address, char TWI_RW )
{
	char retval = FALSE;

	if ( twi_status == TRUE )
	{
		// TWI, Start oder Rep-Start oder IDLE ?
		if ( ( ( TW_STATUS & 0xF8 ) == TWI_START ) || ( ( TW_STATUS & 0xF8 ) == TWI_REP_START ) || ( ( TW_STATUS & 0xF8 ) == 0xF8 ) \
	/*
				|| ( ( TW_STATUS & 0xF8 ) == TWI_MTX_ADR_ACK ) || ( T( TW_STATUS & 0xF8 ) == TWI_MTX_DATA_ACK ) \
				|| ( ( TW_STATUS & 0xF8 ) == TWI_MTX_ADR_NACK ) || ( ( TW_STATUS & 0xF8 ) == TWI_MTX_DATA_NACK ) \
				|| ( ( TW_STATUS & 0xF8 ) == TWI_MRX_ADR_ACK ) || ( ( TW_STATUS & 0xF8 ) == TWI_MRX_DATA_ACK ) \
				|| ( ( TW_STATUS & 0xF8 ) == TWI_MRX_ADR_NACK ) || ( ( TW_STATUS & 0xF8 ) == TWI_MRX_DATA_NACK )
	*/																							)
		{
			// send START condition
			TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
			// warte bis vorgang abgeschlossen ist
			while (!(TWCR & (1<<TWINT)));
			// schau mal ob Start erfolgreich war ?
			switch( TW_STATUS & 0xF8 )
			{
				case TWI_START:			// Sende SLA+W
										TWDR = (Address<<1) + TWI_RW;
										TWCR = (1<<TWINT)|(1<<TWEN);
										retval = TRUE;
										break;
				case TWI_REP_START:		// Sende SLA+W
										TWDR = (Address<<1) + TWI_RW;
										TWCR = (1<<TWINT)|(1<<TWEN);
										retval = TRUE;
										break;
				default:				TWI_SendStop();
										retval = FALSE;
			}
			// warte bis vorgang abgeschlossen ist
			while (!(TWCR & (1<<TWINT)));
			// Bis jetzt noch alles Okay ?
			if ( retval == TRUE )
			{
				// soll geschrieben oder gelesen werden ?
				if ( TWI_RW == TWI_WRITE )
				{
					switch( TW_STATUS & 0xF8 )
					{
						// ein ACK von Slave erhalten ? Wenn nicht Fehler.
						case TWI_MTX_ADR_ACK:	retval = TRUE;
												break;
						case TWI_MTX_ADR_NACK:	TWI_SendStop();
						default:				retval = FALSE;
					}
				}
				else
				{
					switch( TW_STATUS & 0xF8 )
					{
						case TWI_MRX_ADR_ACK:	retval = TRUE;
												break;
						case TWI_MRX_ADR_NACK:	TWI_SendStop();
						default:				retval = FALSE;
					}
				}
			}
		}
	}
	return( retval );
}

/**
 * \brief Sendet die Stopcondition auf dem TWI.
 */
void TWI_SendStop ( void )
{
	if ( twi_status == TRUE )
	{
		/* send stop condition */
		TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
		// wait until stop condition is executed and bus released
		while( TWCR & ( 1<<TWSTO ) );
	}
}

/**
 * \brief Sendet ein Byte an den vorher selektierten Slave.
 * \param	Data	Byte welches gesendet werden soll.
 * \return	TRUW oder FALSE
 */
char TWI_Write ( char Data )
{
	char	retval = FALSE;
	
	if ( twi_status == TRUE )
	{
		// letzte Übertragung erfolgreich ?
		if ( ( ( TW_STATUS & 0xF8 ) == TWI_MTX_ADR_ACK ) || ( ( TW_STATUS & 0xF8 ) == TWI_MTX_DATA_ACK ) )
		{
			// sendet Daten an einen vorher selektierten Slave
			TWDR = Data;
			TWCR = (1<<TWINT) | (1<<TWEN);
			// warte auf ende der Aktion
			while(!(TWCR & (1<<TWINT)));

			// Wenn übertragung nicht erfolgreich, Fehler
			if ( ( TW_STATUS & 0xF8 ) == TWI_MTX_DATA_ACK )
			{
				retval = TRUE;
			}
			else
			{
				TWI_SendStop();
			}
		}
	}
	return( retval );
}

/**
 * \brief Liest Daten vom Slave und sendet ACK
 * \return	TRUE oder FALSE
 */
char TWI_ReadAck ( void )
{
	if ( twi_status == TRUE )
	{
		TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
		while(!(TWCR & (1<<TWINT)));
	}
	return( TWDR );
}

/**
 * \brief Liest Daten vom Slave und sendet NACK
 * \return	TRUE oder FALSE
 */
char TWI_ReadNack ( void )
{
	if ( twi_status == TRUE )
	{
		TWCR = (1<<TWINT) | (1<<TWEN);
		while(!(TWCR & (1<<TWINT)));
	}
	return( TWDR );
}

/**
 * \brief Scannt den TWI-Bus.
 * \return	Anzahl der gefundenen Geräte.
 */
int TWI_Scan( void )
{
	unsigned char i;
	int Devices=0;

	if ( twi_status == TRUE )
	{
		for ( i = 0x10; i <= 127 ; i++ )
		{
			if ( TWI_SendAddress( i , TWI_WRITE ) == TRUE )
			{
				Devices++;
				TWI_SendStop();
			}
		}
	}
	return( Devices );
}#endif
/**
 * @}
 */
