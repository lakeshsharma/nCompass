#include "stdafx.h"
#include "RapidJsonHelper.h"

// Sentinel includes.
#include "Jackpot.h"
#include "Logging/LogString.h"
#include "Utilities.h"

const DWORD SCREEN_DISPLAY_DURATION_IN_MS(3000);

const string UNATTENDED_JACKPOT_UI_STATE("UnattendedJackpotUiState");

namespace UJPScreen
{
	static EnumMap<Type>::EnumPair types[] =
	{
		{ PinEntry,						"PinEntry" },
		{ PinSuccessWithTax,			"PinSuccessWithTax" },
		{ PinSuccessWithoutTax,			"PinSuccessWithoutTax" },
		{ PinError,						"PinError" },
		{ Complete,						"Complete" },
		{ CompleteWithError,			"CompleteWithError" },
		{ CanceledHandpayReset,			"CanceledHandpayReset" },		
		{ ErrorUnknown,					"ErrorUnknown" },
		{ ErrorCreditReset,				"ErrorCreditReset" },
		{ ErrorInsufficientInfo,		"ErrorInsufficientInfo" },
		{ ErrorEGMNotCapable,			"ErrorEGMNotCapable" },
		{ ErrorJackpotAboveLimit,		"ErrorJackpotAboveLimit" },
		{ ErrorAccountLocked,			"ErrorAccountLocked" },
		{ ErrorSessionExpired,			"ErrorSessionExpired" },
		{ ErrorPlayerInVirtualSession,	"ErrorPlayerInVirtualSession" },
		{ CardedTeaser,					"CardedTeaser" },
		{ UncardedTeaser,				"UncardedTeaser" }
	};
	EnumMap<Type> emap(types);
}

namespace UnattendedJackpotType
{
	static EnumMap<Type>::EnumPair types[] =
	{
		{ UJPTYPE_PROCESS							, "UJPTYPE_PROCESS" },
		{ UJPTYPE_CARDED_TEASER						, "UJPTYPE_CARDED_TEASER" },
		{ UJPTYPE_UNCARDED_TEASER					, "UJPTYPE_UNCARDED_TEASER" },
		{ UJPTYPE_CANCEL_INSUFFICIENT_INFO			, "UJPTYPE_CANCEL_INSUFFICIENT_INFO" },
		{ UJPTYPE_CANCEL_EGM_NOT_CAPABLE			, "UJPTYPE_CANCEL_EGM_NOT_CAPABLE" },
		{ UJPTYPE_CANCEL_ABOVE_LIMIT				, "UJPTYPE_CANCEL_ABOVE_LIMIT" },
		{ UJPTYPE_CANCEL_ACCOUNT_LOCKED				, "UJPTYPE_CANCEL_ACCOUNT_LOCKED" },
		{ UJPTYPE_CANCEL_SESSION_EXPIRED			, "UJPTYPE_CANCEL_SESSION_EXPIRED" },
		{ UJPTYPE_CANCEL_PLAYER_IN_VIRTUAL_SESSION	, "UJPTYPE_CANCEL_PLAYER_IN_VIRTUAL_SESSION" },
		{ UJPTYPE_NONE								, "UJPTYPE_NONE" },
	};
	EnumMap<Type> emap(types);
}

namespace UnattendedJackpotPINResponseType
{
	static EnumMap<Type>::EnumPair types[] =
	{
		{ UJPPIN_AUTHORIZED		, "UJPPIN_AUTHORIZED" },
		{ UJPPIN_NOTAUTHORIZED	, "UJPPIN_NOTAUTHORIZED" },
		{ UJPPIN_ACCOUNTLOCKED	, "UJPPIN_ACCOUNTLOCKED" },
	};
	EnumMap<Type> emap(types);
}

