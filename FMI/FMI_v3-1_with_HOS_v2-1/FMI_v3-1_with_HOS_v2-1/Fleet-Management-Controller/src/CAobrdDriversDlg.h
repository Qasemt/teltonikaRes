/*********************************************************************
*
*   HEADER NAME:
*       CAobrdDriversDlg.h
*
*   Copyright 2012 by Garmin Ltd. or its subsidiaries.
*---------------------------------------------------------------------
* $NoKeywords$
*********************************************************************/
#ifndef CAobrdDriversDlg_H
#define CAobrdDriversDlg_H

class CAobrdDriversDlg;

#include "FmiApplicationLayer.h"


#if( FMI_SUPPORT_A610 )

//----------------------------------------------------------------------
//! \brief Dialog allowing the user to manage AOBRD drivers.
//! \details This dialog allows the user to set up a database of drivers
//!    that are used to authenticate and provision a device.
//! \since Protocol A610
//----------------------------------------------------------------------
class CAobrdDriversDlg : public CDialog
{
    DECLARE_DYNAMIC(CAobrdDriversDlg)
    DECLARE_MESSAGE_MAP()

public:
    CAobrdDriversDlg(
        CWnd                * aParent,
        FmiApplicationLayer & aCom
    );

    virtual ~CAobrdDriversDlg();

protected:

    virtual void DoDataExchange(
        CDataExchange * pDX
    );

    BOOL OnInitDialog();
    void clearData();
    void updateListBox();
    void PostNcDestroy();
    afx_msg void OnBnClickedSendUpdate();
#if( FMI_SUPPORT_A615 )
    afx_msg void OnBnClickedSendLogoff();
#endif
    afx_msg void OnBnClickedDelete();
    afx_msg void OnBnClickedSet();
    afx_msg void OnBnClickedShipments();
    afx_msg void OnChangeDriverData();
    afx_msg void OnBnClickedOk();
    afx_msg void OnCancel();
    afx_msg void OnEnSetfocusLoginEdit();
    afx_msg void OnEnKillfocusLoginEdit();
    afx_msg void OnLbnSelchangeDriverList();
    afx_msg void OnLbnSetfocusDriverList();
    afx_msg void OnLbnKillfocusDriverList();

protected:
    //! Reference to the FMI communication controller
    FmiApplicationLayer& mCom;

    //! \brief List box control containing the drivers
    CListBox mListBox;

    //! \brief Index of the list box item currently selected
    int mSelectedIndex;

    //! \brief DDX member: the contents of the Driver ID edit box
    CString mDriverId;

    //! \brief DDX member: the contents of the Driver Password edit box
    CString mDriverPassword;

    //! \brief DDX member: the contents of the First Name edit box
    CString mDriverFirstName;

    //! \brief DDX member: the contents of the Last Name edit box
    CString mDriverLastName;

    //! \brief DDX member: the contents of the Carrier Name edit box
    CString mDriverCarrierName;

    //! \brief DDX member: the contents of the Carrier ID edit box
    CString mDriverCarrierID;

    //! \brief DDX member: the selection of the Long Term Ruleset chooser
    int mDriverLongTermRuleset;

#if( FMI_SUPPORT_A615 )
    //! \brief DDX member: the selection of the Load Type Ruleset chooser
    int mDriverLoadTypeRuleset;
#endif

    //! \brief DDX member: the selection of the Time Zone chooser
    int mDriverTimeZone;
};

#endif

#endif
