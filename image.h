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

#ifndef __IMAGE_H
#define __IMAGE_H

class IMAGE
{
public:
    IMAGE( VOID );
    ~IMAGE( VOID );
    
    VOID save( INT, INT, INT, INT );
    VOID save( VOID );
    VOID restore( VOID );
    
private:
    PBYTE pbBuffer;
    INT   iTop;
    INT   iLeft;
    INT   iBottom;
    INT   iRight;
};

#endif
