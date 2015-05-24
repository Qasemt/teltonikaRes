/*********************************************************************
*
*   MODULE NAME:
*       CsvWriter.h
*
*   Copyright 2012 by Garmin Ltd. or its subsidiaries.
*---------------------------------------------------------------------
* $NoKeywords$
*********************************************************************/

#ifndef _CSVWRITER_H_
#define _CSVWRITER_H_

#include <afx.h>

class CsvWriter
{
public:
    CsvWriter( CFile * file );
    virtual ~CsvWriter();

    void WriteValue( char * value );
    void WriteValue( char * value, int len );
    void NewLine();

    void Finish();

protected:
    CFile * m_file;

};

#endif
