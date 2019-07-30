/***************************************************************************
 *            adc.h
 *
 *  Sun Mar  8 18:45:01 2009
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

#ifndef _ADC_H
	#define _ADC_H
	
	void ADC_init( void );
	int ADC_GetValue( char channel );
	int ADC_mVolt( int ADC_Wert, int ADC_mVoltmax );

#if defined(AVRNETIO) || defined(OpenMCP) || defined(UPP) || defined( EtherSense )
		#define MAX_ADC 	8
#elif defined(myAVR)
		#define MAX_ADC 	1
#elif defined(XPLAIN)
		#define MAX_ADC 	16
#elif defined(ATXM2)
		#define MAX_ADC 	16
#endif

#endif /* _ADC_H */
