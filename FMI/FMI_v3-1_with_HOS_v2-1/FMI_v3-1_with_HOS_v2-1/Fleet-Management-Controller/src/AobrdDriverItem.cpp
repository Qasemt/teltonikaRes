/*********************************************************************
*
*   MODULE NAME:
*       AobrdDriverItem.cpp
*
*   Copyright 2012 by Garmin Ltd. or its subsidiaries.
*---------------------------------------------------------------------
* $NoKeywords$
*********************************************************************/

#include "stdafx.h"
#include "AobrdDriverItem.h"

#define CURRENT_VERSION     1
#define RECORD_SEPARATOR    30

//----------------------------------------------------------------------
//! \brief Constructor
//----------------------------------------------------------------------
AobrdDriverItem::AobrdDriverItem()
    : mParent( NULL ),
      mLongTermRuleset(0),
#if( FMI_SUPPORT_A615 )
      mLoadTypeRuleset(0),
#endif
      mTimeZone(0)
{

}

//----------------------------------------------------------------------
//! \brief Destructor
//----------------------------------------------------------------------
AobrdDriverItem::~AobrdDriverItem()
{

}

//----------------------------------------------------------------------
//! \brief Get the driver ID as a CString
//! \return the driver ID, as a CString
//----------------------------------------------------------------------
CString AobrdDriverItem::getDriverId() const
{
    CString driverId;
    TCHAR tcharName[50];
    MultiByteToWideChar( CP_UTF8, 0, mDriverId.c_str(), -1, tcharName, 50 );
    driverId.SetString( tcharName );

    return driverId;
}

//----------------------------------------------------------------------
//! \brief Get item's ID
//! \details Get the unique ID (driver ID) of this item
//! \return The item's ID
//----------------------------------------------------------------------
const AobrdDriverItem::key_type & AobrdDriverItem::getId() const
{
    return mDriverId;
}

//----------------------------------------------------------------------
//! \brief Get item's password
//! \details Get the password of this item
//! \return The item's password
//----------------------------------------------------------------------
CString AobrdDriverItem::getPassword() const
{
    CString driverPassword;
    TCHAR tcharName[50];
    MultiByteToWideChar( CP_UTF8, 0, mDriverPassword.c_str(), -1, tcharName, 50 );
    driverPassword.SetString( tcharName );

    return driverPassword;
}

//----------------------------------------------------------------------
//! \brief Get item's first name
//! \details Get the first name of this item
//! \return The item's first name
//----------------------------------------------------------------------
CString AobrdDriverItem::getFirstName() const
{
    CString value;
    TCHAR tcharValue[50];
    MultiByteToWideChar( CP_UTF8, 0, mFirstName.c_str(), -1, tcharValue, 50 );
    value.SetString( tcharValue );

    return value;
}

//----------------------------------------------------------------------
//! \brief Get item's last name
//! \details Get the last name of this item
//! \return The item's last name
//----------------------------------------------------------------------
CString AobrdDriverItem::getLastName() const
{
    CString value;
    TCHAR tcharValue[50];
    MultiByteToWideChar( CP_UTF8, 0, mLastName.c_str(), -1, tcharValue, 50 );
    value.SetString( tcharValue );

    return value;
}

//----------------------------------------------------------------------
//! \brief Get item's carrier name
//! \details Get the carrier name of this item
//! \return The item's carrier name
//----------------------------------------------------------------------
CString AobrdDriverItem::getCarrierName() const
{
    CString value;
    TCHAR tcharValue[50];
    MultiByteToWideChar( CP_UTF8, 0, mCarrierName.c_str(), -1, tcharValue, 50 );
    value.SetString( tcharValue );

    return value;
}

//----------------------------------------------------------------------
//! \brief Get item's carrier ID
//! \details Get the carrier ID of this item
//! \return The item's carrier ID
//----------------------------------------------------------------------
CString AobrdDriverItem::getCarrierID() const
{
    CString value;
    TCHAR tcharValue[50];
    MultiByteToWideChar( CP_UTF8, 0, mCarrierID.c_str(), -1, tcharValue, 50 );
    value.SetString( tcharValue );

    return value;
}

//----------------------------------------------------------------------
//! \brief Get item's long term ruleset
//! \details Get the long term ruleset of this item
//! \return The item's long term ruleset
//----------------------------------------------------------------------
uint16 AobrdDriverItem::getLongTermRuleset() const
{
    return mLongTermRuleset;
}

//----------------------------------------------------------------------
//! \brief Get item's time zone
//! \details Get the time zone of this item
//! \return The item's time zone
//----------------------------------------------------------------------
uint16 AobrdDriverItem::getTimeZone() const
{
    return mTimeZone;
}

