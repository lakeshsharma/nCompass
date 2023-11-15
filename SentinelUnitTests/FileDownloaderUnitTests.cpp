#include "stdafx.h"
#include "FileDownloader.h"
#include "gtest/gtest.h"
#include "AssertMsg.h"
#include "TestUtil.h"
#include "Utility/AssertMsg.h"

using namespace std;

namespace SentinelUnitTests
{
	class FileDownloaderTests : public ::testing::Test
	{
	public:

		void AddFile(CFileDownloader& downloader)
		{
			downloader.m_downloadQueue = {};

			downloader.AddFileToDownload("test1", "http://test", CFileDownloader::FILE_TYPE_CAMPAIGN_MEDIA, "1234");
			downloader.AddFileToDownload("test2", "http://test", CFileDownloader::FILE_TYPE_OS, "2345");
			downloader.AddFileToDownload("test3", "http://test", CFileDownloader::FILE_TYPE_EXE, "3456");

			ASSERT_TRUE(downloader.m_downloadQueue.size() == 3);
		}

		void StartStopThread(CFileDownloader& downloader)
		{
			downloader.StartThread(nullptr, nullptr);

			downloader.StopThread();

			ASSERT_TRUE(downloader.m_pThread == nullptr);
		}

		void PurgeFilesToDownload(CFileDownloader& downloader)
		{
			downloader.m_downloadQueue = {};

			downloader.AddFileToDownload("test1", "http://test", CFileDownloader::FILE_TYPE_CAMPAIGN_MEDIA, "1234");
			downloader.AddFileToDownload("test2", "http://test", CFileDownloader::FILE_TYPE_OS, "2345");
			downloader.AddFileToDownload("test3", "http://test", CFileDownloader::FILE_TYPE_EXE, "3456");
			downloader.PurgeFilesToDownload(CFileDownloader::FILE_TYPE_OS);

			ASSERT_TRUE(downloader.m_downloadQueue.size() == 2);
		}

		void GetChecksum1(CFileDownloader& downloader)
		{
			std::string chkSum = downloader.GetChecksum(CFileDownloader::FILE_TYPE_EXE, "test1");
			
			ASSERT_TRUE(chkSum == "");
		}

		void GetChecksum2(CFileDownloader& downloader)
		{
			std::string chkSum = downloader.GetChecksum(CFileDownloader::FILE_TYPE_CAMPAIGN_MEDIA, "test1");

			ASSERT_TRUE(chkSum == "");
		}
	};

	TEST_F(FileDownloaderTests, FileDownloaderAddFile)
	{
		AddFile(CFileDownloader::Instance());
	}

	TEST_F(FileDownloaderTests, FileDownloaderStartStopThread)
	{
		StartStopThread(CFileDownloader::Instance());
	}

	TEST_F(FileDownloaderTests, FileDownloaderPurgeFilesToDownload)
	{
		PurgeFilesToDownload(CFileDownloader::Instance());
	}

	TEST_F(FileDownloaderTests, FileDownloaderGetChecksum1)
	{
		GetChecksum1(CFileDownloader::Instance());
	}

	TEST_F(FileDownloaderTests, FileDownloaderGetChecksum2)
	{
		GetChecksum2(CFileDownloader::Instance());
	}
}
