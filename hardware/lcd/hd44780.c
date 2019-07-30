/***************************************************************************
 *            hd44780.c
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
 * \addtogroup hd44780 Support fuer HD44780 Display 4/8-Bit (hd44780.c)
 *
 * Mit diesen Treiber koennen HD44780-Displays angesprochen werden.
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

#if defined(LCD) && defined(HD44780)

#include "hd44780.h"
#include "hd44780_gen.h"
#include "hardware/timer1/timer1.h"
#include "system/clock/clock.h"

static char	HD44780_row;
static char	HD44780_column;

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Erzeugt ein Puls auf der ENABLE Leitung.
 * \param   NONE		None
 * \return	NONE		None
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
static void HD44780_enable( void )
{
#if defined(__AVR_XMEGA__)
	HD44780_E_PORT.OUTSET = (1<<HD44780_E_PIN);			// Enable auf 1 setzen
	CLOCK_delay( HD44780_DELAY_ENABLE );			// kurze Pause
	HD44780_E_PORT.OUTCLR = (1<<HD44780_E_PIN);			// Enable auf 0 setzen
#else
	HD44780_E_PORT |= (1<<HD44780_E_PIN);				// Enable auf 1 setzen
	CLOCK_delay( HD44780_DELAY_ENABLE );			// kurze Pause
	HD44780_E_PORT &= ~(1<<HD44780_E_PIN);				// Enable auf 0 setzen
#endif
}
 
/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Sendet eine 4 Bit Ausgabe an das LCD.
 * \param   data		Ausgabedaten für das LCD
 * \return	NONE		None
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
static void HD44780_out( char data )
{
	// obere 4 Bit maskieren
	unsigned char lData = data & 0xF0;

#if defined(HD44780_DATAPORT_1) && defined(HD44780_DATAPORT_2)
	  // if splitted data port

	#if defined(__AVR_XMEGA__)
		// clear bits
		HD44780_DATAPORT_1.OUTCLR = HD44780_DATAMASK_1;
		HD44780_DATAPORT_2.OUTCLR = HD44780_DATAMASK_2;
		// set bits
		HD44780_DATAPORT_1.OUTSET = ( HD44780_DATAMASK_1 & ( lData<<2 ) );
		HD44780_DATAPORT_2.OUTSET = ( HD44780_DATAMASK_2 & ( lData>>6 ) );
	#else
		// clear bits
		HD44780_DATAPORT_1 &= ~( HD44780_DATAMASK_1 );
		HD44780_DATAPORT_2 &= ~( HD44780_DATAMASK_2 );
		// set bits
		HD44780_DATAPORT_1 |= ( HD44780_DATAMASK_1 & ( lData<<2 ) );
		HD44780_DATAPORT_2 |= ( HD44780_DATAMASK_2 & ( lData>>6 ) );
	#endif

#elif defined(HD44780_DATAPORT)
	  // if data lines are ascending on one port

	#if defined(__AVR_XMEGA__)
		// clear bits
		HD44780_DATAPORT.OUTCLR = HD44780_DATAMASK;
		// set bits
		HD44780_DATAPORT.OUTSET = ( lData>>4 );
	#else
		// clear bits
		HD44780_DATAPORT &= ~( HD44780_DATAMASK );
		// set bits
		HD44780_DATAPORT |= ( lData>>4 );
	#endif
#endif

	// trigger enable line
    HD44780_enable();
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Initialisiert das LCD.
 * \param   NONE		None
 * \return	NONE		None
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
void HD44780_init( void )
{
	unsigned char i;
	
#if defined(myAVR)
	// LCD-Spannung erzeugen am Ausgang OC2a
    PORTB &= ~(1<<PD3);  // clear port
    DDRB |= (1<<PD3);  // set as output

    TCCR0A = ( 0<<WGM21 ) | ( 1<<WGM20 ) | ( 1<<COM2A1 );
    TCCR0B = ( 0<<WGM22 ) | ( 1<<CS20 );

    TCNT0=0;

    OCR0A=0;
#endif

#if defined(HD44780_DATAPORT_1) && defined(HD44780_DATAPORT_2)

	// die 4 Ausgänge für Daten ans das Display auf 0 setzen
	#if defined(__AVR_XMEGA__)
		HD44780_DATAPORT_1.OUTCLR = HD44780_DATAMASK_1;
		HD44780_DATAPORT_2.OUTCLR = HD44780_DATAMASK_2;
		HD44780_DATAPORT_1.DIRSET = HD44780_DATAMASK_1;
		HD44780_DATAPORT_2.DIRSET = HD44780_DATAMASK_2;
	#else
		HD44780_DATAPORT_1 &= ~( HD44780_DATAMASK_1 );
		HD44780_DATADDR_1  |=  ( HD44780_DATAMASK_1 );
		HD44780_DATAPORT_2 &= ~( HD44780_DATAMASK_2 );
		HD44780_DATADDR_2  |=  ( HD44780_DATAMASK_2 );
	#endif

#elif defined(HD44780_DATAPORT)

	#if defined(__AVR_XMEGA__)
		HD44780_DATAPORT.OUTCLR = HD44780_DATAMASK;
		HD44780_DATAPORT.DIRSET = HD44780_DATAMASK;
	#else
		HD44780_DATAPORT &= ~( HD44780_DATAMASK );
		HD44780_DATADDR  |=    HD44780_DATAMASK;
	#endif

#else

	#error "LCD Konfigurationsfehler!"

#endif

#if defined(__AVR_XMEGA__)
	// Enable auf 0 setzen
	HD44780_E_PORT.OUTCLR = ( 1<<HD44780_E_PIN );
	HD44780_E_PORT.DIRSET = ( 1<<HD44780_E_PIN );
	
	// Register Select auf 0 (command) setzen
	HD44780_RS_PORT.OUTCLR = ( 1<<HD44780_RS_PIN );
	HD44780_RS_PORT.DIRSET = ( 1<<HD44780_RS_PIN );	

	// Read/Write auf 0 (write) setzen	
	HD44780_RW_PORT.OUTCLR = ( 1<<HD44780_RW_PIN );
	HD44780_RW_PORT.DIRSET = ( 1<<HD44780_RW_PIN );
#else
	// Enable auf 0 setzen
	HD44780_E_PORT &= ~( 1<<HD44780_E_PIN );
	HD44780_E_DDR |= ( 1<<HD44780_E_PIN );
	
	// Register Select auf 0 (command) setzen
	HD44780_RS_PORT &= ~( 1<<HD44780_RS_PIN );
	HD44780_RS_DDR |= ( 1<<HD44780_RS_PIN );

	// Read/Write auf 0 (write) setzen
	HD44780_RW_PORT &= ~( 1<<HD44780_RW_PIN );
	HD44780_RW_DDR |= ( 1<<HD44780_RW_PIN );
#endif
	
	// erster Reset in den 8-Bit Modus
	HD44780_out(HD44780_CMD_SOFT_RESET);
	CLOCK_delay( HD44780_DELAY_SOFT_RESET1 );

	// zweiter Reset in den 8-Bit Modus
	HD44780_enable();
	CLOCK_delay( HD44780_DELAY_SOFT_RESET2 );

	// dritter Reset in den 8-Bit Modus
	HD44780_enable();
	CLOCK_delay( HD44780_DELAY_SOFT_RESET3 );

	// und jetzt endlich in den 4-Bit Modus
	HD44780_out( HD44780_CMD_SET_FUNCTION | HD44780_SF_4BIT );
	CLOCK_delay( HD44780_DELAY_SET_4BITMODE );
	
	// Display einstellen, 4Bit, 2 Lines und Zeichensatz 5x8 Pixel
	HD44780_sendCMD( HD44780_CMD_SET_FUNCTION |
			 	 HD44780_SF_4BIT | 			// 4 Bit mode
				 HD44780_SF_2LINES | 		// 2 lines
				 HD44780_SF_5x10 ); 		// 5x10
	// clear screen
	HD44780_sendCMD( HD44780_CMD_CLEAR );
	// Cursor auf Homeposition setzen
	HD44780_sendCMD( HD44780_CMD_CURSOR_HOME );
	// Display einschalten
	HD44780_sendCMD( HD44780_CMD_SET_DISPLAY |    
	             HD44780_SD_DISPLAY_ON /* |		// display on
				 HD44780_SD_CURSOR_ON |			// show cursor
				 HD44780_SD_BLINKING_ON */ );	// cursor blinking

	HD44780_row = 0;
	HD44780_column = 0;
	
