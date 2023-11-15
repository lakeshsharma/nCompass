
#include "stdafx.h"
#include "Util/Convert.h"
#include "RapidJsonTestHelper.h"
#include "TestUtil.h"
#include "PollerProtocol/MobileConnectProtocol/MobileConnectMessageFactory.h"
#include "PollerProtocol/MobileConnectProtocol/MobileConnectProtocolKeys.h"
#include "PollerProtocol/MobileConnectProtocol/PairingInformation.h"
#include "PollerProtocol/MobileConnectProtocol/MobilePBTSessionInfo.h"
#include "PollerProtocol/MobileConnectProtocol/PairingSettingsEvent.h"
#include "PollerProtocol/MobileConnectProtocol/PairingEvent.h"

#include "gtest/gtest.h"
using namespace std;

namespace SentinelNativeUnitTests
{
	class MobileConnectMessageParsingUnitTests : public ::testing::Test
	{
	public:
        const rapidjson::Document GetJsonDocument(string jsonString)
        {
			rapidjson::Document jsonDoc;
			if (jsonDoc.Parse(jsonString.c_str()).HasParseError())
				throw JsonException(jsonDoc.GetParseError(), nullptr, "Parse");
			return jsonDoc;
        }
	};

	TEST_F(MobileConnectMessageParsingUnitTests, ParsePairingSettingsEvent_Shortcode)
	{
		static const char* pairingSettingsEventJson =
			R"(
			{
				"messageType": "mobile.event.pairingSettings",
				"deviceType": "Slot",
				"deviceId": "1191",
				"pairingType": "ShortCode",
				"pairingEnabled": true,
				"pairingTimeOutMs": 30000,
				"idlePairingId": "0265934AA13432CD6300",
				"idlePairingChecksum": "B7EC6E696979",
				"confirmCardRequest": false,
				"transmitPower": 33
			}
		)";

		CPairingSettingsEvent pairingSettingsEvent(GetJsonDocument(pairingSettingsEventJson));

		ASSERT_STREQ(pairingSettingsEvent.GetPairingType().c_str(), "ShortCode") << "GetPairingType";
		ASSERT_STREQ(pairingSettingsEvent.GetDeviceId().c_str(), "1191") << "GetDeviceId";
		ASSERT_STREQ(pairingSettingsEvent.GetDeviceType().c_str(), "Slot") << "GetDeviceType";
		ASSERT_EQ(pairingSettingsEvent.IsPairingEnabled(), true) << "IsPairingEnabled";
		ASSERT_EQ(pairingSettingsEvent.GetPairingTimeOutMs(), 30000) << "30000";
		ASSERT_STREQ(pairingSettingsEvent.GetIdlePairingId().c_str(), "0265934AA13432CD6300") << "GetIdlePairingId";
		ASSERT_STREQ(pairingSettingsEvent.GetIdlePairingChecksum().c_str(), "B7EC6E696979") << "GetIdlePairingChecksum";

