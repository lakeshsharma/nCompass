#include "stdafx.h"
#include "SentinelDlgState.h"
#include "Meters.h"
#include "GlobalDefs.h"
#include "UI/ScreenDefs.h"
#include "RapidJsonHelper.h"
#include "Logging/LogString.h"
#include "DisplayManager.h"
#include "HttpGet.h"
#include "Logging/Logger.h"
#include "Hardware/CardReaderLayer.h"
#include "PersonalBanker.h"

using namespace std;

const string METHOD_NAME_SUBSCRIBE = "subscribe";
const string METHOD_NAME_UNSUBSCRIBE = "unsubscribe";
const string METHOD_NAME_ADMIN= "admin";
const string METHOD_NAME_LOG = "log";
const string METHOD_NAME_SHOW_LEADERBOARD = "showLeaderboard";
const string METHOD_NAME_PERSONAL_BANKER = "PersonalBanker";
const string METHOD = "method";
const string DATA = "data";
const string METHOD_NAME_SESSIONFLUSH = "sessionFlush";
const string PARAM_PLAYER_ID = "playerID";
const string PARAM_JACKPOT_ID = "JackpotId";
const string PARAM_IS_WINNER = "IsWinner";
const string METHOD_MOBILE_CONNECT = "MobileConnect";
const string METHOD_PROMO_GAME_COMPLETE = "PromoGameComplete";
const string METHOD_SIDE_GAME_COMPLETE = "SideGameComplete";
const string METHOD_NAME_MACHINE_STATE = "MachineState";
const string METHOD_NAME_START_PB_STARVATION = "StartPBStarvation";
const string METHOD_NAME_CLEAR_DISPLAY_STARVATION = "ClearDisplayStarvation";
const string METHOD_NAME_PROCESS_UTJ = "ProcessUTJ";
const string METHOD_NAME_UJP_CALL_HOST = "UJPCallHost";
const string METHOD_NAME_UJP_TIMED_OUT = "UJPTimedOut";
const string METHOD_NAME_AUTOCARDOUTTIMEDOUT("AutoCardoutTimedOut");
const string METHOD_NAME_AUTOCARDOUTENDSESSION("AutoCardoutEndSession");
const string METHOD_NAME_AUTOCARDOUTCONTINUESESSION("AutoCardoutContinueSession");
const string METHOD_NAME_STARTPBTSESSION("StartPBTSession");
const string METHOD_NAME_ENDPBTSESSION("EndPBTSession");
const string METHOD_NAME_STARTPBTTRANSFER("StartPBTTransfer");
const string METHOD_NAME_RESETPIN("ResetPbtPin");
const string METHOD_EXTENDED_BUCKETS_EXIT = "ExitExtendedBuckets";
const string METHOD_EXTENDED_BUCKETS_ERROR = "InvalidExtendedBucketsData";
const string METHOD_NAME_UI_ACTIVITY_OCCURRED("UiActivityOccurred");
const string METHOD_NAME_SEND_TILT("SendTilt");
const string METHOD_NAME_REQUEST_PBT_PROBE("RequestPbtProbe");


CSentinelDlgState::CSentinelDlgState() :
m_activeSession(false),
m_employeeCardIn(false),
m_timeToClose(false),
m_clearOnCardRemove(false),
m_employeePinSubscription(nullptr),
m_testUrlSubscription(nullptr),
m_employeeCardEventSubscription(nullptr)
{
}

CSentinelDlgState::~CSentinelDlgState()
{
	if (m_employeePinSubscription)
	{
		delete m_employeePinSubscription;
		m_employeePinSubscription = nullptr;
	}

	if (m_testUrlSubscription)
	{
		delete m_testUrlSubscription;
		m_testUrlSubscription = nullptr;
	}

	if (m_employeeCardEventSubscription)
	{
		delete m_employeeCardEventSubscription;
		m_employeeCardEventSubscription = nullptr;
	}
}

