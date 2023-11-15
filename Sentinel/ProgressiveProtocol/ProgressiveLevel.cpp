#include "stdafx.h"
#include "ProgressiveLevel.h"
#include "Logging/LogString.h"
#include "Util/Convert.h"
#include "ProcessDump/WebDiagnostics.h"
#include "MediaPackageManager.h"

using namespace std;

const int ANTICIPATION_TIME = 10; // Need lead time to get a full 5 seconds anticipation display.

// Initialized partly in header declarations, partly by member constructors, partly in Update() and partly here.
CProgressiveLevel::CProgressiveLevel(ProgressiveLevelInfo& levelInfo, DWORD updatedTickCount)
	: m_id(levelInfo.m_id)
	, m_pteState(levelInfo.m_id)
	, m_eligibleInfo(nullptr)
{
	Update(levelInfo, updatedTickCount);
}

CProgressiveLevel::CProgressiveLevel(const CProgressiveLevel& other)
	: m_id(other.m_id)
	, m_value(other.m_value)
	, m_type(other.m_type)
	, m_updatedTickCount(other.m_updatedTickCount)
	, m_name(other.m_name)
	, m_url(other.m_url)
	, m_sasLevels(other.m_sasLevels)
	, m_state(other.m_state)
	, m_staleCount(other.m_staleCount)
	, m_pteState(other.m_pteState)
	, m_eligibleInfo(other.m_eligibleInfo ? new ProgressiveEligibleLevelInfo(*other.m_eligibleInfo.get()) : nullptr)
{
}

void CProgressiveLevel::Update(ProgressiveLevelInfo& levelInfo, DWORD updatedTickCount)
{
	m_name = levelInfo.m_name;
	m_url = levelInfo.m_url;
	m_sasLevels = levelInfo.m_sasLevels;
	m_updatedTickCount = updatedTickCount;
	m_type = levelInfo.m_type;
}

ProgressiveLevelType::Type CProgressiveLevel::GetType() const
{
	return m_type;
}

__int64 CProgressiveLevel::GetID() const
{
	return m_id;
}

bool CProgressiveLevel::IsEligible() const
{
	bool isEligible = false;

	if (m_type == ProgressiveLevelType::AccountingOnly ||
		m_type == ProgressiveLevelType::Standard ||
		(m_eligibleInfo != nullptr && m_eligibleInfo->IsEligible()))
	{
		isEligible = true;
	}

	return isEligible;
}

bool CProgressiveLevel::ShowOdometer() const
{
	bool showOdometer = false;

	if (m_type == ProgressiveLevelType::Bonus && m_eligibleInfo)
	{
		showOdometer = m_eligibleInfo->ShowOdometer();
		if (IsScatter())
		{
			showOdometer = showOdometer && m_eligibleInfo->DisplayScatterLevel();
		}
		else if(IsPlayXGetY())
		{
			showOdometer = showOdometer && m_eligibleInfo->DisplayPlayXGetYLevel();
		}

		if (showOdometer)
		{
			LogString(PROGRESSIVE, "Determined to show odometer for level %s", m_name.c_str());
		}
	}

	return showOdometer;
}

void CProgressiveLevel::DoNotShowOdometer()
{
	if (m_eligibleInfo)
	{
		m_eligibleInfo->DoNotShowOdometer();
	}
}

bool CProgressiveLevel::ProcessLevelBroadcast(const CProgressiveLevelBroadcast::Level& broadcastLevel, DWORD receivedTickCount)
{
	bool needPriorityForward(false);
	std::string pNewName = broadcastLevel.name == m_name ? "" : broadcastLevel.name.c_str();

	if (broadcastLevel.m_countdownEndTime != INVALID_VALUE && broadcastLevel.m_countdownStartTime != INVALID_VALUE)
	{
		needPriorityForward = true;
		if (m_eligibleInfo && m_eligibleInfo->IsScatter())
		{
			time_t currentTime(CUtilities::GetCurrentTime());
			if (currentTime > broadcastLevel.m_countdownStartTime && currentTime < broadcastLevel.m_countdownEndTime)
			{
				m_eligibleInfo->SetScatterStartTime(broadcastLevel.m_countdownStartTime);
				m_eligibleInfo->SetScatterEndTime(broadcastLevel.m_countdownEndTime);
			}
			else
			{
				m_eligibleInfo->SetScatterStartTime(INVALID_VALUE);
				m_eligibleInfo->SetScatterEndTime(INVALID_VALUE);
			}
		}
	}

	if (m_state == Configured)
	{
		m_value = broadcastLevel.value;
		std::string wLogStr = GetLogString("Level Broadcast (first): ", pNewName.c_str());
		LogStringNoFormat(PROGRESSIVE, wLogStr.c_str());
	}
	else
	{
		const __int64* pNewValue = broadcastLevel.value == m_value ? nullptr : &broadcastLevel.value;
		if (!pNewName.empty() || pNewValue != nullptr || m_state == Stale)
		{
			LogStringNoFormat(PROGRESSIVE, GetLogString("Level Broadcast: ", pNewName.c_str(), pNewValue, NoLongerStale).c_str());
			if (pNewValue != nullptr)
			{
				if (ProgressiveLevelType::Standard == m_type && m_value > broadcastLevel.value)
				{
					needPriorityForward = true; // Standard progressive value went backwards. Need to update EGM quickly.
				}

				m_value = broadcastLevel.value;
			}
		}
	}

	m_state = Updated;

	if (!pNewName.empty())
	{
		m_name = broadcastLevel.name;
	}

	m_updatedTickCount = receivedTickCount;

	return needPriorityForward;
};

