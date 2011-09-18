/**
 * $Id: winpenguinsDlg.h,v 1.5 2001/04/19 19:16:16 mvines Exp $
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

#if !defined(AFX_WINPENGUINSDLG_H__9A375BB6_4AD9_4221_9C06_B83B6BF7DDA8__INCLUDED_)
#define AFX_WINPENGUINSDLG_H__9A375BB6_4AD9_4221_9C06_B83B6BF7DDA8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "UrlStatic.h"

// slowest allowed time between frames in ms
#define MAX_MOVE_DELAY 150

// max number of toons
#define MAX_TOON_COUNT 50

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	virtual BOOL OnInitDialog();

// Implementation
protected:
  CUrlStatic urlStatic;

	//{{AFX_MSG(CAboutDlg)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CWinpenguinsDlg dialog

class CWinpenguinsDlg : public CDialog
{
// Construction
public:
	CWinpenguinsDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CWinpenguinsDlg)
	enum { IDD = IDD_WINPENGUINS_DIALOG };
	int		m_pcount;
	int		m_delay;
	int		m_splat;
	int		m_alpha;
	int		m_soundenabled;
	int		m_santa;
	CString	m_soundfilename;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWinpenguinsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CWinpenguinsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnApply();
	afx_msg void OnSFBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINPENGUINSDLG_H__9A375BB6_4AD9_4221_9C06_B83B6BF7DDA8__INCLUDED_)
