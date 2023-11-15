#include "stdafx.h"
#include "LongPollUMessage.h"

CLongPollUMessage::CLongPollUMessage(byte gameAddress)
: CSASPollMessage(gameAddress)
{
	// allocate buffers
	SetSendBuffer((BYTE *)new byte[XU_POLLSIZE],XU_POLLSIZE);
	SetResponseBuffer((BYTE *)new byte[XU_RESPONSESIZE],XU_RESPONSESIZE);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	memset(GetResponseBuffer(),0,GetResponseBufferSize());
	m_incRetryOnNoResponse = false;

}

CLongPollUMessage::~CLongPollUMessage(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPollUMessage::GetPollValue()
{
	return m_longPoll;
}

bool CLongPollUMessage::SwapPollValue()
{
	return false;
}

void CLongPollUMessage::ResponseReceived()
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

		m_transferAmount = Bcd2Dword(response, 5);
	}

	CSASPollMessage::ResponseReceived();

	return;

}

void CLongPollUMessage::Fill()
{
	// fill long poll command data
	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = m_longPoll;
	m_sendBuffer[2] = m_transNumber;
	m_sendBuffer[3] = m_ack;

	byte *crcLocation;
	ushort messageCRC = CRC(GetSendBuffer(),GetSendBufferSize() - CRCSIZE,0);
	crcLocation = (byte *)&m_sendBuffer[4];
	memcpy(crcLocation,&messageCRC,CRCSIZE);

	return;
}

