/*! \file dns.h \brief Stellt Aufloesung von Hostnmen nach IP (dns.c) */
//***************************************************************************
//*            dns.h
//*
//*  Mon Aug 21 21:34:26 2006
//*  Copyright  2006  Dirk Broßwick
//*  Email: sharandac@snafu.de
//****************************************************************************/
/**
 * \addtogroup network
 * \addtogroup DNS
 * \code #include "dns.h" \endcode
 * \par Uebersicht
 * Loest die Hostnamen in eine IP-Adresse auf um weiter zu verarbeiten.
 * Als IP fuer den DNS-Server dienst DNSserver aus ip.c .
 */
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

//@{

#ifndef _DNS_H
	#define _DNS_H

	#define DNS_BUFFER_LENGHT	210
	#define DNS_SERVER_PORT		53
	#define DNS_NO_ANSWER		0xffffffff
	#define DNS_REQUEST_TIMEOUT 200			// in 1/100 sekunden
	
	/*! \name Answertypes
	 * Die moeglichen DNS_answer Typen an Antworten bei DNS-Anfragen
	 */
	//@[
	#define CNAME				0x0005
	#define A_HOSTNAME			0x0001
	//@}
	unsigned long DNS_ResolveName( char * HOSTNAME );
	unsigned long DNS_ResolveName_P( const char * HOSTNAME_P );
	unsigned int DNS_convertHostName( char * HOSTNAME , unsigned char * Destbuffer );
	
	#define DNS_HEADER_LENGHT   12
	
	/*! \struct DNS
	 *  \brief Der DNS-Header fuer DNS-anfragen.
	 */
	struct DNS_header {
		unsigned int TransactionID;
		unsigned int Flags;
		unsigned int Questions;
		unsigned int Answer_RRs;
		unsigned int Authority_RRs;
		unsigned int Additional_RRs;
		unsigned char Queries[];
	};
	
	#define DNS_ANSWER_HEADER_LENGHT	12
	
	/*! \struct DNS_answer
	 *  \brief Die DNS_answer Struktur fuer Antworten des DNS-Server
	 */
	struct DNS_answer {
		unsigned int Name;
		unsigned int Type;			/*!< Typ der Antwort */
		unsigned int IN;
		unsigned long TTL;
		unsigned int Datalenght;
		unsigned long Adress;
	};
	
#endif /* _DNS_H */
//@}
