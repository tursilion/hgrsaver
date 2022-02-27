// TIPicViewDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TIPicView.h"
#include "TIPicViewDlg.h"
#include "C:\WORK\imgsource\4.0\islibs40_vs05\ISource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern bool StretchHist;
static bool fInSlideMode=false;
extern int PIXA,PIXB,PIXC,PIXD,PIXE,PIXF;
extern int g_nFilter;
extern int g_nPortraitMode;
extern int heightoffset;
extern int g_Perceptual;
extern int g_AccumulateErrors;
extern int g_MaxColDiff;
extern double g_PercepR, g_PercepG, g_PercepB;
extern double g_LumaEmphasis;
extern char cmdFileIn[256], cmdFileOut[256];
extern bool g_Grey;
unsigned char pbuf[8192];

extern unsigned char buf8[280*192];
extern bool fFirstLoad;
extern char imagepath[MAX_PATH];


/////////////////////////////////////////////////////////////////////////////
// CTIPicViewDlg dialog

CTIPicViewDlg::CTIPicViewDlg()
{
	StretchHist=false;
	PIXA=2;
	PIXB=2;
	PIXC=2;
	PIXD=2;
	PIXE=1;
	PIXF=1;
}

/////////////////////////////////////////////////////////////////////////////
// CTIPicViewDlg message handlers

void maincode(int mode, char *pFile);

// This function doesn't get called normally, but that's okay, we can call it the long way ;)
void CTIPicViewDlg::OnDoubleclickedRnd() 
{
	LaunchMain(0,imagepath);
}

void CTIPicViewDlg::LaunchMain(int mode, char *pFile) {
	maincode(mode, pFile);
}

void CTIPicViewDlg::OnBnClickedMono()
{
	// convert the image to mono - this will be the actual bit patterns that we store
	// so it's used in the save function (the button is just for test purposes)
	unsigned char *pOut = pbuf;
	unsigned char *pIn = buf8;
	unsigned char x=0;

	// for kicks, we'll write in green back out to buf8 (so we can view it), palette color 1
	for (int row=0; row<192; row++) {
		for (int col=0; col<280; col+=7) {
			unsigned char work[7];
			unsigned char out;
			unsigned char col1,col2;
			
			memcpy(work, pIn, 7);		// get the 7 pixels - at this point, only color and position matters!

			// sanity test - make sure all 7 pixels are in the same color group
			for (int idx=0; idx<6; idx++) {
				if ((work[idx]&0x04) != (work[idx+1]&0x04)) {
					//AfxMessageBox("Error in color conversion - mismatched color set in byte");
					return;
				}
			}

			out=(work[0]&0x04)<<5;		// make high bit
			// calculate masks
			if ((col/7)&1) {
				// odd, so 1 comes first (but we start at 0x40, not 0x80)
				col1=0x55;
				col2=0xaa;
			} else {
				// even, 2 comes first
				col1=0xaa;
				col2=0x55;
			}
						
			// process the 7 pixels
			unsigned char mask=0x40;	// masking the bitmask 
			unsigned char maskout=0x01;	// goes the other way!
			for (int idx=0; idx<7; idx++) {
				work[idx]&=0x03;
				if (work[idx]==0x03) {
					// white, so it stays on regardless
					work[idx]=1;
				} else if (work[idx]==1) {
					work[idx]=(col1&mask)?1:0;
				} else if (work[idx]==2) {
					work[idx]=(col2&mask)?1:0;
				} else {
					work[idx]=0;
				}
				if (work[idx]) out|=maskout;
				mask>>=1;
				maskout<<=1;
			}

			memcpy(pIn, work, 7);		// get the 7 pixels - at this point, only color and position matters!
			pIn+=7;
			*(pOut++)=out;
		}
	}
}
