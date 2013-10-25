/**
 * $Id: MainWnd.h,v 1.6 2001/04/18 14:46:56 mvines Exp $
 *
 *  Copyright (C) 2000  Michael Vines
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

#pragma once

// MainWnd.h : header file
//

#define UWM_SYSTRAY (WM_USER + 1) // Sent to us by the systray

#include "toon.h"
#include "Afxtempl.h"
#include "winpenguinsdlg.h"
#include "def.h"

extern void (WINAPI *transparentblt) (HDC,int,int,int,int,HDC,int,int,int,int,UINT);
extern void (WINAPI *alphablend)(HDC,int,int,int,int,HDC,int,int,int,int,BLENDFUNCTION);

/////////////////////////////////////////////////////////////////////////////
// CMainWnd window

class CMainWnd : public CFrameWnd
{
private:
	void CreateScreenBitmaps();

	int m_numPenguins;
	int m_moveDelay;
	int m_splatDist;


	CArray<CToon*,CToon*> toonList;

	CBitmap bgBitmap;
	CBitmap activeBmp;

	HMODULE msimg32;
	HMODULE winmon;
	TCHAR winmonFileName[MAX_PATH];

	HICON trayIcon;
	HANDLE hInstanceMutex;

	CDialog *activeDlg;


	// Construction
public:
	CMainWnd();

	// Attributes
public:

	// desktop window that is drawn on
	static CWnd dskWnd;

	// The region that covers all top level windows
	static CRgn *wndRgn;

	// alpha blending level (0-255)
	static unsigned char blendLevel;

	// !0 if sound is enabled
	static int soundEnabled;

	// number between 0-100: 0 = no santa, 100 = always santa
	static int santaPercent;

	// MPA 4-3-2005: added "soundfilename"
	static CString soundFilename;

	// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainWnd)
	//}}AFX_VIRTUAL

	// Implementation
public:
	bool CheckSubType( CToon *pToon );
	void UpdateWndRgn();
	void UpdateBgBitmap(RECT *updateRect);


	void SetToonCountTo(int count);
	void ApplyOptions(CWinpenguinsDlg *const dlg);
	void BrowseSoundFilename(CWinpenguinsDlg *const dlg);
	virtual ~CMainWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMainWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnSysTray(WPARAM wParam, LPARAM lParam);
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnAbout();
	afx_msg void OnExit();
	afx_msg void OnOptions();
	afx_msg void OnScreenCapture();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
