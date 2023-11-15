#pragma once

#include "InternalMessage.h"
#include "ProgressiveKeys.h"

class ProgressiveQueueToOneLink : public CInternalMessage
{
private:
	ToOneLink::Type m_messageType;
	std::string m_messageBody;

public:
	ProgressiveQueueToOneLink(ToOneLink::Type messageType, const std::string& messageBody);
	virtual ~ProgressiveQueueToOneLink();

	ToOneLink::Type GetMessageType() const;
	const std::string& GetMessageBody() const;

private:
	ProgressiveQueueToOneLink(const ProgressiveQueueToOneLink&);
	void operator=(const ProgressiveQueueToOneLink&);
};
