#include "stdafx.h"

#include "Config.h"
#include "ConfigExtFile.h"
#include "ConfigExtData.h"
#include "../Hardware/NonVolatileMemoryMaps.h"
#include "Hardware/WatchDog.h"
#include "Logging/LogString.h"
#include "metersdef.h"
#include "../Utilities.h"
#include "../CheckSum.h"
#include "Diagnostics.h"
#include "RapidJsonHelper.h"
#include "UI/ScreenDefs.h"
#include "Platform.h"
#include "DisplayManager.h"
#include "../PollerProtocol/PollerMGMDData.h"
#include "PersonalBankerInfo.h"
#include "ProcessDump/WebDiagnosticsStatusInfo.h"

using namespace std;

const string CConfig::TXT_SEN_OPTIONS[NUM_SEN_OPTIONS] =
{
	"Display Name",					//SO1_DISPLAY_PLAYER_NAME
	"Display Comp Total",			//SO1_DISPLAY_COMP
	"Display Point Total",			//SO1_DISPLAY_NUMERIC_POINT_TOTOAL
	"Display Point Total As Cash",	//SO1_DISPLAY_POINTS_TOTAL_AS_CASH
	"Display Point Total Only",		//SO1_DISPLAY_PLAYER_POINTS_TOTAL
	"Countdown xxx / Session xxx",	//SO1_DISPLAY_COUNTDOWN_AND_SESSION
	"Good Luck! / Session $x.xx",	//SO1_DISPLAY_GOOD_LUCK_AND_SESSION
	"Good Luck! / Session xxx",		//SO2_DISPLAY_GOOD_LUCK_AND_SESSION_POINTS
	"Total xxx / Session xxx",		//SO2_DISPLAY_TOTAL_AND_SESSION_POINTS_NUM
	"Total $x.xx / Session $x.xx",	//SO1_DISPLAY_TOTAL_POINTS_SESSION_POINTS_AS_CASH
	"Point Total As Cash",			//SO2_DISPLAY_POINTS_TOTAL_AS_CASH_SESSION
	"Disp Partial Pts(Theo Win)",	//SO1_PARTIAL_BONUS_AWARD
	"Display Congrats Message",		//SO1_DISPLAY_CONGRATS
	"Virtual Sessions",				//SO1_VIRTUAL_SESSION_ENABLED
	"Voucher Out",					//SO2_VOUCHER_OUT_ENABLED
	"Voucher In",					//SO1_TICKET_IN_ENABLED
	"Voucher Promo In",				//SO1_VOUCHER_PROMO_IN_ENABLED
	"Coupon Promo In",				//SO1_COUPON_PROMO_IN_ENABLED
	"WAT In(PBT Cash Down)",		//SO1_PBT_CASH_ENABLE
	"CEP In(PBT Pts to Cash Dwn)",	//SO1_PBT_POINSTS_ENABLE
	"NCEP In(Pts To Promo Dwn)",	//SO1_PBT_POINTS_AS_PROMO
	"NCEP In(PBT Promo $ Dwn)",		//SO1_PBT_PROMO_ENABLE
	"WAT Out(PBT Cash Up)",			//SO1_PB_FROM_GAME_ENABLED
	"CEP Out(CEP Credits Up)",		//SO2_CEP_OUT_ENABLED
	"NCEP Out Manual",				//SO2_NCEP_OUT_ENABLED
	"Fill Amount",					//SO1_FILL_AMOUNT
	"Hopper Level",					//SO1_USE_HOPPER
	"NCEP Auto Upload",				//SO2_NCEP_AUTO_UPLOAD
	"CEP Auto Upload",				//SO2_CEP_AUTOPLOAD
	"WAT Auto Upload",				//SO2_WAT_AUTOPLOAD
	"Accrue Points For NCEP",		//SO2_ACCRUE_POINTS_FOR_NCEP
	"Accrue Comp For NCEP",			//SO2_ACCRUE_POINTS_FOR_NCEP
	"Unattended Taxable Jackpot",	//SO2_UJPENABLED
	"Mini Virtual Sessions",		//SO2_MINI_VIRTUAL_SESSION_ENABLED.
	"Enable ONE LINK",				//SO2_ONE_LINK_PROGRESSIVES_ENABLED.
	"Sub-Game Point Calcs",			//SO2_MGMD_ENABLED.
	"PBT Lock Check",				//SO2_PROBE_PBT_LOCK.
	"Enable MobileConnect Communication", // SO2_MOBILE_CONNECT_COMMUNICATION_ENABLED
	"Remote Debugging",			    //SO2_REMOTE_DEBUGGING_ENABLED
	"Partial PBT Cash Up",			//SO2_PB_PARTIAL_WAT_OUT_ENABLED
	"Cashout To PBT",				//SO2_CASHOUT_TO_HOST_ENABLED
	"External Credit Verification"  //SO2_EXTERNAL_CREDIT_TRANSFER_VERIFICATION
};

const string CConfig::TXT_GLOBAL_OPTIONS[NUM_GLOBAL_OPTIONS] =
{
	"Ticketing enabled",
	"Sys generated validation",
	"Ticket buffering enabled",
	"PBT promo enabled",
	"PBT points enabled",
	"PBT cash enabled",
	"Cash transfer tracking",
	"Coin In Accounting",		//SO1_COINS_IN_ACCOUNTING
	"Coin Out Accounting",		//SO1_COINS_OUT_ACCOUNTING
	"Theoretical Accounting",	//SO1_THEORETICAL_ACCOUNTING
	"Employee PIN Required",	//SO1_EMPLOYEE_PIN_REQUIRED
	"SeaPass Cards Enabled",	//SO2_RCCL_CARDS_ENABLED
	"Accrue Points For NCEP",
	"Accrue Comp For NCEP",
	"Sub-Game Point Calcs",
	"Excessive Voucher Out",
	"Partial PBT Cash Up",
	"Auto Upload Credits",
	"Cashout To PBT",
	"Tax Withholding Enabled"
};

DWORD CConfig::m_sasReadTimeOutMultiplier = DEFAULT_SAS_READ_TIMEOUT_MULTIPLIER;

CConfig::CConfig(bool isLcd, const string& firmwareVersion) :
    CDataSubscriptionProvider("SentinelConfiguration")
	, m_lastCoinsPlayed(0) 
    , m_showPointsVsAccountBalance(false)
	, m_cashTransferTracking(false) 
	, m_socialDistanceIgnoreDirty(false) 
	, m_socialDistanceManualCleanEnabled(false) 
	, m_socialDistanceCountdownDelay(0) 
	, m_fillAmount(false)  
	, m_employeePinRequired(false) 
	, m_rcclCardsEnabled(false) 
	, m_ujpEnabled(false)
	, m_jackpotA8Reset(false)
	, m_slotMastID(0) 
	, m_screenBrightness(0)
	, m_leftSpeakerVolume(0)  
	, m_rightSpeakerVolume(0)
	, m_DHCPAuto(false)
	, m_hostAddressOrName{ 0 }
	, m_hostPort(0)
	, m_myIP{ 0 } 
	, m_defaultGateway{ 0 }
	, m_subnetMask{ 0 }
	, m_preferredDNSServer{ 0 }
	, m_alternateDNSServer{ 0 }
	, m_gameDenom(1)
	, m_batteryStatus(BatteryStatus::batteryStatusUnknown)
	, m_isLcd(isLcd)
	, m_imageChecksum(0)
	, m_firmwareVersion(firmwareVersion)
	, m_globalOptions{ false }
{
	m_ticketingEnabled = false;
	m_coinsOutAccounting = false;
	m_coinsInAccounting = false;
	m_ticketBufferingEnabled = false;
	m_systemGeneratedTicketValidation = false;
	m_accruePointsForNCEP = false;
	m_accrueCompForNCEP = false;

	for ( int cri = 0; cri < SIZE_OF_MAC_ADDRESS; cri++ )
	{
		m_macAddress[cri] = 0;
		m_macAddress2[cri] = 0;
	}

	SetOffsets();

	if ( !ConfigIntegrityCheck() )
	{
		// Reseting ConfigMemory at this stage will result in DEBUG files created with UTC times if debug stick is in at startup.
		ResetConfigMemory();
		SetMemoryLost( true );
	}

	// Check eeprom startup
	if (m_eepromConfig.GetMemoryResetStatus(true))
	{
		SetMemoryLost( true );
	}

	// Default atomic redemptions to false
	m_atomicRedemptions = false;
	
	m_globalMGMD = false;
	m_mgmdEnabled = false;
	m_globalExcessiveVoucherOutEnabledFlag = false;
	m_partialWATOutEnabled = false;
	m_pbtPromoEnabled_local = false;
	m_pbtPromoEnabled_global = false;
	m_pbtPointsAsPromo = false;
	m_pbtPointsEnabled_global = false;
	m_pbtCashEnabled_local = false;
	m_pbtCashEnabled_global = false;
	m_partialWATOutEnabled_global = false;
	m_autoUploadCreditsGlobal = false;
	m_cashoutToHostEnabled_global = false;
	m_cashoutToHostEnabled = false;
	m_taxWithholdingEnabled = false;
	m_federalTaxWithholdingPercent = 0;
	m_stateTaxWithholdingPercent = 0;
	m_numberOfSubgamesMismatchCheck = DEFAULT_NUMBER_OF_SUBGAMES_MISMATCH_CHECK;
	m_countdownDisplayIntervalInSec = DEFAULT_COUNTDOWN_DISPLAY_INTERVAL;
	m_pointsCountdownControlByUniverse = false;
	m_senOption2Raw = 0;
	m_autoCardOutSecToCountdownCredits = 0;
	m_autoCardOutSecToCountdownNoCrdts = 0;
	m_autoCardOutCountdownSeconds = 0;
	m_autoCardOutSecToAlert = 0;
	m_rebootAsapThresholdInMb = DEFAULT_REBOOT_ASAP_THRESHOLD_MB;
	m_rebootWarningThresholdMb = DEFAULT_REBOOT_WARNING_THRESHOLD_MB;
	m_restartUIAsapThresholdMb = DEFAULT_REBOOT_UI_ASAP_THRESHOLD_MB;
	m_restartUIAfterDropThresholdMb = DEFAULT_REBOOT_UI_AFTER_DROP_THRESHOLD_MB;
	m_restartUIDelayHours = DEFAULT_RESTART_UI_DELAY_HOURS;
	m_receivedBadDenom = false;
}

CConfig::~CConfig(void)
{
}

void CConfig::CheckForNVRAMUpgrade()
{
	//Penn Universal Card stuff
	if (CMemoryStorage::PreviousNVRAMVersion < NVRAM_UNIVERSAL_VERSION)
	{
		SetUniverseEnabled(false);
		SetMixedModeEnabled(false);

		int numOfUniverseCardPrefixes = 0;
		m_nvramConfig.Set(m_numOfUniversePropCodesNVRAMOffset, (BYTE *)&numOfUniverseCardPrefixes, sizeof(numOfUniverseCardPrefixes));
	}	

	if (CMemoryStorage::PreviousNVRAMVersion < NVRAM_UNIVERSAL_POINTS_VERSION)
	{
		SetSessionPointsMessage(_T(""));
		SetBalanceRetrievalMessage(_T(""));
		SetPointsOrCashLabel(_T(""));
		SetShowPointsVsAccountBalance(false);
	}

	if (CMemoryStorage::PreviousNVRAMVersion < NVRAM_NCL_CARD_TRACK_VERSION)
	{
		SetNclCardsEnabled(NCL_CARDS_ENABLED_DEFAULT);
		SetCardReaderTrackNumber(CARD_READER_TRACK_NUMBER_DEFAULT);
	}

	if (CMemoryStorage::PreviousNVRAMVersion < NVRAM_CONFIG_EXT_VERSION)
	{
		SetConfigExt1Defaults();
	}

	if (CMemoryStorage::PreviousNVRAMVersion < NVRAM_CONFIG_VERSION_2)
	{
		SetDailyPointsDisplaySuffixMessage(_T(""));
		SetTierPointsLabel(_T(""));
	}	

	if (CMemoryStorage::PreviousNVRAMVersion < NVRAM_CONFIG_EXT2_VERSION)
	{
		SetConfigExt2Defaults();
	}

	BYTE configExt3CheckByte;
	m_nvramConfig.Get(m_configExt3CheckByteNVRAMOffset, &configExt3CheckByte, sizeof(configExt3CheckByte));
	bool configExt3Valid(configExt3CheckByte == m_configExt3CheckByteValue);
	if (CMemoryStorage::PreviousNVRAMVersion < NVRAM_CONFIG_EXT3_VERSION || !configExt3Valid)
	{
		SetConfigExt3Defaults();
	}

	BYTE configExt4CheckByte;
	m_nvramConfig.Get(m_configExt4CheckByteNVRAMOffset, &configExt4CheckByte, sizeof(configExt4CheckByte));
	bool configExt4Valid(configExt4CheckByte == m_configExt4CheckByteValue);
	if (CMemoryStorage::PreviousNVRAMVersion < NVRAM_CONFIG_EXT4_VERSION || !configExt4Valid)
	{
		SetConfigExt4Defaults();
	}

	BYTE configExt5CheckByte;
	m_nvramConfig.Get(m_configExt5CheckByteNVRAMOffset, &configExt5CheckByte, sizeof(configExt5CheckByte));
	bool configExt5Valid(configExt5CheckByte == m_configExt5CheckByteValue);
	if (CMemoryStorage::PreviousNVRAMVersion < NVRAM_DISPLAY_PLAYER_PROFILE_BUTTON_VERSION || !configExt5Valid)
	{
		SetConfigExt5Defaults();
	}

	//For external Credit Tranasfer Type
	BYTE externalCreditTransferCheckByte;
	m_nvramConfig.Get(m_externalCreditTransferCheckByteOffset, &externalCreditTransferCheckByte, sizeof(externalCreditTransferCheckByte));
	bool externalCreditTransferCheckByteOffsetValid(externalCreditTransferCheckByte == m_externalCreditTransferCheckByteValue);
	if (CMemoryStorage::PreviousNVRAMVersion < NVRAM_EXTERNAL_CREDIT_TRANSFER_TO_GAME || !externalCreditTransferCheckByteOffsetValid)
	{
		SetExternalCreditTransferDefault();
	}

	BYTE configExt6CheckByte;
	m_nvramConfig.Get(m_configExt6CheckByteNVRAMOffset, &configExt6CheckByte, sizeof(configExt6CheckByte));
	bool configExt6Valid(configExt6CheckByte == m_configExt6CheckByteValue);
	if (CMemoryStorage::PreviousNVRAMVersion < NVRAM_MOBILE_NCOMPASS_VERSION || !configExt6Valid)
	{
		SetConfigExt6Defaults();
	}
	
	BYTE configExt7CheckByte;
	m_nvramConfig.Get(m_configExt7CheckByteNVRAMOffset, &configExt7CheckByte, sizeof(configExt7CheckByte));
	bool configExt7Valid(configExt7CheckByte == m_configExt7CheckByteValue);
	if (CMemoryStorage::PreviousNVRAMVersion < NVRAM_EXCESSIVE_VOUCHER_OUT_VERSION || !configExt7Valid)
	{
		SetConfigExt7Defaults();
	}

	if (CMemoryStorage::PreviousNVRAMVersion < NVRAM_PARTIAL_WAT_OUT_VERSION)
	{
		//Want this log specifically to Poller section for easier debugging.
		LogString(POLLER_LOG, _T("%s: New NVRAM version detected. So writing default partial "
			"transfer configuration flags to NVRAM location"), __FUNCTION__);

		SetPartialWATOutEnabled(false);
		SetPartialWATOutEnabled_Global(false);
		SetAutoUploadCreditsGlobal(false);
		SetCashoutToHostEnabled(false);
		SetCashoutToHostEnabled_Global(false);
	}

	if (CMemoryStorage::PreviousNVRAMVersion < NVRAM_UJP_TAX_WITHHOLDING_VERSION)
	{
		//Want this log specifically to Poller section for easier debugging.
		LogString(POLLER_LOG, _T("%s: New NVRAM version detected. So writing default UJP "
			"tax withholding configuration variables to NVRAM location"), __FUNCTION__);
		SetUJPTaxWithholding_Global(false);
		SetFederalTaxWithholdingPercent(0);
		SetStateTaxWithholdingPercent(0);
	}

	if (CMemoryStorage::PreviousNVRAMVersion < NVRAM_NUMBER_OF_SUBGAMES_MISMATCH_CHECK)
	{
		LogString(NVRAM_LOG, _T("Set default value for NVRAM_NUMBER_OF_SUBGAMES_MISMATCH_CHECK"));
		SetNumberOfSubgamesMismatchCheck(DEFAULT_NUMBER_OF_SUBGAMES_MISMATCH_CHECK);
	}
	
	if (CMemoryStorage::PreviousNVRAMVersion < NVRAM_NUMBER_OF_SYSTEM_TIME)
	{
		LogString(NVRAM_LOG, _T("Set default value for NVRAM_NUMBER_OF_SYSTEM_TIME"));
		SetSystemTime(CUtilities::GetUTCSystemTime(LOCAL_CERT_TIME_ACTIVATION));
	}

	if (CMemoryStorage::PreviousNVRAMVersion < NVRAM_RESET_EXTENSIBLE_SETTINGS)
	{
		LogString(NVRAM_LOG, _T("Set default value for NVRAM_RESET_EXTENSIBLE_SETTINGS"));
		ResetExtensibleSettings();
	}	
}

bool CConfig::ConfigIntegrityCheck( void )
{
	bool retVal = false;

	byte checkValue = 0;
	if ( m_nvramConfig.Get(NVRAM_CONFIG_CHECKBYTE_OFFSET, &checkValue, sizeof(byte) ) )
	{
		if ( checkValue == NVRAM_CONFIG_CHECKBYTE_VALUE )
		{
			retVal = true;
		}
	}

	else
	{
		//If we can't even read the check byte, we need to reset.
		CDiagnostics::Instance().PersistReboot(tltRebootMemoryError, ERROR_LOG, DIAGNOSTICS_PERSIST_EEPROM_ONLY);
		CWatchDog::Instance().BeginShutdown(CWebDiagnosticsStatusInfo::TiltCodeToString(tltRebootMemoryError));
	}

	return (m_nvramConfig.IntegrityCheck() && retVal);
}

void CConfig::ResetConfigMemory()
{
	BYTE	zeroByte = 0;
	DWORD	dwZero = 0;
	int		nZero = 0;
	int	lZero = 0;	
	WORD	wZero = 0;
	__int64 i64Zero = 0;

	WORD    wOne = 1;
	int		nOne = 1;

	BYTE blankCard[CARD_ID_LEN];

	//initialize the blank card
	for(int i = 0; i < CARD_ID_LEN; i++)
		blankCard[i] = zeroByte;

	bool bFalseBool = false;
	bool bTrueBool = true;

	TicketExpirationType defaultTicket = EXPIRES_YEARS;


	m_nvramConfig.Set(NVRAM_CONFIG_CHECKBYTE_OFFSET,	(BYTE *)&NVRAM_CONFIG_CHECKBYTE_VALUE, NVRAM_CONFIG_CHECKBYTE_SIZE,
		m_pendingSlotmastNVRAMOffset,(BYTE *)&NVRAM_CONFIG_PENDING_SLOTMAST_DEFAULT, sizeof(m_pendingSlotmast));

	m_nvramConfig.Set(	m_playerTrackingMultiplierNVRAMOffset,	(BYTE *)&wOne,	sizeof( wOne ),
		m_playerTrackingDivisorNVRAMOffset,		(BYTE *)&NVRAM_PLAYER_TRACKING_DIVISOR_DEFAULT,	sizeof( NVRAM_PLAYER_TRACKING_DIVISOR_DEFAULT ));

	m_nvramConfig.Set(	m_virtualSessionMaxDurationNVRAMOffset,		(BYTE *)&NVRAM_CONFIG_CARD_TIMEOUT_DEFAULT,	sizeof( NVRAM_CONFIG_CARD_TIMEOUT_DEFAULT ),
		m_virtualSessionNoPlayTimeoutNVRAMOffset,	(BYTE *)&NVRAM_CONFIG_CARD_TIMEOUT_DEFAULT,	sizeof( NVRAM_CONFIG_CARD_TIMEOUT_DEFAULT ),
		m_cardTimeoutNVRAMOffset,					(BYTE *)&NVRAM_CONFIG_CARD_TIMEOUT_DEFAULT,	sizeof( NVRAM_CONFIG_CARD_TIMEOUT_DEFAULT ));

	m_nvramConfig.Set(	m_coinsWonAccountingNVRAMOffset,	(BYTE *)&bFalseBool,	sizeof( bFalseBool ),
		m_theoreticalAccountingNVRAMOffset,	(BYTE *)&bFalseBool,	sizeof( bFalseBool ),
		m_coinsInAccountingNVRAMOffset,		(BYTE *)&bTrueBool,		sizeof( bTrueBool ),
		m_coinsOutAccountingNVRAMOffset,	(BYTE *)&bFalseBool,	sizeof( bFalseBool ));

	m_nvramConfig.Set(	m_attractorMessageNVRAMOffset,		&zeroByte,									sizeof( zeroByte ),
		m_goodCardReadMessageNVRAMOffset,	(BYTE *)NVRAM_CONFIG_GOOD_CARD_DEFAULT,		(_tcslen( NVRAM_CONFIG_GOOD_CARD_DEFAULT )+1) * sizeof(TCHAR),
		m_badCarReadMessageNVRAMOffset,		(BYTE *)NVRAM_CONFIG_BAD_CARD_DEFAULT,		(_tcslen( NVRAM_CONFIG_GOOD_CARD_DEFAULT )+1) * sizeof(TCHAR));

	m_nvramConfig.Set(	m_birthdayMessageNVRAMOffset,		(BYTE *)NVRAM_CONFIG_BIRTHDAY_DEFAULT,		(_tcslen( NVRAM_CONFIG_BIRTHDAY_DEFAULT )+1) * sizeof(TCHAR),
		m_anniversaryMessageNVRAMOffset,	(BYTE *)NVRAM_CONFIG_ANNIVERSARY_DEFAULT,	(_tcslen( NVRAM_CONFIG_ANNIVERSARY_DEFAULT )+1) * sizeof(TCHAR) );

	m_nvramConfig.Set(	m_congratulationsMessageNVRAMOffset,(BYTE *)NVRAM_CONFIG_CONGRATS_DEFAULT,		(_tcslen( NVRAM_CONFIG_CONGRATS_DEFAULT )+1) * sizeof(TCHAR),
		m_welcomeMessageNVRAMOffset,		(BYTE *)NVRAM_CONFIG_WELCOME_DEFAULT,		(_tcslen( NVRAM_CONFIG_WELCOME_DEFAULT )+1) * sizeof(TCHAR) );

	m_nvramConfig.Set(  m_timezoneOffsetNVRAMOffset,	&zeroByte,				sizeof( zeroByte ),
		m_useDSTNVRAMOffset,			(BYTE *)&bFalseBool,	sizeof( bFalseBool ) );

	m_nvramConfig.Set(	m_EGMLockoutStateNVRAMOffset,	(BYTE *)&bFalseBool, 		sizeof( bFalseBool ),
		m_memoryLostNVRAMOffset,		(BYTE *)&bTrueBool,			sizeof( bTrueBool ),
		m_lastCoinsPlayedNVRAMOffset,	(BYTE *)&dwZero,			sizeof( dwZero ),
		m_fillAmountNVRAMOffset,		(BYTE *)&bFalseBool,		sizeof( bFalseBool ) );

	m_nvramConfig.Set( 	m_Unused2NVRAMOffset,			(BYTE *)&nZero,		sizeof( nZero ),
		m_Unused3NVRAMOffset,			(BYTE *)&lZero,		sizeof( lZero ),
		m_lastPBTransactionIDNVRAMOffset, (BYTE *)&nZero,	sizeof( nZero ),
		m_PBResponsePendingNVRAMOffset,	(BYTE *)&bFalseBool, sizeof( bFalseBool ) );

	m_nvramConfig.Set( 	m_pbtPromoEnabled_globalNVRAMOffset,	(BYTE *)&bFalseBool, sizeof( bFalseBool ),
		m_pbtPointsEnabled_globalNVRAMOffset,	(BYTE *)&bFalseBool, sizeof( bFalseBool ),
		m_pbtCashEnabled_globalNVRAMOffset,		(BYTE *)&bFalseBool, sizeof( bFalseBool ));

	m_nvramConfig.Set(	m_PBTransactionIDNVRAMOffset,			(BYTE *)&nZero,	sizeof(nZero),
		m_PBTransferSequenceNumberNVRAMOffset,	(BYTE *)&nZero,	sizeof(nZero),
		m_PBCreditSourceNVRAMOffset,			(BYTE *)&nZero, sizeof(nZero),
		m_PBTransferInProgressNVRAMOffset,		(BYTE *)&bFalseBool,sizeof(bFalseBool) );

	m_nvramConfig.Set(	m_currentPBTransactionIDNVRAMOffset,	(BYTE *)&nZero,			sizeof(nZero),
		m_PBTransferAmountNVRAMOffset,			(BYTE *)&dwZero,		sizeof(dwZero),
		m_PBReleaseNeededNVRAMOffset,			(BYTE *)&bFalseBool,	sizeof(bFalseBool) );

	m_nvramConfig.Set(	m_bonusEventBoundariesNVRAMOffset,		(BYTE *)&nZero,			sizeof(nZero),
		m_ticketVendResponseInProgressNVRAMOffset, (BYTE *)&bFalseBool, sizeof(bFalseBool),
		m_employeeAuthorizationOCRNVRAMOffset,	(BYTE *)&nZero,			sizeof(nZero));

	SetCasinoName(NVRAM_CONFIG_CASINO_NAME_DEFAULT);
	SetCasinoAddress1(NVRAM_CONFIG_CASINO_ADDRESS1_DEFAULT);
	SetCasinoAddress2(NVRAM_CONFIG_CASINO_ADDRESS2_DEFAULT);

	m_nvramConfig.Set(	m_numberOfCardPrefixesNVRAMOffset,		(BYTE *)&nOne,			sizeof(nOne),
		m_playerCardPrefixesNVRAMOffset,		(BYTE *)&PLAYER_PREFIX,  sizeof(PLAYER_PREFIX),
		m_globalCardPrefixNVRAMOffset,			(BYTE *)&GLOBAL_PREFIX,  sizeof(GLOBAL_PREFIX),
		m_mechanicCardPrefixNVRAMOffset,		(BYTE *)&MECHANIC_PREFIX, sizeof(MECHANIC_PREFIX) );

	m_nvramConfig.Set(	m_floorCardPrefixNVRAMOffset,			(BYTE *)&FLOOR_PREFIX,	sizeof(FLOOR_PREFIX),
		m_tempCardPrefixNVRAMOffset,			(BYTE *)&TEMP_PREFIX,	sizeof(TEMP_PREFIX) );

	// Tracker #23982 - LossLimit not used anymore.  No longer need to default to true
	m_nvramConfig.Set( 	m_bLossLimitActiveNVRAMOffset,			(BYTE *)&bFalseBool,	sizeof(bFalseBool),
		m_lossLimitSessionNumberNVRAMOffset,	(BYTE *)&dwZero,		sizeof(dwZero),
		m_lossLimitSessionAmountNVRAMOffset,	(BYTE *)&wZero,			sizeof(wZero),
		m_lossLimitBillValidatorOffAmountNVRAMOffset, (BYTE *)&wZero,	sizeof(wZero) );

	m_nvramConfig.Set(	m_lossLimitMessageNVRAMOffset,			(BYTE *)&bFalseBool,	sizeof(bFalseBool),
		m_cashTransferTrackingNVRAMOffset,		&zeroByte,				sizeof(zeroByte),
		m_progressiveDenominationNVRAMOffset,	(BYTE *)&nOne,			sizeof(nOne),
		m_floorLockNVRAMOffset,					(BYTE *)&bFalseBool,	sizeof(bFalseBool) );

	m_nvramConfig.Set(	m_ticketExpTypeNVRAMOffset,				(BYTE *)&defaultTicket,	sizeof(defaultTicket),
		m_ticketExpirationDurationNVRAMOffset,	(BYTE *)&nOne,			sizeof(nOne),
		m_ticketingEnabledNVRAMOffset,			(BYTE *)&bFalseBool,		sizeof(bFalseBool),
		m_systemGeneratedTicketValidationNVRAMOffset, (BYTE *)&bFalseBool, sizeof(bFalseBool) );

	m_nvramConfig.Set(	m_ticketBufferingEnabledNVRAMOffset,	(BYTE *)&bFalseBool,	sizeof(bFalseBool),
		m_employeeAuthorizationPasswordNVRAMOffset, (BYTE *)&dwZero,	sizeof(dwZero),
		m_useHopperLevelNVRAMOffset,			(BYTE *)&bFalseBool,	sizeof(bFalseBool),
		m_virtualSessionEnabledNVRAMOffset,		(BYTE *)&bFalseBool,	sizeof(bFalseBool) );

	m_nvramConfig.Set(	m_displayPlayerNameNVRAMOffset,			(BYTE *)&bTrueBool,		sizeof(bTrueBool),
		m_displayPlayerPtsTotalNVRAMOffset,		(BYTE *)&bTrueBool,		sizeof(bTrueBool),
		m_displayCountdownAndSessionNVRAMOffset,(BYTE *)&bFalseBool,	sizeof(bFalseBool),
		m_displayGoodLuckAndSessionPtsNVRAMOffset, (BYTE *)&bFalseBool,	sizeof(bFalseBool) );

	m_nvramConfig.Set(	m_displayGoodLuckAndSessionNVRAMOffset, (BYTE *)&bFalseBool,	sizeof(bFalseBool),
		m_displayNumericPointTotalNVRAMOffset,	(BYTE *)&bFalseBool,	sizeof(bFalseBool),
		m_displayCompsNVRAMOffset,				(BYTE *)&bFalseBool,	sizeof(bFalseBool),
		m_displayCongratsNVRAMOffset,			(BYTE *)&bFalseBool,	sizeof(bFalseBool) );

	m_nvramConfig.Set( 	m_displayPtsTotalAsCashNVRAMOffset,			(BYTE *)&bFalseBool,	sizeof(bFalseBool),
		m_displayPtsTotalAsCashSessionNVRAMOffset,	(BYTE *)&bFalseBool,	sizeof(bFalseBool),
		m_displayTotalAndSessionPtsNumNVRAMOffset,	(BYTE *)&bFalseBool,	sizeof(bFalseBool),
		m_displayTotalPtsSessionPtsAsCashNVRAMOffset, (BYTE *)&bFalseBool,	sizeof(bFalseBool) );

	m_nvramConfig.Set(  m_pbtCashEnabled_localNVRAMOffset,	(BYTE *)&bFalseBool,	sizeof(bFalseBool),
		m_pbtPointsAsPromoNVRAMOffset,		(BYTE *)&bFalseBool,	sizeof(bFalseBool),
		m_pbtPointsEnabled_localNVRAMOffset,(BYTE *)&bFalseBool,	sizeof(bFalseBool),
		m_pbtPromoEnabled_localNVRAMOffset,	(BYTE *)&bFalseBool,	sizeof(bFalseBool) );

	m_nvramConfig.Set(  m_cardInPBTNVRAMOffset,				(BYTE *)&bFalseBool,	sizeof(bFalseBool),
		m_employeePinRequiredNVRAMOffset,	(BYTE *)&bFalseBool,	sizeof(bFalseBool),
		m_PartialBonusAwardNVRAMOffset,		(BYTE *)&bFalseBool,	sizeof(bFalseBool),
		m_PBFromGameEnabledNVRAMOffset,	(BYTE *)&bTrueBool,		sizeof(bTrueBool) );

	m_nvramConfig.Set(	m_couponPromoInEnabledNVRAMOffset,	(BYTE *)&bFalseBool,	sizeof(bFalseBool),
		m_couponPromoOutEnabledNVRAMOffset,	(BYTE *)&bFalseBool,	sizeof(bFalseBool),
		m_voucherPromoInEnabledNVRAMOffset,	(BYTE *)&bFalseBool,	sizeof(bFalseBool),
		m_CEPOutEnabledNVRAMOffset,			(BYTE *)&bFalseBool,	sizeof(bFalseBool) );

	m_nvramConfig.Set(	m_NCEPOutEnabledNVRAMOffset,	(BYTE *)&bFalseBool,	sizeof(bFalseBool),
		m_PBTPinNotRequiredNVRAMOffset,	(BYTE *)&bFalseBool,	sizeof(bFalseBool),
		m_voucherOutEnabledNVRAMOffset,	(BYTE *)&bFalseBool,	sizeof(bFalseBool),
		m_ticketInEnabledNVRAMOffset,	(BYTE *)&bFalseBool,	sizeof(bFalseBool));

	m_nvramConfig.Set(	m_allowFloorLockoutNVRAMOffset,		(BYTE *)&bFalseBool,	sizeof(bFalseBool),
		m_slotMastRevisionNVRAMOffset,	(BYTE *)&wZero,			sizeof(wZero) );

	m_nvramConfig.Set(  m_lastGameStartNVRAMOffset,		(BYTE *)&i64Zero,		sizeof(i64Zero),
		m_coinInAtLastGameStartNVRAMOffset, (BYTE *)&i64Zero,	sizeof(i64Zero),
		m_lastCardReadNVRAMOffset,		blankCard,				CARD_ID_LEN );

	m_denomination = US_DOLLARS;
	m_pointsPerDollar = nOne;
	m_senOption1Raw = dwZero;
	m_senOption2Raw = dwZero;
	m_nvramConfig.Set(	m_senOption1RawNVRAMOffset,		(BYTE *)&m_senOption1Raw,	sizeof(m_senOption1Raw),
		m_senOption2RawNVRAMOffset,		(BYTE *)&m_senOption2Raw,	sizeof(m_senOption2Raw),
		m_denominationNVRAMOffset,		(BYTE *)&m_denomination,	sizeof(m_denomination),
		m_pointsPerDollarNVRAMOffset,	(BYTE *)&m_pointsPerDollar,	sizeof(m_pointsPerDollar));

	m_nvramConfig.Set( m_rcclCardsEnabledNVRAMOffset,	(BYTE *)&bFalseBool,	sizeof(bFalseBool),
		m_autoUploadCreditsNVRAMOffset, (BYTE *)&bFalseBool, sizeof(bFalseBool));

	m_nvramConfig.Set (m_jackpotA8ResetNVRAMOffset, (BYTE *)&bFalseBool, sizeof(bFalseBool));

	m_nvramConfig.Set(m_universeEnabledNVRAMOffset, (BYTE *)&bFalseBool, sizeof(bFalseBool),
					  m_mixedModeEnabledNVRAMOffset, (BYTE *)&bFalseBool, sizeof(bFalseBool));

	m_nvramConfig.Set(	m_numOfUniversePropCodesNVRAMOffset,		(BYTE *)&nZero,			 sizeof(nZero),
						m_universePropCodesNVRAMOffset,				(BYTE *)&PLAYER_PREFIX,  sizeof(PLAYER_PREFIX),
						m_universePlayerPrefixesNVRAMOffset,		(BYTE *)&PLAYER_PREFIX,  sizeof(PLAYER_PREFIX));

        // here comes nCompass:
	BYTE touchScreenProtocolNameReset[SIZE_OF_TOUCH_SCREEN_PROTOCOL_NAME];
	ZeroMemory(touchScreenProtocolNameReset, sizeof touchScreenProtocolNameReset);
	m_nvramConfig.Set (m_touchScreenProtocolNameNVRAMOffset, (BYTE *)&touchScreenProtocolNameReset, SIZE_OF_TOUCH_SCREEN_PROTOCOL_NAME);

        // in nCompass we need these for Penn Universal support, but I don't know if this is the right spot:
	m_nvramConfig.Set(m_universeEnabledNVRAMOffset, (BYTE *)&bFalseBool, sizeof(bFalseBool),
					  m_mixedModeEnabledNVRAMOffset, (BYTE *)&bFalseBool, sizeof(bFalseBool));

	m_nvramConfig.Set(	m_numOfUniversePropCodesNVRAMOffset,		(BYTE *)&nZero,			 sizeof(nZero),
						m_universePropCodesNVRAMOffset,				(BYTE *)&PLAYER_PREFIX,  sizeof(PLAYER_PREFIX),
						m_universePlayerPrefixesNVRAMOffset,		(BYTE *)&PLAYER_PREFIX,  sizeof(PLAYER_PREFIX));

	m_nvramConfig.Set(m_featureFlagsRawNVRAMOffset,	(BYTE*)&dwZero, sizeof(dwZero));
	m_nvramConfig.Set(m_atomicRedemptionsNVRAMOffset, (BYTE*)&bFalseBool, sizeof(bFalseBool));
	m_nvramConfig.Set(m_jackpotExternalBonusNVRAMOffset, (BYTE*)&bFalseBool, sizeof(bFalseBool));
	m_nvramConfig.Set(m_externalCreditToGameNVRAMOffset, (BYTE*)&bFalseBool, sizeof(bFalseBool));
	m_nvramConfig.Set(m_multiGameMultiDenomNVRAMOffset, (BYTE*)&bFalseBool, sizeof(bFalseBool));
	m_nvramConfig.Set(m_globalMGMDOffset, (BYTE *)&bFalseBool, sizeof(bFalseBool));

	TCHAR messageReset[SIZE_OF_MESSAGES];
	ZeroMemory(messageReset, sizeof(messageReset));
	m_nvramConfig.Set(m_sessionPointsMessageNVRAMOffset, (BYTE *)&messageReset, sizeof(m_sessionPointsMessage));
	m_nvramConfig.Set(m_balanceRetrievalMessageNVRAMOffset, (BYTE *)&messageReset, sizeof(m_balanceRetrievalMessage));
	m_nvramConfig.Set(m_dailyPointsDisplaySuffixMessageNVRAMOffset, (BYTE *)&messageReset, sizeof(m_dailyPointsDisplaySuffixMessage));

	TCHAR labelReset[SIZE_OF_LABEL];
	ZeroMemory(labelReset, sizeof(labelReset));
	m_nvramConfig.Set(m_pointsOrCashLabelNVRAMOffset, (BYTE *)&labelReset, sizeof(m_pointsOrCashLabel));
	m_nvramConfig.Set(m_tierPointsLabelNVRAMOffset, (BYTE *)&labelReset, sizeof(m_tierPointsLabel));

	m_nvramConfig.Set(m_showPointsVsAccountBalanceNVRAMOffset, (BYTE *)&bFalseBool, sizeof(bFalseBool));

	BYTE tempBYTE;
	tempBYTE = (byte)NCL_CARDS_ENABLED_DEFAULT;
	m_nvramConfig.Set(m_nclCardsEnabledNVRAMOffset, &tempBYTE, sizeof(tempBYTE));

	// 2013-02-11  Default value cannot be a constant with m_nvramConfig.Set().
	tempBYTE = CARD_READER_TRACK_NUMBER_DEFAULT;
	m_nvramConfig.Set(m_cardReaderTrackNumberNVRAMOffset, &tempBYTE, sizeof(tempBYTE));

	m_nvramConfig.Set(m_gameStartExceptionsOffset, (BYTE *)&bFalseBool, sizeof(bFalseBool));
	m_nvramConfig.Set(m_gameEndExceptionsOffset, (BYTE *)&bFalseBool, sizeof(bFalseBool));

	SetConfigExt1Defaults();
	SetConfigExt2Defaults();
	SetConfigExt3Defaults();
	SetConfigExt4Defaults();
	SetConfigExt5Defaults();
	SetExternalCreditTransferDefault();
	SetConfigExt6Defaults();
	SetConfigExt7Defaults();

	SetCoinsWonAtLastGameEnd(INVALID_METER_VALUE);
	SetPartialWATOutEnabled(false);
	SetPartialWATOutEnabled_Global(false);
	SetAutoUploadCreditsGlobal(false);
	SetCashoutToHostEnabled(false);
	SetCashoutToHostEnabled_Global(false);

	ResetExtensibleSettings();
	SetUJPTaxWithholding_Global(false);
	SetFederalTaxWithholdingPercent(0);
	SetStateTaxWithholdingPercent(0);
	SetNumberOfSubgamesMismatchCheck(DEFAULT_NUMBER_OF_SUBGAMES_MISMATCH_CHECK);
	SetSystemTime(CUtilities::GetUTCSystemTime(LOCAL_CERT_TIME_ACTIVATION));

	SetRestartValues();
	return;
}

