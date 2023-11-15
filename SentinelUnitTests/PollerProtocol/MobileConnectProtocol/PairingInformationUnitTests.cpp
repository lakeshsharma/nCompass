#include "stdafx.h"
#include "RapidJsonTestHelper.h"
#include "PollerProtocol/MobileConnectProtocol/PairingInformation.h"

// Unit testing includes.
#include "UnitTestingOperatingSystem.h"

// Unit testing includes.
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::Mock;
using ::testing::_;
using ::testing::Return;
using ::testing::Invoke;
using ::testing::NiceMock;

class CBeaconManagerMock : public CBeaconManager
{
public:
	CBeaconManagerMock() {}
	~CBeaconManagerMock() {}

	MOCK_METHOD3(SetEddystoneFrameData, bool(const std::string& uuid, const std::string& checksum, uint8_t measuredPower));
};

namespace SentinelUnitTests
{
    class PairingInformation : public ::testing::Test {
		protected:
			PairingInformation(){}

			virtual void SetUp()
			{
			}

			virtual void TearDown() 
			{
			}

			const rapidjson::Document GetJsonDocument(string jsonString)
			{
				rapidjson::Document jsonDoc;
				if (jsonDoc.Parse(jsonString.c_str()).HasParseError())
					throw JsonException(jsonDoc.GetParseError(), nullptr, "Parse");
				return jsonDoc;
			}

			const std::string SHORT_CODE = "ShortCode";
			const std::string BEACON = "Beacon";
			const std::string PAIRING_ID_SET = "PairingIDSet";
			const std::string BEACON_FAILURE = "BeaconFailure";
			const std::string PAIRING_TYPE_MISMATCH = "PairingTypeMismatch";
		
		void PairingInformation_Default()
		{
			// Perform operation(s) and test(s).
			CPairingInformation &obj = CPairingInformation::Instance();
			ASSERT_TRUE(obj.m_bPairingEnabled == CPairingInformation::DEFAULT_PAIRING_ENABLED) << "m_bPairingEnabled";
			ASSERT_TRUE(obj.m_uPairingTimeOutMs == CPairingInformation::DEFAULT_PAIRING_TIMEOUT_MS) << "m_uPairingTimeOutMs";
			ASSERT_FALSE(obj.m_waitForPairing) << "m_waitForPairing";
			ASSERT_FALSE(obj.m_waitForRequestCardChange) << "m_waitForRequestCardChange";
			ASSERT_FALSE(obj.m_fallbackEnabled) << "m_fallbackEnabled";
			ASSERT_FALSE(obj.m_mobileCardedSession) << "m_mobileCardedSession";
			ASSERT_TRUE(obj.m_measuredPower == CPairingInformation::DEFAULT_MEASURED_POWER) << "m_measuredPower";
		}

		void PairingInformation_UpdateFromPairingSettingsEvent()
		{
			// Setup
			static const char* pairingSettingsEventJson =
				R"(
				{
					"messageType": "mobile.event.pairingSettings",
					"pairingType": "Beacon",
					"deviceId": "101",
					"deviceType": "Slot",
					"pairingEnabled": true,
					"pairingTimeOutMs": 25000,
					"idlePairingId": "01E4115C71F0AAC29000",
					"idlePairingChecksum": "86A2AA75B058",
					"confirmCardRequest": false,
					"transmitPower": 2,
					"measuredPower": 33
				}
			)";

			CPairingSettingsEvent pairingSettingsEvent(GetJsonDocument(pairingSettingsEventJson));
			CPairingInformation &obj = CPairingInformation::Instance();

			// Perform operation(s) and test(s).
			obj.UpdateFromPairingSettingsEvent(&pairingSettingsEvent);
			ASSERT_TRUE(obj.m_sPairingType == "Beacon") << "m_sPairingType";
			ASSERT_TRUE(obj.m_bPairingEnabled) << "m_bPairingEnabled";
			ASSERT_TRUE(obj.m_uPairingTimeOutMs == 25000) << "m_uPairingTimeOutMs";
			ASSERT_TRUE(obj.m_sIdlePairingId == "01E4115C71F0AAC29000") << "m_sIdlePairingId";
			ASSERT_TRUE(obj.m_sIdlePairingChecksum == "86A2AA75B058") << "m_sIdlePairingChecksum";
			ASSERT_TRUE(obj.m_measuredPower == 33) << "m_measuredPower";
		}

		void PairingInformation_UpdateFromPairingSettingsEvent_NULL()
		{
			// Setup
			static const char* pairingSettingsEventJson =
				R"(
				{
					"messageType": "mobile.event.pairingSettings",
					"pairingType": "Beacon",
					"deviceId": "101",
					"deviceType": "Slot",
					"pairingEnabled": true,
					"pairingTimeOutMs": 25000,
					"idlePairingId": "01E4115C71F0AAC29000",
					"idlePairingChecksum": "86A2AA75B058",
					"confirmCardRequest": false,
					"transmitPower": 2,
					"measuredPower": 33
				}
			)";
			CPairingSettingsEvent pairingSettingsEvent(GetJsonDocument(pairingSettingsEventJson));
			CPairingInformation &obj = CPairingInformation::Instance();
			obj.UpdateFromPairingSettingsEvent(&pairingSettingsEvent);

