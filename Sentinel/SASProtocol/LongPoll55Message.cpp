#include "stdafx.h"
#include "LongPoll55Message.h"

CLongPoll55Message::CLongPoll55Message(byte gameAddress)
: CSASPollMessage(gameAddress)
{
	// allocate buffers
	SetSendBuffer((byte *)new byte[2],2);
	SetResponseBuffer((byte*)new byte[6],6);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	memset(GetResponseBuffer(),0,GetResponseBufferSize());
}

CLongPoll55Message::~CLongPoll55Message(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll55Message::GetPollValue()
{
	return 0x55;
}

bool CLongPoll55Message::SwapPollValue()
{
	// what do we do here?
	return 0;
}

void CLongPoll55Message::ResponseReceived()
{
	byte *response = GetResponseBuffer();

	m_selectedGameNumber = Bcd2Dword(response + 2,2);

	CSASPollMessage::ResponseReceived();

	return;

}

void CLongPoll55Message::Fill()
{
	// fill long poll command data
	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0x55;

	return;
}
