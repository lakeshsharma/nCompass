#pragma once
#include "PollerMessage.h"

class CPollerChangeSlotMastIDMessage :
	public CPollerMessage
{
public:
	CPollerChangeSlotMastIDMessage(int NewSlotMastID);
public:
	~CPollerChangeSlotMastIDMessage(void);

	byte *GetReadWriteBuffer( long &bufferSize );
private:
	long m_NewSlotMastID;

};
