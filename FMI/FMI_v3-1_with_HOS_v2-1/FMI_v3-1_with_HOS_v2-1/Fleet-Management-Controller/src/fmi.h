/*********************************************************************
*
*   HEADER NAME:
*       fmi.h - Data structures, types, and constants specific to
*               the Fleet Management Interface Control Specification
*
* Copyright 2008-2013 by Garmin Ltd. or its subsidiaries.
*---------------------------------------------------------------------
* $NoKeywords$
*********************************************************************/
#ifndef FMI_H
#define FMI_H

/*--------------------------------------------------------------------
                           GENERAL INCLUDES
--------------------------------------------------------------------*/
#include "garmin_types.h"

/*--------------------------------------------------------------------
                           LITERAL CONSTANTS
--------------------------------------------------------------------*/
#ifndef FMI_PROTOCOL_LEVEL
#define FMI_PROTOCOL_LEVEL 612
#endif

//! If true, app was build with support for A602 protocols
#define FMI_SUPPORT_A602 ( FMI_PROTOCOL_LEVEL >= 602 )

//! If true, app was build with support for A603 protocols
#define FMI_SUPPORT_A603 ( FMI_PROTOCOL_LEVEL >= 603 )

//! If true, app was build with support for A604 protocols
#define FMI_SUPPORT_A604 ( FMI_PROTOCOL_LEVEL >= 604 )

//! If true, app was build with support for A605 protocols
#define FMI_SUPPORT_A605 ( FMI_PROTOCOL_LEVEL >= 605 )

//! If true, app was build with support for A606 protocols
#define FMI_SUPPORT_A606 ( FMI_PROTOCOL_LEVEL >= 606 )

//! If true, app was build with support for A607 protocols
#define FMI_SUPPORT_A607 ( FMI_PROTOCOL_LEVEL >= 607 )

//! If true, app was build with support for legacy protocols
#define FMI_SUPPORT_LEGACY ( !FMI_SUPPORT_A607 )

//! If true, app was build with support for A608 protocols
#define FMI_SUPPORT_A608 ( FMI_PROTOCOL_LEVEL >= 608 )

//! If true, app was build with support for A609 protocols
#define FMI_SUPPORT_A609 ( FMI_PROTOCOL_LEVEL >= 609 )

//! If true, app was build with support for A610(AOBRD) protocols
#define FMI_SUPPORT_A610 ( FMI_PROTOCOL_LEVEL >= 610 )

//! If true, app was built with support for A611 protocols
#define FMI_SUPPORT_A611 ( FMI_PROTOCOL_LEVEL >= 611 )

//! If true, app was built with support for A612 protocols
#define FMI_SUPPORT_A612 ( FMI_PROTOCOL_LEVEL >= 612 )

//! If true, app was built with support for A613 protocols
#define FMI_SUPPORT_A613 ( FMI_PROTOCOL_LEVEL >= 613 )

//! If true, app was built with support for A614 protocols
#define FMI_SUPPORT_A614 ( FMI_PROTOCOL_LEVEL >= 614 )

//! If true, app was built with support for A615 protocols
#define FMI_SUPPORT_A615 ( FMI_PROTOCOL_LEVEL >= 615 )

//! If true, app was built with support for A616 protocols
#define FMI_SUPPORT_A616 ( FMI_PROTOCOL_LEVEL >= 616 )

//! If true, app was built with support for A617 protocols
#define FMI_SUPPORT_A617 ( FMI_PROTOCOL_LEVEL >= 617 )

//! If true, app was built with support for A618 protocols
#define FMI_SUPPORT_A618 ( FMI_PROTOCOL_LEVEL >= 618 )

//! If true, app was built with support for A619 protocols
#define FMI_SUPPORT_A619 ( FMI_PROTOCOL_LEVEL >= 619 )

//! If TRUE, the server supports Unicode
//! \note This should normally be TRUE
#define UNICODE_ENABLED  ( TRUE )

//! If TRUE, no validation of inputs is performed in the UI.
//! \details Set this to FALSE for a server app that conforms to
//!    the FMI protocols.  Set to TRUE to be allowed to perform
//!    certain operations that violate the Fleet Management
//!    Interface specification.
#define SKIP_VALIDATION  ( FALSE )

//! If TRUE, the Enable is minimal
//! \details If TRUE, the initial FMI enable process only consists
//!     of the Enable FMI protocol.  PVT and auto-ETA are not
//!     enabled, the stop list is not refreshed, and the Unit ID/ESN
//!     Protocol and Product id and Support protocols are not
//!     performed.
//! \note This should normally be FALSE.  If setting this to TRUE
//!     to test a server that does not send any packets after the
//!     enable, also set UNICODE_ENABLED to FALSE.
#define MINIMAL_ENABLE   ( FALSE )

//! If TRUE, interpret the raw packet as ASCII
//! \details If TRUE, the log viewer shows the raw packet as ASCII
//!     as well as in hex; this may make it easier to understand a
//!     packet's content when it contains mostly text.  However,
//!     individual fields are already parsed, and packets are
//!     usually binary data, so this has limited use.  Setting this
//!     to TRUE may require the log viewer window to be resized
//!     to show all the data.
#define LOG_SHOW_RAW_ASCII ( TRUE )

#if FMI_SUPPORT_A605
//! \brief Maximum number of protocols that can be throttled.
//! \details This limit is derived from the size of the
//!     message_throttling_list_data_type
#define MAX_THROTTLED_PROTOCOLS 60
#endif

//! \brief placeholder for invalid latitude value
#define INVALID_LAT          0x7FFFFFFFL

//! \brief Placeholder for an invalid 32-bit value
#define INVALID32            0xFFFFFFFF

//! \brief Placeholder for an invalid 16-bit value
#define INVALID16            0xFFFF

#if( FMI_SUPPORT_A607 )
//! \brief Number of supported drivers
//! \since Protocol A607
#define FMI_DRIVER_COUNT     ( 3 )
#elif( FMI_SUPPORT_A604 )
//! \brief Number of supported drivers
//! \since Protocol A604
#define FMI_DRIVER_COUNT     ( 1 )
#endif

//! \brief Size of the formatted "supported protocols" string
//! \details 5 characters are needed for each protocol_support_data_type,
//! "Xnnn ", plus 1 character is needed for the null terminator.
#define PROTOCOL_SIZE        ( MAX_PAYLOAD_SIZE / sizeof( protocol_support_data_type ) * 5 + 1 )

//! \brief Maximum payload of a single text message, in bytes
#define TEXT_MSG_MAX_SIZE       ( 200 )

#if( FMI_SUPPORT_A611 )
//! \brief Maximum number of bytes in an A611 Long Text Message
#define LONG_TEXT_MSG_MAX_SIZE      ( 2000 )

//! \brief Maximum number of bytes in one segment of an A611 Long Text Message
#define LONG_TEXT_MSG_CHUNK_SIZE    ( 200 )
#endif

#if( FMI_SUPPORT_A611 )
//! \brief The maximum size of any text message, in bytes.
#define TEXT_MSG_BUFFER_MAX_SIZE    LONG_TEXT_MSG_MAX_SIZE
#else
//! \brief The maximum size of any text message, in bytes.
#define TEXT_MSG_BUFFER_MAX_SIZE    TEXT_MSG_MAX_SIZE
#endif

/*--------------------------------------------------------------------
                          FUNDAMENTAL TYPES
--------------------------------------------------------------------*/

/*--------------------------------------------------------------------
                           ENUMERATED TYPES
--------------------------------------------------------------------*/

//! The code page used for encoding of text fields sent to or received
//!     from the client.
//! \since Protocol A604 added UTF8 support.
enum codepage_type
    {
    CODEPAGE_ASCII   = 1252,
#if( FMI_SUPPORT_A604 )
    CODEPAGE_UNICODE = CP_UTF8
#endif
    };

//! Garmin packet ID
enum id_enum
    {
    ID_COMMAND_BYTE         =  10,
    ID_UNIT_ID              =  38,

    ID_DATE_TIME_DATA       =  14,
    ID_PVT_DATA             =  51,

#if( FMI_SUPPORT_LEGACY )
    ID_LEGACY_STOP_MSG      = 135,
    ID_LEGACY_TEXT_MSG      = 136,
#endif

#if( FMI_SUPPORT_A602 )
    ID_FMI_PACKET           = 161,
#endif

#if( FMI_SUPPORT_A615 )
    ID_HOSART_PACKET        = 218,
    ID_SET_TIME_PACKET      = 219,
#endif

    ID_PROTOCOL_ARRAY       = 253,
    ID_PRODUCT_RQST         = 254,
    ID_PRODUCT_DATA         = 255

    }; /* id_type */

#if( FMI_SUPPORT_A615 )
//! HOSART packet ID
enum hosart_packet_ids
{
    HOSART_CHANGE_STATUS = 0,
    HOSART_CHANGE_TO_DRIVING_WARNING,
    HOSART_WARNING_FOUND,
    HOSART_VIOLATION_FOUND,
    HOSART_SET_PS_TO_OFF
};
#endif

//! \brief Garmin command ID (payload when packet ID == ID_COMMAND_BYTE)
//! \see command_enum for supported values
typedef uint16 command_type;
//! Garmin command ID
enum command_enum
    {
    COMMAND_REQ_DATE_TIME       =  5,
    COMMAND_REQ_UNIT_ID         = 14,
    COMMAND_TURN_ON_PVT_DATA    = 49,
    COMMAND_TURN_OFF_PVT_DATA   = 50,
    }; /* command_type */

//! Fleet Management packet ID  (first two bytes of payload when Garmin packet ID == ID_FMI_PACKET)
//! \see fmi_packet_id_enum for supported values
typedef uint16 fmi_id_type;

