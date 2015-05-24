/*********************************************************************
*
*   MODULE NAME:
*       CAobrdDriversDlg.cpp
*
*   Copyright 2012 by Garmin Ltd. or its subsidiaries.
*---------------------------------------------------------------------
* $NoKeywords$
*********************************************************************/
#include "stdafx.h"
#include "CFmiApplication.h"
#include "CAobrdDriversDlg.h"
#include "CAobrdDriverShipmentsDlg.h"

#if( FMI_SUPPORT_A610 )

using namespace std;

IMPLEMENT_DYNAMIC(CAobrdDriversDlg, CDialog)

BEGIN_MESSAGE_MAP( CAobrdDriversDlg, CDialog )
    ON_BN_CLICKED( IDC_DRIVERS_SEND_UPDATE, OnBnClickedSendUpdate )
#if( FMI_SUPPORT_A615 )
    ON_BN_CLICKED( IDC_DRIVERS_SEND_LOGOFF, OnBnClickedSendLogoff )
#endif
    ON_BN_CLICKED( IDC_DRIVERS_BTN_DELETE, OnBnClickedDelete )
    ON_BN_CLICKED( IDC_DRIVERS_BTN_SET, OnBnClickedSet )
    ON_BN_CLICKED( IDC_DRIVERS_SHIPMENTS, OnBnClickedShipments )
    ON_EN_CHANGE( IDC_DRIVERS_EDIT_ID, OnChangeDriverData )
    ON_EN_CHANGE( IDC_DRIVERS_EDIT_PASSWORD, OnChangeDriverData )
    ON_EN_CHANGE( IDC_DRIVERS_EDIT_FIRST_NAME, OnChangeDriverData )
    ON_EN_CHANGE( IDC_DRIVERS_EDIT_LAST_NAME, OnChangeDriverData )
    ON_EN_CHANGE( IDC_DRIVERS_EDIT_CARRIER_NAME, OnChangeDriverData )
    ON_EN_CHANGE( IDC_DRIVERS_EDIT_CARRIER_ID, OnChangeDriverData )
    ON_LBN_SELCHANGE( IDC_DRIVERS_LONG_TERM_RULESETS, OnChangeDriverData )
    ON_LBN_SELCHANGE( IDC_DRIVERS_LOAD_TYPE_RULESETS, OnChangeDriverData )
    ON_LBN_SELCHANGE( IDC_DRIVERS_TIME_ZONES, OnChangeDriverData )
    ON_BN_CLICKED( IDOK, OnBnClickedOk )
    ON_EN_SETFOCUS( IDC_DRIVERS_EDIT_ID, OnEnSetfocusLoginEdit )
    ON_EN_KILLFOCUS( IDC_DRIVERS_EDIT_ID, OnEnKillfocusLoginEdit )
    ON_EN_SETFOCUS( IDC_DRIVERS_EDIT_PASSWORD, OnEnSetfocusLoginEdit )
    ON_EN_KILLFOCUS( IDC_DRIVERS_EDIT_PASSWORD, OnEnKillfocusLoginEdit )
    ON_EN_SETFOCUS( IDC_DRIVERS_EDIT_FIRST_NAME, OnEnSetfocusLoginEdit )
    ON_EN_KILLFOCUS( IDC_DRIVERS_EDIT_FIRST_NAME, OnEnKillfocusLoginEdit )
    ON_EN_SETFOCUS( IDC_DRIVERS_EDIT_LAST_NAME, OnEnSetfocusLoginEdit )
    ON_EN_KILLFOCUS( IDC_DRIVERS_EDIT_LAST_NAME, OnEnKillfocusLoginEdit )
    ON_EN_SETFOCUS( IDC_DRIVERS_EDIT_CARRIER_NAME, OnEnSetfocusLoginEdit )
    ON_EN_KILLFOCUS( IDC_DRIVERS_EDIT_CARRIER_NAME, OnEnKillfocusLoginEdit )
    ON_EN_SETFOCUS( IDC_DRIVERS_EDIT_CARRIER_ID, OnEnSetfocusLoginEdit )
    ON_EN_KILLFOCUS( IDC_DRIVERS_EDIT_CARRIER_ID, OnEnKillfocusLoginEdit )
    ON_CBN_SETFOCUS( IDC_DRIVERS_LONG_TERM_RULESETS, OnEnSetfocusLoginEdit )
    ON_CBN_KILLFOCUS( IDC_DRIVERS_LONG_TERM_RULESETS, OnEnKillfocusLoginEdit )
    ON_CBN_SETFOCUS( IDC_DRIVERS_LOAD_TYPE_RULESETS, OnEnSetfocusLoginEdit )
    ON_CBN_KILLFOCUS( IDC_DRIVERS_LOAD_TYPE_RULESETS, OnEnKillfocusLoginEdit )
    ON_CBN_SETFOCUS( IDC_DRIVERS_TIME_ZONES, OnEnSetfocusLoginEdit )
    ON_CBN_KILLFOCUS( IDC_DRIVERS_TIME_ZONES, OnEnKillfocusLoginEdit )
    ON_LBN_SELCHANGE( IDC_DRIVERS_LST_DRIVERS, OnLbnSelchangeDriverList )
    ON_LBN_SETFOCUS( IDC_DRIVERS_LST_DRIVERS, OnLbnSetfocusDriverList )
    ON_LBN_KILLFOCUS( IDC_DRIVERS_LST_DRIVERS, OnLbnKillfocusDriverList )
