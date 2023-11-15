#include "stdafx.h"
#include "PersonalBankerTransfer.h"

#include "GlobalDefs.h"
#include "Diagnostics.h"
#include "Logging/LogString.h"
#include "Player.h"

using namespace std;

const char* PersonalBankerStateStr[] =
{
	"PBStateUnknown",
	"PBStateInquireSent",
	"PBStateInfoReceived",
	"PBStateTransferingFromToGame",
	"PBStateTransferComplete",
	"PBStateReleaseSent",
	"PBStateWaitingForUploadToStart",
	"PBStateIdle_NotUsed",
	"PBStateSessionRequested_NotUsed",
	"PBStateAuthRequested_NotUsed",
	"PBStateAuthGranted_NotUsed",
	"PBStateAuthDenied",
	"PBStateSendingTransferTransaction_NotUsed",
	"PBStateTransferTransactionSent_NotUsed",
	"PBStateSendingRelease_NotUsed",
};

static_assert(NUMBER_OF_PB_STATES == _countof(PersonalBankerStateStr),
	"PersonalBankerStateStr has to be updated to match all the entries in PersonalBankerState");

static const char * PBTTypeStrings[] = {
	"Undeclared",
	"CashToGame",
	"PointsToGame",
	"PromoDollarsToGame",
	"CashFromGame",
	"PointsFromGame",
	"PromoDollarsFromGame",
	"ExternalCredit",
	"AllFromGame"
};

const LPCTSTR PB_CANCELLATION_TOKEN[NUM_CANCELLATION_ERRORS] =
{
	"",
	"noGameDenom",
	"doorOpen",
	"transactionCanceled",
	"overTransferLimit",
	"sasInvalidTransaction",
	"nonCreditMode",
	"machineTilt",
	"sasInvalidACKMsg",
	"gameInProgress",
	"sasNonBcdDataError",
	"sasDuplicateTransfer",
	"machineDisabledViaSAS",
	"machineOutofService",
	"sasMachineBusy",
	"",
	"sasTimeout",
	"bartGameDenomMismatch",
	"aftLockReject",
	"canceledBySystem",
	"partialTransfer",
	"gameNotRegistered",
	"misMatchedRegistration",
	"noCredits",
	"ticketReceiptError",
	"gameNotLocked",
	"gameAssetError",
	"invalidTransferCode",
	"amountNotEvenMultiple",
	"noPosId",
	"restrictedPoolIdMismatch",
	"generalError",
	"invalidTransactionId",
	"gameCommDown",
	"allowableRangeExceeded",
	"mismatchTransactionId",
	"incompatiblePoll",
	"unsupportedTransferCode",
	"transferInfoUnavailable",
	"promoUploadCancelledBySystem",
	"promoUploadCancelledByEmployee",
	"unableToAuthorizeAmountRequested",
	"communicationsError",
	"creditDeniedBySystem",
	"unknownSystemError",
};


const LPCTSTR PB_CANCELLATION_CODES[NUM_CANCELLATION_ERRORS] =
{
	"",
	"G-01",
	"G-02",
	"G-03",
	"G-04",
	"G-05",
	"G-06",
	"G-07",
	"G-08",
	"G-09",
	"G-10",
	"G-11",
	"G-12",
	"G-13",
	"G-14",
	"G-15",
	"S-22",
	"S-23",
	"G-16",
	"G-24",
	"G-86",
	"G-88",
	"G-89",
	"G-8B",
	"G-8E",
	"G-99",
	"G-93",
	"G-82",
	"G-85",
	"G-8A",
	"G-8F",
	"G-9F",
	"S-32",
	"S-33",
	"S-34",
	"S-35",
	"G-C0",
	"G-C1",
	"G-FF",
	"S-39",
	"",
};

CPersonalBankerTransfer::CPersonalBankerTransfer(DWORD tickCount, MEMORY_TYPE memoryType, bool bBuildFromNVRAM)
{
	m_type = memoryType;
	m_dwAllowedSize = NVRAM_PB_TRANSFER_OFFSET;
	m_dwOffset = NVRAM_PB_TRANSFER_SIZE;
	m_bOffsetIsSet = true;
	m_federalTaxDeductionInPennies = 0;
	m_jackpotId = 0;
	m_cashoutToHostTransfer = false;
	m_originalCreditMeter = 0;
	m_cancelledType = _Unknown;
	m_pbtTransferType = Undeclared;
	m_pbtType = Unspecified;
	m_promoCancelledType = _Unknown;
	m_promoPbtTransferType = Undeclared;
	m_promoPbtType = Unspecified;
	m_pbtPIN = 0;

	m_amountInPennies = 0;
	m_restrictedAmountInPennies = 0;
	m_nonrestrictedAmountInPennies = 0;

	m_sequenceNumber = 0;
	m_restrictedSequenceNumber = 0;
	m_nonrestrictedSequenceNumber = 0;

	m_pbtIDLength = 0;
	memset(m_pbtID, 0, MAX_PBID_LENGTH);

	m_sessionStartTickCount = tickCount;

	m_bPointRedemptionRestricted = true;
	m_bCEPOutEnabled = true;

	// Zero out the card number to start off with
	m_cardNumber = new byte[CARD_ID_LEN];
	memset( m_cardNumber, 0, CARD_ID_LEN);
	m_cardNumberLen = CARD_ID_LEN;

	m_eftNumber = 0;

	m_denomination = 0;

	m_utcCardInTime = 0;

	m_minimumCashUp = 0;
	m_maximumCashUp = 0;
	m_minimumPromoUp = 0;
	m_maximumPromoUp = 0;
	m_PBTSent = false;
	m_pbtTransferFlags.m_uValue = 0;
	m_cashoutErrorFlags = 0;
	m_autoUploadErrorFlags = 0;

	if ( m_type == MEMORY_NVRAM )
	{
		if ( bBuildFromNVRAM )
		{
			BuildYourself();
		}
		else
		{
			StoreTypesDataToNVRAM();
			StoreIDDataToNVRAM();
			StoreStartDataToNVRAM();
			StoreUpdatedDataToNVRAM();
			StoreCardTimeToNVRAM();

		}
		SetActiveLocation(NVRAM_PB_TRANSFER_CHECKBYTE_OFFSET, NVRAM_PB_TRANSFER_CHECKBYTE_VALUE);
	}
}

CPersonalBankerTransfer::~CPersonalBankerTransfer(void)
{
	if (m_cardNumber != NULL)
	{
		delete[] m_cardNumber;
		m_cardNumber = NULL;
	}

	if (m_type == MEMORY_NVRAM)
	{
		SetActiveLocation(NVRAM_PB_TRANSFER_CHECKBYTE_OFFSET, (BYTE)~NVRAM_PB_TRANSFER_CHECKBYTE_VALUE);
	}
}

