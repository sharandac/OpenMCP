/***************************************************************************
 *            vs10xx_buffer.h
 *
 *  Mon Feb  6 18:30:48 2012
 *  Copyright  2012  Dirk Broßwick
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

#ifndef _STREAMBUFF_H

	#define _STREAMBUFF_H

	enum stream_sourcetype {
		stream_http = 1,
		stream_udp,
		stream_file
	};


	void streambuffer_init( void );
	void streambuffer_irq( void );
	void streambuffer_fill( char * buffer, int len );
	void streambuffer_fillfifo( int fifo, int len );
	int streambuffer_getbandwidth ( void );
	int streambuffer_getfps ( void );
	int streambuffer_get ( int len );
	int streambuffer_getstate ( void );
	int streambuffer_getfifo ( void );
	char * streambuffer_getbase ( void );
	void streambuffer_setsource( char source );
	char streambuffer_getsource( void );
	long streambuffer_getcounter( void );

#endif // _STREAMBUFF_H