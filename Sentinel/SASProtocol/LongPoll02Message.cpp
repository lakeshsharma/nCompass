#include "stdafx.h"
#include "LongPoll02Message.h"

CLongPoll02Message::CLongPoll02Message(byte gameAddress)
: CSASPollMessage(gameAddress)
{
	// allocate buffers
	SetSendBuffer((byte *)new byte[LP02SENDSIZE],LP02SENDSIZE);
	SetResponseBuffer((byte*)new byte[LP02RECVSIZE],LP02RECVSIZE);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	memset(GetResponseBuffer(),0,GetResponseBufferSize());
}

CLongPoll02Message::~CLongPoll02Message(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll02Message::GetPollValue()
{
	return 0x02;
}

bool CLongPoll02Message::SwapPollValue()
{
	// what do we do here?
	return 0;
}

void CLongPoll02Message::ResponseReceived()
{


	CSASPollMessage::ResponseReceived();

	return;

}

void CLongPoll02Message::Fill()
{
	// fill long poll command data
	m_sendBuffer[GAMEADDRLOC] = m_gameAddress;
	m_sendBuffer[LPCOMMANDLOC] = 0x02;

	byte *crc;
	ushort messageCRC = CRC(GetSendBuffer(),GetSendBufferSize() - CRCSIZE,0);
	crc = (byte *)&m_sendBuffer[CRCLOC];
	memcpy(crc,&messageCRC,CRCSIZE);

	return;
}

