#include "stdafx.h"
#include "LongPoll9AMessage.h"

CLongPoll9AMessage::CLongPoll9AMessage(byte gameAddress, CMeters *meters)
	: CMetersLongPoll(gameAddress)
{
	const int SendBufferLength = 6;
	const int ResponseBufferLength = 18;

	// allocate buffers
	SetSendBuffer((BYTE *)new byte[SendBufferLength], SendBufferLength);
	SetResponseBuffer((BYTE *)new byte[ResponseBufferLength], ResponseBufferLength);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	memset(GetResponseBuffer(),0,GetResponseBufferSize());

	m_meters = meters;
}

CLongPoll9AMessage::~CLongPoll9AMessage(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll9AMessage::GetPollValue()
{
	return PollValue;
}

bool CLongPoll9AMessage::SwapPollValue()
{
	return false;
}

void CLongPoll9AMessage::ResponseReceived()
{
	const int BcdMeterLength = 4;
	const int MeterOffset = 4;

	if (m_nReturnValue == SASPOLL_SUCCESS)
	{
		BYTE *response = GetResponseBuffer();
		BYTE *responseOffset = response + MeterOffset;
		m_deductibleBonus = Bcd2Int64(responseOffset, BcdMeterLength);

		responseOffset += BcdMeterLength;
		m_nonDeductibleBonus = Bcd2Int64(responseOffset, BcdMeterLength);

		responseOffset += BcdMeterLength;
		m_wagerMatchBonus = Bcd2Int64(responseOffset, BcdMeterLength);

		if (NULL != m_meters)
		{
			CMeter *meter;

			meter = m_meters->GetMeterByNumber(gBW);
			if (meter != NULL && meter->GetValue() == INVALID_METER_VALUE)
			{
				m_meters->SetMeter(gBW, m_deductibleBonus + m_nonDeductibleBonus + m_wagerMatchBonus);
			}
		}
	}

	CSASPollMessage::ResponseReceived();
}

void CLongPoll9AMessage::Fill()
{
	const int BcdGameNumberLength = 2;
	BYTE *sendOffset = m_sendBuffer;

	// fill long poll command data
	*sendOffset = m_gameAddress;
	sendOffset += sizeof(BYTE);

	*sendOffset = PollValue;
	sendOffset += sizeof(BYTE);

	memcpy(sendOffset, &m_gameNumber, BcdGameNumberLength);
	sendOffset += BcdGameNumberLength;

	byte *crcLocation;
	ushort messageCRC = CRC(GetSendBuffer(), GetSendBufferSize() - CRCSIZE, 0);
	crcLocation = (byte *)sendOffset;
	memcpy(crcLocation, &messageCRC, CRCSIZE);
}

void CLongPoll9AMessage::SetGameNumber(int gameNumber)
{
	m_gameNumber = gameNumber;
}

