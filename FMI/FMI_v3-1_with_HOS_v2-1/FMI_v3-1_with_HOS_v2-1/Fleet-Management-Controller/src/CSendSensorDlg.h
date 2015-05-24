#pragma once

#include "FmiApplicationLayer.h"

#if( FMI_SUPPORT_A617 )

//----------------------------------------------------------------------
//! \brief Form for sending A617 sensor popups to the device
//----------------------------------------------------------------------
class CSendSensorDlg : public CDialog
{
    DECLARE_DYNAMIC(CSendSensorDlg)

public:
    CSendSensorDlg(
        CWnd* pParent,
        FmiApplicationLayer & aCom
    );
    virtual ~CSendSensorDlg();

    void reset();

// Dialog Data
    enum { IDD = IDD_SEND_SENSOR };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()

    BOOL OnInitDialog();
    afx_msg void OnBnClickedOk();
    afx_msg void OnEnChangeSensorName();
    afx_msg void OnEnChangeSensorStatus();
    afx_msg void OnEnChangeSensorDescription();
    afx_msg void OnActionChanged();

private:
    //control data members
    int m_action;
    int m_uniqueId;
    int m_sortOrder;
    CComboBox m_icon;
    CComboBox m_severity;
    BOOL m_playSound;
    BOOL m_recordInHistory;
    CString m_name;
    int m_nameLength;
    CString m_status;
    int m_statusLength;
    CString m_description;
    int m_descriptionLength;

    //! Reference to the FMI communication controller
    FmiApplicationLayer& mCom;

};

#endif
