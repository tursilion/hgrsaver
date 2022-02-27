//
// (C) 2013 Mike Brent aka Tursi aka HarmlessLion.com
// This software is provided AS-IS. No warranty
// express or implied is provided.
//
// This notice defines the entire license for this code.
// All rights not explicity granted here are reserved by the
// author.
//
// You may redistribute this software provided the original
// archive is UNCHANGED and a link back to my web page,
// http://harmlesslion.com, is provided as the author's site.
// It is acceptable to link directly to a subpage at harmlesslion.com
// provided that page offers a URL for that purpose
//
// Source code, if available, is provided for educational purposes
// only. You are welcome to read it, learn from it, mock
// it, and hack it up - for your own use only.
//
// Please contact me before distributing derived works or
// ports so that we may work out terms. I don't mind people
// using my code but it's been outright stolen before. In all
// cases the code must maintain credit to the original author(s).
//
// -COMMERCIAL USE- Contact me first. I didn't make
// any money off it - why should you? ;) If you just learned
// something from this, then go ahead. If you just pinched
// a routine or two, let me know, I'll probably just ask
// for credit. If you want to derive a commercial tool
// or use large portions, we need to talk. ;)
//
// If this, itself, is a derived work from someone else's code,
// then their original copyrights and licenses are left intact
// and in full force.
//
// http://harmlesslion.com - visit the web page for contact info
//
// MfcSaver.cpp : Defines the class behaviors for the application.
//
//////////
//
// Copyright (C) 1991-98 Ed Halley.
//   http://www.explorati.com/people/ed/
//   ed@explorati.com
//
// This published source code represents original intellectual
// property, owned and copyrighted by Ed Halley.
//
// The owner has authorized this source code for general public
// use without royalty, under two conditions:
//    * The source code maintains this copyright notice in full.
//    * The source code is only distributed for free or
//      reasonable duplication cost, not for distribution profit.
//
// Unauthorized use, copying or distribution is a violation of
// U.S. and international laws and is strictly prohibited.
//
//////////
//

#include <afxwin.h>
#include <math.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include "C:\WORK\imgsource\4.0\islibs40_vs05\ISource.h"
#include "C:\WORK\imgsource\2.1\src\ISLib\isarray.h"
#include "TIPicViewDlg.h"
#include "MfcSaver.h"
#include "MfcSaverDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define APPNAME "hgrsaver"
#define REGPATH "software\\harmlesslion\\hgrsaver"

bool loadFailed;
bool bFixedMonitor;
int FixedX, FixedY;
char imagepath[MAX_PATH];

extern unsigned char buf8[280*192];
extern RGBQUAD winpal[256];
extern bool GreenMode;	
extern int LineLoad;
extern bool StretchHist;
extern int g_Perceptual;
extern int g_AccumulateErrors;

// bottom-up, Apple 2 conversion from actual line to Apple line
// calculated with:
//     5 DIM l(192)
//     6 idx=0
//    10 maj=0
//    20 mi=0
//    30 inc=0
//    50 l(idx)=maj*64+MI*8+inc
//    55 idx=idx+1
//    60 maj=maj+1: IF maj<3 THEN 50
//    70 maj=0
//    80 MI=MI+1:IF MI<8 THEN 50
//    90 mi=0
//   100 inc=inc+1
//   110 IF inc<8 THEN 50
//   120 PRINT "got ";idx;" lines"
//   130 FOR i=191 TO 0 STEP -1
//   140 PRINT l(i);","
//   150 NEXT i
//
int linetable[192] = {
	191,	127,	63,	183,	119,	55,	175,	111,	47,	167,	103,	39,	159,	95,	31,	151,	87,
	23,	143,	79,	15,	135,	71,	7,	190,	126,	62,	182,	118,	54,	174,	110,	46,	166,	102,
	38,	158,	94,	30,	150,	86,	22,	142,	78,	14,	134,	70,	6,	189,	125,	61,	181,	117,	53,
	173,	109,	45,	165,	101,	37,	157,	93,	29,	149,	85,	21,	141,	77,	13,	133,	69,	5,
	188,	124,	60,	180,	116,	52,	172,	108,	44,	164,	100,	36,	156,	92,	28,	148,	84,
	20,	140,	76,	12,	132,	68,	4,	187,	123,	59,	179,	115,	51,	171,	107,	43,	163,	99,
	35,	155,	91,	27,	147,	83,	19,	139,	75,	11,	131,	67,	3,	186,	122,	58,	178,	114,	50,
	170,	106,	42,	162,	98,	34,	154,	90,	26,	146,	82,	18,	138,	74,	10,	130,	66,	2,	185,
	121,	57,	177,	113,	49,	169,	105,	41,	161,	97,	33,	153,	89,	25,	145,	81,	17,	137,
	73,	9,	129,	65,	1,	184,	120,	56,	176,	112,	48,	168,	104,	40,	160,	96,	32,	152,
	88,	24,	144,	80,	16,	136,	72,	8,	128,	64,	0
};

