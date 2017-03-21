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

#include <string.h>

// Local header files.

#include "keys.h"
#include "menu.h"
#include "nglist.h"
#include "screen.h"
#include "colours.h"
#include "support.h"
#include "navigate.h"

// Prototype local functions.

static INT  DrawMenu( NG *, INT );
static VOID SaySeeAlso( BOOL );

BOOL Navigate( NG *Ng )
{
    BOOL fNewOne = FALSE;
    
    VIOMODEINFO miModeInfo;
    
    miModeInfo.cb = sizeof( VIOMODEINFO );
    VioGetMode( &miModeInfo, 0 );
    
    MENU *pMenu = (MENU *) 0;
    INT  iMenu  = 0;
    
    NGENTRY NgEntry( 3, miModeInfo.row - 2 );
    
    Ng->readMenuEntry( 1, 0, &NgEntry );
    
    NgEntry.redraw();
    
    BOOL fExit     = FALSE;
    LONG lPrevious = 0;
    INT  iKey;
    
    DrawMenu( Ng, 0 );
    
    while ( !fExit )
    {
	iKey = InKey();
	
	switch ( iKey )
	{
	case K_UP :
	{
	    if ( !iMenu )
	    {
		NgEntry.up();
	    }
	    else
	    {
		pMenu->up();
	    }
	    break;
	}
	case K_DOWN :
	{
	    if ( !iMenu )
	    {
		NgEntry.down();
	    }
	    else
	    {
		pMenu->down();
	    }
	    break;
	}
	case K_HOME :
	{
	    if ( !iMenu )
	    {
		NgEntry.home();
	    }
	    else
	    {
		pMenu->home();
	    }
	    break;
	}
	case K_END :
	{
	    if ( !iMenu )
	    {
		NgEntry.end();
	    }
	    else
	    {
		pMenu->end();
	    }
	    break;
	}
	case K_PGUP :
	{
	    if ( !iMenu )
	    {
		NgEntry.pageUp();
	    }
	    else
	    {
		pMenu->home();
	    }
	    break;
	}
	case K_PGDN :
	{
	    if ( !iMenu )
	    {
		NgEntry.pageDown();
	    }
	    else
	    {
		pMenu->end();
	    }
	    break;
	}
	case K_PLUS :
	{
	    if ( NgEntry.next() && !iMenu )
	    {
		lPrevious = NgEntry.address();
		
		Ng->readEntry( NgEntry.next(), &NgEntry );
		NgEntry.redraw();
		if ( NgEntry.isLong() )
		{
		    SaySeeAlso( NgEntry.hasSeeAlso() );
		}
	    }
	    break;
	}
	case K_MINUS :
	{
	    if ( NgEntry.previous() && !iMenu )
	    {
		lPrevious = NgEntry.address();
		
		Ng->readEntry( NgEntry.previous(), &NgEntry );
		NgEntry.redraw();
		if ( NgEntry.isLong() )
		{
		    SaySeeAlso( NgEntry.hasSeeAlso() );
		}
	    }
	    break;
	}
	case K_LEFT :
	{
	    if ( !NgEntry.isLong() )
	    {
		if ( pMenu )
		{
		    pMenu->kill();
		    delete pMenu;
		    pMenu = (MENU *) 0;
		}
		
		--iMenu;
		
		if ( iMenu < 0 )
		{
		    iMenu = Ng->menuCnt() - 1;
		}
		
		INT iMnuLeft = DrawMenu( Ng, iMenu );
		
		if ( iMenu )
		{
		    pMenu = new MENU( Ng->Menus[ iMenu ] );
		    
		    pMenu->drop( 2, iMnuLeft );
		}
	    }
	    
	    break;
	}
	case K_RIGHT :
	{
	    if ( !NgEntry.isLong() )
	    {
		if ( pMenu )
		{
		    pMenu->kill();
		    delete pMenu;
		    pMenu = (MENU *) 0;
		}
		
		++iMenu;
		
		if ( iMenu == Ng->menuCnt() )
		{
		    iMenu = 0;
		}
		
		INT iMnuLeft = DrawMenu( Ng, iMenu );
		
		if ( iMenu )
		{
		    pMenu = new MENU( Ng->Menus[ iMenu ] );
		    
		    pMenu->drop( 2, iMnuLeft );
		}
	    }
	    
	    break;
	}
	case K_RETURN :
	case K_ENTER :
	{
	    if ( !NgEntry.isLong() )
	    {
		if ( !iMenu )
		{
		    if ( Ng->isEntry( NgEntry.jumpTo() ) )
		    {
			lPrevious = NgEntry.address();
			
			Ng->readEntry( NgEntry.jumpTo(), &NgEntry );
			NgEntry.redraw();
		    }
		}
		else
		{
		    lPrevious = NgEntry.address();
		    
		    Ng->readMenuEntry( iMenu, pMenu->chosen(), &NgEntry );
		    pMenu->kill();
		    delete pMenu;
		    iMenu = 0;
		    pMenu = (MENU *) 0;
		    NgEntry.redraw();
		}
	    }
	    else
	    {
		if ( !iMenu )
		{
		    if ( NgEntry.hasSeeAlso() )
		    {
			if ( pMenu )
			{
			    pMenu->kill();
			    delete pMenu;
			}
			
			pMenu = new MENU( NgEntry.seeAlsos() );
			
			pMenu->drop( 2, 2 );
			
			iMenu = 1;
		    }
		}
		else
		{
		    INT i = pMenu->chosen();
		    
		    if ( Ng->isEntry( NgEntry.seeAlsos()->offset( i ) ) )
		    {
			pMenu->kill();
			delete pMenu;
			
			lPrevious = NgEntry.address();
			
			Ng->readEntry( NgEntry.seeAlsos()->offset( i ), 
				       &NgEntry );
			
			iMenu = 0;
			pMenu = (MENU *) 0;
			NgEntry.redraw();
		    }
		}
	    }
	    
	    if ( NgEntry.isLong() )
	    {
		SaySeeAlso( NgEntry.hasSeeAlso() );
	    }
	    else
	    {
		DrawMenu( Ng, iMenu );
	    }
	    
	    break;
	}
	case K_F1 :
	{
	    Ng->showHelp();
	    break;
	}
	case K_F10 :
	{
	    if ( pMenu )
	    {
		pMenu->kill();
		delete pMenu;
		pMenu = (MENU *) 0;
	    }
	    
	    fExit = TRUE;
	}
	case K_ESC :
	{
	    if ( !iMenu )
	    {
		if ( NgEntry.hasParent() )
		{
		    NgEntry.backTrack( Ng );
		    NgEntry.redraw();
		    DrawMenu( Ng, iMenu );
		}
		else if ( NgEntry.isLong() )
		{
		    Ng->readEntry( lPrevious, &NgEntry );
		    NgEntry.redraw();
		    DrawMenu( Ng, iMenu );
		}
		else
		{
		    fExit = TRUE;
		}
	    }
	    else
	    {
		if ( pMenu )
		{
		    pMenu->kill();
		    delete pMenu;
		    pMenu = (MENU *) 0;
		}
		
		iMenu = 0;
		
		if ( NgEntry.isLong() )
		{
		    SaySeeAlso( NgEntry.hasSeeAlso() );
		}
		else
		{
		    DrawMenu( Ng, iMenu );
		}
	    }
	    break;
	}
	case K_ALT_D :
	case K_CTRL_D :
	{
	    if ( !( iMenu && NgEntry.isLong() ) )
	    {
		if ( iMenu )
		{
		    pMenu->kill();
		    delete pMenu;
		    pMenu = (MENU *) 0;
		    iMenu = 0;
		    DrawMenu( Ng, iMenu );
		}
		
		NGLIST *pNgList = new NGLIST( iKey == K_CTRL_D );
		
		if ( pNgList->select() )
		{
		    fExit   = TRUE;
		    fNewOne = TRUE;
		}
		
		delete pNgList;
	    }
	    
	    break;
	}
//                      default :
//                      {
//                              printf( "Unknown key == %d\n", iKey );
//                              InKey();
//                      }
	}
    }
    
    if ( pMenu )
    {
	delete pMenu;
    }
    
    return( fNewOne );
}