void CConfig::SetRestartValues()
{
	SetGameInProgress(false);
}

byte CConfig::GetDesktopOrientationAsByte()
{
    byte retBytes = EEPROM_DESKTOP_ORIENTATION_NONE;
    switch( m_desktopOrientationType )
    {
    case DesktopOrientation90:
        retBytes = EEPROM_DESKTOP_ORIENTATION_90;
        break;
    case DesktopOrientation180:
        retBytes = EEPROM_DESKTOP_ORIENTATION_180;
        break;
    case DesktopOrientation270:
        retBytes = EEPROM_DESKTOP_ORIENTATION_270;
        break;
    case DesktopOrientationDefault:
    default:
        retBytes = 0;
        break;
    }

    return retBytes;
}

WORD CConfig::GetDesktopOrientationAsWord()
{
    WORD retWord = 0;
    switch( m_desktopOrientationType )
    {
    case DesktopOrientation90:
        retWord = 90;
        break;
    case DesktopOrientation180:
        retWord = 180;
        break;
    case DesktopOrientation270:
        retWord = 270;
        break;
    case DesktopOrientationDefault:
    default:
        retWord = 0;
        break;
    }

    return retWord;
}

void CConfig::SetOffsets()
{
	m_EGMLockoutStateNVRAMOffset		= NVRAM_CONFIG_OFFSET;
	m_memoryLostNVRAMOffset				= m_EGMLockoutStateNVRAMOffset + sizeof(m_EGMLockoutState);
	m_lastCoinsPlayedNVRAMOffset		= m_memoryLostNVRAMOffset + sizeof(m_memoryLost);
	m_fillAmountNVRAMOffset				= m_lastCoinsPlayedNVRAMOffset + sizeof(m_lastCoinsPlayed);
	m_Unused2NVRAMOffset				= m_fillAmountNVRAMOffset + sizeof(m_fillAmount);
	m_Unused3NVRAMOffset				= m_Unused2NVRAMOffset + sizeof(m_Unused2);
	m_lastPBTransactionIDNVRAMOffset	= m_Unused3NVRAMOffset + sizeof(m_Unused3);
	m_PBResponsePendingNVRAMOffset		= m_lastPBTransactionIDNVRAMOffset + sizeof(m_lastPBTransactionID);
	m_pbtPromoEnabled_globalNVRAMOffset = m_PBResponsePendingNVRAMOffset + sizeof(m_PBResponsePending);
	m_pbtPointsEnabled_globalNVRAMOffset = m_pbtPromoEnabled_globalNVRAMOffset + sizeof(m_pbtPromoEnabled_global);
	m_pbtCashEnabled_globalNVRAMOffset	= m_pbtPointsEnabled_globalNVRAMOffset + sizeof(m_pbtPointsEnabled_global);
	m_PBTransactionIDNVRAMOffset		= m_pbtCashEnabled_globalNVRAMOffset + sizeof(m_pbtCashEnabled_global);
	m_PBTransferSequenceNumberNVRAMOffset = m_PBTransactionIDNVRAMOffset + sizeof(m_PBTransactionID);
	m_PBCreditSourceNVRAMOffset			= m_PBTransferSequenceNumberNVRAMOffset + sizeof(m_PBTransferSequenceNumber);
	m_PBTransferInProgressNVRAMOffset	= m_PBCreditSourceNVRAMOffset + sizeof(m_PBCreditSource);
	m_currentPBTransactionIDNVRAMOffset = m_PBTransferInProgressNVRAMOffset + sizeof(m_PBTransferInProgress);
	m_PBTransferAmountNVRAMOffset		= m_currentPBTransactionIDNVRAMOffset + sizeof(m_currentPBTransactionID);
	m_PBReleaseNeededNVRAMOffset		= m_PBTransferAmountNVRAMOffset + sizeof(m_PBTransferAmount);
	m_bonusEventBoundariesNVRAMOffset	= m_PBReleaseNeededNVRAMOffset + sizeof(m_PBReleaseNeeded); 
	m_ticketVendResponseInProgressNVRAMOffset = m_bonusEventBoundariesNVRAMOffset + sizeof(m_bonusEventBoundaries);
	m_employeeAuthorizationOCRNVRAMOffset = m_ticketVendResponseInProgressNVRAMOffset + sizeof(m_ticketVendResponseInProgress);
	m_attractorMessageNVRAMOffset		= m_employeeAuthorizationOCRNVRAMOffset + sizeof(m_employeeAuthorizationOCR);
	m_goodCardReadMessageNVRAMOffset	= m_attractorMessageNVRAMOffset + sizeof(m_attractorMessage);
	m_badCarReadMessageNVRAMOffset		= m_goodCardReadMessageNVRAMOffset + sizeof(m_goodCardReadMessage);
	m_birthdayMessageNVRAMOffset		= m_badCarReadMessageNVRAMOffset + sizeof(m_badCarReadMessage);
	m_anniversaryMessageNVRAMOffset		= m_birthdayMessageNVRAMOffset + sizeof(m_birthdayMessage);
	m_congratulationsMessageNVRAMOffset = m_anniversaryMessageNVRAMOffset + sizeof(m_anniversaryMessage);
	m_welcomeMessageNVRAMOffset			= m_congratulationsMessageNVRAMOffset + sizeof(m_congratulationsMessage);
	m_casinoNameNVRAMOffset				= m_welcomeMessageNVRAMOffset + sizeof(m_welcomeMessage);
	m_numberOfCardPrefixesNVRAMOffset	= m_casinoNameNVRAMOffset + SIZE_OF_CASINO_NAME;
	m_playerCardPrefixesNVRAMOffset		= m_numberOfCardPrefixesNVRAMOffset + sizeof(int);
	m_globalCardPrefixNVRAMOffset		= m_playerCardPrefixesNVRAMOffset + NUMBER_OF_VALID_CARDS_PREFIX_IN_NVRAM;
	m_mechanicCardPrefixNVRAMOffset		= m_globalCardPrefixNVRAMOffset + sizeof(m_globalCardPrefix);
	m_floorCardPrefixNVRAMOffset		= m_mechanicCardPrefixNVRAMOffset + sizeof(m_mechanicCardPrefix);
	m_tempCardPrefixNVRAMOffset			= m_floorCardPrefixNVRAMOffset + sizeof(m_floorCardPrefix);
	m_bLossLimitActiveNVRAMOffset		= m_tempCardPrefixNVRAMOffset + sizeof(m_tempCardPrefix);
	m_lossLimitSessionNumberNVRAMOffset = m_bLossLimitActiveNVRAMOffset + sizeof(m_bLossLimitActive);
	m_lossLimitSessionAmountNVRAMOffset = m_lossLimitSessionNumberNVRAMOffset + sizeof(m_lossLimitSessionNumber);
	m_lossLimitBillValidatorOffAmountNVRAMOffset = m_lossLimitSessionAmountNVRAMOffset + sizeof(m_lossLimitSessionAmount);
	m_lossLimitMessageNVRAMOffset		= m_lossLimitBillValidatorOffAmountNVRAMOffset + sizeof(m_lossLimitBillValidatorOffAmount);
	m_cashTransferTrackingNVRAMOffset	= m_lossLimitMessageNVRAMOffset + sizeof(m_lossLimitMessage);
	m_progressiveDenominationNVRAMOffset = m_cashTransferTrackingNVRAMOffset + sizeof(m_cashTransferTracking);
	m_floorLockNVRAMOffset				= m_progressiveDenominationNVRAMOffset + sizeof(m_progressiveDenomination);
	m_ticketExpTypeNVRAMOffset			= m_floorLockNVRAMOffset + sizeof(m_floorLock);
	m_ticketExpirationDurationNVRAMOffset = m_ticketExpTypeNVRAMOffset + sizeof(m_ticketExpType);
	m_ticketingEnabledNVRAMOffset		= m_ticketExpirationDurationNVRAMOffset + sizeof(m_ticketExpirationDuration);
	m_systemGeneratedTicketValidationNVRAMOffset = m_ticketingEnabledNVRAMOffset + sizeof(m_ticketingEnabled);
	m_ticketBufferingEnabledNVRAMOffset = m_systemGeneratedTicketValidationNVRAMOffset + sizeof(m_systemGeneratedTicketValidation);
	m_playerTrackingMultiplierNVRAMOffset = m_ticketBufferingEnabledNVRAMOffset + sizeof(m_ticketBufferingEnabled);
	m_playerTrackingDivisorNVRAMOffset	= m_playerTrackingMultiplierNVRAMOffset + sizeof(m_playerTrackingMultiplier);
	m_virtualSessionMaxDurationNVRAMOffset = m_playerTrackingDivisorNVRAMOffset + sizeof(m_playerTrackingDivisor);
	m_virtualSessionNoPlayTimeoutNVRAMOffset = m_virtualSessionMaxDurationNVRAMOffset + sizeof(m_virtualSessionMaxDuration);
	m_cardTimeoutNVRAMOffset			= m_virtualSessionNoPlayTimeoutNVRAMOffset + sizeof(m_virtualSessionNoPlayTimeout);
	m_useHopperLevelNVRAMOffset = m_cardTimeoutNVRAMOffset + sizeof(m_cardTimeout);
	m_virtualSessionEnabledNVRAMOffset	= m_useHopperLevelNVRAMOffset + sizeof(m_useHopperLevel);
	m_displayPlayerNameNVRAMOffset		= m_virtualSessionEnabledNVRAMOffset + sizeof(m_virtualSessionEnabled);
	m_displayPlayerPtsTotalNVRAMOffset	= m_displayPlayerNameNVRAMOffset + sizeof(m_displayPlayerName);
	m_displayCountdownAndSessionNVRAMOffset	= m_displayPlayerPtsTotalNVRAMOffset + sizeof(m_displayPlayerPtsTotal);
	m_displayGoodLuckAndSessionPtsNVRAMOffset = m_displayCountdownAndSessionNVRAMOffset + sizeof(m_displayCountdownAndSession);
	m_displayGoodLuckAndSessionNVRAMOffset = m_displayGoodLuckAndSessionPtsNVRAMOffset + sizeof(m_displayGoodLuckAndSessionPts);
	m_displayNumericPointTotalNVRAMOffset = m_displayGoodLuckAndSessionNVRAMOffset + sizeof(m_displayGoodLuckAndSession);
	m_displayCompsNVRAMOffset			= m_displayNumericPointTotalNVRAMOffset + sizeof(m_displayNumericPointTotal);
	m_displayCongratsNVRAMOffset		= m_displayCompsNVRAMOffset + sizeof(m_displayComps);
	m_displayPtsTotalAsCashNVRAMOffset	= m_displayCongratsNVRAMOffset + sizeof(m_displayCongrats);
	m_displayPtsTotalAsCashSessionNVRAMOffset = m_displayPtsTotalAsCashNVRAMOffset + sizeof(m_displayPtsTotalAsCash);
	m_displayTotalAndSessionPtsNumNVRAMOffset = m_displayPtsTotalAsCashSessionNVRAMOffset + sizeof(m_displayPtsTotalAsCashSession);
	m_displayTotalPtsSessionPtsAsCashNVRAMOffset = m_displayTotalAndSessionPtsNumNVRAMOffset + sizeof(m_displayTotalAndSessionPtsNum);
	m_pbtCashEnabled_localNVRAMOffset	= m_displayTotalPtsSessionPtsAsCashNVRAMOffset + sizeof(m_displayTotalPtsSessionPtsAsCash);
	m_pbtPointsAsPromoNVRAMOffset		= m_pbtCashEnabled_localNVRAMOffset + sizeof(m_pbtCashEnabled_local);
	m_pbtPointsEnabled_localNVRAMOffset = m_pbtPointsAsPromoNVRAMOffset + sizeof(m_pbtPointsAsPromo);
	m_pbtPromoEnabled_localNVRAMOffset	= m_pbtPointsEnabled_localNVRAMOffset + sizeof(m_pbtPointsEnabled_local);
	m_coinsWonAccountingNVRAMOffset		= m_pbtPromoEnabled_localNVRAMOffset + sizeof(m_pbtPromoEnabled_local);
	m_coinsInAccountingNVRAMOffset		= m_coinsWonAccountingNVRAMOffset + sizeof(m_coinsWonAccounting);
	m_coinsOutAccountingNVRAMOffset		= m_coinsInAccountingNVRAMOffset + sizeof(m_coinsInAccounting);		
	m_theoreticalAccountingNVRAMOffset	= m_coinsOutAccountingNVRAMOffset + sizeof(m_coinsOutAccounting);	
	m_cardInPBTNVRAMOffset				= m_theoreticalAccountingNVRAMOffset + sizeof(m_theoreticalAccounting);				
	m_employeePinRequiredNVRAMOffset	= m_cardInPBTNVRAMOffset + sizeof(m_cardInPBT);
	m_PartialBonusAwardNVRAMOffset		= m_employeePinRequiredNVRAMOffset + sizeof(m_employeePinRequired);
	m_PBFromGameEnabledNVRAMOffset		= m_PartialBonusAwardNVRAMOffset + sizeof(m_PartialBonusAward);	
	m_couponPromoInEnabledNVRAMOffset	= m_PBFromGameEnabledNVRAMOffset + sizeof(m_PBFromGameEnabled);
	m_couponPromoOutEnabledNVRAMOffset	= m_couponPromoInEnabledNVRAMOffset + sizeof(m_couponPromoInEnabled);
	m_voucherPromoInEnabledNVRAMOffset	= m_couponPromoOutEnabledNVRAMOffset + sizeof(m_couponPromoOutEnabled);
	m_CEPOutEnabledNVRAMOffset			= m_voucherPromoInEnabledNVRAMOffset + sizeof(m_voucherPromoInEnabled);
	m_NCEPOutEnabledNVRAMOffset			= m_CEPOutEnabledNVRAMOffset + sizeof(m_CEPOutEnabled_notUsed);
	m_PBTPinNotRequiredNVRAMOffset		= m_NCEPOutEnabledNVRAMOffset + sizeof(m_NCEPOutEnabled_notUsed);
	m_voucherOutEnabledNVRAMOffset		= m_PBTPinNotRequiredNVRAMOffset + sizeof(m_PBTPinNotRequired);
	m_ticketInEnabledNVRAMOffset		= m_voucherOutEnabledNVRAMOffset + sizeof(m_voucherOutEnabled);
	m_pendingSlotmastNVRAMOffset		= m_ticketInEnabledNVRAMOffset + sizeof(m_ticketInEnabled);	
	m_allowFloorLockoutNVRAMOffset		= m_pendingSlotmastNVRAMOffset + sizeof(m_pendingSlotmast);
	m_timezoneOffsetNVRAMOffset			= m_allowFloorLockoutNVRAMOffset + sizeof( m_allowFloorLockout );
	m_useDSTNVRAMOffset					= m_timezoneOffsetNVRAMOffset + sizeof( m_timezoneOffset );
	m_slotMastRevisionNVRAMOffset		= m_useDSTNVRAMOffset + sizeof(m_useDST);
	m_lastGameStartNVRAMOffset			= m_slotMastRevisionNVRAMOffset + sizeof(m_slotMastRevision);
	m_coinInAtLastGameStartNVRAMOffset	= m_lastGameStartNVRAMOffset + sizeof(m_lastGameStart);
	m_lastCardReadNVRAMOffset			= m_coinInAtLastGameStartNVRAMOffset + sizeof(m_coinInAtLastGameStart);
	m_casinoAddress1NVRAMOffset			= m_lastCardReadNVRAMOffset + sizeof(m_lastCardRead);
	m_casinoAddress2NVRAMOffset			= m_casinoAddress1NVRAMOffset + SIZE_OF_CASINO_ADDRESS1;
	m_senOption1RawNVRAMOffset			= m_casinoAddress2NVRAMOffset + SIZE_OF_CASINO_ADDRESS2;
	m_senOption2RawNVRAMOffset			= m_senOption1RawNVRAMOffset + sizeof( DWORD );
	m_denominationNVRAMOffset			= m_senOption2RawNVRAMOffset + sizeof( DWORD );
	m_pointsPerDollarNVRAMOffset		= m_denominationNVRAMOffset + sizeof(m_denomination);
	m_rcclCardsEnabledNVRAMOffset		= m_pointsPerDollarNVRAMOffset + sizeof(m_pointsPerDollar);
	m_autoUploadCreditsNVRAMOffset		= m_rcclCardsEnabledNVRAMOffset + sizeof(m_rcclCardsEnabled);
	m_nudgePortNVRAMOffset				= m_autoUploadCreditsNVRAMOffset + sizeof(m_autoUploadCredits_notUsed);
	m_nudgeIPNVRAMOffset				= m_nudgePortNVRAMOffset + sizeof(int);
	m_cacheAnnouncementPortNVRAMOffset  = m_nudgeIPNVRAMOffset + IP_ADDRESS_SIZE;
	m_cacheAnnouncementIPNVRAMOffset	= m_cacheAnnouncementPortNVRAMOffset + sizeof(int);
	m_jackpotA8ResetNVRAMOffset			= m_cacheAnnouncementIPNVRAMOffset + IP_ADDRESS_SIZE;
	m_accruePointsForNCEPNVRAMOffset	= m_jackpotA8ResetNVRAMOffset + sizeof(m_jackpotA8Reset);
	m_accrueCompForNCEPNVRAMOffset		= m_accruePointsForNCEPNVRAMOffset + sizeof(m_accruePointsForNCEP);
	m_senOpAccruePointsForNCEPNVRAMOffset= m_accrueCompForNCEPNVRAMOffset + sizeof(m_accrueCompForNCEP);
	m_senOpAccrueCompForNCEPNVRAMOffset	= m_senOpAccruePointsForNCEPNVRAMOffset + sizeof(m_senOpAccruePointsForNCEP);
	m_gameInProgressNVRAMOffset	= m_senOpAccrueCompForNCEPNVRAMOffset + sizeof(m_senOpAccrueCompForNCEP);
	m_serviceWindowOpenNVRAMOffset	= m_gameInProgressNVRAMOffset + sizeof(m_gameInProgress);
	m_touchScreenProtocolNameNVRAMOffset	= m_serviceWindowOpenNVRAMOffset + sizeof(m_serviceWindowOpen);

	m_universeEnabledNVRAMOffset        = m_touchScreenProtocolNameNVRAMOffset + SIZE_OF_TOUCH_SCREEN_PROTOCOL_NAME;
	m_mixedModeEnabledNVRAMOffset		= m_universeEnabledNVRAMOffset + sizeof(m_universeEnabled);	
	m_numOfUniversePropCodesNVRAMOffset = m_mixedModeEnabledNVRAMOffset + sizeof(m_mixedModeEnabled);
	m_universePropCodesNVRAMOffset		= m_numOfUniversePropCodesNVRAMOffset + sizeof(int);
	m_universePlayerPrefixesNVRAMOffset	= m_universePropCodesNVRAMOffset + (NUMBER_OF_VALID_CARDS_PREFIX_IN_NVRAM * SIZE_OF_VALID_PROP_CODE);

	m_featureFlagsRawNVRAMOffset  = m_universePlayerPrefixesNVRAMOffset + NUMBER_OF_VALID_CARDS_PREFIX_IN_NVRAM; 
	m_atomicRedemptionsNVRAMOffset = m_featureFlagsRawNVRAMOffset + sizeof(m_featureFlagsRaw); 

	m_sessionPointsMessageNVRAMOffset = m_atomicRedemptionsNVRAMOffset + sizeof(m_atomicRedemptions);
	m_balanceRetrievalMessageNVRAMOffset = m_sessionPointsMessageNVRAMOffset + sizeof(m_sessionPointsMessage);
	m_pointsOrCashLabelNVRAMOffset = m_balanceRetrievalMessageNVRAMOffset + sizeof(m_balanceRetrievalMessage);
	m_showPointsVsAccountBalanceNVRAMOffset = m_pointsOrCashLabelNVRAMOffset + sizeof(m_pointsOrCashLabel);

	m_nclCardsEnabledNVRAMOffset = m_showPointsVsAccountBalanceNVRAMOffset + sizeof(m_showPointsVsAccountBalance);
	m_cardReaderTrackNumberNVRAMOffset = m_nclCardsEnabledNVRAMOffset + sizeof(m_nclCardsEnabled);

	m_negligibleCreditsThresholdNVRAMOffset = m_cardReaderTrackNumberNVRAMOffset + sizeof(m_cardReaderTrackNumber);
	m_employeeOverPlayerIdleCardOutNVRAMOffset = m_negligibleCreditsThresholdNVRAMOffset +
		sizeof(m_negligibleCreditsThreshold);
	m_employeeIdleCardOutSecondsNVRAMOffset = m_employeeOverPlayerIdleCardOutNVRAMOffset +
		sizeof(m_employeeOverPlayerIdleCardOut);
	m_playerRecardEmployeeCardOutNVRAMOffset = m_employeeIdleCardOutSecondsNVRAMOffset +
		sizeof(m_employeeIdleCardOutSeconds);

	m_notificationShowPromotionNVRAMOffset = m_playerRecardEmployeeCardOutNVRAMOffset +
		sizeof(m_playerRecardEmployeeCardOut);
	m_notificationPlayerTrackingNVRAMOffset = m_notificationShowPromotionNVRAMOffset +
		sizeof(m_notificationShowPromotion);
	m_customButton1enableNVRAMOffset = m_notificationPlayerTrackingNVRAMOffset +
		sizeof(m_notificationPlayerTracking);
	m_customMenuButton1CountNVRAMOffset = m_customButton1enableNVRAMOffset +
		sizeof(m_customButton1enable);
	m_customButton2enableNVRAMOffset = m_customMenuButton1CountNVRAMOffset +
		sizeof(m_customMenuButton1Count);
	m_customMenuButton2CountNVRAMOffset = m_customButton2enableNVRAMOffset +
		sizeof(m_customButton2enable);

	m_sessionActiveNegligibleCreditsTimeoutSecondsNVRAMOffset = m_customMenuButton2CountNVRAMOffset +
		sizeof(m_customMenuButton2Count);
	m_Unused4NVRAMOffset = m_sessionActiveNegligibleCreditsTimeoutSecondsNVRAMOffset +
		sizeof(m_sessionActiveNegligibleCreditsTimeoutSeconds);
	m_sessionActiveNonNegligibleCreditsTimeoutSecondsNVRAMOffset = m_Unused4NVRAMOffset + sizeof(m_unused4);
	m_jackpotExternalBonusNVRAMOffset = m_sessionActiveNonNegligibleCreditsTimeoutSecondsNVRAMOffset +
		sizeof(m_sessionActiveNonNegligibleCreditsTimeoutSeconds);

	m_coinsWonAtLastGameEndNVRAMOffset = m_jackpotExternalBonusNVRAMOffset + sizeof(m_jackpotExternalBonus);

	m_resortMenuButtonCountNVRAMOffset = m_coinsWonAtLastGameEndNVRAMOffset + sizeof(m_coinsWonAtLastGameEnd);

	m_configExt2CheckByteNVRAMOffset = m_resortMenuButtonCountNVRAMOffset + sizeof(m_resortMenuButtonCount);

	ConfigNVRAMSize = m_configExt2CheckByteNVRAMOffset +
		sizeof(m_configExt2CheckByteValue)
		- NVRAM_CONFIG_OFFSET;

	m_dailyPointsDisplaySuffixMessageNVRAMOffset = NVRAM_CONFIG_VERSION_2_OFFSET;

	m_tierPointsLabelNVRAMOffset = m_dailyPointsDisplaySuffixMessageNVRAMOffset + sizeof(m_dailyPointsDisplaySuffixMessage);
	m_ujpEnabledNVRAMOffset = m_tierPointsLabelNVRAMOffset + sizeof(m_tierPointsLabel);
	m_configExt3CheckByteNVRAMOffset = m_ujpEnabledNVRAMOffset + sizeof(m_ujpEnabled);
	m_miniVirtualSessionEnabledNVRAMOffset = m_configExt3CheckByteNVRAMOffset + sizeof(m_configExt3CheckByteValue);
	m_oneLinkProgressivesEnabledNVRAMOffset = m_miniVirtualSessionEnabledNVRAMOffset + sizeof(m_miniVirtualSessionEnabled);

	m_configExt4CheckByteNVRAMOffset = m_oneLinkProgressivesEnabledNVRAMOffset + sizeof(m_oneLinkProgressivesEnabled);
	m_rebootUiRestartOverrideOffset = m_configExt4CheckByteNVRAMOffset + sizeof(m_configExt4CheckByteValue);

	m_configExt5CheckByteNVRAMOffset = m_rebootUiRestartOverrideOffset + sizeof(m_rebootUiRestartOverride);
	m_displayPlayerProfileButtonOffset = m_configExt5CheckByteNVRAMOffset + sizeof(m_configExt5CheckByteValue);
	m_mgmdEnabledNVRAMOffset = m_displayPlayerProfileButtonOffset + sizeof(m_displayPlayerProfileButton);
	m_multiGameMultiDenomNVRAMOffset = m_mgmdEnabledNVRAMOffset + sizeof(m_mgmdEnabled);
	m_globalMGMDOffset = m_multiGameMultiDenomNVRAMOffset + sizeof(m_multiGameMultiDenom);
	m_gameStartExceptionsOffset = m_globalMGMDOffset + sizeof(m_globalMGMD);
	m_gameEndExceptionsOffset = m_gameStartExceptionsOffset + sizeof(m_gameStartExceptions);

	m_externalCreditTransferCheckByteOffset = m_gameEndExceptionsOffset + sizeof(m_gameEndExceptions);
	m_externalCreditToGameNVRAMOffset = m_externalCreditTransferCheckByteOffset + sizeof(m_externalCreditTransferCheckByteValue);

	m_configExt6CheckByteNVRAMOffset = m_externalCreditToGameNVRAMOffset + sizeof(m_externalCreditToGameNVRAMOffset);
	m_mobileConnectCommunicationEnabledNVRAMOffset = m_configExt6CheckByteNVRAMOffset + sizeof(m_configExt6CheckByteValue); /* flag not relevant at the moment */
	m_bonusCounterNotificationPriorityOffset = m_mobileConnectCommunicationEnabledNVRAMOffset + sizeof(m_mobileConnectCommEnabled);
	m_bonusCounternRichLevelPriorityOffset = m_bonusCounterNotificationPriorityOffset + sizeof(m_bonusCounterNotificationPriority);
	m_bonusCounterSplashdownPriorityOffset = m_bonusCounternRichLevelPriorityOffset + sizeof(m_bonusCounternRichLevelPriority);
	m_bonusCounterProgressiveOdometerPriorityOffset = m_bonusCounterSplashdownPriorityOffset + sizeof(m_bonusCounterSplashdownPriority);
	m_bonusCounterPtePriorityOffset = m_bonusCounterProgressiveOdometerPriorityOffset + sizeof(m_bonusCounterProgressiveOdometerPriority);
	m_bonusCounterScatterCountdownTimerPriorityOffset = m_bonusCounterPtePriorityOffset + sizeof(m_bonusCounterPtePriority);
	m_bonusCounterPrizePriorityOffset = m_bonusCounterScatterCountdownTimerPriorityOffset + sizeof(m_bonusCounterScatterCountdownTimerPriority);

	m_configExt7CheckByteNVRAMOffset = m_bonusCounterPrizePriorityOffset + sizeof(m_bonusCounterPrizePriority);
	m_globalExcessiveVoucherOutEnabledFlagOffset = m_configExt7CheckByteNVRAMOffset + sizeof(m_configExt7CheckByteValue);
	m_excessiveVoucherOutThresholdOffset = m_globalExcessiveVoucherOutEnabledFlagOffset + sizeof(m_globalExcessiveVoucherOutEnabledFlag);
	m_totalBillInThresholdOffset = m_excessiveVoucherOutThresholdOffset + sizeof(m_excessiveVoucherOutThreshold);
	m_coinInThresholdOffset = m_totalBillInThresholdOffset + sizeof(m_totalBillInThreshold);
	m_coinOutThresholdOffset = m_coinInThresholdOffset + sizeof(m_coinInThreshold);
	m_bonusCounterPlayXGetYPriorityOffset = m_coinOutThresholdOffset + sizeof(m_coinOutThreshold);

	m_remoteDebuggingEnabledNVRAMOffset = m_bonusCounterPlayXGetYPriorityOffset + sizeof(m_bonusCounterPlayXGetYPriority);
	m_partialWATOutEnabledNVRAMOffset = m_remoteDebuggingEnabledNVRAMOffset + sizeof(m_remoteDebuggingEnabled);
	m_partialWATOutEnabledGlobalNVRAMOffset = m_partialWATOutEnabledNVRAMOffset + sizeof(m_partialWATOutEnabled);
	m_autoUploadCreditsGlobalNVRAMOffset = m_partialWATOutEnabledGlobalNVRAMOffset + sizeof(m_partialWATOutEnabled_global);
	m_cashoutToHostEnabledNVRAMOffset = m_autoUploadCreditsGlobalNVRAMOffset + sizeof(m_autoUploadCreditsGlobal);
	m_cashoutToHostEnabledGlobalNVRAMOffset = m_cashoutToHostEnabledNVRAMOffset + sizeof(m_cashoutToHostEnabled);
	m_extensibleFlagsNVRAMOffset = m_cashoutToHostEnabledGlobalNVRAMOffset + sizeof(m_cashoutToHostEnabled_global);
	m_taxWithholdingEnabledOffsetNVRAMOffset = m_extensibleFlagsNVRAMOffset + sizeof(m_extensibleFlags.m_wordValue);
	m_federalTaxWithholdingPercentOffsetNVRAMOffset = m_taxWithholdingEnabledOffsetNVRAMOffset + sizeof(m_taxWithholdingEnabled);
	m_stateTaxWithholdingPercentOffsetNVRAMOffset = m_federalTaxWithholdingPercentOffsetNVRAMOffset + sizeof(m_federalTaxWithholdingPercent);
	m_numberOfSubgamesMismatchCheckOffset = m_stateTaxWithholdingPercentOffsetNVRAMOffset + sizeof(m_stateTaxWithholdingPercent);
	m_systemTimeSprinkleOffset = m_numberOfSubgamesMismatchCheckOffset + sizeof(m_numberOfSubgamesMismatchCheck);
	m_systemTimeOffset = m_systemTimeSprinkleOffset + sizeof(NVRAM_SYSTEM_TIME_SPRINKLE_VALUE);
	m_rebootAsapThresholdInMbOffset = m_systemTimeOffset + sizeof(SYSTEMTIME);
	m_rebootWarningThresholdMbOffset = m_rebootAsapThresholdInMbOffset + sizeof(m_rebootAsapThresholdInMb);
	m_restartUIAsapThresholdMbOffset = m_rebootWarningThresholdMbOffset + sizeof(m_rebootWarningThresholdMb);
	m_restartUIAfterDropThresholdMbOffset = m_restartUIAsapThresholdMbOffset + sizeof(m_restartUIAsapThresholdMb);
	m_restartUIDelayHoursOffset = m_restartUIAfterDropThresholdMbOffset + sizeof(m_restartUIAfterDropThresholdMb);
	m_gameListChunkSizeOffset = NVRAM_GAME_LIST_CHUNKS_CONFIG_OFFSET;//This is 723290.
}

