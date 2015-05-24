/*********************************************************************
*
*   MODULE NAME:
*       AobrdEventLogConverter.cpp
*
*   Copyright 2012 by Garmin Ltd. or its subsidiaries.
*---------------------------------------------------------------------
* $NoKeywords$
*********************************************************************/

#include "AobrdEventLogConverter.h"
#include "CsvWriter.h"
#include "util.h"
#include <stdlib.h>
#include <fstream>
#include <map>

#include "pack_begin.h"

#if( FMI_SUPPORT_A610 )

using namespace std;

__packed struct evt_log_header_fixed {
    uint16 version;
    uint32 field_flags;
    uint32 reserved;
    uint8 reason;
};

//----------------------------------------------------------------------
//  Static function prototypes
//----------------------------------------------------------------------
static void encodeStringFields( uint32 field_flags, int start, int end, ifstream & rawFile, CsvWriter & csvWriter );
static void convertTimeTypeToString( time_type aTimestamp, char * full, int fullSize );
static void encodeTimestampFields( uint32 field_flags, int start, int end, ifstream & rawFile, CsvWriter & csvWriter );
static time_type encodeTimestampField( uint32 field_flags, int start, ifstream & rawFile, CsvWriter & csvWriter );
static void encodeDegreeFields( uint32 field_flags, int start, int end, ifstream & rawFile, CsvWriter & csvWriter, char * format );
static void encodeUint32Fields( uint32 field_flags, int start, int end, ifstream & rawFile, CsvWriter & csvWriter, char * format );
static void encodeUint16Fields( uint32 field_flags, int start, int end, ifstream & rawFile, CsvWriter & csvWriter, char * format );
static uint16 readCSVRecord( ifstream & fstream, vector<string> & record, char delimiter );
static void modifyStatusChangeRecord( bool (*locator)(void * key, vector<string> & record), void * key, void (*updater)(void * val, vector<string> & record), void * val, const char * source );
static void decodeStringFields( uint32 & field_flags, int start, int end, ofstream & rawFile, vector<string> & record, uint16 & fieldIdx );
static void decodeDegreeFields( uint32 & field_flags, int start, int end, ofstream & rawFile, vector<string> & record, uint16 & fieldIdx, char * format );
static void decodeUint32Fields( uint32 & field_flags, int start, int end, ofstream & rawFile, vector<string> & record, uint16 & fieldIdx, char * format );
static void decodeUint16Fields( uint32 & field_flags, int start, int end, ofstream & rawFile, vector<string> & record, uint16 & fieldIdx, char * format );
static void decodeTimestampFields( uint32 & field_flags, int start, int end, ofstream & rawFile, vector<string> & record, uint16 & fieldIdx );
static void checkFieldHeaders( uint32 & field_flags, int start, int end, vector<string> & record, uint16 & fieldIdx );
static time_type parseDateTime( std::string cellValue );


//----------------------------------------------------------------------
//! \brief Constructor
//----------------------------------------------------------------------
AobrdEventLogConverter::AobrdEventLogConverter()
    {

    }

//----------------------------------------------------------------------
//! \brief Convert 1 byte length and character stream into CSV cell
//----------------------------------------------------------------------
static void encodeStringFields( uint32 field_flags, int start, int end, ifstream & rawFile, CsvWriter & csvWriter )
    {
    char buf[257];
    uint8 len;
    for( int i=start; i<=end; i++ )
        {
        if( field_flags & ( 0x01 << i) )
            {
            rawFile.read( (char*) &len, 1 );
            if ( 0 < len )
                {
                rawFile.read( buf, len );
                buf[len] = 0;
                csvWriter.WriteValue( buf );
                }
            else
                {
                csvWriter.WriteValue( "" );
                }
            }
        else
            {
            csvWriter.WriteValue( "" );
            }
        }
    }

static void convertTimeTypeToString( time_type aTimestamp, char * full, int fullSize )
    {
    date_time_data_type date_time;
    UTIL_convert_UTC_to_local( &aTimestamp, &aTimestamp );
    UTIL_convert_seconds_to_date_type( &aTimestamp, &date_time );
    UTIL_convert_seconds_to_time_type( &aTimestamp, &date_time );

    sprintf_s(full, fullSize, "%04d-%02d-%02d %02d:%02d:%02d", date_time.date.year, date_time.date.month, date_time.date.day, date_time.time.hour, date_time.time.minute, date_time.time.second ) ;//, aTimestamp % 1000 );
    }
//----------------------------------------------------------------------
//! \brief Convert time_type into human-readable formatted timestamp
//----------------------------------------------------------------------
static void encodeTimestampFields( uint32 field_flags, int start, int end, ifstream & rawFile, CsvWriter & csvWriter )
    {
    time_type aTimestamp;
    char    full[30];
    for( int i=start; i<=end; i++ )
        {
        if( field_flags & ( 0x01 << i) )
            {
            rawFile.read( ( char*) &aTimestamp, sizeof( aTimestamp ) );
            if( aTimestamp == 0 )
                {
                csvWriter.WriteValue( "" );
                }
            else
                {
                convertTimeTypeToString( aTimestamp, full, ( int ) sizeof( full ) );
                csvWriter.WriteValue( full );
                }
            }
        else
            {
            csvWriter.WriteValue( "" );
            }
        }
    }

