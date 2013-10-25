/**
 * $Id: MainWnd.cpp,v 1.7 2001/04/18 14:46:56 mvines Exp $
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


#include "stdafx.h"
#include "winpenguins.h"
#include "winpenguinsDlg.h"
#include "MainWnd.h"
#include "def.h"
#include "screen_capture.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// Globals defined by CMainWnd
void (WINAPI* transparentblt)(HDC, int, int, int, int, HDC, int, int, int, int, UINT);
void (WINAPI* alphablend)(HDC, int, int, int, int, HDC, int, int, int, int, BLENDFUNCTION);

/////////////////////////////////////////////////////////////////////////////
// Locals used by CMainWnd

#include "Winmon/winmon_ptr.h"

static BOOL CALLBACK EnumWindowCallback(HWND hWnd, LPARAM /*lParam*/)
{
    RECT rt;
    CRgn rgn;

    if (!IsWindowVisible(hWnd)) {
        return TRUE;
    }

    // Ignore windows in our process
    DWORD pid;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid == GetCurrentProcessId()) {
        return TRUE;
    }

    // Ignore the "Program Manager" window
    TCHAR title[256];
    ::GetWindowText(hWnd, title, 256);
    if (!_tcscmp(title, L"Program Manager")) {
        return TRUE;
    }

    // All good, add this window to the region
    if (!GetWindowRect(hWnd, &rt)) {
        return TRUE;
    }

    if (rt.right < 0 && rt.bottom < 0) {
        return TRUE;
    }

    if (CMainWnd::wndRgn != nullptr) {
        if (!rgn.CreateRectRgnIndirect(&rt)) {
            return TRUE;
        }

        CMainWnd::wndRgn->CombineRgn(CMainWnd::wndRgn, &rgn, RGN_OR);
    } else {
        CMainWnd::wndRgn = new CRgn();

        if (!CMainWnd::wndRgn->CreateRectRgnIndirect(&rt)) {
            return TRUE;
        }
    }
    return TRUE;
}


