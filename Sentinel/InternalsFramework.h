#pragma once
#include "IProtocolManager.h"
#include "InternalMessageQueueList.h"
#include "InternalMessage.h"
#include "Games.h"
#include "Config/Config.h"
#include "Config/EGMConfig.h"
#include "Card.h"
#include "Player.h"
#include "Employee.h"
#include "Tickets.h"
#include "Config/ConfigSenOption1Field.h"
#include "Config/ConfigSenOption2Field.h"
#include "Config/ConfigFeatureFlagField.h"
#include "TiltsDef.h"
#include "UI/TransactionLog.h"
#include "PersonalBanker.h"
#include "PersonalBankerInfo.h"
#include "PersonalBankerTransfer.h"
#include "ProcessDump/MemoryThresholds.h"
#include "ProcessDump/WebDiagnostics.h"
#include "GlobalDefs.h"
#include "RunawayStatus.h"
//#include "Hardware/ChassisDoor.h"
#include "Jackpot.h"
#include "State/SentinelState.h"
#include "Hardware/MCRAutoTest.h"
#include "Hardware/MCRWrapper.h"
#include "BadReadCount.h"
#include "Config/ConfigLossLimitField.h"
#include "DisplayManager.h"
#include "Config/ConfigUpdateMMTField.h"
#include "Config/MediaWindowConfig.h"
#include "TicketPBTMaxAmount.h"
#include "ProgressiveProtocol/ProgressiveCriticalText.h"
#include "PlayerSession.h"
#include "MessagePrioritizer.h"
#include "TiltManager.h"
#include "PlayerUpdateManager.h"
#include "RestartManager.h"
#include "DataSubscriptionManager.h"
#include "MsgTargetPair.h"
#include "VideoMixerUpgrade.h"
#include "CarrierUpgrade.h"
#include "MediaPackageManager.h"
#include "UI/NotificationBox.h"
#include "UI/NotificationTextConfig.h"
#include "Util/TcpMcrWrapper.h"
#include "Hardware/NetworkUtils.h"
#include "PollerProtocol/MobileConnectProtocol/PairingInformation.h"
#include "PollerProtocol/MobileConnectProtocol/BeaconManager.h"
#include "PollerProtocol/MobileConnectProtocol/MobilePBTSessionInfo.h"
#include "ExcessiveVoucherOut.h"
#include "UI/SentinelDlgState.h"
#include "UI/BonusCounterState.h"
#include "ProgressiveProtocol/DashboardIdentifiers.h"

using namespace std;

#define NETWORK_ADAPTER_NAME _T("DM9CE1")

// Forward declare class to declare friend for unit test access.
namespace SentinelNativeUnitTests { class InternalsFrameworkUnitTests; }
namespace SentinelNativeUnitTests { class PollerProtocolUnitTests; }
class CInternalsFrameworkMock;

class CInternalsFramework 
{
public:

	//To give Access to UT cases.
	friend class SentinelNativeUnitTests::InternalsFrameworkUnitTests;
        friend class SentinelNativeUnitTests::PollerProtocolUnitTests;
	friend class CInternalsFrameworkMock;
	
// #pragma region public constants
	static const int LCD_MISSING_COUNT_MAXIMUM = 10;
// #pragma endregion public constants

// #pragma region constructors
	/// <summary>
	/// Constructor. Initializes a new instance of the <see cref="CInternalsFramework"/> class.
	/// </summary>
	/// <param name="startupTime">IN - The startup time.</param>
	/// <param name="protocolManager">IN - The protocol manager.</param>
	/// <param name="firmwareVersion">IN - The firmware version.</param>
	CInternalsFramework(IProtocolManager *protocolManager, time_t startupTime, bool isLcd, const std::string& firmwareVersion);
// #pragma endregion constructors

// #pragma region destructor
	/// <summary>
	/// Destructor.
	/// </summary>
	virtual ~CInternalsFramework(void);
// #pragma endregion destructor

	/// <summary>
	/// Gets a pointer to the CQueueList for messages from the SAS Protocol to Internals.
	/// </summary>
	/// <returns>A pointer to the CQueueList for messages from the SAS Protocol to Internals.</returns>
	/// <remarks>
	/// This returns a NON-null pointer (after the constructor completes).
	/// </remarks>
	CQueueList * GetGCFQueue(void)
	{
		return m_gcfToInternalsQueue;
	}

	/// <summary>
	/// Gets a pointer to the CQueueList for messages from the Poller Protocol to Internals.
	/// </summary>
	/// <returns>A pointer to the CQueueList for messages from the Poller Protocol to Internals.</returns>
	/// <remarks>
	/// This returns a NON-null pointer (after the constructor completes).
	/// </remarks>
	CQueueList * GetSCFQueue(void)
	{
		return m_scfToInternalsQueue;
	}


	CQueueList * GetUIQueue(void)
	{
		return m_uiToInternalsQueue;
	}
	
	CQueueList * GetThirdPartyUIQueue(void)
	{
		return m_thirdPartyUIToInternalsQueue;
	}

	bool GetShutdownTimeKeeper();
	
	/// <summary>
	/// Gets a pointer to the CQueueList for messages from the Progressive Protocol to Internals.
	/// </summary>
	/// <returns>A pointer to the CQueueList for messages from the Progressive Protocol to Internals.</returns>
	/// <remarks>
	/// This returns a NON-null pointer (after the constructor completes).
	/// </remarks>
	CQueueList * GetProgressiveQueue(void)
	{
		return m_progressiveToInternalsQueue;
	}

	bool HasPersistentObjectsBeenInitialized ( void ) 
	{
		return m_bPersistentObjectsInitialized;
	}

	void InitializeGCF( void );
	void UninitializeGCF( void );

	void InitializeSCF( void );
	void UninitializeSCF( void );

	void InitializeUI( void );
	void UnitializeUI( void );

     //Making virtual for UT
	virtual void Start( void );

	void InitializeProgressive(void);
	void UninitializeProgressive(void);

	EGMProtocol GetEGMProtocolType( void );

	void ProcessMessageFromGCF( CInternalMessage *msg );

	/// <summary>
	/// Process a message from the systems communications framework.
	/// </summary>
	/// <param name="msg">REF - pointer to message to be processed. Caller MUST NOT access message after call.</param>
	void ProcessMessageFromSCF( CInternalMessage *msg );

	void ProcessMessageFromUI( CInternalMessage *msg );

	/// <summary>
	/// Processes a message from the progressive queue.
	/// </summary>
	/// <param name="msg">IN/OUT - The message.</param>
	void ProcessMessageFromProgressive(CInternalMessage *msg);

	void CalcImageChecksum();

	void CardReaderProcessingLoop( void );

