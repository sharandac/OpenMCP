/***************************************************************************
 *            dogl128w6.c
 *
 *  Sun Jun  18 17:32:17 2011
 *  Copyright  2011 Dirk Broßwick
 *  <sharan@snafu.de>
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
 * \addtogroup DOGL128W6 Support fuer DOG-Displays. (dogl128w6.c)
 *
 * Mit diesen Treiber koennen die DOG-Displays angesprochen werden.
 *
 * @{
 */

/**
 * \file
 *
 * \author Dirk Broßwick
 */

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <string.h>
#include "config.h"

#if defined(LCD) && defined(DOGL128W6)

#include "hardware/lcd/dogl128w6.h"
#include "system/clock/delay_x.h"
#include "hardware/led-tafel/font.h"

static char FrameBuffer[ ( 128 * 64 ) / 8 ];

static char	DOGL128_row = 0;
static char	DOGL128_column = 0;

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Sendet ein Zeichen ans Display.
 * \param   DATA		Zeichen das ausgeben werden soll.
 * \return	NONE		None
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
void DOGL128_init( void )
{	
	DOGL128_PORT.DIRSET = ( 1<<DOGL128_A0_PIN ) | ( 1<<DOGL128_SI_PIN ) | ( 1<<DOGL128_SCL_PIN ) | ( 1<<DOGL128_RESET_PIN ) | ( 1<<DOGL128_CS_PIN ) | ( 1<<DOGL128_PWM_PIN );
	DOGL128_PORT.OUTCLR = ( 1<<DOGL128_A0_PIN ) | ( 1<<DOGL128_SI_PIN ) | ( 1<<DOGL128_SCL_PIN ) | ( 1<<DOGL128_RESET_PIN ) | ( 1<<DOGL128_CS_PIN ) | ( 1<<DOGL128_PWM_PIN );
	_delay_ms( 10 );
	DOGL128_PORT.OUTSET = ( 1<<DOGL128_CS_PIN ) | ( 1<<DOGL128_RESET_PIN );	
	_delay_ms( 10 );

	DOGL128_send( 0x40 );
	DOGL128_send( 0xa1 );
	DOGL128_send( 0xc0 );
	DOGL128_send( 0xa6 );
	DOGL128_send( 0xa2 );
	DOGL128_send( 0x2f );
	DOGL128_send( 0xf8 );
	DOGL128_send( 0x00 );
	DOGL128_send( 0x27 );
	DOGL128_send( 0x81 );
	DOGL128_send( 0x10 ); // kontrast
	DOGL128_send( 0xac );
	DOGL128_send( 0x00 );
	DOGL128_send( 0xaf );		
	DOGL128_send( 0xb0 | 0 );
	DOGL128_send( 0x10 | 0 );
	DOGL128_send( 0x00 | 0 );

	DOGL128_clrscreen();
}


