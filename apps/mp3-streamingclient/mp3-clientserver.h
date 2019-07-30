/*! \file mp3-clientserver.h \brief Das Tsunami-Steuerinferface */
//***************************************************************************
//*            mp3-clientserver.h
//*
//*  Mon May 18:46:47 2008
//*  Copyright  2008  sharandac
//*  Email: sharandac@snafu.de
///	\ingroup software
///	\defgroup mp3client Funktionen für den VS10xx bereit um zu streamen (mp3-clientserver.h)
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
 
#ifndef _MP3_CLIENTSERVER_H
	#define _MP3_CLIENTSERVER_H

	#define HTML 	0
	#define TELNET	1

	int cmd_mp3stream( int argc, char ** argv );
	void PlayURL( char * URL );
	void RePlayURL( void );
	void StopPlay( void );
	void PharseURL( char * URLBuffer );
	void mp3clientupdate( long bandwidth, int tcpbuffer, int streamingbuffer,  int decodetime, char verboselevel );
	void mp3clientupdateMETATITLE( char * streamingTITLE );
	void mp3clientupdateNAME( char * streamingNAME );
	void mp3clientupdateMETAURL( char * streamingURL );
	void mp3clientupdateGENRE( char * streamingGENRE );
	void mp3clientupdateBITRATE( int streamingBITRATE );
	void mp3clientPrintInfo( int SOCKET, char type );
	void mp3client_PrintHeaderinfo( void );
	char * mp3client_geturl( void );

	struct MP3_STATS {
//		char 	trash;
		long 			streamingIP;
		char 			streamingPROTO[8];
		char 			streamingURL[128];
		unsigned int 	streamingPORT;
		char 			streamingFILE[128];
		char 			streamingNAME[256];
		char 			streamingMETATITLE[256];
		char			streamingMETAURL[128];
		char			streamingGENRE[128];
		int				streamingBITRATE;
		long			streamingrate;
		int				streamingtcpbuffer;
		int				streamingbuffer;
		long			streamingtime;
		char			streamingverboselevel;
	};

#endif /* _MP3_CLIENTSERVER_H */
