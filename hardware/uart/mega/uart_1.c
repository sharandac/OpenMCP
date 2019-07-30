/***************************************************************************
 *            uart_1.c
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
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "hardware/uart/uart_core.h"
#include "system/buffer/fifo.h"

#if !defined(__AVR_XMEGA__) && defined(__AVR__) && !defined(__AVR_ATmega644__)

#include "uart_1.h"

volatile unsigned char TX_1_state = TX_complete;

char TX_1_Buffer[ TX_Bufferlen ];
char RX_1_Buffer[ RX_Bufferlen ];

int RX_1_fifo;
int TX_1_fifo;

/* -----------------------------------------------------------------------------------------------------------*/
/*!\brief Initialisiert die Uart0.
 * \param	NONE
 * \return  NONE
 */
/* -----------------------------------------------------------------------------------------------------------*/
void UART_1_init( void )
{
	// FIFO reservieren für RX und TX
	RX_1_fifo = Get_FIFO( RX_1_Buffer, RX_Bufferlen );
	TX_1_fifo = Get_FIFO( TX_1_Buffer, TX_Bufferlen );

	// TX_state auf complete setzen, da ja nix gesendet wurde
	TX_1_state = TX_complete;

	// Bitrate einstellen
	UBRR1H = (unsigned char) ( UBRR_VAL>>8 );
	UBRR1L = (unsigned char) UBRR_VAL;									

	// UART Tx,Rx,RxINT einschalten, UDRIE und RXCIE werden erst während der Benutzung freigegeben nach bedarf
	UCSR1B = ( 1<<TXEN1 ) | ( 1<<RXEN1 ) | ( 1<<RXCIE1 ) ; 
	// Asynchron 8N1
	UCSR1C = ( 1<<UCSZ11 ) | ( 1<<UCSZ10 ) ;    								 
	
	return;
}

/* -----------------------------------------------------------------------------------------------------------*/
/*!\brief Interruptroutine zum senden, wenn Transmiter ist fertig und keine weiteren Daten im Puffer
 */
/* -----------------------------------------------------------------------------------------------------------*/
ISR( USART1_TX_vect )
{
	// Wenn der TX_puffer leer ist, Interrupt sperren
	UCSR1B &= ~( 1<<TXCIE1 );
	TX_1_state = TX_complete;
}

/* -----------------------------------------------------------------------------------------------------------*/
/*!\brief Interruptroutine zum senden, wird aufgerufen wenn UART-Register leer, aber Transmitter noch am senden
 */
/* -----------------------------------------------------------------------------------------------------------*/
ISR( USART1_UDRE_vect )
{
	// checken ob noch bytes im Buffer sind
	if ( Get_Bytes_in_FIFO ( TX_1_fifo ) != 0 )
	{
		UDR1 = Get_Byte_from_FIFO ( TX_1_fifo );
	}
	else
	{
		// Wenn nicht war es das letzte Byte im Puffer, dann diesen Interrupt sperren und TX-Interrupt freigeben
		UCSR1B |= ( 1<<TXCIE1 );
		UCSR1B &= ~( 1<<UDRIE1 );
	}
}

/* -----------------------------------------------------------------------------------------------------------*/
/*!\brief Interruptroutine zum empfangen
 */
/* -----------------------------------------------------------------------------------------------------------*/
ISR( USART1_RX_vect )
{
	char Byte;
	
	Byte = UDR1;
	
	if ( Get_Bytes_in_FIFO ( RX_1_fifo ) < RX_Bufferlen )
		Put_Byte_in_FIFO ( RX_1_fifo, Byte );
}

/* -----------------------------------------------------------------------------------------------------------*/
/*!\brief Sendet ein Byte über die Uart1.
 * \param	ein Byte vom typ unsigned char
 * \return  NONE
 */
/* -----------------------------------------------------------------------------------------------------------*/
void UART_1_SendByte( char Byte )
{
	char temp;

	temp = SREG;
	cli();
	
	if ( Get_Bytes_in_FIFO ( TX_1_fifo ) == 0 )
	{
		// Wenn Controller noch mit senden eines Byte beschäftig, ab in den Puffer
		if ( TX_1_state == TX_sending )
		{			
			// Byte in Buffer schreiben
			Put_Byte_in_FIFO ( TX_1_fifo, Byte );
			
			// Buffer Empty Interrupt freigeben, TX_complete sperren
			UCSR1B &= ~( 1<<TXCIE1 );
			UCSR1B |= ( 1<<UDRIE1 );
		}
		else
		{
			// Buffer Empty sperren, TX_complete freigeben
			UDR1 = Byte;
			// Buffer Emty sperren, TX_complete freigeben
			UCSR1B |= ( 1<<TXCIE1 );
			UCSR1B &= ~( 1<<UDRIE1 );
			
			TX_1_state = TX_sending;
		}
	}
	else
	{	
		// Wenn Puffer voll, warten bis wieder was rein paßt
		while ( 1 )
		{
			cli();
			
			if ( Get_Bytes_in_FIFO ( TX_1_fifo ) < TX_Bufferlen )
				break;
			
			SREG = temp;
		}

		// Byte in Buffer schreiben
		Put_Byte_in_FIFO ( TX_1_fifo, Byte );
	}
	
	SREG = temp;
}

/* -----------------------------------------------------------------------------------------------------------*/
/*!\brief Empfängt ein Byte über die Uart1.
 * \param	NONE
 * \return  Byte aus den Puffer
 */
/* -----------------------------------------------------------------------------------------------------------*/
char UART_1_GetByte( void )
{
	char Byte=0;
	char temp;
	
	temp = SREG;
	cli();
	
	// check ob überhaupt noch Bytes im Puffer sind, und wenn ja, auslesen
	if ( Get_Bytes_in_FIFO ( RX_1_fifo ) != 0 )
		Byte = Get_Byte_from_FIFO ( RX_1_fifo );
	
	SREG = temp;
	
	return( Byte );
}

/* -----------------------------------------------------------------------------------------------------------*/
/*!\brief Liest die Anzahl der Byte die im Empfangsbuffer sind.
 * \param	NONE
 * \return  Anzahl der Byte im Enpfangpuffer
 */
/* -----------------------------------------------------------------------------------------------------------*/
int UART_1_GetBytesinRxBuffer( void )
{
	int BytesInBuffer=1;
	char temp;

	temp = SREG;
	cli();
	
	// Anzahl der Bytes holen
	BytesInBuffer = Get_Bytes_in_FIFO ( RX_1_fifo );
	
	SREG = temp;
	
	return( BytesInBuffer );
}

/* -----------------------------------------------------------------------------------------------------------*/
/*!\brief Liest die Anzahl der Byte die im Empfangsbuffer sind.
 * \param	NONE
 * \return  Anzahl der Byte im Enpfangpuffer
 */
/* -----------------------------------------------------------------------------------------------------------*/
int UART_1_GetBytesinTxBuffer( void )
{
	int BytesInBuffer=1;
	char temp;

	temp = SREG;
	cli();
	
	// Anzahl der Bytes holen
	BytesInBuffer = Get_Bytes_in_FIFO ( TX_1_fifo );
	
	SREG = temp;
	
	return( BytesInBuffer );
}

#endif
