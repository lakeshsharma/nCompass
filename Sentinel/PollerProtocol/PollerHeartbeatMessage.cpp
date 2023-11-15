#include "stdafx.h"
#include "PollerHeartbeatMessage.h"

CPollerHeartbeatMessage::CPollerHeartbeatMessage(void)
{
	m_bufferSize = SIZE_OF_MESSAGE_HEADER;
	m_lpBuffer = new byte[m_bufferSize];
}

CPollerHeartbeatMessage::~CPollerHeartbeatMessage(void)
{
}

byte * CPollerHeartbeatMessage::GetReadWriteBuffer( long &bufferSize )
{
	MessageHeader.MessageID = POLLER_MSG_SENTINEL_HEARTBEAT;
	MessageHeader.DataLength = 0;

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

	bufferSize = m_bufferSize;
	return m_lpBuffer;
}
