#include "stdafx.h"
#include "LongPoll80Message.h"

#include "Logging/LogString.h"
#include "SASLimits.h"

CLongPoll80Message::CLongPoll80Message(byte gameAddress,
	int progressiveGroup,
	const CProgressiveLevelForward::Level &levelRecord) :
	CSASPollMessage(gameAddress),
	m_progressiveGroup(progressiveGroup),
	m_levelRecord(levelRecord)
{
	SetSendBuffer((BYTE *) new byte[POLL_SIZE], 0);
	SetResponseBuffer((BYTE *)new byte[RESPONSE_SIZE], RESPONSE_SIZE);
	ZeroMemory(GetResponseBuffer(), GetResponseBufferSize());
}

CLongPoll80Message::~CLongPoll80Message(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll80Message::GetPollValue()
{
	return POLL_VALUE;
}

bool CLongPoll80Message::FillWithTickCountBeforeSending()
{
	return true;
}

void CLongPoll80Message::Fill()
{
}

void CLongPoll80Message::ResponseReceived()
{
	// Address 0 is a broadcast, do not expect a response.
	if (m_gameAddress != 0)
	{
		if (SASPOLL_SUCCESS == m_nReturnValue)
		{
			if (m_responseBufferSize <= ACK_OFFSET)
			{
				SetReturnValue(SASPOLL_INCOMPLETE_DATA);
			}
			else if (m_responseBuffer[ACK_OFFSET] != m_gameAddress) // Is the response a valid ACK?
			{
				SetReturnValue(SASPOLL_NACK); // Not a valid ACK.
			}
		}
	}

	CSASPollMessage::ResponseReceived();
}

void CLongPoll80Message::Fill(DWORD tickCount)
{
	DWORD elapsedTickCount(tickCount - m_levelRecord.updatedTickCount);

	if (CSASLimits::IsValidSasProgressiveGroup(m_progressiveGroup) &&
		CSASLimits::IsValidSasProgressiveLevelRecord(m_levelRecord, tickCount))
	{
		m_sendBufferSize = POLL_SIZE;
		m_sendBuffer[GAME_ADDRESS_OFFSET] = m_gameAddress;
		m_sendBuffer[POLL_VALUE_OFFSET] = POLL_VALUE;

		GROUP_TYPE sasGroup = (GROUP_TYPE)m_progressiveGroup;
		memcpy(m_sendBuffer + GROUP_OFFSET, &sasGroup, sizeof(sasGroup));

		LEVEL_TYPE sasLevel = (LEVEL_TYPE)m_levelRecord.sasLevel;
		memcpy(m_sendBuffer + LEVEL_OFFSET, &sasLevel, sizeof(sasLevel));

		// If the amount is out of range, send zero as the amount to the EGM.
		Int64ToBcd(m_levelRecord.amount, m_sendBuffer + AMOUNT_OFFSET, AMOUNT_SIZE);

		GenerateSendChecksum();
	}
	else
	{
		m_sendBufferSize = 0; // Nothing to send.
	}

	LogString(SASGENERAL, _T("CLongPoll80Message progressive delay=%ums sendSize=%u group=%d level=%u amount=%lld"),
		(unsigned)elapsedTickCount,
		(unsigned)m_sendBufferSize,
		(int)m_progressiveGroup,
		(unsigned)m_levelRecord.sasLevel,
		(long long)m_levelRecord.amount);
}
