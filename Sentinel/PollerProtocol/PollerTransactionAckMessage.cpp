#include "stdafx.h"
#include "PollerTransactionAckMessage.h"

const int RESPONSE_UNIQUEID_LOCATION = 1;

CPollerTransactionAckMessage::CPollerTransactionAckMessage(DWORD dataLength)
{
	m_lpBuffer = new byte[dataLength];
	m_bufferSize = dataLength;
}

CPollerTransactionAckMessage::~CPollerTransactionAckMessage(void)
{
}

byte * CPollerTransactionAckMessage::GetReadWriteBuffer( long &bufferSize )
{
	bufferSize = m_bufferSize;
	return m_lpBuffer;
}

DWORD CPollerTransactionAckMessage::GetUniqueId()
{
	DWORD uniqueId;
	memcpy(&uniqueId, m_lpBuffer + RESPONSE_UNIQUEID_LOCATION, sizeof(uniqueId));

	return uniqueId;
}

byte CPollerTransactionAckMessage::GetTransType()
{
	byte type = 0;

	memcpy(&type, m_lpBuffer, sizeof(byte));

	return type;
}