//! Fleet Management packet ID
enum fmi_packet_id_enum
    {
#if( FMI_SUPPORT_A602 )
    FMI_ID_ENABLE                           = 0x0000,
    FMI_ID_PRODUCT_ID_SUPPORT_RQST          = 0x0001,
    FMI_ID_PRODUCT_ID_DATA                  = 0x0002,
    FMI_ID_PROTOCOL_DATA                    = 0x0003,
#endif

#if( FMI_SUPPORT_A604 )
    FMI_ID_UNICODE_REQUEST                  = 0x0004,
    FMI_ID_UNICODE_RESPONSE                 = 0x0005,
#endif

#if( FMI_SUPPORT_A602 )
    FMI_ID_TEXT_MSG_ACK                     = 0x0020,
    FMI_ID_SERVER_OPEN_TXT_MSG              = 0x0021,
    FMI_ID_SERVER_OK_ACK_TXT_MSG            = 0x0022,
    FMI_ID_SERVER_YES_NO_CONFIRM_MSG        = 0x0023,
#endif

#if( FMI_SUPPORT_A603 )
    FMI_ID_CLIENT_OPEN_TXT_MSG              = 0x0024,
    FMI_ID_CLIENT_TXT_MSG_RCPT              = 0x0025,
#endif

#if( FMI_SUPPORT_A607 )
    FMI_ID_A607_CLIENT_OPEN_TXT_MSG         = 0x0026,
#endif

#if( FMI_SUPPORT_A604 )
    FMI_ID_SET_CANNED_RESP_LIST             = 0x0028,
    FMI_ID_CANNED_RESP_LIST_RCPT            = 0x0029,
    FMI_ID_A604_OPEN_TEXT_MSG               = 0x002A,
    FMI_ID_A604_OPEN_TEXT_MSG_RCPT          = 0x002B,
    FMI_ID_TEXT_MSG_ACK_RCPT                = 0x002C,
#endif

#if( FMI_SUPPORT_A607 )
    FMI_ID_TEXT_MSG_DELETE_REQUEST          = 0x002D,
    FMI_ID_TEXT_MSG_DELETE_RESPONSE         = 0x002E,
#endif

#if( FMI_SUPPORT_A604 )
    FMI_ID_SET_CANNED_RESPONSE              = 0x0030,
    FMI_ID_DELETE_CANNED_RESPONSE           = 0x0031,
    FMI_ID_SET_CANNED_RESPONSE_RCPT         = 0x0032,
    FMI_ID_DELETE_CANNED_RESPONSE_RCPT      = 0x0033,
    FMI_ID_REFRESH_CANNED_RESP_LIST         = 0x0034,

    FMI_ID_TEXT_MSG_STATUS_REQUEST          = 0x0040,
    FMI_ID_TEXT_MSG_STATUS                  = 0x0041,

    FMI_ID_SET_CANNED_MSG                   = 0x0050,
    FMI_ID_SET_CANNED_MSG_RCPT              = 0x0051,
    FMI_ID_DELETE_CANNED_MSG                = 0x0052,
    FMI_ID_DELETE_CANNED_MSG_RCPT           = 0x0053,
    FMI_ID_REFRESH_CANNED_MSG_LIST          = 0x0054,
#endif

#if( FMI_SUPPORT_A611 )
    FMI_ID_LONG_TEXT_MSG                     = 0x0055,
    FMI_ID_LONG_TEXT_MSG_RCPT                = 0x0056,
#endif

#if( FMI_SUPPORT_A602 )
    FMI_ID_A602_STOP                        = 0x0100,
#endif

#if( FMI_SUPPORT_A603 )
    FMI_ID_A603_STOP                        = 0x0101,
#endif

#if( FMI_SUPPORT_A604 )
    FMI_ID_SORT_STOP_LIST                   = 0x0110,
    FMI_ID_SORT_STOP_LIST_ACK               = 0x0111,
#endif

#if( FMI_SUPPORT_A607 )
    FMI_ID_WAYPOINT                         = 0x0130,
    FMI_ID_WAYPOINT_RCPT                    = 0x0131,
    FMI_ID_WAYPOINT_DELETE                  = 0x0132,
    FMI_ID_WAYPOINT_DELETED                 = 0x0133,
    FMI_ID_WAYPOINT_DELETED_RCPT            = 0x0134,
    FMI_ID_DELETE_WAYPOINT_CAT              = 0x0135,
    FMI_ID_DELETE_WAYPOINT_CAT_RCPT         = 0x0136,
    FMI_ID_CREATE_WAYPOINT_CAT              = 0x0137,
    FMI_ID_CREATE_WAYPOINT_CAT_RCPT         = 0x0138,
#endif

#if( FMI_SUPPORT_A603 )
    FMI_ID_ETA_DATA_REQUEST                 = 0x0200,
    FMI_ID_ETA_DATA                         = 0x0201,
    FMI_ID_ETA_DATA_RCPT                    = 0x0202,

    FMI_ID_STOP_STATUS_REQUEST              = 0x0210,
    FMI_ID_STOP_STATUS                      = 0x0211,
    FMI_ID_STOP_STATUS_RCPT                 = 0x0212,

    FMI_ID_AUTO_ARRIVAL                     = 0x0220,
    FMI_ID_DATA_DELETION                    = 0x0230,
#endif

#if( FMI_SUPPORT_A604 )
    FMI_ID_USER_INTERFACE_TEXT              = 0x0240,
    FMI_ID_USER_INTERFACE_TEXT_RCPT         = 0x0241,

    FMI_ID_MSG_THROTTLING_COMMAND           = 0x0250,
    FMI_ID_MSG_THROTTLING_RESPONSE          = 0x0251,
#endif

#if( FMI_SUPPORT_A605 )
    FMI_ID_MSG_THROTTLING_QUERY             = 0x0252,
    FMI_ID_MSG_THROTTLING_QUERY_RESPONSE    = 0x0253,
#endif

#if( FMI_SUPPORT_A604 )
    FMI_ID_PING                             = 0x0260,
    FMI_ID_PING_RESPONSE                    = 0x0261,

    FMI_ID_FILE_TRANSFER_START              = 0x0400,
    FMI_ID_FILE_DATA_PACKET                 = 0x0401,
    FMI_ID_FILE_TRANSFER_END                = 0x0402,
    FMI_ID_FILE_START_RCPT                  = 0x0403,
    FMI_ID_FILE_PACKET_RCPT                 = 0x0404,
    FMI_ID_FILE_END_RCPT                    = 0x0405,
    FMI_ID_GPI_FILE_INFORMATION_REQUEST     = 0x0406,
    FMI_ID_GPI_FILE_INFORMATION             = 0x0407,

    FMI_ID_SET_DRIVER_STATUS_LIST_ITEM      = 0x0800,
    FMI_ID_DELETE_DRIVER_STATUS_LIST_ITEM   = 0x0801,
    FMI_ID_SET_DRIVER_STATUS_LIST_ITEM_RCPT = 0x0802,
    FMI_ID_DEL_DRIVER_STATUS_LIST_ITEM_RCPT = 0x0803,
    FMI_ID_DRIVER_STATUS_LIST_REFRESH       = 0x0804,
    FMI_ID_DRIVER_ID_REQUEST                = 0x0810,
    FMI_ID_DRIVER_ID_UPDATE                 = 0x0811,
    FMI_ID_DRIVER_ID_RCPT                   = 0x0812,
#endif

#if( FMI_SUPPORT_A607 )
    FMI_ID_DRIVER_ID_UPDATE_D607            = 0x0813,
#endif

#if( FMI_SUPPORT_A604 )
    FMI_ID_DRIVER_STATUS_REQUEST            = 0x0820,
    FMI_ID_DRIVER_STATUS_UPDATE             = 0x0821,
    FMI_ID_DRIVER_STATUS_RCPT               = 0x0822,
#endif

#if( FMI_SUPPORT_A607 )
    FMI_ID_DRIVER_STATUS_UPDATE_D607        = 0x0823,
#endif

#if ( FMI_SUPPORT_A606 )
    FMI_SAFE_MODE                           = 0x0900,
    FMI_SAFE_MODE_RESP                      = 0x0901,
#endif

#if ( FMI_SUPPORT_A608 )
    FMI_SPEED_LIMIT_SET                     = 0X1000,
    FMI_SPEED_LIMIT_RCPT                    = 0X1001,
    FMI_SPEED_LIMIT_ALERT                   = 0X1002,
    FMI_SPEED_LIMIT_ALERT_RCPT              = 0X1003,
#endif

#if ( FMI_SUPPORT_A609 )
    FMI_REBOOT_DEVICE_REQUEST               = 0X1010,
#endif

#if ( FMI_SUPPORT_A610 )
    FMI_SET_ODOMETER_REQUEST                = 0X1100,
    FMI_DRIVER_LOGIN_REQUEST                = 0X1101,
    FMI_DRIVER_LOGIN_RESPONSE               = 0X1102,
    FMI_DRIVER_PROFILE_DOWNLOAD_REQUEST     = 0X1103,
    FMI_DRIVER_PROFILE_DOWNLOAD_RESPONSE    = 0X1104,
    FMI_DRIVER_PROFILE_UPDATE               = 0X1105,
    FMI_DRIVER_STATUS_UPDATE_REQUEST        = 0X1106,
    FMI_DRIVER_STATUS_UPDATE_RESPONSE       = 0X1107,
    FMI_DRIVER_STATUS_UPDATE_RECEIPT        = 0X1108,
    FMI_DOWNLOAD_SHIPMENTS_REQUEST          = 0X1109,
    FMI_SHIPMENT_DOWNLOAD_RESPONSE          = 0X110A,
    FMI_SHIPMENT_DOWNLOAD_RECEIPT           = 0X110B,
    FMI_DRIVER_PROFILE_UPDATE_RESPONSE      = 0X110C,
    FMI_ANNOTATION_DOWNLOAD_REQUEST         = 0X110D,
    FMI_ANNOTATION_DOWNLOAD_RESPONSE        = 0X110E,
    FMI_ANNOTATION_DOWNLOAD_RECEIPT         = 0X110F,
#endif

#if ( FMI_SUPPORT_A612 )
    FMI_CUSTOM_FORM_DEL_REQUEST             = 0x1200,
    FMI_CUSTOM_FORM_DEL_RECEIPT             = 0x1201,
    FMI_CUSTOM_FORM_MOVE_REQUEST            = 0x1202,
    FMI_CUSTOM_FORM_MOVE_RECEIPT            = 0x1203,
    FMI_CUSTOM_FORM_GET_POS_REQUEST         = 0x1204,
    FMI_CUSTOM_FORM_GET_POS_RECEIPT         = 0x1205,
#endif

#if ( FMI_SUPPORT_A614 )
    FMI_STOP_CALC_ACK_REQUEST               = 0x1220,
    FMI_STOP_CALC_ACK_RECEIPT               = 0x1221,
#endif

#if ( FMI_SUPPORT_A613 )
    FMI_CUSTOM_AVOID_ADD_REQUEST            = 0X1230,
    FMI_CUSTOM_AVOID_ADD_RECEIPT            = 0X1231,
    FMI_CUSTOM_AVOID_DEL_REQUEST            = 0X1232,
    FMI_CUSTOM_AVOID_DEL_RECEIPT            = 0X1233,
    FMI_CUSTOM_AVOID_TOGGLE_REQUEST         = 0X1234,
    FMI_CUSTOM_AVOID_TOGGLE_RECEIPT         = 0X1235,
    FMI_CUSTOM_AVOID_ENABLE_FEATURE_REQUEST = 0X1236,
    FMI_CUSTOM_AVOID_ENABLE_FEATURE_RECEIPT = 0X1237,
#endif

#if ( FMI_SUPPORT_A615 )
    FMI_IFTA_DATA_FETCH_REQUEST             = 0X0006,
    FMI_IFTA_DATA_FETCH_RECEIPT             = 0X0007,
    FMI_IFTA_DATA_DELETE_REQUEST            = 0X0008,
    FMI_IFTA_DATA_DELETE_RECEIPT            = 0X0009,
    FMI_DRIVER_PROFILE_UPDATE_V2            = 0X1110,
    FMI_DRIVER_PROFILE_DOWNLOAD_RESPONSE_V2 = 0X1111,
    FMI_HOS_AUTO_STATUS_FEATURE_REQUEST     = 0X1300,   // Deprecated, use 0X1500 with HOS_SETTING_AUTO_STATUS_STOP_MOVING_THRESHOLD_SECONDS
    FMI_HOS_AUTO_STATUS_FEATURE_RECEIPT     = 0X1301,   // Deprecated, use 0X1501 with HOS_SETTING_AUTO_STATUS_STOP_MOVING_THRESHOLD_SECONDS
    FMI_AOBRD_DRIVER_LOGOFF_REQUEST         = 0X1310,
    FMI_AOBRD_DRIVER_LOGOFF_RECEIPT         = 0X1311,
    FMI_HOS_8_HOUR_RULE_ENABLE_REQUEST      = 0X1312,   // Deprecated, use 0X1500 with HOS_SETTING_EIGHT_HOUR_RULE_ENABLE
    FMI_HOS_8_HOUR_RULE_ENABLE_RECEIPT      = 0X1313,   // Deprecated, use 0X1501 with HOS_SETTING_EIGHT_HOUR_RULE_ENABLE
#endif

#if ( FMI_SUPPORT_A616 )
    FMI_SET_BAUD_REQUEST                    = 0X0011,
    FMI_SET_BAUD_RECEIPT                    = 0X0012,
#endif

#if ( FMI_SUPPORT_A617 )
    FMI_ALERT_POPUP_REQUEST                 = 0X1400,
    FMI_ALERT_POPUP_RECEIPT                 = 0X1401,
    FMI_SENSOR_CONFIG_REQUEST               = 0X1402,
    FMI_SENSOR_CONFIG_RECEIPT               = 0X1403,
    FMI_SENSOR_DELETE_REQUEST               = 0X1404,
    FMI_SENSOR_DELETE_RECEIPT               = 0X1405,
    FMI_SENSOR_UPDATE_REQUEST               = 0X1406,
    FMI_SENSOR_QUERY_DISPLAY_INDEX_REQUEST  = 0X1407,
    FMI_SENSOR_QUERY_DISPLAY_INDEX_RECEIPT  = 0X1408,
#endif

#if ( FMI_SUPPORT_A619 )
    FMI_HOS_SET_SETTING_DATA_REQUEST        = 0X1500,
    FMI_HOS_SET_SETTING_DATA_RECEIPT        = 0X1501,
#endif

    FMI_ID_END = 0xFFFF
    }; /* fmi_id_type */

