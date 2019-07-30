/***************************************************************************
 *            zg2100_driver.c
 *
 *  Tue Sep  3 21:22:07 2013
 *  Copyright  2013  Dirk Broßwick
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

/**
 * \ingroup Hardware
 * \addtogroup zg2100 Microchip MRF24WB0MB Wlan Interface driver (zg2100_driver.c)
 *
 * @{
 */

/**
 *
 * \author Dirk Broßwick
 */

#include "config.h"

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include <avr/io.h>

#if defined USE_ZG2100

#include "zg2100_driver.h"
#include "zg2100_hardware.h"
#include "zg2100_com.h"
#include "zg2100_def.h" 
#include "zg2100_mgmt.h" 

#include "hardware/ext_int/ext_int.h"
#include "hardware/led/led_core.h"
#include "system/net/endian.h"
#include "system/net/ethernet.h"
#include "system/stdout/stdout.h"
#include "system/clock/delay_x.h"

#if defined(LCD)
	#include "hardware/lcd/lcd.h"
#endif

static char head_buffer[ ZG2100_HEAD_BUFFER_SIZE ];
static char fifo_buffer[ ZG2100_FIFO_BUFFER_SIZE ];
static char m_ssid[ZG2100_SSID_LEN+1];

/* mgmt variablen */
volatile static char current_mgmt_parm;
volatile static bool mgmt_busy;
volatile static bool connected;
volatile static int rssi=120;
char pmk_key[ZG2100_PMK_LEN ];
static char macaddr[6];

char *scan_buffer = NULL;
int scan_len;
char *mgmt_buffer = NULL;
int mgmt_len;

/* -----------------------------------------------------------------------------------------------------------*/
/*!\brief Initialisiert das Wlan-Modul
 * \param 	void
 * \return	void
 */
/* -----------------------------------------------------------------------------------------------------------*/
void zg2100_drv_init( void )
{
	/* com init */
	zg2100_com_init();

	/* reset hardware */
	zg2100_indexregister_write( ZG2100_IREG_HW_RST, 0x80ff );
	zg2100_indexregister_write( ZG2100_IREG_HW_RST, 0x0fff );

	while( !( zg2100_indexregister_read( ZG2100_IREG_HW_STATUS ) & ZG2100_HW_STATUS_RESET ) );
	while( zg2100_read_register( ZG2100_REG_F0_ROOM ) == 0 );

	/* config interrupts */
	zg2100_write_register( ZG2100_REG_INTF, 0);
	zg2100_write_register( ZG2100_REG_INTE, ZG2100_INT_MASK_F0 | ZG2100_INT_MASK_F1 );
	zg2100_write_register( ZG2100_REG_INTF2, 0xff);
	zg2100_write_register( ZG2100_REG_INTE2, 0 );
	
	/* register interrupts */
	EXTINT_set ( &ZG2100_PORT, ZG2100_INT, SENSE_LOW , zg2100_process );

	/* read config */
	zg2100_mgmt_get_parm( ZG2100_FIFO_MGMT_PARM_MACAD );	

	/* set connection manager on */
	head_buffer[0]=0x01;				/* enable manager 0x01, disable 0x00 */
	head_buffer[1]=0x0a;				/* numbers of try to reconnect */
	head_buffer[2]=0x10|0x02|0x01;		/* connection settings */
	head_buffer[4]=0;					/* must be zero */
	zg2100_mgmt_req(ZG2100_FIFO_MGMT_CONNECT_MANAGE, head_buffer, 4, true );
}

/* -----------------------------------------------------------------------------------------------------------*/
/*!\brief Interruptsroutine. Verarbeitet Ein-/Ausgehende packete, Management-Request und Management Events.
 * \param 	void
 * \return	void
 */
