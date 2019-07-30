/***************************************************************************
 *            zg2100_driver.h
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

#ifndef _ZG2100_DRIVER_H
	#define _ZG2100_DRIVER_H

	#include <stdbool.h>

	void zg2100_drv_init( void );
	void zg2100_int( void );
	void zg2100_process( void );
	void zg2100_mgmt_req( char subtype, char * buffer, int len, bool close );
	void zg2100_mgmt_data( char * buffer, int len, bool close );
	void zg2100_mgmt_set_param( char parm, char * buffer, int len );
	void zg2100_mgmt_get_parm( char parm );
	void zg2100_on_mgmt_get_parm( char * buffer, int len );
	void zg2100_on_rx( char * buffer, int len);
	void zg2100_tx( char * buffer, int len );
	void zg2100_on_mgmt_avl( char subtype, char * buffer, int len );
	void zg2100_on_mgmt_evt( char subtype, char * buffer, int len );
	bool zg2100_mgmt_is_busy( void );
	void zg2100_set_ssid( char * ssid );
	void zg2100_on_mgmt_psk_key( char * buffer, int len );
	void zg2100_set_wep_key( char * key, int len );
	void zg2100_set_wpa_key( char * pass );
	char zg2100_connect( char bss_type, char security );
	void zg2100_free( void );
	void zg2100_lock( void );
	int zg2100_getstat( void );
	int zg2100_getrssi( void );
	void zg2100_getmac( char * buffer );
	void zg2100_start_scan( void );


#endif /* _ZG2100_DRIVER_H */