void CSentinelDlgState::ProcessAdminMessageFromAngularUI(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue)
{
	const string type = JsonGetStringEx(paramsJsonValue, TYPE.c_str());
	
	if ("PollerSettings" == type)
	{
		ProcessPollerSettingsChangeFromTheUi(msg, paramsJsonValue);
	}
	else if ("DhcpConfiguration" == type)
	{
		ProcessDhcpConfigurationChangeFromTheUi(msg, paramsJsonValue);
	}
	else if ("SentinelIpAddress" == type)
	{
		ProcessSentinelIpAddressChangeFromTheUi(msg, paramsJsonValue);
	}
	else if ("SubnetMask" == type)
	{
		ProcessSubnetMaskChangeFromTheUi(msg, paramsJsonValue);
	}
	else if ("DefaultGateway" == type)
	{
		ProcessDefaultGatewayChangeFromTheUi(msg, paramsJsonValue);
	}
	else if ("PreferredDnsServer" == type)
	{
		ProcessPreferredDnsServerChangeFromTheUi(msg, paramsJsonValue);
	}
	else if ("AlternateDnsServer" == type)
	{
		ProcessAlternateDnsServerChangeFromTheUi(msg, paramsJsonValue);
	}
	else if ("EgmSettings" == type)
	{
		ProcessEgmSettingsChangeFromTheUi(msg, paramsJsonValue);
	}
	else if ("EgmChange" == type)
	{
		ProcessEgmChangeFromTheUi(msg, paramsJsonValue);
	}
		else if ("SaveContentWindow" == type)
	{
		ProcessContentWindowChangeFromTheUi(msg, paramsJsonValue);
	}
	else if ("SaveTaskbar" == type)
	{
		ProcessTaskbarChangeFromTheUi(msg, paramsJsonValue);
	}
	else if ("SavePlayerStateOpenWindow" == type)
	{
		ProcessPlayerStateOpenWindowChangeFromTheUi(msg, paramsJsonValue);
	}
	else if ("SavePlayerStateCloseWindow" == type)
	{
		ProcessPlayerStateCloseWindowChangeFromTheUi(msg, paramsJsonValue);
	}
	else if ("SaveAutoClose" == type)
	{
		ProcessMediaWindowAutoCloseChangeFromTheUi(msg, paramsJsonValue);
	}
	else if ("PerformBatteryTest" == type)
	{
		msg->SetMessageType(INT_MSG_PERFORM_BATTERY_TEST);
	}
	else if ("SendA8ResetToCredit" == type)
	{
		ProcessA8ResetToCreditChangeFromTheUi(msg, paramsJsonValue);
	}
	else if ("MakeEgmFullScreen" == type)
	{
		CDisplayManager::Instance().MakeDisplayFullEGM();
	}
	else if ("SendMaintenanceCodes" == type || "SendFailureRepairCodes" == type)
	{
		ProcessRepairCodeFromTheUi(msg, paramsJsonValue, "SendMaintenanceCodes" == type);
	}
	else if ("SendPartialClean" == type)
	{
		msg->SetMessageType(INT_MSG_SEND_MAINTENANCE);
		msg->SetData(PARTIAL_CLEAN_CODE);
	}
	else if ("SendFullClean" == type)
	{
		msg->SetMessageType(INT_MSG_SEND_MAINTENANCE);
		msg->SetData(FULL_CLEAN_CODE);
	}
	else if ("SendPrinterMaintenance" == type)
	{
		msg->SetMessageType(INT_MSG_SEND_PRINTER_MAINTENANCE);
	}
	else if ("SendWitness" == type)
	{
		msg->SetMessageType(INT_MSG_SEND_WITNESS);
	}
	else if ("SendCancelPendingPBTTransaction" == type)
	{
		LogString(PBT_LOG, "SendCancelPendingPBTTransaction");
		msg->SetMessageType(INT_MSG_PB_CANCEL_SESSION);
		msg->SetData(/*displayCancelSession*/WORD(0));
	}
	else if ("SendForceCardOut" == type)
	{
		msg->SetMessageType(INT_MSG_FORCE_CARD_OUT);
	}
	else if ("SendAttendantPaidPayouts" == type)
	{
		ProcessAttendantPaidPayoutsFromTheUi(msg, paramsJsonValue);
	}
	else if ("SendHopperFill" == type)
	{
		msg->SetMessageType(INT_MSG_SEND_HOPPER_FILL);
	}
	else if ("SendHopperFillWithAmount" == type)
	{
		ProcessHopperFillAmountFromTheUi(msg, paramsJsonValue);
	}
	else if ("CaptureMaintenanceBeginningMeters" == type)
	{
		msg->SetMessageType(INT_MSG_START_CAPTURING_METERS);
		msg->SetData((WORD)MaintenancePlayBegin);
	}
	else if ("CaptureMaintenanceEndMeters" == type)
	{
		msg->SetMessageType(INT_MSG_START_CAPTURING_METERS);
		msg->SetData((WORD)MaintenancePlayEnd);
	}
	else if ("CaptureEGMRAMMeters" == type)
	{
		msg->SetMessageType(INT_MSG_START_CAPTURING_METERS);
		msg->SetData((WORD)EGMRAMClear);
	}
	else if ("CaptureInitialMeters" == type)
	{
		msg->SetMessageType(INT_MSG_START_CAPTURING_METERS);
		msg->SetData((WORD)InitialMeters);
	}
	else if ("SaveVideoInputMode" == type)
	{
		ProcessVideoInputFromTheUi(msg, paramsJsonValue);
	}
	else if ("SaveVideoOutputMode" == type)
	{
		ProcessVideoOutputFromTheUi(msg, paramsJsonValue);
	}
	else if ("SaveVideoOffsets" == type)
	{
		ProcessVideoOffsetsFromTheUi(msg, paramsJsonValue);
	}
	else if ("AcceptOrRevertVideoMode" == type)
	{
		ProcessVideoKeepRevertFromTheUi(msg, paramsJsonValue);
	}
	else if ("GoodbyeEmployee" == type)
	{
		msg->SetMessageType(INT_MSG_EMPLOYEE_GOODBYE);
	}
	else if ("SetVolume" == type)
	{
		ProcessSetVolumeFromTheUi(msg, paramsJsonValue);
	}
	else if ("SetBacklight" == type)
	{
		ProcessSetBacklightFromTheUi(msg, paramsJsonValue);
	}
	else if ("Reboot" == type)
	{
		msg->SetMessageType(INT_MSG_SHUTDOWN);
	}
	else if ("MaintainEGMAspectRatio" == type)
	{
		ProcessMaintainEGMAspectRatioFromTheUi(msg, paramsJsonValue);
	}
	else if ("CalibrateScreen" == type)
	{
		msg->SetMessageType(INT_MSG_CALIBRATE_SENTINEL_SCREEN);
		if (m_employeeCardEventSubscription)
		{
			delete m_employeeCardEventSubscription;
			m_employeeCardEventSubscription = nullptr;
		}
	}
	else if ("UpgradeMixer" == type)
	{
		msg->SetMessageType(INT_MSG_UPGRADE_MIXER);
	}
	else if ("UpgradeCarrier" == type)
	{
		msg->SetMessageType(INT_MSG_UPGRADE_CARRIER);
	}
	else if ("EmployeePin" == type)
	{
		ProcessEmployeePinFromTheUi(msg, paramsJsonValue);
	}
    else if ("SetRotation" == type)
    {
		ProcessSetRotationFromTheUi(msg, paramsJsonValue);
    }
	else if ("MemoryClear" == type)
	{
		ProcessMemoryClearFromTheUi(msg, paramsJsonValue);
	}
	else if ("TestUrl" == type)
	{
		ProcessTestUrlFromTheUi(msg, paramsJsonValue);
	}
    else if ("SetTouchscreenProtocol" == type) 
    {
		ProcessSetTouchProtocolFromTheUi(msg, paramsJsonValue);
    } 
	else if ("ArchiveLogs" == type)
	{
		ProcessArhiveLogsFromTheUi(msg, paramsJsonValue);
	}
	else if ("SendPrizeJackpotClear" == type)
	{
		msg->SetMessageType(INT_MSG_PRIZE_LOCK_CLEAR);
	}
	else
	{
		LogString(ERROR_LOG, "Unrecognized admin message from angular UI: %s", type.c_str());
		LogString(ANGULARUILOG, "Unrecognized admin message from angular UI: %s", type.c_str());
	}
}

CSubscription* CSentinelDlgState::ProcessEmployeePinResult(CInternalMessage* msg)
{
	if (msg && nullptr != m_employeePinSubscription)
	{
		bool successful(msg->GetWordData());
		string jsonData("{\"Successful\":");
		jsonData += successful ? "true}" : "false}";
		m_employeePinSubscription->SetSubscriptionData(m_employeePinSubscription->GetTopic(), m_employeePinSubscription->GetWhereId(), jsonData);
	}

	return m_employeePinSubscription;
}

