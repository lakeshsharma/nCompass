#pragma once
#include "PollerMessage.h"

class CPollerHeartbeatMessage :
	public CPollerMessage
{
public:
	CPollerHeartbeatMessage(void);
public:
	~CPollerHeartbeatMessage(void);

	byte *GetReadWriteBuffer( long &bufferSize );

};
