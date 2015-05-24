/*********************************************************************
*
*   MODULE NAME:
*       CAobrdDriverShipmentsDlg.cpp
*
*   Copyright 2012 by Garmin Ltd. or its subsidiaries.
*---------------------------------------------------------------------
* $NoKeywords$
*********************************************************************/
#include "stdafx.h"
#include "CFmiApplication.h"
#include "CAobrdDriverShipmentsDlg.h"
#include "util.h"
#include "garmin_types.h"

using namespace std;

IMPLEMENT_DYNAMIC(CAobrdDriverShipmentsDlg, CDialog)

BEGIN_MESSAGE_MAP( CAobrdDriverShipmentsDlg, CDialog )
    ON_BN_CLICKED( IDC_DRIVER_SHIPMENTS_BTN_NEW, OnBnClickedNew )
    ON_BN_CLICKED( IDC_DRIVER_SHIPMENTS_BTN_DELETE, OnBnClickedDelete )
    ON_BN_CLICKED( IDC_DRIVER_SHIPMENTS_BTN_SET, OnBnClickedSet )
    ON_EN_CHANGE( IDC_DRIVER_SHIPMENTS_EDIT_SHIPPER, OnChangeShipmentData )
    ON_EN_CHANGE( IDC_DRIVER_SHIPMENTS_EDIT_DOC_NUM, OnChangeShipmentData )
    ON_EN_CHANGE( IDC_DRIVER_SHIPMENTS_EDIT_COMMODITY, OnChangeShipmentData )
    ON_EN_CHANGE( IDC_DRIVER_SHIPMENTS_TIMESTAMP_DATE, OnChangeShipmentData )
    ON_EN_CHANGE( IDC_DRIVER_SHIPMENTS_TIMESTAMP_TIME, OnChangeShipmentData )
    ON_EN_CHANGE( IDC_DRIVER_SHIPMENTS_START_TIME_DATE, OnChangeShipmentData )
    ON_EN_CHANGE( IDC_DRIVER_SHIPMENTS_START_TIME_TIME, OnChangeShipmentData )
    ON_EN_CHANGE( IDC_DRIVER_SHIPMENTS_END_TIME_DATE, OnChangeShipmentData )
    ON_EN_CHANGE( IDC_DRIVER_SHIPMENTS_END_TIME_TIME, OnChangeShipmentData )
    ON_BN_CLICKED( IDOK, OnBnClickedOk )
    ON_EN_SETFOCUS( IDC_DRIVER_SHIPMENTS_EDIT_SHIPPER, OnEnSetfocusLoginEdit )
    ON_EN_KILLFOCUS( IDC_DRIVER_SHIPMENTS_EDIT_SHIPPER, OnEnKillfocusLoginEdit )
    ON_EN_SETFOCUS( IDC_DRIVER_SHIPMENTS_EDIT_DOC_NUM, OnEnSetfocusLoginEdit )
    ON_EN_KILLFOCUS( IDC_DRIVER_SHIPMENTS_EDIT_DOC_NUM, OnEnKillfocusLoginEdit )
    ON_EN_SETFOCUS( IDC_DRIVER_SHIPMENTS_EDIT_COMMODITY, OnEnSetfocusLoginEdit )
    ON_EN_KILLFOCUS( IDC_DRIVER_SHIPMENTS_EDIT_COMMODITY, OnEnKillfocusLoginEdit )
    ON_NOTIFY( NM_SETFOCUS, IDC_DRIVER_SHIPMENTS_TIMESTAMP_DATE, OnDateTimeCtrlSetFocus )
    ON_NOTIFY( NM_KILLFOCUS, IDC_DRIVER_SHIPMENTS_TIMESTAMP_DATE, OnDateTimeCtrlKillFocus )
    ON_NOTIFY( NM_SETFOCUS, IDC_DRIVER_SHIPMENTS_TIMESTAMP_TIME, OnDateTimeCtrlSetFocus )
    ON_NOTIFY( NM_KILLFOCUS, IDC_DRIVER_SHIPMENTS_TIMESTAMP_TIME, OnDateTimeCtrlKillFocus )
    ON_NOTIFY( NM_SETFOCUS, IDC_DRIVER_SHIPMENTS_START_TIME_DATE, OnDateTimeCtrlSetFocus )
    ON_NOTIFY( NM_KILLFOCUS, IDC_DRIVER_SHIPMENTS_START_TIME_DATE, OnDateTimeCtrlKillFocus )
    ON_NOTIFY( NM_SETFOCUS, IDC_DRIVER_SHIPMENTS_START_TIME_TIME, OnDateTimeCtrlSetFocus )
    ON_NOTIFY( NM_KILLFOCUS, IDC_DRIVER_SHIPMENTS_START_TIME_TIME, OnDateTimeCtrlKillFocus )
    ON_NOTIFY( NM_SETFOCUS, IDC_DRIVER_SHIPMENTS_END_TIME_DATE, OnDateTimeCtrlSetFocus )
    ON_NOTIFY( NM_KILLFOCUS, IDC_DRIVER_SHIPMENTS_END_TIME_DATE, OnDateTimeCtrlKillFocus )
    ON_NOTIFY( NM_SETFOCUS, IDC_DRIVER_SHIPMENTS_END_TIME_TIME, OnDateTimeCtrlSetFocus )
    ON_NOTIFY( NM_KILLFOCUS, IDC_DRIVER_SHIPMENTS_END_TIME_TIME, OnDateTimeCtrlKillFocus )
    ON_LBN_SELCHANGE( IDC_DRIVER_SHIPMENTS_LST_SHIPMENTS, OnLbnSelchangeShipmentList )
    ON_LBN_SETFOCUS( IDC_DRIVER_SHIPMENTS_LST_SHIPMENTS, OnLbnSetfocusShipmentList )
    ON_LBN_KILLFOCUS( IDC_DRIVER_SHIPMENTS_LST_SHIPMENTS, OnLbnKillfocusShipmentList )
