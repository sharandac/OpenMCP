/***************************************************************************
 *            spi_core.c
 *
 *  Sun Jan 18 11:39:35 2009
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
 
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 *
 * \ingroup Hardware
 * \addtogroup spi_core Interface für den SPI_Bus (spi_core.c/h)
 *
 * @{
 */

/**
 *
 * \author Dirk Broßwick
 *
 */

#include "spi_core.h"

#if defined(__AVR_ATmega2561__)
	#include "spi_0.h"
	#include "spi_1.h"
	#include "spi_2.h"

	#define SPI_BUSSES		3

	SPI_BUS spi_bus[ ] = {
		{ SPI0_init, SPI0_ReadWrite, SPI0_FastMem2Write, SPI0_FastRead2Mem , NULL },
		{ SPI1_init, SPI1_ReadWrite, SPI1_FastMem2Write, SPI1_FastRead2Mem , NULL },
		{ SPI2_init, SPI2_ReadWrite, SPI2_FastMem2Write, SPI2_FastRead2Mem , NULL } };

#elif defined(__AVR_AT90CAN128__) || defined(__AVR_ATmega644__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega1284P__)
	#if defined(myAVR)
		#include "spi_0.h"
		#include "spi_1.h"

	SPI_BUS spi_bus[ ] = {
		{ SPI0_init, SPI0_ReadWrite, SPI0_FastMem2Write, SPI0_FastRead2Mem , NULL },
		{ SPI1_init, SPI1_ReadWrite, SPI1_FastMem2Write, SPI1_FastRead2Mem , NULL } };

	#else
		#include "spi_0.h"
		#include "spi_2.h"

	SPI_BUS spi_bus[ ] = {
		{ SPI0_init, SPI0_ReadWrite, SPI0_FastMem2Write, SPI0_FastRead2Mem , NULL },
		{ SPI2_init, SPI2_ReadWrite, SPI2_FastMem2Write, SPI2_FastRead2Mem , NULL } };

	#endif

	#define SPI_BUSSES		2

#elif defined(__AVR_XMEGA__)
	#include "xmega_spi.h"

	#if defined(__AVR_ATxmega16D3__) || defined(__AVR_ATxmega32D3__) || defined(__AVR_ATxmega64D3__) || defined(__AVR_ATxmega128D3__)
		#define SPI_BUSSES		3
	#elif defined(__AVR_ATxmega16D3__) || defined(__AVR_ATxmega32D3__) || defined(__AVR_ATxmega64D3__) || defined(__AVR_ATxmega128D3__)
		#define SPI_BUSSES		4
	#elif defined(__AVR_ATxmega64A3__) || defined(__AVR_ATxmega128A3__) || defined(__AVR_ATxmega192A3__) || defined(__AVR_ATxmega256A3__) || defined(__AVR_ATxmega256A3B__)
		#define SPI_BUSSES		4
	#elif defined(__AVR_ATxmega64A1__) || defined(__AVR_ATxmega128A1__) || defined(__AVR_ATxmega192A1__) || defined(__AVR_ATxmega256A1__) || defined(__AVR_ATxmega384A1__)
		#define SPI_BUSSES		4
	#endif

#endif

/**
 * \brief Die Init fuer dir SPI-Schnittstelle. Es koennen verschiedene Geschwindigkeiten eingestellt werden.
 */
void SPI_InitCore( void )
{
}

void SPI_init( int SPI_ID )
{
	if ( SPI_ID < SPI_BUSSES )
#if defined(__AVR_XMEGA__)
		xmega_spi_init( SPI_ID );
#else
		spi_bus[ SPI_ID ].INIT( );
#endif
}

/**
 * \brief Schreibt einen Wert auf den SPI-Bus. Gleichzeitig wird ein Wert von diesem im Takt eingelesen.
 * \warning	Auf den SPI-Bus sollte vorher per Chip-select ein Baustein ausgewaehlt werden. Dies geschied nicht in der SPI-Routine sondern
 * muss von der Aufrufenden Funktion gemacht werden.
 * \param	SPI_ID	Nummer des SPI_Bus der angesprochen werden soll.
 * \param 	Data	Der Wert der uebertragen werden soll.
 * \retval  Data	Der wert der gleichzeit empfangen wurde.
 */
char SPI_ReadWrite( int SPI_ID, char Data )
{
	if ( SPI_ID < SPI_BUSSES )
#if defined(__AVR_XMEGA__)
		return( xmega_spi_ReadWrite( SPI_ID, Data ) );
#else
		return( spi_bus[ SPI_ID ].ReadWrite( Data ) );	
#endif
	else
		return( 0 );
}

/**
 * \brief Eine schnelle MEM->SPI Blocksende Routine mit optimierungen auf Speed.
 * \param	SPI_ID		Nummer des SPI_Bus der angesprochen werden soll.
 * \param	buffer		Zeiger auf den Puffer der gesendet werden soll.
 * \param	Datalenght	Anzahl der Bytes die gesedet werden soll.
 */
void SPI_WriteBlock( int SPI_ID, char * buffer, int Datalenght )
{
	if ( SPI_ID < SPI_BUSSES )
#if defined(__AVR_XMEGA__)
		xmega_spi_WriteBlock( SPI_ID, buffer, Datalenght );
#else
		spi_bus[ SPI_ID ].WriteBlock( buffer, Datalenght );	
#endif
}

/**
 * \brief Eine schnelle SPI->MEM Blockempfangroutine mit optimierungen auf Speed.
 * \warning Auf einigen Controller laufen die Optimierungen nicht richtig. Bitte teil des Sourcecode der dies verursacht ist auskommentiert.
 * \param	SPI_ID		Nummer des SPI_Bus der angesprochen werden soll.
 * \param	buffer		Zeiger auf den Puffer wohin die Daten geschrieben werden sollen.
 * \param	Datalenght	Anzahl der Bytes die empfangen werden sollen.
 */
void SPI_ReadBlock( int SPI_ID, char * buffer, int Datalenght )
{
	if ( SPI_ID < SPI_BUSSES )
#if defined(__AVR_XMEGA__)
		xmega_spi_ReadBlock( SPI_ID, buffer, Datalenght );
#else
		spi_bus[ SPI_ID ].ReadBlock( buffer, Datalenght );		
#endif
}

/**
 * \brief 
 * \warning Auf einigen Controller laufen die Optimierungen nicht richtig. Bitte teil des Sourcecode der dies verursacht ist auskommentiert.
 * \param	SPI_ID		Nummer des SPI_Bus der angesprochen werden soll.
 * \param	buffer		Prescaler der gesetzt werden soll. (0,2,8,16,64,128)
 */
void SPI_SetPrescaler( int SPI_ID, char Prescaler )
{
/*
#if defined(__AVR_XMEGA__)
	if ( SPI_ID < SPI_BUSSES )
		xmega_spi_ReadBlock( SPI_ID, buffer, Datalenght );
#else
		spi_bus[ SPI_ID ].ReadBlock( buffer, Datalenght );		
#endif
*/
}

/**
 * @}
 */
