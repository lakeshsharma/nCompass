#include "stdafx.h"

#include "LongPoll01Message.h"

CLongPoll01Message::CLongPoll01Message(byte gameAddress)
: CSASPollMessage(gameAddress)
{
	// allocate buffers
	SetSendBuffer((byte *) new byte[LP01SENDSIZE],LP01SENDSIZE);
	SetResponseBuffer((byte*)new byte[LP01RECVSIZE],LP01RECVSIZE);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	memset(GetResponseBuffer(),0,GetResponseBufferSize());
}

CLongPoll01Message::~CLongPoll01Message(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll01Message::GetPollValue()
{
	return 0x01;
}

bool CLongPoll01Message::SwapPollValue()
{
	// what do we do here?
	return 0;
}

void CLongPoll01Message::ResponseReceived()
{


	CSASPollMessage::ResponseReceived();

	return;

}

void CLongPoll01Message::Fill()
{
	// fill long poll command data
	m_sendBuffer[GAMEADDRLOC] = m_gameAddress;
	m_sendBuffer[LPCOMMANDLOC] = 0x01;

	byte *crc;
	ushort messageCRC = CRC(GetSendBuffer(),GetSendBufferSize() - CRCSIZE,0);
	crc = (byte *)&m_sendBuffer[CRCLOC];
	memcpy(crc,&messageCRC,CRCSIZE);

	return;
}

