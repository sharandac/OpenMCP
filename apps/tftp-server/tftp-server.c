/***************************************************************************
 *            tftp-server.c
 *
 *  Sat Mar  3 21:00:22 2012
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

/**
 * \ingroup software
 * \addtogroup tftp_server TFTP-Server zum übertragen von Dateien. (tftp-server.c)
 *
 * TFTP-Server zum übertragen von Dateien über Netzwerk auf SD-Karte.
 *
 * @{
 */

/**
 * \file
 *
 * \author Dirk Broßwick
 */

#include <avr/pgmspace.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#if defined(__AVR_XMEGA__)
	#include <system/clock/delay_x.h>
#else
	#include "util/delay.h"
#endif
#include "config.h"
#include "tftp-server.h"

#if defined(TFTPSERVER) && defined(MMC)

#include "system/net/udp.h"
#include "system/thread/thread.h"
#include "system/net/endian.h"
#include "system/filesystem/fat.h"
#include "system/filesystem/filesystem.h"

static char TFTP_buffer[ MAX_TFTP_PACKET_SIZE ];
static int TFTP_socket = UDP_SOCKET_ERROR;

// #define TFTP_DEBUG

/**
 * \brief Initializiert den TFTP-Server
 * \param	NONE
 * \return  NONE
 */
void TFTPSERVER_init( void )
{
	// Register UDP-Socket
	TFTP_socket = UDP_ListenOnPort( TFTPSERVER_PORT, MAX_TFTP_PACKET_SIZE, TFTP_buffer );
	THREAD_RegisterThread( TFTPSERVER_thread, PSTR("tftp-server"));
	printf_P(PSTR("TFTP-Server start\r\n"));
}

/**
 * \brief Thread für den TFTP-Server
 * \param	NONE
 * \return  NONE
 */
void TFTPSERVER_thread( void )
{
	// if an Socket created or opened, if not, create them ?
	if ( TFTP_socket == UDP_SOCKET_ERROR )
	{
		TFTP_socket = UDP_ListenOnPort( TFTPSERVER_PORT, MAX_TFTP_PACKET_SIZE, TFTP_buffer );
	}	
	else
	{		
		if ( UDP_GetSocketState( TFTP_socket ) == UDP_SOCKET_BUSY )
		{
			struct TFTP_PACKET *tftp_packet;
			tftp_packet = (struct TFTP_PACKET *) TFTP_buffer;
			
#if defined(TFTP_DEBUG)
			printf_P(PSTR("TFTP: incoming connection\r\n"));
#endif	

			if ( tftp_packet->packet_type == ntohs( TFTP_RRQ ) )
			{
#if defined(TFTP_DEBUG)
				printf_P(PSTR("TFTP: Read file %s.\r\n"), tftp_packet->data );
#endif		
					TFTPSERVER_read();
			}
			else if( tftp_packet->packet_type == ntohs( TFTP_WRQ ) )
			{
#if defined(TFTP_DEBUG)
				printf_P(PSTR("TFTP: Write file %s.\r\n"), tftp_packet->data );
#endif		
					TFTPSERVER_write();
			}
			else
			{
#if defined(TFTP_DEBUG)
				printf_P(PSTR("TFTP: Unknown Error\r\n") );
#endif		
			}
		}
	}
}

/**
 * \brief Liest eine Datei und schickt diese an den Client.
 * \param	NONE
 * \return  NONE
 */
