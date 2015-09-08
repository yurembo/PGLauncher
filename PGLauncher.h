// author: Yuriy Yazev, july 2015
// PGLauncher.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CPGLauncherApp:
// See PGLauncher.cpp for the implementation of this class
//

class CPGLauncherApp : public CWinApp
{
private:
	CMutex *m_mutexHandle;
	CSingleLock *m_mutexLock;

public:
	CPGLauncherApp();

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CPGLauncherApp theApp;