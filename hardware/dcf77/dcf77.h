/***************************************************************************
 *            dcf77.h
 *
 *  Tue Jul 27 03:15:55 2010
 *  Copyright  2010  Dirk Broßwick
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

#ifndef _DFC77_H
	#define _DCF77_H

	#include "config.h"

#if defined(AVRNETIO)
	#define 	DCF77_PORT				PINA
	#define		DCF77_DDR				DDRA
	#define		DCF77_PIN				7
#elif defined(myAVR)
	#define 	DCF77_PORT				PINB
	#define		DCF77_DDR				DDRB
	#define		DCF77_PIN				4
#else
	#error "Plattform wird nicht mit DCF77 unterstützt."
#endif

	#define		DCF77_IMPULSE_0			100
	#define		DCF77_IMPULSE_1			200
	#define		DCF77_IMPULSE_PAUSE		850
	#define 	DCF77_IMPULSE_START		1850
	#define		DCF77_IMPULSE_TOLERANZ	50

	#define		DCF77_STATE_RECEIVE		0
	#define		DCF77_STATE_PARITYERROR	1
	#define		DCF77_STATE_FRAMEERROR	2
	#define		DCF77_STATE_WAITFORSYNC	3

	void DCF77_init( void );
	void DCF77_work( void );
	char bcd2bin( char val);

#endif
