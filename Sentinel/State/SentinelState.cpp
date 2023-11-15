#include "stdafx.h"
#include "State/SentinelState.h"
#include "Utilities.h"
#include "RapidJsonHelper.h"
#include "Game.h"
#include "UI/ScreenDefs.h"
#include "PollerProtocol/PollerMessage.h"
#include "Logging/LogString.h"

using namespace std;

const string CSentinelState::STATUS_TOPIC("Status");
const string CSentinelState::SENTINEL_STATE_TOPIC("SentinelState");
const string CSentinelState::ONELINK_CONNECTION_TOPIC("OneLinkConnection");
const string CSentinelState::DISK_SPACE_TOPIC("DiskSpace");
const string CSentinelState::MEMORY_CLEAR_TOPIC("MemoryClear");
const string CSentinelState::DOWNLOAD_STATE_TOPIC("DownloadState");
const string CSentinelState::METER_CAPTURE_TOPIC("MeterCapture");
const string CSentinelState::GRAPHIC_CARD_CONTENT_TOPIC("GraphicCardContent");

const string MEMORY_CLEAR_TXT_FILE("/MemoryClear.txt");
const string MEMORY_CLEAR_KEY("8D33AE49-D558-4aca-B99B-B7C65C97DCBD");

const string DEFAULT_WARNING_MESSAGE("This machine will not accept cards during this process. Please do not reboot or play until all meters are captured.");
const string DEFAULT_CAPTURE_MESSAGE("Capturing machine meters");
const int METER_CAPTURE_PENDING(0);
const int METER_CAPTURE_FINISHED(100);
const DWORD METER_CAPTURE_TIMEOUT(60000);
const WORD DEF_VAL(0);

//Don't keep setting this value because Angular UI 
//keeps getting bombarded with updates.
const long SET_EGM_ERROR_COUNT_MILLISECONDS(5000);

CSentinelState::CSentinelState(const time_t& startupTime, const string& sentinelVersion) :
CDataSubscriptionProvider(""),
m_startupTime(startupTime),
m_sentinelVersion(sentinelVersion),
m_availablePhysicalMemory(AVAILABLE_PHYSICAL_MEMORY_NOT_AVAILABLE),
m_tickCountOfLastFloorLogixTimeUpdateAvailable(false),
m_tickCountOfLastFloorLogixTimeUpdate(0),
m_timeOfLastFloorLogixConfigAvailable(false),
m_timeOfLastFloorLogixConfig(0),
m_lastUIRestartTime(0),
m_uiRestartCount(0),
m_lastUIRefreshTime(0),
m_uiRefreshCount(0),
m_rebootTilt(tltRebootUnknown),
m_pollerPacketSentVerifyErrors(0),
m_pollerPacketVersion(0),
m_oneLinkProgressivesSenOptionSupported(false),
m_oneLinkProgressivesEnabled(false),
m_mobileConnectCommEnabled(false),
m_pendingAction(NonePending),
m_currentActivity(CurrentActivity::None),
m_doorOpen(false),
m_lastCoinsPlayed(0),
m_virtualPlayer(false),
m_progressiveHost(_T(""), 0),
m_egmSupportsLP7B_Available(false),
m_egmSupportsLP7B(false),
m_meterCaptureMessage(DEFAULT_CAPTURE_MESSAGE),
m_meterCapturePercentage(METER_CAPTURE_PENDING),
m_meterCaptureStatus(CAPTURE_OK),
m_meterCaptureInProgress(false),
m_keepMeterCaptureUp(false),
m_snapShotType(SnapshotNone),
m_snapshotStartTime(CUtilities::GetCurrentTime()),
m_rolloverStartTime(0),
m_snapshotStatusTickCount(CUtilities::GetTickCount()),
m_meterChangeInProgress(meterChangeRequestNone),
m_transactionFillStatus(transactionsFillWarning),// Do not assume our transaction fill status is okay. Let the SCF tell us it is okay.
m_gameOnline(false), 
m_egmErrorCount(0),
m_employeeGoodbyeEnabled(false),
m_memoryClearSuccessful(false),
m_downloadStatus(FLASH_NONE,0,"","","",""),
m_progGames(nullptr),
m_skinId(-1),
m_skinRevision(0),
m_campaignRevision(0),
m_campaignId(-1),
m_buiRestartInProgress(false),
m_gameChunkIndexSent(DEF_VAL),
m_numberOfGameChunks(DEF_VAL),
m_cashoutToHostStatusOnEGM(0),
m_cashoutToHostRequestsAccepted(0),
m_cashoutToHostRequestsCanceled(0),
m_canPerformMeterRollover(false),
m_meterRolloverSyncEnabled(false),
m_meterRolloverTimeHour(INVALID_TIME),
m_meterRolloverTimeMinute(INVALID_TIME),
m_meterRolloverTimeSecond(INVALID_TIME)
{	
	CUtilities::GetUTCSystemTime(&m_lastSetEgmErroCountTimeStamp);

	AddTopic(SENTINEL_STATE_TOPIC);	
	AddTopic(STATUS_TOPIC);
	AddTopic(ONELINK_CONNECTION_TOPIC);
	AddTopic(DISK_SPACE_TOPIC);
	AddTopic(MEMORY_CLEAR_TOPIC);	
	AddTopic(DOWNLOAD_STATE_TOPIC);
	AddTopic(METER_CAPTURE_TOPIC);
	AddTopic(GRAPHIC_CARD_CONTENT_TOPIC);	
}