namespace UnattendedJackpotState
{
	static EnumMap<Type>::EnumPair types[] =
	{
		{ UJPNone								, "None" },
		{ UJPPinEntry							, "Pin Entry" },
		{ UJPPinSent							, "Pin Sent" },
		{ UJPAuthorized							, "Authorized" },
		{ UJPCreditReset						, "Credit Reset" },
		{ UJPCreditAndRemoteReset				, "Credit and Remote Reset" },
		{ UJPCanceled							, "Canceled" },
		{ UJPFailed								, "Failed" },
		{ UJPCanceledHandpayReset				, "CanceledHandpayReset" },
		{ UJPCanceledReboot						, "CanceledReboot" },
		{ UJPCanceledVirtualPlayer				, "CanceledVirtualPlayer" },
		{ UJPCanceledTimedOut					, "CanceledTimedOut" },
		{ UJPCanceledTaskbarButtonPress			, "CanceledTaskbarButtonPress" },
		{ UJPCanceledCloseMediaWindow			, "CanceledCloseMediaWindow" },
		{ UJPCanceledAccountLocked				, "CanceledAccountLocked" },
		{ UJPCanceledPlayerIdMismatch			, "CanceledPlayerIdMismatch" },
		{ UJPCanceledInsufficientInfo			, "CanceledInsufficientInfo" },
		{ UJPCanceledEgmNotCapable				, "CanceledEgmNotCapable" },
		{ UJPCanceledAboveLimit					, "CanceledAboveLimit" },
		{ UJPCanceledCallHost					, "CanceledCallHost" },
		{ UJPPinError							, "Pin Error" },
		{ UJPCardedTeaser						, "CardedTeaser" },
		{ UJPUncardedTeaser						, "UncardedTeaser" },
	};
	EnumMap<Type> emap(types);
}

namespace UnattendedJackpotStatus
{
	static EnumMap<Type>::EnumPair types[] =
	{
		{ UJPStatusCanceled								, "Canceled" },
		{ UJPStatusFailed								, "Failed" },
		{ UJPStatusSuccessKeyToCredit					, "SuccessKeyToCredit" },
		{ UJPStatusSuccessKeyToCreditRemoteReset		, "SuccessKeyToCreditRemoteReset" },
		{ UJPStatusCanceledHandpayReset					, "CanceledHandpayReset" },
		{ UJPStatusCanceledReboot						, "CanceledReboot" },
		{ UJPStatusCanceledVirtualPlayer				, "CanceledVirtualPlayer" },
		{ UJPStatusCanceledTimedOut						, "CanceledTimedOut" },
		{ UJPStatusCanceledTaskbarButtonPress			, "CanceledTaskbarButtonPress" },
		{ UJPStatusCanceledCloseMediaWindow				, "CanceledCloseMediaWindow" },
		{ UJPStatusCanceledAccountLocked				, "CanceledAccountLocked" },
		{ UJPStatusCanceledPlayerIdMismatch				, "CanceledPlayerIdMismatch" },
		{ UJPStatusCanceledInsufficientInfo				, "CanceledInsufficientInfo" },
		{ UJPStatusCanceledEgmNotCapable				, "CanceledEgmNotCapable" },
		{ UJPStatusCanceledAboveLimit					, "CanceledAboveLimit" },
		{ UJPStatusCanceledCallHost						, "CanceledCallHost" },
	};
	EnumMap<Type> emap(types);
}

CJackpot::CJackpot(MEMORY_TYPE memoryType, bool bBuildFromNVRAM, IOperatingSystem& operatingSystem) : CDataSubscriptionProvider(UNATTENDED_JACKPOT_UI_STATE),
m_operatingSystem(operatingSystem)
{
	m_type = memoryType;
	m_dwAllowedSize = NVRAM_JACKPOT_SIZE;
	m_dwOffset = NVRAM_JACKPOT_OFFSET;
	m_bOffsetIsSet = true;
	m_ujpScreen = UJPScreen::PinEntry;
	m_jackpotAmount = 0;
	m_jpType = JT_NONE;
	m_transType = JTRANS_COMPOSITE;
	m_progressiveLevel = 0;
	m_coinsPlayed = 0;
	m_jackpotTransactionSent = false;
	m_keyToCreditAvailable = false;
	m_unattendedState = UnattendedJackpotState::UJPNone;
	m_unattendedStateTimestamp = 0;
	m_jackpotID = 0;
	memset(m_cardID, 0, CARD_ID_LEN);
	//Initialilly m_handpayReset is false
	m_handpayReset = false;
	m_taxWithholdingEnabled = false;
	m_federalTaxWithholdingPercent = 0;
	m_stateTaxWithholdingPercent = 0;
	m_federalTaxOverridden = false;
	m_stateTaxOverridden = false;
	m_federalTaxWithholding = 0;
	m_stateTaxWithholding = 0;

	m_displayStartTickCount = 0;

	if (memoryType == MEMORY_NVRAM)
	{
		if (bBuildFromNVRAM)
		{
			BuildYourself();
		}
		else
		{
			StoreToNVRAM(__FUNCTION__);
		}

		SetJackpotExistanceToNVRAM(true);
	}
}