static HWND foundWnd;
static BOOL CALLBACK FindWndWithClass(HWND hwnd, LPARAM lParam)
{
    TCHAR* className = (TCHAR*)lParam;
    TCHAR thisClass[256];

    GetClassName(hwnd, thisClass, sizeof(thisClass));

    foundWnd = nullptr;
    if (!_tcscmp(className, thisClass)) {
        foundWnd = hwnd;
        return FALSE;
    }
    return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CMainWnd

CWnd CMainWnd::dskWnd;  // desktop window that is drawn on

// The region that covers all top level windows
CRgn* CMainWnd::wndRgn = nullptr;

// alpha blending level (0-255)
unsigned char CMainWnd::blendLevel = 255;

int CMainWnd::soundEnabled = 0;

int CMainWnd::santaPercent = 0;

//MPA 4-3-2005: soundFilename support
CString CMainWnd::soundFilename = "";


CMainWnd::CMainWnd()
{
    activeDlg = 0;
    m_numPenguins = 0;
    m_moveDelay = 0;
    m_splatDist = 0;
    trayIcon = 0;

    // Check if another instance of WinPenguins is already running
    hInstanceMutex = ::CreateMutex(nullptr, FALSE, L"WinPenguinsInstanceMutex");
    if (nullptr == hInstanceMutex) {
        MessageBox(L"Unable to create instance mutex", L"Internal Error", MB_ICONERROR);
        ::ExitProcess(1);
    }
    if (::GetLastError() == ERROR_ALREADY_EXISTS) {
        ::ExitProcess(0);   // Exit quitely if the mutex already exists
    }

    // Find the desktop window
    //   this is kindof kludgy and I'm not sure if it'll work in all cases!
    CWnd* progMan = CWnd::FindWindow(L"Progman", L"Program Manager");

    ASSERT(progMan != nullptr);
    EnumChildWindows(*progMan, FindWndWithClass, (LPARAM)L"SHELLDLL_DefView");
    ASSERT(foundWnd != nullptr);
    EnumChildWindows(foundWnd, FindWndWithClass, (LPARAM)L"SysListView32");
    ASSERT(foundWnd != nullptr);

    dskWnd.Attach(foundWnd);


    transparentblt = nullptr;
    alphablend = nullptr;

    msimg32 = ::LoadLibrary(L"msimg32.dll");
    if (msimg32 != nullptr) {
        (FARPROC&)transparentblt = GetProcAddress(msimg32, "TransparentBlt");
        (FARPROC&)alphablend = GetProcAddress(msimg32, "AlphaBlend");
    }


    // Load the Winmon DLL. First just try to load it (ie. if it is already
    // in the path).  If that fails, then load the copy that is in the EXE
    // resources.
    //
    // The reason this is done is so that people can simply distribute the
    // winpenguins executable without worrying about the additional DLL

    winmonFileName[0] = '\0';
    //winmon = ::LoadLibrary(L"Winmon.dll");
    winmon = nullptr;
    if (nullptr == winmon) {
        HRSRC hRes;
        HGLOBAL hGlobal;
        LPVOID winmonPtr;
        DWORD winmonSize;
        DWORD bytesWritten;

        // find winmon.dll in the exe resources
        hRes = ::FindResource(nullptr, MAKEINTRESOURCE(IDR_WINMONDLL), L"Binary");
        hGlobal = ::LoadResource(nullptr, hRes);

        winmonPtr = ::LockResource(hGlobal);
        if (nullptr == winmonPtr) {
            MessageBox(L"Unable to load 'Winmon.dll'", L"Error", MB_ICONERROR);
            ExitProcess(0);
        }
        winmonSize = ::SizeofResource(nullptr, hRes);

        GetTempPath(MAX_PATH, winmonFileName);
        _tcsncat_s(winmonFileName, L"Winmon.dll", MAX_PATH);

        // write Winmon.dll and load it
        HANDLE hFile = CreateFile(winmonFileName, GENERIC_WRITE, 0, nullptr,
                                  CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        WriteFile(hFile, winmonPtr, winmonSize, &bytesWritten, nullptr);
        CloseHandle(hFile);
        winmon = ::LoadLibrary(winmonFileName);

        if (nullptr == winmon) {
            MessageBox(L"Unable to load 'Winmon.dll'", L"Error", MB_ICONERROR);
            ::ExitProcess(0);
        }
    }
    LoadWinmonFunctions(winmon);

    // Load the penguin bitmap resources
    for (int i = 0; i < PENGUIN_COUNT; i++) {
        penguin_data[i].bmp = new CBitmap();
        penguin_data[i].bmp->LoadBitmap(MAKEINTRESOURCE(penguin_data[i].resId));
        penguin_data[i].mskBmp = new CBitmap();
        penguin_data[i].mskBmp->LoadBitmap(MAKEINTRESOURCE(penguin_data[i].mskResId));
    }

    Create(nullptr, L"MainWnd", WS_OVERLAPPEDWINDOW);

    // Monitor window activity...
    Winmon_LoadHook(GetCurrentProcessId(), dskWnd.m_hWnd);
}


CMainWnd::~CMainWnd()
{
    (void)dskWnd.Detach();

    Winmon_UnloadHook();
    ::SendMessageTimeout(HWND_BROADCAST, WM_NULL, 0, 0, SMTO_ABORTIFHUNG | SMTO_NOTIMEOUTIFNOTHUNG, 1000, nullptr);
    ::FreeLibrary(winmon);
    ::DeleteFile(winmonFileName);

    // Kill all the penguins (remaining glitches) with nuke. (Refresh desktop) Hacky a bit.
    ::SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);

    if (msimg32 != nullptr) {
        ::FreeLibrary(msimg32);
    }

    ::CloseHandle(hInstanceMutex);
}



BEGIN_MESSAGE_MAP(CMainWnd, CWnd)
    //{{AFX_MSG_MAP(CMainWnd)
    ON_WM_CREATE()
    ON_MESSAGE(UWM_SYSTRAY, OnSysTray)
    ON_WM_CLOSE()
    ON_WM_TIMER()
    ON_COMMAND(ID_ABOUT, OnAbout)
    ON_COMMAND(ID_EXIT, OnExit)
    ON_COMMAND(ID_OPTIONS, OnOptions)
    ON_COMMAND(ID_SCREENCAP, OnScreenCapture)
    ON_WM_LBUTTONDBLCLK()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMainWnd message handlers

int CMainWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1) {
        return -1;
    }

    // Load config options from registry
    m_numPenguins = theApp.GetProfileInt(L"Options", L"PenguinCount", 5);
    m_moveDelay = theApp.GetProfileInt(L"Options", L"MoveDelay", 50);
    m_splatDist = theApp.GetProfileInt(L"Options", L"SplatDistance", 2000);
    blendLevel = theApp.GetProfileInt(L"Options", L"BlendLevel", 255);
    santaPercent = theApp.GetProfileInt(L"Options", L"SantaPercent", 0);
    soundEnabled = theApp.GetProfileInt(L"Options", L"SoundEnabled", 0);
    soundFilename = theApp.GetProfileString(L"Options", L"SoundFilename", 0); //MPA 4-3-2005

    SetToonCountTo(m_numPenguins);

    // Create the systray icon
    NOTIFYICONDATA ni;

    ni.cbSize = sizeof(ni);
    ni.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    ni.hWnd = m_hWnd;
    ni.uID = 0;
    ni.uCallbackMessage = UWM_SYSTRAY;
    _tcscpy_s(ni.szTip, L"WinPenguins");
    trayIcon = theApp.LoadIcon(IDR_ICON);
    ni.hIcon = trayIcon;

    Shell_NotifyIcon(NIM_ADD, &ni);

    // Movement timer
    SetTimer(0, m_moveDelay, 0);

    return 0;
}


