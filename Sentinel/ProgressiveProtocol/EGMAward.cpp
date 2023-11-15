#include "stdafx.h"
#include "EGMAward.h"
#include "metersdef.h"
#include "PlayerSession.h"
#include "RapidJsonHelper.h"
#include "Logging/LogString.h"
#include "ProgressiveKeys.h"
#include "ProgressiveLevelForward.h"
#include "ProgressiveLevelInfo.h"
#include "EnumMap.h"
#include "Util/Currency.h"


using namespace std;
using namespace rapidjson;


// Status info constants.
const LPCTSTR CEGMAward::TEXT_EGM_DOES_NOT_CURRENTLY_SUPPORT_TRANSFER_TYPE(
	_T("EGM does not (currently) support transfer type"));
const LPCTSTR CEGMAward::TEXT_HAND_PAY_PENDING_RESOLVED_BY_GAME_START(
	_T("Hand pay pending resolved by game start"));
const LPCTSTR CEGMAward::TEXT_INTERNALS_CREATED_EGM_AWARD_FROM_BONUS_METER_CHANGE(
	_T("Internals created egmAward from bonus meter change"));
const LPCTSTR CEGMAward::TEXT_INTERNALS_DECLINED_NEW_JACKPOT_HIT_FROM_SYSTEM__AWARD_IN_PROGRESS(
	_T("Internals declined new jackpot hit from System -- award in progress"));
const LPCTSTR CEGMAward::TEXT_INTERNALS_DECLINED_NEW_JACKPOT_HIT_FROM_SYSTEM__NO_TRANSFER_CAPABILITY(
	_T("Internals declined new jackpot hit from System -- no transfer capability"));
const LPCTSTR CEGMAward::TEXT_INTERNALS_EXPIRED_EGM_AWARD__SENDING_TO_GCF_TO_BE_CLEARED_THERE_FIRST(
	_T("Internals expired egmAward. Sending to GCF to be cleared there first."));
const LPCTSTR CEGMAward::TEXT_INTERNALS_INITIALIZED_INVALID_AWARD(
	_T("Internals initialized invalid award"));
const LPCTSTR CEGMAward::TEXT_INTERNALS_FAILED_NEW_PROMO_AWARD_FROM_SYSTEM_AWARD_IN_PROGRESS(
	_T("Internals failed new promo award from System -- award in progress"));
const LPCTSTR CEGMAward::TEXT_INTERNALS_RECEIVED_INVALID_JACKPOT_HIT(
	_T("Internals received invalid jackpot hit"));
const LPCTSTR CEGMAward::TEXT_INTERNALS_RECEIVED_NEW_JACKPOT_HIT_FROM_SYSTEM_WHILE_PROGRESSIVE_DOWN(
	_T("Internals received new jackpot hit from System while progressive down"));
const LPCTSTR CEGMAward::TEXT_RESTART_OCCURRED_WITH_LEGACY_BONUSING_TRANSFER(
	_T("Restart occurred with legacy bonusing transfer"));
const LPCTSTR CEGMAward::TEXT_SAS_AFT_BONUS_AWARD_AMOUNT_INVALID(
	_T("SAS AFT bonus award amount invalid"));
const LPCTSTR CEGMAward::TEXT_SAS_AFT_ERROR_GETTING_BONUS_TRANSFER_STATUS(
	_T("SAS AFT ERROR getting Bonus Transfer status"));
const LPCTSTR CEGMAward::TEXT_SAS_AFT_ERROR_TIMED_OUT_WAITING_FOR_LOCK(
	_T("SAS AFT ERROR timed out waiting for lock"));
const LPCTSTR CEGMAward::TEXT_SAS_DENOM_CHECK_FAILED(
	_T("SAS denom check failed"));
const LPCTSTR CEGMAward::TEXT_SAS_DENOM_MISMATCH_ERROR(
	_T("SAS denom mismatch error"));
const LPCTSTR CEGMAward::TEXT_SAS_LEGACY_BONUS_AWARD_AMOUNT_INVALID(
	_T("SAS legacy bonus award amount invalid"));
const LPCTSTR CEGMAward::TEXT_SAS_LEGACY_BONUS_POLL_FAILED(
	_T("SAS legacy bonus poll failed"));
const LPCTSTR CEGMAward::TEXT_SAS_LEGACY_BONUS_SIGNALED(
	_T("SAS legacy bonus signaled"));
const LPCTSTR CEGMAward::TEXT_SAS_MACHINE_INFO_LONG_POLL_ERROR__DENOM_CHECK_FAILED(
	_T("SAS machine info long poll error -- denom check failed"));
const LPCTSTR CEGMAward::TEXT_SAS_NO_BONUSING_IS_AVAILABLE_ON_THIS_EGM(
	_T("SAS no bonusing is available on this EGM"));
const LPCTSTR CEGMAward::TEXT_SETTING_AWARD_AS_HANDPAY_PENDING_AFTER_A_JACKPOT_IS_SET(
	_T("Setting award as handpay pending after a jackpot is set"));

// Status info constants.  AFT results (in numeric order).
const LPCTSTR CEGMAward::TEXT_AFT_TRANSACTION_SUCCESSFUL(
	_T("AFT Transaction successful"));
const LPCTSTR CEGMAward::TEXT_AFT_TRANSFER_STATUS_PENDING_IN_GETAWARDRESULTFROMTRANSFERSTATUS(
	_T("AFT TRANSFER_STATUS_PENDING in GetAwardResultFromTransferStatus"));
const LPCTSTR CEGMAward::TEXT_AFT_TRANSFER_CANCELED_BY_HOST(
	_T("AFT Transfer canceled by host"));
const LPCTSTR CEGMAward::TEXT_AFT_TRANSACTION_ID_NOT_UNIQUE__SUCCESS_AFTER_RESTART(
	_T("AFT Transaction ID not unique -- success after restart"));
const LPCTSTR CEGMAward::TEXT_AFT_NOT_A_VALID_TRANSFER_FUNCTION(
	_T("AFT Not a valid transfer function"));
const LPCTSTR CEGMAward::TEXT_AFT_NOT_A_VALID_TRANSFER_AMOUNT_OR_EXPIRATION(
	_T("AFT Not a valid transfer amount or expiration"));