CPersonalBankerTransfer::CPersonalBankerTransfer(const CPersonalBankerTransfer &rhs)
{
	//When the copy constructor is used, the memory type is always set to none.
	m_type = MEMORY_NONE;

	m_cardNumber = nullptr;

	CopyObj(rhs);
}

CPersonalBankerTransfer & CPersonalBankerTransfer::operator=( const CPersonalBankerTransfer &rhs )
{
	if (this != &rhs)
	{
		CopyObj(rhs);
	}

	return *this;
}

void CPersonalBankerTransfer::CopyObj(const CPersonalBankerTransfer& rhs)
{
	//if this is used without the copy constructor, then the memory type of
	//the lhs object is unchanged.
	SetID(rhs.GetID(), rhs.GetPBTIDLength());
	SetUTCCardInTime(rhs.GetUTCCardInTime());
	SetOriginalCreditMeter(rhs.GetOriginalCreditMeter());
	SetPersonalBankerTransferType(rhs.GetPersonalBankerTransferType());
	SetPersonalBankerType(rhs.GetPersonalBankerType());
	SetPromoCancelledType(rhs.GetPromoCancelledType());
	SetPromoPersonalBankerTransferType(rhs.GetPromoPersonalBankerTransferType());
	SetPromoPersonalBankerType(rhs.GetPromoPersonalBankerType());
	SetCancelledType(rhs.GetCancelledType());
	SetPIN(rhs.GetPIN());
	ResetSessionStartTime(rhs.m_sessionStartTickCount);
	SetAmount(rhs.GetAmount());
	SetRestrictedAmount(rhs.GetRestrictedAmount());
	SetNonrestrictedAmount(rhs.GetNonrestrictedAmount());
	SetSequenceNumber(rhs.GetSequenceNumber());
	SetRestrictedSequenceNumber(rhs.GetRestrictedSequenceNumber());
	SetNonrestrictedSequenceNumber(rhs.GetNonrestrictedSequenceNumber());
	SetMinCashUp(rhs.GetMinCashUp());
	SetMaxCashUp(rhs.GetMaxCashUp());
	SetMinPromoUp(rhs.GetMinPromoUp());
	SetMaxPromoUp(rhs.GetMaxPromoUp());
	SetPBTSent(rhs.GetPBTSent());
	m_pbtTransferFlags.m_uValue = rhs.m_pbtTransferFlags.m_uValue;
	m_cashoutErrorFlags = rhs.m_cashoutErrorFlags;
	m_autoUploadErrorFlags = rhs.m_autoUploadErrorFlags;
	SetCashoutToHostTransfer(rhs.IsCashoutToHostTransfer());
	SetTaxInformation(rhs.IsProcessTaxDeduction(), rhs.IsFederalTaxOverridden(), rhs.IsStateTaxOverridden(),
		rhs.GetJackpotId(), rhs.GetFederalTaxDeductionInPennies());
	m_bPointRedemptionRestricted = rhs.IsPointRedemptionRestricted();
	m_bCEPOutEnabled = rhs.IsCEPOutEnabled();
	SetDenomination(rhs.GetDenomination());

	// Only copy card number if the source number isn't all 00s or "this" doesn't contain a card number
	byte emptyNumber[CARD_ID_LEN]{ 0, 0, 0, 0, 0 };
	int cardNumLen;
	const byte *cardNum = rhs.GetCardNumber(cardNumLen);
	if (memcmp(emptyNumber, rhs.m_cardNumber, CARD_ID_LEN) != 0 || this->m_cardNumber == nullptr)
	{
		SetCardNumber(cardNum, cardNumLen);
	}

	SetEFTNumber(rhs.GetEFTNumber());

	StoreTypesDataToNVRAM();
	StoreIDDataToNVRAM();
	StoreStartDataToNVRAM();
	StoreUpdatedDataToNVRAM();
	StoreCardTimeToNVRAM();
}

bool CPersonalBankerTransfer::operator==( const CPersonalBankerTransfer &rhs )
{
	bool bIsSame = false;

	// See if the pb id's match
	if ( GetPBTIDLength() == rhs.GetPBTIDLength() && memcmp( m_pbtID, rhs.GetID(), m_pbtIDLength ) == 0 )
	{
		bIsSame = true;
	}

	// See if the our pb id matches the eft number of the other guy.
	std::string tempID = std::to_string(rhs.GetEFTNumber());
	if (!bIsSame && GetPBTIDLength() == (int)tempID.length() && strncmp(GetID(), tempID.c_str(), GetPBTIDLength()) == 0)
	{
		bIsSame = true;
	}

	// See if the other guy's eft number matches our pbid
	tempID = std::to_string(GetEFTNumber());
	if (!bIsSame && rhs.GetPBTIDLength() == (int)tempID.length() && strncmp(rhs.GetID(), tempID.c_str(), rhs.GetPBTIDLength()) == 0)
	{
		bIsSame = true;
	}

	return bIsSame;
}

