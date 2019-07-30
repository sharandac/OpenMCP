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
 
 #ifndef _HD44780_GEN_H
	#define _HD44780_GEN_H

	#define		HD44780_ROW_1				0x00
	#define		HD44780_ROW_2				0x40
	#define		HD44780_ROW_3				0x10
	#define		HD44780_ROW_4				0x50
	
	  // CLEAR DISPLAY
	#define		HD44780_CMD_CLEAR			0x01
	  // CURSOR HOME
	#define		HD44780_CMD_CURSOR_HOME		0x02
	  // SET ENTRY
	#define		HD44780_CMD_SET_ENTRY		0x04
	#define		HD44780_SE_DECREASE			0x00
	#define		HD44780_SE_INCREASE			0x02
	#define		HD44780_SE_NOSHIFT			0x00
	#define		HD44780_SE_SHIFT			0x01
	  // SET DISPLAY
	#define		HD44780_CMD_SET_DISPLAY		0x08
	#define		HD44780_SD_DISPLAY_OFF		0x00
	#define		HD44780_SD_DISPLAY_ON		0x04
	#define		HD44780_SD_CURSOR_OFF		0x00
	#define		HD44780_SD_CURSOR_ON		0x02
	#define		HD44780_SD_BLINKING_OFF		0x00
	#define		HD44780_SD_BLINKING_ON		0x01
	  // SET FUNCTION
	#define		HD44780_CMD_SET_FUNCTION	0x20
	#define		HD44780_SF_4BIT				0x00
	#define		HD44780_SF_8BIT				0x10
	#define		HD44780_SF_1LINE			0x00
	#define		HD44780_SF_2LINES			0x08
	#define		HD44780_SF_5x7				0x00
	#define		HD44780_SF_5x10				0x04
	  // SOFT RESET
	#define		HD44780_CMD_SOFT_RESET		0x30
	  // SET CG RAM ADDRESS
	#define		HD44780_CMD_CGRAM			0x40
	  // SET DD RAM ADDRESS
	#define		HD44780_CMD_DDRAM			0x80
			
#endif /* HD44780_H */

/**
 * @}
 */

