#include "stdafx.h"

#include "ProgressiveLevelManager.h"
#include "ProcessDump/WebDiagnostics.h"
#include "Logging/LogString.h"

using namespace std;
using namespace rapidjson;

const string ELIGIBLE_LEVEL_SUBSCRIPTION_TOPIC("EligibleLevel");

ProgressiveLevelManager::ProgressiveLevelManager() : 
CDataSubscriptionProvider(ELIGIBLE_LEVEL_SUBSCRIPTION_TOPIC),
m_lastForwardedTickCounts(PriorityForwardWindowMaxUpdates, 0)
{
	m_levelsItr = m_levels.begin();
}


//Copy constructor
//This calls the base class copy constructor and then sets the 
//internal member variable(s)
ProgressiveLevelManager::ProgressiveLevelManager(const ProgressiveLevelManager& rhs) : CDataSubscriptionProvider(ELIGIBLE_LEVEL_SUBSCRIPTION_TOPIC)
{
	CopyObj(rhs);
}

//Assignment operator
//This calls the base class assignment operator constructor and then sets the 
//internal member variable(s)
ProgressiveLevelManager& ProgressiveLevelManager::operator=(const ProgressiveLevelManager &rhs)
{
	if( this != &rhs)
	{
		CopyObj(rhs);
	}
	return *this;
}

//CDataSubscriptionProvider not initialised in copy onstructor??
void ProgressiveLevelManager::CopyObj(const ProgressiveLevelManager &other)
{
	std::lock_guard<recursive_mutex> lock(other.m_criticalSection);
	AddTopic(ELIGIBLE_LEVEL_SUBSCRIPTION_TOPIC);
	m_receivedTime			  = other.m_receivedTime;
	m_updatedTickCount		  = other.m_updatedTickCount;
	m_forwardedTickCount	  = other.m_forwardedTickCount;
	m_progressiveGroupId	  = other.m_progressiveGroupId;
	m_useAftLockOnTransfers	  = other.m_useAftLockOnTransfers;
	m_queryPokerCards		  = other.m_queryPokerCards;
	m_isUpdated				  = other.m_isUpdated;
	m_needPriorityForward	  = other.m_needPriorityForward;
	m_eligibilityReceivedTime = other.m_eligibilityReceivedTime;
	m_eligibilitySessionId	  = other.m_eligibilitySessionId;
	m_eligibilityDetermined	  = other.m_eligibilityDetermined;
	m_sessionActive			  = other.m_sessionActive;
	m_sessionId				  = other.m_sessionId;
	m_multicastActive		  = other.m_multicastActive;
	m_lastForwardedTickCounts = other.m_lastForwardedTickCounts;
	m_downCount				  = other.m_downCount;
	m_lastLevelIdSubscriptionProvided = other.m_lastLevelIdSubscriptionProvided;

	for (const auto& otherLevel : other.m_levels)
	{
		auto level = make_shared<CProgressiveLevel>(*otherLevel.second);
		m_levels[otherLevel.second->m_id] = level;
		for (auto& sasId : level->m_sasLevels)
		{
			m_levelsBySAS[sasId] = level;
		}
	}

	if (!m_levels.empty())
	{
		m_levelsItr = m_levels.begin();
	}
}

void ProgressiveLevelManager::AddLevel(shared_ptr<CProgressiveLevel>& level)
{
	std::lock_guard<recursive_mutex> lock(m_criticalSection);
	m_levels[level->m_id] = level;

	for (auto& sasId : level->m_sasLevels)
	{
		m_levelsBySAS[sasId] = level;
	}
}

int64_t ProgressiveLevelManager::GetLevelIdFromSASLevel(int sasLevelId) const
{
	std::lock_guard<recursive_mutex> lock(m_criticalSection);

	auto it = m_levelsBySAS.find((uint8_t) sasLevelId);
	return it != m_levelsBySAS.end() ? it->second->GetID() : InvalidProgressiveLevelId;
}

