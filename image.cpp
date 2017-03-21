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

// Local header files.

#include "image.h"

/*****************************************************************************
* Function..: IMAGE()                                                        *
* Class.....: IMAGE                                                          *
* Syntax....: IMAGE( VOID )                                                  *
* By........: David A Pearson                                                *
* Date......: 6th April 1995                                                 *
* Notes.....: Constructor for the IMAGE class.                               *
*****************************************************************************/

IMAGE::IMAGE( VOID )
{
    pbBuffer = (BYTE *) 0;
}

/*****************************************************************************
* Function..: ~IMAGE()                                                       *
* Class.....: IMAGE                                                          *
* Syntax....: ~IMAGE( VOID )                                                 *
* By........: David A Pearson                                                *
* Date......: 6th April 1995                                                 *
* Notes.....: Destructor for the IMAGE class.                                *
*****************************************************************************/

IMAGE::~IMAGE( VOID )
{
    if ( pbBuffer )
    {
	delete[] pbBuffer;
    }
}

/*****************************************************************************
* Function..: save()                                                         *
* Class.....: IMAGE                                                          *
* Syntax....: VOID save( INT iT, INT iL, INT iB, INT iR )                    *
* By........: David A Pearson                                                *
* Date......: 6th April 1995                                                 *
* Notes.....: Save the contents of the passed area of screen.                *
*****************************************************************************/

VOID IMAGE::save( INT iT, INT iL, INT iB, INT iR )
{
    if ( pbBuffer )
    {
	delete[] pbBuffer;
    }
    
    pbBuffer = new BYTE[ ( ( ( iR - iL ) + 1 ) * 
			   ( ( ( iB - iT ) + 1 ) ) ) * 2 ];
    
    PBYTE  pb      = pbBuffer;
    USHORT usWidth = (USHORT) ( (INT) ( ( ( iR - iL ) + 1 ) * 2 ) );
    
    for ( INT i = iT; i <= iB; i++, pb += usWidth )
    {
	VioReadCellStr( pb, &usWidth, (USHORT) i, (USHORT) iL, 0 );
    }
    
    iTop    = iT;
    iLeft   = iL;
    iBottom = iB;
    iRight  = iR;
}

/*****************************************************************************
* Function..: save()                                                         *
* Class.....: IMAGE                                                          *
* Syntax....: VOID save( VOID )                                              *
* By........: David A Pearson                                                *
* Date......: 6th April 1995                                                 *
* Notes.....: Save the total contents of the screen.                         *
*****************************************************************************/

VOID IMAGE::save( VOID )
{
    VIOMODEINFO mi;
    
    mi.cb = sizeof( VIOMODEINFO );
    
    VioGetMode( &mi, 0 );
    
    save( 0, 0, mi.row, mi.col );
}

/*****************************************************************************
* Function..: restore()                                                      *
* Class.....: IMAGE                                                          *
* Syntax....: VOID restore( VOID )                                           *
* By........: David A Pearson                                                *
* Date......: 6th April 1995                                                 *
* Notes.....: Restore a saved screen image.                                  *
*****************************************************************************/

VOID IMAGE::restore( VOID )
{
    if ( pbBuffer )
    {
	PBYTE pb     = pbBuffer;
	INT   iWidth = ( ( ( iRight - iLeft ) + 1 ) * 2 );
	
	for ( INT i = iTop; i <= iBottom; i++, pb += iWidth )
	{
	    VioWrtCellStr( pb, (USHORT) iWidth, (USHORT) i, 
			   (USHORT) iLeft, 0 );
	}
    }
}