/* -----------------------------------------------------------------------------------------------------------*/
void zg2100_process( void )
{
	int int_reg,fifo,len;
	char type, subtype;

	int_reg = zg2100_read_register( ZG2100_REG_INTF ) & zg2100_read_register( ZG2100_REG_INTE );

	if ( int_reg & ZG2100_INT_MASK_F0 )
		fifo = 0;
	else if ( int_reg & ZG2100_INT_MASK_F1 )
		fifo = 1;
	else
		return;

	zg2100_write_register( ZG2100_REG_INTF , ZG2100_INT_MASK_F(fifo));
	len = zg2100_read_register( ZG2100_REG_F_LEN(fifo)) & 0xfff;

	zg2100_fifo_read( ZG2100_FIFO_ANY, &type, &subtype, fifo_buffer, len );

	len-=2;

	switch( type )
	{
		case ZG2100_FIFO_RD_TXD_ACK:	break;
		case ZG2100_FIFO_RD_RXD_AVL:	zg2100_on_rx( fifo_buffer, len );
										break;
		case ZG2100_FIFO_RD_MGMT_AVL:	zg2100_on_mgmt_avl( subtype, fifo_buffer, len );
										mgmt_busy = false;
										break;
		case ZG2100_FIFO_RD_MGMT_EVT:	zg2100_on_mgmt_evt( subtype, fifo_buffer, len );
										mgmt_busy = false;
										break;
		default:						break;
	}
}

/* -----------------------------------------------------------------------------------------------------------*/
/*!\brief Sendet einen management-request ab.
 * \param 	subtype		Subtype für die anfrage
 * \param	buffer		zeiger auf den buffer der mitgesendet werden soll
 * \param	len			größe des buffers in byte
 * \param	close		Verbindung nach dem Request schliesen true/false
 * \return	void		( löst einen mgmt_avl interrupt aus, in der zg2100_on_mgmt_avl funktion verarbeitet und ausgewertet wird )
 */
/* -----------------------------------------------------------------------------------------------------------*/
void zg2100_mgmt_req( char subtype, char * buffer, int len, bool close )
{
	zg2100_fifo_write( ZG2100_FIFO_MGMT, ZG2100_FIFO_WR_MGMT_REQ, subtype, buffer, len, true, close );
	mgmt_busy = true;
}

/* -----------------------------------------------------------------------------------------------------------*/
/*!\brief Sendet management daten
 * \param	buffer		zeiger auf den buffer der mitgesendet werden soll
 * \param	len			größe des buffers in byte
 * \param	close		Verbindung nach dem Request schliesen true/false
 * \return	void		( löst einen mgmt_avl interrupt aus, in der zg2100_on_mgmt_avl funktion verarbeitet und ausgewertet wird )
 */
/* -----------------------------------------------------------------------------------------------------------*/
void zg2100_mgmt_data( char * buffer, int len, bool close )
{
	zg2100_fifo_write( ZG2100_FIFO_DATA, ZG2100_FIFO_WR_MGMT_REQ, 0, buffer, len, false, close );
}

/* -----------------------------------------------------------------------------------------------------------*/
/*!\brief Setzt einen Paramter
 * \param 	parm		paramter der gesetzt werden soll
 * \param	buffer		zeiger auf den buffer mit paramter der mitgesendet werden soll
 * \param	len			größe des buffers in byte
 * \return	void		( löst einen mgmt_avl interrupt aus, in der zg2100_on_mgmt_avl funktion verarbeitet und ausgewertet wird )
 */
/* -----------------------------------------------------------------------------------------------------------*/
void zg2100_mgmt_set_param( char parm, char * buffer, int len )
{
	head_buffer[0] = 0;
	head_buffer[1] = parm;
	zg2100_mgmt_req( ZG2100_FIFO_MGMT_PARM_SET, head_buffer, 4 , false );
	zg2100_mgmt_data( buffer, len, true );
	current_mgmt_parm = parm;	

	while( zg2100_mgmt_is_busy() );
}