const LPCTSTR CEGMAward::TEXT_AFT_TRANSFER_AMOUNT_EXCEEDS_LIMITS(
	_T("AFT Transfer amount exceeds limits"));
const LPCTSTR CEGMAward::TEXT_AFT_TRANSFER_AMOUNT_NOT_EVEN_MULTIPLE_OF_DENOM(
	_T("AFT Transfer amount not even multiple of denom"));
const LPCTSTR CEGMAward::TEXT_AFT_CANNOT_PERFORM_PARTIAL_TRANSFERS(
	_T("AFT cannot perform partial transfers"));
const LPCTSTR CEGMAward::TEXT_AFT_MACHINE_NOT_REGISTERED(
	_T("AFT machine not registered"));
const LPCTSTR CEGMAward::TEXT_AFT_REGISTRATION_KEY_DOES_NOT_MATCH(
	_T("AFT registration key does not match"));
const LPCTSTR CEGMAward::TEXT_AFT_NO_POS_ID(
	_T("AFT No POS ID"));
const LPCTSTR CEGMAward::TEXT_AFT_NO_WON_CREDITS_FOR_CASHOUT(
	_T("AFT No won credits for cashout"));
const LPCTSTR CEGMAward::TEXT_AFT_NO_DENOM_SET(
	_T("AFT No denom set"));
const LPCTSTR CEGMAward::TEXT_AFT_EXPIRATION_NOT_VALID(
	_T("AFT Expiration not valid"));
const LPCTSTR CEGMAward::TEXT_AFT_TICKET_DEVICE_NOT_AVAILABLE(
	_T("AFT ticket device not available"));
const LPCTSTR CEGMAward::TEXT_AFT_RESTRICTED_AMOUNTS_FROM_DIFFERENT_POOLS(
	_T("AFT Restricted amounts from different pools"));
const LPCTSTR CEGMAward::TEXT_AFT_UNABLE_TO_PRINT_TRANSACTION_RECEIPT_DEVICE_UNAVAILABLE(
	_T("AFT Unable to print transaction receipt (device unavailable)"));
const LPCTSTR CEGMAward::TEXT_AFT_INSUFFICIENT_DATA_TO_PRINT_TRANSACTION_RECEIPT(
	_T("AFT Insufficient data to print transaction receipt"));
const LPCTSTR CEGMAward::TEXT_AFT_TRANSACTION_RECEIPT_NOT_ALLOWED_FOR_TRANSFER_TYPE(
	_T("AFT transaction receipt not allowed for transfer type"));
const LPCTSTR CEGMAward::TEXT_AFT_ASSET_NUMBER_0_OR_DOES_NOT_MATCH(
	_T("AFT Asset number 0 or does not match"));
const LPCTSTR CEGMAward::TEXT_AFT_TRANSACTION_ID_NOT_VALID(
	_T("AFT Transaction ID not valid"));
const LPCTSTR CEGMAward::TEXT_AFT_UNEXPECTED_ERROR(
	_T("AFT Unexpected error"));
const LPCTSTR CEGMAward::TEXT_AFT_NOT_COMPATIBLE_WITH_CURRENT_TRANSFER_IN_PROGRESS(
	_T("AFT Not compatible with current transfer in progress"));
const LPCTSTR CEGMAward::TEXT_AFT_UNSUPPORTED_TRANSFER_CODE(
	_T("AFT Unsupported transfer code"));
const LPCTSTR CEGMAward::TEXT_AFT_NO_TRANSFER_INFO_AVAILABLE(
	_T("AFT No transfer info available"));
const LPCTSTR CEGMAward::TEXT_AFT_UNKNOWN_ERROR(
	_T("AFT Unknown error"));

// Status info formatting constants.
const LPCTSTR CEGMAward::FORMAT_INT__EGM_AWARD_DOES_NOT_MATCH_CURRENT_SESSION_ID(
	_T("egmAward does not match current sessionId=%d"));

CEGMAward::CEGMAward()
{
	Init();
}

CEGMAward::CEGMAward(const SYSTEMTIME& receivedTime, const Document& jsonDoc)
	: m_awardAmountPaid(0)
	, m_awardState(ReceivedFromSystem)
	, m_levelId(JsonGetIntEx(jsonDoc, pKeyLevelId))
	, m_sasLevelId(InvalidSasLevel)
	, m_jackpotId(JsonGetInt64Ex(jsonDoc, pKeyJackpotId))
	, m_remoteJackpotId(JsonGetInt64Ex(jsonDoc, pKeyRemoteJackpotId))
	, m_awardAmountHit(Currency::ToInt64(JsonGetDoubleEx(jsonDoc, pKeyAmount)))
	, m_originalMachineBonusMeterValue(INVALID_METER_VALUE)
	, m_originalAttendantBonusingMeterValue(INVALID_METER_VALUE)
	, m_originalLegacyBonusMeterValue(INVALID_METER_VALUE)
	, m_awardTransferType(EGMAwardTransferType_None)
	, m_isStandardProgressive(strcmp(JsonGetStringEx(jsonDoc, pKeyJackpotType), JackpotTypes::pStandardProgressive) == 0)
	, m_sessionId(JsonGetOptionalIntEx(jsonDoc, pKeySessionId))
	, m_sentToGameCount(0)
	, m_receivedFromSystemTime(receivedTime)
	, m_duplicateHitCount(0)
	, m_otherHitCount(0)
	, m_paidType(EGMAwardPaidType_Unknown)
	, m_isHitReceived(false)
	, m_isHandpayPendingSignaled(false)
	, m_isEgmPaidSignaled(false)
	, m_methodOfPayment(MethodOfPayment::emap.At(JsonGetString(jsonDoc, pKeyMethodOfPayment), DEFAULT_METHOD_OF_PAYMENT))
	, m_active(true)
	, m_awardType(AwardType::emap.At(JsonGetString(jsonDoc, pKeyAwardType), DEFAULT_AWARD_TYPE))
	, m_fromPromoGame(false)
{
	ZeroMemory(&m_sentToGameTime, sizeof(m_sentToGameTime));
	ZeroMemory(&m_receivedFromEGMTime, sizeof(m_receivedFromEGMTime));
	if (!m_sessionId)
		m_sessionId.val = SESSION_ID_DEFAULT;
	LogInfo(_T("jj Jackpot Hit Received from System"));
}