void ProgressiveLevelManager::UpdateLevels(const ProgressiveLevelsInfo* pLevelsInfo)
{
	if (pLevelsInfo != nullptr)
	{
		std::lock_guard<recursive_mutex> lock(m_criticalSection);

		m_receivedTime = pLevelsInfo->m_receivedTime;
		m_updatedTickCount = pLevelsInfo->m_receivedTickCount;
		m_progressiveGroupId = pLevelsInfo->m_progressiveGroupId;
		m_useAftLockOnTransfers = pLevelsInfo->m_useAftLockOnTransfers;
		m_queryPokerCards = pLevelsInfo->QueryPokerCards();

		m_levelsBySAS.clear();

		// First clear out the levels that are no longer valid
		for (auto it = m_levels.begin(); it != m_levels.end();)
		{
			if (pLevelsInfo->m_levels.find(it->first) == pLevelsInfo->m_levels.end())
			{
				LogString(PROGRESSIVE, _T("ProgressiveLevelManager: Removing level %lld"), it->second->GetID());
				it = m_levels.erase(it);

				// reset m_levels iterator
				m_levelsItr = m_levels.begin();
			}
			else
			{
				++it;
			}
		}

		for (auto& levelInfo : pLevelsInfo->m_levels)
		{	
			auto levelIt = m_levels.find(levelInfo.second.get()->m_id);
			if (levelIt == m_levels.end())
			{
				auto level = make_shared<CProgressiveLevel>(*levelInfo.second, m_updatedTickCount);
				AddLevel(level);
				LogString(PROGRESSIVE, _T("ProgressiveLevelManager: Added level %lld: %s"), level->GetID(), level->m_name.c_str());
			}
			else
			{
				levelIt->second->Update(*levelInfo.second.get(), pLevelsInfo->m_receivedTickCount);
				LogString(PROGRESSIVE, _T("ProgressiveLevelManager: Updated level %lld: %s"), levelIt->second->GetID(), levelIt->second->m_name.c_str());

				for (auto& sasId : levelIt->second->m_sasLevels)
				{
					m_levelsBySAS[sasId] = levelIt->second;
				}
			}
		}
	}
}

bool ProgressiveLevelManager::IsWinnerPickValid(int64_t levelId) const
{
	bool success = false;

	std::lock_guard<recursive_mutex> lock(m_criticalSection);
	auto levelIt = m_levels.find(levelId);
	if (levelIt != m_levels.end())
	{
		success = levelIt->second->IsEligible();
	}

	return success;
}

void ProgressiveLevelManager::SetProgressiveDown(bool progressiveDown)
{
	std::lock_guard<recursive_mutex> lock(m_criticalSection);

	m_downCount.Set(progressiveDown);
}

void ProgressiveLevelManager::SetSessionActiveInfo(bool sessionActive, int sessionId)
{
	std::lock_guard<recursive_mutex> lock(m_criticalSection);

	m_sessionActive = sessionActive;
	m_sessionId = sessionId;
}

bool ProgressiveLevelManager::GetMulticastActive() const
{
	return m_multicastActive;
}

bool ProgressiveLevelManager::GetEligibilityDetermined() const
{
	return m_eligibilityDetermined;
}

int ProgressiveLevelManager::GetProgressiveGroupId() const
{
	return m_progressiveGroupId;
}

bool ProgressiveLevelManager::UseAftLockOnTransfers() const
{
	return m_useAftLockOnTransfers;
}

void ProgressiveLevelManager::GetLogStrings(bool isNew, vector<std::string>& logStrings) const
{
	std::lock_guard<recursive_mutex> lock(m_criticalSection);

	logStrings.clear();

	std::stringstream str;
	str<< (isNew ? "New " : "Current ") << "ProgressiveLevelConfiguration groupId=" << m_progressiveGroupId<<" useAftLock="<<CWebDiagnostics::GetBoolString(m_useAftLockOnTransfers)<<" numLevels="<<m_levels.size();

	logStrings.push_back(str.str());

	for (auto& level : m_levels)
	{
		logStrings.push_back(level.second->GetLogString(nullptr));
	}
}

void ProgressiveLevelManager::Log(bool isNew) const
{
	std::lock_guard<recursive_mutex> lock(m_criticalSection);

	vector<std::string> logStrings;
	GetLogStrings(isNew, logStrings);

	for (auto& logString : logStrings)
	{
		LogStringNoFormat(PROGRESSIVE, logString);
	}
}

