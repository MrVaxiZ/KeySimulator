
// KeySimulator.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CKeySimulatorApp:
// See KeySimulator.cpp for the implementation of this class
//

class CKeySimulatorApp : public CWinApp
{
public:
	CKeySimulatorApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CKeySimulatorApp theApp;