/* -----------------------------------------------------------------------------------------------------------*/
/*!\brief Holt einen Paramter
 * \param 	parm		paramter der abgefragt werden soll
 * \return	void		( löst einen mgmt_avl interrupt aus, in der zg2100_on_mgmt_avl funktion verarbeitet und ausgewertet wird )
 */
/* -----------------------------------------------------------------------------------------------------------*/
void zg2100_mgmt_get_parm( char parm )
{
	head_buffer[0] = 0;
	head_buffer[1] = parm;
	zg2100_mgmt_req( ZG2100_FIFO_MGMT_PARM_GET, head_buffer, 4, true );
	current_mgmt_parm = parm;

	while( zg2100_mgmt_is_busy() );
}

/* -----------------------------------------------------------------------------------------------------------*/
/*!\brief Interruptroutine für ein ankommendes Frame vom ZG2100
 * \param 	buffer		Pointer auf einen Puffer in dem das Frame liegt
 * \param 	len			Länge des Frames
 * \return	void
 */
/* -----------------------------------------------------------------------------------------------------------*/
void zg2100_on_rx( char * buffer, int len)
{

	rssi = (int)buffer[0];
	memcpy( buffer + 14, buffer + 2, 12);
	len -= 14;
	buffer += 14;
	ethernet_rx( buffer, len );

}

/* -----------------------------------------------------------------------------------------------------------*/
/*!\brief Sendet ein Ethernetframe über die ZG2100
 * \param 	buffer		Pointer auf einen Puffer in dem das Frame liegt
 * \param 	len			Länge des Frames
 * \return	void
 */
/* -----------------------------------------------------------------------------------------------------------*/
void zg2100_tx( char * buffer, int len )
{
	head_buffer[0] = ZG2100_CMD_FIFO_WR_DATA;
	head_buffer[1] = ZG2100_FIFO_WR_TXD_REQ;
	head_buffer[2] = ZG2100_FIFO_TXD_STD;
	head_buffer[3] = 0;
	head_buffer[4] = 0;
	zg2100_trans( head_buffer, 5, false, ZG2100_WRITE );
		buffer[6] = 0xaa;
	buffer[7] = 0xaa;
	buffer[8] = 0x03;
	buffer[9] = buffer[10] = buffer[11] = 0x00;
	zg2100_fifo_write( 0,0,0, buffer, len, false, true );
}

/* -----------------------------------------------------------------------------------------------------------*/
/*!\brief Holt einen Paramter
 * \param 	parm		paramter der abgefragt werden soll
 * \return	void		( löst einen mgmt_avl interrupt aus, in der zg2100_on_mgmt_avl verarbeitet wird )
 */
/* -----------------------------------------------------------------------------------------------------------*/
void zg2100_on_mgmt_avl( char subtype, char * buffer, int len )
{
	switch( subtype )
	{
		case ZG2100_FIFO_MGMT_SCAN:				if ( scan_buffer != NULL )
												{
													memcpy( scan_buffer, buffer, len );
													scan_len = len;
												}
												break; 
		case ZG2100_FIFO_MGMT_PARM_SET:			break;
		case ZG2100_FIFO_MGMT_PARM_GET:			zg2100_on_mgmt_get_parm( buffer, len );
												break;
		case ZG2100_FIFO_MGMT_CONNECT:			if( buffer[0]==1)
												{
													connected = true;
												}
												else
												{
													connected = false;
												}
												break;
		case ZG2100_FIFO_MGMT_PSK_CALC:			zg2100_on_mgmt_psk_key( buffer, len );
												break;
		default:								break;
	}
	mgmt_buffer = buffer;
	mgmt_len = len;
}

/* -----------------------------------------------------------------------------------------------------------*/
/*!\brief Holt einen Paramter
 * \param 	parm		paramter der abgefragt werden soll
 * \return	void		( löst einen mgmt_avl interrupt aus, in der zg2100_on_mgmt_avl verarbeitet wird )
 */
