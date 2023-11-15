#include "stdafx.h"

#include "TiltManager.h"
#include "UI/TiltLog.h"

using namespace std;

const int DefaultTiltTimeoutInSeconds = TILT_TIMEOUT * 60;

CTiltManager::CTiltManager() : CDataSubscriptionProvider(TILT_LOGS)
{
	m_tiltTimeoutInSeconds = DefaultTiltTimeoutInSeconds;
	Init(MEMORY_NVRAM);
}

CTiltManager::CTiltManager(MEMORY_TYPE memoryType, int timeoutInSeconds) :
CDataSubscriptionProvider(TILT_LOGS)
{
	m_tiltTimeoutInSeconds = timeoutInSeconds;
	Init(memoryType);
}

CTiltManager::~CTiltManager()
{
	delete m_tiltLog;
}

string CTiltManager::GetSubscriptionDataResponse(const string& topic, const string& whereId)
{
    UNREFERENCED_PARAMETER(&topic); 
    UNREFERENCED_PARAMETER(&whereId); 

	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);

	writer.StartObject();
	writer.Key(TILT_LOGS.c_str());
	{
		writer.StartArray();
		{
			int numLogs(m_tiltLog->GetNumItems());
			string timeStamp, code;
			CTiltLog* log((CTiltLog *)m_tiltLog->GetFirst());
			for (int index = 0; index < numLogs; index++)
			{
				if (log != nullptr)
				{
					writer.StartObject();
					{
						timeStamp = FormatString("(%d) %s", index + 1, log->GetDateTimeAsString().c_str());
						code = FormatString("%d", log->GetTiltCode());

						writer.Key("TimeStamp");	writer.String(timeStamp);
						writer.Key("Code");			writer.String(code);						
					}
					writer.EndObject();
					log = (CTiltLog *)m_tiltLog->GetNext();
				}
				else
				{
					break;
				}
			}
		}
		writer.EndArray();
	}
	writer.EndObject();

	return writeStream.m_str;
}

void CTiltManager::Init(MEMORY_TYPE memoryType)
{
	m_tiltLog = new CLogs(memoryType, LOG_TILT, TILT_LOGS);
	
	m_lastTilt = tltUnknown;
	m_lastButOneTilt = tltUnknown;
	m_lastButTwoTilt = tltUnknown;
	m_lastButThreeTilt = tltUnknown;
	m_lastTiltTime = CUtilities::GetCurrentTime();
	m_lastButOneTiltTime = m_lastTiltTime;
	m_lastButTwoTiltTime = m_lastTiltTime;
	m_lastButThreeTiltTime = m_lastTiltTime;
}

CLogs *CTiltManager::GetTiltLogs()
{
	return m_tiltLog;
}

bool CTiltManager::CheckIfTiltTimeLimitNeedsToBeIgnored(TiltCode tilt) const
{
	bool sendTilt(false);

	switch (tilt)
	{
	case tltAutoCardOutCountdownStartWithCredits:
	case tltAutoCardOutCountdownStartWithNoCredits:
	case tltAutoCardOutTimerReset:
	case tltAutoCardOutTimerExpired:
	case tltAutoCardOutTimerEndedByPlayer:
		sendTilt = true;
		break;
	default:
		break;
	}

	return sendTilt;
}

bool CTiltManager::ProcessTilt(TiltCode tilt, InternalMsgType &additionaltransactionToSend)
{
	additionaltransactionToSend = INT_MSG_NONE;
	bool sendTilt = tilt != tltUnknown;
	time_t tiltTime = CUtilities::GetCurrentTime();

	//if it's the same tilt as three or four of the last four.
	if (sendTilt && 
		!CheckIfTiltTimeLimitNeedsToBeIgnored(tilt) &&
		((tilt == m_lastButTwoTilt) || (tilt == m_lastButThreeTilt)))
	{
		//and it is with in the time limit, don't send the tilt
		time_t timeDifference = tiltTime - m_lastButTwoTiltTime;
		//if this one's ok:
		if (timeDifference >= m_tiltTimeoutInSeconds)
		{
			//check the other one
			timeDifference = tiltTime - m_lastButThreeTiltTime;
			if (timeDifference < m_tiltTimeoutInSeconds)
			{
				LogString(POLLER_LOG, "%s%d don't send tilt code %d not within time limit", __FUNCTION__, __LINE__, tilt);
				sendTilt = false;
			}
		}
		else
		{
			LogString(POLLER_LOG, "%s%d don't send tilt code %d not within time limit", __FUNCTION__, __LINE__, tilt);
			sendTilt = false;
		}
	}

	if (sendTilt)
	{
		switch (tilt)
		{
		case tltBillStackerOut:
			additionaltransactionToSend = INT_MSG_SEND_STACKER_OUT;
			sendTilt = false;
			break;
		case tltBillStackerIn:
			additionaltransactionToSend = INT_MSG_SEND_STACKER_IN;
			sendTilt = false;
			break;
		case tltGamePowerOn:
			additionaltransactionToSend = INT_MSG_TRANSACTION_SLOT_POWER_ON;
			break;
		case tltGamePowerOff:
			additionaltransactionToSend = INT_MSG_TRANSACTION_SLOT_POWER_OFF;
			break;
		default:
			additionaltransactionToSend = INT_MSG_NONE;
			break;
		}

		//send the composite tilt transaction
		if (sendTilt)
		{
			//Add tilt to tiltLog
			CTiltLog *tiltLog = new CTiltLog((WORD)tilt, tiltTime);
			m_tiltLog->Add(tiltLog);
		}

		//only shunt up the tilt times if we send the transaction
		//this is so a continuous tilt gets sent after the timeout
		m_lastButThreeTiltTime = m_lastButTwoTiltTime;
		m_lastButTwoTiltTime = m_lastButOneTiltTime;
		m_lastButOneTiltTime = m_lastTiltTime;
		m_lastTiltTime = tiltTime;
	}

	m_lastButThreeTilt = m_lastButTwoTilt;
	m_lastButTwoTilt = m_lastButOneTilt;
	m_lastButOneTilt = m_lastTilt;
	m_lastTilt = tilt;

	return sendTilt;
}
