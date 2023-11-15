#include "stdafx.h"
#include "PersonalBankerInfo.h"
#include "resource.h"

// Sentinel includes.
#include "Logging/LogString.h"

using namespace std;

static const DWORD DEFAULT_POINTS_PER_DOLLAR = 1;
static const DWORD MININUM_TRANSFER_IN_DOLLARS = 1;

const DWORD FEECONVERSION_DENOMINATOR = 100000;// 100000 = thousandth of a percent to dollars
const string FEE_VERIFICATION_TEXT("A $[fee] service fee will be applied to your account to download $[amount]. Do you wish to continue?");
const string NO_FEE_VERIFICATION_TEXT("You are about to download $[amount]. Do you wish to continue?");

std::map<PBError,std::string> PBErrorString = {
	{AccessDenied,  "AccessDenied"},
	{InvalidPin,    "InvalidPin"},
	{AccountInUse,  "AccountInUse"},
	{AccountLocked, "AccountLocked"},
	{NewPinRequired,"NewPinRequired"},
	{AccessError,   "AccessError"},
	{AcctHasNoPin,  "AcctHasNoPin"},
	{CreditTransferOtherIssue,           "CreditTransferOtherIssue"},
	{CreditTransferCommserror,           "CreditTransferCommserror"},
	{CreditTransferInsufficientCredits,  "CreditTransferInsufficientCredits"},
	{InvalidID,     "InvalidID"},
	{NoError,       "NoError"},
};

CPersonalBankerInfo::CPersonalBankerInfo(MEMORY_TYPE memoryType, bool bBuildFromNVRAM)
{
	m_type = memoryType;
	m_dwOffset = NVRAM_PB_INFO_OFFSET;
	m_dwAllowedSize = NVRAM_PB_INFO_SIZE;
	m_bOffsetIsSet = true;

	ZeroMemory(m_cardNumber, sizeof(m_cardNumber));
	ZeroMemory(m_PBTID, sizeof(m_PBTID));
	m_PBIDLength = 0;
	m_bTransferCashToGameEnabled = false;
	m_bTransferPointsToGameEnabled = false;
	m_bTransferPromoDollarsToGameEnabled = false;
	m_bTransferCashFromGameEnabled = false;
	m_cepFromGame = false;
	m_bTransferPromoFromGameEnabled = false;
	m_showToGameButton = false;
	m_showFromGameButton = false;
	m_bGameDenomMatches = true;
	m_bEGMInitialized = true;
	m_externalCreditVerificationEnabled = false;
	m_showPointsVsAccountBalance = false;
	m_bPartialTransferCashFromGameEnabled = false;
	m_isSessionForceExit = false;

	m_errorCode = AccessDenied;
	m_cashBalance = 0;
	m_pbFlags.uValue = 0;
	m_promo2Balance = 0;
	m_pointsAvailable = 0;
	m_pointsPerDollar = DEFAULT_POINTS_PER_DOLLAR;
	m_sequenceNumber = 0;
	m_minimumPoints = 0;
	m_maximumPoints = 0;
	m_minimumCashDown = 0;
	m_maximumCashDown = 0;
	m_minimumCashUp = 0;
	m_maximumCashUp = 0;
	m_minimumPromoDown = 0;
	m_maximumPromoDown = 0;
	m_minimumPromoUp = 0;
	m_maximumPromoUp = 0;
	m_promoBalanceOnGame = 0;
	m_cashBalanceOnGame = 0;
	m_externalCreditProviderEnabled = false;
	m_creditFee = 0;
	m_creditFeeFixed = false;
	m_currentCreditTransferAmount = 0;
	m_extCreditTransferType = Invalid_Transfer;

	if ( m_type == MEMORY_NVRAM )
	{
		if ( bBuildFromNVRAM )
		{
			BuildYourself();
		}
		else
		{
			StoreToNVRAM();

		}
		SetActiveLocation(NVRAM_PB_INFO_CHECKBYTE_OFFSET, NVRAM_PB_INFO_CHECKBYTE_VALUE);
	}
}

