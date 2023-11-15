#include "stdafx.h"
#include "LongPoll7FMessage.h"

// Sentinel includes.
#include "Utilities.h"

CLongPoll7FMessage::CLongPoll7FMessage(byte gameAddress)
: CSASPollMessage(gameAddress)
{
	SetSendBuffer((BYTE *) new byte[X7F_POLLSIZE],X7F_POLLSIZE);
	SetResponseBuffer((BYTE *)new byte[X7F_RESPONSESIZE],X7F_RESPONSESIZE);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	memset(GetResponseBuffer(),0,GetResponseBufferSize());
}

CLongPoll7FMessage::~CLongPoll7FMessage(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll7FMessage::GetPollValue()
{
	return 0x7F;
}

bool CLongPoll7FMessage::SwapPollValue()
{
	return false;
}

void CLongPoll7FMessage::ResponseReceived()
{


	CSASPollMessage::ResponseReceived();
}

void CLongPoll7FMessage::Fill()
{
	SYSTEMTIME systime;
	CUtilities::GetLocalSystemTime(&systime);
	DWORD dateNumber = systime.wMonth * 1000000 + systime.wDay * 10000 + systime.wYear;
	DWORD timeNumber = systime.wHour * 10000 + systime.wMinute * 100 + systime.wSecond;

	// fill long poll command data
	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0x7F;
	Dword2Bcd( dateNumber, &m_sendBuffer[2], 4);
	Dword2Bcd( timeNumber, &m_sendBuffer[6], 3);
	
	ushort commandCRC = CRC(m_sendBuffer, X7F_POLLSIZE - CRCSIZE,0);
	byte *crc = (byte *)&m_sendBuffer[X7F_POLLSIZE - CRCSIZE];
	memcpy(crc,&commandCRC,CRCSIZE);
}