#if( FMI_SUPPORT_A608 )
//! Enum for speed limit alert category
//! \since Protocol A608
enum speed_limit_alert_category_type
    {
    SPEED_LIMIT_ALERT_BEGIN     = 0,
    SPEED_LIMIT_ALERT_CHANGE    = 1,
    SPEED_LIMIT_ALERT_END       = 2,
    SPEED_LIMIT_ALERT_ERROR     = 3,
    SPEED_LIMIT_ALERT_INVALID   = 4
    };

//! Enum for speed limit alert result
//! \since Protocol A608
enum speed_limit_alert_result_data_type
    {
    SPEED_LIMIT_RESULT_SUCCESS          = 0,
    SPEED_LIMIT_RESULT_ERROR            = 1,
    SPEED_LIMIT_RESULT_MODE_UNSUPPORTED = 2
    };

//! Enum for speed limit alert mode
//! \since Protocol A608
enum speed_limit_alert_mode_type
    {
    SPEED_LIMIT_MODE_CAR                = 0,
    SPEED_LIMIT_MODE_OFF                = 1,
    SPEED_LIMIT_MODE_TRUCK              = 2,

    SPEED_LIMIT_MODE_CNT
    };
#endif

#if( FMI_SUPPORT_A607 )
//! Valid values for the features field of the fmi_features_data_type
//! \since Protocol A607
enum fmi_feature_type
{
    FEATURE_ID_UNICODE               = 1,
    FEATURE_ID_A607_SUPPORT          = 2,
    FEATURE_ID_DRIVER_PASSWORDS      = 10,
    FEATURE_ID_MULTIPLE_DRIVERS      = 11,
    FEATURE_ID_AOBRD_SUPPORT         = 12,

    FEATURE_STATE_ENABLED            = 1 << 15,
    FEATURE_STATE_DISABLED           = 0 << 15,

    FEATURE_ID_MASK                  = setbits(  0, 15 ),
    FEATURE_STATE_MASK               = setbits( 15,  1 )
};
#endif

//! Valid values for file transfer types
//! \since Protocol A610
enum file_type_type
    {
    FMI_FILE_TYPE_GPI,
    #if( FMI_SUPPORT_A610 )
        FMI_FILE_TYPE_AOBRD                 = 1,
    #endif
    #if( FMI_SUPPORT_A612 )
        FMI_FILE_TYPE_CUSTOM_FORMS          = 2,
    #endif
    #if( FMI_SUPPORT_A614 )
        FMI_FILE_TYPE_PATH_SPECIFIC_ROUTE   = 3,
    #endif
    #if( FMI_SUPPORT_A615 )
        FMI_FILE_TYPE_IFTA                  = 4,
    #endif
    #if( FMI_SUPPORT_A618 )
        FMI_FILE_TYPE_LONG_STOP             = 5,
    #endif
    FMI_FILE_TYPE_CNT
    };

#if( FMI_SUPPORT_A610 )
//! Enum for shipment download packet content type
//! \since Protocol A610
enum AOBRD_download_response_result_code
{
    CONTAINS_DATA  = 0,
    EMPTY_DATA     = 1,
    UNKNOWN_DRIVER = 2,
    UNSUPPORTED    = 3
};

//! Enum for log status/shipment download packet receipt result code
//! \since Protocol A610
enum aobrd_download_receipt_result_code
{
    FMI_LOGIN_NO_ERRORS                     = 0,
    FMI_LOGIN_WRONG_ID_SERVER_RESPONSE      = 1,
    FMI_LOGIN_INVALID_ID_SERVER_RESPONSE    = 2,
    FMI_LOGIN_UNEXPECTED_RESPONSE_PACKET    = 3,
    FMI_LOGIN_SHIPMENT_DB_ERROR             = 4,
    FMI_LOGIN_ANNOTATION_DB_ERROR           = 5
};

//! Enum for supported time zone IDs
//! \since Protocol A610
enum aobrd_time_zone
{
    EASTERN     = 0,
    CENTRAL     = 1,
    MOUNTAIN    = 2,
    PACIFIC     = 3,
    ALASKA      = 4,
    HAWAII      = 5,
    TIME_ZONE_COUNT
};

//! Enum for fmi_ack_type
//! \since Protocol A610
enum aobrd_update_result_code_type
{
    UPDATE_ACCEPT,             //0     Profile updated OK
    UPDATE_FAIL,               //1     Profile update fail
    UPDATE_DECLINE,            //2     Driver declined update
    UPDATE_NOT_READY,          //3     Profile update already in progress (the driver in progress is the driver_id)
    UPDATE_NO_DATA,            //4     Server did not provide a data field
    UPDATE_STORAGE_ERROR,      //5     FMI had a storage problem saving to a buffer
    UPDATE_RESULT_ERROR,       //6     GUI sent FMI a bad pointer to report success/fail to driver
    UPDATE_INTERFACE_ERROR,    //7     HOS asked for the Driver ID but gave FMI a bad pointer

    NUM_OF_UPDATE_PROFILE_CODES
};
#endif

#if( FMI_SUPPORT_A615 )
enum HosLoadTypeRuleSet
{
    HOS_RULES_LOAD_TYPE_PROPERTY,   //0     Property-carrying vehicle
    HOS_RULES_LOAD_TYPE_PASSENGER,  //1     Passenger-carrying vehicle

    NUM_HOS_RULES_LOAD_TYPE
};

//! Enum for IFTA file fetch receipt
//! \since Protocol A615
enum IFTA_data_mod_result_code_type
{
    FMI_IFTA_OK,
    FMI_IFTA_EXPORT_NO_DATA_FOUND,
    FMI_IFTA_BUSY_ERROR = 0x10,     // start of internal errors
    FMI_IFTA_NULL_INPUT_PTR_ERROR,
    FMI_IFTA_STATE_ERROR,
    FMI_IFTA_MIN_DATA_SIZE_ERROR,
    FMI_IFTA_MAX_DATA_SIZE_ERROR,
    FMI_IFTA_MALLOC_ERROR,
    FMI_IFTA_GZIP_ERROR,
    FMI_IFTA_GFS_OPEN_ERROR,
    FMI_IFTA_GFS_DIR_OPEN_ERROR,
    FMI_IFTA_GFS_FSTAT_ERROR,
    FMI_IFTA_GFS_READ_ERROR,
    FMI_IFTA_GFS_REMOVE_ERROR,
    FMI_IFTA_EXPORT_DATA_ERROR,
    FMI_IFTA_DATA_GFS_WRITE_ERROR,
    FMI_IFTA_TX_ERROR
};

//! Enum for Auto Status Update result code
//! \since Protocol A615
enum HOS_settings_result_code_type
{
    FMI_HOS_SETTING_RESULT_OK,          // change as requested
    FMI_HOS_SETTING_RESULT_SET_TO_MIN,  // threshold set to min
    FMI_HOS_SETTING_RESULT_SET_TO_MAX,  // threshold set to max
    FMI_HOS_SETTING_RESULT_FAIL = 0xFF  // error
};

#endif

#if( FMI_SUPPORT_A616 )
//! Enum for Baud Rate receipt result code
//! \since Protocol A616
enum fmi_baud_result_code_type
{
    FMI_BAUD_RATE_OK,
    FMI_BAUD_REQUEST_TYPE_SERVER_ERROR,     /* server sent invalid request */
    FMI_BAUD_RATE_SERVER_ERROR,             /* server sent invalid baud rate */
    FMI_BAUD_RATE_CLIENT_ERROR = 0x10,      /* Internal error */
    FMI_BAUD_SET_ERROR,                     /* Internal error */
    FMI_BAUD_PLUG_ID_ERROR                  /* Internal error */
};

//! Enum for Baud Rate request type
//! \since Protocol A616
enum fmi_baud_request_type
{
    FMI_BAUD_NEW_RATE_REQUEST,
    FMI_BAUD_RATE_SYNC_REQUEST
};

//! Enum for Baud Rate code type
//! \since Protocol A616
enum fmi_baud_rate_code_type
{
    FMI_BAUD_RATE_9600  = 0x06,
    FMI_BAUD_RATE_57600 = 0x0c
};

#endif

#if( FMI_SUPPORT_A617 )
//! Enum for Alert Popup predefined icon numbers
//! \since Protocol A617
enum fmi_alert_popup_icons
{
    FMI_ALERT_ICON_NONE,
    FMI_ALERT_ICON_DRIVER_BEHAVIOR,
    FMI_ALERT_ICON_TIRE_PRESSURE,
    FMI_ALERT_ICON_TEMPERATURE,
    FMI_ALERT_ICON_DOOR_SENSOR,
    FMI_ALERT_ICON_VEHICLE_MAINTENANCE,
    FMI_ALERT_ICON_OBD2_GENERIC_SENSOR,
    FMI_ALERT_ICON_GENERIC_SENSOR1,
    FMI_ALERT_ICON_GENERIC_SENSOR2,
    FMI_ALERT_ICON_GENERIC_SENSOR3,
    FMI_ALERT_ICON_GENERAL_CONNECTIVITY,
    FMI_ALERT_ICON_DAILY_HOURS_COUNTER,
    FMI_ALERT_ICON_WEEKLY_HOURS_COUNTER,
    FMI_ALERT_ICON_REST_HOURS_COUNTER,
    FMI_ALERT_ICON_BREAK_HOURS_COUNTER,
    FMI_ALERT_ICON_TASKS,
    FMI_ALERT_ICON_WEIGHT,
    FMI_ALERT_ICON_INFORMATION,
    FMI_ALERT_ICON_FUEL,
    FMI_ALERT_ICON_EU_AVAILABLE,
    FMI_ALERT_ICON_EU_DRIVING,
    FMI_ALERT_ICON_EU_REST,
    FMI_ALERT_ICON_EU_WORK,

