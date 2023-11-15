#include "stdafx.h"
#include "PollerMobileConnectResponseMessage.h"
#include "../Utilities.h"
#include "Logging/LogString.h"

CPollerMobileConnectResponseMessage::CPollerMobileConnectResponseMessage( byte *msgJson )
{
	// parse response, fill in m_jsonResponse
	WORD jsonLength = 0;
	memcpy_s( &jsonLength, sizeof( jsonLength ), msgJson, sizeof( jsonLength ) );

	if (jsonLength > MAX_MOBILE_CONNECT_MESSAGE_SIZE)
	{
		LogString (POLLER_LOG, "PollerMobileConnectResponseMessage: JSON message length exceeds maximum size (%d), truncated!",
			jsonLength);

		jsonLength = MAX_MOBILE_CONNECT_MESSAGE_SIZE;
	}
	msgJson += sizeof(jsonLength);
	std::string jsonData((const char*)msgJson, jsonLength);
	m_jsonResponse = jsonData;

	// To remove this log later
#ifdef _DEBUG
	LogString (POLLER_LOG, "PollerMobileConnectResponseMessage received '%s', size=%d", m_jsonResponse.c_str(), jsonLength);
#endif
}

CPollerMobileConnectResponseMessage::~CPollerMobileConnectResponseMessage( void )
{
}

string& CPollerMobileConnectResponseMessage::GetJson( void )
{
	return m_jsonResponse;
}