void CConfig::BuildYourself()
{
	CEEPROMConfig eepromConfig;
	CNVRAMConfig nvramConfig;

	InitializeMACAddress();

	eepromConfig.Get(EEPROM_SLOTMAST_ID_OFFSET,		(BYTE *)&m_slotMastID,		sizeof(m_slotMastID));
	eepromConfig.Get(EEPROM_CASINO_NUMBER_OFFSET,	(BYTE *)&m_casinoNumber,	sizeof(m_casinoNumber));
	eepromConfig.Get(EEPROM_COMM_ID_OFFSET,			(BYTE *)&m_commID,			sizeof(m_commID));
	eepromConfig.Get(EEPROM_MACHINE_TYPE_OFFSET,	(BYTE *)&m_machineType,		sizeof(m_machineType));
	eepromConfig.Get(EEPROM_SLOT_NUMBER_OFFSET,		(BYTE *)&m_slotNumber,		sizeof(m_slotNumber));
	eepromConfig.Get(EEPROM_HARD_PIN_TYPES_OFFSET,	(BYTE *)&m_hardPinTypes,	sizeof(m_hardPinTypes));
	eepromConfig.Get(EEPROM_HARD_PIN_ACTIVE_STATE_OFFSET, (BYTE *)&m_hardPinActiveState, sizeof(m_hardPinActiveState));
	eepromConfig.Get(EEPROM_SERIAL_PROTOCOL_OFFSET, (BYTE *)&m_serialProtocol,	sizeof(m_serialProtocol));
	eepromConfig.Get(EEPROM_SCREEN_BRIGHTNESS_OFFSET,    (BYTE *) &m_screenBrightness,   sizeof(m_screenBrightness));
	eepromConfig.Get(EEPROM_LEFT_SPEAKER_VOLUME_OFFSET,  (BYTE *) &m_leftSpeakerVolume,  sizeof(m_leftSpeakerVolume));
	eepromConfig.Get(EEPROM_RIGHT_SPEAKER_VOLUME_OFFSET, (BYTE *) &m_rightSpeakerVolume, sizeof(m_rightSpeakerVolume));
	eepromConfig.Get(EEPROM_GLOBAL_CARD_PASS_OFFSET, (BYTE *)&m_globalCardPassword, sizeof(m_globalCardPassword));
	eepromConfig.Get(EEPROM_DHCPAUTO_OFFSET,		(BYTE *)&m_DHCPAuto,		sizeof(m_DHCPAuto));
	eepromConfig.Get(EEPROM_HOST_ADDRESS_OFFSET,	(BYTE *)m_hostAddressOrName,SIZE_OF_NAME_IP);
	m_hostAddressOrName[SIZE_OF_NAME_IP] = '\0';
	eepromConfig.Get(EEPROM_HOST_PORT_OFFSET,		(BYTE *)&m_hostPort,			sizeof(m_hostPort));
	eepromConfig.Get(EEPROM_MY_IP_OFFSET,			(BYTE *)m_myIP,				sizeof(m_myIP));
	eepromConfig.Get(EEPROM_DEFAULT_GATEWAY_OFFSET,	(BYTE *)m_defaultGateway,	sizeof(m_defaultGateway));
	eepromConfig.Get(EEPROM_SUBNET_MASK_OFFSET,		(BYTE *)m_subnetMask,		sizeof(m_subnetMask));
	eepromConfig.Get(EEPROM_PREFERRED_DNS_OFFSET,	(BYTE *)m_preferredDNSServer,sizeof(m_preferredDNSServer));
	eepromConfig.Get(EEPROM_ALT_DNS_OFFSET,			(BYTE *)m_alternateDNSServer,sizeof(m_alternateDNSServer));
	eepromConfig.Get(EEPROM_GAME_DENOM_OFFSET,		(BYTE *)&m_gameDenom,		sizeof(m_gameDenom));
	m_batteryStatus = batteryStatusUnknown;
	eepromConfig.Get(EEPROM_BATTERY_STATUS_OFFSET,	(BYTE *)&m_batteryStatus,	EEPROM_BATTERY_STATUS_SIZE);

	eepromConfig.Get(EEPROM_TSPROTO_TYPE_OFFSET, (BYTE *)&m_touchScreenProtocolType, EEPROM_TSPROTO_TYPE_SIZE);
	eepromConfig.Get(EEPROM_TSPROTO_VENDOR_ID_OFFSET, (BYTE *)&m_touchScreenProtocolVendorId, EEPROM_TSPROTO_VENDOR_ID_SIZE);
	eepromConfig.Get(EEPROM_TSPROTO_BAUD_OFFSET, (BYTE *)&m_touchScreenProtocolBaud, EEPROM_TSPROTO_BAUD_SIZE);

	eepromConfig.Get(EEPROM_MIXER_REBOOT_COUNT_OFFSET, (BYTE *)&m_mixerRebootCount, EEPROM_MIXER_REBOOT_COUNT_SIZE);
	if ( m_mixerRebootCount > EEPROM_MIXER_REBOOT_COUNT_MAX )
	{
		m_mixerRebootCount = 0;	// This will reset the count if greater than the max. Assume "wacky" values are unitialized and should be reset to 0.
		eepromConfig.Set(EEPROM_MIXER_REBOOT_COUNT_OFFSET, (BYTE *)&m_mixerRebootCount, EEPROM_MIXER_REBOOT_COUNT_SIZE);
	}

	BYTE tempBYTE;
	eepromConfig.Get(EEPROM_VIDEO_INPUT_AUTO_DETECT_OFFSET, &tempBYTE, EEPROM_VIDEO_INPUT_AUTO_DETECT_SIZE);
	if ( tempBYTE == EEPROM_VIDEO_INPUT_AUTO_DETECT_MANUAL )
	{
		m_autoDetectInputVideoMode = false;
	}
	else
	{
		m_autoDetectInputVideoMode = true;
	}
	eepromConfig.Get(EEPROM_VIDEO_OUTPUT_AUTO_CONFIG_OFFSET, &tempBYTE, EEPROM_VIDEO_OUTPUT_AUTO_CONFIG_SIZE); 
	if ( tempBYTE == EEPROM_VIDEO_OUTPUT_AUTO_CONFIG_MANUAL ) 
	{ 
		m_outputVideoModeConfig = EEPROM_VIDEO_OUTPUT_AUTO_CONFIG_MANUAL; 
	} 
	else if ( tempBYTE == EEPROM_VIDEO_OUTPUT_AUTO_CONFIG_NATIVE )
	{ 
		m_outputVideoModeConfig = EEPROM_VIDEO_OUTPUT_AUTO_CONFIG_NATIVE; 
	}
	else
	{
		m_outputVideoModeConfig = EEPROM_VIDEO_OUTPUT_AUTO_CONFIG_AUTO; 
	}

	tempBYTE = EEPROM_DESKTOP_ORIENTATION_DEFAULT;
	eepromConfig.Get(EEPROM_DESKTOP_ORIENTATION_OFFSET, &tempBYTE, EEPROM_DESKTOP_ORIENTATION_SIZE);
	switch ( tempBYTE )
	{
	case EEPROM_DESKTOP_ORIENTATION_90:
		m_desktopOrientationType = DesktopOrientation90;
		break;
	case EEPROM_DESKTOP_ORIENTATION_180:
		m_desktopOrientationType = DesktopOrientation180;
		break;
	case EEPROM_DESKTOP_ORIENTATION_270:
		m_desktopOrientationType = DesktopOrientation270;
		break;
	case EEPROM_DESKTOP_ORIENTATION_NONE:
	default:
		m_desktopOrientationType = DesktopOrientationDefault;
		break;
	}

	byte timeBuffer[NVRAM_TIME_SIZE];
	eepromConfig.Get(EEPROM_BATTERY_STATUS_TIME_OFFSET,	(BYTE *)timeBuffer,		NVRAM_TIME_SIZE);


	m_lastBatteryCheckTime = CUtilities::MakeTimeFromNvramBuffer( timeBuffer, NVRAM_TIME_SIZE );

	nvramConfig.Get(m_EGMLockoutStateNVRAMOffset, (BYTE*)&m_EGMLockoutState, sizeof(m_EGMLockoutState));
	nvramConfig.Get(m_memoryLostNVRAMOffset, (BYTE*)&m_memoryLost, sizeof(m_memoryLost));
	nvramConfig.Get(m_lastCoinsPlayedNVRAMOffset, (BYTE*)&m_lastCoinsPlayed, sizeof(m_lastCoinsPlayed));
	nvramConfig.Get(m_Unused3NVRAMOffset, (BYTE*)&m_Unused3  , sizeof(m_Unused3));
	nvramConfig.Get(m_lastPBTransactionIDNVRAMOffset, (BYTE*)&m_lastPBTransactionID , sizeof(m_lastPBTransactionID));
	nvramConfig.Get(m_PBResponsePendingNVRAMOffset, (BYTE*)&m_PBResponsePending , sizeof(m_PBResponsePending));
	nvramConfig.Get(m_pbtPromoEnabled_globalNVRAMOffset, (BYTE*)&m_pbtPromoEnabled_global , sizeof(m_pbtPromoEnabled_global));
	nvramConfig.Get(m_pbtPointsEnabled_globalNVRAMOffset, (BYTE*)&m_pbtPointsEnabled_global , sizeof(m_pbtPointsEnabled_global));
	nvramConfig.Get(m_pbtCashEnabled_globalNVRAMOffset, (BYTE*)&m_pbtCashEnabled_global , sizeof(m_pbtCashEnabled_global));
	nvramConfig.Get(m_PBTransactionIDNVRAMOffset, (BYTE*)&m_PBTransactionID , sizeof(m_PBTransactionID));
	nvramConfig.Get(m_PBTransferSequenceNumberNVRAMOffset, (BYTE*)&m_PBTransferSequenceNumber , sizeof(m_PBTransferSequenceNumber));
	nvramConfig.Get(m_PBCreditSourceNVRAMOffset, (BYTE*)&m_PBCreditSource , sizeof(m_PBCreditSource));
	nvramConfig.Get(m_PBTransferInProgressNVRAMOffset, (BYTE*)&m_PBTransferInProgress , sizeof(m_PBTransferInProgress));
	nvramConfig.Get(m_currentPBTransactionIDNVRAMOffset, (BYTE*)&m_currentPBTransactionID , sizeof(m_currentPBTransactionID));
	nvramConfig.Get(m_PBTransferAmountNVRAMOffset, (BYTE*)&m_PBTransferAmount , sizeof(m_PBTransferAmount));
	nvramConfig.Get(m_PBReleaseNeededNVRAMOffset, (BYTE*)&m_PBReleaseNeeded , sizeof(m_PBReleaseNeeded));
	nvramConfig.Get(m_bonusEventBoundariesNVRAMOffset, (BYTE*)&m_bonusEventBoundaries , sizeof(m_bonusEventBoundaries));
	nvramConfig.Get(m_ticketVendResponseInProgressNVRAMOffset, (BYTE*)&m_ticketVendResponseInProgress , sizeof(m_ticketVendResponseInProgress));
	nvramConfig.Get(m_employeeAuthorizationOCRNVRAMOffset, (BYTE*)&m_employeeAuthorizationOCR , sizeof(m_employeeAuthorizationOCR));
	nvramConfig.Get(m_attractorMessageNVRAMOffset, (BYTE*)&m_attractorMessage , sizeof(m_attractorMessage));
	nvramConfig.Get(m_goodCardReadMessageNVRAMOffset, (BYTE*)&m_goodCardReadMessage , sizeof(m_goodCardReadMessage));
	nvramConfig.Get(m_badCarReadMessageNVRAMOffset, (BYTE*)&m_badCarReadMessage , sizeof(m_badCarReadMessage));
	nvramConfig.Get(m_birthdayMessageNVRAMOffset, (BYTE*)&m_birthdayMessage , sizeof(m_birthdayMessage));
	nvramConfig.Get(m_anniversaryMessageNVRAMOffset, (BYTE*)&m_anniversaryMessage , sizeof(m_anniversaryMessage));
	nvramConfig.Get(m_congratulationsMessageNVRAMOffset, (BYTE*)&m_congratulationsMessage , sizeof(m_congratulationsMessage));
	nvramConfig.Get(m_welcomeMessageNVRAMOffset, (BYTE*)&m_welcomeMessage , sizeof(m_welcomeMessage));

	char * tempCasinoName = new char[SIZE_OF_CASINO_NAME + 1];
	nvramConfig.Get(m_casinoNameNVRAMOffset, (BYTE*)tempCasinoName , SIZE_OF_CASINO_NAME);
	tempCasinoName[SIZE_OF_CASINO_NAME] = 0;
	m_strCasinoName = tempCasinoName;
	delete [] tempCasinoName;

	char * tempCasinoAddress = new char[SIZE_OF_CASINO_ADDRESS1 + 1];
	nvramConfig.Get(m_casinoAddress1NVRAMOffset, (BYTE*)tempCasinoAddress, SIZE_OF_CASINO_ADDRESS1);
	tempCasinoAddress[SIZE_OF_CASINO_ADDRESS1] = 0;
	m_casinoAddress1 = tempCasinoAddress;
	delete [] tempCasinoAddress;

	tempCasinoAddress = new char[SIZE_OF_CASINO_ADDRESS2 + 1];
	nvramConfig.Get(m_casinoAddress2NVRAMOffset, (BYTE*)tempCasinoAddress, SIZE_OF_CASINO_ADDRESS2);
	tempCasinoAddress[SIZE_OF_CASINO_ADDRESS2] = 0;
	m_casinoAddress2 = tempCasinoAddress;
	delete [] tempCasinoAddress;

	// Load the player card prefix vector from NVRAM
	{
		int numberOfCardPrefixes;
		BYTE playerCardPrefixes[NUMBER_OF_VALID_CARDS_PREFIX_IN_NVRAM];
		nvramConfig.Get(m_numberOfCardPrefixesNVRAMOffset, (BYTE*)&numberOfCardPrefixes , sizeof(numberOfCardPrefixes));
		nvramConfig.Get(m_playerCardPrefixesNVRAMOffset, (BYTE*)&playerCardPrefixes , sizeof(playerCardPrefixes));
		numberOfCardPrefixes = min(numberOfCardPrefixes, NUMBER_OF_VALID_CARDS_PREFIX_IN_NVRAM);
		m_playerCardPrefixes.clear();
		for(int prefixIndex = 0; prefixIndex < numberOfCardPrefixes; prefixIndex++)
		{
			m_playerCardPrefixes.push_back(playerCardPrefixes[prefixIndex]);
		}
	}

	nvramConfig.Get(m_globalCardPrefixNVRAMOffset, (BYTE*)&m_globalCardPrefix , sizeof(m_globalCardPrefix));
	nvramConfig.Get(m_mechanicCardPrefixNVRAMOffset, (BYTE*)&m_mechanicCardPrefix , sizeof(m_mechanicCardPrefix));
	nvramConfig.Get(m_floorCardPrefixNVRAMOffset, (BYTE*)&m_floorCardPrefix , sizeof(m_floorCardPrefix));
	nvramConfig.Get(m_tempCardPrefixNVRAMOffset, (BYTE*)&m_tempCardPrefix , sizeof(m_tempCardPrefix));

	nvramConfig.Get(m_bLossLimitActiveNVRAMOffset, (BYTE*)&m_bLossLimitActive , sizeof(m_bLossLimitActive));
	nvramConfig.Get(m_lossLimitSessionNumberNVRAMOffset, (BYTE*)&m_lossLimitSessionNumber , sizeof(m_lossLimitSessionNumber));
	nvramConfig.Get(m_lossLimitSessionAmountNVRAMOffset, (BYTE*)&m_lossLimitSessionAmount , sizeof(m_lossLimitSessionAmount));
	nvramConfig.Get(m_lossLimitBillValidatorOffAmountNVRAMOffset, (BYTE*)&m_lossLimitBillValidatorOffAmount , sizeof(m_lossLimitBillValidatorOffAmount));
	nvramConfig.Get(m_lossLimitMessageNVRAMOffset, (BYTE*)&m_lossLimitMessage , sizeof(m_lossLimitMessage));
	nvramConfig.Get(m_cashTransferTrackingNVRAMOffset, (BYTE*)&m_cashTransferTracking , sizeof(m_cashTransferTracking));
	nvramConfig.Get(m_progressiveDenominationNVRAMOffset, (BYTE*)&m_progressiveDenomination , sizeof(m_progressiveDenomination));

	nvramConfig.Get(m_floorLockNVRAMOffset, (BYTE*)&m_floorLock , sizeof(m_floorLock));
	nvramConfig.Get(m_ticketExpTypeNVRAMOffset, (BYTE*)&m_ticketExpType , sizeof(m_ticketExpType));
	nvramConfig.Get(m_ticketExpirationDurationNVRAMOffset, (BYTE*)&m_ticketExpirationDuration , sizeof(m_ticketExpirationDuration));
	nvramConfig.Get(m_ticketingEnabledNVRAMOffset, (BYTE*)&m_ticketingEnabled , sizeof(m_ticketingEnabled));
	nvramConfig.Get(m_systemGeneratedTicketValidationNVRAMOffset, (BYTE*)&m_systemGeneratedTicketValidation , sizeof(m_systemGeneratedTicketValidation));
	nvramConfig.Get(m_ticketBufferingEnabledNVRAMOffset, (BYTE*)&m_ticketBufferingEnabled , sizeof(m_ticketBufferingEnabled));
	nvramConfig.Get(m_playerTrackingMultiplierNVRAMOffset, (BYTE*)&m_playerTrackingMultiplier , sizeof(m_playerTrackingMultiplier));
	nvramConfig.Get(m_playerTrackingDivisorNVRAMOffset, (BYTE*)&m_playerTrackingDivisor , sizeof(m_playerTrackingDivisor));
	nvramConfig.Get(m_virtualSessionMaxDurationNVRAMOffset, (BYTE*)& m_virtualSessionMaxDuration, sizeof(m_virtualSessionMaxDuration));
	nvramConfig.Get(m_virtualSessionNoPlayTimeoutNVRAMOffset, (BYTE*)&m_virtualSessionNoPlayTimeout , sizeof(m_virtualSessionNoPlayTimeout));
	nvramConfig.Get(m_cardTimeoutNVRAMOffset, (BYTE*)&m_cardTimeout , sizeof(m_cardTimeout));
	nvramConfig.Get(m_fillAmountNVRAMOffset, (BYTE*)&m_fillAmount , sizeof(m_fillAmount));
	nvramConfig.Get(m_useHopperLevelNVRAMOffset, (BYTE*)&m_useHopperLevel , sizeof(m_useHopperLevel));
	nvramConfig.Get(m_virtualSessionEnabledNVRAMOffset, (BYTE*)&m_virtualSessionEnabled, sizeof(m_virtualSessionEnabled));
	nvramConfig.Get(m_displayPlayerNameNVRAMOffset, (BYTE*)&m_displayPlayerName , sizeof(m_displayPlayerName));
	nvramConfig.Get(m_displayPlayerPtsTotalNVRAMOffset, (BYTE*)&m_displayPlayerPtsTotal, sizeof(m_displayPlayerPtsTotal));
	nvramConfig.Get(m_displayCountdownAndSessionNVRAMOffset, (BYTE*)&m_displayCountdownAndSession , sizeof(m_displayCountdownAndSession));
	nvramConfig.Get(m_displayGoodLuckAndSessionPtsNVRAMOffset, (BYTE*)&m_displayGoodLuckAndSessionPts , sizeof(m_displayGoodLuckAndSessionPts));
	nvramConfig.Get(m_displayGoodLuckAndSessionNVRAMOffset, (BYTE*)&m_displayGoodLuckAndSession , sizeof(m_displayGoodLuckAndSession));
	nvramConfig.Get(m_displayNumericPointTotalNVRAMOffset, (BYTE*)&m_displayNumericPointTotal, sizeof(m_displayNumericPointTotal));
	nvramConfig.Get(m_displayCompsNVRAMOffset, (BYTE*)&m_displayComps , sizeof(m_displayComps));
	nvramConfig.Get(m_displayCongratsNVRAMOffset, (BYTE*)&m_displayCongrats , sizeof(m_displayCongrats));
	nvramConfig.Get(m_displayPtsTotalAsCashNVRAMOffset, (BYTE*)&m_displayPtsTotalAsCash , sizeof(m_displayPtsTotalAsCash));
	nvramConfig.Get(m_displayPtsTotalAsCashSessionNVRAMOffset, (BYTE*)&m_displayPtsTotalAsCashSession , sizeof(m_displayPtsTotalAsCashSession));
	nvramConfig.Get(m_displayTotalAndSessionPtsNumNVRAMOffset, (BYTE*)&m_displayTotalAndSessionPtsNum, sizeof(m_displayTotalAndSessionPtsNum));
	nvramConfig.Get(m_displayTotalPtsSessionPtsAsCashNVRAMOffset, (BYTE*)&m_displayTotalPtsSessionPtsAsCash, sizeof(m_displayTotalPtsSessionPtsAsCash));
	nvramConfig.Get(m_pbtCashEnabled_localNVRAMOffset, (BYTE*)&m_pbtCashEnabled_local , sizeof(m_pbtCashEnabled_local));
	nvramConfig.Get(m_pbtPointsAsPromoNVRAMOffset, (BYTE*)&m_pbtPointsAsPromo, sizeof(m_pbtPointsAsPromo));
	nvramConfig.Get(m_pbtPointsEnabled_localNVRAMOffset, (BYTE*)&m_pbtPointsEnabled_local , sizeof(m_pbtPointsEnabled_local));
	nvramConfig.Get(m_pbtPromoEnabled_localNVRAMOffset, (BYTE*)&m_pbtPromoEnabled_local , sizeof(m_pbtPromoEnabled_local));
	nvramConfig.Get(m_coinsWonAccountingNVRAMOffset, (BYTE*)&m_coinsWonAccounting , sizeof(m_coinsWonAccounting));
	nvramConfig.Get(m_coinsInAccountingNVRAMOffset, (BYTE*)&m_coinsInAccounting , sizeof(m_coinsInAccounting));
	nvramConfig.Get(m_coinsOutAccountingNVRAMOffset	, (BYTE*)&m_coinsOutAccounting , sizeof(m_coinsOutAccounting));
	nvramConfig.Get(m_theoreticalAccountingNVRAMOffset, (BYTE*)&m_theoreticalAccounting , sizeof(m_theoreticalAccounting));
	nvramConfig.Get(m_cardInPBTNVRAMOffset, (BYTE*)&m_cardInPBT , sizeof(m_cardInPBT));
	nvramConfig.Get(m_employeePinRequiredNVRAMOffset, (BYTE*)&m_employeePinRequired , sizeof(m_employeePinRequired));
	nvramConfig.Get(m_PartialBonusAwardNVRAMOffset, (BYTE*)&m_PartialBonusAward , sizeof(m_PartialBonusAward));
	nvramConfig.Get(m_PBFromGameEnabledNVRAMOffset, (BYTE*)&m_PBFromGameEnabled, sizeof(m_PBFromGameEnabled));
	nvramConfig.Get(m_couponPromoInEnabledNVRAMOffset, (BYTE*)&m_couponPromoInEnabled , sizeof(m_couponPromoInEnabled));
	nvramConfig.Get(m_couponPromoOutEnabledNVRAMOffset, (BYTE*)&m_couponPromoOutEnabled , sizeof(m_couponPromoOutEnabled));
	nvramConfig.Get(m_voucherPromoInEnabledNVRAMOffset, (BYTE*)&m_voucherPromoInEnabled, sizeof(m_voucherPromoInEnabled));
	nvramConfig.Get(m_PBTPinNotRequiredNVRAMOffset, (BYTE*)&m_PBTPinNotRequired, sizeof(m_PBTPinNotRequired));
	nvramConfig.Get(m_voucherOutEnabledNVRAMOffset, (BYTE*)&m_voucherOutEnabled, sizeof(m_voucherOutEnabled));
	nvramConfig.Get(m_ticketInEnabledNVRAMOffset, (BYTE*)&m_ticketInEnabled , sizeof(m_ticketInEnabled));
	nvramConfig.Get(m_pendingSlotmastNVRAMOffset, (BYTE*)&m_pendingSlotmast, sizeof(m_pendingSlotmast)); 
	nvramConfig.Get(m_allowFloorLockoutNVRAMOffset, (BYTE*)&m_allowFloorLockout, sizeof(m_allowFloorLockout));
	nvramConfig.Get(m_timezoneOffsetNVRAMOffset, (BYTE*)&m_timezoneOffset, sizeof(m_timezoneOffset));
	nvramConfig.Get(m_useDSTNVRAMOffset, (BYTE*)&m_useDST, sizeof(m_useDST));
	nvramConfig.Get(m_slotMastRevisionNVRAMOffset,	(BYTE*)&m_slotMastRevision, sizeof(m_slotMastRevision));
	nvramConfig.Get(m_lastGameStartNVRAMOffset,		(BYTE *)&m_lastGameStart,	sizeof(m_lastGameStart));
	nvramConfig.Get(m_coinInAtLastGameStartNVRAMOffset, (BYTE *)&m_coinInAtLastGameStart, sizeof(m_coinInAtLastGameStart));
	nvramConfig.Get(m_lastCardReadNVRAMOffset, m_lastCardRead, CARD_ID_LEN);
	nvramConfig.Get(m_senOption1RawNVRAMOffset, (BYTE *)&m_senOption1Raw, sizeof(m_senOption1Raw));
	nvramConfig.Get(m_senOption2RawNVRAMOffset, (BYTE *)&m_senOption2Raw, sizeof(m_senOption2Raw));
	nvramConfig.Get(m_denominationNVRAMOffset, (BYTE *)&m_denomination, sizeof(m_denomination));
	nvramConfig.Get(m_pointsPerDollarNVRAMOffset, (BYTE *)&m_pointsPerDollar, sizeof(m_pointsPerDollar));
	nvramConfig.Get(m_rcclCardsEnabledNVRAMOffset, (BYTE *)&m_rcclCardsEnabled, sizeof(m_rcclCardsEnabled));
	nvramConfig.Get(m_jackpotA8ResetNVRAMOffset, (BYTE *)&m_jackpotA8Reset, sizeof(bool));

	nvramConfig.Get(m_gameInProgressNVRAMOffset, (BYTE*)&m_gameInProgress, sizeof(m_gameInProgress));
	nvramConfig.Get(m_serviceWindowOpenNVRAMOffset, (BYTE*)&m_serviceWindowOpen, sizeof(m_serviceWindowOpen));
	nvramConfig.Get(m_senOpAccruePointsForNCEPNVRAMOffset, (BYTE *)&m_senOpAccruePointsForNCEP, sizeof(m_senOpAccruePointsForNCEP));
	nvramConfig.Get(m_senOpAccrueCompForNCEPNVRAMOffset, (BYTE *)&m_senOpAccrueCompForNCEP, sizeof(m_senOpAccrueCompForNCEP));
	nvramConfig.Get(m_accruePointsForNCEPNVRAMOffset, (BYTE *)&m_accruePointsForNCEP, sizeof(m_accruePointsForNCEP));
	nvramConfig.Get(m_accrueCompForNCEPNVRAMOffset, (BYTE *)&m_accrueCompForNCEP, sizeof(m_accrueCompForNCEP));

	char * tempTouchScreenProtocolName = new char[SIZE_OF_TOUCH_SCREEN_PROTOCOL_NAME + 1];
	nvramConfig.Get(m_touchScreenProtocolNameNVRAMOffset, (BYTE*)tempTouchScreenProtocolName , SIZE_OF_TOUCH_SCREEN_PROTOCOL_NAME);
	tempTouchScreenProtocolName[SIZE_OF_TOUCH_SCREEN_PROTOCOL_NAME] = 0;
	m_touchScreenProtocolName = tempTouchScreenProtocolName;
	delete [] tempTouchScreenProtocolName;

    nvramConfig.Get(m_universeEnabledNVRAMOffset, (BYTE *)&m_universeEnabled, sizeof(m_universeEnabled));
	nvramConfig.Get(m_mixedModeEnabledNVRAMOffset, (BYTE *)&m_mixedModeEnabled, sizeof(m_mixedModeEnabled));


    // Load the prop codes and universe player prefixes from NVRAM
    {
        int numberOfPropCodes;
        BYTE universePropCodes[NUMBER_OF_VALID_CARDS_PREFIX_IN_NVRAM * SIZE_OF_VALID_PROP_CODE];
        BYTE universeCardPrefixes[NUMBER_OF_VALID_CARDS_PREFIX_IN_NVRAM];
        nvramConfig.Get(m_numOfUniversePropCodesNVRAMOffset, (BYTE *)&numberOfPropCodes, sizeof(numberOfPropCodes));	
        nvramConfig.Get(m_universePropCodesNVRAMOffset, (BYTE *)&universePropCodes, sizeof(universePropCodes));
        nvramConfig.Get(m_universePlayerPrefixesNVRAMOffset, (BYTE *)&universeCardPrefixes, sizeof(universeCardPrefixes));

        numberOfPropCodes = min(numberOfPropCodes, NUMBER_OF_VALID_CARDS_PREFIX_IN_NVRAM);
        m_universePropCodes.clear();
        m_universePlayerCardPrefixes.clear();
        for(int propCodeIndex = 0; propCodeIndex < numberOfPropCodes; propCodeIndex++)
        {

            m_universePropCodes.push_back( *(reinterpret_cast<WORD *>(&universePropCodes[propCodeIndex * SIZE_OF_VALID_PROP_CODE])));
            m_universePlayerCardPrefixes.push_back(universeCardPrefixes[propCodeIndex]);
        }
    }

	nvramConfig.Get(m_featureFlagsRawNVRAMOffset, (BYTE *)&m_featureFlagsRaw, sizeof(m_featureFlagsRaw));
	nvramConfig.Get(m_atomicRedemptionsNVRAMOffset, (BYTE *)&m_atomicRedemptions, sizeof(m_atomicRedemptions));
	nvramConfig.Get(m_jackpotExternalBonusNVRAMOffset, (BYTE *)&m_jackpotExternalBonus, sizeof(m_jackpotExternalBonus));

	nvramConfig.Get(m_sessionPointsMessageNVRAMOffset, (BYTE *)&m_sessionPointsMessage, sizeof(m_sessionPointsMessage));
	nvramConfig.Get(m_balanceRetrievalMessageNVRAMOffset, (BYTE *)&m_balanceRetrievalMessage, sizeof(m_balanceRetrievalMessage));
	nvramConfig.Get(m_pointsOrCashLabelNVRAMOffset, (BYTE *)&m_pointsOrCashLabel, sizeof(m_pointsOrCashLabel));
	nvramConfig.Get(m_showPointsVsAccountBalanceNVRAMOffset, (BYTE *)&m_showPointsVsAccountBalance, sizeof(m_showPointsVsAccountBalance));

	nvramConfig.Get(m_nclCardsEnabledNVRAMOffset, (BYTE *)&m_nclCardsEnabled, sizeof(m_nclCardsEnabled));

	// Get a valid value before updating m_cardReaderTrackNumber.
	tempBYTE = CARD_READER_TRACK_NUMBER_DEFAULT;
	nvramConfig.Get(m_cardReaderTrackNumberNVRAMOffset, &tempBYTE, sizeof(tempBYTE));
	if (ValidateCardReaderTrackNumber(tempBYTE))
	{
		// The value was not valid and was changed.  Write the valid value back to peristent storage.
		nvramConfig.Set(m_cardReaderTrackNumberNVRAMOffset, &tempBYTE, sizeof(tempBYTE));
	}
	m_cardReaderTrackNumber = tempBYTE;

	nvramConfig.Get(m_negligibleCreditsThresholdNVRAMOffset,
		(BYTE *)&m_negligibleCreditsThreshold,
		sizeof(m_negligibleCreditsThreshold));
	nvramConfig.Get(m_employeeOverPlayerIdleCardOutNVRAMOffset,
		(BYTE *)&m_employeeOverPlayerIdleCardOut,
		sizeof(m_employeeOverPlayerIdleCardOut));
	nvramConfig.Get(m_employeeIdleCardOutSecondsNVRAMOffset,
		(BYTE *)&m_employeeIdleCardOutSeconds,
		sizeof(m_employeeIdleCardOutSeconds));
	nvramConfig.Get(m_playerRecardEmployeeCardOutNVRAMOffset,
		(BYTE *)&m_playerRecardEmployeeCardOut,
		sizeof(m_playerRecardEmployeeCardOut));

	nvramConfig.Get(m_dailyPointsDisplaySuffixMessageNVRAMOffset, (BYTE *)&m_dailyPointsDisplaySuffixMessage, sizeof(m_dailyPointsDisplaySuffixMessage));
	nvramConfig.Get(m_tierPointsLabelNVRAMOffset, (BYTE *)&m_tierPointsLabel, sizeof(m_tierPointsLabel));
	nvramConfig.Get(m_ujpEnabledNVRAMOffset, (BYTE *)&m_ujpEnabled, sizeof(m_ujpEnabled));
	nvramConfig.Get(m_miniVirtualSessionEnabledNVRAMOffset, (BYTE *)&m_miniVirtualSessionEnabled, sizeof(m_miniVirtualSessionEnabled));
	nvramConfig.Get(m_oneLinkProgressivesEnabledNVRAMOffset, (BYTE *)&m_oneLinkProgressivesEnabled, sizeof(m_oneLinkProgressivesEnabled));
	nvramConfig.Get(m_rebootUiRestartOverrideOffset, (BYTE *)&m_rebootUiRestartOverride, sizeof(m_rebootUiRestartOverride));
	nvramConfig.Get(m_displayPlayerProfileButtonOffset, (BYTE *)&m_displayPlayerProfileButton, sizeof(m_displayPlayerProfileButton));
	nvramConfig.Get(m_externalCreditToGameNVRAMOffset, (BYTE *)&m_ExternalCreditToGameEnabled, sizeof(m_ExternalCreditToGameEnabled));

	LogString(PBT_LOG, "On offset %d in Nvram: getting the external credit to game enbale flag from poller is :%d ", m_externalCreditToGameNVRAMOffset, m_ExternalCreditToGameEnabled);
	nvramConfig.Get(m_mgmdEnabledNVRAMOffset, (BYTE *)&m_mgmdEnabled, sizeof(m_mgmdEnabled));
	nvramConfig.Get(m_multiGameMultiDenomNVRAMOffset, (BYTE *)&m_multiGameMultiDenom, sizeof(m_multiGameMultiDenom));
	nvramConfig.Get(m_globalMGMDOffset, (BYTE *)&m_globalMGMD, sizeof(m_globalMGMD));
	nvramConfig.Get(m_gameStartExceptionsOffset, (BYTE *)&m_gameStartExceptions, sizeof(m_gameStartExceptions));
	nvramConfig.Get(m_gameEndExceptionsOffset, (BYTE *)&m_gameEndExceptions, sizeof(m_gameEndExceptions));
	nvramConfig.Get(m_mobileConnectCommunicationEnabledNVRAMOffset, (BYTE *)&m_mobileConnectCommEnabled, sizeof(m_mobileConnectCommEnabled)); /* flag not relevant at the moment */
	nvramConfig.Get(m_bonusCounterNotificationPriorityOffset, (BYTE *)&m_bonusCounterNotificationPriority, sizeof(m_bonusCounterNotificationPriority));
	nvramConfig.Get(m_bonusCounternRichLevelPriorityOffset, (BYTE *)&m_bonusCounternRichLevelPriority, sizeof(m_bonusCounternRichLevelPriority));
	nvramConfig.Get(m_bonusCounterSplashdownPriorityOffset, (BYTE *)&m_bonusCounterSplashdownPriority, sizeof(m_bonusCounterSplashdownPriority));
	nvramConfig.Get(m_bonusCounterProgressiveOdometerPriorityOffset, (BYTE *)&m_bonusCounterProgressiveOdometerPriority, sizeof(m_bonusCounterProgressiveOdometerPriority));
	nvramConfig.Get(m_bonusCounterPtePriorityOffset, (BYTE *)&m_bonusCounterPtePriority, sizeof(m_bonusCounterPtePriority));
	nvramConfig.Get(m_bonusCounterScatterCountdownTimerPriorityOffset, (BYTE *)&m_bonusCounterScatterCountdownTimerPriority, sizeof(m_bonusCounterScatterCountdownTimerPriority));
	nvramConfig.Get(m_bonusCounterPrizePriorityOffset, (BYTE *)&m_bonusCounterPrizePriority, sizeof(m_bonusCounterPrizePriority));
	nvramConfig.Get(m_globalExcessiveVoucherOutEnabledFlagOffset, (BYTE *)&m_globalExcessiveVoucherOutEnabledFlag, sizeof(m_globalExcessiveVoucherOutEnabledFlag));
	nvramConfig.Get(m_excessiveVoucherOutThresholdOffset, (BYTE *)&m_excessiveVoucherOutThreshold, sizeof(m_excessiveVoucherOutThreshold));
	nvramConfig.Get(m_totalBillInThresholdOffset, (BYTE *)&m_totalBillInThreshold, sizeof(m_totalBillInThreshold));
	nvramConfig.Get(m_coinInThresholdOffset, (BYTE *)&m_coinInThreshold, sizeof(m_coinInThreshold));
	nvramConfig.Get(m_coinOutThresholdOffset, (BYTE *)&m_coinOutThreshold, sizeof(m_coinOutThreshold));
	nvramConfig.Get(m_bonusCounterPlayXGetYPriorityOffset, (BYTE *)&m_bonusCounterPlayXGetYPriority, sizeof(m_bonusCounterPlayXGetYPriority));
	nvramConfig.Get(m_remoteDebuggingEnabledNVRAMOffset, (BYTE *)&m_remoteDebuggingEnabled, sizeof(m_remoteDebuggingEnabled));
	nvramConfig.Get(m_partialWATOutEnabledNVRAMOffset, (BYTE *)&m_partialWATOutEnabled, sizeof(m_partialWATOutEnabled));
	nvramConfig.Get(m_partialWATOutEnabledGlobalNVRAMOffset, (BYTE *)&m_partialWATOutEnabled_global, sizeof(m_partialWATOutEnabled_global));
	nvramConfig.Get(m_autoUploadCreditsGlobalNVRAMOffset, (BYTE *)&m_autoUploadCreditsGlobal, sizeof(m_autoUploadCreditsGlobal));
	nvramConfig.Get(m_cashoutToHostEnabledNVRAMOffset, (BYTE *)&m_cashoutToHostEnabled, sizeof(m_cashoutToHostEnabled));
	nvramConfig.Get(m_cashoutToHostEnabledGlobalNVRAMOffset, (BYTE *)&m_cashoutToHostEnabled_global, sizeof(m_cashoutToHostEnabled_global));
	nvramConfig.Get(m_extensibleFlagsNVRAMOffset, (BYTE *)&m_extensibleFlags.m_wordValue, sizeof(m_extensibleFlags.m_wordValue));
	m_sasReadTimeOutMultiplier = m_extensibleFlags.GetSASReadTimeOutMultiplier();
	nvramConfig.Get(m_taxWithholdingEnabledOffsetNVRAMOffset, (BYTE *)&m_taxWithholdingEnabled, sizeof(m_taxWithholdingEnabled));
	nvramConfig.Get(m_federalTaxWithholdingPercentOffsetNVRAMOffset, (BYTE *)&m_federalTaxWithholdingPercent, sizeof(m_federalTaxWithholdingPercent));
	nvramConfig.Get(m_stateTaxWithholdingPercentOffsetNVRAMOffset, (BYTE *)&m_stateTaxWithholdingPercent, sizeof(m_stateTaxWithholdingPercent));
	nvramConfig.Get(m_numberOfSubgamesMismatchCheckOffset, (BYTE *)&m_numberOfSubgamesMismatchCheck, sizeof(m_numberOfSubgamesMismatchCheck));
	LogString(GAMES, "Config-BuildYourself m_numberOfSubgamesMismatchCheck=%u", m_numberOfSubgamesMismatchCheck);
	nvramConfig.Get(m_gameListChunkSizeOffset, (BYTE *)&m_gameListChunkSize, sizeof(m_gameListChunkSize));

	CPollerMGMDData::Instance().SetMGMDGameStart(byte(m_gameStartExceptions));
	CPollerMGMDData::Instance().SetMGMDGameEnd(byte(m_gameEndExceptions));
	CPollerMGMDData::Instance().SetMGMDGlobalOption(byte(m_globalMGMD));
	CPollerMGMDData::Instance().SetMGMDSenOption(byte(m_mgmdEnabled));

	GetConfigExt2();

	CUtilities::SetTzOffset( m_timezoneOffset );

	//finally check to see if there are additional values to add to NVRAM.
	//note: this won't work for rearranged NVRAM; only for additional entries that need initializing.
	CheckForNVRAMUpgrade();

	SetRestartValues();
}

