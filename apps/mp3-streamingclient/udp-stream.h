/***************************************************************************
 *            udp-stream.h
 *
 *  Mon Nov 28 19:37:36 2011
 *  Copyright  2011  Dirk Broßwick
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

#ifndef	_UDP_STREAM 

	#define _UDP_STREAM

	#include <stdint.h>

	void udp_stream_init( void );
	void udp_stream_callback( char * buffer, int len );

	#define	UDP_STREAM_PORT			1717

	enum stream_msgtype {
		data = 1,
		info,
		relay,
		init,
		control,
		video,
		config
	};

	enum stream_infotype {
		title = 1,
		source,
	};

	struct STREAM_PACKET {
		char		packet_type;
		char		data[];
	};

	struct STREAM_DATAPACKET {
		long		sequencenumber;
		int			len;
		char		data[];
	};

	struct STREAM_INFOPACKET {
		char		infotype;
		char		data[];
	};

	struct STREAM_RELAYPACKET {
		int			len;
		char		data[];
	};

#endif // _UDP_STREAM