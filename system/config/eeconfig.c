//***************************************************************************
//*            eeconfig.c
//*
//*  Son Aug 10 16:25:49 2008
//*  Copyright  2008  Dirk Broßwick
//*  Email: sharandac@snafu.de
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
#include <avr/pgmspace.h>

#if defined(__AVR_XMEGA__)
	#include "hardware/xeemem/xeemem.h"
#else
	#include <avr/eeprom.h>
#endif
#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include "eeconfig.h"

/**
 * \ingroup system
 * \addtogroup EE_Config Speichert Einstellungen im EEprom (config.c)
 *
 * Diese Modul sorgt dafür das Werte Dauerhaft im EEprom
 * gespeichert werden können in Textform.
 * @{
 */

/**
 * \file
 * Speichert Einstellungen im EEprom
 *
 * \author Dirk Broßwick
 */

/**
 * @}
 */
static volatile int ConfigPos = -1;
static volatile int ConfigOffset = -1;
static volatile int ConfigProtect = UNPROTECT;

static struct Config MCPconfig;

/*------------------------------------------------------------------------------------------------------------*/
/**
 * \ingroup EE_Config
 * \brief Init der Config, sucht eine Config im EEprom und legt eine an wenn keine gefunden wurde.
 * \return	void
 */
/*------------------------------------------------------------------------------------------------------------*/
void Config_Init( void )
{	
#if defined(__AVR_XMEGA__)
	EEpromInit();
#endif
	
//	makeConfig( );
	
	if ( findConfig() == -1 )
	{
		makeConfig( );
		findConfig();
	}
}

/*------------------------------------------------------------------------------------------------------------*/
/**
 * \ingroup EE_Config
 * \brief Sucht im EEprom nach einer Configsignatur.
 * \return	int		Fehler oder Okay.
 * \retval  0		Okay
 * \retval  -1		Fehler
 */
/*------------------------------------------------------------------------------------------------------------*/
int findConfig( void )
{
	int i;
	char configstring[] = ( configID );

	char string[32];
	
	for ( i = 0 ; i < ( E2END - sizeof ( MCPconfig ) ); i++ )
	{
		eeprom_read_block( MCPconfig.TAG , (const char * ) i, sizeof ( configstring ) );
		if ( !memcmp( MCPconfig.TAG , configID , sizeof ( configstring ) ) )
		{
			eeprom_read_block( &MCPconfig, (const char * ) i, sizeof ( MCPconfig ) ); 
			ConfigPos = i;
			ConfigOffset = ConfigPos + sizeof( MCPconfig ) + 1;
			break;
		}
	}
	
	if( ConfigPos == -1 ) 
	{
		return(-1);
	}
	else 
	{
		if( readConfig_P ( PSTR("WRITE_PROTECT"), string ) != -1 )
		{
			if ( !strcmp_P( string, PSTR("OFF") ) )
			    ConfigProtect = UNPROTECT;
			else
			    ConfigProtect = PROTECT;
		}
		return(0);
	}
	return(0);
}

/*------------------------------------------------------------------------------------------------------------*/
/**
 * \ingroup EE_Config
 * \brief Gibt den Inhalt der Config auf der aktuellen STDOUT aus.
 * \return	void
 */
/*------------------------------------------------------------------------------------------------------------*/
void PrintConfig( void )
{
	int i,Configsize ;
	
	printf_P( PSTR("Config ist %d bytes gross, %d byte werden benutzt. Offset %d\r\n\r\n"), MCPconfig.Configlen, getConfigsizeUsed(),ConfigOffset );

	Configsize = getConfigsizeUsed();
		
	for ( i = 0 ; i < Configsize ; i++ )
	{
		// Wenn '&' vorhanden, die zeile überspringen, soll sowas wie ein geheimer Eintrag sein
		if ( eeprom_read_byte( (const unsigned char * ) ConfigOffset + i ) == '&' )
		{
			for( ; i < Configsize ; i++ )
			{
				if ( eeprom_read_byte( (const unsigned char * ) ConfigOffset + i ) == '\r' )
					break;
			}
		}
		else
		{
			printf_P(PSTR("%c"), eeprom_read_byte( (const unsigned char * ) ConfigOffset + i ) );
			if ( eeprom_read_byte( (const unsigned char * ) ConfigOffset + i ) == '\r' ) printf_P( PSTR("\n"));
		}
	}
	
	printf_P( PSTR("\r\n"));	
}

/*------------------------------------------------------------------------------------------------------------*/
/**
 * \ingroup EE_Config
 * \brief Legt die Configsignatur im EEprom an.
 * \return		void
 */
