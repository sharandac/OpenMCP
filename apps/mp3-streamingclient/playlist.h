/***************************************************************************
 *            playlist.h
 *
 *  Tue Mar 27 01:48:28 2012
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

#ifndef _PLAYLIST_H

	#define _PLAYLIST_H

	#define	MAX_PLAYLIST	20

	void PLAYLIST_init( void );
	void cgi_printplaylist( void * pStruct );
	void PLAYLIST_playentry( int entry );
	char * PLAYLIST_getlistpointer( void );
	void PLAYLIST_loadlist( void );
	int PLAYLIST_getfreeentry( void );
	int PLAYLIST_addentry( char * playliststring );
	int PLAYLIST_delentry( int entry );
	int PLAYLIST_findentry( char * playliststring );
	int PLAYLIST_getentry( char * playliststring, int entry );

#endif // _PLAYLIST_H