bool ProgressiveLevelManager::ProcessLevelBroadcast(const CProgressiveLevelBroadcast* pBroadcast)
{
	bool needPriorityForward(false);
	if (pBroadcast != nullptr)
	{
		std::lock_guard<recursive_mutex> lock(m_criticalSection);

		for (auto& broadcastLevel : pBroadcast->m_levels)
		{
			auto levelIt = m_levels.find(broadcastLevel.id);
			if (levelIt != m_levels.end())
			{
				m_isUpdated = true;
				m_updatedTickCount = pBroadcast->m_receivedTickCount;
				if (levelIt->second->ProcessLevelBroadcast(broadcastLevel, pBroadcast->m_receivedTickCount))
				{
					needPriorityForward = true;
					m_needPriorityForward = true;
				}
			}
		}
	}

	return needPriorityForward;
}

void ProgressiveLevelManager::ProcessBonusingAvailable(const CProgressiveBonusingAvailable* bonusingAvailable, CProgressiveEligibleLevelsInfo *eligibleLevelsInfo)
{
	std::lock_guard<recursive_mutex> lock(m_criticalSection);
	std::vector<int64_t> levelsThatExpired;
	if (bonusingAvailable != nullptr)
	{
		m_eligibilityReceivedTime = bonusingAvailable->GetReceivedTime();
		m_eligibilityDetermined = bonusingAvailable->IsEligibilityDetermined();

		// See if the session ID has changed.
		if (m_eligibilitySessionId != bonusingAvailable->GetSessionId())
		{
			LogString(PROGRESSIVE, _T("ProgressiveLevelManager: Processing new session ID. Was %d. Now is %d"), m_eligibilitySessionId, bonusingAvailable->GetSessionId());

			m_eligibilitySessionId = bonusingAvailable->GetSessionId();

			// Remove the eligible level objects from all of the levels.
			for (const auto& level : m_levels)
			{
				level.second->ClearEligibleLevelInfo();
			}
		}
	}

	if (eligibleLevelsInfo != nullptr)
	{
		bool levelChanged = false;

		// See if a level has changed
		for (const auto& level : m_levels)
		{
			ProgressiveEligibleLevelInfo *eligibleLevel = eligibleLevelsInfo->FindEligibleLevel(level.second->m_id);
			if (eligibleLevel)
			{
				if (level.second->m_eligibleInfo)
				{
					eligibleLevel->SetScatterStartTime(level.second->m_eligibleInfo->GetScatterStartTime());
					eligibleLevel->SetScatterEndTime(level.second->m_eligibleInfo->GetScatterEndTime());
				}
				if (level.second->ProcessEligibleLevelInfo(eligibleLevel))
				{
					LogStringNoFormat(PROGRESSIVE.c_str(), level.second->GetLogString(_T("ProgressiveLevelManager: Eligible info added or changed for: ")).c_str());
					levelChanged = true;
				}
			}
			else
			{
				LogString(PROGRESSIVE, "Do not show levelId=%lld because it is not in the bonusing available.", level.first);
				level.second->DoNotShowOdometer();
			}

		}

		// Reset m_levels iterator
		m_levelsItr = m_levels.begin();

		if (levelChanged)
		{
			m_eligibleLevelsJSONString = eligibleLevelsInfo->GetEligibleLevelsJSONString();
		}
	}
}

void ProgressiveLevelManager::SetForwardedTickCount(DWORD tickCount)
{
	std::lock_guard<recursive_mutex> lock(m_criticalSection);

	m_forwardedTickCount = tickCount;

	m_lastForwardedTickCounts.pop_back();
	m_lastForwardedTickCounts.insert(m_lastForwardedTickCounts.begin(), tickCount);
}

bool ProgressiveLevelManager::OkToSendPriorityForward(DWORD tickCount) const
{
	return m_needPriorityForward && 
		(tickCount - m_lastForwardedTickCounts.back()) >= PriorityForwardWindowTimeout;
}

// no parameter checking is done for pProgressiveLevelForward since caller will always pass something valid

