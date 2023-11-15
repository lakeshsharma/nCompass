#include "stdafx.h"
#include "LongPoll51Message.h"

CLongPoll51Message::CLongPoll51Message(byte gameAddress)
: CSASPollMessage(gameAddress)
{
	// allocate buffers
	SetSendBuffer((byte *)new byte[2],2);
	SetResponseBuffer((byte*)new byte[6],6);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	memset(GetResponseBuffer(),0,GetResponseBufferSize());
	m_numberSubGamesImplemented = 0;
}

CLongPoll51Message::~CLongPoll51Message(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll51Message::GetPollValue()
{
	return 0x51;
}

bool CLongPoll51Message::SwapPollValue()
{
	// what do we do here?
	return 0;
}

void CLongPoll51Message::ResponseReceived()
{
	if (m_nReturnValue == SASPOLL_SUCCESS)
	{
		byte *response = GetResponseBuffer();

		m_numberSubGamesImplemented = Bcd2Dword(response + 2,2);
	}
	else
	{
		m_numberSubGamesImplemented = 0;	
	}

	CSASPollMessage::ResponseReceived();

	return;

}

void CLongPoll51Message::Fill()
{
	// fill long poll command data
	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0x51;

	return;
}