void CPersonalBankerTransfer::BuildYourself( void )
{
	if ( m_type == MEMORY_NVRAM )
	{
		byte nvramBuffer[NVRAM_MAX_READ];

		if ( !Read( NVRAM_PB_TRANSFER_TYPES_OFFSET, nvramBuffer, NVRAM_PB_TRANSFER_TYPES_SIZE ) )
			LogString(ERROR_LOG, "Could not read pb transfer types data from NVRAM" );
		byte *bufferPos = nvramBuffer;
		memcpy( &m_cancelledType, bufferPos, sizeof( m_cancelledType ) );
		bufferPos += sizeof( m_cancelledType );
		memcpy( &m_pbtType, bufferPos, sizeof( m_pbtType ) );
		bufferPos += sizeof( m_pbtType );
		memcpy( &m_pbtTransferType, bufferPos, sizeof( m_pbtTransferType ) );
		bufferPos += sizeof( m_pbtTransferType );
		memcpy (&m_promoPbtTransferType, bufferPos, sizeof(m_promoPbtTransferType));
		bufferPos += sizeof( m_promoPbtTransferType);

		if ( !Read( NVRAM_PB_TRANSFER_IDS_OFFSET, nvramBuffer, NVRAM_PB_TRANSFER_IDS_SIZE ) )
			LogString(ERROR_LOG, "Could not read pb transfer ids data from NVRAM" );
		bufferPos = nvramBuffer;
		memcpy( &m_pbtIDLength, bufferPos, sizeof( m_pbtIDLength ) );
		bufferPos += sizeof( m_pbtIDLength );
		memcpy( &m_pbtID, bufferPos, MAX_PBID_LENGTH );
		bufferPos += sizeof( m_pbtID );
		memcpy( &m_eftNumber, bufferPos, sizeof( m_eftNumber ) );
		bufferPos += sizeof( m_eftNumber );

		if ( !Read( NVRAM_PB_TRANSFER_START_OFFSET, nvramBuffer, NVRAM_PB_TRANSFER_START_SIZE ) )
			LogString(ERROR_LOG, "Could not read pb transfer start data from NVRAM" );
		bufferPos = nvramBuffer;
		memcpy( &m_pbtPIN, bufferPos, sizeof( m_pbtPIN ) );
		bufferPos += sizeof( m_pbtPIN );
		//memcpy( &m_sessionStartTickCount, bufferPos, sizeof( m_sessionStartTickCount ) );
		bufferPos += sizeof( m_sessionStartTickCount );
		memcpy( &m_bPointRedemptionRestricted, bufferPos, sizeof( m_bPointRedemptionRestricted ) );
		bufferPos += sizeof( m_bPointRedemptionRestricted );
		memcpy( &m_cardNumberLen, bufferPos, sizeof( m_cardNumberLen ) );
		bufferPos += sizeof( m_cardNumberLen );
		if ( m_cardNumber != NULL )
			delete [] m_cardNumber;
		m_cardNumber = new byte[MAX_PLAYER_CARD_NUMBER_LENGTH];
		memcpy( m_cardNumber, bufferPos, MAX_PLAYER_CARD_NUMBER_LENGTH );
		bufferPos += MAX_PLAYER_CARD_NUMBER_LENGTH;
		memcpy( &m_bCEPOutEnabled, bufferPos, sizeof( m_bCEPOutEnabled ) );
		bufferPos += sizeof( m_bCEPOutEnabled );
		memcpy( &m_denomination, bufferPos, sizeof( m_denomination ) );
		bufferPos += sizeof( m_denomination );
		memcpy(&m_federalTaxDeductionInPennies, bufferPos, sizeof(m_federalTaxDeductionInPennies));
		bufferPos += sizeof(m_federalTaxDeductionInPennies);
		memcpy(&m_jackpotId, bufferPos, sizeof(m_jackpotId));
		bufferPos += sizeof(m_jackpotId);

		if ( !Read( NVRAM_PB_TRANSFER_UPDATED_OFFSET, nvramBuffer, NVRAM_PB_TRANSFER_UPDATED_SIZE ) )
			LogString(ERROR_LOG, "Could not read pb transfer updated data from NVRAM" );
		bufferPos = nvramBuffer;
		memcpy( &m_state_NotUsed, bufferPos, sizeof( m_state_NotUsed ) );
		bufferPos += sizeof( m_state_NotUsed );
		memcpy( &m_sequenceNumber, bufferPos, sizeof( m_sequenceNumber ) );
		bufferPos += sizeof( m_sequenceNumber );
		memcpy( &m_restrictedSequenceNumber, bufferPos, sizeof( m_restrictedSequenceNumber ) );
		bufferPos += sizeof( m_restrictedSequenceNumber );
		memcpy( &m_nonrestrictedSequenceNumber, bufferPos, sizeof( m_nonrestrictedSequenceNumber ) );
		bufferPos += sizeof( m_nonrestrictedSequenceNumber );
		memcpy( &m_originalCreditMeter, bufferPos, sizeof( m_originalCreditMeter ) );
		bufferPos += sizeof( m_originalCreditMeter );
		memcpy( &m_amountInPennies, bufferPos, sizeof( m_amountInPennies ) );
		bufferPos += sizeof( m_amountInPennies );
		memcpy( &m_restrictedAmountInPennies, bufferPos, sizeof( m_restrictedAmountInPennies ) );
		bufferPos += sizeof( m_restrictedAmountInPennies );
		memcpy( &m_nonrestrictedAmountInPennies, bufferPos, sizeof( m_nonrestrictedAmountInPennies ) );
		bufferPos += sizeof( m_nonrestrictedAmountInPennies );
		memcpy (&m_promoCancelledType, bufferPos, sizeof(m_promoCancelledType));
		bufferPos += sizeof( m_promoCancelledType);
		memcpy (&m_promoPbtType, bufferPos, sizeof(m_promoPbtType));
		bufferPos += sizeof( m_promoPbtType);
		memcpy(&m_pbtTransferFlags.m_uValue, bufferPos, sizeof(m_pbtTransferFlags.m_uValue));
		bufferPos += sizeof(m_pbtTransferFlags.m_uValue);

		if ( !Read( NVRAM_PB_TRANSFER_CARDTIME_OFFSET, nvramBuffer, NVRAM_TIME_SIZE ) )
			LogString(ERROR_LOG, "Could not read lpb transfer card time  from NVRAM" );
		m_utcCardInTime = CUtilities::MakeTimeFromNvramBuffer( nvramBuffer, NVRAM_TIME_SIZE );

		if (CMemoryStorage::PreviousNVRAMVersion < NVRAM_UJP_TAX_WITHHOLDING_VERSION)
		{
			LogString(NVRAM_LOG, _T("CPersonalBankerTransfer::BuildYourself NVRAM upgraded for NVRAM_UJP_TAX_WITHHOLDING_VERSION"));
			SetTaxInformation(0, false, false, 0, 0);
		}
	}
}

const char* CPersonalBankerTransfer::GetStateString(PersonalBankerState state)
{
	if (state < NUMBER_OF_PB_STATES)
	{
		return PersonalBankerStateStr[state];
	}
	else
	{
		return "unknown";
	}	
}

// NOTE: Keep this function static.
bool CPersonalBankerTransfer::DoesPersonalBankerTransferExistInNVRAM( void )
{
	bool bExists = CMemoryStorage::IntegrityCheck() && IsActiveLocation( NVRAM_PB_TRANSFER_CHECKBYTE_OFFSET, NVRAM_PB_TRANSFER_CHECKBYTE_VALUE );

	if ( !bExists )
		SetInactiveLocation();

	return bExists;
}

DWORD CPersonalBankerTransfer::GetSecondsSinceCardIn()
{
	time_t now;
	double	timeSinceCardIn;

	now = CUtilities::GetUTCTime();

	timeSinceCardIn = difftime(now, m_utcCardInTime);

	DWORD seconds = (DWORD)(timeSinceCardIn);

	return seconds;
}

bool CPersonalBankerTransfer::HasSessionExpired( DWORD tickCount, DWORD sessionTimeoutValueInMilliseconds ) const
{
	DWORD millisecondsSinceStart = tickCount - m_sessionStartTickCount;
	bool bExpired = millisecondsSinceStart >= sessionTimeoutValueInMilliseconds;

	return bExpired;
}

void CPersonalBankerTransfer::SetPersonalBankerTransferType(PersonalBankerTransferType type)
{
	m_pbtTransferType = type;

	switch (type)
	{
	case CashToGame:
		m_pbtType = CashDownload;
		break;
	case PointsToGame:
		if ( IsPointRedemptionRestricted() )
			m_pbtType = PointRedemptionToPromoCredits;
		else
			m_pbtType = PointRedemptionToCash;
		break;
	case PromoDollarsToGame:
		m_pbtType = PromoCreditsToCash;
		break;
	case CashFromGame:
		m_pbtType = CashUpload;
		break;
	case PromoDollarsFromGame:
		m_pbtType = Unspecified;
		m_promoPbtType = PromoCreditsUpload;
		break;
	case AllFromGame:
		m_pbtType = CashUpload;
		m_promoPbtType = PromoCreditsUpload;
		break;
	case ExternalCredit:
		m_pbtType = CreditTransfer;
		break;
	default:
		m_pbtType = Unspecified;
		break;
	}

	StoreTypesDataToNVRAM();
}

