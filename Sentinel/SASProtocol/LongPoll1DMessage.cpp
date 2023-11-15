#include "stdafx.h"
#include "LongPoll1DMessage.h"

CLongPoll1DMessage::CLongPoll1DMessage(byte gameAddress, CMeters *meters)
	: CMetersLongPoll(gameAddress)
{
	// allocate buffers
	SetSendBuffer((BYTE *)new byte[2],2);
	SetResponseBuffer((BYTE *)new byte[20],20);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	memset(GetResponseBuffer(),0,GetResponseBufferSize());

	m_meters = meters;
}

CLongPoll1DMessage::~CLongPoll1DMessage(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll1DMessage::GetPollValue()
{
	return 0x1D;
}

bool CLongPoll1DMessage::SwapPollValue()
{
	// what do we do here?
	return 0;
}

void CLongPoll1DMessage::SetDenomMultiplier(WORD denomMult)
{
	m_denomMult = denomMult;
}

void CLongPoll1DMessage::ResponseReceived()
{
	if (m_nReturnValue == SASPOLL_SUCCESS)
	{
		BYTE *response = GetResponseBuffer();
		if (m_meters)
		{
			CMeter *meter;
			m_promoCreditIn = Bcd2Int64(response+2,4);
			meter = m_meters->GetMeterByNumber(mPbAR2G);
			if (meter != NULL && meter->GetValue() == INVALID_METER_VALUE)
				m_meters->SetMeter( mPbAR2G, m_promoCreditIn);
			m_nonCashCreditIn = Bcd2Int64(response+6,4);
			meter = m_meters->GetMeterByNumber(mPbAN2G);
			if (meter != NULL && meter->GetValue() == INVALID_METER_VALUE)
				m_meters->SetMeter( mPbAN2G, m_nonCashCreditIn);
			m_transferredCredits = Bcd2Int64(response+10,4);
			meter = m_meters->GetMeterByNumber(mPbAC2H);
			if (meter != NULL && meter->GetValue() == INVALID_METER_VALUE)
				m_meters->SetMeter( mPbAC2H, m_transferredCredits);
			m_cashableCredits = Bcd2Int64(response+14,4);
			meter = m_meters->GetMeterByNumber(mPbAC2G);
			if (meter != NULL && meter->GetValue() == INVALID_METER_VALUE)
				m_meters->SetMeter( mPbAC2G, m_cashableCredits);
		}
	}

	CSASPollMessage::ResponseReceived();

	return;

}

void CLongPoll1DMessage::Fill()
{
	// fill long poll command data
	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0x1D;

	return;
}

