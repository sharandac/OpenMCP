/***************************************************************************
 *            ks0073_twi.c
 *
 *  Thu Jul 28 21:04:48 2011
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
 
/**
 * \ingroup lcd
 * \addtogroup ks0073_twi Support fuer ks0073 Display ueber TWI (ks0073_twi.c)
 *
 * Mit diesen Treiber koennen KS0073-Displays ueber TWI angesprochen werden.
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

#if defined(LCD) && defined(KS0073_TWI)

#include "hd44780_gen.h"
#include "hd44780_twi.h"
#include "ks0073_twi.h"
#include "hardware/twi/twi.h"
#include "system/clock/clock.h"

static char KS0073_TWI_Byte;
static char	KS0073_TWI_row;
static char	KS0073_TWI_column;

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Erzeugt ein Puls auf der ENABLE Leitung.
 * \param   NONE		None
 * \return	NONE		None
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
void KS0073_TWI_enable( void )
{
	if ( TWI_SendAddress( KS0073_TWI_ADDR , TWI_WRITE ) == TRUE )
	{
		KS0073_TWI_Byte &= ~( 1 << KS0073_TWI_E_PIN );
		TWI_Write( KS0073_TWI_Byte );
		_delay_us( 100 );
		KS0073_TWI_Byte |= ( 1 << KS0073_TWI_E_PIN );
		TWI_Write( KS0073_TWI_Byte );
		_delay_us( 90 );
		KS0073_TWI_Byte &= ~( 1 << KS0073_TWI_E_PIN );
		TWI_Write( KS0073_TWI_Byte );
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
void KS0073_TWI_out( char data )
{
	KS0073_TWI_Byte &= ~KS0073_TWI_DATAMASK;
	KS0073_TWI_Byte |= ( data>>4 & KS0073_TWI_DATAMASK );

	// trigger enable line
    KS0073_TWI_enable();
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Initialisiert das LCD.
 * \param   NONE		None
 * \return	NONE		None
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
void KS0073_TWI_init( void )
{
	unsigned char i;
	int y;
	
	KS0073_TWI_Byte = 0;
	// erster Reset in den 8-Bit Modus
	KS0073_TWI_out(HD44780_CMD_SOFT_RESET);
	CLOCK_delay( HD44780_DELAY_SOFT_RESET1 );

	// zweiter Reset in den 8-Bit Modus
	KS0073_TWI_enable();
	CLOCK_delay( HD44780_DELAY_SOFT_RESET2 );

	// dritter Reset in den 8-Bit Modus
	KS0073_TWI_enable();
	CLOCK_delay( HD44780_DELAY_SOFT_RESET3 );

	// und jetzt endlich in den 4-Bit Modus und 'RE-Bit ein b00101100  
	KS0073_TWI_out(HD44780_CMD_SET_FUNCTION |
	            	HD44780_SF_4BIT);							// RE:1
	CLOCK_delay( HD44780_DELAY_SET_4BITMODE );
	KS0073_TWI_sendCMD(HD44780_CMD_SET_FUNCTION |
						HD44780_SF_4BIT |
						HD44780_SF_2LINES |
						KS0073_RE_ON);
						
	// Display für KS0073 einstellen b00001001
	KS0073_TWI_sendCMD(KS0073_CMD_EXT_FUNCTION |
						HD44780_SF_5x7 |					// 5-dot-Font
						KS0073_4LINES);						// 4-line display

	// RE:Bit wieder auf 0 setzen b00101000
	KS0073_TWI_sendCMD(HD44780_CMD_SET_FUNCTION |
						HD44780_SF_4BIT |
						HD44780_SF_2LINES|
						KS0073_RE_OFF);

	// clear screen
	KS0073_TWI_sendCMD( HD44780_CMD_CLEAR );
	
	//Display ausschalten
	KS0073_TWI_sendCMD(HD44780_CMD_SET_DISPLAY |
						HD44780_SD_DISPLAY_OFF);
						
	// Entry Mode
	KS0073_TWI_sendCMD(HD44780_CMD_SET_ENTRY |
						HD44780_SE_INCREASE);
						
	// Display clear cursor home
	KS0073_TWI_sendCMD( HD44780_CMD_CLEAR );
	
	// Cursor auf Homeposition setzen
	KS0073_TWI_sendCMD( HD44780_CMD_CURSOR_HOME );
	
	
	// Display einschalten
	KS0073_TWI_sendCMD( HD44780_CMD_SET_DISPLAY |    
			 	             HD44780_SD_DISPLAY_ON /* |		// display on
							 HD44780_SD_CURSOR_ON |			// show cursor
							 HD44780_SD_BLINKING_ON */ );	// cursor blinking

	KS0073_TWI_row = 0;
	KS0073_TWI_column = 0;

	// fade in LCD-Backlight
	if ( TWI_SendAddress( KS0073_TWI_ADDR , TWI_WRITE ) == TRUE )
	{
		for( y = 0 ; y < 128 ; y++ )// Backlight auf 100% dimmen
		{
			for ( i = 0 ; i < 128 ; i++ )
			{
				if( i <= y)
					KS0073_TWI_Byte &= ~( 1 << KS0073_TWI_BL_PIN );
				else
					KS0073_TWI_Byte |= ( 1 << KS0073_TWI_BL_PIN );

				TWI_Write( KS0073_TWI_Byte );
			}
		}
		TWI_SendStop();
	}
	KS0073_TWI_Byte &= ~( 1 << KS0073_TWI_BL_PIN );
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Sendet Kommandos ans Display.
 * \param   CMD			Kommando fürs Display im Datamodus
 * \return	NONE		None
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
void KS0073_TWI_sendCMD( char CMD )
{
	KS0073_TWI_Byte &= ~( 1 << KS0073_TWI_RS_PIN );
	KS0073_TWI_out( CMD );
	KS0073_TWI_out( CMD<<4 );

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
void KS0073_TWI_sendDATA( char Data )
{
	KS0073_TWI_Byte |= ( 1 << KS0073_TWI_RS_PIN );

	KS0073_TWI_out( Data );
	KS0073_TWI_out( Data<<4 );

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
void KS0073_TWI_clrscreen( void )
{
	KS0073_TWI_sendCMD( HD44780_CMD_CLEAR );
	KS0073_TWI_sendCMD( HD44780_CMD_CURSOR_HOME );
	KS0073_TWI_row = 0;
	KS0073_TWI_column = 0;
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Setzt den Cursor auf die angebene Position.
 * \param   XPos		Die X-Position des Cursors.
 * \param   YPos		Die Y-Position des Cursors.
 * \return	NONE		None
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
void KS0073_TWI_setXY( char XPos, char YPos )
{
	KS0073_TWI_row = YPos;
	KS0073_TWI_column = XPos;
	
	switch( YPos )
	{
		case	0:		KS0073_TWI_sendCMD( HD44780_CMD_DDRAM | KS0073_ROW_1 | XPos );
						break;
		case	1:		KS0073_TWI_sendCMD( HD44780_CMD_DDRAM | KS0073_ROW_2 | XPos );
						break;
#if KS0073_TWI_ROWS == 4
		case	2:		KS0073_TWI_sendCMD( HD44780_CMD_DDRAM | KS0073_ROW_3 | XPos );
						break;
		case	3:		KS0073_TWI_sendCMD( HD44780_CMD_DDRAM | KS0073_ROW_4 | XPos );
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
void KS0073_TWI_SendChar( char Data )
{	 
	// Wenn es ein Steuerzeichen ist, ignorieren
	if ( Data < 10)
		return;

	// Wenn Return, dann zum Zeilenanfang springen
	if ( Data == '\r' )
	{
		KS0073_TWI_setXY( 0, KS0073_TWI_row );
		return;
	}
	
	// Wenn Newline, dann eine Zeil weiter runter springen
	if ( Data == '\n' )
	{
		KS0073_TWI_row++;
		if ( KS0073_TWI_row < KS0073_TWI_ROWS )
			KS0073_TWI_setXY( 0, KS0073_TWI_row );
		else
			KS0073_TWI_clrscreen();
		return;
	}

	// Sind wir schon am rechten Rand? Wenn nein, weiter, sonst Zeilensprung
	if ( KS0073_TWI_column < KS0073_TWI_COLUMNS )
	{
		KS0073_TWI_sendDATA( Data );
		KS0073_TWI_column++;
	}
	else
	{
		// eine Zeile weiter
		KS0073_TWI_row++;
		// Sind wir schon am unteren Rand? Wenn nein, eine Zeile weiter
		// sonst Display löschen und wieder oben anfangen
		if ( KS0073_TWI_row < KS0073_TWI_ROWS )
		{
			KS0073_TWI_column = 0;
			KS0073_TWI_setXY( KS0073_TWI_column, KS0073_TWI_row );
			KS0073_TWI_sendDATA( Data );
			KS0073_TWI_column++;
		}
		else
			KS0073_TWI_clrscreen();
	}
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Schaltet die Hintergrundbeleuchtung
 * \param   value		Helligkeit von 0-255
 * \return	NONE		None
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
void KS0073_TWI_Backlight( unsigned char value )
{
	if ( TWI_SendAddress( KS0073_TWI_ADDR , TWI_WRITE ) == TRUE )
	{
		if ( LCD_CMD != LCD_BL_OFF )
		{
			KS0073_TWI_Byte |= ( 1 << KS0073_TWI_BL_PIN );        //  Backlight off
		}
		else
		{
			KS0073_TWI_Byte &= ~( 1 << KS0073_TWI_BL_PIN );        //  Backlight off
		}
		TWI_Write( KS0073_TWI_Byte );
		TWI_SendStop();        
	}
}

#endif

/**
 * @}
 */ 