/*********************************************************************
*
*   MODULE NAME:
*       CStopNewDlg.cpp
*
*   Copyright 2008-2009 by Garmin Ltd. or its subsidiaries.
*---------------------------------------------------------------------
* $NoKeywords$
*********************************************************************/
#include "stdafx.h"
#include "CFmiApplication.h"
#include "CStopNewDlg.h"
#include "util.h"

#if( FMI_SUPPORT_A618 )
#define LONG_STOP_TEMP_FILE _T( "long_stop_temp.bin" )
#endif

IMPLEMENT_DYNAMIC( CStopNewDlg, CDialog )

BEGIN_MESSAGE_MAP( CStopNewDlg, CDialog )
    ON_BN_CLICKED( IDOK, OnBnClickedOk )
    ON_EN_CHANGE( IDC_STOPNEW_EDIT_LAT, OnFormChanged )
    ON_EN_CHANGE( IDC_STOPNEW_EDIT_LON, OnFormChanged )
    ON_EN_CHANGE( IDC_STOPNEW_EDIT_NAME, OnFormChanged )
    ON_BN_CLICKED( IDC_STOPNEW_RDO_LEGACY, OnFormChanged )
#if( FMI_SUPPORT_A602 )
    ON_BN_CLICKED( IDC_STOPNEW_RDO_A602, OnFormChanged )
#endif
#if( FMI_SUPPORT_A603 )
    ON_EN_CHANGE( IDC_STOPNEW_EDIT_STOPID, OnFormChanged )
    ON_BN_CLICKED( IDC_STOPNEW_RDO_A603, OnFormChanged )
#endif
#if( FMI_SUPPORT_A618 )
    ON_EN_CHANGE( IDC_STOPNEW_EDIT_STOPID, OnFormChanged )
    ON_BN_CLICKED( IDC_STOPNEW_RDO_A618, OnFormChanged )
#endif
END_MESSAGE_MAP()

//----------------------------------------------------------------------
//! \brief Constructor
//! \param aParent The parent of this dialog
//! \param aCom Reference to the FMI communication controller
//----------------------------------------------------------------------
CStopNewDlg::CStopNewDlg
    (
    CWnd                * aParent,
    FmiApplicationLayer & aCom
    )
    : CDialog( IDD_STOP_NEW, aParent )
    , mCom( aCom )
    , mLatitudeStr( _T("") )
    , mLongitudeStr( _T("") )
    , mMessageStr( _T("") )
    , mCompressLongStop( TRUE )
{
#if( FMI_SUPPORT_A618 )
    mStopProtocol = STOP_PROTOCOL_A618;
#elif( FMI_SUPPORT_A603 )
    mStopProtocol = STOP_PROTOCOL_A603;
#elif( FMI_SUPPORT_A602 )
    mStopProtocol = STOP_PROTOCOL_A602;
#else
    mStopProtocol = STOP_PROTOCOL_LEGACY;
#endif
}

//----------------------------------------------------------------------
//! \brief Destructor
//----------------------------------------------------------------------
CStopNewDlg::~CStopNewDlg()
{
}

//----------------------------------------------------------------------
//! \brief Perform dialog data exchange and validation
//! \param  aDataExchange The DDX context
//----------------------------------------------------------------------
void CStopNewDlg::DoDataExchange
    (
    CDataExchange * aDataExchange
    )
{
    CDialog::DoDataExchange( aDataExchange );

    DDX_Text( aDataExchange, IDC_STOPNEW_EDIT_LAT, mLatitudeStr );
    DDX_Text( aDataExchange, IDC_STOPNEW_EDIT_LON, mLongitudeStr );
    DDX_Text( aDataExchange, IDC_STOPNEW_EDIT_NAME, mMessageStr );
    DDX_Radio( aDataExchange, IDC_STOPNEW_RDO_A618, mStopProtocol );
#if( FMI_SUPPORT_A603 )
    DDX_Text( aDataExchange, IDC_STOPNEW_EDIT_STOPID, mStopId );
#endif
    DDX_Check( aDataExchange, IDC_COMPRESS_LONG_STOP, mCompressLongStop );
}

//----------------------------------------------------------------------
//! \brief Initialize the dialog
//! \details This function is called when the window is created. It
//!     sets up the parent, so it can get info from and send a message
//!     to FmiApplicationLayer.
//! \return TRUE, since this function does not set focus to a control
//----------------------------------------------------------------------
BOOL CStopNewDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
#if( !FMI_SUPPORT_A618 )
    CButton * rdo618 = (CButton *)GetDlgItem( IDC_STOPNEW_RDO_A618 );
    rdo618->SetCheck( BST_UNCHECKED );
    rdo618->EnableWindow( FALSE );
    GetDlgItem( IDC_COMPRESS_LONG_STOP )->EnableWindow( FALSE );
