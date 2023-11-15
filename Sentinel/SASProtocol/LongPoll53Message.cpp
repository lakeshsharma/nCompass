#include "stdafx.h"
#include "LongPoll53Message.h"

CLongPoll53Message::CLongPoll53Message(byte gameAddress)
: CSASPollMessage(gameAddress)
{
	// allocate buffers
	SetSendBuffer((BYTE *)new byte[X53_POLLSIZE],X53_POLLSIZE);
	SetResponseBuffer((BYTE *)new byte[X53_RESPONSESIZE],X53_RESPONSESIZE);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	memset(GetResponseBuffer(),0,GetResponseBufferSize());

	memset( m_gameID, 0, sizeof(m_gameID) );
	memset( m_additionalID, 0, sizeof(m_additionalID) );
	m_denomination = 1;
	m_maxBet = 1;
	m_progressiveGroup = 0;
	memset( m_gameOptions, 0, sizeof( m_gameOptions ) );
	memset( m_payTable, 0, sizeof(m_payTable) );
	memset( m_basePercent, 0, sizeof(m_basePercent) );
}

CLongPoll53Message::~CLongPoll53Message(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll53Message::GetPollValue()
{
	return 0x53;
}

bool CLongPoll53Message::SwapPollValue()
{
	// what do we do here?
	return false;
}

void CLongPoll53Message::ResponseReceived()
{
	if (m_nReturnValue == SASPOLL_SUCCESS)
	{
		BYTE *response = GetResponseBuffer();

		memcpy(m_gameID,(const char *)&response[4],2);
		memcpy(m_additionalID,(const char *)&response[6],3);
		m_denomination = response[9];
		m_maxBet = response[10];
		m_progressiveGroup = response[11];
		memcpy(m_gameOptions,&response[12],2);
		memcpy(m_payTable,(const char *)&response[14],6);
		memcpy(m_basePercent,(const char *)&response[20],4);
	}
	CSASPollMessage::ResponseReceived();

	return;

}

void CLongPoll53Message::Fill()
{
	// fill long poll command data
	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0x53;
	Dword2Bcd(m_gameNumber,&m_sendBuffer[2],2);

	byte *crcLocation;
	ushort messageCRC = CRC(GetSendBuffer(),GetSendBufferSize() - CRCSIZE,0);
	crcLocation = (byte *)&m_sendBuffer[4];
	memcpy(crcLocation,&messageCRC,CRCSIZE);

	return;
}

void CLongPoll53Message::SetGameNumber(int gameNumber)
{
	m_gameNumber = gameNumber;

	return;
}
