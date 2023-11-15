#include "stdafx.h"
#include "PollerFlashBurnMessage.h"

CPollerFlashBurnMessage::CPollerFlashBurnMessage(byte *response)
{
	m_lpBuffer = new byte[FLASH_BURN_MESSAGE_LENGTH];
	m_bufferSize = FLASH_BURN_MESSAGE_LENGTH;
	memcpy(m_lpBuffer, response, FLASH_BURN_MESSAGE_LENGTH);
}

CPollerFlashBurnMessage::~CPollerFlashBurnMessage(void)
{
}

DWORD CPollerFlashBurnMessage::GetSessionID()
{
	DWORD sessionID;
	memcpy(&sessionID, m_lpBuffer + SESSIONID_LOCATION, sizeof(sessionID));
	return sessionID;
}

byte CPollerFlashBurnMessage::GetBurnClear()
{
	byte burnClear;
	memcpy(&burnClear, m_lpBuffer + FLASH_BURN_LOCATION, sizeof(byte));
	return burnClear;
}

