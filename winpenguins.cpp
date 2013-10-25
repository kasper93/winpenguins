/**
 * $Id: winpenguins.cpp,v 1.2 2001/04/08 08:07:35 tlee1 Exp $
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
#include "MainWnd.h"
#include "screen_capture.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWinpenguinsApp

BEGIN_MESSAGE_MAP(CWinpenguinsApp, CWinApp)
	//{{AFX_MSG_MAP(CWinpenguinsApp)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWinpenguinsApp construction

CWinpenguinsApp::CWinpenguinsApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CWinpenguinsApp object

CWinpenguinsApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CWinpenguinsApp initialization

BOOL CWinpenguinsApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	SetRegistryKey(L"WinPenguins");

	m_pMainWnd = new CMainWnd;
	return TRUE;
}

