#include "stdafx.h"
#include "PollerFlashStatusMessage.h"

CPollerFlashStatusMessage::CPollerFlashStatusMessage(byte *response)
{
	m_lpBuffer = new byte[FLASH_STATUS_MESSAGE_LENGTH];
	m_bufferSize = FLASH_STATUS_MESSAGE_LENGTH;
	memcpy(m_lpBuffer, response, FLASH_STATUS_MESSAGE_LENGTH);
}

CPollerFlashStatusMessage::~CPollerFlashStatusMessage(void)
{
	if (m_lpBuffer != NULL)
		delete [] m_lpBuffer;
}


DWORD CPollerFlashStatusMessage::GetSessionID()
{
	DWORD sessionID;
	memcpy(&sessionID, m_lpBuffer + SESSIONID_LOCATION, sizeof(sessionID));
	return sessionID;
}

DWORD CPollerFlashStatusMessage::GetBlockElement(DWORD element)
{
	DWORD blockNumber;
	byte *location, *firstLocation;
	
	firstLocation = m_lpBuffer + BLOCKARRAY_LOCATION;
	location = firstLocation + element * sizeof(DWORD);
	blockNumber = (DWORD)*location;

	return blockNumber;
}


