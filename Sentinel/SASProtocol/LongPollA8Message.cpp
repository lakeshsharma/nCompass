#include "stdafx.h"
#include "LongPollA8Message.h"

CLongPollA8Message::CLongPollA8Message(byte gameAddress)
: CSASPollMessage(gameAddress)
{
	// allocate buffers
	SetSendBuffer((BYTE *)new byte[5],5);
	SetResponseBuffer((BYTE *)new byte[5],5);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	memset(GetResponseBuffer(),0,GetResponseBufferSize());

	m_ACKCode = 0;
	m_resetMethod = 0;
}


CLongPollA8Message::~CLongPollA8Message(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPollA8Message::GetPollValue()
{
	return 0xA8;
}

bool CLongPollA8Message::SwapPollValue()
{
	return 0;
}

void CLongPollA8Message::ResponseReceived()
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

void CLongPollA8Message::Fill()
{
	// fill long poll command data
	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0xA8;
	m_sendBuffer[2] = m_resetMethod;

	ushort messageCRC = CRC(GetSendBuffer(),GetSendBufferSize() - CRCSIZE,0);
	byte *crc = (byte *)&m_sendBuffer[3];
	memcpy(crc,&messageCRC,CRCSIZE);
}

void CLongPollA8Message::SetStandardHandpay()
{
	m_resetMethod = 0;
}

void CLongPollA8Message::SetCreditReset()
{
	m_resetMethod = 1;
}

byte CLongPollA8Message::GetAckCode()
{
	return m_ACKCode;
}
