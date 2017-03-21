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

#define INCL_VIO
#include <os2.h>

// Standard header files.

#include <ctype.h>
#include <string.h>
#include <process.h>

// Local header files.

#include "ng.h"
#include "support.h"
#include "ngentry.h"

/*****************************************************************************
* Function..: NGENTRY()                                                      *
* Class.....: NGENTRY                                                        *
* Syntax....: NGENTRY( INT iScrnTop, iScrnBtm )                              *
* By........: David A Pearson                                                *
* Date......: 14th March 1995                                                *
* Notes.....: Constructor for the NGENTRY class.                             *
*****************************************************************************/

NGENTRY::NGENTRY( INT iScrnTop, INT iScrnBtm )
{
    ppszLines = (PSZ *) 0;
    plOffsets = (LONG *) 0;
    pSeeAlsos = (NGMENU *) 0;
    iLines    = 0;
    iTop      = iScrnTop;
    iBottom   = iScrnBtm;
}

/*****************************************************************************
* Function..: ~NGENTRY()                                                     *
* Class.....: NGENTRY                                                        *
* Syntax....: ~NGENTRY( VOID )                                               *
* By........: David A Pearson                                                *
* Date......: 14th March 1995                                                *
* Notes.....: Destructor for the NGENTRY class.                              *
*****************************************************************************/

NGENTRY::~NGENTRY( VOID )
{
    killLines();
    
    if ( pSeeAlsos )
    {
	delete pSeeAlsos;
    }
}

/*****************************************************************************
* Function..: readShort()                                                    *
* Class.....: NGENTRY                                                        *
* Syntax....: VOID readShort( FILE *f )                                      *
* By........: David A Pearson                                                *
* Date......: 14th March 1995                                                *
* Notes.....: Read a short entry from the file.                              *
*****************************************************************************/

VOID NGENTRY::readShort( FILE *f )
{
    killLines();
    
    lOffset = ( ftell( f ) - 2L );
    
    ReadWord( f ); // Dunno
    
    INT iItems = ReadWord( f );
    
    ReadWord( f ); // Dunno
    
    iParentLine = ReadWord( f );
    lParent     = ReadLong( f );
    
    fseek( f, 12L, SEEK_CUR );
    
    plOffsets = new LONG[ iItems ];
    
    for ( INT i = 0; i < iItems; i++ )
    {
	ReadWord( f ); // Dunno?
	plOffsets[ i ] = ReadLong( f );
    }
    
    readText( f, iItems );
    
    iVisTop   = 0;
    iVisBtm   = ( iItems > ( iBottom - iTop ) ? ( iBottom - iTop ) : 
		  iItems - 1 );
    iCurrent  = 0;
    iPrevious = 0;
    fLong     = FALSE;
    fSeeAlso  = FALSE;
}

/*****************************************************************************
* Function..: readLong()                                                     *
* Class.....: NGENTRY                                                        *
* Syntax....: VOID readLong( FILE *f )                                       *
* By........: David A Pearson                                                *
* Date......: 14th March 1995                                                *
* Notes.....: Read a long entry from the file.                               *
*****************************************************************************/

VOID NGENTRY::readLong( FILE *f )
{
    killLines();
    
    if ( pSeeAlsos )
    {
	delete pSeeAlsos;
	pSeeAlsos = (NGMENU *) 0;
    }
    
    lOffset = ( ftell( f ) - 2L );
    
    ReadWord( f );
    
    INT iItems    = ReadWord( f );
    INT iSeeAlsos = ReadWord( f );
    
    iParentLine = ReadWord( f );
    lParent     = ReadLong( f );
    
    fseek( f, 4L, SEEK_CUR );
    
    lPrevious = ReadLong( f );
    lNext     = ReadLong( f );
    
    if ( lNext == -1L )
    {
	lNext = 0L;
    }
    if ( lPrevious == -1L )
    {
	lPrevious = 0L;
    }
    
    readText( f, iItems );
    
    if ( iSeeAlsos )
    {
	pSeeAlsos   = new NGMENU;
	INT  iCnt   = ReadWord( f );
	LONG *plOff = new LONG[ iCnt ];
	
	for ( INT i = 0; i < iCnt; i++ )
	{
	    if ( i < 20 )
	    {
		plOff[ i ] = ReadLong( f );
	    }
	    else
	    {
		fseek( f, 4L, SEEK_CUR );
	    }
	}
	
	BYTE *pbBuffer = new BYTE[ 512 ];
	
	for ( INT i = 0; i < iCnt; i++ )
	{
	    if ( i < 20 )
	    {
		GetStrZ( f, pbBuffer, 512 );
		pSeeAlsos->addItem( pbBuffer, plOff[ i ] );
	    }
	}
	
	delete[] pbBuffer;
	delete[] plOff;
    }
    
    // Don't forget the see also stuff.....
    
    iVisTop   = 0;
    iVisBtm   = ( iItems > ( iBottom - iTop ) ? ( iBottom - iTop ) : 
		  iItems - 1 );
    iCurrent  = 0;
    iPrevious = 0;
    fLong     = TRUE;
    fSeeAlso  = iSeeAlsos;
}

