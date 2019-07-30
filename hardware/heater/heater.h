/***************************************************************************
 *            heater.h
 *
 *  Do Februar 26 19:31:28 2015
 *  Copyright  2015  Dirk Broßwick
 *  <sharandac@snafu.de>
 ****************************************************************************/
/*
 * heater.h
 *
 * Copyright (C) 2015 - Dirk Broßwick
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _HEATER_H
	#define _HEATER_H

	#define HEATER_PIN1 0
	#define HEATER_PIN2 1
	#define HEATER_OFF 0
	#define HEATER_ON  1

	void HEATER_init( void );
	void HEATER_work( void );
	void HEATER_save( void );
	void HEATER_set( int temp );
	void HEATER_on( void );
	void HEATER_off( void );
	long HEATER_get_time( void );
	int HEATER_get_temp( void );
	char HEATER_get_status( void );

#endif /* _HEATER_H */