	void CardProcessingLoop( void );
	DWORD ReadUIQueue();
	
	void WaitForProtocol(IProtocolManager::ProtocolType protocolType);

	/// <summary>
	/// Gets whether the Global MGMD option is enabled.
	/// </summary>
	/// <returns>true if the Global MGMD option is enabled, false otherwise.</returns>
	bool IsGlobalMGMDOptionEnabled();
	
private:	
	void ReadUIQueueInner(CQueueList *queueList, binsem& itemAddedEventHandle);

	void McrSetBezelStyle(CARD_STYLE style, DWORD m_rankId = CPlayerInfo::INVALID_RANK_ID);
	void McrSetBezelStyle(MACHINE_STATE machineState);
	CARD_STYLE GetBezelStyle(MACHINE_STATE machineState);

	void RemoveMixerUpgradeFiles();

	CPlayer* GetPlayerData();

	PackageOrientation GetPackageOrientation() const;

		
public:
	void PerformControlledRebootCheck();
	void PerformGeneralTimedEvents( void );

	void SendStartupMessageToUI( void );
	void InitialzePersisentObjects( void );
	void StartProcessingCards();
	void ProcessSentinelChangeInProgressBeforeEGMStart( void );
	void ProcessSentinelChangeInProgressAfterEGMStart( void );
	void SendSentinelOnMessages( void );

	/// <summary>
	/// This gets called at start-up to process a pending personal banker transaction.
	/// </summary>
	void ProcessInitialPersonalBanker( void );

	//Lets the thread know if it's time to shut down
	bool ShutdownReadGCF()
	{
		return m_shutdownReadGCF;
	}
	//Lets the thread know if it's time to shut down
	bool ShutdownReadSCF()
	{
		return m_shutdownReadSCF;
	}
	//Lets the thread know if it's time to shut down
	bool ShutdownReadUI()
	{
		return m_shutdownReadUI;
	}
	bool ShutDownReadThirdPartyUI()
	{
		return m_shutdownReadThirdPartyUI;
	}
	bool ShutDownReadProgressive()
	{
		return m_shutdownReadProgressive;
	}

	bool ShutDownPolicySvr()
	{
		return m_shutdownpolicySvr;
	}


	bool FileValid(const string& path, const string& filename, int checksum);
	bool CalcFileChecksum(std::string filename, DWORD &outChecksum, bool simple);
	std::recursive_mutex m_validateFilesCriticalSection;

	void CheckInWithPoller();
    //To execute ProgressiveCacheThreadInner from static worker function ProgressiveCacheThread, this function is made public 
    //void ProgressiveCacheThreadInner(CInternalsFramework *internals);

    //To Release m_progressiveCacheThread thread 
    void ReleaseProgressiveCacheThread( void )
    {
        if (m_progressiveCacheThread.get() != nullptr)
        {
            delete this->m_progressiveCacheThread.release();
            m_progressiveCacheThread = nullptr;
        }
    }

    // Progressive cache thread.
	// Want an internals thread that should complete every loop in less than 0.5 seconds.
	// Want a thread that is not the ReadProgressiveQueueThread/InternalsFramework thread,
	// so the ReadProgressiveQueueThread/InternalsFramework thread can process messages as quickly as practical.
	static void ProgressiveCacheThreadInner(CInternalsFramework *internals);

	void UpdateFirmareDownloadStatus(CFlashStatus status);

private:
// #pragma region private constants
	static const DWORD ADAPTER_CHECK_TIMEOUT = 60000; //60 seconds
	static const DWORD ADAPTER_CHECK_STARTUP_TIMEOUT = (ADAPTER_CHECK_TIMEOUT * 2); //at startup I want this to start 2 mins after startup

	// Advance the end time for virtual sessions -- because if the virtual session starts and ends at the exact same time as the player end session 
	// then the PMT treats the virtual card out as a duplicate transaction.
	static const int VIRTUAL_SESSION_END_ADD_SECONDS = 1; // Allowed range is 0-59.

	// Constants for EFT transaction ID length.
	static const int EFT_TRANS_ID_LEN_MINIMUM = 1; // EFT transaction ID has to be 1-3 digits in length.
	static const int EFT_TRANS_ID_LEN_MAXIMUM = 3; // EFT transaction ID has to be 1-3 digits in length.

	// For RFID, device indicator will turn Blue momentarily indicating existing player session end and then turns green to indicate Employee session in progress.
	static const DWORD DISPLAY_PLAYER_OUT_EMPLOYEE_IN_PERIOD = 1000;

	static const __int64 MINIMUM_LAST_COINS_PLAYED = 0;
	static const __int64 MAXIMUM_LAST_COINS_PLAYED = LONG_MAX;
	static const __int64 MINIMUM_LAST_COINS_WON = 0;
	static const __int64 MAXIMUM_LAST_COINS_WON = LONG_MAX;
// #pragma endregion private constants

    IProtocolManager *m_protocolManager;

	// The queue for messages coming from the GCF
	CQueueList *m_gcfToInternalsQueue;

	// The queue for messages coming from the SCF
	CQueueList *m_scfToInternalsQueue;

	// The queue for messages coming from the UI
	CQueueList *m_uiToInternalsQueue;

	// The queue for messages coming from the ThirdPartyUI
	CQueueList *m_thirdPartyUIToInternalsQueue;

	// The queue for messages coming from the progressive protocol
	CQueueList *m_progressiveToInternalsQueue;

	// Is the sentinel in TFT mode
	int m_lcdMissingCount;
	bool m_lcdIsPresent;
	bool m_lcdDefaultSet;
	bool m_lcdGetStatusFailed;
	// THE games object.
	CGames m_games;

	//Threads
	std::unique_ptr<std::thread> m_ndiThread;
	bool		 m_shutdownNDI;
	std::unique_ptr<std::thread> m_readCardThread;
	bool		 m_shutdownReadCard;
	std::unique_ptr<std::thread> m_readCardReaderThread;
	bool		 m_shutdownReadCardReader;
	std::unique_ptr<std::thread>  m_readGCFThread;
	bool		 m_shutdownReadGCF;
	std::unique_ptr<std::thread>  m_readSCFThread;
	bool		 m_shutdownReadSCF;
	std::unique_ptr<std::thread> m_readUIThread;
	bool		 m_shutdownReadUI;
	std::unique_ptr<std::thread> m_readThirdPartyUIThread;
	bool		 m_shutdownReadThirdPartyUI;
	std::unique_ptr<std::thread> m_readProgressiveThread;
	bool		 m_shutdownReadProgressive;
	std::unique_ptr<std::thread> m_policySvrThread;
	bool		 m_shutdownpolicySvr;
	std::unique_ptr<std::thread> m_dumpMemoryThread;
	bool m_shutdownDumpMemory;
	std::unique_ptr<std::thread>  m_updateSubgameMetersThread;
	std::unique_ptr<std::thread>  m_updateMixerThread;
	
