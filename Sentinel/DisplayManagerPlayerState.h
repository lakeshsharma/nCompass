#pragma once

#include "Config/MediaWindowConfig.h"

// CDisplayManagerPlayerState
// Encapsulate Media (service) Window player state (and related) information and
// provide methods for managing that information.
class CDisplayManagerPlayerState
{
public:
	CDisplayManagerPlayerState(const CMediaWindowConfig &mediaWindowConfig);
	virtual ~CDisplayManagerPlayerState(void);
	void SetMediaWindowConfig(const CMediaWindowConfig &mediaWindowConfig);
	DWORD GetGameNotInProgressBeginSessionDelayInMilliseconds() const;
	DWORD GetGameNotInProgressEndSessionDelayInMilliseconds() const;

	void SetHasFirstPlay(bool hasFirstPlay);
	bool GetHasFirstPlay() const;

	void SetGameInProgress(bool inProgress, DWORD currentTickCount);
	bool GetGameInProgress() const;
	DWORD GetGameNotInProgressTickCount() const;
	bool IsExpiredGameNotInProgressBeginSessionDelay(DWORD currentTickCount) const;
	bool IsExpiredGameNotInProgressEndSessionDelay(DWORD currentTickCount) const;
private:
	// Disable default constructor, default copy constructor, and default assignment operator.
	CDisplayManagerPlayerState();
	CDisplayManagerPlayerState(const CDisplayManagerPlayerState &rhs);
	CDisplayManagerPlayerState& operator=(const CDisplayManagerPlayerState &rhs);

	void SetGameNotInProgressTickCount(DWORD currentTickCount);

	// "mutable" allows the critical section to be used inside const methods.
	mutable std::recursive_mutex m_memberCS;
	CMediaWindowConfig m_mediaWindowConfig;
	volatile DWORD m_gameNotInProgressTickCount;
	volatile bool m_hasFirstPlay;
	volatile bool m_gameInProgress;
};
