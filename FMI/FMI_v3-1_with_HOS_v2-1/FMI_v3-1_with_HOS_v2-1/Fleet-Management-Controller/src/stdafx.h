// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN          // Exclude rarely-used stuff from Windows headers
#endif

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER                // Allow use of features specific to Windows 2000 or later.
#define WINVER 0x0500         // Change this to the appropriate value to target Windows Vista or later.
#endif

#ifndef _WIN32_WINDOWS        // Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE             // Allow use of features specific to IE 4.0 or later.
#define _WIN32_IE 0x0600      // Change this to the appropriate value to target IE 7.0 or later.
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS    // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

// silently replace many string functions with the _s variants in MSVC
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT 1

#include <afxwin.h>           // MFC core and standard components
#include <afxext.h>           // MFC extensions
#include <afxdisp.h>          // MFC Automation classes

#include <afxdtctl.h>         // MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>           // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include "resource.h"
