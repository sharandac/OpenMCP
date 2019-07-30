/*! \file vs10xx.c \brief Stellt Funktionen für den VS10xx Decoder bereit */
//***************************************************************************
//*            vs10xx.c
//*
//*  Mon May 12 17:46:47 2008
//*  Copyright  2008 Dirk Broßwick
//*  Email: sharandac@snafu.de
//****************************************************************************/
///	\ingroup Hardware
///	\defgroup VS10xx Funktionen für den VS10xx (vs10xx.c)
///	\code #include "vs10xx.h" \endcode
///	\par Uebersicht
///		Stellt Funktionen für den MP3-Decoder VS10xx von VLSI bereit. 
///	Unter www.vlsi.fi finden sich weiter Beispiele zu weiteren Decoder von
/// VLSI. Einige Funktionen entstammen dem Yampp-Projekt und sind an diese
///	angelehnt.
//****************************************************************************/
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
//@{
#include <avr/io.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "config.h"

#if defined(VS10XX)

#if defined(__AVR_XMEGA__)
	#include <system/clock/delay_x.h>
#else
	#include "util/delay.h"
#endif

#include "vs10xx.h"
#include "vs10xx_buffer.h"
#include "vs10xx_spi.h"
#include "vs10xx_plugin.h"
#include "system/clock/clock.h"
#include "system/buffer/fifo.h"

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Initialisiert den VS1001k.
 * \param 	NONE
 * \return	NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
char VS10xx_INIT( void )
{

	printf_P( PSTR("VS10xx initialisieren, found an "));

	// Steuerleitungen einrichten für MP3-Decoder
	VS10XX_POWER_init;
	VS10XX_RESET_init;
	VS10XX_BSYNC_init;
	VS10XX_DREQ_init;
	VS10XX_SS2_init;
	
	// Steuerleitungen auf Startzustand setzen
	VS10XX_RESET_high;
	VS10XX_deselect;
	VS10XX_deselect_bsync;
		
	VS10XX_SPI_init();

	// MP3-Decoder einschalten
	VS10XX_POWER_high;

	// 100ms warten bis Spannung stabil ist und Controller sie initialisiert hat
	CLOCK_delay( 100 );

	// reset the decoder
	VS10xx_reset();
	VS10xx_vol ( 255, 255 );

	// Wenn xtal nicht erfolgreich gesetzt
	if ( VS10xx_get_xtal() != VS10xx_CLOCKF )
	{
		printf_P( PSTR("Reset failed\r\n"));
		return( RESET_FAILED );
	}
	
	switch( VS10xx_GetVersion() )
	{
		case 0: 	printf_P( PSTR("VS1001,"));
					break;
		case 1: 	printf_P( PSTR("VS1011,"));
					break;
		case 2: 	printf_P( PSTR("VS1002 or VS1011e (with plugin),"));
					VS10XX_loadplugin();
					break;
		case 3: 	printf_P( PSTR("VS1003,"));
					break;
		default:	printf_P( PSTR(" unknown VS10xx,"));
					break;
	}
	
	printf_P( PSTR(" Clockspeed = %d.%03dMHz\r\n"), ( VS10xx_get_xtal() * 2)/1000 ,  ( VS10xx_get_xtal() * 2)%1000 );

	streambuffer_init();
	
	return( RESET_OK );
}

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Holt den Typ des Decoders.
 * \param 	NONE
 * \return	Version
 */
/*------------------------------------------------------------------------------------------------------------*/

int VS10xx_GetVersion( void )
{
	int Version;
	
	Version = VS10xx_read( VS10xx_Register_STATUS ) & ( 7<<4 );
	
	return( Version>>4 );
}

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Liest ein Register vom VS10xx aus.
 * \param 	address	Registernummer im VS10xx.
 * \return	Value	Der Ihanlt des 16-Bit-Register.
 */
/*------------------------------------------------------------------------------------------------------------*/
int VS10xx_read( char address)
{
	char sreg_temp;
	unsigned int Data;

	sreg_temp = SREG;
	cli();
	
	VS10XX_select;

	asm volatile("nop");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("nop");

	VS10XX_SPI_send_cmd( VS10xx_READ );
	VS10XX_SPI_send_cmd( address );

	Data = VS10XX_SPI_send_cmd( 0 ) << 8;
	Data |= VS10XX_SPI_send_cmd( 0 );

	asm volatile("nop");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("nop");

	VS10XX_deselect;
	
	SREG = sreg_temp;

	return( Data );
}