CInternalMessage* CSentinelDlgState::CreateAngularUIMessageForInternals(const string& message)
{
	CInternalMessage* msg(nullptr);
	rapidjson::Document jsonDoc;
	try
	{
		if (jsonDoc.Parse(message.c_str()).HasParseError())
		{
			string logMessage(FormatString("[UI MESSAGE PARSE ERROR] Message: %s, Error: %s",
				message.c_str(), JsonException::GetParseErrorString(jsonDoc.GetParseError())));
			LogStringNoFormat(ERROR_LOG, logMessage.c_str());
		}
		else
		{
			LogString(ANGULARUILOG, "Received: %s", message.c_str());

			string method = JsonGetStringEx(jsonDoc, METHOD.c_str());
			if (method == METHOD_NAME_LOG)
			{
				if (message.find("[UI ERROR]") != string::npos)
				{
					// Log important Angular UI errors in error log also
					const rapidjson::Value& paramsJsonValue = JsonGetObjectEx(jsonDoc, PARAMS.c_str());
					if (JsonMemberExists(paramsJsonValue, DATA.c_str()))
					{
						string data = JsonGetStringEx(paramsJsonValue, DATA.c_str());
						LogStringNoFormat(ERROR_LOG, data.c_str());
					}
				}
			}
			else if (method == METHOD_NAME_SUBSCRIBE)
			{
				CSubscription* subscription(new CSubscription(jsonDoc));
				if (subscription->GetTopic() == "EmployeePinValidation")
				{
					if (m_employeePinSubscription)
					{
						delete m_employeePinSubscription;
					}
					m_employeePinSubscription = subscription;
				}
				else if (subscription->GetTopic() == "TestUrl")
				{
					if (m_testUrlSubscription)
					{
						delete m_testUrlSubscription;
						m_testUrlSubscription = nullptr;
					}
					m_testUrlSubscription = subscription;
				}
				else if (subscription->GetTopic() == "EmployeeCardEvent")
				{
					if (m_employeeCardEventSubscription)
					{
						delete m_employeeCardEventSubscription;
						m_employeeCardEventSubscription = nullptr;
					}
					m_employeeCardEventSubscription = subscription;
					msg = new CInternalMessage(INT_MSG_DISPLAY_EMPLOYEE_SCREEN);
				}
				else
				{
					msg = new CInternalMessage(INT_MSG_SUBSCRIBE_REQUEST, subscription);
					if (ArchiveLogsProgress == subscription->GetTopic() && !m_archiveLogDescription.empty())
					{
						msg->SetData(m_archiveLogDescription);
						m_archiveLogDescription.clear();
					}
				}
			}
			else if (method == METHOD_NAME_UNSUBSCRIBE)
			{
				CSubscription* subscription(new CSubscription(jsonDoc));
				msg = new CInternalMessage(INT_MSG_UNSUBSCRIBE_REQUEST, subscription);
			}
			else if (method == METHOD_NAME_MACHINE_STATE)
			{
				const rapidjson::Value& paramsJsonValue = JsonGetObjectEx(jsonDoc, PARAMS.c_str());
				if (JsonMemberExists(paramsJsonValue, "values"))
				{
					const rapidjson::Value& values = JsonGetObjectEx(paramsJsonValue, "values");
					std::string machineState = JsonGetStringEx(values, "state");
					msg = new CInternalMessage(INT_MSG_SET_CARD_BEZEL, (WORD)MachineStateEnumMap.At(machineState.c_str()));
				}
				else
				{
					LogString(ERROR_LOG, "[Social Distance] Machine State method did not include values");
				}
			}
			else if (method == METHOD_NAME_PROCESS_UTJ)
			{
				const rapidjson::Value& paramsJsonValue = JsonGetObjectEx(jsonDoc, PARAMS.c_str());
				if (JsonMemberExists(paramsJsonValue, "values"))
				{
					const rapidjson::Value& values = JsonGetObjectEx(paramsJsonValue, "values");
					std::string pin = JsonGetStringEx(values, "Pin");
					DWORD federalTaxWithheld = JsonGetUintEx(values, "FederalTaxWithheld");
					DWORD stateTaxWithheld = JsonGetUintEx(values, "StateTaxWithheld");
					bool federalTaxOverridden = JsonGetBoolEx(values, "FederalTaxOverridden");
					bool stateTaxOverridden = JsonGetBoolEx(values, "StateTaxOverridden");

					LogString(JACKPOT, "ProcessUTJ Pin=%s federalTaxWithheld=%u stateTaxWithheld=%u federalTaxOverridden=%u stateTaxOverridden=%u",
						pin.c_str(), federalTaxWithheld, stateTaxWithheld, federalTaxOverridden, stateTaxOverridden);
					msg = new CInternalMessage(INT_MSG_UJP_CHECK_PIN, pin);
					msg->SetData(federalTaxWithheld, stateTaxWithheld);
					msg->SetData((WORD)federalTaxOverridden, (WORD)stateTaxOverridden);
				}
				else
				{
					LogString(ERROR_LOG, "ProcessUTJ method did not include values");
				}
			}			
			else if (method == METHOD_NAME_START_PB_STARVATION)
			{
				msg = new CInternalMessage(INT_MSG_START_PB_STARVATION);
			}
			else if (method == METHOD_NAME_CLEAR_DISPLAY_STARVATION)
			{
				msg = new CInternalMessage(INT_MSG_CLEAR_DISPLAY_STARVATION);
			}
			else if (method == METHOD_NAME_UJP_CALL_HOST)
			{
				msg = new CInternalMessage(INT_MSG_UJP_CALLHOST);
			}
			else if (method == METHOD_NAME_UJP_TIMED_OUT)
			{
				msg = new CInternalMessage(INT_MSG_UJP_TIMED_OUT);
			}
			else if (method == METHOD_MOBILE_CONNECT)
			{
				msg = new CInternalMessage(INT_MSG_MOBILE_CONNECT_PRESS);
			}
			else if (method == METHOD_PROMO_GAME_COMPLETE || method == METHOD_SIDE_GAME_COMPLETE)
			{
				bool validJson(false);
				if (jsonDoc.HasMember(PARAMS) && 
					jsonDoc[PARAMS].IsObject() && 
					jsonDoc[PARAMS].HasMember(VALUES) && 
					jsonDoc[PARAMS][VALUES].IsObject())
				{
					const rapidjson::Value& values = jsonDoc[PARAMS][VALUES];
					int jackpotId = values[PARAM_JACKPOT_ID].GetInt();
					bool isWinner = values[PARAM_IS_WINNER].GetBool();
					msg = new CInternalMessage(INT_MSG_PROMOGAME_COMPLETE, (new CPromoGameAcceptanceStatus(jackpotId)));
					msg->SetData((WORD)isWinner);
					validJson = true;
				}
				if (!validJson)
				{
					LogString(ERROR_LOG, "PromoGameComplete message is not formmatted correctly");
				}
			}
			else if (method == METHOD_NAME_ADMIN)
			{
				msg = new CInternalMessage(INT_MSG_ADMIN_MESSAGE, message);
			}
			else if (method == METHOD_NAME_SESSIONFLUSH)
			{
				const rapidjson::Value& paramsJsonValue = JsonGetObjectEx(jsonDoc, PARAMS.c_str());
				if (JsonMemberExists(paramsJsonValue, PARAM_PLAYER_ID.c_str()))
				{
					int playerId = JsonGetIntEx(paramsJsonValue, PARAM_PLAYER_ID.c_str());
					msg = new CInternalMessage(INT_MSG_FLUSH_PLAYER_UPDATE, playerId);
				}
				else
				{
					string logMessage("Session Flush method did not include playerID");
					LogStringNoFormat(ERROR_LOG, logMessage.c_str());
				}
			}
			else if (method == METHOD_NAME_AUTOCARDOUTTIMEDOUT)
			{
				msg = new CInternalMessage(INT_MSG_AUTO_CARDOUT_TIMED_OUT);
			}
			else if (method == METHOD_NAME_AUTOCARDOUTENDSESSION)
			{
				msg = new CInternalMessage(INT_MSG_AUTO_CARDOUT_END_SESSION);
			}
			else if (method == METHOD_NAME_AUTOCARDOUTCONTINUESESSION)
			{
				msg = new CInternalMessage(INT_MSG_AUTO_CARDOUT_CONTINUE_SESSION);
			}
			else if (method == METHOD_NAME_STARTPBTSESSION)
			{
				const rapidjson::Value& paramsJsonValue = JsonGetObjectEx(jsonDoc, PARAMS.c_str());
				msg = new CInternalMessage();
				ProcessStartPBTSessionFromAngularUI(msg, paramsJsonValue);
			}
			else if (method == METHOD_NAME_ENDPBTSESSION)
			{
				msg = new CInternalMessage(INT_MSG_PB_CANCEL_SESSION);
			}
			else if (method == METHOD_NAME_RESETPIN)
			{
				const rapidjson::Value& paramsJsonValue = JsonGetObjectEx(jsonDoc, PARAMS.c_str());
				msg = new CInternalMessage();
				ProcessResetPBTPinFromAngularUI(msg, paramsJsonValue);
			}
			else if (method == METHOD_NAME_STARTPBTTRANSFER)
			{
				const rapidjson::Value& paramsJsonValue = JsonGetObjectEx(jsonDoc, PARAMS.c_str());
				msg = new CInternalMessage();
				ProcessStartPBTTransferFromAngularUI(msg, paramsJsonValue);
			}
			else if (method == METHOD_EXTENDED_BUCKETS_EXIT)
			{
				msg = new CInternalMessage(INT_MSG_EXTENDED_BUCKETS_EXIT);
			}
			else if (method == METHOD_EXTENDED_BUCKETS_ERROR)
			{
				LogString(ERROR_LOG, "Invalid extended buckets data.");
			}
			else if (method == METHOD_NAME_UI_ACTIVITY_OCCURRED)
			{
				msg = new CInternalMessage(INT_MSG_UI_ACTIVITY_OCCURRED);
			}
			else if (method == METHOD_NAME_SEND_TILT)
			{
				const rapidjson::Value& paramsJsonValue = JsonGetObjectEx(jsonDoc, PARAMS.c_str());
				if (JsonMemberExists(paramsJsonValue, "values"))
				{
					const rapidjson::Value& values = JsonGetObjectEx(paramsJsonValue, "values");
					int tiltCode = JsonGetInt(values, "TiltCode");
					msg = new CInternalMessage(INT_MSG_SEND_TILT, (WORD)tiltCode);
				}
			}
			else if (method == METHOD_NAME_REQUEST_PBT_PROBE)
			{
				const rapidjson::Value& paramsJsonValue = JsonGetObjectEx(jsonDoc, PARAMS.c_str());
				if (JsonMemberExists(paramsJsonValue, "values"))
				{
					const rapidjson::Value& values = JsonGetObjectEx(paramsJsonValue, "values");
					bool bTransferTo = JsonGetBool(values, "IsTransferTo");
					msg = new CInternalMessage(INT_MSG_PROBE_PBT, (WORD)bTransferTo);
				}
			}
			else
			{
				string logMessage(FormatString("CSentinelDlgState::CreateAngularUIMessageForInternals - Invalid method: %s", method.c_str()));
				LogStringNoFormat(ERROR_LOG, logMessage.c_str());
			}
		}
	}
	catch (JsonException& e)
	{
		string logMessage(FormatString("[CSentinelDlgState::CreateAngularUIMessageForInternals] JSON exception: %s: '%s'", e.codestring(), e.what()));
		LogStringNoFormat(ERROR_LOG, logMessage.c_str());
	}
	catch (std::out_of_range& e)
	{
		string logMessage(FormatString("[CSentinelDlgState::CreateAngularUIMessageForInternals] out_of_range exception: '%s'", e.what()));
		LogStringNoFormat(ERROR_LOG, logMessage.c_str());
	}
	catch (std::invalid_argument& e)
	{
		string logMessage(FormatString("[CSentinelDlgState::CreateAngularUIMessageForInternals] invalid_argument exception: '%s'", e.what()));
		LogStringNoFormat(ERROR_LOG, logMessage.c_str());
	}
	catch (...)
	{
		LogString(ERROR_LOG, "[CSentinelDlgState::CreateAngularUIMessageForInternals] unknown exception.");
	}

	return msg;
}