void ProgressiveLevelManager::GetLevelForward(
	DWORD tickCount,
	CProgressiveLevelForward* pProgressiveLevelForward,
	const SYSTEMTIME* pLocalTime)
{
	std::lock_guard<recursive_mutex> lock(m_criticalSection);

	bool multicastActive = true;

	for (auto& level : m_levels)
	{
		if (!level.second->ProcessForward(tickCount, pProgressiveLevelForward, pLocalTime))
		{
			if (ProgressiveLevelType::Standard == level.second->GetType() || ProgressiveLevelType::Bonus == level.second->GetType())
			{
				multicastActive = false;
			}
		}
	}

	m_multicastActive = multicastActive;
}

CProgressiveLevelForward* ProgressiveLevelManager::GetLevelForward(
	DWORD tickCount,
	const SYSTEMTIME* pLocalTime)
{
	std::lock_guard<recursive_mutex> lock(m_criticalSection);

	CProgressiveLevelForward* pProgressiveLevelForward = nullptr;

	DWORD interval = tickCount - m_forwardedTickCount;

	if ((interval >= CProgressiveLevelForward::StrobeInterval) ||
		(m_isUpdated && interval >= CProgressiveLevelForward::MinInterval) ||
		OkToSendPriorityForward(tickCount))
	{
		m_isUpdated = false;
		SetForwardedTickCount(tickCount);

		pProgressiveLevelForward = new CProgressiveLevelForward(m_progressiveGroupId, m_useAftLockOnTransfers);
		GetLevelForward(tickCount, pProgressiveLevelForward, pLocalTime);

		m_needPriorityForward = false;
	}

	return pProgressiveLevelForward;
}

CProgressiveLevelForward* ProgressiveLevelManager::GetLevelForward(
	DWORD tickCount,
	bool  progressiveProcessingIsDown,
	bool& multicastActiveChanged,
	const SYSTEMTIME* pLocalTime)
{
	std::lock_guard<recursive_mutex> lock(m_criticalSection);

	bool oldMulticastActive = m_multicastActive;
	std::stringstream sStream;
	CProgressiveLevelForward* levelForward = GetLevelForward(tickCount, pLocalTime);
	multicastActiveChanged = oldMulticastActive != m_multicastActive;

	if (levelForward != nullptr)
	{
		// Check if bonus levels should be removed from the return value.
		if (m_eligibilitySessionId != m_sessionId || progressiveProcessingIsDown)
		{
			// Remove all bonus levels.
			bool removedLevels(false);
			sStream << "ProgressiveCounters " << "m_eligibilitySessionId=" << m_eligibilitySessionId << " m_sessionId=" << m_sessionId << " progressiveProcessingIsDown=" << progressiveProcessingIsDown << " MISMATCH/DOWN:";

			for (auto it = levelForward->cbegin(); it != levelForward->cend();)
			{
				if (ProgressiveLevelType::Bonus == it->type)
				{
					removedLevels = true;
					sStream << endl << it->levelName << " amount=" << (long long)it->amount;
					it = levelForward->EraseLevel(it);
				}
				else
				{
					++it;
				}
			}

			if (removedLevels)
			{
				string levelString =  sStream.str();
				LogStringNoFormat(PROGRESSIVE,levelString.c_str());
			}
		}
	}

	return levelForward;
}

int ProgressiveLevelManager::GetEligibilitySessionId() const
{
	return m_eligibilitySessionId;
}

bool ProgressiveLevelManager::HasLevelType(ProgressiveLevelType::Type type) const
{
	std::lock_guard<recursive_mutex> lock(m_criticalSection);

	bool retVal = false;
	for (auto& level : m_levels)
	{
		if (type == level.second->GetType())
		{
			retVal = true;
			break;
		}
	}
	return retVal;
}

bool ProgressiveLevelManager::QueryPokerCards() const
{
	return m_queryPokerCards;
}