/*****************************************************************************
* Function..: DrawMenu()                                                     *
* Syntax....: INT DrawMenu( NG *Ng, INT iMenu )                              *
* By........: David A Pearson                                                *
* Date......: 15th March 1995                                                *
* Notes.....: Display Draw the initial menu display.                         *
*****************************************************************************/

static INT DrawMenu( NG *Ng, INT iMenu )
{
    INT iLeft    = 2;
    INT iMnuLeft = 0;
    
    ClrArea( 1, 1, 1, 78, (BYTE) iClrNormal );
    
    for ( INT i = 0; i < Ng->menuCnt(); i++ )
    {
	if ( iMenu == i )
	{
	    iMnuLeft = iLeft;
	}
	SayString( 1, (USHORT) iLeft, (PSZ) Ng->Menus[ i ]->szTitle, 
		   (BYTE) ( iMenu != i ? iClrNormal : iClrMenuPrompt ) );
	iLeft += strlen( Ng->Menus[ i ]->szTitle ) + 2;
    }
    
    return( iMnuLeft );
}

/*****************************************************************************
* Function..: SaySeeAlso()                                                   *
* Syntax....: VOID SaySeeAlso( BOOL fSayIt )                                 *
* By........: David A Pearson                                                *
* Date......: 13th April 1995                                                *
* Notes.....: Show the See also stuff.                                       *
*****************************************************************************/

static VOID SaySeeAlso( BOOL fSayIt )
{
    ClrArea( 1, 1, 1, 78, (BYTE) iClrNormal );
    
    if ( fSayIt )
    {
	SayString( 1, 2, (PSZ) "See Also...", (BYTE) iClrMenuPrompt );
    }
}
