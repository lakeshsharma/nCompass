#include "stdafx.h"
#include "LongPollB1Message.h"

CLongPollB1Message::CLongPollB1Message(byte gameAddress)
: CSASPollMessage(gameAddress)
{
	// allocate buffers
	SetSendBuffer((byte *)new byte[2],2);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	SetResponseBuffer((byte *)new byte[5],5);
	memset(GetSendBuffer(),0,GetSendBufferSize());
}

CLongPollB1Message::~CLongPollB1Message(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPollB1Message::GetPollValue()
{
	return 0xB1;
}

bool CLongPollB1Message::SwapPollValue()
{
	// what do we do here?
	return 0;
}

void CLongPollB1Message::ResponseReceived()
{
	try
	{
		if (m_nReturnValue == SASPOLL_SUCCESS)
		{
			byte *response = GetResponseBuffer();
			m_currentDenom = response[2];
		}
		CSASPollMessage::ResponseReceived();
	}
	catch (...)
	{
	}

	return;

}

void CLongPollB1Message::Fill()
{
	try
	{
		// fill long poll command data
		m_sendBuffer[0] = m_gameAddress;
		m_sendBuffer[1] = 0xB1; // b1 command
	}
	catch(...)
	{
	}
	return;
}
