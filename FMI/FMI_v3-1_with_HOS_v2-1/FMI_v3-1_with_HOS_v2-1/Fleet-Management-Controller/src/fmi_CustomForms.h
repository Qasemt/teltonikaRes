/*********************************************************************
*
*   HEADER NAME:
*       fmi_CustomForms.h - Protocol info for custom forms
*
* Copyright 2008-2013 by Garmin Ltd. or its subsidiaries.
*---------------------------------------------------------------------
* $NoKeywords$
*********************************************************************/
#ifndef FMI_CF_H
#define FMI_CF_H

#if( FMI_SUPPORT_A612 )

#define FMI_CF_SCHEMA_ERR_BASE   50
#define FMI_CF_XSP_ERR_BASE     200

enum FMI_cf_rcode {
    FMI_CF_RCODE_OK,                                   //!< [0]   OK result code
    FMI_CF_RCODE_GFS_ERROR,                            //!< [1]   GFS error result code
    FMI_CF_RCODE_SQL_ERROR,                            //!< [2]   SQL error result code
    FMI_CF_RCODE_SQL_PREPARE_FAILED,                   //!< [3]   SQL prepare statement failed
    FMI_CF_RCODE_SQL_BIND_FAILED,                      //!< [4]   SQL bind parameters failed
    FMI_CF_RCODE_SQL_EXEC_FAILED,                      //!< [5]   SQL step (query/execution) failed
    FMI_CF_RCODE_SQL_TRANSACTION_ALREADY_STARTED,      //!< [6]   SQL transaction has already been started
    FMI_CF_RCODE_SQL_NUMBER_OF_FORMS_LIMIT_EXCEEDED,   //!< [7]   SQL number of forms exceeded supported limit
    FMI_CF_RCODE_SQL_POSITION_FREE_FAILED,             //!< [8]   SQL free form position failed
    FMI_CF_RCODE_SQL_POSITION_PACK_FAILED,             //!< [9]   SQL pack form positions failed
    FMI_CF_RCODE_SQL_ITEM_EXTRAS_NULL_TYPE,            //!< [10]  SQL item extras data type is NULL and cannot be used for query
    FMI_CF_RCODE_SQL_ITEM_EXTRAS_UNEXPECTED_SIZE,      //!< [11]  SQL item extras blob and type sizes are not the same
    FMI_CF_RCODE_SQL_OLDEST_SAVED_DELETE_FAILED,       //!< [12]  SQL oldest saved form deletion failed
    FMI_CF_RCODE_SQL_NO_UNSENT_FORMS,                  //!< [13]  SQL no unsent forms are in the database
    FMI_CF_RCODE_SQL_SUBMITTED_STRING_POINTER_NULL,    //!< [14]  SQL submitted string pointer is NULL and cannot be used for query
    FMI_CF_RCODE_SQL_SUBMITTED_STRING_UNEXPECTED_SIZE, //!< [15]  SQL data blob and submitted string sizes are not the same
    FMI_CF_RCODE_SQL_FORM_TEMPLATE_NOT_FOUND,          //!< [16]  SQL form template is not found in the database

