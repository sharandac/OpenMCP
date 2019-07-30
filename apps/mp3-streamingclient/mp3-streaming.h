/*! \file mp3-streaming.h \brief MP3-Streamingengine zum streamen von MP3 */
//***************************************************************************
//*            mp3-streaming.h
//*
//*  Sat May  10 21:07:42 2008
//*  Copyright  2008  Dirk Broßwick
//*  Email: sharandac@snafu.de
//****************************************************************************/
///	\ingroup software
///	\addtogroup mp3stream Tsumani MP3-Streamingengine (mp3streaming.h)
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
#ifndef _MP3_STREAMING_H
	#define _MP3_STREAMING_H

	void mp3client_thread( void );
	void mp3client_stream( int socket, int fifo );
	void mp3client_init( void );
	void mp3client_stopstreaming( void );
	void mp3client_startstreaming( char * streamingPROTO, char * streamingURL, long streamingIP, int streamingPORT, char * streamingFILE );
	unsigned char mp3client_setverboselevel( void );
	unsigned char mp3client_setmetainfo( void );
	void mp3client_autoplay( char autoplay );
	int mp3client_pharseheader( unsigned int socket );
	void mp3client_readmetainfo( int fifo );
//	void mp3client_readmetainfo( char * metainfo, unsigned int metainfolen );

	#define pitch_factor	50; // = 100khz ( 100kHz / 2000kHz = 50 ) see datasheet; siehe Datenblatt !!

	#define mp3_buffer_size	1024*8

	#define buffercleaninterval	25

	#define MP3TCPtranslen	1024ul

#endif /* _MP3_STREAMING_H */
//@}
