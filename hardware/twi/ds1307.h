/***************************************************************************
 *				ds1307.h
 *
 * Controller	: ATmega644p (Clock: 20000 Mhz-internal)
 * Compiler		: AVR-GCC (winAVR with AVRStudio)
 * Version 		: 2.4
 * Author		: CC K-H Legat
 * Date			: 18 Januar 2011
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

#ifndef DS1307_H_
	#define DS1307_H_

	#include "system/clock/clock.h"

	#define		DS1307_ADDR			0x68

	#define		DS1307_CH			0x80
	#define		DS1307_24H			0x40

	#define		DS1307_OK			0
	#define		DS1307_ERROR		1

	struct DS1307_MEM {
		char RTC_SECONDS;
		char RTC_MINUTES;
		char RTC_HOURS;
		char RTC_DAY;
		char RTC_DATE;
		char RTC_MONTH;
		char RTC_YEAR;
		char RTC_CONTROL;
	};

	int RTC_GetTimeExt( struct TIME * Timestruct );
	int RTC_WriteTimeExt( struct TIME * Timestruct );

#endif
//@}	