CPersonalBankerInfo::CPersonalBankerInfo(const CPersonalBankerInfo& rhs)
{
	m_type = MEMORY_NONE;
	*this = rhs;
}

CPersonalBankerInfo::~CPersonalBankerInfo(void)
{
	if ( m_type == MEMORY_NVRAM )
	{
		SetActiveLocation( NVRAM_PB_INFO_CHECKBYTE_OFFSET, (BYTE)~NVRAM_PB_INFO_CHECKBYTE_VALUE );
	}
}

CPersonalBankerInfo & CPersonalBankerInfo::operator=( const CPersonalBankerInfo &rhs )
{
	 const byte * temp = rhs.GetCardNumber();
	 memcpy(m_cardNumber, temp, CARD_ID_LEN);
	
	 m_errorCode = rhs.GetErrorCode();
	 m_cashBalance = rhs.GetCashBalance();
	 m_pbFlags.uValue = rhs.GetPBFlags();
	 m_promo2Balance = rhs.GetPromo2Balance();
	 m_pointsAvailable = rhs.GetPointsAvailable();
	 m_pointsPerDollar = rhs.GetPointsPerDollar();
	 m_sequenceNumber = rhs.GetSequenceNumber();
	 m_minimumPoints = rhs.GetMinimumPoints();
	 m_maximumPoints = rhs.GetMaximumPoints();
	 m_minimumCashDown = rhs.GetMinimumCashDown();
	 m_maximumCashDown = rhs.GetMaximumCashDown();
	 m_minimumCashUp = rhs.GetMinimumCashUp();
	 m_maximumCashUp = rhs.GetMaximumCashUp();
	 m_minimumPromoDown = rhs.GetMinimumPromoDown();
	 m_maximumPromoDown = rhs.GetMaximumPromoDown();
	 m_minimumPromoUp = rhs.GetMinimumPromoUp();
	 m_maximumPromoUp = rhs.GetMaximumPromoUp();
	 m_promoBalanceOnGame = rhs.GetPromoBalanceOnGame();
	 m_cashBalanceOnGame = rhs.GetCashBalanceOnGame();
	 SetPersonalBankerID(rhs.GetPersonalBankerID(), rhs.GetPBIDLength());

	m_bTransferCashToGameEnabled = rhs.IsTransferCashToGameEnabled();
	m_bTransferPointsToGameEnabled = rhs.IsTransferPointsToGameEnabled();
	m_bTransferPromoDollarsToGameEnabled = rhs.IsTransferPromoDollarsToGameEnabled();
	m_bTransferCashFromGameEnabled = rhs.IsTransferCashFromGameEnabled();
	m_cepFromGame = rhs.m_cepFromGame;
	m_bPartialTransferCashFromGameEnabled = rhs.IsPartialTransferCashFromGameEnabled();
	m_bGameDenomMatches = rhs.DoesGameDenomMatch();
	m_bEGMInitialized = rhs.m_bEGMInitialized;
	m_bTransferPromoFromGameEnabled = rhs.IsTransferPromoFromGameEnabled();
	m_showToGameButton = rhs.m_showToGameButton;
	m_showFromGameButton = rhs.m_showFromGameButton;
	m_externalCreditProviderEnabled = rhs.m_externalCreditProviderEnabled;
	m_creditFee = rhs.m_creditFee;
	m_creditFeeFixed = rhs.m_creditFeeFixed;
	m_extCreditTransferType = rhs.m_extCreditTransferType;
	m_externalCreditVerificationEnabled = rhs.m_externalCreditVerificationEnabled;
	m_showPointsVsAccountBalance = rhs.m_showPointsVsAccountBalance;
	m_currentCreditTransferAmount = rhs.m_currentCreditTransferAmount;
	m_isSessionForceExit = rhs.m_isSessionForceExit;

	StoreToNVRAM();

	return *this;
}

