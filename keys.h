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

#ifndef __KEYS_H
#define __KEYS_H

#define K_EXTENDED      0xFF

// Cursor movement keys.

#define K_UP            ( K_EXTENDED + 0x48 )
#define K_DOWN          ( K_EXTENDED + 0x50 )
#define K_RIGHT         ( K_EXTENDED + 0x4D )
#define K_LEFT          ( K_EXTENDED + 0x4B )
#define K_HOME          ( K_EXTENDED + 0x47 )
#define K_END           ( K_EXTENDED + 0x4F )
#define K_PGUP          ( K_EXTENDED + 0x49 )
#define K_PGDN          ( K_EXTENDED + 0x51 )

// "General" keys.

#define K_RETURN        ( 0x0D )
#define K_ENTER         ( K_EXTENDED + 0xE0 )
#define K_ESC           ( 0x1B )
#define K_PLUS          ( 0x2B )
#define K_MINUS         ( 0x2D )

// Function keys.

#define K_F1            ( K_EXTENDED + 0x3B )
#define K_F10           ( K_EXTENDED + 0x44 )

// Alt Keys.

#define K_ALT_D         ( K_EXTENDED + 0x20 )

// Control Keys.

#define K_CTRL_D        ( 0x4 )

#endif
