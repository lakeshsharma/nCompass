#pragma once
#include "PollerMessage.h"

class CPollerMobileConnectResponseMessage :
	public CPollerMessage
{
public:
	CPollerMobileConnectResponseMessage(byte *msgJson);
	~CPollerMobileConnectResponseMessage(void);
    string& GetJson( void );

private:
    string m_jsonResponse;
};