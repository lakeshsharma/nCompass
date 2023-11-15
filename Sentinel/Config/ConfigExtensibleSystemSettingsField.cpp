#include "stdafx.h"
#include "ConfigExtensibleSystemSettingsField.h"

// Sentinel includes.
#include "Logging/LogString.h"
#include "Logging/Logger.h"

using namespace std;

const string AUTO_UPLOAD_DELAY_SECONDS("AutoUploadDelaySeconds");
const string AUTO_CARDOUT_SECONDS_TO_COUNTDOWN_CREDITS("AutoCardOutSecToCountdownCredits");
const string AUTO_CARDOUT_SECONDS_TO_COUNTDOWN_NO_CREDITS("AutoCardOutSecToCountdownNoCrdts");
const string AUTO_CARDOUT_COUNTDOWN_SECONDS("AutoCardOutCountdownSeconds");
const string AUTO_CARDOUT_SECONDS_TO_ALERT("AutoCardOutSecToAlert");
const string AUTO_UPLOAD_CREDITS_KEY_NAME("AutoUploadCredits");
const string CASHOUT_TO_HOST_KEY_NAME("CashoutToPBT");
const string UTJ_TAX_WITHHOLD_KEY_NAME("UTJTaxWithhold");
const string FEDERAL_TAX_WITHHOLD_PERCENTAGE_KEY_NAME("FedTaxWithholdPercent");
const string STATE_TAX_WITHHOLD_PERCENTAGE_KEY_NAME("StateTaxWithholdPercent");
const string POLLER_LOAD_BALANCE_TIME_KEY_NAME("PollerLoadBalanceTime");
const string POLLER_LOAD_BALANCE_RECONNECT_KEY_NAME("Reconnect");
const string MAINTENANCE_MODE_KEY_NAME("MaintenanceMode");
const string COMBINE_ZIP_LOGS_KEY_NAME("CombineZipLogs");
const string USE_SDS_CARD_FORMAT_KEY_NAME("SDSCARDFULLSUPPORT");
const string NUMBER_OF_SUBGAMES_MISMATCH_CHECK("NumberOfSubgamesMismatchCheck");
const string COUNTDOWN_DISPLAY_INTERVAL("CountdownDisplayIntervalInSec");
const string POINTS_COUNTDOWN_CONTROL_BY_UNIVERSE("CountdownControlByUniverse");
const string NCONNECT_URL("nConnectUrl");
const string NCONNECT_TOKEN("nConnectToken");
const string AVATAR_URL("AvatarUrl");
const string AVATAR_TOKEN("AvatarToken");
const string REQUEST_SERVICE_URL("RequestServiceUrl");
const string REQUEST_SERVICE_TOKEN("RequestServiceToken");
const string DRINKS_ON_TAP_URL("DrinksOnTapUrl");
const string DRINKS_ON_TAP_TOKEN("DrinksOnTapToken");
const string ARCHIVE_LOG_DESCRIPTION("ArchiveLogDescription");
const string REBOOT_ASAP_THRESHOLD_MB("RebootAsapThresholdInMb");
const string REBOOT_WARNING_THRESHOLD_MB("RebootWarningThresholdMb");
const string REBOOT_UI_ASAP_THRESHOLD_MB("RestartUIAsapThresholdMb");
const string REBOOT_UI_AFTER_DROP_THRESHOLD_MB("RestartUIAfterDropThresholdMb");
const string RESTART_UI_AFTER_X_DAYS("RestartUIAfterxDays");
const string RESTART_UI_DELAY_HOURS("RestartUIDelayHours");
const string SAS_READ_TIMEOUT_MILLISECONDS("SASReadTimeOutMilliseconds");
const string METER_ROLLOVER_SYNC_ENABLED_KEY_NAME("MeterRolloverSyncEnabled");
const string METER_ROLLOVER_TIME_KEY_NAME("MeterRolloverTime");
const string TICKET_IN_RESEND_NAME("TicketInResend");
const string ENABLE_LOCATION_SERVICES_NAME("EnableLocationServices");
const string ENABLED_VALUES("1YyYesyesTruetrue");
const string SOCIAL_DISTANCE_IGNORE_DIRTY("SocialDistanceIgnoreDirty");
const string SOCIAL_DISTANCE_COUNTDOWN_DELAY("SocialDistanceCountdownDelay");
const string SOCIAL_DISTANCE_MANUAL_CLEAN_ENABLED("SocialDistanceManualCleanEnabled");
const string SOCIAL_DISTANCE_URL("SocialDistanceUrl");
const string SOCIAL_DISTANCE_TOKEN("SocialDistanceToken");