CSentinelState::~CSentinelState()
{
	delete m_progGames;
}

string CSentinelState::GetSubscriptionDataResponse(const std::string& topic, const std::string& whereId)
{
    UNREFERENCED_PARAMETER(&whereId);

	string dataResponse;
	if (SENTINEL_STATE_TOPIC == topic)
	{
		dataResponse = ResponseForSentinelState();
	}	
	else if (STATUS_TOPIC == topic)
	{
		dataResponse = ResponseForStatus();
	}
	else if (ONELINK_CONNECTION_TOPIC == topic)
	{
		dataResponse = ResponseForOneLinkConnection();
	}
	else if (DISK_SPACE_TOPIC == topic)
	{
		dataResponse = ResponseForDiskSpace();
	}
	else if (MEMORY_CLEAR_TOPIC == topic)
	{
		dataResponse = ResponseForMemoryClear();
	}
	else if (DOWNLOAD_STATE_TOPIC == topic)
	{
		dataResponse = ResponseForDownloadState();
	}
	else if (METER_CAPTURE_TOPIC == topic)
	{
		dataResponse = ResponseForMeterCapture();
	}
	else if (GRAPHIC_CARD_CONTENT_TOPIC == topic)
	{
		dataResponse = ResponseForGraphicCardContent();
	}
	
	return dataResponse;
}

void CSentinelState::SetProgressiveConfig(const CProgressiveConfig &progressiveConfig)
{
		lock_guard<recursive_mutex> lock(m_criticalSection);
		m_progressiveConfig = progressiveConfig;
	}

CProgressiveConfig* CSentinelState::GetProgressiveConfigCopy() const
{
	lock_guard<recursive_mutex> lock(m_criticalSection);

	CProgressiveConfig* progressiveConfig(new CProgressiveConfig(m_progressiveConfig));

	return progressiveConfig;
}

void CSentinelState::SetProgressiveConnectionInfo(const CProgressiveConnectionInfo &progressiveConnectionInfo)
{
	lock_guard<recursive_mutex> lock(m_criticalSection);
	m_progressiveConnectionInfo = progressiveConnectionInfo;
}

CProgressiveConnectionInfo* CSentinelState::GetProgressiveConnectionInfoCopy() const
{
	lock_guard<recursive_mutex> lock(m_criticalSection);
	CProgressiveConnectionInfo* progressiveConnectionInfo(new CProgressiveConnectionInfo(m_progressiveConnectionInfo));
	return progressiveConnectionInfo;
}

void CSentinelState::SetProgressiveHost(const CProgressiveHost &progressiveHost)
{
	lock_guard<recursive_mutex> lock(m_criticalSection);
	m_progressiveHost = progressiveHost;
}

CProgressiveHost* CSentinelState::GetProgressiveHostCopy() const
{
	lock_guard<recursive_mutex> lock(m_criticalSection);
	CProgressiveHost* progressiveHost(new CProgressiveHost(m_progressiveHost));

	return progressiveHost;
} 

void CSentinelState::SetPendingAction(PendingAction pendingAction)
{
	m_pendingAction = pendingAction;
}

PendingAction CSentinelState::GetPendingAction()
{
	return m_pendingAction;
}

string CSentinelState::GetPendingActionAsString() const
{
	string pendingAction;

	switch (m_pendingAction)
	{
	case NonePending:
		pendingAction = "NO_PENDING_ACTION";
		break;
	case RestartUI:
		pendingAction = "RESTART_UI";
		break;
	case RebootAfterDrop:
		pendingAction = "REBOOT_AFTER_DROP";
		break;
	case RebootAsap:
		pendingAction = "REBOOT_ASAP";
		break;
	case OsUpgrade:
		pendingAction = "OS_UPGRADE";
		break;
	case RemoteReboot:
		pendingAction = "REMOTE_REBOOT";
		break;
	case RefreshUI:
		pendingAction = "REFRESH_UI";
		break;
	default:
		pendingAction = "NO_PENDING_ACTION";
		break;
	}
	
	return pendingAction;
}

