#pragma once

#include "GlobalDefs.h"
#include "BonusCounterItem.h"

// Declare friend class(es) for unit test access.
namespace SentinelNativeUnitTests { class BonusCounterItemUnitTests; }

class CPrizeBonusCounterItem : public CBonusCounterItem
{
	// Grant "friend" access for testing.
	friend class SentinelNativeUnitTests::BonusCounterItemUnitTests;

public:
	static const DWORD DEFAULT_PRIZE_DISPLAY_TIME = 7000; // 7 seconds.

	/// <summary>
	/// Initializes a new instance of the <see cref="CPrizeBonusCounterItem"/> class.
	/// </summary>
	/// <param name="operatingSystem">The operating system.</param>
	/// <param name="itemId">The item identifier.</param>
	/// <param name="priorityLevel">priority level.</param>
	CPrizeBonusCounterItem(IOperatingSystem* operatingSystem, long long itemId, WORD priorityLevel);

	/// <summary>
	/// Initializes a new instance of the <see cref="CPrizeBonusCounterItem"/> class.
	/// NOTE: Need to create our own copy construtor because this class requires a deep copy.
	/// </summary>
	/// <param name="rhs">The CPrizeBonusCounterItem to copy</param>
	CPrizeBonusCounterItem(const CPrizeBonusCounterItem &rhs);

	/// <summary>
	/// Finalizes an instance of the <see cref="CPrizeBonusCounterItem"/> class.
	/// </summary>
	~CPrizeBonusCounterItem();

	/// <summary>
	/// The assignment operator for bonus counter item.
	/// </summary>
	/// <param name="rhs">The bonus counter object to assign from.</param>
	/// <returns>reference to CPrizeBonusCounterItem object</returns>
	virtual CPrizeBonusCounterItem & operator = (const CPrizeBonusCounterItem &rhs);

	/// <summary>
	/// Determines if this item needs to be displayed upon add or update
	/// </summary>
	/// <param name="currentlyDisplayedItem">The currently displayed item. May be NULL</param>
	/// <returns>true if item needs to be displayed now, false otherwise</returns>
	virtual bool DisplayOnAddOrUpdate(CBonusCounterItem *currentlyDisplayedItem);

private:
};
