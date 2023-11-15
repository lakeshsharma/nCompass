#include "stdafx.h"
#include "LongPoll20Message.h"
#include "Sentinel.h"
#include "Logging/LogString.h"

CLongPoll20Message::CLongPoll20Message(byte gameAddress, CMeters *meters, WORD denomMult)
	:
	CMetersLongPoll(gameAddress),
	m_totalBills(INVALID_METER_VALUE),
	m_meters(meters),
	m_denomMult(denomMult)
{
	// allocate buffers
	SetSendBuffer((BYTE *) new byte[POLL_SIZE],POLL_SIZE);
	SetResponseBuffer((BYTE *)new byte[RESPONSE_SIZE],RESPONSE_SIZE);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	memset(GetResponseBuffer(),0,GetResponseBufferSize());
}

CLongPoll20Message::~CLongPoll20Message(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll20Message::GetPollValue()
{
	return POLL_VALUE;
}

__int64 CLongPoll20Message::GetTotalBills()
{
	return m_totalBills;
}

void CLongPoll20Message::ResponseReceived()
{
	if (m_nReturnValue == SASPOLL_SUCCESS)
	{
		BYTE *response = GetResponseBuffer();

		bool ok(RESPONSE_SIZE == GetResponseBufferSize());
		if (ok)
		{
			ok = m_gameAddress == response[GAME_ADDRESS_OFFSET] &&
				POLL_VALUE == response[POLL_VALUE_OFFSET];
		}

		if (ok)
		{
			m_totalBills = Bcd2Int64(response + METER_VALUE_OFFSET, METER_VALUE_SIZE);
			if (m_meters != nullptr)
			{
				if (m_denomMult > 0)
				{
					// Convert m_totalBills from dollars to cents, then divide by denomination to give credits.
					m_meters->SetMeter(mBD, m_totalBills * ONE_DOLLAR_IN_CENTS / m_denomMult);
				}
				else
				{
					// Set the meter (to 0), otherwise, CMeterStepdown will treat the meter as unsupported and may quit polling for it.
					m_meters->SetMeter(mBD, 0);
				}
			}
		}

		if (!ok)
		{
			m_nReturnValue = SASPOLL_NACK; // Not a valid response.
			LogString(ERROR_LOG, _T("SAS LP %02X m_gameAddress=%02x format/parsing error: %s"),
				(unsigned)POLL_VALUE,
				(unsigned)m_gameAddress,
				CUtilities::ToHexString(GetResponseBuffer(), (DWORD)GetResponseBufferSize()).c_str());
		}
	}	
	CSASPollMessage::ResponseReceived();

	return;

}

void CLongPoll20Message::Fill()
{
	// fill long poll command data
	m_sendBuffer[GAME_ADDRESS_OFFSET] = m_gameAddress;
	m_sendBuffer[POLL_VALUE_OFFSET] = POLL_VALUE;

	return;
}