string CConfig::GetSubscriptionDataResponse(const std::string& topic, const std::string& whereId)
{
	UNREFERENCED_PARAMETER(&whereId);

	CIPAddress myIPAddress, nudgeIPAddress, cacheAnnouncementIPAddress, subnetMask, defaultGateway, preferredDNSServer, alternateDNSServer;
	GetMyIP(myIPAddress);
	GetSubnetMask(subnetMask);
	GetDefaultGateway(defaultGateway);
	GetPreferredDNSServer(preferredDNSServer);
	GetAlternateDNSServer(alternateDNSServer);
	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);
	writer.StartObject();
	writer.Key("SlotNumber");					writer.Int(m_slotNumber);
	writer.Key("SlotMastID");					writer.Int(m_slotMastID);
	writer.Key("Host");							writer.String(m_hostAddressOrName);
	writer.Key("Port");							writer.Int(m_hostPort);
	writer.Key("MyIPAddress");					writer.String(myIPAddress.GetIP());
	writer.Key("ActualIPAddress");				writer.String(m_actualIPAddress.GetIP());
	writer.Key("DHCP");							writer.Bool(m_DHCPAuto);
	writer.Key("SubnetMask");					writer.String(subnetMask.GetIP());
	writer.Key("DefaultGateway");				writer.String(defaultGateway.GetIP());
	writer.Key("PreferredDNSServer");			writer.String(preferredDNSServer.GetIP());
	writer.Key("AlternateDNSServer");			writer.String(alternateDNSServer.GetIP());
	writer.Key("Domain");						writer.String(m_domain);
	writer.Key("MacAddress");					writer.String(m_strMACAddress);

	//Denominations
	string denomination(FormatString("$%s.%02d", CUtilities::GetFormattedNumberString(m_denomination / ONE_DOLLAR_IN_CENTS).c_str(),
		m_denomination % ONE_DOLLAR_IN_CENTS));
	string gameDenom(FormatString("$%s.%02d", CUtilities::GetFormattedNumberString(m_gameDenom / ONE_DOLLAR_IN_CENTS).c_str(),
		m_gameDenom % ONE_DOLLAR_IN_CENTS));
	writer.Key("MeterDenom");			writer.String(denomination);
	writer.Key("GameDenom");			writer.String(gameDenom);
	writer.Key("PersonalBankerDenom");	writer.String(denomination);
	
	writer.Key("BatteryStatus");		writer.String(GetBatteryStatusText());

	writer.Key("IsLcd");				writer.Bool(m_isLcd);

	writer.Key("FillAmountRequired");	writer.Bool(m_fillAmount);

	writer.Key("A8ResetToCredit");		writer.Bool(!m_ujpEnabled && m_jackpotA8Reset);

	writer.Key("OsVersion");			writer.String(CPlatform::Instance().GetOSImageVersionNumber());
	writer.Key("SentinelVersion");		writer.String(CUtilities::GetEXEVersion());
	writer.Key("SASVersion");			writer.String(m_sasVersion);	
	
	writer.Key("ImageChecksum");				writer.String(FormatString("%X", m_imageChecksum));

	writer.Key("CarrierFirmwareVersion");	writer.String(m_firmwareVersion);
	writer.Key("TouchFirmwareVersion");		writer.String(m_touchFirmwareVersion);
	writer.Key("TargetCarrierFirmwareVersion");	writer.String(m_targetFirmwareVersion);
	writer.Key("Backlight");			writer.Int(m_screenBrightness);
	writer.Key("Volume");				writer.Int(m_leftSpeakerVolume);
	writer.Key("ExternalCreditVerificationEnabled");	writer.Bool(ExternalCreditVerificationEnabled());
	
	writer.Key("SentinelOptions");
	{
		int index = 0;
		m_senOptions[index++] = (m_senOption1Raw & SO1_DISPLAY_PLAYER_NAME) == SO1_DISPLAY_PLAYER_NAME;
		m_senOptions[index++] = (m_senOption1Raw & SO1_DISPLAY_COMP) == SO1_DISPLAY_COMP;
		m_senOptions[index++] = (m_senOption1Raw & SO1_DISPLAY_NUMERIC_POINT_TOTOAL) == SO1_DISPLAY_NUMERIC_POINT_TOTOAL;
		m_senOptions[index++] = (m_senOption1Raw & SO1_DISPLAY_POINTS_TOTAL_AS_CASH) == SO1_DISPLAY_POINTS_TOTAL_AS_CASH;
		m_senOptions[index++] = (m_senOption1Raw & SO1_DISPLAY_PLAYER_POINTS_TOTAL) == SO1_DISPLAY_PLAYER_POINTS_TOTAL;
		m_senOptions[index++] = (m_senOption1Raw & SO1_DISPLAY_COUNTDOWN_AND_SESSION) == SO1_DISPLAY_COUNTDOWN_AND_SESSION;
		m_senOptions[index++] = (m_senOption2Raw & SO2_DISPLAY_GOOD_LUCK_AND_SESSION_POINTS) == SO2_DISPLAY_GOOD_LUCK_AND_SESSION_POINTS;
		m_senOptions[index++] = (m_senOption1Raw & SO1_DISPLAY_GOOD_LUCK_AND_SESSION) == SO1_DISPLAY_GOOD_LUCK_AND_SESSION;
		m_senOptions[index++] = (m_senOption2Raw & SO2_DISPLAY_TOTAL_AND_SESSION_POINTS_NUM) == SO2_DISPLAY_TOTAL_AND_SESSION_POINTS_NUM;
		m_senOptions[index++] = (m_senOption1Raw & SO1_DISPLAY_TOTAL_POINTS_SESSION_POINTS_AS_CASH) == SO1_DISPLAY_TOTAL_POINTS_SESSION_POINTS_AS_CASH;
		m_senOptions[index++] = (m_senOption2Raw & SO2_DISPLAY_POINTS_TOTAL_AS_CASH_SESSION) == SO2_DISPLAY_POINTS_TOTAL_AS_CASH_SESSION;
		m_senOptions[index++] = (m_senOption1Raw & SO1_PARTIAL_BONUS_AWARD) == SO1_PARTIAL_BONUS_AWARD;
		m_senOptions[index++] = (m_senOption1Raw & SO1_DISPLAY_CONGRATS) == SO1_DISPLAY_CONGRATS;
		m_senOptions[index++] = (m_senOption1Raw & SO1_VIRTUAL_SESSION_ENABLED) == SO1_VIRTUAL_SESSION_ENABLED;
		m_senOptions[index++] = (m_senOption2Raw & SO2_VOUCHER_OUT_ENABLED) == SO2_VOUCHER_OUT_ENABLED;
		m_senOptions[index++] = (m_senOption1Raw & SO1_TICKET_IN_ENABLED) == SO1_TICKET_IN_ENABLED;
		m_senOptions[index++] = (m_senOption1Raw & SO1_VOUCHER_PROMO_IN_ENABLED) == SO1_VOUCHER_PROMO_IN_ENABLED;
		m_senOptions[index++] = (m_senOption1Raw & SO1_COUPON_PROMO_IN_ENABLED) == SO1_COUPON_PROMO_IN_ENABLED;
		m_senOptions[index++] = (m_senOption1Raw & SO1_PBT_CASH_ENABLE) == SO1_PBT_CASH_ENABLE;
		m_senOptions[index++] = (m_senOption1Raw & SO1_PBT_POINSTS_ENABLE) == SO1_PBT_POINSTS_ENABLE;
		m_senOptions[index++] = (m_senOption1Raw & SO1_PBT_POINTS_AS_PROMO) == SO1_PBT_POINTS_AS_PROMO;
		m_senOptions[index++] = (m_senOption1Raw & SO1_PBT_PROMO_ENABLE) == SO1_PBT_PROMO_ENABLE;
		m_senOptions[index++] = (m_senOption1Raw & SO1_PB_FROM_GAME_ENABLED) == SO1_PB_FROM_GAME_ENABLED;
		m_senOptions[index++] = CEPOutEnabled();
		m_senOptions[index++] = (m_senOption2Raw & SO2_NCEP_OUT_ENABLED) == SO2_NCEP_OUT_ENABLED;
		m_senOptions[index++] = (m_senOption1Raw & SO1_FILL_AMOUNT) == SO1_FILL_AMOUNT;
		m_senOptions[index++] = (m_senOption1Raw & SO1_USE_HOPPER) == SO1_USE_HOPPER;
		m_senOptions[index++] = NcepAutoUpload();
		m_senOptions[index++] = CepAutoUpload();
		m_senOptions[index++] = WatAutoUpload();
		m_senOptions[index++] = (m_senOption2Raw & SO2_ACCRUE_POINTS_FOR_NCEP) == SO2_ACCRUE_POINTS_FOR_NCEP;
		m_senOptions[index++] = (m_senOption2Raw & SO2_ACCRUE_COMP_FOR_NCEP) == SO2_ACCRUE_COMP_FOR_NCEP;
		m_senOptions[index++] = (m_senOption2Raw & SO2_UJP_ENABLED) == SO2_UJP_ENABLED;
		m_senOptions[index++] = (m_senOption2Raw & SO2_MINI_VIRTUAL_SESSION_ENABLED) == SO2_MINI_VIRTUAL_SESSION_ENABLED;
		m_senOptions[index++] = (m_senOption2Raw & SO2_ONE_LINK_PROGRESSIVES_ENABLED) == SO2_ONE_LINK_PROGRESSIVES_ENABLED;
		m_senOptions[index++] = (m_senOption2Raw & SO2_MGMD_ENABLED) == SO2_MGMD_ENABLED;
		m_senOptions[index++] = (m_senOption2Raw & SO2_PROBE_PBT_LOCK) == SO2_PROBE_PBT_LOCK;
		m_senOptions[index++] = (m_senOption2Raw & SO2_MOBILE_CONNECT_COMMUNICATION_ENABLED) == SO2_MOBILE_CONNECT_COMMUNICATION_ENABLED;
		m_senOptions[index++] = (m_senOption2Raw & SO2_REMOTE_DEBUGGING_ENABLED) == SO2_REMOTE_DEBUGGING_ENABLED;
		m_senOptions[index++] = (m_senOption2Raw & SO2_PB_PARTIAL_WAT_OUT_ENABLED) == SO2_PB_PARTIAL_WAT_OUT_ENABLED;
		m_senOptions[index++] = (m_senOption2Raw & SO2_CASHOUT_TO_HOST_ENABLED) == SO2_CASHOUT_TO_HOST_ENABLED;
		m_senOptions[index++] = (m_senOption2Raw & SO2_EXTERNAL_CREDIT_TRANSFER_VERIFICATION) == SO2_EXTERNAL_CREDIT_TRANSFER_VERIFICATION;

		writer.StartArray();
		for (index = 0; index < NUM_SEN_OPTIONS; ++index)
		{
			writer.StartObject();
			writer.Key("Label");	writer.String(TXT_SEN_OPTIONS[index]);
			writer.Key("Value");	writer.String(m_senOptions[index] ? "Yes" : "No");
			writer.EndObject();
		}
		writer.EndArray();
	}
			
	writer.Key("GlobalOptions");
	{
		int index = 0;
		m_globalOptions[index++] = TicketingEnabled();
		m_globalOptions[index++] = SystemGeneratedTicketValidation();
		m_globalOptions[index++] = TicketBufferingEnabled();
		m_globalOptions[index++] = PBTPromoEnabledGlobal();
		m_globalOptions[index++] = PBTPointsEnabledGlobal();
		m_globalOptions[index++] = PBTCashEnabledGlobal();
		m_globalOptions[index++] = GetCashTransferTracking();
		m_globalOptions[index++] = GetAccountingType() == ACCOUNTING_COINS_IN;
		m_globalOptions[index++] = GetAccountingType() == ACCOUNTING_COINS_OUT;
		m_globalOptions[index++] = GetAccountingType() == ACCOUNTING_THEORETICAL;
		m_globalOptions[index++] = EmployeePinRequired();
		m_globalOptions[index++] = RcclCardsEnabled();
		m_globalOptions[index++] = GetGlobalAccruePointsForNCEP();
		m_globalOptions[index++] = GetGlobalAccrueCompForNCEP();
		m_globalOptions[index++] = GetGlobalMGMDEnabled();
		m_globalOptions[index++] = GetGlobalExcessiveVoucherOutEnabledFlag();
		m_globalOptions[index++] = PartialWATOutEnabled_Global();
		m_globalOptions[index++] = AutoUploadCreditsGlobal();
		m_globalOptions[index++] = CashoutToHostEnabled_Global();
		m_globalOptions[index++] = UJPTaxWithholding();

		writer.StartArray();
		for (index = 0; index < NUM_GLOBAL_OPTIONS; ++index)
		{
			writer.StartObject();
			writer.Key("Label");	writer.String(TXT_GLOBAL_OPTIONS[index]);
			writer.Key("Value");	writer.String(m_globalOptions[index] ? "Yes" : "No");
			writer.EndObject();
		}
		writer.EndArray();
	}

	if (NO_EXTENSIBLE_SETTINGS_TOPIC != topic)
	{
		writer.Key("AutoCardOutCountdownSeconds");			writer.Int(m_autoCardOutCountdownSeconds);
		writer.Key("AutoCardOutSecToAlert");				writer.Int(m_autoCardOutSecToAlert);
		writer.Key("nConnectUrl");							writer.String(m_nConnectUrl);
		writer.Key("nConnectToken");						writer.String(m_nConnectToken);
		writer.Key("AvatarUrl");							writer.String(m_avatarUrl);
		writer.Key("AvatarToken");							writer.String(m_avatarToken);
		writer.Key("RequestServiceUrl");					writer.String(m_requestServiceUrl);
		writer.Key("RequestServiceToken");					writer.String(m_requestServiceToken);
		writer.Key("DrinksOnTapUrl");						writer.String(m_drinksOnTapUrl);
		writer.Key("DrinksOnTapToken");						writer.String(m_drinksOnTapToken);
		writer.Key("SocialDistanceUrl");					writer.String(m_socialDistanceUrl);
		writer.Key("SocialDistanceToken");					writer.String(m_socialDistanceToken);
		writer.Key("SocialDistanceIgnoreDirty");			writer.Bool(m_socialDistanceIgnoreDirty);
		writer.Key("SocialDistanceManualCleanEnabled");		writer.Bool(m_socialDistanceManualCleanEnabled);
		writer.Key("SocialDistanceCountdownDelay");			writer.Uint64(m_socialDistanceCountdownDelay);
	}

	writer.Key("RemoteUIEnabled");				writer.Bool(CUtilities::DoesFileExist(CDirectoryManager::REMOTE_UI_FILE));
	writer.Key("isDeviceMediaWindow");			writer.Bool(CDisplayManager::Instance().GetDisplayPlatform() == VideoMixerPlatform);

	writer.EndObject();

	return writeStream.m_str;
}

void CConfig::InitializeMACAddress()
{
	static const char *matchName("eth0"); // Match this interface name.

	// Try to get a list of network interface addresses.
	struct ifaddrs *ifaList(nullptr);
	if (0 != getifaddrs(&ifaList))
	{
		LogString(ERROR_LOG, "InitializeMACAddress: getifaddrs() failed errno=%d", (int)errno);
	}
	else
	{
		//find the appropriate interface
		for (struct ifaddrs *ifa(ifaList); ifa != nullptr; ifa = ifa->ifa_next)
		{
			// If there are multiple network interface adapters or the desired adapter name is unknown then ...
			// We only use the MAC address to identify to the poller, so it does not have to be from
			// the same adapter that is connected. In fact, it is better if we always return the same
			// address regardless. We choose the highest to ensure this and to make sure it is not 0.0.0.0

			// Check for a record that matches the desired interface name.
			if (ifa->ifa_name != nullptr && 0 == strcmp(matchName, ifa->ifa_name))
			{
				// Check address family for an internet (IPv4) record.
				if (AF_INET == ifa->ifa_addr->sa_family)
				{
					// If we did not already match on the network adapter name, we should exclude records with a (loopback) netmask of "255.0.0.0".

					// Get IP address as a string.
					struct sockaddr_in *inetRec((struct sockaddr_in*)ifa->ifa_addr);
					if (nullptr == inetRec)
					{
						LogString(WARNING_LOG, "Warning: \"%s\" internet record is null",
							static_cast<LPCSTR>(ifa->ifa_name));
					}
					else
					{
						char ipAddress[INET_ADDRSTRLEN];
						if (nullptr == inet_ntop(AF_INET, &inetRec->sin_addr, ipAddress, _countof(ipAddress)))
						{
							LogString(ERROR_LOG, "InitializeMACAddress: inet_ntop(inetRec) failed errno=%d", (int)errno);
						}
						else
						{
							m_actualIPAddress.SetIP(ipAddress);
							m_strNetworkAdaptername = ifa->ifa_name;
//							LogString(GENERAL, "NetworkAdapterName=%s IPAddress=%s",
//								static_cast<LPCSTR>(m_strNetworkAdaptername.c_str()), static_cast<LPCSTR>(ipAddress));
						}
					}
				}
				// Check address family for a low-level physical-layer packet record.
				else if (AF_PACKET == ifa->ifa_addr->sa_family)
				{
					struct sockaddr_ll *lowlevelRec((struct sockaddr_ll *)ifa->ifa_addr);
					if (nullptr == lowlevelRec)
					{
						LogString(WARNING_LOG, "Warning: \"%s\" low-level record is null",
							static_cast<LPCSTR>(ifa->ifa_name));
					}
					else
					{
						if (lowlevelRec->sll_halen != SIZE_OF_MAC_ADDRESS)
						{
							LogString(WARNING_LOG, "Warning: \"%s\" physical-layer address length=%u expected=%u",
								static_cast<LPCSTR>(ifa->ifa_name), (unsigned)lowlevelRec->sll_halen, (unsigned)SIZE_OF_MAC_ADDRESS);
						}
						const unsigned char *mac(lowlevelRec->sll_addr);
						memcpy(m_macAddress, mac, sizeof(m_macAddress));

						char macAddress[SIZE_OF_MAC_ADDRESS * 3];
						snprintf(macAddress, _countof(macAddress), "%02X-%02X-%02X-%02X-%02X-%02X", *mac, *(mac + 1), *(mac + 2), *(mac + 3), *(mac + 4), *(mac + 5));
						m_strMACAddress = macAddress;
//						LogString(GENERAL, "NetworkAdapterName=%s MACAddress=%s",
//							static_cast<LPCSTR>(ifa->ifa_name), static_cast<LPCSTR>(macAddress));
					}
				}
			}
		}
	}

	if (ifaList != nullptr)
	{
		freeifaddrs(ifaList);
	}
}

CIPAddress CConfig::GetActualIP()
{
	//this gets us the ip address too
	InitializeMACAddress();

	return m_actualIPAddress;
}

const std::string &CConfig::GetNetworkAdapterName() const
{
	return m_strNetworkAdaptername;
}

const std::string &CConfig::GetMACAddressAsString() const
{
	return m_strMACAddress;
}

const std::string &CConfig::GetMACAddress2AsString() const
{
	return m_strMACAddress2;
}

const BYTE * CConfig::GetMacAddressAsBytes() const
{
	return m_macAddress;
}

const BYTE * CConfig::GetMacAddress2AsBytes() const
{
	return m_macAddress2;
}

/// <summary>
/// Sets the EGM lockout state.
/// </summary>
/// <param name="EGMLockoutState">true if EGM needs to be locked, false otherwise.</param>
bool CConfig::SetEGMLockoutState(bool EGMLockoutState)
{
	bool bOldVal(m_EGMLockoutState);

	CNVRAMConfig nvramConfig;
	m_EGMLockoutState = EGMLockoutState; 

	nvramConfig.Set(m_EGMLockoutStateNVRAMOffset,
		(BYTE *) &m_EGMLockoutState,
		sizeof(m_EGMLockoutState));

	return bOldVal;
}

/// <summary>
/// Returns the EGM lockout state.
/// </summary>
/// <returns>true if EGM needs to be locked, false otherwise.</returns>
bool CConfig::GetEGMLockoutState()
{
	return m_EGMLockoutState;
}


bool CConfig::SetMemoryLost(bool memoryLost)
{
	bool bOldVal = m_memoryLost;

	m_memoryLost = memoryLost;
	m_nvramConfig.Set(m_memoryLostNVRAMOffset, (BYTE*) &m_memoryLost, sizeof(m_memoryLost));

	return bOldVal;
}	

bool CConfig::GetMemoryLost()
{
	return (m_memoryLost);
}

WORD CConfig::SetSlotMastRevision(WORD slotMastRevision)
{
	WORD bOldVal = m_slotMastRevision;

	m_slotMastRevision = slotMastRevision;
	m_nvramConfig.Set(m_slotMastRevisionNVRAMOffset, (BYTE*) &m_slotMastRevision, sizeof(m_slotMastRevision));

	return bOldVal;
}	

WORD CConfig::GetSlotMastRevision()
{
	return m_slotMastRevision;
}

bool CConfig::SetLossLimitActive(bool lossLimitActive)
{
	bool bOldVal = m_bLossLimitActive;

	m_bLossLimitActive = lossLimitActive;
	m_nvramConfig.Set(m_bLossLimitActiveNVRAMOffset, (BYTE *)&m_bLossLimitActive, sizeof(m_bLossLimitActive));

	return bOldVal;
}

bool CConfig::GetLossLimitActive() const
{
	return (m_bLossLimitActive);
}


WORD CConfig::SetLossLimitSessionAmount(WORD lossLimitSessionAmount)
{
	WORD bOldVal = m_lossLimitSessionAmount;

	m_lossLimitSessionAmount = lossLimitSessionAmount; 
	m_nvramConfig.Set(m_lossLimitSessionAmountNVRAMOffset, (BYTE *)&m_lossLimitSessionAmount, sizeof(m_lossLimitSessionAmount));

	return bOldVal;
}	

WORD CConfig::GetLossLimitSessionAmount()
{
	return (m_lossLimitSessionAmount);
}


WORD CConfig::SetLossLimitBillValidatorOffAmount(WORD lossLimitBillValidatorOffAmount)
{
	WORD bOldVal = m_lossLimitBillValidatorOffAmount;

	m_lossLimitBillValidatorOffAmount = lossLimitBillValidatorOffAmount;
	m_nvramConfig.Set(m_lossLimitBillValidatorOffAmountNVRAMOffset, (BYTE *)&m_lossLimitBillValidatorOffAmount, sizeof(m_lossLimitBillValidatorOffAmount));

	return bOldVal;
}

int CConfig::SetLossLimitSessionNumber(int lossLimitSessionNumber)
{
	int bOldVal = m_lossLimitSessionNumber;

	m_lossLimitSessionNumber = lossLimitSessionNumber;
	m_nvramConfig.Set(m_lossLimitSessionNumberNVRAMOffset, (BYTE *)&m_lossLimitSessionNumber, sizeof(m_lossLimitSessionNumber));

	return bOldVal;
}

int CConfig::GetLossLimitSessionNumber()
{
	return (m_lossLimitSessionNumber);
}

WORD CConfig::GetLossLimitBillValidatorOffAmount()
{
	return (m_lossLimitBillValidatorOffAmount);
}

int CConfig::SetLastCoinsPlayed(int lastCoinsPlayed)
{
	int bOldVal = m_lastCoinsPlayed;

	m_lastCoinsPlayed = lastCoinsPlayed;
	m_nvramConfig.Set(m_lastCoinsPlayedNVRAMOffset, (BYTE*) &m_lastCoinsPlayed, sizeof(m_lastCoinsPlayed));

	return bOldVal;
}

int CConfig::GetLastCoinsPlayed() const
{
	return (m_lastCoinsPlayed);
}

int CConfig::SetLastPBTransactionID(int lastPBTransactionID)
{
	int bOldVal = m_lastPBTransactionID;

	m_lastPBTransactionID = lastPBTransactionID;
	m_nvramConfig.Set(m_lastPBTransactionIDNVRAMOffset, (BYTE*) &m_lastPBTransactionID, sizeof(m_lastPBTransactionID));

	return bOldVal;
}

int CConfig::GetLastPBTransactionID()
{
	return (m_lastPBTransactionID);
}

bool CConfig::SetPBResponsePending(bool PBResponsePending)
{
	bool bOldVal = m_PBResponsePending;

	m_PBResponsePending = PBResponsePending;
	m_nvramConfig.Set(m_PBResponsePendingNVRAMOffset, (BYTE*) &m_PBResponsePending, sizeof(m_PBResponsePending));

	return bOldVal;
}

bool CConfig::GetPBResponsePending()
{
	return (m_PBResponsePending);
}

bool CConfig::SetPBTransferInProgress(bool PBTransferInProgress)
{
	bool bOldVal = m_PBTransferInProgress;

	m_PBTransferInProgress = PBTransferInProgress;
	m_nvramConfig.Set(m_PBTransferInProgressNVRAMOffset, (BYTE*) &m_PBTransferInProgress, sizeof(m_PBTransferInProgress));

	return bOldVal;
}

bool CConfig::GetPBTransferInProgress()
{
	return (m_PBTransferInProgress);
}

int CConfig::SetCurrentPBTransactionID(int currentPBTransactionID)
{
	int bOldVal = m_currentPBTransactionID;

	m_currentPBTransactionID = currentPBTransactionID;
	m_nvramConfig.Set(m_currentPBTransactionIDNVRAMOffset, (BYTE*) &m_currentPBTransactionID, sizeof(m_currentPBTransactionID));

	return bOldVal;
}

int CConfig::GetCurrentPBTransactionID()
{
	return(m_currentPBTransactionID);
}

int CConfig::SetPBTransferSequenceNumber(int PBTransferSequenceNumber)
{
	int bOldVal = m_PBTransferSequenceNumber;

	m_PBTransferSequenceNumber = PBTransferSequenceNumber;
	m_nvramConfig.Set(m_PBTransferSequenceNumberNVRAMOffset, (BYTE*) &m_PBTransferSequenceNumber, sizeof(m_PBTransferSequenceNumber));

	return bOldVal;
}

int CConfig::GetPBTransferSequenceNumber()
{
	return (m_PBTransferSequenceNumber);
}


int CConfig::SetPBCreditSource(int PBCreditSource)
{
	int bOldVal = m_PBCreditSource;

	m_PBCreditSource = PBCreditSource;
	m_nvramConfig.Set(m_PBCreditSourceNVRAMOffset, (BYTE*) &m_PBCreditSource, sizeof(m_PBCreditSource));

	return bOldVal;
}

int CConfig::GetPBCreditSource()
{
	return (m_PBCreditSource);
}

DWORD CConfig::SetPBTransferAmount(DWORD PBTransferAmount)
{
	DWORD bOldVal = m_PBTransferAmount;

	m_PBTransferAmount = PBTransferAmount;
	m_nvramConfig.Set(m_PBTransferAmountNVRAMOffset, (BYTE*) &m_PBTransferAmount, sizeof(m_PBTransferAmount));

	return bOldVal;
}

DWORD CConfig::GetPBTransferAmount()
{
	return (m_PBTransferAmount);
}

void CConfig::SetPBLog()
{
	//	wcsncpy(m_PBLog, PBLog, SIZE_OF_PB_LOGS);//TBD write to NVRAM
}

LPCTSTR CConfig::GetPBLog()
{
	return NULL; //TBD.(m_PBLog);
}

void CConfig::SetPBValidLog()
{
	//	wcsncpy(m_PBValidLog, PBValidLog, SIZE_OF_PB_LOGS);//TBD write to NVRAM
}



LPCTSTR CConfig::GetPBValidLog()
{
	return NULL; //TBD. (m_PBValidLog);
}

bool CConfig::SetPBReleaseNeeded(bool PBReleaseNeeded)
{
	bool bOldVal = m_PBReleaseNeeded;

	m_PBReleaseNeeded = PBReleaseNeeded;
	m_nvramConfig.Set(m_PBReleaseNeededNVRAMOffset, (BYTE*) &m_PBReleaseNeeded, sizeof(m_PBReleaseNeeded));

	return bOldVal;
}

bool CConfig::GetPBReleaseNeeded()
{
	return (m_PBReleaseNeeded);
}

int CConfig::SetEmployeeAuthorizationOCR(int employeeAuthorizationOCR)
{
	int bOldVal = m_employeeAuthorizationOCR;

	m_employeeAuthorizationOCR = employeeAuthorizationOCR;
	m_nvramConfig.Set(m_employeeAuthorizationOCRNVRAMOffset, (BYTE*) &m_employeeAuthorizationOCR, sizeof(m_employeeAuthorizationOCR));

	return bOldVal;
}

int CConfig::GetEmployeeAuthorizationOCR()
{
	return (m_employeeAuthorizationOCR);
}

int CConfig::SetSlotMastID(int slotMastID)
{
	int bOldVal = m_slotMastID;

	if(m_slotMastID != slotMastID)
	{
		m_slotMastID = slotMastID;

		m_eepromConfig.Set(EEPROM_SLOTMAST_ID_OFFSET, (BYTE *)&m_slotMastID, EEPROM_SLOTMAST_ID_SIZE);
	}

	return bOldVal;
}

int CConfig::GetSlotMastID()
{
	return (m_slotMastID);
}

byte CConfig::SetCasinoNumber(byte casinoNumber)
{
	byte bOldVal = m_casinoNumber;

	if(m_casinoNumber != casinoNumber)
	{
		m_casinoNumber = casinoNumber;

		m_eepromConfig.Set(EEPROM_CASINO_NUMBER_OFFSET, (BYTE *)&m_casinoNumber, EEPROM_CASINO_NUMBER_SIZE);
	}

	return bOldVal;
}

byte CConfig::GetCasinoNumber() const
{
	return (m_casinoNumber);
}

byte CConfig::SetCommID(byte commID)
{
	byte bOldVal = m_commID;

	if(m_commID != commID)
	{
		m_commID = commID;

		m_eepromConfig.Set(EEPROM_COMM_ID_OFFSET, (BYTE *)&m_commID, EEPROM_COMM_ID_SIZE);
	}

	return bOldVal;
}

byte CConfig::GetCommID() const
{
	return (m_commID);
}

byte CConfig::SetMachineType(byte machineType)
{
	byte bOldVal = m_machineType;

	if(m_machineType != machineType)
	{
		m_machineType = machineType;

		m_eepromConfig.Set(EEPROM_MACHINE_TYPE_OFFSET, (BYTE *)&m_machineType, EEPROM_MACHINE_TYPE_SIZE );
	}

	return bOldVal;
}

byte CConfig::GetMachineType()
{
	return (m_machineType);
}

int CConfig::SetSlotNumber(int slotNumber)
{
	int bOldVal = m_slotNumber;

	if(m_slotNumber != slotNumber)
	{
		m_slotNumber = slotNumber;

		m_eepromConfig.Set(EEPROM_SLOT_NUMBER_OFFSET, (BYTE *) &m_slotNumber, EEPROM_SLOT_NUMBER_SIZE);
	}

	return bOldVal;
}

int CConfig::GetSlotNumber() const
{
	return (m_slotNumber);
}

WORD CConfig::SetDenomination( WORD denom )
{
	WORD bOldVal = m_denomination;

	m_denomination = denom;
	m_nvramConfig.Set(m_denominationNVRAMOffset, (BYTE*) &m_denomination, sizeof(m_denomination));
	m_receivedBadDenom = false;

	return bOldVal;
}

WORD CConfig::GetDenomination( void ) const
{
	return ( m_denomination );
}

byte CConfig::SetScreenBrightness(byte screenBrightness)
{
	byte bOldVal = m_screenBrightness;

	if(m_screenBrightness != screenBrightness)
	{
		m_screenBrightness = screenBrightness;

		m_eepromConfig.Set(EEPROM_SCREEN_BRIGHTNESS_OFFSET, (BYTE *)&m_screenBrightness, EEPROM_SCREEN_BRIGHTNESS_SIZE);
	}

	return bOldVal;
}

