/***************************************************************************
 *            led_tafel.h
 *
 *  Sun Jun  6 16:36:51 2010
 *  Copyright  2010  Dirk Broßwick
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

#ifndef LED_TAFEL_H
	#define LED_TAFEL_H

	#define 	LEDTAFEL_ROW				56
	#define 	LEDTAFEL_LINES				20

	#define		LINECONTROL					PORTF
	#define		LINECONTROL_DDR				DDRF

	#define		SERIALLINE_CLOCK			PE5
	#define		SERIALLINE_CLOCK_PORT		PORTE
	#define		SERIALLINE_CLOCK_DDR		DDRE
	
	#define		MAXIM_SELECT				PE7
	#define		MAXIM_SELECT_PORT			PORTE
	#define		MAXIM_SELECT_DDR			DDRE
	#define		MAXIM_DESELECT				PB4
	#define		MAXIM_DESELECT_PORT			PORTB
	#define		MAXIM_DESELECT_DDR			DDRB
	#define		MAXIM_LATCHCONTROL			PE6
	#define		MAXIM_LATCHCONTROL_PORT		PORTE
	#define		MAXIM_LATCHCONTROL_DDR		DDRE
	
	#define 	SETUPTIME					1

	#define		MAXIM_CMD_NOP				0x00
	#define		MAXIM_CMD_DECODE			0x09
	#define		MAXIM_CMD_INTENSITY			0x0A
	#define		MAXIM_CMD_SCANLIMIT			0x0B
	#define		MAXIM_CMD_SHUTDOWN			0x0C
	#define		MAXIM_CMD_TESTMODE			0x0F

	struct TAFEL_CMD_HEADER {
		int16_t		TAFEL_CMD;
		int16_t		TAFEL_X;
		int16_t		TAFEL_Y;
		int16_t		TAFEL_W;
		int16_t		TAFEL_H;
		char	TAFEL_DATA[ LEDTAFEL_ROW * LEDTAFEL_LINES ];
		char	TRASH;
	};

	enum tafel_display { 	ack=0,
							nak=1, 
							clear=2, 
							write_raw=3, 
							write_std=4, 
							write_lum_raw=5, 
							write_lum_std=6,
							intensity=7,
							reset=8,
							read_raw=9,
							read_lum_raw=10,
							hardreset=11,
							allpattern=12,
							fade_out=13,
							graphic=14,
							graphic_fast=15,
							graphic_direct=16,
							graphic_update=17	};

	int LEDTAFEL_init (void);
	void LEDTAFEL_initialize_display( void );
	void LEDTAFEL_print_char( unsigned char Zeichen );
	void LEDTAFEL_print_char_stdio( unsigned char Zeichen );
	void LEDTAFEL_fullupdate_display( void );
	void LEDTAFEL_screenupdate_display( void );
	void LEDTAFEL_lumupdate_display( void );
	void LEDTAFEL_scrollup_display( void );
	void LEDTAFEL_selectMatrix( signed char line, signed char row );
	void LEDTAFEL_selectMatrix_fast( signed char line, signed char row );
	void LEDTAFEL_init_maxim( void );
	void LEDTAFEL_fill_with_pattern(unsigned char pattern);
	void LEDTAFEL_cmd_maxim( char command, char value );
	void LEDTAFEL_intensity_maxim( char value);
	void LEDTAFEL_intensity_maxim2( char value, unsigned char oldlum);
	void LEDTAFEL_maxim_select( void );
	void LEDTAFEL_maxim_deselect( void );
	void LEDTAFEL_maxim_setdata( void );
	void LEDTAFEL_serialclock_toggle( void );
	void LEDTAFEL_fadeout_display( int loop , char * lumbuffer );
	char LEDTAFEL_getlum( unsigned short offset , char * lumbuffer );
	char LEDTAFEL_getRow( void );
	char LEDTAFEL_getLine( void );
	void LEDTAFEL_printlogo( void );
	void LEDTAFEL_process_cmd( struct TAFEL_CMD_HEADER * CMD );
	void LEDTAFEL_copy_screen( char * buffer, int xpos, int ypos, int xsize, int ysize );
	void LEDTAFEL_copy_screenlum( char * buffer, int xpos, int ypos, int xsize, int ysize );
	void LEDTAFEL_set_command_and_value_for_screen( char command , char value );
	void LEDTAFEL_set_intensity( char value );
	void LEDTAFEL_set_pattern_for_screen_fast( unsigned char value );
	void LEDTAFEL_set_clearscreen( void );
	void LEDTAFEL_set_resetscreen( void );
	void LEDTAFEL_set_reinitialize( void );
	void LEDTAFEL_graphic_direct( char * buffer, unsigned short offset, unsigned short length, unsigned short update);
	void LEDTAFEL_graphic_update( void );
	void LEDTAFEL_graphic_update_nobuffering(void);
	void LEDTAFEL_graphic_update_nobuffering2(void);

	#define	LEDTAFEL_SPINUM		0

#endif
