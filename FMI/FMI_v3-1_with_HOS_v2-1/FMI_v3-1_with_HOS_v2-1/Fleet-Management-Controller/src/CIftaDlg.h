/*********************************************************************
*
*   HEADER NAME:
*       CIftaDlg.h
*
* Copyright 2013 by Garmin Ltd. or its subsidiaries.
*---------------------------------------------------------------------
* $NoKeywords$
*********************************************************************/
#pragma once

#include "FmiApplicationLayer.h"
#include "CWndEventListener.h"

#if( FMI_SUPPORT_A615 )

//----------------------------------------------------------------------
//! \brief Form for interacting with IFTA data on the device
//! \details Download IFTA data or delete IFTA for a specified timeframe
//----------------------------------------------------------------------
class CIftaDlg : public CDialog, public CWndEventListener
{
    DECLARE_DYNAMIC(CIftaDlg)

public:
    CIftaDlg(
        CWnd* pParent,
        FmiApplicationLayer & aCom
    );
    virtual ~CIftaDlg();

// Dialog Data
    enum { IDD = IDD_IFTA };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()

private:
    //event handlers
    afx_msg void OnBnClickedDataFetch();
    afx_msg void OnBnClickedDataDelete();
    afx_msg LRESULT OnEventIftaFileTransferReceived( WPARAM, LPARAM );

    //control data members
    COleDateTime mStartDate;
    COleDateTime mStartTime;
    COleDateTime mEndDate;
    COleDateTime mEndTime;

    //! Reference to the FMI communication controller
    FmiApplicationLayer& mCom;
    };

#endif
