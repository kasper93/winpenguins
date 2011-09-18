/**
 * $Id: screen_capture.h,v 1.2 2001/04/08 08:20:29 tlee1 Exp $
 *
 *  Copyright (C) 2000  Tom Lee
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

#ifndef _SCREEN_CAPTURE_H_INCLUDED_
#define _SCREEN_CAPTURE_H_INCLUDED_

#include <windows.h>
#include <memory.h>

#ifdef __cplusplus
extern "C"
{
#endif

	typedef struct _tagRECTCAPINFO
	{
		HDC hDisplayDC;
		HDC hImageDC;
		HANDLE hImage;
		int nCapX;
		int nCapY;
		int nCapWidth;
		int nCapHeight;
		DWORD dwBPP;
		DWORD dwNumColors;
		LPVOID pBits;

	} RECTCAPINFO, FAR *LPRECTCAPINFO;

	// low level functions
	int __cdecl CreateDisplayDC( LPRECTCAPINFO lpCapInfo );
	void __cdecl AdjustCaptureRect( LPRECTCAPINFO lpCapInfo, int x, int y, int nWidth, int nHeight );
	int __cdecl CreateSuitableDIB( LPRECTCAPINFO lpCapInfo );
	int __cdecl PerformBitBlockTransfer( LPRECTCAPINFO lpCapInfo );

	// high level functions
	int __cdecl FullScreenCapture( LPRECTCAPINFO lpCapInfo );
	int __cdecl CaptureScreenRect( LPRECTCAPINFO lpCapInfo, LPRECT lpBounds );
	int __cdecl CaptureWindow( LPRECTCAPINFO lpCapInfo, HWND hWnd );

	// cleanup functions
	int __cdecl ResetCaptureInfo( LPRECTCAPINFO lpCapInfo, BOOL bCheckMem );

	// file i/o functions
	int __cdecl SaveCaptureToFile( LPRECTCAPINFO lpCapInfo, const char *pszFileName );

#ifdef __cplusplus
};
#endif

#endif
