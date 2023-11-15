#pragma once
#include "PollerMessage.h"

class CPollerDisplayMessage :
	public CPollerMessage
{
public:
	CPollerDisplayMessage(int DataLength);
public:
	~CPollerDisplayMessage(void);

	std::string GetDisplayString( DisplayMessageType &displayMsgType );

};
