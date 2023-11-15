#include "stdafx.h"

#include "SessionPoints.h"
#include "Utilities.h"
#include "Logging/LogString.h"
#include "Diagnostics.h"
#include "DirectoryManager.h"

CSessionPoints::CSessionPoints()
{
	m_sessionPointsFlushed = 0;
	m_bonusEvents = nullptr;
	m_sessionStartMeters = nullptr;
	m_sessionStartMetersCurrent = nullptr;
	m_lastPlayedSubgameNumber = 0;
	m_lastPlayedSubgameDenom = 0;
	m_currentSubgameSession = nullptr;
	m_firstPlay = true;
	m_deactivated = false;
	m_sessionBucketsListIdx = 0;
}

CSessionPoints::CSessionPoints(const CSessionPoints &sessionPoints) 
{
	m_bonusEvents = nullptr;
	m_sessionStartMeters = nullptr;
	m_sessionStartMetersCurrent = nullptr;
	m_currentSubgameSession = nullptr;
	CopyObj(sessionPoints);
}

CSessionPoints & CSessionPoints::operator=(const CSessionPoints &sessionPointsRHS)
{
	if (this != &sessionPointsRHS)
	{
		CleanUp();
		CopyObj(sessionPointsRHS);
	}
	return *this;
}

CSessionPoints::~CSessionPoints()
{
	CleanUp();
}

void CSessionPoints::CopyObj(const CSessionPoints &sessionPointsRHS)
{
	// invoke base class assignment operator to copy base class data.
	CBucketSessionPoints::operator=(sessionPointsRHS);

	m_sessionPointsFlushed = sessionPointsRHS.m_sessionPointsFlushed;
	m_lastPlayedSubgameNumber = sessionPointsRHS.m_lastPlayedSubgameNumber;
	m_lastPlayedSubgameDenom = sessionPointsRHS.m_lastPlayedSubgameDenom;
	m_firstPlay = sessionPointsRHS.m_firstPlay;
	m_deactivated = sessionPointsRHS.m_deactivated;

	if (m_bonusEvents == nullptr && sessionPointsRHS.m_bonusEvents != nullptr)
	{
		m_bonusEvents = new CBonusEvents(*sessionPointsRHS.m_bonusEvents);
	}

	m_sessionStartMeters = nullptr;
	m_sessionStartMetersCurrent = nullptr;

	if (m_currentSubgameSession == nullptr && sessionPointsRHS.m_currentSubgameSession != nullptr)
	{
		m_currentSubgameSession = new CSubgameSession(*sessionPointsRHS.m_currentSubgameSession);
		m_subgameSessionMap[m_currentSubgameSession->GetSubgameSessionParameters().m_gameSessionSequenceNumber] = m_currentSubgameSession;
	}

	{
		lock_guard<recursive_mutex> lock(sessionPointsRHS.m_sessionBucketsListCriticalSection);

		// Iterate over the current bucket list and create copy of only these objects.
		for (auto bucketSessionPoints : sessionPointsRHS.m_sessionBucketsList)
		{
			if (this->m_bucketId == bucketSessionPoints->GetBucketId())
			{
				m_sessionBucketsList.push_back(this);
			}
			else
			{
				const auto bucketSessionPointsItr = sessionPointsRHS.m_ebSessionPointsMap.find(bucketSessionPoints->GetBucketId());
				if (bucketSessionPointsItr != sessionPointsRHS.m_ebSessionPointsMap.end())
				{
					CBucketSessionPoints *bucketSessionPts = new CBucketSessionPoints(*bucketSessionPointsItr->second);
					m_sessionBucketsList.push_back(bucketSessionPts);
					m_ebSessionPointsMap[bucketSessionPoints->GetBucketId()] = bucketSessionPts;  // to make sure the object gets deleted in the destructor.
				}
			}
		}
		m_sessionBucketsListIdx = sessionPointsRHS.m_sessionBucketsListIdx;
	}
}

