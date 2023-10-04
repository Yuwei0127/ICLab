// ICSCDLLTestDlg.h : header file
//

#if !defined(AFX_ICSCDLLTESTDLG_H__0E704670_CDB0_4FF4_A724_ADB7504A131F__INCLUDED_)
#define AFX_ICSCDLLTESTDLG_H__0E704670_CDB0_4FF4_A724_ADB7504A131F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CICSCDLLTestDlg dialog

class CICSCDLLTestDlg : public CDialog
{
// Construction
public:
	CICSCDLLTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CICSCDLLTestDlg)
	enum { IDD = IDD_ICSCDLLTEST_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CICSCDLLTestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CICSCDLLTestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnICSCDLLTest();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ICSCDLLTESTDLG_H__0E704670_CDB0_4FF4_A724_ADB7504A131F__INCLUDED_)
