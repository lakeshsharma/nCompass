#include "stdafx.h"
#include "Utilities.h"
#include "NotificationBoxCounterItem.h"
#include "Utilities.h"
#include "RapidJsonHelper.h"

const int NOTIFICATIONBOX_ID = 0;

CNotificationBoxCounterItem::CNotificationBoxCounterItem(IOperatingSystem* operatingSystem, WORD priorityLevel) :
	CBonusCounterItem(operatingSystem, BonusCounterType::NotificationBox, NOTIFICATIONBOX_ID, priorityLevel)
{
}

CNotificationBoxCounterItem::CNotificationBoxCounterItem(const CNotificationBoxCounterItem &rhs) :
	CBonusCounterItem( rhs )
{
}

CNotificationBoxCounterItem::~CNotificationBoxCounterItem()
{
}

CNotificationBoxCounterItem & CNotificationBoxCounterItem::operator = (const CNotificationBoxCounterItem &rhs)
{
	CBonusCounterItem::operator =(rhs);

	return *this;
}