LRESULT CMainWnd::OnSysTray(WPARAM /*wParam*/, LPARAM lParam)
{
    switch (lParam) {
        case WM_LBUTTONUP:
            if (activeDlg != 0) {
                activeDlg->SetForegroundWindow();
                break;
            }

            OnOptions();
            break;

        case WM_RBUTTONDOWN:
            if (activeDlg != 0) {
                activeDlg->SetForegroundWindow();
                break;
            }

            {
                POINT pt;
                ::GetCursorPos(&pt);

                CMenu menu;
                CMenu* sysMenu;

                menu.LoadMenu(MAKEINTRESOURCE(IDR_SYSTRAY));

                sysMenu = menu.GetSubMenu(0);
                sysMenu->SetDefaultItem(1, TRUE);
                sysMenu->TrackPopupMenu(TPM_RIGHTALIGN | TPM_RIGHTBUTTON,
                                        pt.x, pt.y, this, nullptr);
            }
            break;

        case WM_LBUTTONDBLCLK:
            if (activeDlg != 0) {
                activeDlg->SetForegroundWindow();
                break;
            }

            PostMessage(WM_COMMAND, ID_OPTIONS, 0);
            break;

        default:
            break;
    }
    return 0;
}


void CMainWnd::OnClose()
{
    if (m_numPenguins > 0 || toonList.GetSize() > 0) {
        m_numPenguins = 0;
        SetToonCountTo(m_numPenguins);
        return;
    }

    CWnd::OnClose();
}