			// Perform operation(s) and test(s).
			obj.UpdateFromPairingSettingsEvent(NULL);
			ASSERT_TRUE(obj.m_sPairingType == "Beacon") << "m_sPairingType";
			ASSERT_TRUE(obj.m_bPairingEnabled) << "m_bPairingEnabled";
			ASSERT_TRUE(obj.m_uPairingTimeOutMs == 25000) << "m_uPairingTimeOutMs";
			ASSERT_TRUE(obj.m_sIdlePairingId == "01E4115C71F0AAC29000") << "m_sIdlePairingId";
			ASSERT_TRUE(obj.m_sIdlePairingChecksum == "86A2AA75B058") << "m_sIdlePairingChecksum";
			ASSERT_TRUE(obj.m_measuredPower == 33) << "m_measuredPower";
		}

		void PairingInformation_GetPairingType()
		{
			// Setup.
			CPairingInformation &obj = CPairingInformation::Instance();
			obj.m_sPairingType = "Beacon";

			// Perform operation(s) and test(s).
			ASSERT_TRUE(obj.GetPairingType() == "Beacon") << "GetPairingType()";
		}

		void PairingInformation_SetPairingType()
		{
			// Setup.
			CPairingInformation &obj = CPairingInformation::Instance();

			// Perform operation(s) and test(s).
			obj.SetPairingType("Beacon");
			ASSERT_TRUE(obj.m_sPairingType == "Beacon") << "SetPairingType()";
		}

		void PairingInformation_IsPairingEnabled()
		{
			// Setup.
			CPairingInformation &obj = CPairingInformation::Instance();
			obj.m_bPairingEnabled = false;

			// Perform operation(s) and test(s).
			ASSERT_FALSE(obj.IsPairingEnabled()) << "IsPairingEnabled() - false";

			obj.m_bPairingEnabled = true;
			ASSERT_TRUE(obj.IsPairingEnabled()) << "IsPairingEnabled() -  true";
		}

		void PairingInformation_SetPairingEnabled()
		{
			// Setup.
			CPairingInformation &obj = CPairingInformation::Instance();

			// Perform operation(s) and test(s).
			obj.SetPairingEnabled(false);
			ASSERT_FALSE(obj.m_bPairingEnabled) << "SetPairingEnabled() - false";

			obj.SetPairingEnabled(true);
			ASSERT_TRUE(obj.m_bPairingEnabled) << "SetPairingEnabled() - true";
		}

		void PairingInformation_GetPairingTimeOutMs()
		{
			// Setup.
			CPairingInformation &obj = CPairingInformation::Instance();
			obj.m_uPairingTimeOutMs = 15000;

			// Perform operation(s) and test(s).
			ASSERT_TRUE(obj.GetPairingTimeOutMs() == 15000) << "GetPairingTimeOutMs()";
		}

		void PairingInformation_SetPairingTimeOutMs()
		{
			// Setup.
			CPairingInformation &obj = CPairingInformation::Instance();

			// Perform operation(s) and test(s).
			obj.SetPairingTimeOutMs(12000);
			ASSERT_TRUE(obj.m_uPairingTimeOutMs == 12000) << "SetPairingTimeOutMs()";
		}

		void PairingInformation_GetIdlePairingId()
		{
			// Setup.
			CPairingInformation &obj = CPairingInformation::Instance();
			obj.m_sIdlePairingId = "ABCDE12345";

			// Perform operation(s) and test(s).
			ASSERT_TRUE(obj.GetIdlePairingId() == "ABCDE12345") << "GetIdlePairingId()";
		}

		void PairingInformation_SetIdlePairingId()
		{
			// Setup.
			CPairingInformation &obj = CPairingInformation::Instance();

			// Perform operation(s) and test(s).
			obj.SetIdlePairingId("12345ABCDE");
			ASSERT_TRUE(obj.m_sIdlePairingId == "12345ABCDE") << "SetIdlePairingId()";
		}

		void PairingInformation_GetIdlePairingChecksum()
		{
			// Setup.
			CPairingInformation &obj = CPairingInformation::Instance();
			obj.m_sIdlePairingChecksum = "ABCDE12345";

			// Perform operation(s) and test(s).
			ASSERT_TRUE(obj.GetIdlePairingChecksum() == "ABCDE12345") << "GetIdlePairingChecksum()";
		}

		void PairingInformation_SetIdlePairingChecksum()
		{
			// Setup.
			CPairingInformation &obj = CPairingInformation::Instance();

			// Perform operation(s) and test(s).
			obj.SetIdlePairingChecksum("12345ABCDE");
			ASSERT_TRUE(obj.m_sIdlePairingChecksum == "12345ABCDE") << "SetIdlePairingChecksum()";
		}

		void PairingInformation_IsWaitingForPairing()
		{
			// Setup.
			CPairingInformation &obj = CPairingInformation::Instance();
			obj.m_waitForPairing = false;

			// Perform operation(s) and test(s).
			ASSERT_FALSE(obj.IsWaitingForPairing()) << "IsWaitingForPairing() - false";

			obj.m_waitForPairing = true;
			ASSERT_TRUE(obj.IsWaitingForPairing()) << "IsWaitingForPairing() -  true";
		}

		void PairingInformation_SetWaitingForPairing()
		{
			// Setup.
			CPairingInformation &obj = CPairingInformation::Instance();

			// Perform operation(s) and test(s).
			obj.SetWaitingForPairing(false);
			ASSERT_FALSE(obj.m_waitForPairing) << "SetPairingEnabled() - false";

			obj.SetWaitingForPairing(true);
			ASSERT_TRUE(obj.m_waitForPairing) << "SetPairingEnabled() - true";
		}

		void PairingInformation_GetPairingRequestTime()
		{
			// Setup.
			CPairingInformation &obj = CPairingInformation::Instance();
			time_t time = CUtilities::GetCurrentTime();
			obj.m_pairingRequestTime = time;

			// Perform operation(s) and test(s).
			ASSERT_TRUE(obj.GetPairingRequestTime() == time) << "GetPairingRequestTime()";
		}

		void PairingInformation_SetPairingRequestTime()
		{
			// Setup.
			CPairingInformation &obj = CPairingInformation::Instance();
			time_t time = CUtilities::GetCurrentTime();

			// Perform operation(s) and test(s).
			obj.SetPairingRequestTime(time);
			ASSERT_TRUE(obj.m_pairingRequestTime == time) << "SetPairingRequestTime()";
		}

		void PairingInformation_IsWaitingForRequestCardChange()
		{
			// Setup.
			CPairingInformation &obj = CPairingInformation::Instance();
			obj.m_waitForRequestCardChange = false;

			// Perform operation(s) and test(s).
			ASSERT_FALSE(obj.IsWaitingForRequestCardChange()) << "IsWaitingForRequestCardChange() - false";

			obj.m_waitForRequestCardChange = true;
			ASSERT_TRUE(obj.IsWaitingForRequestCardChange()) << "IsWaitingForRequestCardChange() -  true";
		}

		void PairingInformation_SetWaitingForRequestCardChange()
		{
			// Setup.
			CPairingInformation &obj = CPairingInformation::Instance();

			// Perform operation(s) and test(s).
			obj.SetWaitingForRequestCardChange(false);
			ASSERT_FALSE(obj.m_waitForRequestCardChange) << "SetWaitingForRequestCardChange() - false";

			obj.SetWaitingForRequestCardChange(true);
			ASSERT_TRUE(obj.m_waitForRequestCardChange) << "SetWaitingForRequestCardChange() - true";
		}

		void PairingInformation_GetRequestCardChangeTime()
		{
			// Setup.
			CPairingInformation &obj = CPairingInformation::Instance();
			time_t time = CUtilities::GetCurrentTime();
			obj.m_requestCardChangeTime = time;

			// Perform operation(s) and test(s).
			ASSERT_TRUE(obj.GetRequestCardChangeTime() == time) << "GetRequestCardChangeTime()";
		}

		void PairingInformation_SetRequestCardChangeTime()
		{
			// Setup.
			CPairingInformation &obj = CPairingInformation::Instance();
			time_t time = CUtilities::GetCurrentTime();

			// Perform operation(s) and test(s).
			obj.SetRequestCardChangeTime(time);
			ASSERT_TRUE(obj.m_requestCardChangeTime == time) << "SetRequestCardChangeTime()";
		}

		void PairingInformation_GetCorrelationId()
		{
			// Setup.
			CPairingInformation &obj = CPairingInformation::Instance();
			obj.m_correlationId[MessageTypeForCorrelation::RequestCardChange] = "Request_Card_Change";
			obj.m_correlationId[MessageTypeForCorrelation::RequestFundsTransferBalance] = "Request_Funds_Transfer_Balance";
			obj.m_correlationId[MessageTypeForCorrelation::RequestFundsTransfer] = "Request_Funds_Transfer";

			// Perform operation(s) and test(s).
			ASSERT_TRUE(obj.GetCorrelationId(MessageTypeForCorrelation::RequestCardChange) == "Request_Card_Change") << "GetCorrelationId() - RequestCardChange";
			ASSERT_TRUE(obj.GetCorrelationId(MessageTypeForCorrelation::RequestFundsTransferBalance) == "Request_Funds_Transfer_Balance") << "GetCorrelationId() - RequestFundsTransferBalance";
			ASSERT_TRUE(obj.GetCorrelationId(MessageTypeForCorrelation::RequestFundsTransfer) == "Request_Funds_Transfer") << "GetCorrelationId() - RequestFundsTransfer";
		}

		void PairingInformation_SetCorrelationId()
		{
			// Setup.
			CPairingInformation &obj = CPairingInformation::Instance();

			// Perform operation(s) and test(s).
			obj.SetCorrelationId(MessageTypeForCorrelation::RequestCardChange, "request_card_change");
			obj.SetCorrelationId(MessageTypeForCorrelation::RequestFundsTransferBalance, "request_funds_transfer_balance");
			obj.SetCorrelationId(MessageTypeForCorrelation::RequestFundsTransfer, "request_funds_transfer");
			ASSERT_TRUE(obj.m_correlationId[MessageTypeForCorrelation::RequestCardChange] == "request_card_change") << "SetCorrelationId() - RequestCardChange";
			ASSERT_TRUE(obj.m_correlationId[MessageTypeForCorrelation::RequestFundsTransferBalance] == "request_funds_transfer_balance") << "SetCorrelationId() - RequestFundsTransferBalance";
			ASSERT_TRUE(obj.m_correlationId[MessageTypeForCorrelation::RequestFundsTransfer] == "request_funds_transfer") << "SetCorrelationId() - RequestFundsTransfer";
		}

		void PairingInformation_IsFallbackEnabled()
		{
			// Setup.
			CPairingInformation &obj = CPairingInformation::Instance();
			obj.m_fallbackEnabled = false;

			// Perform operation(s) and test(s).
			ASSERT_FALSE(obj.IsFallbackEnabled()) << "IsFallbackEnabled() - false";

			obj.m_fallbackEnabled = true;
			ASSERT_TRUE(obj.IsFallbackEnabled()) << "IsFallbackEnabled() -  true";
		}

		void PairingInformation_SetFallbackEnabled()
		{
			// Setup.
			CPairingInformation &obj = CPairingInformation::Instance();

			// Perform operation(s) and test(s).
			obj.SetFallbackEnabled(false);
			ASSERT_FALSE(obj.m_fallbackEnabled) << "SetFallbackEnabled() - false";

			obj.SetFallbackEnabled(true);
			ASSERT_TRUE(obj.m_fallbackEnabled) << "SetFallbackEnabled() - true";
		}

		void PairingInformation_IsMobileCardedSession()
		{
			// Setup.
			CPairingInformation &obj = CPairingInformation::Instance();
			obj.m_mobileCardedSession = false;

			// Perform operation(s) and test(s).
			ASSERT_FALSE(obj.IsMobileCardedSession()) << "IsMobileCardedSession() - false";

			obj.m_mobileCardedSession = true;
			ASSERT_TRUE(obj.IsMobileCardedSession()) << "IsMobileCardedSession() -  true";
		}

		void PairingInformation_SetMobileCardedSession()
		{
			// Setup.
			CPairingInformation &obj = CPairingInformation::Instance();

			// Perform operation(s) and test(s).
			obj.SetMobileCardedSession(false);
			ASSERT_FALSE(obj.m_mobileCardedSession) << "SetMobileCardedSession() - false";

			obj.SetMobileCardedSession(true);
			ASSERT_TRUE(obj.m_mobileCardedSession) << "SetMobileCardedSession() - true";
		}

		void PairingInformation_CheckMobilePairingTimeOut_NotWaiting()
		{
			// Setup.
			CPairingInformation &obj = CPairingInformation::Instance();
			obj.m_waitForPairing = false;

			// Perform operation(s) and test(s).
			ASSERT_FALSE(obj.CheckMobilePairingTimeOut()) << "CheckMobilePairingTimeOut()";
		}

		void PairingInformation_CheckMobilePairingTimeOut_NotTimeout()
		{
			// Setup.
			CPairingInformation &obj = CPairingInformation::Instance();
			time_t currentTime = CUtilities::GetCurrentTime();
			obj.m_waitForPairing = true;
			obj.m_pairingRequestTime = currentTime - 8;
			obj.m_uPairingTimeOutMs = 10000;

			// Perform operation(s) and test(s).
			ASSERT_FALSE(obj.CheckMobilePairingTimeOut()) << "CheckMobilePairingTimeOut()";
		}

		void PairingInformation_CheckMobilePairingTimeOut_Timeout()
		{
			// Setup.
			CPairingInformation &obj = CPairingInformation::Instance();
			time_t currentTime = CUtilities::GetCurrentTime();
			obj.m_waitForPairing = true;
			obj.m_pairingRequestTime = currentTime - 12;
			obj.m_uPairingTimeOutMs = 10000;

			// Perform operation(s) and test(s).
			ASSERT_TRUE(obj.CheckMobilePairingTimeOut()) << "CheckMobilePairingTimeOut()";
		}

		void PairingInformation_CheckMobileRequestCardChangeTimeOut_NotWaiting()
		{
			// Setup.
			CPairingInformation &obj = CPairingInformation::Instance();
			obj.m_waitForRequestCardChange = false;

			// Perform operation(s) and test(s).
			ASSERT_FALSE(obj.CheckMobileRequestCardChangeTimeOut()) << "CheckMobileRequestCardChangeTimeOut()";
		}

		void PairingInformation_CheckMobileRequestCardChangeTimeOut_NotTimeout()
		{
			// Setup.
			CPairingInformation &obj = CPairingInformation::Instance();
			time_t currentTime = CUtilities::GetCurrentTime();
			obj.m_waitForRequestCardChange = true;
			obj.m_requestCardChangeTime = currentTime - 8;
			obj.m_uPairingTimeOutMs = 10000;

			// Perform operation(s) and test(s).
			ASSERT_FALSE(obj.CheckMobileRequestCardChangeTimeOut()) << "CheckMobileRequestCardChangeTimeOut()";
		}

		void PairingInformation_CheckMobileRequestCardChangeTimeOut_Timeout()
		{
			// Setup.
			CPairingInformation &obj = CPairingInformation::Instance();
			time_t currentTime = CUtilities::GetCurrentTime();
			obj.m_waitForRequestCardChange = true;
			obj.m_requestCardChangeTime = currentTime - 12;
			obj.m_uPairingTimeOutMs = 10000;

			// Perform operation(s) and test(s).
			ASSERT_TRUE(obj.CheckMobileRequestCardChangeTimeOut()) << "CheckMobileRequestCardChangeTimeOut()";
		}

		void PairingInformation_HandlePairingEvent_ShortCode_Success()
		{
			// Setup.
			static const char* pairingEventJson =
				R"(
				{
					"messageType": "mobile.event.pairing",
					"deviceType": "Slot",
					"deviceId": "1191",
					"pairingType": "ShortCode",
					"pairingId": "7066",
					"pairingChecksum": "201CAE90371E",
					"success": true,
					"status":""
				}
			)";

			CPairingEvent pairingEvent(GetJsonDocument(pairingEventJson));
			CNotificationBox notificationBox;
			bool success = false;
			std::string status;
			CPairingInformation &obj = CPairingInformation::Instance();
			obj.m_sPairingType = SHORT_CODE;
			obj.m_fallbackEnabled = true;
			time_t currentTime = CUtilities::GetCurrentTime();

			// Perform operation(s) and test(s).
			obj.HandlePairingEvent(&pairingEvent, notificationBox, NULL, success, status);
			ASSERT_TRUE(notificationBox.m_mobileState == MobileState::PleaseEnterCode) << "notificationBox.m_mobileState";
			ASSERT_TRUE(notificationBox.m_mobilePairingCode == "7066") << "notificationBox.m_mobilePairingCode";
			ASSERT_TRUE(obj.m_requestCardChangeTime - currentTime < 2) << "obj.m_requestCardChangeTime";
			ASSERT_FALSE(obj.m_fallbackEnabled) << "obj.m_fallbackEnabled";
		}

		void PairingInformation_HandlePairingEvent_ShortCode_EventFailure()
		{
			// Setup.
			static const char* pairingEventJson =
				R"(
				{
					"messageType": "mobile.event.pairing",
					"deviceType": "Slot",
					"deviceId": "1191",
					"pairingType": "ShortCode",
					"pairingId": "7066",
					"pairingChecksum": "201CAE90371E",
					"success": false,
					"status":"some-error"
				}
			)";

			CPairingEvent pairingEvent(GetJsonDocument(pairingEventJson));
			CNotificationBox notificationBox;
			bool success = true;
			std::string status;
			CPairingInformation &obj = CPairingInformation::Instance();
			obj.m_sPairingType = BEACON;
			obj.m_fallbackEnabled = true;

			// Perform operation(s) and test(s).
			obj.HandlePairingEvent(&pairingEvent, notificationBox, NULL, success, status);
			ASSERT_TRUE(notificationBox.m_mobileState == MobileState::ErrorOccured) << "notificationBox.m_mobileState";
			ASSERT_TRUE(notificationBox.m_mobilePairingCode == "some-error") << "notificationBox.m_mobilePairingCode";
			ASSERT_FALSE(success) << "success";
			ASSERT_TRUE(status == "some-error") << "status";
		}

		void PairingInformation_HandlePairingEvent_Beacon_Success()
		{
			// Setup.
			static const char* pairingEventJson =
				R"(
				{
					"messageType": "mobile.event.pairing",
					"deviceType": "Slot",
					"deviceId": "1191",
					"pairingType": "Beacon",
					"pairingId": "7066",
					"pairingChecksum": "201CAE90371E",
					"success": true,
					"status":"some-error"
				}
			)";

			CPairingEvent pairingEvent(GetJsonDocument(pairingEventJson));
			CNotificationBox notificationBox;
			bool success = false;
			std::string status;
			CPairingInformation &obj = CPairingInformation::Instance();
			obj.m_sPairingType = BEACON;
			time_t currentTime = CUtilities::GetCurrentTime();

			NiceMock<CBeaconManagerMock> beaconManager;
			ON_CALL(beaconManager, SetEddystoneFrameData(_, _, _)).WillByDefault(Return(true));

			// Perform operation(s) and test(s).
			obj.HandlePairingEvent(&pairingEvent, notificationBox, &beaconManager, success, status);
			ASSERT_TRUE(success) << "success";
			ASSERT_TRUE(status == PAIRING_ID_SET) << "status";
			ASSERT_TRUE(obj.m_requestCardChangeTime - currentTime < 2) << "obj.m_requestCardChangeTime";
		}

		void PairingInformation_HandlePairingEvent_Beacon_Failure()
		{
			// Setup.
			static const char* pairingEventJson =
				R"(
				{
					"messageType": "mobile.event.pairing",
					"deviceType": "Slot",
					"deviceId": "1191",
					"pairingType": "Beacon",
					"pairingId": "7066",
					"pairingChecksum": "201CAE90371E",
					"success": true,
					"status":"some-error"
				}
			)";

			CPairingEvent pairingEvent(GetJsonDocument(pairingEventJson));
			CNotificationBox notificationBox;
			bool success = false;
			std::string status;
			CPairingInformation &obj = CPairingInformation::Instance();
			obj.m_sPairingType = BEACON;
			obj.m_waitForPairing = false;
			obj.m_fallbackEnabled = false;

			NiceMock<CBeaconManagerMock> beaconManager;
			ON_CALL(beaconManager, SetEddystoneFrameData(_, _, _)).WillByDefault(Return(false));

			// Perform operation(s) and test(s).
			obj.HandlePairingEvent(&pairingEvent, notificationBox, &beaconManager, success, status);
			ASSERT_FALSE(success) << "success";
			ASSERT_TRUE(status == BEACON_FAILURE) << "status";
			ASSERT_TRUE(obj.m_waitForPairing) << "m_waitForPairing";
			ASSERT_TRUE(obj.m_fallbackEnabled) << "m_fallbackEnabled";
		}

		void PairingInformation_HandlePairingEvent_Beacon_EventFailure()
		{
			// Setup.
			static const char* pairingEventJson =
				R"(
				{
					"messageType": "mobile.event.pairing",
					"deviceType": "Slot",
					"deviceId": "1191",
					"pairingType": "Beacon",
					"pairingId": "7066",
					"pairingChecksum": "201CAE90371E",
					"success": false,
					"status":"some-error"
				}
			)";

			CPairingEvent pairingEvent(GetJsonDocument(pairingEventJson));
			CNotificationBox notificationBox;
			bool success = false;
			std::string status;
			CPairingInformation &obj = CPairingInformation::Instance();
			obj.m_sPairingType = BEACON;

			// Perform operation(s) and test(s).
			obj.HandlePairingEvent(&pairingEvent, notificationBox, NULL, success, status);
			ASSERT_TRUE(notificationBox.m_mobileState == MobileState::ErrorOccured) << "notificationBox.m_mobileState";
			ASSERT_TRUE(notificationBox.m_mobilePairingCode == "some-error") << "notificationBox.m_mobilePairingCode";
			ASSERT_FALSE(success) << "success";
			ASSERT_TRUE(status == "some-error") << "status";
		}

		void PairingInformation_HandlePairingEvent_PairingTypeMismatch()
		{
			// Setup.
			static const char* pairingEventJson =
				R"(
				{
					"messageType": "mobile.event.pairing",
					"deviceType": "Slot",
					"deviceId": "1191",
					"pairingType": "Beacon",
					"pairingId": "7066",
					"pairingChecksum": "201CAE90371E",
					"success": true,
					"status":"some-error"
				}
			)";

			CPairingEvent pairingEvent(GetJsonDocument(pairingEventJson));
			CNotificationBox notificationBox;
			bool success = false;
			std::string status;
			CPairingInformation &obj = CPairingInformation::Instance();
			obj.m_sPairingType = SHORT_CODE;

			// Perform operation(s) and test(s).
			obj.HandlePairingEvent(&pairingEvent, notificationBox, NULL, success, status);
			ASSERT_TRUE(notificationBox.m_mobileState == MobileState::ErrorOccured) << "notificationBox.m_mobileState";
			ASSERT_TRUE(notificationBox.m_mobilePairingCode == PAIRING_TYPE_MISMATCH) << "notificationBox.m_mobilePairingCode";
			ASSERT_FALSE(success) << "success";
			ASSERT_TRUE(status == PAIRING_TYPE_MISMATCH) << "status";
		}
	};