byte CConfig::GetScreenBrightness()
{
	return (m_screenBrightness);
}

void CConfig::SetSpeakerVolume( byte leftSpeakerVolume, byte rightSpeakerVolume )
{
	m_leftSpeakerVolume = leftSpeakerVolume;
	m_rightSpeakerVolume = rightSpeakerVolume;

	m_eepromConfig.Set(EEPROM_LEFT_SPEAKER_VOLUME_OFFSET, (BYTE *)&m_leftSpeakerVolume, EEPROM_LEFT_SPEAKER_VOLUME_SIZE );
	m_eepromConfig.Set(EEPROM_RIGHT_SPEAKER_VOLUME_OFFSET, (BYTE *)&m_rightSpeakerVolume, EEPROM_RIGHT_SPEAKER_VOLUME_SIZE );
}

byte CConfig::GetLeftSpeakerVolume()
{
	return (m_leftSpeakerVolume);
}

byte CConfig::GetRightSpeakerVolume()
{
	return (m_rightSpeakerVolume);
}

WORD CConfig::SetGlobalCardPassword(WORD globalCardPassword)
{
	WORD bOldVal = m_globalCardPassword;

	if(m_globalCardPassword != globalCardPassword)
	{
		m_globalCardPassword = globalCardPassword;
		m_eepromConfig.Set(EEPROM_GLOBAL_CARD_PASS_OFFSET, (BYTE *)&m_globalCardPassword, EEPROM_GLOBAL_CARD_PASS_SIZE);
	}

	return bOldVal;
}

std::string CConfig::GetGlobalCardPassword() const
{
	string retStr;

	if ( m_globalCardPassword != 0 )
	{
		retStr = std::to_string(m_globalCardPassword);
	}

	return (retStr);
}

int CConfig::SetPointsPerDollar(int pointsPerDollar)
{
	int bOldVal = m_pointsPerDollar;

	m_pointsPerDollar = pointsPerDollar;

	m_nvramConfig.Set(m_pointsPerDollarNVRAMOffset, (BYTE *)&m_pointsPerDollar, sizeof(m_pointsPerDollar));

	return bOldVal;
}

int CConfig::GetPointsPerDollar() const
{
	return (m_pointsPerDollar);
}

bool CConfig::SetHostAddressOrName(const string& hostAddressOrName)
{
	bool retVal = false;
	if(strcmp(hostAddressOrName.c_str(), m_hostAddressOrName) != 0)
	{
		retVal = true;
		memset(m_hostAddressOrName, 0, SIZE_OF_NAME_IP + 1);
		strncpy(m_hostAddressOrName, hostAddressOrName.c_str(), SIZE_OF_NAME_IP);
		m_eepromConfig.Set(EEPROM_HOST_ADDRESS_OFFSET, (BYTE *)m_hostAddressOrName, SIZE_OF_NAME_IP);
	}
	return retVal;
}

char* CConfig::GetHostAddressOrName()
{
	return (m_hostAddressOrName);
}

int CConfig::SetHostPort( int port )
{
	int bOldVal = m_hostPort;

	if(m_hostPort != port)
	{
		m_hostPort = port;

		m_eepromConfig.Set(EEPROM_HOST_PORT_OFFSET, (BYTE *)&m_hostPort, EEPROM_HOST_PORT_SIZE );
	}

	return bOldVal;
}

int CConfig::GetHostPort( void )
{
	return m_hostPort;
}

bool CConfig::SetDHCPConfig(bool DHCPAuto)
{
	bool bOldVal = m_DHCPAuto;

	if(m_DHCPAuto != DHCPAuto)
	{
		m_DHCPAuto = DHCPAuto;

		m_eepromConfig.Set( EEPROM_DHCPAUTO_OFFSET, (BYTE *)&m_DHCPAuto, EEPROM_DHCPAUTO_SIZE );
	}

	return bOldVal;
}

bool CConfig::GetDHCPConfig()
{
	return m_DHCPAuto;
}

void CConfig::SetMyIP(const string& myIP)
{
	CIPAddress ipAddress;
	ipAddress.SetIP(myIP);
	SetMyIP(ipAddress);
}

void CConfig::SetMyIP(CIPAddress const &ipAddress)
{
	byte temp[IP_ADDRESS_SIZE];

	ipAddress.GetIP( temp, IP_ADDRESS_SIZE );

	if(memcmp(temp, m_myIP, IP_ADDRESS_SIZE) != 0)
	{
		ipAddress.GetIP( m_myIP, IP_ADDRESS_SIZE );
		m_eepromConfig.Set( EEPROM_MY_IP_OFFSET, (BYTE *)m_myIP, EEPROM_MY_IP_SIZE );
	}
}

void CConfig::GetMyIP(CIPAddress &ipAddress)
{
	ipAddress.SetIP( m_myIP );
}

void CConfig::SetDefaultGateway(const std::string& defaultGateway)
{
	CIPAddress ipAddress;
	ipAddress.SetIP(defaultGateway);
	SetDefaultGateway(ipAddress);
}

void CConfig::SetDefaultGateway(CIPAddress const &ipAddress)
{
	byte temp[IP_ADDRESS_SIZE];

	ipAddress.GetIP( temp, IP_ADDRESS_SIZE );

	if(memcmp(temp, m_defaultGateway, IP_ADDRESS_SIZE) != 0)
	{
		ipAddress.GetIP( m_defaultGateway, IP_ADDRESS_SIZE );
		m_eepromConfig.Set( EEPROM_DEFAULT_GATEWAY_OFFSET, (BYTE *)m_defaultGateway, EEPROM_DEFAULT_GATEWAY_SIZE );
	}
}

void CConfig::GetDefaultGateway(CIPAddress &ipAddress)
{
	ipAddress.SetIP( m_defaultGateway );
}

void CConfig::SetSubnetMask(const std::string& subnetMask)
{
	CIPAddress ipAddress;
	ipAddress.SetIP(subnetMask);
	SetSubnetMask(ipAddress);
}

void CConfig::SetSubnetMask(CIPAddress const &ipAddress)
{
	byte temp[IP_ADDRESS_SIZE];

	ipAddress.GetIP( temp, IP_ADDRESS_SIZE );

	if(memcmp(temp, m_subnetMask, IP_ADDRESS_SIZE) != 0)
	{
		ipAddress.GetIP( m_subnetMask, IP_ADDRESS_SIZE );
		m_eepromConfig.Set( EEPROM_SUBNET_MASK_OFFSET, (BYTE *)m_subnetMask, EEPROM_SUBNET_MASK_SIZE );
	}
}

void CConfig::GetSubnetMask(CIPAddress &ipAddress)
{
	ipAddress.SetIP( m_subnetMask );
}

void CConfig::SetPreferredDNSServer(const string& preferredDnsServer)
{
	CIPAddress ipAddress;
	ipAddress.SetIP(preferredDnsServer);
	SetPreferredDNSServer(ipAddress);
}

void CConfig::SetPreferredDNSServer(CIPAddress const &ipAddress)
{
	byte temp[IP_ADDRESS_SIZE];

	ipAddress.GetIP( temp, IP_ADDRESS_SIZE );

	if(memcmp(temp, m_preferredDNSServer, IP_ADDRESS_SIZE) != 0)
	{
		ipAddress.GetIP( m_preferredDNSServer, IP_ADDRESS_SIZE );
		m_eepromConfig.Set( EEPROM_PREFERRED_DNS_OFFSET, (BYTE *)m_preferredDNSServer, EEPROM_PREFERRED_DNS_SIZE );
	}
}

void CConfig::GetPreferredDNSServer(CIPAddress &ipAddress)
{
	ipAddress.SetIP( m_preferredDNSServer );
}

void CConfig::SetAlternateDNSServer(const std::string& alternateDnsServer)
{
	CIPAddress ipAddress;
	ipAddress.SetIP(alternateDnsServer);
	SetAlternateDNSServer(ipAddress);
}

void CConfig::SetAlternateDNSServer(CIPAddress const &ipAddress)
{
	byte temp[IP_ADDRESS_SIZE];

	ipAddress.GetIP( temp, IP_ADDRESS_SIZE );

	if(memcmp(temp, m_alternateDNSServer, IP_ADDRESS_SIZE) != 0)
	{
		ipAddress.GetIP( m_alternateDNSServer, IP_ADDRESS_SIZE );
		m_eepromConfig.Set( EEPROM_ALT_DNS_OFFSET, (BYTE *)m_alternateDNSServer, EEPROM_ALT_DNS_SIZE );
	}
}

void CConfig::GetAlternateDNSServer(CIPAddress &ipAddress)
{
	ipAddress.SetIP( m_alternateDNSServer );
}

void CConfig::SetDomain(const string& domain)
{
	if (domain.empty())
	{
		m_domain = "NA";
	}
	else
	{
		m_domain = domain;
	}
}

string CConfig::GetDomain() const
{
	return m_domain;
}

WORD CConfig::SetGameDenom(WORD gameDenom)
{
	WORD bOldVal = m_gameDenom;

	if(m_gameDenom != gameDenom)
	{
		m_gameDenom = gameDenom;
		m_eepromConfig.Set( EEPROM_GAME_DENOM_OFFSET, (BYTE *)&m_gameDenom, EEPROM_GAME_DENOM_SIZE );
	}

	return bOldVal;
}

WORD CConfig::GetGameDenom() const
{
	return (m_gameDenom);
}

void CConfig::SetBatteryStatus( BatteryStatus batteryStatus )
{
	if(m_batteryStatus != batteryStatus)
	{
		m_batteryStatus = batteryStatus;
		m_eepromConfig.Set( EEPROM_BATTERY_STATUS_OFFSET, (BYTE *)&m_batteryStatus, EEPROM_BATTERY_STATUS_SIZE );
	}
}

BatteryStatus CConfig::GetBatteryStatus( void )
{
	return m_batteryStatus;
}

string CConfig::GetBatteryStatusText() const
{
	string text;

	switch (m_batteryStatus)
	{
	case batteryStatusUnknown:
		text = TXT_MECHANIC_BATTERY_UKNOWN;
		break;
	case batteryStatusOkay:
		text = TXT_MECHANIC_BATTERY_OKAY;
		break;
	case batteryStatusLow:
		text = TXT_MECHANIC_BATTERY_LOW;
		break;
	case batteryStatusNoBattery:
		text = TXT_MECHANIC_BATTERY_NO;
		break;
	}

	return text;
}

void CConfig::SetLastBatteryCheckTime( time_t const &checkTime )
{
	m_lastBatteryCheckTime = checkTime;
	byte timeBuffer[NVRAM_TIME_SIZE];
	CUtilities::GetNvramBufferFromTime( m_lastBatteryCheckTime, timeBuffer, NVRAM_TIME_SIZE );
	m_eepromConfig.Set(EEPROM_BATTERY_STATUS_TIME_OFFSET,	(BYTE *)timeBuffer,					EEPROM_BATTERY_STATUS_TIME_SIZE);
}

time_t CConfig::GetLastBatteryCheckTime( void )
{
	return m_lastBatteryCheckTime;
}

void CConfig::SetAttractorMessage(LPCTSTR attractorMessage)
{
	strncpy(m_attractorMessage, attractorMessage, SIZE_OF_MESSAGES - 1);
	m_nvramConfig.Set(m_attractorMessageNVRAMOffset, (BYTE*)m_attractorMessage, sizeof(m_attractorMessage));
}

LPCTSTR CConfig::GetAttractorMessage()
{
	return (m_attractorMessage);
}

void CConfig::SetGoodCardReadMessage(LPCTSTR goodCardReadMessage)
{
	strncpy(m_goodCardReadMessage, goodCardReadMessage, SIZE_OF_MESSAGES - 1);
	m_nvramConfig.Set(m_goodCardReadMessageNVRAMOffset, (BYTE*)m_goodCardReadMessage, sizeof(m_goodCardReadMessage));
}

/// <summary>
/// Returns a pointer to the good card read message.
/// </summary>
/// <returns>A pointer to the good card read message.</returns>
LPCTSTR CConfig::GetGoodCardReadMessage() const
{
	return (m_goodCardReadMessage);
}


void CConfig::SetBadCarReadMessage(LPCTSTR badCarReadMessage)
{
	strncpy(m_badCarReadMessage, badCarReadMessage, SIZE_OF_MESSAGES - 1);
	m_nvramConfig.Set(m_badCarReadMessageNVRAMOffset, (BYTE*)m_badCarReadMessage, sizeof(m_badCarReadMessage));
}

LPCTSTR CConfig::GetBadCarReadMessage()
{
	return (m_badCarReadMessage);
}


void CConfig::SetBirthdayMessage(LPCTSTR birthdayMessage)
{
	strncpy(m_birthdayMessage, birthdayMessage, SIZE_OF_MESSAGES - 1);
	m_nvramConfig.Set(m_birthdayMessageNVRAMOffset, (BYTE*)m_birthdayMessage, sizeof(m_birthdayMessage));
}

LPCTSTR CConfig::GetBirthdayMessage() const
{
	return(m_birthdayMessage);
}


void CConfig::SetAnniversaryMessage(LPCTSTR anniversaryMessage)
{
	strncpy(m_anniversaryMessage, anniversaryMessage, SIZE_OF_MESSAGES - 1);
	m_nvramConfig.Set(m_anniversaryMessageNVRAMOffset, (BYTE*)m_anniversaryMessage, sizeof(m_anniversaryMessage));
}

LPCTSTR CConfig::GetAnniversaryMessage() const
{
	return(m_anniversaryMessage);
}


void CConfig::SetCongratulationsMessage(LPCTSTR congratulationsMessage)
{
	strncpy(m_congratulationsMessage, congratulationsMessage, SIZE_OF_MESSAGES - 1);
	m_nvramConfig.Set(m_congratulationsMessageNVRAMOffset, (BYTE*)m_congratulationsMessage, sizeof(m_congratulationsMessage));
}

LPCTSTR CConfig::GetCongratulationsMessage() const
{
	return(m_congratulationsMessage);
}


void CConfig::SetWelcomeMessage(LPCTSTR welcomeMessage)
{
	strncpy(m_welcomeMessage, welcomeMessage, SIZE_OF_MESSAGES - 1);
	m_nvramConfig.Set(m_welcomeMessageNVRAMOffset, (BYTE*)m_welcomeMessage, sizeof(m_welcomeMessage));
}

LPCTSTR CConfig::GetWelcomeMessage() const
{
	return(m_welcomeMessage);
}

void CConfig::SetSessionPointsMessage(LPCTSTR message)
{
	strncpy(m_sessionPointsMessage, message, SIZE_OF_MESSAGES - 1);
	m_nvramConfig.Set(m_sessionPointsMessageNVRAMOffset, (BYTE*)m_sessionPointsMessage, sizeof(m_sessionPointsMessage));
}

LPCTSTR CConfig::GetSessionPointsMessage() const
{
	return(m_sessionPointsMessage);
}

void CConfig::SetBalanceRetrievalMessage(LPCTSTR message)
{
	strncpy(m_balanceRetrievalMessage, message, SIZE_OF_MESSAGES - 1);
	m_nvramConfig.Set(m_balanceRetrievalMessageNVRAMOffset, (BYTE*)m_balanceRetrievalMessage, sizeof(m_balanceRetrievalMessage));
}

LPCTSTR CConfig::GetBalanceRetrievalMessage() const
{
	return(m_balanceRetrievalMessage);
}

void CConfig::SetDailyPointsDisplaySuffixMessage(LPCTSTR dailyPointsDisplaySuffixMessage)
{
	strncpy(m_dailyPointsDisplaySuffixMessage, dailyPointsDisplaySuffixMessage, SIZE_OF_MESSAGES - 1);
	m_nvramConfig.Set(m_dailyPointsDisplaySuffixMessageNVRAMOffset, (BYTE*)m_dailyPointsDisplaySuffixMessage, sizeof(m_dailyPointsDisplaySuffixMessage));
}

LPCTSTR CConfig::GetDailyPointsDisplaySuffixMessage() const
{
	return m_dailyPointsDisplaySuffixMessage;
}

void CConfig::SetTierPointsLabel(LPCTSTR tierPointsLabel)
{
	strncpy(m_tierPointsLabel, tierPointsLabel, SIZE_OF_LABEL - 1);
	m_nvramConfig.Set(m_tierPointsLabelNVRAMOffset, (BYTE*)m_tierPointsLabel, sizeof(m_tierPointsLabel));
}

LPCTSTR CConfig::GetTierPointsLabel() const
{
	return m_tierPointsLabel;
}

void CConfig::SetPointsOrCashLabel(LPCTSTR pointsOrCashLabel)
{
	strncpy(m_pointsOrCashLabel, pointsOrCashLabel, SIZE_OF_LABEL - 1);
	m_nvramConfig.Set(m_pointsOrCashLabelNVRAMOffset, (BYTE*)m_pointsOrCashLabel, sizeof(m_pointsOrCashLabel));
}

LPCTSTR CConfig::GetPointsOrCashLabel() const
{
	return(m_pointsOrCashLabel);
}

bool CConfig::SetShowPointsVsAccountBalance(bool showPointsVsAccountBalance)
{
	bool bOldVal = m_showPointsVsAccountBalance;

	m_showPointsVsAccountBalance = showPointsVsAccountBalance;
	m_nvramConfig.Set(m_showPointsVsAccountBalanceNVRAMOffset, (BYTE *)&m_showPointsVsAccountBalance, sizeof(m_showPointsVsAccountBalance));

	return bOldVal;
}

bool CConfig::GetShowPointsVsAccountBalance() const
{
	return(m_showPointsVsAccountBalance);
}

void CConfig::SetCasinoName(LPCTSTR casinoName)
{
	m_strCasinoName = casinoName;
	char tempCasinoName[SIZE_OF_CASINO_NAME];
	CUtilities::ConvertCharsToChars( m_strCasinoName, tempCasinoName, SIZE_OF_CASINO_NAME);
	m_nvramConfig.Set(m_casinoNameNVRAMOffset, (BYTE *)tempCasinoName, sizeof(tempCasinoName));
}

const std::string &CConfig::GetCasinoName() const
{
	return(m_strCasinoName);
}

void CConfig::SetCasinoAddress1(LPCTSTR casinoAddress1)
{
	m_casinoAddress1 = casinoAddress1;
	char tempCasinoAddress1[SIZE_OF_CASINO_ADDRESS1];
	CUtilities::ConvertCharsToChars( m_casinoAddress1, tempCasinoAddress1, SIZE_OF_CASINO_ADDRESS1);
	m_nvramConfig.Set(m_casinoAddress1NVRAMOffset, (BYTE *)tempCasinoAddress1, SIZE_OF_CASINO_ADDRESS1);
}

const std::string &CConfig::GetCasinoAddress1() const
{
	return(m_casinoAddress1);
}


void CConfig::SetCasinoAddress2(LPCTSTR casinoAddress2)
{
	m_casinoAddress2 = casinoAddress2;
	char tempCasinoAddress2[SIZE_OF_CASINO_ADDRESS2];
	CUtilities::ConvertCharsToChars( m_casinoAddress2, tempCasinoAddress2, SIZE_OF_CASINO_ADDRESS2);
	m_nvramConfig.Set(m_casinoAddress2NVRAMOffset, (BYTE *)tempCasinoAddress2, SIZE_OF_CASINO_ADDRESS2);
}

const std::string &CConfig::GetCasinoAddress2() const
{
	return(m_casinoAddress2);
}

// CConfig::SetVirtualSessionMaxDuration.
//     IN virtualSessionMaxDuration - the maximum duration for a virtual session.
//     RETURN - the prevous maximum duration for a virtual session.
// Set the maximum duration for a virtual session using the virtualSessionMaxDuration parameter.
DWORD CConfig::SetVirtualSessionMaxDuration(DWORD virtualSessionMaxDuration)
{
	DWORD bOldVal = m_virtualSessionMaxDuration;

	m_virtualSessionMaxDuration = virtualSessionMaxDuration;
	m_nvramConfig.Set(m_virtualSessionMaxDurationNVRAMOffset, (BYTE *)&m_virtualSessionMaxDuration, sizeof(m_virtualSessionMaxDuration));

	return bOldVal;
}

// CConfig::GetVirtualSessionMaxDuration.
//     RETURN - the maximum duration for a virtual session.
DWORD CConfig::GetVirtualSessionMaxDuration()
{
	return(m_virtualSessionMaxDuration);
}


// CConfig::SetVirtualSessionMaxDuration.
//     IN virtualSessionNoPlayTimeout - the virtual session timeout period for no play.
//     RETURN - the prevous virtual session timeout period for no play.
// Set the virtual session timeout period for no play using the virtualSessionNoPlayTimeout parameter.
DWORD CConfig::SetVirtualSessionNoPlayTimeout(DWORD virtualSessionNoPlayTimeout)
{
	DWORD bOldVal = m_virtualSessionNoPlayTimeout;

	m_virtualSessionNoPlayTimeout = virtualSessionNoPlayTimeout;
	m_nvramConfig.Set(m_virtualSessionNoPlayTimeoutNVRAMOffset, (BYTE *)&m_virtualSessionNoPlayTimeout, sizeof(m_virtualSessionNoPlayTimeout));

	return bOldVal;
}

// CConfig::GetVirtualSessionNoPlayTimeout.
//     RETURN - the virtual session timeout period for no play.
DWORD CConfig::GetVirtualSessionNoPlayTimeout()
{
	return(m_virtualSessionNoPlayTimeout);
}


DWORD CConfig::SetCardTimeout(DWORD cardTimeout)
{
	DWORD bOldVal = m_cardTimeout;

	m_cardTimeout = cardTimeout;
	m_nvramConfig.Set(m_cardTimeoutNVRAMOffset, (BYTE *)&m_cardTimeout, sizeof(m_cardTimeout));

	return bOldVal;
}

DWORD CConfig::GetCardTimeout()
{
	return(m_cardTimeout);
}

void CConfig::SetPlayerCardPrefixes(const std::vector<BYTE> &playerCardsPrefix)
{
    m_playerCardPrefixes = playerCardsPrefix;
    int numberOfCardPrefixesInNVRAM = min((int)playerCardsPrefix.size(), NUMBER_OF_VALID_CARDS_PREFIX_IN_NVRAM);

    // Copy the prefixes into NVRAM
    BYTE prefixForNVRAM[NUMBER_OF_VALID_CARDS_PREFIX_IN_NVRAM];
    for (int prefixIndex = 0; prefixIndex < NUMBER_OF_VALID_CARDS_PREFIX_IN_NVRAM; prefixIndex++)
    {
        byte prefix = 0;
        if (prefixIndex < numberOfCardPrefixesInNVRAM)
        {
            prefix = playerCardsPrefix[prefixIndex];
            LogString(GENERAL, _T("Player prefix stored in NVRAM: %x"), prefix);
        }
        
        prefixForNVRAM[prefixIndex] = prefix;
    }

    for (int prefixIndex = numberOfCardPrefixesInNVRAM; prefixIndex < playerCardsPrefix.size(); prefixIndex++) 
    {
        LogString(GENERAL, _T("Player prefix NOT stored in NVRAM: %x"), playerCardsPrefix[prefixIndex]);
    }

    m_nvramConfig.Set(m_numberOfCardPrefixesNVRAMOffset, (BYTE *)&numberOfCardPrefixesInNVRAM, sizeof(numberOfCardPrefixesInNVRAM), 
                      m_playerCardPrefixesNVRAMOffset, (BYTE *)prefixForNVRAM, NUMBER_OF_VALID_CARDS_PREFIX_IN_NVRAM);
}

const std::vector<BYTE> CConfig::GetPlayerCardPrefixes() const
{
	return(m_playerCardPrefixes);
}

bool CConfig::IsPlayerCardPrefix( byte prefix, bool isUniverseCard )
{
	bool bFound = false;
	
	if(isUniverseCard && m_universeEnabled)
	{
        for (auto &universalPrefix : m_universePlayerCardPrefixes)
        {
			if ( universalPrefix == prefix )
            {
				bFound = true;
                break;
            }
		}
	}
	else if(!m_universeEnabled || (m_universeEnabled && m_mixedModeEnabled))
	{
        for (auto &playerPrefix : m_playerCardPrefixes)
        {
			if ( playerPrefix == prefix )
            {
				bFound = true;
                break;
            }
		}
	}

	return bFound;
}

BYTE CConfig::GetRcclCardPlayerPrefix() const
{
	BYTE playerCardPrefix = DEFAULT_RCCL_PLAYER_CARD_PREFIX;

	if ( m_playerCardPrefixes.size() > RCCL_PLAYER_CARD_PREFIXES_INDEX )
	{
		playerCardPrefix = m_playerCardPrefixes[RCCL_PLAYER_CARD_PREFIXES_INDEX];
	}

	return playerCardPrefix;
}


BYTE CConfig::SetGlobalCardPrefix(BYTE globalCardPrefix)
{
	BYTE bOldVal = m_globalCardPrefix;

	m_globalCardPrefix = globalCardPrefix;
	m_nvramConfig.Set(m_globalCardPrefixNVRAMOffset, (BYTE *)&m_globalCardPrefix, sizeof(m_globalCardPrefix));

	return bOldVal;
}

BYTE CConfig::GetGlobalCardPrefix() const
{
	return(m_globalCardPrefix);
}


BYTE CConfig::SetMechanicCardPrefix(BYTE mechanicCardPrefix)
{
	BYTE bOldVal = m_mechanicCardPrefix;

	m_mechanicCardPrefix = mechanicCardPrefix;
	m_nvramConfig.Set(m_mechanicCardPrefixNVRAMOffset, (BYTE *)&m_mechanicCardPrefix, sizeof(m_mechanicCardPrefix));

	return bOldVal;
}

BYTE CConfig::GetMechanicCardPrefix() const
{
	return(m_mechanicCardPrefix);
}


BYTE CConfig::SetFloorCardPrefix(BYTE floorCardPrefix)
{
	BYTE bOldVal = m_floorCardPrefix;

	m_floorCardPrefix = floorCardPrefix;
	m_nvramConfig.Set(m_floorCardPrefixNVRAMOffset, (BYTE *)&m_floorCardPrefix, sizeof(m_floorCardPrefix));

	return bOldVal;
}

BYTE CConfig::GetFloorCardPrefix() const
{
	return(m_floorCardPrefix);
}


BYTE CConfig::SetTempCardPrefix(BYTE tempCardPrefix)
{
	BYTE bOldVal = m_tempCardPrefix;

	m_tempCardPrefix = tempCardPrefix;
	m_nvramConfig.Set(m_tempCardPrefixNVRAMOffset, (BYTE *)&m_tempCardPrefix, sizeof(m_tempCardPrefix));

	return bOldVal;
}

BYTE CConfig::GetTempCardPrefix() const
{
	return(m_tempCardPrefix);
}

bool CConfig::SetFillAmount( bool fillAmount )
{
	bool bOldVal = m_fillAmount;

	m_fillAmount = fillAmount;
	m_nvramConfig.Set(m_fillAmountNVRAMOffset, (BYTE *)&m_fillAmount, sizeof(m_fillAmount));

	return bOldVal;
}

bool CConfig::FillAmount()
{
	return m_fillAmount;
}

TicketExpirationType CConfig::SetTicketExpirationType( TicketExpirationType expType)
{
	TicketExpirationType bOldVal = m_ticketExpType;

	m_ticketExpType = expType;
	m_nvramConfig.Set(m_ticketExpTypeNVRAMOffset, (BYTE *)&m_ticketExpType, sizeof(m_ticketExpType));

	return bOldVal;
}

TicketExpirationType CConfig::GetTicketExpirationType() const
{
	return m_ticketExpType;
}

int CConfig::SetTicketExpirationDuration( int expirationDuration )
{
	int bOldVal = m_ticketExpirationDuration;

	m_ticketExpirationDuration = expirationDuration;
	m_nvramConfig.Set(m_ticketExpirationDurationNVRAMOffset, (BYTE *)&m_ticketExpirationDuration, sizeof(m_ticketExpirationDuration));

	return bOldVal;
}

int CConfig::GetTicketExpirationDuration() const
{
	return m_ticketExpirationDuration;
}

bool CConfig::SetTicketingEnabled( bool ticketingEnabled )
{
	bool bOldVal = m_ticketingEnabled;

	m_ticketingEnabled = ticketingEnabled;
	m_nvramConfig.Set(m_ticketingEnabledNVRAMOffset, (BYTE *)&m_ticketingEnabled, sizeof(m_ticketingEnabled));

	return bOldVal;
}

bool CConfig::TicketingEnabled() const
{
	return m_ticketingEnabled;
}

bool CConfig::SetSystemGeneratedTicketValidation ( bool systemGenerated )
{
	bool bOldVal = m_systemGeneratedTicketValidation;

	m_systemGeneratedTicketValidation = systemGenerated;
	m_nvramConfig.Set(m_systemGeneratedTicketValidationNVRAMOffset, (BYTE *)&m_systemGeneratedTicketValidation, sizeof(m_systemGeneratedTicketValidation));

	return bOldVal;
}

bool CConfig::SystemGeneratedTicketValidation()
{
	return m_systemGeneratedTicketValidation;
}

bool  CConfig::SetTicketBufferingEnabled( bool tickBuffEnabled )
{
	bool bOldVal = m_ticketBufferingEnabled;

	m_ticketBufferingEnabled = tickBuffEnabled;
	m_nvramConfig.Set(m_ticketBufferingEnabledNVRAMOffset, (BYTE *)&m_ticketBufferingEnabled, sizeof(m_ticketBufferingEnabled));

	return bOldVal;
}

bool  CConfig::TicketBufferingEnabled() const
{
	return m_ticketBufferingEnabled;
}

WORD CConfig::SetPlayerTrackingMultiplier( WORD playerTrackingMult )
{
	WORD bOldVal = m_playerTrackingMultiplier;

	m_playerTrackingMultiplier = playerTrackingMult;
	m_nvramConfig.Set(m_playerTrackingMultiplierNVRAMOffset, (BYTE *)&m_playerTrackingMultiplier, sizeof(m_playerTrackingMultiplier));

	return bOldVal;
}

int CConfig::GetPlayerTrackingMultiplier()
{
	return m_playerTrackingMultiplier;
}

WORD CConfig::SetPlayerTrackingDivisor( WORD playerTrackingDiv )
{
	WORD bOldVal = m_playerTrackingDivisor;

	m_playerTrackingDivisor = playerTrackingDiv;
	m_nvramConfig.Set(m_playerTrackingDivisorNVRAMOffset, (BYTE *)&m_playerTrackingDivisor, sizeof(m_playerTrackingDivisor));

	return bOldVal;
}

int CConfig::GetPlayerTrackingDivisor()
{
	return m_playerTrackingDivisor;
}

bool CConfig::SetUseHopperLevel(bool useHopperLevel)
{
	bool bOldVal = m_useHopperLevel;

	m_useHopperLevel = useHopperLevel;
	m_nvramConfig.Set(m_useHopperLevelNVRAMOffset, (BYTE *)&m_useHopperLevel, sizeof(m_useHopperLevel));

	return bOldVal;
}

bool CConfig::UseHopperLevel() const
{
	return m_useHopperLevel;
}

bool CConfig::SetVirtualSessionEnabled(bool virtualSessionEnabled)
{
	bool bOldVal = m_virtualSessionEnabled;

	m_virtualSessionEnabled = virtualSessionEnabled;
	m_nvramConfig.Set(m_virtualSessionEnabledNVRAMOffset, (BYTE *)&m_virtualSessionEnabled, sizeof(m_virtualSessionEnabled));

	return bOldVal;
}

bool CConfig::VirtualSessionEnabled() const
{
	return m_virtualSessionEnabled;
}

bool CConfig::SetDisplayPlayerName(bool displayPlayerName)
{
	bool bOldVal = m_displayPlayerName;

	m_displayPlayerName = displayPlayerName;
	m_nvramConfig.Set(m_displayPlayerNameNVRAMOffset, (BYTE *)&m_displayPlayerName, sizeof(m_displayPlayerName));

	return bOldVal;
}

bool CConfig::DisplayPlayerName() const
{ 
	return m_displayPlayerName;
}

bool CConfig::SetDisplayPlayerPtsTotal(bool displayPlayerPtsTotal)
{
	bool bOldVal = m_displayPlayerPtsTotal;

	m_displayPlayerPtsTotal = displayPlayerPtsTotal;
	m_nvramConfig.Set(m_displayPlayerPtsTotalNVRAMOffset, (BYTE *)&m_displayPlayerPtsTotal, sizeof(m_displayPlayerPtsTotal));

	return bOldVal;
}
bool CConfig::DisplayPlayerPtsTotal() const
{ 
	return m_displayPlayerPtsTotal;
}

bool CConfig::SetDisplayCountdownAndSession(bool displayCountdownAndSession)
{
	bool bOldVal = m_displayCountdownAndSession;

	m_displayCountdownAndSession = displayCountdownAndSession;
	m_nvramConfig.Set(m_displayCountdownAndSessionNVRAMOffset, (BYTE *)&m_displayCountdownAndSession, sizeof(m_displayCountdownAndSession));

	return bOldVal;
}

bool  CConfig::DisplayCountdownAndSession() const
{
	return m_displayCountdownAndSession;
}

bool CConfig::SetDisplayGoodLuckAndSessionPts(bool displayGoodLuckAndSessionPts)
{
	bool bOldVal = m_displayGoodLuckAndSessionPts;

	m_displayGoodLuckAndSessionPts = displayGoodLuckAndSessionPts;
	m_nvramConfig.Set(m_displayGoodLuckAndSessionPtsNVRAMOffset, (BYTE *)&m_displayGoodLuckAndSessionPts, sizeof(m_displayGoodLuckAndSessionPts));

	return bOldVal;
}

bool CConfig::DisplayGoodLuckAndSessionPts() const
{
	return m_displayGoodLuckAndSessionPts;
}

bool CConfig::SetDisplayGoodLuckAndSession(bool displayGoodLuckAndSession)
{
	bool bOldVal = m_displayGoodLuckAndSession;

	m_displayGoodLuckAndSession = displayGoodLuckAndSession;
	m_nvramConfig.Set(m_displayGoodLuckAndSessionNVRAMOffset, (BYTE *)&m_displayGoodLuckAndSession, sizeof(m_displayGoodLuckAndSession));

	return bOldVal;
}

bool CConfig::DisplayGoodLuckAndSession() const
{
	return m_displayGoodLuckAndSession;
}

bool CConfig::SetDisplayNumericPointTotal(bool displayNumericPointTotal)
{
	bool bOldVal = m_displayNumericPointTotal;

	m_displayNumericPointTotal = displayNumericPointTotal;
	m_nvramConfig.Set(m_displayNumericPointTotalNVRAMOffset, (BYTE *)&m_displayNumericPointTotal, sizeof(m_displayNumericPointTotal));

	return bOldVal;
}

bool CConfig::DisplayNumericPointTotal() const
{
	return m_displayNumericPointTotal;
}

bool CConfig::SetDisplayComps(bool displayComps)
{
	bool bOldVal = m_displayComps;

	m_displayComps = displayComps;
	m_nvramConfig.Set(m_displayCompsNVRAMOffset, (BYTE *)&m_displayComps, sizeof(m_displayComps));

	return bOldVal;
}

bool  CConfig::DisplayComps() const
{
	return m_displayComps;
}

bool CConfig::SetDisplayCongrats(bool displayCongrats)
{
	bool bOldVal = m_displayCongrats;

	m_displayCongrats = displayCongrats;
	m_nvramConfig.Set(m_displayCongratsNVRAMOffset, (BYTE *)&m_displayCongrats, sizeof(m_displayCongrats));

	return bOldVal;
}

bool  CConfig::DisplayCongrats() const
{
	return m_displayCongrats;
}

bool CConfig::SetDisplayPtsTotalAsCash(bool displayPtsTotalAsCash)
{
	bool bOldVal = m_displayPtsTotalAsCash;

	m_displayPtsTotalAsCash = displayPtsTotalAsCash;
	m_nvramConfig.Set(m_displayCongratsNVRAMOffset, (BYTE *)&m_displayCongrats, sizeof(m_displayCongrats));

	return bOldVal;
}

