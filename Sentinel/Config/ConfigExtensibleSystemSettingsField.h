#pragma once
#include "ConfigField.h"

class CConfigExtensibleSystemSettingsField : public CConfigField
{
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="CConfigExtensibleSystemSettingsField"/> class.
	/// </summary>
	CConfigExtensibleSystemSettingsField(byte* buffer, int length);

	/// <summary>
	/// Finalizes an instance of the <see cref="CConfigExtensibleSystemSettingsField"/> class.
	/// </summary>
	~CConfigExtensibleSystemSettingsField(void);

	/// <summary>
	/// Returns true if automatic PersonalBanker Auto Upload Credits from the EGM are enabled, false otherwise.
	/// </summary>
	/// <returns>true if automatic PersonalBanker Auto Upload Credits from the EGM are enabled, false otherwise.</returns>
	bool AutoUploadCredits() const;

	/// <summary>
	/// Returns true if cashout to host feature is enabled, false otherwise.
	/// </summary>
	/// <returns>true if cashout to host feature is enabled, false otherwise.</returns>
	bool CashoutToHost() const;

	/// <summary>
	/// Returns whether ticket in resend flag is enabled.
	/// </summary>
	/// <returns>true if ticket in resend flag is enabled, false otherwise.</returns>	
	bool GetTicketInResend() const;

	/// <summary>
	/// Returns whether location services are enabled.
	/// </summary>
	/// <returns>true if location services are enabled, false otherwise.</returns>	
	bool GetEnableLocationServices() const;
	
	/// <summary>
	/// Returns true if UJP Tax Withholding is enabled, false otherwise.
	/// </summary>
	/// <returns>true if UJP Tax Withholding is enabled, false otherwise.</returns>
	bool UJPTaxWithholding() const;

	/// <summary>
	/// Gets federal tax withholding percentage.
	/// </summary>
	/// <returns>federal tax withholding percentage</returns>
	float GetFederalTaxWithholdingPercent() const;

	/// <summary>
	/// Gets state tax withholding percentage.
	/// </summary>
	/// <returns>state tax withholding percentage</returns>
	float GetStateTaxWithholdingPercent() const;

	/// <summary>
	/// Gets the poller load balance time.
	/// </summary>
	void GetPollerLoadBalanceTime(BYTE &hour, BYTE &minute) const;

	/// <summary>
	/// Returns true if poller is asking us to reconnect to itself, false otherwise.
	/// </summary>
	/// <returns>true if poller is asking us to reconnect to itself, false otherwise.</returns>
	bool PollerReconnect() const;

	/// <summary>
	/// Returns true if poller is asking us to enter maintenance mode or refresh it, false otherwise.
	/// </summary>
	/// <returns>true if poller is asking us to enter maintenance mode or refresh it, false otherwise.</returns>
	bool MaintenanceMode() const;

	/// <summary>
	/// Returns true if zip logs should be combined into a single file, false otherwise.
	/// </summary>
	/// <returns>true if zip logs should be combined into a single file, false otherwise.</returns>
	bool CombineZipLogs() const;	

	/// <summary>
	/// Returns true if SDS card format to be used, false otherwise.
	/// </summary>
	/// <returns>true if SDS card format to be used, false otherwise.</returns>
	bool UseSDSCardFormat() const;

	/// <summary>
	/// Returns the auto upload delay in seconds.
	/// </summary>
	/// <returns>the auto upload delay in seconds</returns>
	DWORD GetAutoUploadDelaySeconds() const;

	/// <summary>
	/// Returns the number of seconds of inactivity with credits before displaying countdown inactivity timer.
	/// </summary>
	/// <returns>the number of seconds of inactivity  with credits before displaying countdown inactivity timer</returns>
	DWORD GetAutoCardOutSecToCountdownCredits() const;

	/// <summary>
	/// Returns the number of seconds of inactivity without credits before displaying countdown inactivity timer.
	/// </summary>
	/// <returns>the number of seconds of inactivity  without credits before displaying countdown inactivity timer</returns>
	DWORD GetAutoCardOutSecToCountdownNoCrdts() const;

