/**
 * $Id: Toon.h,v 1.2 2001/04/08 08:07:35 tlee1 Exp $
 *
 *  Copyright (C) 2000  Michael Vines
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

#if !defined(AFX_TOON_H__7339F5A5_53B0_4448_8C93_420E8E3B5FEC__INCLUDED_)
#define AFX_TOON_H__7339F5A5_53B0_4448_8C93_420E8E3B5FEC__INCLUDED_

#pragma once
// Toon.h : header file
//

// return values from AdvanceToon
#define TOON_OK 0
#define TOON_PARTIALMOVE 1
#define TOON_BLOCKED -1

#define TOON_LEFT 0
#define TOON_RIGHT 1
#define TOON_UP 2
#define TOON_DOWN 3
#define TOON_HERE -1


typedef enum TToonSubType
{
	TST_UNSPECIFIED,
	TST_NORMAL_PENGUIN,
	TST_SANTA_PENGUIN

} ToonSubType;

/* Bitmap colour that will be transparent */
#define BG_COLOUR RGB(0,0,0)


/////////////////////////////////////////////////////////////////////////////
// CToon window

class CToon
{

	// Construction
public:
	CToon();

	// Attributes
public:
	int m_x;
	int m_y;
	int m_u;
	int m_v;
	int m_prefd;  // prefered direction -- -1 = none
	int m_tumbleStartY;

	int m_bmpIndex;
	int m_frameIndex;
	int m_directionIndex;

	bool m_active;
	bool m_startingUp;

	ToonSubType m_tstSubType;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToon)
	//}}AFX_VIRTUAL

	// Implementation
public:
	bool IsBlocked(int direction);
	int AdvanceToon(bool force);
	void SetVelocity(int u, int v);
	void SetPosition(int x, int y);
	void SetType(int type, int direction);
	virtual ~CToon();

	void DeleteAni();
	void ExplodeAni();

	void GetRect(RECT &rt);
	void GetRect(RECT &rt, int newx, int newy);

	void Erase(CWnd &wnd, RECT *wndRt);

	void Paint(CDC *activeDC, CDC *tmpDc);
	void PaintBackground(CDC *bgBitmapDC, CDC *activeDC);
	void PaintToDesktop(CDC *dc, CDC *activeBmpDC);

	void SetSubType( ToonSubType tst ) {
		m_tstSubType = tst;
	}
	ToonSubType GetSubType( void ) {
		return m_tstSubType;
	}
private:
	void AdvanceFrame();

	// Generated message map functions
protected:


	//{{AFX_MSG(CToon)
	//}}AFX_MSG
	//	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOON_H__7339F5A5_53B0_4448_8C93_420E8E3B5FEC__INCLUDED_)