/*------------------------------------------------------------------------------------------------------------*/
void makeConfig( void )
{
	int i;
	int len = E2END - sizeof ( MCPconfig );
	int Pos = 0;
	unsigned char * MCPconfig_P = (unsigned char *) &MCPconfig;
	
	memcpy_P( &MCPconfig.TAG , PSTR( configID ), sizeof ( configID ) );
	MCPconfig.Configlen = len;
	
	for( i = 0; i < sizeof( MCPconfig ) ; i++ )
		eeprom_write_byte(  ( unsigned char * ) Pos + i, *MCPconfig_P++ );
	
	ConfigOffset = Pos + sizeof( MCPconfig ) + 1;
	
	eeprom_write_byte( ( unsigned char * ) ConfigOffset, '\0' );
	
	return;
}

/*------------------------------------------------------------------------------------------------------------*/
/**
 * \ingroup EE_Config
 * \brief Gibt den belegten Speicher in Bytes aus.
 * \return	int		Anzahl der verbrauchten Bytes.
 * \retval	>=0		Größe der Config in Bytes.
 * \retval  -1		Nix gefunden. Eintrag nicht vorhanden.
 */
/*------------------------------------------------------------------------------------------------------------*/
int getConfigsizeUsed( void )
{
	int i;
	
	if ( ConfigPos == -1 ) return( -1 );
	
	for( i = 0 ; i < MCPconfig.Configlen ; i++ )
		if ( eeprom_read_byte( (const unsigned char * ) ConfigOffset + i ) == '\0' )
			break;
	
	return( i );
}

/*------------------------------------------------------------------------------------------------------------*/
/**
 * \ingroup EE_Config
 * \brief Sucht nach eine Paramter und gibt den Inhalt zurück.
 * \param 	ConfigName		Pointer auf den Parameter der als String hinterlegt ist.
 * \param 	ConfigValue		Pointer auf den Speicher in welchen der Inhalt zurüch gegeben wird.
 * \return  int
 * \retval	1		Alles okay, es wurde was gefunden.
 * \retval  -1		Nix gefunden. Eintrag nicht vorhanden.
 */
/*------------------------------------------------------------------------------------------------------------*/
int readConfig( char * ConfigName, char * ConfigValue )
{
	int i = 0, Pos=0, Configsize;
	char E2ConfigName[30];
	
	Configsize = getConfigsizeUsed();
	
	i = checkConfigName ( ConfigName );
	
	if ( i == -1 ) return( -1 );
	
	for( ; i < Configsize ; Pos++ , i++ )
	{
		E2ConfigName[ Pos ] = eeprom_read_byte( (const unsigned char * ) ConfigOffset + i );
		if ( E2ConfigName[ Pos ] == '=' || E2ConfigName[ Pos ] == '\0' ) 
		{
			E2ConfigName[ Pos ] = '\0';
			break;
		}
	}
		
	if ( eeprom_read_byte( (const unsigned char * ) ConfigOffset + i ) == '=' )
	{
		if ( !strcmp( E2ConfigName , ConfigName ) ) 
		{
			i++;
			Pos = 0;
			for ( ; i < Configsize ; Pos++ , i++ )
			{
				ConfigValue[ Pos ] = eeprom_read_byte( (const unsigned char * ) ConfigOffset + i );
				if ( ConfigValue[ Pos ] == '\r' || ConfigValue[ Pos ] == '\0' ) break;
			}
			ConfigValue[ Pos ] = '\0';
			return( 1 );
		}
	}
	return( -1 );
}

/*------------------------------------------------------------------------------------------------------------*/
/**
 * \ingroup EE_Config
 * \brief Speichert einen Paramter mit Inhalt ab.
 * \param 	ConfigName		Pointer auf den Parameter der als String hinterlegt ist.
 * \param 	ConfigValue		Pointer auf den Inhalt der als String hinterlegt ist.
 * \return
 * \retval	1		Alles okay.
 * \retval  -1		Ging irgentwie net, what ever.
 */
/*------------------------------------------------------------------------------------------------------------*/
int writeConfig( char * ConfigName, char * ConfigValue )
{
	int Pos,i;
		
	if ( checkConfigName ( ConfigName ) != -1 ) return( -1 );

	if ( ConfigProtect == PROTECT ) return( 1 );
	
	Pos = getConfigsizeUsed();

	if ( Pos != 0 ) eeprom_write_byte( ( unsigned char * ) ConfigOffset + Pos++ , '\r' );

	for( i = 0 ; i < strlen( ConfigName ) ; i++ )
	{
		eeprom_write_byte( ( unsigned char * ) ConfigOffset + Pos++, ConfigName[ i ] ) ;
	}
	eeprom_write_byte( ( unsigned char * ) ConfigOffset + Pos++, '=' );

	for( i = 0 ; i < strlen( ConfigValue ) ; i++ )
		eeprom_write_byte( ( unsigned char * ) ConfigOffset + Pos++, ConfigValue[ i ] );
	
	eeprom_write_byte( ( unsigned char * ) ConfigOffset + Pos, '\0' );
	
	return( 1 );
}