void TFTPSERVER_read( void )
{
	
	int block = 1, size=0, counter = 0 ;
	long filesize = 0;

	struct fat_dir_entry_struct directory;
	struct fat_dir_struct* dd;
	struct fat_file_struct* fd;
	struct fat_dir_entry_struct dir_entry;
	
	struct TFTP_PACKET *tftp_packet;
	tftp_packet = (struct TFTP_PACKET *) TFTP_buffer;

	// Wenn Readrequest, dann anfangen einzulesen
	if ( tftp_packet->packet_type == ntohs( TFTP_RRQ ) )
	{
		// path setzen
		fat_get_dir_entry_of_path(fs, "/" , &directory);
		// Verzeichnis öffnen
		dd = fat_open_dir(fs, &directory);

		// Verzeichniss inhalt lesen und Datei suchen
#if defined(TFTP_DEBUG)
		printf_P(PSTR("tftp: oeffne datei.\r\n"));
#endif
		while(fat_read_dir(dd, &dir_entry))
		{
			if(strcmp( dir_entry.long_name, &tftp_packet->data ) == 0)
			{
				filesize = dir_entry.file_size;
				fat_reset_dir(dd);					
				break;
			}
		}

		struct TFTP_PACKET_EXT *tftp_packet_ext;
		tftp_packet_ext = (struct TFTP_PACKET_EXT *) &tftp_packet->data;

		// Wenn Datei vorhanden, lesen und ausgeben
		if ( filesize != 0 )
		{
			fd = fat_open_file(fs, &dir_entry);

#if defined(TFTP_DEBUG)
			printf_P(PSTR("tftp: uebertrage datei.\r\n"));
#endif

			while(1)
			{
				tftp_packet->packet_type = htons(TFTP_DATA);
				tftp_packet_ext->block = htons(block);

				size = fat_read_file(fd, (unsigned char *) &tftp_packet_ext->data, 512);
#if defined(TFTP_DEBUG)
				printf_P(PSTR("tftp: block %d, size %d.\r\n"),block, size);
#endif
				block++;

				UDP_FreeBuffer( TFTP_socket );
				UDP_SendPacket( TFTP_socket, 4 + size, TFTP_buffer );

				counter = 1000;
				
				while( counter-- )
				{
					if ( UDP_GetSocketState( TFTP_socket ) == UDP_SOCKET_BUSY )
						break;
					_delay_ms(1);
				}

				if ( counter == 0 )
				{
					tftp_packet->packet_type = htons(TFTP_ERR);
					tftp_packet_ext->block = htons(block);
					tftp_packet_ext->data = '\0';
					UDP_FreeBuffer( TFTP_socket );
					UDP_SendPacket( TFTP_socket, 5, TFTP_buffer );
					break;
				}

				if ( tftp_packet->packet_type == ntohs( TFTP_ERR ) )
					break;

				if ( size == -1 || size < 512 )
					break;
			}
			fat_close_file( fd );

		}
		else
		{
			tftp_packet->packet_type = htons(TFTP_ERR);
			tftp_packet_ext->block = htons(block);
			tftp_packet_ext->data = '\0';
			UDP_FreeBuffer( TFTP_socket );
			UDP_SendPacket( TFTP_socket, 5, TFTP_buffer );
		}

		fat_close_dir ( dd );
		
	}
	
	UDP_CloseSocket( TFTP_socket );
	TFTP_socket = UDP_ListenOnPort( TFTPSERVER_PORT, MAX_TFTP_PACKET_SIZE, TFTP_buffer );
	
}


/**
 * \brief Schreibt eine Datei die vom Client kommt.
 * \param	NONE
 * \return  NONE
 */
