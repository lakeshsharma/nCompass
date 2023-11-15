#include "stdafx.h"
#include "LongPoll2AMessage.h"
#include "SASProtocol.h"

CLongPoll2AMessage::CLongPoll2AMessage(byte gameAddress, CMeters *meters)
	: CMetersLongPoll(gameAddress)
{
	// allocate buffers
	SetSendBuffer((BYTE *) new byte[2],2);
	SetResponseBuffer((BYTE *)new byte[8],8);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	memset(GetResponseBuffer(),0,GetResponseBufferSize());

	m_meters = meters;
}

CLongPoll2AMessage::~CLongPoll2AMessage(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll2AMessage::GetPollValue()
{
	return 0x2A;
}

bool CLongPoll2AMessage::SwapPollValue()
{
	// what do we do here?
	return 0;
}

void CLongPoll2AMessage::ResponseReceived()
{
	if (m_nReturnValue == SASPOLL_SUCCESS)
	{
		BYTE *response = GetResponseBuffer();
		m_trueCoinIn = Bcd2Int64(response+2,4);

		if (m_meters)
			m_meters->SetMeter( mTCi, m_trueCoinIn);
	}	
	CSASPollMessage::ResponseReceived();

	return;

}

void CLongPoll2AMessage::Fill()
{
	// fill long poll command data
	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0x2A;

	return;
}

