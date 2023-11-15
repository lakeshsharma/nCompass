#include "stdafx.h"
#include "PollerFlashDataBlockMessage.h"

CPollerFlashDataBlockMessage::CPollerFlashDataBlockMessage(byte *response)
{
	m_lpBuffer = new byte[FLASH_DATA_BLOCK_MESSAGE_LENGTH];
	m_bufferSize = FLASH_DATA_BLOCK_MESSAGE_LENGTH;
	memcpy(m_lpBuffer, response, FLASH_DATA_BLOCK_MESSAGE_LENGTH);
}

CPollerFlashDataBlockMessage::~CPollerFlashDataBlockMessage(void)
{
}

DWORD CPollerFlashDataBlockMessage::GetBlockNumber()
{
	DWORD blockNumber;
	memcpy(&blockNumber, m_lpBuffer + FLASH_BLOCK_NUMBER_LOCATION, sizeof(blockNumber));
	return blockNumber;
}

DWORD CPollerFlashDataBlockMessage::GetSessionID()
{
	DWORD sessionID;
	memcpy(&sessionID, m_lpBuffer + FLASH_DATA_BLOCK_SESSIONID_LOCATION, sizeof(sessionID));
	return sessionID;
}

byte *CPollerFlashDataBlockMessage::GetDataBlock()
{
	return m_lpBuffer + FLASH_DATA_LOCATION;
}
