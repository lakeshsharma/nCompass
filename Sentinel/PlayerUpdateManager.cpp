#include "stdafx.h"
#include "PlayerUpdateManager.h"

#include "Utilities.h"
#include "Logging/LogString.h"

using namespace std;

const int PlayerUpdateIntervalInSeconds = 3600;	// Interval to send player updates in seconds.
const int PlayerUpdateFlushIntervalInSeconds = 300;	// Interval to flush player updates in seconds (5 mins).

CPlayerUpdateManager::CPlayerUpdateManager() 
{
}


CPlayerUpdateManager::~CPlayerUpdateManager()
{
	while (m_playerUpdateTimes.size() > 0)
	{
		time_t *removedTime = (time_t *)m_playerUpdateTimes.front();
		m_playerUpdateTimes.pop_front();
		delete removedTime;
	}
}


void CPlayerUpdateManager::AddPlayerUpdateTime(time_t *newUpdateTime)
{
	lock_guard<recursive_mutex> lock(m_criticalSection);

	if (*newUpdateTime > CUtilities::GetCurrentTime())
	{
		bool bUpdateTimeAdded = false;
		auto pos = m_playerUpdateTimes.rbegin();
		for (int ti = m_playerUpdateTimes.size() - 1; ti >= 0 && !bUpdateTimeAdded; ti--)
		{
			time_t *updateTime = (time_t *)*pos;

			if (*updateTime < *newUpdateTime)
			{
				LogString(GENERAL, "PlayerUpdateManager: Player update time inserted into list.");
				m_playerUpdateTimes.insert(pos.base(), newUpdateTime);
				bUpdateTimeAdded = true;
			}

			// Check for duplicate.
			else if (*updateTime == *newUpdateTime)
			{
				delete newUpdateTime;
				bUpdateTimeAdded = true;
			}

			++pos;
		}

		if (!bUpdateTimeAdded)
		{
			LogString(GENERAL, "PlayerUpdateManager: Player update time added to end of list");
			m_playerUpdateTimes.push_front(newUpdateTime);
		}
	}
	else
	{
		LogString(GENERAL, "PlayerUpdateManager: update time to be added is in past");
		delete newUpdateTime;
	}
}

bool CPlayerUpdateManager::CheckIfPlayerUpdateNeedsToBeSent(time_t lastPlayerUpdateTime)
{
	time_t currentTime = CUtilities::GetCurrentTime();

	m_criticalSection.lock();
	// Check to see if we have a player update time.
	// Remove all of the old times in the process.
	bool bIsPlayerUpdateTime = false;
	bool bDoneFlushing = false;
	while (m_playerUpdateTimes.size() > 0 && !bDoneFlushing)
	{
		time_t *updateTime = (time_t *)m_playerUpdateTimes.front();

		if (*updateTime <= currentTime)
		{
			LogString(GENERAL, "PlayerUpdateManager: Player update time has passed");
			bIsPlayerUpdateTime = true;
			updateTime = (time_t *)m_playerUpdateTimes.front();
			m_playerUpdateTimes.pop_front();
			delete updateTime;
			updateTime = nullptr;
		}

		// All the dates are now in the future, so stop looking.
		else
		{
			bDoneFlushing = true;
		}
	}
	m_criticalSection.unlock();

	// Now see if we exceeded the maximum time for sending a player update.
	if (!bIsPlayerUpdateTime)
	{
		time_t utcTime = CUtilities::GetCurrentTime();

		time_t span = utcTime - lastPlayerUpdateTime;
		if (span >= PlayerUpdateIntervalInSeconds)
		{
			LogString(GENERAL, "PlayerUpdateManager: Player update interval has passed");
			bIsPlayerUpdateTime = true;
		}
	}


	return bIsPlayerUpdateTime;
}

bool CPlayerUpdateManager::CheckIfPlayerBeFlushed(time_t lastPlayerUpdateTime)
{
	bool bIsPlayerFlushTime = false;
	bool bDoneFlushing = false;
	time_t currentTime = CUtilities::GetUTCTime();

	m_criticalSection.lock();

	time_t span = currentTime - lastPlayerUpdateTime;

	if (span >= PlayerUpdateFlushIntervalInSeconds)
	{
		LogString(GENERAL, "PlayerUpdateManager: Player update can be flushed");
		bIsPlayerFlushTime = true;

		// Remove any passed time
		while (m_playerUpdateTimes.size() > 0 && !bDoneFlushing)
		{
			time_t *updateTime = (time_t *)m_playerUpdateTimes.front();

			if (*updateTime <= currentTime)
			{
				LogString(GENERAL, "PlayerUpdateManager: Player update time has passed");
				updateTime = (time_t *)m_playerUpdateTimes.front();
				m_playerUpdateTimes.pop_front();
				delete updateTime;
				updateTime = nullptr;
			}
			else
			{
				bDoneFlushing = true;
			}
		}
	}
	m_criticalSection.unlock();
	return bIsPlayerFlushTime;
}

void CPlayerUpdateManager::ClearPlayerUpdateTimes()
{
	lock_guard<recursive_mutex> lock(m_criticalSection);
	while (m_playerUpdateTimes.size() > 0)
	{
		LogString(GENERAL, "PlayerUpdateManager::ClearPlayerUpdateTimes");
		time_t* updateTime((time_t*)m_playerUpdateTimes.front());
		delete updateTime;
		m_playerUpdateTimes.pop_front();
	}
}
