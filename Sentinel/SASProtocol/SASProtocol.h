#pragma once
#include "../SystemProtocol.h"
#include "SASPollMessage.h"
#include "SASSerialWrapper.h"
#include "SASScheduler.h"
#include "MeterStepdown.h"
#include "../metersdef.h"
#include "../Meters.h"
#include "SASBills.h"
#include "SASGameList.h"
#include "../TiltsDef.h"
#include "../PersonalBankerTransfer.h"
#include "LongPoll72Message.h"
#include "LongPoll74Message.h"
#include "LongPoll8EMessage.h"
#include "TicketSeed.h"
#include "SASGeneralPoll.h"
#include "SASInterface.h"
#include "SASConsumer.h"
#include "SASSerialWrapper.h"
#include "Jackpot.h"
//#include "ProgressiveProtocol\EGMAward.h"
#include "SASProtocolState.h"
#include "binsem.h"

union u_SetMeterFlags
{
	// first representation (member of union) 
	struct tSetMeterFlags
	{
		WORD meterSnapshop		: 1;//Bit1
		WORD sendMeterUpdate304 : 1;//Bit2
		WORD m_spare			: 14;//Bits 3-16
	} flags;

	// second representation (member of union) 
	WORD uValue;

	u_SetMeterFlags()
	{
		uValue = 0;
	}
};

const int LOCK_DURATION = 1000; // hundredths of seconds
const int PRIZE_LOCK_DURATION = 9999; // 99.99 seconds
const int PRIZE_LEGACY_LOCK_DURATION = 1000; // 100 seconds
const byte SAS_TRANSFER_TYPE_CASH_TO_GAME = 0;
const byte SAS_TRANSFER_TYPE_COIN_OUT_BONUS = 0x10;
const byte SAS_TRANSFER_TYPE_JACKPOT_BONUS = 0x11;	// This forces lockup
const byte SAS_TRANSFER_TYPE_CASH_TO_HOST = 0x80;
const int PBTLOCKSLEEP = 200; // 200ms sleep before transfer

// Declare friend class(es) for unit test access.
namespace SentinelUnitTests { class SASProtocol; }
namespace SentinelUnitTests { class SASProtocol2; }

class CSASProtocol : public CSystemProtocol, CSASConsumer
{
	// Grant "friend" access for testing.
	friend class SentinelUnitTests::SASProtocol;
	friend class SentinelUnitTests::SASProtocol2;

public:
// #pragma region constructors
	/// <summary>
	/// Initializes a new instance of the <see cref="CSASProtocol"/> class.
	/// </summary>
	/// <param name="gcfToInternalsQueue">The GCF to internals queue.</param>
	/// <param name="operatingSystem">IN/OUT - The operating system.</param>
	CSASProtocol(CQueueList &gcfToInternalsQueue,
		IOperatingSystem &operatingSystem = COperatingSystem::m_operatingSystem);
// #pragma endregion constructors

// #pragma region destructor
	/// <summary>
	/// Destructor.
	/// </summary>
	virtual ~CSASProtocol(void);
// #pragma endregion destructor

	/// <summary>
	/// Initializer.  Intended to be called immediately after the constructor.
	/// </summary>
	virtual void Init();

	/// <summary>
	/// Establishes communications with the EGM.
	/// </summary>
	/// <remarks>
	/// The caller is responsible for deleting the returned instance.
	/// Does not return until communications are established with the EGM or we are shutting down.
	/// </remarks>
	void EstablishEGMCommunications();

	void ProcessMessageFromInternals( CInternalMessage *msg );
	void SendMessageToSAS( CSASPollMessage *msg );
	void PerformScheduledTask( SAS_SCHEDULED_TASK taskID );

	/// <summary>
	/// Gets the last poll value.
	/// </summary>
	/// <returns>The last poll value.</returns>
	BYTE GetLastPoll( void ) const;

	/// <summary>
	/// Sets the last poll value.
	/// </summary>
	/// <param name="pollValue">IN - The last poll value.</param>
	void SetLastPoll( BYTE pollValue );

	/// <summary>
	/// Determines whether game communication has started.
	/// </summary>
	/// <returns>True if game communications have started</returns>
	bool IsInitialized(void);

	CQueueList * GetSASPollQueue(void)
	{
		return m_sasPollQueue;
	}

	void SendToEGMThreadClosed( void );
	void ScheduledSASTaskThreadClosed( void );

	bool AreWeShuttingDownSendToEGMThread( void )
	{
		return m_bShutdownSendToEGMThread;
	}

