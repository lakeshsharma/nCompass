#pragma once

#include "InternalMessage.h"
#include "ProgressiveProtocol/EGMAwardMessageSource.h"

/// <summary>
/// A Message target pair to use for passing multiple messages around.
/// </summary>
struct MsgTargetPair
{
	MsgTargetPair(MessageQueueId target = mqNone, CInternalMessage *msg = nullptr)
		: msg(msg)
		, target(target)
	{
	}

	CInternalMessage *msg;
	MessageQueueId target;
};
