/*********************************************************************
*
*   MODULE NAME:
*       XMLTransformer.cpp
*
*   Copyright 2013 by Garmin Ltd. or its subsidiaries.
*---------------------------------------------------------------------
* $NoKeywords$
*********************************************************************/
#include "XMLTransformer.h"

//----------------------------------------------------------------------
//! \brief Constructor
//----------------------------------------------------------------------
XMLTransformer::XMLTransformer(LPCTSTR txf)
{
    m_stylesheet = txf;
}

//----------------------------------------------------------------------
//! \brief Destructor
//----------------------------------------------------------------------
XMLTransformer::~XMLTransformer(void)
{
}

//----------------------------------------------------------------------
//! \brief transform an XML document
//! \details Load this instance's stylesheet and the specified XML
//!    document source and then transform it with the specified params
//! \param src The XML document to be transformed
//! \param dst The destination for the transformation output
//! \param params An associative array of parameters to be supplied to
//!    the stylesheet during transformation
//----------------------------------------------------------------------
BOOL XMLTransformer::transform(LPCTSTR src, IStream * dst, CMapStringToString & params)
    {
    BOOL bResult = FALSE;
    HRESULT hr;

    // load stylesheet, create a template from it, and a processor from that
    MSXML2::IXMLDOMDocument3Ptr pStyleSheet( MSXML2::CLSID_FreeThreadedDOMDocument60 );
    MSXML2::IXSLTemplatePtr pTemplate( MSXML2::CLSID_XSLTemplate60 );
    MSXML2::IXSLProcessorPtr pProcessor;
    try {
        pStyleSheet->async = VARIANT_FALSE;
        pStyleSheet->resolveExternals = VARIANT_TRUE;
        pStyleSheet->setProperty( "AllowDocumentFunction", VARIANT_TRUE );

        bResult = pStyleSheet->load( _variant_t( m_stylesheet ) );

        pTemplate->stylesheet = pStyleSheet;

        pProcessor = pTemplate->createProcessor();
        }
    catch ( _com_error &e ) {
        TRACE1("Error loading XSL stylesheet : %s\n", ( const char * ) _bstr_t( e.Description() ) );
        return FALSE;
        }

    // now load the source XML document
    MSXML2::IXMLDOMDocumentPtr pSrcDoc( MSXML2::CLSID_DOMDocument60 );

    try {
        bResult = pSrcDoc->load( _variant_t( src ) );
        if ( !bResult ) {
            TRACE1( "Error parsing source document for transform: %s\n", src );
            if ( pSrcDoc->parseError )
                {
                CString text = pSrcDoc->parseError->Getreason();
                TRACE1( "Parse error: %s\n", text );
                }
            TRACE1( "error: %d\n", GetLastError() );
            }

        hr = pProcessor->put_output( CComVariant( dst ) );
        if ( !SUCCEEDED( hr ) ) {
            TRACE1( "error: %d\n", GetLastError() );
            }

        hr = pProcessor->put_input( _variant_t( ( IUnknown * ) pSrcDoc ) );
        if ( !SUCCEEDED( hr ) ) {
            TRACE1( "error: %d\n", GetLastError() );
            }

        // assign the parameters for the transformation
        CString key, val;
        for ( POSITION pos = params.GetStartPosition(); pos != NULL; ) {
            params.GetNextAssoc( pos, key, val );
            pProcessor->addParameter( _bstr_t( key ), _variant_t( val ), "" );
            TRACE2( "adding params: %s = %s\r\n", key, val );
            }

        bResult = pProcessor->transform();
    }
    catch ( _com_error &e ) {
        bResult = FALSE;
        TRACE1( "Error processing XML : %s\n", ( const char * ) _bstr_t( e.Description() ) );
    }

    return bResult;
}
