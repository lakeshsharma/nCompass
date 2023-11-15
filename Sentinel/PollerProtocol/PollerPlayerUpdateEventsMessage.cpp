#include "stdafx.h"
#include "PollerPlayerUpdateEventsMessage.h"

CPollerPlayerUpdateEventsMessage::CPollerPlayerUpdateEventsMessage(byte *response)
{
	int offset = 0;
	m_lpBuffer = NULL;

	memcpy_s(&m_eventCount, sizeof(m_eventCount), response, sizeof(m_eventCount));
	offset += sizeof(m_eventCount);


	// allocate memory for CTime array
	m_timeArray = new time_t[m_eventCount];

	//set array values
	SYSTEMTIME sysTime;

	for (int i = 0; i < m_eventCount; i++)
	{
		memcpy_s(&sysTime,sizeof(SYSTEMTIME),response + offset, sizeof(SYSTEMTIME));
		offset += sizeof(SYSTEMTIME);

		m_timeArray[i] = CUtilities::GetTimeFromUTCSystemTime(sysTime);
	}
}

CPollerPlayerUpdateEventsMessage::~CPollerPlayerUpdateEventsMessage(void)
{
	delete [] m_timeArray;
}

byte CPollerPlayerUpdateEventsMessage::GetEventCount()
{
	return m_eventCount;
}

time_t CPollerPlayerUpdateEventsMessage::GetEventTime(byte index)
{
	time_t sysTime = 0;

	if (index < m_eventCount)
		sysTime = m_timeArray[index];

	return sysTime;
}