CEGMAward::CEGMAward(MEMORY_TYPE memoryType, bool buildFromNvram, DWORD nvramActiveOffset)	:
	CMemoryStorageEx(memoryType,
		nvramActiveOffset + NVRAM_EGM_AWARD_CHECKBYTE_SIZE,
		NVRAM_DATA_SIZE,
		nvramActiveOffset,
		NVRAM_ACTIVE_VALUE)
{
	Init();

	if (buildFromNvram)
	{
		BuildYourself();
	}
}

CEGMAward::~CEGMAward()
{
}

void CEGMAward::Init()
{
	m_awardAmountPaid = 0;
	m_awardState = NoAward;
	m_sentToGameCount = 0;
	ZeroMemory(&m_sentToGameTime, sizeof(m_sentToGameTime));
	m_originalMachineBonusMeterValue = INVALID_METER_VALUE;
	m_originalAttendantBonusingMeterValue = INVALID_METER_VALUE;
	m_originalLegacyBonusMeterValue = INVALID_METER_VALUE;

	m_levelId = 0;
	m_sasLevelId = InvalidSasLevel;
	m_jackpotId = DEFAULT_JACKPOT_ID;
	m_remoteJackpotId = DEFAULT_REMOTE_JACKPOT_ID;
	m_awardAmountHit = 0;

	ZeroMemory(&m_receivedFromSystemTime, sizeof(m_receivedFromSystemTime));
	ZeroMemory(&m_receivedFromEGMTime, sizeof(m_receivedFromEGMTime));
	m_duplicateHitCount = 0;
	m_otherHitCount = 0;
	m_awardTransferType = EGMAwardTransferType_None;
	m_paidType = EGMAwardPaidType_Unknown;
	m_isStandardProgressive = false;
	m_sessionId.isValid = false;
	m_sessionId.val = SESSION_ID_DEFAULT;
	m_statusInfo="";

	m_isHitReceived = false;
	m_isHandpayPendingSignaled = false;
	m_isEgmPaidSignaled = false;
	m_methodOfPayment = DEFAULT_METHOD_OF_PAYMENT;
	m_active = true;
	m_awardType = DEFAULT_AWARD_TYPE;
	m_fromPromoGame = false;
}

// Use the default constructor of the parent class.
CEGMAward::CEGMAward(const CEGMAward &rhs)	:
	CMemoryStorageEx(MEMORY_NONE,
		 NVRAM_EGM_AWARD_CHECKBYTE_SIZE,
		0,
		0,
		false)
{
	*this = rhs;
}

CEGMAward& CEGMAward::operator=(const CEGMAward &rhs)
{
	if (this != &rhs)
	{
		m_awardAmountPaid = rhs.m_awardAmountPaid;
		m_awardState = rhs.m_awardState;
		m_sentToGameCount = rhs.m_sentToGameCount;
		m_sentToGameTime = rhs.m_sentToGameTime;
		m_originalMachineBonusMeterValue = rhs.m_originalMachineBonusMeterValue;
		m_originalAttendantBonusingMeterValue = rhs.m_originalAttendantBonusingMeterValue;
		m_originalLegacyBonusMeterValue = rhs.m_originalLegacyBonusMeterValue;

		m_levelId = rhs.m_levelId;
		m_sasLevelId = rhs.m_sasLevelId;
		m_jackpotId = rhs.m_jackpotId;
		m_remoteJackpotId = rhs.m_remoteJackpotId;
		m_awardAmountHit = rhs.m_awardAmountHit;

		m_receivedFromSystemTime = rhs.m_receivedFromSystemTime;
		m_receivedFromEGMTime = rhs.m_receivedFromEGMTime;
		m_duplicateHitCount = rhs.m_duplicateHitCount;
		m_otherHitCount = rhs.m_otherHitCount;
		m_awardTransferType = rhs.m_awardTransferType;
		m_paidType = rhs.m_paidType;
		m_isStandardProgressive = rhs.m_isStandardProgressive;
		m_sessionId = rhs.m_sessionId;
		m_statusInfo = rhs.m_statusInfo;

		m_isHitReceived = rhs.m_isHitReceived;
		m_isHandpayPendingSignaled = rhs.m_isHandpayPendingSignaled;
		m_isEgmPaidSignaled = rhs.m_isEgmPaidSignaled;
		m_receivedFromEGMTime = rhs.m_receivedFromEGMTime;
		m_methodOfPayment = rhs.m_methodOfPayment;
		m_active = rhs.m_active;
		m_awardType = rhs.m_awardType;
		m_fromPromoGame = rhs.m_fromPromoGame;
	}

	return *this;
}

void CEGMAward::SetMemoryStorage(DWORD nvramActiveOffset)
{
	if (nvramActiveOffset != NVRAM_DEFAULT_OFFSET)
	{
		SetMemoryStorageParameters(MEMORY_NVRAM,
			nvramActiveOffset + NVRAM_EGM_AWARD_CHECKBYTE_SIZE,
			NVRAM_DATA_SIZE,
			nvramActiveOffset,
			NVRAM_ACTIVE_VALUE);
		StoreYourself();
	}
	else
	{
		ClearMemoryStorageParameters();
	}
}

bool CEGMAward::IsValidJackpotId(LONG64 jackpotId)
{
	// For signed 64-bit jackpot IDs, treat any negative value as invalid.
	assert(INVALID_JACKPOT_ID == -1);
	bool valid(jackpotId >= 0);
	return valid;
}