/*------------------------------------------------------------------------------------------------------------*/
/**
 * \ingroup EE_Config
 * \brief Sucht nach einen Parameter.
 * \param 	ConfigName	Pointer auf den Parameter der als String hinterlegt ist.
 * \return
 * \retval	>=0		Position im EEprom, alles okay, wurde was gefunden.
 * \retval  -1		Gin irgentwie net, what ever.
 */
/*------------------------------------------------------------------------------------------------------------*/
int checkConfigName( char * ConfigName )
{
	int i = 0, Pos, E2NamePos, Configsize;
	char E2ConfigName[30];
	
	Configsize = getConfigsizeUsed();

	for( ; i < Configsize ; i++ )
	{
		E2NamePos = i;
		Pos = 0 ;
		for( ; i < Configsize ; Pos++ , i++ )
		{
			E2ConfigName[ Pos ] = eeprom_read_byte( (const unsigned char * ) ConfigOffset + i );
			if ( E2ConfigName[ Pos ] == '=' || E2ConfigName[ Pos ] == '\0' ) 
			{
				E2ConfigName[ Pos ] = '\0';
				if ( !strcmp( E2ConfigName , ConfigName ) ) return( E2NamePos );
				else break;
			}
		}
		
		for ( ; i < Configsize; Pos++ , i++ )
		{
			if ( eeprom_read_byte( (const unsigned char * ) ConfigOffset + i ) == '\r' || eeprom_read_byte( (const unsigned char * ) ConfigOffset + i ) == '\0' ) break;
		}
		
		if ( eeprom_read_byte( (const unsigned char * ) ConfigOffset + i ) == '\0' ) return( -1 );
	}
	return( -1 );
}

/*------------------------------------------------------------------------------------------------------------*/
/**
 * \ingroup EE_Config
 * \brief Ändert einen Paramter mit Inhalt.
 * \param 	ConfigName	Pointer auf den Parameter der als String hinterlegt ist.
 * \param 	ConfigValue	Pointer auf den Inhalt der als String hinterlegt ist.
 * \return
 * \retval	1		Alles okay.
 * \retval  -1		Gin irgentwie net, what ever.
 */
/*------------------------------------------------------------------------------------------------------------*/
int changeConfig( char * ConfigName, char * ConfigValue )
{
	int i;
	
	i = checkConfigName ( ConfigName );
	
	if ( i != -1 ) deleteConfig ( ConfigName );
	
	writeConfig ( ConfigName, ConfigValue );
	
	return( 1 );
}

/*------------------------------------------------------------------------------------------------------------*/
/**
 * \ingroup EE_Config
 * \brief Löscht einen Paramter mit Inhalt.
 * \param 	ConfigName	Pointer auf den Parameter der als String hinterlegt ist.
 * \return
 * \retval	1		Alles okay.
 * \retval  -1		Gin irgentwie net, what ever.
 */
/*------------------------------------------------------------------------------------------------------------*/
int deleteConfig( char * ConfigName)
{
	int i, EntryLen=0, Configsize;
	char Buffer='\r';

	if ( ConfigProtect == PROTECT ) return( 1 );

	Configsize = getConfigsizeUsed ();
	
	i = checkConfigName ( ConfigName );
	
	if ( i == -1 ) return( -1 );
	
	for ( ; i < Configsize ; i++ )
	{
		Buffer = eeprom_read_byte( (const unsigned char * ) ConfigOffset + i );
		if ( Buffer == '\r' || Buffer == '\0' )
			break;
		EntryLen++;
	}
	
	i = checkConfigName ( ConfigName );

	if ( Buffer == '\r' )
	{
		EntryLen++;
		for( ; i < Configsize ; i++ )
		{
			eeprom_write_byte( ( unsigned char *) ConfigOffset + i , eeprom_read_byte( ( const unsigned char * ) ConfigOffset + i + EntryLen ) );
		}
		eeprom_write_byte( ( unsigned char *) ConfigOffset + i, '\0' );
	}
	else
	{
		eeprom_write_byte( ( unsigned char *) ConfigOffset + i - 1, '\0' );
	}
		
	return(1);
}