END_MESSAGE_MAP()

//----------------------------------------------------------------------
//! \brief Constructor
//! \param  aParent The parent window.
//! \param aCom Reference to the FMI communication controller
//----------------------------------------------------------------------
CAobrdDriversDlg::CAobrdDriversDlg
    (
    CWnd                * aParent,
    FmiApplicationLayer & aCom
    )
    : CDialog( IDD_AOBRD_DRIVERS, aParent )
    , mCom( aCom )
    , mSelectedIndex( -1 )
    , mDriverId( _T("") )
    , mDriverPassword( _T("") )
    , mDriverFirstName( _T("") )
    , mDriverLastName( _T("") )
    , mDriverCarrierName( _T("") )
    , mDriverCarrierID( _T("") )
    , mDriverLongTermRuleset( -1 )
#if( FMI_SUPPORT_A615 )
    , mDriverLoadTypeRuleset( -1 )
#endif
    , mDriverTimeZone( -1 )
{
}

//----------------------------------------------------------------------
//! \brief Destructor
//----------------------------------------------------------------------
CAobrdDriversDlg::~CAobrdDriversDlg()
{
}

//----------------------------------------------------------------------
//! \brief Perform dialog data exchange and validation
//! \param  aDataExchange The DDX context
//----------------------------------------------------------------------
void CAobrdDriversDlg::DoDataExchange
    (
    CDataExchange * aDataExchange
    )
{
    CDialog::DoDataExchange( aDataExchange );
    DDX_Control( aDataExchange, IDC_DRIVERS_LST_DRIVERS, mListBox );
    DDX_Text( aDataExchange, IDC_DRIVERS_EDIT_ID, mDriverId );
    DDX_Text( aDataExchange, IDC_DRIVERS_EDIT_PASSWORD, mDriverPassword );
    DDX_Text( aDataExchange, IDC_DRIVERS_EDIT_FIRST_NAME, mDriverFirstName );
    DDX_Text( aDataExchange, IDC_DRIVERS_EDIT_LAST_NAME, mDriverLastName );
    DDX_Text( aDataExchange, IDC_DRIVERS_EDIT_CARRIER_NAME, mDriverCarrierName );
    DDX_Text( aDataExchange, IDC_DRIVERS_EDIT_CARRIER_ID, mDriverCarrierID );
    DDX_CBIndex( aDataExchange, IDC_DRIVERS_LONG_TERM_RULESETS, mDriverLongTermRuleset );
#if( FMI_SUPPORT_A615 )
    DDX_CBIndex( aDataExchange, IDC_DRIVERS_LOAD_TYPE_RULESETS, mDriverLoadTypeRuleset );
#endif
    DDX_CBIndex( aDataExchange, IDC_DRIVERS_TIME_ZONES, mDriverTimeZone );
}

