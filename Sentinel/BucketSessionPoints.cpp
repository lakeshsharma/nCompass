#include "stdafx.h"

#include "BucketSessionPoints.h"
#include "Utilities.h"
#include "Logging/LogString.h"
#include "Diagnostics.h"
#include "DirectoryManager.h"

const LPCTSTR INVALIDMETERS = _T("Invalid meter update resulted in negative countdown increment.");

CBucketSessionPoints::CBucketSessionPoints()
{
	m_sessionPoints = 0;
	m_baseSessionPoints = 0;
	m_initialSessionPoints = 0;
	m_countdown = 0;
	m_startingCountdown = 0;
	m_startingCountdownPrecision = 0;
	m_startingMultiplier = 0;
	m_startingDivider = 0;
	m_bucketId = 0;
}

void CBucketSessionPoints::SetSessionPoints(const DWORD points)
{
	m_sessionPoints = points;
}

void CBucketSessionPoints::SetInitialSessionPoints(const DWORD points)
{
	m_initialSessionPoints = points;
}

DWORD CBucketSessionPoints::GetSessionPoints()const
{
	return m_sessionPoints;
}

DWORD CBucketSessionPoints::GetSessionCountdown()const
{
	return m_countdown;
}

void CBucketSessionPoints::SetSessionCountdown(const DWORD sessionCountDown)
{
	m_countdown = sessionCountDown;
}

DWORD CBucketSessionPoints::GetBaseSessionPoints()const
{
	return m_baseSessionPoints;
}

void CBucketSessionPoints::SetStartingMultiplierDividerCountdown(DWORD startingMultiplier, DWORD startingDivider, DWORD startingCountdown, DWORD startingCountdownHighPrecision, DWORD precisionMultilier)
{
	LogString(SESSION_LOG, "[bid:%u] %s:%d startingMultiplier: %u, startingDivider: %u", m_bucketId, __FUNCTION__, __LINE__, startingMultiplier, startingDivider);

	m_startingMultiplier = startingMultiplier;
	m_startingDivider = startingDivider;
	SetStartingCountdown(startingCountdown, startingCountdownHighPrecision, precisionMultilier);
}

void CBucketSessionPoints::SetStartingCountdown(DWORD startingCountdown, DWORD startingCountdownHighPrecision, DWORD precisionMultilier)
{
	m_startingCountdown = startingCountdown;

	// We need the m_startingCountdownPrecision to be in a factor of COUNTDOWN_PRECISION_MULTIPLIER
	// The supplied high precision countdown will be in another factor so we need to adjust
	if (precisionMultilier < COUNTDOWN_PRECISION_MULTIPLIER)
	{
		m_startingCountdownPrecision = startingCountdownHighPrecision * (COUNTDOWN_PRECISION_MULTIPLIER / precisionMultilier);
	}
	else
	{
		m_startingCountdownPrecision = startingCountdownHighPrecision / (precisionMultilier / COUNTDOWN_PRECISION_MULTIPLIER);
	}

	LogString(SESSION_LOG, "[bid:%u] %s:%d startingCountdown: %u, startingCountdownPrecision: %u", m_bucketId, __FUNCTION__, __LINE__, m_startingCountdown, m_startingCountdownPrecision);
}

void CBucketSessionPoints::SetDisplayName(const std::string displayName)
{
	m_displayName = displayName;
}

std::string CBucketSessionPoints::GetDisplayName() const
{
	return m_displayName;
}

void CBucketSessionPoints::SetBucketId(const DWORD id)
{
	m_bucketId = id;
}

DWORD CBucketSessionPoints::GetBucketId() const
{
	return m_bucketId;
}

void CBucketSessionPoints::SetBucketCurrency(const std::string bucketCurrency)
{
	m_bucketCurrency = bucketCurrency;
}

std::string CBucketSessionPoints::GetBucketCurrency() const
{
	return m_bucketCurrency;
}