END_MESSAGE_MAP()

//----------------------------------------------------------------------
//! \brief Main entry point for showing the shipments editor
//! \param  aParent The parent window.
//! \param aDriver Reference to the driver being edited
//----------------------------------------------------------------------
BOOL CAobrdDriverShipmentsDlg::ShowShipments( CWnd * aParent, AobrdDriverItem * aDriver )
{
    CAobrdDriverShipmentsDlg dlg( aParent, aDriver );
    INT_PTR r = dlg.DoModal();
    return ( IDOK == r );
}

//----------------------------------------------------------------------
//! \brief Constructor
//! \param  aParent The parent window.
//! \param aDriver Reference to the driver being edited
//----------------------------------------------------------------------
CAobrdDriverShipmentsDlg::CAobrdDriverShipmentsDlg
    (
    CWnd                    * aParent,
    AobrdDriverItem   * aDriver
    )
    : CDialog( IDD_AOBRD_DRIVER_SHIPMENTS, aParent )
    , mDriver( aDriver )
    , mSelectedIndex( -1 )
    , mShipperName( _T("") )
    , mDocumentNumber( _T("") )
    , mCommodity( _T("") )
    , mTimestampDate( COleDateTime::GetCurrentTime() )
    , mTimestampTime( COleDateTime::GetCurrentTime() )
    , mStartTimeDate( COleDateTime::GetCurrentTime() )
    , mStartTimeTime( COleDateTime::GetCurrentTime() )
    , mEndTimeDate( COleDateTime::GetCurrentTime() )
    , mEndTimeTime( COleDateTime::GetCurrentTime() )
{
}

//----------------------------------------------------------------------
//! \brief Destructor
//----------------------------------------------------------------------
CAobrdDriverShipmentsDlg::~CAobrdDriverShipmentsDlg()
{
}