//----------------------------------------------------------------------
//! \brief This function is called when the window is created.
//! \details This function is called when the window is created. It
//!     initializes the drivers list, and sets the initial position
//!     of the window.
//! \return TRUE, since this function does not set focus to a control
//----------------------------------------------------------------------
BOOL CAobrdDriversDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    updateListBox();

    GetDlgItem( IDC_DRIVERS_SEND_UPDATE )->EnableWindow( FALSE );
    GetDlgItem( IDC_DRIVERS_BTN_DELETE )->EnableWindow( FALSE );
    GetDlgItem( IDC_DRIVERS_SHIPMENTS )->EnableWindow( FALSE );

    SetWindowPos( NULL, 700, 350, 0, 0, SWP_NOSIZE | SWP_NOZORDER );

    CComboBox * longTermRulesets = (CComboBox*) GetDlgItem( IDC_DRIVERS_LONG_TERM_RULESETS );
    longTermRulesets->ResetContent();
    longTermRulesets->AddString(_T("60 hour/7 day"));
    longTermRulesets->AddString(_T("70 hour/8 day"));

#if( FMI_SUPPORT_A615 )
    CComboBox * loadTypeRulesets = (CComboBox*) GetDlgItem( IDC_DRIVERS_LOAD_TYPE_RULESETS );
    loadTypeRulesets->ResetContent();
    loadTypeRulesets->AddString(_T("Property"));
    loadTypeRulesets->AddString(_T("Passenger"));

    GetDlgItem( IDC_DRIVERS_SEND_LOGOFF )->EnableWindow( FALSE );
#else
    GetDlgItem( IDC_DRIVERS_LOAD_TYPE_RULESETS )->ShowWindow( SW_HIDE );
    GetDlgItem( IDC_DRIVERS_LBL_LOAD_TYPE_RULESET )->ShowWindow( SW_HIDE );
    GetDlgItem( IDC_DRIVERS_SEND_LOGOFF )->ShowWindow( SW_HIDE );
#endif

    CComboBox * timeZones = (CComboBox*) GetDlgItem( IDC_DRIVERS_TIME_ZONES );
    timeZones->ResetContent();
    timeZones->AddString(_T("Eastern"));
    timeZones->AddString(_T("Central"));
    timeZones->AddString(_T("Mountain"));
    timeZones->AddString(_T("Pacific"));
    timeZones->AddString(_T("Alaska"));
    timeZones->AddString(_T("Hawaii"));

    ((CEdit*)GetDlgItem(IDC_DRIVERS_EDIT_ID))->LimitText(39);
    ((CEdit*)GetDlgItem(IDC_DRIVERS_EDIT_PASSWORD))->LimitText(19);
    ((CEdit*)GetDlgItem(IDC_DRIVERS_EDIT_FIRST_NAME))->LimitText(34);
    ((CEdit*)GetDlgItem(IDC_DRIVERS_EDIT_LAST_NAME))->LimitText(34);
    ((CEdit*)GetDlgItem(IDC_DRIVERS_EDIT_CARRIER_NAME))->LimitText(119);
    ((CEdit*)GetDlgItem(IDC_DRIVERS_EDIT_CARRIER_ID))->LimitText(7);

    return TRUE;
} /* OnInitDialog() */

//----------------------------------------------------------------------
//! \brief Clear values from the editable fields.
//----------------------------------------------------------------------
void CAobrdDriversDlg::clearData()
{
    mDriverId = _T("");
    mDriverPassword = _T("");
    mDriverFirstName = _T("");
    mDriverLastName = _T("");
    mDriverCarrierName = _T("");
    mDriverCarrierID = _T("");
    mDriverLongTermRuleset = -1;
#if( FMI_SUPPORT_A615 )
    mDriverLoadTypeRuleset = -1;
#endif
    mDriverTimeZone = -1;
} /* clearData() */

