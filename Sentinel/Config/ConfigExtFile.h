#pragma once

// Sentinel headers.
#include "Config.h"
#include "ConfigExtData.h"
#include "ConfigFile.h"
#include "InternalMsgContent.h"

// Make setters public for unit tests.
class CConfigExtFile : public CConfigFile, public CInternalMsgContent<INT_MSG_CONTENT_PROGRESSIVE_CONFIG>
{
// #pragma region common members
public:
	/// <summary>
	/// Standard/Default constructor. Initializes a new instance of the <see cref="CConfigExtFile"/> class.
	/// </summary>
	/// <param name="operatingSystem">An interface to operating system functionality.</param>
	CConfigExtFile(IOperatingSystem &operatingSystem = COperatingSystem::m_operatingSystem);


	/// <summary>
	/// Destructor. Finalizes an instance of the <see cref="CConfigExtFile"/> class.
	/// </summary>
	~CConfigExtFile();
// #pragma endregion common members



// #pragma region configuration file members
public:
	/// <summary>
	/// Starts initial config file load for this instance.
	/// </summary>
	/// <returns>true if any name/value pairs were processed from the configuration file, false otherwise.</returns>
	bool Start();

	/// <summary>
	/// Interprets the name and value.
	/// </summary>
	/// <param name="name">IN - The name.</param>
	/// <param name="value">IN - The value.</param>
	/// <returns>true if the name and value are valid, false</returns>
	bool InterpretNameValue(const std::string& name, const std::string& value);

	/// <summary>
	/// Sets up the configuration maps
	/// </summary>
	void SetDefaults();

	/// <summary>
	/// Logs the configuration values.
	/// </summary>
	/// <param name="sourceDesc">IN - Name of the configuration file.</param>
	/// <param name="valid">IN - true if the configuration file contained at least one valid name/value pair, false otherwise.</param>
	void LogConfigValues(LPCTSTR sourceDesc, bool valid);

	/// <summary>
	/// Deletes the configuration file.
	/// </summary>
	bool DeleteConfigFile();

private:

	/// <summary>
	/// Instantiates and populates the configuration maps (called by SetDefaults())
	/// </summary>
	void FillConfigMaps();

	static const LPCTSTR CONFIG_FILENAME; // The configuration filename.

// #pragma endregion configuration file members



// #pragma region configuration extension members
public:
	/// <summary>
	/// CConfig constructor. Initializes a new instance of the <see cref="CConfigExtFile"/> class.
	/// </summary>
	/// <param name="config">IN - a CConfig instance.</param>
	/// <param name="operatingSystem">An interface to operating system functionality.</param>
	CConfigExtFile(const CConfig& config, IOperatingSystem &operatingSystem = COperatingSystem::m_operatingSystem);

	/// <summary>
	/// Copy constructor. Initializes a new instance of the <see cref="CConfigExtFile"/> class.
	/// </summary>
	/// <param name="rhs">IN - The source instance.</param>
	CConfigExtFile(const CConfigExtFile &rhs);

	/// <summary>
	/// Assigment operator.
	/// </summary>
	/// <param name="rhs">IN - The source instance.</param>
	/// <returns>A reference to the target of the assignment.</returns>
	CConfigExtFile& operator=(const CConfigExtFile &rhs);

	/// <summary>
	/// Gets the negligible credits threshold for treating the credit meter as having zero credits.
	/// </summary>
	/// <param name="negligibleCreditsThreshold">OUT - the threshold value.</param>
	/// <returns>true if the value was read from a configuration file, false otherwise.</returns>
	bool GetNegligibleCreditsThreshold(WORD &negligibleCreditsThreshold) const;

	/// <summary>
	/// Returns true if employee cards time out when inserted over a (virtual) player session, false otherwise.
	/// </summary>
	/// <param name="employeeOverPlayerIdleCardOut">OUT - true to enable the time out, false to disable.</param>
	/// <returns>true if the value was read from a configuration file, false otherwise.</returns>
	bool GetEmployeeOverPlayerIdleCardOut(bool &employeeOverPlayerIdleCardOut) const;

	/// <summary>
	/// Gets the time period for employee cards to time out when inserted over a (virtual) player session.
	/// </summary>
	/// <param name="employeeIdleCardOutSeconds">OUT - The time out period.</param>
	/// <returns>true if the value was read from a configuration file, false otherwise.</returns>
	bool GetEmployeeIdleCardOutSeconds(WORD &employeeIdleCardOutSeconds) const;

	/// <summary>
	/// Gets whether to recard a player after an employee card out.
	/// </summary>
	/// <param name="playerRecardEmployeeCardOut">OUT - true to recard a player after an employee card out, false otherwise.</param>
	/// <returns>true if the value was read from a configuration file, false otherwise.</returns>
	bool GetPlayerRecardEmployeeCardOut(bool &playerRecardEmployeeCardOut) const;

	/// <summary>
	/// Sets the timeout seconds for a session active with negligible credits.
	/// </summary>
	/// <param name="sessionActiveNegligibleCreditsTimeoutSeconds">OUT - The timeout seconds for a session active with negligible credits.</param>
	/// <returns>true if the value was read from a configuration file, false otherwise.</returns>
	bool GetSessionActiveNegligibleCreditsTimeoutSeconds(WORD &sessionActiveNegligibleCreditsTimeoutSeconds) const;

	/// <summary>
	/// Gets the timeout seconds for a session active with non-negligible credits.
	/// </summary>
	/// <param name="sessionActiveNonNegligibleCreditsTimeoutSeconds">OUT - The timeout seconds for a session active with non-negligible credits.</param>
	/// <returns>true if the value was read from a configuration file, false otherwise.</returns>
	bool GetSessionActiveNonNegligibleCreditsTimeoutSeconds(WORD &sessionActiveNonNegligibleCreditsTimeoutSeconds) const;