/* -----------------------------------------------------------------------------------------------------------*/
void zg2100_on_mgmt_get_parm( char * buffer, int len )
{
	if( buffer[0] != 1 )
		return;

	buffer += 4;
	len -= 4;
	
	switch( current_mgmt_parm )
	{
		case ZG2100_FIFO_MGMT_PARM_MACAD:		memcpy( macaddr, buffer, 6);
												break;
		default:								break;
	}
}

/* -----------------------------------------------------------------------------------------------------------*/
/*!\brief Eventmanager für ausgelöste Events
 * \param 	subtype		Subtype des Events
 * \param 	buffer		Pointer auf den Puffer mit dem Event
 * \param 	len			Länge des Puffer
 * \return	void
 */
/* -----------------------------------------------------------------------------------------------------------*/
void zg2100_on_mgmt_evt( char subtype, char * buffer, int len )
{
	
	switch( subtype )
	{
		case ZG2100_FIFO_MGMT_DISASSOC:		connected = false;
											break;
		case ZG2100_FIFO_MGMT_DEAUTH:		connected = false;
											break;
		case ZG2100_FIFO_MGMT_CONNECT:		
											if (buffer[0] == 1 || buffer[0] == 5 )
											{
												connected = false;
											}
											else if ( buffer[0] == 2|| buffer[0] == 6)
											{
												connected = true;
											}
											break;
		default:							break;
	}
}

/* -----------------------------------------------------------------------------------------------------------*/
/*!\brief Errechnet den PSK auf dem Passwort
 * \param 	buffer		Pointer auf dem Puffer in dem das Passwort steht
 * \param 	len			Länge des Passwortes
 * \return	void
 */
/* -----------------------------------------------------------------------------------------------------------*/
void zg2100_on_mgmt_psk_key( char * buffer, int len )
{
	ZG2100_PSK_CALC_RES * res = (ZG2100_PSK_CALC_RES*)buffer;
	
	if ( res->result != 1 || res->key_returned != 1 )
		return;

	memcpy( pmk_key, res->key, ZG2100_PMK_LEN );
}

/* -----------------------------------------------------------------------------------------------------------*/
/*!\brief Wartet auf die Bestätigung eines Managment-Request
 * \param 	void
 * \return	bool		True oder False
 */
/* -----------------------------------------------------------------------------------------------------------*/
bool zg2100_mgmt_is_busy( void )
{
	int i=0;
	
	while( mgmt_busy )
	{		
#if defined(LCD)
		i++;

		STDOUT_set( _LCD, 0);
		LCD_setXY( 0, 7, 4 );

		if ( i == 1 )
			printf_P( PSTR("-"));
		else if ( i == 2 )
			printf_P( PSTR("\\"));
		else if ( i == 3 )
			printf_P( PSTR("|"));
		else if ( i == 4 )
			printf_P( PSTR("/"));
		else
			i = 0 ;

		_delay_ms( 150 );
		
		STDOUT_set( RS232, 0);	
#endif
	};
	
/*	 printf_P( PSTR("MEMT_AVL eingetroffen: "));

	for(int i = 0 ; i < mgmt_len ; i++ )
	{
		printf_P( PSTR("%02x "), *mgmt_buffer);
		mgmt_buffer++;
	}
	printf_P( PSTR("\r\n"));
*/
	return mgmt_busy;
}

/* -----------------------------------------------------------------------------------------------------------*/
/*!\brief Setzt die SSID
 * \param 	ssid		Pointer auf die SSID
 * \return	void
 */
/* -----------------------------------------------------------------------------------------------------------*/
void zg2100_set_ssid( char * ssid )
{
	strcpy( m_ssid, ssid );
}

/* -----------------------------------------------------------------------------------------------------------*/
/*!\brief Errechnet den WEP-Key
 * \param 	key			Pointer auf den Key
 * \param	len			Länge des Key
 * \return	void
 */
