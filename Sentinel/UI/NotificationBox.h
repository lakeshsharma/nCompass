#pragma once
#include "InternalMessage.h"
#include "NotificationTextConfig.h"
#include "ProcessDump/OperatingSystem.h"
#include "DataSubscriptionProvider.h"
#include "Config/MediaWindowConfig.h"

// Declare friend class(es) for unit test access.
namespace SentinelUnitTests { class NotificationBoxUnitTests; }
namespace SentinelUnitTests { class PairingInformation; }

enum class MobileState
{
	PleaseWait,
	PleaseEnterCode,
	ErrorOccured,
	Connected,
	Disconnected
};

enum class NotificationState
{
	IDLE,
    UNCARDED,
    SYSTEM_BUSY,
    BAD_CARD_READ,
    VIRTUAL_PLAYER,
    INELIGIBLE_PLAYER,
    MAX_CARD_LIMIT_EXCEEDED,
    GUEST_WELCOME,
    PLAYER_WELCOME,
    PLAYER_IN_PLAY
};

/// <summary>
/// Class for updating and calculating the notification box state information.
/// Includes messages for player tracking, promotions, and announcements, based on the Events or Player action on the Slot Machine.
/// </summary>
class CNotificationBox : public CDataSubscriptionProvider
{
	// Grant "friend" access for testing.
	friend class SentinelUnitTests::NotificationBoxUnitTests;
	friend class SentinelUnitTests::PairingInformation;

public:

	/// <summary>
	/// Default constructor. Initializes a new instance of the <see cref="CNotificationBox"/> class.
	/// </summary>
	/// <param name="operatingSystem">IN/OUT - The operating environment.</param>
	CNotificationBox(IOperatingSystem &operatingSystem = COperatingSystem::m_operatingSystem);

	/// <summary>
	/// Destructor. Finalizes an instance of the <see cref="CNotificationBox"/> class.
	/// </summary>
	~CNotificationBox();

	/// <summary>
	/// Gets the subscription data response.
	/// </summary>
	/// <param name="whereId">The subscription topic.</param>
	/// <param name="whereId">The where identifier.</param>
	/// <returns>subscription data response</returns>
	virtual std::string GetSubscriptionDataResponse(const std::string& topic, const std::string& whereId);

	/// <summary>
	/// Checks for text changes.
	/// </summary>
	/// <returns>
	/// true if the display state changed, false otherwise
	/// </returns>
	/// <remarks>
	/// Call GetText() to get the text for the current state.
	/// </remarks>
	bool CheckIfNotificationTextHasChanged();

	/// <summary>
	/// Gets the text for the current state.
	/// </summary>
	/// <returns>The text for the current state.</returns>
	std::string GetText();

	/// <summary>
	/// Sets the download pending status.
	/// </summary>
	/// <param name="downloadPending">IN - The download pending status.</param>
	/// <returns>true if the display state changed, false otherwise</returns>
	bool SetDownloadPending(bool downloadPending);

	/// <summary>
	/// Sets the player state.
	/// </summary>
	/// <param name="notificationState">IN - new notificaiton state.</param>
	/// <returns>true if the display state changed, false otherwise</returns>
	/// <remarks>
	/// Not normally called with MAX_CARD_LIMIT_EXCEEDED, GUEST_WELCOME, PLAYER_WELCOME, or PLAYER_IN_PLAY.
	/// </remarks>
	bool SetNotificationState(NotificationState notificationState);

	/// <summary>
	/// Indicates whether to keep the notification box up front or not.
	/// </summary>
	/// <returns>true if notification box needs to kept up front, false otherwise</returns>
	bool KeepNotificationBoxUpFront() const;

	/// <summary>
	/// Sets the notification state by screen identifier.
	/// </summary>
	/// <param name="screenId">IN - The screen identifier.</param>
	/// <param name="isCardInserted">IN - card inserted identifier.</param>
	/// <returns>true if the display state changed, false otherwise</returns>
	/// <remarks>
	/// Call GetText() to get the text for the current state.
	/// </remarks>
	bool SetNotificationStateByScreenId(int screenId, bool isCardInserted);

	/// <summary>
	/// Returns the current notification state (screen identifier).
	/// </summary>
	/// <returns>Current screen identifier</returns>
	NotificationState GetNotificationState() const;

	/// <summary>
	/// Sets the guest player welcome state.
	/// </summary>
	/// <param name="guestWelcomeText">IN - list of text messages to display if player remains a guest.</param>
	/// <param name="advanceStateAfterGuestText">IN - true to advance to the next state after displaying the list,
	/// else continue to display last item in the list.</param>
	/// <param name="inPlayText">IN - text to display when in play state.</param>
	/// <param name="inPlayTextArgs">IN - arguments of in play state text.</param>
	/// <returns>true if the display state changed, false otherwise</returns>
	bool SetGuestWelcome(const std::vector<std::string>& guestWelcomeText, bool advanceStateAfterGuestText,
		const std::string& inPlayText, const std::vector<std::string>& inPlayTextArgs);

	/// <summary>
	/// Sets the (non-guest) player welcome state.
	/// </summary>
	/// <param name="playerWelcomeText">IN - list of text messages to display for the player.</param>
	/// <param name="playerWelcomeArgs">IN - list of arguments for text messages to display for the player.</param>
	/// <param name="inPlayText">IN - text to display when in play state.</param>
	/// <param name="inPlayTextArgs">IN - arguments of in play state text.</param>
	/// <returns>true if the display state changed, false otherwise</returns>
	bool SetPlayerWelcome(const std::vector<std::string>& playerWelcomeText, const std::vector<std::vector<std::string>>& playerWelcomeArgs,
		const std::string& inPlayText, const std::vector<std::string>& inPlayTextArgs);

