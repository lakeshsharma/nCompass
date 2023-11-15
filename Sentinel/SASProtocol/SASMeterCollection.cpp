#include "stdafx.h"

#include "SASMeterCollection.h"
#include "Sentinel.h"
#include "QueueList.h"
#include "Logging/LogString.h"
#include "LongPoll10Message.h"
#include "LongPoll19Message.h"
#include "LongPoll1AMessage.h"
#include "LongPoll1CMessage.h"
#include "LongPoll1DMessage.h"
#include "LongPoll1EMessage.h"
#include "LongPoll20Message.h"
#include "LongPoll27Message.h"
#include "LongPoll2AMessage.h"
#include "LongPoll2BMessage.h"
#include "LongPoll2DMessage.h"
#include "LongPoll2FMessage.h"
#include "LongPoll31Message.h"
#include "LongPoll32Message.h"
#include "LongPoll33Message.h"
#include "LongPoll34Message.h"
#include "LongPoll35Message.h"
#include "LongPoll36Message.h"
#include "LongPoll37Message.h"
#include "LongPoll38Message.h"
#include "LongPoll39Message.h"
#include "LongPoll3AMessage.h"
#include "LongPoll46Message.h"
#include "LongPoll52Message.h"
#include "LongPoll6FMessage.h"
#include "LongPoll9AMessage.h"

const byte GAMEADDRESS = 1;
const int DEFAULTMETERCOUNT = 1;
const int MAXATTEMPTS = 3;

CSASMeterCollection::CSASMeterCollection(CSASInterface *sas, CMeters *meters, WORD gameNumber, WORD denom, bool bInSnapshot, bool tryOnce, CMeterStepdown *stepdown, bool isIgtS2000) :
	m_meters(meters),
	m_gameNumber(gameNumber),
	m_denomMult(denom),
	m_inSnapshot(bInSnapshot),
	m_tryOnce(tryOnce),
	m_stepdown(stepdown),
	m_isIgtS2000(isIgtS2000),
	m_tryCount(0),
	m_sas(sas),
	m_meterIndexToStart(0)
{
	//ASSERT(stepdown != nullptr);
	m_numberOfMeters = 0;
}


CSASMeterCollection::~CSASMeterCollection()
{
}


int CSASMeterCollection::CollectMeters(const std::function<void(CSASPollMessage *, int priority)>& sasCallback, const std::function<void(CInternalMessage *msg, int priority)>& intCallback)
{
	if (sasCallback != nullptr && intCallback != nullptr)
	{
		m_sasCallback = sasCallback;
		m_intCallback = intCallback;
		m_consecutiveFailedAttempts = 0;

		m_triedMeters.resize(m_meters->GetMeterCount());
		for (auto& i : m_triedMeters) i  = 0xff;

		m_targetMeterCount = m_stepdown->GetMeterCount();

		// Update internals with snapshot progress
		if (m_inSnapshot && m_gameNumber == EGM_GAME_NUMBER)
		{
			CInternalMessage *msg = new CInternalMessage(INT_MSG_SNAPSHOT_PROGRESS, (WORD)m_numberOfMeters, m_targetMeterCount);
			msg->SetData((DWORD)m_gameNumber);
			intCallback(msg, QUEUE_PRIORITY_HIGHEST);
		}

		// Send long polls until there is nothing left to do
		while (DoNextLongPoll())
		{
			CUtilities::Sleep(0);
		}

		// Log meters collected
#ifdef _DEBUG
		for (int mi = 0; mi < m_meters->GetMeterCount(); mi++)
		{
			if (m_meters->GetMeterByIndex(mi)->GetValue() != INVALID_METER_VALUE)
			{
				// log meter
				LogString(SASGENERAL, "Collected meter %d = %lld", (int)m_meters->GetMeterByIndex(mi)->GetMeterNumber(), (long long)m_meters->GetMeterByIndex(mi)->GetValue());
			}
		}

#endif
		// Send the count of 100 even if all meters were not gotten.
		if (m_inSnapshot && m_numberOfMeters != m_targetMeterCount && m_numberOfMeters > 0 && m_gameNumber == EGM_GAME_NUMBER)
		{
			CInternalMessage *msg = new CInternalMessage(INT_MSG_SNAPSHOT_PROGRESS, m_targetMeterCount, m_targetMeterCount);
			msg->SetData((DWORD)m_gameNumber);
			intCallback(msg, QUEUE_PRIORITY_HIGHEST);
		}

	}

	return m_numberOfMeters;
}