	static void DumpMemoryThreadInner(CInternalsFramework *internals);
	static UINT AFX_CDECL DumpMemoryThread(PVOID threadParam);

    std::unique_ptr<std::thread> m_progressiveCacheThread;
	binsem m_progressiveCacheEventHandle;
	bool m_shutdownProgressiveCacheThread;

	static const DWORD PROGRESSIVE_CACHE_EVENT_INTERVAL = 100; // 100 = 0.1 seconds in tick counts.
	void ProgressiveCacheLoop();
	CProgressiveLevelForward *GetProgressiveLevelForward(bool &mulitcastActiveChanged);
	void SendProgressiveLevelForwardToGCF(const CProgressiveLevelForward &levels);

	CConfig m_config;
	CEGMConfig m_EGMConfig;

	CDataSubscriptionManager m_dataSubscriptionManager;
	ProgressiveLevelManager m_progressiveLevelManager;
	CEGMAwardManager m_egmAwardManager;

	std::recursive_mutex m_egmAwardCriticalSection; // Also used to protect m_progressiveLevelManager.

	std::recursive_mutex m_listOfCertsCriticalSection;
	vector<string> m_listOfCerts;

	CSentinelState m_sentinelState;

	std::recursive_mutex m_csReadCardReader;
	// m_mcr.SetBezelStyle may be called by any thread at any time.
	// Else CardReaderProcessingLoop should be the only function to use m_mcr.
	CMCRAutoTest * m_mcrAuto;
 	CMCRWrapper * m_mcr;
	CTcpMcrWrapper *m_tcpMcr;

	bool m_bCardInserted;
	bool m_bNvramValidAtStartup;
	bool m_hasPlayedSent;

	bool m_SCFInitialized;

	CPlayer *m_player;
	CPlayerUpdateManager m_playerUpdateManager;
	std::recursive_mutex m_playerCriticalSection;
	volatile bool m_playerGoodbyeRequested;

	CPersonalBanker* m_personalBanker;
	WORD m_pbSequence;
	byte m_pbEFTNumber;
	bool m_bIsEFT;

	// Protocol version from SCF
	// Currently, 2 = CDM, 3 = CDM + PBT Upload
	WORD m_scfProtocolVersion;

	CEmployee *m_employee1;
	CEmployee *m_employee2;
	bool m_bCurrentlyPerformingSnapshot;
	std::recursive_mutex  m_employeeCriticalSection;
	volatile bool m_employeeGoodbyeRequested;

	CTickets *m_tickets;
	std::recursive_mutex m_ticketsCriticalSection;

	CJackpot *m_jackpot;
	std::recursive_mutex m_jackpotCriticalSection;

	//Runaway Meter / Pin
	CRunawayStatus m_runaway;

	//Tilt Handling
	CTiltManager *m_tiltManager;

	bool m_bPersistentObjectsInitialized; 

	std::recursive_mutex m_eventsCriticalSection;

	bool m_firstTimeSet;

	MediaWindowPlayerStatePending m_mediaWindowPlayerStatePending;

	void BusyBoxBind();

	void Start1();
	void Start2();
	void Start3();
	void StartGCF(void);
	void BeginMeterCapture(SnapshotType type, bool setMeterCaptureInProgress, bool doAllGames);
	void BeginRebootProcess( void );

	void UninitializeMembers();
	void ShutdownThreads();
	void DeleteMembers();

	double m_par;

	DWORD m_adapterCheckTimeout;
	std::recursive_mutex m_adapterCriticalSection;

	// Indicates if there is promition media currently playing.
	bool m_promotionMediaInPlay;

	CRestartManager m_restartManager;

	CDisplayManager *m_displayManager;
	CMediaPackageManager *m_mediaPackageManager;

	// NPFP status
	bool m_pendingCoinIn;
	__int64 m_origCoinIn;

	CMeters *m_origMeters;

	bool m_updateMMT;
	bool m_bSendSubgameMetersInProgress;
	bool m_firmwareAvailable;

	// track when to signal to signal poller that we are still running
	bool m_firstPing;
	DWORD m_lastPollerPing;
	DWORD m_lastVMPing;

	bool m_IsItPollerLoadBalancingWindow;
	BYTE m_PollerLoadBalanceTime_Hour;
	BYTE m_PollerLoadBalanceTime_Minute;

	CMediaWindowConfig *m_mwConfig;
	bool m_pendingMWConfigChange;

	bool m_enableEGM;
	CBadReadCount m_badCardCounter;

	bool m_progressiveWaitingForSubGames;
	bool m_finishedFirstPassForSubGames;
	bool m_finishedOfflineCheckForSubgames;
	std::recursive_mutex m_progressiveCriticalSection;
	CProgressiveCriticalText m_progressiveCriticalText;

	// Use m_playerCriticalSection to synchronize m_playerSession access and subsequent result processing
	// in order to avoid lock ordering issues.
	CPlayerSession m_playerSession;
	CLogs *m_attnPaidLog;
	CLogs *m_repairLog;
	CLogs *m_transLog;
	CMessagePrioritizer m_msgPriority;
	int m_lastSessionID;
	DWORD m_lastFirmwareCheckTickCount;

	bool m_bannedPlayerCardInserted;

	// temp code
	std::string m_memoryLogFilename;
	DWORD m_memoryLogTickCount;

	// temp code
	std::string m_campaignMemoryLogFilename;
	DWORD m_campaignMemoryLogTickCount;
	void LaunchCampaignMemoryLogIfReqd();

	// temp code
	std::string m_campaignCpuLogFilename;
	DWORD m_campaignCpuLogTickCount;
	void LaunchCampaignCpuLogIfReqd();

public:
	// Send messages to other guys
	void SendMessageToGCF( CInternalMessage *msg, int priorityLevel );
	void SendMessageToUI( CInternalMessage *msg, int priorityLevel );
	void SetMessagePersistenceAndSendMessageToUI( CInternalMessage *msg);
	void SendMessageToSCF( CInternalMessage *msg, int priorityLevel );
	void SendTransactionMessageToSCF( CInternalMessage *msg, int priorityLevel );
	void SendMessageToProgressive(CInternalMessage *msg, int priorityLevel);

