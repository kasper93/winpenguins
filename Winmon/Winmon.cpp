/**
 * $Id: Winmon.cpp,v 1.1.1.1 2001/04/03 19:29:43 mvines Exp $
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
#include "winmon.h"
#include <assert.h>
#include <stdio.h>

HINSTANCE hMod;
HANDLE hSem;

BOOL APIENTRY DllMain( HINSTANCE hModule,
					   DWORD  ul_reason_for_call,
					   LPVOID lpReserved
					 )
{
	hMod = hModule;
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			hSem = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE,
								 "WinPenguins-DeskPaintSem");
			if (hSem == NULL) {
				hSem = CreateSemaphore(NULL, 1, 1,
									   "WinPenguins-DeskPaintSem");
				assert(hSem != NULL);
			}
			break;

		case DLL_PROCESS_DETACH:
			DeleteObject(hSem);
			break;

		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;

	}
	return TRUE;
}


// Data in the sdata segment is shared between all winmon instances
#pragma bss_seg("sdata")
#pragma data_seg("sdata")

BOOL wndPosInvalid;
BOOL dskWndPainted;
BOOL dskWndResized;
RECT dskWndPaintedRect;

HHOOK hWndMsgHook, hWndRetHook;
DWORD pidToIgnore;
HWND desktopWnd;

#pragma data_seg(".data")
#pragma bss_seg(".bss")


LRESULT CALLBACK SysMsgHook(int nCode, WPARAM wParam, LPARAM lParam)
{
	MSG *cw = (MSG*)lParam;

	switch (cw->message) {
		case WM_ERASEBKGND:
		case WM_PAINT:
			if (cw->hwnd == desktopWnd) {
				RECT rt;

				if (!GetUpdateRect(cw->hwnd, &rt, false)) {
					::GetClientRect(cw->hwnd, &rt);
				}

				WaitForSingleObject(hSem, INFINITE);
				UnionRect(&dskWndPaintedRect, &dskWndPaintedRect, &rt);
				ReleaseSemaphore(hSem, 1, NULL);

				dskWndPainted = TRUE;
			}
			break;
		default:
			break;
	}

	return CallNextHookEx(hWndMsgHook, nCode, wParam, lParam);
}


LRESULT CALLBACK SysMsgRetHook(int nCode, WPARAM wParam, LPARAM lParam)
{
	DWORD pid;
	CWPRETSTRUCT *cw = (CWPRETSTRUCT*)lParam;

	GetWindowThreadProcessId(cw->hwnd, &pid);

	if (pid != pidToIgnore) {
		switch (cw->message) {
			case WM_WINDOWPOSCHANGED:
				if (cw->hwnd == desktopWnd) {
					dskWndResized = TRUE;
				}

				wndPosInvalid = TRUE;
				break;
			default:
				break;
		}
	}

	return CallNextHookEx(hWndRetHook, nCode, wParam, lParam);
}


WINMON_API void Winmon_LoadHook(DWORD myPid, HWND dskWnd)
{
	wndPosInvalid = TRUE;
	dskWndPainted = TRUE;
	dskWndResized = TRUE;

	pidToIgnore = myPid;
	desktopWnd = dskWnd;

	GetClientRect(dskWnd, &dskWndPaintedRect);

	hWndMsgHook = SetWindowsHookEx(WH_GETMESSAGE, SysMsgHook, hMod, 0);
	hWndRetHook = SetWindowsHookEx(WH_CALLWNDPROCRET, SysMsgRetHook, hMod, 0);
}



WINMON_API BOOL Winmon_Moved(void)
{
	BOOL ret = wndPosInvalid;
	wndPosInvalid = FALSE;

	return ret;
}


WINMON_API BOOL Winmon_DeskWndPainted(RECT *dskRt)
{
	BOOL ret = dskWndPainted;
	dskWndPainted = FALSE;

	if (dskRt != NULL) {
		WaitForSingleObject(hSem, INFINITE);
		CopyRect(dskRt, &dskWndPaintedRect);
		SetRectEmpty(&dskWndPaintedRect);
		ReleaseSemaphore(hSem, 1, NULL);
	}

	return ret;
}


WINMON_API BOOL Winmon_DesktopChanged(void)
{
	BOOL ret = dskWndResized;
	dskWndResized = FALSE;

	return ret;
}



WINMON_API void Winmon_UnloadHook(void)
{
	UnhookWindowsHookEx(hWndMsgHook);
	UnhookWindowsHookEx(hWndRetHook);
}


