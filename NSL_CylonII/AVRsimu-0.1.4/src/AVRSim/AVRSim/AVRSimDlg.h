// AVRSimDlg.h : header file
//

#pragma once
#include "afxwin.h"


// CAVRSimDlg dialog
class CAVRSimDlg : public CDialog
{
// Construction
public:
	CAVRSimDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_AVRSIM_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CMFCLog m_Log;

	CListBox m_ListBox;
};
