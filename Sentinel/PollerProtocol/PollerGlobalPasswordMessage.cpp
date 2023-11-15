#include "stdafx.h"
#include "PollerGlobalPasswordMessage.h"

CPollerGlobalPasswordMessage::CPollerGlobalPasswordMessage(void)
{
	m_bufferSize = sizeof(WORD);
	m_lpBuffer = new byte[m_bufferSize];
}

CPollerGlobalPasswordMessage::~CPollerGlobalPasswordMessage(void)
{
}

WORD CPollerGlobalPasswordMessage::GetPassword( void )
{
	WORD retVal;
	memcpy_s(&retVal, sizeof(retVal), m_lpBuffer, sizeof(retVal));
	return retVal;
}
