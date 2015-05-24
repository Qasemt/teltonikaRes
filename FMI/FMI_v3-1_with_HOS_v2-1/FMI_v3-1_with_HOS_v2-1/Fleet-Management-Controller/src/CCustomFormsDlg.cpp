/*********************************************************************
*
*   MODULE NAME:
*       CCustomFormsDlg.cpp
*
* Copyright 2013 by Garmin Ltd. or its subsidiaries.
*---------------------------------------------------------------------
* $NoKeywords$
*********************************************************************/
#include "stdafx.h"
#include "CCustomFormsDlg.h"
#include "Event.h"
#include "CFileTransferDlg.h"
#include "CFileTransferProgressDlg.h"
#include "FileStream.h"
#include "comdef.h"
#include "util.h"

#import <msxml6.dll> named_guids

using namespace MSXML2;

#if( FMI_SUPPORT_A612 )

// CCustomFormsDlg dialog

IMPLEMENT_DYNAMIC(CCustomFormsDlg, CDialog)

BEGIN_MESSAGE_MAP(CCustomFormsDlg, CDialog)
    ON_MESSAGE( WM_EVENT( EVENT_FMI_FORM_DELETED ), OnFormDeleted )
    ON_MESSAGE( WM_EVENT( EVENT_FMI_FORM_POSITION_CHANGED ), OnFormPositionChanged )
    ON_MESSAGE( WM_EVENT( EVENT_FMI_FORM_SUBMIT_RECEIVED ), OnFormSubmitReceived )
    ON_WM_SIZE()
END_MESSAGE_MAP()

//----------------------------------------------------------------------
//! \brief Constructor
//----------------------------------------------------------------------
CCustomFormsDlg::CCustomFormsDlg(CWnd* pParent, FmiApplicationLayer & aCom)
    : CDialog( CCustomFormsDlg::IDD, pParent ),
    mCom( aCom )
    {
    m_templateTransform = new XMLTransformer( _T( "custom-forms\\preview_template.xsl" ) );
    m_submittedTransform = new XMLTransformer( _T( "custom-forms\\preview_submitted.xsl" ) );
    }

//----------------------------------------------------------------------
//! \brief Destructor
//----------------------------------------------------------------------
CCustomFormsDlg::~CCustomFormsDlg()
{
    delete m_templateTransform;
    delete m_submittedTransform;

    CoUninitialize();
}

//----------------------------------------------------------------------
//! \brief Perform dialog data exchange and validation
//! \param  aDataExchange The DDX context
//----------------------------------------------------------------------
void CCustomFormsDlg::DoDataExchange(CDataExchange* pDX)
    {
    CDialog::DoDataExchange(pDX);
    DDX_Control( pDX, IDC_EMBEDDED_BROWSER, m_browser );
    }