	/// <summary>
	/// Returns number of seconds to countdown before carding out a player.
	/// </summary>
	/// <returns>the number of seconds to countdown before carding out a player</returns>
	DWORD GetAutoCardOutCountdownSeconds() const;

	/// <summary>
	/// Returns the number of seconds to play the alert sound in the UI.
	/// </summary>
	/// <returns>the number of seconds to play the alert sound in the UI.</returns>	
	DWORD GetAutoCardOutSecToAlert() const;

	/// <summary>
	/// Returns the Number of subgames mismatch check.
	/// </summary>
	/// <returns>the Number of subgames mismatch check.</returns>	
	DWORD GetNumberOfSubgamesMismatchCheck() const;

	/// <summary>
	/// Returns the extensible settings string
	/// </summary>
	/// <returns>the extensible settings string.</returns>	
	std::string GetExtensibleSettingsStr() const;

	/// <summary>
	/// Returns the extended bucket countdown display interval in seconds.
	/// </summary>
	/// <returns>the extended bucket countdown refresh interval in seconds.</returns>	
	DWORD GetCountdownDisplayIntervalSeconds() const;

	/// <summary>
	/// Returns whether points countdowns are controlled by universe or not.
	/// </summary>
	/// <returns>points countdowns are controlled by universe or no.</returns>	
	bool GetPointsCountdownControlledByUniverse() const;

	/// <summary>
	/// Returns the description for archiving logs.
	/// </summary>
	/// <returns>the description for archiving logs.</returns>	
	std::string GetArchiveLogDescriptionStr() const;

	/// <summary>
	/// Returns Reboot ASAP threshold in mb.
	/// </summary>
	/// <returns>Reboot ASAP threshold in mb</returns>	
	DWORD GetRebootAsapThresholdInMb() const;

	/// <summary>
	/// Returns Reboot Warning threshold in mb.
	/// </summary>
	/// <returns>Reboot Warning threshold in mb</returns>	
	DWORD GetRebootWarningThresholdMb() const;

	/// <summary>
	/// Returns UI ASAP threshold in mb.
	/// </summary>
	/// <returns>UI ASAP threshold in mb</returns>	
	DWORD GetRestartUIAsapThresholdMb() const;

	/// <summary>
	/// Returns UI after the drop threshold in mb.
	/// </summary>
	/// <returns>UI after the drop threshold in mb</returns>	
	DWORD GetRestartUIAfterDropThresholdMb() const;
		
	/// <summary>
	/// Returns restart UI after x days value.
	/// </summary>
	/// <returns>restart UI after x days value</returns>	
	DWORD GetRestartUIAfterxDays() const;

	/// <summary>
	/// Returns restart UI delay in hours.
	/// </summary>
	/// <returns>restart UI delay in hours</returns>	
	DWORD GetRestartUIDelayHours() const;

	/// <summary>
	/// Returns sas read timeout multiplier.
	/// </summary>
	/// <returns>sas read timeout multiplier</returns>	
	DWORD GetSASReadTimeOutMultiplier() const;

	/// <summary>
	/// Returns meter rollover sync enabled flag.
	/// </summary>
	/// <returns>true if meter rollover sync flag is enabled, false otherwise.</returns>	
	bool GetMeterRolloverSyncEnabled() const;

	/// <summary>
	/// Gets the meter rollover time.
	/// </summary>
	void GetMeterRolloverTime(bool& rolloverTimeSycnEnabled, BYTE& hour, BYTE& minute, BYTE& second) const;

	/// <summary>
	/// Returns the nConnect URL.
	/// </summary>
	/// <returns>the nConnect URL.</returns>	
	std::string GetnConnectUrl() const;

	/// <summary>
	/// Returns the nConnect token.
	/// </summary>
	/// <returns>the nConnect token.</returns>	
	std::string GetnConnectToken() const;

	/// <summary>
	/// Returns the avatar URL.
	/// </summary>
	/// <returns>the avatar URL.</returns>	
	std::string GetAvatarUrl() const;