/*****************************************************************************
* Function..: killLines()                                                    *
* Class.....: NGENTRY                                                        *
* Syntax....: VOID killLines( VOID )                                         *
* By........: David A Pearson                                                *
* Date......: 14th March 1995                                                *
* Notes.....: Clean up the array of lines.                                   *
*****************************************************************************/

VOID NGENTRY::killLines( VOID )
{
    if ( ppszLines )
    {
	for ( INT i = 0; i < iLines; i++ )
	{
	    delete[] ppszLines[ i ];
	}
	
	delete[] ppszLines;
	
	ppszLines = (PBYTE *) 0;
	iLines   = 0;
    }
    
    if ( plOffsets )
    {
	delete[] plOffsets;
	plOffsets = (LONG *) 0;
    }
}

/*****************************************************************************
* Function..: readText()                                                     *
* Class.....: NGENTRY                                                        *
* Syntax....: VOID readText( FILE *f, INT iItems )                           *
* By........: David A Pearson                                                *
* Date......: 14th March 1995                                                *
* Notes.....: Read some text from the file.                                  *
*****************************************************************************/

VOID NGENTRY::readText( FILE *f, INT iItems )
{
    ppszLines = (PSZ *) new PCHAR[ iItems ];
    
    iLines = iItems;
    
    CHAR szBuffer[ 512 ];
    
    for ( INT i = 0; i < iItems; i++ )
    {
	GetStrZ( f, (PBYTE) szBuffer, sizeof( szBuffer ) );
	
	ppszLines[ i ] = (PSZ) new CHAR[ strlen( szBuffer ) + 1 ];
	
	strcpy( (char *) ppszLines[ i ], szBuffer );
    }
}

/*****************************************************************************
* Function..: formatLine()                                                   *
* Class.....: NGENTRY                                                        *
* Syntax....: PSZ formatLine( PSZ pszBuffer )                                *
* By........: David A Pearson                                                *
* Date......: 14th March 1995                                                *
* Notes.....: Format a line of NG text.                                      *
*****************************************************************************/