void CSentinelState::ClearPendingAction(PendingAction pendingAction)
{
	if (m_pendingAction == pendingAction)
	{
		m_pendingAction = NonePending;
	}	
}

void CSentinelState::SetCurrentActivity(CurrentActivity currentActivity)
{
	m_currentActivity = currentActivity;

	if (m_currentActivity == CurrentActivity::RestartingUi && m_pendingAction == PendingAction::RestartUI)
	{
		LogString(PROGRESSIVE, "Resetting pending action to None, as current activity is RestartingUi.");
		m_pendingAction = PendingAction::NonePending;
	}
}

CurrentActivity CSentinelState::GetCurrentActivity()
{
	return m_currentActivity;
}

void CSentinelState::ClearCurrentActivity(CurrentActivity currentActivity)
{
	if (m_currentActivity == currentActivity)
	{
		m_currentActivity = CurrentActivity::None;
	}
}

void CSentinelState::SetDoorOpen(bool value)
{
	m_doorOpen = value;
}

void CSentinelState::SetLastCoinsPlayed(int lastCoinsPlayed)
{
	m_lastCoinsPlayed = lastCoinsPlayed;
}

void CSentinelState::SetReportedEgmProtocolVersion(const string& reportedEgmProtocolVersion)
{
	lock_guard<recursive_mutex> lock(m_criticalSection);
	m_reportedEgmProtocolVersion = reportedEgmProtocolVersion;
}

string CSentinelState::GetReportedEgmProtocolVersion() const
{
	lock_guard<recursive_mutex> lock(m_criticalSection);
	string returnValue(m_reportedEgmProtocolVersion);
	return returnValue;
}

void CSentinelState::SetEGMCashoutToHostStatus(byte value)
{
	m_cashoutToHostStatusOnEGM = value;
}

byte CSentinelState::GetEGMCashoutToHostStatus() const
{
	return m_cashoutToHostStatusOnEGM;
}

void CSentinelState::IncrementCashoutToHostRequestsAccepted()
{
	m_cashoutToHostRequestsAccepted++;
}

DWORD CSentinelState::GetCashoutToHostRequestsAccepted() const
{
	return m_cashoutToHostRequestsAccepted;
}

void CSentinelState::IncrementCashoutToHostRequestsCanceled()
{
	m_cashoutToHostRequestsCanceled++;
}

DWORD CSentinelState::GetCashoutToHostRequestsCanceled() const
{
	return m_cashoutToHostRequestsCanceled;
}

void CSentinelState::SetEgmSupportsLP7B_Available(bool egmSupportsLP7B_Available)
{
	m_egmSupportsLP7B_Available = egmSupportsLP7B_Available;
}

bool CSentinelState::GetEgmSupportsLP7B_Available() const
{
	return m_egmSupportsLP7B_Available;
}

void CSentinelState::SetEgmSupportsLP7B(bool egmSupportsLP7B)
{
	m_egmSupportsLP7B = egmSupportsLP7B;
}

bool CSentinelState::GetEgmSupportsLP7B() const
{
	return m_egmSupportsLP7B;
}

bool CSentinelState::IsMeterCaptureInProgress() const
{
	return m_snapShotType != SnapshotNone;
}

SnapshotType CSentinelState::GetMeterCaptureType() const
{
	return m_snapShotType;
}

void CSentinelState::SetMeterCaptureStartTime(SnapshotType type)
{
	m_snapShotType = type;
	m_snapshotStartTime = CUtilities::GetCurrentTime();
	if (MachineRollover == m_snapShotType)
	{
		m_snapshotStartTime = m_rolloverStartTime;
	}
}

time_t CSentinelState::GetMeterCaptureStartTime() const
{
	return m_snapshotStartTime;
}

void CSentinelState::BeginMeterCapture(SnapshotType type, bool keepMeterCaptureDisplayUpTilFinished)
{
	if (m_snapShotType == SnapshotNone)
	{
		m_meterCaptureInProgress = true;
		m_keepMeterCaptureUp = keepMeterCaptureDisplayUpTilFinished;
		m_snapShotType = type;
		m_snapshotStatusTickCount = CUtilities::GetTickCount();// This will time out the meter capture.
		m_meterCaptureStatus = CAPTURE_OK;
		m_meterCapturePercentage = METER_CAPTURE_PENDING;
		m_meterCaptureMessage = DEFAULT_CAPTURE_MESSAGE;
	}
}

