#include "stdafx.h"
#include "Config.h"

// Unit testing includes.
#include "gtest/gtest.h"

using namespace std;

class ConfigUnitTests : public ::testing::Test
{

public:
};


TEST(ConfigUnitTests, ConfigUJPEnabledUnitTest)
{
	CConfig config(false, "");
	config.SetUJPEnabled(false);

	ASSERT_FALSE(config.GetUJPEnabled());

	bool oldVal = config.SetUJPEnabled(true);
	ASSERT_TRUE(config.GetUJPEnabled());
	ASSERT_FALSE(oldVal);
}

TEST(ConfigUnitTests, ConfigMGMDEnabledUnitTest)
{
	CConfig config(false, "");
	config.SetMGMDEnabled(false);

	ASSERT_FALSE(config.GetMGMDEnabled());

	bool oldVal = config.SetMGMDEnabled(true);
	ASSERT_TRUE(config.GetMGMDEnabled());
	ASSERT_FALSE(oldVal);
}

TEST(ConfigUnitTests, ConfigFeatureFlagUnitTest)
{
	CConfig config(false, "");
	bool atomicRedemptions = false;
	bool jackpotExternalBonus = false;
	bool externalCreditToGame = false;
	bool multiGameMultiDenom = false;

	bool oldVal = config.SetFeatureFlags(true, false, false, false);
	config.GetFeatureFlags(atomicRedemptions, jackpotExternalBonus, externalCreditToGame, multiGameMultiDenom);
	ASSERT_TRUE(atomicRedemptions);
	ASSERT_FALSE(jackpotExternalBonus);
	ASSERT_FALSE(multiGameMultiDenom);
	ASSERT_TRUE(config.AtomicRedemptions());
	ASSERT_FALSE(config.JackpotExternalBonus());
	ASSERT_FALSE(config.GetExternalCreditToGameEnabled());
	ASSERT_FALSE(config.MultiGameMultiDenom());
	ASSERT_FALSE(oldVal);

	oldVal = config.SetFeatureFlags(false, true, true, true);
	config.GetFeatureFlags(atomicRedemptions, jackpotExternalBonus, externalCreditToGame, multiGameMultiDenom);
	ASSERT_FALSE(atomicRedemptions);
	ASSERT_TRUE(jackpotExternalBonus);
	ASSERT_TRUE(externalCreditToGame);
	ASSERT_TRUE(multiGameMultiDenom);
	ASSERT_FALSE(config.AtomicRedemptions());
	ASSERT_TRUE(config.JackpotExternalBonus());
	ASSERT_TRUE(config.GetExternalCreditToGameEnabled());
	ASSERT_TRUE(config.MultiGameMultiDenom());
	ASSERT_TRUE(oldVal);
}

TEST(ConfigUnitTests, ConfigMGMDFeatureFlagUnitTest)
{
	CConfig config(false, "");
	config.SetMultiGameMultiDenom(false);

	ASSERT_FALSE(config.MultiGameMultiDenom());

	bool oldVal = config.SetMultiGameMultiDenom(true);
	ASSERT_TRUE(config.MultiGameMultiDenom());
	ASSERT_FALSE(oldVal);
}

TEST(ConfigUnitTests, ConfigExternalCreditToGameUnitTest1)
{
	CConfig config(false, "");
	config.SetExternalCreditToGame(false);

	ASSERT_FALSE(config.GetExternalCreditToGameEnabled());

	bool oldVal = config.SetExternalCreditToGame(true);
	ASSERT_TRUE(config.GetExternalCreditToGameEnabled());
	ASSERT_FALSE(oldVal);
}

TEST(ConfigUnitTests, ConfigGlobalMGMDEnabledUnitTest)
{
	CConfig config(false, "");
	config.SetGlobalMGMD(false);

	ASSERT_FALSE(config.GetGlobalMGMDEnabled());

	bool oldVal = config.SetGlobalMGMD(true);
	ASSERT_TRUE(config.GetGlobalMGMDEnabled());
	ASSERT_FALSE(oldVal);
}