#if defined(myAVR)

	// Backlight auf 100% dimmen
	for ( i = 0 ; i < 255 ; i++ )
	{
        OCR0A=i;
		CLOCK_delay( 5 );
	}	
	// Backlight auf 50% dimmen
	for ( ; i > 128 ; i-- )
	{
        OCR0A=i;
		CLOCK_delay( 5 );
	}	
#endif
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Sendet Kommandos ans Display.
 * \param   CMD			Kommando fürs Display im Datamodus
 * \return	NONE		None
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
void HD44780_sendCMD( char CMD )
{
#if defined(__AVR_XMEGA__)
	HD44780_RS_PORT.OUTCLR = ( 1<<HD44780_RS_PIN );
#else
	HD44780_RS_PORT &= ~( 1<<HD44780_RS_PIN );
#endif

	HD44780_out( CMD );
	HD44780_out( CMD<<4 );

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
void HD44780_sendDATA( char Data )
{
#if defined(__AVR_XMEGA__)
	HD44780_RS_PORT.OUTSET = ( 1<<HD44780_RS_PIN );
#else
	HD44780_RS_PORT |= ( 1<<HD44780_RS_PIN );
#endif

	HD44780_out( Data );
	HD44780_out( Data<<4 );

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
void HD44780_clrscreen( void )
{
	HD44780_sendCMD( HD44780_CMD_CLEAR );
	HD44780_sendCMD( HD44780_CMD_CURSOR_HOME );
	HD44780_row = 0;
	HD44780_column = 0;
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Setzt den Cursor auf die angebene Position.
 * \param   XPos		Die X-Position des Cursors.
 * \param   YPos		Die Y-Position des Cursors.
 * \return	NONE		None
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
void HD44780_setXY( char XPos, char YPos )
{
	HD44780_row = YPos;
	HD44780_column = XPos;
	
	switch( YPos )
	{
		case	0:		HD44780_sendCMD( HD44780_CMD_DDRAM | HD44780_ROW_1 | XPos );
						break;
		case	1:		HD44780_sendCMD( HD44780_CMD_DDRAM | HD44780_ROW_2 | XPos );
						break;
#if HD44780_Rows == 4
		case	2:		HD44780_sendCMD( HD44780_CMD_DDRAM | HD44780_ROW_3 | XPos );
						break;
		case	3:		HD44780_sendCMD( HD44780_CMD_DDRAM | HD44780_ROW_4 | XPos );
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
void HD44780_SendChar( char Data )
{	 
	// Wenn es ein Steuerzeichen ist, ignorieren
	if ( Data < 10)
		return;

	// Wenn Return, dann zum Zeilenanfang springen
	if ( Data == '\r' )
	{
		HD44780_setXY( 0, HD44780_row );
		return;
	}
	
	// Wenn Newline, dann eine Zeil weiter runter springen
	if ( Data == '\n' )
	{
		HD44780_row++;
		if ( HD44780_row < HD44780_Rows )
			HD44780_setXY( 0, HD44780_row );
		else
			HD44780_clrscreen();
		return;
	}

	// Sind wir schon am rechten Rand? Wenn nein, weiter, sonst Zeilensprung
	if ( HD44780_column < HD44780_Columns )
	{
		HD44780_sendDATA( Data );
		HD44780_column++;
	}
	else
	{
		// eine Zeile weiter
		HD44780_row++;
		// Sind wir schon am unteren Rand? Wenn nein, eine Zeile weiter
		// sonst Display löschen und wieder oben anfangen
		if ( HD44780_row < HD44780_Rows )
		{
			HD44780_column = 0;
			HD44780_setXY( HD44780_column, HD44780_row );
			HD44780_sendDATA( Data );
			HD44780_column++;
		}
		else
			HD44780_clrscreen();
	}
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Schaltet die Hintergrundbeleuchtung
 * \param   value		Helligkeit von 0-255
 * \return	NONE		None
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
void HD44780_Backlight( unsigned char value )
{
    OCR0A = value;
}

#endif

/**
 * @}
 */

