/***************************************************************************
 *            adc.c
 *
 *  Sun Mar  8 18:44:14 2009
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
 *
 * \ingroup Hardware
 * \addtogroup adc Funktionen für den ADC (adc.c)
 *
 * @{
 */

/**
 *
 * \author Dirk Broßwick
 *
 */

#include <avr/io.h>
#include "config.h"

#if defined(ANALOG)

#include "adc.h"

/*------------------------------------------------------------------------------------------------------------*/
/**
 * \brief Hier wird der ADC Initialisiert.
 * \param		NONE
 * \return		NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
void ADC_init( void )
{
#if defined(__AVR_XMEGA__)
	//ADC enable
	ADCA.CTRLA = ADC_ENABLE_bm;
	// Interne VCC/1.6
	ADCA.REFCTRL = ADC_REFSEL0_bm;
	// Vorteiler auf 8 
	ADCA.PRESCALER = ADC_PRESCALER0_bm;
	//input mode single ended Kanal 0, start
	ADCA.CH0.CTRL = ADC_CH_INPUTMODE0_bm | ADC_CH_START_bm;
	// set adcchannel 0
	ADCA.CH0.MUXCTRL = 0;

	//ADC enable
	ADCB.CTRLA = ADC_ENABLE_bm;
	// Interne VCC/1.6
	ADCB.REFCTRL = ADC_REFSEL0_bm;
	// Vorteiler auf 8 
	ADCB.PRESCALER = ADC_PRESCALER0_bm;
	//input mode single ended Kanal 0, start
	ADCB.CH0.CTRL = ADC_CH_INPUTMODE0_bm | ADC_CH_START_bm;
	// set adcchannel 0
	ADCB.CH0.MUXCTRL = 0;
#else
	// ADC einschalten
	ADCSRA = ( 1<<ADEN ) | ( 1<<ADPS2 ) ;
	// AREF extern an AREF
	ADMUX = ( 1<<REFS0 );

	// starte erste conversation
	ADCSRA |= ( 1<<ADSC );
		
	while( bit_is_set( ADCSRA , ADSC ) );
#endif
}

/*------------------------------------------------------------------------------------------------------------*/
/**
 * \brief Holt von eine ADC-Eingang den digitalisierten Wert
 * \param		Channel	 Der ADC-Eingang von den digitalisiert werden soll
 * \return		Der digitalisiert Wert.
 * \retval		Der digitalisiert Wert.
 */
/*------------------------------------------------------------------------------------------------------------*/
int ADC_GetValue( char Channel )
{
#if defined(__AVR_XMEGA__)
	int retval = 0;

	Channel &= 0x0f;
	
	if ( Channel < 8 )
	{
		// set adcchannel 0
		ADCA.CH0.MUXCTRL = ADC_CH_MUXPOS_gp << Channel;
		// start conversation
		ADCA.CH0.CTRL |= ADC_CH_START_bm;
		// wait for finish
		while( !ADCA.INTFLAGS );
		retval = ADCA_CH0RES ;
	}
	else
	{
		// set adcchannel 0
		ADCB.CH0.MUXCTRL = ADC_CH_MUXPOS_gp << ( Channel & 0x07 );
		// start conversation
		ADCB.CH0.CTRL |= ADC_CH_START_bm;
		// wait for finish
		while( !ADCB.INTFLAGS );
		retval = ADCB_CH0RES ;
	}
	return( retval );
#else
	#if defined(OpenMCP) || defined(AVRNETIO) || defined(UUP)
		Channel = Channel & 0x7;
	#elif defined(myAVR)
		Channel = 3;
	#endif

		ADMUX &= ~( 0x7 );
		ADMUX |= Channel;
		// starte erste conversation
		ADCSRA |= ( 1<<ADSC );
		while( bit_is_set( ADCSRA , ADSC ) );
	
		return( ADC );
#endif
}

/*------------------------------------------------------------------------------------------------------------*/
/**
 * \brief Rechnet einen Wert in Millivolt um.
 * \param		ADC_Wert		Der Wert der umgerechnet werden soll.
 * \param		ADC_mVoltmax	Der Maximalwert in den umgerechnet werden soll
 * \return		Wert in Millivolt.
 * \retval		Wert in Millivolt.
 */
/*------------------------------------------------------------------------------------------------------------*/
int ADC_mVolt( int ADC_Wert, int ADC_mVoltmax )
{
	long temp;
	
#if defined(__AVR_XMEGA__)
	// 12-Bit ADC conversation
	temp = ( ( ( long ) ADC_Wert ) * ADC_mVoltmax ) / 4000 ;
#else
	// 10-Bit ADC conversation
	temp = ( ( ( long ) ADC_Wert ) * ADC_mVoltmax ) / 1000 ;
#endif
	temp = temp - ( temp / 64 ) - ( temp / 128 );

	return( (int) temp );
}

#endif

/**
 * @}
 */
