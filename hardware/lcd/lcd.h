/***************************************************************************
 *            lcd.h
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
 * \ingroup hardware
 * \addtogroup lcd Stellt Funktionen für ein/mehrere LCD bereit (lcd.h)
 *
 * @{
 */

/**
 * \file
 *
 * \author Dirk Broßwick
 */

 #ifndef _LCD_H
	#define _LCD_H

	#include "config.h"

	void LCD_init( int num );
	void LCD_sendchar( int num, char Data );
	void LCD_setXY( int num, char XPos, char YPos );
	void LCD_clrscreen( int num );
	void LCD_backlight( int num, unsigned char value );

	typedef void pLCD_init ( void );
	typedef void pLCD_sendchar ( char Data );
	typedef void pLCD_setXY ( char XPos, char YPos );
	typedef void pLCD_clrscreen ( void );
	typedef void pLCD_backlight ( unsigned char value );

	typedef struct {
		pLCD_init					* init;
		pLCD_sendchar				* sendchar;
		pLCD_clrscreen				* clrscreen;
		pLCD_setXY					* setXY;
		pLCD_backlight				* backlight;
	} const LCD_NUM;

#endif /* LCD_H */

/**
 * @}
 */

