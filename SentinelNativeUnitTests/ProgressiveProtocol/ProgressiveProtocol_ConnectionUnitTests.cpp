#if 000//TODO-PORT
#include "stdafx.h"
#include "ProgressiveProtocol/ProgressiveProtocol.h"

// Sentinel includes.
#include "InternalMessage.h"

// Unit testing includes.
#include "gmock/gmock.h"
#include "ProgressiveConnectionMock.h"
#include "UnitTestingOperatingSystem.h"
#include "UnitTestingToString.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using ::testing::_;
using ::testing::InSequence;
using ::testing::Invoke;
using ::testing::Mock;

namespace SentinelNativeUnitTests
{
	MATCHER_P2(CheckMessage, messageTypeExpected, dwordValueExpected, "")
	{
		bool matches(messageTypeExpected == arg->GetMessageType() && dwordValueExpected == arg->GetDWordData());
		return matches;
	}

	TEST_CLASS(ProgressiveProtocol_Connection)
	{
		static const CString m_testThreadName;

		static CString m_gmockFailure;
		static const LPCTSTR m_progConfigFileName;
		static const CFile::OpenFlags m_progConfigOpenFlags = CFile::modeRead;
		static const char m_progConfigFileContent_Active[]; // For fileName.
		static const char m_progConfigFileContent_NotActive[]; // For fileName.

		class CTersePrinter : public ::testing::EmptyTestEventListener {
			// Called after a failed assertion or a SUCCEED() invocation.
			virtual void OnTestPartResult(const ::testing::TestPartResult& test_part_result) {
				if (test_part_result.failed())
				{
					if (!m_gmockFailure.IsEmpty())
					{
						m_gmockFailure += "\n---------------------------\n";
					}

					CStringA failure;
					failure.Format("FAILED in %s:%d\n%s",
						test_part_result.file_name(),
						test_part_result.line_number(),
						test_part_result.summary());
					m_gmockFailure += failure;
				}
			}
		};

		static CTersePrinter m_tersePrinter;

		/// <summary>
		/// Initialize and/or test static class members.
		/// </summary>
		TEST_CLASS_INITIALIZE(ProgressiveConnectionUnitTests2_Init)
		{
			// Class initialization code.
			CDirectoryManager::MockInstance(NULL);
			::testing::UnitTest::GetInstance()->listeners().Append(&m_tersePrinter);
		}

		/// <summary>
		/// Cleanup static class member(s).
		/// </summary>
		TEST_CLASS_CLEANUP(ProgressiveConnectionUnitTests2_Deinit)
		{
			// Class cleanup code.
			CDirectoryManager::MockInstanceCleanup();
			::testing::UnitTest::GetInstance()->listeners().Release(&m_tersePrinter);
		}

		/// <summary>
		/// Initialize class member(s) before each test method is run.
		/// </summary>
		TEST_METHOD_INITIALIZE(Initialize)
		{
			// Test method initialization code.
			m_gmockFailure.Empty();
		}

		/// <summary>
		/// Cleanup class member(s) before each test method is run.
		/// </summary>
		TEST_METHOD_CLEANUP(Cleanup)
		{
			// Report a gmock failure reported by our event listener, if one occurred.
			// With VS 2010, this hides any CppUnitTestFramework "Assert" failures.
			// - Use EXPECT_TRUE, EXPECT_EQ, etc., inside of "Assert:IsTrue", Assert::AreEqual" within test methods.
			if (!m_gmockFailure.IsEmpty())
			{
				Assert::Fail(m_gmockFailure);
			}
		}

		class CProtocol : public CProgressiveProtocol
		{
		public:
			CProtocol(CQueueList &scfToInternalsQueue, IOperatingSystem &operatingSystem = COperatingSystem::m_operatingSystem)
				:
				CProgressiveProtocol(scfToInternalsQueue, operatingSystem)
			{
			}

			MOCK_METHOD4(GetProgressiveConnection,
				CProgressiveConnection *(CProgressiveDataConsumer &dataConsumer,
					CProgressiveConfig *progConfig,
					const CString &paltronicsCertificatePathname,
					IOperatingSystem &operatingSystem));

			MOCK_METHOD0(RefreshConnection, void());
			MOCK_METHOD0(LoadConfigFile, CProgressiveConfig *());

			virtual CProgressiveConnection *MockGetProgressiveConnection(CProgressiveDataConsumer &dataConsumer,
				CProgressiveConfig *progConfig,
				const CString &paltronicsCertificatePathname,
				IOperatingSystem &operatingSystem = COperatingSystem::m_operatingSystem)
			{
				return new CProgressiveConnectionMock(dataConsumer, progConfig, paltronicsCertificatePathname, operatingSystem);
			}

			virtual CProgressiveConnection *BaseGetProgressiveConnection(CProgressiveDataConsumer &dataConsumer,
				CProgressiveConfig *progConfig,
				const CString &paltronicsCertificatePathname,
				IOperatingSystem &operatingSystem = COperatingSystem::m_operatingSystem)
			{
				return CProgressiveProtocol::GetProgressiveConnection(dataConsumer, progConfig, paltronicsCertificatePathname, operatingSystem);
			}

			virtual CProgressiveConfig *BaseLoadConfigFile()
			{
				return CProgressiveProtocol::LoadConfigFile();
			}

			virtual void BaseRefreshConnection()
			{
				return CProgressiveProtocol::RefreshConnection();
			}
		};

		void AssertRemoveMessage(CQueueList &scfToInternalsQueue, InternalMsgType msgType, int msgPriority, LPCSTR description)
		{
			CQueueItem *item = scfToInternalsQueue.Remove();
			EXPECT_TRUE(nullptr != item) << "item " << description;

			unique_ptr<CInternalMessage> msg(dynamic_cast<CInternalMessage *>(item));
			EXPECT_TRUE(nullptr != msg.get()) << "msg " << description;
			EXPECT_EQ(msgPriority, msg->GetPriority()) << "GetPriority " << description;
			EXPECT_EQ(msgType, msg->GetMessageType()) << "GetMessageType() " << description;
		}

		void AssertRemoveMessage(CQueueList &scfToInternalsQueue,
			InternalMsgType msgType,
			int msgPriority,
			LPCSTR description,
			unique_ptr<CInternalMessage> &msg)
		{
			CQueueItem *item = scfToInternalsQueue.Remove();
			EXPECT_TRUE(nullptr != item) << "item " << description;

			msg.reset(dynamic_cast<CInternalMessage *>(item));
			EXPECT_TRUE(nullptr != msg.get()) << "msg " << description;
			EXPECT_EQ(msgPriority, msg->GetPriority()) << "GetPriority " << description;
			EXPECT_EQ(msgType, msg->GetMessageType()) << "GetMessageType() " << description;
		}

		void AssertRemoveMessage_SetProgressiveConfig(CQueueList &scfToInternalsQueue, bool active, LPCSTR description)
		{
			std::unique_ptr<CInternalMessage> msg;
			AssertRemoveMessage(scfToInternalsQueue, INT_MSG_SET_PROGRESSIVE_CONFIG, QUEUE_PRIORITY_NORMAL, description, msg);

			const CProgressiveConfig* progressiveConfig(msg->GetProgressiveConfig());
			EXPECT_TRUE(nullptr != progressiveConfig) << "progressiveConfig " << description;
			EXPECT_EQ(active, progressiveConfig->GetActive()) << "GetActive " << description;
		}

		void AssertRemoveMessage_SetProgressiveHost(CQueueList &scfToInternalsQueue, LPCSTR description)
		{
			const std::string hostName("");
			const DWORD hostPort(0);

			std::unique_ptr<CInternalMessage> msg;
			AssertRemoveMessage(scfToInternalsQueue, INT_MSG_SET_PROGRESSIVE_HOST, QUEUE_PRIORITY_NORMAL, description, msg);

			CProgressiveHost *progressiveHost(msg->GetProgressiveHost());
			EXPECT_TRUE(nullptr != progressiveHost) << "progressiveHost " << description;
			EXPECT_EQ(hostName, progressiveHost->GetName()) << "GetName " << description;
			EXPECT_EQ(hostPort, progressiveHost->GetHostPort()) << "GetHostPort " << description;
		}

		void AssertRemoveMessage_SetProgressiveConnectionInfo(CQueueList &scfToInternalsQueue, ProgressiveConnectionState connectionState, LPCSTR description)
		{
			std::unique_ptr<CInternalMessage> msg;
			AssertRemoveMessage(scfToInternalsQueue, INT_MSG_SET_PROGRESSIVE_CONNECTION_INFO, QUEUE_PRIORITY_NORMAL, description, msg);
			CProgressiveConnectionInfo *connectionInfo(msg->GetProgressiveConnectionInfo());
			EXPECT_TRUE(nullptr != connectionInfo) << "connectionInfo " << description;
			EXPECT_EQ(connectionState, connectionInfo->GetState()) << "GetState " << description;
		}

		void AssertRemoveMessage_GetProgressiveGamesInfo(CQueueList &scfToInternalsQueue, LPCSTR description)
		{
			AssertRemoveMessage(scfToInternalsQueue, INT_MSG_GET_PROGRESSIVE_GAMES_INFO, QUEUE_PRIORITY_NORMAL, description);
		}

		void AssertRemoveMessage_ProgressiveExternalCommunicationsStatus(CQueueList &scfToInternalsQueue, LPCSTR description)
		{
			AssertRemoveMessage(scfToInternalsQueue, INT_MSG_PROGRESSIVE_EXTERNAL_COMMUNICATIONS_STATUS, QUEUE_PRIORITY_NORMAL, description);
		}

		// Remove all messages and assert that they are the correct intial message(s).
		void AssertRemoveInitialMessages(CQueueList &scfToInternalsQueue, bool active = false, LPCSTR desc = "")
		{
			// Check first message.
			AssertRemoveMessage_SetProgressiveConfig(scfToInternalsQueue, active, "i1" + CStringA(desc));

			if (active)
			{
				// Check additional messages for (transition to) active state.
				AssertRemoveMessage_SetProgressiveHost(scfToInternalsQueue, "i2" + CStringA(desc));
				AssertRemoveMessage_SetProgressiveConnectionInfo(scfToInternalsQueue, WaitingForInfo, "i3" + CStringA(desc));
				AssertRemoveMessage_GetProgressiveGamesInfo(scfToInternalsQueue, "i4" + CStringA(desc));
				AssertRemoveMessage_SetProgressiveHost(scfToInternalsQueue, "i5" + CStringA(desc));
				AssertRemoveMessage_ProgressiveExternalCommunicationsStatus(scfToInternalsQueue, "i6" + CStringA(desc));
			}

			// Verify that there are no more messages.
			EXPECT_TRUE(nullptr == scfToInternalsQueue.Remove()) << "Remove iX" + CStringA(desc); // Verify that the queue is empty.
		}

#pragma region CProgressiveProtocol
		/// <summary>
		/// Functional test for:
		///     CProgressiveProtocol::CProgressiveProtocol()
		/// Test for default values with default operating system.
		/// </summary>
		TEST_METHOD(CProgressiveProtocol_Default)
		{
			// Setup.
			CQueueList scfToInternalsQueue(true);

			// Perform operation(s) and test(s).
			CProtocol obj(scfToInternalsQueue);
			ON_CALL(obj, GetProgressiveConnection(_, _, _, _)).WillByDefault(Invoke(&obj, &CProtocol::BaseGetProgressiveConnection));
			ON_CALL(obj, RefreshConnection()).WillByDefault(Invoke(&obj, &CProtocol::BaseRefreshConnection));
			ON_CALL(obj, LoadConfigFile()).WillByDefault(Invoke(&obj, &CProtocol::BaseLoadConfigFile));
			EXPECT_CALL(obj, GetProgressiveConnection(_, _, _, _)).Times(0);
			EXPECT_CALL(obj, RefreshConnection()).Times(0);
			EXPECT_CALL(obj, LoadConfigFile()).Times(0);

			EXPECT_TRUE(nullptr == scfToInternalsQueue.Remove()) << "Remove default"; // Verify that the queue is empty.
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveProtocol::CProgressiveProtocol()
		///     CProgressiveProtocol::Init()
		/// Test for initial values with default operating system.
		/// </summary>
		TEST_METHOD(CProgressiveProtocol_DefaultInit)
		{
			// Setup.
			CQueueList scfToInternalsQueue(true);

			// Perform operation(s) and test(s).
			CProtocol obj(scfToInternalsQueue);
			ON_CALL(obj, GetProgressiveConnection(_, _, _, _)).WillByDefault(Invoke(&obj, &CProtocol::BaseGetProgressiveConnection));
			ON_CALL(obj, RefreshConnection()).WillByDefault(Invoke(&obj, &CProtocol::BaseRefreshConnection));
			ON_CALL(obj, LoadConfigFile()).WillByDefault(Invoke(&obj, &CProtocol::BaseLoadConfigFile));
			EXPECT_CALL(obj, GetProgressiveConnection(_, _, _, _)).Times(0);
			EXPECT_CALL(obj, RefreshConnection()).Times(1);
			EXPECT_CALL(obj, LoadConfigFile()).Times(1);
			EXPECT_TRUE(nullptr == scfToInternalsQueue.Remove()) << "Remove default"; // Verify that the queue is empty.
			obj.Init();
			AssertRemoveInitialMessages(scfToInternalsQueue);
		}
#pragma endregion CProgressiveProtocol

#pragma region RefreshConnection
		/// <summary>
		/// Functional test for:
		///     CProgressiveProtocol::RefreshConnection()
		/// Test RefreshConnection() with default operating system.
		/// </summary>
		TEST_METHOD(CProgressiveProtocol_RefreshConnection_Default)
		{
			// Data.
			const InternalMsgType msgType(INT_MSG_SET_PROGRESSIVE_CONFIG);
			const int msgPriority(QUEUE_PRIORITY_NORMAL);
			const bool active(false);

			// Setup.
			CQueueList scfToInternalsQueue(true);

			// Perform operation(s) and test(s).
			CProtocol obj(scfToInternalsQueue);
			ON_CALL(obj, GetProgressiveConnection(_, _, _, _)).WillByDefault(Invoke(&obj, &CProtocol::BaseGetProgressiveConnection));
			ON_CALL(obj, RefreshConnection()).WillByDefault(Invoke(&obj, &CProtocol::BaseRefreshConnection));
			ON_CALL(obj, LoadConfigFile()).WillByDefault(Invoke(&obj, &CProtocol::BaseLoadConfigFile));
			EXPECT_CALL(obj, GetProgressiveConnection(_, _, _, _)).Times(0);
			EXPECT_CALL(obj, RefreshConnection()).Times(2);
			EXPECT_CALL(obj, LoadConfigFile()).Times(2);
			obj.Init();
			AssertRemoveInitialMessages(scfToInternalsQueue);

			obj.RefreshConnection();
			AssertRemoveMessage_SetProgressiveConfig(scfToInternalsQueue, active, "1");
			EXPECT_TRUE(nullptr == scfToInternalsQueue.Remove()) << "Remove final"; // Verify that the queue is now empty.
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveProtocol::RefreshConnection()
		/// Test RefreshConnection() with non-default operating system and default configuration file.
		/// </summary>
		TEST_METHOD(CProgressiveProtocol_RefreshConnection_NonDefault)
		{
			// Data.
			const bool active(false);

			// Setup.
			CQueueList scfToInternalsQueue(true);
			COperatingSystemTest operatingSystem;

			// Perform operation(s) and test(s).
			CProtocol obj(scfToInternalsQueue, operatingSystem);
			ON_CALL(obj, GetProgressiveConnection(_, _, _, _)).WillByDefault(Invoke(&obj, &CProtocol::BaseGetProgressiveConnection));
			ON_CALL(obj, RefreshConnection()).WillByDefault(Invoke(&obj, &CProtocol::BaseRefreshConnection));
			ON_CALL(obj, LoadConfigFile()).WillByDefault(Invoke(&obj, &CProtocol::BaseLoadConfigFile));
			EXPECT_CALL(obj, GetProgressiveConnection(_, _, _, _)).Times(0);
			EXPECT_CALL(obj, RefreshConnection()).Times(2);
			EXPECT_CALL(obj, LoadConfigFile()).Times(2);
			obj.Init();
			AssertRemoveInitialMessages(scfToInternalsQueue);

			obj.RefreshConnection();
			AssertRemoveMessage_SetProgressiveConfig(scfToInternalsQueue, active, "1");
			EXPECT_TRUE(nullptr == scfToInternalsQueue.Remove()) << "Remove final"; // Verify that the queue is now empty.
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveProtocol::RefreshConnection()
		/// Test RefreshConnection() with non-default operating system and a not active configuration file.
		/// </summary>
		TEST_METHOD(CProgressiveProtocol_RefreshConnection_NotActive)
		{
			// Data.
			const LPCTSTR fileName(m_progConfigFileName);
			const CFile::OpenFlags nOpenFlags(m_progConfigOpenFlags);
			const bool active(false);
			const LPCSTR fileContent(active ? m_progConfigFileContent_Active : m_progConfigFileContent_NotActive);

			// Setup.
			CQueueList scfToInternalsQueue(true);
			COperatingSystemTest operatingSystem(fileName, nOpenFlags, fileContent, strlen(fileContent));

			// Perform operation(s) and test(s).
			CProtocol obj(scfToInternalsQueue, operatingSystem);
			ON_CALL(obj, GetProgressiveConnection(_, _, _, _)).WillByDefault(Invoke(&obj, &CProtocol::BaseGetProgressiveConnection));
			ON_CALL(obj, RefreshConnection()).WillByDefault(Invoke(&obj, &CProtocol::BaseRefreshConnection));
			ON_CALL(obj, LoadConfigFile()).WillByDefault(Invoke(&obj, &CProtocol::BaseLoadConfigFile));
			EXPECT_CALL(obj, GetProgressiveConnection(_, _, _, _)).Times(0);
			EXPECT_CALL(obj, RefreshConnection()).Times(2);
			EXPECT_CALL(obj, LoadConfigFile()).Times(2);
			obj.Init();
			AssertRemoveInitialMessages(scfToInternalsQueue);

			obj.RefreshConnection();
			AssertRemoveMessage_SetProgressiveConfig(scfToInternalsQueue, active, "1");
			EXPECT_TRUE(nullptr == scfToInternalsQueue.Remove()) << "Remove final"; // Verify that the queue is now empty.
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveProtocol::RefreshConnection()
		/// Test RefreshConnection() with non-default operating system and an active configuration file.
		/// Includes code coverage (deleting the m_progressiveConnection instance) for the destructor:
		///     CProgressiveProtocol::~CProgressiveProtocol()
		/// </summary>
		TEST_METHOD(CProgressiveProtocol_RefreshConnection_Active)
		{
			// Data.
			const LPCTSTR fileName(m_progConfigFileName);
			const CFile::OpenFlags nOpenFlags(m_progConfigOpenFlags);
			const bool active(true);
			const LPCSTR fileContent(active ? m_progConfigFileContent_Active : m_progConfigFileContent_NotActive);

			// Setup.
			CQueueList scfToInternalsQueue(true);
			COperatingSystemTest operatingSystem(fileName, nOpenFlags, fileContent, strlen(fileContent));

			// Perform operation(s) and test(s).
			CProtocol obj(scfToInternalsQueue, operatingSystem);
			ON_CALL(obj, GetProgressiveConnection(_, _, _, _)).WillByDefault(Invoke(&obj, &CProtocol::BaseGetProgressiveConnection));
			ON_CALL(obj, RefreshConnection()).WillByDefault(Invoke(&obj, &CProtocol::BaseRefreshConnection));
			ON_CALL(obj, LoadConfigFile()).WillByDefault(Invoke(&obj, &CProtocol::BaseLoadConfigFile));
			EXPECT_CALL(obj, GetProgressiveConnection(_, _, _, _)).Times(1);
			EXPECT_CALL(obj, RefreshConnection()).Times(2);
			EXPECT_CALL(obj, LoadConfigFile()).Times(2);

			obj.m_oneLinkProgressivesSenOptionEnabled = true;
			obj.m_oneLinkProgressivesSupportedByEGM = true;
			obj.Init();
			AssertRemoveInitialMessages(scfToInternalsQueue, active);

			obj.RefreshConnection();
			AssertRemoveMessage_SetProgressiveConfig(scfToInternalsQueue, active, "1");
			AssertRemoveMessage_SetProgressiveHost(scfToInternalsQueue, "2");
			AssertRemoveMessage_ProgressiveExternalCommunicationsStatus(scfToInternalsQueue, "3");
			EXPECT_TRUE(nullptr == scfToInternalsQueue.Remove()) << "Remove final"; // Verify that the queue is now empty.
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveProtocol::RefreshConnection()
		/// Test RefreshConnection() non-default operating system and an active configuration file
		/// and then again with a not active configuration file.
		/// </summary>
		TEST_METHOD(CProgressiveProtocol_RefreshConnection_ActiveNotActive)
		{
			// Data.
			const LPCTSTR fileName(m_progConfigFileName);
			const CFile::OpenFlags nOpenFlags(m_progConfigOpenFlags);
			bool active(true);
			LPCSTR fileContent(active ? m_progConfigFileContent_Active : m_progConfigFileContent_NotActive);

			// Setup.
			CQueueList scfToInternalsQueue(true);
			COperatingSystemTest operatingSystem(fileName, nOpenFlags, fileContent, strlen(fileContent));

			// Perform operation(s) and test(s).
			CProtocol obj(scfToInternalsQueue, operatingSystem);
			ON_CALL(obj, GetProgressiveConnection(_, _, _, _)).WillByDefault(Invoke(&obj, &CProtocol::BaseGetProgressiveConnection));
			ON_CALL(obj, RefreshConnection()).WillByDefault(Invoke(&obj, &CProtocol::BaseRefreshConnection));
			ON_CALL(obj, LoadConfigFile()).WillByDefault(Invoke(&obj, &CProtocol::BaseLoadConfigFile));
			EXPECT_CALL(obj, GetProgressiveConnection(_, _, _, _)).Times(1);
			EXPECT_CALL(obj, RefreshConnection()).Times(3);
			EXPECT_CALL(obj, LoadConfigFile()).Times(3);

			obj.m_oneLinkProgressivesSenOptionEnabled = true;
			obj.m_oneLinkProgressivesSupportedByEGM = true;
			obj.Init();
			AssertRemoveInitialMessages(scfToInternalsQueue, active);

			obj.RefreshConnection();
			AssertRemoveMessage_SetProgressiveConfig(scfToInternalsQueue, active, "1");
			AssertRemoveMessage_SetProgressiveHost(scfToInternalsQueue, "2");
			AssertRemoveMessage_ProgressiveExternalCommunicationsStatus(scfToInternalsQueue, "3");
			EXPECT_TRUE(nullptr == scfToInternalsQueue.Remove()) << "Remove final"; // Verify that the queue is now empty.

																					// Setup.  Set operating system to non-active configuration file.
			active = false;
			obj.m_oneLinkProgressivesSenOptionEnabled = false;
			fileContent = active ? m_progConfigFileContent_Active : m_progConfigFileContent_NotActive;
			operatingSystem.ChangeFileContents(reinterpret_cast<const BYTE *>(fileContent), strlen(fileContent));

			obj.RefreshConnection();
			AssertRemoveMessage_SetProgressiveConfig(scfToInternalsQueue, active, "1");
			AssertRemoveMessage_SetProgressiveHost(scfToInternalsQueue, "2");
			AssertRemoveMessage_SetProgressiveConnectionInfo(scfToInternalsQueue, Disconnected, "2d");
			AssertRemoveMessage_SetProgressiveConnectionInfo(scfToInternalsQueue, Disabled, "3");
			EXPECT_TRUE(nullptr == scfToInternalsQueue.Remove()) << "Remove final2"; // Verify that the queue is now empty.
		}
#pragma endregion RefreshConnection

#pragma region LoadConfigFile
		void CProgressiveProtocol_LoadConfigFile(bool active,
			bool oneLinkProgressivesSenOptionEnabled,
			bool oneLinkProgressivesSupportedByEGM)
		{
			// Data.
			const LPCTSTR fileName(m_progConfigFileName);
			const CFile::OpenFlags nOpenFlags(m_progConfigOpenFlags);
			const LPCSTR fileContent(active ? m_progConfigFileContent_Active : m_progConfigFileContent_NotActive);
			const bool activeResult(oneLinkProgressivesSupportedByEGM && oneLinkProgressivesSenOptionEnabled);

			// Setup.
			CQueueList scfToInternalsQueue(true);
			COperatingSystemTest operatingSystem(fileName, nOpenFlags, fileContent, strlen(fileContent));

			// Perform operation(s) and test(s).
			CProtocol obj(scfToInternalsQueue, operatingSystem);
			ON_CALL(obj, GetProgressiveConnection(_, _, _, _)).WillByDefault(Invoke(&obj, &CProtocol::BaseGetProgressiveConnection));
			ON_CALL(obj, RefreshConnection()).WillByDefault(Invoke(&obj, &CProtocol::BaseRefreshConnection));
			ON_CALL(obj, LoadConfigFile()).WillByDefault(Invoke(&obj, &CProtocol::BaseLoadConfigFile));
			EXPECT_CALL(obj, GetProgressiveConnection(_, _, _, _)).Times(activeResult ? 1 : 0);
			EXPECT_CALL(obj, RefreshConnection()).Times(1);
			EXPECT_CALL(obj, LoadConfigFile()).Times(1);

			obj.m_oneLinkProgressivesSenOptionEnabled = oneLinkProgressivesSenOptionEnabled;
			obj.m_oneLinkProgressivesSupportedByEGM = oneLinkProgressivesSupportedByEGM;
			obj.Init();
			AssertRemoveInitialMessages(scfToInternalsQueue, activeResult);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveProtocol::LoadConfigFile()
		/// Test LoadConfigFile() with:
		///    progConfig.m_active = false
		///    CProgressiveProtocol::m_oneLinkProgressivesSenOptionEnabled = false
		/// </summary>
		TEST_METHOD(CProgressiveProtocol_LoadConfigFile0a)
		{
			CProgressiveProtocol_LoadConfigFile(false, false, false);
		}

		TEST_METHOD(CProgressiveProtocol_LoadConfigFile0b)
		{
			CProgressiveProtocol_LoadConfigFile(false, false, true);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveProtocol::LoadConfigFile()
		/// Test LoadConfigFile() with:
		///    progConfig.m_active = false
		///    CProgressiveProtocol::m_oneLinkProgressivesSenOptionEnabled = true
		/// </summary>
		TEST_METHOD(CProgressiveProtocol_LoadConfigFile1a)
		{
			CProgressiveProtocol_LoadConfigFile(false, true, false);
		}

		TEST_METHOD(CProgressiveProtocol_LoadConfigFile1b)
		{
			CProgressiveProtocol_LoadConfigFile(false, true, true);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveProtocol::LoadConfigFile()
		/// Test LoadConfigFile() with:
		///    progConfig.m_active = false
		///    CProgressiveProtocol::m_oneLinkProgressivesSenOptionEnabled = false
		/// </summary>
		TEST_METHOD(CProgressiveProtocol_LoadConfigFile2a)
		{
			CProgressiveProtocol_LoadConfigFile(false, false, false);
		}

		TEST_METHOD(CProgressiveProtocol_LoadConfigFile2b)
		{
			CProgressiveProtocol_LoadConfigFile(false, false, true);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveProtocol::LoadConfigFile()
		/// Test LoadConfigFile() with:
		///    progConfig.m_active = false
		///    CProgressiveProtocol::m_oneLinkProgressivesSenOptionEnabled = true
		/// </summary>
		TEST_METHOD(CProgressiveProtocol_LoadConfigFile3a)
		{
			CProgressiveProtocol_LoadConfigFile(false, true, false);
		}

		TEST_METHOD(CProgressiveProtocol_LoadConfigFile3b)
		{
			CProgressiveProtocol_LoadConfigFile(false, true, true);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveProtocol::LoadConfigFile()
		/// Test LoadConfigFile() with:
		///    progConfig.m_active = true
		///    CProgressiveProtocol::m_oneLinkProgressivesSenOptionEnabled = false
		/// </summary>
		TEST_METHOD(CProgressiveProtocol_LoadConfigFile4a)
		{
			CProgressiveProtocol_LoadConfigFile(true, false, false);
		}

		TEST_METHOD(CProgressiveProtocol_LoadConfigFile4)
		{
			CProgressiveProtocol_LoadConfigFile(true, false, true);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveProtocol::LoadConfigFile()
		/// Test LoadConfigFile() with:
		///    progConfig.m_active = true
		///    CProgressiveProtocol::m_oneLinkProgressivesSenOptionEnabled = true
		/// </summary>
		TEST_METHOD(CProgressiveProtocol_LoadConfigFile5a)
		{
			CProgressiveProtocol_LoadConfigFile(true, true, false);
		}

		TEST_METHOD(CProgressiveProtocol_LoadConfigFile5)
		{
			CProgressiveProtocol_LoadConfigFile(true, true, true);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveProtocol::LoadConfigFile()
		/// Test LoadConfigFile() with:
		///    progConfig.m_active = true
		///    CProgressiveProtocol::m_oneLinkProgressivesSenOptionEnabled = false
		/// </summary>
		TEST_METHOD(CProgressiveProtocol_LoadConfigFile6a)
		{
			CProgressiveProtocol_LoadConfigFile(true, false, false);
		}

		TEST_METHOD(CProgressiveProtocol_LoadConfigFile6b)
		{
			CProgressiveProtocol_LoadConfigFile(true, false, true);
		}
		/// <summary>
		/// Functional test for:
		///     CProgressiveProtocol::LoadConfigFile()
		/// Test LoadConfigFile() with:
		///    progConfig.m_active = true
		///    CProgressiveProtocol::m_oneLinkProgressivesSenOptionEnabled = true
		/// </summary>
		TEST_METHOD(CProgressiveProtocol_LoadConfigFile7a)
		{
			CProgressiveProtocol_LoadConfigFile(true, true, false);
		}

		TEST_METHOD(CProgressiveProtocol_LoadConfigFile7b)
		{
			CProgressiveProtocol_LoadConfigFile(true, true, true);
		}
#pragma endregion LoadConfigFile

#pragma region SetSenOption
		bool CalcActive(bool configFileActive,
			bool oneLinkProgressivesSenOptionEnabled,
			bool oneLinkProgressivesSupportedByEGM)
		{
			return oneLinkProgressivesSupportedByEGM && oneLinkProgressivesSenOptionEnabled;
		}

		void CProgressiveProtocol_SetSenOption(bool active,
			bool oneLinkProgressivesSenOptionEnabled,
			bool newOneLinkProgressivesSenOptionEnabled,
			bool oneLinkProgressivesSupportedByEGM,
			bool newOneLinkProgressivesSupportedByEGM)
		{
			// Data.
			const LPCTSTR fileName(m_progConfigFileName);
			const CFile::OpenFlags nOpenFlags(m_progConfigOpenFlags);
			const LPCSTR fileContent(active ? m_progConfigFileContent_Active : m_progConfigFileContent_NotActive);
			const bool activeInit(CalcActive(active,
				oneLinkProgressivesSenOptionEnabled,
				oneLinkProgressivesSupportedByEGM));
			const bool activeResult(CalcActive(active,
				newOneLinkProgressivesSenOptionEnabled,
				newOneLinkProgressivesSupportedByEGM));

			bool needRefresh(activeInit != activeResult);

			CStringA desc;
			desc.Format(" egm=%d->%d", (int)oneLinkProgressivesSupportedByEGM, (int)newOneLinkProgressivesSupportedByEGM);
			CStringW descW(desc);

			// Setup.
			CQueueList scfToInternalsQueue(true);
			COperatingSystemTest operatingSystem(fileName, nOpenFlags, fileContent, strlen(fileContent));

			// Perform operation(s) and test(s).
			CProtocol obj(scfToInternalsQueue, operatingSystem);
			ON_CALL(obj, GetProgressiveConnection(_, _, _, _)).WillByDefault(Invoke(&obj, &CProtocol::BaseGetProgressiveConnection));
			ON_CALL(obj, RefreshConnection()).WillByDefault(Invoke(&obj, &CProtocol::BaseRefreshConnection));
			ON_CALL(obj, LoadConfigFile()).WillByDefault(Invoke(&obj, &CProtocol::BaseLoadConfigFile));
			int nTimes((activeInit ? 1 : 0) + (needRefresh && activeResult && activeInit != activeResult ? 1 : 0));
			EXPECT_CALL(obj, GetProgressiveConnection(_, _, _, _)).Times(nTimes);
			EXPECT_CALL(obj, RefreshConnection()).Times(needRefresh ? 2 : 1);
			EXPECT_CALL(obj, LoadConfigFile()).Times(needRefresh ? 2 : 1);

			obj.m_oneLinkProgressivesSenOptionEnabled = oneLinkProgressivesSenOptionEnabled;
			obj.m_oneLinkProgressivesSupportedByEGM = oneLinkProgressivesSupportedByEGM;
			obj.Init();
			AssertRemoveInitialMessages(scfToInternalsQueue, activeInit, desc);

			// Test nullptr messages.
			obj.CProgressiveProtocol::ProcessMessageFromInternals(nullptr);
			obj.CProgressiveProtocol::ProcessSetOneLinkProgressivesSupportFromInternals(nullptr);
			Assert::IsNull(scfToInternalsQueue.Remove(), L"Remove nullptr" + descW); // Verify that the queue is still empty.

																					 // Process a message with "new" values.
																					 // Don't startup the object thread or use the message queue, in order to avoid interactions with timed events.
			obj.CProgressiveProtocol::ProcessMessageFromInternals(new CInternalMessage(INT_MSG_SET_ONE_LINK_PROGRESSIVES_SUPPORT,
				(WORD)newOneLinkProgressivesSenOptionEnabled,
				(WORD)newOneLinkProgressivesSupportedByEGM));

			// Test result of processing the message with the "new" values.
			if (needRefresh)
			{
				AssertRemoveMessage_SetProgressiveConfig(scfToInternalsQueue, activeResult, "1" + desc);
				if (activeResult)
				{
					AssertRemoveMessage_SetProgressiveHost(scfToInternalsQueue, "2" + desc);
					if (!activeInit)
					{
						AssertRemoveMessage_SetProgressiveConnectionInfo(scfToInternalsQueue, WaitingForInfo, "3" + desc);
						AssertRemoveMessage_GetProgressiveGamesInfo(scfToInternalsQueue, "4" + desc);
						AssertRemoveMessage_SetProgressiveHost(scfToInternalsQueue, "5" + desc);
					}
					AssertRemoveMessage_ProgressiveExternalCommunicationsStatus(scfToInternalsQueue, "6" + desc);
				}
				else if (activeInit)
				{
					AssertRemoveMessage_SetProgressiveHost(scfToInternalsQueue, "r2" + desc);
					AssertRemoveMessage_SetProgressiveConnectionInfo(scfToInternalsQueue, Disconnected, "r2d" + desc);
					AssertRemoveMessage_SetProgressiveConnectionInfo(scfToInternalsQueue, Disabled, "r3" + desc);
				}
			}
			EXPECT_TRUE(nullptr == scfToInternalsQueue.Remove()) << "Remove final" + desc; // Verify that the queue is now empty.

			Assert::IsTrue(Mock::VerifyAndClearExpectations(&obj));
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveProtocol::ProcessMessageFromInternals()
		///     CProgressiveProtocol::ProcessSetOneLinkProgressivesSupportFromInternals()
		/// Test LoadConfigFile() with:
		///    progConfig.m_active = false
		///    CProgressiveProtocol::m_oneLinkProgressivesSenOptionSupported = false
		///    CProgressiveProtocol::m_oneLinkProgressivesSenOptionEnabled = false
		/// Test ProcessSetOneLinkProgressivesSupportFromInternals() with varying values.
		/// </summary>
		TEST_METHOD(CProgressiveProtocol_SetSenOption0a)
		{
			CProgressiveProtocol_SetSenOption(false, false, false, false, false);
			CProgressiveProtocol_SetSenOption(false, false, false, false, true);
			CProgressiveProtocol_SetSenOption(false, false, false, true, false);
			CProgressiveProtocol_SetSenOption(false, false, false, true, true);
		}

		TEST_METHOD(CProgressiveProtocol_SetSenOption0b)
		{
			CProgressiveProtocol_SetSenOption(false, false, true, false, false);
			CProgressiveProtocol_SetSenOption(false, false, true, false, true);
			CProgressiveProtocol_SetSenOption(false, false, true, true, false);
			CProgressiveProtocol_SetSenOption(false, false, true, true, true);
		}

		TEST_METHOD(CProgressiveProtocol_SetSenOption0c)
		{
			CProgressiveProtocol_SetSenOption(false, false, false, false, false);
			CProgressiveProtocol_SetSenOption(false, false, false, false, true);
			CProgressiveProtocol_SetSenOption(false, false, false, true, false);
			CProgressiveProtocol_SetSenOption(false, false, false, true, true);
		}

		TEST_METHOD(CProgressiveProtocol_SetSenOption0d)
		{
			CProgressiveProtocol_SetSenOption(false, false, true, false, false);
			CProgressiveProtocol_SetSenOption(false, false, true, false, true);
			CProgressiveProtocol_SetSenOption(false, false, true, true, false);
			CProgressiveProtocol_SetSenOption(false, false, true, true, true);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveProtocol::ProcessMessageFromInternals()
		///     CProgressiveProtocol::ProcessSetOneLinkProgressivesSupportFromInternals()
		/// Test LoadConfigFile() with:
		///    progConfig.m_active = false
		///    CProgressiveProtocol::m_oneLinkProgressivesSenOptionSupported = false
		///    CProgressiveProtocol::m_oneLinkProgressivesSenOptionEnabled = true
		/// Test ProcessSetOneLinkProgressivesSupportFromInternals() with varying values.
		/// </summary>
		TEST_METHOD(CProgressiveProtocol_SetSenOption1a)
		{
			CProgressiveProtocol_SetSenOption(false, true, false, false, false);
			CProgressiveProtocol_SetSenOption(false, true, false, false, true);
			CProgressiveProtocol_SetSenOption(false, true, false, true, false);
			CProgressiveProtocol_SetSenOption(false, true, false, true, true);
		}

		TEST_METHOD(CProgressiveProtocol_SetSenOption1b)
		{
			CProgressiveProtocol_SetSenOption(false, true, true, false, false);
			CProgressiveProtocol_SetSenOption(false, true, true, false, true);
			CProgressiveProtocol_SetSenOption(false, true, true, true, false);
			CProgressiveProtocol_SetSenOption(false, true, true, true, true);
		}

		TEST_METHOD(CProgressiveProtocol_SetSenOption1c)
		{
			CProgressiveProtocol_SetSenOption(false, true, false, false, false);
			CProgressiveProtocol_SetSenOption(false, true, false, false, true);
			CProgressiveProtocol_SetSenOption(false, true, false, true, false);
			CProgressiveProtocol_SetSenOption(false, true, false, true, true);
		}

		TEST_METHOD(CProgressiveProtocol_SetSenOption1d)
		{
			CProgressiveProtocol_SetSenOption(false, true, true, false, false);
			CProgressiveProtocol_SetSenOption(false, true, true, false, true);
			CProgressiveProtocol_SetSenOption(false, true, true, true, false);
			CProgressiveProtocol_SetSenOption(false, true, true, true, true);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveProtocol::ProcessMessageFromInternals()
		///     CProgressiveProtocol::ProcessSetOneLinkProgressivesSupportFromInternals()
		/// Test LoadConfigFile() with:
		///    progConfig.m_active = false
		///    CProgressiveProtocol::m_oneLinkProgressivesSenOptionSupported = true
		///    CProgressiveProtocol::m_oneLinkProgressivesSenOptionEnabled = false
		/// Test ProcessSetOneLinkProgressivesFromInternals() with varying values.
		/// </summary>
		TEST_METHOD(CProgressiveProtocol_SetSenOption2a)
		{
			CProgressiveProtocol_SetSenOption(false, false, false, false, false);
			CProgressiveProtocol_SetSenOption(false, false, false, false, true);
			CProgressiveProtocol_SetSenOption(false, false, false, true, false);
			CProgressiveProtocol_SetSenOption(false, false, false, true, true);
		}

		TEST_METHOD(CProgressiveProtocol_SetSenOption2b)
		{
			CProgressiveProtocol_SetSenOption(false, false, true, false, false);
			CProgressiveProtocol_SetSenOption(false, false, true, false, true);
			CProgressiveProtocol_SetSenOption(false, false, true, true, false);
			CProgressiveProtocol_SetSenOption(false, false, true, true, true);
		}

		TEST_METHOD(CProgressiveProtocol_SetSenOption2c)
		{
			CProgressiveProtocol_SetSenOption(false, false, false, false, false);
			CProgressiveProtocol_SetSenOption(false, false, false, false, true);
			CProgressiveProtocol_SetSenOption(false, false, false, true, false);
			CProgressiveProtocol_SetSenOption(false, false, false, true, true);
		}

		TEST_METHOD(CProgressiveProtocol_SetSenOption2d)
		{
			CProgressiveProtocol_SetSenOption(false, false, true, false, false);
			CProgressiveProtocol_SetSenOption(false, false, true, false, true);
			CProgressiveProtocol_SetSenOption(false, false, true, true, false);
			CProgressiveProtocol_SetSenOption(false, false, true, true, true);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveProtocol::ProcessMessageFromInternals()
		///     CProgressiveProtocol::ProcessSetOneLinkProgressivesSupportFromInternals()
		/// Test LoadConfigFile() with:
		///    progConfig.m_active = false
		///    CProgressiveProtocol::m_oneLinkProgressivesSenOptionSupported = true
		///    CProgressiveProtocol::m_oneLinkProgressivesSenOptionEnabled = true
		/// Test ProcessSetOneLinkProgressivesSupportFromInternals() with varying values.
		/// </summary>
		TEST_METHOD(CProgressiveProtocol_SetSenOption3a)
		{
			CProgressiveProtocol_SetSenOption(false, true, false, false, false);
			CProgressiveProtocol_SetSenOption(false, true, false, false, true);
			CProgressiveProtocol_SetSenOption(false, true, false, true, false);
			CProgressiveProtocol_SetSenOption(false, true, false, true, true);
		}

		TEST_METHOD(CProgressiveProtocol_SetSenOption3b)
		{
			CProgressiveProtocol_SetSenOption(false, true, true, false, false);
			CProgressiveProtocol_SetSenOption(false, true, true, false, true);
			CProgressiveProtocol_SetSenOption(false, true, true, true, false);
			CProgressiveProtocol_SetSenOption(false, true, true, true, true);
		}

		TEST_METHOD(CProgressiveProtocol_SetSenOption3c)
		{
			CProgressiveProtocol_SetSenOption(false, true, false, false, false);
			CProgressiveProtocol_SetSenOption(false, true, false, false, true);
			CProgressiveProtocol_SetSenOption(false, true, false, true, false);
			CProgressiveProtocol_SetSenOption(false, true, false, true, true);
		}

		TEST_METHOD(CProgressiveProtocol_SetSenOption3d)
		{
			CProgressiveProtocol_SetSenOption(false, true, true, false, false);
			CProgressiveProtocol_SetSenOption(false, true, true, false, true);
			CProgressiveProtocol_SetSenOption(false, true, true, true, false);
			CProgressiveProtocol_SetSenOption(false, true, true, true, true);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveProtocol::ProcessMessageFromInternals()
		///     CProgressiveProtocol::ProcessSetOneLinkProgressivesSupportFromInternals()
		/// Test LoadConfigFile() with:
		///    progConfig.m_active = true
		///    CProgressiveProtocol::m_oneLinkProgressivesSenOptionSupported = false
		///    CProgressiveProtocol::m_oneLinkProgressivesSenOptionEnabled = false
		/// Test ProcessSetOneLinkProgressivesSupportFromInternals() with varying values.
		/// </summary>
		TEST_METHOD(CProgressiveProtocol_SetSenOption4a)
		{
			CProgressiveProtocol_SetSenOption(true, false, false, false, false);
			CProgressiveProtocol_SetSenOption(true, false, false, false, true);
			CProgressiveProtocol_SetSenOption(true, false, false, true, false);
			CProgressiveProtocol_SetSenOption(true, false, false, true, true);
		}

		TEST_METHOD(CProgressiveProtocol_SetSenOption4b)
		{
			CProgressiveProtocol_SetSenOption(true, false, true, false, false);
			CProgressiveProtocol_SetSenOption(true, false, true, false, true);
			CProgressiveProtocol_SetSenOption(true, false, true, true, false);
			CProgressiveProtocol_SetSenOption(true, false, true, true, true);
		}

		TEST_METHOD(CProgressiveProtocol_SetSenOption4c)
		{
			CProgressiveProtocol_SetSenOption(true, false, false, false, false);
			CProgressiveProtocol_SetSenOption(true, false, false, false, true);
			CProgressiveProtocol_SetSenOption(true, false, false, true, false);
			CProgressiveProtocol_SetSenOption(true, false, false, true, true);
		}

		TEST_METHOD(CProgressiveProtocol_SetSenOption4d)
		{
			CProgressiveProtocol_SetSenOption(true, false, true, false, false);
			CProgressiveProtocol_SetSenOption(true, false, true, false, true);
			CProgressiveProtocol_SetSenOption(true, false, true, true, false);
			CProgressiveProtocol_SetSenOption(true, false, true, true, true);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveProtocol::ProcessMessageFromInternals()
		///     CProgressiveProtocol::ProcessSetOneLinkProgressivesSupportFromInternals()
		/// Test LoadConfigFile() with:
		///    progConfig.m_active = true
		///    CProgressiveProtocol::m_oneLinkProgressivesSenOptionSupported = false
		///    CProgressiveProtocol::m_oneLinkProgressivesSenOptionEnabled = true
		/// Test ProcessSetOneLinkProgressivesSupportFromInternals() with varying values.
		/// </summary>
		TEST_METHOD(CProgressiveProtocol_SetSenOption5a)
		{
			CProgressiveProtocol_SetSenOption(true, true, false, false, false);
			CProgressiveProtocol_SetSenOption(true, true, false, false, true);
			CProgressiveProtocol_SetSenOption(true, true, false, true, false);
			CProgressiveProtocol_SetSenOption(true, true, false, true, true);
		}

		TEST_METHOD(CProgressiveProtocol_SetSenOption5b)
		{
			CProgressiveProtocol_SetSenOption(true, true, true, false, false);
			CProgressiveProtocol_SetSenOption(true, true, true, false, true);
			CProgressiveProtocol_SetSenOption(true, true, true, true, false);
			CProgressiveProtocol_SetSenOption(true, true, true, true, true);
		}

		TEST_METHOD(CProgressiveProtocol_SetSenOption5c)
		{
			CProgressiveProtocol_SetSenOption(true, true, false, false, false);
			CProgressiveProtocol_SetSenOption(true, true, false, false, true);
			CProgressiveProtocol_SetSenOption(true, true, false, true, false);
			CProgressiveProtocol_SetSenOption(true, true, false, true, true);
		}

		TEST_METHOD(CProgressiveProtocol_SetSenOption5d)
		{
			CProgressiveProtocol_SetSenOption(true, true, true, false, false);
			CProgressiveProtocol_SetSenOption(true, true, true, false, true);
			CProgressiveProtocol_SetSenOption(true, true, true, true, false);
			CProgressiveProtocol_SetSenOption(true, true, true, true, true);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveProtocol::ProcessMessageFromInternals()
		///     CProgressiveProtocol::ProcessSetOneLinkProgressivesSupportFromInternals()
		/// Test LoadConfigFile() with:
		///    progConfig.m_active = true
		///    CProgressiveProtocol::m_oneLinkProgressivesSenOptionSupported = true
		///    CProgressiveProtocol::m_oneLinkProgressivesSenOptionEnabled = false
		/// Test ProcessSetOneLinkProgressivesSupportFromInternals() with varying values.
		/// </summary>
		TEST_METHOD(CProgressiveProtocol_SetSenOption6a)
		{
			CProgressiveProtocol_SetSenOption(true, false, false, false, false);
			CProgressiveProtocol_SetSenOption(true, false, false, false, true);
			CProgressiveProtocol_SetSenOption(true, false, false, true, false);
			CProgressiveProtocol_SetSenOption(true, false, false, true, true);
		}

		TEST_METHOD(CProgressiveProtocol_SetSenOption6b)
		{
			CProgressiveProtocol_SetSenOption(true, false, true, false, false);
			CProgressiveProtocol_SetSenOption(true, false, true, false, true);
			CProgressiveProtocol_SetSenOption(true, false, true, true, false);
			CProgressiveProtocol_SetSenOption(true, false, true, true, true);
		}

		TEST_METHOD(CProgressiveProtocol_SetSenOption6c)
		{
			CProgressiveProtocol_SetSenOption(true, false, false, false, false);
			CProgressiveProtocol_SetSenOption(true, false, false, false, true);
			CProgressiveProtocol_SetSenOption(true, false, false, true, false);
			CProgressiveProtocol_SetSenOption(true, false, false, true, true);
		}

		TEST_METHOD(CProgressiveProtocol_SetSenOption6d)
		{
			CProgressiveProtocol_SetSenOption(true, false, true, false, false);
			CProgressiveProtocol_SetSenOption(true, false, true, false, true);
			CProgressiveProtocol_SetSenOption(true, false, true, true, false);
			CProgressiveProtocol_SetSenOption(true, false, true, true, true);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveProtocol::ProcessMessageFromInternals()
		///     CProgressiveProtocol::ProcessSetOneLinkProgressivesSupportFromInternals()
		/// Test LoadConfigFile() with:
		///    progConfig.m_active = true
		///    CProgressiveProtocol::m_oneLinkProgressivesSenOptionSupported = true
		///    CProgressiveProtocol::m_oneLinkProgressivesSenOptionEnabled = true
		/// Test ProcessSetOneLinkProgressivesSupportFromInternals() with varying values.
		/// </summary>
		TEST_METHOD(CProgressiveProtocol_SetSenOption7a)
		{
			CProgressiveProtocol_SetSenOption(true, true, false, false, false);
			CProgressiveProtocol_SetSenOption(true, true, false, false, true);
			CProgressiveProtocol_SetSenOption(true, true, false, true, true);
			CProgressiveProtocol_SetSenOption(true, true, false, true, true);
		}

		TEST_METHOD(CProgressiveProtocol_SetSenOption7b)
		{
			CProgressiveProtocol_SetSenOption(true, true, true, false, false);
			CProgressiveProtocol_SetSenOption(true, true, true, false, true);
			CProgressiveProtocol_SetSenOption(true, true, true, true, false);
			CProgressiveProtocol_SetSenOption(true, true, true, true, true);
		}

		TEST_METHOD(CProgressiveProtocol_SetSenOption7c)
		{
			CProgressiveProtocol_SetSenOption(true, true, false, false, false);
			CProgressiveProtocol_SetSenOption(true, true, false, false, true);
			CProgressiveProtocol_SetSenOption(true, true, false, true, false);
			CProgressiveProtocol_SetSenOption(true, true, false, true, true);
		}

		TEST_METHOD(CProgressiveProtocol_SetSenOption7d)
		{
			CProgressiveProtocol_SetSenOption(true, true, true, false, false);
			CProgressiveProtocol_SetSenOption(true, true, true, false, true);
			CProgressiveProtocol_SetSenOption(true, true, true, true, false);
			CProgressiveProtocol_SetSenOption(true, true, true, true, true);
		}
#pragma endregion SetSenOption
	};

	const CString ProgressiveProtocol_Connection::m_testThreadName(_T("ReadFromInternals/ProgressiveProtocol"));
	const LPCTSTR ProgressiveProtocol_Connection::m_progConfigFileName(_T("D:\\SentinelData\\ProgressiveProtocolConfig.txt"));
	const char ProgressiveProtocol_Connection::m_progConfigFileContent_Active[] =
		"Active=true\n"
		"DNSLookup=false\n"
		"Endpoint=invalid.com:12345\n"
		"PingInterval=6000\n";
	const char ProgressiveProtocol_Connection::m_progConfigFileContent_NotActive[] =
		"Active=false\n"
		"DNSLookup=false\n"
		"Endpoint=invalid.com:12345\n"
		"PingInterval=6000\n";

	CString ProgressiveProtocol_Connection::m_gmockFailure;
	ProgressiveProtocol_Connection::CTersePrinter ProgressiveProtocol_Connection::m_tersePrinter;
}
#endif //TODO-PORT