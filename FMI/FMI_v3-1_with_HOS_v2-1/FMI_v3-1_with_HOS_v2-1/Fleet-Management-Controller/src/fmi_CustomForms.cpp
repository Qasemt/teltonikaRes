/*********************************************************************
*
*   MODULE NAME:
*       fmi_CustomForms.cpp
*
*   Copyright 2008-2013 by Garmin Ltd. or its subsidiaries.
*---------------------------------------------------------------------
* $NoKeywords$
*********************************************************************/
#include "stdafx.h"
#include "fmi_CustomForms.h"

#if( FMI_SUPPORT_A612 )

const char * getCustomFormErrorMsg
    (
    const FMI_cf_rcode code
    )
{
    switch ( code )
    {
        case FMI_CF_RCODE_OK:                                   return "OK";
        case FMI_CF_RCODE_GFS_ERROR:                            return "GFS error result code";
        case FMI_CF_RCODE_SQL_ERROR:                            return "SQL error result code";
        case FMI_CF_RCODE_SQL_PREPARE_FAILED:                   return "SQL prepare statement failed";
        case FMI_CF_RCODE_SQL_BIND_FAILED:                      return "SQL bind parameters failed";
        case FMI_CF_RCODE_SQL_EXEC_FAILED:                      return "SQL step (query/execution) failed";
        case FMI_CF_RCODE_SQL_TRANSACTION_ALREADY_STARTED:      return "SQL transaction has already been started";
        case FMI_CF_RCODE_SQL_NUMBER_OF_FORMS_LIMIT_EXCEEDED:   return "SQL number of forms exceeded supported limit";
        case FMI_CF_RCODE_SQL_POSITION_FREE_FAILED:             return "SQL free form position failed";
        case FMI_CF_RCODE_SQL_POSITION_PACK_FAILED:             return "SQL pack form positions failed";
        case FMI_CF_RCODE_SQL_ITEM_EXTRAS_NULL_TYPE:            return "SQL item extras data type is NULL and cannot be used for query";
        case FMI_CF_RCODE_SQL_ITEM_EXTRAS_UNEXPECTED_SIZE:      return "SQL item extras blob and type sizes are not the same";
        case FMI_CF_RCODE_SQL_OLDEST_SAVED_DELETE_FAILED:       return "SQL oldest saved form deletion failed";
        case FMI_CF_RCODE_SQL_NO_UNSENT_FORMS:                  return "SQL no unsent forms are in the database";
        case FMI_CF_RCODE_SQL_SUBMITTED_STRING_POINTER_NULL:    return "SQL submitted string pointer is NULL and cannot be used for query";
        case FMI_CF_RCODE_SQL_SUBMITTED_STRING_UNEXPECTED_SIZE: return "SQL data blob and submitted string sizes are not the same";
        case FMI_CF_RCODE_SQL_FORM_TEMPLATE_NOT_FOUND:          return "SQL form template is not found in the database";

        case FMI_CF_RCODE_SCHEMA_FORM_ELMT_INCOMPLETE:          return "Form element is incomplete - required values are missing";
        case FMI_CF_RCODE_SCHEMA_ITEM_ELMT_INCOMPLETE:          return "Item element is incomplete - required values are missing";
        case FMI_CF_RCODE_SCHEMA_ITEM_PARENT_INVALID:           return "An item elements parent is not a form element";
        case FMI_CF_RCODE_SCHEMA_ITEM_ID_REUSE:                 return "Item ID is used more than one time";
        case FMI_CF_RCODE_SCHEMA_ITEM_CNT_OUT_OF_RANGE:         return "Item count is out of range for a form";
        case FMI_CF_RCODE_SCHEMA_TYPE_PARENT_INVALID:           return "A type elements parent is not an item element";
        case FMI_CF_RCODE_SCHEMA_TYPE_CNT_OUT_OF_RANGE:         return "Type count is out of range for an item (> 1)";
        case FMI_CF_RCODE_SCHEMA_TEXT_LEN_OUT_OF_RANGE:         return "Text length is out of range";
        case FMI_CF_RCODE_SCHEMA_INT_LEN_OUT_OF_RANGE:          return "Integer length is out of range";
        case FMI_CF_RCODE_SCHEMA_INT_MIN_OUT_OF_RANGE:          return "Minimum integer value is out of range";
        case FMI_CF_RCODE_SCHEMA_INT_MAX_OUT_OF_RANGE:          return "Maximum integer value is out of range";
        case FMI_CF_RCODE_SCHEMA_INT_MIN_GRT_THAN_MAX:          return "Minimum integer value is greater than the maximum integer value";
        case FMI_CF_RCODE_SCHEMA_OPT_ELMT_INCOMPLETE:           return "Option element is incomplete - required values are missing";
        case FMI_CF_RCODE_SCHEMA_OPT_PARENT_INVALID:            return "An option elements parent is not a single select or multiple select element";
        case FMI_CF_RCODE_SCHEMA_OPT_ID_REUSE:                  return "Option ID is used more than one time for a given item";
        case FMI_CF_RCODE_SCHEMA_OPT_CNT_OUT_OF_RANGE:          return "Option count is out of range for an item";
        case FMI_CF_RCODE_SCHEMA_MONTH_OUT_OF_RANGE:            return "Month value is out of range";
        case FMI_CF_RCODE_SCHEMA_DAY_OUT_OF_RANGE:              return "Day value is out of range";
        case FMI_CF_RCODE_SCHEMA_YEAR_OUT_OF_RANGE:             return "Year value is out of range";
        case FMI_CF_RCODE_SCHEMA_INVALID_DAYS_IN_MONTH:         return "Number of days in the month is not valid";
        case FMI_CF_RCODE_SCHEMA_NO_DATE_SET:                   return "Use current is set to false and no date was specified";
        case FMI_CF_RCODE_SCHEMA_HOUR_OUT_OF_RANGE:             return "Hour value is out of range";
        case FMI_CF_RCODE_SCHEMA_MINUTE_OUT_OF_RANGE:           return "Minute value is out of range";
        case FMI_CF_RCODE_SCHEMA_SECOND_OUT_OF_RANGE:           return "Second value is out of range";
        case FMI_CF_RCODE_SCHEMA_NO_TIME_SET:                   return "Use current is set to false and no time was specified";
        case FMI_CF_RCODE_SCHEMA_UNKNOWN_ITEM_TYPE:             return "Unknown form item type";

        case FMI_CF_RCODE_XSP_NO_MEMORY:                        return "Out of memory";
        case FMI_CF_RCODE_XSP_SYNTAX:                           return "Syntax error";
        case FMI_CF_RCODE_XSP_NO_ELEMENTS:                      return "No element found";
        case FMI_CF_RCODE_XSP_INVALID_TOKEN:                    return "Not well-formed (invalid token)";
        case FMI_CF_RCODE_XSP_UNCLOSED_TOKEN:                   return "Unclosed token";
        case FMI_CF_RCODE_XSP_PARTIAL_CHAR:                     return "Partial character";
        case FMI_CF_RCODE_XSP_TAG_MISMATCH:                     return "Mismatched tag";
        case FMI_CF_RCODE_XSP_DUPLICATE_ATTRIBUTE:              return "Duplicate attribute";
        case FMI_CF_RCODE_XSP_JUNK_AFTER_DOC_ELEMENT:           return "Junk after document element";
        case FMI_CF_RCODE_XSP_PARAM_ENTITY_REF:                 return "Illegal parameter entity reference";
        case FMI_CF_RCODE_XSP_UNDEFINED_ENTITY:                 return "Undefined entity";
        case FMI_CF_RCODE_XSP_RECURSIVE_ENTITY_REF:             return "Recursive entity reference";
        case FMI_CF_RCODE_XSP_ASYNC_ENTITY:                     return "Asynchronous entity";
        case FMI_CF_RCODE_XSP_BAD_CHAR_REF:                     return "Rreference to invalid character number";
        default: return "Unknown Error";
    }
}

#endif