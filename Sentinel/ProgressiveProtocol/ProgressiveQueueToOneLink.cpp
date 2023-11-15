#include "stdafx.h"
#include "ProgressiveQueueToOneLink.h"

using namespace std;

ProgressiveQueueToOneLink::ProgressiveQueueToOneLink(ToOneLink::Type messageType, const string& messageBody)
	: CInternalMessage(INT_DMSG_PROGRESSIVE_QUEUE_TO_ONELINK)
	, m_messageType(messageType)
	, m_messageBody(messageBody)
{
}

ProgressiveQueueToOneLink::~ProgressiveQueueToOneLink()
{
}

ToOneLink::Type ProgressiveQueueToOneLink::GetMessageType() const
{
	return m_messageType;
}

const string& ProgressiveQueueToOneLink::GetMessageBody() const
{
	return m_messageBody;
}

