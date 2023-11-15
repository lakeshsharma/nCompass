#include "stdafx.h"
#include "LongPoll6AMessage.h"

CLongPoll6AMessage::CLongPoll6AMessage(byte gameAddress)
: CSASPollMessage(gameAddress)
{
	// allocate buffers
	SetSendBuffer((byte *)new byte[2],2);
	SetResponseBuffer((byte*)new byte[8],8);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	memset(GetResponseBuffer(),0,GetResponseBufferSize());

	m_flag = 0;
}

CLongPoll6AMessage::~CLongPoll6AMessage(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll6AMessage::GetPollValue()
{
	return 0x6A;
}

bool CLongPoll6AMessage::SwapPollValue()
{
	// what do we do here?
	return 0;
}

void CLongPoll6AMessage::ResponseReceived()
{
	if (m_nReturnValue == SASPOLL_SUCCESS)
	{
		byte *response = GetResponseBuffer();

		memcpy(&m_flag, &response[5], sizeof(byte));

	}
	CSASPollMessage::ResponseReceived();

	return;

}

void CLongPoll6AMessage::Fill()
{
	// fill long poll command data
	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0x6A;

	return;
}

bool CLongPoll6AMessage::CanEFTDownload()
{
	bool isEnabled = false;
	if (m_flag & 1)
		isEnabled = true;
	return isEnabled;
}

bool CLongPoll6AMessage::CanEFTUpload()
{
	bool isEnabled = false;
	if (m_flag & 2)
		isEnabled = true;
	return isEnabled;
}

