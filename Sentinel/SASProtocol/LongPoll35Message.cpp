#include "stdafx.h"
#include "LongPoll35Message.h"

CLongPoll35Message::CLongPoll35Message(byte gameAddress, CMeters *meters)
	: CMetersLongPoll(gameAddress)
{
	// allocate buffers
	SetSendBuffer((byte *)new byte[2], 2);
	SetResponseBuffer((byte*)new byte[8], 8);
	memset(GetSendBuffer(), 0, GetSendBufferSize());
	memset(GetResponseBuffer(), 0, GetResponseBufferSize());

	m_meters = meters;
}

CLongPoll35Message::~CLongPoll35Message(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll35Message::GetPollValue()
{
	return 0x35;
}

bool CLongPoll35Message::SwapPollValue()
{
	// what do we do here?
	return 0;
}

void CLongPoll35Message::ResponseReceived()
{
	if (m_nReturnValue == SASPOLL_SUCCESS)
	{
		byte *response = GetResponseBuffer();

		m_twenty = Bcd2Int64(response + 2, 4);
		if (m_meters)
			m_meters->SetMeter(mD20, m_twenty);
	}
	CSASPollMessage::ResponseReceived();

	return;

}

void CLongPoll35Message::Fill()
{
	// fill long poll command data
	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0x35;

	return;
}

