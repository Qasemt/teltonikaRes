/*********************************************************************
*
*   MODULE NAME:
*       CFeatureDlg.cpp
*
*   Copyright 2008-2009 by Garmin Ltd. or its subsidiaries.
*---------------------------------------------------------------------
* $NoKeywords$
*********************************************************************/

#include "stdafx.h"
#include "CFeatureDlg.h"
#include "util.h"

BEGIN_MESSAGE_MAP( CFeatureDlg, CDialog )
    ON_BN_CLICKED( IDOK, OnBnClickedOk )
END_MESSAGE_MAP()

//----------------------------------------------------------------------
//! \brief Constructor
//! \param aParent The parent window
//! \param aCom Reference to the FMI communication controller
//----------------------------------------------------------------------
CFeatureDlg::CFeatureDlg
    (
    CWnd                * aParent,
    FmiApplicationLayer & aCom
    )
    : CDialog( IDD_FEATURE_SELECT, aParent )
    , mCom( aCom )
{
    mFeatureCount = 0;
}

//----------------------------------------------------------------------
//! \brief Destructor
//----------------------------------------------------------------------
CFeatureDlg::~CFeatureDlg()
{
}

//----------------------------------------------------------------------
//! \brief Initialize the dialog
//! \details This function is called when the window is created.  It
//!    gets references to each of the check boxes, and sets the default
//!    state of the Unicode Support and A607 Support boxes to checked.
//! \return TRUE, since this function does not set focus to a control
//----------------------------------------------------------------------
BOOL CFeatureDlg::OnInitDialog()
{
    ( ( CButton* )GetDlgItem( IDC_FEATURE_CHK_UNICODE ) )->SetCheck( BST_CHECKED );
    ( ( CButton* )GetDlgItem( IDC_FEATURE_CHK_A607 ) )->SetCheck( BST_CHECKED );

#if( FMI_SUPPORT_A613 )
    ( ( CButton* )GetDlgItem( IDC_FEATURE_CHK_CUST_AVOID ) )->SetCheck( BST_INDETERMINATE );
#endif

    return CDialog::OnInitDialog();
}

//----------------------------------------------------------------------
//! \brief Process a feature check box
//! \details Based on the state of aCheckBox, add aFeature to
//!    mFeatureCodes as enabled or disabled if appropriate.
//! \param aCheckBox The check box to inspect
//! \param aFeature The feature code to enable or disable
//----------------------------------------------------------------------
void CFeatureDlg::checkFeature
    (
    CButton          * aCheckBox,
    fmi_feature_type   aFeature
    )
{
    switch( aCheckBox->GetCheck() )
    {
    case BST_CHECKED:
        mFeatureCodes[mFeatureCount] = (uint16)( aFeature | FEATURE_STATE_ENABLED );
        ++mFeatureCount;
        break;

    case BST_UNCHECKED:
        mFeatureCodes[mFeatureCount] = (uint16)( aFeature | FEATURE_STATE_DISABLED );
        ++mFeatureCount;
        break;

    default:
        break;

    }
}

//----------------------------------------------------------------------
//! \brief Click handler for OK button
//! \details Build and send an extended FMI enable packet based on the
//!    state of the check boxes.
//----------------------------------------------------------------------
void CFeatureDlg::OnBnClickedOk()
{
    // Default values
    mCom.mClientCodepage = CODEPAGE_ASCII;
    mCom.mUsePasswords = FALSE;
    mCom.mUseMultipleDrivers = FALSE;

    if( BST_CHECKED == ( ( CButton* )GetDlgItem( IDC_FEATURE_CHK_UNICODE ) )->GetCheck() )
    {
        mCom.mClientCodepage = CODEPAGE_UNICODE;
    }

    if( BST_CHECKED == ( ( CButton* )GetDlgItem( IDC_FEATURE_CHK_PASSWORDS ) )->GetCheck() )
    {
        mCom.mUsePasswords = TRUE;
    }

    if( BST_CHECKED == ( ( CButton* )GetDlgItem( IDC_FEATURE_CHK_MULTIDRIVER ) )->GetCheck() )
    {
        mCom.mUseMultipleDrivers = TRUE;
    }

#if( FMI_SUPPORT_A610 )
    mCom.mUseAOBRD = ( BST_CHECKED == ( ( CButton* )GetDlgItem( IDC_FEATURE_CHK_AOBRD ) )->GetCheck() );
#endif

    // Build and send the enable request
    checkFeature( ( CButton* )GetDlgItem( IDC_FEATURE_CHK_UNICODE ), FEATURE_ID_UNICODE );
    checkFeature( ( CButton* )GetDlgItem( IDC_FEATURE_CHK_A607 ), FEATURE_ID_A607_SUPPORT );
    checkFeature( ( CButton* )GetDlgItem( IDC_FEATURE_CHK_PASSWORDS ), FEATURE_ID_DRIVER_PASSWORDS );
    checkFeature( ( CButton* )GetDlgItem( IDC_FEATURE_CHK_MULTIDRIVER ), FEATURE_ID_MULTIPLE_DRIVERS );
#if( FMI_SUPPORT_A610 )
    checkFeature( ( CButton* )GetDlgItem( IDC_FEATURE_CHK_AOBRD ), FEATURE_ID_AOBRD_SUPPORT );
#endif

    mCom.sendEnable( mFeatureCodes, mFeatureCount );

#if( FMI_SUPPORT_A613 )
    custom_avoid_feature_enable_type message;
    memset( &message, 0, sizeof( message ) );
    message.origination_time = UTIL_get_current_garmin_time();
    switch( ( ( CButton* )GetDlgItem( IDC_FEATURE_CHK_CUST_AVOID ) )->GetCheck() )
        {
        case BST_CHECKED:
            {
            message.enable = true;
            mCom.sendFmiPacket( FMI_CUSTOM_AVOID_ENABLE_FEATURE_REQUEST, (uint8*)&message, sizeof( message ) );
            break;
            }
        case BST_UNCHECKED:
            {
            message.enable = false;
            mCom.sendFmiPacket( FMI_CUSTOM_AVOID_ENABLE_FEATURE_REQUEST, (uint8*)&message, sizeof( message ) );
            break;
            }
        case BST_INDETERMINATE:
            {
            // don't change anything
            break;
            }
        }
#endif
    OnOK();
}