// #pragma region CPairingInformation
		/// <summary>
		/// Functional test for:
		///     CPairingInformation::CPairingInformation()
		/// Test for initial values of object created via default constructor.
		/// </summary>
		TEST_F(PairingInformation, PairingInformation_Default)
		{
			PairingInformation_Default();
		}

		/// <summary>
		/// Functional test for:
		///     CPairingInformation::UpdateFromPairingSettingsEvent()
		/// Test for UpdateFromPairingSettingsEvent() method.
		/// </summary>
		TEST_F(PairingInformation, PairingInformation_UpdateFromPairingSettingsEvent)
		{
			PairingInformation_UpdateFromPairingSettingsEvent();
		}

		/// <summary>
		/// Functional test for:
		///     CPairingInformation::UpdateFromPairingSettingsEvent()
		/// Test for UpdateFromPairingSettingsEvent() method when NULL is passed.
		/// </summary>
		TEST_F(PairingInformation, PairingInformation_UpdateFromPairingSettingsEvent_NULL)
		{
			PairingInformation_UpdateFromPairingSettingsEvent_NULL();
		}

		/// <summary>
		/// Functional test for:
		///     CPairingInformation::GetPairingType()
		/// Test for GetPairingType() method.
		/// </summary>
		TEST_F(PairingInformation, PairingInformation_GetPairingType)
		{
			PairingInformation_GetPairingType();
		}

		/// <summary>
		/// Functional test for:
		///     CPairingInformation::SetPairingType()
		/// Test for SetPairingType() method.
		/// </summary>
		TEST_F(PairingInformation, PairingInformation_SetPairingType)
		{
			PairingInformation_SetPairingType();
		}

		/// <summary>
		/// Functional test for:
		///     CPairingInformation::IsPairingEnabled()
		/// Test for IsPairingEnabled() method.
		/// </summary>
		TEST_F(PairingInformation, PairingInformation_IsPairingEnabled)
		{
			PairingInformation_IsPairingEnabled();
		}

		/// <summary>
		/// Functional test for:
		///     CPairingInformation::SetPairingEnabled()
		/// Test for SetPairingEnabled() method.
		/// </summary>
		TEST_F(PairingInformation, PairingInformation_SetPairingEnabled)
		{
			PairingInformation_SetPairingEnabled();
		}

		/// <summary>
		/// Functional test for:
		///     CPairingInformation::GetPairingTimeOutMs()
		/// Test for GetPairingTimeOutMs() method.
		/// </summary>
		TEST_F(PairingInformation, PairingInformation_GetPairingTimeOutMs)
		{
			PairingInformation_GetPairingTimeOutMs();
		}

		/// <summary>
		/// Functional test for:
		///     CPairingInformation::SetPairingTimeOutMs()
		/// Test for SetPairingTimeOutMs() method.
		/// </summary>
		TEST_F(PairingInformation, PairingInformation_SetPairingTimeOutMs)
		{
			PairingInformation_SetPairingTimeOutMs();
		}

		/// <summary>
		/// Functional test for:
		///     CPairingInformation::GetIdlePairingId()
		/// Test for GetIdlePairingId() method.
		/// </summary>
		TEST_F(PairingInformation, PairingInformation_GetIdlePairingId)
		{
			PairingInformation_GetIdlePairingId();
		}

		/// <summary>
		/// Functional test for:
		///     CPairingInformation::SetIdlePairingId()
		/// Test for SetIdlePairingId() method.
		/// </summary>
		TEST_F(PairingInformation, PairingInformation_SetIdlePairingId)
		{
			PairingInformation_SetIdlePairingId();
		}

		/// <summary>
		/// Functional test for:
		///     CPairingInformation::GetIdlePairingChecksum()
		/// Test for GetIdlePairingChecksum() method.
		/// </summary>
		TEST_F(PairingInformation, PairingInformation_GetIdlePairingChecksum)
		{
			PairingInformation_GetIdlePairingChecksum();
		}

		/// <summary>
		/// Functional test for:
		///     CPairingInformation::SetIdlePairingChecksum()
		/// Test for SetIdlePairingChecksum() method.
		/// </summary>
		TEST_F(PairingInformation, PairingInformation_SetIdlePairingChecksum)
		{
			PairingInformation_SetIdlePairingChecksum();
		}

		/// <summary>
		/// Functional test for:
		///     CPairingInformation::IsWaitingForPairing()
		/// Test for IsWaitingForPairing() method.
		/// </summary>
		TEST_F(PairingInformation, PairingInformation_IsWaitingForPairing)
		{
			PairingInformation_IsWaitingForPairing();
		}

		/// <summary>
		/// Functional test for:
		///     CPairingInformation::SetWaitingForPairing()
		/// Test for SetWaitingForPairing() method.
		/// </summary>
		TEST_F(PairingInformation, PairingInformation_SetWaitingForPairing)
		{
			PairingInformation_SetWaitingForPairing();
		}

		/// <summary>
		/// Functional test for:
		///     CPairingInformation::GetPairingRequestTime()
		/// Test for GetPairingRequestTime() method.
		/// </summary>
		TEST_F(PairingInformation, PairingInformation_GetPairingRequestTime)
		{
			PairingInformation_GetPairingRequestTime();
		}

		/// <summary>
		/// Functional test for:
		///     CPairingInformation::SetPairingRequestTime()
		/// Test for SetPairingRequestTime() method.
		/// </summary>
		TEST_F(PairingInformation, PairingInformation_SetPairingRequestTime)
		{
			PairingInformation_SetPairingRequestTime();
		}

		/// <summary>
		/// Functional test for:
		///     CPairingInformation::IsWaitingForRequestCardChange()
		/// Test for IsWaitingForRequestCardChange() method.
		/// </summary>
		TEST_F(PairingInformation, PairingInformation_IsWaitingForRequestCardChange)
		{
			PairingInformation_IsWaitingForRequestCardChange();
		}

		/// <summary>
		/// Functional test for:
		///     CPairingInformation::SetWaitingForRequestCardChange()
		/// Test for SetWaitingForRequestCardChange() method.
		/// </summary>
		TEST_F(PairingInformation, PairingInformation_SetWaitingForRequestCardChange)
		{
			PairingInformation_SetWaitingForRequestCardChange();
		}

		/// <summary>
		/// Functional test for:
		///     CPairingInformation::GetRequestCardChangeTime()
		/// Test for GetRequestCardChangeTime() method.
		/// </summary>
		TEST_F(PairingInformation, PairingInformation_GetRequestCardChangeTime)
		{
			PairingInformation_GetRequestCardChangeTime();
		}

		/// <summary>
		/// Functional test for:
		///     CPairingInformation::SetRequestCardChangeTime()
		/// Test for SetRequestCardChangeTime() method.
		/// </summary>
		TEST_F(PairingInformation, PairingInformation_SetRequestCardChangeTime)
		{
			PairingInformation_SetRequestCardChangeTime();
		}

		/// <summary>
		/// Functional test for:
		///     CPairingInformation::GetCorrelationId()
		/// Test for GetCorrelationId() method.
		/// </summary>
		TEST_F(PairingInformation, PairingInformation_GetCorrelationId)
		{
			PairingInformation_GetCorrelationId();
		}

		/// <summary>
		/// Functional test for:
		///     CPairingInformation::SetCorrelationId()
		/// Test for SetCorrelationId() method.
		/// </summary>
		TEST_F(PairingInformation, PairingInformation_SetCorrelationId)
		{
			PairingInformation_SetCorrelationId();
		}

		/// <summary>
		/// Functional test for:
		///     CPairingInformation::IsFallbackEnabled()
		/// Test for IsFallbackEnabled() method.
		/// </summary>
		TEST_F(PairingInformation, PairingInformation_IsFallbackEnabled)
		{
			PairingInformation_IsFallbackEnabled();
		}

		/// <summary>
		/// Functional test for:
		///     CPairingInformation::SetFallbackEnabled()
		/// Test for SetFallbackEnabled() method.
		/// </summary>
		TEST_F(PairingInformation, PairingInformation_SetFallbackEnabled)
		{
			PairingInformation_SetFallbackEnabled();
		}

		/// <summary>
		/// Functional test for:
		///     CPairingInformation::IsMobileCardedSession()
		/// Test for IsMobileCardedSession() method.
		/// </summary>
		TEST_F(PairingInformation, PairingInformation_IsMobileCardedSession)
		{
			PairingInformation_IsMobileCardedSession();
		}

		/// <summary>
		/// Functional test for:
		///     CPairingInformation::SetMobileCardedSession()
		/// Test for SetMobileCardedSession() method.
		/// </summary>
		TEST_F(PairingInformation, PairingInformation_SetMobileCardedSession)
		{
			PairingInformation_SetMobileCardedSession();
		}

		/// <summary>
		/// Functional test for:
		///     CPairingInformation::CheckMobilePairingTimeOut()
		/// Test for CheckMobilePairingTimeOut() method.
		/// </summary>
		TEST_F(PairingInformation, PairingInformation_CheckMobilePairingTimeOut_NotWaiting)
		{
			PairingInformation_CheckMobilePairingTimeOut_NotWaiting();
		}

		/// <summary>
		/// Functional test for:
		///     CPairingInformation::CheckMobilePairingTimeOut()
		/// Test for CheckMobilePairingTimeOut() method.
		/// </summary>
		TEST_F(PairingInformation, PairingInformation_CheckMobilePairingTimeOut_NotTimeout)
		{
			PairingInformation_CheckMobilePairingTimeOut_NotTimeout();
		}

		/// <summary>
		/// Functional test for:
		///     CPairingInformation::CheckMobilePairingTimeOut()
		/// Test for CheckMobilePairingTimeOut() method.
		/// </summary>
		TEST_F(PairingInformation, PairingInformation_CheckMobilePairingTimeOut_Timeout)
		{
			PairingInformation_CheckMobilePairingTimeOut_Timeout();
		}

		/// <summary>
		/// Functional test for:
		///     CPairingInformation::CheckMobileRequestCardChangeTimeOut()
		/// Test for CheckMobileRequestCardChangeTimeOut() method.
		/// </summary>
		TEST_F(PairingInformation, PairingInformation_CheckMobileRequestCardChangeTimeOut_NotWaiting)
		{
			PairingInformation_CheckMobileRequestCardChangeTimeOut_NotWaiting();
		}

		/// <summary>
		/// Functional test for:
		///     CPairingInformation::CheckMobileRequestCardChangeTimeOut()
		/// Test for CheckMobileRequestCardChangeTimeOut() method.
		/// </summary>
		TEST_F(PairingInformation, PairingInformation_CheckMobileRequestCardChangeTimeOut_NotTimeout)
		{
			PairingInformation_CheckMobileRequestCardChangeTimeOut_NotTimeout();
		}

		/// <summary>
		/// Functional test for:
		///     CPairingInformation::CheckMobileRequestCardChangeTimeOut()
		/// Test for CheckMobileRequestCardChangeTimeOut() method.
		/// </summary>
		TEST_F(PairingInformation, PairingInformation_CheckMobileRequestCardChangeTimeOut_Timeout)
		{
			PairingInformation_CheckMobileRequestCardChangeTimeOut_Timeout();
		}

		/// <summary>
		/// Functional test for:
		///     CPairingInformation::HandlePairingEvent()
		/// Test for HandlePairingEvent() method for ShortCode success scenario.
		/// </summary>
		TEST_F(PairingInformation, PairingInformation_HandlePairingEvent_ShortCode_Success)
		{
			PairingInformation_HandlePairingEvent_ShortCode_Success();
		}

		/// <summary>
		/// Functional test for:
		///     CPairingInformation::HandlePairingEvent()
		/// Test for HandlePairingEvent() method for ShortCode event failure scenario.
		/// </summary>
		TEST_F(PairingInformation, PairingInformation_HandlePairingEvent_ShortCode_EventFailure)
		{
			PairingInformation_HandlePairingEvent_ShortCode_EventFailure();
		}

		/// <summary>
		/// Functional test for:
		///     CPairingInformation::HandlePairingEvent()
		/// Test for HandlePairingEvent() method for Beacon success scenario.
		/// </summary>
		TEST_F(PairingInformation, PairingInformation_HandlePairingEvent_Beacon_Success)
		{
			PairingInformation_HandlePairingEvent_Beacon_Success();
		}

		/// <summary>
		/// Functional test for:
		///     CPairingInformation::HandlePairingEvent()
		/// Test for HandlePairingEvent() method for Beacon failure scenario.
		/// </summary>
		TEST_F(PairingInformation, PairingInformation_HandlePairingEvent_Beacon_Failure)
		{
			PairingInformation_HandlePairingEvent_Beacon_Failure();
		}

		/// <summary>
		/// Functional test for:
		///     CPairingInformation::HandlePairingEvent()
		/// Test for HandlePairingEvent() method for Beacon event failure scenario.
		/// </summary>
		TEST_F(PairingInformation, PairingInformation_HandlePairingEvent_Beacon_EventFailure)
		{
			PairingInformation_HandlePairingEvent_Beacon_EventFailure();
		}

		/// <summary>
		/// Functional test for:
		///     CPairingInformation::HandlePairingEvent()
		/// Test for HandlePairingEvent() method for pairing type mismatch.
		/// </summary>
		TEST_F(PairingInformation, PairingInformation_HandlePairingEvent_PairingTypeMismatch)
		{
			PairingInformation_HandlePairingEvent_PairingTypeMismatch();
		}

// #pragma endregion CPairingInformation
}
