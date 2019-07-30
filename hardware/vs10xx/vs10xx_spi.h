/***************************************************************************
 *            vs10xx_spi.h
 *
 *  Thu Dec 15 19:18:51 2011
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

#ifndef _VS10XX_SPI

	#define _VS10XX_SPI

	void VS10XX_SPI_init( void );
	char VS10XX_SPI_send_data( char data );
	char VS10XX_SPI_send_cmd( char data );

#if defined(__AVR_XMEGA__)
	
	#include <avr/io.h>

	#define MISO_PORT		PORTF
	#define MISO 			0

	#define DCLK_PORT		PORTF
	#define DCLK			1

	#define SCK_PORT		PORTF
	#define SCK				2

	#define MOSI_PORT		PORTF
	#define MOSI			3

	#define SS2_PORT		PORTF
	#define SS2				4

	#define RESET_PORT		PORTF
	#define RESET			5

	#define DREQ_PORT		PORTF
	#define DREQ_PIN		PORTF.IN
	#define DREQ			6

	#define BSYNC_PORT		PORTF
	#define BSYNC			7

	// Steuerleitungen einrichten für MP3-Decoder
	#define VS10XX_POWER_init
	#define VS10XX_RESET_init		( RESET_PORT.DIRSET = ( 1<<RESET ) )
	#define VS10XX_BSYNC_init		( BSYNC_PORT.DIRSET = ( 1<<BSYNC ) )
	#define VS10XX_DREQ_init		( DREQ_PORT.DIRCLR = ( 1<<DREQ ) )
	#define VS10XX_SS2_init			( SS2_PORT.DIRSET = ( 1<<SS2 ) )

	#define VS10XX_select			( SS2_PORT.OUTCLR = ( 1<<SS2 ) )
	#define VS10XX_deselect			( SS2_PORT.OUTSET = ( 1<<SS2 ) )
	#define VS10XX_select_bsync		( BSYNC_PORT.OUTSET = ( 1<<BSYNC ) )
	#define VS10XX_deselect_bsync	( BSYNC_PORT.OUTCLR = ( 1<<BSYNC ) )
	#define VS10XX_RESET_low		( RESET_PORT.OUTCLR = ( 1<<RESET ) )
	#define VS10XX_RESET_high		( RESET_PORT.OUTSET = ( 1<<RESET ) )
	#define VS10XX_POWER_low
	#define VS10XX_POWER_high

#else

	#define SS2_PORT		PORTB
	#define SS2_DDR			DDRB
	#define SS2				PB0

	#define RESET_PORT		PORTB
	#define RESET_DDR		DDRB
	#define RESET			PB4

	#define	BSYNC_DDR		DDRF
	#define BSYNC_PORT		PORTF
	#define BSYNC			PF0

	#define DREQ_DDR		DDRE
	#define DREQ_PORT		PORTE
	#define DREQ_PIN		PINE
	#define DREQ			PE5

	#define POWER_DDR		DDRF
	#define POWER_PORT		PORTF
	#define POWER			PF1

	// Steuerleitungen einrichten für MP3-Decoder
	#define VS10XX_POWER_init		( POWER_DDR |= ( 1<<POWER ) )
	#define VS10XX_RESET_init		( RESET_DDR |= ( 1<<RESET ) )
	#define VS10XX_BSYNC_init		( BSYNC_DDR |= ( 1<<BSYNC ) )
	#define VS10XX_DREQ_init		( DREQ_DDR &= ~( 1<<DREQ ) )
	#define VS10XX_SS2_init			( SS2_DDR |= ( 1<<SS2 ) )

	#define VS10XX_select			( SS2_PORT &= ~( 1<<SS2 ) )
	#define VS10XX_deselect			( SS2_PORT |= ( 1<<SS2 ) )
	#define VS10XX_select_bsync		( BSYNC_PORT |= ( 1<<BSYNC ) )
	#define VS10XX_deselect_bsync	( BSYNC_PORT &= ~( 1<<BSYNC ) )
	#define VS10XX_RESET_low		( RESET_PORT &= ~( 1<<RESET ) )
	#define VS10XX_RESET_high		( RESET_PORT |= ( 1<<RESET ) )
	#define VS10XX_POWER_low		( POWER_PORT &= ~( 1<<POWER ) )
	#define VS10XX_POWER_high		( POWER_PORT |= ( 1<<POWER ) )

#endif // __AVR_XMEGA__

#endif // _VS10XX_SPI