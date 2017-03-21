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

#ifndef __NGENTRY_H
#define __NGENTRY_H

#include <stdio.h>

class NG;
class NGMENU;

class NGENTRY
{
public:
    NGENTRY( INT, INT );
    ~NGENTRY( VOID );
    
    INT     lineCnt( VOID )         { return( iLines ); }
    PSZ     line( INT i )           { return( ppszLines[ i ] ); }
    LONG    jumpTo( VOID )          { return( fLong ? 0L : 
					      ( plOffsets[ iCurrent ] > 0 ? 
						plOffsets[ iCurrent ] : 
						0L ) ); }
    LONG    address( VOID )         { return( lOffset ); }
    LONG    previous( VOID )        { return( fLong ? lPrevious : 0L ); }
    LONG    next( VOID )            { return( fLong ? lNext     : 0L ); }
    BOOL    hasParent( VOID )       { return( (BOOL) lParent > 0 && 
					      ( iParentLine > -1 && 
						iParentLine < 0xFFFF ) ); }
    LONG    parent( VOID )          { return( lParent ); }
    BOOL    isLong( VOID )          { return( fLong ); }
    NGMENU  *seeAlsos( VOID )       { return( pSeeAlsos ); }
    BOOL    hasSeeAlso( VOID )      { return( fSeeAlso && 
					      ( pSeeAlsos->size() != 0 ) ); }
    VOID    backTrack( NG * );

    VOID    readShort( FILE * );
    VOID    readLong( FILE * );
    VOID    redraw( VOID );
    VOID    up( VOID );
    VOID    down( VOID );
    VOID    home( VOID );
    VOID    end( VOID );
    VOID    pageUp( VOID );
    VOID    pageDown( VOID );
    
private:
    PSZ    *ppszLines;
    LONG   *plOffsets;
    NGMENU *pSeeAlsos;
    LONG   lOffset;
    INT    iLines;
    BOOL   fLong;
    INT    iTop;
    INT    iBottom;
    INT    iCurrent;
    INT    iPrevious;
    INT    iVisTop;
    INT    iVisBtm;
    LONG   lParent;
    INT    iParentLine;
    LONG   lPrevious;
    LONG   lNext;
    BOOL   fSeeAlso;
    
    PSZ     formatLine( PSZ );
    VOID    killLines( VOID );
    BYTE    hex2Byte( PSZ );
    VOID    readText( FILE *, INT );
    VOID    showNormal( INT );
    VOID    showHigh( INT );
    VOID    jumpToLine( INT );
};

#endif
