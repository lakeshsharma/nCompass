#include "stdafx.h"
#include "BonusCounterState.h"
#include "ScreenDefs.h"
#include "ProgressiveBonusCounterItem.h"
#include "NotificationBoxCounterItem.h"
#include "ScatterBonusCounterItem.h"
#include "PrizeBonusCounterItem.h"
#include "PlayXGetYBonusCounterItem.h"
#include "DisplayManager.h"
#ifdef LOGGING
#include "Logging/LogString.h"
#endif

using namespace std;
using namespace BonusCounterState;

template <typename T> bool PComp(T * const & a, T * const & b)
{
	return *a < *b;
}

const WORD LOGO_PRIORITY = 1000;

CBonusCounterState::CBonusCounterState(IOperatingSystem &operatingSystem) :
m_operatingSystem(operatingSystem),
m_logoMediaItem(&operatingSystem, LOGO_PRIORITY),
m_currentCounterItem(nullptr)
{
	if (CompactPlatform == CDisplayManager::Instance().GetDisplayPlatform())
	{
		AddNotificationBoxBonusCounterItem();
	}
}


CBonusCounterState::~CBonusCounterState()
{
	for_each(m_counterItemsAll.begin(), m_counterItemsAll.end(), std::default_delete<CBonusCounterItem>());
	m_counterItemsAll.clear();
}

/// <summary>
/// Checks the timed events.
/// </summary>
/// <param name="timer">The timer.</param>
void CBonusCounterState::CheckTimedEvents(const time_t& timer)
{
	lock_guard<recursive_mutex> lock(m_criticalSection);

	if (nullptr != m_currentCounterItem && !m_currentCounterItem->CheckTimedEvent(timer))
	{
		LogString(UI_LOG, _T("BonusCounterState: Counter item display time expired"));
		CycleDisplay();
	}
	else if (m_currentCounterItem == nullptr)
	{
		CycleDisplay();
	}
}

void CBonusCounterState::AddNotificationBoxBonusCounterItem()
{
    CBonusCounterItem* bonusCounterItem(nullptr);

	bonusCounterItem = new CNotificationBoxCounterItem(&m_operatingSystem, m_uiConfig.GetBonusCounterNotificationPriority());
	
	// Add the count item to our list
	if (nullptr != bonusCounterItem)
	{
		m_counterItemsAll.push_back(bonusCounterItem);
		m_counterItemsAll.sort(PComp<CBonusCounterItem>);
		m_currentCounterItem = m_counterItemsAll.front();
		LogString(UI_LOG, _T("BonusCounterState: Added new counter item (ID:%lld, type: %d, priority: %d) for a total of %d."),
			bonusCounterItem->GetID(), (int)bonusCounterItem->GetBonusCounterType(), bonusCounterItem->GetPriorityLevel(), (int)m_counterItemsAll.size());
	}
}

void CBonusCounterState::AddOrUpdateBonusCounterItem(const CBonusCounterItem& itemToAdd)
{
	lock_guard<recursive_mutex> lock(m_criticalSection);

	CBonusCounterItem* bonusCounterItem(nullptr);
	const BonusCounterType counterType = itemToAdd.GetBonusCounterType();

	//Update existing entry
	if (GetBonusCounterItem(itemToAdd, bonusCounterItem))
	{
		if (BonusCounterType::ProgressiveOdometer == counterType)
		{
			CProgressiveBonusCounterItem* bonusCounterItemToUpdate = (CProgressiveBonusCounterItem*)&itemToAdd;
			*((CProgressiveBonusCounterItem*)bonusCounterItem) = *bonusCounterItemToUpdate;
		}
		else if (BonusCounterType::PlayXGetY == counterType)
		{
			CPlayXGetYBonusCounterItem* bonusCounterItemToUpdate = (CPlayXGetYBonusCounterItem*)&itemToAdd;
			*((CPlayXGetYBonusCounterItem*)bonusCounterItem) = *bonusCounterItemToUpdate;
		}
		else if (BonusCounterType::ScatterTimer == counterType)
		{
			CScatterBonusCounterItem* bonusCounterItemToUpdate = (CScatterBonusCounterItem*)&itemToAdd;
			*((CScatterBonusCounterItem*)bonusCounterItem) = *bonusCounterItemToUpdate;
		}
		else if (BonusCounterType::ProgressivePrize == counterType)
		{
			CPrizeBonusCounterItem* bonusCounterItemToUpdate = (CPrizeBonusCounterItem*)&itemToAdd;
			*((CPrizeBonusCounterItem*)bonusCounterItem) = *bonusCounterItemToUpdate;
		}
	}
	else//Add new entry
	{
		if (BonusCounterType::ProgressiveOdometer == counterType)
		{
			bonusCounterItem = new CProgressiveBonusCounterItem(*(CProgressiveBonusCounterItem*)&itemToAdd);
		}
		else if (BonusCounterType::PlayXGetY == counterType)
		{
			bonusCounterItem = new CPlayXGetYBonusCounterItem(*(CPlayXGetYBonusCounterItem*)&itemToAdd);
		}
		else if (BonusCounterType::ScatterTimer == counterType)
		{
			bonusCounterItem = new CScatterBonusCounterItem(*(CScatterBonusCounterItem*)&itemToAdd);
		}		
		else if (BonusCounterType::ProgressivePrize == counterType)
		{
			bonusCounterItem = new CPrizeBonusCounterItem(*(CPrizeBonusCounterItem*)&itemToAdd);
		}

		// Add the count item to our list
		if (nullptr != bonusCounterItem)
		{
			m_counterItemsAll.push_back(bonusCounterItem);
			m_counterItemsAll.sort(PComp<CBonusCounterItem>);

			LogString(UI_LOG, _T("BonusCounterState: Added new counter item (ID:%lld, type: %d, priority: %d) for a total of %d."),
				bonusCounterItem->GetID(), (int)bonusCounterItem->GetBonusCounterType(), bonusCounterItem->GetPriorityLevel(), (int)m_counterItemsAll.size());
		}
	}

	// See if we need to display or update the display for this counter item
	if (nullptr != bonusCounterItem && bonusCounterItem->DisplayOnAddOrUpdate(m_currentCounterItem))
	{
		m_currentCounterItem = bonusCounterItem;
	}
}

