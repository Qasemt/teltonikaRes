/*********************************************************************
*
*   MODULE NAME:
*       FmiLogParser.cpp
*
* Copyright 2008-2012 by Garmin Ltd. or its subsidiaries.
*---------------------------------------------------------------------
* $NoKeywords$
*********************************************************************/

#include "stdafx.h"

#include <fstream>
#include "FmiLogParser.h"
#include "fmi.h"
#include "util.h"
#include "GarminTransportLayer.h"
#if( FMI_SUPPORT_A612 )
#include "fmi_CustomForms.cpp"
#endif

//! Enumeration for packet status values.
//! \details This is used to print various link-level packet issues.
enum PacketStatusType
{
    PACKET_STATUS_OK,
    PACKET_STATUS_FRAMING_ERROR_STX,
    PACKET_STATUS_FRAMING_ERROR_ETX,
    PACKET_STATUS_TOO_SHORT,
    PACKET_STATUS_DLE_STUFF_ERROR
};

const float cMsToMph = 2.23693629f;
const float cMsToKph = 3.6f;

using namespace std;

//----------------------------------------------------------------------
//! \brief Constructor
//! \details Initialize the maps used to format packet and command IDs
//----------------------------------------------------------------------
FmiLogParser::FmiLogParser()
{
    // Initialize map of Garmin packet IDs to names
    initGarminPacketNames();

    // Initialize map of command IDs to names
    initGarminCommandNames();

#if( FMI_SUPPORT_A602 )
    // Initialize the map of FMI packet IDs to names
    initFmiPacketNames();
#endif

#if( FMI_SUPPORT_A607 )
    // Initialize the map of FMI feature IDs to names
    initFmiFeatureNames();
#endif
}

//----------------------------------------------------------------------
//! \brief Destructor
//----------------------------------------------------------------------
FmiLogParser::~FmiLogParser()
{
}

//----------------------------------------------------------------------
//! \brief Construct a packet title for the given log logLine.
//! \details The packet title is normally the Garmin packet name, but
//!     for FMI packets the FMI packet name is used, and for Garmin
//!     commands the Garmin command name is used.
//! \param aPacketNumber The index of the packet to retrieve
//! \return The packet title
//----------------------------------------------------------------------
CString FmiLogParser::getPacketTitle
    (
    int aPacketNumber
    )
{
    CString packetTitle;

    int filenameLength = WideCharToMultiByte( CP_ACP, 0, mLogFilename, -1, NULL, 0, NULL, NULL );
    char *filenameAnsi = new char[filenameLength];
    WideCharToMultiByte( CP_ACP, 0, mLogFilename, -1, filenameAnsi, filenameLength, NULL, NULL );

    ifstream logFile( filenameAnsi, ios_base::in );
    delete[] filenameAnsi;

    if( logFile.good() )
    {
        const char  * logLine;
        std::string   logLineString;
        logFile.seekg( mLineOffset[aPacketNumber], ios_base::beg );

        getline( logFile, logLineString );
        logLine = logLineString.c_str();

        uint8 packetId;
        int i;

        if( logLine[0] == 'T' )
            packetTitle.Format( _T("TX: ") ); //first char tells TX or RX
        else
            packetTitle.Format( _T("RX: ") );
        i = 1;  // i keeps track of the index in the current logLine (already read one char)

        while( logLine[i++] != '-' ); // skip over time and hyphen

        i += 2; //skip the DLE byte
        ASSERT( strlen( logLine+i ) > 0 );        // log must have data
        ASSERT( strlen( logLine+i ) % 2 == 0 );   // log data must be whole uint8 as hex
        UTIL_hex_to_uint8( logLine + i, &packetId, 1 );    //convert 2 hex digits to one byte

        i += 2; //point past packetId
        switch( packetId )
        {
        case ID_COMMAND_BYTE:
            {
                uint16 commandId;
                uint8  commandIdByte[2];
                uint8  sizeByte;

                //not using size here, but need to check for DLE byte stuffing
                UTIL_hex_to_uint8( logLine + i, &sizeByte, 1 );
                i += 2; //skip past size
                if( sizeByte == ID_DLE_BYTE )
                    i += 2; //skip the DLE stuffing
                UTIL_hex_to_uint8( logLine + i, commandIdByte, 2 );  //convert 4 hex digits to 2 uint8s

                //uses little endian...so can't use UTIL_hex_to_uint16()
                commandId = commandIdByte[0] + commandIdByte[1] * 256;
                packetTitle.Append( getGarminCommandName( commandId ) );
                }
            break;
#if( FMI_SUPPORT_A602 )
        case ID_FMI_PACKET:
            {
                uint16 fmiPacketId;
                uint8  packetIdByte[2];
                uint8  sizeByte;

                //not using size here, but need to check for DLE byte stuffing
                UTIL_hex_to_uint8( logLine + i, &sizeByte, 1 );
                i += 2; //skip past size
                if( sizeByte == ID_DLE_BYTE )
                    i += 2; //skip the DLE stuffing
                UTIL_hex_to_uint8( logLine + i, packetIdByte, 2 );
                if( packetIdByte[0] == ID_DLE_BYTE )
                {
                    i += 4; //skip the payload id just read in
                    UTIL_hex_to_uint8( logLine + i, &packetIdByte[1], 1 ); //read in actual byte (not stuffing)
                }
                //uses little endian...so can't use UTIL_hex_to_uint16()
                fmiPacketId = packetIdByte[0] + packetIdByte[1] * 256;
                packetTitle.Append( getFmiPacketName( fmiPacketId ) ); //display the FMI packet name or unknown
                break;
            }
#if( FMI_SUPPORT_A615 )
        case ID_HOSART_PACKET:
            {
                uint16 hosartPacketId;
                uint8  packetIdByte[2];
                uint8  sizeByte;

                //not using size here, but need to check for DLE byte stuffing
                UTIL_hex_to_uint8( logLine + i, &sizeByte, 1 );
                i += 2; //skip past size
                if( sizeByte == ID_DLE_BYTE )
                    i += 2; //skip the DLE stuffing
                UTIL_hex_to_uint8( logLine + i, packetIdByte, 2 );
                if( packetIdByte[0] == ID_DLE_BYTE )
                {
                    i += 4; //skip the payload id just read in
                    UTIL_hex_to_uint8( logLine + i, &packetIdByte[1], 1 ); //read in actual byte (not stuffing)
                }
                //uses little endian...so can't use UTIL_hex_to_uint16()
                hosartPacketId = packetIdByte[0] + packetIdByte[1] * 256;
                packetTitle.Append( getHosartPacketName( hosartPacketId ) ); //display the FMI packet name or unknown
                break;
            }
        case ID_SET_TIME_PACKET:
            {
                packetTitle.Append( _T( "SET_TIME" ) );
                break;
            }
#endif  //end FMI_SUPPORT_A615
#endif  //end FMI_SUPPORT_A602
        default:
            // all other packet IDs are listed with the Garmin packet name
            packetTitle.Append( getGarminPacketName( packetId ) );
            break;
        } // end of switch (packetId)
    }
    logFile.close();
    return packetTitle;
}    /* getPacketTitle() */

//----------------------------------------------------------------------
//! \brief Print a particular packet to the packet view.
//! \details Opens the currently viewed log file, reads to the logLine
//!     requested, and prints the information to the packet view
//!     of the log dialog
//! \param aPacketNumber The index of the packet data logLine to read.
//!     Note that this index does not include the log header; the first
//!     actual data packet logged has an index of 0.
//----------------------------------------------------------------------
CString FmiLogParser::getPacketDetail
    (
    int aPacketNumber
    )
{
    CString packet;

    int filenameLength = WideCharToMultiByte( CP_ACP, 0, mLogFilename, -1, NULL, 0, NULL, NULL );

    char *filenameAnsi = new char[filenameLength];
    WideCharToMultiByte( CP_ACP, 0, mLogFilename, -1, filenameAnsi, filenameLength, NULL, NULL );
    ifstream logFile( filenameAnsi, ios_base::in );
    delete[] filenameAnsi;

    if( logFile.good() )
    {
        const char* logLine;
        std::string        logLineString;
        uint8              packetId;
        uint8              payloadSize;
        int                rawByteCount;
        PacketStatusType   packetStatus = PACKET_STATUS_OK;
        uint8              rawBytes[MAX_PACKET_SIZE];
        uint8              packetChecksum;

        int i = 1; //i used to keep track of index within the logLine (start at one to skip over T/R)
        int packetTimeOffset = 0;
        logFile.seekg( mLineOffset[aPacketNumber] );
        getline( logFile, logLineString );
        logLine = logLineString.c_str();
        while( logLine[i] != '-' )        //read in the time
        {
            packetTimeOffset = packetTimeOffset * 10 + ( logLine[i++] - 0x30 );
        }

        BOOL packetTimeIsMorning = mIsMorning;
        int packetTimeHour       = mLogStartHr;
        int packetTimeMinute     = mLogStartMin;
        int packetTimeSecond     = mLogStartSec;
        int packetTimeMillis     = mLogStartMillis;
        packetTimeMillis += packetTimeOffset % 1000;
        if( packetTimeMillis > 999 )
        {
            packetTimeMillis -= 1000;    //ms overflow...subtract a seconds worth of ms and add one second
            packetTimeSecond++;
        }
        packetTimeSecond += ( packetTimeOffset % 60000 ) / 1000;
        if( packetTimeSecond > 59 )
        {
            packetTimeSecond -= 60;    //sec overflow...subtract a minutes worth of sec and add one minute
            packetTimeMinute++;
        }
        packetTimeMinute += ( packetTimeOffset % 3600000 ) / 60000;
        if( packetTimeMinute > 59 )
        {
            packetTimeMinute -= 60;    //minute overflow...sub an hours worth of mins and add one hour
            packetTimeHour++;
        }
        packetTimeHour += ( packetTimeOffset % 43200000 ) / 3600000;
        if( packetTimeHour >= 12 )
        {
            if( packetTimeHour != 12 ) packetTimeHour -= 12; //if above after noon...no longer morning
            packetTimeIsMorning = !mIsMorning;
        }

        i++;    //skip the hyphen
        rawByteCount = UTIL_hex_to_uint8( logLine + i, rawBytes, sizeof( rawBytes ) );

        // NOTE: From this point forward i refers to the index into rawBytes array, not to line.
        i = 0;
        if( rawBytes[i++] != ID_DLE_BYTE )
            {
            packetStatus = PACKET_STATUS_FRAMING_ERROR_STX;
            }

        packet.Format
            (
            _T(" Time:\t\t %02d:%02d:%02d.%03d "),
            packetTimeHour,
            packetTimeMinute,
            packetTimeSecond,
            packetTimeMillis
            );
        if( packetTimeIsMorning )
            packet.Append( _T("AM\r\n") );
        else
            packet.Append( _T("PM\r\n") );

        packetId = rawBytes[i++];
        packet.AppendFormat( _T(" Packet ID:\t 0x%02x\r\n"), packetId );

        payloadSize = rawBytes[i++];
        if( payloadSize == ID_DLE_BYTE )
        {
            i++;
        }
        packet.AppendFormat( _T(" Payload Size:\t %d\t(dec)\r\n"), payloadSize );

        //parse out the packet's payload, removing DLE stuffing
        uint8 * rawData = new uint8[payloadSize];
        for( int j = 0; j < payloadSize; j++ )
        {
            rawData[j] = rawBytes[i++];
            if( ID_DLE_BYTE == rawData[j] )
            {
                uint8 stuffing = rawBytes[i++];
                if( ID_DLE_BYTE != stuffing )     // check for and remove DLE stuffing
                    packetStatus = PACKET_STATUS_DLE_STUFF_ERROR;
            }
            if( ( i + SIZE_OF_FOOTER ) > rawByteCount )
            {
                packetStatus = PACKET_STATUS_TOO_SHORT;
                break;
            }
        }

        packetChecksum = rawBytes[i++];
        if( ID_DLE_BYTE == packetChecksum )
            {
            i++;
            }

        if( ( i + SIZE_OF_FOOTER ) > rawByteCount )
        {
            packetStatus = PACKET_STATUS_TOO_SHORT;
        }
        else
        {
            if( ID_DLE_BYTE != rawBytes[i++] )
                {
                packetStatus = PACKET_STATUS_FRAMING_ERROR_ETX;
                }
            if( ID_ETX_BYTE != rawBytes[i++] )
                {
                packetStatus = PACKET_STATUS_FRAMING_ERROR_ETX;
                }
        }


        rawByteCount = minval( rawByteCount, i );

        switch( packetStatus )
        {
        case PACKET_STATUS_DLE_STUFF_ERROR:
            packet.Append( _T(" WARNING: DLE stuffing error in application payload\r\n") );
            break;
        case PACKET_STATUS_TOO_SHORT:
            packet.Append( _T(" WARNING: Packet too short (truncated or bad payload size?)\r\n") );
            break;
        case PACKET_STATUS_FRAMING_ERROR_STX:
            packet.Append( _T(" WARNING: Framing error at start of packet\r\n") );
            break;
        case PACKET_STATUS_FRAMING_ERROR_ETX:
            packet.Append( _T(" WARNING: Framing error at end of packet\r\n") );
            break;
        }

        switch( packetId )
        {
        case ID_ACK_BYTE:
        case ID_NAK_BYTE:
            {
                uint8 forPacketId = rawData[0];
                packet.AppendFormat( _T(" For packet:\t 0x%02x\t%s\r\n"), forPacketId, getGarminPacketName( forPacketId ) );
                break;
            }

#if( FMI_SUPPORT_A602 )
        case ID_FMI_PACKET:
            {
                uint16 fmiPacketId = rawData[0] + 256 * rawData[1]; //little endian
                map<uint16, CString>::iterator it = mFmiPacketNames.find( fmiPacketId );
                if( it != mFmiPacketNames.end() )  //find returns end iterator if not found
                {
                    packet.AppendFormat( _T(" FMI Packet ID:\t 0x%04x\r\n\t\t %s\r\n"), fmiPacketId, it->second );
                    packet.Append( formatFmiPacket( logLine[0] == 'T', fmiPacketId, rawData + 2, payloadSize - 2 ) );
                }
                else
                {
                    packet.AppendFormat( _T(" FMI Packet ID:\t 0x%04x\r\n\t\t (Unknown)\r\n"), fmiPacketId );
                }
                break;
            }
#if( FMI_SUPPORT_A615 )
        case ID_HOSART_PACKET:
            {
                uint16 hosartPacketId = rawData[0] + 256 * rawData[1]; //little endian
                map<uint16, CString>::iterator it = mHosartPacketNames.find( hosartPacketId );
                if( it != mHosartPacketNames.end() )  //find returns end iterator if not found
                {
                    packet.AppendFormat( _T(" HOSART Packet ID:\t 0x%04x\r\n\t\t %s\r\n"), hosartPacketId, it->second );
                    packet.Append( formatHosartPacket( logLine[0] == 'T', hosartPacketId, rawData + 2, payloadSize - 2 ) );
                }
                else
                {
                    packet.AppendFormat( _T(" HOSART Packet ID:\t 0x%04x\r\n\t\t (Unknown)\r\n"), hosartPacketId );
                }
                break;
            }
        case ID_SET_TIME_PACKET:
            {
                time_type time;
                memset( &time, 0, sizeof( time ) );
                memcpy( &time, rawData + 2, sizeof( time ) );
                packet.AppendFormat( _T( " SET_TIME:\t 0x%08x - %s\n\r"), time, formatTime( time ) );

                break;
            }
#endif  //FMI_SUPPORT_A615
#endif  //FMI_SUPPORT_A602
        case ID_COMMAND_BYTE:
            {
                uint16 command = rawData[0] + 256 * rawData[1]; //little endian
                packet.AppendFormat( _T(" Command ID:\t 0x%04x\r\n\t\t %s\r\n"), command, getGarminCommandName( command ) );
                break;
            }
        case ID_UNIT_ID:
            {
                unit_id_data_type unitIdData;
                memset( &unitIdData, 0, sizeof( unitIdData ) );
                memcpy( &unitIdData, rawData, minval( payloadSize, sizeof( unitIdData ) ) );
                packet.AppendFormat( _T(" Unit ID:\t 0x%08x\r\n"), unitIdData.unit_id );
                break;
            }
        case ID_DATE_TIME_DATA:
            {
                date_time_data_type dateTimeData;
                memset( &dateTimeData, 0, sizeof( dateTimeData ) );
                memcpy( &dateTimeData, rawData, minval( payloadSize, sizeof( dateTimeData ) ) );
                packet.AppendFormat( _T(" Month:\t %d\n\r"), dateTimeData.date.month );
                packet.AppendFormat( _T(" Day:\t\t %d\n\r"), dateTimeData.date.day );
                packet.AppendFormat( _T(" Year:\t %d\n\r"), dateTimeData.date.year );
                packet.AppendFormat( _T(" Hour:\t %d\n\r"), dateTimeData.time.hour );
                packet.AppendFormat( _T(" Minute:\t %d\n\r"), dateTimeData.time.minute );
                packet.AppendFormat( _T(" Second:\t %d\n\r"), dateTimeData.time.second );
                break;
            }
        case ID_PVT_DATA:
            {
                pvt_data_type pvtData;

                memset( &pvtData, 0, sizeof( pvtData ) );
                memcpy( &pvtData, rawData, minval( payloadSize, sizeof( pvtData ) ) );

                packet.AppendFormat( _T(" Altitude:\t %05.3f\r\n"), pvtData.altitude );
                packet.AppendFormat( _T(" EPE:\t\t %f\r\n"), pvtData.epe );
                packet.AppendFormat( _T(" EPV:\t\t %f\r\n"), pvtData.epv );
                packet.AppendFormat( _T(" EPH:\t\t %f\r\n"), pvtData.eph );
                packet.AppendFormat( _T(" GPS Fix:\t %d\r\n"), pvtData.type_of_gps_fix );
                packet.AppendFormat( _T(" Time of Wk:\t %lf\r\n"), pvtData.time_of_week );
                packet.AppendFormat( _T(" Lat:\t\t %08.6f\r\n"), UTIL_convert_radians_to_degrees( pvtData.position.lat ) );
                packet.AppendFormat( _T(" Lon:\t\t %08.6f\r\n"), UTIL_convert_radians_to_degrees( pvtData.position.lon ) );
                packet.AppendFormat( _T(" East Vel:\t %05.3f\r\n"), pvtData.east_velocity );
                packet.AppendFormat( _T(" North Vel:\t %05.3f\r\n"), pvtData.north_velocity );
                packet.AppendFormat( _T(" Up Vel:\t\t %05.3f\r\n"), pvtData.up_velocity );
                packet.AppendFormat( _T(" M above sea:\t %05.3f\r\n"), pvtData.mean_sea_level_height );
                packet.AppendFormat( _T(" Leap Secs:\t %u\r\n"), pvtData.leap_seconds );
                packet.AppendFormat( _T(" Wk # days:\t %u\r\n"), pvtData.week_number_days );
                break;
            }
#if( FMI_SUPPORT_LEGACY )
        case ID_LEGACY_STOP_MSG:
            {
                legacy_stop_data_type legacyStopData;

                memset( &legacyStopData, 0, sizeof( legacyStopData ) );
                memcpy( &legacyStopData, rawData, minval( payloadSize, sizeof( legacyStopData ) ) );

                packet.AppendFormat( _T(" Latitude:\t %08.6f\r\n"), UTIL_convert_semicircles_to_degrees( legacyStopData.stop_position.lat ) );
                packet.AppendFormat( _T(" Longitude:\t %08.6f\r\n"), UTIL_convert_semicircles_to_degrees( legacyStopData.stop_position.lon ) );
                packet.AppendFormat( _T(" Message:\t %s\r\n"), formatText( legacyStopData.text, sizeof( legacyStopData.text ) ) );
                break;
            }
        case ID_LEGACY_TEXT_MSG:
            {
                char message[200];

                memset( &message, 0, sizeof( message ) );
                memcpy( &message, rawData, minval( payloadSize, sizeof( message ) ) );

                packet.AppendFormat( _T(" Message:\t %s\n\r"), formatText( message, sizeof( message ) ) );
                break;
            }
#endif
        case ID_PRODUCT_DATA:
            {
                product_id_data_type productIdData;
                char strings[MAX_PAYLOAD_SIZE];
                char * currentString = strings + sizeof( productIdData );
                char * end = strings + minval( payloadSize, MAX_PAYLOAD_SIZE );

                memset( &productIdData, 0, sizeof( productIdData ) );
                memset( &strings, 0, sizeof( strings ) );
                memcpy( &productIdData, rawData, minval( payloadSize, sizeof( productIdData ) ) );
                memcpy( &strings, rawData, minval( payloadSize, MAX_PAYLOAD_SIZE ) );

                packet.AppendFormat( _T(" Product ID:\t 0x%04x\n\r"), productIdData.product_id );
                packet.AppendFormat( _T(" SW Version:\t 0x%04x\n\r"), productIdData.software_version );
                while( currentString < end )
                {
                    packet.AppendFormat( _T(" String:\t\t %s\r\n"), CString( currentString ) );
                    currentString += strlen( currentString );
                    ++currentString;
                }
                break;
            }
        default:
            break;
        } // end of switch(aFmiPacketId)


#if( LOG_SHOW_RAW_ASCII )
        packet.Append( _T(" Raw Text:\t ") );
        for( int k = 0; k < rawByteCount; k++ )
        {
            if( k != 0 && k % 32 == 0 )
                packet.Append( _T("\r\n\t\t ") );

            //print the data
            packet.AppendFormat
                (
                _T("%c"),
                isprint( rawBytes[k] ) ? rawBytes[k] : '.'
                );
        }
        packet.Append( _T("\r\n") );
#endif

        packet.Append( _T(" Raw Hex:\t") );
        packet.Append( formatMultiLineHex( rawByteCount, rawBytes ) );
        delete[] rawData;
    }
    logFile.close();

    return packet;
}