		// Transmit power supplied is actually measured power in this version
		ASSERT_EQ(pairingSettingsEvent.GetMeasuredPower(), 33) << "GetMeasuredPower";
		ASSERT_EQ(pairingSettingsEvent.GetTransmitPower(), 0xff) << "GetTransmitPower";
	}

	TEST_F(MobileConnectMessageParsingUnitTests, ParsePairingSettingsEvent_Beacon_WithoutMeasuredPower)
	{
		static const char* pairingSettingsEventJson =
			R"(
			{
				"messageType": "mobile.event.pairingSettings",
				"deviceType": "Slot",
				"deviceId": "101",
				"pairingType": "Beacon",
				"pairingEnabled": true,
				"pairingTimeOutMs": 25000,
				"idlePairingId": "01E4115C71F0AAC29000",
				"idlePairingChecksum": "86A2AA75B058",
				"confirmCardRequest": false,
				"transmitPower": 33
			}
		)";

		CPairingSettingsEvent pairingSettingsEvent(GetJsonDocument(pairingSettingsEventJson));

		ASSERT_STREQ(pairingSettingsEvent.GetPairingType().c_str(), "Beacon") << "GetPairingType";
		ASSERT_STREQ(pairingSettingsEvent.GetDeviceId().c_str(), "101") << "GetDeviceId";
		ASSERT_STREQ(pairingSettingsEvent.GetDeviceType().c_str(), "Slot") << "GetDeviceType";
		ASSERT_EQ(pairingSettingsEvent.IsPairingEnabled(), true) << "IsPairingEnabled";
		ASSERT_EQ(pairingSettingsEvent.GetPairingTimeOutMs(), 25000) << "25000";
		ASSERT_STREQ(pairingSettingsEvent.GetIdlePairingId().c_str(), "01E4115C71F0AAC29000") << "GetIdlePairingId";
		ASSERT_STREQ(pairingSettingsEvent.GetIdlePairingChecksum().c_str(), "86A2AA75B058") << "GetIdlePairingChecksum";

		// Transmit power supplied is actually measured power in this version
		ASSERT_EQ(pairingSettingsEvent.GetMeasuredPower(), 33) << "GetMeasuredPower";
		ASSERT_EQ(pairingSettingsEvent.GetTransmitPower(), 0xff) << "GetTransmitPower";
	}
					
	TEST_F(MobileConnectMessageParsingUnitTests, ParsePairingSettingsEvent_Beacon_WithMeasuredPower)
	{
		static const char* pairingSettingsEventJson =
			R"(
			{
				"messageType": "mobile.event.pairingSettings",
				"deviceType": "Slot",
				"deviceId": "101",
				"pairingType": "Beacon",
				"pairingEnabled": true,
				"pairingTimeOutMs": 25000,
				"idlePairingId": "01E4115C71F0AAC29000",
				"idlePairingChecksum": "86A2AA75B058",
				"confirmCardRequest": false,
				"measuredPower": 33,
				"transmitPower": 2
			}
		)";

		CPairingSettingsEvent pairingSettingsEvent(GetJsonDocument(pairingSettingsEventJson));
		ASSERT_STREQ(pairingSettingsEvent.GetDeviceType().c_str(), "Slot") << "GetDeviceType";
		ASSERT_STREQ(pairingSettingsEvent.GetDeviceId().c_str(), "101") << "GetDeviceId";
		ASSERT_STREQ(pairingSettingsEvent.GetPairingType().c_str(), "Beacon") << "GetPairingType";
		ASSERT_EQ(pairingSettingsEvent.IsPairingEnabled(), true) << "IsPairingEnabled";
		ASSERT_EQ(pairingSettingsEvent.GetPairingTimeOutMs(), 25000) << "25000";
		ASSERT_STREQ(pairingSettingsEvent.GetIdlePairingId().c_str(), "01E4115C71F0AAC29000") << "GetIdlePairingId";
		ASSERT_STREQ(pairingSettingsEvent.GetIdlePairingChecksum().c_str(), "86A2AA75B058") << "GetIdlePairingChecksum";
		ASSERT_EQ(pairingSettingsEvent.GetMeasuredPower(), 33) << "GetMeasuredPower";
		ASSERT_EQ(pairingSettingsEvent.GetTransmitPower(), 2) << "GetTransmitPower";
	}

	TEST_F(MobileConnectMessageParsingUnitTests, ParseFundsTransferRequest_ToGame_Cash)
	{
		static const char* fundsTransferRequestJson =
			R"(
			{
				"payload": {
					"messageType": "session.request.fundsTransfer",
					"deviceType": "Slot",
					"deviceId": "133",
					"pin": "9523",
					"fundType": "Cash",
					"transferDirection": "ToGame",
					"amountInPennies": 1000
				},
				"correlationId":"60aa2ba1-57b6-4cdb-b06d-c2068102621c"
			}
		)";

		CFundsTransferRequest fundsTransferRequest(GetJsonDocument(fundsTransferRequestJson));
		ASSERT_STREQ(fundsTransferRequest.GetCorrelationId().c_str(), 
			"60aa2ba1-57b6-4cdb-b06d-c2068102621c") << "GetCorrelationId";
		ASSERT_STREQ(fundsTransferRequest.GetDeviceType().c_str(), "Slot") << "GetDeviceType";
		ASSERT_STREQ(fundsTransferRequest.GetDeviceId().c_str(), "133") << "GetDeviceId";
		ASSERT_STREQ(fundsTransferRequest.GetPin().c_str(), "9523") << "GetPin";
		ASSERT_STREQ(fundsTransferRequest.GetFundType().c_str(), "Cash") << "GetFundType";
		ASSERT_STREQ(fundsTransferRequest.GetTransferDirection().c_str(), "ToGame") << "GetTransferDirection";
		ASSERT_EQ(fundsTransferRequest.GetAmountInPennies(), 1000) << "GetAmountInPennies";
	}

	TEST_F(MobileConnectMessageParsingUnitTests, ParseFundsTransferRequest_ToGame_CashablePromo)
	{
		static const char* fundsTransferRequestJson =
			R"(
			{
				"payload": {
					"messageType": "session.request.fundsTransfer",
					"deviceType": "Slot",
					"deviceId": "133",
					"pin": "9523",
					"fundType": "CashablePromo",
					"transferDirection": "ToGame",
					"amountInPennies": 1000
				},
				"correlationId":"60aa2ba1-57b6-4cdb-b06d-c2068102621c"
			}
		)";

		CFundsTransferRequest fundsTransferRequest(GetJsonDocument(fundsTransferRequestJson));
		ASSERT_STREQ(fundsTransferRequest.GetCorrelationId().c_str(),
			"60aa2ba1-57b6-4cdb-b06d-c2068102621c") << "GetCorrelationId";
		ASSERT_STREQ(fundsTransferRequest.GetDeviceType().c_str(), "Slot") << "GetDeviceType";
		ASSERT_STREQ(fundsTransferRequest.GetDeviceId().c_str(), "133") << "GetDeviceId";
		ASSERT_STREQ(fundsTransferRequest.GetPin().c_str(), "9523") << "GetPin";
		ASSERT_STREQ(fundsTransferRequest.GetFundType().c_str(), "CashablePromo") << "GetFundType";
		ASSERT_STREQ(fundsTransferRequest.GetTransferDirection().c_str(), "ToGame") << "GetTransferDirection";
		ASSERT_EQ(fundsTransferRequest.GetAmountInPennies(), 1000) << "GetAmountInPennies";
	}

	TEST_F(MobileConnectMessageParsingUnitTests, ParseFundsTransferRequest_ToGame_NonCashablePromo)
	{
		static const char* fundsTransferRequestJson =
			R"(
			{
				"payload": {
					"messageType": "session.request.fundsTransfer",
					"deviceType": "Slot",
					"deviceId": "133",
					"pin": "9523",
					"fundType": "NonCashablePromo",
					"transferDirection": "ToGame",
					"amountInPennies": 100
				},
				"correlationId":"60aa2ba1-57b6-4cdb-b06d-c2068102621c"
			}
		)";

		CFundsTransferRequest fundsTransferRequest(GetJsonDocument(fundsTransferRequestJson));
		ASSERT_STREQ(fundsTransferRequest.GetCorrelationId().c_str(),
			"60aa2ba1-57b6-4cdb-b06d-c2068102621c") << "GetCorrelationId";
		ASSERT_STREQ(fundsTransferRequest.GetDeviceType().c_str(), "Slot") << "GetDeviceType";
		ASSERT_STREQ(fundsTransferRequest.GetDeviceId().c_str(), "133") << "GetDeviceId";
		ASSERT_STREQ(fundsTransferRequest.GetPin().c_str(), "9523") << "GetPin";
		ASSERT_STREQ(fundsTransferRequest.GetFundType().c_str(), "NonCashablePromo") << "GetFundType";
		ASSERT_STREQ(fundsTransferRequest.GetTransferDirection().c_str(), "ToGame") << "GetTransferDirection";
		ASSERT_EQ(fundsTransferRequest.GetAmountInPennies(), 100) << "GetAmountInPennies";
	}

	TEST_F(MobileConnectMessageParsingUnitTests, ParseFundsTransferRequest_FromGame_Cash)
	{
		static const char* fundsTransferRequestJson =
			R"(
			{
				"payload": {
					"messageType": "session.request.fundsTransfer",
					"deviceType": "Slot",
					"deviceId": "133",
					"pin": "9523",
					"fundType": "Cash",
					"transferDirection": "FromGame",
					"amountInPennies": 200
				}
			}
		)";

		CFundsTransferRequest fundsTransferRequest(GetJsonDocument(fundsTransferRequestJson));
		ASSERT_STREQ(fundsTransferRequest.GetDeviceType().c_str(), "Slot") << "GetDeviceType";
		ASSERT_STREQ(fundsTransferRequest.GetDeviceId().c_str(), "133") << "GetDeviceId";
		ASSERT_STREQ(fundsTransferRequest.GetPin().c_str(), "9523") << "GetPin";
		ASSERT_STREQ(fundsTransferRequest.GetFundType().c_str(), "Cash") << "GetFundType";
		ASSERT_STREQ(fundsTransferRequest.GetTransferDirection().c_str(), "FromGame") << "GetTransferDirection";
		ASSERT_EQ(fundsTransferRequest.GetAmountInPennies(), 200) << "GetAmountInPennies";
	}

	TEST_F(MobileConnectMessageParsingUnitTests, ParseFundsTransferRequest_FromGame_NonCashablePromo)
	{
		static const char* fundsTransferRequestJson =
			R"(
			{
				"payload": {
					"messageType": "session.request.fundsTransfer",
					"deviceType": "Slot",
					"deviceId": "133",
					"pin": "9523",
					"fundType": "NonCashablePromo",
					"transferDirection": "FromGame",
					"amountInPennies": 100
				}
			}
		)";

		CFundsTransferRequest fundsTransferRequest(GetJsonDocument(fundsTransferRequestJson));
		ASSERT_STREQ(fundsTransferRequest.GetDeviceType().c_str(), "Slot") << "GetDeviceType";
		ASSERT_STREQ(fundsTransferRequest.GetDeviceId().c_str(), "133") << "GetDeviceId";
		ASSERT_STREQ(fundsTransferRequest.GetPin().c_str(), "9523") << "GetPin";
		ASSERT_STREQ(fundsTransferRequest.GetFundType().c_str(), "NonCashablePromo") << "GetFundType";
		ASSERT_STREQ(fundsTransferRequest.GetTransferDirection().c_str(), "FromGame") << "GetTransferDirection";

		// Amount passed in Promos From Game request is ignored and set to 0 so that 
		// 'all' promos can be taken out from game to the PBT account
		ASSERT_EQ(fundsTransferRequest.GetAmountInPennies(), 0) << "GetAmountInPennies";
	}

	TEST_F(MobileConnectMessageParsingUnitTests, ParsePairingEvent_Beacon)
	{
		static const char* pairingEventJson =
			R"(
			{
				"messageType": "mobile.event.pairing",
				"deviceType": "Slot",
				"deviceId": "101",
				"pairingType": "Beacon",
				"pairingId": "01A847A9854D12B71401",
				"pairingChecksum": "A832E896AF33",
				"success": true,
				"status":""
			}
		)";

		CPairingEvent pairingEvent(GetJsonDocument(pairingEventJson));
		ASSERT_STREQ(pairingEvent.GetDeviceType().c_str(), "Slot") << "GetDeviceType";
		ASSERT_STREQ(pairingEvent.GeDeviceId().c_str(), "101") << "GeDeviceId";
		ASSERT_STREQ(pairingEvent.GetPairingType().c_str(), "Beacon") << "GetPairingType";
		ASSERT_STREQ(pairingEvent.GetPairingId().c_str(), "01A847A9854D12B71401") << "GetPairingId";
		ASSERT_STREQ(pairingEvent.GetPairingChecksum().c_str(), "A832E896AF33") << "GetPairingChecksum";
		ASSERT_TRUE(pairingEvent.GetIsSuccess()) << "GetIsSuccess";
		ASSERT_STREQ(pairingEvent.GetStatus().c_str(), "") << "GetStatus";
	}

	TEST_F(MobileConnectMessageParsingUnitTests, ParsePairingEvent_ShortCode)
	{
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
		ASSERT_STREQ(pairingEvent.GetDeviceType().c_str(), "Slot") << "GetDeviceType";
		ASSERT_STREQ(pairingEvent.GeDeviceId().c_str(), "1191") << "GeDeviceId";
		ASSERT_STREQ(pairingEvent.GetPairingType().c_str(), "ShortCode") << "GetPairingType";
		ASSERT_STREQ(pairingEvent.GetPairingId().c_str(), "7066") << "GetPairingId";
		ASSERT_STREQ(pairingEvent.GetPairingChecksum().c_str(), "201CAE90371E") << "GetPairingChecksum";
		ASSERT_TRUE(pairingEvent.GetIsSuccess()) << "GetIsSuccess";
		ASSERT_STREQ(pairingEvent.GetStatus().c_str(), "") << "GetStatus";
	}

	TEST_F(MobileConnectMessageParsingUnitTests, ParseCardChangeRequest_Insert)
	{
		static const char* cardChangeRequestJson =
			R"(
			{
				"payload": {
					"messageType": "session.request.cardChange",
					"cardNumber": "1142000014",
					"deviceType": "Slot",
					"deviceId": "101",
					"changeType":"Insert"
				}
			}
		)";

		CCardChangeRequest cardChangeRequest(GetJsonDocument(cardChangeRequestJson));
		ASSERT_STREQ(cardChangeRequest.GetDeviceType().c_str(), "Slot") << "GetDeviceType";
		ASSERT_STREQ(cardChangeRequest.GetDeviceId().c_str(), "101") << "GetDeviceId";
		ASSERT_STREQ(cardChangeRequest.GetCardNumber().c_str(), "1142000014") << "GetCardNumber";
		ASSERT_STREQ(cardChangeRequest.GetChangeType().c_str(), "Insert") << "GetChangeType";
	}

	TEST_F(MobileConnectMessageParsingUnitTests, ParseCardChangeRequest_Remove)
	{
		static const char* cardChangeRequestJson =
			R"(
			{
				"payload": {
					"messageType": "session.request.cardChange",
					"cardNumber": "1142000014",
					"deviceType": "Slot",
					"deviceId": "101",
					"changeType":"Remove"
				}
			}
		)";

		CCardChangeRequest cardChangeRequest(GetJsonDocument(cardChangeRequestJson));
		ASSERT_STREQ(cardChangeRequest.GetDeviceType().c_str(), "Slot") << "GetDeviceType";
		ASSERT_STREQ(cardChangeRequest.GetDeviceId().c_str(), "101") << "GetDeviceId";
		ASSERT_STREQ(cardChangeRequest.GetCardNumber().c_str(), "1142000014") << "GetCardNumber";
		ASSERT_STREQ(cardChangeRequest.GetChangeType().c_str(), "Remove") << "GetChangeType";
	}

	TEST_F(MobileConnectMessageParsingUnitTests, ParseFundsTransferBalanceRequest)
	{
		static const char* fundsTransferBalanceRequestJson =
			R"(
			{
				"payloadVersion":"1.0",
				"payload": {
					"messageType": "session.request.fundsTransferBalance",
					"deviceType": "Slot",
					"deviceId": "101",
					"pin":"8632"
				}
			}
		)";

		CFundsTransferBalanceRequest fundsTransferBalanceRequest(GetJsonDocument(fundsTransferBalanceRequestJson));
		ASSERT_STREQ(fundsTransferBalanceRequest.GetDeviceType().c_str(), "Slot") << "GetDeviceType";
		ASSERT_STREQ(fundsTransferBalanceRequest.GetDeviceId().c_str(), "101") << "GetDeviceId";
		ASSERT_STREQ(fundsTransferBalanceRequest.GetPin().c_str(), "8632") << "GetPin";
		ASSERT_STREQ(fundsTransferBalanceRequest.GetPayloadVersion().c_str(), "1.0") << "GetPayloadVersion";
	}
}