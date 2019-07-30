/***************************************************************************
 *            lcd_info.c
 *
 *  Wed Jun  17 23:18:31 2009
 *  Copyright  2009  Dirk Broßwick
 *  Email
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
 *
 * \ingroup modules
 * \addtogroup LCD_info Der LCD_infodienst (lcd_info.c)
 *
 * @{
 */

/**
 *
 * \author Dirk Broßwick
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/pgmspace.h>
#include "config.h"

#if defined(LCD)

#include "hardware/lcd/dogl128w6.h"
#include "system/clock/clock.h"
#include "system/stdout/stdout.h"
#include "system/net/ip.h"
#include "system/thread/thread.h"
#include "hardware/lcd/lcd.h"
#include "hardware/twi/twi.h"
#include "apps/modules/impulse.h"
#include "system/sensor/temp.h"
#include "lcd_info.h"

// struct TIME lasttime;

static char lasttime_SS;
static char lasttime_MM;

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Die Initfunktion für den Cron-Dienst.
 * \param 	NONE
 * \return	NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
void LCDINFO_init( void )
{
	struct TIME nowtime;

	CLOCK_GetTime ( &nowtime );

	lasttime_SS = nowtime.ss;
	
	THREAD_RegisterThread( LCDINFO_thread, PSTR("LCD-Info"));
}

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Diese Funktion sollte zyklisch ausgerufen werden. Hier wird überprüft ob ein Cron ausgeführt werden soll.
 * \param 	NONE
 * \return	NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
void LCDINFO_thread( void )
{

	 char ip[32];
#if defined(TWI) || defined(ONEWIRE)
	char TempString[10];
	static char i=0;
#endif
#if defined(IMPULSCOUNTER)
	long PreScaler;
#endif


	static int LCD_info_Counter=0;
	
	struct TIME nowtime;
	struct STDOUT oldstream;

	CLOCK_GetTime ( &nowtime );
	
	if ( lasttime_SS != nowtime.ss )
	{
		lasttime_SS = nowtime.ss;

		STDOUT_save( &oldstream );
		STDOUT_set( _LCD , 0 );

		LCD_setXY(0,0,0);

		if ( lasttime_MM != nowtime.mm )
		{
			printf_P( PSTR( "Zeit: %02d:%02d    \r\n"), nowtime.hh, nowtime.mm );
			lasttime_MM = nowtime.mm;
		}

		LCD_setXY(0,0,1);

		if ( !(nowtime.ss%5) )
		{
			LCD_info_Counter++;

			printf_P( PSTR("                \r"));
			
			switch( LCD_info_Counter )
			{
				case	1:		
#if defined(TWI) || defined(ONEWIRE)
								for ( ; i < TEMP_MAX_SENSORS ; i++ )
								{
									if ( TEMP_readtemp( i ) == TEMP_ERROR )
										i++;
									else
									{
										TEMP_Sensor2String( i , TempString );
										printf_P( PSTR("Temp %d: %sC"), i, TempString );
										i++;
										break;
									}
								}
								if ( i >= TEMP_MAX_SENSORS ) i = 0;				
#endif
								break;
				case	2:		printf_P( PSTR("Uptime: %lds"), nowtime.uptime );
								break;
				case	3:		printf_P( PSTR("%s"), iptostr( myIP, ip ) );
								break;
				case	4:		
#if defined(IMPULSCOUNTER)
								printf_P( PSTR("Impulszaehler:"));
#endif
								break;
				case	5:		
#if defined(IMPULSCOUNTER)
								PreScaler = IMPULS_getPrescaler();
								if ( IMPULS_getUnit( ip ) == NULL )
									ip[0] = '\0';
								printf_P( PSTR("%ld,%03ld %s"), IMPULS_getCounter() / PreScaler, ( ( 1000 / PreScaler ) * ( IMPULS_getCounter() % PreScaler ) ), ip );
#endif
								break;
				case	6:		printf_P( PSTR( "Date: %u.%u.%02u  "), nowtime.DD, nowtime.MM, nowtime.YY % 100 );
								break;
				default:		LCD_info_Counter = 0;
								printf_P( PSTR("Keine Infos."));
			}
		}
		STDOUT_Flush();
		STDOUT_restore( &oldstream );		
	}
}

#endif

/**
 * @}
 */