#endif

#if( FMI_SUPPORT_A603 )
    mStopId.Format( _T("%u"), mCom.getNextStopId() );
#endif
#if( !FMI_SUPPORT_A603 )
    CButton * rdo603 = (CButton *)GetDlgItem( IDC_STOPNEW_RDO_A603 );
    rdo603->SetCheck( BST_UNCHECKED );
    rdo603->EnableWindow( FALSE );
    GetDlgItem( IDC_STOPNEW_EDIT_STOPID )->EnableWindow( FALSE );
#endif
#if( !FMI_SUPPORT_A602 )
    CButton * rdo602 = (CButton *)GetDlgItem( IDC_STOPNEW_RDO_A602 );
    rdo602->SetCheck( BST_UNCHECKED );
    rdo602->EnableWindow( FALSE );
#endif
#if( !FMI_SUPPORT_LEGACY )
    CButton * rdoLegacy = (CButton *)GetDlgItem( IDC_STOPNEW_RDO_LEGACY );
    rdoLegacy->SetCheck( BST_UNCHECKED );
    rdoLegacy->EnableWindow( FALSE );
#endif
    UpdateData( FALSE );
    return TRUE;
}   /* OnInitDialog() */

//----------------------------------------------------------------------
//! \brief Change handler for all radio buttons and edit boxes on this
//!     dialog
//! \details Validate all required fields, and enable the OK button
//!     if all fields are present.
//----------------------------------------------------------------------
void CStopNewDlg::OnFormChanged()
{
    bool formIsValid = true;

#if !SKIP_VALIDATION
    char stopIdString[11];
    memset( stopIdString, 0, sizeof( stopIdString ) );

    //convert LF and CR+LF to CR since the serialization framework delimits records by LF
    mMessageStr.Replace( _T( "\r\n" ), _T( "\r" ) );
    mMessageStr.Replace( _T( "\n" ), _T( "\r" ) );

    UpdateData( TRUE );
    if( mLatitudeStr == "" || _tstof( mLatitudeStr ) < -90 || _tstof( mLatitudeStr ) >= 90 )
        formIsValid = false;
    else if( mLongitudeStr == "" || _tstof( mLongitudeStr ) < -180 || _tstof( mLongitudeStr ) >= 180 )
        formIsValid = false;
    else if( mMessageStr.GetLength() == 0 )
        formIsValid = false;
    else
    {
        switch ( mStopProtocol )
        {
            case STOP_PROTOCOL_A602:
                formIsValid = ( mMessageStr.GetLength() < 51 );
                break;
#if( FMI_SUPPORT_A603 )
            case STOP_PROTOCOL_A603:
                formIsValid = ( mMessageStr.GetLength() < 200 );
                break;
#endif
#if( FMI_SUPPORT_A618 )
            case STOP_PROTOCOL_A618:
                formIsValid = ( mMessageStr.GetLength() < 2000 );
                break;
#endif
            }
    }

    BOOL stopIdEnabled = FALSE;
    GetDlgItem( IDC_COMPRESS_LONG_STOP )->EnableWindow( FALSE );
#if( FMI_SUPPORT_A618 )
    if( mStopProtocol == STOP_PROTOCOL_A618 )
    {
        GetDlgItem( IDC_COMPRESS_LONG_STOP )->EnableWindow( TRUE );
        stopIdEnabled = TRUE;
    }
#endif

#if( FMI_SUPPORT_A603 )
    if( mStopProtocol == STOP_PROTOCOL_A603 )
    {
        stopIdEnabled = TRUE;
    }
    if( 0 != WideCharToMultiByte( CP_ACP, 0, mStopId.GetBuffer(), mStopId.GetLength(), stopIdString, sizeof( stopIdString ), NULL, NULL ) )
    {
        if( !UTIL_data_is_uint32( stopIdString ) )
        {
            formIsValid = false;
        }

    }
    else
    {
        formIsValid = false;
    }
    #endif
#endif

    GetDlgItem( IDC_STOPNEW_EDIT_STOPID )->EnableWindow( stopIdEnabled );
    GetDlgItem( IDC_STOPNEW_LBL_STOPID )->EnableWindow( stopIdEnabled );

    GetDlgItem( IDOK )->EnableWindow( formIsValid );

}   /* OnFormChanged */

