/***************************************************************************
 *            temp.c
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
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include <avr/version.h>
#include <string.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"

/**
 * \ingroup system
 * \addtogroup Sensor Interface um Tempsensoren anzusprechen (Sensor.c)
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
 * \author Dirk Broßwick
 */

#include "apps/httpd/cgibin/cgi-bin.h"
#include "apps/httpd/httpd2.h"
#include "apps/httpd/httpd2_pharse.h"

#if defined(TWI)
	#include "hardware/twi/lm75.h"
#endif
#if defined(ONEWIRE)
	#include "hardware/1wire/DS16xxx.h"
#endif
#if defined(UDP)
	#include "system/net/ip.h"
	#include "system/net/udp.h"
	#include "system/net/dns.h"
	#include "system/clock/clock.h"
	#include "system/thread/thread.h"
	#include "system/config/eeconfig.h"
	#include "hardware/led/led_core.h"
#endif
#include "system/string/string.h"
#include "temp.h"

const char TEMP_EECONFIGNAME_P[] PROGMEM = "TEMPSENSOR_%02d";
struct TempCache TEMPCache [ TEMP_MAX_SENSORS ];

/*------------------------------------------------------------------------------------------------------------*/
/**
 * \brief	Initialisiert das Tempsensormodul
 */
 /*------------------------------------------------------------------------------------------------------------*/
void TEMP_init( void )
{
#if defined(UDP)
		THREAD_RegisterThread( TEMP_thread , PSTR("Tempsensor thread"));
#endif
#if defined(HTTPSERVER)
	cgi_RegisterCGI( TEMP_config_cgi, PSTR("tempconfig.cgi"));
#endif
	for( int i = 0 ; i < TEMP_MAX_SENSORS ; i++ )
	{
		TEMPCache[ i ].counter = 0;
		TEMPCache[ i ].temp = TEMP_ERROR;
	}
	
	CLOCK_RegisterCallbackFunction( TempCache_Timeouthandler, SECOUND );
}

/*------------------------------------------------------------------------------------------------------------*/
/**
 * \brief	Timeouthandler für gecachte Temperaturwerte
 */
/*------------------------------------------------------------------------------------------------------------*/
void TempCache_Timeouthandler( void )
{
	for ( int i = 0 ; i < TEMP_MAX_SENSORS ; i++ )
		
	if ( TEMPCache[ i ].counter != 0 )
	{
		TEMPCache[ i ].counter--;
		if ( TEMPCache[ i ].counter == 0 )
		{
			TEMPCache[ i ].temp = TEMP_ERROR;
		}
	}
}

/*------------------------------------------------------------------------------------------------------------*/
/**
 * \param	SensorConfig	Pointer auf einen ConfigString und liest davon die Temperatur ein.
 *							Beispiel für einen ConfigString:
 *
 * 							"TWI;4f;Wohnzimmer"
 *
 * 							"ONEWIRE;xx:xx:xx:xx:xx:xx:xx:xx;Wohnzimmer"
 *
 * 							"UDP;IP/XX;Wohnzimmer"
 *
 * \return	Temp2			Temperatur im 8.8 Fixkommaformat oder TEMP_ERROR
 */
/*------------------------------------------------------------------------------------------------------------*/
int TEMP_readtempstr( char * SensorConfig )
{
	char * BUS_String = NULL;
	char * ID_String = NULL;
	char * NAME_String = NULL;
	int Temp = TEMP_ERROR;

	BUS_String = SensorConfig;
	
	// String zerlegen für ID
	while( * SensorConfig )
	{
		if ( * SensorConfig == ';' )
		{
			* SensorConfig = '\0';
			NAME_String = SensorConfig;
			SensorConfig++;
			ID_String = SensorConfig;
			break;
		}
		SensorConfig++;
	}

	// weiter suchen nach Namen
	while( * SensorConfig )
	{
		if ( * SensorConfig == ';' )
		{
			* SensorConfig = '\0';
			SensorConfig++;
			NAME_String = SensorConfig;
			break;
		}
		SensorConfig++;
	}

#if defined(TWI) || defined(ONEWIRE)
	char ID[8];
#endif

#if defined(TWI)
	if ( !strcmp_P( BUS_String, PSTR("TWI") ) )
	{
		strtobin( ID_String, ID, 2 );
		Temp = LM75_readtemp( ID );
	}
#endif
#if defined(ONEWIRE)
	int Temp2;
	
	if ( !strcmp_P( BUS_String, PSTR("ONEWIRE") ) )
	{
		strtobin( ID_String, ID, 16 );
		Temp = DS16xxx_readtemp( ID );

		// Wenn Temp komisch ist, nochmal einlesen
		if ( Temp == 0xfff0 )
		{
			Temp2 = DS16xxx_readtemp( ID );
			if ( Temp2 != Temp )
				Temp = Temp2;
			else
				Temp = TEMP_ERROR;
		}
	}
#endif
#if defined(UDP)
	if ( !strcmp_P( BUS_String, PSTR("UDP") ) )
	{
		Temp = TEMP_readUDPtemp( ID_String );
	}
#endif
	
	if ( ID_String != NULL )
	{
		ID_String--;
		* ID_String = ';';
	}
	if ( NAME_String != NULL )
	{
		NAME_String--;
		* NAME_String = ';';
	}
	return( Temp );
}