    FMI_ALERT_ICON_RESERVED             /* all up to maximum value is reserved for future use */
};

//! Enum for Alert Popup predefined icon numbers
//! \since Protocol A617
enum fmi_alert_popup_severities
{
    FMI_ALERT_SEVERITY_NORMAL,
    FMI_ALERT_SEVERITY_MEDIUM,
    FMI_ALERT_SEVERITY_HIGH
};

//! Enum for Alert Popup receipt result code
//! \since Protocol A617
enum fmi_alert_popup_result_code_type
{
    FMI_ALERT_POPUP_SUCCESS,
    FMI_ALERT_TEXT_TOO_LONG,                /* Text string is too long to fit on the device screen */
    FMI_ALERT_ICON_NUM_OUT_OF_RANGE,        /* Icon was not in the available list, so no icon will be displayed */
    FMI_ALERT_MISSING_ALERT_TEXT_AND_ICON,  /* Either alert text or icon must be specified */
    FMI_ALERT_SEVERITY_OUT_OF_RANGE,        /* Severity invalid */
    FMI_ALERT_TIMEOUT_OUT_OF_RANGE          /* Timeout is too large */
};

//! Enum for Sensor receipt result code
//! \since Protocol A617
enum fmi_sensor_create_result_code_type
{
    FMI_SENSOR_SUCCESS,
    FMI_SENSOR_ERR_NAME_TOO_LONG,                /* Name string is too long to fit on the device screen */
    FMI_SENSOR_ERR_ICON_NUM_OUT_OF_RANGE,        /* Icon was not in the available list, so no icon will be displayed */
    FMI_SENSOR_ERR_MISSING_SENSOR_NAME,          /* Name must be specified */
    FMI_SENSOR_ERR_SEVERITY_OUT_OF_RANGE,        /* Severity invalid */
    FMI_SENSOR_ERR_STATUS_TOO_LONG,              /* Sensor status is too long */
    FMI_SENSOR_ERR_DESCR_TOO_LONG,               /* Sensor description is too long */
    FMI_SENSOR_ERR_TOO_MANY_SENSORS,             /* Too many sensors */
    FMI_SENSOR_ERR_ID_NOT_FOUND,                 /* Sensor unique ID not found */
    FMI_SENSOR_ERR_DB                            /* Unable to save to database */
};

#endif

#if( FMI_SUPPORT_A619 )
//! Enum for HOS config setting type
//! \since Protocol A619
enum HOS_setting_tag_type
{
    HOS_SETTING_AUTO_STATUS_STOP_MOVING_THRESHOLD_SECONDS,          /* auto status updates feature's stop moving threshold in seconds */
    HOS_SETTING_EIGHT_HOUR_RULE_ENABLE,                             /* eight hour rule feature (enable only) */
    HOS_SETTING_PERIODIC_STATUS_PERIOD_SECONDS,                     /* periodic status updates feature's period in seconds */
    HOS_SETTING_LAST = HOS_SETTING_PERIODIC_STATUS_PERIOD_SECONDS   /* last tag */
};
#endif

#if( FMI_SUPPORT_A604 )
//! Valid values for the message_type field of the
//! A604_server_to_client_open_text_msg_data_type
//! \since Protocol A604
enum a604_message_type
{
    A604_MESSAGE_TYPE_NORMAL          = 0,
    A604_MESSAGE_TYPE_DISP_IMMEDIATE  = 1
};

//! Enum for A604 message status protocol.
//! \since Protocol A604
enum fmi_A604_message_status
{
    MESSAGE_STATUS_UNREAD      = 0,
    MESSAGE_STATUS_READ        = 1,
    MESSAGE_STATUS_NOT_FOUND   = 2
};

//! Enumeration for result_code from the canned_response_list packet.
//! \since Protocol A604
enum canned_response_list_result
{
    CANNED_RESP_LIST_SUCCESS            = 0,
    CANNED_RESP_LIST_INVALID_COUNT      = 1,
    CANNED_RESP_LIST_INVALID_MSG_ID     = 2,
    CANNED_RESP_LIST_DUPLICATE_MSG_ID   = 3,
    CANNED_RESP_LIST_FULL               = 4
};

//! Type for new_state from the message_throttling_data_type
//! \since Protocol A604
typedef uint16 message_throttling_state_type;

//! Enumeration for new_state from the message_throttling_data_type
//! \since Protocol A604
enum message_throttling_state_enum
{
    MESSAGE_THROTTLE_STATE_DISABLE = 0,
    MESSAGE_THROTTLE_STATE_ENABLE  = 1,
    MESSAGE_THROTTLE_STATE_ERROR   = 4095
};

//! Valid values for file transfer result
//! \since Protocol A604
enum file_result_data_type
    {
    FMI_FILE_RESULT_SUCCESS          = 0,
    FMI_FILE_RESULT_CRC_ERROR        = 1,
    FMI_FILE_RESULT_LOW_MEM          = 2,
    FMI_FILE_RESULT_INVALID_FILE     = 3,
    FMI_FILE_RESULT_NO_TRANSFER      = 4,
    FMI_FILE_RESULT_SEVERE           = 5,
    FMI_FILE_RESULT_INVALID_FILE_TYPE = 6,
    FMI_FILE_RESULT_INVALID_GPI_FILE  = 7,
    FMI_FILE_START_OPEN_ERROR         = 8,
    FMI_FILE_START_CLOSE_ERROR        = 9,
    FMI_FILE_RESULT_FINALIZE_ERROR    = 10,
    FMI_FILE_RESULT_NOT_PROCESSED     = 11,
    FMI_FILE_RESULT_RX_GZIP_ERROR     = 12,
    FMI_FILE_RESULT_BUSY              = 13
    };


#endif

#if( FMI_SUPPORT_A602 )
//! Enumeration for A602 ack text message responses.
//! \since Protocol A602
enum txt_ack_type
{
    OK_ACK       = 0,
    YES_ACK      = 1,
    NO_ACK       = 2
};
#endif

#if( FMI_SUPPORT_A603 )
//! Enumeration for Stop Status protocol.
//! \since Protocol A603
typedef uint16 stop_status_status_type;
enum stop_status_status_enum
{
    INVALID_STOP_STATUS      = INVALID16,

    REQUEST_STOP_STATUS      = 0,
    REQUEST_MARK_STOP_DONE   = 1,
    REQUEST_ACTIVATE_STOP    = 2,
    REQUEST_DELETE_STOP      = 3,
    REQUEST_MOVE_STOP        = 4,

    STOP_STATUS_ACTIVE       = 100,
    STOP_STATUS_DONE         = 101,
    STOP_STATUS_UNREAD       = 102,
    STOP_STATUS_READ         = 103,
    STOP_STATUS_DELETED      = 104
};

//! Enumeration for Data Deletion protocol
//! \since Protocol A603
enum del_data
{
    DELETE_ALL_STOPS             = 0,
    DELETE_ALL_MESSAGES          = 1,
#if( FMI_SUPPORT_A604 )
    DELETE_ACTIVE_ROUTE          = 2,
    DELETE_CANNED_MESSAGES       = 3,
    DELETE_CANNED_RESPONSES      = 4,
    DELETE_GPI_FILE              = 5,
    DELETE_DRIVER_ID_AND_STATUS  = 6,
    DISABLE_FMI                  = 7,
#endif
#if( FMI_SUPPORT_A607 )
    DELETE_WAYPOINTS             = 8,
#endif
#if( FMI_SUPPORT_A612 )
    DELETE_FORMS                 = 10,
#endif
#if( FMI_SUPPORT_A613 )
    DELETE_CUSTOM_AVOIDANCES     = 11,
#endif
#if( FMI_SUPPORT_A617 )
    DELETE_SENSORS               = 12
#endif
};
#endif     // end of #if( FMI_SUPPORT_A603 )

#if( FMI_SUPPORT_A611 )
enum FMI_long_text_response_codes_t8
{
    LONG_TEXT_SUCCESS                   = 0,
    LONG_TEXT_INVALID_ID_SIZE           = 1,
    LONG_TEXT_NON_ZERO_SEQ_NUM_ERROR    = 2,
    LONG_TEXT_ID_IN_USE_SQL_ERROR       = 3,
    LONG_TEXT_ID_IN_USE_ERROR           = 4,
    LONG_TEXT_MALLOC_BUFFER_FAIL        = 5,
    LONG_TEXT_WRONG_ID_SIZE             = 6,
    LONG_TEXT_WRONG_ID_ERROR            = 7,
    LONG_TEXT_NON_INC_SEQ_NUM_ERROR     = 8,
    LONG_TEXT_WRONG_ORIG_TIME           = 9,
    LONG_TEXT_WRONG_MESSAGE_TYPE        = 10,
    LONG_TEXT_EXCEEDED_MAX_BYTES        = 11,
    LONG_TEXT_APPEND_NULL_PTR           = 12,
    LONG_TEXT_SAVE_SQL_NULL_PTR         = 13,
    LONG_TEXT_MALLOC_INBOX_FAIL         = 14,
    LONG_TEXT_SQL_INSERT_FAIL           = 15
};
#endif

#if ( FMI_SUPPORT_A613 )
enum FMI_custom_avoid_response_codes
{
    CUSTOM_AVOID_SUCCESS                = 0,
    CUSTOM_AVOID_ID_NOT_FOUND           = 1,
    CUSTOM_AVOID_ERR_FULL               = 2,
    CUSTOM_AVOID_ERR_NV                 = 3,
    CUSTOM_AVOID_ERR_NM_INUSE           = 4,
    CUSTOM_AVOID_ERR_FEAT_NOT_ENBL      = 5,
    CUSTOM_AVOID_ERR_ID_OUT_OF_RANGE    = 6
};
#endif

#include "pack_begin.h"
//! Payload for Garmin ID_PVT_DATA packet
__packed struct pvt_data_type
    {
    float32                 altitude;               //!< Altitude above the WGS84 ellipsoid, in meters.
    float32                 epe;                    //!< Estimated position error, 2 sigma, in meters.
    float32                 eph;                    //!< Estimated horizontal position error, 2 sigma, in meters.
    float32                 epv;                    //!< Estimated vertical position error, 2 sigma, in meters.
    uint16                  type_of_gps_fix;        //!< Enum for type of GPS fix, see gps_fix_type
    float64                 time_of_week;           //!< Seconds since Sunday 12:00 AM (excludes leap seconds)
    double_position_type    position;               //!< Current position of the client
    float32                 east_velocity;          //!< East velocity in m/s, negative is west
    float32                 north_velocity;         //!< North velocity in m/s, negative is south
    float32                 up_velocity;            //!< Up velocity in m/s, negative is down
    float32                 mean_sea_level_height;  //!< Height of WGS84 ellipsoid above MSL at current position, in meters
    sint16                  leap_seconds;           //!< Number of leap seconds as of the current time
    uint32                  week_number_days;       //!< Days from UTC December 31st, 1989 to beginning of current week
    };

