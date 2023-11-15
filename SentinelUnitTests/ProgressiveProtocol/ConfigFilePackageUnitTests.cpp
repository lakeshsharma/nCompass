#include "stdafx.h"
#include "ProgressiveProtocol/ConfigFilePackage.h"
#include "Utilities.h"

// Unit testing includes.
#include "UnitTestingUtils.h"

#include "gtest/gtest.h"

const std::string configFileDownloadMessage("{\"FileUrl\":\"http://localhost:8080/onelink/config.zip\",\"FileName\":\"config.zip\",\"MD5\":\"055e2d4c22a68f7d6316e89e2eff8be5\",\"MessageId\":\"30\"}");

class ConfigFilePackageTests : public ::testing::Test
{
};

TEST_F(ConfigFilePackageTests, ConfigFilePackageTest)
{
	// expected data
	std::string filename("config.zip");
	std::string fileUrl("http://localhost:8080/onelink/config.zip");
	std::string checksum("055e2d4c22a68f7d6316e89e2eff8be5");
	std::string messageId("30");

	// setup
	rapidjson::Document jsonDoc;
	jsonDoc.Parse(configFileDownloadMessage.c_str()).HasParseError();

	// perform test
	CConfigFilePackage obj(jsonDoc);

	// verify
	ASSERT_EQ(obj.GetFilename(), filename) << "Filename is incorrect";
	ASSERT_EQ(obj.GetUrl(), fileUrl) << "File URL is incorrect";
	ASSERT_EQ(obj.GetChecksum(), checksum) << "Checksum is incorrect";
	ASSERT_EQ(obj.GetMessageId(), messageId) << "MessageId is incorrect";
}