	virtual void ResetStepdowns();

protected:
	CSASInterface *m_sas;

	/// <summary>
	/// Get a new SAS interface instance (to the EGM).
	/// </summary>
	/// <param name="sasProtocol">IN/OUT - The SAS consumer.</param>
	/// <param name="gcfToInternalsQueue">IN/OUT - The GCF to nternals queue.</param>
	/// <param name="internalsToGCFQueue">IN/OUT - The internals to GCF queue.</param>
	/// <param name="operatingSystem">IN/OUT - An interface to operating system functionality.</param>
	/// <remarks>
	/// May be overridden for testing.
	/// </remarks>
	virtual CSASInterface *GetSASInterface(CSASConsumer *sasProtocol,
		CQueueList &gcfToInternalsQueue,
		CQueueList &internalsToGCFQueue,
		IOperatingSystem &operatingSystem);

private:
// #pragma region private constants
	static const short DEFAULT_POLLING_SPEED_MILLISECONDS = 40;
	static const int GAME_ID_LENGTH = 2;
	static LPCTSTR const ATIBONUS_TRANSACTION_ID_PREFIX;
	static LPCTSTR const ATIGENERIC_TRANSACTION_ID_PREFIX;
	static const DWORD SHUTDOWN_SCHEDULER_SLEEP_PERIOD = 100; // In milliseconds.

	// SAS version constants.
	static const byte SASVERSION3 = '3';
	static const byte SASVERSION5 = '5';
	static const byte m_gameAddress = 1;

	// Number of times to retry high priority meters when any meters involved in No Point For Promo calculations
	// have changed on the last two tries. Does not include the initial two tries.
	// Also see bNPFPMetersChanged in CInternalsFramework::ProcessSetMeters(CMeters *, ...).
	static const short HIGH_PRIORITY_CHANGED_METER_RETRIES = 3;
// #pragma endregion private constants


	int m_SASSendLoopCount;
	int m_SASGetMetersCount;

	bool m_bInitialized;
	virtual void Start(void);
	void Shutdown( void );

	void ProcessSetSlotNumber( CInternalMessage *msg);
	void TryToSendMessageToSAS( CSASPollMessage *msg, byte retryCount = 0 );
	void PopulateMeterStepdowns( void );
	void GetSASVersion();
	bool FindEGM();
	void GetEGMCapabilities();
	void GetAFTAndBonusCapabilities(CGames *sasInfo);
	void GetGameDenomInfo();
	void SendSubGameMeters();
	void SendSubGameMeters(WORD gameNumber, bool sendInfo, bool bMeterSnapshot = false);
	bool  GetSelectedGameNumber(int &gameNumber);
	void SendTiltToInternals( TiltCode tiltCode );
	void SendDoorOpenToInternals(oasisMeters meterNumber);
	void SendDoorClosedToInternals(oasisMeters meterNumber);
	void GetTicketValidationData( bool bAlwaysUpdate );
	void SetGameDelayAndUpdateCurrentGameDenom();
	void SetGameDelay( void );
	void UpdateCurrentGame(bool gameSelected);
	void UpdateCurrentDenom(bool gameSelected);
	void CheckCurrentGameDenom(bool gameStarted);
	void DisableRealTimeEvents( void );
	void EnableDisableEGM();
	void EnableDisableCashoutToHost();
	void SetGameTime( void );
	void SetTicketValidationParms( void );
	void GetHighPriorityMeters(void);
	CMeters *GetNewHighPriorityMetersList();
	CMeters *GetHighPriorityMetersAll();
	void GetHighPriorityMetersTickets(CMeters *meters);
	void GetHighPriorityMetersPbt(CMeters *meters);
	void SendHighPriorityMetersResults(CMeters *meters);
	void GetMediumPriorityMeters(void);
	void GetBillMeters(int billMeterNumber = mNone);
	void ProcessMeterSnapshot( void );
	bool GetCurrentDenom(byte &denomCode);
	WORD GetMeters( CMeters &meters, WORD gameNumber, bool bReportProgress = false, bool tryOnce = false );
	void SendSubGameInfo(WORD gameNumber, bool resetToLastPreviousValuesIfPollsFail);
	bool SendSubGameInfo(WORD gameNumber);

	//Get Game Name, PaytableID, max bet and progressive goup
	bool GetInfoFromLongPollB5(CGame &game); 
	
	void GetTicketMeters(bool sendMeterUpdate304);
	void GetPBTMeters( void );
	bool CheckForBillMeterPolls();
	void GetBillMeter(int queuePriority = QUEUE_PRIORITY_NORMAL);

