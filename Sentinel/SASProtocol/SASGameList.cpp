#include "stdafx.h"
#include "SASGameList.h"

#include "SASSerialWrapper.h"
#include "Logging/LogString.h"

CSASGameList::CSASGameList(void)
{
	Clear();
}

CSASGameList::~CSASGameList(void)
{
}

void CSASGameList::SetCount( WORD gameCount )
{
	// Only set the count if it is changed.
	// NOTE: This is called every hour. If we did not check the count before we do this, we would cause a memory leak.
	//  Found this with tracker 18996
	if ( gameCount > GetCount() )
	{
		//int nonEnabledGameIndex = 0;
		int enabledGameIndex = 0;
		for ( int gameIndex = 0; gameIndex <= gameCount; gameIndex++ )
		{
			// Don't add the first game to the list of non-enabled games.
			// This is always the EGM.
			if ( gameIndex > 0 )
			{
				// Iterate through the enabled games to see if it game number is there.
				// Stop in the enabled game list if we have found the game number or we passed where it would have been. 
				while ( enabledGameIndex < (int)m_enabledGames.size() && gameIndex > (int)m_enabledGames[enabledGameIndex] )
				{
					enabledGameIndex++;
				}

				// If the game was not present in the enabled game list, put it into the non-enabled game list.
				if ( enabledGameIndex == (int)m_enabledGames.size() || gameIndex != (int)m_enabledGames[enabledGameIndex] )
				{
					m_nonEnabledGames.push_back(gameIndex);
				}
			}
		}
	}

	m_bFirstPass = true;
}

WORD CSASGameList::GetCount( void )
{
	return m_enabledGames.size() + m_nonEnabledGames.size();
}

void CSASGameList::Clear( void )
{
	m_currentGame = 0;
	m_enabledGames.clear();
	m_nonEnabledGames.clear();
	m_bFirstPassEnabledGameListSent = false;
	m_bCurrentGameUsedLast = false;
	m_enabledGameIndex = 0;
	m_nonEnabledGameIndex = 0;

	m_bFirstPass = true;

	m_enabledDenomCount = 0;
	m_bEnabledGameCountsReached = false;
	m_currentDenom = DENOM_UNKNOWN;
	ClearError();

	m_revertEnabledGameIndex = 0;
	m_revertNonEnabledGameIndex = 0;
	m_revertCurrentGame = 0;
	m_revertLastEnabledGameReached = false;
	m_reverFirstPass = false;
	m_revertEnabledGameCountsReached = false;
	m_revertCurrentGameUsedLast = false;
}

void CSASGameList::SetFirstPass( void )
{
	m_bFirstPass = true;
	m_enabledGameIndex = 0;
	m_nonEnabledGameIndex = 0;
	m_bLastEnabledGameReached = false;
	LogString(GAMES, "Set game list first pass.");
	LogString(GAMES, "Enabled size %d, Nonenabled Size %d", m_enabledGames.size(), m_nonEnabledGames.size());
}

void CSASGameList::SetGameEnabled( WORD gameNumber )
{
	// Find the index in  the enabled list where the game is to be added
	int eIndex;
	for ( eIndex = 0; eIndex < (int)m_enabledGames.size() && gameNumber > m_enabledGames[eIndex]; eIndex++ )
	{
	}

	// Add the game to the end of the enabled list
	if ( eIndex >= (int)m_enabledGames.size() )
	{
		m_enabledGames.push_back( gameNumber );
	}

	// Insert the game at the index of the enabled list
	else if ( gameNumber < m_enabledGames[eIndex] )
	{
		auto it = m_enabledGames.begin();
		m_enabledGames.insert( it + eIndex, gameNumber );
	}

	// Find the index in the not-enabled list
	int nIndex;
	for ( nIndex = 0; nIndex < (int)m_nonEnabledGames.size() && gameNumber > m_nonEnabledGames[nIndex]; nIndex++ )
	{
	}

	// remove from non-enabled game list it if found in non-enabled game list
	if ( nIndex < (int)m_nonEnabledGames.size() && gameNumber == m_nonEnabledGames[nIndex] )
	{
		auto it = m_nonEnabledGames.begin();
		m_nonEnabledGames.erase( it + nIndex );
	}

}

bool CSASGameList::IsGameEnabled( WORD gameNumber )
{
	bool bFound = false;

	// Find the index in  the enabled list where the game is to be added
	for ( int eIndex = 0; eIndex < (int)m_enabledGames.size() && !bFound; eIndex++ )
	{
		if ( m_enabledGames[eIndex] == gameNumber )
			bFound = true;
	}

	return bFound;
}


