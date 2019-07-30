/**
 *            led_tafel-par.c
 *
 *  Wen Jun  1 19:39:37 2011
 *  Copyright  2011  Dirk Broßwick
 *  <sharandac@snafu.de>
 ***************************************************************************/
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

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/crc16.h>
#include <string.h>

#include "hardware/led/led_core.h"
#include "system/clock/clock.h"
#include "system/stdout/stdout.h"
#include "system/net/udp.h"
#include "system/softreset/softreset.h"
#include "util/delay.h"
#include "config.h"

#if defined(LEDTAFEL_PAR)

#include "font.h"
#include "led_tafel-par.h"

static char LEDTAFEL_screen_graphic[ LEDTAFEL_ROW * LEDTAFEL_LINES * 8];
static char LEDTAFEL_screenlum[ LEDTAFEL_ROW * LEDTAFEL_LINES ];

char knoten[] = {
	"  ,cCCCCCCCCCCCCCCCCCCCCCc,                             "
	"  CC'       `\\,    C    `CC                             "
	"  CC'       `\\,    C    `CC                             "
	"  CC          \\\\   C     CC                             "
	"  CC           `\\, \\================\\, \\ \\ \\            "
	"  CC             \\===================\\\\ \\ \\ \\           "
	"  CC            ,/====================\\| |/|/\\\\         "
	"  CC          ,//  /=================\\====/////         "
	"  CC         //    C     CC          .=====/ / /        "
	"  CC,       //     B    ,CC             /-=-/ //        "
	"  `CCCCCCCCCCCCCCCCCCCCCCC'            /  .\\|/\\/        "
	"                                            X  \\        "
	"                                           / \\  \\____/  "
	"  Willkommen im                           /  |  |       "
	"  Chaos Computer Club Berlin             /   /  |       "
	"                                            |  /        "
	"                                        ----|-`         "
	"                                            |           "
	"                                            \\           "
	"                                             \\          "
	"                                                        " };

/* -----------------------------------------------------------------------------------------------------------*/
/*! Initialisiert das komplette Display.
 * \param 	void
 * \retval  void
 */
