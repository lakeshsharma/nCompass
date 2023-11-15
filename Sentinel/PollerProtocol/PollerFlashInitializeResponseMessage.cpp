#include "stdafx.h"
#include "PollerFlashInitializeResponseMessage.h"

CPollerFlashInitializeResponseMessage::CPollerFlashInitializeResponseMessage(void)
{
    m_status = 0;
	m_lpBuffer = new byte[FLASH_INITIALIZE_RESPONSE_MESSAGE_LENGTH];
	m_bufferSize = FLASH_INITIALIZE_RESPONSE_MESSAGE_LENGTH;
	m_sessionID = 0;
}

CPollerFlashInitializeResponseMessage::~CPollerFlashInitializeResponseMessage(void)
{
}


byte * CPollerFlashInitializeResponseMessage::GetReadWriteBuffer( long &bufferSize )
{
	bufferSize = m_bufferSize;
	SetReadWriteBuffer();
	return m_lpBuffer;
}


void CPollerFlashInitializeResponseMessage::SetReadWriteBuffer()
{
	MessageHeader.MessageID = POLLER_MSG_FLASH_INITIALIZE_RESPONSE;
	MessageHeader.DataLength = m_bufferSize - SIZE_OF_MESSAGE_HEADER;

	int offset = 0;
	//Header (piece by piece to avoid missaligned data error)
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(MessageHeader.PacketVersion), sizeof(MessageHeader.PacketVersion));
	offset+= sizeof(MessageHeader.PacketVersion);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(MessageHeader.MessageID), sizeof(MessageHeader.MessageID));
	offset+= sizeof(MessageHeader.MessageID);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(MessageHeader.DataLength), sizeof(MessageHeader.DataLength));
	offset+= sizeof(MessageHeader.DataLength);
	
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(MessageHeader.MacAddress), sizeof(MessageHeader.MacAddress));
	offset+= sizeof(MessageHeader.MacAddress);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(MessageHeader.SequenceNumber), sizeof(MessageHeader.SequenceNumber));
	offset+= sizeof(MessageHeader.SequenceNumber);
	
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(MessageHeader.SlotMastID), sizeof(MessageHeader.SlotMastID));
	offset+= sizeof(MessageHeader.SlotMastID);
	
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(MessageHeader.SlotmastRevision), sizeof(MessageHeader.SlotmastRevision));
	offset+= sizeof(MessageHeader.SlotmastRevision);

	// message content
	// sessionid
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_sessionID, sizeof(m_sessionID));
	offset+= sizeof(m_sessionID);

	// status
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_status, sizeof(m_status));
}


void CPollerFlashInitializeResponseMessage::SetSessionID(DWORD sessionID)
{
	m_sessionID = sessionID;
}

