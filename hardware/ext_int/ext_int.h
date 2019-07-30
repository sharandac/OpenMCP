//***************************************************************************
//*            ext_int.h
//*
//*  Mon Jul 31 21:46:47 2006
//*  Copyright  2006  Dirk Broßwick
//*  Email: sharandac@snafu.de
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
 
#ifndef _EXT_INT_H
	#define _EXT_INT_H

	typedef void ( * EXT_INT_CALLBACK_FUNC ) ( void );

	void EXTINT_init( void );
	#if defined(__AVR_XMEGA__)
		char EXTINT_set ( PORT_t * Port, int Pin, int interrupt_sensemode, EXT_INT_CALLBACK_FUNC pFunc );
		void EXTINT_block ( PORT_t * PORT, int Pin );
		void EXTINT_free ( PORT_t * PORT, int Pin );
	#else
		char EXTINT_set ( int interrupt_number, int interrupt_sensemode, EXT_INT_CALLBACK_FUNC pFunc );
		void EXTINT_free ( int interrupt_number );
		void EXTINT_block ( int interrupt_number );
	#endif

	// only one ext_int per Port implement
	#if defined(__AVR_XMEGA__)
		#define		MAX_EXT_INT				16
	#elif defined(__AVR_ATmega2561__) || defined(__AVR_AT90CAN128__)
		#define 	MAX_EXT_INT				8
	#elif defined(__AVR_ATmega644__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega1284P__)
		#define 	MAX_EXT_INT				3
	#endif

	#define		ERROR_SENSEMODE			-1
	#define		ERROR_INTERRUPTNUMBER	-2
	#define		OK						0

	#if defined(__AVR_XMEGA__)
		#define		SENSE_CHANGE			0x00
		#define		SENSE_RISING			0x01
		#define		SENSE_FALLING			0x02
		#define     SENSE_LOW				0x03
		#define     SENSE_DISABLE			0x07
	#else
		#define     SENSE_LOW				0x00
		#define		SENSE_CHANGE			0x01
		#define		SENSE_FALLING			0x02
		#define		SENSE_RISING			0x03
	#endif

#endif /* _EXT_INT_H */

 