void CSentinelState::SetMeterCapturePercentage(DWORD gameNum, WORD totalMeters, WORD finishedMeters)
{
	m_snapshotStatusTickCount = CUtilities::GetTickCount();
	if (gameNum != EGM_GAME_NUMBER)
	{
		m_meterCaptureMessage = FormatString(TXT_MECHANIC_CAPTURED_SUB_GAMES.c_str(), finishedMeters, totalMeters);
	}

	if (totalMeters > 0)
	{
		m_meterCapturePercentage = (int)(finishedMeters * 100) / totalMeters;
	}
}

void CSentinelState::SetMeterCaptureEnd(MeterCaptureStatus captureStatus, WORD gameCount)
{
	m_snapShotType = SnapshotNone;
	m_keepMeterCaptureUp = false;
	m_meterCaptureStatus = captureStatus;
	m_meterCaptureMessage = gameCount >= 1 ? TXT_FINISHED_METERS_SUB : TXT_FINISHED_METERS;
	if (CAPTURE_OK == m_meterCaptureStatus)
	{
		m_meterCapturePercentage = METER_CAPTURE_FINISHED;
	}
	else if (CAPTURE_REBOOT == m_meterCaptureStatus)
	{
		m_meterCapturePercentage = METER_CAPTURE_FINISHED;
		m_meterCaptureMessage = TXT_MECHANIC_REBOOT_IN_PROGRESS;
	}
	else if (CAPTURE_CANCELED == m_meterCaptureStatus)
	{
		m_meterCaptureMessage = TXT_FAILED_METERS;
	}
}

void CSentinelState::SetGameOnline(bool value)
{
	m_gameOnline = value;
}

bool CSentinelState::IsGameOnline() const
{
	return m_gameOnline;
}

void CSentinelState::SetEgmErrorCount(WORD value)
{
	if (value == 0 || (!m_gameOnline && CUtilities::GetMillisecondsSinceTimestamp(m_lastSetEgmErroCountTimeStamp) >= SET_EGM_ERROR_COUNT_MILLISECONDS))
	{
		m_egmErrorCount = value;
		CUtilities::GetUTCSystemTime(&m_lastSetEgmErroCountTimeStamp);
	}
}

WORD CSentinelState::GetEgmErrorCount() const
{
	return m_egmErrorCount;
}

void CSentinelState::SetPollerOnline(bool value)
{
	m_connectivityStatus.flags.isPollerOnline = value;
}

bool CSentinelState::IsPollerOnline() const
{
	return m_connectivityStatus.flags.isPollerOnline;
}

bool CSentinelState::IsSystemOnline() const
{
	return m_connectivityStatus.flags.isPollerOnline && 
		   m_connectivityStatus.flags.isTcpConnected && 
		   m_connectivityStatus.flags.isPMTUp && 
		   m_connectivityStatus.flags.isMMTUp;
}

bool CSentinelState::IsPollerReadyForSubgameList() const
{
	return m_connectivityStatus.IsPollerReadyForSubgameList();
}

bool CSentinelState::GetTcpConnected()
{
	return m_connectivityStatus.flags.isTcpConnected;
}

void CSentinelState::SetConnectivityStatus(WORD value)
{
	m_connectivityStatus.uValue = value;
}

bool CSentinelState::GetPmtUp()
{

	return m_connectivityStatus.flags.isPMTUp;
}

void CSentinelState::SetEmployeeGoodbyeEnabled(bool value)
{
	m_employeeGoodbyeEnabled = value;
}

bool CSentinelState::TransactionFillStatusOkay() const
{
	return m_transactionFillStatus == transactionsFillOkay;
}

void CSentinelState::SetTransactionFillStatus(TransactionsFillStatus fillStatus)
{
	m_transactionFillStatus = fillStatus;
}

TransactionsFillStatus CSentinelState::GetTransactionFillStatus() const
{
	return m_transactionFillStatus;
}

bool CSentinelState::MeterCaptureTimedOut() const
{
	return IsMeterCaptureInProgress() && CUtilities::GetTickCount() - m_snapshotStatusTickCount >= METER_CAPTURE_TIMEOUT;
}

bool CSentinelState::IsBackgroundSnapshotInProgress(int& meterCapturePercentage) const
{
	meterCapturePercentage = m_meterCapturePercentage;
	return (m_snapShotType != SnapshotNone && m_snapShotType != MpDropDoor && m_snapShotType != MpCashDoor) || m_keepMeterCaptureUp;
}

void CSentinelState::SetMeterChangeRequestType(MeterChangeRequestType type)
{
	m_meterChangeInProgress = type;
}

