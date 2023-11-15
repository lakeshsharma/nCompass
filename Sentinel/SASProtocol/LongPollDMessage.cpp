#include "stdafx.h"
#include "LongPollDMessage.h"

CLongPollDMessage::CLongPollDMessage(byte gameAddress)
: CSASPollMessage(gameAddress)
{
	// allocate buffers
	SetSendBuffer((BYTE *)new byte[XD_POLLSIZE],XD_POLLSIZE);
	SetResponseBuffer((BYTE *)new byte[XD_RESPONSESIZE],XD_RESPONSESIZE);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	memset(GetResponseBuffer(),0,GetResponseBufferSize());
	m_incRetryOnNoResponse = false;

}

CLongPollDMessage::~CLongPollDMessage(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPollDMessage::GetPollValue()
{
	return m_longPoll;
}

bool CLongPollDMessage::SwapPollValue()
{
	return false;
}

void CLongPollDMessage::ResponseReceived()
{
	if (m_nReturnValue == SASPOLL_SUCCESS)
	{
		BYTE *response = GetResponseBuffer();
		
		response += 1; // skip past game address byte

		memcpy(&m_longPoll, response, sizeof(byte));
		response += sizeof(byte);

		memcpy(&m_transNumber,response, sizeof(byte));
		response += sizeof(byte);

		memcpy(&m_ack, response, sizeof(byte));
		response += sizeof(byte);

		memcpy(&m_status, response, sizeof(byte));
		response += sizeof(byte);

		m_transferAmount = Bcd2Dword(response, 4);
	}

	CSASPollMessage::ResponseReceived();

	return;

}

void CLongPollDMessage::Fill()
{
	// fill long poll command data
	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = m_longPoll;
	m_sendBuffer[2] = m_transNumber;
	m_sendBuffer[3] = m_ack;
	Dword2Bcd(m_transferAmount,&m_sendBuffer[4],4);

	byte *crcLocation;
	ushort messageCRC = CRC(GetSendBuffer(),GetSendBufferSize() - CRCSIZE,0);
	crcLocation = (byte *)&m_sendBuffer[8];
	memcpy(crcLocation,&messageCRC,CRCSIZE);

	return;
}