/*------------------------------------------------------------------------------------------------------------*/
/**
 * \brief	Liest die Temperatur von einen Sensor ein deren Configstring im Configspeicher hinterlegt ist.
 * \param	Sensor	Die Nummer des Sensor die eingelesen werden soll
 * \return	TEMP_ERROR wenn Fehler, sonst Temperatur im 8.8 Format.
 */
/*------------------------------------------------------------------------------------------------------------*/
int TEMP_readtemp( int Sensor )
{
	int Temp = TEMP_ERROR;
	char TEMPSENSOR[64];

	if ( TEMPCache[ Sensor ].counter != 0 )
	{
		Temp = TEMPCache[ Sensor ].temp;
	}
	else
	{
		if ( TEMP_getSensorConfig( Sensor, TEMPSENSOR ) != NULL )
		{
			Temp = TEMP_readtempstr( TEMPSENSOR );
			TEMPCache[ Sensor ].temp = Temp;
			if ( Temp == TEMP_ERROR )
				TEMPCache[ Sensor ].counter = 0;
			else
				TEMPCache[ Sensor ].counter = TEMPSENSOR_STORETIME;
		}
	}
	return( Temp );
}

/*------------------------------------------------------------------------------------------------------------*/
/**
 * \brief	Liest den Configstring zu einem Sensor aus dem Configspeicher ein.
 * \param	Sensor			Nummer des Sensor im Configspeicher.
 * \param	SensorString	Pointer auf einen Speicherbereich in welchen der ConfigString eingelesen wird.
 * \return	NULL wenn Fehler, sonst Pionter auf den String, String == SensorString.
 */
/*------------------------------------------------------------------------------------------------------------*/
char * TEMP_getSensorConfig( char Sensor, char * SensorString )
{
	sprintf_P( SensorString, TEMP_EECONFIGNAME_P , Sensor );

	if ( readConfig( SensorString , SensorString ) == -1 )
		SensorString = NULL;

	return( SensorString );
}

/*------------------------------------------------------------------------------------------------------------*/
/**
 * \brief	Wandelt Temperaturwert im 8.8 Format in einen String.
 * \param	Temp		Die Temperatur im 8.8 Format die in einen String umgewandelt werden soll.
 * \param	TempString	Pointer auf den Speicher in den der String abgelegt werden soll.
 * \return	NULL wenn Fehler, sonst Pointer auf den String, String == TempString.
 */
/*------------------------------------------------------------------------------------------------------------*/
char * TEMP_Temp2String( int Temp, char * TempString )
{
	char VZ;
	
	if ( Temp != TEMP_ERROR )
	{
		if ( Temp < 0 )
		{
			VZ = '-';
			Temp = ~(Temp - 1);
		}
		else
		{
			VZ = '+';
		}
		sprintf_P( TempString, PSTR("%c%d.%02d"), VZ, Temp >> 8 , ( ( Temp & 0xff ) * 100 ) / 256 ) ; // 0.1øC
	}
	else
	{
		sprintf_P( TempString, PSTR("n/a"));
		TempString = NULL;
	}
	
	return( TempString );
}

/*------------------------------------------------------------------------------------------------------------*/
/**
 * \brief	Liest die Temperatur eines Sensors und wandelt ihn gleich in einen String um.
 * \param	Sensor			Nummer des Sensor der im Configspeicher hinterlegt ist.
 * \param	SensorString	Pointer auf einen freien Speicher in den der String abgelegt wird.
 */
