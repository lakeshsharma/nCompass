#include "stdafx.h"
#include "LongPoll33Message.h"

CLongPoll33Message::CLongPoll33Message(byte gameAddress, CMeters *meters)
	: CMetersLongPoll(gameAddress)
{
	// allocate buffers
	SetSendBuffer((byte *)new byte[2], 2);
	SetResponseBuffer((byte*)new byte[8], 8);
	memset(GetSendBuffer(), 0, GetSendBufferSize());
	memset(GetResponseBuffer(), 0, GetResponseBufferSize());

	m_meters = meters;
}

CLongPoll33Message::~CLongPoll33Message(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll33Message::GetPollValue()
{
	return 0x33;
}

bool CLongPoll33Message::SwapPollValue()
{
	// what do we do here?
	return 0;
}

void CLongPoll33Message::ResponseReceived()
{
	if (m_nReturnValue == SASPOLL_SUCCESS)
	{
		byte *response = GetResponseBuffer();

		m_five = Bcd2Int64(response + 2, 4);
		if (m_meters)
			m_meters->SetMeter(mD5, m_five);
	}
	CSASPollMessage::ResponseReceived();

	return;

}

void CLongPoll33Message::Fill()
{
	// fill long poll command data
	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0x33;

	return;
}

