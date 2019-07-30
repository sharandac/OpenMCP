
/*
 * Copyright (c) 2006-2009 by Roland Riegel <feedback@roland-riegel.de>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#ifndef SD_RAW_CONFIG_H
#define SD_RAW_CONFIG_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * \ingroup hardware
 * \addtogroup sd_raw
 *
 * @{
 */
/**
 * \file
 * MMC/SD support configuration (license: GPLv2 or LGPLv2.1)
 */

/**
 * \ingroup sd_raw_config
 * Controls MMC/SD write support.
 *
 * Set to 1 to enable MMC/SD write support, set to 0 to disable it.
 */
#define SD_RAW_WRITE_SUPPORT 1

/**
 * \ingroup sd_raw_config
 * Controls MMC/SD write buffering.
 *
 * Set to 1 to buffer write accesses, set to 0 to disable it.
 *
 * \note This option has no effect when SD_RAW_WRITE_SUPPORT is 0.
 */
#define SD_RAW_WRITE_BUFFERING 0

/**
 * \ingroup sd_raw_config
 * Controls MMC/SD access buffering.
 * 
 * Set to 1 to save static RAM, but be aware that you will
 * lose performance.
 *
 * \note When SD_RAW_WRITE_SUPPORT is 1, SD_RAW_SAVE_RAM will
 *       be reset to 0.
 */
#define SD_RAW_SAVE_RAM 1

/**
 * \ingroup sd_raw_config
 * Controls support for SDHC cards.
 *
 * Set to 1 to support so-called SDHC memory cards, i.e. SD
 * cards with more than 2 gigabytes of memory.
 */
#define SD_RAW_SDHC 0

/**
 * @}
 */

/* defines for customisation of sd/mmc port access */

// defines for the MMC-interface on myAVR
#if defined(myAVR)

		#define spi_bus_num 						1
		#define configure_pin_ss() 					DDRC |= (1 << DDC6)	
		#define ss_PIN 								PC6
		#define ss_PORT								PORTC
		#define get_pin_available()					0
		#define get_pin_locked()					1
	
#elif defined(ATXM2)

		#define spi_bus_num 						1
		#define configure_pin_ss() 					PORTD.DIRSET |= (1 << 4)	
		#define ss_PIN 								4
		#define ss_PORT								PORTD.OUT
		#define get_pin_available()					0
		#define get_pin_locked()					1

// defines for the AVR-NETIO addon MMC-interface and the OpenMCP MMC-addon on AVR-NETIO
#elif defined(AVRNETIO)
	#if defined(AVRNETIO_ADDON)

		#define spi_bus_num 0
		#define configure_pin_ss() 					DDRB |= (1 << DDD3)	
		#define ss_PIN 								PB3
		#define ss_PORT								PORTB
		#define configure_pin_available() 			DDRD &= ~(1 << DDD7)	
		#define configure_pin_available_pullup() 	PORTD |= (1 << PD7)
		#define get_pin_available() 				0
		#define get_pin_locked() 					1
	
	#elif defined(NONE_ADDON)

		#define spi_bus_num 1
		#define configure_pin_ss() 					DDRD |= (1 << DDD3)	
		#define ss_PIN 								PD3
		#define ss_PORT								PORTD
		#define configure_pin_available() 			DDRD &= ~(1 << DDD6)	
		#define configure_pin_available_pullup() 	PORTD |= (1 << PD6)
		#define get_pin_available() 				( (PIND >> PD6) & 0x01)
		#define get_pin_locked() 					(~(PINB >> PB3) & 0x01)
	
		#define configure_pin_locked() 				DDRB &= ~(1 << DDB3)
		#define configure_pin_locked_pullup() 		PORTB |= (1 << PB3)
		#define configure_power_up()				DDRD |= ( 1 << DDD7 )
		#define power_up()							PORTD |= (1 << PD7 )

	#else
	
		#error "None MMC-interface defined."

	#endif

#else

	#error "Hardwareplattform doesn't support MMC-interface!"

#endif
		
#if SD_RAW_SDHC
    typedef uint64_t offset_t;
#else
    typedef uint32_t offset_t;
#endif

/* configuration checks */
#if SD_RAW_WRITE_SUPPORT
#undef SD_RAW_SAVE_RAM
#define SD_RAW_SAVE_RAM 0
#else
#undef SD_RAW_WRITE_BUFFERING
#define SD_RAW_WRITE_BUFFERING 0
#endif

#ifdef __cplusplus
}
#endif

#endif

