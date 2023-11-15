#include "stdafx.h"
#include "PollerConfigRequestMessage.h"

CPollerConfigRequestMessage::CPollerConfigRequestMessage(void)
{
	m_bufferSize = SIZE_OF_MESSAGE_HEADER + sizeof(DWORD);
	m_lpBuffer = new byte[m_bufferSize];
}

CPollerConfigRequestMessage::~CPollerConfigRequestMessage(void)
{
}

byte * CPollerConfigRequestMessage::GetReadWriteBuffer( long &bufferSize )
{
	MessageHeader.MessageID = POLLER_MSG_REQUEST_CONFIG;
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
	offset+= sizeof(MessageHeader.SlotmastRevision);

	//Data Packet tells FloorLogix that this Sentinel/nCompass is 12.5 compatible
	DWORD featuresFlag = FF_ATOMIC_REDEMPTIONS | FF_JACKPOT_EXTERNAL_BONUS;
	featuresFlag |= FF_EXTENDED_TICKET_EXPIRATION;
	featuresFlag |= FF_EXTERNAL_CREDIT_TO_GAME;
	featuresFlag |= FF_VOUCHER_SEPARATION;
	featuresFlag |= FF_MULTI_GAME_MULTI_DENOM;
	featuresFlag |= FF_CHUNKED_GAMES_LIST;
	featuresFlag |= FF_WALLET_2_0_SUPPORTED;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &featuresFlag, sizeof(featuresFlag));
	offset += sizeof(featuresFlag);

	bufferSize = m_bufferSize;
	return m_lpBuffer;
}
