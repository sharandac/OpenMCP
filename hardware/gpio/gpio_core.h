/***************************************************************************
 *            gpio_core.h
 *
 *  Sat Oct  2 19:28:41 2010
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

#ifndef GPIO_CORE_H
	#define GPIO_CORE_H

#if defined(OpenMCP)
	#define MAX_GPIO_PORTS	4
	#define MAX_GPIO_PIN	32
#elif defined(AVRNETIO) || defined(myAVR) || defined( EtherSense )
	#define MAX_GPIO_PORTS	4
	#define MAX_GPIO_PIN	32
#elif defined(XPLAIN) || defined(UPP)
	#define MAX_GPIO_PORTS	6
	#define MAX_GPIO_PIN	48
#elif defined(ATXM2)
	#define MAX_GPIO_PORTS	6
	#define MAX_GPIO_PIN	48
#else
	#error "Hardware wird nicht unterstützt."
#endif

#if MAX_GPIO_PORTS > 0
	#define	PORTA_0		0x00
	#define	PORTA_1		0x01
	#define	PORTA_2		0x02
	#define	PORTA_3		0x03
	#define	PORTA_4		0x04
	#define	PORTA_5		0x05
	#define	PORTA_6		0x06
	#define	PORTA_7		0x07
#endif

#if MAX_GPIO_PORTS > 1
	#define	PORTB_0		0x08
	#define	PORTB_1		0x09
	#define	PORTB_2		0x0A
	#define	PORTB_3		0x0B
	#define	PORTB_4		0x0C
	#define	PORTB_5		0x0D
	#define	PORTB_6		0x0E
	#define	PORTB_7		0x0F
#endif

#if MAX_GPIO_PORTS > 2
	#define	PORTC_0		0x10
	#define	PORTC_1		0x11
	#define	PORTC_2		0x12
	#define	PORTC_3		0x13
	#define	PORTC_4		0x14
	#define	PORTC_5		0x15
	#define	PORTC_6		0x16
	#define	PORTC_7		0x17
#endif

#if MAX_GPIO_PORTS > 3
	#define	PORTD_0		0x18
	#define	PORTD_1		0x19
	#define	PORTD_2		0x1A
	#define	PORTD_3		0x1B
	#define	PORTD_4		0x1C
	#define	PORTD_5		0x1D
	#define	PORTD_6		0x1E
	#define	PORTD_7		0x1F
#endif

#if MAX_GPIO_PORTS > 4
	#define	PORTE_0		0x20
	#define	PORTE_1		0x21
	#define	PORTE_2		0x22
	#define	PORTE_3		0x23
	#define	PORTE_4		0x24
	#define	PORTE_5		0x25
	#define	PORTE_6		0x26
	#define	PORTE_7		0x27
#endif

#if MAX_GPIO_PORTS > 5
	#define	PORTF_0		0x28
	#define	PORTF_1		0x29
	#define	PORTF_2		0x2A
	#define	PORTF_3		0x2B
	#define	PORTF_4		0x2C
	#define	PORTF_5		0x2D
	#define	PORTF_6		0x2E
	#define	PORTF_7		0x2F
#endif

#if MAX_GPIO_PORTS > 6
	#define	PORTG_0		0x30
	#define	PORTG_1		0x31
	#define	PORTG_2		0x32
	#define	PORTG_3		0x33
	#define	PORTG_4		0x34
	#define	PORTG_5		0x35
	#define	PORTG_6		0x36
	#define	PORTG_7		0x37
#endif

	#define GPIO_OUT		1
	#define GPIO_IN			2

	#define GPIO_OK			0
	#define GPIO_ERROR		-1

	int GPIO_setdirection( char direction, char PIN );
	int GPIO_getdirection( char PIN );
	int GPIO_setPin( char PIN );
	int GPIO_clearPin( char PIN );
	int GPIO_getPin( char PIN );
	int GPIO_setPullUp( char PIN );
	int GPIO_clearPullUp( char PIN );

#endif // GPIO_CORE_H
