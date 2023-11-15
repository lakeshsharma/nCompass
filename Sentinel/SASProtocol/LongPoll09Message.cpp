#include "stdafx.h"
#include "LongPoll09Message.h"

CLongPoll09Message::CLongPoll09Message(byte gameAddress)
: CSASPollMessage(gameAddress)
{
	// allocate buffers
	SetSendBuffer((BYTE *)new byte[10],10);
	SetResponseBuffer((BYTE *)new byte[1],1);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	memset(GetResponseBuffer(),0,GetResponseBufferSize());
}

CLongPoll09Message::~CLongPoll09Message(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll09Message::GetPollValue()
{
	return 0x09;
}

bool CLongPoll09Message::SwapPollValue()
{
	// what do we do here?
	return false;
}

void CLongPoll09Message::ResponseReceived()
{

	CSASPollMessage::ResponseReceived();

	return;

}

void CLongPoll09Message::Fill(byte enable)
{
	// fill long poll command data
	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0xB0;
	m_sendBuffer[2] = 5;
	m_sendBuffer[3] = 4;
	m_sendBuffer[4] = 0x09;
	Dword2Bcd(m_gameNumber,&m_sendBuffer[5],2);
	m_sendBuffer[7] = enable;

	byte *crcLocation;
	ushort messageCRC = CRC(GetSendBuffer(),GetSendBufferSize() - CRCSIZE,0);
	crcLocation = (byte *)&m_sendBuffer[8];
	memcpy(crcLocation,&messageCRC,CRCSIZE);

	return;
}

void CLongPoll09Message::Fill()
{
}

void CLongPoll09Message::SetGameNumber(int gameNumber)
{
	m_gameNumber = gameNumber;

	return;
}
