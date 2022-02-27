//
// (C) 2004 Mike Brent aka Tursi aka HarmlessLion.com
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
// MfcSaverDlg.cpp : implementation file
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

#include "StdAfx.h"
#include <Objbase.h>
#include <Shlobj.h>
#include "TIPicViewDlg.h"
#include "MfcSaver.h"
#include "MfcSaverDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern bool bFixedMonitor;
extern int FixedX, FixedY;
extern char imagepath[MAX_PATH];
extern unsigned char buf8[280*192];
extern RGBQUAD winpal[256];
extern bool GreenMode;	
extern int LineLoad;
extern bool StretchHist;
extern int g_Perceptual;
extern int g_AccumulateErrors;
int ChangeTime;

/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CMfcSaverDlg, CScreenSaverDlg)
	//{{AFX_MSG_MAP(CMfcSaverDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BROWSE, &CMfcSaverDlg::OnBnClickedBrowse)
END_MESSAGE_MAP()

CMfcSaverDlg::CMfcSaverDlg()
{
	//{{AFX_DATA_INIT(CMfcSaverDlg)
	//}}AFX_DATA_INIT
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
}

BOOL CMfcSaverDlg::OnInitDialog()
{
	CScreenSaverDlg::OnInitDialog();

	return TRUE;
}

void CMfcSaverDlg::LoadData() {
	char buf[256];

	btnHist.SetCheck(StretchHist ? BST_CHECKED : BST_UNCHECKED);
	btnColor.SetCheck(g_Perceptual ? BST_CHECKED : BST_UNCHECKED);
	btnErrors.SetCheck(g_AccumulateErrors ? BST_CHECKED : BST_UNCHECKED);
	btnGreen.SetCheck(GreenMode ? BST_CHECKED : BST_UNCHECKED);
	slideTime.SetRange(1, 60);
	slideTime.SetPos(ChangeTime);
	slideLoad.SetRange(1,192);
	slideLoad.SetPos(LineLoad);
	btnMonitor.SetCheck(bFixedMonitor ? BST_CHECKED : BST_UNCHECKED);
	sprintf(buf, "%d", FixedX);
	editX.SetWindowTextA(buf);
	sprintf(buf, "%d", FixedY);
	editY.SetWindowTextA(buf);
	editPath.SetWindowTextA(imagepath);
}

void CMfcSaverDlg::SaveData() {
	char buf[256];

	StretchHist = btnHist.GetCheck() == BST_CHECKED;
	g_Perceptual = btnColor.GetCheck() == BST_CHECKED;
	g_AccumulateErrors = btnErrors.GetCheck() == BST_CHECKED;
	GreenMode = btnGreen.GetCheck() == BST_CHECKED;
	bFixedMonitor = btnMonitor.GetCheck() == BST_CHECKED;
	ChangeTime = slideTime.GetPos();
	LineLoad = slideLoad.GetPos();

	editX.GetWindowTextA(buf, 256);
	FixedX = atoi(buf);
	editY.GetWindowTextA(buf, 256);
	FixedY = atoi(buf);

	editPath.GetWindowTextA(imagepath, MAX_PATH);
}

/////////////////////////////////////////////////////////////////////////////

void CMfcSaverDlg::DoDataExchange(CDataExchange* pDX)
{
	CScreenSaverDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMfcSaverDlg)
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_CHKHIST, btnHist);
	DDX_Control(pDX, IDC_CHKCOLOR, btnColor);
	DDX_Control(pDX, IDC_CHKERRORS, btnErrors);
	DDX_Control(pDX, IDC_CHKGREEN, btnGreen);
	DDX_Control(pDX, IDC_SLIDETIME, slideTime);
	DDX_Control(pDX, IDC_SLIDESPEED, slideLoad);
	DDX_Control(pDX, IDC_PATH, editPath);
	DDX_Control(pDX, IDC_CHKMONITOR, btnMonitor);
	DDX_Control(pDX, IDC_XPOS, editX);
	DDX_Control(pDX, IDC_YPOS, editY);
}

void CMfcSaverDlg::OnBnClickedBrowse()
{
	char buf[MAX_PATH];

	// get a path for pictures
	BROWSEINFO info;

	info.hwndOwner = GetSafeHwnd();
	info.pidlRoot = NULL;
	strcpy(buf, imagepath);
	info.pszDisplayName = buf;
	info.lpszTitle = "Select folder for images";
	info.ulFlags = BIF_USENEWUI;
	info.lpfn = NULL;
	info.lParam = 0;
	info.iImage = 0;

	PIDLIST_ABSOLUTE pp = SHBrowseForFolder(&info);
	if (NULL != pp) {
		SHGetPathFromIDList(pp, imagepath);
	}
	CoTaskMemFree(pp);

	editPath.SetWindowTextA(imagepath);

}

