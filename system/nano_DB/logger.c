/***************************************************************************
 *            logger.c
 *
 *  Sun Sep  5 16:59:05 2010
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
#include "nano_db.h"
#include <avr/pgmspace.h>
#include <stdio.h>
#include <string.h>
#include "logger.h"

#include "config.h"

#if defined(MMC)

// #define DEBUG

int LOGGER_getlastDBEntry( struct LOGGER_ENTRY * logger_entry, char * FILENAME )
{
	int retval = LOGGER_ERROR;
	int DBEntrys;

	// anzahl der einträge holen
	DBEntrys = nano_DB_getnumbersofDB( FILENAME , sizeof( struct LOGGER_ENTRY ) + logger_entry->DataSize );

	// Wenn keine Datenbank vorhanden -> Fehler
	if ( DBEntrys == nanoDB_ERROR )
	{
#if defined(DEBUG)
		printf_P( PSTR("Keine Datenbank vorhanden.\r\n"), FILENAME, sizeof( struct LOGGER_ENTRY ) + logger_entry->DataSize );
#endif
		retval = LOGGER_ERROR;
	}
	else
	{
		// Letzten Eintrag einlesen
		nano_DB_readDBentry( FILENAME , DBEntrys - 1, logger_entry , sizeof( struct LOGGER_ENTRY ) + logger_entry->DataSize );
#if defined(DEBUG)
		printf_P( PSTR("Lese Daten aus %s mit %d Bytes.\r\n"), FILENAME, sizeof( struct LOGGER_ENTRY ) + logger_entry->DataSize );
#endif
		retval = LOGGER_OK;
	}
	return( retval );
}

/*------------------------------------------------------------------------------------------------------------*/
int LOGGER_writeDBEntry( struct LOGGER_ENTRY * logger_entry,  char * FILENAME )
{
	int retval = LOGGER_ERROR;
	int DBEntrys;

	// anzahl der einträge holen
	DBEntrys = nano_DB_getnumbersofDB( FILENAME , sizeof( struct LOGGER_ENTRY ) + logger_entry->DataSize );

	// Wenn Datenbank vorhanden, Eintrag schreiben
	if ( DBEntrys != nanoDB_ERROR )
	{
		DBEntrys = DBEntrys - 1 ;
		DBEntrys = nano_DB_writeDBentry( FILENAME, DBEntrys , logger_entry , sizeof( struct LOGGER_ENTRY ) + logger_entry->DataSize );
#if defined(DEBUG)
		printf_P( PSTR("Speichere Eintrag in %s mit %d Bytes.\r\n"), FILENAME, sizeof( struct LOGGER_ENTRY ) + logger_entry->DataSize );
#endif
		if ( DBEntrys != nanoDB_ERROR )
			retval = LOGGER_OK;
	}

	return( retval );
}

/*------------------------------------------------------------------------------------------------------------*/
int LOGGER_addDBentry( struct LOGGER_ENTRY * logger_entry, char * FILENAME )
{
	int retval = LOGGER_ERROR;
	int DBEntrys;

	// anzahl der einträge holen
	DBEntrys = nano_DB_getnumbersofDB( FILENAME , sizeof( struct LOGGER_ENTRY ) + logger_entry->DataSize );

	// Wenn keine Datenbank vorhanden, neu erstellen und einen Eintrag schreiben
	if ( DBEntrys == nanoDB_ERROR )
	{
		DBEntrys = 0 ;
		nano_DB_makeDB( FILENAME );
		if ( nano_DB_writeDBentry( FILENAME, DBEntrys , logger_entry , sizeof( struct LOGGER_ENTRY ) + logger_entry->DataSize ) != nanoDB_ERROR )
			retval = LOGGER_OK;

#if defined(DEBUG)
		printf_P( PSTR("Erzeuge neue Datenbank %s mit %d Bytes.\r\n"), FILENAME, sizeof( struct LOGGER_ENTRY ) + logger_entry->DataSize );
#endif

	}
	else
	{
		if ( nano_DB_writeDBentry( FILENAME, DBEntrys , logger_entry , sizeof( struct LOGGER_ENTRY ) + logger_entry->DataSize ) != nanoDB_ERROR )
			retval = LOGGER_OK;

#if defined(DEBUG)
		printf_P( PSTR("Füge neuen Eintrag in %s mit %d Bytes hinzu.\r\n"), FILENAME, sizeof( struct LOGGER_ENTRY ) + logger_entry->DataSize );
#endif

	}

	return( retval );
}

#endif