//----------------------------------------------------------------------
//! \brief Translate a boolean to an equivalent string
//! \param aBool The boolean value to format
//! \return The CString "TRUE" or "FALSE"
//----------------------------------------------------------------------
CString FmiLogParser::formatBoolean
    (
    boolean aBool
    )
{
    return aBool ? _T("TRUE") : _T("FALSE");
}

//----------------------------------------------------------------------
//! \brief Format latitude
//! \param aSemicircles Semicircles
//! \return The string representation of latitude in degrees
//----------------------------------------------------------------------
CString FmiLogParser::formatLatitude
    (
    sint32  aSemicircles
    )
{
    double latitudeDegrees;
    CString text;

    latitudeDegrees = UTIL_convert_semicircles_to_degrees( aSemicircles );
    if( latitudeDegrees < 0 )
    {
        text.Format( _T( "%08.6f �S" ), latitudeDegrees * -1 );
    }
    else
    {
        text.Format( _T( "%08.6f �N" ), latitudeDegrees );
    }

    return text;
}

//----------------------------------------------------------------------
//! \brief Format longitude
//! \param aSemicircles Semicircles
//! \return The string representation of longitude
//----------------------------------------------------------------------
CString FmiLogParser::formatLongitude
    (
    sint32  aSemicircles
    )
{
    double longitudeDegrees;
    CString text;

    longitudeDegrees = UTIL_convert_semicircles_to_degrees( aSemicircles );
    if( longitudeDegrees < 0 )
    {
        text.Format( _T( "%08.6f �W" ), longitudeDegrees * -1 );
    }
    else
    {
        text.Format( _T(" %08.6f �E" ), longitudeDegrees );
    }

    return text;
}

//----------------------------------------------------------------------
//! \brief Format a message ID in hex, with up to 8 bytes per line.
//! \param aMessageId The message ID to format
//! \param aMessageIdSize The number of significant bytes in the
//!     message ID
//! \return A string representation of the message ID
//----------------------------------------------------------------------
CString FmiLogParser::formatMessageId
    (
    const uint8 * aMessageId,
    uint8         aMessageIdSize
    )
{
    int i = 0;
    CString text;

    for( i = 0; i < minval( aMessageIdSize, 16 ); i++ )
    {
        if( i == 8 )
        {
            text.Append( _T("\n\r\t\t") );
        }
        text.AppendFormat( _T(" %02x"), aMessageId[i] );
    }
    return text;
}

//----------------------------------------------------------------------
//! \brief Format a text string, with up to 29 characters per line.
//! \param aText The text string to format
//! \param aMaxLength The maximum number of bytes to translate
//! \return The string, split into multiple lines
//----------------------------------------------------------------------
CString FmiLogParser::formatText
    (
    const char * aText,
    int aMaxLength
    )
{
    int i;
    WCHAR source[256]; // Big enough to store anything this function will receive
    CString text;

    // If support is available, display Unicode characters properly.
    #if( FMI_SUPPORT_A604 )
        memset( source, 0, sizeof( source ) );
        MultiByteToWideChar( CODEPAGE_UNICODE, 0, aText, aMaxLength, source, 256 );

        for( i = 0; i < 256 && source[i] != '\0'; i++ )
        {
            if( i != 0 && ( i % 29 ) == 0 )
            {
                text.Append( _T("\n\r\t\t ") );
            }
            text.AppendFormat( _T("%c"), source[i] );
        }
    #else
        for( i = 0; i < aMaxLength && aText[i] != '\0'; i++ )
        {
            if( i != 0 && ( i % 29 ) == 0 )
            {
                text.Append( _T("\n\r\t\t ") );
            }
            text.AppendFormat( _T("%c"), aText[i] );
        }
    #endif

    return text;
}

//----------------------------------------------------------------------
//! \brief Format Garmin UTC timestamp
//! \param aTimestamp Garmin UTC timestamp
//! \return The string representation of UTC date and time
//----------------------------------------------------------------------
CString FmiLogParser::formatTime
    (
    time_type aTimestamp
    )
{
    // convert timestamp to date_time
    date_time_data_type date_time;
    UTIL_convert_UTC_to_local( &aTimestamp, &aTimestamp );
    UTIL_convert_seconds_to_date_type( &aTimestamp, &date_time );
    UTIL_convert_seconds_to_time_type( &aTimestamp, &date_time );

    // get string representations of date and time
    char    date[11];
    char    time[13];
    UTIL_format_date_string( &date_time, date, sizeof( date ) );
    UTIL_format_time_string( &date_time, time, sizeof( time ) );

    // format date and time for output
    CString dateString = CString( date, sizeof( date ) );
    CString timeString = CString( time, sizeof( time ) );
    CString text;
    text.Format( _T("%s %s"), dateString, timeString );

    return text;
}

