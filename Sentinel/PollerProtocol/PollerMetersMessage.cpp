#include "stdafx.h"
#include "PollerMetersMessage.h"
#include "Logging/LogString.h"

CPollerMetersMessage::CPollerMetersMessage(CGames * games, 
	bool nfdSupported,
	DWORD playSessionId, 
	DWORD playTxSeqNum)
{
	LogString(POLLER_LOG, "[%s]: nfdSupported: %d playSessionId: %u playTxSeqNum: %u",
		__FUNCTION__, nfdSupported, playSessionId, playTxSeqNum);

	m_games = games;
	m_nfdSupported = nfdSupported;
	m_playSessionId = playSessionId;
	m_playTxSeqNum = playTxSeqNum;

	CGame *game;
	CMeters *meters;
	
	game = m_games->GetGameByIndex(0);
	meters = game->GetMeters();

	m_meterCount = meters->GetMeterCount();

	if( m_meterCount > NUM_METERS )
		m_meterCount = NUM_METERS;

	m_bufferSize = SIZE_OF_MESSAGE_HEADER + 
		sizeof(m_meterCount) +
		(m_meterCount * (sizeof(PollerMeter().Number) + sizeof(PollerMeter().Value)));

	if (m_nfdSupported)
	{
		// Its NFD message format which we need to create which would be having extra fields
		m_bufferSize += sizeof(SYSTEMTIME) + sizeof(m_playSessionId) + sizeof(m_playTxSeqNum) +
			sizeof(DWORD) +   // Current subgame number, and its related fields follow
			POLLER_MFG_ID_LEN + POLLER_PAY_TABLE_ID_LEN + POLLER_ADDITIONAL_ID_LEN + 
			sizeof(WORD) + sizeof(BYTE) +  // Theoretical payback, game status
			(NVRAM_NUMBER_OF_SUBGAME_METERS * sizeof(__int64)); // current subgame meters
	}

	m_lpBuffer = new byte[m_bufferSize];
	memset(m_lpBuffer, 0, m_bufferSize);
}

CPollerMetersMessage::~CPollerMetersMessage(void)
{
	if ( m_games != NULL )
		delete m_games;

}

byte * CPollerMetersMessage::GetReadWriteBuffer( long &bufferSize )
{
	bufferSize = m_bufferSize;
	SetReadWriteBuffer();
	return m_lpBuffer;
}

void CPollerMetersMessage::SetReadWriteBuffer()
{
	MessageHeader.MessageID = POLLER_MSG_METERS;
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

	if (m_nfdSupported)
	{
		// Timestamp
		offset = AppendUTCTimeStamp(offset);

		// Play Session ID
		memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_playSessionId, sizeof(m_playSessionId));
		offset += sizeof(m_playSessionId);

		// Play transaction sequence
		memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_playTxSeqNum, sizeof(m_playTxSeqNum));
		offset += sizeof(m_playTxSeqNum);
	}

	//Count
	memcpy(m_lpBuffer + offset, &m_meterCount, sizeof(m_meterCount));
	offset+= sizeof(m_meterCount);

	PollerMeter pollerMeter;
	CMeter *lpMeter;
	CGame *game;
	CMeters *meters;
	
	game = m_games->GetGameByIndex(0);
	meters = game->GetMeters();

	//Meters
	for(int n = 0; n < m_meterCount && n < NUM_METERS; n++)
	{
		lpMeter = meters->GetMeterByIndex(n);
		pollerMeter.Number = lpMeter->GetMeterNumber();
		pollerMeter.Value = lpMeter->GetValue() == INVALID_METER_VALUE ? 0 : lpMeter->GetValue();

		memcpy(m_lpBuffer + offset, &(pollerMeter.Number), sizeof(pollerMeter.Number));
		offset+=sizeof(pollerMeter.Number);

		memcpy(m_lpBuffer + offset, &(pollerMeter.Value), sizeof(pollerMeter.Value));
		offset+=sizeof(pollerMeter.Value);
	}

	if (m_nfdSupported)
	{
		game = m_games->GetCurrentGame();
		if (game && !game->IsEGM())
		{
			DWORD currentSubgameNumber = game->GetGameNumber();
			memcpy(m_lpBuffer + offset, &currentSubgameNumber, sizeof(currentSubgameNumber));
			offset += sizeof(currentSubgameNumber);

			memcpy(m_lpBuffer + offset, game->GetMfgID(), POLLER_MFG_ID_LEN);
			offset += POLLER_MFG_ID_LEN;

			memcpy(m_lpBuffer + offset, game->GetPaytableID(), POLLER_PAY_TABLE_ID_LEN);
			offset += POLLER_PAY_TABLE_ID_LEN;

			memcpy(m_lpBuffer + offset, game->GetAdditionalID(), POLLER_ADDITIONAL_ID_LEN);
			offset += POLLER_ADDITIONAL_ID_LEN;

			WORD theoreticalPayback = game->GetTheoreticalPayback();
			memcpy(m_lpBuffer + offset, &theoreticalPayback, sizeof(theoreticalPayback));
			offset += sizeof(theoreticalPayback);

			BYTE gameStatus = game->IsEnabled();
			memcpy(m_lpBuffer + offset, &gameStatus, sizeof(gameStatus));
			offset += sizeof(gameStatus);

			meters = game->GetMeters();

			__int64 meterValue = 0;
			for (byte meterIdx = 0; meterIdx < SUBGAME_METER_LAST; meterIdx++)
			{
				lpMeter = meters->GetMeterByIndex(meterIdx);
				meterValue = (lpMeter && (lpMeter->GetValue() != INVALID_METER_VALUE)) ? lpMeter->GetValue() : 0;
				memcpy(m_lpBuffer + offset, &meterValue, sizeof(meterValue));
				offset += sizeof(meterValue);
			}
		}
		else
		{
			LogString(POLLER_LOG, "WARN: [CPollerMetersMessage::SetReadWriteBuffer] Current "
				"game information not available, so not included in the poller packet");
		}
	}
}