bool CSentinelState::IsMeterChangeInProgress() const
{
	return m_meterChangeInProgress != meterChangeRequestNone;
}

bool CSentinelState::IsMeterChangeMajor() const
{
	return m_meterChangeInProgress == meterChangeRequestMajor;
}

void CSentinelState::CheckIfCardInsertionIsToBeIgnored(CCard*& card) const
{
	int meterCapturePercentage(0);
	if ((card != nullptr && IsBackgroundSnapshotInProgress(meterCapturePercentage)) || IsUIRestartInProgress())
	{
		delete card;
		card = nullptr;
	}
}

void CSentinelState::SetVirtualPlayerFlag(bool value)
{
	m_virtualPlayer = value;
}

void CSentinelState::SetSkinInfo(const DWORD skinId, const string& skinName, const string& skinVersion, const DWORD skinRevision)
{
	m_skinId = skinId;
	m_skinName = skinName;
	m_skinVersion = skinVersion;
	m_skinRevision = skinRevision;
}

DWORD CSentinelState::GetSkinId() const
{
	return m_skinId;
}

string CSentinelState::GetSkinName() const
{
	return m_skinName;
}

string CSentinelState::GetSkinVersion() const
{
	return m_skinVersion;
}

DWORD CSentinelState::GetSkinRevision() const
{
	return m_skinRevision;
}

void CSentinelState::SetCampaignInfo(const DWORD campaignId, const std::string& campaignName, const DWORD campaignRevision)
{
	m_campaignId = campaignId;
	m_campaignName = campaignName;
	m_campaignRevision = campaignRevision;
}

DWORD CSentinelState::GetCampaignId() const
{
	return m_campaignId;
}

string CSentinelState::GetCampaignName() const
{
	return m_campaignName;
}

DWORD CSentinelState::GetCampaignRevision() const
{
	return m_campaignRevision;
}

void CSentinelState::MemoryClearSuccessful()
{
	m_memoryClearSuccessful = true;
}

string CSentinelState::GetUsbFilePath()
{
	bool moreUSBPaths = true;
	int usbIndex = 0;
	string usbFilePath;
	while (moreUSBPaths)
	{
		string tempPath(CDirectoryManager::Instance().USBPath(usbIndex));
		if (!tempPath.empty())
		{
			string dfCommand("df " + tempPath + " >/dev/null 2>&1");
			if (system(dfCommand.c_str()) == 0)
			{
				usbFilePath = tempPath;
				break;
			}
		}
		else
		{
			moreUSBPaths = false;
		}
		usbIndex++;
	}

	return usbFilePath;
}


string CSentinelState::ResponseForSentinelState() const
{
	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);

	writer.StartObject();

	writer.Key("PollerPacketVersion");	writer.Uint(m_pollerPacketVersion);
	writer.Key("TimeZoneOffset");		writer.Int(CUtilities::GetTzOffset());

	writer.Key("ConnectivityStatus");
	{
		writer.StartObject();
		writer.Key(TXT_MECHANIC_COMM_EGM.c_str());		writer.String(m_gameOnline ? TXT_MECHANIC_COMM_CONNECTED : TXT_MECHANIC_COMM_NOT_CONNECTED);
		writer.Key(TXT_MECHANIC_COMM_ERRORS.c_str());	writer.Uint(m_egmErrorCount);
		writer.Key(TXT_MECHANIC_COMM_POLLER.c_str());	writer.String(m_connectivityStatus.flags.isPollerOnline ? TXT_MECHANIC_COMM_CONNECTED : TXT_MECHANIC_COMM_NOT_CONNECTED);
		writer.Key(TXT_MECHANIC_COMM_TCPIP.c_str());	writer.String(m_connectivityStatus.flags.isTcpConnected ? TXT_MECHANIC_COMM_CONNECTED : TXT_MECHANIC_COMM_NOT_CONNECTED);
		if (m_connectivityStatus.flags.isTcpConnected)
		{
			writer.Key(TXT_MECHANIC_COMM_PMT.c_str());		writer.String(m_connectivityStatus.flags.isPMTUp ? TXT_MECHANIC_COMM_CONNECTED : TXT_MECHANIC_COMM_NOT_CONNECTED);
			writer.Key(TXT_MECHANIC_COMM_MMT.c_str());		writer.String(m_connectivityStatus.flags.isMMTUp ? TXT_MECHANIC_COMM_CONNECTED : TXT_MECHANIC_COMM_NOT_CONNECTED);
			if (m_pollerPacketVersion >= PACKET_VERSION_MULTI_GAME_MULTI_DENOM)
			{
				writer.Key(TXT_MECHANIC_COMM_VOUCHER_SERVICE.c_str());		writer.String(m_connectivityStatus.flags.isVoucherServiceUp ? TXT_MECHANIC_COMM_CONNECTED : TXT_MECHANIC_COMM_NOT_CONNECTED);
			}
			else
			{
				writer.Key(TXT_MECHANIC_COMM_VOUCHER_SERVICE.c_str());		writer.String(TXT_MECHANIC_COMM_VOUCHER_SERVICE_NA);
			}
		}
		else
		{
			writer.Key(TXT_MECHANIC_COMM_PMT.c_str());		writer.String(TXT_MECHANIC_COMM_PMT_NA);
			writer.Key(TXT_MECHANIC_COMM_MMT.c_str());		writer.String(TXT_MECHANIC_COMM_MMT_NA);
			writer.Key(TXT_MECHANIC_COMM_VOUCHER_SERVICE.c_str());		writer.String(TXT_MECHANIC_COMM_VOUCHER_SERVICE_NA);
		}
		writer.Key(TXT_MECHANIC_DISPLAY_YELLOW_COMM_DOWN.c_str());		writer.Bool(!m_gameOnline);
		writer.Key(TXT_MECHANIC_DISPLAY_RED_COMM_DOWN.c_str());		writer.Bool(!IsSystemOnline());
		writer.EndObject();
	}

	writer.Key("VirtualPlayer");	writer.Bool(m_virtualPlayer);

	writer.Key("IsOneLinkEnabled");	writer.Bool(m_oneLinkProgressivesEnabled);

	writer.Key("IsMobileConnectEnabled"); writer.Bool(m_mobileConnectCommEnabled);

	writer.Key("IsEmployeeGoodbyeEnabled");	writer.Bool(m_employeeGoodbyeEnabled && m_snapShotType == SnapshotNone);

	writer.EndObject();

	return writeStream.m_str;
}

