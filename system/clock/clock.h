/*! \file clock.h \brief Stellt die CLOCK Funkionalitaet bereit */
//***************************************************************************
//*            clock.h
//*
//*  Sat Jun  3 23:01:42 2006
//*  Copyright  2006  Dirk Broßwick
//*  Email: sharandac@snafu.de
//****************************************************************************/
///	\ingroup system
/// \defgroup CLOCK
///	\par Uebersicht
///		Stellt funktionen bereit um eine genaue Zeit zu realisieren und funktionen
/// um Zeitgesteuert eigene Funktionen die man hinterlegt aufzurufen
//****************************************************************************/
/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
//@{

#ifndef _CLOCK_H
	#define _CLOCK_H

	#include "config.h"

	typedef void ( * CLOCK_CALLBACK_FUNC ) ( void );

	#if defined(EXTMEM)
		#define MAX_CLOCK_COUNTDOWNTIMER	32
		#define MAX_CLOCK_CALLBACKS		16
	#else
		#define MAX_CLOCK_COUNTDOWNTIMER	8
		#define MAX_CLOCK_CALLBACKS		8
	#endif

	#define MSECOUND			1
	#define SECOUND				2
	#define MINUTE				3
	#define HOUR				4
	#define DAY					5
	#define NO_USE				6

	#define	NonExecutionbit		0
	#define	Executionbit		1

	#define CLOCK_FAILED		-1
	#define CLOCK_OK			0

    #define	EPOCH_YR	1900
    //(24L * 60L * 60L)
    #define	SECS_DAY	86400UL  
    #define	LEAPYEAR(year)	(!((year) % 4) && (((year) % 100) || !((year) % 400)))
    #define	YEARSIZE(year)	(LEAPYEAR(year) ? 366 : 365)

    // Number of seconds between 1-Jan-1900 and 1-Jan-1970, unix time starts 1970
    // and ntp time starts 1900.
    #define GETTIMEOFDAY_TO_NTP_OFFSET 2208988800UL

	/*! \struct TIME
	 *  \brief Definiert die Struktur in der die Zeit an eine Anwendung übergeben wird.
	 */
	struct	TIME {
		volatile unsigned char ms;			/*!< Zeit in 1/100 Sekunden */
		volatile unsigned char ss;			/*!< Sekunden */
		volatile unsigned char mm;			/*!< Minuten */
		volatile unsigned char hh;			/*!< Stunden */
		volatile unsigned char WW;			/*!< Wochentag So = 0 und so weiter */
		volatile unsigned char DD;			/*!< Tag des Monats */
		volatile unsigned char MM;			/*!< Monat Januar = 0 */
		volatile unsigned int YY;			/*!< Jahr */
		volatile char timezone;				/*!< Zeitzone */
		volatile unsigned long uptime;		/*!< Laufzeit in Sekunden seid dem letzten Reset */
		volatile unsigned long time;		/*!< UTC */
	};

	/*! \struct CALLBACK
	 *  \brief Struktur für Callbackeinträge.
	 */
	struct CALLBACK {
		volatile CLOCK_CALLBACK_FUNC 	CallbackFunc;	/*!< Pointer auf die Funktion die aufgerufen wird. NULL wenn nicht benutzt */
		volatile unsigned char 			Resolution;		/*!< Die mögliche zeitliche auflösung */
		volatile unsigned char			Execution;		/*!< Der wert wird gesetzt wenn die Funktion nicht im aktuellen Tick passt, wird dann beim nächsten Tick ausgeführt */
	};
	
	/*! \struct COUNTER
	 *  \brief Hier werden die Counter verwaltet
	 */
	struct COUNTER {
		volatile unsigned long			Counter;	/*!< Der Counter der gezählt wird */
		volatile unsigned char			Resolution; /*!< Die Auflösung mit den der Counter verringert wird, siehe define MSECOUND, SECOUND, ... */
	};

	void CLOCK_init( void );
	void CLOCK_tick( void );
	char CLOCK_RegisterCallbackFunction( CLOCK_CALLBACK_FUNC pFunc, unsigned char Resolution);
	char CLOCK_RemoveCallbackFunction( CLOCK_CALLBACK_FUNC pFunc );
	int CLOCK_RegisterCountdowntimer( void );
	void CLOCK_SetCountdownTimer( int counter, unsigned int value, unsigned char Resolution );
	unsigned int CLOCK_GetCountdownTimer( int counter );
	void CLOCK_ReleaseCountdownTimer( int counter );
	int CLOCK_GetTime( struct TIME * Timestruct );
	int CLOCK_SetTime( struct TIME * Timestruct );
	void CLOCK_delay(unsigned int ms);
	unsigned char CLOCK_monthlen(unsigned char isleapyear,unsigned char month);
	void CLOCK_decode_time( struct TIME * Timestruct );
	void CLOCK_encode_time( struct TIME * Timestruct );
#endif /* _CLOCK_H */

//@}