void TFTPSERVER_write( void )
{
	int block = 1, size=0, counter = 0 ;
	char * filename = NULL;
	long filesize = 0;

	struct fat_dir_entry_struct directory;
	struct fat_dir_struct* dd;
	struct fat_file_struct* fd;
	struct fat_dir_entry_struct dir_entry;
	
	struct TFTP_PACKET *tftp_packet;
	tftp_packet = (struct TFTP_PACKET *) TFTP_buffer;

	struct TFTP_PACKET_EXT *tftp_packet_ext;
	tftp_packet_ext = (struct TFTP_PACKET_EXT *) &tftp_packet->data;
	
	filename = &tftp_packet->data;

	if ( tftp_packet->packet_type == ntohs( TFTP_WRQ ) )
	{
		// path setzen, Verzeichnis öffnen und Datei anlegen
		fat_get_dir_entry_of_path(fs, "/" , &directory);

		dd = fat_open_dir(fs, &directory);
		while(fat_read_dir(dd, &dir_entry))
		{
			if(strcmp( dir_entry.long_name, filename ) == 0)
			{
#if defined(TFTP_DEBUG)
				printf_P(PSTR("tftp: datei gefunden zum loeschen.\r\n"));
#endif
				fat_reset_dir(dd);					
				fat_delete_file( fs , &dir_entry );
				break;
			}
		}


		dd = fat_open_dir(fs, &directory);
		if(dd)
			fat_create_file( dd , filename , &dir_entry );
		
		while(fat_read_dir(dd, &dir_entry))
		{
			if(strcmp( dir_entry.long_name, filename ) == 0)
			{
				filesize = dir_entry.file_size;
				fat_reset_dir(dd);					
				break;
			}
		}

#if defined(TFTP_DEBUG)
		printf_P(PSTR("tftp: oeffne datei.\r\n"));
#endif

		// Wenn Datei vorhanden, lesen und ausgeben
		if ( filesize == 0 )
		{
			long offset = 0;

			// Datei oeffnen
			fd = fat_open_file(fs, &dir_entry);
			fat_seek_file(fd, &offset , FAT_SEEK_SET);

#if defined(TFTP_DEBUG)
			printf_P(PSTR("tftp: uebertrage datei.\r\n"));
#endif

			//
			// erstes Packet uebertragen
			//
			tftp_packet->packet_type = htons(TFTP_ACK);
			tftp_packet_ext->block = htons(0);
			block = 1;
			UDP_SendPacket( TFTP_socket, 4, TFTP_buffer );
			UDP_FreeBuffer( TFTP_socket );

			// schleife zum uebertragen der Datei
			while(1)
			{
				// auf antwort warten
				counter = 1000;
				while( counter-- )
				{
					if ( UDP_GetSocketState( TFTP_socket ) == UDP_SOCKET_BUSY )
						break;
					_delay_ms(1);
				}

				// ist es ein antwortpacket ? wenn ja nächstes
				if ( tftp_packet->packet_type == ntohs( TFTP_DATA ) )
				{
					//
					// Daten kopieren und in Datei speichern
					//
					size = UDP_GetByteInBuffer( TFTP_socket ) - 4 ;
					fat_write_file( fd, (unsigned char *) &tftp_packet_ext->data , size );
					//
					// antwortpacket basteln und senden
					//
					tftp_packet->packet_type = htons(TFTP_ACK);
					block = ntohs( tftp_packet_ext->block );
					UDP_SendPacket( TFTP_socket, 4, TFTP_buffer );
					UDP_FreeBuffer( TFTP_socket );
#if defined(TFTP_DEBUG)
					printf_P(PSTR("tftp: block %d, size %d.\r\n"),block, size);
#endif
					//
					// auf antwort warten
					//
					counter = 1000;
					while( counter-- )
					{
						if ( UDP_GetSocketState( TFTP_socket ) == UDP_SOCKET_BUSY )
							break;

						_delay_ms(1);
					}

					//
					// ist der counter in den timeout gelaufen -> abbruch
					//
					if ( counter == 0 )
					{
						UDP_SendPacket( TFTP_socket, 4, TFTP_buffer );
						UDP_FreeBuffer( TFTP_socket );
#if defined(TFTP_DEBUG)
						printf_P(PSTR("tftp: retrans.\r\n"),block, size);
#endif
					}

				}

				//
				// wurde ERR empfangen ? -> abbruch
				//
				if ( tftp_packet->packet_type == ntohs( TFTP_ERR ) )
					break;

				//
				// Daten kleiner 512, dann letztes Packet
				if ( size < 512 )
				{
#if defined(TFTP_DEBUG)
					printf_P(PSTR("TFTP: fertig\r\n"));
#endif
					break;
				}
			}
			// datei schliessen
			fat_close_file( fd );
		}
		else
		{
			// wenn Fehler, dann ERR-Packet schicken
			tftp_packet->packet_type = htons(TFTP_ERR);
			tftp_packet_ext->block = htons(block);
			tftp_packet_ext->data = '\0';
			UDP_FreeBuffer( TFTP_socket );
			UDP_SendPacket( TFTP_socket, 5, TFTP_buffer );
		}
		// Verzeichniss schliessen
		fat_close_dir ( dd );
	}

	// UDP-Socket schliessen und neues listen auf machen
	UDP_CloseSocket( TFTP_socket );
	TFTP_socket = UDP_ListenOnPort( TFTPSERVER_PORT, MAX_TFTP_PACKET_SIZE, TFTP_buffer );

}

#endif // TFTPSERVER

/**
 * @}
**/
