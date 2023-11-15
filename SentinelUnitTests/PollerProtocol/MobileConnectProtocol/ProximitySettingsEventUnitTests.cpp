#include "stdafx.h"
#include "RapidJsonTestHelper.h"
#include "PollerProtocol/MobileConnectProtocol/ProximitySettingsEvent.h"
#include "PollerProtocol/MobileConnectProtocol/BeaconManager.h"

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

using namespace std;

class CIBeaconMock : public CBeaconManager
{
public:
	CIBeaconMock() {}
	~CIBeaconMock() {}

	MOCK_METHOD4(SetIBeaconFrameData, bool(const std::string &uuid, uint16_t major, uint16_t minor, uint8_t measuredPower));
};

namespace SentinelNativeUnitTests
{
	class ProximitySettingsEventUnitTests : public ::testing::Test
	{
	protected:
        const rapidjson::Document GetJsonDocument(string jsonString)
        {
			rapidjson::Document jsonDoc;
			if (jsonDoc.Parse(jsonString.c_str()).HasParseError())
				throw JsonException(jsonDoc.GetParseError(), nullptr, "Parse");
			return jsonDoc;
        }

		const std::string PROXIMITY_ID_SET = "ProximityIdSet";
		const std::string BEACON_ERROR = "BeaconError";
	};

	TEST_F(ProximitySettingsEventUnitTests, ParseProximitySettingsEvent)
	{
		static const char* proximitySettingsEventJson =
			R"(
			{
				"messageType": "mobile.event.proximitySettings",
				"deviceType": "Slot",
				"deviceId": "1191",
				"uuid": "4d0bd9a3-2f55-4867-9b85-7f39503fea3a",
				"beaconId": 51,
				"pointOfInterest": 308,
				"transmitPower": 15,
				"transmitFrequency": 160 
			}
		)";

		CProximitySettingsEvent proximitySettingsEvent(GetJsonDocument(proximitySettingsEventJson));

		ASSERT_STREQ(proximitySettingsEvent.GetDeviceType().c_str(), "Slot") << "GetDeviceType";
        ASSERT_STREQ(proximitySettingsEvent.GetDeviceId().c_str(), "1191") << "GetDeviceId";
        ASSERT_STREQ(proximitySettingsEvent.GetUUID().c_str(), "4d0bd9a3-2f55-4867-9b85-7f39503fea3a") << "GetUUID";
		ASSERT_EQ(proximitySettingsEvent.GetBeaconId(), 51) << "GetBeaconId";
        ASSERT_EQ(proximitySettingsEvent.GetPointOfInterest(), 308) << "GetPointOfInterest";
		ASSERT_EQ(proximitySettingsEvent.GetTransmitPower(), 15) << "GetTransmitPower";
        ASSERT_EQ(proximitySettingsEvent.GetTransmitFrequency(), 160) << "GetTransmitFrequency";
	}

	TEST_F(ProximitySettingsEventUnitTests, ProximitySettingsEvent_HandleProximityEvent_Success)
	{
		// Setup.
		static const char *proximitySettingsEventJson =
			R"(
			{
				"messageType": "mobile.event.proximitySettings",
				"deviceType": "Slot",
				"deviceId": "1191",
				"uuid": "4d0bd9a3-2f55-4867-9b85-7f39503fea3a",
				"beaconId": 51,
				"pointOfInterest": 308,
				"transmitPower": 15,
				"transmitFrequency": 160 
			}
		    )";

		CProximitySettingsEvent proximitySettingsEvent(GetJsonDocument(proximitySettingsEventJson));
		bool success = false;
		std::string status;
		std::string UUID = proximitySettingsEvent.GetUUID();

		// remove '-' character from UUID string
		UUID.erase(std::remove(UUID.begin(), UUID.end(), '-'), UUID.end());

		NiceMock<CIBeaconMock> beaconManager;
		ON_CALL(beaconManager, SetIBeaconFrameData(_,_,_,_)).WillByDefault(Return(true));

		success = beaconManager.SetIBeaconFrameData(UUID,
												    proximitySettingsEvent.GetPointOfInterest(),
													proximitySettingsEvent.GetBeaconId(),
												    proximitySettingsEvent.GetTransmitPower());

		status = success ? PROXIMITY_ID_SET : BEACON_ERROR;

		ASSERT_TRUE(success) << "success";
		ASSERT_TRUE(status == PROXIMITY_ID_SET) << "status";
	}

	TEST_F(ProximitySettingsEventUnitTests, ProximitySettingsEvent_HandleProximityEvent_Failure)
	{
		// Setup.
		static const char *proximitySettingsEventJson =
			R"(
			{
				"messageType": "mobile.event.proximitySettings",
				"deviceType": "Slot",
				"deviceId": "1191",
				"uuid": "4d0bd9a3-2f55-4867-9b85-7f39503fea3a",
				"beaconId": 51,
				"pointOfInterest": 308,
				"transmitPower": 15,
				"transmitFrequency": 160 
			}
		    )";

		CProximitySettingsEvent proximitySettingsEvent(GetJsonDocument(proximitySettingsEventJson));
		bool success = false;
		std::string status;
		std::string UUID = proximitySettingsEvent.GetUUID();

		// remove '-' character from UUID string
		UUID.erase(std::remove(UUID.begin(), UUID.end(), '-'), UUID.end());

		NiceMock<CIBeaconMock> beaconManager;
		ON_CALL(beaconManager, SetIBeaconFrameData(_,_,_,_)).WillByDefault(Return(false));

		success = beaconManager.SetIBeaconFrameData(UUID,
												    proximitySettingsEvent.GetPointOfInterest(),
													proximitySettingsEvent.GetBeaconId(),
												    proximitySettingsEvent.GetTransmitPower());

		status = success ? PROXIMITY_ID_SET : BEACON_ERROR;

		ASSERT_FALSE(success) << "success";
		ASSERT_TRUE(status == BEACON_ERROR) << "status";
	}
}