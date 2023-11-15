#pragma once
#include "PollerMessage.h"
class CPollerAuthorizePINResponseMessage :
	public CPollerMessage
{
public:
	CPollerAuthorizePINResponseMessage(const byte *response);
	~CPollerAuthorizePINResponseMessage();

	/// <summary>
	/// Gets the card identifier.
	/// </summary>
	/// <returns>Card ID</returns>
	byte *GetCardID();

	/// <summary>
	/// Gets the card identifier.
	/// </summary>
	/// <returns>PIN authorization result</returns>
	byte GetAuthorizeResult();

private:
	byte m_cardID[CARD_ID_LEN];
	byte m_result;
};

