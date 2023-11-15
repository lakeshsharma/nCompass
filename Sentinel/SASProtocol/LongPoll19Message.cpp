#include "stdafx.h"
#include "LongPoll19Message.h"

CLongPoll19Message::CLongPoll19Message(byte gameAddress, CMeters *meters)
	: CMetersLongPoll(gameAddress)
{
	// allocate buffers
	SetSendBuffer((BYTE *)new byte[2],2);
	SetResponseBuffer((BYTE *)new byte[24],24);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	memset(GetResponseBuffer(),0,GetResponseBufferSize());

	m_meters = meters;
}

CLongPoll19Message::~CLongPoll19Message(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll19Message::GetPollValue()
{
	return 0x19;
}

bool CLongPoll19Message::SwapPollValue()
{
	// what do we do here?
	return 0;
}

void CLongPoll19Message::ResponseReceived()
{
	if (m_nReturnValue == SASPOLL_SUCCESS)
	{

		BYTE *response = GetResponseBuffer();
		CMeter *meter;
		m_coinIn = Bcd2Int64(response+2,4);
		if (m_meters)
		{
			meter = m_meters->GetMeterByNumber( gCI );
			if ( meter != NULL && meter->GetValue() == INVALID_METER_VALUE)
				m_meters->SetMeter( gCI, m_coinIn);
		}

		m_coinOut = Bcd2Int64(response+6,4);
		if (m_meters)
		{
			meter = m_meters->GetMeterByNumber( gCO );
			if ( meter != NULL && meter->GetValue() == INVALID_METER_VALUE)
				m_meters->SetMeter( gCO, m_coinOut);
		}

		m_totalDrop = Bcd2Int64(response+10,4);
		if (m_meters)
		{
			meter = m_meters->GetMeterByNumber( mTotalDrop );
			if ( meter != NULL && meter->GetValue() == INVALID_METER_VALUE)
				m_meters->SetMeter( mTotalDrop, m_totalDrop);

			meter = m_meters->GetMeterByNumber(mCD);
			if (meter != NULL && meter->GetValue() == INVALID_METER_VALUE)
				m_meters->SetMeter(mCD, m_totalDrop);
		}

		m_jackpot = Bcd2Int64(response+14,4);
		if (m_meters)
		{
			meter = m_meters->GetMeterByNumber( gJP );
			if ( meter != NULL && meter->GetValue() == INVALID_METER_VALUE)
				m_meters->SetMeter( gJP, m_jackpot);
		}

		m_gamesPlayed = Bcd2Int64(response+18,4);
		if (m_meters)
		{
			meter = m_meters->GetMeterByNumber( gGS );
			if ( meter != NULL && meter->GetValue() == INVALID_METER_VALUE)
				m_meters->SetMeter( gGS, m_gamesPlayed);
		}
	}
	CSASPollMessage::ResponseReceived();

	return;
 
}

void CLongPoll19Message::Fill()
{
	// fill long poll command data
	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0x19;

	return;
}