void CPersonalBankerInfo::BuildYourself( void )
{
	if (m_type == MEMORY_NVRAM)
	{
		byte nvramBuffer[NVRAM_PB_INFO_SIZE - NVRAM_PB_INFO_CHECKBYTE_SIZE];

		if ( !Read(  NVRAM_PB_INFO_OFFSET + NVRAM_PB_INFO_CHECKBYTE_SIZE, nvramBuffer, NVRAM_PB_INFO_SIZE - NVRAM_PB_INFO_CHECKBYTE_SIZE ) )
			LogString( ERROR_LOG, "Could not read pb info data from NVRAM" );

		byte *bufferPos = nvramBuffer;
		memcpy( m_cardNumber, bufferPos, CARD_ID_LEN );
		bufferPos += CARD_ID_LEN;
		memcpy( &m_errorCode, bufferPos, sizeof( m_errorCode ) );
		bufferPos += sizeof( m_errorCode );
		memcpy( &m_cashBalance, bufferPos, sizeof( m_cashBalance ) );
		bufferPos += sizeof( m_cashBalance );
		memcpy( &m_pbFlags.uValue, bufferPos, sizeof(m_pbFlags.uValue) );
		bufferPos += sizeof(m_pbFlags.uValue);
		memcpy( &m_promo2Balance, bufferPos, sizeof( m_promo2Balance ) );
		bufferPos += sizeof( m_promo2Balance );
		memcpy( &m_pointsAvailable, bufferPos, sizeof( m_pointsAvailable ) );
		bufferPos += sizeof( m_pointsAvailable );
		memcpy( &m_pointsPerDollar, bufferPos, sizeof( m_pointsPerDollar ) );
		bufferPos += sizeof( m_pointsPerDollar );
		memcpy( &m_sequenceNumber, bufferPos, sizeof( m_sequenceNumber ) );
		bufferPos += sizeof( m_sequenceNumber );
		memcpy( &m_minimumPoints, bufferPos, sizeof( m_minimumPoints ) );
		bufferPos += sizeof( m_minimumPoints );
		memcpy( &m_maximumPoints, bufferPos, sizeof( m_maximumPoints ) );
		bufferPos += sizeof( m_maximumPoints );
		memcpy( &m_minimumCashDown, bufferPos, sizeof( m_minimumCashDown ) );
		bufferPos += sizeof( m_minimumCashDown );
		memcpy( &m_maximumCashDown, bufferPos, sizeof( m_maximumCashDown ) );
		bufferPos += sizeof( m_maximumCashDown );
		memcpy( &m_minimumCashUp, bufferPos, sizeof( m_minimumCashUp ) );
		bufferPos += sizeof( m_minimumCashUp );
		memcpy( &m_maximumCashUp, bufferPos, sizeof( m_maximumCashUp ) );
		bufferPos += sizeof( m_maximumCashUp );
		memcpy( &m_minimumPromoDown, bufferPos, sizeof( m_minimumPromoDown ) );
		bufferPos += sizeof( m_minimumPromoDown );
		memcpy( &m_maximumPromoDown, bufferPos, sizeof( m_maximumPromoDown ) );
		bufferPos += sizeof( m_maximumPromoDown );
		memcpy( &m_PBIDLength, bufferPos, sizeof( m_PBIDLength ) );
		m_PBIDLength = max(0, m_PBIDLength);
		m_PBIDLength = min(m_PBIDLength, MAX_PBID_LENGTH);
		bufferPos += sizeof( m_PBIDLength );
		memcpy( &m_PBTID, bufferPos, MAX_PBID_LENGTH );
		bufferPos += MAX_PBID_LENGTH;
		memcpy( &m_minimumPromoUp, bufferPos, sizeof( m_minimumPromoUp ) );
		bufferPos += sizeof( m_minimumPromoUp );
		memcpy( &m_maximumPromoUp, bufferPos, sizeof( m_maximumPromoUp ) );
		bufferPos += sizeof( m_maximumPromoUp );
		bufferPos += sizeof(byte); // m_externalCreditProviderEnabled is not built from NVRAM.
		memcpy( &m_creditFee, bufferPos, sizeof( m_creditFee ) );
		bufferPos += sizeof(m_creditFee);
		bufferPos += sizeof( m_creditFeeFixed );//Skip m_creditFeeFixed

		LogString(PBT_LOG, "%s :  External Credit Transfer Type : %d read from nvRam at buffer position = %p", __FUNCTION__, m_extCreditTransferType, bufferPos);
		memcpy(&m_extCreditTransferType, bufferPos, sizeof(m_extCreditTransferType));
	}
}