bool CEGMAward::IsValidJackpotHitInitalization()
{
	bool valid(true);

	// Check information received from the progressive system.
	if (!IsValidJackpotId(m_jackpotId) ||
		(m_awardAmountHit <= 0 && m_awardType != AwardType::Prize) ||
		m_awardType == AwardType::Offer ||
		(m_methodOfPayment != MethodOfPayment::PayToGame && m_methodOfPayment != MethodOfPayment::PayToGameWithHandPay))
	{
		valid = false;

        stringstream sStream;
		std::string statusInfo(CEGMAward::TEXT_INTERNALS_RECEIVED_INVALID_JACKPOT_HIT);
        sStream << ": jackpotId=" << (long long)m_jackpotId << " awardAmountHit=" << (long long)m_awardAmountHit << " methodOfPayment=" << (int)m_methodOfPayment;
        statusInfo += sStream.str();
		SetStatusInfo(statusInfo);
	}

	// Check initial setup.
	if (m_awardState != ReceivedFromSystem ||
		m_originalMachineBonusMeterValue != INVALID_METER_VALUE ||
		m_originalAttendantBonusingMeterValue != INVALID_METER_VALUE ||
		m_originalLegacyBonusMeterValue != INVALID_METER_VALUE ||
		m_awardTransferType != EGMAwardTransferType_None)
	{
		valid = false;

        stringstream sStream;
		std::string statusInfo(CEGMAward::TEXT_INTERNALS_INITIALIZED_INVALID_AWARD);
        sStream << ": awardState=" << (int)m_awardState << " origMachBonusMeter=" << (long long)m_originalMachineBonusMeterValue << " origAttnBonusMeter=" << (long long)m_originalAttendantBonusingMeterValue <<" awardTransferType=" << (int)m_awardTransferType;
        statusInfo += sStream.str();
		SetStatusInfo(statusInfo);
	}

	return valid;
}

bool CEGMAward::IsSameHit(CEGMAward const &rhs) const
{
	bool same(true);
	
	if (same)
	{
		same = m_jackpotId == rhs.m_jackpotId;
	}

	return same;
}

void CEGMAward::SetInactiveLocation()
{
	if (!CMemoryStorage::SetInactiveLocation(NVRAM_ACTIVE_OFFSET, (BYTE)~NVRAM_ACTIVE_VALUE))
	{
		LogString(ERROR_LOG, _T("Could not write EGM inactive check byte to NVRAM"));
	}
}

void CEGMAward::ValidateSystemTime(SYSTEMTIME& systemTime)
{
	bool valid =
		(systemTime.wYear == 0 &&
		systemTime.wMonth == 0 &&
		systemTime.wDay == 0 &&
		systemTime.wHour == 0 &&
		systemTime.wMinute == 0 &&
		systemTime.wSecond == 0 &&
		systemTime.wMilliseconds == 0)
		||
		(systemTime.wYear >= 1900 &&
		systemTime.wMonth >= 1 && systemTime.wMonth <= 12 &&
		systemTime.wDay >= 1 && systemTime.wDay <= 31 &&
		systemTime.wHour <= 23 &&
		systemTime.wMinute <= 59 &&
		systemTime.wSecond <= 59);
			

	if (!valid)
	{
		ZeroMemory(&systemTime, sizeof(systemTime));
	}

}

void CEGMAward::SetFromNvramBuffer(const BYTE *buffer, UINT bufferSize, bool active)
{
	m_active = active;

	size_t offset(0);
	bool ok(true);

	CUtilities::MemCopyZero_FromOffset(&ok, &m_awardAmountPaid, sizeof (m_awardAmountPaid), buffer, bufferSize, &offset);
	CUtilities::MemCopyZero_FromOffset(&ok, &m_awardState, sizeof (m_awardState), buffer, bufferSize, &offset);
	CUtilities::MemCopyZero_FromOffset(&ok, &m_sentToGameCount, sizeof (m_sentToGameCount), buffer, bufferSize, &offset);
	CUtilities::MemCopyZero_FromOffset(&ok, &m_sentToGameTime, sizeof (m_sentToGameTime), buffer, bufferSize, &offset);
	CUtilities::MemCopyZero_FromOffset(&ok, &m_originalMachineBonusMeterValue, sizeof (m_originalMachineBonusMeterValue), buffer, bufferSize, &offset);
	CUtilities::MemCopyZero_FromOffset(&ok, &m_originalAttendantBonusingMeterValue, sizeof (m_originalAttendantBonusingMeterValue), buffer, bufferSize, &offset);

	CUtilities::MemCopyZero_FromOffset(&ok, &m_levelId, sizeof (m_levelId), buffer, bufferSize, &offset);
	CUtilities::MemCopyZero_FromOffset(&ok, &m_jackpotId, sizeof (m_jackpotId), buffer, bufferSize, &offset);
	CUtilities::MemCopyZero_FromOffset(&ok, &m_remoteJackpotId, sizeof (m_remoteJackpotId), buffer, bufferSize, &offset);
	CUtilities::MemCopyZero_FromOffset(&ok, &m_awardAmountHit, sizeof (m_awardAmountHit), buffer, bufferSize, &offset);

	CUtilities::MemCopyZero_FromOffset(&ok, &m_receivedFromSystemTime, sizeof (m_receivedFromSystemTime), buffer, bufferSize, &offset);
	CUtilities::MemCopyZero_FromOffset(&ok, &m_duplicateHitCount, sizeof (m_duplicateHitCount), buffer, bufferSize, &offset);
	CUtilities::MemCopyZero_FromOffset(&ok, &m_otherHitCount, sizeof (m_otherHitCount), buffer, bufferSize, &offset);
	CUtilities::MemCopyZero_FromOffset(&ok, &m_awardTransferType, sizeof (m_awardTransferType), buffer, bufferSize, &offset);

	// If extra check bytes are not correct, leave extra members set to previous (default) values.
	byte checkByte2 = ~NVRAM_EGM_AWARD_CHECKBYTE2_VALUE;
	byte checkByte3 = ~NVRAM_EGM_AWARD_CHECKBYTE3_VALUE;
	CUtilities::MemCopyZero_FromOffset(&ok, &checkByte2, sizeof(checkByte2), buffer, bufferSize, &offset);
	if (ok && checkByte2 == NVRAM_EGM_AWARD_CHECKBYTE2_VALUE)
	{
		// v1.50.0 extension(s).
		CUtilities::MemCopyZero_FromOffset(&ok, &m_paidType, sizeof(m_paidType), buffer, bufferSize, &offset);

		CUtilities::MemCopyZero_FromOffset(&ok, &m_isStandardProgressive, sizeof(m_isStandardProgressive), buffer, bufferSize, &offset);
		CUtilities::MemCopyZero_FromOffset(&ok, &m_sessionId.isValid, sizeof(m_sessionId.isValid), buffer, bufferSize, &offset);
		CUtilities::MemCopyZero_FromOffset(&ok, &m_sessionId.val, sizeof(m_sessionId.val), buffer, bufferSize, &offset);

		CUtilities::MemCopyZero_FromOffset(&ok, &m_sasLevelId, sizeof(m_sasLevelId), buffer, bufferSize, &offset);
		CUtilities::MemCopyZero_FromOffset(&ok, &m_isHitReceived, sizeof(m_isHitReceived), buffer, bufferSize, &offset);
		CUtilities::MemCopyZero_FromOffset(&ok, &m_isHandpayPendingSignaled, sizeof(m_isHandpayPendingSignaled), buffer, bufferSize, &offset);
		CUtilities::MemCopyZero_FromOffset(&ok, &m_isEgmPaidSignaled, sizeof(m_isEgmPaidSignaled), buffer, bufferSize, &offset);
		CUtilities::MemCopyZero_FromOffset(&ok, &m_receivedFromEGMTime, sizeof(m_receivedFromEGMTime), buffer, bufferSize, &offset);
		CUtilities::MemCopyZero_FromOffset(&ok, &m_methodOfPayment, sizeof(m_methodOfPayment), buffer, bufferSize, &offset);

		// Check to see if the next extra checkbyte is set.
		CUtilities::MemCopyZero_FromOffset(&ok, &checkByte3, sizeof(checkByte3), buffer, bufferSize, &offset);
	}
	else
	{
		// Set value(s) for backwards compatibility.
		ok = false;
		m_methodOfPayment = UPGRADE_METHOD_OF_PAYMENT;

		// If record is inactive and the (first) check byte is not ok then clear the record, as the record is not useful for diagnostics.
		if (!active)
		{
			Init();
		}
	}

	// if the checkbyte3 is set, get the next set of extra data. Otherwise set to default value.
	if (ok && checkByte3 == NVRAM_EGM_AWARD_CHECKBYTE3_VALUE)
	{
		// v1.51.0 extension(s).
		CUtilities::MemCopyZero_FromOffset(&ok, &m_originalLegacyBonusMeterValue, sizeof(m_originalLegacyBonusMeterValue), buffer, bufferSize, &offset);
		
		// v1.58/v2.7 extension(s).
		CUtilities::MemCopyZero_FromOffset(&ok, &m_awardType, sizeof(m_awardType), buffer, bufferSize, &offset);
		CUtilities::MemCopyZero_FromOffset(&ok, &m_fromPromoGame, sizeof(m_fromPromoGame), buffer, bufferSize, &offset);
	}
	else
	{
		ok = false;
		m_originalLegacyBonusMeterValue = INVALID_METER_VALUE;
		m_awardType = DEFAULT_AWARD_TYPE;
		m_fromPromoGame = false;
	}


	m_statusInfo = _T("Restart occurred");

	ValidateSystemTime(m_sentToGameTime);
	ValidateSystemTime(m_receivedFromSystemTime);
	ValidateSystemTime(m_receivedFromEGMTime);
}

