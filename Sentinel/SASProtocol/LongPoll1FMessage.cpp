#include "stdafx.h"
#include "LongPoll1FMessage.h"

const int GAMEIDLENGTH = 2;
const int ADDID = 3;
const int PAYTABLELENGTH = 6;
const int BASEPERCENTLENGTH = 4;

CLongPoll1FMessage::CLongPoll1FMessage(byte gameAddress)
: CSASPollMessage(gameAddress)
{
	// allocate buffers
	SetSendBuffer((BYTE *)new byte[X1F_POLLSIZE],X1F_POLLSIZE);
	SetResponseBuffer((BYTE *)new byte[X1F_RESPONSESIZE],X1F_RESPONSESIZE);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	memset(GetResponseBuffer(),0,GetResponseBufferSize());
}

CLongPoll1FMessage::~CLongPoll1FMessage(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll1FMessage::GetPollValue()
{
	return 0x1F;
}

bool CLongPoll1FMessage::SwapPollValue()
{
	// what do we do here?
	return false;
}

void CLongPoll1FMessage::ResponseReceived()
{
	if (m_nReturnValue == SASPOLL_SUCCESS)
	{
		BYTE *response = GetResponseBuffer();
		strncpy(m_gameID,(const char *)&response[2],GAMEIDLENGTH);
		m_gameID[GAMEIDLENGTH] = 0;
		strncpy(m_additionalID,(const char *)&response[4],ADDID);
		m_additionalID[ADDID] = 0;
		m_denomination = response[7];
		m_maxBet = response[8];
		m_progressiveGroup = response[9];
		memcpy(m_gameOptions,&response[10],2);
		strncpy(m_payTable,(const char *)&response[12],PAYTABLELENGTH);
		m_payTable[PAYTABLELENGTH] = 0;
		strncpy(m_basePercent,(const char *)&response[18],BASEPERCENTLENGTH);
		m_basePercent[BASEPERCENTLENGTH] = 0;
	}
	CSASPollMessage::ResponseReceived();

	return;

}

void CLongPoll1FMessage::Fill()
{
	// fill long poll command data
	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0x1F;

	return;
}
