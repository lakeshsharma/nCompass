#include "stdafx.h"
#include "PollerTimeMessage.h"

CPollerTimeMessage::CPollerTimeMessage(void)
{
	m_bufferSize = sizeof(SYSTEMTIME);
	m_lpBuffer = new byte[m_bufferSize];
}

CPollerTimeMessage::~CPollerTimeMessage(void)
{
}

SYSTEMTIME * CPollerTimeMessage::GetTime( void )
{
	SYSTEMTIME *sysTime = new SYSTEMTIME;
	memcpy_s(sysTime, sizeof(SYSTEMTIME), m_lpBuffer, sizeof(SYSTEMTIME));
	return sysTime;
}
