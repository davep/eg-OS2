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

// Says it should be included before all other header files.

#include <sys/emx.h>

// OS/2 header file.

#define INCL_VIO
#include <os2.h>

// Standard header files.

#include <dos.h>
#include <share.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>

// Local header files.

#include "ng.h"
#include "keys.h"
#include "image.h"
#include "screen.h"
#include "nglist.h"
#include "support.h"

NGLIST::NGLIST( BOOL fForceScan )
{
    ppszList  = (PSZ *) 0;
    ppszPaths = (PSZ *) 0;
    iCount    = ListCount();
    
    if ( !iCount || fForceScan )
    {
	PSZ pszWork = (PSZ) new CHAR[ _MAX_PATH ];
	PSZ pszCWD  = (PSZ) new CHAR[ _MAX_PATH ];
	PSZ pszHome = (PSZ) new CHAR[ _MAX_PATH ];
	
	struct _find ft;
	
	getcwd( (char *) pszCWD, _MAX_PATH );
	strcat( (char *) pszCWD, "\\" );
	strcpy( (char *) pszHome, (char *) HomeDir() );
	
	INT iTimes = ( !stricmp( (char *) pszHome, (char *) pszCWD ) ? 1 : 2 );
	
	for ( INT i = 0; i < iTimes; i++ )
	{
	    sprintf( (char *) pszWork, "%s%s", ( i == 0 ? pszHome : pszCWD ), 
		     "*.NG" );
	    
	    INT iDone = __findfirst( (char *) pszWork, _A_NORMAL, &ft );
	    
	    while ( !iDone )
	    {
		++iCount;
		iDone = __findnext( &ft );
	    }
	    
	    // What about __findclose( &ft )?
	}
	
	if ( iCount )
	{
	    ppszList  = new PSZ[ iCount ];
	    ppszPaths = new PSZ[ iCount ];
	    
	    iCount = 0;
	    
	    for ( INT i = 0; i < iTimes; i++ )
	    {
		sprintf( (char *) pszWork, "%s%s", (char *) ( i == 0 ? 
							      pszHome : 
							      pszCWD ), 
			 "*.NG" );
		
		INT iDone = __findfirst( (char *) pszWork, _A_NORMAL, &ft );
		
		while ( !iDone )
		{
		    if ( getDetails( iCount, ( i == 0 ? pszHome : pszCWD ), 
				     (PSZ) ft.name ) )
		    {
			++iCount;
		    }
		    iDone = __findnext( &ft );
		}
		
		// What about __findclose( &ft )?
	    }
	    
	    SaveListArgs sl;
	    
	    sl.iCount    = iCount;
	    sl.ppszList  = ppszList;
	    sl.ppszPaths = ppszPaths;
	    
	    _beginthread( SaveList, NULL, 10240, (void *) &sl );
	}
	
	delete[] pszWork;
	delete[] pszCWD;
	delete[] pszHome;
    }
    else
    {
	SaveListArgs sl;
	
	sl.iCount = iCount;
	
	LoadList( &sl );
	
	ppszList  = sl.ppszList;
	ppszPaths = sl.ppszPaths;
    }
}

NGLIST::~NGLIST( VOID )
{
        if ( ppszList )
        {
	    for ( INT i = 0; i < iCount; i++ )
	    {
		delete[] ppszList[ i ];
		delete[] ppszPaths[ i ];
	    }
	    
	    delete[] ppszList;
	    delete[] ppszPaths;
        }
}

