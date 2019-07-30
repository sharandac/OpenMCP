/***************************************************************************
 *            playlist.c
 *
 *  Tue Mar 27 01:48:27 2012
 *  Copyright  2012  Dirk Broßwick
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/pgmspace.h>

#include "system/shell/shell.h"
#include "system/clock/clock.h"
#include "system/config/eeconfig.h"
#include "system/stdout/stdout.h"
#include "system/thread/thread.h"

#include "apps/httpd/cgibin/cgi-bin.h"
#include "apps/httpd/httpd2.h"
#include "apps/httpd/httpd2_pharse.h"
#include "apps/mp3-streamingclient/mp3-clientserver.h"

#include "playlist.h"

const char liststring[] PROGMEM = "PLAYLIST_ENTRY_%02d";

static char playlist[1526];
static char playlistdata[MAX_PLAYLIST];

void PLAYLIST_init( void )
{
#if defined(HTTPSERVER_CRON)
	cgi_RegisterCGI( cgi_printplaylist, PSTR("playlist.cgi"));
#endif
	PLAYLIST_loadlist();
}

void cgi_printplaylist( void * pStruct )
{
	int i;
	char string[128];	
	
	struct HTTP_REQUEST * http_request;
	http_request = (struct HTTP_REQUEST *) pStruct;
		
	cgi_PrintHttpheaderStart();

	if ( http_request->argc == 0 )
	{
		printf_P( PSTR(	"<table border=\"0\" cellpadding=\"5\" cellspacing=\"0\">" ));
		for( i = 0 ; i < MAX_PLAYLIST ; i++ )
		{
			if ( PLAYLIST_getentry( string, i) == 1 )
			{
				printf_P( PSTR(	"<tr>"
					   			"<td align=\"left\">%s</td>"
					   			"<td align=\"left\">"
								"<a href=\"playlist.cgi?del=%d\" style=\"text-decoration:none\" title=\"Del\"><input type=\"button\" value=\"del\" class=\"actionBtn\"></a>"
								"</td>"
  								"</tr>"), string, i );
			}
		}

		printf_P( PSTR(	"<tr>"
			   			"<td align=\"left\">"
						"<a href=\"playlist.cgi?addnew\" style=\"text-decoration:none\" title=\"Add\"><input type=\"button\" value=\"add new station\" class=\"actionBtn\"></a>"
						"</td>"
						"</tr>"
						"</table>\r") );

	}
	else
	{
		if ( PharseCheckName_P( http_request, PSTR("addnew") ) )
		{
			printf_P( PSTR(	"<form action=\"playlist.cgi\">"
					   		"<table border=\"0\" cellpadding=\"5\" cellspacing=\"0\">"
							"<tr>"
						   	"<td align=\"right\">Station Name</td>"
							"<td><input name=\"name\" type=\"text\" size=\"32\" value=\"\" maxlength=\"32\"></td>"
  							"</tr>"
							"<tr>"
						   	"<td align=\"right\">station URL</td>"
							"<td><input name=\"station\" type=\"text\" size=\"96\" maxlength=\"96\"></td>"
  							"</tr>"
 							"<tr>"
   							"<td></td><td><input type=\"submit\" value=\" Eintrag sichern \"></td>"
  							"</tr>"
						   	"</table>"
							"</form>\r" ) );
		}
		else if ( PharseCheckName_P( http_request, PSTR("station") ) )
		{
			sprintf_P( string, PSTR("%s;%s"),http_request->argvalue[ PharseGetValue_P ( http_request, PSTR("name") ) ], http_request->argvalue[ PharseGetValue_P ( http_request, PSTR("station") ) ] );
			PLAYLIST_addentry( string );
			PLAYLIST_loadlist();
			printf_P( PSTR("Einstellungen uebernommen!\r\n"));

		}
		else if ( PharseCheckName_P( http_request, PSTR("del") ) )
		{
			PLAYLIST_delentry( atoi( http_request->argvalue[ PharseGetValue_P ( http_request, PSTR("del") ) ] ) );
			PLAYLIST_loadlist();
		}
	}

	cgi_PrintHttpheaderEnd();

}

void PLAYLIST_playentry( int entry )
{
	char string[128],*search;

	if ( PLAYLIST_getentry( string, playlistdata[ entry ] ) == 1 )
	{
		// string kürzen auf den Namen
		search = string;
		while(*search != ';')
		{
			search++;
		}
		search++;
		// play url
		PlayURL( search );
	}
}

char * PLAYLIST_getlistpointer( void )
{
	return( playlist );
}
/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Sucht den nächsten freien Eintrag in der Crontabelle im Configspeicher.
 * \param 	NONE
 * \return	int
 * \retval  -1			Kein freier Platz vorhanden.
 * \retval  >0			Nummer des freien Speicherplatzes.
 */
