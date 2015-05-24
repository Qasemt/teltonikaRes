/*********************************************************************
*
*   HEADER NAME:
*       CChangeBaudRateDlg.h
*
* Copyright 2013 by Garmin Ltd. or its subsidiaries.
*---------------------------------------------------------------------
* $NoKeywords$
*********************************************************************/
#pragma once

#include "FmiApplicationLayer.h"

#if( FMI_SUPPORT_A616 )

// CChangeBaudRateDlg dialog

class CChangeBaudRateDlg : public CDialog
{
    DECLARE_DYNAMIC(CChangeBaudRateDlg)

public:
    CChangeBaudRateDlg(
        CWnd* pParent,
        FmiApplicationLayer & aCom
    );   // standard constructor

    virtual ~CChangeBaudRateDlg();

    virtual afx_msg void OnOK();

// Dialog Data
    enum { IDD = IDD_CHANGE_BAUD_RATE };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()

    //! Reference to the FMI communication controller
    FmiApplicationLayer& mCom;
    int mNewRate;

public:
    int mCurrentRate;
    };

#endif