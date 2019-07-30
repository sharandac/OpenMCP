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

#include "endian.h"

int htons(int x)
{
#if defined(__AVR__) || defined(__AVR_XMEGA__)

	asm volatile (
		"eor %A0, %B0"   "\n\t"
		"eor %B0, %A0"   "\n\t"
		"eor %A0, %B0"
		: "=r" (x)
		: "0"  (x)
	);
	return x;
	
#else

	return ((((uint16_t)(A) & 0xff00) >> 8) | (((uint16_t)(A) & 0x00ff) << 8));

#endif
}

long htonl(long x)
{
#if defined(__AVR__) || defined(__AVR_XMEGA__)

	asm volatile (
		"eor %A0, %D0"	"\n\t"
		"eor %D0, %A0"	"\n\t"
		"eor %A0, %D0"	"\n\t"
		"eor %B0, %C0"	"\n\t"
		"eor %C0, %B0"	"\n\t"
		"eor %B0, %C0"
		: "=r" (x)
		: "0"  (x)
	);
	return x;
	
#else

	return ((((uint32_t)(A) & 0xff000000) >> 24) | (((uint32_t)(A) & 0x00ff0000) >> 8) | (((uint32_t)(A) & 0x0000ff00) << 8)  | (((uint32_t)(A) & 0x000000ff) << 24));

#endif
}