//----------------------------------------------------------------------
//! \brief This function is called when the window is created.
//! \return TRUE, since this function does not set focus to a control
//----------------------------------------------------------------------
BOOL CCustomFormsDlg::OnInitDialog()
    {
    CoInitialize(NULL);

    CDialog::OnInitDialog();

    _variant_t    flags( 0L, VT_I4 );
    _variant_t    target_frame_name( "" );
    _variant_t    post_data( "" );
    _variant_t    headers( "" );

    // write catalog to file and then display in browser control
    CFile catalog;
    CFileException pEx;
    if ( !catalog.Open( _T( "custom-forms\\catalog.xml" ), CFile::modeCreate | CFile::modeWrite, &pEx ) )
        {
        m_browser.Navigate( _T( "about:blank" ), &flags, &target_frame_name, &post_data, &headers );
        TCHAR msg[256];
        pEx.GetErrorMessage( msg, 256 );
        m_browser.WriteContent( CString( msg ) );
        }
    else {
        CString filePath = catalog.GetFilePath();
        catalog.Close();

        if ( buildCatalog( filePath ) )
            {
            CString url( "file:///" );
            url += filePath;
            m_browser.Navigate( url, &flags, &target_frame_name, &post_data, &headers );
            }
        }

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
void CCustomFormsDlg::OnSize
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
//! \brief This function handles form delete ACK event from device
//----------------------------------------------------------------------
LRESULT CCustomFormsDlg::OnFormDeleted( WPARAM wParam, LPARAM )
    {
    CString message;
    message.Format( _T( "The device has reported that form template %d was deleted" ), (int) wParam );
    MessageBox( message, _T( "Form Template Deleted" ), MB_OK | MB_ICONINFORMATION );

    return 0;
    }

//----------------------------------------------------------------------
//! \brief This function handles form position report event from device
//----------------------------------------------------------------------
LRESULT CCustomFormsDlg::OnFormPositionChanged( WPARAM wParam, LPARAM lParam)
    {
    CString message;
    message.Format( _T( "The device has reported that form template %d is at position %d" ), (int) wParam, (uint8) lParam );
    MessageBox( message, _T( "Form Template Position" ), MB_OK | MB_ICONINFORMATION );

    return 0;
    }

//----------------------------------------------------------------------
//! \brief This function handles completed form submit event from device
//----------------------------------------------------------------------
LRESULT CCustomFormsDlg::OnFormSubmitReceived( WPARAM, LPARAM )
    {
    // attempt to open the file and read the form id and version
    MSXML2::IXMLDOMDocument2Ptr submitDoc( MSXML2::CLSID_DOMDocument60 );

    submitDoc->async = VARIANT_FALSE;
    submitDoc->validateOnParse = VARIANT_FALSE;
    submitDoc->resolveExternals = VARIANT_FALSE;
    //unzip if necessary into another file
    CString tempFile = unzip( SAVE_RECEIVED_FORM_SUBMIT );
    VARIANT_BOOL loaded = submitDoc->load( _variant_t( tempFile ) );
    if ( VARIANT_TRUE != loaded )
        {
        if ( submitDoc->parseError )
            {
            CString reason = submitDoc->parseError->Getreason();
            MessageBox( reason, _T( "Form Submitted"), MB_ICONERROR | MB_OK );
            }
        else
            {
            MessageBox( _T( "Unable to parse submitted form file" ), _T( "Form Submitted"), MB_ICONERROR | MB_OK );
            }
        return 0;
        }

    MSXML2::IXMLDOMNodePtr formId = submitDoc->selectSingleNode( _bstr_t( "//form/@id" ) );
    MSXML2::IXMLDOMNodePtr formVersion = submitDoc->selectSingleNode( _bstr_t( "//form/@version" ) );
    if ( formId == NULL )
        {
        MessageBox( _T( "Submitted form file's form element does not contain required attribute 'id'" ), _T( "Form Submitted"), MB_ICONERROR | MB_OK );
        return 0;
        }
    if ( formVersion == NULL)
        {
        MessageBox( _T( "Submitted form file's form element does not contain required attribute 'version'" ), _T( "Form Submitted"), MB_ICONERROR | MB_OK );
        return 0;
        }

    //store the file in the appropriate location - same folder as the corresponding form
    //even if the form file does not exist in the repository
    CStringArray folders;
    folders.Add( _T( "custom-forms" ) );
    folders.Add( _T( "library" ) );
    folders.Add( formId->Gettext() );
    folders.Add( formVersion->Gettext() );
    CString path;
    for ( int i=0; i<folders.GetCount(); i++ )
        {
        path += folders[i] + _T( "\\" );
        if ( ERROR_PATH_NOT_FOUND == CreateDirectory( path, NULL) )
            {
            MessageBox( _T( "Failed to create repository folder " ) + path, _T( "Form Submitted" ), MB_OK | MB_ICONERROR );
            return 0;
            }
        }

    COleDateTime submitTime = COleDateTime::GetCurrentTime();
    path += submitTime.Format( _T( "%Y-%m-%d_%H.%M.%S" ) );
    path += _T( ".xml" );
    if ( !MoveFile( tempFile, path ) )
        {
        MessageBox( _T( "Failed to move submitted form file to repository " ) + path, _T( "Form Submitted" ), MB_OK | MB_ICONERROR );
        return 0;
        }

    refreshCatalog();
    return 0;
    }

//----------------------------------------------------------------------
//! \brief This function reconstructs catalog file and redisplays it
//----------------------------------------------------------------------
void CCustomFormsDlg::refreshCatalog()
    {
    CFile catalog;
    CFileException pEx;
    if ( !catalog.Open( _T( "custom-forms\\catalog.xml" ), CFile::modeCreate | CFile::modeWrite, &pEx ) )
        {
        return;
        }

    CString filePath = catalog.GetFilePath();
    catalog.Close();

    if ( buildCatalog( filePath ) )
        {
        CString url( "file:///" );
        url += filePath;
        url.Replace( '\\', '/' );

        // refresh the browser if it is currently displaying the old catalog
        UpdateData();
        CString browserAddress = CWebBrowser2::URLDecode( m_browser.get_LocationURL() );
        if ( 0 == url.CompareNoCase( browserAddress ) )
            {
            m_browser.Refresh();
            }
        }
    }

//----------------------------------------------------------------------
//! \brief Construct a catalog document based on the library structure
//----------------------------------------------------------------------
bool CCustomFormsDlg::buildCatalog(LPCTSTR filePath)
    {
    MSXML2::IXMLDOMDocument2Ptr doc( MSXML2::CLSID_DOMDocument60 );
    doc->loadXML( _T( "<catalog></catalog>" ) );
    MSXML2::IXMLDOMElementPtr root = doc->GetdocumentElement();
    _variant_t vtObject;
    vtObject.vt = VT_DISPATCH;
    vtObject.pdispVal = root;
    vtObject.pdispVal->AddRef();
    MSXML2::IXMLDOMProcessingInstructionPtr xmlProc = doc->createProcessingInstruction( "xml", " version='1.0' encoding='UTF-8'" );
    doc->insertBefore( xmlProc, vtObject );
    MSXML2::IXMLDOMProcessingInstructionPtr xslProc = doc->createProcessingInstruction( "xml-stylesheet", " type='text/xsl' href='catalog.xsl'" );
    doc->insertBefore( xslProc, vtObject );

    CFileFind ff;
    // search for form folders
    CStringArray formFolders;
    BOOL bWorking = ff.FindFile( _T( "custom-forms\\library\\*" ) );
    while ( bWorking )
        {
        bWorking = ff.FindNextFileW();
        if ( ff.IsDirectory() && !ff.IsDots() )
            {
            formFolders.Add( ff.GetFileName() );
            }
        }
    ff.Close();

    // specify the schema for the form template validator since it isn't in same folder as loaded files (and cache it)
    MSXML2::IXMLDOMSchemaCollection2Ptr templateSchemas( MSXML2::CLSID_XMLSchemaCache60 );
    templateSchemas->add( _bstr_t( "" ), _bstr_t( "custom-forms\\form_template.xsd" ) );

    MSXML2::IXMLDOMDocument2Ptr templateDoc( MSXML2::CLSID_DOMDocument60 );
    templateDoc->schemas = templateSchemas.GetInterfacePtr();;

    templateDoc->async = VARIANT_FALSE;
    templateDoc->validateOnParse = VARIANT_TRUE;
    templateDoc->resolveExternals = VARIANT_TRUE;

    // specify the schema for the submitted form validator since it isn't in same folder as loaded files (and cache it)
    MSXML2::IXMLDOMSchemaCollection2Ptr submitSchemas( MSXML2::CLSID_XMLSchemaCache60 );
    submitSchemas->add( _bstr_t( "" ), _bstr_t( "custom-forms\\form_submit.xsd" ) );

    MSXML2::IXMLDOMDocument2Ptr submitDoc( MSXML2::CLSID_DOMDocument60 );
    submitDoc->schemas = submitSchemas.GetInterfacePtr();;

    submitDoc->async = VARIANT_FALSE;
    submitDoc->validateOnParse = VARIANT_TRUE;
    submitDoc->resolveExternals = VARIANT_TRUE;

    CString tmp;

    // search form version folders
    CStringArray versionFolders;
    for ( int i=0; i<formFolders.GetCount(); i++ )
        {
        MSXML2::IXMLDOMElementPtr formElem = doc->createElement( _T( "form" ) );
        formElem->setAttribute( _bstr_t( "id" ), _variant_t( formFolders[i] ) );
        root->appendChild( formElem );

        bWorking = ff.FindFile( _T( "custom-forms\\library\\" ) + formFolders[i] + _T( "\\*" ) );
        while ( bWorking )
            {
            bWorking = ff.FindNextFileW();
            if ( ff.IsDirectory() && !ff.IsDots() )
                {
                versionFolders.Add( ff.GetFileName() );
                }
            }
        ff.Close();
        for ( int j=0; j<versionFolders.GetCount(); j++ )
            {
            CString versionFolder = _T( "custom-forms\\library\\" ) + formFolders[i] + _T( "\\" ) + versionFolders[j];
            MSXML2::IXMLDOMElementPtr versionElem = doc->createElement( _T( "version" ) );
            versionElem->setAttribute( _bstr_t( "id" ), _variant_t( versionFolders[j] ) );
            formElem->appendChild( versionElem );

            // if form file does exist, add its full path to the catalog and validate it
            CFile formFile;
            if ( formFile.Open( versionFolder + _T( "\\form.xml" ), CFile::modeRead ) )
                {
                CString formFilePath = formFile.GetFilePath();
                formFile.Close();
                VARIANT_BOOL valid = templateDoc->load( _variant_t( formFilePath ) );
                MSXML2::IXMLDOMElementPtr templateElem = doc->createElement( _T( "template" ) );
                versionElem->appendChild( templateElem );
                if ( VARIANT_TRUE != valid )
                    {
                    MSXML2::IXMLDOMElementPtr parseErrorElem = doc->createElement( _T( "parseError" ) );
                    parseErrorElem->setAttribute( _bstr_t( "line" ), _variant_t( templateDoc->parseError->Getline() ) );
                    parseErrorElem->setAttribute( _bstr_t( "char" ), _variant_t( templateDoc->parseError->Getlinepos() ) );
                    parseErrorElem->appendChild( doc->createCDATASection( templateDoc->parseError->Getreason() ) );
                    templateElem->appendChild( parseErrorElem );
                    }
                MSXML2::IXMLDOMElementPtr urlElem = doc->createElement( _T( "url" ) );
                formFilePath.Replace( '\\', '/' );
                urlElem->Puttext( _bstr_t( _T( "file:///" ) + formFilePath ) );
                templateElem->appendChild( urlElem );
                }
            // validate all submitted form files while adding to catalog
            bWorking = ff.FindFile( versionFolder + _T( "\\*_*.xml" ) );
            while (bWorking) {
                bWorking = ff.FindNextFileW();
                MSXML2::IXMLDOMElementPtr submitElem = doc->createElement( _T( "submit" ) );
                versionElem->appendChild( submitElem );
                VARIANT_BOOL valid = submitDoc->load( _variant_t( ff.GetFilePath() ) );
                if ( VARIANT_TRUE != valid )
                    {
                    MSXML2::IXMLDOMElementPtr parseErrorElem = doc->createElement( _T( "parseError" ) );
                    parseErrorElem->setAttribute( _bstr_t( "line" ), _variant_t( submitDoc->parseError->Getline() ) );
                    parseErrorElem->setAttribute( _bstr_t( "char" ), _variant_t( submitDoc->parseError->Getlinepos() ) );
                    parseErrorElem->appendChild( doc->createCDATASection( submitDoc->parseError->Getreason() ) );
                    submitElem->appendChild( parseErrorElem );
                    }
                MSXML2::IXMLDOMElementPtr urlElem = doc->createElement( _T( "url" ) );
                CString submitPath = ff.GetFilePath();
                submitPath.Replace( '\\', '/' );
                urlElem->Puttext( _bstr_t( _T( "file:///" ) + submitPath ) );
                submitElem->appendChild( urlElem );
                }
            ff.Close();
            }
        versionFolders.RemoveAll();
        }
    HRESULT hr = doc->save( _variant_t( filePath ) );
    return SUCCEEDED( hr );
    }

//----------------------------------------------------------------------
//! \brief Transform specified XML document and display in web view
//----------------------------------------------------------------------
void CCustomFormsDlg::transform( LPCTSTR src, XMLTransformer * transformer, CMapStringToString & params )
    {
    // link the m_browser member variable
    UpdateData();
    CFile tmpFile;
    if ( !tmpFile.Open( _T( "custom-forms\\tmp.html" ), CFile::modeCreate | CFile::modeWrite ) ) {
        m_browser.WriteContent( CString( "unable to open temporary file" ) );
        return;
        }

    // remember the full path to the temp file for navigation
    CString url( "file://" );
    url += tmpFile.GetFilePath();
    tmpFile.Close();

    // open a FileStream to the temp file to hold the transformation output
    FileStream *fileStream;
    FileStream::OpenFile( _T( "custom-forms\\tmp.html" ), ( IStream** ) &fileStream, true );
    transformer->transform( src, fileStream, params );
    delete fileStream;

    _variant_t    flags( 0L, VT_I4 );
    _variant_t    target_frame_name( "" );
    _variant_t    post_data( "" );
    _variant_t    headers( "" );

    m_browser.Navigate(url, &flags, &target_frame_name, &post_data, &headers );
    }

// CCustomFormsDlg message handlers
BEGIN_EVENTSINK_MAP(CCustomFormsDlg, CDialog)
    ON_EVENT(CCustomFormsDlg, IDC_EMBEDDED_BROWSER, 250, CCustomFormsDlg::OnBeforeNavigate2, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
END_EVENTSINK_MAP()

//----------------------------------------------------------------------
//! \brief Handle application-specific navigation URLs
//----------------------------------------------------------------------
void CCustomFormsDlg::OnBeforeNavigate2(LPDISPATCH, VARIANT* URL, VARIANT*, VARIANT*, VARIANT*, VARIANT*, BOOL* Cancel)
    {
    if ( URL->vt != VT_BSTR )
        {
        return;
        }

    CString urlStr = URL->bstrVal;
    // manually transform form templates and submitted forms since the XSL templates do not know where the
    // supporting resources are located
    if ( urlStr.Find( _T( "template:" ) ) == 0)
        {
        // take over
        *Cancel = TRUE;

        // override relative path param defaults in the template (which were set for debugging purposes)
        CMapStringToString params;
        // resources are relative to the temp file
        params.SetAt( _T( "resources" ), _T( "" ) );
        CString src = CWebBrowser2::URLDecode( urlStr.Right( urlStr.GetLength() - (int) strlen( "template:" ) ) );
        transform( src, m_templateTransform, params );
        }
    else if ( urlStr.Find( _T( "submitted:") ) == 0 )
        {
        // take over
        *Cancel = TRUE;

        CString submittedFile = CWebBrowser2::URLDecode( urlStr.Right( urlStr.GetLength() - (int) strlen( "submitted:" ) ) );

        // override relative path param defaults in the template (which were set for debugging purposes)
        CMapStringToString params;
        // resources are relative to the temp file
        params.SetAt( _T( "resources" ), _T( "" ) );
        // the form template file is in the same folder as the submitted form file
        CString formPath = submittedFile.Left( submittedFile.ReverseFind( '/' ) + 1 );
        params.SetAt( _T( "formPath" ), formPath );
        transform( submittedFile, m_submittedTransform, params );
        }
    else if ( urlStr.Find( _T( "app:") ) == 0 )
        {
        // take over
        *Cancel = TRUE;

        CString command = urlStr.Right( urlStr.GetLength() - (int) strlen( "app:" ) );
        CString procedure = command.Left( command.Find( '(' ) );
        CString args = command.Mid( procedure.GetLength() + 1, command.ReverseFind( ')') - ( procedure.GetLength() + 1 ) );

        TRACE2( "app command '%s' executed with args '%s'\r\n", procedure, args );

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

        if ( procedure == _T( "importForm" ) )
            {
            importForm();
            }
        else if ( procedure == _T( "deleteCustom" ) )
            {
            deleteFromDevice( _ttoi( arguments[0] ) );
            }
        else if ( procedure == _T( "changePosition" ) )
            {
            moveOnDevice( _ttoi( arguments[0] ), (uint8) _ttoi( arguments[1] ) );
            }
        else if ( procedure == _T( "requestPosition" ) )
            {
            requestPosition( _ttoi( arguments[0] ) );
            }
        else if ( procedure == _T( "sendForm" ) )
            {
            CString formFile = arguments[0];
            formFile.Replace( '/', '\\');
            sendToDevice( formFile.Right( formFile.GetLength() - (int) strlen( "file:///" ) ) );
            }
        else if ( procedure == _T( "removeForm" ) )
            {
            CString formFile = arguments[0];
            formFile.Replace( '/', '\\');
            if ( DeleteFile( formFile.Right( formFile.GetLength() - (int) strlen( "file:///" ) ) ) )
                {
                refreshCatalog();
                }
            else {
                CString message;
                message.Format( _T( "Unable to remove form. Error code: %d" ), GetLastError() );
                MessageBox( message, _T( "Remove form" ), MB_OK | MB_ICONERROR );
                }
            }
        }
    }

BOOL CCustomFormsDlg::importForm()
    {
    TCHAR workingDirectory[200];
    // opening a file in another directory changes the current
    // directory, which will cause problems because the log and data
    // files are opened relative to the current directory.  So, get the
    // directory now so that it can be restored when the user is done
    // picking a file.
    DWORD returnValue = GetCurrentDirectory( 200, workingDirectory );
    if( returnValue == 0 || returnValue > 200 )
    {
        MessageBox( _T( "Unable to get current directory" ), _T( "Import Form Template" ), MB_OK | MB_ICONERROR );
        return FALSE;
    }

    CFileDialog dlg
    (
    TRUE,
    _T( "xml" ),
    NULL,
    OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
    _T( "XML Files (*.xml)|*.xml||" )
    );

    if( dlg.DoModal() == IDOK )
        {
        // restore working directory
        SetCurrentDirectory( workingDirectory );

        // specify the schema for the form template validator since it isn't in same folder as loaded files (and cache it)
        MSXML2::IXMLDOMSchemaCollection2Ptr templateSchemas( MSXML2::CLSID_XMLSchemaCache60 );
        templateSchemas->add( _bstr_t( "" ), _bstr_t( "custom-forms\\form_template.xsd" ) );

        MSXML2::IXMLDOMDocument2Ptr templateDoc( MSXML2::CLSID_DOMDocument60 );
        templateDoc->schemas = templateSchemas.GetInterfacePtr();;

        templateDoc->async = VARIANT_FALSE;
        // validate on load
        templateDoc->validateOnParse = VARIANT_TRUE;
        templateDoc->resolveExternals = VARIANT_TRUE;

        VARIANT_BOOL valid = templateDoc->load( _variant_t( dlg.GetPathName() ) );
        if ( VARIANT_TRUE != valid )
            {
            CString text = templateDoc->parseError->Getreason();
            CString message;
            message.Format( _T( "Form validation failed.\nLine %d, char %d: %s \nDo you want to import anyway?" ),
                templateDoc->parseError->Getline(), templateDoc->parseError->Getlinepos(), text );
            if (IDNO == MessageBox(
                message,
                _T( "Import Form Template" ),
                MB_YESNO | MB_ICONWARNING ) )
                {
                return FALSE;
                }
            // reload without validation
            templateDoc->validateOnParse = VARIANT_FALSE;
            templateDoc->load( _variant_t( dlg.GetPathName() ) );
            }

        MSXML2::IXMLDOMNodePtr formId = templateDoc->selectSingleNode( _bstr_t( "//form/@id" ) );
        MSXML2::IXMLDOMNodePtr formVersion = templateDoc->selectSingleNode( _bstr_t( "//form/@version" ) );
        MSXML2::IXMLDOMNodePtr formTitle = templateDoc->selectSingleNode( _bstr_t( "//form/title" ) );
        CStringArray messages;
        if ( formId == NULL )
            {
            messages.Add( _T( "form element does not contain required attribute 'id'" ) );
            }
        if ( formVersion == NULL)
            {
            messages.Add( _T( "form element does not contain required attribute 'version'" ) );
            }
        if ( formTitle == NULL)
            {
            messages.Add( _T( "form element does not contain required child element 'title'" ) );
            }
        if ( messages.GetCount() > 0 )
            {
            CString message( "Severe error importing form template:" );
            for ( int i=0; i<messages.GetCount(); i++ )
                {
                message += _T( "\n" );
                message += messages[i];
                }
            MessageBox( message, _T( "Import Form Template" ), MB_OK | MB_ICONERROR );
            return FALSE;
            }

        // create folder structure
        CStringArray folders;
        folders.Add( _T( "custom-forms" ) );
        folders.Add( _T( "library" ) );
        folders.Add( formId->Gettext() );
        folders.Add( formVersion->Gettext() );
        CString path;
        for ( int i=0; i<folders.GetCount(); i++ )
            {
            path += folders[i] + _T( "\\" );
            if ( ERROR_PATH_NOT_FOUND == CreateDirectory( path, NULL) )
                {
                MessageBox( _T( "Failed to create repository folder " ) + path, _T( "Import Form Template" ), MB_OK | MB_ICONERROR );
                return FALSE;
                }
            }

        CString targetPath = path + _T( "\\form.xml" );

        CFileStatus existingFile;
        if ( CFile::GetStatus(targetPath, existingFile) )
            {
            if ( IDOK != MessageBox(
                _T( "A template file already exists for this form id and version\nand will be overwritten if you continue." ),
                _T( "Import Form Template" ),
                MB_OKCANCEL | MB_ICONWARNING ) )
                {
                return FALSE;
                }
            }

        // copy file to correct folder
        if ( CopyFile( dlg.GetPathName(), targetPath, FALSE ) )
            {
            refreshCatalog();
            return TRUE;
            }
        return FALSE;
        }

    //restore working directory
    SetCurrentDirectory( workingDirectory );
    return FALSE;
    }

BOOL CCustomFormsDlg::sendToDevice( LPCTSTR filename )
    {
    CFileStatus fileStatus;
    if ( !CFile::GetStatus( filename, fileStatus ) )
        {
        CString message;
        message.Format( _T( "Form file %s does not exist." ), filename );
        MessageBox( message, _T( "Send Form Template to Device" ), MB_OK | MB_ICONERROR );
        return FALSE;
        }

    // validate on load

    // specify the schema for the form template validator since it isn't in same folder as loaded files (and cache it)
    MSXML2::IXMLDOMSchemaCollection2Ptr templateSchemas( MSXML2::CLSID_XMLSchemaCache60 );
    templateSchemas->add( _bstr_t( "" ), _bstr_t( "custom-forms\\form_template.xsd" ) );

    MSXML2::IXMLDOMDocument2Ptr templateDoc( MSXML2::CLSID_DOMDocument60 );
    templateDoc->schemas = templateSchemas.GetInterfacePtr();;

    templateDoc->async = VARIANT_FALSE;
    templateDoc->validateOnParse = VARIANT_TRUE;
    templateDoc->resolveExternals = VARIANT_TRUE;

    VARIANT_BOOL valid = templateDoc->load( _variant_t( filename ) );
    if ( VARIANT_TRUE != valid )
        {
        if (IDNO == MessageBox(
            _T( "Form validation failed.\nDo you want to send the file anyway?" ),
            _T( "Send Form Template to Device" ),
            MB_YESNO | MB_ICONWARNING ) )
            {
            return FALSE;
            }
        // reload without validation
        templateDoc->validateOnParse = VARIANT_FALSE;
        templateDoc->load( _variant_t( filename ) );
        }

    MSXML2::IXMLDOMNodePtr formVersion = templateDoc->selectSingleNode( _bstr_t( "//form/@version" ) );
    if ( formVersion == NULL)
        {
        MessageBox(
            _T( "Form template does not specify a version attribute, which is necessary for file transport." ),
            _T( "Send Form Template to Device" ),
            MB_OK | MB_ICONERROR );
        return FALSE;
        }

    char file[MAX_PATH+1];
    char    versionString[35];
    uint8   version[16];
    uint8   versionLength;

    memset( version, 0, sizeof( version ) );
    WideCharToMultiByte( mCom.mClientCodepage, 0, formVersion->Gettext(), -1, versionString, 34, NULL, NULL );
    versionString[34] = '\0';
    versionLength = (uint8)minval( 16, strlen( versionString ) );
    memmove( version, versionString, versionLength );

    WideCharToMultiByte( CP_ACP, 0, filename, -1, file, MAX_PATH+1, NULL, NULL );
    file[MAX_PATH] = '\0';

    mCom.sendFile( file, versionLength, version, (uint8) FMI_FILE_TYPE_CUSTOM_FORMS, TRUE );

    CFileTransferProgressDlg sending_dlg( this, mCom );
    sending_dlg.DoModal();

    return TRUE;
    }

BOOL CCustomFormsDlg::deleteFromDevice(int id)
    {
    uint32 message = id;

    mCom.sendFmiPacket( FMI_CUSTOM_FORM_DEL_REQUEST, (uint8*)&message, sizeof( message ) );

    return TRUE;
    }

BOOL CCustomFormsDlg::moveOnDevice(int id, uint8 newPosition)
    {
    custom_form_move_type message;
    memset( &message, 0, sizeof( message ) );
    message.form_id = id;
    message.new_position = newPosition;

    mCom.sendFmiPacket( FMI_CUSTOM_FORM_MOVE_REQUEST, (uint8*)&message, sizeof( message ) );

    return TRUE;
    }

BOOL CCustomFormsDlg::requestPosition(int id)
    {
    uint32 message = id;

    mCom.sendFmiPacket( FMI_CUSTOM_FORM_GET_POS_REQUEST, (uint8*)&message, sizeof( message ) );

    return TRUE;
    }

#endif