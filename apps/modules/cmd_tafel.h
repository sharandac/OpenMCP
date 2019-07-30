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
 
#ifndef _CMD_TAFEL_H
	#define _CMD_TAFEL_H

	void init_cmd_tafel( void );
	void cgi_tafel( void * pStruct );
	int cmd_tafel( int argc, char ** argv );

	#define TAFEL_UDP_Bufferlen 	2000
	#define TAFEL_UDPPORT			2342

#endif /* _CMD_TAFEL_H */