CJackpot::CJackpot(CJackpot& jackpotRHS) : CDataSubscriptionProvider(UNATTENDED_JACKPOT_UI_STATE),
m_operatingSystem(COperatingSystem::m_operatingSystem)
{
	m_type = MEMORY_NONE;
	m_dwAllowedSize = NVRAM_JACKPOT_SIZE;
	m_dwOffset = NVRAM_JACKPOT_OFFSET;
	m_bOffsetIsSet = true;
	m_jackpotTransactionSent = false;

	*this = jackpotRHS;
}

CJackpot::~CJackpot()
{
	SetJackpotExistanceToNVRAM(false);
}

CJackpot& CJackpot::operator =(CJackpot& jackpotRHS)
{
	m_pin = jackpotRHS.GetPlayerPin();
	m_jackpotAmount = jackpotRHS.GetJackpotAmount();
	m_ujpScreen = jackpotRHS.m_ujpScreen;
	m_jpType = jackpotRHS.GetJackpotType();
	m_transType = jackpotRHS.GetJackpotTransactionType();
	m_coinsPlayed = jackpotRHS.GetCoinsPlayed();
	m_progressiveLevel = jackpotRHS.GetProgressiveLevel();
	m_jackpotID = jackpotRHS.GetJackpotID();
	m_keyToCreditAvailable = jackpotRHS.IsKeyToCreditAvailable();
	m_unattendedState = jackpotRHS.GetUnattendedState();
	m_unattendedStateTimestamp = jackpotRHS.GetUnattendedStateTimestamp();
	memcpy(m_cardID, jackpotRHS.GetCardID(), CARD_ID_LEN);
	m_handpayReset = jackpotRHS.GetHandPayReset();
	m_taxWithholdingEnabled = jackpotRHS.IsTaxWithholdingEnabled();
	m_federalTaxWithholdingPercent = jackpotRHS.GetFederalTaxWithholdingPercent();
	m_stateTaxWithholdingPercent = jackpotRHS.GetStateTaxWithholdingPercent();
	m_federalTaxWithholding = jackpotRHS.GetFederalTaxWithholding();
	m_stateTaxWithholding = jackpotRHS.GetStateTaxWithholding();
	m_federalTaxOverridden = jackpotRHS.IsFederalTaxOverriden();
	m_stateTaxOverridden = jackpotRHS.IsStateTaxOverriden();
	m_jackpotTransactionSent = jackpotRHS.IsTransactionSent();
	StoreToNVRAM(__FUNCTION__);
	return *this;
}

string CJackpot::GetSubscriptionDataResponse(const std::string& topic, const std::string& whereId)
{
	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);
	
	SetScreenToDisplay();

	writer.StartObject();
	writer.Key("UjpScreen");	writer.String(UJPScreen::emap.At(m_ujpScreen));
	writer.EndObject();

	return writeStream.m_str;
}

void CJackpot::SetJackpotData(const byte* cardID, int cardLength, bool keyToCreditAvailable, DWORD coinsPlayed)
{
	if (cardID != nullptr && cardLength > 0 && cardLength <= CARD_ID_LEN)
	{
		memcpy(m_cardID, cardID, cardLength);
	}
	m_keyToCreditAvailable = keyToCreditAvailable;
	m_coinsPlayed = coinsPlayed;
	StoreToNVRAM(__FUNCTION__);
}