void CMfcSaver::LoadSettings() {
	CRegKey key;

	// set defaults
	StretchHist = true;
	g_Perceptual = 1;
	g_AccumulateErrors = 0;
	GreenMode = false;
	ChangeTime = 5;
	LineLoad = 1;
	bFixedMonitor = false;
	FixedX = 2000;
	FixedY = 100;
	strcpy(imagepath, "c:\\my pictures\\");

	try {
		key.Create(HKEY_CURRENT_USER, REGPATH); 

		key.QueryDWORDValue("stretchhist", (DWORD&)StretchHist);
		key.QueryDWORDValue("perceptual", (DWORD&)g_Perceptual);
		key.QueryDWORDValue("accumulate_errors", (DWORD&)g_AccumulateErrors);
		key.QueryDWORDValue("greenmode", (DWORD&)GreenMode);
		key.QueryDWORDValue("changetime", (DWORD&)ChangeTime);
		key.QueryDWORDValue("lineload", (DWORD&)LineLoad);
		key.QueryDWORDValue("fixedmonitor", (DWORD&)bFixedMonitor);
		key.QueryDWORDValue("fixedx", (DWORD&)FixedX);
		key.QueryDWORDValue("fixedy", (DWORD&)FixedY);
		ULONG len=MAX_PATH;
		key.QueryStringValue("imagepath", imagepath, &len);

		key.Close();
	}
	catch(CException *) {
		return;
	}

}

void CMfcSaver::SaveSettings() {
	CRegKey key;

	try {
		key.Create(HKEY_CURRENT_USER, REGPATH); 

		key.SetDWORDValue("stretchhist", StretchHist);
		key.SetDWORDValue("perceptual", g_Perceptual);
		key.SetDWORDValue("accumulate_errors", g_AccumulateErrors);
		key.SetDWORDValue("greenmode", GreenMode);
		key.SetDWORDValue("changetime", ChangeTime);
		key.SetDWORDValue("lineload", LineLoad);
		key.SetDWORDValue("fixedmonitor", bFixedMonitor);
		key.SetDWORDValue("fixedx", FixedX);
		key.SetDWORDValue("fixedy", FixedY);
		key.SetStringValue("imagepath", imagepath);

		key.Close();
	}
	catch(CException *) {
		return;
	}
}

/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CMfcSaver, CScreenSaverWnd)
	//{{AFX_MSG_MAP(CMfcSaver)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// As required of CScreenSaverWnd-based screen savers, these are the two
// global instances of screen saver objects.  One is the saver itself,
// and one is the dialog for configuring the options of the screen saver.
//
// Unlike most MFC applications, there is no instance of any CWinApp object.
//
CMfcSaver theSaver;
CMfcSaverDlg theSaverDialog;

