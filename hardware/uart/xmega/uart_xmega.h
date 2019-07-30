/***************************************************************************
 *            uart_0.h
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

#ifndef _UART_XMEGA_H
	#define _UART_XMEGA_H

	#ifdef __AVR_XMEGA__
		#define USART_Format_Set(_usart, _charSize, _parityMode, _twoStopBits) 	(_usart)->CTRLC = (uint8_t) _charSize | _parityMode |  \
																				(_twoStopBits ? USART_SBMODE_bm : 0)
		#define USART_Baudrate_Set(_usart, _bselValue, _bScaleFactor)			(_usart)->BAUDCTRLA =(uint8_t)_bselValue; \
																				(_usart)->BAUDCTRLB =(_bScaleFactor << USART_BSCALE0_bp)|(_bselValue >> 8)
		#define USART_Rx_Enable(_usart)											((_usart)->CTRLB |= USART_RXEN_bm)
		#define USART_Rx_Disable(_usart)										((_usart)->CTRLB &= ~USART_RXEN_bm)
		#define USART_Tx_Enable(_usart)											((_usart)->CTRLB |= USART_TXEN_bm)
		#define USART_Tx_Disable(_usart)										((_usart)->CTRLB &= ~USART_TXEN_bm)
		#define USART_RxdInterruptLevel_Set(_usart, _rxdIntLevel)				((_usart)->CTRLA = ((_usart)->CTRLA & ~USART_RXCINTLVL_gm) | _rxdIntLevel)
		#define USART_TxdInterruptLevel_Set(_usart, _txdIntLevel)				(_usart)->CTRLA = ((_usart)->CTRLA & ~USART_TXCINTLVL_gm) | _txdIntLevel
		#define USART_DreInterruptLevel_Set(_usart, _dreIntLevel)				(_usart)->CTRLA = ((_usart)->CTRLA & ~USART_DREINTLVL_gm) | _dreIntLevel
		#define USART_SetMode(_usart, _usartMode)								((_usart)->CTRLC = ((_usart)->CTRLC & (~USART_CMODE_gm)) | _usartMode)
		#define USART_IsTXDataRegisterEmpty(_usart)								(((_usart)->STATUS & USART_DREIF_bm) != 0)
		#define USART_PutChar(_usart, _data)									((_usart)->DATA = _data)
		#define USART_IsRXComplete(_usart)										(((_usart)->STATUS & USART_RXCIF_bm) != 0)
		#define USART_GetChar(_usart)											((_usart)->DATA)
	#endif

#endif // _UART_XMEGA_H