void CJackpot::BuildYourself()
{
	if (m_type == MEMORY_NVRAM)
	{
		byte nvramBuffer[NVRAM_JACKPOT_DATA_SIZE];

		if (!Read(NVRAM_JACKPOT_OFFSET + NVRAM_JACKPOT_CHECKBYTE_SIZE, nvramBuffer, NVRAM_JACKPOT_DATA_SIZE))
			LogString(ERROR_LOG, "Could not read jackpot data from NVRAM");
		byte* bufferPos = nvramBuffer;
		memcpy(&m_jpType, bufferPos, sizeof(m_jpType));
		bufferPos += sizeof(m_jpType);
		memcpy(&m_transType, bufferPos, sizeof(m_transType));
		bufferPos += sizeof(m_transType);
		memcpy(&m_jackpotAmount, bufferPos, sizeof(m_jackpotAmount));
		bufferPos += sizeof(m_jackpotAmount);
		memcpy(&m_progressiveLevel, bufferPos, sizeof(m_progressiveLevel));
		bufferPos += sizeof(m_progressiveLevel);
		memcpy(&m_coinsPlayed, bufferPos, sizeof(m_coinsPlayed));
		bufferPos += sizeof(m_coinsPlayed);
		memcpy(&m_jackpotID, bufferPos, sizeof(m_jackpotID));
		bufferPos += sizeof(m_jackpotID);
		memcpy(m_cardID, bufferPos, CARD_ID_LEN);
		bufferPos += CARD_ID_LEN;
		memcpy(&m_keyToCreditAvailable, bufferPos, sizeof(m_keyToCreditAvailable));
		bufferPos += sizeof(m_keyToCreditAvailable);
		memcpy(&m_unattendedState, bufferPos, sizeof(m_unattendedState));
		bufferPos += sizeof(m_unattendedState);
		memcpy(&m_jackpotTransactionSent, bufferPos, sizeof(m_jackpotTransactionSent));
		bufferPos += sizeof(m_jackpotTransactionSent);
		memcpy(&m_handpayReset, bufferPos, sizeof(m_handpayReset));
		bufferPos += sizeof(m_handpayReset);
		memcpy(&m_federalTaxWithholding, bufferPos, sizeof(m_federalTaxWithholding));
		bufferPos += sizeof(m_federalTaxWithholding);
		memcpy(&m_stateTaxWithholding, bufferPos, sizeof(m_stateTaxWithholding));
		bufferPos += sizeof(m_stateTaxWithholding);
		memcpy(&m_federalTaxOverridden, bufferPos, sizeof(m_federalTaxOverridden));
		bufferPos += sizeof(m_federalTaxOverridden);
		memcpy(&m_stateTaxOverridden, bufferPos, sizeof(m_stateTaxOverridden));
		bufferPos += sizeof(m_stateTaxOverridden);
	}

}

// NOTE: Keep this function static.
bool CJackpot::DoesJackpotExistInNVRAM()
{
	bool bExists = CMemoryStorage::IntegrityCheck() && IsActiveLocation(NVRAM_JACKPOT_CHECKBYTE_OFFSET, NVRAM_JACKPOT_CHECKBYTE_VALUE);

	if (!bExists)
		SetInactiveLocation(NVRAM_JACKPOT_CHECKBYTE_OFFSET, ~NVRAM_JACKPOT_CHECKBYTE_VALUE);

	return bExists;
}

uint64_t CJackpot::GetJackpotAmount() const
{
	return m_jackpotAmount;
}

bool CJackpot::IsKeyToCreditAvailable() const
{
	return m_keyToCreditAvailable;
}

void CJackpot::SetTransactionSent()
{
	m_jackpotTransactionSent = true;
	StoreToNVRAM(__FUNCTION__);
}

bool CJackpot::IsTransactionSent() const
{
	return m_jackpotTransactionSent;
}

void CJackpot::SetKeyToCreditAvailable(bool keyToCreditAvailable)
{
	m_keyToCreditAvailable = keyToCreditAvailable;
	StoreToNVRAM(__FUNCTION__);
}

