/***************************************************************************
 *            hd44780_twi.c
 *
 *  Thu Nov  5 17:02:56 2009
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
 * \ingroup lcd
 * \addtogroup hd44780_twi Support fuer HD44780 Display ueber TWI (hd44780_twi.c)
 *
 * Mit diesen Treiber koennen HD44780-Displays ueber TWI angesprochen werden.
 * @{
 */

/**
 * \file
 *
 * \author Dirk Broßwick
 */

#include <avr/io.h>
#include <util/delay.h>

#include "config.h"

#if defined(LCD) && defined(HD44780_TWI)

#include "hd44780_gen.h"
#include "hd44780_twi.h"
#include "hardware/twi/twi.h"
#include "system/clock/clock.h"

static char HD44780_TWI_Byte;
static char	HD44780_TWI_row;
static char	HD44780_TWI_column;

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Erzeugt ein Puls auf der ENABLE Leitung.
 * \param   NONE		None
 * \return	NONE		None
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
void HD44780_TWI_enable( void )
{
	if ( TWI_SendAddress( HD44780_TWI_ADDR , TWI_WRITE ) == TRUE )
	{
		HD44780_TWI_Byte &= ~( 1 << HD44780_TWI_E_PIN );
		TWI_Write( HD44780_TWI_Byte );
		_delay_us( 100 );
		HD44780_TWI_Byte |= ( 1 << HD44780_TWI_E_PIN );
		TWI_Write( HD44780_TWI_Byte );
		_delay_us( 90 );
		HD44780_TWI_Byte &= ~( 1 << HD44780_TWI_E_PIN );
		TWI_Write( HD44780_TWI_Byte );
		TWI_SendStop();
		_delay_us( 90 );
	}
}
 
