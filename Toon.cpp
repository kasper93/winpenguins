/**
 * $Id: Toon.cpp,v 1.7 2001/04/16 17:20:25 mvines Exp $
 *
 *  Copyright (C) 2000,2001  Michael Vines
 *
 *  This file contains code from XPenguins 1.1
 *		Copyright (C) 1999,2000  Robin Hogan
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

#include "stdafx.h"
#include "winpenguins.h"
#include "Toon.h"
#include "MainWnd.h"
#include "def.h"
#include <mmsystem.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


ToonData penguin_data[PENGUIN_COUNT] = {
	{ IDB_WALKER,		 IDB_WALKERMSK,			NULL, NULL, 8, 2, 32, 32, TRUE },
	{ IDB_FALLER,		 IDB_FALLERMSK,			NULL, NULL, 8, 1, 32, 32, TRUE },
	{ IDB_TUMBLER,		 IDB_TUMBLERMSK,		NULL, NULL, 8, 1, 32, 32, TRUE },
	{ IDB_FLOATER,		 IDB_FLOATERMSK,		NULL, NULL, 8, 1, 32, 32, TRUE },
	{ IDB_CLIMBER,		 IDB_CLIMBERMSK,		NULL, NULL, 8, 2, 32, 32, TRUE },
	{ IDB_BOMBER,		 IDB_BOMBERMSK,			NULL, NULL, 17, 1, 32, 32, FALSE },
	{ IDB_EXPLOSION,	 IDB_EXPLOSIONMSK,		NULL, NULL, 3, 1, 64, 64, FALSE },
	{ IDB_SANTA_WALKER,	 IDB_SANTA_WALKERMSK,	NULL, NULL, 8, 2, 32, 32, TRUE },
	{ IDB_SANTA_CLIMBER, IDB_SANTA_CLIMBERMSK,	NULL, NULL, 8, 2, 32, 32, TRUE }
};


#define PAINT_PADDING 4


// Used in Win95/98/NT.  Win2000 uses the TransparentBlt function.
//  -  Win98 also has this function but it seems to leak GDI resources
//     whenever I use it
static void myTransparentBlt(CDC *dst, int x, int y, int w, int h,
							 CDC *src, int srcx, int srcy, CBitmap *mskBmp)
{
	if (NULL == transparentblt) {
		CDC mskDC;
		CBitmap *oldBmp;

		mskDC.CreateCompatibleDC(dst);

		oldBmp = mskDC.SelectObject(mskBmp);

		// Mask out places where the bitmap will be placed
		// SRCAND of dst and mskBmp
		dst->BitBlt(x, y, w, h, &mskDC, srcx, srcy, SRCAND);

		// OR the bitmap to the destination
		dst->BitBlt(x, y, w, h, src, srcx, srcy, SRCPAINT);

		mskDC.SelectObject(oldBmp);
	} else {
		transparentblt(*dst, x, y, w, h,
					   *src, srcx, srcy, w, h, BG_COLOUR);
	}
}


static void doAlphaBlend(CDC *dst, int x, int y, int w, int h,
						 CDC *src, int srcx, int srcy, CBitmap *mskBmp)
{
	BLENDFUNCTION bf;
	CDC blendDC, mergeDC, maskDC;
	CBitmap *oldBlendBmp, *oldMergeBmp, *oldMaskBmp;
	CBitmap blendBmp, mergeBmp;


	bf.BlendOp = AC_SRC_OVER;
	bf.BlendFlags = 0;
	bf.SourceConstantAlpha = CMainWnd::blendLevel;
	bf.AlphaFormat = 0;


	blendBmp.CreateCompatibleBitmap(dst, w, h);
	blendDC.CreateCompatibleDC(dst);
	oldBlendBmp = blendDC.SelectObject(&blendBmp);

	mergeBmp.CreateCompatibleBitmap(dst, w, h);
	mergeDC.CreateCompatibleDC(dst);
	oldMergeBmp = mergeDC.SelectObject(&mergeBmp);

	maskDC.CreateCompatibleDC(dst);
	oldMaskBmp = maskDC.SelectObject(mskBmp);


	blendDC.BitBlt(0, 0, w, h, dst, x, y, SRCCOPY);
	alphablend(blendDC, 0, 0, w, h, *src, srcx, srcy, w, h, bf);

	/*  now blendDC has a alpha blended version of the penguin, unfortunatly
	    the background has also been alpha blended and needs to be
	    masked out */
	mergeDC.BitBlt(0, 0, w, h, &maskDC, srcx, srcy, NOTSRCCOPY);
	mergeDC.BitBlt(0, 0, w, h, &blendDC, 0, 0, SRCAND);


	if (NULL == transparentblt) {
		dst->BitBlt(x, y, w, h, &maskDC, srcx, srcy, SRCAND);
		dst->BitBlt(x, y, w, h, &mergeDC, 0, 0, SRCPAINT);
	} else {
		transparentblt(*dst, x, y, w, h,
					   mergeDC, 0, 0, w, h, BG_COLOUR);
	}


	maskDC.SelectObject(oldMaskBmp);
	mergeDC.SelectObject(oldMergeBmp);
	blendDC.SelectObject(oldBlendBmp);
}