//! Possible values for pvt_data_type.type_of_gps_fix
enum gps_fix_type
{
    GPS_FIX_UNUSABLE = 0,
    GPS_FIX_INVALID  = 1,
    GPS_FIX_2D       = 2,
    GPS_FIX_3D       = 3,
    GPS_FIX_2D_DIFF  = 4,
    GPS_FIX_3D_DIFF  = 5
};

//! Payload for Garmin ID_UNIT_ID packet
__packed struct unit_id_data_type /* Garmin */
    {
    uint32                    unit_id;     //!< Unit ID (ESN) of the client
    };

//! Payload for Garmin ID_PRODUCT_DATA (A000)
//! and FMI FMI_ID_PRODUCT_ID_DATA (A602) packet
__packed struct product_id_data_type
    {
    uint16                  product_id;          //!< Product ID of the client
    sint16                  software_version;    //!< Software version * 100 (312 means version 3.12)
    };

//! \brief Element of the array returned in Garmin ID_PROTOCOL_ARRAY (A001)
//! or FMI_ID_PROTOCOL_DATA (A602) packets.
__packed struct protocol_support_data_type
    {
    char                    tag;                  //!< Type of protocol (e.g., 'A', D')
    sint16                  data;                 //!< Protocol number
    };

//! \brief Payload for FMI_ID_ENABLE
__packed struct fmi_features_data_type
    {
    uint8                   feature_count;       //!< Number of feature IDs in features[]
    uint8                   reserved;            //!< Set to 0
    uint16                  features[ 126 ];     //!< Array of feature IDs
    };

#if( FMI_SUPPORT_A602 )
//! \brief Payload of FMI_ID_SERVER_OPEN_TXT_MSG packet
//! \since Protocol A602
__packed struct A602_server_to_client_open_text_msg_data_type
    {
    time_type           origination_time;    //!< Time when the client sent the message
    char                text_message[ 200 ]; //!< The message text (variable length, null terminated, 200 bytes max)
    };

//! \brief Payload of server to client messages requiring a response (A602)
//! \details Payload of FMI_ID_SERVER_OK_ACK_TXT_MSG and FMI_ID_SERVER_YES_NO_CONFIRM_MSG packets.
//! \since Protocol A602
__packed struct server_to_client_ack_text_msg_data_type
    {
    time_type            origination_time;        //!< Origination time of the message
    uint8                id_size;                 //!< Number of significant bytes of the message ID
    uint8                reserved[3];             //!< set to 0
    uint8                id[ 16 ];                //!< message ID
    char                 text_message[ 200 ];     //!< Text message (variable length, null-terminated string, 200 bytes max)
    };

//! \brief Payload of FMI_ID_TEXT_MSG_ACK packet
//! \since Protocol A602
__packed struct text_msg_ack_data_type /* D602 */
    {
    time_type            origination_time;        //!< Origination time of the response
    uint8                id_size;                 //!< Number of significant bytes of the message ID
    uint8                reserved[3];             //!< set to 0
    uint8                id[ 16 ];                //!< message ID
    uint32               msg_ack_type;            //!< The response selected by the user
    };
#endif //FMI_SUPPORT_A602)

#if( FMI_SUPPORT_A603 )
//! \brief Payload of FMI_ID_CLIENT_OPEN_TXT_MSG packet
//! \since Protocol A603
__packed struct client_to_server_open_text_msg_data_type /* D603 */
    {
    time_type           origination_time;         //!< Time when the message was sent by the client
    uint32              unique_id;                //!< Unique ID generated by client
    char                text_message[ 200 ];      //!< Message text (variable length, null-terminated string)
    };

//! \brief Payload of FMI_ID_CLIENT_TXT_MSG_RCPT packet
//! \since Protocol A603
__packed struct client_to_server_text_msg_receipt_data_type /* D603 */
    {
    uint32 unique_id;                              //!< unique_id from client_to_server_open_text_msg_data_type
    };
#endif

#if( FMI_SUPPORT_A607 )
//! \brief Payload of FMI_ID_A607_CLIENT_OPEN_TXT_MSG packet
//! \since Protocol A607
__packed struct client_to_server_D607_open_text_msg_data_type /* D607 */
{
    time_type           origination_time;         //!< Time when the message was created by the client
    sc_position_type    scposn;                   //!< Position when the text message was created by the client
    uint32              unique_id;                //!< Unique ID generated by client
    uint8               id_size;                  //!< ID size of message being responded to
    uint8               reserved[ 3 ];            //!< Set to 0
    uint8               id[ 16 ];                 //!< ID of message being responded to
    char                text_message[ 200 ];      //!< Message text (variable length, null-terminated string)
};
#endif

#if( FMI_SUPPORT_A603 )
//! \brief Payload of FMI_ID_A603_STOP packet
//! \since Protocol A603
__packed struct A603_stop_data_type
    {
    time_type             origination_time;    //!< Time when the stop was originated by the server
    sc_position_type      stop_position;       //!< Location of the stop
    uint32                unique_id;           //!< Unique ID of the stop for use with the Stop Status protocol
    char                  text[ 200 ];         //!< Text (description) of stop. Variable length, null-terminated string.
    };

//! \brief Payload of FMI_ID_STOP_STATUS and FMI_ID_STOP_STATUS_REQUEST packets
//! \since Protocol A603
__packed struct stop_status_data_type
    {
    uint32                    unique_id;           //!< Unique ID of the A603 stop
    uint16                    stop_status;         //!< The stop status
    uint16                    stop_index_in_list;  //!< The stop index in list
    };

//! \brief Payload of FMI_ID_STOP_STATUS_RCPT packet
//! \since Protocol A603
__packed struct stop_status_receipt_data_type
    {
    uint32                    unique_id;           //!< unique_id from the stop_status_data_type.
    };

#endif // FMI_SUPPORT_A603

#if( FMI_SUPPORT_A602 )
//! \brief Payload of FMI_ID_A602_STOP packet
//! \since Protocol A602
__packed struct A602_stop_data_type
    {
    time_type           origination_time;     //!< Origination time when the server sent the stop to the client
    sc_position_type    stop_position;        //!< Location of the stop
    char                text[ 51 ];           //!< Text/description of the stop.  Variable length, null-terminated string.
    };
#endif

#if( FMI_SUPPORT_LEGACY )
//! \brief Payload of Garmin ID_LEGACY_STOP_MSG packet
__packed struct legacy_stop_data_type
    {
    sc_position_type    stop_position;         //!< Location of the stop
    char                text[ 200 ];           //!< Text/description of the stop.  Variable length, null-terminated string.
    };
#endif

#if( FMI_SUPPORT_A603 )
//! \brief Payload of FMI_ID_AUTO_ARRIVAL packet
//! \since Protocol A603
__packed struct auto_arrival_data_type /* D603 */
    {
    uint32 stop_time;                          //!< Minimum stop time before auto-arrival is activated, in seconds
    uint32 stop_distance;                      //!< Minimum distance to destination before auto-arrival is activated, in meters
    };

//! Data type for the ETA Data Packet ID
//! \since Protocol A603
__packed struct eta_data_type /* D603 */
    {
    uint32                  unique_id;               //!< Uniquely identifies the ETA message
    time_type               eta_time;                //!< Estimated time of arrival, or 0xFFFFFFFF if no active destination
    uint32                  distance_to_destination; //!< Distance to destination, in meters, or 0xFFFFFFFF if no active destination
    sc_position_type        position_of_destination; //!< Location of destination
    };

//! Data type for the ETA Data Receipt Packet ID
//! \since Protocol A603
__packed struct eta_data_receipt_type /* D603 */
    {
    uint32 unique_id;                                //!< Unique ID from eta_data_type
    };

//! Data type for the Data Deletion Packet ID
//! \since Protocol A603
__packed struct data_deletion_data_type /* D603 */
    {
    uint32 data_type;                                //!< Type of data to delete, see del_data for valid values
    };
#endif // FMI_SUPPORT_A603

#if( FMI_SUPPORT_A604 )
//! Data type for the File Transfer Start Packet ID
//! \since Protocol A604
__packed struct  file_info_data_type /*  D604  */
    {
    uint32 file_size;                                //!< Size of the file, in bytes
    uint8  file_version_length;                      //!< Number of significant bytes in file_version
    uint8  file_type;                                //!< File type
    uint8  reserved[2];                              //!< Set to 0
    uint8  file_version[16];                         //!< Server-defined version string
    };

//! Data type for File Start Receipt Packet ID
//! and File End Receipt Packet ID
//! \since Protocol A604
__packed struct  file_receipt_data_type /*  D604  */
    {
    uint8  result_code;                              //!< Result of operation
    uint8  error_code_or_file_type_when_gpi;         //!< Set if result code is 5
    uint16 record_count_or_reserved_when_gpi;        //!< Useful for locating cause of errors
    };

//! Data type for File Start Receipt Packet ID
//! and File End Receipt Packet ID when sent from Server
//! \since Protocol A604
__packed struct  file_receipt_from_srvr_data_type /*  D604  */
    {
    uint8  result_code;                              //!< Result of operation
    uint8  file_type;                                //!< File type
    uint8  reserved[2];                              //!< Set to 0
    };

//! Data type for File Data Packet ID
//! \since Protocol A604
__packed struct  file_packet_data_type /*  D604  */
    {
    uint32 offset;            //!< offset of this data from the beginning of the file
    uint8  data_length;       //!< length of file_data (0..245)
    uint8  reserved[3];       //!< Set to 0
    uint8  file_data[245];    //!< file data, variable length
    };

//! Packet receipt for Packet Receipt Packet ID
//! \since Protocol A604
__packed struct packet_receipt_data_type /* D604 */
    {
    uint32 offset;            //!< offset of data received
    uint32 next_offset;       //!< offset of next data the server should send, or 0xFFFFFFFF for an error
    };

//! Data type for File Transfer End
//! \since Protocol A604
__packed struct file_end_data_type /* D604 */
    {
    uint32 crc;                       //!< CRC of entire file as computed by UTL_calc_crc32
    };

//! Data type for the A604 Server to Client Open Text Message Packet ID
//! \since Protocol A604
__packed struct A604_server_to_client_open_text_msg_data_type /* D604 */
    {
    time_type  origination_time;      //!< Time the message was sent from the server
    uint8      id_size;               //!< Number of significant bytes in the message ID
    uint8      message_type;          //!< Message type, a valid a604_message_type
    uint16     reserved;              //!< Set to 0
    uint8      id[16];                //!< Message ID
    char       text_message[ 200 ];   //!< Message text, variable length, null-terminated string, 200 bytes max
    };

