/*********************************************************************
*
*   MODULE NAME:
*       CFileTransferDlg.cpp
*
*   Copyright 2008-2011 by Garmin Ltd. or its subsidiaries.
*---------------------------------------------------------------------
* $NoKeywords$
*********************************************************************/
#include "stdafx.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "CFileTransferDlg.h"
#include "CFileTransferProgressDlg.h"
#include "AobrdEventLogConverter.h"
#include "util.h"

using namespace std;

IMPLEMENT_DYNAMIC( CFileTransferDlg, CDialog )

BEGIN_MESSAGE_MAP( CFileTransferDlg, CDialog )
    ON_EN_CHANGE( IDC_FILE_XFER_EDIT_FILENAME, OnEnChangeFile )
    ON_EN_CHANGE( IDC_FILE_XFER_EDIT_FILE_VERSION, OnEnChangeFile )
    ON_BN_CLICKED( IDC_FILE_XFER_BTN_FIND, OnBnClickedFind )
    ON_BN_CLICKED( IDOK, OnBnClickedOk )
END_MESSAGE_MAP()

/*--------------------------------------------------------------------
LITERAL CONSTANTS
--------------------------------------------------------------------*/

//! File types that the client accepts
//! \see fmi.h for the enumeration
static CFileTransferDlg::file_type file_types[] =
{
    { FMI_FILE_TYPE_GPI, _T( "GPI" ), NULL, _T( "GPI Files (*.gpi)||" ) }
};

//----------------------------------------------------------------------
//! \brief Constructor
//! \param aParent The parent of this dialog
//! \param aCom Reference to the FMI communication controller
//----------------------------------------------------------------------
CFileTransferDlg::CFileTransferDlg
    (
    CWnd                * aParent,
    FmiApplicationLayer & aCom
    )
    : CDialog( IDD_FILE_TRANSFER, aParent )
    , mCom( aCom )
    , mFileType( 0 )
    , mFilePath( _T("") )
    , mVersion( _T("") )
{
}

//----------------------------------------------------------------------
//! \brief Destructor
//----------------------------------------------------------------------
CFileTransferDlg::~CFileTransferDlg()
{
}

//----------------------------------------------------------------------
//! \brief Perform dialog data exchange and validation
//! \param aDataExchange The DDX context
//----------------------------------------------------------------------
void CFileTransferDlg::DoDataExchange
    (
    CDataExchange * aDataExchange
    )
{
    CDialog::DoDataExchange( aDataExchange );

    DDX_CBIndex( aDataExchange, IDC_FILE_XFER_CBO_FILE_TYPE, mFileType );
    mFileType = file_types[mFileType].index;

    DDX_Text( aDataExchange, IDC_FILE_XFER_EDIT_FILENAME, mFilePath );
    DDX_Text( aDataExchange, IDC_FILE_XFER_EDIT_FILE_VERSION, mVersion );
}

//----------------------------------------------------------------------
//! \brief Initialize the dialog
//! \details This function is called when the window is created. It
//!     sets up the parent, so it can get info from and send a message
//!     to FmiApplicationLayer.
//! \return TRUE, since this function does not set focus to a control
//----------------------------------------------------------------------
BOOL CFileTransferDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // Populate file type combobox
    CComboBox * cbo = (CComboBox *)GetDlgItem( IDC_FILE_XFER_CBO_FILE_TYPE );
    cbo->ResetContent();
    for( int i = 0; i < cnt_of_array( file_types ); i++ )
    {
        cbo->AddString( file_types[i].text );
    }
    cbo->SetCurSel( 0 );

    return TRUE;
} /* OnInitDialog() */

//----------------------------------------------------------------------
//! \brief Edit Change handler for all text boxes.
//! \details Enables the OK button if both a file name and mVersion are
//!     specified, and the mVersion is of the appropriate length
//----------------------------------------------------------------------
void CFileTransferDlg::OnEnChangeFile()
{
    BOOL isValid = TRUE;

    UpdateData( TRUE );

    if( mFilePath == "" || mVersion == "" )
    {
        isValid = FALSE;
    }

    if( mVersion.Left( 2 ) == "0x" )
    {
        if( mVersion.GetLength() > 34 )
            isValid = FALSE;
    }
    else
    {
        if( mVersion.GetLength() > 16 )
            isValid = FALSE;
    }

    if( isValid )
    {
        GetDlgItem( IDOK )->EnableWindow( TRUE );
    }
    else
    {
        GetDlgItem( IDOK )->EnableWindow( FALSE );
    }
}

//----------------------------------------------------------------------
//! \brief Click handler for Find (file to send) button.
//! \details Displays a File..Open dialog allowing the user to select
//!     the file to send.
//----------------------------------------------------------------------
void CFileTransferDlg::OnBnClickedFind()
{
    UpdateData();

    TCHAR workingDirectory[200];
    // opening a file in another directory changes the current
    // directory, which will cause problems because the log and data
    // files are opened relative to the current directory.  So, get the
    // directory now so that it can be restored when the user is done
    // picking a file.
    DWORD result = GetCurrentDirectory( 200, workingDirectory );
    if( result == 0 || result > 200 )
    {
        MessageBox( _T("Unable to get current directory"), _T("Severe Error") );
        OnCancel();
        return;
    }
    CFileDialog dlg
        (
        TRUE,
        file_types[mFileType].extension,
        NULL,
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
        file_types[mFileType].filter
        );
    if( dlg.DoModal() == IDOK )
    {
        mFilePath = dlg.GetPathName();
        UpdateData( FALSE );
        OnEnChangeFile();
    }
    if( !SetCurrentDirectory( workingDirectory ) )
    {
        MessageBox( _T("Unable to set current directory"), _T("Severe Error") );
        OnCancel();
        return;
    }
}    /* OnBnClickedFind() */

//----------------------------------------------------------------------
//! \brief Click handler for OK button.
//! \details Initiates the file transfer process.  Actual packets are
//!     sent by the mCom itself, but the file entered is checked before
//!     sending the message.  If the file exists, a File Progress
//!     dialog box will appear that will not let the user continue
//!     until the file transfer completes or the user cancels it. If
//!     the file doesn't exist, an error dialog will appear.
//----------------------------------------------------------------------
void CFileTransferDlg::OnBnClickedOk()
{
    UpdateData( TRUE );
    char file[200];

    char    versionString[35];
    uint8   version[16];
    uint8   versionLength;

    memset( version, 0, sizeof( version ) );
    WideCharToMultiByte( mCom.mClientCodepage, 0, mVersion, -1, versionString, 34, NULL, NULL );
    versionString[34] = '\0';

    if( strncmp( versionString, "0x", 2 ) == 0 )
    {
        versionLength = (uint8)UTIL_hex_to_uint8( versionString + 2, version, 16 );
    }
    else
    {
        versionLength = (uint8)minval( 16, strlen( versionString ) );
        memmove( version, versionString, versionLength );
    }

    WideCharToMultiByte( CP_ACP, 0, mFilePath.GetBuffer(), -1, file, 200, NULL, NULL );
    file[199] = '\0';
    fstream open_file( file, ios_base::binary | ios_base::in );
    if( open_file.good() )
    {
        open_file.close();

        mCom.sendFile( file, versionLength, version, (uint8)mFileType );

        CFileTransferProgressDlg sending_dlg( this, mCom );
        sending_dlg.DoModal();
    }
    else
    {
        mFilePath += _T(" could not be opened ");
        MessageBox( mFilePath, _T("Error!") );
    }
    open_file.close();
    OnOK();
}    /* OnBnClickedOk */
