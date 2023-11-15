#include "stdafx.h"
#include "LongPoll83Message.h"

CLongPoll83Message::CLongPoll83Message(byte gameAddress)
: CSASPollMessage(gameAddress),
	m_amount(0)
{
	// allocate buffers
	SetSendBuffer((BYTE *)new byte[6],6);
	SetResponseBuffer((BYTE *)new byte[10],10);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	memset(GetResponseBuffer(),0,GetResponseBufferSize());
	m_gameNumber = 0;	//initialize to game 0
}

BYTE CLongPoll83Message::GetPollValue()
{
	return 0x83;
}

bool CLongPoll83Message::SwapPollValue()
{
	return 0;
}

void CLongPoll83Message::SetGameNumber(int gameNumber)
{
	m_gameNumber = gameNumber;
	return;
}

__int64  CLongPoll83Message::GetAmount()
{
	return m_amount;
}

void CLongPoll83Message::ResponseReceived()
{
	if (m_nReturnValue == SASPOLL_SUCCESS)
	{
		BYTE *response = GetResponseBuffer();
		response += 2; // address and command byte
		response += 2; // 2 byte game number  
		m_amount = Bcd2Int64(response,4);
    }

	CSASPollMessage::ResponseReceived();
	return;
}

void CLongPoll83Message::Fill()
{
	// fill long poll command data
	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0x83;
	memcpy(&m_sendBuffer[2], &m_gameNumber, 2);

	byte *crcLocation;
	ushort messageCRC = CRC(GetSendBuffer(), GetSendBufferSize() - CRCSIZE, 0);
	crcLocation = (byte *)&m_sendBuffer[4];
	memcpy(crcLocation, &messageCRC, CRCSIZE);
	return;
}