// NOTE: Keep this function static.
bool CPersonalBankerInfo::DoesPersonalBankerInfoExistInNVRAM( void )
{
	bool bExists = CMemoryStorage::IntegrityCheck() && IsActiveLocation( NVRAM_PB_INFO_CHECKBYTE_OFFSET, NVRAM_PB_INFO_CHECKBYTE_VALUE );

	if ( !bExists )
		SetInactiveLocation( NVRAM_PB_INFO_CHECKBYTE_OFFSET, (BYTE)~NVRAM_PB_INFO_CHECKBYTE_VALUE );

	return bExists;
}

//added isEFT for TR #19343
void CPersonalBankerInfo::SetConfigData( const CConfig &config, bool bEGMIsInitialized, bool bIsEFT )
{
	m_bTransferCashToGameEnabled = config.PBTCashEnabled();
	m_bTransferPointsToGameEnabled = (config.PBTPointsEnabled()&& !bIsEFT ) || config.PBTPointsAsPromo();
	m_bTransferPromoDollarsToGameEnabled = config.PBTPromoEnabled(); //config.PBTPointsAsPromo();
	m_bTransferCashFromGameEnabled = config.PBFromGameEnabled();
	m_cepFromGame = config.CEPOutEnabled();
	m_bGameDenomMatches = config.GetGameDenom() == config.GetDenomination();
	m_bTransferPromoFromGameEnabled = config.NCEPOutEnabled(); //used for ncep out project
	m_bPartialTransferCashFromGameEnabled = config.PartialWATOutEnabled() && !bIsEFT;
	m_bEGMInitialized = bEGMIsInitialized;
	m_externalCreditVerificationEnabled = config.ExternalCreditVerificationEnabled();
	m_showPointsVsAccountBalance = config.GetShowPointsVsAccountBalance();
}

