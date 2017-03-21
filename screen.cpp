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

// Standard C headers.

#include <stdlib.h>
#include <string.h>

// Local header files.

#include "screen.h"
#include "colours.h"
#include "support.h"

/*****************************************************************************
* Function..: DrawScreen()                                                   *
* Syntax....: VOID DrawScreen( NG *Ng )                                      *
* By........: David A Pearson                                                *
* Date......: 14th March 1995                                                *
* Notes.....: Draw the main screen.                                          *
*****************************************************************************/

VOID DrawScreen( NG *Ng )
{
    VIOMODEINFO miModeInfo;
    BYTE        bAttr = (BYTE) iClrNormal;
    
    miModeInfo.cb = sizeof( VIOMODEINFO );
    
    Cls( bAttr );
    VioGetMode( &miModeInfo, 0 );
    DrawBox( 0, 0, (USHORT) ( miModeInfo.row - 1 ), 79, bAttr );
    
    CHAR szTitle[ 41 ];
    
    memset( szTitle, 0, 41 );
    
    strncpy( szTitle, Ng->NgHeader.szTitle, 40 );
    
    SayCent( 0, (PSZ) szTitle, bAttr );
    
    for ( USHORT us = 1; us < 80; us++ )
    {
	VioWrtCharStrAtt( (PSZ) "\xC4", 1, 2, us, &bAttr, 0 );
    }
    VioWrtCharStrAtt( (PSZ) "\xC7", 1, 2, 0, &bAttr, 0 );
    VioWrtCharStrAtt( (PSZ) "\xB6", 1, 2, 79, &bAttr, 0 );
}

/*****************************************************************************
* Function..: Cls()                                                          *
* Syntax....: VOID Cls( BYTE bAttr )                                         *
* By........: David A Pearson                                                *
* Date......: 14th March 1995                                                *
* Notes.....: Clear the screen.                                              *
*****************************************************************************/

VOID Cls( BYTE bAttr )
{
    BYTE bCell[ 2 ];
    
    bCell[ 0 ] = ' ';
    bCell[ 1 ] = bAttr;
    
    VioScrollUp( 0, 0, (USHORT) -1, 79, (USHORT) -1, bCell, 0 );
}

/*****************************************************************************
* Function..: DrawBox()                                                      *
* Syntax....: VOID DrawBox( USHORT usTop, USHORT usLeft, USHORT usBottom,    *
* ..........:               USHORT usRight, BYTE bAttr )                     *
* By........: David A Pearson                                                *
* Date......: 14th March 1995                                                *
* Notes.....: Draw a double lined box.                                       *
*****************************************************************************/

VOID DrawBox( USHORT usTop, USHORT usLeft, USHORT usBtm, USHORT usRight, 
	      BYTE bAttr )
{
    for ( USHORT us = usLeft; us < usRight; us++ )
    {
	VioWrtCharStrAtt( (PSZ) "\xCD", 1, usTop, us, &bAttr, 0 );
	VioWrtCharStrAtt( (PSZ) "\xCD", 1, usBtm, us, &bAttr, 0 );
    }
    
    for ( USHORT us = usTop; us < usBtm; us++ )
    {
	VioWrtCharStrAtt( (PSZ) "\xBA", 1, us, usLeft, &bAttr, 0 );
	VioWrtCharStrAtt( (PSZ) "\xBA", 1, us, usRight, &bAttr, 0 );
    }
    
    VioWrtCharStrAtt( (PSZ) "\xC9", 1, usTop, usLeft, &bAttr, 0 );
    VioWrtCharStrAtt( (PSZ) "\xBB", 1, usTop, usRight, &bAttr, 0 );
    VioWrtCharStrAtt( (PSZ) "\xC8", 1, usBtm, usLeft, &bAttr, 0 );
    VioWrtCharStrAtt( (PSZ) "\xBC", 1, usBtm, usRight, &bAttr, 0 );
}

