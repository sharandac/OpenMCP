/*! \file tcp.h \brief Stellt die TCP/IP Funkionalitaet bereit */
//***************************************************************************
// *            tcp.h
// *
// *  Sat Jun  3 23:01:49 2006
// *  Copyright  2006  Dirk Broßwick
// *  Email: sharandac@snafu.de
//****************************************************************************/
/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __TCP_H__
	#define __TCP_H__

	#include <avr/pgmspace.h>
	#include "config.h"

	extern unsigned int TXErrorCounter;
	extern unsigned int RXErrorCounter;
	extern unsigned int RXErrorUnsort;
	extern unsigned int RXErrorOldSeq;

	typedef void ( * TCP_CALLBACK_FUNC ) ( int socket, int fifo );

	void tcp_init( void );

	void tcp( int packet_lenght, char *buffer);
	void TCPTimeOutHandler( void );
	void LockTCP( void );
	void FreeTCP( void );

	int Getfreesocket(void);
	int GetSocket( char * buffer);
	int GetFIFO( int socket );
	int RegisterSocket( char *buffer);
	
	int CopyTCPdata2socketbuffer( int Socket, int Datalenght , char *buffer);
	void MakeTCPheader( int Socket, unsigned char TCP_flags, int Datalenght, unsigned int Windowsize, char *buffer );
	
	int RegisterTCPPort( unsigned int Port );
	void RegisterTCPCallBack( int socket , TCP_CALLBACK_FUNC pFunc );
	void UnRegisterTCPPort( unsigned int Port );
	int CheckPortInList( unsigned int Port );
	int CheckPortRequest( unsigned int Port );
	char CheckSocketState( int Socket );
	void CloseTCPSocket( int Socket);
	int Connect2IP( long IP, unsigned int Port );
	
	void TCP_pharseOptions( int Socket, unsigned char * Optionfield, int length );
	int GetSocketData( int Socket , int bufferlen, char * buffer);
	int PutSocketData_RPE( int Socket, int Datalenght, char * Sendbuffer, char Mode );
	int SendData_RPE( int Socket, int Datalenght, char * Sendbuffer, char Mode, int RetransmissionCounter );
	int FlushSocketData( int Socket );
	int GetSocketDataToFIFO( int Socket , int FIFO, int bufferlen );

	char GetByteFromSocketData( int Socket );
	int GetBytesInSocketData( int Socket );
	
	#define	RAM		0
	#define FLASH	1

	#define Lock	0
	#define Unlock	1

	// Struktur für den Pseudoheader, dieser wird bei der Berechnung der TCP-Checksumme benötigt und wird vor dem TCP-header gesetzt.
	// Jetzt wird zur berechnung der Checksumme im TCP-header die Checksumme auf 0 gesetzt.
	// Danach wird die Checksumme für den Pseudoheader, TCP-header und dem Nutzdatensgment ( TCP_Segment ) berechnet.
	// wenn die Checksumme errechnet ist kann wird diese in die TCP-header geschrieben.
	// wenn alles okay kann der empfängr auf dem selben weg rückwerts die checksumme kontrollieren und bekommt als ergenis 0 zurück, weil
	// das ergebnis der Checkumme ja auch im TCP_header mit drinne steht.
	
	#define IP_PSEUDOHEADER_LENGHT 12

	struct IP_Pseudoheader {
			volatile unsigned long IP_SourceIP;			// Quell IP
			volatile unsigned long IP_DestinationIP;		// Ziel IP
			volatile unsigned char IP_ZERO;				// muss 0 sein
			volatile unsigned char IP_Protokoll;			// bei TCP immer 6
			volatile unsigned int IP_TCP_lenght;			// länge des Nutzdaten Segmentes ( TCP_Segment )
	};
	
	#define MAX_LISTEN_PORTS 4					// Anzahl der Port auf den gelauscht wird
	#define TCP_Port_not_use 0
	
	struct TCP_PORT {							// Struktur für Ports
			volatile unsigned int TCP_Port;
	};
	
	// Struktur für den Connection SOocket. Hier werden die Sockets für die TCP-Verbindungen angelegt und den wichtige Werte gespeichert.
	// damit kann man jede verbindung genau nachverfolgen und auch halten.

	#if defined(EXTMEM)
		#define MAX_TCP_CONNECTIONS			4	// maximale gleichzeitige verbindungen
	#elif defined(__AVR_ATmega1284P__)
		#define MAX_TCP_CONNECTIONS			4	// maximale gleichzeitige verbindungen
	#else
		#define MAX_TCP_CONNECTIONS			3	// maximale gleichzeitige verbindungen
	#endif

	#define MAX_TCP_RETRANSMISSIONS			3

	#if defined(EXTMEM)
		#if defined(LEDTAFEL)
			#define MAX_RECIVEBUFFER_LENGHT 	1024*1
		#else
			#define MAX_RECIVEBUFFER_LENGHT 	1024*4
		#endif
	#elif defined(__AVR_ATmega1284P__)
		#define MAX_RECIVEBUFFER_LENGHT 	512
	#else
		#define MAX_RECIVEBUFFER_LENGHT 	48
	#endif

	#define CLOSETIMEOUT				200		// angabe in 1/100 sec
	#define RETRANSMISSIONTIMEOUT		200		// der timeout für retransmission
	#define CONNECTTIMEOUT				500		// angabe in 1/100 sec
	#define TimeOutCounter 				120		// angabe in 1 sec
	#define CloseTimeout				2

	struct TCP_SOCKET {
			volatile char ConnectionState;				// 1 Byte
			volatile unsigned char SendState;					// 1 Byte
			volatile unsigned int SourcePort;					// 2 byte
			volatile unsigned int DestinationPort;				// 2 Byte
			volatile unsigned long SourceIP;					// 4 Byte	
			volatile unsigned long SequenceNumber;				// 4 Byte
			volatile unsigned long AcknowledgeNumber;			// 4 Byte
			volatile unsigned int Windowsize;					// 2 Byte
			volatile unsigned int SendetBytes;					// 2 Byte
			volatile unsigned int Timeoutcounter;				// 2 Byte
			TCP_CALLBACK_FUNC TCP_CallbackFunc;

#if defined(TCP_RTT)
			volatile unsigned long TSval;
			volatile unsigned long TSecr;
#endif
			char MACadress[6];				// 6 Byte
			volatile unsigned int fifo;
			char Recivebuffer[ MAX_RECIVEBUFFER_LENGHT ];
	};
	
	// definitionen für die ConnectionState
	
	#define SOCKET_NOT_USE		0			// SOCKET ist Frei	
	#define SOCKET_SYNINIT		1			// SOCKET SYN init
	#define SOCKET_WAIT2SYNACK	4			// SOCKET hat SYN & ACK gesendet und wartet auf ACK
	#define SOCKET_SYNACK_OK	5			// SOCKET hat SYN+ACK nachen einen SYN empfangen ( fuer berbindungsaufbau )
	#define SOCKET_READY2USE	15			// SYN erfolgreich und Socket bereit zur übernahme durch den Anwender
	#define SOCKET_READY		16			// Socket ist Bereit zur Benutzung
	#define SOCKET_WAIT2FINACK	113			// SOCKET wartet auch das FIN + ACK nach einem FIN + ACK
	#define SOCKET_WAIT2FIN		114			// SOCKET wartet auch das ACK nach einem FIN + ACK
	#define SOCKET_RESET		115			// SOCKET empfängt RST ( Reset )
	#define SOCKET_CLOSED		127			// SOCKET geschlossen

    #define NO_SOCKET_USED 		-1           // Rueckgabewert
    #define SOCKET_ERROR 		-1           // Rueckgabewert
	
	// definitionen für die SendState
	
	#define SOCKET_READY2SEND	0x00			// Socket bereit zum senden
	#define SOCKET_DATASENDED	0x80			// Socket hat Daten gesendet aber Window ist nicht voll und wartet auf ACK für Daten
	#define SOCKET_BUSY			0xff			// Socket hat Daten gesendet und warten auf ACK für Daten
	

	#define TCP_HEADER_LENGTH 	36

	#if defined(EXTMEM)
		#define MAX_TCP_Datalenght 	1452
	#elif defined(__AVR_ATmega1284P__)
		#define MAX_TCP_Datalenght 	512
	#else
		#define MAX_TCP_Datalenght 	48
	#endif

	struct TCP_UNSORT {
			unsigned int lenght;
			unsigned int socket;
			unsigned long Sequencenumber;
			char Recivebuffer[ MAX_TCP_Datalenght ];
	};

