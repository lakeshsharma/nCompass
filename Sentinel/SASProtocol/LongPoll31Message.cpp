#include "stdafx.h"
#include "LongPoll31Message.h"

CLongPoll31Message::CLongPoll31Message(byte gameAddress, CMeters *meters)
	: CMetersLongPoll(gameAddress)
{
	// allocate buffers
	SetSendBuffer((byte *)new byte[2], 2);
	SetResponseBuffer((byte*)new byte[8], 8);
	memset(GetSendBuffer(), 0, GetSendBufferSize());
	memset(GetResponseBuffer(), 0, GetResponseBufferSize());

	m_meters = meters;
}

CLongPoll31Message::~CLongPoll31Message(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll31Message::GetPollValue()
{
	return 0x31;
}

bool CLongPoll31Message::SwapPollValue()
{
	// what do we do here?
	return 0;
}

void CLongPoll31Message::ResponseReceived()
{
	if (m_nReturnValue == SASPOLL_SUCCESS)
	{
		byte *response = GetResponseBuffer();

		m_one = Bcd2Int64(response + 2, 4);
		if (m_meters)
			m_meters->SetMeter(mD1, m_one);
	}
	CSASPollMessage::ResponseReceived();

	return;

}

void CLongPoll31Message::Fill()
{
	// fill long poll command data
	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0x31;

	return;
}