/*****************************************************************************
* Function..: ClrArea()                                                      *
* Syntax....: VOID ClrArea( USHORT usTop, USHORT usLeft, USHORT usBottom,    *
* ..........:               USHORT usRight, BYTE bAttr )                     *
* By........: David A Pearson                                                *
* Date......: 7th April 1995                                                 *
* Notes.....: Clear an area of screen.                                       *
*****************************************************************************/

VOID ClrArea( USHORT usTop, USHORT usLeft, USHORT usBottom, USHORT usRight, 
	      BYTE bAttr )
{
    BYTE b[ 2 ];
    
    b[ 0 ] = ' ';
    b[ 1 ] = bAttr;
    
    VioScrollUp( usTop, usLeft, usBottom, usRight, (USHORT) -1, b, 0 );
}

/*****************************************************************************
* Function..: SayCent()                                                      *
* Syntax....: VOID SayCent( USHORT usRow, PBYTE pbText, BYTE bAttr )         *
* By........: David A Pearson                                                *
* Date......: 14th March 1995                                                *
* Notes.....: Display text centered on the display area.                     *
*****************************************************************************/

VOID SayCent( USHORT usRow, PBYTE pbText, BYTE bAttr )
{
    USHORT usCol = (USHORT) ( 40 - ( (USHORT) ( strlen( (char *) pbText ) 
						>> 1 ) ) );

    VioWrtCharStrAtt( pbText, (USHORT) strlen( (char *) pbText ), usRow, 
		      usCol, &bAttr, 0 );
}

/*****************************************************************************
* Function..: SayString()                                                    *
* Syntax....: VOID SayString( USHORT usRow, USHORT usCol, PBYTE pbText,      *
* ..........:                 BYTE bAttr )                                   *
* By........: David A Pearson                                                *
* Date......: 15th March 1995                                                *
* Notes.....: Display text centered on the screen.                           *
*****************************************************************************/

VOID SayString( USHORT usRow, USHORT usCol, PBYTE pbText, BYTE bAttr )
{
    VioWrtCharStrAtt( pbText, (USHORT) strlen( (char *) pbText ), usRow, 
		      usCol, &bAttr, 0 );
}

/*****************************************************************************
* Function..: DropShadow()                                                   *
* Syntax....: VOID DropShadow( USHORT usTop, USHORT usLeft, USHORT, usBottom,*
* ..........:                  USHORT usRight )                              *
* By........: David A Pearson                                                *
* Date......: 7th April 1995                                                 *
* Notes.....: Draw a drop shadow.                                            *
*****************************************************************************/

VOID DropShadow( USHORT usTop, USHORT usLeft, USHORT usBottom, USHORT usRight )
{
    BYTE   bBuffer[ 2 ];
    USHORT usWidth = 2;
    
    // There has to be a better way of doing this side....
    
    for ( INT i = ( usTop + 1 ); i < ( usBottom + 1 ); i++ )
    {
	VioReadCellStr( bBuffer, &usWidth, (USHORT) i, (USHORT) 
			( usRight + 1 ), 0 );
	bBuffer[ 1 ] = (BYTE) iClrShadow;
	VioWrtCellStr( bBuffer, usWidth, (USHORT) i, (USHORT) 
		       ( usRight + 1 ), 0 );
    }
    
    // Now the bottom (easier);
    
    usWidth = (USHORT) ( ( (INT) ( usRight - usLeft ) + 1 ) * 2 );
    
    PBYTE pbBuffer = new BYTE[ usWidth ];
    PBYTE pb       = pbBuffer;
    
    VioReadCellStr( pbBuffer, &usWidth, (USHORT) ( usBottom + 1 ), 
		    (USHORT) ( usLeft + 1 ), 0 );
    
    INT iLen = ( ( usRight - usLeft ) + 1 );
    
    for ( INT i = 0; i < iLen; i++, pb += 2 )
    {
	pb[ 1 ] = (BYTE) iClrShadow;
    }
    
    VioWrtCellStr( pbBuffer, usWidth, (USHORT) ( usBottom + 1 ), 
		   (USHORT) ( usLeft + 1 ), 0 );
    
    delete[] pbBuffer;
}