//! Data type for the Server to Client Open Text Message Receipt Packet ID
//! \since Protocol A604
__packed struct    server_to_client_text_msg_receipt_data_type /* D604 */
    {
    time_type    origination_time;         //!< Origination time of the message being acknowledged
    uint8        id_size;                  //!< Size of the message ID
    boolean      result_code;              //!< Result code.  TRUE if success, FALSE otherwise
    uint16       reserved;                 //!< Set to 0
    uint8        id[16];                   //!< The message ID from the server to client open text message
    };

//! Data type for the Set Canned Response Packet ID
//! \since Protocol A604
__packed struct canned_response_data_type /*  D604  */
    {
    uint32   response_id;        //!< Unique ID of this canned response
    char     response_text[50];  //!< Response text to display on client (variable length, null terminated string)
    };

//! Data type for the Delete Canned Response Packet ID
//! \since Protocol A604
__packed struct canned_response_delete_data_type /* D604 */
    {
    uint32    response_id;       //!< The canned response ID to delete
    };

//! Data type for the Set Canned Response Receipt Packet ID
//! and Delete Canned Response Receipt Packet ID
//! \since Protocol A604
__packed struct canned_response_receipt_data_type /*  D604  */
    {
    uint32   response_id;        //!< The canned response ID from the set or delete
    boolean  result_code;        //!< True if the operation was successful
    uint8    reserved[3];        //!< Set to 0
    };

//! Data type for the Canned Response List Packet ID
//! \since Protocol A604
__packed struct  canned_response_list_data_type /*  D604  */
    {
    uint8      id_size;           //!< Size of the message ID
    uint8      response_count;    //!< Number of elements in response_id array
    uint16     reserved;          //!< Set to 0
    uint8      id[16];            //!< Message ID that this list is for
    uint32     response_id[50];   //!< List of responses that are allowed
    };

//! List of canned responses that the client requests updated text for
//! \since Protocol A604
__packed struct request_canned_response_list_refresh_data_type /*  D604  */
    {
    uint32 response_count;        //!< Number of responses in the array; if 0, all responses need refresh
    uint32 response_id[50];       //!< Canned response IDs
    };

//! Data type for Canned Response List Packet ID
//! \since Protocol A604
__packed struct canned_response_list_receipt_data_type /*  D604  */
    {
    uint8     id_size;            //!< id_size from the canned_response_list_data_type
    uint8     result_code;        //!< Enum indicating result code, see canned_response_list_result for valid values
    uint16    reserved;           //!< Set to 0
    uint8     id[16];             //!< Message ID from the canned_response_list_data_type
    };

//! Data type for Driver ID Update Packet ID
//! \since Protocol A604
__packed struct driver_id_data_type /* D604 */
    {
    uint32     status_change_id;   //!< Unique ID for this driver ID change
    time_type  status_change_time; //!< Time when the driver ID changed
    char       driver_id[50];      //!< New driver ID (null terminated string, 50 bytes max)
    };

#if( FMI_SUPPORT_A607 )
//! Data type for Driver ID Update Packet ID
//! \since Protocol A607
__packed struct driver_id_D607_data_type /* D607 */
    {
    uint32     status_change_id;   //!< Unique ID for this driver ID change
    time_type  status_change_time; //!< Time when the driver ID changed
    uint8      driver_idx;         //!< Driver index to change
    uint8      reserved[3];        //!< Set to 0
    char       driver_id[50];      //!< New driver ID (null terminated string)
    char       password[20];       //!< Driver password (null terminated string).  Optional if driver password support is not enabled.
    };
#endif

#if( FMI_SUPPORT_A607 )
//! Data type for Driver ID Request Packet ID
//! \note Prior to A607, the Driver ID Request Packet had no payload
//! \since Protocol A607
__packed struct driver_id_request_data_type /* D607 */
{
    uint8      driver_idx;         //!< Driver index to change
    uint8      reserved[ 3 ];      //!< Set to 0
};
#endif

//! Data type for Driver ID Receipt packet
//! \since Protocol A604
__packed struct driver_id_receipt_data_type /* D604 */
    {
    uint32  status_change_id;      //!< status_change_id from the driver_id_data_type
    boolean result_code;           //!< True if the update was successful
    uint8   driver_idx;            //!< Index of driver changed
    uint8   reserved[2];           //!< Set to 0
    };

//! Data type for the Set Driver Status List Item packet
//! \since Protocol A604
__packed struct  driver_status_list_item_data_type /* D604 */
    {
    uint32   status_id;            //!< Unique identifier and sort key for the status item
    char     status[50];           //!< Text displayed for the item (variable length, null terminated, 50 bytes max)
    };

//! Data type for the Set Driver Status List Item
//! and Delete Driver Status List Item Receipt packets
//! \since Protocol A604
__packed struct  driver_status_list_item_receipt_data_type /* D604 */
    {
    uint32  status_id;              //!< status_id from the driver_status_list_item_data_type or driver_status_list_item_delete_data_type
    boolean result_code;            //!< True if the update was successful
    uint8   driver_idx;             //!< Index of driver changed
    uint8   reserved[2];            //!< Set to 0
    };

//! Data type for Delete Driver Status List Item Receipt
//! \since Protocol A604
__packed struct driver_status_list_item_delete_data_type /* D604 */
    {
    uint32  status_id;              //!< ID for the driver status list item to delete
    };

//! Data type for the Driver Status Update packet
//! \since Protocol A604
__packed struct  driver_status_data_type /* D604 */
    {
    uint32           status_change_id;    //!< unique identifier
    time_type        status_change_time;  //!< timestamp of status change
    uint32           driver_status;       //!< ID corresponding to the new driver status
    };

//! Data type for the A607 Driver Status Update packet
//! \since Protocol A607
__packed struct  driver_status_D607_data_type /* D607 */
    {
    uint32           status_change_id;    //!< unique identifier
    time_type        status_change_time;  //!< timestamp of status change
    uint32           driver_status;       //!< ID corresponding to the new driver status
    uint8            driver_idx;          //!< Index of driver to change
    uint8            reserved[ 3 ];       //!< Set to zero
    };

//! Data type for the Driver Status Update Receipt packet
//! \since Protocol A604
__packed struct driver_status_receipt_data_type /* D604 */
    {
    uint32       status_change_id;        //!< status_change_id from the driver_status_data_type
    boolean      result_code;             //!< True if the update was successful
    uint8        driver_idx;              //!< Index of the driver to update
    uint8        reserved[ 2 ];           //!< Set to 0
    };

#if( FMI_SUPPORT_A607 )
//! Data type for Driver Status Request Packet ID
//! \since Protocol A607
__packed struct driver_status_request_data_type /* D607 */
{
    uint8      driver_idx;         //!< Driver index requested
    uint8      reserved[ 3 ];      //!< Set to 0
};
#endif

//! Data type for the Set Canned Message Packet ID
//! \since Protocol A604
__packed struct canned_message_data_type /*  D604  */
    {
    uint32     message_id;                //!< Unique identifier and sort key for this canned message
    char       message[50];               //!< Message text, variable length, null terminated (50 bytes max)
    };

//! Data type for the Delete Canned Message Packet ID
//! \since Protocol A604
__packed struct canned_message_delete_data_type /*  D604  */
    {
    uint32 message_id;                    //!< ID of the canned message to delete
    };

//! Data type for the Set Canned Message Receipt Packet ID
//! and Delete Canned Message Receipt Packet ID
//! \since Protocol A604
__packed struct canned_message_receipt_data_type /*  D604  */
    {
    uint32  message_id;                   //!< ID of the canned message
    boolean result_code;                  //!< Result (true if successful, false otherwise)
    uint8   reserved[3];                  //!< Set to 0
    };

//! Data type for the Message Status Request Packet ID
//! \since Protocol A604
__packed struct message_status_request_data_type /*  D604  */
    {
    uint8 id_size;                        //!< Number of significant bytes in the message ID
    uint8 reserved[3];                    //!< Set to 0
    uint8 id[ 16 ];                       //!< The message ID
    };

//! Data type for the Message Status Packet ID
//! \since Protocol A604
__packed struct message_status_data_type /*  D604  */
    {
    uint8 id_size;                        //!< Number of significant bytes in the message ID
    uint8 status_code;                    //!< Message status, see fmi_A604_message_status for valid values
    uint16 reserved;                      //!< Set to 0
    uint8 id[ 16 ];                       //!< The message ID
    };

//! Data type for the User Interface Text Packet ID
//! \since Protocol A604
__packed struct user_interface_text_data_type /*  D604  */
    {
    uint32 text_element_id;               //!< ID of the user interface element being changed
    char   new_text[50];                  //!< Text to display
    };

//! Data type for the User Interface Text Receipt Packet ID
//! \since Protocol A604
__packed struct user_interface_text_receipt_data_type /*  D604  */
    {
    uint32  text_element_id;              //!< text_element_id from the user_interface_text_data_type
    boolean result_code;                  //!< True if the update was successful
    uint8   reserved[3];                  //!< Set to 0
    };

//! Data type for the Message Throttling Command Packet ID
//! and Message Throttling Response Packet ID
//! \since Protocol A604
__packed struct message_throttling_data_type /*  D604  */
    {
    uint16  packet_id;                    //!< First packet ID in the protocol to throttle
    uint16  new_state;                    //!< New state, see message_throttling_state_type for valid values
    };

//! Data type for the Text Message Ack Receipt Packet ID
//! \since Protocol A604
__packed struct text_msg_id_data_type /* D604 */
    {
    uint8    id_size;                    //!< Size of the message ID
    uint8    reserved[3];                //!< Set to 0
    uint8    id[16];                     //!< Message ID
    };
#endif   // FMI_SUPPORT_A604

#if( FMI_SUPPORT_A605 )
//! Data type for the Message Throttling Query Response Packet ID
//! \since Protocol A605
__packed struct message_throttling_list_data_type /*  D605  */
    {
    uint16                       response_count;     //!< Number of protocols in the response_list
    message_throttling_data_type response_list[60];  //!< One element for each protocol with ID and state
    };
#endif

#if( FMI_SUPPORT_A606 )
//! Data type for the FMI Safe Mode setup Packet ID
//! \since Protocol A606
__packed struct safe_mode_speed_data_type /*  D606  */
    {
    float32  speed;                       //!< FMI safe mode speed
    };

//! Data type for the User Interface Text Receipt Packet ID
//! \since Protocol A606
__packed struct safe_mode_speed_receipt_data_type /*  D606  */
    {
    boolean result_code;                  //!< True if the update was successful
    uint8   reserved[3];                  //!< Set to 0
    };
#endif

#if( FMI_SUPPORT_A608 )
//! Data type for the Speed Limit Alert Packet ID
//! \since Protocol A608
__packed struct speed_limit_alert_data_type
    {
    uint8            category;            //!< Alert category, a valid speed_limit_alert_category_type
    uint8            reserved[3];         //!< Set to 0
    sc_position_type posn;                //!< Position at the time of alert
    time_type        timestamp;           //!< Time the alert was generated
    float            speed;               //!< Speed at the time of alert
    float            speed_limit;         //!< Speed limit at the time of alert
    float            max_speed;           //!< Maximum speed since last alert
    };

