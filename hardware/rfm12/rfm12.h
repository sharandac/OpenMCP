/***************************************************************************
 *            rfm12.h
 *
 *  Sun Apr 17 17:41:35 2011
 *  Based on Firmware version 2.0.1 by Jürgen Eckert
 *  -> http://www.mikrocontroller.net/articles/AVR_RFM12
 *	Ported to OpenMcp by Jan Wissel 
 *	OpenMcp, by  Dirk BroÃwick
 *  <sharandac@snafu.de>
 *  for documentation see:
 *  http://www.mikrocontroller.net/articles/RFM12 
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
 
#ifndef _RFM12_H

	#define _RFM12_H
	#if defined(AVRNETIO) && defined(AVRNETIO_ADDON)
	
	// core functionality
	// ISR, registered in RFM12_init
	void rfrecieve(void);
	void rftransmit(void);
	void RFM12_isr(void);
	
	// Initialization
	void RFM12_init( void );
	
	//Operational Control
	unsigned char RFM12_rxstart(void);
	unsigned char RFM12_rxfinish(unsigned char *data);
	unsigned char RFM12_txstart(unsigned char *data, unsigned char size);
	unsigned char RFM12_txfinished(void);
	void RFM12_allstop(void);


    // internal used functions. 
	// usually not called from elsewhere    
	void RFM12_config(void);
    void RFM12_setfreq(unsigned short freq);
    void RFM12_setbandwidth(unsigned char bandwidth, unsigned char gain, unsigned char drssi);
    void RFM12_setbaud(unsigned short baud);
    void RFM12_setpower(unsigned char power, unsigned char mod);
    unsigned int crcUpdate(unsigned int crc, unsigned char serialData);
	
	// hardware level functions
	unsigned int RFM12_trans( unsigned int CMD );
	void RFM12_select( void );
	void RFM12_deselect( void );
    
	// Hardware Configuration
	#define	RFM12_SPI_BUS_NUM		0

	#define	RFM12_CS_DDR			DDRD
	#define	RFM12_CS_PORT			PORTD
	#define	RFM12_CS_PIN			PD5

	#define	RFM12_IRQ_DDR			DDRD
	#define	RFM12_IRQ_PORT			PORTD
	#define	RFM12_IRQ_PIN			PD3
	#define	RFM12_IRQ_EXINT_NUM		1
    
    #define RFM12_BAUDRATE   		19200
    #define RFM12_DataLength		50           


       
	/* ------ */
	//struct that contains the status of RFM12 
	struct RFM12_stati{
		unsigned char Rx:1;
		unsigned char Tx:1;
		unsigned char New:1;
	};
	//macro for converting data for SPI transfers
	typedef union conver2_ {
        unsigned int w;
        unsigned char b[2];
    } CONVERTW;

	//definitions according to RFM12 datasheet
	#define RxBW400		1
	#define RxBW340		2
	#define RxBW270		3
	#define RxBW200		4
	#define RxBW134		5
	#define RxBW67		6

	#define TxBW15		0
	#define TxBW30		1
	#define TxBW45		2
	#define TxBW60		3
	#define TxBW75		4
	#define TxBW90		5
	#define TxBW105		6
	#define TxBW120		7

	#define LNA_0		0
	#define LNA_6		1
	#define LNA_14		2
	#define LNA_20		3

	#define RSSI_103	0
	#define RSSI_97		1
	#define RSSI_91		2
	#define RSSI_85		3
	#define RSSI_79		4
	#define RSSI_73		5
	#define RSSI_67		6
	#define	RSSI_61		7

	#define PWRdB_0		0
	#define PWRdB_3		1
	#define PWRdB_6		2
	#define PWRdB_9		3
	#define PWRdB_12	4
	#define PWRdB_15	5
	#define PWRdB_18	6
	#define PWRdB_21	7

	#define RFM12TxBDW(kfrq)	((unsigned char)(kfrq/15)-1)
	#define RFM12FREQ(freq)		((freq-430.0)/0.0025)	// macro for calculating frequency value out of frequency in MHz
	
	#else
		#error "RFM12 wird fuer diese Hardwareplatform nicht unterstuetzt, bitte in der config.h abwaehlen!"
	#endif
	

#endif