static time_type encodeTimestampField( uint32 field_flags, int start, ifstream & rawFile, CsvWriter & csvWriter )
    {
    time_type aTimestamp = 0;
    char    full[30];
    if( field_flags & ( 0x01 << start) )
        {
        rawFile.read( ( char*) &aTimestamp, sizeof( aTimestamp ) );
        if( aTimestamp == 0 )
            {
            csvWriter.WriteValue( "" );
            }
        else
            {
            convertTimeTypeToString( aTimestamp, full, sizeof( full ) );
            csvWriter.WriteValue( full );
            }
        }
    else
        {
        csvWriter.WriteValue( "" );
        }

    return aTimestamp;
    }

//----------------------------------------------------------------------
//! \brief Convert 4 byte signed int into degrees
//----------------------------------------------------------------------
static void encodeDegreeFields( uint32 field_flags, int start, int end, ifstream & rawFile, CsvWriter & csvWriter, char * format = "%08.6f" )
    {
    char tmp[20];
    sint32 rawValue;
    double degrees;
    for( int i=start; i<=end; i++ )
        {
        if( field_flags & ( 0x01 << i) )
            {
            rawFile.read( ( char*) &rawValue, sizeof( rawValue ) );
            degrees = UTIL_convert_semicircles_to_degrees( rawValue );
            sprintf( tmp, format, degrees );
            csvWriter.WriteValue( tmp );
            }
        else
            {
            csvWriter.WriteValue( "" );
            }
        }
    }

//----------------------------------------------------------------------
//! \brief Convert 4 byte unsigned int into 10 byte hex string
//----------------------------------------------------------------------
static void encodeUint32Fields( uint32 field_flags, int start, int end, ifstream & rawFile, CsvWriter & csvWriter, char * format = "0x%.8x" )
    {
    char tmp[20];
    uint32 value;
    for( int i=start; i<=end; i++ )
        {
        if( field_flags & ( 0x01 << i) )
            {
            rawFile.read( ( char*) &value, sizeof( value ) );
            sprintf( tmp, format, value );
            csvWriter.WriteValue( tmp );
            }
        else
            {
            csvWriter.WriteValue( "" );
            }
        }
    }

//----------------------------------------------------------------------
//! \brief Convert 2 byte unsigned int into 6 byte hex string
//----------------------------------------------------------------------
static void encodeUint16Fields( uint32 field_flags, int start, int end, ifstream & rawFile, CsvWriter & csvWriter, char * format = "0x%.4x" )
    {
    char tmp[20];
    uint16 value;
    for( int i=start; i<=end; i++ )
        {
        if( field_flags & ( 0x01 << i) )
            {
            rawFile.read( ( char*) &value, sizeof( value ) );
            sprintf( tmp, format, value );
            csvWriter.WriteValue( tmp );
            }
        else
            {
            csvWriter.WriteValue( "" );
            }
        }
    }

//----------------------------------------------------------------------
//! \brief Convert CSV record into vector of strings
//! \return the number of 'cells' in the record
//----------------------------------------------------------------------
static uint16 readCSVRecord( ifstream & fstream, vector<string> & record, char delimiter = ',' )
    {
    int inquotes=false;
    char c;
    string curstring;
    record.clear();

    while( fstream.good() && !fstream.eof() )
    {
        fstream.read( &c, 1 );

        if (!inquotes && curstring.length()==0 && c=='"')
        {
            //beginquotechar
            inquotes=true;
        }
        else if (inquotes && c=='"')
        {
            //quotechar
            if ( fstream.peek()=='"' )
            {
                //encountered 2 double quotes in a row (resolves to 1 double quote)
                curstring.push_back(c);
                fstream.read( &c, 1 );  //skip the second double-quote
            }
            else
            {
                //endquotechar
                inquotes=false;
            }
        }
        else if (!inquotes && c==delimiter)
        {
            //end of field
            record.push_back( curstring );
            curstring="";
        }
        else if (!inquotes && (c=='\r' || c=='\n') )
        {
        if( c == '\r' && fstream.peek() == '\n' ) {
                fstream.read( &c, 1 );
            }
            record.push_back( curstring );
            return (uint16) record.size();
        }
        else
        {
            curstring.push_back(c);
        }
        fstream.peek();
    }
    record.push_back( curstring );
    return (uint16) record.size();
    }


//----------------------------------------------------------------------
//! \brief callback for finding a Driver Status Change event in CSV file
//----------------------------------------------------------------------
bool locateEventBeingVerified(void * key, vector<string> & record)
    {
    return ( record[1].compare( "0x01" ) == 0 && record[2].compare( ( char* ) key ) == 0 );
    }

