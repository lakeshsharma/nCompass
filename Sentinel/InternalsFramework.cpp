#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

#include "InternalsFramework.h"

// Sentinel includes.
#include "QueueList.h"
#include "Sentinel.h"
#include "Connection.h"
#include "IPAddress.h"
#include "Logging/LogString.h"
#include "Logging/Logger.h"
#include "UI/MediaWindowLog.h"
#include "UI/Logs.h"
#include "CheckSum.h"
#include "Hardware/WatchDog.h"
#include "DirectoryManager.h"
#include "Platform.h"
#include "Diagnostics.h"
#include "Config/ConfigExtFile.h"
#include "Utilities.h"
#include "CarrierUpgrade.h"
#include "VideoMixerUpgrade.h"
#include "PollerProtocol/PollerMGMDData.h"
#include "FileDownloader.h"

using namespace std;

// Declare the thread functions.
static UINT AFX_CDECL ReadGCFQueueThread( PVOID threadParam );
static UINT AFX_CDECL ReadSCFQueueThread( PVOID threadParam );
static UINT AFX_CDECL ReadUIQueueThread( PVOID threadParam );
static UINT AFX_CDECL ReadCardThread( PVOID threadParam );
static UINT AFX_CDECL ReadCardReaderThread( PVOID threadParam );
static UINT AFX_CDECL ReadProgressiveQueueThread(PVOID threadParam);
static UINT AFX_CDECL UpdatePollerWithSubgameMeters(PVOID threadParam);
static UINT AFX_CDECL ProgressiveCacheThread(LPVOID threadParam);

// 2 minutes
const DWORD POLLERPINGTIME = 120000;
const DWORD FIRMWARECHECK = 10000; // 10 seconds
const int NETWORK_CABLE_CHANGED_CHECK = 5; // 5 seconds
const LPCTSTR LCD_NA_STRING = "N/A PT Series";
const DWORD ALLOW_SPEED_MEDIA_STATUS_TO_BE_SENT(3000);
const DWORD TCP_MCR_PORT_NUMBER = 8079;
const DWORD MAINTENANCE_MODE_TIMEOUT_MSECS = 5 * MILLISECONDS_PER_MINUTE; // 5 minutes
const DWORD MEMORY_LOG_INTERVAL = 30000; // 30 seconds;

CInternalsFramework::CInternalsFramework(IProtocolManager *protocolManager, time_t startupTime, bool isLcd, const string& firmwareVersion) :
	m_gcfToInternalsQueue(nullptr),
	m_scfToInternalsQueue(nullptr),
	m_uiToInternalsQueue(nullptr),
	m_thirdPartyUIToInternalsQueue(nullptr),
	m_progressiveToInternalsQueue(nullptr),
	m_games(MEMORY_NVRAM),
	m_shutdownDumpMemory(false),
	m_config(isLcd, firmwareVersion),
	m_EGMConfig(MEMORY_EEPROM),
	m_egmAwardManager(m_progressiveLevelManager),
	m_sentinelState(startupTime, CUtilities::GetEXEVersion()),
	m_playerSession(MEMORY_NVRAM),
	m_pairingInformation(CPairingInformation::Instance()),
	m_mobilePBTInfo(CMobilePBTSessionInfo::Instance()),
	m_machineState(IN_THE_CLEAR),
	m_lastCashoutToHostSessionTickCount(0),
	m_countdownDisplayTickCount(0)
{
	m_protocolManager = protocolManager;

    m_lcdIsPresent = false;
	m_lcdDefaultSet = false;
	m_lcdGetStatusFailed = true;
	m_lcdMissingCount = 0;

	m_bPersistentObjectsInitialized = false;
	
	m_readCardReaderThread = nullptr;
	m_shutdownReadCardReader = false;
	m_readGCFThread = nullptr;
	m_shutdownReadGCF = false;
	m_readSCFThread = nullptr;
	m_shutdownReadSCF = false;
	
	m_readCardThread = nullptr;
	m_shutdownReadCard = false;	

	m_readUIThread = nullptr;
	m_shutdownReadUI = false;
	m_readThirdPartyUIThread = nullptr;
	m_shutdownReadThirdPartyUI = false;
	m_readProgressiveThread = nullptr;
	m_shutdownReadProgressive = false;

	m_updateMixerThread = nullptr;

	m_mcr = nullptr;
	m_tcpMcr = nullptr;

	m_bCardInserted = false;
	m_bIsEFT = false;
	m_hasPlayedSent = false;
	m_mediaWindowPlayerStatePending = MAINTAIN_PLAYER_STATE_PENDING;

	m_SCFInitialized = false;
	m_scfProtocolVersion = 0;

	m_progressiveCacheThread = nullptr;
	//m_progressiveCacheEventHandle = NULL;
	m_shutdownProgressiveCacheThread = false;

	m_player = NULL;
	m_playerGoodbyeRequested = false;
	m_employee1 = NULL;
	m_employee2 = NULL;
	m_employeeGoodbyeRequested = false;
	m_tickets = NULL;

    //Get timezone offset from nvram before loading personal banker logs from nvram
	m_config.BuildYourself();
	m_sentinelState.SetLastCoinsPlayed(m_config.GetLastCoinsPlayed());
	CSmartDataLogger::Instance().SetRemoteLoggingEnabledFlag(m_config.RemoteDebuggingEnabled());
	CSmartDataLogger::Instance().SetSlotMastId(m_config.GetSlotMastID());
	m_personalBanker = new CPersonalBanker(MEMORY_NVRAM);
	m_jackpot = NULL;

	m_tiltManager = NULL;

	m_firstTimeSet = false;

	m_bNvramValidAtStartup = true;

    m_bCurrentlyPerformingSnapshot = false;

    m_promotionMediaInPlay = false;

	m_pendingCoinIn = false;
	m_origCoinIn = 0;
	m_origMeters = NULL;
	m_updateMMT = false;
	m_bSendSubgameMetersInProgress = false;

	m_displayManager = &CDisplayManager::Instance();	

	m_firstPing = true;
	m_lastPollerPing = CUtilities::GetTickCount();
	m_lastVMPing = CUtilities::GetTickCount();
	m_IsItPollerLoadBalancingWindow = false;
	m_PollerLoadBalanceTime_Hour = 0xFF;
	m_PollerLoadBalanceTime_Minute = 0xFF;

	m_pendingMWConfigChange = false;

	m_enableEGM = false;
	m_finishedFirstPassForSubGames = false;
	m_finishedOfflineCheckForSubgames = false;
	m_bannedPlayerCardInserted = false;
	m_lastSessionID = -1;
	m_firmwareAvailable = false;
	m_mwConfig = CMediaWindowConfig::Instance();
	m_mediaPackageManager = &CMediaPackageManager::Instance();

	m_dataSubscriptionManager.RegisterProvider(&m_progressiveLevelManager);
	m_dataSubscriptionManager.RegisterProvider(&m_EGMConfig);
	m_dataSubscriptionManager.RegisterProvider(&m_config);
	m_dataSubscriptionManager.RegisterProvider(&m_sentinelState);
	m_dataSubscriptionManager.RegisterProvider(&m_games);
	m_dataSubscriptionManager.RegisterProvider(m_personalBanker);
	m_dataSubscriptionManager.RegisterProvider(m_mwConfig);
	m_dataSubscriptionManager.RegisterProvider(m_displayManager);
	m_dataSubscriptionManager.RegisterProvider(&m_notificationBox);
	m_dataSubscriptionManager.RegisterProvider(&m_playerSession);
	m_dataSubscriptionManager.RegisterProvider(&CSmartDataLogger::Instance());
	m_MGMDAvailable = m_MGMDGlobalOption = false;
	m_sendPlayerChangeEvent = false;
	m_beaconManager = nullptr;

	m_excessiveVoucherOut.SetGlobalExcessiveVoucherOutEnabledFlag(m_config.GetGlobalExcessiveVoucherOutEnabledFlag());
	m_excessiveVoucherOut.SetExcessiveVoucherOutThreshold(m_config.GetExcessiveVoucherOutThreshold());
	m_excessiveVoucherOut.SetTotalBillInThreshold(m_config.GetTotalBillInThreshold());
	m_excessiveVoucherOut.SetCoinInThreshold(m_config.GetCoinInThreshold());
	m_excessiveVoucherOut.SetCoinOutThreshold(m_config.GetCoinOutThreshold());

	m_currentSubgameMetersChanged = false;
	m_maintenanceMode = false;
	m_lastMaintenanceModeTickCount = 0;
	m_memoryLogTickCount = 0;
	m_campaignMemoryLogTickCount = 0;
	m_campaignCpuLogTickCount = 0;

	m_restartManager.SetRestartUIDelayHours(m_config.GetRestartUIDelayHours());

	CCard tempCard(CUtilities::GetCurrentTime(), CUtilities::GetUTCTime());
	tempCard.ClearLastCardReadIfNecessary(m_config);

	CFileDownloader::Instance().StartThread(this, m_mediaPackageManager);
	m_lastFirmwareCheckTickCount = CUtilities::GetTickCount();

	if (CMediaPackageManager::Instance().LoadActiveCampaignInfo())
	{
		CPackageInfo info = CMediaPackageManager::Instance().GetActiveCampaignInfo();
		m_sentinelState.SetCampaignInfo(info.GetId(), info.GetName(), info.GetRevision());
	}

	CMediaPackageManager::Instance().LoadActiveSkinInfo();
	CPackageInfo info = CMediaPackageManager::Instance().GetActiveSkinInfo();
	m_sentinelState.SetSkinInfo(info.GetId(), info.GetName(), info.GetVersion(), info.GetRevision());
}

CInternalsFramework::~CInternalsFramework(void)
{
	ShutdownThreads();
	UninitializeMembers();
	DeleteMembers();
	m_displayManager->PrepareDisplayForShutdown();
}

void CInternalsFramework::UninitializeMembers()
{
	// Only uninitialize these if we are debug so we can test for memory leaks on nice shutdown.
	// We don't want to unitiialize these normailly because it will clear these object from NVRAM (tracker 20165)
#ifdef _DEBUG	
	UninitializeEmployee();
	UninitializePlayer();
	UninitializeTickets();
	UninitializeJackpot();
#endif	

	UninitializeGCF();
	UninitializeSCF();
	UninitializeProgressive();
	UnitializeUI();
}

void CInternalsFramework::ShutdownThreads()
{
	m_shutdownNDI = true;
	m_shutdownReadGCF = true;
	m_shutdownReadCard = true;
	m_shutdownReadCardReader = true;
	m_shutdownReadSCF = true;
	m_shutdownReadUI = true;
	m_shutdownReadThirdPartyUI = true;
	m_shutdownReadProgressive = true;
	m_shutdownpolicySvr = true;
	m_shutdownDumpMemory = true;
	m_shutdownProgressiveCacheThread = true;

	//SPS added to speed up shutdown time
	if (m_gcfToInternalsQueue != nullptr)
	{
		m_gcfToInternalsQueue->GetAddedItemEventHandle().signal(false);
	}
	if (m_scfToInternalsQueue != nullptr)
	{
		m_scfToInternalsQueue->GetAddedItemEventHandle().signal(false);
	}
	if (m_uiToInternalsQueue != nullptr)
	{
		m_uiToInternalsQueue->GetAddedItemEventHandle().signal(false);
	}
	if (m_progressiveToInternalsQueue != nullptr)
	{
		m_progressiveToInternalsQueue->GetAddedItemEventHandle().signal(false);
	}
	if (m_ndiThread.get() != nullptr)
	{
		m_ndiThread->join();
		delete m_ndiThread.release();
	}
	if (m_readCardThread.get() != nullptr)
	{
		m_readCardThread->join();
		delete m_readCardThread.release();
	}
	if (m_readCardReaderThread.get() != nullptr)
	{
		m_readCardReaderThread->join();
		delete m_readCardReaderThread.release();
	}
	if (m_readGCFThread.get() != nullptr)
	{
		m_readGCFThread->join();
		delete m_readGCFThread.release();
	}
	if (m_readSCFThread.get() != nullptr)
	{
		m_readSCFThread->join();
		delete m_readSCFThread.release();
	}
	if (m_readUIThread.get() != nullptr)
	{
		m_readUIThread->join();
		delete m_readUIThread.release();
	}
	if (m_readThirdPartyUIThread.get() != nullptr)
	{
		m_readThirdPartyUIThread->join();
		delete m_readThirdPartyUIThread.release();
	}
	if (m_readProgressiveThread.get() != nullptr)
	{
		m_readProgressiveThread->join();
		delete m_readProgressiveThread.release();
	}
	if (m_policySvrThread.get() != nullptr)
	{
		m_policySvrThread->join();
		m_policySvrThread.release();
	}
	if (m_dumpMemoryThread.get() != nullptr)
	{
		m_dumpMemoryThread->join();
		delete m_dumpMemoryThread.release();
	}
	if (m_progressiveCacheThread.get() != nullptr)
	{
		m_progressiveCacheThread->join();
		delete m_progressiveCacheThread.release();
        m_progressiveCacheThread = nullptr;
	}
}

void CInternalsFramework::DeleteMembers()
{
	delete m_tiltManager;
	delete m_transLog;
	delete m_repairLog;
	delete m_attnPaidLog;
	delete m_beaconManager;

	#ifdef LEAK_CHECK
	delete m_personalBanker; // Don't delete m_personalBanker, as it effectively clears transfers in progress from persisted memory.
	#endif

	//m_progressiveCacheEventHandle.wait();
	delete m_gcfToInternalsQueue;
	delete m_scfToInternalsQueue;
	delete m_uiToInternalsQueue;
	delete m_progressiveToInternalsQueue;
	if (m_mwConfig != NULL)
	{
		delete m_mwConfig;
	}
	if (m_mcr != NULL)
	{
		delete m_mcr;
	}
	if (m_tcpMcr != nullptr)
	{
		delete m_tcpMcr;
	}
	delete m_jackpot;

	delete m_mediaPackageManager;

	if (m_mcrAuto != nullptr)
	{
		delete m_mcrAuto;
	}
}

void CInternalsFramework::BusyBoxBind()
{
	LogString(GENERAL, "Busy Box Bind");
	const string busyBoxBind("busybox httpd -p 8080 -h /opt/ncompass/media");
	if (system(busyBoxBind.c_str()) == 0)
	{
		LogString(GENERAL, "busybox httpd successful %s", busyBoxBind.c_str());
	}
	else
	{
		LogString(ERROR_LOG, "Error busybox httpd with command %s", busyBoxBind.c_str());
	}
}

void CInternalsFramework::Start(void)
{
	Start1();
	Start2();
	Start3();
}

void CInternalsFramework::Start1()
{
	{
		lock_guard<recursive_mutex> lock(m_playerCriticalSection);
		m_playerSession.UpdateConfigItems(m_config);

		bool sessionActive;
		bool cardedPlayer;
		int sessionId;
		m_playerSession.GetSessionActiveInfo(sessionActive, cardedPlayer, sessionId);
		m_progressiveLevelManager.SetSessionActiveInfo(sessionActive, sessionId);
	}

	m_mcrAuto = new CMCRAutoTest(m_config, CUtilities::GetTickCount(), COperatingSystem::m_operatingSystem);
	// Don't initialize m_mcr until m_config is valid.
	m_mcr = new CMCRWrapper(m_config);

	// Enable remote TCP/IP emulation of CardIn/CardOut if REMOTE_DEBUG file is present
	if (CUtilities::FileExists(CDirectoryManager::REMOTE_DEBUG_FILE))
	{
		m_tcpMcr = new CTcpMcrWrapper(m_config, TCP_MCR_PORT_NUMBER);
	}

	m_beaconManager = new CBeaconManager();

	m_EGMConfig.BuildYourself();

	{
        std::lock_guard<std::recursive_mutex> lock(m_egmAwardCriticalSection);
		m_egmAwardManager.Start(CUtilities::GetTickCount());
	}

	bool sessionActiveInfoChanged(false), creditsIncreased(false);
	CGame* game = m_games.GetEGM();
	__int64 meterValueInt64 = INVALID_METER_VALUE;
    if (game != nullptr) 
		meterValueInt64 = game->GetMeter(mCrd);
	DWORD meterValue(meterValueInt64 == INVALID_METER_VALUE ? 0 : DWORD(meterValueInt64));
	m_playerSession.SetCreditsInPennies(meterValue * m_config.GetDenomination(), sessionActiveInfoChanged, creditsIncreased);

	CDisplayManager::Instance(m_config.GetServiceWindowOpen(), *m_mwConfig, m_playerSession.HasSignificantCredits(), m_config.GetGameInProgress());
	m_displayManager->SetLastCardRead(m_config.GetLastCardRead());
    m_displayManager->SetDesktopRotation(m_config.GetOrientationType(), false);
}

void CInternalsFramework::Start2()
{
	m_bNvramValidAtStartup = CMemoryStorage::IntegrityCheck();
	
	if (!m_games.MemoryValid())
		m_config.SetMemoryLost(true);

	if (m_bNvramValidAtStartup)
	{
		// Did eeprom check fail?
		if (m_config.GetMemoryLost())
		{
			m_bNvramValidAtStartup = false;
		}
	}

	//TBD All of the initializations need to report if they cleared NVRAM
	//for the m_config.SetMemoryLost(true);

	m_tiltManager = new CTiltManager();

	m_transLog = new CLogs( MEMORY_NVRAM, LOG_TRANS, TRANSACTION_LOG);
	m_repairLog = new CLogs( MEMORY_NVRAM, LOG_FAILURE, REPAIR_LOG);
	m_attnPaidLog = new CLogs( MEMORY_NVRAM, LOG_ATT_PAID_PAYOUT, ATTENDANT_PAID_LOG);
	m_dataSubscriptionManager.RegisterProvider(m_attnPaidLog);
	m_dataSubscriptionManager.RegisterProvider(m_repairLog);
	m_dataSubscriptionManager.RegisterProvider(m_transLog);

	m_dataSubscriptionManager.RegisterProvider(m_tiltManager);

	// Set the timezone on the system
	SetTimezone();

	// Set up Network Adapter
	ConfigureNetworkAdapter(true);

	// Set Volume
	AdjustSpeakerVolume();

	// Set Backlight brightness
	AdjustScreenBrightness();


	// Initialize coin in at last play with current coin in meter
	InitializeLastCoinsPlayedAndWon();

	// extrct NCD files.
	CMediaPackageManager::Instance().ExtractNCDFiles();

	// UI_REWRITE_CLEANUP : temp code
	if (CUtilities::DoesFileExist("/sdboot/MEMORY_LOG"))
	{
		// calculate timestamp to append in filename.
		const DWORD _TIMESTAMP_SIZE = 128;
		char timeStamp[_TIMESTAMP_SIZE];
		tm tmpTm(CUtilities::GetLocalTmFromTime(CUtilities::GetCurrentTime()));
		size_t time_size(strftime(timeStamp, _TIMESTAMP_SIZE, "%Y_%m_%d_%I_%M_%S", &tmpTm));
		std::string timeStampStr(timeStamp, time_size);

		// filename
		m_memoryLogFilename = "/sdboot/MemoryLog_" + timeStampStr + ".txt";

		// write header
		const std::string tickCountStr(std::to_string(CUtilities::GetTickCount()));
		std::string commandStr = "free - m | grep 'total' | awk '{print \"" + tickCountStr + ": \" $0}' > " + m_memoryLogFilename;;
		system(commandStr.c_str());
	}

	LaunchCampaignMemoryLogIfReqd();
	LaunchCampaignCpuLogIfReqd();
}

