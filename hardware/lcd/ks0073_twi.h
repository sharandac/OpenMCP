/***************************************************************************
 *            ks0073_twi.h
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

#ifndef KS0073_H_

	#define KS0073_H_

	#include "config.h"

	// LCD size now in config.h!

	void KS0073_TWI_init( void );
	void KS0073_TWI_enable( void );
	void KS0073_TWI_out( char data );
	void KS0073_TWI_sendCMD( char CMD );
	void KS0073_TWI_sendDATA( char Data );
	void KS0073_TWI_clrscreen( void );
	void KS0073_TWI_setXY( char Xpos, char Ypos );
	void KS0073_TWI_SendChar( char Data );
	void KS0073_TWI_Backlight( unsigned char value );

	#define		KS0073_ROW_1				0x00
	#define		KS0073_ROW_2				0x20
	#define		KS0073_ROW_3				0x40
	#define		KS0073_ROW_4				0x60

	#define		KS0073_TWI_ADDR				0x20
	#define		KS0073_TWI_DATAMASK			0x0F
	#define		KS0073_TWI_RS_PIN				4
	#define		KS0073_TWI_RW_PIN				5
	#define		KS0073_TWI_E_PIN				6
	#define		KS0073_TWI_BL_PIN				7

	#define		KS0073_CMD_EXT_FUNCTION		0x08	// extended function set with RE=1
	#define		KS0073_RE_ON				0x04	// extension register RE:1
	#define		KS0073_RE_OFF				0x00	// extension register RE:0
	#define		KS0073_4LINES				0x09
	#define		KS0073_DISPLAY_ON			0x0C	// Display:on  Cursor:off
	#define		KS0073_ENTRY_MODE			0x06


#endif // KS0073_H_