/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Sendet eine 4 Bit Ausgabe an das LCD.
 * \param   data		Ausgabedaten für das LCD
 * \return	NONE		None
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
void HD44780_TWI_out( char data )
{
	HD44780_TWI_Byte &= ~HD44780_TWI_DATAMASK;
	HD44780_TWI_Byte |= ( data>>4 & HD44780_TWI_DATAMASK );

	// trigger enable line
    HD44780_TWI_enable();
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Initialisiert das LCD.
 * \param   NONE		None
 * \return	NONE		None
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
void HD44780_TWI_init( void )
{
	unsigned char i;
	int y;
	
	HD44780_TWI_Byte = 0;
	// erster Reset in den 8-Bit Modus
	HD44780_TWI_out(HD44780_CMD_SOFT_RESET);
	CLOCK_delay( HD44780_DELAY_SOFT_RESET1 );

	// zweiter Reset in den 8-Bit Modus
	HD44780_TWI_enable();
	CLOCK_delay( HD44780_DELAY_SOFT_RESET2 );

	// dritter Reset in den 8-Bit Modus
	HD44780_TWI_enable();
	CLOCK_delay( HD44780_DELAY_SOFT_RESET3 );

	// und jetzt endlich in den 4-Bit Modus
	HD44780_TWI_out(	HD44780_CMD_SET_FUNCTION |
	            	HD44780_SF_4BIT );
	
	CLOCK_delay( HD44780_DELAY_SET_4BITMODE );
	
	// Display einstellen, 4Bit, 2 Lines und Zeichensatz 5x8 Pixel
	HD44780_TWI_sendCMD( 	HD44780_CMD_SET_FUNCTION |
			 	 		HD44780_SF_4BIT | 			// 4 Bit mode
				 		HD44780_SF_2LINES | 		// 2 lines
				 		HD44780_SF_5x10 ); 		// 5x10

	// clear screen
	HD44780_TWI_sendCMD( HD44780_CMD_CLEAR );
	// Cursor auf Homeposition setzen
	HD44780_TWI_sendCMD( HD44780_CMD_CURSOR_HOME );
	// Display einschalten
	HD44780_TWI_sendCMD( HD44780_CMD_SET_DISPLAY |    
	             HD44780_SD_DISPLAY_ON /* |		// display on
				 HD44780_SD_CURSOR_ON |			// show cursor
				 HD44780_SD_BLINKING_ON */ );	// cursor blinking

	HD44780_TWI_row = 0;
	HD44780_TWI_column = 0;

	// fade in LCD-Backlight
	if ( TWI_SendAddress( HD44780_TWI_ADDR , TWI_WRITE ) == TRUE )
	{
		for( y = 0 ; y < 128 ; y++ )// Backlight auf 100% dimmen
		{
			for ( i = 0 ; i < 128 ; i++ )
			{
				if( i <= y)
					HD44780_TWI_Byte &= ~( 1 << HD44780_TWI_BL_PIN );
				else
					HD44780_TWI_Byte |= ( 1 << HD44780_TWI_BL_PIN );

				TWI_Write( HD44780_TWI_Byte );
			}
		}
		TWI_SendStop();
	}

	HD44780_TWI_Byte |= ( 1 << HD44780_TWI_BL_PIN );
	TWI_Write( HD44780_TWI_Byte );
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Sendet Kommandos ans Display.
 * \param   CMD			Kommando fürs Display im Datamodus
 * \return	NONE		None
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
void HD44780_TWI_sendCMD( char CMD )
{
	HD44780_TWI_Byte &= ~( 1 << HD44780_TWI_RS_PIN );
	HD44780_TWI_out( CMD );
	HD44780_TWI_out( CMD<<4 );

#if HD44780_DELAY_COMMAND > 0
	CLOCK_delay( HD44780_DELAY_COMMAND );
#endif

}

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Sendet Daten ans Display.
 * \param   DATA		Daten fürs Display im Datamodus
 * \return	NONE		None
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
void HD44780_TWI_sendDATA( char Data )
{
	HD44780_TWI_Byte |= ( 1 << HD44780_TWI_RS_PIN );

	HD44780_TWI_out( Data );
	HD44780_TWI_out( Data<<4 );

#if HD44780_DELAY_WRITEDATA > 0
	CLOCK_delay( HD44780_DELAY_WRITEDATA );
#endif

}

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Löscht das Display und setzt den Cursor auf die Homeposition.
 * \param   NONE		None
 * \return	NONE		None
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
void HD44780_TWI_clrscreen( void )
{
	HD44780_TWI_sendCMD( HD44780_CMD_CLEAR );
	HD44780_TWI_sendCMD( HD44780_CMD_CURSOR_HOME );
	HD44780_TWI_row = 0;
	HD44780_TWI_column = 0;
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Setzt den Cursor auf die angebene Position.
 * \param   XPos		Die X-Position des Cursors.
 * \param   YPos		Die Y-Position des Cursors.
 * \return	NONE		None
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
void HD44780_TWI_setXY( char XPos, char YPos )
{
	HD44780_TWI_row = YPos;
	HD44780_TWI_column = XPos;
	
	switch( YPos )
	{
		case	0:		HD44780_TWI_sendCMD( HD44780_CMD_DDRAM | HD44780_ROW_1 | XPos );
						break;
		case	1:		HD44780_TWI_sendCMD( HD44780_CMD_DDRAM | HD44780_ROW_2 | XPos );
						break;
#if HD44780_TWI_ROWS == 4
		case	2:		HD44780_TWI_sendCMD( HD44780_CMD_DDRAM | HD44780_ROW_3 | XPos );
						break;
		case	3:		HD44780_TWI_sendCMD( HD44780_CMD_DDRAM | HD44780_ROW_4 | XPos );
						break;
#endif
		default:		break;
	}
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Sendet ein Zeichen ans Display.
 * \param   DATA		Zeichen das ausgeben werden soll.
 * \return	NONE		None
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
void HD44780_TWI_SendChar( char Data )
{	 
	// Wenn es ein Steuerzeichen ist, ignorieren
	if ( Data < 10)
		return;

	// Wenn Return, dann zum Zeilenanfang springen
	if ( Data == '\r' )
	{
		HD44780_TWI_setXY( 0, HD44780_TWI_row );
		return;
	}
	
	// Wenn Newline, dann eine Zeil weiter runter springen
	if ( Data == '\n' )
	{
		HD44780_TWI_row++;
		if ( HD44780_TWI_row < HD44780_TWI_ROWS )
			HD44780_TWI_setXY( 0, HD44780_TWI_row );
		else
			HD44780_TWI_clrscreen();
		return;
	}

	// Sind wir schon am rechten Rand? Wenn nein, weiter, sonst Zeilensprung
	if ( HD44780_TWI_column < HD44780_TWI_COLUMNS )
	{
		HD44780_TWI_sendDATA( Data );
		HD44780_TWI_column++;
	}
	else
	{
		// eine Zeile weiter
		HD44780_TWI_row++;
		// Sind wir schon am unteren Rand? Wenn nein, eine Zeile weiter
		// sonst Display löschen und wieder oben anfangen
		if ( HD44780_TWI_row < HD44780_TWI_ROWS )
		{
			HD44780_TWI_column = 0;
			HD44780_TWI_setXY( HD44780_TWI_column, HD44780_TWI_row );
			HD44780_TWI_sendDATA( Data );
			HD44780_TWI_column++;
		}
		else
			HD44780_TWI_clrscreen();
	}
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Schaltet die Hintergrundbeleuchtung
 * \param   value		Helligkeit von 0-255
 * \return	NONE		None
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
void HD44780_TWI_Backlight( unsigned char value )
{
	if ( TWI_SendAddress( HD44780_TWI_ADDR , TWI_WRITE ) == TRUE )
	{
		if ( value == 0 )
		{
			HD44780_TWI_Byte |= ( 1 << HD44780_TWI_BL_PIN );        //  Backlight off
		}
		else
		{
			HD44780_TWI_Byte &= ~( 1 << HD44780_TWI_BL_PIN );        //  Backlight on
		}
		TWI_Write( HD44780_TWI_Byte );
		TWI_SendStop();        
	}
}

#endif

/**
 * @}
 */
