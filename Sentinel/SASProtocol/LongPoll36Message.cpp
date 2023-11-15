#include "stdafx.h"
#include "LongPoll36Message.h"

CLongPoll36Message::CLongPoll36Message(byte gameAddress, CMeters *meters)
	: CMetersLongPoll(gameAddress)
{
	// allocate buffers
	SetSendBuffer((byte *)new byte[2], 2);
	SetResponseBuffer((byte*)new byte[8], 8);
	memset(GetSendBuffer(), 0, GetSendBufferSize());
	memset(GetResponseBuffer(), 0, GetResponseBufferSize());

	m_meters = meters;
}

CLongPoll36Message::~CLongPoll36Message(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll36Message::GetPollValue()
{
	return 0x36;
}

bool CLongPoll36Message::SwapPollValue()
{
	// what do we do here?
	return 0;
}

void CLongPoll36Message::ResponseReceived()
{
	if (m_nReturnValue == SASPOLL_SUCCESS)
	{
		byte *response = GetResponseBuffer();

		m_fifty = Bcd2Int64(response + 2, 4);
		if (m_meters)
			m_meters->SetMeter(mD50, m_fifty);
	}
	CSASPollMessage::ResponseReceived();

	return;

}

void CLongPoll36Message::Fill()
{
	// fill long poll command data
	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0x36;

	return;
}

