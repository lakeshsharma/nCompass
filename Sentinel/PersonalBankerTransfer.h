#pragma once

#include "Hardware/MemoryStorage.h"
#include "GlobalDefs.h"
#include "InternalMsgContent.h"
#include "Logging/LogString.h"


const int NUM_CANCELLATION_ERRORS = 45;
const int EFT_ID_LENGTH = 3;
extern const LPCTSTR PB_CANCELLATION_TOKEN[NUM_CANCELLATION_ERRORS];
extern const LPCTSTR PB_CANCELLATION_CODES[NUM_CANCELLATION_ERRORS];

typedef enum
{
	_Unknown = 0,
	NoGameDenomination = 1,
	DoorOpen = 2,
	UnderTransferLimit = 3,
	OverTransferLimit = 4,
	SASInvalidTransactionNum = 5,
	NonCreditMode = 6,
	MachineTilt = 7,
	SASInvalidAckMessage = 8,
	GameInProgress = 9,
	SASNonBCDDataError = 10,
	SASDuplicateTransferNum = 11,
	MachineDisabledViaSAS = 12,
	MachineOutOfService = 13,
	SASMachineBusy = 14,
	Unknown15 = 15,
	SASTimeout = 16,
	BartGameDenomMismatch = 17,
	LockReject = 18,
	CanceledBySystem = 19, // Can be used when nCompass cancels with authorization is in process.
	PartialTransfer = 20,
	GameNotRegistered = 21,
	MismatchedRegistrationNum = 22,
	NoCredits = 23,
	TicketReceiptError = 24,
	GameNotLocked = 25,
	GameAssetNumError = 26,
	InvalidTransCode = 27,
	AmountNotEvenMult = 28,
	NoPosID = 29,
	RestrictedPoolIDMismatch= 30,
	GeneralError = 31,
	InvalidTransID = 32,
	GameCommDown = 33,
	AllowableRangeExceeded = 34,
	MismatchTransID = 35,
	IncompatiblePoll = 36,
	UnsupportedTransCode = 37,
	TransInfoUnavailable = 38,
	AutomaticUploadCanceledBySystem = 39,
	AutomaticUploadCanceledByUser = 40,
	CreditTransferErrorInsufficientCredits = 41,
	CreditTransferErrorComms = 42,
	CreditTransferErrorOther = 43,
	CreditTransferErrorUnknown = 44,
	CreditTransferErrorInternalToPoller1 = 45,
	CreditTransferErrorInternalToPoller2 = 46,
	CreditTransferErrorTimeout = 47,
}PollerCancellationType;

static const std::string PollerCancellationTypeStrings[] =
{
	"Unknown",
	"NoGameDenomination",
	"DoorOpen",
	"UnderTransferLimit",
	"OverTransferLimit",
	"SASInvalidTransactionNum",
	"NonCreditMode",
	"MachineTilt",
	"SASInvalidAckMessage",
	"GameInProgress",
	"SASNonBCDDataError",
	"SASDuplicateTransferNum",
	"MachineDisabledViaSAS",
	"MachineOutOfService",
	"SASMachineBusy",
	"Unknown15",
	"SASTimeout",
	"BartGameDenomMismatch",
	"LockReject",
	"CanceledBySystem",
	"PartialTransfer",
	"GameNotRegistered",
	"MismatchedRegistrationNum",
	"NoCredits",
	"TicketReceiptError",
	"GameNotLocked",
	"GameAssetNumError",
	"InvalidTransCode",
	"AmountNotEvenMult",
	"NoPosID",
	"RestrictedPoolIDMismatch",
	"GeneralError",
	"InvalidTransID",
	"GameCommDown",
	"AllowableRangeExceeded",
	"MismatchTransID",
	"IncompatiblePoll",
	"UnsupportedTransCode",
	"TransInfoUnavailable",
	"AutomaticUploadCanceledBySystem",
	"AutomaticUploadCanceledByUser",
	"CreditTransferErrorInsufficientCredits",
	"CreditTransferErrorComms",
	"CreditTransferErrorOther",
	"CreditTransferErrorUnknown",
	"CreditTransferErrorInternalToPoller1",
	"CreditTransferErrorInternalToPoller2",
	"CreditTransferErrorTimeout"
};