UINT CEGMAward::GetNvramBuffer(BYTE *buffer, UINT bufferSize, bool& active) const
{
	size_t offset(0);
	bool ok(true);

	active = m_awardState != NoAward && m_active;
	if (m_awardState != NoAward)
	{
		CUtilities::MemCopySafe_ToOffset(&ok, buffer, bufferSize, &offset, &m_awardAmountPaid, sizeof (m_awardAmountPaid));
		CUtilities::MemCopySafe_ToOffset(&ok, buffer, bufferSize, &offset, &m_awardState, sizeof (m_awardState));
		CUtilities::MemCopySafe_ToOffset(&ok, buffer, bufferSize, &offset, &m_sentToGameCount, sizeof (m_sentToGameCount));
		CUtilities::MemCopySafe_ToOffset(&ok, buffer, bufferSize, &offset, &m_sentToGameTime, sizeof (m_sentToGameTime));
		CUtilities::MemCopySafe_ToOffset(&ok, buffer, bufferSize, &offset, &m_originalMachineBonusMeterValue, sizeof (m_originalMachineBonusMeterValue));
		CUtilities::MemCopySafe_ToOffset(&ok, buffer, bufferSize, &offset, &m_originalAttendantBonusingMeterValue, sizeof (m_originalAttendantBonusingMeterValue));

		CUtilities::MemCopySafe_ToOffset(&ok, buffer, bufferSize, &offset, &m_levelId, sizeof (m_levelId));
		CUtilities::MemCopySafe_ToOffset(&ok, buffer, bufferSize, &offset, &m_jackpotId, sizeof (m_jackpotId));
		CUtilities::MemCopySafe_ToOffset(&ok, buffer, bufferSize, &offset, &m_remoteJackpotId, sizeof (m_remoteJackpotId));
		CUtilities::MemCopySafe_ToOffset(&ok, buffer, bufferSize, &offset, &m_awardAmountHit, sizeof (m_awardAmountHit));

		CUtilities::MemCopySafe_ToOffset(&ok, buffer, bufferSize, &offset, &m_receivedFromSystemTime, sizeof (m_receivedFromSystemTime));
		CUtilities::MemCopySafe_ToOffset(&ok, buffer, bufferSize, &offset, &m_duplicateHitCount, sizeof (m_duplicateHitCount));
		CUtilities::MemCopySafe_ToOffset(&ok, buffer, bufferSize, &offset, &m_otherHitCount, sizeof (m_otherHitCount));
		CUtilities::MemCopySafe_ToOffset(&ok, buffer, bufferSize, &offset, &m_awardTransferType, sizeof (m_awardTransferType));

		// v1.50.0 extension(s).
		const byte extraCheckByte(NVRAM_EGM_AWARD_CHECKBYTE2_VALUE);
		CUtilities::MemCopySafe_ToOffset(&ok, buffer, bufferSize, &offset, &extraCheckByte, sizeof(extraCheckByte));
		CUtilities::MemCopySafe_ToOffset(&ok, buffer, bufferSize, &offset, &m_paidType, sizeof(m_paidType));

		CUtilities::MemCopySafe_ToOffset(&ok, buffer, bufferSize, &offset, &m_isStandardProgressive, sizeof(m_isStandardProgressive));
		CUtilities::MemCopySafe_ToOffset(&ok, buffer, bufferSize, &offset, &m_sessionId.isValid, sizeof(m_sessionId.isValid));
		CUtilities::MemCopySafe_ToOffset(&ok, buffer, bufferSize, &offset, &m_sessionId.val, sizeof(m_sessionId.val));

		CUtilities::MemCopySafe_ToOffset(&ok, buffer, bufferSize, &offset, &m_sasLevelId, sizeof(m_sasLevelId));
		CUtilities::MemCopySafe_ToOffset(&ok, buffer, bufferSize, &offset, &m_isHitReceived, sizeof(m_isHitReceived));
		CUtilities::MemCopySafe_ToOffset(&ok, buffer, bufferSize, &offset, &m_isHandpayPendingSignaled, sizeof(m_isHandpayPendingSignaled));
		CUtilities::MemCopySafe_ToOffset(&ok, buffer, bufferSize, &offset, &m_isEgmPaidSignaled, sizeof(m_isEgmPaidSignaled));
		CUtilities::MemCopySafe_ToOffset(&ok, buffer, bufferSize, &offset, &m_receivedFromEGMTime, sizeof(m_receivedFromEGMTime));
		CUtilities::MemCopySafe_ToOffset(&ok, buffer, bufferSize, &offset, &m_methodOfPayment, sizeof(m_methodOfPayment));	

		// v1.51.0 extension(s).
		byte checkByte3 = NVRAM_EGM_AWARD_CHECKBYTE3_VALUE;
		CUtilities::MemCopySafe_ToOffset(&ok, buffer, bufferSize, &offset, &checkByte3, sizeof(checkByte3));
		CUtilities::MemCopySafe_ToOffset(&ok, buffer, bufferSize, &offset, &m_originalLegacyBonusMeterValue, sizeof(m_originalLegacyBonusMeterValue));

		// v1.58/v2.8 extension(s).
		CUtilities::MemCopySafe_ToOffset(&ok, buffer, bufferSize, &offset, &m_awardType, sizeof(m_awardType));
		CUtilities::MemCopySafe_ToOffset(&ok, buffer, bufferSize, &offset, &m_fromPromoGame, sizeof(m_fromPromoGame));
	}

	return (UINT)offset;
}

