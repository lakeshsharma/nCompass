#include "stdafx.h"
#include "Utilities.h"
#include "ProgressiveBonusCounterItem.h"
#include "Utilities.h"

using namespace std;

CProgressiveBonusCounterItem::CProgressiveBonusCounterItem(IOperatingSystem* operatingSystem, long long itemId, const string& levelName, long long amountInPennies, WORD priorityLevel) :
	CBonusCounterItem(operatingSystem, BonusCounterType::ProgressiveOdometer, itemId, priorityLevel, DEFAULT_COUNTER_DISPLAY_TIME),
	m_levelName(levelName),
	m_amountInPennies(amountInPennies)
{
}

CProgressiveBonusCounterItem::CProgressiveBonusCounterItem(const CProgressiveBonusCounterItem &rhs) : CBonusCounterItem(rhs)
{
	m_levelName = rhs.m_levelName;
	m_amountInPennies = rhs.m_amountInPennies;
}

CProgressiveBonusCounterItem::~CProgressiveBonusCounterItem()
{

}

string CProgressiveBonusCounterItem::GetParameters() const
{
	return FormatString("timeString=\"%s\" percentage=\"%lld\"", m_levelName.c_str(), m_amountInPennies);
}

bool CProgressiveBonusCounterItem::DisplayOnAddOrUpdate(CBonusCounterItem *currentlyDisplayedItem)
{
	// Set to display if no item currently displayed OR this is the currently displayed item OR
	//   this item has a higher priority than the currently displayed item
	return (nullptr == currentlyDisplayedItem || *this == *currentlyDisplayedItem || *this < *currentlyDisplayedItem);
}