	void PerformMemoryDumps();
	void PerformMemoryThresholdChecks(CMemoryThresholds &memoryThresholds, DWORD currentTick);
	void PerformWebDiagnostics(CWebDiagnostics &webDiagnostics, CMemoryThresholds &memoryThresholds);
	void PerformUpdateWebDiagnostics(CWebDiagnostics& webDiagnostics,
		const CWebDiagnosticsCommonInfo& commonInfo,
		CMemoryThresholds &memoryThresholds,
		const time_t& currentTime,
		DWORD currentTickCount);
	void PerformProgressiveWebDiagnostics(CWebDiagnostics& webDiagnostics, const CWebDiagnosticsCommonInfo& commonInfo);
	void PerformPlayerWebDiagnostics(CWebDiagnostics& webDiagnostics,const CWebDiagnosticsCommonInfo& commonInfo);
	void PerformOptionsAndSettingsWebDiagnostics(CWebDiagnostics& webDiagnostics, const CWebDiagnosticsCommonInfo& commonInfo);
	void PerformConfigExtWebDiagnostics(CWebDiagnostics& webDiagnostics, const CWebDiagnosticsCommonInfo& commonInfo);
	void PerformQueuingConfigWebDiagnostics(CWebDiagnostics& webDiagnostics, const CWebDiagnosticsCommonInfo& commonInfo);
	void PerformCardReaderDiagnostics(CWebDiagnostics& webDiagnostics, const CWebDiagnosticsCommonInfo& commonInfo);
	void PerformJackpotInfoDiagnostics(const CWebDiagnosticsCommonInfo& commonInfo);
	void PerformIPAddressCheck();
	void SendSubgameMetersToSCF();
	const CPairingInformation& GetMobilePairingInformation();
	void RequestPairing();


	std::string FindFirmware(CVideoMixerUpgrade *&vmUpgrade, CCarrierUpgrade *&cUpgrade);
	void SetUiStateObject (CSentinelDlgState* uiState);

	void ProcessOsUpgrade();
	void ProcessDefaultDataDirectoryUpdated(const std::string& onelinkConfigMsgRefId = "", const std::string& onelinkConfigFileName = "");
	void PendingCampaignMediaDownloaded();
	void PendingSkinMediaDownloaded();
	void ProcessRestartServiceWindow();
	void SendDashboardStatusToProgressive();
	void SetCurrentActivity(CurrentActivity currentActivity);

private:
	std::string GetLcdModel() const;

	void AddRemoteDebuggingLinkToDiagnosticPage(const std::string& ipaddress);

	// Configuration functions
	void ConfigureNetworkAdapter(bool startup = false);
	void GetSubnetMask();
	void AdjustSpeakerVolume();
	void AdjustScreenBrightness();
	void SetTimezone( void );
	void GetVersion(byte &major, byte &minor, byte &build, byte &revision);
	void SendVersionToSCF();
	void SendSlotmastChangeToSCF();
	void SendSlotNumberToGCF( void );
	void SendEGMSettingsToGCF( void );
	void CheckIfEgmNeedsToBeLockedOrUnlocked(bool startup = false);
	void SendEGMEnableToGCF(bool startup, bool bEnable);
	void SendEnableCashoutToHostToGCF(bool bEnable);
	void SendAllowFloorLockoutToGCF( void );
	void InitializeLastCoinsPlayedAndWon(void);
	
	// Process messages from GCF.
	void ProcessSetMetersFromGCF( CInternalMessage *msg );
	void ProcessSetGameListFromGCF( CInternalMessage *msg );
	void ProcessSnapshotProgressFromGCF( CInternalMessage *msg );
	void ProcessSnapshotEndFromGCF( CInternalMessage *msg );
	void ProcessSetDenomFromGCF( CInternalMessage *msg );
	void ProcessGameCommErrorFromGCF( CInternalMessage *msg );
	void ProcessSasStartupQueueEmptyFromGCF(bool sasStartupQueueEmpty);
	void ProcessGameCommStatusFromGCF(CInternalMessage *msg);
	void ProcessGameCommDownFromGCF();
	void ProcessGameCommUpFromGCF();
	bool ProcessCashoutToHostFromGCF(CInternalMessage *msg);
	void ProcessGameStartFromGCF(CInternalMessage *msg);

	/// <summary>
	/// Prepare Game Session Change transaction message if Game session id is changed
	/// and MultiGameMultiDenom configured.
	/// </summary>
	/// <returns>A reference to the Game Session Change transaction message.</returns>
	CInternalMessage* PrepareGameSessionChangeTransactionMsg();

	void ProcessGameEndFromGCF( CInternalMessage *msg );
	void ProcessGeneralPollExceptionFromGCF(CInternalMessage * msg);
	void ProcessDoorOpenFromGCF( CInternalMessage * msg );
	void ProcessDoorClosedFromGCF( CInternalMessage * msg );
	void ProcessJackpotInfoFromGCF( CInternalMessage * msg );

	/// <summary>
	/// Processes the returned messages.
	/// </summary>
	/// <param name="returnMessages">The return messages.</param>
	void ProcessReturnedMessages(std::vector<MsgTargetPair*> * returnMessages);

	void ProcessMeterChangeRequestFromGCF(CInternalMessage * msg);
	void ProcessMeterChangeAllowSentFromGCF( CInternalMessage * msg );
	void ProcessMeterChangeCompleteFromGCF();
	void ProcessMeterChangeCancelFromGCF();
	void ProcessEgmVersionFromGCF( CInternalMessage *msg );
	void ProcessEgmSupportsLP7B(CInternalMessage *msg);
	void ProcessSendTicketFromGCF( CInternalMessage * msg );
	void ProcessTicketStatusFromGCF( CInternalMessage * msg );
	void ProcessTransferCompleteFromGCF( CInternalMessage * msg );
	void ProcessEFTSeedFromGCF( CInternalMessage * msg );
	void MeterSnapshotEnd( bool bCanceled = false );
	void PerformFinalOfflineStep( void );
	void SendDownloadAllowToGCF( bool bAllowDownload );
	void ProcessChangeLightOnFromGCF();
	void ProcessChangeLightOffFromGCF();
	void SetLastCoinsPlayedAndUpdateMaxCoinBetMeter(CMeter *coinInMeter, __int64 maxBet);
	long CalcLastCoinsWon();
	void ProcessStartCapturingMetersFromCGF( CInternalMessage * msg );
	void ProcessBillMetersCompleteFromGCF( void );
	void ProcessPBBalancesFromGCF( CInternalMessage *msg );
	void ProcessGCFStarted();
	void ProcessIncBillMeter(CInternalMessage *msg);
	CGameEndData *GetGameData(long lastCoinsWon, PokerHand *pokerHand, bool gameStart = false);
	void SendGameDataToProgressive(InternalMsgType messageType, long lastCoinsWon, PokerHand *pokerHand);
	void SendOfflineCoinInToProgressive(CGame* gameData, long lastCointsPlayed);
	void ProcessCurrentDenom(CInternalMessage *msg);
	void SendCurrentDenomToProgressive();

