#include "stdafx.h"
#include "Ticket.h"

#include "Hardware/NonVolatileMemoryMaps.h"
#include "Utilities.h"
#include "Diagnostics.h"

using namespace std;

static const char * TicketExpirationTypeStrings[] =
{
	"UNUSED",
	"EXPIRES_SECONDS",
	"EXPIRES_MINUTES",
	"EXPIRES_HOURS",
	"EXPIRES_DAYS",
	"EXPIRES_YEARS"
};

CTicket::CTicket(void)
{
	m_number = -1;
	m_amount = -1; 
	m_actionCode = TicketActionCodeNoValue;
	m_resultCode = TicketResultCodeNoValue;
	m_type = TicketTypeNoValue;
	m_bTicketIn = true;
	m_validationNumber = new byte[VALIDATION_NUMBER_LENGTH];
	memset(m_validationNumber, 0, VALIDATION_NUMBER_LENGTH);
	m_validationNumberLength = 0;
	m_machineNumber = -1;
	m_expirationType = 	EXPIRES_DAYS;
	m_expirationDuration = 1;
	m_sequenceNumber = 0;
	m_ID = 0;
    m_gameDateTime = 0;
}

CTicket::CTicket(const byte *nvramBuffer)
{
	const byte *bufferPos = nvramBuffer;

	memcpy( &m_number, bufferPos, sizeof( m_number ) );
	bufferPos += sizeof( m_number );

	memcpy( &m_amount, bufferPos, sizeof( m_amount ) );
	bufferPos += sizeof( m_amount );

	m_gameDateTime = CUtilities::MakeTimeFromNvramBuffer( bufferPos, NVRAM_TIME_SIZE );
	bufferPos += NVRAM_TIME_SIZE;

	memcpy( &m_actionCode, bufferPos, sizeof( m_actionCode ) );
	bufferPos += sizeof( m_actionCode );

	memcpy( &m_resultCode, bufferPos, sizeof( m_resultCode ) );
	bufferPos += sizeof( m_resultCode );

	memcpy( &m_type, bufferPos, sizeof( m_type ) );
	bufferPos += sizeof( m_type );

	m_validationNumber = new byte[VALIDATION_NUMBER_LENGTH];
	memcpy( m_validationNumber, bufferPos, VALIDATION_NUMBER_LENGTH );	
	bufferPos += VALIDATION_NUMBER_LENGTH;

	memcpy( &m_machineNumber, bufferPos, sizeof( m_machineNumber ) );
	bufferPos += sizeof( m_machineNumber );

	memcpy( &m_expirationType, bufferPos, sizeof( m_expirationType ) );
	bufferPos += sizeof( m_expirationType );

	memcpy( &m_expirationDuration, bufferPos, sizeof( m_expirationDuration ) );
	bufferPos += sizeof( m_expirationDuration );

	memcpy( &m_sequenceNumber, bufferPos, sizeof( m_sequenceNumber ) );
	bufferPos += sizeof( m_sequenceNumber );

	memcpy( &m_ID, bufferPos, sizeof( m_ID ) );
	bufferPos += sizeof( m_ID );

	memcpy( &m_validationNumberLength, bufferPos, sizeof( m_validationNumberLength ) );
	if ( m_validationNumberLength > VALIDATION_NUMBER_LENGTH )
		m_validationNumberLength = VALIDATION_NUMBER_LENGTH;
}

CTicket::CTicket(const CTicket &rhs)
{
	m_validationNumber = NULL;
	*this = rhs;
}

CTicket & CTicket::operator=( const CTicket &rhs )
{
	m_number = rhs.Number();
	m_amount = rhs.Amount();
	m_actionCode = rhs.ActionCode();
	m_resultCode = rhs.ResultCode();
	m_type = rhs.Type();
	m_bTicketIn = rhs.IsTicketIn();
	const byte * temp = rhs.ValidationNumber( m_validationNumberLength );
	if ( m_validationNumber == NULL )
		m_validationNumber = new byte[VALIDATION_NUMBER_LENGTH];
	if (temp != NULL)
		memcpy(m_validationNumber, temp, m_validationNumberLength);
	m_machineNumber = rhs.MachineNumber();
	m_expirationType = rhs.ExpirationType();
	m_expirationDuration = rhs.ExpirationDuration();
	m_sequenceNumber = rhs.SequenceNumber();
	m_ID = rhs.ID();
	m_gameDateTime = rhs.GameDateTime();	

	return *this;
}

