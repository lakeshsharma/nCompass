#pragma once
#include "PollerMessage.h"
#include "../Games.h"
#include "../Game.h"

class CParseGameList
{
public:
	/// <summary>
	/// Object construction.
	/// </summary>
    CParseGameList();

	~CParseGameList();

	DWORD GetNumberOfEnabledGames() const;

	bool IsGameNumberEnabled(DWORD gameNumber) const;

	/// <summary>
	/// Get list of Denoms.
	/// </summary>
	/// <returns>list of Denoms.</returns>
	std::set<WORD> GetDenoms();

	/// <summary>
	/// Get Games List.
	/// </summary>
	/// <returns>Games List</returns>
	std::vector<CGame*> GetGamesList();

	/// <summary>
	/// Get list of denoms for given game ID.
	/// </summary>
    /// <param name="gameId">gameId for which list of denoms need to 
    /// be returned</param> 
	/// <returns>List of denoms for given game ID.</returns>
	std::set<WORD> GetDenomsForGame(DWORD gameId);

	/// <summary>
	/// Parse Denoms from Games List .
	/// </summary>
	/// <param name="denomGames">Games List to parse</param> 
	/// <returns>None.</returns>
    void ParseDenomGames(CGames *denomGames);

	void ParseEnabledGames(std::vector<DWORD>* enabledGames);

	/// <summary>
	/// Get Game Denoms List.
	/// </summary>
	/// <returns>Game Denoms List.</returns>
	std::map<DWORD, std::set<WORD> > GetGameDenoms();

	/// <summary>
	/// Clear Game List once list has been forwarded to poller.
	/// </summary>
	/// <returns>None.</returns>
    void ClearGameList();

	/// <summary>
	/// Parses Games from a given range - startIndex to endIndex. The parsed game list will be sent to poller as
	/// part of message 322 .
	/// </summary>
    /// <param name="startIndex">Index in the 'allGames' list from 
    /// where to start fetching/parsing the game and 
	/// store in class member</param> 
    /// <param name="endIndex">Index in the 'allGames' list till 
    /// where to fetch/parse the game and 
	/// store in class member</param> 
	/// <returns>None.</returns>
	void ParseGamesRange(CGames *allGames, WORD startIndex, WORD endIndex);

	/// <summary>
	/// Gets list of games to send to Floorgix.
	/// </summary>
	/// <param name="gameListChunkoffset">Game list chunk offset</param> 
	/// <param name="gameListChunkSize">Game list chunk size.</param> 
	/// <returns>vector of games to send to FloorLogix.</returns>
	void GetGamesToSend(std::vector<CGame*>& gamesToSend, WORD gameListChunkoffset, WORD gameListChunkSize);

private:
	std::vector<DWORD> m_enabledGames;
	std::map<DWORD, std::set<WORD> > m_GameDenoms;	//<GameNumber, list of supported denoms>
	std::set<WORD> m_denoms;  // set of denoms
    std::vector<CGame*> m_gameList;				//<GameNumber, game*>
};