void CMainWnd::OnTimer(UINT_PTR nIDEvent)
{
    switch (nIDEvent) {
        case 0: {
            // Update the WindowRegion if any windows have moved...
            UpdateWndRgn();

            // recreate the screen bitmaps when the desktop window is
            // changed, ie. resizing the taskbar
            if (Winmon_DesktopChanged()) {
                CreateScreenBitmaps();
            }

            // desktop window has been painted on, flush the penguins
            // and recapture the bitmap
            RECT dskRt;
            if (Winmon_DeskWndPainted(&dskRt)) {
                // Clear all the penguins
                for (int i = 0; i < toonList.GetSize(); i++) {
                    toonList[i]->Erase(dskWnd, &dskRt);
                }
                ::SendMessage(dskWnd, WM_PAINT, 0, 0);

                UpdateBgBitmap(&dskRt);

                // Ignore the WM_PAINT message we just sent to
                // the desktop window
                (void)Winmon_DeskWndPainted(nullptr);
            }

            // Do any required processing...
            for (int i = 0; i < toonList.GetSize(); i++) {
                int status;
                bool startingAndBlocked = false;

                // check if the toon is inside a window...
                if (toonList[i]->IsBlocked(TOON_HERE)) {
                    if (!toonList[i]->m_startingUp) {
                        toonList[i]->ExplodeAni();
                    } else {
                        startingAndBlocked = true;
                    }
                } else {
                    toonList[i]->m_startingUp = false;
                }


                status = toonList[i]->AdvanceToon(startingAndBlocked);

                switch (toonList[i]->m_bmpIndex) {
                    case PENGUIN_FALLER:
                        if (startingAndBlocked) {
                            break;
                        }

                        if (status != TOON_OK) {
                            if (toonList[i]->IsBlocked(TOON_DOWN)) {
                                int direction;

                                if (toonList[i]->m_prefd > -1) {
                                    direction = toonList[i]->m_prefd;
                                } else {
                                    direction = rand() % 2;
                                }

                                CheckSubType(toonList[i]);

                                if (toonList[i]->GetSubType() == TST_NORMAL_PENGUIN) {
                                    toonList[i]->SetType(PENGUIN_WALKER, direction);

                                } else if (toonList[i]->GetSubType() == TST_SANTA_PENGUIN) {
                                    toonList[i]->SetType(PENGUIN_SANTA_WALKER, direction);

                                } else  {
                                    // this shouldn't happen, but all the same...
                                    toonList[i]->SetType(PENGUIN_WALKER, direction);
                                }

                                toonList[i]->SetVelocity(4 * ((2 * direction) - 1), 0);
                                toonList[i]->m_prefd = -1;
                            } else {
                                if (rand() % 10 > 5) {
                                    toonList[i]->SetVelocity(-toonList[i]->m_u, 3);
                                }

                                CheckSubType(toonList[i]);

                                if (toonList[i]->GetSubType() == TST_NORMAL_PENGUIN) {
                                    toonList[i]->SetType(PENGUIN_CLIMBER, toonList[i]->m_u > 0);
                                } else if (toonList[i]->GetSubType() == TST_SANTA_PENGUIN) {
                                    toonList[i]->SetType(PENGUIN_SANTA_CLIMBER, toonList[i]->m_u > 0);
                                } else {
                                    toonList[i]->SetType(PENGUIN_CLIMBER, toonList[i]->m_u > 0);
                                }

                                toonList[i]->SetVelocity(0, -4);
                            }
                        }
                        break;
                    case PENGUIN_TUMBLER:
                        if (status != TOON_OK) {
                            int direction;

                            if (toonList[i]->m_prefd > -1) {
                                direction = toonList[i]->m_prefd;
                            } else {
                                direction = rand() % 2;
                            }

                            CheckSubType(toonList[i]);

                            if (toonList[i]->GetSubType() == TST_NORMAL_PENGUIN) {
                                toonList[i]->SetType(PENGUIN_WALKER, direction);
                            } else if (toonList[i]->GetSubType() == TST_SANTA_PENGUIN) {
                                toonList[i]->SetType(PENGUIN_SANTA_WALKER, direction);
                            } else { // this shouldn't happen, but all the same...
                                toonList[i]->SetType(PENGUIN_WALKER, direction);
                            }

                            toonList[i]->SetVelocity(4 * ((2 * direction) - 1), 0);
                            toonList[i]->m_prefd = -1;

                            if (toonList[i]->m_y - toonList[i]->m_tumbleStartY > m_splatDist) {
                                toonList[i]->ExplodeAni();
                            }

                        }
                        break;
                    case PENGUIN_WALKER:
                    case PENGUIN_SANTA_WALKER:
                        if (!toonList[i]->IsBlocked(TOON_DOWN)) {
                            toonList[i]->m_prefd = toonList[i]->m_directionIndex;
                            toonList[i]->SetType(PENGUIN_TUMBLER, PENGUIN_FOREWARD);
                            toonList[i]->SetVelocity(0, 6);
                            toonList[i]->m_tumbleStartY = toonList[i]->m_y;
                        } else if (status != TOON_OK) {
                            /* Blocked!  We can turn around, fly or climb */
                            switch (rand() % 8) {
                                case 0:
                                case 1:
                                    toonList[i]->SetType(PENGUIN_FLOATER, PENGUIN_FOREWARD);
                                    toonList[i]->SetVelocity(rand() % 5 * (-toonList[i]->m_u / 4), -3);
                                    break;
                                case 2:
                                case 3:

                                    CheckSubType(toonList[i]);

                                    if (toonList[i]->GetSubType() == TST_NORMAL_PENGUIN) {
                                        toonList[i]->SetType(PENGUIN_CLIMBER, toonList[i]->m_directionIndex);
                                    } else if (toonList[i]->GetSubType() == TST_SANTA_PENGUIN) {
                                        toonList[i]->SetType(PENGUIN_SANTA_CLIMBER, toonList[i]->m_directionIndex);
                                    } else {
                                        toonList[i]->SetType(PENGUIN_CLIMBER, toonList[i]->m_directionIndex);
                                    }

                                    toonList[i]->SetVelocity(0, -4);
                                    break;
                                default:

                                    CheckSubType(toonList[i]);

                                    if (toonList[i]->GetSubType() == TST_NORMAL_PENGUIN) {
                                        toonList[i]->SetType(PENGUIN_WALKER, !toonList[i]->m_directionIndex);
                                    } else if (toonList[i]->GetSubType() == TST_SANTA_PENGUIN) {
                                        toonList[i]->SetType(PENGUIN_SANTA_WALKER, !toonList[i]->m_directionIndex);
                                    }

                                    toonList[i]->SetVelocity(-toonList[i]->m_u, 0);

                                    break;
                            }
                        }
                        break;
                    case PENGUIN_CLIMBER:
                    case PENGUIN_SANTA_CLIMBER:
                        if (!toonList[i]->IsBlocked(toonList[i]->m_directionIndex)) {

                            CheckSubType(toonList[i]);

                            if (toonList[i]->GetSubType() == TST_NORMAL_PENGUIN) {
                                toonList[i]->SetType(PENGUIN_WALKER, toonList[i]->m_directionIndex);
                            } else if (toonList[i]->GetSubType() == TST_SANTA_PENGUIN) {
                                toonList[i]->SetType(PENGUIN_SANTA_WALKER, toonList[i]->m_directionIndex);
                            } else { // again, should never happen... but...
                                toonList[i]->SetType(PENGUIN_WALKER, toonList[i]->m_directionIndex);
                            }

                            toonList[i]->SetVelocity(4 * ((2 * toonList[i]->m_directionIndex) - 1), 0);
                            toonList[i]->SetPosition(toonList[i]->m_x +
                                                     (2 * toonList[i]->m_directionIndex) - 1,
                                                     toonList[i]->m_y);
                            toonList[i]->m_prefd = toonList[i]->m_directionIndex;
                        } else if (status != TOON_OK) {
                            toonList[i]->SetType(PENGUIN_FALLER, PENGUIN_FOREWARD);
                            toonList[i]->SetVelocity(1 - toonList[i]->m_directionIndex * 2, 3);
                        }
                        break;
                    case PENGUIN_FLOATER:
                        if (status != TOON_OK) {
                            if (toonList[i]->IsBlocked(TOON_UP)) {
                                toonList[i]->SetType(PENGUIN_FALLER, PENGUIN_FOREWARD);
                                toonList[i]->SetVelocity(((toonList[i]->m_u > 0) * 2 - 1), 3);
                            } else {
                                toonList[i]->SetVelocity(-toonList[i]->m_u, -3);
                            }
                        }
                        break;
                    default:
                        break;
                }

            }

            CDC* dc = dskWnd.GetDC();
            CDC bgBitmapDC, activeBmpDC, tmpDC;

            bgBitmapDC.CreateCompatibleDC(dc);
            activeBmpDC.CreateCompatibleDC(dc);
            tmpDC.CreateCompatibleDC(dc);

            CBitmap* oldBgBmp, *oldActiveBmp;

            oldBgBmp = bgBitmapDC.SelectObject(&bgBitmap);
            oldActiveBmp = activeBmpDC.SelectObject(&activeBmp);

            for (int i = 0; i < toonList.GetSize(); i++) {
                toonList[i]->PaintBackground(&bgBitmapDC, &activeBmpDC);
            }

            for (int i = 0; i < toonList.GetSize(); i++) {
                toonList[i]->Paint(&activeBmpDC, &tmpDC);
            }

            BOOL deadToons = FALSE;
            // Finally output to the desktop window
            for (int i = 0; i < toonList.GetSize(); i++) {
                if (!toonList[i]->m_active) {
                    deadToons = TRUE;
                }

                toonList[i]->PaintToDesktop(dc, &activeBmpDC);
            }

            bgBitmapDC.SelectObject(oldBgBmp);
            activeBmpDC.SelectObject(oldActiveBmp);

            tmpDC.DeleteDC();

            if (deadToons) {
                for (int i = (int)toonList.GetSize() - 1; i >= 0; i--) {
                    if (!toonList[i]->m_active) {
                        toonList.RemoveAt(i);
                    }
                }

            }

            dskWnd.ReleaseDC(dc);
            SetToonCountTo(m_numPenguins);
        }

        break;

        default:
            break;
    }

    CWnd::OnTimer(nIDEvent);
}


