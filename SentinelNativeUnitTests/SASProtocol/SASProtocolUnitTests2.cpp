#if 000//TODO-PORT
#include "stdafx.h"
#include "gmock/gmock.h" // Include before <setupapi.h>.
#include "SASProtocol/SASProtocol.h"

// Unit testing includes.
#include "UnitTestingOperatingSystem.h"
#include "SASProtocol/SASSerialWrapperMock.h"
#include "TestUtil/UnitTestingMatchers.h"
#include "UnitTestingToString.h"
#include "UnitTestingUtils.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using ::testing::_;
using ::testing::Assign;
using ::testing::Invoke;
using ::testing::SetArgReferee;

namespace SentinelNativeUnitTests
{
	/// <summary>
	/// Class intended primarily for GMock-related tests.
	/// </summary>
	TEST_CLASS(SASProtocol2)
	{
// #pragma region NonGmockTests
		/// <summary>
		/// Ensure that the first test method of each module does NOT generate a gmock error.
		/// The build server can stop reporting before indicating that a gmock test failed, but indicate that the prior method passed.
		/// Therefore, it is very useful to ensure that the prior method is in the same module.
		/// </summary>
		TEST_METHOD(SASProtocol2_NotNullTest)
		{
			// Setup.
			CQueueList gcfToInternalsQueue(true);
			CSASProtocol obj(gcfToInternalsQueue);

			// Test.
			Assert::IsNotNull(&obj, _T("&obj"));
		}
// #pragma endregion NonGmockTests

		static const byte m_gameAddr = 1; // Default SAS game address.

		// Remove all messages and assert that they are the correct default message(s).
		static void AssertRemoveDefaultMessages(CQueueList &gcfToInternalsQueue)
		{
			// No messages present by default.

			// Verify that there are no more messages.
			ASSERT_EQ(nullptr, gcfToInternalsQueue.Remove()) << "Remove default final";
		}

		// Remove all messages and assert that they are the correct Start() message(s).
		static void AssertRemoveStartMessages(CQueueList &gcfToInternalsQueue)
		{
			// Check first message.
			CQueueItem *item1 = gcfToInternalsQueue.Remove();
			ASSERT_TRUE(nullptr != item1);
			CInternalMessage *msg1 = dynamic_cast<CInternalMessage *>(item1);
			ASSERT_TRUE(nullptr != msg1);
			ASSERT_EQ(INT_MSG_GCF_STARTED, msg1->GetMessageType());

			// Verify that there are no more messages.
			ASSERT_EQ(nullptr, gcfToInternalsQueue.Remove());
		}

		// Class member(s) used by test (data) methods.
		static CString m_gmockFailure;

		class CTersePrinter : public ::testing::EmptyTestEventListener {
			// Called after a failed assertion or a SUCCEED() invocation.
			virtual void OnTestPartResult(const ::testing::TestPartResult& test_part_result) {
				if (test_part_result.failed())
				{
					CStringA failure;
					failure.Format("FAILED in %s:%d\n%s",
						test_part_result.file_name(),
						test_part_result.line_number(),
						test_part_result.summary());
					m_gmockFailure = failure;
				}
			}
		};

		static CTersePrinter m_tersePrinter;

		/// <summary>
		/// Initialize and/or test static class members.
		/// </summary>
		TEST_CLASS_INITIALIZE(ClassInitialize)
		{
			// Class initialization code.
			::testing::UnitTest::GetInstance()->listeners().Append(&m_tersePrinter);
		}

		/// <summary>
		/// Cleanup static class member(s).
		/// </summary>
		TEST_CLASS_CLEANUP(ClassCleanup)
		{
			// Class cleanup code.
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
			// - Use EXPECT_TRUE, EXPECT_EQ, etc., instead of "Assert:IsTrue", Assert::AreEqual" within test methods.
			if (!m_gmockFailure.IsEmpty())
			{
				Assert::IsTrue(false, m_gmockFailure);
			}
		}

		class CSASInterfaceMock : public CSASInterface
		{
		public:
			const byte *m_response; // Set directly to control next SentToSAS() ressult.
			DWORD m_responseSize; // Set directly to control next SentToSAS() ressult.

			CSASInterfaceMock(CSASSerialWrapper *serial, CSASConsumer *sasProtocol, CQueueList *gcfToInternalsQueue, CQueueList *internalsToGCFQueue)
				:
				m_response(nullptr),
				m_responseSize(0),
				CSASInterface(serial, sasProtocol, gcfToInternalsQueue, internalsToGCFQueue)
			{
			}

			MOCK_METHOD2(SendToSAS, void(CSASPollMessage *msg, byte retryCount));

			void MockSendToSAS(CSASPollMessage *msg, byte retryCount)
			{
				UNREFERENCED_PARAMETER(retryCount);
				bool isNack(true);
				if (nullptr == m_response)
				{
					m_responseSize = 0;
				}

				if (m_responseSize > 0)
				{
					if (msg->m_isVariableResponse && msg->GetResponseBufferSize() >= m_responseSize ||
						msg->GetResponseBufferSize() == m_responseSize)
					{
						memcpy(msg->GetResponseBuffer(), m_response, m_responseSize);
						msg->SetResponseBuffer(msg->GetResponseBuffer(), m_responseSize);
						isNack = msg->IsNACK(m_responseSize);
					}
				}

				if (isNack)
				{
					msg->SetReturnValue(SASPOLL_NACK);
					m_consecutiveCommErrorCount += 3;
					m_gameComStatus = false;
				}
				else
				{
					msg->SetReturnValue(SASPOLL_SUCCESS);
					m_consecutiveCommErrorCount = 0;
					m_gameComStatus = true;
				}
			}
		};

		// Use a test class that implements a GetSASInterface() suitable for testing.
		// Test with CSASInterface::SendToSAS() mocked.
		class CProtocol : public CSASProtocol
		{
		public:
			static CSASInterface *m_lastSASInterface; // A pointer to the last interface returned by MockGetSASInterface.

			CProtocol(CQueueList &gcfToInternalsQueue, IOperatingSystem &operatingSystem)
				:
				CSASProtocol(gcfToInternalsQueue, operatingSystem)
			{
			}

			MOCK_METHOD4(GetSASInterface,
				CSASInterface *(CSASConsumer *sasProtocol,
				CQueueList &gcfToInternalsQueue,
				CQueueList &internalsToGCFQueue,
				IOperatingSystem &operatingSystem));

			virtual CSASInterface *MockGetSASInterface(CSASConsumer *sasProtocol,
				CQueueList &gcfToInternalsQueue,
				CQueueList &internalsToGCFQueue,
				IOperatingSystem &operatingSystem)
			{
				CSASSerialWrapperMock *serial(new CSASSerialWrapperMock());
				ON_CALL(*serial, ReadSAS(_, _, _)).WillByDefault(SetArgReferee<2>(0));

				CSASInterfaceMock *sas(new CSASInterfaceMock(serial, sasProtocol, &gcfToInternalsQueue, &internalsToGCFQueue));
				ON_CALL(*sas, SendToSAS(_, _)).WillByDefault(Invoke(sas, &CSASInterfaceMock::MockSendToSAS));
				m_lastSASInterface = sas;
				return sas;
			}
		};

// #pragma region Init
		/// <summary>
		/// Functional test for:
		///     CSASProtocol::Init()
		/// Verify that the Init() function calls the GetSASInterface() function as expected.
		/// </summary>
		TEST_METHOD(Init_MockTest)
		{
			// Setup.
			CQueueList gcfToInternalsQueue(true);
			COperatingSystemTest operatingSystem;
			CProtocol obj(gcfToInternalsQueue, operatingSystem);
			ASSERT_EQ(nullptr, obj.m_sas);
			ON_CALL(obj, GetSASInterface(_, _, _, _)).WillByDefault(Invoke(&obj, &CProtocol::MockGetSASInterface));

			// Perform operation(s) and test(s).
			EXPECT_CALL(obj, GetSASInterface(_, _, _, _)).Times(0);
			EXPECT_CALL(obj, GetSASInterface(&obj,
				MatchRef(&obj.m_scfToInternalsQueue),
				MatchRef(&obj.m_internalsToSCFQueue),
				MatchRef(&obj.m_operatingSystem))).Times(1);

			obj.Init();
			ASSERT_TRUE(nullptr != obj.m_sas);
			ASSERT_EQ(CProtocol::m_lastSASInterface, obj.m_sas);
		}
// #pragma endregion Init

// #pragma region Start
		/// <summary>
		/// Functional test for:
		///     CSASProtocol::Start()
		/// Verify that the Start() function can be called successfully, followed by destructing the object.
		/// </summary>
		TEST_METHOD(Start_MockTest)
		{
			// Setup.
			CQueueList gcfToInternalsQueue(true);
			COperatingSystemTest operatingSystem;
			CProtocol obj(gcfToInternalsQueue, operatingSystem);
			ON_CALL(obj, GetSASInterface(_, _, _, _)).WillByDefault(Invoke(&obj, &CProtocol::MockGetSASInterface));

			obj.Init();
			ASSERT_TRUE(nullptr != obj.m_sas);
			ASSERT_EQ(CProtocol::m_lastSASInterface, obj.m_sas);

			ASSERT_TRUE(nullptr != obj.m_sas->m_serial);
			ASSERT_TRUE(nullptr != obj.m_sasPollQueue);
			ASSERT_TRUE(nullptr != obj.m_sendToEGMThread);
			ASSERT_EQ(nullptr, obj.m_scheduleThread);

			// Perform operation(s) and test(s).
			obj.Start();
			ASSERT_TRUE(nullptr != obj.m_scheduleThread);
			Sleep(5*1000);

			AssertRemoveStartMessages(gcfToInternalsQueue);
		}
// #pragma endregion Start

// #pragma region GeneralHelpers
		// Get the next non-comm message. Return nullptr if there are no non-comm messages.
		static CInternalMessage *GetNextNonCommMsg(CQueueList &queue)
		{
			CQueueItem *item(queue.Remove());
			CInternalMessage *msg(dynamic_cast<CInternalMessage *>(item));
			bool done(false);
			do
			{
				if (msg == nullptr)
				{
					done = true;
				}
				else
				{
					switch (msg->GetMessageType())
					{
					case INT_MSG_GAME_COMM_ERROR:
					case INT_MSG_GAME_COMM_STATUS:
						item = queue.Remove();
						msg = dynamic_cast<CInternalMessage *>(item);
						break;
					default:
						done = true;
						break;
					}
				}
			} while (!done);

			return msg;
		}

		class CTestSetup
		{
		public:
			// Data.
			CQueueList m_gcfToInternalsQueue;
			COperatingSystemTest m_operatingSystem;
			CProtocol m_obj;

			// Handy pointers.
			CSASInterfaceMock *m_mockSas;

			// Constructor.
			CTestSetup()
				:
				m_gcfToInternalsQueue(true),
				m_obj(m_gcfToInternalsQueue, m_operatingSystem),
				m_mockSas(nullptr)
			{
				ON_CALL(m_obj, GetSASInterface(_, _, _, _)).WillByDefault(Invoke(&m_obj, &CProtocol::MockGetSASInterface));
			}

			void Init()
			{
				m_obj.Init();
				m_mockSas = dynamic_cast<CSASInterfaceMock *>(m_obj.m_sas);
				ASSERT_TRUE(nullptr != m_mockSas);
				ON_CALL(*m_mockSas, SendToSAS(_, _)).WillByDefault(Invoke(m_mockSas, &CSASInterfaceMock::MockSendToSAS));

				AssertRemoveDefaultMessages(m_gcfToInternalsQueue); // Verify initialization.
			}

			// Secify simple ON_CALL behavior with message match by poll value.
			void OnCall(BYTE pollValue, const byte *response, DWORD responseSize)
			{
				ON_CALL(*m_mockSas, SendToSAS(MatchSASPollMessage(pollValue), 0))
					.WillByDefault(DoAll(
					Assign(&m_mockSas->m_response, response),
					Assign(&m_mockSas->m_responseSize, responseSize),
					Invoke(m_mockSas, &CSASInterfaceMock::MockSendToSAS)
					));
			}
		};
// #pragma endregion GeneralHelpers

// #pragma region GetSASVersion
		static const int VERSION_LENGTH_EXPECTED = 3; // 1 digit for major field, 2 digits for minor field.
		static const byte pollValue6f = 0x6f;
		static const byte pollValue2f = 0x2f;

		class CVersionData
		{
		public:
			// Data.
			static const BYTE sasVersionPollValue = 0x54;
			const LPCSTR versionSetup;
			const LPCSTR versionSetupGames;
			const LPCSTR version601;
			const LPCSTR version502;
			const LPCSTR version403;
			const LPCSTR version340;

			const LPCTSTR serialNumber601; // Serial number returned with version601 response;
			const LPCTSTR serialNumber502; // Serial number returned with version502 response;
			const LPCTSTR serialNumber403; // Serial number returned with version403 response;
			const LPCTSTR serialNumber340; // Serial number returned with version340 response;

			// Possible responses to SendToSAS() for GetSASVersion().
			std::vector<byte> receiveData601;
			std::vector<byte> receiveData502;
			std::vector<byte> receiveData403;
			std::vector<byte> receiveData340;

			// Constructor.
			CVersionData()
				:
				versionSetup("0"),
				versionSetupGames(""),
				version601("601"),
				version502("502"),
				version403("403"),
				version340("340"),
				serialNumber601(_T("")),
				serialNumber502(_T("TUVWXYZ1234567890")),
				serialNumber403(_T("abcdefghij!@#$%^&*()~`-_=+[]:.1234567890")),
				serialNumber340(_T("x"))
				{
				// 601 response.
				byte receiveData601Raw[] = {
					m_gameAddr, sasVersionPollValue, 0x03, // Header with length byte.
					(byte)version601[0], (byte)version601[1], (byte)version601[2], // Data.
					// Zero bytes of serial number data.
					0x00, 0x00 // CRC.
				};
				CUnitTestingUtils::SetSasLengthAndCrc(receiveData601Raw, _countof(receiveData601Raw));
				receiveData601.resize(_countof(receiveData601Raw));
				memcpy(receiveData601.data(), receiveData601Raw, _countof(receiveData601Raw));

				// 502 response.
				byte receiveData502Raw[] = {
					m_gameAddr, sasVersionPollValue, 0x00, // Header with length byte.
					(byte)version502[0], (byte)version502[1], (byte)version502[2], // Data.
					'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', // Serial number data.
					0x00, 0x00 // CRC.
				};
				CUnitTestingUtils::SetSasLengthAndCrc(receiveData502Raw, _countof(receiveData502Raw));
				receiveData502.resize(_countof(receiveData502Raw));
				memcpy(receiveData502.data(), receiveData502Raw, _countof(receiveData502Raw));

				// 403 response.
				byte receiveData403Raw[] = {
					m_gameAddr, sasVersionPollValue, 0x00, // Header with length byte.
					(byte)version403[0], (byte)version403[1], (byte)version403[2], // Data.
					'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', // Maximum length (40 bytes) of serial number data.
					'!', '@', '#', '$', '%', '^', '&', '*', '(', ')',
					'~', '`', '-', '_', '=', '+', '[', ']', ':', '.',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
					0x00, 0x00 // CRC.
				};
				CUnitTestingUtils::SetSasLengthAndCrc(receiveData403Raw, _countof(receiveData403Raw));
				receiveData403.resize(_countof(receiveData403Raw));
				memcpy(receiveData403.data(), receiveData403Raw, _countof(receiveData403Raw));

				// 340 response.
				byte receiveData340Raw[] = {
					m_gameAddr, sasVersionPollValue, 0x00, // Header with length byte.
					(byte)version340[0], (byte)version340[1], (byte)version340[2], // Version data.
					'x', // Serial number data.
					0x00, 0x00 // CRC.
				};
				CUnitTestingUtils::SetSasLengthAndCrc(receiveData340Raw, _countof(receiveData340Raw));
				receiveData340.resize(_countof(receiveData340Raw));
				memcpy(receiveData340.data(), receiveData340Raw, _countof(receiveData340Raw));
			}
		};

		/// <summary>
		/// Test current values for:
		///     CSASProtocol::m_sas.GetVersion()
		///     CSASProtocol::m_sas.GetMajorVersion()
		///     CSASProtocol::CSASInterface.GetVersionString()
		///     CSASProtocol::m_sasInfo.GetSASVersion()
		///     CSASProtocol::m_sasInfo.GetSerialNumber()
		///     CSASProtocol::m_mainGameMeterStepdown.GetMeterLongPoll()
		/// </summary>
		static void AssertRemoveMessages_GetSASVersion(CQueueList &gcfToInternalsQueue, LPCTSTR sasVersion, LPCSTR desc)
		{
			// Ignore comm error/status messages.
			CInternalMessage *msg(GetNextNonCommMsg(gcfToInternalsQueue));
			ASSERT_TRUE(nullptr != msg) << " -- " << desc;
			ASSERT_EQ(INT_MSG_EGM_PROTOCOL_VERSION, msg->GetMessageType()) << " -- " << desc;
			ASSERT_STREQ(CString(sasVersion), msg->GetStringData()) << " -- " << desc;

			// Verify that there are no more messages.
			ASSERT_EQ(nullptr, gcfToInternalsQueue.Remove());
		}

		/// <summary>
		/// Test current values for:
		///     CSASProtocol::m_sas.GetVersion()
		///     CSASProtocol::m_sas.GetMajorVersion()
		///     CSASProtocol::CSASInterface.GetVersionString()
		///     CSASProtocol::m_sasInfo.GetSASVersion()
		///     CSASProtocol::m_sasInfo.GetSerialNumber()
		///     CSASProtocol::m_mainGameMeterStepdown.GetMeterLongPoll()
		/// </summary>
		static void AssertSetupVersion(const CSASProtocol &obj, LPCSTR sasVersion, LPCSTR gamesVersion, LPCSTR desc)
		{
			ASSERT_EQ(3, (size_t)CSASInterface::VERSION_LENGTH) << " -- " << desc;
			ASSERT_EQ(0, strncmp(sasVersion, (LPCSTR)obj.m_sas->GetVersion(), (size_t)CSASInterface::VERSION_LENGTH)) << " -- " << desc;
			ASSERT_EQ(sasVersion[0], obj.m_sas->GetMajorVersion()) << " -- " << desc;
			ASSERT_STREQ(CString(sasVersion), obj.m_sas->GetVersionString()) << " -- " << desc;
			ASSERT_STREQ(CString(gamesVersion), obj.m_sasInfo->GetSASVersion()) << " -- " << desc;
			ASSERT_STREQ(CString(_T("")), obj.m_sasInfo->GetSerialNumber()) << " -- " << desc;
			ASSERT_EQ(pollValue6f, obj.m_mainGameMeterStepdown.GetMeterLongPoll(gCI)) << " -- " << desc;
		}

		/// <summary>
		/// Test current values for:
		///     CSASProtocol::m_sas.GetVersion()
		///     CSASProtocol::m_sas.GetMajorVersion()
		///     CSASProtocol::CSASInterface.GetVersionString()
		///     CSASProtocol::m_sasInfo.GetSASVersion()
		///     CSASProtocol::m_mainGameMeterStepdown.GetMeterLongPoll
		/// </summary>
		static void AssertVersion(const CSASProtocol &obj, LPCSTR sasVersion, LPCTSTR serialNumber, byte gciPollValue, LPCSTR desc)
		{
			ASSERT_EQ(3, (size_t)CSASInterface::VERSION_LENGTH) << " -- " << desc;
			ASSERT_EQ(0, strncmp(sasVersion, (LPCSTR)obj.m_sas->GetVersion(), (size_t)CSASInterface::VERSION_LENGTH)) << " -- " << desc;
			ASSERT_EQ(sasVersion[0], obj.m_sas->GetMajorVersion()) << " -- " << desc;
			ASSERT_STREQ(CString(sasVersion), obj.m_sas->GetVersionString()) << " -- " << desc;
			ASSERT_STREQ(CString(sasVersion), obj.m_sasInfo->GetSASVersion()) << " -- " << desc;
			ASSERT_STREQ(CString(serialNumber), obj.m_sasInfo->GetSerialNumber()) << " -- " << desc;
			ASSERT_EQ(gciPollValue, obj.m_mainGameMeterStepdown.GetMeterLongPoll(gCI)) << " -- " << desc;
		}

		/// <summary>
		/// Functional test for:
		///     CSASProtocol::GetSASVersion()
		/// Verify default values after calling the constructor and without calling GetSASVersion().
		/// </summary>
		TEST_METHOD(GetSASVersion_DefaultTest)
		{
			// Data.
			CVersionData data;

			// Setup.
			CTestSetup test;
			test.Init();
			AssertSetupVersion(test.m_obj, data.versionSetup, data.versionSetupGames, "setup");
		}

		/// <summary>
		/// Functional test for:
		///     CSASProtocol::GetSASVersion()
		/// Verify that the GetSASVersion() provides default results when there is no response from the EGM.
		/// </summary>
		TEST_METHOD(GetSASVersion_NoResponseTest)
		{
			// Data.
			CVersionData data;

			// Setup.
			CTestSetup test;
			test.Init();
			AssertSetupVersion(test.m_obj, data.versionSetup, data.versionSetupGames, "setup");

			// Perform operation(s) and test(s).
			test.OnCall(data.sasVersionPollValue, nullptr, 0);
			test.m_obj.GetSASVersion();
			AssertVersion(test.m_obj, data.versionSetup, data.serialNumber601, pollValue6f, "first response");
			AssertRemoveMessages_GetSASVersion(test.m_gcfToInternalsQueue, CString(data.versionSetup), "first response");
		}

		/// <summary>
		/// Functional test for:
		///     CSASProtocol::GetSASVersion()
		/// Verify that the GetSASVersion() provides the version from the EGM when there is a response from the EGM.
		/// </summary>
		TEST_METHOD(GetSASVersion_Response601Test)
		{
			// Data.
			CVersionData data;

			// Setup.
			CTestSetup test;
			test.Init();
			AssertSetupVersion(test.m_obj, data.versionSetup, data.versionSetupGames, "setup");

			// Perform operation(s) and test(s).
			test.OnCall(data.sasVersionPollValue, data.receiveData601.data(), data.receiveData601.size());
			test.m_obj.GetSASVersion();
			AssertVersion(test.m_obj, data.version601, data.serialNumber601, pollValue6f, "first response");
			AssertRemoveMessages_GetSASVersion(test.m_gcfToInternalsQueue, CString(data.version601), "first response");
		}

		/// <summary>
		/// Functional test for:
		///     CSASProtocol::GetSASVersion()
		/// Verify that the GetSASVersion() provides the version from the EGM when there is a response from the EGM.
		/// Verify that the meter stepdown occurs and 6f long polls are no longer supported.
		/// </summary>
		TEST_METHOD(GetSASVersion_Response502Test)
		{
			// Data.
			CVersionData data;

			// Setup.
			CTestSetup test;
			test.Init();
			AssertSetupVersion(test.m_obj, data.versionSetup, data.versionSetupGames, "setup");

			// Perform operation(s) and test(s).
			test.OnCall(data.sasVersionPollValue, data.receiveData502.data(), data.receiveData502.size());
			test.m_obj.GetSASVersion();
			AssertVersion(test.m_obj, data.version502, data.serialNumber502, pollValue2f, "first response");
			AssertRemoveMessages_GetSASVersion(test.m_gcfToInternalsQueue, CString(data.version502), "first response");
		}

		/// <summary>
		/// Functional test for:
		///     CSASProtocol::GetSASVersion()
		/// Verify that the GetSASVersion() provides the version from the EGM when there are different responses from the EGM.
		/// Verify that the meter stepdown occurs and 6f long polls are no longer supported, even when the version changes back to 601.
		/// </summary>
		TEST_METHOD(GetSASVersion_Response502Response601Test)
		{
			// Data.
			CVersionData data;

			// Setup.
			CTestSetup test;
			test.Init();
			AssertSetupVersion(test.m_obj, data.versionSetup, data.versionSetupGames, "setup");

			// Perform operation(s) and test(s).
			test.OnCall(data.sasVersionPollValue, data.receiveData502.data(), data.receiveData502.size());
			test.m_obj.GetSASVersion();
			AssertVersion(test.m_obj, data.version502, data.serialNumber502, pollValue2f, "first response");
			AssertRemoveMessages_GetSASVersion(test.m_gcfToInternalsQueue, CString(data.version502), "first response");

			// Perform operation(s) and test(s).
			test.OnCall(data.sasVersionPollValue, data.receiveData601.data(), data.receiveData601.size());
			test.m_obj.GetSASVersion();
			AssertVersion(test.m_obj, data.version601, data.serialNumber601, pollValue2f, "second response");
			AssertRemoveMessages_GetSASVersion(test.m_gcfToInternalsQueue, CString(data.version601), "second response");
		}

		/// <summary>
		/// Functional test for:
		///     CSASProtocol::GetSASVersion()
		/// Verify that the GetSASVersion() provides the version from the EGM when there is a response from the EGM.
		/// </summary>
		TEST_METHOD(GetSASVersion_Response403Test)
		{
			// Data.
			CVersionData data;

			// Setup.
			CTestSetup test;
			test.Init();
			AssertSetupVersion(test.m_obj, data.versionSetup, data.versionSetupGames, "setup");

			// Perform operation(s) and test(s).
			test.OnCall(data.sasVersionPollValue, data.receiveData403.data(), data.receiveData403.size());
			test.m_obj.GetSASVersion();
			AssertVersion(test.m_obj, data.version403, data.serialNumber403, pollValue6f, "first response");
			AssertRemoveMessages_GetSASVersion(test.m_gcfToInternalsQueue, CString(data.version403), "first response");
		}

		/// <summary>
		/// Functional test for:
		///     CSASProtocol::GetSASVersion()
		/// Verify that the GetSASVersion() provides the version from the EGM when there is a response from the EGM.
		/// </summary>
		TEST_METHOD(GetSASVersion_Response340Test)
		{
			// Data.
			CVersionData data;

			// Setup.
			CTestSetup test;
			test.Init();
			AssertSetupVersion(test.m_obj, data.versionSetup, data.versionSetupGames, "setup");

			// Perform operation(s) and test(s).
			test.OnCall(data.sasVersionPollValue, data.receiveData340.data(), data.receiveData340.size());
			test.m_obj.GetSASVersion();
			AssertVersion(test.m_obj, data.version340, data.serialNumber340, pollValue6f, "first response");
			AssertRemoveMessages_GetSASVersion(test.m_gcfToInternalsQueue, CString(data.version340), "first response");
		}

		/// <summary>
		/// Functional test for:
		///     CSASProtocol::GetSASVersion()
		/// Verify that the GetSASVersion() provides the version(s) from the EGM when there are the same responses from the EGM.
		/// </summary>
		TEST_METHOD(GetSASVersion_ResponseSameResponseTest)
		{
			// Data.
			CVersionData data;

			// Setup.
			CTestSetup test;
			test.Init();
			AssertSetupVersion(test.m_obj, data.versionSetup, data.versionSetupGames, "setup");

			// Perform operation(s) and test(s).
			test.OnCall(data.sasVersionPollValue, data.receiveData601.data(), data.receiveData601.size());
			test.m_obj.GetSASVersion();
			AssertVersion(test.m_obj, data.version601, data.serialNumber601, pollValue6f, "first response");
			AssertRemoveMessages_GetSASVersion(test.m_gcfToInternalsQueue, CString(data.version601), "first response");

			// Perform operation(s) and test(s).
			test.OnCall(data.sasVersionPollValue, data.receiveData601.data(), data.receiveData601.size());
			test.m_obj.GetSASVersion();
			AssertVersion(test.m_obj, data.version601, data.serialNumber601, pollValue6f, "second response");
			AssertRemoveMessages_GetSASVersion(test.m_gcfToInternalsQueue, CString(data.version601), "second response");
		}

		/// <summary>
		/// Functional test for:
		///     CSASProtocol::GetSASVersion()
		/// Verify that the GetSASVersion() provides the versions from the EGM when there are different responses from the EGM.
		/// </summary>
		TEST_METHOD(GetSASVersion_ResponseDiffResponseTest)
		{
			// Data.
			CVersionData data;

			// Setup.
			CTestSetup test;
			test.Init();
			AssertSetupVersion(test.m_obj, data.versionSetup, data.versionSetupGames, "setup");

			// Perform operation(s) and test(s).
			test.OnCall(data.sasVersionPollValue, data.receiveData601.data(), data.receiveData601.size());
			test.m_obj.GetSASVersion();
			AssertVersion(test.m_obj, data.version601, data.serialNumber601, pollValue6f, "first response");
			AssertRemoveMessages_GetSASVersion(test.m_gcfToInternalsQueue, CString(data.version601), "first response");

			// Perform operation(s) and test(s).
			test.OnCall(data.sasVersionPollValue, data.receiveData340.data(), data.receiveData340.size());
			test.m_obj.GetSASVersion();
			AssertVersion(test.m_obj, data.version340, data.serialNumber340, pollValue6f, "second response");
			AssertRemoveMessages_GetSASVersion(test.m_gcfToInternalsQueue, CString(data.version340), "second response");
		}

		/// <summary>
		/// Functional test for:
		///     CSASProtocol::GetSASVersion()
		/// Verify that the GetSASVersion() provides the version from the EGM results when there is no subsequent response from the EGM.
		/// </summary>
		TEST_METHOD(GetSASVersion_ResponseNoResponseTest)
		{
			// Data.
			CVersionData data;

			// Setup.
			CTestSetup test;
			test.Init();
			AssertSetupVersion(test.m_obj, data.versionSetup, data.versionSetupGames, "setup");

			// Perform operation(s) and test(s).
			test.OnCall(data.sasVersionPollValue, data.receiveData601.data(), data.receiveData601.size());
			test.m_obj.GetSASVersion();
			AssertVersion(test.m_obj, data.version601, data.serialNumber601, pollValue6f, "first response");
			AssertRemoveMessages_GetSASVersion(test.m_gcfToInternalsQueue, CString(data.version601), "first response");

			// Perform operation(s) and test(s).
			test.OnCall(data.sasVersionPollValue, nullptr, 0);
			test.m_obj.GetSASVersion();
			AssertVersion(test.m_obj, data.version601, data.serialNumber601, pollValue6f, "second response");
			AssertRemoveMessages_GetSASVersion(test.m_gcfToInternalsQueue, CString(data.version601), "second response");
		}
// #pragma endregion GetSASVersion
	};

	CString SASProtocol2::m_gmockFailure;
	SASProtocol2::CTersePrinter SASProtocol2::m_tersePrinter;

	CSASInterface *SASProtocol2::CProtocol::m_lastSASInterface = nullptr;
}
#endif //TODO-PORT