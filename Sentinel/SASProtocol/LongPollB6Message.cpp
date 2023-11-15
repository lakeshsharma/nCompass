#include "stdafx.h"
#include "LongPollB6Message.h"

CLongPollB6Message::CLongPollB6Message(byte gameAddress)
: CSASPollMessage(gameAddress)
{
}

CLongPollB6Message::~CLongPollB6Message(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPollB6Message::GetPollValue()
{
	return 0xB6;
}

bool CLongPollB6Message::SwapPollValue()
{
	// what do we do here?
	return false;
}

void CLongPollB6Message::ResponseReceived()
{
	BYTE *response = GetResponseBuffer();

	if (GetReturnValue() == SASPOLL_SUCCESS)
	{
		// copy status byte from response
		memcpy(&m_status, response + 3, 1);
	}

	CSASPollMessage::ResponseReceived();

	return;

}

void CLongPollB6Message::Fill()
{
	// allocate buffers
	byte *crc;

	SetSendBuffer((BYTE *) new byte[XB6_POLLSIZE],XB6_POLLSIZE);
	SetResponseBuffer((BYTE *)new byte[XB6_RESPONSESIZE],XB6_RESPONSESIZE);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	memset(GetResponseBuffer(),0,GetResponseBufferSize());

	// fill long poll command data
	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0xB6;
	m_sendBuffer[2] = 1;
	m_sendBuffer[3] = m_status;
	
	ushort commandCRC = CRC(m_sendBuffer, XB6_POLLSIZE - CRCSIZE,0);
	crc = (byte *)&m_sendBuffer[XB6_POLLSIZE - CRCSIZE];
	memcpy(crc,&commandCRC,CRCSIZE);


	return;
}

void CLongPollB6Message::SetAcknowledge()
{
	m_status = 0x00;
}

void CLongPollB6Message::SetReady()
{
	m_status = 0x01;
}

void CLongPollB6Message::SetUnable()
{
	m_status = 0x80;
}

byte CLongPollB6Message::GetStatus()
{
	return m_status;
}
