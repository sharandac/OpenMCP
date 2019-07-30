/**
 *            led_tafel.c
 *
 *  Sun Jun  6 16:36:51 2010
 *  Copyright  2010  Dirk Broßwick
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
#include <util/delay.h>
#include <stdio.h>
#include <string.h>

#include "config.h"

#if defined(LEDTAFEL)

#include "system/softreset/softreset.h"
#include "led_tafel.h"
#include "font.h"

static char LEDTAFEL_screen[ LEDTAFEL_ROW * LEDTAFEL_LINES ];
static char LEDTAFEL_screen_graphic_buffer[ LEDTAFEL_ROW * LEDTAFEL_LINES * 8];
static char LEDTAFEL_screen_graphic[ LEDTAFEL_ROW * LEDTAFEL_LINES * 8];
static char LEDTAFEL_screenlum[ LEDTAFEL_ROW * LEDTAFEL_LINES ];
static char LEDTAFEL_shutdown[ LEDTAFEL_ROW * LEDTAFEL_LINES ];
static char LEDTAFEL_row = 0;
static char LEDTAFEL_line = 0;
static char LEDTAFEL_lum = 0;

void LEDTAFEL_cmd_maxim( char command, char value);
void LEDTAFEL_cmd_maxim_rotate( char command, char value);

void SPI2_WriteValue4( unsigned char value );
void SPI2_WriteValue8( unsigned char value );
void SPI2_WriteValue8_rotate( unsigned char value );

char knoten[] = {
	"  ,cCCCCCCCCCCCCCCCCCCCCCc,                             "
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
int LEDTAFEL_init (void)
{
	DDRB |= _BV(PB2) | _BV(PB1);	// SPI with bitbanging

	// Datenrichtung der Ausgänge setzen
	MAXIM_DESELECT_DDR |= ( 1 << MAXIM_DESELECT );	// select
	MAXIM_SELECT_DDR |= ( 1 << MAXIM_SELECT );	// deselect
	MAXIM_LATCHCONTROL_DDR |= ( 1 << MAXIM_LATCHCONTROL );	// latchcontroll für select/deselect
	SERIALLINE_CLOCK_DDR |= ( 1<< SERIALLINE_CLOCK ); // shifter zum selektieren der maxims
	LINECONTROL_DDR = 0xFF; // Linecontrol
	
	// Ausgänge auf Ausgangszustand setzen
	MAXIM_DESELECT_PORT |= ( 1 << MAXIM_DESELECT );	// select
	MAXIM_SELECT_PORT |= ( 1 << MAXIM_SELECT );	// deselect
	MAXIM_LATCHCONTROL_PORT |= ( 1 << MAXIM_LATCHCONTROL );	// latchcontrol für de/-select
	SERIALLINE_CLOCK_PORT |= ( 1<< SERIALLINE_CLOCK ); // shifter zum selektieren der maxims
	LINECONTROL = 0; // Linecontrol

	//	display_test_with_pattern(0x00);	// einmal alles loeschen
	LEDTAFEL_initialize_display();

	return(0);
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! Selektiert und deselektiert den Maxim an der aktuellen Position so das er die letzten 16 Bit übernimmt
 * \param 	void
 * \retval  void
 */