//----------------------------------------------------------------------
//! \brief callback for updating a verified Driver Status Change event
//----------------------------------------------------------------------
void updateEventBeingVerified(void * val, vector<string> & record)
    {
    record[22] = ( char* ) val;
    }

//----------------------------------------------------------------------
//! \brief Function for finding and updating a record
//----------------------------------------------------------------------
static void modifyStatusChangeRecord(bool (*locator)(void * key, vector<string> & record), void * key, void (*updater)(void * val, vector<string> & record), void * val, const char * source)
    {
    ifstream origFile( source, ios_base::in | ios_base::binary );
    if( !origFile.good() )
        {
        return;
        }

    char tmpFileName[_MAX_PATH];

    // Leave 5 characters remaining: 4 for .tmp (below), +1 for NUL terminator
    strncpy( tmpFileName, source, _MAX_PATH - 5 );
    tmpFileName[_MAX_PATH - 5] = '\0';
    strcat( tmpFileName, ".tmp" );

    CFile modFile( CString( tmpFileName ), CFile::modeCreate | CFile::modeWrite );
    CsvWriter csvWriter( &modFile );

    evt_log_header_fixed hdr;
    memset( &hdr, 0, sizeof( hdr ) );

    //read each record
    vector<string> record;
    uint16 cellCount = 0;
    char cell[256];
    while( 1 < ( cellCount = readCSVRecord( origFile, record ) ) )
        {
        if( '#' != record[0][0] )
            {
            if ( locator( key, record ) )
                {
                updater( val, record );
                }
            }

        //re-write record
        for( uint32 i=0; i<record.size()-1; i++ )
            {
            strncpy( cell, record[i].c_str(), cnt_of_array( cell ) - 1 );
            cell[cnt_of_array( cell ) - 1] = '\0';
            csvWriter.WriteValue( cell );
            }

        csvWriter.NewLine();

        modFile.Flush();
        }
    modFile.Close();
    origFile.close();

    if( DeleteFile( CString( source ) ) )
        {
        MoveFile( CString( tmpFileName ), CString( source ) );
        }

    }

