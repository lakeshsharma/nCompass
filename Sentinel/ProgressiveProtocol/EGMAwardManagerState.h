#pragma once

/// <summary>
/// EGM award manager state (for display on web diagnostics pages).
/// </summary>
class CEGMAwardManagerState
{
public:
	// Members used to manage the CEGMAward instance.
	static const DWORD SIGNAL_TICK_COUNT_EXPIRED;
	static const DWORD AWARD_TICK_COUNT_EXPIRED;
	static const DWORD GCF_DELAY_TICK_COUNT_EXPIRED;

	// AFT_LOCK_PENDING_TICK_COUNT_EXPIRED is used with m_state.m_gcfDelayTickCountStart and m_state.m_gcfDelayExpired
	// when m_egmAward.GetAwardState() == AftLockPending.
	static const DWORD AFT_LOCK_PENDING_TICK_COUNT_EXPIRED;
	static const DWORD PRIZE_AWARD_LOCK_REFRESH;

	// m_gcfDelayTickCountStart is not valid unless m_waitingForGCF == false and m_gcfDelayExpired == false.
	DWORD m_gcfDelayTickCountStart = 0;

	// m_awardTickCountStart is not valid unless m_awardExpired == false.
	DWORD m_awardTickCountStart = 0; // When we started processing the award.
	DWORD m_lastPrizeGameLock = 0; // Last time egm lock/delay sent for prize award

	bool m_awardExpired = false;
	// GCF retries will occur 3 seconds apart.
	// GCF tries can occur immediately.
	bool m_waitingForGCF = false; // Do not allow more than one outstanding request at a time.
	bool m_gcfDelayExpired = true;
};


