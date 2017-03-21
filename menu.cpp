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

#include <os2.h>

#include <string.h>

#include "menu.h"
#include "screen.h"
#include "support.h"

MENU::MENU( NGMENU *NgMenu )
{
    pNgMenu   = NgMenu;
    pImage    = new IMAGE;
    iCurrent  = 0;
    iPrevious = 0;
}

MENU::~MENU( VOID )
{
    delete pImage;
}

VOID MENU::drop( INT iT, INT iL )
{
    INT iMaxWidth = 0;
    INT iLen;
    
    for ( INT i = 0; i < pNgMenu->size(); i++ )
    {
	iLen = strlen( (char *) pNgMenu->prompt( i ) );
	
	if ( iLen > iMaxWidth )
	{
	    iMaxWidth = iLen;
	}
    }
    
    iTop    = iT;
    iLeft   = iL;
    iBottom = ( iTop + pNgMenu->size() + 1 );
    iRight  = ( iLeft + iMaxWidth + 3 );
    
    pImage->save( iTop, iLeft, iBottom + 1, iRight + 1 );
    
    ClrArea( (USHORT) iTop, (USHORT) iLeft, (USHORT) iBottom, 
	     (USHORT) iRight, (BYTE) iClrMenu );
    DrawBox( (USHORT) iTop, (USHORT) iLeft, (USHORT) iBottom, 
	     (USHORT) iRight, (BYTE) iClrMenu );
    DropShadow( (USHORT) iTop, (USHORT) iLeft, (USHORT) iBottom, 
		(USHORT) iRight );
    
    for ( INT i = 0; i < pNgMenu->size(); i++ )
    {
	SayString( (USHORT) ( iTop + 1 + i ), (USHORT) ( iLeft + 2 ), 
		   pNgMenu->prompt( i ), (BYTE) iClrMenu );
    }
    
    refresh();
}

VOID MENU::kill( VOID )
{
    pImage->restore();
}

VOID MENU::up( VOID )
{
    iPrevious = iCurrent--;
    
    if ( iCurrent < 0 )
    {
	iCurrent = ( pNgMenu->size() - 1 );
    }
    
    refresh();
}

VOID MENU::down( VOID )
{
    iPrevious = iCurrent++;
    
    if ( iCurrent == pNgMenu->size() )
    {
	iCurrent = 0;
    }
    
    refresh();
}

VOID MENU::home( VOID )
{
    iPrevious = iCurrent;
    iCurrent  = 0;
    refresh();
}

VOID MENU::end( VOID )
{
    iPrevious = iCurrent;
    iCurrent  = ( pNgMenu->size() - 1 );
    refresh();
}

VOID MENU::refresh( VOID )
{
    ClrArea( (USHORT) ( iTop + 1 + iPrevious ), (USHORT) ( iLeft + 1 ), 
	     (USHORT) ( iTop + 1 + iPrevious ), (USHORT) ( iRight - 1 ), 
	     (BYTE) iClrMenu );
    SayString( (USHORT) ( iTop + 1 + iPrevious ), (USHORT) ( iLeft + 2 ), 
	       pNgMenu->prompt( iPrevious ), (BYTE) iClrMenu );
    ClrArea( (USHORT) ( iTop + 1 + iCurrent ), (USHORT) ( iLeft + 1 ), 
	     (USHORT) ( iTop + 1 + iCurrent ), (USHORT) ( iRight - 1 ), 
	     (BYTE) iClrMenuPrompt );
    SayString( (USHORT) ( iTop + 1 + iCurrent ), (USHORT) ( iLeft + 2 ), 
	       pNgMenu->prompt( iCurrent ), (BYTE) iClrMenuPrompt );
}