void CSessionPoints::CleanUp()
{
	if (m_bonusEvents != nullptr)
	{
		delete m_bonusEvents;
		m_bonusEvents = nullptr;
	}
	if (m_sessionStartMeters != nullptr)
	{
		delete m_sessionStartMeters;
		m_sessionStartMeters = nullptr;
	}
	if (m_sessionStartMetersCurrent != nullptr)
	{
		delete m_sessionStartMetersCurrent;
		m_sessionStartMetersCurrent = nullptr;
	}

	// Need to delete all items in map, which includes m_currentSubgameSession
	for (auto& kv : m_subgameSessionMap)
	{
		delete kv.second;
	}
	m_subgameSessionMap.clear();
	m_currentSubgameSession = nullptr;

	for (auto& kv : m_ebSessionPointsMap)
	{
		delete kv.second;
	}
	m_ebSessionPointsMap.clear();
	m_sessionBucketsList.clear();
}

void CSessionPoints::SetSessionPointsFlushed(const DWORD points)
{
	m_sessionPointsFlushed = points;
}

DWORD CSessionPoints::GetSessionPointsFlushed()const
{
	return m_sessionPointsFlushed;
}

CBonusEvents * CSessionPoints::GetBonusEvents() const
{
	return m_bonusEvents;
}

void CSessionPoints::SetBonusEvents(CBonusEvents *events)
{
	m_bonusEvents = events;
}

void CSessionPoints::BuildSessionStartMeters() // Build session start meters from NVRAM, called by CPlayer class.
{
	if (CMemoryStorage::IsActiveLocation(NVRAM_PLAYER_START_METERS_OFFSET, ACTIVE_LOCATION))
	{
		// Make "meters" valid before assigning to "m_sessionStartMeters".
		CMeters *meters = new CMeters(MEMORY_NVRAM);
		meters->SetOffset(NVRAM_PLAYER_START_METERS_OFFSET);
		meters->SetAllowedSize(NVRAM_PLAYER_START_METERS_SIZE);
		meters->BuildYourself();

		lock_guard<recursive_mutex> lock(m_sessionStartMetersCritSection);
		{
			m_sessionStartMeters = meters;
		}
	}
	else
	{
		lock_guard<recursive_mutex> lock(m_sessionStartMetersCritSection);
		{
			m_sessionStartMeters = NULL;
		}
	}
	// Meter captured at the time of the start of the player session Current value
	if (CMemoryStorage::IsActiveLocation(NVRAM_PLAYER_START_METERS_CURRENT_OFFSET, ACTIVE_LOCATION))
	{
		// Make "meters" valid before assigning to "m_sessionStartMeters".
		CMeters *meters = new CMeters(MEMORY_NVRAM);
		meters->SetOffset(NVRAM_PLAYER_START_METERS_CURRENT_OFFSET);
		meters->SetAllowedSize(NVRAM_PLAYER_START_METERS_CURRENT_SIZE);
		meters->BuildYourself();

		lock_guard<recursive_mutex> lock(m_sessionStartMetersCritSection);
		{
			m_sessionStartMetersCurrent = meters;
		}
	}
	else
	{
		lock_guard<recursive_mutex> lock(m_sessionStartMetersCritSection);
		{
			m_sessionStartMetersCurrent = NULL;
		}
	}

}

CMeters *CSessionPoints::GetSessionStartMetersCopy(void) const
{
	CMeters *meters = NULL;

	lock_guard<recursive_mutex> lock(m_sessionStartMetersCritSection);
	{
		if (m_sessionStartMeters)
		{
			meters = new CMeters(*m_sessionStartMeters);
		}
	}
	return meters;
}

CMeters* CSessionPoints::GetSessionStartMeters(void) const
{
	lock_guard<recursive_mutex> lock(m_sessionStartMetersCritSection);
	return m_sessionStartMeters;
}

