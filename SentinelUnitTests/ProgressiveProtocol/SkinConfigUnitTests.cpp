#include "stdafx.h"
#include "ProgressiveProtocol/SkinConfig.h"
#include "Utilities.h"

// Unit testing includes.
#include "UnitTestingUtils.h"

#include "gtest/gtest.h"

const std::string iCardConfiguration("{\"AssetNumber\": \"29063\",\"DeviceAddress\": \"@device.ICARD.206FEC0067CD\",\"DeviceConfigVersion\": 30,\"DeviceId\": 6,\"OneLinkSystemId\": \"b8476a15-66b8-4688-befd-431bcb919161\",\"ProgressiveGroupId\": 0,\"ProtocolType\": \"sas\",\"SourceAsset\": \"\",\"SourceDeviceConfigVersion\": 0,\"SourceDeviceId\": 1,\"TimeZone\": \"America/Los_Angeles\",\"Timestamp\": 1648587816932,\"SkinId\": 236,\"SkinRevision\": 5}");

class SkinConfigTests : public ::testing::Test
{
};

TEST_F(SkinConfigTests, SkinConfigTest)
{
	// expected data
	DWORD revision = 5;
	DWORD id = 236;

	// setup
	rapidjson::Document jsonDoc;
	jsonDoc.Parse(iCardConfiguration.c_str()).HasParseError();

	// perform test
	CSkinConfig obj(jsonDoc);

	// verify
	ASSERT_EQ(obj.GetRevision(), revision) << "Revision is incorrect";
	ASSERT_EQ(obj.GetId(), id) << "Id is incorrect";
}