PSZ NGENTRY::formatLine( PSZ pszBuffer )
{
    // First, work out the length including "RLE" spaces.
    
    PSZ psz  = pszBuffer;
    INT iLen = 0;
    
    while ( *psz )
    {
	if ( *psz == 0xFF )
	{
	    ++psz;
	    iLen += *psz;
	}
	else
	{
	    ++iLen;
	}
	++psz;
    }
    
    if ( iLen < 80 )
    {
	iLen = 80;
    }
    
    iLen = iLen << 1;
    ++iLen;
    
    // Now go format it.
    
    PSZ  pszClean = new BYTE[ iLen ];
    PSZ  pszTgt   = pszClean;
    BYTE bAttr    = (BYTE) iClrNormal;
    INT  iChar    = 0;
    
    psz = pszBuffer;
    
    while ( *psz )
    {
	if ( *psz == '^' )
	{
	    psz++;
	    
	    switch ( *psz )
	    {
	    case 'a' :      // Attribute.
	    case 'A' :
	    {
		++psz;
		bAttr = hex2Byte( psz );
		++psz;
		break;
	    }
	    case 'b' :      // Bold.
	    case 'B' :
	    {
		bAttr = (BYTE) ( bAttr == iClrBold ? iClrNormal : iClrBold );
		break;
	    }
	    case 'c' :      // Character.
	    case 'C' :
	    {
		++psz;
		*pszTgt++ = hex2Byte( psz );
		++psz;
		*pszTgt++ = bAttr;
		++iChar;
		break;
	    }
	    case 'n' :      // Normal.
	    case 'N' :
	    {
		bAttr = (BYTE) iClrNormal;
		break;
	    }
	    case 'r' :
	    case 'R' :      // Reverse.
	    {
		bAttr = (BYTE) ( bAttr == iClrReverse ? iClrNormal : 
				 iClrReverse );
		break;
	    }
	    case 'u' :      // Underlined.
	    case 'U' :
	    {
		bAttr = (BYTE) ( bAttr == iClrUnderline ? iClrNormal : 
				 iClrUnderline );
		break;
	    }
	    case '^' :      // '^' chatacter.
	    {
		*pszTgt++ = '^';
		*pszTgt++ = bAttr;
		++iChar;
		break;
	    }
	    default : // Bogus '^', roll back one.
	    {
		--psz;
	    }
	    }
	}
	else if ( *psz == 0xFF )
	{
	    ++psz;
	    
	    INT iSpaces = *psz;
	    
	    for ( INT i = 0; i < iSpaces; i++ )
	    {
		*pszTgt++ = ' ';
		*pszTgt++ = bAttr;
		++iChar;
	    }
	}
	else
	{
	    *pszTgt++ = *psz;
	    *pszTgt++ = bAttr;
	    ++iChar;
	}
	
	++psz;
    }
    
    while ( iChar++ < 80 )
    {
	*pszTgt++ = ' ';
	*pszTgt++ = bAttr;
    }
    
    *pszTgt = 0;
    
    return( pszClean );
}

/*****************************************************************************
* Function..: hex2Byte()                                                     *
* Class.....: NGENTRY                                                        *
* Syntax....: BYTE hex2Byte( PSZ psz )                                       *
* By........: David A Pearson                                                *
* Date......: 14th March 1995                                                *
* Notes.....: Convert a string hex value into a byte value.                  *
*****************************************************************************/

BYTE NGENTRY::hex2Byte( PSZ psz )
{
    INT  iByte = 0;
    BYTE bByte = 0;
    
    for ( INT i = 0; i < 2; i++, psz++ )
    {
	*psz = (BYTE) toupper( *psz );
	
	if ( *psz > '/' && *psz < ':' )
	{
	    iByte = ( ( (INT) *psz ) - '0' );
	}
	else if ( *psz > '@' && *psz < 'G' )
	{
	    iByte = ( ( (INT) *psz ) - '7' );
	}
	
	bByte += ( iByte * ( !i ? 16 : 1 ) );
    }
    
    return( bByte );
}

/*****************************************************************************
* Function..: redraw()                                                       *
* Class.....: NGENTRY                                                        *
* Syntax....: VOID redraw( VOID )                                            *
* By........: David A Pearson                                                *
* Date......: 31st March 1995                                                *
* Notes.....: Redraw the whole entry.                                        *
*****************************************************************************/

VOID NGENTRY::redraw( VOID )
{
    INT iRow;
    INT i;
    
    for ( i = iVisTop, iRow = 0; i <= iVisBtm; i++, iRow++ )
    {
	showNormal( i );
    }
    
    if ( !fLong )
    {
	showHigh( iCurrent );
    }
    
    if ( ( ( iTop + iRow ) - 1 ) < iBottom )
    {
	BYTE b[ 2 ];
	
	b[ 0 ] = ' ';
	b[ 1 ] = (BYTE) iClrNormal;
	
	VioScrollUp( (USHORT) ( iTop + iRow ), 1, (USHORT) iBottom, 
		     78, (USHORT) -1, b, 0 );
    }
}

/*****************************************************************************
* Function..: showNormal()                                                   *
* Class.....: NGENTRY                                                        *
* Syntax....: VOID showNormal( INT iLine )                                   *
* By........: David A Pearson                                                *
* Date......: 31st March 1995                                                *
* Notes.....: Show the current line in it's "normal" colours.                *
*****************************************************************************/