/*------------------------------------------------------------------------------------------------------------*/
void PLAYLIST_loadlist( void )
{
	int i,y=0,a=0;
	char string[128],*search;

	playlist[0] ='\0';
	
	for( i = 0 ; i < MAX_PLAYLIST ; i++ )
	{
		if ( PLAYLIST_getentry( string, i ) == 1 )
		{
			// string kürzen auf den Namen
			search = string;
			while(*search != ';')
			{
				search++;
			}
			*search = '\0';
			search++;

			// passt der name noch in die Liste ?
			if ( ( a + strlen( string ) ) > sizeof(playlist ) )
			    break;

			// Nummer von Namen eintragen in liste
			playlistdata[ y ] = i;

			// return einfügen wenn es nicht der erste eintrag ist
			if(a)
			{
				playlist[a] = '\n';
				a++;
			}

			// string an die liste hängen
			strcpy( &playlist[ a ], string );
			a = a + strlen( string );
			y++;
		}
	}
}

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Sucht den nächsten freien Eintrag in der Crontabelle im Configspeicher.
 * \param 	NONE
 * \return	int
 * \retval  -1			Kein freier Platz vorhanden.
 * \retval  >0			Nummer des freien Speicherplatzes.
 */
/*------------------------------------------------------------------------------------------------------------*/
int PLAYLIST_getfreeentry( void )
{
	char string[32];
	
	int i;
	
	for( i = 0 ; i < MAX_PLAYLIST ; i++ )
	{
		sprintf_P( string, liststring, i );
		if ( checkConfigName( string ) == -1 )
			return( i );
	}
	return( -1 );
}

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Speichert ein Cronentrag im Configspeicher ab.
 * \param 	cronstring  String der von der Shell ausgeführt werden soll.
 * \return	int
 * \retval  -1			Kein freier Platz vorhanden.
 * \retval  >0			Nummer des freien Speicherplatzes.
 */
/*------------------------------------------------------------------------------------------------------------*/
int PLAYLIST_addentry( char * playliststring )
{
	char string[32];
	int i;
	
	i = PLAYLIST_getfreeentry( );
	
	if ( i != -1 )
	{
		sprintf_P( string, liststring, i );
		writeConfig( string, playliststring );
	}	
	return(0);
}

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Löscht einen Eintrag aus der Crontabelle.
 * \param 	entry		Eintrag der gelöscht werden soll.
 * \return	int
 * \retval  -1			Kein freier Platz vorhanden.
 * \retval  >0			Nummer des freien Speicherplatzes.
 */
/*------------------------------------------------------------------------------------------------------------*/
int PLAYLIST_delentry( int entry )
{
	char string[32];

	sprintf_P( string, liststring, entry );
	deleteConfig( string );
	return(0);
}

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Holt einen Croneintrag.
 * \param 	cronentry		Zeiger auf einen Speicherbereich in dem der String gespeichert werden soll.
 * \param 	entry			Eintrag der geholt werden soll.
 * \return	NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
int PLAYLIST_findentry( char * playliststring )
{
	int i;
	char string[32];

	for( i = 0 ; i < MAX_PLAYLIST ; i++ )
	{
		PLAYLIST_getentry( string , i );
		if ( !strcmp( string, playliststring ) )
		    return( i );
	}
	
	return( -1 );
}

/*------------------------------------------------------------------------------------------------------------*/
/*!\brief Holt einen Croneintrag.
 * \param 	cronentry		Zeiger auf einen Speicherbereich in dem der String gespeichert werden soll.
 * \param 	entry			Eintrag der geholt werden soll.
 * \return	NONE
 */
/*------------------------------------------------------------------------------------------------------------*/
int PLAYLIST_getentry( char * playliststring, int entry )
{
	char string[32];
	
	sprintf_P( string, liststring, entry );
	return( readConfig( string, playliststring ) );
}

 