void CInternalsFramework::Start3()
{
	// Set up the incoming message queues
	m_gcfToInternalsQueue = new CQueueList( GCF2INT_MessageQueueLogIdentifier );
	m_scfToInternalsQueue = new CQueueList( SCF2INT_MessageQueueLogIdentifier );
	m_uiToInternalsQueue = new CQueueList( SUI2INT_MessageQueueLogIdentifier );

	m_progressiveToInternalsQueue = new CInternalMessageQueueList( PROGINT_MessageQueueLogIdentifier );

	// Setup event handles before starting threads.
	//m_progressiveCacheEventHandle = CreateEvent(NULL, FALSE, FALSE, NULL);


	// Start the incoming queue threads.
	m_readGCFThread	= unique_ptr<std::thread>(new std::thread(ReadGCFQueueThread, this ));
	CWatchDog::Instance().RegisterThread(THREAD_READ_GCF_QUEUE, m_readGCFThread->get_id(), GCF_TO_INTERNALS_QUEUE_TIMEOUT, m_readGCFThread->native_handle());
	m_readSCFThread = unique_ptr<std::thread>(new std::thread(ReadSCFQueueThread, this ));    
	CWatchDog::Instance().RegisterThread(THREAD_READ_SCF_QUEUE, m_readSCFThread->get_id(), SCF_TO_INTERNALS_QUEUE_TIMEOUT, m_readSCFThread->native_handle());
	m_readUIThread = unique_ptr<std::thread>(new std::thread(ReadUIQueueThread, this ));
	CWatchDog::Instance().RegisterThread(THREAD_READ_UI_QUEUE, m_readUIThread->get_id(), UI_TO_INTERNALS_QUEUE_TIMEOUT, m_readUIThread->native_handle());
	m_readCardReaderThread = unique_ptr<std::thread>(new std::thread(ReadCardReaderThread, this));
	CWatchDog::Instance().RegisterThread(THREAD_READ_CARD_READER, m_readCardReaderThread->get_id(), CARD_READER_TIMEOUT, m_readCardReaderThread->native_handle());
	m_readProgressiveThread = std::unique_ptr<std::thread>(new std::thread(ReadProgressiveQueueThread, this));
	CWatchDog::Instance().RegisterThread(THREAD_READ_PROGRESSIVE_QUEUE, m_readProgressiveThread->get_id(), PROGRESSIVE_TO_INTERNALS_QUEUE_TIMEOUT,m_readProgressiveThread->native_handle());
	
	// Start other thread(s).
	m_progressiveCacheThread = std::unique_ptr<std::thread>(new std::thread(ProgressiveCacheThread, this));
	CWatchDog::Instance().RegisterThread(THREAD_PROGRESSIVE_CACHE_INTERNALS, m_progressiveCacheThread->get_id(), INTERNALS_PROGRESS_CACHE_TIMEOUT,m_progressiveCacheThread->native_handle());
	//Need to determine how this functionality will be implemented in Sentinel 5
	//start the memory dump thread

	m_dumpMemoryThread.reset(new thread(DumpMemoryThread, this));
	CWatchDog::Instance().RegisterThread(THREAD_DUMP_MEMORY, m_dumpMemoryThread->get_id(), DUMP_MEMORY_THREAD_TIMEOUT, m_dumpMemoryThread->native_handle());

	m_progressiveWaitingForSubGames = false;
}

// UI_REWRITE_CLEANUP : temp code
void CInternalsFramework::LaunchCampaignMemoryLogIfReqd()
{
	CPackageInfo info = CMediaPackageManager::Instance().GetActiveCampaignInfo();
	if (CUtilities::DoesFileExist("/sdboot/CAMPAIGN_MEMORY_LOG") && !info.GetName().empty())
	{
		// calculate timestamp to append in filename.
		const DWORD _TIMESTAMP_SIZE = 128;
		char timeStamp[_TIMESTAMP_SIZE];
		tm tmpTm(CUtilities::GetLocalTmFromTime(CUtilities::GetCurrentTime()));
		size_t time_size(strftime(timeStamp, _TIMESTAMP_SIZE, "%Y_%m_%d_%I_%M_%S", &tmpTm));
		std::string timeStampStr(timeStamp, time_size);

		// filename
		m_campaignMemoryLogFilename = "/sdboot/CampMemLog_" + std::to_string(info.GetId()) + "_" + info.GetName() + "_"+ timeStampStr + ".txt";
		CUtilities::FindAndReplace(m_campaignMemoryLogFilename, " ", "_");

		// write header
		const std::string tickCountStr(std::to_string(CUtilities::GetTickCount()));
		std::string commandStr = "free - m | grep 'total' | awk '{print \"" + tickCountStr + ": \" $0}' > " + m_campaignMemoryLogFilename;;
		system(commandStr.c_str());
	}
}

// UI_REWRITE_CLEANUP : temp code
void CInternalsFramework::LaunchCampaignCpuLogIfReqd()
{
	CPackageInfo info = CMediaPackageManager::Instance().GetActiveCampaignInfo();
	if (CUtilities::DoesFileExist("/sdboot/CAMPAIGN_CPU_LOG") && !info.GetName().empty())
	{
		// calculate timestamp to append in filename.
		const DWORD _TIMESTAMP_SIZE = 128;
		char timeStamp[_TIMESTAMP_SIZE];
		tm tmpTm(CUtilities::GetLocalTmFromTime(CUtilities::GetCurrentTime()));
		size_t time_size(strftime(timeStamp, _TIMESTAMP_SIZE, "%Y_%m_%d_%I_%M_%S", &tmpTm));
		std::string timeStampStr(timeStamp, time_size);

		// filename
		m_campaignCpuLogFilename = "/sdboot/CampCpuLog_" + std::to_string(info.GetId()) + "_" + info.GetName() + "_" + timeStampStr + ".txt";
		CUtilities::FindAndReplace(m_campaignCpuLogFilename, " ", "_");
	}
}

void CInternalsFramework::SendStartupMessageToUI( void )
{	
	m_protocolManager->WaitForProtocol(IProtocolManager::UIProtocol);

	// Set interruptible flag
	m_protocolManager->SetInboundQueueEqualPriorityInterrupt(IProtocolManager::UIProtocol, m_msgPriority.GetEqualPriorityInterrupt());
	
	// Add the start-up alert in the notification box.
	if (m_bNvramValidAtStartup)
	{
		m_notificationBox.AddAlert(Alerts::ALERT_REBOOTED_MEMORY_OK);
	}
	else
	{
		m_notificationBox.AddAlert(Alerts::ALERT_REBOOTED_MEMORY_ERROR);
	}

	// Start the display manager.	
	m_displayManager->SetMediaWindowConfig(*m_mwConfig);
	bool bShow = m_config.GetServiceWindowOpen();
	m_displayManager->Start(bShow);
	SendServiceWindowShowToUI(bShow);

	CPackageInfo info = CMediaPackageManager::Instance().GetActiveSkinInfo();
	m_sentinelState.SetSkinInfo(info.GetId(), info.GetName(), info.GetVersion(), info.GetRevision());
	SendSubscriptionUpdateIfNecessary();
	SendDashboardStatusToProgressive();
}

void CInternalsFramework::InitialzePersisentObjects( void )
{
	InitializePlayer();

	InitializeTickets();

	InitializeJackpot();

	m_bPersistentObjectsInitialized = true;
}

// This will get called when we need to start the GCF durring execution.
void CInternalsFramework::StartGCF( void )
{
	m_EGMConfig.SetEGMProtocolState( protocolStateEnabled );
	m_config.SetEGMProtocolState( protocolStateEnabled );
	LogString(SASGENERAL, "StartGCF() starting protocol");
	m_protocolManager->StartProtocol(IProtocolManager::EGMProtocol);
	InitializeGCF();
}

void CInternalsFramework::StartProcessingCards()
{
	m_readCardThread = unique_ptr<std::thread>(new std::thread(ReadCardThread, this ));
	CWatchDog::Instance().RegisterThread(THREAD_READ_CARD, m_readCardThread->get_id(), CARD_READER_TIMEOUT, m_readCardThread->native_handle());
}

void CInternalsFramework::ProcessSentinelChangeInProgressBeforeEGMStart( void )
{
	bool bStartGCF = false;
	bool bReboot = false;

	// If we shutdown while we were trying to do an online change...
	SentinelOnlineState onlineState = m_EGMConfig.GetSentinelOnlineState();
	switch( onlineState )
	{
		// If we were doing an Online, there are many states we could have been at the time of a shutdown
		case sentinelComingOnline:
		{
			// If pending slotmast, send a slotmast request.
			/// This will do a meter capture if a successful slotmast is returned.
			if ( m_config.GetPendingSlotmast() != INVALID_SLOT_MAST_ID )
			{
				SendSlotmastChangeToSCF();
			}

			// Assume that we have a cool slotmast, Start up the GCF so it will do a meter capture.
			else if ( m_config.GetSlotMastID() != INVALID_SLOT_MAST_ID )
			{
				bStartGCF = true;
			}

			// The user never entered a slot mast. Allow the user to go back into the online screen.
			else
			{
				m_EGMConfig.SetSentinelOnlineState( sentinelOffline );
			}
			break;
		}

		// If we shut down while completing an EGM Move, we need to start the GCF so the meters can be captured.
		case sentinelComingOnlineMove:
			bStartGCF = true;
			break;

		// If we shutdown while doing an offline or a move and we no longer have an EGM protocol, all we can do is reboot 
		//  once the SCF is finished doing its stuff.
		case sentinelGoingOffline:
		case sentinelGoingOfflineMove:
		case sentinelGoingOfflineDownload:
		default:

			// Do nothing here

			break;
	}

	// Start the GCF if we need to
	if (bStartGCF && m_EGMConfig.GetEGMProtocolState() != protocolStateEnabled)
	{
		StartGCF();
	}

	// Start the rebooting process if we need to.
	else if ( bReboot )
	{
		BeginRebootProcess();
	}
}

// This will process the EGM change if there was one in progress on last shutdown.
void CInternalsFramework::ProcessSentinelChangeInProgressAfterEGMStart( void )
{
	SnapshotType meterCaptureToPerform = SnapshotNone;

	switch ( m_EGMConfig.GetSentinelOnlineState() )
	{
		case sentinelComingOnline:
			// We can only come online if we got a config and we have a slotmast. tracker 17994
			if ( m_sentinelState.IsPollerOnline() && m_config.GetSlotMastID() != INVALID_SLOT_MAST_ID  )
			{
				// If the GCF is initialized, assume we need to capture initial meters
				meterCaptureToPerform = InitialMeters;
			}
			break;

		case sentinelGoingOffline:
		case sentinelGoingOfflineDownload:
			// If the GCF is initialized, assume we need to capture final meters
			meterCaptureToPerform = FinalMeters;
			break;

		case sentinelGoingOfflineMove:
			// If the GCF is initialized, assume we need to capture begin move meters
			meterCaptureToPerform = MachineMoveBegin;
			break;

		case sentinelComingOnlineMove:
			meterCaptureToPerform = MachineMoveEnd;
			break;

		default:
			break;
	}

	if ( meterCaptureToPerform != SnapshotNone )
	{
		BeginMeterCapture(meterCaptureToPerform, true, sentinelComingOnline == m_EGMConfig.GetSentinelOnlineState());
	}
}

void CInternalsFramework::BeginMeterCapture(SnapshotType type, bool keepMeterCaptureDisplayUpTilFinished, bool doAllGames)
{
	if (type != m_sentinelState.GetMeterCaptureType())
	{
		m_sentinelState.BeginMeterCapture(type, keepMeterCaptureDisplayUpTilFinished);
		SendMessageToGCF(new CInternalMessage(INT_MSG_START_CAPTURING_METERS, (WORD)type, (WORD)doAllGames), QUEUE_PRIORITY_NORMAL);
	}
}

void CInternalsFramework::BeginRebootProcess( void )
{
	// By sending this request, the SCF will notify us when the resend list is empty.
	// When we get this response, we know it is time to reboot.
	LogString(GENERAL, "Begin reboot process");
	SendMessageToSCF(new CInternalMessage(INT_MSG_NOTIFY_RESEND_EMPTY, (WORD) m_restartManager.GetRemoteRebootASAP()), QUEUE_PRIORITY_NORMAL);
}

void CInternalsFramework::PerformControlledRebootCheck()
{
	CGame *game = m_games.GetGame(EGM_GAME_NUMBER);
	lock_guard<recursive_mutex> lock(m_playerCriticalSection);
	bool isOSUpgrade = false;

	//Get Old shutdown
	bool old_isShutdownInProcess = m_restartManager.IsShutdownInProcess();
	if (!m_pairingInformation.IsWaitingForPairing() && 
		!m_pairingInformation.IsWaitingForRequestCardChange() &&
		!m_pairingInformation.IsMobileCardedSession() &&
		m_restartManager.PerformControlledRebootCheck(m_player,
		m_employee1,
		m_playerSession.GetLastCreditMeterZeroTime(),
		(game != NULL && game->IsDoorOpen(SlotDoor)),
		m_playerSession.HasSignificantCredits(),
		m_sentinelState.IsMeterCaptureInProgress(),
		m_config.GetRebootUiRestartOverrideEnabled(),
		isOSUpgrade))
	{
		bool new_isShutdownInProcess = m_restartManager.IsShutdownInProcess();
		if ( (old_isShutdownInProcess != new_isShutdownInProcess) && (true == new_isShutdownInProcess))
		{
			if (isOSUpgrade)
			{
				m_sentinelState.SetCurrentActivity(CurrentActivity::Upgrading);
				SendDashboardStatusToProgressive();
			}
			else
			{
				m_sentinelState.SetCurrentActivity(CurrentActivity::Rebooting);
				SendDashboardStatusToProgressive();

				// Send the reboot transaction.
				SendTransactionMessageToSCF(PrepareTransactionMessage(INT_MSG_TRANSACTION_CONTROLLED_REBOOT, IsPlayerVirtual()), QUEUE_PRIORITY_NORMAL);
			}
		}
		m_notificationBox.AddAlert(Alerts::ALERT_REBOOT_IN_PROGRESS);
		SendSubscriptionUpdateIfNecessary();

		CUtilities::Sleep(ALLOW_SPEED_MEDIA_STATUS_TO_BE_SENT);

		// Start the Reboot process
		BeginRebootProcess();
	}
	
	if (m_restartManager.IsSendRemoteRebootPendingTilt())
	{
		GenerateTilt(tltRebootRemotePending);
		m_restartManager.SetSendRemoteRebootPendingTilt(false);
	}
}

// 2012-02-28 -- Under some conditions this function is a called about once every 5 seconds.
void CInternalsFramework::PerformGeneralTimedEvents( void )
{
	// See if it is time to check the battery.
	time_t currentTime = CUtilities::GetCurrentTime();
	time_t oneDay(1 * SECONDS_PER_DAY);
	time_t dayInThePast = currentTime - oneDay;

	if ( m_config.GetLastBatteryCheckTime() < dayInThePast )
	{
		PerformBatteryTest();
	}

	if (m_sentinelState.MeterCaptureTimedOut())
	{
		MeterSnapshotEnd( true );
	}
	
	// See if firmware update available
	DWORD currentTickCount = CUtilities::GetTickCount();
	if (currentTickCount - m_lastFirmwareCheckTickCount > FIRMWARECHECK)
	{
		SetFirmwareVersion();
		m_lastFirmwareCheckTickCount = currentTickCount;
	}

	// See if unattended jackpot has timed out.
	PerformUnattendedJackpotCheck();

	// See if it is time to do a controlled reboot.
	PerformControlledRebootCheck();
	
	// See if it is time to do a controlled UI restart
	PerformControlledUIRestartCheck();

	// See if it is time to do a controlled UI refresh
	PerformControlledUIRefreshCheck();
	
	SendSubscriptionUpdateIfNecessary();

	SendMGMDupdateIfNecessary();

	CheckInWithPoller();

    CheckForMetersSend();

    NetworkUtils::Instance().NetWorkSvcRestartIfNetworkCableChanged();
	
	CheckBeaconConnectionStatus();
	
	if (m_pairingInformation.CheckMobilePairingTimeOut())
	{
		HandleMobilePairingTimeOut();
	}
	
	if (m_pairingInformation.CheckMobileRequestCardChangeTimeOut())
	{
		HandleMobileRequestCardChangeTimeOut();
	}

	if ((m_PollerLoadBalanceTime_Hour != 0xFF) && (m_PollerLoadBalanceTime_Minute != 0xFF))
	{
		PerformPollerLoadBalancing();
	}

	if (IsNFDPoller())
	{
		PerformRolloverMeterCheck();
	}	

	if (m_maintenanceMode && ((CUtilities::GetTickCount() - m_lastMaintenanceModeTickCount) >
		MAINTENANCE_MODE_TIMEOUT_MSECS))
	{
		LogString(GAMES, "[%s]: Exiting maintenance mode", __FUNCTION__);
		m_maintenanceMode = false;
	}

	UpdateCountdownDisplay();

	m_progressiveLevelManager.CheckTimedEvents(currentTime);
	SendSubscriptionUpdateIfNecessary();

	// UI_REWRITE_CLEANUP : temp code
	if (!m_memoryLogFilename.empty())
	{
		DWORD currentTickCount = CUtilities::GetTickCount();
		if (currentTickCount - m_memoryLogTickCount > MEMORY_LOG_INTERVAL)
		{
			m_memoryLogTickCount = currentTickCount;
			const std::string tickCountStr(std::to_string(currentTickCount));

			std::string commandStr = "free - m | grep 'Mem' | awk '{print \"" + tickCountStr + ": \" $0}' >> " + m_memoryLogFilename;
			system(commandStr.c_str());
		}
	}

	// UI_REWRITE_CLEANUP : temp code
	if (!m_campaignMemoryLogFilename.empty())
	{
		DWORD currentTickCount = CUtilities::GetTickCount();
		if (currentTickCount - m_campaignMemoryLogTickCount > MEMORY_LOG_INTERVAL)
		{
			m_campaignMemoryLogTickCount = currentTickCount;
			const std::string tickCountStr(std::to_string(currentTickCount));

			std::string commandStr = "free - m | grep 'Mem' | awk '{print \"" + tickCountStr + ": \" $0}' >> " + m_campaignMemoryLogFilename;
			system(commandStr.c_str());
		}
	}

	// UI_REWRITE_CLEANUP : temp code
	if (!m_campaignCpuLogFilename.empty())
	{
		DWORD currentTickCount = CUtilities::GetTickCount();
		if (currentTickCount - m_campaignCpuLogTickCount > MEMORY_LOG_INTERVAL)
		{
			m_campaignCpuLogTickCount = currentTickCount;
			const std::string tickCountStr(std::to_string(currentTickCount));

			std::string commandStr = "echo \"" + tickCountStr + ": -------\" >> " + m_campaignCpuLogFilename +
				" | top -n 1 | head -n 10 >> " + m_campaignCpuLogFilename;
			system(commandStr.c_str());
		}
	}
}