	void GetGameList(bool gpeOptionsChanged, bool scheduledTask);
	void GetCumulativeProgressiveWin();
	void SendCumulativeProgressiveWinToInternals(__int64 amount);
	bool GetMaxBetAndProgressiveGroup(WORD gameNumber, CGame &game);

	void SetGame(int gameNumber, byte enable);

	// Process messages from Internals
	void ProcessEnableGame( CInternalMessage *msg );
	void ProcessEnableCashoutToHost( CInternalMessage *msg );
	void ProcessGetGameInfoFromInternals( CInternalMessage *msg );
	void ProcessSetSlotMastIDFromInternals( CInternalMessage *msg );
	void ProcessEGMSettingsFromInternals( CInternalMessage *msg );
	void ProcessMeterSnapshot( CInternalMessage *msg );
	void ProcessRolloverMeterSnapshot();
	void DoMeterSnapshot();
	void ProcessMeterChangeAllow( CInternalMessage *msg );
	void ProcessEGMAward(CInternalMessage *msg);
	void ProcessProgressiveData(CInternalMessage *msg);

	// Send to OneLink
	void SendPollExceptionToInternals(GeneralPollException exceptionValue);

	// General Poll functions
	void DoGeneralPoll(void);
	void DoGeneralPollTimers(DWORD currentTime);
	void ProcessGeneralPollException( GeneralPollException exceptionValue );
	void ProcessGeneralPollBufferOverflow(void);
	void ProcessGeneralPollGameStarted(void);
	CMeters *GetGameEndedMeters();
	void ProcessGameEndedAccountingOnlyLink(const CMeters &meters);
	void ProcessGeneralPollGameEnded(void);
	void ProcessGeneralPollGameSelected(void);
	void SendCurrentSelectedGameToInternals(bool gameSelected);
	
	/// <summary>
	/// Sends the message to Internals on game change.
	/// </summary>
	void SendGameSessionIdIfRequiredToInternals();
	void ProcessInitTicketingRequest(void);
	void GetPrintedTicketInfo(bool sendMeterUpdate304);
	void ProcessHandpayPending( void );
	void ProcessFinalTicketStatus();
	void ProcessGeneralPollGameLocked();
	void ProcessPersonalBankerGameLocked();
	void ProcessAFTBonusGameLocked();
	void ProcessGeneralPollProgressiveJackpot();
	void ProcessGeneralPollHandpayReset();
	void ProcessGeneralPollProgressiveMachinePay();
	void ProcessGeneralPollHandpayPending();
	void ProcessGeneralPollCashoutToHost();
	void ProcessProgressiveJackpot();
	void ProcessProgressiveMachinePay();
	void ProcessMeterChangePending();
	void ProcessMeterChangeCancelled();
	void ProcessEnabledGamesDenomChanged();
	void ProcessChangeLightOn();
	void ProcessChangeLightOff();
	void ExecuteAFTBonusAward(DWORD assetNumber);
	static std::string GetTransactionId(LONG64 jackpotId);
	PokerHand *GetPokerHand(CLongPoll8EMessage::PokerHandType pokerhandType);

	// Ticketing Functions
	void ProcessSetTicketingInfo(CInternalMessage *msg);
	void ProcessTicketStored();
	void ClearTicketOut();
	void ProcessTicketInsertedGP(void);
	void ProcessTicketIn();
	void ProcessTicketResponse(CInternalMessage *msg);

	void LossLimitBillMeters();

	// Personal Banker Functions
	void ProcessPBTransfer(CInternalMessage *msg);
	void ExecutePBTransfer();
	void CancelPBTransfer(PollerCancellationType cancelType);
	void UpdatePBTWithTransferStatus(byte transferStatus);
	void ProcessGeneralPollXferComplete();
	void CancelPBLock();
	void GetCurrentCreditMeter(CPersonalBankerTransfer *pbt);
	void ProcessPBStatus(CInternalMessage *msg);
	void CancelSASPBTransfer();
	void CancelEFT(PollerCancellationType type);
	void GetFinalCreditMeter();
	void ResetJackpot(CInternalMessage *msg);
	CJackpot::ResetResult ResetToCredit(bool credit = true);
	CJackpot::ResetResult RemoteReset();
	void GetPBBalances();
	void ResendPBTLockRequest();
	void ResendAwardLockRequest();
	void ReturnAwardIneligible();
	void CheckDenomForPB(bool &denomGood, bool &gameComGood);
	void PerformEFT();
	CSASPollMessage *SetupEFTPoll( bool &isEftSupported, byte &direction);
	void HandleEFTErrorStatus(byte status);
	void LockGameForAFTTransfer();
	CLongPoll72Message *SetupAFT72Poll( bool &proceedWithTransfer );
	void SetupAFT72PollPersonalBankerType( CLongPoll72Message *lp72, CLongPoll74Message *lp74, bool &proceedWithCashTransfer );
	void SetupAFT72PollPromoPersonalBankerType( CLongPoll72Message *lp72, CLongPoll74Message *lp74, bool &proceedWithPromoTransfer  );
	void Setup72AFTPollOtherInfo( CLongPoll72Message *lp72, CLongPoll74Message *lp74 );
	EGMAwardState GetAwardResultFromTransferStatus(byte transferStatus, LPCTSTR &statusInfo);
	void ProcessPBTransferComplete(CLongPoll72Message *lp72);
	void SetDenomMult(WORD denomMult);
	void ProbePBT(bool isTransferToGame);
	void CancelCashoutToHost();
	void ProcessProbePbtGameLocked();
	void ResetPBTProbe();

