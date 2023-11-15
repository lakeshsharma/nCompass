#include "stdafx.h"
#include "PollerProcessUJPMessage.h"


CPollerProcessUJPMessage::CPollerProcessUJPMessage(byte *response) :
m_jackpotID(0),
m_type(UnattendedJackpotType::UJPTYPE_CANCEL_INSUFFICIENT_INFO),
m_cardID(),
m_amount(0)
{
	m_lpBuffer = NULL;

	if (response != nullptr)
	{
		size_t offset = 0;
		memcpy(&m_jackpotID, response + offset, sizeof(m_jackpotID));
		offset += sizeof(m_jackpotID);

		memcpy(&m_type, response + offset, sizeof(byte));
		offset += sizeof(byte);

		memcpy(m_cardID, response + offset, CARD_ID_LEN);
		offset += CARD_ID_LEN;

		memcpy(&m_amount, response + offset, sizeof(m_amount));
	}
}


CPollerProcessUJPMessage::~CPollerProcessUJPMessage()
{
}

int CPollerProcessUJPMessage::GetJackpotID()
{
	return m_jackpotID;
}

UnattendedJackpotType::Type CPollerProcessUJPMessage::GetType()
{
	return m_type;
}

BYTE *CPollerProcessUJPMessage::GetCardID()
{
	return m_cardID;
}

__int64 CPollerProcessUJPMessage::GetAmount()
{
	return m_amount;
}
