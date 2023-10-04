// ICSCDLLTest.h : main header file for the ICSCDLLTEST application
//

#if !defined(AFX_ICSCDLLTEST_H__7A786570_0F1A_4828_93F8_0095A937F411__INCLUDED_)
#define AFX_ICSCDLLTEST_H__7A786570_0F1A_4828_93F8_0095A937F411__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CICSCDLLTestApp:
// See ICSCDLLTest.cpp for the implementation of this class
//

class CICSCDLLTestApp : public CWinApp
{
public:
	CICSCDLLTestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CICSCDLLTestApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CICSCDLLTestApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ICSCDLLTEST_H__7A786570_0F1A_4828_93F8_0095A937F411__INCLUDED_)