typedef enum
{
	Unspecified                             = 0, //Default when created
	SessionRequest                          = 1,
	CashDownload                            = 2,
	PromoCreditsToGame                      = 3, //Not Currently Used
	PromoCreditsToCash                      = 4,
	PointRedemptionToCash                   = 5,
	CashUpload                              = 6,
	TransferCreditsFromGame                 = 7, //Not Currently Used
	SessionRelease                          = 8,
 	ChangePinAtSentinel                     = 9, //Not Currently Used
	PointRedemptionToPromoCredits           = 10,
	PointCashUpload                         = 11,
	PromoCreditsUpload						= 12,
	PromoCreditsUploadAuto					= 13,
	CreditTransfer							= 14,
	PartialCashUpload                       = 15,
	PartialPointCashUpload                  = 16,
	CashUploadAuto							= 17,
	PointCashUploadAuto						= 18,
	CashUploadCashout						= 19,
	PointCashUploadCashout					= 20,
	PromoCreditsUploadCashout				= 21,
	TaxWithheld								= 22,
	PendingResponse							= 50,
	CanceledSessionRequest                  = 101, //Should never happen because curently we don't cancel session requests.
	CanceledCashDownload                    = 102,
	CanceledPromoCreditsToGame              = 103, //Should not happen because 3 is not supported.
	CanceledPromoCreditsToCash              = 104,
	CanceledPointRedemption                 = 105,
	CanceledCashUpload                      = 106,
	CanceledTransferCreditsFromGame         = 107, //Should not happen because 7 is not supported.
	CanceledSessionRelease                  = 108, //Should never happen because curently we don't cancel session release.
	CanceledChangePinAtSentinel             = 109, //Should not happen because 9 is not supported.
	CanceledPointRedemptionToPromoCredits   = 110,
	CanceledPointCashUpload                 = 111,
	CanceledPromoCreditsUpload				= 112,
	CanceledPromoCreditsUploadAuto			= 113,
	CanceledCreditTransfer                  = 114,
	CancelExternalCreditsToGame				= 115,//CancelExternalCredit
	CanceledPartialCashUpload               = 116,
	CanceledPartialPointCashUpload          = 117,
	CanceledCashUploadAuto					= 118,
	CanceledPointCashUploadAuto				= 119,
	CanceledTaxWithheld						= 120,
	AuthorizeCashToGame						= 121,
	AuthorizePromoToGame					= 122,
	AuthorizePointsToGameCash				= 123,
	AuthorizePointsToGamePromo				= 124,
	AuthorizeExternalCreditsToGame			= 125, //RCCL external credit to games
	CanceledCashUploadCashout				= 126,
	CanceledPointCashUploadCashout			= 127,
	CanceledPromoCreditsUploadCashout		= 128,

	/* Special Codes */
	UnknownCreditSource                     = 198, //Internal system error @ the sentinel
	InvalidTransIDFromSystem                = 199,
	NonMatchingTransactionID				= 200,
	UnexpectedRestrictedToGame              = 210,
	UnexpectedUnRestrictedToGame            = 211,
	UnexpectedCashToGame                    = 212,
	UnexpectedRestrictedFromGame            = 220,
	UnexpectedUnRestrictedFromGame          = 221,
	UnexpectedCashFromGame                  = 222,
	UnexpectedRestrictedToGameErr			= 230,
	UnexpectedUnrestrictedToGameErr			= 231,
	UnexpectedCashToGameErr					= 240,
	UnexpectedUnrestrictedFromGameErr		= 241,
	UnexpectedCashFromGameErr				= 242,
	UnknownError                            = 255,
} PersonalBankerType;

typedef enum
{
	EFTSuccess = 0x00,
	EFTErrorDoorOpen = 0x02,
	EFTErorrOverTransferLimit = 0x04,
	EFTErrorSASInvalidTransactionNm = 0x05,
	EFTErrrorNonCreditMode = 0x06,
	EFTErrorMachineTilt = 0x07,
	EFTErrorSASInvalidAckMessage = 0x08,
	EFTErrorGameInProgress =  0x09,
	EFTErrorNonBCDDataError =  0x0a,
	EFTErrorSASDuplicateTransferNum = 0x0b,
	EFTErrorMachineDisabledViaSAS = 0x0c,
	EFTErrorMachineOutOfService = 0x0d,
	EFTErrorSASMachineBusy = 0x0e,
	EFTErrorUnderTransferLimit = 0xfd,
	EFTErrorSASTimeout = 0xfe,
	EFTErrorGameCommDown = 0xff,
} EFTStatusType;

