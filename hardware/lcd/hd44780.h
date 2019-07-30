/***************************************************************************
 *            hd44780.h
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

#ifndef _HD44780_H
	#define _HD44780_H

	#include "config.h"

	// LCD size now in config.h!

	void HD44780_init( void );
	void HD44780_sendCMD( char CMD );
	void HD44780_sendDATA( char Data );
	void HD44780_clrscreen( void );
	void HD44780_setXY( char Xpos, char Ypos );
	void HD44780_SendChar( char Data );
	void HD44780_Backlight( unsigned char value );

#if defined(myAVR)
	
	#define		HD44780_E_PORT				PORTA
	#define		HD44780_E_PIN				PA5
	#define		HD44780_E_DDR				DDRA
	#define		HD44780_RS_PORT				PORTA
	#define		HD44780_RS_PIN				PA4
	#define		HD44780_RS_DDR				DDRA
	#define		HD44780_RW_PORT				PORTB
	#define		HD44780_RW_PIN				PB2
	#define		HD44780_RW_DDR				DDRB
	
	#define		HD44780_DATAPORT_1			PORTA
	#define		HD44780_DATADDR_1			DDRA
	#define		HD44780_DATAMASK_1			0xC0
	
	#define		HD44780_DATAPORT_2			PORTB
	#define		HD44780_DATADDR_2			DDRB
	#define		HD44780_DATAMASK_2			0x03
	#define		HD44780_DELAY_WRITEDATA		0
	#define		HD44780_DELAY_COMMAND		0
	#define		HD44780_DELAY_SOFT_RESET1	5
	#define		HD44780_DELAY_SOFT_RESET2	1
	#define		HD44780_DELAY_SOFT_RESET3	1
	#define		HD44780_DELAY_SET_4BITMODE	5	
	#define		HD44780_DELAY_ENABLE		10

#elif defined(ATXM2)
	
	#define		HD44780_E_PORT				PORTA
	#define		HD44780_E_PIN				6
	#define		HD44780_RS_PORT				PORTA
	#define		HD44780_RS_PIN				4
	#define		HD44780_RW_PORT				PORTA
	#define		HD44780_RW_PIN				5
	
	#define		HD44780_DATAPORT			PORTA
	#define		HD44780_DATAMASK			0x0F

	#define		HD44780_DELAY_WRITEDATA		0
	#define		HD44780_DELAY_COMMAND		0
	#define		HD44780_DELAY_SOFT_RESET1	5
	#define		HD44780_DELAY_SOFT_RESET2	1
	#define		HD44780_DELAY_SOFT_RESET3	1
	#define		HD44780_DELAY_SET_4BITMODE	5
	#define		HD44780_DELAY_ENABLE		10

#else
	#error "LCD wird für diese Hardwareplatform nicht unterstützt, bitte in der config.h abwählen!"
#endif
				
#endif /* HD44780_H */

/**
 * @}
 */