#if( FMI_SUPPORT_A602 )
//----------------------------------------------------------------------
//! \brief Interpret an FMI packet; appending the information to the
//!     text in the packet window.
//! \param transmitted The direction of the packet
//!      true = transmitted, !true = received
//! \param aFmiPacketId The FMI packet ID
//! \param aFmiPayload  The FMI payload
//! \param aFmiPayloadSize The size of the FMI payload in bytes.
//! \since Protocol A602
//----------------------------------------------------------------------
CString FmiLogParser::formatFmiPacket
    (
    BOOL    transmitted,
    uint16  aFmiPacketId,
    uint8 * aFmiPayload,
    uint8   aFmiPayloadSize
    )
{
    CString packet;

    switch( aFmiPacketId )
    {
    case FMI_ID_ENABLE: // 0x0000
        {
#if( FMI_SUPPORT_A607 )
            // This packet only has a payload when A607 support is enabled
            uint8 i;
            fmi_features_data_type features;

            memset( &features, 0, sizeof( features ) );
            memcpy( &features, aFmiPayload, minval( aFmiPayloadSize, sizeof( features ) ) );

            packet.AppendFormat( _T(" Feature Count:\t %u\n\r"), features.feature_count );

            for( i = 0; i < minval( features.feature_count, cnt_of_array( features.features ) ); i++ )
            {
                uint16 id = features.features[i] & FEATURE_ID_MASK;
                uint16 state = features.features[i] & FEATURE_STATE_MASK;

                packet.AppendFormat
                    (
                    _T(" Feature:\t %s is %s\n\r"),
                    mFmiFeatureNames[id],
                    state == FEATURE_STATE_ENABLED ? _T("enabled") : _T("disabled")
                    );
            }
#endif
            break;
        }
    case FMI_ID_PRODUCT_ID_DATA: // 0x0002
        {
            product_id_data_type productIdData;
            memset( &productIdData, 0, sizeof( productIdData ) );
            memcpy( &productIdData, aFmiPayload, minval( aFmiPayloadSize, sizeof( productIdData ) ) );
            packet.AppendFormat( _T(" Prod ID:\t 0x%04x\n\r"), productIdData.product_id );
            packet.AppendFormat( _T(" Soft Ver:\t 0x%04x\n\r"), productIdData.software_version );
            break;
        }

    case FMI_ID_PROTOCOL_DATA: // 0x0003
        {
            protocol_support_data_type protocol;
            unsigned int i;

            packet.Append( _T(" Protocols:\t") );
            for( i = 0; i < ( aFmiPayloadSize - sizeof( protocol ) ); i += sizeof( protocol ) )
            {
                if( i != 0 && ( i / sizeof( protocol ) ) % 5 == 0 )
                {
                    packet.Append( _T("\n\r\t\t") );
                }
                memset( &protocol, 0, sizeof( protocol ) );

                //copy one protocol from aFmiPayload
                memcpy( &protocol, &aFmiPayload[i], sizeof( protocol ) );
                //print one char tag and 3 digit data (e.g.,. A604) and a space to protocol string
                packet.AppendFormat( _T(" %c%03d"), (char)protocol.tag, protocol.data );
            }
            packet.Append( _T("\n\r") );
            break;
        }

    case FMI_ID_TEXT_MSG_ACK: // 0x0020
        {
            text_msg_ack_data_type textMessageAck;
            memset( &textMessageAck, 0, sizeof( textMessageAck ) );
            memcpy( &textMessageAck, aFmiPayload, minval( sizeof( textMessageAck ), aFmiPayloadSize ) );
            packet.AppendFormat( _T(" Orig Time:\t 0x%08x\n\r"), textMessageAck.origination_time );
            packet.AppendFormat( _T(" ID Size:\t %u\n\r"), textMessageAck.id_size );
            packet.AppendFormat( _T(" ID(hex):\t%s\n\r"), formatMessageId( textMessageAck.id, textMessageAck.id_size ) );
            packet.AppendFormat( _T(" Ack Type:\t 0x%08x\n\r"), textMessageAck.msg_ack_type );
            break;
        }

    case FMI_ID_SERVER_OPEN_TXT_MSG: //0x0021
        {
            A602_server_to_client_open_text_msg_data_type textMessage;

            memset( &textMessage, 0, sizeof( textMessage ) );
            memcpy( &textMessage, aFmiPayload, minval( sizeof( textMessage ), aFmiPayloadSize ) );

            packet.AppendFormat( _T(" Orig Time:\t 0x%08x\n\r"), textMessage.origination_time );
            packet.AppendFormat( _T(" Msg:\t\t %s\r\n"), formatText( textMessage.text_message, sizeof( textMessage.text_message ) ) );
            break;
        }

    case FMI_ID_SERVER_OK_ACK_TXT_MSG: //0x0022
    case FMI_ID_SERVER_YES_NO_CONFIRM_MSG: //0x0023
        {
            server_to_client_ack_text_msg_data_type textMessage;

            memset( &textMessage, 0, sizeof( textMessage ) );
            memcpy( &textMessage, aFmiPayload, minval( sizeof( textMessage ), aFmiPayloadSize ) );

            packet.AppendFormat( _T(" Orig Time:\t 0x%08x\n\r"), textMessage.origination_time );
            packet.AppendFormat( _T(" ID Size:\t %u\n\r"), textMessage.id_size );
            packet.AppendFormat( _T(" ID (hex):\t%s\n\r"), formatMessageId( textMessage.id, textMessage.id_size ) );
            packet.AppendFormat( _T(" Message:\t %s\n\r"), formatText( textMessage.text_message, sizeof( textMessage.text_message ) ) );
            break;
        }

#if( FMI_SUPPORT_A603 )
    case FMI_ID_CLIENT_OPEN_TXT_MSG: // 0x0024
        {
            client_to_server_open_text_msg_data_type textMessage;

            memset( &textMessage, 0, sizeof( textMessage ) );
            memcpy( &textMessage, aFmiPayload, minval( aFmiPayloadSize, sizeof( textMessage ) ) );

            packet.AppendFormat( _T(" Orig Time:\t 0x%08x\n\r"), textMessage.origination_time );
            packet.AppendFormat( _T(" Unique ID:\t 0x%08x\n\r"), textMessage.unique_id );
            packet.AppendFormat( _T(" Message:\t %s\n\r"), formatText( textMessage.text_message, sizeof( textMessage.text_message ) ) );

            break;
        }

    case FMI_ID_CLIENT_TXT_MSG_RCPT: //0x0025
        {
            client_to_server_text_msg_receipt_data_type receipt;

            memset( &receipt, 0, sizeof( receipt ) );
            memcpy( &receipt, aFmiPayload, minval( aFmiPayloadSize, sizeof( receipt ) ) );

            packet.AppendFormat( _T(" Unique ID:\t 0x%08x\n\r"), receipt.unique_id );
            break;
        }
#endif

#if( FMI_SUPPORT_A607 )
    case FMI_ID_A607_CLIENT_OPEN_TXT_MSG:         // 0x0026
        {
            client_to_server_D607_open_text_msg_data_type textMessage;

            memset( &textMessage, 0, sizeof( textMessage ) );
            memcpy( &textMessage, aFmiPayload, minval( aFmiPayloadSize, sizeof( textMessage ) ) );

            packet.AppendFormat( _T(" Orig Time:\t 0x%08x\n\r"), textMessage.origination_time );
            packet.AppendFormat( _T(" Lat:\t\t 0x%08x\n\r"), textMessage.scposn.lat );
            packet.AppendFormat( _T(" Lon:\t\t 0x%08x\n\r"), textMessage.scposn.lon );
            packet.AppendFormat( _T(" Unique ID:\t 0x%08x\n\r"), textMessage.unique_id );
            packet.AppendFormat( _T(" Link ID size:\t %u\n\r"), textMessage.id_size );
            packet.AppendFormat( _T(" Link ID (hex):\t%s\n\r"), formatMessageId( textMessage.id, textMessage.id_size ) );
            packet.AppendFormat( _T(" Message:\t %s\n\r"), formatText( textMessage.text_message, sizeof( textMessage.text_message ) ) );
            break;
        }
#endif

#if( FMI_SUPPORT_A604 )
    case FMI_ID_SET_CANNED_RESP_LIST: //0x0028
        {
            canned_response_list_data_type cannedResponseList;

            memset( &cannedResponseList, 0, sizeof( cannedResponseList ) );
            memcpy( &cannedResponseList, aFmiPayload, minval( aFmiPayloadSize, sizeof( cannedResponseList ) ) );

            packet.AppendFormat( _T(" ID Size:\t %u\r\n"), cannedResponseList.id_size );
            packet.AppendFormat( _T(" Resp Count:\t %u\r\n"), cannedResponseList.response_count );
            packet.AppendFormat( _T(" ID (hex):\t%s\n\r"), formatMessageId( cannedResponseList.id, cannedResponseList.id_size ) );

            packet.Append( _T("\n\r Responses:\t") );
            int i = 0;
            while( i < cannedResponseList.response_count )
            {
                if( i != 0 && ( i % 2 ) == 0 )
                    packet.Append( _T("\n\r\t\t") );
                packet.AppendFormat( _T(" 0x%08x"), cannedResponseList.response_id[i++] );
            }
            packet.Append( _T("\n\r") );
            break;
        }

    case FMI_ID_CANNED_RESP_LIST_RCPT: // 0x0029
        {
            canned_response_list_receipt_data_type receipt;

            memset( &receipt, 0, sizeof( receipt ) );
            memcpy( &receipt, aFmiPayload, minval( aFmiPayloadSize, sizeof( receipt ) ) );

            packet.AppendFormat( _T(" ID Size:\t %u\n\r"), receipt.id_size );
            packet.AppendFormat( _T(" Result code:\t %u\n\r"), receipt.result_code );
            packet.AppendFormat( _T(" ID (hex):\t%s\n\r"), formatMessageId( receipt.id, receipt.id_size ) );
            break;
        }

    case FMI_ID_A604_OPEN_TEXT_MSG: // 0x002A
        {
            A604_server_to_client_open_text_msg_data_type textMessage;

            memset( &textMessage, 0, sizeof( textMessage ) );
            memcpy( &textMessage, aFmiPayload, minval( aFmiPayloadSize, sizeof( textMessage ) ) );

            packet.AppendFormat( _T(" Orig Time:\t 0x%08x\n\r"), textMessage.origination_time );
            packet.AppendFormat( _T(" ID Size:\t %u\n\r"), textMessage.id_size );
            packet.AppendFormat( _T(" Msg Type:\t %u\n\r"), textMessage.message_type );
            packet.AppendFormat( _T(" ID (hex):\t%s\n\r"), formatMessageId( textMessage.id, textMessage.id_size ) );
            packet.AppendFormat( _T(" Message:\t %s\n\r"), formatText( textMessage.text_message, sizeof( textMessage.text_message ) ) );
            break;
        }

    case FMI_ID_A604_OPEN_TEXT_MSG_RCPT: // 0x002B
        {
            server_to_client_text_msg_receipt_data_type receipt;

            memset( &receipt, 0, sizeof( receipt ) );
            memcpy( &receipt, aFmiPayload, minval( aFmiPayloadSize, sizeof( receipt ) ) );

            packet.AppendFormat( _T(" Orig Time:\t 0x%08x\n\r"), receipt.origination_time );
            packet.AppendFormat( _T(" ID Size:\t %u\n\r"), receipt.id_size );
            packet.AppendFormat( _T(" Success:\t %s\n\r"), formatBoolean( receipt.result_code ) );
            packet.AppendFormat( _T(" ID (hex):\t%s\n\r"), formatMessageId( receipt.id, receipt.id_size ) );
            break;
        }

    case FMI_ID_TEXT_MSG_ACK_RCPT: // 0x002C
        {
            text_msg_id_data_type receipt;

            memset( &receipt, 0, sizeof( receipt ) );
            memcpy( &receipt, aFmiPayload, minval( aFmiPayloadSize, sizeof( receipt ) ) );

            packet.AppendFormat( _T(" ID Size:\t %u\n\r"), receipt.id_size );
            packet.AppendFormat( _T(" ID (hex):\t%s\n\r"), formatMessageId( receipt.id, receipt.id_size ) );
            break;
        }

#if FMI_SUPPORT_A607
    case FMI_ID_TEXT_MSG_DELETE_REQUEST: // 0x002D
        {
            delete_message_request_data_type messageDeleteRequest;

            memset( &messageDeleteRequest, 0, sizeof( messageDeleteRequest ) );
            memcpy( &messageDeleteRequest, aFmiPayload, minval( aFmiPayloadSize, sizeof( messageDeleteRequest ) ) );

            packet.AppendFormat( _T(" ID Size:\t %u\n\r"), messageDeleteRequest.id_size );
            packet.AppendFormat( _T(" ID (hex):\t%s\n\r"), formatMessageId( messageDeleteRequest.id, messageDeleteRequest.id_size ) );
            break;
        }

    case FMI_ID_TEXT_MSG_DELETE_RESPONSE: // 0x002E
        {
            delete_message_response_data_type deleteResponse;

            memset( &deleteResponse, 0, sizeof( deleteResponse ) );
            memcpy( &deleteResponse, aFmiPayload, minval( aFmiPayloadSize, sizeof( deleteResponse ) ) );

            packet.AppendFormat( _T(" ID Size:\t %u\n\r"), deleteResponse.id_size );
            packet.AppendFormat( _T(" Result Code:\t %u\n\r"), deleteResponse.result_code );
            packet.AppendFormat( _T(" ID (hex):\t%s\n\r"), formatMessageId( deleteResponse.id, deleteResponse.id_size ) );
            break;
        }
#endif

    case FMI_ID_SET_CANNED_RESPONSE: //0x0030
        {
            canned_response_data_type cannedResponse;

            memset( &cannedResponse, 0, sizeof( cannedResponse ) );
            memcpy( &cannedResponse, aFmiPayload, minval( aFmiPayloadSize, sizeof( cannedResponse ) ) );

            packet.AppendFormat( _T(" Response ID:\t 0x%08x\n\r"), cannedResponse.response_id );
            packet.AppendFormat( _T(" Response Text:\t%s\r\n"), formatText( cannedResponse.response_text, sizeof( cannedResponse.response_text ) ) );
            break;
        }

    case FMI_ID_DELETE_CANNED_RESPONSE:    //0x0031
        {
            canned_response_delete_data_type cannedResponseDelete;

            memset( &cannedResponseDelete, 0, sizeof( cannedResponseDelete ) );
            memcpy( &cannedResponseDelete, aFmiPayload, minval( aFmiPayloadSize, sizeof( cannedResponseDelete ) ) );

            packet.AppendFormat( _T(" Response ID:\t 0x%08x\n\r"), cannedResponseDelete.response_id );
            break;
        }

    case FMI_ID_SET_CANNED_RESPONSE_RCPT:    //0x0032
    case FMI_ID_DELETE_CANNED_RESPONSE_RCPT://0x0033
        {
            canned_response_receipt_data_type receipt;

            memset( &receipt, 0, sizeof( receipt ) );
            memcpy( &receipt, aFmiPayload, minval( aFmiPayloadSize, sizeof( receipt ) ) );

            packet.AppendFormat( _T(" Response ID:\t 0x%08x\n\r"), receipt.response_id );
            packet.AppendFormat( _T(" Success:\t %s\n\r"), formatBoolean(receipt.result_code) );
            break;
        }

    case FMI_ID_REFRESH_CANNED_RESP_LIST:    //0x0034
        {
            request_canned_response_list_refresh_data_type refresh;

            memset( &refresh, 0, sizeof( refresh ) );
            memcpy( &refresh, aFmiPayload, minval( aFmiPayloadSize, sizeof( refresh ) ) );

            packet.AppendFormat( _T(" Resp Count:\t %u\n\r"), refresh.response_count );

            packet.AppendFormat( _T(" Response:\t") );
            unsigned int i = 0;
            while( i < refresh.response_count && i <= 50 )
            {
                if( i != 0 && i % 2 == 0 ) packet.Append( _T("\n\r") );
                packet.AppendFormat( _T(" 0x%08x") );
            }
            packet.Append( _T("\n\r") );
            break;
        }

    case FMI_ID_TEXT_MSG_STATUS_REQUEST: //0x0040
        {
            message_status_request_data_type statusRequest;

            memset( &statusRequest, 0, sizeof( statusRequest ) );
            memcpy( &statusRequest, aFmiPayload, minval( aFmiPayloadSize, sizeof( statusRequest ) ) );

            packet.AppendFormat( _T(" ID Size:\t %u\n\r"), statusRequest.id_size );
            packet.AppendFormat( _T(" ID (hex):\t%s\n\r"), formatMessageId( statusRequest.id, statusRequest.id_size ) );
            break;
        }

    case FMI_ID_TEXT_MSG_STATUS: //0x0041
        {
            message_status_data_type messageStatus;

            memset( &messageStatus, 0, sizeof( messageStatus ) );
            memcpy( &messageStatus, aFmiPayload, minval( aFmiPayloadSize, sizeof( messageStatus ) ) );

            packet.AppendFormat( _T(" ID Size:\t %u\n\r"), messageStatus.id_size );
            packet.AppendFormat( _T(" Status Code:\t %u\n\r"), messageStatus.status_code );
            packet.AppendFormat( _T(" ID (hex):\t%s\n\r"), formatMessageId( messageStatus.id, messageStatus.id_size ) );
            break;
        }

    case FMI_ID_SET_CANNED_MSG: //0x0050
        {
            canned_message_data_type message;

            memset( &message, 0, sizeof( message ) );
            memcpy( &message, aFmiPayload, minval( aFmiPayloadSize, sizeof( message ) ) );

            packet.AppendFormat( _T(" Msg ID:\t 0x%08x\n\r"), message.message_id );
            packet.AppendFormat( _T(" Message:\t %s\n\r"), formatText( message.message , sizeof( message.message ) ) );
            break;
        }

    case FMI_ID_SET_CANNED_MSG_RCPT:    //0x0051
    case FMI_ID_DELETE_CANNED_MSG_RCPT:    //0x0053
        {
            canned_message_receipt_data_type receipt;

            memset( &receipt, 0, sizeof( receipt ) );
            memcpy( &receipt, aFmiPayload, minval( aFmiPayloadSize, sizeof( receipt ) ) );

            packet.AppendFormat( _T(" Msg ID:\t 0x%08x\n\r"), receipt.message_id );
            packet.AppendFormat( _T(" Success:\t %s\n\r"), formatBoolean( receipt.result_code ) );
            break;
        }

    case FMI_ID_DELETE_CANNED_MSG: //0x0052
        {
            canned_message_delete_data_type deleteRequest;

            memset( &deleteRequest, 0, sizeof( deleteRequest ) );
            memcpy( &deleteRequest, aFmiPayload, minval( aFmiPayloadSize, sizeof( deleteRequest ) ) );

            packet.AppendFormat( _T(" Msg ID:\t 0x%08x\n\r"), deleteRequest.message_id );
            break;
        }

    case FMI_ID_REFRESH_CANNED_MSG_LIST: // 0x0054
        // No FMI payload associated with this packet
        break;
#endif

#if( FMI_SUPPORT_A611 )
    case FMI_ID_LONG_TEXT_MSG: // 0x0055
        {
            long_text_msg_data_type textMessage;

            memset( &textMessage, 0, sizeof( textMessage ) );
            memcpy( &textMessage, aFmiPayload, minval( aFmiPayloadSize, sizeof( textMessage ) ) );

            packet.AppendFormat( _T(" Orig Time:\t 0x%08x\n\r"), textMessage.origination_time );
            packet.AppendFormat( _T(" ID Size:\t %u\n\r"), textMessage.id_size );
            packet.AppendFormat( _T(" Msg Type:\t %u\n\r"), textMessage.type );
            packet.AppendFormat( _T(" Finished:\t%s\n\r"), formatBoolean( textMessage.finished_flag ) );
            packet.AppendFormat( _T(" Seq Num:\t%u\n\r"), textMessage.sequence_number );
            packet.AppendFormat( _T(" ID (hex):\t%s\n\r"), formatMessageId( textMessage.id, textMessage.id_size ) );
            packet.AppendFormat( _T(" Msg Data:\t %s\n\r"), formatText( textMessage.text_message, sizeof( textMessage.text_message ) ) );
            break;
        }

    case FMI_ID_LONG_TEXT_MSG_RCPT: // 0x0056
        {
            long_text_msg_receipt_data_type receipt;

            memset( &receipt, 0, sizeof( receipt ) );
            memcpy( &receipt, aFmiPayload, minval( aFmiPayloadSize, sizeof( receipt ) ) );

            packet.AppendFormat( _T(" Orig Time:\t 0x%08x\n\r"), receipt.origination_time );
            packet.AppendFormat( _T(" ID Size:\t %u\n\r"), receipt.id_size );
            packet.AppendFormat( _T(" Result Code:\t %u\n\r"), receipt.result_code );
            packet.AppendFormat( _T(" Finished:\t%s\n\r"), formatBoolean( receipt.finished_flag ) );
            packet.AppendFormat( _T(" Seq Num:\t%u\n\r"), receipt.sequence_number );
            packet.AppendFormat( _T(" ID (hex):\t%s\n\r"), formatMessageId( receipt.id, receipt.id_size ) );
            break;
        }
#endif

    case FMI_ID_A602_STOP:    //0x0100
        {
            A602_stop_data_type stop;

            memset( &stop, 0, sizeof( stop ) );
            memcpy( &stop, aFmiPayload, minval( aFmiPayloadSize, sizeof( stop ) ) );

            packet.AppendFormat( _T(" Orig Time:\t 0x%08x\n\r"), stop.origination_time );
            packet.AppendFormat( _T(" Lat:\t\t 0x%08x\n\r"), stop.stop_position.lat );
            packet.AppendFormat( _T(" Lon:\t\t 0x%08x\n\r"), stop.stop_position.lon );
            packet.AppendFormat( _T(" Text:\t\t %s\n\r"), formatText( stop.text, sizeof( stop.text ) ) );
            break;
        }

#if( FMI_SUPPORT_A603 )
    case FMI_ID_A603_STOP: //0x0101
        {
            A603_stop_data_type stop;

            memset( &stop, 0, sizeof( stop ) );
            memcpy( &stop, aFmiPayload, minval( aFmiPayloadSize, sizeof( stop ) ) );

            packet.AppendFormat( _T(" Orig Time:\t 0x%08x\n\r"), stop.origination_time );
            packet.AppendFormat( _T(" Lat:\t\t 0x%08x\n\r"), stop.stop_position.lat );
            packet.AppendFormat( _T(" Lon:\t\t 0x%08x\n\r"), stop.stop_position.lon );
            packet.AppendFormat( _T(" Unique ID:\t 0x%08x\n\r"), stop.unique_id );
            packet.AppendFormat( _T(" Text:\t\t %s\n\r"), formatText( stop.text, sizeof( stop.text ) ) );
            break;
        }
#endif

#if( FMI_SUPPORT_A604 )
    case FMI_ID_SORT_STOP_LIST: // 0x0110
        // no FMI payload associated with this packet
        break;

    case FMI_ID_SORT_STOP_LIST_ACK: // 0x0111
        // no FMI payload associated with this packet
        break;
#endif

#if( FMI_SUPPORT_A607 )
    case FMI_ID_WAYPOINT:                   // 0x0130
        {
            waypoint_data_type waypointData;

            memset( &waypointData, 0, sizeof( waypointData ) );
            memcpy( &waypointData, aFmiPayload, minval( aFmiPayloadSize, sizeof( waypointData ) ) );

            packet.AppendFormat( _T(" Unique ID:\t 0x%04x\n\r"), waypointData.unique_id );
            packet.AppendFormat( _T(" Symbol:\t 0x%04x\n\r"), waypointData.symbol );
            packet.AppendFormat( _T(" Lat:\t\t 0x%08x\n\r"), waypointData.posn.lat );
            packet.AppendFormat( _T(" Lon:\t\t 0x%08x\n\r"), waypointData.posn.lon );
            packet.AppendFormat( _T(" Cat:\t\t 0x%04x\n\r"), waypointData.cat );
            packet.AppendFormat( _T(" Name:\t\t %s\n\r"), formatText( waypointData.name, sizeof( waypointData.name ) ) );
            packet.AppendFormat( _T(" Comment: \t %s\n\r"), formatText( waypointData.comment, sizeof( waypointData.comment ) ) );
            break;
        }

    case FMI_ID_WAYPOINT_RCPT:              // 0x0131
        {
            waypoint_rcpt_data_type waypointReceipt;

            memset( &waypointReceipt, 0, sizeof( waypointReceipt ) );
            memcpy( &waypointReceipt, aFmiPayload, minval( aFmiPayloadSize, sizeof( waypointReceipt ) ) );

            packet.AppendFormat( _T(" Unique ID:\t 0x%04x\n\r"), waypointReceipt.unique_id );
            packet.AppendFormat( _T(" Result Code:\t %u\n\r"), waypointReceipt.result_code );
            break;
        }

    case FMI_ID_WAYPOINT_DELETE:            // 0x0132
    case FMI_ID_WAYPOINT_DELETED_RCPT:      // 0x0134
        {
            uint16 waypointId;

            memset( &waypointId, 0, sizeof( waypointId ) );
            memcpy( &waypointId, aFmiPayload, minval( aFmiPayloadSize, sizeof( waypointId ) ) );

            packet.AppendFormat( _T(" Unique ID:\t 0x%04x\n\r"), waypointId );
            break;
        }

    case FMI_ID_WAYPOINT_DELETED:           // 0x0133
        {
            waypoint_deleted_data_type waypointDeleted;

            memset( &waypointDeleted, 0, sizeof( waypointDeleted ) );
            memcpy( &waypointDeleted, aFmiPayload, minval( aFmiPayloadSize, sizeof( waypointDeleted ) ) );

            packet.AppendFormat( _T(" Unique ID:\t 0x%04x\n\r"), waypointDeleted.unique_id );
            packet.AppendFormat( _T(" Result Code:\t %u\n\r"), waypointDeleted.result_code );

            break;
        }

    case FMI_ID_DELETE_WAYPOINT_CAT:        // 0x0135
        {
            uint16 waypointCategories;

            memset( &waypointCategories, 0, sizeof( waypointCategories ) );
            memcpy( &waypointCategories, aFmiPayload, minval( aFmiPayloadSize, sizeof( waypointCategories ) ) );

            packet.AppendFormat( _T(" Category:\t 0x%04x\n\r"), waypointCategories );
            break;
        }

    case FMI_ID_DELETE_WAYPOINT_CAT_RCPT:   // 0x0136
        {
            delete_by_category_rcpt_data_type receipt;

            memset( &receipt, 0, sizeof( receipt ) );
            memcpy( &receipt, aFmiPayload, minval( aFmiPayloadSize, sizeof( receipt ) ) );

            packet.AppendFormat( _T(" Category:\t 0x%04x\n\r"), receipt.cat_id );
            packet.AppendFormat( _T(" Count:\t\t %u\n\r"), receipt.count );
            break;
        }

    case FMI_ID_CREATE_WAYPOINT_CAT:        // 0x0137
        {
            category_data_type receipt;

            memset( &receipt, 0, sizeof( receipt ) );
            memcpy( &receipt, aFmiPayload, minval( aFmiPayloadSize, sizeof( receipt ) ) );

            packet.AppendFormat( _T(" Category:\t %u\n\r"), receipt.id );
            packet.AppendFormat( _T(" Name: \t\t %s\n\r"), formatText( receipt.name, sizeof( receipt.name ) ) );
            break;
        }

    case FMI_ID_CREATE_WAYPOINT_CAT_RCPT:   // 0x0138
        {
            category_rcpt_data_type receipt;

            memset( &receipt, 0, sizeof( receipt ) );
            memcpy( &receipt, aFmiPayload, minval( aFmiPayloadSize, sizeof( receipt ) ) );

            packet.AppendFormat( _T(" Category:\t %u\n\r"), receipt.id );
            packet.AppendFormat( _T(" Result Code:\t %u\n\r"), receipt.result_code );
            break;
        }

#endif

#if( FMI_SUPPORT_A603 )
    case FMI_ID_ETA_DATA_REQUEST: // 0x0200
        // No FMI payload associated with this packet
        break;

    case FMI_ID_ETA_DATA: //0x0201
        {
            eta_data_type etaData;

            memset( &etaData, 0, sizeof( etaData ) );
            memcpy( &etaData, aFmiPayload, minval( sizeof( etaData ), aFmiPayloadSize ) );

            packet.AppendFormat( _T(" Unique ID:\t 0x%08x\r\n"), etaData.unique_id );
            packet.AppendFormat( _T(" ETA time:\t 0x%08x\r\n"), etaData.eta_time );
            packet.AppendFormat( _T(" Dist to Dest:\t 0x%08x\r\n"), etaData.distance_to_destination );
            packet.AppendFormat( _T(" Lat of Dest:\t 0x%08x\r\n"), etaData.position_of_destination.lat );
            packet.AppendFormat( _T(" Lon of Dest:\t 0x%08x\r\n"), etaData.position_of_destination.lon );
            break;
        }

    case FMI_ID_ETA_DATA_RCPT:       //0x0202
        {
            eta_data_receipt_type receipt;

            memset( &receipt, 0, sizeof( receipt ) );
            memcpy( &receipt, aFmiPayload, minval( aFmiPayloadSize, sizeof( receipt ) ) );

            packet.AppendFormat( _T(" Unique ID:\t 0x%08x\n\r"), receipt.unique_id );
            break;
        }

    case FMI_ID_STOP_STATUS_REQUEST:    // 0x0210
    case FMI_ID_STOP_STATUS:            // 0x0211
        {
            stop_status_data_type stopData;

            memset( &stopData, 0, sizeof( stopData ) );
            memcpy( &stopData, aFmiPayload, minval( aFmiPayloadSize, sizeof( stopData ) ) );

            packet.AppendFormat( _T(" Unique ID:\t 0x%08x\n\r"), stopData.unique_id );
            packet.AppendFormat( _T(" Status:\t\t %u\n\r"), stopData.stop_status );
            packet.AppendFormat( _T(" Index in List:\t %u\n\r"), stopData.stop_index_in_list );
            break;
        }

    case FMI_ID_STOP_STATUS_RCPT: //0x0212
        {
            stop_status_receipt_data_type receipt;
            memset( &receipt, 0, sizeof( receipt ) );
            memcpy( &receipt, aFmiPayload, minval( aFmiPayloadSize, sizeof( receipt ) ) );
            packet.AppendFormat( _T(" Unique ID:\t 0x%08x\n\r"), receipt.unique_id );
            break;
        }

    case FMI_ID_AUTO_ARRIVAL: //0x0220
        {
            auto_arrival_data_type autoArrival;

            memset( &autoArrival, 0, sizeof( autoArrival ) );
            memcpy( &autoArrival, aFmiPayload, minval( aFmiPayloadSize, sizeof( autoArrival ) ) );

            packet.AppendFormat( _T(" Stop Time:\t 0x%08x\n\r"), autoArrival.stop_time );
            packet.AppendFormat( _T(" Stop Dist:\t 0x%08x\n\r"), autoArrival.stop_distance );
            break;
        }

    case FMI_ID_DATA_DELETION: //0x0230
        {
            data_deletion_data_type delData;

            memset( &delData, 0, sizeof( delData ) );
            memcpy( &delData, aFmiPayload, minval( aFmiPayloadSize, sizeof( delData ) ) );

            packet.AppendFormat( _T(" Data Type:\t %u\r\n"), delData.data_type );
            break;
        }
#endif

#if( FMI_SUPPORT_A604 )
    case FMI_ID_USER_INTERFACE_TEXT: // 0x0240
        {
            user_interface_text_data_type uiText;

            memset( &uiText, 0, sizeof( uiText ) );
            memcpy( &uiText, aFmiPayload, minval( aFmiPayloadSize, sizeof( uiText ) ) );

            packet.AppendFormat( _T(" Element ID:\t %u\r\n"), uiText.text_element_id );
            packet.AppendFormat( _T(" New Text:\t %s\r\n"), formatText( uiText.new_text, sizeof( uiText.new_text ) ) );
            break;
        }

    case FMI_ID_USER_INTERFACE_TEXT_RCPT: // 0x0241
        {
            user_interface_text_receipt_data_type receipt;
            memset( &receipt, 0, sizeof( receipt ) );
            memcpy( &receipt, aFmiPayload, minval( aFmiPayloadSize, sizeof( receipt ) ) );
            packet.AppendFormat( _T(" Element ID:\t %u\r\n"), receipt.text_element_id );
            packet.AppendFormat( _T(" Success:\t %s\r\n"), receipt.result_code ? _T("TRUE") : _T("FALSE") );
            break;
        }

    case FMI_ID_MSG_THROTTLING_COMMAND:  // 0x0250
    case FMI_ID_MSG_THROTTLING_RESPONSE: // 0x0251
        {
            message_throttling_data_type throttle;

            memset( &throttle, 0, sizeof( throttle ) );
            memcpy( &throttle, aFmiPayload, minval( aFmiPayloadSize, sizeof( throttle ) ) );

            packet.AppendFormat
                (
                _T(" Throttle ID:\t 0x%04x (%s)\n\r"),
                throttle.packet_id,
                ( mFmiPacketNames.find( throttle.packet_id ) != mFmiPacketNames.end() )
                ? mFmiPacketNames[throttle.packet_id]
                : _T("Unknown FMI Protocol")
                );

            packet.Append( _T(" New State:\t ") );
            switch( throttle.new_state )
            {
                case MESSAGE_THROTTLE_STATE_ENABLE:
                    packet.Append( _T("Enabled\n\r") );
                    break;
                case MESSAGE_THROTTLE_STATE_DISABLE:
                    packet.Append( _T("Disabled\n\r") );
                    break;
                case MESSAGE_THROTTLE_STATE_ERROR:
                    packet.Append( _T("Error\n\r") );
                    break;
                default:
                    packet.Append( _T("Unknown\n\r") );
                    break;
            }
            break;
        }
#endif

#if( FMI_SUPPORT_A605 )
    case FMI_ID_MSG_THROTTLING_QUERY:
        // no FMI payload associated with this packet
        break;

    case FMI_ID_MSG_THROTTLING_QUERY_RESPONSE:
        {
            message_throttling_list_data_type list;
            uint16 i;

            memset( &list, 0, sizeof( list ) );
            memcpy( &list, aFmiPayload, minval( aFmiPayloadSize, sizeof( list ) ) );

            packet.AppendFormat( _T(" Count:\t\t %u\n\r"), list.response_count );

            for( i = 0; i < list.response_count; i++ )
            {
                packet.AppendFormat
                    (
                    _T(" Protocol:\t %s is %s\n\r"),
                    ( mFmiPacketNames.find( list.response_list[i].packet_id ) != mFmiPacketNames.end() )
                        ? mFmiPacketNames[ list.response_list[i].packet_id ]
                        : _T("Unknown FMI protocol"),
                    list.response_list[i].new_state ? _T("enabled") : _T("disabled")
                    );
            }
            break;
        }
#endif

#if( FMI_SUPPORT_A604 )
    case FMI_ID_PING:
        //no FMI payload associated with this packet
        break;

    case FMI_ID_PING_RESPONSE:
        //no FMI payload associated with this packet
        break;

    case FMI_ID_FILE_TRANSFER_START:  // 0x0400
    case FMI_ID_GPI_FILE_INFORMATION: // 0x0407
        {
            file_info_data_type fileInfo;

            memset( &fileInfo, 0, sizeof( fileInfo ) );
            memcpy( &fileInfo, aFmiPayload, minval( aFmiPayloadSize, sizeof( fileInfo ) ) );

            packet.AppendFormat( _T(" File Size:\t %u\n\r"), fileInfo.file_size );
            packet.AppendFormat( _T(" Version Len:\t %u\n\r"), fileInfo.file_version_length );
            packet.AppendFormat( _T(" Type:\t\t %u\n\r"), fileInfo.file_type );
            packet.AppendFormat( _T(" Version (hex):\t %s\n\r"), formatMessageId( fileInfo.file_version, fileInfo.file_version_length ) );
            break;
        }

    case FMI_ID_FILE_DATA_PACKET: //0x0401
        {
            file_packet_data_type fileData;

            memset( &fileData, 0, sizeof( fileData ) );
            memcpy( &fileData, aFmiPayload, minval( aFmiPayloadSize, sizeof( fileData ) ) );
            packet.AppendFormat( _T(" Offset:\t\t %u\n\r"), fileData.offset );
            packet.AppendFormat( _T(" Data Length:\t %u\n\r"), fileData.data_length );

            int length = minval( 245, fileData.data_length );
            packet.Append( _T(" Data (hex):\t") );
            packet.Append( formatMultiLineHex( length, fileData.file_data ) );
            packet.Append( _T("\n\r") );
            break;
        }

    case FMI_ID_FILE_TRANSFER_END: //0x0402
        {
            file_end_data_type fileEndData;

            memset( &fileEndData, 0, sizeof( fileEndData ) );
            memcpy( &fileEndData, aFmiPayload, minval( aFmiPayloadSize, sizeof( fileEndData ) ) );
            packet.AppendFormat( _T(" CRC:\t\t 0x%08x\n\r"), fileEndData.crc );
            break;
        }

    case FMI_ID_FILE_START_RCPT: //0x0403
    case FMI_ID_FILE_END_RCPT:   //0x0405
        {
            uint8 result_code = 0;
            if ( transmitted || FMI_ID_FILE_START_RCPT == aFmiPacketId )
                {
                file_receipt_from_srvr_data_type receipt;

                memset( &receipt, 0, sizeof( receipt ) );
                memcpy( &receipt, aFmiPayload, minval( aFmiPayloadSize, sizeof( receipt ) ) );

                packet.AppendFormat( _T(" File Type:\t %u\n\r"), receipt.file_type );

                result_code = receipt.result_code;
                }
            else
                {
                file_receipt_data_type receipt;

                memset( &receipt, 0, sizeof( receipt ) );
                memcpy( &receipt, aFmiPayload, minval( aFmiPayloadSize, sizeof( receipt ) ) );
                packet.AppendFormat( _T(" Error Code/File Type (1 byte):\t %u\n\r"), receipt.error_code_or_file_type_when_gpi );
                packet.AppendFormat( _T(" Record Count/Reserved Bytes (2 bytes):\t %u\n\r"), receipt.record_count_or_reserved_when_gpi );

                result_code = receipt.result_code;
                }

            switch( result_code )
                {
                case FMI_FILE_RESULT_SUCCESS:
                    {
                    packet.AppendFormat( _T(" Result Code:\t %u - Success\n\r"), result_code );
                    break;
                    }
                case FMI_FILE_RESULT_CRC_ERROR:
                    {
                    packet.AppendFormat( _T(" Result Code:\t %u - CRC Error\n\r"), result_code );
                    break;
                    }
                case FMI_FILE_RESULT_LOW_MEM:
                    {
                    packet.AppendFormat( _T(" Result Code:\t %u - Low Memory\n\r"), result_code );
                    break;
                    }
                case FMI_FILE_RESULT_INVALID_FILE:
                    {
                    packet.AppendFormat( _T(" Result Code:\t %u - Invalid File\n\r"), result_code );
                    break;
                    }
                case FMI_FILE_RESULT_NO_TRANSFER:
                    {
                    packet.AppendFormat( _T(" Result Code:\t %u - No Transfer\n\r"), result_code );
                    break;
                    }
                case FMI_FILE_RESULT_SEVERE:
                    {
                    packet.AppendFormat( _T(" Result Code:\t %u - Severe Error\n\r"), result_code );
                    break;
                    }
                case FMI_FILE_RESULT_INVALID_FILE_TYPE:
                    {
                    packet.AppendFormat( _T(" Result Code:\t %u - Invalid File Type\n\r"), result_code );
                    break;
                    }
                case FMI_FILE_RESULT_INVALID_GPI_FILE:
                    {
                    packet.AppendFormat( _T(" Result Code:\t %u - Invalid GPI File\n\r"), result_code );
                    break;
                    }
                case FMI_FILE_START_OPEN_ERROR:
                    {
                    packet.AppendFormat( _T(" Result Code:\t %u - Error on Opening\n\r"), result_code );
                    break;
                    }
                case FMI_FILE_START_CLOSE_ERROR:
                    {
                    packet.AppendFormat( _T(" Result Code:\t %u - Error on Closing\n\r"), result_code );
                    break;
                    }
                case FMI_FILE_RESULT_FINALIZE_ERROR:
                    {
                    packet.AppendFormat( _T(" Result Code:\t %u - Finalizing Error\n\r"), result_code );
                    break;
                    }
                case FMI_FILE_RESULT_NOT_PROCESSED:
                    {
                    packet.AppendFormat( _T(" Result Code:\t %u - Result Not Processed\n\r"), result_code );
                    break;
                    }
                case FMI_FILE_RESULT_RX_GZIP_ERROR:
                    {
                    packet.AppendFormat( _T(" Result Code:\t %u - GZIP Error\n\r"), result_code );
                    break;
                    }
                case FMI_FILE_RESULT_BUSY:
                    {
                    packet.AppendFormat( _T(" Result Code:\t %u - Busy\n\r"), result_code );
                    break;
                    }
                default:
                    {
                    packet.AppendFormat( _T("Result Code:\t %u - Unrecognized\n\r"), result_code );
                    }
                }
            break;
        }

    case FMI_ID_FILE_PACKET_RCPT: //0x0404
        {
            packet_receipt_data_type receipt;
            memset( &receipt, 0, sizeof( receipt ) );
            memcpy( &receipt, aFmiPayload, minval( aFmiPayloadSize, sizeof( receipt ) ) );
            packet.AppendFormat( _T(" Offset:\t\t %u\n\r"), receipt.offset );
            packet.AppendFormat( _T(" Next Offset:\t %u\n\r"), receipt.next_offset );
            break;
        }

    case FMI_ID_GPI_FILE_INFORMATION_REQUEST:  // 0x0406
        //no FMI payload associated with this packet
        break;

    case FMI_ID_SET_DRIVER_STATUS_LIST_ITEM:   // 0x0800
        {
            driver_status_list_item_data_type item;

            memset( &item, 0, sizeof( item ) );
            memcpy( &item, aFmiPayload, minval( aFmiPayloadSize, sizeof( item ) ) );

            packet.AppendFormat( _T(" ID:\t\t 0x%08x\n\r"), item.status_id );
            packet.AppendFormat( _T(" Status:\t\t %s\n\r"), formatText( item.status, sizeof( item.status ) ) );
            break;
        }

    case FMI_ID_DELETE_DRIVER_STATUS_LIST_ITEM: //0x0801
        {
            driver_status_list_item_delete_data_type item;

            memset( &item, 0, sizeof( item ) );
            memcpy( &item, aFmiPayload, minval( aFmiPayloadSize, sizeof( item ) ) );

            packet.AppendFormat( _T(" ID:\t\t 0x%08x\n\r"), item.status_id );
            break;
        }

    case FMI_ID_SET_DRIVER_STATUS_LIST_ITEM_RCPT: //0x0802
    case FMI_ID_DEL_DRIVER_STATUS_LIST_ITEM_RCPT: //0x0803
        {
            driver_status_list_item_receipt_data_type receipt;

            memset( &receipt, 0, sizeof( receipt ) );
            memcpy( &receipt, aFmiPayload, minval( aFmiPayloadSize, sizeof( receipt ) ) );

            packet.AppendFormat( _T(" ID:\t\t 0x%08x\n\r"), receipt.status_id );
            packet.AppendFormat( _T(" Success:\t %s\n\r"), formatBoolean( receipt.result_code ) );
            break;
        }

    case FMI_ID_DRIVER_STATUS_LIST_REFRESH:
        //no FMI payload associated with this packet
        break;

    case FMI_ID_DRIVER_ID_REQUEST:
        {
#if( FMI_SUPPORT_A607 )
            // This packet only has a payload when A607 support is enabled
            driver_id_request_data_type request;

            memset( &request, 0, sizeof( request ) );
            memcpy( &request, aFmiPayload, minval( aFmiPayloadSize, sizeof( request ) ) );

            packet.AppendFormat( _T(" Driver Index:\t %u\n\r"), request.driver_idx );
#endif
            break;
        }

    case FMI_ID_DRIVER_ID_UPDATE: //0x0811
        {
            driver_id_data_type driverId;

            memset( &driverId, 0, sizeof( driverId ) );
            memcpy( &driverId, aFmiPayload, minval( aFmiPayloadSize, sizeof( driverId ) ) );

            packet.AppendFormat( _T(" Change ID:\t 0x%08x\n\r"), driverId.status_change_id );
            packet.AppendFormat( _T(" Change Time:\t 0x%08x\n\r"), driverId.status_change_time );
            packet.AppendFormat( _T(" Driver ID:\t %s\n\r"), formatText( driverId.driver_id, sizeof( driverId.driver_id ) ) );
            break;
        }

    case FMI_ID_DRIVER_ID_RCPT: //0x0812
        {
            driver_id_receipt_data_type receipt;

            memset( &receipt, 0, sizeof( receipt ) );
            memcpy( &receipt, aFmiPayload, minval( aFmiPayloadSize, sizeof( receipt ) ) );

            packet.AppendFormat( _T(" Change ID:\t 0x%08x\n\r"), receipt.status_change_id );
            packet.AppendFormat( _T(" Success:\t %s\n\r"), formatBoolean( receipt.result_code ) );
            packet.AppendFormat( _T(" Driver Index:\t %u\n\r"), receipt.driver_idx );
            break;
        }

#if( FMI_SUPPORT_A607 )
    case FMI_ID_DRIVER_ID_UPDATE_D607:
        {
            driver_id_D607_data_type driverId;

            memset( &driverId, 0, sizeof( driverId ) );
            memcpy( &driverId, aFmiPayload, minval( aFmiPayloadSize, sizeof( driverId ) ) );

            packet.AppendFormat( _T(" Change ID:\t 0x%08x\n\r"), driverId.status_change_id );
            packet.AppendFormat( _T(" Change Time:\t 0x%08x\n\r"), driverId.status_change_time );
            packet.AppendFormat( _T(" Driver Index:\t %u\n\r"), driverId.driver_idx );
            packet.AppendFormat( _T(" Driver ID:\t %s\n\r"), formatText( driverId.driver_id, sizeof( driverId.driver_id ) ) );

            if( offset_of( driver_id_D607_data_type, password ) < aFmiPayloadSize )
            {
                packet.AppendFormat( _T(" Password:\t %s\n\r"), formatText( driverId.password, sizeof( driverId.password ) ) );
            }
            break;
        }
#endif

    case FMI_ID_DRIVER_STATUS_REQUEST: // 0x0820
        {
#if( FMI_SUPPORT_A607 )
            // This packet only has a payload when A607 support is enabled
            driver_status_request_data_type request;

            memset( &request, 0, sizeof( request ) );
            memcpy( &request, aFmiPayload, minval( aFmiPayloadSize, sizeof( request ) ) );

            packet.AppendFormat( _T(" Driver Index:\t %u\n\r"), request.driver_idx );
#endif
            break;
        }

    case FMI_ID_DRIVER_STATUS_UPDATE:  // 0x0821
        {
            driver_status_data_type status;

            memset( &status, 0, sizeof( status ) );
            memcpy( &status, aFmiPayload, minval( aFmiPayloadSize, sizeof( status ) ) );

            packet.AppendFormat( _T(" Change ID:\t 0x%08x\n\r"), status.status_change_id );
            packet.AppendFormat( _T(" Change Time:\t 0x%08x\n\r"), status.status_change_time );
            packet.AppendFormat( _T(" Status ID:\t 0x%08x\n\r"), status.driver_status );
            break;
        }

    case FMI_ID_DRIVER_STATUS_RCPT: // 0x0822
        {
            driver_status_receipt_data_type receipt;

            memset( &receipt, 0, sizeof( receipt ) );
            memcpy( &receipt, aFmiPayload, minval( aFmiPayloadSize, sizeof( receipt ) ) );

            packet.AppendFormat( _T(" Change ID:\t 0x%08x\n\r"), receipt.status_change_id );
            packet.AppendFormat( _T(" Success:\t %s\n\r"), formatBoolean( receipt.result_code ) );
            packet.AppendFormat( _T(" Driver Index:\t %u\n\r"), receipt.driver_idx );
            break;
        }
#endif //FMI_SUPPORT_A604

#if( FMI_SUPPORT_A607 )
    case FMI_ID_DRIVER_STATUS_UPDATE_D607:  // 0x0823
        {
            driver_status_D607_data_type status;

            memset( &status, 0, sizeof( status ) );
            memcpy( &status, aFmiPayload, minval( aFmiPayloadSize, sizeof( status ) ) );

            packet.AppendFormat( _T(" Change ID:\t 0x%08x\n\r"), status.status_change_id );
            packet.AppendFormat( _T(" Change Time:\t 0x%08x\n\r"), status.status_change_time );
            packet.AppendFormat( _T(" Driver Index:\t %u\n\r"), status.driver_idx );
            packet.AppendFormat( _T(" Status ID:\t 0x%08x\n\r"), status.driver_status );
            break;
        }
#endif

#if( FMI_SUPPORT_A606 )
    case FMI_SAFE_MODE: //0x0900
        {
        safe_mode_speed_data_type request;

        memset( &request, 0, sizeof( request ) );
        memcpy( &request, aFmiPayload, minval( aFmiPayloadSize, sizeof( request ) ) );

        if( request.speed < 0 )
            {
            packet.AppendFormat( _T(" FMI Safe Mode:\tTurn Off\n\r") );
            }
        else
            {
            packet.AppendFormat( _T(" Speed(m/s):\tTurn On\n\r") );
            }
        packet.AppendFormat( _T(" Speed(m/s):\t%f\n\r"),request.speed );
        break;
        }
    case FMI_SAFE_MODE_RESP: //0x0901
        {
        safe_mode_speed_receipt_data_type receipt;

        memset( &receipt, 0, sizeof( receipt ) );
        memcpy( &receipt, aFmiPayload, minval( aFmiPayloadSize, sizeof( receipt ) ) );

        packet.AppendFormat( _T(" Success:\t %s\n\r"), formatBoolean( receipt.result_code ) );
        break;
        }
#endif

#if( FMI_SUPPORT_A608 )
    case FMI_SPEED_LIMIT_SET: //0X1000
        {
        speed_limit_data_type data;

        memset( &data, 0, sizeof( data ) );
        memcpy( &data, aFmiPayload, minval( aFmiPayloadSize, sizeof( data ) ) );

        CString mode;
        switch( data.mode )
        {
            case SPEED_LIMIT_MODE_CAR:
                mode.Format( _T("Car" ) );
                break;
            case SPEED_LIMIT_MODE_OFF:
                mode.Format( _T("Off") );
                break;
            case SPEED_LIMIT_MODE_TRUCK:
                mode.Format( _T("Truck") );
                break;
            default:
                mode.Format( _T("Unknown") );
                break;
        }

        packet.AppendFormat( _T(" Mode:\t\t %d - %s\n\r"), data.mode, mode );
        packet.AppendFormat( _T(" Time Over:\t%d(secs)\n\r"), data.time_over );
        packet.AppendFormat( _T(" Time Under:\t%d(secs)\n\r"), data.time_under );
        packet.AppendFormat( _T(" Alert User:\t%s\n\r"), formatBoolean( data.alert_user ) );
        packet.AppendFormat( _T(" Threshold:\t%f(m/s) - %.2f(mph) - %.2f(kph)\n\r"), data.threshold, data.threshold * cMsToMph, data.threshold * cMsToKph );

        break;
        }
    case FMI_SPEED_LIMIT_RCPT: //0X1001
        {
        speed_limit_receipt_data_type receipt;

        memset( &receipt, 0, sizeof( receipt ) );
        memcpy( &receipt, aFmiPayload, minval( aFmiPayloadSize, sizeof( receipt ) ) );

        CString result_code;
        switch( receipt.result_code )
        {
            case SPEED_LIMIT_RESULT_SUCCESS:
                result_code.Format( _T("Success") );
                break;
            case SPEED_LIMIT_RESULT_ERROR:
                result_code.Format( _T("Error") );
                break;
            case SPEED_LIMIT_RESULT_MODE_UNSUPPORTED:
                result_code.Format( _T("Unsupported" ) );
                break;
            default:
                result_code.Format( _T("Unknown") );
                break;
        }

        packet.AppendFormat( _T(" Result Code:\t %d - %s\n\r"), receipt.result_code, result_code );

        break;
        }
    case FMI_SPEED_LIMIT_ALERT: //0X1002
        {
        speed_limit_alert_data_type alert;

        memset( &alert, 0, sizeof( alert ) );
        memcpy( &alert, aFmiPayload, minval( aFmiPayloadSize, sizeof( alert ) ) );

        CString category;
        switch( alert.category )
        {
        case SPEED_LIMIT_ALERT_BEGIN:
            category.Format( _T("Begin") );
            break;
        case SPEED_LIMIT_ALERT_CHANGE:
            category.Format( _T("Change") );
            break;
        case SPEED_LIMIT_ALERT_END:
            category.Format( _T("End") );
            break;
        case SPEED_LIMIT_ALERT_ERROR:
            category.Format( _T("Error") );
            break;
        case SPEED_LIMIT_ALERT_INVALID:
            category.Format( _T("Invalid") );
            break;
        default:
            category.Format( _T("Unknown") );
            break;
        }

        packet.AppendFormat( _T(" Category:\t %d - %s\n\r"), alert.category, category );
        packet.AppendFormat( _T(" Lat:\t\t 0x%08x - %s\n\r"), alert.posn.lat, formatLatitude( alert.posn.lat ) );
        packet.AppendFormat( _T(" Lon:\t\t 0x%08x - %s\n\r"), alert.posn.lon, formatLongitude( alert.posn.lon ) );
        packet.AppendFormat( _T(" Timestamp:\t 0x%08x - %s\n\r"), alert.timestamp, formatTime( alert.timestamp ) );
        packet.AppendFormat( _T(" Speed:\t\t %f(m/s) - %.2f(mph) - %.2f(kph)\n\r"), alert.speed, alert.speed * cMsToMph, alert.speed * cMsToKph );
        packet.AppendFormat( _T(" Speed Limit:\t %f(m/s) - %.2f(mph) - %.2f(kph)\n\r"), alert.speed_limit, alert.speed_limit * cMsToMph, alert.speed_limit * cMsToKph );
        packet.AppendFormat( _T(" Max Speed:\t %f(m/s) - %.2f(mph) - %.2f(kph)\n\r"), alert.max_speed, alert.max_speed * cMsToMph, alert.max_speed * cMsToKph );
        break;
        }
    case FMI_SPEED_LIMIT_ALERT_RCPT: //0x1003
        {
        speed_limit_alert_receipt_data_type receipt;

        memset( &receipt, 0, sizeof( receipt ) );
        memcpy( &receipt, aFmiPayload, minval( aFmiPayloadSize, sizeof( receipt ) ) );

        packet.AppendFormat( _T(" Timestamp:\t 0x%08x - %s\n\r"), receipt.timestamp, formatTime( receipt.timestamp ) );
        break;
        }
#endif

#if( FMI_SUPPORT_A610 )
    case FMI_SET_ODOMETER_REQUEST: // 0x1100
        {
        set_odometer_request_data_type data;
        memset( &data, 0, sizeof( data) );
        memcpy( &data, aFmiPayload, minval( sizeof( data), aFmiPayloadSize ) );

        packet.AppendFormat( _T(" Odometer Value:\t%d(mi)\n\r"), data.odometer_value );
        break;
        }
    case FMI_DRIVER_LOGIN_REQUEST: // 0x1101
        {
        driver_login_service_data_type data;
        memset( &data, 0, sizeof( data ) );
        memcpy( &data, aFmiPayload, minval( sizeof( data ), aFmiPayloadSize ) );

        packet.AppendFormat( _T(" Driver ID:\t %s\n\r"), CString( data.driver_id ) );
        packet.AppendFormat( _T(" Driver Password:%s\n\r"), CString( data.driver_password ) );
        packet.AppendFormat( _T(" UI Timestamp:\t 0x%08x - %s\n\r"), data.ui_timestamp, formatTime( data.ui_timestamp ) );
        break;
        }
    case FMI_DRIVER_LOGIN_RESPONSE: // 0x1102
        {
        driver_login_service_receipt receipt;
        memset( &receipt, 0, sizeof( receipt ) );
        memcpy( &receipt, aFmiPayload, minval( sizeof( receipt ), aFmiPayloadSize ) );

        packet.AppendFormat( _T(" UI Timestamp:\t 0x%08x - %s\n\r"), receipt.ui_timestamp, formatTime( receipt.ui_timestamp ) );
        packet.AppendFormat( _T(" Result Code:\t %d\n\r"), receipt.result_code );
        break;
        }
    case FMI_DRIVER_PROFILE_DOWNLOAD_REQUEST: //0x1103
        {
        fmi_driver_profile_type data;
        memset( &data, 0, sizeof( data ) );
        memcpy( &data, aFmiPayload, minval( sizeof( data ), aFmiPayloadSize ) );

        packet.AppendFormat( _T(" Driver ID:\t %s\n\r"), CString( data.driver_id ) );
        break;
        }
    case FMI_DRIVER_PROFILE_DOWNLOAD_RESPONSE: //0x1104
    case FMI_DRIVER_PROFILE_UPDATE: //0x1105
        {
        fmi_driver_profile_data_type receipt;
        memset( &receipt, 0, sizeof( receipt ) );
        memcpy( &receipt, aFmiPayload, minval( sizeof( receipt ), aFmiPayloadSize ) );

        packet.AppendFormat( _T(" PIN:\t\t %d\n\r"), receipt.pin );
        packet.AppendFormat( _T(" First Name:\t %s\n\r"), CString( receipt.first_name ) );
        packet.AppendFormat( _T(" Last Name:\t %s\n\r"), CString( receipt.last_name ) );
        packet.AppendFormat( _T(" Driver ID:\t %s\n\r"), CString( receipt.driver_id ) );
        packet.AppendFormat( _T(" Carrier Name:\t %s\n\r"), CString( receipt.carrier_name ) );
        packet.AppendFormat( _T(" Carrier ID:\t %s\n\r"), CString( receipt.carrier_id ) );
        packet.AppendFormat( _T(" Long Term Rule Set:\t %d\n\r"), receipt.long_term_rule_set );
        packet.AppendFormat( _T(" Time Zone:\t %d\n\r"), receipt.time_zone );
        packet.AppendFormat( _T(" Status:\t\t %d\n\r"), receipt.status );
        packet.AppendFormat( _T(" Result Code:\t %d\n\r"), receipt.result_code );
        break;
        }
    case FMI_DRIVER_PROFILE_UPDATE_RESPONSE: //0X110C
        {
        fmi_ack_type receipt;
        memset( &receipt, 0, sizeof( receipt ) );
        memcpy( &receipt, aFmiPayload, minval( sizeof( receipt ), aFmiPayloadSize ) );

        packet.AppendFormat( _T( " Driver ID:\t %s\n\r" ), CString( receipt.driver_id ) );
        switch( receipt.result_code )
            {
            case UPDATE_ACCEPT:
                packet.AppendFormat( _T(" Result Code:\t %d - Accepted\n\r"), receipt.result_code );
                break;
            case UPDATE_FAIL:
                packet.AppendFormat( _T(" Result Code:\t %d - Failed\n\r"), receipt.result_code );
                break;
            case UPDATE_DECLINE:
                packet.AppendFormat( _T(" Result Code:\t %d - Declined\n\r"), receipt.result_code );
                break;
            case UPDATE_NOT_READY:
                packet.AppendFormat( _T(" Result Code:\t %d - Not Ready\n\r"), receipt.result_code );
                break;
            case UPDATE_NO_DATA:
                packet.AppendFormat( _T(" Result Code:\t %d - No Data\n\r"), receipt.result_code );
                break;
            case UPDATE_STORAGE_ERROR:
                packet.AppendFormat( _T(" Result Code:\t %d - Storage Error\n\r"), receipt.result_code );
                break;
            case UPDATE_RESULT_ERROR:
                packet.AppendFormat( _T(" Result Code:\t %d - Result Error\n\r"), receipt.result_code );
                break;
            case UPDATE_INTERFACE_ERROR:
                packet.AppendFormat( _T(" Result Code:\t %d - Interface Error\n\r"), receipt.result_code );
                break;
            }
        break;
        }
    case FMI_DRIVER_STATUS_UPDATE_REQUEST: //0X1106
        {
        fmi_driver_status_update_request_data_type data;
        memset( &data, 0, sizeof( data ) );
        memcpy( &data, aFmiPayload, minval( sizeof( data ), aFmiPayloadSize ) );

        packet.AppendFormat( _T( " Driver ID:\t %s\n\r" ), CString( data.driver_id ) );
        break;
        }
    case FMI_DRIVER_STATUS_UPDATE_RESPONSE: //0X1107
        {
        fmi_driver_status_log_dnld_resp_data_type receipt;
        memset( &receipt, 0, sizeof( receipt ) );
        memcpy( &receipt, aFmiPayload, minval( sizeof( receipt ), aFmiPayloadSize ) );

        packet.AppendFormat( _T( " Driver ID:\t %s\n\r" ), CString( receipt.driver_id ) );
        packet.AppendFormat( _T(" Result Code:\t %d\n\r"), receipt.result_code );

        break;
        }
    case FMI_DRIVER_STATUS_UPDATE_RECEIPT: //0X1108
        {
        fmi_status_log_download_receipt_data_type data;
        memset( &data, 0, sizeof( data ) );
        memcpy( &data, aFmiPayload, minval( sizeof( data ), aFmiPayloadSize ) );

        packet.AppendFormat( _T( " Driver ID:\t %s\n\r" ), CString( data.driver_id ) );

        switch( data.result_code )
            {
            case FMI_LOGIN_NO_ERRORS:
                {
                packet.AppendFormat( _T( " Result Code:\t %d - No Errors\n\r" ), data.result_code );
                break;
                }
            case FMI_LOGIN_WRONG_ID_SERVER_RESPONSE:
                {
                packet.AppendFormat( _T( " Result Code:\t %d - Wrong ID Server Response\n\r" ), data.result_code );
                break;
                }
            case FMI_LOGIN_INVALID_ID_SERVER_RESPONSE:
                {
                packet.AppendFormat( _T( " Result Code:\t %d - Invalid ID Server Response\n\r" ), data.result_code );
                break;
                }
            case FMI_LOGIN_UNEXPECTED_RESPONSE_PACKET:
                {
                packet.AppendFormat( _T( " Result Code:\t %d - Unexpected Response Packet\n\r" ), data.result_code );
                break;
                }
            case FMI_LOGIN_SHIPMENT_DB_ERROR:
                {
                packet.AppendFormat( _T( " Result Code:\t %d - Shipment DB Error\n\r" ), data.result_code );
                break;
                }
            }
        break;
        }
    case FMI_DOWNLOAD_SHIPMENTS_REQUEST: //0X1109
        {
        fmi_driver_profile_type data;
        memset( &data, 0, sizeof( data ) );
        memcpy( &data, aFmiPayload, minval( sizeof( data ), aFmiPayloadSize ) );

        packet.AppendFormat( _T( " Driver ID:\t %s\n\r" ), CString( data.driver_id ) );
        break;
        }
    case FMI_SHIPMENT_DOWNLOAD_RESPONSE: //0X110A
        {
        fmi_driver_shipment_data_type data;
        memset( &data, 0, sizeof( data ) );
        memcpy( &data, aFmiPayload, minval( sizeof( data ), aFmiPayloadSize ) );

        packet.AppendFormat( _T( " Driver ID:\t %s\n\r" ), CString( data.driver_id ) );

        switch( data.server_result_code )
            {
            case CONTAINS_DATA:
                {
                packet.AppendFormat( _T( " Result Code:\t %d - Next Shipment\n\r" ), data.server_result_code );
                packet.AppendFormat( _T( " Srvr Timestamp: 0x%08x - %s\n\r" ), data.server_timestamp, formatTime( data.server_timestamp ) );
                packet.AppendFormat( _T( " Srvr Start Time:\t 0x%08x - %s\n\r" ), data.server_start_time, formatTime( data.server_start_time ) );
                packet.AppendFormat( _T( " Srvr End Time:\t 0x%08x - %s\n\r" ), data.server_end_time, formatTime( data.server_end_time ) );
                packet.AppendFormat( _T( " Shipper Name:\t %s\n\r" ), CString( data.server_shipper_name ) );
                packet.AppendFormat( _T( " Document Num:\t %s\n\r" ), CString( data.server_doc_number ) );
                packet.AppendFormat( _T( " Commodity:\t %s\n\r" ), CString( data.server_commodity ) );
                break;
                }
            case EMPTY_DATA:
                {
                packet.AppendFormat( _T( " Result Code:\t %d - No Shipments\n\r" ), data.server_result_code );
                break;
                }
            case UNKNOWN_DRIVER:
                {
                packet.AppendFormat( _T( " Result Code:\t %d - Unknown Driver\n\r" ), data.server_result_code );
                break;
                }
            case UNSUPPORTED:
                {
                packet.AppendFormat( _T( " Result Code:\t %d - Unsupported\n\r" ), data.server_result_code );
                break;
                }
            }
        break;
        }
    case FMI_SHIPMENT_DOWNLOAD_RECEIPT: //0X110B
        {
        fmi_shipment_download_receipt_data_type data;
        memset( &data, 0, sizeof( data ) );
        memcpy( &data, aFmiPayload, minval( sizeof( data ), aFmiPayloadSize ) );

        packet.AppendFormat( _T( " Driver ID:\t %s\n\r" ), CString( data.driver_id ) );

        switch( data.result_code )
            {
            case FMI_LOGIN_NO_ERRORS:
                {
                packet.AppendFormat( _T( " Result Code:\t %d - No Errors\n\r" ), data.result_code );
                break;
                }
            case FMI_LOGIN_WRONG_ID_SERVER_RESPONSE:
                {
                packet.AppendFormat( _T( " Result Code:\t %d - Wrong ID Server Response\n\r" ), data.result_code );
                break;
                }
            case FMI_LOGIN_INVALID_ID_SERVER_RESPONSE:
                {
                packet.AppendFormat( _T( " Result Code:\t %d - Invalid ID Server Response\n\r" ), data.result_code );
                break;
                }
            case FMI_LOGIN_UNEXPECTED_RESPONSE_PACKET:
                {
                packet.AppendFormat( _T( " Result Code:\t %d - Unexpected Response Packet\n\r" ), data.result_code );
                break;
                }
            case FMI_LOGIN_SHIPMENT_DB_ERROR:
                {
                packet.AppendFormat( _T( " Result Code:\t %d - Shipment DB Error\n\r" ), data.result_code );
                break;
                }
            }
        break;
        }
    case FMI_ANNOTATION_DOWNLOAD_REQUEST: //0X110D
        {
            fmi_annotation_download_request_data_type data;
            memset( &data, 0, sizeof( data ) );
            memcpy( &data, aFmiPayload, minval( sizeof( data ), aFmiPayloadSize ) );

            packet.AppendFormat( _T( " Driver ID:\t %s\n\r" ), CString( data.driver_id ) );
            break;
        }
    case FMI_ANNOTATION_DOWNLOAD_RESPONSE: //0X110E
        {
            fmi_driver_annotation_data_type data;
            memset( &data, 0, sizeof( data ) );
            memcpy( &data, aFmiPayload, minval( sizeof( data ), aFmiPayloadSize ) );

            packet.AppendFormat( _T( " Driver ID:\t %s\n\r" ), CString( data.driver_id ) );

            switch( data.server_result_code )
            {
            case CONTAINS_DATA:
                {
                packet.AppendFormat( _T( " Result Code:\t %d - Next Annotation\n\r" ), data.server_result_code );
                packet.AppendFormat( _T( " Srvr Timestamp: 0x%08x - %s\n\r" ), data.server_timestamp, formatTime( data.server_timestamp ) );
                packet.AppendFormat( _T( " Srvr Start Time:\t 0x%08x - %s\n\r" ), data.server_start_time, formatTime( data.server_start_time ) );
                packet.AppendFormat( _T( " Srvr End Time:\t 0x%08x - %s\n\r" ), data.server_end_time, formatTime( data.server_end_time ) );
                packet.AppendFormat( _T( " Annotation:\t%s\n\r" ), data.server_annotation );
                break;
                }
                case EMPTY_DATA:
                {
                packet.AppendFormat( _T( " Result Code:\t %d - No Annotations\n\r" ), data.server_result_code );
                break;
                }
            case UNKNOWN_DRIVER:
                {
                packet.AppendFormat( _T( " Result Code:\t %d - Unknown Driver\n\r" ), data.server_result_code );
                break;
                }
            case UNSUPPORTED:
                {
                packet.AppendFormat( _T( " Result Code:\t %d - Unsupported\n\r" ), data.server_result_code );
                break;
                }
            }
            break;
        }
    case FMI_ANNOTATION_DOWNLOAD_RECEIPT: //0X110F
        {
            fmi_driver_aobrd_ack_data_type data;
            memset( &data, 0, sizeof( data ) );
            memcpy( &data, aFmiPayload, minval( sizeof( data ), aFmiPayloadSize ) );

            packet.AppendFormat( _T( " Driver ID:\t %s\n\r" ), CString( data.driver_id ) );

            switch( data.result_code )
            {
            case FMI_LOGIN_NO_ERRORS:
                {
                packet.AppendFormat( _T( " Result Code:\t %d - No Errors\n\r" ), data.result_code );
                break;
                }
            case FMI_LOGIN_WRONG_ID_SERVER_RESPONSE:
                {
                packet.AppendFormat( _T( " Result Code:\t %d - Wrong ID Server Response\n\r" ), data.result_code );
                break;
                }
            case FMI_LOGIN_INVALID_ID_SERVER_RESPONSE:
                {
                packet.AppendFormat( _T( " Result Code:\t %d - Invalid ID Server Response\n\r" ), data.result_code );
                break;
                }
            case FMI_LOGIN_UNEXPECTED_RESPONSE_PACKET:
                {
                packet.AppendFormat( _T( " Result Code:\t %d - Unexpected Response Packet\n\r" ), data.result_code );
                break;
                }
            case FMI_LOGIN_ANNOTATION_DB_ERROR:
                {
                packet.AppendFormat( _T( " Result Code:\t %d - Annotation DB Error\n\r" ), data.result_code );
                break;
                }
            }
            break;
        }
#endif

#if( FMI_SUPPORT_A615 )
    case FMI_IFTA_DATA_FETCH_REQUEST: //0x0006
        {
            fmi_ifta_data_fetch_request_type data;
            memset( &data, 0, sizeof( data ) );
            memcpy( &data, aFmiPayload, minval( sizeof( data ), aFmiPayloadSize ) );

            packet.AppendFormat( _T( " Start Time:\t 0x%08x - %s\n\r" ), data.start_time, formatTime( data.start_time ) );
            packet.AppendFormat( _T( " End Time:\t 0x%08x - %s\n\r" ), data.end_time, formatTime( data.end_time ) );

            break;
        }
    case FMI_IFTA_DATA_FETCH_RECEIPT: //0x0007
        {
            fmi_ifta_data_fetch_receipt_type data;
            memset( &data, 0, sizeof( data ) );
            memcpy( &data, aFmiPayload, minval( sizeof( data ), aFmiPayloadSize ) );

            appendIFTADataModResultCode( packet, data.result_code );

            break;
        }
    case FMI_IFTA_DATA_DELETE_REQUEST: //0x0008
        {
            fmi_ifta_data_delete_request_type data;
            memset( &data, 0, sizeof( data ) );
            memcpy( &data, aFmiPayload, minval( sizeof( data ), aFmiPayloadSize ) );

            packet.AppendFormat( _T( " Start Time:\t 0x%08x - %s\n\r" ), data.start_time, formatTime( data.start_time ) );
            packet.AppendFormat( _T( " End Time:\t 0x%08x - %s\n\r" ), data.end_time, formatTime( data.end_time ) );

            break;
        }
    case FMI_IFTA_DATA_DELETE_RECEIPT: //0x0009
        {
            fmi_ifta_data_delete_receipt_type data;
            memset( &data, 0, sizeof( data ) );
            memcpy( &data, aFmiPayload, minval( sizeof( data ), aFmiPayloadSize ) );

            appendIFTADataModResultCode( packet, data.result_code );

            break;
        }
    case FMI_DRIVER_PROFILE_DOWNLOAD_RESPONSE_V2: //0x1110
    case FMI_DRIVER_PROFILE_UPDATE_V2: //0x1111
        {
            fmi_driver_profile_data_type_V2 receipt;
            memset( &receipt, 0, sizeof( receipt ) );
            memcpy( &receipt, aFmiPayload, minval( sizeof( receipt ), aFmiPayloadSize ) );

            packet.AppendFormat( _T( " First Name:\t %s\n\r" ), CString( receipt.first_name ) );
            packet.AppendFormat( _T( " Last Name:\t %s\n\r" ), CString( receipt.last_name ) );
            packet.AppendFormat( _T( " Driver ID:\t %s\n\r" ), CString( receipt.driver_id ) );
            packet.AppendFormat( _T( " Carrier Name:\t %s\n\r" ), CString( receipt.carrier_name ) );
            packet.AppendFormat( _T( " Carrier ID:\t %s\n\r" ), CString( receipt.carrier_id ) );
            packet.AppendFormat( _T( " Long Term Rule Set:\t %d\n\r" ), receipt.long_term_rule_set );
            packet.AppendFormat( _T( " Load Type Rule Set:\t %d\n\r" ), receipt.load_type_rule_set );
            packet.AppendFormat( _T( " Time Zone:\t %d\n\r" ), receipt.time_zone );
            packet.AppendFormat( _T( " Adverse Cond. Time:\t 0x%08x - %s\n\r" ), receipt.adverse_condition_time, formatTime( receipt.adverse_condition_time ) );
            packet.AppendFormat( _T( " Status:\t\t %d\n\r" ), receipt.status );
            packet.AppendFormat( _T( " Result Code:\t %d\n\r" ), receipt.result_code );
            break;
        }
    case FMI_HOS_AUTO_STATUS_FEATURE_REQUEST: //0x1300
        {
            fmi_hos_auto_status_update_feature data;
            memset( &data, 0, sizeof( data ) );
            memcpy( &data, aFmiPayload, minval( sizeof( data ), aFmiPayloadSize ) );

            packet.AppendFormat( _T( " Enable:\t\t %d\n\r" ), data.enable );
            packet.AppendFormat( _T( " Threshold:\t %d second(s)\n\r" ), data.stop_moving_threshold );
            break;
        }
    case FMI_HOS_AUTO_STATUS_FEATURE_RECEIPT: //0x1301
        {
            fmi_hos_auto_status_update_feature_rcpt data;
            memset( &data, 0, sizeof( data ) );
            memcpy( &data, aFmiPayload, minval( sizeof( data ), aFmiPayloadSize ) );

            packet.AppendFormat( _T( " Enable:\t\t %d\n\r" ), data.enable );
            packet.AppendFormat( _T( " Threshold:\t %d second(s)\n\r" ), data.stop_moving_threshold );
            switch( data.result_type )
                {
                case FMI_HOS_SETTING_RESULT_OK:
                    {
                    packet.Append( _T( " Result Type:\t OK\n\r" ) );
                    break;
                    }
                case FMI_HOS_SETTING_RESULT_SET_TO_MIN:
                    {
                    packet.Append( _T( " Result Type:\t Threshold set to Minimum\n\r" ) );
                    break;
                    }
                case FMI_HOS_SETTING_RESULT_SET_TO_MAX:
                    {
                    packet.Append( _T( " Result Type:\t Threshold set to Maximum\n\r" ) );
                    break;
                    }
                case FMI_HOS_SETTING_RESULT_FAIL:
                    {
                    packet.Append( _T( " Result Type:\t Failure\n\r" ) );
                    break;
                    }
                }
            break;
        }
    case FMI_HOS_8_HOUR_RULE_ENABLE_REQUEST:    // 0X1312
    case FMI_HOS_8_HOUR_RULE_ENABLE_RECEIPT:    // 0X1313
        {
            boolean data;
            memcpy( &data, aFmiPayload, minval( sizeof( data ), aFmiPayloadSize ) );

            packet.AppendFormat( _T( " Enable:\t\t %s\n\r" ), data ? _T( "true" ) : _T( "false" ) );
            break;
        }
#endif  // end FMI_SUPPORT_A615

#if( FMI_SUPPORT_A612 )
    case FMI_CUSTOM_FORM_DEL_REQUEST:  // 0x1200
    case FMI_CUSTOM_FORM_GET_POS_REQUEST: // 0x1204
        {
            uint32 form_id = 0;

            memcpy( &form_id, aFmiPayload, minval( aFmiPayloadSize, sizeof( form_id ) ) );

            packet.AppendFormat( _T( " Form ID:\t %d\n\r" ), form_id );

            break;
        }
    case FMI_CUSTOM_FORM_DEL_RECEIPT:  // 0x1201
        {
            custom_form_delete_ack_type message;

            memset( &message, 0, sizeof( message ) );
            memcpy( &message, aFmiPayload, minval( aFmiPayloadSize, sizeof( message ) ) );

            packet.AppendFormat( _T( " Form ID:\t %d\n\r" ), message.form_id );
            packet.AppendFormat( _T( " Result:\t\t %d - %s\n\r" ), message.return_code, CString( getCustomFormErrorMsg( (FMI_cf_rcode) message.return_code ) ) );

            break;
        }
    case FMI_CUSTOM_FORM_MOVE_REQUEST: // 0x1202
        {
            custom_form_move_type message;

            memset( &message, 0, sizeof( message ) );
            memcpy( &message, aFmiPayload, minval( aFmiPayloadSize, sizeof( message ) ) );

            packet.AppendFormat( _T( " Form ID:\t %d\n\r" ), message.form_id );
            packet.AppendFormat( _T( " To Position:\t %d\n\r" ), message.new_position );

            break;
        }
    case FMI_CUSTOM_FORM_MOVE_RECEIPT: // 0x1203
    case FMI_CUSTOM_FORM_GET_POS_RECEIPT: // 0x1205
        {
            custom_form_position_ack_type message;

            memset( &message, 0, sizeof( message ) );
            memcpy( &message, aFmiPayload, minval( aFmiPayloadSize, sizeof( message ) ) );

            packet.AppendFormat( _T( " Form ID:\t %d\n\r" ), message.form_id );
            packet.AppendFormat( _T( " Position:\t %d\n\r" ), message.current_position );
            packet.AppendFormat( _T( " Result:\t\t %d - %s\n\r" ), message.return_code, CString( getCustomFormErrorMsg( (FMI_cf_rcode) message.return_code ) ) );

            break;
        }
#endif
#if( FMI_SUPPORT_A614 )
    case FMI_STOP_CALC_ACK_REQUEST: // 0x1220
        {
            stop_calc_ack_type message;

            memset( &message, 0, sizeof( message ) );
            memcpy( &message, aFmiPayload, minval( aFmiPayloadSize, sizeof( message ) ) );

            packet.AppendFormat( _T( " Unique ID:\t %d\n\r" ), message.unique_id );
            packet.AppendFormat( _T( " Distance:\t 0x%08x\n\r" ), message.distance );
            switch ( message.result_code )
                {
                case 0:
                    {
                    packet.AppendFormat( _T( " Result:\t\t %d - Success\n\r" ), message.result_code );
                    break;
                    }
                default:
                    {
                    packet.AppendFormat( _T( " Result:\t\t %d - Failure\n\r" ), message.result_code );
                    break;
                    }
                }
            break;
        }
    case FMI_STOP_CALC_ACK_RECEIPT: // 0x1221
        {
            stop_calc_ack_rcpt_type message;

            memset( &message, 0, sizeof( message ) );
            memcpy( &message, aFmiPayload, minval( aFmiPayloadSize, sizeof( message ) ) );

            packet.AppendFormat( _T( " Unique ID:\t %d\n\r" ), message.unique_id );
            break;
        }
#endif
#if( FMI_SUPPORT_A613 )
    case FMI_CUSTOM_AVOID_ADD_REQUEST: // 0x1230
        {
            custom_avoid_type message;

            memset( &message, 0, sizeof( message ) );
            memcpy( &message, aFmiPayload, minval( aFmiPayloadSize, sizeof( message ) ) );

            packet.AppendFormat( _T( " Unique ID:\t %d\n\r" ), message.unique_id );
            packet.AppendFormat( _T( " North:\t\t %08.6f\t(%d)\n\r" ), UTIL_convert_semicircles_to_degrees( message.point1.lat ), message.point1.lat );
            packet.AppendFormat( _T( " East:\t\t %08.6f\t(%d)\n\r" ), UTIL_convert_semicircles_to_degrees( message.point1.lon ), message.point1.lon );
            packet.AppendFormat( _T( " South:\t\t %08.6f\t(%d)\n\r" ), UTIL_convert_semicircles_to_degrees( message.point2.lat ), message.point2.lat );
            packet.AppendFormat( _T( " West:\t\t %08.6f\t(%d)\n\r" ), UTIL_convert_semicircles_to_degrees( message.point2.lon ), message.point2.lon );
            packet.AppendFormat( _T( " Enabled:\t %s\n\r" ), message.enable ? _T( "true" ) : _T( "false" ) );
            packet.AppendFormat( _T( " Name:\t\t %s\n\r" ), formatText( message.name, sizeof( message.name ) ) );

            break;
        }
    case FMI_CUSTOM_AVOID_DEL_REQUEST: // 0x1232
        {
            custom_avoid_delete_type message;
            memset( &message, 0, sizeof( message ) );
            memcpy( &message, aFmiPayload, minval( aFmiPayloadSize, sizeof( message ) ) );

            packet.AppendFormat( _T( " Unique ID:\t %d\n\r" ), message.unique_id );

            break;
        }
    case FMI_CUSTOM_AVOID_TOGGLE_REQUEST: // 0x1235
        {
            custom_avoid_enable_type message;
            memset( &message, 0, sizeof( message ) );
            memcpy( &message, aFmiPayload, minval( aFmiPayloadSize, sizeof( message ) ) );

            packet.AppendFormat( _T( " Unique ID:\t %d\n\r" ), message.unique_id );
            packet.AppendFormat( _T( " Enabled:\t %s\n\r" ), message.enable == 1 ? _T( "true" ) : _T( "false" ) );

            break;
        }
    case FMI_CUSTOM_AVOID_ENABLE_FEATURE_REQUEST: // 0x1237
    case FMI_CUSTOM_AVOID_ENABLE_FEATURE_RECEIPT: // 0x1237
        {
            custom_avoid_feature_enable_type message;
            memset( &message, 0, sizeof( message ) );
            memcpy( &message, aFmiPayload, minval( aFmiPayloadSize, sizeof( message ) ) );

            packet.AppendFormat( _T( " Orig Time:\t 0x%08x\n\r" ), message.origination_time );
            packet.AppendFormat( _T( " Enabled:\t %s\n\r" ), message.enable == 1 ? _T( "true" ) : _T( "false" ) );

            break;
        }
    case FMI_CUSTOM_AVOID_ADD_RECEIPT: // 0x1231
    case FMI_CUSTOM_AVOID_DEL_RECEIPT: // 0x1233
    case FMI_CUSTOM_AVOID_TOGGLE_RECEIPT: // 0x1235
        {
            custom_avoid_rcpt_type receipt;
            memset( &receipt, 0, sizeof( receipt ) );
            memcpy( &receipt, aFmiPayload, minval( aFmiPayloadSize, sizeof( receipt ) ) );

            packet.AppendFormat( _T( " Unique ID:\t %d\n\r" ), receipt.unique_id );
            switch ( receipt.result_code ) {
                case CUSTOM_AVOID_SUCCESS:
                    {
                        packet.AppendFormat( _T( " Result:\t\t %d - %s\n\r" ), receipt.result_code, _T( "Success") );
                        break;
                    }
                case CUSTOM_AVOID_ID_NOT_FOUND:
                    {
                        packet.AppendFormat( _T( " Result:\t\t %d - %s\n\r" ), receipt.result_code, _T( "Error - ID not found") );
                        break;
                    }
                case CUSTOM_AVOID_ERR_FULL:
                    {
                        packet.AppendFormat( _T( " Result:\t\t %d - %s\n\r" ), receipt.result_code, _T( "Error - Full") );
                        break;
                    }
                case CUSTOM_AVOID_ERR_NV:
                    {
                        packet.AppendFormat( _T( " Result:\t\t %d - %s\n\r" ), receipt.result_code, _T( "Error - NV") );
                        break;
                    }
                case CUSTOM_AVOID_ERR_NM_INUSE:
                    {
                        packet.AppendFormat( _T( " Result:\t\t %d - %s\n\r" ), receipt.result_code, _T( "Error - NM in use") );
                        break;
                    }
                case CUSTOM_AVOID_ERR_ID_OUT_OF_RANGE:
                    {
                        packet.AppendFormat( _T( " Result:\t\t %d - %s\n\r" ), receipt.result_code, _T( "Error - ID out of range") );
                        break;
                    }
                case CUSTOM_AVOID_ERR_FEAT_NOT_ENBL:
                    {
                        packet.AppendFormat( _T( " Result:\t\t %d - %s\n\r" ), receipt.result_code, _T( "Error - Custom Avoidance feature is not enabled") );
                        break;
                    }
                }

            break;
        }
#endif
#if( FMI_SUPPORT_A615 )
    case FMI_AOBRD_DRIVER_LOGOFF_REQUEST: // 0x1310
        {
            fmi_logoff_driver_request message;

            memset( &message, 0, sizeof( message ) );
            memcpy( &message, aFmiPayload, minval( aFmiPayloadSize, sizeof( message ) ) );

            packet.AppendFormat( _T( " Driver ID:\t %s\n\r" ), CString( message.driver_id ) );
            break;
        }
    case FMI_AOBRD_DRIVER_LOGOFF_RECEIPT: // 0x1311
        {
            fmi_logoff_driver_receipt receipt;

            memset( &receipt, 0, sizeof( receipt ) );
            memcpy( &receipt, aFmiPayload, minval( aFmiPayloadSize, sizeof( receipt ) ) );

            packet.AppendFormat( _T( " Driver ID:\t %s\n\r" ), CString( receipt.driver_id ) );
            packet.AppendFormat( _T( " Status:\t\t %u\n\r" ), receipt.status );
            packet.AppendFormat( _T( " Result code:\t %u\n\r" ), receipt.result_code );
            break;
        }
#endif

#if( FMI_SUPPORT_A616 )

    case FMI_SET_BAUD_REQUEST: //0x0011
        {
            fmi_set_baud_request data;
            memset( &data, 0, sizeof( data ) );
            memcpy( &data, aFmiPayload, minval( sizeof( data ), aFmiPayloadSize ) );

            switch( data.request_type )
                {
                case FMI_BAUD_NEW_RATE_REQUEST:
                    {
                    packet.Append( _T( " Request Type:\t New Rate\n\r" ) );
                    break;
                    }
                case FMI_BAUD_RATE_SYNC_REQUEST:
                    {
                    packet.Append( _T( " Request Type:\t Rate Sync\n\r" ) );
                    break;
                    }
                }
            switch( data.baud_rate_type )
                {
                case FMI_BAUD_RATE_9600:
                    {
                    packet.Append( _T( " Baud Rate:\t 9600\n\r" ) );
                    break;
                    }
                case FMI_BAUD_RATE_57600:
                    {
                    packet.Append( _T( " Baud Rate:\t 57600\n\r" ) );
                    break;
                    }
                }
            break;
        }
    case FMI_SET_BAUD_RECEIPT: //0x0012
        {
            fmi_set_baud_receipt data;
            memset( &data, 0, sizeof( data ) );
            memcpy( &data, aFmiPayload, minval( sizeof( data ), aFmiPayloadSize ) );

            switch( data.request_type )
                {
                case FMI_BAUD_NEW_RATE_REQUEST:
                    {
                    packet.Append( _T( " Request Type:\t New Rate\n\r" ) );
                    break;
                    }
                case FMI_BAUD_RATE_SYNC_REQUEST:
                    {
                    packet.Append( _T( " Request Type:\t Rate Sync\n\r" ) );
                    break;
                    }
                }
            switch( data.baud_rate_type )
                {
                case FMI_BAUD_RATE_9600:
                    {
                    packet.Append( _T( " Baud Rate:\t 9600\n\r" ) );
                    break;
                    }
                case FMI_BAUD_RATE_57600:
                    {
                    packet.Append( _T( " Baud Rate:\t 57600\n\r" ) );
                    break;
                    }
                }
            switch( data.result_code )
                {
                case FMI_BAUD_RATE_OK:
                    {
                    packet.Append( _T( " Result Code:\t Ok\n\r" ) );
                    break;
                    }
                case FMI_BAUD_REQUEST_TYPE_SERVER_ERROR:
                    {
                    packet.Append( _T( " Result Code:\t Server Error (Request Type)\n\r" ) );
                    break;
                    }
                case FMI_BAUD_RATE_SERVER_ERROR:
                    {
                    packet.Append( _T( " Result Code:\t Server Error (Rate)\n\r" ) );
                    break;
                    }
                case FMI_BAUD_RATE_CLIENT_ERROR:
                    {
                    packet.Append( _T( " Result Code:\t Client Error\n\r" ) );
                    break;
                    }
                case FMI_BAUD_SET_ERROR:
                    {
                    packet.Append( _T( " Result Code:\t Baud Set Error\n\r" ) );
                    break;
                    }
                case FMI_BAUD_PLUG_ID_ERROR:
                    {
                    packet.Append( _T( " Result Code:\t Plug ID Error\n\r" ) );
                    break;
                    }
                }
            break;
        }
#endif
#if( FMI_SUPPORT_A617 )
    case FMI_ALERT_POPUP_REQUEST: //0x1400
        {
            fmi_alert_popup_request data;
            memset( &data, 0, sizeof( data ) );
            memcpy( &data, aFmiPayload, minval( sizeof( data ), aFmiPayloadSize ) );

            packet.AppendFormat( _T( " Unique ID:\t %d\n\r" ), data.unique_id );

            switch( data.icon )
                {
                case FMI_ALERT_ICON_NONE:
                    {
                    packet.Append( _T( " Icon:\t\t none\n\r" ) );
                    break;
                    }
                case FMI_ALERT_ICON_DRIVER_BEHAVIOR:
                    {
                    packet.Append( _T( " Icon:\t\t Driver Behavior\n\r" ) );
                    break;
                    }
                case FMI_ALERT_ICON_TIRE_PRESSURE:
                    {
                    packet.Append( _T( " Icon:\t\t Tier Pressure\n\r" ) );
                    break;
                    }
                case FMI_ALERT_ICON_TEMPERATURE:
                    {
                    packet.Append( _T( " Icon:\t\t Temperature\n\r" ) );
                    break;
                    }
                case FMI_ALERT_ICON_DOOR_SENSOR:
                    {
                    packet.Append( _T( " Icon:\t\t Door Sensor\n\r" ) );
                    break;
                    }
                case FMI_ALERT_ICON_VEHICLE_MAINTENANCE:
                    {
                    packet.Append( _T( " Icon:\t\t Vehicle Maintenance\n\r" ) );
                    break;
                    }
                case FMI_ALERT_ICON_OBD2_GENERIC_SENSOR:
                    {
                    packet.Append( _T( " Icon:\t\t OBD-II Generic Sensor\n\r" ) );
                    break;
                    }
                case FMI_ALERT_ICON_GENERIC_SENSOR1:
                    {
                    packet.Append( _T( " Icon:\t\t Generic Sensor 1\n\r" ) );
                    break;
                    }
                case FMI_ALERT_ICON_GENERIC_SENSOR2:
                    {
                    packet.Append( _T( " Icon:\t\t Generic Sensor 2\n\r" ) );
                    break;
                    }
                case FMI_ALERT_ICON_GENERIC_SENSOR3:
                    {
                    packet.Append( _T( " Icon:\t\t Generic Sensor 3\n\r" ) );
                    break;
                    }
                case FMI_ALERT_ICON_GENERAL_CONNECTIVITY:
                    {
                    packet.Append( _T( " Icon:\t\t General Connectivity\n\r" ) );
                    break;
                    }
                case FMI_ALERT_ICON_DAILY_HOURS_COUNTER:
                    {
                    packet.Append( _T( " Icon:\t\t Daily Hours Counter\n\r" ) );
                    break;
                    }
                case FMI_ALERT_ICON_WEEKLY_HOURS_COUNTER:
                    {
                    packet.Append( _T( " Icon:\t\t Weekly Hours Counter\n\r" ) );
                    break;
                    }
                case FMI_ALERT_ICON_REST_HOURS_COUNTER:
                    {
                    packet.Append( _T( " Icon:\t\t Rest Hours Counter\n\r" ) );
                    break;
                    }
                case FMI_ALERT_ICON_BREAK_HOURS_COUNTER:
                    {
                    packet.Append( _T( " Icon:\t\t Break Hours Counter\n\r" ) );
                    break;
                    }
                case FMI_ALERT_ICON_TASKS:
                    {
                    packet.Append( _T( " Icon:\t\t Tasks\n\r" ) );
                    break;
                    }
                case FMI_ALERT_ICON_WEIGHT:
                    {
                    packet.Append( _T( " Icon:\t\t Weight\n\r" ) );
                    break;
                    }
                case FMI_ALERT_ICON_INFORMATION:
                    {
                    packet.Append( _T( " Icon:\t\t Information\n\r" ) );
                    break;
                    }
                case FMI_ALERT_ICON_FUEL:
                    {
                    packet.Append( _T( " Icon:\t\t Fuel\n\r" ) );
                    break;
                    }
                case FMI_ALERT_ICON_EU_AVAILABLE:
                    {
                    packet.Append( _T( " Icon:\t\t EU Available\n\r" ) );
                    break;
                    }
                case FMI_ALERT_ICON_EU_DRIVING:
                    {
                    packet.Append( _T( " Icon:\t\t EU Driving\n\r" ) );
                    break;
                    }
                case FMI_ALERT_ICON_EU_REST:
                    {
                    packet.Append( _T( " Icon:\t\t EU Rest\n\r" ) );
                    break;
                    }
                case FMI_ALERT_ICON_EU_WORK:
                    {
                    packet.Append( _T( " Icon:\t\t EU Work\n\r" ) );
                    break;
                    }
                default:
                    {
                    packet.Append( _T( " Icon:\t\t RESERVED\n\r" ) );
                    break;
                    }
                }

            packet.AppendFormat( _T( " Timeout:\t %d\n\r" ), data.timeout );
            packet.AppendFormat( _T( " Severity:\t %d\n\r" ), data.severity );
            packet.AppendFormat( _T( " Play sound:\t %s\n\r" ), data.play_sound ? _T( "true" ) : _T( "false" ) );
            packet.AppendFormat( _T( " Text:\t\t %s\n\r" ), CString( data.alert_text ) );
            break;
        }

    case FMI_ALERT_POPUP_RECEIPT: //0x1401
        {
            fmi_alert_popup_receipt data;
            memset( &data, 0, sizeof( data ) );
            memcpy( &data, aFmiPayload, minval( sizeof( data ), aFmiPayloadSize ) );

            packet.AppendFormat( _T( " Unique ID:\t %d\n\r" ), data.unique_id );

            switch( data.result_code )
                {
                case FMI_ALERT_POPUP_SUCCESS:
                    {
                    packet.Append( _T( " Result:\t Success\n\r" ) );
                    break;
                    }
                case FMI_ALERT_TEXT_TOO_LONG:
                    {
                    packet.Append( _T( " Result:\t Text too long\n\r" ) );
                    break;
                    }
                case FMI_ALERT_ICON_NUM_OUT_OF_RANGE:
                    {
                    packet.Append( _T( " Result:\t Icon number out of range (alert will be displayed with no icon)\n\r" ) );
                    break;
                    }
                case FMI_ALERT_MISSING_ALERT_TEXT_AND_ICON:
                    {
                    packet.Append( _T( " Result:\t No text or icon (at least one is required)\n\r" ) );
                    break;
                    }
                case FMI_ALERT_SEVERITY_OUT_OF_RANGE:
                    {
                    packet.Append( _T( " Result:\t Severity out of range\n\r" ) );
                    break;
                    }
                case FMI_ALERT_TIMEOUT_OUT_OF_RANGE:
                    {
                    packet.Append( _T( " Result:\t Timeout out of range\n\r" ) );
                    break;
                    }
                }
            break;
        }
    case FMI_SENSOR_CONFIG_REQUEST: //0x1402
        {
            fmi_sensor_config_request data;
            memset( &data, 0, sizeof( data ) );
            memcpy( &data, aFmiPayload, minval( sizeof( data ), aFmiPayloadSize ) );

            packet.AppendFormat( _T( " Change ID:\t %d\n\r" ), data.change_id );
            packet.AppendFormat( _T( " Unique ID:\t %d\n\r" ), data.unique_id );
            packet.AppendFormat( _T( " Name:\t\t %s\n\r" ), CString( data.name ) );

            switch( data.icon )
                {
                case FMI_ALERT_ICON_NONE:
                    {
                    packet.Append( _T( " Icon:\t\t none\n\r" ) );
                    break;
                    }
                case FMI_ALERT_ICON_DRIVER_BEHAVIOR:
                    {
                    packet.Append( _T( " Icon:\t\t Driver Behavior\n\r" ) );
                    break;
                    }
                case FMI_ALERT_ICON_TIRE_PRESSURE:
                    {
                    packet.Append( _T( " Icon:\t\t Tier Pressure\n\r" ) );
                    break;
                    }
                case FMI_ALERT_ICON_TEMPERATURE:
                    {
                    packet.Append( _T( " Icon:\t\t Temperature\n\r" ) );
                    break;
                    }
                case FMI_ALERT_ICON_DOOR_SENSOR:
                    {
                    packet.Append( _T( " Icon:\t\t Door Sensor\n\r" ) );
                    break;
                    }
                case FMI_ALERT_ICON_VEHICLE_MAINTENANCE:
                    {
                    packet.Append( _T( " Icon:\t\t Vehicle Maintenance\n\r" ) );
                    break;
                    }
                case FMI_ALERT_ICON_OBD2_GENERIC_SENSOR:
                    {
                    packet.Append( _T( " Icon:\t\t OBD-II Generic Sensor\n\r" ) );
                    break;
                    }
                case FMI_ALERT_ICON_GENERIC_SENSOR1:
                    {
                    packet.Append( _T( " Icon:\t\t Generic Sensor 1\n\r" ) );
                    break;
                    }
                case FMI_ALERT_ICON_GENERIC_SENSOR2:
                    {
                    packet.Append( _T( " Icon:\t\t Generic Sensor 2\n\r" ) );
                    break;
                    }
                case FMI_ALERT_ICON_GENERIC_SENSOR3:
                    {
                    packet.Append( _T( " Icon:\t\t Generic Sensor 3\n\r" ) );
                    break;
                    }
                case FMI_ALERT_ICON_GENERAL_CONNECTIVITY:
                    {
                    packet.Append( _T( " Icon:\t\t General Connectivity\n\r" ) );
                    break;
                    }
                case FMI_ALERT_ICON_DAILY_HOURS_COUNTER:
                    {
                    packet.Append( _T( " Icon:\t\t Daily Hours Counter\n\r" ) );
                    break;
                    }
                case FMI_ALERT_ICON_WEEKLY_HOURS_COUNTER:
                    {
                    packet.Append( _T( " Icon:\t\t Weekly Hours Counter\n\r" ) );
                    break;
                    }
                case FMI_ALERT_ICON_REST_HOURS_COUNTER:
                    {
                    packet.Append( _T( " Icon:\t\t Rest Hours Counter\n\r" ) );
                    break;
                    }
                case FMI_ALERT_ICON_BREAK_HOURS_COUNTER:
                    {
                    packet.Append( _T( " Icon:\t\t Break Hours Counter\n\r" ) );
                    break;
                    }
                case FMI_ALERT_ICON_TASKS:
                    {
                    packet.Append( _T( " Icon:\t\t Tasks\n\r" ) );
                    break;
                    }
                case FMI_ALERT_ICON_WEIGHT:
                    {
                    packet.Append( _T( " Icon:\t\t Weight\n\r" ) );
                    break;
                    }
                case FMI_ALERT_ICON_INFORMATION:
                    {
                    packet.Append( _T( " Icon:\t\t Information\n\r" ) );
                    break;
                    }
                case FMI_ALERT_ICON_FUEL:
                    {
                    packet.Append( _T( " Icon:\t\t Fuel\n\r" ) );
                    break;
                    }
                case FMI_ALERT_ICON_EU_AVAILABLE:
                    {
                    packet.Append( _T( " Icon:\t\t EU Available\n\r" ) );
                    break;
                    }
                case FMI_ALERT_ICON_EU_DRIVING:
                    {
                    packet.Append( _T( " Icon:\t\t EU Driving\n\r" ) );
                    break;
                    }
                case FMI_ALERT_ICON_EU_REST:
                    {
                    packet.Append( _T( " Icon:\t\t EU Rest\n\r" ) );
                    break;
                    }
                case FMI_ALERT_ICON_EU_WORK:
                    {
                    packet.Append( _T( " Icon:\t\t EU Work\n\r" ) );
                    break;
                    }
                default:
                    {
                    packet.Append( _T( " Icon:\t\t RESERVED\n\r" ) );
                    break;
                    }
                }

            packet.AppendFormat( _T( " Display index:\t %d\n\r" ), data.display_index );
            break;
        }
        case FMI_SENSOR_CONFIG_RECEIPT: //0x1403
        case FMI_SENSOR_DELETE_RECEIPT: //0x1405
        {
            fmi_sensor_receipt data;
            memset( &data, 0, sizeof( data ) );
            memcpy( &data, aFmiPayload, minval( sizeof( data ), aFmiPayloadSize ) );

            packet.AppendFormat( _T( " Change ID:\t %d\n\r" ), data.change_id );

            switch( data.result_code )
                {
                case FMI_SENSOR_SUCCESS:
                    {
                    packet.Append( _T( " Result:\t\t Success\n\r" ) );
                    break;
                    }
                case FMI_SENSOR_ERR_NAME_TOO_LONG:
                    {
                    packet.Append( _T( " Result:\t\t Name too long\n\r" ) );
                    break;
                    }
                case FMI_SENSOR_ERR_ICON_NUM_OUT_OF_RANGE:
                    {
                    packet.Append( _T( " Result:\t\t Icon number out of range (alert will be displayed with no icon)\n\r" ) );
                    break;
                    }
                case FMI_SENSOR_ERR_MISSING_SENSOR_NAME:
                    {
                    packet.Append( _T( " Result:\t\t Sensor name must be specified\n\r" ) );
                    break;
                    }
                case FMI_SENSOR_ERR_SEVERITY_OUT_OF_RANGE:
                    {
                    packet.Append( _T( " Result:\t\t Severity out of range\n\r" ) );
                    break;
                    }
                case FMI_SENSOR_ERR_STATUS_TOO_LONG:
                    {
                    packet.Append( _T( " Result:\t\t Status too long\n\r" ) );
                    break;
                    }
                case FMI_SENSOR_ERR_DESCR_TOO_LONG:
                    {
                    packet.Append( _T( " Result:\t\t Description too long\n\r" ) );
                    break;
                    }
                case FMI_SENSOR_ERR_TOO_MANY_SENSORS:
                    {
                    packet.Append( _T( " Result:\t\t Too many sensors\n\r" ) );
                    break;
                    }
                case FMI_SENSOR_ERR_ID_NOT_FOUND:
                    {
                    packet.Append( _T( " Result:\t\t Unique ID not found\n\r" ) );
                    break;
                    }
                case FMI_SENSOR_ERR_DB:
                    {
                    packet.Append( _T( " Result:\t\t Error saving to database\n\r" ) );
                    break;
                    }
                }
            switch ( data.operation_mode )
                {
                case 0:
                    {
                    packet.Append( _T( " Operation:\t delete\n\r" ) );
                    break;
                    }
                case 1:
                    {
                    packet.Append( _T( " Operation:\t add\n\r" ) );
                    break;
                    }
                case 2:
                    {
                    packet.Append( _T( " Operation:\t modify\n\r" ) );
                    break;
                    }
                }
            break;
        }
    case FMI_SENSOR_DELETE_REQUEST: //0x1404
        {
            fmi_sensor_delete_request data;
            memset( &data, 0, sizeof( data ) );
            memcpy( &data, aFmiPayload, minval( sizeof( data ), aFmiPayloadSize ) );

            packet.AppendFormat( _T( " Change ID:\t %d\n\r" ), data.change_id );
            packet.AppendFormat( _T( " Unique ID:\t %d\n\r" ), data.unique_id );
            break;
        }
    case FMI_SENSOR_UPDATE_REQUEST: //0X1406
        {
            fmi_sensor_update_request data;
            memset( &data, 0, sizeof( data ) );
            memcpy( &data, aFmiPayload, minval( sizeof( data ), aFmiPayloadSize ) );

            packet.AppendFormat( _T( " Change ID:\t %d\n\r" ), data.change_id );
            packet.AppendFormat( _T( " Unique ID:\t %d\n\r" ), data.unique_id );
            packet.AppendFormat( _T( " Severity:\t %d\n\r" ), data.severity );
            packet.AppendFormat( _T( " Play sound:\t %s\n\r" ), data.play_sound ? _T( "true" ) : _T( "false" ) );
            packet.AppendFormat( _T( " Record:\t %s\n\r" ), data.record_sensor ? _T( "true" ) : _T( "false" ) );
            packet.AppendFormat( _T( " Status:\t\t %s\n\r" ), CString( data.status ) );
            packet.AppendFormat( _T( " Description:\t %s\n\r" ), CString( data.description ) );
            break;
        }
    case FMI_SENSOR_QUERY_DISPLAY_INDEX_REQUEST: //0X1407
        {
            fmi_sensor_query_display_index_request data;
            memset( &data, 0, sizeof( data ) );
            memcpy( &data, aFmiPayload, minval( sizeof( data ), aFmiPayloadSize ) );

            packet.AppendFormat( _T( " Change ID:\t %d\n\r" ), data.change_id );
            packet.AppendFormat( _T( " Unique ID:\t %d\n\r" ), data.unique_id );
            break;
        }
    case FMI_SENSOR_QUERY_DISPLAY_INDEX_RECEIPT: //0X1408
        {
            fmi_sensor_query_display_index_receipt data;
            memset( &data, 0, sizeof( data ) );
            memcpy( &data, aFmiPayload, minval( sizeof( data ), aFmiPayloadSize ) );

            packet.AppendFormat( _T( " Change ID:\t %d\n\r" ), data.change_id );

            switch( data.result_code )
                {
                case FMI_SENSOR_SUCCESS:
                    {
                    packet.Append( _T( " Result:\t\t Success\n\r" ) );
                    break;
                    }
                case FMI_SENSOR_ERR_NAME_TOO_LONG:
                    {
                    packet.Append( _T( " Result:\t\t Name too long\n\r" ) );
                    break;
                    }
                case FMI_SENSOR_ERR_ICON_NUM_OUT_OF_RANGE:
                    {
                    packet.Append( _T( " Result:\t\t Icon number out of range (alert will be displayed with no icon)\n\r" ) );
                    break;
                    }
                case FMI_SENSOR_ERR_MISSING_SENSOR_NAME:
                    {
                    packet.Append( _T( " Result:\t\t Sensor name must be specified\n\r" ) );
                    break;
                    }
                case FMI_SENSOR_ERR_SEVERITY_OUT_OF_RANGE:
                    {
                    packet.Append( _T( " Result:\t\t Severity out of range\n\r" ) );
                    break;
                    }
                case FMI_SENSOR_ERR_STATUS_TOO_LONG:
                    {
                    packet.Append( _T( " Result:\t\t Status too long\n\r" ) );
                    break;
                    }
                case FMI_SENSOR_ERR_DESCR_TOO_LONG:
                    {
                    packet.Append( _T( " Result:\t\t Description too long\n\r" ) );
                    break;
                    }
                case FMI_SENSOR_ERR_TOO_MANY_SENSORS:
                    {
                    packet.Append( _T( " Result:\t\t Too many sensors\n\r" ) );
                    break;
                    }
                case FMI_SENSOR_ERR_ID_NOT_FOUND:
                    {
                    packet.Append( _T( " Result:\t\t Unique ID not found\n\r" ) );
                    break;
                    }
                case FMI_SENSOR_ERR_DB:
                    {
                    packet.Append( _T( " Result:\t\t Error saving to database\n\r" ) );
                    break;
                    }
                }
            packet.AppendFormat( _T( " Display Index:\t %d\n\r" ), data.display_index );
            break;
        }
#endif
#if( FMI_SUPPORT_A619 )
    case FMI_HOS_SET_SETTING_DATA_REQUEST: //0X1500
        {
        fmi_hos_set_settings_request_data_type data;
        memset( &data, 0, sizeof( data ) );
        memcpy( &data, aFmiPayload, minval( sizeof( data ), aFmiPayloadSize ) );

        switch( data.tag )
            {
            case HOS_SETTING_AUTO_STATUS_STOP_MOVING_THRESHOLD_SECONDS:
                {
                packet.Append( _T( " Tag:\t\t Stop Moving Threshold\n\r" ) );
                break;
                }
            case HOS_SETTING_EIGHT_HOUR_RULE_ENABLE:
                {
                packet.Append( _T( " Tag:\t\t 8 Hour Rule\n\r" ) );
                break;
                }
            case HOS_SETTING_PERIODIC_STATUS_PERIOD_SECONDS:
                {
                packet.Append( _T( " Tag:\t\t Periodic Status Interval\n\r" ) );
                break;
                }
            }
        packet.AppendFormat( _T( " Value:\t\t %d\n\r" ), data.settings_value );
        packet.AppendFormat( _T( " Enabled:\t %s\n\r" ), data.enable ? _T( "true" ) : _T( "false" ) );
        break;
    }
    case FMI_HOS_SET_SETTING_DATA_RECEIPT: //0X1501
        {
        fmi_hos_set_settings_receipt_data_type data;
        memset( &data, 0, sizeof( data ) );
        memcpy( &data, aFmiPayload, minval( sizeof( data ), aFmiPayloadSize ) );

        switch( data.tag )
            {
            case HOS_SETTING_AUTO_STATUS_STOP_MOVING_THRESHOLD_SECONDS:
                {
                packet.Append( _T( " Tag:\t\t Stop Moving Threshold\n\r" ) );
                break;
                }
            case HOS_SETTING_EIGHT_HOUR_RULE_ENABLE:
                {
                packet.Append( _T( " Tag:\t\t 8 Hour Rule\n\r" ) );
                break;
                }
            case HOS_SETTING_PERIODIC_STATUS_PERIOD_SECONDS:
                {
                packet.Append( _T( " Tag:\t\t Periodic Status Interval\n\r" ) );
                break;
                }
            }
        packet.AppendFormat( _T( " Value:\t\t %d\n\r" ), data.settings_value );
        packet.AppendFormat( _T( " Enabled:\t %s\n\r" ), data.enabled ? _T( "true" ) : _T( "false" ) );
        switch( data.result_code )
            {
            case FMI_HOS_SETTING_RESULT_OK:
                {
                packet.Append( _T( " Result Code:\t OK\n\r" ) );
                break;
                }
            case FMI_HOS_SETTING_RESULT_SET_TO_MIN:
                {
                packet.Append( _T( " Result Code:\t Threshold set to Minimum\n\r" ) );
                break;
                }
            case FMI_HOS_SETTING_RESULT_SET_TO_MAX:
                {
                packet.Append( _T( " Result Code:\t Threshold set to Maximum\n\r" ) );
                break;
                }
            case FMI_HOS_SETTING_RESULT_FAIL:
                {
                packet.Append( _T( " Result Code:\t Failure\n\r" ) );
                break;
                }
            }
        break;
        }
#endif

    default:
        break;
    }

    return packet;
}