void CSentinelDlgState::SetActiveSession(bool value)
{
	m_activeSession = value;
}

bool CSentinelDlgState::IsActiveSession() const
{
	return m_activeSession;
}

void CSentinelDlgState::SetEmployeeCardIn(bool employeeCardIn)
{
	m_employeeCardIn = employeeCardIn;

	if (!m_employeeCardIn)
	{
		SetCardEventSubscription(CARD_UNKNOWN, false, false);
	}
}

bool CSentinelDlgState::GetEmployeeCardIn() const
{
	return m_employeeCardIn;
}

/// <summary>
/// Sets the time to close.
/// </summary>
/// <param name="value">if set to <c>true</c> [value].</param>
void CSentinelDlgState::SetTimeToClose(bool value)
{
	m_timeToClose = value;
}

/// <summary>
/// Determines whether [is time to close].
/// </summary>
/// <returns></returns>
bool CSentinelDlgState::IsTimeToClose() const
{
	return m_timeToClose;
}

void CSentinelDlgState::ProcessPollerSettingsChangeFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue)
{
	if (nullptr != msg)
	{
		if (JsonMemberExists(paramsJsonValue, "values"))
		{
			const rapidjson::Value& values = JsonGetObjectEx(paramsJsonValue, "values");
			msg->SetData(JsonGetStringEx(values, "Host"), JsonGetStringEx(values, "Port"));
			msg->SetMessageType(INT_MSG_SET_POLLER_SETTINGS);
		}
	}
}