VOID NGENTRY::showNormal( INT iLine )
{
    PBYTE pszBuffer = formatLine( ppszLines[ iLine ] );
    
    VioWrtCellStr( pszBuffer, 156, (USHORT) ( iTop + ( iLine - iVisTop ) ), 
		   1, 0 );

    delete[] pszBuffer;
}

/*****************************************************************************
* Function..: showHigh()                                                     *
* Class.....: NGENTRY                                                        *
* Syntax....: VOID showHigh( INT iLine )                                     *
* By........: David A Pearson                                                *
* Date......: 31st March 1995                                                *
* Notes.....: Show the current line in it's "highlight" colours.             *
*****************************************************************************/

VOID NGENTRY::showHigh( INT iLine )
{
    PBYTE pszBuffer = formatLine( ppszLines[ iLine ] );
    PBYTE psz       = pszBuffer;
    BOOL fNull      = FALSE;
    CHAR szBuffer[ 156 ];
    
    for ( INT i = 0; i < 156; i += 2 )
    {
	if ( !*psz )
	{
	    fNull = TRUE;
	}
	
	szBuffer[ i ]     = ( fNull ? ' ' : *psz );
	szBuffer[ i + 1 ] = (BYTE) iClrReverse;
	
	if ( !fNull )
	{
	    psz += 2;
	}
    }
    
    VioWrtCellStr( (PSZ) szBuffer, 156, (USHORT) ( iTop + 
						   ( iLine - iVisTop ) ), 
		   1, 0 );
    
    delete[] pszBuffer;
}

/*****************************************************************************
* Function..: up()                                                           *
* Class.....: NGENTRY                                                        *
* Syntax....: VOID up( VOID )                                                *
* By........: David A Pearson                                                *
* Date......: 31st March 1995                                                *
* Notes.....: Move the cursor up.                                            *
*****************************************************************************/

VOID NGENTRY::up( VOID )
{
    BYTE b[ 2 ];
    
    b[ 0 ] = ' ';
    b[ 1 ] = (BYTE) iClrNormal;
    
    if ( fLong )
    {
	if ( iVisTop )
	{
	    --iVisTop;
	    --iVisBtm;
	    VioScrollDn( (USHORT) iTop, 1, (USHORT) iBottom, 78, 1, b, 0 );
	    showNormal( iVisTop );
	}
    }
    else
    {
	if ( iCurrent )
	{
	    iPrevious = iCurrent--;
	    
	    if ( iCurrent < iVisTop )
	    {
		--iVisTop;
		--iVisBtm;
		
		VioScrollDn( (USHORT) iTop, 1, (USHORT) iBottom, 78, 1, 
			     b, 0 );
	    }
	    
	    showNormal( iPrevious );
	    showHigh( iCurrent );
	}
    }
}

/*****************************************************************************
* Function..: down()                                                         *
* Class.....: NGENTRY                                                        *
* Syntax....: VOID down( VOID )                                              *
* By........: David A Pearson                                                *
* Date......: 31st March 1995                                                *
* Notes.....: Move the cursor down.                                          *
*****************************************************************************/

VOID NGENTRY::down( VOID )
{
    BYTE b[ 2 ];
    
    b[ 0 ] = ' ';
    b[ 1 ] = (BYTE) iClrNormal;
    
    if ( fLong )
    {
	if ( iVisBtm < ( iLines - 1 ) )
	{
	    ++iVisTop;
	    ++iVisBtm;
	    VioScrollUp( (USHORT) iTop, 1, (USHORT) iBottom, 78, 1, b, 0 );
	    showNormal( iVisBtm );
	}
    }
    else
    {
	if ( iCurrent < ( iLines - 1 ) )
	{
	    iPrevious = iCurrent++;
	    
	    if ( iCurrent > iVisBtm )
	    {
		++iVisTop;
		++iVisBtm;
		
		VioScrollUp( (USHORT) iTop, 1, (USHORT) iBottom, 78, 1, 
			     b, 0 );
	    }
	    
	    showNormal( iPrevious );
	    showHigh( iCurrent );
	}
    }
}

