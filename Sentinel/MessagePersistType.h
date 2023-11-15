#pragma once

#include "InternalMsgContent.h"

typedef enum
{
	PERSIST_NONE,
	PERSIST_CONTENT_WINDOW,
	PERSIST_EMPLOYEE_WINDOW,
	PERSIST_JACKPOTHIT,
	PERSIST_FS_SEND,
	PERSIST_MAX,
} MessagePersistType;

class CMessagePersistType : public CInternalMsgContent<INT_MSG_CONTENT_PERSIST>
{
public:
	CMessagePersistType(MessagePersistType persistType);
	virtual ~CMessagePersistType(void);

	MessagePersistType GetPersistType();

private:
	MessagePersistType m_persistType;
};