bool  CConfig::DisplayPtsTotalAsCash() const
{
	return m_displayPtsTotalAsCash;
}

bool CConfig::SetDisplayPtsTotalAsCashSession(bool displayPtsTotalAsCashSession)
{
	bool bOldVal = m_displayPtsTotalAsCashSession;

	m_displayPtsTotalAsCashSession = displayPtsTotalAsCashSession;
	m_nvramConfig.Set(m_displayPtsTotalAsCashSessionNVRAMOffset, (BYTE *)&m_displayPtsTotalAsCashSession, sizeof(m_displayPtsTotalAsCashSession));

	return bOldVal;
}

bool  CConfig::DisplayPtsTotalAsCashSession() const
{
	return m_displayPtsTotalAsCashSession;
}

bool CConfig::SetDisplayTotalAndSessionPtsNum(bool displayTotalAndSessionPtsNum)
{
	bool bOldVal = m_displayTotalAndSessionPtsNum ;

	m_displayTotalAndSessionPtsNum = displayTotalAndSessionPtsNum;
	m_nvramConfig.Set(m_displayTotalAndSessionPtsNumNVRAMOffset, (BYTE *)&m_displayTotalAndSessionPtsNum, sizeof(m_displayTotalAndSessionPtsNum));

	return bOldVal;
}

bool CConfig::DisplayTotalAndSessionPtsNum() const
{
	return m_displayTotalAndSessionPtsNum;
}

bool CConfig::SetDisplayTotalPtsSessionPtsAsCash( bool displayTotalPtsSessionPtsAsCash )
{
	bool bOldVal = m_displayTotalPtsSessionPtsAsCash;

	m_displayTotalPtsSessionPtsAsCash = displayTotalPtsSessionPtsAsCash;
	m_nvramConfig.Set(m_displayTotalPtsSessionPtsAsCashNVRAMOffset, (BYTE *)&m_displayTotalPtsSessionPtsAsCash, sizeof(m_displayTotalPtsSessionPtsAsCash));

	return bOldVal;
}
bool CConfig::DisplayTotalPtsSessionPtsAsCash() const
{
	return m_displayTotalPtsSessionPtsAsCash;
}

bool CConfig::SetPBTCashEnabled_local(bool pbtCashEnabled_local)
{
	bool bOldVal = m_pbtCashEnabled_local;

	m_pbtCashEnabled_local = pbtCashEnabled_local;
	m_nvramConfig.Set(m_pbtCashEnabled_localNVRAMOffset, (BYTE *)&m_pbtCashEnabled_local, sizeof(m_pbtCashEnabled_local));

	return bOldVal;
}

bool CConfig::PBTCashEnabled() const
{
	return m_pbtCashEnabled_local && m_pbtCashEnabled_global;
}

bool CConfig::PBTCashEnabledGlobal()
{
	return m_pbtCashEnabled_global;
}

bool CConfig::SetPBTPointsAsPromo(bool pbtPointsAsPromo)
{
	bool bOldVal = m_pbtPointsAsPromo;

	m_pbtPointsAsPromo = pbtPointsAsPromo;
	//m_nvramConfig.Set(m_pbtCashEnabled_localNVRAMOffset, (BYTE *)&m_pbtCashEnabled_local, sizeof(m_pbtCashEnabled_local));
	m_nvramConfig.Set(m_pbtPointsAsPromoNVRAMOffset, (BYTE *)&m_pbtPointsAsPromo, sizeof(m_pbtPointsAsPromo));
	return bOldVal;
}

bool CConfig::PBTPointsAsPromo() const
{
	return m_pbtPointsAsPromo && m_pbtPointsEnabled_global;
}

bool CConfig::SetPBTPointsEnabled_local(bool pbtPointsEnabled_local)
{
	bool bOldVal = m_pbtPointsEnabled_local;

	m_pbtPointsEnabled_local = pbtPointsEnabled_local;
	m_nvramConfig.Set(m_pbtPointsEnabled_localNVRAMOffset, (BYTE *)&m_pbtPointsEnabled_local, sizeof(m_pbtPointsEnabled_local));

	return bOldVal;
}


bool CConfig::PBTPointsEnabled() const
{

	return m_pbtPointsEnabled_local && m_pbtPointsEnabled_global;
}

bool CConfig::PBTPointsEnabledGlobal() const
{

	return m_pbtPointsEnabled_global;
}

bool CConfig::SetPBTPromoEnabled_local(bool pbtPromoEnabled_local)
{
	bool bOldVal = m_pbtPromoEnabled_local;

	m_pbtPromoEnabled_local = pbtPromoEnabled_local;
	m_nvramConfig.Set(m_pbtPromoEnabled_localNVRAMOffset, (BYTE *)&m_pbtPromoEnabled_local, sizeof(m_pbtPromoEnabled_local));

	return bOldVal;
}


bool CConfig::PBTPromoEnabled() const
{
	return m_pbtPromoEnabled_local && m_pbtPromoEnabled_global;
}

bool CConfig::PBTPromoEnabledGlobal()
{
	return m_pbtPromoEnabled_global;
}


bool CConfig::SetCoinsWonAccounting(bool coinsWonAccounting)
{
	bool bOldVal = m_coinsWonAccounting;

	m_coinsWonAccounting = coinsWonAccounting;
	m_nvramConfig.Set(m_coinsWonAccountingNVRAMOffset, (BYTE *)&m_coinsWonAccounting, sizeof(m_coinsWonAccounting));

	return bOldVal;
}


bool CConfig::CoinsWonAccounting()
{
	return m_coinsWonAccounting;
}


bool CConfig::SetCoinsInAccounting(bool coinsInAccounting)
{
	bool bOldVal = m_coinsInAccounting;

	m_coinsInAccounting = coinsInAccounting;
	m_nvramConfig.Set(m_coinsInAccountingNVRAMOffset, (BYTE *)&m_coinsInAccounting, sizeof(m_coinsInAccounting));

	return bOldVal;
}


bool CConfig::CoinsInAccounting()
{
	return m_coinsInAccounting;
}


bool CConfig::SetCoinsOutAccounting(bool coinsOutAccounting)
{
	bool bOldVal = m_coinsOutAccounting;

	m_coinsOutAccounting = coinsOutAccounting;
	m_nvramConfig.Set(m_coinsOutAccountingNVRAMOffset, (BYTE *)&m_coinsOutAccounting, sizeof(m_coinsOutAccounting));

	return bOldVal;
}


bool CConfig::CoinsOutAccounting()
{
	return m_coinsOutAccounting;
}


bool CConfig::SetTheoreticalAccounting(bool theoreticalAccounting)
{
	bool bOldVal = m_theoreticalAccounting;

	m_theoreticalAccounting = theoreticalAccounting;
	m_nvramConfig.Set(m_theoreticalAccountingNVRAMOffset, (BYTE *)&m_theoreticalAccounting, sizeof(m_theoreticalAccounting));

	return bOldVal;
}


bool CConfig::TheoreticalAccounting()
{
	return m_theoreticalAccounting;
}

AccountingType CConfig::GetAccountingType( void )
{
	AccountingType type = ACCOUNTING_COINS_IN;

	if ( CoinsInAccounting() )
		type = ACCOUNTING_COINS_IN;
	else if ( CoinsOutAccounting() )
		type = ACCOUNTING_COINS_OUT;
	else if ( CoinsWonAccounting() )
		type = ACCOUNTING_COINS_WON;
	else if ( TheoreticalAccounting() )
		type = ACCOUNTING_THEORETICAL;

	return type;
}

bool CConfig::SetCardInPBT(bool cardInPBT)
{
	bool bOldVal = m_cardInPBT;

	m_cardInPBT = cardInPBT;
	m_nvramConfig.Set(m_cardInPBTNVRAMOffset, (BYTE *)&m_cardInPBT, sizeof(m_cardInPBT));

	return bOldVal;
}


bool CConfig::CardInPBT() const
{
	return m_cardInPBT;
}


bool CConfig::SetEmployeePinRequired(bool employeePinRequired)
{
	bool bOldVal = m_employeePinRequired;

	m_employeePinRequired = employeePinRequired;
	m_nvramConfig.Set(m_employeePinRequiredNVRAMOffset, (BYTE *)&m_employeePinRequired, sizeof(m_employeePinRequired));

	return bOldVal;
}


bool CConfig::EmployeePinRequired() const
{
	return m_employeePinRequired;
}


bool CConfig::SetPartialBonusAward(bool PartialBonusAward)
{
	bool bOldVal = m_PartialBonusAward;

	m_PartialBonusAward = PartialBonusAward;
	m_nvramConfig.Set(m_PartialBonusAwardNVRAMOffset, (BYTE *)&m_PartialBonusAward, sizeof(m_PartialBonusAward));

	return bOldVal;
}


bool CConfig::PartialBonusAward() const
{
	return m_PartialBonusAward;
}

bool CConfig::SetPBFromGameEnabled(bool PBFromGameEnabled)
{
	bool bOldVal = m_PBFromGameEnabled;

	m_PBFromGameEnabled = PBFromGameEnabled;
	m_nvramConfig.Set(m_PBFromGameEnabledNVRAMOffset, (BYTE *)&m_PBFromGameEnabled, sizeof(m_PBFromGameEnabled));

	return bOldVal;
}

bool CConfig::PBFromGameEnabled() const
{
	return m_PBFromGameEnabled && m_pbtCashEnabled_global;
}


bool CConfig::SetCouponPromoInEnabled(bool couponPromoInEnabled)
{
	bool bOldVal = m_couponPromoInEnabled;

	m_couponPromoInEnabled = couponPromoInEnabled;
	m_nvramConfig.Set(m_couponPromoInEnabledNVRAMOffset, (BYTE *)&m_couponPromoInEnabled, sizeof(m_couponPromoInEnabled));

	return bOldVal;
}

bool CConfig::CouponPromoInEnabled() const
{
	return m_couponPromoInEnabled;
}

bool CConfig::SetCouponPromoOutEnabled(bool couponPromoOutEnabled)
{
	bool bOldVal = m_couponPromoOutEnabled;

	m_couponPromoOutEnabled = couponPromoOutEnabled;
	m_nvramConfig.Set(m_couponPromoOutEnabledNVRAMOffset, (BYTE *)&m_couponPromoOutEnabled, sizeof(m_couponPromoOutEnabled));

	return bOldVal;
}

bool CConfig::CouponPromoOutEnabled()
{
	return m_couponPromoOutEnabled;
}

bool CConfig::SetVoucherPromoInEnabled(bool voucherPromoInEnabled)
{
	bool bOldVal = m_voucherPromoInEnabled;

	m_voucherPromoInEnabled = voucherPromoInEnabled;
	m_nvramConfig.Set(m_voucherPromoInEnabledNVRAMOffset, (BYTE *)&m_voucherPromoInEnabled, sizeof(m_voucherPromoInEnabled));

	return bOldVal;
}


bool CConfig::VoucherPromoInEnabled() const
{
	return m_voucherPromoInEnabled;
}

bool CConfig::CEPOutEnabled() const
{
	return (m_senOption2Raw & SO2_CEP_OUT_ENABLED) == SO2_CEP_OUT_ENABLED;
}

bool CConfig::NCEPOutEnabled() const
{
	return ((m_senOption2Raw & SO2_NCEP_OUT_ENABLED) == SO2_NCEP_OUT_ENABLED) && m_pbtPromoEnabled_global;
}

bool CConfig::SetPBTPinNotRequired(bool PBTPinNotRequired)
{
	bool bOldVal = m_PBTPinNotRequired;

	m_PBTPinNotRequired = PBTPinNotRequired;
	m_nvramConfig.Set(m_PBTPinNotRequiredNVRAMOffset, (BYTE *)&m_PBTPinNotRequired, sizeof(m_PBTPinNotRequired));

	return bOldVal;
}

bool CConfig::PBTPinNotRequired()
{
	return m_PBTPinNotRequired;
}

bool CConfig::SetVoucherOutEnabled(bool voucherOutEnabled)
{
	bool bOldVal = m_voucherOutEnabled;

	m_voucherOutEnabled = voucherOutEnabled;
	m_nvramConfig.Set(m_voucherOutEnabledNVRAMOffset, (BYTE *)&m_voucherOutEnabled, sizeof(m_voucherOutEnabled));

	return bOldVal;
}

bool CConfig::VoucherOutEnabled() const
{
	return m_voucherOutEnabled;
}

bool CConfig::SetRcclCardsEnabled(bool rcclCardsEnabled)
{
	bool bOldVal = m_rcclCardsEnabled;

	m_rcclCardsEnabled = rcclCardsEnabled;
	m_nvramConfig.Set(m_rcclCardsEnabledNVRAMOffset, (BYTE *)&m_rcclCardsEnabled, sizeof(m_rcclCardsEnabled));

	return bOldVal;
}

bool CConfig::RcclCardsEnabled() const
{
	return m_rcclCardsEnabled;;
}

// CConfig::SetNclCardsEnabled()
// Set the NCL cards enabled configuration.
bool CConfig::SetNclCardsEnabled(bool nclCardsEnabled)
{
	bool bOldVal = m_nclCardsEnabled;

	m_nclCardsEnabled = nclCardsEnabled;
	m_nvramConfig.Set(m_nclCardsEnabledNVRAMOffset, (BYTE *)&m_nclCardsEnabled, sizeof(m_nclCardsEnabled));

	return bOldVal;
}

// CConfig::NclCardsEnabled()
// Return the NCL cards enabled configuration.
bool CConfig::NclCardsEnabled() const
{
	return m_nclCardsEnabled;
}

bool CConfig::SetTicketInEnabled(bool ticketInEnabled)
{
	bool bOldVal = m_ticketInEnabled;

	m_ticketInEnabled = ticketInEnabled;
	m_nvramConfig.Set(m_ticketInEnabledNVRAMOffset, (BYTE *)&m_ticketInEnabled, sizeof(m_ticketInEnabled));

	return bOldVal;
}

bool CConfig::TicketInEnabled() const
{
	return m_ticketInEnabled;
}


bool CConfig::SetPBTPromoEnabled_Global( bool pbtPromoEnabled_global)
{
	bool bOldVal = m_pbtPromoEnabled_global;

	m_pbtPromoEnabled_global = pbtPromoEnabled_global;
	m_nvramConfig.Set(m_pbtPromoEnabled_globalNVRAMOffset, (BYTE *)&m_pbtPromoEnabled_global, sizeof(m_pbtPromoEnabled_global));

	return bOldVal;
}

bool CConfig::SetPBTPointsEnabled_Global(bool pbtPointsEnabled_global)
{
	bool bOldVal = m_pbtPointsEnabled_global;

	m_pbtPointsEnabled_global = pbtPointsEnabled_global;
	m_nvramConfig.Set(m_pbtPointsEnabled_globalNVRAMOffset, (BYTE *)&m_pbtPointsEnabled_global, sizeof(m_pbtPointsEnabled_global));

	return bOldVal;
}

bool CConfig::SetPBTCashEnabled_Global(bool pbtCashEnabled_global)
{
	bool bOldVal = m_pbtCashEnabled_global;

	m_pbtCashEnabled_global = pbtCashEnabled_global;
	m_nvramConfig.Set(m_pbtCashEnabled_globalNVRAMOffset, (BYTE *)&m_pbtCashEnabled_global, sizeof(m_pbtCashEnabled_global));

	return bOldVal;
}

bool CConfig::SetCashTransferTracking( bool cashTransferTracking)
{
	bool bOldVal = m_cashTransferTracking;

	m_cashTransferTracking = cashTransferTracking;
	m_nvramConfig.Set(m_cashTransferTrackingNVRAMOffset, (BYTE *)&m_cashTransferTracking, sizeof(m_cashTransferTracking));

	return bOldVal;
}

bool  CConfig::GetCashTransferTracking()
{
	return m_cashTransferTracking;
}

int CConfig::SetProgressiveDenomination(int progressiveDenomination)
{
	int bOldVal = m_progressiveDenomination;

	m_progressiveDenomination = progressiveDenomination;
	m_nvramConfig.Set(m_progressiveDenominationNVRAMOffset, (BYTE *)&m_progressiveDenomination, sizeof(m_progressiveDenomination));

	return bOldVal;
}

int CConfig::GetProgressiveDenomination()
{
	return m_progressiveDenomination;
}

int CConfig::SetPendingSlotmast(int slotmast)
{
	int bOldVal = m_pendingSlotmast;

	m_pendingSlotmast = slotmast;
	m_nvramConfig.Set( m_pendingSlotmastNVRAMOffset, (BYTE *)&m_pendingSlotmast, sizeof(m_pendingSlotmast));

	return bOldVal;
}

int CConfig::GetPendingSlotmast()
{
	return m_pendingSlotmast;
}

bool CConfig::SetAllowFloorLockout(bool allowFloorLockout)
{
	bool bOldVal = m_allowFloorLockout;

	m_allowFloorLockout = allowFloorLockout;
	m_nvramConfig.Set(m_allowFloorLockoutNVRAMOffset, (BYTE *)&m_allowFloorLockout, sizeof(m_allowFloorLockout));

	return bOldVal;
}

bool CConfig::AllowFloorLockout()
{
	return m_allowFloorLockout;
}

short CConfig::SetTimezoneOffset( short timezoneOffset )
{
	short bOldVal = m_timezoneOffset;

	m_timezoneOffset = timezoneOffset;
	m_nvramConfig.Set(m_timezoneOffsetNVRAMOffset, (BYTE *)&m_timezoneOffset, sizeof(m_timezoneOffset));

	return bOldVal;
}

short CConfig::GetTimezoneOffset( void )
{
	return m_timezoneOffset;
}

bool CConfig::SetUseDST( bool useDST )
{
	bool bOldVal = m_useDST;

	m_useDST = useDST;
	m_nvramConfig.Set(m_useDSTNVRAMOffset, (BYTE *)&m_useDST, sizeof(m_useDST));

	return bOldVal;
}

bool CConfig::UseDST( void )
{
	return m_useDST;
}

__int64 CConfig::SetLastGameStart(__int64 lastGameStart)
{
	__int64 bOldVal = m_lastGameStart;

	m_lastGameStart = lastGameStart;
	m_nvramConfig.Set(m_lastGameStartNVRAMOffset, (BYTE *)&m_lastGameStart, sizeof(m_lastGameStart));

	return bOldVal;
}

__int64 CConfig::GetLastGameStart()
{
	return m_lastGameStart;
}

__int64 CConfig::SetCoinInAtLastGameStart(__int64 coinInAtLastGameStart)
{
	__int64 bOldVal = m_coinInAtLastGameStart;

	m_coinInAtLastGameStart = coinInAtLastGameStart;
	m_nvramConfig.Set(m_coinInAtLastGameStartNVRAMOffset, (BYTE *)&m_coinInAtLastGameStart, sizeof(m_coinInAtLastGameStart));

	return bOldVal;
}

__int64 CConfig::GetCoinInAtLastGameStart()
{
	return m_coinInAtLastGameStart;
}

__int64 CConfig::SetCoinsWonAtLastGameEnd(__int64 coinsWonAtLastGameEnd)
{
	__int64 bOldVal = coinsWonAtLastGameEnd;

	m_coinsWonAtLastGameEnd = coinsWonAtLastGameEnd;
	m_nvramConfig.Set(m_coinsWonAtLastGameEndNVRAMOffset, (BYTE *)&m_coinsWonAtLastGameEnd, sizeof(m_coinsWonAtLastGameEnd));

	return bOldVal;
}

__int64 CConfig::GetCoinsWonAtLastGameEnd() const
{
	return m_coinsWonAtLastGameEnd;
}

void CConfig::SetLastCardRead(const byte* cardID)
{
	memcpy(m_lastCardRead, cardID, CARD_ID_LEN);
	m_nvramConfig.Set(m_lastCardReadNVRAMOffset, (BYTE *)&m_lastCardRead, CARD_ID_LEN);
}

void CConfig::ClearLastCardRead( void )
{
	memset( m_lastCardRead, 0, CARD_ID_LEN );
	m_nvramConfig.Set(m_lastCardReadNVRAMOffset, (BYTE *)&m_lastCardRead, CARD_ID_LEN);
}

byte * CConfig::GetLastCardRead()
{
	return m_lastCardRead;
}

DWORD CConfig::SetSenOption1Raw( DWORD senOption1Raw )
{
	DWORD oldValue = m_senOption1Raw;

	m_senOption1Raw = senOption1Raw;
	m_nvramConfig.Set( m_senOption1RawNVRAMOffset, (BYTE *)&m_senOption1Raw, sizeof( m_senOption1Raw ) );

	return oldValue;
}

DWORD CConfig::GetSenOption1Raw( void )
{
	return m_senOption1Raw;
}

DWORD CConfig::SetSenOption2Raw( DWORD senOption2Raw )
{
	DWORD oldValue = m_senOption2Raw;

	m_senOption2Raw = senOption2Raw;
	m_nvramConfig.Set( m_senOption2RawNVRAMOffset, (BYTE *)&m_senOption2Raw, sizeof( m_senOption2Raw ) );

	return oldValue;
}

DWORD CConfig::GetSenOption2Raw( void )
{
	return m_senOption2Raw;
}

bool CConfig::AutoUploadCredits() const
{
	bool autoUploadCredits(false);
	bool ncepAuto(NcepAutoUpload() && NCEPOutEnabled());
	bool cepAuto(CepAutoUpload() && PBFromGameEnabled() && CEPOutEnabled());
	bool watAuto(WatAutoUpload() && PBFromGameEnabled());
	if (m_extensibleFlags.IsPoller_12_11_6000_or_Higher())
	{
		autoUploadCredits = m_autoUploadCreditsGlobal && (ncepAuto || cepAuto || watAuto);
	}
	else
	{
		autoUploadCredits = NcepAutoUpload() && m_pbtPromoEnabled_global;
	}

	LogStringNoFormat(PBT_LOG, FormatString("%s m_autoUploadCreditsGlobal=%d IsPoller_12_11_6000_or_Higher=%d ncepAuto=%d cepAuto=%d watAuto=%d m_pbtPromoEnabled_global=%d PBFromGameEnabled=%d CEPOutEnabled=%d NCEPOutEnabled=%d senOptions2=0x%08X",
		__FUNCTION__, m_autoUploadCreditsGlobal, m_extensibleFlags.IsPoller_12_11_6000_or_Higher(), NcepAutoUpload(), CepAutoUpload(), WatAutoUpload(), m_pbtPromoEnabled_global, PBFromGameEnabled(), CEPOutEnabled(), NCEPOutEnabled(), m_senOption2Raw).c_str());

	return autoUploadCredits;
}

bool CConfig::NcepAutoUpload() const
{
	return (m_senOption2Raw & SO2_NCEP_AUTO_UPLOAD) == SO2_NCEP_AUTO_UPLOAD;
}

bool CConfig::CepAutoUpload() const
{
	return (m_senOption2Raw & SO2_CEP_AUTOPLOAD) == SO2_CEP_AUTOPLOAD;
}

bool CConfig::WatAutoUpload() const
{
	return (m_senOption2Raw & SO2_WAT_AUTOPLOAD) == SO2_WAT_AUTOPLOAD;
}

bool CConfig::JackpotA8Reset()
{
	return m_jackpotA8Reset;
}

bool CConfig::SetJackpotA8Reset ( bool resetToCredit )
{
	bool origValue = m_jackpotA8Reset;
	m_jackpotA8Reset = resetToCredit;
	m_nvramConfig.Set(m_jackpotA8ResetNVRAMOffset, (BYTE *)&m_jackpotA8Reset, sizeof(m_jackpotA8Reset));

	return origValue;
}

void CConfig::SetEGMProtocolState( EGMProtocolState protocolState )
{
	if(m_egmProtocolState != protocolState)
	{
		m_egmProtocolState = protocolState;
		if ( !m_eepromConfig.Set( EEPROM_EGM_PROTOCOL_STATE_OFFSET, (BYTE *)&m_egmProtocolState, EEPROM_EGM_PROTOCOL_STATE_SIZE ) )
			LogString(ERROR_LOG, "Could not write egm protocol state to EEPROM" );
	}
}

EGMProtocolState CConfig::GetEGMProtocolState( void )
{
	return m_egmProtocolState;
}

void CConfig::SetSentinelOnlineState( SentinelOnlineState state, bool bSetInPersistMemory)
{
	if(m_sentinelOnlineState != state)
	{
		m_sentinelOnlineState = state;
		if ( bSetInPersistMemory )
			if ( !m_eepromConfig.Set( EEPROM_SENTINEL_ONLINE_STATE_OFFSET, (BYTE *)&m_sentinelOnlineState, EEPROM_SENTINEL_ONLINE_STATE_SIZE ) )
				LogString(ERROR_LOG, "Could not write egm change in progress to EEPROM" );
	}
}

SentinelOnlineState CConfig::GetSentinelOnlineState( void )
{
	return m_sentinelOnlineState;
}

//Sets the global option for accumulating points for NCEP play
bool CConfig::SetAccruePointsForNCEP(bool accruePointsForNCEP)
{
	bool bOldVal = m_accruePointsForNCEP;

	m_accruePointsForNCEP = accruePointsForNCEP;
	m_nvramConfig.Set(m_accruePointsForNCEPNVRAMOffset, (BYTE*) &m_accruePointsForNCEP, sizeof(m_accruePointsForNCEP));

	return bOldVal;
}	

bool CConfig::GetAccruePointsForNCEP()
{
	//Both the Sen Option and the global option must be true.
	return m_accruePointsForNCEP && m_senOpAccruePointsForNCEP;
}

bool CConfig::GetGlobalAccruePointsForNCEP()
{
	return m_accruePointsForNCEP;
}

//Sets the global option for accumulating comp for NCEP play
bool CConfig::SetAccrueCompForNCEP(bool accrueCompForNCEP)
{
	bool bOldVal = m_accrueCompForNCEP;

	m_accrueCompForNCEP = accrueCompForNCEP;
	m_nvramConfig.Set(m_accrueCompForNCEPNVRAMOffset, (BYTE*) &m_accrueCompForNCEP, sizeof(m_accrueCompForNCEP));

	return bOldVal;
}	

bool CConfig::GetAccrueCompForNCEP()
{
	//Both the Sen Option and the global option must be true.
	return m_accrueCompForNCEP && m_senOpAccrueCompForNCEP;
}

bool CConfig::GetGlobalAccrueCompForNCEP()
{
	return m_accrueCompForNCEP;
}

//Sets the sentinel option for accumulating points for NCEP play
bool CConfig::SetSenOpAccruePointsForNCEP(bool accruePointsForNCEP)
{
	bool bOldVal = m_senOpAccruePointsForNCEP;

	m_senOpAccruePointsForNCEP = accruePointsForNCEP;
	m_nvramConfig.Set(m_senOpAccruePointsForNCEPNVRAMOffset, (BYTE*) &m_senOpAccruePointsForNCEP, sizeof(m_senOpAccruePointsForNCEP));

	return bOldVal;
}	

//Sets the sentinel option for accumulating comp for NCEP play
bool CConfig::SetSenOpAccrueCompForNCEP(bool accrueCompForNCEP)
{
	bool bOldVal = m_senOpAccrueCompForNCEP;

	m_senOpAccrueCompForNCEP = accrueCompForNCEP;
	m_nvramConfig.Set(m_senOpAccrueCompForNCEPNVRAMOffset, (BYTE*) &m_senOpAccrueCompForNCEP, sizeof(m_senOpAccrueCompForNCEP));

	return bOldVal;
}	

bool CConfig::GetGameInProgress() const
{
	return m_gameInProgress;
}

bool CConfig::SetGameInProgress(bool gameInProgress)
{
	bool bOldVal = m_gameInProgress;

	m_gameInProgress = gameInProgress;
	m_nvramConfig.Set(m_gameInProgressNVRAMOffset, (BYTE*) &m_gameInProgress, sizeof(m_gameInProgress));

	return bOldVal;
}

bool CConfig::GetServiceWindowOpen()
{
	return m_serviceWindowOpen || m_isLcd;
}

bool CConfig::SetServiceWindowOpen(bool serviceWindowOpen)
{
	bool bOldVal = m_serviceWindowOpen;

	m_serviceWindowOpen = serviceWindowOpen;
	m_nvramConfig.Set(m_serviceWindowOpenNVRAMOffset, (BYTE*) &m_serviceWindowOpen, sizeof(m_serviceWindowOpen));

	return bOldVal;
}

byte CConfig::SetTouchScreenProtocolType(byte touchScreenProtocolType)
{
	byte oldVal = m_touchScreenProtocolType;

	if(m_touchScreenProtocolType != touchScreenProtocolType)
	{
		m_touchScreenProtocolType = touchScreenProtocolType;

		m_eepromConfig.Set( EEPROM_TSPROTO_TYPE_OFFSET, (BYTE *)&m_touchScreenProtocolType, EEPROM_TSPROTO_TYPE_SIZE );
	}

	return oldVal;
}

byte CConfig::GetTouchScreenProtocolType()
{
	return(m_touchScreenProtocolType);
}

WORD CConfig::SetTouchScreenProtocolVendorId(WORD touchScreenProtocolVendorId)
{
	WORD oldVal = m_touchScreenProtocolVendorId;

	if(m_touchScreenProtocolVendorId != touchScreenProtocolVendorId)
	{
		m_touchScreenProtocolVendorId = touchScreenProtocolVendorId;

		m_eepromConfig.Set( EEPROM_TSPROTO_VENDOR_ID_OFFSET, (BYTE *)&m_touchScreenProtocolVendorId, EEPROM_TSPROTO_VENDOR_ID_SIZE );
	}

	return oldVal;
}

WORD CConfig::GetTouchScreenProtocolVendorId()
{
	return(m_touchScreenProtocolVendorId);
}

DWORD CConfig::SetTouchScreenProtocolBaud(DWORD touchScreenProtocolBaud)
{
	DWORD oldVal = m_touchScreenProtocolBaud;

	if(m_touchScreenProtocolBaud != touchScreenProtocolBaud)
	{
		m_touchScreenProtocolBaud = touchScreenProtocolBaud;

		m_eepromConfig.Set( EEPROM_TSPROTO_BAUD_OFFSET, (BYTE *)&m_touchScreenProtocolBaud, EEPROM_TSPROTO_BAUD_SIZE );
	}

	return oldVal;
}

DWORD CConfig::GetTouchScreenProtocolBaud()
{
	return(m_touchScreenProtocolBaud);
}

void CConfig::SetTouchScreenProtocolName(LPCTSTR touchScreenProtocolName)
{
	m_touchScreenProtocolName = touchScreenProtocolName;
	char tempTouchScreenProtocolName[SIZE_OF_TOUCH_SCREEN_PROTOCOL_NAME];
	CUtilities::ConvertCharsToChars( m_touchScreenProtocolName.c_str(), tempTouchScreenProtocolName, SIZE_OF_TOUCH_SCREEN_PROTOCOL_NAME);
	m_nvramConfig.Set(m_touchScreenProtocolNameNVRAMOffset, (BYTE *)tempTouchScreenProtocolName, SIZE_OF_TOUCH_SCREEN_PROTOCOL_NAME);
}

const std::string &CConfig::GetTouchScreenProtocolName()
{
	return(m_touchScreenProtocolName);
}

// Increments the Mixer reboot count and returns new count
byte CConfig::IncrementMixerRebootCount()
{
	SetMixerRebootCount(m_mixerRebootCount + 1);

	return m_mixerRebootCount;
}

// Sets the Mixer reboot count back to 0 and returns the previous value
byte CConfig::ResetMixerRebootCount()
{
	return SetMixerRebootCount( 0 );
}

// Sets the Mixer reboot count to the specified value and returns the previous value
byte CConfig::SetMixerRebootCount( byte rebootCount )
{
	byte oldVal = m_mixerRebootCount;

	if ( oldVal != rebootCount )
	{
		m_mixerRebootCount = rebootCount;
		m_eepromConfig.Set( EEPROM_MIXER_REBOOT_COUNT_OFFSET, (BYTE *)&m_mixerRebootCount, EEPROM_MIXER_REBOOT_COUNT_SIZE );
	}

	return oldVal;
}

// Gets the current mixer reboot count
byte CConfig::GetMixerRebootCount()
{
	return m_mixerRebootCount;
}


bool CConfig::SetVideoInputModeToAutoDetect( bool autoDetect )
{
	bool oldVal = m_autoDetectInputVideoMode;

	if ( oldVal != autoDetect )
	{
		m_autoDetectInputVideoMode = autoDetect;
		BYTE tempBYTE = m_autoDetectInputVideoMode ? EEPROM_VIDEO_INPUT_AUTO_DETECT_AUTO : EEPROM_VIDEO_INPUT_AUTO_DETECT_MANUAL;
		m_eepromConfig.Set( EEPROM_VIDEO_INPUT_AUTO_DETECT_OFFSET, &tempBYTE, EEPROM_VIDEO_INPUT_AUTO_DETECT_SIZE );
	}

	return oldVal;
}

bool CConfig::IsVideoInputModeAutoDetect() const
{
	return m_autoDetectInputVideoMode;
}

bool CConfig::SetVideoOutputModeToConfig( bool autoConfig, bool nativeConfig )
{
	byte oldVal = m_outputVideoModeConfig;
	BYTE tempBYTE = nativeConfig ? EEPROM_VIDEO_OUTPUT_AUTO_CONFIG_NATIVE :
		(autoConfig ? EEPROM_VIDEO_OUTPUT_AUTO_CONFIG_AUTO : EEPROM_VIDEO_INPUT_AUTO_DETECT_MANUAL);

	if ( oldVal != tempBYTE )
	{
		m_outputVideoModeConfig = tempBYTE;
		m_eepromConfig.Set( EEPROM_VIDEO_OUTPUT_AUTO_CONFIG_OFFSET, &tempBYTE, EEPROM_VIDEO_OUTPUT_AUTO_CONFIG_SIZE );
	}

	return oldVal;
}

bool CConfig::IsVideoOutputModeAutoConfig() const
{
	return EEPROM_VIDEO_OUTPUT_AUTO_CONFIG_AUTO == m_outputVideoModeConfig;
}

bool CConfig::IsVideoOutputModeNativeConfig() const
{
	return EEPROM_VIDEO_OUTPUT_AUTO_CONFIG_NATIVE == m_outputVideoModeConfig;
}

DesktopOrientationType CConfig::SetOrientationType( DesktopOrientationType rotationType )
{
	DesktopOrientationType oldVal = m_desktopOrientationType;

	if ( oldVal != rotationType )
	{
		m_desktopOrientationType = rotationType;
		BYTE tempBYTE = GetDesktopOrientationAsByte();
		m_eepromConfig.Set( EEPROM_DESKTOP_ORIENTATION_OFFSET, &tempBYTE, EEPROM_DESKTOP_ORIENTATION_SIZE );
	}

	return oldVal;
}

DesktopOrientationType CConfig::GetOrientationType() const
{
	return m_desktopOrientationType;
}

bool CConfig::SetUniverseEnabled(bool universeEnabled)
{
	bool bOldVal = m_universeEnabled;

	m_universeEnabled = universeEnabled;
	m_nvramConfig.Set(m_universeEnabledNVRAMOffset, (BYTE*) &m_universeEnabled, sizeof(m_universeEnabled));

	return bOldVal;
}

bool CConfig::GetUniverseEnabled(void) const
{
	return m_universeEnabled;
}

bool CConfig::SetMixedModeEnabled(bool mixedModeEnabled)
{
	bool bOldVal = m_mixedModeEnabled;

	m_mixedModeEnabled = mixedModeEnabled;
	m_nvramConfig.Set(m_mixedModeEnabledNVRAMOffset, (BYTE*) &m_mixedModeEnabled, sizeof(m_mixedModeEnabled));

	return bOldVal;
}

bool CConfig::GetMixedModeEnabled(void) const
{
	return m_mixedModeEnabled;
}

