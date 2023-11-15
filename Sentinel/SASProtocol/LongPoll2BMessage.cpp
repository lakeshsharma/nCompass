#include "stdafx.h"
#include "LongPoll2BMessage.h"
#include "SASProtocol.h"

CLongPoll2BMessage::CLongPoll2BMessage(byte gameAddress, CMeters *meters)
	: CMetersLongPoll(gameAddress)
{
	// allocate buffers
	SetSendBuffer((BYTE *) new byte[2],2);
	SetResponseBuffer((BYTE *)new byte[8],8);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	memset(GetResponseBuffer(),0,GetResponseBufferSize());

	m_meters = meters;
}

CLongPoll2BMessage::~CLongPoll2BMessage(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll2BMessage::GetPollValue()
{
	return 0x2B;
}

bool CLongPoll2BMessage::SwapPollValue()
{
	// what do we do here?
	return 0;
}

void CLongPoll2BMessage::ResponseReceived()
{
	if (m_nReturnValue == SASPOLL_SUCCESS)
	{
		BYTE *response = GetResponseBuffer();
		m_trueCoinOut = Bcd2Int64(response+2,4);

		if (m_meters)
			m_meters->SetMeter( mTCo, m_trueCoinOut);
	}	
	CSASPollMessage::ResponseReceived();

	return;

}

void CLongPoll2BMessage::Fill()
{
	// fill long poll command data
	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0x2B;

	return;
}

