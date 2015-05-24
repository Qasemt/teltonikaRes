#pragma once

#include "FmiApplicationLayer.h"

#if( FMI_SUPPORT_A617 )

//----------------------------------------------------------------------
//! \brief Form for sending A617 alerts to the device
//----------------------------------------------------------------------
class CSendAlertDlg : public CDialog
{
    DECLARE_DYNAMIC(CSendAlertDlg)

public:
    CSendAlertDlg(
        CWnd* pParent,
        FmiApplicationLayer & aCom
    );
    virtual ~CSendAlertDlg();

    void reset();

// Dialog Data
    enum { IDD = IDD_SEND_ALERT };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()

    BOOL OnInitDialog();
    afx_msg void OnBnClickedOk();
    afx_msg void OnEnChangeAlertText();

private:
    //control data members
    int m_uniqueId;
    CComboBox m_icon;
    int m_timeout;
    CComboBox m_severity;
    BOOL m_playSound;
    CString m_text;
    int m_textLength;

    //! Reference to the FMI communication controller
    FmiApplicationLayer& mCom;

    };

#endif
