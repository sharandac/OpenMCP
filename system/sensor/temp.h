/***************************************************************************
 *            temp.h
 *
 *  Sat Dec 26 21:53:01 2009
 *  Copyright  2009  Dirk Broßwick
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

/**
 * \defgroup Sensor Interface um Tempsensoren anzusprechen (Sensor.c)
 *
 * Mit diesen Modul kann man Temperaturen einheitlich auslesen. Dazu kann man den
 * der Funktion einen String übergeben in dem alles steht wie:
 *
 * @{
 */

/**
 * \file
 * Mit diesen Modul kann man Temperaturen einheitlich auslesen. Dazu kann man den
 * der Funktion einen String übergeben in dem alles steht wie:
 *
 * "TWI;4f;Wohnzimmer"
 *
 * oder
 *
 * "ONEWIRE;xx:xx:xx:xx:xx:xx:xx:xx;Wohnzimmer"
 *
 * \author Dirk Broßwick
 */

#ifndef TEMP_H
	#define TEMP_H

	void TEMP_init( void );
	void TempCache_Timeouthandler( void );
	int TEMP_readtempstr( char * SensorData );
	int TEMP_readtemp( int Sensor );
	char * TEMP_Temp2String( int Temp, char * TempString );
	void TEMP_Sensor2String( char Sensor, char * SensorString );
	char * TEMP_getSensorConfig( char Sensor, char * SensorString );
	int	TEMP_readUDPtemp( char * InetAddress );
	void TEMP_thread( void );
	void TEMP_config_cgi( void * pStruct );

	#define TEMP_MAX_SENSORS		8
	#define TEMP_ERROR				0x8000
	#define TEMPSENSOR_STORETIME	45

	#define SENSOR_PORT				4223

	struct TempRequest {
		char Sensor;
		int Temp;
	};

	struct TempCache {
		unsigned int counter;
		unsigned int temp;
	};

#endif

/**
 * @}
 */
