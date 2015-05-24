/*********************************************************************
*
*   MODULE NAME:
*       CMapViewerDlg.cpp
*
* Copyright 2013 by Garmin Ltd. or its subsidiaries.
*---------------------------------------------------------------------
* $NoKeywords$
*********************************************************************/
#include "stdafx.h"
#include "CMapViewerDlg.h"
#include "CFileTransferDlg.h"
#include "CFileTransferProgressDlg.h"
#include "util.h"
#include "comdef.h"
#include "StopListItem.h"

#import <msxml6.dll> named_guids

using namespace MSXML2;

#if( FMI_SUPPORT_A613 )

#if( FMI_SUPPORT_A614 )
#define SEND_ROUTE_TEMP_FILE _T( "send_route_temp.bin" )
#endif

// CMapViewerDlg dialog

IMPLEMENT_DYNAMIC(CMapViewerDlg, CDialog)

BEGIN_MESSAGE_MAP(CMapViewerDlg, CDialog)
    ON_WM_SIZE()
END_MESSAGE_MAP()

//----------------------------------------------------------------------
//! \brief Constructor
//----------------------------------------------------------------------
CMapViewerDlg::CMapViewerDlg(CWnd* pParent, FmiApplicationLayer & aCom)
    : CDialog( CMapViewerDlg::IDD, pParent ),
    mCom( aCom )
    {
    }

//----------------------------------------------------------------------
//! \brief Destructor
//----------------------------------------------------------------------
CMapViewerDlg::~CMapViewerDlg()
{
    CoUninitialize();
}

//----------------------------------------------------------------------
//! \brief Perform dialog data exchange and validation
//! \param  aDataExchange The DDX context
//----------------------------------------------------------------------
void CMapViewerDlg::DoDataExchange(CDataExchange* pDX)
   {
   CDialog::DoDataExchange(pDX);
   DDX_Control( pDX, IDC_EMBEDDED_BROWSER, m_browser );
   }

//----------------------------------------------------------------------
//! \brief This function is called when the window is created.
//! \return TRUE, since this function does not set focus to a control
//----------------------------------------------------------------------
BOOL CMapViewerDlg::OnInitDialog()
    {
    CoInitialize(NULL);

    CDialog::OnInitDialog();

    refreshCatalog();

    return TRUE;
    }

//----------------------------------------------------------------------
//! \brief Called after the dialog is resized; repositions the contents
//!     of the display.
//! \details Moves/sizes the contents as follows:
//!     - Moves the search box to the bottom-left corner, keeping its
//!       size
//!     - Moves the packet list to the top-left, expands vertically
//!     - Moves the packet detail to the right of the list, expands
//!       horizontally to fill the dialog and vertically to match the
//!       height of the packet list
//!     - Centers the buttons under the packet detail.
//! \param aType The type of resizing requested (maximized, etc.)
//! \param aClientWidth The new width of the client area
//! \param aClientHeight The new height of the client area
//----------------------------------------------------------------------
void CMapViewerDlg::OnSize
    (
    UINT aType,
    int  aClientWidth,
    int  aClientHeight
    )
{
    CDialog::OnSize( aType, aClientWidth, aClientHeight );

    if ( NULL == m_browser.GetSafeHwnd() )
        {
        return;
        }

    UpdateData();

    CRect rect;
    GetClientRect( &rect );

    m_browser.MoveWindow( rect.left, rect.top, rect.Width(), rect.Height() );
}

//----------------------------------------------------------------------
//! \brief This function is a helper for writing text to a file
//----------------------------------------------------------------------
static void write(CFile * catalog, char * text)
    {
    catalog->Write( ( const void * )text, ( UINT ) strlen( text ) );
    }

//----------------------------------------------------------------------
//! \brief This function is a helper for writing Unicode text to a file
//----------------------------------------------------------------------
static void write(CFile * catalog, LPCTSTR wText)
    {
    // convert Unicode to character array
    int textLen = WideCharToMultiByte( CP_UTF8, 0, wText, -1, NULL, 0, NULL, NULL );
    char * text = new char[textLen];
    WideCharToMultiByte( CP_UTF8, 0, wText, -1, text, textLen, NULL, NULL );
    catalog->Write( ( const void * ) text, textLen-1 );
    delete text;
    }