void CPersonalBankerInfo::StoreToNVRAM( void )
{
	if ( m_type == MEMORY_NVRAM )
	{
		byte nvramBuffer[NVRAM_PB_INFO_SIZE - NVRAM_PB_INFO_CHECKBYTE_SIZE];

		byte *bufferPos = nvramBuffer;

		memcpy( bufferPos, m_cardNumber, CARD_ID_LEN );
		bufferPos += CARD_ID_LEN;

		memcpy( bufferPos, &m_errorCode, sizeof( m_errorCode ) );
		bufferPos += sizeof( m_errorCode );

		memcpy( bufferPos, &m_cashBalance, sizeof( m_cashBalance ) );
		bufferPos += sizeof( m_cashBalance );

		memcpy( bufferPos, &m_pbFlags.uValue, sizeof(m_pbFlags.uValue) );
		bufferPos += sizeof(m_pbFlags.uValue);

		memcpy( bufferPos, &m_promo2Balance, sizeof( m_promo2Balance ) );
		bufferPos += sizeof( m_promo2Balance );

		memcpy( bufferPos, &m_pointsAvailable, sizeof( m_pointsAvailable ) );
		bufferPos += sizeof( m_pointsAvailable );

		memcpy( bufferPos, &m_pointsPerDollar, sizeof( m_pointsPerDollar ) );
		bufferPos += sizeof( m_pointsPerDollar );

		memcpy( bufferPos, &m_sequenceNumber, sizeof( m_sequenceNumber ) );
		bufferPos += sizeof( m_sequenceNumber );

		memcpy( bufferPos, &m_minimumPoints, sizeof( m_minimumPoints ) );
		bufferPos += sizeof( m_minimumPoints );

		memcpy( bufferPos, &m_maximumPoints, sizeof( m_maximumPoints ) );
		bufferPos += sizeof( m_maximumPoints );

		memcpy( bufferPos, &m_minimumCashDown, sizeof( m_minimumCashDown ) );
		bufferPos += sizeof( m_minimumCashDown );

		memcpy( bufferPos, &m_maximumCashDown, sizeof( m_maximumCashDown ) );
		bufferPos += sizeof( m_maximumCashDown );

		memcpy( bufferPos, &m_minimumCashUp, sizeof( m_minimumCashUp ) );
		bufferPos += sizeof( m_minimumCashUp );

		memcpy( bufferPos, &m_maximumCashUp, sizeof( m_maximumCashUp ) );
		bufferPos += sizeof( m_maximumCashUp );

		memcpy( bufferPos, &m_minimumPromoDown, sizeof( m_minimumPromoDown ) );
		bufferPos += sizeof( m_minimumPromoDown );

		memcpy( bufferPos, &m_maximumPromoDown, sizeof( m_maximumPromoDown ) );
		bufferPos += sizeof( m_maximumPromoDown );

		memcpy( bufferPos, &m_PBIDLength, sizeof( m_PBIDLength ) );
		bufferPos += sizeof( m_PBIDLength );

		memcpy( bufferPos, m_PBTID, MAX_PBID_LENGTH );
		bufferPos += MAX_PBID_LENGTH;

		memcpy( bufferPos, &m_minimumPromoUp, sizeof( m_minimumPromoUp ) );
		bufferPos += sizeof( m_minimumPromoUp );

		memcpy( bufferPos, &m_maximumPromoUp, sizeof( m_maximumPromoUp ) );
		bufferPos += sizeof( m_maximumPromoUp );

		byte tmpByte = m_externalCreditProviderEnabled ? 1 : 0;
		memcpy( bufferPos, &tmpByte, sizeof( tmpByte ) );
		bufferPos += sizeof( tmpByte );

		memcpy( bufferPos, &m_creditFee, sizeof( m_creditFee ) );
		bufferPos += sizeof( m_creditFee );

		tmpByte = m_creditFeeFixed ? 1 : 0;
		memcpy( bufferPos, &tmpByte, sizeof( tmpByte ) );
		bufferPos += sizeof( tmpByte );

		memcpy(bufferPos, &m_extCreditTransferType, sizeof(m_extCreditTransferType));
		bufferPos += sizeof(m_extCreditTransferType);
		LogString(PBT_LOG, "%s :  External Credit Transfer Type : %d read from nvRam at buffer position = %p", __FUNCTION__, m_extCreditTransferType, bufferPos);

		int bytesInBuffer = bufferPos - nvramBuffer;
		if ( !Write( NVRAM_PB_INFO_OFFSET + NVRAM_PB_INFO_CHECKBYTE_SIZE, nvramBuffer, bytesInBuffer ) )
			LogString(ERROR_LOG, "Could not write pb info data to NVRAM" );
	}
}

