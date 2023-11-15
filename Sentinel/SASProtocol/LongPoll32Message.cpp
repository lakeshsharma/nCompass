#include "stdafx.h"
#include "LongPoll32Message.h"

CLongPoll32Message::CLongPoll32Message(byte gameAddress, CMeters *meters)
	: CMetersLongPoll(gameAddress)
{
	// allocate buffers
	SetSendBuffer((byte *)new byte[2],2);
	SetResponseBuffer((byte*)new byte[8],8);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	memset(GetResponseBuffer(),0,GetResponseBufferSize());

	m_meters = meters;
}

CLongPoll32Message::~CLongPoll32Message(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll32Message::GetPollValue()
{
	return 0x32;
}

bool CLongPoll32Message::SwapPollValue()
{
	// what do we do here?
	return 0;
}

void CLongPoll32Message::ResponseReceived()
{
	if (m_nReturnValue == SASPOLL_SUCCESS)
	{
		byte *response = GetResponseBuffer();

		m_two = Bcd2Int64(response + 2,4);
		if (m_meters)
			m_meters->SetMeter( mD2, m_two);
	}
	CSASPollMessage::ResponseReceived();

	return;

}

void CLongPoll32Message::Fill()
{
	// fill long poll command data
	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0x32;

	return;
}

