#include "stdafx.h"
#include "DisplayManagerPlayerState.h"

using namespace std;

// CDisplayManagerPlayerState::CDisplayManagerPlayerState.
//     IN mediaWindowConfig - initial media window configuration values.
// Constructor for a CDisplayManagerPlayerState instance.
CDisplayManagerPlayerState::CDisplayManagerPlayerState(const CMediaWindowConfig &mediaWindowConfig) :
m_mediaWindowConfig(mediaWindowConfig),
m_gameNotInProgressTickCount(0),
m_hasFirstPlay(true), // Default to prevent unwanted changes.
m_gameInProgress(true) // Default to prevent unwanted (timer-based) changes.
{
}

// CDisplayManagerPlayerState::~CDisplayManagerPlayerState.
// Destructor for a CDisplayManagerPlayerState instance.
CDisplayManagerPlayerState::~CDisplayManagerPlayerState()
{
}

// CDisplayManagerPlayerState::SetMediaWindowConfig.
//     IN mediaWindowConfig - new media window configuration values.
// Set member variable to new value.
void CDisplayManagerPlayerState::SetMediaWindowConfig(const CMediaWindowConfig &mediaWindowConfig)
{
	lock_guard<recursive_mutex> lock(m_memberCS);
	m_mediaWindowConfig.SetFromConfigAlways(mediaWindowConfig);
}

// CDisplayManagerPlayerState::SetHasFirstPlay
//     IN hasFirstPlay - indicates if the carded/uncarded player session has had its first EGM game play completed.
// Update the state to the value specified.
void CDisplayManagerPlayerState::SetHasFirstPlay(bool hasFirstPlay)
{
	m_hasFirstPlay = hasFirstPlay;
}

// CDisplayManagerPlayerState::GetHasFirstPlay
//     RETURN - true if carded/uncarded player session has had its first EGM game play completed;
//         otherwise, false is returned.
bool CDisplayManagerPlayerState::GetHasFirstPlay() const
{
	return m_hasFirstPlay;
}

// CDisplayManagerPlayerState::GetGameNotInProgressBeginSessionDelayInMilliseconds.
//     RETURN - the amount of time in milliseconds for a game to not be in progress before beginning
//         another media window player session.
DWORD CDisplayManagerPlayerState::GetGameNotInProgressBeginSessionDelayInMilliseconds() const
{
	lock_guard<recursive_mutex> lock(m_memberCS);
	DWORD returnValue = m_mediaWindowConfig.GetGameNotInProgressBeginSessionDelayInMilliseconds();
	return returnValue;
}

// CDisplayManagerPlayerState::GetGameNotInProgressEndSessionDelayInMilliseconds.
//     RETURN - the amount of time in milliseconds for a game to not be in progress before ending
//         the media window player session.
DWORD CDisplayManagerPlayerState::GetGameNotInProgressEndSessionDelayInMilliseconds() const
{
	lock_guard<recursive_mutex> lock(m_memberCS);
	DWORD returnValue = m_mediaWindowConfig.GetGameNotInProgressEndSessionDelayInMilliseconds();
	return returnValue;
}

// CDisplayManagerPlayerState::GetGameInProgress
//     RETURN - the game in progress status -- true for a game in progress, false otherwise.
bool CDisplayManagerPlayerState::GetGameInProgress() const
{
	return m_gameInProgress;
}

// CDisplayManagerPlayerState::GetGameNotInProgressTickCount
//     RETURN - the tick count for the system when m_gameNotInProgress was changed to false.
DWORD CDisplayManagerPlayerState::GetGameNotInProgressTickCount() const
{
	return m_gameNotInProgressTickCount;
}

// CDisplayManagerPlayerState::SetGameNotInProgressTickCount
//     IN currentTickCount - the current tick count for the system, generally from WINAPI ::GetTickCount().
void CDisplayManagerPlayerState::SetGameNotInProgressTickCount(DWORD currentTickCount)
{
	m_gameNotInProgressTickCount = currentTickCount;
}

// CDisplayManagerPlayerState::SetGameInProgress
//     IN inProgress - used to set the game in progress status flag.
//     IN currentTickCount - the current tick count for the system, generally from WINAPI ::GetTickCount().
void CDisplayManagerPlayerState::SetGameInProgress(bool inProgress, DWORD currentTickCount)
{
	// Don't update volatile status unless it needs to change.
	if (m_gameInProgress != inProgress)
	{
		if (m_gameInProgress && !inProgress)
		{
			// Make the tick count valid before setting status to not in progress.
			SetGameNotInProgressTickCount(currentTickCount);
		}
		
		m_gameInProgress = inProgress;
	}
}

// CDisplayManagerPlayerState::IsExpiredGameNotInProgressBeginSessionDelay
//     IN currentTickCount - the current tick count for the system, generally from WINAPI ::GetTickCount().
//     RETURN - true if the period that game is not in progress before starting an (uncarded) player session has elapsed.
//         - returns false otherwise.
// Returns whether the timer indicated above has expired.
bool CDisplayManagerPlayerState::IsExpiredGameNotInProgressBeginSessionDelay(DWORD currentTickCount) const
{
	DWORD millisecondsSinceGameNotInProgress = currentTickCount - GetGameNotInProgressTickCount();
	bool returnValue = millisecondsSinceGameNotInProgress >= GetGameNotInProgressBeginSessionDelayInMilliseconds();
	return returnValue;
}

// CDisplayManagerPlayerState::IsExpiredGameNotInProgressEndSessionDelay
//     IN currentTickCount - the current tick count for the system, generally from WINAPI ::GetTickCount().
//     RETURN - true if the period that game is not in progress before starting an (uncarded) player session has elapsed.
//         - returns false otherwise.
// Returns whether the timer indicated above has expired.
bool CDisplayManagerPlayerState::IsExpiredGameNotInProgressEndSessionDelay(DWORD currentTickCount) const
{
	DWORD millisecondsSinceGameNotInProgress = currentTickCount - GetGameNotInProgressTickCount();
	bool returnValue = millisecondsSinceGameNotInProgress >= GetGameNotInProgressEndSessionDelayInMilliseconds();
	return returnValue;
}
