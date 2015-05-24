/*********************************************************************
*
*   MODULE NAME:
*       CAobrdOdometerDlg.cpp
*
*   Copyright 2012 by Garmin Ltd. or its subsidiaries.
*---------------------------------------------------------------------
* $NoKeywords$
*********************************************************************/

#include "stdafx.h"
#include <stdlib.h>
#include "CFmiApplication.h"
#include "CAobrdOdometerDlg.h"

#if( FMI_SUPPORT_A610 )

IMPLEMENT_DYNAMIC( CAobrdOdometerDlg, CDialog )

BEGIN_MESSAGE_MAP( CAobrdOdometerDlg, CDialog )
    ON_EN_CHANGE( IDC_AOBRD_ODOMETER_VALUE, OnEnChangeAobrdOdometer )
    ON_BN_CLICKED( IDOK, OnBnClickedOk )
END_MESSAGE_MAP()

//--------------------------------------------------------------------
//! \brief Constructor
//! \param aParent The parent of this dialog
//! \param aCom Reference to the FMI communication controller
//--------------------------------------------------------------------
CAobrdOdometerDlg::CAobrdOdometerDlg
    (
    CWnd                * aParent,
    FmiApplicationLayer & aCom
    )
    : CDialog( IDD_AOBRD_ODOMETER, aParent )
    , mCom( aCom )
    , odometer_value( _T("") )
{
}

//--------------------------------------------------------------------
//! \brief Destructor
//--------------------------------------------------------------------
CAobrdOdometerDlg::~CAobrdOdometerDlg()
{
}

//---------------------------------------------------------------------
//! \brief Perform dialog data exchange and validation
//! \param aDataExchange The DDX context
//---------------------------------------------------------------------
void CAobrdOdometerDlg::DoDataExchange
    (
    CDataExchange * aDataExchange
    )
{
    CDialog::DoDataExchange( aDataExchange );
    DDX_Text( aDataExchange, IDC_AOBRD_ODOMETER_VALUE, odometer_value );
}

//---------------------------------------------------------------------
//! \brief Initialize the dialog
//! \details This function is called when the window is created. It
//!     sets up the parent, so it can get info from and send a message
//!     to Com.
//! \return TRUE, since this function does not set focus to a control
//---------------------------------------------------------------------
BOOL CAobrdOdometerDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    return TRUE;
} /* OnInitDialog() */

//---------------------------------------------------------------------
//! \brief Edit Change handler for AOBRD odometer value text box
//! \details Enables OK button if the odometer reading is not empty;
//!     disables OK button otherwise.
//---------------------------------------------------------------------
void CAobrdOdometerDlg::OnEnChangeAobrdOdometer()
{
    UpdateData( TRUE );
    if( odometer_value != "" )
        GetDlgItem( IDOK )->EnableWindow( TRUE );
    else
        GetDlgItem( IDOK )->EnableWindow( FALSE );
}    /* OnEnChangeSafeModeSpeed() */

//---------------------------------------------------------------------
//! \brief Click handler for the OK button
//! \details Sends a speed to the client based on
//!     the input entered by the user.
//---------------------------------------------------------------------
void CAobrdOdometerDlg::OnBnClickedOk()
{
    UpdateData( TRUE );
    char    str[35];

    WideCharToMultiByte( mCom.mClientCodepage, 0, odometer_value, -1, str, 34, NULL, NULL );
    str[34] = '\0';
    mCom.sendAOBRDOdometerValue( (uint32)atoi( str ) );
    OnOK();
}    /* OnBnClickedOk() */

#endif