//----------------------------------------------------------------------
//! \brief Update the drivers list box from the drivers map owned
//!     by FmiApplicationLayer.
//----------------------------------------------------------------------
void CAobrdDriversDlg::updateListBox()
{
    CString listItem;

    //must keep track of where the list was scrolled to
    //since we reset content we must reinitialize these
    mSelectedIndex = mListBox.GetCurSel();
    int topIndex = mListBox.GetTopIndex();

    //reset content and then add current drivers
    mListBox.ResetContent();
    FileBackedMap<AobrdDriverItem>::const_iterator iter = mCom.mAOBRDDrivers.begin();
    for( ; iter != mCom.mAOBRDDrivers.end(); ++iter )
    {
        if( iter->second.isValid() )
        {
            listItem.Format( _T("%s"), iter->second.getDriverId() );
            mListBox.AddString( listItem );
        }
    }

    //reset scroll and selection
    mSelectedIndex = -1;

    //reset scroll and selection
    mListBox.SetCurSel( mSelectedIndex );
    mListBox.SetTopIndex( topIndex );

}   /* updateListBox() */

//----------------------------------------------------------------------
//! \brief Button handler for the Send Update button
//! \details Send the driver info to the device.
//----------------------------------------------------------------------
void CAobrdDriversDlg::OnBnClickedSendUpdate()
{
    CListBox * driverListBox = ( CListBox* ) GetDlgItem( IDC_DRIVERS_LST_DRIVERS );
    mSelectedIndex = driverListBox->GetCurSel();

    if( mSelectedIndex >= 0 && mSelectedIndex < driverListBox->GetCount() )
    {
        const AobrdDriverItem & driver = mCom.mAOBRDDrivers.get( mCom.mAOBRDDrivers.getKeyAt( mSelectedIndex ) );
        mCom.sendAOBRDDriverProfile( driver );
    }
}   /* OnBnClickedSendUpdate() */

#if( FMI_SUPPORT_A615 )

//----------------------------------------------------------------------
//! \brief Button handler for the Send Logoff button
//! \details Send a driver logoff command to the device.
//----------------------------------------------------------------------
void CAobrdDriversDlg::OnBnClickedSendLogoff()
{
    CListBox * driverListBox = ( CListBox* ) GetDlgItem( IDC_DRIVERS_LST_DRIVERS );
    mSelectedIndex = driverListBox->GetCurSel();

    if( mSelectedIndex >= 0 && mSelectedIndex < driverListBox->GetCount() )
    {
        const AobrdDriverItem & driver = mCom.mAOBRDDrivers.get( mCom.mAOBRDDrivers.getKeyAt( mSelectedIndex ) );
        mCom.sendAOBRDDriverLogoff( driver );
    }
}   /* OnBnClickedSendLogoff() */

#endif

//----------------------------------------------------------------------
//! \brief Button handler for the Delete button
//! \details Remove the selected driver from the allowed logins.
//----------------------------------------------------------------------
void CAobrdDriversDlg::OnBnClickedDelete()
{
    UpdateData( TRUE );
    if( mSelectedIndex >= 0 )
    {
        mCom.mAOBRDDrivers.remove( mCom.mAOBRDDrivers.getKeyAt( mSelectedIndex ) );
        updateListBox();
    }

    if( mSelectedIndex < 0 )
    {
        GetDlgItem( IDC_DRIVERS_SEND_UPDATE )->EnableWindow( FALSE );
        GetDlgItem( IDC_DRIVERS_SEND_LOGOFF )->EnableWindow( FALSE );
        GetDlgItem( IDC_DRIVERS_BTN_DELETE )->EnableWindow( FALSE );
        clearData();
    }

    UpdateData(FALSE);

}   /* OnBnClickedDelete() */

