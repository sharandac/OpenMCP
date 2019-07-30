/*! \file httpd2-pharse.h \brief Ein sehr einfacher HTTP-Server */
//***************************************************************************
//*            httpd2-pharse.h
//*
//*  Mon Jun 23 14:19:16 2008
//*  Copyright  2006 Dirk Broßwick
//*  Email: sharandac@snafu.de
//****************************************************************************/
///	\ingroup httpd2
///	\addtogroup pharse
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
//@{
 
#ifndef _HTTPD2_PHARSE_H
	#define _HTTPD2_PHARSE_H

	#include "httpd2.h"

	void PharseRequest( void * pStruct );
	void DecodeRequestString( char * String );
	int PharseCheckName( void * pStruct , char * ArgName );
	int PharseCheckName_P( void * pStruct , const char * ArgName );
	int PharseGetValue( void * pStruct , char * ArgName );
	int PharseGetValue_P( void * pStruct , const char * ArgName );

#endif /* _HTTPD2_PHARSE_H */

//@}
