/*********************************************************************
*
*   HEADER NAME:
*       CCustomFormsDlg.h
*
* Copyright 2013 by Garmin Ltd. or its subsidiaries.
*---------------------------------------------------------------------
* $NoKeywords$
*********************************************************************/
#pragma once

#include "CWebBrowser2.h"
#include "XMLTransformer.h"
#include "FmiApplicationLayer.h"
#include "CWndEventListener.h"

#if( FMI_SUPPORT_A612 )

// CCustomFormsDlg dialog

//----------------------------------------------------------------------
//! \brief Browser-based container for listing the Custom Forms library
//! \details When first loaded this dialog lists all the forms imported
//!    into its file-based repository. Each list item has a link to a
//!    preview of the template, as well as links to a preview of any
//!    forms submitted against that template.
//----------------------------------------------------------------------
class CCustomFormsDlg : public CDialog, public CWndEventListener
{
    DECLARE_DYNAMIC(CCustomFormsDlg)

public:
    CCustomFormsDlg(
        CWnd* pParent,
        FmiApplicationLayer & aCom
    );
    virtual ~CCustomFormsDlg();

// Dialog Data
    enum { IDD = IDD_CUSTOM_FORMS };

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

    afx_msg LRESULT CCustomFormsDlg::OnFormDeleted( WPARAM, LPARAM );
    afx_msg LRESULT CCustomFormsDlg::OnFormPositionChanged( WPARAM, LPARAM );
    afx_msg LRESULT CCustomFormsDlg::OnFormSubmitReceived( WPARAM, LPARAM );

    bool buildCatalog(LPCTSTR filename);
    void refreshCatalog();

    void transform(LPCTSTR src, XMLTransformer * transformer, CMapStringToString & params);

    BOOL importForm();
    BOOL sendToDevice(LPCTSTR filename);
    BOOL deleteFromDevice(int formId);
    BOOL moveOnDevice(int formId, uint8 newPosition);
    BOOL requestPosition(int formId);

    CWebBrowser2 m_browser;

    //! cache the form template transformer for re-use
    XMLTransformer * m_templateTransform;
    //! cache the submitted form transformer for re-use
    XMLTransformer * m_submittedTransform;

    //! Reference to the FMI communication controller
    FmiApplicationLayer& mCom;

public:
    DECLARE_EVENTSINK_MAP()
    //! This catches navigation attempts for the purpose of handling application-specific navigation
    void OnBeforeNavigate2(LPDISPATCH pDisp, VARIANT* URL, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers, BOOL* Cancel);
    };

#endif