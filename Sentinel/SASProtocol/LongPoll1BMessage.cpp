#include "stdafx.h"
#include "LongPoll1BMessage.h"

CLongPoll1BMessage::CLongPoll1BMessage(byte gameAddress)
: CSASPollMessage(gameAddress)
{
	// allocate buffers
	SetSendBuffer((BYTE *)new byte[2],2);
	SetResponseBuffer((BYTE *)new byte[24],24);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	memset(GetResponseBuffer(),0,GetResponseBufferSize());
	m_resetID = StandardResetAvailable;
}

CLongPoll1BMessage::~CLongPoll1BMessage(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll1BMessage::GetPollValue()
{
	return 0x1B;
}

bool CLongPoll1BMessage::SwapPollValue()
{
	// what do we do here?
	return 0;
}

void CLongPoll1BMessage::ResponseReceived()
{
	if (m_nReturnValue == SASPOLL_SUCCESS)
	{
		BYTE *response = GetResponseBuffer();

		response += 2; // address and command byte

		memcpy(&m_progressiveGroup, response, sizeof(byte));
		response += sizeof(byte);

		memcpy(&m_progressiveLevel, response, sizeof(byte));
		response += sizeof(byte);

		m_amount = Bcd2Int64(response,5);
		response += 5;

		m_partialAmountPaid = (WORD)Bcd2Int64(response, 2);
		response += 2;

		memcpy(&m_resetID, response, sizeof(byte));
	}

	CSASPollMessage::ResponseReceived();

	return;

}

void CLongPoll1BMessage::Fill()
{
	// fill long poll command data
	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0x1B;

	return;
}

bool CLongPoll1BMessage::isEmpty()
{
	return m_progressiveGroup == 0 && m_progressiveLevel == 0 && m_amount == 0 && m_partialAmountPaid == 0 && m_resetID == 0;
}

bool CLongPoll1BMessage::KeyToCreditAvailable()
{
	return (m_resetID == CreditMeterResetAvailable);
}