//----------------------------------------------------------------------
//! \brief This function is a helper for writing Unicode text to a file
//----------------------------------------------------------------------
static void write(CFile * catalog, LPCTSTR wText, int maxLength )
    {
    // convert Unicode to character array
    int textLen = WideCharToMultiByte( CP_UTF8, 0, wText, -1, NULL, 0, NULL, NULL );
    char * text = new char[textLen];
    WideCharToMultiByte( CP_UTF8, 0, wText, -1, text, textLen, NULL, NULL );
    if ( textLen > maxLength )
        {
        catalog->Write( ( const void * ) text, maxLength-1 );
        }
    else {
        catalog->Write( ( const void * ) text, textLen-1 );
        }
    catalog->Write( "\0", 1 );
    delete text;
    }

//----------------------------------------------------------------------
//! \brief This function is called to reload the catalog document
//----------------------------------------------------------------------
void CMapViewerDlg::refreshCatalog()
    {
    _variant_t    flags( 0L, VT_I4 );
    _variant_t    target_frame_name( "" );
    _variant_t    post_data( "" );
    _variant_t    headers( "" );

    CFile catalog;
    CFileException pEx;
    if ( !catalog.Open( _T( "map-viewer\\catalog.xml" ), CFile::modeRead, &pEx ) )
        {
        if ( !catalog.Open( _T( "map-viewer\\catalog.xml" ), CFile::modeCreate | CFile::modeWrite, &pEx ) )
            {
            m_browser.Navigate( _T( "about:blank" ), &flags, &target_frame_name, &post_data, &headers );
            TCHAR msg[256];
            pEx.GetErrorMessage( msg, 256 );
            m_browser.WriteContent( CString( msg ) );
            return;
            }

            //build empty catalog with XSL directive
            write( &catalog, "<?xml version=\"1.0\" encoding=\"utf-8\"?><?xml-stylesheet type=\"text/xsl\" href=\"catalog.xsl\"?><catalog></catalog>\r\n" );
        }
    CString url( "file:///" );
    url += catalog.GetFilePath();

    catalog.Close();

    m_browser.Navigate( url, &flags, &target_frame_name, &post_data, &headers );
    }

