#include "stdafx.h"
#include "PollerSubGameMetersMessage.h"
#include "../Utilities.h"
#include "Logging/LogString.h"

CPollerSubGameMetersMessage::CPollerSubGameMetersMessage(CGames *games, bool nfdSupported)
{
	LogString(POLLER_LOG, "[%s]: nfdSupported: %d",	__FUNCTION__, nfdSupported);

	m_lpPaytableID = new char[PAY_TABLE_ID_LEN];
	m_lpMfgID = new char[MFG_ID_LEN];
	m_lpAdditionalID = new char[ADDITIONAL_ID_LEN];

	m_games = games;
	m_nfdSupported = nfdSupported;

	CGame *game;
	CMeters *meters;
	
	game = m_games->GetGameByIndex(0);
	meters = game->GetMeters();

	//we need all ten meters, and invalids must be zero
	for( byte meterNum = 0; meterNum < SUBGAME_METER_LAST; meterNum++ )
	{
		if((meters->GetMeterByNumber(meterNum) == NULL) ||(meters->GetMeterByNumber(meterNum)->GetValue() == INVALID_METER_VALUE))
		{
			meters->SetMeter(meterNum, 0);
		}
	}

	m_meterCount = meters->GetMeterCount();
	m_gameNumber = game->GetGameNumber();

	CUtilities::ConvertCharsToChars( game->GetMfgID(), m_lpMfgID, MFG_ID_LEN );
	CUtilities::ConvertCharsToChars( game->GetPaytableID(), m_lpPaytableID, PAY_TABLE_ID_LEN  );
	CUtilities::ConvertCharsToChars( game->GetAdditionalID(), m_lpAdditionalID, ADDITIONAL_ID_LEN );

	m_wTheoreticalPayback = game->GetTheoreticalPayback();
	m_gameStatus = game->IsEnabled();

	m_bufferSize = SIZE_OF_MESSAGE_HEADER + 
		(m_nfdSupported ? (sizeof(SYSTEMTIME) + sizeof(DWORD)) : sizeof(WORD)) + // Timestamp and Game number 
		POLLER_PAY_TABLE_ID_LEN + POLLER_MFG_ID_LEN +
		POLLER_ADDITIONAL_ID_LEN + sizeof(m_wTheoreticalPayback) + sizeof( m_gameStatus ) +
		(m_meterCount * sizeof(PollerMeter().Value));

	m_lpBuffer = new byte[m_bufferSize];	
	
}

CPollerSubGameMetersMessage::~CPollerSubGameMetersMessage(void)
{
	delete [] m_lpPaytableID;
	delete [] m_lpMfgID;
	delete [] m_lpAdditionalID;

	if ( m_games != NULL )
		delete m_games;
}

byte * CPollerSubGameMetersMessage::GetReadWriteBuffer( long &bufferSize )
{
	bufferSize = m_bufferSize;
	SetReadWriteBuffer();
	return m_lpBuffer;
}

void CPollerSubGameMetersMessage::SetReadWriteBuffer()
{
	MessageHeader.MessageID = POLLER_MSG_SUBGAME_METERS;
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

		// 4 bytes game number
		memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_gameNumber, sizeof(m_gameNumber));
		offset += sizeof(m_gameNumber);
	}
	else
	{
		WORD gameNumber = (WORD)m_gameNumber;

		// 2 bytes game number
		memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &gameNumber, sizeof(gameNumber));
		offset += sizeof(gameNumber);
	}

	memcpy(m_lpBuffer + offset, m_lpMfgID, POLLER_MFG_ID_LEN);
	offset+= POLLER_MFG_ID_LEN;

	memcpy(m_lpBuffer + offset, m_lpPaytableID, POLLER_PAY_TABLE_ID_LEN);
	offset+= POLLER_PAY_TABLE_ID_LEN;

	memcpy(m_lpBuffer + offset, m_lpAdditionalID, POLLER_ADDITIONAL_ID_LEN);
	offset+= POLLER_ADDITIONAL_ID_LEN;

	memcpy(m_lpBuffer + offset, &m_wTheoreticalPayback, sizeof(m_wTheoreticalPayback));
	offset+= sizeof(m_wTheoreticalPayback);

	memcpy(m_lpBuffer + offset, &m_gameStatus, sizeof(m_gameStatus));
	offset+= sizeof(m_gameStatus);

	PollerMeter pollerMeter;
	CMeter *lpMeter;
	CGame *game;
	CMeters *meters;
	
	game = m_games->GetGameByIndex(0);
	meters = game->GetMeters();

	//Meters
	for(int n = 0; n < m_meterCount; n++)
	{
		lpMeter = meters->GetMeterByIndex(n);
		pollerMeter.Value = lpMeter->GetValue() == INVALID_METER_VALUE ? 0 : lpMeter->GetValue();

		memcpy(m_lpBuffer + offset, &(pollerMeter.Value), sizeof(pollerMeter.Value));
		offset+=sizeof(pollerMeter.Value);
	}
}
