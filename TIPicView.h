// TIPicView.h : main header file for the TIPICVIEW application
//

#if !defined(AFX_TIPICVIEW_H__F4725884_9A29_4492_AE12_0E3B106C2738__INCLUDED_)
#define AFX_TIPICVIEW_H__F4725884_9A29_4492_AE12_0E3B106C2738__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#define XOFFSET 250

typedef struct tagMYRGBQUAD {
	// mine is RGB instead of BGR
        BYTE    rgbRed;
        BYTE    rgbGreen;
        BYTE    rgbBlue;
        BYTE    rgbReserved;
} MYRGBQUAD;

BOOL MYRGBTo8BitDithered(BYTE *pRGB, UINT32 uWidth, UINT32 uHeight, BYTE *p8Bit, UINT32 uColors, MYRGBQUAD *pal);

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


#endif // !defined(AFX_TIPICVIEW_H__F4725884_9A29_4492_AE12_0E3B106C2738__INCLUDED_)