TEST(ConfigUnitTests, ConfigPartialWATOutEnabledUnitTest)
{
	CConfig config(false, "");

	// Validate default values
	ASSERT_FALSE(config.PartialWATOutEnabled_Global());
	ASSERT_FALSE(config.PartialWATOutEnabled());

	// Validate that even if local option is enabled
	// but global option disabled, feature is disabled
	config.SetPartialWATOutEnabled(true);
	ASSERT_FALSE(config.PartialWATOutEnabled());

	// Validate that when both senoption and global option
	// are enabled then the feature is enabled
	config.SetPartialWATOutEnabled_Global(true);
	ASSERT_TRUE(config.PartialWATOutEnabled());
}

TEST(ConfigUnitTests, AutoUploadCreditsUnitTest12_11_6000_or_Higher)
{
	CConfig config(false, "");
	config.SetPoller_12_11_6000_or_Higher(true);

	// Validate default values
	ASSERT_FALSE(config.AutoUploadCreditsGlobal());
	ASSERT_FALSE(config.AutoUploadCredits());

	// Validate that even if local option is enabled
	// but global option disabled, feature is disabled
	config.SetSenOption2Raw(config.GetSenOption2Raw() + SO2_NCEP_AUTO_UPLOAD);
	ASSERT_FALSE(config.AutoUploadCredits());

	// Validate that even if local option is enabled
	// and global option enabled, feature is still 
	// disabled because both PBFromGameEnabled() and 
	// NCEPOutEnabled() are disabled
	config.SetAutoUploadCreditsGlobal(true);
	ASSERT_FALSE(config.AutoUploadCredits());

	// SO2_WAT_AUTOPLOAD test
	config.SetPBTCashEnabled_Global(true);
	config.SetPBFromGameEnabled(true);
	ASSERT_FALSE(config.AutoUploadCredits());
	config.SetSenOption2Raw(config.GetSenOption2Raw() + SO2_WAT_AUTOPLOAD);
	ASSERT_TRUE(config.AutoUploadCredits());
	config.SetPBTCashEnabled_Global(false);
	config.SetPBFromGameEnabled(false);
	ASSERT_FALSE(config.AutoUploadCredits());

	// SO2_CEP_AUTOPLOAD test
	config.SetPBTCashEnabled_Global(true);
	config.SetPBFromGameEnabled(true);
	config.SetSenOption2Raw(config.GetSenOption2Raw() - SO2_WAT_AUTOPLOAD);
	ASSERT_FALSE(config.AutoUploadCredits());
	config.SetSenOption2Raw(config.GetSenOption2Raw() + SO2_CEP_AUTOPLOAD);
	ASSERT_FALSE(config.AutoUploadCredits());
	config.SetSenOption2Raw(config.GetSenOption2Raw() + SO2_CEP_OUT_ENABLED);
	ASSERT_TRUE(config.AutoUploadCredits());
	config.SetPBTCashEnabled_Global(false);
	config.SetPBFromGameEnabled(false);
	config.SetSenOption2Raw(config.GetSenOption2Raw() - SO2_CEP_OUT_ENABLED);
	ASSERT_FALSE(config.AutoUploadCredits());

	// NCEPOutEnabled
	config.SetPBTPromoEnabled_Global(true);
	config.SetSenOption2Raw(config.GetSenOption2Raw() + SO2_NCEP_OUT_ENABLED);
	ASSERT_TRUE(config.AutoUploadCredits());
}

TEST(ConfigUnitTests, AutoUploadCreditsUnitTestLowerThan12_11_6000)
{
	CConfig config(false, "");
	config.SetPoller_12_11_6000_or_Higher(false);

	// Validate default values
	ASSERT_FALSE(config.AutoUploadCreditsGlobal());
	ASSERT_FALSE(config.AutoUploadCredits());

	// Validate that even if local option is enabled
	// but global option disabled, feature is disabled
	config.SetSenOption2Raw(config.GetSenOption2Raw() + SO2_NCEP_AUTO_UPLOAD);
	ASSERT_FALSE(config.AutoUploadCredits());

	// PBFromGameEnabled
	config.SetPBTCashEnabled_Global(true);
	config.SetPBFromGameEnabled(true);
	ASSERT_FALSE(config.AutoUploadCredits());
	config.SetPBTCashEnabled_Global(false);
	config.SetPBFromGameEnabled(false);

	// NCEPOutEnabled
	config.SetPBTPromoEnabled_Global(true);
	config.SetSenOption2Raw(config.GetSenOption2Raw() + SO2_NCEP_OUT_ENABLED);
	ASSERT_TRUE(config.AutoUploadCredits());
}