//----------------------------------------------------------------------
//! \brief Convert raw file to CSV
//----------------------------------------------------------------------
void AobrdEventLogConverter::convertToCsv( const char * source, const char * destFormat )
    {
    ifstream rawFile( source, ios_base::binary | ios_base::in );
    if( !rawFile.good() )
        {
        return;
        }

    evt_log_header_fixed hdr;
    memset( &hdr, 0, sizeof( hdr ) );

    time_type recordTimestamp = 0;

    map<time_type,time_type> verifiedStatusUpdates;

    char len[1];
    char tmp[256];
    tmp[0] = 0;
    //seek past the fixed-width header and timestamp
    rawFile.seekg( sizeof( hdr ) + sizeof( uint32 ), ios_base::beg );
    //read the first 3 variable-width strings (the third is the driver ID)
    for( int i=0; i<3; i++ )
        {
        rawFile.read( len, 1 );
        rawFile.read( tmp, (int) len[0] );
        tmp[(int)len[0]] = 0;
        }
    rawFile.seekg( 0, ios_base::beg );

    CFile csvFile;
    char dest[MAX_PATH];
    sprintf(dest, destFormat, tmp);
    if( !csvFile.Open( CString( dest ), CFile::modeCreate | CFile::modeWrite | CFile::modeNoTruncate ) )
        {
        rawFile.close();
        return;
        }

    uint32 newPos = (uint32) csvFile.SeekToEnd();

    CsvWriter csvWriter( &csvFile );

    if( 0 == newPos )
        {
        csvWriter.WriteValue( "#version" );
        csvWriter.WriteValue( "reason" );
        csvWriter.WriteValue( "timestamp" );
        csvWriter.WriteValue( "Driver first name" );
        csvWriter.WriteValue( "Driver last name" );
        csvWriter.WriteValue( "Driver ID" );
        csvWriter.WriteValue( "Co-Driver first name" );
        csvWriter.WriteValue( "Co-Driver last name" );
        csvWriter.WriteValue( "Co-Driver ID" );
        csvWriter.WriteValue( "Tractor number" );
        csvWriter.WriteValue( "Trailer number" );
        csvWriter.WriteValue( "Tractor VIN" );
        csvWriter.WriteValue( "Current odom. reading" );
        csvWriter.WriteValue( "Carrier ID" );
        csvWriter.WriteValue( "Carrier name" );
        csvWriter.WriteValue( "Nearest city name" );
        csvWriter.WriteValue( "Nearest state name" );
        csvWriter.NewLine();
        }

    while ( rawFile.good() && !rawFile.eof() )
        {
        //read fixed header
        rawFile.read( ( char* ) &hdr, sizeof( hdr ) );

        csvWriter.WriteValue( "#" );

        switch( hdr.reason )
            {
            case DRIVER_ANNOTATION:
                {
                csvWriter.WriteValue( "Driver annotation" );
                for( int i = 0; i<15; i++)
                    {
                    csvWriter.WriteValue( "" );
                    }
                csvWriter.WriteValue( "Start Time" );
                csvWriter.WriteValue( "End Time" );
                csvWriter.WriteValue( "Annotation Text" );
                break;
                }
            case DRIVER_STATUS_CHANGE:
                {
                csvWriter.WriteValue( "Driver status change" );
                for( int i = 0; i<15; i++)
                    {
                    csvWriter.WriteValue( "" );
                    }
                csvWriter.WriteValue( "Old driver status" );
                csvWriter.WriteValue( "New driver status" );
                csvWriter.WriteValue( "Event number" );
                csvWriter.WriteValue( "Latitude of change" );
                csvWriter.WriteValue( "Longitude of change" );
                csvWriter.WriteValue( "Verified time" );
                csvWriter.WriteValue( "Additional status flag" );
                break;
                }
            case DRIVER_VERIFIED_LOG:
                {
                csvWriter.WriteValue( "Driver verified log" );
                for( int i = 0; i<15; i++)
                    {
                    csvWriter.WriteValue( "" );
                    }
                csvWriter.WriteValue( "Original Timestamp" );
                break;
                }
            case DRIVER_COULD_NOT_VERIFY_LOG:
                {
                csvWriter.WriteValue( "Driver could not verify log" );
                for( int i = 0; i<15; i++)
                    {
                    csvWriter.WriteValue( "" );
                    }
                csvWriter.WriteValue( "Original Timestamp" );
                break;
                }
            case DRIVER_ADDED_NEW_SHIPMENT_ENTRY:
                {
                csvWriter.WriteValue( "Driver added new shipment entry" );
                for( int i = 0; i<15; i++)
                    {
                    csvWriter.WriteValue( "" );
                    }
                csvWriter.WriteValue( "Original Timestamp" );
                csvWriter.WriteValue( "Start Time" );
                csvWriter.WriteValue( "End Time" );
                csvWriter.WriteValue( "Shipper name" );
                csvWriter.WriteValue( "Shipment document number" );
                csvWriter.WriteValue( "Shipment commodity" );
                break;
                }
            case DRIVER_MODIFIED_EXISTING_SHIPMENT_ENTRY:
                {
                csvWriter.WriteValue( "Driver modified existing shipment entry" );
                for( int i = 0; i<15; i++)
                    {
                    csvWriter.WriteValue( "" );
                    }
                csvWriter.WriteValue( "Original Timestamp" );
                csvWriter.WriteValue( "Start Time" );
                csvWriter.WriteValue( "End Time" );
                csvWriter.WriteValue( "Shipper name" );
                csvWriter.WriteValue( "Shipment document number" );
                csvWriter.WriteValue( "Shipment commodity" );
                break;
                }
            case DRIVER_DELETED_SHIPMENT_ENTRY:
                {
                csvWriter.WriteValue( "Driver deleted shipment entry" );
                for( int i = 0; i<15; i++)
                    {
                    csvWriter.WriteValue( "" );
                    }
                csvWriter.WriteValue( "Original Timestamp" );
                csvWriter.WriteValue( "Start Time" );
                csvWriter.WriteValue( "End Time" );
                csvWriter.WriteValue( "Shipper name" );
                csvWriter.WriteValue( "Shipment document number" );
                csvWriter.WriteValue( "Shipment commodity" );
                break;
                }
            case PND_FAILURE_DETECTION:
                {
                csvWriter.WriteValue( "PND failure detection" );
                for( int i = 0; i<15; i++)
                    {
                    csvWriter.WriteValue( "" );
                    }
                csvWriter.WriteValue( "Event error code" );
                csvWriter.WriteValue( "Event error timestamp" );
                break;
                }
            }

        csvWriter.NewLine();

        //convert fixed header fields to hex strings
        sprintf( tmp, "0x%.4x", hdr.version );
        csvWriter.WriteValue( tmp );
        //sprintf( tmp, "0x%.8x", hdr.field_flags );
        //csvWriter.WriteValue( tmp );
        //sprintf( tmp, "0x%.8x", hdr.reserved );
        //csvWriter.WriteValue( tmp );
        sprintf( tmp, "0x%.2x", hdr.reason );
        csvWriter.WriteValue( tmp );

        //fields 0-15 are always present
        recordTimestamp = encodeTimestampField( hdr.field_flags, 0, rawFile, csvWriter );
        encodeStringFields( hdr.field_flags, 1, 9, rawFile, csvWriter );
        encodeUint32Fields( hdr.field_flags, 10, 10, rawFile, csvWriter, "%d" );
        encodeStringFields( hdr.field_flags, 11, 14, rawFile, csvWriter );

        for (int ii=0; ii < 32; ii++) {
            if (hdr.field_flags & (0x01 << ii)) {
                TRACE1("field flag %d present\n", ii);
                }
            }

        switch( hdr.reason )
            {
            case DRIVER_ANNOTATION:
                {
                encodeTimestampFields( hdr.field_flags, 27, 28, rawFile, csvWriter );
                encodeStringFields( hdr.field_flags, 16, 16, rawFile, csvWriter );
                break;
                }
            case DRIVER_STATUS_CHANGE:
                {
                encodeStringFields( hdr.field_flags, 17, 18, rawFile, csvWriter );
                encodeUint32Fields( hdr.field_flags, 19, 19, rawFile, csvWriter );
                encodeDegreeFields( hdr.field_flags, 20, 21, rawFile, csvWriter );//, "%h" );
                encodeTimestampFields( hdr.field_flags, 22, 22, rawFile, csvWriter );
                encodeUint16Fields( hdr.field_flags, 29, 29, rawFile, csvWriter ); //it is actually for 29 and 30, 1 byte each
                break;
                }
            case DRIVER_VERIFIED_LOG:
                {
                time_type originalTimestamp = encodeTimestampField( hdr.field_flags, 23, rawFile, csvWriter );
                verifiedStatusUpdates[originalTimestamp] = recordTimestamp;
                break;
                }
            case DRIVER_COULD_NOT_VERIFY_LOG:
                {
                encodeTimestampFields( hdr.field_flags, 23, 23, rawFile, csvWriter );
                break;
                }
            case DRIVER_ADDED_NEW_SHIPMENT_ENTRY:
                {
                encodeTimestampFields( hdr.field_flags, 23, 23, rawFile, csvWriter );
                encodeTimestampFields( hdr.field_flags, 27, 28, rawFile, csvWriter );
                encodeStringFields( hdr.field_flags, 24, 26, rawFile, csvWriter );
                break;
                }
            case DRIVER_MODIFIED_EXISTING_SHIPMENT_ENTRY:
                {
                encodeTimestampFields( hdr.field_flags, 23, 23, rawFile, csvWriter );
                encodeTimestampFields( hdr.field_flags, 27, 28, rawFile, csvWriter );
                encodeStringFields( hdr.field_flags, 24, 26, rawFile, csvWriter );
                break;
                }
            case DRIVER_DELETED_SHIPMENT_ENTRY:
                {
                encodeTimestampFields( hdr.field_flags, 23, 23, rawFile, csvWriter );
                encodeTimestampFields( hdr.field_flags, 27, 28, rawFile, csvWriter );
                encodeStringFields( hdr.field_flags, 24, 26, rawFile, csvWriter );
                break;
                }
            case PND_FAILURE_DETECTION:
                {
                hdr.field_flags |= 0x01 << 27;
                hdr.field_flags |= 0x01 << 28;
                encodeUint16Fields( hdr.field_flags, 27, 27, rawFile, csvWriter );
                encodeTimestampFields( hdr.field_flags, 28, 28, rawFile, csvWriter );
                break;
                }
            }
        csvWriter.NewLine();
        rawFile.peek();
        }

    rawFile.close();

    csvWriter.Finish();
    csvFile.Close();

    char originalTimestampString[30], verifiedTimestampString[30];
    for( map<time_type,time_type>::iterator it = verifiedStatusUpdates.begin(); it != verifiedStatusUpdates.end(); ++it )
        {
        convertTimeTypeToString( ( *it ).first, originalTimestampString, ( int ) sizeof( originalTimestampString ) );
        convertTimeTypeToString( ( *it ).second, verifiedTimestampString, ( int ) sizeof( verifiedTimestampString ) );
        modifyStatusChangeRecord(&locateEventBeingVerified, ( void * ) originalTimestampString, &updateEventBeingVerified, ( void * ) verifiedTimestampString, dest );
        }
    }