void CInternalsFramework::SendMessageToGCF( CInternalMessage *msg, int priorityLevel )
{
	m_protocolManager->SendMessageToProtocol(IProtocolManager::EGMProtocol, msg, priorityLevel);
}

void CInternalsFramework::SendMessageToSCF( CInternalMessage *msg, int priorityLevel )
{
	m_protocolManager->SendMessageToProtocol(IProtocolManager::PollerProtocol, msg, priorityLevel);
}

void CInternalsFramework::SendMessageToProgressive(CInternalMessage *msg, int priorityLevel)
{
	m_protocolManager->SendMessageToProtocol(IProtocolManager::ProgressiveProtocol, msg, priorityLevel);
}

void CInternalsFramework::SendTransactionMessageToSCF( CInternalMessage *msg, int priorityLevel )
{
	TransactionType transType = NONE_SPECIFIED;
	__int64 amount = 0;
	

	switch( msg->GetMessageType() )
	{
		case INT_MSG_TRANSACTION_BILL_ACCEPTED:
			transType = BILL_ACCEPTED_CODE;
			break;
		case INT_MSG_TRANSACTION_BONUS_EVENTS:
			break;
		case INT_MSG_TRANSACTION_CASH_TRANSFER:
			transType = CASH_TRANSFER_TRACKING_CODE;
			break;
		case INT_MSG_TRANSACTION_CONFIG_CHANGE:
			transType = CONFIG_CHANGE_CODE;
			break;
		case INT_MSG_TRANSACTION_PD_CHASSIS_OPEN:
			transType = CHASSIS_DOOR_OPEN_POWER_DOWN_CODE;
			break;
		case INT_MSG_TRANSACTION_FAILURE_CODE:
			transType = FAILURE_CODE;
			break;
		case INT_MSG_TRANSACTION_ILLEGAL_DOOR_OPEN:
			transType = ILLEGAL_DOOR_OPEN_CODE;
			break;
		case INT_MSG_TRANSACTION_CHANGE_LIGHT_ON:
			transType = CHANGE_LIGHT_ON_CODE;
			break;
		case INT_MSG_TRANSACTION_CHANGE_LIGHT_OFF:
			transType = CHANGE_LIGHT_OFF_CODE;
			break;
		case INT_MSG_TRANSACTION_JACKPOT:
			{
				CJackpot *jackpot = msg->GetJackpotData();
				transType = DetermineJackpotTransType(jackpot);
				amount = jackpot->GetJackpotAmount();
			}
			break;
		case INT_MSG_TRANSACTION_JACKPOT_FILL:
			transType = JACKPOT_FILL_CODE;
			amount = msg->GetInt64Data();
			break;
		case INT_MSG_TRANSACTION_MACHINE_ONLINE:
			transType = MACHINE_ONLINE_CODE;
			break;
		case INT_MSG_TRANSACTION_MACHINE_OFFLINE:
			transType = MACHINE_OFFLINE_CODE;
			break;
		case INT_MSG_TRANSACTION_MACHINE_MOVE_BEGIN:
			transType = BEGIN_MACHINE_MOVE_CODE;
			break;
		case INT_MSG_TRANSACTION_MACHINE_MOVE_COMPLETE:
			transType = MACHINE_MOVE_COMPLETE_CODE;
			break;
		case INT_MSG_TRANSACTION_MAINTENANCE:
			transType = MAINTENANCE_CODE;
			break;
		case INT_MSG_TRANSACTION_METER_RUNAWAY:
			transType = METER_RUNAWAY_CODE;
			break;
		case INT_MSG_TRANSACTION_PB:
			{
				transType = PERSONAL_BANKER_CODE;
				CPersonalBankerTransfer * msgPBT = msg->GetPBTransferData();
				amount = msgPBT->GetAmount();
			}
			break;
		case INT_MSG_TRANSACTION_PB_AUTHORIZE:
			{
				transType = PERSONAL_BANKER_CODE;
				CPersonalBankerTransfer * msgPBT = msg->GetPBTransferData();
				amount = msgPBT->GetAmount();
			}
			break;
		case INT_MSG_TRANSACTION_SLOT_POWER_OFF:
			transType = SLOT_POWER_OFF_CODE;
			break;
		case INT_MSG_TRANSACTION_SLOT_POWER_ON:
			transType = SLOT_POWER_ON_CODE;
			break;
		case INT_MSG_TRANSACTION_SNAPSHOT:
			{
				// gamenumber is in word data - if it's zero it's Tx 121, any other its Tx 122
				WORD gameNumber = msg->GetWordData();
				transType = (gameNumber == 0 ? EGM_SNAPSHOT_CODE : SUBGAME_SNAPSHOT_CODE);
			}
			break;
		case INT_MSG_TRANSACTION_TICKET:
			{
				CTicket *transTicket = msg->GetTicketData();
				transType = TICKET_CODE;
				amount = transTicket->Amount();
			}
			break;
		case INT_MSG_TRANSACTION_SENTINEL_ON:
			if ( msg->GetWordData() == NVRAM_MEMORY_BAD )
			transType = SENTINEL_ON_MEM_BAD_CODE;
			else
			transType = SENTINEL_ON_MEM_OK_CODE;			
		break;
		case INT_MSG_TRANSACTION_GAME_INFO:
			transType = GAME_INFO_CODE;
			break;
		case INT_MSG_TRANSACTION_VIRTUAL_SESSION_STARTED:
			transType = VIRTUAL_SESSION_STARTED;
			break;
		case INT_MSG_TRANSACTION_PLAYER_CARD_IN:
			transType = PLAYER_CARD_IN_CODE;
		break;
		case INT_MSG_TRANSACTION_PLAYER_CARD_OUT:
			transType = PLAYER_CARD_OUT_CODE;
		break;
		case INT_MSG_TRANSACTION_PLAYER_UPDATE:
			transType = PLAYER_UPDATE_CODE;
		break;
		case INT_MSG_DOOR_OPEN:
			switch (msg->GetWordData()) 
			{
				case mSDr:
					transType = SLOT_DOOR_OPEN_CODE;
					break;
				case mDDr:
					transType = DROP_DOOR_OPEN_CODE;
					break;
				case mLDr:
					transType = LOGIC_DOOR_OPEN_CODE;
					break;
				case mCDr:
					transType = CASH_DOOR_OPEN_CODE;
					break;
				case mFDr:
					transType = FILL_DOOR_OPEN_CODE;
					break;
				case mBDr:
					transType = BELLY_DOOR_OPEN_CODE;
					break;
				case mDr:
					transType = DOOR_OPEN_CODE;
					break;
				default:
					break;
			}
		break;
		case INT_MSG_DOOR_CLOSED:
			switch (msg->GetWordData()) 
			{
				case mSDr:
					transType = SLOT_DOOR_CLOSED_CODE;
					break;
				case mDDr:
					transType = DROP_DOOR_CLOSED_CODE;
					break;
				case mLDr:
					transType = LOGIC_DOOR_CLOSED_CODE;
					break;
				case mCDr:
					transType = CASH_DOOR_CLOSED_CODE;
					break;
				case mFDr:
					transType = FILL_DOOR_CLOSED_CODE;
					break;
				case mBDr:
					transType = BELLY_DOOR_CLOSED_CODE;
					break;
				case mDr:
					transType = DOOR_CLOSED_CODE;
					break;
				default:
					break;
			}
		break;
		case INT_MSG_TRANSACTION_EMPLOYEE_CARD_IN:
			switch ( msg->GetEmployeeData()->GetEmployeeCardType() ) 
			{
				case CARD_MECHANIC:
					transType = MECHANIC_CARD_IN_CODE;
					break;
				case CARD_GLOBAL:
					transType = GLOBAL_CARD_IN_CODE;
					break;
				case CARD_FLOOR:
					transType = FLOOR_CARD_IN_CODE;
					break;
				case CARD_CURRENCY:
					transType = CURRENCY_CARD_IN_CODE;
					break;			
				case CARD_METER:
					transType = METER_CARD_IN_CODE;
					break;
				default:
					break;
			}
		break;
		case INT_MSG_TRANSACTION_EMPLOYEE_CARD_OUT:
			switch ( msg->GetEmployeeData()->GetEmployeeCardType() ) 
			{
				case CARD_MECHANIC:
					transType = MECHANIC_CARD_OUT_CODE;
					break;
				case CARD_GLOBAL:
					transType = GLOBAL_CARD_OUT_CODE;
					break;
				case CARD_FLOOR:
					transType = FLOOR_CARD_OUT_CODE;
					break;
				case CARD_CURRENCY:
					transType = CURRENCY_CARD_OUT_CODE;
					break;			
				case CARD_METER:
					transType = METER_CARD_OUT_CODE;
					break;
				default:
					break;
			}
		break;
		case INT_MSG_SEND_TILT:
			transType = COMPOSITE_TILT;
		break;

		case INT_MSG_SEND_STACKER_OUT:
			transType = STACKER_REMOVED_CODE;
			break;
		
		case INT_MSG_SEND_STACKER_IN:
			transType = STACKER_INSTALLED_CODE;
			break;
		
		case INT_MSG_SEND_WITNESS:
			transType = WITNESS_CODE;
			break;

		case INT_MSG_BAD_CARD_READ:
			transType = POSSIBLE_BAD_CARD_READER_CODE;
			break;
		
		case INT_MSG_CHANGE_SLOTMASTID:
			transType = CHANGE_SLOTMAST_ID_CODE;
			break;

		case INT_MSG_TRANSACTION_CONTROLLED_REBOOT_WARNING:
			transType = CONTROLLED_REBOOT_WARNING_CODE;
			break;

		case INT_MSG_TRANSACTION_CONTROLLED_REBOOT:
			transType = CONTROLLED_REBOOT_CODE;
			break;

		case INT_MSG_TRANSACTION_AUTO_UPLOAD_CANCELED:
			transType = AUTO_UPLOAD_CANCELED_BY_USER;
			break;
		case INT_MSG_SEND_EXTERNAL_BONUS:
			transType = EXTERNAL_BONUS_CODE;
			break;
		case INT_MSG_UJP_CHECK_PIN:
			transType = AUTHORIZE_PIN;
			break;
		case INT_MSG_UNATTENDED_JACKPOT:
			transType = UNATTENDED_JACKPOT_RESULT;
			break;
		case INT_MSG_TRANSACTION_GAME_SESSION_CHANGE:
			transType = GAME_SESSION_CHANGE;
			break;
		case INT_MSG_TRANSACTION_ONELINK:
		case INT_MSG_TRANSACTION_GENERIC:
			transType = (TransactionType)msg->GetWordData();
			break;

		case INT_MSG_TRANSACTION_CARDLESS_SESSION_START:
			transType = CARDLESS_SESSION_START_CODE;
			break;

		case INT_MSG_TRANSACTION_CARDLESS_SESSION_END:
			transType = CARDLESS_SESSION_END_CODE;
			break;

		case INT_MSG_TRANSACTION_PROXIMITY_REQUEST_BEACON_SCAN_LIST:
			transType = PROXIMITY_REQUEST_BEACON_SCAN_LIST;
			break;

		default:
			break;
	}

	m_transLog->Add(new CTransactionLog(transType, amount, *msg->GetTimeData()));

	// We want to sit here until this queue is set up.
	m_protocolManager->WaitForProtocol(IProtocolManager::PollerProtocol);

	m_protocolManager->SendMessageToProtocol(IProtocolManager::PollerProtocol, msg, priorityLevel);
}

void CInternalsFramework::SendSentinelOnMessages( void )
{
	// Send tilt code from last reboot
	TiltCode rebootTilt = CDiagnostics::Instance().GetRebootTiltCode();
	if (tltUnknown == rebootTilt)
	{
		rebootTilt = tltRebootUnknown;
	}
	m_sentinelState.m_rebootTilt = rebootTilt;

	if (tltRebootUnknown == rebootTilt)
	{
		// If the code is tltRebootUnknown then the code has not been reported to the (Error.txt) file.  So, report it now.
		CDiagnostics::ReportRebootToFile(CDirectoryManager::RebootDiagnosticsFilePath(), rebootTilt, GENERAL);
	}

	GenerateTilt(rebootTilt);
	CDiagnostics::Instance().ClearReboot(); // Clear reboot tilt code AFTER saving the transaction.

	bool bMemoryLost = m_config.GetMemoryLost();

	//Send the Sentinel On (tx1) or Sentinel On - Memory OK (tx42) transactions.
	CInternalMessage *SentinelOnMessage = PrepareTransactionMessage(INT_MSG_TRANSACTION_SENTINEL_ON, IsPlayerVirtual());
	if ( bMemoryLost )
	{
		SentinelOnMessage->SetData(	NVRAM_MEMORY_BAD );
		SendTransactionMessageToSCF(SentinelOnMessage, QUEUE_PRIORITY_NORMAL);

		//send the GameInfo(Send Config) message for historic reasons
		CInternalMessage *transMsg = PrepareTransactionMessage( INT_MSG_TRANSACTION_GAME_INFO, IsPlayerVirtual() );
		transMsg->SetData( (WORD)m_config.GetGameDenom(), (WORD)m_EGMConfig.GetEGMType() );
		transMsg->SetData( (DWORD)m_config.GetSlotNumber() );
		SendTransactionMessageToSCF( transMsg, QUEUE_PRIORITY_NORMAL);
	}
	else
	{
		SentinelOnMessage->SetData(	NVRAM_MEMORY_OK );
		SendTransactionMessageToSCF(SentinelOnMessage, QUEUE_PRIORITY_NORMAL);
	}

	if (!CPlatform::Instance().CheckIfTouchScreenWorksWithTheCurrentOS())
	{
		GenerateTilt(tltTouchscreenDoesNotWorkWithCurrentOS);
	}
	
	//and reset the memory lost
	m_config.SetMemoryLost(false);
}

void CInternalsFramework::SendSnapshotTransactionToSCF( WORD gameNumber )
{	
	bool bPlayerIsVirtual = IsPlayerVirtual();

	time_t *transactionTime = new time_t(m_sentinelState.GetMeterCaptureStartTime());

	CMeters *transactionMeters = nullptr;
	CGame *subGame = nullptr;

	if ((gameNumber == EGM_GAME_NUMBER) && !IsNFDPoller())
	{
		// We want all the 91 meters in EGM meter snapshot 
		// Tx for non-NFD pollers
		transactionMeters = new CMeters(MEMORY_NONE, true);
		CGame *game = m_games.GetEGM();

		// Get ALL the game meters for transaction meters
		for (byte meterNum = mNone + 1; meterNum < mMeterLast; meterNum++)
		{
			transactionMeters->SetMeter(meterNum, (game != nullptr ? 
				game->GetMeter(meterNum) : INVALID_METER_NUMBER));
		}
	}
	else if (gameNumber != EGM_GAME_NUMBER)
	{
		// Get the subgame object if its subgame snapshot
		CGame *game = m_games.GetGame( gameNumber );
		
		if (game != nullptr)
		{
			subGame = new CGame(*game);
		}
		else
		{
			subGame = new CGame(MEMORY_NONE);
		}

		// Force the subgame to have all meters.
		for (byte meterNum = 0; meterNum < SUBGAME_METER_LAST; meterNum++)
		{
			if (subGame->GetMeter(meterNum) == INVALID_METER_NUMBER)
			{
				// This forces the meter object to exist
				subGame->SetMeter(meterNum, INVALID_METER_NUMBER);
			}
		}
	}

	CInternalMessage *transMsg = PrepareTransactionMessage(INT_MSG_TRANSACTION_SNAPSHOT, 
		bPlayerIsVirtual, transactionTime, transactionMeters);
	transMsg->SetData( gameNumber, (WORD)m_sentinelState.GetMeterCaptureType());
	LogString(GAMES, "Sending snapshot for game %d of type %d\n", gameNumber, m_sentinelState.GetMeterCaptureType());

	// If we have a subgame object, add it to the message.
	if (subGame != nullptr)
	{
		// The subgame has to be sent wrapped in a games object,
		CGames *games = new CGames();
		games->AddGame(subGame);
		transMsg->SetData(games);
	}

	// Ship it.
	SendTransactionMessageToSCF( transMsg, QUEUE_PRIORITY_NORMAL);
}


void CInternalsFramework::SendSentinelChangeTransactionToSCF( void )
{
	InternalMsgType msgType = INT_MSG_NONE;

	switch ( m_EGMConfig.GetSentinelOnlineState() )
	{
	case sentinelComingOnline:
		msgType = INT_MSG_TRANSACTION_MACHINE_ONLINE;
		break;
	case sentinelGoingOffline:
	case sentinelGoingOfflineDownload:
		msgType = INT_MSG_TRANSACTION_MACHINE_OFFLINE;
		break;
	case sentinelGoingOfflineMove:
		msgType = INT_MSG_TRANSACTION_MACHINE_MOVE_BEGIN;
		break;
	case sentinelComingOnlineMove:
		msgType = INT_MSG_TRANSACTION_MACHINE_MOVE_COMPLETE;
		break;
	default:
		break;
	}

	if ( msgType != INT_MSG_NONE )
	{
		CInternalMessage *transMsg = PrepareTransactionMessage( msgType, IsPlayerVirtual() );

		transMsg->SetData( (WORD)m_config.GetGameDenom(), (WORD)m_EGMConfig.GetEGMType() );
		transMsg->SetData( (DWORD)m_config.GetSlotNumber() );

		SendTransactionMessageToSCF( transMsg, QUEUE_PRIORITY_NORMAL);
	}
}