//----------------------------------------------------------------------
//! \brief Get the list of shipments
//! \details Get the shipments for this item
//! \return The item's shipments
//----------------------------------------------------------------------
const std::vector<AobrdShipment> & AobrdDriverItem::getShipments() const
{
    return mShipments;
}

//----------------------------------------------------------------------
//! \brief Add a shipment
//! \details Adds a shipment to the list of shipments
//! \param shipment The shipment to add
//----------------------------------------------------------------------
void AobrdDriverItem::addShipment( AobrdShipment & shipment )
{
    mShipments.push_back( shipment );
}

//----------------------------------------------------------------------
//! \brief Update a shipment
//! \details Replaces a shipment at existing location with a new one
//! \param shipment The new shipment
//! \param index The index of the existing shipment to replace
//----------------------------------------------------------------------
void AobrdDriverItem::replaceShipment( AobrdShipment & shipment, int index )
{
    deleteShipment( index );
    mShipments.insert( mShipments.begin()+index, shipment );
}

//----------------------------------------------------------------------
//! \brief Delete a shipment
//! \details Removes a shipment from the list of shipments
//! \param index The index of the shipment to remove
//----------------------------------------------------------------------
void AobrdDriverItem::deleteShipment( int index )
{
    mShipments.erase( mShipments.begin()+index );
}

//----------------------------------------------------------------------
//! \brief Read a AobrdDriverItem from an input stream
//! \details Read id and password from the input stream, and
//!     updates the member variables appropriately.
//! \param aStream The stream to read from
//----------------------------------------------------------------------
void AobrdDriverItem::readFromStream
    (
    std::istream &aStream
    )
{
#if( FMI_SUPPORT_A615 )
    uint16 version = 0;

    // see if the record is of the newer format
    // older formats shouldn't have the next character be anything less than a 32
    if ( RECORD_SEPARATOR == aStream.peek() )
        {
        aStream.get();  //read record separator
        version = aStream.get();
        }
#endif

    char temp;

    mDriverId.clear();
    while( aStream.peek() != '\0' && !aStream.eof() )
    {
        aStream.read( &temp, 1 );
        mDriverId.append( 1, temp );
    }

    aStream.get();    // consume the '\0' inserted between mDriverId and mPassword

    mDriverPassword.clear();
    while( aStream.peek() != '\0' && !aStream.eof() )
    {
        aStream.read( &temp, 1 );
        mDriverPassword.append( 1, temp );
    }

    aStream.get();    // consume the '\0' inserted after mPassword

    mFirstName.clear();
    while( aStream.peek() != '\0' && !aStream.eof() )
    {
        aStream.read( &temp, 1 );
        mFirstName.append( 1, temp );
    }

    aStream.get();    // consume the '\0' inserted after first name

    mLastName.clear();
    while( aStream.peek() != '\0' && !aStream.eof() )
    {
        aStream.read( &temp, 1 );
        mLastName.append( 1, temp );
    }

    aStream.get();    // consume the '\0' inserted after last name

    mCarrierName.clear();
    while( aStream.peek() != '\0' && !aStream.eof() )
    {
        aStream.read( &temp, 1 );
        mCarrierName.append( 1, temp );
    }

    aStream.get();    // consume the '\0' inserted after carrier name

    mCarrierID.clear();
    while( aStream.peek() != '\0' && !aStream.eof() )
    {
        aStream.read( &temp, 1 );
        mCarrierID.append( 1, temp );
    }

    aStream.get();    // consume the separator

    //read long term ruleset

    aStream >> mLongTermRuleset;
    aStream.get();    // consume the separator

#if( FMI_SUPPORT_A615 )
    if (version > 0 )
        {
        //read load type ruleset

        aStream >> mLoadTypeRuleset;
        aStream.get();    // consume the separator
        }
    else {
        mLoadTypeRuleset = 0;
        }
#endif

    //read time zone

    aStream >> mTimeZone;

    aStream.get();    // consume the '\0'

    //read shipment count and shipments
    uint32 shipmentCount = aStream.get();

    for( uint32 i=0; i<shipmentCount; i++ )
        {
        AobrdShipment shipment;
        shipment.readFromStream( aStream );
        mShipments.push_back( shipment );
        }

    if( !aStream.eof() )
    {
        mIsValid = TRUE;
    }

    aStream.get();  // consume the '\n'
}

//----------------------------------------------------------------------
//! \brief Save this item
//! \details Invokes parent to save all items
//----------------------------------------------------------------------
void AobrdDriverItem::save()
{
    if( mParent )
    {
        mParent->save();
    }
}

//----------------------------------------------------------------------
//! \brief Set the ID (key) of this item
//! \details Sets the key associated with this item
//! \param aId The item's ID
//----------------------------------------------------------------------
void AobrdDriverItem::setId
    (
    const key_type & aId
    )
{
    mDriverId = aId;
}

