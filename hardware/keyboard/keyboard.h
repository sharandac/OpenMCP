/***************************************************************************
 *            key.h
 *
 *  Sun Sep 11 17:18:16 2011
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

#ifndef KEY_BOARD_H
	#define KEY_BOARD_H

	#define	KEYBOARD_Bufferlen	8
	#define KEYBOARD_DELAY		50
	#define KEYBOARD_REPEAT		5

	void KEYBOARD_init( void );
	void KEYBOARD_Interrupt( void );
	int KEYBOARD_GetKey( void );
	void KEYBOARD_Flush( void );

#endif // KEY_BOARD_H