bool CBonusCounterState::GetBonusCounterItem(const CBonusCounterItem& itemToGet, CBonusCounterItem*& item)
{
	bool ok = false;
	for (std::list<CBonusCounterItem*>::iterator it = m_counterItemsAll.begin(); it != m_counterItemsAll.end(); ++it)
	{
		if (*(*it) == itemToGet)// Found the item?
		{
			item = *it;
			ok = true;
			break;
		}
	}
	return ok;
}

void CBonusCounterState::CycleDisplay()
{
	lock_guard<recursive_mutex> lock(m_criticalSection);

	bool itemFound = false;

	int currentPriority = 0;
	int prevPriority = 0;

	long long currentId = -1;
	long long prevId = -1;

	CBonusCounterItem* nextCounterItem(nullptr);

	if (m_counterItemsAll.empty()) // if the list is not empty, then cycle the first priority.
	{
		nextCounterItem = &m_logoMediaItem;
	}
	else
	{
		prevPriority = m_counterItemsAll.front()->GetPriorityLevel();

		for (std::list<CBonusCounterItem*>::iterator it = m_counterItemsAll.begin(); it != m_counterItemsAll.end(); ++it)
		{
			currentPriority = (*it)->GetPriorityLevel();
			currentId = (*it)->GetID();

			if (currentPriority != prevPriority) // The priority changed before the id is found.
			{
				itemFound = false;
				break;
			}
			// the previous id is found, so set the new id
			else if (nullptr != m_currentCounterItem && prevId == m_currentCounterItem->GetID())
			{
				nextCounterItem = *it;
				itemFound = true;
				break;
			}

			prevId = currentId;
			prevPriority = currentPriority;
		}

		//The id was not found before the priority changed so set the id to the first.  
		//Edge Case: Only or Last item of the first priority
		if (!itemFound)
		{
			nextCounterItem = m_counterItemsAll.front();
		}
	}

	if (nullptr != nextCounterItem)
	{
		LogString(UI_LOG, _T("BonusCounterState: Displaying new counter item. ID:%lld, type:%d, priority: %d."),
			nextCounterItem->GetID(), (int)nextCounterItem->GetBonusCounterType(), nextCounterItem->GetPriorityLevel());
		m_currentCounterItem = nextCounterItem;
	}

	m_currentCounterItem->ResetDisplayStartTime();
}

void CBonusCounterState::SetCurrentCounterItemToEndDisplay()
{
	lock_guard<recursive_mutex> lock(m_criticalSection);
	if (m_currentCounterItem != nullptr)
	{
		m_currentCounterItem->SetItemToDisplay(false);
		CheckTimedEvents(CUtilities::GetCurrentTime());
	}
}

void CBonusCounterState::DeleteCardedBonusCounterItems()
{
	lock_guard<recursive_mutex> lock(m_criticalSection);

	auto iter = m_counterItemsAll.begin();
	while (iter != m_counterItemsAll.end())
	{
		if (BonusCounterType::nRichLevel == (*iter)->GetBonusCounterType() ||
			BonusCounterType::SplashdownCountdownTimer == (*iter)->GetBonusCounterType())
		{
			if (nullptr != m_currentCounterItem)
			{
				if (*m_currentCounterItem == *(*iter))
				{
					m_currentCounterItem = nullptr;
				}
			}
			delete *iter;
			iter = m_counterItemsAll.erase(iter);
		}
		else
		{
			++iter;
		}
	}

	CycleDisplay();
}

void CBonusCounterState::ResetCurrentCounterItemToInvalid()
{
	lock_guard<recursive_mutex> lock(m_criticalSection);

	m_currentCounterItem = nullptr;

	CheckTimedEvents(CUtilities::GetCurrentTime());
}

