#ifndef LED_TAFEL_PAR_H
	#define LED_TAFEL_PAR_H

	#define 	LEDTAFEL_ROW				56
	#define 	LEDTAFEL_LINES				20

	#define		CONTROL_ACK			( 1<<PE5 )
	#define		CONTROL_REQ			( 1<<PE6 )
	#define		CONTROL_SEL			( 1<<PE7 )
	#define		CONTROL_PORT		( PORTE)
	#define		CONTROL_PIN			( PINE )
	#define		CONTROL_DDR			( DDRE )

	#define		CONTROL_SYNC		( 1<<PB0 )
	#define		CONTROL_SYNC_PIN	( PINB )
	#define		CONTROL_SYNC_PORT	( PORTB )
	#define		CONTROL_SYNC_DDR	( DDRB )

	#define		DATA_PORT			PORTF
	#define		DATA_PIN			PINF
	#define		DATA_DDR			DDRF

	#define 	TIMEOUTLOOP		64
	#define		TAFELSENDERR	1
	#define		TAFELSENDOK		0

	#define TAFEL_UDP_Bufferlen 	2000
	#define TAFEL_UDPPORT			2342

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

	void LEDTAFEL_init( void );
	void LEDTAFEL_init_updserver( void );	
	void LEDTAFEL_print_char_stdio( char Data );
	void LEDTAFEL_irq( void );
	int LEDTAFEL_sendblock( char line );
	void LEDTAFEL_process_cmd( struct TAFEL_CMD_HEADER * CMD );
	void LEDTAFEL_copy_screen( char * buffer, int xpos, int ypos, int xsize, int ysize );
	void LEDTAFEL_copy_screenlum( char * buffer, int xpos, int ypos, int xsize, int ysize );
	void LEDTAFEL_set_intensity( char value );
	void LEDTAFEL_set_clearscreen( void );
	void LEDTAFEL_graphic_direct( char * buffer, unsigned short start, unsigned short length, unsigned short update);
	void LEDTAFEL_fadeout_display( int loop, char * lumbuffer );
	void LEDTAFEL_printlogo( void );


#endif
