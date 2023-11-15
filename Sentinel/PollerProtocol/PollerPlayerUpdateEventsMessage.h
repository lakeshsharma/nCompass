#pragma once
#include "PollerMessage.h"

class CPollerPlayerUpdateEventsMessage :
	public CPollerMessage
{
public:
	CPollerPlayerUpdateEventsMessage(byte *response);

	byte GetEventCount();
	time_t GetEventTime(byte index);

public:
	~CPollerPlayerUpdateEventsMessage(void);

private:
	byte m_eventCount;
	time_t *m_timeArray;
};