void CBucketSessionPoints::CalculateSessionPointsAndCountdown(std::map<DWORD, CSubgameSession*> &subgameSessionMap,
	DWORD currentGameSessionSeqNo, bool isPartialBonusing, bool &pointsAdded)
{
	LogString(SESSION_LOG, "[bid:%u] %s:%d := Old sessionPoints: %u, and countdown: %u", m_bucketId, __FUNCTION__, __LINE__, m_sessionPoints, m_countdown);
	// Get the starting adjusted residual delta from. 
	uint64_t residualDelta = (uint64_t)ceil(((int64_t)m_startingDivider * COUNTDOWN_PRECISION_MULTIPLIER - m_startingCountdownPrecision) / (double)m_startingDivider);

	DWORD previousSessionPoints = m_sessionPoints;

	m_sessionPoints = m_initialSessionPoints;

	// running total of base (pre-multiplied) session points
	DWORD baseSessionPoints = 0;

	CalculateSessionPtsFromSubgameSessions(subgameSessionMap, residualDelta, m_startingMultiplier, baseSessionPoints);

	// determine the active multiplier and divider
	DWORD activeMultiplier = 0;
	DWORD activeDivider = 0;

	GetActiveMultiplierDivider(subgameSessionMap[currentGameSessionSeqNo], activeMultiplier, activeDivider);

	if (activeMultiplier != INVALID_MULTIPLIER && activeDivider != INVALID_DIVIDER)
	{
		// calculate the partial points from the residual
		if (isPartialBonusing)
		{
			DWORD partialPoints = (DWORD)(residualDelta * activeMultiplier) / COUNTDOWN_PRECISION_MULTIPLIER;
			baseSessionPoints += partialPoints;
			m_sessionPoints += (partialPoints * activeMultiplier);
		}

		// If session points were added, add the new session points to the Bonus Events
		if (m_sessionPoints > previousSessionPoints)
		{
			pointsAdded = (m_sessionPoints - previousSessionPoints);
		}

		//Update the m_baseSessionPoints
		m_baseSessionPoints = baseSessionPoints;

		// Calculate the new countdown
		m_countdown = activeDivider - ((DWORD)(residualDelta * activeDivider) / COUNTDOWN_PRECISION_MULTIPLIER);

		//The sentinel should NEVER display countdown of 0, even if it is, fall back to divider.
		if (m_countdown == 0)
		{
			m_countdown = activeDivider;
		}
	}

	LogString(SESSION_LOG, "[bid:%u] %s:%d := New sessionPoints: %u, and countdown: %u", m_bucketId, __FUNCTION__, __LINE__, m_sessionPoints, m_countdown);
}

