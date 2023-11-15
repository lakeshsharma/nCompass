#pragma once

#include "InternalMsgContent.h"

const static byte VALIDATION_NUMBER_LENGTH = 19;

typedef enum 
{
	UNUSED,
	EXPIRES_SECONDS,
	EXPIRES_MINUTES,
	EXPIRES_HOURS,
	EXPIRES_DAYS,
	EXPIRES_YEARS
} TicketExpirationType;

typedef enum 
{
	TicketActionCodeNoValue = 0x00,
	RequestValditionNumber = 0x01,
	RequestVendAuthorization = 0x02,
	RequestVoucherInfo = 0x03,
	TicketVoucherProcessing = 0x10,      //not used by Poller
	PrintComplete = 0x11,
	PrintCompleteGameGeneratedValNum = 0x12,
	VendComplete = 0x13,
	PrintCompleteCompTicket = 0x15,
	SlotMachineRejectedVoucher = 0x17,
	SlotMachineConfiscatedVoucher = 0x18,
	PrintStatusUnknown = 0x20,
	SlotMachineTimedOut = 0x21,
	InvalidPrintMessage = 0x22,
	PrinterFailure = 0x2f,
	UnknownVendVoucherStatus = 0x30,
	VenReplyTimeout = 0x31,
	IncompatibleDenom = 0x32,
	VoucherOverLimit = 0x33,
	VoucherOverCreditLimit = 0x34,
	VoucherRejectedByGame = 0x35,
	ValidatorFailure = 0x3f,
	// TBD. This is where the result codes
	TicketVoucherStatusRequest = 128,   //not used by Poller
	AssignBarcode = 129,
	TellGameToGenerateBarcode = 130,
	VendVoucher = 131,
	IncorrectMachineId = 145,
	RejectVoucherVoucherNoMatch = 162,
	RequestValidationNumIssue = 192,
	ActionCode1DeniedByDPU = 232,
	ActionCode2DeniedByDPU = 233,
	ActionCode192DeniedByDPU = 234,
}TicketActionCode;


typedef enum
{
	TicketTypeNoValue = 0,
	VoucherTicket = 1,
	CouponPromoGameGenerated = 3,
	VoucherPromo = 4,
    CouponPromo = 5,
    AttPaidCCReceipt = 33,
    AttPaidJPReceipt = 34,
    CashoutNoReceipt = 49,
    SingleWinNoReceipt = 50,
}TicketType;

typedef enum 
{
	Acknowledged = 0,
	AssignBarcodeNumber = 129,
	TellGameToGenerateOwnBarcodeNum = 130,
	AuthorizedToVendVoucher = 131,
	PrintCompTicket = 133,
	RequestForValidationNumDenied = 144,
	CannotAssignNumber = 146,
	RejectVoucher = 160,
	RejectVoucherAlreadyRedeemed = 161,
	RejectVoucherNoMatch = 162,
	RejectVoucherInvalidNumber = 163,
	RejectVoucherVoided = 164,
	RejectVoucherExpired = 165,
	RejectVoucherMultipleMatches = 166,
	RejectVoucherStaleRequest = 167,
	RejectVoucherReqFromFuture = 168,
	RejectVoucherNotRedeemable = 169,
	RejectVoucherRedeemInProgress = 170,
	ConfiscateVoucher = 176,
	ConfiscateVoucherAlreadyRedeemed = 177,
	ConfiscateVoucherNoMatch = 178,
	ConfiscateVoucherInvalidNumber = 179,
	ConfiscateVoucherVoided = 180,
	ConfiscateVoucherExpired = 181,
	TicketStatusChangedToExpired = 203,
	PollerAssignedValImmediateUse = 204,
	PollerAssignedValLaterUse = 205,
	requestSeqNumForSystemGeneratedVal = 206,
	TicketStatusChangedToExpiredManually = 207,
	ValidationNumberIssued = 208,
	SequenceNumberIssued = 222,
	CannotComplyWith192 = 224,
	WillNotIssueValidationNumbers = 225,
	TicketStatusChangedToVoidedManually = 240,
	TicketPrintedAddedManually = 241,
	Duplicate = 299,
	AssumedDuplicate = 300,
	DuplicateValidationNumber = 301,
	RedeemedAmtTypeDifferentFromPrintedAmtType = 302,
	TicketRedeemedNotInDB = 303,
	TicketRedeemedNotAuthorized = 304,
	TicketRedeemedMoreThanOnce = 306,
	TicketAssignedToSlotWithDifferentAmt = 307,
	UnknownValNumAssignedToSlot = 308,
	DuplicateValNumAssignedToSlot = 309,
	UnknownValNumVoidedBySlot = 310,
	DuplicateValNumVoidedBySlot = 311,
	StatusChangedToExpiredDuringVendReq = 312,
	VoucherLockedBySameSlot = 313,
	ActionCodeNotSupported = 314,
	PreIssuedRecNotFound = 315,
	PreIssuedRecHasDifferentStatus = 316,
	TicketPrintedNotIssued = 317,
	TransactionFromUnknownSlot = 318,
	VendAuthDeniedTktPrintedOnUnknownSlot = 319,
	VendAuthDeniedCouponNotValid = 320,
	VoucherRedeemedNotPrinted = 513,
	VoucherRedeemedAgain = 514,
	VoucherRedeemedNotPrintedRedeemedAgain = 515,
	VoucherRedeemedWithDiffAmtType = 516,
	VoucherRedeemedNotPrintedWithDiffAmtType = 517,
	VoucherRedeemedAgainWithDifferentAmtType = 518,
	VoucherRedeemedNotPrintedRedeemedAgainWithDiffAmtType = 519,
	VoucherRedeemedWithStatus0567 = 520,
	VoucherRedeemedWithStatus0567NotPrinted = 521,
	VoucherRedeemedWithStatus0567AndDiffAmtType = 524,
	VoucherRedeemedWithStatus0567AndDiffAmtTypeNotPrinted = 525,
	VoucherRejectedNotLocked = 528,
	VoucherRejectedMultipleEntries = 544,
	TicketResultCodeNoValue = 0xFFFF,
} TicketResultCode;

