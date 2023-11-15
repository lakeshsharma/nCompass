#include "stdafx.h"
#include "LongPoll46Message.h"

CLongPoll46Message::CLongPoll46Message(byte gameAddress, CMeters *meters)
	: CMetersLongPoll(gameAddress)
{
	// allocate buffers
	SetSendBuffer((byte *)new byte[2],2);
	SetResponseBuffer((byte*)new byte[8],8);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	memset(GetResponseBuffer(),0,GetResponseBufferSize());

	m_meters = meters;
}

CLongPoll46Message::~CLongPoll46Message(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll46Message::GetPollValue()
{
	return 0x46;
}

bool CLongPoll46Message::SwapPollValue()
{
	// what do we do here?
	return 0;
}

void CLongPoll46Message::ResponseReceived()
{
	if (m_nReturnValue == SASPOLL_SUCCESS)
	{
		byte *response = GetResponseBuffer();

		m_billsAcceptedCredits = Bcd2Int64(response + 2,4);
		if (m_meters)
			m_meters->SetMeter( mBD, m_billsAcceptedCredits);
	}
	CSASPollMessage::ResponseReceived();

	return;

}

void CLongPoll46Message::Fill()
{
	// fill long poll command data
	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0x46;

	return;
}