bool CEGMAward::InProgress() const
{
	bool inProgress(false);

	switch (m_awardState)
	{
	case ReceivedFromSystem:
	case WaitForMetersInitial:
	case SentToGame:
	case GameBusy:
	case Signaled:
	case HandpayPending:
	case AftLockPending:
		inProgress = true;
		break;
	default:
		break;
	}

	return inProgress;
}

void CEGMAward::SetAwardAmountPaid(__int64 awardAmountPaid)
{
	m_awardAmountPaid = awardAmountPaid;
}


__int64 CEGMAward::GetAwardAmountPaid() const
{
	return m_awardAmountPaid;
}

__int64 CEGMAward::GetAwardAmount() const
{
	return (m_awardState == AwardToGameSuccess ? m_awardAmountPaid : m_awardAmountHit);
}

void CEGMAward::SetAwardState(EGMAwardState awardState)
{
	m_awardState = awardState;
}

EGMAwardState CEGMAward::GetAwardState() const
{
	return m_awardState;
}

bool CEGMAward::IsAwardStateFinalOrPending() const
{
	bool isFinal(false);

	switch (m_awardState)
	{
	case HandpayPending:
	case AwardToGameSuccess:
	case AwardToGameError:
	case AwardToGameIneligible:
	case AwardSessionMismatch:
		isFinal = true;
		break;
	default:
		break;
	}

	return isFinal;
}

std::string CEGMAward::GetAwardStateString() const
{
	std::string stateString;

	switch (m_awardState)
	{
	case	NoAward:
		stateString = _T("No Award");
		break;
	case ReceivedFromSystem:
		stateString = _T("Received From System");
		break;
	case ReceivedFromGame:
		stateString = _T("Received From Game");
		break;
	case WaitForMetersInitial:
		stateString = _T("Wait For Meters Initial");
		break;
	case SentToGame:
		stateString = _T("Sent To Game");
		break;
	case SentToSystem:
		stateString = _T("Sent To System");
		break;
	case GameBusy:
		stateString = _T("Game Busy");
		break;
	case Signaled:
		stateString = _T("Signaled on Game");
		break;
	case HandpayPending:
		stateString = _T("Handpay is pending");
		break;
	case AwardToGameSuccess:
		stateString = _T("Award To Game Success");
		break;
	case AwardToGameError:
		stateString = _T("Award To Game Error");
		break;
	case AwardToGameIneligible:
		stateString = _T("Award To Game Ineligible");
		break;
	case AftLockPending:
		stateString = _T("Game Lock Pending");
		break;
	case AwardSessionMismatch:
		stateString = _T("Award Session Mismatch");
		break;
	default:
		stateString = _T("N/A");
		break;
	}

	return stateString;
}

void CEGMAward::SetSentToGameCount(int sentToGameCount)
{
	m_sentToGameCount = sentToGameCount;
}

int CEGMAward::GetSentToGameCount() const
{
	return m_sentToGameCount;
}

void CEGMAward::SetSentToGameTime(const SYSTEMTIME& sentToGameTime)
{
	m_sentToGameTime = sentToGameTime;
}

const SYSTEMTIME& CEGMAward::GetSentToGameTime() const
{
	return m_sentToGameTime;
}

void CEGMAward::SetReceivedFromSystemTime(const SYSTEMTIME& receivedFromSystemTime)
{
	m_receivedFromSystemTime = receivedFromSystemTime;
}

const SYSTEMTIME& CEGMAward::GetReceivedFromSystemTime() const
{
	return m_receivedFromSystemTime;
}

void CEGMAward::SetReceivedFromEGMTime(const SYSTEMTIME& receivedFromEGMTime)
{
	m_receivedFromEGMTime = receivedFromEGMTime;
}

const SYSTEMTIME& CEGMAward::GetReceivedFromEGMTime() const
{
	return m_receivedFromEGMTime;
}

void CEGMAward::SetAwardTransferType(EGMAwardTransferType awardTransferType)
{
	m_awardTransferType = awardTransferType;
}

EGMAwardTransferType CEGMAward::GetAwardTransferType() const
{
	return m_awardTransferType;
}

