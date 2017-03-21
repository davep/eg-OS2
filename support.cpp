/*

     EXPERT GUIDE - NORTON GUIDE READER FOR OS/2
     Copyright (C) 1996 David A Pearson
   
     This program is free software; you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation; either version 2 of the license, or 
     (at your option) any later version.
     
     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.
     
     You should have received a copy of the GNU General Public License
     along with this program; if not, write to the Free Software
     Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

// OS/2 header file.

#define INCL_KBD
#define INCL_WINWINDOWMGR
#define INCL_WINSHELLDATA
#include <os2.h>

// Standard header files.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>

// Local header files.

#include "keys.h"
#include "support.h"
#include "colours.h"

static PSZ  pszCurrentGuide = NULL;
static HAB  hAB;
static HINI hIni;
static CHAR szProfile[ _MAX_PATH ];
static PSZ  pszHomeDir;

INT iClrNormal;
INT iClrBold;
INT iClrUnderline;
INT iClrReverse;
INT iClrHelp;
INT iClrShadow;
INT iClrMenu;
INT iClrMenuPrompt;

PSZ pszApp          = (PSZ) "Expert Guide/2";
PSZ pszGuide        = (PSZ) "CurrentGuide";
PSZ pszClrNormal    = (PSZ) "ColourNormal";
PSZ pszClrBold      = (PSZ) "ColourBold";
PSZ pszClrUnderline = (PSZ) "ColourUnderline";
PSZ pszClrReverse   = (PSZ) "ColourRevrese";
PSZ pszClrHelp      = (PSZ) "ColourHelp";
PSZ pszClrShadow    = (PSZ) "ColourShadow";
PSZ pszClrMenu      = (PSZ) "ColourMenu";
PSZ pszClrMenuPrmt  = (PSZ) "ColourMenuPrompt";
PSZ pszAppList      = (PSZ) "GuideList";
PSZ pszListCount    = (PSZ) "Count";

static INT RealPrfQueryProfileInt( PSZ, PSZ, INT );

/*****************************************************************************
* Function..: ReadByte()                                                     *
* Syntax....: BYTE ReadByte( FILE *f )                                       *
* By........: David A Pearson                                                *
* Date......: 24th March 1995                                                *
* Notes.....: Read a byte from the guide.                                    *
*****************************************************************************/

BYTE ReadByte( FILE *f )
{
    return( decrypt( (BYTE) getc( f ) ) );
}

/*****************************************************************************
* Function..: ReadWord()                                                     *
* Syntax....: BYTE ReadWord( FILE *f )                                       *
* By........: David A Pearson                                                *
* Date......: 24th March 1995                                                *
* Notes.....: Read a word from the guide.                                    *
*****************************************************************************/

USHORT ReadWord( FILE *f )
{
    BYTE b1 = (BYTE) ReadByte( f );
    BYTE b2 = (BYTE) ReadByte( f );
    
    return( (USHORT) ( ( b2 << 8 ) + b1 ) );
}

/*****************************************************************************
* Function..: ReadLong()                                                     *
* Syntax....: BYTE ReadLong( FILE *f )                                       *
* By........: David A Pearson                                                *
* Date......: 24th March 1995                                                *
* Notes.....: Read a long from the guide.                                    *
*****************************************************************************/

LONG ReadLong( FILE *f )
{
    USHORT us1 = ReadWord( f );
    USHORT us2 = ReadWord( f );
    
    return( (LONG) ( us2 << 16 ) + us1 );
}

/*****************************************************************************
* Function..: GetStrZ()                                                      *
* Syntax....: INT GetStrZ( FILE *f, PBYTE pbBuffer, INT iMax )               *
* By........: David A Pearson                                                *
* Date......: 24th March 1995                                                *
* Notes.....: Load a null terminated string from the file.                   *
*****************************************************************************/

INT GetStrZ( FILE *f, PBYTE pbBuffer, INT iMax )
{
    LONG lSavPos = ftell( f );
    
    fread( pbBuffer, iMax, 1, f );
    
    BOOL fEOS = FALSE;
    INT  i;
    
    for ( i = 0; i < iMax && !fEOS; i++, pbBuffer++ )
    {
	*pbBuffer = decrypt( *pbBuffer );
	fEOS      = (BOOL) ( *pbBuffer == 0 );
    }
    
    fseek( f, (LONG) lSavPos + i, SEEK_SET );
    
    return( i );
}

/*****************************************************************************
* Function..: InKey()                                                        *
* Syntax....: INT InKey( VOID )                                              *
* By........: David A Pearson                                                *
* Date......: 30th March 1995                                                *
* Notes.....: Get a key press.                                               *
*****************************************************************************/

INT InKey( VOID )
{
    KBDKEYINFO kbd;
    INT        iKey;
    
    KbdCharIn( &kbd, IO_WAIT, 0 );
    
    iKey = kbd.chChar;
    
    if ( kbd.fbStatus & 2 )
    {
	iKey = kbd.chScan + K_EXTENDED;
    }
    
    return( iKey );
}