//! Data type for the Speed Limit Alerts setup Packet ID
//! \since Protocol A608
__packed struct speed_limit_data_type
    {
    uint8            mode;                //!< Mode, a valid speed_limit_alert_mode_type
    uint8            time_over;           //!< Seconds until speeding event begins
    uint8            time_under;          //!< Seconds until speeding event ends
    boolean          alert_user;          //!< Audibly alert the driver
    float            threshold;           //!< Speed over speed limit when speeding event begins
    };

//! Data type for the Speed Limit Alerts setup Receipt Packet ID
//! \since Protocol A608
__packed struct speed_limit_receipt_data_type
    {
    uint8            result_code;         //!< Result code, a valid speed_limit_alert_result_data_type
    uint8            reserved[3];         //!< Set to 0
    };

//! Data type for the Speed Limit Alert Receipt Packet ID
//! \since Protocol A608
__packed struct speed_limit_alert_receipt_data_type
    {
    time_type       timestamp;            //!< Timestamp of the alert that is being acknowledged
    };
#endif

#if( FMI_SUPPORT_A607 )
//! Data type for FMI_ID_WAYPOINT packet
//! \since Protocol A607
__packed struct waypoint_data_type
{
    uint16                       unique_id;         //!< Server-assigned unique ID for the waypoint
    uint16                       symbol;            //!< Waypoint symbol
    sc_position_type             posn;              //!< Waypoint position
    uint16                       cat;               //!< Waypoint categories, bit-mapped
    char                         name[ 30 + 1 ];    //!< Waypoint name, null-terminated
    char                         comment[ 50 + 1 ]; //!< Waypoint comment, null-terminated
};

//! Data type for the FMI_ID_WAYPOINT_RCPT packet
//! \since Protocol A607
__packed struct waypoint_rcpt_data_type
{
    uint16                       unique_id;         //!< Server-assigned unique ID from the FMI_ID_WAYPOINT packet
    boolean                      result_code;       //!< TRUE if the operation was successful, FALSE otherwise
    uint8                        reserved;          //!< Set to 0
};

//! Data type for the FMI_ID_WAYPOINT_DELETED packet
//! \since Protocol A607
typedef waypoint_rcpt_data_type waypoint_deleted_data_type;

//! Data type for the FMI_ID_CREATE_WAYPOINT_CAT packet
//! \since Protocol A607
__packed struct category_data_type
{
    uint8                        id;                //!< Waypoint category
    char                         name[16 + 1];      //!< Category name, null terminated
};

//! Data type for the FMI_ID_CREATE_WAYPOINT_CAT_RCPT packet
//! \since Protocol A607
__packed struct category_rcpt_data_type
{
    uint8                        id;                //!< Waypoint category (0-15)
    boolean                      result_code;       //!< TRUE if the operation was successful, FALSE otherwise
};

//! Data type for the FMI_ID_DELETE_WAYPOINT_CAT_RCPT packet
__packed struct delete_by_category_rcpt_data_type
{
    uint16                       cat_id;            //!< Category that was deleted (0-15)
    uint16                       count;             //!< Number of items deleted
};

//! Data type for the Message Status Request Packet ID
//! \since Protocol A607
__packed struct delete_message_request_data_type /*  D607  */
{
    uint8                        id_size;           //!< Number of significant bytes in the message ID
    uint8                        reserved[3];       //!< Set to 0
    uint8                        id[ 16 ];          //!< The message ID
};

//! Data type for the Delete Message Status Packet ID
//! \since Protocol A607
__packed struct delete_message_response_data_type /*  D607  */
{
    uint8                        id_size;           //!< Number of significant bytes in the message ID
    boolean                      result_code;       //!< TRUE if message was deleted, FALSE if message was not found
    uint16                       reserved;          //!< Set to 0
    uint8                        id[ 16 ];          //!< The message ID
};
#endif

#if( FMI_SUPPORT_A610 )

//! Data type for the Set Odometer Request Packed ID (0X1100) from server to client
//! \since Protocol A610
__packed struct set_odometer_request_data_type /* D610 */
{
    uint32         odometer_value;
};

//! Data type for the Driver Login Data Packet ID (0X1101) from client to server
//! \since Protocol A610
__packed struct driver_login_service_data_type /* D610 */
{
    time_type      ui_timestamp;
    char           driver_password[ 20 ];
    char           driver_id[ 50 ];
};

//! Data type for the Driver Login Data Packet ID (0X1102) from server to client
//! \since Protocol A610
__packed struct driver_login_service_receipt /* D610 */
{
    time_type      ui_timestamp;
    uint8          result_code;
};

//! Data type for the Driver Profile Data Packet ID (0X1103) from client to server
//! \since Protocol A610
__packed struct fmi_driver_profile_type /* D610 */
{
    char           driver_id[ 50 ];
};

//! Data type for the Driver Profile Data Packet ID (0X1104) from server to client
//! \since Protocol A610
__packed struct fmi_driver_profile_data_type /* D610 */
{
    uint32         pin;
    char           first_name[ 35 ];
    char           last_name[ 35 ];
    char           driver_id[ 40 ];
    char           carrier_name[ 120 ];
    char           carrier_id[ 8 ];
    uint8          long_term_rule_set;
    uint8          time_zone;
    uint8          status;
    uint8          result_code;
};

//! Data type for the Driver Profile Update Response Data Packet ID (?) from client to server
//! \since Protocol A610
__packed struct fmi_ack_type
{
    uint8          result_code;
    char           driver_id[ 40 ];
};

//! Data type for the Driver Profile Update Data Packet ID (0X1105) from server to client
//! \since Protocol A610
__packed struct fmi_update_driver_profile_request_data_type
{
    char           driver_id[ 40 ];
};

//! Data type for the Driver Status Log Update Data Packet ID (0X1106) from client to server
//! \since Protocol A610
__packed struct fmi_driver_status_update_request_data_type
{
    char           driver_id[ 40 ];
};

//! Data type for the Driver Status Log response Packet ID (0X1107) from server to client
//! \since Protocol A610
__packed struct fmi_driver_status_log_dnld_resp_data_type /* D610 */
{
    char            driver_id[40];
    uint8           result_code;
};

//! Data type for the Driver Status Log Receipt Packet ID (0X1108) from client to server
//! \since Protocol A610
__packed struct fmi_status_log_download_receipt_data_type
{
    char            driver_id[ 40 ];
    uint8           result_code;
};

//! Data type for the Shipment Download Response Packet ID (0X110A) from server to client
//! \since Protocol A610
__packed struct fmi_driver_shipment_data_type
{
    time_type   server_timestamp;
    time_type   server_start_time;
    time_type   server_end_time;
    char        server_shipper_name[ 40 ];
    char        server_doc_number[ 40 ];
    char        server_commodity[ 40 ];
    char        driver_id[ 40 ];
    uint8       server_result_code;
};

//! Data type for the Shipment Download Receipt Packet ID (0X110B) from client to server
//! \since Protocol A610
__packed struct fmi_shipment_download_receipt_data_type
{
    char            driver_id[ 40 ];
    uint8           result_code;
};

//! Data type for the Annotation Download Request Packet ID (0X110C) from client to server
//! \since Protocol A610
__packed struct fmi_annotation_download_request_data_type
{
    char           driver_id[ 50 ];
};

//! Data type for the Annotation Download Response Packet ID (0X110D) from server to client
//! \since Protocol A610
__packed struct fmi_driver_annotation_data_type
{
    time_type  server_timestamp;
    time_type  server_start_time;
    time_type  server_end_time;
    char       server_annotation[ 60 ];
    char       driver_id[ 40 ];
    uint8      server_result_code;
};

//! Data type for the Annotation Download Receipt Packet ID (0X110E) from client to server
//! \since Protocol A610
__packed struct fmi_driver_aobrd_ack_data_type /* D610 */
{
    char                                driver_id[ 40 ];
    aobrd_download_receipt_result_code  result_code;
};
#endif

#if( FMI_SUPPORT_A611 )

//! Data type for Long Text Message Request Packet ID (0X0055) from server to client
//! \since Protocol A611
__packed struct long_text_msg_data_type
{
    /* Time sent from server */
    time_type           origination_time;

    /* Number of characters used in the id member */
    uint8               id_size;

    /* 0 = Floating button for received message, 1 = Display immediately */
    uint8               type;

    /* 0 = Indicates additional packet, 1 = Indicates final packet segment */
    boolean             finished_flag;

    /* Indicates packet segment number of a long message, 0 = First packet */
    uint8               sequence_number;

    /* id_size, 16 characters max */
    uint8               id[ 16 ];

    /* 200 bytes, or 200 or less variable length for final segment */
    char                text_message[ LONG_TEXT_MSG_CHUNK_SIZE ];
};

//! Data type for Long Test Message Receipt Packet ID (0X0056) from client to server
//! \since Protocol A611
__packed struct long_text_msg_receipt_data_type /* D611 */
{
    time_type           origination_time;     /* Original time sent from Server */
    uint8               id_size;              /* Number of characters used in the id member */
    uint8               result_code;          /* 0 = Message accepted, non-zero = Error occurred or information */
    boolean             finished_flag;        /* 0 = Indicates additional packet, 1 = Indicates final packet */
    uint8               sequence_number;      /* Indicates packet number of a long message, 0 = First packet */
    uint8               id[ 16 ];             /* id_size, 16 characters max */
};

#endif

#if( FMI_SUPPORT_A612 )

//! Data type for Custom Forms Packet ID (0X1201) from client to server
//! \since Protocol A612
__packed struct custom_form_delete_ack_type /* D612 */
{
    uint32      form_id;            /* ID of deleted form */
    uint8       return_code;        /* Result code, 0 = No errors */
};

//! Data type for Custom Forms Packet ID (0X1202) from client to server
//! \since Protocol A612
__packed struct custom_form_move_type /* D612 */
{
    uint32      form_id;            /* ID of form to move */
    uint32      new_position;       /* Position to move form to */
};

//! Data type for Custom Forms Packet ID (0X1203 and 0X1205) from client to server
//! \since Protocol A612
__packed struct custom_form_position_ack_type /* D612 */
{
    uint32      form_id;            /* ID of deleted form */
    uint8       current_position;   /* Actual position after move */
    uint8       return_code;        /* Result code, 0 = No errors */
};

#endif

#if( FMI_SUPPORT_A614 )
//! Data type for Route Calculation ACK Packet ID (0X1220) from client to server
//! \since Protocol A614
__packed struct stop_calc_ack_type /* D614 */
{
    uint32      unique_id;          /* ID of the stop */
    uint32      distance;           /* Distance to destination, in meters, or 0xFFFFFFFF if there is an error */
    uint8       result_code;        /* Result code, 0 = No errors */
};

//! Data type for Route Calculation ACK Receipt Packet ID (0X1221) from server to client
//! \since Protocol A614
__packed struct stop_calc_ack_rcpt_type /* D614 */
{
    uint32      unique_id;          /* ID of the stop */
};
#endif

#if( FMI_SUPPORT_A613 )
//! Data type for Custom Avoidance Feature Enable Packet ID (0X1236, 0X1235)
//! \since Protocol A613
__packed struct custom_avoid_feature_enable_type
{
    time_type   origination_time;   /* The time sent from the server */
    boolean     enable;             /* 0 = Disable, 1 = Enable Custom Avoidance feature */
};