string ProgressiveLevelManager::GetSubscriptionDataResponse(const std::string& topic, const std::string& whereId)
{
    UNREFERENCED_PARAMETER(&topic);
    UNREFERENCED_PARAMETER(&whereId);

	std::lock_guard<recursive_mutex> lock(m_criticalSection);

	string subscriptionDataResponse;

	if (ELIGIBLE_LEVEL_SUBSCRIPTION_TOPIC == topic)
	{
		auto level = m_levels.find(m_bonusCounterState.GetCurrentLevelId());
		if (level != m_levels.end())
		{
			//We have this check because we don't want to keeping sending
			//an update every 1 second for a scatter bonus level
			if (m_lastLevelIdSubscriptionProvided != m_bonusCounterState.GetCurrentLevelId())
			{
				level->second->ResetScatterDuration();
			}
			m_lastLevelIdSubscriptionProvided = m_bonusCounterState.GetCurrentLevelId();
			const std::string dynamicContentPath(CMediaPackageManager::Instance().GetDynamicContentPath(
				level->second->GetID(), "", SlotsContentType::Odometer, pKeyLcdContent));
			subscriptionDataResponse = level->second->GetSubscriptionOdometerData(dynamicContentPath);
		}
		else
		{//Go to logo
			JsonWriteStreamString writeStream;
			JsonWriter writer(writeStream);
			writer.StartObject();
			writer.Key(pKeyBonusLevelType);	writer.String("");
			writer.EndObject();
			subscriptionDataResponse = writeStream.m_str;
		}
	}

	return subscriptionDataResponse;
}

CInternalMessage *ProgressiveLevelManager::GetNextProgressiveLevelDisplayItem()
{
	CInternalMessage *msg = nullptr;
	std::lock_guard<recursive_mutex> lock(m_criticalSection);
	if (!m_levels.empty())
	{


		if (m_levelsItr == m_levels.end())
		{
			m_levelsItr = m_levels.begin();
		}

		// Cycle through levels until finding a message to be sent or reach end of levels
		for (std::map<int64_t, std::shared_ptr<CProgressiveLevel>>::iterator it = m_levelsItr; it != m_levels.end() && msg == nullptr; it++)
		{
			const shared_ptr<CProgressiveLevel> pLvl = it->second;

			if (pLvl->IsPte())
			{

				// Check anticipation
				msg = CheckAnticipation(pLvl);
				if (msg == nullptr)
				{
					// check ending anticipation
					msg = CheckEndCountdown(pLvl);
				}

				if (msg == nullptr)
				{
					// check pte result
					msg = CheckPteResult(pLvl);
				}

				if (msg == nullptr)
				{
					// check level up
					msg = CheckPteLevelUp(pLvl);
				}

				if (msg == nullptr)
				{
					// check leaderboard
					msg = CheckPteLeaderboard(pLvl);
				}

				if (msg != nullptr)
				{
					m_levelsItr = it;
				}
			}
		}

	}

	return msg;
}

CInternalMessage *ProgressiveLevelManager::CheckAnticipation(const shared_ptr<CProgressiveLevel> level)
{

	CInternalMessage *msg = nullptr;
	level->CheckDisplayAnticipation(msg);
	return msg;
}

CInternalMessage *ProgressiveLevelManager::CheckEndCountdown(const shared_ptr<CProgressiveLevel> level)
{

	CInternalMessage *msg = nullptr;
	level->CheckDisplayEndCountdown(msg);
	return msg;
}

CInternalMessage *ProgressiveLevelManager::CheckPteResult(const shared_ptr<CProgressiveLevel> level)
{

	CInternalMessage *msg = nullptr;
	level->CheckDisplayPteResult(msg);
	return msg;
}

CInternalMessage *ProgressiveLevelManager::CheckPteLevelUp(const shared_ptr<CProgressiveLevel> level)
{
	CInternalMessage *msg = nullptr;
	level->CheckDisplayPteLevelUp(msg);
	return msg;
}

CInternalMessage *ProgressiveLevelManager::CheckPteLeaderboard(const shared_ptr<CProgressiveLevel> level)
{
	CInternalMessage *msg = nullptr;
	level->CheckDisplayPteLeaderboard(msg);
	return msg;
}

