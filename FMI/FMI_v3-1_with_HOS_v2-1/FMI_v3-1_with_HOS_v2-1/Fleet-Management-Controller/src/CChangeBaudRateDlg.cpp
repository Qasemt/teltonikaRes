/*********************************************************************
*
*   MODULE NAME:
*       CChangeBaudRateDlg.cpp
*
* Copyright 2013 by Garmin Ltd. or its subsidiaries.
*---------------------------------------------------------------------
* $NoKeywords$
*********************************************************************/

#include "stdafx.h"
#include "CChangeBaudRateDlg.h"
#include "SerialPort.h"

#if( FMI_SUPPORT_A616 )

// CChangeBaudRateDlg dialog

IMPLEMENT_DYNAMIC(CChangeBaudRateDlg, CDialog)

//----------------------------------------------------------------------
//! \brief Constructor
//! \param aParent The parent window
//! \param aCom Reference to the FMI communication controller
//----------------------------------------------------------------------
CChangeBaudRateDlg::CChangeBaudRateDlg(
    CWnd* pParent,
    FmiApplicationLayer & aCom
    )
    : CDialog( CChangeBaudRateDlg::IDD, pParent )
    , mCom( aCom )
    , mCurrentRate( SerialPort::getInstance()->getBaudRate() )
    {
}

CChangeBaudRateDlg::~CChangeBaudRateDlg()
{
}

void CChangeBaudRateDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_CBIndex(pDX, IDC_NEW_BAUD_RATE, mNewRate);
    DDX_Text(pDX, IDC_CURRENT_BAUD_RATE, mCurrentRate);
}


BEGIN_MESSAGE_MAP(CChangeBaudRateDlg, CDialog)
END_MESSAGE_MAP()

// CChangeBaudRateDlg message handlers
//----------------------------------------------------------------------
//! \brief Click handler for OK button
//! \details Build and send a SET baud rate packet based on the state
//!  of the fields
//----------------------------------------------------------------------
void CChangeBaudRateDlg::OnOK()
{
    UpdateData();

    fmi_set_baud_request request;
    memset( &request, 0, sizeof( request ) );
    request.request_type = FMI_BAUD_NEW_RATE_REQUEST;
    request.baud_rate_type = ( uint8 )( 0 != mNewRate ? FMI_BAUD_RATE_57600 : FMI_BAUD_RATE_9600 );

    mCom.sendFmiPacket( FMI_SET_BAUD_REQUEST, ( uint8* )&request, sizeof( request ) );

    CDialog::OnOK();
}

#endif
