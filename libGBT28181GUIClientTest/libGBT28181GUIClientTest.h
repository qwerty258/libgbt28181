
// libGBT28181GUIClientTest.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// ClibGBT28181GUIClientTestApp:
// See libGBT28181GUIClientTest.cpp for the implementation of this class
//

class ClibGBT28181GUIClientTestApp : public CWinApp
{
public:
    ClibGBT28181GUIClientTestApp();

    // Overrides
public:
    virtual BOOL InitInstance();

    // Implementation

    DECLARE_MESSAGE_MAP()
};

extern ClibGBT28181GUIClientTestApp theApp;