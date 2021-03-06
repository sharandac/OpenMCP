/***************************************************************************
 *            checksum.c
 *
 *  Sat Jun  3 20:17:31 2006
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
 * \addtogroup CHECKSUM 16-bit Checksummenfuntion (checksum.c)
 * \par Uebersicht 
 * Berechnung der 16.Bit Checksumme eines Speicherbereiches im RAM.
 * Benotig z.B. fuer die Checksumme von Datenpacketen wie in TCP/IP.
 * 
 * @{
 */

/**
 *
 * \author Dirk Broßwick
 *
 */

#include "checksum.h"

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Bildet die CRC16 über einen Speicherbereich,
 * \param 	* data		Zeiger auf den Speicherbereich.
 * \param	len			Große des Speicherbereiches in Bytes  
 * \retval	CRC16
 */
/* -----------------------------------------------------------------------------------------------------------*/
int Checksum_16 ( char * data, int len )
	{
		unsigned long checksum = 0x0;
		unsigned int result;
		unsigned char DataH;
		unsigned char DataL;
		
		//Jetzt werden alle Packete in einer While Schleife addiert
		while( len > 1)
			{
				DataH=*data++;
				DataL=*data++;
				result =~ ((DataH << 8)+ DataL);
				checksum = checksum + result;
				//decrimiert Länge von TCP Headerschleife um 2
				len -=2 ;
			}
		//Ist der Wert result16 ungerade ist DataL = 0
		if( len > 0)
			{
				DataH=*data;
				result =~ (DataH << 8);
				checksum = checksum + result;
			}
		//Komplementbildung (addiert Long INT_H Byte mit Long INT L Byte)
		checksum = ((checksum & 0x0000FFFF)+ ((checksum & 0xFFFF0000) >> 16));
		checksum = ((checksum & 0x0000FFFF)+ ((checksum & 0xFFFF0000) >> 16));
		checksum = (checksum & 0x0000FFFF);
		return (checksum);
}

/**
 * @}
 */
