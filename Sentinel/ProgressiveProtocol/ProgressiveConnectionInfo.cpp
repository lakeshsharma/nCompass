#include "stdafx.h"
#include "ProgressiveConnectionInfo.h"

#include "Sentinel.h"
#include "Utilities.h"

CProgressiveConnectionInfo::CProgressiveConnectionInfo() :
	m_connectionState(NotStarted),
	m_lastConnectionError(ProgressiveConnectionErrorNone),
	m_connectAttemptCount(0),
	m_lastHeartbeatMessageTickCount(0),
	m_pingInterval(CProgressiveConfig::DEFAULT_PING_INTERVAL)
{
	m_startupTimestamp = CUtilities::Get1970Time();
	m_lastMessageTimestamp = CUtilities::Get1970Time();
	m_lastPatronInfoTimestamp = CUtilities::Get1970Time();
	m_lastCardInTimestamp = CUtilities::Get1970Time();
	m_lastConnectAttemptTimestamp = CUtilities::Get1970Time();
	m_lastConnectionTimestamp = CUtilities::Get1970Time();
	m_lastAddressUpdateTimestamp = CUtilities::Get1970Time();
    m_DisconnectTimeStampSinceBoot = CUtilities::Get1970Time();
	m_lastMulticastSocketFailedTimeStamp = CUtilities::Get1970Time();
	m_DisconnectCount = 0;
	m_multicastFailCount = 0;
}

/// <summary>
/// Copy Constructor
/// </summary>
CProgressiveConnectionInfo::CProgressiveConnectionInfo(const CProgressiveConnectionInfo &rhs)
{
	CopyObj(rhs);
}

CProgressiveConnectionInfo::~CProgressiveConnectionInfo()
{}

CProgressiveConnectionInfo& CProgressiveConnectionInfo::operator=(const CProgressiveConnectionInfo &rhs)
{
	if (this != &rhs)
	{
		CleanUp();
		CopyObj(rhs);
	}
	return *this;
}

/// <summary>
/// Deletes all the dynamically allocated memory.
/// </summary>
void CProgressiveConnectionInfo::CleanUp()
{}

/// <summary>
/// Copies an object of type CProgressiveConnectionInfo.
/// </summary>
/// <param name="rhs">Constant reference to 
/// CProgressiveConnectionInfo object that is to be 
/// copied.</param> 
void CProgressiveConnectionInfo::CopyObj(const CProgressiveConnectionInfo& rhs)
{
	SetLastMessageIP(rhs.GetLastMessageIP());
	SetLastConnectTimestamp(rhs.GetLastConnectTimestamp());
	SetLastMessageTimestamp(rhs.GetLastMessageTimestamp());
	SetLastPatronInfoTimestamp(rhs.GetLastPatronInfoTimestamp());
	SetLastCardInTimestamp(rhs.GetLastPatronInfoTimestamp());
	SetState(rhs.GetState());
	SetLastError(rhs.GetLastError());
	SetConnectAttemptCount(rhs.GetConnectAttemptCount());
	SetStartupTimestamp(rhs.GetStartupTimestamp());
	SetLastConnectAttemptTimestamp(rhs.GetLastConnectAttemptTimestamp());
	SetLastAddressUpdateTimestamp(rhs.GetLastAddressUpdateTimestamp());
    SetDisconnectTimeStampSinceBoot(rhs.m_DisconnectTimeStampSinceBoot);
	SetDisconnectCount(rhs.m_DisconnectCount);
	m_lastMulticastSocketFailedTimeStamp = rhs.m_lastMulticastSocketFailedTimeStamp;
	m_multicastFailCount = rhs.m_multicastFailCount;
}


void CProgressiveConnectionInfo::SetPingInterval(DWORD pingInterval)
{
	m_pingInterval = pingInterval;
}

const CIPAddress& CProgressiveConnectionInfo::GetLastMessageIP() const
{
	return m_lastMessageIP;
}

void CProgressiveConnectionInfo::SetLastMessageIP(const CIPAddress &messageIP)
{
	m_lastMessageIP = messageIP;
}

const SYSTEMTIME & CProgressiveConnectionInfo::GetStartupTimestamp() const
{
	return m_startupTimestamp;
}

const std::string CProgressiveConnectionInfo::GetStartupTimestampString() const
{
	time_t timestamp = CUtilities::GetTimeFromUTCSystemTime((SYSTEMTIME)m_startupTimestamp);

	return CUtilities::FormatMilitaryDateTimeString(timestamp);
}

void CProgressiveConnectionInfo::SetStartupTimestamp(const SYSTEMTIME &timestamp)
{
	m_startupTimestamp = timestamp;
}

void CProgressiveConnectionInfo::SetStartupTimestamp()
{
	SYSTEMTIME currentTimestamp;
	CUtilities::GetUTCSystemTime(&currentTimestamp);
	m_startupTimestamp = currentTimestamp;
}

