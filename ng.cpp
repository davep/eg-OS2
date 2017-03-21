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

#include <os2.h>

// Standard header files.

#include <share.h>
#include <string.h>

// Local header files.

#include "ng.h"
#include "image.h"
#include "screen.h"
#include "version.h"
#include "support.h"

/*****************************************************************************
* Function..: NG()                                                           *
* Class.....: NG                                                             *
* Syntax....: NG( PSZ pszName )                                              *
* By........: David A Pearson                                                *
* Date......: 14th March 1995                                                *
* Notes.....: Constructor for the NG class.                                  *
*****************************************************************************/

NG::NG( PSZ pszName )
{
    Menus     = (NGMENU **) 0;
    iNextMenu = 1;
    filNg     = _fsopen( (char *) pszName, "rb", SH_DENYNO );
    
    if ( filNg != NULL )
    {
	if ( fread( &NgHeader, sizeof( NGHEADER ), 1, filNg ) )
	{
	    NgHeader.usMenuCnt++;
	    
	    if ( isValid() )
	    {
		Menus = new NGMENU *[ menuCnt() ];
		
		for ( INT i = 0; i < menuCnt(); i++ )
		{
		    Menus[ i ] = new NGMENU;
		}
		
		strcpy( Menus[ 0 ]->szTitle, "Expand" );
		
		loadMenus();
	    }
	    else
	    {
		fclose( filNg );
	    }
	}
	else
	{
	    fclose( filNg );
	}
    }
}

/*****************************************************************************
* Function..: ~NG()                                                          *
* Class.....: NG                                                             *
* Syntax....: ~NG( VOID )                                                    *
* By........: David A Pearson                                                *
* Date......: 14th March 1995                                                *
* Notes.....: Destructor for the NG class.                                   *
*****************************************************************************/

NG::~NG( VOID )
{
    if ( isValid() )
    {
	fclose( filNg );
    }
    
    if ( Menus )
    {
	for ( INT i = 0; i < menuCnt(); i++ )
	{
	    delete Menus[ i ];
	}
	
	delete[] Menus;
    }
}

/*****************************************************************************
* Function..: loadMenus()                                                    *
* Class.....: NG                                                             *
* Syntax....: VOID loadMenus( VOID )                                         *
* By........: David A Pearson                                                *
* Date......: 15th March 1995                                                *
* Notes.....: Load the menu's from the file.                                 *
*****************************************************************************/

VOID NG::loadMenus( VOID )
{
    INT iID;
    
    do
    {
	iID = (INT) ReadWord( filNg );
	
	switch( iID )
	{
	case 0 :
	case 1 :
	{
	    skipShortLong();
	    break;
	}
	case 2 :
	{
	    loadMenu();
	    break;
	}
	default :
	{
	    iID = 5;
	}
	}
    } while ( iID != 5 && iNextMenu != NgHeader.usMenuCnt );
}

/*****************************************************************************
* Function..: skipShortLong()                                                *
* Class.....: NG                                                             *
* Syntax....: VOID skipShortLong( VOID )                                     *
* By........: David A Pearson                                                *
* Date......: 15th March 1995                                                *
* Notes.....: Skip an entry in the file.                                     *
*****************************************************************************/

VOID NG::skipShortLong( VOID )
{
    INT iLen = ReadWord( filNg );
    
    fseek( filNg, (LONG) 22L + iLen, SEEK_CUR );
}

/*****************************************************************************
* Function..: loadMenu()                                                     *
* Class.....: NG                                                             *
* Syntax....: VOID loadMenu( VOID )                                          *
* By........: David A Pearson                                                *
* Date......: 15th March 1995                                                *
* Notes.....: Load a single menu.                                            *
*****************************************************************************/

VOID NG::loadMenu( VOID )
{
    ReadWord( filNg );
    
    INT iItems = ReadWord( filNg );
    
    fseek( filNg, 20L, SEEK_CUR );
    
    LONG *lpOffsets = new LONG[ iItems ];
    
    for ( INT i = 1; i < iItems; i++ )
    {
	lpOffsets[ i - 1 ] = (LONG) ReadLong( filNg );
    }
    
    LONG l = ftell( filNg );
    
    l += ( iItems * 8 );
    
    fseek( filNg, l, SEEK_SET );
    
    GetStrZ( filNg, (PSZ) Menus[ iNextMenu ]->szTitle, 40 );
    
    BYTE szBuffer[ 51 ];
    
    for ( INT i = 1; i < iItems; i++ )
    {
	GetStrZ( filNg, szBuffer, sizeof( szBuffer ) );
	Menus[ iNextMenu ]->addItem( szBuffer, lpOffsets[ i - 1 ] );
    }
    
    ++iNextMenu;
    
    ReadByte( filNg );
    
    delete[] lpOffsets;
}

