/***************************************************************************
 *            zg2100.h
 *
 *  Tue Sep  3 18:42:42 2013
 *  Copyright  2013  Dirk Broßwick
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
 

#ifndef _ZG2100_COM_H
	#define _ZG2100_COM_H

	#include <stdbool.h>

	#define ZG2100_CMD_SIZE		16

	#define	ZG2100_READ			1
	#define ZG2100_WRITE		2
	#define ZG2100_RW			3

	void zg2100_com_init( void );
	 int zg2100_read_register(  int addr );
	void zg2100_write_register(  int addr,  int reg );
	 int zg2100_indexregister_read(  int addr );
	void zg2100_indexregister_write(  int addr ,  int reg );
	void zg2100_fifo_read(  char fifo,  char * type,  char * subtype,  char * buffer,  int len );
	void zg2100_fifo_write(  char fifo,  char type,  char subtype,  char * buffer,  int len, bool start, bool stop );
	void zg2100_trans(  char * buffer,  int len, bool chipsel,  char direction );
	char zg2100_soft_spi( char byte );

#endif /* _ZG2100_COM_H */