/***************************************************************************
 *            lcd_streaminfo.c
 *
 *  Thu Feb  9 09:49:53 2012
 *  Copyright  2012  Dirk Broßwick
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
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>

#include "system/config/eeconfig.h"
#include "system/thread/thread.h"
#include "system/clock/clock.h"
#include "system/stdout/stdout.h"
#include "system/net/ip.h"
#include "system/softreset/softreset.h"

#if defined(__AVR_XMEGA__)
	#include <system/clock/delay_x.h>
#else
	#include "util/delay.h"
#endif

#include "config.h"

#if defined(VS10XX)

#include "hardware/vs10xx/vs10xx.h"
#include "hardware/vs10xx/vs10xx_buffer.h"
#include "hardware/vs10xx/vs10xx_plugin.h"
#include "streaminfo.h"
#include "hardware/lcd/dogl128w6.h"

#include "apps/mp3-streamingclient/mp3-streaming.h"
#include "apps/mp3-streamingclient/font3x5.h"
#include "apps/mp3-streamingclient/menu/menu-interpreter.h"
#include "apps/mp3-streamingclient/menu/menudata-progmem.c"

#include "playlist.h"

#if defined(LCD)
	#include "hardware/lcd/lcd.h"
	#include "hardware/lcd/dogl128w6.h"
#endif

#include "mp3-clientserver.h"
#include "system/sensor/temp.h"

#include "hardware/network/zg2100/zg2100_driver.h"

// idle-time für timeout menu berechnen
static int counter = IDLE_TIME;

char time_string[] = "Zeit:  --:--";
char contrast_string[] = "64%  ";
char contrast_value = 16;
char backlight_string[] = "255";
char backlight_value = 255;
unsigned int backlight_current = 0;
unsigned int backlight_destination = 0;
char firststart = 5;
char volcounter = 15;

unsigned int powerlight_current = 0;
unsigned char powerlight_state = 0;

char volume_value = 255;

char STREAM_BANDWIDTH[32] = "0kbs";
char STREAM_FPS[32] = "0";
char STREAM_PROTOCOL[32] = "";
char STREAM_SOURCE[32] = "";
char STREAM_BUFFER[32] = "";
char STREAM_COUNTER[32] = "";

char IP_string[4][5];
char IP_value[4] = {0,0,0,0};
char GW_string[4][5];
char GW_value[4] = {0,0,0,0};
char NM_string[] = "0 ";
char NM_value = 0 ;
char DNS_string[4][5];
char DNS_value[4] = {0,0,0,0};

char streaminfotitle[256];
char streaminfosource[256];
static int startpostitle = 0, startpossource = 0;

unsigned char menu_byte_get(MENUADDR addr)
{
	return( pgm_read_byte( &menudata[addr] ) );
}
void menu_screen_set(SCREENPOS x, SCREENPOS y, unsigned char color)
{
	DOGL128_FBsetPixel( x, y, color );
}

void menu_screen_flush(void)
{
	DOGL128_FBupdate();
}

void menu_screen_clear(void)
{
	DOGL128_FBclear();	
}

unsigned char menu_action(unsigned short action)
{
	int data;
	char string[32];
	
	switch( action )
	{
		case MENU_ACTION_STREAM_PLAY_URL:			printf_P( PSTR("Play URL\r\n"));
													counter = 3;
													break;
		case MENU_ACTION_STREAM_REPLAY:				printf_P( PSTR("Replay\r\n"));
													RePlayURL();
													counter = 3;
													break;
		case MENU_ACTION_BASS:						data = VS10xx_read(2);
													if ( menu_checkboxstate[ MENU_CHECKBOX_BASS ] != 0 )
														VS10xx_write( 2, ( data & 0xff00 ) | 0x00fa );
													else
														VS10xx_write( 2, ( data & 0xff00 ) );
													printf_P( PSTR("Bass %d\r\n"),menu_checkboxstate[ MENU_CHECKBOX_BASS ] );
													break;
		case MENU_ACTION_TREBLE:					data = VS10xx_read(2);
													if ( menu_checkboxstate[ MENU_CHECKBOX_TREBLE ] != 0 )
														VS10xx_write( 2, ( data & 0x00ff ) | 0x7500 );
													else
														VS10xx_write( 2, ( data & 0x00ff ) );
													printf_P( PSTR("TREBLE %d\r\n"),menu_checkboxstate[ MENU_CHECKBOX_TREBLE ] );
													break;
		case MENU_ACTION_STREAM_STOP:				printf_P( PSTR("Stop"));
													StopPlay();
													break;
		case MENU_ACTION_RESET:						softreset();
													break;
		case MENU_ACTION_CONF_DISPLAY:				printf_P( PSTR("Conf Display\r\n"));
													break;
/*		case MENU_ACTION_STREAM_INFO:				key = getchar();
													menu_redraw();
													while( key != '\r' )
													{
														key = getchar();
														sprintf_P( STREAM_FPS, PSTR("%d"), streambuffer_getfps ());
														sprintf_P( STREAM_BUFFER, PSTR("%d%%"), streambuffer_get ( 100 ));
														menu_keypress( key );
													}
													break;
*/		case MENU_ACTION_AUTOPLAY:					mp3client_autoplay( menu_checkboxstate[ MENU_CHECKBOX_AUTOPLAY ] );
													break;
		case MENU_ACTION_CONF_NET:					printf_P( PSTR("Conf Network\r\n"));
													break;
		case MENU_ACTION_CONTRAST:					
													while( MENU_param_uint8( &contrast_value, 0 , 25 ) != 13 )
													{
														DOGL128_Contrast( contrast_value );
														sprintf_P( contrast_string , PSTR("%d%%"), contrast_value * 4 );
													}
													break;
		case MENU_ACTION_BACKLIGHT:					while( MENU_param_uint8( &backlight_value, 0 , 255 ) != 13 )
													{
														backlight_destination = backlight_value;
														backlight_current = backlight_value * 256;
														sprintf_P( backlight_string , PSTR("%d"), backlight_value );
													}
													break;
		case MENU_ACTION_IP24:						while( MENU_param_uint8( &IP_value[0], 0 , 255 ) != 13 )
													{
														sprintf_P( IP_string[0] , PSTR("%d"), IP_value[0] );
													}
													break;
		case MENU_ACTION_IP16:						while( MENU_param_uint8( &IP_value[1], 0 , 255 ) != 13 )
													{
														sprintf_P( IP_string[1] , PSTR("%d"), IP_value[1] );
													}
													break;
		case MENU_ACTION_IP8:						while( MENU_param_uint8( &IP_value[2], 0 , 255 ) != 13 )
													{
														sprintf_P( IP_string[2] , PSTR("%d"), IP_value[2] );
													}
													break;
		case MENU_ACTION_IP0:						while( MENU_param_uint8( &IP_value[3], 0 , 255 ) != 13 )
													{
														sprintf_P( IP_string[3] , PSTR("%d"), IP_value[3] );
													}
													break;
		case MENU_ACTION_GW24:						while( MENU_param_uint8( &GW_value[0], 0 , 255 ) != 13 )
													{
														sprintf_P( GW_string[0] , PSTR("%d"), GW_value[0] );
													}
													break;
		case MENU_ACTION_GW16:						while( MENU_param_uint8( &GW_value[1], 0 , 255 ) != 13 )
													{
														sprintf_P( GW_string[1] , PSTR("%d"), GW_value[1] );
													}
													break;
		case MENU_ACTION_GW8:						while( MENU_param_uint8( &GW_value[2], 0 , 255 ) != 13 )
													{
														sprintf_P( GW_string[2] , PSTR("%d"), GW_value[2] );
													}
													break;
		case MENU_ACTION_GW0:						while( MENU_param_uint8( &GW_value[3], 0 , 255 ) != 13 )
													{
														sprintf_P( GW_string[3] , PSTR("%d"), GW_value[3] );
													}
													break;
		case MENU_ACTION_DNS24:						while( MENU_param_uint8( &DNS_value[0], 0 , 255 ) != 13 )
													{
														sprintf_P( DNS_string[0] , PSTR("%d"), DNS_value[0] );
													}
													break;
		case MENU_ACTION_DNS16:						while( MENU_param_uint8( &DNS_value[1], 0 , 255 ) != 13 )
													{
														sprintf_P( DNS_string[1] , PSTR("%d"), DNS_value[1] );
													}
													break;
		case MENU_ACTION_DNS8:						while( MENU_param_uint8( &DNS_value[2], 0 , 255 ) != 13 )
													{
														sprintf_P( DNS_string[2] , PSTR("%d"), DNS_value[2] );
													}
													break;
		case MENU_ACTION_DNS0:						while( MENU_param_uint8( &DNS_value[3], 0 , 255 ) != 13 )
													{
														sprintf_P( DNS_string[3] , PSTR("%d"), DNS_value[3] );
													}
													break;
		case MENU_ACTION_MASK:						while( MENU_param_uint8( &NM_value, 0 , 32 ) != 13 )
													{
														sprintf_P( NM_string , PSTR("%d"), NM_value );
													}
													break;
		case MENU_ACTION_SAVE_SETTINGS:				STREAM_saveconfig();
													break;			
		case MENU_ACTION_CONF_STREAM:				printf_P( PSTR("Conf Stream\r\n"));
													break;
		case MENU_ACTION_PLAYLIST:					printf_P( PSTR("play %d\r\n"), menu_listindexstate[ MENU_LISTINDEX_PLAYLIST ] );
													PLAYLIST_playentry( menu_listindexstate[ MENU_LISTINDEX_PLAYLIST ] );
													sprintf_P( string, PSTR("%d"), menu_listindexstate[ MENU_LISTINDEX_PLAYLIST ] );
													changeConfig_P( PSTR("STREAM_LASTPLAY"), string );
													counter = 1;
													break;
	}

	
	return(1);
}