//----------------------------------------------------------------------
//! \brief Perform dialog data exchange and validation
//! \param  aDataExchange The DDX context
//----------------------------------------------------------------------
void CAobrdDriverShipmentsDlg::DoDataExchange
    (
    CDataExchange * aDataExchange
    )
{
    CDialog::DoDataExchange( aDataExchange );
    DDX_Control( aDataExchange, IDC_DRIVER_SHIPMENTS_LST_SHIPMENTS, mListBox );
    DDX_Text( aDataExchange, IDC_DRIVER_SHIPMENTS_EDIT_SHIPPER, mShipperName );
    DDX_Text( aDataExchange, IDC_DRIVER_SHIPMENTS_EDIT_DOC_NUM, mDocumentNumber );
    DDX_Text( aDataExchange, IDC_DRIVER_SHIPMENTS_EDIT_COMMODITY, mCommodity );
    DDX_DateTimeCtrl( aDataExchange, IDC_DRIVER_SHIPMENTS_TIMESTAMP_DATE, mTimestampDate );
    DDX_DateTimeCtrl( aDataExchange, IDC_DRIVER_SHIPMENTS_TIMESTAMP_TIME, mTimestampTime );
    DDX_DateTimeCtrl( aDataExchange, IDC_DRIVER_SHIPMENTS_START_TIME_DATE, mStartTimeDate );
    DDX_DateTimeCtrl( aDataExchange, IDC_DRIVER_SHIPMENTS_START_TIME_TIME, mStartTimeTime );
    DDX_DateTimeCtrl( aDataExchange, IDC_DRIVER_SHIPMENTS_END_TIME_DATE, mEndTimeDate );
    DDX_DateTimeCtrl( aDataExchange, IDC_DRIVER_SHIPMENTS_END_TIME_TIME, mEndTimeTime );
}   /* DoDataExchange */

//----------------------------------------------------------------------
//! \brief This function is called when the window is created.
//! \details This function is called when the window is created. It
//!     initializes the shipments list, and sets the initial position
//!     of the window.
//! \return TRUE, since this function does not set focus to a control
//----------------------------------------------------------------------
BOOL CAobrdDriverShipmentsDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    updateListBox();

    GetDlgItem( IDC_DRIVER_SHIPMENTS_BTN_NEW )->EnableWindow( FALSE );
    GetDlgItem( IDC_DRIVER_SHIPMENTS_BTN_DELETE )->EnableWindow( FALSE );

    SetWindowPos( NULL, 700, 350, 0, 0, SWP_NOSIZE | SWP_NOZORDER );

    ( ( CEdit* ) GetDlgItem( IDC_DRIVER_SHIPMENTS_EDIT_SHIPPER ) )->LimitText( 39 );
    ( ( CEdit* ) GetDlgItem( IDC_DRIVER_SHIPMENTS_EDIT_DOC_NUM ) )->LimitText( 39 );
    ( ( CEdit* ) GetDlgItem( IDC_DRIVER_SHIPMENTS_EDIT_COMMODITY ) )->LimitText( 39 );

    CString title;
    title.Format( _T( "AOBRD Driver Shipments for %s %s (%s)" ), mDriver->getFirstName(), mDriver->getLastName(), mDriver->getDriverId() );
    SetWindowText( title );

    return TRUE;
} /* OnInitDialog */

//----------------------------------------------------------------------
//! \brief Clear values from the editable fields.
//----------------------------------------------------------------------
void CAobrdDriverShipmentsDlg::clearData()
{
    mShipperName = _T( "" );
    mDocumentNumber = _T( "" );
    mCommodity = _T( "" );
    mTimestampDate = COleDateTime::GetCurrentTime();
    mTimestampTime = COleDateTime::GetCurrentTime();
    mStartTimeDate = COleDateTime::GetCurrentTime();
    mStartTimeTime = COleDateTime::GetCurrentTime();
    mEndTimeDate = COleDateTime::GetCurrentTime();
    mEndTimeTime = COleDateTime::GetCurrentTime();
} /* clearData */

//----------------------------------------------------------------------
//! \brief Update the shipments list box from the list owned by the
//!     driver.
//----------------------------------------------------------------------
void CAobrdDriverShipmentsDlg::updateListBox()
{
    CString listItem;

    //must keep track of where the list was scrolled to
    //since we reset content we must reinitialize these
    mSelectedIndex = mListBox.GetCurSel();
    int topIndex = mListBox.GetTopIndex();

    //reset content and then add current drivers
    mListBox.ResetContent();

    std::vector<AobrdShipment> shipments = mDriver->getShipments();
    std::vector<AobrdShipment>::iterator iter;

    for( iter = shipments.begin(); iter != shipments.end(); ++iter )
    {
        AobrdShipment shipment = ( *iter );
        listItem.Format( _T( "%s, %s, %s" ), shipment.getShipperName(), shipment.getDocNumber(), shipment.getCommodity() );
        mListBox.AddString( listItem );
    }

    //reset scroll and selection
    mSelectedIndex = -1;

    //reset scroll and selection
    mListBox.SetCurSel( mSelectedIndex );
    mListBox.SetTopIndex( topIndex );

}   /* updateListBox */

