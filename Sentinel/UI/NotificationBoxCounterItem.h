#pragma once

#include "GlobalDefs.h"
#include "BonusCounterItem.h"

using namespace std;

class CNotificationBoxCounterItem : public CBonusCounterItem
{

public:
	/// <summary>
	/// Initializes a new instance of the <see cref="CLogoCounterItem"/> class.
	/// </summary>
	/// <param name="operatingSystem">The operating system.</param>
	/// <param name="priorityLevel">priority level.</param>
	CNotificationBoxCounterItem(IOperatingSystem* operatingSystem, WORD priorityLevel);

	/// <summary>
	/// Initializes a new instance of the <see cref="CLogoCounterItem"/> class.
	/// NOTE: Need to create our own copy construtor because this class requires a deep copy.
	/// </summary>
	/// <param name="rhs">The CLogoCounterItem to copy</param>
	CNotificationBoxCounterItem(const CNotificationBoxCounterItem &rhs);


	/// <summary>
	/// Finalizes an instance of the <see cref="CNotificationBoxCounterItem"/> class.
	/// </summary>
	~CNotificationBoxCounterItem();

	/// <summary>
	/// The assignment operator for bonus counter item.
	/// </summary>
	/// <param name="rhs">The bonus counter object to assign from.</param>
	/// <returns>reference to CLogoCounterItem object</returns>
	virtual CNotificationBoxCounterItem & operator = (const CNotificationBoxCounterItem &rhs);

private:

};
