#pragma once

#include "BonusCounterItem.h"
#include "LogoCounterItem.h"
#include "GlobalDefs.h"
#include "ProgressiveProtocol/ProgressiveLevelForward.h"
#include "InternalMessage.h"
#include "Config/UIConfig.h"

namespace BonusCounterState
{
	/// <summary>
	/// Management class to orgainize, cycle and provide items sorted and returned based on level.
	/// </summary>
	class CBonusCounterState
	{
	public:
		/// <summary>
		/// Default Ctor
		/// </summary>
		CBonusCounterState(IOperatingSystem &operatingSystem = COperatingSystem::m_operatingSystem);

		/// <summary>
		/// Default Dtor
		/// </summary>
		~CBonusCounterState();

		/// <summary>
		/// Add or update a bonus conter item.
		/// </summary>
		/// <param name="itemToAdd">item to add</param>
		void AddOrUpdateBonusCounterItem(const CBonusCounterItem& itemToAdd);

		/// <summary>
		/// Checks the timed events.
		/// </summary>
		/// <param name="timer">The timer.</param>
		void CheckTimedEvents(const time_t& timer);

		/// <summary>
		/// Cycles and returns the next item id.
		/// </summary>
		/// <remarks>
		/// The items added are sorted by level (smallest to largest).  The next item is only
		/// the next item of the same priority.  Only when the items of a level are removed will
		/// the next level be cycled.
		/// </remarks>
		void CycleDisplay();

		/// <summary>
		/// Deletes the carded bonus counter items.
		/// </summary>
		void DeleteCardedBonusCounterItems();

		/// <summary>
		/// Resets the current counter item to invalid.
		/// </summary>
		void ResetCurrentCounterItemToInvalid();

		/// <summary>
		/// Updates the level counters based upon ght elevel forward list.
		/// </summary>
		/// <param name="msg">The MSG that contains the level forward list.</param>
		void UpdateLevelCounters(CProgressiveLevelForward* pLevels);

		/// <summary>
		/// Sets the currently displayed counter item to no longer display.
		/// </summary>
		void SetCurrentCounterItemToEndDisplay();

		/// <summary>
		/// Adds notification box bonus counter item to counter items list.
		/// </summary>
		void AddNotificationBoxBonusCounterItem();

		/// <summary>
		/// Sets the UI configuration object.
		/// </summary>
		void SetUIConfig(const CUIConfig &uiConfig);
			    
		/// <summary>
		/// Returns current level ID
		/// </summary>
		/// <returns>current level ID</returns>
		int64_t GetCurrentLevelId();

	private:
		std::recursive_mutex m_criticalSection;

		IOperatingSystem &m_operatingSystem;
		CLogoCounterItem m_logoMediaItem;
		CBonusCounterItem* m_currentCounterItem;
		CUIConfig m_uiConfig;

		/// <summary>
		/// the collection of items which are sorted by priority and contain the item id's
		/// </summary>
		std::list<CBonusCounterItem*> m_counterItemsAll;

		bool GetBonusCounterItem(const CBonusCounterItem& itemToGet, CBonusCounterItem*& item);
	};
}
