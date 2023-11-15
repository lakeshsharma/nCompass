#include "PollerGamesListMessage.h"
#include "Utilities.h"
#include "Logging/LogString.h"


CParseGameList::CParseGameList() { }

std::vector<CGame*> CParseGameList::GetGamesList()
{
	return m_gameList;
}

std::set<WORD> CParseGameList::GetDenoms()
{
	return m_denoms;
}

CParseGameList::~CParseGameList()
{
	ClearGameList();
}

DWORD CParseGameList::GetNumberOfEnabledGames() const
{
	return m_enabledGames.size();
}

bool CParseGameList::IsGameNumberEnabled(DWORD gameNumberRight) const
{
	return (std::find_if(m_enabledGames.begin(), m_enabledGames.end(), [&gameNumberRight](const DWORD value) {return value == gameNumberRight; }) != m_enabledGames.end());
}

void CParseGameList::ClearGameList()
{
	LogString(GAMES, "%s %d", __FUNCTION__, __LINE__);
	for_each(m_gameList.begin(), m_gameList.end(), [](CGame *game) { delete game; });
	m_gameList.clear();
	m_GameDenoms.clear();
	m_denoms.clear();
}

std::set<WORD> CParseGameList::GetDenomsForGame(DWORD gameId)
{
	std::set<WORD> tempV;
	std::map<DWORD, set<WORD>>::iterator itr = m_GameDenoms.find(gameId);

	if(itr != m_GameDenoms.end())
	{
		tempV = itr->second;
	}

	return tempV;
}

std::map<DWORD, std::set<WORD> > CParseGameList::GetGameDenoms()
{
	return m_GameDenoms;
}

void CParseGameList::ParseDenomGames(CGames *denomGames)
{
	if (denomGames != nullptr)
	{
		for (auto game : denomGames->GetGames())
		{
			std::map<DWORD, set<WORD>>::iterator it = m_GameDenoms.find(game->GetGameNumber());
			if (it != m_GameDenoms.end())
			{
				it->second.clear(); // this is necessary as we get SAS accounting denom previously for enabled games with all list
			}
		}
	}

	if (denomGames != nullptr)
	{
		for(auto game : denomGames->GetGames())
		{
			if (game != nullptr) {
				std::map<DWORD, set<WORD>>::iterator itr = m_GameDenoms.find(game->GetGameNumber());
				if(itr == m_GameDenoms.end())
				{
					//this happens for 1 time 
					std::set<WORD> denomList;
					denomList.insert(game->GetDenomMultiplier());
					m_GameDenoms.insert(std::make_pair(game->GetGameNumber(), denomList));
				}
				else
				{
					itr->second.insert(game->GetDenomMultiplier());
				}
				m_denoms.insert(game->GetDenomMultiplier());
			}
		}
	}
}

void CParseGameList::ParseEnabledGames(std::vector<DWORD>* enabledGames)
{
	m_enabledGames = *enabledGames;
	LogString(GAMES, "There are %u games enabled.", m_enabledGames.size());
	for (auto gameNumber : m_enabledGames)
	{
		LogString(GAMES, "Game number %u is enabled", gameNumber);
	}
}

void CParseGameList::ParseGamesRange(CGames *allGames, WORD startIndex, WORD endIndex)
{
	LogString(GAMES, "%s %d: startIndex %u, endIndex %u", __FUNCTION__, __LINE__, startIndex, endIndex);

	if (allGames != nullptr)
	{
		vector <CGame*> games = allGames->GetGames();
		auto range_start = std::next(games.begin(), startIndex);  //we could also use last game number. But time complexity is same for both and it avoids maintaining extra variable
		auto range_end = std::next(games.begin(), endIndex);

		for (auto it = range_start; it != range_end && it != games.end(); it++)
		{
			CGame* game = *it;
			if (game != nullptr)
			{
				LogString(GAMES, "%s %d: game->GetGameNumber(): %u MfgID: %s", __FUNCTION__, __LINE__, game->GetGameNumber(), game->GetMfgID());

				WORD gameNumber = game->GetGameNumber();
				auto found = std::find_if(m_gameList.begin(), m_gameList.end(), [&gameNumber](const CGame* gm) {return gm->GetGameNumber() == gameNumber; });

				if (found == m_gameList.end())
				{
					m_gameList.push_back(new CGame(*game));
				}
				else
				{
					LogString(GAMES, "%s %d: Updating blindly old game for GameNumber: %u", __FUNCTION__, __LINE__, game->GetGameNumber());
					auto idx = std::distance(m_gameList.begin(), found);
					*m_gameList[idx] = *game;
				}

				m_denoms.insert(game->GetDenomMultiplier());

				std::map<DWORD, set<WORD>>::iterator itr = m_GameDenoms.find(game->GetGameNumber());
				if(itr == m_GameDenoms.end())
				{
					//this loop is for disabled games only
					//this happens for 1 time 
					std::set<WORD> denomList;
					denomList.insert(game->GetDenomMultiplier());
					m_GameDenoms[game->GetGameNumber()] = denomList;

				}
				//will not fill m_denoms as this function meant to be for disabled games
			}
		}
	}
}

void CParseGameList::GetGamesToSend(std::vector<CGame*>& gamesToSend, WORD gameListChunkoffset, WORD gameListChunkSize)
{
	WORD currentSubameIndex = 0; // counter to increment till the game counts which are already sent, means subgamesCountToSent
	WORD subgameCountsToSkip = gameListChunkoffset;
	WORD subgamesCountToSent = subgameCountsToSkip + gameListChunkSize;
	for (auto game : m_gameList)
	{
		WORD gameNumber = game->GetGameNumber();
		std::map<DWORD, std::set<WORD>>::iterator itr = m_GameDenoms.find(gameNumber);
		if (itr != m_GameDenoms.end())
		{
			if ((currentSubameIndex >= subgameCountsToSkip) && (currentSubameIndex < subgamesCountToSent))
			{
				if (IsGameNumberEnabled(game->GetGameNumber()))
				{
					game->Enable();
				}
				LogString(GAMES, "%s %d: GameNumber: %u enabled=%d", __FUNCTION__, __LINE__, game->GetGameNumber(), game->IsEnabled());
				gamesToSend.push_back(new CGame(*game));
			}

			if (currentSubameIndex >= subgamesCountToSent)
				break;	 // Avoid unnecessary looping if required number of games are retrieved

			currentSubameIndex++;
		}
	}
}