#include "stdafx.h"
#include "LongPoll86Message.h"

#include "Logging/LogString.h"
#include "SASLimits.h"
#include "Utilities.h"

using namespace std;

CLongPoll86Message::CLongPoll86Message(byte gameAddress,
	const CProgressiveLevelForward &levelInfo) :
	CSASPollMessage(gameAddress),
	m_levelInfo(levelInfo)
{
	SetSendBuffer((BYTE *) new byte[MAX_POLL_SIZE], 0);
	SetResponseBuffer((BYTE *)new byte[RESPONSE_SIZE], RESPONSE_SIZE);
	ZeroMemory(GetResponseBuffer(), GetResponseBufferSize());
}

CLongPoll86Message::~CLongPoll86Message(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll86Message::GetPollValue()
{
	return POLL_VALUE;
}

bool CLongPoll86Message::FillWithTickCountBeforeSending()
{
	return true;
}

void CLongPoll86Message::Fill()
{
}

void CLongPoll86Message::ResponseReceived()
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

void CLongPoll86Message::Fill(DWORD tickCount)
{
	DWORD maxDelayForSentValues(0);
	string levelAmountString; // For LogString().
	int levelCount(0);

	m_sendBuffer[GAME_ADDRESS_OFFSET] = m_gameAddress;
	m_sendBuffer[POLL_VALUE_OFFSET] = POLL_VALUE;
	BYTE *currentBuffer = m_sendBuffer + OPTIONAL_DATA_OFFSET;

	int progressiveGroup(m_levelInfo.GetProgressiveGroupId());
	if (CSASLimits::IsValidSasProgressiveGroup(progressiveGroup))
	{
		GROUP_TYPE sasGroup = (GROUP_TYPE)progressiveGroup;
		memcpy(currentBuffer, &sasGroup, sizeof(sasGroup));
		currentBuffer += sizeof(sasGroup);

		for (auto it = m_levelInfo.cbegin(); it != m_levelInfo.cend() && levelCount < MAX_LEVEL_COUNT; ++it)
		{
			if (CSASLimits::IsValidSasProgressiveLevelRecord(*it, tickCount))
			{
				LEVEL_TYPE sasLevel = (LEVEL_TYPE)it->sasLevel;
				memcpy(currentBuffer, &sasLevel, sizeof(sasLevel));
				currentBuffer += sizeof(sasLevel);

				__int64 amount(it->amount);
				// If the amount is out of range, send zero as the amount to the EGM.
				Int64ToBcd(amount, currentBuffer, AMOUNT_SIZE);
				currentBuffer += AMOUNT_SIZE;

				++levelCount;
				levelAmountString += FormatString(_T(" L%u=%lld"), (unsigned)sasLevel, (long long)amount);
				maxDelayForSentValues = max(maxDelayForSentValues, (tickCount - it->updatedTickCount));
			}
		}
	}

	if (levelCount > 0)
	{
		m_sendBufferSize = MAX_POLL_SIZE;
		SetSizeLengthAndCRC(currentBuffer + CRCSIZE - m_sendBuffer);
	}
	else
	{
		m_sendBufferSize = 0; // Nothing to send.
	}

	LogString(SASGENERAL, _T("CLongPoll86Message progressive maxDelay=%ums groupId=%d levelCount=%d/%u%s"),
		(unsigned)maxDelayForSentValues,
		(unsigned)progressiveGroup,
		(int)levelCount,
		(unsigned)m_levelInfo.GetNumLevels(),
		static_cast<LPCTSTR>(levelAmountString.c_str()));
}