int CSASMeterCollection::GetMetersMessageForLongPoll(int longpoll, CSASPollMessage *& msg, CMeters *meters)
{


	int maxMeterCount = DEFAULTMETERCOUNT;
	switch (longpoll)
	{
	case LP10:
		maxMeterCount = X10_MAX_METERS;
		msg = new CLongPoll10Message(GAMEADDRESS, meters);
		break;
	case LP19:
		maxMeterCount = X19_MAX_METERS;
		msg = new CLongPoll19Message(GAMEADDRESS, meters);
		break;
	case LP1A:
		msg = new CLongPoll1AMessage(GAMEADDRESS, meters);
		break;
	case LP1C:
		maxMeterCount = X1C_MAX_METERS;
		msg = new CLongPoll1CMessage(GAMEADDRESS, meters);
		break;
	case LP1D:
		maxMeterCount = X1D_MAX_METERS;
		msg = new CLongPoll1DMessage(GAMEADDRESS, meters);
		((CLongPoll1DMessage *)msg)->SetDenomMultiplier(m_denomMult);
		break;
	case LP1E:
		maxMeterCount = X1E_MAX_METERS;
		msg = new CLongPoll1EMessage(GAMEADDRESS, meters);
		break;
	case LP20:
		maxMeterCount = X20_MAX_METERS;
		msg = new CLongPoll20Message(GAMEADDRESS, meters, m_denomMult);
		break;
	case LP27:
		maxMeterCount = X27_MAX_METERS;
		msg = new CLongPoll27Message(GAMEADDRESS, meters);
		break;
	case LP2A:
		maxMeterCount = X2A_MAX_METERS;
		msg = new CLongPoll2AMessage(GAMEADDRESS, meters);
		break;
	case LP2B:
		maxMeterCount = X2B_MAX_METERS;
		msg = new CLongPoll2BMessage(GAMEADDRESS, meters);
		break;
	case LP2D:
		maxMeterCount = X2D_MAX_METERS;
		msg = new CLongPoll2DMessage(GAMEADDRESS, meters);
		msg->SetGameNumber(m_gameNumber);
		break;
	case LP2F:
		maxMeterCount = X2F_MAX_METERS;
		msg = new CLongPoll2FMessage(GAMEADDRESS, meters);
		msg->SetGameNumber(m_gameNumber);
		((CLongPoll2FMessage *)msg)->SetDenomMultiplier(m_denomMult);
		break;
	case LP38:
		msg = new CLongPoll38Message(GAMEADDRESS, meters);
		break;
	case LP39:
		msg = new CLongPoll39Message(GAMEADDRESS, meters);
		break;
	case LP31:
		msg = new CLongPoll31Message(GAMEADDRESS, meters);
		break;
	case LP32:
		msg = new CLongPoll32Message(GAMEADDRESS, meters);
		break;
	case LP33:
		msg = new CLongPoll33Message(GAMEADDRESS, meters);
		break;
	case LP34:
		msg = new CLongPoll34Message(GAMEADDRESS, meters);
		break;
	case LP35:
		msg = new CLongPoll35Message(GAMEADDRESS, meters);
		break;
	case LP36:
		msg = new CLongPoll36Message(GAMEADDRESS, meters);
		break;
	case LP37:
		msg = new CLongPoll37Message(GAMEADDRESS, meters);
		break;
	case LP3A:
		msg = new CLongPoll3AMessage(GAMEADDRESS, meters);
		break;
	case LP6F:
		maxMeterCount = X6F_MAX_METERS;
		msg = new CLongPoll6FMessage(GAMEADDRESS, meters);
		msg->SetGameNumber(m_gameNumber);
		((CLongPoll6FMessage *)msg)->SetDenomMultiplier(m_denomMult);
		break;
	case LP46:
		maxMeterCount = X46_MAX_METERS;
		msg = new CLongPoll46Message(GAMEADDRESS, meters);
		break;
	case LP52:
		maxMeterCount = X52_MAX_METERS;
		msg = new CLongPoll52Message(GAMEADDRESS, meters);
		msg->SetGameNumber(m_gameNumber);
		break;
	case LP9A:
		maxMeterCount = X9A_MAX_METERS;
		msg = new CLongPoll9AMessage(GAMEADDRESS, meters);
		msg->SetGameNumber(m_gameNumber);
		break;
	default:
		//ASSERT(false);
		break;
	}

	return maxMeterCount;
}