//----------------------------------------------------------------------
//! \brief Translate an FMI packet ID into a string describing the
//!     packet name.
//! \details Uses the mFmiPacketNames map.
//! \param aPacketId The FMI packet ID
//! \return The packet name as an English CString, or the string
//!     "Unknown FMI packet 0xNNNN" if the packet does not have a name
//----------------------------------------------------------------------
CString FmiLogParser::getFmiPacketName
    (
    uint16 aPacketId
    )
{
    CString packetName;

    if( mFmiPacketNames.find( aPacketId ) == mFmiPacketNames.end() )
    {
        packetName.Format( _T("Unknown FMI packet 0x%04x"), aPacketId );
    }
    else
    {
        packetName = mFmiPacketNames[aPacketId];
    }

    return packetName;
}
#endif //FMI_SUPPORT_A602

#if( FMI_SUPPORT_A615 )
CString FmiLogParser::getHosartPacketName
(
 uint16 aPacketId
 )
    {
    CString packetName;

    if( mHosartPacketNames.find( aPacketId ) == mHosartPacketNames.end() )
    {
        packetName.Format( _T("Unknown HOSART packet 0x%04x"), aPacketId );
    }
    else
    {
        packetName = mHosartPacketNames[aPacketId];
    }

    return packetName;
}