std::string CEGMAward::GetAwardTransferTypeString() const
{
	std::string typeString;

	switch (m_awardTransferType)
	{
	case	EGMAwardTransferType_None:
		typeString = _T("None");
		break;
	case	EGMAwardTransferType_SASLegacyBonusing:
		typeString = _T("SAS Legacy Bonusing");
		break;
	case	EGMAwardTransferType_SASAFTBonusingCash:
		typeString = _T("SAS AFT Bonusing Cash");
		break;
	default:
		typeString = _T("N/A");
		break;
	}

	return typeString;
}

void CEGMAward::SetDuplicateHitCount(DWORD duplicateHitCount)
{
	m_duplicateHitCount = duplicateHitCount;
}

DWORD CEGMAward::GetDuplicateHitCount() const
{
	return m_duplicateHitCount;
}

void CEGMAward::SetOtherHitCount(DWORD otherHitCount)
{
	m_otherHitCount = otherHitCount;
}

DWORD CEGMAward::GetOtherHitCount() const
{
	return m_otherHitCount;
}

void CEGMAward::SetOriginalMachineBonusMeterValue(__int64 originalMachineBonusMeterValue)
{
	m_originalMachineBonusMeterValue = originalMachineBonusMeterValue;
}

__int64 CEGMAward::GetOriginalMachineBonusMeterValue() const
{
	return m_originalMachineBonusMeterValue;
}

void CEGMAward::SetOriginalAttendantBonusingMeterValue(__int64 originalAttendantBonusingMeterValue)
{
	m_originalAttendantBonusingMeterValue = originalAttendantBonusingMeterValue;
}

__int64 CEGMAward::GetOriginalAttendantBonusingMeterValue() const
{
	return m_originalAttendantBonusingMeterValue;
}

void CEGMAward::SetOriginalLegacyBonusMeterValue(__int64 originalLegacyBonusMeterValue)
{
	m_originalLegacyBonusMeterValue = originalLegacyBonusMeterValue;
}

__int64 CEGMAward::GetOriginalLegacyBonusMeterValue() const
{
	return m_originalLegacyBonusMeterValue;
}

void CEGMAward::SetLevelId(int levelId)
{
	m_levelId = levelId;
}

int CEGMAward::GetLevelId() const
{
	return m_levelId;
}

void CEGMAward::SetSASLevelId(int levelId)
{
	m_sasLevelId = levelId;
}

int CEGMAward::GetSASLevelId() const
{
	return m_sasLevelId;
}

void CEGMAward::SetStandardProgressive(bool isStandardProgressive)
{
	m_isStandardProgressive = isStandardProgressive;
}

bool CEGMAward::IsStandardProgressive() const
{
	return m_isStandardProgressive;
}

bool CEGMAward::GetSessionId(int &sessionId) const
{
	sessionId = m_sessionId.val;
	return m_sessionId.isValid;
}

bool CEGMAward::DoesAwardMatchSession(int sessionId) const
{
	bool match = !m_sessionId || sessionId == m_sessionId.val;
	return match;
}

void CEGMAward::SetJackpotId(LONG64 jackpotId)
{
	m_jackpotId = jackpotId;
}

LONG64 CEGMAward::GetJackpotId() const
{
	return m_jackpotId;
}

void CEGMAward::SetRemoteJackpotId(LONG64 remoteJackpotId)
{
	m_remoteJackpotId = remoteJackpotId;
}

LONG64 CEGMAward::GetRemoteJackpotId() const
{
	return m_remoteJackpotId;
}

static EnumSparseMap<EGMAwardState>::EnumPair jackpotPaidStatus[] =
{
	{ Signaled,				JackpotPaidStatus::pSignaled},
	{ HandpayPending,		JackpotPaidStatus::pPending },
	{ AwardSessionMismatch, JackpotPaidStatus::pSessionMismatch },
	{ AwardToGameSuccess,	JackpotPaidStatus::pPaid	},
	{ AwardToGameError,		JackpotPaidStatus::pFailed	},
	{ AwardToGameIneligible,JackpotPaidStatus::pIneligible	}
};

static EnumSparseMap<EGMAwardState> jackpotPaidStatusMap(jackpotPaidStatus);

const char* CEGMAward::GetJackpotPaidStatus() const
{
	return jackpotPaidStatusMap.At(m_awardState, nullptr);
}

void CEGMAward::SetAwardAmountHit(__int64 awardAmountHit)
{
	m_awardAmountHit = awardAmountHit;
}

__int64 CEGMAward::GetAwardAmountHit() const
{
	return m_awardAmountHit;
}

std::string CEGMAward::GetLogInfoString(LPCTSTR description) const
{
    stringstream SStream;
	std::string sessionIdString("N/A");
	if (m_sessionId)
        SStream << (int)m_sessionId.val;
    sessionIdString = SStream.str();
    SStream.str()="";

	std::string logInfoString;
	SStream << "AWARD: rcvd=" << CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(m_receivedFromSystemTime);
	SStream << " state=\"" << GetAwardStateString() << "\"";
	SStream << " lvl=" << (int)m_levelId;
	SStream << " sasLvl=" << (int)m_sasLevelId;
	SStream << " std=" << (int)m_isStandardProgressive;
	SStream << " sesId=" << sessionIdString;
	SStream << " id=" << (long long)m_jackpotId;
	SStream << " hit=" << (long long)m_awardAmountHit;
	SStream << " paid=" << (long long)m_awardAmountPaid;
	SStream << " sent=" << (int)m_sentToGameCount;
	SStream << " sentTime=" << CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(m_sentToGameTime);
	SStream << " machBnsMtr=" << (long long)m_originalMachineBonusMeterValue;
	SStream << " attnBnsMtr=" << (long long)m_originalAttendantBonusingMeterValue;
	SStream << " legcyBnsMtr=" << (long long)m_originalLegacyBonusMeterValue;
	SStream << " dups=" << (int)m_duplicateHitCount;
	SStream << " other=" << (int)m_otherHitCount;
	SStream << " xferType=" << "\"" << GetAwardTransferTypeString() << "\"";
	SStream << " hitRcvd=" <<(int)m_isHitReceived;
	SStream << " hpPend=" << (int)m_isHandpayPendingSignaled;
	SStream << " paidSig=" << (int)m_isEgmPaidSignaled;
	SStream << " fromEgm=" << CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(m_receivedFromEGMTime);
	SStream << " methOfPay=" << (int)m_methodOfPayment;
	SStream << " awardType=" << AwardType::emap.At(m_awardType);
	SStream << " fromPromoGame=" << (int)m_fromPromoGame;
	SStream << " info=" << "\"" <<GetStatusInfo() <<"\"";
	SStream << " -- " << (description != NULL ? description : _T(""));
	logInfoString = SStream.str();
	return logInfoString;
}

