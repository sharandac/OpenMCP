/***************************************************************************
 *            cgi-bin.h
 *
 *  Tue Jun 24 17:36:32 2008
 *  Copyright  2008  sharan
 *  <sharan@bastard>
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
 * \ingroup httpd2
 * \addtogroup cgibin
 *
 * @{
 */

/**
 *
 * \author Dirk Broßwick
 *
 */

#ifndef _CGI_BIN_H
	#define _CGI_BIN_H

	#include <stddef.h>

	#define MAX_CGI_ENTRYS 		20

	typedef void ( * DYN_CGI_CALLBACK ) ( void * pStruct );

	void cgi_init( void );
	int cgi_RegisterCGI( DYN_CGI_CALLBACK dyncgi_function, const char * funktionname );
	int check_cgibin( void * pStruct );
	void cgi_PrintHttpheaderStart( void );
	void cgi_PrintHttpheaderEnd( void );

	/*! \struct DYN_CGIBIN
	 *  \brief Struktur fuer die hinterlegten CGI-Funktionen
	 */
	struct DYN_CGIBIN {
		volatile	DYN_CGI_CALLBACK	dyncgi_function;	/*!< Pointer auf die CGI-Funktion */
		const char 	* funktionname;							/*!< Pointer in Flash auf die Beschreibung zur CGI-Funktion */
	};

#endif /* CGI_BIN_H */

/**
 * @}
 */