/////////////////////////////////////////////////////////////////////////////
// CToon

CToon::CToon()
	: m_tstSubType( TST_UNSPECIFIED )
	, m_tumbleStartY(0)
{
	RECT rt;
	CMainWnd::dskWnd.GetClientRect(&rt);

	m_frameIndex = 0;
	m_active = true;
	m_startingUp = true;
	m_prefd = -1;

	SetType(PENGUIN_FALLER, PENGUIN_FOREWARD);
	SetPosition(rand() % (rt.right - penguin_data[m_bmpIndex].width), 0);
	SetVelocity((rand() % 2) - 1, 3);
}


CToon::~CToon()
{
}



void CToon::DeleteAni()
{
	if (m_bmpIndex != PENGUIN_BOMBER) {
		m_directionIndex = 0;
		m_frameIndex = 0;
		m_bmpIndex = PENGUIN_BOMBER;
		m_active = true;
	}
}


void CToon::ExplodeAni()
{
	if (PENGUIN_EXPLOSION == m_bmpIndex) {
		return;
	}

	m_directionIndex = 0;
	m_frameIndex = 0;
	m_bmpIndex = PENGUIN_EXPLOSION;
	m_active = true;
	m_x -= 16;
	m_y -= 16;

	if (CMainWnd::soundEnabled) {

		//MPA 4-3-2005: play default sound if no soundFilename, else try file
		if (CMainWnd::soundFilename == "") {
			PlaySound(MAKEINTRESOURCE( IDR_SOUND_BOOM ), theApp.m_hInstance,
							SND_RESOURCE | SND_ASYNC | SND_NOWAIT);
		} else {
			PlaySound(CMainWnd::soundFilename, theApp.m_hInstance,
							SND_FILENAME | SND_ASYNC | SND_NOWAIT);
		}
	}
}



void CToon::GetRect(RECT &rt)
{
	rt.left = m_x;
	rt.top = m_y;
	rt.right = rt.left + penguin_data[m_bmpIndex].width;
	rt.bottom = rt.top + penguin_data[m_bmpIndex].height;
}


void CToon::GetRect(RECT &rt, int newx, int newy)
{
	rt.left = newx;
	rt.top = newy;
	rt.right = rt.left + penguin_data[m_bmpIndex].width;
	rt.bottom = rt.top + penguin_data[m_bmpIndex].height;
}


void CToon::Erase(CWnd &wnd, RECT *wndRt)
{
	RECT rt, ignored;

	GetRect(rt);
	rt.left -= PAINT_PADDING;
	rt.right += PAINT_PADDING;
	rt.top -= PAINT_PADDING;
	rt.bottom += PAINT_PADDING;

	if (IntersectRect(&ignored, &rt, wndRt)) {
		::InvalidateRect(wnd, &rt, TRUE);
	}
}


void CToon::AdvanceFrame()
{
	m_frameIndex++;

	if (m_frameIndex >= penguin_data[m_bmpIndex].nframes) {
		if (penguin_data[m_bmpIndex].cycle) {
			m_frameIndex = 0;
		} else {
			m_active = false;
		}
	}
}



void CToon::Paint(CDC *activeDC, CDC *tmpDc)
{
	if (!m_active) {
		return;
	}

	CBitmap *oldBmp = tmpDc->SelectObject(penguin_data[m_bmpIndex].bmp);

	if ((255 == CMainWnd::blendLevel) || (NULL == alphablend)) {
		myTransparentBlt(activeDC, m_x, m_y,
						 penguin_data[m_bmpIndex].width, penguin_data[m_bmpIndex].height,
						 tmpDc, m_frameIndex * penguin_data[m_bmpIndex].width,
						 m_directionIndex * penguin_data[m_bmpIndex].height,
						 penguin_data[m_bmpIndex].mskBmp);

	} else {
		doAlphaBlend(activeDC, m_x, m_y,
					 penguin_data[m_bmpIndex].width, penguin_data[m_bmpIndex].height,
					 tmpDc, m_frameIndex * penguin_data[m_bmpIndex].width,
					 m_directionIndex * penguin_data[m_bmpIndex].height,
					 penguin_data[m_bmpIndex].mskBmp);
	}

	tmpDc->SelectObject(oldBmp);
}


void CToon::PaintBackground(CDC *bgBitmapDC, CDC *activeDC)
{
	activeDC->BitBlt(m_x - PAINT_PADDING, m_y - PAINT_PADDING,
					 penguin_data[m_bmpIndex].width + PAINT_PADDING*2,
					 penguin_data[m_bmpIndex].height + PAINT_PADDING*2,
					 bgBitmapDC, m_x - PAINT_PADDING, m_y - PAINT_PADDING, SRCCOPY);
}