void CInternalsFramework::SendMobileConnectCommunicationSupport()
{
	bool mobileConnectCommEnabled = (bool)m_config.MobileConnectCommunicationEnabled();

	CInternalMessage *msg(new CInternalMessage(INT_MSG_SET_MOBILE_CONNECT_COMMUNICATION_SUPPORT,
		(WORD)mobileConnectCommEnabled));

	if (!mobileConnectCommEnabled)
	{
		m_notificationBox.SetMobileEnableFlag(mobileConnectCommEnabled);
		SendUIConfigToUI();
	}

	m_sentinelState.m_mobileConnectCommEnabled = mobileConnectCommEnabled;

	SendMessageToSCF(msg, QUEUE_PRIORITY_HIGHEST);
}

// 2009-09-30: Requirements to ensure no arithmetic overflow during calculations.
// 2009-09-30: 0 < billAmount && billAmount <= 0x7fff.
// 2009-09-30: 0 < acceptedBillCount && acceptedBillCount <= 0x7fffffff.
void CInternalsFramework::ProcessBillAccepted(WORD billAmount, __int64 acceptedBillCount, __int64 totalBillCount, CGame* game, CMeter* meterBillCredits)
{
	if (game && 
		meterBillCredits && 
		0 != (long)game->CompareMeter(meterBillCredits))
	{
		game->SetMeter(mBD, meterBillCredits->GetValue());
		meterBillCredits->SetMeterChanged();
	}

	m_excessiveVoucherOut.ProcessBillIn(billAmount * ONE_DOLLAR_IN_CENTS);

	//Send the bill accepted transaction to the poller
	CInternalMessage *transMsg = PrepareTransactionMessage( INT_MSG_TRANSACTION_BILL_ACCEPTED, IsPlayerVirtual() );
	transMsg->SetData( billAmount );
	transMsg->SetData( acceptedBillCount, totalBillCount );

	SendTransactionMessageToSCF( transMsg, QUEUE_PRIORITY_NORMAL);

	// Check service window opening.
	CheckMoneyInServiceWindow();
}

void CInternalsFramework::ProcessVoucherOut(DWORD amountInPennies)
{
	DWORD currentBillInPennies(0);
	if (amountInPennies > 0 && m_excessiveVoucherOut.ProcessVoucherOut(amountInPennies, currentBillInPennies))
	{
		CInternalMessage *transMsg = PrepareTransactionMessage(INT_MSG_TRANSACTION_GENERIC, IsPlayerVirtual());
		transMsg->SetData((WORD)EXCESSIVE_TICKET_OUT);
		transMsg->SetData((int64_t)amountInPennies, (int64_t)currentBillInPennies);
		SendTransactionMessageToSCF(transMsg, QUEUE_PRIORITY_NORMAL);
	}
}

void CInternalsFramework::SendMessageToUI(CInternalMessage *msg, int priorityLevel)
{
	bool isCardInserted = (m_mcr ? m_mcr->IsCardInserted() : false);

	if (m_notificationBox.SetNotificationStateByScreenId(msg->GetMessageType(), isCardInserted))
	{
		SendSubscriptionUpdateIfNecessary();
	}

	m_protocolManager->SendMessageToProtocol(IProtocolManager::UIProtocol, msg, priorityLevel);
}

void CInternalsFramework::SetMessagePersistenceAndSendMessageToUI(CInternalMessage *msg)
{
	int priority = QUEUE_PRIORITY_NORMAL;
	msg->SetMessagePersist(m_msgPriority.PrioritizeMessage(msg, priority));
	SendMessageToUI(msg, priority);
}

void CInternalsFramework::InitializeGCF( void )
{
	LogString(SASGENERAL, "Starting InitializeGCF()");
	m_protocolManager->WaitForProtocol(IProtocolManager::EGMProtocol);
	LogString(SASGENERAL, "WaitForProtocol returned");

	m_EGMConfig.SetEGMProtocolState( protocolStateEnabled );

	SendEGMSettingsToGCF();

	// We want to send the GCF the slotmast ID ASAP.
	CInternalMessage *msg = new CInternalMessage( INT_MSG_SET_SLOTMASTID, (DWORD)m_config.GetSlotMastID() );
	SendMessageToGCF( msg, QUEUE_PRIORITY_HIGHEST );

	SendAllowFloorLockoutToGCF();
	
	CheckIfEgmNeedsToBeLockedOrUnlocked(true);

	// Send cashout to host configuration setting to GCF so that it can enable/disable
	// the cashout to host feature in case current EGM state mismatches with it
	SendEnableCashoutToHostToGCF(m_config.CashoutToHostEnabled());

	SendSlotNumberToGCF();

	// We need to ask the SCF how many tickets are pending. 
	// When the response is received, it will go to the GCF.
	SendTicketOutPendingRequestToSCF();

	ProcessSentinelChangeInProgressAfterEGMStart();
}

void CInternalsFramework::UninitializeGCF( void )
{
	// We are obviously no longer on line
	m_sentinelState.SetGameOnline(false);

	m_protocolManager->RemoveProtocol(IProtocolManager::EGMProtocol);

	// Send a message to our own queue to nudge the thread to see that it has
	//  to wait to be initialized.
	CInternalMessage *msg = new CInternalMessage( INT_MSG_NONE );
	m_gcfToInternalsQueue->Add( msg, QUEUE_PRIORITY_HIGHEST );
}

void CInternalsFramework::InitializeSCF( void )
{
	m_protocolManager->WaitForProtocol(IProtocolManager::PollerProtocol);

	// Send the valid nvram message.
	CInternalMessage *validMsg = new CInternalMessage( INT_MSG_NVRAM_VALID, (WORD)(m_bNvramValidAtStartup ? 1 : 0) );
	SendMessageToSCF( validMsg, QUEUE_PRIORITY_HIGHEST );

	// send image version info
	SendVersionToSCF();

	// We want to sent the SCF the slotmast ID ASAP.
	CInternalMessage *msg1 = new CInternalMessage( INT_MSG_SET_SLOTMASTID, (DWORD)m_config.GetSlotMastID() );
	SendMessageToSCF( msg1, QUEUE_PRIORITY_HIGHEST );

	// We want to send the revision id to the scf asap
	CInternalMessage* msg2 = new CInternalMessage(INT_MSG_SET_SLOTMAST_REVISION, m_config.GetSlotMastRevision());
	SendMessageToSCF(msg2, QUEUE_PRIORITY_HIGHEST);

	// We want to send the cached IsNFDPoller config to the scf asap so that Txes/messages are created
	// based on that until we receive the proper packet version from the poller on connect
	CInternalMessage* msg = new CInternalMessage(INT_MSG_SET_NFD_POLLER, (WORD) m_config.IsNFDPoller());
	SendMessageToSCF(msg, QUEUE_PRIORITY_HIGHEST);

	SendMessageToSCF(new CInternalMessage(INT_MSG_EXTENSIBLE_SETTINGS, (DWORD)m_config.GetExtensibleSettings()), QUEUE_PRIORITY_HIGHEST);

	SendMessageToGCF(new CInternalMessage(INT_MSG_SET_NUMBER_OF_SUBGAMES_MISMATCH_CHECK, (DWORD)m_config.GetNumberOfSubgamesMismatchCheck()), QUEUE_PRIORITY_HIGHEST);

	// We want to send the SCF the MAC Address ASAP.
	// copy MAC address
	byte *MAC = new byte[SIZE_OF_MAC_ADDRESS];
	memcpy(MAC,m_config.GetMacAddressAsBytes(),SIZE_OF_MAC_ADDRESS);
	CInternalMessage *msg3 = new CInternalMessage( INT_MSG_MAC_ADDR, MAC, SIZE_OF_MAC_ADDRESS );
	SendMessageToSCF( msg3, QUEUE_PRIORITY_HIGHEST );

	// If pending slotmastid change, send it now
	if (INVALID_SLOT_MAST_ID != m_config.GetPendingSlotmast())
	{
		// We want to sent the SCF the slotmast ID ASAP.
		SendSlotmastChangeToSCF();
	}

	// We want to send SCF the host IP ASAP.
	CInternalMessage *setHostMsg = CreateConnectionDataMessage();
	SendMessageToSCF( setHostMsg, QUEUE_PRIORITY_HIGHEST );
	m_sentinelIp = setHostMsg->GetConnectionData()->GetActualIP().GetIP();
	LogString(NETWORKING_LOG, "[CInternalsFramework::InitializeSCF] Sentinel IP sent to SCF: [%s]", m_sentinelIp.c_str());

	// We want to send the feature flag to the scf asap
	CInternalMessage* msg4 = new CInternalMessage(INT_MSG_FEATURE_FLAG_UPDATE, m_config.GetFeatureFlagsRaw());
	LogString(POLLER_LOG, "Read feature flag from config.");

	SendMessageToSCF(msg4, QUEUE_PRIORITY_HIGHEST);

	m_SCFInitialized = true;
}



void CInternalsFramework::UninitializeSCF()
{
	m_SCFInitialized = false;

	m_sentinelState.SetPollerOnline(false);

	m_protocolManager->RemoveProtocol(IProtocolManager::PollerProtocol);

	// Send a message to our own queue to nudge the thread to see that it has
	//  to wait to be initialized.
	CInternalMessage *msg = new CInternalMessage( INT_MSG_NONE );
	m_scfToInternalsQueue->Add( msg, QUEUE_PRIORITY_HIGHEST );
}


void CInternalsFramework::UnitializeUI( void )
{
	m_protocolManager->RemoveProtocol(IProtocolManager::UIProtocol);

	// Send a message to our own queue to nudge the thread to see that it has
	//  to wait to be initialized.
	m_uiToInternalsQueue->Add(new CInternalMessage(INT_MSG_NONE), QUEUE_PRIORITY_HIGHEST);
}

void CInternalsFramework::InitializeUI( void )
{
	m_protocolManager->WaitForProtocol(IProtocolManager::UIProtocol);

	// send initial stuff to UI
	SendUIConfigToUI();
	SendProgressiveCriticalTextToUI(); // In case m_internalsToUIQueue was NULL when we went progressive down.
	SetDoorState();
}

EGMProtocol CInternalsFramework::GetEGMProtocolType( void )
{
	return m_EGMConfig.GetEGMProtocolType();
}

std::string CInternalsFramework::GetLcdModel() const
{
	return string(LCD_NA_STRING);
}

void CInternalsFramework::AddRemoteDebuggingLinkToDiagnosticPage(const string& ipaddress)
{
	if (CUtilities::FileExists(CDirectoryManager::REMOTE_DEBUG_FILE))
	{
		ofstream ipaddressStream("/sdboot/ipaddress.txt", ios_base::out | ios_base::binary | ios_base::trunc);
		if (ipaddressStream)
		{
			ipaddressStream << ipaddress;
			ipaddressStream.flush();
			ipaddressStream.close();
		}
	}

	const string TEMP_HTML_FILEPATH("/opt/ncompass/etc/www/tempIndex.html");
	const string NCOMPASS_HTML_FILEPATH(CDirectoryManager::Instance().HtmlFilePath() + "/ncompass.html");
	const string INDEX_HTML_FILEPATH(CDirectoryManager::Instance().HtmlFilePath() + "/index.html");
	const string IP_PATTERN("x.x.x.x:x");
	const string SLOTMASTID_PATTERN("(X)");
	const string SLOTMASTID(FormatString("(%d)", m_config.GetSlotMastID()));
	ifstream tempHtmlStream(TEMP_HTML_FILEPATH, std::ios::in | std::ios::binary);
	if (tempHtmlStream)
	{
		string tempIndexHtmlContent(std::string((std::istreambuf_iterator<char>(tempHtmlStream)), std::istreambuf_iterator<char>()));
		string ipAddressWithPort(ipaddress + ":12345");
		CUtilities::FindAndReplace(tempIndexHtmlContent, IP_PATTERN, ipAddressWithPort);
		CUtilities::FindAndReplace(tempIndexHtmlContent, SLOTMASTID_PATTERN, SLOTMASTID);
		ofstream indexHtmlStream(INDEX_HTML_FILEPATH, ios_base::out | ios_base::binary | ios_base::trunc);
		if (indexHtmlStream)
		{
			indexHtmlStream << tempIndexHtmlContent;
			indexHtmlStream.flush();
			indexHtmlStream.close();
			CUtilities::CopyFile(INDEX_HTML_FILEPATH, NCOMPASS_HTML_FILEPATH);
		}
		tempHtmlStream.close();
	}
}

void CInternalsFramework::ConfigureNetworkAdapter(bool startup)
{
	LogString(NETWORKING_LOG, "Configuring network adapter");

	bool wantDHCP = m_config.GetDHCPConfig();

	LogString(NETWORKING_LOG, "Mode requested:%s", wantDHCP ? "DHCP" : "static");

	if (wantDHCP)
	{
		if (!startup)
		{
			//Initialize DHCP
			const DWORD WAIT_FOR_ETHERNET_ADAPTER_TO_INITIALIZE(6000);
			system("/sbin/ifconfig eth0 down");
			system("/sbin/ifconfig eth0 up");
			CUtilities::Sleep(WAIT_FOR_ETHERNET_ADAPTER_TO_INITIALIZE);

		}
		//Set Subnet Mask
		GetSubnetMask();
					
		//Set Gateway
		const string pipeGatewayToFile("/sbin/ip route | awk '/default/ { print $3 }'");
		m_config.SetDefaultGateway(CUtilities::GetSystemResultString(pipeGatewayToFile));

		//Set Domain
		const string pipeDomainToFile("awk '/^domain / { i++; if (i == 1) { print $2; exit } }' /etc/resolv.conf");
		m_config.SetDomain(CUtilities::GetSystemResultString(pipeDomainToFile));

		//Set DNS
		const string pipeDnsToFile("awk '/^nameserver / { i++; if (i == 1) { print $2; exit } }' /etc/resolv.conf");
		m_config.SetPreferredDNSServer(CUtilities::GetSystemResultString(pipeDnsToFile));

		//Set Alternate DNS
		const string pipeAlternateDnsToFile("awk '/^nameserver / { i++; if (i == 2) { print $2; exit } }' /etc/resolv.conf");
		m_config.SetAlternateDNSServer(CUtilities::GetSystemResultString(pipeAlternateDnsToFile));
	}
	else
	{
		CIPAddress ip;
		m_config.GetMyIP(ip);
		CIPAddress subnetmask;
		m_config.GetSubnetMask(subnetmask);
		string setIpAndNetmask(FormatString("/sbin/ifconfig eth0 %s netmask %s", ip.GetIP().c_str(), subnetmask.GetIP().c_str()));
		CUtilities::GetSystemResultString(setIpAndNetmask);
		LogStringNoFormat(NETWORKING_LOG, setIpAndNetmask);

		CIPAddress gateway;
		m_config.GetDefaultGateway(gateway);
		string setGateway(FormatString("/sbin/ip route add default via %s", gateway.GetIP().c_str()));
		CUtilities::GetSystemResultString(setGateway);
		LogStringNoFormat(NETWORKING_LOG, setGateway);
	}

	if (NetworkUtils::Instance().GetNetworkCableToggleState())
	{
		NetworkUtils::Instance().SetNetworkCableToggleState(false);
		NetworkUtils::Instance().SetNetworkServiceRestarted(true);
	}

	if (startup)
	{
		GetSubnetMask();

		//Set Gateway
		const string pipeGatewayToFile("/sbin/ip route | awk '/default/ { print $3 }'");
		const string gateway(CUtilities::GetSystemResultString(pipeGatewayToFile));
		m_config.SetDefaultGateway(gateway);

		//Set DNS
		const string pipeDnsToFile("awk '/^nameserver / { i++; if (i == 1) { print $2; exit } }' /etc/resolv.conf");
		const string dns(CUtilities::GetSystemResultString(pipeDnsToFile));
		m_config.SetPreferredDNSServer(dns);
		
		//Set Alternate DNS
		const string pipeAlternateDnsToFile("awk '/^nameserver / { i++; if (i == 2) { print $2; exit } }' /etc/resolv.conf");
		const string alternateDns(CUtilities::GetSystemResultString(pipeAlternateDnsToFile));
		m_config.SetAlternateDNSServer(alternateDns);

		//Set Domain
		const string pipeDomainToFile("awk '/^domain / { i++; if (i == 1) { print $2; exit } }' /etc/resolv.conf");
		const string domain(CUtilities::GetSystemResultString(pipeDomainToFile));
		m_config.SetDomain(domain);
	
		LogString(NETWORKING_LOG, "ConfigureNetworkAdapter gateway=%s dns=%s alternateDns=%s domain=%s", 
			gateway.c_str(), dns.c_str(), alternateDns.c_str(), domain.c_str());
	}

	AddRemoteDebuggingLinkToDiagnosticPage(m_config.GetActualIP().GetIP());
}

void CInternalsFramework::GetSubnetMask()
{
	const string pipeMaskToFile = "/sbin/ifconfig eth0 | awk '/ Mask:| netmask / { gsub(/Mask:/, \"\"); print $4; exit }'";

	string subnetMask(CUtilities::GetSystemResultString(pipeMaskToFile));

	LogString(NETWORKING_LOG, "GetSubnetMask pipeMaskToFile=%s subnetMask=%s", pipeMaskToFile.c_str(), subnetMask.c_str());

	cout << "GetSubnetMask=" << subnetMask << endl;

	m_config.SetSubnetMask(subnetMask);
}

void CInternalsFramework::AdjustSpeakerVolume()
{
	const string adjustVolume(FormatString("axpt-volume %u", m_config.GetLeftSpeakerVolume()));
	if (system(adjustVolume.c_str()) != 0)
	{
		LogString(ERROR_LOG, "Adjusting volume failed with command %s", adjustVolume.c_str());
	}
}

void CInternalsFramework::AdjustScreenBrightness()
{
	USHORT percentBackLight = m_config.GetScreenBrightness();
	if (BACKLIGHT_INCREMENT > percentBackLight)
	{
		percentBackLight = BACKLIGHT_INCREMENT;
	}		
	USHORT pwmBackLight = percentBackLight * 1023 / 100;
	S7LITE_RemoteLCD_SetBacklightPWM(pwmBackLight);
}

BatteryStatus CInternalsFramework::GetBatteryStatus( void )
{
	BatteryStatus batteryStatus = batteryStatusUnknown;

	BOOLEAN batteryOK = FALSE;
	S7_Result result = S7LITE_Battery_GetStatus(&batteryOK);
	if ( result == S7DLL_STATUS_OK )
	{
		if ( batteryOK )
		{
			batteryStatus = batteryStatusOkay;
		}
		else
		{
			batteryStatus = batteryStatusLow;
		}
	}

	else
	{
		LogString(ERROR_LOG, "Check of battery status failed.");
	}

	return batteryStatus;
}

