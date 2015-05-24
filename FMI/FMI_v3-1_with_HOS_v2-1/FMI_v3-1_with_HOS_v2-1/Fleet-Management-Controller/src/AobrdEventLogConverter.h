/*********************************************************************
*
*   MODULE NAME:
*       AobrdEventLogConverter.cpp
*
*   Copyright 2012 by Garmin Ltd. or its subsidiaries.
*---------------------------------------------------------------------
* $NoKeywords$
*********************************************************************/
#ifndef AOBRDEVENTLOGCONVERTER_H
#define AOBRDEVENTLOGCONVERTER_H

#include "stdafx.h"
#include "garmin_types.h"
#include "fmi.h"
#include <vector>
#include <string>

#if( FMI_SUPPORT_A610 )

//! \brief Utility for converting between raw AOBRD data to human-
//!     readable CSV file.
//!  Used to attempt to auto-convert files received from device and
//!     CSV files selected for upload as AOBRD content to device.
//! \since Protocol A610
class AobrdEventLogConverter
{
public:

    enum evt_log_reason {
        DRIVER_ANNOTATION,
        DRIVER_STATUS_CHANGE,
        DRIVER_VERIFIED_LOG,
        DRIVER_COULD_NOT_VERIFY_LOG,
        DRIVER_ADDED_NEW_SHIPMENT_ENTRY,
        DRIVER_MODIFIED_EXISTING_SHIPMENT_ENTRY,
        PND_FAILURE_DETECTION,
        DRIVER_DELETED_SHIPMENT_ENTRY,

        EVT_LOG_REASON_COUNT
    };

    AobrdEventLogConverter();

    void convertToCsv( const char * source, const char * destFormat );
    uint32 convertToRaw( const char * source, const char * dest );

    uint16 countRecords( const char * source, evt_log_reason type );
    bool findRecordAt( const char* source, evt_log_reason type, uint16 index, bool ( *visitor ) ( AobrdEventLogConverter * converter, std::vector<std::string> & record, void * data ), void * data );

    bool convertAnnotation( std::vector<std::string> & record, fmi_driver_annotation_data_type * annotation );

};
#endif // FMI_SUPPORT_A610

#endif // include guard