//----------------------------------------------------------------------
//! \brief Button handler for the Set button
//! \details Saves the new driver info.
//----------------------------------------------------------------------
void CAobrdDriversDlg::OnBnClickedSet()
{
    UpdateData( TRUE );

    char driverId[50];
    char driverPassword[20];
    char firstName[50];
    char lastName[50];
    char carrierName[50];
    char carrierID[50];

    WideCharToMultiByte( CP_UTF8, 0, mDriverId, -1, driverId, sizeof( driverId ), NULL, NULL );
    WideCharToMultiByte( CP_UTF8, 0, mDriverPassword, -1, driverPassword, sizeof( driverPassword ), NULL, NULL );
    WideCharToMultiByte( CP_UTF8, 0, mDriverFirstName, -1, firstName, sizeof( firstName ), NULL, NULL );
    WideCharToMultiByte( CP_UTF8, 0, mDriverLastName, -1, lastName, sizeof( lastName ), NULL, NULL );
    WideCharToMultiByte( CP_UTF8, 0, mDriverCarrierName, -1, carrierName, sizeof( carrierName ), NULL, NULL );
    WideCharToMultiByte( CP_UTF8, 0, mDriverCarrierID, -1, carrierID, sizeof( carrierID ), NULL, NULL );

    AobrdDriverItem & driver = mCom.mAOBRDDrivers.get( driverId );
    if( ( driver.isValid() ) &&
        ( ( 0 != driver.getFirstName().Compare(mDriverFirstName) ) ||
          ( 0 != driver.getLastName().Compare(mDriverLastName) ) ) )
        {
        MessageBox( _T( "Driver First and Last name should not be changed since the device uses that information as a key." ), _T( "Error" ), MB_ICONERROR );
        return;
        }

    driver.setPassword( driverPassword );
    driver.setFirstName( firstName );
    driver.setLastName( lastName );
    driver.setCarrierName( carrierName );
    driver.setCarrierID( carrierID );
    driver.setLongTermRuleset( (uint16) mDriverLongTermRuleset );
#if( FMI_SUPPORT_A615 )
    driver.setLoadTypeRuleset( (uint16) mDriverLoadTypeRuleset );
#endif
    driver.setTimeZone( (uint16) mDriverTimeZone );

    mCom.mAOBRDDrivers.put( driver );

    updateListBox();
    clearData();
    UpdateData(FALSE);

}   /* OnBnClickedSet() */

void CAobrdDriversDlg::OnBnClickedShipments()
{
    AobrdDriverItem& driver = mCom.mAOBRDDrivers.get( mCom.mAOBRDDrivers.getKeyAt( mSelectedIndex ) );
    if( CAobrdDriverShipmentsDlg::ShowShipments( this, &driver ) )
        {
        mCom.mAOBRDDrivers.put( driver );
        }
}

//----------------------------------------------------------------------
//! \brief Edit/Change handler for the driver-specific data controls.
//! \details If any of controls are unset, disables the Set button; if all
//!     are specified, enables the Set button.
//----------------------------------------------------------------------
void CAobrdDriversDlg::OnChangeDriverData()
{
    UpdateData( TRUE );
    if( ( "" != mDriverId ) && ( "" != mDriverPassword ) &&
        ( "" != mDriverFirstName ) && ( "" != mDriverLastName ) &&
        ( "" != mDriverCarrierName ) && ( "" != mDriverCarrierID ) &&
        ( -1 != mDriverLongTermRuleset ) &&
#if( FMI_SUPPORT_A615 )
        ( -1 != mDriverLoadTypeRuleset ) &&
#endif
        ( -1 != mDriverTimeZone ) )
    {
        GetDlgItem( IDC_DRIVERS_BTN_SET )->EnableWindow( TRUE );
    }
    else
    {
        GetDlgItem( IDC_DRIVERS_BTN_SET )->EnableWindow( FALSE );
    }
}   /* OnEnChangeEditBoxes() */


