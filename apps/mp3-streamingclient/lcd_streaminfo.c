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

#include "config.h"

#include "system/thread/thread.h"
#include "system/net/udp.h"
#include "system/stdout/stdout.h"
#include "system/thread/thread.h"
#include "udp-stream.h"
#include "vs10xx_buffer.h"

#if defined(LCD)
	#include "hardware/lcd/lcd.h"
#endif

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Init um den Streamingdienst zu starten.
 * \return	NONE		None
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
void streaminfo_init( void )
{	
	THREAD_RegisterThread( udp_stream_thread, PSTR("lcd-streaminfo"));
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Thread für ausgaben auf den Display.
 * \return	NONE		None
 */
/* -----------------------------------------------------------------------------------------------------------*/ 
void udp_stream_thread( void )
{
	int fifo_size, fps, i;

	fifo_size = streambuffer_get(14);
	fps = streambuffer_getfps();

#if defined(LCD)

	struct STDOUT oldstream;

	STDOUT_save( &oldstream );
	STDOUT_set( _LCD , 0 );


	LCD_setXY(0,0,2);
	printf_P(PSTR("Title:"));
	LCD_setXY(0,0,4);
	printf_P(PSTR("Source:"));

	LCD_setXY(0,0,6);
	if ( streambuffer_getstate() == 0 )
		printf_P(PSTR("stop            "), fps);
	else
		printf_P(PSTR("playing   %3dkbs"), fps);
		
	LCD_setXY(0,0,7);
	printf_P(PSTR("["));
	for ( i = 0 ; i < fifo_size ; i++ )
		printf_P( PSTR("%c"),0xdb);

	for ( ; i < 14 ; i++ )
		printf_P( PSTR(" "));

	printf_P(PSTR("]"));
	STDOUT_Flush();
	STDOUT_restore( &oldstream );		

#endif
}