void CSessionPoints::SetSessionStartMeters(CMeters* meters)
{
	lock_guard<recursive_mutex> lock(m_sessionStartMetersCritSection);
	{
		if (m_sessionStartMeters != NULL)
		{
			delete m_sessionStartMeters;
			m_sessionStartMeters = NULL;
		}
		m_sessionStartMeters = meters;
	}
}

CMeters* CSessionPoints::GetSessionStartMetersCurrent() const
{
	lock_guard<recursive_mutex> lock(m_sessionStartMetersCritSection);
	return m_sessionStartMetersCurrent;
}

void CSessionPoints::SetSessionStartMetersCurrent(CMeters* meter)
{
	lock_guard<recursive_mutex> lock(m_sessionStartMetersCritSection);
	{
		if (m_sessionStartMetersCurrent != NULL)
		{
			delete m_sessionStartMetersCurrent;
			m_sessionStartMetersCurrent = NULL;
		}
		m_sessionStartMetersCurrent = meter;
	}
}

// Reset current version of "BonusEvents"
void CSessionPoints::ResetCurrentBonusData()
{
	// Reset Bonus events
	if (m_bonusEvents != NULL)
		m_bonusEvents->ResetCurrentAwardedPoints();
}

DWORD CSessionPoints::GetBonusPointsCurrent() const
{
	DWORD bonusPointsCurrent = 0;

	if (m_bonusEvents != NULL)
		bonusPointsCurrent = m_bonusEvents->GetBonusPointsCurrent();

	return bonusPointsCurrent;
}

DWORD CSessionPoints::GetBonusPoints() const
{
	DWORD bonusPoints = 0;

	if (m_bonusEvents != NULL)
		bonusPoints = m_bonusEvents->GetBonusPoints();

	return bonusPoints;
}

bool CSessionPoints::SetGameSession(SubgameSessionParameters &subgameSessionParams)
{
	LogString(SESSION_LOG, "%s:%d: m_gameSessionSequenceNumber: %u, m_gameNumber: %u, m_denom : %u", __FUNCTION__, __LINE__,
		subgameSessionParams.m_gameSessionSequenceNumber, subgameSessionParams.m_gameNumber, subgameSessionParams.m_denom);

	bool subgameSessionCreated(false);
	bool oldSubgameSessionClosed(false);

	// If this is not the first time then close the old subgame session
	if (m_currentSubgameSession != nullptr)
	{
		DWORD oldGameSessionSequenceNumber =
			m_currentSubgameSession->GetSubgameSessionParameters().m_gameSessionSequenceNumber;

		oldSubgameSessionClosed = m_currentSubgameSession->CloseSession(oldGameSessionSequenceNumber);
	}
	else
	{
		oldSubgameSessionClosed = true;  //otherwise set the retStatus to true
		m_firstPlay = true;
	}

	if (subgameSessionParams.m_gameSessionSequenceNumber == 0)
	{
		// Default subgame, don't send trans 127
		m_firstPlay = false;
		m_lastPlayedSubgameDenom = subgameSessionParams.m_denom;
		m_lastPlayedSubgameNumber = subgameSessionParams.m_gameNumber;
	}

	// second,  create new game session and store in dictionary only if old subgame session was 
	// closed successfully or create it only if it's the very first subgame session 
	if (oldSubgameSessionClosed)
	{
		subgameSessionCreated = CreateNewSubgameSession(subgameSessionParams);
	}

	return subgameSessionCreated;
}