//----------------------------------------------------------------------
//! \brief Click handler for the OK button
//! \details Gets the detail from the form and sends a new stop to the
//!     client using the mStopProtocol specified by the user.
//----------------------------------------------------------------------
void CStopNewDlg::OnBnClickedOk()
{
    double lat;
    double lon;
    char message[2000];
#if( FMI_SUPPORT_A603 )
    uint32 stopId;
#endif

    UpdateData( TRUE );
    //convert LF and CR+LF to CR since the serialization framework delimits records by LF
    mMessageStr.Replace( _T( "\r\n" ), _T( "\r" ) );
    mMessageStr.Replace( _T( "\n" ), _T( "\r" ) );
    WideCharToMultiByte( mCom.mClientCodepage, 0, mMessageStr.GetBuffer(), -1, message, 2000, NULL, NULL );
    message[1999] = '\0';
    lat = _tstof( mLatitudeStr.GetBuffer() );
    lon = _tstof( mLongitudeStr.GetBuffer() );
    switch( mStopProtocol )
        {
#if( FMI_SUPPORT_A618 )
        case STOP_PROTOCOL_A618: {// A618
            if( mStopId == "" )
                stopId = mCom.getNextStopId();
            else
                stopId = _tstoi( mStopId.GetBuffer() );

            CFile tmpFile;
            if ( !tmpFile.Open( LONG_STOP_TEMP_FILE, CFile::modeCreate | CFile::modeWrite ) )
                {
                MessageBox( _T( "Unable to open temp file for building long stop payload" ), _T( "Send Stop" ), MB_ICONWARNING | MB_OK );
                return;
                }

            short ver = 0;
            time_type origination_time = UTIL_get_current_garmin_time();
            sc_position_type pos;
            pos.lat = UTIL_convert_degrees_to_semicircles( lat );
            pos.lon = UTIL_convert_degrees_to_semicircles( lon );

            tmpFile.Write( ( const void* ) "STOP", 4 );
            tmpFile.Write( ( const void* ) &ver, sizeof( ver ) );
            tmpFile.Write( ( const void* ) &origination_time, sizeof( origination_time ) );
            tmpFile.Write( ( const void* ) &stopId, sizeof( stopId ) );
            tmpFile.Write( ( const void* ) message, ( UINT ) strlen( message ) + 1 );
            tmpFile.Write( ( const void* ) &pos.lat, sizeof( pos.lat ) );
            tmpFile.Write( ( const void* ) &pos.lon, sizeof( pos.lon ) );
            tmpFile.Flush();
            tmpFile.Close();

            char file[MAX_PATH+1];
            WideCharToMultiByte( CP_ACP, 0, LONG_STOP_TEMP_FILE, -1, file, MAX_PATH+1, NULL, NULL );
            file[MAX_PATH] = '\0';

            char    versionString[35];
            uint8   version[16];
            uint8   versionLength;

            memset( version, 0, sizeof( version ) );
            WideCharToMultiByte( mCom.mClientCodepage, 0, _T( "1" ), -1, versionString, 34, NULL, NULL );
            versionString[34] = '\0';
            versionLength = (uint8)minval( 16, strlen( versionString ) );
            memmove( version, versionString, versionLength );

            mCom.sendFile( file, versionLength, version, (uint8) FMI_FILE_TYPE_LONG_STOP, mCompressLongStop );

            StopListItem stopListItem;
            stopListItem.setId( stopId );
            stopListItem.setCurrentName( mMessageStr );
            stopListItem.setStopStatus( STOP_STATUS_UNREAD );
            stopListItem.setValid();
            mCom.mA603Stops.put( stopListItem );
            break;
        }
#endif
#if( FMI_SUPPORT_A603 )
        case STOP_PROTOCOL_A603: // A603
            if( mStopId == "" )
                stopId = mCom.getNextStopId();
            else
                stopId = _tstoi( mStopId.GetBuffer() );
            mCom.sendA603Stop( lat, lon, message, stopId );
            break;
#endif
#if( FMI_SUPPORT_A602 )
        case STOP_PROTOCOL_A602: // A602
            mCom.sendA602Stop( lat, lon, message );
            break;
#endif
#if( FMI_SUPPORT_LEGACY )
        case STOP_PROTOCOL_LEGACY: // Legacy
            mCom.sendLegacyStop( lat, lon, message );
            break;
#endif
        default:
            break;
        }

    OnOK();
}   /* OnBnClickedOk() */