	// Process messages from SCF
	void ProcessSystemConfigFromSCF( CInternalMessage *msg );
	void ProcessSetTimeFromSCF( CInternalMessage *msg );
	void ProcessSetDisplayMessageFromSCF( CInternalMessage *msg );
	void ProcessSetGlobalPasswordFromSCF( CInternalMessage *msg );
	void ProcessSetSlotMastIDFromSCF( CInternalMessage *msg );
	void ProcessPlayerInfoFromSCF( CInternalMessage *msg );
	void ProcessPlayerGlobalBalanceFromSCF( CInternalMessage *msg );
	void ProcessPlayerBonusEventsFromSCF( CInternalMessage *msg );
	void ProcessPlayerUpdateTimeFromSCF( CInternalMessage *msg );
	void ProcessGetMetersFromSCF( CInternalMessage *msg );
	void ProcessGetSubgameMetersFromSCF(void);
	void ProcessGetStatusFromSCF(void);
	void ProcessOnlineFromSCF( CInternalMessage *msg );
	void ProcessConnectivityStatus( CInternalMessage *msg );
	void ProcessPollerPacketSentVerifyErrors( CInternalMessage *msg );
	void ProcessTilt(CInternalMessage *msg);
	void ProcessTicketStoredFromSCF( CInternalMessage *msg );
	void ProcessTicketOutPendingCountFromSCF( CInternalMessage *msg );
	void ProcessTicketResponseFromSCF( CInternalMessage *msg );
	void ProcessEmployeeResponseFromSCF( CInternalMessage *msg );
	void ProcessPBResponseFromSCF( CInternalMessage *msg );
	void ProcessResendEmptyFromSCF(  CInternalMessage *msg );
	void ProcessPersonalBankerTransactionAckedFromSCF( CInternalMessage *msg );
	void ProcessPersonalBankerAuthorizationFromSCF( CInternalMessage *msg, bool granted );
	void ProcessTransactionFillStatus(TransactionsFillStatus fillStatus);
	void ProcessSCFProtocolVersion( CInternalMessage *msg );
	void SendIPToSCFProtocol();
	void ProcessSetOneLinkProgressivesSenOptionSupport(CInternalMessage* msg);
	void SendOneLinkProgressivesSupportToProgressive();
	void ProcessMWConfigFileAvailable(CInternalMessage *msg);
	void ProcessFinishedFirstPass(CInternalMessage *msg);
	void ProcessFinishedOfflinePlayPass(CInternalMessage* msg);
	void RefreshConfigs();
	void SendUIConfigToUI();
	void ProcessSubgameSelectionResponse(CInternalMessage *msg);
	void SendSubgameSelectToPoller();
	void ProcessPBPinResetResponseFromPoller(CInternalMessage *msg);
	void ProcessSetPollerIPFromSCF(CInternalMessage *msg);
	void ProcessTaxDeductionComplete(CInternalMessage *msg);
	void ProcessCallMobileConnectPairingSettingsRequest();
	void PerformPollerLoadBalancing();
	void PerformRolloverMeterCheck();
	void UpdateCountdownDisplay();
	void ProcessCallProximitySettingsRequest();
	void ProcessCallProximityRequestBeaconScanList();

	//Bitmapped Flags
	void SetSenOptions1(CConfigSenOption1Field* senOptions1);
	void SetSenOptions2(CConfigSenOption2Field* senOptions2);
	DWORD GetTransactionStatus(bool isPlayerVirtual);

	void SetFeatureFlags(CConfigFeatureFlagField* featureFlags);

	void SendOneLinkAwardStatus(AwardType::Type awardType, EGMAwardState awardState, MethodOfPayment::Type methodPayment, int64_t jackpotId, int64_t amount, bool fromWinnerPicked, bool clearedByOneLink);
	void ProcessPrizeLockClear(int64_t jackpotId, bool clearedByOneLink);

	// Process messages from UI
	void ProcessMachineOnlineFromUI( CInternalMessage *msg );
	void ProcessMachineOfflineFromUI( void );
	void ProcessBeginMachineMoveFromUI( void );
	void ProcessCompleteMachineMoveFromUI( void );
	void ProcessCheckEmployeePin( CInternalMessage *msg );
	void SetDoorState();
	void SendServiceWindowShowToUI( bool bShow );
	void ProcessPBCheckDenom( void );
	void ProcessPBCheckPinFromUI( CInternalMessage *msg );
	void ProcessRequestPersonalBankerTransferFromUI( CInternalMessage *msg );	
	void ProcessChangeEGMTypeFromUI( CInternalMessage * msg );
	void ProcessMeterSnapshot( CInternalMessage *msg );
	void ProcessSetVolumeFromUI( CInternalMessage *msg );
	void ProcessSetBacklightFromUI( CInternalMessage *msg );
	void ProcessSendRepairCodeFromUI( CInternalMessage *msg );
	void ProcessSetVideoModeConfigFromUI( CInternalMessage *msg  );
	void ProcessAcceptVideoModeFromUI( CInternalMessage *msg  );
	void ProcessSetDesktopOrientationTypeFromUI( CInternalMessage *msg  );
	void ProcessPrinterMaintenanceFromUI();
	void ProcessAttendantPayoutFromUI( CInternalMessage *msg );
	void ProcessHopperFillFromUI( CInternalMessage *msg );
	void PerformBatteryTest();
	void ProcessSendWitnessFromUI( );
	void ProcessSendMaintenanceFromUI(  CInternalMessage *msg );
	void ProcessForceCardOutFromUI();
	void ProcessRestartMediaWindowFromUI(CInternalMessage *msg);
	void ProcessBrowserTerminationFromUI();
	void ProcessSetJackpotResetSettingFromUI( CInternalMessage *msg );
	void ProcessSetTouchFromUI(CInternalMessage *msg);
	void ProcessPreviewSWSizeFromUI();
	void ProcessPreviewTaskbarSizeFromUI();
	void ProcessPreviewSWLocationFromUI();
	void ProcessSetServiceWindowFromUI( CInternalMessage *msg );
	void ProcessSetTaskbarFromUI( CInternalMessage *msg );
	void ProcessUiSubscribeRequest(CInternalMessage *msg);
	void ProcessUiUnsubscribeRequest(CInternalMessage *msg);
	void ProcessMobileConnectPressed();
	void SaveServiceWindowOpen(bool mediaWindowOpen);
	void ProcessHideServiceWindow();
	void ProcessSetConnection();
	void ProcessSetPollerSettings(CInternalMessage *msg);
	void ProcessSetDhcpSettings(CInternalMessage *msg);
	void ProcessSetSentinelIpAddress(CInternalMessage *msg);
	void ProcessSetSubnetMask(CInternalMessage *msg);
	void ProcessSetDefaultGateway(CInternalMessage *msg);
	void ProcessSetPreferredDnsServer(CInternalMessage *msg);
	void ProcessSetAlternateDnsServer(CInternalMessage *msg);	
	void ProcessClearPersistentMemory(CInternalMessage *msg);
	void SetFirmwareVersion( );
	void ProcessUpgradeMixer();
	void ProcessUpgradeCarrier();
	void ProcessGetEgmDisplaySetting(CInternalMessage *msg);
	void ProcessUpdateEgmDisplaySettings( CInternalMessage *msg );
	void ProcessUpdatePlayerStateCloseSettings( CInternalMessage *msg );
	void ProcessUpdatePlayerStateOpenSettings(CInternalMessage *msg);
	void ProcessUpdateAutoCloseSetting( CInternalMessage *msg );
	void ProcessEmployeeGoodbye();
	void ProcessPlayerGoodbye();
	void ProcessButtonActivity(CInternalMessage *msg);
	void ProcessSetNewPinFromUI(CInternalMessage *msg);
	void ProcessFlushPlayerUpdate(CInternalMessage *msg);
	void ProcessPrizeLockClearFromUI();

