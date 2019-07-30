/*! \file stdout.c \brief STDOUT-Funktion um die Ausgaben umzulenken */
//***************************************************************************
//*            stdout.c
//*
//*  Sat July  13 21:07:42 2008
//*  Copyright  2008  Dirk Broßwick
//*  Email: sharandac@snafu.de
//****************************************************************************/
///	\ingroup system
///	\defgroup stdout Stdout Funktionen (stdout.c)
///	\code #include "stdout.h" \endcode
///	\par Uebersicht
//****************************************************************************/
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
//@{
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include "stdout.h"
#include "config.h"
#include "hardware/uart/uart_core.h"

#if defined(TCP)
	#include "system/net/tcp.h"
#endif
#if defined(KEYBOARD)
	#include "hardware/keyboard/keyboard.h"
#endif
#if defined(ENCODER_P2W1)
	#include "hardware/keyboard/encoder_p2w1.h"
#endif
#if defined(LCD)
	#include "hardware/lcd/lcd.h"
#endif
#if defined(LEDTAFEL)
	#include "hardware/led-tafel/led_tafel.h"
#endif
#if defined(LEDTAFEL_PAR)
	#include "hardware/led-tafel/led_tafel-par.h"
#endif

int STDOUT_Send_Byte ( char Byte, FILE * stream );
int STDOUT_Get_Byte ( FILE * stream );

static unsigned char BUFFER[ STDIO_BUFFER ];
 
struct STDOUT streamout;

// stdout auf UART_Send_Byte verbiegen
static FILE mystdout = FDEV_SETUP_STREAM( STDOUT_Send_Byte , STDOUT_Get_Byte , _FDEV_SETUP_RW );

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Initialisiert die StandardOut.
 * \retval	void		NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
void STDOUT_INIT( void )
{
	stderr = stdout = stdin = &mystdout;
	streamout.TYPE = UNKNOWN;
	streamout.BUFFER = BUFFER ;
	streamout.BUFFER_POS = 0 ;
}	

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Sendet ein Byte auf den StandardOut Stream
 * \param   Byte		Byte welchen gesendet werden soll.
 * \param   stream		Pointer auf den Stream.
 * \retval  int			keiner.
 */
/*------------------------------------------------------------------------------------------------------------*/
int STDOUT_Send_Byte ( char Byte, FILE * stream )
{
	switch ( streamout.TYPE )
	{
		
		case RS232:		UART_SendByte ( streamout.DEVICE, Byte );
						break;		
#if defined(TCP)
		case _TCP:		if ( streamout.BUFFER_POS == STDIO_BUFFER )
							STDOUT_Flush();
		
						streamout.BUFFER[ streamout.BUFFER_POS++ ] = Byte;
						break;
#endif
#if defined(LCD)
		case _LCD:		LCD_sendchar( streamout.DEVICE, Byte );
#endif
#if defined(LEDTAFEL) || defined(LEDTAFEL_PAR)
		case TAFEL:		LEDTAFEL_print_char_stdio( Byte );
						break;
#endif
		default:		break;
	}

	return(0);
}

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Holt ein Byte von der StandardOut.
 * \param   stream		Pointer auf den Stream über den sendet werden soll.
 * \retval  int			Das Byte oder End Of File (EOF). Derzeit wird immer EOF zurück geliefert.
 */
/*------------------------------------------------------------------------------------------------------------*/
int STDOUT_Get_Byte ( FILE * stream )
{
	int Data = EOF;
	
	switch ( streamout.TYPE )
	{
		case RS232:		if( UART_GetBytesinRxBuffer( streamout.DEVICE ) > 0 )
							Data = ( int ) UART_GetByte ( streamout.DEVICE );
						break;		
		#ifdef TCP
		case _TCP:		if ( GetBytesInSocketData( streamout.DEVICE ) > 0 )
							Data = ( int ) GetByteFromSocketData( streamout.DEVICE );
						break;
		#endif
		#ifdef KEYBOARD
		case _KEYBOARD:	Data = KEYBOARD_GetKey();
						break;
		#endif
		#ifdef ENCODER_P2W1
		case _ENCODER_P2W1:
						Data = ENCODER_P2W1_GetKey();
						break;
		#endif
		default:		break;
	}
	return( Data );
}

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Setzt eine neue StandardOut.
 * \code
 *
 *		// Neuen Stream einstellen, z.b. eine TCP-Verbindung
 *		STDOUT_set( _TCP , SOCKET );
 *
 * \endcode
 * \param 	TYPE		Typ auf den umgebogen werden soll.
 * \param 	DEVICE		Devicenummer oder Socket auf den umgebogen werden soll.
 */
/*------------------------------------------------------------------------------------------------------------*/
void STDOUT_set( char TYPE, int DEVICE )
{
	STDOUT_Flush();
	streamout.TYPE = TYPE;
	streamout.DEVICE = DEVICE;
	streamout.BUFFER_POS = 0;
}

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Sichert die aktuelle StandardOut.
 * \code
 *
 *		// Struktur anlegen für STDOUT
 *		struct STDOUT oldstream;
 *
 *		// alte Struktur sichern
 *		STDOUT_save( &oldstream );
 *
 * \endcode
 * \param 	pStruct		Pointer auf Struktur vom Typ streamout in den der aktuelle StandardOut gesichert werden soll.
 */
/*------------------------------------------------------------------------------------------------------------*/
void STDOUT_save( void * pStruct )
{
	STDOUT_Flush();
	memcpy( pStruct, &streamout, sizeof( streamout ) );
}

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Stelle den gesicherten StandardOut wieder her.
 * \code *
 *		// Struktur anlegen für STDOUT
 *		struct STDOUT oldstream;
 *
 *		// alte Struktur sichern
 *		STDOUT_save( &oldstream );
 *		// Neuen Stream einstellen, z.b. eine TCP-Verbindung
 *		STDOUT_set( _TCP , SOCKET );
 *
 *		// mach mal nen paar andere Sachen
 *		foorbar();
 *
 *		STDOUT_restore( &oldstream );
 *
 * \endcode
 * \param 	pStruct		Pointer auf Struktur vom Typ streamout in den der wieder hergestellt werden soll.
 */
/*------------------------------------------------------------------------------------------------------------*/
void STDOUT_restore( void * pStruct )
{
	STDOUT_Flush();
	memcpy( &streamout, pStruct, sizeof( streamout ) );
}

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Schickt alle Daten die sich um Puffer befinden könnten raus.
 */
/*------------------------------------------------------------------------------------------------------------*/
void STDOUT_Flush( void )
{
	switch( streamout.TYPE )
	{
		case	RS232:		break;
#ifdef TCP
		case	_TCP:		if ( streamout.BUFFER_POS != 0 )
								PutSocketData_RPE ( streamout.DEVICE, streamout.BUFFER_POS, ( char *) streamout.BUFFER, RAM );
							streamout.BUFFER_POS = 0;
							break;
#endif
#if defined(LCD)
		case	_LCD:		break;
#endif
#if defined(KEYBOARD)		
		case	_KEYBOARD:	KEYBOARD_Flush();
							break;
#endif
		default:			break;
	}
}

//@}
