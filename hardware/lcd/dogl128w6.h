/***************************************************************************
 *            dogl128w6.h
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
 * \addtogroup DOGL128W6 Support for DOG-Displays. (dogl128w6.h)
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

#ifndef _DOGL128W6_H
	#define _DOGL128W6_H


	#define DOGL128_PORT		PORTA
	#define	DOGL128_PWM_PIN		1
	#define	DOGL128_A0_PIN		2
	#define	DOGL128_RESET_PIN	3
	#define	DOGL128_CS_PIN		4
	#define	DOGL128_SI_PIN		5
	#define	DOGL128_SCL_PIN		7

	#define	DOGL128_Rows		8
	#define	DOGL128_Columns		16

	void DOGL128_init( void );
	void DOGL128_send( char Data );
	void DOGL128_writechar( char Data );
	void DOGL128_clrscreen( void );
	void DOGL128_sendChar( char Data );
	void DOGL128_setXY( char XPos, char YPos );
	void DOGL128_Backlight( unsigned char value );
	void DOGL128_Contrast( unsigned char value );

	void DOGL128_FBsetPixel( unsigned char x, unsigned char y, unsigned char color );
	void DOGL128_FBclear( void );
	void DOGL128_FBupdate( void );

#endif /* _DOGL128W6_H */

/**
 * @}
 */