void CJackpot::SetUnattendedState(UnattendedJackpotState::Type state)
{
	LogString(JACKPOT, "SetUnattendedState currentstate=%s newstate=%s", GetUnattendedStateStr(), UnattendedJackpotState::emap.At(state, "unknown"));
	m_unattendedState = state;
	SetUnattendedStateTimestamp(CUtilities::GetTickCount());
	StoreToNVRAM(__FUNCTION__);

	//PinEntry and error conditions that bypass PinEntry 
	//is displayed immediately and do not need to be pushed to m_ujpScreensToDisplay
	switch (m_unattendedState)
	{
	case UnattendedJackpotState::UJPPinSent:
	case UnattendedJackpotState::UJPPinEntry:
		m_ujpScreen = UJPScreen::PinEntry;
		break;
	case UnattendedJackpotState::UJPCanceledHandpayReset:
		m_ujpScreen = UJPScreen::CanceledHandpayReset;		
		break;
	case UnattendedJackpotState::UJPCanceledVirtualPlayer:
		m_ujpScreen = UJPScreen::ErrorPlayerInVirtualSession;
		break;
	case UnattendedJackpotState::UJPCanceledInsufficientInfo:
		m_ujpScreen = UJPScreen::ErrorInsufficientInfo;
		break;
	case UnattendedJackpotState::UJPCanceledEgmNotCapable:
		m_ujpScreen = UJPScreen::ErrorEGMNotCapable;
		break;
	case UnattendedJackpotState::UJPCanceledAboveLimit:
		m_ujpScreen = UJPScreen::ErrorJackpotAboveLimit;
		break;
	case UnattendedJackpotState::UJPCardedTeaser:
		m_ujpScreen = UJPScreen::CardedTeaser;
		break;
	case UnattendedJackpotState::UJPUncardedTeaser:
		m_ujpScreen = UJPScreen::UncardedTeaser;
		break;
	case UnattendedJackpotState::UJPPinError:
		m_ujpScreensToDisplay.push(UJPScreen::PinError);
		//Resetting to PinEntry in case the next state is another PinError, 
		//otherwise there won't be a state change and Angular will be stuck
		//displaying validating pin when there is another pin error.
		m_ujpScreensToDisplay.push(UJPScreen::PinEntry);
		break;		
	case UnattendedJackpotState::UJPAuthorized:
		m_ujpScreensToDisplay.push(IsTaxWithholdingEnabled() ? UJPScreen::PinSuccessWithTax : UJPScreen::PinSuccessWithoutTax);
		break;
	case UnattendedJackpotState::UJPCreditAndRemoteReset:
		m_ujpScreensToDisplay.push(IsTaxWithholdingEnabled() ? (TaxWithholdingFailed() ? UJPScreen::CompleteWithError : UJPScreen::Complete) : UJPScreen::Complete);
		break;
	case UnattendedJackpotState::UJPCreditReset:
		m_ujpScreensToDisplay.push(UJPScreen::ErrorCreditReset);
		break;
	case UnattendedJackpotState::UJPFailed:
	case UnattendedJackpotState::UJPCanceled:
		m_ujpScreensToDisplay.push(UJPScreen::ErrorUnknown);
		break;
	case UnattendedJackpotState::UJPCanceledAccountLocked:
		m_ujpScreensToDisplay.push(UJPScreen::ErrorAccountLocked);
		break;
	case UnattendedJackpotState::UJPCanceledTimedOut:
		m_ujpScreensToDisplay.push(UJPScreen::ErrorSessionExpired);
		break;
	default:
		break;
	}
}

UnattendedJackpotState::Type CJackpot::GetUnattendedState() const
{
	return m_unattendedState;
}

const char* CJackpot::GetUnattendedStateStr() const
{
	return UnattendedJackpotState::emap.At(m_unattendedState, "unknown");
}

UnattendedJackpotStatus::Type CJackpot::GetJackpotStatus() const
{
	UnattendedJackpotStatus::Type status(UnattendedJackpotStatus::UJPStatusCanceled);

	switch (m_unattendedState)
	{
	case UnattendedJackpotState::UJPCreditAndRemoteReset:
		status = UnattendedJackpotStatus::UJPStatusSuccessKeyToCreditRemoteReset;
		break;
	case UnattendedJackpotState::UJPCreditReset:
		status = UnattendedJackpotStatus::UJPStatusSuccessKeyToCredit;
		break;
	case UnattendedJackpotState::UJPFailed:
		status = UnattendedJackpotStatus::UJPStatusFailed;
		break;
	case UnattendedJackpotState::UJPCanceled:
		status = UnattendedJackpotStatus::UJPStatusCanceled;
		break;
	case UnattendedJackpotState::UJPCanceledHandpayReset:
		status = UnattendedJackpotStatus::UJPStatusCanceledHandpayReset;
		break;
	case UnattendedJackpotState::UJPCanceledReboot:
		status = UnattendedJackpotStatus::UJPStatusCanceledReboot;
		break;
	case UnattendedJackpotState::UJPCanceledVirtualPlayer:
		status = UnattendedJackpotStatus::UJPStatusCanceledVirtualPlayer;
		break;
	case UnattendedJackpotState::UJPCanceledTimedOut:
		status = UnattendedJackpotStatus::UJPStatusCanceledTimedOut;
		break;
	case UnattendedJackpotState::UJPCanceledTaskbarButtonPress:
		status = UnattendedJackpotStatus::UJPStatusCanceledTaskbarButtonPress;
		break;
	case UnattendedJackpotState::UJPCanceledCloseMediaWindow:
		status = UnattendedJackpotStatus::UJPStatusCanceledCloseMediaWindow;
		break;
	case UnattendedJackpotState::UJPCanceledAccountLocked:
		status = UnattendedJackpotStatus::UJPStatusCanceledAccountLocked;
		break;
	case UnattendedJackpotState::UJPCanceledPlayerIdMismatch:
		status = UnattendedJackpotStatus::UJPStatusCanceledPlayerIdMismatch;
		break;
	case UnattendedJackpotState::UJPCanceledInsufficientInfo:
		status = UnattendedJackpotStatus::UJPStatusCanceledInsufficientInfo;
		break;
	case UnattendedJackpotState::UJPCanceledEgmNotCapable:
		status = UnattendedJackpotStatus::UJPStatusCanceledEgmNotCapable;
		break;
	case UnattendedJackpotState::UJPCanceledAboveLimit:
		status = UnattendedJackpotStatus::UJPStatusCanceledAboveLimit;
		break;
	case UnattendedJackpotState::UJPCanceledCallHost:
		status = UnattendedJackpotStatus::UJPStatusCanceledCallHost;
		break;
	default:
		status = UnattendedJackpotStatus::UJPStatusCanceled;
		break;
	}

	return status;
}