// CMapViewerDlg message handlers
BEGIN_EVENTSINK_MAP(CMapViewerDlg, CDialog)
    ON_EVENT(CMapViewerDlg, IDC_EMBEDDED_BROWSER, 250, CMapViewerDlg::OnBeforeNavigate2, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
END_EVENTSINK_MAP()

//----------------------------------------------------------------------
//! \brief Handle application-specific navigation URLs
//----------------------------------------------------------------------
void CMapViewerDlg::OnBeforeNavigate2(LPDISPATCH, VARIANT* URL, VARIANT*, VARIANT*, VARIANT*, VARIANT*, BOOL* Cancel)
    {
    if ( URL->vt != VT_BSTR )
        {
        return;
        }

    CString urlStr = URL->bstrVal;
    if ( urlStr.Find( _T( "app:") ) == 0 )
        {
        // take over
        *Cancel = TRUE;

        CString command = urlStr.Right( urlStr.GetLength() - (int) strlen( "app:" ) );
        CString procedure = command.Left( command.Find( '(' ) );
        CString args = command.Mid( procedure.GetLength() + 1, command.ReverseFind( ')') - ( procedure.GetLength() + 1 ) );
        CStringArray arguments;
        //split arguments separated by comma
        int curPos = 0;
        CString token = args.Tokenize( _T( "," ), curPos );
        while ( token != _T( "" ) )
            {
            if ( 0 == token.Find( '"' ) )
                {
                if ( token.GetLength()-1 == token.ReverseFind( '"' ) )
                    {
                    token = token.Mid( 1, token.GetLength() - 2 );
                    token = CWebBrowser2::URLDecode( token );
                    }
                else {
                    token += _T( "," );
                    token += args.Tokenize( _T( "," ), curPos );
                    continue;
                    }
                }
            arguments.Add( token );
            token = args.Tokenize( _T( "," ), curPos );
            }

        TRACE2( "app command '%s' executed with args '%s'\r\n", procedure, args );

        if ( procedure == _T( "addAvoidanceToCatalog" ) )
            {
            if ( arguments.GetCount() < 7 )
                {
                MessageBox( _T( "Insufficient arguments"), _T( "Add Region" ), MB_ICONERROR );
                return;
                }
            addAvoidanceToCatalog(
                _ttoi( arguments[0] ),
                arguments[1],
                arguments[2],
                _ttol( arguments[3] ),
                _ttol( arguments[4] ),
                _ttol( arguments[5] ),
                _ttol( arguments[6] )
            );
            }
        else if ( procedure == _T( "removeAvoidanceFromCatalog" ) )
            {
            BOOL removed = removeAvoidanceFromCatalog( _ttoi( arguments[0] ) );
            if ( arguments.GetCount() > 1 )
                {
                CStringArray callbackArgs;
                callbackArgs.Add( removed ? _T( "true" ) : _T( "false" ) );
                callbackArgs.Add( arguments[0] );
                m_browser.InvokeScript( arguments[1], &callbackArgs );
                }
            }
        else if ( procedure == _T( "sendAvoidanceToDevice" ) )
            {
            if ( arguments.GetCount() < 6 )
                {
                MessageBox( _T( "Insufficient arguments"), _T( "Send to Device" ), MB_ICONERROR );
                return;
                }
            BOOL sent = sendAvoidanceToDevice(
                _ttoi( arguments[0] ),
                arguments[1],
                _ttol( arguments[2] ),
                _ttol( arguments[3] ),
                _ttol( arguments[4] ),
                _ttol( arguments[5] )
            );
            if ( arguments.GetCount() > 6 )
                {
                CStringArray callbackArgs;
                callbackArgs.Add( sent ? _T( "true" ) : _T( "false" ) );
                m_browser.InvokeScript( arguments[6], &callbackArgs);
                }
            }
        else if ( procedure == _T( "deleteAvoidanceFromDevice" ) )
            {
            BOOL sent = deleteAvoidanceFromDevice( _ttoi( arguments[0] ) );
            if ( arguments.GetCount() > 1 )
                {
                CStringArray callbackArgs;
                callbackArgs.Add( sent ? _T( "true" ) : _T( "false" ) );
                m_browser.InvokeScript( arguments[1], &callbackArgs);
                }
            }
        else if ( procedure == _T( "enableAvoidanceOnDevice" ) )
            {
            BOOL sent = setAvoidanceEnabled( _ttoi( arguments[0] ), TRUE );
            if ( arguments.GetCount() > 1 )
                {
                CStringArray callbackArgs;
                callbackArgs.Add( sent ? _T( "true" ) : _T( "false" ) );
                m_browser.InvokeScript( arguments[1], &callbackArgs);
                }
            }
        else if ( procedure == _T( "disableAvoidanceOnDevice" ) )
            {
            BOOL sent = setAvoidanceEnabled( _ttoi( arguments[0] ), FALSE );
            if ( arguments.GetCount() > 1 )
                {
                CStringArray callbackArgs;
                callbackArgs.Add( sent ? _T( "true" ) : _T( "false" ) );
                m_browser.InvokeScript( arguments[1], &callbackArgs);
                }
            }
#if( FMI_SUPPORT_A614 )
        else if ( procedure == _T( "beginAddRouteToCatalog" ) )
            {
            m_addRouteArguments.RemoveAll();
            m_addRouteArguments.Append( arguments );
            }
        else if ( procedure == _T( "continueAddRouteToCatalog" ) )
            {
            m_addRouteArguments.Append( arguments );
            }
        else if ( procedure == _T( "endAddRouteToCatalog" ) )
            {
            if ( removeRouteFromCatalog( _ttoi( m_addRouteArguments[0] ) ) )
                {
                addRouteToCatalog( m_addRouteArguments );
                }
            m_addRouteArguments.RemoveAll();
            }
        else if ( procedure == _T( "removeRouteFromCatalog" ) )
            {
            BOOL removed = removeRouteFromCatalog( _ttoi( arguments[0] ) );
            if ( arguments.GetCount() > 1 )
                {
                CStringArray callbackArgs;
                callbackArgs.Add( removed ? _T( "true" ) : _T( "false" ) );
                callbackArgs.Add( arguments[0] );
                m_browser.InvokeScript( arguments[1], &callbackArgs );
                }
            }
        else if ( procedure == _T( "sendRouteToDevice" ) )
            {
            if ( arguments.GetCount() == 1 )
                {
                BOOL sent = sendRouteToDevice( _ttoi( arguments[0] ) );
                }
            }
        else if ( procedure == _T( "deleteRouteFromDevice" ) )
            {
            if ( arguments.GetCount() == 1 )
                {
                BOOL sent = deleteRouteFromDevice( _ttoi( arguments[0] ) );
                if ( arguments.GetCount() > 1 )
                    {
                    CStringArray callbackArgs;
                    callbackArgs.Add( sent ? _T( "true" ) : _T( "false" ) );
                    m_browser.InvokeScript( arguments[1], &callbackArgs);
                    }
                }
            }
        else if ( procedure == _T( "activateStopOnDevice" ) )
            {
            if ( arguments.GetCount() == 1 )
                {
                BOOL sent = activateStopOnDevice( _ttoi( arguments[0] ) );
                if ( arguments.GetCount() > 1 )
                    {
                    CStringArray callbackArgs;
                    callbackArgs.Add( sent ? _T( "true" ) : _T( "false" ) );
                    m_browser.InvokeScript( arguments[1], &callbackArgs);
                    }
                }
            }
        else if ( procedure == _T( "completeStopOnDevice" ) )
            {
            if ( arguments.GetCount() == 1 )
                {
                BOOL sent = completeStopOnDevice( _ttoi( arguments[0] ) );
                    if ( arguments.GetCount() > 1 )
                    {
                    CStringArray callbackArgs;
                    callbackArgs.Add( sent ? _T( "true" ) : _T( "false" ) );
                    m_browser.InvokeScript( arguments[1], &callbackArgs);
                    }
                }
            }
        else if ( procedure == _T( "moveStopOnDevice" ) )
            {
            if ( arguments.GetCount() == 2 )
                {
                BOOL sent = moveStopOnDevice( _ttoi( arguments[0] ), _ttoi( arguments[1] ) );
                if ( arguments.GetCount() > 2 )
                    {
                    CStringArray callbackArgs;
                    callbackArgs.Add( sent ? _T( "true" ) : _T( "false" ) );
                    m_browser.InvokeScript( arguments[2], &callbackArgs);
                    }
                }
            }
#endif
        }
    }

//----------------------------------------------------------------------
//! \brief This function adds an item to the catalog
//! \details Load the catalog document, create the new region element,
//!     add that to the catalog element, and then save the document
//! \return TRUE if the operation completed successfully
//----------------------------------------------------------------------
BOOL CMapViewerDlg::addAvoidanceToCatalog( int itemId, LPCTSTR name, LPCTSTR description, long north, long east, long south, long west )
    {
    // load the catalog
    CString documentPath( _T( "map-viewer\\catalog.xml" ) );
    MSXML2::IXMLDOMDocument2Ptr doc( MSXML2::CLSID_DOMDocument60 );
    doc->async = VARIANT_FALSE;
    doc->validateOnParse = VARIANT_FALSE;
    doc->resolveExternals = VARIANT_TRUE;

    if ( VARIANT_TRUE == doc->load( _variant_t( documentPath ) ) )
        {
        // create new element and add to document root (catalog)
        MSXML2::IXMLDOMElementPtr newRegion = doc->createElement( _bstr_t( "region" ) );
        if ( 0 != _tcslen( description ) )
            {
            MSXML2::IXMLDOMNodePtr descr = doc->createElement( _bstr_t( "description" ) );
            descr->put_text( _bstr_t( description ) );
            newRegion->appendChild( descr );
            }
        newRegion->setAttribute( _bstr_t( "id" ), _variant_t( itemId ) );
        newRegion->setAttribute( _bstr_t( "name" ), _variant_t( name ) );
        newRegion->setAttribute( _bstr_t( "north" ), _variant_t( north ) );
        newRegion->setAttribute( _bstr_t( "east" ), _variant_t( east ) );
        newRegion->setAttribute( _bstr_t( "south" ), _variant_t( south ) );
        newRegion->setAttribute( _bstr_t( "west" ), _variant_t( west ) );
        doc->GetdocumentElement()->appendChild( newRegion );

        // store updated catalog
        HRESULT hr = doc->save( _variant_t( documentPath ) );
        if ( SUCCEEDED( hr ) )
            {
            return TRUE;
            }
        }

    return FALSE;
    }

//----------------------------------------------------------------------
//! \brief This function removes specific regions from the catalog
//! \details Load the catalog document, search for all regions by id,
//!     remove them, and then save the document
//! \return TRUE if the operation completed successfully
//----------------------------------------------------------------------
BOOL CMapViewerDlg::removeAvoidanceFromCatalog(int id)
    {
    CString path;
    path.Format( _T( "region[@id=%d]" ), id );
    return removeItemsFromCatalog( path );
    }

//----------------------------------------------------------------------
//! \brief This function sends the specified region to the device
//! \details This passes data directly to the device. It does not load
//!   any information from the catalog
//! \return TRUE if the operation completed successfully
//----------------------------------------------------------------------
BOOL CMapViewerDlg::sendAvoidanceToDevice( int itemId, LPCTSTR name, long north, long east, long south, long west )
    {
    custom_avoid_type message;
    memset( &message, 0, sizeof( message ) );

    message.unique_id = (uint16) itemId;

    message.point1.lat = north;
    message.point1.lon = east;
    message.point2.lat = south;
    message.point2.lon = west;

    message.enable = true;

    WideCharToMultiByte( mCom.mClientCodepage, 0, name, -1, message.name, sizeof( message.name ) - 1, NULL, NULL );
    message.name[sizeof( message.name ) - 1] = '\0';

    mCom.sendFmiPacket( FMI_CUSTOM_AVOID_ADD_REQUEST, (uint8*)&message, sizeof( message ) );

    return TRUE;
    }

//----------------------------------------------------------------------
//! \brief This function deletes the specified region from the device
//! \return TRUE if the operation completed successfully
//----------------------------------------------------------------------
BOOL CMapViewerDlg::deleteAvoidanceFromDevice(int id)
    {
    custom_avoid_delete_type message;
    memset( &message, 0, sizeof( message ) );

    message.unique_id = (uint16) id;

    mCom.sendFmiPacket( FMI_CUSTOM_AVOID_DEL_REQUEST, (uint8*)&message, sizeof( message ) );

    return TRUE;
    }

//----------------------------------------------------------------------
//! \brief This function toggles enabled for the region on the device
//! \return TRUE if the operation completed successfully
//----------------------------------------------------------------------
BOOL CMapViewerDlg::setAvoidanceEnabled(int id, BOOL enabled)
    {
    custom_avoid_enable_type message;
    memset( &message, 0, sizeof( message ) );

    message.unique_id = (uint16) id;
    message.enable = enabled == TRUE;

    mCom.sendFmiPacket( FMI_CUSTOM_AVOID_TOGGLE_REQUEST, (uint8*)&message, sizeof( message ) );

    return TRUE;
    }

#if( FMI_SUPPORT_A614 )
//----------------------------------------------------------------------
//! \brief This function adds an item to the catalog
//! \details Load the catalog document, create the new route element,
//!     add that to the catalog element, and then save the document
//! \return TRUE if the operation completed successfully
//----------------------------------------------------------------------
BOOL CMapViewerDlg::addRouteToCatalog( CStringArray & args )
    {
    // load the catalog
    CString documentPath( _T( "map-viewer\\catalog.xml" ) );
    MSXML2::IXMLDOMDocument2Ptr doc( MSXML2::CLSID_DOMDocument60 );
    doc->async = VARIANT_FALSE;
    doc->validateOnParse = VARIANT_FALSE;
    doc->resolveExternals = VARIANT_TRUE;

    if ( VARIANT_TRUE == doc->load( _variant_t( documentPath ) ) )
        {
        // create new element and add to document root (catalog)
        MSXML2::IXMLDOMElementPtr newItem = doc->createElement( _bstr_t( "route" ) );
        newItem->setAttribute( _bstr_t( "id" ), _variant_t( _ttoi( args[0] ) ) );
        newItem->setAttribute( _bstr_t( "name" ), _variant_t( args[1] ) );
        if ( 0 != _tcslen( args[2] ) )
            {
            MSXML2::IXMLDOMElementPtr descr = doc->createElement( _bstr_t( "description" ) );
            descr->put_text( _bstr_t( args[2] ) );
            newItem->appendChild( descr );
            }
        newItem->setAttribute( _bstr_t( "end-lat" ), _variant_t( _ttol( args[3] ) ) );
        newItem->setAttribute( _bstr_t( "end-lon" ), _variant_t( _ttol( args[4] ) ) );
        for ( int i=5; i+4<=args.GetCount(); i+=4 )
            {
            MSXML2::IXMLDOMElementPtr intermediate = doc->createElement( 0 == _ttoi( args[i] ) ? _bstr_t( "intermediate" ) : _bstr_t( "shaping" ) );
            intermediate->setAttribute( _bstr_t( "name" ), _variant_t( args[i+1] ) );
            intermediate->setAttribute( _bstr_t( "lat" ), _variant_t( _ttol( args[i+2] ) ) );
            intermediate->setAttribute( _bstr_t( "lon" ), _variant_t( _ttol( args[i+3] ) ) );
            newItem->appendChild( intermediate );
            }
        doc->GetdocumentElement()->appendChild( newItem );

        // store updated catalog
        HRESULT hr = doc->save( _variant_t( documentPath ) );
        if ( SUCCEEDED( hr ) )
            {
            return TRUE;
            }
        }

    return FALSE;
    }

//----------------------------------------------------------------------
//! \brief This function removes specific routes from the catalog
//! \details Load the catalog document, search for all routes by id,
//!     remove them, and then save the document
//! \return TRUE if the operation completed successfully
//----------------------------------------------------------------------
BOOL CMapViewerDlg::removeRouteFromCatalog(int id)
    {
    CString path;
    path.Format( _T( "route[@id=%d]" ), id );
    return removeItemsFromCatalog( path );
    }

//----------------------------------------------------------------------
//! \brief This function sends the specified route to the device
//! \details This passes data directly to the device. It does load
//!   information from the catalog
//! \return TRUE if the operation completed successfully
//----------------------------------------------------------------------
BOOL CMapViewerDlg::sendRouteToDevice( int itemId )
    {
    char file[MAX_PATH+1];
    char    versionString[35];
    uint8   version[16];
    uint8   versionLength;

    memset( version, 0, sizeof( version ) );
    WideCharToMultiByte( mCom.mClientCodepage, 0, _T( "1" ), -1, versionString, 34, NULL, NULL );
    versionString[34] = '\0';
    versionLength = (uint8)minval( 16, strlen( versionString ) );
    memmove( version, versionString, versionLength );

    WideCharToMultiByte( CP_ACP, 0, SEND_ROUTE_TEMP_FILE, -1, file, MAX_PATH+1, NULL, NULL );
    file[MAX_PATH] = '\0';

    //read route info from catalog
    CString documentPath( _T( "map-viewer\\catalog.xml" ) );
    MSXML2::IXMLDOMDocument2Ptr doc( MSXML2::CLSID_DOMDocument60 );
    doc->async = VARIANT_FALSE;
    doc->validateOnParse = VARIANT_FALSE;
    doc->resolveExternals = VARIANT_TRUE;

    if ( VARIANT_TRUE != doc->load( _variant_t( documentPath ) ) )
        {
        return FALSE;
        }

    CString selector;
    selector.Format( _T( "route[@id=%d]" ), itemId );
    MSXML2::IXMLDOMNodeListPtr nodes = doc->GetdocumentElement()->selectNodes( _bstr_t( selector ) );
    if ( 0 == nodes->Getlength() )
        {
        return FALSE;
        }
    MSXML2::IXMLDOMElementPtr routeNode = nodes->Getitem( 0 );
    //serialize it to a temporary file
    CFile tmpFile;
    if ( !tmpFile.Open( SEND_ROUTE_TEMP_FILE, CFile::modeCreate | CFile::modeWrite ) )
        {
        return FALSE;
        }

    CString nameString = routeNode->getAttribute( "name" );
    CString stopText = nameString;
    MSXML2::IXMLDOMNodeListPtr childNodes = routeNode->GetchildNodes();

    BYTE isShaping = 1;
    short ver = 0;
    time_type origination_time = UTIL_get_current_garmin_time();
    uint32 unique_id = itemId;

    write( &tmpFile, "PSR+" );
    tmpFile.Write( ( const void* ) &ver, sizeof( ver ) );

    //use the route description as the Stop Text, if available
    //otherwise fall back to the route name
    for ( int i=0, count = childNodes->Getlength(); i<count; i++ )
        {
        MSXML2::IXMLDOMElementPtr childNode = childNodes->Getitem( i );
        if ( childNode->GetnodeName() == _bstr_t( "description" ) )
            {
            nameString = stopText = (LPCTSTR) childNode->Gettext();
            break;
            }
        }
    tmpFile.Write( ( const void* ) &origination_time, sizeof( origination_time ) );
    tmpFile.Write( ( const void* ) &unique_id, sizeof( unique_id ) );
    write( &tmpFile, nameString, 200 );

    sc_position_type pos;

    //send them to the device in reverse order since the catalog stores them last to first
    for ( int i=childNodes->Getlength() - 1; i>=0; i-- )
        {
        MSXML2::IXMLDOMElementPtr childNode = childNodes->Getitem( i );
        if ( childNode->GetnodeName() == _bstr_t( "intermediate" ) )
            {
            pos.lat = childNode->getAttribute( _T( "lat" ) );
            pos.lon = childNode->getAttribute( _T( "lon" ) );
            tmpFile.Write( ( const void *) &pos.lat, sizeof( pos.lat ) );
            tmpFile.Write( ( const void *) &pos.lon, sizeof( pos.lon ) );
            tmpFile.Write( "\0", 1 );
            nameString = childNode->getAttribute( _T( "name" ) );
            write( &tmpFile, nameString, 40 );
            }
        else if ( childNode->GetnodeName() == _bstr_t( "shaping" ) )
            {
            pos.lat = childNode->getAttribute( _T( "lat" ) );
            pos.lon = childNode->getAttribute( _T( "lon" ) );
            tmpFile.Write( ( const void *) &pos.lat, sizeof( pos.lat ) );
            tmpFile.Write( ( const void *) &pos.lon, sizeof( pos.lon ) );
            tmpFile.Write( ( const void* ) &isShaping, 1 );
            }
        }

    //now write the final destination (the stop) information contained on the top-level route element
    pos.lat = routeNode->getAttribute( _T( "end-lat" ) );
    pos.lon = routeNode->getAttribute( _T( "end-lon" ) );
    tmpFile.Write( ( const void *) &pos.lat, sizeof( pos.lat ) );
    tmpFile.Write( ( const void *) &pos.lon, sizeof( pos.lon ) );
    tmpFile.Write( "\0", 1 );
    nameString = routeNode->getAttribute( _T( "name" ) );
    write( &tmpFile, nameString, 40 );

    tmpFile.Flush();
    tmpFile.Close();

    mCom.sendFile( file, versionLength, version, (uint8) FMI_FILE_TYPE_PATH_SPECIFIC_ROUTE );

    //add this to the Stops list now so that the UI can receive status updates, as opposed
    //to adding it after the transfer progress dialog returns from modal, which may happen
    //after the device reports a status update of the new stop it receives to the Stops dialog
    StopListItem& stopListItem = mCom.mA603Stops.get( itemId );
    stopListItem.setCurrentName( stopText );
    stopListItem.setStopStatus( STOP_STATUS_UNREAD );
    stopListItem.setValid();
    mCom.mA603Stops.put( stopListItem );

    CFileTransferProgressDlg sending_dlg( this, mCom );
    UINT sendResult = sending_dlg.DoModal();
    //remove the record from the Stops list if the file transfer failed
    if ( IDOK != sendResult )
        {
        mCom.mA603Stops.remove( itemId );
        }

    return TRUE;
    }

//----------------------------------------------------------------------
//! \brief This function deletes the specified route from the device
//! \return TRUE if the operation completed successfully
//----------------------------------------------------------------------
BOOL CMapViewerDlg::deleteRouteFromDevice(int id)
    {
    mCom.sendStopStatusRequest( id, REQUEST_DELETE_STOP );
    return TRUE;
    }

//----------------------------------------------------------------------
//! \brief This function activates the specified route on the device
//! \return TRUE if the operation completed successfully
//----------------------------------------------------------------------
BOOL CMapViewerDlg::activateStopOnDevice(int id)
    {
    mCom.sendStopStatusRequest( id, REQUEST_ACTIVATE_STOP );
    return TRUE;
    }

//----------------------------------------------------------------------
//! \brief This function marks the specified route as done on the device
//! \return TRUE if the operation completed successfully
//----------------------------------------------------------------------
BOOL CMapViewerDlg::completeStopOnDevice(int id)
    {
    mCom.sendStopStatusRequest( id, REQUEST_MARK_STOP_DONE );
    return TRUE;
    }

//----------------------------------------------------------------------
//! \brief This function repositions the specified stop on the device
//! \return TRUE if the operation completed successfully
//----------------------------------------------------------------------
BOOL CMapViewerDlg::moveStopOnDevice(int id, short newPosition)
    {
    mCom.sendStopStatusRequest( id, REQUEST_MOVE_STOP, newPosition );
    return TRUE;
    }

#endif

//----------------------------------------------------------------------
//! \brief This function removes specific routes from the catalog
//! \details Load the catalog document, search for all routes by id,
//!     remove them, and then save the document
//! \return TRUE if the operation completed successfully
//----------------------------------------------------------------------
BOOL CMapViewerDlg::removeItemsFromCatalog(LPCTSTR selector)
    {
    // load the catalog
    CString documentPath( _T( "map-viewer\\catalog.xml" ) );
    MSXML2::IXMLDOMDocument2Ptr doc( MSXML2::CLSID_DOMDocument60 );
    doc->async = VARIANT_FALSE;
    doc->validateOnParse = VARIANT_FALSE;
    doc->resolveExternals = VARIANT_TRUE;

    if ( VARIANT_TRUE == doc->load( _variant_t( documentPath ) ) )
        {
        // find node(s) by specified selector and then remove from the root element (catalog)
        MSXML2::IXMLDOMNodeListPtr nodes = doc->GetdocumentElement()->selectNodes( _bstr_t( selector ) );
        for ( int i=nodes->Getlength()-1; i>=0; i-- )
            {
            doc->GetdocumentElement()->removeChild( nodes->Getitem( i ) );
            }

        // store updated catalog
        HRESULT hr = doc->save( _variant_t( documentPath ) );
        if ( SUCCEEDED( hr ) )
            {
            return TRUE;
            }
        }

    return FALSE;
    }
#endif