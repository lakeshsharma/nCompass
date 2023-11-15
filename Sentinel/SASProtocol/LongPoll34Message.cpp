#include "stdafx.h"
#include "LongPoll34Message.h"

CLongPoll34Message::CLongPoll34Message(byte gameAddress, CMeters *meters)
	: CMetersLongPoll(gameAddress)
{
	// allocate buffers
	SetSendBuffer((byte *)new byte[2], 2);
	SetResponseBuffer((byte*)new byte[8], 8);
	memset(GetSendBuffer(), 0, GetSendBufferSize());
	memset(GetResponseBuffer(), 0, GetResponseBufferSize());

	m_meters = meters;
}

CLongPoll34Message::~CLongPoll34Message(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll34Message::GetPollValue()
{
	return 0x34;
}

bool CLongPoll34Message::SwapPollValue()
{
	// what do we do here?
	return 0;
}

void CLongPoll34Message::ResponseReceived()
{
	if (m_nReturnValue == SASPOLL_SUCCESS)
	{
		byte *response = GetResponseBuffer();

		m_ten = Bcd2Int64(response + 2, 4);
		if (m_meters)
			m_meters->SetMeter(mD10, m_ten);
	}
	CSASPollMessage::ResponseReceived();

	return;

}

void CLongPoll34Message::Fill()
{
	// fill long poll command data
	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0x34;

	return;
}

