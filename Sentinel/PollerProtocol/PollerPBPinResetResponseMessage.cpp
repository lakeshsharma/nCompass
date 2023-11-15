#include "stdafx.h"
#include "PollerPBPinResetResponseMessage.h"

CPollerPBPinResetResponseMessage::CPollerPBPinResetResponseMessage(const byte *response)
{
	int offset = 0;

	//length
	byte pinResetResult;
	memcpy(&pinResetResult, response + offset, sizeof(pinResetResult));
	offset+= sizeof(pinResetResult);

	LogString (POLLER_LOG, "%s %d: pinResetResult %u", __FUNCTION__, __LINE__, pinResetResult);
	m_pinResetResult = static_cast<PinResetResult>(pinResetResult);
}

PinResetResult CPollerPBPinResetResponseMessage::GetPinResetResult(void)
{
	return m_pinResetResult;
}


