/*********************************************************************
*
*   HEADER NAME:
*       CWebBrowser2.cpp
*
* Copyright 2013 by Garmin Ltd. or its subsidiaries.
*---------------------------------------------------------------------
* $NoKeywords$
*********************************************************************/
#include "stdafx.h"
#include "CWebBrowser2.h"
#include <Shlwapi.h>

IMPLEMENT_DYNCREATE(CWebBrowser2, CWnd)

//----------------------------------------------------------------------
//! \brief Shortcut method for accessing the ITHMLDocument2 instance
//! \return The IHTMLDocument2, if available, or NULL
//----------------------------------------------------------------------
IHTMLDocument2 *CWebBrowser2::GetDocument()
{
    IHTMLDocument2 *document = NULL;

    // get browser document's dispatch interface

    IDispatch *document_dispatch = get_Document();

    if (document_dispatch != NULL) {

        // get the actual document interface

        document_dispatch->QueryInterface(IID_IHTMLDocument2,
                                               (void **)&document);

        // release dispatch interface

        document_dispatch->Release();
    }

    return document;
}

//----------------------------------------------------------------------
//! \brief Dynamically set the HTML content on the control
//! \return This is a convenience method for setting the browser content
//!     from memory instead of navigating to a URL
//----------------------------------------------------------------------
void CWebBrowser2::WriteContent(CString content)
{
    IHTMLDocument2 *document = GetDocument();

    if (document != NULL) {

        // construct text to be written to browser as SAFEARRAY

        SAFEARRAY *safe_array = SafeArrayCreateVector(VT_VARIANT,0,1);

        VARIANT    *variant;

        SafeArrayAccessData(safe_array,(LPVOID *)&variant);

        variant->vt      = VT_BSTR;
        variant->bstrVal = content.AllocSysString();

        SafeArrayUnaccessData(safe_array);

        // write SAFEARRAY to browser document

        document->write(safe_array);

        // cleanup

        document->Release();
        document = NULL;

        ::SysFreeString(variant->bstrVal);
        variant->bstrVal = NULL;

        SafeArrayDestroy(safe_array);

    }
}


CComVariant CWebBrowser2::InvokeScript(LPCTSTR funcName, const CStringArray * paramArray)
    {
    // look up the Script object
    CComPtr<IDispatch> spScript;
    HRESULT hr = GetDocument()->get_Script( &spScript );
    if ( FAILED( hr ) )
        {
        return FALSE;
        }

    // obtain a pointer to the desired function
    CComBSTR bstrMember( funcName );
    DISPID dispid = NULL;
    hr = spScript->GetIDsOfNames( IID_NULL, &bstrMember, 1, LOCALE_SYSTEM_DEFAULT, &dispid );
    if ( FAILED( hr ) )
        {
        return FALSE;
        }

    // convert parameters
    DISPPARAMS dispparams;
    memset( &dispparams, 0, sizeof dispparams );
    dispparams.cArgs = 0;
    dispparams.cNamedArgs = 0;
    if ( paramArray )
        {
        const int arraySize = (int) paramArray->GetCount();
        dispparams.cArgs = arraySize;
        dispparams.rgvarg = new VARIANT[ dispparams.cArgs ];
        for (int i=0; i< arraySize; i++ )
            {
            CComBSTR bstr = paramArray->GetAt( arraySize - 1 - i );
            bstr.CopyTo( &dispparams.rgvarg[i].bstrVal );
            dispparams.rgvarg[i].vt = VT_BSTR;
            }
        }

    EXCEPINFO excepInfo;
    memset( &excepInfo, 0, sizeof excepInfo );
    CComVariant vaResult;
    UINT nArgErr = (UINT)-1;

    // call the function with the supplied paramaters
    hr = spScript->Invoke( dispid, IID_NULL, 0, DISPATCH_METHOD, &dispparams, &vaResult, &excepInfo, &nArgErr );
    if ( 0 < dispparams.cArgs )
        {
        delete [] dispparams.rgvarg;
        }
    if ( FAILED( hr ) )
        {
        return FALSE;
        }

    return vaResult;
    }

CString CWebBrowser2::URLDecode(CString sIn)
    {
    DWORD bufLen = sIn.GetLength()+1;
    TCHAR * buf = new TCHAR[bufLen];
    UrlCanonicalize( sIn, buf, &bufLen, URL_UNESCAPE );
    CString out( buf );
    delete buf;
    return out;
    }

CString CWebBrowser2::URLEncode(CString sIn)
    {
    DWORD bufLen = sIn.GetLength()+1;
    TCHAR * buf = new TCHAR[bufLen];
    UrlCanonicalize( sIn, buf, &bufLen, 0 );
    CString out( buf );
    delete buf;
    return out;
    }

