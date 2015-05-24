/*********************************************************************
*
*   MODULE NAME:
*       CHosSettingDlg.cpp
*
* Copyright 2014 by Garmin Ltd. or its subsidiaries.
*---------------------------------------------------------------------
* $NoKeywords$
*********************************************************************/

#include "stdafx.h"
#include "CHosSettingDlg.h"

#if( FMI_SUPPORT_A615 )

// CHosSettingDlg dialog

IMPLEMENT_DYNAMIC(CHosSettingDlg, CDialog)

UINT CHosSettingDlg::CaptureSetting(
    CWnd* pParent,
    UINT titleStringId,
    UINT valueLabelStringId,
    UINT valueUnitStringId,
    long valueLowerLimit,
    long valueUpperLimit,
    BOOL & enabled,
    uint16 & value
    )
{
    CHosSettingDlg dlg( pParent );
    dlg.mTitleStringId = titleStringId;
    dlg.mValueLabelStringId = valueLabelStringId;
    dlg.mValueUnitStringId = valueUnitStringId;
    dlg.mValueLowerLimit = valueLowerLimit;
    dlg.mValueUpperLimit = valueUpperLimit;
    if( IDOK == dlg.DoModal() )
    {
        enabled = !dlg.mDisabled;
        value = ( uint16 )_tcstol( dlg.mValue, NULL, 10 );
        return IDOK;
    }
    return IDCANCEL;
}

//----------------------------------------------------------------------
//! \brief Constructor
//! \param aParent The parent window
//! \param aCom Reference to the FMI communication controller
//----------------------------------------------------------------------
CHosSettingDlg::CHosSettingDlg(
    CWnd* pParent
    )
    : CDialog(CHosSettingDlg::IDD, pParent)
    , mValue(_T(""))
    , mDisabled(FALSE)
    {

}

CHosSettingDlg::~CHosSettingDlg()
{
}

void CHosSettingDlg::DoDataExchange(CDataExchange* pDX)
{
CDialog::DoDataExchange(pDX);
DDX_Text(pDX, IDC_HOS_SETTING_VALUE, mValue);
DDV_MaxChars(pDX, mValue, 5);
DDX_Radio(pDX, IDC_ENABLE_YES, mDisabled);
    }


BEGIN_MESSAGE_MAP(CHosSettingDlg, CDialog)
    ON_BN_CLICKED(IDC_ENABLE_YES, OnReevaluate)
    ON_BN_CLICKED(IDC_ENABLE_NO, OnReevaluate)
    ON_EN_CHANGE(IDC_HOS_SETTING_VALUE, OnReevaluate)
END_MESSAGE_MAP()

// CHosSettingDlg message handlers

//----------------------------------------------------------------------
//! \brief Initialize the dialog
//! \details This function is called when the window is created. It
//!     sets up the parent, so it can get info from and send a message
//!     to FmiApplicationLayer.
//! \return TRUE, since this function does not set focus to a control
//----------------------------------------------------------------------
BOOL CHosSettingDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    //initialize controls
    CString title;
    title.LoadString( mTitleStringId );
    SetWindowText( title );

    CString label;
    label.LoadString( mValueLabelStringId );
    GetDlgItem( IDC_HOS_SETTING_VALUE_LABEL )->SetWindowText( label );

    CString units;
    units.LoadString( mValueUnitStringId );
    GetDlgItem( IDC_HOS_SETTING_VALUE_UNIT )->SetWindowText( units );

    return TRUE;
}

//----------------------------------------------------------------------
//! \brief Change handler for editable controls
//! \details Make the OK button clickable if the data is valid for sending
//----------------------------------------------------------------------
void CHosSettingDlg::OnReevaluate()
    {
    UpdateData();

    if( 0 == ((CButton*)GetDlgItem( IDC_ENABLE_YES ))->GetCheck() &&
        0 == ((CButton*)GetDlgItem( IDC_ENABLE_NO ))->GetCheck() )
        {
        GetDlgItem( IDOK )->EnableWindow( FALSE );
        return;
        }
    if( 0 != ((CButton*)GetDlgItem( IDC_ENABLE_NO ))->GetCheck() )
        {
        GetDlgItem( IDC_HOS_SETTING_VALUE )->EnableWindow( FALSE );
        }
    else
        {
        GetDlgItem( IDC_HOS_SETTING_VALUE )->EnableWindow();
        if( 0 == mValue.GetLength() ||
            mValueUpperLimit < _tcstol( mValue, NULL, 10 ) ||
            mValueLowerLimit > _tcstol( mValue, NULL, 10 ) )
            {
            GetDlgItem( IDOK )->EnableWindow( FALSE );
            return;
            }
        }

    GetDlgItem( IDOK )->EnableWindow();
    }

#endif
