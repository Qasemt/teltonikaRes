/*********************************************************************
*
*   HEADER NAME:
*       XMLTransformer.h
*
* Copyright 2013 by Garmin Ltd. or its subsidiaries.
*---------------------------------------------------------------------
* $NoKeywords$
*********************************************************************/
#pragma once
#include "stdafx.h"
#import <msxml6.dll> named_guids

using namespace MSXML2;

//----------------------------------------------------------------------
//! \brief Utility class for simplifying XSL transformations
//! \details This class encapsulates loading a stylesheet for trans-
//!    forming an XML source document, along with ability to cache
//!    the loaded transformation processor for re-use
//----------------------------------------------------------------------
class XMLTransformer
{
public:
    XMLTransformer(LPCTSTR stylesheet);
    virtual ~XMLTransformer(void);

    BOOL transform(LPCTSTR src, IStream * dst, CMapStringToString & params);

protected:
    //! The path to the XSL stylesheet use by this transformer
    CString m_stylesheet;

};
