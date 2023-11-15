#pragma once
#include "stdafx.h"

#include "Hardware/MemoryStorageEx.h"
#include "InternalMsgContent.h"
#include "ProgressiveKeys.h"
#include "OptionalVal.h"

// Declare friend class(es) for unit test access.
namespace SentinelNativeUnitTests { class EGMAward; }
namespace SentinelNativeUnitTests { class EGMAwardManager; }

// This enumeration is persisted. Do not change existing values without addressing upgrades.
enum EGMAwardState
{
	NoAward,
	ReceivedFromSystem,
	WaitForMetersInitial,
	SentToGame,
	GameBusy, 
	Signaled, // paltronics1__JackpotPaidStatus__Signaled.
	HandpayPending,	// paltronics1__JackpotPaidStatus__Pending.
	AwardToGameSuccess, // paltronics1__JackpotPaidStatus__Paid.
	AwardToGameError, // paltronics1__JackpotPaidStatus__Failed.
	AwardToGameIneligible, // paltronics1__JackpotPaidStatus__Ineligible.
	AftLockPending, // lock pending in SAS
	AwardSessionMismatch, // paltronics1__JackpotPaidStatus__SessionMismatch.
	ReceivedFromGame,
	SentToSystem,
};

// This enumeration is persisted. Do not change existing values without addressing upgrades.
enum EGMAwardTransferType
{
	EGMAwardTransferType_None,
	EGMAwardTransferType_SASLegacyBonusing,
	EGMAwardTransferType_SASAFTBonusingCash,
};

// This enumeration is persisted. Do not change existing values without addressing upgrades.
enum EGMAwardPaidType
{
	EGMAwardPaidType_Unknown,
	EGMAwardPaidType_Attendant,
	EGMAwardPaidtype_Machine,
};

class CInternalMessage;

