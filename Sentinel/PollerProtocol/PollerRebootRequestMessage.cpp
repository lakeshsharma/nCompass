#include "stdafx.h"
#include "Logging/LogString.h"
#include "PollerRebootRequestMessage.h"


CPollerRebootRequestMessage::CPollerRebootRequestMessage(byte *response)
{
	byte offset = 0;

	memcpy(&m_rebootRequestType, response + offset, sizeof(m_rebootRequestType));
	LogString(POLLER_LOG, "Reboot Request received from poller %s: %d Value: %d", __FUNCTION__, __LINE__,(int)m_rebootRequestType);
}

CPollerRebootRequestMessage::~CPollerRebootRequestMessage()
{
}

byte CPollerRebootRequestMessage::GetRebootRequestType()
{
	return m_rebootRequestType;
}

