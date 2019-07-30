/*! \file fifo.h \brief Stellt die FIFO Funkionalitaet bereit */
//***************************************************************************
//*            fifo.h
//*
//*  Thu Apr  3 23:01:42 2008
//*  Copyright  2008 Dirk Broßwick
//*  Email: sharandac@snafu.de
//****************************************************************************/
///	\ingroup system
///	\defgroup FIFO Die FIFO-Puffer funktionalität (fifo.c)
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
#ifndef _FIFO_H
	#define _FIFO_H

	#include "config.h"

	void INIT_FIFO( void );
	int Get_FIFO( char * buffer, int bufferlenght );
	int Free_FIFO ( int FIFO );
	int Flush_FIFO( int FIFO );

	int Put_Byte_in_FIFO( int FIFO, char Byte );
	int Put_Block_in_FIFO( int FIFO, int bufferlenght, char * buffer );
	unsigned char Get_Byte_from_FIFO( int FIFO );
	int Get_Block_from_FIFO( int FIFO, int bufferlenght, char * buffer );

	int Get_FIFO_to_FIFO( int Src_FIFO, int bufferlenght, int Dest_FIFO );

	int Get_Bytes_in_FIFO( int FIFO );
	int Get_FIFOsize( int FIFO );
	int Get_FIFOrestsize( int FIFO );
	
	unsigned char Read_Byte_ahead_FIFO( int FIFO, int Pos );


	#if defined(EXTMEM)
		#define	MAX_FIFO_BUFFERS	16
	#else
		#define	MAX_FIFO_BUFFERS	6
	#endif

	#define	FIFO_ERROR		-1
	#define FIFO_OK			0x00
	#define FIFO_UNLOCK		0xff
	#define	FIFO_LOCK		0x00

	#define _fastcopy_
//	#define FIFO_BLOCKCOPYSTATS

	struct FIFO {
		char	* buffer;
		unsigned int 	bufferlenght;
		unsigned int	writepointer;
		unsigned int	readpointer;
		unsigned int	byteinbuffer;
#ifdef FIFO_BLOCKCOPYSTATS
		unsigned long	Blockcopyhit;
		unsigned long	Bytecopyhit;
#endif
		unsigned char	lock;
	};

#endif /* _FIFO_H */
//@}