//----------------------------------------------------------------------
//! \brief Button handler for the New button
//! \details Clears the edit controls and list box selection in order
//!     to allow a new item to be created
//----------------------------------------------------------------------
void CAobrdDriverShipmentsDlg::OnBnClickedNew()
{
    UpdateData( TRUE );

    mSelectedIndex = -1;

    clearData();

    mListBox.SetCurSel( mSelectedIndex );

    GetDlgItem( IDC_DRIVER_SHIPMENTS_BTN_NEW )->EnableWindow( FALSE );
    GetDlgItem( IDC_DRIVER_SHIPMENTS_BTN_DELETE )->EnableWindow( FALSE );
    GetDlgItem( IDC_DRIVER_SHIPMENTS_BTN_SET )->EnableWindow( FALSE );

    UpdateData( FALSE );
}   /* OnBnClickedNew */

//----------------------------------------------------------------------
//! \brief Button handler for the Delete button
//! \details Remove the selected driver from the allowed logins.
//----------------------------------------------------------------------
void CAobrdDriverShipmentsDlg::OnBnClickedDelete()
{
    UpdateData( TRUE );
    if( mSelectedIndex >= 0 )
    {
        mDriver->deleteShipment( mSelectedIndex );
        updateListBox();
    }

    if( mSelectedIndex < 0 )
    {
        GetDlgItem( IDC_DRIVER_SHIPMENTS_BTN_NEW )->EnableWindow( FALSE );
        GetDlgItem( IDC_DRIVER_SHIPMENTS_BTN_DELETE )->EnableWindow( FALSE );
        GetDlgItem( IDC_DRIVER_SHIPMENTS_BTN_SET )->EnableWindow( FALSE );
        clearData();
    }

    UpdateData(FALSE);

}   /* OnBnClickedDelete */

//----------------------------------------------------------------------
//! \brief Convert COleDateTime pairs to time_type
//----------------------------------------------------------------------
static time_type convertToTimeType( COleDateTime & datePart, COleDateTime & timePart )
{
    date_time_data_type date_time;
    time_type time, date;
    time_type timestamp;
    date_time.date.year = ( uint16 ) ( 0xFFFF & datePart.GetYear() );
    date_time.date.month = ( uint8 ) ( 0x00FF & datePart.GetMonth() );
    date_time.date.day = ( uint8 ) ( 0x00FF & datePart.GetDay() );
    date_time.time.hour = ( sint16 ) ( 0xFFFF & timePart.GetHour() );
    date_time.time.minute = ( uint8 ) ( 0x00FF & timePart.GetMinute() );
    date_time.time.second = ( uint8 ) ( 0x00FF & timePart.GetSecond() );
    UTIL_convert_time_type_to_seconds( &date_time, &time );
    UTIL_convert_date_time_to_seconds( &date_time, &date );
    timestamp = date + time;
    UTIL_convert_local_to_UTC( &timestamp, &timestamp );
    return timestamp;
}   /* convertToTimeType */

//----------------------------------------------------------------------
//! \brief Convert time_type to COleDateTime for use with CDateTimeCtrl
//----------------------------------------------------------------------
static COleDateTime convertFromTimeType( time_type aTimestamp )
{
    date_time_data_type date_time;
    UTIL_convert_UTC_to_local( &aTimestamp, &aTimestamp );
    UTIL_convert_seconds_to_date_type( &aTimestamp, &date_time );
    UTIL_convert_seconds_to_time_type( &aTimestamp, &date_time );
    if( 0 == date_time.time.hour )
        {
         date_time.time.hour = 12;
        }

    COleDateTime dt(
        date_time.date.year, date_time.date.month, date_time.date.day,
        date_time.time.hour, date_time.time.minute, date_time.time.second
    );

    return dt;
}   /* convertFromTimeType */