const unsigned int ASCII_VALUE_0 = 48;
const unsigned int ASCII_VALUE_9 = 57;

const DWORD MIN_SASREAD_TIMEOUT_MILLISECONDS(100);

CConfigExtensibleSystemSettingsField::CConfigExtensibleSystemSettingsField(byte* buffer, int length) :
	CConfigField(EXTENSIBLE_SYSTEM_SETTINGS),
	m_autoUploadCredits(false),
	m_cashoutToHost(false),
	m_taxWithholdingEnabled(false),
	m_federalTaxWithholdingPercent(0),
	m_stateTaxWithholdingPercent(0),
	m_PollerLoadBalanceTime_Hour(INVALID_TIME),
	m_PollerLoadBalanceTime_Minute(INVALID_TIME),
	m_pollerReconnect(false),
	m_maintenanceMode(false),
	m_combineZipLogs(true),
	m_useSDSCardFormat(false),
	m_meterRolloverSyncEnabled(false),
	m_meterRolloverTimeHour(INVALID_TIME),
	m_meterRolloverTimeMinute(INVALID_TIME),
	m_meterRolloverTimeSecond(INVALID_TIME),
	m_autoUploadDelaySeconds(DEFAULT_AUTO_UPLOAD_DELAY_SECONDS),
	m_autoCardOutSecToCountdownCredits(0),
	m_autoCardOutSecToCountdownNoCrdts(0),
	m_autoCardOutCountdownSeconds(0),
	m_autoCardOutSecToAlert(0),
	m_numberOfSubgamesMismatchCheck(DEFAULT_NUMBER_OF_SUBGAMES_MISMATCH_CHECK),
	m_countdownDisplayIntervalInSec(DEFAULT_COUNTDOWN_DISPLAY_INTERVAL),
	m_rebootAsapThresholdInMb(DEFAULT_REBOOT_ASAP_THRESHOLD_MB),
	m_rebootWarningThresholdMb(DEFAULT_REBOOT_WARNING_THRESHOLD_MB),
	m_restartUIAsapThresholdMb(DEFAULT_REBOOT_UI_ASAP_THRESHOLD_MB),
	m_restartUIAfterDropThresholdMb(DEFAULT_REBOOT_UI_AFTER_DROP_THRESHOLD_MB),
	m_restartUIAfterxDays(DEFAULT_RESTART_UI_AFTER_X_DAYS),
	m_restartUIDelayHours(DEFAULT_RESTART_UI_DELAY_HOURS),
	m_sasReadTimeOutMultiplier(DEFAULT_SAS_READ_TIMEOUT_MULTIPLIER),
	m_socialDistanceCountdownDelay(25),
	m_pointsCountdownControlByUniverse(false),
	m_ticketInResend(false),
	m_enableLocationServices(false),
	m_socialDistanceIgnoreDirty(false),
	m_socialDistanceManualCleanEnabled(false),
	m_socialDistanceUrl(""),
	m_socialDistanceToken("") 
{	
	//{TotalKeyCount} {KeyNameLength}KeyName{ ValueLength }Value[{KeyNameLength}KeyName{ ValueLength }Value....]
	//	- {TotalKeyCount} size 2 bytes
	//	- {KeyNameLength} size 1 byte
	//	- KeyName size would be defined by KeyNameLength
	//	- {ValueLength} size 1 byte
	//	- Value size would be defined by ValueLength
	WORD totalKeyCount(0);
	DWORD offset(0);
	memcpy_s(&totalKeyCount, sizeof(totalKeyCount), &buffer[offset], sizeof(totalKeyCount));
	offset += sizeof(totalKeyCount);
	LogStringNoFormat(POLLER_LOG, FormatString("EXTENSIBLE_SYSTEM_SETTINGS totalKeyCount=%u", totalKeyCount).c_str());
	for (int index = 0; index < totalKeyCount; ++index)
	{
		string keyName, value;
		byte valueLength(0), keyNameLength(0);
		memcpy_s(&keyNameLength, sizeof(keyNameLength), &buffer[offset], sizeof(keyNameLength));
		offset += sizeof(keyNameLength);
		if (keyNameLength > 0)
		{
			keyName.assign(reinterpret_cast<char const*>(&buffer[offset]), (size_t)keyNameLength);
			offset += keyNameLength;
			memcpy_s(&valueLength, sizeof(valueLength), &buffer[offset], sizeof(valueLength));
			offset += sizeof(valueLength);
			if (valueLength > 0)
			{
				try
				{
					value.assign(reinterpret_cast<char const*>(&buffer[offset]), (size_t)valueLength);
					
					bool settingMatched(true);

					if (AUTO_UPLOAD_CREDITS_KEY_NAME == keyName)
					{
						m_autoUploadCredits = (string::npos != ENABLED_VALUES.find(value));
						LogStringNoFormat(POLLER_LOG, FormatString("m_autoUploadCredits=%d", m_autoUploadCredits).c_str());
					}
					else if (CASHOUT_TO_HOST_KEY_NAME == keyName)
					{
						m_cashoutToHost = (string::npos != ENABLED_VALUES.find(value));
						LogStringNoFormat(POLLER_LOG, FormatString("m_cashoutToHost=%d", m_cashoutToHost).c_str());
					}
					else if (UTJ_TAX_WITHHOLD_KEY_NAME == keyName)
					{
						m_taxWithholdingEnabled = (string::npos != ENABLED_VALUES.find(value));
						LogStringNoFormat(POLLER_LOG, FormatString("m_taxWithholdingEnabled=%d", m_taxWithholdingEnabled).c_str());
					}
					else if (FEDERAL_TAX_WITHHOLD_PERCENTAGE_KEY_NAME == keyName)
					{
						m_federalTaxWithholdingPercent = std::stof(value);
						LogStringNoFormat(POLLER_LOG, FormatString("m_federalTaxWithholdingPercent=%f", m_federalTaxWithholdingPercent).c_str());
					}
					else if (STATE_TAX_WITHHOLD_PERCENTAGE_KEY_NAME == keyName)
					{
						m_stateTaxWithholdingPercent = std::stof(value);
						LogStringNoFormat(POLLER_LOG, FormatString("m_stateTaxWithholdingPercent=%f", m_stateTaxWithholdingPercent).c_str());
					}
					else if (POLLER_LOAD_BALANCE_TIME_KEY_NAME == keyName)
					{
						// NOTE: we assume that the time will be in hh:mm format.
						if ((value.size() == 5)
							&& (value.at(0) >= ASCII_VALUE_0 && value.at(0) <= ASCII_VALUE_9)
							&& (value.at(1) >= ASCII_VALUE_0 && value.at(1) <= ASCII_VALUE_9)
							&& (value.at(2) == ':')
							&& (value.at(3) >= ASCII_VALUE_0 && value.at(3) <= ASCII_VALUE_9)
							&& (value.at(4) >= ASCII_VALUE_0 && value.at(4) <= ASCII_VALUE_9))
						{
							DWORD hour = std::stoul(value.substr(0, 2));
							DWORD minute = std::stoul(value.substr(3));
							if (hour < 24 && minute < 60)
							{
								m_PollerLoadBalanceTime_Hour = (BYTE)hour;
								m_PollerLoadBalanceTime_Minute = (BYTE)minute;
								LogStringNoFormat(POLLER_LOG, FormatString("m_PollerLoadBalanceTime_Hour=%u, m_PollerLoadBalanceTime_Minute=%u",
									m_PollerLoadBalanceTime_Hour, m_PollerLoadBalanceTime_Minute).c_str());
							}
							else
							{
								LogString(POLLER_LOG, "Poller load balancing time is invalid. Data = '%s'", value.c_str());
							}
						}
						else
						{
							LogString(POLLER_LOG, "Poller load balancing time is in wrong format. Data = '%s'", value.c_str());
						}
					}
					else if (POLLER_LOAD_BALANCE_RECONNECT_KEY_NAME == keyName)
					{
						m_pollerReconnect = (string::npos != ENABLED_VALUES.find(value));
						LogStringNoFormat(POLLER_LOG, FormatString("m_pollerReconnect=%d", m_pollerReconnect).c_str());
					}
					else if (MAINTENANCE_MODE_KEY_NAME == keyName)
					{
						m_maintenanceMode = (string::npos != ENABLED_VALUES.find(value));
						LogStringNoFormat(POLLER_LOG, FormatString("m_maintenanceMode=%d", m_maintenanceMode).c_str());
					}					
					else if (COMBINE_ZIP_LOGS_KEY_NAME == keyName)
					{
						m_combineZipLogs = (string::npos != ENABLED_VALUES.find(value));
						LogStringNoFormat(POLLER_LOG, FormatString("m_combineZipLogs=%d", m_combineZipLogs).c_str());
					}
					else if (AUTO_CARDOUT_SECONDS_TO_COUNTDOWN_CREDITS == keyName)
					{
						m_autoCardOutSecToCountdownCredits = stoul(value, nullptr, 10);
						LogStringNoFormat(POLLER_LOG, FormatString("CConfigExtensibleSystemSettingsField m_autoCardOutSecToCountdownCredits=%u", m_autoCardOutSecToCountdownCredits).c_str());
					}
					else if (AUTO_UPLOAD_DELAY_SECONDS == keyName)
					{
						m_autoUploadDelaySeconds = stoul(value, nullptr, 10);
						LogStringNoFormat(POLLER_LOG, FormatString("CConfigExtensibleSystemSettingsField m_autoUploadDelaySeconds=%u", m_autoUploadDelaySeconds).c_str());
					}
					else if (AUTO_CARDOUT_SECONDS_TO_COUNTDOWN_NO_CREDITS == keyName)
					{
						m_autoCardOutSecToCountdownNoCrdts = stoul(value, nullptr, 10);
						LogStringNoFormat(POLLER_LOG, FormatString("CConfigExtensibleSystemSettingsField m_autoCardOutSecToCountdownNoCrdts=%u", m_autoCardOutSecToCountdownNoCrdts).c_str());
					}
					else if (AUTO_CARDOUT_COUNTDOWN_SECONDS == keyName)
					{
						m_autoCardOutCountdownSeconds = stoul(value, nullptr, 10);
						LogStringNoFormat(POLLER_LOG, FormatString("CConfigExtensibleSystemSettingsField m_autoCardOutCountdownSeconds=%u", m_autoCardOutCountdownSeconds).c_str());
					}
					else if (AUTO_CARDOUT_SECONDS_TO_ALERT == keyName)
					{
						m_autoCardOutSecToAlert = stoul(value, nullptr, 10);
						LogStringNoFormat(POLLER_LOG, FormatString("CConfigExtensibleSystemSettingsField m_autoCardOutSecToAlert=%u", m_autoCardOutSecToAlert).c_str());
					}
					else if (NUMBER_OF_SUBGAMES_MISMATCH_CHECK == keyName)
					{
						m_numberOfSubgamesMismatchCheck = stoul(value, nullptr, 10);
						LogStringNoFormat(POLLER_LOG, FormatString("CConfigExtensibleSystemSettingsField m_numberOfSubgamesMismatchCheck=%u", m_numberOfSubgamesMismatchCheck).c_str());
					}
					else if (COUNTDOWN_DISPLAY_INTERVAL == keyName)
					{
						m_countdownDisplayIntervalInSec = stoul(value, nullptr, 10);
						LogStringNoFormat(POLLER_LOG, FormatString("CConfigExtensibleSystemSettingsField m_countdownDisplayIntervalInSec=%u", m_countdownDisplayIntervalInSec).c_str());
					}
					else if (POINTS_COUNTDOWN_CONTROL_BY_UNIVERSE == keyName)
					{
						m_pointsCountdownControlByUniverse = (string::npos != ENABLED_VALUES.find(value));
						LogStringNoFormat(POLLER_LOG, FormatString("CConfigExtensibleSystemSettingsField m_pointsCountdownControlByUniverse=%d", m_pointsCountdownControlByUniverse).c_str());
					}
					else if (ARCHIVE_LOG_DESCRIPTION == keyName)
					{
						m_archiveLogDescription = value;
						LogStringNoFormat(POLLER_LOG, FormatString("CConfigExtensibleSystemSettingsField m_archiveLogDescription=%s", m_archiveLogDescription.c_str()).c_str());
					}
					else if (REBOOT_ASAP_THRESHOLD_MB == keyName)
					{
						m_rebootAsapThresholdInMb = stoul(value, nullptr, 10);
						LogStringNoFormat(POLLER_LOG, FormatString("CConfigExtensibleSystemSettingsField m_rebootAsapThresholdInMb=%u", m_rebootAsapThresholdInMb).c_str());
					}
					else if (REBOOT_WARNING_THRESHOLD_MB == keyName)
					{
						m_rebootWarningThresholdMb = stoul(value, nullptr, 10);
						LogStringNoFormat(POLLER_LOG, FormatString("CConfigExtensibleSystemSettingsField m_rebootWarningThresholdMb=%u", m_rebootWarningThresholdMb).c_str());
					}
					else if (REBOOT_UI_ASAP_THRESHOLD_MB == keyName)
					{
						m_restartUIAsapThresholdMb = stoul(value, nullptr, 10);
						LogStringNoFormat(POLLER_LOG, FormatString("CConfigExtensibleSystemSettingsField m_restartUIAsapThresholdMb=%u", m_restartUIAsapThresholdMb).c_str());
					}
					else if (REBOOT_UI_AFTER_DROP_THRESHOLD_MB == keyName)
					{
						m_restartUIAfterDropThresholdMb = stoul(value, nullptr, 10);
						LogStringNoFormat(POLLER_LOG, FormatString("CConfigExtensibleSystemSettingsField m_restartUIAfterDropThresholdMb=%u", m_restartUIAfterDropThresholdMb).c_str());
					}					
					else if (RESTART_UI_AFTER_X_DAYS == keyName)
					{
						m_restartUIAfterxDays = stoul(value, nullptr, 10);
						LogStringNoFormat(POLLER_LOG, FormatString("CConfigExtensibleSystemSettingsField m_restartUIAfterxDays=%u", m_restartUIAfterxDays).c_str());
					}
					else if (RESTART_UI_DELAY_HOURS == keyName)
					{
						m_restartUIDelayHours = stoul(value, nullptr, 10);
						LogStringNoFormat(POLLER_LOG, FormatString("CConfigExtensibleSystemSettingsField m_restartUIDelayHours=%u", m_restartUIDelayHours).c_str());
					}
					else if (SAS_READ_TIMEOUT_MILLISECONDS == keyName)
					{
						m_sasReadTimeOutMultiplier = stoul(value, nullptr, 10) / MIN_SASREAD_TIMEOUT_MILLISECONDS;
						LogStringNoFormat(POLLER_LOG, FormatString("CConfigExtensibleSystemSettingsField m_sasReadTimeOutMultiplier=%u", m_sasReadTimeOutMultiplier).c_str());
					}
					else if (USE_SDS_CARD_FORMAT_KEY_NAME == keyName)
					{
						m_useSDSCardFormat = (string::npos != ENABLED_VALUES.find(value));
						LogStringNoFormat(POLLER_LOG, FormatString("m_useSDSCardFormat=%d", m_useSDSCardFormat).c_str());
					}
					else if (METER_ROLLOVER_SYNC_ENABLED_KEY_NAME == keyName)
					{
						m_meterRolloverSyncEnabled = (string::npos != ENABLED_VALUES.find(value));
						LogStringNoFormat(POLLER_LOG, FormatString("CConfigExtensibleSystemSettingsField m_meterRolloverSyncEnabled=%d", m_meterRolloverSyncEnabled).c_str());
					}
					else if (METER_ROLLOVER_TIME_KEY_NAME == keyName)
					{
						// NOTE: we assume that the time will be in hh:mm:ss format.
						if ((value.size() == 8)
							&& (value.at(0) >= ASCII_VALUE_0 && value.at(0) <= ASCII_VALUE_9)
							&& (value.at(1) >= ASCII_VALUE_0 && value.at(1) <= ASCII_VALUE_9)
							&& (value.at(2) == ':')
							&& (value.at(3) >= ASCII_VALUE_0 && value.at(3) <= ASCII_VALUE_9)
							&& (value.at(4) >= ASCII_VALUE_0 && value.at(4) <= ASCII_VALUE_9)
							&& (value.at(5) == ':')
							&& (value.at(6) >= ASCII_VALUE_0 && value.at(6) <= ASCII_VALUE_9)
							&& (value.at(7) >= ASCII_VALUE_0 && value.at(7) <= ASCII_VALUE_9))
						{
							DWORD hour = std::stoul(value.substr(0, 2));
							DWORD minute = std::stoul(value.substr(3, 5));
							DWORD second = std::stoul(value.substr(6));
							if (hour < 24 && minute < 60 && second < 60)
							{
								m_meterRolloverTimeHour = (BYTE)hour;
								m_meterRolloverTimeMinute = (BYTE)minute;
								m_meterRolloverTimeSecond = (BYTE)second;
								LogStringNoFormat(POLLER_LOG, FormatString("m_meterRolloverTimeHour=%u, m_meterRolloverTimeMinute=%u m_meterRolloverTimeSecond=%u",
									m_meterRolloverTimeHour, m_meterRolloverTimeMinute, m_meterRolloverTimeSecond).c_str());
							}
							else
							{
								LogString(POLLER_LOG, "keyName=%s is invalid. Data='%s'", keyName.c_str(), value.c_str());
							}
						}
						else
						{
							LogString(POLLER_LOG, "Meter rollover time is in the wrong format. Data = '%s'", value.c_str());
						}
					}
					else if (TICKET_IN_RESEND_NAME == keyName)
					{
						m_ticketInResend = (string::npos != ENABLED_VALUES.find(value));
						LogStringNoFormat(POLLER_LOG, FormatString("m_ticketInResend=%d", m_ticketInResend).c_str());
					}
					else if (ENABLE_LOCATION_SERVICES_NAME == keyName)
					{
						m_enableLocationServices = (string::npos != ENABLED_VALUES.find(value));
						LogStringNoFormat(POLLER_LOG, FormatString("CConfigExtensibleSystemSettingsField m_enableLocationServices=%d", m_enableLocationServices).c_str());
					}
					else if (NCONNECT_URL == keyName)
					{
						m_nConnectUrl = value;
						LogStringNoFormat(POLLER_LOG, FormatString("CConfigExtensibleSystemSettingsField m_nConnectUrl=%s", m_nConnectUrl.c_str()).c_str());
					}
					else if (NCONNECT_TOKEN == keyName)
					{
						m_nConnectToken = value;
						LogStringNoFormat(POLLER_LOG, FormatString("CConfigExtensibleSystemSettingsField m_nConnectToken=%s", m_nConnectToken.c_str()).c_str());
					}
					else if (AVATAR_URL == keyName)
					{
						m_avatarUrl = value;
						LogStringNoFormat(POLLER_LOG, FormatString("CConfigExtensibleSystemSettingsField m_avatarUrl=%s", m_avatarUrl.c_str()).c_str());
					}
					else if (AVATAR_TOKEN == keyName)
					{
						m_avatarToken = value;
						LogStringNoFormat(POLLER_LOG, FormatString("CConfigExtensibleSystemSettingsField m_avatarToken=%s", m_avatarToken.c_str()).c_str());
					}
					else if (REQUEST_SERVICE_URL == keyName)
					{
						m_requestServiceUrl = value;
						LogStringNoFormat(POLLER_LOG, FormatString("CConfigExtensibleSystemSettingsField m_requestServiceUrl=%s", m_requestServiceUrl.c_str()).c_str());
					}
					else if (REQUEST_SERVICE_TOKEN == keyName)
					{
						m_requestServiceToken = value;
						LogStringNoFormat(POLLER_LOG, FormatString("CConfigExtensibleSystemSettingsField m_requestServiceToken=%s", m_requestServiceToken.c_str()).c_str());
					}
					else if (DRINKS_ON_TAP_URL == keyName)
					{
						m_drinksOnTapUrl = value;
						LogStringNoFormat(POLLER_LOG, FormatString("CConfigExtensibleSystemSettingsField m_drinksOnTapUrl=%s", m_drinksOnTapUrl.c_str()).c_str());
					}
					else if (DRINKS_ON_TAP_TOKEN == keyName)
					{
						m_drinksOnTapToken = value;
						LogStringNoFormat(POLLER_LOG, FormatString("CConfigExtensibleSystemSettingsField m_drinksOnTapToken=%s", m_drinksOnTapToken.c_str()).c_str());
					}
					else if (SOCIAL_DISTANCE_IGNORE_DIRTY == keyName)
					{
						m_socialDistanceIgnoreDirty = (string::npos != ENABLED_VALUES.find(value));
						LogStringNoFormat(POLLER_LOG, FormatString("CConfigExtensibleSystemSettingsField m_socialDistanceIgnoreDirty=%d", m_socialDistanceIgnoreDirty));
					}
					else if (SOCIAL_DISTANCE_MANUAL_CLEAN_ENABLED == keyName)
					{
						m_socialDistanceManualCleanEnabled = (string::npos != ENABLED_VALUES.find(value));
						LogStringNoFormat(POLLER_LOG, FormatString("CConfigExtensibleSystemSettingsField m_socialDistanceManualCleanEnabled=%d", m_socialDistanceManualCleanEnabled));
					}
					else if (SOCIAL_DISTANCE_COUNTDOWN_DELAY == keyName)
					{
						m_socialDistanceCountdownDelay = stoul(value, nullptr, 10);
						LogStringNoFormat(POLLER_LOG, FormatString("CConfigExtensibleSystemSettingsField m_socialDistanceCountdownDelay=%d", m_socialDistanceCountdownDelay));
					}
					else if (SOCIAL_DISTANCE_URL == keyName)
					{
						m_socialDistanceUrl = value;
						LogStringNoFormat(POLLER_LOG, FormatString("CConfigExtensibleSystemSettingsField m_socialDistanceUrl=%s", m_socialDistanceUrl.c_str()).c_str());
					}
					else if (SOCIAL_DISTANCE_TOKEN == keyName)
					{
						m_socialDistanceToken = value;
						LogStringNoFormat(POLLER_LOG, FormatString("CConfigExtensibleSystemSettingsField m_socialDistanceToken=%s", m_socialDistanceToken.c_str()).c_str());
					}
					else
					{
						settingMatched = false;
						LogStringNoFormat(POLLER_LOG, FormatString("Unknown setting entry keyNameLength=%u keyName=%s valueLength=%u value=%s",
							keyNameLength, keyName.c_str(), valueLength, value.c_str()).c_str());
					}

					if (settingMatched)
					{
						m_extensibleSettings[keyName] = value;
					}

					offset += valueLength;
				}
				catch (const std::invalid_argument& e)
				{
					LogStringNoFormat(ERROR_LOG, FormatString("CConfigExtensibleSystemSettingsField invalid_argument keyNameLength=%u keyName=%s valueLength=%u value=%s exception=%s",
						keyNameLength, keyName.c_str(), valueLength, value.c_str(), e.what()).c_str());
					offset += valueLength;
				}
				catch (const std::out_of_range& e)
				{
					LogStringNoFormat(ERROR_LOG, FormatString("CConfigExtensibleSystemSettingsField Out of Range error keyNameLength=%u keyName=%s valueLength=%u value=%s exception=%s",
						keyNameLength, keyName.c_str(), valueLength, value.c_str(), e.what()).c_str());
					offset += valueLength;
				}
				catch(const std::exception& e)
				{
					LogStringNoFormat(ERROR_LOG, FormatString("CConfigExtensibleSystemSettingsField bad setting entry format keyNameLength=%u keyName=%s valueLength=%u value=%s exception=%s",
						keyNameLength, keyName.c_str(), valueLength, value.c_str(), e.what()).c_str());
					offset += valueLength;
				}
			}
			else
			{
				LogString(POLLER_LOG, "CConfigExtensibleSystemSettingsField valueLength is not greater than 0 for keyName=%s", keyName.c_str());
				break;
			}
		}
		else
		{
			LogString(POLLER_LOG, "CConfigExtensibleSystemSettingsField keyNameLength is not greater than 0");
			break;
		}
	}
}