CTicket::~CTicket(void)
{
	if ( m_validationNumber != NULL )
	{
		delete [] m_validationNumber;
	}
}

bool CTicket::operator ==(const CTicket &rhs) const
{
	//Tickets are equal when validation number, amount, and ticketdate are equal
	byte valLength;
	const byte *valNumber;

	bool retVal = false;

	valNumber = rhs.ValidationNumber(valLength);
	if (valLength == m_validationNumberLength && !memcmp(m_validationNumber,valNumber,valLength)  )
		retVal = true;

	//if it's a ticket out then the amounts must also match.
	if (!IsTicketIn() && ((m_amount != rhs.Amount()) ||
		m_gameDateTime != rhs.GameDateTime()))
		retVal = false;

	return retVal;
}

bool CTicket::operator !=(const CTicket &rhs) const
{
	byte valLength;
	const byte *valNumber = rhs.ValidationNumber(valLength);
	if (valLength != m_validationNumberLength || memcmp(m_validationNumber,valNumber,valLength) ||
		m_amount != rhs.Amount() ||
		m_gameDateTime != rhs.GameDateTime() )
		return true;
	else
		return false;
}

int CTicket::GetNvramBuffer( byte *nvramBuffer, int nvramBufferLength ) const
{
	byte *bufferPos = nvramBuffer;

	if (nvramBuffer == NULL)
	{
		nvramBufferLength = 0;
	}

	// Fix signed/unsigned comparison issue with NVRAM_TICKET_LOG_DATA_SIZE.
	if (nvramBufferLength < 0)
	{
		nvramBufferLength = 0;
	}

	if ( nvramBufferLength >= (int)NVRAM_TICKET_SIZE )
	{
		memcpy( bufferPos, &m_number, sizeof( m_number ) );
		bufferPos += sizeof( m_number );

		memcpy( bufferPos, &m_amount, sizeof( m_amount ) );
		bufferPos += sizeof( m_amount );

		CUtilities::GetNvramBufferFromTime( m_gameDateTime, bufferPos, NVRAM_TIME_SIZE );
		bufferPos += NVRAM_TIME_SIZE;

		memcpy( bufferPos, &m_actionCode, sizeof( m_actionCode ) );
		bufferPos += sizeof( m_actionCode );

		memcpy( bufferPos, &m_resultCode, sizeof( m_resultCode ) );
		bufferPos += sizeof( m_resultCode );

		memcpy( bufferPos, &m_type, sizeof( m_type ) );
		bufferPos += sizeof( m_type );

		memcpy( bufferPos, m_validationNumber, VALIDATION_NUMBER_LENGTH );	
		bufferPos += VALIDATION_NUMBER_LENGTH;

		memcpy( bufferPos, &m_machineNumber, sizeof( m_machineNumber ) );
		bufferPos += sizeof( m_machineNumber );

		memcpy( bufferPos, &m_expirationType, sizeof( m_expirationType ) );
		bufferPos += sizeof( m_expirationType );

		memcpy( bufferPos, &m_expirationDuration, sizeof( m_expirationDuration ) );
		bufferPos += sizeof( m_expirationDuration );

		memcpy( bufferPos, &m_sequenceNumber, sizeof( m_sequenceNumber ) );
		bufferPos += sizeof( m_sequenceNumber );

		memcpy( bufferPos, &m_ID, sizeof( m_ID ) );
		bufferPos += sizeof( m_ID );

		memcpy( bufferPos, &m_validationNumberLength, sizeof( m_validationNumberLength ) );
		bufferPos += sizeof( m_validationNumberLength );
	}

	return (bufferPos - nvramBuffer);

}


void CTicket::SetTicketNumber(int number)
{
	m_number = number;
}

int	CTicket::Number() const
{
	return m_number;
}

void CTicket::SetAmount(__int64 amount)
{
	m_amount = amount;
}

__int64 CTicket::Amount() const
{
	return m_amount;
}

void CTicket::SetActionCode(TicketActionCode code)
{
	m_actionCode = code;
}

TicketActionCode CTicket::ActionCode() const
{
	return m_actionCode;
}

void CTicket::SetResultCode(TicketResultCode code)
{
	m_resultCode = code;
}