void CMainWnd::OnAbout()
{
    CAboutDlg dlg;

    activeDlg = &dlg;

    (void) dlg.DoModal();

    activeDlg = 0;
}

// <TL> Thought some people might find this nice...

void CMainWnd::OnScreenCapture()
{
    CFileDialog dlg(FALSE, L"BMP", nullptr, OFN_OVERWRITEPROMPT, L"Bitmap Files (*.bmp)|*.bmp||", this);

    activeDlg = &dlg;

    if (dlg.DoModal() == IDOK) {
        RECTCAPINFO rci;

        CMainWnd::OnTimer(0);   // step forward a frame to avoid messing
        // up our pretty scenery

        ResetCaptureInfo(&rci, FALSE);

        FullScreenCapture(&rci);
        SaveCaptureToFile(&rci, LPCTSTR(dlg.GetPathName()));
        ResetCaptureInfo(&rci, TRUE);
    }

    activeDlg = 0;
}

// </TL>

void CMainWnd::OnExit()
{
    // Remove the systray icon
    NOTIFYICONDATA ni;

    ni.cbSize = sizeof(ni);
    ni.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    ni.uID = 0;
    ni.hWnd = m_hWnd;
    ni.uCallbackMessage = UWM_SYSTRAY;
    _tcscpy_s(ni.szTip, L"WinPenguins");
    ni.hIcon = trayIcon;

    Shell_NotifyIcon(NIM_DELETE, &ni);

    PostMessage(WM_CLOSE, 0, 0);
}


