/*********************************************************************
*
*   HEADER NAME:
*       CAobrdOdometerDlg.h
*
*   Copyright 2012 by Garmin Ltd. or its subsidiaries.
*---------------------------------------------------------------------
* $NoKeywords$
*********************************************************************/
#ifndef CAobrdOdometerDlg_H
#define CAobrdOdometerDlg_H

#include "FmiApplicationLayer.h"

#if( FMI_SUPPORT_A610 )

//---------------------------------------------------------------------
//! \brief Modal dialog allowing the user to set AOBRD Odometer value.
//! \since Protocol A610
//---------------------------------------------------------------------
class CAobrdOdometerDlg : public CDialog
{
    DECLARE_DYNAMIC( CAobrdOdometerDlg )
    DECLARE_MESSAGE_MAP()

public:
    CAobrdOdometerDlg
        (
        CWnd                * aParent,
        FmiApplicationLayer & aCom
        );
    virtual ~CAobrdOdometerDlg();

protected:
    virtual void DoDataExchange
        (
        CDataExchange* aDataExchange
        );
    afx_msg void OnEnChangeAobrdOdometer();
    afx_msg void OnBnClickedOk();
    BOOL OnInitDialog();

    //! Reference to the FMI communication controller that this dialog uses
    FmiApplicationLayer & mCom;

    //! Odometer reading entered by the user
    CString odometer_value;
};
#endif

#endif
