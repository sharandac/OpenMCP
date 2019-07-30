/***************************************************************************
 *            thread.c
 *
 *  Wed Dec 16 19:43:03 2009
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
 * \ingroup system
 * \addtogroup THREAD Ein einfacher Schedular um Programme/Funktionen zyklisch aufzurufen.
 *
 * @{
 */

/**
 * \file
 * Verwaltet eine Threadliste mit Funktionen die zyklisch durchlaufen werden. Diese Programm/Funktionen
 * sollten nicht blockierend geschrieben sein um eine Funktion zu garantieren.
 *
 * \author Dirk Broßwick
 */
#include <avr/pgmspace.h>
#include <avr/version.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "thread.h"

struct THREAD thread_table[ THREAD_MAX ];

/*-----------------------------------------------------------------------------------------------------------*/
/**
 * \brief Initialisiert den Threader.
 */
/*------------------------------------------------------------------------------------------------------------*/
void THREAD_init( void )
{
	int i;

	// Alle einträge auf NULL setzen
	for ( i = 0 ; i < THREAD_MAX ; i ++ )
		thread_table[ i ].thread_function = NULL;		
}

/*-----------------------------------------------------------------------------------------------------------*/
/**
 * \brief Registriert einen Thread der zyklisch aufgerufen werden soll.
 * \param	thread_function	Pointer auf die Funktion die aufgerufen werden soll.
 * \param	thread_name		Pointer auf den Namen des Threads der im Flash liegt.
 * \return	-1 wenn ein Fehler aufgetreten ist oder einen positiven Wert wenn erfolgeich.
 */
/*------------------------------------------------------------------------------------------------------------*/
int THREAD_RegisterThread( THREAD_CALLBACK thread_function, const char * thread_name )
{
	int i,retval;

	retval = -1;

	// In der Threadtabelle nach einen freien eintrag suchen oder ob er vorhanden ist.
	for ( i = 0 ; i < THREAD_MAX ; i++ )
	{
		// Schon vorhanden ?
		if ( thread_table[ i ].thread_function == thread_function )
			break;

		// Freier eintrag, dann eintrag setzen
		if ( thread_table[ i ].thread_function == NULL )
		{
			thread_table[ i ].thread_function = thread_function;
			thread_table[ i ].thread_name = thread_name;
			retval = 0;
			break;
		}
	}
	
	return( retval );
}

/*-----------------------------------------------------------------------------------------------------------*/
/**
 * \brief Durchläuft die Threadliste.
 */
/*------------------------------------------------------------------------------------------------------------*/
void THREAD_mainloop( void )
{
	static int i=0;

	// Schon das ende der Threadliste erreicht ? dann i auf i setzen.
	if ( thread_table[ i ].thread_function == NULL )
		i = 0;

	// thread ausführen
	thread_table[i].thread_function( );

	i++;

	return;
}

/**
 * @}
 */