void CMainWnd::OnOptions()
{

    CWinpenguinsDlg optiondlg;

    activeDlg = &optiondlg;

    optiondlg.m_pcount = m_numPenguins;
    optiondlg.m_delay = MAX_MOVE_DELAY - m_moveDelay;
    optiondlg.m_splat = m_splatDist;
    optiondlg.m_alpha = blendLevel;
    optiondlg.m_santa = santaPercent;
    optiondlg.m_soundenabled = soundEnabled;
    optiondlg.m_soundfilename = soundFilename;//MPA 4-3-2005

    if (optiondlg.DoModal() == IDOK)    {
        ApplyOptions(&optiondlg);

        theApp.WriteProfileInt(L"Options", L"PenguinCount", m_numPenguins);
        theApp.WriteProfileInt(L"Options", L"MoveDelay", m_moveDelay);
        theApp.WriteProfileInt(L"Options", L"SplatDistance", m_splatDist);
        theApp.WriteProfileInt(L"Options", L"BlendLevel", blendLevel);
        theApp.WriteProfileInt(L"Options", L"SantaPercent", santaPercent);
        theApp.WriteProfileInt(L"Options", L"SoundEnabled", soundEnabled);
        theApp.WriteProfileString(L"Options", L"SoundFilename", soundFilename);//MPA 4-3-2005
    } else {
        /* restore original values  */
        m_numPenguins = theApp.GetProfileInt(L"Options", L"PenguinCount", 5);
        m_moveDelay = theApp.GetProfileInt(L"Options", L"MoveDelay", 50);
        m_splatDist = theApp.GetProfileInt(L"Options", L"SplatDistance", 2000);
        blendLevel = theApp.GetProfileInt(L"Options", L"BlendLevel", 255);
        santaPercent = theApp.GetProfileInt(L"Options", L"SantaPercent", 0);
        soundEnabled = theApp.GetProfileInt(L"Options", L"SoundEnabled", 0);
        soundFilename = theApp.GetProfileString(L"Options", L"SoundFilename", L"");//MPA 4-3-2005

        optiondlg.m_pcount = m_numPenguins;
        optiondlg.m_delay = MAX_MOVE_DELAY - m_moveDelay;
        optiondlg.m_splat = m_splatDist;
        optiondlg.m_alpha = blendLevel;
        optiondlg.m_santa = santaPercent;
        optiondlg.m_soundenabled = soundEnabled;
        optiondlg.m_soundfilename = soundFilename;//MPA 4-3-2005

        ApplyOptions(&optiondlg);
    }

    activeDlg = 0;
}