TicketResultCode CTicket::ResultCode() const
{
	return m_resultCode;
}

void CTicket::SetType(TicketType type, bool bTicketIn )
{
	m_type = type;
	m_bTicketIn = bTicketIn;
}

bool CTicket::IsTicketIn( void ) const
{
	return m_bTicketIn;
}

TicketType CTicket::Type() const
{
	return m_type;
}

void CTicket::SetValidationNumber(const byte *validationNumber, byte validationLength)
{

	if (validationNumber == NULL)
	{
		validationLength = 0;
	}
    else
    {
        if (validationLength > VALIDATION_NUMBER_LENGTH)
            validationLength = VALIDATION_NUMBER_LENGTH;
        memcpy(m_validationNumber, validationNumber, validationLength);
    }
	m_validationNumberLength = validationLength;

}

const byte *CTicket::ValidationNumber(byte &validationLength) const
{
	validationLength = m_validationNumberLength;
	return m_validationNumber;
}

byte CTicket::ValidationLength() const
{
	return m_validationNumberLength;
}


void CTicket::SetMachineNumber(int machineNumber)
{
	m_machineNumber = machineNumber;
}

int CTicket::MachineNumber() const
{
	return m_machineNumber;
}

void CTicket::SetExpirationType(TicketExpirationType expirationType)
{
	m_expirationType = expirationType;
}

TicketExpirationType CTicket::ExpirationType() const
{
	return m_expirationType;
}

void CTicket::SetExpirationDuration(byte duration)
{
	m_expirationDuration = duration;
}

byte CTicket::ExpirationDuration() const
{
	return m_expirationDuration;
}

void CTicket::SetGameDateTime(time_t dateTime)
{
	m_gameDateTime = dateTime;
}

time_t CTicket::GameDateTime() const
{
	return m_gameDateTime;
}

void CTicket::SetSequenceNumber(WORD sequenceNumber)
{
	m_sequenceNumber = sequenceNumber;
}

WORD CTicket::SequenceNumber() const
{ 
	return m_sequenceNumber;
}

void CTicket::SetID(__int64 id)
{
	m_ID = id;
}

__int64 CTicket::ID() const
{
	return m_ID;
}

std::string CTicket::ToStringForLog()
{
// #pragma region Field Names
	const string NUMBER = "Number";
	const string AMOUNT = "Amount";
	const string TICKETACTIONCODE = "TicketActionCode";
	const string RESULTCODE = "ResultCode";
	const string TICKETTYPE = "TicketType";
	const string TICKETINOUT = "TicketInOut";
	const string VALIDATIONNUMBER = "ValidationNumber";
	const string VALIDATIONNUMBERLENGTH = "ValidationNumberLength";
	const string MACHINENUMBER = "MachineNumber";
	const string EXPIRATIONTYPE = "ExpirationType";
	const string EXPIRATIONDURATION = "expirationDuration";
	const string GAMEDATETIME = "gameDateTime";
	const string SEQUENCENUMBER = "SequenceNumber";
	const string ID = "ID";
// #pragma endregion Field Names
	
	string logStr;
	string timeStr = CUtilities::FormatDateTimeString(m_gameDateTime);

	logStr = FormatString("%s=%d; %s=%lld; %s=%s; %s=%s; %s=%s; %s=%s; %s=%s; %s=%d; %s=%d; %s=%s; %s=%d; %s=%s; %s=%d; %s=%lld",
		NUMBER.c_str(), m_number,
		AMOUNT.c_str(), (long long)m_amount,
		TICKETACTIONCODE.c_str(), GetActionCodeText().c_str(),
		RESULTCODE.c_str(), GetResultCodeText().c_str(),
		TICKETTYPE.c_str(), GetTicketTypeText().c_str(),
		TICKETINOUT.c_str(), GetTicketInOutText().c_str(),
		VALIDATIONNUMBER.c_str(), GetValidationString().c_str(),
		VALIDATIONNUMBERLENGTH.c_str(), m_validationNumberLength,
		MACHINENUMBER.c_str(), m_machineNumber,
		EXPIRATIONTYPE.c_str(), GetExpirationTypeText().c_str(),
		EXPIRATIONDURATION.c_str(), m_expirationDuration,
		GAMEDATETIME.c_str(), timeStr.c_str(),
		SEQUENCENUMBER.c_str(), m_sequenceNumber,
		ID.c_str(), (long long)m_ID
		);
	return logStr;

}

