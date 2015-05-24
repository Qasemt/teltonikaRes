/*********************************************************************
*
*   HEADER NAME:
*       CFileTransferDlg.h
*
*   Copyright 2008-2011 by Garmin Ltd. or its subsidiaries.
*---------------------------------------------------------------------
* $NoKeywords$
*********************************************************************/
#ifndef CFileTransferDlg_H
#define CFileTransferDlg_H

#include "FmiApplicationLayer.h"

//----------------------------------------------------------------------
//! \brief Modal dialog allowing the user to select a file to
//!     transfer to the client.
//! \since Protocol A604
//----------------------------------------------------------------------
class CFileTransferDlg : public CDialog
{
    DECLARE_DYNAMIC( CFileTransferDlg )
    DECLARE_MESSAGE_MAP()

public:
    CFileTransferDlg
        (
        CWnd                * aParent,
        FmiApplicationLayer & aCom
        );
    virtual ~CFileTransferDlg();

    //! Defines file type for a supported file type
    typedef struct
        {
        uint8 index;
        CString text;
        CString extension;
        CString filter;
        } file_type;

private:
    virtual void DoDataExchange
        (
        CDataExchange* aDataExchange
        );

    BOOL OnInitDialog();
    afx_msg void OnEnChangeFile();
    afx_msg void OnBnClickedFind();
    afx_msg void OnBnClickedOk();

    //! Reference to the FMI communication controller
    FmiApplicationLayer& mCom;

    //! Path and file name of the file to transfer, as specified
    //!     by the user
    CString mFilePath;

    //! File type to be transferred, as specified by the user
    int mFileType;

    //! File string specified by the user.
    CString mVersion;
};

#endif
