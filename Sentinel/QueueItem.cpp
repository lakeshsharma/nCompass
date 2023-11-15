#include "stdafx.h"
#include "QueueItem.h"

CQueueItem::CQueueItem(void) : m_okToFlushLowerOnAdd(true)
{
	SetPriority( 0 );
}

CQueueItem::~CQueueItem(void)
{
}

void CQueueItem::SetOkToFlushLowerOnAdd(bool okToFlushLowerOnAdd)
{
	m_okToFlushLowerOnAdd = okToFlushLowerOnAdd;
}

bool CQueueItem::OkToFlushLowerOnAdd()
{
	return m_okToFlushLowerOnAdd;
}