void CBucketSessionPoints::CalculateSessionPtsFromSubgameSessions(std::map<DWORD, CSubgameSession*>& subgameSessionMap,
	uint64_t &residualDelta, DWORD residualMultiplier, DWORD &baseSessionPoints)
{
	// The multiplier for the residual weighted against the amount of the adjusted residual delta 
	uint64_t dividerAdjustedResidualMultiplier = residualDelta * residualMultiplier;

	for (auto subgameSessionIter : subgameSessionMap)
	{
		//Retrieve pointer to current subgame session map
		CSubgameSession *subgameSession = subgameSessionIter.second;
		if (subgameSession == NULL)
		{
			LogString(ERROR_LOG, "Error : CSubgameSession instance is NULL for Game Sequence No. = %u", subgameSessionIter.first);
			continue;
		}
		const SubgameSessionParameters & subgameSessionParams = subgameSession->GetSubgameSessionParameters();

		DWORD subgameMultiplier = 0;
		uint64_t subgameDivider = 0;
		uint64_t subgameSessionMeterDelta = 0;

		bool thisSubgametilted = false;

		if (m_bucketId == 0) // means legacy bucket
		{
			if (subgameSessionParams.m_multiplier == INVALID_MULTIPLIER ||
				subgameSessionParams.m_divider == INVALID_DIVIDER)
			{
				continue;
			}
			subgameMultiplier = subgameSessionParams.m_multiplier;
			subgameDivider = (uint64_t)subgameSessionParams.m_divider;
			subgameSessionMeterDelta = (uint64_t)subgameSession->GetMeterDelta(thisSubgametilted);

			if (!subgameDivider)
			{
				subgameDivider = NVRAM_PLAYER_TRACKING_DIVISOR_DEFAULT;
			}
		}
		else
		{
			auto itr = subgameSessionParams.m_ebSessionParams.find(m_bucketId);
			if (itr == subgameSessionParams.m_ebSessionParams.end())
			{
				continue;
			}

			subgameMultiplier = itr->second.m_multiplier;
			subgameDivider = (uint64_t)itr->second.m_divider;
			subgameSessionMeterDelta = (uint64_t)subgameSession->GetMeterDelta(thisSubgametilted, true);
		}

		// Get the adjusted divider for this subgame session (percentage * 100000)
		uint64_t dividerAdjustedSubgameDelta = (subgameSessionMeterDelta * COUNTDOWN_PRECISION_MULTIPLIER) / subgameDivider;

		// Add the adjusted residual delta
		uint64_t dividerAdjustedDelta = dividerAdjustedSubgameDelta + residualDelta;

		// Determine the base session points for this subgame session
		// subgameSessionPoints is block-local 
		DWORD subgameSessionPoints = (DWORD)(dividerAdjustedDelta / COUNTDOWN_PRECISION_MULTIPLIER);

		// Add to the running total of base session points
		baseSessionPoints += subgameSessionPoints;

		// If points awarded with this subgame
		if (subgameSessionPoints > 0)
		{
			// Get the new residual delta
			residualDelta = dividerAdjustedDelta % ((uint64_t)subgameSessionPoints * COUNTDOWN_PRECISION_MULTIPLIER);

			// The multiplier for this subgame weighted against the amount of the adjusted subgame delta
			uint64_t dividerAdjustedSubgameMultiplier = (dividerAdjustedSubgameDelta - residualDelta) * subgameMultiplier;

			// Multiply the session points by the weighted multiplier
			DWORD multipliedSubgameSessionPoints = (DWORD)((dividerAdjustedResidualMultiplier + dividerAdjustedSubgameMultiplier) / COUNTDOWN_PRECISION_MULTIPLIER);

			DWORD residualSessionPts = (dividerAdjustedResidualMultiplier + dividerAdjustedSubgameMultiplier) % COUNTDOWN_PRECISION_MULTIPLIER;

			// Add the multiplied session points to the total session points
			m_sessionPoints += multipliedSubgameSessionPoints;

			// Get the new divider adjusted residual multipler 
			dividerAdjustedResidualMultiplier = (residualDelta * subgameMultiplier) + residualSessionPts;
		}

		// No points awarded with this subgame.
		else if (!thisSubgametilted)
		{
			// Get the new residual delta
			residualDelta = dividerAdjustedDelta;

			// Add to the residual 
			dividerAdjustedResidualMultiplier += (dividerAdjustedSubgameDelta * subgameMultiplier);
		}

		if (thisSubgametilted)
		{
			CDiagnostics::ReportErrorToFile(CDirectoryManager::PollerBadMessageDiagnosticsTextFilePath(), INVALIDMETERS, _T("NOPTS"));
		}
	}
}

void CBucketSessionPoints::GetActiveMultiplierDivider(CSubgameSession *currentSubgameSession, DWORD &activeMultiplier, DWORD &activeDivider)
{
	activeMultiplier = m_startingMultiplier;
	activeDivider = m_startingDivider;

	if (currentSubgameSession && (currentSubgameSession->GetStatus() == SubgameSessionStatus::Active))
	{
		const SubgameSessionParameters & subgameSessionParams = currentSubgameSession->GetSubgameSessionParameters();
		if (m_bucketId == 0)  // means legacy bucket
		{
			activeMultiplier = subgameSessionParams.m_multiplier;
			activeDivider = subgameSessionParams.m_divider;
		}
		else
		{
			const auto itr = subgameSessionParams.m_ebSessionParams.find(m_bucketId);
			if (itr != subgameSessionParams.m_ebSessionParams.end())
			{
				activeMultiplier = itr->second.m_multiplier;
				activeDivider = itr->second.m_divider;
			}
		}
	}
}
