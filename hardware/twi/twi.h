/***************************************************************************
 *            twi.h
 *
 *  Tue Oct  6 22:29:59 2009
 *  Copyright  2009  Dirk Broßwick
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
#ifndef _TWI_H
	#define _TWI_H

#if defined(myAVR) || defined(OpenMCP) || defined(AVRNETIO) || defined(UPP) || defined( EtherSense )

	// General TWI Master staus codes                      
	#define TWI_START					0x08  // START has been transmitted  
	#define TWI_REP_START				0x10  // Repeated START has been transmitted
	#define TWI_ARB_LOST				0x38  // Arbitration lost

	// TWI Master Transmitter staus codes                      
	#define TWI_MTX_ADR_ACK				0x18  // SLA+W has been tramsmitted and ACK received
	#define TWI_MTX_ADR_NACK			0x20  // SLA+W has been tramsmitted and NACK received 
	#define TWI_MTX_DATA_ACK			0x28  // Data byte has been tramsmitted and ACK received
	#define TWI_MTX_DATA_NACK			0x30  // Data byte has been tramsmitted and NACK received 

	// TWI Master Receiver staus codes  
	#define TWI_MRX_ADR_ACK				0x40  // SLA+R has been tramsmitted and ACK received
	#define TWI_MRX_ADR_NACK			0x48  // SLA+R has been tramsmitted and NACK received
	#define TWI_MRX_DATA_ACK			0x50  // Data byte has been received and ACK tramsmitted
	#define TWI_MRX_DATA_NACK			0x58  // Data byte has been received and NACK tramsmitted

	#define TWI_READ	1
	#define TWI_WRITE	0

	#define	TRUE		1
	#define	FALSE		0

	#define TWITIMEOUT	100

#endif

	char TWI_Init ( long TWI_Bitrate );
	char TWI_SendAddress ( char Address, char TWI_RW );
	void TWI_SendStop ( void );
	char TWI_Write ( char Data );
	char TWI_ReadAck ( void );
	char TWI_ReadNack ( void );
	int TWI_Scan( void );


#endif
