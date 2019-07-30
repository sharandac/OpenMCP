/***************************************************************************
 *            mem-check.c
 *
 *  Wed Dec 23 17:04:05 2009
 *  Copyright  2009  Dirk Broßwick
 *  <sharandac@snafu.de>
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/**
 *
 * \ingroup Hardware
 * \addtogroup memcheck Prüft das interne RAM (mem-check.c)
 *
 * @{
 */

/**
 *
 * \author Dirk Broßwick
 *
 */


#include <avr/io.h> // RAMEND
#include "mem-check.h"

// Mask to init SRAM and check against
#define MASK 0xaa

// From linker script
extern unsigned char __heap_start;

/**
 * \brief Gibt den verwendeten Speicher in Bytes zurück der anhand der Maske ermittelt wird
 * \param	NONE
 * \return	Anzel der unbenutzten Bytes
 */
unsigned short get_mem_unused (void)
{
	unsigned short unused = 0;
	unsigned char *p = &__heap_start;
	
	do
	{
		if (*p++ != MASK)
			break;
			
		unused++;
	} while (p <= (unsigned char*) __heap_end);

	return unused;
}

/**
 * \brief Testet den Speicher
 * \warning !!! never call this function !!!
 * \param	NONE
 * \return	NONE
 */
void __attribute__ ((naked, section (".init8"))) __init8_mem (void)
{	
	__asm volatile (
		"ldi r30, lo8 (__heap_start)"  "\n\t"
		"ldi r31, hi8 (__heap_start)"  "\n\t"
		"ldi r24, %0"                  "\n\t"
		"ldi r25, hi8 (%1)"            "\n"
		"0:"                           "\n\t"
		"st  Z+,  r24"                 "\n\t"
		"cpi r30, lo8 (%1)"            "\n\t"
		"cpc r31, r25"                 "\n\t"
		"brlo 0b"
		:
		: "i" (MASK), "i" (__heap_end+1)
	);
}

/**
 * @}
 */