bool CSessionPoints::CreateNewSubgameSession(SubgameSessionParameters &subgameSessionParams)
{
	LogString(SESSION_LOG, "%s:%d", __FUNCTION__, __LINE__);

	CSubgameSession *newSubgameSession = new CSubgameSession(subgameSessionParams);
	if (subgameSessionParams.m_gameSessionSequenceNumber == 0)
	{
		// Default subgame, set status to active
		newSubgameSession->SetStatus(SubgameSessionStatus::Active);
	}

	bool subgameSessionCreated(false);

	//Taking value in a temp var to avoid repetition
	DWORD gameSessionSequenceNumber = subgameSessionParams.m_gameSessionSequenceNumber;

	if (newSubgameSession != nullptr)
	{
		// Determine if the previous subgame sessions had no game play going back to this same subgame/denom combo.
		bool prevPlayed = false;
		int gameStartCount = 0;
		std::map<DWORD, CSubgameSession *>::reverse_iterator rit = m_subgameSessionMap.rbegin();
		for (; rit != m_subgameSessionMap.rend(); ++rit)
		{
			CSubgameSession *session = rit->second;
			if (newSubgameSession->IsSameGameNumberAndDenom(session))
			{
				// Same subgame/denom as new session. Stop traversing map
				gameStartCount = session->GetGameStartCount();
				break;
			}
			else if (session->GetGameStartCount() > 0)
			{
				// previous game played
				prevPlayed = true;
				break;
			}
		}

		m_subgameSessionMap[gameSessionSequenceNumber] = newSubgameSession;

		// if prev "other" games have not been played and this game was played, make sure it won't return "first play" 
		if (!prevPlayed && gameStartCount > 0)
		{
			// There hasn't been any play since this game was last selected.
			newSubgameSession->IncrementGameStart();
		}
		m_currentSubgameSession = newSubgameSession;

		// Update countdown enabled buckets list for current session
		RefreshSessionBucketsList();

		subgameSessionCreated = true;
	}
	else
	{
		LogString(ERROR_LOG, "%s:%d := Failed to create new subgame session object for gsnId:%u",
			__FUNCTION__, __LINE__, gameSessionSequenceNumber);
	}

	return subgameSessionCreated;
}

bool CSessionPoints::ApplySubgameSessionParams(SubgameSessionParameters &subgameSessionParams, bool isPartialBonusing, bool& pointsAdded)
{
	bool applyingSubgameSessionParamsStatus(false);

	if (!m_deactivated)
	{
		LogString(SESSION_LOG, "%s:%d := Multiplier: %u, Divider: %u, m_gameSessionSequenceNumber: %u, m_gameNumber: %u, m_denom : %u", __FUNCTION__, __LINE__,
			subgameSessionParams.m_multiplier, subgameSessionParams.m_divider, subgameSessionParams.m_gameSessionSequenceNumber, subgameSessionParams.m_gameNumber, subgameSessionParams.m_denom);

		//Taking value in a temp var to shorten the stmt
		DWORD gameSessionSequenceNumber = subgameSessionParams.m_gameSessionSequenceNumber;

		std::map<DWORD, CSubgameSession*>::iterator subgameSessionIter = m_subgameSessionMap.find(gameSessionSequenceNumber);

		if (subgameSessionIter != m_subgameSessionMap.end())
		{
			//Retrieve pointer to current subgame session map
			CSubgameSession *sgs = subgameSessionIter->second;
			if (sgs)
			{
				//First retrieve the existing values from the SubgameSession class and apply that to the current subgameSessionParams
				//as 423 msg response is not sending us back these values. So take care of that before applying.
				subgameSessionParams.m_denom = sgs->GetSubgameSessionParameters().m_denom;
				subgameSessionParams.m_gameNumber = sgs->GetSubgameSessionParameters().m_gameNumber;

				applyingSubgameSessionParamsStatus = sgs->ApplySubgameSessionParameters(subgameSessionParams);

				/*If subgamesession becomes active/closed and Subgame session params could be applied successfully
				then calculate session points and countdown now*/
				if (applyingSubgameSessionParamsStatus && (sgs->GetStatus() == SubgameSessionStatus::Active))
				{
					LogString(SESSION_LOG, "%s:%d := Calculating Session Points and Countdown as subgame session recvd parameters", __FUNCTION__, __LINE__);

					// Update countdown enabled buckets list for current session
					RefreshSessionBucketsList();

					bool bonusPointsAwarded(false);
					CalculateSessionPointsAndCountdown(isPartialBonusing, bonusPointsAwarded, pointsAdded);
				}
			}
			else
			{
				LogString(ERROR_LOG, "%s:%d := Error occurred in retrieving subgame session object for gameSessionSeqNo:%u",
					__FUNCTION__, __LINE__, gameSessionSequenceNumber);
			}
		}
		else
		{
			LogString(ERROR_LOG, "%s:%d := Subgame Session object doesn't exists for gameSessionSeqNo:%u",
				__FUNCTION__, __LINE__, gameSessionSequenceNumber);
		}

	}
	else
	{
		LogString(SESSION_LOG, "MGMD has been deactivated for this session, will not apply subgame parameters");
	}

	return applyingSubgameSessionParamsStatus;
}