TEST(ConfigUnitTests, ConfigCashoutToHostEnabledUnitTest)
{
	CConfig config(false, "");

	// Validate default values
	ASSERT_FALSE(config.CashoutToHostEnabled_Global());
	ASSERT_FALSE(config.CashoutToHostEnabled());

	// Validate that even if local option is enabled
	// but global option disabled, feature is disabled
	config.SetCashoutToHostEnabled(true);
	ASSERT_FALSE(config.CashoutToHostEnabled());

	// Validate that when both senoption and global option
	// are enabled then the feature is enabled
	config.SetCashoutToHostEnabled_Global(true);
	ASSERT_TRUE(config.CashoutToHostEnabled());
}

TEST(ConfigUnitTests, ConfigNFDPollerUnitTest)
{
	CConfig config(false, "");

	// Validate default value
	ASSERT_FALSE(config.IsNFDPoller());

	// Validate config retrieval when config is set to TRUE
	config.SetNFDPoller(true);
	ASSERT_TRUE(config.IsNFDPoller());

	// Validate config retrieval when config is set to FALSE
	config.SetNFDPoller(false);
	ASSERT_FALSE(config.IsNFDPoller());
}

TEST(ConfigUnitTests, ConfigEnableLocationServicesUnitTest)
{
	CConfig config(false, "");

	// Validate default value
	ASSERT_FALSE(config.GetEnableLocationServices());

	// Validate config retrieval when config is set to TRUE
	config.SetEnableLocationServices(true);
	ASSERT_TRUE(config.GetEnableLocationServices());

	// Validate config retrieval when config is set to FALSE
	config.SetEnableLocationServices(false);
	ASSERT_FALSE(config.GetEnableLocationServices());
}

TEST(ConfigUnitTests, ConfigAutoCardOutSecToCountdownCreditsUnitTest)
{
	CConfig config(false, "");
	DWORD value = 30;
	ASSERT_TRUE(0 == config.GetAutoCardOutSecToCountdownCredits());
	ASSERT_TRUE(0 == config.SetAutoCardOutSecToCountdownCredits(value));
	ASSERT_TRUE(value == config.GetAutoCardOutSecToCountdownCredits());
}

TEST(ConfigUnitTests, ConfigAutoCardOutSecToCountdownNoCreditsUnitTest)
{
	CConfig config(false, "");
	DWORD value = 29;
	ASSERT_TRUE(0 == config.GetAutoCardOutSecToCountdownNoCrdts());
	ASSERT_TRUE(0 == config.SetAutoCardOutSecToCountdownNoCrdts(value));
	ASSERT_TRUE(value == config.GetAutoCardOutSecToCountdownNoCrdts());
}

TEST(ConfigUnitTests, ConfigAutoCardOutCountdownSecondsUnitTest)
{
	CConfig config(false, "");
	DWORD value = 28;
	ASSERT_TRUE(0 == config.GetAutoCardOutCountdownSeconds());
	ASSERT_TRUE(0 == config.SetAutoCardOutCountdownSeconds(value));
	ASSERT_TRUE(value == config.GetAutoCardOutCountdownSeconds());
}

TEST(ConfigUnitTests, ConfigAutoCardOutSecToAlertUnitTest)
{
	CConfig config(false, "");
	DWORD value = 28;
	ASSERT_TRUE(0 == config.GetAutoCardOutSecToAlert());
	ASSERT_TRUE(0 == config.SetAutoCardOutSecToAlert(value));
	ASSERT_TRUE(value == config.GetAutoCardOutSecToAlert());
}

