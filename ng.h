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

#ifndef __NG_H
#define __NG_H

#include <stdio.h>

// Include local header files.

#include "ngmenu.h"
#include "ngentry.h"

// The magic number of a Norton Guide file.

#define NG_MAGIC        0x474E

// Structure of a Norton Guide header.

struct NGHEADER
{
    USHORT usMagic;
    SHORT  sUnknown1;
    SHORT  sUnknown2;
    USHORT usMenuCnt;
    CHAR   szTitle[ 40 ];
    CHAR   pszCredits[ 5 ][ 66 ];
};

// Norton Guide reader engine class..

class NG
{
public:
    NGHEADER        NgHeader;
    NGMENU          **Menus;
    
    NG( PSZ );
    ~NG( VOID );
    
    BOOL    isValid( VOID )         { return( NgHeader.usMagic == NG_MAGIC ); }
    INT     menuCnt( VOID )         { return( (INT) NgHeader.usMenuCnt ); }
    VOID    readMenuEntry( INT, INT, NGENTRY * );
    VOID    readEntry( LONG, NGENTRY * );
    BOOL    isEntry( LONG );
    VOID    showHelp( VOID );
    
private:
    FILE    *filNg;
    INT     iNextMenu;
    
    VOID    loadMenus( VOID );
    VOID    loadMenu( VOID );
    VOID    skipShortLong( VOID );
};

#endif
