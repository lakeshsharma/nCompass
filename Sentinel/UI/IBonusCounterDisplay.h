#pragma once

#include "BonusCounterItem.h"

// Interface for displaying BonusCounterType Items
class IBonusCounterDisplay
{
public:
	/// <summary>
	/// Shows the bonus counter media.
	/// </summary>
	/// <param name="item">The item.</param>
	virtual void ShowBonusCounterMedia(CBonusCounterItem* item) = 0;
};
