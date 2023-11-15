#pragma once

#include "PersonalBankerAutoUpload.h"
#include "PersonalBankerInfo.h"
#include "UI/Logs.h"
#include "UI/PersonalBankerLog.h"
#include "PersonalBankerTransfer.h"
#include "Games.h"
#include "GlobalDefs.h"
#include "Hardware/MemoryStorageEx.h"
#include "Meters.h"
#include "Player.h"
#include "TiltsDef.h"
#include "DataSubscriptionProvider.h"
#include "Jackpot.h"

const int VALID_PIN_LENGTH = 4;
const int PBT_UPLOAD_DELAY = 3;
const int PBT_UPLOAD_DELAY_PLAYER_ACTIVE = 1;

typedef enum
{
	// Do not change existing enumeration values if persisted.
	PBActionNone,
	PBActionGetEGMBalances,
	PBActionGetEGMBalancesWithMessage,
	PBActionSendTransferTransaction,
	PBActionReleaseSession,
	PBActionSendCardSessionInProgressToUI,
	PBActionStartTransferOnly,
	PBActionSendPersonalBankerNcepOutFail,
	PBActionSendDisplayPBAnimationToUI,
	PBActionSendDisplayPBAnimationToUIAndTransferToEGM,
	PBActionSendPersonalBankerInfoToUI,
	PBActionProcessRequestPlayerResume,
	PBActionSendAutoTransferCancelledToUI,
	PBActionSendCompleteToUI,
	PBActionSendAutoUploadCancelledMessage,
	PBActionProcessAuthorizationCanceled,
	PBActionProcessRequestPlayerResumeDueToCashInactive,
	PBActionProcessRequestPlayerResumeDueToGuestPlayer,
	PBActionSendCancelCashoutToHostToEGM,
	PBActionCancelAutoUploadDueToCashErrorSessionRelease,
	PBActionSendTaxDeductionRequest,
	NUMBER_OF_PB_ACTIONS,
} PersonalBankerAction;

extern const char* PersonalBankerActionStr[];

// Declare friend class(es) for unit test access.
namespace SentinelUnitTests {  class PersonalBankerUnitTests; }

// Declare friend class(es) for unit test access.
class PersonalBanker2;
class PersonalBanker4;
class PersonalBankerUnitTests;

union u_AutoUploadPendingFlags
{
	// first representation (member of union) 
	struct tAutoUploadPendingFlags
	{
		byte m_delayAutoUpload		   : 1;//Bit0
		byte m_cancelPendingAutoUpload : 1;//Bit1
		byte m_cancellable			   : 1;//Bit2
		byte m_spare				   : 5;//Bit3-7
	} m_flags;

	// second representation (member of union) 
	byte m_byteValue;

	u_AutoUploadPendingFlags()
	{
		m_byteValue = 0;
	}
};

