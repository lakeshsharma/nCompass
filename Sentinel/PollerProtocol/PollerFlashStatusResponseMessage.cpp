#include "stdafx.h"
#include "PollerFlashStatusResponseMessage.h"

CPollerFlashStatusResponseMessage::CPollerFlashStatusResponseMessage()
{
	m_lpBuffer = new byte[FLASH_STATUS_RESPONSE_MESSAGE_LENGTH];
	m_bufferSize = FLASH_STATUS_RESPONSE_MESSAGE_LENGTH;
	m_sessionID = 0;
	m_elementCount = 0;
	m_status = 0;
	m_nextBlock = 0;

	// initialize array to 0's
	for (int i = 0; i < FLASH_BLOCKARRAY_SIZE; i++)
	{
		m_blockArray[i] = 0;
	}
}

CPollerFlashStatusResponseMessage::~CPollerFlashStatusResponseMessage(void)
{
}


byte * CPollerFlashStatusResponseMessage::GetReadWriteBuffer( long &bufferSize )
{
	bufferSize = m_bufferSize;
	SetReadWriteBuffer();
	return m_lpBuffer;
}


void CPollerFlashStatusResponseMessage::SetReadWriteBuffer()
{
	MessageHeader.MessageID = POLLER_MSG_FLASH_STATUS_RESPONSE;
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

	// element count
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_elementCount, sizeof(m_elementCount));
	offset+=sizeof(m_elementCount);

	// array elements
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, m_blockArray, FLASH_BLOCKARRAY_SIZE * sizeof(DWORD));
	offset+=FLASH_BLOCKARRAY_SIZE * sizeof(DWORD);


	// status
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_status, sizeof(m_status));
	offset+=sizeof(m_status);

	// next block
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_nextBlock, sizeof(m_nextBlock));
}


void CPollerFlashStatusResponseMessage::SetSessionID(DWORD sessionID)
{
	m_sessionID = sessionID;
}

void CPollerFlashStatusResponseMessage::SetStatus(byte status)
{
	m_status = status;
}

void CPollerFlashStatusResponseMessage::SetNextBlock(DWORD nextBlock)
{
	m_nextBlock = nextBlock;
}

DWORD CPollerFlashStatusResponseMessage::GetNextBlock()
{
	return m_nextBlock;
}

byte CPollerFlashStatusResponseMessage::GetBlockCount()
{
	return m_elementCount;
}

void CPollerFlashStatusResponseMessage::AddBlockElement(DWORD element)
{
	// add "element" to array and increment array counter.
	
	m_blockArray[m_elementCount++] = element;
}


