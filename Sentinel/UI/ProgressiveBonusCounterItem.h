#pragma once
#include "GlobalDefs.h"
#include "BonusCounterItem.h"

// Declare friend class(es) for unit test access.
namespace SentinelNativeUnitTests { class ProgressiveBonusCounterItemTest; }

class CProgressiveBonusCounterItem : public CBonusCounterItem
{
	// Grant "friend" access for testing.
	friend class SentinelNativeUnitTests::ProgressiveBonusCounterItemTest;

public:
	static const DWORD DEFAULT_COUNTER_DISPLAY_TIME = 7000; // 7 seconds.

	/// <summary>
	/// Initializes a new instance of the <see cref="CProgressiveBonusCounterItem"/> class.
	/// </summary>
	/// <param name="operatingSystem">The operating system.</param>
	/// <param name="itemId">The item identifier.</param>
	/// <param name="levelName">Name of the level.</param>
	/// <param name="amountInPennies">The amount in pennies.</param>
	/// <param name="priorityLevel">priority level.</param>
	CProgressiveBonusCounterItem(IOperatingSystem* operatingSystem,
		long long itemId, const std::string& levelName, long long amountInPennies, WORD priorityLevel);

	/// <summary>
	/// Initializes a new instance of the <see cref="CProgressiveBonusCounterItem"/> class.
	/// NOTE: Need to create our own copy construtor because this class requires a deep copy.
	/// </summary>
	/// <param name="rhs">The CProgressiveBonusCounterItem to copy</param>
	CProgressiveBonusCounterItem(const CProgressiveBonusCounterItem &rhs);

	/// <summary>
	/// Finalizes an instance of the <see cref="CProgressiveBonusCounterItem"/> class.
	/// </summary>
	~CProgressiveBonusCounterItem();

	/// <summary>
	/// Gets the parameters.
	/// </summary>
	/// <returns>the parameters for the module</returns>
	virtual std::string GetParameters() const;

	/// <summary>
	/// Determines if this item needs to be displayed upon add or update
	/// </summary>
	/// <param name="currentlyDisplayedItem">The currently displayed item. May be NULL</param>
	/// <returns>true if item needs to be displayed now, false otherwise</returns>
	virtual bool DisplayOnAddOrUpdate(CBonusCounterItem *currentlyDisplayedItem);

private:
	std::string m_levelName;
	long long m_amountInPennies;
};