	/// <summary>
	/// Gets the duration in seconds for showing promotion messages in the notification box.
	/// </summary>
	/// <param name="showPromotionTime">OUT - time value.</param>
	/// <returns>true if set from file, false otherwise.</returns>
	bool GetNotificationForShowPromotionTime(WORD &showPromotionTime) const;

	/// <summary>
	/// Gets the duration in seconds for showing player tracking messages in the notification box.
	/// </summary>
	/// <param name="playerTrackingTime">OUT - time value.</param>
	/// <returns>true if set from file, false otherwise.</returns>
	bool GetNotificationForPlayerTrackingTime(WORD &playerTrackingTime) const;

	/// <summary>
	/// Gets the number of buttons on custom resort.
	/// </summary>
	/// <param name="count">OUT - the count.</param>
	/// <returns>true if set from file, false otherwise.</returns>
	bool GetResortMenuButtonCount(WORD &count) const;

	/// <summary>
	/// Gets whether custom button 1 is enabled.
	/// </summary>
	/// <param name="enable">OUT - true for enabled, false otherwise.</param>
	/// <returns>true if set from file, false otherwise.</returns>
	bool GetCustomButton1Enabled(bool &enable) const;

	/// <summary>
	/// Gets whether custom button 2 is enabled.
	/// </summary>
	/// <param name="enable">OUT - true for enabled, false otherwise.</param>
	/// <returns>true if set from file, false otherwise.</returns>
	bool GetCustomButton2Enabled(bool &enable) const;

	/// <summary>
	/// Gets the number of buttons on custom menu 1.
	/// </summary>
	/// <param name="count">OUT - the count.</param>
	/// <returns>true if set from file, false otherwise.</returns>
	bool GetCustomMenuButton1Count(WORD &count) const;

	/// <summary>
	/// Gets the number of buttons on custom menu 2.
	/// </summary>
	/// <param name="count">OUT - the count.</param>
	/// <returns>true if set from file, false otherwise.</returns>
	bool GetCustomMenuButton2Count(WORD &count) const;

	/// <summary>
	/// Gets the reboot UI restart override.
	/// </summary>
	/// <param name="enable">The enable.</param>
	/// <returns>true if set from file, false otherwise.</returns>
	bool GetRebootUiRestartOverride(bool &enable) const;

	/// <summary>
	/// Gets the bonus counter Notification priority
	/// </summary>
	/// <param name="priority">OUT - Notification priority</param>
	/// <returns>true if set from file, false otherwise.</returns>
	bool GetBonusCounterNotificationPriority(WORD& priority) const;

	/// <summary>
	/// Gets the bonus counter Rich Level priority
	/// </summary>
	/// <param name="priority">OUT - Rich Level priority</param>
	/// <returns>true if set from file, false otherwise.</returns>
	bool GetBonusCounternRichLevelPriority(WORD& priority) const;

	/// <summary>
	/// Gets the bonus counter Splashdown priority
	/// </summary>
	/// <param name="priority">OUT - Splashdown priority</param>
	/// <returns>true if set from file, false otherwise.</returns>
	bool GetBonusCounterSplashdownPriority(WORD& priority) const;

	/// <summary>
	/// Gets the bonus counter Progressive Odometer priority
	/// </summary>
	/// <param name="priority">OUT - Progressive Odometer priority</param>
	/// <returns>true if set from file, false otherwise.</returns>
	bool GetBonusCounterProgressiveOdometerPriority(WORD& priority) const;

	/// <summary>
	/// Gets the bonus counter PTE priority
	/// </summary>
	/// <param name="priority">OUT - PTE priority</param>
	/// <returns>true if set from file, false otherwise.</returns>
	bool GetBonusCounterPtePriority(WORD& priority) const;

	/// <summary>
	/// Gets the bonus counter Scatter Countdown Timer priority
	/// </summary>
	/// <param name="priority">OUT - Scatter Countdown Timer priority</param>
	/// <returns>true if set from file, false otherwise.</returns>
	bool GetBonusCounterScatterCountdownTimerPriority(WORD& priority) const;

	/// <summary>
	/// Gets the bonus counter prize priority
	/// </summary>
	/// <param name="priority">OUT - Prize odometer priority</param>
	/// <returns>true if set from file, false otherwise.</returns>
	bool GetBonusCounterPrizePriority(WORD& priority) const;

	/// <summary>
	/// Gets the bonus counter playxgety priority
	/// </summary>
	/// <param name="priority">OUT - playxgety odometer priority</param>
	/// <returns>true if set from file, false otherwise.</returns>
	bool GetBonusCounterPlayXGetYPriority(WORD& priority) const;

	/// <summary>
	/// Sets the reboot UI restart override.
	/// </summary>
	/// <param name="enable">if set to <c>true</c> [enable].</param>
	void SetRebootUiRestartOverride(bool enable);

	/// <summary>
	/// Sets the Game List Chunk Size.
	/// </summary>
	/// <param name="gameListChunkSize">Game List Chunk Size.</param>
	void SetGameListChunkSize(WORD gameListChunkSize);

	/// <summary>
	/// Gets the Game List Chunk Size.
	/// </summary>
	/// <param name="gameListChunkSize">The Game List Chunk Size.</param>
	/// <returns>true if set from file, false otherwise.</returns>
	bool GetGameListChunkSize(WORD &gameListChunkSize) const;
		
private:
	CConfigExtData m_configData;

// #pragma endregion configuration extension members
};
