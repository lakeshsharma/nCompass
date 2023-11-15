#include "stdafx.h"
#include "LongPoll39Message.h"

CLongPoll39Message::CLongPoll39Message(byte gameAddress, CMeters *meters)
	: CMetersLongPoll(gameAddress)
{
	// allocate buffers
	SetSendBuffer((byte *)new byte[2],2);
	SetResponseBuffer((byte*)new byte[8],8);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	memset(GetResponseBuffer(),0,GetResponseBufferSize());

	m_meters = meters;
}

CLongPoll39Message::~CLongPoll39Message(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll39Message::GetPollValue()
{
	return 0x39;
}

bool CLongPoll39Message::SwapPollValue()
{
	// what do we do here?
	return 0;
}

void CLongPoll39Message::ResponseReceived()
{
	if (m_nReturnValue == SASPOLL_SUCCESS)
	{
		byte *response = GetResponseBuffer();

		m_thousand = Bcd2Int64(response + 2,4);
		if (m_meters)
			m_meters->SetMeter( mD1000, m_thousand);
	}
	CSASPollMessage::ResponseReceived();

	return;

}

void CLongPoll39Message::Fill()
{
	// fill long poll command data
	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0x39;

	return;
}