void CPersonalBankerTransfer::SetPromoPersonalBankerTransferType(PersonalBankerTransferType type)
{
	// the promo type is only used for NCEP Uploads
	m_promoPbtTransferType = type;

	switch (type)
	{
	case PromoDollarsFromGame:
		m_promoPbtType = PromoCreditsUpload;
		break;
	default:
		m_promoPbtType = Unspecified;
		break;
	}

	StoreTypesDataToNVRAM();
	StoreUpdatedDataToNVRAM();
}

void CPersonalBankerTransfer::SetCardNumber( const byte *cardNumber, int cardNumberLen )
{
	if ( m_cardNumber != NULL )
	{
		delete [] m_cardNumber;
	}

	m_cardNumber = new byte[CARD_ID_LEN];
	memset( m_cardNumber, 0, CARD_ID_LEN);
	m_cardNumberLen = CARD_ID_LEN;

	if ( cardNumber != NULL )
	{
		if (cardNumberLen > CARD_ID_LEN)
		{
			LogString(ERROR_LOG, _T("cardNumberLen=%d cardNumber=%s in %s"),
				(int)cardNumberLen,
				static_cast<LPCTSTR>(CUtilities::ToHexString(cardNumber, cardNumberLen).c_str()),
				static_cast<LPCTSTR>(__TFUNCTION__));
		}
		else
		{
			memcpy( m_cardNumber, cardNumber, cardNumberLen );
			m_cardNumberLen = cardNumberLen;
		}
	}

	StoreStartDataToNVRAM();
}


bool  CPersonalBankerTransfer::IsTransferTypeToGame(void)
{
	bool retVal = false;
	retVal = m_pbtTransferType == CashToGame || m_pbtTransferType == PointsToGame || m_pbtTransferType == PromoDollarsToGame || 
				(m_pbtTransferType == ExternalCredit && ((m_pbtType == CashDownload)||(m_pbtType == CanceledCreditTransfer)||(m_pbtType == CancelExternalCreditsToGame)));
	return 	retVal;
}

const byte * CPersonalBankerTransfer::GetCardNumber( int &cardNumberLen ) const
{
	cardNumberLen = m_cardNumberLen;

	return m_cardNumber;
}

PersonalBankerType CPersonalBankerTransfer::GetCancelledCode(PersonalBankerType type)
{
	// determines cancelled code based on personal banker transfer type
	PersonalBankerType cType = Unspecified;

	switch(type)
	{
	case CashDownload:
	{
		//For RCCL card the personal banker transfer type is ExternalCredit
		if (!(GetPersonalBankerTransferType() == ExternalCredit))
		{
		cType = CanceledCashDownload;
			LogString(PBT_LOG, "%s :  Personal Banker type set to after cacellation for transfer to game: %d ", __FUNCTION__, CanceledCashDownload);
		}
		else
		{
			cType = CancelExternalCreditsToGame;
			LogString(PBT_LOG, "%s :  Personal Banker type set to after cancellation for transfer type credit to account: %d ", __FUNCTION__, CancelExternalCreditsToGame);
		}
		break;
	}
	case PointRedemptionToCash:
		cType = CanceledPointRedemption;
		break;
	case PointRedemptionToPromoCredits:
		cType = CanceledPointRedemptionToPromoCredits;
		break;
	case PromoCreditsToCash:
	{
		//For RCCL card the personal banker transfer type is ExternalCredit
		if (!(GetPersonalBankerTransferType() == ExternalCredit))
		{
		cType = CanceledPromoCreditsToCash;
			LogString(PBT_LOG, "%s :  Personal Banker type set to after cacellation for transfer to game: %d ", __FUNCTION__, CanceledPromoCreditsToCash);
		}
		else
		{
			cType = CancelExternalCreditsToGame;
			LogString(PBT_LOG, "%s :  Personal Banker type set to after cancellation for transfer type credit to account: %d ", __FUNCTION__, CancelExternalCreditsToGame);
		}
		break;
	}
	case CashUpload:
		cType = CanceledCashUpload;
		break;
	case CashUploadAuto:
		cType = CanceledCashUploadAuto;
		break;
	case PointCashUploadAuto:
		cType = CanceledPointCashUploadAuto;
		break;
	case PromoCreditsUpload:
		cType = CanceledPromoCreditsUpload;
		break;
	case PromoCreditsUploadAuto:
		cType = CanceledPromoCreditsUploadAuto;
		break;
	default:
		cType = type;
	}
	return cType;
}

void CPersonalBankerTransfer::StoreTypesDataToNVRAM()
{
	if ( m_type == MEMORY_NVRAM )
	{
		byte nvramBuffer[NVRAM_PB_TRANSFER_TYPES_SIZE];

		byte *bufferPos = nvramBuffer;

		memcpy( bufferPos, &m_cancelledType, sizeof( m_cancelledType ) );
		bufferPos += sizeof( m_cancelledType );

		memcpy( bufferPos, &m_pbtType, sizeof( m_pbtType ) );
		bufferPos += sizeof( m_pbtType );

		memcpy( bufferPos, &m_pbtTransferType, sizeof( m_pbtTransferType ) );
		bufferPos += sizeof( m_pbtTransferType );

		memcpy( bufferPos, &m_promoPbtTransferType, sizeof( m_promoPbtTransferType ) );
		bufferPos += sizeof( m_promoPbtTransferType );

		int bytesInBuffer = bufferPos - nvramBuffer;
		if ( !Write( NVRAM_PB_TRANSFER_TYPES_OFFSET, nvramBuffer, bytesInBuffer ) )
			LogString(ERROR_LOG, "Could not write pb transfer types data to NVRAM" );
	}
}

void CPersonalBankerTransfer::StoreIDDataToNVRAM()
{
	if ( m_type == MEMORY_NVRAM )
	{

		byte nvramBuffer[NVRAM_PB_TRANSFER_IDS_SIZE];

		byte *bufferPos = nvramBuffer;

		memcpy( bufferPos, &m_pbtIDLength, sizeof( m_pbtIDLength ) );
		bufferPos += sizeof( m_pbtIDLength );

		memcpy( bufferPos, &m_pbtID, MAX_PBID_LENGTH );
		bufferPos += sizeof( m_pbtID );

		memcpy( bufferPos, &m_eftNumber, sizeof( m_eftNumber ) );
		bufferPos += sizeof( m_eftNumber );

		int bytesInBuffer = bufferPos - nvramBuffer;
		if ( !Write( NVRAM_PB_TRANSFER_IDS_OFFSET, nvramBuffer, bytesInBuffer ) )
			LogString(ERROR_LOG, "Could not write pb id data types data to NVRAM" );
	}
}

