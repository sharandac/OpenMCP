/***************************************************************************
 *            uart_c1.h
 *
 *  Thu Dec  9 11:21:03 2010
 *  Copyright  2010  sharan
 *  <sharan@<host>>
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

#ifndef _UART_C1_H
	#define _UART_C1_H

	void UART_C1_init( void );
	void UART_C1_SendByte( char Byte );
	char UART_C1_GetByte( void );
	int UART_C1_GetBytesinRxBuffer( void );
	int UART_C1_GetBytesinTxBuffer( void ); 

#endif // _UART_C1_H
