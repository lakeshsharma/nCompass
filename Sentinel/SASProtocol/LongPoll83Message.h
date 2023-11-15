#pragma once
#include "SASPollMessage.h"


/// <summary>
/// 0x83 is the Send cumulative progressive wins poll to SAS.
/// This is normally used for "Accounting Only" links.
/// </summary>
class CLongPoll83Message : public CSASPollMessage
{
public:
	static const DWORD MAXIMUM_CUMULATIVE_PROGRESSIVE_WINS_AMOUNT_IN_CREDITS = 99999999; // 8-digit maximum, in credits.

	CLongPoll83Message(byte gameAddress);
	
	/// <summary>
	/// Gets the poll value.
	/// </summary>
	/// <returns>the poll value</returns>
	virtual BYTE GetPollValue();
	
	/// <summary>
	/// Swaps the poll value.
	/// </summary>
	/// <returns>true if the value is swapped</returns>
	virtual bool SwapPollValue();
	
	/// <summary>
	/// Sets the game number.
	/// </summary>
	/// <param name="gameNumber">The game number.</param>
	void SetGameNumber(int gameNumber);
	
	/// <summary>
	/// Gets the amount.
	/// </summary>
	/// <returns>the amount</returns>
	__int64 GetAmount();
	
	/// <summary>
	/// Responses the received.
	/// </summary>
	void ResponseReceived(void);
	
	/// <summary>
	/// Fills this instance.
	/// </summary>
	void Fill();

private:
	__int64 m_amount;
	int m_gameNumber;
};
