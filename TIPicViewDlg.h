// TIPicViewDlg.h : header file
//

#include "afxwin.h"
#if !defined(AFX_TIPICVIEWDLG_H__B6982AC5_ECCF_44C3_B525_9A2ADEE846F2__INCLUDED_)
#define AFX_TIPICVIEWDLG_H__B6982AC5_ECCF_44C3_B525_9A2ADEE846F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CTIPicViewDlg dialog

class CTIPicViewDlg
{
// Construction
public:
	CTIPicViewDlg();	// standard constructor

// Implementation
	// Generated message map functions
	afx_msg void OnDoubleclickedRnd();

	void LaunchMain(int mode, char *pFile);	// wrapper for maincode()

	// old ErrDistDlg
	afx_msg void OnBnClickedMono();
	bool GreenMode;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIPICVIEWDLG_H__B6982AC5_ECCF_44C3_B525_9A2ADEE846F2__INCLUDED_)
