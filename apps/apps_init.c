/***************************************************************************
 *            apps_init.c
 *
 *  Sat Dec 19 23:32:24 2009
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
#include "system/thread/thread.h"
#include "apps/cron/cron.h"
#include "apps/apps_init.h"
#include "apps/telnet/telnet.h"
#include "apps/httpd/httpd2.h"

#include "config.h"

#if defined(LCD)
	#if defined(HD44780)
		#include "apps/modules/lcd_info.h"
	#endif
	#if defined(HTTPSERVER_STREAM) & defined(DOGL128W6)
		#include "apps/mp3-streamingclient/streaminfo.h"
	#endif
#endif

#if defined(HTTPSERVER_STREAM)
	#include "apps/mp3-streamingclient/mp3-streaming.h"
	#include "apps/mp3-streamingclient/udp-stream.h"
#endif

#if defined(TEMP_LOGGER)
	#include "system/sensor/temp.h"
#endif

#if defined(TFTPSERVER)
	#include "apps/tftp-server/tftp-server.h"
#endif

#if defined(CAN)
	#include "apps/can/can_eth_rep.h"
#endif


void apps_init( void )
{
	// Cron-Dienst starten
	CRON_init();

	CRON_reloadcrontable();

	// Dienste starten
	#ifdef HTTPSERVER
		httpd_init();
	#endif
	#if defined(TELNETSERVER)
		telnet_init();
	#endif
	#if defined(HTTPSERVER_STREAM)
		mp3client_init();
		udp_stream_init ();
		#if defined(DOGL128W6)
			streaminfo_init();
		#endif
	#endif
	#if defined(TFTPSERVER)
		TFTPSERVER_init();
	#endif
	#if defined(LCD) & defined(HD44780)
		LCDINFO_init();
	#endif
	#if defined(TEMP_LOGGER)
		TEMP_init();
	#endif
	#if defined(CAN)
		CANETHREP_init();
	#endif
}
