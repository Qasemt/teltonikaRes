/*********************************************************************
*
*   MODULE NAME:
*       CSendSensorDlg.cpp
*
* Copyright 2014 by Garmin Ltd. or its subsidiaries.
*---------------------------------------------------------------------
* $NoKeywords$
*********************************************************************/

#include "stdafx.h"
#include "CSendSensorDlg.h"

#if( FMI_SUPPORT_A617 )

// CSendSensorDlg dialog

IMPLEMENT_DYNAMIC(CSendSensorDlg, CDialog)

//----------------------------------------------------------------------
//! \brief Constructor
//! \param aParent The parent window of this dialog
//! \param aCom The FmiApplicationLayer used to send commands to device
//----------------------------------------------------------------------
CSendSensorDlg::CSendSensorDlg(CWnd* pParent, FmiApplicationLayer & aCom)
    : CDialog(CSendSensorDlg::IDD, pParent)
    , mCom( aCom )
    , m_action(0)
    , m_uniqueId(0)
    , m_sortOrder(0)
    , m_playSound(FALSE)
    , m_recordInHistory(FALSE)
    , m_name(_T(""))
    , m_nameLength(0)
    , m_status(_T(""))
    , m_statusLength(0)
    , m_description(_T(""))
    , m_descriptionLength(0)
    {

}

CSendSensorDlg::~CSendSensorDlg()
{
}

void CSendSensorDlg::DoDataExchange(CDataExchange* pDX)
{
CDialog::DoDataExchange(pDX);
DDX_Text(pDX, IDC_SENSOR_UNIQUE_ID, m_uniqueId);
DDX_Text(pDX, IDC_SENSOR_SORT_ORDER, m_sortOrder);
DDX_Control(pDX, IDC_SENSOR_ICON, m_icon);
DDX_Control(pDX, IDC_SENSOR_SEVERITY, m_severity);
DDX_Check(pDX, IDC_SENSOR_PLAY_SOUND, m_playSound);
DDX_Check(pDX, IDC_SENSOR_RECORD_IN_HISTORY, m_recordInHistory);
DDX_Text(pDX, IDC_SENSOR_NAME, m_name);
DDX_Text(pDX, IDC_SENSOR_NAME_LENGTH, m_nameLength);
DDX_Text(pDX, IDC_SENSOR_STATUS, m_status);
DDX_Text(pDX, IDC_SENSOR_STATUS_LENGTH, m_statusLength);
DDX_Text(pDX, IDC_SENSOR_DESCRIPTION, m_description);
DDX_Text(pDX, IDC_SENSOR_DESCR_LENGTH, m_descriptionLength);
DDX_Radio(pDX, IDC_CREATE_SENSOR, m_action);
    }


BEGIN_MESSAGE_MAP(CSendSensorDlg, CDialog)
    ON_BN_CLICKED(IDOK, &CSendSensorDlg::OnBnClickedOk)
    ON_EN_CHANGE(IDC_SENSOR_NAME, &CSendSensorDlg::OnEnChangeSensorName)
    ON_EN_CHANGE(IDC_SENSOR_STATUS, &CSendSensorDlg::OnEnChangeSensorStatus)
    ON_EN_CHANGE(IDC_SENSOR_DESCRIPTION, &CSendSensorDlg::OnEnChangeSensorDescription)
    ON_BN_CLICKED(IDC_CREATE_SENSOR, &CSendSensorDlg::OnActionChanged)
    ON_BN_CLICKED(IDC_UPDATE_SENSOR, &CSendSensorDlg::OnActionChanged)
    ON_BN_CLICKED(IDC_DELETE_SENSOR, &CSendSensorDlg::OnActionChanged)
    ON_BN_CLICKED(IDC_QUERY_SENSOR_DISPLAY_INDEX, &CSendSensorDlg::OnActionChanged)
END_MESSAGE_MAP()

//----------------------------------------------------------------------
//! \brief reset the form to defaults (since this dialog is long-lived)
//----------------------------------------------------------------------
void CSendSensorDlg::reset()
    {
    m_action = 0;
    m_uniqueId = 1;
    m_icon.SetCurSel( -1 );
    m_severity.SetCurSel( -1 );
    m_sortOrder = 0;
    m_playSound = FALSE;
    m_recordInHistory = FALSE;
    m_name = _T( "" );
    m_nameLength = 0;
    m_status = _T( "" );
    m_statusLength = 0;
    m_description = _T( "" );
    m_descriptionLength = 0;
    UpdateData( FALSE );

    OnActionChanged();
    }

// CSendSensorDlg message handlers

//----------------------------------------------------------------------
//! \brief This function is called when the window is created.
//! \return TRUE, since this function does not set focus to a control
//----------------------------------------------------------------------
BOOL CSendSensorDlg::OnInitDialog()
    {
    CDialog::OnInitDialog();

    (( CSpinButtonCtrl*) GetDlgItem( IDC_SPIN1 ) )->SetRange( 1, 65535/2 );
    (( CSpinButtonCtrl*) GetDlgItem( IDC_SPIN2 ) )->SetRange( 1, 256 );

    return TRUE;
    }