/*------------------------------------------------------------------------------------------------------------*/
void TEMP_Sensor2String( char Sensor, char * SensorString )
{
	int Temp;

	Temp = TEMP_readtemp( Sensor );
	TEMP_Temp2String( Temp, SensorString );
}

/*------------------------------------------------------------------------------------------------------------*/
/**
 * \brief	Liest die Temeratur eines entfernten Tempsensor über das Netzwerk ein.
 * \param	InetAddress		Pointer auf einen String der den ConfigString enthält.
 * \return	TEMP_ERROR wenn Fehler, sonst Temperatur im 8.8 Format.
 */
/*------------------------------------------------------------------------------------------------------------*/
int	TEMP_readUDPtemp( char * InetAddress )
{
	struct TempRequest UDPBuffer;

	char * Hostname = NULL;;
	int timer, temp = TEMP_ERROR, socket ;
	long IP;

	Hostname = InetAddress;

	// String zerlegen
	while( * InetAddress != ':' && * InetAddress != '\0') InetAddress++;

	if ( InetAddress == '\0' )
	{	
		return( temp );
	}
	else
	{
		* InetAddress = '\0';
		InetAddress++;
		UDPBuffer.Sensor = atoi( InetAddress );
		UDPBuffer.Temp = temp;
	}
	
	// String Hostname/IP auflösen
	if ( strtoip ( Hostname ) != 0 )
		IP = strtoip ( Hostname );
	else
		IP = DNS_ResolveName( Hostname );

	if ( IP == DNS_NO_ANSWER )
	{
		return( temp );
	}

	// String wieder herstellen
	InetAddress--;
	* InetAddress = ':';
	
	// UDP-Socket reservieren
	socket = UDP_RegisterSocket( IP , SENSOR_PORT , sizeof( struct TempRequest ) , (char*)&UDPBuffer);
	// Wenn Fehler aufgetretten, return
	if ( socket == UDP_SOCKET_ERROR ) 
	{
		UDP_CloseSocket( socket );
		return ( temp );
	}

	// UDP-Anfrage senden
	UDP_SendPacket( socket, sizeof( struct TempRequest ) , (char*)&UDPBuffer);

	// Timeout-counter reservieren und starten
	timer = CLOCK_RegisterCountdowntimer();
	if ( timer == CLOCK_FAILED )
	{
		UDP_CloseSocket( socket );
		return ( temp );
	}
	
	CLOCK_SetCountdownTimer( timer , 500, MSECOUND );
	
	// Auf Antwort des Timer-Servers warten
	while( 1 )
	{
		// Wenn Temp-Server geantwortet hat inerhalb des Timeouts, hier weiter
		if ( UDP_GetSocketState( socket ) == UDP_SOCKET_BUSY && ( CLOCK_GetCountdownTimer( timer ) != 0 ) )
		{
			// Sind 4 Bytes empfangen worden, wenn ja okay, sonst fehler
			if ( UDP_GetByteInBuffer( socket ) == sizeof( struct TempRequest ) )
			{		
				temp = UDPBuffer.Temp;
				break;
			}
			else
				break;
		}
		// Timeout erreicht ? Wenn ja Fehler.
		if ( CLOCK_GetCountdownTimer( timer ) == 0 )
			break;

		// Timeout erreicht ? Wenn ja Fehler.
		if ( CLOCK_GetCountdownTimer( timer ) == 50 )
			UDP_SendPacket( socket, sizeof( struct TempRequest ) , (char*)&UDPBuffer);

		if ( CLOCK_GetCountdownTimer( timer )%100 )
			TEMP_thread();
	}
	// timer freigeben und UDP-Socket schliessen
	CLOCK_ReleaseCountdownTimer( timer );

	UDP_CloseSocket( socket );
	return( temp );	
}

/*------------------------------------------------------------------------------------------------------------*/
/**
 * \brief	Dienst der die Abfrage eine Sensors über Netzwerk erlaubt.
 */
/*------------------------------------------------------------------------------------------------------------*/
void TEMP_thread( void )
{
	static int UDP_Socket = UDP_SOCKET_ERROR;
	static struct TempRequest UDPBuffer;

	// if an Socket created or opened, if not, create them ?
	if ( UDP_Socket == UDP_SOCKET_ERROR )
	{
		UDP_Socket = UDP_ListenOnPort( SENSOR_PORT, sizeof( struct TempRequest ), (char*)&UDPBuffer );
	}	
	else
	{
		if ( UDP_GetSocketState( UDP_Socket ) == UDP_SOCKET_BUSY )
		{
			UDPBuffer.Temp = TEMP_readtemp( UDPBuffer.Sensor );
			UDP_SendPacket( UDP_Socket, sizeof( struct TempRequest ), (char*)&UDPBuffer );
			UDP_CloseSocket( UDP_Socket );
			UDP_Socket = UDP_ListenOnPort( SENSOR_PORT, sizeof( struct TempRequest ), (char*)&UDPBuffer );
		}
	}
	return;
}

