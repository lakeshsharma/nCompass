#include "stdafx.h"
#include "LongPoll1CMessage.h"

/// <summary>
/// Initializes a new instance of the <see cref="CLongPoll1CMessage"/> class.
/// </summary>
/// <param name="gameAddress">The game address.</param>
/// <param name="meters">The meters.</param>
CLongPoll1CMessage::CLongPoll1CMessage(byte gameAddress, CMeters *meters)
	: CMetersLongPoll(gameAddress)
{
	// allocate buffers
	SetSendBuffer((BYTE *)new byte[2],2);
	SetResponseBuffer((BYTE *)new byte[36],36);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	memset(GetResponseBuffer(),0,GetResponseBufferSize());

	m_meters = meters;
}

CLongPoll1CMessage::~CLongPoll1CMessage(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll1CMessage::GetPollValue()
{
	return 0x1C;
}

bool CLongPoll1CMessage::SwapPollValue()
{
	// what do we do here?
	return 0;
}

void CLongPoll1CMessage::ResponseReceived()
{
	if (m_nReturnValue == SASPOLL_SUCCESS)
	{
		BYTE *response = GetResponseBuffer();
		CMeter *meter;
		if (m_meters)
		{
			m_coinIn = Bcd2Int64(response+2,4);
			meter = m_meters->GetMeterByNumber( gCI );
			if ( meter != NULL && meter->GetValue() == INVALID_METER_VALUE)
				m_meters->SetMeter( gCI, m_coinIn);

			m_coinOut = Bcd2Int64(response+6,4);
			meter = m_meters->GetMeterByNumber( gCO );
			if ( meter != NULL && meter->GetValue() == INVALID_METER_VALUE)
				m_meters->SetMeter( gCO, m_coinOut);

			m_totalDrop = Bcd2Int64(response+10,4);
			meter = m_meters->GetMeterByNumber( mTotalDrop );
			if ( meter != NULL && meter->GetValue() == INVALID_METER_VALUE)
				m_meters->SetMeter( mTotalDrop, m_totalDrop);
	
			meter = m_meters->GetMeterByNumber(mCD);
			if (meter != NULL && meter->GetValue() == INVALID_METER_VALUE)
				m_meters->SetMeter(mCD, m_totalDrop);

			m_jackpot = Bcd2Int64(response + 14, 4);
			meter = m_meters->GetMeterByNumber( gJP );
			if ( meter != NULL && meter->GetValue() == INVALID_METER_VALUE)
				m_meters->SetMeter( gJP, m_jackpot);

			m_gamesPlayed = Bcd2Int64(response+18,4);
			meter = m_meters->GetMeterByNumber( gGS );
			if ( meter != NULL && meter->GetValue() == INVALID_METER_VALUE)
				m_meters->SetMeter( gGS, m_gamesPlayed);

			m_gamesWon = Bcd2Int64(response+22,4);
			meter = m_meters->GetMeterByNumber( gGW );
			if ( meter != NULL && meter->GetValue() == INVALID_METER_VALUE)
			m_meters->SetMeter( gGW, m_gamesWon);

			m_slotDoorOpen = Bcd2Int64(response+26,4);
			meter = m_meters->GetMeterByNumber( mActualSlotDr );
			if ( meter != NULL && meter->GetValue() == INVALID_METER_VALUE)
			m_meters->SetMeter( mActualSlotDr, m_slotDoorOpen);

			m_powerReset = Bcd2Int64(response+30,4);
			// m_powerReset doesn't have an oasis meter equivelant
		}
	}

	CSASPollMessage::ResponseReceived();

	return;

}

void CLongPoll1CMessage::Fill()
{
	// fill long poll command data
	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0x1C;

	return;
}