/* -----------------------------------------------------------------------------------------------------------*/
void zg2100_set_wep_key( char * key, int len )
{
	ZG2100_WEP_REQ * req = ( ZG2100_WEP_REQ*)fifo_buffer;
	memset(req, 0, sizeof(ZG2100_WEP_REQ ));
	req->slot = 3;
	req->key_size = len;
	req->default_key = 0;
	strncpy(req->ssid, m_ssid, len );
	req->ssid_len = strlen(m_ssid);
	memcpy(req->keys, key, len );
	zg2100_mgmt_req( ZG2100_FIFO_MGMT_SET_WEP_KEY, fifo_buffer, sizeof(ZG2100_WEP_REQ ), true);
	zg2100_mgmt_is_busy();		
}

/* -----------------------------------------------------------------------------------------------------------*/
/*!\brief Errechnet den WPA-Key
 * \param 	pass		Pointer auf das Passwort
 * \return	void
 */
/* -----------------------------------------------------------------------------------------------------------*/
void zg2100_set_wpa_key( char * pass )
{
	ZG2100_PSK_CALC_REQ * req = (ZG2100_PSK_CALC_REQ*)fifo_buffer;
	memset(req, 0, sizeof(ZG2100_PSK_CALC_REQ));
	req->config = 0;
	req->phrase_len = strlen(pass);
	strncpy(req->ssid, m_ssid, ZG2100_SSID_LEN );
	req->ssid_len = strlen(m_ssid);
	strncpy(req->pass_phrase, pass, ZG2100_WPA_PASS_LEN );
	zg2100_mgmt_req( ZG2100_FIFO_MGMT_PSK_CALC, fifo_buffer, sizeof(ZG2100_PSK_CALC_REQ ), true );
	zg2100_mgmt_is_busy();

//	for(int i = 0 ; i < ZG2100_PMK_LEN; i++)
//		printf_P(PSTR("%02x "), pmk_key[i]);

	ZG2100_PMK_REQ * pmk_req = (ZG2100_PMK_REQ*)fifo_buffer;
	memset(pmk_req, 0, sizeof(ZG2100_PMK_REQ));

	pmk_req->slot = 0;
	strncpy( pmk_req->ssid, m_ssid, ZG2100_SSID_LEN );
	pmk_req->ssid_len = strlen(m_ssid);
	memcpy( pmk_req->key, pmk_key, ZG2100_PMK_LEN);

	zg2100_mgmt_req( ZG2100_FIFO_MGMT_PMK_KEY, fifo_buffer, sizeof( ZG2100_PMK_REQ ), true );
	zg2100_mgmt_is_busy();
	
}

/* -----------------------------------------------------------------------------------------------------------*/
/*!\brief Verbindet sich mit einen Funknetzwekt
 * \param 	bss_type	1 = Infra, 2 = ADHOC und 3 = ANY
 * \param 	security	0xff für autodetection
 * \return	char		1 = ok
 */
/* -----------------------------------------------------------------------------------------------------------*/
char zg2100_connect( char bss_type, char security )
{
	ZG2100_CONNECT_REQ * req = (ZG2100_CONNECT_REQ*)fifo_buffer;
	memset(req, 0, sizeof(ZG2100_CONNECT_REQ));
	req->security = security;
	strncpy(req->ssid, m_ssid, ZG2100_SSID_LEN);
	req->ssid_len = strlen(m_ssid);
	req->sleep_duration = 0;
	req->bss_type = bss_type;
	zg2100_mgmt_req(ZG2100_FIFO_MGMT_CONNECT, fifo_buffer, sizeof(ZG2100_CONNECT_REQ ), true );	
	zg2100_mgmt_is_busy();

	return(*mgmt_buffer);
}

/* -----------------------------------------------------------------------------------------------------------*/
/*!\brief Gibt den Interrupt für den Zg2100 frei
 * \param 	none
 * \return	none
 */
/* -----------------------------------------------------------------------------------------------------------*/
void zg2100_free( void )
{
	EXTINT_free ( &ZG2100_PORT, ZG2100_INT );
}

