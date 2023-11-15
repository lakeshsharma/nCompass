#include "stdafx.h"
#include "Utilities.h"
#include "PlayXGetYBonusCounterItem.h"
#include "Utilities.h"

CPlayXGetYBonusCounterItem::CPlayXGetYBonusCounterItem(IOperatingSystem* operatingSystem, long long itemId, WORD priorityLevel) :
	CBonusCounterItem(operatingSystem, BonusCounterType::PlayXGetY, itemId, priorityLevel, DEFAULT_PLAY_X_GET_Y_DISPLAY_TIME)
{
}

CPlayXGetYBonusCounterItem::CPlayXGetYBonusCounterItem(const CPlayXGetYBonusCounterItem &rhs) : CBonusCounterItem( rhs )
{
}

CPlayXGetYBonusCounterItem::~CPlayXGetYBonusCounterItem()
{
}

CPlayXGetYBonusCounterItem & CPlayXGetYBonusCounterItem::operator = (const CPlayXGetYBonusCounterItem &rhs)
{
	CBonusCounterItem::operator =(rhs);

	return *this;
}

bool CPlayXGetYBonusCounterItem::DisplayOnAddOrUpdate(CBonusCounterItem *currentlyDisplayedItem)
{
	// Set to display if no item currently displayed OR this item has a higher priority than the currently displayed item
	return (nullptr == currentlyDisplayedItem || *this < *currentlyDisplayedItem);
}
