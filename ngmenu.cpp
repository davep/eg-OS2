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

#include "ngmenu.h"

NGMENU::NGMENU( VOID )
{
    usMax  = 10;
    usNext = 0;
    
    prompts = new NGMENUENTRY *[ usMax ];
}

NGMENU::~NGMENU( VOID )
{
    for ( USHORT us = 0; us < usNext; us++ )
    {
	delete prompts[ us ];
    }
    
    delete[] prompts;
}

VOID NGMENU::addItem( PSZ pszName, LONG lOffset )
{
    if ( usNext == usMax )
    {
	usMax += 10;
	
	NGMENUENTRY **New = new NGMENUENTRY *[ usMax ];
	NGMENUENTRY **Temp;
	
	memcpy( New, prompts, sizeof( NGMENUENTRY * ) * usNext );
	
	Temp    = prompts;
	prompts = New;
	
	delete[] Temp;
    }
    
    prompts[ usNext ] = new NGMENUENTRY;
    
    strcpy( (char *) prompts[ usNext ]->szName, (char *) pszName );
    prompts[ usNext ]->lPtr = lOffset;
    
    ++usNext;
}