CInternalMessage *ProgressiveLevelManager::GetPteAwardDisplayItem(CProgressiveWinnerPicked *winner)
{
	CInternalMessage *msg = nullptr;
	std::lock_guard<recursive_mutex> lock(m_criticalSection);

	if (winner != nullptr)
	{
		// Check level to see if it is a PTE
		auto progLevel(m_levels.find(winner->GetLevelId()));
		if (progLevel != m_levels.end() && progLevel->second->IsPte())
		{
			// PTE Send message to UI
			msg = new CInternalMessage(INT_MSG_DISPLAY_PTE_AWARD, new CProgressiveWinnerPicked(*winner));
			msg->SetData((DWORD)winner->GetLevelId());
		}
	}

	return msg;
}

void ProgressiveLevelManager::LeaderboardManuallyInitiated(int64_t levelId)
{
	std::lock_guard<recursive_mutex> lock(m_criticalSection);

	auto levelIt = m_levels.find(levelId);
	if (levelIt != m_levels.end())
	{
		levelIt->second->LeaderboardManuallyInitiated();
	}
}

string ProgressiveLevelManager::GetPrizeDescription(int64_t levelId)
{
	std::lock_guard<recursive_mutex> lock(m_criticalSection);

	string prizeDescription;

	auto levelIt = m_levels.find(levelId);
	if (levelIt != m_levels.end())
	{
		prizeDescription = levelIt->second->GetPrizeDesription();
	}

	return prizeDescription;
}


void ProgressiveLevelManager::ProgressiveDownClearLevels()
{
	std::lock_guard<recursive_mutex> lock(m_criticalSection);

	LogString(PROGRESSIVE, "Clearing progressive levels");
	m_levels.clear();
	m_levelsItr = m_levels.end();
}

void ProgressiveLevelManager::CheckTimedEvents(const time_t& timer)
{
	std::lock_guard<recursive_mutex> lock(m_criticalSection);
	m_bonusCounterState.CheckTimedEvents(timer);
	CMediaPackageManager::Instance().TimedEvents();
}

void ProgressiveLevelManager::UpdateLevelCounters(CProgressiveLevelForward* pLevels)
{
	std::lock_guard<recursive_mutex> lock(m_criticalSection);
	std::string eligibleString("ProgressiveCounters ELIGIBLE:");
	std::string ineligibleString(" -- INELIGIBLE:");
	std::string notShownString(" -- NOT-SHOWN:");

	std::stringstream sStreameligible;
	std::stringstream ineligiblesStream;
	std::stringstream notShownsStream;

	sStreameligible << eligibleString;
	ineligiblesStream << ineligibleString;
	notShownsStream << notShownString;

	// Remove non-bonusing levels.
	for (auto it = pLevels->cbegin(); it != pLevels->cend();)
	{
		if (it->type != ProgressiveLevelType::Bonus || !it->eligible || !it->showOdometer)
		{
			if (ProgressiveLevelType::Bonus == it->type)
			{
				if (!it->eligible)
				{
					ineligiblesStream << " \"" << it->levelName << "\"=" << (long long)it->amount;
				}
				else
				{
					notShownsStream << " \"" << it->levelName << "\"=" << (long long)it->amount;
				}
			}

			// The level is not good enough. Remove it.
			it = pLevels->EraseLevel(it);
		}
		else
		{
			sStreameligible << " \"" << it->levelName << "\"=" << (long long)it->amount;
			++it;
		}
	}

	m_bonusCounterState.UpdateLevelCounters(pLevels);
}

void ProgressiveLevelManager::DeleteCardedBonusCounterItems()
{
	std::lock_guard<recursive_mutex> lock(m_criticalSection);
	m_bonusCounterState.DeleteCardedBonusCounterItems();
}

void ProgressiveLevelManager::SetUIConfig(const CUIConfig& uiConfig)
{
	std::lock_guard<recursive_mutex> lock(m_criticalSection);
	m_bonusCounterState.SetUIConfig(uiConfig);
}

void ProgressiveLevelManager::RemoveLevel(int64_t levelId)
{
	std::lock_guard<recursive_mutex> lock(m_criticalSection);
	auto iter = m_levels.find(levelId);;
	if (iter != m_levels.end())
	{
		LogString(PROGRESSIVE, _T("Remove expired level=%lld"), levelId);
		m_levels.erase(iter);
		m_levelsItr = m_levels.begin();
	}
}