//----------------------------------------------------------------------
//! \brief Interpret an FMI packet; appending the information to the
//!     text in the packet window.
//! \param transmitted The direction of the packet
//!      true = transmitted, !true = received
//! \param aFmiPacketId The FMI packet ID
//! \param aFmiPayload  The FMI payload
//! \param aFmiPayloadSize The size of the FMI payload in bytes.
//! \since Protocol A602
//----------------------------------------------------------------------
CString FmiLogParser::formatHosartPacket
    (
    BOOL    /* transmitted */,
    uint16  aPacketId,
    uint8 * aPayload,
    uint8   aPayloadSize
    )
{
    CString packet;

    switch( aPacketId )
    {
    case HOSART_CHANGE_STATUS: // 0x0000
        {
        hosart_change_status data;
        memset( &data, 0, sizeof( data ) );
        memcpy( &data, aPayload, minval( aPayloadSize, sizeof( data ) ) );

        packet.AppendFormat( _T( " Driver ID:\t %d\n\r" ), data.driver_id );
        packet.AppendFormat( _T( " New Status:\t %d\n\r" ), data.new_status );
        break;
        }
    case HOSART_CHANGE_TO_DRIVING_WARNING: // 0x0001
        {
        break;
        }
    case HOSART_WARNING_FOUND: // 0x0002
        {
        hosart_change_to_driving_warning data;
        memset( &data, 0, sizeof( data ) );
        memcpy( &data, aPayload, minval( aPayloadSize, sizeof( data ) ) );

        packet.AppendFormat( _T( " Warning Type:\t %d\n\r" ), data.warning_type );
        packet.AppendFormat( _T( " Violation Type:\t %d\n\r" ), data.violation_type );
        packet.AppendFormat( _T( " Driver ID:\t %d\n\r" ), data.driver_id );
        packet.AppendFormat( _T( " Violation Time:\t 0x%08x - %s\n\r"), data.timestamp, formatTime( data.timestamp ) );
        break;
        }
    case HOSART_VIOLATION_FOUND: // 0x0003
        {
        hosart_violation_found data;
        memset( &data, 0, sizeof( data ) );
        memcpy( &data, aPayload, minval( aPayloadSize, sizeof( data ) ) );

        packet.AppendFormat( _T( " Violation Type:\t %d\n\r" ), data.violation_type );
        packet.AppendFormat( _T( " Driver ID:\t %d\n\r" ), data.driver_id );
        packet.AppendFormat( _T( " Violation Time:\t 0x%08x - %s\n\r"), data.timestamp, formatTime( data.timestamp ) );
        break;
        }
    case HOSART_SET_PS_TO_OFF: // 0x0004
        {
        uint32 data;
        memset( &data, 0, sizeof( data ) );
        memcpy( &data, aPayload, minval( aPayloadSize, sizeof( data ) ) );
        packet.AppendFormat( _T( " Seconds Moved:\t 0x%08x - %s\n\r"), data, formatTime( data ) );
        break;
        }
    }

    return packet;
}
#endif

