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

// This is AVRNETIO.config.h

#ifndef AVRNETIO_CONFIG_H
	#define AVRNETIO_CONFIG_H

	// set the baudrate for the usart
	#define UART_BAUD 9600L

	// Define network config if DHCP fails these are the values that will be used as a fallback or when DHCP disable
	#define MYIP			IPDOT( 192l, 168l, 178l, 111l )
	#define NETMASK 		IPDOT( 255l, 255l, 255l, 0l )
	#define GATEWAY			IPDOT( 192l, 168l, 178l, 1l )
	#define DNSSERVER		IPDOT( 192l, 168l, 178l, 1l )

	// aktiviert 1-Wire
	#define ONEWIRE 
	// aktiviert ADC
	#define ANALOG
	// aktiviert GPIO
	#define DIGITAL_IO
	// aktiviert EXTINT
	#define EXTINT
	// aktiviert PCint
	#define PC_INT
	// aktiviert TWI
	#define TWI
	// aktiviert MMC
	#define MMC
	// aktiviert LED
	#define LED
	// aktivier den DCF77 empfänger
	#define DCF77

	// aktiviert den Impulscounter und logger
	#define IMPULSCOUNTER
	// aktiviert den Templogger
	#define TEMP_LOGGER

	// #define HEATER_CONTROL

	#define LCD
	#ifdef LCD
		// Support for HD44780 display
		#define HD44780_TWI

		#define HD44780_TWI_ROWS	2
		#define HD44780_TWI_COLUMNS	16

		// Support for KS0073 display
		// #define KS0073_TWI

		// #define KS0073_TWI_ROWS	2
		// #define KS0073_TWI_COLUMNS	16
	#endif

	// aktiviert TCP
	#define TCP
	#ifdef TCP
		// empfängt und sendet RTT im TCP-Header mit um die Performace zu verbessern
		#define TCP_RTT
		// Erlaubt den Stack ein Packet zwischen zu speichern wenn sie in der Falschen reihenfolge kommen
		#define TCP_with_unsortseq
		// Beschleunigt TCP in Verbindung mit Windows
		#define TCP_delayed_ack
	#endif

	// aktiviert UDP
	#define UDP
	
	#ifdef UDP
		// aktiviert DHCP, dazu muss UDP aktiv sein
		#define DHCP
		// Versucht bei einem Fehlversuch von DHCP die IP aus der Config zu lesen
		#define READ_CONFIG
		// aktiviert DNS, dazu muss UDP aktiv sein
		#define DNS
		// aktiviert NTP, dazu muss UDP aktiv sein
		#define NTP
		// aktiviert den TFTP-Server
		#define TFTPSERVER
	#endif

	#ifdef TCP

		#define SHELL
		#ifdef SHELL
			// aktiviert den Telnet-Server, dazu muss TCP aktiv sein
			#define TELNETSERVER
		#endif

		// aktiviert den HTTP-Server, dazu muss TCP aktiv sein
		#define HTTPSERVER
		#ifdef HTTPSERVER
			#if defined(MMC)// holt Dateien vom HTTP-Server nur von der MMC
				#define HTTP_FILES_FROM_MMC
			#endif
			// aktiviert die Stats per Webinterface
			#define HTTPSERVER_STATS
			// aktiviert die Konfiguration per Webinterface
			#define HTTPSERVER_NETCONFIG

			// aktiviert das System-Menü per Webinterface
			#define HTTPSERVER_SYSTEM
			#ifdef HTTPSERVER_SYSTEM
				#ifdef DYNDNS
					#define HTTPSERVER_DYNDNS
				#endif
				// aktiviert twitter per Webinterface
				#if defined(TWITTER)
					#define HTTPSERVER_TWITTER
				#endif
				// aktiviert ntp per Webinterface
				#if defined(NTP)
					#define HTTPSERVER_NTP
				#endif
				// aktiviert eemem per Webinterface
				#define HTTPSERVER_EEMEM
				// aktiviert reset per Webinterface
				#if defined(SHELL)
					#define HTTPSERVER_CRON
				#endif
				// aktiviert reset per Webinterface
				#define HTTPSERVER_RESET
			#endif

			// aktiviert IO auf dem Webinterface
			#define HTTPSERVER_IO
			#ifdef HTTPSERVER_IO
				// aktiviert ADC auf dem Webinterface
				#ifdef ANALOG
					#define HTTPSERVER_ANALOG
				#endif
				// aktiviert GPIO auf dem Webinterface
				#ifdef DIGITAL_IO
					#define HTTPSERVER_DIGITAL_IO
				#endif
				// aktiviert 1-Wire auf dem Webinterface
				#ifdef ONEWIRE
					#define HTTPSERVER_ONEWIRE
				#endif
				// aktiviert TWI auf dem Webinterface
				#ifdef TWI
					#define HTTPSERVER_TWI
				#endif
				// aktivert Impulsecounter per Webinterface
				#ifdef IMPULSCOUNTER
					#define HTTPSERVER_IMPULSECOUNTER
				#endif
			#endif
		#endif
	#endif

#endif /* AVRNETIO_CONFIG_H */
