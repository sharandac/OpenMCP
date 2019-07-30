/***************************************************************************
 *            shell.h
 *
 *  Tue May 25 23:37:37 2010
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

#ifndef SHELL_H
	#define SHELL_H

	#define SHELL_BUFFER_LEN	64

	#define MAX_SHELL_ENTRYS	16
	#define MAX_ARGC			4

	typedef int ( * DYN_SHELL_CALLBACK ) ( int argc, char ** argv );

	struct DYN_SHELL {
		volatile	DYN_SHELL_CALLBACK		dynshell_function;
		const char  						* functionname;
	};

	void SHELL_init( void );
	int SHELL_RegisterCMD( DYN_SHELL_CALLBACK dynshell_function, const char * funktionname );
	int SHELL_pharse( char * BUFFER, char ** argv, int max_argc );
	int SHELL_runcmd( int argc, char ** argv );
	int SHELL_runcmdextern( char * cmdstring );
	int SHELL_runcmdextern_P( const char * cmdstring_P );

#endif // SHELL_H
