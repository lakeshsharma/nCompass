#include "stdafx.h"
#include "Logging/Logger.h"

// Unit testing includes.
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace std;
using ::testing::_;
using ::testing::Return;
using ::testing::Invoke;

class CSmartDataLoggerMock : public CSmartDataLogger
{
public:
	MOCK_METHOD0(CheckIfDiskSpaceIsAvailableForArchive, void());
};

class LoggerTests : public ::testing::Test {};

TEST_F(LoggerTests, Instance2)
{
	CSmartDataLogger *logger(&CSmartDataLogger::Instance());
	ASSERT_EQ((void *)logger, (void *)&CSmartDataLogger::Instance()) << L"Instance";
	CSmartDataLogger::CleanupInstance();
}

TEST_F(LoggerTests, DefaultRemoteDebuggingFlagTest)
{
	CSmartDataLogger *logger(&CSmartDataLogger::Instance());
	ASSERT_FALSE(logger->m_remoteLoggingEnabled);
	CSmartDataLogger::CleanupInstance();
}

TEST_F(LoggerTests, SetRemoteLoggingEnabledFlagTest)
{
	CSmartDataLogger *logger(&CSmartDataLogger::Instance());
	logger->SetRemoteLoggingEnabledFlag(true);
	ASSERT_TRUE(logger->m_remoteLoggingEnabled);
	CSmartDataLogger::CleanupInstance();
}

TEST_F(LoggerTests, DefaultGetSubscriptionDataResponseTest)
{
	CSmartDataLogger *logger(&CSmartDataLogger::Instance());
	logger->SetRemoteLoggingEnabledFlag(true);
	ASSERT_TRUE(logger->m_remoteLoggingEnabled);

	ASSERT_TRUE(logger->GetSubscriptionDataResponse("ArchiveLogsProgress", "ALL") == "");
	CSmartDataLogger::CleanupInstance();
}

TEST_F(LoggerTests, ArchiveLogsTest)
{
	string expected1percentSubscriptionResponse("{\"InProgress\":true,\"Status\":\"\",\"Percentage\":0,\"Message\":\"ARCHIVING_LOGS\"}");
	string expected50percentSubscriptionResponse("{\"InProgress\":true,\"Status\":\"\",\"Percentage\":50,\"Message\":\"ARCHIVING_LOGS\"}");

	CSmartDataLoggerMock* loggerMock(new CSmartDataLoggerMock());

	//Setup
	loggerMock->m_mediaDiskExists = true;
	loggerMock->m_diskSpaceAvailable = true;
	loggerMock->m_remoteLoggingEnabled = true;
	CSmartDataLogger::m_instance = loggerMock;
	string dataResponse(loggerMock->GetSubscriptionDataResponse("ArchiveLogsProgress", "ALL"));
	ASSERT_TRUE(dataResponse == "");
	
	loggerMock->ArchiveLogs("Test");
	loggerMock->m_vProgressData.push(tProgressData(true, 50, "ARCHIVING_LOGS", ""));
	dataResponse = loggerMock->GetSubscriptionDataResponse("ArchiveLogsProgress", "ALL");
	ASSERT_TRUE(dataResponse == expected1percentSubscriptionResponse);
	
	dataResponse = loggerMock->GetSubscriptionDataResponse("ArchiveLogsProgress", "ALL");
	ASSERT_TRUE(dataResponse == expected50percentSubscriptionResponse);

	dataResponse = loggerMock->GetSubscriptionDataResponse("ArchiveLogsProgress", "ALL");
	ASSERT_TRUE(dataResponse == "");

	delete loggerMock;
}