// CConfigUniversePropCodesField::SetUniversePropCodes.
//     IN universePropCodes - pointer to data to be used for Universe Penn property codes.
//         If buffer is NULL then a length of zero is used.
//     IN propCodeCount - number of codes in universePropCodes to be used for setting Universe Penn property codes.
//         If propCodeCount is negative then a propCodeCount of zero is used.
//         If propCodeCount is greater than SIZE_OF_VALID_CARDS_PREFIX then a propCodeCount of SIZE_OF_VALID_CARDS_PREFIX is used.
// Set the Universe Penn property codes, using propCodeCount codes of universePropCodes.
void CConfig::SetUniversePropCodes(const std::vector<WORD> &universePropCodes)
{
    m_universePropCodes = universePropCodes;
    int numOfPropCodesInNVRAM = min((int)universePropCodes.size(), NUMBER_OF_VALID_CARDS_PREFIX_IN_NVRAM);

    BYTE propCodesForNVRAM[NUMBER_OF_VALID_CARDS_PREFIX_IN_NVRAM * SIZE_OF_VALID_PROP_CODE];
    for (int propCodeIndex = 0; propCodeIndex < NUMBER_OF_VALID_CARDS_PREFIX_IN_NVRAM; propCodeIndex++)
    {
        WORD propCode = 0;
        if (propCodeIndex < numOfPropCodesInNVRAM)
        {
            propCode = universePropCodes[propCodeIndex];
            LogString(GENERAL, _T("Universe propcode stored in NVRAM: %c%c"), (char)(propCode/100), (char)(propCode%100));
        }

		memcpy_s(&propCodesForNVRAM[propCodeIndex * SIZE_OF_VALID_PROP_CODE], SIZE_OF_VALID_PROP_CODE, reinterpret_cast<BYTE *>(&propCode), SIZE_OF_VALID_PROP_CODE);
    }

    for (int propCodeIndex = numOfPropCodesInNVRAM; propCodeIndex < universePropCodes.size(); propCodeIndex++) 
    {
        LogString(GENERAL, _T("Universe propcode NOT stored in NVRAM: %c%c"), (char)(universePropCodes[propCodeIndex]/100), (char)(universePropCodes[propCodeIndex]%100));
    }
    
	m_nvramConfig.Set(m_numOfUniversePropCodesNVRAMOffset, (BYTE *) &numOfPropCodesInNVRAM, sizeof(numOfPropCodesInNVRAM),
			m_universePropCodesNVRAMOffset, (BYTE *)propCodesForNVRAM, sizeof(propCodesForNVRAM));
}

const std::vector<WORD> CConfig::GetUniversePropCodes() const
{
	return (m_universePropCodes);
}

/// <summary>
/// Set the Universe Penn (player) card OCR prefixes.
/// <para/>
/// If universePrefixes is NULL then a prefixCount of zero is used.
/// <para/>
/// If prefixCount is negative then a propCodeCount of zero is used.
/// If prefixCount is greater than SIZE_OF_VALID_CARDS_PREFIX then a prefixCount of SIZE_OF_VALID_CARDS_PREFIX is used.
/// </summary>
/// <param name="universePrefixes">IN - address of the (player) card OCR prefixes -- one byte per prefix.</param>
/// <param name="prefixCount">IN - number of prefixes in universePrefixes.</param>
void CConfig::SetUniversePrefixes(const std::vector<BYTE> &universePrefixes)
{
    m_universePlayerCardPrefixes = universePrefixes;
    int numberOfCardPrefixesInNVRAM = min((int)universePrefixes.size(), NUMBER_OF_VALID_CARDS_PREFIX_IN_NVRAM);

    // Copy the prefixes into NVRAM
    BYTE prefixForNVRAM[NUMBER_OF_VALID_CARDS_PREFIX_IN_NVRAM];
    for (int prefixIndex = 0; prefixIndex < NUMBER_OF_VALID_CARDS_PREFIX_IN_NVRAM; prefixIndex++)
    {
        byte prefix = 0;
        if (prefixIndex < numberOfCardPrefixesInNVRAM)
        {
            prefix = universePrefixes[prefixIndex];
            LogString(GENERAL, _T("Universe prefix stored in NVRAM: %x"), prefix);
        }

        prefixForNVRAM[prefixIndex] = prefix;
    }

    for (int prefixIndex = numberOfCardPrefixesInNVRAM; prefixIndex < universePrefixes.size(); prefixIndex++) 
    {
        LogString(GENERAL, _T("Universe prefix NOT stored in NVRAM: %x"), universePrefixes[prefixIndex]);
    }

    m_nvramConfig.Set(m_universePlayerPrefixesNVRAMOffset, (BYTE *)prefixForNVRAM, NUMBER_OF_VALID_CARDS_PREFIX_IN_NVRAM);

}

const std::vector<BYTE> CConfig::GetUniverseCardPrefixes() const
{
	return (m_universePlayerCardPrefixes);
}

//Sets the features flags raw data
DWORD CConfig::SetFeatureFlagsRaw( DWORD featureFlagsRaw )
{
	DWORD oldValue = m_featureFlagsRaw;

	m_featureFlagsRaw = featureFlagsRaw;
	m_nvramConfig.Set( m_featureFlagsRawNVRAMOffset, (BYTE *)&m_featureFlagsRaw, sizeof( m_featureFlagsRaw ) );

	return oldValue;
}

//Gets the features flag raw data
DWORD CConfig::GetFeatureFlagsRaw( void )
{
	return m_featureFlagsRaw;
}

//sets the atomic redemptions
bool CConfig::SetAtomicRedemptions( bool atomicRedemptions )
{
	bool bOldVal = m_atomicRedemptions;

	m_atomicRedemptions = atomicRedemptions;
	m_nvramConfig.Set(m_atomicRedemptionsNVRAMOffset, (BYTE *)&m_atomicRedemptions, sizeof(m_atomicRedemptions));

	return bOldVal;
}

//returns atomic redemptions flag
bool CConfig::AtomicRedemptions()
{
	return m_atomicRedemptions;
}

bool CConfig::SetJackpotExternalBonus(bool jackpotExternalBonus)
{
	bool bOldVal = m_jackpotExternalBonus;

	m_jackpotExternalBonus = jackpotExternalBonus;
	m_nvramConfig.Set(m_jackpotExternalBonusNVRAMOffset, (BYTE *)&m_jackpotExternalBonus, sizeof(m_jackpotExternalBonus));

	return bOldVal;
}

bool CConfig::JackpotExternalBonus()
{
	return m_jackpotExternalBonus;
}

bool CConfig::SetMultiGameMultiDenom(bool multiGameMultiDenom)
{
	bool bOldVal = m_multiGameMultiDenom;

	m_multiGameMultiDenom = multiGameMultiDenom;
	m_nvramConfig.Set(m_multiGameMultiDenomNVRAMOffset, (BYTE *)&m_multiGameMultiDenom, sizeof(m_multiGameMultiDenom));

	return bOldVal;
}

bool CConfig::MultiGameMultiDenom()
{
	if(m_multiGameMultiDenom)
		LogString(POLLER_LOG, "MultiGameMultiDenom: yes");
	else
		LogString(POLLER_LOG, "MultiGameMultiDenom: no");
	
	return m_multiGameMultiDenom;
}

//returns the features flags
void CConfig::GetFeatureFlags(bool& atomicRedemptions, bool& jackpotExternalBonus, bool &externalCreditTogame , bool& multiGameMultiDenom)
{
	atomicRedemptions = m_atomicRedemptions;
	jackpotExternalBonus = m_jackpotExternalBonus;
    externalCreditTogame = m_ExternalCreditToGameEnabled;
	multiGameMultiDenom = m_multiGameMultiDenom;
}

//This sets the features flags only used for atomicRedemptions now
bool CConfig::SetFeatureFlags(bool atomicRedemptions, bool jackpotExternalBonus, bool externalCreditTogame, bool multiGameMultiDenom)
{
	bool origValue = m_atomicRedemptions;
	m_atomicRedemptions = atomicRedemptions;
	m_jackpotExternalBonus = jackpotExternalBonus;
    m_ExternalCreditToGameEnabled = externalCreditTogame;
	m_multiGameMultiDenom = multiGameMultiDenom;
	m_nvramConfig.Set(m_atomicRedemptionsNVRAMOffset, (BYTE *)&m_atomicRedemptions, sizeof(m_atomicRedemptions));
	m_nvramConfig.Set(m_jackpotExternalBonusNVRAMOffset, (BYTE *)&m_jackpotExternalBonus, sizeof(m_jackpotExternalBonus));
	m_nvramConfig.Set(m_externalCreditToGameNVRAMOffset, (BYTE *)&m_ExternalCreditToGameEnabled, sizeof(m_ExternalCreditToGameEnabled));
	LogString(PBT_LOG, "On offset %d in Nvram: External Credit to game enbale flag from poller is :%d ", m_externalCreditToGameNVRAMOffset, m_ExternalCreditToGameEnabled);

	m_nvramConfig.Set(m_multiGameMultiDenomNVRAMOffset, (BYTE *)&m_multiGameMultiDenom, sizeof(m_multiGameMultiDenom));

	return origValue;
}

// CConfig::ValidateCardReaderTrackNumber()
//     IN/OUT cardReaderTrackNumber - card reader track number to make valid, if not already valid.
//     RETURN - true if cardReaderTrackNumber was changed, false otherwise.
// Make cardReaderTrackNumber valid if it is not already valid.
// If cardReaderTrackNumber is not valid then set cardReaderTrackNumber to the default value.
bool CConfig::ValidateCardReaderTrackNumber(BYTE &cardReaderTrackNumber)
{
	bool returnValue = false;

	if (cardReaderTrackNumber == CARD_READER_TRACK_1 ||
		cardReaderTrackNumber == CARD_READER_TRACK_2)
	{
		// The value is already valid.  Return the value unchanged.
	}
	else
	{
		// The value is not valid.  Set the value to the default value before returning.
		cardReaderTrackNumber = CARD_READER_TRACK_NUMBER_DEFAULT;
		returnValue = true;
	}

	return returnValue;
}

BYTE CConfig::SetCardReaderTrackNumber(BYTE cardReaderTrackNumber)
{
	BYTE bOldVal = m_cardReaderTrackNumber;

	ValidateCardReaderTrackNumber(cardReaderTrackNumber);
	m_cardReaderTrackNumber = cardReaderTrackNumber;
	m_nvramConfig.Set(m_cardReaderTrackNumberNVRAMOffset, &m_cardReaderTrackNumber, sizeof(m_cardReaderTrackNumber));

	return bOldVal;
}

BYTE CConfig::CardReaderTrackNumber() const
{
	return m_cardReaderTrackNumber;
}

/// <summary>
/// Gets the value from NVRAM.
/// Verifies the value and sets it to the default value if invalid.
/// </summary>
/// <param name="nvramOffset">IN - The nvram offset of the value.</param>
/// <param name="value">OUT - The value read or the default value if the value read was invalid.</param>
/// <param name="defaultValue">IN - default value to use if the value read was invalid.</param>
/// <param name="invalidCount">IN/OUT - Incremented if the value read was invalid.</param>
void CConfig::GetValue(DWORD nvramOffset, bool &value, bool defaultValue, int &invalidCount)
{
	m_nvramConfig.Get(nvramOffset, (BYTE *)&value, sizeof(value));

	if (*(BYTE *)&value > (BYTE)true)
	{
		value = defaultValue;
		++invalidCount;
	}
}

/// <summary>
/// Gets the value from NVRAM.
/// Verifies the value and sets it to the default value if invalid.
/// </summary>
/// <param name="nvramOffset">IN - The nvram offset of the value.</param>
/// <param name="value">OUT - The value read or the default value if the value read was invalid.</param>
/// <param name="defaultValue">IN - default value to use if the value read was invalid.</param>
/// <param name="minimumValue">IN - minimum valid value.</param>
/// <param name="maximumValue">IN - maximum valid value.</param>
/// <param name="invalidCount">IN/OUT - Incremented if the value read was invalid.</param>
void CConfig::GetValue(DWORD nvramOffset,
	WORD &value,
	WORD defaultValue,
	WORD minimumValue,
	WORD maximumValue,
	int &invalidCount)
{
	m_nvramConfig.Get(nvramOffset, (BYTE *)&value, sizeof(value));

	if (value < minimumValue || maximumValue < value)
	{
		value = defaultValue;
		++invalidCount;
	}
}

// #pragma region configuration extension members
/// <summary>
/// Gets the configuration extension member variables from NVRAM.
/// Verifies variables and sets them to default values if invalid.
/// </summary>
void CConfig::GetConfigExt2()
{
	int invalidCount(0);

	GetValue(m_notificationShowPromotionNVRAMOffset,
		m_notificationShowPromotion,
		CConfigExtData::DEFAULT_NOTIFICATION_SHOW_PROMOTION,
		CConfigExtData::MINIMUM_NOTIFICATION_SHOW_PROMOTION,
		CConfigExtData::MAXIMUM_NOTIFICATION_SHOW_PROMOTION,
		invalidCount);

	GetValue(m_notificationPlayerTrackingNVRAMOffset,
		m_notificationPlayerTracking,
		CConfigExtData::DEFAULT_NOTIFICATION_PLAYER_TRACKING,
		CConfigExtData::MINIMUM_NOTIFICATION_PLAYER_TRACKING,
		CConfigExtData::MAXIMUM_NOTIFICATION_PLAYER_TRACKING,
		invalidCount);

	GetValue(m_customButton1enableNVRAMOffset,
		m_customButton1enable,
		CConfigExtData::DEFAULT_CUSTOM_BUTTON_1_ENABLE,
		invalidCount);

	GetValue(m_customMenuButton1CountNVRAMOffset,
		m_customMenuButton1Count,
		CConfigExtData::DEFAULT_CUSTOM_MENU_BUTTON_1_COUNT,
		CConfigExtData::MINIMUM_CUSTOM_MENU_BUTTON_1_COUNT,
		CConfigExtData::MAXIMUM_CUSTOM_MENU_BUTTON_1_COUNT,
		invalidCount);

	GetValue(m_customButton2enableNVRAMOffset,
		m_customButton2enable,
		CConfigExtData::DEFAULT_CUSTOM_BUTTON_2_ENABLE,
		invalidCount);

	GetValue(m_customMenuButton2CountNVRAMOffset,
		m_customMenuButton2Count,
		CConfigExtData::DEFAULT_CUSTOM_MENU_BUTTON_2_COUNT,
		CConfigExtData::MINIMUM_CUSTOM_MENU_BUTTON_2_COUNT,
		CConfigExtData::MAXIMUM_CUSTOM_MENU_BUTTON_2_COUNT,
		invalidCount);

	GetValue(m_sessionActiveNegligibleCreditsTimeoutSecondsNVRAMOffset,
		m_sessionActiveNegligibleCreditsTimeoutSeconds,
		CConfigExtData::DEFAULT_SESSION_ACTIVE_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS,
		CConfigExtData::MINIMUM_SESSION_ACTIVE_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS,
		CConfigExtData::MAXIMUM_SESSION_ACTIVE_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS,
		invalidCount);

	GetValue(m_sessionActiveNonNegligibleCreditsTimeoutSecondsNVRAMOffset,
		m_sessionActiveNonNegligibleCreditsTimeoutSeconds,
		CConfigExtData::DEFAULT_SESSION_ACTIVE_NON_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS,
		CConfigExtData::MINIMUM_SESSION_ACTIVE_NON_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS,
		CConfigExtData::MAXIMUM_SESSION_ACTIVE_NON_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS,
		invalidCount);

	GetValue(m_resortMenuButtonCountNVRAMOffset,
		m_resortMenuButtonCount,
		CConfigExtData::DEFAULT_RESORT_MENU_BUTTON_COUNT,
		CConfigExtData::MINIMUM_RESORT_MENU_BUTTON_COUNT,
		CConfigExtData::MAXIMUM_RESORT_MENU_BUTTON_COUNT,
		invalidCount);

	byte configExt2CheckByte;
	m_nvramConfig.Get(m_configExt2CheckByteNVRAMOffset, &configExt2CheckByte, sizeof(configExt2CheckByte));
	bool checkByteValid(configExt2CheckByte == m_configExt2CheckByteValue);

	if (!checkByteValid || invalidCount > 1)
	{
		// If multiple values are invalid then set the entire group to defaults,
		// as values that happen to be in valid ranges might not be configured correctly.
		SetConfigExt2Defaults();
	}
}

void CConfig::LoadConfigExtFile()
{
	LogString(POLLER_LOG, "%s: %d", __FUNCTION__, __LINE__);
	
	CConfigExtFile configExtFile;
	bool ok(configExtFile.Start());

	if (ok)
	{
		LoadConfigExtFromFileValues(configExtFile);
		configExtFile.DeleteConfigFile();
	}
}

/// <summary>
/// Loads the configuration extensions #1 from file values.
/// </summary>
/// <param name="configExtFile">IN - The configuration extensions file.</param>
void CConfig::LoadConfigExtFromFileValues(const CConfigExtFile &configExtFile)
{
	{
		WORD gameListChunkSize;
		if (configExtFile.GetGameListChunkSize(gameListChunkSize))
		{
			SetGameListChunkSize(gameListChunkSize);
		}
	}

	{
		WORD negligibleCreditsThreshold;
		if (configExtFile.GetNegligibleCreditsThreshold(negligibleCreditsThreshold))
		{
			SetNegligibleCreditsThreshold(negligibleCreditsThreshold);
		}
	}

	{
		bool employeeOverPlayerIdleCardOut;
		if (configExtFile.GetEmployeeOverPlayerIdleCardOut(employeeOverPlayerIdleCardOut))
		{
			SetEmployeeOverPlayerIdleCardOut(employeeOverPlayerIdleCardOut);
		}
	}

	{
		WORD employeeIdleCardOutSeconds;
		if (configExtFile.GetEmployeeIdleCardOutSeconds(employeeIdleCardOutSeconds))
		{
			SetEmployeeIdleCardOutSeconds(employeeIdleCardOutSeconds);
		}
	}

	{
		bool playerRecardEmployeeCardOut;
		if (configExtFile.GetPlayerRecardEmployeeCardOut(playerRecardEmployeeCardOut))
		{
			SetPlayerRecardEmployeeCardOut(playerRecardEmployeeCardOut);
		}
	}

	{
		WORD notificationForShowPromotionTime;
		if (configExtFile.GetNotificationForShowPromotionTime(notificationForShowPromotionTime))
		{
			SetNotificationForShowPromotionTime(notificationForShowPromotionTime);
		}
	}

	{
		WORD notificationForPlayerTrackingTime;
		if (configExtFile.GetNotificationForPlayerTrackingTime(notificationForPlayerTrackingTime))
		{
			SetNotificationForPlayerTrackingTime(notificationForPlayerTrackingTime);
		}
	}

	{
		bool customButton1Enabled;
		if (configExtFile.GetCustomButton1Enabled(customButton1Enabled))
		{
			SetCustomButton1Enabled(customButton1Enabled);
		}
	}

	{
		bool customButton2Enabled;
		if (configExtFile.GetCustomButton2Enabled(customButton2Enabled))
		{
			SetCustomButton2Enabled(customButton2Enabled);
		}
	}

	{
		WORD customMenuButton1Count;
		if (configExtFile.GetCustomMenuButton1Count(customMenuButton1Count))
		{
			SetCustomMenuButton1Count(customMenuButton1Count);
		}
	}

	{
		WORD customMenuButton2Count;
		if (configExtFile.GetCustomMenuButton2Count(customMenuButton2Count))
		{
			SetCustomMenuButton2Count(customMenuButton2Count);
		}
	}

	{
		WORD sessionActiveNegligibleCreditsTimeoutSeconds;
		if (configExtFile.GetSessionActiveNegligibleCreditsTimeoutSeconds(sessionActiveNegligibleCreditsTimeoutSeconds))
		{
			SetSessionActiveNegligibleCreditsTimeoutSeconds(sessionActiveNegligibleCreditsTimeoutSeconds);
		}
	}

	{
		WORD sessionActiveNonNegligibleCreditsTimeoutSeconds;
		if (configExtFile.GetSessionActiveNonNegligibleCreditsTimeoutSeconds(sessionActiveNonNegligibleCreditsTimeoutSeconds))
		{
			SetSessionActiveNonNegligibleCreditsTimeoutSeconds(sessionActiveNonNegligibleCreditsTimeoutSeconds);
		}
	}

	{
		WORD resortMenuButtonCount;
		if (configExtFile.GetResortMenuButtonCount(resortMenuButtonCount))
		{
			SetResortMenuButtonCount(resortMenuButtonCount);
		}
	}

	{
		bool rebootUiRestartOverride;
		if (configExtFile.GetRebootUiRestartOverride(rebootUiRestartOverride))
		{
			SetRebootUiRestartOverride(rebootUiRestartOverride);
		}
	}

	{
		WORD priority;

		if (configExtFile.GetBonusCounterNotificationPriority(priority))
		{
			SetBonusCounterNotificationPriority(priority);
		}

		if (configExtFile.GetBonusCounternRichLevelPriority(priority))
		{
			SetBonusCounternRichLevelPriority(priority);
		}

		if (configExtFile.GetBonusCounterSplashdownPriority(priority))
		{
			SetBonusCounterSplashdownPriority(priority);
		}

		if (configExtFile.GetBonusCounterProgressiveOdometerPriority(priority))
		{
			SetBonusCounterProgressiveOdometerPriority(priority);
		}

		if (configExtFile.GetBonusCounterPtePriority(priority))
		{
			SetBonusCounterPtePriority(priority);
		}

		if (configExtFile.GetBonusCounterScatterCountdownTimerPriority(priority))
		{
			SetBonusCounterScatterCountdownTimerPriority(priority);
		}

		if (configExtFile.GetBonusCounterPrizePriority(priority))
		{
			SetBonusCounterPrizePriority(priority);
		}

		if (configExtFile.GetBonusCounterPlayXGetYPriority(priority))
		{
			SetBonusCounterPlayXGetYPriority(priority);
		}
	}
}

/// <summary>
/// Sets configuration extension #1 member variables to default values.
/// </summary>
void CConfig::SetConfigExt1Defaults()
{
	SetNegligibleCreditsThreshold(CConfigExtData::DEFAULT_NEGLIGIBLE_CREDITS_THRESHOLD);
	SetEmployeeOverPlayerIdleCardOut(CConfigExtData::DEFAULT_EMPLOYEE_OVER_PLAYER_IDLE_CARD_OUT);
	SetEmployeeIdleCardOutSeconds(CConfigExtData::DEFAULT_EMPLOYEE_IDLE_CARD_OUT_SECONDS);
	SetPlayerRecardEmployeeCardOut(CConfigExtData::DEFAULT_PLAYER_RECARD_EMPLOYEE_CARD_OUT);
}

WORD CConfig::SetNegligibleCreditsThreshold(WORD negligibleCreditsThreshold)
{
	WORD oldValue = m_negligibleCreditsThreshold;
	m_negligibleCreditsThreshold = negligibleCreditsThreshold;
	m_nvramConfig.Set(m_negligibleCreditsThresholdNVRAMOffset, (BYTE *)&m_negligibleCreditsThreshold, sizeof(m_negligibleCreditsThreshold));
	return oldValue;
}

WORD CConfig::GetNegligibleCreditsThreshold() const
{
	return m_negligibleCreditsThreshold;
}

bool CConfig::SetEmployeeOverPlayerIdleCardOut(bool employeeOverPlayerIdleCardOut)
{
	bool oldValue = m_employeeOverPlayerIdleCardOut;
	m_employeeOverPlayerIdleCardOut = employeeOverPlayerIdleCardOut;
	m_nvramConfig.Set(m_employeeOverPlayerIdleCardOutNVRAMOffset, (BYTE *)&m_employeeOverPlayerIdleCardOut, sizeof(m_employeeOverPlayerIdleCardOut));
	return oldValue;
}

bool CConfig::GetEmployeeOverPlayerIdleCardOut() const
{
	return m_employeeOverPlayerIdleCardOut;
}

WORD CConfig::SetEmployeeIdleCardOutSeconds(WORD employeeIdleCardOutSeconds)
{
	WORD oldValue = m_employeeIdleCardOutSeconds;
	m_employeeIdleCardOutSeconds = employeeIdleCardOutSeconds;
	m_nvramConfig.Set(m_employeeIdleCardOutSecondsNVRAMOffset, (BYTE *)&m_employeeIdleCardOutSeconds, sizeof(m_employeeIdleCardOutSeconds));
	return oldValue;
}

WORD CConfig::GetEmployeeIdleCardOutSeconds() const
{
	return m_employeeIdleCardOutSeconds;
}

bool CConfig::SetPlayerRecardEmployeeCardOut(bool playerRecardEmployeeCardOut)
{
	bool oldValue = m_playerRecardEmployeeCardOut;
	m_playerRecardEmployeeCardOut = playerRecardEmployeeCardOut;
	m_nvramConfig.Set(m_playerRecardEmployeeCardOutNVRAMOffset, (BYTE *)&m_playerRecardEmployeeCardOut, sizeof(m_playerRecardEmployeeCardOut));
	return oldValue;
}

bool CConfig::GetPlayerRecardEmployeeCardOut() const
{
	return m_playerRecardEmployeeCardOut;
}

WORD CConfig::SetGameListChunkSize(WORD gameListChunkSize)
{
	WORD oldValue = m_gameListChunkSize;
	m_gameListChunkSize = gameListChunkSize;
	m_nvramConfig.Set(m_gameListChunkSizeOffset, (BYTE *)&m_gameListChunkSize, sizeof(m_gameListChunkSize));
	return oldValue;
}

WORD CConfig::GetGameListChunkSize() const
{
	return m_gameListChunkSize;
}

/// <summary>
/// Sets configuration extension #2 member variables to default values.
/// </summary>
void CConfig::SetConfigExt2Defaults()
{
	SetNotificationForShowPromotionTime(CConfigExtData::DEFAULT_NOTIFICATION_SHOW_PROMOTION);
	SetNotificationForPlayerTrackingTime(CConfigExtData::DEFAULT_NOTIFICATION_PLAYER_TRACKING);
	SetCustomButton1Enabled(CConfigExtData::DEFAULT_CUSTOM_BUTTON_1_ENABLE);
	SetCustomButton2Enabled(CConfigExtData::DEFAULT_CUSTOM_BUTTON_2_ENABLE);
	SetCustomMenuButton1Count(CConfigExtData::DEFAULT_CUSTOM_MENU_BUTTON_1_COUNT);
	SetCustomMenuButton2Count(CConfigExtData::DEFAULT_CUSTOM_MENU_BUTTON_2_COUNT);
	SetSessionActiveNegligibleCreditsTimeoutSeconds(
		CConfigExtData::DEFAULT_SESSION_ACTIVE_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS);
	SetSessionActiveNonNegligibleCreditsTimeoutSeconds(
		CConfigExtData::DEFAULT_SESSION_ACTIVE_NON_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS);
	SetResortMenuButtonCount(CConfigExtData::DEFAULT_RESORT_MENU_BUTTON_COUNT);

	byte configExt2CheckByte(m_configExt2CheckByteValue);
	m_nvramConfig.Set(m_configExt2CheckByteNVRAMOffset, &configExt2CheckByte, sizeof(configExt2CheckByte));
}

WORD CConfig::GetNotificationForShowPromotionTime() const
{
	return m_notificationShowPromotion;
}

WORD CConfig::SetNotificationForShowPromotionTime(WORD showPromotionTime)
{
	// Set only valid values.
	if (showPromotionTime < CConfigExtData::MINIMUM_NOTIFICATION_SHOW_PROMOTION ||
		showPromotionTime > CConfigExtData::MAXIMUM_NOTIFICATION_SHOW_PROMOTION)
	{
		showPromotionTime = CConfigExtData::DEFAULT_NOTIFICATION_SHOW_PROMOTION;
	}

	WORD oldValue = m_notificationShowPromotion;
	m_notificationShowPromotion = showPromotionTime;
	m_nvramConfig.Set(m_notificationShowPromotionNVRAMOffset, (BYTE *)&m_notificationShowPromotion, sizeof(m_notificationShowPromotion));
	return oldValue;
}

WORD CConfig::GetNotificationForPlayerTrackingTime() const
{
	return m_notificationPlayerTracking;
}

WORD CConfig::SetNotificationForPlayerTrackingTime(WORD playerTrackingTime)
{
	// Set only valid values.
	if (playerTrackingTime < CConfigExtData::MINIMUM_NOTIFICATION_PLAYER_TRACKING ||
		playerTrackingTime > CConfigExtData::MAXIMUM_NOTIFICATION_PLAYER_TRACKING)
	{
		playerTrackingTime = CConfigExtData::DEFAULT_NOTIFICATION_PLAYER_TRACKING;
	}

	WORD oldValue = m_notificationPlayerTracking;
	m_notificationPlayerTracking = playerTrackingTime;
	m_nvramConfig.Set(m_notificationPlayerTrackingNVRAMOffset, (BYTE *)&m_notificationPlayerTracking, sizeof(m_notificationPlayerTracking));
	return oldValue;
}

bool CConfig::GetCustomButton1Enabled() const
{
	return m_customButton1enable;
}

bool CConfig::SetCustomButton1Enabled(bool enable)
{
	bool oldValue = m_customButton1enable;
	m_customButton1enable = enable;
	m_nvramConfig.Set(m_customButton1enableNVRAMOffset, (BYTE *)&m_customButton1enable, sizeof(m_customButton1enable));
	return oldValue;
}

bool CConfig::GetCustomButton2Enabled() const
{
	return m_customButton2enable;
}

bool CConfig::SetCustomButton2Enabled(bool enable)
{
	bool oldValue = m_customButton2enable;
	m_customButton2enable = enable;
	m_nvramConfig.Set(m_customButton2enableNVRAMOffset, (BYTE *)&m_customButton2enable, sizeof(m_customButton2enable));
	return oldValue;
}

WORD CConfig::GetResortMenuButtonCount() const
{
	return m_resortMenuButtonCount;
}

WORD CConfig::SetResortMenuButtonCount(WORD count)
{
	// Set only valid values.
	if (count < CConfigExtData::MINIMUM_RESORT_MENU_BUTTON_COUNT ||
		count > CConfigExtData::MAXIMUM_RESORT_MENU_BUTTON_COUNT)
	{
		count = CConfigExtData::DEFAULT_RESORT_MENU_BUTTON_COUNT;
	}

	WORD oldValue = m_resortMenuButtonCount;
	m_resortMenuButtonCount = count;
	m_nvramConfig.Set(m_resortMenuButtonCountNVRAMOffset, (BYTE *)&m_resortMenuButtonCount, sizeof(m_resortMenuButtonCount));
	return oldValue;
}

WORD CConfig::GetCustomMenuButton1Count() const
{
	return m_customMenuButton1Count;
}

WORD CConfig::SetCustomMenuButton1Count(WORD count)
{
	// Set only valid values.
	if (count > CConfigExtData::MAXIMUM_CUSTOM_MENU_BUTTON_1_COUNT)
	{
		count = CConfigExtData::DEFAULT_CUSTOM_MENU_BUTTON_1_COUNT;
	}

	WORD oldValue = m_customMenuButton1Count;
	m_customMenuButton1Count = count;
	m_nvramConfig.Set(m_customMenuButton1CountNVRAMOffset, (BYTE *)&m_customMenuButton1Count, sizeof(m_customMenuButton1Count));
	return oldValue;
}

WORD CConfig::GetCustomMenuButton2Count() const
{
	return m_customMenuButton2Count;
}

WORD CConfig::SetCustomMenuButton2Count(WORD count)
{
	// Set only valid values.
	if (count > CConfigExtData::MAXIMUM_CUSTOM_MENU_BUTTON_2_COUNT)
	{
		count = CConfigExtData::DEFAULT_CUSTOM_MENU_BUTTON_2_COUNT;
	}

	WORD oldValue = m_customMenuButton2Count;
	m_customMenuButton2Count = count;
	m_nvramConfig.Set(m_customMenuButton2CountNVRAMOffset, (BYTE *)&m_customMenuButton2Count, sizeof(m_customMenuButton2Count));
	return oldValue;
}


WORD CConfig::SetSessionActiveNegligibleCreditsTimeoutSeconds(WORD sessionActiveNegligibleCreditsTimeoutSeconds)
{
	WORD oldValue = m_sessionActiveNegligibleCreditsTimeoutSeconds;
	m_sessionActiveNegligibleCreditsTimeoutSeconds = sessionActiveNegligibleCreditsTimeoutSeconds;
	m_nvramConfig.Set(m_sessionActiveNegligibleCreditsTimeoutSecondsNVRAMOffset, (BYTE *)&m_sessionActiveNegligibleCreditsTimeoutSeconds, sizeof(m_sessionActiveNegligibleCreditsTimeoutSeconds));
	return oldValue;
}

WORD CConfig::GetSessionActiveNegligibleCreditsTimeoutSeconds() const
{
	return m_sessionActiveNegligibleCreditsTimeoutSeconds;
}

WORD CConfig::SetSessionActiveNonNegligibleCreditsTimeoutSeconds(WORD sessionActiveNonNegligibleCreditsTimeoutSeconds)
{
	WORD oldValue = m_sessionActiveNonNegligibleCreditsTimeoutSeconds;
	m_sessionActiveNonNegligibleCreditsTimeoutSeconds = sessionActiveNonNegligibleCreditsTimeoutSeconds;
	m_nvramConfig.Set(m_sessionActiveNonNegligibleCreditsTimeoutSecondsNVRAMOffset, (BYTE *)&m_sessionActiveNonNegligibleCreditsTimeoutSeconds, sizeof(m_sessionActiveNonNegligibleCreditsTimeoutSeconds));
	return oldValue;
}

WORD CConfig::GetSessionActiveNonNegligibleCreditsTimeoutSeconds() const
{
	return m_sessionActiveNonNegligibleCreditsTimeoutSeconds;
}

// #pragma endregion configuration extension members

/// <summary>
/// Sets configuration extension #3 member variables to default values.
/// </summary>
void CConfig::SetConfigExt3Defaults()
{
	SetUJPEnabled(CConfigExtData::DEFAULT_UJP_ENABLED);
	SetMiniVirtualSessionEnabled(CConfigExtData::DEFAULT_MINI_VIRTUAL_SESSION_ENABLED);
	SetOneLinkProgressivesEnabled(CConfigExtData::DEFAULT_ONE_LINK_PROGRESSIVES_ENABLED);

	// Set extension check byte valid only after setting all other extension members to valid values.
	byte configExt3CheckByte(m_configExt3CheckByteValue); // CNVRAMConfig::Set() needs a read/write variable.
	m_nvramConfig.Set(m_configExt3CheckByteNVRAMOffset, &configExt3CheckByte, sizeof(m_configExt3CheckByteValue));
}

void CConfig::SetConfigExt4Defaults()
{
	SetRebootUiRestartOverride(CConfigExtData::DEFAULT_REBOOT_UI_RESTART_OVERRIDE_ENABLE);

	// Set extension check byte valid only after setting all other extension members to valid values.
	byte configExt4CheckByte(m_configExt4CheckByteValue); // CNVRAMConfig::Set() needs a read/write variable.
	m_nvramConfig.Set(m_configExt4CheckByteNVRAMOffset, &configExt4CheckByte, sizeof(m_configExt4CheckByteValue));
}

void CConfig::SetConfigExt5Defaults()
{
	SetDisplayPlayerProfileButton(false);

	// Set extension check byte valid only after setting all other extension members to valid values.
	byte configExt5CheckByte(m_configExt5CheckByteValue); // CNVRAMConfig::Set() needs a read/write variable.
	m_nvramConfig.Set(m_configExt5CheckByteNVRAMOffset, &configExt5CheckByte, sizeof(m_configExt5CheckByteValue));
}