/* -----------------------------------------------------------------------------------------------------------*/
void LEDTAFEL_maxim_setdata( void )
{
	LEDTAFEL_maxim_select();
	LEDTAFEL_maxim_deselect();
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! Deselektiert den Maxim an der aktuellen Position
 * \param 	void
 * \retval  void
 */
/* -----------------------------------------------------------------------------------------------------------*/
void LEDTAFEL_maxim_deselect( void )
{
	MAXIM_DESELECT_PORT ^= ( 1 << MAXIM_DESELECT );	// latch clear
	MAXIM_LATCHCONTROL_PORT ^= ( 1 << MAXIM_LATCHCONTROL );	// latch1 clock
	MAXIM_LATCHCONTROL_PORT ^= ( 1 << MAXIM_LATCHCONTROL );	// latch1 clock
//	_delay_us( SETUPTIME );
	MAXIM_DESELECT_PORT ^=  ( 1 << MAXIM_DESELECT );	// latch clear
	MAXIM_LATCHCONTROL_PORT ^= ( 1 << MAXIM_LATCHCONTROL );	// latch1 clock
	MAXIM_LATCHCONTROL_PORT ^= ( 1 << MAXIM_LATCHCONTROL );	// latch1 clock
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! Selektiert den Maxim an der aktuellen Position
 * \param 	void
 * \retval  void
 */
/* -----------------------------------------------------------------------------------------------------------*/
void LEDTAFEL_maxim_select( void )
{
	MAXIM_SELECT_PORT ^= ( 1 << MAXIM_SELECT );	// latch clock
	MAXIM_LATCHCONTROL_PORT ^= ( 1 << MAXIM_LATCHCONTROL );	// latch1 clock
	MAXIM_LATCHCONTROL_PORT ^= ( 1 << MAXIM_LATCHCONTROL );	// latch1 clock

	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("nop");

	asm volatile ("nop");	// deutlich fehler mit 4 nops
	asm volatile ("nop");	// mit 5 nops kommen minimal fehler	
	asm volatile ("nop");	// mit 6 nops gehts eigentlich sehr gut

	// aber erst mit sieben nops ist es wohl perfekt

	MAXIM_SELECT_PORT ^=  ( 1 << MAXIM_SELECT );	// latch clock
	MAXIM_LATCHCONTROL_PORT ^= ( 1 << MAXIM_LATCHCONTROL );	// latch1 clock
	MAXIM_LATCHCONTROL_PORT ^= ( 1 << MAXIM_LATCHCONTROL );	// latch1 clock
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! Shiftet das selektionsbit um ein in der Zeile weiter
 * \param 	void
 * \retval  void
 */
/* -----------------------------------------------------------------------------------------------------------*/
void LEDTAFEL_serialclock_toggle( void )
{
	SERIALLINE_CLOCK_PORT ^= ( 1 << SERIALLINE_CLOCK );	// serialshifter clock
	_delay_us( SETUPTIME );
	SERIALLINE_CLOCK_PORT ^= ( 1 << SERIALLINE_CLOCK );	// serialshifter clock
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! Initialisiert den aktuell selektieren Maxim
 * \param 	void
 * \retval  void
 */
/* -----------------------------------------------------------------------------------------------------------*/
void LEDTAFEL_init_maxim( void )
{
	LEDTAFEL_cmd_maxim( MAXIM_CMD_TESTMODE , 0x00 );
	LEDTAFEL_cmd_maxim( MAXIM_CMD_SCANLIMIT , 0x07 );
	LEDTAFEL_cmd_maxim( MAXIM_CMD_SHUTDOWN , 0x01 );
	LEDTAFEL_cmd_maxim( MAXIM_CMD_DECODE , 0x00 );
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! Selektiert einen bestimmt Maxim
 * \param 	line		Zeile
 * \param 	row			Spalte
 * \retval  void
 */
/* -----------------------------------------------------------------------------------------------------------*/
void LEDTAFEL_selectMatrix( signed char line, signed char row )
{
	uint8_t i;

	for (i = 0;i < (LEDTAFEL_ROW-row);i++)	// shift register leermachen
		LEDTAFEL_serialclock_toggle();

	LINECONTROL = line + 1;	// serial data 1
	LEDTAFEL_serialclock_toggle();
	LINECONTROL = 0;	// serial data 0

	for ( ; row > 0 ; row-- )
	{
		LEDTAFEL_serialclock_toggle();
	}
}

// fast assumes that the last or no maxim is selected!

void LEDTAFEL_selectMatrix_fast( signed char line, signed char row )
{
	LINECONTROL = line + 1;	// serial data 1
	LEDTAFEL_serialclock_toggle();
	LINECONTROL = 0;	// serial data 0

	for ( ; row > 0 ; row-- )
	{
		LEDTAFEL_serialclock_toggle();
	}
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! Schreibt an die aktuelle Position ein Pattern
 * \param 	pattern		Das Füllmuster
 * \retval  void
 */
/* -----------------------------------------------------------------------------------------------------------*/
void LEDTAFEL_fill_with_pattern(unsigned char pattern)
{
	for (signed char row = 8;row>=1;row--)	// shift register leermachen
	{
		LEDTAFEL_cmd_maxim_rotate( row, pattern );
	}
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! Schreibt an die aktuelle Position ein Zeichen
 * \param 	Zeichen		Das Zeichen das ausgegeben werden soll
 * \retval  void
 */
/* -----------------------------------------------------------------------------------------------------------*/
void LEDTAFEL_print_char( unsigned char Zeichen )
{
	for (signed char row = 7; row >= 0 ; row-- )	// shift register leermachen
	{
		uint8_t blub = font[Zeichen][row];
		LEDTAFEL_cmd_maxim_rotate( row + 1, blub );
	}
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! Schreibt ein Zeichen unter Berücksichtigung der aktuellen Position in das Display und rückt um ein weiter
 * \param 	Zeichen		Das Zeichen das ausgegeben werden soll
 * \retval  void
 */
/* -----------------------------------------------------------------------------------------------------------*/
void LEDTAFEL_print_char_stdio( unsigned char Zeichen )
{
	if ( Zeichen == '\r' )
	{
		LEDTAFEL_row = 0;
		LEDTAFEL_selectMatrix( LEDTAFEL_line , LEDTAFEL_row );
		return;
	}

	if ( Zeichen == '\n' )
	{
		LEDTAFEL_line++;
		if ( LEDTAFEL_line < LEDTAFEL_LINES )
		{
			LEDTAFEL_selectMatrix( LEDTAFEL_line , LEDTAFEL_row );
		}
		else
		{
			LEDTAFEL_line--;
			LEDTAFEL_selectMatrix( LEDTAFEL_line , 0 );
			LEDTAFEL_scrollup_display();
		}
		return;
	}

	if ( LEDTAFEL_row >= LEDTAFEL_ROW )
	{
		LEDTAFEL_row = 0;
		LEDTAFEL_line++;

		if ( LEDTAFEL_line < LEDTAFEL_LINES )
		{
			LEDTAFEL_selectMatrix( LEDTAFEL_line , LEDTAFEL_row );
		}
		else
		{
			LEDTAFEL_line--;
			LEDTAFEL_selectMatrix( LEDTAFEL_line , LEDTAFEL_row );
			LEDTAFEL_scrollup_display();
		}
	}

	unsigned short position_char = ( LEDTAFEL_line * LEDTAFEL_ROW ) + LEDTAFEL_row;

	LEDTAFEL_print_char( Zeichen );

	if (Zeichen == 0x20)
	{
		if (LEDTAFEL_shutdown[position_char] != 0x00)
		{
			LEDTAFEL_shutdown[position_char] = 0x00;
			LEDTAFEL_cmd_maxim( MAXIM_CMD_SHUTDOWN , 0x00 );	// shutdown mode on
		}
	}
	else
	{
		if (LEDTAFEL_shutdown[position_char] != 0x01)
		{
			LEDTAFEL_shutdown[position_char] = 0x01;
			LEDTAFEL_cmd_maxim( MAXIM_CMD_SHUTDOWN , 0x01 );	// shutdown mode off
		}
	}

	LEDTAFEL_screen[ position_char ] = Zeichen;
	LEDTAFEL_screenlum[ position_char ] = LEDTAFEL_lum;

	LEDTAFEL_serialclock_toggle();
	LEDTAFEL_row++;
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! Initialisiert das komplette Display
 * \retval  void
 */
/* -----------------------------------------------------------------------------------------------------------*/
void LEDTAFEL_initialize_display( void )
{
	LEDTAFEL_maxim_deselect();

	// Alle Maxims einmal durchgehen und initialisieren
	for (uint8_t line = 0 ; line < LEDTAFEL_LINES ; line++ )
	{
		LEDTAFEL_selectMatrix( line , 0 );

		for (uint8_t row = 0 ; row < LEDTAFEL_ROW ; row++ )
		{
			LEDTAFEL_init_maxim();
			LEDTAFEL_print_char( ' ' );
			LEDTAFEL_intensity_maxim( LEDTAFEL_lum );
			LEDTAFEL_screen[ ( line * LEDTAFEL_ROW ) + row ] = ' ';
			LEDTAFEL_screenlum[ ( line * LEDTAFEL_ROW ) + row ] = LEDTAFEL_lum;
			LEDTAFEL_serialclock_toggle();
		}
	}

	// ersten Maxim links oben selektieren und Position speichern
	LEDTAFEL_selectMatrix( 0 , 0 );
	LEDTAFEL_row = 0;
	LEDTAFEL_line = 0;
	LEDTAFEL_lum = 7;	// starthelligkeit
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! Scrollt den Displayinhalt um eins nach oben und aktualisiert das Display
 * \param 	command
 * \param 	command
 * \retval  void
 */
/* -----------------------------------------------------------------------------------------------------------*/
void LEDTAFEL_scrollup_display( void )
{
	unsigned short i;

	for ( i = 0 ; i < LEDTAFEL_ROW * ( LEDTAFEL_LINES - 1 ) ; i ++ )
	{
		LEDTAFEL_screen[ i ] = LEDTAFEL_screen[ i + LEDTAFEL_ROW ];
		LEDTAFEL_screenlum[ i ] = LEDTAFEL_screenlum[ i + LEDTAFEL_ROW ];
	}

	for ( i = 0 ; i < LEDTAFEL_ROW ; i ++ )
	{
		LEDTAFEL_screen[ ( LEDTAFEL_LINES - 1 ) * LEDTAFEL_ROW + i ] = ' ';
		LEDTAFEL_screenlum[ ( LEDTAFEL_LINES - 1 ) * LEDTAFEL_ROW + i ] = LEDTAFEL_lum;
	}

	LEDTAFEL_fullupdate_display();
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! Updatet das komplette Display
 * \retval  void
 */
/* -----------------------------------------------------------------------------------------------------------*/
void LEDTAFEL_fullupdate_display( void )
{
	for (uint8_t line = 0 ; line < LEDTAFEL_LINES ; line++ )
	{
		LEDTAFEL_selectMatrix( line , 0 );

		for (uint8_t row = 0 ; row < LEDTAFEL_ROW ; row++ )
		{
			LEDTAFEL_print_char( LEDTAFEL_screen[ ( ( line ) * LEDTAFEL_ROW ) + row ] );
			LEDTAFEL_intensity_maxim( LEDTAFEL_screenlum[ ( ( line ) * LEDTAFEL_ROW ) + row ] );
			LEDTAFEL_serialclock_toggle();
		}
	}
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! Updatet das komplette Display
 * \retval  void
 */
/* -----------------------------------------------------------------------------------------------------------*/
void LEDTAFEL_graphic_direct( char * buffer, unsigned short start, unsigned short length, unsigned short update)
{
	if (length > 1450)
		length = 1450;

	if (start >= (20*56*8)-1)
		start = (20*56*8)-1;

	if (start + length > (20*56*8))
		length = (20*56*8) - start;

	for (unsigned short offset = start; offset < start+length;offset++)
	{
		uint8_t data = *buffer++;

//		asm( "mov __tmp_reg__, %0 \n\t"
//			"ror __tmp_reg__ \n\t"
//			"ror %0 \n\t" : "+r"(data) : : "cc");

		LEDTAFEL_screen_graphic[offset] = data;
	}

	if (update == 2342)
		LEDTAFEL_graphic_update();
	else if(update == 4223)
		LEDTAFEL_graphic_update_nobuffering2();
}

void LEDTAFEL_graphic_update()
{
	static unsigned char refresh = 0;

	unsigned short offset = 0;
	unsigned short position_char = 0;

	unsigned char i;
	for (i = 0;i < LEDTAFEL_ROW;i++)	// shift register leermachen
		LEDTAFEL_serialclock_toggle();

	for (uint8_t line = 0; line < LEDTAFEL_LINES; line++ )
	{
		LEDTAFEL_selectMatrix_fast( line ,0 );

		for (uint8_t row = 0 ; row < LEDTAFEL_ROW ; row++ )
		{
			uint8_t content = 0;

			for (uint8_t matrixrow = 8; matrixrow >= 1 ; matrixrow-- )
			{
				uint8_t data = LEDTAFEL_screen_graphic[offset];

				if (LEDTAFEL_screen_graphic_buffer[offset] != data)
				{
					LEDTAFEL_screen_graphic_buffer[offset] = data;
					LEDTAFEL_cmd_maxim_rotate( matrixrow, data );
				}

				content |= data;	// hatten wir inhalt?

				offset++;
			}

			content = !!content;	// bool erstellen

			if (content != LEDTAFEL_shutdown[position_char])
			{
				LEDTAFEL_shutdown[position_char] = content;
				LEDTAFEL_cmd_maxim( MAXIM_CMD_SHUTDOWN , content );
			}

			LEDTAFEL_serialclock_toggle();

			position_char++;
		}
	}

	refresh++;

	if (refresh == 0)
	{
		LEDTAFEL_set_reinitialize();
		refresh = 192;
	}
}

void LEDTAFEL_graphic_update_nobuffering2()
{
	unsigned short offset = 0;

	unsigned short position_char = 0;

	for (uint8_t line = 0; line < LEDTAFEL_LINES; line++ )
	{
		LEDTAFEL_selectMatrix( line ,0 );

		for (uint8_t row = 0 ; row < LEDTAFEL_ROW ; row++ )
		{
			uint8_t content = 0;

			for (uint8_t matrixrow = 8; matrixrow >= 1 ; matrixrow-- )
			{
				uint8_t data = LEDTAFEL_screen_graphic[offset];
				LEDTAFEL_screen_graphic_buffer[offset] = data;
				LEDTAFEL_cmd_maxim_rotate( matrixrow, data );

				content |= data;	// hatten wir inhalt?

				offset++;
			}

			content = !!content;	// bool erstellen

			if (content != LEDTAFEL_shutdown[position_char])
			{
				LEDTAFEL_shutdown[position_char] = content;
				LEDTAFEL_cmd_maxim( MAXIM_CMD_SHUTDOWN , content );
			}

			LEDTAFEL_serialclock_toggle();

			position_char++;
		}
	}
}

void LEDTAFEL_graphic_update_nobuffering()
{
	unsigned short offset = 0;

	for (uint8_t line = 0; line < LEDTAFEL_LINES; line++ )
	{
		LEDTAFEL_selectMatrix( line ,0 );

		for (uint8_t row = 0 ; row < LEDTAFEL_ROW ; row++ )
		{
			uint8_t content = 0;

			for (uint8_t matrixrow = 8; matrixrow >= 1 ; matrixrow-- )
			{
				uint8_t data = LEDTAFEL_screen_graphic_buffer[offset];
				LEDTAFEL_cmd_maxim_rotate( matrixrow, data );

				content |= data;	// hatten wir inhalt?

				offset++;
			}

			content = !!content;	// bool erstellen
			LEDTAFEL_cmd_maxim( MAXIM_CMD_SHUTDOWN , content );

			LEDTAFEL_serialclock_toggle();
		}
	}
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! Updatet den Screenbuffer des kompletten Displays
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
		LEDTAFEL_selectMatrix( line , xpos );

		for (uint8_t row = xpos ; row < ( xsize + xpos ) ; row++ )
		{
			if ( row < LEDTAFEL_ROW )
			{
				uint8_t data = buffer[ i ];

				unsigned short position_char = ( LEDTAFEL_ROW * line ) + row;

				unsigned char oldlum = LEDTAFEL_screenlum[ position_char ];

				if ( oldlum != data )
				{
					LEDTAFEL_screenlum[ position_char ] = data;
					LEDTAFEL_intensity_maxim2( data , oldlum);
				}
			}

			LEDTAFEL_serialclock_toggle();
			i++;
		}
	}
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! Updatet den Screenbuffer des kompletten Displays
 * \retval  void
 */
/* -----------------------------------------------------------------------------------------------------------*/
void LEDTAFEL_copy_screen( char * buffer, int xpos, int ypos, int xsize, int ysize )
{
	unsigned short i = 0;

	static unsigned char refresh = 0;

	if (xpos+xsize > LEDTAFEL_ROW || ypos+ysize > LEDTAFEL_LINES)
		return;

	for (uint8_t line = ypos ; line < ( ysize + ypos ) ; line++ )
	{
		LEDTAFEL_selectMatrix( line , xpos );

		for (uint8_t row = xpos ; row < ( xsize + xpos ); row++ )
		{
			unsigned short position_char = ( LEDTAFEL_ROW * line ) + row;
			unsigned short position_gr = position_char * 8;

			uint8_t Zeichen = buffer[ i++ ];

			uint8_t content = 0;

			for (int8_t matrixrow = 7; matrixrow >= 0 ; matrixrow-- )
			{
				uint8_t data = font[Zeichen][matrixrow];

				if (LEDTAFEL_screen_graphic_buffer[position_gr] != data)
				{
					LEDTAFEL_screen_graphic_buffer[position_gr] = data;
					LEDTAFEL_screen_graphic[position_gr] = data;
					LEDTAFEL_cmd_maxim_rotate( matrixrow + 1, data );
				}

				position_gr++;
				content |= data;
			}

			content = !!content;	// bool erstellen

			if (content != LEDTAFEL_shutdown[position_char])
			{
				LEDTAFEL_shutdown[position_char] = content;
				LEDTAFEL_cmd_maxim( MAXIM_CMD_SHUTDOWN , content );
			}

			LEDTAFEL_serialclock_toggle();
		}
	}

	refresh++;

	if (refresh == 0)
		LEDTAFEL_set_reinitialize();
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! Updatet die Helligkeit des komplettes Display
 * \retval  void
 */
/* -----------------------------------------------------------------------------------------------------------*/
void LEDTAFEL_lumupdate_display( void )
{
	unsigned char i;

	for (i = 0;i < LEDTAFEL_ROW;i++)	// shift register leermachen
		LEDTAFEL_serialclock_toggle();

	for (uint8_t line = 0 ; line < LEDTAFEL_LINES ; line++ )
	{
		LEDTAFEL_selectMatrix_fast( line, 0 );

		for (uint8_t row = 0 ; row < LEDTAFEL_ROW ; row++ )
		{
			unsigned short position_char = ( line * LEDTAFEL_ROW ) + row;

			uint8_t lum = LEDTAFEL_screenlum[ position_char ];

			if (lum == 0)
			{
				if (LEDTAFEL_shutdown[position_char] != 0x00)
				{
					LEDTAFEL_shutdown[position_char] = 0x00;
					LEDTAFEL_cmd_maxim( MAXIM_CMD_SHUTDOWN , 0x00 );	// shutdown mode on
				}
			}
			else
			{
				if (LEDTAFEL_shutdown[position_char] != 0x01)
				{
					LEDTAFEL_shutdown[position_char] = 0x01;
					LEDTAFEL_cmd_maxim( MAXIM_CMD_SHUTDOWN , 0x01 );	// shutdown mode off
				}
			}

			LEDTAFEL_cmd_maxim( MAXIM_CMD_INTENSITY , lum - 1 );

			LEDTAFEL_serialclock_toggle();
		}
	}
}

void LEDTAFEL_lumupdate_display_forlogo( void )
{
	LEDTAFEL_selectMatrix( 0 , 0 );

	for (uint8_t line = 0 ; line < LEDTAFEL_LINES ; line++ )
	{
		for (uint8_t row = 0 ; row < LEDTAFEL_ROW ; row++ )
		{
			unsigned short position_char = ( line * LEDTAFEL_ROW ) + row;

			uint8_t lum = LEDTAFEL_screenlum[ position_char ];

			if (LEDTAFEL_shutdown[position_char] != 0x00)
			{
				LEDTAFEL_cmd_maxim( MAXIM_CMD_INTENSITY , lum - 1 );
			}

			LEDTAFEL_serialclock_toggle();
		}

		LEDTAFEL_selectMatrix( line + 1 , 0 );
	}
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! Updatet die Helligkeit des komplettes Display
 * \retval  void
 */
/* -----------------------------------------------------------------------------------------------------------*/
void LEDTAFEL_fadeout_display( int loop, char * lumbuffer )
{
	unsigned short i;
	uint8_t lum;

	if ( lumbuffer == NULL )
		lumbuffer = &LEDTAFEL_screenlum[0];

	for( ; loop > 0 ; loop-- )
	{
		for ( i = 0 ; i < ( LEDTAFEL_LINES * LEDTAFEL_ROW ) ; i++ )
		{
			lum = lum + LEDTAFEL_getlum( i - 1 , lumbuffer);
			lum = LEDTAFEL_getlum( i , lumbuffer );
			lum = lum + LEDTAFEL_getlum( i + 1 , lumbuffer);

//			lum = lum + LEDTAFEL_getlum( i + LEDTAFEL_ROW - 1 , lumbuffer);
			lum = lum + LEDTAFEL_getlum( i + LEDTAFEL_ROW , lumbuffer);
//			lum = lum + LEDTAFEL_getlum( i + LEDTAFEL_ROW + 1 , lumbuffer);

//			lum = lum + LEDTAFEL_getlum( i - LEDTAFEL_ROW - 1 , lumbuffer);
			lum = lum + LEDTAFEL_getlum( i - LEDTAFEL_ROW , lumbuffer);
//			lum = lum + LEDTAFEL_getlum( i - LEDTAFEL_ROW + 1 , lumbuffer);

			lum = lum / 5 ;
			LEDTAFEL_screenlum[ i ] = lum;
		}
		LEDTAFEL_lumupdate_display();
	}
}

char LEDTAFEL_getRow( void )
{
	return( LEDTAFEL_row );
}

char LEDTAFEL_getLine( void )
{
	return( LEDTAFEL_line );
}


/* -----------------------------------------------------------------------------------------------------------*/
/*! Updatet die Helligkeit des komplettes Display
 * \retval  void
 */
/* -----------------------------------------------------------------------------------------------------------*/
char LEDTAFEL_getlum( unsigned short offset , char * lumbuffer )
{
	if ( offset < 0 )
		return( 0 );
	if ( offset > ( LEDTAFEL_LINES * LEDTAFEL_ROW ) )
		return(0);

	return( lumbuffer[ offset ] );
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! Setzt die Helligkeit des aktuellen Maxim (0-16)
 * \param 	value		Helligkeit von 0 bis 16
 * \retval  void
 */
/* -----------------------------------------------------------------------------------------------------------*/
void LEDTAFEL_intensity_maxim( char value)
{
	if ( value == 0 )
	{
		LEDTAFEL_cmd_maxim( MAXIM_CMD_SHUTDOWN , 0x00 );
	}
	else
	{
		LEDTAFEL_cmd_maxim( MAXIM_CMD_INTENSITY , value - 1 );
		LEDTAFEL_cmd_maxim( MAXIM_CMD_SHUTDOWN , 0x01 );
	}
}

void LEDTAFEL_intensity_maxim2( char value, unsigned char oldlum)
{
	LEDTAFEL_cmd_maxim( MAXIM_CMD_INTENSITY , value - 1 );

	if ( oldlum == 0)
	{
		LEDTAFEL_cmd_maxim( MAXIM_CMD_SHUTDOWN , 0x01 );
	}
	else if (value == 0)
	{
		LEDTAFEL_cmd_maxim( MAXIM_CMD_SHUTDOWN , 0x00 );
	}
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! Gibt den Chaosknoten aus und fade in rein
 * \param 	void
 * \retval  void
 */
/* -----------------------------------------------------------------------------------------------------------*/
void LEDTAFEL_printlogo( void )
{
	LEDTAFEL_set_clearscreen();
	LEDTAFEL_copy_screen(knoten,0,0,LEDTAFEL_ROW,LEDTAFEL_LINES);

	for(uint8_t y = 1 ; y < 16 ; y++  )
	{
		memset ( LEDTAFEL_screenlum , y , LEDTAFEL_ROW * LEDTAFEL_LINES );
		LEDTAFEL_lumupdate_display_forlogo();
	}

	for(uint8_t y = 15 ; y > 4 ; y--  )
	{
		memset ( LEDTAFEL_screenlum , y , LEDTAFEL_ROW * LEDTAFEL_LINES );
		LEDTAFEL_lumupdate_display_forlogo();
	}
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! Sendet einen kompletten Befehl + Daten (zusammen 16 Bit) und lässt ihm von aktuellen Maxim übernehmen
 * \param 	command		Der Befehl der gesendet werden soll
 * \param 	value		Die Daten die zum Befehl gehören
 * \retval  void
 */
/* -----------------------------------------------------------------------------------------------------------*/
void LEDTAFEL_cmd_maxim( char command, char value)
{
	SPI2_WriteValue4( command);
	SPI2_WriteValue8( value);

	LEDTAFEL_maxim_setdata();
}

void LEDTAFEL_cmd_maxim_rotate( char command, char value)
{
        SPI2_WriteValue4( command);
        SPI2_WriteValue8_rotate( value);

        LEDTAFEL_maxim_setdata();
}

void SPI2_WriteValue4( unsigned char value )
{
        char off  = PORTB | ( 1<<PB2 );
        char on = PORTB & ~( 1<<PB2 );

        unsigned char i;

        for( i = 0 ; i < 4 ; i++ )
        {
                // Kieck mal ob MSB 1 ist
                if (value & 0x08)
                        PORTB = off;
                else
                        PORTB = on;

                // SCK auf High setzen
                PORTB |= ( 1<<PB1 );

                value <<= 1;
                asm volatile ("nop");
                asm volatile ("nop");
                asm volatile ("nop");
                asm volatile ("nop");

                // SCK auf Low setzen
                PORTB &= ~( 1<<PB1 );
        }
}

void SPI2_WriteValue8( unsigned char value )
{
	char off  = PORTB | ( 1<<PB2 );
	char on = PORTB & ~( 1<<PB2 );

	unsigned char i;

	for( i = 0 ; i < 8 ; i++ )
	{
		// Kieck mal ob MSB 1 ist
		if (value & 0x80)
			PORTB = off;
		else
			PORTB = on;

		// SCK auf High setzen
		PORTB |= ( 1<<PB1 );

		value <<= 1;
		asm volatile ("nop");
		asm volatile ("nop");
		asm volatile ("nop");
		asm volatile ("nop");

		// SCK auf Low setzen
		PORTB &= ~( 1<<PB1 );
	}
}

void SPI2_WriteValue8_rotate( unsigned char value )
{
        char off  = PORTB | ( 1<<PB2 );
        char on = PORTB & ~( 1<<PB2 );

         // Kieck mal ob MSB 1 ist
         if (value & 0x01)
                PORTB = off;
          else
                PORTB = on;

         PORTB |= ( 1<<PB1 );

         asm volatile ("nop");
         asm volatile ("nop");
         asm volatile ("nop");
         asm volatile ("nop");

         // SCK auf Low setzen
         PORTB &= ~( 1<<PB1 );

         asm volatile ("nop");

         if (value & 0x80)
                PORTB = off;
          else
                PORTB = on;

         // SCK auf High setzen
         PORTB |= ( 1<<PB1 );

         asm volatile ("nop");
         asm volatile ("nop");
         asm volatile ("nop");
         asm volatile ("nop");

         // SCK auf Low setzen
         PORTB &= ~( 1<<PB1 );

         asm volatile ("nop");

         if (value & 0x40)
                PORTB = off;
          else
                PORTB = on;

         // SCK auf High setzen
         PORTB |= ( 1<<PB1 );

         asm volatile ("nop");
         asm volatile ("nop");
         asm volatile ("nop");
         asm volatile ("nop");

         // SCK auf Low setzen
         PORTB &= ~( 1<<PB1 );

         asm volatile ("nop");

         if (value & 0x20)
                PORTB = off;
         else
                PORTB = on;

         // SCK auf High setzen
         PORTB |= ( 1<<PB1 );

         asm volatile ("nop");
         asm volatile ("nop");
         asm volatile ("nop");
         asm volatile ("nop");

         // SCK auf Low setzen
         PORTB &= ~( 1<<PB1 );

         asm volatile ("nop");

         if (value & 0x10)
                PORTB = off;
          else
                PORTB = on;

         // SCK auf High setzen
         PORTB |= ( 1<<PB1 );

         asm volatile ("nop");
         asm volatile ("nop");
         asm volatile ("nop");
         asm volatile ("nop");

         // SCK auf Low setzen
         PORTB &= ~( 1<<PB1 );

         asm volatile ("nop");

         if (value & 0x08)
                PORTB = off;
          else
                PORTB = on;

         // SCK auf High setzen
         PORTB |= ( 1<<PB1 );

         asm volatile ("nop");
         asm volatile ("nop");
         asm volatile ("nop");
         asm volatile ("nop");

         // SCK auf Low setzen
         PORTB &= ~( 1<<PB1 );

         asm volatile ("nop");

         if (value & 0x04)
                PORTB = off;
          else  
              PORTB = on;

         // SCK auf High setzen
         PORTB |= ( 1<<PB1 );

         asm volatile ("nop");
         asm volatile ("nop");
         asm volatile ("nop");
         asm volatile ("nop");

         // SCK auf Low setzen
         PORTB &= ~( 1<<PB1 );

         asm volatile ("nop");

         if (value & 0x02)
                PORTB = off;
          else  
              PORTB = on;

         // SCK auf High setzen
         PORTB |= ( 1<<PB1 );

         asm volatile ("nop");
         asm volatile ("nop");
         asm volatile ("nop");
         asm volatile ("nop");

         // SCK auf Low setzen
         PORTB &= ~( 1<<PB1 );
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
		case	write_std:		LEDTAFEL_selectMatrix( CMD->TAFEL_Y, CMD->TAFEL_X );
								for( unsigned short i = 0 ; CMD->TAFEL_DATA[ i ] != '\0' && i < ( LEDTAFEL_ROW * LEDTAFEL_LINES ) ; i++ )
									LEDTAFEL_print_char_stdio( CMD->TAFEL_DATA[ i ] );
								CMD->TAFEL_CMD = ack;
								break;
		case	write_lum_raw:	LEDTAFEL_copy_screenlum( CMD->TAFEL_DATA, CMD->TAFEL_X, CMD->TAFEL_Y, CMD->TAFEL_W, CMD->TAFEL_H );
								CMD->TAFEL_CMD = ack;
								break;
		case	write_lum_std:	LEDTAFEL_lum = CMD->TAFEL_DATA[ 0 ];
								CMD->TAFEL_CMD = ack;
								break;
		case	read_raw:		for( unsigned short i = 0 ; i < ( LEDTAFEL_ROW * LEDTAFEL_LINES ) ; i++ )
									CMD->TAFEL_DATA[ i ] = LEDTAFEL_screen[i];
								CMD->TAFEL_CMD = ack;
								break;
		case	read_lum_raw:	for( unsigned short i = 0 ; i < ( LEDTAFEL_ROW * LEDTAFEL_LINES ) ; i++ )
									CMD->TAFEL_DATA[ i ] = LEDTAFEL_screenlum[i];
								CMD->TAFEL_CMD = ack;
								break;
		case	intensity:		LEDTAFEL_set_intensity( CMD->TAFEL_DATA[0] );
								CMD->TAFEL_CMD = ack;
								break;
		case	clear:			LEDTAFEL_set_clearscreen();
								CMD->TAFEL_CMD = ack;
								break;
		case	reset:			LEDTAFEL_set_resetscreen();
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
		case	graphic_update:	LEDTAFEL_graphic_update();
								CMD->TAFEL_CMD = ack;
								break;
		default:				CMD->TAFEL_CMD = nak;
	}
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! Sendet einen kompletten Befehl + Daten (zusammen 16 Bit) und lässt ihm von aktuellen Maxim übernehmen
 * \param 	command		Der Befehl der gesendet werden soll
 * \param 	value		Die Daten die zum Befehl gehören
 * \retval  void
 */
/* -----------------------------------------------------------------------------------------------------------*/
void LEDTAFEL_set_command_and_value_for_screen( char command , char value )
{
	uint8_t line;

	for (line = 1;line <= 26;line++)
	{
		LINECONTROL = line;

		uint8_t i;
		for (i=0;i<=7;i++)
		{
			LEDTAFEL_serialclock_toggle();
		}

		LEDTAFEL_cmd_maxim( command , value );
	}

	LEDTAFEL_selectMatrix( 0 , 0 );
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! Sendet einen kompletten Befehl + Daten (zusammen 16 Bit) und lässt ihm von aktuellen Maxim übernehmen
 * \param 	command		Der Befehl der gesendet werden soll
 * \param 	value		Die Daten die zum Befehl gehören
 * \retval  void
 */
/* -----------------------------------------------------------------------------------------------------------*/
void LEDTAFEL_set_pattern_for_screen_fast( unsigned char value )
{
	for (uint8_t line = 1;line <= 22;line++)
	{
		LINECONTROL = line;

		uint8_t i;
		for (i=0;i<=19;i++)
		{
			LEDTAFEL_serialclock_toggle();
		}

		LEDTAFEL_fill_with_pattern( value );
	}
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! Sendet einen kompletten Befehl + Daten (zusammen 16 Bit) und lässt ihm von aktuellen Maxim übernehmen
 * \param 	command		Der Befehl der gesendet werden soll
 * \param 	value		Die Daten die zum Befehl gehören
 * \retval  void
 */
/* -----------------------------------------------------------------------------------------------------------*/
void LEDTAFEL_set_intensity( char value )
{
	if (value == 0)
		value = 1;

	LEDTAFEL_set_command_and_value_for_screen( MAXIM_CMD_INTENSITY , value-1 );

	memset ( LEDTAFEL_screenlum , value , LEDTAFEL_ROW * LEDTAFEL_LINES );
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! Sendet einen kompletten Befehl + Daten (zusammen 16 Bit) und lässt ihm von aktuellen Maxim übernehmen
 * \param 	command		Der Befehl der gesendet werden soll
 * \param 	value		Die Daten die zum Befehl gehören
 * \retval  void
 */
/* -----------------------------------------------------------------------------------------------------------*/
void LEDTAFEL_set_clearscreen( void )
{
	LEDTAFEL_set_command_and_value_for_screen( MAXIM_CMD_SHUTDOWN , 0x00 );	// put maxim in shutdown mode
	memset ( LEDTAFEL_shutdown , 0x00 , LEDTAFEL_ROW * LEDTAFEL_LINES);	// 0x00 means maxim is in shutdown mode

	LEDTAFEL_set_pattern_for_screen_fast( 0x00 );		// dann inhalt loeschen

	memset ( LEDTAFEL_screen , 0x20 , LEDTAFEL_ROW * LEDTAFEL_LINES );

	// graphicbuffer loeschen
	memset ( LEDTAFEL_screen_graphic_buffer , 0x00 , LEDTAFEL_ROW * LEDTAFEL_LINES * 8);
	memset ( LEDTAFEL_screen_graphic        , 0x00 , LEDTAFEL_ROW * LEDTAFEL_LINES * 8);
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! Sendet einen kompletten Befehl + Daten (zusammen 16 Bit) und lässt ihm von aktuellen Maxim übernehmen
 * \param 	command		Der Befehl der gesendet werden soll
 * \param 	value		Die Daten die zum Befehl gehören
 * \retval  void
 */
/* -----------------------------------------------------------------------------------------------------------*/
void LEDTAFEL_set_resetscreen( void )
{
	LEDTAFEL_set_reinitialize();
	LEDTAFEL_graphic_update_nobuffering();
}

void LEDTAFEL_set_reinitialize( void )
{
	LEDTAFEL_set_command_and_value_for_screen( MAXIM_CMD_SCANLIMIT , 7 );
	LEDTAFEL_set_command_and_value_for_screen( MAXIM_CMD_DECODE , 0 );
	LEDTAFEL_set_command_and_value_for_screen( MAXIM_CMD_TESTMODE , 0 );
	LEDTAFEL_lumupdate_display();

	// alle in den theoretischen shutdown senden
	memset(LEDTAFEL_shutdown,0,sizeof(LEDTAFEL_shutdown));
}

#endif
