#pragma once
#include "GlobalDefs.h"
#include "BonusCounterItem.h"

// Declare friend class(es) for unit test access.
namespace SentinelUnitTests { class BonusCounterItemUnitTests; }

class CScatterBonusCounterItem : public CBonusCounterItem
{
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="CScatterBonusCounterItem"/> class.
	/// </summary>
	/// <param name="operatingSystem">The operating system.</param>
	/// <param name="itemId">The item identifier.</param>
	/// <param name="levelName">Name of the level.</param>
	/// <param name="countdownTimerEnd">The countdown timer end.</param>
	/// <param name="priorityLevel">priority level.</param>
	CScatterBonusCounterItem(IOperatingSystem* operatingSystem,
		long long itemId, const std::string& levelName, time_t countdownTimerEnd, WORD priorityLevel);

	/// <summary>
	/// Finalizes an instance of the <see cref="CScatterBonusCounterItem"/> class.
	/// </summary>
	~CScatterBonusCounterItem();

	/// <summary>
	/// The copy constructor for bonus counter item.
	/// </summary>
	/// <param name="rhs">The bonus counter object to copy from.</param>
	/// <returns>reference to CScatterBonusCounterItem object</returns>
	CScatterBonusCounterItem(const CScatterBonusCounterItem &rhs);

	/// <summary>
	/// The assignment operator for bonus counter item.
	/// </summary>
	/// <param name="rhs">The bonus counter object to assign from.</param>
	/// <returns>reference to CScatterBonusCounterItem object</returns>
	virtual CScatterBonusCounterItem & operator = (const CScatterBonusCounterItem &rhs);

	/// <summary>
	/// Gets the parameters.
	/// </summary>
	virtual std::string GetParameters() const;

	/// <summary>
	/// Determines if this item needs upon add or update
	/// </summary>
	/// <param name="currentlyDisplayedItem">The currently displayed item. May be NULL</param>
	/// <returns>true if item needs to be displayed now, false otherwise</returns>
	virtual bool DisplayOnAddOrUpdate(CBonusCounterItem *currentlyDisplayedItem);

	/// <summary>
	/// Checks the timed event.
	/// Note: This will also update the splashdown timer in the counter.
	/// </summary>
	/// <param name="timer">The timer.</param>
	/// <returns>true item is okay to be displayed. false if set to not display or the display time expired</returns>
	virtual bool CheckTimedEvent(time_t timer);

	/// <summary>
	/// Checks for splashdown countdown.
	/// </summary>
	/// <param name="timer">The timer.</param>
	/// <returns>true if Splashdown Countdown needs to be updated</returns>
	bool  CheckForSplashdownCountdown(time_t timer);

private:
	std::string m_levelName;
	time_t m_timer;
	time_t m_countdownTimerEnd;
	int m_remainingDuration;

	// Grant "friend" access for testing.
	friend class SentinelUnitTests::BonusCounterItemUnitTests;
	FRIEND_TEST(BonusCounterItemUnitTests, ScatterBonusCounterItemTest);
};