	/// <summary>
	/// Sets the text for in play state.
	/// </summary>
	/// <param name="inPlayText">IN - text to display when in play state.</param>
	/// <param name="inPlayTextArgs">IN - arguments of in play state text.</param>
	/// <param name="inPlayMultipleBucketDisplay">IN - is multiple buckets to display in play state.</param>
	/// <returns>true if the display state changed, false otherwise</returns>
	bool SetInPlayText(const std::string& inPlayText, const std::vector<std::string>& inPlayTextArgs, bool inPlayMultipleBucketDisplay = false);

	/// <summary>
	/// This sets the correct text for a four-field player screen.
	/// </summary>
	/// <param name="player">IN - The player.</param>
	/// <returns>true if the display state changed, false otherwise</returns>
	bool Set4FieldPlayerText(const CPlayer *player);

	/// <summary>
	/// Sets the task bar configuration.
	/// </summary>
	/// <param name="uiConfig">IN - The new UI configuration object.</param>
	void SetUIConfig(const CUIConfig &uiConfig);

	/// <summary>
	/// Sets RFID status so bad card read can be expired.
	/// </summary>
	/// <param name="isRFID">if set to <c>true</c> [is rfid].</param>
	void SetRFID(bool isRFID);
	
	/// <summary>
	/// Sets the mobile enable flag.
	/// </summary>
	/// <param name="value">IN - mobile enable.</param>
	void SetMobileEnableFlag(bool value);
	
	/// <summary>
	/// Check if mobile enabled.
	/// </summary>
	/// <returns>true if mobile enabled, false otherwise</returns>
	bool IsMobileEnabled() const;
	
	/// <summary>
	/// Sets Mobile state & Pairing code.
	/// </summary>
	/// <param name="mobileState">IN - Mobile state.</param>
	/// <param name="pairingCode">IN - Pairing code.</param>
	void SetMobileState(MobileState mobileState, const std::string& pairingCode = "");
	
	/// <summary>
	/// Check if waiting for mobile connection.
	/// </summary>
	/// <returns>true if waiting for mobile connection, false otherwise</returns>
	bool WaitingForMobileConnection() const;
	
	/// <summary>
	/// Sets the employee active flag.
	/// </summary>
	/// <param name="value">IN - the employee active.</param>
	void SetEmployeeCardFlag(bool value);

	/// <summary>
	/// Adds the alert.
	/// </summary>
	/// <param name="alert">IN - alert to add.</param>
	/// <param name="percentage">IN - percentage value.</param>
	/// <returns>true if alert text has changed, false otherwise</returns>
	bool AddAlert(Alerts alert, WORD percentage = 0);

	/// <summary>
	/// Remove the alert state.
	/// </summary>
	/// <param name="alert">IN - alert to remove.</param>
	/// <returns>true if alert text has changed, false otherwise</returns>
	bool RemoveAlert(Alerts alert);

	/// <summary>
	/// Sets the text toggle interval in milli-seconds.
	/// </summary>
	/// <param name="interval">IN - interval in milli-seconds.</param>
	void SetTextToggleInterval(WORD intervalInMilliSecs);

private:
	std::recursive_mutex m_criticalSection;

	MediaWindowPlayerState m_bonusEventPlayerState;

	// Configuration.
	CNotificationTextConfig m_notificationTextConfig;
	CUIConfig m_uiConfig;

	// Status.
	bool m_downloadPending;
	bool m_employeeActive;
	NotificationState m_notificationState;
	std::vector<std::string> m_guestWelcomeText;
	bool m_advanceStateAfterGuestText; // If false, display last item in m_guestWelcomeText until player welcome text is received.
	std::vector<std::string> m_playerWelcomeText;
	std::vector<std::vector<std::string>> m_playerWelcomeTextArgs;
	std::string m_inPlayText;
	std::vector<std::string> m_inPlayTextArgs;
	bool m_inPlayMultipleBucketDisplay;

	// Handle automatic notification removal when RFID reader
	bool m_isRFID;
	DWORD m_notificationStartTickCount;
	NotificationState m_prevNotificationState;

	// Calculated info.
	std::string m_mobileText;
	std::string m_text;
	std::vector<std::string> m_textArgs;
	CNotificationTextConfig::TextPriority m_textPriority;
	bool m_isMultipleBucketDisplay;
	DWORD m_startTickCount; // Updated for each state change as needed.
	DWORD m_startTickCountForMobileText; // Updated for each state change as needed.
	IOperatingSystem &m_operatingSystem;

	bool m_mobileEnabled;
	MobileState m_mobileState;
	std::string m_mobilePairingCode;
	bool m_mobileLoadBalancingInProgress;
	DWORD m_mobileLoadBalancingInProgressStarted;
	bool m_mobileConnectStarted; // flag to prolong display of welcome msg in case of mobile connect

	bool IsPlayerPresent() const;
	bool CheckGuestWelcomeTimer();
	bool CheckPlayerWelcomeTimer();
	void CheckPlayerMessageState();
	bool CalcText();

	std::string ResponseForNotificationBox();
	std::string ResponseForMobileConnect() const;

	bool m_isSubscriptionDataSent;

	// Alert related stuff
	// -------------------
	WORD m_alertFlags;
	WORD m_meterSnapshotPercentage;
	std::string m_alertText;
	std::vector<std::string> m_alertTextArgs;
	CNotificationTextConfig::TextPriority m_alertPriority;
	DWORD m_rebootedAlertStartTickCount;

	bool CalcAlertText();
	bool CheckAlertTimer();

	WORD m_textToggleIntervalInMilliSecs;
};
