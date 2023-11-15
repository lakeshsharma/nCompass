#include "stdafx.h"
#include "MessagePersistType.h"

CMessagePersistType::CMessagePersistType(MessagePersistType persistType)
{
	m_persistType = persistType;
}

CMessagePersistType::~CMessagePersistType(void)
{
}

MessagePersistType CMessagePersistType::GetPersistType()
{
	return m_persistType;
}
