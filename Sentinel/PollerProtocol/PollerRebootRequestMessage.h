#pragma once

#include "PollerMessage.h"

class CPollerRebootRequestMessage :
	public CPollerMessage
{
public:
	CPollerRebootRequestMessage(byte *response);
	~CPollerRebootRequestMessage();

	byte GetRebootRequestType();

protected:
	byte m_rebootRequestType;
};

