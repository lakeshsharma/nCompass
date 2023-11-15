#include "stdafx.h"

#include "LongPoll0EMessage.h"
#include "../Sentinel.h"

CLongPoll0EMessage::CLongPoll0EMessage(byte gameAddress)
: CSASPollMessage(gameAddress)
{
	SetSendBuffer((BYTE *) new byte[X0E_POLLSIZE],X0E_POLLSIZE);
	SetResponseBuffer((BYTE *)new byte[X0E_RESPONSESIZE],X0E_RESPONSESIZE);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	memset(GetResponseBuffer(),0,GetResponseBufferSize());
}

CLongPoll0EMessage::~CLongPoll0EMessage(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll0EMessage::GetPollValue()
{
	return 0x0E;
}

bool CLongPoll0EMessage::SwapPollValue()
{
	return false;
}

void CLongPoll0EMessage::ResponseReceived()
{

	CSASPollMessage::ResponseReceived();
}

void CLongPoll0EMessage::Fill()
{
	// fill long poll command data
	m_sendBuffer[GAMEADDRLOC] = m_gameAddress;
	m_sendBuffer[LPCOMMANDLOC] = 0x0E;
	m_sendBuffer[2] = m_bEnableRealTimerReporting ? true : false;
	
	ushort commandCRC = CRC(m_sendBuffer, X0E_POLLSIZE - CRCSIZE,0);

	byte *crc = (byte *)&m_sendBuffer[X0E_POLLSIZE - CRCSIZE];
	memcpy(crc,&commandCRC,CRCSIZE);

}
