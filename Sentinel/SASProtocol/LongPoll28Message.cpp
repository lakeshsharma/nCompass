#include "stdafx.h"
#include "LongPoll28Message.h"

CLongPoll28Message::CLongPoll28Message(byte gameAddress)
: CSASPollMessage(gameAddress)
{
	// allocate buffers
	SetSendBuffer((byte *)new byte[2],2);
	SetResponseBuffer((byte*)new byte[49],49);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	memset(GetResponseBuffer(),0,GetResponseBufferSize());
	m_EFTSeed = 0;
}

CLongPoll28Message::~CLongPoll28Message(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll28Message::GetPollValue()
{
	return 0x28;
}

bool CLongPoll28Message::SwapPollValue()
{
	// what do we do here?
	return 0;
}

void CLongPoll28Message::ResponseReceived()
{
	if (m_nReturnValue == SASPOLL_SUCCESS)
	{
		byte *response = GetResponseBuffer();
		response += 2; // skip address and command bytes

		// only using last transaction...
		memcpy(&m_lastCmd, response, sizeof(byte));
		response += sizeof(byte);

		memcpy(&m_EFTSeed,response,sizeof(byte));
		response += sizeof(byte);

		memcpy(&m_lastAck, response, sizeof(byte));
		response += sizeof(byte);

		memcpy(&m_lastStatus, response, sizeof(byte));
		response += sizeof(byte);

		m_lastAmount = Bcd2Dword(response, 5);
	}
	CSASPollMessage::ResponseReceived();

	return;

}

void CLongPoll28Message::Fill()
{
	// fill long poll command data
	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0x28;

	return;
}
