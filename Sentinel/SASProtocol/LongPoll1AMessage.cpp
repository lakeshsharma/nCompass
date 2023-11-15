#include "stdafx.h"
#include "LongPoll1AMessage.h"

CLongPoll1AMessage::CLongPoll1AMessage(byte gameAddress, CMeters *meters)
	: CMetersLongPoll(gameAddress)
{
	// allocate buffers
	SetSendBuffer((byte *)new byte[2],2);
	SetResponseBuffer((byte*)new byte[8],8);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	memset(GetResponseBuffer(),0,GetResponseBufferSize());

	m_meters = meters;
}

CLongPoll1AMessage::~CLongPoll1AMessage(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll1AMessage::GetPollValue()
{
	return 0x1A;
}

bool CLongPoll1AMessage::SwapPollValue()
{
	// what do we do here?
	return 0;
}

void CLongPoll1AMessage::ResponseReceived()
{
	if (m_nReturnValue == SASPOLL_SUCCESS)
	{
		byte *response = GetResponseBuffer();

		m_currentCredits = Bcd2Int64(response + 2,4);
		if (m_meters)
			m_meters->SetMeter( mCrd, m_currentCredits);
	}
	CSASPollMessage::ResponseReceived();

	return;

}

void CLongPoll1AMessage::Fill()
{
	// fill long poll command data
	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0x1A;

	return;
}