int MENU_param_uint8( char * value, char from, char to)
{
	int key;

	key = getchar();

	if ( key == 's')
	{
		if ( (*value) > from )
		{
			(*value)--;
		}
	}
	if ( key == 'a')
	{
		if ( (*value) < to )
		{
			(*value)++;
		}
	}
	menu_redraw();

	return(key);
}


static char LED_mode = 0;

void streaminfo_backlight( void )
{	
	static char timer=0;
	if ( LED_mode == 0 )
	{	
		PORTC.DIRSET = ( 1 << 0 );
		timer = 0;
		if ( backlight_destination != backlight_current )
		{
			if ( ( backlight_destination * 256 ) < backlight_current )
			{
				backlight_current = backlight_current - 1;
			}
			else if ( ( backlight_destination * 256 ) > backlight_current )
			{
				backlight_current = backlight_current + 256;
			}

			TCC0.CCB = backlight_current;
			TCC0.CCA = backlight_current;
		}
	}
	else
	{
		timer++;
		if ( timer%10 == 0 )
			PORTC.DIRTGL = ( 1 << 0 );

		if ( timer == 100 )
			timer = 0;
	}
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Init um den Streamingdienst zu starten.
 * \return	NONE		None
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
void streaminfo_init( void )
{	
	streaminfotitle[0] = '\0';
	streaminfosource[0] = '\0';
	THREAD_RegisterThread( streaminfo_thread, PSTR("streaminfo"));

	PORTC.DIRSET = (1<<1) | (1<<0);	
	TCC0.CCB = 0;
	TCC0.CCA = 0;
	TCC0.CTRLB = 0x33;
	/*Timertopwert*/
	TCC0.PER = 0xffff;
	/*4 Vorteiler*/
	TCC0.CTRLA = 0x01;

	STREAM_loadconfig();

	menu_redraw();

	backlight_destination = backlight_value;
	backlight_current = 0;
	// Callback eintragen
	CLOCK_RegisterCallbackFunction( streaminfo_backlight , MSECOUND );

	VS10xx_vol( volume_value, volume_value );
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Thread für ausgaben auf den Display.
 * \return	NONE		None
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
void streaminfo_thread( void )
{
	int i, temp;
	char tempstring[10], spec[16], string[32];
	
#if defined(LCD)

	static char lastmm = 0;
	static char lastss = 0;
	static char lastms = 0;
	
	// Zeit holen
	struct TIME nowtime;
	CLOCK_GetTime ( &nowtime );

	// hole eine taste vom Encoder
	struct STDOUT oldstream;
	STDOUT_save( &oldstream );
	
	STDOUT_set( _ENCODER_P2W1 , 0 );
	int key = getchar();

	STDOUT_restore( &oldstream );		

	if ( lastss != nowtime.ss )
	{
		lastss = nowtime.ss;
		if ( counter > 0 )
		{
			if ( counter == 1 )
			{
				LCD_clrscreen( 0 );
				if ( menu_checkboxstate[ MENU_CHECKBOX_DIMIDLE ] )
					backlight_destination = backlight_value / 16;
			}
			counter--;
		}

		if ( firststart != 0 )
		{
			firststart--;
			if ( firststart == 0 )
				if ( menu_checkboxstate[ MENU_CHECKBOX_AUTOPLAY ] )
			{
				readConfig_P ( PSTR("STREAM_LASTPLAY"), string );
				PLAYLIST_playentry( atoi(string) );
				counter = 1;
			}
		}

		if ( menu_checkboxstate[ MENU_CHECKBOX_AUTOVOLUME ] )
		{
			if ( volcounter > 0 )
			{
				volcounter--;
				if ( volcounter == 0 )
				{
					sprintf_P( string, PSTR("%d"), volume_value );
					deleteConfig_P( PSTR("STREAM_VOL") );
					writeConfig_P( PSTR("STREAM_VOL"), string );
				}
			}
		}
	}

	if ( key != EOF )
	{
		if ( counter == 0 )
		{
			if ( key == '\r')
			{
				key = EOF;
				counter = IDLE_TIME;
				backlight_destination = backlight_value;
			}
		}
		else
			counter = IDLE_TIME;
	}
	
	if ( counter != 0 )
	{		
		STDOUT_set( _ENCODER_P2W1 , 0 );

		if ( ( nowtime.ms%5 ) == 0 )
		{
			if (streambuffer_getstate() )
			{
				sprintf_P( STREAM_BANDWIDTH, PSTR("%dkbs"), streambuffer_getbandwidth() );
				sprintf_P( STREAM_BUFFER, PSTR("%d%%"), streambuffer_get ( 100 ));
				sprintf_P( STREAM_COUNTER, PSTR("%ldkbyte"), streambuffer_getcounter() / 1024 );
				switch( streambuffer_getsource() )
				{
					case stream_http:		sprintf_P( STREAM_PROTOCOL, PSTR("http / tcp / icecast") );
											menu_strings[ MENU_TEXT_SOURCE ] = (unsigned char *) mp3client_geturl();
											break;
					case stream_udp:		sprintf_P( STREAM_PROTOCOL, PSTR("udp") );
											menu_strings[ MENU_TEXT_SOURCE ] = (unsigned char *) &streaminfotitle[16];
											break;
					case stream_file:		sprintf_P( STREAM_PROTOCOL, PSTR("file") );
											break;
					default:				sprintf_P( STREAM_PROTOCOL, PSTR("unknown") );
				}

				sprintf_P( STREAM_FPS, PSTR("%dfps"), streambuffer_getfps() );
			}
			else
			{
				STREAM_BANDWIDTH[0] = '\0';
				STREAM_FPS[0] = '\0';
				STREAM_PROTOCOL[0] = '\0';
				STREAM_BUFFER[0] = '\0';
				STREAM_COUNTER[0] = '\0';
				menu_strings[ MENU_TEXT_SOURCE ] = (unsigned char *) STREAM_SOURCE;
			}
		}
		sprintf_P( time_string , PSTR("Zeit:  %02d:%02d"), nowtime.hh, nowtime.mm );
		menu_redraw();
		menu_keypress( key );

		STDOUT_Flush();
		STDOUT_restore( &oldstream );		
	}	
	else
	{		
		if ( key == 'a' || key == 's' )
		{
			if ( key == 's' )
				if ( volume_value > 0 )
					volume_value--;

			if ( key == 'a' )
				if ( volume_value < 255 )
					volume_value++;

			volcounter = 15;
			VS10xx_vol( volume_value, volume_value );

			if ( menu_checkboxstate[ MENU_CHECKBOX_DIMIDLE ] )
				backlight_destination = backlight_value / 2;
			
		}
		
		if ( volcounter == 0 )
			if ( menu_checkboxstate[ MENU_CHECKBOX_DIMIDLE ] )
				backlight_destination = backlight_value / 16;

		if ( lastms != nowtime.ms )
		{
			lastms = nowtime.ms;
			
			STDOUT_set( _LCD , 0 );

			int x,y;
			char count;

			// gibt aktuelle Uhrzeit aus
			if( ( nowtime.ms%50 ) == 0 )
			{
				if ( streambuffer_getstate() == 0 )
				{
					char zeit[6];
						sprintf_P(zeit, PSTR("%02d:%02d"),nowtime.hh,nowtime.mm);
						
					for( i = 0 ; i < 5 ; i++ )
					{
						for( y = 0 ; y < 5 ; y++ )
						{
							for( x = 0 ; x < 3 ; x++ )
							{
								LCD_setXY(0,x + i * 3,y+1);
								printf_P(PSTR(" "));
								LCD_setXY(0,x + i * 3,y+1);
								printf_P(PSTR("%c"),pgm_read_byte( &font3x5[ zeit[i]-0x30 ][y][x] ) );
							}
						}
					}
					
					if ( ( lastmm != nowtime.mm ) && ( nowtime.mm%5 == 0 ) )
					{
						lastmm = nowtime.mm;
						
						LCD_setXY(0,0,7);
						temp = TEMP_readtemp(0);
						TEMP_Temp2String( temp, tempstring );
#if defined(USE_ZG2100)
						printf_P(PSTR("    %sC    %02d"),tempstring, zg2100_getstat() );
#endif
					}
				}
				else
				{
					LCD_setXY(0,0,1);
					printf_P(PSTR("   Zeit %02d:%02d   "),nowtime.hh,nowtime.mm);
					LCD_setXY(0,0,2);
					printf_P(PSTR("                "),nowtime.hh,nowtime.mm);
				}
			}

				// Zeichne Spektrum der aktuellen Music die gespielt wird
			if (streambuffer_getstate() != 0)
			{
				if( ( nowtime.ms%5 ) == 0 )
				{				
					if ( VS10XX_readspec( spec ) == 0 && streambuffer_getstate() != 0)
					{	
						for ( x = 1 ; x < 15 ; x++ )
						{
							count = spec[x-1];

							for( y = 0 ; y < 4 ; y++ )
							{
								LCD_setXY(0,x,7-y);

								if( count > (y*8))			
								{
									if ( count - ( y*8 ) < 8 )
										printf_P(PSTR("%c"),0xe0 + count%8 );
									else
										printf_P(PSTR("%c"),0xe8 );

								}
								else
								{
									printf_P(PSTR("%c"),0x20);
								}
							}
						}
					}
				}

				// Scrollt aktuelle Informationen durch das LCD wenn Musik gespielt wird
				if( ( nowtime.ms%45 ) == 0 )
				{				
					if ( streambuffer_getstate() == 0 )
					{
						startpostitle = 0;
						startpossource = 0;
						streaminfotitle[ 0 ] = '\0';
						streaminfosource[ 0 ] = '\0';
					}
					else
					{
						if ( strlen( streaminfotitle ) > 16 )
						{
							startpostitle++;
							if ( startpostitle > ( strlen( streaminfotitle ) - 16 ) )
								startpostitle = 0;
						}

						LCD_setXY(0,0,3);

						for( i = 0 ; i < 16 ; i++ )
						{
							if ( streaminfotitle[ i + startpostitle ] == '\0')
								break;
							printf_P(PSTR("%c"), streaminfotitle[ i + startpostitle ] );
						}
					}
				}
			}
		}		
		STDOUT_Flush();
		STDOUT_restore( &oldstream );		
	}
#endif
}

void streaminfo_updatetitle( char * title )
{
	int y, i;
	
	if( strlen( title ) > 16 )
	{
		for(i=0;i<16;i++)
			streaminfotitle[i]=' ';
		
		strcpy( &streaminfotitle[i], title );

		y = strlen( title ) + 16;

		while(i)
		{
			streaminfotitle[y++]=' ';
			i--;
		}
		streaminfotitle[y++]='\0';		
	}
	else if( strlen( title ) < 16 )
	{
		for(i=0;i<( ( 16 - strlen( title ) ) / 2  );i++)
			streaminfotitle[i]=' ';
		strcpy( &streaminfotitle[i], title );
	}
	else
	{
		strcpy( streaminfotitle, title );
	}
	
	startpostitle = 0;
}

void streaminfo_updatesource( char * source )
{
	int y,i;
	
	if( strlen( source ) > 16 )
	{
		for(i=0;i<16;i++)
			streaminfosource[i]=' ';
		
		strcpy( &streaminfosource[i], source );

		y = strlen( source )+16;
		while(i)
		{
			streaminfosource[y++]=' ';
			i--;
		}
		streaminfosource[y++]='\0';		
	}
	else if( strlen( source ) < 16 )
	{
		for(i=0;i<( ( 16 - strlen( source ) ) / 2  );i++)
			streaminfosource[i]=' ';
		strcpy( &streaminfosource[i], source );
	}
	else
	{	
		strcpy( streaminfosource, source );
	}
	startpossource = 0;
}

void STREAM_loadconfig( void )
{
	char string[128];
	int data;
	
	PLAYLIST_init();

	menu_strings[ MENU_TEXT_TIME ] = (unsigned char *) time_string;
	menu_strings[ MENU_TEXT_CONTRAST ] = (unsigned char *) contrast_string;
	menu_strings[ MENU_TEXT_BACKLIGHT ] = (unsigned char *) backlight_string;
	menu_strings[ MENU_TEXT_IP24 ] = (unsigned char *) IP_string[0];
	menu_strings[ MENU_TEXT_IP16 ] = (unsigned char *) IP_string[1];
	menu_strings[ MENU_TEXT_IP8 ] = (unsigned char *) IP_string[2];
	menu_strings[ MENU_TEXT_IP0 ] = (unsigned char *) IP_string[3];
	menu_strings[ MENU_TEXT_BANDWIDTH ] = (unsigned char *) STREAM_BANDWIDTH;
	menu_strings[ MENU_TEXT_BUFFER ] = (unsigned char *) STREAM_BUFFER;
	menu_strings[ MENU_TEXT_SOURCE ] = (unsigned char *) STREAM_SOURCE;
	menu_strings[ MENU_TEXT_PROTO ] = (unsigned char *) STREAM_PROTOCOL;
	menu_strings[ MENU_TEXT_FPS ] = (unsigned char *) STREAM_FPS;
	menu_strings[ MENU_TEXT_BYTECOUNTER ] = (unsigned char *) STREAM_COUNTER;

	memcpy(&IP_value,&myIP,4);
	sprintf_P( IP_string[0] , PSTR("%d"), IP_value[0] );
	sprintf_P( IP_string[1] , PSTR("%d"), IP_value[1] );
	sprintf_P( IP_string[2] , PSTR("%d"), IP_value[2] );
	sprintf_P( IP_string[3] , PSTR("%d"), IP_value[3] );
	menu_strings[ MENU_TEXT_GW24 ] = (unsigned char *) GW_string[0];
	menu_strings[ MENU_TEXT_GW16 ] = (unsigned char *) GW_string[1];
	menu_strings[ MENU_TEXT_GW8 ] = (unsigned char *) GW_string[2];
	menu_strings[ MENU_TEXT_GW0 ] = (unsigned char *) GW_string[3];
	memcpy(&GW_value,&DNSserver,4);
	sprintf_P( GW_string[0] , PSTR("%d"), GW_value[0] );
	sprintf_P( GW_string[1] , PSTR("%d"), GW_value[1] );
	sprintf_P( GW_string[2] , PSTR("%d"), GW_value[2] );
	sprintf_P( GW_string[3] , PSTR("%d"), GW_value[3] );
	menu_strings[ MENU_TEXT_DNS24 ] = (unsigned char *) DNS_string[0];
	menu_strings[ MENU_TEXT_DNS16 ] = (unsigned char *) DNS_string[1];
	menu_strings[ MENU_TEXT_DNS8 ] = (unsigned char *) DNS_string[2];
	menu_strings[ MENU_TEXT_DNS0 ] = (unsigned char *) DNS_string[3];
	memcpy(&DNS_value,&Gateway,4);
	sprintf_P( DNS_string[0] , PSTR("%d"), DNS_value[0] );
	sprintf_P( DNS_string[1] , PSTR("%d"), DNS_value[1] );
	sprintf_P( DNS_string[2] , PSTR("%d"), DNS_value[2] );
	sprintf_P( DNS_string[3] , PSTR("%d"), DNS_value[3] );
	menu_strings[ MENU_TEXT_MASK ] = (unsigned char *) NM_string;
	menu_strings[ MENU_TEXT_PLAYLIST ] = (unsigned char *) PLAYLIST_getlistpointer();

	if (  checkConfigName_P( PSTR("STREAM_DIMIDLE") ) != -1 ) 
	{
		readConfig_P ( PSTR("STREAM_DIMIDLE"), string );
		menu_checkboxstate[ MENU_CHECKBOX_DIMIDLE ] = atoi( string );
	}

	if (  checkConfigName_P( PSTR("STREAM_BACKLIGHT") ) != -1 ) 
	{
		readConfig_P ( PSTR("STREAM_BACKLIGHT"), string );
		backlight_value = atoi( string );
		sprintf_P( backlight_string , PSTR("%d%"), backlight_value);
	}

	if (  checkConfigName_P( PSTR("STREAM_CONTRAST") ) != -1 ) 
	{
		readConfig_P ( PSTR("STREAM_CONTRAST"), string );
		contrast_value = atoi( string );
		sprintf_P( contrast_string , PSTR("%d%%"), contrast_value * 4);
	}

	if (  checkConfigName_P( PSTR("STREAM_AUTOVOLUME") ) != -1 ) 
	{
		readConfig_P ( PSTR("STREAM_AUTOVOLUME"), string );
		menu_checkboxstate[ MENU_CHECKBOX_AUTOVOLUME ] = atoi( string );
	}

	if (  checkConfigName_P( PSTR("STREAM_VOL") ) != -1 ) 
	{
		readConfig_P ( PSTR("STREAM_VOL"), string );
		volume_value = (unsigned char)atoi( string );
	}

	if (  checkConfigName_P( PSTR("TFTP_SERVER") ) != -1 ) 
	{
		readConfig_P ( PSTR("TFTP_SERVER"), string );
		menu_checkboxstate[ MENU_CHECKBOX_TFTP ] = atoi( string );
	}
	
	if (  checkConfigName_P( PSTR("HTTP_SERVER") ) != -1 ) 
	{
		readConfig_P ( PSTR("HTTP_SERVER"), string );
		menu_checkboxstate[ MENU_CHECKBOX_HTTP ] = atoi( string );
	}

	if (  checkConfigName_P( PSTR("STREAM_ANNONCE") ) != -1 ) 
	{
		readConfig_P ( PSTR("STREAM_ANNONCE"), string );
		menu_checkboxstate[ MENU_CHECKBOX_ANNONCE ] = atoi( string );
	}

	if (  checkConfigName_P( PSTR("STREAM_AUTOPLAY") ) != -1 ) 
	{
		readConfig_P ( PSTR("STREAM_AUTOPLAY"), string );
		menu_checkboxstate[ MENU_CHECKBOX_AUTOPLAY ] = atoi( string );
		mp3client_autoplay(atoi( string ));
	}

	if (  checkConfigName_P( PSTR("STREAM_RESTARTBROKEN") ) != -1 ) 
	{
		readConfig_P ( PSTR("STREAM_RESTARTBROKEN"), string );
		menu_checkboxstate[ MENU_CHECKBOX_RESTARTBROKEN ] = atoi( string );
	}

	if (  checkConfigName_P( PSTR("STREAM_TCPUDP") ) != -1 ) 
	{
		readConfig_P ( PSTR("STREAM_TCPUDP"), string );
		menu_checkboxstate[ MENU_CHECKBOX_TCPUDP ] = atoi( string );
	}

	if (  checkConfigName_P( PSTR("STREAM_BASS") ) != -1 ) 
	{
		readConfig_P ( PSTR("STREAM_BASS"), string );
		menu_checkboxstate[ MENU_CHECKBOX_BASS ] = atoi( string );
		data = VS10xx_read(2);
		if ( menu_checkboxstate[ MENU_CHECKBOX_BASS ] != 0 )
			VS10xx_write( 2, ( data & 0xff00 ) | 0x00fa );
		else
			VS10xx_write( 2, ( data & 0xff00 ) );
	}
	if (  checkConfigName_P( PSTR("STREAM_TREBLE") ) != -1 ) 
	{
		readConfig_P ( PSTR("STREAM_TREBLE"), string );
		menu_checkboxstate[ MENU_CHECKBOX_TREBLE ] = atoi( string );
		data = VS10xx_read(2);
		if ( menu_checkboxstate[ MENU_CHECKBOX_TREBLE ] != 0 )
			VS10xx_write( 2, ( data & 0x00ff ) | 0x7500 );
		else
			VS10xx_write( 2, ( data & 0x00ff ) );
	}
}

void STREAM_saveconfig( void )
{
	char string[128];

	LED_mode = 1;
	
	deleteConfig_P( PSTR("STREAM_BACKLIGHT") );
	deleteConfig_P( PSTR("STREAM_CONTRAST") );
	deleteConfig_P( PSTR("STREAM_BASS") );
	deleteConfig_P( PSTR("STREAM_TREBLE") );
	deleteConfig_P( PSTR("TFTP_SERVER") );
	deleteConfig_P( PSTR("HTTP_SERVER") );
	deleteConfig_P( PSTR("STREAM_ANNONCE") );
	deleteConfig_P( PSTR("STREAM_AUTOPLAY") );
	deleteConfig_P( PSTR("STREAM_RESTARTBROKEN") );
	deleteConfig_P( PSTR("STREAM_TCPUDP") );
	deleteConfig_P( PSTR("STREAM_AUTOVOLUME") );
	deleteConfig_P( PSTR("STREAM_DIMIDLE") );

	sprintf_P( string, PSTR("%d"), backlight_value );
	writeConfig_P( PSTR("STREAM_BACKLIGHT"), string );

	sprintf_P( string, PSTR("%d"), contrast_value );
	writeConfig_P( PSTR("STREAM_CONTRAST"), string );

	sprintf_P( string, PSTR("%d"), menu_checkboxstate[ MENU_CHECKBOX_BASS ] );
	writeConfig_P( PSTR("STREAM_BASS"), string );

	sprintf_P( string, PSTR("%d"), menu_checkboxstate[ MENU_CHECKBOX_TREBLE ] );
	writeConfig_P( PSTR("STREAM_TREBLE"), string );

	sprintf_P( string, PSTR("%d"), menu_checkboxstate[ MENU_CHECKBOX_TFTP ] );
	writeConfig_P( PSTR("TFTP_SERVER"), string );

	sprintf_P( string, PSTR("%d"), menu_checkboxstate[ MENU_CHECKBOX_HTTP ] );
	writeConfig_P( PSTR("HTTP_SERVER"), string );

	sprintf_P( string, PSTR("%d"), menu_checkboxstate[ MENU_CHECKBOX_ANNONCE ] );
	writeConfig_P( PSTR("STREAM_ANNONCE"), string );

	sprintf_P( string, PSTR("%d"), menu_checkboxstate[ MENU_CHECKBOX_AUTOPLAY ] );
	writeConfig_P( PSTR("STREAM_AUTOPLAY"), string );
	
	sprintf_P( string, PSTR("%d"), menu_checkboxstate[ MENU_CHECKBOX_RESTARTBROKEN ] );
	writeConfig_P( PSTR("STREAM_RESTARTBROKEN"), string );

	sprintf_P( string, PSTR("%d"), menu_checkboxstate[ MENU_CHECKBOX_AUTOVOLUME ] );
	writeConfig_P( PSTR("STREAM_AUTOVOLUME"), string );

	sprintf_P( string, PSTR("%d"), menu_checkboxstate[ MENU_CHECKBOX_DIMIDLE ] );
	writeConfig_P( PSTR("STREAM_DIMIDLE"), string );

	sprintf_P( string, PSTR("%d"), volume_value );
	writeConfig_P( PSTR("STREAM_VOL"), string );

	sprintf_P( string, PSTR("%d"), menu_checkboxstate[ MENU_CHECKBOX_TCPUDP ] );
	writeConfig_P( PSTR("STREAM_TCPUDP"), string );

	LED_mode = 0;
}

#endif // VS10XX

