/*********************************************************************
*
*   MODULE NAME:
*       CSendAlertDlg.cpp
*
* Copyright 2013 by Garmin Ltd. or its subsidiaries.
*---------------------------------------------------------------------
* $NoKeywords$
*********************************************************************/

#include "stdafx.h"
#include "CSendAlertDlg.h"

#if( FMI_SUPPORT_A617 )

// CSendAlertDlg dialog

IMPLEMENT_DYNAMIC(CSendAlertDlg, CDialog)

//----------------------------------------------------------------------
//! \brief Constructor
//! \param aParent The parent window of this dialog
//! \param aCom The FmiApplicationLayer used to send commands to device
//----------------------------------------------------------------------
CSendAlertDlg::CSendAlertDlg(CWnd* pParent, FmiApplicationLayer & aCom)
    : CDialog(CSendAlertDlg::IDD, pParent)
    , mCom( aCom )
    , m_uniqueId(0)
    , m_timeout(0)
    , m_playSound(FALSE)
    , m_text(_T(""))
    , m_textLength(0)
    {

}

CSendAlertDlg::~CSendAlertDlg()
{
}

void CSendAlertDlg::DoDataExchange(CDataExchange* pDX)
{
CDialog::DoDataExchange(pDX);
DDX_Text(pDX, IDC_ALERT_UNIQUE_ID, m_uniqueId);
DDX_Control(pDX, IDC_ALERT_ICON, m_icon);
DDX_Text(pDX, IDC_ALERT_TIMEOUT, m_timeout);
DDX_Control(pDX, IDC_ALERT_SEVERITY, m_severity);
DDX_Check(pDX, IDC_ALERT_PLAY_SOUND, m_playSound);
DDX_Text(pDX, IDC_ALERT_TEXT, m_text);
DDX_Text(pDX, IDC_ALERT_TEXT_LENGTH, m_textLength);
    }


BEGIN_MESSAGE_MAP(CSendAlertDlg, CDialog)
    ON_BN_CLICKED(IDOK, &CSendAlertDlg::OnBnClickedOk)
    ON_EN_CHANGE(IDC_ALERT_TEXT, &CSendAlertDlg::OnEnChangeAlertText)
END_MESSAGE_MAP()

//----------------------------------------------------------------------
//! \brief reset the form to defaults (since this dialog is long-lived)
//----------------------------------------------------------------------
void CSendAlertDlg::reset()
    {
    m_uniqueId = 1;
    m_icon.SetCurSel( -1 );
    m_timeout = 1;
    m_severity.SetCurSel( -1 );
    m_playSound = FALSE;
    m_text = _T( "" );
    m_textLength = 0;
    UpdateData( FALSE );
    }

// CSendAlertDlg message handlers

//----------------------------------------------------------------------
//! \brief This function is called when the window is created.
//! \return TRUE, since this function does not set focus to a control
//----------------------------------------------------------------------
BOOL CSendAlertDlg::OnInitDialog()
    {
    CDialog::OnInitDialog();

    (( CSpinButtonCtrl*) GetDlgItem( IDC_SPIN1 ) )->SetRange( 1, 65535/2 );
    (( CSpinButtonCtrl*) GetDlgItem( IDC_SPIN2 ) )->SetRange( 0, 100 );

    return TRUE;
    }

//----------------------------------------------------------------------
//! \brief Validate form, build and send FMI packet to device
//----------------------------------------------------------------------
void CSendAlertDlg::OnBnClickedOk()
    {
    UpdateData();

    fmi_alert_popup_request message;
    memset( &message, 0, sizeof( message ) );
    message.unique_id = m_uniqueId;
    message.play_sound = m_playSound;
    message.timeout = m_timeout;
    message.icon = m_icon.GetCurSel();
    message.severity = m_severity.GetCurSel();

    WideCharToMultiByte( mCom.mClientCodepage, 0, m_text.GetBuffer(), -1, message.alert_text, sizeof( message.alert_text ), NULL, NULL );
    message.alert_text[sizeof( message.alert_text ) - 1] = '\0';

    mCom.sendFmiPacket( FMI_ALERT_POPUP_REQUEST, (uint8*)&message, sizeof( message ) );

    OnOK();
    }

//----------------------------------------------------------------------
//! \brief Update the text length label in response to change in text
//----------------------------------------------------------------------
void CSendAlertDlg::OnEnChangeAlertText()
    {
    UpdateData();

    fmi_alert_popup_request message;
    char text[ sizeof( message.alert_text ) ];
    WideCharToMultiByte( mCom.mClientCodepage, 0, m_text.GetBuffer(), -1, text, sizeof( text ), NULL, NULL );
    text[sizeof( text ) - 1] = '\0';

    m_textLength = strlen( text );

    UpdateData( FALSE );
    }

#endif
