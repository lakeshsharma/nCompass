#include "stdafx.h"
#include "PollerAuthorizePINResponseMessage.h"


CPollerAuthorizePINResponseMessage::CPollerAuthorizePINResponseMessage(const byte *response) :
	m_result(0)
{
	m_lpBuffer = NULL;

	if (response != nullptr)
	{

		size_t offset = 0;
		memcpy(m_cardID, response + offset, CARD_ID_LEN);
		offset += CARD_ID_LEN;

		memcpy(&m_result, response + offset, sizeof(byte));
	}
	else
	{
		memset(m_cardID, 0, CARD_ID_LEN);
	}

}


CPollerAuthorizePINResponseMessage::~CPollerAuthorizePINResponseMessage()
{
}

byte *CPollerAuthorizePINResponseMessage::GetCardID()
{
	return m_cardID;
}

byte CPollerAuthorizePINResponseMessage::GetAuthorizeResult()
{
	return m_result;
}