CSubgameSession* CSessionPoints::GetCurrentSubgameSession()
{
	return m_currentSubgameSession;
}

bool CSessionPoints::GameStart()
{
	bool firstGameStart(false);

	if (m_currentSubgameSession != nullptr)
	{
		// Also, check whether it's firstgame start in active session state. Status check
		// will be done inside SubgameSession class.
		firstGameStart = m_currentSubgameSession->IncrementGameStart();

		// It could be that game session was back to last played one without playing in between. If that's the case, then 
		// reset firstGameStart else update last Played Game info with current one on first game start
		if (firstGameStart)
		{
			DWORD currentPlayedSubgameNumber = m_currentSubgameSession->GetSubgameSessionParameters().m_gameNumber;
			DWORD currentPlayedSubgameDenom = m_currentSubgameSession->GetSubgameSessionParameters().m_denom;

			if ((m_lastPlayedSubgameNumber != currentPlayedSubgameNumber) || (m_lastPlayedSubgameDenom != currentPlayedSubgameDenom))
			{
				m_lastPlayedSubgameDenom = currentPlayedSubgameDenom;
				m_lastPlayedSubgameNumber = currentPlayedSubgameNumber;
				LogString(SESSION_LOG, "%s:%d := Updated last Played SubGame Number: %u and Denom: %u", __FUNCTION__, __LINE__, m_lastPlayedSubgameNumber, m_lastPlayedSubgameDenom);
			}
			else
			{
				firstGameStart = false;
			}

		}

		if (m_firstPlay)
			m_firstPlay = false;
	}

	LogString(SESSION_LOG, "%s:%d := Is FirstGameStart:%u", __FUNCTION__, __LINE__, firstGameStart);

	return firstGameStart;
}

bool CSessionPoints::ApplyMeterChange(CMeters *meters, CMeters *origMeters, bool isPartialBonusing, bool isMGMDEnabled, bool &isBonusPointsAwarded, bool& pointsAdded)
{
	bool isMeterDeltaApplied(false);

	if (m_currentSubgameSession != nullptr)
	{
		isMeterDeltaApplied = m_currentSubgameSession->ApplyMeterChange(meters, origMeters);
	}

	//Also check that current subgame session is active one
	if (isMeterDeltaApplied && (!isMGMDEnabled || m_currentSubgameSession->GetStatus() == SubgameSessionStatus::Active))
	{
		CalculateSessionPointsAndCountdown(isPartialBonusing, isBonusPointsAwarded, pointsAdded);
	}

	return isMeterDeltaApplied;
}


bool CSessionPoints::AwardBonusPoints(DWORD addedBasePoints)
{
	bool bBonusPointsAwarded = false;

	if (addedBasePoints > 0)
	{
		if (m_bonusEvents != NULL)
		{
			bBonusPointsAwarded = m_bonusEvents->AwardBonusPoints(addedBasePoints);
		}
	}

	return bBonusPointsAwarded;
}

DWORD CSessionPoints::GetSubgameSessionCount() const
{
	return m_subgameSessionMap.size();
}

void CSessionPoints::DeactivateMGMD()
{
	m_deactivated = true;
}