//----------------------------------------------------------------------
//! \brief Validate form, build and send appropriate FMI packet to device
//----------------------------------------------------------------------
void CSendSensorDlg::OnBnClickedOk()
    {
    UpdateData();

    switch ( m_action )
        {
        case 0:
            {
            fmi_sensor_config_request config;
            memset( &config, 0, sizeof( config ) );
            config.change_id = GetTickCount();
            config.unique_id = m_uniqueId;
            config.display_index = m_sortOrder;
            config.icon = m_icon.GetCurSel();

            WideCharToMultiByte( mCom.mClientCodepage, 0, m_name.GetBuffer(), -1, config.name, sizeof( config.name ), NULL, NULL );
            config.name[sizeof( config.name ) - 1] = '\0';

            mCom.sendFmiPacket( FMI_SENSOR_CONFIG_REQUEST, (uint8*)&config, sizeof( config ) );
            break;
            }
        case 1:
            {
            fmi_sensor_update_request update;
            memset( &update, 0, sizeof( update ) );
            update.change_id = GetTickCount();
            update.unique_id = m_uniqueId;
            update.severity = m_severity.GetCurSel();
            update.play_sound = m_playSound;
            update.record_sensor = m_recordInHistory;
            WideCharToMultiByte( mCom.mClientCodepage, 0, m_status.GetBuffer(), -1, update.status, sizeof( update.status ), NULL, NULL );
            update.status[sizeof( update.status ) - 1] = '\0';
            WideCharToMultiByte( mCom.mClientCodepage, 0, m_description.GetBuffer(), -1, update.description, sizeof( update.description ), NULL, NULL );
            update.description[sizeof( update.description ) - 1] = '\0';

            mCom.sendFmiPacket( FMI_SENSOR_UPDATE_REQUEST, (uint8*)&update, sizeof( update ) );
            break;
            }
        case 2:
            {
            fmi_sensor_delete_request message;
            memset( &message, 0, sizeof( message ) );
            message.change_id = GetTickCount();
            message.unique_id = m_uniqueId;

            mCom.sendFmiPacket( FMI_SENSOR_DELETE_REQUEST, (uint8*)&message, sizeof( message ) );

            break;
            }
        case 3:
            {
            fmi_sensor_query_display_index_request message;
            memset( &message, 0, sizeof( message ) );
            message.change_id = GetTickCount();
            message.unique_id = m_uniqueId;

            mCom.sendFmiPacket( FMI_SENSOR_QUERY_DISPLAY_INDEX_REQUEST, (uint8*)&message, sizeof( message ) );

            break;
            }
        }

    OnOK();
    }

//----------------------------------------------------------------------
//! \brief Update the name length label in response to change in name
//----------------------------------------------------------------------
void CSendSensorDlg::OnEnChangeSensorName()
    {
    UpdateData();

    fmi_sensor_config_request message;
    char text[ sizeof( message.name ) ];
    WideCharToMultiByte( mCom.mClientCodepage, 0, m_name.GetBuffer(), -1, text, sizeof( text ), NULL, NULL );
    text[sizeof( text ) - 1] = '\0';

    m_nameLength = strlen( text );

    UpdateData( FALSE );
    }

//----------------------------------------------------------------------
//! \brief Update the status length label in response to change in status
//----------------------------------------------------------------------
void CSendSensorDlg::OnEnChangeSensorStatus()
    {
    UpdateData();

    fmi_sensor_update_request message;
    char text[ sizeof( message.status ) ];
    WideCharToMultiByte( mCom.mClientCodepage, 0, m_status.GetBuffer(), -1, text, sizeof( text ), NULL, NULL );
    text[sizeof( text ) - 1] = '\0';

    m_statusLength = strlen( text );

    UpdateData( FALSE );
    }

//----------------------------------------------------------------------
//! \brief Update the description length label in response to change in description
//----------------------------------------------------------------------
void CSendSensorDlg::OnEnChangeSensorDescription()
    {
    UpdateData();

    fmi_sensor_update_request message;
    char text[ sizeof( message.description ) ];
    WideCharToMultiByte( mCom.mClientCodepage, 0, m_description.GetBuffer(), -1, text, sizeof( text ), NULL, NULL );
    text[sizeof( text ) - 1] = '\0';

    m_descriptionLength = strlen( text );

    UpdateData( FALSE );
    }

#endif

void CSendSensorDlg::OnActionChanged()
    {
    UpdateData();

    bool adding = 0 == m_action;
    bool updating = 1 == m_action;

    GetDlgItem(IDC_SENSOR_NAME)->EnableWindow( adding );
    GetDlgItem(IDC_SENSOR_SORT_ORDER)->EnableWindow( adding );
    GetDlgItem(IDC_SENSOR_ICON)->EnableWindow( adding );
    GetDlgItem(IDC_SENSOR_SEVERITY)->EnableWindow( updating );
    GetDlgItem(IDC_SENSOR_PLAY_SOUND)->EnableWindow( updating );
    GetDlgItem(IDC_SENSOR_RECORD_IN_HISTORY)->EnableWindow( updating );
    GetDlgItem(IDC_SENSOR_STATUS)->EnableWindow( updating );
    GetDlgItem(IDC_SENSOR_DESCRIPTION)->EnableWindow( updating );
    }
