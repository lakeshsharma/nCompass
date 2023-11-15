#pragma once
#include "PollerMessage.h"

class CPollerTransactionAckMessage :
	public CPollerMessage
{
public:
	CPollerTransactionAckMessage(DWORD dataLength);
	byte *GetReadWriteBuffer( long &bufferSize );
	DWORD GetUniqueId();
	byte GetTransType();

public:
	~CPollerTransactionAckMessage(void);
};
