/*! \file led_core.h \brief Stellt die LED Funkionalitaet bereit */
//***************************************************************************
//*            led_core.h
//*
//*  Sat Jun  3 23:01:42 2006
//*  Copyright  2006  User
//*  Email
//****************************************************************************/
///	\ingroup hardware
///	\defgroup LED Stellt die LED Funkionalitaet bereit (led_core.h)
///	\code #include "led_core.h" \endcode
///	\par Uebersicht
///		Stellt funktionen bereit um LED über ein definiertes interface anzusprechen.
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

#ifndef _LED_CORE_H
	#define _LED_CORE_H

	void LED_init( void );
	void LED_on ( char LED_index );
	void LED_off ( char LED_index );
	void LED_toggle ( char LED_index );

#endif /* _LED_CORE_H */

 