/* -----------------------------------------------------------------------------------------------------------*/
/*!\brief Sperrt den Interrupt für den Zg2100
 * \param 	none
 * \return	none
 */
/* -----------------------------------------------------------------------------------------------------------*/
void zg2100_lock( void )
{
	EXTINT_block ( &ZG2100_PORT, ZG2100_INT );
}

/* -----------------------------------------------------------------------------------------------------------*/
/*!\brief Holt die MAC-Adresse des ZG2100
 * \param 	char	Pointer auf einen 6-Byte Puffer für die MAC-Adresse
 * \return	none
 */
/* -----------------------------------------------------------------------------------------------------------*/
void zg2100_getmac( char * buffer )
{
	memcpy( buffer, macaddr, 6);
}

int zg2100_getstat( void )
{
	if ( connected == true )
		return( 1 );
	return(-1);
}

int zg2100_getrssi( void )
{
	return(rssi);
}

void zg2100_start_scan( void )
{
	char buffer[ZG2100_FIFO_BUFFER_SIZE];
	scan_buffer = buffer;
	ZG2100_SCAN_ITEM* it;
	
	ZG2100_SCAN_REQ * req = (ZG2100_SCAN_REQ*)fifo_buffer;
	memset(req, 0, sizeof( ZG2100_SCAN_REQ ));

	req->probe_delay = htons( ZG2100_SCAN_PROBE_DELAY );
	req->min_chan_time = htons( ZG2100_SCAN_MIN_CHAN_TIME );
	req->max_chan_time = htons( ZG2100_SCAN_MAX_CHAN_TIME );
	memset( req->bssid_mask , 0xff, ZG2100_MACADDR_LEN );
	req->bss_type = 3;
	req->probe_req = 1;
	req->ssid_len = 0;
	req->channel_count = ZG2100_SCAN_MAX_CHANNELS;
	for( int i = 0 ; i < ZG2100_SCAN_MAX_CHANNELS ; i++ )
	{
		req->channels[i] = i + 1;
		req->channels[i + 1] = 0;
	}

	zg2100_mgmt_req( ZG2100_FIFO_MGMT_SCAN, fifo_buffer, sizeof(ZG2100_SCAN_REQ) , true );
	zg2100_mgmt_is_busy();	

	if ( scan_len > 2 )
	{
		printf_P(PSTR("%d netzwerke gefunden\r\n"), scan_buffer[3]);
		
		it = (ZG2100_SCAN_ITEM*)(scan_buffer + 4);
		for( char i = 0 ; i < scan_buffer[3]; i++)
		{
			if ( it->ssid_len > 1 && it->ssid_len < 32 )
			{
				int y;

				printf_P(PSTR("["));
				for(y=0;y<5;y++)
					printf_P(PSTR("%02x:"), it->bssid[y]);
				printf_P(PSTR("%02x] "), it->bssid[y]);
					
				for( y = 0 ; y < it->ssid_len; y++)
				{
					printf_P(PSTR("%c"),it->ssid[y]);
				}
				for(;y<ZG2100_SSID_LEN;y++)
					printf_P(PSTR(" "));
			
				printf_P(PSTR("%2d,-%ddbm,"), it->channel,200 - it->rssi );
				if(it->apConfig&0x10)
					if(it->apConfig&0xc0)
					{	
						if((it->apConfig&0xC0)==0xc0)
							printf_P(PSTR("WPA/WPA2"));
						else if((it->apConfig&0x40)==0x40)
							printf_P(PSTR("WPA"));
						else if((it->apConfig&0x80)==0x80)
							printf_P(PSTR("WPA2"));
					}
					else
						printf_P(PSTR("WEP40/104"));					
				else
					printf_P(PSTR("OPEN"));
				printf_P(PSTR("\r\n"));
			}
			it++;
		}
	}
	scan_buffer = NULL;
	scan_len = 0;
}

#endif

/**
 * @}
 */