class CEGMAward :
	public CMemoryStorageEx,
	public CInternalMsgContent<INT_MSG_CONTENT_EGM_AWARD>
{
public:
	static const LONG64 INVALID_JACKPOT_ID = -1;
	static const MethodOfPayment::Type DEFAULT_METHOD_OF_PAYMENT = MethodOfPayment::Invalid;
	static const MethodOfPayment::Type UPGRADE_METHOD_OF_PAYMENT = MethodOfPayment::PayToGame; // From prior to v1.50.0.

	/// <summary>
	/// Default constructor. Initializes a new instance of the <see cref="CEGMAward"/> class.
	/// </summary>
	CEGMAward();

	/// <summary>
	/// CEGMAward json deserialize constructor.
	/// </summary>
	/// <param name="receivedTime">The time award message was received from OneLink.</param>
	/// <param name="jsonDoc">The json document to use for deserialization.</param>
	CEGMAward(const SYSTEMTIME& receivedTime, const rapidjson::Document& jsonDoc);

	/// <summary>
	/// Memory type constructor. Initializes a new instance of the <see cref="CEGMAward"/> class.
	/// </summary>
	/// <param name="memoryType">IN - The type of persistent memory to use for the instance.</param>
	/// <param name="bBuildFromNVRAM">IN - true to build the instance from NVRAM, false otherwise.</param>
	/// <param name="nvramActiveOffset">IN - The NVRAM offset for the active flag,
	/// indicating whether an instance is stored in the following memory.</param>
	CEGMAward(MEMORY_TYPE memoryType, bool bBuildFromNVRAM, DWORD nvramActiveOffset);

	/// <summary>
	/// Destructor. Finalizes an instance of the <see cref="CEGMAward"/> class.
	/// </summary>
	~CEGMAward();

	/// <summary>
	/// Copy constructor. Initializes a new instance of the <see cref="CEGMAward"/> class.
	/// </summary>
	/// <param name="rhs">IN - The source instance.</param>
	/// <remarks>
	/// The memory type of the new instance is set to MEMORY_NONE.
	/// </remarks>
	CEGMAward(const CEGMAward &rhs);

	/// <summary>
	/// Assigment operator.
	/// </summary>
	/// <param name="rhs">IN - The source instance.</param>
	/// <returns>A reference to the target of the assignment.</returns>
	/// <remarks>
	/// The memory type of the target instance is retained.
	/// </remarks>
	CEGMAward& operator=(const CEGMAward &rhs);

	/// <summary>
	/// Initializes an instance of the <see cref="CEGMAward"/> class.
	/// </summary>
	void Init();

	/// <summary>
	/// Sets the memory storage parameters.
	/// If nvramActiveOffset != NVRAM_DEFAULT_OFFSET, the instance is persisted.
	/// </summary>
	/// <param name="nvramActiveOffset">IN - The NVRAM offset for the active flag, indicating whether an instance is stored
	/// in the following memory. Use CMemoryStorage::NVRAM_DEFAULT_OFFSET for a non-persisted instance.</param>
	void SetMemoryStorage(DWORD nvramActiveOffset);

	/// <summary>
	/// Determines whether the jackpotId is valid.
	/// </summary>
	/// <returns>true if the jackpotId is valid, false otherwise.</returns>
	/// <remarks>
	/// This method is not intended to be called for remote jackpot ID values.
	/// </remarks>
	static bool IsValidJackpotId(LONG64 jackpotId);

	/// <summary>
	/// Determines whether the inital jackpot information and setup is good enough to try processing.
	/// </summary>
	/// <returns>true if the instance is good enough to try processing, false otherwise.</returns>
	/// <remarks>
	/// This method is not intended to be called except after initialization.
	/// This method sets the status info if the initial jackpot information is not good enough.
	/// </remarks>
	bool IsValidJackpotHitInitalization();

	/// <summary>
	/// Determines whether the instance to compare is for the same jackpot hit from the progressive protocol.
	/// </summary>
	/// <param name="rhs">IN - The instance to compare.</param>
	/// <returns>true if the progressive protocol jackpot IDs match, false otherwise.</returns>
	bool IsSameHit(const CEGMAward &rhs) const;

	/// <summary>
	/// Sets the check byte location to indicate that there is no active record.
	/// </summary>
	/// <returns></returns>
	static void SetInactiveLocation();

	/// <summary>
	/// Makes a system date/time valid enough for a conversion to CTime without raising debug asserts.
	/// </summary>
	/// <param name="systemTime">IN/OUT - The system date/time.</param>
	void ValidateSystemTime(SYSTEMTIME& systemTime);

	/// <summary>
	/// Updates the instance from an NVRAM buffer.
	/// </summary>
	/// <param name="buffer">IN - The buffer.</param>
	/// <param name="bufferSize">IN - The size of the buffer.</param>
	/// <param name="active">IN - true if the buffer location is marked as active, false otherwise.</param>
	void SetFromNvramBuffer(const BYTE *buffer, UINT bufferSize, bool active);

	/// <summary>
	/// Gets the NVRAM buffer for the instance.
	/// </summary>
	/// <param name="buffer">OUT - The buffer.</param>
	/// <param name="bufferSize">IN - The size of the buffer.</param>
	/// <param name="active">OUT - true to indicate the the record is active, false to indicate inactive.</param>
	/// <returns>The size of the buffer actually used.</returns>
	UINT GetNvramBuffer(BYTE *buffer, UINT bufferSize, bool& active) const;

	/// <summary>
	/// Gets whether there is an EGM award in progress.
	/// </summary>
	/// <returns>Returns true if there is an EGM award in progress, false otherwise.</returns>
	bool InProgress() const;

	// Members used for progressive and GCF communications.

	/// <summary>
	/// Sets the award amount paid.
	/// </summary>
	/// <param name="awardAmount">IN - The award amount in cents.</param>
	void SetAwardAmountPaid(__int64 awardAmountPaid);

	/// <summary>
	/// Gets the award amount paid.
	/// </summary>
	/// <returns>The award amount in cents.</returns>
	__int64 GetAwardAmountPaid() const;

	/// <summary>
	/// Gets the award amount. 
	/// This will return the Hit amount or the Paid amount, depending on the state of the award.
	/// </summary>
	/// <returns>The award amount in cents.</returns>
	__int64 GetAwardAmount() const;

	/// <summary>
	/// Sets the state of the award.
	/// </summary>
	/// <param name="awardState">IN - The state of the award.</param>
	void SetAwardState(EGMAwardState awardState);

	/// <summary>
	/// Gets the state of the award.
	/// </summary>
	/// <returns>The state of the award.</returns>
	EGMAwardState GetAwardState() const;

	/// <summary>
	/// Determines whether the award state is a final state or the (potentially long-duration) hand pay pending state.
	/// </summary>
	/// <returns>true if the award state is a final or hand pay pending state, false otherwise.</returns>
	bool IsAwardStateFinalOrPending() const;

	/// <summary>
	/// Gets a string that corresponds to the current award state for web diagnostics and debug logs.
	/// </summary>
	/// <returns>A string that corresponds to the current award state for web diagnostics and debug logs.</returns>
	std::string GetAwardStateString() const;

	/// <summary>
	/// Sets the original machine bonus meter value.
	/// </summary>
	/// <param name="originalMachineBonusMeterValue">IN - The original machine bonus meter value.</param>
	void SetOriginalMachineBonusMeterValue(__int64 originalMachineBonusMeterValue);

	/// <summary>
	/// Gets the original machine bonus meter value.
	/// </summary>
	/// <returns>The original machine bonus meter value.</returns>
	__int64 GetOriginalMachineBonusMeterValue() const;

	/// <summary>
	/// Sets the original attendant bonus meter value.
	/// </summary>
	/// <param name="originalAttendantBonusMeterValue">IN - The original attendant bonus meter value.</param>
	void SetOriginalAttendantBonusingMeterValue(__int64 originalAttendantBonusingMeterValue);

	/// <summary>
	/// Gets the original attendant bonus meter value.
	/// </summary>
	/// <returns>The original attendant bonus meter value.</returns>
	__int64 GetOriginalAttendantBonusingMeterValue() const;

	/// <summary>
	/// Sets the legacy machine bonus meter value.
	/// </summary>
	/// <param name="originalLegacyBonusMeterValue">IN - The original legacy bonus meter value.</param>
	void SetOriginalLegacyBonusMeterValue(__int64 originalLegacyBonusMeterValue);

	/// <summary>
	/// Gets the original legacy bonus meter value.
	/// </summary>
	/// <returns>The original legacy bonus meter value.</returns>
	__int64 GetOriginalLegacyBonusMeterValue() const;

	// Getters and setters for the member variables from the progressive protocol.

	/// <summary>
	/// Sets the level identifier.
	/// </summary>
	/// <param name="levelId">IN - The level identifier.</param>
	void SetLevelId(int levelId);

	/// <summary>
	/// Gets the level identifier.
	/// </summary>
	/// <returns>The level identifier.</returns>
	int GetLevelId() const;

	/// <summary>
	/// Sets the SAS level identifier.
	/// </summary>
	/// <param name="levelId">IN - The level identifier.</param>
	void SetSASLevelId(int levelId);

	/// <summary>
	/// Gets the SAS level identifier.
	/// </summary>
	/// <returns>The level identifier.</returns>
	int GetSASLevelId() const;

	/// <summary>
	/// Sets the standard (SAS) progressive status.
	/// </summary>
	/// <param name="isStandardProgressive">IN - true if the award is a standard (SAS) progressive, false otherwise.</param>
	void SetStandardProgressive(bool isStandardProgressive);

	/// <summary>
	/// Gets the standard (SAS) progressive status.
	/// </summary>
	/// <returns>true if the award is a standard (SAS) progressive, false otherwise.</returns>
	bool IsStandardProgressive() const;

	/// <summary>
	/// Gets the session identifier information.
	/// </summary>
	/// <param name="sessionId">OUT - The session identifier.</param>
	/// <returns>true if sessionId was set to an associated session identifier value, otherwise false to indicate no associated session identifier.</returns>
	bool GetSessionId(int &sessionId) const;

	/// <summary>
	/// Determines if the award matches the current session.
	/// </summary>
	/// <param name="sessionId">INT - The current session identifier.</param>
	/// <returns>true if award matches the current session (or all sessions), false otherwise.</returns>
	bool DoesAwardMatchSession(int sessionId) const;

	/// <summary>
	/// Sets the jackpot identifier.
	/// </summary>
	/// <param name="jackpotId">IN - The jackpot identifier.</param>
	void SetJackpotId(LONG64 jackpotId);

	/// <summary>
	/// Gets the jackpot identifier.
	/// </summary>
	/// <returns>The jackpot identifier.</returns>
	LONG64 GetJackpotId() const;

	/// <summary>
	/// Sets the remote jackpot identifier.
	/// </summary>
	/// <param name="remoteJackpotId">IN - The remote jackpot identifier.</param>
	void SetRemoteJackpotId(LONG64 remoteJackpotId);

	/// <summary>
	/// Gets the remote jackpot identifier.
	/// </summary>
	/// <returns>The remote jackpot identifier.</returns>
	LONG64 GetRemoteJackpotId() const;

	/// <summary>
	/// Gets the jackpot paid status.
	/// </summary>
	/// <returns>A JSON string representing the jackpot paid status.</returns>
	const char* GetJackpotPaidStatus() const;

	/// <summary>
	/// Sets the award amount hit from the jackpot hit message. .
	/// </summary>
	/// <param name="amount">IN - The award amount hit in cents.</param>
	void SetAwardAmountHit(__int64 awardAmountHit);

	/// <summary>
	/// Gets the award amount hit.
	/// </summary>
	/// <returns>The award amount hit in cents.</returns>
	__int64 GetAwardAmountHit() const;

	// Members used for diagnostics.

	/// <summary>
	/// Sets the sent to game count.
	/// </summary>
	/// <param name="sentToGameCount">IN - The sent to game count.</param>
	void SetSentToGameCount(int sentToGameCount);

	/// <summary>
	/// Gets the sent to game count.
	/// </summary>
	/// <returns>The sent to game count.</returns>
	int GetSentToGameCount() const;

	/// <summary>
	/// Sets the sent to game time.
	/// </summary>
	/// <param name="sentToGameTime">IN - The sent to game time.</param>
	void SetSentToGameTime(const SYSTEMTIME& sentToGameTime);

	/// <summary>
	/// Gets the sent to game time.
	/// </summary>
	/// <returns>A read-only reference to the sent to game time.</returns>
	const SYSTEMTIME& GetSentToGameTime() const;

	/// <summary>
	/// Sets the received from system time.
	/// </summary>
	/// <param name="receivedFromSystemTime">IN - The received from system time.</param>
	void SetReceivedFromSystemTime(const SYSTEMTIME& receivedFromSystemTime);

	/// <summary>
	/// Gets the received from system time.
	/// </summary>
	/// <returns>The received from system time.</returns>
	const SYSTEMTIME& GetReceivedFromSystemTime() const;

	/// <summary>
	/// Sets the received from EGM time.
	/// </summary>
	/// <param name="receivedFromEGMTime">IN - The received from EGM time.</param>
	void SetReceivedFromEGMTime(const SYSTEMTIME& receivedFromEGMTime);

	/// <summary>
	/// Gets the received from EGM time.
	/// </summary>
	/// <returns>The received from EGM time.</returns>
	const SYSTEMTIME& GetReceivedFromEGMTime() const;

	/// <summary>
	/// Sets the EGM award transfer type to be used.
	/// </summary>
	/// <param name="receivedFromSystemTime">IN - The EGM award transfer type to be used.</param>
	void SetAwardTransferType(EGMAwardTransferType awardTransferType);

	/// <summary>
	/// Gets the EGM award transfer type to be used.
	/// </summary>
	/// <returns>The EGM award transfer type to be used.</returns>
	EGMAwardTransferType GetAwardTransferType() const;

	/// <summary>
	/// Gets a string that corresponds to the current award transfer type for web diagnostics and debug logs.
	/// </summary>
	/// <returns>A string that corresponds to the current award transfer type for web diagnostics and debug logs.</returns>
	std::string GetAwardTransferTypeString() const;

	/// <summary>
	/// Sets the number of times that nCompass has received hits with the same jackpot IDs, while this was the last award received.
	/// </summary>
	/// <param name="duplicateHitCount">IN - The duplicate hit count.</param>
	void SetDuplicateHitCount(DWORD duplicateHitCount);

	/// <summary>
	/// Gets the number of times that nCompass has received hits with the same jackpot IDs, while this was the last award received.
	/// </summary>
	/// <returns>The duplicate hit count.</returns>
	DWORD GetDuplicateHitCount() const;

	/// <summary>
	/// Sets the number of times that nCompass has received hits with the different jackpot IDs, while this was the last award received.
	/// In other words, the number of times that jackpot hits for different jackpots were ignored.
	/// </summary>
	/// <param name="otherHitCount">IN - The duplicate hit count.</param>
	void SetOtherHitCount(DWORD otherHitCount);

	/// <summary>
	/// Gets the number of times that nCompass has received hits with the different jackpot IDs, while this was the last award received.
	/// In other words, the number of times that jackpot hits for different jackpots were ignored.
	/// </summary>
	/// <returns>The other hit count.</returns>
	DWORD GetOtherHitCount() const;

	/// <summary>
	/// Get a string with the award member variables formatted suitable for logging.
	/// </summary>
	/// <param name="description">IN - Reason for logging.</param>
	std::string GetLogInfoString(LPCTSTR description) const;

	/// <summary>
	/// Sets the award amount paid rounding to even credits if necessary
	/// </summary>
	/// <param name="awardAmount">IN - Denomination of current game.</param>
	/// <returns>Amount paid after rounding.</returns>
	long long RoundAndSetAwardAmountPaid(WORD denomMult);

	/// <summary>
	/// Determines whether awardAmount matches a pending jackpot award.
	/// </summary>
	/// <param name="awardAmount">The award amount.</param>
	/// <returns></returns>
	bool IsJackpotMatch(__int64 awardAmount);

	/// <summary>
	/// Log the award member variables.
	/// </summary>
	/// <param name="description">IN - Reason for logging.</param>
	/// <remarks>
	/// Prefix suggestions for "description":
	/// ">>" or "[Internals From Progressive]"   CEGMAward instance was received by Internals from the progressive protocol (System).
	/// "=="   CEGMAward instance (persisted) by Internals.
	/// "<<" or "[Internals from GCF]"   CEGMAward instance was received (or created) by Internals from the GCF (EGM/SAS protocol).
	/// [Internals to GCF]   CEGMAward instance created by Internals to send to the GCF (EGM/SAS protocol).
	/// "ss" or "[SAS]"   CEGMAward instance that is an input parameter to a CSASProtocol instance.
	/// "MM" or "[SAS Member]"   CEGMAward instance that is a member of a CSASProtocol instance.
	/// "jj" or "[Progressive from One Link]"   CEGMAward instance that was created using a jackpot hit message from Paltronics.
	/// "PP"   CEGMAward instance that is used to create a jackpot paid message to send to Paltronics.
	/// </remarks>
	void LogInfo(const std::string& description) const;

	/// <summary>
	/// Sets the type of the award paid.
	/// </summary>
	/// <param name="paidType">Type of the paid.</param>
	void SetAwardPaidType(EGMAwardPaidType paidType);

	/// <summary>
	/// Gets the type of the award paid.
	/// </summary>
	/// <returns></returns>
	EGMAwardPaidType GetAwardPaidType() const;

	/// <summary>
	/// Status info constants.  These constants are sent to One Link and are used for diagnostics.
	/// </summary>
	static const LPCTSTR TEXT_EGM_DOES_NOT_CURRENTLY_SUPPORT_TRANSFER_TYPE;
	static const LPCTSTR TEXT_HAND_PAY_PENDING_RESOLVED_BY_GAME_START;
	static const LPCTSTR TEXT_INTERNALS_CREATED_EGM_AWARD_FROM_BONUS_METER_CHANGE;
	static const LPCTSTR TEXT_INTERNALS_DECLINED_NEW_JACKPOT_HIT_FROM_SYSTEM__AWARD_IN_PROGRESS;
	static const LPCTSTR TEXT_INTERNALS_DECLINED_NEW_JACKPOT_HIT_FROM_SYSTEM__NO_TRANSFER_CAPABILITY;
	static const LPCTSTR TEXT_INTERNALS_EXPIRED_EGM_AWARD__SENDING_TO_GCF_TO_BE_CLEARED_THERE_FIRST;
	static const LPCTSTR TEXT_INTERNALS_INITIALIZED_INVALID_AWARD;
	static const LPCTSTR TEXT_INTERNALS_FAILED_NEW_PROMO_AWARD_FROM_SYSTEM_AWARD_IN_PROGRESS;
	static const LPCTSTR TEXT_INTERNALS_RECEIVED_INVALID_JACKPOT_HIT;
	static const LPCTSTR TEXT_INTERNALS_RECEIVED_NEW_JACKPOT_HIT_FROM_SYSTEM_WHILE_PROGRESSIVE_DOWN;
	static const LPCTSTR TEXT_RESTART_OCCURRED_WITH_LEGACY_BONUSING_TRANSFER;
	static const LPCTSTR TEXT_SAS_AFT_BONUS_AWARD_AMOUNT_INVALID;
	static const LPCTSTR TEXT_SAS_AFT_ERROR_GETTING_BONUS_TRANSFER_STATUS;
	static const LPCTSTR TEXT_SAS_AFT_ERROR_TIMED_OUT_WAITING_FOR_LOCK;
	static const LPCTSTR TEXT_SAS_DENOM_CHECK_FAILED;
	static const LPCTSTR TEXT_SAS_DENOM_MISMATCH_ERROR;
	static const LPCTSTR TEXT_SAS_LEGACY_BONUS_AWARD_AMOUNT_INVALID;
	static const LPCTSTR TEXT_SAS_LEGACY_BONUS_POLL_FAILED;
	static const LPCTSTR TEXT_SAS_LEGACY_BONUS_SIGNALED;
	static const LPCTSTR TEXT_SAS_MACHINE_INFO_LONG_POLL_ERROR__DENOM_CHECK_FAILED;
	static const LPCTSTR TEXT_SAS_NO_BONUSING_IS_AVAILABLE_ON_THIS_EGM;
	static const LPCTSTR TEXT_SETTING_AWARD_AS_HANDPAY_PENDING_AFTER_A_JACKPOT_IS_SET;

	/// <summary>
	/// Status info constants.  These constants are sent to One Link and are used for diagnostics.
	/// AFT results (in numeric order).
	/// </summary>
	static const LPCTSTR TEXT_AFT_TRANSACTION_SUCCESSFUL;
	static const LPCTSTR TEXT_AFT_TRANSFER_STATUS_PENDING_IN_GETAWARDRESULTFROMTRANSFERSTATUS;
	static const LPCTSTR TEXT_AFT_TRANSFER_CANCELED_BY_HOST;
	static const LPCTSTR TEXT_AFT_TRANSACTION_ID_NOT_UNIQUE__SUCCESS_AFTER_RESTART;
	static const LPCTSTR TEXT_AFT_NOT_A_VALID_TRANSFER_FUNCTION;
	static const LPCTSTR TEXT_AFT_NOT_A_VALID_TRANSFER_AMOUNT_OR_EXPIRATION;
	static const LPCTSTR TEXT_AFT_TRANSFER_AMOUNT_EXCEEDS_LIMITS;
	static const LPCTSTR TEXT_AFT_TRANSFER_AMOUNT_NOT_EVEN_MULTIPLE_OF_DENOM;
	static const LPCTSTR TEXT_AFT_CANNOT_PERFORM_PARTIAL_TRANSFERS;
	static const LPCTSTR TEXT_AFT_MACHINE_NOT_REGISTERED;
	static const LPCTSTR TEXT_AFT_REGISTRATION_KEY_DOES_NOT_MATCH;
	static const LPCTSTR TEXT_AFT_NO_POS_ID;
	static const LPCTSTR TEXT_AFT_NO_WON_CREDITS_FOR_CASHOUT;
	static const LPCTSTR TEXT_AFT_NO_DENOM_SET;
	static const LPCTSTR TEXT_AFT_EXPIRATION_NOT_VALID;
	static const LPCTSTR TEXT_AFT_TICKET_DEVICE_NOT_AVAILABLE;
	static const LPCTSTR TEXT_AFT_RESTRICTED_AMOUNTS_FROM_DIFFERENT_POOLS;
	static const LPCTSTR TEXT_AFT_UNABLE_TO_PRINT_TRANSACTION_RECEIPT_DEVICE_UNAVAILABLE;
	static const LPCTSTR TEXT_AFT_INSUFFICIENT_DATA_TO_PRINT_TRANSACTION_RECEIPT;
	static const LPCTSTR TEXT_AFT_TRANSACTION_RECEIPT_NOT_ALLOWED_FOR_TRANSFER_TYPE;
	static const LPCTSTR TEXT_AFT_ASSET_NUMBER_0_OR_DOES_NOT_MATCH;
	static const LPCTSTR TEXT_AFT_TRANSACTION_ID_NOT_VALID;
	static const LPCTSTR TEXT_AFT_UNEXPECTED_ERROR;
	static const LPCTSTR TEXT_AFT_NOT_COMPATIBLE_WITH_CURRENT_TRANSFER_IN_PROGRESS;
	static const LPCTSTR TEXT_AFT_UNSUPPORTED_TRANSFER_CODE;
	static const LPCTSTR TEXT_AFT_NO_TRANSFER_INFO_AVAILABLE;
	static const LPCTSTR TEXT_AFT_UNKNOWN_ERROR;

	/// <summary>
	/// Status info formatting constants.  After formatting, these constants are sent to One Link and are used for diagnostics.
	/// </summary>
	static const LPCTSTR FORMAT_INT__EGM_AWARD_DOES_NOT_MATCH_CURRENT_SESSION_ID;

	/// <summary>
	/// Sets the status information.
	/// </summary>
	/// <param name="statusInfo">IN - Status information.</param>
	/// <remarks>
	/// Not persisted.
	/// If statusInfo is a nullptr or has a length of zero then the existing value is retained.
	/// </remarks>
	void SetStatusInfo(const std::string& statusInfo);

	/// <summary>
	/// Gets the status information string.  It may be an empty string.
	/// </summary>
	/// <returns>The status information string.</returns>
	std::string GetStatusInfo() const;

	/// <summary>
	/// Sets the hit received.
	/// </summary>
	/// <param name="">if set to <c>true</c> [].</param>
	void SetHitReceived(bool);

	/// <summary>
	/// Determines whether [is hit received].
	/// </summary>
	/// <returns>true if a Hit has been receive from progressive system</returns>
	bool isHitReceived() const;

	/// <summary>
	/// Sets the handpay pending signaled.
	/// </summary>
	/// <param name="">if set to <c>true</c> [].</param>
	void setHandpayPendingSignaled(bool);

	/// <summary>
	/// Determines whether [is handpay pending signaled].
	/// </summary>
	/// <returns>true if handpay is pending and has been signaled</returns>
	bool isHandpayPendingSignaled() const;

	/// <summary>
	/// Sets the egm paid signaled.
	/// </summary>
	/// <param name="">if set to <c>true</c> [].</param>
	void setEgmPaidSignaled(bool);

	/// <summary>
	/// Determines whether [is egm paid signaled].
	/// </summary>
	/// <returns>true if the egm has been paid</returns>
	bool isEgmPaidSignaled() const;

	/// <summary>
	/// Sets the method of payment for bonus awards initiated by One Link.
	/// </summary>
	/// <param name="">IN - The method of payment for bonus awards initiated by One Link.</param>
	/// <returns></returns>
	void SetMethodOfPayment(MethodOfPayment::Type methodOfPayment);

	/// <summary>
	/// Gets the method of payment for bonus awards initiated by One Link.
	/// </summary>
	/// <returns>The method of payment for bonus awards initiated by One Link.</returns>
	MethodOfPayment::Type GetMethodOfPayment() const;


	/// <summary>
	/// Marks the instance as not active.
	/// </summary>
	void ClearActive();

	/// <summary>
	/// Gets whether the instance is active (especially with regard to standard SAS progressive wins).
	/// </summary>
	/// <returns>false when the instance is no longer useful except for diagnostics, true generally indicates otherwise.</returns>
	bool IsActive() const;

	/// <summary>
	/// Gets jackpot award type.
	/// </summary>
	/// <returns>Award type</returns>
	AwardType::Type GetAwardType() const;

	/// <summary>
	/// Sets the from promo game flag
	/// </summary>
	/// <param name="value">IN - From the promo game flag.</param>
	void SetFromPromoGame(bool value);

	/// <summary>
	/// Checks whether this award is generated from a promo game.
	/// </summary>
	/// <returns>True if from a promo game, false otherwise.</returns>
	bool IsFromPromoGame() const;

	/// <summary>
	/// Sets the fields for the OneLink award hit message
	/// </summary>
	/// <param name="msg">IN - message that holds all the fields for OneLink award hit message.</param>
	void SetFieldsForOneLinkAwardHit(CInternalMessage *msg);

	/// <summary>
	/// Sets the fields for the OneLink award status message
	/// </summary>
	/// <param name="msg">IN - message that holds all the fields for OneLink award hit message.</param>
	/// <param name="awardType">IN - award type.</param>
	/// <param name="awardState">IN - award state.</param>
	/// <param name="methodPayment">IN - method of payment.</param>
	/// <param name="jackpotId">IN - jackpot id.</param>
	/// <param name="amount">IN - award type.</param>
	/// <param name="fromWinnerPicked">IN - flag that indicates whether if this status comes from the winner picked message.</param>
	/// <param name="clearedByOneLink">IN - flag that indicates whether if the prize award was cleared by OneLink> </param>
	static bool SetFieldsForOneLinkAwardStatus(CInternalMessage *msg, AwardType::Type awardType, EGMAwardState awardState, MethodOfPayment::Type methodPayment, int64_t jackpotId, int64_t amount, bool fromWinnerPicked, bool clearedByOneLink);

	/// <summary>
	/// Gets the fields for the OneLink transaction message
	/// </summary>
	/// <param name="msg">IN - message that holds all the fields for OneLink transaction message.</param>
	/// <param name="transCode">IN - transaction code (ONE_LINK_AWARD_HIT or ONE_LINK_AWARD_STATUS).</param>
	/// <param name="sessionPoints">IN - session points.</param>
	/// <param name="bonusPoints">IN - bonus points.</param>
	/// <param name="residualValue">IN - residual value.</param>
	static void GetFieldsForOneLinkTransactionMessage(CInternalMessage* msg, byte& transCode, int64_t& sessionPoints, int64_t& bonusPoints, int64_t& residualValue);

private:
	static const DWORD NVRAM_DATA_OFFSET = NVRAM_EGM_AWARD_DATA_OFFSET; // For Bonus awards, not Standard hits.
	static const DWORD NVRAM_DATA_SIZE = NVRAM_EGM_AWARD_DATA_SIZE;
	static const DWORD NVRAM_ACTIVE_OFFSET = NVRAM_EGM_AWARD_CHECKBYTE_OFFSET;
	static const DWORD NVRAM_ACTIVE_VALUE = NVRAM_EGM_AWARD_CHECKBYTE_VALUE;

	static const LONG64 DEFAULT_JACKPOT_ID = INVALID_JACKPOT_ID;
	static const LONG64 DEFAULT_REMOTE_JACKPOT_ID = -1;
	static const AwardType::Type DEFAULT_AWARD_TYPE = AwardType::Invalid;

	// Members used for progressive and GCF communications.

	/// <summary>
	/// The award amount paid to the EGM in cents.
	/// </summary>
	__int64 m_awardAmountPaid;

	/// <summary>
	/// The award state. Set to ReceivedFromSystem when an instance is created.
	/// </summary>
	EGMAwardState m_awardState;

	/// <summary>
	/// The level identifier from the jackpot hit message.
	/// </summary>
	int m_levelId;

	/// <summary>
	/// The Sas level identifier from the egm for a standard progressive
	/// </summary>
	int m_sasLevelId;

	/// <summary>
	/// The jackpot identifier from the jackpot hit message.
	/// </summary>
	LONG64 m_jackpotId;

	/// <summary>
	/// The remote jackpot identifier from the jackpot hit message.
	/// </summary>
	LONG64 m_remoteJackpotId;

	/// <summary>
	/// The award amount in cents from the jackpot hit message.
	/// </summary>
	__int64 m_awardAmountHit;

	/// <summary>
	/// The original machine bonus meter value, from when m_sentToGameCount == 0.
	/// </summary>
	__int64 m_originalMachineBonusMeterValue;

	/// <summary>
	/// The original attendant bonus meter value, from when m_sentToGameCount == 0
	/// </summary>
	__int64 m_originalAttendantBonusingMeterValue;

	/// <summary>
	/// The original legacy bonus meter value, from when m_sentToGameCount == 0
	/// Only use this if the machine and attendant bonus meters are not available.
	/// </summary>
	__int64 m_originalLegacyBonusMeterValue;

	/// <summary>
	/// Indicates the transfer type to be used for transfer attempts.
	/// This is used to ensure that consistent functionality is used to resolve transfer attempts after a restart.
	/// </summary>
	EGMAwardTransferType m_awardTransferType;

	/// <summary>
	/// true if the hit is a standard (SAS) progressive hit, false otherwise. Not persisted as of 2015-05-01.
	/// </summary>
	bool m_isStandardProgressive;

	/// <summary>
	/// The associated session identifier, if m_sessionId.isValid is true.
	/// </summary>
	/// <remarks>
	/// If m_sessionId.isValid is false then the session identifier is valid and matches all sessions.
	/// </remarks>
	OptionalVal<int> m_sessionId;

	// Members used for diagnostics.

	/// <summary>
	/// The number of times that nCompass has tried to sent the amount to the EGM.
	/// </summary>
	int m_sentToGameCount;

	/// <summary>
	/// The last time that m_sentToGameCount was incremented. Not valid if m_sentToGameCount == 0.
	/// </summary>
	SYSTEMTIME m_sentToGameTime;

	/// <summary>
	/// The time that the award was received from the progressive system. Not valid if m_awardState == NoAward.
	/// </summary>
	SYSTEMTIME m_receivedFromSystemTime;

	/// <summary>
	/// The time that the award was received from the EGM. Only valid when using Standard Progressives.
	/// </summary>
	SYSTEMTIME m_receivedFromEGMTime;

	/// <summary>
	/// The number of times that nCompass has received hits with the same jackpot IDs, while this was the last award received.
	/// </summary>
	DWORD m_duplicateHitCount;

	/// <summary>
	/// The number of times that nCompass has received hits with the different jackpot IDs, while this was the last award received.
	/// In other words, the number of times that jackpot hits for different jackpots were ignored.
	/// </summary>
	DWORD m_otherHitCount;

	/// <summary>
	/// Specifies how the award was paid.
	/// </summary>
	EGMAwardPaidType m_paidType;

	/// <summary>
	/// Provides optional info about award status -- for JackpotPaid messages to One Link.
	/// </summary>
	std::string m_statusInfo;

	/// <summary>
	/// Set to true when a hit is returned from progressive system
	/// </summary>
	bool m_isHitReceived;

	/// <summary>
	/// Set to true when a handpay is pending
	/// </summary>
	bool m_isHandpayPendingSignaled;

	/// <summary>
	/// Set to true when an Egm pays a prog, via 0x52 or coin in.
	/// </summary>
	bool m_isEgmPaidSignaled;

	/// <summary>
	/// Indicates the method of payment for bonus awards initiated by One Link.
	/// </summary>
	MethodOfPayment::Type m_methodOfPayment;

	/// <summary>
	/// Set to false when the instance is no longer useful except for diagnostics, true generally indicates otherwise.
	/// </summary>
	bool m_active;

	/// <summary>
	/// Award type
	/// </summary>
	AwardType::Type m_awardType;

	/// <summary>
	/// Set to true if this award is a result of a promo game, false otherwise.
	/// </summary>
	bool m_fromPromoGame;

	// Grant "friend" access for testing.
	friend class SentinelNativeUnitTests::EGMAward;
	friend class SentinelNativeUnitTests::EGMAwardManager;
};