/*****************************************************************************
* Function..: home()                                                         *
* Class.....: NGENTRY                                                        *
* Syntax....: VOID home( VOID )                                              *
* By........: David A Pearson                                                *
* Date......: 31st March 1995                                                *
* Notes.....: Move the cursor to the top of the entry.                       *
*****************************************************************************/

VOID NGENTRY::home( VOID )
{
    iVisTop   = 0;
    iVisBtm   = ( iLines > ( iBottom - iTop ) ? ( iBottom - iTop ) : 
		  ( iLines - 1 ) );
    iCurrent  = 0;
    iPrevious = 0;
    
    redraw();
}

/*****************************************************************************
* Function..: end()                                                          *
* Class.....: NGENTRY                                                        *
* Syntax....: VOID end( VOID )                                               *
* By........: David A Pearson                                                *
* Date......: 31st March 1995                                                *
* Notes.....: Move the cursor to the bottom of the entry.                    *
*****************************************************************************/

VOID NGENTRY::end( VOID )
{
    iVisTop   = ( iLines > ( iBottom - iTop ) ? 
		  ( iLines - ( iBottom - iTop ) - 1 ) : 0 );
    iVisBtm   = iLines - 1;
    iCurrent  = iLines - 1;
    iPrevious = iLines - 1;
    
    redraw();
}

/*****************************************************************************
* Function..: pageUp()                                                       *
* Class.....: NGENTRY                                                        *
* Syntax....: VOID pageUp( VOID )                                            *
* By........: David A Pearson                                                *
* Date......: 4th April 1995                                                 *
* Notes.....: Move the cursor up one page.                                   *
*****************************************************************************/

VOID NGENTRY::pageUp( VOID )
{
    INT iPage = ( iBottom - iTop );
    
    if ( iLines > iPage )
    {
	if ( fLong )
	{
	    iCurrent = iVisTop - iPage;
	}
	else
	{
	    iCurrent -= iPage;
	}
	
	if ( iCurrent < 0 )
	{
	    iCurrent  = 0;
	    iVisTop   = 0;
	    iVisBtm   = iPage;
	}
	else
	{
	    iVisTop = iCurrent;
	    iVisBtm = iVisTop + iPage;
	}
	
	iPrevious = iCurrent;
	
	redraw();
    }
    else
    {
	home();
    }
}

/*****************************************************************************
* Function..: pageDown()                                                     *
* Class.....: NGENTRY                                                        *
* Syntax....: VOID pageDown( VOID )                                          *
* By........: David A Pearson                                                *
* Date......: 4th April 1995                                                 *
* Notes.....: Move the cursor down one page.                                 *
*****************************************************************************/

VOID NGENTRY::pageDown( VOID )
{
    INT iPage = ( iBottom - iTop );
    
    if ( iLines > iPage )
    {
	if ( fLong )
	{
	    iCurrent = iVisBtm + iPage;
	}
	else
	{
	    iCurrent += iPage;
	}
	
	if ( iCurrent > ( iLines - 1 ) )
	{
	    iCurrent  = iLines - 1;
	    iVisTop   = iCurrent - iPage;
	    iVisBtm   = iCurrent;
	}
	else
	{
	    iVisTop = iCurrent - iPage;
	    iVisBtm = iCurrent;
	}
	
	iPrevious = iCurrent;
	
	redraw();
    }
    else
    {
	end();
    }
}

VOID NGENTRY::backTrack( NG *Ng )
{
    INT iLine = iParentLine;
    
    Ng->readEntry( lParent, this );
    
    jumpToLine( iLine );
}

VOID NGENTRY::jumpToLine( INT iLine )
{
    if ( !fLong )
    {
	INT iPage = ( iBottom - iTop );
	
	iCurrent = iLine;
	
	if ( iLines > iPage )
	{
	    INT ihPage = ( iPage / 2 );
	    
	    iVisTop = ( iCurrent - ihPage );
	    iVisBtm = ( iVisTop + iPage );
	    
	    if ( iVisTop < 0 )
	    {
		iVisTop = 0;
		iVisBtm = iVisTop + iPage;
	    }
	    else if ( iVisBtm > ( iLines - 1 ) )
	    {
		iVisBtm = ( iLines - 1 );
		iVisTop = ( ( iLines - 1 ) - iPage );
	    }
	}
    }
}