/*------------------------------------------------------------------------------------------------------------*/
/**
 * \ingroup EE_Config
 * \brief Sucht nach einen Paramter und gibt den Inhalt zurück.
 * \param 	ConfigName	Pointer auf den Parameter der als String hinterlegt ist im Flash.
 * \param 	ConfigValue	Pointer auf den Speicher in welchen der Inhalt zurüch gegeben wird.
 * \return
 * \retval	1		Alles okay, es wurde was gefunden.
 * \retval  -1		Nix gefunden. Eintrag nicht vorhanden.
 */
/*------------------------------------------------------------------------------------------------------------*/
int readConfig_P( const char * ConfigName, char * ConfigValue )
{
	char * ConfigNameBuffer;
	ConfigNameBuffer = (char*) __builtin_alloca (( size_t ) strlen_P( ConfigName ) + 1);
	strcpy_P( ConfigNameBuffer , ConfigName );
	return( readConfig ( ConfigNameBuffer, ConfigValue ) );
}

/*------------------------------------------------------------------------------------------------------------*/
/**
 * \ingroup EE_Config
 * \brief Speichert einen Paramter mit Inhalt ab.
 * \param 	ConfigName	Pointer auf den Parameter der als String hinterlegt ist im Flash.
 * \param 	ConfigValue	Pointer auf den Inhalt der als String hinterlegt ist.
 * \return
 * \retval	1		Alles okay.
 * \retval  -1		Gin irgentwie net, what ever.
 */
/*------------------------------------------------------------------------------------------------------------*/
int writeConfig_P( const char * ConfigName, char * ConfigValue )
{
	char * ConfigNameBuffer;
	ConfigNameBuffer = (char*) __builtin_alloca (( size_t ) strlen_P( ConfigName ) + 1);
	strcpy_P( ConfigNameBuffer , ConfigName );
	return( writeConfig ( ConfigNameBuffer, ConfigValue ) );
}

/*------------------------------------------------------------------------------------------------------------*/
/**
 * \ingroup EE_Config
 * \brief Ändert einen Paramter mit Inhalt.
 * \param 	ConfigName	Pointer auf den Parameter der als String hinterlegt ist im Flash.
 * \param 	ConfigValue	Pointer auf den Inhalt der als String hinterlegt ist.
 * \return
 * \retval	1		Alles okay.
 * \retval  -1		Gin irgentwie net, what ever.
 */
/*------------------------------------------------------------------------------------------------------------*/
int changeConfig_P( const char * ConfigName, char * ConfigValue )
{
	char * ConfigNameBuffer;
	ConfigNameBuffer = (char*) __builtin_alloca (( size_t ) strlen_P( ConfigName ) + 1);
	strcpy_P( ConfigNameBuffer , ConfigName );
	return( changeConfig ( ConfigNameBuffer, ConfigValue ) );
}

/*------------------------------------------------------------------------------------------------------------*/
/**
 * \ingroup EE_Config
 * \brief Löscht einen Paramter mit Inhalt.
 * \param 	ConfigName	Pointer auf den Parameter der als String hinterlegt ist im Flash.
 * \return
 * \retval	1		Alles okay.
 * \retval  -1		Gin irgentwie net, what ever.
 */
/*------------------------------------------------------------------------------------------------------------*/
int deleteConfig_P( const char * ConfigName)
{
	char * ConfigNameBuffer;
	ConfigNameBuffer = (char*) __builtin_alloca (( size_t ) strlen_P( ConfigName ) + 1);
	strcpy_P( ConfigNameBuffer , ConfigName );
	return( deleteConfig ( ConfigNameBuffer ) );
}

/*------------------------------------------------------------------------------------------------------------*/
/**
 * \ingroup EE_Config
 * \brief Sucht nach einen Parameter.
 * \param 	ConfigName	Pointer auf den Parameter der als String hinterlegt ist im Flash.
 * \return
 * \retval	>= 0	Position im EEprom, alles okay, wurde was gefunden.
 * \retval  -1		Gin irgentwie net, what ever.
 */
/*------------------------------------------------------------------------------------------------------------*/
int checkConfigName_P( const char * ConfigName )
{
	char * ConfigNameBuffer;
	ConfigNameBuffer = (char*) __builtin_alloca (( size_t ) strlen_P( ConfigName ) + 1);
	strcpy_P( ConfigNameBuffer , ConfigName );
	return( checkConfigName ( ConfigNameBuffer ) );
}

void setprotectConfig( int Protect )
{
	ConfigProtect = Protect;
}