	// Process messages from Progressive
	void ProcessGetProgressiveGamesInfo(CInternalMessage* msg);
	void ProcessRemoteReboot(CInternalMessage *msg);
	void ProcessPromoGameRequest(CInternalMessage *msg);
	void CheckProgressiveWaitingForSubGames();
	void ProcessSetProgressiveConfig(CInternalMessage* msg);
	void ProcessProgressiveAnnouncement(CInternalMessage* msg);
	void ProcessProgressiveClaimableAward(CInternalMessage *msg);
	void ProcessProgressiveWinnerPick(CInternalMessage *msg);
	void ProcessProgressiveWinnerPicked(CInternalMessage *msg);
	void ProcessSetProgressiveConnectionInfo(CInternalMessage* msg);
	void ProcessSetProgressiveHost(CInternalMessage* msg);
	void ProcessEgmAwardMessage(CInternalMessage* msg, MessageQueueId source);
	bool ProcessEgmAwardStatus(CInternalMessage* msg);
	void ProcessProgressiveConfiguration(CInternalMessage* msg);
	void ProcessProgressiveBonusingAvailable(CInternalMessage *msg);
	void ProcessProgressiveLevelBroadcast(CInternalMessage* msg);
	void ProcessCumulativeProgressiveWin(CInternalMessage* msg, MessageQueueId source);
	void ProcessFirmwarePackageAvailable(CInternalMessage* msg);
	void ProcessConfigFileAvailable(CInternalMessage* msg);
	void ProcessCampaignAvailable(CInternalMessage* msg);
	void ProcessCampaignEventInfo(CampaignEvent &campEvent) const;
	void ProcessSkinAvailable(CInternalMessage* msg);
	void ProcessScheduleEnd(CInternalMessage* msg);
	void PerformProgressiveTimedEvents();
	void ProcessUpdateSasInfo(CInternalMessage* msg);
	void ProcessBonusCapabilities(CInternalMessage *msg);
	void ProcessSasStartupQueueEmptyUpdate();
	void ProcessSystemChangeForProgressive();
	void SendProgressiveCriticalTextToUI();
	void ProcessEgmStateRequest(CInternalMessage* msg);
	void ProcessSetProgressiveDown(CInternalMessage* msg);
	void SendPayoutToOneLink(CJackpot* jackpot);
	void SendPayoutToOneLink(CEGMAward* award);

	// Process messages from MobileConnect
	void ProcessPairingSettingsEventData(CInternalMessage *msg);
	void ProcessPairingEventData(CInternalMessage *msg);
	void ProcessCardChangeRequestData(CInternalMessage *msg);
	void ProcessCreditTotalsRequestData(CInternalMessage *msg);
	void ProcessFundsTransferRequestData(CInternalMessage *msg);
	void HandleCardChangeRequest(const CCardChangeRequest *pCardChangeRequest, CCard *card, bool& bStatus, std::string& statusMsg);
	void ProcessProximitySettingsEventData(CInternalMessage *msg);

	// Card functions.
	CardType DetermineCardType( CCard *card );
	void SendEmployeeCardMessageToSCF( bool bCardIn );
	void SendCardSessionInProgressToUI(bool bPBTransferInProgress);
	void SendSystemBusyToUI(bool enable);
	void SendDisplayAttractorToUI(bool uncarded = true);
	void SendDisplayPlayerRemovedToUI();
	void SendBadCardReadToUI( void );
	void SendBadCardReadToSCF( void );
	void PerformCardTimedEvents( void );
	
	void TellGCFToGetBalances();
	void PerformPlayerCardTimedEvents(InternalMsgType msgType);
	void PerformVirtualBadCardReadExpired(bool sentDisplayAttractorToUI);
	void WaitForCardEvent(bool &bInserted, bool &bRemoved, bool &bTimeout, bool &bIsCardInserted, DWORD cardRemovedTicks, CCard *&card);
	void NoCardActivity(bool &bRemoved, bool &bTimeout);
	CCard *AutoCardActivity(bool &bInserted, bool &bRemoved, bool &bTimeout);
	void CheckIfBackgroundSnapshotIsInProgress();
	void HandleCardRemoved(DWORD cardInTicks, bool postponePlayerRecard);
	bool HandleCardInserted(CCard *card, bool bIsCardInserted);
	void ShowRFIDCardRemoved(bool bSendAttractorToUI);
	void CardRemoved(DWORD cardInTicks, bool postponePlayerRecard);
	bool IsPlayerGoodbyeRequested();
	void SetPlayerGoodbyeRequested(bool val);
	
