#pragma once
#include "Meters.h"
#include "PokerHand.h"
#include "Game.h"

class CGameEndData :
	public CInternalMsgContent<INT_MSG_CONTENT_GAME_END>
{
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="CGameEndData"/> class.
	/// </summary>
	/// <param name="currentGame">The current game.</param>
	/// <param name="lastPlayedCashableAmount">IN - The last played cashable amount in credits.</param>
	/// <param name="lastPlayedRestrictedAmount">IN - The last played restricted amount in credits.</param>
	/// <param name="lastWinAmount">IN - The last win amount in credits.</param>
	/// <param name="pokerhand">IN - Poker hand.</param>
	/// <param name="currentGameDenom">IN - Current game denom multiplier.</param>
	/// <remarks>
	/// This instance deletes currentGame.
	/// </remarks>
	CGameEndData(CGame *currentGame,
		long lastPlayedCashableAmount,
		long lastPlayedRestrictedAmount,
		long lastWinAmount,
		PokerHand *pokerHand,
		int currentGameDenom,
		double basePointsEarned,
		bool offline = false);

	/// <summary>
	/// Destructor. Finalizes an instance of the <see cref="CGameEndData"/> class.
	/// </summary>
	~CGameEndData();

	/// <summary>
	/// Gets the current game.
	/// </summary>
	/// <returns>The current game.</returns>
	const CGame *GetCurrentGame() const;

	/// <summary>
	/// Gets the last played cashable amount in credits.
	/// </summary>
	/// <returns>The last played cashable amount in credits.</returns>
	long GetLastPlayedCashableAmount() const;

	/// <summary>
	/// Gets the last played restricted amount in credits.
	/// </summary>
	/// <returns>The last played restricted amount in credits.</returns>
	long GetLastPlayedRestrictedAmount() const;

	/// <summary>
	/// Gets the last win amount in credits.
	/// </summary>
	/// <returns>The last win amount in credits.</returns>
	long GetLastWinAmount() const;

	/// <summary>
	/// Gets the base points earned for the player
	/// </summary>
	/// <returns> the base points earned for the player</returns>
	double GetBasePointsEarned() const;

	/// <summary>
	/// Gets the poker hand.
	/// </summary>
	/// <returns>The poker hand.</returns>
	const PokerHand &GetPokerHand() const;

	/// <summary>
	/// Gets the current denom.
	/// </summary>
	/// <returns></returns>
	int GetCurrentDenom() const;

	/// <summary>
	/// Sets offline status. Used for CoinIn5 message
	/// </summary>
	/// <returns></returns>
	void SetOffline(bool offline);

	/// <summary>
	/// Gets offline status. Used for CoinIn5 message
	/// </summary>
	/// <returns></returns>
	bool GetOffline();

private:
	const CGame *m_currentGame;
	const long m_lastPlayedCashableAmount; // In credits.
	const long m_lastPlayedRestrictedAmount; // In credits.
	const long m_lastWinAmount; // In credits.
	const int m_currentGameDenomMult;
	const double m_basePointsEarned;
	PokerHand m_pokerHand;
	bool m_offline;
};