/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Beschreibt ein Register im VS10xx.
 * \param 	adress		Registernummer im VS10xx.
 * \param 	Data		Daten die ins Register sollen.
 * \return	NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
void VS10xx_write( char address, int Data)
{
	char sreg_temp;

	sreg_temp = SREG;
	cli();
	
	VS10XX_select;

	asm volatile("nop");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("nop");

	VS10XX_SPI_send_cmd( VS10xx_WRITE);
	VS10XX_SPI_send_cmd( address);

	VS10XX_SPI_send_cmd( ( char) (Data >> 8));
	VS10XX_SPI_send_cmd( ( char) (Data ));

	asm volatile("nop");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("nop");

	VS10XX_deselect;

	SREG = sreg_temp;
}

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Befüllt den FIFO des Decoders des VS10xx
 * \param 	FIFO		Die Nummer des FIFO aus dem der VS10xx gefüllt werden soll.
 * \return	NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
int VS10xx_flush_from_FIFO( int FIFO )
{
	VS10XX_select_bsync;

	char buffer[32];
	int j,i=0;
	
//	while( bit_is_set( DREQ_PIN , DREQ ) && Get_Bytes_in_FIFO ( FIFO ) >= 32 )
	while( ( DREQ_PIN & ( 1<<DREQ ) ) != 0 && Get_Bytes_in_FIFO ( FIFO ) >= 32 )
	{
		i++;
		Get_Block_from_FIFO ( FIFO, 32, buffer );

		for (j=0;j<32;j++)
		{
			VS10XX_SPI_send_data( buffer[j] );
		}
	}

	VS10XX_deselect_bsync;

	return(i);
}


/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Sendet 32Byte an den FIFO des  Decoders.
 * \param 	FIFO		Die Nummer des FIFO aus dem der VS10xx gefüllt werden soll.
 * \return	NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
void VS10xx_send_32_from_FIFO( int FIFO )
{

	VS10XX_select_bsync;

	int j;
	
	for (j=0;j<32;j++)
	{
		VS10XX_SPI_send_data( Get_Byte_from_FIFO ( FIFO ) );
	}

	VS10XX_deselect_bsync;
	
}


/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Sendet 32Byte an den FIFO des  Decoders.
 * \param 	pBuffer		Pointer auf einen 32 Byte größen Puffer.
 * \return	NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
void VS10xx_send_32( char *pBuffer )
{

	VS10XX_select_bsync;

	int j;
	
	for (j=0;j<32;j++)
	{
		VS10XX_SPI_send_data( *pBuffer );
		pBuffer++;
	}

	VS10XX_deselect_bsync;

}


/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Sendet ein Byte an den FIFO des VS10xx.
 * \param 	Data		Das Byte welches rein soll.
 * \return	NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
void VS10xx_send_data( char Data )
{
	
	VS10XX_select_bsync;

	VS10XX_SPI_send_data( Data );		// send data

	VS10XX_deselect_bsync;

}


/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Initialisiert den VS1001k.
 * \param 	reset_e r	Softreset oder Hardwarereset das ist hier die frage.
 * \return	NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
char VS10xx_reset( void )
{
	char R,L;

	R = 255 - ( VS10xx_read(VS10xx_Register_VOL)>>8 );
	L = 255 - ( VS10xx_read(VS10xx_Register_VOL) & 0xff );

	VS10XX_RESET_low;
	_delay_ms(50);	// 10 mS	    
	VS10XX_RESET_high;
	_delay_ms(100);	// 50 mS	    
				
	VS10xx_set_xtal ( VS10xx_CLOCKF );
	VS10xx_vol ( R, L );

	return( RESET_OK );
}

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Initialisiert den VS1001k.
 * \param 	reset_e r	Softreset oder Hardwarereset das ist hier die frage.
 * \return	NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
