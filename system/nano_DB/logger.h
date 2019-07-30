/***************************************************************************
 *            logger.h
 *
 *  Sun Sep  5 16:59:05 2010
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

#ifndef LOGGER_H_
#define LOGGER_H_

#include "system/clock/clock.h"

	struct LOGGER_ENTRY
	{
		struct TIME		time;
		int				DataSize;
	};

	#define LOGGER_OK		1
	#define LOGGER_ERROR	0

	int LOGGER_getlastDBEntry( struct LOGGER_ENTRY * logger_entry, char * FILENAME );
	int LOGGER_writeDBEntry( struct LOGGER_ENTRY * logger_entry, char * FILENAME );
	int LOGGER_addDBentry( struct LOGGER_ENTRY * logger_entry , char * FILENAME );

#endif // LOGGER_H_
