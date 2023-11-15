#pragma once

#include "GlobalDefs.h"
#include "BonusCounterItem.h"

// Declare friend class(es) for unit test access.
namespace SentinelNativeUnitTests { class BonusCounterItemUnitTests; }

class CLogoCounterItem : public CBonusCounterItem
{
	// Grant "friend" access for testing.
	friend class SentinelNativeUnitTests::BonusCounterItemUnitTests;

public:
	/// <summary>
	/// Initializes a new instance of the <see cref="CLogoCounterItem"/> class.
	/// </summary>
	/// <param name="operatingSystem">The operating system.</param>
	/// <param name="priorityLevel">priority level.</param>
	CLogoCounterItem(IOperatingSystem* operatingSystem, WORD priorityLevel);

	/// <summary>
	/// Initializes a new instance of the <see cref="CLogoCounterItem"/> class.
	/// NOTE: Need to create our own copy construtor because this class requires a deep copy.
	/// </summary>
	/// <param name="rhs">The CLogoCounterItem to copy</param>
	CLogoCounterItem(const CLogoCounterItem &rhs);


	/// <summary>
	/// Finalizes an instance of the <see cref="CLogoCounterItem"/> class.
	/// </summary>
	~CLogoCounterItem();

	/// <summary>
	/// The assignment operator for bonus counter item.
	/// </summary>
	/// <param name="rhs">The bonus counter object to assign from.</param>
	/// <returns>reference to CLogoCounterItem object</returns>
	virtual CLogoCounterItem & operator = (const CLogoCounterItem &rhs);

	/// <summary>
	/// Checks the timed event.
	/// </summary>
	/// <param name="timer">The timer.</param>
	/// <returns>This will always return true for logo counter item</returns>
	virtual bool CheckTimedEvent(time_t timer);

private:
};