//----------------------------------------------------------------------
//! \brief Translate a Garmin packet ID into a string describing the
//!     packet name.
//! \details Uses the mGarminPacketNames map.
//! \param aPacketId The FMI packet ID
//! \return The packet name as an English CString, or the string
//!     "Unknown Garmin packet 0xNNNN" if the packet does not have a
//!     name
//----------------------------------------------------------------------
CString FmiLogParser::getGarminPacketName
    (
    uint8 aPacketId
    )
{
    CString packetName;

    if( mGarminPacketNames.find( aPacketId ) == mGarminPacketNames.end() )
    {
        packetName.Format( _T("Unknown Garmin packet 0x%02x"), aPacketId );
    }
    else
    {
        packetName = mGarminPacketNames[aPacketId];
    }

    return packetName;
}

//----------------------------------------------------------------------
//! \brief Translate a Garmin command ID into a string describing the
//!     command name.
//! \details Uses the mGarminCommandNames map.
//! \param aCommandId The Garmin command ID
//! \return The command name as an English CString, or the string
//!     "Unknown Garmin command 0xNNNN" if the command does not have a
//!     name
//----------------------------------------------------------------------
CString FmiLogParser::getGarminCommandName
    (
    uint16 aCommandId
    )
{
    CString commandName;

    if( mGarminCommandNames.find( aCommandId ) == mGarminCommandNames.end() )
    {
        commandName.Format( _T("Unknown Garmin command 0x%04x"), aCommandId );
    }
    else
    {
        commandName = mGarminCommandNames[aCommandId];
    }

    return commandName;
}

