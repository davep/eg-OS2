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

#ifndef __SUPPORT_H
#define __SUPPORT_H

#include <stdio.h>

inline BYTE decrypt( BYTE b ) { return( (BYTE) ( b ^ 0x1A ) ); }

struct SaveListArgs
{
    INT iCount;
    PSZ *ppszList;
    PSZ *ppszPaths;
};

BYTE    ReadByte( FILE * );
USHORT  ReadWord( FILE * );
LONG    ReadLong( FILE * );
INT     GetStrZ( FILE *, PBYTE, INT );
INT     InKey( VOID );
PSZ     CurrentGuide( VOID );
VOID    CurrentGuide( PSZ );
VOID    BeginEG( PSZ );
VOID    EndEG( VOID );
VOID    SaveSettings( VOID );
PSZ     HomeDir( VOID );
INT     ListCount( VOID );
VOID    SaveList( VOID * );
VOID    LoadList( SaveListArgs * );

extern INT iClrNormal;
extern INT iClrBold;
extern INT iClrUnderline;
extern INT iClrReverse;
extern INT iClrHelp;
extern INT iClrShadow;
extern INT iClrMenu;
extern INT iClrMenuPrompt;

#endif