//----------------------------------------------------------------------
//! \brief Convert CSV fields into 1 byte length and character stream
//----------------------------------------------------------------------
static void decodeStringFields( uint32 & field_flags, int start, int end, ofstream & rawFile, vector<string> & record, uint16 & fieldIdx )
    {
    char len[1];

    ASSERT(record[fieldIdx].length() <= UCHAR_MAX);

    for( int i=start; i<=end; i++ )
        {
        if( field_flags & ( 0x01 << i) )
            {
            if( fieldIdx < record.size() )
                {
                len[0] = (char) record[fieldIdx].length();
                rawFile.write( len, 1 );
                rawFile.write( record[fieldIdx].c_str(), (std::streamsize) record[fieldIdx].length() );
                }
            else
                {
                len[0] = 0;
                rawFile.write( len, 1 );
                }
            }
        fieldIdx++;
        }
    }

//----------------------------------------------------------------------
//! \brief Convert degrees into 4 byte signed int
//----------------------------------------------------------------------
static void decodeDegreeFields( uint32 & /*field flags*/, int start, int end, ofstream & rawFile, vector<string> & record, uint16 & fieldIdx, char * format = "%f" )
    {
    float degrees = 0;
    sint32 rawValue = 0;
    for( int i=start; i<=end; i++ )
        {
        if( ( fieldIdx < record.size() ) && ( 0 < record[fieldIdx].length() ) )
            {
            sscanf_s( record[fieldIdx].c_str(), format, &degrees );
            rawValue = UTIL_convert_degrees_to_semicircles( degrees );
            rawFile.write( ( char* ) &rawValue, sizeof( rawValue ) );
            //field_flags |= ( 0x01 << i );
            }
        fieldIdx++;
        }
    }