void CMainWnd::SetToonCountTo(int count)
{
    for (int i = count - (int)toonList.GetSize(); i > 0; i--) {
        CToon* toon = new CToon();
        toonList.Add(toon);
    }

    if (count < toonList.GetSize()) {
        for (int i = count; i < toonList.GetSize(); i++) {
            toonList[i]->DeleteAni();
        }
    }

    if (toonList.GetSize() == 0) {
        PostMessage(WM_CLOSE, 0, 0);
    }
}


//MPA 4-3-2005: sound filename support
//Browse dialog for getting filename
//
//Used code from
//http://www.mindcracker.com/mindcracker/c_cafe/mfc/filedlg.asp
void CMainWnd::BrowseSoundFilename(CWinpenguinsDlg* const dlg)
{
    CFileDialog fileDlg(TRUE, nullptr, nullptr, OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY, L"Wave Files (*.wav)|*.wav||", dlg);
    fileDlg.m_ofn.lpstrTitle = L"Select Boom-Splat Sound File";
    if (fileDlg.DoModal() == IDOK) {
        dlg->m_soundfilename = fileDlg.GetPathName();
    }
}

void CMainWnd::ApplyOptions(CWinpenguinsDlg* const dlg)
{
    m_numPenguins = dlg->m_pcount;
    m_moveDelay = MAX_MOVE_DELAY - dlg->m_delay;
    m_splatDist = dlg->m_splat;
    blendLevel = dlg->m_alpha;
    soundEnabled = dlg->m_soundenabled;
    santaPercent = dlg->m_santa;
    soundFilename = dlg->m_soundfilename;

    // Movement timer
    KillTimer(0);
    SetTimer(0, m_moveDelay, 0);

    SetToonCountTo(m_numPenguins);
}



void CMainWnd::UpdateWndRgn()
{
    if (Winmon_Moved()) {
        delete wndRgn;
        wndRgn = nullptr;

        // First find all the top level windows
        ::EnumWindows(EnumWindowCallback, 0);
    }
}



void CMainWnd::UpdateBgBitmap(RECT* updateRect)
{
    CDC* dc = dskWnd.GetDC();
    CDC bmpDc;
    CBitmap* oldBmp;

    bmpDc.CreateCompatibleDC(dc);
    oldBmp = bmpDc.SelectObject(&bgBitmap);


    bmpDc.BitBlt(updateRect->left, updateRect->top,
                 updateRect->right - updateRect->left,
                 updateRect->bottom - updateRect->top,
                 dc, updateRect->left, updateRect->top, SRCCOPY);
    /*
        RECT rt;
        ::GetClientRect(dskWnd, &rt);
        bmpDc.BitBlt(0, 0, rt.right - rt.left, rt.bottom - rt.top, dc, 0, 0, SRCCOPY);
    */

    bmpDc.SelectObject(oldBmp);

    dskWnd.ReleaseDC(dc);
}





void CMainWnd::CreateScreenBitmaps()
{
    RECT rt;
    CDC* dc = dskWnd.GetDC();
    CDC bmpDc;

    ::GetWindowRect(dskWnd, &rt);

    bgBitmap.DeleteObject();
    bgBitmap.CreateCompatibleBitmap(dc, rt.right - rt.left, rt.bottom - rt.top);

    activeBmp.DeleteObject();
    activeBmp.CreateCompatibleBitmap(dc, rt.right - rt.left, rt.bottom - rt.top);

    dskWnd.ReleaseDC(dc);
}


bool CMainWnd::CheckSubType(CToon* pToon)
{
    if (pToon->GetSubType() == TST_UNSPECIFIED) {
        if ((rand() % 100) > santaPercent) {
            pToon->SetSubType(TST_NORMAL_PENGUIN);
        }   else {
            pToon->SetSubType(TST_SANTA_PENGUIN);
        }

        return true;
    }

    return false;
}
