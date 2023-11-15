#include "stdafx.h"
#include "LongPoll3AMessage.h"

CLongPoll3AMessage::CLongPoll3AMessage(byte gameAddress, CMeters *meters)
	: CMetersLongPoll(gameAddress)
{
	// allocate buffers
	SetSendBuffer((byte *)new byte[2],2);
	SetResponseBuffer((byte*)new byte[8],8);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	memset(GetResponseBuffer(),0,GetResponseBufferSize());

	m_meters = meters;
}

CLongPoll3AMessage::~CLongPoll3AMessage(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll3AMessage::GetPollValue()
{
	return 0x3A;
}

bool CLongPoll3AMessage::SwapPollValue()
{
	// what do we do here?
	return 0;
}

void CLongPoll3AMessage::ResponseReceived()
{
	if (m_nReturnValue == SASPOLL_SUCCESS)
	{
		byte *response = GetResponseBuffer();

		m_twohundred = Bcd2Int64(response + 2,4);
		if (m_meters)
			m_meters->SetMeter( mD200, m_twohundred);
	}
	CSASPollMessage::ResponseReceived();

	return;

}

void CLongPoll3AMessage::Fill()
{
	// fill long poll command data
	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0x3A;

	return;
}