void CBonusCounterState::UpdateLevelCounters(CProgressiveLevelForward* pLevels)
{
	lock_guard<recursive_mutex> lock(m_criticalSection);

	if (pLevels != nullptr)
	{
		bool removingActiveCounter = false;

		// Iterate thru all the forward levels supplied in the message from internals
		for (auto it = pLevels->cbegin(); it != pLevels->cend(); ++it)
		{
			if (it->isScatter)
			{
				AddOrUpdateBonusCounterItem(CScatterBonusCounterItem(&m_operatingSystem,
					it->levelId, it->levelName, (time_t)it->m_countdownEndTime, m_uiConfig.GetBonusCounterScatterCountdownTimerPriority()));
			}
			else if (it->isPlayXGetY)
			{
				AddOrUpdateBonusCounterItem(CPlayXGetYBonusCounterItem(&m_operatingSystem, it->levelId, m_uiConfig.GetBonusCounterPlayXGetYPriority()));
			}
			else if (it->isPrize)
			{
				AddOrUpdateBonusCounterItem(CPrizeBonusCounterItem(&m_operatingSystem, it->levelId, m_uiConfig.GetBonusCounterPrizePriority()));
			}			
			else// This will add or update the progressive bonus counter item for this forward item
			{
				AddOrUpdateBonusCounterItem(CProgressiveBonusCounterItem(&m_operatingSystem,
					it->levelId, it->levelName, it->amount, m_uiConfig.GetBonusCounterProgressiveOdometerPriority()));
			}
		}

		//Delete expired bonus counters
		auto bonusCounterItemIter = m_counterItemsAll.begin();
		while (bonusCounterItemIter != m_counterItemsAll.end())
		{
			bool found = false;
			for (auto progressiveLevelCounterIter = pLevels->cbegin();
				progressiveLevelCounterIter != pLevels->cend();
				++progressiveLevelCounterIter)
			{
				if ((*bonusCounterItemIter)->GetID() == progressiveLevelCounterIter->levelId && (*bonusCounterItemIter)->IsLevelCounter())
				{
					found = true;
					break;
				}
			}

			if (!found && (*bonusCounterItemIter)->IsLevelCounter())
			{				
				if (nullptr != m_currentCounterItem &&
					*m_currentCounterItem == *(*bonusCounterItemIter))
				{
					removingActiveCounter = true;
					m_currentCounterItem = nullptr;
				}
				delete *bonusCounterItemIter;
				bonusCounterItemIter = m_counterItemsAll.erase(bonusCounterItemIter);
                long unsigned int NoOfCounterItem = m_counterItemsAll.size();
				LogString(UI_LOG, _T("BonusCounterState: Removed expired the counter item since not in level forward list. Number of bounus counters is now %lu."), NoOfCounterItem);
			}
			else
			{
				bonusCounterItemIter++;
			}
		}

		if (removingActiveCounter)
		{
			CycleDisplay();
		}
	}
}

void CBonusCounterState::SetUIConfig(const CUIConfig &uiConfig)
{
	lock_guard<recursive_mutex> lock(m_criticalSection);
	m_uiConfig = uiConfig;	
	for (CBonusCounterItem* bonusCounterItem : m_counterItemsAll)
	{
		WORD priority(0);
		switch (bonusCounterItem->GetBonusCounterType())
		{
		case BonusCounterType::nRichLevel:
			priority = m_uiConfig.GetBonusCounternRichLevelPriority();
			break;
		case BonusCounterType::SplashdownCountdownTimer:
			priority = m_uiConfig.GetBonusCounterSplashdownPriority();
			break;
		case BonusCounterType::ProgressiveOdometer:
			priority = m_uiConfig.GetBonusCounterProgressiveOdometerPriority();
			break;
		case BonusCounterType::PlayXGetY:
			priority = m_uiConfig.GetBonusCounterPlayXGetYPriority();
			break;
		case BonusCounterType::ScatterTimer:
			priority = m_uiConfig.GetBonusCounterScatterCountdownTimerPriority();
			break;
		case BonusCounterType::ProgressivePrize:
			priority = m_uiConfig.GetBonusCounterPrizePriority();
			break;
		case BonusCounterType::NotificationBox:
			priority = m_uiConfig.GetBonusCounterNotificationPriority();
			break;
		default:
			break;
		};
		LogString(UI_LOG, _T("SetUIConfig: counter type=%d oldPriority=%d newPriority=%d"), 
			(int)bonusCounterItem->GetBonusCounterType(), bonusCounterItem->GetPriorityLevel(), priority);
		bonusCounterItem->SetPriorityLevel(priority);
	}
}

int64_t CBonusCounterState::GetCurrentLevelId()
{
	lock_guard<recursive_mutex> lock(m_criticalSection);
	int64_t levelId(CBonusCounterItem::INVALID_VALUE);
	
	if (m_currentCounterItem)
	{
		levelId = m_currentCounterItem->GetID();
	}

	return levelId;
}