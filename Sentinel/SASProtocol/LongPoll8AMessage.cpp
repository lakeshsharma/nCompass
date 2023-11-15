#include "stdafx.h"
#include "LongPoll8AMessage.h"

CLongPoll8AMessage::CLongPoll8AMessage(byte gameAddress)
: CSASPollMessage(gameAddress)
{
	SetSendBuffer((BYTE *) new byte[X8A_POLLSIZE], X8A_POLLSIZE);
	SetResponseBuffer((BYTE *)new byte[X8A_RESPONSESIZE], X8A_RESPONSESIZE);
	memset(GetSendBuffer(), 0, GetSendBufferSize());
	memset(GetResponseBuffer(), 0, GetResponseBufferSize());
	m_legacyBonusAmount = 0;
	m_legacyBonusTaxStatus = 0;
}

CLongPoll8AMessage::~CLongPoll8AMessage(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll8AMessage::GetPollValue()
{
	return 0x8A;
}

bool CLongPoll8AMessage::SwapPollValue()
{
	return false;
}

void CLongPoll8AMessage::ResponseReceived()
{
	CSASPollMessage::ResponseReceived();
}

void CLongPoll8AMessage::Fill()
{
	// fill long poll command data
	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0x8A;
	Dword2Bcd(m_legacyBonusAmount, &m_sendBuffer[2], 4);
	m_sendBuffer[6] = m_legacyBonusTaxStatus; // 0=Deductible, 1=Non-deductible, 2=Wager match

	ushort commandCRC = CRC(m_sendBuffer, X8A_POLLSIZE - CRCSIZE, 0);
	byte *crc = (byte *)&m_sendBuffer[X8A_POLLSIZE - CRCSIZE];
	memcpy(crc, &commandCRC, CRCSIZE);
}