//----------------------------------------------------------------------
//! \brief Convert 10 byte hex string into 4 byte unsigned int
//----------------------------------------------------------------------
static void decodeUint32Fields( uint32 & /*field flags*/, int start, int end, ofstream & rawFile, vector<string> & record, uint16 & fieldIdx, char * format = "%x" )
    {
    uint32 value = 0;
    for( int i=start; i<=end; i++ )
        {
        if( ( fieldIdx < record.size() ) && ( 0 < record[fieldIdx].length() ) )
            {
            if( 0 == strcmp( "%x", format ) )
                {
                sscanf_s( record[fieldIdx].substr(2).c_str(), format, &value );
                }
            else
                {
                sscanf_s( record[fieldIdx].c_str(), format, &value );
                }
            rawFile.write( ( char* ) &value, sizeof( value ) );
            //field_flags |= ( 0x01 << i );
            }
        fieldIdx++;
        }
    }

//----------------------------------------------------------------------
//! \brief Convert 6 byte hex string into 2 byte unsigned int
//----------------------------------------------------------------------
static void decodeUint16Fields( uint32 & /*field flags*/, int start, int end, ofstream & rawFile, vector<string> & record, uint16 & fieldIdx, char * format = "%x" )
    {
    uint32 value = 0;
    uint16 actualValue = 0;
    for( int i=start; i<=end; i++ )
        {
        if( ( fieldIdx < record.size() ) && ( 0 < record[fieldIdx].length() ) )
            {
            sscanf_s( record[fieldIdx].substr(2).c_str(), format, &value );
            actualValue = (uint16) value;
            rawFile.write( ( char* ) &actualValue, sizeof( actualValue ) );
            //field_flags |= ( 0x01 << i );
            }
        fieldIdx++;
        }
    }

//----------------------------------------------------------------------
//! \brief Convert human-readable formatted timestamp into seconds
//----------------------------------------------------------------------
static void decodeTimestampFields( uint32 & /*field flags*/, int start, int end, ofstream & rawFile, vector<string> & record, uint16 & fieldIdx )
    {
    time_type timestamp;
    for( int i=start; i<=end; i++ )
        {
        if( fieldIdx < record.size() )
            {
            timestamp = 0;
            if( 0 < record[fieldIdx].length() )
                {
                timestamp = parseDateTime( record[fieldIdx] );
                }
            rawFile.write( ( char* ) &timestamp, sizeof( timestamp ) );
            }
        fieldIdx++;
        }
    }

static void checkFieldHeaders( uint32 & field_flags, int start, int end, vector<string> & record, uint16 & fieldIdx )
    {
    for( int i=start; i<=end && (uint32)i<record.size(); i++ )
        {
        if( 0 < record[i].length() )
            {
            field_flags |= ( 0x01 << i );
            }
        }
    fieldIdx = (uint16) end;
    }