class CPersonalBanker : public CDataSubscriptionProvider,
					    public CMemoryStorageEx
{
	// Grant "friend" access for testing.
	friend class SentinelUnitTests::PersonalBankerUnitTests;

public:
// #pragma region public constants
	static const DWORD NVRAM_PB_STATE_SIZE = sizeof(PersonalBankerState);	// Room to add stuff.

	static const DWORD NVRAM_PB_STATE_OFFSET =	NVRAM_PB_OFFSET + NVRAM_PB_CHECKBYTE_SIZE;

	static const WORD PB_SESSION_TIMEOUT	= 30000;	// PB session timeout in milliseconds.
	static const WORD PB_SESSION_TIMEOUT_MOBILE	= 60000;	// PB session timeout in milliseconds.
	static const DWORD PB_AUTHORIZATION_TIMEOUT = 120 * MILLISECONDS_PER_SECOND; // 120 * 1000 milliseconds == 2 minutes.

	static const DWORD TELL_UI_OF_FAILED_PB_SLEEP_MILLISECONDS = 3000;

	static const DWORD NVRAM_ACTIVE_OFFSET = NVRAM_PB_CHECKBYTE_OFFSET;
	static const BYTE NVRAM_ACTIVE_VALUE = NVRAM_PB_CHECKBYTE_VALUE;
	static const DWORD NVRAM_DATA_OFFSET = NVRAM_PB_DATA_OFFSET;
	static const DWORD NVRAM_DATA_SIZE = NVRAM_PB_DATA_SIZE;

	static const bool DEFAULT_PBT_AUTHORIZATION_IN_PROCESS = false;

	// Constants for EFT transaction ID value.
	static const int EFT_TRANS_ID_DEFAULT = 0; // Use 0 if a valid value is not available.
	static const int EFT_TRANS_ID_MINIMUM = 1; // Only valid if in the range 1..255 -- use 0 if it is not in that range.
	static const int EFT_TRANS_ID_MAXIMUM = 255; // Only valid if in the range 1..255 -- use 0 if it is not in that range.
	
// #pragma endregion public constants

	CPersonalBanker(MEMORY_TYPE memoryType = MEMORY_NONE);
	virtual ~CPersonalBanker(void);

	/// <summary>
	/// Gets the subscription data response.
	/// </summary>
	/// <param name="whereId">The subscription topic.</param>
	/// <param name="whereId">The where identifier.</param>
	/// <returns>subscription data response</returns>
	virtual std::string GetSubscriptionDataResponse(const std::string& topic, const std::string& whereId);

//Functions
	/// <summary>
	/// Updates the instance from an active NVRAM buffer.
	/// </summary>
	/// <param name="buffer">IN - The buffer.</param>
	/// <param name="bufferSize">IN - The size of the buffer.</param>
	virtual void SetFromNvramBuffer(const BYTE *buffer, UINT bufferSize);

	/// <summary>
	/// Gets the NVRAM buffer for the instance.
	/// </summary>
	/// <param name="buffer">OUT - The buffer.</param>
	/// <param name="bufferSize">IN - The size of the buffer.</param>
	/// <param name="active">OUT - true to indicate the the record is active, false to indicate inactive.</param>
	/// <returns>The size of the buffer actually used.</returns>
	virtual UINT GetNvramBuffer(BYTE *buffer, UINT bufferSize, bool& active) const;

	/// <summary>
	/// Logs current PBT transfer information.
	/// </summary>
	///	<param name="descriptionFormat">IN - Description of the condition being logged and
	/// format specification for any additional arguments.</param>
	void LogPBT(LPCTSTR descriptionFormat, ...) const;

	/// <summary>
	/// Deletes the transfer object.
	/// </summary>
	void DoneWithTransfer();

	/// <summary>
	/// Returns a copy of the Transfer object.
	/// </summary>
	CPersonalBankerTransfer* GetTransferCopy() const;

	/// <summary>
	/// Creates the Transfer object.
	/// </summary>
	/// <param name="tickCount">IN - The current tick count, generally from ::GetTickCount().</param>
	/// <param name="memoryType">Type of memory used for storing and retrieving the object.</param>
	/// <param name="bBuildFromNVRAM">If false, Personal Banker Transfer object is to be built from NVRAM, otherwise store it in NVRAM.</param>
	void CreateTransfer(DWORD tickCount, MEMORY_TYPE memoryType = MEMORY_NONE, bool bBuildFromNVRAM = false);

	/// <summary>
	/// Returns true if there is a transfer in progress.
	/// </summary>
	/// <returns>true if there is a transfer in progress, false otherwise.</returns>
	bool HasTransfer();

	/// <summary>
	/// Returns true if there is a transfer in progress for a different card number.
	/// <para/>
	/// The caller MUST ensure that the cardID byte array for the card number provided is NOT NULL.
	/// <para/>
	/// The caller MUST ensure that the length of the card number provided is EXACTLY the same
	/// length as the card number for any transaction already in progress.
	/// </summary>
	/// <param name="card">IN - card information to use for comparision.</param>
	/// <returns>true if there is a transfer in progress for a different card number, false otherwise.</returns>
	bool HasTransferForDifferentCardNumber(const CCard &card);

	/// <summary>
	/// Returns a copy of the Info object.
	/// </summary>
	/// <returns>A pointer to a copy of the Info instance if one exists, NULL otherwise.</returns>
	/// The conversion from char* to cstring leaves characters at the end. We fix it by truncating
	CPersonalBankerInfo* GetInfoCopy() const;

	/// <summary>
	/// Returns the PersonalBanker ID from the m_pbInfo instance.
	/// </summary>
	/// <returns>The PersonalBanker ID from the m_pbInfo instance if there is no m_pbInfo instance, an empty string otherwise .</returns>
	/// <remarks>
	/// The conversion from char* to cstring leaves characters at the end.
	/// </remarks>
	std::string GetInfoPersonalBankerID() const;

#ifdef _DEBUG
	/// <summary>
	/// Delete transfer and info.
	/// </summary>
	/// <remarks>
	/// This method is typically used when uninitializing a player -- AKA removing player at shutdown.
	/// This method should only be called when testing testing for memory leaks on nice shutdown.
	/// Do NOT call this method during normal operation, because it will clear this information from NVRAM (tracker 20165).
	/// </remarks>
	void DeleteTransferAndInfo();
#endif // _DEBUG.

	/// <summary>
	/// Returns a copy of the Log object.
	/// </summary>
	/// <returns>A pointer to a new allocated copy of the Log instance, this pointer has to be deleted to prevent memory leaks.</returns>
	CLogs* GetLogCopy();

	/// <summary>
	/// Returns a copy of the Success Log object.
	/// </summary>
	/// <returns>A pointer to a new allocated copy of the Success Log instance, this pointer has to be deleted to prevent memory leaks.</returns>
	CLogs* GetSuccessLogCopy();

	/// <summary>
	/// Returns the last personal banker log.
	/// </summary>
	/// <returns>A pointer to the CPersonalBankerLog object.</returns>
	CPersonalBankerLog* GetLastLogCopy();

	/// <summary>
	/// Sets the waiting for transfer to be completed flag.
	/// </summary>
	/// <param name="waitingForTransferToBeCompleted">
	///	If true, waiting is needed for transfer to be completed, otherwise waiting is not needed.
	///	</param>
	void SetWaitingForTransferToBeCompleted(bool waitingForTransferToBeCompleted);

	/// <summary>
	/// Returns true if waiting is needed for transfer to be completed is required, otherwise returns false.
	/// </summary>
	/// <returns>Returns a bool to indicate if PB is waiting for transfer to be completed.</returns>
	bool IsWaitingForTransferToBeCompleted() const;

	/// <summary>
	/// Sets the waiting for for denom check flag.
	/// </summary>
	/// <param name="waitingForDenomCheck">
	///	If true, waiting is needed for denom check, otherwise waiting for denom check is not needed.
	///	</param>
	void SetWaitingForDenomCheck(bool waitingForDenomCheck);

	/// <summary>
	/// Returns true if the waiting for denom check is needed, otherwise returns false.
	/// </summary>
	/// <returns>Returns a bool to indicate if PB is waiting for a denom check</returns>
	bool IsWaitingForDenomCheck() const;

	/// <summary>
	/// Clear the personal banker action to PBActionNone.
	/// </summary>
	void ClearPersonalBankerAction();

	/// <summary>
	/// Returns the personal banker action.
	/// </summary>
	/// <returns>Returns PersonalBankerAction enum</returns>
	PersonalBankerAction GetAction() const;

	/// <summary>
	/// Sets the personal banker state.
	/// </summary>
	/// <param name="state">Personal Bankers States(PBStateInquireSent, PBStateInfoReceived, PBStateTransferingToGame, etc...)
	///	</param>
	void SetState(PersonalBankerState state);

	/// <summary>
	/// Returns the personal banker state.
	/// </summary>
	/// <returns>Returns PersonalBankerState enum</returns>
	PersonalBankerState GetState() const;

	/// <summary>
	/// Returns the personal banker state for UI when Personal Banker transaction completes.
	/// </summary>
	/// <returns>Returns PersonalBankerState enum</returns>
	PersonalBankerState GetPbStateForUi() const;

	/// <summary>
	/// Returns true if personal banker transfer is in progress, otherwise returns false.
	/// </summary>
	/// <returns>Returns a bool to indicate personal banker transfer is in progress.</returns>
	bool IsPbtInProgress() const;

	/// <summary>
	/// Removes the personal banker session.
	/// Deletes personal banker transfer and informations objects.
	/// </summary>
	void RemoveSession();

	/// <summary>
	/// Sets the transfer card number.
	/// </summary>
	/// <param name="card">Card associated with the transfer.</param>
	/// <param name="time">Time associated with the transfer.</param>
	void SetTransferCardNumber(const CCard* card, time_t time);

	/// <summary>
	/// Deletes card info associated with auto upload.
	/// </summary>
	void DeleteCard();

	/// <summary>
	/// Creates a new card associated with personal banker.
	/// </summary>
	void RebuildCard();

	/// <summary>
	/// Creates a new card associated with personal banker.
	/// </summary>
	/// <param name="card">This card is copied into the personal banker transfer object.</param>
	void RebuildCard(const CCard* card);

	/// <summary>
	/// Begins a personal banker auto session.
	/// Returns with m_pbTransfer != NULL.
	/// Always returns false if sendToPoller == false.
	/// </summary>
	/// <param name="sendToPoller">Flag to indicate whether a new transfer will need a PB Transaction sent to the Poller.</param>
	/// <param name="config">Configuration object</param>
	/// <param name="autoUploadPin">Pin associated with the auto upload.</param>
	/// <param name="tickCount">IN - The current tick count, generally from ::GetTickCount().</param>
	/// <returns>true if a PB transaction needs to be sent to the Poller, false otherwise.</returns>
	bool BeginPersonalBankerAutoSession(bool sendToPoller, const CConfig& config, DWORD autoUploadPin, DWORD tickCount);

	/// <summary>
	/// Prepare personal banker transfer transaction.
	/// </summary>
	/// <param name="pbSequence">IN/OUT - personal banker sequence number</param>
	void PreparePersonalBankerTransfer(WORD& pbSequence);

	/// <summary>
	/// Returns true if personal banker transfer is in progress, otherwise returns false.
	/// </summary>
	/// <returns>Returns a bool to indicate personal banker transfer is in progress.</returns>
	bool IsPersonalBankerTransferInProgress() const;

	/// <summary>
	/// Returns true if personal banker if the transfer type is to the game, otherwise returns false.
	/// </summary>
	/// <returns>Returns a bool to indicate if personal banker transfer type is to the game.</returns>
	bool IsPersonalBankerTransferToGame() const;

	/// <summary>
	/// Begins personal banker session.
	/// </summary>
	/// <param name="player">IN - player associated with the session</param>
	/// <param name="pbPin">IN - pin entered by the player to access personal banker</param>
	/// <param name="config">IN - configuration object</param>
	/// <param name="cashoutToHostSession">IN - whether session to start is cashout to host session</param>
	bool BeginPersonalBankerSession(CPlayer* player, const std::string& pbPin, CConfig& config, 
		bool cashoutToHostSession = false, CJackpot* jackpot = nullptr);

	/// <summary>
	/// Performs card timed events for players.
	/// </summary>
	/// <param name="delayAutoNcepOutWhenNotDefaultedPlayer">IN - true to delay auto NCEP out when not defaultedPlayer, otherwise use false.</param>
	/// <param name="haveEmployee1">IN - true if internals has a first employee carded in, false otherwise.</param>
	/// <param name="config">IN - nCompass system configuration: configPBTPointsAsPromo, configCEPOutEnabled, configDenomination, etc.</param>
	/// <param name="tickCount">IN - The current tick count, generally from ::GetTickCount().</param>
	/// <param name="sessionTimeoutValueInMilliseconds">IN - Session timeout value in milliseconds</param>
	/// <param name="authorizationTimeoutValueInMilliseconds">IN - Authorization timeout value in milliseconds.</param>
	void PerformTimedEvents(bool delayAutoNcepOutWhenNotDefaultedPlayer,
		bool haveEmployee1,
		const CConfig& config,
		DWORD tickCount,
		DWORD sessionTimeoutValueInMilliseconds = PB_SESSION_TIMEOUT,
		DWORD authorizationTimeoutValueInMilliseconds = PB_AUTHORIZATION_TIMEOUT);

	/// <summary>
	/// Process PersonalBanker balances from GCF.
	/// </summary>
	/// <param name="wat">IN - amount of cash on game.</param>
	/// <param name="cep">IN - cash o promo on game.</param>
	/// <param name="ncep">IN - amount of promo on game.</param>
	/// <param name="bCanTransferOff">IN - true if can transfer off, false otherwise.</param>
	/// <param name="bCallToGameFailed">IN - true if call to EGM failed, false otherwise.</param>
	/// <param name="config">IN - nCompass system configuration: configPBTPointsAsPromo, configCEPOutEnabled, configDenomination, etc.</param>
	/// <param name="eftMode">IN - true if EFT mode, false otherwise.</param>
	void ProcessBalancesFromEGM(DWORD wat,
		DWORD cep,
		DWORD ncep,
		bool bCanTransferOff,
		bool bCallToGameFailed,
		const CConfig& config,
		bool eftMode = false);

	/// <summary>
	/// Process Personal Banker Transfer Complete message.
	/// </summary>
	/// <param name="pbTransfer">IN - Personal Banker Transfer.</param>
	/// <param name="games">IN - contains game information.</param>
	/// <param name="meterNumberToAddTo">OUT - meter number to add this transaction to.  mNone for no meter update.</param>
	/// <param name="isDuplicate">OUT - true if last success transaction id matches the current transaction id, false otherwise.</param>
	/// <param name="isEFT">IN - true if EFT, false otherwise.</param>
	void ProcessPersonalBankerTransferComplete(const CPersonalBankerTransfer *pbTransfer,
		CGames& games,
		byte& meterNumberToAddTo,
		bool& isDuplicate,
		bool isEFT);

	void ProcessPersonalBankerCreditTransfer(const CPersonalBankerInfo *pbInfo);

	/// <summary>
	/// Process PersonalBankerInfo.
	/// </summary>
	/// <param name="pbInfo">IN/OUT - PersonalBanker info.  Should NOT be NULL.</param>
	/// <param name="config">IN - nCompass system configuration.</param>
	/// <param name="gameOnline">IN - true if game online, false otherwise.</param>
	/// <param name="bisEFT">IN - true if EFT, false otherwise.</param>
	void ProcessPersonalBankerInfo(const CPersonalBankerInfo *pbInfo,
		const CConfig &config,
		bool gameOnline,
		bool bIsEFT);

	/// <summary>
	/// End an active player session.
	/// </summary>
	/// <param name="config">IN/OUT - nCompass system configuration.</param>
	void EndActivePlayerSession(CConfig const &config);

	/// <summary>
	/// Save the current (recently completed) transfer to the persisted PersonalBanker logs.
	/// </summary>
	/// <param name="player">IN - player associated with the personal banker</param>
	/// <param name="config">IN - configuration object</param>
	/// <param name="currentTime">IN - current time</param>
	void LogTransfer(CPlayer* player, const CConfig& config, const time_t currentTime);

	/// <summary>
	/// This gets called at start-up to process a pending personal banker transaction.
	/// </summary>
	/// <param name="config">IN - Configuration object</param>
	void ProcessInitialPersonalBanker(const CConfig& config);

	/// <summary>
	/// This gets called to begin personal banker transfer.
	/// </summary>
	/// <param name="games">IN - Games object</param>
	/// <param name="type">IN - Personal Banker Type</param>
	/// <param name="amountInDollars">IN - Amount in dollars</param>
	/// <param name="pbEFTNumber">IN/OUT - EFT Number</param>
	bool BeginPersonalBankerTransfer(CGames& games, PersonalBankerTransferType type, DWORD amountInDollars, byte& pbEFTNumber);

	/// <summary>
	/// This gets called to check if authorization is needed from the poller 
	/// before we can transfer to the game.
	/// </summary>
	/// <param name="type">IN - Personal Banker Type</param>
	/// <param name="amountInDollars">IN - Amount in dollars</param>
	/// <param name="atomicRedemptions">IN - Atomic Redemptions</param>
	/// <returns>
	/// Returns true if authorization is needed, returns false otherwise.
	///	</returns>
	bool CheckIfAuthorizationIsNeeded(PersonalBankerTransferType type, DWORD amountInDollars, bool atomicRedemptions);

	/// <summary>
	/// This gets called to release personal banker session.
	/// </summary>
	/// <param name="config">IN - Configuration object</param>
	/// <param name="canceledFromUI">IN - Cancelled from UI flag</param>
	void ReleasePersonalBankerSession(const CConfig& config, bool canceledFromUI);

	/// <summary>
	/// This sets to the transfer session flags to be released.
	/// </summary>
	void SetToReleaseTransferSession();

	/// <summary>
	/// Processes the cancel personal banker session.
	/// </summary>
	/// <param name="pbTransfer">The pb transfer.</param>
	/// <param name="displayCancelSession">The display cancel session.</param>
	/// <returns>Returns true if personal banker session has to be released. Otherwise, returns false.</returns>
	bool ProcessCancelPersonalBankerSession(CPersonalBankerTransfer *personalBankerTransfer = NULL, bool displayCancelSession = false);

	void ProcessRestorePersonalBankerSession();

	/// <summary>
	/// This gets called to process personal banker transaction acknowledged message from the poller.
	/// </summary>
	void ProcessPersonalBankerTransactionAcked(WORD sequenceNumber);

	/// <summary>
	/// Returns true if EGM can be unlocked to do a personal banker transfer, false otherwise.
	/// </summary>
	/// <returns>True if EGM can be unlocked during transfer, false otherwise</returns>
	bool GetUnlockEgmDuringTransfer() const;

	/// <summary>
	/// Returns personal banker transfer type.
	/// </summary>
	/// <returns>Returns PersonalBankerTransferType enum</returns>
	PersonalBankerTransferType GetPersonalBankerTransferType() const;

	/// <summary>
	/// Returns personal banker transfer amount.
	/// </summary>
	/// <returns>Returns an amount value in __int64</returns>
	__int64 GetTransferAmount();

	/// <summary>
	/// Sets the personal banker type.
	/// </summary>
	/// <param name="type">personal banker type</param>
	void SetPersonalBankerType(PersonalBankerType type);

	/// <summary>
	/// Sets the personal banker transfer type.
	/// </summary>
	/// <param name="type">personal banker type</param>
	void SetPersonalBankerTransferType(PersonalBankerTransferType type);

	/// <summary>
	/// Returns true if transfer has an amount.
	/// </summary>
	/// <returns>Returns a bool to indicate transfer has amount greater than 0</returns>
	bool TransferHasAmount();

	/// <summary>
	/// Returns true if transfer request is cancelled; returns false otherwise.
	/// </summary>
	/// <returns>Returns a bool to indicate transfer request is cancelled</returns>
	bool IsTransferRequestCanceled() const;

	/// <summary>
	/// Resets the transfer session start time to tickCount.
	/// </summary>
	/// <param name="tickCount">IN - The current tick count, generally from ::GetTickCount().</param>
	void ResetTransferSessionStartTime(DWORD tickCount);

	/// <summary>
	/// Sets the auto in process flag
	/// </summary>
	/// <param name="autoInProcess">true if auto is in process; false otherwise</param>
	void SetAutoInProcess(bool autoInProcess);

	/// <summary>
	/// Gets the auto in process flag
	/// </summary>
	/// <returns>Returns a bool to indicate auto transfer in process</returns>
	bool IsAutoInProcess() const;

	/// <summary>
	/// Sets the auto ncep out pending flag to true.
	/// </summary>
	void MakeAutoUploadPending();

	/// <summary>
	/// Returns true if auto transfer is pending; returns false otherwise.
	/// </summary>
	/// <returns>Returns a bool to indicate auto transfer is pending</returns>
	bool IsAutoTransferPending();

	/// <summary>
	/// Sets the auto upload time to expired.
	/// </summary>
	void SetAutoUploadTimerExpired();

	/// <summary>
	/// Returns a pointer to card object associated with the auto upload.
	/// </summary>
	/// <returns>Returns a pointer to the CCard object associated with the auto upload</returns>
	CCard* GetAutoUploadCard();

	/// <summary>
	/// Returns true if auto ncep out is pending; returns false otherwise.
	/// </summary>
	/// <returns>Returns a bool to indicate auto ncep is pending</returns>
	bool IsAutoUploadPending() const;

	/// <summary>
	/// Returns true if session is released to due to a time out, false otherwise.
	/// </summary>
	/// <returns>Returns a bool to indicate if pb session is released due to a timeout.</returns>
	bool IsSessionReleasedDueToTimeout() const;

	/// <summary>
	/// Returns true if invalid transcation ID was received from poller, false otherwise.
	/// </summary>
	/// <returns>Returns a bool to indicate if invalid transcation ID was received from poller.</returns>
	bool IsInvalidTransactionIDReceived() const;

	/// <summary>
	/// Determines if time has expred since last Carded Session In Progress last sent to UI.
	/// </summary>
	/// <returns>Returns PBActionSendCardSessionInProgressToUI if time has expired.</returns>
	PersonalBankerAction LimitSessionInProgressToUIExpiredAction();

	/// <summary>
	/// Sets last carded session in progress to UI time to current tick count.
	/// </summary>
	/// <param name="custom">Tick count will be set to custom value if supplied. Used for unit testing</param>
	/// <returns></returns>
	void SetLastCardedSessionInProgressToCurrent(DWORD custom = 0);

	bool IsCreditTransfer() const;

	/// <summary>
	/// Verifies the transfered amounts for transfers onto the game and verifies that all amounts not above the maximum allowed amount.
	/// </summary>
	/// <param name="transfer">Completed transfer information</param>
	/// <returns>tltInvalidPbAmount or tltTransferOvrLmtAftr when amount is invalid, tltUnknown otherwise.</returns>
	TiltCode VerifyTransferedAmounts(CPersonalBankerTransfer *transfer);

	/// <summary>
	/// Sets the poller cancellation type to invalid transaction id and sets state to PBActionSendCompleteToUI.
	/// </summary>
	/// <param name="pbInfo">IN - personal banker info</param>
	/// <param name="slotMastId">IN - slotmast ID</param>
	/// <param name="personalBankerId">OUT - personalBankerId</param>
	bool ProcessInvalidTransIdIfNecessary(CPersonalBankerInfo *pbInfo, long slotMastId, std::string& personalBankerId);

	/// <summary>
	/// Checks that the requested transfer amount is valid
	/// </summary>
	/// <param name="type">The type.</param>
	/// <param name="amount">The amount in pennies.</param>
	/// <returns></returns>
	bool CheckValidTransferAmount(PersonalBankerTransferType type, DWORD amount);

	/// <summary>
	/// Determines whether [is pin valid length] [the specified pin].
	/// </summary>
	/// <param name="pin">The pin.</param>
	/// <returns>true if valid length, false otherwise.</returns>
	static bool IsPinValidLength(const std::string& pin);

	/// <summary> 
	/// Check if Authorisation is in-progress. 
	/// </summary>
	bool IsAuthorisationInProcess();
	
	/// <summary> 
	/// Checks if the tranfer type is to-game. 
	/// <param name="type">PB Transfer type.</param> 
	/// </summary>
	bool IsToGame(PersonalBankerTransferType type);
	
	/// <summary> 
	/// Check if transfer is successful. 
	/// </summary>
	bool IsTransferSuccess();

	/// <summary>
	/// Stores the PlayerId who is Waiting for Pin Reset. 
	/// Handles the scenario when player tries to reset pin and PC goes down. In the meantime, another player inserts card. 
	/// Pop up for successful reset comes for new player
	/// </summary>
	/// <param name="playerId">IN - PlayerId who is Waiting for Pin Reset.</param>
	/// <returns>None.</returns>
	void SetPlayerIdWaitingPinReset(DWORD playerId);

	/// <summary>
	/// Compares the passed palyer-id with the stored PlayerId who was Waiting for Pin Reset. If both are same then it means 
	/// player id was waiting for pin-reset otherwise it will return false which would discard the pin-reset response message.
	/// Handles the scenario when player tries to reset pin and PC goes down. In the meantime, another player inserts card. 
	/// Pop up for successful reset comes for new player
	/// </summary>
	/// <param name="playerId">IN - Current PlayerId to compare with the stored PlayerId who was Waiting for Pin Reset.</param>
	/// <returns>Whether passed palyer-id is same as the stored PlayerId who was Waiting for Pin Reset.</returns>
	bool IsPlayerIdWaitingPinReset(DWORD playerId);

	/// <summary>
	/// returns the Current pin for the player required during for Pin Reset. 
	/// </summary>
	/// <param name="pin">IN - Current pin for the player.</param>
	/// <returns>Current PIN.</returns>
	DWORD GetCurrentPin();

	/// <summary>
	/// Stores the new pin for the player who is Waiting for Pin Reset. 
	/// </summary>
	/// <param name="pin">IN - new pin for the player who is Waiting for Pin Reset.</param>
	/// <returns>None.</returns>
	void SetNewPin(DWORD pin);

	/// <summary>
	/// returns the new pin for the player who was waiting for Pin Reset. 
	/// </summary>
	/// <returns>New PIN.</returns>
	DWORD GetNewPin();

	/// <summary>
	/// Returns true if player's cash is inactive, false otherwise.
	/// </summary>
	/// <returns>true if player's cash is inactive, false otherwise.</returns>
	bool IsCashInactive() const;

	/// <summary>
	/// Clears the inactive accounts flag.
	/// </summary>
	void ClearInactiveAccountsFlag();

	/// <summary>
	/// Set eligible credits to upload for cashout-to-host sessions.
	/// Cash part includes both WAT and CEP (if CEP-Out enabled)
	/// </summary>
	void SetEligibleCreditsToUpload(DWORD cash, DWORD promos);

	/// <summary>
	/// Sets the guest player flag
	/// </summary>
	/// <param name="guest">IN - guest player flag.</param>
	void SetGuestPlayer(bool guest);

	const char* GetActionString(PersonalBankerAction action);

	/// <summary>
	/// Create pending Auto Upload card
	/// </summary>
	void SetToDelayAutoUpload();

	/// <summary>
	/// Cancels pending Auto Upload if necessary
	/// </summary>
	/// <param name="pollException">IN - poll exception.</param>
	void CancelPendingAutoUploadIfNecessary(WORD pollException);

	/// <summary>
	/// Checks display checking for uploadable credits flag
	/// </summary>
	/// <returns>true if display checking for uploadable credits is set, false otherwise.</returns>
	bool DelayAutoUpload();

	/// <summary>
	/// Set auto upload delay value
	/// </summary>
	/// <param name="seconds">IN - length of time to delay auto upload in seconds.</param>
	void SetAutoUploadDelayValue(DWORD seconds);

	/// <summary>
	/// Stop delaying auto upload
	/// </summary>
	void StopDelayAutoUpload() { m_autoUploadPendingFlags.m_flags.m_delayAutoUpload = false; }

	/// <summary>
	/// Appends the cashout error flag into PB transfer instance.
	/// </summary>
	/// <param name="error">IN - cashout error flags.</param>
	void AppendCashoutErrorFlags(WORD error);

	/// <summary>
	/// Gets the cashout error flags from PB transfer instance.
	/// </summary>
	/// <returns>Cashout Error Flags.</returns>
	WORD GetCashoutErrorFlags() const;

	/// <summary>
	/// Gets the autoupload error flags from PB transfer instance.
	/// </summary>
	/// <returns>AutoUpload Error Flags.</returns>
	WORD GetAutoUploadErrorFlags() const;

	/// <summary>
	/// Validates the credit for cashout to PBT and returns the error flag if any.
	/// </summary>
	/// <param name="player">IN - player associated with cashout request.</param>
	/// <param name="eligibleCashToUpload">IN/OUT - eligible cash to upload.</param>
	/// <param name="eligiblePromosToUpload">IN/OUT - eligible promo to upload.</param>
	/// <returns>Cashout error flag if any, otherwise returns 0.</returns>
	WORD ValidateCreditsForCashoutToHost(const CPlayer *player, DWORD &eligibleCashToUpload, DWORD &eligiblePromosToUpload) const;
	
	/// <summary>
	/// Set reset pin result
	/// </summary>
	/// <param name="seconds">IN - result of reset pin operation.</param>
	void SetResetPinResult(WORD resetPinResult);

	/// <summary>
	/// Clear reset pin result
	/// </summary>
	void ClearResetPinResult();

	/// <summary>
	/// Sets session force exit flag in PBInfo instance.
	/// </summary>
	void SetSessionForceExit();

	/// <summary>
	/// Gets session force exit flag from PBInfo instance.
	/// </summary>
	bool IsSessionForceExit() const;

private:
//Variables
	mutable std::recursive_mutex m_CriticalSection;

	bool m_waitingForTransferToBeCompleted;
	bool m_waitingForDenomCheck;

	char* m_previousTransferId;
	int m_previousTransferIdLength;
	
	CPersonalBankerAutoUpload m_autoUpload;
	CPersonalBankerInfo* m_pbInfo;
	CLogs m_pbLog;
	CLogs m_pbSuccessLog;
	CPersonalBankerTransfer* m_pbTransfer;
	PersonalBankerAction m_readyAction;
	PersonalBankerState m_state;
	PersonalBankerState m_pbStateForUi;
	bool m_ReleaseSessionDueToTimeout;
	bool m_InvalidTransIdReceived;
	bool m_pbtAuthorizationInProcess;// true if the most recent transfer attempt used authorization, false otherwise.
	u_AutoUploadPendingFlags m_autoUploadPendingFlags;
	DWORD m_lastSessionInProgressToUI;
	bool m_cashInactive;
	bool m_guestPlayer;
	bool m_isUploadableCashIgnored;  // true if uploadable cash is ignored due to Cash Inactive or Guest Player

	bool m_receivedBalancesFromEGM;
	WORD m_resetPinResult;

//Functions
	void CleanUp();
	void CreateInfo(MEMORY_TYPE memoryType = MEMORY_NONE, bool bBuildFromNVRAM = false);
	void DeleteInfo();
	void DeleteTransfer();
	void CreatePreviousTransferId();
	void DeletePreviousTransferId();
	void CheckIfTransferMatchWhatWeHaveInMemory(const CPersonalBankerTransfer *pbTransfer);
	void SetPbtAuthorizationInProcess(bool pbtAuthorizationInProcess);

	/// <summary>
	/// Calculate and return the original PersonalBanker type.
	/// </summary>
	/// <returns>the calculated PersonalBanker type.</returns>
	PersonalBankerType GetOriginalPBType();

	/// <summary>
	/// Check if meters need to be updated.
	/// </summary>
	/// <param name="wasTransferCompleteExpected">IN - true if we expected this transfer complete, false otherwise.
	/// <param name="isEFT">IN - true if EFT, false otherwise.</param>
	/// <param name="meterNumberToAddTo">OUT - meter number to add this transaction to.  mNone for no meter update.</param>
	/// <param name="originalPBType">IN - original PersonalBanker type for transfer.</param>
	void GetMeterToUpdate(bool wasTransferCompleteExpected,
		bool isEFT,
		byte& meterNumberToAddTo,
		PersonalBankerType originalPBType);

	void CheckIfPersonalBankerTypeHasToBeChanged(PersonalBankerType originalPBType,
		bool wasTransferCompleteExpected, 
		bool& isDuplicate);
	void SetPersonalBankerAction(PersonalBankerAction action);

	/// <summary>
	/// Returns true if personal banker transfer type is from game, otherwise returns false.
	/// </summary>
	/// <returns>Returns a bool to indicate if personal banker transfer type is from the game.</returns>
	bool IsPersonalBankerTransferFromGame() const;

	/// <summary>
	/// Performs timed events for auto uploads.
	/// </summary>
	/// <param name="delayAutoNcepOutWhenNotDefaultedPlayer">IN - true to delay auto NCEP out when not defaultedPlayer, otherwise use false.</param>
	/// <param name="haveEmployee1">IN - true if internals has a first employee carded in, false otherwise.</param>
	/// <param name="tickCount">IN - The current tick count, generally from ::GetTickCount().</param>
	/// <param name="config">IN - nCompass system configuration.</param>
	void PerformTimedEventsAuto(bool delayAutoNcepOutWhenNotDefaultedPlayer,
		bool haveEmployee1,
		const CConfig& config,
		DWORD tickCount);

	/// <summary>
	/// Performs timed events for auto uploads with not defaulted players.
	/// </summary>
	/// <param name="haveEmployee1">IN - true if internals has a first employee carded in, false otherwise.</param>
	/// <param name="config">IN - nCompass system configuration.</param>
	/// <param name="tickCount">IN - The current tick count, generally from ::GetTickCount().</param>
	void PerformTimedEventsAutoNotDefaulted(bool haveEmployee1,
		const CConfig& config,
		DWORD tickCount);

	/// <summary>
	/// Performs timed events for sending, releasing, or creating transfers.
	/// </summary>
	/// <param name="config">IN - nCompass system configuration.</param>
	/// <param name="tickCount">IN - The current tick count, generally from ::GetTickCount().</param>
	/// <param name="sessionTimeoutValueInMilliseconds">IN - Session timeout value in milliseconds</param>
	/// <param name="authorizationTimeoutValueInMilliseconds">IN - Authorization timeout value in milliseconds.</param>
	void PerformTimedEventsTransfer(const CConfig& config,
		DWORD tickCount,
		DWORD sessionTimeoutValueInMilliseconds,
		DWORD authorizationTimeoutValueInMilliseconds);

	/// <summary>
	/// Process PersonalBankerInfo when an auto upload is in process.
	/// </summary>
	/// <param name="pbInfo">IN/OUT - PersonalBanker info.  Should NOT be NULL.</param>
	/// <param name="config">IN - nCompass system configuration.</param>
	/// <param name="gameOnline">IN - true if game online, false otherwise.</param>
	/// <param name="bisEFT">IN - true if EFT, false otherwise.</param>
	/// <param name="config">IN - Configuration object.</param>
	void ProcessPersonalBankerInfoAuto(const CPersonalBankerInfo *pbInfo, const CConfig &config);

	/// <summary>
	/// Process PersonalBankerInfo when an auto upload is not in process.
	/// </summary>
	/// <param name="pbInfo">IN/OUT - PersonalBanker info.  Should NOT be NULL.</param>
	/// <param name="config">IN - nCompass system configuration.</param>
	/// <param name="gameOnline">IN - true if game online, false otherwise.</param>
	/// <param name="bisEFT">IN - true if EFT, false otherwise.</param>
	void ProcessPersonalBankerInfoNotAuto(const CPersonalBankerInfo *pbInfo);

	/// <summary>
	/// This gets called to release personal banker sessions related to auto uploads.
	/// </summary>
	/// <param name="canceledFromUI">IN - Cancelled from UI flag.</param>
	/// <param name="sendToUI">IN/OUT - Send to UI flag.</param>
	void ReleasePersonalBankerSessionAutoUpload(
		bool canceledFromUI,
		bool &sendToUI);

	/// <summary>
	/// This gets called to release personal banker sessions and generate actions to perform.
	/// </summary>
	/// <param name="config">IN - Configuration object.</param>
	/// <param name="canceledFromUI">IN - Cancelled from UI flag.</param>
	/// <param name="sendToUI">IN - Send to UI flag.</param>
	void ReleasePersonalBankerSessionAction(const CConfig &config,
		bool canceledFromUI,
		bool sendToUI);

	DWORD m_playerIdWaitingPinReset;
	DWORD m_newPin;
	
	// Eligible cash and promos to upload for cashout-to-host, not-persisted
	DWORD m_eligibleCashToUpload;
	DWORD m_eligiblePromosToUpload;

	std::string ResponseForPbPendingLogs();
	std::string ResponseForPbLogs(const std::string& topic);
	std::string ResponseForPBInfo();
	std::string ResponseForPBTransfer();
	std::string ResponseForResetPin();

	DWORD m_pendingAutoUploadTickCount;
	DWORD m_autoUploadDelaySeconds;

	// Grant "friend" access for testing.
	friend class SentinelUnitTests::PersonalBankerUnitTests;
	friend class PersonalBanker2;
    friend class PersonalBanker4;
	FRIEND_TEST(PersonalBanker4, GetSubscriptionDataResponse_PBTransferTest1);
	FRIEND_TEST(PersonalBanker4, GetSubscriptionDataResponse_PBTransferTest2);
	FRIEND_TEST(PersonalBanker4, GetSubscriptionDataResponse_PBTransferTest3);
	FRIEND_TEST(PersonalBanker4, ProcessPersonalBankerInfoAutoPoller_12_11_6000_or_Higher);
	FRIEND_TEST(PersonalBanker4, ProcessPersonalBankerInfo_AutoInfoNullNoErrorInfoIDTestPoller_12_11_6000_or_Higher);
	FRIEND_TEST(PersonalBanker2, ProcessCancelAutoUPloadPersonalBankerSession);
	FRIEND_TEST(PersonalBanker2, ReleasePersonalBankerSessionUTJTest);
	FRIEND_TEST(PersonalBanker2, PerformTimedEventsAutoCancelPendingAutoUpload);
	FRIEND_TEST(PersonalBanker2, SetAutoUploadDelayValueTest);
	FRIEND_TEST(PersonalBanker2, SetToDelayAutoUploadCashTest);
	FRIEND_TEST(PersonalBanker2, CancelPendingAutoUploadIfNecessaryTest);
	FRIEND_TEST(PersonalBanker2, DelayAutoUploadTest);
	FRIEND_TEST(PersonalBanker2, ProcessBalancesFromEGMDelayAutoUploadTest);
	FRIEND_TEST(PersonalBanker2, ValidateCreditsForCashoutToHost_CashInactive);
	
};