bool CSASMeterCollection::DoNextLongPoll()
{

	bool retVal = true;

	// check to see if it is necessary to insert
	if (m_consecutiveFailedAttempts > MAXATTEMPTS)
	{
		// Need to insert something that should be a success to keep game comm up.
		CSASPollMessage *msg = nullptr;
        GetMetersMessageForLongPoll(LP1A, msg, nullptr);
		msg->Fill();
		m_sasCallback(msg, m_inSnapshot ? QUEUE_PRIORITY_HIGHEST : QUEUE_PRIORITY_NORMAL);
		if (msg->GetReturnValue() == SASPOLL_SUCCESS)
		{
			m_consecutiveFailedAttempts = 0;
		}
		delete msg;
	}

	// Get the first long poll needed to resolve the passed in meters that don't have a value
	WORD longPoll = INVALID_LONG_POLL;
	for (int mi = m_meterIndexToStart; mi < m_meters->GetMeterCount() && longPoll == INVALID_LONG_POLL; mi++)
	{
		CMeter *meter = m_meters->GetMeterByIndex(mi);
		if (meter->GetValue() == INVALID_METER_VALUE)
		{
			longPoll = m_stepdown->GetMeterLongPoll(meter->GetMeterNumber());

			// look to see if this meter has been tried
			if (m_tryOnce)
			{
				bool thisMeterTried = false;
				for (int i = 0; i < (int)m_triedMeters.size() && !thisMeterTried; i++) if (m_triedMeters[i] == meter->GetMeterNumber())
					{
						longPoll = INVALID_LONG_POLL;
						thisMeterTried = true;
					}
			}
			if (longPoll != INVALID_LONG_POLL)
			{
				// Check to make sure this poll hasn't already failed in this SASMeterCollection instance
				bool _continue = true;
				for (auto it = m_failedPolls.begin(); _continue && it != m_failedPolls.end(); ++it)
				{
					_continue = *it != longPoll;
				}

				if (!_continue)
				{
					longPoll = INVALID_LONG_POLL;
				}
			}
		}
		m_meterIndexToStart = mi;
	}

	m_meterIndexToStart++;
	if (longPoll != INVALID_LONG_POLL)
	{
		m_currentPollMeters = new CMeters(MEMORY_NONE);
		CSASPollMessage *sasMsg = nullptr;
		int maxMeterCount = GetMetersMessageForLongPoll(longPoll, sasMsg, m_currentPollMeters);
		int meterCount = 0;
		if (sasMsg != nullptr)
		{
			// Get a list of all of the meters using this long poll.
			for (int mi = 0; mi < m_meters->GetMeterCount() && meterCount <= maxMeterCount; mi++)
			{
				CMeter *meter = m_meters->GetMeterByIndex(mi);

				//ASSERT(meter->GetMeterNumber() < mMeterLast);

				// if tryOnce is set and this meter is in triedMeters, then skip it
				bool skipMeter = false;
				if (m_tryOnce)
				{
					for (int i = 0; i < (int)m_triedMeters.size() && !skipMeter; i++) if (m_triedMeters[i] == meter->GetMeterNumber())
							skipMeter = true;
					if (skipMeter)
						continue; // this meter has been tried, skip it
				}

				if (meter->GetValue() == INVALID_METER_VALUE && m_stepdown->GetMeterLongPoll(meter->GetMeterNumber()) == longPoll)
				{
					// The meter we are trying to merge should be included in the original list of meters.
					//ASSERT(m_meters->GetMeterByNumber(meter->GetMeterNumber()) != NULL);

					// check if secondary meter would be called
					byte primaryMeterCode, secondaryMeterCode;

					//Get meterCodes
					CSASPollMessage::GetMeterCodeForMeterNumber(meter->GetMeterNumber(), m_gameNumber, primaryMeterCode, secondaryMeterCode);

					if (secondaryMeterCode != INVALID_SAS_METER_CODE)
						meterCount++; // this meternumber will be two metercodes


					// add this meter,
					if (meterCount < maxMeterCount)
					{
						meterCount++;
						m_currentPollMeters->SetMeter(meter->GetMeterNumber());
					}
					else if (secondaryMeterCode != INVALID_SAS_METER_CODE)
					{
						// not adding primary meter, so the secondary meter should not be counted
						meterCount--;
					}
				}
			}

			// Set the data to go to SAS
			// Tracker #25297 -- 2f and 6f polls are handled differently when dealing with igts2000 and ticketing polls.
			if (longPoll == 0x6f || longPoll == 0x2f)
				sasMsg->Fill(m_isIgtS2000);
			else sasMsg->Fill();

			// Callback into CSASProtocol
			m_tryCount++;
			m_sasCallback(sasMsg, m_inSnapshot ? QUEUE_PRIORITY_HIGHEST : QUEUE_PRIORITY_NORMAL);

			WORD lastAcquiredMeterCount = m_numberOfMeters;
			ParseResults(sasMsg);

			if (m_inSnapshot && lastAcquiredMeterCount != m_numberOfMeters && m_gameNumber == EGM_GAME_NUMBER)
			{
				CInternalMessage *msg = new CInternalMessage(INT_MSG_SNAPSHOT_PROGRESS, (WORD)m_numberOfMeters, m_targetMeterCount);
				msg->SetData((DWORD)m_gameNumber);
				m_intCallback(msg, QUEUE_PRIORITY_HIGHEST);
			}

			delete sasMsg;
			delete m_currentPollMeters;
		}
	}
	else
	{
		// Found nothing to do
		retVal = false;
	}

	return retVal;
}