bool CProgressiveLevel::ProcessEligibleLevelInfo(const ProgressiveEligibleLevelInfo* eligibleLevelInfo)
{
	bool eligibleInfoChanged = false;
	if (m_type != ProgressiveLevelType::Standard && m_id == eligibleLevelInfo->GetLevelId())
	{
		if (m_eligibleInfo == nullptr)
		{
			m_eligibleInfo.reset(new ProgressiveEligibleLevelInfo(eligibleLevelInfo->GetLevelId()));
		}

		eligibleInfoChanged = m_eligibleInfo->Update(eligibleLevelInfo);
	}

	return eligibleInfoChanged;
}

void CProgressiveLevel::ClearEligibleLevelInfo()
{
	LogStringNoFormat(PROGRESSIVE, GetLogString("Clearing eligible info for: "));

	m_eligibleInfo.reset();
	ProgressivePteState newState(m_pteState.GetLevelId());
	m_pteState = newState;
}

std::string CProgressiveLevel::GetEligibleLevelJSONString()
{
	std::string elibleLevelString;

	if (m_eligibleInfo != nullptr)
	{
		elibleLevelString = m_eligibleInfo->GetJSonString();
	}

	return elibleLevelString;
}


std::string CProgressiveLevel::GetLogString(LPCSTR pLogEvent, LPCSTR pNewName, const __int64* pNewValue, StaleState staleState) const
{
    std::stringstream SStream;
	std::string str;
	if (pLogEvent != nullptr)
		SStream << pLogEvent;


	SStream << "id=" << m_id << " name=\"" << m_name.c_str() << "\"";

	if (pNewName != nullptr)
		SStream << "->\"" << pNewName << "\"";

	SStream << " value=" << m_value;
	if (pNewValue != nullptr)
		SStream << "->" << *pNewValue;

	SStream << " type=" << (int)m_type;
	SStream << " eligible=" << (int)IsEligible();
	SStream << " show=" << (int)ShowOdometer();
	SStream << " url=\"" << m_url.c_str() << "\"";
	SStream << " sasIds=";

	if (!m_sasLevels.empty())
	{
		SStream << (int)m_sasLevels[0];
		for (size_t i = 1; i < m_sasLevels.size(); i++)
			SStream << "," << (int)m_sasLevels[i];
	}

	if (m_state == Stale)
	{
		LPCSTR pStaleStateStr;
		switch (staleState)
		{
		case GoneStale:
			pStaleStateStr = "GoneStale";
			break;

		case NoLongerStale:
			pStaleStateStr = "NoLongerStale";
			break;

		default:
			pStaleStateStr = "IsStale";
			break;
		}

		SStream << " " << pStaleStateStr << " - Last Updated TickCount=" << m_updatedTickCount;
	}

	if (m_eligibleInfo != nullptr)
	{
		SStream << " ";
		SStream << m_eligibleInfo->GetLogString();
	}

	return SStream.str();
}


