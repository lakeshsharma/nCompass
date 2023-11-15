#include "stdafx.h"
#include "PollerFlashInitializeMessage.h"

CPollerFlashInitializeMessage::CPollerFlashInitializeMessage(byte *response, WORD packetVersion, WORD dataLength)
{
	m_lpBuffer = NULL;

	size_t offset = 0;
	memcpy(&m_initializer.sessionID, response + offset, sizeof(m_initializer.sessionID));
	offset += sizeof(m_initializer.sessionID);

	memcpy(&m_initializer.blockCount, response + offset, sizeof(m_initializer.blockCount));
	offset += sizeof(m_initializer.blockCount);

	memcpy(&m_initializer.checksum, response + offset, sizeof(m_initializer.checksum));
	offset += sizeof(m_initializer.checksum);

	memcpy(&m_initializer.type, response + offset, sizeof(m_initializer.type));
	offset += sizeof(m_initializer.type);

	memcpy(&m_initializer.autoBurn, response + offset, sizeof(m_initializer.autoBurn));
	offset += sizeof(m_initializer.autoBurn);

	memcpy(&m_initializer.port, response + offset, sizeof(m_initializer.port));
	offset += sizeof(m_initializer.port);

	memcpy(&m_initializer.fileSize, response + offset, sizeof(m_initializer.fileSize));
	offset += sizeof(m_initializer.fileSize);

	memcpy(&m_initializer.keyLength, response + offset, sizeof(m_initializer.keyLength));
	offset += sizeof(m_initializer.keyLength);

	m_initializer.ipAddress = 0; // Default.

	if (FLASHINIT_KEYLENGTH_REQ == m_initializer.keyLength)
	{
		m_initializer.sessionKey = new byte[m_initializer.keyLength];
		memcpy(m_initializer.sessionKey, response + offset, m_initializer.keyLength);
		offset += m_initializer.keyLength;

		if ((packetVersion >= 2) && (dataLength > offset))
		{
			memcpy(&m_initializer.ipAddress, response + offset, sizeof(m_initializer.ipAddress));
		}
	}
	else
	{
		m_initializer.keyLength = FLASHINIT_KEYLENGTH_REQ;
		m_initializer.sessionKey = new byte[m_initializer.keyLength];
		ZeroMemory(m_initializer.sessionKey, m_initializer.keyLength);
	}
}

CPollerFlashInitializeMessage::~CPollerFlashInitializeMessage(void)
{
	delete [] m_initializer.sessionKey;
}

DWORD CPollerFlashInitializeMessage::GetNumberOfBlocks()
{
	return m_initializer.blockCount;
}

DWORD CPollerFlashInitializeMessage::GetFileSize()
{
	return m_initializer.fileSize;
}

byte CPollerFlashInitializeMessage::GetType()
{
	return m_initializer.type;
}

byte CPollerFlashInitializeMessage::GetAutoBurn()
{
	return m_initializer.autoBurn;
}

DWORD CPollerFlashInitializeMessage::GetSessionID()
{
	return m_initializer.sessionID;
}

DWORD CPollerFlashInitializeMessage::GetIP()
{
	return m_initializer.ipAddress;
}

WORD CPollerFlashInitializeMessage::GetPort()
{
	return m_initializer.port;
}

DWORD CPollerFlashInitializeMessage::GetChecksum()
{
	return m_initializer.checksum;
}

byte *CPollerFlashInitializeMessage::GetSessionKey()
{
	return m_initializer.sessionKey;
}

DWORD CPollerFlashInitializeMessage::GetSessionKeyLength()
{
	return m_initializer.keyLength;
}