	// Awarding Functions
	void AwardBonus();
	void AwardLegacyBonus();
	void AwardAFTBonus();
	bool CheckAwardDenom();
	void GetOriginalBonusingMeters();
	void ProcessAFTBonusXferComplete(CLongPoll72Message *lp72);

	void LockForEGMPrizeAward();
	void LockClearForEGMPrizeAward(CInternalMessage* msg);
	void AFTLockForEGMPrizeAward();
	void AFTLockClearForEGMPrizeAward();
	void LegacyLockForEGMPrizeAward();
	void LegacyLockClearForEGMPrizeAward();
	void ReportPrizeAwardStatus(EGMAwardState status);
	void CheckAFTLockForPrizeAward();
	void CheckIfPendingAutoUploadNeedsBeCancelled(GeneralPollException exceptionValue);

	CMeterStepdown m_mainGameMeterStepdown;
	CMeterStepdown m_subGameMeterStepdown;

	bool m_accountingOnlyLinks;

	// The external queue for messages going to SAS
	CSASQueueList *m_sasPollQueue;

	BYTE m_lastPollValue;

	CSASGameList m_gameList;
	char m_gameID[GAME_ID_LENGTH + 1]; // Make room for trailing NUL character.

	bool m_bInSelfTest;
	bool m_bInUserMenu;

	bool m_bIsAFTSupported;
	bool m_bIsEFTSupported;
	EGMTransferMode m_egmTransferMode;
	bool m_resetToCredit;

	// Tracker #22679
	bool m_PBTEnabled;
	bool m_bEFTCashToGame;
	bool m_bEFTCashFromGame;
	std::string m_casinoName;
	std::string m_address1;
	std::string m_address2;
	WORD m_slotNumber; // Use lower 2 bytes of Oasis "Slot Number" for SAS "Host ID" for ticketing.  2009-10-01 Alan Sheldon
	WORD m_ticketSequenceNumber;
	bool m_couponPromoEnabled;
	bool m_bVoucherPromoInEnabled;

	DWORD m_pbtStartTickCount;
	WORD m_denomMult;
	binsem m_meterMutex;
	// Tracker 23188
	bool m_bGameDelaySent;
	bool m_billMeterSuccess;
	CSASBills m_sasBills;
	bool m_queryPokerhand;
	bool m_startSnapshot;
	bool m_snapshotAllGames;

	DWORD m_assetNumber;

	// Will contain complete list of games with all denoms
	CGames *m_denomGames;

	CPersonalBankerTransfer *m_pbt; // this is the currently in process pbt operation;
	PersonalBankerState m_state;
	std::recursive_mutex m_pbtCritSection;
	CSASPollMessage *m_lp74; // used for keeping track of PBT Lock status

	std::unique_ptr<std::thread> m_sendToEGMThread;
	std::unique_ptr<std::thread> m_scheduleThread;

	bool m_bShutdownSendToEGMThread;
	int m_totalRetryCount;
	DWORD m_lastEGMEnableTickCount;
	CTicketSeed *m_ticketSeed;
	CGames *m_sasInfo;
	CEGMAward *m_egmAward; // Used for AFT Bonusing.
	std::unique_ptr<CEGMAward> m_egmPrizeAward;// Used for AFT Prize Bonusing.
	std::recursive_mutex m_egmAwardCritSection;
	std::recursive_mutex m_egmAwardPrizeCritSection;
	CSASProtocolState m_sasState;
	DWORD m_numberOfSubgamesMismatchCheck;
	std::recursive_mutex m_doGpMutex;
};
