//***************************************************************************
//*            files.c
//*
//*  Sat Jul 13 21:07:42 2008
//*  Copyright  2008  Dirk Broßwick
//*  Email: sharandac@snafu.de
//****************************************************************************/
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
 * \ingroup httpd2
 * \addtogroup http_files Suche nach Files für http (files.c)
 *
 * @{
 */

/**
 *
 * Liefert Files von der SD-Karte aus oder aus dem internen Flash.
 *
 * \author Dirk Broßwick
 */

#include "config.h"

#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "httpd2.h" 
#include "system/net/tcp.h"
#include "system/stdout/stdout.h"
#include "files.h"
#if !defined(HTTP_FILES_FROM_MMC)
	#include "files_data.h"
#endif

#if defined(MMC)
	#if defined(AVRNETIO) || defined(myAVR) || defined(ATXM2) 
		#include "system/filesystem/fat.h"
		#include "system/filesystem/filesystem.h"
	#else
		#error "MMC Hardwareplatform wird nicht unterstützt!"
	#endif
#endif

/*------------------------------------------------------------------------------------------------------------*/
/**
 * \brief Liefert ein File aus per HTTP.
 * \param 	pStruct		Pointer auf die HTTP Struktur.
 * \return	returncode	-1 Failed, 1 OK, File gefunden
 */
/*------------------------------------------------------------------------------------------------------------*/
int check_files( void * pStruct )
{

	struct HTTP_REQUEST * http_request;
	http_request = (struct HTTP_REQUEST *) pStruct;
		
	int i, returnvalue = -1;
	long filesize = 0;

#ifdef HTTP_DEBUG
	struct STDOUT oldstream;

	STDOUT_save( &oldstream );
	STDOUT_set( RS232, 0 );

	printf_P( PSTR("http-server: %s ") , http_request->GET_FILE );
	
	STDOUT_Flush();
	STDOUT_restore( &oldstream );
#endif

#if !defined(HTTP_FILES_FROM_MMC)
	for( i = 0 ; i < MAX_FILES_ENTRYS ; i++ )
	{
		if ( files[ i ].len == 0 )
			break;

		if ( !strcmp_P( http_request->GET_FILE, files[ i ].filesname ) )
		{
			printf_P( PSTR("HTTP/1.0 200 Document follows\r\n"
			               "Content-Type: ") );

			switch ( files[ i ].filestype )
			{
				case PNG:					printf_P( PSTR("png") );
				break;
				case JPEG:					printf_P( PSTR("jpeg") );
				break;
				case TEXT:					printf_P( PSTR("text/html") );
				break;
				default:					printf_P( PSTR("bin") );
			}

			printf_P( PSTR(	"\r\n"
			               "Content-Length: %d\r\n"
			               "Cache-Control: max-age=3600\r\n"
			               "Connection: close\r\n"
			               "\r\n" ), files[ i ].len );

			STDOUT_Flush();

			filesize = files[ i ].len;
			PutSocketData_RPE( http_request->HTTP_SOCKET, files[ i ].len, ( void * ) files[ i ].files, FLASH );
			return( 1 );
		}
	}
#endif
	
#ifdef MMC
	int  size=0;
	char * filename = NULL;
	char * filetype = NULL;
	
	if ( returnvalue == -1 )
	{
		i = strlen( http_request->GET_FILE );
		filename = http_request->GET_FILE;
		filename += i;
		
		while( i )
		{
			if ( * filename == '/' )
			{
				* filename = '\0';
				filename++;
				break;
			}
			if( * filename == '.' )
			{
				filetype = filename;
				filetype++;
			}
			filename--;
			i--;
		}

		struct fat_dir_entry_struct directory;
		struct fat_dir_struct* dd;

		// Wenn nur filename dann Stammverzeichniss wählen, wenn nicht Verzeichnis wählen
		if ( i == 0 )
		{
			fat_get_dir_entry_of_path(fs, "/" , &directory);
		}
		else	
		{
			fat_get_dir_entry_of_path(fs, http_request->GET_FILE , &directory);
		}

		// Verzeichbnis öffnen
		dd = fat_open_dir(fs, &directory);
		if(dd)
        {

			struct fat_dir_entry_struct dir_entry;
			// Verzeichniss inhalt lesen und Datei suchen
			while(fat_read_dir(dd, &dir_entry))
		    {
		        if(strcmp( dir_entry.long_name, filename ) == 0)
		        {
					filesize = dir_entry.file_size;
		            fat_reset_dir(dd);					
		            break;
		        }
			}

			// Wenn Datei vorhanden, lesen und ausgeben
			if ( filesize != 0 )
			{
				struct fat_file_struct* fd = fat_open_file(fs, &dir_entry); // open_file_in_dir(fs, dd, http_request->GET_FILE );

                if( fd )
                {
					char type[6];
					type[0] = '\0';
					if ( !strcmp_P( filetype, PSTR("html") ) || !strcmp_P( filetype, PSTR("htm") ) )
						strcpy_P( type , PSTR("text/") );
					
					printf_P( PSTR(	"HTTP/1.0 200 Document follows\r\n"
					               	"Content-Length: %ld\r\n"
								   	"Content-Type: %s%s\r\n"
								    "Cache-Control: max-age=3600\r\n"
									"Connection: close\r\n"
									"\r\n" ), filesize, type, filetype );

					char buffer[ MAX_TCP_Datalenght ];

					STDOUT_Flush();
					
					while( 1 )
	                {
						if( ( CheckSocketState( http_request->HTTP_SOCKET ) <= SOCKET_NOT_USE ) )
						{
							http_request->HTTP_SOCKET = SOCKET_ERROR;
							http_request->STATE = DISCONNECT;
							returnvalue = -1;
							break;
						}

						size = fat_read_file(fd, (unsigned char *) buffer, sizeof( buffer ));
						
						if ( size == -1 || size == 0 )
						{
							returnvalue = -1;
							break;
						}

						PutSocketData_RPE( http_request->HTTP_SOCKET, size, ( void * ) buffer, RAM );
/*
						for ( i = 0 ; i < size ; i++ )
						{
							printf_P(PSTR("%c"),buffer[i]);
						}
*/						
						if ( size < MAX_TCP_Datalenght )
						{
							returnvalue = 1;
							break;
	                	}
					}
					STDOUT_Flush();
				}			
                fat_close_file( fd );
			}
		}
		fat_close_dir ( dd );
	}

#endif

#ifdef HTTP_DEBUG
	struct STDOUT oldstream;

	STDOUT_save( &oldstream );
	STDOUT_set( RS232, 0 );

	printf_P( PSTR("( %ld Byte übertragen\r\n") , filesize );
	
	STDOUT_Flush();
	STDOUT_restore( &oldstream );
#endif

	return( returnvalue );
} 

/**
 * @}
 */