//----------------------------------------------------------------------
//! \brief Selection Changed handler for the Driver List box.
//! \details Fills in the driver ID and password fields of the
//!     dialog with the information from the selected list item, for
//!     easy editing.
//----------------------------------------------------------------------
void CAobrdDriversDlg::OnLbnSelchangeDriverList()
{
    CListBox * driverListBox = (CListBox*) GetDlgItem( IDC_DRIVERS_LST_DRIVERS );
    mSelectedIndex = driverListBox->GetCurSel();

    if( mSelectedIndex >= 0 && mSelectedIndex < driverListBox->GetCount() )
    {
        const AobrdDriverItem& item = mCom.mAOBRDDrivers.get( mCom.mAOBRDDrivers.getKeyAt( mSelectedIndex ) );

        mDriverId = item.getDriverId();
        mDriverPassword = item.getPassword();
        mDriverFirstName = item.getFirstName();
        mDriverLastName = item.getLastName();
        mDriverCarrierName = item.getCarrierName();
        mDriverCarrierID = item.getCarrierID();
        mDriverLongTermRuleset = item.getLongTermRuleset();
#if( FMI_SUPPORT_A615 )
        mDriverLoadTypeRuleset = item.getLoadTypeRuleset();
#endif

        mDriverTimeZone = item.getTimeZone();

        UpdateData( FALSE );
        GetDlgItem( IDC_DRIVERS_SEND_UPDATE )->EnableWindow( TRUE );
        GetDlgItem( IDC_DRIVERS_SEND_LOGOFF )->EnableWindow( TRUE );
        GetDlgItem( IDC_DRIVERS_BTN_DELETE )->EnableWindow( TRUE );
        GetDlgItem( IDC_DRIVERS_SHIPMENTS )->EnableWindow( TRUE );

        OnChangeDriverData();
    }
    else
    {
        GetDlgItem( IDC_DRIVERS_SEND_UPDATE )->EnableWindow( FALSE );
        GetDlgItem( IDC_DRIVERS_SEND_LOGOFF )->EnableWindow( FALSE );
        GetDlgItem( IDC_DRIVERS_BTN_DELETE )->EnableWindow( FALSE );
        GetDlgItem( IDC_DRIVERS_SHIPMENTS )->EnableWindow( FALSE );
    }

    driverListBox->SetCurSel( mSelectedIndex );
}


//----------------------------------------------------------------------
//! \brief Button handler for the OK button
//! \details Closes the window.
//----------------------------------------------------------------------
void CAobrdDriversDlg::OnBnClickedOk()
{
    CDialog::OnOK();
}

//----------------------------------------------------------------------
//! \brief Handler for the Cancel action
//! \details Closes the window.
//----------------------------------------------------------------------
void CAobrdDriversDlg::OnCancel()
{
    CDialog::OnCancel();
}

//----------------------------------------------------------------------
//! \brief Called by MFC after the window has been destroyed; performs
//!     final termination activities.
//----------------------------------------------------------------------
void CAobrdDriversDlg::PostNcDestroy()
{
    CDialog::PostNcDestroy();
}   /* PostNcDestroy() */

//----------------------------------------------------------------------
//! \brief Handles the set focus event for the driver ID and password
//!     edit boxes.
//! \details Sets the default control to the Set button so that it is
//!     activated if the user presses the Enter key.
//----------------------------------------------------------------------
void CAobrdDriversDlg::OnEnSetfocusLoginEdit()
{
    SendMessage( DM_SETDEFID, IDC_DRIVERS_BTN_SET );
}   /* OnEnSetfocusLoginEdit */

//----------------------------------------------------------------------
//! \brief Handles the kill focus event for the driver ID and password
//!     edit boxes.
//! \details Sets the default control to the OK button so that it is
//!     activated if the user presses the Enter key.
//----------------------------------------------------------------------
void CAobrdDriversDlg::OnEnKillfocusLoginEdit()
{
    SendMessage( DM_SETDEFID, IDOK );
}   /* OnEnKillfocusLoginEdit */

//----------------------------------------------------------------------
//! \brief Handles the set focus event for the driver list.
//! \details Sets the default control to the Delete button so that it
//!     is activated if the user presses the Enter key.
//----------------------------------------------------------------------
void CAobrdDriversDlg::OnLbnSetfocusDriverList()
{
    SendMessage( DM_SETDEFID, IDC_DRIVERS_SEND_UPDATE );
}   /* OnLbnSetfocusDriverList */

//----------------------------------------------------------------------
//! \brief Handles the kill focus event for the driver list.
//! \details Sets the default control to the OK button so that it is
//!     activated if the user presses the Enter key.
//----------------------------------------------------------------------
void CAobrdDriversDlg::OnLbnKillfocusDriverList()
{
    SendMessage( DM_SETDEFID, IDOK );
}   /* OnLbnKillfocusDriverList */
#endif