CConfigExtensibleSystemSettingsField::~CConfigExtensibleSystemSettingsField(void)
{
}

bool CConfigExtensibleSystemSettingsField::AutoUploadCredits() const
{
	return m_autoUploadCredits;
}

bool CConfigExtensibleSystemSettingsField::CashoutToHost() const
{
	return m_cashoutToHost;
}

bool CConfigExtensibleSystemSettingsField::GetTicketInResend() const
{
	return m_ticketInResend;
}

bool CConfigExtensibleSystemSettingsField::GetEnableLocationServices() const
{
	return m_enableLocationServices;
}

bool CConfigExtensibleSystemSettingsField::UJPTaxWithholding() const
{
	return m_taxWithholdingEnabled;
}

float CConfigExtensibleSystemSettingsField::GetFederalTaxWithholdingPercent() const
{
	return m_federalTaxWithholdingPercent;
}

float CConfigExtensibleSystemSettingsField::GetStateTaxWithholdingPercent() const
{
	return m_stateTaxWithholdingPercent;
}

void CConfigExtensibleSystemSettingsField::GetPollerLoadBalanceTime(BYTE &hour, BYTE &minute) const
{
	hour = m_PollerLoadBalanceTime_Hour;
	minute = m_PollerLoadBalanceTime_Minute;
}

