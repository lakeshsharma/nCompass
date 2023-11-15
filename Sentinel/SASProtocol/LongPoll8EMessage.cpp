#include "stdafx.h"
#include "LongPoll8EMessage.h"

const BYTE CLongPoll8EMessage::COMMAND = 0x8E;
const UINT CLongPoll8EMessage::SENDBUFFERLENGTH = 2;
const UINT CLongPoll8EMessage::RECBUFFERLENGTH = 10;
const UINT CLongPoll8EMessage::HAND_TYPE_LOCATION = 2;
const UINT CLongPoll8EMessage::CARDLOC = 3;

CLongPoll8EMessage::CLongPoll8EMessage(byte gameAddress, PokerHandType pokerHandType)
	: CSASPollMessage(gameAddress),
	m_pokerHandType(pokerHandType)
{
	// allocate buffers
	SetSendBuffer((byte *)new byte[SENDBUFFERLENGTH], SENDBUFFERLENGTH);
	SetResponseBuffer((byte*)new byte[RECBUFFERLENGTH], RECBUFFERLENGTH);
	memset(GetSendBuffer(), 0, GetSendBufferSize());
	memset(GetResponseBuffer(), 0, GetResponseBufferSize());
}


CLongPoll8EMessage::~CLongPoll8EMessage()
{
	CSASPollMessage::WaitForSASProcessingDone();
}

void CLongPoll8EMessage::ResponseReceived()
{
	if (m_nReturnValue == SASPOLL_SUCCESS && m_responseBuffer[LPCOMMANDLOC] == COMMAND &&
		m_responseBuffer[HAND_TYPE_LOCATION] == m_pokerHandType)
	{
		m_pokerHand.Set(&m_responseBuffer[CARDLOC]);
	}
	CSASPollMessage::ResponseReceived();
}

void CLongPoll8EMessage::Fill()
{
	// fill long poll command data
	m_sendBuffer[GAMEADDRLOC] = m_gameAddress;
	m_sendBuffer[LPCOMMANDLOC] = COMMAND;
}

BYTE CLongPoll8EMessage::GetPollValue()
{
	return COMMAND;
}

PokerHand CLongPoll8EMessage::GetPokerHand()
{
	return m_pokerHand;
}
