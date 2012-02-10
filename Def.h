/**
 * $Id: Def.h,v 1.2 2001/04/08 08:07:34 tlee1 Exp $
 *
 *  Copyright (C) 2000  Michael Vines
 *
 *  This file contains code from XPenguins 1.1
 *      Copyright (C) 1999,2000  Robin Hogan
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  As a special exception, Michael Vines gives permission to link this program
 *  with the Microsoft Visual C++ Runtime/MFC Environment, and distribute the
 *  resulting executable, without including the source code for the Microsoft
 *  Visual C++ Runtime/MFC Environment in the source distribution
 */

#ifndef _DEF_H_INCLUDED_
#define _DEF_H_INCLUDED_

#define PENGUIN_FOREWARD		0
#define PENGUIN_LEFTRIGHT		1
#define PENGUIN_LEFT			0
#define PENGUIN_RIGHT			1

#define PENGUIN_WALKER			0
#define PENGUIN_FALLER			1
#define PENGUIN_TUMBLER			2
#define PENGUIN_FLOATER			3
#define PENGUIN_CLIMBER			4
#define PENGUIN_BOMBER			5
#define PENGUIN_EXPLOSION		6
#define PENGUIN_SANTA_WALKER	7
#define PENGUIN_SANTA_CLIMBER	8
#define PENGUIN_COUNT			9

typedef struct {
	DWORD resId;	// resource id
	DWORD mskResId;	// resource id of the bitmask
	CBitmap *bmp;
	CBitmap *mskBmp;

	int nframes,ndirections;	/* number of frames and directions in image */
	int width,height;			/* width and height of an individual frame/direction */

	bool cycle;  // does the animation cycle
} ToonData;


extern ToonData penguin_data[PENGUIN_COUNT];

#endif