TEST(ConfigUnitTests, ConfigNumberOfSubgamesMismatchChecktUnitTest)
{
	CConfig config(false, "");
	DWORD value = 28;
	ASSERT_TRUE(50 == config.GetNumberOfSubgamesMismatchCheck());
	ASSERT_TRUE(50 == config.SetNumberOfSubgamesMismatchCheck(value));
	ASSERT_TRUE(value == config.GetNumberOfSubgamesMismatchCheck());
}

TEST(ConfigUnitTests, ExtensibleSystemSettingsUnitTest)
{
	CConfig config(false, "");
	string value("Extensible System Settings");
	ASSERT_TRUE("" == config.GetExtensibleSystemSettingsStr());
	config.SetExtensibleSystemSettings(value);
	ASSERT_TRUE(value == config.GetExtensibleSystemSettingsStr());
}

TEST(ConfigUnitTests, CountdownDisplayIntervalInSecondsUnitTest)
{
	CConfig config(false, "");
	DWORD value = 10;
	ASSERT_TRUE(DEFAULT_COUNTDOWN_DISPLAY_INTERVAL == config.GetCountdownDisplayIntervalInSeconds());
	ASSERT_TRUE(DEFAULT_COUNTDOWN_DISPLAY_INTERVAL == config.SetCountdownDisplayIntervalInSeconds(value));
	ASSERT_TRUE(value == config.GetCountdownDisplayIntervalInSeconds());
}

TEST(ConfigUnitTests, PointsCountdownControlByUniverseUnitTest)
{
	CConfig config(false, "");

	// Validate default value
	ASSERT_FALSE(config.GetPointsCountdownControlByUniverse());

	// Validate config retrieval when config is set to TRUE
	config.SetPointsCountdownControlByUniverse(true);
	ASSERT_TRUE(config.GetPointsCountdownControlByUniverse());

	// Validate config retrieval when config is set to FALSE
	config.SetPointsCountdownControlByUniverse(false);
	ASSERT_FALSE(config.GetPointsCountdownControlByUniverse());
}

TEST(ConfigUnitTests, RestartUIAfterxDaysTest)
{
	CConfig config(false, "");

	ASSERT_TRUE(DEFAULT_RESTART_UI_AFTER_X_DAYS == config.GetRestartUIAfterxDays());

	config.SetRestartUIAfterxDays(255);
	ASSERT_TRUE(255 == config.GetRestartUIAfterxDays());

	config.SetRestartUIAfterxDays(256);
	ASSERT_TRUE(0 == config.GetRestartUIAfterxDays());

	config.SetRestartUIAfterxDays(257);
	ASSERT_TRUE(1 == config.GetRestartUIAfterxDays());
}

TEST(ConfigUnitTests, SASReadTimeOutMultiplierTest)
{
	CConfig config(false, "");

	ASSERT_TRUE(DEFAULT_SAS_READ_TIMEOUT_MULTIPLIER == CConfig::m_sasReadTimeOutMultiplier);
	ASSERT_TRUE(DEFAULT_SAS_READ_TIMEOUT_MULTIPLIER == config.GetSASReadTimeOutMultiplier());

	ASSERT_TRUE(DEFAULT_SAS_READ_TIMEOUT_MULTIPLIER == config.SetSASReadTimeOutMultiplier(31));
	ASSERT_TRUE(31 == config.GetSASReadTimeOutMultiplier());

	config.SetSASReadTimeOutMultiplier(32);
	ASSERT_TRUE(0 == config.GetSASReadTimeOutMultiplier());

	config.SetSASReadTimeOutMultiplier(33);
	ASSERT_TRUE(1 == config.GetSASReadTimeOutMultiplier());
}

TEST(ConfigUnitTests, NVRAMOffsetsUnitTest)
{
	CConfig config(false, "");	
	ASSERT_TRUE(config.m_timezoneOffsetNVRAMOffset == TIME_ZONE_OFFSET);
	ASSERT_TRUE(config.m_remoteDebuggingEnabledNVRAMOffset == REMOTE_DEBUGGING_ENABLED_OFFSET);	
	ASSERT_TRUE(config.m_systemTimeSprinkleOffset == SYSTEM_TIME_SPRINKLE_OFFSET);
	ASSERT_TRUE(config.m_systemTimeOffset == SYSTEM_TIME_OFFSET);
	ASSERT_TRUE(config.m_gameListChunkSizeOffset == 723290);
}

