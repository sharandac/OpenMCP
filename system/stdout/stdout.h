/*! \file stdout.h \brief STDOUT-Funktion um die Ausgaben umzulenken */
//***************************************************************************
//*            stdout.h
//*
//*  Sat July  13 21:07:42 2008
//*  Copyright  2008  Dirk Broßwick
//*  Email: sharandac@snafu.de
//****************************************************************************/
///	\ingroup system
///	\defgroup stdout Stdout Funktionen (stdout.h)
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
#ifndef _STDOUT_H
	#define _STDOUT_H

	#include "config.h"

	void STDOUT_INIT( void );
	void STDOUT_set( char TYPE, int DEVICE );
	void STDOUT_save( void * pStruct );
	void STDOUT_restore( void * pStruct );
	void STDOUT_Flush( void );

	/*! \struct STDOUT
	 * \brief Definiert die Struktur für den StandartOut. Hier werden alle wichtigen Daten zur Standartout gesichert, welche Verbindungsart u.s.w.
	 * und es wird der Puffer für die Daten verwaltet, das ist z.b. bei TCP wichtig, damit nicht wür jedes gesendete Byte ein komplettes Packet gesendet wird,
	 * das würde eine menge Traffic und Banbdbreite verschwenden.
	 */
	struct STDOUT {
		volatile char					TYPE;				/*!< Speichert den Verbindungtyp. */
		volatile unsigned int			DEVICE;				/*!< Speichert das Subdevice. */
		volatile unsigned char *		BUFFER;				/*!< Pointer auf den Puffer für Daten wenn benötig. */
		volatile int					BUFFER_POS;			/*!< Position im Puffer. */
	};

	#if defined(EXTMEM)
		#define STDIO_BUFFER 512
	#elif defined(__AVR_ATmega1284P__)
		#define STDIO_BUFFER 512
	#elif defined(__AVR_ATxmega128A1__)
		#define STDIO_BUFFER 512
	#else
		#define STDIO_BUFFER 32
	#endif

	#define UNKNOWN			-1
	#define NONE			0
	#define	RS232			1
	#define _TCP			2
	#define TAFEL			3
	#define _LCD			4
	#define	USER			5
	#define	_KEYBOARD		6
	#define	_ENCODER_P2W1	7

#endif /* STDOUT_H */

 //@}