void CPersonalBankerTransfer::StoreStartDataToNVRAM()
{
	if ( m_type == MEMORY_NVRAM )
	{
		byte nvramBuffer[NVRAM_PB_TRANSFER_START_SIZE];

		byte *bufferPos = nvramBuffer;

		memcpy( bufferPos, &m_pbtPIN, sizeof( m_pbtPIN ) );
		bufferPos += sizeof( m_pbtPIN );

		memcpy( bufferPos, &m_sessionStartTickCount, sizeof( m_sessionStartTickCount ) );
		bufferPos += sizeof( m_sessionStartTickCount );

		memcpy( bufferPos, &m_bPointRedemptionRestricted, sizeof( m_bPointRedemptionRestricted ) );
		bufferPos += sizeof( m_bPointRedemptionRestricted );

		memcpy( bufferPos, &m_cardNumberLen, sizeof( m_cardNumberLen ) );
		bufferPos += sizeof( m_cardNumberLen );

		memcpy( bufferPos, m_cardNumber, m_cardNumberLen );
		bufferPos += MAX_PLAYER_CARD_NUMBER_LENGTH;
		
		memcpy( bufferPos, &m_bCEPOutEnabled, sizeof( m_bCEPOutEnabled ) );
		bufferPos += sizeof( m_bCEPOutEnabled );
		
		memcpy( bufferPos, &m_denomination, sizeof( m_denomination ) );
		bufferPos += sizeof( m_denomination );

		memcpy(bufferPos, &m_federalTaxDeductionInPennies, sizeof(m_federalTaxDeductionInPennies));
		bufferPos += sizeof(m_federalTaxDeductionInPennies);

		memcpy(bufferPos, &m_jackpotId, sizeof(m_jackpotId));
		bufferPos += sizeof(m_jackpotId);		

		int bytesInBuffer = bufferPos - nvramBuffer;
		if ( !Write( NVRAM_PB_TRANSFER_START_OFFSET, nvramBuffer, bytesInBuffer ) )
			LogString(ERROR_LOG, "Could not write pb start data types data to NVRAM" );
	}
}

void CPersonalBankerTransfer::StoreUpdatedDataToNVRAM()
{
	if ( m_type == MEMORY_NVRAM )
	{
		byte nvramBuffer[NVRAM_PB_TRANSFER_UPDATED_SIZE];

		byte *bufferPos = nvramBuffer;

		memcpy( bufferPos, &m_state_NotUsed, sizeof( m_state_NotUsed ) );
		bufferPos += sizeof( m_state_NotUsed );

		memcpy( bufferPos, &m_sequenceNumber, sizeof( m_sequenceNumber ) );
		bufferPos += sizeof( m_sequenceNumber );

		memcpy( bufferPos, &m_restrictedSequenceNumber, sizeof( m_restrictedSequenceNumber ) );
		bufferPos += sizeof( m_restrictedSequenceNumber );

		memcpy( bufferPos, &m_nonrestrictedSequenceNumber, sizeof( m_nonrestrictedSequenceNumber ) );
		bufferPos += sizeof( m_nonrestrictedSequenceNumber );

		memcpy( bufferPos, &m_originalCreditMeter, sizeof( m_originalCreditMeter ) );
		bufferPos += sizeof( m_originalCreditMeter );

		memcpy( bufferPos, &m_amountInPennies, sizeof( m_amountInPennies ) );
		bufferPos += sizeof( m_amountInPennies );

		memcpy( bufferPos, &m_restrictedAmountInPennies, sizeof( m_restrictedAmountInPennies ) );
		bufferPos += sizeof( m_restrictedAmountInPennies );

		memcpy( bufferPos, &m_nonrestrictedAmountInPennies, sizeof( m_nonrestrictedAmountInPennies ) );
		bufferPos += sizeof( m_nonrestrictedAmountInPennies );

		memcpy( bufferPos, &m_promoCancelledType, sizeof( m_promoCancelledType ) );
		bufferPos += sizeof( m_promoCancelledType );

		memcpy( bufferPos, &m_promoPbtType, sizeof( m_promoPbtType ) );
		bufferPos += sizeof( m_promoPbtType );

		memcpy(bufferPos, &m_pbtTransferFlags.m_uValue, sizeof(m_pbtTransferFlags.m_uValue));
		bufferPos += sizeof(m_pbtTransferFlags.m_uValue);

		int bytesInBuffer = bufferPos - nvramBuffer;
		if ( !Write( NVRAM_PB_TRANSFER_UPDATED_OFFSET, nvramBuffer, bytesInBuffer ) )
			LogString(ERROR_LOG, "Could not write pb update data types data to NVRAM" );
	}
}

void CPersonalBankerTransfer::StoreCardTimeToNVRAM()
{
	if ( m_type == MEMORY_NVRAM )
	{
		byte nvramBuffer[NVRAM_TIME_SIZE];

		int bytesInBuffer = CUtilities::GetNvramBufferFromTime( m_utcCardInTime, nvramBuffer, NVRAM_TIME_SIZE );

		if ( bytesInBuffer > 0 )
		{
			if ( !Write( NVRAM_PB_TRANSFER_CARDTIME_OFFSET, nvramBuffer, bytesInBuffer ) )
				LogString(ERROR_LOG, "Could not write pb card time data to NVRAM" );
		}
		else
		{
			LogString(ERROR_LOG, "Could not get pb card time to write to NVRAM" );
		}
	}
}

LPCTSTR CPersonalBankerTransfer::GetCancelledToken( void ) const
{
	PollerCancellationType type;

	if (GetPersonalBankerType() >= CanceledSessionRequest || m_cancelledType > _Unknown)
	{
		// regular pbt error
		type = m_cancelledType;
	}
	else
	{
		type = m_promoCancelledType;
	}

	if (type < 0 || _countof(PB_CANCELLATION_TOKEN) <= type)
	{
		LogString(ERROR_LOG, _T("Cancelled type=%d < 0 || %d <= Cancelled type=%d in %s"),
			(int)type,
			(int)_countof(PB_CANCELLATION_TOKEN),
			(int)type,
			static_cast<LPCTSTR>(__TFUNCTION__));
		type = _Unknown;
	}
	return PB_CANCELLATION_TOKEN[type];
}