bool CSessionPoints::InitializeExtendedBucketCountdownTracking(const ExtendedBucketsCountdownData *ebCountdownData)
{
	bool ebCountdownInited = false;

	if (ebCountdownData == NULL)
	{
		return ebCountdownInited;
	}

	auto itr = m_ebSessionPointsMap.find(ebCountdownData->id);
	if (itr == m_ebSessionPointsMap.end())
	{
		// Session points and countdown tracking hasn't been started for this bucket yet
		CBucketSessionPoints *ebSessionPoints = new CBucketSessionPoints();
		ebSessionPoints->SetSessionCountdown(ebCountdownData->countdown ? ebCountdownData->countdown : ebCountdownData->divider);
		ebSessionPoints->SetStartingMultiplierDividerCountdown(ebCountdownData->multiplier, ebCountdownData->divider, ebCountdownData->countdown, ebCountdownData->countdown, 1);
		ebSessionPoints->SetDisplayName(ebCountdownData->displayName);
		ebSessionPoints->SetBucketCurrency(ebCountdownData->bucketCurrency);
		ebSessionPoints->SetBucketId(ebCountdownData->id);

		m_ebSessionPointsMap[ebCountdownData->id] = ebSessionPoints;

		ebCountdownInited = true;
	}

	return ebCountdownInited;
}

void CSessionPoints::CalculateSessionPointsAndCountdown(bool isPartialBonusing, bool &isBonusPointsAwarded, bool& pointsAdded)
{
	pointsAdded = 0;

	// Call for legacy bucket countdown recalculations
	CBucketSessionPoints::CalculateSessionPointsAndCountdown(m_subgameSessionMap,
		m_currentSubgameSession->GetSubgameSessionParameters().m_gameSessionSequenceNumber, isPartialBonusing, pointsAdded);

	if (0 != pointsAdded)
	{
		isBonusPointsAwarded = AwardBonusPoints(pointsAdded);
	}

	// Call for extended buckets countdown recalculations, for buckets with countdowns applicable to current subgame only.
	for (CBucketSessionPoints* ebSessionPointListEntry : m_sessionBucketsList)
	{
		if (this == ebSessionPointListEntry)
		{
			// Legacy bucket instance, already calculated for this, so ignore it here.
			continue;
		}

		ebSessionPointListEntry->CalculateSessionPointsAndCountdown(m_subgameSessionMap,
			m_currentSubgameSession->GetSubgameSessionParameters().m_gameSessionSequenceNumber, isPartialBonusing, pointsAdded);
	}
}

void CSessionPoints::GetActiveMultiplierDivider(DWORD &activeMultiplier, DWORD &activeDivider)
{
	activeMultiplier = m_startingMultiplier;
	activeDivider = m_startingDivider;

	if (m_currentSubgameSession && (m_currentSubgameSession->GetStatus() == SubgameSessionStatus::Active))
	{
		activeMultiplier = m_currentSubgameSession->GetSubgameSessionParameters().m_multiplier;
		activeDivider = m_currentSubgameSession->GetSubgameSessionParameters().m_divider;
	}
}

void CSessionPoints::RefreshSessionBucketsList()
{
	std::vector<CBucketSessionPoints*> sessionBucketsList;

	const SubgameSessionParameters & curSessionParams = m_currentSubgameSession->GetSubgameSessionParameters();
	if (curSessionParams.m_multiplier != INVALID_MULTIPLIER &&
		curSessionParams.m_divider != INVALID_DIVIDER)
	{
		sessionBucketsList.push_back(this);
	}

	for (auto& ebCountdownCalcInfo : curSessionParams.m_ebSessionParams)
	{
		auto itr = m_ebSessionPointsMap.find(ebCountdownCalcInfo.first);
		if (itr != m_ebSessionPointsMap.end())
		{
			sessionBucketsList.push_back(itr->second);
		}
	}

	// critical section for bucket list 
	{
		lock_guard<recursive_mutex> lock(m_sessionBucketsListCriticalSection);
		m_sessionBucketsListIdx = 0;
		m_sessionBucketsList = sessionBucketsList;
	}
}

