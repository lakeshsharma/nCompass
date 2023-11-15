#include "stdafx.h"
#include "Utilities.h"
#include "PrizeBonusCounterItem.h"
#include "Utilities.h"

CPrizeBonusCounterItem::CPrizeBonusCounterItem(IOperatingSystem* operatingSystem, long long itemId, WORD priorityLevel) :
	CBonusCounterItem(operatingSystem, BonusCounterType::ProgressivePrize, itemId, priorityLevel, DEFAULT_PRIZE_DISPLAY_TIME)
{
}

CPrizeBonusCounterItem::CPrizeBonusCounterItem(const CPrizeBonusCounterItem &rhs) : CBonusCounterItem( rhs )
{
}

CPrizeBonusCounterItem::~CPrizeBonusCounterItem()
{
}

CPrizeBonusCounterItem & CPrizeBonusCounterItem::operator = (const CPrizeBonusCounterItem &rhs)
{
	CBonusCounterItem::operator =(rhs);

	return *this;
}

bool CPrizeBonusCounterItem::DisplayOnAddOrUpdate(CBonusCounterItem *currentlyDisplayedItem)
{
	// Set to display if no item currently displayed OR this is the currently displayed item OR
	//   this item has a higher priority than the currently displayed item
	return (nullptr == currentlyDisplayedItem || *this == *currentlyDisplayedItem || *this < *currentlyDisplayedItem);
}