TEST(ConfigUnitTests, ConfigBadDenomReceivedUnitTest)
{
	CConfig config(false, "");
	ASSERT_FALSE(config.GetReceivedBadDenomFlag());
	config.SetReceivedBadDenomFlag(true);
	ASSERT_TRUE(config.GetReceivedBadDenomFlag());
}


/// <summary>
/// Functional test for:
///     CConfig::GetSubscriptionDataResponse()
/// string CConfig::GetSubscriptionDataResponse(const std::string& topic, const std::string& whereId) 
/// </summary>
TEST(ConfigUnitTests, GetSubscriptionDataResponse)
{
	// Setup			
	CConfig config(false, "");
	config.SetSlotNumber(0);
	string expected = 
		"{\"SlotNumber\":0,\"SlotMastID\":0,\"Host\":\"\",\"Port\":0,\"MyIPAddress\":\"0.0.0.0\",\"ActualIPAddress\":\"0.0.0.0\","
		"\"DHCP\":false,\"SubnetMask\":\"0.0.0.0\",\"DefaultGateway\":\"0.0.0.0\",\"PreferredDNSServer\":\"0.0.0.0\","
		"\"AlternateDNSServer\":\"0.0.0.0\",\"Domain\":\"\",\"MacAddress\":\"\",\"MeterDenom\":\"$0.03\",\"GameDenom\":\"$0.01\","
		"\"PersonalBankerDenom\":\"$0.03\",\"BatteryStatus\":\"Unknown\",\"IsLcd\":false,\"FillAmountRequired\":false,"
		"\"A8ResetToCredit\":false,\"OsVersion\":\"\",\"SentinelVersion\":\"0.0.0.0\",\"SASVersion\":\"\",\"ImageChecksum\":\"0\","
		"\"CarrierFirmwareVersion\":\"\",\"TouchFirmwareVersion\":\"\",\"TargetCarrierFirmwareVersion\":\"\",\"Backlight\":0,"
		"\"Volume\":0,\"ExternalCreditVerificationEnabled\":false,\"SentinelOptions\":[{\"Label\":\"Display Name\",\"Value\":\"No\"},"
		"{\"Label\":\"Display Comp Total\",\"Value\":\"No\"},{\"Label\":\"Display Point Total\",\"Value\":\"No\"},"
		"{\"Label\":\"Display Point Total As Cash\",\"Value\":\"No\"},{\"Label\":\"Display Point Total Only\",\"Value\":\"No\"},"
		"{\"Label\":\"Countdown xxx / Session xxx\",\"Value\":\"No\"},{\"Label\":\"Good Luck! / Session $x.xx\",\"Value\":\"No\"},"
		"{\"Label\":\"Good Luck! / Session xxx\",\"Value\":\"No\"},{\"Label\":\"Total xxx / Session xxx\",\"Value\":\"No\"},"
		"{\"Label\":\"Total $x.xx / Session $x.xx\",\"Value\":\"No\"},{\"Label\":\"Point Total As Cash\",\"Value\":\"No\"},"
		"{\"Label\":\"Disp Partial Pts(Theo Win)\",\"Value\":\"No\"},{\"Label\":\"Display Congrats Message\",\"Value\":\"No\"},"
		"{\"Label\":\"Virtual Sessions\",\"Value\":\"No\"},{\"Label\":\"Voucher Out\",\"Value\":\"No\"},{\"Label\":\"Voucher In\",\"Value\":\"No\"},"
		"{\"Label\":\"Voucher Promo In\",\"Value\":\"No\"},{\"Label\":\"Coupon Promo In\",\"Value\":\"No\"},"
		"{\"Label\":\"WAT In(PBT Cash Down)\",\"Value\":\"No\"},{\"Label\":\"CEP In(PBT Pts to Cash Dwn)\",\"Value\":\"No\"},"
		"{\"Label\":\"NCEP In(Pts To Promo Dwn)\",\"Value\":\"No\"},{\"Label\":\"NCEP In(PBT Promo $ Dwn)\",\"Value\":\"No\"},"
		"{\"Label\":\"WAT Out(PBT Cash Up)\",\"Value\":\"No\"},{\"Label\":\"CEP Out(CEP Credits Up)\",\"Value\":\"No\"},"
		"{\"Label\":\"NCEP Out Manual\",\"Value\":\"No\"},{\"Label\":\"Fill Amount\",\"Value\":\"No\"},{\"Label\":\"Hopper Level\",\"Value\":\"No\"},"
		"{\"Label\":\"NCEP Auto Upload\",\"Value\":\"No\"},{\"Label\":\"CEP Auto Upload\",\"Value\":\"No\"},"
		"{\"Label\":\"WAT Auto Upload\",\"Value\":\"No\"},{\"Label\":\"Accrue Points For NCEP\",\"Value\":\"No\"},"
		"{\"Label\":\"Accrue Comp For NCEP\",\"Value\":\"No\"},{\"Label\":\"Unattended Taxable Jackpot\",\"Value\":\"No\"},"
		"{\"Label\":\"Mini Virtual Sessions\",\"Value\":\"No\"},{\"Label\":\"Enable ONE LINK\",\"Value\":\"No\"},"
		"{\"Label\":\"Sub-Game Point Calcs\",\"Value\":\"No\"},{\"Label\":\"PBT Lock Check\",\"Value\":\"No\"},"
		"{\"Label\":\"Enable MobileConnect Communication\",\"Value\":\"No\"},{\"Label\":\"Remote Debugging\",\"Value\":\"No\"},"
		"{\"Label\":\"Partial PBT Cash Up\",\"Value\":\"No\"},{\"Label\":\"Cashout To PBT\",\"Value\":\"No\"},"
		"{\"Label\":\"External Credit Verification\",\"Value\":\"No\"}],\"GlobalOptions\":[{\"Label\":\"Ticketing enabled\",\"Value\":\"No\"},"
		"{\"Label\":\"Sys generated validation\",\"Value\":\"No\"},{\"Label\":\"Ticket buffering enabled\",\"Value\":\"No\"},"
		"{\"Label\":\"PBT promo enabled\",\"Value\":\"No\"},{\"Label\":\"PBT points enabled\",\"Value\":\"No\"},"
		"{\"Label\":\"PBT cash enabled\",\"Value\":\"No\"},{\"Label\":\"Cash transfer tracking\",\"Value\":\"No\"},"
		"{\"Label\":\"Coin In Accounting\",\"Value\":\"No\"},{\"Label\":\"Coin Out Accounting\",\"Value\":\"No\"},"
		"{\"Label\":\"Theoretical Accounting\",\"Value\":\"No\"},{\"Label\":\"Employee PIN Required\",\"Value\":\"No\"},"
		"{\"Label\":\"SeaPass Cards Enabled\",\"Value\":\"No\"},{\"Label\":\"Accrue Points For NCEP\",\"Value\":\"No\"},"
		"{\"Label\":\"Accrue Comp For NCEP\",\"Value\":\"No\"},{\"Label\":\"Sub-Game Point Calcs\",\"Value\":\"No\"},"
		"{\"Label\":\"Excessive Voucher Out\",\"Value\":\"No\"},{\"Label\":\"Partial PBT Cash Up\",\"Value\":\"No\"},"
		"{\"Label\":\"Auto Upload Credits\",\"Value\":\"No\"},{\"Label\":\"Cashout To PBT\",\"Value\":\"No\"},"
		"{\"Label\":\"Tax Withholding Enabled\",\"Value\":\"No\"}],\"AutoCardOutCountdownSeconds\":0,\"AutoCardOutSecToAlert\":0,"
		"\"nConnectUrl\":\"\",\"nConnectToken\":\"\",\"AvatarUrl\":\"\",\"AvatarToken\":\"\",\"RequestServiceUrl\":\"\","
		"\"RequestServiceToken\":\"\",\"DrinksOnTapUrl\":\"\",\"DrinksOnTapToken\":\"\",\"SocialDistanceUrl\":\"\",\"SocialDistanceToken\":\"\","
		"\"SocialDistanceIgnoreDirty\":false,\"SocialDistanceManualCleanEnabled\":false,\"SocialDistanceCountdownDelay\":0,\"RemoteUIEnabled\":false}";

	string result = config.GetSubscriptionDataResponse(NO_EXTENSIBLE_SETTINGS_TOPIC, "");
	ASSERT_EQ(expected, result);
}