bool CProgressiveLevel::ProcessForward(DWORD tickCount, CProgressiveLevelForward* pProgressiveLevelForward, const SYSTEMTIME* pLocalTime)
{
	bool multicastActive(false);

	DWORD elapsedUpdated = tickCount - m_updatedTickCount;
	if (m_state == Stale || elapsedUpdated > CProgressiveLevelForward::MaxInterval)
	{
		if (m_state != Stale)
		{
			m_state = Stale;
			LogStringNoFormat(PROGRESSIVE, this->GetLogString("Cannot forward stale: ", nullptr, nullptr, GoneStale));
		}
		else
		{
			LogStringNoFormat(PROGRESSIVE, GetLogString("Cannot forward stale: ", nullptr, nullptr, IsStale));
		}
	}
	else
	{
		multicastActive = true; // Allow up to CProgressiveLevelForward::MaxInterval for m_state == Configured.
		if (m_state != Configured && pProgressiveLevelForward != nullptr)
		{
			if (ProgressiveLevelType::Bonus == m_type)
			{
				m_state = Forwarded;

				bool isPte(false), isPrize(false), isPlayXGetY(false), isScatter(false);
				string prizeDescription;
				int64_t countdownStartTime(INVALID_VALUE);
				int64_t countdownEndTime(INVALID_VALUE);
				if (m_eligibleInfo)
				{
					isPte = m_eligibleInfo->IsPte();
					isPrize = m_eligibleInfo->GetAwardType() == AwardType::Prize || m_eligibleInfo->GetAwardType() == AwardType::Offer;
					isPlayXGetY = m_eligibleInfo->IsPlayXGetY();
                    isScatter = m_eligibleInfo->IsScatter();
					prizeDescription = m_eligibleInfo->GetPrizeDescription();
					countdownStartTime = m_eligibleInfo->GetScatterStartTime();
					countdownEndTime = m_eligibleInfo->GetScatterEndTime();
				}
				pProgressiveLevelForward->AddLevel(m_id, m_name.c_str(), IsEligible(), ShowOdometer(), isPte, isPrize, isPlayXGetY, isScatter, prizeDescription.c_str(), countdownStartTime, countdownEndTime, m_value, m_type, m_updatedTickCount);
			}
			else
			{
				for (auto& sasLevel : m_sasLevels)
				{
					m_state = Forwarded;
					pProgressiveLevelForward->AddLevel(sasLevel, m_value, m_type, m_updatedTickCount);
				}
			}
		}
	}

	m_staleCount.Set(m_state == Stale, pLocalTime);

	return multicastActive;
}

void CProgressiveLevel::UpdatePteState(const ProgressivePteState& pteState)
{
	m_pteState = pteState;
}

ProgressivePteState CProgressiveLevel::GetPteState()
{
	return m_pteState;
}

bool CProgressiveLevel::CheckDisplayAnticipation(CInternalMessage *&intMsg)
{
	bool shouldDisplay = false;
	intMsg = nullptr;
	if (m_eligibleInfo != nullptr)
	{
		if (m_eligibleInfo->GetPteStatus() == PteStatus::StartCountdown
			&& m_eligibleInfo->IsPlayerQualified())
		{
			if (!m_pteState.wasStartingDisplayed)
			{
				shouldDisplay = true;
			}
			else if (!m_pteState.was5secondDisplayed)
			{
				time_t currentTime = CUtilities::GetCurrentTime();
				int64_t eventStart = m_eligibleInfo->GetStartTimeUtc() / MILLISECONDS_PER_SECOND; // Actually is local time

				if (currentTime > eventStart - ANTICIPATION_TIME)
				{
					LogStringNoFormat(PROGRESSIVE, GetLogString("Should display PTE 10 seconds Anticipation for: "));
					shouldDisplay = true;
					m_pteState.was5secondDisplayed = true;
				}
			}
			
			if (shouldDisplay)
			{
				LogStringNoFormat(PROGRESSIVE, GetLogString("Should display PTE Anticipation for: "));
	
				shouldDisplay = true;
				m_pteState.wasStartingDisplayed = true;
	
				intMsg = new CInternalMessage(INT_MSG_DISPLAY_PTE_ANTICIPATION);
				intMsg->SetData((DWORD)m_id);
			}
		}
	}

	return shouldDisplay;
}

bool CProgressiveLevel::CheckDisplayEndCountdown(CInternalMessage *&intMsg)
{
	bool shouldDisplay = false;
	intMsg = nullptr;
	if (m_eligibleInfo != nullptr)
	{
		if (!m_pteState.wasEndingDisplayed && m_eligibleInfo->GetPteStatus() == PteStatus::EndCountdown)
		{
			LogStringNoFormat(PROGRESSIVE, GetLogString("Should display PTE EndCountdown for: "));

			shouldDisplay = true;
			m_pteState.wasEndingDisplayed = true;

			intMsg = new CInternalMessage(INT_MSG_DISPLAY_PTE_ENDCOUNTDOWN);
			intMsg->SetData((DWORD)m_id);
		}
	}

	return shouldDisplay;
}

bool CProgressiveLevel::CheckDisplayPteResult(CInternalMessage *&intMsg)
{
	bool shouldDisplay = false;
	intMsg = nullptr;
	if (m_eligibleInfo != nullptr)
	{
		if (!m_pteState.wasEligible && m_eligibleInfo->IsEligible())
		{
			m_pteState.wasEligible = true;
		}

		if (m_pteState.wasEligible && !m_pteState.wereResultsDisplayed && m_eligibleInfo->GetPteStatus() == PteStatus::Results)
		{
			LogStringNoFormat(PROGRESSIVE, GetLogString("Should display PTE Results for: "));

			shouldDisplay = true;
			m_pteState.wereResultsDisplayed = true;

			intMsg = new CInternalMessage(INT_MSG_DISPLAY_PTE_RESULTS);
			intMsg->SetData((DWORD)m_id);
		}
	}

	return shouldDisplay;
}

