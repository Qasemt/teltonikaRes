/*********************************************************************
*
*   HEADER NAME:
*       CFileTransferProgressDlg.h
*
*   Copyright 2008-2011 by Garmin Ltd. or its subsidiaries.
*---------------------------------------------------------------------
* $NoKeywords$
*********************************************************************/
#ifndef CFileTransferProgressDlg_H
#define CFileTransferProgressDlg_H

#include "CWndEventListener.h"
#include "FmiApplicationLayer.h"

//----------------------------------------------------------------------
//! \brief Modal dialog displaying the status of the file transfer.
//! \details This dialog is displayed when a file transfer begins.
//!     When FmiApplicationLayer receives a file or packet receipt, it
//!     sends a status update message which this dialog receives and
//!     processes to update the text box and progress bar.  Canceling
//!     the file transfer sets a flag in the FmiApplicationLayer
//!     instance which instructs the server to end the transfer (by
//!     not sending any more data).
//! \since Protocol A604
//----------------------------------------------------------------------
class CFileTransferProgressDlg : public CDialog, public CWndEventListener
{
    DECLARE_DYNAMIC( CFileTransferProgressDlg )
    DECLARE_MESSAGE_MAP()

public:
    CFileTransferProgressDlg
        (
        CWnd                * aParent,
        FmiApplicationLayer & aCom
        );

    virtual ~CFileTransferProgressDlg();

protected:
    virtual void DoDataExchange
        (
        CDataExchange* aDataExchange
        );

    BOOL OnInitDialog();
    afx_msg void OnBnClickedStop();
    afx_msg LRESULT OnFileTransferStateChange( WPARAM, LPARAM );
    afx_msg LRESULT OnFileTransferProgress( WPARAM, LPARAM );

    //! Reference to the FMI communication controller
    FmiApplicationLayer & mCom;

    //! Current status of the transfer, as a string
    //! \details This may be a generic string ("Started", "Cancelled")
    //!     or, for an in progress transfer, include the bytes
    //!     transferred and total
    CString mTransferStatus;

    //! The progress bar.
    CProgressCtrl mProgressBar;

    //! If TRUE, the Stop button was clicked.
    BOOL mStopButtonClicked;

};

#endif
