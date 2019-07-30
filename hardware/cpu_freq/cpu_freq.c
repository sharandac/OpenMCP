/***************************************************************************
 *            cpu_freq.c
 *
 *  Sun May 23 18:58:14 2010
 *  Copyright  2010  Dirk Broßwick
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
 * \addtogroup cpu_freq Treiber zum einstellen der Taktfrequenz einer ATXMEGA-CPU (cpu_freq.c)
 *
 * @{
 */

/**
 * \file
 *
 * \author Dirk Broßwick
 */

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "config.h"

#if defined(__AVR_XMEGA__)

/*!\brief Stelle die in der XXXconfig.h-Datei eingestellten Frequenzen ein
 * \param 	none
 * \return	none
 */

void __attribute__ ((naked, section(".init2"))) init_cpu_freq(void)
{	
	// System Clock initialisieren
#if defined(EXT_XTAL)
	
	#if EXT_XTAL >= 12000000
	// external OSC between 12 and 16 MHz
	// external OSC needs 16K CLKs to startup
	CCP = CCP_IOREG_gc; //Security Signature to modify clock
	OSC.XOSCCTRL  = OSC_FRQRANGE_12TO16_gc | OSC_XOSCSEL_XTAL_16KCLK_gc;
	#elif EXT_XTAL >= 9000000
	// external OSC between 9 and 12 MHz
	// external OSC needs 16K CLKs to startup
	CCP = CCP_IOREG_gc; //Security Signature to modify clock
	OSC.XOSCCTRL  = OSC_FRQRANGE_9TO12_gc | OSC_XOSCSEL_XTAL_16KCLK_gc;
	#elif EXT_XTAL >= 2000000
	// external OSC between 2 and 9 MHz
	// external OSC needs 16K CLKs to startup
	CCP = CCP_IOREG_gc; //Security Signature to modify clock
	OSC.XOSCCTRL  = OSC_FRQRANGE_2TO9_gc | OSC_XOSCSEL_XTAL_16KCLK_gc;
	#else
	// external OSC between 0.4 and 2 MHz
	// external OSC needs 16K CLKs to startup
	CCP = CCP_IOREG_gc; //Security Signature to modify clock
	OSC.XOSCCTRL  = OSC_FRQRANGE_04TO2_gc | OSC_XOSCSEL_XTAL_16KCLK_gc;
	#endif	
	// enable external OSk
	CCP = CCP_IOREG_gc; //Security Signature to modify clock
	OSC.CTRL |= OSC_XOSCEN_bm;  
	// wait until stable
	while(!(OSC.STATUS & OSC_XOSCRDY_bm));

	// 2. Setup and enable the PLL
	// select external OSC as source 
	// multiplicator is EXT_XTAL_MULTI (resulting in 64 MHz on 32MHz CPU_CLK)
	CCP = CCP_IOREG_gc; //Security Signature to modify clock
	OSC.PLLCTRL = OSC_PLLSRC_XOSC_gc | EXT_XTAL_MULTI;
	// enable PLL
	CCP = CCP_IOREG_gc; //Security Signature to modify clock
	OSC.CTRL |= OSC_PLLEN_bm;
	// 3. Setup the clock prescalers
	// Note this operation is protected by the CCP register
	// clock divider A: 1, B: 1, C: 2
	CCP = CCP_IOREG_gc;
	// ClkCPU = 32Mhz; ClkPer2 = 64 MHz
	CLK.PSCTRL = (CLK_PSADIV_1_gc | CLK_PSBCDIV_1_1_gc);
	
#else

	// 32MHz/4 * 8 = 64MHz
	CCP = CCP_IOREG_gc; //Security Signature to modify clock
	OSC.PLLCTRL    = OSC_PLLSRC_RC32M_gc | OSC_PLLFAC3_bm;
	// 32MHz Rc kalibrieren
	CCP = CCP_IOREG_gc; //Security Signature to modify clock
	OSC.DFLLCTRL   = OSC_RC32MCREF_bm;
	// Kalibrierung ein
	CCP = CCP_IOREG_gc; //Security Signature to modify clock
	DFLLRC32M.CTRL = DFLL_ENABLE_bm;

	// 32M OSC einschalten
	CCP = CCP_IOREG_gc; //Security Signature to modify clock
	OSC.CTRL |= OSC_RC32MEN_bm;
	// warten bis stabil
	while(!(OSC.STATUS & OSC_RC32MRDY_bm));

	// PLL mit 64Mhz ein
	CCP = CCP_IOREG_gc; //Security Signature to modify clock
	OSC.CTRL |= OSC_PLLEN_bm ;
	uint8_t CTRLn  = (CLK.PSCTRL & (~(CLK_PSADIV_gm | CLK_PSBCDIV1_bm))) | CLK_PSADIV_1_gc | CLK_PSBCDIV_1_2_gc;
	// ClkCPU = 32Mhz; ClkPer2 = 64 MHz
	CCP = CCP_IOREG_gc;
	// Prescaler setzen
	CLK.PSCTRL = CTRLn;

#endif

	// warten bis PLL stabil
	while(!(OSC.STATUS & OSC_PLLRDY_bm));
	// Sysclock über PLL
	CCP = CCP_IOREG_gc;
	CLK.CTRL = CLK_SCLKSEL_PLL_gc;

#if defined(EXT_RTCXTAL)  
	// RTCEN (0x01) RTC clock enable  
	// TOSC32 32 kHz from 32 kHz Crystal OSC on TOSC
	CCP = CCP_IOREG_gc;
	CLK.RTCCTRL = CLK_RTCSRC_TOSC32_gc | CLK_RTCEN_bm;
#else
	// internen 32K OSC einschalten
	CCP = CCP_IOREG_gc; //Security Signature to modify clock
	OSC.CTRL |= OSC_RC32KEN_bm;
	// warten bis stabil
	while(!(OSC.STATUS & OSC_RC32KRDY_bm));
#endif
	
	/* Enable PMIC interrupt level low, med and high. */
	PMIC.CTRL |= PMIC_LOLVLEX_bm | PMIC_MEDLVLEX_bm | PMIC_HILVLEX_bm ;
}
#endif

/**
 * @}
 */
