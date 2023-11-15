#pragma once
#include "../Games.h"

// Declare friend class(es) for unit test access.
namespace SentinelUnitTests { class SASGameListTests; }

class CSASGameList
{
public:
	static const int DENOM_UNKNOWN = 0;

	CSASGameList(void);
	~CSASGameList(void);

	/// <summary>
	/// Clears this instance.
	/// </summary>
	void Clear( void );

	void SetCount( WORD gameCount );
	WORD GetCount( void );
	void SetGameEnabled( WORD gameNumber );

	/// <summary>
	/// Sets the current game number.
	/// </summary>
	/// <param name="isValid">IN - true if gameNumber was obtained from a successful poll, false otherwise.</param>
	/// <param name="gameSelected">IN - true if this update is in response to a Game Selected exception.</param>
	/// <param name="gameNumber">IN - The current game number.</param>
	void SetCurrentGame(bool isValid, bool gameSelected, WORD gameNumber );

	/// <summary>
	/// Gets the current game.
	/// </summary>
	/// <returns>The current game.</returns>
	WORD GetCurrentGame( void ) const;

	bool IsGameEnabled( WORD gameNumber );


	// This will get the next game in the following order.
	// current game
	// 1st enabled game
	// current game
	// 2nd enabled game
	// .
	// .
	// .
	// current game
	// last enabled game
	// 1st non enabled game
	// current game
	// 1st enabled game
	// ...
	/// <summary>
	/// Gets the next game in list of games.
	/// </summary>
	/// <param name="needToSendInfo">The need to send information.</param>
	/// <returns>next game number</returns>
	WORD GetNextGame(bool &needToSendInfo);

	/// <summary>
	/// Determines whether this is the first pass through game list.
	/// </summary>
	/// <returns></returns>
	bool IsFirstPass(void)
	{
		return m_bFirstPass;
	}

	/// <summary>
	/// Determines whether all enabled games have been returned.
	/// </summary>
	/// <returns>Is last last enabled game been returned on a first pass.</returns>
	bool IsLastEnabledGameOnFirstPass();

	/// <summary>
	/// Determines whether all enabled games have been returned.
	/// </summary>
	/// <returns>Is last last enabled game been returned on a first pass.</returns>
	bool IsEnabledGameCountsReached();
	
	void SetFirstPass( void );

	/// <summary>
	/// Clears game list error.
	/// </summary>
	void ClearError();

	/// <summary>
	/// Sets game list erorr condition.
	/// </summary>
	void SetError();

	/// <summary>
	/// Determines whether this instance is error.
	/// </summary>
	/// <returns>Was there an error retrieving game list or subgames</returns>
	bool IsError();

	/// <summary>
	/// Sets the enabled denominations count.
	/// </summary>
	/// <param name="denomCount">IN - The enabled denominations count.</param>
	void SetEnabledDenomCount(byte denomCount);

	/// <summary>
	/// Gets the enabled denominations count.
	/// </summary>
	/// <returns>The enabled denominations count.</returns>
	byte GetEnabledDenomCount() const;

	/// <summary>
	/// Gets the denomination from a SAS denomination code.
	/// </summary>
	/// <param name="denomCode">IN - The SAS denomination code.</param>
	/// <returns>The denomination from the SAS denomination code.</returns>
	/// <remarks>
	/// Returns DENOM_UNKNOWN if the SAS denomination code is out of range.
	/// </remarks>
	static int GetDenomFromDenomCode(byte denomCode);

	/// <summary>
	/// Sets the current player denomination from a SAS denomination code.
	/// </summary>
	/// <param name="isValid">IN - true if denomCode was obtained from a successful poll, false otherwise.</param>
	/// <param name="gameSelected">IN - true if this update is in response to a Game Selected exception.</param>
	/// <param name="denomCode">IN - The SAS denomination code.</param>
	/// <remarks>
	/// The previous denomination is retained if the new denomination == DENOM_UNKNOWN and GetDenomCount() == 1.
	/// </remarks>
	void SetCurrentDenomCode(bool isValid, bool gameSelected, byte denomCode);

	/// <summary>
	/// Sets the current player denomination.
	/// </summary>
	/// <param name="isValid">IN - true if currentDenom was obtained from a successful poll, false otherwise.</param>
	/// <param name="gameSelected">IN - true if this update is in response to a Game Selected exception.</param>
	/// <param name="currentDenom">IN - The current player denomination.</param>
	/// <remarks>
	/// The previous denomination is retained if currentDenom == DENOM_UNKNOWN && GetDenomCount() == 1.
	/// </remarks>
	void SetCurrentDenom(bool isValid, bool gameSelected, int currentDenom);