void CConfig::SetConfigExt6Defaults()
{
	SetMobileConnectCommunicationEnabled(false);

	SetBonusCounterNotificationPriority(CConfigExtData::DEFAULT_NOTIFICATION_BOX_PRIORITY);
	SetBonusCounternRichLevelPriority(CConfigExtData::DEFAULT_NRICH_PRIORITY);
	SetBonusCounterPrizePriority(CConfigExtData::DEFAULT_PRIZE_ODOMETER_PRIORITY);
	SetBonusCounterProgressiveOdometerPriority(CConfigExtData::DEFAULT_PROGRESSIVE_ODOMETER_PRIORITY);
	SetBonusCounterPtePriority(CConfigExtData::DEFAULT_PTE_PRIORITY);
	SetBonusCounterScatterCountdownTimerPriority(CConfigExtData::DEFAULT_SCATTER_COUNTDOWN_TIMER_PRIORITY);
	SetBonusCounterSplashdownPriority(CConfigExtData::DEFAULT_SPLASHDOWN_PRIORITY);

	// Set extension check byte valid only after setting all other extension members to valid values.
	byte configExt6CheckByte(m_configExt6CheckByteValue); // CNVRAMConfig::Set() needs a read/write variable.
	m_nvramConfig.Set(m_configExt6CheckByteNVRAMOffset, &configExt6CheckByte, sizeof(m_configExt6CheckByteValue));
}

void CConfig::SetConfigExt7Defaults()
{
	SetGlobalExcessiveVoucherOutEnabledFlag(false);
	SetExcessiveVoucherOutThreshold(0);
	SetTotalBillInThreshold(0);
	SetCoinInThreshold(0);
	SetCoinOutThreshold(0);
	SetBonusCounterPlayXGetYPriority(CConfigExtData::DEFAULT_PLAYXGETY_PRIORITY);
	SetRemoteDebuggingEnabled(false);

	// Set extension check byte valid only after setting all other extension members to valid values.
	byte configExt7CheckByte(m_configExt7CheckByteValue); // CNVRAMConfig::Set() needs a read/write variable.
	m_nvramConfig.Set(m_configExt7CheckByteNVRAMOffset, &configExt7CheckByte, sizeof(m_configExt7CheckByteValue));
}

void CConfig::SetExternalCreditTransferDefault()
{
	SetExternalCreditToGame(false);
	byte externalCreditTransferCheckByteValue(m_externalCreditTransferCheckByteValue); // CNVRAMConfig::Set() needs a read/write variable.
	m_nvramConfig.Set(m_externalCreditTransferCheckByteOffset, &externalCreditTransferCheckByteValue, sizeof(m_externalCreditTransferCheckByteValue));
}

bool CConfig::GetUJPEnabled() const
{
	return m_ujpEnabled;
}

bool CConfig::SetUJPEnabled(bool ujpEnabled)
{
	bool oldValue = m_ujpEnabled;
	m_ujpEnabled = ujpEnabled;
	m_nvramConfig.Set(m_ujpEnabledNVRAMOffset, (BYTE *)&m_ujpEnabled, sizeof(m_ujpEnabled));

	// jacpot a8 reset must be set to false if ujp is enabled
	if (ujpEnabled)
	{
		m_jackpotA8Reset = false;
		m_nvramConfig.Set(m_jackpotA8ResetNVRAMOffset, (BYTE *)&m_jackpotA8Reset, sizeof(m_jackpotA8Reset));
	}

	return oldValue;
}

bool CConfig::GetMGMDEnabled() const
{
	return m_mgmdEnabled;
}

bool CConfig::SetMGMDEnabled(bool mgmdEnabled)
{
	bool oldValue = m_mgmdEnabled;
	m_mgmdEnabled = mgmdEnabled;
	m_nvramConfig.Set(m_mgmdEnabledNVRAMOffset, (BYTE *)&m_mgmdEnabled, sizeof(m_mgmdEnabled));
	CPollerMGMDData::Instance().SetMGMDSenOption(byte(m_mgmdEnabled));
	return oldValue;
}

bool CConfig::SetMiniVirtualSessionEnabled(bool miniVirtualSessionEnabled)
{
	bool oldValue = m_miniVirtualSessionEnabled;
	m_miniVirtualSessionEnabled = miniVirtualSessionEnabled;
	m_nvramConfig.Set(m_miniVirtualSessionEnabledNVRAMOffset,
		(BYTE *)&m_miniVirtualSessionEnabled,
		sizeof(m_miniVirtualSessionEnabled));
	return oldValue;
}

bool CConfig::MiniVirtualSessionEnabled() const
{
	return m_miniVirtualSessionEnabled;
}

bool CConfig::SetOneLinkProgressivesEnabled(bool oneLinkProgressivesEnabled)
{
	bool oldValue = m_oneLinkProgressivesEnabled;
	m_oneLinkProgressivesEnabled = oneLinkProgressivesEnabled;
	m_nvramConfig.Set(m_oneLinkProgressivesEnabledNVRAMOffset,
		(BYTE *)&m_oneLinkProgressivesEnabled,
		sizeof(m_oneLinkProgressivesEnabled));
	return oldValue;
}

bool CConfig::SetMobileConnectCommunicationEnabled(bool mobileConnectCommunicationEnabled)
{
	bool oldValue = m_mobileConnectCommEnabled;
	m_mobileConnectCommEnabled = mobileConnectCommunicationEnabled;
	/* flag not relevant at the moment */
	m_nvramConfig.Set(m_mobileConnectCommunicationEnabledNVRAMOffset,
					  (BYTE *)&m_mobileConnectCommEnabled,
					  sizeof(m_mobileConnectCommEnabled));
	return oldValue;
}

bool CConfig::MobileConnectCommunicationEnabled() const
{
	return m_mobileConnectCommEnabled;
}

bool CConfig::SetRemoteDebuggingEnabled(bool value)
{
	bool oldValue = m_remoteDebuggingEnabled;
	m_remoteDebuggingEnabled = value;
	m_nvramConfig.Set(m_remoteDebuggingEnabledNVRAMOffset,
		(BYTE *)&m_remoteDebuggingEnabled,
		sizeof(m_remoteDebuggingEnabled));
	return oldValue;
}

bool CConfig::RemoteDebuggingEnabled() const
{
	return m_remoteDebuggingEnabled || CUtilities::FileExists(CDirectoryManager::REMOTE_DEBUG_FILE);
}

bool CConfig::SetPartialWATOutEnabled_Global(bool partialWATOutEnabled_global)
{
	bool oldValue(m_partialWATOutEnabled_global);

	m_partialWATOutEnabled_global = partialWATOutEnabled_global;
	m_nvramConfig.Set(m_partialWATOutEnabledGlobalNVRAMOffset, (BYTE *)&partialWATOutEnabled_global, 
		sizeof(partialWATOutEnabled_global));

	return oldValue;
}

bool CConfig::PartialWATOutEnabled_Global() const
{
	return m_partialWATOutEnabled_global;
}

bool CConfig::SetPartialWATOutEnabled(bool partialWatOutEnabled)
{
	bool oldValue = m_partialWATOutEnabled;
	m_partialWATOutEnabled = partialWatOutEnabled;
	m_nvramConfig.Set(m_partialWATOutEnabledNVRAMOffset,
		(BYTE *)&m_partialWATOutEnabled,
		sizeof(m_partialWATOutEnabled));
	return oldValue;
}

bool CConfig::PartialWATOutEnabled() const
{
	return m_partialWATOutEnabled_global && m_partialWATOutEnabled;
}

bool CConfig::SetAutoUploadCreditsGlobal(bool autoUploadCreditsGlobal)
{
	bool oldValue(m_autoUploadCreditsGlobal);
	m_autoUploadCreditsGlobal = autoUploadCreditsGlobal;
	m_nvramConfig.Set(m_autoUploadCreditsGlobalNVRAMOffset, (BYTE *)&m_autoUploadCreditsGlobal,
		sizeof(m_autoUploadCreditsGlobal));
	return oldValue;
}

bool CConfig::AutoUploadCreditsGlobal() const
{
	return m_autoUploadCreditsGlobal;
}

bool CConfig::SetCashoutToHostEnabled_Global(bool cashoutToHostEnabled_global)
{
	bool oldValue(m_cashoutToHostEnabled_global);

	m_cashoutToHostEnabled_global = cashoutToHostEnabled_global;
	m_nvramConfig.Set(m_cashoutToHostEnabledGlobalNVRAMOffset, (BYTE *)&m_cashoutToHostEnabled_global,
		sizeof(m_cashoutToHostEnabled_global));

	return oldValue;
}

bool CConfig::CashoutToHostEnabled_Global() const
{
	return m_cashoutToHostEnabled_global;
}

bool CConfig::SetCashoutToHostEnabled(bool cashoutToHostEnabled)
{
	bool oldValue = m_cashoutToHostEnabled;
	m_cashoutToHostEnabled = cashoutToHostEnabled;
	m_nvramConfig.Set(m_cashoutToHostEnabledNVRAMOffset,
		(BYTE *)&m_cashoutToHostEnabled,
		sizeof(m_cashoutToHostEnabled));
	return oldValue;
}

bool CConfig::CashoutToHostEnabled() const
{
	return m_cashoutToHostEnabled_global && m_cashoutToHostEnabled;
}

bool CConfig::OneLinkProgressivesEnabled() const
{
	return m_oneLinkProgressivesEnabled;
}

bool CConfig::SetRebootUiRestartOverride(bool rebootUiRestartOverride)
{
	bool oldValue = m_rebootUiRestartOverride;
	m_rebootUiRestartOverride = rebootUiRestartOverride;
	m_nvramConfig.Set(m_rebootUiRestartOverride,
		(BYTE *)&m_rebootUiRestartOverride,
		sizeof(m_rebootUiRestartOverride));

	return oldValue;
}

bool CConfig::GetRebootUiRestartOverrideEnabled() const
{
	return m_rebootUiRestartOverride;
}

bool CConfig::SetDisplayPlayerProfileButton(bool displayProfileButton)
{
	bool oldValue = m_displayPlayerProfileButton;
	m_displayPlayerProfileButton = displayProfileButton;
	m_nvramConfig.Set(m_displayPlayerProfileButtonOffset,
		(BYTE *)&m_displayPlayerProfileButton,
		sizeof(m_displayPlayerProfileButton));

	return oldValue;
}

bool CConfig::DisplayPlayerProfileButton() const
{
	return m_displayPlayerProfileButton;
}

bool CConfig::SetGlobalMGMD(bool globalMGMD)
{
	bool oldValue = m_globalMGMD;
	m_globalMGMD = globalMGMD;
	m_nvramConfig.Set(m_globalMGMDOffset,
		(BYTE *)&globalMGMD,
		sizeof(globalMGMD));
	CPollerMGMDData::Instance().SetMGMDGlobalOption(byte(m_globalMGMD));
	return oldValue;
}

bool CConfig::GetGlobalMGMDEnabled() const
{
	return m_globalMGMD;
}

void CConfig::SetSASVersion(const std::string& version)
{
	m_sasVersion = version;
}

void CConfig::SetMixerFirmwareVersions(const std::string& mixerFirmwareVersion, const std::string& touchFirmwareVersion)
{
	m_mixerFirmwareVersion = mixerFirmwareVersion;
	m_touchFirmwareVersion = touchFirmwareVersion;
}

void CConfig::SetMixerFirmwareVersion(const std::string& mixerFirmwareVersion)
{
	m_mixerFirmwareVersion = mixerFirmwareVersion;
}

void CConfig::SetTargetFirmwareVersions(const std::string& mixerFirmwareVersion, const std::string& carrierFirmwareVersion)
{
	if (CDisplayManager::Instance().GetDisplayPlatform() == VideoMixerPlatform)
	{
		m_targetMixerFirmwareVersion = mixerFirmwareVersion;
	}
	m_targetFirmwareVersion = carrierFirmwareVersion;
}

void CConfig::SetImageChecksum(DWORD value)
{
	m_imageChecksum = value;
}

DWORD CConfig::GetImageChecksum() const
{
	return m_imageChecksum;
}

void CConfig::SetProbePBTLock(bool probeLock)
{
   m_probePBTLock = probeLock;
}

bool CConfig::GetProbePBTLock() const
{
   return m_probePBTLock;
}

void CConfig::SetGameStartExceptions(bool gameStart)
{
   if (m_gameStartExceptions != gameStart)
   {
      m_gameStartExceptions = gameStart;
      m_nvramConfig.Set(m_gameStartExceptionsOffset,
                        (BYTE *)&m_gameStartExceptions,
                        sizeof(m_gameStartExceptions));
	  CPollerMGMDData::Instance().SetMGMDGameStart(byte(m_gameStartExceptions));
   }
}

bool CConfig::GetGameStartExceptions()
{
   return m_gameStartExceptions;
}

void CConfig::SetGameEndExceptions(bool gameEnd)
{
   if (m_gameEndExceptions != gameEnd)
   {
      m_gameEndExceptions = gameEnd;
      m_nvramConfig.Set(m_gameEndExceptionsOffset,
                        (BYTE *)&m_gameEndExceptions,
                        sizeof(m_gameEndExceptions));
	  CPollerMGMDData::Instance().SetMGMDGameEnd(byte(m_gameEndExceptions));
   }
}

bool CConfig::GetGameEndExceptions()
{
   return m_gameEndExceptions;
}

void CConfig::SetBonusCounterNotificationPriority(WORD priority)
{
	// Set only valid values.
	if (priority < 0)
	{
		priority = CConfigExtData::DEFAULT_NOTIFICATION_BOX_PRIORITY;
	}
	m_bonusCounterNotificationPriority = priority;
	m_nvramConfig.Set(m_bonusCounterNotificationPriorityOffset, (BYTE *)&m_bonusCounterNotificationPriority, sizeof(m_bonusCounterNotificationPriority));
}

WORD CConfig::GetBonusCounterNotificationPriority() const
{
	return m_bonusCounterNotificationPriority;
}

void CConfig::SetBonusCounternRichLevelPriority(WORD priority)
{
	// Set only valid values.
	if (priority < 0)
	{
		priority = CConfigExtData::DEFAULT_NRICH_PRIORITY;
	}
	m_bonusCounternRichLevelPriority = priority;
	m_nvramConfig.Set(m_bonusCounternRichLevelPriorityOffset, (BYTE *)&m_bonusCounternRichLevelPriority, sizeof(m_bonusCounternRichLevelPriority));
}

WORD CConfig::GetBonusCounternRichLevelPriority() const
{
	return m_bonusCounternRichLevelPriority;
}

void CConfig::SetBonusCounterSplashdownPriority(WORD priority)
{
	// Set only valid values.
	if (priority < 0)
	{
		priority = CConfigExtData::DEFAULT_SPLASHDOWN_PRIORITY;
	}
	m_bonusCounterSplashdownPriority = priority;
	m_nvramConfig.Set(m_bonusCounterSplashdownPriorityOffset, (BYTE *)&m_bonusCounterSplashdownPriority, sizeof(m_bonusCounterSplashdownPriority));
}

WORD CConfig::GetBonusCounterSplashdownPriority() const
{
	return m_bonusCounterSplashdownPriority;
}

void CConfig::SetBonusCounterProgressiveOdometerPriority(WORD priority)
{
	// Set only valid values.
	if (priority < 0)
	{
		priority = CConfigExtData::DEFAULT_PROGRESSIVE_ODOMETER_PRIORITY;
	}
	m_bonusCounterProgressiveOdometerPriority = priority;
	m_nvramConfig.Set(m_bonusCounterProgressiveOdometerPriorityOffset, (BYTE *)&m_bonusCounterProgressiveOdometerPriority, sizeof(m_bonusCounterProgressiveOdometerPriority));
}

WORD CConfig::GetBonusCounterProgressiveOdometerPriority() const
{
	return m_bonusCounterProgressiveOdometerPriority;
}

void CConfig::SetBonusCounterPtePriority(WORD priority)
{
	// Set only valid values.
	if (priority < 0)
	{
		priority = CConfigExtData::DEFAULT_PTE_PRIORITY;
	}
	m_bonusCounterPtePriority = priority;
	m_nvramConfig.Set(m_bonusCounterPtePriorityOffset, (BYTE *)&m_bonusCounterPtePriority, sizeof(m_bonusCounterPtePriority));
}

WORD CConfig::GetBonusCounterPtePriority() const
{
	return m_bonusCounterPtePriority;
}

void CConfig::SetBonusCounterScatterCountdownTimerPriority(WORD priority)
{
	// Set only valid values.
	if (priority < 0)
	{
		priority = CConfigExtData::DEFAULT_SCATTER_COUNTDOWN_TIMER_PRIORITY;
	}
	m_bonusCounterScatterCountdownTimerPriority = priority;
	m_nvramConfig.Set(m_bonusCounterScatterCountdownTimerPriorityOffset, (BYTE *)&m_bonusCounterScatterCountdownTimerPriority, sizeof(m_bonusCounterScatterCountdownTimerPriority));
}

WORD CConfig::GetBonusCounterScatterCountdownTimerPriority() const
{
	return m_bonusCounterScatterCountdownTimerPriority;
}

void CConfig::SetBonusCounterPrizePriority(WORD priority)
{
	// Set only valid values.
	if (priority < 0)
	{
		priority = CConfigExtData::DEFAULT_PRIZE_ODOMETER_PRIORITY;
	}
	m_bonusCounterPrizePriority = priority;
	m_nvramConfig.Set(m_bonusCounterPrizePriorityOffset, (BYTE *)&m_bonusCounterPrizePriority, sizeof(m_bonusCounterPrizePriority));
}

WORD CConfig::GetBonusCounterPrizePriority() const
{
	return m_bonusCounterPrizePriority;
}

void CConfig::SetBonusCounterPlayXGetYPriority(WORD priority)
{
	// Set only valid values.
	if (priority < 0)
	{
		priority = CConfigExtData::DEFAULT_PLAYXGETY_PRIORITY;
	}
	m_bonusCounterPlayXGetYPriority = priority;
	m_nvramConfig.Set(m_bonusCounterPlayXGetYPriorityOffset, (BYTE *)&m_bonusCounterPlayXGetYPriority, sizeof(m_bonusCounterPlayXGetYPriority));
}

WORD CConfig::GetBonusCounterPlayXGetYPriority() const
{
	return m_bonusCounterPlayXGetYPriority;
}

bool CConfig::SetExternalCreditToGame(bool bCreditTogame)
{
	bool oldValue = m_ExternalCreditToGameEnabled;
	if (m_ExternalCreditToGameEnabled != bCreditTogame)
	{
		m_ExternalCreditToGameEnabled = bCreditTogame;
		m_nvramConfig.Set(m_externalCreditToGameNVRAMOffset,
			(BYTE *)&m_ExternalCreditToGameEnabled,
			sizeof(m_ExternalCreditToGameEnabled));
	}

	LogString(PBT_LOG, "%s : External Credit to game enbale flag set is :%d ",__FUNCTION__, m_ExternalCreditToGameEnabled);
	return oldValue;
}

bool CConfig::GetExternalCreditToGameEnabled( void ) const
{
	LogString(PBT_LOG, "%s : External Credit to game enbale flag :%d ", __FUNCTION__, m_ExternalCreditToGameEnabled);
	return m_ExternalCreditToGameEnabled;
}

bool CConfig::ExternalCreditVerificationEnabled() const
{
	return (m_senOption2Raw & SO2_EXTERNAL_CREDIT_TRANSFER_VERIFICATION) == SO2_EXTERNAL_CREDIT_TRANSFER_VERIFICATION;
}

bool CConfig::SetGlobalExcessiveVoucherOutEnabledFlag(bool enabled)
{
	bool oldValue(m_globalExcessiveVoucherOutEnabledFlag);
	
	m_globalExcessiveVoucherOutEnabledFlag = enabled;
	m_nvramConfig.Set(m_globalExcessiveVoucherOutEnabledFlagOffset, (BYTE *)&enabled, sizeof(enabled));

	return oldValue;
}

bool CConfig::GetGlobalExcessiveVoucherOutEnabledFlag() const
{
	return m_globalExcessiveVoucherOutEnabledFlag;
}

DWORD CConfig::SetExcessiveVoucherOutThreshold(DWORD threshold)
{
	DWORD bOldVal(m_excessiveVoucherOutThreshold);

	m_excessiveVoucherOutThreshold = threshold;
	m_nvramConfig.Set(m_excessiveVoucherOutThresholdOffset, (BYTE*)&m_excessiveVoucherOutThreshold, sizeof(m_excessiveVoucherOutThreshold));

	return bOldVal;
}

DWORD CConfig::GetExcessiveVoucherOutThreshold()
{
	return m_excessiveVoucherOutThreshold;
}

DWORD CConfig::SetTotalBillInThreshold(DWORD threshold)
{
	DWORD bOldVal(m_totalBillInThreshold);

	m_totalBillInThreshold = threshold;
	m_nvramConfig.Set(m_totalBillInThresholdOffset, (BYTE*)&m_totalBillInThreshold, sizeof(m_totalBillInThreshold));

	return bOldVal;
}

DWORD CConfig::GetTotalBillInThreshold()
{
	return m_totalBillInThreshold;
}


DWORD CConfig::SetCoinInThreshold(DWORD threshold)
{
	DWORD bOldVal(m_coinInThreshold);

	m_coinInThreshold = threshold;
	m_nvramConfig.Set(m_coinInThresholdOffset, (BYTE*)&m_coinInThreshold, sizeof(m_coinInThreshold));

	return bOldVal;
}

DWORD CConfig::GetCoinInThreshold()
{
	return m_coinInThreshold;
}

DWORD CConfig::SetCoinOutThreshold(DWORD threshold)
{
	DWORD bOldVal(m_coinOutThreshold);

	m_coinOutThreshold = threshold;
	m_nvramConfig.Set(m_coinOutThresholdOffset, (BYTE*)&m_coinOutThreshold, sizeof(m_coinOutThreshold));

	return bOldVal;
}

DWORD CConfig::GetCoinOutThreshold()
{
	return m_coinOutThreshold;
}

void CConfig::SetPoller_12_11_6000_or_Higher(bool value)
{
	m_extensibleFlags.m_data.isPoller_12_11_6000_or_Higher = value;
	m_nvramConfig.Set(m_extensibleFlagsNVRAMOffset, (BYTE*)&m_extensibleFlags.m_wordValue, sizeof(m_extensibleFlags.m_wordValue));
}

bool CConfig::IsPoller_12_11_6000_or_Higher() const
{
	return m_extensibleFlags.IsPoller_12_11_6000_or_Higher();
}

void CConfig::SetNFDPoller(bool value)
{
	m_extensibleFlags.m_data.isNFDPoller = value;
	m_nvramConfig.Set(m_extensibleFlagsNVRAMOffset, (BYTE*)&m_extensibleFlags.m_wordValue, sizeof(m_extensibleFlags.m_wordValue));
}

bool CConfig::IsNFDPoller() const
{
	return m_extensibleFlags.IsNFDPoller();
}

bool CConfig::SetTicketInResend(bool value)
{
	bool oldValue(m_extensibleFlags.TicketInResend());

	m_extensibleFlags.m_data.ticketInResend = value;
	m_nvramConfig.Set(m_extensibleFlagsNVRAMOffset, (BYTE*)&m_extensibleFlags.m_wordValue, sizeof(m_extensibleFlags.m_wordValue));

	return oldValue;
}

bool CConfig::SetEnableLocationServices(bool value)
{
	bool oldValue(m_extensibleFlags.EnableLocationServices());

	m_extensibleFlags.m_data.enableLocationServices = value;
	m_nvramConfig.Set(m_extensibleFlagsNVRAMOffset, (BYTE*)&m_extensibleFlags.m_wordValue, sizeof(m_extensibleFlags.m_wordValue));

	return oldValue;
}

bool CConfig::GetEnableLocationServices() const
{
	return m_extensibleFlags.EnableLocationServices();
}

DWORD CConfig::SetRestartUIAfterxDays(DWORD value)
{
	DWORD oldValue(m_extensibleFlags.RestartUIAfterxDays());

	m_extensibleFlags.m_data.restartUIAfterxDays = value;
	m_nvramConfig.Set(m_extensibleFlagsNVRAMOffset, (BYTE*)&m_extensibleFlags.m_wordValue, sizeof(m_extensibleFlags.m_wordValue));

	return oldValue;
}

DWORD CConfig::GetRestartUIAfterxDays() const
{
	return m_extensibleFlags.RestartUIAfterxDays();
}

DWORD CConfig::SetSASReadTimeOutMultiplier(DWORD value)
{
	DWORD oldValue(m_extensibleFlags.GetSASReadTimeOutMultiplier());

	m_extensibleFlags.m_data.sasReadTimeOutMultiplier = value;
	m_sasReadTimeOutMultiplier = m_extensibleFlags.m_data.sasReadTimeOutMultiplier;
	m_nvramConfig.Set(m_extensibleFlagsNVRAMOffset, (BYTE*)&m_extensibleFlags.m_wordValue, sizeof(m_extensibleFlags.m_wordValue));

	return oldValue;
}

DWORD CConfig::GetSASReadTimeOutMultiplier()
{
	return m_extensibleFlags.GetSASReadTimeOutMultiplier();
}

bool CConfig::SetUJPTaxWithholding_Global(bool enabled)
{
	bool oldValue(m_taxWithholdingEnabled);

	m_taxWithholdingEnabled = enabled;
	m_nvramConfig.Set(m_taxWithholdingEnabledOffsetNVRAMOffset, (BYTE *)&enabled, sizeof(enabled));

	return oldValue;
}

bool CConfig::UJPTaxWithholding() const
{
	return m_taxWithholdingEnabled;
}

float CConfig::SetFederalTaxWithholdingPercent(float percent)
{
	float bOldVal(m_federalTaxWithholdingPercent);

	m_federalTaxWithholdingPercent = percent;
	m_nvramConfig.Set(m_federalTaxWithholdingPercentOffsetNVRAMOffset, (BYTE*)&percent, sizeof(percent));

	return bOldVal;
}

float CConfig::GetFederalTaxWithholdingPercent() const
{
	return m_federalTaxWithholdingPercent;
}

float CConfig::SetStateTaxWithholdingPercent(float percent)
{
	float bOldVal(m_stateTaxWithholdingPercent);

	m_stateTaxWithholdingPercent = percent;
	m_nvramConfig.Set(m_stateTaxWithholdingPercentOffsetNVRAMOffset, (BYTE*)&percent, sizeof(percent));

	return bOldVal;
}

float CConfig::GetStateTaxWithholdingPercent() const
{
	return m_stateTaxWithholdingPercent;
}

DWORD CConfig::SetNumberOfSubgamesMismatchCheck(DWORD value)
{
	DWORD oldVal(m_numberOfSubgamesMismatchCheck);

	m_numberOfSubgamesMismatchCheck = value;
	m_nvramConfig.Set(m_numberOfSubgamesMismatchCheckOffset, (BYTE*)&value, sizeof(value));

	return oldVal;
}

DWORD CConfig::GetNumberOfSubgamesMismatchCheck() const
{
	return m_numberOfSubgamesMismatchCheck;
}

DWORD CConfig::SetAutoCardOutSecToCountdownCredits(DWORD numberOfseconds)
{
	DWORD oldVal(m_autoCardOutSecToCountdownCredits);

	m_autoCardOutSecToCountdownCredits = numberOfseconds;

	return oldVal;
}

DWORD CConfig::SetAutoCardOutSecToCountdownNoCrdts(DWORD numberOfseconds)
{
	DWORD oldVal(m_autoCardOutSecToCountdownNoCrdts);

	m_autoCardOutSecToCountdownNoCrdts = numberOfseconds;

	return oldVal;
}

DWORD CConfig::SetAutoCardOutCountdownSeconds(DWORD numberOfseconds)
{
	DWORD oldVal(m_autoCardOutCountdownSeconds);

	m_autoCardOutCountdownSeconds = numberOfseconds;

	return oldVal;
}

DWORD CConfig::SetAutoCardOutSecToAlert(DWORD numberOfseconds)
{
	DWORD oldVal(m_autoCardOutSecToAlert);

	m_autoCardOutSecToAlert = numberOfseconds;

	return oldVal;
}

DWORD CConfig::GetAutoCardOutSecToCountdownCredits() const
{
	return m_autoCardOutSecToCountdownCredits;
}

DWORD CConfig::GetAutoCardOutSecToCountdownNoCrdts() const
{
	return m_autoCardOutSecToCountdownNoCrdts;
}

DWORD CConfig::GetAutoCardOutCountdownSeconds() const
{
	return m_autoCardOutCountdownSeconds;
}

DWORD CConfig::GetAutoCardOutSecToAlert() const
{
	return m_autoCardOutSecToAlert;
}

void CConfig::SetExtensibleSystemSettings(const string& extensibleSystemSettings)
{
	m_extensibleSystemSettings = extensibleSystemSettings;
}

string CConfig::GetExtensibleSystemSettingsStr() const
{
	return m_extensibleSystemSettings;
}

DWORD CConfig::GetCountdownDisplayIntervalInSeconds() const
{
	return m_countdownDisplayIntervalInSec;
}

DWORD CConfig::SetCountdownDisplayIntervalInSeconds(const DWORD numberOfseconds)
{
	DWORD oldVal(m_countdownDisplayIntervalInSec);

	m_countdownDisplayIntervalInSec = numberOfseconds;

	return oldVal;
}

bool CConfig::GetPointsCountdownControlByUniverse() const
{
	return m_pointsCountdownControlByUniverse;
}

bool CConfig::SetPointsCountdownControlByUniverse(const bool flag)
{
	bool oldVal(m_pointsCountdownControlByUniverse);

	m_pointsCountdownControlByUniverse = flag;

	return oldVal;
}

void CConfig::SetSystemTime(const SYSTEMTIME& systemTime)
{	
	m_nvramConfig.Set(m_systemTimeOffset, (BYTE*)&systemTime, sizeof(systemTime));
	m_nvramConfig.Set(m_systemTimeSprinkleOffset, (BYTE*)&NVRAM_SYSTEM_TIME_SPRINKLE_VALUE, sizeof(NVRAM_SYSTEM_TIME_SPRINKLE_VALUE));
}

DWORD CConfig::SetRebootAsapThresholdInMb(DWORD value)
{
	DWORD oldVal(m_rebootAsapThresholdInMb);

	m_rebootAsapThresholdInMb = value;
	m_nvramConfig.Set(m_rebootAsapThresholdInMbOffset, (BYTE*)&value, sizeof(value));

	return oldVal;
}

DWORD CConfig::GetRebootAsapThresholdInMb() const
{
	return m_rebootAsapThresholdInMb;
}

DWORD CConfig::SetRebootWarningThresholdMb(DWORD value)
{
	DWORD oldVal(m_rebootWarningThresholdMb);

	m_rebootWarningThresholdMb = value;
	m_nvramConfig.Set(m_rebootWarningThresholdMbOffset, (BYTE*)&value, sizeof(value));

	return oldVal;
}

DWORD CConfig::GetRebootWarningThresholdMb() const
{
	return m_rebootWarningThresholdMb;
}

DWORD CConfig::SetRestartUIAsapThresholdMb(DWORD value)
{
	DWORD oldVal(m_restartUIAsapThresholdMb);

	m_restartUIAsapThresholdMb = value;
	m_nvramConfig.Set(m_restartUIAsapThresholdMbOffset, (BYTE*)&value, sizeof(value));

	return oldVal;
}

DWORD CConfig::GetRestartUIAsapThresholdMb() const
{
	return m_restartUIAsapThresholdMb;
}
 
DWORD CConfig::SetRestartUIAfterDropThresholdMb(DWORD value)
{
	DWORD oldVal(m_restartUIAfterDropThresholdMb);

	m_restartUIAfterDropThresholdMb = value;
	m_nvramConfig.Set(m_restartUIAfterDropThresholdMbOffset, (BYTE*)&value, sizeof(value));

	return oldVal;
}

DWORD CConfig::GetRestartUIAfterDropThresholdMb() const
{
	return m_restartUIAfterDropThresholdMb;
}

DWORD CConfig::SetRestartUIDelayHours(DWORD value)
{
	DWORD oldVal(m_restartUIDelayHours);

	m_restartUIDelayHours = value;
	m_nvramConfig.Set(m_restartUIDelayHoursOffset, (BYTE*)&value, sizeof(value));

	return oldVal;
}

DWORD CConfig::GetRestartUIDelayHours() const
{
	return m_restartUIDelayHours;
}

bool CConfig::SetUseSDSCardFormat(bool value)
{
	bool oldVal(m_extensibleFlags.UseSDSCardFormat());

	m_extensibleFlags.m_data.useSDSCardFormat = value;
	m_nvramConfig.Set(m_extensibleFlagsNVRAMOffset, (BYTE*)&m_extensibleFlags.m_wordValue, sizeof(m_extensibleFlags.m_wordValue));

	return oldVal;
}

bool CConfig::GetUseSDSCardFormat() const
{
	return m_extensibleFlags.UseSDSCardFormat();
}

DWORD CConfig::GetExtensibleSettings() const
{
	return m_extensibleFlags.m_wordValue;
}

void CConfig::ResetExtensibleSettings()
{
	m_extensibleFlags.Reset();
	m_nvramConfig.Set(m_extensibleFlagsNVRAMOffset, (BYTE*)&m_extensibleFlags.m_wordValue, sizeof(m_extensibleFlags.m_wordValue));
	m_sasReadTimeOutMultiplier = m_extensibleFlags.GetSASReadTimeOutMultiplier();
}

void CConfig::SetReceivedBadDenomFlag(DWORD value)
{
	m_receivedBadDenom = value;
}

bool CConfig::GetReceivedBadDenomFlag() const
{
	return m_receivedBadDenom;
}

string CConfig::GetnConnectUrl() const
{
	return m_nConnectUrl;
}

void CConfig::SetnConnectUrl(const string& value)
{
	m_nConnectUrl = value;
}

string CConfig::GetnConnectToken() const
{
	return m_nConnectToken;
}

void CConfig::SetnConnectToken(const string& value)
{
	m_nConnectToken = value;
}

string CConfig::GetAvatarUrl() const
{
	return m_avatarUrl;
}

void CConfig::SetAvatarUrl(const string& value)
{
	m_avatarUrl = value;
}

string CConfig::GetAvatarToken() const
{
	return m_avatarToken;
}

void CConfig::SetAvatarToken(const string& value)
{
	m_avatarToken = value;
}

string CConfig::GetRequestServiceUrl() const
{
	return m_requestServiceUrl;
}

void CConfig::SetRequestServiceUrl(const string& value)
{
	m_requestServiceUrl = value;
}

string CConfig::GetRequestServiceToken() const
{
	return m_requestServiceToken;
}

void CConfig::SetRequestServiceToken(const string& value)
{
	m_requestServiceToken = value;
}

string CConfig::GetDrinksOnTapUrl() const
{
	return m_drinksOnTapUrl;
}

void CConfig::SetDrinksOnTapUrl(const string& value)
{
	m_drinksOnTapUrl = value;
}

string CConfig::GetDrinksOnTapToken() const
{
	return m_drinksOnTapToken;
}

void CConfig::SetDrinksOnTapToken(const string& value)
{
	m_drinksOnTapToken = value;
}

void CConfig::SetSocialDistanceUrl(const string& value)
{
	m_socialDistanceUrl = value;
}

string CConfig::GetSocialDistanceUrl() const
{
	return m_socialDistanceUrl;
}

void CConfig::SetSocialDistanceToken(const string& value)
{
	m_socialDistanceToken = value;
}

string CConfig::GetSocialDistanceToken() const
{
	return m_socialDistanceToken;
}

void CConfig::SetSocialDistanceIgnoreDirty(const bool value)
{
	m_socialDistanceIgnoreDirty = value;
}

bool CConfig::GetSocialDistanceIgnoreDirty() const
{
	return m_socialDistanceIgnoreDirty;
}

void CConfig::SetSocialDistanceManualCleanEnabled(const bool value)
{
	m_socialDistanceManualCleanEnabled = value;
}

bool CConfig::GetSocialDistanceManualCleanEnabled() const
{
	return m_socialDistanceManualCleanEnabled;
}

void CConfig::SetSocialDistanceCountdownDelay(const DWORD value)
{
	m_socialDistanceCountdownDelay = value;
}

DWORD CConfig::GetSocialDistanceCountdownDelay() const
{
	return m_socialDistanceCountdownDelay;
}

std::string CConfig::GetFirmwareVersion() const
{
	return m_firmwareVersion;
}