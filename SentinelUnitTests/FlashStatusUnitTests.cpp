
#include "stdafx.h"
#include "FlashStatus.h"


#include "gtest/gtest.h"


namespace SentinelNativeUnitTests
{
		TEST(FlashStatusUnitTests, FlashStatusConstructorUnitTest)
		{
			const std::string checksum = "a1b2c3d4";
			const std::string timestamp = "10:30 AM";
			const std::string filename = "os.zip";
			const std::string url = "http://127.10.20.30/os.zip";
			const byte partial = 80;

			CFlashStatus *status = new CFlashStatus(FLASH_SYSTEM, partial, checksum, "", "", "");
			status->SetFilename(filename);
			status->SetTimestamp(timestamp);
			status->SetUrl(url);

			ASSERT_TRUE(status != NULL);
			ASSERT_TRUE(status->GetChecksum() == checksum);
			ASSERT_TRUE(status->GetPercentComplete() == partial);
			ASSERT_TRUE(status->GetType() == FLASH_SYSTEM);
			ASSERT_TRUE(status->GetTimestamp() == timestamp);
			ASSERT_TRUE(status->GetFilename() == filename);
			ASSERT_TRUE(status->GetUrl() == url);
			ASSERT_TRUE(status->GetStatus() == "");

			delete status;
		}

		TEST(FlashStatusUnitTests, FlashStatusCopyConstructor)
		{
			const std::string checksum = "a1b2c3d4";
			const std::string timestamp = "10:30 AM";
			const std::string filename = "os.zip";
			const std::string url = "http://127.10.20.30/os.zip";
			const byte partial = 80;

			CFlashStatus *status = new CFlashStatus(FLASH_SYSTEM, partial, checksum, timestamp, filename, url);
			CFlashStatus *copy = new CFlashStatus(*status);

			ASSERT_TRUE(copy != NULL);
			ASSERT_TRUE(copy->GetChecksum() == checksum);
			ASSERT_TRUE(copy->GetPercentComplete() == partial);
			ASSERT_TRUE(copy->GetType() == FLASH_SYSTEM);
			ASSERT_TRUE(copy->GetTimestamp() == timestamp);
			ASSERT_TRUE(copy->GetFilename() == filename);
			ASSERT_TRUE(copy->GetUrl() == url);
			ASSERT_TRUE(copy->GetStatus() == "");

			delete status;
			delete copy;
		}

		TEST(FlashStatusUnitTests, FlashStatusPercentComplete)
		{
			const std::string checksum = "a1b2c3d4";
			const std::string timestamp = "10:30 AM";
			const std::string filename = "os.zip";
			const std::string url = "http://127.10.20.30/os.zip";
			const byte complete = 100;
			const std::string DOWNLOAD_STATUS_SUCCESS = "success";

			CFlashStatus *status = new CFlashStatus(FLASH_SYSTEM, 0, checksum, timestamp, filename, url);
			status->SetStatus(DOWNLOAD_STATUS_SUCCESS);
			status->SetPercentComplete(complete);

			ASSERT_TRUE(status != NULL);
			ASSERT_TRUE(status->GetChecksum() == checksum);
			ASSERT_TRUE(status->GetPercentComplete() == complete);
			ASSERT_TRUE(status->GetType() == FLASH_SYSTEM);
			ASSERT_TRUE(status->GetTimestamp() == timestamp);
			ASSERT_TRUE(status->GetFilename() == filename);
			ASSERT_TRUE(status->GetUrl() == url);
			ASSERT_TRUE(status->GetStatus() == DOWNLOAD_STATUS_SUCCESS);

			delete status;
		}
}