//----------------------------------------------------------------------
//! \brief Button handler for the Set button
//! \details Saves the new shipment info
//----------------------------------------------------------------------
void CAobrdDriverShipmentsDlg::OnBnClickedSet()
{
    UpdateData( TRUE );

    AobrdShipment shipment;
    if( 0 <= mSelectedIndex )
        {
        shipment = mDriver->getShipments().at( mSelectedIndex );
        }

    char shipperName[40];
    char docNumber[40];
    char commodity[40];

    WideCharToMultiByte( CP_UTF8, 0, mShipperName, -1, shipperName, sizeof( shipperName ), NULL, NULL );
    WideCharToMultiByte( CP_UTF8, 0, mDocumentNumber, -1, docNumber, sizeof( docNumber ), NULL, NULL );
    WideCharToMultiByte( CP_UTF8, 0, mCommodity, -1, commodity, sizeof( commodity ), NULL, NULL );

    shipment.setShipperName( shipperName );
    shipment.setDocNumber( docNumber );
    shipment.setCommodity( commodity );

    shipment.setServerTimestamp( convertToTimeType( mTimestampDate, mTimestampTime ) );
    shipment.setServerStartTime( convertToTimeType( mStartTimeDate, mStartTimeTime ) );
    shipment.setServerEndTime( convertToTimeType( mEndTimeDate, mEndTimeTime ) );

    if( 0 <= mSelectedIndex )
        {
        mDriver->replaceShipment( shipment, mSelectedIndex );
        }
    else
        {
        mDriver->addShipment( shipment );
        }

    updateListBox();
    clearData();
    UpdateData( FALSE );

}   /* OnBnClickedSet */

//----------------------------------------------------------------------
//! \brief Edit/Change handler for the shipment-specific data controls.
//! \details If any of controls are unset, disables the Set button; if all
//!     are specified, enables the Set button.
//----------------------------------------------------------------------
void CAobrdDriverShipmentsDlg::OnChangeShipmentData()
{
    UpdateData( TRUE );
    if( ( "" != mShipperName ) && ( "" != mDocumentNumber ) &&
        ( "" != mCommodity ) &&
        ( COleDateTime::valid == mTimestampDate.GetStatus() ) && ( COleDateTime::valid == mTimestampTime.GetStatus() ) &&
        ( COleDateTime::valid == mStartTimeDate.GetStatus() ) && ( COleDateTime::valid == mStartTimeTime.GetStatus() ) &&
        ( COleDateTime::valid == mEndTimeDate.GetStatus() )   && ( COleDateTime::valid == mEndTimeTime.GetStatus() ) )
    {
        GetDlgItem( IDC_DRIVER_SHIPMENTS_BTN_SET )->EnableWindow( TRUE );
    }
    else
    {
        GetDlgItem( IDC_DRIVER_SHIPMENTS_BTN_SET )->EnableWindow( FALSE );
    }
}   /* OnChangeShipmentData */


//----------------------------------------------------------------------
//! \brief Selection Changed handler for the Shipments List box.
//! \details Fills in the fields of the dialog with the information from
//!     the selected list item, for easy editing.
//----------------------------------------------------------------------
void CAobrdDriverShipmentsDlg::OnLbnSelchangeShipmentList()
{
    CListBox * listBox = ( CListBox* ) GetDlgItem( IDC_DRIVER_SHIPMENTS_LST_SHIPMENTS );
    mSelectedIndex = listBox->GetCurSel();

    if( mSelectedIndex >= 0 && mSelectedIndex < listBox->GetCount() )
    {
        AobrdShipment shipment = mDriver->getShipments().at( mSelectedIndex );

        mShipperName = shipment.getShipperName();
        mDocumentNumber = shipment.getDocNumber();
        mCommodity = shipment.getCommodity();

        mTimestampDate = mTimestampTime = convertFromTimeType( shipment.getServerTimestamp() );
        mStartTimeDate = mStartTimeTime = convertFromTimeType( shipment.getServerStartTime() );
        mEndTimeDate = mEndTimeTime     = convertFromTimeType( shipment.getServerEndTime() );

        UpdateData( FALSE );
        GetDlgItem( IDC_DRIVER_SHIPMENTS_BTN_NEW )->EnableWindow( TRUE );
        GetDlgItem( IDC_DRIVER_SHIPMENTS_BTN_DELETE )->EnableWindow( TRUE );

        OnChangeShipmentData();
    }
    else
    {
        GetDlgItem( IDC_DRIVER_SHIPMENTS_BTN_NEW )->EnableWindow( FALSE );
        GetDlgItem( IDC_DRIVER_SHIPMENTS_BTN_DELETE )->EnableWindow( FALSE );
    }

    listBox->SetCurSel( mSelectedIndex );
}   /* OnLbnSelchangeShipmentList */

