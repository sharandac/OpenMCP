/***************************************************************************
 *            zg2100_hardware.h
 *
 *  Tue Sep  3 22:40:40 2013
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

#ifndef _ZG2100_HARDWARE_H
	#define _ZG2100_HARDWARE_H

	#define ZG2100_SPI_ID		1

	#define	ZG2100_PORT			PORTB
	#define ZG2100_RESET		1
	#define ZG2100_HIBERNATE	2
	#define ZG2100_INT			3
	#define ZG2100_CS			4
	#define ZG2100_MOSI			5
	#define ZG2100_MISO			6
	#define ZG2100_SCK			7

#endif /* _ZG2100_Hardware_H */