string CSentinelState::ResponseForStatus() const
{
	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);

	writer.StartObject();
	{
		writer.Key("Version");			writer.String(m_sentinelVersion);
		writer.Key("Door");				writer.String(m_doorOpen ? TXT_OPEN : TXT_CLOSED);
		writer.Key("LastCoinsPlayed");	writer.Int(m_lastCoinsPlayed);
	}
	writer.EndObject();

	return writeStream.m_str;
}

string CSentinelState::ResponseForOneLinkConnection() const
{
	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);

	writer.StartObject();
	{
		lock_guard<recursive_mutex> lock(m_criticalSection);
		writer.Key("Info");
		{
			writer.StartObject();
			{
				writer.Key("ConnectionState");	writer.String(m_progressiveConnectionInfo.GetConnectionStateString());
				writer.Key("LastMessageIp");	writer.String(m_progressiveConnectionInfo.GetLastMessageIP().GetIP());
				writer.Key("LastMessageTime");	writer.String(m_progressiveConnectionInfo.GetLastMessageTimestampString());
				writer.Key("HostName");			writer.String(m_progressiveConfig.GetDNSLookup() ? m_progressiveHost.GetName() : TXT_NA);
				writer.Key("CurrentHostIp");	writer.String(m_progressiveHost.GetIPAddress().GetIP().empty() ? TXT_NA : m_progressiveHost.GetIPAddress().GetIP());
				writer.Key("HostList");
				{
					writer.StartArray();
					for (int index = 0; index < m_progressiveConfig.GetEndpointCount(); index++)
					{
						writer.StartObject();
						writer.Key("Host");		writer.String(m_progressiveConfig.GetEndpoint(index));
						writer.EndObject();
					}
					writer.EndArray();
				}
				writer.Key("HostIpAddresses");
				{
					writer.StartArray();
					for (size_t index = 0; index < m_progressiveHost.GetIPAddressCount(); index++)
					{
						writer.StartObject();
						writer.Key("HostIp");		writer.String(m_progressiveHost.GetIPAddress(index).GetIP());
						writer.EndObject();
					}
					writer.EndArray();
				}
			}
			writer.EndObject();
		}
	}

	writer.EndObject();

	return writeStream.m_str;
}