void CSentinelDlgState::ProcessDhcpConfigurationChangeFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue)
{
	if (nullptr != msg)
	{
		if (JsonMemberExists(paramsJsonValue, "values"))
		{
			const rapidjson::Value& values = JsonGetObjectEx(paramsJsonValue, "values");
			msg->SetData((WORD)JsonGetBoolEx(values, "DHCP"));
			msg->SetMessageType(INT_MSG_SET_DHCP_SETTING);
		}
	}
}

void CSentinelDlgState::ProcessSentinelIpAddressChangeFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue)
{
	if (nullptr != msg)
	{
		if (JsonMemberExists(paramsJsonValue, "values"))
		{
			const rapidjson::Value& values = JsonGetObjectEx(paramsJsonValue, "values");
			msg->SetData(JsonGetStringEx(values, "IPAddress"));
			msg->SetMessageType(INT_MSG_SET_IP_ADDRESS);
		}
	}
}

void CSentinelDlgState::ProcessSubnetMaskChangeFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue)
{
	if (nullptr != msg)
	{
		if (JsonMemberExists(paramsJsonValue, "values"))
		{
			const rapidjson::Value& values = JsonGetObjectEx(paramsJsonValue, "values");
			msg->SetData(JsonGetStringEx(values, "SubnetMask"));
			msg->SetMessageType(INT_MSG_SET_SUBNET_MASK);
		}
	}
}

void CSentinelDlgState::ProcessDefaultGatewayChangeFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue)
{
	if (nullptr != msg)
	{
		if (JsonMemberExists(paramsJsonValue, "values"))
		{
			const rapidjson::Value& values = JsonGetObjectEx(paramsJsonValue, "values");
			msg->SetData(JsonGetStringEx(values, "DefaultGateway"));
			msg->SetMessageType(INT_MSG_SET_DEFAULT_GATEWAY);
		}
	}
}

void CSentinelDlgState::ProcessPreferredDnsServerChangeFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue)
{
	if (nullptr != msg)
	{
		if (JsonMemberExists(paramsJsonValue, "values"))
		{
			const rapidjson::Value& values = JsonGetObjectEx(paramsJsonValue, "values");
			msg->SetData(JsonGetStringEx(values, "PreferredDnsServer"));
			msg->SetMessageType(INT_MSG_SET_PREFERRED_DNS);
		}
	}
}

void CSentinelDlgState::ProcessAlternateDnsServerChangeFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue)
{
	if (nullptr != msg)
	{
		if (JsonMemberExists(paramsJsonValue, "values"))
		{
			const rapidjson::Value& values = JsonGetObjectEx(paramsJsonValue, "values");
			msg->SetData(JsonGetStringEx(values, "AlternateDnsServer"));
			msg->SetMessageType(INT_MSG_SET_ALTERNATE_DNS);
		}
	}
}

void CSentinelDlgState::ProcessEgmSettingsChangeFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue)
{
	if (nullptr != msg)
	{
		if (JsonMemberExists(paramsJsonValue, "values"))
		{
			const rapidjson::Value& values = JsonGetObjectEx(paramsJsonValue, "values");
			const int type = JsonGetIntEx(values, "Type");
			const int protocol = JsonGetIntEx(values, "Protocol");
			const int pollingSpeed = JsonGetIntEx(values, "PollingSpeed");
			CEGMConfig* egmConfig(new CEGMConfig(MEMORY_NONE));
			EGMType egmType((EGMType)type);
			switch (egmType)
			{
			case CUSTOM:
			case STANDARD:
			case WMS:
			case BALLY:
			case IGTS2000:
				egmConfig->SetEGMType(egmType);
				break;
			default:
				egmConfig->SetEGMType(NO_EGM);
				break;
			}

			EGMProtocol egmProtocol((EGMProtocol)protocol);
			switch (egmProtocol)
			{
			case IGT_SAS_3xx:
			case SAS5xx:
			case SAS6xx:
				egmConfig->SetEGMProtocol(egmProtocol);
				break;
			default:
				egmConfig->SetEGMProtocol(NO_EGM_PROTOCOL);
				break;
			}

			egmConfig->SetEGMPollingSpeed((EGMPollingSpeed)pollingSpeed);
			msg->SetData(egmConfig);
			msg->SetMessageType(INT_MSG_SET_EGM_SETTINGS);
		}
	}
}

void CSentinelDlgState::ProcessEgmChangeFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue)
{
	if (nullptr != msg)
	{
		if (JsonMemberExists(paramsJsonValue, "values"))
		{
			const rapidjson::Value& values = JsonGetObjectEx(paramsJsonValue, "values");
			const string machineAction = JsonGetStringEx(values, "Machine");
			if ("Online" == machineAction)
			{
				const DWORD slotMastId = JsonGetUint64Ex(values, "SlotMastId");
				msg->SetData(slotMastId);
				msg->SetMessageType(INT_MSG_MACHINE_ONLINE);
			}
			else if ("Offline" == machineAction)
			{
				msg->SetData((WORD)FinalMeters);
				msg->SetMessageType(INT_MSG_MACHINE_OFFLINE);
			}
			else if ("BeginMove" == machineAction)
			{
				msg->SetData((WORD)MachineMoveBegin);
				msg->SetMessageType(INT_MSG_MACHINE_MOVE_BEGIN);
			}
			else if ("MoveComplete" == machineAction)
			{
				msg->SetData((WORD)MachineMoveEnd);
				msg->SetMessageType(INT_MSG_MACHINE_MOVE_COMPLETE);
			}
		}
	}
}

