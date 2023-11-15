#pragma once
#include "PollerMessage.h"

class CPollerRequestLossLimitBalanceMessage :
	public CPollerMessage
{
public:
	CPollerRequestLossLimitBalanceMessage(const BYTE * cardID, int length);
	~CPollerRequestLossLimitBalanceMessage(void);

	byte * GetReadWriteBuffer( long &bufferSize );

private:
	byte * m_cardID;

};