const char* CJackpot::GetJackpotStatusStr() const
{
	return UnattendedJackpotStatus::emap.At(GetJackpotStatus(), "unknown");
}

bool CJackpot::IsJackpotComplete() const
{
	return (m_jackpotTransactionSent && !IsUnattendedJackpotInProgress());
}

bool CJackpot::IsUnattendedJackpotInProgress() const
{
	return (UnattendedJackpotState::UJPPinEntry == m_unattendedState || UnattendedJackpotState::UJPPinSent == m_unattendedState || UnattendedJackpotState::UJPAuthorized == m_unattendedState);
}

void CJackpot::SetUnattendedResetResult(ResetResult keyToCreditResult, ResetResult remoteResetResult)
{
	if (ResetSuccess == keyToCreditResult)
	{
		m_unattendedState = ResetSuccess == remoteResetResult ? UnattendedJackpotState::UJPCreditAndRemoteReset : UnattendedJackpotState::UJPCreditReset;
		SetUnattendedStateTimestamp(CUtilities::GetTickCount());
	}
	else if (NotInHandpay == keyToCreditResult)
	{
		m_unattendedState = UnattendedJackpotState::UJPCanceled;
		SetUnattendedStateTimestamp(CUtilities::GetTickCount());
	}
	else
	{
		m_unattendedState = UnattendedJackpotState::UJPFailed;
		SetUnattendedStateTimestamp(CUtilities::GetTickCount());
	}
	StoreToNVRAM(__FUNCTION__);
}

void CJackpot::SetJackpotExistanceToNVRAM(bool bJackpotExists)
{
	if (m_type == MEMORY_NVRAM)
	{
		byte jackpotCheckByteValue = bJackpotExists ? NVRAM_JACKPOT_CHECKBYTE_VALUE : ~NVRAM_JACKPOT_CHECKBYTE_VALUE;

		if (!Write(NVRAM_JACKPOT_CHECKBYTE_OFFSET, &jackpotCheckByteValue, sizeof(jackpotCheckByteValue)))
			LogString(ERROR_LOG, "Could not write jackpot check byte to NVRAM");
	}
}

