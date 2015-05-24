/*********************************************************************
*
*   HEADER NAME:
*       AobrdShipment.h
*
*   Copyright 2012 by Garmin Ltd. or its subsidiaries.
*---------------------------------------------------------------------
* $NoKeywords$
*********************************************************************/
#ifndef AobrdShipment_H
#define AobrdShipment_H

#include "stdafx.h"
#include <string>
#include <fstream>
#include "garmin_types.h"


//! \brief Data structure to holds an AOBRD shipment info.
//!  Used by the FmiApplicationLayer to provision shipments
//      for AOBRD driver login requests.
//! \since Protocol A610
class AobrdShipment
{
public:
    AobrdShipment();
    virtual ~AobrdShipment();

    time_type getServerTimestamp() const;
    time_type getServerStartTime() const;
    time_type getServerEndTime() const;
    CString getShipperName() const;
    CString getDocNumber() const;
    CString getCommodity() const;

    void readFromStream( std::istream &aStream );

    void setServerTimestamp( const time_type & aTimestamp );
    void setServerStartTime( const time_type & aTimestamp );
    void setServerEndTime( const time_type & aTimestamp );
    void setShipperName( const std::string & aShipperName );
    void setDocNumber( const std::string & aDocNumber );
    void setCommodity( const std::string & aCommodity );

    void writeToStream ( std::ofstream &aStream ) const;

private:
    //! The shipper name, UTF8 encoded.
    std::string mShipperName;

    //! The document number, UTF8 encoded.
    std::string mDocNumber;

    //! The commodity, UTF8 encoded.
    std::string mCommodity;

    //! The server timestamp.
    time_type mServerTimestamp;

    //! The server start time.
    time_type mServerStartTime;

    //! The server end time.
    time_type mServerEndTime;

};

#endif