WORD CSASGameList::GetNextGame(bool &needToSendInfo)
{
	WORD nextGameNumber = m_currentGame;
	needToSendInfo = false;

	m_revertEnabledGameIndex = m_enabledGameIndex;
	m_revertCurrentGame = m_currentGame;
	m_revertLastEnabledGameReached = m_bLastEnabledGameReached;
	m_reverFirstPass = m_bFirstPass;
	m_revertEnabledGameCountsReached = m_bEnabledGameCountsReached;
	m_revertNonEnabledGameIndex = m_nonEnabledGameIndex;
	m_revertCurrentGameUsedLast = m_bCurrentGameUsedLast;

	if (m_enabledGames.size() + m_nonEnabledGames.size() == 0)
	{
		// Single game machine, nextgamenumber always 0
		nextGameNumber = 0;
		if (m_bFirstPass && !m_bLastEnabledGameReached)
		{
			LogString(GAMES, "%s %d nextGameNumber=%u", __FUNCTION__, __LINE__, nextGameNumber);
			m_bLastEnabledGameReached = true;
			m_bFirstPass = false;
		}
	}
	else
	{
		if ( m_bFirstPass )
		{
			if ( m_enabledGameIndex < (int)m_enabledGames.size() )
			{
				nextGameNumber = m_enabledGames[m_enabledGameIndex++];
				needToSendInfo = true;
				if (m_enabledGameIndex == (int)m_enabledGames.size())
				{
					// This is the first pass and last enabled game
					LogString(GAMES, "%s %d nextGameNumber=%u", __FUNCTION__, __LINE__, nextGameNumber);
					m_bLastEnabledGameReached = true;

					// This variable will remain true once all the enabled games are fetched.
					m_bEnabledGameCountsReached = true;
				}
			}
			else if ( m_nonEnabledGameIndex < (int)m_nonEnabledGames.size() )
			{
				nextGameNumber = m_nonEnabledGames[m_nonEnabledGameIndex++];
				needToSendInfo = true;
			}
			else
			{
				m_bFirstPass = false;
				m_enabledGameIndex = 0;
				m_nonEnabledGameIndex = 0;
			}
		}

		if ( !m_bFirstPass )
		{
			// We will get the current game every other time.
			// So if we last got the current game, get from enabled or non-enabled list.
			if ( m_bCurrentGameUsedLast )
			{
				// We will go thru the entire list of enabled games before
				//  we get a single game the non-enabled game list.
				if ( m_enabledGameIndex < (int)m_enabledGames.size() )
				{
					nextGameNumber = m_enabledGames[m_enabledGameIndex++];
				}

				// We reached the end of the enabled game list
				//  so get a game from the non-enabled list.
				else if (m_nonEnabledGames.size() > 0)
				{
					m_enabledGameIndex = 0;	// This will get us to the begining of the enabled list next time.

					if ( m_nonEnabledGameIndex >= (int)m_nonEnabledGames.size() )
					{
						m_nonEnabledGameIndex = 0;
					}
					nextGameNumber = m_nonEnabledGames[m_nonEnabledGameIndex++];
				}
				else 
				{
					m_enabledGameIndex = 0;
					nextGameNumber = m_enabledGames[m_enabledGameIndex++];
				}
				m_bCurrentGameUsedLast = false;	// This will make us get the current game next time.
			}
			else
			{
				m_bCurrentGameUsedLast = true;	// This will make us get a enabled or non-enabled game next time
			}
		}
	}

	return nextGameNumber;
}

bool CSASGameList::IsLastEnabledGameOnFirstPass()
{
	return m_bLastEnabledGameReached;
}


bool CSASGameList::IsEnabledGameCountsReached()
{
	return m_bEnabledGameCountsReached;
}

void CSASGameList::ClearError()
{
	m_errorCondition = false;
}

void CSASGameList::SetError()
{
	m_errorCondition = true;
}

bool CSASGameList::IsError()
{
	return m_errorCondition;
}

void CSASGameList::SetCurrentGame(bool isValid, bool gameSelected, WORD gameNumber)
{
	// If this update is in response to a gameSelected exception then set EGM_GAME_NUMBER if not isValid.
	if (isValid || gameSelected)
	{
		if (!isValid)
		{
			gameNumber = EGM_GAME_NUMBER;
		}

		// It's not useful to set EGM_GAME_NUMBER unless there are multiple enabled games.  Otherwise, set the gameNumber.
		if (gameNumber != EGM_GAME_NUMBER || m_enabledGames.size() > 1)
		{
			if (m_currentGame != gameNumber)
			{
				m_currentGame = gameNumber;
				m_bCurrentGameUsedLast = false;	// Force it to get the current game info next.
			}
		}
	}
}

WORD CSASGameList::GetCurrentGame() const
{
	return m_currentGame;
}

void CSASGameList::SetEnabledDenomCount(byte enabledDenomCount)
{
	m_enabledDenomCount = enabledDenomCount;
}

byte CSASGameList::GetEnabledDenomCount() const
{
	return m_enabledDenomCount;
}

