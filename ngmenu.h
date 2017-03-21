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

#ifndef __NGMENU_H
#define __NGMENU_H

struct NGMENUENTRY
{
    LONG    lPtr;
    BYTE    szName[ 50 ];
};

class NGMENU
{
public:
    CHAR szTitle[ 40 ];
    
    NGMENU( VOID );
    ~NGMENU( VOID );
    
    VOID    addItem( PSZ, LONG );
    USHORT  size( VOID )            { return( usNext ); }
    PSZ     prompt( INT i )         { return( prompts[ i ]->szName ); }
    LONG    offset( INT i )         { return( prompts[ i ]->lPtr ); }
    
private:
    NGMENUENTRY     **prompts;
    
    USHORT  usNext;
    USHORT  usMax;
};

#endif