std::string CTicket::GetActionCodeText()
{
	string string;
	switch (m_actionCode)
	{
	case TicketActionCodeNoValue :
		string = "TicketActionCodeNoValue";
		break;
	case RequestValditionNumber:
		string = "RequestValditionNumber";
		break;
	case RequestVendAuthorization:
		string = "RequestVendAuthorization";
		break;
	case RequestVoucherInfo:
		string = "RequestVoucherInfo";
		break;
	case TicketVoucherProcessing:
		string = "TicketVoucherProcessing";
		break;
	case PrintComplete:
		string = "PrintComplete";
		break;
	case PrintCompleteGameGeneratedValNum:
		string = "PrintCompleteGameGeneratedValNum";
		break;
	case VendComplete:
		string = "VendComplete";
		break;
	case PrintCompleteCompTicket:
		string = "PrintCompleteCompTicket";
		break;
	case SlotMachineRejectedVoucher:
		string = "SlotMachineRejectedVoucher";
		break;
	case SlotMachineConfiscatedVoucher:
		string = "SlotMachineConfiscatedVoucher";
		break;
	case PrintStatusUnknown:
		string = "PrintStatusUnknown";
		break;
	case SlotMachineTimedOut:
		string = "SlotMachineTimedOut";
		break;
	case InvalidPrintMessage:
		string = "InvalidPrintMessage";
		break;
	case PrinterFailure:
		string = "PrinterFailure";
		break;
	case UnknownVendVoucherStatus:
		string = "UnknownVendVoucherStatus";
		break;
	case VenReplyTimeout:
		string = "VenReplyTimeout";
		break;
	case IncompatibleDenom:
		string = "IncompatibleDenom";
		break;
	case VoucherOverLimit:
		string = "VoucherOverLimit";
		break;
	case VoucherOverCreditLimit:
		string = "VoucherOverCreditLimit";
		break;
	case VoucherRejectedByGame:
		string = "VoucherRejectedByGame";
		break;
	case ValidatorFailure:
		string = "ValidatorFailure";
		break;
	case TicketVoucherStatusRequest:
		string = "TicketVoucherStatusRequest";
		break;
	case AssignBarcode:
		string = "AssignBarcode";
		break;
	case TellGameToGenerateBarcode:
		string = "TellGameToGenerateBarcode";
		break;
	case VendVoucher:
		string = "VendVoucher";
		break;
	case IncorrectMachineId:
		string = "IncorrectMachineId";
		break;
	case RejectVoucherVoucherNoMatch:
		string = "RejectVoucherVoucherNoMatch";
		break;
	case RequestValidationNumIssue:
		string = "RequestValidationNumIssue";
		break;
	case ActionCode1DeniedByDPU:
		string = "ActionCode1DeniedByDPU";
		break;
	case ActionCode2DeniedByDPU:
		string = "ActionCode2DeniedByDPU";
		break;
	case ActionCode192DeniedByDPU:
		string = "ActionCode192DeniedByDPU";
		break;

	default:
		string = FormatString("%d", m_actionCode);
		break;
	}

	return string;
}