//----------------------------------------------------------------------
//! \brief Convert CSV file to raw
//----------------------------------------------------------------------
uint32 AobrdEventLogConverter::convertToRaw( const char * source, const char * dest )
    {
    ifstream csvFile( source, ios_base::in | ios_base::binary );
    if( !csvFile.good() )
        {
        return 0;
        }

    ofstream rawFile( dest, ios_base::out | ios_base::binary );

    evt_log_header_fixed hdr;
    memset( &hdr, 0, sizeof( hdr ) );

    uint32 convertedRecords = 0;

    COleDateTime timestamp;
    COleDateTime cutoff = COleDateTime::GetCurrentTime();
    cutoff -= COleDateTimeSpan(14,0,0,0);    //filter out data over 14 days old
    vector<string> record;
    uint16 cellCount = 0;
    while( 1 < ( cellCount = readCSVRecord( csvFile, record ) ) )
        {
        if( '#' == record[0][0] )
            {
            continue;
            }

        uint16 fieldIdx = 0;
        sscanf_s( record[fieldIdx++].substr(2).c_str(), "%x", &hdr.version );
        //sscanf_s( record[fieldIdx++].substr(2).c_str(), "%x", &hdr.field_flags );
        //sscanf_s( record[fieldIdx++].substr(2).c_str(), "%x", &hdr.reserved );
        uint32 reason = 0;
        sscanf_s( record[fieldIdx++].substr(2).c_str(), "%x", &reason );
        hdr.reason = (uint8) reason;

        switch( reason )
            {
            //filter out everything but Driver Status Changes
            case DRIVER_STATUS_CHANGE:
                break;
            default:
                continue;
            }

        if( !timestamp.ParseDateTime( CString( record[fieldIdx].c_str() ) ) )
            {
            continue;
            }

        if( timestamp < cutoff )
            {
            continue;
            }

        convertedRecords++;

        //reconstruct field flags
        uint16 hdrFieldIdx = fieldIdx;
        //checkFieldHeaders( hdr.field_flags, 0, 14, record, hdrFieldIdx );
        hdr.field_flags = 0x7FFF;

        switch( hdr.reason )
            {
            case DRIVER_ANNOTATION:
                {
                //can't use checkFieldFlags because the fields do not line up with the positions of the field flag bits
                hdr.field_flags |= 0x01 << 27;  //start time
                hdr.field_flags |= 0x01 << 28;  //end time
                hdr.field_flags |= 0x01 << 16;  //text
                break;
                }
            case DRIVER_STATUS_CHANGE:
                {
                checkFieldHeaders( hdr.field_flags, 17, 22, record, hdrFieldIdx );
                hdr.field_flags |= 0x01 << 22;  //verified time is always included
                hdr.field_flags |= 0x01 << 29;  //new status byte A
                hdr.field_flags |= 0x01 << 30;  //new status byte B
                break;
                }
            case DRIVER_VERIFIED_LOG:
                {
                checkFieldHeaders( hdr.field_flags, 23, 23, record, hdrFieldIdx );
                break;
                }
            case DRIVER_COULD_NOT_VERIFY_LOG:
                {
                checkFieldHeaders( hdr.field_flags, 23, 23, record, hdrFieldIdx );
                break;
                }
            case DRIVER_ADDED_NEW_SHIPMENT_ENTRY:
                {
                checkFieldHeaders( hdr.field_flags, 23, 28, record, hdrFieldIdx );
                break;
                }
            case DRIVER_MODIFIED_EXISTING_SHIPMENT_ENTRY:
                {
                checkFieldHeaders( hdr.field_flags, 23, 28, record, hdrFieldIdx );
                break;
                }
            case DRIVER_DELETED_SHIPMENT_ENTRY:
                {
                checkFieldHeaders( hdr.field_flags, 23, 28, record, hdrFieldIdx );
                break;
                }
            case PND_FAILURE_DETECTION:
                {
                checkFieldHeaders( hdr.field_flags, 27, 28, record, hdrFieldIdx );
                break;
                }
            }

        rawFile.write( ( char* ) &hdr, sizeof( hdr ) );

        decodeTimestampFields( hdr.field_flags, 0, 0, rawFile, record, fieldIdx );
        decodeStringFields( hdr.field_flags, 1, 9, rawFile, record, fieldIdx );
        decodeUint32Fields( hdr.field_flags, 10, 10, rawFile, record, fieldIdx, "%d" );
        decodeStringFields( hdr.field_flags, 11, 14, rawFile, record, fieldIdx );

        switch( hdr.reason )
            {
            case DRIVER_ANNOTATION:
                {
                //corresponds to field flag positions 29,30
                decodeTimestampFields( hdr.field_flags, 15, 16, rawFile, record, fieldIdx );
                //corresponds to field flag position 16
                decodeStringFields( hdr.field_flags, 17, 17, rawFile, record, fieldIdx );
                break;
                }
            case DRIVER_STATUS_CHANGE:
                {
                decodeStringFields( hdr.field_flags, 17, 18, rawFile, record, fieldIdx );
                decodeUint32Fields( hdr.field_flags, 19, 19, rawFile, record, fieldIdx );
                decodeDegreeFields( hdr.field_flags, 20, 21, rawFile, record, fieldIdx );
                decodeTimestampFields( hdr.field_flags, 22, 22, rawFile, record, fieldIdx );
                decodeUint16Fields( hdr.field_flags, 29, 29, rawFile, record, fieldIdx );
                break;
                }
            case DRIVER_VERIFIED_LOG:
                {
                decodeTimestampFields( hdr.field_flags, 23, 23, rawFile, record, fieldIdx );
                break;
                }
            case DRIVER_COULD_NOT_VERIFY_LOG:
                {
                decodeTimestampFields( hdr.field_flags, 23, 23, rawFile, record, fieldIdx );
                break;
                }
            case DRIVER_ADDED_NEW_SHIPMENT_ENTRY:
                {
                decodeTimestampFields( hdr.field_flags, 23, 23, rawFile, record, fieldIdx );
                decodeTimestampFields( hdr.field_flags, 27, 28, rawFile, record, fieldIdx );
                decodeStringFields( hdr.field_flags, 24, 26, rawFile, record, fieldIdx );
                break;
                }
            case DRIVER_MODIFIED_EXISTING_SHIPMENT_ENTRY:
                {
                decodeTimestampFields( hdr.field_flags, 23, 23, rawFile, record, fieldIdx );
                decodeTimestampFields( hdr.field_flags, 27, 28, rawFile, record, fieldIdx );
                decodeStringFields( hdr.field_flags, 24, 26, rawFile, record, fieldIdx );
                break;
                }
            case DRIVER_DELETED_SHIPMENT_ENTRY:
                {
                decodeTimestampFields( hdr.field_flags, 23, 23, rawFile, record, fieldIdx );
                decodeTimestampFields( hdr.field_flags, 27, 28, rawFile, record, fieldIdx );
                decodeStringFields( hdr.field_flags, 24, 26, rawFile, record, fieldIdx );
                break;
                }
            case PND_FAILURE_DETECTION:
                {
                decodeUint16Fields( hdr.field_flags, 27, 27, rawFile, record, fieldIdx );
                decodeTimestampFields( hdr.field_flags, 28, 28, rawFile, record, fieldIdx );
                break;
                }
            }

        rawFile.flush();
        }
    rawFile.close();
    csvFile.close();

    return convertedRecords;
    }

