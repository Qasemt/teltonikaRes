/*********************************************************************
*
*   MODULE NAME:
*       AobrdShipment.cpp
*
*   Copyright 2012 by Garmin Ltd. or its subsidiaries.
*---------------------------------------------------------------------
* $NoKeywords$
*********************************************************************/

#include "AobrdShipment.h"

using namespace std;

//----------------------------------------------------------------------
//! \brief Constructor
//----------------------------------------------------------------------
AobrdShipment::AobrdShipment()
    : mServerTimestamp(0),
      mServerStartTime(0),
      mServerEndTime(0)
{

}

//----------------------------------------------------------------------
//! \brief Destructor
//----------------------------------------------------------------------
AobrdShipment::~AobrdShipment()
{
}

//----------------------------------------------------------------------
//! \brief Get item's server timestamp
//! \details Get the server timestamp of this item
//! \return The item's server timestamp
//----------------------------------------------------------------------
time_type AobrdShipment::getServerTimestamp() const
{
    return mServerTimestamp;
}

//----------------------------------------------------------------------
//! \brief Get item's server start time
//! \details Get the server start time of this item
//! \return The item's server start time
//----------------------------------------------------------------------
time_type AobrdShipment::getServerStartTime() const
{
    return mServerStartTime;
}

//----------------------------------------------------------------------
//! \brief Get item's server end time
//! \details Get the server end time of this item
//! \return The item's server end time
//----------------------------------------------------------------------
time_type AobrdShipment::getServerEndTime() const
{
    return mServerEndTime;
}

//----------------------------------------------------------------------
//! \brief Get item's shipper name
//! \details Get the shipper name of this item
//! \return The item's shipper name
//----------------------------------------------------------------------
CString AobrdShipment::getShipperName() const
{
    CString shipperName;
    TCHAR tcharName[50];
    MultiByteToWideChar( CP_UTF8, 0, mShipperName.c_str(), -1, tcharName, 50 );
    shipperName.SetString( tcharName );

    return shipperName;
}

//----------------------------------------------------------------------
//! \brief Get item's document number
//! \details Get the document number of this item
//! \return The item's document number
//----------------------------------------------------------------------
CString AobrdShipment::getDocNumber() const
{
    CString docNumber;
    TCHAR tcharName[50];
    MultiByteToWideChar( CP_UTF8, 0, mDocNumber.c_str(), -1, tcharName, 50 );
    docNumber.SetString( tcharName );

    return docNumber;
}

//----------------------------------------------------------------------
//! \brief Get item's commodity
//! \details Get the commodity of this item
//! \return The item's commodity
//----------------------------------------------------------------------
CString AobrdShipment::getCommodity() const
{
    CString commodity;
    TCHAR tcharName[50];
    MultiByteToWideChar( CP_UTF8, 0, mCommodity.c_str(), -1, tcharName, 50 );
    commodity.SetString( tcharName );

    return commodity;
}

//----------------------------------------------------------------------
//! \brief Read a AobrdShipment from an input stream
//! \details Read values from the input stream, and
//!     updates the member variables appropriately.
//! \param aStream The stream to read from
//----------------------------------------------------------------------
void AobrdShipment::readFromStream
    (
    std::istream &aStream
    )
{
    char temp;

    aStream >> mServerTimestamp;
    aStream.get();    // consume the separator

    aStream >> mServerStartTime;
    aStream.get();    // consume the separator

    aStream >> mServerEndTime;
    aStream.get();    // consume the separator

    mShipperName.clear();
    while( aStream.peek() != '\0' && !aStream.eof() )
    {
        aStream.read( &temp, 1 );
        mShipperName.append( 1, temp );
    }

    aStream.get();    // consume the '\0' inserted between shipper name and doc number

    mDocNumber.clear();
    while( aStream.peek() != '\0' && !aStream.eof() )
    {
        aStream.read( &temp, 1 );
        mDocNumber.append( 1, temp );
    }

    aStream.get();    // consume the '\0' inserted between doc number and commodity

    mCommodity.clear();
    while( aStream.peek() != '\0' && !aStream.eof() )
    {
        aStream.read( &temp, 1 );
        mCommodity.append( 1, temp );
    }

    aStream.get();    // consume the '\0'
    aStream.get();    // consume the '\n'

    if( !aStream.eof() )
    {
        //mIsValid = TRUE;
    }
}

//----------------------------------------------------------------------
//! \brief Set the Shipper Name for this item
//! \details Sets the Shipper Name for this item
//! \param aShipperName The shipper's name
//----------------------------------------------------------------------
void AobrdShipment::setShipperName( const string & aShipperName )
{
    mShipperName = aShipperName;
}

//----------------------------------------------------------------------
//! \brief Set the Document Number for this item
//! \details Sets the Document Number for this item
//! \param aDocNumber The document number
//----------------------------------------------------------------------
void AobrdShipment::setDocNumber( const string & aDocNumber )
{
    mDocNumber = aDocNumber;
}

//----------------------------------------------------------------------
//! \brief Set the Commodity for this item
//! \details Sets the Commodity for this item
//! \param aCommodity The commodity
//----------------------------------------------------------------------
void AobrdShipment::setCommodity( const string & aCommodity )
{
    mCommodity = aCommodity;
}

//----------------------------------------------------------------------
//! \brief Set the Server Timestamp for this item
//! \details Sets the Server Timestamp for this item
//! \param aTimestamp The server timestamp
//----------------------------------------------------------------------
void AobrdShipment::setServerTimestamp( const time_type & aTimestamp )
{
    mServerTimestamp = aTimestamp;
}

//----------------------------------------------------------------------
//! \brief Set the Server Start Time for this item
//! \details Sets the Server Start Time for this item
//! \param aTimestamp The server start time
//----------------------------------------------------------------------
void AobrdShipment::setServerStartTime( const time_type & aTimestamp )
{
    mServerStartTime = aTimestamp;
}

//----------------------------------------------------------------------
//! \brief Set the Server End Time for this item
//! \details Sets the Server End Time for this item
//! \param aTimestamp The server end time
//----------------------------------------------------------------------
void AobrdShipment::setServerEndTime( const time_type & aTimestamp )
{
    mServerEndTime = aTimestamp;
}

//----------------------------------------------------------------------
//! \brief Write a AobrdShipment to an output stream
//! \details Append values to the output stream.
//! \param aStream The stream to write to
//----------------------------------------------------------------------
void AobrdShipment::writeToStream
    (
    std::ofstream &aStream
    ) const
{
    aStream << mServerTimestamp << '\0' << mServerStartTime << '\0' << mServerEndTime << '\0'
            << mShipperName << '\0' << mDocNumber << '\0' << mCommodity << '\0' << '\n';
}