// CInternalsFramework::TellUIMediaWindowPlayerState
//   IN playerState - indicates the media window open/close change to be made, if any.
// Tell UI (CUIProtocol and CMediaWindowManager) if media window opened or closed.
void CInternalsFramework::TellUIMediaWindowPlayerState(MediaWindowPlayerState playerState)
{
	switch (playerState)
	{
	case OPEN_PLAYER_STATE:
		// Tell the UI that the media window opened.
		SendServiceWindowShowToUI(true);
		break;
	case CLOSE_PLAYER_STATE:
		// Tell the UI that the media window closed.
		SendServiceWindowShowToUI(false);
		break;
	case MAINTAIN_PLAYER_STATE:
	default:
		// Maintain current media window (opened/closed) state.
		break;
	}
}

void CInternalsFramework::SetTimezone( void )
{
	// Get the timezone offset (in minutes)
	short timeZoneOffset = m_config.GetTimezoneOffset();

	if ( CUtilities::IsTimezoneValid(timeZoneOffset) )
	{
		CUtilities::SetTzOffset( timeZoneOffset );
	}
}

void CInternalsFramework::CalcImageChecksum()
{	
	//Delete System Volume Information folder if it exists so that checksum won't be messed up
	const string systemVolumeInfoFolder(CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_OS) + "/System Volume Information");
	struct stat statsystemVolumeInfoFolder;
	if ((!stat(systemVolumeInfoFolder.c_str(), &statsystemVolumeInfoFolder)))
	{
		CUtilities::DeleteDirectory(systemVolumeInfoFolder, false, nullptr);
	}

	// Set to image checksum to zero while calculating.
	m_config.SetImageChecksum(0);
	const string sdbootChecksumToTxtFile = "cd " + CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_OS) + 
		" && find . -type f -exec sha1sum '{}' + | sort > " + CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_APPLICATIONS) + "/checksum.txt";
	const string pipeChecksumToFile = "sha1sum " + CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_APPLICATIONS) + 
		"/checksum.txt | awk '{print $1}'";

	//We perform the checksum by doing a sha1sum of the whole /sdboot directory to a file (checksum.txt) and then 
	//we sha1sum the checksum.txt file and take the last 8 hexadecimal characters of the sha1sum value as our checksum.
	CUtilities::GetSystemResultString(sdbootChecksumToTxtFile);	
	string checksum(CUtilities::GetSystemResultString(pipeChecksumToFile));
	const size_t LAST_8_HEXADECIMAL_SHA1SUM_CHARS(8);
	checksum.erase(0, checksum.size() - LAST_8_HEXADECIMAL_SHA1SUM_CHARS);
	const int BASE_16(16);
	m_config.SetImageChecksum(std::stoul("0x" + checksum, nullptr, BASE_16));
	SendMessageToSCF(new CInternalMessage(INT_MSG_SET_IMAGE_CHECKSUM, m_config.GetImageChecksum()), QUEUE_PRIORITY_NORMAL);
}

bool CInternalsFramework::CalcFileChecksum(std::string filename, DWORD &outChecksum, bool simple)
{
	bool retVal = false;

	outChecksum = 0;
	FILE *file;

	file = fopen(filename.c_str(), "r");
	if (file != NULL)
	{
		// initialize checksum object
		CCheckSum checksum(simple);
		checksum.Clear();

		// read through file
		const DWORD FILEBUFFERSIZE = 10000;
		DWORD numberBytesRead = 0;
		byte buffer[FILEBUFFERSIZE];
		DWORD sleepIndex = 0;

		memset(buffer, 0, FILEBUFFERSIZE);
		numberBytesRead = fread(buffer, 1, FILEBUFFERSIZE, file);
		while(numberBytesRead > 0)
		{
			// read data
			checksum.Add( buffer, (UINT) numberBytesRead);
			// Sleep once in awhile, but not every time.
			if (++sleepIndex > 10)
			{
				CUtilities::Sleep(5);
				sleepIndex = 0;
			}

			memset(buffer, 0, FILEBUFFERSIZE);
			numberBytesRead = fread(buffer, 1, FILEBUFFERSIZE, file);
		}

		fclose(file);
		outChecksum = checksum.Get();
		retVal = true;
	}
	else
	{
		LogString(ERROR_LOG, "InternalsFramework: Unable to check file %s checksum - error: File does not exist",filename.c_str());
	}

	return retVal;
}

void CInternalsFramework::GetVersion(byte &major, byte &minor, byte &build, byte &revision)
{
	ifstream ifs("/opt/ncompass/version.txt", std::ifstream::in);

	major = 0, minor = 0, build = 0, revision = 0;

	if (ifs.is_open())
	{
		WORD temp;
		ifs >> temp;
		major = temp;
		ifs >> temp;
		minor = temp;
		ifs >> temp;
		build = temp;
		ifs >> temp;
		revision = temp;
		ifs.close();
	}
}

void CInternalsFramework::SendVersionToSCF()
{
	byte major, minor, build, revision;
	
	GetVersion(major, minor, build, revision);
	DWORD result = (build << 24) | (major << 16) | (minor << 8) | revision;
	CInternalMessage *msg = new CInternalMessage( INT_MSG_SET_VERSION, result );
	SendMessageToSCF( msg, QUEUE_PRIORITY_NORMAL);
}

void CInternalsFramework::SendSlotNumberToGCF( void )
{
	CInternalMessage *msg = new CInternalMessage( INT_MSG_SET_SLOT_NUMBER, (DWORD) m_config.GetSlotNumber() );
	SendMessageToGCF( msg, QUEUE_PRIORITY_BELOW_NORMAL );	
}

void CInternalsFramework::SendAllowFloorLockoutToGCF( void )
{
	CInternalMessage *msg = new CInternalMessage( INT_MSG_SET_ALLOW_FLOOR_LOCKOUT, (WORD) m_config.AllowFloorLockout() );
	SendMessageToGCF( msg, QUEUE_PRIORITY_HIGHEST );	
}

void CInternalsFramework::SendDashboardStatusToProgressive()
{
	DWORD campaignId = m_sentinelState.GetCampaignId();
	DWORD skinId = m_sentinelState.GetSkinId();
	int campaignRevision = m_sentinelState.GetCampaignRevision();
	int skinRevision = m_sentinelState.GetSkinRevision();
	WORD currentActivity = (WORD)m_sentinelState.GetCurrentActivity();
	WORD pendingAction = (WORD)m_sentinelState.GetPendingAction();

	CInternalMessage *msg = new CInternalMessage(INT_MSG_PROGRESSIVE_DASHBOARD_STATUS, campaignId, skinId);
	msg->SetData(currentActivity, pendingAction);
	msg->SetData(campaignRevision, skinRevision);
	LogString(PROGRESSIVE, "[%s] campaignId = %d, campaignRevision = %d, skinId = %d, skinRevision = %d, currentActivity = %d, pendingAction = %d",
		__FUNCTION__, campaignId, campaignRevision, skinId, skinRevision, currentActivity, pendingAction);
	SendMessageToProgressive(msg, QUEUE_PRIORITY_NORMAL);
}

/// <summary>
/// Processes the returned messages.
/// </summary>
/// <param name="returnMessages">The return messages.</param>
void CInternalsFramework::ProcessReturnedMessages(std::vector<MsgTargetPair*> * returnMessages)
{
	//Send all objects to the needed target then delete and remove the items.
	for (auto awardIterator = returnMessages->cbegin(); awardIterator != returnMessages->cend();/*no need to in iterator*/)
	{		
        LogString(PROGRESSIVE, _T("ProcessReturnedMessages: Target %d"), (*awardIterator)->target);
		switch ((*awardIterator)->target)
		{
		case mqPoller:
		{
			if (nullptr == (*awardIterator)->msg)	//this was from someone passing in a null msg... ugh
			{
				CEGMAward* award = m_egmAwardManager.GetEGMAwardCopy();

				SendPayoutToOneLink(award);

				// external bonus meters just incremented, send to poller
				CInternalMessage * transMsg = PrepareTransactionMessage(INT_MSG_SEND_EXTERNAL_BONUS, IsPlayerVirtual());
				transMsg->SetData(award);
				SendTransactionMessageToSCF(transMsg, QUEUE_PRIORITY_NORMAL);

				if (nullptr != (*awardIterator)->msg)
				{
					delete (*awardIterator)->msg; //Delete the message that was created if one was.
				}
			}
			else
			{				
				LogString(PROGRESSIVE, _T("ProcessReturnedMessages: Sending message %s to poller protocol"), (*awardIterator)->msg->GetMessageTypeStr().c_str());
				SendTransactionMessageToSCF((*awardIterator)->msg, QUEUE_PRIORITY_NORMAL);
			}
			break;
		}
		case mqGCF:
			if (nullptr != (*awardIterator)->msg)
			{
				LogString(PROGRESSIVE, _T("ProcessReturnedMessages: Sending message %s to SAS protocol"), (*awardIterator)->msg->GetMessageTypeStr().c_str());
				SendMessageToGCF((*awardIterator)->msg, QUEUE_PRIORITY_NORMAL);  //send the message and don't delete it.
			}
			break;
		case mqProgressive:
			if (nullptr != (*awardIterator)->msg)
			{				
				LogString(PROGRESSIVE, _T("ProcessReturnedMessages: Sending message %s to progressive protocol"), (*awardIterator)->msg->GetMessageTypeStr().c_str());
				SendMessageToProgressive((*awardIterator)->msg, QUEUE_PRIORITY_NORMAL); //send the message and don't delete it.
			}
			break;

		default:
			if (nullptr != (*awardIterator)->msg)
			{
				delete (*awardIterator)->msg; //Delete the message that was created
			}
			break;
		}
		delete (*awardIterator);
		awardIterator = returnMessages->erase(awardIterator);	//remove the item from the vector and repair the iterator
	}
}

/// <summary>
/// Checks to see if EGM needs to be unlocked or locked.
/// </summary>
/// <param name="startup">true if this call is made from startup.</param>
void CInternalsFramework::CheckIfEgmNeedsToBeLockedOrUnlocked(bool startup)
{
	bool oldEnableValue(m_enableEGM);

	if(m_config.GetEGMLockoutState() || IsPlayerBannedFromPlay())
	{
		m_enableEGM = false;
		if(m_personalBanker->GetUnlockEgmDuringTransfer())
		{
			m_enableEGM = true;
		}
	}
	else
	{
		m_enableEGM = true;
	}

	if(startup || (oldEnableValue!= m_enableEGM))
	{
		SendEGMEnableToGCF(startup, m_enableEGM);
	}
}

void CInternalsFramework::SendEGMSettingsToGCF( void )
{
	CInternalMessage *msg(new CInternalMessage(INT_MSG_SET_EGM_SETTINGS, new CEGMConfig(m_EGMConfig)));
	LogString(PBT_LOG, "SendEGMSettingsToGCF m_PBTEnabled = %d", (WORD)IsPBTEnabled());
	
	msg->SetData((WORD)IsPBTEnabled());

	msg->SetData((__int64)m_games.GetEgmMeterValue(mBD));

	SendMessageToGCF( msg, QUEUE_PRIORITY_HIGHEST );

	// Send Jackpot Reset method to GCF
	SendMessageToGCF(new CInternalMessage(INT_MSG_SET_JACKPOT_RESET, (WORD) (!m_config.GetUJPEnabled() && m_config.JackpotA8Reset())), QUEUE_PRIORITY_HIGHEST);
}

bool CInternalsFramework::IsPBTEnabled( void )
{
	return ((this->m_config.PBTCashEnabled() && this->m_config.PBTCashEnabledGlobal()) || 
		(this->m_config.PBTPointsEnabled() && this->m_config.PBTPointsEnabledGlobal()) ||
		(this->m_config.PBTPromoEnabled() && this->m_config.PBTPromoEnabledGlobal()));
}

/// <summary>
/// Sends the EGM Protocol a message to lock or unlock the EGM.
/// </summary>
/// <param name="bEnable">
/// Flag to enable/disable the EGM. 
///	true = enable
/// false = disable
/// </param>
void CInternalsFramework::SendEGMEnableToGCF(bool startup, bool bEnable)
{
	// We want to send the lockout state to the egm
    SendMessageToGCF(new CInternalMessage(INT_MSG_ENABLE_GAME, (WORD)startup, (WORD)bEnable), QUEUE_PRIORITY_HIGHEST );
}

/// <summary>
/// Sends the EGM Protocol a message to enable or disable cashout to host feature.
/// </summary>
/// <param name="bEnable">
/// Flag to enable/disable cashout to host feature on EGM. 
///	true = enable
/// false = disable
/// </param>
void CInternalsFramework::SendEnableCashoutToHostToGCF( bool bEnable )
{
	LogString(SASGENERAL, "[%s] Queuing INT_MSG_ENABLE_CASHOUT_TO_HOST [%d]", __FUNCTION__, bEnable);
	CInternalMessage *msg = new CInternalMessage(INT_MSG_ENABLE_CASHOUT_TO_HOST, (WORD)bEnable);
	SendMessageToGCF(msg, QUEUE_PRIORITY_HIGHEST);
}