/*****************************************************************************
* Function..: CurrentGuide()                                                 *
* Syntax....: PSZ CurrentGuide( VOID )                                       *
* By........: David A Pearson                                                *
* Date......: 23rd April 1995                                                *
* Notes.....: Returns the name of the current guide.                         *
*****************************************************************************/

PSZ CurrentGuide( VOID )
{
    return( pszCurrentGuide );
}

/*****************************************************************************
* Function..: CurrentGuide()                                                 *
* Syntax....: VOID CurrentGuide( PSZ pszNew )                                *
* By........: David A Pearson                                                *
* Date......: 23rd April 1995                                                *
* Notes.....: Sets the new current guide name.                               *
*****************************************************************************/

VOID CurrentGuide( PSZ pszNew )
{
    if ( pszCurrentGuide )
    {
	delete[] pszCurrentGuide;
    }
    
    pszCurrentGuide = (PSZ) new CHAR[ strlen( (char *) pszNew ) + 1 ];
    
    strcpy( (char *) pszCurrentGuide, (char *) pszNew );
}

/*****************************************************************************
* Function..: HomeDir()                                                      *
* Syntax....: PSZ HomeDir( VOID )                                            *
* By........: David A Pearson                                                *
* Date......: 25th April 1995                                                *
* Notes.....: Return the path to the "home" directory.                       *
*****************************************************************************/

PSZ HomeDir( VOID )
{
    return( pszHomeDir );
}

/*****************************************************************************
* Function..: BeginEG( PSZ pszEXE )                                          *
* Syntax....: VOID BeginEG( PSZ pszEXE )                                     *
* By........: David A Pearson                                                *
* Date......: 23rd April 1995                                                *
* Notes.....: Init EG.                                                       *
*****************************************************************************/

VOID BeginEG( PSZ pszEXE )
{
    char szDrive[ _MAX_DRIVE ];
    char szDir[ _MAX_DIR ];
    
    _splitpath( (char *) pszEXE, szDrive, szDir, NULL, NULL );
    _makepath( szProfile, szDrive, szDir, "EG", "INI" );
    
    pszHomeDir = (PSZ) new CHAR[ strlen( (char *) pszEXE ) + 1 ];
    _makepath( (char *) pszHomeDir, szDrive, szDir, NULL, NULL );
    
    hAB  = WinInitialize( 0 );
    hIni = PrfOpenProfile( hAB, (PSZ) szProfile );
    
    iClrNormal     = RealPrfQueryProfileInt( pszApp, pszClrNormal, 
					     HI_WHITE_BLUE );
    iClrBold       = RealPrfQueryProfileInt( pszApp, pszClrBold, 
					     HI_YELLOW_BLUE );
    iClrUnderline  = RealPrfQueryProfileInt( pszApp, pszClrUnderline, 
					     MAGENTA_BLUE );
    iClrReverse    = RealPrfQueryProfileInt( pszApp, pszClrReverse, 
					     HI_WHITE_RED );
    iClrHelp       = RealPrfQueryProfileInt( pszApp, pszClrHelp, BLACK_WHITE );
    iClrShadow     = RealPrfQueryProfileInt( pszApp, pszClrShadow, 
					     HI_BLACK_BLACK );
    iClrMenu       = RealPrfQueryProfileInt( pszApp, pszClrMenu, BLACK_WHITE );
    iClrMenuPrompt = RealPrfQueryProfileInt( pszApp, pszClrMenuPrmt, 
					     HI_WHITE_RED );
    
    ULONG ulLen;
    
    if ( PrfQueryProfileSize( hIni, pszApp, pszGuide, &ulLen ) )
    {
	pszCurrentGuide = (PSZ) new CHAR[ ulLen + 1 ];
	
	PrfQueryProfileString( hIni, pszApp, pszGuide, NULL, pszCurrentGuide,
			       ulLen );
	
	if ( !*pszCurrentGuide )
	{
	    delete[] pszCurrentGuide;
	    pszCurrentGuide = NULL;
	}
    }
}

/*****************************************************************************
* Function..: EndEG()                                                        *
* Syntax....: VOID EndEG( VOID )                                             *
* By........: David A Pearson                                                *
* Date......: 23rd April 1995                                                *
* Notes.....: Close down EG.                                                 *
*****************************************************************************/

VOID EndEG( VOID )
{
    if ( pszCurrentGuide )
    {
	delete[] pszCurrentGuide;
    }
    
    delete[] pszHomeDir;
    
    PrfCloseProfile( hIni );
    WinTerminate( hAB );
}

/*****************************************************************************
* Function..: SaveSettings()                                                 *
* Syntax....: VOID SaveSettings( VOID )                                      *
* By........: David A Pearson                                                *
* Date......: 23rd April 1995                                                *
* Notes.....: Save the settings to the INI file.                             *
*****************************************************************************/