/// <summary>
/// Functional test for:
///     CConfig::GetSubscriptionDataResponse(NO_EXTENSIBLE_SETTINGS_TOPIC)
/// string CConfig::GetSubscriptionDataResponse(const std::string& topic, const std::string& whereId) 
/// </summary>
TEST(ConfigUnitTests, GetSubscriptionDataResponse_NoExtensibleSettings)
{
	// Setup			
	CConfig config(false, "");
	config.SetSlotNumber(0);
	string expected = 
		"{\"SlotNumber\":0,\"SlotMastID\":0,\"Host\":\"\",\"Port\":0,\"MyIPAddress\":\"0.0.0.0\",\"ActualIPAddress\":\"0.0.0.0\",\"DHCP\":false,"
		"\"SubnetMask\":\"0.0.0.0\",\"DefaultGateway\":\"0.0.0.0\",\"PreferredDNSServer\":\"0.0.0.0\",\"AlternateDNSServer\":\"0.0.0.0\","
		"\"Domain\":\"\",\"MacAddress\":\"\",\"MeterDenom\":\"$0.03\",\"GameDenom\":\"$0.01\",\"PersonalBankerDenom\":\"$0.03\","
		"\"BatteryStatus\":\"Unknown\",\"IsLcd\":false,\"FillAmountRequired\":false,\"A8ResetToCredit\":false,\"OsVersion\":\"\","
		"\"SentinelVersion\":\"0.0.0.0\",\"SASVersion\":\"\",\"ImageChecksum\":\"0\",\"CarrierFirmwareVersion\":\"\",\"TouchFirmwareVersion\":\"\","
		"\"TargetCarrierFirmwareVersion\":\"\",\"Backlight\":0,\"Volume\":0,\"ExternalCreditVerificationEnabled\":false,"
		"\"SentinelOptions\":[{\"Label\":\"Display Name\",\"Value\":\"No\"},{\"Label\":\"Display Comp Total\",\"Value\":\"No\"},"
		"{\"Label\":\"Display Point Total\",\"Value\":\"No\"},{\"Label\":\"Display Point Total As Cash\",\"Value\":\"No\"},"
		"{\"Label\":\"Display Point Total Only\",\"Value\":\"No\"},{\"Label\":\"Countdown xxx / Session xxx\",\"Value\":\"No\"},"
		"{\"Label\":\"Good Luck! / Session $x.xx\",\"Value\":\"No\"},{\"Label\":\"Good Luck! / Session xxx\",\"Value\":\"No\"},"
		"{\"Label\":\"Total xxx / Session xxx\",\"Value\":\"No\"},{\"Label\":\"Total $x.xx / Session $x.xx\",\"Value\":\"No\"},"
		"{\"Label\":\"Point Total As Cash\",\"Value\":\"No\"},{\"Label\":\"Disp Partial Pts(Theo Win)\",\"Value\":\"No\"},"
		"{\"Label\":\"Display Congrats Message\",\"Value\":\"No\"},{\"Label\":\"Virtual Sessions\",\"Value\":\"No\"},"
		"{\"Label\":\"Voucher Out\",\"Value\":\"No\"},{\"Label\":\"Voucher In\",\"Value\":\"No\"},{\"Label\":\"Voucher Promo In\",\"Value\":\"No\"},"
		"{\"Label\":\"Coupon Promo In\",\"Value\":\"No\"},{\"Label\":\"WAT In(PBT Cash Down)\",\"Value\":\"No\"},"
		"{\"Label\":\"CEP In(PBT Pts to Cash Dwn)\",\"Value\":\"No\"},{\"Label\":\"NCEP In(Pts To Promo Dwn)\",\"Value\":\"No\"},"
		"{\"Label\":\"NCEP In(PBT Promo $ Dwn)\",\"Value\":\"No\"},{\"Label\":\"WAT Out(PBT Cash Up)\",\"Value\":\"No\"},"
		"{\"Label\":\"CEP Out(CEP Credits Up)\",\"Value\":\"No\"},{\"Label\":\"NCEP Out Manual\",\"Value\":\"No\"},"
		"{\"Label\":\"Fill Amount\",\"Value\":\"No\"},{\"Label\":\"Hopper Level\",\"Value\":\"No\"},{\"Label\":\"NCEP Auto Upload\",\"Value\":\"No\"},"
		"{\"Label\":\"CEP Auto Upload\",\"Value\":\"No\"},{\"Label\":\"WAT Auto Upload\",\"Value\":\"No\"},"
		"{\"Label\":\"Accrue Points For NCEP\",\"Value\":\"No\"},{\"Label\":\"Accrue Comp For NCEP\",\"Value\":\"No\"},"
		"{\"Label\":\"Unattended Taxable Jackpot\",\"Value\":\"No\"},{\"Label\":\"Mini Virtual Sessions\",\"Value\":\"No\"},"
		"{\"Label\":\"Enable ONE LINK\",\"Value\":\"No\"},{\"Label\":\"Sub-Game Point Calcs\",\"Value\":\"No\"},"
		"{\"Label\":\"PBT Lock Check\",\"Value\":\"No\"},{\"Label\":\"Enable Floor Communication\",\"Value\":\"No\"},"
		"{\"Label\":\"Remote Debugging\",\"Value\":\"No\"},{\"Label\":\"Partial PBT Cash Up\",\"Value\":\"No\"},"
		"{\"Label\":\"Cashout To PBT\",\"Value\":\"No\"},{\"Label\":\"External Credit Verification\",\"Value\":\"No\"}],"
		"\"GlobalOptions\":[{\"Label\":\"Ticketing enabled\",\"Value\":\"No\"},{\"Label\":\"Sys generated validation\",\"Value\":\"No\"},"
		"{\"Label\":\"Ticket buffering enabled\",\"Value\":\"No\"},{\"Label\":\"PBT promo enabled\",\"Value\":\"No\"},"
		"{\"Label\":\"PBT points enabled\",\"Value\":\"No\"},{\"Label\":\"PBT cash enabled\",\"Value\":\"No\"},"
		"{\"Label\":\"Cash transfer tracking\",\"Value\":\"No\"},{\"Label\":\"Coin In Accounting\",\"Value\":\"Yes\"},"
		"{\"Label\":\"Coin Out Accounting\",\"Value\":\"No\"},{\"Label\":\"Theoretical Accounting\",\"Value\":\"No\"},"
		"{\"Label\":\"Employee PIN Required\",\"Value\":\"No\"},{\"Label\":\"SeaPass Cards Enabled\",\"Value\":\"No\"},"
		"{\"Label\":\"Accrue Points For NCEP\",\"Value\":\"No\"},{\"Label\":\"Accrue Comp For NCEP\",\"Value\":\"No\"},"
		"{\"Label\":\"Sub-Game Point Calcs\",\"Value\":\"No\"},{\"Label\":\"Excessive Voucher Out\",\"Value\":\"No\"},"
		"{\"Label\":\"Partial PBT Cash Up\",\"Value\":\"No\"},{\"Label\":\"Auto Upload Credits\",\"Value\":\"No\"},"
		"{\"Label\":\"Cashout To PBT\",\"Value\":\"No\"},{\"Label\":\"Tax Withholding Enabled\",\"Value\":\"No\"}],\"RemoteUIEnabled\":false}";

	string result = config.GetSubscriptionDataResponse(NO_EXTENSIBLE_SETTINGS_TOPIC, "");
	ASSERT_EQ(expected, result);
}

TEST(ConfigUnitTests, UseSDSCardFormatTest)
{
	CConfig config(false, "");

	// default check
	ASSERT_FALSE(config.GetUseSDSCardFormat());

	// enabled check
	config.SetUseSDSCardFormat(true);
	ASSERT_TRUE(config.GetUseSDSCardFormat());

	// disabled check
	config.SetUseSDSCardFormat(false);
	ASSERT_FALSE(config.GetUseSDSCardFormat());
}