bool CConfigExtensibleSystemSettingsField::PollerReconnect() const
{
	return m_pollerReconnect;
}

bool CConfigExtensibleSystemSettingsField::MaintenanceMode() const
{
	return m_maintenanceMode;
}

bool CConfigExtensibleSystemSettingsField::CombineZipLogs() const
{
	return m_combineZipLogs;
}

bool CConfigExtensibleSystemSettingsField::UseSDSCardFormat() const
{
	return m_useSDSCardFormat;
}

DWORD CConfigExtensibleSystemSettingsField::GetAutoUploadDelaySeconds() const
{
	return m_autoUploadDelaySeconds;
}

DWORD CConfigExtensibleSystemSettingsField::GetAutoCardOutSecToCountdownCredits() const
{
	return m_autoCardOutSecToCountdownCredits;
}

DWORD CConfigExtensibleSystemSettingsField::GetAutoCardOutSecToCountdownNoCrdts() const
{
	return m_autoCardOutSecToCountdownNoCrdts;
}

DWORD CConfigExtensibleSystemSettingsField::GetAutoCardOutCountdownSeconds() const
{
	return m_autoCardOutCountdownSeconds;
}

DWORD CConfigExtensibleSystemSettingsField::GetAutoCardOutSecToAlert() const
{
	return m_autoCardOutSecToAlert;
}