//----------------------------------------------------------------------
//! \brief Count records by type
//----------------------------------------------------------------------
uint16 AobrdEventLogConverter::countRecords( const char* source, evt_log_reason type )
    {
    ifstream csvFile( source, ios_base::in | ios_base::binary );
    if( !csvFile.good() )
        {
        return 0;
        }

    uint16 count = 0;

    evt_log_header_fixed hdr;
    memset( &hdr, 0, sizeof( hdr ) );

    //read each record
    vector<string> record;
    uint16 cellCount = 0;
    uint32 reason = 0;
    while( 1 < ( cellCount = readCSVRecord( csvFile, record ) ) )
        {
        if( '#' != record[0][0] )
            {
            sscanf_s( record[1].substr(2).c_str(), "%x", &reason );
            if ( reason == (uint32) type ) {
                count++;
                }
            }
        }
    csvFile.close();

    return count;
    }

//----------------------------------------------------------------------
//! \brief Visit records by type and index
//----------------------------------------------------------------------
bool AobrdEventLogConverter::findRecordAt( const char* source, evt_log_reason type, uint16 index, bool ( *visitor ) ( AobrdEventLogConverter * converter, vector<string> & record, void * data ), void * data )
    {
    ifstream csvFile( source, ios_base::in | ios_base::binary );
    if( !csvFile.good() )
        {
        return 0;
        }

    uint16 count = 0;

    evt_log_header_fixed hdr;
    memset( &hdr, 0, sizeof( hdr ) );

    //read each record
    vector<string> record;
    uint16 cellCount = 0;
    uint32 reason = 0;
    while( 1 < ( cellCount = readCSVRecord( csvFile, record ) ) )
        {
        if( '#' != record[0][0] )
            {
            sscanf_s( record[1].substr(2).c_str(), "%x", &reason );
            if ( reason == (uint32) type ) {
                if( index == count++ )
                    {
                    visitor( this, record, data );
                    break;
                    }
                }
            }
        }
    csvFile.close();
    return( index == count-1 );
    }

//----------------------------------------------------------------------
//! \brief Convert date-time string to UTC timestamp
//----------------------------------------------------------------------
static time_type parseDateTime( std::string cellValue )
    {
    time_type timestamp;
    time_type time, date;
    COleDateTime dt;
    date_time_data_type date_time;
    dt.ParseDateTime( CString( cellValue.c_str() ) );
    date_time.date.year = (uint16) dt.GetYear();
    date_time.date.month = (uint8) dt.GetMonth();
    date_time.date.day = (uint8) dt.GetDay();
    date_time.time.hour = (sint16) dt.GetHour();
    date_time.time.minute = (uint8) dt.GetMinute();
    date_time.time.second = (uint8) dt.GetSecond();
    UTIL_convert_time_type_to_seconds( &date_time, &time );
    UTIL_convert_date_time_to_seconds( &date_time, &date );
    timestamp = date + time;// + dt.GetMilliseconds();
    UTIL_convert_local_to_UTC( &timestamp, &timestamp );
    return timestamp;
    }

//----------------------------------------------------------------------
//! \brief Convert an Annotation record to structure sent to FMI device
//----------------------------------------------------------------------
bool AobrdEventLogConverter::convertAnnotation( std::vector<std::string> & record, fmi_driver_annotation_data_type * annotation )
    {
    annotation->server_timestamp = parseDateTime( record[2] );
    annotation->server_start_time = parseDateTime( record[17] );
    annotation->server_end_time = parseDateTime( record[18] );
    strncpy( annotation->server_annotation, record[19].c_str(), cnt_of_array( annotation->server_annotation ) - 1 );
    annotation->server_annotation[cnt_of_array( annotation->server_annotation ) - 1] = '\0';
    return true;
    }
#endif
