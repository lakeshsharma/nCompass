#include "stdafx.h"
#include "PollerRequestLossLimitBalanceMessage.h"

CPollerRequestLossLimitBalanceMessage::CPollerRequestLossLimitBalanceMessage(const BYTE * cardID, int length)
{
	m_bufferSize = SIZE_OF_MESSAGE_HEADER + CARD_ID_LEN;
	m_lpBuffer = new byte[m_bufferSize];

	m_cardID = new byte[CARD_ID_LEN];
	memset(m_cardID, 0, CARD_ID_LEN);
	
	if (length <= CARD_ID_LEN)
	{
		memcpy(m_cardID, cardID, length);
	}
}

CPollerRequestLossLimitBalanceMessage::~CPollerRequestLossLimitBalanceMessage(void)
{
	delete [] m_cardID;
}

byte * CPollerRequestLossLimitBalanceMessage::GetReadWriteBuffer( long &bufferSize )
{
	MessageHeader.MessageID = POLLER_MSG_REQUEST_LOSS_LIMIT_BALANCE_UPDATE;
	MessageHeader.DataLength = CARD_ID_LEN;

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

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, m_cardID, CARD_ID_LEN);

	bufferSize = m_bufferSize;
	return m_lpBuffer;
}