DWORD CConfigExtensibleSystemSettingsField::GetNumberOfSubgamesMismatchCheck() const
{
	return m_numberOfSubgamesMismatchCheck;
}

string CConfigExtensibleSystemSettingsField::GetExtensibleSettingsStr() const
{
	string str;
	if (m_extensibleSettings.size() > 0)
	{
		str.append("<b>Extensible System Settings:</b><br>");
		for (const auto& kv : m_extensibleSettings)
		{
			str.append(kv.first);
			str.append(":");
			str.append(kv.second);
			str.append("<br>");
		}
	}

	return str;
}

DWORD CConfigExtensibleSystemSettingsField::GetCountdownDisplayIntervalSeconds() const
{
	return m_countdownDisplayIntervalInSec;
}

bool CConfigExtensibleSystemSettingsField::GetPointsCountdownControlledByUniverse() const
{
	return m_pointsCountdownControlByUniverse;
}

string CConfigExtensibleSystemSettingsField::GetArchiveLogDescriptionStr() const
{
	return m_archiveLogDescription;
}

DWORD CConfigExtensibleSystemSettingsField::GetRebootAsapThresholdInMb() const
{
	return m_rebootAsapThresholdInMb;
}

DWORD CConfigExtensibleSystemSettingsField::GetRebootWarningThresholdMb() const
{
	return m_rebootWarningThresholdMb;
}

