/**
 * $Id: winpenguinsDlg.cpp,v 1.5 2001/04/19 19:16:16 mvines Exp $
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

#include "stdafx.h"
#include "winpenguins.h"
#include "winpenguinsDlg.h"
#include "MainWnd.h"
#include "screen_capture.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

BOOL CAboutDlg::OnInitDialog()
{
	(void) CDialog::OnInitDialog();

	urlStatic.Create(this, IDC_STATIC_URL);

	CWnd *gplEdit = GetDlgItem(IDC_GPL);
	CString gplText;
	gplText.LoadString(IDS_GPL);
	gplEdit->SetWindowText(gplText);

	SetForegroundWindow();
	return true;
}


void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()





void CAboutDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CDialog::OnLButtonDblClk(nFlags, point);

	//	CAnimateCtrl *ani = (CAnimateCtrl*)GetDlgItem(IDC_ANIWALKER);
	//	ani->Open(penguinRes[rand()%PENGUIN_COUNT]);
}




/////////////////////////////////////////////////////////////////////////////
// CWinpenguinsDlg dialog

CWinpenguinsDlg::CWinpenguinsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWinpenguinsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWinpenguinsDlg)
	m_pcount = 0;
	m_delay = 0;
	m_alpha = 255;
	m_santa = 10;
	m_soundenabled = 0;
	m_soundfilename = "";
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWinpenguinsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWinpenguinsDlg)
	DDX_Slider(pDX, IDC_PCOUNT, m_pcount);
	DDX_Slider(pDX, IDC_SPEED, m_delay);
	DDX_Slider(pDX, IDC_SPLAT, m_splat);
	DDX_Slider(pDX, IDC_ALPHA, m_alpha);
	DDX_Slider(pDX, IDC_SANTA, m_santa);
	DDX_Text(pDX, IDC_SOUNDFILENAME, m_soundfilename);
	DDX_Check(pDX, IDC_SOUNDENABLED, m_soundenabled);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWinpenguinsDlg, CDialog)
	//{{AFX_MSG_MAP(CWinpenguinsDlg)
	ON_BN_CLICKED(IDAPPLY, OnApply)
	ON_BN_CLICKED(IDSFBROWSE, OnSFBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWinpenguinsDlg message handlers

BOOL CWinpenguinsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	SetForegroundWindow();

	CSliderCtrl *p = (CSliderCtrl*)GetDlgItem(IDC_PCOUNT);
	p->SetRange(1, MAX_TOON_COUNT);
	p->SetTicFreq(5);
	p->SetPos(m_pcount);
	p->UpdateWindow();

	p = (CSliderCtrl*)GetDlgItem(IDC_SPEED);
	p->SetRange(0, MAX_MOVE_DELAY);
	p->SetTicFreq(10);
	p->SetPos(m_delay);
	p->UpdateWindow();


	RECT disp;
	::GetWindowRect(::GetDesktopWindow(), &disp);

	p = (CSliderCtrl*)GetDlgItem(IDC_SPLAT);
	p->SetRange(0, disp.bottom);
	p->SetTicFreq(100);
	p->SetPos(m_splat);
	p->UpdateWindow();


	p = (CSliderCtrl*)GetDlgItem(IDC_ALPHA);
	p->SetRange(0, 255);
	p->SetTicFreq(32);
	p->SetPos(m_alpha);
	p->UpdateWindow();

	p = (CSliderCtrl*)GetDlgItem(IDC_SANTA);
	p->SetRange(0, 100);
	p->SetTicFreq(10);
	p->SetPos(m_santa);
	p->UpdateWindow();


	//--- MPA 4-3-2005: soundFilename support ---
	CEdit *e = (CEdit*)GetDlgItem(IDC_SOUNDFILENAME);
	e->SetDlgItemText(IDC_SOUNDFILENAME,m_soundfilename);
	e->UpdateWindow();
	CButton *f = (CButton*)GetDlgItem(IDSFBROWSE);
	f->UpdateWindow();
	//---

	CButton *b = (CButton*)GetDlgItem(IDC_SOUNDENABLED);
	b->SetCheck(m_soundenabled);


	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CWinpenguinsDlg::OnApply()
{
	UpdateData(TRUE);
	((CMainWnd*)theApp.m_pMainWnd)->ApplyOptions(this);
}


//MPA 4-3-2005: soundfilename support
void CWinpenguinsDlg::OnSFBrowse()
{
	UpdateData(TRUE);
	((CMainWnd*)theApp.m_pMainWnd)->BrowseSoundFilename(this);
	CEdit *ef = (CEdit*)GetDlgItem(IDC_SOUNDFILENAME);
	ef->SetWindowText((LPCSTR)m_soundfilename);
	ef->UpdateWindow();
}