typedef enum
{
	TICKET_IN_STATUS_UNKNOWN = 0,
	TICKET_IN_STATUS_STACKED,
	TICKET_IN_STATUS_PENDING,
	TICKET_IN_STATUS_ERROR,
} TicketInStatus;

class CTicket :
	public CInternalMsgContent<INT_MSG_CONTENT_TICKET>
{
public:
	/// <summary>
	/// Default constructor. Initializes a new instance of the <see cref="CTicket"/> class.
	/// </summary>
	CTicket(void);

	/// <summary>
	/// Copy constructor. Initializes a new instance of the <see cref="CTicket"/> class.
	/// </summary>
	/// <param name="rhs">IN - The instance to copy from.</param>
	CTicket(const CTicket &rhs);

	/// <summary>
	/// byte buffer constructor.  Initializes a new instance of the <see cref="CTicket"/> class.
	/// </summary>
	/// <param name="nvramBuffer">IN - The nvram buffer.</param>
	/// <param name="nvramBufferLength">IN - Length of the nvram buffer.</param>
	/// <remarks>
	/// The caller is responsible for setting m_bTicketIn.
	/// </remarks>
	CTicket( const byte *nvramBuffer);

	/// <summary>
	/// Destructor. Finalizes an instance of the <see cref="CTicket"/> class.
	/// </summary>
	~CTicket(void);

	/// <summary>
	/// Sets the nvram buffer.
	/// </summary>
	/// <param name="nvramBuffer">IN/OUT - The nvram buffer.</param>
	/// <param name="nvramBufferLength">Length of the nvram buffer.</param>
	/// <returns>The number of bytes of nvramBuffer that were set.</returns>
	int GetNvramBuffer( byte *nvramBuffer, int nvramBufferLength ) const;

	/// <summary>
	/// Sets the ticket number.
	/// </summary>
	/// <param name="number">The number.</param>
	void	SetTicketNumber(int number);

	/// <summary>
	/// Get the ticket number.
	/// </summary>
	/// <returns>The ticket number.</returns>
	int		Number() const;

	/// <summary>
	/// Sets the amount.
	/// </summary>
	/// <param name="amount">The amount.</param>
	void	SetAmount(__int64 amount);

	/// <summary>
	/// Get the amount.
	/// </summary>
	/// <returns>The amount.</returns>
	__int64 Amount() const;

	/// <summary>
	/// Sets the action code.
	/// </summary>
	/// <param name="code">The code.</param>
	void	SetActionCode(TicketActionCode code);

	/// <summary>
	/// Get the action code.
	/// </summary>
	/// <returns>The action code.</returns>
	TicketActionCode ActionCode() const;

	/// <summary>
	/// Sets the result code.
	/// </summary>
	/// <param name="code">The code.</param>
	void	SetResultCode(TicketResultCode code);

	/// <summary>
	/// Get the result code.
	/// </summary>
	/// <returns>The result code.</returns>
	TicketResultCode ResultCode() const;

	/// <summary>
	/// Sets the ticket type.
	/// </summary>
	/// <param name="type">The type.</param>
	/// <param name="bTicketIn">true for ticket inserted, false otherwise.</param>
	void	SetType(TicketType type, bool bTicketIn = true );

	/// <summary>
	/// Get the ticket type.
	/// </summary>
	/// <returns>The ticket type.</returns>
	TicketType Type() const;

	/// <summary>
	/// Get the ticket inserted flag.
	/// </summary>
	/// <returns>true for ticket inserted, false otherwise.</returns>
	bool	IsTicketIn( void ) const;

	/// <summary>
	/// Sets the validation number.
	/// </summary>
	/// <param name="validationNumber">The validation number.</param>
	/// <param name="validationLength">Length of the validation.</param>
	void	SetValidationNumber(const byte *validationNumber, byte validationLength);

	/// <summary>
	/// Get a pointer to the validation number.
	/// </summary>
	/// <param name="validationLength">OUT - The length of the validation number.</param>
	/// <returns>A pointer to the validation number.</returns>
	const byte	*ValidationNumber(byte &validationLength) const;

	/// <summary>
	/// Get the length of the validation number.
	/// </summary>
	/// <returns>The length of the validation number.</returns>
	byte ValidationLength() const;

	/// <summary>
	/// Sets the machine number.
	/// </summary>
	/// <param name="machineNumber">The machine number.</param>
	void	SetMachineNumber(int machineNumber);

	/// <summary>
	/// Get the machine number.
	/// </summary>
	/// <returns>The machine number.</returns>
	int MachineNumber() const;

	/// <summary>
	/// Sets the type of the expiration.
	/// </summary>
	/// <param name="expirationType">Type of the expiration.</param>
	void	SetExpirationType(TicketExpirationType expirationType);

	/// <summary>
	/// Get the type of the expiration.
	/// </summary>
	/// <returns>The type of the expiration.</returns>
	TicketExpirationType ExpirationType() const;

	/// <summary>
	/// Sets the expiration duration.
	/// </summary>
	/// <param name="duration">The expiration duration.</param>
	void	SetExpirationDuration(byte duration);

	/// <summary>
	/// Get the expiration duration.
	/// </summary>
	/// <returns>The expiration duration.</returns>
	byte	ExpirationDuration() const;

	/// <summary>
	/// Sets the game date time.
	/// </summary>
	/// <param name="dateTime">The date time.</param>
	void	SetGameDateTime(time_t dateTime);

	/// <summary>
	/// Get the game date time.
	/// </summary>
	/// <returns>The date time.</returns>
	time_t   GameDateTime() const;

	/// <summary>
	/// Sets the sequence number.
	/// </summary>
	/// <param name="sequenceNumber">The sequence number.</param>
	void	SetSequenceNumber(WORD sequenceNumber);

	/// <summary>
	/// Get the sequence number.
	/// </summary>
	/// <returns>The sequence number.</returns>
	WORD	SequenceNumber() const;

	/// <summary>
	/// Sets the ID.
	/// </summary>
	/// <param name="id">The id.</param>
	void	SetID(__int64 id);

	/// <summary>
	/// Get the ID.
	/// </summary>
	/// <returns>The ID.</returns>
	__int64 ID() const;

	/// <summary>
	/// Equals operator.
	/// </summary>
	/// <param name="rhs">The instance to compare with.</param>
	/// <returns>true if the instances are considered equal, false otherwise.</returns>
	bool operator==( const CTicket & ) const;

	/// <summary>
	/// Not equals operator.
	/// </summary>
	/// <param name="rhs">The instance to compare with.</param>
	/// <returns>true if the instances are not considered equal, false otherwise.</returns>
	bool operator!=( const CTicket & ) const;

	/// <summary>
	/// Assignment operator.
	/// </summary>
	/// <param name="rhs">The instance to assign from.</param>
	/// <returns>A reference to the target of the assignment.</returns>
	CTicket &operator=( const CTicket &rhs );

	/// <summary>
	/// Serialize ticket object to string for logging purposes.
	/// </summary>
	/// <returns>A string with all of the ticket fields.</returns>
	std::string ToStringForLog();

	/// <summary>
	/// Determines whether voucher ticket amount is less than max.
	/// </summary>
	/// <returns>True if voucher amount is less than max allowed or ticket is not voucher.</returns>
	bool IsAmountLessThanMax();

private:

	int				m_number;
	__int64			m_amount; //in pennies
	TicketActionCode m_actionCode;
	TicketResultCode m_resultCode;
	TicketType		m_type;
	bool			m_bTicketIn;
	byte			*m_validationNumber;
	byte			m_validationNumberLength;
	int				m_machineNumber;
	TicketExpirationType m_expirationType;
	byte			m_expirationDuration;
	time_t			m_gameDateTime;
	WORD			m_sequenceNumber;
	__int64			m_ID;

	std::string GetActionCodeText();
	std::string GetResultCodeText();
	std::string GetTicketTypeText();
	std::string GetTicketInOutText();
	std::string GetValidationString();
	std::string GetExpirationTypeText();
	
};