typedef enum
{
	AFTErrorFullTransferSuccessful = 0x00,
	AFTErrorParitalTransferSuccessful = 0x01,
	AFTErrorTransferPending = 0x40,

	// Real errors start here
	AFTErrorCanceledBySystem = 0x80,
	AFTErrorSASDuplicateTransferNum = 0x81,
	AFTErrorUnsupportedTransCode = 0x82,
	AFTErrorAllowableRangeExceeded = 0x83,
	AFTErrorOverTransferLimit = 0x84,
	AFTErrorAmountNotEvenMult = 0x85,
	AFTErrorGeneralError = 0x86,
	AFTErrorMachineTilt = 0x87,
	AFTErrorGameNotRegistered = 0x88,
	AFTErrorRegistrationKeyNotMatched = 0x89,
	AFTErrorNoPosID = 0x8A,
	AFTErrorNoCredits = 0x8B,
	AFTErrorBartGameDenomMismatch = 0x8C,
	AFTErrorTicketAlreadyExpired = 0x8D,
	AFTErrorTicketDeviceNotAvailable = 0x8E,
	AFTErrorRestrictedPoolIDMismatch = 0x8F,
	AFTErrorReceitDeviceNotAvailable = 0x90,
	AFTErrorInsuficientReceiptData = 0x91,
	AFTErrorReceiptNotAllowed = 0x92,
	AFTErrorAssetNumberNoMatch = 0x93,
	AFTErrorGameNotLocked = 0x94,
	AFTErrorInvalidTransID = 0x95,
	AFTErrorUnexpectedError = 0x9F,
	AFTErrorNotCompatable = 0xC0,
	AFTErrorUsupportedTransfercode = 0xC1,
	AFTErrorNoTransferInfoAvailable = 0xFF,
} AFTSTatustype;

typedef enum 
{
	Undeclared = 0,
	CashToGame = 1,
	PointsToGame = 2,
	PromoDollarsToGame = 3,
	CashFromGame = 4,
	PointsFromGame = 5,
	PromoDollarsFromGame = 6,
	ExternalCredit = 7,
	AllFromGame = 8,
} PersonalBankerTransferType;

typedef enum
{
	PBStateUnknown,
	PBStateInquireSent,
	PBStateInfoReceived,
	PBStateTransferingFromToGame,
	PBStateTransferComplete,
	PBStateReleaseSent,
	PBStateWaitingForUploadToStart,
	PBStateIdle_NotUsed,
	PBStateSessionRequested_NotUsed,
	PBStateAuthRequested_NotUsed,
	PBStateAuthGranted_NotUsed,
	PBStateAuthDenied,
	PBStateSendingTransferTransaction_NotUsed,
	PBStateTransferTransactionSent_NotUsed,
	PBStateSendingRelease_NotUsed,
	NUMBER_OF_PB_STATES
} PersonalBankerState;

extern const char* PersonalBankerStateStr[];

enum class PinResetResult
{
	PinResetSuccess,//PIN has been successfully reset,
	PinResetErrorSequence,//PIN cannot be a repeated number(1111) or a sequence of numbers(1234).
	PinResetErrorBirthday,//PIN cannot match player's birthday.
	PinResetErrorUnknown//Error resetting PIN
};


const DWORD NVRAM_PB_TRANSFER_TYPES_SIZE = 16;	// No room to add stuff.
const DWORD NVRAM_PB_TRANSFER_IDS_SIZE = 32;	// Room to add stuff.
const DWORD NVRAM_PB_TRANSFER_START_SIZE = 48;	// Room to add stuff.
const DWORD NVRAM_PB_TRANSFER_UPDATED_SIZE = 52;	// No Room to add stuff.

const DWORD NVRAM_PB_TRANSFER_TYPES_OFFSET =	NVRAM_PB_TRANSFER_OFFSET + NVRAM_PB_TRANSFER_CHECKBYTE_SIZE;
const DWORD NVRAM_PB_TRANSFER_IDS_OFFSET =		NVRAM_PB_TRANSFER_TYPES_OFFSET + NVRAM_PB_TRANSFER_TYPES_SIZE;
const DWORD NVRAM_PB_TRANSFER_START_OFFSET =	NVRAM_PB_TRANSFER_IDS_OFFSET + NVRAM_PB_TRANSFER_IDS_SIZE;
const DWORD NVRAM_PB_TRANSFER_UPDATED_OFFSET =	NVRAM_PB_TRANSFER_START_OFFSET + NVRAM_PB_TRANSFER_START_SIZE;
const DWORD NVRAM_PB_TRANSFER_CARDTIME_OFFSET =	NVRAM_PB_TRANSFER_UPDATED_OFFSET + NVRAM_PB_TRANSFER_UPDATED_SIZE;

union u_PersonalBankerTransferFlags
{
	// first representation (member of union) 
	struct tPersonalBankerTransferFlags
	{
		WORD m_isPartialWatOut		 :  1;//Bit0
		WORD m_isPartialCEPOut		 :  1;//Bit1
		WORD m_isNcepAutoUpload		 :  1;//Bit2
		WORD m_isCepAutoUpload		 :  1;//Bit3
		WORD m_isWatAutoUpload		 :  1;//Bit4
		WORD m_isProcessTaxDeduction :  1;//Bit5		
		WORD m_federalTaxOverridden  :  1;//Bit6
		WORD m_stateTaxOverridden    :  1;//Bit7
		WORD m_spare		         :  8;//Bit8-15
	} m_flags;

	// second representation (member of union) 
	WORD m_uValue;

	u_PersonalBankerTransferFlags()
	{
		m_uValue = 0;
	}