/* -----------------------------------------------------------------------------------------------------------*/
void LEDTAFEL_init( void )
{
	// DatenPin auf OUT setzen
	DATA_DDR = 0xFF;
	DATA_PORT = 0x00;

	// Alle IO auf IN und LOW setzen
	CONTROL_DDR &= ~( CONTROL_REQ | CONTROL_SEL | CONTROL_ACK );
	CONTROL_PORT &= ~( CONTROL_REQ | CONTROL_SEL | CONTROL_ACK );
	CONTROL_SYNC_PORT &= ~( CONTROL_SYNC );
	CONTROL_SYNC_DDR &= ~( CONTROL_SYNC );

	// Select auf OUT und LOW setzen
	CONTROL_DDR |= CONTROL_SEL;
	CONTROL_DDR |= CONTROL_REQ;
	CONTROL_PORT &= ~CONTROL_SEL;
	CONTROL_PORT &= ~CONTROL_REQ;

	CLOCK_RegisterCallbackFunction ( LEDTAFEL_irq, MSECOUND );
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! Schreibt an die aktuelle Position ein Zeichen.
 * \param 	Row		Spalte an der das Zeichen geschrieben werden soll.
 * \param 	Line	Zeile an der das Zeichen geschrieben werden soll.
 * \param 	Zeichen	Zeichen das geschrieben werden soll.
 * \retval  void
 */
/* -----------------------------------------------------------------------------------------------------------*/
void LEDTAFEL_print_char( int Row, int Line, unsigned char Zeichen )
{
	memcpy( &LEDTAFEL_screen_graphic[ Row * 8 + Line * ( LEDTAFEL_LINES * 8 * LEDTAFEL_ROW ) ], &font[Zeichen][0] , 8 );
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! Schreibt an die aktuelle Position ein Zeichen ueber die stdout.
 * \param 	Zeichen		Das Zeichen das ausgegeben werden soll.
 * \retval  void
 */
/* -----------------------------------------------------------------------------------------------------------*/
void LEDTAFEL_print_char_stdio( char Zeichen )
{
	static char LEDTAFEL_row = 0;
	static char LEDTAFEL_line = 0;

	if ( Zeichen == '\r' )
	{
		LEDTAFEL_row = 0;
		return;
	}

	if ( Zeichen == '\n' )
	{
		LEDTAFEL_line++;
		if ( LEDTAFEL_line >= LEDTAFEL_LINES )
		{
			LEDTAFEL_line--;
			// scroll up
			memcpy( &LEDTAFEL_screen_graphic[ 0 ] , &LEDTAFEL_screen_graphic[ LEDTAFEL_ROW * 8 ] , ( LEDTAFEL_LINES - 1 ) * LEDTAFEL_ROW * 8 );
		}
		return;
	}

	if ( LEDTAFEL_row >= LEDTAFEL_ROW )
	{
		LEDTAFEL_row = 0;
		LEDTAFEL_line++;

		if ( LEDTAFEL_line >= LEDTAFEL_LINES )
		{
			LEDTAFEL_line--;
			// scroll up
			memcpy( &LEDTAFEL_screen_graphic[ 0 ] , &LEDTAFEL_screen_graphic[ LEDTAFEL_ROW * 8 ] , ( LEDTAFEL_LINES - 1 ) * LEDTAFEL_ROW * 8 );
			memset( &LEDTAFEL_screen_graphic[ ( LEDTAFEL_LINES - 1 ) * LEDTAFEL_ROW * 8 ] , 0 , LEDTAFEL_ROW * 8 ); 
		}
	}

	LEDTAFEL_print_char( LEDTAFEL_row, LEDTAFEL_line, Zeichen );
	LEDTAFEL_row++;
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! Interruptroutine zum bearbeiten und kopieren der Daten fuer die Zeilencontroller
 * \retval  void
 */
/* -----------------------------------------------------------------------------------------------------------*/
void LEDTAFEL_irq( void)
{
	static char Line = 0;
	char i;
	
	Line++;
	
	if ( Line == 4 )
	{
		for ( i = 0 ; i < LEDTAFEL_LINES ; i++ )
		{
			LEDTAFEL_sendblock( i );
		}
		Line = 0 ;
	}
	
/*
	for ( char i = 0 ; i < 3 ; i++ )
	{
		LEDTAFEL_sendblock( Line );
		Line++;
		if ( Line == LEDTAFEL_LINES )
			Line = 0;
	}
*/
	
/*
	LEDTAFEL_sendblock( Line );
	Line++;
	if ( Line == LEDTAFEL_LINES )
	if ( Line == 5 )
		Line = 0;
*/
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! Sendet einen Datenblock.
 * \param	line		Zeilennummer die uebertragen werden soll.
 * \retval  int			TAFELSENDERR oder TAFELSENDOK
 *				     ___
 *	SEL			____/   \_____________________________________________________		(Master)
 *					   ___
 *	SYNC		______/   \___________________________________________________		(Slave)
 *					   		    ____        ____        ____       ____
 *	REQ			_______________/    \______/    \______/    \_____/    \______		(Master)
 *
 *	DB[0..7]	--<LLLLLLLL><DDDD><DDDD><DDDD><DDDD><DDDD><DDDD><DDDD><DDDD>--		(Master)
 */
/* -----------------------------------------------------------------------------------------------------------*/
int LEDTAFEL_sendblock( char line ) 
{
	char data ;
	int i, block ;
	char * gfx_buffer, * lum_buffer;

	// Zeile setzen und SEL setzen
	DATA_PORT = line + 1;
	CONTROL_PORT |= CONTROL_SEL;

	LED_on(1);
	i = 0 ;
	// auf SYNC gleich HIGH warten oder in den Timeout laufen und abbrechen
	while( !( CONTROL_SYNC & CONTROL_SYNC_PIN ) )
	{
		i++;
		if ( i > TIMEOUTLOOP )
		{
			CONTROL_PORT &= ~CONTROL_SEL;
			LED_off(1);
			return( TAFELSENDERR );
		}
	}

	// SEL auf Low setzen
	CONTROL_PORT &= ~CONTROL_SEL;

	 i = 0;
	// auf SYNC gleich LOW warten oder in den Timeout laufen und abbrechen
	while( ( CONTROL_SYNC & CONTROL_SYNC_PIN ) )
	{
		i++;
		if ( i > TIMEOUTLOOP )
		{
			LED_off(1);
			return( TAFELSENDERR );
		}
	}

	gfx_buffer = &LEDTAFEL_screen_graphic[ line * LEDTAFEL_ROW * 8 ];
	block = LEDTAFEL_ROW;
		
	while( block )
	{
		data = *gfx_buffer;
		gfx_buffer++;
		CONTROL_PORT |= CONTROL_REQ;
		DATA_PORT = data;

		data = *gfx_buffer;
		gfx_buffer++;
		CONTROL_PORT &= ~CONTROL_REQ;
		DATA_PORT = data;

		data = *gfx_buffer;
		gfx_buffer++;
		CONTROL_PORT |= CONTROL_REQ;
		DATA_PORT = data;

		data = *gfx_buffer;
		gfx_buffer++;
		CONTROL_PORT &= ~CONTROL_REQ;
		DATA_PORT = data;

		data = *gfx_buffer;
		gfx_buffer++;
		CONTROL_PORT |= CONTROL_REQ;
		DATA_PORT = data;

		data = *gfx_buffer;
		gfx_buffer++;
		CONTROL_PORT &= ~CONTROL_REQ;
		DATA_PORT = data;

		data = *gfx_buffer;
		gfx_buffer++;
		CONTROL_PORT |= CONTROL_REQ;
		DATA_PORT = data;

		data = *gfx_buffer;
		gfx_buffer++;
		CONTROL_PORT &= ~CONTROL_REQ;
		DATA_PORT = data;

		block--;
	}

	lum_buffer = &LEDTAFEL_screenlum[ line * LEDTAFEL_ROW ];
	block = LEDTAFEL_ROW;

	while( block )
	{
		data = *lum_buffer;
		lum_buffer++;
		CONTROL_PORT |= CONTROL_REQ;
		DATA_PORT = data;

		data = *lum_buffer;
		lum_buffer++;
		CONTROL_PORT &= ~CONTROL_REQ;
		DATA_PORT = data;

		data = *lum_buffer;
		lum_buffer++;
		CONTROL_PORT |= CONTROL_REQ;
		DATA_PORT = data;

		data = *lum_buffer;
		lum_buffer++;
		CONTROL_PORT &= ~CONTROL_REQ;
		DATA_PORT = data;

		data = *lum_buffer;
		lum_buffer++;
		CONTROL_PORT |= CONTROL_REQ;
		DATA_PORT = data;

		data = *lum_buffer;
		lum_buffer++;
		CONTROL_PORT &= ~CONTROL_REQ;
		DATA_PORT = data;

		data = *lum_buffer;
		lum_buffer++;
		CONTROL_PORT |= CONTROL_REQ;
		DATA_PORT = data;

		data = *lum_buffer;
		lum_buffer++;
		CONTROL_PORT &= ~CONTROL_REQ;
		DATA_PORT = data;

		block--;
	}

	lum_buffer = &LEDTAFEL_screenlum[ line * LEDTAFEL_ROW ];
	block = LEDTAFEL_ROW;

	while( block )
	{
		data = *lum_buffer;
		lum_buffer++;
		CONTROL_PORT |= CONTROL_REQ;
		DATA_PORT = data;

		data = *lum_buffer;
		lum_buffer++;
		CONTROL_PORT &= ~CONTROL_REQ;
		DATA_PORT = data;

		data = *lum_buffer;
		lum_buffer++;
		CONTROL_PORT |= CONTROL_REQ;
		DATA_PORT = data;

		data = *lum_buffer;
		lum_buffer++;
		CONTROL_PORT &= ~CONTROL_REQ;
		DATA_PORT = data;

		data = *lum_buffer;
		lum_buffer++;
		CONTROL_PORT |= CONTROL_REQ;
		DATA_PORT = data;

		data = *lum_buffer;
		lum_buffer++;
		CONTROL_PORT &= ~CONTROL_REQ;
		DATA_PORT = data;

		data = *lum_buffer;
		lum_buffer++;
		CONTROL_PORT |= CONTROL_REQ;
		DATA_PORT = data;

		data = *lum_buffer;
		lum_buffer++;
		CONTROL_PORT &= ~CONTROL_REQ;
		DATA_PORT = data;

		block--;
	}

	LED_off(1);

	return( TAFELSENDOK );
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! Sendet einen kompletten Befehl + Daten (zusammen 16 Bit) und lässt ihm von aktuellen Maxim übernehmen
 * \param 	command		Der Befehl der gesendet werden soll
 * \param 	value		Die Daten die zum Befehl gehören
 * \retval  void
 */
/* -----------------------------------------------------------------------------------------------------------*/
void LEDTAFEL_process_cmd( struct TAFEL_CMD_HEADER * CMD )
{	
	switch( CMD->TAFEL_CMD )
	{
		case	write_raw:		LEDTAFEL_copy_screen( CMD->TAFEL_DATA, CMD->TAFEL_X, CMD->TAFEL_Y, CMD->TAFEL_W, CMD->TAFEL_H );
								CMD->TAFEL_CMD = ack;
								break;
		case	write_lum_raw:	LEDTAFEL_copy_screenlum( CMD->TAFEL_DATA, CMD->TAFEL_X, CMD->TAFEL_Y, CMD->TAFEL_W, CMD->TAFEL_H );
								CMD->TAFEL_CMD = ack;
								break;
		case	intensity:		LEDTAFEL_set_intensity( CMD->TAFEL_DATA[0] );
								CMD->TAFEL_CMD = ack;
								break;
		case	clear:			LEDTAFEL_set_clearscreen();
								CMD->TAFEL_CMD = ack;
								break;
		case	hardreset:		CMD->TAFEL_CMD = ack;	// trotzdem nen ACK senden!
								softreset();
								while(1);
		case	fade_out:		LEDTAFEL_fadeout_display( (int) CMD->TAFEL_DATA[0] , LEDTAFEL_screenlum );
								CMD->TAFEL_CMD = ack;
								break;
		case	graphic_direct:	LEDTAFEL_graphic_direct( CMD->TAFEL_DATA, CMD->TAFEL_X,CMD->TAFEL_Y, CMD->TAFEL_W );
								CMD->TAFEL_CMD = ack;
								break;
		default:				CMD->TAFEL_CMD = nak;
	}
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! Updatet den Screenbuffer des kompletten Displays
 * \param	buffer	Pointer auf den Puffer der kopiert werden soll.
 * \param	xpos	X-Position des Bildschirmes in die kopiert werden soll.
 * \param	ypos	Y-Position des Bildschirmes in die kopiert werden soll.
 * \param	xsize	X-Groesse des Puffers.
 * \param	ysize	Y-Groesse des Puffers.
 * \retval  void
 */
/* -----------------------------------------------------------------------------------------------------------*/
void LEDTAFEL_copy_screen( char * buffer, int xpos, int ypos, int xsize, int ysize )
{
	unsigned short i = 0;

	if (xpos+xsize > LEDTAFEL_ROW || ypos+ysize > LEDTAFEL_LINES)
		return;

	for (uint8_t line = ypos ; line < ( ysize + ypos ) ; line++ )
	{
		for (uint8_t row = xpos ; row < ( xsize + xpos ); row++ )
		{
			unsigned short position_char = ( LEDTAFEL_ROW * line ) + row;
			unsigned short position_gr = position_char * 8;

			uint8_t Zeichen = buffer[ i++ ];
			
			for (int8_t matrixrow = 7; matrixrow >= 0 ; matrixrow-- )
			{
				uint8_t data = font[ Zeichen ][ matrixrow ];

				LEDTAFEL_screen_graphic[ position_gr++ ] = data;
			}
		}
	}
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! Kopiert die Helligkeit in einen bestimmten Bereich des Bildschirmpuffers
 * \param	buffer	Pointer auf den Puffer der kopiert werden soll.
 * \param	xpos	X-Position des Bildschirmes in die kopiert werden soll.
 * \param	ypos	Y-Position des Bildschirmes in die kopiert werden soll.
 * \param	xsize	X-Groesse des Puffers.
 * \param	ysize	Y-Groesse des Puffers.
 * \retval  void
 */
/* -----------------------------------------------------------------------------------------------------------*/
void LEDTAFEL_copy_screenlum( char * buffer, int xpos, int ypos, int xsize, int ysize )
{
	unsigned short i = 0;

	if (xpos+xsize > LEDTAFEL_ROW || ypos+ysize > LEDTAFEL_LINES)
		return;

	for (uint8_t line = ypos ; line < ( ysize + ypos ) && line < LEDTAFEL_LINES ; line++ )
	{
		for (uint8_t row = xpos ; row < ( xsize + xpos ) ; row++ )
		{
			if ( row < LEDTAFEL_ROW )
			{
				uint8_t data = buffer[ i ];
				unsigned short position_char = ( LEDTAFEL_ROW * line ) + row;
				LEDTAFEL_screenlum[ position_char ] = data;
			}
			i++;
		}
	}
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! Setzt die Helligkeit fuer den ganzen Bildschirm.
 * \param	value	Helligkeit auf die der Bildschirm gesetzt werden soll.
 * \retval  void
 */
/* -----------------------------------------------------------------------------------------------------------*/
void LEDTAFEL_set_intensity( char value )
{
	memset ( LEDTAFEL_screenlum , value , LEDTAFEL_ROW * LEDTAFEL_LINES );
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! Loescht den gesamten Bildschirm
 * \retval  void
 */
/* -----------------------------------------------------------------------------------------------------------*/
void LEDTAFEL_set_clearscreen( void )
{
	memset ( LEDTAFEL_screen_graphic , 0x00 , LEDTAFEL_ROW * LEDTAFEL_LINES * 8);
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! Kopiert den Puffer in einen bestimmten Teil des Bildschirmes
 * \param	buffer	Pointer auf den Puffer der kopiert werden soll
 * \param	start	Startposition im Bildschirmspeicher
 * \param	length	Laenge des Puffers der kopiert werden soll
 * \param	update	Not use.
 * \retval  void
 */
/* -----------------------------------------------------------------------------------------------------------*/
void LEDTAFEL_graphic_direct( char * buffer, unsigned short start, unsigned short length, unsigned short update)
{
	if (length > 1450)
		length = 1450;

	if (start >= ( ( LEDTAFEL_LINES * LEDTAFEL_ROW * 8 ) - 1 ) )
		start = ( LEDTAFEL_LINES * LEDTAFEL_ROW * 8 ) - 1 ;

	if (start + length > ( LEDTAFEL_LINES * LEDTAFEL_ROW * 8 ))
		length = ( LEDTAFEL_LINES * LEDTAFEL_ROW * 8 ) - start;

	memcpy( &LEDTAFEL_screen_graphic[ start ], buffer, length ); 
	
/*
	 for (unsigned short offset = start; offset < start+length;offset++)
	{
		uint8_t data = *buffer++;

//		asm( "mov __tmp_reg__, %0 \n\t"
//			"ror __tmp_reg__ \n\t"
//			"ror %0 \n\t" : "+r"(data) : : "cc");

		LEDTAFEL_screen_graphic[offset] = data;
	}
*/
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! Gibt den Chaosknoten aus und fade in rein
 * \param 	void
 * \retval  void
 */
/* -----------------------------------------------------------------------------------------------------------*/
void LEDTAFEL_printlogo( void )
{
	LEDTAFEL_copy_screen(knoten,0,0,LEDTAFEL_ROW,LEDTAFEL_LINES);

	for(uint8_t y = 1 ; y < 16 ; y++  )
	{
		memset ( LEDTAFEL_screenlum , y , LEDTAFEL_ROW * LEDTAFEL_LINES );
	}

	for(uint8_t y = 15 ; y > 4 ; y--  )
	{
		memset ( LEDTAFEL_screenlum , y , LEDTAFEL_ROW * LEDTAFEL_LINES );
	}
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! Updatet die Helligkeit des komplettes Display
 * \retval  void
 */
/* -----------------------------------------------------------------------------------------------------------*/
void LEDTAFEL_fadeout_display( int loop, char * lumbuffer )
{
	int i;
	char lum;

	if ( lumbuffer == NULL )
		lumbuffer = &LEDTAFEL_screenlum[0];

	for( ; loop > 0 ; loop-- )
	{
		for ( i = 0 ; i < ( LEDTAFEL_LINES * LEDTAFEL_ROW ) ; i++ )
		{
			lum = LEDTAFEL_screenlum[ i - 1 ];
			lum = lum + LEDTAFEL_screenlum[ i ];
			lum = lum + LEDTAFEL_screenlum[ i + 1 ];
			lum = lum + LEDTAFEL_screenlum[ i + LEDTAFEL_ROW ];
			lum = lum + LEDTAFEL_screenlum[ i - LEDTAFEL_ROW ];
			lum = lum / 5 ;
			LEDTAFEL_screenlum[ i ] = lum;
		}
	}
}

#endif