bool CSessionPoints::GetCurrentBucketDetails(DWORD &id, std::string &name, std::string &currency, DWORD &countdown, DWORD &sessionPoints) const
{
	bool returnVal = false;
	lock_guard<recursive_mutex> lock(m_sessionBucketsListCriticalSection);

	if (m_sessionBucketsListIdx >= m_sessionBucketsList.size())
	{
		LogString(SESSION_LOG, "WARNING : %s %d (index %u >= size %d)", __FUNCTION__, __LINE__, m_sessionBucketsListIdx, m_sessionBucketsList.size());

		id = 0;
		name.clear();
		currency.clear();
		countdown = 0;
		sessionPoints = 0;
	}
	else
	{
		id = m_sessionBucketsList[m_sessionBucketsListIdx]->GetBucketId();
		name = m_sessionBucketsList[m_sessionBucketsListIdx]->GetDisplayName();
		currency = m_sessionBucketsList[m_sessionBucketsListIdx]->GetBucketCurrency();
		countdown = m_sessionBucketsList[m_sessionBucketsListIdx]->GetSessionCountdown();
		sessionPoints = m_sessionBucketsList[m_sessionBucketsListIdx]->GetSessionPoints();

		returnVal = true;
	}

	return returnVal;
}

void CSessionPoints::MoveToNextBucket()
{
	lock_guard<recursive_mutex> lock(m_sessionBucketsListCriticalSection);
	if (++m_sessionBucketsListIdx == m_sessionBucketsList.size())
	{
		m_sessionBucketsListIdx = 0;
	}
}

WORD CSessionPoints::GetBucketCount() const
{
	return m_sessionBucketsList.size();
}

void CSessionPoints::GetBucketsDisplayData(std::vector<std::unique_ptr<BucketDisplayData>> &data, DWORD &gameNumber, int &denom) const
{
	lock_guard<recursive_mutex> lock(m_sessionBucketsListCriticalSection);

	for (auto itr : m_sessionBucketsList)
	{
		if (itr->GetBucketId())
		{
			std::unique_ptr<BucketDisplayData> bucketDisplayData(new BucketDisplayData);
			bucketDisplayData->id = itr->GetBucketId();
			bucketDisplayData->bucketName = itr->GetDisplayName();
			bucketDisplayData->displayName = bucketDisplayData->bucketName;
			bucketDisplayData->currency = itr->GetBucketCurrency();
			bucketDisplayData->countdown = itr->GetSessionCountdown();
			bucketDisplayData->sessionPoints = itr->GetSessionPoints();

			data.push_back(std::move(bucketDisplayData));
		}
	}

	if (m_currentSubgameSession)
	{
		gameNumber = m_currentSubgameSession->GetSubgameSessionParameters().m_gameNumber;
		denom = m_currentSubgameSession->GetSubgameSessionParameters().m_denom;
	}
}

DWORD CSessionPoints::GetEBSessionPoints(const DWORD bucketId) const
{
	DWORD sessionPoints = 0;

	auto bucketSessionPtsItr = m_ebSessionPointsMap.find(bucketId);
	if (bucketSessionPtsItr != m_ebSessionPointsMap.end())
	{
		sessionPoints = bucketSessionPtsItr->second->GetSessionPoints();
	}

	return sessionPoints;
}

DWORD CSessionPoints::GetEBSessionCountdown(const DWORD bucketId) const
{
	DWORD countdown = 0;

	auto bucketSessionPtsItr = m_ebSessionPointsMap.find(bucketId);
	if (bucketSessionPtsItr != m_ebSessionPointsMap.end())
	{
		countdown = bucketSessionPtsItr->second->GetSessionCountdown();
	}

	return countdown;
}
