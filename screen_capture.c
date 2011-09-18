/**
 * $Id: screen_capture.c,v 1.3 2001/04/08 15:53:07 mvines Exp $
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
#include "screen_capture.h"

////////////////////////////////////////////////////////////////
//
// + Method:      CreateDisplayDC
//
// + Description: Creates a DC associated with the Display
//
// + Params:      
//				  -> LPRECTCAPINFO lpCapInfo: A reset 
//					 capture info structure
//
// + Return Type: int
//
// + Returns:     
//				  -> -1: Display DC has already been created in
//						 this LPRECTCAPINFO
//				  -> -2: Unable to create new Display DC
//				  -> 0:  Display DC successfully created
//
// + Change Log:  
//
//				-> 3/28/01: Method created
//
////////////////////////////////////////////////////////////////

int __cdecl CreateDisplayDC( LPRECTCAPINFO lpCapInfo )
{
	if( lpCapInfo->hDisplayDC != NULL )
	{
		return -1;
	}

	lpCapInfo->hDisplayDC = CreateDC( "DISPLAY", NULL, NULL, NULL );

	if( lpCapInfo->hDisplayDC == NULL )
	{
		return -2;
	}

	lpCapInfo->dwBPP = GetDeviceCaps( lpCapInfo->hDisplayDC, BITSPIXEL );

	if( lpCapInfo->dwBPP <= 8 )
	{
		lpCapInfo->dwNumColors = GetDeviceCaps( lpCapInfo->hDisplayDC, NUMCOLORS );
	}
	else
	{
		lpCapInfo->dwNumColors = 0;
	}

	return 0;
}

////////////////////////////////////////////////////////////////
//
// + Method:      AdjustCaptureRect
//
// + Description: Adjusts the rectangle to capture
//
// + Params:      
//				  -> LPRECTCAPINFO lpCapInfo: The LPRECTCAPINFO
//					 whose coordinates are to be adjusted
//				  -> int x, y, nWidth, nHeight: Position & size
//					 of the capture rectangle
//
// + Return Type: void
//
// + Returns:     None
//
// + Change Log:  
//
//				-> 3/28/01: Method created
//
////////////////////////////////////////////////////////////////

void __cdecl AdjustCaptureRect( LPRECTCAPINFO lpCapInfo, int x, int y, int nWidth, int nHeight )
{
	lpCapInfo->nCapX = x;
	lpCapInfo->nCapY = y;
	lpCapInfo->nCapWidth = nWidth;
	lpCapInfo->nCapHeight = nHeight;
}

////////////////////////////////////////////////////////////////
//
// + Method:      CreateSuitableDIB
//
// + Description: Creates a DIB and a DC to be associated with it.
//				  These are used together to save the copy of the
//				  screen to memory.
//
// + Params:      
//				  -> LPRECTCAPINFO lpCapInfo: Capture info that's
//					 had CreateDisplayDC and AdjustCaptureRect
//					 perform the necessary operations.
//
// + Return Type: int
//
// + Returns:     
//				  -1: hImage or hImageDC has already been created
//				  -2: Unable to create the DIB
//				  -3: Unable to create the DIB's DC
//				  0: Created both the new DIB and DC
//
// + Change Log:  
//
//				-> 3/28/01: Method created
//
////////////////////////////////////////////////////////////////

int __cdecl CreateSuitableDIB( LPRECTCAPINFO lpCapInfo )
{
	BITMAPINFO bi;
	BITMAPINFOHEADER *hdr;
	
	if( lpCapInfo->hImage != 0 || lpCapInfo->hImageDC != 0 )
	{
		return -1;
	}

	hdr = &bi.bmiHeader;

	hdr->biSize				= sizeof( BITMAPINFOHEADER );
	hdr->biBitCount			= (WORD)lpCapInfo->dwBPP;
	hdr->biClrImportant		= lpCapInfo->dwNumColors;
	hdr->biClrUsed			= lpCapInfo->dwNumColors;
	hdr->biCompression		= BI_RGB;
	hdr->biPlanes			= 1;
	hdr->biWidth			= lpCapInfo->nCapWidth;
	hdr->biHeight			= lpCapInfo->nCapHeight;
	hdr->biXPelsPerMeter	= 0;
	hdr->biYPelsPerMeter	= 0;
	hdr->biSizeImage		= 0;

	lpCapInfo->hImage = CreateDIBSection( lpCapInfo->hDisplayDC, &bi, DIB_PAL_COLORS, &lpCapInfo->pBits, NULL, 0 );

	if( lpCapInfo->hImage == NULL )
	{
		return -2;
	}

	lpCapInfo->hImageDC = CreateCompatibleDC( lpCapInfo->hDisplayDC );

	if( lpCapInfo->hImageDC == NULL )
	{
		DeleteObject( lpCapInfo->hImage );
		return -3;
	}
	else
	{
		SelectObject( lpCapInfo->hImageDC, lpCapInfo->hImage );
		return 0;
	}
}

////////////////////////////////////////////////////////////////
//
// + Method:      PerformBitBlockTransfer
//
// + Description: A fancy way of saying BitBlt :)
//
// + Params:      
//				  -> LPRECTCAPINFO lpCapInfo: A pointer to a 
//					 RECTCAPINFO structure that's had the hDisplayDC,
//					 hImage, hImageDC, x, y, nWidth, nHeight members set.
//
// + Return Type: int
//
// + Returns:     
//				  -> -1: hImageDC or hDisplayDC is NULL
//				  -> -2: BitBlt failed
//				  -> 0: Copied the image to hImage
//
// + Change Log:  
//
//				-> 3/28/01: Method created
//
////////////////////////////////////////////////////////////////

int __cdecl PerformBitBlockTransfer( LPRECTCAPINFO lpCapInfo )
{
	if( lpCapInfo->hImageDC == NULL || lpCapInfo->hDisplayDC == NULL )
	{
		return -1;
	}

	if( !BitBlt( lpCapInfo->hImageDC, 0, 0, lpCapInfo->nCapWidth, lpCapInfo->nCapHeight, lpCapInfo->hDisplayDC, lpCapInfo->nCapX, lpCapInfo->nCapY, SRCCOPY ) )
	{
		return -2;
	}
	else
	{
		return 0;
	}
}

////////////////////////////////////////////////////////////////
//
// + Method:      FullScreenCapture
//
// + Description: Captures the entire display and saves all
//				  recorded info into lpCapInfo
//
// + Params:      
//				  -> LPRECTCAPINFO lpCapInfo: receives info about
//					 the capture.
//
// + Return Type: int
//
// + Returns:     
//				  -> -1: Couldn't create display DC
//				  -> -2: Couldn't create suitable DIB
//				  -> -3: BitBlt failed
//				  -> 0: Screen sucessfully captured
//
// + Change Log:  
//
//				-> 3/28/01: Method created
//
////////////////////////////////////////////////////////////////

int __cdecl FullScreenCapture( LPRECTCAPINFO lpCapInfo )
{
	ResetCaptureInfo( lpCapInfo, FALSE );

	if( CreateDisplayDC( lpCapInfo ) != 0 )
	{
		return -1;
	}

	AdjustCaptureRect( lpCapInfo, 0, 0, GetDeviceCaps( lpCapInfo->hDisplayDC, HORZRES ), GetDeviceCaps( lpCapInfo->hDisplayDC, VERTRES ) );

	if( CreateSuitableDIB( lpCapInfo ) != 0 )
	{
		ResetCaptureInfo( lpCapInfo, TRUE );
		return -2;
	}

	if( PerformBitBlockTransfer( lpCapInfo ) != 0 )
	{
		ResetCaptureInfo( lpCapInfo, TRUE );
		return -3;
	}

	return 0;
}

////////////////////////////////////////////////////////////////
//
// + Method:      CaptureScreenRect
//
// + Description: Captures a portion of the screen
//
// + Params:      
//				  -> LPRECTCAPINFO lpCapInfo: Structure to receive
//					 capture information
//				  -> LPRECT lpBounds: Pointer to a rect containing
//					 the coordinates to capture
//
// + Return Type: int
//
// + Returns:     As for FullScreenCapture
//
// + Change Log:  
//
//				-> 3/28/01: Method created
//
////////////////////////////////////////////////////////////////

int __cdecl CaptureScreenRect( LPRECTCAPINFO lpCapInfo, LPRECT lpBounds )
{
	if( CreateDisplayDC( lpCapInfo ) != 0 )
	{
		return -1;
	}

	AdjustCaptureRect( lpCapInfo, lpBounds->left, lpBounds->top, lpBounds->right - lpBounds->left, lpBounds->bottom - lpBounds->top );

	if( CreateSuitableDIB( lpCapInfo ) != 0 )
	{
		ResetCaptureInfo( lpCapInfo, TRUE );
		return -2;
	}

	if( PerformBitBlockTransfer( lpCapInfo ) != 0 )
	{
		ResetCaptureInfo( lpCapInfo, TRUE );
		return -3;
	}

	return 0;
}

////////////////////////////////////////////////////////////////
//
// + Method:      CaptureWindow
//
// + Description: Captures a still image of a window
//
// + Params:      
//				  -> LPRECTCAPINFO lpCapInfo: structure to receive
//					 info about the capture operation
//				  -> HWND hWnd: Window to capture
//
// + Return Type: int
//
// + Returns:     As per CaptureScreenRect
//
// + Change Log:  
//
//				-> 3/28/01: Method created
//
////////////////////////////////////////////////////////////////

int __cdecl CaptureWindow( LPRECTCAPINFO lpCapInfo, HWND hWnd )
{
	RECT rc;

	GetWindowRect( hWnd, &rc );

	return CaptureScreenRect( lpCapInfo, &rc );
}

////////////////////////////////////////////////////////////////
//
// + Method:      ResetCaptureInfo
//
// + Description: Zeroes RECTCAPINFO structure and, if wished,
//				  frees memory associated with it.
//
// + Params:      
//				  -> LPRECTCAPINFO lpCapInfo: structure to reset
//				  -> BOOL bCheckMem: checks for allocated memory 
//					 within the structure and frees it as necessary
//
// + Return Type: int
//
// + Returns:     0 - always
//
// + Change Log:  
//
//				-> 3/28/01: Method created
//
////////////////////////////////////////////////////////////////

int __cdecl ResetCaptureInfo( LPRECTCAPINFO lpCapInfo, BOOL bCheckMem )
{
	if( bCheckMem )
	{
		if( lpCapInfo->hDisplayDC != NULL )
		{
			DeleteDC( lpCapInfo->hDisplayDC );
		}

		if( lpCapInfo->hImageDC != NULL )
		{
			DeleteDC( lpCapInfo->hImageDC );
		}

		if( lpCapInfo->hImage != NULL )
		{
			DeleteObject( lpCapInfo->hImage );
		}
	}

	memset( lpCapInfo, 0, sizeof( RECTCAPINFO ) );

	return 0;
}

////////////////////////////////////////////////////////////////
//
// + Method:      SaveCaptureToFile
//
// + Description: Saves a screen capture to disk
//
// + Params:      
//				  -> LPRECTCAPINFO lpCapInfo: structure containing
//					 information retrieved from a successful capture
//				  -> const char *pszFileName: Name of the file to save
//				     to. Be warned, this file will be OVERWRITTEN if
//					 it already exists!
//
// + Return Type: int
//
// + Returns:     
//				  -> -1: lpCapInfo doesn't contain enough info
//				  -> -2: Couldn't open pszFileName for writing
//				  -> 0: Saved capture to file!
//
// + Change Log:  
//
//				-> 3/28/01: Method created
//
////////////////////////////////////////////////////////////////

int __cdecl SaveCaptureToFile( LPRECTCAPINFO lpCapInfo, const char *pszFileName )
{
	HANDLE hFile; DWORD dwWriteSize;
	RGBQUAD clrs[ 256 ];
	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bih;

	if( lpCapInfo->hDisplayDC == NULL || lpCapInfo->hImageDC == NULL || lpCapInfo->hImage == NULL || lpCapInfo->pBits == NULL )
	{
		return -1;
	}

	hFile = CreateFile( pszFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

	if( hFile == INVALID_HANDLE_VALUE )
	{
		return -2;
	}

	if( lpCapInfo->dwNumColors != 0 )
	{
		lpCapInfo->dwNumColors = GetDIBColorTable( lpCapInfo->hImageDC, 0, lpCapInfo->dwNumColors, clrs );
	}

	bfh.bfType = 0x4D42;
	bfh.bfSize = ((lpCapInfo->nCapWidth * lpCapInfo->nCapHeight * lpCapInfo->dwBPP)/8 ) + sizeof( BITMAPFILEHEADER ) + sizeof( BITMAPINFOHEADER ) + (lpCapInfo->dwNumColors * sizeof( RGBQUAD ) );
	bfh.bfReserved1 = 0;
	bfh.bfReserved2 = 0;
	bfh.bfOffBits = sizeof( BITMAPFILEHEADER ) + sizeof( BITMAPINFOHEADER ) + ( lpCapInfo->dwNumColors * sizeof( RGBQUAD ) );

	bih.biSize = sizeof( BITMAPINFOHEADER );
	bih.biWidth = lpCapInfo->nCapWidth;
	bih.biHeight = lpCapInfo->nCapHeight;
	bih.biPlanes = 1;
	bih.biBitCount = (WORD)lpCapInfo->dwBPP;
	bih.biCompression = BI_RGB;
	bih.biClrUsed = lpCapInfo->dwNumColors;
	bih.biClrImportant = 0;
	bih.biXPelsPerMeter = 0;
	bih.biYPelsPerMeter = 0;
	bih.biSizeImage = 0;

	WriteFile( hFile, &bfh, sizeof( BITMAPFILEHEADER ), &dwWriteSize, NULL );
	WriteFile( hFile, &bih, sizeof( BITMAPINFOHEADER ), &dwWriteSize, NULL );

	if( lpCapInfo->dwNumColors != 0 )
	{
		WriteFile( hFile, clrs, lpCapInfo->dwNumColors * sizeof( RGBQUAD ), &dwWriteSize, NULL );
	}

	WriteFile( hFile, lpCapInfo->pBits, (lpCapInfo->nCapWidth * lpCapInfo->nCapHeight * lpCapInfo->dwBPP)/8, &dwWriteSize, NULL );
	CloseHandle( hFile );

	return 0;
}
