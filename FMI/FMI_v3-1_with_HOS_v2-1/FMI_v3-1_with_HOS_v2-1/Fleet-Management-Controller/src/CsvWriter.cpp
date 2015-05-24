/*********************************************************************
*
*   MODULE NAME:
*       CsvWriter.cpp
*
*   Copyright 2012 by Garmin Ltd. or its subsidiaries.
*---------------------------------------------------------------------
* $NoKeywords$
*********************************************************************/

#include "stdafx.h"
#include "CsvWriter.h"

CsvWriter::CsvWriter(CFile * file) :
    m_file( file )
    {

    }

CsvWriter::~CsvWriter()
    {

    }

void CsvWriter::WriteValue(char * value)
    {
    WriteValue( value, (int) strlen( value ) );
    }

void CsvWriter::WriteValue(char * value, int len)
    {
    bool escape = ( ( 0 != strchr( value, ',' ) ) || ( 0 != strchr( value, '\n' ) ) );
    if( escape )
        {
        m_file->Write( "\"", 1 );
        }
    int w = 0;
    char * context = NULL;
    char * pch = strtok_s( value, "\"" , &context);
    while( pch != NULL )
        {
        m_file->Write( pch, max( len-w, len ) );
        len += (int) strlen( pch );
        pch = strtok_s( NULL, "\"", &context );
        if( pch != NULL )
            {
            m_file->Write( "\"", 1 );
            }
        }
    if( escape)
        {
        m_file->Write( "\"", 1 );
        }
    m_file->Write( ",", 1 );
    }

void CsvWriter::NewLine()
    {
    m_file->Write( "\n", 1 );
    m_file->Flush();
    }

void CsvWriter::Finish()
    {
    m_file->Flush();
    }
