#include "stdafx.h"
#include "LongPoll27Message.h"
#include "SASProtocol.h"

CLongPoll27Message::CLongPoll27Message(byte gameAddress, CMeters *meters)
	: CMetersLongPoll(gameAddress)
{
	// allocate buffers
	SetSendBuffer((BYTE *) new byte[2],2);
	SetResponseBuffer((BYTE *)new byte[8],8);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	memset(GetResponseBuffer(),0,GetResponseBufferSize());

	m_meters = meters;
}

CLongPoll27Message::~CLongPoll27Message(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll27Message::GetPollValue()
{
	return 0x27;
}

bool CLongPoll27Message::SwapPollValue()
{
	// what do we do here?
	return 0;
}

void CLongPoll27Message::ResponseReceived()
{
	if (m_nReturnValue == SASPOLL_SUCCESS)
	{
		BYTE *response = GetResponseBuffer();
		m_promoCredits = Bcd2Int64(response+2,4);

		if (m_meters)
			m_meters->SetMeter( mNcepCredits, m_promoCredits);
	}	
	CSASPollMessage::ResponseReceived();

	return;

}

void CLongPoll27Message::Fill()
{
	// fill long poll command data
	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0x27;

	return;
}