void CEGMAward::LogInfo(const std::string& description) const
{
    std::string logInfoString(GetLogInfoString(description.c_str()));
	LogStringNoFormat(PROGRESSIVE, logInfoString);
}


long long CEGMAward::RoundAndSetAwardAmountPaid(WORD denomMult)
{
	long long awardAmount = GetAwardAmountHit();

	if (denomMult > 0)
	{
		if (awardAmount % denomMult > 0)
		{
			awardAmount += (denomMult - (awardAmount % denomMult));
		}
	}

	// Set amount to be paid after rounding
	SetAwardAmountPaid(awardAmount);

	return awardAmount;
}

bool CEGMAward::IsJackpotMatch(__int64 awardAmount)
{
	// Check that status indicates award sent to game and that the awardAmount matches.
	return (m_awardState == SentToGame || m_awardState == Signaled || m_awardState == HandpayPending || m_awardState == AftLockPending) && awardAmount == m_awardAmountHit;
}

void CEGMAward::SetAwardPaidType(EGMAwardPaidType paidType)
{
	m_paidType = paidType;
}

EGMAwardPaidType CEGMAward::GetAwardPaidType() const
{
	return m_paidType;
}

void CEGMAward::SetStatusInfo(const std::string& statusInfo)
{
    if (!statusInfo.empty())
	{
        if (_tcslen(statusInfo.c_str()) > 0)
		{
			m_statusInfo = statusInfo;
		}
	}
}

std::string CEGMAward::GetStatusInfo() const
{
	return m_statusInfo;
}

void CEGMAward::SetHitReceived(bool hitReceived)
{
	m_isHitReceived = hitReceived;
}

bool CEGMAward::isHitReceived() const
{
	return m_isHitReceived;
}

void CEGMAward::setHandpayPendingSignaled(bool handPayPendingSignaled)
{
	m_isHandpayPendingSignaled = handPayPendingSignaled;
}

bool CEGMAward::isHandpayPendingSignaled()	 const
{
	return m_isHandpayPendingSignaled;
}

void CEGMAward::setEgmPaidSignaled(bool egmPaidSignaled)
{
	m_isEgmPaidSignaled = egmPaidSignaled;
}

bool CEGMAward::isEgmPaidSignaled() const
{
	return m_isEgmPaidSignaled;
}

void CEGMAward::SetMethodOfPayment(MethodOfPayment::Type methodOfPayment)
{
	m_methodOfPayment = methodOfPayment;
}

MethodOfPayment::Type CEGMAward::GetMethodOfPayment() const
{
	return m_methodOfPayment;
}

void CEGMAward::ClearActive()
{
	m_active = false;
}

bool CEGMAward::IsActive() const
{
	return m_active;
}

AwardType::Type CEGMAward::GetAwardType() const
{
	return m_awardType;
}

void CEGMAward::SetFromPromoGame(bool value)
{
	m_fromPromoGame = value;
}

bool CEGMAward::IsFromPromoGame() const
{
	return m_fromPromoGame;
}

void CEGMAward::SetFieldsForOneLinkAwardHit(CInternalMessage* msg)
{
	OneLinkAwardType::Type awardType(OneLinkAwardType::PayToGameAward);
	if (AwardType::Prize == m_awardType)
	{
		awardType = OneLinkAwardType::PrizeAward;
	}
	else if (MethodOfPayment::PayToPlayerAccount == m_methodOfPayment)
	{
		awardType = OneLinkAwardType::PlayerAcountAward;
	}
	msg->SetData((WORD)ONE_LINK_AWARD_HIT, (WORD)awardType);
	msg->SetData(GetJackpotId(), GetAwardAmount());
}

bool CEGMAward::SetFieldsForOneLinkAwardStatus(CInternalMessage *msg, AwardType::Type awardType, EGMAwardState awardState, MethodOfPayment::Type methodPayment, int64_t jackpotId, int64_t amount, bool fromWinnerPicked, bool clearedByOneLink)
{
	bool sendStatus(true);

	OneLinkAwardStatus::Type awardStatus(OneLinkAwardStatus::Invalid);

	if (fromWinnerPicked)
	{
		if (MethodOfPayment::PayToPlayerAccount == methodPayment && AwardType::Prize != awardType)
		{
			awardStatus = AwardToGameSuccess == awardState ? OneLinkAwardStatus::PlayerAcountAwardSuccessful : OneLinkAwardStatus::PlayerAcountAwardFailed;
		}
		else
		{
			sendStatus = false;
		}
	}
	else if (AwardToGameSuccess == awardState)
	{
		if (AwardType::Prize == awardType)
		{
			awardStatus = clearedByOneLink ? OneLinkAwardStatus::PrizeAwardClearedByOneLink : OneLinkAwardStatus::PrizeAwardClearedByEmployee;
			amount = 0;
		}
		else
		{
			awardStatus = OneLinkAwardStatus::PayToGameAwardSuccessful;
		}

	}
	else if(Signaled != awardState && HandpayPending != awardState)
	{
		awardStatus = MethodOfPayment::PayToPlayerAccount == methodPayment ? OneLinkAwardStatus::PlayerAcountAwardFailed : OneLinkAwardStatus::PayToGameAwardFailed;
	}
	else
	{
		sendStatus = false;
	}

	msg->SetData(jackpotId, amount);
	msg->SetData((WORD)ONE_LINK_AWARD_STATUS, (WORD)awardStatus);

	return sendStatus;
}

void CEGMAward::GetFieldsForOneLinkTransactionMessage(CInternalMessage* msg, byte& transCode, int64_t& sessionPoints, int64_t& bonusPoints, int64_t& residualValue)
{
	transCode = (byte)msg->GetWordData();
	bonusPoints = msg->GetWordData(true);//awardType or award status
	sessionPoints = msg->GetInt64Data();//jackpotId
	residualValue = msg->GetInt64Data(true);//amount
}