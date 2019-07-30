/***************************************************************************
 *            gpio_out.h
 *
 *  Mon Mar  2 03:36:12 2009
 *  Copyright  2009  Dirk Broßwick
 *  <sharandac@snafu.de>
 ***************************************************************************/
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

#ifndef _GPIO_OUT_H
	#define _GPIO_OUT_H
	void __attribute__((noinline)) GPIO_out_init( void );
	void __attribute__((noinline)) GPIO_out_set( char pin );
	void __attribute__((noinline)) GPIO_out_clear( char pin );
	char __attribute__((noinline)) GPIO_out_state( char pin );
	char __attribute__((noinline)) GPIO_out_max( void );
	char __attribute__((noinline)) GPIO_out_Port( char pin );

/*	void GPIO_out_init( void );
	void GPIO_out_set( char pin );
	void GPIO_out_clear( char pin );
	char GPIO_out_state( char pin );
	char GPIO_out_max( void );
	char GPIO_out_Port( char pin );
*/
#endif /* _GPIO_OUT_H */
