/***************************************************************************
 *            init.c
 *
 *  Sun Mar 15 03:39:39 2009
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
 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include <avr/version.h>
#include "config.h"
#include "hardware/network/enc28j60.h"
#include "hardware/uart/uart_core.h"
#include "apps/cron/cron.h"
#include "system/buffer/fifo.h"
#include "system/clock/clock.h"
#include "system/net/network.h"
#include "system/stdout/stdout.h"
#include "system/config/eeconfig.h"
#include "system/thread/thread.h"
#include "system/shell/shell.h"

#if defined(EXTMEM)
	#include "hardware/memory/xram.h"
#endif
#if defined(ANALOG)
	#include "hardware/adc/adc.h"
#endif
#if defined(CAN)
	#include "hardware/can/Can_Declaration.h"
#endif
#if defined(DIGITAL_IO)
	#include "hardware/gpio/gpio_out.h"
	#include "hardware/gpio/gpio_in.h"
#endif
#if defined(ONEWIRE)
	#include "hardware/1wire/1wire.h"
#endif
#if defined(TWI)
	#include "hardware/twi/twi.h"
#endif
#if defined(LCD)
	#include "hardware/lcd/lcd.h"
#endif
#if defined(MMC)
	#include "hardware/sd_raw/sd_raw.h"
	#include "system/filesystem/filesystem.h"
#endif
#if defined(PC_INT)
	#include "hardware/pcint/pcint.h"
#endif
#if defined(EXTINT)
	#include "hardware/ext_int/ext_int.h"
#endif
#if defined(LEDTAFEL)
	#include "hardware/led-tafel/led_tafel.h"
#endif
#if defined(LEDTAFEL_PAR)
	#include "hardware/led-tafel/led_tafel-par.h"
#endif
#if defined(LED)
	#include "hardware/led/led_core.h"
#endif
#if defined(VS10XX)
	#include "hardware/vs10xx/vs10xx.h"
#endif
#if defined( RFM12 )
	#include "hardware/rfm12/rfm12.h"
#endif
#if defined(_AVR_XMEGA__)
	#include "hardware/cpu_freq/cpu_freq.h"
#endif
#if defined(DCF77)
	#include "hardware/dcf77/dcf77.h"
#endif
#if defined(KEYBOARD)
	#include "hardware/keyboard/keyboard.h"
#endif
#if defined( ENCODER_P2W1 )
	#include "hardware/keyboard/encoder_p2w1.h"
#endif
#if defined(HH10D)
	#include "hardware/twi/hh10d.h"
#endif

const char config_ok[] PROGMEM = " Initialisiert\r\n";
const char config_error[] PROGMEM = " Error\r\n";

#if defined(LED)
	// Timerinterrupt mit abstand 1 sekunde, dabei wird die LED 0 getoggelt
	void blinkinglights( void )
	{
		LED_toggle(0);
	}
#endif

void init( void )
{
 	// Interrupts freigeben
	sei();

	INIT_FIFO();

	// RS232 starten und printf auf RS232 verbiegen
	UART_init( 0 );

	STDOUT_INIT ();
	STDOUT_set( RS232, 0 );

	printf_P( PSTR("OpenMCP ...\r\n") );

	// Uart einrichten
	printf_P( PSTR("UART"));
	printf_P( config_ok );

	// Standart Out einrichten
	printf_P( PSTR("STDOUT"));
	printf_P( config_ok );

	// Clock starten
	CLOCK_init();
	printf_P( PSTR("CLOCK"));
	printf_P( config_ok );

#if defined(VS10XX)
	// VS10xx starten
	VS10xx_INIT();
#endif

#if defined(LEDTAFEL) || defined(LEDTAFEL_PAR)
	LEDTAFEL_init();
//	STDOUT_set( TAFEL, 0);
#endif
	
	// LED-Modul starten
#if defined(LED)
	LED_init();
	printf_P( PSTR("LED_core"));
	printf_P( config_ok );
	// LED_core starten
	// Callbackfunktion für die blinkende LED eintragen
//	LED_on(0);
//	CLOCK_RegisterCallbackFunction ( blinkinglights, SECOUND );
#endif

	// Configmodul initialisieren
	Config_Init ();
	printf_P( PSTR("Config"));
	printf_P( config_ok );

	// Externe Interrupt aktivieren
#if defined(EXTINT)
	EXTINT_init();
	printf_P( PSTR("EXTINT"));
	printf_P( config_ok );
#endif

	// PinChange Interrupt aktivieren
#if defined(PC_INT)
	PCINT_init();
	printf_P( PSTR("PCINT"));
	printf_P( config_ok );
#endif

// TWI aktivieren
#if defined(TWI)
	printf_P( PSTR("TWI"));

	if ( TWI_Init( 100000 ) == TRUE )
	{
		printf_P( PSTR(" (%d Devices)"), TWI_Scan() );
		printf_P( config_ok );
	}
	else
		printf_P( config_error );
#endif
	
// LCD initialisieren wenn in Config angewählt und wenn wir auf einen myEthernet laufen
#if defined(LCD)
		LCD_init( 0 );
	
		printf_P( PSTR("LCD"));
		printf_P( config_ok );
	
		STDOUT_set( _LCD, 0);

		LCD_setXY( 0, 0, 0 );
		printf_P( PSTR("OpenMCP ...\r"));
#if defined(USE_ZG2100)
		LCD_setXY( 0, 2, 2 );
		printf_P( PSTR("connect2wifi\r"));
#endif		
		LCD_backlight( 0, 255 );
	
		STDOUT_set( RS232, 0);	
#endif

	// GPIO und ADC aktivieren
#if defined(DIGITAL_IO)
	GPIO_out_init();
	GPIO_in_init();
	printf_P( PSTR("GPIO"));
	printf_P( config_ok );
#endif

#if defined(ANALOG)
	ADC_init();
	printf_P( PSTR("ADC"));
	printf_P( config_ok );
#endif

#if defined ( CAN )
	canInit();
	printf_P( PSTR("CAN"));
	printf_P( config_ok );
#endif

#ifdef ONEWIRE
	printf_P( PSTR("1-WIRE"));
	if ( ONEWIRE_reset() )
		printf_P( config_error );
	else
		printf_P( config_ok );	
#endif

#if defined(DCF77)
	DCF77_init();
	printf_P( PSTR("DCF77"));
	printf_P( config_ok );
#endif

#if defined(KEYBOARD)
	KEYBOARD_init();
	printf_P( PSTR("KEYBOARD"));
	printf_P( config_ok );
#endif

#if defined( ENCODER_P2W1 )
	ENCODER_P2W1_init();
	printf_P( PSTR("Encoder P2W1"));
	printf_P( config_ok );
#endif

	// MCC-Interface aktivieren
#if defined(MMC)
	printf_P( PSTR("MMC/SD"));
	if(!sd_raw_init())
	{
		printf_P( config_error );			
	}
	else
	{
		printf_P( config_ok );
		printf_P( PSTR("Filesystem"));
		if ( FILE_init() != FILESYSTEM_OK )
			printf_P( config_error );
		else
			printf_P( config_ok );
	}
#else
	#if defined(myAVR)
		// muss auf High stehen, sonst kann es sein wenn MMC nicht aktiv ist und eine MMC steckt das der SPI-Bus blödsinn macht
		DDRC |= (1<<PC6);
		PORTC |= (1<<PC6);
	#endif
#endif

#if defined(HH10D)
	HH10D_init();
#endif
	
#if defined( RFM12 )
	RFM12_init( );
	printf_P( PSTR("RFM12") );
//	if ( RFM12_rxstart() == 0 )
//		printf_P( PSTR("all fine"));
	printf_P( config_ok );
#endif

#if defined(SHELL)
	SHELL_init();
	printf_P( PSTR("SHELL"));
	printf_P( config_ok );
#endif
	
	// Threadliste init
	THREAD_init();
	printf_P( PSTR("THREAD"));
	printf_P( config_ok );

	// Netzwerk starten
	network_init();
	
#if defined(LEDTAFEL) || defined(LEDTAFEL_PAR)
	LEDTAFEL_fadeout_display( 16 , NULL );
	LEDTAFEL_printlogo();
	STDOUT_set( RS232, 0);
#endif
}