void CSentinelDlgState::ProcessContentWindowChangeFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue)
{
	if (nullptr != msg)
	{
		if (JsonMemberExists(paramsJsonValue, "values"))
		{
			const rapidjson::Value& values = JsonGetObjectEx(paramsJsonValue, "values");
			msg->SetData((WORD)JsonGetIntEx(values, "Location"), (WORD)JsonGetIntEx(values, "ContentWindowSize"));
			msg->SetMessageType(INT_MSG_SET_SERVICE_WINDOW);
		}
	}
}

void CSentinelDlgState::ProcessTaskbarChangeFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue)
{
	if (nullptr != msg)
	{
		if (JsonMemberExists(paramsJsonValue, "values"))
		{
			const rapidjson::Value& values = JsonGetObjectEx(paramsJsonValue, "values");
			msg->SetData((WORD)JsonGetIntEx(values, "TaskbarSize"));
			msg->SetMessageType(INT_MSG_SET_TASKBAR);
		}
	}
}

void CSentinelDlgState::ProcessPlayerStateOpenWindowChangeFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue)
{
	if (nullptr != msg)
	{
		if (JsonMemberExists(paramsJsonValue, "values"))
		{
			const rapidjson::Value& values = JsonGetObjectEx(paramsJsonValue, "values");
			msg->SetData(JsonGetStringEx(values, "CardInOpenWindowOrMaintainState"), 
				JsonGetStringEx(values, "BonusEventOpenWindowOrMaintainState"));
			msg->SetMessageType(INT_MSG_UPDATE_PLAYER_STATE_OPEN_SETTING);
		}
	}
}

void CSentinelDlgState::ProcessPlayerStateCloseWindowChangeFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue)
{
	if (nullptr != msg)
	{
		if (JsonMemberExists(paramsJsonValue, "values"))
		{
			const rapidjson::Value& values = JsonGetObjectEx(paramsJsonValue, "values");
			msg->SetData(JsonGetStringEx(values, "CardOutCloseWindowOrMaintainState"), 
				JsonGetStringEx(values, "FirstPlayCloseWindowOrMaintainState"));
			msg->SetMessageType(INT_MSG_UPDATE_PLAYER_STATE_CLOSE_SETTING);
		}
	}
}

void CSentinelDlgState::ProcessMediaWindowAutoCloseChangeFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue)
{
	if (nullptr != msg)
	{
		if (JsonMemberExists(paramsJsonValue, "values"))
		{
			const rapidjson::Value& values = JsonGetObjectEx(paramsJsonValue, "values");
			const string autoCloseOrMaintain = JsonGetStringEx(values, "MediaWindowAutoCloseOrMaintain");
			const WORD mediaWindowAutoCloseMinutes = JsonGetIntEx(values, "MediaWindowAutoCloseMinutes");
			const WORD radioValue = autoCloseOrMaintain == "AutoClose" ? CMediaWindowConfig::AUTOCLOSE_MASK : CMediaWindowConfig::AUTO_NONE_MASK;
			msg->SetData(radioValue, mediaWindowAutoCloseMinutes);
			msg->SetMessageType(INT_MSG_UPDATE_AUTO_CLOSE_SETTING);
		}
	}
}

void CSentinelDlgState::ProcessRepairCodeFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue, bool sendMaintenance)
{
	if (nullptr != msg)
	{
		if (JsonMemberExists(paramsJsonValue, "values"))
		{
			const rapidjson::Value& values = JsonGetObjectEx(paramsJsonValue, "values");
			msg->SetData((DWORD)JsonGetUintEx(values, "RepairCode"));
			msg->SetMessageType(sendMaintenance ? INT_MSG_SEND_MAINTENANCE : INT_MSG_SEND_REPAIR_CODE);
		}
	}
}

void CSentinelDlgState::ProcessAttendantPaidPayoutsFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue)
{
	if (nullptr != msg)
	{
		if (JsonMemberExists(paramsJsonValue, "values"))
		{
			const rapidjson::Value& values = JsonGetObjectEx(paramsJsonValue, "values");
			msg->SetData(CUtilities::ConvertStringToInt64(JsonGetStringEx(values, "PayoutAmount")));
			msg->SetMessageType(INT_MSG_ATTENDANT_PAID_PAYOUT);
		}
	}
}

void CSentinelDlgState::ProcessHopperFillAmountFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue)
{
	if (nullptr != msg)
	{
		if (JsonMemberExists(paramsJsonValue, "values"))
		{
			const rapidjson::Value& values = JsonGetObjectEx(paramsJsonValue, "values");
			msg->SetData(CUtilities::ConvertStringToInt64(JsonGetStringEx(values, "HopperFillAmount")));
			msg->SetMessageType(INT_MSG_SEND_HOPPER_FILL);
		}
	}
}

void CSentinelDlgState::ProcessVideoInputFromTheUi(CInternalMessage *msg, const rapidjson::Value& paramsJsonValue)
{
	if (nullptr != msg)
	{
		if (JsonMemberExists(paramsJsonValue, "values"))
		{
			const rapidjson::Value& values = JsonGetObjectEx(paramsJsonValue, "values");
			
			string inputType = JsonGetStringEx(values, "InputType");
			bool inputModeIsAuto = JsonGetBoolEx(values, "InputModeAuto");
			int inputMode = JsonGetIntEx(values, "InputModeNumber");
			
			CVideoModeConfig *videoModeConfig = new CVideoModeConfig(inputType, inputModeIsAuto, inputMode);
			msg->SetVideoModeConfig(videoModeConfig);
			msg->SetMessageType(INT_MSG_SET_VIDEO_MODE_CONFIG);
		}
	}
}

void CSentinelDlgState::ProcessVideoOutputFromTheUi(CInternalMessage *msg, const rapidjson::Value& paramsJsonValue)
{
	if (nullptr != msg)
	{
		if (JsonMemberExists(paramsJsonValue, "values"))
		{
			const rapidjson::Value& values = JsonGetObjectEx(paramsJsonValue, "values");
			
			bool outputModeIsAuto = JsonGetBoolEx(values, "OutputModeAuto");
			int outputMode = JsonGetIntEx(values, "OutputModeNumber");
			
			CVideoModeConfig *videoModeConfig = new CVideoModeConfig(outputModeIsAuto, outputMode);
			msg->SetVideoModeConfig(videoModeConfig);
			msg->SetMessageType(INT_MSG_SET_VIDEO_MODE_CONFIG);
		}
	}
}