TEST_F(LoggerTests, WriteLogsToMediaDisk_SuccessTest)
{
	CSmartDataLoggerMock* loggerMock(new CSmartDataLoggerMock());
	CSmartDataLogger::m_instance = loggerMock;

	//Setup
	loggerMock->m_mediaDiskExists = true;
	loggerMock->m_diskSpaceAvailable = true;
	loggerMock->m_remoteLoggingEnabled = true;

	loggerMock->m_zipLogActionType = ZipLogActionType::None;
	TiltCode actualTiltCode = loggerMock->WriteLogsToMediaDisk(ZipLogActionType::Archive, "", "Test");
	ASSERT_TRUE(tltUnknown == actualTiltCode);

	loggerMock->m_zipLogActionType = ZipLogActionType::None;
	actualTiltCode = loggerMock->WriteLogsToMediaDisk(ZipLogActionType::AutoArchive, "", "Test");
	ASSERT_TRUE(tltUnknown == actualTiltCode);

	loggerMock->m_zipLogActionType = ZipLogActionType::None;
	actualTiltCode = loggerMock->WriteLogsToMediaDisk(ZipLogActionType::Shutdown, "", "Test");
	ASSERT_TRUE(tltUnknown == actualTiltCode);

	delete loggerMock;
}

TEST_F(LoggerTests, WriteLogsToMediaDisk_ArchivingLogsDeniedAlreadyInProgress)
{
	CSmartDataLoggerMock* loggerMock(new CSmartDataLoggerMock());
	CSmartDataLogger::m_instance = loggerMock;

	//Setup
	loggerMock->m_mediaDiskExists = true;
	loggerMock->m_diskSpaceAvailable = true;
	loggerMock->m_remoteLoggingEnabled = true;

	loggerMock->m_zipLogActionType = ZipLogActionType::Archive;
	ASSERT_TRUE(tltArchivingLogsDeniedAlreadyInProgress == loggerMock->WriteLogsToMediaDisk(ZipLogActionType::Archive, "", "Test"));

	delete loggerMock;
}

TEST_F(LoggerTests, WriteLogsToMediaDisk_ArchivingLogsDeniedRemoteDebuggingDisabled)
{
	CSmartDataLoggerMock* loggerMock(new CSmartDataLoggerMock());
	CSmartDataLogger::m_instance = loggerMock;

	//Setup
	loggerMock->m_mediaDiskExists = true;
	loggerMock->m_diskSpaceAvailable = true;
	loggerMock->m_remoteLoggingEnabled = false;
	loggerMock->m_manualLoggingInProgress = false;

	loggerMock->m_zipLogActionType = ZipLogActionType::None;
	ASSERT_TRUE(tltArchivingLogsDeniedRemoteDebuggingDisabled == loggerMock->WriteLogsToMediaDisk(ZipLogActionType::Archive, "", "Test"));

	delete loggerMock;
}

TEST_F(LoggerTests, WriteLogsToMediaDisk_ArchivingLogsFailedNoDisk)
{
	CSmartDataLoggerMock* loggerMock(new CSmartDataLoggerMock());
	CSmartDataLogger::m_instance = loggerMock;

	//Setup
	loggerMock->m_mediaDiskExists = false;
	loggerMock->m_diskSpaceAvailable = true;
	loggerMock->m_remoteLoggingEnabled = true;
	loggerMock->m_manualLoggingInProgress = true;

	loggerMock->m_zipLogActionType = ZipLogActionType::None;
	ASSERT_TRUE(tltArchivingLogsFailedNoDisk == loggerMock->WriteLogsToMediaDisk(ZipLogActionType::Archive, "", "Test"));

	delete loggerMock;
}

TEST_F(LoggerTests, WriteLogsToMediaDisk_ArchivingLogsFailedNoDiskSpace)
{
	CSmartDataLoggerMock* loggerMock(new CSmartDataLoggerMock());
	CSmartDataLogger::m_instance = loggerMock;

	//Setup
	loggerMock->m_mediaDiskExists = true;
	loggerMock->m_diskSpaceAvailable = false;
	loggerMock->m_remoteLoggingEnabled = true;
	loggerMock->m_manualLoggingInProgress = true;

	loggerMock->m_zipLogActionType = ZipLogActionType::None;
	TiltCode actualTiltCode = loggerMock->WriteLogsToMediaDisk(ZipLogActionType::Archive, "", "Test");
	ASSERT_TRUE(tltArchivingLogsFailedNoDiskSpace == actualTiltCode);

	delete loggerMock;
}