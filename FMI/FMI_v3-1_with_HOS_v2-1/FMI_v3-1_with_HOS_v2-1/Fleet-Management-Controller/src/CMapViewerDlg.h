/*********************************************************************
*
*   HEADER NAME:
*       CMapViewerDlg.h
*
* Copyright 2013 by Garmin Ltd. or its subsidiaries.
*---------------------------------------------------------------------
* $NoKeywords$
*********************************************************************/
#pragma once

#include "CWebBrowser2.h"
#include "FmiApplicationLayer.h"
#include "CWndEventListener.h"

#if( FMI_SUPPORT_A613 )

// CMapViewerDlg dialog

//----------------------------------------------------------------------
//! \brief Browser-based container for listing the Custom Avoidances library
//! \details
//----------------------------------------------------------------------
class CMapViewerDlg : public CDialog, public CWndEventListener
{
    DECLARE_DYNAMIC(CMapViewerDlg)

public:
    CMapViewerDlg(
        CWnd* pParent,
        FmiApplicationLayer & aCom
    );
    virtual ~CMapViewerDlg();

// Dialog Data
    enum { IDD = IDD_MAP_VIEWER };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()

    BOOL OnInitDialog();
    afx_msg void OnSize
        (
        UINT aType,
        int  aClientWidth,
        int  aClientHeight
        );

    void refreshCatalog();

    BOOL addAvoidanceToCatalog(int itemId, LPCTSTR name, LPCTSTR description, long north, long east, long south, long west);
    BOOL removeAvoidanceFromCatalog(int itemId);
    BOOL sendAvoidanceToDevice(int itemId, LPCTSTR name, long north, long east, long south, long west);
    BOOL deleteAvoidanceFromDevice(int itemId);
    BOOL setAvoidanceEnabled(int itemId, BOOL enabled = TRUE);

#if( FMI_SUPPORT_A614 )
    BOOL addRouteToCatalog(CStringArray & args);
    BOOL removeRouteFromCatalog(int itemId);
    BOOL sendRouteToDevice(int itemId);
    BOOL deleteRouteFromDevice(int itemId);
    BOOL activateStopOnDevice(int itemId);
    BOOL completeStopOnDevice(int itemId);
    BOOL moveStopOnDevice(int itemId, short newPosition);
#endif
    BOOL removeItemsFromCatalog(LPCTSTR selector);

    CWebBrowser2 m_browser;

    //! Reference to the FMI communication controller
    FmiApplicationLayer& mCom;

private:
#if( FMI_SUPPORT_A614 )
    //! List of arguments that are accumulated across several calls from browser
    CStringArray    m_addRouteArguments;
#endif

public:
    DECLARE_EVENTSINK_MAP()
    //! This catches navigation attempts for the purpose of handling application-specific navigation
    void OnBeforeNavigate2(LPDISPATCH pDisp, VARIANT* URL, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers, BOOL* Cancel);
    };

#endif