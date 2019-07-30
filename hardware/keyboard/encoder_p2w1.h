/***************************************************************************
 *            encoder_p2w1.h
 *
 *  Thu Mar 15 13:40:45 2012
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
 
#ifndef _ENCODER_P2W1_H

	#define _ENCODER_P2W1_H

	#define	ENCODER_P2W1_Bufferlen	16
	#define ENCODER_P2W1_DELAY		50
	#define ENCODER_P2W1_REPEAT		5

	void ENCODER_P2W1_init( void );
	void ENCODER_P2W1_Interrupt( void );
	int ENCODER_P2W1_GetKey( void );
	void ENCODER_P2W1_Flush( void );

#endif // _ENCODER_P2W1_H
