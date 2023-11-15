#include "stdafx.h"
#include "ProgressiveProtocol/FirmwarePackage.h"
#include "Utilities.h"

// Unit testing includes.
#include "UnitTestingUtils.h"

#include "gtest/gtest.h"

const std::string firmwareDownloadMessage("{\"FirmwareFile\":[{\"FileUrl\":\"http://localhost:8080/onelink/os.fb\",\"FileName\":\"os.fb\",\"Signature\":\"7845ee79fee3f362eeb224ce77301b38161ba65d\",\"Version\":\"1.1.5\",\"Platform\":\"NCOMPASS-PT206\"},{\"FileUrl\":\"http://localhost:8080/onelink/exe.fb\",\"FileName\":\"exe.fb\",\"Signature\":\"b9092beb7962875f8771d78dc0d13db668f96ce4\",\"Version\":\"15.1.0.350\",\"Platform\":\"NCOMPASS-PT206\"}]}");
const std::string assignedFirmwareDownloadMessage("{\"AssignedFirmware\":[{\"FileUrl\":\"http://localhost:8080/onelink/os.fb\",\"FileName\":\"os.fb\",\"Signature\":\"7845ee79fee3f362eeb224ce77301b38161ba65d\",\"Version\":\"1.1.5\",\"Platform\":\"NCOMPASS-PT206\"},{\"FileUrl\":\"http://localhost:8080/onelink/exe.fb\",\"FileName\":\"exe.fb\",\"Signature\":\"b9092beb7962875f8771d78dc0d13db668f96ce4\",\"Version\":\"15.1.0.350\",\"Platform\":\"NCOMPASS-PT206\"}]}");

class FirmwarePackageTests : public ::testing::Test
{
};

TEST_F(FirmwarePackageTests, FirmwarePackageTest)
{
	// expected data
	std::string filename[2] = {"os.fb", "exe.fb"};
	std::string fileUrl[2] = {"http://localhost:8080/onelink/os.fb", "http://localhost:8080/onelink/exe.fb"};
	std::string checksum[2] = { "7845ee79fee3f362eeb224ce77301b38161ba65d", "b9092beb7962875f8771d78dc0d13db668f96ce4" };
	std::string version[2] = { "1.1.5", "15.1.0.350" };
	std::string platform[2] = { "NCOMPASS-PT206", "NCOMPASS-PT206" };

	// setup
    rapidjson::Document jsonDoc;
    jsonDoc.Parse(firmwareDownloadMessage.c_str()).HasParseError();

	// perform test
	CFirmwarePackage obj(jsonDoc);

	// verify
	uint16_t index = 0;
	for (auto &firmwareFile : obj.GetFirmwareFiles())
	{
		ASSERT_EQ(firmwareFile->m_filename, filename[index]) << "Incorrect filename, index = " << index ;
		ASSERT_EQ(firmwareFile->m_downloadURL, fileUrl[index]) << "Incorrect file url, index = " << index ;
		ASSERT_EQ(firmwareFile->m_checksum, checksum[index]) << "Incorrect checksum, index = " << index ;
		ASSERT_EQ(firmwareFile->m_version, version[index]) << "Incorrect version, index = " << index ;
		ASSERT_EQ(firmwareFile->m_platform, platform[index]) << "Incorrect platform, index = " << index ;

		index++;
	}
}

TEST_F(FirmwarePackageTests, Assigned_Firmware_Package_Test)
{
	// expected data
	std::string filename[2] = { "os.fb", "exe.fb" };
	std::string fileUrl[2] = { "http://localhost:8080/onelink/os.fb", "http://localhost:8080/onelink/exe.fb" };
	std::string checksum[2] = { "7845ee79fee3f362eeb224ce77301b38161ba65d", "b9092beb7962875f8771d78dc0d13db668f96ce4" };
	std::string version[2] = { "1.1.5", "15.1.0.350" };
	std::string platform[2] = { "NCOMPASS-PT206", "NCOMPASS-PT206" };

	// setup
	rapidjson::Document jsonDoc;
	jsonDoc.Parse(assignedFirmwareDownloadMessage.c_str()).HasParseError();

	// perform test
	CFirmwarePackage obj(jsonDoc, true);

	// verify
	uint16_t index = 0;
	for (auto& firmwareFile : obj.GetFirmwareFiles())
	{
		ASSERT_EQ(firmwareFile->m_filename, filename[index]) << "Incorrect filename, index = " << index;
		ASSERT_EQ(firmwareFile->m_downloadURL, fileUrl[index]) << "Incorrect file url, index = " << index;
		ASSERT_EQ(firmwareFile->m_checksum, checksum[index]) << "Incorrect checksum, index = " << index;
		ASSERT_EQ(firmwareFile->m_version, version[index]) << "Incorrect version, index = " << index;
		ASSERT_EQ(firmwareFile->m_platform, platform[index]) << "Incorrect platform, index = " << index;

		index++;
	}
}
