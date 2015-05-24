/*********************************************************************
*
*   HEADER NAME:
*       AobrdDriverItem.h
*
*   Copyright 2012 by Garmin Ltd. or its subsidiaries.
*---------------------------------------------------------------------
* $NoKeywords$
*********************************************************************/
#ifndef AobrdDriverItem_H
#define AobrdDriverItem_H

#include <string>
#include <vector>

#include "ClientListItem.h"
#include "AobrdShipment.h"
#include "fmi.h"

//! \brief Data structure to holds an AOBRD driver info.
//!  Used by the FmiApplicationLayer to validate logins
//!     when driver password support is enabled.
//! \since Protocol A610
class AobrdDriverItem :
    public ClientListItem
{
public:
    typedef std::string key_type;

    AobrdDriverItem();
    virtual ~AobrdDriverItem();

    CString getDriverId() const;

    const key_type & getId() const;
    CString getPassword() const;
    CString getFirstName() const;
    CString getLastName() const;
    CString getCarrierName() const;
    CString getCarrierID() const;
    uint16 getLongTermRuleset() const;
#if( FMI_SUPPORT_A615 )
    uint16 getLoadTypeRuleset() const;
#endif
    uint16 getTimeZone() const;

    const std::vector<AobrdShipment> & getShipments() const;
    void addShipment( AobrdShipment & shipment );
    void replaceShipment( AobrdShipment & shipment, int index );
    void deleteShipment( int index );

    void readFromStream( std::istream &aStream );
    void save();
    void setId( const key_type & aId );
    void setParent( FileBackedMap<AobrdDriverItem>* aParent );

    void setPassword( const std::string & aPassword );
    void setFirstName( const std::string & aFirstName );
    void setLastName( const std::string & aLastName );
    void setCarrierName( const std::string & aCarrierName );
    void setCarrierID( const std::string & aCarrierID );
    void setLongTermRuleset( const uint16 aRuleset );
#if( FMI_SUPPORT_A615 )
    void setLoadTypeRuleset( const uint16 aRuleset );
#endif
    void setTimeZone( const uint16 aTimeZone );

    void writeToStream ( std::ofstream &aStream ) const;

private:
    //! The driver ID, UTF8 encoded.
    std::string mDriverId;

    //! The driver password, UTF8 encoded.
    std::string mDriverPassword;

    //! The driver first name, UTF8 encoded.
    std::string mFirstName;

    //! The driver last name, UTF8 encoded.
    std::string mLastName;

    //! The driver carrier name, UTF8 encoded.
    std::string mCarrierName;

    //! The driver first name, UTF8 encoded.
    std::string mCarrierID;

    //! The driver long term ruleset.
    uint16 mLongTermRuleset;

#if( FMI_SUPPORT_A615 )
    //! The driver load type ruleset.
    uint16 mLoadTypeRuleset;
#endif

    //! The driver time zone.
    uint16 mTimeZone;

    //! The shipments.
    std::vector<AobrdShipment> mShipments;

    //! The map that this DriverItem is a part of.
    FileBackedMap<AobrdDriverItem>* mParent;

};

#endif