/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Sendet ein Zeichen ans Display.
 * \param   DATA		Zeichen das ausgeben werden soll.
 * \return	NONE		None
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
void DOGL128_send( char Data )
{
	unsigned char i;
		
	DOGL128_PORT.OUTCLR = ( 1<<DOGL128_CS_PIN );

	for( i = 0 ; i < 8 ; i++ )
	{
		// SCK auf High setzen
		DOGL128_PORT.OUTCLR = ( 1<<DOGL128_SCL_PIN);

		// Kieck mal ob MSB 1 ist
		if ( ( Data & 0x80 ) > 0 )
			DOGL128_PORT.OUTSET = ( 1<<DOGL128_SI_PIN);
		else
			DOGL128_PORT.OUTCLR = ( 1<<DOGL128_SI_PIN);
										
		// SCK auf High setzen
		DOGL128_PORT.OUTSET = ( 1<<DOGL128_SCL_PIN);

		Data <<= 1;
	}

	DOGL128_PORT.OUTSET = ( 1<<DOGL128_CS_PIN );
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Sendet ein Zeichen ans Display.
 * \param   DATA		Zeichen das ausgeben werden soll.
 * \return	NONE		None
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
void DOGL128_setXY( char XPos, char YPos )
{
	DOGL128_row = YPos;
	DOGL128_column = XPos;

	XPos = XPos * 8;

	DOGL128_send( 0x10 | ( XPos >> 4 ) );
	DOGL128_send( 0x00 | ( XPos & 0x0f ) );
	DOGL128_send( 0xb0 | YPos );
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Sendet ein Zeichen ans Display.
 * \param   DATA		Zeichen das ausgeben werden soll.
 * \return	NONE		None
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
void DOGL128_writechar( char Data )
{
	char i,c,b;

	DOGL128_PORT.OUTSET = ( 1<<DOGL128_A0_PIN);

	for( i = 0 ; i < 8 ; i++ )
	{
		b = pgm_read_byte( &font[ (int)Data ][ (int)( 7 - i ) ] );
		
		c = ( ( b >> 7 ) & 0x01 ) | ( b << 7 & 0x80 );
		c |= ( ( b >> 5 ) & 0x02 ) | ( b << 5 & 0x40 );
		c |= ( ( b >> 3 ) & 0x04 ) | ( b << 3 & 0x20 );
		c |= ( ( b >> 1 ) & 0x08 ) | ( b << 1 & 0x10 );

		DOGL128_send( c );
	}

	DOGL128_PORT.OUTCLR = ( 1<<DOGL128_A0_PIN);
}	

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Sendet ein Zeichen ans Display.
 * \param   DATA		Zeichen das ausgeben werden soll.
 * \return	NONE		None
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
void DOGL128_clrscreen( void )
{
	char row,colum;

	for ( row = 0 ; row < 8 ; row ++ )
	{
		for( colum = 0 ; colum < 16 ; colum++ )
		{	
			DOGL128_setXY( colum, row );
			DOGL128_writechar( ' ' );
		}
	}
}
/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Sendet ein Zeichen ans Display.
 * \param   DATA		Zeichen das ausgeben werden soll.
 * \return	NONE		None
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
void DOGL128_sendChar( char Data )
{	 
	// Wenn es ein Steuerzeichen ist, ignorieren
	if ( Data < 10)
		return;

	// Wenn Return, dann zum Zeilenanfang springen
	if ( Data == '\r' )
	{
		DOGL128_setXY( 0, DOGL128_row );
		return;
	}
	
	// Wenn Newline, dann eine Zeil weiter runter springen
	if ( Data == '\n' )
	{
		DOGL128_row++;
		if ( DOGL128_row < DOGL128_Rows )
			DOGL128_setXY( 0, DOGL128_row );
		else
			DOGL128_clrscreen();
		return;
	}

	// Sind wir schon am rechten Rand? Wenn nein, weiter, sonst Zeilensprung
	if ( DOGL128_column < DOGL128_Columns )
	{
		DOGL128_writechar( Data );
		DOGL128_column++;
	}
	else
	{
		// eine Zeile weiter
		DOGL128_row++;
		// Sind wir schon am unteren Rand? Wenn nein, eine Zeile weiter
		// sonst Display löschen und wieder oben anfangen
		if ( DOGL128_row < DOGL128_Rows )
		{
			DOGL128_column = 0;
			DOGL128_writechar( Data );
			DOGL128_column++;
		}
		else
			DOGL128_clrscreen();
	}
}

void DOGL128_Backlight( unsigned char value )
{
	 if ( value != 0 )
	{
		DOGL128_PORT.OUTSET = ( 1<<DOGL128_PWM_PIN );
	}
	else
	{
		DOGL128_PORT.OUTCLR = ( 1<<DOGL128_PWM_PIN );
	}
}

void DOGL128_Contrast( unsigned char value )
{
	DOGL128_send( 0x81 );
	DOGL128_send( value & 0x1f ); // kontrast
}

void DOGL128_FBsetPixel( unsigned char x, unsigned char y, unsigned char color )
{
	int offset;
	char data;

	if ( x >= 128 ) return;
	if ( y >= 64 ) return;
	
	offset = ( y / 8 ) * 128 + x;

	data = FrameBuffer[ offset ];

	if( color != 0 )
		data |= ( 1<< ( y%8 ) );
	else
		data &= ~( 1<< ( y%8 ) );

	FrameBuffer[ offset ] = data;
}

void DOGL128_FBclear( void )
{
	memset( FrameBuffer , 0 , sizeof( FrameBuffer ) );
}

void DOGL128_FBupdate( void )
{
	char i;
	int a;
	char *data;

	data = FrameBuffer;
	
	for( i = 0; i < 8 ; i++ )
	{
		DOGL128_send( 0x10 );
		DOGL128_send( 0x00 );
		DOGL128_send( 0xb0 | i );
		DOGL128_PORT.OUTSET = ( 1<<DOGL128_A0_PIN);

		for( a = 0 ; a < 128 ; a++ )
			DOGL128_send( *data++ );
			
		DOGL128_PORT.OUTCLR = ( 1<<DOGL128_A0_PIN);
	}
}	
#endif

/**
 * @}
 */