void CPersonalBankerInfo::SetPBFlags(DWORD pbFlags)
{
	m_pbFlags.uValue = pbFlags;
	LogString(PBT_LOG, "SetPBFlags PropertiesOffline=%d IsBannedFromRedemptions=%d CashInactive=%d PromoInactive=%d UnableToMoveLeftoverCash=%d spare=%d",
		m_pbFlags.flags.PropertiesOffline,
		m_pbFlags.flags.IsBannedFromRedemptions,
		m_pbFlags.flags.CashInactive,
		m_pbFlags.flags.PromoInactive,
		m_pbFlags.flags.UnableToMoveLeftoverCash,
		m_pbFlags.flags.m_spare);
}

DWORD CPersonalBankerInfo::GetPBFlags() const
{
	return m_pbFlags.uValue;
}

bool CPersonalBankerInfo::ArePropertiesOffline() const
{
	return m_pbFlags.flags.PropertiesOffline;
}

bool CPersonalBankerInfo::IsBannedFromRedemptions() const
{
	return m_pbFlags.flags.IsBannedFromRedemptions;
}

bool CPersonalBankerInfo::IsCashInactive() const
{
	return m_pbFlags.flags.CashInactive;
}

bool CPersonalBankerInfo::IsUnableToMoveLeftoverCash() const
{
	return m_pbFlags.flags.UnableToMoveLeftoverCash;
}

void CPersonalBankerInfo::SetExternalCreditProviderEnabled(bool enabled)
{
	m_externalCreditProviderEnabled = enabled;
} 

bool CPersonalBankerInfo::ExternalCreditProviderEnabled() const
{
	return m_externalCreditProviderEnabled;
}

void CPersonalBankerInfo::SetCreditFee(DWORD value)
{
	m_creditFee = value;
} 

DWORD CPersonalBankerInfo::CreditFee() const
{
	return m_creditFee;
}

void CPersonalBankerInfo::SetCreditFeeFixed(bool fixed)
{
	m_creditFeeFixed = fixed;
} 

bool CPersonalBankerInfo::CreditFeeFixed() const
{
	return m_creditFeeFixed;
}

DWORD CPersonalBankerInfo::GetMaximumPromoUp() const
{
	return m_maximumPromoUp;
}

bool CPersonalBankerInfo::DisplayTransferCashToGame() const
{

	// 2016-05-27 GetMaximumCashDown() returns the lesser of m_cashBalance in dollars and m_maximumCashDown.
	// ... So this method does not check the cash balance directly.
	bool displayCash(IsTransferToGameEnabled() &&
		!IsBannedFromRedemptions() &&
		IsTransferCashToGameEnabled() &&
		(GetMaximumCashDown() >= GetMinimumCashDown()) &&
		(GetMaximumCashDown() >= MININUM_TRANSFER_IN_DOLLARS));
		

	return displayCash;
}

bool CPersonalBankerInfo::DisplayTransferCashFromGame() const
{
	bool displayCash(true);
	//This checks to see which buttons to display
	if (!IsTransferCashFromGameEnabled() ||
		(GetCashBalanceOnGame() < GetMinimumCashUp() * ONE_DOLLAR_IN_CENTS) ||
		(GetCashBalanceOnGame() == 0) ||
		(!IsPartialTransferCashFromGameEnabled() && (GetCashBalanceOnGame() > GetMaximumCashUp() * ONE_DOLLAR_IN_CENTS)) ||
		GetMaximumCashUp() == 0)
	{
		displayCash = false;
	}

	return displayCash;
}

bool CPersonalBankerInfo::DisplayTransferPointsToGame() const
{
	// 2016-05-27 GetMaximumPointsDollarValueDownload() returns the lesser dollar value of m_pointsAvailable and m_maximumPoints.
	// ... So this method does not check the points available directly.
	bool displayPoints(IsTransferToGameEnabled() &&
		!IsBannedFromRedemptions() &&
		IsTransferPointsToGameEnabled() &&
		GetMaximumPointsDollarValueDownload() >= GetMinimumPointsDollarValueDownload() &&
		GetMaximumPointsDollarValueDownload() >= MININUM_TRANSFER_IN_DOLLARS);

	return displayPoints;
}