const SYSTEMTIME & CProgressiveConnectionInfo::GetLastConnectTimestamp() const
{
	return m_lastConnectionTimestamp;
}

std::string CProgressiveConnectionInfo::GetLastConnectTimestampString() const
{
	return CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(m_lastConnectionTimestamp);
}

void CProgressiveConnectionInfo::SetLastConnectTimestamp(const SYSTEMTIME &timestamp)
{
	m_lastConnectionTimestamp = timestamp;
}

const SYSTEMTIME & CProgressiveConnectionInfo::GetLastMessageTimestamp() const
{
	return m_lastMessageTimestamp;
}

std::string CProgressiveConnectionInfo::GetLastMessageTimestampString() const
{
	return CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(m_lastMessageTimestamp);
}

void CProgressiveConnectionInfo::SetLastMessageTimestamp(const SYSTEMTIME &timestamp)
{
	m_lastMessageTimestamp = timestamp;
}

void CProgressiveConnectionInfo::SetLastMessageTimestamp()
{
	SYSTEMTIME currentTimestamp;
	CUtilities::GetUTCSystemTime(&currentTimestamp);
	m_lastMessageTimestamp = currentTimestamp;
}

bool CProgressiveConnectionInfo::IsTimeForPing(DWORD tickCount) const
{
	bool expired(false);

	if (Connected == m_connectionState)
	{
		DWORD elapsedTickCount = tickCount - m_lastHeartbeatMessageTickCount;
		expired = elapsedTickCount >= m_pingInterval;
	}

	return expired;
}

void CProgressiveConnectionInfo::SetLastHeartbeatMessageTickCount(DWORD tickCount)
{
	m_lastHeartbeatMessageTickCount = tickCount;
}

const SYSTEMTIME & CProgressiveConnectionInfo::GetLastPatronInfoTimestamp() const
{
	return m_lastPatronInfoTimestamp;
}

void CProgressiveConnectionInfo::SetLastPatronInfoTimestamp(const SYSTEMTIME &timestamp)
{
	m_lastPatronInfoTimestamp = timestamp;
}

void CProgressiveConnectionInfo::SetLastPatronInfoTimestamp()
{
	SYSTEMTIME currentTimestamp;
	CUtilities::GetUTCSystemTime(&currentTimestamp);
	m_lastPatronInfoTimestamp = currentTimestamp;
}

const SYSTEMTIME & CProgressiveConnectionInfo::GetLastCardInTimestamp() const
{
	return m_lastCardInTimestamp;
}

void CProgressiveConnectionInfo::SetLastCardInTimestamp(const SYSTEMTIME &timestamp)
{
	m_lastCardInTimestamp = timestamp;
}

void CProgressiveConnectionInfo::SetLastCardInTimestamp()
{
	SYSTEMTIME currentTimestamp;
	CUtilities::GetUTCSystemTime(&currentTimestamp);
	m_lastCardInTimestamp = currentTimestamp;
}

ProgressiveConnectionState CProgressiveConnectionInfo::GetState() const
{
	return m_connectionState;
}

void CProgressiveConnectionInfo::SetState(ProgressiveConnectionState state)
{
	m_connectionState = state;
}

ProgressiveConnectionError CProgressiveConnectionInfo::GetLastError() const
{
	return m_lastConnectionError;
}

void CProgressiveConnectionInfo::SetLastError(ProgressiveConnectionError error)
{
	m_lastConnectionError = error;
}

void CProgressiveConnectionInfo::SetConnectAttemptCount(const int connectAttemptCount)
{
	m_connectAttemptCount = connectAttemptCount;
}

int CProgressiveConnectionInfo::GetConnectAttemptCount() const
{
	return m_connectAttemptCount;
}

const SYSTEMTIME & CProgressiveConnectionInfo::GetLastConnectAttemptTimestamp() const
{
	return m_lastConnectAttemptTimestamp;
}

void CProgressiveConnectionInfo::SetLastConnectAttemptTimestamp(const SYSTEMTIME &timestamp)
{
	m_lastConnectAttemptTimestamp = timestamp;
}

const SYSTEMTIME & CProgressiveConnectionInfo::GetLastAddressUpdateTimestamp() const
{
	return m_lastAddressUpdateTimestamp;
}

void CProgressiveConnectionInfo::SetLastAddressUpdateTimestamp(const SYSTEMTIME &timestamp)
{
	m_lastAddressUpdateTimestamp = timestamp;
}

void CProgressiveConnectionInfo::SetLastAddressUpdateTimestamp()
{
	SYSTEMTIME currentTimestamp;
	CUtilities::GetUTCSystemTime(&currentTimestamp);
	m_lastAddressUpdateTimestamp = currentTimestamp;
}

void CProgressiveConnectionInfo::ConnectAttempt()
{
	m_connectAttemptCount++;
	SYSTEMTIME currentTimestamp;
	CUtilities::GetUTCSystemTime(&currentTimestamp);
	SetLastConnectAttemptTimestamp(currentTimestamp);
}

