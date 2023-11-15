#include "stdafx.h"
#include "SASGeneralPoll.h"

CSASGeneralPoll::CSASGeneralPoll(byte gameAddress)
: CSASPollMessage(gameAddress)
{
	// allocate buffers
	SetSendBuffer((BYTE *)new byte[1],1);
	SetResponseBuffer((BYTE *)new byte[1],1);

	ResetExceptionValue();
}

CSASGeneralPoll::~CSASGeneralPoll(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

bool CSASGeneralPoll::IsGeneralPoll()
{
	return true;
}

BYTE CSASGeneralPoll::GetPollValue()
{
	return POLL_VALUE;
}

bool CSASGeneralPoll::SwapPollValue()
{
	// what do we do here?
	return 0;
}

void CSASGeneralPoll::Fill()
{
	// fill long poll command data
	m_sendBuffer[0] = m_gameAddress | 0x80;

	return;
}

void CSASGeneralPoll::ResponseReceived(void)
{
	if (m_nReturnValue == SASPOLL_SUCCESS)
		m_exceptionValue = (GeneralPollException)m_responseBuffer[0];
	else
		ResetExceptionValue();

	CSASPollMessage::ResponseReceived();
}