//! Data type for Custom Avoidance Packet ID (0X1230) from server to client
//! \since Protocol A613
__packed struct custom_avoid_type /* D613 */
{
    sc_position_type    point1;     /* Coordinates for NorthEast corner         */
    sc_position_type    point2;     /* Coordinates for SouthWest corner         */
    uint16              unique_id;  /* Server-assigned unique ID for the avoidance */
    boolean             enable;     /* 0 = disable, 1 = enable the custom avoidance */
    uint8               reserved;   /* Set to 0 */
    char                name[ 49 ]; /* 49 bytes, null-terminated */
};

//! Data type for Custom Avoidance Receipt ID (0X1231) from client to server
//! \since Protocol A613
__packed struct custom_avoid_rcpt_type /* D613 */
{
    uint16          unique_id;      /* Server-assigned unique ID */
    uint8           result_code;    /* 0 = success, non-zero indicates error (see below) */
};

//! Data type for Custom Avoidance Delete (0X1232) from server to client
//! \since Protocol A613
__packed struct custom_avoid_delete_type /* D613 */
{
    uint16          unique_id;      /* Server-assigned unique ID to be deleted */
};

//! Data type for Custom Avoidance Enable/Disable (0X1234) from server to client
//! \since Protocol A613
__packed struct custom_avoid_enable_type /* D613 */
{
    uint16          unique_id;      /* Server-assigned unique ID */
    boolean         enable;         /* 0 = Avoidance Area deleted, 1 = Unique ID not found */
};

#endif

#if( FMI_SUPPORT_A615 )
//! Data type for the IFTA Data Fetch Request Packet ID (0X0006) from server to client
//! \since Protocol A615
__packed struct fmi_ifta_data_fetch_request_type /* D610 */
{
    time_type           start_time;         //Timestamp for start of range, in Garmin time
    time_type           end_time;           //Timestamp for end of range, in Garmin time
};

//! Data type for the IFTA Data Fetch Receipt Packet ID (0X0007) from client to server
//! \since Protocol A615
__packed struct fmi_ifta_data_fetch_receipt_type /* D610 */
{
    uint8               result_code;        // 0 = No errors, non-zero value for failures (see IFTA_data_mod_result_code_type)
};

//! Data type for the IFTA Data Delete Request Packet ID (0X0008) from server to client
//! \since Protocol A615
__packed struct fmi_ifta_data_delete_request_type /* D610 */
{
    time_type           start_time;         //Timestamp for start of range, in Garmin time
    time_type           end_time;           //Timestamp for end of range, in Garmin time
};

//! Data type for the IFTA Data Delete Receipt Packet ID (0X0009) from client to server
//! \since Protocol A615
__packed struct fmi_ifta_data_delete_receipt_type /* D610 */
{
    uint8               result_code;        // 0 = No errors, non-zero value for failures (see IFTA_data_mod_result_code_type)
};

//! Data type for the Driver Profile Data Packet ID (0X1110) from server to client
//!  and Driver Profile Data Push Packet ID (0x1111) from server to client
//! \since Protocol A615
__packed struct fmi_driver_profile_data_type_V2 /* D615 */
{
    char           first_name[ 35 ];
    char           last_name[ 35 ];
    char           driver_id[ 40 ];
    char           carrier_name[ 120 ];
    char           carrier_id[ 8 ];
    uint8          long_term_rule_set;
    uint8          load_type_rule_set;
    time_type      adverse_condition_time;
    uint8          time_zone;
    uint8          status;
    uint8          result_code;
};

//! Data type for the HOS Auto Status Update Enable Packet ID (0X1300) from server to client
//! \since Protocol A615
__packed struct fmi_hos_auto_status_update_feature /* D615 */
{
    uint16         stop_moving_threshold;  /* seconds */
    boolean        enable;                 /* 1 = enabled, 0 = disabled */
};

//! Data type for the HOS Auto Status Update Receipt Packet ID (0X1301) from client to server
//! \since Protocol A615
__packed struct fmi_hos_auto_status_update_feature_rcpt /* D615 */
{
    uint16         stop_moving_threshold;  /* seconds */
    boolean        enable;                 /* 1 = enabled, 0 = disabled */
    uint8          result_type;            /* see HOS_auto_status_update_result_code_type */
};

//! Data type for AOBRD Driver Logoff Request Packet ID (0X1310) from server to client
//! \since Protocol A615
__packed struct fmi_logoff_driver_request /* D615 */
{
    char            driver_id[ 40 ];    /* unique identifier of the driver */
};

//! Data type for AOBRD Driver Logoff Receipt Packet ID (0X1311) from client to server
//! \since Protocol A615
__packed struct fmi_logoff_driver_receipt /* D615 */
{
    char            driver_id[ 40 ];    /* unique identifier of the driver */
    uint8           status;
    uint8           result_code;        /* 0 = success, non-zero indicates error (see below) */
};

//! Data type for Change Status HOSART Packet ID (0X0000)
//! \since Protocol A615
__packed struct hosart_change_status
{
    uint32    driver_id;
    uint32    new_status;
};

//! Data type for Change to Driving Warning HOSART Packet ID (0X0001)
//! \since Protocol A615
__packed struct hosart_change_to_driving_warning
{
    uint32    warning_type;
    uint32    violation_type;
    uint32    driver_id;
    time_type timestamp;
};

//! Data type for Warning Found and Violation Found HOSART Packet IDs (0X0002,0X0003)
//! \since Protocol A615
__packed struct hosart_violation_found
{
    uint32    violation_type;
    uint32    driver_id;
    time_type timestamp;
};
#endif

#if( FMI_SUPPORT_A616 )
//! Data type for Baud Rate Change Request Packet ID (0x0011) from server to client
//! \since Protocol A616
__packed struct fmi_set_baud_request /* D616 */
{
    uint8           request_type;       /* see fmi_baud_request_type enum */
    uint8           baud_rate_type;     /* see fmi_baud_rate_code_type enum */
};

//! Data type for Baud Rate Change Receipt Packet ID (0x0012) from client to server
//! \since Protocol A616
__packed struct fmi_set_baud_receipt /* D617 */
{
    uint8           result_code;        /* see fmi_baud_result_code_type enum */
    uint8           request_type;       /* see fmi_baud_request_type enum */
    uint8           baud_rate_type;     /* see fmi_baud_rate_code_type enum */
};
#endif

#if( FMI_SUPPORT_A617 )
//! Data type for Alert Popup Request Packet ID (0x1400) from server to client
//! \since Protocol A617
__packed struct fmi_alert_popup_request /* D617 */
{
    uint16      unique_id;          /* unique identifier for the alert popup */
    uint16      icon;               /* Icon selected from Garmin pre-loaded icons */
    uint8       timeout;            /* Popup time out in seconds, max 15 seconds, 0 default to 15 seconds  */
    uint8       severity;           /* 0 = normal, 1 = medium, 2 = high  */
    boolean     play_sound;         /* Play sound to driver when updated */
    char        alert_text[110];    /* null-terminated text, 110 bytes max including null terminator */
};

//! Data type for Alert Popup Receipt Packet ID (0x1401) from client to server
//! \since Protocol A617
__packed struct fmi_alert_popup_receipt /* D616 */
{
    uint16      unique_id;          /* Server-assigned unique ID */
    uint8       result_code;        /* 0 = success, non-zero indicates error (see below) */
};

//! Data type for Configure Sensor Request Packet ID (0x1402) from server to client
//! \since Protocol A617
__packed struct fmi_sensor_config_request /* D617 */
{
    uint32      change_id;          /* unique identifier for this message */
    uint32      unique_id;          /* unique identifier for the sensor */
    uint16      icon;               /* Icon selected from Garmin pre-loaded icons */
    uint8       display_index;      /* sensor sorting index used to order sensor */
    uint8       reserved[3];        /* set to 0 */
    char        name[40];           /* name of the sensor null-terminated text, 40 bytes max including null terminator */
};

//! Data type for Configure/Update/Delete Sensor Receipt Packet IDs (0x1403,0x1405) from client to server
//! \since Protocol A617
__packed struct fmi_sensor_receipt /* D617 */
{
    uint32      change_id;          /* unique identifier from the original server request */
    uint8       result_code;        /* 0 = success, non-zero indicates error (see below) */
    uint8       operation_mode;     /* 0 = delete, 1 = add, 2 = modify */
};

//! Data type for Delete Sensor Request Packet ID (0x1404) from server to client
//! \since Protocol A617
__packed struct fmi_sensor_delete_request /* D617 */
{
    uint32      change_id;          /* unique identifier of this message */
    uint32      unique_id;          /* Server-assigned unique ID */
};

//! Data type for Update Sensor Request Packet ID (0x1406) from server to client
//! \since Protocol A617
__packed struct fmi_sensor_update_request /* D617 */
{
    uint32      change_id;          /* unique identifier of this message */
    uint32      unique_id;          /* unique identifier for the sensor */
    uint8       severity;           /* 0 = normal, 1 = medium, 2 = high  */
    boolean     play_sound;         /* Play sound to driver when updated */
    boolean     record_sensor;      /* whether to record this to the history log */
    uint8       reserved[3];        /* set to 0 */
    char        status[80];         /* current status of the sensor null-terminated text, 80 bytes max including null terminator */
    char        description[110];   /* null-terminated text, 110 bytes max including null terminator */
};

//! Data type for Query Sensor Display Index Request Packet ID (0x1407) from server to client
//! \since Protocol A617
__packed struct fmi_sensor_query_display_index_request /* D617 */
{
    uint32      change_id;          /* unique identifier of this message */
    uint32      unique_id;          /* unique identifier for the sensor } */
};

//! Data type for Query Sensor Display Index Receipt Packet ID (0x1408) from client to server
//! \since Protocol A617
__packed struct fmi_sensor_query_display_index_receipt /* D617 */
{
    uint32      change_id;          /* unique identifier of this message */
    uint8       result_code;        /* 0 = success, non-zero indicates error */
    uint8       display_index;      /* sensor current display index */
};
#endif

#if( FMI_SUPPORT_A619 )
//! Data type for HOS Settings Request Packet ID (0x1500) from server to client
//! \since Protocol A619
__packed struct fmi_hos_set_settings_request_data_type /* D619 */
{
    uint16      tag;                /* HOS_setting_tag_type defining which setting is to be set */
    uint16      settings_value;     /* value to set setting to */
    boolean     enable;             /* enable or disable the feature */
};

//! Data type for HOS Settings Receipt Packet ID (0x1501) from client to server
//! \since Protocol A619
__packed struct fmi_hos_set_settings_receipt_data_type /* D619 */
{
    uint16      tag;                    /* HOS_setting_tag_type defining which setting was set */
    uint16      settings_value;         /* value of setting after attempt to set */
    boolean     enabled;                /* enabled or disabled state of the feature */
    uint8       result_code;            /* HOS_settings_result_code_type describing the outcome of the set */
};
#endif

#include "pack_end.h"

#endif  /* _FMI_H_ */
