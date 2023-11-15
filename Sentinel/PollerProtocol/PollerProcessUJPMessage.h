#pragma once
#include "PollerMessage.h"
#include "Jackpot.h"

class CPollerProcessUJPMessage :
	public CPollerMessage
{
public:
	CPollerProcessUJPMessage(byte *response);
	~CPollerProcessUJPMessage();

	/// <summary>
	/// Gets the jackpot identifier.
	/// </summary>
	/// <returns>jackpot ID</returns>
	int GetJackpotID();

	/// <summary>
	/// Gets the type.
	/// </summary>
	/// <returns>Jackpot type</returns>
	UnattendedJackpotType::Type GetType();

	/// <summary>
	/// Gets the card identifier.
	/// </summary>
	/// <returns>Card ID</returns>
	BYTE *GetCardID();

	/// <summary>
	/// Gets the jackpot amount.
	/// </summary>
	/// <returns>Jackpot amount</returns>
	__int64 GetAmount();

private:
	int m_jackpotID;
	UnattendedJackpotType::Type m_type;
	BYTE m_cardID[CARD_ID_LEN];
	__int64 m_amount;
	
};

