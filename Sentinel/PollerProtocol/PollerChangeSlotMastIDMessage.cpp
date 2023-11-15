#include "stdafx.h"
#include "PollerChangeSlotMastIDMessage.h"

CPollerChangeSlotMastIDMessage::CPollerChangeSlotMastIDMessage(int NewSlotMastID)
{
	m_bufferSize = SIZE_OF_MESSAGE_HEADER + sizeof(DWORD);
	m_lpBuffer = new byte[m_bufferSize];
	m_NewSlotMastID = NewSlotMastID;
}

CPollerChangeSlotMastIDMessage::~CPollerChangeSlotMastIDMessage(void)
{
}

byte * CPollerChangeSlotMastIDMessage::GetReadWriteBuffer( long &bufferSize )
{
	MessageHeader.MessageID = POLLER_MSG_SLOTMASTID_CHANGE;
	MessageHeader.DataLength = sizeof(m_NewSlotMastID);

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

	//Data Packet
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_NewSlotMastID, sizeof(m_NewSlotMastID));
	
	bufferSize = m_bufferSize;
	return m_lpBuffer;
}