void CSentinelDlgState::ProcessVideoOffsetsFromTheUi(CInternalMessage *msg, const rapidjson::Value& paramsJsonValue)
{
	if (nullptr != msg)
	{
		if (JsonMemberExists(paramsJsonValue, "values"))
		{
			const rapidjson::Value& values = JsonGetObjectEx(paramsJsonValue, "values");
			
			int egmOffsetHorizontal = JsonGetIntEx(values, "EGMOffsetHorizontal");
			int egmOffsetVertical = JsonGetIntEx(values, "EGMOffsetVertical");
			int monitorOffsetHorizontal = JsonGetIntEx(values, "MonitorOffsetHorizontal");
			int monitorOffsetVertical = JsonGetIntEx(values, "MonitorOffsetVertical");
			
			CVideoModeConfig *videoModeConfig = new CVideoModeConfig(egmOffsetHorizontal, egmOffsetVertical, monitorOffsetHorizontal, monitorOffsetVertical);
			msg->SetVideoModeConfig(videoModeConfig);
			msg->SetMessageType(INT_MSG_SET_VIDEO_MODE_CONFIG);
		}
	}
}

void CSentinelDlgState::ProcessVideoKeepRevertFromTheUi(CInternalMessage *msg, const rapidjson::Value& paramsJsonValue)
{
	if (nullptr != msg)
	{
		if (JsonMemberExists(paramsJsonValue, "values"))
		{
			const rapidjson::Value& values = JsonGetObjectEx(paramsJsonValue, "values");
			msg->SetData((WORD)JsonGetBoolEx(values, "AcceptVideoMode"));
			msg->SetMessageType(INT_MSG_ACCEPT_VIDEO_MODE_CONFIG);
		}
	}
}

void CSentinelDlgState::ProcessMaintainEGMAspectRatioFromTheUi(CInternalMessage *msg, const rapidjson::Value& paramsJsonValue)
{
	if (nullptr != msg)
	{
		if (JsonMemberExists(paramsJsonValue, "values"))
		{
			const rapidjson::Value& values = JsonGetObjectEx(paramsJsonValue, "values");
			msg->SetData((WORD)JsonGetBoolEx(values, "MaintainEGMDisplayAspectRatio"));
			msg->SetMessageType(INT_MSG_GET_EGM_DISPLAY);
		}
	}
}

void CSentinelDlgState::ProcessA8ResetToCreditChangeFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue)
{
	if (nullptr != msg)
	{
		if (JsonMemberExists(paramsJsonValue, "values"))
		{
			const rapidjson::Value& values = JsonGetObjectEx(paramsJsonValue, "values");
			msg->SetData((WORD)JsonGetBoolEx(values, "A8ResetToCredit"));
			msg->SetMessageType(INT_MSG_SET_JACKPOT_RESET);
		}
	}
}

void CSentinelDlgState::ProcessSetVolumeFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue)
{
	if (nullptr != msg)
	{
		if (JsonMemberExists(paramsJsonValue, "values"))
		{
			const rapidjson::Value& values = JsonGetObjectEx(paramsJsonValue, "values");
			msg->SetData((WORD)JsonGetIntEx(values, "Volume"));
			msg->SetMessageType(INT_MSG_SET_VOLUME);
		}
	}
}

void CSentinelDlgState::ProcessSetBacklightFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue)
{
	if (nullptr != msg)
	{
		if (JsonMemberExists(paramsJsonValue, "values"))
		{
			const rapidjson::Value& values = JsonGetObjectEx(paramsJsonValue, "values");
			msg->SetData((WORD)JsonGetIntEx(values, "Backlight"));
			msg->SetMessageType(INT_MSG_SET_BACKLIGHT);
		}
	}
}

void CSentinelDlgState::ProcessEmployeePinFromTheUi(CInternalMessage *msg, const rapidjson::Value& paramsJsonValue)
{
	if (nullptr != msg)
	{
		if (JsonMemberExists(paramsJsonValue, "values"))
		{
			const rapidjson::Value& values = JsonGetObjectEx(paramsJsonValue, "values");
			string pin(JsonGetStringEx(values, "PIN"));
			msg->SetData(pin);
			msg->SetMessageType(INT_MSG_CHECK_EMPLOYEE_PIN);
		}
	}
}

void CSentinelDlgState::ProcessArhiveLogsFromTheUi(CInternalMessage *msg, const rapidjson::Value& paramsJsonValue)
{
	if (nullptr != msg)
	{
		if (JsonMemberExists(paramsJsonValue, "values"))
		{
			const rapidjson::Value& values = JsonGetObjectEx(paramsJsonValue, "values");
			m_archiveLogDescription = JsonGetStringEx(values, "ArchiveLogsMessage");
			msg->SetMessageType(INT_MSG_ARCHIVE_LOGS);
		}
	}
}

void CSentinelDlgState::ProcessSetRotationFromTheUi(CInternalMessage *msg, const rapidjson::Value& paramsJsonValue)
{
	if (nullptr != msg)
	{
		if (JsonMemberExists(paramsJsonValue, "values"))
		{
			const rapidjson::Value& values = JsonGetObjectEx(paramsJsonValue, "values");
			DesktopOrientationType displayRotation = CDisplayManager::ConvertDesktopOrientationFromWord(JsonGetIntEx(values, "DisplayRotation"));
			DesktopOrientationType touchRotation = CDisplayManager::ConvertDesktopOrientationFromWord(JsonGetIntEx(values, "TouchRotation"));

            msg->SetData((WORD)displayRotation, (WORD)touchRotation);
			msg->SetMessageType(INT_MSG_SET_DESKTOP_ORIENTATION_TYPE);
		}
	}
}

void CSentinelDlgState::ProcessMemoryClearFromTheUi(CInternalMessage *msg, const rapidjson::Value& paramsJsonValue)
{
	if (nullptr != msg)
	{
		if (JsonMemberExists(paramsJsonValue, "values"))
		{
			const rapidjson::Value& values = JsonGetObjectEx(paramsJsonValue, "values");
			msg->SetData(JsonGetStringEx(values, "MemoryType"));
			msg->SetMessageType(INT_MSG_CLEAR_PERSISTENT_MEMORY);
		}
	}
}