#if( FMI_SUPPORT_A615 )
void FmiLogParser::appendIFTADataModResultCode
    (
    CString & packet,
    uint8 result_code
    )
    {
    switch( result_code )
        {
        case FMI_IFTA_OK:
            {
            packet.AppendFormat( _T( " Result Code:\t %d - OK\n\r" ), result_code );
            break;
            }
        case FMI_IFTA_EXPORT_NO_DATA_FOUND:
            {
            packet.AppendFormat( _T( " Result Code:\t %d - No Data Found\n\r" ), result_code );
            break;
            }
        case FMI_IFTA_BUSY_ERROR:
            {
            packet.AppendFormat( _T( " Result Code:\t %d - Busy\n\r" ), result_code );
            break;
            }
        case FMI_IFTA_NULL_INPUT_PTR_ERROR:
            {
            packet.AppendFormat( _T( " Result Code:\t %d - NULL Input Pointer\n\r" ), result_code );
            break;
            }
        case FMI_IFTA_STATE_ERROR:
            {
            packet.AppendFormat( _T( " Result Code:\t %d - State error\n\r" ), result_code );
            break;
            }
        case FMI_IFTA_MIN_DATA_SIZE_ERROR:
            {
            packet.AppendFormat( _T( " Result Code:\t %d - Minimum Data Size error\n\r" ), result_code );
            break;
            }
        case FMI_IFTA_MAX_DATA_SIZE_ERROR:
            {
            packet.AppendFormat( _T( " Result Code:\t %d - Maximum Data Size error\n\r" ), result_code );
            break;
            }
        case FMI_IFTA_MALLOC_ERROR:
            {
            packet.AppendFormat( _T( " Result Code:\t %d - Malloc error\n\r" ), result_code );
            break;
            }
        case FMI_IFTA_GZIP_ERROR:
            {
            packet.AppendFormat( _T( " Result Code:\t %d - GZip error\n\r" ), result_code );
            break;
            }
        case FMI_IFTA_GFS_OPEN_ERROR:
            {
            packet.AppendFormat( _T( " Result Code:\t %d - GFS Open error\n\r" ), result_code );
            break;
            }
        case FMI_IFTA_GFS_DIR_OPEN_ERROR:
            {
            packet.AppendFormat( _T( " Result Code:\t %d - GFS Dir Open error\n\r" ), result_code );
            break;
            }
        case FMI_IFTA_GFS_FSTAT_ERROR:
            {
            packet.AppendFormat( _T( " Result Code:\t %d - GFS FSTAT error\n\r" ), result_code );
            break;
            }
        case FMI_IFTA_GFS_READ_ERROR:
            {
            packet.AppendFormat( _T( " Result Code:\t %d - GFS Read error\n\r" ), result_code );
            break;
            }
        case FMI_IFTA_GFS_REMOVE_ERROR:
            {
            packet.AppendFormat( _T( " Result Code:\t %d - GFS Remove error\n\r" ), result_code );
            break;
            }
        case FMI_IFTA_EXPORT_DATA_ERROR:
            {
            packet.AppendFormat( _T( " Result Code:\t %d - Export Data error\n\r" ), result_code );
            break;
            }
        case FMI_IFTA_DATA_GFS_WRITE_ERROR:
            {
            packet.AppendFormat( _T( " Result Code:\t %d - GFS Write error\n\r" ), result_code );
            break;
            }
        case FMI_IFTA_TX_ERROR:
            {
            packet.AppendFormat( _T( " Result Code:\t %d - Transmission error\n\r" ), result_code );
            break;
            }
        }
    }
#endif