void CPersonalBankerTransfer::ChangePersonalBankerTypeToAuthorization(void)
{
	switch(GetPersonalBankerType())
	{
	case CashDownload:
	{
		LogString(PBT_LOG, "%s :  Personal Banker type set to : %d  for authorization", __FUNCTION__, CashDownload);
		SetPersonalBankerType(AuthorizeCashToGame);
		break;
	}
	case PromoCreditsToCash:
	{
		LogString(PBT_LOG, "%s :  Personal Banker type set to : %d  for authorization", __FUNCTION__, PromoCreditsToCash);
		SetPersonalBankerType(AuthorizePromoToGame);
		break;
	}
	case PointRedemptionToCash:
		SetPersonalBankerType(AuthorizePointsToGameCash);
		break;
	case PointRedemptionToPromoCredits:
		SetPersonalBankerType(AuthorizePointsToGamePromo);
		break;
	//If credit to account for external credit provider
	case CreditTransfer:
		SetPersonalBankerType(AuthorizeExternalCreditsToGame);
		LogString(PBT_LOG, "%s :  SetPersonalBankerType : %d ", __FUNCTION__, AuthorizeExternalCreditsToGame);
		break;
	default:
		break;
	}
}

void CPersonalBankerTransfer::ChangePersonalBankerTypeToCanceled(void)
{
	PersonalBankerType origType(GetPersonalBankerType());
	switch(origType)
	{
    case AuthorizeExternalCreditsToGame:
        SetPersonalBankerType(CancelExternalCreditsToGame);
        LogString(PBT_LOG, "%s :  Reboot scenerio only : Personal Banker type set to  CancelExternalCreditsToGame: %d ", __FUNCTION__, GetPersonalBankerType());
        break;
	case CashUpload:
		SetPersonalBankerType(CanceledCashUpload);
		break;
	case CashUploadAuto:
		SetPersonalBankerType(CanceledCashUploadAuto);
		break;
	case PointCashUploadAuto:
		SetPersonalBankerType(CanceledPointCashUploadAuto);
		break;
	case CashDownload:
	{
		//nCompass need to send CancelExternalCreditsToGame for external credit provider
		if (GetPersonalBankerTransferType() == ExternalCredit)
		{
			LogString(PBT_LOG, "%s :  Personal Banker type set for cancellation", __FUNCTION__);
			SetPersonalBankerType(CancelExternalCreditsToGame);
		}
		else
		{
			LogString(PBT_LOG, "%s :  Personal Banker type set to  cash download cancelled", __FUNCTION__);
		SetPersonalBankerType(CanceledCashDownload);
		}
		LogString(PBT_LOG, "%s :  Personal Banker type set to  cash download cancelled: %d ", __FUNCTION__, GetPersonalBankerType());

		break;
	}
	case PromoCreditsToCash:
	{
		//nCompass need to send CancelExternalCreditsToGame for external credit provider
		if (GetPersonalBankerTransferType() == ExternalCredit)
		{
			SetPersonalBankerType(CancelExternalCreditsToGame);
		}
		else
		{
		SetPersonalBankerType(CanceledPromoCreditsToCash);
		}
		LogString(PBT_LOG, "%s :  Personal Banker type set to  cash download cancelled: %d ", __FUNCTION__, GetPersonalBankerType());
		break;
	}
	case PointRedemptionToCash:
		SetPersonalBankerType(CanceledPointRedemption);
		break;
	case PointRedemptionToPromoCredits:
		SetPersonalBankerType(CanceledPointRedemptionToPromoCredits);
		break;
	case PromoCreditsUpload:
		SetPersonalBankerType(CanceledPromoCreditsUpload);
		break;
	case PromoCreditsUploadAuto:
		SetPersonalBankerType(CanceledPromoCreditsUploadAuto);
		break;
	case CreditTransfer:
	{
		LogString(PBT_LOG, "%s :  Personal Banker type set to: %d for external transfer to game for cancellation ", __FUNCTION__, CanceledCreditTransfer);
		SetPersonalBankerType(CanceledCreditTransfer);
		break;
	}
    default:
        {
            // not a relevant value for cancelation
        }
 	}

	// Need to cancel promo if it exists
	switch( GetPromoPersonalBankerType() )
	{
	case PromoCreditsUpload:
		SetPromoPersonalBankerType(CanceledPromoCreditsUpload);
		if (GetPersonalBankerType() < CanceledCashUpload)
			SetPersonalBankerType(CanceledPromoCreditsUpload);
		break;
	case PromoCreditsUploadAuto:
		SetPromoPersonalBankerType(CanceledPromoCreditsUploadAuto);
		if (GetPersonalBankerType() < CanceledCashUpload)
			SetPersonalBankerType(CanceledPromoCreditsUploadAuto);
		break;
    default:
        {
            // not a relevant type
        }
	}

	LogString(PBT_LOG,"%s %d origType=%s newType=%s", __FUNCTION__, __LINE__,
		GetPBTypeText(origType).c_str(), GetPBTypeText(GetPersonalBankerType()).c_str());
}

void CPersonalBankerTransfer::ChangePersonalBankerPromoTypeToCanceled( void )
{
	switch( GetPromoPersonalBankerType() )
	{
	case PromoCreditsUpload:
		SetPromoPersonalBankerType(CanceledPromoCreditsUpload);
		if (GetPersonalBankerType() < CanceledCashUpload)
			SetPersonalBankerType(CanceledPromoCreditsUpload);
		break;
	case PromoCreditsUploadAuto:
		SetPromoPersonalBankerType(CanceledPromoCreditsUploadAuto);
		if (GetPersonalBankerType() < CanceledCashUpload)
			SetPersonalBankerType(CanceledPromoCreditsUploadAuto);
		break;
	default:
		{
			LogString(WARNING_LOG, "GetPromoPersonalBankerType() had unexpected value=%d in ChangePersonalBankerPromoTypeToCanceled( void )",
				(int)GetPromoPersonalBankerType());
		}
	}
}

/// <summary>
/// Sets the inactive location.
/// </summary>
/// <returns></returns>
bool CPersonalBankerTransfer::SetInactiveLocation()
{	
	bool bReturnValue = CMemoryStorage::SetInactiveLocation( NVRAM_PB_TRANSFER_CHECKBYTE_OFFSET, (BYTE)~NVRAM_PB_TRANSFER_CHECKBYTE_VALUE );

	return bReturnValue;
}

void CPersonalBankerTransfer::LogPBT(LPCTSTR descriptionFormat, ...) const
{
	va_list args;
	va_start(args, descriptionFormat);
	LogPBTV(this, descriptionFormat, args);
	va_end(args);
}

void CPersonalBankerTransfer::LogPBT(const CPersonalBankerTransfer *pbt, LPCTSTR descriptionFormat, ...)
{
	va_list args;
	va_start(args, descriptionFormat);
	LogPBTV(pbt, descriptionFormat, args);
	va_end(args);
}

