#include "stdafx.h"
#include "PollerResendItem.h"

CPollerResendItem::CPollerResendItem()
{
	m_pollerMsg = NULL;
	m_tTimeStamp = CUtilities::GetCurrentTime();
	m_nResendPeriod = 0;
	m_rType = NONE;
}

CPollerResendItem::~CPollerResendItem(void)
{
	delete m_pollerMsg;
}

void CPollerResendItem::ResetTime()
{
	m_tTimeStamp = CUtilities::GetCurrentTime();
}

void CPollerResendItem::SetContent(CPollerMessage *msg, ResendMessageType type)
{
	m_rType = type;

	m_pollerMsg = msg;
}

ResendMessageType CPollerResendItem::GetType(void)
{
	return m_rType;
}


void CPollerResendItem::SetResendPeriod(int nResendPeriod)
{
	m_nResendPeriod = nResendPeriod;
}

CPollerMessage* CPollerResendItem::GetContent(void)
{
	return m_pollerMsg;
}

time_t CPollerResendItem::GetTimeoutTime(void)
{
	return (m_tTimeStamp + m_nResendPeriod);
}

time_t CPollerResendItem::GetTimeStamp(void)
{
	return (m_tTimeStamp);
}