    FMI_CF_RCODE_SCHEMA_BASE = FMI_CF_SCHEMA_ERR_BASE, //!< [50]  Base value for schema error result codes - USED FOR ENUM ALIGNMENT ONLY
    FMI_CF_RCODE_SCHEMA_FORM_ELMT_INCOMPLETE,          //!< [51]  Form element is incomplete - required values are missing
    FMI_CF_RCODE_SCHEMA_ITEM_ELMT_INCOMPLETE,          //!< [52]  Item element is incomplete - required values are missing
    FMI_CF_RCODE_SCHEMA_ITEM_PARENT_INVALID,           //!< [53]  An item elements parent is not a form element
    FMI_CF_RCODE_SCHEMA_ITEM_ID_REUSE,                 //!< [54]  Item ID is used more than one time
    FMI_CF_RCODE_SCHEMA_ITEM_CNT_OUT_OF_RANGE,         //!< [55]  Item count is out of range for a form
    FMI_CF_RCODE_SCHEMA_TYPE_PARENT_INVALID,           //!< [56]  A type elements parent is not an item element
    FMI_CF_RCODE_SCHEMA_TYPE_CNT_OUT_OF_RANGE,         //!< [57]  Type count is out of range for an item (> 1)
    FMI_CF_RCODE_SCHEMA_TEXT_LEN_OUT_OF_RANGE,         //!< [58]  Text length is out of range
    FMI_CF_RCODE_SCHEMA_INT_LEN_OUT_OF_RANGE,          //!< [59]  Integer length is out of range
    FMI_CF_RCODE_SCHEMA_INT_MIN_OUT_OF_RANGE,          //!< [60]  Minimum integer value is out of range
    FMI_CF_RCODE_SCHEMA_INT_MAX_OUT_OF_RANGE,          //!< [61]  Maximum integer value is out of range
    FMI_CF_RCODE_SCHEMA_INT_MIN_GRT_THAN_MAX,          //!< [62]  Minimum integer value is greater than the maximum integer value
    FMI_CF_RCODE_SCHEMA_OPT_ELMT_INCOMPLETE,           //!< [63]  Option element is incomplete - required values are missing
    FMI_CF_RCODE_SCHEMA_OPT_PARENT_INVALID,            //!< [64]  An option elements parent is not a single select or multiple select element
    FMI_CF_RCODE_SCHEMA_OPT_ID_REUSE,                  //!< [65]  Option ID is used more than one time for a given item
    FMI_CF_RCODE_SCHEMA_OPT_CNT_OUT_OF_RANGE,          //!< [66]  Option count is out of range for an item
    FMI_CF_RCODE_SCHEMA_MONTH_OUT_OF_RANGE,            //!< [67]  Month value is out of range
    FMI_CF_RCODE_SCHEMA_DAY_OUT_OF_RANGE,              //!< [68]  Day value is out of range
    FMI_CF_RCODE_SCHEMA_YEAR_OUT_OF_RANGE,             //!< [69]  Year value is out of range
    FMI_CF_RCODE_SCHEMA_INVALID_DAYS_IN_MONTH,         //!< [70]  Number of days in the month is not valid
    FMI_CF_RCODE_SCHEMA_NO_DATE_SET,                   //!< [71]  Use current is set to false and no date was specified
    FMI_CF_RCODE_SCHEMA_HOUR_OUT_OF_RANGE,             //!< [72]  Hour value is out of range
    FMI_CF_RCODE_SCHEMA_MINUTE_OUT_OF_RANGE,           //!< [73]  Minute value is out of range
    FMI_CF_RCODE_SCHEMA_SECOND_OUT_OF_RANGE,           //!< [74]  Second value is out of range
    FMI_CF_RCODE_SCHEMA_NO_TIME_SET,                   //!< [75]  Use current is set to false and no time was specified
    FMI_CF_RCODE_SCHEMA_UNKNOWN_ITEM_TYPE,             //!< [76]  Unknown form item type