VOID SaveSettings( VOID )
{
    PSZ pszFull = (PSZ) new CHAR[ _MAX_PATH ];
    
    _fullpath( (char *) pszFull, (char *) pszCurrentGuide, _MAX_PATH );
    
    PrfWriteProfileString( hIni, pszApp, pszGuide, pszFull );
    PrfWriteProfileData( hIni, pszApp, pszClrNormal, &iClrNormal, 
			 sizeof( INT ) );
    PrfWriteProfileData( hIni, pszApp, pszClrBold, &iClrBold, sizeof( INT ) );
    PrfWriteProfileData( hIni, pszApp, pszClrUnderline, &iClrUnderline, 
			 sizeof( INT ) );
    PrfWriteProfileData( hIni, pszApp, pszClrReverse, &iClrReverse, 
			 sizeof( INT ) );
    PrfWriteProfileData( hIni, pszApp, pszClrHelp, &iClrHelp, sizeof( INT ) );
    PrfWriteProfileData( hIni, pszApp, pszClrShadow, &iClrShadow, 
			 sizeof( INT ) );
    PrfWriteProfileData( hIni, pszApp, pszClrMenu, &iClrMenu, sizeof( INT ) );
    PrfWriteProfileData( hIni, pszApp, pszClrMenuPrmt, &iClrMenuPrompt, 
			 sizeof( INT ) );
    
    delete[] pszFull;
}

/*****************************************************************************
* Function..: ListCount()                                                    *
* Syntax....: INT ListCount( VOID )                                          *
* By........: David A Pearson                                                *
* Date......: 11th May 1995                                                  *
* Notes.....: Returns the size of the list.                                  *
*****************************************************************************/

INT ListCount( VOID )
{
    return( RealPrfQueryProfileInt( pszAppList, pszListCount, 0 ) );
}

/*****************************************************************************
* Function..: SaveList()                                                     *
* Syntax....: VOID SaveList( VOID *args )                                    *
* By........: David A Pearson                                                *
* Date......: 11th May 1995                                                  *
* Notes.....: Save the guide list to the INI file.                           *
*****************************************************************************/

VOID SaveList( VOID *args )
{
    SaveListArgs *sl = (SaveListArgs *) args;
    CHAR szKey[ 10 ];
    
    PrfWriteProfileData( hIni, pszAppList, pszListCount, &sl->iCount,
			 sizeof( INT ) );
    
    for ( INT i = 0; i < sl->iCount; i++ )
    {
	sprintf( szKey, "Title%X", i );
	PrfWriteProfileString( hIni, pszAppList, (PSZ) szKey, 
			       sl->ppszList[ i ] );
	sprintf( szKey, "Path%X", i );
	PrfWriteProfileString( hIni, pszAppList, (PSZ) szKey, 
			       sl->ppszPaths[ i ] );
    }
    
    _endthread();
}

VOID LoadList( SaveListArgs *sl )
{
    CHAR  szKey[ 10 ];
    PSZ   pszBuffer = (PSZ) new CHAR[ 1024 ];
    ULONG ulLen;
    
    sl->ppszList  = new PSZ[ sl->iCount ];
    sl->ppszPaths = new PSZ[ sl->iCount ];
    
    for ( INT i = 0; i < sl->iCount; i++ )
    {
	sprintf( szKey, "Title%X", i );
	
	ulLen = PrfQueryProfileString( hIni, pszAppList, (PSZ) szKey, 
				       (PSZ) "", pszBuffer, 1024 );
	
	sl->ppszList[ i ] = (PSZ) new CHAR[ ulLen + 1 ];
	strcpy( (char *) sl->ppszList[ i ], (char *) pszBuffer );
	
	sprintf( szKey, "Path%X", i );
	
	ulLen = PrfQueryProfileString( hIni, pszAppList, (PSZ) szKey, 
				       (PSZ) "", pszBuffer, 1024 );
	
	sl->ppszPaths[ i ] = (PSZ) new CHAR[ ulLen + 1 ];
	strcpy( (char *) sl->ppszPaths[ i ], (char *) pszBuffer );
    }
    
    delete[] pszBuffer;
}

/*****************************************************************************
* Function..: RealPrfQueryProfileInt()                                       *
* Syntax....: INT RealPrfQueryProfileInt( PSZ pszAddID, PSZ pszKey,          *
* ..........:                             INT iDefa )                        *
* By........: David A Pearson                                                *
* Date......: 25th April 1995                                                *
* Notes.....: Load a "real" integer from the INI file.                       *
*****************************************************************************/

static INT RealPrfQueryProfileInt( PSZ pszAppID, PSZ pszKey, INT iDefa )
{
    ULONG ulLen;
    INT   i = iDefa;
    
    if ( PrfQueryProfileSize( hIni, pszAppID, pszKey, &ulLen ) )
    {
	if ( ulLen == sizeof( INT ) )
	{
	    PrfQueryProfileData( hIni, pszAppID, pszKey, &i, &ulLen );
	}
    }
    
    return( i );
}
