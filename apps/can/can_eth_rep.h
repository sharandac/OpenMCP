/***************************************************************************
 *            can.h
 *
 *  Tue Nov 30 01:11:46 2010
 *  Copyright  2010  sharan
 *  <sharan@<host>>
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

#ifndef CANETHERNET_H

#define CANETHERNET_H

#define CANETHREP_UDP_Bufferlen			50
#define CANETHREP_UDPPORT			50707
#define CANETHREP_MULTICASTGROUP	IPDOT( 239l, 40l, 41l, 42l );

void CANETHREP_Callback( char * Buffer, int Bufferlen );
void CANETHREP_init( void );
void CANETHREP_thread( void );

#endif // CANETHERNET_H
