#include "stdafx.h"
#include "LongPoll94Message.h"

CLongPoll94Message::CLongPoll94Message(byte gameAddress)
: CSASPollMessage(gameAddress)
{
	// allocate buffers
	SetSendBuffer((BYTE *)new byte[4],4);
	SetResponseBuffer((BYTE *)new byte[5],5);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	memset(GetResponseBuffer(),0,GetResponseBufferSize());

	m_ACKCode = 0;
}


CLongPoll94Message::~CLongPoll94Message(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll94Message::GetPollValue()
{
	return 0x94;
}

bool CLongPoll94Message::SwapPollValue()
{
	return 0;
}

void CLongPoll94Message::ResponseReceived()
{
	
	BYTE *response = GetResponseBuffer();
	BYTE *send = GetSendBuffer();
	if (response[1] != send[1])
		m_nReturnValue = SASPOLL_INCOMPLETE_DATA;

	if (m_nReturnValue == SASPOLL_SUCCESS)
	{

		response += 2; // address and command byte

		memcpy(&m_ACKCode, response, sizeof(byte));

	}

	CSASPollMessage::ResponseReceived();

	return;
}

void CLongPoll94Message::Fill()
{
	// fill long poll command data
	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0x94;

	ushort messageCRC = CRC(GetSendBuffer(),GetSendBufferSize() - CRCSIZE,0);
	byte *crc = (byte *)&m_sendBuffer[2];
	memcpy(crc,&messageCRC,CRCSIZE);
}

byte CLongPoll94Message::GetAckCode()
{
	return m_ACKCode;
}