/*****************************************************************************
* Function..: readMenuEntry()                                                *
* Class.....: NG                                                             *
* Syntax....: VOID readMenuEntry( INT iMenu, INT iItem, NGENTRY *NgEntry )   *
* By........: David A Pearson                                                *
* Date......: 15th March 1995                                                *
* Notes.....: Read an entry from the file when you have the menu and it's    *
* ..........: prompt as a pointer.                                           *
*****************************************************************************/

VOID NG::readMenuEntry( INT iMenu, INT iItem, NGENTRY *NgEntry )
{
    readEntry( Menus[ iMenu ]->offset( iItem ), NgEntry );
}

/*****************************************************************************
* Function..: readEntry()                                                    *
* Class.....: NG                                                             *
* Syntax....: VOID readEntry( LONG lPtr, NGENTRY *NgEntry )                  *
* By........: David A Pearson                                                *
* Date......: 15th March 1995                                                *
* Notes.....: Read an entry from the file.                                   *
*****************************************************************************/

VOID NG::readEntry( LONG lPtr, NGENTRY *NgEntry )
{
    fseek( filNg, lPtr, SEEK_SET );
    
    INT iID = ReadWord( filNg );
    
    if ( iID == 0 )
    {
	NgEntry->readShort( filNg );
    }
    else if ( iID == 1 )
    {
	NgEntry->readLong( filNg );
    }
    else
    {
	printf( "DEBUG: Unknown ID (#%d)\n", iID );
	InKey();
    }
}

/*****************************************************************************
* Function..: isEntry()                                                      *
* Class.....: NG                                                             *
* Syntax....: BOOL isEntry( LONG lPtr )                                      *
* By........: David A Pearson                                                *
* Date......: 31st March 1995                                                *
* Notes.....: Check that a file offset points to a valid NG entry.           *
*****************************************************************************/

BOOL NG::isEntry( LONG lPtr )
{
    LONG lSav = ftell( filNg );
    
    fseek( filNg, lPtr, SEEK_SET );
    
    INT iID = ReadWord( filNg );
    
    fseek( filNg, lSav, SEEK_SET );
    
    return( iID == 0 || iID == 1 );
}

/*****************************************************************************
* Function..: showHelp()                                                     *
* Class.....: NG                                                             *
* Syntax....: BOOL showHelp( VOID )                                          *
* By........: David A Pearson                                                *
* Date......: 31st March 1995                                                *
* Notes.....: Display the NG help and some of out own details.               *
*****************************************************************************/

VOID NG::showHelp( VOID )
{
    IMAGE Image;
    INT   iTop   = 10;
    INT   iLeft  = 6;
    INT   iBtm   = 22;
    INT   iRight = 73;
    
    Image.save( iTop, iLeft, iBtm + 1, iRight + 1 );
    
    DrawBox( (USHORT) iTop, (USHORT) iLeft, (USHORT) iBtm, (USHORT) iRight, 
	     (BYTE) iClrHelp );
    ClrArea( (USHORT) ( iTop + 1 ), (USHORT) ( iLeft + 1 ), (USHORT) 
	     ( iBtm - 1 ), (USHORT) ( iRight - 1 ), (BYTE) iClrHelp );
    DropShadow( (USHORT) iTop, (USHORT) iLeft, (USHORT) iBtm, 
		(USHORT) iRight );

    SayString( (USHORT) ( iTop + 6 ), (USHORT) iLeft, (PSZ) "\xCC", 
	       (BYTE) iClrHelp );
    SayString( (USHORT) ( iTop + 6 ), (USHORT) iRight, (PSZ) "\xB9", 
	       (BYTE) iClrHelp );

    for ( INT i = ( iLeft + 1 ); i < iRight; i++ )
    {
	SayString( (USHORT) ( iTop + 6 ), (USHORT) i, (PSZ) "\xCD", 
		   (BYTE) iClrHelp );
    }
    
    SayString( (USHORT) ( iTop + 1 ), (USHORT) ( iLeft + 1 ), 
	       (PSZ) "Expert Guide - OS/2 Norton Guide Reader "
	       "By Dave Pearson", (BYTE) iClrHelp );
    SayString( (USHORT) ( iTop + 2 ), (USHORT) ( iLeft + 1 ), 
	       (PSZ) "Alpha Test Version - " EG_VERSION, (BYTE) iClrHelp );
    SayString( (USHORT) ( iTop + 4 ), (USHORT) ( iLeft + 1 ), 
	       (PSZ) "Being For The Benefit Of Mr. Kite!", (BYTE) iClrHelp );

    for ( INT i = 0; i < 5; i++ )
    {
	if ( *( NgHeader.pszCredits[ i ] ) )
	{
	    SayString( (USHORT) ( ( iTop + 7 ) + i ), (USHORT) ( iLeft + 1 ), 
		       (PSZ) NgHeader.pszCredits[ i ], (BYTE) iClrHelp );
	}
    }
    
    InKey();
    
    Image.restore();
}
