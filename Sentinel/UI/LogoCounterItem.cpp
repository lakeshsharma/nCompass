#include "stdafx.h"
#include "Utilities.h"
#include "LogoCounterItem.h"
#include "Utilities.h"

const int LOGO_ID = -1;

CLogoCounterItem::CLogoCounterItem(IOperatingSystem* operatingSystem, WORD priorityLevel) :
	CBonusCounterItem(operatingSystem, BonusCounterType::Logo, LOGO_ID, priorityLevel, 0)
{
}

CLogoCounterItem::CLogoCounterItem(const CLogoCounterItem &rhs) :
	CBonusCounterItem( rhs )
{
}

CLogoCounterItem::~CLogoCounterItem()
{
}

CLogoCounterItem & CLogoCounterItem::operator = (const CLogoCounterItem &rhs)
{
	CBonusCounterItem::operator =(rhs);

	return *this;
}

bool CLogoCounterItem::CheckTimedEvent(time_t timer)
{
    UNREFERENCED_PARAMETER(&timer); 

	return true;
}