	/// <summary>
	/// Gets the current player denomination.
	/// </summary>
	/// <returns>The current player denomination.</returns>
	int GetCurrentDenom() const;

	/// <summary>
	/// Checks if the current game number should be updated.
	/// </summary>
	/// <param name="gameStarted">IN - true if processing a Game Started exception, false otherwise.</param>
	/// <param name="pollingSpeed">The polling speed.</param>
	/// <returns>true if the current game number and current player denomination should be updated, false otherwise.</returns>
	/// <remarks>
	/// Returns false unless there is more than one enabled game or more than one enabled denomination.
	/// Otherwise:
	///     Returns true if this part of the normal polling cycle (!gameStarted).
	///     Returns true if we are using the fast polling cycle.
	///     Returns true if there is more than one enabled game and the current game number is EGM_GAME_NUMBER.
	/// </remarks>
	bool ShouldUpdateCurrentGame(bool gameStarted, short pollingSpeed) const;

	/// <summary>
	/// Checks if the current player denomination should be updated.
	/// </summary>
	/// <param name="gameStarted">IN - true if processing a Game Started exception, false otherwise.</param>
	/// <param name="pollingSpeed">The polling speed.</param>
	/// <returns>true if the current game number and current player denomination should be updated, false otherwise.</returns>
	/// <remarks>
	/// Returns false unless there is more than one enabled game or more than one enabled denomination.
	/// Otherwise:
	///     Returns true if this part of the normal polling cycle (!gameStarted).
	///     Returns true if we are using the fast polling cycle.
	///     Returns true if there is more than one enabled denomination and the current denomination is DENOM_INVALID.
	/// </remarks>
	bool ShouldUpdateCurrentDenom(bool gameStarted, short pollingSpeed) const;

	void ResetTheValuesToLastGameWeTriedToSend();

	/// <summary>
	/// Adds previously enabled game to the list
	/// </summary>
	/// <param name="gameNumber">IN - Game number to add the list of previously enabled games.</param>
	void AddPreviouslyEnabledGame(DWORD gameNumber);

	/// <summary>
	/// Gets list of enabled games
	/// </summary>
	/// <returns>Vector of enabled games.</returns>
	const std::vector<DWORD>& GetEnabledGames();

	/// <summary>
	/// Sets the list of disabled games
	/// </summary>
	/// <returns>Vector of disabled games.</returns>
	void SetNonEnabledGames(const std::vector<DWORD>& nonEnabledGames);

	/// <summary>
	/// Gets list of disabled games
	/// </summary>
	/// <returns>Vector of disabled games.</returns>
	const std::vector<DWORD>& GetNonEnabledGames();

	/// <summary>
	/// Gets list of previously enabled games with coinin greater than 0
	/// </summary>
	/// <returns>Vector of previously enabled games.</returns>
	const std::vector<DWORD>& GetPreviouslyEnabledGames();

	/// <summary>
	/// Checks if first pass of enabled game list is sent
	/// </summary>
	/// <returns>true if first pass of enabled game list is sent, false otherwise.</returns>
	bool IsFirstPassEnabledGameListSent() const;

	/// <summary>
	/// Sets first pass of enabled game list is sent flag
	/// </summary>
	void SetFirstPassEnabledGameListSentFlag();

	/// <summary>
	/// Clear first pass of enabled game list is sent flag
	/// </summary>
	void ClearFirstPassEnabledGameListSentFlag();

private:

	WORD m_currentGame;
	std::vector<DWORD> m_enabledGames;
	std::vector<DWORD> m_previouslyEnabledGames;
	std::vector<DWORD> m_nonEnabledGames;
	bool m_bFirstPassEnabledGameListSent;
	bool m_bCurrentGameUsedLast;
	int m_enabledGameIndex;
	int m_nonEnabledGameIndex;
	bool m_bFirstPass;
	bool m_bLastEnabledGameReached;
	bool m_bEnabledGameCountsReached;
	bool m_errorCondition;

	int m_revertEnabledGameIndex;
	int m_revertNonEnabledGameIndex;
	WORD m_revertCurrentGame;
	bool m_revertLastEnabledGameReached;
	bool m_reverFirstPass;
	bool m_revertEnabledGameCountsReached;
	bool m_revertCurrentGameUsedLast;

	byte m_enabledDenomCount;
	int m_currentDenom;

	// Grant "friend" access for testing.
	friend class SentinelUnitTests::SASGameListTests;
};

