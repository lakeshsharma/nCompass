#pragma once

#include "../ContentDeliverySystem/ThirdPartyMessage.h"
#include "InternalMessage.h"

namespace SentinelUnitTests { class SentinelDlgStateUnitTests; }

const std::string PARAMS = "params";
const std::string VALUES = "values";
const std::string TYPE = "type";

/// State class for CSentinelDlg class
class CSentinelDlgState
{
	// Grant "friend" access for testing.
	friend class SentinelUnitTests::SentinelDlgStateUnitTests;

public:
	/// <summary>
	/// Standard constructor. Initializes a new instance of the <see cref="CSentinelDlgState"/> class.
	/// </summary>
	CSentinelDlgState();

	/// <summary>
	/// Standard Destructor.
	/// </summary>
	~CSentinelDlgState();

	/// <summary>
	/// Processes the admin message from angular UI.
	/// </summary>
	/// <param name="msg">internal msg.</param>
	/// <param name="paramsJsonValue">The parameters json value.</param>
	void ProcessAdminMessageFromAngularUI(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue);

	/// <summary>
	/// Processes the employee pin result.
	/// </summary>
	/// <param name="msg">The MSG.</param>
	/// <returns>Returns CSubscription object</returns>
	CSubscription* ProcessEmployeePinResult(CInternalMessage* msg);

	/// <summary>
	/// Creates angular UI message for internals.
	/// </summary>
	/// <param name="message">The message.</param>
	/// <returns>Internal Message</returns>
	CInternalMessage* CreateAngularUIMessageForInternals(const std::string& message);

	/// <summary>
	/// Sets the active session.
	/// </summary>
	/// <param name="value">if set to <c>true</c> [value].</param>
	void SetActiveSession(bool value);

	/// <summary>
	/// Determines whether [is active session].
	/// </summary>
	/// <returns>true if there is an active session, false otherwise.</returns>
	bool IsActiveSession() const;

	/// <summary>
	/// Sets whether an employee is carded in.
	/// </summary>
	/// <param name="employeeCardIn">IN - true if an employee is carded in, false otherwise.</param>
	void SetEmployeeCardIn(bool employeeCardIn);

	/// <summary>
	/// Gets whether an employee is carded in.
	/// </summary>
	/// <returns>true if an employee is carded in, false otherwise.</returns>
	bool GetEmployeeCardIn() const;

	/// <summary>
	/// Sets the time to close.
	/// </summary>
	/// <param name="value">if set to <c>true</c> [value].</param>
	void SetTimeToClose(bool value);

	/// <summary>
	/// Determines whether [is time to close].
	/// </summary>
	/// <returns></returns>
	bool IsTimeToClose() const;

	/// <summary>
	/// Gets the employee card event subscription object.
	/// </summary>
	/// <param name="subscriptionData">OUT - subscription data</param>
	void GetEmployeeCardEventSubscriptionData(std::string& subscriptionData);

	/// <summary>
	/// Sets the card event subscription k
	/// </summary>
	/// <param name="cardType">card type</param>
	/// <param name="showApplyRevertScreen">showApply revert screen flag</param>
	/// <param name="pinRequired">pin required flag</param>
	void SetCardEventSubscription(CardType cardType, bool showApplyRevertScreen, bool pinRequired);

	/// <summary>
	/// Sets clear screen on card removal state
	/// </summary>
	/// <param name="clearValue">Should screen be cleared on card removal</param>
	void SetClearOnCardRemove(bool clearValue);

	/// <summary>
	/// Gets clear screen on card removal state
	/// </summary>
	/// <param name="clearValue">OUT - Clear on card removal state</param>
	bool GetClearOnCardRemove();

	std::string GetCardTypeStr(CardType cardType, bool& pinRequired);

private:
	bool m_activeSession;
	bool m_employeeCardIn;
	bool m_timeToClose;

	// Keep track of indefinite attractor screens that need to be cleared upon card removal
	bool m_clearOnCardRemove;

	CSubscription* m_employeePinSubscription;
	CSubscription* m_testUrlSubscription;
	CSubscription* m_employeeCardEventSubscription;

	std::string m_archiveLogDescription;

	void ProcessPollerSettingsChangeFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue);
	void ProcessDhcpConfigurationChangeFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue);
	void ProcessSentinelIpAddressChangeFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue);
	void ProcessSubnetMaskChangeFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue);
	void ProcessDefaultGatewayChangeFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue);
	void ProcessPreferredDnsServerChangeFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue);
	void ProcessAlternateDnsServerChangeFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue);
	void ProcessEgmSettingsChangeFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue);
	void ProcessEgmChangeFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue);
	void ProcessContentWindowChangeFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue);
	void ProcessTaskbarChangeFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue);
	void ProcessPlayerStateOpenWindowChangeFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue);
	void ProcessPlayerStateCloseWindowChangeFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue);
	void ProcessMediaWindowAutoCloseChangeFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue);
	void ProcessRepairCodeFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue, bool sendMaintenance);
	void ProcessAttendantPaidPayoutsFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue);
	void ProcessHopperFillAmountFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue);
	void ProcessVideoInputFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue);
	void ProcessVideoOutputFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue);
	void ProcessVideoOffsetsFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue);
	void ProcessVideoKeepRevertFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue);
	void ProcessMaintainEGMAspectRatioFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue);
	void ProcessA8ResetToCreditChangeFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue);
	void ProcessSetVolumeFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue);
	void ProcessSetBacklightFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue);
	void ProcessEmployeePinFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue);
	void ProcessSetRotationFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue);
	void ProcessMemoryClearFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue);
	void ProcessTestUrlFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue);
	void ProcessSetTouchProtocolFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue);
	void ProcessArhiveLogsFromTheUi(CInternalMessage *msg, const rapidjson::Value& paramsJsonValue);
	void ProcessStartPBTSessionFromAngularUI(CInternalMessage *msg, const rapidjson::Value& paramsJsonValue);
	void ProcessStartPBTTransferFromAngularUI(CInternalMessage *msg, const rapidjson::Value& paramsJsonValue);
	void ProcessResetPBTPinFromAngularUI(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue);
};