	// Player functions
	void InitializePlayer( void );
#ifdef _DEBUG
	void UninitializePlayer( void );
#endif // _DEBUG.
	bool PlayerCardInserted(CCard *card, bool goodbyeEnabled);
	bool PlayerCardRemoved( time_t &removalTime, time_t &utcRemovalTime, bool& bannedPlayerCardIsRemoved);
	bool IsPlayerVirtual( void );
	bool IsPlayerVirtualFromEmployeeCardIn(void);
	bool DoesPlayerExist( void );
	void ForcePlayerCardOut( bool bannedPlayerIsDisplayed = false );
	bool PlayerTimedEvents(InternalMsgType msgType);
	void PerformPlayerSessionTimedEvents();
	bool CheckIfVirtualSessionNeedsToBeRemoved(time_t currentTime, time_t utcTime);
	bool CheckIfActiveSessionNeedsToBeRemoved(InternalMsgType msgType, time_t currentTime, time_t utcTime);
	void CheckIfAutoOpenCloseInServiceWindowIsNeeded();
	void CheckIfMediaWindowConfigNeedsToBeSent();
	void SetPlayerLastPlay( bool hasPlayed = true );
	void ProcessPlayerInfo( CPlayerInfo *playerInfo );
	void ProcessPlayerGlobalBalance( CPlayerGlobalBalance *playerInfo );
	// overloaded version used for no points for promo calculations
	void ApplyMeterChangeToPlayer( CMeters *meters, CMeters *oldMeters );
	void SetPlayerConfig( void );
	void SetPlayerBonusEvents( CBonusEvents *bonusEvents );
	void ProcessPersonalBankerUIActivity(void);
	void ResetPersonalBankerInactivityTimer( void );
	void ProcessGetAccountBalance(  );
	void ProcessRequestPlayerResume(PersonalBankerAction personalBankerAction);
	bool EndActivePlayerSession(time_t const removalTime, time_t const utcRemovalTime, bool bCanMakeVirtual, bool bannedPlayerIsDisplayed = false);
	void EndVirtualPlayerSession( time_t const sessionEndTime, bool bSamePlayer = false ); // bSamePlayer = true causes no automatic NCEP out
	void EndMiniVirtualSession();
	void StartAutoUpload();
	CPlayerInfo *MakeDefaultPlayerInfo( void );
	void SendCardlessSessionTransactionsToSCF(bool sessionStart, DWORD psid = (DWORD)-1, DWORD ptsn = (DWORD)-1);
	void SendPlayerTransactionToSCF( PlayerTransactionType transactionType, CMeters *transactionMeters );
	void SendOutstandingBonusTransactionsToSCF( bool bCardOutTime, CMeters *transactionMeters );
	void SendCashTransferTransactionsIfNecessary( CMeters *transactionMeters );
	void SendPersonalBankerMessageToUI( CInternalMessage *msg, bool lowerPriority = false, int priority = 0);
	bool IsPlayerBannedFromPlay();
	void SendMobileConnectCommunicationSupport();

	// PersonalBanker functions.
	void BeginPersonalBankerAutoSession(bool sendToPoller = true);
	void BeginPersonalBankerSession(CInternalMessage *msg);
	void BeginPersonalBankerTransaction(PersonalBankerTransferType type, DWORD amountInDollars);
	void BeginPersonalBankerTransfer( PersonalBankerTransferType type, DWORD amount );
	void EndPersonalBankerActivePlayerSession();
	bool GetPbtInProgress();
	void PerformPersonalBankerActions();
	void PerformPersonalBankerTimedEvents();
	void ProcessCancelPersonalBankerSession(CInternalMessage *msg);
	void ProcessRestorePersonalBankerSession();
	void ProcessPersonalBankerAuthorization(const std::string& transactionID, bool granted );
	void ProcessPersonalBankerInfo( CPersonalBankerInfo *pbInfo );
	void ProcessPersonalBankerTransactionAcked( WORD sequenceNumber );
	void ProcessPersonalBankerTransactionStored( void );
	void ProcessPersonalBankerTransferComplete( CPersonalBankerTransfer *pbTransfer );
	void ReleasePersonalBankerSession( bool canceledFromUI = false );
	void RequestPersonalBankerStatus( void );
	void ProcessUJPCheckPinFromUI(CInternalMessage *msg);
	void ProcessProbePBTFromUI(bool isTransferToGame);
	void ProcessUpdateUIConfigFromUI(CInternalMessage* msg);
	bool ProcessProbePBTFromGCF(CInternalMessage *msg);
	bool IsPBTEnabled( void );
	void SetPlayerCardIDToUJPTransaction(CInternalMessage *pollerMsg);
	
	/// <summary>
	/// Prepare and send a PersonalBanker transfer.
	/// </summary>
	/// <param name="transactionTime">IN/OUT - transaction time.</param>
	/// <param name="deleteTransferObjectAfterCopying">IN - true if Transfer is to be deleted after sending it, false otherwise</param>
	void SendPersonalBankerTransaction(time_t *transactionTime = NULL);

	void SendPersonalBankerTransfer(CPersonalBankerTransfer *pbTransfer, time_t *transactionTime = NULL);
	void SendPlayerDataToUI( InternalMsgType aMsgType, bool displayCongrats = true );
	void DistributePlayerData(bool sendToProgressiveProtocol = true, bool attachPlayer = false, bool samePlayer = false);
	void DistributePlayerDataToProgressiveProtocol();
	void SendPlayerDataToProgressiveProtocol(bool sessionActive, bool cardedPlayer, int sessionId);
	void StartPersonalBankerTransfer();

	// Mobile Connect messages
	void SendMCPlayerDataToPollerProtocol(bool cardedPlayer, int sessionId);
	void SendMCPlayerSessionInfoToPollerProtocol(PlayerSessionChangeEventType evtType = SESSION_CHANGE_EVENT_GAMEPLAY, WORD cashoutErrorFlags = 0, std::string sasErrorText = "");
	void SendMCPairingStatusToPollerProtocol(bool isSuccess, std::string status);
	void SendMCFundsTransferBalanceToPollerProtocol();
	void SendMCFundsTransferToPollerProtocol( bool success, std::string status, bool sendErrorToUI = false, DWORD cashable = 0, DWORD cashablePromo = 0, DWORD nonCashablePromo = 0);

	// Proximity messages
	void ProcessProximityBeaconScanListResponse(CInternalMessage *msg);
	void SendProximityStatusToPollerProtocol(bool isSuccess, std::string status);

	// PersonalBanker perform action functions, same order as enumerations.
	void PerformPBActionSendCardSessionInProgressToUI();
	void PerformPBActionStartTransfer();
	void PerformPBActionSendPersonalBankerNcepOutFail();
	void PerformPBActionSendDisplayPBAnimationToUI(PersonalBankerAction personalBankerAction);
	void PerformPBActionSendPersonalBankerInfoToUI(CPersonalBankerInfo *pbInfo);
	void PerformPBActionSendAutoTransferCancelledToUI();
	void PerformPBActionSendCancelCashoutToHostToEGM();
	void PerformPBActionSendCompleteToUI(PersonalBankerAction action);
	void PerformPBActionSendAutoUploadCancelledMessage();
	void PerformPBActionProcessAuthorizationCanceled();
	void HandlePBTProbeMobile();
	bool IsPBTProbeRequiredMobile(CPersonalBankerInfo *pbInfo);