std::string CTicket::GetResultCodeText()
{
	string string;
	switch (m_resultCode)
	{
	case Acknowledged:
		string = "Acknowledged";
		break;
	case AssignBarcodeNumber:
		string = "AssignBarcodeNumber";
		break;
	case TellGameToGenerateOwnBarcodeNum:
		string = "TellGameToGenerateOwnBarcodeNum";
		break;
	case AuthorizedToVendVoucher:
		string = "AuthorizedToVendVoucher";
		break;
	case PrintCompTicket:
		string = "PrintCompTicket";
		break;
	case RequestForValidationNumDenied:
		string = "RequestForValidationNumDenied";
		break;
	case CannotAssignNumber:
		string = "CannotAssignNumber";
		break;
	case RejectVoucher:
		string = "RejectVoucher";
		break;
	case RejectVoucherAlreadyRedeemed:
		string = "RejectVoucherAlreadyRedeemed";
		break;
	case RejectVoucherNoMatch:
		string = "RejectVoucherNoMatch";
		break;
	case RejectVoucherInvalidNumber:
		string = "RejectVoucherInvalidNumber";
		break;
	case RejectVoucherVoided:
		string = "RejectVoucherVoided";
		break;
	case RejectVoucherExpired:
		string = "RejectVoucherExpired";
		break;
	case RejectVoucherMultipleMatches:
		string = "RejectVoucherMultipleMatches";
		break;
	case RejectVoucherStaleRequest:
		string = "RejectVoucherStaleRequest";
		break;
	case RejectVoucherReqFromFuture:
		string = "RejectVoucherReqFromFuture";
		break;
	case RejectVoucherNotRedeemable:
		string = "RejectVoucherNotRedeemable";
		break;
	case RejectVoucherRedeemInProgress:
		string = "RejectVoucherRedeemInProgress";
		break;
	case ConfiscateVoucher:
		string = "ConfiscateVoucher";
		break;
	case ConfiscateVoucherAlreadyRedeemed:
		string = "ConfiscateVoucherAlreadyRedeemed";
		break;
	case ConfiscateVoucherInvalidNumber:
		string = "ConfiscateVoucherInvalidNumber";
		break;
	case ConfiscateVoucherVoided:
		string = "ConfiscateVoucherVoided";
		break;
	case ConfiscateVoucherExpired:
		string = "ConfiscateVoucherExpired";
		break;
	case TicketStatusChangedToExpired:
		string = "TicketStatusChangedToExpired";
		break;
	case PollerAssignedValImmediateUse:
		string = "PollerAssignedValImmediateUse";
		break;
	case PollerAssignedValLaterUse:
		string = "PollerAssignedValLaterUse";
		break;
	case requestSeqNumForSystemGeneratedVal:
		string = "requestSeqNumForSystemGeneratedVal";
		break;
	case TicketStatusChangedToExpiredManually:
		string = "TicketStatusChangedToExpiredManually";
		break;
	case ValidationNumberIssued:
		string = "ValidationNumberIssued";
		break;
	case SequenceNumberIssued:
		string = "SequenceNumberIssued";
		break;
	case CannotComplyWith192:
		string = "CannotComplyWith192";
		break;
	case WillNotIssueValidationNumbers:
		string = "WillNotIssueValidationNumbers";
		break;
	case TicketStatusChangedToVoidedManually:
		string = "TicketStatusChangedToVoidedManually";
		break;
	case TicketPrintedAddedManually:
		string = "TicketPrintedAddedManually";
		break;
	case Duplicate:
		string = "Duplicate";
		break;
	case AssumedDuplicate:
		string = "AssumedDuplicate";
		break;
	case DuplicateValidationNumber:
		string = "DuplicateValidationNumber";
		break;
	case RedeemedAmtTypeDifferentFromPrintedAmtType:
		string = "RedeemedAmtTypeDifferentFromPrintedAmtType";
		break;
	case TicketRedeemedNotInDB:
		string = "TicketRedeemedNotInDB";
		break;
	case TicketRedeemedNotAuthorized:
		string = "TicketRedeemedNotAuthorized";
		break;
	case TicketRedeemedMoreThanOnce:
		string = "TicketRedeemedMoreThanOnce";
		break;
	case TicketAssignedToSlotWithDifferentAmt:
		string = "TicketAssignedToSlotWithDifferentAmt";
		break;
	case UnknownValNumAssignedToSlot:
		string = "UnknownValNumAssignedToSlot";
		break;
	case DuplicateValNumAssignedToSlot:
		string = "DuplicateValNumAssignedToSlot";
		break;
	case UnknownValNumVoidedBySlot:
		string = "UnknownValNumVoidedBySlot";
		break;
	case DuplicateValNumVoidedBySlot:
		string = "DuplicateValNumVoidedBySlot";
		break;
	case StatusChangedToExpiredDuringVendReq:
		string = "StatusChangedToExpiredDuringVendReq";
		break;
	case VoucherLockedBySameSlot:
		string = "VoucherLockedBySameSlot";
		break;
	case ActionCodeNotSupported:
		string = "ActionCodeNotSupported";
		break;
	case PreIssuedRecNotFound:
		string = "PreIssuedRecNotFound";
		break;
	case PreIssuedRecHasDifferentStatus:
		string = "PreIssuedRecHasDifferentStatus";
		break;
	case TicketPrintedNotIssued:
		string = "TicketPrintedNotIssued";
		break;
	case TransactionFromUnknownSlot:
		string = "TransactionFromUnknownSlot";
		break;
	case VendAuthDeniedTktPrintedOnUnknownSlot:
		string = "VendAuthDeniedTktPrintedOnUnknownSlot";
		break;
	case VendAuthDeniedCouponNotValid:
		string = "VendAuthDeniedCouponNotValid";
		break;
	case VoucherRedeemedNotPrinted:
		string = "VoucherRedeemedNotPrinted";
		break;
	case VoucherRedeemedAgain:
		string = "VoucherRedeemedAgain";
		break;
	case VoucherRedeemedNotPrintedRedeemedAgain:
		string = "VoucherRedeemedNotPrintedRedeemedAgain";
		break;
	case VoucherRedeemedWithDiffAmtType:
		string = "VoucherRedeemedWithDiffAmtType";
		break;
	case VoucherRedeemedNotPrintedWithDiffAmtType:
		string = "VoucherRedeemedNotPrintedWithDiffAmtType";
		break;
	case VoucherRedeemedAgainWithDifferentAmtType:
		string = "VoucherRedeemedAgainWithDifferentAmtType";
		break;
	case VoucherRedeemedNotPrintedRedeemedAgainWithDiffAmtType:
		string = "VoucherRedeemedNotPrintedRedeemedAgainWithDiffAmtType";
		break;
	case VoucherRedeemedWithStatus0567:
		string = "VoucherRedeemedWithStatus0567";
		break;
	case VoucherRedeemedWithStatus0567NotPrinted:
		string = "VoucherRedeemedWithStatus0567NotPrinted";
		break;
	case VoucherRedeemedWithStatus0567AndDiffAmtType:
		string = "VoucherRedeemedWithStatus0567AndDiffAmtType";
		break;
	case VoucherRedeemedWithStatus0567AndDiffAmtTypeNotPrinted:
		string = "VoucherRedeemedWithStatus0567AndDiffAmtTypeNotPrinted";
		break;
	case VoucherRejectedNotLocked:
		string = "VoucherRejectedNotLocked";
		break;
	case VoucherRejectedMultipleEntries:
		string = "VoucherRejectedMultipleEntries";
		break;
	case TicketResultCodeNoValue:
		string = "TicketResultCodeNoValue";
		break;
	default:
		string = FormatString("%d", m_resultCode);
		break;
	}

	return string;
}

