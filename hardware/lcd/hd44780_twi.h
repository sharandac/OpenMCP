/***************************************************************************
 *            hd44780_twi.h
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
 * \addtogroup hd44780_twi Support fuer HD44780 Display ueber TWI (hd44780_twi.h)
 *
 * Mit diesen Treiber koennen HD44780-Displays ueber TWI angesprochen werden.
 * @{
 */

/**
 * \file
 *
 * \author Dirk Broßwick
 */

#ifndef _HD44780_TWI_H
	#define _HD44780_TWI_H

	#include "config.h"

	// LCD size now in config.h!

	void HD44780_TWI_init( void );
	void HD44780_TWI_enable( void );
	void HD44780_TWI_out( char data );
	void HD44780_TWI_sendCMD( char CMD );
	void HD44780_TWI_sendDATA( char Data );
	void HD44780_TWI_clrscreen( void );
	void HD44780_TWI_setXY( char Xpos, char Ypos );
	void HD44780_TWI_SendChar( char Data );
	void HD44780_TWI_Backlight( unsigned char value );

	#define		HD44780_TWI_ADDR				0x20
	#define		HD44780_TWI_DATAMASK			0x0F
	#define		HD44780_TWI_RS_PIN				4
	#define		HD44780_TWI_RW_PIN				5
	#define		HD44780_TWI_E_PIN				6
	#define		HD44780_TWI_BL_PIN				7

	#define		HD44780_DELAY_WRITEDATA		1
	#define		HD44780_DELAY_COMMAND		1
	#define		HD44780_DELAY_SOFT_RESET1	5
	#define		HD44780_DELAY_SOFT_RESET2	1
	#define		HD44780_DELAY_SOFT_RESET3	1
	#define		HD44780_DELAY_SET_4BITMODE	5
	#define		HD44780_DELAY_ENABLE		1

#endif /* HD44780_TWI_H */

/**
 * @}
 */