	bool IsPartialWATOutTransfer() const
	{
		return m_flags.m_isPartialWatOut;
	}

	bool IsPartialCEPOutTransfer() const
	{
		return m_flags.m_isPartialCEPOut;
	}

	bool IsNcepAutoUpload() const
	{
		return m_flags.m_isNcepAutoUpload;
	}

	bool IsCepAutoUpload() const
	{
		return m_flags.m_isCepAutoUpload;
	}

	bool IsWatAutoUpload() const
	{
		return m_flags.m_isWatAutoUpload;
	}

	bool IsProcessTaxDeduction() const
	{
		return m_flags.m_isProcessTaxDeduction;
	}

	bool IsFederalTaxOverridden() const
	{
		return m_flags.m_federalTaxOverridden;
	}

	bool IsStateTaxOverridden() const
	{
		return m_flags.m_stateTaxOverridden;
	}
};

class CPersonalBankerTransfer:
	public CMemoryStorage, public CInternalMsgContent<INT_MSG_CONTENT_PERSONALBANKERTRANSFER>
{
public:
// #pragma region constructors
	/// <summary>
	/// Standard memory type constructor.
	/// </summary>
	/// <param name="tickCount">IN - The current tick count, generally from ::GetTickCount().</param>
	/// <param name="memoryType">IN - Specifies the persisted memory type for the instance.</param>
	/// <param name="bBuildFromNVRAM">IN - If memoryType == MEMORY_NVRAM then true indicates build from NVRAM and false indicates store to NVRAM.
	/// No effect if memoryType != MEMORY_NVRAM.</param>
	/// <remarks>
	/// This constructor does not check if the NVRAM location is active.
	/// </remarks>
	CPersonalBankerTransfer(DWORD tickCount, MEMORY_TYPE memoryType = MEMORY_NONE, bool bBuildFromNVRAM = false);

	/// <summary>
	/// Copy constructor.
	/// </summary>
	/// <param name="memoryType">IN/OUT - instance to copy from.</param>
	CPersonalBankerTransfer(const CPersonalBankerTransfer &rhs);
// #pragma endregion constructors

// #pragma region destructor
	/// <summary>
	/// Destructor.
	/// </summary>
	~CPersonalBankerTransfer(void);
// #pragma endregion destructor

// #pragma region operators
	/// <summary>
	/// Assignment operator.
	/// </summary>
	/// <param name="rhs">IN/OUT - instance to assign from.</param>
	/// <returns>A reference to the left hand side (lhs).</returns>
	/// <remarks>
	/// 2013-06-30 m_sessionStartTickCount is now assigned from rhs.
	/// </remarks>
	CPersonalBankerTransfer &operator=( const CPersonalBankerTransfer &rhs );

	/// <summary>
	/// Equals operator.
	/// </summary>
	/// <param name="rhs">IN/OUT - right hand side for comparison.</param>
	/// <returns>true if instances are equal, false otherwise.</returns>
	bool operator==( const CPersonalBankerTransfer &rhs );
// #pragma endregion operators

	static const char* GetStateString(PersonalBankerState state);

	static bool DoesPersonalBankerTransferExistInNVRAM( void );
	static bool SetInactiveLocation();

	/// <summary>
	/// If m_type == MEMORY_NVRAM then try to update the instance from NVRAM, else do nothing.
	/// </summary>
	/// <remarks>
	/// m_sessionStartTickCount is not updated, regardless of m_type.
	/// This method does not check if the NVRAM location is active.
	/// </remarks>
	virtual void BuildYourself();

	/// <summary>
	/// Determines whether PB ID is a valid ID.
	/// </summary>
	/// <param name="slotmast_ID">The slotmast_ID.</param>
	/// <param name="pbtId">The PBT identifier.</param>
	/// <param name="pbtIdLength">Length of the PBT identifier.</param>
	/// <returns>
	/// True if PB ID is valid.
	/// </returns>
	static bool IsValidID(long slotmast_ID, const char *pbtId, int pbtIdLength);

	/// <summary>
	/// Logs current PBT transfer information.
	/// </summary>
	///	<param name="descriptionFormat">IN - Description of the condition being logged and
	/// format specification for any additional arguments.</param>
	void LogPBT(LPCTSTR descriptionFormat, ...) const;

	/// <summary>
	/// Logs PBT transfer information.
	/// </summary>
	/// <param name="pbt">IN - Pointer to CPersonalBankerTransfer to log.  MAY be nullptr.</param>
	///	<param name="descriptionFormat">IN - Description of the condition being logged and
	/// format specification for any additional arguments.</param>
	static void LogPBT(const CPersonalBankerTransfer *pbt, LPCTSTR descriptionFormat, ...);

	///	<summary>
	/// Logs PBT transfer information.
	///	</summary>
	/// <param name="pbt">IN - Pointer to CPersonalBankerTransfer to log.  MAY be nullptr.</param>
	///	<param name="descriptionFormat">IN - Description of the condition being logged and
	/// format specification for any additional arguments.</param>
	///	<param name="args">IN/OUT - Pointer to list of arguments.</param>
	static void LogPBTV(const CPersonalBankerTransfer *pbt, LPCTSTR descriptionFormat, va_list args);

	void SetID(const char* ID, int length)
	{
		memcpy_s(m_pbtID, MAX_PBID_LENGTH, ID, length);
		m_pbtIDLength = length;
		StoreIDDataToNVRAM();
	}

	const char* GetID() const
	{
		return m_pbtID;
	}

	int GetPBTIDLength() const
	{
		return m_pbtIDLength;
	}

	void SetUTCCardInTime(time_t inUTCTime)
	{
		m_utcCardInTime = inUTCTime;
		StoreCardTimeToNVRAM();
	}

	time_t GetUTCCardInTime() const
	{
		return m_utcCardInTime;
	}

	DWORD GetSecondsSinceCardIn();

	void SetOriginalCreditMeter(__int64 meterValue)
	{
		m_originalCreditMeter = meterValue;
		StoreUpdatedDataToNVRAM();
	}

	__int64 GetOriginalCreditMeter() const
	{
		return m_originalCreditMeter;
	}

	void SetCancelledType(PollerCancellationType type)
	{
		m_cancelledType = type;
		if (type != _Unknown)
			ChangePersonalBankerTypeToCanceled();
		StoreTypesDataToNVRAM();
	}

	PollerCancellationType GetCancelledType() const
	{
		return m_cancelledType;
	}

	void SetPromoCancelledType(PollerCancellationType type)
	{
		m_promoCancelledType = type;
		m_cancelledType = type;
		if (type != _Unknown)
			ChangePersonalBankerPromoTypeToCanceled();
		StoreUpdatedDataToNVRAM();
	}

	PollerCancellationType GetPromoCancelledType() const
	{
		return m_promoCancelledType;
	}

	void SetPersonalBankerType(PersonalBankerType type)
	{
		m_pbtType = type;
		StoreTypesDataToNVRAM();
	}

	/// <summary>
	/// Returns the PersonalBanker type for the transfer.
	/// </summary>
	/// <returns>the PersonalBanker type for the transfer.</returns>
	PersonalBankerType GetPersonalBankerType() const
	{
		return m_pbtType;
	}

	void SetPromoPersonalBankerType(PersonalBankerType type)
	{
		m_promoPbtType = type;
		StoreUpdatedDataToNVRAM();
	}
	PersonalBankerType GetPromoPersonalBankerType() const
	{
		return m_promoPbtType;
	}

	void SetPersonalBankerTransferType(PersonalBankerTransferType type);

	/// <summary>
	/// Returns the PersonalBanker transfer type.
	/// </summary>
	/// <returns>The PersonalBanker transfer type.</returns>
	PersonalBankerTransferType GetPersonalBankerTransferType() const
	{
		return m_pbtTransferType;
	}

	void SetPromoPersonalBankerTransferType(PersonalBankerTransferType type);
	PersonalBankerTransferType GetPromoPersonalBankerTransferType() const
	{
		return m_promoPbtTransferType;
	}

	bool IsTransferTypeToGame(void);

	void SetPIN(DWORD pin)
	{
		m_pbtPIN = pin;
		StoreStartDataToNVRAM();
	}
	DWORD GetPIN() const
	{
		return m_pbtPIN;
	}

	/// <summary>
	/// Returns true if the PIN is the PersonalBanker automatic upload PIN, false otherwise.
	/// </summary>
	/// <returns>true if the PIN is the PersonalBanker automatic upload PIN, false otherwise.</returns>
	bool IsAutoUploadPIN() const;

	void SetAmount(__int64 amountInPennies)
	{
		m_amountInPennies = amountInPennies;
		StoreUpdatedDataToNVRAM();
	}

	__int64 GetAmount() const
	{
		return m_amountInPennies;
	}

	void SetRestrictedAmount(__int64 amountInPennies)
	{
		m_restrictedAmountInPennies = amountInPennies;
		StoreUpdatedDataToNVRAM();
	}

	__int64 GetRestrictedAmount() const
	{
		return m_restrictedAmountInPennies;
	}

	bool IsPartialWATOutTransfer() const
	{
		return m_pbtTransferFlags.IsPartialWATOutTransfer();
	}

	bool IsNcepAutoUpload() const
	{
		return m_pbtTransferFlags.IsNcepAutoUpload();
	}

	bool IsCepAutoUpload() const
	{
		return m_pbtTransferFlags.IsCepAutoUpload();
	}

	bool IsWatAutoUpload() const
	{
		return m_pbtTransferFlags.IsWatAutoUpload();
	}

	bool IsPartialCEPOutTransfer() const
	{
		return m_pbtTransferFlags.IsPartialCEPOutTransfer();
	}

	void SetPartialWATOutTransfer(bool isPartial)
	{
		m_pbtTransferFlags.m_flags.m_isPartialWatOut = isPartial;
	}

	void SetPartialCEPOutTransfer(bool isPartial)
	{
		m_pbtTransferFlags.m_flags.m_isPartialCEPOut = isPartial;
	}

	void SetNcepAutoUpload(bool value)
	{
		m_pbtTransferFlags.m_flags.m_isNcepAutoUpload = value;
	}

	void SetCepAutoUpload(bool value)
	{
		m_pbtTransferFlags.m_flags.m_isCepAutoUpload = value;
	}

	void SetWatAutoUpload(bool value)
	{
		m_pbtTransferFlags.m_flags.m_isWatAutoUpload = value;
	}

	bool IsCashoutToHostTransfer() const
	{
		return m_cashoutToHostTransfer;
	}

	bool IsProcessTaxDeduction() const
	{
		return m_pbtTransferFlags.IsProcessTaxDeduction();
	}

	bool IsFederalTaxOverridden() const
	{
		return m_pbtTransferFlags.IsFederalTaxOverridden();
	}

	bool IsStateTaxOverridden() const
	{
		return m_pbtTransferFlags.IsStateTaxOverridden();
	}

	uint64_t GetFederalTaxDeductionInPennies() const
	{
		return m_federalTaxDeductionInPennies;
	}

	int GetJackpotId() const
	{
		return m_jackpotId;
	}

	void SetTaxInformation(bool processTaxDeduction, bool federalTaxOverridden, bool stateTaxOverridden, 
		int jackpotId, uint64_t federalTaxDeductionInPennies)
	{
		m_pbtTransferFlags.m_flags.m_isProcessTaxDeduction = processTaxDeduction;
		m_pbtTransferFlags.m_flags.m_federalTaxOverridden = federalTaxOverridden;
		m_pbtTransferFlags.m_flags.m_stateTaxOverridden = stateTaxOverridden;
		m_jackpotId = jackpotId;
		m_federalTaxDeductionInPennies = federalTaxDeductionInPennies;
		StoreStartDataToNVRAM();
		StoreUpdatedDataToNVRAM();
	}

	void SetCashoutToHostTransfer(bool cashoutToHostTransfer)
	{
		m_cashoutToHostTransfer = cashoutToHostTransfer;
	}

	void SetNonrestrictedAmount(__int64 amountInPennies)
	{
		m_nonrestrictedAmountInPennies = amountInPennies;
		StoreUpdatedDataToNVRAM();
	}

	__int64 GetNonrestrictedAmount() const
	{
		return m_nonrestrictedAmountInPennies;
	}

	void SetSequenceNumber(WORD sequenceNumber)
	{
		m_sequenceNumber = sequenceNumber;
		StoreUpdatedDataToNVRAM();
	}

	WORD GetSequenceNumber() const
	{
		return m_sequenceNumber;
	}

	void SetRestrictedSequenceNumber(WORD sequenceNumber)
	{
		m_restrictedSequenceNumber = sequenceNumber;
		StoreUpdatedDataToNVRAM();
	}

	WORD GetRestrictedSequenceNumber() const
	{
		return m_restrictedSequenceNumber;
	}

	void SetNonrestrictedSequenceNumber(WORD sequenceNumber)
	{
		m_nonrestrictedSequenceNumber = sequenceNumber;
		StoreUpdatedDataToNVRAM();
	}

	WORD GetNonrestrictedSequenceNumber() const
	{
		return m_nonrestrictedSequenceNumber;
	}

	void SetEFTNumber( byte eftNum )
	{
		m_eftNumber = eftNum;
		StoreIDDataToNVRAM();
	}

	byte GetEFTNumber( void ) const
	{
		return m_eftNumber;
	}

	/// <summary>
	/// Returns true if session has expired due a to timeout, returns false otherwise.
	/// </summary>
	/// <param name="tickCount">IN - The current tick count, generally from ::GetTickCount().</param>
	/// <param name="sessionTimeoutValueInMilliseconds">IN - Session timeout value in milliseconds</param>
	/// <returns>Returns a boolean to indicate if a session expired or not.</returns>
	bool HasSessionExpired( DWORD tickCount, DWORD sessionTimeoutValueInMilliseconds ) const;

	bool IsPointRedemptionRestricted( void ) const
	{
		return 	m_bPointRedemptionRestricted;
	}

	bool IsCEPOutEnabled( void ) const
	{
		return m_bCEPOutEnabled;
	}
	
	/// <summary>
	/// Sets the card number.
	/// </summary>
	/// <param name="cardNumber">IN - byte array with card number data.  MUST be at least cardNumberLen bytes long.</param>
	/// <param name="cardNumberLen">IN - card number length.  Normally (less than or) equal to CARD_ID_LEN.</param>
	/// <remarks>
	/// If length != CARD_ID_LEN then the card number is cleared.
	/// </remarks>
	void SetCardNumber( const byte *cardNumber, int cardNumberLen );

	const byte *GetCardNumber( int &cardNumberLen ) const;

	int CompareCardNumber( const byte *cardNumber, int cardNumLen )
	{
		return memcmp( cardNumber, m_cardNumber, cardNumLen );
	}


	PersonalBankerType GetCancelledCode(PersonalBankerType type);

	/// <summary>
	/// Returns the token for current cancelled type.
	/// </summary>
	/// <returns>token for the current cancelled type.</returns>
	LPCTSTR GetCancelledToken(void) const;

	/// <summary>
	/// Resets the transfer session start time to tickCount.
	/// </summary>
	/// <param name="tickCount">IN - The current tick count, generally from ::GetTickCount().</param>
	void ResetSessionStartTime(DWORD tickCount)
	{
		m_sessionStartTickCount = tickCount;
	}
	
	void SetRestrictedPointRedemption( bool bPointRedemptionRestricted )
	{
		m_bPointRedemptionRestricted = bPointRedemptionRestricted;
		StoreStartDataToNVRAM();
	}
	
	void SetDenomination( WORD denomination )
	{
		m_denomination = denomination;
		StoreStartDataToNVRAM();
	}
	WORD GetDenomination( void ) const
	{
		return m_denomination;
	}

	void SetCEPOutEnabled( bool bCEPOutEnabled )
	{
		m_bCEPOutEnabled = bCEPOutEnabled;
		StoreStartDataToNVRAM();
	}

	bool IsTransferRequestCanceled( void ) const
	{
		bool retVal = false;
		if ( GetPersonalBankerType() >= CanceledSessionRequest )
		{
			retVal = true;
		}
		else if (GetPersonalBankerType() == Unspecified)
		{
			// check promo type
			if (GetPromoPersonalBankerType() >= CanceledSessionRequest)
			{
				retVal = true;
			}
		}
		return retVal;
	}

	// Returns true if there is an amoutn to the transfer.
	bool HasAmount( void )
	{
		return ( m_amountInPennies > 0 || (!IsTransferTypeToGame() && (m_restrictedAmountInPennies || m_nonrestrictedAmountInPennies )) );
	}

	/// <summary>
	/// Changes personal banker types to require authorization.
	/// CashDownload -> AuthorizeCashToGame
	/// PromoCreditsToCash -> AuthorizePromoToGame
	/// PointRedemptionToCash -> AuthorizePointsToGameCash
	/// PointRedemptionToPromoCredits -> AuthorizePointsToGamePromo
	/// </summary>
	void ChangePersonalBankerTypeToAuthorization(void);

	/// <summary>
	/// Changes personal banker types to cancelled types.
	/// </summary>
	void ChangePersonalBankerTypeToCanceled(void);

	/// <summary>
	/// Changes personal banker promo types to cancelled types.
	/// </summary>
	void ChangePersonalBankerPromoTypeToCanceled( void );

	int GetCardNumberLen()
	{
		return m_cardNumberLen;
	}

	void SetMinCashUp( DWORD minCashUp )
	{
		m_minimumCashUp = minCashUp;
	}

	/// <summary>
	/// Returns the minimum amount of cash allowed (in the minor unit of the base currency) for a transfer from the EGM.
	/// </summary>
	/// <returns>The minimum amount of cash allowed (in the minor unit of the base currency) for a transfer from the EGM.</returns>
	DWORD GetMinCashUp( void ) const
	{
		return m_minimumCashUp;
	}

	void SetMaxCashUp( DWORD maxCashUp )
	{
		m_maximumCashUp = maxCashUp;
	}

	/// <summary>
	/// Returns the maximum amount of cash allowed (in the minor unit of the base currency) for a transfer from the EGM.
	/// </summary>
	/// <returns>The maximum amount of cash allowed (in the minor unit of the base currency) for a transfer from the EGM.</returns>
	DWORD GetMaxCashUp( void ) const
	{
		return m_maximumCashUp;
	}

	void SetMinPromoUp( DWORD minPromoUp )
	{
		m_minimumPromoUp = minPromoUp;
	}

	/// <summary>
	/// Returns the minimum amount of promotional value allowed (in the minor unit of the base currency) for a transfer from the EGM.
	/// </summary>
	/// <returns>The minimum amount of promotional value allowed (in the minor unit of the base currency) for a transfer from the EGM.</returns>
	int GetMinPromoUp( void ) const
	{
		return m_minimumPromoUp;
	}

	void SetMaxPromoUp( DWORD maxPromoUp )
	{
		m_maximumPromoUp = maxPromoUp;
	}

	/// <summary>
	/// Returns the maximum amount of promotional value allowed (in the minor unit of the base currency) for a transfer from the EGM.
	/// </summary>
	/// <returns>The maximum amount of promotional value allowed (in the minor unit of the base currency) for a transfer from the EGM.</returns>
	int GetMaxPromoUp( void ) const
	{
		return m_maximumPromoUp;
	}

	void SetPBTSent( bool pbtSent)
	{
		m_PBTSent = pbtSent;
	}
	bool GetPBTSent() const
	{
		return m_PBTSent;
	}

	void SetAftTransferFlags(byte flags)
	{
		m_aftTransferFlags = flags;
	}

	byte GetAftTransferFlags() const
	{
		return m_aftTransferFlags;
	}

	/// <summary>
	/// Serialize personal banker transfer object to string for logging purposes.
	/// </summary>
	/// <returns>
	/// std::string object of serialized personal banker transfer members
	/// </returns>
	std::string ToStringForLog() const;

	/// <summary>
	/// Verifies the maximum transfer amounts.
	/// </summary>
	/// <returns>True if amounts are okay.</returns>
	bool VerifyMaxAmounts();


	std::string GetPBTTypeText(PersonalBankerTransferType pbtType) const;
	std::string GetPBTypeText(PersonalBankerType pbType) const;

	/// <summary>
	/// Appends the cashout error flags.
	/// </summary>
	/// <param name="error">IN - cashout error flags.</param>
	void AppendCashoutErrorFlags(WORD error);

	/// <summary>
	/// Gets the cashout error flags.
	/// </summary>
	/// <returns>Cashout Error Flags.</returns>
	WORD GetCashoutErrorFlags() const;

	/// <summary>
	/// Appends the autoupload error flags.
	/// </summary>
	/// <param name="error">IN - autoupload error flags.</param>
	void AppendAutoUploadErrorFlags(WORD error);

	/// <summary>
	/// Gets the autoupload error flags.
	/// </summary>
	/// <returns>Autoupload Error Flags.</returns>
	WORD GetAutoUploadErrorFlags() const;

private:

	void CleanUp();
	void CopyObj(const CPersonalBankerTransfer& rhs);

	// Types and states.
	PollerCancellationType	m_cancelledType;
	PersonalBankerType		m_pbtType;
	PersonalBankerTransferType m_pbtTransferType;
	PersonalBankerTransferType m_promoPbtTransferType;
	void StoreTypesDataToNVRAM();
	std::string GetPollerCancellationType(PollerCancellationType pcType) const;

	// PBT IDs
	int			m_pbtIDLength;
	char		m_pbtID[MAX_PBID_LENGTH];
	byte		m_eftNumber;
	void StoreIDDataToNVRAM();

	// Session start data.
	DWORD		m_pbtPIN;
	DWORD		m_sessionStartTickCount;
	bool		m_bPointRedemptionRestricted;
	int			m_cardNumberLen;
	byte		*m_cardNumber;
	bool		m_bCEPOutEnabled;
	WORD		m_denomination;
	uint64_t	m_federalTaxDeductionInPennies;
	int			m_jackpotId;
	void StoreStartDataToNVRAM();

	// Updated data (52 bytes)
	PersonalBankerState m_state_NotUsed; // Not used. But still written to and read from NVRAM.
	WORD		m_restrictedSequenceNumber;
	WORD		m_nonrestrictedSequenceNumber;
	WORD		m_sequenceNumber;
	__int64		m_originalCreditMeter;
	__int64		m_amountInPennies;
	__int64		m_restrictedAmountInPennies;
	__int64		m_nonrestrictedAmountInPennies;

	// Added types for NCEP Out.  Need to use this spot since there isn't room in Types section.  Now using 50 of 56 bytes.
	PollerCancellationType	m_promoCancelledType;
	PersonalBankerType		m_promoPbtType;

	u_PersonalBankerTransferFlags m_pbtTransferFlags;

	void StoreUpdatedDataToNVRAM();

	// CardTime data.
	time_t		m_utcCardInTime;
	void StoreCardTimeToNVRAM();

	// Not persitently stored. Tracker 17751
	DWORD m_minimumCashUp;
	DWORD m_maximumCashUp;

	// Not persistently stored.
	DWORD m_minimumPromoUp;
	DWORD m_maximumPromoUp;

	// Not persistently stored. Tracker 12550
	bool m_PBTSent;

	// Cashout to host transfers will not be NVRAM backed-up
	bool m_cashoutToHostTransfer;

	// Not-persisted, transfer flags from last AFT transaction
	byte m_aftTransferFlags;

	WORD m_cashoutErrorFlags;
	WORD m_autoUploadErrorFlags;
};