void CPersonalBankerTransfer::LogPBTV(const CPersonalBankerTransfer *pbt, LPCTSTR descriptionFormat, va_list args)
{
	std::string pbtString;
	if (descriptionFormat != nullptr)
	{
		pbtString = FormatStringV(descriptionFormat, args);
	}

	if (pbt != nullptr)
	{
		pbtString += _T(" -- ");
		pbtString += pbt->ToStringForLog();
	}

	LogStringNoFormat(PBT_LOG, pbtString);
}

bool CPersonalBankerTransfer::IsAutoUploadPIN() const
{
	bool returnValue = m_pbtPIN == PBT_AUTO_UPLOAD_PIN;
	return returnValue;
}

std::string CPersonalBankerTransfer::ToStringForLog() const
{
// #pragma region Field Names
	const std::string PBTID = "PBTId";
	const std::string CARDNUMBER = "CardNumber";
	const std::string EFTNUMBER = "EFTNumber";
	const std::string RESTRICTEDSEQUENCENUMBER = "RestrictedSequenceNumber";
	const std::string NONRESTRICTEDSEQUENCENUMBER = "NonRestrictedSequenceNumber";
	const std::string SEQUENCENUMBER = "SequenceNumber";
	const std::string PBTSESSIONSTARTTICKS = "PBTSessionStartTicks";
	const std::string PBTYPE = "PBType";
	const std::string PROMOPBTYPE = "PromoPBType";
	const std::string PBTRANSFERTYPE = "PBTransferType";
	const std::string PROMOPBTRANSFERTYPE = "PromoPBTransferType";
	const std::string POINTREDEMPTOINRESTRICTED = "PointRedemptionRestricted";
	const std::string CEPOUTENABLED = "CEPOutEnabled";
	const std::string DENOMINATION = "Denomination";
	const std::string ORIGINALCREDITMETER = "OriginalCreditmeter";
	const std::string AMOUNTINPENNIES = "AmountInPennies";
	const std::string RESTRICTEDAMOUNTINPENNIES = "RestrictedAmountInPennies";
	const std::string NONRESTRICTEDAMOUNTINPENNIES = "NonRestrictedAmountInPennies";
	const std::string POLLERCANCELLATIONTYPE = "PollerCancellationType";
	const std::string PROMOCANCELLEDTYPE = "PromoCancelledType";
	const std::string UTCCARDINTIME = "UTCCardInTime";
	const std::string MINCASHUP = "MinimumCashUp";
	const std::string MAXCASHUP = "MaximumCashUp";
	const std::string MINPROMOUP = "MinimumPromoUp";
	const std::string MAXPROMOUP = "MaximumPromoUp";
	const std::string PBTSENT = "PBTSent";

// #pragma endregion Field Names

	TCHAR PBTId[MAX_PBID_LENGTH + 1];
	strncpy(PBTId, m_pbtID, MAX_PBID_LENGTH);
	PBTId[MAX_PBID_LENGTH] = '\0';
	long long cardNumber = 0;
	for (int i = 0; i < m_cardNumberLen; i++)
	{
		cardNumber <<= 8;
		cardNumber += m_cardNumber[i];
	}
	
	std::string string1;
	std::string timeStr = CUtilities::FormatDateTimeString(m_utcCardInTime);

	string1 = FormatString("%s=%s; %s=%010llX; %s=%d; %s=%d; %s=%d; %s=%d; %s=%u; %s=%s; %s=%s; %s=%s; %s=%s; %s=%d; %s=%d; %s=%d; %s=%lld; %s=%lld; %s=%lld; %s=%lld; %s=%s; %s=%s; %s=%s; ",
		PBTID.c_str(), PBTId,
		CARDNUMBER.c_str(), cardNumber,
		EFTNUMBER.c_str(), m_eftNumber,
		RESTRICTEDSEQUENCENUMBER.c_str(), m_restrictedSequenceNumber,
		NONRESTRICTEDSEQUENCENUMBER.c_str(), m_nonrestrictedSequenceNumber,
		SEQUENCENUMBER.c_str(), m_sequenceNumber,
		PBTSESSIONSTARTTICKS.c_str(), m_sessionStartTickCount,
		PBTYPE.c_str(), GetPBTypeText(m_pbtType).c_str(),
		PROMOPBTYPE.c_str(), GetPBTypeText(m_promoPbtType).c_str(),
		PBTRANSFERTYPE.c_str(), GetPBTTypeText(m_pbtTransferType).c_str(),
		PROMOPBTRANSFERTYPE.c_str(), GetPBTTypeText(m_promoPbtTransferType).c_str(),
		POINTREDEMPTOINRESTRICTED.c_str(), m_bPointRedemptionRestricted,
		CEPOUTENABLED.c_str(), m_bCEPOutEnabled,
		DENOMINATION.c_str(), m_denomination,
		ORIGINALCREDITMETER.c_str(), (long long)m_originalCreditMeter,
		AMOUNTINPENNIES.c_str(), (long long)m_amountInPennies,
		RESTRICTEDAMOUNTINPENNIES.c_str(), (long long)m_restrictedAmountInPennies,
		NONRESTRICTEDAMOUNTINPENNIES.c_str(), (long long)m_nonrestrictedAmountInPennies,
		POLLERCANCELLATIONTYPE.c_str(), GetPollerCancellationType(m_cancelledType).c_str(),
		PROMOCANCELLEDTYPE.c_str(), GetPollerCancellationType(m_promoCancelledType).c_str(),
		UTCCARDINTIME.c_str(), static_cast<LPCTSTR>(timeStr.c_str())
		);

	std::string string2;
	string2 = FormatString("%s=%d; %s=%d; %s=%d, %s=%d; %s=%d;",
		MINCASHUP.c_str(), m_minimumCashUp,
		MAXCASHUP.c_str(), m_maximumCashUp,
		MINPROMOUP.c_str(), m_minimumPromoUp,
		MAXPROMOUP.c_str(), m_maximumPromoUp,
		PBTSENT.c_str(), (int)m_PBTSent
		);
	return string1 + string2;
}

std::string CPersonalBankerTransfer::GetPBTTypeText(PersonalBankerTransferType pbtType) const
{
	std::string value;
	if (pbtType <= AllFromGame)
	{
		value = PBTTypeStrings[pbtType];
	}
	else
	{
		value = "";
	}
	return value;
}

