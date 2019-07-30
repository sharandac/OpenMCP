/*! \file telnet.h \brief Ein sehr einfacher Telnetclient */
/***************************************************************************
 *            telnet.h
 *
 *  Mon Jun 19 21:56:05 2006
 *  Copyright  2006 Dirk Broßwick
 *  Email: sharandac@snafu.de
 ****************************************************************************/
///	\ingroup software
///	\defgroup telnet Ein sehr einfacher Telnetclient (telnet.h)
///	\code #include "telnet.h" \endcode
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
#ifndef _TELNET_H
	#define _TELNET_H

	#include <avr/pgmspace.h>  
	#include "system/shell/shell.h"
	#include "config.h"

	#define TELNET_PORT 23
	#if defined(EXTMEM)
		#define TELNET_BUFFER_LEN 128
	#else
		#define TELNET_BUFFER_LEN 48
	#endif

	struct TELNET_SESSION {
		int			SOCKET;
		char		BUFFER [ TELNET_BUFFER_LEN ];
		int			POS;
		int			STATE;
		int			argc;
		char *		argv[ MAX_ARGC ];
	};

	void telnet_init( void );
	void telnet_thread( void );

#endif /* _TELNET_H */
//@}