void CSentinelDlgState::ProcessTestUrlFromTheUi(CInternalMessage *msg, const rapidjson::Value& paramsJsonValue)
{
	if (nullptr != msg)
	{
		if (JsonMemberExists(paramsJsonValue, "values"))
		{
			const rapidjson::Value& values = JsonGetObjectEx(paramsJsonValue, "values");
			string url = JsonGetStringEx(values, "Url");
			if (m_testUrlSubscription && !url.empty())
			{
				long statusCode(0);
				HttpGet(url, nullptr, statusCode, true, HTTP_GET_TIMEOUT_IN_SECONDS);
				string jsonData("{\"StatusCode\":");
				jsonData += to_string(statusCode) + "}";
				m_testUrlSubscription->SetSubscriptionData(m_testUrlSubscription->GetTopic(), m_testUrlSubscription->GetWhereId(), jsonData);
				msg->SetMessageType(INT_MSG_TEST_URL);

				//Make a copy of m_testUrlSubscription and send to Internal message.So Internal message has responsibility 
				//to delete the heap address it is sent and CSentinelDlgState has responsibilty to free the memory of m_testUrlSubscription
				CSubscription *msgSubscription = new CSubscription(*m_testUrlSubscription);
				msg->SetData(msgSubscription);
			}
		}
	}
}

void CSentinelDlgState::ProcessSetTouchProtocolFromTheUi(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue)
{
	if (nullptr != msg)
	{
		if (JsonMemberExists(paramsJsonValue, "values"))
		{
			const rapidjson::Value& values = JsonGetObjectEx(paramsJsonValue, "values");
			msg->SetData(JsonGetStringEx(values, "Name"));
			msg->SetMessageType(INT_MSG_SET_TOUCH);
		}
	}
}

void CSentinelDlgState::ProcessStartPBTSessionFromAngularUI(CInternalMessage *msg, const rapidjson::Value& paramsJsonValue)
{
	if (nullptr != msg)
	{
		if (JsonMemberExists(paramsJsonValue, "values"))
		{
			const rapidjson::Value& values = JsonGetObjectEx(paramsJsonValue, "values");
			string pin(JsonGetStringEx(values, "PIN"));
			msg->SetData(pin);
			msg->SetData(CPersonalBanker::IsPinValidLength(pin) ? (WORD)true : (WORD)false);
			msg->SetMessageType(INT_MSG_PB_CHECK_PIN);
		}
	}
}

void CSentinelDlgState::ProcessStartPBTTransferFromAngularUI(CInternalMessage *msg, const rapidjson::Value& paramsJsonValue)
{
	if (nullptr != msg)
	{
		if (JsonMemberExists(paramsJsonValue, "values"))
		{
			const rapidjson::Value& values = JsonGetObjectEx(paramsJsonValue, "values");
			string transferTypeStr(JsonGetStringEx(values, "TransferType"));
			DWORD amount = JsonGetInt64(values, "Amount");

			PersonalBankerTransferType transferType;
			if (transferTypeStr == "Undeclared")
			{
				transferType = PersonalBankerTransferType::Undeclared;
			}
			else if (transferTypeStr == "CashToGame")
			{
				transferType = PersonalBankerTransferType::CashToGame;
			}
			else if (transferTypeStr == "PointsToGame")
			{
				transferType = PersonalBankerTransferType::PointsToGame;
			}
			else if (transferTypeStr == "PromoDollarsToGame")
			{
				transferType = PersonalBankerTransferType::PromoDollarsToGame;
			}
			else if (transferTypeStr == "CashFromGame")
			{
				transferType = PersonalBankerTransferType::CashFromGame;
			}
			else if (transferTypeStr == "PointsFromGame")
			{
				transferType = PersonalBankerTransferType::PointsFromGame;
			}
			else if (transferTypeStr == "PromoDollarsFromGame")
			{
				transferType = PersonalBankerTransferType::PromoDollarsFromGame;
			}
			else if (transferTypeStr == "ExternalCredit")
			{
				transferType = PersonalBankerTransferType::ExternalCredit;
			}
			else if (transferTypeStr == "AllFromGame")
			{
				transferType = PersonalBankerTransferType::AllFromGame;
			}
			msg->SetData((DWORD)transferType, amount);
			msg->SetMessageType(INT_MSG_PB_REQUEST_TRANSFER);
		}
	}
}

void CSentinelDlgState::ProcessResetPBTPinFromAngularUI(CInternalMessage* msg, const rapidjson::Value& paramsJsonValue)
{
	if (nullptr != msg)
	{
		if (JsonMemberExists(paramsJsonValue, "values"))
		{
			const rapidjson::Value& values = JsonGetObjectEx(paramsJsonValue, "values");
			string pin(JsonGetStringEx(values, "PIN"));
			WORD pinNumber = std::stoi(pin);
			msg->SetData(pinNumber);
			msg->SetMessageType(INT_MSG_PB_SET_NEW_PIN);
		}
	}
}

void CSentinelDlgState::SetCardEventSubscription(CardType cardType, bool showApplyRevertScreen, bool pinRequired)
{
	string cardTypeStr(GetCardTypeStr(cardType, pinRequired));

	string jsonData("{\"EmployeeCardEvent\":{\"CardType\":");
	jsonData += "\"" + cardTypeStr + "\",\"ShowApplyRevertScreen\":";
	jsonData += showApplyRevertScreen ? "true" : "false";
	jsonData += ",\"PinRequired\":";
	jsonData += pinRequired ? "true}}" : "false}}";

	if (m_employeeCardEventSubscription)
	{
		m_employeeCardEventSubscription->SetSubscriptionData(m_employeeCardEventSubscription->GetTopic(), m_employeeCardEventSubscription->GetWhereId(), jsonData);
	}
}

void CSentinelDlgState::GetEmployeeCardEventSubscriptionData(std::string& subscriptionData)
{
	if (m_employeeCardEventSubscription)
	{
		subscriptionData = m_employeeCardEventSubscription->GetSubscriptionData();
	}
}

void CSentinelDlgState::SetClearOnCardRemove(bool clearValue)
{
	m_clearOnCardRemove = clearValue;
}

/// <summary>
/// Gets clear screen on card removal state
/// </summary>
/// <param name="clearValue">OUT - Clear on card removal state</param>
bool CSentinelDlgState::GetClearOnCardRemove()
{
	return m_clearOnCardRemove;
}

std::string CSentinelDlgState::GetCardTypeStr(CardType cardType, bool& pinRequired)
{
	string cardTypeStr;
	switch (cardType)
	{
	case CARD_MECHANIC:
		cardTypeStr = "mechanic";
		break;
	case CARD_GLOBAL:
		cardTypeStr = "global";
		break;
	case CARD_FLOOR:
		cardTypeStr = "floor";
		break;
	case CARD_METER:
		cardTypeStr = "meter";
		pinRequired = false;
		break;
	case CARD_CURRENCY:
		cardTypeStr = "currency";
		pinRequired = false;
		break;
	default:
		break;
	}

	return cardTypeStr;
}