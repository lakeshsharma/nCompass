#include "gtest/gtest.h"
#include "stdafx.h"
#include "../Sentinel/ConfigFile.h"
#include "../Sentinel/Utilities.h"
#include "../Sentinel/DirectoryManager.h"
#include "ConfigFileReader.h"

// Unit testing includes.
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace std;

class ConfigFileReaderUnitTests : public ::testing::Test
{
protected:
	virtual void SetUp()
	{
	}

	virtual void TearDown()
	{
	}

public:
	void ReadConfig_Valid()
	{
		//setup
		string configPath = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_SENTINELDATA) + "/config.txt";
		ofstream outputStream(configPath);
		ASSERT_TRUE(outputStream.good()) << "unable to create config.txt file";

		// write to config.txt file
		outputStream << "Cache Nudge Address: <239.10.10.7>\n";
		outputStream << "Cache Nudge Port: <10551>\n";
		outputStream << "Cache IP Announcement Address: <239.10.10.6>\n";
		outputStream << "Cache IP Announcement Port: <10550>\n";
		outputStream.close();

		// perform operation
		CConfigFileReader obj;

		// verify
		ASSERT_TRUE(obj.ConfigFileExists()) << "Config file not found.";
		ASSERT_TRUE(obj.GetNudgeMulticastIP().GetIP() == "239.10.10.7") << "Nudge IP";
		ASSERT_TRUE(obj.GetNudgeMulticastPort() == 10551) << "Nudge Port";
		ASSERT_TRUE(obj.GetCacheIPMulticastIP().GetIP() == "239.10.10.6") << "Cache IP";
		ASSERT_TRUE(obj.GetCacheIPMulticastPort() == 10550) << "Cache Port";
	}

	void ReadConfig_Invalid()
	{
		//setup
		string configPath = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_SENTINELDATA) + "/config.txt";
		ofstream outputStream(configPath);
		ASSERT_TRUE(outputStream.good()) << "unable to create config.txt file";

		// write incomplete data to config.txt file.
		outputStream << "Cache Nudge Address: <239.10.10.7>\n";
		outputStream << "Cache Nudge Port: <10551>\n";
		outputStream.close();

		// perform operation
		CConfigFileReader obj;

		// verify
		ASSERT_FALSE(obj.ConfigFileExists()) << "Config file found.";
	}
};

/// <summary>
/// Functional test for reading a valid SpeedMedia config file via CConfigFileReader instance.
/// </summary>
TEST_F(ConfigFileReaderUnitTests, ReadConfig_Valid)
{
	ReadConfig_Valid();
}

/// <summary>
/// Functional test for reading an invalid SpeedMedia config file via CConfigFileReader instance.
/// </summary>
TEST_F(ConfigFileReaderUnitTests, ReadConfig_Invalid)
{
	ReadConfig_Invalid();
}
