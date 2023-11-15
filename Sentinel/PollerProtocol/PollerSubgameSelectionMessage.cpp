#include "stdafx.h"
#include "PollerSubgameSelectionMessage.h"

const int NONVARIABLE_MESSAGE_SIZE = 23;

CPollerSubgameSelectionMessage::CPollerSubgameSelectionMessage(DWORD gameSessionSequence,
									bool cardedPlayerIndicator,
									const byte *cardID,
									int cardNumberLen,
									DWORD playerId,
									int playerSessionId,
									WORD gameNumber,
									WORD denom, 
									bool mgmdAvailable)
{
	m_gameSessionSequence = gameSessionSequence;
	m_cardedPlayerIndicator = cardedPlayerIndicator; 

	if (cardNumberLen > 0 && cardID != nullptr) 
	{
		m_cardID = new byte[cardNumberLen];
		m_cardNumberLen = cardNumberLen;
		memcpy(m_cardID, cardID, cardNumberLen);
	}
	else
	{
		m_cardID = nullptr;
		m_cardNumberLen = 0;
	}

	m_playerId = playerId;
	m_playerSessionId = playerSessionId;
	m_gameNumber = gameNumber;
	m_denom = denom;
	m_mgmdAvailable = mgmdAvailable;
	m_bufferSize = SIZE_OF_MESSAGE_HEADER + 
		NONVARIABLE_MESSAGE_SIZE;
	m_lpBuffer = new byte[m_bufferSize];
}

CPollerSubgameSelectionMessage::~CPollerSubgameSelectionMessage(void)
{
	delete[] m_cardID;
}

byte * CPollerSubgameSelectionMessage::GetReadWriteBuffer(long &bufferSize)
{
	bufferSize = m_bufferSize;
	SetReadWriteBuffer();
	return m_lpBuffer;
}

void CPollerSubgameSelectionMessage::SetReadWriteBuffer()
{
	int offset = 0;

	//Header (piece by piece to avoid missaligned data error)
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(MessageHeader.PacketVersion), sizeof(MessageHeader.PacketVersion));
	offset+= sizeof(MessageHeader.PacketVersion);

	MessageHeader.MessageID = POLLER_MSG_SUBGAME_SELECTION;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(MessageHeader.MessageID), sizeof(MessageHeader.MessageID));
	offset+= sizeof(MessageHeader.MessageID);

	MessageHeader.DataLength = NONVARIABLE_MESSAGE_SIZE;
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

	// Message body
	// Game Session Sequence
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_gameSessionSequence, sizeof(m_gameSessionSequence));
	offset+= sizeof(m_gameSessionSequence);

	// Carded Player Indiciator
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_cardedPlayerIndicator, sizeof(m_cardedPlayerIndicator));
	offset+= sizeof(m_cardedPlayerIndicator);

	// card #
	m_cardNumberLen = m_cardNumberLen > CARD_ID_LEN ? CARD_ID_LEN : m_cardNumberLen;
	if (m_cardNumberLen > 0)
	{
		memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, m_cardID, m_cardNumberLen);
	}
	else
	{
		memset(m_lpBuffer + offset, 0, CARD_ID_LEN);
	}

	offset += CARD_ID_LEN;

	// Player ID
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_playerId, sizeof(m_playerId));
	offset+= sizeof(m_playerId);

	// Player Session ID
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_playerSessionId, sizeof(m_playerSessionId));
	offset+= sizeof(m_playerSessionId);

	// Game Number
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_gameNumber, sizeof(m_gameNumber));
	offset+= sizeof(m_gameNumber);

	// Denomination
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_denom, sizeof(m_denom));
	offset+= sizeof(m_denom);

	// MGMD Available
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_mgmdAvailable, sizeof(bool));
}