#if( FMI_SUPPORT_A602 )
//----------------------------------------------------------------------
//! \brief Initialize the map of FMI packet IDs to text strings
//! \since Protocol A602
//----------------------------------------------------------------------
void FmiLogParser::initFmiPacketNames()
{
    mFmiPacketNames.clear();

    mFmiPacketNames[ FMI_ID_ENABLE                           ] = _T("Enable");
    mFmiPacketNames[ FMI_ID_PRODUCT_ID_SUPPORT_RQST          ] = _T("Product ID and Support Request");
    mFmiPacketNames[ FMI_ID_PRODUCT_ID_DATA                  ] = _T("Product ID Data");
    mFmiPacketNames[ FMI_ID_PROTOCOL_DATA                    ] = _T("Protocol Data");
    mFmiPacketNames[ FMI_ID_TEXT_MSG_ACK                     ] = _T("Text Message ACK");
    mFmiPacketNames[ FMI_ID_SERVER_OPEN_TXT_MSG              ] = _T("Text Message Open From Server");
    mFmiPacketNames[ FMI_ID_SERVER_OK_ACK_TXT_MSG            ] = _T("Text Message Simple ACK");
    mFmiPacketNames[ FMI_ID_SERVER_YES_NO_CONFIRM_MSG        ] = _T("Text Message Yes/No");
    mFmiPacketNames[ FMI_ID_A602_STOP                        ] = _T("A602 Stop");

#if( FMI_SUPPORT_A603 )
    mFmiPacketNames[ FMI_ID_CLIENT_OPEN_TXT_MSG              ] = _T("Text Message Open from Client");
    mFmiPacketNames[ FMI_ID_CLIENT_TXT_MSG_RCPT              ] = _T("Text Message Receipt");
    mFmiPacketNames[ FMI_ID_A603_STOP                        ] = _T("A603 Stop");
    mFmiPacketNames[ FMI_ID_ETA_DATA_REQUEST                 ] = _T("ETA Data Request");
    mFmiPacketNames[ FMI_ID_ETA_DATA                         ] = _T("ETA Data");
    mFmiPacketNames[ FMI_ID_ETA_DATA_RCPT                    ] = _T("ETA Data Receipt");
    mFmiPacketNames[ FMI_ID_STOP_STATUS_REQUEST              ] = _T("Stop Status Update/Request");
    mFmiPacketNames[ FMI_ID_STOP_STATUS                      ] = _T("Stop Status");
    mFmiPacketNames[ FMI_ID_STOP_STATUS_RCPT                 ] = _T("Stop Status Receipt");
    mFmiPacketNames[ FMI_ID_AUTO_ARRIVAL                     ] = _T("Auto Arrival Change");
    mFmiPacketNames[ FMI_ID_DATA_DELETION                    ] = _T("Data Deletion");
#endif

#if( FMI_SUPPORT_A604 )
    mFmiPacketNames[ FMI_ID_UNICODE_REQUEST                  ] = _T("Unicode Request");
    mFmiPacketNames[ FMI_ID_UNICODE_RESPONSE                 ] = _T("Unicode Response");
    mFmiPacketNames[ FMI_ID_SET_CANNED_RESP_LIST             ] = _T("Set Canned Response List");
    mFmiPacketNames[ FMI_ID_CANNED_RESP_LIST_RCPT            ] = _T("Canned Response List Receipt");
    mFmiPacketNames[ FMI_ID_A604_OPEN_TEXT_MSG               ] = _T("A604 Open Text Message");
    mFmiPacketNames[ FMI_ID_A604_OPEN_TEXT_MSG_RCPT          ] = _T("A604 Open Text Message Receipt");
    mFmiPacketNames[ FMI_ID_TEXT_MSG_ACK_RCPT                ] = _T("Text Message ACK Receipt");
    mFmiPacketNames[ FMI_ID_SET_CANNED_RESPONSE              ] = _T("Set Canned Response");
    mFmiPacketNames[ FMI_ID_DELETE_CANNED_RESPONSE           ] = _T("Delete Canned Response");
    mFmiPacketNames[ FMI_ID_SET_CANNED_RESPONSE_RCPT         ] = _T("Set Canned Response Receipt");
    mFmiPacketNames[ FMI_ID_DELETE_CANNED_RESPONSE_RCPT      ] = _T("Delete Canned Response Receipt");
    mFmiPacketNames[ FMI_ID_REFRESH_CANNED_RESP_LIST         ] = _T("Request Canned Response List Refresh");
    mFmiPacketNames[ FMI_ID_TEXT_MSG_STATUS_REQUEST          ] = _T("Text Message Status Request");
    mFmiPacketNames[ FMI_ID_TEXT_MSG_STATUS                  ] = _T("Text Message Status");
    mFmiPacketNames[ FMI_ID_SET_CANNED_MSG                   ] = _T("Set Canned Message");
    mFmiPacketNames[ FMI_ID_SET_CANNED_MSG_RCPT              ] = _T("Set Canned Message Receipt");
    mFmiPacketNames[ FMI_ID_DELETE_CANNED_MSG                ] = _T("Delete Canned Message ");
    mFmiPacketNames[ FMI_ID_DELETE_CANNED_MSG_RCPT           ] = _T("Delete Canned Message Receipt");
    mFmiPacketNames[ FMI_ID_REFRESH_CANNED_MSG_LIST          ] = _T("Refresh Canned Message List");
    mFmiPacketNames[ FMI_ID_SORT_STOP_LIST                   ] = _T("Sort Stop List");
    mFmiPacketNames[ FMI_ID_SORT_STOP_LIST_ACK               ] = _T("Sort Stop List ACK");
    mFmiPacketNames[ FMI_ID_USER_INTERFACE_TEXT              ] = _T("User Interface Text Change");
    mFmiPacketNames[ FMI_ID_USER_INTERFACE_TEXT_RCPT         ] = _T("User Interface Text Receipt");
    mFmiPacketNames[ FMI_ID_MSG_THROTTLING_COMMAND           ] = _T("Message Throttling Command");
    mFmiPacketNames[ FMI_ID_MSG_THROTTLING_RESPONSE          ] = _T("Message Throttling Response");
    mFmiPacketNames[ FMI_ID_PING                             ] = _T("Ping Request");
    mFmiPacketNames[ FMI_ID_PING_RESPONSE                    ] = _T("Ping Response");
    mFmiPacketNames[ FMI_ID_FILE_TRANSFER_START              ] = _T("File Transfer Start");
    mFmiPacketNames[ FMI_ID_FILE_DATA_PACKET                 ] = _T("File Data Packet");
    mFmiPacketNames[ FMI_ID_FILE_TRANSFER_END                ] = _T("File Transfer End");
    mFmiPacketNames[ FMI_ID_FILE_START_RCPT                  ] = _T("File Start Receipt");
    mFmiPacketNames[ FMI_ID_FILE_PACKET_RCPT                 ] = _T("Packet Receipt");
    mFmiPacketNames[ FMI_ID_FILE_END_RCPT                    ] = _T("File End Receipt");
    mFmiPacketNames[ FMI_ID_GPI_FILE_INFORMATION_REQUEST     ] = _T("GPI File Information Request");
    mFmiPacketNames[ FMI_ID_GPI_FILE_INFORMATION             ] = _T("GPI File Information");
    mFmiPacketNames[ FMI_ID_SET_DRIVER_STATUS_LIST_ITEM      ] = _T("Set Driver Status List Item");
    mFmiPacketNames[ FMI_ID_DELETE_DRIVER_STATUS_LIST_ITEM   ] = _T("Delete Driver Status List Item");
    mFmiPacketNames[ FMI_ID_SET_DRIVER_STATUS_LIST_ITEM_RCPT ] = _T("Set Driver Status List Item Receipt");
    mFmiPacketNames[ FMI_ID_DEL_DRIVER_STATUS_LIST_ITEM_RCPT ] = _T("Delete Driver Status List Item Receipt");
    mFmiPacketNames[ FMI_ID_DRIVER_STATUS_LIST_REFRESH       ] = _T("Driver Status List Refresh");
    mFmiPacketNames[ FMI_ID_DRIVER_ID_REQUEST                ] = _T("Request Driver ID");
    mFmiPacketNames[ FMI_ID_DRIVER_ID_UPDATE                 ] = _T("Driver ID Update");
    mFmiPacketNames[ FMI_ID_DRIVER_ID_RCPT                   ] = _T("Driver ID Receipt");
    mFmiPacketNames[ FMI_ID_DRIVER_STATUS_REQUEST            ] = _T("Request Driver Status");
    mFmiPacketNames[ FMI_ID_DRIVER_STATUS_UPDATE             ] = _T("Driver Status Update");
    mFmiPacketNames[ FMI_ID_DRIVER_STATUS_RCPT               ] = _T("Driver Status Receipt");
#endif

#if( FMI_SUPPORT_A605 )
    mFmiPacketNames[ FMI_ID_MSG_THROTTLING_QUERY             ] = _T("Message Throttling Query");
    mFmiPacketNames[ FMI_ID_MSG_THROTTLING_QUERY_RESPONSE    ] = _T("Message Throttling Query Response");
#endif

#if (FMI_SUPPORT_A606)
    mFmiPacketNames[ FMI_SAFE_MODE                           ] = _T("Set Safe Mode Speed");
    mFmiPacketNames[ FMI_SAFE_MODE_RESP                      ] = _T("Set Safe Mode Speed Response");
#endif

#if( FMI_SUPPORT_A607 )
    mFmiPacketNames[ FMI_ID_A607_CLIENT_OPEN_TXT_MSG         ] = _T("A607 Client to Server Text Message");
    mFmiPacketNames[ FMI_ID_WAYPOINT                         ] = _T("Create Waypoint");
    mFmiPacketNames[ FMI_ID_WAYPOINT_RCPT                    ] = _T("Create Waypoint Receipt");
    mFmiPacketNames[ FMI_ID_WAYPOINT_DELETE                  ] = _T("Delete Waypoint");
    mFmiPacketNames[ FMI_ID_WAYPOINT_DELETED                 ] = _T("Waypoint Deleted");
    mFmiPacketNames[ FMI_ID_WAYPOINT_DELETED_RCPT            ] = _T("Waypoint Deleted Receipt");
    mFmiPacketNames[ FMI_ID_DELETE_WAYPOINT_CAT              ] = _T("Delete Waypoints by Category");
    mFmiPacketNames[ FMI_ID_DELETE_WAYPOINT_CAT_RCPT         ] = _T("Delete Waypoints by Category Receipt");
    mFmiPacketNames[ FMI_ID_CREATE_WAYPOINT_CAT              ] = _T("Create Waypoint Category");
    mFmiPacketNames[ FMI_ID_CREATE_WAYPOINT_CAT_RCPT         ] = _T("Create Waypoint Category Receipt");
    mFmiPacketNames[ FMI_ID_TEXT_MSG_DELETE_REQUEST          ] = _T("Text Message Delete Request");
    mFmiPacketNames[ FMI_ID_TEXT_MSG_DELETE_RESPONSE         ] = _T("Text Message Delete Response");
    mFmiPacketNames[ FMI_ID_DRIVER_ID_UPDATE_D607            ] = _T("Driver ID Update D607");
    mFmiPacketNames[ FMI_ID_DRIVER_STATUS_UPDATE_D607        ] = _T("Driver Status Update D607");
#endif

#if( FMI_SUPPORT_A608 )
    mFmiPacketNames[ FMI_SPEED_LIMIT_SET                     ] = _T("Set Speed Limit Alerts");
    mFmiPacketNames[ FMI_SPEED_LIMIT_RCPT                    ] = _T("Set Speed Limit Alerts Receipt");
    mFmiPacketNames[ FMI_SPEED_LIMIT_ALERT                   ] = _T("Speed Limit Alerts speeding alert");
    mFmiPacketNames[ FMI_SPEED_LIMIT_ALERT_RCPT              ] = _T("Speed Limit Alerts speeding alert Receipt");
#endif

#if( FMI_SUPPORT_A609 )
    mFmiPacketNames[ FMI_REBOOT_DEVICE_REQUEST               ] = _T("Remote Reboot Request");
#endif

#if( FMI_SUPPORT_A610 )
    mFmiPacketNames[ FMI_SET_ODOMETER_REQUEST                ] = _T("AOBRD Set Odometer Request");
    mFmiPacketNames[ FMI_DRIVER_LOGIN_REQUEST                ] = _T("AOBRD Driver Login Request");
    mFmiPacketNames[ FMI_DRIVER_LOGIN_RESPONSE               ] = _T("AOBRD Driver Login Response");
    mFmiPacketNames[ FMI_DRIVER_PROFILE_DOWNLOAD_REQUEST     ] = _T("AOBRD Driver Profile Download Request");
    mFmiPacketNames[ FMI_DRIVER_PROFILE_DOWNLOAD_RESPONSE    ] = _T("AOBRD Driver Profile Download Response");
    mFmiPacketNames[ FMI_DRIVER_PROFILE_UPDATE               ] = _T("AOBRD Driver Profile Update");
    mFmiPacketNames[ FMI_DRIVER_PROFILE_UPDATE_RESPONSE      ] = _T("AOBRD Driver Profile Update Response");
    mFmiPacketNames[ FMI_DRIVER_STATUS_UPDATE_REQUEST        ] = _T("AOBRD Driver Status Update Request");
    mFmiPacketNames[ FMI_DRIVER_STATUS_UPDATE_RESPONSE       ] = _T("AOBRD Driver Status Update Response");
    mFmiPacketNames[ FMI_DRIVER_STATUS_UPDATE_RECEIPT        ] = _T("AOBRD Driver Status Update Receipt");
    mFmiPacketNames[ FMI_DOWNLOAD_SHIPMENTS_REQUEST          ] = _T("AOBRD Download Shipments Request");
    mFmiPacketNames[ FMI_SHIPMENT_DOWNLOAD_RESPONSE          ] = _T("AOBRD Shipment Download Response");
    mFmiPacketNames[ FMI_SHIPMENT_DOWNLOAD_RECEIPT           ] = _T("AOBRD Shipment Download Receipt");
    mFmiPacketNames[ FMI_ANNOTATION_DOWNLOAD_REQUEST         ] = _T("AOBRD Annotation Download Request");
    mFmiPacketNames[ FMI_ANNOTATION_DOWNLOAD_RESPONSE        ] = _T("AOBRD Annotation Download Response");
    mFmiPacketNames[ FMI_ANNOTATION_DOWNLOAD_RECEIPT         ] = _T("AOBRD Annotation Download Receipt");
#endif

#if( FMI_SUPPORT_A611 )
    mFmiPacketNames[ FMI_ID_LONG_TEXT_MSG                    ] = _T("A611 Long Text Message");
    mFmiPacketNames[ FMI_ID_LONG_TEXT_MSG_RCPT               ] = _T("A611 Long Text Message Receipt");
#endif

#if( FMI_SUPPORT_A612 )
    mFmiPacketNames[ FMI_CUSTOM_FORM_DEL_REQUEST             ] = _T("A612 Delete Form");
    mFmiPacketNames[ FMI_CUSTOM_FORM_DEL_RECEIPT             ] = _T("A612 Delete Form Receipt");
    mFmiPacketNames[ FMI_CUSTOM_FORM_MOVE_REQUEST            ] = _T("A612 Move Form");
    mFmiPacketNames[ FMI_CUSTOM_FORM_MOVE_RECEIPT            ] = _T("A612 Move Form Receipt");
    mFmiPacketNames[ FMI_CUSTOM_FORM_GET_POS_REQUEST         ] = _T("A612 Request Position of Form");
    mFmiPacketNames[ FMI_CUSTOM_FORM_GET_POS_RECEIPT         ] = _T("A612 Request Position of Form Receipt");
#endif
#if( FMI_SUPPORT_A614 )
    mFmiPacketNames[ FMI_STOP_CALC_ACK_REQUEST               ] = _T("A614 Stop Route Calculation ACK");
    mFmiPacketNames[ FMI_STOP_CALC_ACK_RECEIPT               ] = _T("A614 Stop Route Calculation ACK Receipt");
#endif
#if( FMI_SUPPORT_A613 )
    mFmiPacketNames[ FMI_CUSTOM_AVOID_ENABLE_FEATURE_REQUEST ] = _T("A613 Enable Custom Avoidances Feature Request");
    mFmiPacketNames[ FMI_CUSTOM_AVOID_ENABLE_FEATURE_RECEIPT ] = _T("A613 Enable Custom Avoidances Feature Receipt");
    mFmiPacketNames[ FMI_CUSTOM_AVOID_ADD_REQUEST            ] = _T("A613 Add Custom Avoidance");
    mFmiPacketNames[ FMI_CUSTOM_AVOID_ADD_RECEIPT            ] = _T("A613 Add Custom Avoidance Receipt");
    mFmiPacketNames[ FMI_CUSTOM_AVOID_DEL_REQUEST            ] = _T("A613 Delete Custom Avoidance");
    mFmiPacketNames[ FMI_CUSTOM_AVOID_DEL_RECEIPT            ] = _T("A613 Delete Custom Avoidance Receipt");
    mFmiPacketNames[ FMI_CUSTOM_AVOID_TOGGLE_REQUEST         ] = _T("A613 Toggle Custom Avoidance");
    mFmiPacketNames[ FMI_CUSTOM_AVOID_TOGGLE_RECEIPT         ] = _T("A613 Toggle Custom Avoidance Receipt");
#endif
#if( FMI_SUPPORT_A615 )
    mFmiPacketNames[ FMI_IFTA_DATA_FETCH_REQUEST             ] = _T("A615 IFTA Data Fetch Request");
    mFmiPacketNames[ FMI_IFTA_DATA_FETCH_RECEIPT             ] = _T("A615 IFTA Data Fetch Receipt");
    mFmiPacketNames[ FMI_IFTA_DATA_DELETE_REQUEST            ] = _T("A615 IFTA Data Delete Request");
    mFmiPacketNames[ FMI_IFTA_DATA_DELETE_RECEIPT            ] = _T("A615 IFTA Data Delete Receipt");
    mFmiPacketNames[ FMI_AOBRD_DRIVER_LOGOFF_REQUEST         ] = _T("A615 AOBRD Remote Logoff Request");
    mFmiPacketNames[ FMI_AOBRD_DRIVER_LOGOFF_RECEIPT         ] = _T("A615 AOBRD Remote Logoff Receipt");
    mFmiPacketNames[ FMI_DRIVER_PROFILE_UPDATE_V2            ] = _T("A615 AOBRD Driver Profile Update V2");
    mFmiPacketNames[ FMI_DRIVER_PROFILE_DOWNLOAD_RESPONSE_V2 ] = _T("A615 AOBRD Driver Profile Download Response V2");
    mFmiPacketNames[ FMI_HOS_AUTO_STATUS_FEATURE_REQUEST     ] = _T("A615 HOS Auto Status Feature Request");
    mFmiPacketNames[ FMI_HOS_AUTO_STATUS_FEATURE_RECEIPT     ] = _T("A615 HOS Auto Status Feature Receipt");
    mFmiPacketNames[ FMI_HOS_8_HOUR_RULE_ENABLE_REQUEST      ] = _T("A615 HOS 8 Hour Rule Enable Request");
    mFmiPacketNames[ FMI_HOS_8_HOUR_RULE_ENABLE_RECEIPT      ] = _T("A615 HOS 8 Hour Rule Enable Receipt");
    mHosartPacketNames[ HOSART_CHANGE_STATUS                 ] = _T("HOSART CHANGE STATUS");
    mHosartPacketNames[ HOSART_CHANGE_TO_DRIVING_WARNING     ] = _T("HOSART CHANGE TO DRIVING WARNING");
    mHosartPacketNames[ HOSART_WARNING_FOUND                 ] = _T("HOSART WARNING FOUND");
    mHosartPacketNames[ HOSART_VIOLATION_FOUND               ] = _T("HOSART VIOLATION FOUND");
    mHosartPacketNames[ HOSART_SET_PS_TO_OFF                 ] = _T("HOSART SET PS TO OFF");
#endif
#if( FMI_SUPPORT_A616 )
    mFmiPacketNames[ FMI_SET_BAUD_REQUEST                    ] = _T("A616 Set Baud Request");
    mFmiPacketNames[ FMI_SET_BAUD_RECEIPT                    ] = _T("A616 Set Baud Receipt");
#endif
#if( FMI_SUPPORT_A617 )
    mFmiPacketNames[ FMI_ALERT_POPUP_REQUEST                 ] = _T("A617 Create Alert Popup Request");
    mFmiPacketNames[ FMI_ALERT_POPUP_RECEIPT                 ] = _T("A617 Create Alert Popup Receipt");
    mFmiPacketNames[ FMI_SENSOR_CONFIG_REQUEST               ] = _T("A617 Config Sensor Request");
    mFmiPacketNames[ FMI_SENSOR_CONFIG_RECEIPT               ] = _T("A617 Config Sensor Receipt");
    mFmiPacketNames[ FMI_SENSOR_DELETE_REQUEST               ] = _T("A617 Delete Sensor Request");
    mFmiPacketNames[ FMI_SENSOR_DELETE_RECEIPT               ] = _T("A617 Delete Sensor Receipt");
    mFmiPacketNames[ FMI_SENSOR_UPDATE_REQUEST               ] = _T("A617 Update Sensor Request");
    mFmiPacketNames[ FMI_SENSOR_QUERY_DISPLAY_INDEX_REQUEST  ] = _T("A617 Query Sensor Display Index Request");
    mFmiPacketNames[ FMI_SENSOR_QUERY_DISPLAY_INDEX_RECEIPT  ] = _T("A617 Query Sensor Display Index Receipt");
#endif
#if( FMI_SUPPORT_A619 )
    mFmiPacketNames[ FMI_HOS_SET_SETTING_DATA_REQUEST        ] = _T("A619 HOS Set Setting Data Request");
    mFmiPacketNames[ FMI_HOS_SET_SETTING_DATA_RECEIPT        ] = _T("A619 HOS Set Setting Data Receipt");
#endif
}
#endif

#if( FMI_SUPPORT_A607 )
//----------------------------------------------------------------------
//! \brief Initialize the map of FMI packet IDs to text strings
//! \since Protocol A602
//----------------------------------------------------------------------
void FmiLogParser::initFmiFeatureNames()
{
    mFmiFeatureNames.clear();

    mFmiFeatureNames[ FEATURE_ID_UNICODE          ] = _T("Unicode");
    mFmiFeatureNames[ FEATURE_ID_A607_SUPPORT     ] = _T("A607 Support");
    mFmiFeatureNames[ FEATURE_ID_DRIVER_PASSWORDS ] = _T("Driver Password Support");
    mFmiFeatureNames[ FEATURE_ID_MULTIPLE_DRIVERS ] = _T("Multiple Drivers");
    mFmiFeatureNames[ FEATURE_ID_AOBRD_SUPPORT    ] = _T("AOBRD Support");
}
#endif

//----------------------------------------------------------------------
//! \brief Initialize the map of Garmin packet IDs to text strings
//----------------------------------------------------------------------
void FmiLogParser::initGarminPacketNames()
{
    mGarminPacketNames.clear();
    mGarminPacketNames[ ID_ACK_BYTE        ] = _T("ACK");
    mGarminPacketNames[ ID_NAK_BYTE        ] = _T("NAK");
    mGarminPacketNames[ ID_PVT_DATA        ] = _T("PVT Data");
    mGarminPacketNames[ ID_UNIT_ID         ] = _T("Unit ID Data");
    mGarminPacketNames[ ID_COMMAND_BYTE    ] = _T("Command Packet");
    mGarminPacketNames[ ID_PRODUCT_DATA    ] = _T("Legacy Product Data");
    mGarminPacketNames[ ID_PRODUCT_RQST    ] = _T("Legacy Product Request");
    mGarminPacketNames[ ID_PROTOCOL_ARRAY  ] = _T("Legacy Protocol Array");

#if( FMI_SUPPORT_LEGACY )
    mGarminPacketNames[ ID_LEGACY_STOP_MSG ] = _T("Legacy Stop Message");
    mGarminPacketNames[ ID_LEGACY_TEXT_MSG ] = _T("Legacy Text Message");
#endif

#if( FMI_SUPPORT_A602 )
    mGarminPacketNames[ ID_FMI_PACKET      ] = _T("FMI Packet");
#endif

#if( FMI_SUPPORT_A615 )
    mGarminPacketNames[ ID_HOSART_PACKET   ] = _T("HOSART Packet");
    mGarminPacketNames[ ID_SET_TIME_PACKET ] = _T("Set Time Packet");
#endif
}

//----------------------------------------------------------------------
//! \brief Initialize the map of Garmin command IDs to text strings
//----------------------------------------------------------------------
void FmiLogParser::initGarminCommandNames()
{
    mGarminCommandNames.clear();
    mGarminCommandNames[ COMMAND_REQ_DATE_TIME     ] = _T("Request Date/Time");
    mGarminCommandNames[ COMMAND_REQ_UNIT_ID       ] = _T("Request Unit ID (ESN)");
    mGarminCommandNames[ COMMAND_TURN_ON_PVT_DATA  ] = _T("Turn On PVT Data");
    mGarminCommandNames[ COMMAND_TURN_OFF_PVT_DATA ] = _T("Turn Off PVT Data");
}

//----------------------------------------------------------------------
//! \brief Resend a packet to the client
//! \param aLineNumber The line number of tha packet to send
//----------------------------------------------------------------------
void FmiLogParser::resendPacket
    (
    int aLineNumber
    )
{
    int filenameLength = WideCharToMultiByte( CP_ACP, 0, mLogFilename, -1, NULL, 0, NULL, NULL );
    char *filenameAnsi = new char[filenameLength];
    WideCharToMultiByte( CP_ACP, 0, mLogFilename, -1, filenameAnsi, filenameLength, NULL, NULL );
    ifstream logFile( filenameAnsi, ios_base::in );
    delete[] filenameAnsi;

    if( logFile.good() )
    {
        std::string lineString;
        const char* line;
        uint8       packet[MAX_PACKET_SIZE];
        id_type     packetId;
        uint8       payloadSize;
        uint8       payload[MAX_PAYLOAD_SIZE];

        int i = 0;
        logFile.seekg( mLineOffset[aLineNumber] );
        getline( logFile, lineString );
        line = lineString.c_str();

        while( line[i++] != '-' ); //skip time

        UTIL_hex_to_uint8( line + i, packet, MAX_PACKET_SIZE );    //transform to uint8

        i = 1;                    // point past DLE that starts the frame
        packetId = packet[i++];
        payloadSize = packet[i++];
        if( payloadSize == ID_DLE_BYTE )
        {
            i++; //size may be DLE stuffed
        }

        int payloadIdx = 0;
        for( payloadIdx = 0; payloadIdx < payloadSize; payloadIdx++ )
        {
            payload[payloadIdx] = packet[i++];
            if( payload[payloadIdx] == ID_DLE_BYTE )
            {
                i++;
            }
        }
        GarminTransportLayer::getInstance()->tx( new GarminPacket( packetId, payload, payloadSize ), FALSE );
    }
}
