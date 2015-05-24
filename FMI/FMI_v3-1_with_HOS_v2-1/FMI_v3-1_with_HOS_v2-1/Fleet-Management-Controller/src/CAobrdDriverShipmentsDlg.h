/*********************************************************************
*
*   HEADER NAME:
*       CAobrdDriverShipmentsDlg.h
*
*   Copyright 2012 by Garmin Ltd. or its subsidiaries.
*---------------------------------------------------------------------
* $NoKeywords$
*********************************************************************/
#ifndef CAobrdDriverShipmentsDlg_H
#define CAobrdDriverShipmentsDlg_H

class CAobrdDriverShipmentsDlg;

#include "FmiApplicationLayer.h"
#include "AobrdDriverItem.h"

//----------------------------------------------------------------------
//! \brief Dialog allowing the user to manage AOBRD driver shipments.
//! \details This dialog allows the user to set up a database of driver
//!    shipments that are downloaded to device during driver login.
//! \since Protocol A610
//----------------------------------------------------------------------
class CAobrdDriverShipmentsDlg : public CDialog
{
    DECLARE_DYNAMIC(CAobrdDriverShipmentsDlg)
    DECLARE_MESSAGE_MAP()

public:
    static BOOL ShowShipments( CWnd * aParent, AobrdDriverItem * driver );

    virtual ~CAobrdDriverShipmentsDlg();

protected:

    CAobrdDriverShipmentsDlg(
        CWnd                * aParent,
        AobrdDriverItem     * aDriver
    );

    virtual void DoDataExchange(
        CDataExchange * pDX
    );

    BOOL OnInitDialog();
    void clearData();
    void updateListBox();
    void PostNcDestroy();
    afx_msg void OnBnClickedNew();
    afx_msg void OnBnClickedDelete();
    afx_msg void OnBnClickedSet();
    afx_msg void OnChangeShipmentData();
    afx_msg void OnBnClickedOk();
    afx_msg void OnCancel();
    afx_msg void OnEnSetfocusLoginEdit();
    afx_msg void OnEnKillfocusLoginEdit();
    afx_msg void OnDateTimeCtrlSetFocus( NMHDR * pNotifyStruct, LRESULT * result );
    afx_msg void OnDateTimeCtrlKillFocus( NMHDR * pNotifyStruct, LRESULT * result );
    afx_msg void OnLbnSelchangeShipmentList();
    afx_msg void OnLbnSetfocusShipmentList();
    afx_msg void OnLbnKillfocusShipmentList();

protected:
    //! \brief List box control containing the shipments
    CListBox mListBox;

    //! \brief Index of the list box item currently selected
    int mSelectedIndex;

    //! \brief DDX member: the contents of the Shipper Name edit box
    CString mShipperName;

    //! \brief DDX member: the contents of the Document Number edit box
    CString mDocumentNumber;

    //! \brief DDX member: the contents of the Commodity edit box
    CString mCommodity;

    //! \brief DDX member: the contents of the Timestamp Date control
    COleDateTime mTimestampDate;

    //! \brief DDX member: the contents of the Timestamp Time control
    COleDateTime mTimestampTime;

    //! \brief DDX member: the contents of the Start Time Date control
    COleDateTime mStartTimeDate;

    //! \brief DDX member: the contents of the Start Time Time control
    COleDateTime mStartTimeTime;

    //! \brief DDX member: the contents of the End Time Date control
    COleDateTime mEndTimeDate;

    //! \brief DDX member: the contents of the End Time Time control
    COleDateTime mEndTimeTime;

    //! \brief pointer to the driver whose shipments are being edited
    AobrdDriverItem * mDriver;
};

#endif