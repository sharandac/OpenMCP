/***************************************************************************
 *            tftp-server.h
 *
 *  Sat Mar  3 21:00:22 2012
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

#ifndef _TFTP_SERVER_H

	#define _TFTP_SERVER_H

	#define TFTPSERVER_PORT	69
	#define MAX_TFTP_PACKET_SIZE 516

	void TFTPSERVER_init( void );
	void TFTPSERVER_thread( void );
	void TFTPSERVER_read( void );
	void TFTPSERVER_write( void );

	enum tftserver_msgtype {
		TFTP_RRQ = 1,
		TFTP_WRQ,
		TFTP_DATA,
		TFTP_ACK,
		TFTP_ERR
	};

	struct TFTP_PACKET {
		int			packet_type;
		char		data;
	};

	struct TFTP_PACKET_EXT {
		int			block;
		char		data;
	};

#endif // _TFTP_SERVER_H