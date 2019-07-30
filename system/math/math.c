/***************************************************************************
 *            math.c
 *
 *  Sun Jun 11 20:30:57 2006
 *  Copyright  2006  User
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
 * \addtogroup MATH Einige Mathfunktionen die AVR-GCC nicht hat (math.c)
 * \par Uebersicht 
 * Einige Funktionen die AVR-GCC nicht hat wie das tauschen
 * der Bytes in einen int oder long. Big-Endian <-> Little-Endian. 
 *
 * @{
 */

/**
 *
 * \author Dirk Broßwick
 *
 */

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Umwandlung von Big-Endian <-> Little-Endian.
 * \param 	Wert		Der zu wandelne Wert
 * \retval	Umgewandelter Wert
 */
/* -----------------------------------------------------------------------------------------------------------*/
unsigned long ChangeEndian32bit( unsigned long Wert)
{
#if defined(__AVR__) || defined(__AVR_XMEGA__)

	asm volatile (
	              "eor %A0, %D0"	"\n\t"
	              "eor %D0, %A0"	"\n\t"
	              "eor %A0, %D0"	"\n\t"
	              "eor %B0, %C0"	"\n\t"
	              "eor %C0, %B0"	"\n\t"
	              "eor %B0, %C0"
	              : "=r" (Wert)
	              : "0"  (Wert)
	              );
	return Wert;

#else
	Wert = ( ( Wert & 0xff000000 ) >> 24 ) | ( ( Wert & 0x000000ff ) << 24 ) | ( ( Wert & 0x00ff0000 ) >> 8 ) | ( ( Wert & 0x0000ff00 ) << 8 );
	return( Wert );
#endif
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Umwandlung von Big-Endian <-> Little-Endian.
 * \param 	Wert		Der zu wandelne Wert
 * \retval	Umgewandelter Wert
 */
/* -----------------------------------------------------------------------------------------------------------*/
unsigned int ChangeEndian16bit( unsigned int Wert)
{
#if defined(__AVR__) || defined(__AVR_XMEGA__)
	asm volatile (
	              "eor %A0, %B0"   "\n\t"
	              "eor %B0, %A0"   "\n\t"
	              "eor %A0, %B0"
	              : "=r" (Wert)
	              : "0"  (Wert)
	              );
	return Wert;	
#else
	Wert = ( ( Wert & 0xff00 ) >> 8 ) | ( ( Wert & 0x00ff ) << 8 );
	return( Wert );
#endif
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Umwandlung einer BCD-Zahl nach Binär
 * \param 	val		Der zu wandelne Wert
 * \retval	Umgewandelter Wert
 */
/* -----------------------------------------------------------------------------------------------------------*/
char bcd2bin( char val)
{
	return( ( val & 0x0f ) + ( ( val & 0xf0 ) >> 4 ) * 10 ); 
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Umwandlung einer Binär-Zahl nach BCD
 * \param 	val		Der zu wandelne Wert
 * \retval	Umgewandelter Wert
 */
/* -----------------------------------------------------------------------------------------------------------*/
char bin2bcd( char val)
{
	return( ( val % 10 ) + ( ( val / 10 ) << 4 ) ); 
}

/**
 * @}
 */
