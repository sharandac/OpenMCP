/***************************************************************************
 *            ext_int.c
 *
 *  Mon Jul 31 21:46:47 2006
 *  Copyright  2006  Dirk Broßwick
 *  Email: sharandac@snafu.de
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
 * \addtogroup ext_int externe Interrupts mit Callback (ext_int.c)
 * \code #include "ext_int.h" \endcode
 * 
 * @{
 */

/**
 *
 * \author Dirk Broßwick
 *
 */


#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>


#include "config.h"

#if defined(EXTINT)

#include "hardware/ext_int/ext_int.h"

EXT_INT_CALLBACK_FUNC extint_CallBack_Table[ MAX_EXT_INT ];

void EXTINT_init( void )
{
	#if defined(__AVR_XMEGA__)

	#else
		EICRA = 0;
		#if defined(__AVR_ATmega2561__) || defined(__AVR_AT90CAN128__)
			EICRB = 0;
		#endif
	#endif	
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Init the externe interrupt and select her sensemode.
 * On ATmeg
 *
 * \param 	interrupt_number		External interrupt number
 * \param 	interrupt_sensemode		Sensemode: SENSE_LOW, SENSE_RISING, SENSE_FALLING, SENSE_CHANGE
 * \param 	pFunc					Pointer to the function
 * \retval  						Give ERROR_SENSEMODE, ERROR_INTERRUPTNUMBER or OK back
 *
 * On ATXmega
 *
 * \param 	PORT					Pointer to the portstructure
 * \param 	Pin						Pin
 * \param 	interrupt_number		The interrupt number, see interruptlist in sourcecode.
 * \param 	interrupt_sensemode		Sensemode: SENSE_LOW, SENSE_RISING, SENSE_FALLING, SENSE_CHANGE
 * \param 	pFunc					Pointer to the function.
 * \retval  						Give ERROR_SENSEMODE, ERROR_INTERRUPTNUMBER or OK back.
 */
/* -----------------------------------------------------------------------------------------------------------*/
#if defined(__AVR_XMEGA__)
char EXTINT_set ( PORT_t * PORT, int Pin, int interrupt_sensemode, EXT_INT_CALLBACK_FUNC pFunc )
#else
char EXTINT_set ( int interrupt_number, int interrupt_sensemode, EXT_INT_CALLBACK_FUNC pFunc )
#endif
{
	#if defined(__AVR_XMEGA__)

		int interrupt_number;

		// PINnCTRL maskieren um Sensemode zu setzen, Sensemode setzen und Interruptlevel setzen
		*(&PORT->PIN0CTRL + Pin ) &= ~(0x07);
		*(&PORT->PIN0CTRL + Pin ) |= interrupt_sensemode ;
		PORT->INTCTRL = PORT_INT0LVL_LO_gc;
		
		// welcher Interrupt ist gemeint ?
		interrupt_number = ( int ) PORT;
		interrupt_number = ( interrupt_number >> 5 ) & 0x0f;
		// CallbackFunc eintragen
		extint_CallBack_Table[ interrupt_number ] = pFunc ;
			
		// Interrupt freigeben
		EXTINT_free( PORT, Pin );
		
		return( OK );			
	#else
		// ist der interrupt gültig ?
		if ( interrupt_number < MAX_EXT_INT )
		{
			// sensemode gültig ?
			if ( interrupt_sensemode < 4 )
			{
				// SENSEMODE auf die Register aufteilen
				#if defined(__AVR_ATmega2561__) || defined(__AVR_AT90CAN128__)
					if ( interrupt_number < 4 )
				#elif defined(__AVR_ATmega644__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega1284P__)
					if ( interrupt_number < 3 )
				#endif
				{
					EICRA &= ~( 3 << ( interrupt_number << 1 ) );
					EICRA |= ( interrupt_sensemode << ( interrupt_number << 1 ) );
				}
				#if defined(__AVR_ATmega2561__) || defined(__AVR_AT90CAN128__)
				else
				{
					EICRB &= ~( 3 << ( interrupt_number << 1 ) );
					EICRB |= ( interrupt_sensemode << ( interrupt_number << 1 ) );
				}
				#endif
			}
			else
			{
				return( ERROR_SENSEMODE );
			}
			// CallbackFunc eintragen
			extint_CallBack_Table[ interrupt_number ] = pFunc ;
			
			// Interrupt freigeben
			EXTINT_free( interrupt_number );
		
			return( OK );		
		}
		else
		{
			return( ERROR_INTERRUPTNUMBER );
		}
	#endif
}
	
/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Enable an external interrupt.
 * \param 	interrupt_number		On Atmega: Number of the external interrupt. On ATxmega: bit 4-7 Port and bit 0-3 Pin. Only one interrupt per port implemented.
 * \retval	NONE
 */
/* -----------------------------------------------------------------------------------------------------------*/
#if defined(__AVR_XMEGA__)
void EXTINT_free ( PORT_t * PORT, int Pin )
#else
void EXTINT_free ( int interrupt_number )
#endif
{
#if defined(__AVR_XMEGA__)
	PORT->INT0MASK = ( 1 << Pin );
#else
	if ( interrupt_number < MAX_EXT_INT )
		EIMSK |= ( 1 << interrupt_number );
#endif
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! \brief Disable an external interrupt.
 * \param 	interrupt_number		On Atmega: Number of the external interrupt. On ATxmega: bit 4-7 Port and bit 0-3 Pin. Only one interrupt per port implemented.
 * \retval	NONE
 */
/* -----------------------------------------------------------------------------------------------------------*/
#if defined(__AVR_XMEGA__)
	void EXTINT_block ( PORT_t * PORT, int Pin )
#else
	void EXTINT_block ( int interrupt_number )
#endif
{
#if defined(__AVR_XMEGA__)
	PORT->INT0MASK &= ~( 1 << Pin );
#else
	if ( interrupt_number < MAX_EXT_INT )
		EIMSK &= ~( 1 << interrupt_number );	
#endif
}

#if defined(__AVR_XMEGA__)
	ISR(PORTA_INT0_vect)
	{
		extint_CallBack_Table[0]();
	}

	ISR(PORTB_INT0_vect)
	{
		extint_CallBack_Table[1]();
	}

	ISR(PORTC_INT0_vect)
	{
		extint_CallBack_Table[2]();
	}
	
	ISR(PORTD_INT0_vect)
	{
		extint_CallBack_Table[3]();
	}
	
	ISR(PORTE_INT0_vect)
	{
		extint_CallBack_Table[4]();
	}

#if defined(__AVR_ATxmega16D3__) || defined(__AVR_ATxmega32D3__) || defined(__AVR_ATxmega64D3__) || defined(__AVR_ATxmega128D3__)
	ISR(PORTF_INT0_vect)
	{
		extint_CallBack_Table[5]();
	}
#elif defined(__AVR_ATxmega64A3__) || defined(__AVR_ATxmega128A3__) || defined(__AVR_ATxmega192A3__) || defined(__AVR_ATxmega256A3__) || defined(__AVR_ATxmega256A3B__)
	ISR(PORTF_INT0_vect)
	{
		extint_CallBack_Table[5]();
	}
#elif defined(__AVR_ATxmega64A1__) || defined(__AVR_ATxmega128A1__) || defined(__AVR_ATxmega192A1__) || defined(__AVR_ATxmega256A1__) || defined(__AVR_ATxmega384A1__)
	ISR(PORTF_INT0_vect)
	{
		extint_CallBack_Table[5]();
	}
	
	ISR(PORTH_INT0_vect)
	{
		extint_CallBack_Table[7]();
	}
	
	ISR(PORTJ_INT0_vect)
	{
		extint_CallBack_Table[8]();
	}
	
	ISR(PORTK_INT0_vect)
	{
		extint_CallBack_Table[9]();
	}
	
	ISR(PORTQ_INT0_vect)
	{
		extint_CallBack_Table[14]();
	}
#endif
	ISR(PORTR_INT0_vect)
	{
		extint_CallBack_Table[15]();
	}

#else

	ISR(INT0_vect)
	{
		extint_CallBack_Table[0]();
	}
	
	ISR(INT1_vect)
	{
		extint_CallBack_Table[1]();
	}
	
	ISR(INT2_vect)
	{
		extint_CallBack_Table[2]();
	}
	
		#if defined(__AVR_ATmega2561__) || defined(__AVR_AT90CAN128__)
		ISR(INT3_vect)
		{
			extint_CallBack_Table[3]();
		}
	
		ISR(INT4_vect)
		{
			extint_CallBack_Table[4]();
		} 
	
		ISR(INT5_vect)
		{
			extint_CallBack_Table[5]();
		}
	
		ISR(INT6_vect)
		{
			extint_CallBack_Table[6]();
		}
	
		ISR(INT7_vect)
		{
			extint_CallBack_Table[7]();
		}
		#endif
#endif

#endif

/**
 * @}
 */