DWORD CConfigExtensibleSystemSettingsField::GetRestartUIAsapThresholdMb() const
{
	return m_restartUIAsapThresholdMb;
}

DWORD CConfigExtensibleSystemSettingsField::GetRestartUIAfterDropThresholdMb() const
{
	return m_restartUIAfterDropThresholdMb;
}

DWORD CConfigExtensibleSystemSettingsField::GetRestartUIAfterxDays() const
{
	return m_restartUIAfterxDays;
}

DWORD CConfigExtensibleSystemSettingsField::GetRestartUIDelayHours() const
{
	return m_restartUIDelayHours;
}

DWORD CConfigExtensibleSystemSettingsField::GetSASReadTimeOutMultiplier() const
{
	return m_sasReadTimeOutMultiplier;
}

bool CConfigExtensibleSystemSettingsField::GetMeterRolloverSyncEnabled() const
{
	return m_meterRolloverSyncEnabled;
}

void CConfigExtensibleSystemSettingsField::GetMeterRolloverTime(bool& rolloverTimeSycnEnabled, BYTE& hour, BYTE& minute, BYTE& second) const
{
	rolloverTimeSycnEnabled = m_meterRolloverSyncEnabled;
	hour = m_meterRolloverTimeHour;
	minute = m_meterRolloverTimeMinute;
	second = m_meterRolloverTimeSecond;
}

