#include "stdafx.h"
#include "LongPoll84Message.h"

CLongPoll84Message::CLongPoll84Message(byte gameAddress)
: CSASPollMessage(gameAddress)
{
	// allocate buffers
	SetSendBuffer((BYTE *)new byte[2],2);
	SetResponseBuffer((BYTE *)new byte[11],11);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	memset(GetResponseBuffer(),0,GetResponseBufferSize());

}

CLongPoll84Message::~CLongPoll84Message(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll84Message::GetPollValue()
{
	return 0x84;
}

bool CLongPoll84Message::SwapPollValue()
{
	return 0;
}

void CLongPoll84Message::ResponseReceived()
{
	if (m_nReturnValue == SASPOLL_SUCCESS)
	{
		BYTE *response = GetResponseBuffer();

		response += 2; // address and command byte

		memcpy(&m_progressiveGroup, response, sizeof(byte));
		response += sizeof(byte);

		memcpy(&m_progressiveLevel, response, sizeof(byte));
		response += sizeof(byte);

		m_amount = Bcd2Int64(response,5);

	}

	CSASPollMessage::ResponseReceived();

	return;

}

void CLongPoll84Message::Fill()
{
	// fill long poll command data
	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0x84;

	return;
}

