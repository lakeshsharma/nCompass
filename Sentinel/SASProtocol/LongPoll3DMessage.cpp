#include "stdafx.h"
#include "LongPoll3DMessage.h"
#include "../Ticket.h"

CLongPoll3DMessage::CLongPoll3DMessage(byte gameAddress)
: CSASPollMessage(gameAddress)
{
	// allocate buffers
	SetSendBuffer((byte *)new byte[2],2);
	SetResponseBuffer((byte*)new byte[13],13);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	memset(GetResponseBuffer(),0,GetResponseBufferSize());
}

CLongPoll3DMessage::~CLongPoll3DMessage(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll3DMessage::GetPollValue()
{
	return 0x3D;
}

bool CLongPoll3DMessage::SwapPollValue()
{
	return false;
}

void CLongPoll3DMessage::ResponseReceived()
{
	byte *response = GetResponseBuffer();

	if (m_nReturnValue == SASPOLL_SUCCESS)
	{
		m_responseTicket = new CTicket();
		m_responseTicket->SetValidationNumber(response,4);
		response += 4;

		m_responseTicket->SetAmount(Bcd2Dword(response,5));
		m_responseTicket->SetTicketNumber(0);
	}

 	CSASPollMessage::ResponseReceived();
}

void CLongPoll3DMessage::Fill()
{
	// fill long poll command data
	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0x3D;

	return;
}

