#include "stdafx.h"
#include "LongPoll2EMessage.h"

CLongPoll2EMessage::CLongPoll2EMessage(byte gameAddress)
: CSASPollMessage(gameAddress)
{
}

CLongPoll2EMessage::~CLongPoll2EMessage(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll2EMessage::GetPollValue()
{
	return 0x2E;
}

bool CLongPoll2EMessage::SwapPollValue()
{
	// what do we do here?
	return false;
}

void CLongPoll2EMessage::ResponseReceived()
{


	CSASPollMessage::ResponseReceived();

	return;

}

void CLongPoll2EMessage::Fill()
{
	// allocate buffers
	byte *crc;

	SetSendBuffer((BYTE *) new byte[X2E_POLLSIZE],X2E_POLLSIZE);
	SetResponseBuffer((BYTE *)new byte[X2E_RESPONSESIZE],X2E_RESPONSESIZE);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	memset(GetResponseBuffer(),0,GetResponseBufferSize());

	// fill long poll command data
	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0x2E;
	memcpy(&m_sendBuffer[2],&m_gameDelay,2);
	
	ushort commandCRC = CRC(m_sendBuffer, X2E_POLLSIZE - CRCSIZE,0);
	crc = (byte *)&m_sendBuffer[X2E_POLLSIZE - CRCSIZE];
	memcpy(crc,&commandCRC,CRCSIZE);


	return;
}

void CLongPoll2EMessage::SetGameDelay(short gameDelay)
{
	m_gameDelay = gameDelay;
}