	// Employee functions
	void UninitializeEmployee( void );
	bool EmployeeCardInserted( CCard *card, bool goodbyeEnabled );
	bool EmployeeCardRemoved( time_t removalTime );
	bool IsEmployeeVirtual( void );
	bool DoesEmployeeExist( void );
	void ValidateEnteredEmployeePin( const std::string &pin );
	void ProcessEmployeePin( byte *cardID, int cardIdLen, const std::string &pin );
	void EmployeeTimedEvents( void );
	void ProcessEmployeeButtonActivity();
	void ValidateEmployeePin( CEmployee *employee );
	bool EndActiveEmployeeSession( const time_t removalTime, bool bCanMakeVirtual );
	bool RecardPlayer(bool employeeCardRemoved);
	// UI_REWRITE_CLEANUP: Validate if we ar missing any use-case of employee screen launch this way, or we want to keep this around for media window use case reference.
	void SendEmployeeDisplayStart(bool showVideoModeRevert, bool pinRequestOverride, bool startCalibration=false);
	void SendEmployeeCardInStatusToUI();
	

	// Tickets functions
	void InitializeTickets( void );
	void UninitializeTickets( void );
	void SetTicketsConfig( void );
	void SendTicketingInfoToGCF( void );
	void ProcessTicketFromGame( CTicket &ticket );
	void ProcessTicketInserted( void );
	void ProcessTicketStored( CTicket &ticket );
	void ProcessTicketResponse( CTicket &ticket, DWORD scfTicketID );
	void ProcessTicketStatus( CTicket *ticket, TicketInStatus status );
	void SetTicketOutPendingCount( WORD pendingCount );
	void TicketTimedEvents( void );
	void RequestTicketStatusFromGame( void );
	void SendTicketInFinalToSCF( CTicket *ticket );
	void SendTicketStoredToGCF( CTicket &ticket );

	// NonIsolated Digital Input Functions
	void ProcessDoorOpen( WORD oasisMeter, bool sendTransaction = true );
	void ProcessDoorClosed( WORD oasisMeter, bool sendTransaction = true);

	// Jackpot functions.
	void InitializeJackpot( void );
	void UninitializeJackpot( void );
	void SendJackpotTransactionToSCF(CJackpot *jackpotToSend);
	void JackpotHit(CInternalMessage *msg);
	void ProcessProgressiveWinHit(CInternalMessage *msg);
	void ProcessHandPayReset(CInternalMessage *msg);
	void ProcessJackpot( CJackpot *jackpot );
	void ProcessJackpotTransactionStored( void );
	void ProcessUJPAuthorizePin(CInternalMessage *msg);
	void ProcessUJP(CInternalMessage *msg);
	void ProcessDisplayUJP(UnattendedJackpotState::Type state, bool sendUjpStatusToPoller = true);
	void ProcessJackpotResetResult(CInternalMessage *msg);
	void PerformUnattendedJackpotCheck();
	void SendUJPToPoller(CJackpot *jackpot);
	void ProcessUJPCallHost(bool closeMediaWindow);
	void ProcessUJPPinpadTimeout();

	void ProcessBillAccepted(WORD billAmount, __int64 acceptedBillCount, __int64 totalBillCount, CGame* game, CMeter* meterBillCredits);
	void ProcessVoucherOut(DWORD amountInPennies);

	// Helper Functions
	void ProcessSetMeters( CMeters *meters, WORD gameNumber, bool isSnapshot, bool sendMeterUpdate304);
	void ProcessSetCreditMeter(const CMeter &meter);
	void TellDisplayManagerCreditMeterNonZero();
	void CheckMoneyInServiceWindow();
	void SetCurrentGame( WORD newCurrentGameNumber );
	void SendGameMetersToSCF( CGame *game );
	void SendTicketOutPendingRequestToSCF( void );
	CMeters * GetTransactionMeters( MEMORY_TYPE memoryType = MEMORY_NONE, DWORD memoryOffset = 0 );
	CInternalMessage *PrepareTransactionMessage( InternalMsgType msgType, bool isPlayerVirtual, time_t *transactionTime = NULL, CMeters *meters = NULL );
	CInternalMessage * GetStackerOutMessage();
	CInternalMessage * GetStackerInMessage();
	CInternalMessage * GetTiltMessage(TiltCode tilt);
	bool IsDoorOpen( int doorMeterNumber );
	void SendSnapshotTransactionToSCF( WORD gameNumber );
	void SendSentinelChangeTransactionToSCF( void );
	void SendConfigChangeTransactionToSCF( ConfigChangeType type, __int64 previousValue, __int64 currentValue, byte pinNumber = 0 );
	BatteryStatus GetBatteryStatus( void );
	void TellUIMediaWindowPlayerState(MediaWindowPlayerState playerState);
	bool CalculateNcepMeters(CMeters *meters, CGame *game, CMeters *origMeters);
	void SetWebLogString( CInternalMessage *msg );
	void CheckIfUJPNeedsToBeCancelled(int buttonId);
	TransactionType DetermineJackpotTransType(CJackpot *jackpot);
	void GenerateTilt(TiltCode code);
	void HandleMobilePairingTimeOut();
	void HandleMobileRequestCardChangeTimeOut();
	void CheckBeaconConnectionStatus();
	bool IsNFDPoller();
	__int64 CalculateNonNCEPPlayed(__int64 coinIn, __int64 NCEPPLayed);

	//Adapter helper functions
	void PerformAdapterTimedEvents();

	//GeneralTimedEvents
	void PerformControlledUIRestartCheck();
	void PerformControlledUIRefreshCheck();
	void PerformExternalFilesCheck();
	void PerformEGMDisplayCheck();
	void PerformVideoModeRevertCheck();
	void PerformCalibrationCompleteCheck();

	void PopulateConnection(CConnection &connection);
	CInternalMessage * CreateConnectionDataMessage();

	void SendSubscriptionUpdateIfNecessary();
	void SendMGMDupdateIfNecessary();

	//Calibration Card helper functions
	void StartCalibration();

	void StartSubgameMetersUpdateThread();
    void CheckForMetersSend();

	//Game List Chunks information
	void ProcessGameListChunksSentInformation(CInternalMessage *msg);

	CNotificationBox m_notificationBox;
	CExcessiveVoucherOut m_excessiveVoucherOut;

	bool m_MGMDAvailable, m_MGMDGlobalOption;

	bool m_sendPlayerChangeEvent;// todo: this will go with resend list iimplementation.
	CPairingInformation& m_pairingInformation;
	CBeaconManager *m_beaconManager;
	CMobilePBTSessionInfo &m_mobilePBTInfo;
	MACHINE_STATE m_machineState;
	DWORD m_lastCashoutToHostSessionTickCount;
	bool m_currentSubgameMetersChanged;
	bool m_maintenanceMode;
	DWORD m_lastMaintenanceModeTickCount;
	DWORD m_countdownDisplayTickCount;  // for extended buckets countdown display
	string m_sentinelIp;
};
