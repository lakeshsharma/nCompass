#include "stdafx.h"
#include "LongPoll37Message.h"

CLongPoll37Message::CLongPoll37Message(byte gameAddress, CMeters *meters)
	: CMetersLongPoll(gameAddress)
{
	// allocate buffers
	SetSendBuffer((byte *)new byte[2], 2);
	SetResponseBuffer((byte*)new byte[8], 8);
	memset(GetSendBuffer(), 0, GetSendBufferSize());
	memset(GetResponseBuffer(), 0, GetResponseBufferSize());

	m_meters = meters;
}

CLongPoll37Message::~CLongPoll37Message(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll37Message::GetPollValue()
{
	return 0x37;
}

bool CLongPoll37Message::SwapPollValue()
{
	// what do we do here?
	return 0;
}

void CLongPoll37Message::ResponseReceived()
{
	if (m_nReturnValue == SASPOLL_SUCCESS)
	{
		byte *response = GetResponseBuffer();

		m_hundred = Bcd2Int64(response + 2, 4);
		if (m_meters)
			m_meters->SetMeter(mD100, m_hundred);
	}
	CSASPollMessage::ResponseReceived();

	return;

}

void CLongPoll37Message::Fill()
{
	// fill long poll command data
	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0x37;

	return;
}