/*------------------------------------------------------------------------------------------------------------*/
/**
 * \brief	Configinterface für die Tempsensoren.
 */
/*------------------------------------------------------------------------------------------------------------*/
void TEMP_config_cgi( void * pStruct )
{
	int i;
	char * TEMPBUS, * TEMPADDRESS, * TEMPNAME , TEMPSTRING[10]="\0", TEMPSENSOR[64] = "\0";
	
	struct HTTP_REQUEST * http_request;
	http_request = (struct HTTP_REQUEST *) pStruct;

	cgi_PrintHttpheaderStart();

	printf_P( PSTR(	"<table border=\"0\" cellpadding=\"5\" cellspacing=\"0\">"
		    		"<tr><td></td><td>Bus</td><td>Adresse</td><td>Beschreibung</td><td></td>" ));

	if( PharseCheckName_P( http_request, PSTR("del") ) )
	{
		sprintf_P( TEMPSENSOR, TEMP_EECONFIGNAME_P , atoi( http_request->argvalue[ PharseGetValue_P ( http_request, PSTR("del") ) ] ) );
		deleteConfig( TEMPSENSOR );
	}
	
	for ( i = 0 ; i < TEMP_MAX_SENSORS ; i ++ )
	{
		sprintf_P( TEMPSENSOR, TEMP_EECONFIGNAME_P , i );

		if( PharseCheckName( http_request, TEMPSENSOR ) )
		{
				sprintf_P( &TEMPSENSOR[ strlen( TEMPSENSOR ) + 1 ] , PSTR("%s;%s;%s"), http_request->argvalue[ PharseGetValue_P ( http_request, PSTR("BUS") ) ], http_request->argvalue[ PharseGetValue_P ( http_request, PSTR("ADDRESS") ) ], http_request->argvalue[ PharseGetValue_P ( http_request, PSTR("NAME") ) ] );
				changeConfig( TEMPSENSOR , &TEMPSENSOR[ strlen( TEMPSENSOR ) + 1 ] );
		}
			
		TEMPBUS = TEMPSENSOR;
		TEMPADDRESS = TEMPSENSOR;
		TEMPNAME = TEMPSENSOR;

		if ( TEMP_getSensorConfig( i, TEMPSENSOR ) != NULL )
		{
			while( * TEMPADDRESS != ';' ) TEMPADDRESS++;
			*TEMPADDRESS = '\0';
			TEMPADDRESS++;
			TEMPNAME = TEMPADDRESS;
			while( *TEMPNAME != ';' ) TEMPNAME++;
			*TEMPNAME = '\0';
			TEMPNAME++;
		}		
		else
			TEMPSENSOR[0] = '\0';
		
		TEMP_Sensor2String( i, TEMPSTRING );			

		printf_P( PSTR(	"<form action=\"tempconfig.cgi\">"
		    			"<tr>"
		    			"<td>Sensor %d (%s C)</td>"
						"<td><input name=\"BUS\"type=\"text\"value=\"%s\"size=\"8\" maxlength=\"8\"></td>"
						"<td><input name=\"ADDRESS\"type=\"text\"value=\"%s\"size=\"23\"maxlength=\"23\"></td>"
						"<td><input name=\"NAME\"type=\"text\"value=\"%s\"size=\"28\"maxlength=\"28\"></td>"
						"<td><input type=\"submit\"value=\"&Auml;ndern\"name=\"TEMPSENSOR_%02d\"></td>"
		    			"<td><a href=\"tempconfig.cgi?del=%d\" style=\"text-decoration:none\" title=\"Del\"><input type=\"button\" value=\"L&ouml;schen\" class=\"actionBtn\"></a></td>"
		    			"</tr>"
						"</form>" ),i, TEMPSTRING, TEMPBUS, TEMPADDRESS, TEMPNAME, i ,i );

	}

	printf_P( PSTR(	"</table>" ));
	
	cgi_PrintHttpheaderEnd();
}

/**
 * @}
 */
