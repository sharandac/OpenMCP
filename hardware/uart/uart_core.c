/***************************************************************************
 *            uart_core.c
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
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "uart_core.h"
#include "config.h"

/**
 * \ingroup Hardware
 * \addtogroup UART Interface um die Uart anzusprechen (uart_core.c).
 *
 * Mit diesen Interface koennen alle UARTs angeprochen werden.
 *
 * @{
 */

/**
 *
 * \author Dirk Broßwick
 */

#if defined( OpenMCP )
	#include "hardware/uart/mega/uart_1.h"

	UART_NUM uart_num[ ] = {
		{ UART_1_init , UART_1_SendByte , UART_1_GetByte , UART_1_GetBytesinRxBuffer , UART_1_GetBytesinRxBuffer } };

#elif defined( EtherSense )
	#include "hardware/uart/mega/uart_0.h"

	UART_NUM uart_num[ ] = {
		{ UART_0_init , UART_0_SendByte , UART_0_GetByte , UART_0_GetBytesinRxBuffer , UART_0_GetBytesinRxBuffer } };

#elif defined( AVRNETIO )
	#include "hardware/uart/mega/uart_0.h"

	UART_NUM uart_num[ ] = {
		{ UART_0_init , UART_0_SendByte , UART_0_GetByte , UART_0_GetBytesinRxBuffer , UART_0_GetBytesinRxBuffer } };

#elif defined( myAVR )
	#include "hardware/uart/mega/uart_0.h"

	UART_NUM uart_num[ ] = {
		{ UART_0_init , UART_0_SendByte , UART_0_GetByte , UART_0_GetBytesinRxBuffer , UART_0_GetBytesinRxBuffer } };

#elif defined( UPP )
	#include "hardware/uart/mega/uart_1.h"

	UART_NUM uart_num[ ] = {
		{ UART_1_init , UART_1_SendByte , UART_1_GetByte , UART_1_GetBytesinRxBuffer , UART_1_GetBytesinRxBuffer }, };

#elif defined( ATXM2 )
	#include "hardware/uart/xmega/uart_c1.h"
	#include "hardware/uart/xmega/uart_d0.h"
	#include "hardware/uart/xmega/uart_e0.h"

	UART_NUM uart_num[ ] = {
		{ UART_C1_init , UART_C1_SendByte , UART_C1_GetByte , UART_C1_GetBytesinRxBuffer , UART_C1_GetBytesinRxBuffer } ,
		{ UART_D0_init , UART_D0_SendByte , UART_D0_GetByte , UART_D0_GetBytesinRxBuffer , UART_D0_GetBytesinRxBuffer } ,
		{ UART_E0_init , UART_E0_SendByte , UART_E0_GetByte , UART_E0_GetBytesinRxBuffer , UART_E0_GetBytesinRxBuffer } ,
		};

#elif defined( XPLAIN )
	#include "hardware/uart/xmega/uart_c0.h"
	
	UART_NUM uart_num[ ] = {
		{ UART_C0_init , UART_C0_SendByte , UART_C0_GetByte , UART_C0_GetBytesinRxBuffer , UART_C0_GetBytesinRxBuffer } };

#endif

/**
 * \brief Initialisiert die Uart.
 * \param	UART	Nummber der Uart.
 * \return  NONE
 */
void UART_init( int UART )
{
	uart_num[ UART ].init( );
}

/**
 * \brief Sendet ein Byte ueber die UART.
 * \param	UART	Nummber der Uart.
 * \param	Byte	Byte welches gesendet werden soll.
 * \return  NONE
 */
void UART_SendByte( int UART, char Byte )
{
	uart_num[ UART ].SendByte( Byte );
}

/**
 * \brief Empfaengt ein Byte ueber die UART.
 * \param	UART	Nummber der Uart.
 * \return  BYTE	Byte welches empfangen worden ist.
 */
char UART_GetByte( int UART )
{
	return( uart_num[ UART ].GetByte() );
}

/**
 * \brief Liest die Anzahl der Byte die sich im RX-Puffer befinden.
 * \param	UART	Nummber der Uart.
 * \return  SIZE	Anzahl der Bytes im Puffer.
 */
int UART_GetBytesinRxBuffer( int UART )
{
	return( uart_num[ UART ].GetBytesinRxBuffer() );
}

/**
 * \brief Liest die Anzahl der Byte die sich im TX-Puffer befinden.
 * \param	UART	Nummber der Uart.
 * \return  SIZE	Anzahl der Bytes im Puffer.
 */
int UART_GetBytesinTxBuffer( int UART )
{
	return( uart_num[ UART ].GetBytesinTxBuffer() );
}

/**
 * @}
 */