void CSASMeterCollection::ParseResults(CSASPollMessage *msg)
{
	if (IsIndividualBillLP(msg->GetPollValue()) && msg->GetReturnValue() == SASPOLL_SUCCESS)
	{
		// Don't count this poll as a failed try
		m_tryCount--;
	}

	if (msg->GetReturnValue() == SASPOLL_SUCCESS)
	{
		// Merge the meters found in the poll with the meters we passed in
		// also add meters to triedMeters array.
		for (int mi = 0; mi < m_currentPollMeters->GetMeterCount(); mi++)
		{
			CMeter *meter = m_currentPollMeters->GetMeterByIndex(mi);
			if (meter->GetValue() != INVALID_METER_VALUE)
			{

				// If the game is EGM, we should not have meter 0.
				//ASSERT(!(m_gameNumber == EGM_GAME_NUMBER && meter->GetMeterNumber() == mNone));

				// The meter we are trying to merge should be included in the original list of meters.
				//					ASSERT( meters.GetMeterByNumber( meter->GetMeterNumber() ) != NULL );
				//ASSERT(meter->GetMeterNumber() < mMeterLast);

				//ASSERT(m_gameNumber == EGM_GAME_NUMBER || meter->GetMeterNumber() < SUBGAME_METER_LAST);

				// Don't double count meters if we got this one already.
				CMeter *targetMeter = m_meters->GetMeterByNumber(meter->GetMeterNumber());
				if (targetMeter != NULL && targetMeter->GetValue() == INVALID_METER_VALUE)
				{
					m_numberOfMeters++;
				}

				m_meters->SetMeter(meter->GetMeterNumber(), meter->GetValue());
				m_stepdown->MeterLongPollSuccessful(meter->GetMeterNumber());
			}
			else
			{
				if (m_sas->GameComStatus()) // Don't decrement stepdown if gamecomm is down
					m_stepdown->MeterLongPollFail(meter->GetMeterNumber());
			}

			// add metercode to triedMeters
			bool notAdded = true;
			for (int i = 0; m_tryOnce && i < (int)m_triedMeters.size() && notAdded; i++) if (m_triedMeters[i] == 0xff)
				{
					notAdded = false;
					m_triedMeters[i] = meter->GetMeterNumber();
				}

			if (notAdded)
			{
				// still not added.
				m_triedMeters.push_back(meter->GetMeterNumber());
			}

		}
	}
	else
	{
		// Need to "fail" for stepdown purposes
		m_consecutiveFailedAttempts++;
		for (int mi = 0; mi < m_currentPollMeters->GetMeterCount(); mi++)
		{
			CMeter *meter = m_currentPollMeters->GetMeterByIndex(mi);
			if (m_sas->GameComStatus()) // Don't decrement stepdown if gamecomm is down
				m_stepdown->MeterLongPollFail(meter->GetMeterNumber());
		}

		// After longpoll fails, don't try anymore of this poll during this SASMeterCollection instance
		m_failedPolls.push_back(msg->GetPollValue());
	}
}

bool CSASMeterCollection::IsIndividualBillLP(BYTE lp)
{
	bool retVal = false;
	if (lp >= 0x31 && lp <= 0x45 && lp != 0x3d)
	{
		retVal = true;
	}

	return retVal;
}

