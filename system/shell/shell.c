/***************************************************************************
 *            shell.c
 *
 *  Tue May 25 23:37:37 2010
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
 * \ingroup system
 * \addtogroup shell Interface zur Shell (shell.c)
 *
 * @{
 */

/**
 * \file
 *
 * \author Dirk Broßwick
 */

#include <avr/pgmspace.h>
#include <avr/version.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "shell.h"

struct DYN_SHELL shell_cmd_table [ MAX_SHELL_ENTRYS ];

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Initialisiert den Telnet-clinet und registriert den Port auf welchen dieser lauschen soll.
 * \param 	NONE
 * \return	NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
void SHELL_init( void )
{
	int i;

	// Befehlstabelle löschen
	for ( i = 0 ; i < MAX_SHELL_ENTRYS ; i++ )
	{
		shell_cmd_table[ i ].dynshell_function = NULL;
	}
}

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Initialisiert den Telnet-clinet und registriert den Port auf welchen dieser lauschen soll.
 * \param 	NONE
 * \return	NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
int SHELL_RegisterCMD( DYN_SHELL_CALLBACK dynshell_function, const char * funktionname )
{
	int i,retval;

	retval = -1;
	
	for ( i = 0 ; i < MAX_SHELL_ENTRYS ; i++ )
	{
		if ( shell_cmd_table[ i ].dynshell_function == dynshell_function )
			break;
		
		if ( shell_cmd_table[ i ].dynshell_function == NULL )
		{
			shell_cmd_table[ i ].dynshell_function = dynshell_function;			
			shell_cmd_table[ i ].functionname = funktionname;
			retval = 0;
			break;
		}
	}
	return( retval );
}

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Ruft ein Kommando extern.
 * \param 	cmdstring   Kommandostring der ausgeführt werden soll.
 * \return	int			0 wenn Kommando gefunden, bei Fehler -1.
 */
/*------------------------------------------------------------------------------------------------------------*/
int SHELL_pharse( char * BUFFER, char ** argv, int max_argc )
{
		
	int i = 0;
	int _argc = 0;
	char toggle = 0;
	int stringlen = strlen( BUFFER );
	
	argv[ _argc ] = &BUFFER[ i ]; 
	
	for( i = 0 ; i < stringlen && _argc < max_argc ; i++ )
	{
		switch( BUFFER[ i ] )
		{
			
		case '\0':		i = stringlen;
						break;
		case 0x0d:		BUFFER[ i ] = '\0';
						i = stringlen;
						break;
		case '\"':		if ( toggle == 0 )
						{	
							toggle = 1;
							BUFFER[ i ] = '\0';
							argv[ _argc ] = &BUFFER[ i + 1];
						}
						else
						{
							toggle = 0;
							BUFFER[ i ] = '\0';
						}
						break;
		case ' ':		if ( toggle == 0 )						
						{
							BUFFER[ i ] = '\0';
							_argc++;
							argv[ _argc ] = &BUFFER[ i + 1 ];
						}
						break;
		}
	}
	return( _argc + 1 );
}

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Ruft ein Kommando extern.
 * \param 	cmdstring   Kommandostring der ausgeführt werden soll.
 * \return	int			0 wenn Kommando gefunden, bei Fehler -1.
 */
/*------------------------------------------------------------------------------------------------------------*/
int SHELL_runcmd( int argc, char ** argv )
{
	int i, returncode = -1;

	// auf QUIT checken
	if ( !strcmp_P( argv[ 0 ] , PSTR("help") ) ) 
	{
		printf_P( PSTR("Commands:\r\n\r\n") );
		for( i = 0 ; i < MAX_SHELL_ENTRYS ; i++ )
		{
			if ( shell_cmd_table[ i ].functionname != NULL )
			{
				printf_P( shell_cmd_table[ i ].functionname );
				printf_P( PSTR("\r\n") );
			}
		}
	}
	else
	{
		for( i = 0 ; i < MAX_SHELL_ENTRYS ; i++ )
		{
			if ( shell_cmd_table[ i ].functionname == NULL )
			{
				break;
			}
			if ( !strcmp_P( argv[0] , shell_cmd_table[ i ].functionname ) )
			{
				shell_cmd_table[i].dynshell_function( argc, argv );
				returncode = 0;
				break;
			}
		}
	}
	return( returncode );
}

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Ruft ein Kommando extern auf.
 * \param 	cmdstring   Kommandostring der ausgeführt werden soll.
 * \return	int			0 wenn Kommando gefunden, bei Fehler -1.
 */
/*------------------------------------------------------------------------------------------------------------*/
int SHELL_runcmdextern( char * cmdstring )
{
	char * argv[ MAX_ARGC ];
	int argc;
	
	argc = SHELL_pharse( cmdstring, argv, MAX_ARGC );
	
	return( SHELL_runcmd( argc, argv ) );
}

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Ruft ein Kommando extern auf.
 * \param 	cmdstring   Kommandostring der ausgeführt werden soll der im Flash liegt.
 * \return	int			0 wenn Kommando gefunden, bei Fehler -1.
 */
/*------------------------------------------------------------------------------------------------------------*/
int SHELL_runcmdextern_P( const char * cmdstring_P )
{
	char * argv[ MAX_ARGC ];
	int argc;
	char cmdstring[ SHELL_BUFFER_LEN ];
	
	strcpy_P( cmdstring, cmdstring_P );
	
	argc = SHELL_pharse( cmdstring, argv, MAX_ARGC );
	
	return( SHELL_runcmd( argc, argv ) );
}

/**
 * @}
 */