void CJackpot::StoreToNVRAM(const char* functionName) const
{
	if (m_type == MEMORY_NVRAM)
	{
		byte nvramBuffer[NVRAM_JACKPOT_DATA_SIZE];

		byte* bufferPos = nvramBuffer;

		memcpy(bufferPos, &m_jpType, sizeof(m_jpType));
		bufferPos += sizeof(m_jpType);

		memcpy(bufferPos, &m_transType, sizeof(m_transType));
		bufferPos += sizeof(m_transType);

		memcpy(bufferPos, &m_jackpotAmount, sizeof(m_jackpotAmount));
		bufferPos += sizeof(m_jackpotAmount);

		memcpy(bufferPos, &m_progressiveLevel, sizeof(m_progressiveLevel));
		bufferPos += sizeof(m_progressiveLevel);

		memcpy(bufferPos, &m_coinsPlayed, sizeof(m_coinsPlayed));
		bufferPos += sizeof(m_coinsPlayed);

		memcpy(bufferPos, &m_jackpotID, sizeof(m_jackpotID));
		bufferPos += sizeof(m_jackpotID);

		memcpy(bufferPos, m_cardID, CARD_ID_LEN);
		bufferPos += CARD_ID_LEN;

		memcpy(bufferPos, &m_keyToCreditAvailable, sizeof(m_keyToCreditAvailable));
		bufferPos += sizeof(m_keyToCreditAvailable);

		memcpy(bufferPos, &m_unattendedState, sizeof(m_unattendedState));
		bufferPos += sizeof(m_unattendedState);

		memcpy(bufferPos, &m_jackpotTransactionSent, sizeof(m_jackpotTransactionSent));
		bufferPos += sizeof(m_jackpotTransactionSent);

		memcpy(bufferPos, &m_handpayReset, sizeof(m_handpayReset));
		bufferPos += sizeof(m_handpayReset);

		memcpy(bufferPos, &m_federalTaxWithholding, sizeof(m_federalTaxWithholding));
		bufferPos += sizeof(m_federalTaxWithholding);

		memcpy(bufferPos, &m_stateTaxWithholding, sizeof(m_stateTaxWithholding));
		bufferPos += sizeof(m_stateTaxWithholding);

		memcpy(bufferPos, &m_federalTaxOverridden, sizeof(m_federalTaxOverridden));
		bufferPos += sizeof(m_federalTaxOverridden);

		memcpy(bufferPos, &m_stateTaxOverridden, sizeof(m_stateTaxOverridden));
		bufferPos += sizeof(m_stateTaxOverridden);

		int bytesInBuffer = bufferPos - nvramBuffer;
		LogString(JACKPOT, "%s-StoreToNVRAM bytesInBuffer=%d", functionName, bytesInBuffer);
		if (!Write(NVRAM_JACKPOT_OFFSET + NVRAM_JACKPOT_CHECKBYTE_SIZE, nvramBuffer, bytesInBuffer))
			LogString(ERROR_LOG, "Could not write jackpot data to NVRAM");
	}
}

void CJackpot::SetHandPayReset(bool handpayReset)
{
	m_handpayReset = handpayReset;
	StoreToNVRAM(__FUNCTION__);
}

bool CJackpot::GetHandPayReset() const
{
	return m_handpayReset;
}

void CJackpot::SetUnattendedStateTimestamp(DWORD tickCount)
{
	m_unattendedStateTimestamp = tickCount;
}

/// <summary>
/// Gets the unattended state timestamp.
/// </summary>
/// <returns></returns>
DWORD CJackpot::GetUnattendedStateTimestamp() const
{
	return m_unattendedStateTimestamp;
}

void CJackpot::SetCardID(const byte* cardID, int dataLen)
{
	if (cardID != nullptr && dataLen > 0 && dataLen <= CARD_ID_LEN)
	{
		memcpy(m_cardID, cardID, dataLen);
		StoreToNVRAM(__FUNCTION__);
	}
}

const byte* CJackpot::GetCardID() const
{
	return m_cardID;
}

void CJackpot::SetJackpotID(int jackpotID)
{
	m_jackpotID = jackpotID;
	StoreToNVRAM(__FUNCTION__);
}

int CJackpot::GetJackpotID() const
{
	return m_jackpotID;
}

bool CJackpot::IsExpiredUnattendedJackpot(DWORD expireTicks) const
{
	const int defaultExpireTicks(60000);
	bool canceled = false;

	if (expireTicks == 0)
	{
		expireTicks = defaultExpireTicks;
	}

	if (IsUnattendedJackpotInProgress() && m_unattendedState != UnattendedJackpotState::UJPPinEntry && CUtilities::GetTickCount() - m_unattendedStateTimestamp > (DWORD)expireTicks)
	{
		// Its been too long
		canceled = true;
	}

	return canceled;
}

void CJackpot::SetTaxWithholdingConfig(bool taxWithholdingEnabled, float federalPercentage, float statePercentage)
{
	LogString(JACKPOT, "SetTaxWithholdingConfig taxWithholdingEnabled=%u federalPercentage=%.2f statePercentage=%.2f",
		taxWithholdingEnabled, federalPercentage, statePercentage);
	m_federalTaxWithholdingPercent = federalPercentage;
	m_stateTaxWithholdingPercent = statePercentage;
	if ((m_federalTaxWithholdingPercent + m_stateTaxWithholdingPercent) > 0)
	{
		m_taxWithholdingEnabled = taxWithholdingEnabled;
	}
	else
	{
		m_taxWithholdingEnabled = false;
	}
}

bool CJackpot::IsTaxWithholdingEnabled() const
{
	return m_taxWithholdingEnabled;
}

float CJackpot::GetFederalTaxWithholdingPercent() const
{
	return m_federalTaxWithholdingPercent;
}

