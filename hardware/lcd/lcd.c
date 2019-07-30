/***************************************************************************
 *            lcd.c
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
 * \ingroup Hardware
 * \addtogroup lcd Stellt Funktionen für ein/mehrere LCD bereit (lcd.c)
 *
 * @{
 */

/**
 * \file
 *
 * \author Dirk Broßwick
 */

#include <avr/pgmspace.h>
#include "config.h"

#if defined(LCD)

#include "lcd.h"

#if defined( HD44780 )
	#include "hd44780.h"
#endif
#if defined( HD44780_TWI )
	#include "hd44780_twi.h"
#endif
#if defined( KS0073_TWI )
	#include "ks0073_twi.h"
#endif
#if defined( DOGL128W6 )
	#include "dogl128w6.h"
#endif

	LCD_NUM lcd_num[ ] = {
#if defined( HD44780 )
		{ HD44780_init , HD44780_SendChar , HD44780_clrscreen , HD44780_setXY, HD44780_Backlight } ,
#endif
#if defined( HD44780_TWI )
		{ HD44780_TWI_init , HD44780_TWI_SendChar , HD44780_TWI_clrscreen , HD44780_TWI_setXY, HD44780_TWI_Backlight } ,
#endif
#if defined( KS0073_TWI )
		{ KS0073_TWI_init , KS0073_TWI_SendChar , KS0073_TWI_clrscreen , KS0073_TWI_setXY, KS0073_TWI_Backlight } ,
#endif
#if defined( DOGL128W6 )
		{ DOGL128_init , DOGL128_sendChar , DOGL128_clrscreen , DOGL128_setXY, DOGL128_Backlight } ,
#endif
	};

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Initialisiert das LCD.
 * \param   num		Nummer des Display, in der Regel 0 da meist nur ein Display angeschlossen.
 * \return	NONE		None
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
void LCD_init( int num )
{
	lcd_num[ num ].init( );
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Sendet ein Zeichen ans Display.
 * \param   Data		Zeichen das ausgeben werden soll.
 * \param   num			Nummer des Display, in der Regel 0 da meist nur ein Display angeschlossen.
 * \return	NONE		None
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
void LCD_sendchar( int num, char Data )
{
	lcd_num[ num ].sendchar( Data );
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Löscht das Display und setzt den Cursor auf die Homeposition.
 * \param   num			Nummer des Display, in der Regel 0 da meist nur ein Display angeschlossen.
 * \return	NONE		None
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
void LCD_clrscreen( int num )
{
	lcd_num[ num ].clrscreen( );
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Setzt den Cursor auf die angebene Position.
 * \param   num			Nummer des Display, in der Regel 0 da meist nur ein Display angeschlossen.
 * \param   XPos		Die X-Position des Cursors.
 * \param   YPos		Die Y-Position des Cursors.
 * \return	NONE		None
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
void LCD_setXY( int num, char XPos, char YPos )
{
	lcd_num[ num ].setXY( XPos , YPos );
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Schaltet die Hintergrundbeleuchtung
 * \param   value		Helligkeit von 0-255
 * \return	NONE		None
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
void LCD_backlight( int num, unsigned char value )
{
	lcd_num[ num ].backlight( value );
}

#endif

/**
 * @}
 */