int CSASGameList::GetDenomFromDenomCode(byte denomCode)
{
	int denomination(CSASGameList::DENOM_UNKNOWN);
	if (denomCode > 0 && denomCode < _countof(denomCodeMap))
	{
		denomination = denomCodeMap[denomCode].multiplier;
	}

	return denomination;
}

void CSASGameList::SetCurrentDenomCode(bool isValid, bool gameSelected, byte denomCode)
{
	// Not that an unsupported denomCode obtained from a successful was still obtained from a successful poll.
	SetCurrentDenom(isValid, gameSelected, GetDenomFromDenomCode(denomCode));
}

void CSASGameList::SetCurrentDenom(bool isValid, bool gameSelected, int currentDenom)
{
	// If this update is in response to a gameSelected exception then set DENOM_UNKNOWN if not isValid.
	// Otherwise, don't set the currentDenom unless it is valid.
	if (isValid || gameSelected)
	{
		if (!isValid)
		{
			currentDenom = DENOM_UNKNOWN;
		}

		// It's not useful to DENOM_UNKNOWN unless there are multiple enabled denominations.  Otherwise, set the currentDenom.
		if (currentDenom != DENOM_UNKNOWN || m_enabledDenomCount > 1)
		{
			m_currentDenom = currentDenom;
		}
	}
}

int CSASGameList::GetCurrentDenom() const
{
	return m_currentDenom;
}

bool CSASGameList::ShouldUpdateCurrentGame(bool gameStarted, short pollingSpeed) const
{
	bool returnValue(false);

	// We don't need an extra update unless there is more than one enabled game.
	if (m_enabledGames.size() > 1)
	{
		// Do an extra update if
		//     it is part of the normal polling cycle (!gameStarted),
		//     or we are using the fast polling cycle,
		//     or the game number is invalid for reporting game play activity to One Link.
		// In other words, don't do extra updates when
		//     if is a gameStart and we are using the slow polling cycle and the game number is valid for reporting game play activity to One Link.
		if (!gameStarted ||
			CSASSerialWrapper::MS40_POLLING_SPEED_MILLISECONDS == pollingSpeed ||
			EGM_GAME_NUMBER == m_currentGame)
		{
			returnValue = true;
		}
	}

	return returnValue;
}

bool CSASGameList::ShouldUpdateCurrentDenom(bool gameStarted, short pollingSpeed) const
{
	bool returnValue(false);

	// We don't need an extra update unless there is more than one enabled game.
	if (m_enabledDenomCount > 1)
	{
		// Do an extra update if
		//     it is part of the normal polling cycle (!gameStarted),
		//     or we are using the fast polling cycle,
		//     or the denomination is invalid for reporting game play activity to One Link.
		// In other words, don't do extra updates when
		//     if is a gameStart and we are using the slow polling cycle and the denomination is valid for reporting game play activity to One Link.
		if (!gameStarted ||
			CSASSerialWrapper::MS40_POLLING_SPEED_MILLISECONDS == pollingSpeed ||
			DENOM_UNKNOWN == m_currentDenom)
		{
			returnValue = true;
		}
	}

	return returnValue;
}

void CSASGameList::ResetTheValuesToLastGameWeTriedToSend()
{
	m_enabledGameIndex = m_revertEnabledGameIndex;
	m_currentGame = m_revertCurrentGame;
	m_bLastEnabledGameReached = m_revertLastEnabledGameReached;
	m_bFirstPass = m_reverFirstPass;
	m_bEnabledGameCountsReached = m_revertEnabledGameCountsReached;
	m_nonEnabledGameIndex = m_revertNonEnabledGameIndex;
	m_bCurrentGameUsedLast = m_revertCurrentGameUsedLast;
}

void CSASGameList::AddPreviouslyEnabledGame(DWORD gameNumber)
{
	if (m_previouslyEnabledGames.end() == std::find(m_previouslyEnabledGames.begin(), m_previouslyEnabledGames.end(), gameNumber))
	{
		LogString(GAMES, "AddPreviouslyEnabledGame %u", gameNumber);
		m_previouslyEnabledGames.push_back(gameNumber);
	}
}

const vector<DWORD>& CSASGameList::GetEnabledGames()
{
	return m_enabledGames;
}

void CSASGameList::SetNonEnabledGames(const vector<DWORD>& nonEnabledGames)
{
	m_nonEnabledGames = nonEnabledGames;
}

const vector<DWORD>& CSASGameList::GetNonEnabledGames()
{
	return m_nonEnabledGames;
}

const vector<DWORD>& CSASGameList::GetPreviouslyEnabledGames()
{
	return m_previouslyEnabledGames;
}

bool CSASGameList::IsFirstPassEnabledGameListSent() const
{
	return m_bFirstPassEnabledGameListSent;
}

void CSASGameList::SetFirstPassEnabledGameListSentFlag()
{
	m_bFirstPassEnabledGameListSent = true;
}

void CSASGameList::ClearFirstPassEnabledGameListSentFlag()
{
	m_bFirstPassEnabledGameListSent = false;
}