	/// <summary>
	/// Returns the avatar token.
	/// </summary>
	/// <returns>the avatar token.</returns>	
	std::string GetAvatarToken() const;

	/// <summary>
	/// Returns the request service URL.
	/// </summary>
	/// <returns>the request service URL.</returns>	
	std::string GetRequestServiceUrl() const;

	/// <summary>
	/// Returns the request service token.
	/// </summary>
	/// <returns>the request service token.</returns>	
	std::string GetRequestServiceToken() const;

	/// <summary>
	/// Returns the drinks on tap URL.
	/// </summary>
	/// <returns>the drinks on tap URL.</returns>	
	std::string GetDrinksOnTapUrl() const;

	/// <summary>
	/// Returns the drinks on tap token.
	/// </summary>
	/// <returns>the drinks on tap token.</returns>	
	std::string GetDrinksOnTapToken() const;

	/// <summary>
	/// Returns Social Distance countdown delay.
	/// </summary>
	/// <returns>Social Distance countdown delay</returns>	
	DWORD GetSocialDistanceCountdownDelay() const;

	/// <summary>
	/// Returns Social Distance ignore dirty.
	/// </summary>
	/// <returns>Social Distance ignore dirty</returns>	
	bool GetSocialDistanceIgnoreDirty() const;

	/// <summary>
	/// Returns Social Distance manual clean enabled.
	/// </summary>
	/// <returns>Social Distance manual clean enabled</returns>	
	bool GetSocialDistanceManualCleanEnabled() const;

	/// <summary>
	/// Returns the social distance URL.
	/// </summary>
	/// <returns>the request service URL.</returns>	
	std::string GetSocialDistanceUrl() const;

	/// <summary>
	/// Returns the social distance token.
	/// </summary>
	/// <returns>the request service token.</returns>	
	std::string GetSocialDistanceToken() const;

private:
	bool m_autoUploadCredits;
	bool m_cashoutToHost;
	bool m_taxWithholdingEnabled;
	float m_federalTaxWithholdingPercent;
	float m_stateTaxWithholdingPercent;
	BYTE m_PollerLoadBalanceTime_Hour;
	BYTE m_PollerLoadBalanceTime_Minute;
	bool m_pollerReconnect;
	bool m_maintenanceMode;
	bool m_combineZipLogs;
	bool m_useSDSCardFormat;
	bool m_meterRolloverSyncEnabled;
	BYTE m_meterRolloverTimeHour;
	BYTE m_meterRolloverTimeMinute;
	BYTE m_meterRolloverTimeSecond;

	DWORD m_autoUploadDelaySeconds;
	DWORD m_autoCardOutSecToCountdownCredits;
	DWORD m_autoCardOutSecToCountdownNoCrdts;
	DWORD m_autoCardOutCountdownSeconds;
	DWORD m_autoCardOutSecToAlert;
	DWORD m_numberOfSubgamesMismatchCheck;
	DWORD m_countdownDisplayIntervalInSec;
	DWORD m_rebootAsapThresholdInMb;
	DWORD m_rebootWarningThresholdMb;
	DWORD m_restartUIAsapThresholdMb;
	DWORD m_restartUIAfterDropThresholdMb;
	DWORD m_restartUIAfterxDays;
	DWORD m_restartUIDelayHours;
	DWORD m_sasReadTimeOutMultiplier;
	DWORD m_socialDistanceCountdownDelay;

	bool m_pointsCountdownControlByUniverse;	
	bool m_ticketInResend;
	bool m_enableLocationServices;
	bool m_socialDistanceIgnoreDirty;
	bool m_socialDistanceManualCleanEnabled;

	std::string m_nConnectUrl;
	std::string m_nConnectToken;
	std::string m_avatarUrl;
	std::string m_avatarToken;
	std::string m_requestServiceUrl;
	std::string m_requestServiceToken;
	std::string m_drinksOnTapUrl;
	std::string m_drinksOnTapToken;
	std::string m_socialDistanceUrl;
	std::string m_socialDistanceToken;

	std::string m_archiveLogDescription;

	std::map<std::string, std::string> m_extensibleSettings;
};


