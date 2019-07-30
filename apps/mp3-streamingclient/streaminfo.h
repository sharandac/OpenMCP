/***************************************************************************
 *            lcd_streaminfo.h
 *
 *  Thu Feb  9 09:49:53 2012
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

#ifndef _STREAMINFO_H

	#define _STREAMINFO_H

	#define	IDLE_TIME	10

	int MENU_param_uint8( char * value, char from, char to);
	void streaminfo_init( void );
	void streaminfo_thread( void );
	void streaminfo_updatetitle( char * title );
	void streaminfo_updatesource( char * source );
	void STREAM_loadconfig( void );
	void STREAM_saveconfig( void );

	
#endif // _STREAMINFO_H