#include "stdafx.h"
#include "LongPoll10Message.h"

CLongPoll10Message::CLongPoll10Message(byte gameAddress, CMeters *meters)
	: CMetersLongPoll(gameAddress)
{
	// allocate buffers
	SetSendBuffer((byte *)new byte[2],2);
	SetResponseBuffer((byte*)new byte[8],8);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	memset(GetResponseBuffer(),0,GetResponseBufferSize());

	m_meters = meters;
}

CLongPoll10Message::~CLongPoll10Message(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll10Message::GetPollValue()
{
	return 0x10;
}

bool CLongPoll10Message::SwapPollValue()
{
	// what do we do here?
	return 0;
}

void CLongPoll10Message::ResponseReceived()
{
	byte *response = GetResponseBuffer();

	if (m_nReturnValue == SASPOLL_SUCCESS)
	{
		m_cancelledCredits = Bcd2Int64(response + 2,4);
		m_meters->SetMeter( gCC, m_cancelledCredits );
	}

	CSASPollMessage::ResponseReceived();

	return;

}

void CLongPoll10Message::Fill()
{
	// fill long poll command data
	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0x10;

	return;
}

