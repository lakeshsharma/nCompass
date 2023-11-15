#include "stdafx.h"

#include "LongPoll38Message.h"

CLongPoll38Message::CLongPoll38Message(byte gameAddress, CMeters *meters)
	: CMetersLongPoll(gameAddress)
{
	// allocate buffers
	SetSendBuffer((byte *)new byte[2],2);
	SetResponseBuffer((byte*)new byte[8],8);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	memset(GetResponseBuffer(),0,GetResponseBufferSize());

	m_meters = meters;
}

CLongPoll38Message::~CLongPoll38Message(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll38Message::GetPollValue()
{
	return 0x38;
}

bool CLongPoll38Message::SwapPollValue()
{
	// what do we do here?
	return 0;
}

void CLongPoll38Message::ResponseReceived()
{
	if (m_nReturnValue == SASPOLL_SUCCESS)
	{
		byte *response = GetResponseBuffer();

		m_fivehundred = Bcd2Int64(response + 2,4);
		if (m_meters)
			m_meters->SetMeter( mD500, m_fivehundred);
	}
	CSASPollMessage::ResponseReceived();

	return;

}

void CLongPoll38Message::Fill()
{
	// fill long poll command data
	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0x38;

	return;
}