    FMI_CF_RCODE_XSP_BASE = FMI_CF_XSP_ERR_BASE,       //!< [200] Base value for XSP error result codes - USED FOR ENUM ALIGNMENT ONLY
    FMI_CF_RCODE_XSP_NO_MEMORY,                        //!< [201] 1  - Out of memory
    FMI_CF_RCODE_XSP_SYNTAX,                           //!< [202] 2  - Syntax error
    FMI_CF_RCODE_XSP_NO_ELEMENTS,                      //!< [203] 3  - No element found
    FMI_CF_RCODE_XSP_INVALID_TOKEN,                    //!< [204] 4  - Not well-formed (invalid token)
    FMI_CF_RCODE_XSP_UNCLOSED_TOKEN,                   //!< [205] 5  - Unclosed token
    FMI_CF_RCODE_XSP_PARTIAL_CHAR,                     //!< [206] 6  - Partial character
    FMI_CF_RCODE_XSP_TAG_MISMATCH,                     //!< [207] 7  - Mismatched tag
    FMI_CF_RCODE_XSP_DUPLICATE_ATTRIBUTE,              //!< [208] 8  - Duplicate attribute
    FMI_CF_RCODE_XSP_JUNK_AFTER_DOC_ELEMENT,           //!< [209] 9  - Junk after document element
    FMI_CF_RCODE_XSP_PARAM_ENTITY_REF,                 //!< [210] 10 - Illegal parameter entity reference
    FMI_CF_RCODE_XSP_UNDEFINED_ENTITY,                 //!< [211] 11 - Undefined entity
    FMI_CF_RCODE_XSP_RECURSIVE_ENTITY_REF,             //!< [212] 12 - Recursive entity reference
    FMI_CF_RCODE_XSP_ASYNC_ENTITY,                     //!< [213] 13 - Asynchronous entity
    FMI_CF_RCODE_XSP_BAD_CHAR_REF,                     //!< [214] 14 - Rreference to invalid character number
    FMI_CF_RCODE_XSP_BINARY_ENTITY_REF,                //!< [215] 15 - Reference to binary entity
    FMI_CF_RCODE_XSP_ATTRIBUTE_EXTERNAL_ENTITY_REF,    //!< [216] 16 - Reference to external entity in attribute
    FMI_CF_RCODE_XSP_MISPLACED_XML_PI,                 //!< [217] 17 - XML declaration not at start of external entity
    FMI_CF_RCODE_XSP_UNKNOWN_ENCODING,                 //!< [218] 18 - Unknown encoding
    FMI_CF_RCODE_XSP_INCORRECT_ENCODING,               //!< [219] 19 - Encoding specified in XML declaration is incorrect
    FMI_CF_RCODE_XSP_UNCLOSED_CDATA_SECTION,           //!< [220] 20 - Unclosed CDATA section
    FMI_CF_RCODE_XSP_EXTERNAL_ENTITY_HANDLING,         //!< [221] 21 - Error in processing external entity reference
    FMI_CF_RCODE_XSP_NOT_STANDALONE,                   //!< [222] 22 - Document is not standalone
    FMI_CF_RCODE_XSP_UNEXPECTED_STATE,                 //!< [223] 23 - Unexpected parser state - please send a bug report
    FMI_CF_RCODE_XSP_ENTITY_DECLARED_IN_PE,            //!< [224] 24 - Entity declared in parameter entity
    FMI_CF_RCODE_XSP_FEATURE_REQUIRES_XML_DTD,         //!< [225] 25 - Requested feature requires XML_DTD support in Expat
    FMI_CF_RCODE_XSP_CANT_CHANGE_FEATURE_ONCE_PARSING, //!< [226] 26 - Cannot change setting once parsing has begun
    FMI_CF_RCODE_XSP_UNBOUND_PREFIX,                   //!< [227] 27 - Unbound prefix
    FMI_CF_RCODE_XSP_UNDECLARING_PREFIX,               //!< [228] 28 - Must not undeclare prefix
    FMI_CF_RCODE_XSP_INCOMPLETE_PE,                    //!< [229] 29 - Incomplete markup in parameter entity
    FMI_CF_RCODE_XSP_XML_DECL,                         //!< [230] 30 - XML declaration not well-formed
    FMI_CF_RCODE_XSP_TEXT_DECL,                        //!< [231] 31 - Text declaration not well-formed
    FMI_CF_RCODE_XSP_PUBLICID,                         //!< [232] 32 - Illegal character(s) in public id
    FMI_CF_RCODE_XSP_SUSPENDED,                        //!< [233] 33 - Parser suspended
    FMI_CF_RCODE_XSP_NOT_SUSPENDED,                    //!< [234] 34 - Parser not suspended
    FMI_CF_RCODE_XSP_ABORTED,                          //!< [235] 35 - Parsing aborted
    FMI_CF_RCODE_XSP_FINISHED,                         //!< [236] 36 - Parsing finished
    FMI_CF_RCODE_XSP_RESERVED_PREFIX_XML,              //!< [237] 37 - Reserved XML prefix
    FMI_CF_RCODE_XSP_RESERVED_PREFIX_XMLNS,            //!< [238] 38 - Reserved XMLNS prefix
    FMI_CF_RCODE_XSP_RESERVED_NAMESPACE_URI            //!< [239] 39 - Reserved namespace URI prefix
};

    //! Lookup for custom forms transmission error codes
    const char * getCustomFormErrorMsg( const FMI_cf_rcode code );

#endif

#endif