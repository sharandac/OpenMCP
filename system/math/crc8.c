/***************************************************************************
 *            crc8.c
 *
 *  Sun Mai 01 01:30:31 2011
 *  Copyright  2006  Dirk Broßwick
 *  Email
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
 * \ingroup system
 * \addtogroup CRC8 8-bit Checksummenfuntion (crc8.c)
 * \par Uebersicht 
 * Berechnung von crc8 eines Speicherbereichs
 * Benoetigt z.B. fuer die Checksumme der ID von 1Wire Devices * 
 *
 * @{
 */

/**
 *
 * \author Dirk Broßwick
 *
 */

#include "crc8.h"

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Bildet die CRC8 über einen Speicherbereich,
 * \param 	* data		Zeiger auf den Speicherbereich.
 * \param	len			Größe des Speicherbereiches in Bytes  
 * \retval	CRC8
 */
/* -----------------------------------------------------------------------------------------------------------*/
char crc8( char * data, char len )
{
	uint8_t crc, loop_count, b, feedback_bit;

	crc = 0x00;		// crc init

	for (loop_count = 0; loop_count != len; loop_count++)
	{
		b = data[loop_count];

		char i;
		
		for (i=0;i<8;i++)
		{
			feedback_bit = (crc ^ b) & 0x01;

			if ( feedback_bit == 0x01 )
			{
				crc = crc ^ 0x18;	// 0x18 = POLYNOM (X^8+X^5+X^4+X^0)
			}

			crc = (crc >> 1) & 0x7F;

			if ( feedback_bit == 0x01 )
			{
				crc |= 0x80;
			}

			b = b >> 1;
		}
	}

	return crc;
}

/**
 * @}
 */