std::string CTicket::GetTicketTypeText()
{
	string string;
	switch (m_type)
	{
	case TicketTypeNoValue:
		string = "TicketTypeNoValue";
		break;
	case VoucherTicket:
		string = "VoucherTicket";
		break;
	case CouponPromoGameGenerated:
		string = "CouponPromoGameGenerated";
		break;
	case VoucherPromo:
		string = "VoucherPromo";
		break;
	case CouponPromo:
		string = "CouponPromo";
		break;
	case AttPaidCCReceipt:
		string = "AttPaidCCReceipt";
		break;
	case AttPaidJPReceipt:
		string = "AttPaidJPReceipt";
		break;
	case CashoutNoReceipt:
		string = "CashoutNoReceipt";
		break;
	case SingleWinNoReceipt:
		string = "SingleWinNoReceipt";
		break;
	default:
		string = FormatString("%d", m_type);
		break;
	}

	return string;
}

std::string CTicket::GetTicketInOutText()
{
	string string;
	if (m_bTicketIn)
	{
		string = "In";
	}
	else
	{
		string = "Out";
	}
	return string;
}

std::string CTicket::GetValidationString()
{
	string str = "";
	string byteString;
	for (int i = 0; i < m_validationNumberLength; i++)
	{
		byteString = FormatString("%02X", m_validationNumber[i]);
		str += byteString;
	}

	return str;
}

std::string CTicket::GetExpirationTypeText()
{
	string str;
	if (m_expirationType <= EXPIRES_YEARS)
	{
		str = TicketExpirationTypeStrings[m_expirationType];
	}
	else
	{
		str = "";
	}
	return str;
}

bool CTicket::IsAmountLessThanMax()
{
	// Only return false if amount is larger than max and this is a voucher ticket
	bool retVal = true;
	if (m_type == VoucherTicket && m_amount > CDiagnostics::Instance().GetMaxTicketAmount())
	{
		retVal = false;
	}
	return retVal;
}