void VS10xx_streammode( void )
{
	int mode;
	
	mode = VS10xx_read( VS10xx_Register_MODE );
	mode |= (1<<6);
	VS10xx_write( VS10xx_Register_MODE, mode);
}

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Initialisiert den VS1001k.
 * \param 	reset_e r	Softreset oder Hardwarereset das ist hier die frage.
 * \return	NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
void VS10xx_normalmode( void )
{
	int mode;
	
	mode = VS10xx_read( VS10xx_Register_MODE );
	mode &= ~(1<<6);
	VS10xx_write( VS10xx_Register_MODE, mode);
}

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Initialisiert den VS1001k.
 * \param 	reset_e r	Softreset oder Hardwarereset das ist hier die frage.
 * \return	NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
char VS10xx_softreset( void )
{
	char R,L;

	R = 255 - ( VS10xx_read(VS10xx_Register_VOL)>>8 );
	L = 255 - ( VS10xx_read(VS10xx_Register_VOL) & 0xff );
		
	VS10XX_select;
	VS10xx_write( VS10xx_Register_MODE, 0x0004 );
	VS10XX_deselect;
	
	_delay_us( 1000 );
	
	VS10xx_set_xtal ( VS10xx_CLOCKF );
	VS10xx_vol ( R, L );

	return( RESET_OK );
}

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Schreibt anzahl von nullen an den VS10xx, wird für reset gebraucht.
 * \param 	nNulls		Anzahl der Nullen die geschrieben werden sollen.
 * \return	NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
void VS10xx_nulls( int nNulls)
{
	while (nNulls--)
		VS10xx_send_data(0);
}


/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Sinustest ein.
 * \param 	freq		Die Frequenz, sieht Datenblatt.
 * \return	NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
void VS10xx_sine_on( char freq)
{
	unsigned char i,buf[4];
	
	// sine on
	buf[0] = 0x53;	buf[1] = 0xEF;	buf[2] = 0x6E;	buf[3] = freq;

	for (i=0;i<4;i++)
		VS10xx_send_data(buf[i]);
	VS10xx_nulls(4);
}


/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Schlatet den Sinustest aus.
 * \param 	NONE
 * \return	NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
void VS10xx_sine_off(void)
{
	unsigned char i,buf[4];

	// sine off
	buf[0] = 0x45;	buf[1] = 0x78;	buf[2] = 0x69;	buf[3] = 0x74;

	for (i=0;i<4;i++)
		VS10xx_send_data(buf[i]);
	VS10xx_nulls(4);
}


/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Durchlaufender Sinustest.
 * \param 	NONE
 * \return	NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
char VS10xx_sine_sweep(void)
{
	char i;

	for (i=48;i<119;i++)
	{
		VS10xx_sine_off();
		VS10xx_sine_on(i);	
		CLOCK_delay(250);
	}
	VS10xx_sine_off();
	return 0;	
}


/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Setzt die Lautstärke des Decoders. 0-255.
 * \param 	Lvol	Linke Seite die Laustärke.
 * \param 	Rvol	Rechte Seite die Laustärke.
 * \return	NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
void VS10xx_vol( unsigned char Lvol, unsigned char Rvol )
{
	VS10xx_write ( VS10xx_Register_VOL, ( ( 255 - Lvol )<<8 ) | ( 255 - Rvol ) );
}


/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Setzt die Frequenz des XTAL.
 * \param 	NONE
 * \return	NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
void VS10xx_set_xtal( int clock )
{
	if ( clock < ( 13000000/2000 ) )
		clock |= 0x8000;
	
	VS10xx_write( VS10xx_Register_CLOCKF, clock );
}

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Setzt die Frequenz des XTAL.
 * \param 	NONE
 * \return	NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
int VS10xx_get_xtal( void )
{
	int clock;
	
	clock = VS10xx_read( VS10xx_Register_CLOCKF );

	return( clock & 0x7fff );
}

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Holt die Zeit die der Decoder schon decodiert.
 * \param 	NONE
 * \return	time	Zeit in Sekunden.
 */
/*------------------------------------------------------------------------------------------------------------*/
int VS10xx_get_decodetime( void )
{
	return ( VS10xx_read( VS10xx_Register_DECODE_TIME ) );
}


#endif

//@}