float CJackpot::GetStateTaxWithholdingPercent() const
{
	return m_stateTaxWithholdingPercent;
}

void CJackpot::SetTaxWithholding(DWORD federal, DWORD state, bool federalTaxOverridden, bool stateTaxOverridden)
{
	m_federalTaxWithholding = federal;
	m_stateTaxWithholding = state;
	m_federalTaxOverridden = federalTaxOverridden;
	m_stateTaxOverridden = stateTaxOverridden;
	StoreToNVRAM(__FUNCTION__);
}

bool CJackpot::TaxWithholdingFailed() const
{
	return 0 == m_federalTaxWithholding &&
		0 == m_stateTaxWithholding &&
		!m_federalTaxOverridden &&
		!m_stateTaxOverridden;
}

DWORD CJackpot::GetFederalTaxWithholding() const
{
	return m_federalTaxWithholding;
}

DWORD CJackpot::GetStateTaxWithholding() const
{
	return m_stateTaxWithholding;
}

uint64_t CJackpot::GetJackpotAmountAfterTaxes() const
{
	uint64_t jackpotAmountAfterTaxes(m_jackpotAmount);
	uint64_t totalWitholdingInCents((uint64_t)GetTotalWithholdingInPennies());

	if (totalWitholdingInCents <= jackpotAmountAfterTaxes)
	{
		jackpotAmountAfterTaxes -= totalWitholdingInCents;
	}

	return jackpotAmountAfterTaxes;
}

DWORD CJackpot::GetTotalWithholdingInPennies() const
{
	return (m_federalTaxWithholding + m_stateTaxWithholding);
}

bool CJackpot::IsFederalTaxOverriden() const
{
	return 	m_federalTaxOverridden;
}

bool CJackpot::IsStateTaxOverriden() const
{
	return 	m_stateTaxOverridden;
}

void CJackpot::SetPlayerPin(const string& pin)
{
	m_pin = pin;
}

string CJackpot::GetPlayerPin() const
{
	return m_pin;
}

std::string CJackpot::GetLogInfoString(const CJackpot* jackpot, LPCTSTR description)
{
	std::string logInfoString;

	if (nullptr == jackpot)
	{
		logInfoString = FormatString(_T("%s -- JACKPOT: null"),
			static_cast<LPCTSTR>(description != NULL ? description : _T("")));
	}
	else
	{
		logInfoString = FormatString(
			_T("%s -- ") // No comma.
			_T("JACKPOT: ID=%d type=%d transType=%d amt=%lld progLvl=%d coinsPlayed=%u") // No comma.
			_T(" keyToCrdAvail=%d unattState=%s unattTicks=%u jpSent=%d card=%s"),

			static_cast<LPCTSTR>(description != NULL ? description : _T("")),

			(int)jackpot->m_jackpotID,
			(int)jackpot->m_jpType,
			(int)jackpot->m_transType,
			(long long)jackpot->m_jackpotAmount,
			(int)jackpot->m_progressiveLevel,
			(unsigned)jackpot->m_coinsPlayed,

			(int)jackpot->m_keyToCreditAvailable,
			jackpot->GetUnattendedStateStr(),
			(unsigned)jackpot->m_unattendedStateTimestamp,
			(int)jackpot->m_jackpotTransactionSent,
			static_cast<LPCTSTR>(CUtilities::MakeStringFromCardID(jackpot->m_cardID).c_str()));
	}
	return logInfoString;
}

void CJackpot::LogInfo(const CJackpot* jackpot, LPCTSTR description)
{
	std::string logInfoString(GetLogInfoString(jackpot, description));
	LogStringNoFormat(JACKPOT, logInfoString);
}


void CJackpot::SetScreenToDisplay()
{
	DWORD deltaTickCount(m_operatingSystem.GetTickCount32() - m_displayStartTickCount);
	if (deltaTickCount >= SCREEN_DISPLAY_DURATION_IN_MS && !m_ujpScreensToDisplay.empty())
	{
		m_displayStartTickCount = m_operatingSystem.GetTickCount32();
		m_ujpScreen = m_ujpScreensToDisplay.front();
		m_ujpScreensToDisplay.pop();
		if (UJPScreen::PinEntry == m_ujpScreen)
		{
			SetUnattendedState(UnattendedJackpotState::UJPPinEntry);
		}
	}
}

string CJackpot::GetStartupScreen() const
{
	return string(UJPScreen::emap.At(m_ujpScreen));
}