//----------------------------------------------------------------------
//! \brief Set the parent map of this item
//! \details Sets the map that this item is in
//! \param aParent The item's parent
//----------------------------------------------------------------------
void AobrdDriverItem::setParent
    (
    FileBackedMap<AobrdDriverItem>* aParent
    )
{
    mParent = aParent;
}

//----------------------------------------------------------------------
//! \brief Set the password of this driver
//! \details Sets the password for this driver
//! \param aPassword The driver's password
//----------------------------------------------------------------------
void AobrdDriverItem::setPassword
    (
    const std::string & aPassword
    )
{
    mDriverPassword = aPassword;

    setValid();
}

//----------------------------------------------------------------------
//! \brief Set the first name of this driver
//! \details Sets the first name for this driver
//! \param aFirstName The driver's first name
//----------------------------------------------------------------------
void AobrdDriverItem::setFirstName
    (
    const std::string & aFirstName
    )
{
    mFirstName = aFirstName;

    setValid();
}

//----------------------------------------------------------------------
//! \brief Set the last name of this driver
//! \details Sets the last name for this driver
//! \param aLastName The driver's last name
//----------------------------------------------------------------------
void AobrdDriverItem::setLastName
    (
    const std::string & aLastName
    )
{
    mLastName = aLastName;

    setValid();
}

//----------------------------------------------------------------------
//! \brief Set the carrier name of this driver
//! \details Sets the carrier name for this driver
//! \param aCarrierName The driver's carrier name
//----------------------------------------------------------------------
void AobrdDriverItem::setCarrierName
    (
    const std::string & aCarrierName
    )
{
    mCarrierName = aCarrierName;

    setValid();
}

//----------------------------------------------------------------------
//! \brief Set the carrier ID of this driver
//! \details Sets the carrier ID for this driver
//! \param aCarrierID The driver's carrier ID
//----------------------------------------------------------------------
void AobrdDriverItem::setCarrierID
    (
    const std::string & aCarrierID
    )
{
    mCarrierID = aCarrierID;

    setValid();
}

//----------------------------------------------------------------------
//! \brief Set the long term ruleset of this driver
//! \details Sets the long term ruleset for this driver
//! \param aRuleset The driver's long term ruleset
//----------------------------------------------------------------------
void AobrdDriverItem::setLongTermRuleset
    (
    const uint16 aRuleset
    )
{
    mLongTermRuleset = aRuleset;

    setValid();
}

#if( FMI_SUPPORT_A615 )
//----------------------------------------------------------------------
//! \brief Get item's load type ruleset
//! \details Get the load type ruleset of this item
//! \return The item's load type ruleset
//----------------------------------------------------------------------
uint16 AobrdDriverItem::getLoadTypeRuleset() const
{
    return mLoadTypeRuleset;
}

//----------------------------------------------------------------------
//! \brief Set the load type ruleset of this driver
//! \details Sets the load type ruleset for this driver
//! \param aRuleset The driver's load type ruleset
//----------------------------------------------------------------------
void AobrdDriverItem::setLoadTypeRuleset
    (
    const uint16 aRuleset
    )
{
    mLoadTypeRuleset = aRuleset;

    setValid();
}
#endif

//----------------------------------------------------------------------
//! \brief Set the time zone of this driver
//! \details Sets the time zone for this driver
//! \param aTimeZone The driver's time zone
//----------------------------------------------------------------------
void AobrdDriverItem::setTimeZone
    (
    const uint16 aTimeZone
    )
{
    mTimeZone = aTimeZone;

    setValid();
}

//----------------------------------------------------------------------
//! \brief Write a AobrdDriverItem to an output stream
//! \details Append driver ID and password to the output stream.
//! \param aStream The stream to write to
//----------------------------------------------------------------------
void AobrdDriverItem::writeToStream
    (
    std::ofstream &aStream
    ) const
{
#if( FMI_SUPPORT_A615 )
    // insert value below first readable ascii character as the record separator to indicate new format
    aStream.put( RECORD_SEPARATOR );
    aStream.put( CURRENT_VERSION );
#endif

    aStream << mDriverId << '\0' << mDriverPassword << '\0'
            << mFirstName << '\0' << mLastName << '\0'
            << mCarrierName << '\0' << mCarrierID << '\0'
            << mLongTermRuleset << '\0'
#if( FMI_SUPPORT_A615 )
            << mLoadTypeRuleset << '\0'
#endif
            << mTimeZone << '\0';

    //write the shipment count and shipments
    uint8 shipmentCount = (uint8) mShipments.size();
    aStream.put(shipmentCount);

    for( uint8 i=0; i<shipmentCount; i++ )
        {
        AobrdShipment shipment = mShipments[ i ];
        shipment.writeToStream( aStream );
        }

    //record separator
    aStream << '\n';
}
