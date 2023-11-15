#include "stdafx.h"
#include "PollerBonusTransactionMessage.h"

CPollerBonusTransactionMessage::CPollerBonusTransactionMessage(CMeters *meters, CPlayer *player, CBonusEvent *bonusEvent, byte transType, 
                                                               DWORD uniqueId)
{
    m_status = 0;
	m_meters = meters;
	m_player = player;
	m_bonusEvent = bonusEvent;

	m_meterCount = m_meters->GetMeterCount();
	m_uniqueId = uniqueId;
	m_transCode = transType;

	this->m_DataCount = 5;

	m_bufferSize = SIZE_OF_MESSAGE_HEADER + 
		NONVARIABLE_TRANSACTION_SIZE +
		sizeof(byte) +
		(m_meterCount * (sizeof(PollerMeter().Number) + sizeof(PollerMeter().Value))) +
		sizeof(m_DataCount) +
		sizeof(byte) + sizeof(byte) + sizeof(DWORD) + //Session Points
		sizeof(byte) + sizeof(byte) + sizeof(DWORD) + //Bonus Points
		sizeof(byte) + sizeof(byte) + sizeof(DWORD) + //Residual Value
		sizeof(byte) + sizeof(byte) + sizeof(__int64) + //Total Coin Out
		sizeof(byte) + sizeof(byte) + sizeof(DWORD); //Bonus Event ID

	m_lpBuffer = new byte[m_bufferSize];
	m_bufferIsSet = false;
}

CPollerBonusTransactionMessage::~CPollerBonusTransactionMessage(void)
{
}

void CPollerBonusTransactionMessage::SetStatus(DWORD status)
{
	m_status = status;
}

DWORD CPollerBonusTransactionMessage::GetStatus()
{
	return m_status;
}

byte * CPollerBonusTransactionMessage::GetReadWriteBuffer(DWORD &bufferSize)
{
	bufferSize = m_bufferSize;
	return m_lpBuffer;
}

void CPollerBonusTransactionMessage::SetReadWriteBuffer()
{
	m_bufferIsSet = true;

	MessageHeader.MessageID = POLLER_MSG_TRANSACTION;
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

	// Transaction body
	// Transaction code
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_transCode, sizeof(m_transCode));
	offset+= sizeof (m_transCode);

	// Transaction time
	offset = AppendUTCTimeStamp(offset);

	// unique ID.
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_uniqueId, sizeof(m_uniqueId));
	offset += sizeof(m_uniqueId);

	// status.
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_status, sizeof(m_status));
	offset += sizeof(m_status);

	// card #
	int cardIdLen;
	const byte *cardId = m_player->GetCard()->GetCardID(cardIdLen);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, cardId, cardIdLen > CARD_ID_LEN ? CARD_ID_LEN : cardIdLen );
	offset += cardIdLen;

	//Meter Count
	memcpy(m_lpBuffer + offset, &m_meterCount, sizeof(byte));
	offset+= sizeof(byte);

	PollerMeter pollerMeter;
	CMeter *lpMeter;

	//Meters
	for(int n = 0; n < m_meterCount; n++)
	{
		lpMeter = m_meters->GetMeterByIndex(n);
		pollerMeter.Number = lpMeter->GetMeterNumber();
		pollerMeter.Value = lpMeter->GetValue();

		memcpy(m_lpBuffer + offset, &(pollerMeter.Number), sizeof(pollerMeter.Number));
		offset+=sizeof(pollerMeter.Number);

		memcpy(m_lpBuffer + offset, &(pollerMeter.Value), sizeof(pollerMeter.Value));
		offset+=sizeof(pollerMeter.Value);
	}

	// Transaction Data
	byte transDataType;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_DataCount, sizeof(byte));
	offset += sizeof(byte);

	// Session Points
	transDataType = DATA_TYPE_SESSION_POINTS;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);

	byte dataSize = sizeof(DWORD);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);

	DWORD Pts = m_player->GetSessionPoints();
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &Pts, sizeof(Pts));
	offset += sizeof(Pts);

	// Bonus Points
	transDataType = DATA_TYPE_BONUS_POINTS;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);

	dataSize = sizeof(DWORD);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);

	Pts = m_bonusEvent->GetAwardedPoints();
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &Pts, sizeof(Pts));
	offset += sizeof(Pts);

	// Residual Value/Countdown
	transDataType = DATA_TYPE_RESIDUAL_VALUE;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);

	dataSize = sizeof(DWORD);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);

	Pts = m_player->GetCurrentCountdown();
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &Pts, sizeof(Pts));
	offset += sizeof(Pts);

	// Total Coin Out
	transDataType = DATA_TYPE_TOTAL_COIN_OUT;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);

	dataSize = sizeof(DWORD);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);

	__int64 mgCO = 0, mmBonusMach = 0, mmPrgPayMac = 0;
	CMeter *meter;
	meter = m_meters->GetMeterByNumber(gCO);
	if (meter)
		mgCO = meter->GetValue();

	meter = m_meters->GetMeterByNumber(mBonusMach);
	if (meter)
		mmBonusMach = meter->GetValue();

	meter = m_meters->GetMeterByNumber(mPrgPayMac);
	if (meter)
		mmPrgPayMac = meter->GetValue();

	__int64 totalCoinOut = mgCO + mmBonusMach + mmPrgPayMac;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &totalCoinOut, sizeof(totalCoinOut));
	offset += sizeof(totalCoinOut);

	// Bonus EventID
	transDataType = DATA_TYPE_BONUS_EVENT_ID;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);

	dataSize = sizeof(DWORD);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);

	Pts = m_bonusEvent->GetEventID();
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &Pts, sizeof(Pts));
}
