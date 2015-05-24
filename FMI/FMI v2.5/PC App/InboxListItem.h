/*********************************************************************
*
*   HEADER NAME:
*       InboxListItem.h
*
*   Copyright 2008-2009 by Garmin Ltd. or its subsidiaries.
*---------------------------------------------------------------------
* $NoKeywords$
*********************************************************************/
#ifndef INBOXLISTITEM_H
#define INBOXLISTITEM_H

#include "ClientListItem.h"
#include "MessageId.h"

class InboxListItem : public ClientListItem
{
public:
    typedef MessageId key_type;

    void setId
        (
        const key_type & aId
        );

    const key_type & getId() const;

    void setParent( FileBackedMap<InboxListItem> * aParent );

    void readFromStream( std::istream &aStream );

    void writeToStream( std::ofstream &aStream );

private:
    FileBackedMap<InboxListItem> * mParent;
    MessageId messageId;

};
#endif