BOOL NGLIST::select( VOID )
{
    BOOL fChosen = FALSE;
    
    if ( iCount )
    {
	VIOMODEINFO mi;
	
	mi.cb = sizeof( VIOMODEINFO );
	VioGetMode( &mi, 0 );
	
	INT iTop    = 3;
	INT iLeft   = 18;
	INT iBottom = 20;
	INT iRight  = 62;
	
	if ( iCount < ( mi.row - 4 ) )
	{
	    iTop    = ( ( mi.row / 2 ) - ( iCount / 2 ) ) - 1;
	    iBottom = iTop + iCount + 1;
	}
	
	IMAGE Image;
	
	Image.save( iTop, iLeft, iBottom + 1, iRight + 1 );
	
	DrawBox( (USHORT) iTop, (USHORT) iLeft, (USHORT) iBottom, 
		 (USHORT) iRight, (BYTE) iClrMenu );
	ClrArea( (USHORT) ( iTop + 1 ), (USHORT) ( iLeft + 1 ), 
		 (USHORT) ( iBottom - 1 ), (USHORT) ( iRight - 1 ), 
		 (BYTE) iClrMenu );
	DropShadow( (USHORT) iTop, (USHORT) iLeft, (USHORT) iBottom, 
		    (USHORT) iRight );
	
	INT   iCurrent  = 0;
	INT   iPrevious = 0;
	INT   iVisTop   = 0;
	INT   iVisBtm   = ( iCount > ( ( iBottom - iTop ) - 2 ) ? 
			    ( ( iBottom - iTop ) - 2 ) : iCount - 1 );
	BOOL  fAsking   = TRUE;
	BYTE b[ 2 ];
	
	b[ 0 ] = ' ';
	b[ 1 ] = (BYTE) iClrMenu;
	
	redraw( iTop, iLeft, iBottom, iRight, iVisTop, iVisBtm );
	
	while ( fAsking )
	{
	    ClrArea( (USHORT) ( iTop + ( iPrevious - iVisTop ) + 1 ), 
		     (USHORT) ( iLeft + 1 ), (USHORT) 
		     ( iTop + ( iPrevious - iVisTop ) + 1 ), (USHORT) 
		     ( iRight - 1 ), (BYTE) iClrMenu );
	    SayString( (USHORT) ( iTop + ( iPrevious - iVisTop ) + 1 ), 
		       (USHORT) ( iLeft + 1 ), ppszList[ iPrevious ], 
		       (BYTE) iClrMenu );
	    ClrArea( (USHORT) ( iTop + ( iCurrent - iVisTop ) + 1 ), 
		     (USHORT) ( iLeft + 1 ), (USHORT) 
		     ( iTop + ( iCurrent - iVisTop ) + 1 ), (USHORT) 
		     ( iRight - 1 ), (BYTE) iClrMenuPrompt );
	    SayString( (USHORT) ( iTop + ( iCurrent  - iVisTop ) + 1 ), 
		       (USHORT) ( iLeft + 1 ), ppszList[ iCurrent ], 
		       (BYTE) iClrMenuPrompt );
	    
	    INT iKey = InKey();
	    
	    iPrevious = iCurrent;
	    
	    switch ( iKey )
	    {
	    case K_UP :
	    case K_LEFT :
	    {
		if ( iCurrent )
		{
		    --iCurrent;
		    
		    if ( iCurrent < iVisTop )
		    {
			--iVisTop;
			--iVisBtm;
			
			VioScrollDn( (USHORT) ( iTop + 1 ), (USHORT) 
				     ( iLeft + 1 ), (USHORT) ( iBottom - 1 ), 
				     (USHORT) ( iRight - 1 ), 1, b, 0 );
		    }
		}
		break;
	    }
	    case K_DOWN :
	    case K_RIGHT :
	    {
		if ( iCurrent < ( iCount - 1 ) )
		{
		    ++iCurrent;
		    
		    if ( iCurrent > iVisBtm )
		    {
			++iVisTop;
			++iVisBtm;
			
			VioScrollUp( (USHORT) ( iTop + 1 ), (USHORT) 
				     ( iLeft + 1 ), (USHORT) ( iBottom - 1 ), 
				     (USHORT) ( iRight - 1 ), 1, b, 0 );
		    }
		}
		break;
	    }
	    case K_HOME :
	    {
		iVisTop   = 0;
		iVisBtm   = ( iCount > ( ( iBottom - iTop ) - 2 ) ? 
			      ( ( iBottom - iTop ) - 2 ) : iCount - 1 );
		iCurrent  = 0;
		iPrevious = 0;
		redraw( iTop, iLeft, iBottom, iRight, iVisTop, iVisBtm );
		break;
	    }
	    case K_END :
	    {
		iVisTop   = ( iCount > ( ( iBottom - iTop ) - 2 ) 
			      ? ( iCount - ( ( iBottom - iTop ) - 2 ) - 1 ) 
			      : 0 );
		iVisBtm   = ( iCount - 1 );
		iCurrent  = ( iCount - 1 );
		iPrevious = ( iCount - 1 );
		redraw( iTop, iLeft, iBottom, iRight, iVisTop, iVisBtm );
		break;
	    }
	    case K_RETURN :
	    case K_ENTER :
	    {
		CurrentGuide( ppszPaths[ iCurrent ] );
		fChosen = TRUE;
		fAsking = FALSE;
		break;
	    }
	    case K_ESC :
	    {
		fAsking = FALSE;
		break;
	    }
	    }
	}
	
	Image.restore();
    }
    
    return( fChosen );
}

BOOL NGLIST::getDetails( INT i, PSZ pszDir, PSZ pszName )
{
    BOOL fOk = FALSE;
    
    PSZ pszWork = (PSZ) new CHAR[ _MAX_PATH ];
    
    sprintf( (char *) pszWork, "%s%s", pszDir, pszName );
    
    FILE *f = _fsopen( (char *) pszWork, "rb", SH_DENYNO );
    
    if ( f != NULL )
    {
	NGHEADER NgHead;
	
	if ( fread( &NgHead, sizeof( NGHEADER ), 1, f ) )
	{
	    if ( NgHead.usMagic == NG_MAGIC )
	    {
		ppszList[ i ]  = (PSZ) new CHAR[ 41 ];
		ppszPaths[ i ] = (PSZ) new CHAR[ strlen( ( char *) pszWork ) 
					       + 1 ];
		
		memset( (char *) ppszList[ i ], 0, 41 );
		
		strncpy( (char *) ppszList[ i ], NgHead.szTitle, 40 );
		strcpy( (char *) ppszPaths[ i ], (char *) pszWork );
		
		fOk = TRUE;
	    }
	}
	
	fclose( f );
    }
    
    delete[] pszWork;
    
    return( fOk );
}

VOID NGLIST::redraw( INT iTop, INT iLeft, INT iBottom, INT iRight, 
		     INT iVisTop, INT iVisBtm )
{
    BYTE b[ 2 ];
    
    b[ 0 ] = ' ';
    b[ 1 ] = (BYTE) iClrMenu;
    
    VioScrollDn( (USHORT) ( iTop + 1 ), (USHORT) ( iLeft + 1 ), 
		 (USHORT) ( iBottom - 1 ), (USHORT) ( iRight - 1 ), 
		 (USHORT) -1, b, 0 );
    
    for ( INT i = iVisTop; i <= iVisBtm; i++ )
    {
	SayString( (USHORT) ( iTop + ( i - iVisTop ) + 1 ), (USHORT) 
		   ( iLeft + 1 ), ppszList[ i ], (BYTE) iClrMenu );
    }
}
