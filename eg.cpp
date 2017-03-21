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

#include "ng.h"
#include "image.h"
#include "screen.h"
#include "support.h"
#include "version.h"
#include "navigate.h"

/*****************************************************************************
* Function..: main()                                                         *
* Syntax....: void main( int ArgC, char **ArgV )                             *
* By........: David A Pearson                                                *
* Date......: 14th March 1995                                                *
* Notes.....: Guess!                                                         *
*****************************************************************************/

INT main( int ArgC, char **ArgV )
{
    BeginEG( (PSZ) ArgV[ 0 ] );

    if ( ArgC > 1 )
    {
	CurrentGuide( (PSZ) ArgV[ 1 ] );
    }
    
    if ( CurrentGuide() )
    {
	VIOCURSORINFO savCsr;
	VIOCURSORINFO setCsr;
	
	VioGetCurType( &savCsr, 0 );
	VioGetCurType( &setCsr, 0 );
	
	setCsr.attr = (USHORT) -1;
	
	VioSetCurType( &setCsr, 0 );
	
	IMAGE imgScreen;
	
	imgScreen.save();
	
	BOOL fReading = TRUE;
	
	while ( fReading )
	{
	    NG *pNg = new NG( CurrentGuide() );
	    
	    if ( pNg->isValid() )
	    {
		DrawScreen( pNg );
		
		fReading = Navigate( pNg );
	    }
	    else
	    {
		fprintf( stderr, "\nBARF!! Sorry, can't read '%s'\n"
			 "Press any key.....", CurrentGuide() );
		InKey();
		fReading = FALSE;
	    }
	    
	    delete pNg;
	}
	
	imgScreen.restore();
	
	SaveSettings();
	
	VioSetCurType( &savCsr, 0 );
    }
    else
    {
	fprintf( stderr, "\nExpert Guide/2 " EG_VERSION " By Dave Pearson"
		 "\nFor the moment the syntax is:\n"
		 "\nEg <NgFileName>\n" );
	
    }
    
    EndEG();

    return( 0 );
}
