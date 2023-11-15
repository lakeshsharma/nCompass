#include "stdafx.h"
#include "LongPoll85Message.h"

CLongPoll85Message::CLongPoll85Message(byte gameAddress)
: CSASPollMessage(gameAddress)
{
	// allocate buffers
	SetSendBuffer((BYTE *)new byte[2],2);
	SetResponseBuffer((BYTE *)new byte[11],11);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	memset(GetResponseBuffer(),0,GetResponseBufferSize());

}

CLongPoll85Message::~CLongPoll85Message(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll85Message::GetPollValue()
{
	return 0x85;
}

bool CLongPoll85Message::SwapPollValue()
{
	return 0;
}

void CLongPoll85Message::ResponseReceived()
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

void CLongPoll85Message::Fill()
{
	// fill long poll command data
	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0x85;

	return;
}

