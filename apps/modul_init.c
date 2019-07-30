/***************************************************************************
 *            modul_init.c
 *
 *  Mon Dec 14 16:54:30 2009
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

/**
 * \ingroup software
 * \addtogroup modules Initialisiert alle Module cmd/cgi (modul_init.c)
 *
 * @{
 */

/**
 * \file
 *
 * \author Dirk Broßwick
 */

#include <avr/pgmspace.h>
#include <avr/version.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "apps/telnet/telnet.h"
#include "config.h"

// hier kommen die includes der Befehle rein die Eingebunden werden
#include "apps/modules/cmd_dns.h"
#include "apps/modules/cmd_reset.h"
#include "apps/modules/cmd_arp.h"
#include "apps/modules/cmd_reset.h"
#include "apps/modules/cmd_stats.h"
#include "apps/modules/cmd_ifconfig.h"
#include "apps/modules/cmd_iwconfig.h"
#include "apps/modules/cmd_eemem.h"
#include "apps/modules/cmd_cron.h"

#include "apps/modules/cmd_adc.h"
#include "apps/modules/cmd_gpio.h"
#include "apps/modules/cmd_dns.h"
#include "apps/modules/cmd_ping.h"
#include "apps/modules/cmd_dyndns.h"
#include "apps/modules/cmd_ntp.h"
#include "apps/modules/cmd_twitter.h"
#include "apps/modules/cmd_twi.h"
#include "apps/modules/cmd_heater.h"
#include "apps/modules/cmd_onewire.h"
#include "apps/modules/cmd_tafel.h"
#include "apps/modules/cmd_stream.h"
#include "apps/modules/impulse.h"
#include "apps/modules/temp_json.h"
#include "apps/modules/cmd_temp.h"
#include "apps/modules/cmd_foo.h"
#include "apps/modules/cmd_mon.h"

void modul_init( void )
{
	// Befehle registrieren
	init_cmd_arp();
	init_cmd_ping();
	init_cmd_stats();
	init_cmd_ifconfig(),
	init_cmd_eemem();
	init_cmd_reset();
	init_cmd_cron();
	init_cmd_foo();
	
#if defined(NTP)
	init_cmd_ntp();
#endif
#if defined(DNS)
	init_cmd_dns();
	#if defined(DYNDNS)
		init_cmd_dyndns();
	#endif
#endif
#if defined(HTTPSERVER_TWITTER)
	init_cmd_twitter();
#endif
#if defined(ANALOG)
	init_cmd_adc();
#endif
#if defined(DIGITAL_IO)
	init_cmd_gpio();
#endif
#if defined(TWI)
	init_cmd_twi();
#endif
#if defined(ONEWIRE)
	init_cmd_onewire();
#endif
#if defined(LEDTAFEL) || defined(LEDTAFEL_PAR)
	init_cmd_tafel();
#endif
#if defined(HTTPSERVER_STREAM)
	init_cmd_stream();
#endif
#if defined(IMPULSCOUNTER)
	IMPULS_init();
#endif
#if ( ( defined(TWI) || defined(ONEWIRE) ) && defined(MMC) ) && defined( TEMP_LOGGER )
	temp_json_init();
#endif
#if defined(TWI) || defined(ONEWIRE) || defined(TEMP_LOGGER)
	init_cmd_temp();
#endif
#if defined( EtherSense )
	MON_init();
#endif
#if defined(USE_ZG2100)
	init_cmd_iwconfig();
#endif
#if defined(HEATERCONTROL)
	init_cmd_heater();
#endif
}

/**
 * @}
 */