/// <summary>
/// Gets the transaction meters.
/// </summary>
/// <param name="memoryType">IN - Type of the memory.</param>
/// <param name="memoryOffset">IN - The memory offset.</param>
/// <returns>A pointer to the transaction meters.  MUST NOT be NULL.</returns>
CMeters * CInternalsFramework::GetTransactionMeters( MEMORY_TYPE memoryType, DWORD memoryOffset )
{
	CMeters *meters = new CMeters(memoryType, true);

	CGame *game = m_games.GetEGM();

	if (memoryType == MEMORY_NVRAM)
	{
		meters->SetOffset( memoryOffset );
		meters->SetAllowedSize( NVRAM_PLAYER_START_METERS_SIZE );
		// Activate the NVRAM block and set meter count as 0
		meters->InitialWriteToPermanentStore( false );
	}

	if (!IsNFDPoller())
	{
		// Use a fixed set of 53 meters in the meter object, with anyone not present converted into 0.
		// NOTE: Not persisting these value here. Will do it in blocks of 4s after the values are set.
		meters->SetMeter(mCD, (game != NULL ? game->GetMeter(mCD) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(mSDr, (game != NULL ? game->GetMeter(mSDr) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(mDDr, (game != NULL ? game->GetMeter(mDDr) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(mLDr, (game != NULL ? game->GetMeter(mLDr) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(mCDr, (game != NULL ? game->GetMeter(mCDr) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(mFDr, (game != NULL ? game->GetMeter(mFDr) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(mD1, (game != NULL ? game->GetMeter(mD1) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(mD2, (game != NULL ? game->GetMeter(mD2) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(mD5, (game != NULL ? game->GetMeter(mD5) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(mD10, (game != NULL ? game->GetMeter(mD10) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(mD20, (game != NULL ? game->GetMeter(mD20) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(mD50, (game != NULL ? game->GetMeter(mD50) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(mD100, (game != NULL ? game->GetMeter(mD100) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(mCoupV, (game != NULL ? game->GetMeter(mCoupV) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(mTVal, (game != NULL ? game->GetMeter(mTVal) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(mDr, (game != NULL ? game->GetMeter(mDr) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(mCrd, (game != NULL ? game->GetMeter(mCrd) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(mTCi, (game != NULL ? game->GetMeter(mTCi) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(mTCo, (game != NULL ? game->GetMeter(mTCo) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(mBD, (game != NULL ? game->GetMeter(mBD) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(mTHP, (game != NULL ? game->GetMeter(mTHP) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(mCTktPV, (game != NULL ? game->GetMeter(mCTktPV) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(mHPWinRcptVal, (game != NULL ? game->GetMeter(mHPWinRcptVal) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(mActualCD, (game != NULL ? game->GetMeter(mActualCD) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(mCPromoOutVal, (game != NULL ? game->GetMeter(mCPromoOutVal) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(gCI, (game != NULL ? game->GetMeter(gCI) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(gCO, (game != NULL ? game->GetMeter(gCO) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(gGS, (game != NULL ? game->GetMeter(gGS) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(gGW, (game != NULL ? game->GetMeter(gGW) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(gMC, (game != NULL ? game->GetMeter(gMC) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(gCC, (game != NULL ? game->GetMeter(gCC) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(gBW, (game != NULL ? game->GetMeter(gBW) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(gJP, (game != NULL ? game->GetMeter(gJP) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(gPI, (game != NULL ? game->GetMeter(gPI) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(mHCC, (game != NULL ? game->GetMeter(mHCC) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(mActualSlotDr, (game != NULL ? game->GetMeter(mActualSlotDr) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(mPbAC2G, (game != NULL ? game->GetMeter(mPbAC2G) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(mPbAR2G, (game != NULL ? game->GetMeter(mPbAR2G) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(mPbAN2G, (game != NULL ? game->GetMeter(mPbAN2G) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(mPbAC2H, (game != NULL ? game->GetMeter(mPbAC2H) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(mPbAR2H, (game != NULL ? game->GetMeter(mPbAR2H) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(mPbAN2H, (game != NULL ? game->GetMeter(mPbAN2H) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(mPbAD2G, (game != NULL ? game->GetMeter(mPbAD2G) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(mBonusMach, (game != NULL ? game->GetMeter(mBonusMach) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(mBonusAtt, (game != NULL ? game->GetMeter(mBonusAtt) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(mPrgPayAtt, (game != NULL ? game->GetMeter(mPrgPayAtt) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(mPrgPayMac, (game != NULL ? game->GetMeter(mPrgPayMac) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(mTValueCI, (game != NULL ? game->GetMeter(mTValueCI) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(mTValueCO, (game != NULL ? game->GetMeter(mTValueCO) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(mRstrPlayd, (game != NULL ? game->GetMeter(mRstrPlayd) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(mGameWTPP, (game != NULL ? game->GetMeter(mGameWTPP) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(mTotalDrop, (game != NULL ? game->GetMeter(mTotalDrop) : INVALID_METER_NUMBER), false, false);
		meters->SetMeter(mVchrTktDrop, (game != NULL ? game->GetMeter(mVchrTktDrop) : INVALID_METER_NUMBER), false, false);
	}
	else if (game)
	{
		// Copy all EGM meters into new CMeters object
		*meters = *game->GetMeters();
		// Set persistency flag back, copy constructor may have removed it
		meters->SetMemoryType(memoryType);
	}

    // Persisting the meters above (will be done only if memory type was passed as NVRAM).
	/* NOTE : passing false here because the current function is being called always
	   for player session meters and we want to maintain backward compatibility with
	   offsets on which these meters were being written in the NVRAM earlier. */
	// Note: This just persist the meters in the array it has and doesnt invalidate
	// other NVRAM locations that may have already been marked valid.
	meters->PersistMeters(false);

	return meters;
}

// This will create and add the necessary items to the transaction message and send to the SCF.
// Reutrns NULL if the mesasge cannot be created at this time (like no game yet).
CInternalMessage * CInternalsFramework::PrepareTransactionMessage( InternalMsgType msgType, bool isPlayerVirtual, time_t *transactionTime, CMeters *meters )
{
	CInternalMessage *msg = NULL;

	if ( meters == NULL )
		meters = GetTransactionMeters();

	if ( meters != NULL )
	{
		if (IsNFDPoller())
		{
			// For NFD pollers we just want to send non-0 valid meter values
			// So remove those having 0 or INVALID_METER_VALUE (-1)
			meters->TrimToNonZeroValidMeters();
		}

		if ( transactionTime == NULL )
		{
			transactionTime = new time_t(CUtilities::GetCurrentTime());
		}

		msg = new CInternalMessage( msgType, transactionTime );
		DWORD dwStatus = GetTransactionStatus(isPlayerVirtual);
		msg->SetData((DWORD)0, dwStatus);
		msg->SetTransactionMetersData( meters );

		if ((INT_MSG_TRANSACTION_CARDLESS_SESSION_START != msgType) &&
			(INT_MSG_TRANSACTION_CARDLESS_SESSION_END != msgType))
		{
			// Do not add card number to transaction data for cardless transactions
			byte * cardID = new byte[CARD_ID_LEN];
			memcpy(cardID, m_config.GetLastCardRead(), CARD_ID_LEN);
			msg->SetData(cardID, CARD_ID_LEN);
		}
	}

	return msg;
}

void CInternalsFramework::SendSlotmastChangeToSCF()
{
	// prepare the transaction message.
	CInternalMessage *msg = PrepareTransactionMessage( INT_MSG_CHANGE_SLOTMASTID, IsPlayerVirtual() );
	
	if ( msg != NULL )
	{
		// Add DWORD slotmastid data
		msg->SetData((DWORD)m_config.GetPendingSlotmast());

		SendTransactionMessageToSCF( msg, QUEUE_PRIORITY_HIGHEST );
	}
}

DWORD CInternalsFramework::GetTransactionStatus(bool IsPlayerVirtual)
{

	DWORD dwStatus = 0;

	CGame* game = m_games.GetEGM();
	if (game != NULL)
	{
		// Slot Door Open
		if (game->IsDoorOpen(SlotDoor))
		{
			dwStatus+= 0x00000001;
		}
		// Drop Door Open
		if (game->IsDoorOpen(DropDoor))
		{
			dwStatus+= 0x00000002;
		}
		// Logic Door Open
		if (game->IsDoorOpen(LogicDoor))
		{
			dwStatus+= 0x00000004;
		}
		// Cash Door Open
		if (game->IsDoorOpen(CashDoor))
		{
			dwStatus+= 0x00000008;
		}
		// Aux Fill Door
		if (game->IsDoorOpen(AuxFillDoor))
		{
			dwStatus+= 0x00000010;
		}
		// Extra Door
		if (game->IsDoorOpen(ExtraDoor))
		{
			dwStatus+= 0x00000020;
		}
		// Non-Zero Credit Meter
		if (game->GetMeter(mCrd) != 0)
		{
			dwStatus+= 0x00000100;
		}

	}

	// TDB OASIS Security Door
	//dwStatus+=0x00000040;

	// Meters Lost
	// TBD There's a transaction to send at startup (1) that says
	// Sentinel On... transaction 42 says sentinel on - memory ok
	// ALSO TBD The memory lost in nvram must be set after a rest.
	// 
	if (m_config.GetMemoryLost())
	{
		dwStatus+= 0x00000080;
	}
	
	// Virtual Session
	if (IsPlayerVirtual)
	{
		dwStatus+= 0x00000200;
	}

	// Game / Slave Comms Down
	if (!m_sentinelState.IsGameOnline())
	{
		dwStatus+= 0x00000400;
	}

	// TBD Host Progressive Comms Down

	// Host System Comms Down
	if (!m_sentinelState.IsPollerOnline())
	{
		dwStatus+= 0x00001000;
	}

	// Game Disabled
	if (m_config.GetEGMLockoutState())
	{
		dwStatus+= 0x00002000;
	}

	// Sentinel Transactions Backlogged
	// dwStatus+= 0x00008000;

	// Reserved (bit 16,17, 19-29)

	// Mobile Carded Session (bit 18)
	if (m_pairingInformation.IsMobileCardedSession())
	{
		dwStatus += 0x00020000;
	}	

	// DPU Transactions Backlogged (Extant)
	// dwStatus+= 0x40000000;


	// TBD PC Transactions Backlogged
	// dwStatus+= 0x80000000;

	return dwStatus;
}

void CInternalsFramework::StartSubgameMetersUpdateThread()
{
	m_updateSubgameMetersThread.reset(new thread(UpdatePollerWithSubgameMeters, this));
	m_updateSubgameMetersThread->detach();
}

void CInternalsFramework::SendSubgameMetersToSCF()
{
	//if we are already doing this just bail out
	if(!m_bSendSubgameMetersInProgress)
	{
		m_bSendSubgameMetersInProgress = true;

		CGame *game = nullptr;

		// Send current subgame meters to SCF
		game = m_games.GetCurrentGame();
		if (game && !game->IsEGM())
		{
			LogString(GAMES, "[%s]: Sending current subgame meters to Poller",
				__FUNCTION__);
			SendGameMetersToSCF(game);
		}

		LogString(GAMES, "[%s]: Sending enabled subgames meters to Poller", 
			__FUNCTION__);
		// Send enabled subgame meters to SCF
		for ( int mi = 0; mi < m_games.GetGameCount() && !m_shutdownReadCard; mi++ )
		{
			game = m_games.GetGameByIndex( mi );
			if (game && game->IsEnabled() && !game->IsCurrentGame() && !game->IsEGM())
			{
				SendGameMetersToSCF(game);
			}

			CUtilities::Sleep(100);
		}

		LogString(GAMES, "[%s]: Sending non-enabled or 'played earlier' subgames meters to Poller", 
			__FUNCTION__);

		// Send non-enabled subgames meters to SCF. For NFD pollers send 
		// meters of only those games which were played sometime earlier.
		for ( int mi = 0; mi < m_games.GetGameCount() && !m_shutdownReadCard; mi++ )
		{
			game = m_games.GetGameByIndex( mi );
			if (game && !game->IsEnabled() && !game->IsEGM() &&
				(!IsNFDPoller() || (game->GetMeter(SUBGAME_METER_COIN_IN) > 0)))
			{
				SendGameMetersToSCF(game);
			}

			CUtilities::Sleep(100);
		}

		m_bSendSubgameMetersInProgress = false;

        LogString(GAMES,"[%s]: All subgame meters sent to Poller Protocol",
			__FUNCTION__);
	}
}

// Thread functions that performs memory dumps. 
// Loops forever.
void CInternalsFramework::PerformMemoryDumps()
{
	DWORD dumpProcessTimeout = CUtilities::GetTickCount();

	DWORD startupTickCount = CUtilities::GetTickCount();
	CMemoryThresholds memoryThresholds(startupTickCount);

	CWebDiagnostics webDiagnostics;
	webDiagnostics.Start();

	//loop for ever
	while ( !m_shutdownReadCard )
	{
		CWatchDog::Instance().Checkin();
		DWORD currentTick = CUtilities::GetTickCount();

		if (currentTick - dumpProcessTimeout > CWebDiagnostics::TIME_BETWEEN_MEMORY_DUMPS)
		{

			// Periodically check memory threshold status.
			PerformMemoryThresholdChecks(memoryThresholds, currentTick);
			CWatchDog::Instance().Checkin();

			// Update diagnostics after obtaining available physical memory -- obtained via PerformMemoryThresholdChecks().
			PerformWebDiagnostics(webDiagnostics, memoryThresholds);
			CWatchDog::Instance().Checkin();
			dumpProcessTimeout = CUtilities::GetTickCount();

		}
		
		//check our IP address
		if ((currentTick - m_adapterCheckTimeout > ADAPTER_CHECK_TIMEOUT) && !m_shutdownReadCard)
		{
			PerformIPAddressCheck();
			CWatchDog::Instance().Checkin();
		}

		CUtilities::Sleep(500);

	}
}

void CInternalsFramework::PerformMemoryThresholdChecks(CMemoryThresholds &memoryThresholds, DWORD currentTick)
{
	// Depending on the status, plan when to reboot and send a reboot warning transaction when appropriate.
	MemoryThresholdStatusType memoryThresholdStatus = memoryThresholds.GetMemoryThresholdStatus(
		m_restartManager.IsRestartUiAfterxDays(),
		m_restartManager.IsAbleToRestartUi(),
		m_config.GetRebootAsapThresholdInMb(),
		m_config.GetRebootWarningThresholdMb(),
		m_config.GetRestartUIAsapThresholdMb(),
		m_config.GetRestartUIAfterDropThresholdMb(),
		currentTick, 
		m_sentinelState.m_availablePhysicalMemory);

	switch (memoryThresholdStatus)
	{
	case MemoryThresholdStatusUIRestartAfterDrop:
		// Only log it once
		if (!m_restartManager.GetUiRestartAfterNextDropFlag())
		{
			LogString(UI_LOG, _T("Met memory threshold criteria for UI restart after drop, plan to restart UI after drop."));

			m_sentinelState.SetPendingAction(RestartUI);
			SendDashboardStatusToProgressive();

			// Set the flag to reboot after a subsequent drop.
			m_restartManager.SetUiRestartAfterNextDropFlag(true);
		}
		break;

	case MemoryThresholdStatusUIRestartAsap:
		// Only log it once
		if (!m_restartManager.GetPerformUiRestartAsapFlag())
		{
			LogString(UI_LOG, _T("Met memory threshold criteria for UI restart ASAP, plan to restart ASAP."));

			m_sentinelState.SetPendingAction(RestartUI);
			SendDashboardStatusToProgressive();

			m_restartManager.SetPerformUiRestartAsapFlag(true);
		}
		break;

	case MemoryThresholdStatusRebootAfterDrop:
		// Don't send reboot warning transactions (repeatedly) if we are already going to reboot.
		if (m_restartManager.NeedToSendRebootWarningTransaction())
		{
			LogString(GENERAL, _T("Met memory threshold criteria for reboot after drop, plan to reboot after drop"));

			// Send the reboot warning transaction.
			CInternalMessage *transMsg = PrepareTransactionMessage(INT_MSG_TRANSACTION_CONTROLLED_REBOOT_WARNING, IsPlayerVirtual());
			SendTransactionMessageToSCF(transMsg, QUEUE_PRIORITY_NORMAL);
		}

		m_sentinelState.SetPendingAction(RebootAfterDrop);
		SendDashboardStatusToProgressive();

		// Set the flag to reboot after a subsequent drop.
		m_restartManager.SetToPerformRebootAfterNextDrop();

		break;

	case MemoryThresholdStatusRebootASAP:
		if (!m_restartManager.NeedToPerformControlledRebootAsap())
		{
			m_sentinelState.SetPendingAction(RebootAsap);
			SendDashboardStatusToProgressive();

			LogString(GENERAL, _T("Met memory threshold criteria for reboot ASAP, plan to reboot ASAP"));

			// Set the flag to reboot as soon as we can.
			m_restartManager.SetToPerformControlledRebootAsap();
		}
		break;

	default:
		break;
	}
}

void CInternalsFramework::PerformWebDiagnostics(CWebDiagnostics &webDiagnostics, CMemoryThresholds &memoryThresholds)
{
	time_t currentTime = CUtilities::GetCurrentTime();
	DWORD currentTickCount = CUtilities::GetTickCount();

	const CWebDiagnosticsCommonInfo commonInfo(m_sentinelState.m_startupTime,
		currentTime,
		m_config.GetSlotMastID(),
		m_config.GetSlotNumber(),
		m_sentinelState.m_sentinelVersion,
		CPlatform::Instance().GetOSImageVersionNumber(),
		CMediaPackageManager::Instance().GetActiveSkinInfo(),
		CMediaPackageManager::Instance().GetPendingSkinInfo(),
		CMediaPackageManager::Instance().GetActiveCampaignInfo(),
		CMediaPackageManager::Instance().GetPendingCampaignInfo(),
		m_sentinelState.m_availablePhysicalMemory,
		m_config.GetHostAddressOrName(),
		m_sentinelState.m_pollerPacketSentVerifyErrors,
		m_sentinelState.m_pollerPacketVersion,
		m_EGMConfig.GetSentinelOnlineStateStr(),
		CPlatform::Instance().GetCarrierFirmwareVersion(),
		CPlatform::Instance().GetVideoMixerFirmwareVersion(),
		m_config.GetBatteryStatusText(),
		CPlatform::Instance().GetCPUTemperature());

	PerformUpdateWebDiagnostics(webDiagnostics, commonInfo, memoryThresholds, currentTime, currentTickCount);

	PerformProgressiveWebDiagnostics(webDiagnostics, commonInfo);
	PerformPlayerWebDiagnostics(webDiagnostics, commonInfo);
	PerformOptionsAndSettingsWebDiagnostics(webDiagnostics, commonInfo);
	PerformConfigExtWebDiagnostics(webDiagnostics, commonInfo);
	{
		std::lock_guard<std::recursive_mutex> lock(m_listOfCertsCriticalSection);
		webDiagnostics.UpdateCertificatesInfo(commonInfo, m_listOfCerts);
	}
	webDiagnostics.UpdateOneLinkContentInfo(commonInfo, m_mediaPackageManager->GetOneLinkContentInformation(m_config.GetActualIP().GetIP()));
	PerformQueuingConfigWebDiagnostics(webDiagnostics, commonInfo);
	PerformCardReaderDiagnostics(webDiagnostics, commonInfo);
	PerformJackpotInfoDiagnostics(commonInfo);

}

void CInternalsFramework::PerformUpdateWebDiagnostics(CWebDiagnostics& webDiagnostics,
	const CWebDiagnosticsCommonInfo& commonInfo,
	CMemoryThresholds &memoryThresholds,
	const time_t& currentTime,
	DWORD currentTickCount)
{
	const CEGMConfig egmConfig(m_EGMConfig);

	DWORD activeMultiplier = 0;
	DWORD activeDivider = 0;
	if (m_player != nullptr)
	{
		m_player->GetActiveMultiplierDivider(activeMultiplier, activeDivider);
	}
		
	const CWebDiagnosticsStatusInfo statusInfo(currentTime,
		currentTickCount,
		m_sentinelState.m_tickCountOfLastFloorLogixTimeUpdateAvailable,
		m_sentinelState.m_tickCountOfLastFloorLogixTimeUpdate,
		m_sentinelState.m_timeOfLastFloorLogixConfigAvailable,
		m_sentinelState.m_timeOfLastFloorLogixConfig,
		m_config.GetTimezoneOffset(),
		m_sentinelState.m_rebootTilt,
		m_sentinelState.m_lastUIRestartTime,
		m_sentinelState.m_uiRestartCount,
		m_sentinelState.m_lastUIRefreshTime,
		m_sentinelState.m_uiRefreshCount,
		m_config.GetMACAddressAsString(),
		m_config.GetMACAddress2AsString(),
		m_config.GetRebootWarningThresholdMb(),
		m_config.GetRebootAsapThresholdInMb(),
		m_config.GetRestartUIAfterDropThresholdMb(),
		m_config.GetRestartUIAsapThresholdMb(),
		CDiagnostics::Instance().GetMaxPBTAmount(),
		CDiagnostics::Instance().GetMaxTicketAmount(),
		GetLcdModel(),
		m_sentinelState.GetPendingActionAsString(),
		m_sentinelState.GetReportedEgmProtocolVersion(),
		m_sentinelState.GetEgmSupportsLP7B_Available(),
		m_sentinelState.GetEgmSupportsLP7B(),
		activeMultiplier, 
		activeDivider,
		m_games.GetNumberOfImplementedGames(),
		m_games.GetNumberOfEnabledGames(),
		m_games.GetLastGamelistTimestamp(),
		m_games.IsCompleteGamelistFetched(),
		m_games.GetEGMTransferMode(),
		m_games.GetBonusPaymentMethod(),
		m_sentinelState.GetGameChunkIndexSent(),
		m_sentinelState.GetTotalNumberOfGameChunks(),
		m_config.JackpotA8Reset(),
		m_sentinelState.GetDownloadStatus(),
		m_config.CashoutToHostEnabled(),
		m_sentinelState.GetEGMCashoutToHostStatus(),
		m_sentinelState.GetCashoutToHostRequestsAccepted(),
		m_sentinelState.GetCashoutToHostRequestsCanceled(),
		m_maintenanceMode);

	webDiagnostics.Update(commonInfo, statusInfo, m_playerSession, egmConfig, m_excessiveVoucherOut);
}

void CInternalsFramework::PerformPlayerWebDiagnostics(CWebDiagnostics& webDiagnostics,
		const CWebDiagnosticsCommonInfo& commonInfo)
{
	std::unique_lock<std::recursive_mutex> playerLock( m_playerCriticalSection );
	CPlayer *player(m_player != nullptr ? new CPlayer(*m_player) : nullptr);
	const CPlayerSession playerSession(m_playerSession);
	playerLock.unlock();
	
	webDiagnostics.UpdatePlayerInfo(commonInfo,
		player,
		CPairingInformation::Instance(),
		playerSession,
		m_beaconManager->GetBeacon(),
		m_beaconManager->GetBeaconPower()
		);
	
	delete player;

}

void CInternalsFramework::PerformProgressiveWebDiagnostics(CWebDiagnostics& webDiagnostics,
	const CWebDiagnosticsCommonInfo& commonInfo)
{
    std::unique_lock<std::recursive_mutex> playerLock( m_playerCriticalSection );
	const CProgressiveConfig* progConfig(m_sentinelState.GetProgressiveConfigCopy());
	const CProgressiveConnectionInfo* progInfo(m_sentinelState.GetProgressiveConnectionInfoCopy());
	const CProgressiveHost* progHost(m_sentinelState.GetProgressiveHostCopy());

	const CPlayerSession playerSession(m_playerSession);
	playerLock.unlock();

	std::unique_lock<std::recursive_mutex> egmAwardLock(m_egmAwardCriticalSection);
	const CEGMAward* progAward(m_egmAwardManager.GetEGMAwardCopy());
	const CEGMAwardManagerState progState(m_egmAwardManager.GetEGMAwardManagerState());
	std::shared_ptr<CEGMAward> lastStandardWin;
	std::vector<std::shared_ptr<CEGMAward>> standardWins;
	m_egmAwardManager.GetStandardWins(lastStandardWin, standardWins);

	ProgressiveLevelManager progressiveLevelManager(m_progressiveLevelManager);
	egmAwardLock.unlock();

	SYSTEMTIME localTime;
    CUtilities::GetLocalSystemTime(&localTime);

	webDiagnostics.UpdateProgressive(commonInfo,
		CUtilities::GetTickCount(),
		localTime,
		*progConfig,
		*progInfo,
		*progHost,
		*progAward,
		lastStandardWin,
		standardWins,
		playerSession,
		progState,
		progressiveLevelManager);

	delete progConfig;
	delete progInfo;
	delete progHost;
	delete progAward;
}

void CInternalsFramework::PerformOptionsAndSettingsWebDiagnostics(CWebDiagnostics& webDiagnostics, const CWebDiagnosticsCommonInfo& commonInfo)
{	
	webDiagnostics.UpdateOptionsAndSettings(commonInfo, 
		m_config.GetSubscriptionDataResponse(NO_EXTENSIBLE_SETTINGS_TOPIC, ""), 
		m_config.GetExtensibleSystemSettingsStr());
}

void CInternalsFramework::PerformConfigExtWebDiagnostics(CWebDiagnostics& webDiagnostics,
	const CWebDiagnosticsCommonInfo& commonInfo)
{
	CConfigExtFile configExt(m_config);
	webDiagnostics.UpdateConfigExt(commonInfo, configExt);
}

void CInternalsFramework::PerformQueuingConfigWebDiagnostics(CWebDiagnostics& webDiagnostics,
	const CWebDiagnosticsCommonInfo& commonInfo)
{
	webDiagnostics.UpdateQueuingConfig(commonInfo, m_msgPriority.GetQueuingConfig());
}

void CInternalsFramework::PerformCardReaderDiagnostics(CWebDiagnostics& webDiagnostics,
	const CWebDiagnosticsCommonInfo& commonInfo)
{
	webDiagnostics.UpdateCardReader(commonInfo, m_mcr);
}

void CInternalsFramework::PerformJackpotInfoDiagnostics(
	const CWebDiagnosticsCommonInfo& commonInfo)
{
	unique_lock<recursive_mutex> lock(m_jackpotCriticalSection);
	if (m_jackpot != nullptr)
	{
		CJackpot jackpot(*m_jackpot);
		lock.unlock();

		jackpot.UpdateWebDiagnostics(commonInfo, m_config);
	}
	else
	{
		lock.unlock();
		//just populate the commonInfo instead of showing 404 error page
		CJackpot jackpot;
		jackpot.UpdateWebDiagnostics(commonInfo, m_config);
	}
}

void CInternalsFramework::PerformIPAddressCheck()
{
	//check our IP address
	string ip;
	lock_guard<recursive_mutex> lock(m_adapterCriticalSection);
	{
		m_config.GetActualIP().GetIP(ip);
		//If network cable is changed then its time to restart network services to discover new ip again
		if(ip == "0.0.0.0" || (ip.find("169.") == 0) || NetworkUtils::Instance().GetNetworkCableToggleState())
		{
			LogString(NETWORKING_LOG, "ConfigureNetworkAdapter1");
			ConfigureNetworkAdapter();
		}

		if (m_sentinelIp != ip)
		{
			LogString(NETWORKING_LOG, "[CInternalsFramework::PerformIPAddressCheck] Sentinel IP change detected. "
				"Old: [%s] New: [%s]", m_sentinelIp.c_str(), ip.c_str());
			m_sentinelIp = ip;
			SendMessageToSCF(new CInternalMessage(INT_MSG_SET_STATUS, m_sentinelIp), QUEUE_PRIORITY_NORMAL);
		}

		m_adapterCheckTimeout = CUtilities::GetTickCount();
	}
}

void CInternalsFramework::CheckInWithPoller()
{
	// Check if Poller needs to be pinged
	if (m_sentinelState.IsGameOnline())
	{
		DWORD tickCount = CUtilities::GetTickCount();
		DWORD lastPollerPingTimeSpan;

		lastPollerPingTimeSpan = tickCount - m_lastPollerPing;
		if (m_firstPing && POLLERPINGTIME <= lastPollerPingTimeSpan)
		{
			// 2011-11-17 - FL Manager reports nCompass serial comm down when comm is up.
			// 2012-02-07 - [Because] The Sentinel may send comms online and comms offline out of order.
			// 2012-01-20 - Determined that the easiest way at this time to fix this is to have nCompass send up its comm status 2 minutes after connection to poller and then again every hour after that.
			// 2012-03-06 - To avoid FL logs being filled with 'serial com up' entries, now only send once after 2 minutes.
			CInternalMessage * transMsg = PrepareTransactionMessage(INT_MSG_TRANSACTION_SERIAL_COMM_UP, IsPlayerVirtual());
			transMsg->SetData((WORD)m_EGMConfig.GetEGMProtocol());
			SendMessageToSCF( transMsg, QUEUE_PRIORITY_HIGH );
			m_lastPollerPing = CUtilities::GetTickCount();
			m_firstPing = false;
		}
	}
}


void CInternalsFramework::CheckForMetersSend()
{
	if (m_config.GetGameEndExceptions() && m_config.GetGameInProgress())
	{
		// Delay sending periodic EGM and subgame meters while any game play is in 
		// progress so that we have more probability to end up with final meters 
		return;
	}

	// Check if its time to send EGM meters
	if (m_games.IsTimeToSendMeters(true))
	{
		LogString(GAMES, "[%s]: Sending 304 EGM meters to poller.",
			__FUNCTION__);
		SendGameMetersToSCF(m_games.GetGame(EGM_GAME_NUMBER));
		m_games.UpdateLastMetersSentTime(true);
	}

	// Check if its time to send subgame meters
    if (m_games.IsTimeToSendMeters(false)) 
    {
		LogString(GAMES, "[%s]: Starting thread to send subgame meters to poller.",
			__FUNCTION__);
        StartSubgameMetersUpdateThread();
		m_games.UpdateLastMetersSentTime(false);
    }
}

void CInternalsFramework::WaitForProtocol(IProtocolManager::ProtocolType protocolType)
{
	m_protocolManager->WaitForProtocol(protocolType);
}


static void ReadGCFQueueThreadInner(CInternalsFramework *internals, CQueueList *queueList, binsem &itemAddedEventHandle)
{
	CUtilities::ShowThreadId(_T(THREAD_READ_GCF_QUEUE));

	// Don't read messages from the GCF until persistent objects initialized.
	while ( !internals->HasPersistentObjectsBeenInitialized() )
	{
		CWatchDog::Instance().Checkin();
		CUtilities::Sleep( 100 );
	}

	// Now process any EGM change we might have had in progress before the EGM start.
	internals->ProcessSentinelChangeInProgressBeforeEGMStart();

	// We don't want to send any new transactions until we have done the above step.
	// but we need to wait until the SCF a the UI is initialized
	internals->WaitForProtocol(IProtocolManager::PollerProtocol);
	internals->WaitForProtocol(IProtocolManager::EGMProtocol);

	internals->SendSentinelOnMessages();

	//Use this thread to wait until the we resolved the pointer to the queue to GCF
	internals->InitializeGCF();

	// This loop never ends
	while ( !(internals->ShutdownReadGCF()) )
	{
		// Wait for an item to be added to the queue
		CWatchDog::Instance().Checkin();
        itemAddedEventHandle.try_wait_for(EVENT_WAIT_TIMEOUT);

		// Remove the messages from the queue and give them to the internals object
		//  until there are no more.
		CInternalMessage *msg = (CInternalMessage *)queueList->Remove(); 
		while ( msg != NULL && !(internals->ShutdownReadGCF()) )
		{
			// Give this message to internals to process
			internals->ProcessMessageFromGCF( msg );

			//check in with the watchdog
			CWatchDog::Instance().Checkin();

			// Just in case this loops for a long time, let other threads have a go.
			CUtilities::Sleep(SLEEP_YIELD_MILLISECONDS);

			// Get the next message (if any)
			msg = (CInternalMessage *)queueList->Remove(); 
		}
	}
}

static UINT ReadGCFQueueThread( PVOID threadParam )
{
	CInternalsFramework *internals = (CInternalsFramework *)threadParam;

	CUtilities::SetThreadName(std::this_thread::get_id(), THREAD_READ_GCF_QUEUE);

	// Get the queue of messages from the GCF
	CQueueList *queueList = internals->GetGCFQueue();

	// Get the handle to the event that gets called when an item is put in the queue
	binsem &itemAddedEventHandle = queueList->GetAddedItemEventHandle();

	try
	{
		ReadGCFQueueThreadInner(internals, queueList, itemAddedEventHandle);
	}
    catch (...) 
    {
        CRestartManager::ReportThreadExceptionAndRequestAReboot(THREAD_READ_GCF_QUEUE);
    }

	CWatchDog::Instance().DeregisterThread();

	return 0;
}

static void ReadSCFQueueThreadInner(CInternalsFramework *internals, CQueueList *queueList, binsem &itemAddedEventHandle)
{
	CUtilities::ShowThreadId(_T(THREAD_READ_SCF_QUEUE));

	// Don't read messages from the SCF until persistent objects initialized.
	while (!internals->HasPersistentObjectsBeenInitialized())
	{
		CWatchDog::Instance().Checkin();
		CUtilities::Sleep(100);
	}

	const DWORD EVENT_WAIT_TIMEOUT(1000);
	
	// This loop never ends
	bool firstLoop = true;
	while (!(internals->ShutdownReadSCF()))
	{
		// Wait for an item to be added to the queue
		itemAddedEventHandle.try_wait_for(EVENT_WAIT_TIMEOUT);
		CWatchDog::Instance().Checkin();

		// Remove the messages from the queue and give them to the internals object
		//  until there are no more.
		CInternalMessage *msg = (CInternalMessage *) queueList->Remove();

		while (msg != NULL && !(internals->ShutdownReadSCF()))		//Tracker #25893
		{
			CWatchDog::Instance().Checkin();

			// Give this message to internals to process
			internals->ProcessMessageFromSCF(msg);

			// Just in case this loops for a long time, let other threads have a go.
			CUtilities::Sleep(SLEEP_YIELD_MILLISECONDS);

			// Get the next message (if any)
			msg = (CInternalMessage *) queueList->Remove();
		}

		if (firstLoop)
		{
			firstLoop = false;
		}
		internals->PerformGeneralTimedEvents();

		CUtilities::Sleep(SLEEP_YIELD_MILLISECONDS);
		
	}
}

static UINT ReadSCFQueueThread( PVOID threadParam )
{
	CInternalsFramework *internals = (CInternalsFramework *)threadParam;

	CUtilities::SetThreadName(std::this_thread::get_id(), THREAD_READ_SCF_QUEUE);

	//Use this thread to wait until the we resolved the pointer to the queue to SCF
	internals->InitializeSCF();

	// Get the queue of messages from the SCF
	CQueueList *queueList = internals->GetSCFQueue();

	// Get the handle to the event that gets called when an item is put in the queue
	binsem &itemAddedEventHandle = queueList->GetAddedItemEventHandle();

	try
	{
		ReadSCFQueueThreadInner(internals, queueList, itemAddedEventHandle);
	}
    catch (...) 
    {
		CRestartManager::ReportThreadExceptionAndRequestAReboot(THREAD_READ_SCF_QUEUE);
    }

	CWatchDog::Instance().DeregisterThread();
	return 0;
}

static UINT AFX_CDECL ReadUIQueueThread( PVOID threadParam )
{
	CInternalsFramework *internals = (CInternalsFramework *)threadParam;
	return internals->ReadUIQueue();
}

void CInternalsFramework::ReadUIQueueInner(CQueueList *queueList, binsem &itemAddedEventHandle)
{
	CUtilities::ShowThreadId(_T(THREAD_READ_UI_QUEUE));

	// This loop never ends
	while ( !(ShutdownReadUI()) )
	{
		// Wait for an item to be added to the queue
		itemAddedEventHandle.try_wait_for(EVENT_WAIT_TIMEOUT);
		CWatchDog::Instance().Checkin();

		// Remove the messages from the queue and give them to the internals object
		//  until there are no more.
		CInternalMessage *msg = (CInternalMessage *)queueList->Remove(); 
		while ( msg != NULL && !(ShutdownReadUI()) )		//Tracker #25893
		{
			// Give this message to internals to process
			ProcessMessageFromUI( msg );

			// Just in case this loops for a long time, let other threads have a go.
			CUtilities::Sleep(0);

			// Get the next message (if any)
			msg = (CInternalMessage *)queueList->Remove(); 
		}

		if (!ShutdownReadUI())
		{
			// Set true after we have processed any backlogged messages.
			m_restartManager.SetProcessingUiHeartbeats(true);
		}
	}

	m_restartManager.SetProcessingUiHeartbeats(false);
}

DWORD CInternalsFramework::ReadUIQueue()
{
	CUtilities::SetThreadName(CUtilities::GetCurrentThreadId(), THREAD_READ_UI_QUEUE);

	//Use this thread to wait until the we resolved the pointer to the queue to UI
	InitializeUI();

	// Get the queue of messages from the UI
	CQueueList *queueList = GetUIQueue();

	// Get the handle to the event that gets called when an item is put in the queue
	binsem &itemAddedEventHandle = queueList->GetAddedItemEventHandle();

	try
	{
		ReadUIQueueInner(queueList, itemAddedEventHandle);
	}
    catch (...) 
    {
		CRestartManager::ReportThreadExceptionAndRequestAReboot(THREAD_READ_UI_QUEUE);
    }

	CWatchDog::Instance().DeregisterThread();

	return 0;
}

static void ReadProgressiveQueueThreadInner(CInternalsFramework *internals, CQueueList *queueList, binsem& itemAddedEventHandle)
{
	CUtilities::ShowThreadId(_T(THREAD_READ_PROGRESSIVE_QUEUE));

	// This loop never ends
	while (!(internals->ShutDownReadProgressive()))
	{
		// Wait for an item to be added to the queue
		itemAddedEventHandle.try_wait_for(EVENT_WAIT_TIMEOUT);
		CWatchDog::Instance().Checkin();

		// Remove the messages from the queue and give them to the internals object
		//  until there are no more.
		CInternalMessage *msg = (CInternalMessage *)queueList->Remove();
		while (msg != NULL && !(internals->ShutDownReadProgressive()))		//Tracker #25893
		{
			CWatchDog::Instance().Checkin();
			// Give this message to internals to process
			internals->ProcessMessageFromProgressive(msg);

			// Just in case this loops for a long time, let other threads have a go.
			CUtilities::Sleep(0);

			// Get the next message (if any)
			msg = (CInternalMessage *)queueList->Remove();
		}
	}
}

static UINT AFX_CDECL ReadProgressiveQueueThread(PVOID threadParam)
{
	CInternalsFramework *internals = (CInternalsFramework *)threadParam;

	CUtilities::SetThreadName(CUtilities::GetCurrentThreadId(), THREAD_READ_PROGRESSIVE_QUEUE);
	//Use this thread to wait until we resolved the pointer to the queue to Progressive
	internals->InitializeProgressive();

	// Get the queue of messages from the Progressive
	CQueueList *queueList = internals->GetProgressiveQueue();

	// Get the handle to the event that gets called when an item is put in the queue
	binsem &itemAddedEventHandle = queueList->GetAddedItemEventHandle();

	try
	{
		ReadProgressiveQueueThreadInner(internals, queueList, itemAddedEventHandle);
	}
    catch (...) 
    {
		CRestartManager::ReportThreadExceptionAndRequestAReboot(THREAD_READ_PROGRESSIVE_QUEUE);
    }

	CWatchDog::Instance().DeregisterThread();

	return 0;
}

static UINT AFX_CDECL ProgressiveCacheThread(LPVOID threadParam)
{
	CInternalsFramework *internals((CInternalsFramework *)threadParam);
	try
	{
		internals->ProgressiveCacheThreadInner(internals);
	}
    catch (...) 
    {
		CRestartManager::ReportThreadExceptionAndRequestAReboot(THREAD_PROGRESSIVE_CACHE_INTERNALS);
    }

	CWatchDog::Instance().DeregisterThread(CUtilities::GetCurrentThreadId());
	return 0;
}


void BootupSuccessful()
{
	// delete bootcnt file so updates don't rollback and new updates continue to happen
	std::string const bootcnt = "/bootcnt.txt";
	std::string const upgradeScript = "/s5-update.txt";
	std::string const upgradeSmd = "/" + (std::string)CPlatform::Instance().GetUpgradeSMDfilename();

	std::string upgradePath = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_UPGRADE);
	std::string bootcntPathName = upgradePath + bootcnt;
	bool couldDelete = CUtilities::DeleteFile(bootcntPathName.c_str());
	if (couldDelete)
	{
		LogString("GENERAL", "Successfully deleted bootcnt.txt file at startup.");
	}
	std::string upgradeSmdPathName = upgradePath + upgradeSmd;
	couldDelete = CUtilities::DeleteFile(upgradeSmdPathName.c_str());
	if (couldDelete)
	{
		LogString("GENERAL", "Successfully deleted S5Upgrade.smd file at startup.");
	}
	std::string upgradeScriptPathName = upgradePath + upgradeScript;
	couldDelete = CUtilities::DeleteFile(upgradeScriptPathName.c_str());
	if (couldDelete)
	{
		LogString("GENERAL", "Successfully deleted s5-update.txt file at startup.");
	}

	// delete backup directory if it exists
	CUtilities::DeleteDirectory(CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_UPGRADE) + "\\backup");
}

bool CInternalsFramework::IsDoorOpen( int doorMeterNumber )
{
	bool bDoorOpen = false;

	// If the specified meter is odd, the door is open 
	CGame *game = m_games.GetGame( EGM_GAME_NUMBER );
	if ( game != NULL && (game->GetMeter( (byte)doorMeterNumber ) % 2 == 1) )
	{
		bDoorOpen = true;
	}

	return bDoorOpen;
}

static void ReadCardThreadInner(CInternalsFramework *internals)
{
	CUtilities::ShowThreadId(THREAD_READ_CARD);

	// This loop forever
	internals->CardProcessingLoop();
}


static UINT ReadCardThread( PVOID threadParam )
{
	CInternalsFramework *internals = (CInternalsFramework *)threadParam;

	CUtilities::SetThreadName(std::this_thread::get_id(), THREAD_READ_CARD);

	try
	{
		ReadCardThreadInner(internals);
	}
    catch (...) 
    {
		CRestartManager::ReportThreadExceptionAndRequestAReboot(THREAD_READ_CARD);
    }

	CWatchDog::Instance().DeregisterThread();

	return 0;
}

static void ReadCardReaderThreadInner(CInternalsFramework *internals)
{
	CUtilities::ShowThreadId(THREAD_READ_CARD_READER);

	// This loop forever
	internals->CardReaderProcessingLoop();
}

static UINT ReadCardReaderThread( PVOID threadParam )
{
	CInternalsFramework *internals = (CInternalsFramework *)threadParam;

	CUtilities::SetThreadName(std::this_thread::get_id(), THREAD_READ_CARD_READER);

	try
	{
		ReadCardReaderThreadInner(internals);
	}
    catch (...) 
    {
		CRestartManager::ReportThreadExceptionAndRequestAReboot(THREAD_READ_CARD_READER);
    }

	CWatchDog::Instance().DeregisterThread();

	return 0;
}

static void UpdatePollerWithSubgameMetersInner(PVOID threadParam)
{
	CUtilities::ShowThreadId(_T(THREAD_UPDATE_POLLER_WITH_SUBGAME_METERS));
	CInternalsFramework *internals = (CInternalsFramework *)threadParam;
	internals->SendSubgameMetersToSCF();
}

static UINT AFX_CDECL UpdatePollerWithSubgameMeters(PVOID threadParam)
{
	CUtilities::SetThreadName(CUtilities::GetCurrentThreadId(), 
		THREAD_UPDATE_POLLER_WITH_SUBGAME_METERS);

	try
	{
		UpdatePollerWithSubgameMetersInner(threadParam);
	}
    catch (...) 
    {
		CRestartManager::ReportThreadExceptionAndRequestAReboot(THREAD_UPDATE_POLLER_WITH_SUBGAME_METERS);
    }

	return 0;
}


void CInternalsFramework::DumpMemoryThreadInner(CInternalsFramework *internals)
{
	CUtilities::SetThreadName(CUtilities::GetCurrentThreadId(), THREAD_DUMP_MEMORY);
	CUtilities::ShowThreadId(_T(THREAD_DUMP_MEMORY));

	//this will loop for ever
	internals->PerformMemoryDumps();
}

UINT AFX_CDECL CInternalsFramework::DumpMemoryThread( PVOID threadParam )
{
	CInternalsFramework *internals((CInternalsFramework *)threadParam);

	try
	{
		DumpMemoryThreadInner(internals);
	}
    catch (...) 
    {
		CRestartManager::ReportThreadExceptionAndRequestAReboot(THREAD_DUMP_MEMORY);
    }

	CWatchDog::Instance().DeregisterThread();
	return 0;
}

void CInternalsFramework::PerformControlledUIRestartCheck()
{
	if (!CUtilities::IsAutomationMode() && m_displayManager->OKToRestartUI())
	{
		lock_guard<recursive_mutex> lock(m_playerCriticalSection);
		if (!m_pairingInformation.IsWaitingForPairing() &&
			!m_pairingInformation.IsWaitingForRequestCardChange() &&
			!m_pairingInformation.IsMobileCardedSession() && 
			m_restartManager.PerformControlledUIRestartCheck(m_player, m_employee1,
			m_playerSession.GetLastCreditMeterZeroTime(),
			m_playerSession.HasSignificantCredits(),
			m_sentinelState.IsMeterCaptureInProgress(), m_config.GetRebootUiRestartOverrideEnabled()))
		{
			m_sentinelState.SetCurrentActivity(CurrentActivity::RestartingUi);
			SendDashboardStatusToProgressive();

            m_displayManager->MakeDisplayFullEGM();
			m_sentinelState.ClearPendingAction(RestartUI);
			SendMessageToUI(new CInternalMessage(INT_MSG_RESTART_SERVICEWINDOW), QUEUE_PRIORITY_HIGHEST);
		}
	}
}

void CInternalsFramework::PerformControlledUIRefreshCheck()
{
	if (!CUtilities::IsAutomationMode() && m_displayManager->OKToRestartUI())
	{
		lock_guard<recursive_mutex> lock(m_playerCriticalSection);
		if (!m_pairingInformation.IsWaitingForPairing() &&
			!m_pairingInformation.IsWaitingForRequestCardChange() &&
			!m_pairingInformation.IsMobileCardedSession() && 
			m_restartManager.PerformControlledUIRefreshCheck(m_player, m_employee1,
			m_playerSession.GetLastCreditMeterZeroTime(),
			m_playerSession.HasSignificantCredits(),
			m_sentinelState.IsMeterCaptureInProgress()))
		{
			// Also check if there is any pending skin which is ready to be applied as active skin.
			// If yes, exchange it with active skin folder before we send RefreshUI command to the UI.
			CPackageInfo pendingSkinInfo = CMediaPackageManager::Instance().GetPendingSkinInfo();
			const std::string pendingSkinName = pendingSkinInfo.GetName();
			if (!pendingSkinName.empty() && 0 == pendingSkinInfo.GetPendingMediaCount())
			{
				CPackageInfo activeSkinInfo = CMediaPackageManager::Instance().MakePendingSkinAsActiveSkin();
				if (pendingSkinInfo == activeSkinInfo)
				{
					m_sentinelState.SetSkinInfo(pendingSkinInfo.GetId(), pendingSkinName, pendingSkinInfo.GetVersion(), pendingSkinInfo.GetRevision());

					// extrct NCD files.
					CMediaPackageManager::Instance().ExtractNCDFiles();
				}
			}

			m_sentinelState.ClearPendingAction(RefreshUI);
			SendDashboardStatusToProgressive();

			m_sentinelState.m_lastUIRefreshTime = CUtilities::GetCurrentTime();
			++m_sentinelState.m_uiRefreshCount;

			SendSubscriptionUpdateIfNecessary();
			LogString(FLASHING, "[%s] Sending INT_MSG_REFRESH_UI msg to UI...", __FUNCTION__);
			SendMessageToUI(new CInternalMessage(INT_MSG_REFRESH_UI), QUEUE_PRIORITY_HIGHEST);
		}
	}
}

void CInternalsFramework::PopulateConnection(CConnection &connection)
{
	string tempStr;
	CIPAddress tempIP;

	m_config.GetActualIP(); // Get a current IP address from the operating system.

	connection.SetSlotNumber(m_config.GetSlotNumber());

	connection.SetSlotmastID(m_config.GetSlotMastID());

	tempStr = m_config.GetHostAddressOrName();
	connection.SetHost(tempStr);
	connection.SetPort(m_config.GetHostPort());

	m_config.GetMyIP(tempIP);
	connection.SetMyIP(tempIP);

	connection.SetActualIP(m_config.GetActualIP());

	connection.SetDHCP(m_config.GetDHCPConfig());

	m_config.GetSubnetMask(tempIP);
	connection.SetSubnetMask(tempIP);

	m_config.GetDefaultGateway(tempIP);
	connection.SetDefaultGateway(tempIP);

	m_config.GetPreferredDNSServer(tempIP);
	connection.SetPreferredDNS(tempIP);

	m_config.GetAlternateDNSServer(tempIP);
	connection.SetAlternateDNS(tempIP);

	connection.SetMacAddress(m_config.GetMACAddressAsString());
	m_config.GetActualIP();
}

CInternalMessage * CInternalsFramework::CreateConnectionDataMessage()
{

	// Pass connection data to progressive protocol
	CConnection *connection = new CConnection();
	PopulateConnection(*connection);

	return (new CInternalMessage(INT_MSG_SET_CONNECTION_DATA, connection));
}

void CInternalsFramework::SendSubscriptionUpdateIfNecessary()
{
	string mixerVersion, touchVersion;
	m_displayManager->GetFirmwareVersions(mixerVersion, touchVersion);
	m_config.SetMixerFirmwareVersions(mixerVersion, touchVersion);

    if (m_tickets != NULL) // might be null during wait-for-reboot
		m_tickets->SetVoucherBufferingEnabled(m_config.TicketBufferingEnabled());

	lock_guard<recursive_mutex> lock(m_playerCriticalSection);
	m_sentinelState.SetVirtualPlayerFlag(IsPlayerVirtual());
	std::vector<CSubscription>* subscriptions = new std::vector<CSubscription>;
	m_dataSubscriptionManager.GetSubscriptionData(*subscriptions);
	if (subscriptions->size() > 0)
	{
		CInternalMessage* msg(new CInternalMessage(INT_MSG_SUBSCRIPTION_UPDATES, subscriptions));
		msg->SetData((WORD)m_notificationBox.KeepNotificationBoxUpFront());
		SendMessageToUI(msg, QUEUE_PRIORITY_HIGHEST);
	}
	else
	{
		delete subscriptions;
	}
}

void CInternalsFramework::SetFirmwareVersion( )
{
	CVideoMixerUpgrade *vmUpgrade = nullptr;
	CCarrierUpgrade *cUpgrade = nullptr;
	FindFirmware(vmUpgrade, cUpgrade);

	string mixerTarget = vmUpgrade != nullptr ? vmUpgrade->GetUpgradeVersionString() : "";
	string carrierTarget = cUpgrade != nullptr ? cUpgrade->GetUpgradeVersionString() : "";
	
	// Determine if upgrade available for UI
	bool upgradeAvailable = ((vmUpgrade && !vmUpgrade->IsCurrent() && CDisplayManager::Instance().GetDisplayPlatform() == VideoMixerPlatform) || (cUpgrade && !cUpgrade->IsCurrent()));
	if (upgradeAvailable != m_firmwareAvailable)
	{
		m_firmwareAvailable = upgradeAvailable;
		SendMessageToUI(new CInternalMessage(INT_MSG_SET_FIRMWARE_UPGRADE_AVAILABLE, (WORD)upgradeAvailable), QUEUE_PRIORITY_NORMAL);
	}

	if (vmUpgrade != nullptr)
	{
		if (CDisplayManager::Instance().GetDisplayPlatform() == VideoMixerPlatform)
		{
			m_config.SetMixerFirmwareVersion(vmUpgrade->GetCurrentVersionString());
		}
		else
		{
			RemoveMixerUpgradeFiles();
		}
	}

	m_config.SetTargetFirmwareVersions((upgradeAvailable && CDisplayManager::Instance().GetDisplayPlatform() == VideoMixerPlatform) ? mixerTarget : "", upgradeAvailable ? carrierTarget : "");
	
	delete vmUpgrade;
	delete cUpgrade;
}

void CInternalsFramework::RemoveMixerUpgradeFiles()
{
	CVideoMixerUpgrade *tmpvmUpgrade = nullptr;
	CCarrierUpgrade *tmpcUpgrade = nullptr;
	string path = FindFirmware(tmpvmUpgrade, tmpcUpgrade);

	if ((!tmpvmUpgrade || CDisplayManager::Instance().GetDisplayPlatform() != VideoMixerPlatform) &&
		(!tmpcUpgrade || tmpcUpgrade->IsCurrent()))
	{
		// Remove FPGA folder from media disk
		// Don't want to remove FPGA folder from USB stick so make sure path doesn't contain "sd"
		if (!path.empty() && CUtilities::DoesDirExist(path.c_str()) && path.find("sd") == std::string::npos && !CUtilities::DeleteDirectory(path))
		{
			// Log error
			LogString(ERROR_LOG, "RemoveMixerUpgradeFiles-Could not remove FPGA folder %s.", path.c_str());
		}
	}
	delete tmpvmUpgrade;
	delete tmpcUpgrade;
}


void CInternalsFramework::SendMGMDupdateIfNecessary()
{
	bool mgmdAvailable = false, mgmdGlobalOption = false;

	if(CPollerMGMDData::Instance().GetMGMDAvailable())
		mgmdAvailable = true;
	if (IsGlobalMGMDOptionEnabled())
		mgmdGlobalOption = true;

	if ((mgmdAvailable != m_MGMDAvailable) || (mgmdGlobalOption != m_MGMDGlobalOption))
	{
		m_MGMDAvailable = mgmdAvailable;
		m_MGMDGlobalOption = mgmdGlobalOption;
		
		// Send a message to send status to poller
		CInternalMessage *msg = new CInternalMessage(INT_MSG_SET_STATUS);
		SendMessageToSCF(msg, QUEUE_PRIORITY_NORMAL);
	}
}

bool CInternalsFramework::IsGlobalMGMDOptionEnabled()
{
	return (m_config.GetGlobalMGMDEnabled());
}

void CInternalsFramework::ProcessRemoteReboot(CInternalMessage *msg)
{
	byte rebootType = (byte)msg->GetWordData();

	GenerateTilt(tltRebootRemoteRequest);

	switch(rebootType)
	{
		case REBOOT_NO_ACTIVE_SESSION:
			LogString(GENERAL, "Begin REBOOT_NO_ACTIVE_SESSION");
			m_sentinelState.SetPendingAction(RemoteReboot);
			SendDashboardStatusToProgressive();
			m_restartManager.SetFirstTimeAfterRemoteRebootRequest(true);
			m_restartManager.SetToPerformControlledRemoteReboot();
			break;
		
		case REBOOT_ASAP:
			LogString(GENERAL, "Begin REBOOT_ASAP");
			m_restartManager.SetRemoteRebootASAP(true);
			
			m_sentinelState.SetCurrentActivity(CurrentActivity::Rebooting);
			SendDashboardStatusToProgressive();

			SendTransactionMessageToSCF(PrepareTransactionMessage(INT_MSG_TRANSACTION_CONTROLLED_REBOOT, IsPlayerVirtual()), QUEUE_PRIORITY_NORMAL);
			m_notificationBox.AddAlert(Alerts::ALERT_REBOOT_IN_PROGRESS);
			SendSubscriptionUpdateIfNecessary();

			CUtilities::Sleep(ALLOW_SPEED_MEDIA_STATUS_TO_BE_SENT);
			// Start the Reboot process
			BeginRebootProcess();
			break;
		
		default:
			LogString(ERROR_LOG, _T("Unexpected Reboot type in Message type: %d  %d" ), (int)msg->GetMessageType(), (int)rebootType);
	
	}
}

void CInternalsFramework::PerformPollerLoadBalancing()
{
	// fetch current local time.
	SYSTEMTIME localTime;
	CUtilities::GetLocalSystemTime(&localTime);

	if (!m_IsItPollerLoadBalancingWindow)
	{
		// Checking if it is time for load balancing
		if (localTime.wHour == m_PollerLoadBalanceTime_Hour && localTime.wMinute == m_PollerLoadBalanceTime_Minute)
		{
			m_IsItPollerLoadBalancingWindow = true;

			if (m_playerSession.GetPlayerTypeEnum() == PlayerTypeState::PLAYERTYPEIDLE)  // means no session in progress
			{
				LogString(POLLER_LOG, "[LB] Internals : It is poller load balancing time [SecondsLag: %d]. "
					"Sending INT_MSG_PERFORM_LOAD_BALANCING to poller...", localTime.wSecond);

				// Generate tilt indicating that we are about to do load balance reconnect
				GenerateTilt(tltPollerLoadBalanceReconnect);

				SendMessageToSCF(new CInternalMessage(INT_MSG_PERFORM_LOAD_BALANCING), QUEUE_PRIORITY_NORMAL);
			}
			else
			{
				// Generate tilt indicating that load balancing reconect is denied
				GenerateTilt(tltPollerLoadBalanceReconnectDenied);
				LogString(POLLER_LOG, "[LB] Internals : It is poller load balancing time [SecondsLag: %d], "
					"but session is not idle. So deny load balancing.", localTime.wSecond);
			}
		}
	}
	else
	{
		if (localTime.wMinute != m_PollerLoadBalanceTime_Minute)
		{
			m_IsItPollerLoadBalancingWindow = false;
		}
	}
}

void CInternalsFramework::PerformRolloverMeterCheck()
{
	if (m_sentinelState.PerformRolloverMeterCheck())
	{
		LogString(POLLER_LOG, "PerformRolloverMeterCheck BeginMeterCapture MachineRollover");
		BeginMeterCapture(MachineRollover, false, false);
	}
}

void CInternalsFramework::UpdateCountdownDisplay()
{
	if (m_player != NULL && m_notificationBox.GetNotificationState() == NotificationState::PLAYER_IN_PLAY)
	{
		if (m_countdownDisplayTickCount == 0)
		{
			m_countdownDisplayTickCount = CUtilities::GetTickCount();
		}
		else
		{
			DWORD currentTickCount = CUtilities::GetTickCount();
			DWORD displayInterval = m_config.GetCountdownDisplayIntervalInSeconds() * MILLISECONDS_PER_SECOND;

			if (currentTickCount >= m_countdownDisplayTickCount + displayInterval)
			{
				m_countdownDisplayTickCount = currentTickCount;

				m_player->MoveToNextBucket();
				SendPlayerDataToUI(INT_MSG_PLAYER_UPDATE, false);
			}
		}
	}
	else
	{
		m_countdownDisplayTickCount = 0;
	}
}

bool CInternalsFramework::IsNFDPoller()
{
	return m_config.IsNFDPoller();
}

void CInternalsFramework::ProcessTilt(CInternalMessage *msg)
{
	GenerateTilt((TiltCode)msg->GetWordData());
}

void CInternalsFramework::ProcessDefaultDataDirectoryUpdated(const std::string& onelinkConfigMsgRefId, const std::string& onelinkConfigFileName)
{
	SendMessageToProgressive(new CInternalMessage(
		INT_MSG_DEFAULT_DATA_DIRECTORY_UPDATED, onelinkConfigMsgRefId, onelinkConfigFileName), QUEUE_PRIORITY_HIGHEST);
	SendMessageToSCF(new CInternalMessage(INT_MSG_DEFAULT_DATA_DIRECTORY_UPDATED), QUEUE_PRIORITY_HIGHEST);

	m_config.LoadConfigExtFile();
	SendUIConfigToUI();

	RefreshConfigs();
}

void CInternalsFramework::RefreshConfigs()
{
	// Update player session and MCRWrapper objects
	{
		// Use m_playerCriticalSection to synchronize m_playerSession access and subsequent result processing.
		lock_guard<recursive_mutex> lock(m_playerCriticalSection);
		m_playerSession.UpdateConfigItems(m_config);
	}

	m_mcrAuto->RefreshConfig(m_config);
	m_mcr->RefreshConfig(m_config);

	if (m_tcpMcr != nullptr)
	{
		m_tcpMcr->RefreshConfig(m_config);
	}

	m_msgPriority.BuildMessagePriorityMap();

	m_protocolManager->SetInboundQueueEqualPriorityInterrupt(IProtocolManager::UIProtocol, m_msgPriority.GetEqualPriorityInterrupt());
}

void CInternalsFramework::SendUIConfigToUI()
{
	CUIConfig* uiConfig(new CUIConfig(m_config, m_notificationBox.IsMobileEnabled()));
	m_notificationBox.SetUIConfig(*uiConfig);
	CInternalMessage* msg(new CInternalMessage(INT_MSG_UPDATE_UI_CONFIG, uiConfig));
	SendMessageToUI(msg, QUEUE_PRIORITY_NORMAL);
}

void CInternalsFramework::SetCurrentActivity(CurrentActivity currentActivity)
{
	m_sentinelState.SetCurrentActivity(currentActivity);
}