void CProgressiveConnectionInfo::SetConnected()
{
	SetState(Connected);
	m_connectAttemptCount = 0;
	SYSTEMTIME currentTimestamp;
	CUtilities::GetUTCSystemTime(&currentTimestamp);
	SetLastConnectTimestamp(currentTimestamp);
}

std::string CProgressiveConnectionInfo::GetConnectionStateString(bool shortString) const
{
	std::string stateString;

	switch (m_connectionState)
	{
	case	NotStarted:
		stateString = CUtilities::LoadResourceString(IDS_CONNECTION_NOT_STARTED);
		break;
	case Disabled:
		stateString = CUtilities::LoadResourceString(IDS_CONNECTION_DISABLED);
		break;
	case WaitingForInfo:
		stateString = CUtilities::LoadResourceString(IDS_CONNECTION_WAITING_FOR_INFO);
		break;
	case InDiscovery:
		stateString = CUtilities::LoadResourceString(IDS_CONNECTION_IN_DISCOVERY);
		break;
	case InitiateDiscovery:
		stateString = CUtilities::LoadResourceString(IDS_CONNECTION_INITIATE_DISCOVERY);
		break;
	case Connected:
		stateString = CUtilities::LoadResourceString(IDS_CONNECTION_CONNECTED);
		break;
	case Disconnected:
		stateString = CUtilities::LoadResourceString(IDS_CONNECTION_DISCONNECTED);
		break;
	case Error:
		switch (m_lastConnectionError)
		{
		case CouldNotFindServer:
			stateString = CUtilities::LoadResourceString(
				shortString ? IDS_CONNECTION_ERROR_COULD_NOT_FIND_SERVER_SHORT : IDS_CONNECTION_ERROR_COULD_NOT_FIND_SERVER);
			break;
		case DiscoveryConnectionError:
			stateString = CUtilities::LoadResourceString(
				shortString ? IDS_CONNECTION_ERROR_DISCOVERY_CONNECTION_SHORT : IDS_CONNECTION_ERROR_DISCOVERY_CONNECTION);
			break;
		case MessageTransmissionError:
			stateString = CUtilities::LoadResourceString(
				shortString ? IDS_CONNECTION_ERROR_MESSAGE_TRANSMISSION_SHORT : IDS_CONNECTION_ERROR_MESSAGE_TRANSMISSION);
			break;
		case ConnectFailed:
			stateString = CUtilities::LoadResourceString(
				shortString ? IDS_CONNECTION_ERROR_CONNECT_FAILED_SHORT : IDS_CONNECTION_ERROR_CONNECT_FAILED);
			break;
		case DisconnectFailed:
			stateString = CUtilities::LoadResourceString(
				shortString ? IDS_CONNECTION_ERROR_DISCONNECT_FAILED_SHORT : IDS_CONNECTION_ERROR_DISCONNECT_FAILED);
			break;
		default:
			stateString = CUtilities::LoadResourceString(IDS_CONNECTION_ERROR_DEFAULT);
			break;
		}
		break;
	default:
		stateString = CUtilities::LoadResourceString(IDS_CONNECTION_DEFAULT);
		break;
	}

	return stateString;
}

int CProgressiveConnectionInfo::GetDisconnectCount(void) const
{
	return (m_DisconnectCount);
}

void CProgressiveConnectionInfo::SetDisconnectTimeStampSinceBoot(void)
{
	time_t today_time = time(0);
	m_DisconnectTimeStampSinceBoot = CUtilities::GetUTCSystemTime(today_time);
}

void CProgressiveConnectionInfo::IncrementDisconnectCount(void)
{
	++m_DisconnectCount;
}

SYSTEMTIME CProgressiveConnectionInfo::GetDisconnectTimeStampSinceBoot(void) const
{
	return m_DisconnectTimeStampSinceBoot;
}

const SYSTEMTIME& CProgressiveConnectionInfo::GetLastMulticastSocketFailedTimeStamp(int& failedMulticastCount) const
{
	failedMulticastCount = m_multicastFailCount;
	return m_lastMulticastSocketFailedTimeStamp;
}

void CProgressiveConnectionInfo::SetLastMulticastSocketFailedTimeStamp()
{
	SYSTEMTIME currentTimestamp;
	CUtilities::GetUTCSystemTime(&currentTimestamp);
	m_lastMulticastSocketFailedTimeStamp = currentTimestamp;
	++m_multicastFailCount;
}

void CProgressiveConnectionInfo::SetDisconnectTimeStampSinceBoot(SYSTEMTIME timestamp)
{
	m_DisconnectTimeStampSinceBoot = timestamp;
}

void CProgressiveConnectionInfo::SetDisconnectCount(int counter)
{
	m_DisconnectCount = counter;
}