bool CPersonalBankerInfo::DisplayTransferPromoToGame() const
{
	// 2016-05-27 GetMaximumPromoDown() returns the lesser of m_promo2Balance in dollars and m_maximumPromoDown.
	// ... So this method does not check the promo balance directly.
	bool displayPromo(IsTransferToGameEnabled() &&
		!IsBannedFromRedemptions() &&
		IsTransferPromoDollarsToGameEnabled() &&
		GetMaximumPromoDown() >= GetMinimumPromoDown() &&
		GetMaximumPromoDown() >= MININUM_TRANSFER_IN_DOLLARS);

	return displayPromo;
}

bool CPersonalBankerInfo::DisplayTransferPromoFromGame() const
{
	bool displayPromo(true);

	if (!IsTransferPromoFromGameEnabled() ||
		(GetPromoBalanceOnGame() < GetMinimumPromoUp() * ONE_DOLLAR_IN_CENTS) ||
		(GetPromoBalanceOnGame() == 0) ||
		(GetPromoBalanceOnGame() > GetMaximumPromoUp() * ONE_DOLLAR_IN_CENTS))
	{
		displayPromo = false;
	}

	return displayPromo;
}

bool CPersonalBankerInfo::GetShowPointsVsAccountBalance() const
{
	return m_showPointsVsAccountBalance;
}

void CPersonalBankerInfo::SetShowFromGameAndToGameButtonFlags()
{
	//Check to see if we should hide transfer to game button
	m_showToGameButton = screenPlayerPBTNone != GetTransferToGameScreen();

	//Check to see if we should show transfer from game button
	m_showFromGameButton = false;
	if (!IsBannedFromRedemptions() && m_bEGMInitialized && 
		((IsTransferCashFromGameEnabled() &&
			GetCashBalanceOnGame() > GetMinimumCashUp() * ONE_DOLLAR_IN_CENTS &&
			GetMaximumCashUp() > 0 &&
			(IsPartialTransferCashFromGameEnabled() || GetCashBalanceOnGame() <= GetMaximumCashUp() * ONE_DOLLAR_IN_CENTS)) ||
			(IsTransferPromoFromGameEnabled() &&
		  GetPromoBalanceOnGame() > GetMinimumPromoUp() * ONE_DOLLAR_IN_CENTS && 
		  GetPromoBalanceOnGame() <= GetMaximumPromoUp() * ONE_DOLLAR_IN_CENTS)))
	{
		m_showFromGameButton = true;
	}
}

bool CPersonalBankerInfo::GetShowToGameButton() const
{
	return m_showToGameButton;
}

bool CPersonalBankerInfo::GetShowFromGameButton() const
{
	return m_showFromGameButton;
}

void CPersonalBankerInfo::SetShowToGameButton(bool value)
{
	m_showToGameButton = value;
}

screenID CPersonalBankerInfo::GetPersonalBankerMainScreen() const
{
	screenID screenId(screenPlayerPBMain);

	//All Buttons Available
	if (m_showToGameButton && m_showFromGameButton)
	{
		screenId = screenPlayerPBMain;
	}
	//just balance and to game
	else if (m_showToGameButton && !m_showFromGameButton)
	{
		screenId = screenPlayerPBHomeWithToGame;
	}
	//just balance and from game
	else if (!m_showToGameButton && m_showFromGameButton)
	{
		screenId = screenPlayerPBHomeWithFromGame;
	}
	//just account balance
	else if (!m_showToGameButton && !m_showFromGameButton)
	{
		screenId = screenPlayerPBHomeWithBalance;
	}

	return screenId;
}