string CConfigExtensibleSystemSettingsField::GetnConnectUrl() const
{
	return m_nConnectUrl;
}

string CConfigExtensibleSystemSettingsField::GetnConnectToken() const
{
	return m_nConnectToken;
}

string CConfigExtensibleSystemSettingsField::GetAvatarUrl() const
{
	return m_avatarUrl;
}

string CConfigExtensibleSystemSettingsField::GetAvatarToken() const
{
	return m_avatarToken;
}

string CConfigExtensibleSystemSettingsField::GetRequestServiceUrl() const
{
	return m_requestServiceUrl;
}

string CConfigExtensibleSystemSettingsField::GetRequestServiceToken() const
{
	return m_requestServiceToken;
}

string CConfigExtensibleSystemSettingsField::GetDrinksOnTapUrl() const
{
	return m_drinksOnTapUrl;
}

string CConfigExtensibleSystemSettingsField::GetDrinksOnTapToken() const
{
	return m_drinksOnTapToken;
}

string CConfigExtensibleSystemSettingsField::GetSocialDistanceUrl() const
{
	return m_socialDistanceUrl;
}

string CConfigExtensibleSystemSettingsField::GetSocialDistanceToken() const
{
	return m_socialDistanceToken;
}

bool CConfigExtensibleSystemSettingsField::GetSocialDistanceIgnoreDirty() const
{
	return m_socialDistanceIgnoreDirty;
}

bool CConfigExtensibleSystemSettingsField::GetSocialDistanceManualCleanEnabled() const
{
	return m_socialDistanceManualCleanEnabled;
}

DWORD CConfigExtensibleSystemSettingsField::GetSocialDistanceCountdownDelay() const
{
	return m_socialDistanceCountdownDelay;
}
