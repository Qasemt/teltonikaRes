/*********************************************************************
*
*   HEADER NAME:
*       CHosSettingDlg.h
*
* Copyright 2014 by Garmin Ltd. or its subsidiaries.
*---------------------------------------------------------------------
* $NoKeywords$
*********************************************************************/
#pragma once

#include "FmiApplicationLayer.h"

#if( FMI_SUPPORT_A615 )

//----------------------------------------------------------------------
//! \brief Form for individually capturing general HOS settings
//! \details Capture parameters for and send the Auto Status Updates
//    toggle command to the device
//----------------------------------------------------------------------
class CHosSettingDlg : public CDialog
{
    DECLARE_DYNAMIC(CHosSettingDlg)

public:
    static UINT CaptureSetting(
        CWnd* pParent,
        UINT titleStringId,
        UINT valueLabelStringId,
        UINT valueUnitStringId,
        long valueLowerLimit,
        long valueUpperLimit,
        BOOL & enabled,
        uint16 & value
    );

    virtual ~CHosSettingDlg();

// Dialog Data
    enum { IDD = IDD_HOS_SETTING };

protected:
    CHosSettingDlg(
        CWnd* pParent
    );   // standard constructor
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()

    BOOL OnInitDialog();
    afx_msg void OnReevaluate();

    //! Control value representing radio button for enable/disable
    BOOL mDisabled;

    //! Control value representing the HOS setting's value
    CString mValue;

    //! Resource ID for the window title
    UINT mTitleStringId;

    //! Resource ID for the value field label
    UINT mValueLabelStringId;

    //! Resource ID for the value field units label
    UINT mValueUnitStringId;

    //! Field value lower limit
    long mValueLowerLimit;

    //! Field value upper limit
    long mValueUpperLimit;

    };

#endif