string CSentinelState::ResponseForDiskSpace()
{
	vector<tDiskSpace> vdiskSpace;
	
	AddDiskSpaceInfo(vdiskSpace, "SD Card1 (OS)", CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_OS));
	AddDiskSpaceInfo(vdiskSpace, "SD Card1 (SentinelData)", CDirectoryManager::SENTINEL_DATA_SD_PATH);
	AddDiskSpaceInfo(vdiskSpace, "SD Card2 (Content)", CDirectoryManager::CONTENT_SD_PATH);

	string usbPath(GetUsbFilePath());
	if (!usbPath.empty())
	{
		AddDiskSpaceInfo(vdiskSpace, "USB Drive", usbPath);
	}

	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);

	writer.StartObject();
	writer.Key(DISK_SPACE_TOPIC.c_str());
	{
		writer.StartArray();
		{
			for (const tDiskSpace& diskSpace : vdiskSpace)
			{
				writer.StartObject();
				{
					writer.Key("FileSystem");	writer.String(diskSpace.m_fileSystem);
					writer.Key("Size");			writer.String(diskSpace.m_size);
					writer.Key("Available");	writer.String(diskSpace.m_available);
					writer.Key("Used");			writer.String(diskSpace.m_used);
				}
				writer.EndObject();
			}
		}
		writer.EndArray();
	}
	writer.EndObject();

	return writeStream.m_str;
}

string CSentinelState::ResponseForDownloadState() const
{
	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);
	
	writer.StartObject();

	string downloadtype;
	switch (m_downloadStatus.GetType())
	{
		case FLASH_SYSTEM:
			downloadtype = "firmwarePackage";
		break;
		case FLASH_CONFIG:
			downloadtype = "miscellaneousFiles";
		break;
		case FLASH_CUSTOM_CONTENT:
			downloadtype = "customContent";
		break;
		case FLASH_CAMPAIGN_MEDIA:
			downloadtype = "campaignMedia";
		break;
		case FLASH_SKIN:
			downloadtype = "skinMedia";
		break;
		default:
			downloadtype = "none";
		break;
	}

	writer.Key("Type");	writer.String(downloadtype);
	
	writer.Key("Checksum");			writer.String(m_downloadStatus.GetChecksum());
	writer.Key("Percent");			writer.Int(m_downloadStatus.GetPercentComplete());
	writer.Key("LastReceivedTime");	writer.String(m_downloadStatus.GetTimestamp());
	writer.Key("LastReceivedFile");	writer.String(m_downloadStatus.GetFilename());
	writer.Key("PendingAction");	writer.String(GetPendingActionAsString());
	writer.Key("Status");			writer.String(m_downloadStatus.GetStatus());
	writer.EndObject();
	return writeStream.m_str;
}

string CSentinelState::ResponseForMeterCapture()
{
	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);

	writer.StartObject();

	string meterCaptureStatus;
	switch (m_meterCaptureStatus)
	{
	case CAPTURE_OK:
		meterCaptureStatus = "Okay";
		break;
	case CAPTURE_REBOOT:
		meterCaptureStatus = "Reboot";
		break;
	case CAPTURE_CANCELED:
		meterCaptureStatus = "Fail";
		break;
	default:
		break;
	}

	writer.Key("InProgress");	writer.Bool(m_meterCaptureInProgress);
	writer.Key("Status");		writer.String(meterCaptureStatus);
	writer.Key("Percentage");	writer.Int(m_meterCapturePercentage);
	writer.Key("Message");		writer.String(m_meterCaptureMessage);
	writer.Key("Warning");		writer.String(DEFAULT_WARNING_MESSAGE);

	writer.EndObject();

	if (m_snapShotType == SnapshotNone && m_meterCaptureInProgress)
	{
		m_meterCaptureInProgress = false;
	}

	return writeStream.m_str;
}

string CSentinelState::ResponseForMemoryClear() const
{
	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);
	 
	bool memoryClearKeyFound(false);

	string memoryClearPath(GetUsbFilePath() + MEMORY_CLEAR_TXT_FILE);
	if (CUtilities::FileExists(memoryClearPath))
	{
		ifstream ifs(memoryClearPath, std::ifstream::in);

		if (ifs.is_open())
		{
			string key;
			ifs >> key;
			if (MEMORY_CLEAR_KEY == key)
			{
				memoryClearKeyFound = true;
			}
			ifs.close();
		}
	}

	writer.StartObject();
	writer.Key("FileExists");	writer.Bool(memoryClearKeyFound);
	writer.Key("Successful");	writer.Bool(m_memoryClearSuccessful);
	writer.EndObject();

	return writeStream.m_str;
}

string CSentinelState::ResponseForGraphicCardContent() const
{
	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);

	writer.StartObject();

	writer.Key("SkinName");				writer.String(m_skinName);
	writer.Key("SkinRevision");			writer.Uint(m_skinRevision);
	writer.Key("CampaignName");			writer.String(m_campaignName);
	writer.Key("CampaignRevision");		writer.Uint(m_campaignRevision);

	writer.EndObject();


	return writeStream.m_str;
}

