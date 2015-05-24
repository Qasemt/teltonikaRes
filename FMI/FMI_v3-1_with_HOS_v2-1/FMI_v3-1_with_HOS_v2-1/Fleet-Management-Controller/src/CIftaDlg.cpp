/*********************************************************************
*
*   MODULE NAME:
*       CIftaDlg.cpp
*
* Copyright 2013 by Garmin Ltd. or its subsidiaries.
*---------------------------------------------------------------------
* $NoKeywords$
*********************************************************************/

#include "stdafx.h"
#include "CIftaDlg.h"
#include "util.h"
#include "EventId.h"
#include <shlobj.h>

#pragma comment(lib, "shell32.lib")

#if( FMI_SUPPORT_A615 )

// CIftaDlg dialog

IMPLEMENT_DYNAMIC(CIftaDlg, CDialog)

//----------------------------------------------------------------------
//! \brief Constructor
//! \param aParent The parent window of this dialog
//! \param aCom The FmiApplicationLayer used to send commands to device
//----------------------------------------------------------------------
CIftaDlg::CIftaDlg(CWnd* pParent, FmiApplicationLayer & aCom)
    : CDialog(CIftaDlg::IDD, pParent)
    , mCom( aCom )
    , mStartDate(COleDateTime::GetCurrentTime())
    , mStartTime(COleDateTime::GetCurrentTime())
    , mEndDate(COleDateTime::GetCurrentTime())
    , mEndTime(COleDateTime::GetCurrentTime())
    {

}

CIftaDlg::~CIftaDlg()
{
}

void CIftaDlg::DoDataExchange(CDataExchange* pDX)
{
CDialog::DoDataExchange(pDX);
DDX_DateTimeCtrl(pDX, IDC_IFTA_START_DATE, mStartDate);
DDX_DateTimeCtrl(pDX, IDC_IFTA_START_TIME, mStartTime);
DDX_DateTimeCtrl(pDX, IDC_IFTA_END_DATE, mEndDate);
DDX_DateTimeCtrl(pDX, IDC_IFTA_END_TIME, mEndTime);
    }


BEGIN_MESSAGE_MAP(CIftaDlg, CDialog)
    ON_BN_CLICKED(IDC_IFTA_DATA_FETCH, &CIftaDlg::OnBnClickedDataFetch)
    ON_BN_CLICKED(IDC_IFTA_DATA_DELETE, &CIftaDlg::OnBnClickedDataDelete)
    ON_MESSAGE( WM_EVENT( EVENT_FMI_IFTA_FILE_TRANSFER_RECEIVED ), OnEventIftaFileTransferReceived )
END_MESSAGE_MAP()

static time_type convertDateTime( int year, int month, int day, int hour, int minute, int second )
    {
    time_type timestamp;
    time_type time, date;
    date_time_data_type date_time;
    date_time.date.year = (uint16) year;
    date_time.date.month = (uint8) month;
    date_time.date.day = (uint8) day;
    date_time.time.hour = (sint16) hour;
    date_time.time.minute = (uint8) minute;
    date_time.time.second = (uint8) second;
    UTIL_convert_time_type_to_seconds( &date_time, &time );
    UTIL_convert_date_time_to_seconds( &date_time, &date );
    timestamp = date + time;
    UTIL_convert_local_to_UTC( &timestamp, &timestamp );
    return timestamp;
    }

// CIftaDlg message handlers

//----------------------------------------------------------------------
//! \brief Attempt to fetch IFTA data from the device.
//! \details Validate the selected timeframe and if valid, send fetch
//!    command.
//----------------------------------------------------------------------
void CIftaDlg::OnBnClickedDataFetch()
    {
    UpdateData();

    time_type start = convertDateTime(
        mStartDate.GetYear(), mStartDate.GetMonth(), mStartDate.GetDay(),
        mStartTime.GetHour(), mStartTime.GetMinute(), mStartTime.GetSecond()
    );

    time_type end = convertDateTime(
        mEndDate.GetYear(), mEndDate.GetMonth(), mEndDate.GetDay(),
        mEndTime.GetHour(), mEndTime.GetMinute(), mEndTime.GetSecond()
    );

    if (start <= end)
        {
        fmi_ifta_data_fetch_request_type message;
        memset( &message, 0, sizeof( message ) );
        message.start_time = start;
        message.end_time = end;
        mCom.sendFmiPacket( FMI_IFTA_DATA_FETCH_REQUEST, (uint8*)&message, sizeof( message ) );
        }
    }

//----------------------------------------------------------------------
//! \brief Attempt to delete IFTA data on the device.
//! \details Validate the selected timeframe and if valid, send delete
//!    command.
//----------------------------------------------------------------------
void CIftaDlg::OnBnClickedDataDelete()
    {
    UpdateData();

    time_type start = convertDateTime(
        mStartDate.GetYear(), mStartDate.GetMonth(), mStartDate.GetDay(),
        mStartTime.GetHour(), mStartTime.GetMinute(), mStartTime.GetSecond()
    );

    time_type end = convertDateTime(
        mEndDate.GetYear(), mEndDate.GetMonth(), mEndDate.GetDay(),
        mEndTime.GetHour(), mEndTime.GetMinute(), mEndTime.GetSecond()
    );

    if (start <= end)
        {
        fmi_ifta_data_delete_request_type message;
        memset( &message, 0, sizeof( message ) );
        message.start_time = start;
        message.end_time = end;
        mCom.sendFmiPacket( FMI_IFTA_DATA_DELETE_REQUEST, (uint8*)&message, sizeof( message ) );
        }
    }

//----------------------------------------------------------------------
//! \brief Attempt to unzip received file and shell open the result.
//! \details Attempt to unzip received file and shell open the result.
//----------------------------------------------------------------------
afx_msg LRESULT CIftaDlg::OnEventIftaFileTransferReceived( WPARAM, LPARAM )
{
    TCHAR tmpPath[ MAX_PATH-14 ];
    TCHAR tmpFileName[ MAX_PATH ];

    memset( tmpPath, 0, sizeof( tmpPath ) );
    memset( tmpFileName, 0, sizeof( tmpFileName ) );

    GetTempPath( MAX_PATH-14, tmpPath );
    GetTempFileName( tmpPath, _T( "IFTA" ), 0, tmpFileName );

    CString tmpFile( tmpFileName );
    tmpFile += _T( ".csv" );

    if( unzip( SAVE_RECEIVED_IFTA_FILE, tmpFile ) )
        {
        if( IDYES == MessageBox(
                _T( "An IFTA data file has been received from the device.\nWould you like to view the contents?" ),
                _T( "IFTA Data Fetch" ), MB_YESNO | MB_ICONQUESTION
            ) )
            {
            // open up the file with whatever program handles the CSV file type (most likely Excel)
            // use My Documents as the working dir in hopes that Excel will default to that folder when saving
            TCHAR myDocs[ MAX_PATH ];
            if( S_OK == SHGetFolderPath( NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, myDocs ) )
                {
                ShellExecute( NULL, _T( "open" ), tmpFile, NULL, myDocs, SW_SHOW );
                }
            else {
                ShellExecute( NULL, _T( "open" ), tmpFile, NULL, NULL, SW_SHOW );
                }
            }
        }
    return 0;
}
#endif