// der TCP-header :-)
	
	struct TCP_header {
			volatile unsigned int TCP_SourcePort;
			volatile unsigned int TCP_DestinationPort;
			volatile unsigned long TCP_SequenceNumber;
			volatile unsigned long TCP_AcknowledgeNumber;
			volatile unsigned char TCP_DataOffset;
			volatile unsigned char TCP_ControllFlags;
			volatile unsigned int TCP_Window;
			volatile unsigned int TCP_Checksum;
			volatile unsigned int TCP_UrgentPointer;
			volatile unsigned char TCP_Options[16];
	};
			
	// die einzelnen Controllflags des TCP-headers
	
	#define  TCP_NON_FLAG			0x00		// kein Flag gesetzt
	#define  TCP_FIN_FLAG			0x01		// Verbindung beenden gesenden/empfangen
	#define  TCP_SYN_FLAG			0x02		// Verbindung aufbauen gesenden/empfangen
	#define  TCP_RST_FLAG			0x04		// Reset gesenden/empfangen
	#define  TCP_PSH_FLAG			0x08		// Push gesendet/empfangen
	#define  TCP_ACK_FLAG			0x10        // Acknowledge gesendet/empfangen
	#define  TCP_URG_FLAG			0X20		// Urgentflag für dringlichkeit gesetzt
	
#endif

