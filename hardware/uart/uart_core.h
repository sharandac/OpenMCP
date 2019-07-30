/***************************************************************************
 *            uart_core.h
 *
 *  Thu Dec  9 11:34:40 2010
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

#ifndef _UART_CORE_H

	#define _UART_CORE_H

	#include "config.h"

	#define UBRR_VAL (F_CPU/(UART_BAUD*16)-1)

	#if defined(EXTMEM)
		#define TX_Bufferlen  	256
		#define RX_Bufferlen  	256
	#else
		#define TX_Bufferlen  	16
		#define RX_Bufferlen  	16
	#endif

	#define TX_complete		0
	#define TX_sending		1

	void UART_init( int UART );
	void UART_SendByte( int UART, char Byte );
	char UART_GetByte( int UART );
	int UART_GetBytesinRxBuffer( int UART );
	int UART_GetBytesinTxBuffer( int UART );

	typedef void pUART_init ( void );
	typedef void pUART_SendByte ( char Data );
	typedef char pUART_GetByte ( void );
	typedef int pUART_GetBytesinRxBuffer ( void );
	typedef int pUART_GetBytesinTxBuffer ( void );

	typedef struct {
		pUART_init					* init;
		pUART_SendByte				* SendByte;
		pUART_GetByte				* GetByte;
		pUART_GetBytesinRxBuffer	* GetBytesinRxBuffer;
		pUART_GetBytesinTxBuffer	* GetBytesinTxBuffer;
	} const UART_NUM ;		

#endif // _UART_CORE_H
