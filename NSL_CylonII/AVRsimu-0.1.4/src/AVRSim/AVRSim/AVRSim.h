// AVRSim.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


class CMFCLog
{
public:
	CMFCLog()
	{
		m_pLB = NULL;
	}
	~CMFCLog()
	{
	}

	CListBox *m_pLB; // pointer to a CListBox
	void SetLB( CListBox *box) 
	{
		m_pLB = box;
	}

	void MFC_Printf(const char *formatstring,...)
	{
		if( m_pLB == NULL ) 
			return;

		CString    str, s2;
		va_list    args;

		va_start(args, str);

		str.FormatV(formatstring, args);

		str.Replace('\n',' ');

		m_pLB->AddString(str);	  
	}
};



// CAVRSimApp:
// See AVRSim.cpp for the implementation of this class
//

class CAVRSimApp : public CWinApp
{
public:
	CAVRSimApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CAVRSimApp theApp;