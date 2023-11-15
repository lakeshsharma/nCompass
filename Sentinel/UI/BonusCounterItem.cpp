#include "stdafx.h"
#include "BonusCounterItem.h"

using namespace std;

CBonusCounterItem::CBonusCounterItem(IOperatingSystem* operatingSystem, BonusCounterType bonusCounterType, long long itemId, WORD priorityLevel, DWORD displayTime) :
	m_bonusCounterType(bonusCounterType),
	m_id(itemId),
	m_operatingSystem(operatingSystem),
	m_displayTime(displayTime),	
	m_startTickCount(0),
	m_priorityLevel(priorityLevel),
	m_okToDisplay(false)
{
}

CBonusCounterItem::~CBonusCounterItem()
{
}

CBonusCounterItem & CBonusCounterItem::operator = (const CBonusCounterItem &rhs)
{
	m_bonusCounterType = rhs.GetBonusCounterType();
	m_id = rhs.GetID();
	SetPriorityLevel(rhs.GetPriorityLevel());

	// NOTE: We do not want to copy the state items.

	return *this;
}

bool CBonusCounterItem::operator == (const CBonusCounterItem &rhs)
{
	return m_id == rhs.m_id && m_bonusCounterType == rhs.m_bonusCounterType;
}

bool CBonusCounterItem::operator < (const CBonusCounterItem &second)
{
	return m_priorityLevel < second.m_priorityLevel;
}

long long CBonusCounterItem::GetID() const
{
	return m_id;
}

WORD CBonusCounterItem::GetPriorityLevel() const
{
	return m_priorityLevel;
}

void CBonusCounterItem::SetPriorityLevel(WORD priorityLevel)
{
	m_priorityLevel = priorityLevel;
}

BonusCounterType CBonusCounterItem::GetBonusCounterType() const
{
	return m_bonusCounterType;
}

string CBonusCounterItem::GetParameters() const
{
	return "";
}

bool CBonusCounterItem::CheckTimedEvent(time_t timer)
{
    UNREFERENCED_PARAMETER(&timer); 

	if (m_okToDisplay)
	{
		DWORD deltaTickCount(m_operatingSystem->GetTickCount32() - m_startTickCount);

		if (deltaTickCount >= m_displayTime)
		{
			m_okToDisplay = false;
		}
	}

	return m_okToDisplay;
}

void CBonusCounterItem::ResetDisplayStartTime()
{
	m_okToDisplay = true;
	m_startTickCount = m_operatingSystem->GetTickCount32();
}

void CBonusCounterItem::SetItemToDisplay(bool display)
{
	m_okToDisplay = display;
}

bool CBonusCounterItem::DisplayOnAddOrUpdate(CBonusCounterItem *currentlyDisplayedItem)
{
	UNREFERENCED_PARAMETER(&currentlyDisplayedItem);

	return false;
}

bool CBonusCounterItem::IsLevelCounter() const
{
	return m_bonusCounterType == BonusCounterType::ProgressiveOdometer || 
		   m_bonusCounterType == BonusCounterType::PTE ||
		   m_bonusCounterType == BonusCounterType::PlayXGetY ||		
		   m_bonusCounterType == BonusCounterType::ScatterTimer ||
		   m_bonusCounterType == BonusCounterType::ProgressivePrize;
}