void CToon::PaintToDesktop(CDC *dc, CDC *activeBmpDC)
{
	dc->BitBlt(m_x - PAINT_PADDING, m_y - PAINT_PADDING,
			   penguin_data[m_bmpIndex].width + PAINT_PADDING*2,
			   penguin_data[m_bmpIndex].height + PAINT_PADDING*2,
			   activeBmpDC, m_x - PAINT_PADDING, m_y - PAINT_PADDING, SRCCOPY);
}


void CToon::SetType(int type, int direction)
{
	m_bmpIndex = type;
	m_directionIndex = direction;
}

void CToon::SetPosition(int x, int y)
{
	m_x = x;
	m_y = y;
}

void CToon::SetVelocity(int u, int v)
{
	m_u = u;
	m_v = v;
}


int CToon::AdvanceToon(bool force)
{
	RECT disp, screen;
	int newx, newy;
	int status = TOON_OK;

	CMainWnd::dskWnd.GetClientRect(&disp);

	newx = m_x + m_u;
	newy = m_y + m_v;

	if (newx < 0) {
		newx = 0;
		status = TOON_PARTIALMOVE;
	} else if (newx + penguin_data[m_bmpIndex].width > disp.right) {
		newx = disp.right - penguin_data[m_bmpIndex].width;
		status = TOON_PARTIALMOVE;
	}
	if (newy < 0) {
		newy = 0;
		status = TOON_PARTIALMOVE;
	} else if (newy + penguin_data[m_bmpIndex].height > disp.bottom) {
		if (!force) {
			newy = disp.bottom - penguin_data[m_bmpIndex].height;
			status = TOON_PARTIALMOVE;
		}
	}

	RECT rt;
	int move_ahead = 1;
	CMainWnd::dskWnd.GetWindowRect(&screen);
	GetRect(rt, newx + screen.left, newy + screen.top);

	if (CMainWnd::wndRgn->RectInRegion(&rt) && !force) {
		int tryx, tryy, step = 1;

		move_ahead = 0;
		status = TOON_BLOCKED;

		if (abs(m_v) < abs(m_u)) {
			if (newx > m_x) {
				step = -1;
			}

			for (tryx = newx + step; tryx != m_x; tryx += step) {
				tryy = m_y + ((tryx - m_x)*m_v)/m_u;

				GetRect(rt, tryx + screen.left, tryy + screen.top);
				if (!CMainWnd::wndRgn->RectInRegion(&rt)) {
					newx = tryx;
					newy = tryy;
					status = TOON_PARTIALMOVE;
					move_ahead = 1;
					break;
				}
			}
		} else {
			if (newy > m_y) {
				step = -1;
			}

			for (tryy = newy + step; tryy != m_y; tryy += step) {
				tryx = m_x + ((tryy - m_y)*m_u)/m_v;

				GetRect(rt, tryx + screen.left, tryy + screen.top);
				if (!CMainWnd::wndRgn->RectInRegion(&rt)) {
					newx = tryx;
					newy = tryy;
					status = TOON_PARTIALMOVE;
					move_ahead = 1;
					break;
				}
			}
		}

	}

	if (move_ahead) {
		m_x = newx;
		m_y = newy;
	}

	if (move_ahead || m_bmpIndex >= PENGUIN_BOMBER) {
		AdvanceFrame();
	}

	if (m_y + penguin_data[m_bmpIndex].height > disp.bottom) {
		ExplodeAni();
	}


	return status;
}

bool CToon::IsBlocked(int direction)
{
	RECT disp, rt, screen;

	CMainWnd::dskWnd.GetWindowRect(&screen);
	CMainWnd::dskWnd.GetClientRect(&disp);


	switch (direction) {
		case TOON_LEFT:
			if (m_x <= 0) {
				return true;
			}
			GetRect(rt, screen.left + m_x - 1, screen.top + m_y);
			break;
		case TOON_RIGHT:
			if (m_x + penguin_data[m_bmpIndex].width >= disp.right) {
				return true;
			}
			GetRect(rt, screen.left + m_x + 1, screen.top + m_y);
			break;
		case TOON_UP:
			if (m_y <= 0) {
				return true;
			}
			GetRect(rt, screen.left + m_x, screen.top + m_y - 1);
			break;
		case TOON_DOWN:
			if (m_y + penguin_data[m_bmpIndex].height >= disp.bottom) {
				return true;
			}
			GetRect(rt, screen.left + m_x, screen.top + m_y + 1);
			break;
		case TOON_HERE:
			GetRect(rt, screen.left + m_x, screen.top + m_y);
			break;
		default:
			ASSERT(0);
	}

	if (CMainWnd::wndRgn->RectInRegion(&rt)) {
		return true;
	}

	return false;
}