screenID CPersonalBankerInfo::GetCreditTransferScreen() const
{
	screenID screenId(screenPlayerPBCreditTransfersWithFromToGame);

	//All Buttons Available
	if (m_showToGameButton && m_showFromGameButton)
	{
		screenId = screenPlayerPBCreditTransfersWithFromToGame;
	}
	//just credit and to game
	else if (m_showToGameButton && !m_showFromGameButton)
	{
		screenId = screenPlayerPBCreditTransfersWithToGame;
	}
	//just credit and from game
	else if (!m_showToGameButton && m_showFromGameButton)
	{
		screenId = screenPlayerPBCreditTransfersWithFromGame;
	}
	//just credit
	else if (!m_showToGameButton && !m_showFromGameButton)
	{
		screenId = screenPlayerPBCreditTransfersWithOutGame;
	}

	return screenId;
}

screenID CPersonalBankerInfo::GetTransferToGameScreen()
{
	screenID screenId(screenPlayerPBTNone);

	//This checks to see which buttons to display
	bool cash(DisplayTransferCashToGame());
	bool points(DisplayTransferPointsToGame());
	bool promo(DisplayTransferPromoToGame());

	//All buttons available
	if (cash && points && promo)
	{
		screenId = screenPlayerPBToGame;
	}
	//just cash and points
	else if (cash && points && !promo)
	{
		screenId = screenPlayerPBToGameCashPoints;
	}
	//just cash and promo
	else if (cash && !points && promo)
	{
		screenId = screenPlayerPBToGameCashPromo;
	}
	//just points and promo
	else if (!cash && points && promo)
	{
		screenId = screenPlayerPBToGamePointsPromo;
	}
	//just cash
	else if (cash && !points && !promo)
	{
		screenId = screenPlayerPBToGameCash;
	}
	//just points
	else if (!cash && points && !promo)
	{
		screenId = screenPlayerPBToGamePoints;
	}
	//just promo
	else if (!cash && !points && promo)
	{
		screenId = screenPlayerPBToGamePromo;
	}

	return screenId;
}

screenID CPersonalBankerInfo::GetTransferFromGameScreen()
{
	screenID screenId(screenPlayerPBTNone);

	//This checks to see which buttons to display
	bool cash(DisplayTransferCashFromGame());
	bool promo(DisplayTransferPromoFromGame());

	//All buttons available
	if (cash && promo)
	{
		if (GetCashBalanceOnGame() <= GetMaximumCashUp() * ONE_DOLLAR_IN_CENTS)
		{
			screenId = screenPlayerPBFromGame;
		}
		else
		{
			// We show 'Cash' button even when cash is over transfer limit, when partial WAT-Out
			// is enabled. But to decide whether to show 'Transfer from Game > All' button, we
			// should consider cash over transfer limit condition also.
			screenId = screenPlayerPBFromGameCashPromo;
		}
	}
	else if (cash)
	{
		screenId = screenPlayerPBFromGameCash;
	}
	else if (promo)
	{
		screenId = screenPlayerPBFromGamePromo;
	}
	else
	{
		screenId = screenPlayerPBTNone;
	}

	return screenId;
}

DWORD CPersonalBankerInfo::GetMaximumPointsDollarValueDownload() const
{
	return GetMaximumPoints() / GetPointsPerDollar();
}

DWORD CPersonalBankerInfo::GetMinimumPointsDollarValueDownload() const
{
	DWORD minimumDollars(GetMinimumPoints() / GetPointsPerDollar());
	if (minimumDollars * GetPointsPerDollar() < GetMinimumPoints())
	{
		++minimumDollars;
	}

	return minimumDollars;
}

void CPersonalBankerInfo::SetExternalCreditTransferType(ExtCreditTransferType val)
{
	LogString(PBT_LOG, "%s :  External Credit Transfer Type : %d ", __FUNCTION__, val);
	m_extCreditTransferType = val;
}

ExtCreditTransferType CPersonalBankerInfo::ExternalCreditTransferType() const
{
	LogString(PBT_LOG, "%s :  External Credit Transfer Type : %d ", __FUNCTION__, m_extCreditTransferType);
	return m_extCreditTransferType;
}