void CSentinelState::AddDiskSpaceInfo(std::vector<tDiskSpace>& vdiskSpace, const std::string& fileSytemName, const std::string& fileSytemPath)
{
	if (CUtilities::FileExists(fileSytemPath))
	{
		string size, used, available;
		CUtilities::GetPathMemoryUsageInformation(fileSytemPath, size, used, available);
		vdiskSpace.push_back(tDiskSpace(fileSytemName, size, available, used));
	}
}

void CSentinelState::SetDownloadStatus(const CFlashStatus &status)
{
	m_downloadStatus = status;
}

void CSentinelState::SetProgGames(CGames *games)
{
	if (games)
	{
		delete m_progGames;
		m_progGames = new CGames(*games);
	}
}


CGames *CSentinelState::GetProgGames()
{
	return m_progGames;
}

std::string CSentinelState::GetLastReceivedFile() const
{
	return m_downloadStatus.GetFilename();
}

std::string CSentinelState::GetLastReceivedTime() const
{
	return m_downloadStatus.GetTimestamp();
}

WORD CSentinelState::GetGameChunkIndexSent() const
{
	return m_gameChunkIndexSent;
}

void CSentinelState::SetGameChunkIndexSent(WORD gameChunkIndexSent)
{
	m_gameChunkIndexSent = gameChunkIndexSent;
}

WORD CSentinelState::GetTotalNumberOfGameChunks() const
{
	return m_numberOfGameChunks;
}

void CSentinelState::SetTotalNumberOfGameChunks(WORD numberOfGameChunks)
{
	m_numberOfGameChunks = numberOfGameChunks;
}

void CSentinelState::SetMeterRolloverConfig(bool meterRolloverSyncEnabled,
	BYTE meterRolloverTimeHour,
	BYTE meterRolloverTimeMinute,
	BYTE meterRolloverTimeSecond)
{
	m_meterRolloverSyncEnabled	= meterRolloverSyncEnabled;
	m_meterRolloverTimeHour		= meterRolloverTimeHour;
	m_meterRolloverTimeMinute	= meterRolloverTimeMinute;
	m_meterRolloverTimeSecond	= meterRolloverTimeSecond;
}

bool CSentinelState::PerformRolloverMeterCheck()
{
	bool performMeterRollover(false);

	if (!IsMeterCaptureInProgress() &&
		m_meterRolloverSyncEnabled &&
		INVALID_TIME != m_meterRolloverTimeHour &&
		INVALID_TIME != m_meterRolloverTimeMinute &&
		INVALID_TIME != m_meterRolloverTimeSecond)
	{
		// fetch current local time.
		SYSTEMTIME localTime, rolloverTime;
		CUtilities::GetLocalSystemTime(&localTime);
		rolloverTime = localTime;
		rolloverTime.wHour = m_meterRolloverTimeHour;
		rolloverTime.wMinute = m_meterRolloverTimeMinute;
		rolloverTime.wSecond = m_meterRolloverTimeSecond;
		time_t localTimeT = CUtilities::SysTimeToUlarge(localTime);
		time_t rolloverTimeT = CUtilities::SysTimeToUlarge(rolloverTime);

		if (m_canPerformMeterRollover)
		{
			//Add a 1 second buffer so that we can start 
			//the processing of meter capture because it takes time 
			//to send the message to SAS, there could also be a delay
			//due to other threads running.
			if (localTimeT + 1 >= rolloverTimeT)
			{
				performMeterRollover = true;
				m_canPerformMeterRollover = false;
				//Convert rollover time to UTC time then subract 1 to it
				//Oasis requires this so MachineRollover so that TX won't go into the next day.
				m_rolloverStartTime = CUtilities::GetUTCFromLocalTime(rolloverTimeT) - 1;
				LogString(POLLER_LOG, "Perform Rollover Meter localTime=%s m_rolloverStartTime=%s",
					CUtilities::GetStdStringFromSystemTime(localTime).c_str(),
					CUtilities::GetStdStringFromSystemTime(rolloverTime).c_str());
			}
		}
		else
		{
			//We won't be in this meter rollover window until the next day
			//To make sure we do the rollover somewhat on time we have to add 1 second buffer
			//to the current time so we can start to allow rollover 
			//as close as possible to rolloverTimeT the next time we come back to this function.
			if (rolloverTimeT >= localTimeT + 1)
			{
				LogString(POLLER_LOG, "Allow to perform rollover meter localTime=%s rolloverTime=%s",
					CUtilities::GetStdStringFromSystemTime(localTime).c_str(),
					CUtilities::GetStdStringFromSystemTime(rolloverTime).c_str());
				m_canPerformMeterRollover = true;
			}
		}
	}

	return performMeterRollover;
}

CFlashStatus CSentinelState::GetDownloadStatus() const
{
	return m_downloadStatus;
}