CMfcSaver::CMfcSaver()
{
	bFixedMonitor = false;
	FixedX = 0;
	FixedY = 0;

	pWnd=NULL;

	mutex = CreateMutex(NULL, false, NULL);
	cntdown = 0;

	for (int idx=0; idx<10; idx++) {
		lclbuf8[idx]=NULL;
	}
	lclidx=0;

	// create a console for debugging
#ifdef _DEBUG
	AllocConsole();
	int hCrt, i;
	FILE *hf;
	hCrt = _open_osfhandle((long) GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
	hf = _fdopen( hCrt, "w" );
	*stdout = *hf;
	i = setvbuf( stdout, NULL, _IONBF, 0 ); 
	printf("CONSOLE UP!\n");
#endif

	LoadSettings();

	srand((unsigned int)time(NULL));
}

CMfcSaver::~CMfcSaver()
{
}

// Monitor enumerator
BOOL CALLBACK AddMonitor(HMONITOR , HDC , LPRECT pRect, LPARAM dwData) {
	CMfcSaver *p = (CMfcSaver*)dwData;

	// max of 9 monitors
	if (p->lclidx == 9) return FALSE;

	p->monx[p->lclidx] = pRect->left;
	p->mony[p->lclidx] = pRect->top;
	p->monw[p->lclidx] = pRect->right-pRect->left;
	p->monh[p->lclidx] = pRect->bottom-pRect->top;
	p->lclbuf8[p->lclidx] = (unsigned char*)malloc(p->monw[p->lclidx]*(p->monh[p->lclidx]+1));

	printf("Monitor %d: %d,%d  - %dx%d\n", p->lclidx, p->monx[p->lclidx], p->mony[p->lclidx], p->monw[p->lclidx], p->monh[p->lclidx]);

	p->lclidx++;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////

void CMfcSaver::OnDraw(	CDC* pdc)
{
	bool bForce = false;
	// This is a horrible way to do it. But whatever. quick hack. I need to
	// write a new screensaver framework at some point. Why am I putting that
	// here? I have no idea.
	if (NULL == pWnd) {
		// process each monitor, zero them out. Put a bload command on the primary
		lclidx=0;
		EnumDisplayMonitors(NULL, NULL, AddMonitor, (LPARAM)this);
		printf("Got %d monitors\n", lclidx);

		// if configured for a monitor, then we run exclusively on that monitor
		if (bFixedMonitor) {
			POINT pt;
			pt.x=FixedX;
			pt.y=FixedY;
			HMONITOR mon = MonitorFromPoint(pt, MONITOR_DEFAULTTONULL);
			if (mon != NULL) {
				MONITORINFO info;
				info.cbSize = sizeof(info);
				if (GetMonitorInfo(mon, &info)) {
					bForce = true;
					SetWindowPos(&CWnd::wndNoTopMost, info.rcMonitor.left, info.rcMonitor.top, 
						info.rcMonitor.right-info.rcMonitor.left, info.rcMonitor.bottom-info.rcMonitor.top, 0);

					for (int idx=0; idx<lclidx; idx++) {
						free(lclbuf8[idx]);
						lclbuf8[idx]=NULL;
					}
					lclidx=1;
					monx[0]=info.rcMonitor.left;
					mony[0]=info.rcMonitor.top;
					monw[0]=info.rcMonitor.right-info.rcMonitor.left;
					monh[0]=info.rcMonitor.bottom-info.rcMonitor.top;
					lclbuf8[0]=(unsigned char*)malloc(monw[0]*(monh[0]+1));
				}
			}
		}

		// start the setup
		pWnd=(HWND)GetSafeHwnd();
		HDC dc=::GetDC(pWnd);
		RECT myRect;
		::GetWindowRect(pWnd, &myRect);
		printf("Window: %d,%d  - %dx%d\n", myRect.left, myRect.top, myRect.right-myRect.left, myRect.bottom-myRect.top);

#ifdef _DEBUG
		// size to the test window (assumes horizontal multimonitor only)
		if (!bForce) {
			printf("Setting up for debug window for %d monitors\n", lclidx);
			int truew=0;
			for (int idx=0; idx<lclidx; idx++) {
				truew+=monw[idx];
			}
			int nw=0;
			for (int idx=0; idx<lclidx; idx++) {
				double ratio=1024.0/(double)(truew);
				monx[idx]=nw;
				monw[idx]=(int)(monw[idx]*ratio);
				nw+=monw[idx];

				// now we need to convert the client coordinates to screen coordinates so it looks like monitor definitions
				RECT myRect;
				myRect.left = monx[idx];
				myRect.top = mony[idx];
				myRect.bottom = mony[idx]+10;	// doesn't matter to me
				myRect.right = monx[idx]+10;
				ClientToScreen(&myRect);
				monx[idx] = myRect.left;
				mony[idx] = myRect.top;

				printf("Window %d squished: startx: %d  width: %d  (ratio: %f)\n", idx, monx[idx], monw[idx], ratio);
			}
		}
#endif

		pdc->FillSolidRect(0, 0, myRect.right-myRect.left, myRect.bottom-myRect.top, RGB(0,0,0));
		HICON hIcon = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
		if (hIcon) printf("got the icon\n");

		{
			RECT r;
			double ratio=monw[0]/256.0;
			if (monh[0]/192.0 < ratio) ratio=monh[0]/192.0;
			int offsetx=(int)((monw[0]-(280*ratio))/2)+monx[0];
			int offsety=(int)((monh[0]-(192*ratio))/2)+mony[0];			
			r.left = offsetx;
			r.top = offsety;
			r.right = monx[0]+10;
			r.bottom = mony[0]+10;
			ScreenToClient(&r);
			DrawIconEx(dc, r.left, (int)(r.top+191*ratio-16*ratio), hIcon, (int)(120*ratio), (int)(8*ratio), 0, NULL, DI_NORMAL);
			DestroyIcon(hIcon);	// we never need it again
			bErase = TRUE;		// erase before drawing next
		}
		::ReleaseDC(pWnd, dc);
		
		// start the main process running
		lclidx=0;
		myTimer=::SetTimer(pWnd, 1, 50, NULL);
	} else {
		// we are just being asked to redraw, so.... do it
		unsigned char *pLcl = lclbuf8[lclidx];
		if ((NULL != buf8)&&(NULL != pLcl)&&(cntdown!=0)) {
			RECT myRect;

			if (bErase) {
				printf("Erasing first\n");
				// only monitor 0 is needed to erase
				myRect.left = monx[0];
				myRect.right = monx[0]+monw[0];
				myRect.top = mony[0];
				myRect.bottom = mony[0]+monh[0];
				ScreenToClient(&myRect);

				pdc->FillSolidRect(&myRect, RGB(0,0,0));
				bErase = FALSE;
			}
			// ratio to fit the window (a race on lclidx might deform the image, but no biggie and not likely)
			double ratio = monw[lclidx]/280.0;
			if (monh[lclidx]/192.0 < ratio) ratio = monh[lclidx]/192.0;

			myRect.left = monx[lclidx];
			myRect.right = monx[lclidx]+monw[lclidx];
			myRect.top = mony[lclidx];
			myRect.bottom = mony[lclidx]+monh[lclidx];
			ScreenToClient(&myRect);

			IS40_StretchDraw8Bit(pdc->GetSafeHdc(), pLcl, 280, 192, 280, winpal, (UINT32)((monw[lclidx]-280*ratio)/2)+myRect.left, (UINT32)((monh[lclidx]-192*ratio)/2)+myRect.top, (int)(280*ratio), (int)(192*ratio));
		} else {
			//printf("No buffer to paint\n");
		}
	}	
}


/////////////////////////////////////////////////////////////////////////////

void CMfcSaver::OnInitialUpdate()
{
}

void CMfcSaver::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent==1) {
		// Tick expired - process only if we are not busy
		if (WAIT_OBJECT_0 == WaitForSingleObject(mutex, 0)) {
			if (cntdown == 0) {
				printf("Processing image...\n");
				// try up to three times
				loadFailed = false;
				dlg.OnDoubleclickedRnd();
				if (loadFailed) dlg.OnDoubleclickedRnd();
				if (loadFailed) dlg.OnDoubleclickedRnd();

				if (GreenMode) {
					dlg.OnBnClickedMono();
				}

				printf("requesting paint for monitor %d\n", lclidx);
				cntdown=-192;
			} else if (cntdown < 0) {
				RECT myRect;
				myRect.left = monx[lclidx];
				myRect.right = monx[lclidx]+monw[lclidx];
				myRect.top = mony[lclidx];
				myRect.bottom = mony[lclidx]+monh[lclidx];
				ScreenToClient(&myRect);

				for (int idx=0; idx<LineLoad; idx++) {
					// drawing scanlines
					++cntdown;
					if (cntdown == 0) break;
					// which apple line is it?
					// I know this can be done mathematically, but it drove me up the wall
					int ad=linetable[-cntdown]*280;
					// copy that line
					memcpy(lclbuf8[lclidx]+ad, &buf8[ad], 280);
				}

				InvalidateRect(&myRect);
				//InvalidateRect(NULL);
				if (cntdown == 0) {
					cntdown = ChangeTime*20;
				}
			} else {
				// countdown to next image
				--cntdown;
				if (cntdown == 0) {
					if (lclbuf8[++lclidx] == NULL) lclidx=0;
				}
			}
			ReleaseMutex(mutex);
		}
	}
}