//----------------------------------------------------------------------
//! \brief Button handler for the OK button
//! \details Closes the window.
//----------------------------------------------------------------------
void CAobrdDriverShipmentsDlg::OnBnClickedOk()
{
    CDialog::OnOK();
}   /* OnBnClickedOk */

//----------------------------------------------------------------------
//! \brief Handler for the Cancel action
//! \details Closes the window.
//----------------------------------------------------------------------
void CAobrdDriverShipmentsDlg::OnCancel()
{
    CDialog::OnCancel();
}   /* OnCancel */

//----------------------------------------------------------------------
//! \brief Called by MFC after the window has been destroyed; performs
//!     final termination activities.
//----------------------------------------------------------------------
void CAobrdDriverShipmentsDlg::PostNcDestroy()
{
    CDialog::PostNcDestroy();
}   /* PostNcDestroy */

//----------------------------------------------------------------------
//! \brief Handles the set focus event for the driver ID and password
//!     edit boxes.
//! \details Sets the default control to the Set button so that it is
//!     activated if the user presses the Enter key.
//----------------------------------------------------------------------
void CAobrdDriverShipmentsDlg::OnEnSetfocusLoginEdit()
{
    SendMessage( DM_SETDEFID, IDC_DRIVER_SHIPMENTS_BTN_SET );
}   /* OnEnSetfocusLoginEdit */

//----------------------------------------------------------------------
//! \brief Handles the set focus event for any date time ctrl.
//! \details Sets the default control to the Set button so that it is
//!     activated if the user presses the Enter key.
//----------------------------------------------------------------------
void CAobrdDriverShipmentsDlg::OnDateTimeCtrlSetFocus( NMHDR *, LRESULT * )
{
    SendMessage( DM_SETDEFID, IDC_DRIVER_SHIPMENTS_BTN_SET );
}   /* OnDateTimeCtrlSetFocus */

//----------------------------------------------------------------------
//! \brief Handles the kill focus event for the driver ID and password
//!     edit boxes.
//! \details Sets the default control to the OK button so that it is
//!     activated if the user presses the Enter key.
//----------------------------------------------------------------------
void CAobrdDriverShipmentsDlg::OnEnKillfocusLoginEdit()
{
    SendMessage( DM_SETDEFID, IDOK );
}   /* OnEnKillfocusLoginEdit */

//----------------------------------------------------------------------
//! \brief Handles the kill focus event for date time controls.
//! \details Sets the default control to the OK button so that it is
//!     activated if the user presses the Enter key.
//----------------------------------------------------------------------
void CAobrdDriverShipmentsDlg::OnDateTimeCtrlKillFocus( NMHDR *, LRESULT * )
{
    SendMessage( DM_SETDEFID, IDOK );
}   /* OnDateTimeCtrlKillFocus */

//----------------------------------------------------------------------
//! \brief Handles the set focus event for the driver list.
//! \details Sets the default control to the Delete button so that it
//!     is activated if the user presses the Enter key.
//----------------------------------------------------------------------
void CAobrdDriverShipmentsDlg::OnLbnSetfocusShipmentList()
{
    SendMessage( DM_SETDEFID, IDC_DRIVER_SHIPMENTS_BTN_DELETE );
}   /* OnLbnSetfocusShipmentList */

//----------------------------------------------------------------------
//! \brief Handles the kill focus event for the driver list.
//! \details Sets the default control to the OK button so that it is
//!     activated if the user presses the Enter key.
//----------------------------------------------------------------------
void CAobrdDriverShipmentsDlg::OnLbnKillfocusShipmentList()
{
    SendMessage( DM_SETDEFID, IDOK );
}   /* OnLbnKillfocusShipmentList */