std::string CPersonalBankerTransfer::GetPBTypeText(PersonalBankerType pbType) const
{
	std::string value;

	switch (pbType)
	{
	case Unspecified:
		value = "Unspecified";
		break;
	case SessionRequest:
		value = "SessionRequest";
		break;
	case CashDownload:
		value = "CashDownload";
		break;
	case PromoCreditsToGame:
		value = "PromoCreditsToGame";
		break;
	case PromoCreditsToCash:
		value = "PromoCreditsToCash";
		break;
	case PointRedemptionToCash:
		value = "PointRedemptionToCash";
		break;
	case CashUpload:
		value = "CashUpload";
		break;
	case CashUploadAuto:
		value = "CashUploadAuto";
		break;
	case TransferCreditsFromGame:
		value = "TransferCreditsFromGame";
		break;
	case SessionRelease:
		value = "SessionRelease";
		break;
	case ChangePinAtSentinel:
		value = "ChangePinAtSentinel";
		break;
	case PointRedemptionToPromoCredits:
		value = "PointRedemptionToPromoCredits";
		break;
	case PointCashUpload:
		value = "PointCashUpload";
		break;
	case PointCashUploadAuto:
		value = "PointCashUploadAuto";
		break;
	case PromoCreditsUpload:
		value = "PromoCreditsUpload";
		break;
	case PromoCreditsUploadAuto:
		value = "PromoCreditsUploadAuto";
		break;
	case CreditTransfer:
		value = "CreditTransfer";
		break;
	case PendingResponse:
		value = "PendingResponse";
		break;
	case CanceledSessionRequest:
		value = "CanceledSessionRequest";
		break;
	case CanceledCashDownload:
		value = "CanceledCashDownload";
		break;
	case CanceledPromoCreditsToGame:
		value = "CanceledPromoCreditsToGame";
		break;
	case CanceledPromoCreditsToCash:
		value = "CanceledPromoCreditsToCash";
		break;
	case CanceledPointRedemption:
		value = "CanceledPointRedemption";
		break;
	case CanceledCashUpload:
		value = "CanceledCashUpload";
		break;
	case CanceledTransferCreditsFromGame:
		value = "CanceledTransferCreditsFromGame";
		break;
	case CanceledSessionRelease:
		value = "CanceledSessionRelease";
		break;
	case CanceledChangePinAtSentinel:
		value = "CanceledChangePinAtSentinel";
		break;
	case CanceledPointRedemptionToPromoCredits:
		value = "CanceledPointRedemptionToPromoCredits";
		break;
	case CanceledPointCashUpload:
		value = "CanceledPointCashUpload";
		break;
	case CanceledPromoCreditsUpload:
		value = "CanceledPromoCreditsUpload";
		break;
	case CanceledPromoCreditsUploadAuto:
		value = "CanceledPromoCreditsUploadAuto";
		break;
	case CanceledCreditTransfer:
		value = "CanceledCreditTransfer";
		break;
	case CanceledCashUploadAuto:
		value = "CanceledCashUploadAuto";
		break;
	case CanceledPointCashUploadAuto:
		value = "CanceledPointCashUploadAuto";
		break;
	case AuthorizeCashToGame:
		value = "AuthorizeCashToGame";
		break;
	case AuthorizePromoToGame:
		value = "AuthorizePromoToGame";
		break;
	case AuthorizePointsToGameCash:
		value = "AuthorizePointsToGameCash";
		break;
	case AuthorizePointsToGamePromo:
		value = "AuthorizePointsToGamePromo";
		break;
	case AuthorizeExternalCreditsToGame:
		value = "AuthorizeExternalCreditsToGame";
		break;
	case UnknownCreditSource:
		value = "UnknownCreditSource";
		break;
	case InvalidTransIDFromSystem:
		value = "InvalidTransIDFromSystem";
		break;
	case NonMatchingTransactionID:
		value = "NonMatchingTransactionID";
		break;
	case UnexpectedRestrictedToGame:
		value = "UnexpectedRestrictedToGame";
		break;
	case UnexpectedUnRestrictedToGame:
		value = "UnexpectedUnRestrictedToGame";
		break;
	case UnexpectedCashToGame:
		value = "UnexpectedCashToGame";
		break;
	case UnexpectedRestrictedFromGame:
		value = "UnexpectedRestrictedFromGame";
		break;
	case UnexpectedUnRestrictedFromGame:
		value = "UnexpectedUnRestrictedFromGame";
		break;
	case UnexpectedCashFromGame:
		value = "UnexpectedCashFromGame";
		break;
	case UnexpectedRestrictedToGameErr:
		value = "UnexpectedRestrictedToGameErr";
		break;
	case UnexpectedUnrestrictedToGameErr:
		value = "UnexpectedUnrestrictedToGameErr";
		break;
	case UnexpectedCashToGameErr:
		value = "UnexpectedCashToGameErr";
		break;
	case UnexpectedUnrestrictedFromGameErr:
		value = "UnexpectedUnrestrictedFromGameErr";
		break;
	case UnexpectedCashFromGameErr:
		value = "UnexpectedCashFromGameErr";
		break;
	case UnknownError:
		value = "UnknownError";
		break;
	case CancelExternalCreditsToGame:
		value = "CancelExternalCreditsToGame";
		break;
	default:
		value = FormatString("%d", pbType);
		break;
	}
	return value;
}

std::string CPersonalBankerTransfer::GetPollerCancellationType(PollerCancellationType pcType) const
{
	std::string value;
	if (pcType <= CreditTransferErrorTimeout)
	{
		value = PollerCancellationTypeStrings[pcType];
	}
	else
	{
		value = "";
	}
	return value;
}

bool CPersonalBankerTransfer::IsValidID(long slotmast_ID, const char *pbtId, int pbtIdLength)
{
	const int SLOTMASTPOS = 5;
	const int SLOTMASTSIZE = 6;
	bool  valid = true;
	char slotmastChar[SLOTMASTSIZE + 1];

	// Check length
	if (pbtIdLength != MAX_PBID_LENGTH)
	{
		valid = false;
	}
	else
	{
		char *slotmastpos = (char *)pbtId + SLOTMASTPOS;

		// Copy slotmast substring to array
		memcpy_s(slotmastChar, SLOTMASTSIZE + 1, slotmastpos, SLOTMASTSIZE);
		slotmastChar[SLOTMASTSIZE] = '\0';
		std::string slotmastStr = FormatString("%06ld", slotmast_ID);
		if (slotmastStr != slotmastChar)
		{
			valid = false;
		}
	}

	return valid;
}

bool CPersonalBankerTransfer::VerifyMaxAmounts()
{
	bool retVal = true;

	if (m_amountInPennies > CDiagnostics::Instance().GetMaxPBTAmount() ||
		m_restrictedAmountInPennies > CDiagnostics::Instance().GetMaxPBTAmount() ||
		m_nonrestrictedAmountInPennies > CDiagnostics::Instance().GetMaxPBTAmount())
	{
		retVal = false;
	}
	return retVal;
}

void CPersonalBankerTransfer::AppendCashoutErrorFlags(WORD error)
{
	m_cashoutErrorFlags = m_cashoutErrorFlags | error;
}

WORD CPersonalBankerTransfer::GetCashoutErrorFlags() const
{
	return m_cashoutErrorFlags;
}

void CPersonalBankerTransfer::AppendAutoUploadErrorFlags(WORD error)
{
	m_autoUploadErrorFlags = m_autoUploadErrorFlags | error;
}

WORD CPersonalBankerTransfer::GetAutoUploadErrorFlags() const
{
	return m_autoUploadErrorFlags;
}