bool CProgressiveLevel::CheckDisplayPteLevelUp(CInternalMessage *&intMsg)
{
	bool shouldDisplay = false;
	intMsg = nullptr;
	if (m_eligibleInfo != nullptr)
	{
		// Look for eligible minor level less than pte_state current sequence number.
		ProgressivePteState pteState = m_pteState;
		m_eligibleInfo->ForEachMinor([&pteState, &shouldDisplay](const ProgressiveEligibleMinorInfo& minor)->bool
		{
			bool bContinue = true;
			if (minor.IsEligible() && (minor.GetSequence() < pteState.curSequence || pteState.curSequence == 0))
			{
				bContinue = false;

				// update sequence number in ptestate
				pteState.curSequence = minor.GetSequence();
				shouldDisplay = true;
			}

			return bContinue;
		});


		if (shouldDisplay)
		{
			LogStringNoFormat(PROGRESSIVE, GetLogString("Should display PTE EndCountdown for level-up for: "));
			LogString(PROGRESSIVE, "Current sequence %d. New sequence %d", m_pteState.curSequence, pteState.curSequence);

			m_pteState = pteState;

			// Only display message if this isn't the first time through
			if (m_pteState.initialized)
			{
				intMsg = new CInternalMessage(INT_MSG_DISPLAY_PTE_LEVELUP);
				intMsg->SetData((DWORD)m_id);
			}
			else
			{
				LogStringNoFormat(PROGRESSIVE, GetLogString("No PTE level-up for first level: "));
			}
		}

		m_pteState.initialized = true;
	}

	return shouldDisplay;
}


bool CProgressiveLevel::CheckDisplayPteLeaderboard(CInternalMessage *&intMsg)
{
	bool shouldDisplay = false;
	intMsg = nullptr;
	if (m_eligibleInfo != nullptr)
	{
		if (!m_pteState.isLeaderboardDisplayed && m_eligibleInfo->IsEligible() && m_eligibleInfo->ShowInfo() && !m_pteState.wasLeaderboardManuallyInitiated)
		{
			LogStringNoFormat(PROGRESSIVE, GetLogString("Should display PTE Leaderboard for: "));

			shouldDisplay = true;
			m_pteState.isLeaderboardDisplayed = true;

			intMsg = new CInternalMessage(INT_MSG_DISPLAY_PTE_LEADERBOARD);
			intMsg->SetData((DWORD)m_id);
		}
		else if (!m_eligibleInfo->ShowInfo() && m_pteState.isLeaderboardDisplayed && !m_pteState.wasLeaderboardManuallyInitiated)
		{
			LogStringNoFormat(PROGRESSIVE, GetLogString("Should close PTE Leaderboard for: "));

			// Close leaderboard
			m_pteState.isLeaderboardDisplayed = false;
			intMsg = new CInternalMessage(INT_MSG_CLOSE_PTE_LEADERBOARD);
		}
	}

	return shouldDisplay;
}

bool CProgressiveLevel::IsPte() const
{
	bool retVal = false;
	if (m_eligibleInfo != nullptr)
	{
		retVal = m_eligibleInfo->IsPte();
	}

	return retVal;
}

bool CProgressiveLevel::IsScatter() const
{
	bool retVal = false;
	if (m_eligibleInfo != nullptr)
	{
		retVal = m_eligibleInfo->IsScatter();
	}
	return retVal;
}

bool CProgressiveLevel::IsPlayXGetY() const
{
	bool retVal = false;
	if (m_eligibleInfo != nullptr)
	{
		retVal = m_eligibleInfo->IsPlayXGetY();
	}

	return retVal;
}

void CProgressiveLevel::LeaderboardManuallyInitiated()
{
	m_pteState.wasLeaderboardManuallyInitiated = true;
}

void CProgressiveLevel::ResetScatterDuration()
{
	if (m_eligibleInfo != nullptr)
	{
		m_eligibleInfo->ResetScatterDuration();
	}
}

std::string CProgressiveLevel::GetPrizeDesription()
{
	string retVal;

	if (m_eligibleInfo != nullptr)
	{
		retVal = m_eligibleInfo->GetPrizeDescription();
	}

	return retVal;
}

string CProgressiveLevel::GetSubscriptionOdometerData(const std::string &dynamicContentPath) const
{
	string retVal;

	if (m_eligibleInfo != nullptr)
	{
		retVal = m_eligibleInfo->GetSubscriptionOdometerData(dynamicContentPath);
	}

	return retVal;
}