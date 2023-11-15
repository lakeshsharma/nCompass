#if 000//TODO-PORT
#include "stdafx.h"
#include "ProgressiveConnectionMock.h"
#include "ProgressiveDataConsumerMock.h"
#include "RapidJsonTestHelper.h"
#include "ProgressiveLevelTestHelpers.h"

// Unit testing includes.
#include "UnitTestingOperatingSystem.h"
#include "UnitTestingToString.h"
#include "UnitTestingUtils.h"
#include "UnitTestingXmlData.h"
#include "UnitTestingXmlLite.h"
#include "TestUtil.h"

// AssertResult macros for calling CUnitTestingUtils from a test method.
#define AssertResult3(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, m_testIndex, __TFUNCTION__, __LINE__);
#define AssertResult2(result, expected) AssertResult3(result, expected, _T(#result))

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace activemq::commands;
using namespace cms;
using ::testing::Eq;
using ::testing::Invoke;
using ::testing::Mock;
using ::testing::Ne;
using ::testing::Return;
using ::testing::_;

namespace SentinelNativeUnitTests
{
	MATCHER_P(CheckBonusingAvailableSessionId, sessionIdExpected, "")
	{
		return arg->GetSessionId() == sessionIdExpected;
	}

	MATCHER_P2(CheckSessionId, hasSessionIdExpected, sessionIdExpected, "")
	{
		int sessionId;
		bool hasSessionId(arg->GetSessionId(sessionId));
		return hasSessionId == hasSessionIdExpected && sessionId == sessionIdExpected;
	}

	MATCHER_P2(CheckJackpotPaidMessage, statusExpected, statusInfoExpected, "")
	{
		rapidjson::Document jsonDoc;
		if (jsonDoc.Parse(arg.c_str()).HasParseError())
			throw JsonException(jsonDoc.GetParseError(), nullptr, L"Parse");

		const char *status(JsonGetString(jsonDoc, pKeyStatus));
		const char *statusInfo(JsonGetString(jsonDoc, pKeyStatusInfo));
		return 0 == strcmp(status, statusExpected) && 0 == strcmp(statusInfo, statusInfoExpected);
	}

	TEST_CLASS(ProgressiveConnectionUnitTests2)
	{
		// Class member(s) constants used by test (data) methods.
		static const CString paltronicsCertificatePath;

		static const bool m_coinIn5IsHearbeat = false;
		static const bool m_gameEnd5IsHearbeat = false;

		// Expected value(s).
		static const int m_currentGameNumberDefault = -1;
		static const __int64 m_currentDenominationDefault = 1;

		static const DWORD m_pingIntervalExpected = 15000;
		static const DWORD m_pingIntervalMinimumExpected = 5000;
		static const DWORD m_pingIntervalMaximumExpected = DWORD_MAX;

		static const bool m_dependentCommunicationsOKExpected = false;
		static const bool m_multicastActiveExpected = true;

		// Class member variable(s).
		int m_testIndex;
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
		/// Test static class members.
		/// </summary>
		void TestStaticMembers()
		{
			// Test static default value(s).
			Assert::AreEqual(m_currentGameNumberDefault, (int)CGame::DEFAULT_GAME_NUMBER, _T("m_currentGameNumberDefault"));
			Assert::AreEqual(m_currentDenominationDefault, (__int64)CGame::DEFAULT_DENOMINATION_MULTIPLIER, _T("m_currentDenominationDefault"));

			AssertResult2(CProgressiveConfig::DEFAULT_PING_INTERVAL, m_pingIntervalExpected);
			AssertResult2(CProgressiveConfig::MINIMUM_PING_INTERVAL, m_pingIntervalMinimumExpected);
			AssertResult2(CProgressiveConfig::MAXIMUM_PING_INTERVAL, m_pingIntervalMaximumExpected);
		}

		/// <summary>
		/// Initialize and/or test static class members.
		/// </summary>
		TEST_CLASS_INITIALIZE(ProgressiveConnectionUnitTests2_Init)
		{
			// Class initialization code.
			::testing::UnitTest::GetInstance()->listeners().Append(&m_tersePrinter);
		}

		/// <summary>
		/// Cleanup static class member(s).
		/// </summary>
		TEST_CLASS_CLEANUP(ProgressiveConnectionUnitTests2_Deinit)
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
			m_testIndex = 0;
			m_gmockFailure.Empty();

			// Perform static tests here so each test method gets a useful error message.
			TestStaticMembers();
		}

		/// <summary>
		/// Cleanup class member(s) before each test method is run.
		/// </summary>
		TEST_METHOD_CLEANUP(Cleanup)
		{
			// Report a gmock failure reported by our event listener, if one occurred.
			if (!m_gmockFailure.IsEmpty())
			{
				Assert::Fail(m_gmockFailure);
			}

			// Test method cleanup code.
		}

// #pragma region CProgressiveConnection::m_haveGameData
		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		/// Test for default value.
		/// </summary>
		TEST_METHOD(HaveGameData_ProgressiveConnection)
		{
			// Data.
			const bool haveGameDataExpected(false);

			// Setup.
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;

			// Perform operation(s) and test(s).
			CProgressiveConnection progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);
			Assert::AreEqual(haveGameDataExpected, progressiveConnection.m_haveGameData);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::UpdateGameData()
		/// Test for default value still present when gameData is nullptr.
		/// </summary>
		TEST_METHOD(HaveGameData_UpdateGameData_GameDataNull)
		{
			// Data.
			const bool changeExpected(false);
			const bool haveGameDataExpected(false);
			const CGameEndData *gameData(nullptr);

			// Setup.
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;

			// Perform operation(s) and test(s).
			CProgressiveConnection progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);
			Assert::AreEqual(changeExpected, progressiveConnection.UpdateGameData(gameData), L"progressiveConnection.UpdateGameData(gameData)");
			Assert::AreEqual(haveGameDataExpected, progressiveConnection.m_haveGameData, L"progressiveConnection.m_haveGameData");
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::UpdateGameData()
		/// Test for default value still present when gameData->GetCurrentGame() is nullptr.
		/// </summary>
		TEST_METHOD(HaveGameData_UpdateGameData_CurrentGameNull)
		{
			// Data.
			const bool changeExpected(false);
			const bool haveGameDataExpected(false);
			unique_ptr<CGameEndData> gameData(new CGameEndData(nullptr, 0, 0, 0, NULL, 1));

			// Setup.
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;

			// Perform operation(s) and test(s).
			CProgressiveConnection progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);
			Assert::AreEqual(changeExpected, progressiveConnection.UpdateGameData(gameData.get()), L"progressiveConnection.UpdateGameData(gameData.get()");
			Assert::AreEqual(haveGameDataExpected, progressiveConnection.m_haveGameData);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::UpdateGameData()
		/// Test for true value present when gameData->GetCurrentGame() is not nullptr.
		/// </summary>
		TEST_METHOD(HaveGameData_UpdateGameData)
		{
			// Data.
			const bool changeExpected(true);
			const bool haveGameDataExpected(true);
			unique_ptr<CGameEndData> gameData(new CGameEndData(new CGame(MEMORY_NONE), 0, 0, 0, NULL, 1));

			// Setup.
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;

			// Perform operation(s) and test(s).
			CProgressiveConnection progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);
			Assert::AreEqual(changeExpected, progressiveConnection.UpdateGameData(gameData.get()), L"progressiveConnection.UpdateGameData(gameData.get()");
			Assert::AreEqual(haveGameDataExpected, progressiveConnection.m_haveGameData);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::UpdateGameData()
		/// Test for true value still present when gameData is nullptr on second call.
		/// </summary>
		TEST_METHOD(HaveGameData_UpdateGameData2_GameDataNull)
		{
			// Data.
			const bool changeExpected(false);
			const bool haveGameDataExpected(true);
			const CGameEndData *gameData(nullptr);

			// Setup.
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;
			CProgressiveConnection progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);
			unique_ptr<CGameEndData> gameDataSetup(new CGameEndData(new CGame(MEMORY_NONE), 0, 0, 0, NULL, 1));
			progressiveConnection.UpdateGameData(gameDataSetup.get());
			Assert::AreEqual(haveGameDataExpected, progressiveConnection.m_haveGameData, L"Setup"); // Verify setup.

			// Perform operation(s) and test(s).
			Assert::AreEqual(changeExpected, progressiveConnection.UpdateGameData(gameData), L"progressiveConnection.UpdateGameData(gameData)");
			Assert::AreEqual(haveGameDataExpected, progressiveConnection.m_haveGameData, L"Test");
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::UpdateGameData()
		/// Test for true value still present when gameData->GetCurrentGame() is nullptr on second call.
		/// </summary>
		TEST_METHOD(HaveGameData_UpdateGameData2_CurrentGameNull)
		{
			// Data.
			const bool changeExpected(false);
			const bool haveGameDataExpected(true);
			unique_ptr<CGameEndData> gameData(new CGameEndData(nullptr, 0, 0, 0, NULL, 1));

			// Setup.
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;
			CProgressiveConnection progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);
			unique_ptr<CGameEndData> gameDataSetup(new CGameEndData(new CGame(MEMORY_NONE), 0, 0, 0, NULL, 1));
			progressiveConnection.UpdateGameData(gameDataSetup.get());
			Assert::AreEqual(haveGameDataExpected, progressiveConnection.m_haveGameData, L"Setup"); // Verify setup.

			// Perform operation(s) and test(s).
			Assert::AreEqual(changeExpected, progressiveConnection.UpdateGameData(gameData.get()), L"progressiveConnection.UpdateGameData(gameData.get())");
			Assert::AreEqual(haveGameDataExpected, progressiveConnection.m_haveGameData, L"Test");
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::UpdateGameData()
		/// Test for true value present when gameData->GetCurrentGame() is not nullptr.
		/// </summary>
		TEST_METHOD(HaveGameData_UpdateGameData2)
		{
			// Data.
			const bool changeExpected(false);
			const bool haveGameDataExpected(true);
			unique_ptr<CGameEndData> gameData(new CGameEndData(new CGame(MEMORY_NONE), 0, 0, 0, NULL, 1));

			// Setup.
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;
			CProgressiveConnection progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);
			unique_ptr<CGameEndData> gameDataSetup(new CGameEndData(new CGame(MEMORY_NONE), 0, 0, 0, NULL, 1));
			progressiveConnection.UpdateGameData(gameDataSetup.get());
			Assert::AreEqual(haveGameDataExpected, progressiveConnection.m_haveGameData, L"Setup"); // Verify setup.

			// Perform operation(s) and test(s).
			Assert::AreEqual(changeExpected, progressiveConnection.UpdateGameData(gameData.get()), L"progressiveConnection.UpdateGameData(gameData.get())");
			Assert::AreEqual(haveGameDataExpected, progressiveConnection.m_haveGameData, L"Test");
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::ProcessGameData()
		/// Test for default value still present when gameData is nullptr.
		/// </summary>
		TEST_METHOD(HaveGameData_ProcessGameData_GameDataNull)
		{
			// Data.
			const bool changeExpected(false);
			const bool haveGameDataExpected(false);
			const CGameEndData *gameData(nullptr);

			// Setup.
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;

			// Perform operation(s) and test(s).
			CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);
			EXPECT_CALL(progressiveConnection, SendCardStatus()).Times(0);
			progressiveConnection.ProcessGameData(gameData);
			Assert::AreEqual(haveGameDataExpected, progressiveConnection.m_haveGameData, L"progressiveConnection.m_haveGameData");

			Assert::IsTrue(Mock::VerifyAndClearExpectations(&dataConsumer));
			Assert::IsTrue(Mock::VerifyAndClearExpectations(&progressiveConnection));
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::ProcessGameData()
		/// Test for default value still present when gameData->GetCurrentGame() is nullptr.
		/// </summary>
		TEST_METHOD(HaveGameData_ProcessGameData_CurrentGameNull)
		{
			// Data.
			const bool changeExpected(false);
			const bool haveGameDataExpected(false);
			unique_ptr<CGameEndData> gameData(new CGameEndData(nullptr, 0, 0, 0, NULL, 1));

			// Setup.
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;

			// Perform operation(s) and test(s).
			CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);
			EXPECT_CALL(progressiveConnection, SendCardStatus()).Times(0);
			progressiveConnection.ProcessGameData(gameData.get());
			Assert::AreEqual(haveGameDataExpected, progressiveConnection.m_haveGameData);

			Assert::IsTrue(Mock::VerifyAndClearExpectations(&dataConsumer));
			Assert::IsTrue(Mock::VerifyAndClearExpectations(&progressiveConnection));
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::ProcessGameData()
		/// Test for true value present when gameData->GetCurrentGame() is not nullptr.
		/// </summary>
		TEST_METHOD(HaveGameData_ProcessGameData)
		{
			// Data.
			const bool changeExpected(true);
			const bool haveGameDataExpected(true);
			unique_ptr<CGameEndData> gameData(new CGameEndData(new CGame(MEMORY_NONE), 0, 0, 0, NULL, 1));

			// Setup.
			CProgressiveConfig progressiveConfig;
			progressiveConfig.SetActive(false);
			CProgressiveDataConsumerMock dataConsumer;

			// Perform operation(s) and test(s).
			CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);
			EXPECT_CALL(progressiveConnection, SendCardStatus()).Times(1);
			progressiveConnection.ProcessGameData(gameData.get());
			Assert::AreEqual(haveGameDataExpected, progressiveConnection.m_haveGameData);

			Assert::IsTrue(Mock::VerifyAndClearExpectations(&dataConsumer));
			Assert::IsTrue(Mock::VerifyAndClearExpectations(&progressiveConnection));
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::ProcessGameData()
		/// Test for true value still present when gameData is nullptr on second call.
		/// </summary>
		TEST_METHOD(HaveGameData_ProcessGameData2_GameDataNull)
		{
			// Data.
			const bool changeExpected(false);
			const bool haveGameDataExpected(true);
			const CGameEndData *gameData(nullptr);

			// Setup.
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;
			CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);
			EXPECT_CALL(progressiveConnection, SendCardStatus()).Times(1);
			unique_ptr<CGameEndData> gameDataSetup(new CGameEndData(new CGame(MEMORY_NONE), 0, 0, 0, NULL, 1));
			progressiveConnection.ProcessGameData(gameDataSetup.get());
			Assert::AreEqual(haveGameDataExpected, progressiveConnection.m_haveGameData, L"Setup"); // Verify setup.

			// Perform operation(s) and test(s).
			progressiveConnection.ProcessGameData(gameData);
			Assert::AreEqual(haveGameDataExpected, progressiveConnection.m_haveGameData, L"Test");

			Assert::IsTrue(Mock::VerifyAndClearExpectations(&dataConsumer));
			Assert::IsTrue(Mock::VerifyAndClearExpectations(&progressiveConnection));
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::ProcessGameData()
		/// Test for true value still present when gameData->GetCurrentGame() is nullptr on second call.
		/// </summary>
		TEST_METHOD(HaveGameData_ProcessGameData2_CurrentGameNull)
		{
			// Data.
			const bool changeExpected(false);
			const bool haveGameDataExpected(true);
			unique_ptr<CGameEndData> gameData(new CGameEndData(nullptr, 0, 0, 0, NULL, 1));

			// Setup.
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;
			CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);
			EXPECT_CALL(progressiveConnection, SendCardStatus()).Times(1);
			unique_ptr<CGameEndData> gameDataSetup(new CGameEndData(new CGame(MEMORY_NONE), 0, 0, 0, NULL, 1));
			progressiveConnection.ProcessGameData(gameDataSetup.get());
			Assert::AreEqual(haveGameDataExpected, progressiveConnection.m_haveGameData, L"Setup"); // Verify setup.

			// Perform operation(s) and test(s).
			progressiveConnection.ProcessGameData(gameData.get());
			Assert::AreEqual(haveGameDataExpected, progressiveConnection.m_haveGameData, L"Test");

			Assert::IsTrue(Mock::VerifyAndClearExpectations(&dataConsumer));
			Assert::IsTrue(Mock::VerifyAndClearExpectations(&progressiveConnection));
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::ProcessGameData()
		/// Test for true value present when gameData->GetCurrentGame() is not nullptr.
		/// </summary>
		TEST_METHOD(HaveGameData_ProcessGameData2)
		{
			// Data.
			const bool changeExpected(false);
			const bool haveGameDataExpected(true);
			unique_ptr<CGameEndData> gameData(new CGameEndData(new CGame(MEMORY_NONE), 0, 0, 0, NULL, 1));

			// Setup.
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;
			CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);
			EXPECT_CALL(progressiveConnection, SendCardStatus()).Times(1);
			unique_ptr<CGameEndData> gameDataSetup(new CGameEndData(new CGame(MEMORY_NONE), 0, 0, 0, NULL, 1));
			progressiveConnection.ProcessGameData(gameDataSetup.get());
			Assert::AreEqual(haveGameDataExpected, progressiveConnection.m_haveGameData, L"Setup"); // Verify setup.

			// Perform operation(s) and test(s).
			progressiveConnection.ProcessGameData(gameData.get());
			Assert::AreEqual(haveGameDataExpected, progressiveConnection.m_haveGameData, L"Test");

			Assert::IsTrue(Mock::VerifyAndClearExpectations(&dataConsumer));
			Assert::IsTrue(Mock::VerifyAndClearExpectations(&progressiveConnection));
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::ProcessGameData()
		///     CProgressiveConnection::SendCardStatus()
		/// Test for true value present when gameData->GetCurrentGame() is not nullptr.
		/// </summary>
		TEST_METHOD(HaveGameData_SendCardStatus_ProcessGameData2)
		{
			// Data.
			const bool changeExpected(false);
			const bool haveGameDataExpected(true);
			unique_ptr<CGameEndData> gameData(new CGameEndData(new CGame(MEMORY_NONE), 0, 0, 0, NULL, 1));

			// Setup.
			CConnection connection;
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;
			CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);
			ON_CALL(progressiveConnection, SendCardStatus()).WillByDefault(Invoke(&progressiveConnection, &CProgressiveConnectionMock::BaseSendCardStatus));
			EXPECT_CALL(progressiveConnection, SendCardStatus()).Times(1);
			EXPECT_CALL(progressiveConnection, SendMessageToOneLink(ToOneLink::ICardStatus, _)).Times(1);
			progressiveConnection.LoadDeviceConnectionData(&connection);
			Assert::IsTrue(progressiveConnection.IsMessageFactoryCreated(), L"progressiveConnection.IsMessageFactoryCreated()"); // Verify setup.
			unique_ptr<CGameEndData> gameDataSetup(new CGameEndData(new CGame(MEMORY_NONE), 0, 0, 0, NULL, 1));
			progressiveConnection.ProcessGameData(gameDataSetup.get());
			Assert::AreEqual(haveGameDataExpected, progressiveConnection.m_haveGameData, L"Setup"); // Verify setup.

			// Perform operation(s) and test(s).
			progressiveConnection.ProcessGameData(gameData.get());
			Assert::AreEqual(haveGameDataExpected, progressiveConnection.m_haveGameData, L"Test");

			Assert::IsTrue(Mock::VerifyAndClearExpectations(&dataConsumer));
			Assert::IsTrue(Mock::VerifyAndClearExpectations(&progressiveConnection));
		}
// #pragma endregion CProgressiveConnection::m_haveGameData

// #pragma region CProgressiveConnection::m_currentGameNumber
		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		/// Test for default value.
		/// </summary>
		TEST_METHOD(CurrentGameNumber_ProgressiveConnection)
		{
			// Data.
			const int currentGameNumberExpected(m_currentGameNumberDefault);

			// Setup.
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;

			// Perform operation(s) and test(s).
			CProgressiveConnection progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);
			Assert::AreEqual(currentGameNumberExpected, progressiveConnection.m_currentGameNumber);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::UpdateGameData()
		/// Test for default value still present when gameData is nullptr.
		/// </summary>
		TEST_METHOD(CurrentGameNumber_UpdateGameData_GameDataNull)
		{
			// Data.
			const bool changeExpected(false);
			const int currentGameNumberExpected(m_currentGameNumberDefault);
			const CGameEndData *gameData(nullptr);

			// Setup.
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;

			// Perform operation(s) and test(s).
			CProgressiveConnection progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);
			Assert::AreEqual(changeExpected, progressiveConnection.UpdateGameData(gameData), L"progressiveConnection.UpdateGameData(gameData)");
			Assert::AreEqual(currentGameNumberExpected, progressiveConnection.m_currentGameNumber);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::UpdateGameData()
		/// Test for default value still present when gameData->GetCurrentGame() is nullptr.
		/// </summary>
		TEST_METHOD(CurrentGameNumber_UpdateGameData_CurrentGameNull)
		{
			// Data.
			const bool changeExpected(false);
			const int currentGameNumberExpected(m_currentGameNumberDefault);
			unique_ptr<CGameEndData> gameData(new CGameEndData(nullptr, 0, 0, 0, NULL, 1));

			// Setup.
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;

			// Perform operation(s) and test(s).
			CProgressiveConnection progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);
			Assert::AreEqual(changeExpected, progressiveConnection.UpdateGameData(gameData.get()), L"progressiveConnection.UpdateGameData(gameData.get()");
			Assert::AreEqual(currentGameNumberExpected, progressiveConnection.m_currentGameNumber);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::UpdateGameData()
		/// Test for non-default value present when gameData->GetCurrentGame() is not nullptr.
		/// </summary>
		TEST_METHOD(CurrentGameNumber_UpdateGameData)
		{
			// Data.
			const bool changeExpected(true);
			const int currentGameNumberExpected(57);
			CGame *game(new CGame(MEMORY_NONE));
			game->SetGameNumber(currentGameNumberExpected);
			AssertResult2((int)game->GetGameNumber(), currentGameNumberExpected); // Verify data.
			unique_ptr<CGameEndData> gameData(new CGameEndData(game, 0, 0, 0, NULL, 1));

			// Setup.
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;

			// Perform operation(s) and test(s).
			CProgressiveConnection progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);
			Assert::AreEqual(changeExpected, progressiveConnection.UpdateGameData(gameData.get()), L"progressiveConnection.UpdateGameData(gameData.get()");
			Assert::AreEqual(currentGameNumberExpected, progressiveConnection.m_currentGameNumber, L"progressiveConnection.m_currentGameNumber");
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::UpdateGameData()
		/// Test for non-default value present when gameData->GetCurrentGame() is not nullptr.
		/// Test for setting non-default value back to 0 or no game currently selected.
		/// </summary>
		TEST_METHOD(CurrentGameNumber_UpdateGameData2_None)
		{
			// Setup.
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;
			CProgressiveConnection progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);

			// Set to non-default value.
			{
				// Data.
				const bool changeExpected(true);
				const int currentGameNumberExpected(57);
				CGame *game(new CGame(MEMORY_NONE));
				game->SetGameNumber(currentGameNumberExpected);
				AssertResult2((int)game->GetGameNumber(), currentGameNumberExpected); // Verify data.
				unique_ptr<CGameEndData> gameData(new CGameEndData(game, 0, 0, 0, NULL, 1));

				// Perform operation(s) and test(s).
				Assert::AreEqual(changeExpected, progressiveConnection.UpdateGameData(gameData.get()), L"progressiveConnection.UpdateGameData(gameData.get()");
				Assert::AreEqual(currentGameNumberExpected, progressiveConnection.m_currentGameNumber);
			}

			// Set back to 0 or no game currently selected.
			{
				// Data.
				const bool changeExpected(true);
				const int currentGameNumberExpected(68);
				CGame *game(new CGame(MEMORY_NONE));
				game->SetGameNumber(currentGameNumberExpected);
				AssertResult2((int)game->GetGameNumber(), currentGameNumberExpected); // Verify data.
				unique_ptr<CGameEndData> gameData(new CGameEndData(game, 0, 0, 0, NULL, 1));

				// Perform operation(s) and test(s).
				Assert::AreEqual(changeExpected, progressiveConnection.UpdateGameData(gameData.get()), L"progressiveConnection.UpdateGameData(gameData.get()");
				Assert::AreEqual(currentGameNumberExpected, progressiveConnection.m_currentGameNumber);
			}
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::UpdateGameData()
		/// Test for non-default value present when gameData->GetCurrentGame() is not nullptr.
		/// Test for setting a different non-default value after setting a non-default value.
		/// </summary>
		TEST_METHOD(CurrentGameNumber_UpdateGameData2_Diff)
		{
			// Setup.
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;
			CProgressiveConnection progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);

			// Set to non-default value.
			{
				// Data.
				const bool changeExpected(true);
				const int currentGameNumberExpected(57);
				CGame *game(new CGame(MEMORY_NONE));
				game->SetGameNumber(currentGameNumberExpected);
				AssertResult2((int)game->GetGameNumber(), currentGameNumberExpected); // Verify data.
				unique_ptr<CGameEndData> gameData(new CGameEndData(game, 0, 0, 0, NULL, 1));

				// Perform operation(s) and test(s).
				Assert::AreEqual(changeExpected, progressiveConnection.UpdateGameData(gameData.get()), L"progressiveConnection.UpdateGameData(gameData.get()");
				Assert::AreEqual(currentGameNumberExpected, progressiveConnection.m_currentGameNumber);
			}

			// Set back to 0 or no game currently selected.
			{
				// Data.
				const bool changeExpected(true);
				const int currentGameNumberExpected(68);
				CGame *game(new CGame(MEMORY_NONE));
				game->SetGameNumber(currentGameNumberExpected);
				AssertResult2((int)game->GetGameNumber(), currentGameNumberExpected); // Verify data.
				unique_ptr<CGameEndData> gameData(new CGameEndData(game, 0, 0, 0, NULL, 1));

				// Perform operation(s) and test(s).
				Assert::AreEqual(changeExpected, progressiveConnection.UpdateGameData(gameData.get()), L"progressiveConnection.UpdateGameData(gameData.get()");
				Assert::AreEqual(currentGameNumberExpected, progressiveConnection.m_currentGameNumber);
			}
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::UpdateGameData()
		/// Test for non-default value present when gameData->GetCurrentGame() is not nullptr.
		/// Test for setting the same non-default value after setting a non-default value.
		/// </summary>
		TEST_METHOD(CurrentGameNumber_UpdateGameData2_Same)
		{
			// Setup.
			const int currentGameNumberExpected(57);
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;
			CProgressiveConnection progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);

			// Set to non-default value.
			{
				// Data.
				const bool changeExpected(true);
				CGame *game(new CGame(MEMORY_NONE));
				game->SetGameNumber(currentGameNumberExpected);
				AssertResult2((int)game->GetGameNumber(), currentGameNumberExpected); // Verify data.
				unique_ptr<CGameEndData> gameData(new CGameEndData(game, 0, 0, 0, NULL, 1));

				// Perform operation(s) and test(s).
				Assert::AreEqual(changeExpected, progressiveConnection.UpdateGameData(gameData.get()), L"progressiveConnection.UpdateGameData(gameData.get()");
				Assert::AreEqual(currentGameNumberExpected, progressiveConnection.m_currentGameNumber);
			}

			// Set back to 0 or no game currently selected.
			{
				// Data.
				const bool changeExpected(false);
				CGame *game(new CGame(MEMORY_NONE));
				game->SetGameNumber(currentGameNumberExpected);
				AssertResult2((int)game->GetGameNumber(), currentGameNumberExpected); // Verify data.
				unique_ptr<CGameEndData> gameData(new CGameEndData(game, 0, 0, 0, NULL, 1));

				// Perform operation(s) and test(s).
				Assert::AreEqual(changeExpected, progressiveConnection.UpdateGameData(gameData.get()), L"progressiveConnection.UpdateGameData(gameData.get()");
				Assert::AreEqual(currentGameNumberExpected, progressiveConnection.m_currentGameNumber);
			}
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::UpdateGameData()
		/// Test a range of values when gameData->GetCurrentGame() is not nullptr.
		/// Use a differnt CProgressiveConnection instance for each test.
		/// </summary>
		TEST_METHOD(CurrentGameNumber_UpdateGameData_Data_DiffObj)
		{
			typedef DWORD TestDataType;
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData((TestDataType)0), &m_testIndex);

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the test data.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);

				// Data.
				const bool changeExpected(true);
				const int currentGameNumberExpected((int)testValue);
				CGame *game(new CGame(MEMORY_NONE));
				game->SetGameNumber(currentGameNumberExpected);
				AssertResult2((int)game->GetGameNumber(), currentGameNumberExpected); // Verify data.
				unique_ptr<CGameEndData> gameData(new CGameEndData(game, 0, 0, 0, NULL, 1));

				// Setup.
				CProgressiveConfig progressiveConfig;
				CProgressiveDataConsumerMock dataConsumer;

				// Perform operation(s) and test(s).
				CProgressiveConnection progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);
				AssertResult2(progressiveConnection.UpdateGameData(gameData.get()), changeExpected);
				AssertResult2(progressiveConnection.m_currentGameNumber, currentGameNumberExpected);
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.

		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::UpdateGameData()
		/// Test a range of values when gameData->GetCurrentGame() is not nullptr.
		/// Use a the same CProgressiveConnection instance for each test.
		/// </summary>
		TEST_METHOD(CurrentGameNumber_UpdateGameData_Data_SameObj)
		{
			typedef int TestDataType;
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData((TestDataType)0), &m_testIndex);

			// Setup.
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;
			CProgressiveConnection progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the test data.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);

				// Data.
				const bool changeExpected(true);
				const int currentGameNumberExpected(testValue);
				CGame *game(new CGame(MEMORY_NONE));
				game->SetGameNumber((DWORD)currentGameNumberExpected);
				AssertResult2((int)game->GetGameNumber(), currentGameNumberExpected); // Verify data.
				unique_ptr<CGameEndData> gameData(new CGameEndData(game, 0, 0, 0, NULL, 1));

				// Perform operation(s) and test(s).
				AssertResult2(progressiveConnection.UpdateGameData(gameData.get()), changeExpected);
				AssertResult2(progressiveConnection.m_currentGameNumber, currentGameNumberExpected);
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.

		}
		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::ProcessGameData()
		/// Test for default value still present when gameData is nullptr.
		/// </summary>
		TEST_METHOD(CurrentGameNumber_ProcessGameData_GameDataNull)
		{
			// Data.
			const bool changeExpected(false);
			const int currentGameNumberExpected(m_currentGameNumberDefault);
			const CGameEndData *gameData(nullptr);

			// Setup.
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;

			// Perform operation(s) and test(s).
			CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);
			EXPECT_CALL(progressiveConnection, SendCardStatus()).Times(0);
			progressiveConnection.ProcessGameData(gameData);
			Assert::AreEqual(currentGameNumberExpected, progressiveConnection.m_currentGameNumber, L"progressiveConnection.m_currentGameNumber");

			Assert::IsTrue(Mock::VerifyAndClearExpectations(&dataConsumer));
			Assert::IsTrue(Mock::VerifyAndClearExpectations(&progressiveConnection));
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::ProcessGameData()
		/// Test for default value still present when gameData->GetCurrentGame() is nullptr.
		/// </summary>
		TEST_METHOD(CurrentGameNumber_ProcessGameData_CurrentGameNull)
		{
			// Data.
			const bool changeExpected(false);
			const int currentGameNumberExpected(m_currentGameNumberDefault);
			unique_ptr<CGameEndData> gameData(new CGameEndData(nullptr, 0, 0, 0, NULL, 1));

			// Setup.
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;

			// Perform operation(s) and test(s).
			CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);
			EXPECT_CALL(progressiveConnection, SendCardStatus()).Times(0);
			progressiveConnection.ProcessGameData(gameData.get());
			Assert::AreEqual(currentGameNumberExpected, progressiveConnection.m_currentGameNumber, L"progressiveConnection.m_currentGameNumber");

			Assert::IsTrue(Mock::VerifyAndClearExpectations(&dataConsumer));
			Assert::IsTrue(Mock::VerifyAndClearExpectations(&progressiveConnection));
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::ProcessGameData()
		/// Test for for non-default value when gameData->GetCurrentGame() is not nullptr.
		/// </summary>
		TEST_METHOD(CurrentGameNumber_ProcessGameData)
		{
			// Data.
			const bool changeExpected(true);
			const int currentGameNumberExpected(57);
			CGame *game(new CGame(MEMORY_NONE));
			game->SetGameNumber(currentGameNumberExpected);
			AssertResult2((int)game->GetGameNumber(), currentGameNumberExpected); // Verify data.
			unique_ptr<CGameEndData> gameData(new CGameEndData(game, 0, 0, 0, NULL, 1));

			// Setup.
			CProgressiveConfig progressiveConfig;
			progressiveConfig.SetActive(false);
			CProgressiveDataConsumerMock dataConsumer;

			// Perform operation(s) and test(s).
			CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);
			EXPECT_CALL(progressiveConnection, SendCardStatus()).Times(1);
			progressiveConnection.ProcessGameData(gameData.get());
			Assert::AreEqual(currentGameNumberExpected, progressiveConnection.m_currentGameNumber, L"progressiveConnection.m_currentGameNumber");

			Assert::IsTrue(Mock::VerifyAndClearExpectations(&dataConsumer));
			Assert::IsTrue(Mock::VerifyAndClearExpectations(&progressiveConnection));
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::ProcessGameData()
		/// Test for non-default value still present when gameData is nullptr on second call.
		/// </summary>
		TEST_METHOD(CurrentGameNumber_ProcessGameData2_GameDataNull)
		{
			// Data.
			const bool changeExpected(false);
			const int currentGameNumberExpected(57);
			const CGameEndData *gameData(nullptr);

			// Setup.
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;
			CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);
			EXPECT_CALL(progressiveConnection, SendCardStatus()).Times(1);
			CGame *gameSetup(new CGame(MEMORY_NONE));
			gameSetup->SetGameNumber(currentGameNumberExpected);
			AssertResult2((int)gameSetup->GetGameNumber(), currentGameNumberExpected); // Verify setup.
			unique_ptr<CGameEndData> gameDataSetup(new CGameEndData(gameSetup, 0, 0, 0, NULL, 1));
			progressiveConnection.ProcessGameData(gameDataSetup.get());
			Assert::AreEqual(currentGameNumberExpected, progressiveConnection.m_currentGameNumber, L"Setup"); // Verify setup.


			// Perform operation(s) and test(s).
			progressiveConnection.ProcessGameData(gameData);
			Assert::AreEqual(currentGameNumberExpected, progressiveConnection.m_currentGameNumber, L"Test");

			Assert::IsTrue(Mock::VerifyAndClearExpectations(&dataConsumer));
			Assert::IsTrue(Mock::VerifyAndClearExpectations(&progressiveConnection));
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::ProcessGameData()
		/// Test for true value still present when gameData->GetCurrentGame() is nullptr on second call.
		/// </summary>
		TEST_METHOD(CurrentGameNumber_ProcessGameData2_CurrentGameNull)
		{
			// Data.
			const bool changeExpected(false);
			const bool haveGameDataExpected(true);
			unique_ptr<CGameEndData> gameData(new CGameEndData(nullptr, 0, 0, 0, NULL, 1));

			// Setup.
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;
			CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);
			EXPECT_CALL(progressiveConnection, SendCardStatus()).Times(1);
			unique_ptr<CGameEndData> gameDataSetup(new CGameEndData(new CGame(MEMORY_NONE), 0, 0, 0, NULL, 1));
			progressiveConnection.ProcessGameData(gameDataSetup.get());
			Assert::AreEqual(haveGameDataExpected, progressiveConnection.m_haveGameData, L"Setup"); // Verify setup.

			// Perform operation(s) and test(s).
			progressiveConnection.ProcessGameData(gameData.get());
			Assert::AreEqual(haveGameDataExpected, progressiveConnection.m_haveGameData, L"Test");

			Assert::IsTrue(Mock::VerifyAndClearExpectations(&dataConsumer));
			Assert::IsTrue(Mock::VerifyAndClearExpectations(&progressiveConnection));
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::ProcessGameData()
		/// Test for non-default value present when gameData->GetCurrentGame() is not nullptr.
		/// Test for setting non-default value back to 0 or no game currently selected.
		/// </summary>
		TEST_METHOD(CurrentGameNumber_ProcessGameData2_None)
		{
			// Setup.
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;
			CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);

			// Set to non-default value.
			{
				// Data.
				const int currentGameNumberExpected(57);
				CGame *game(new CGame(MEMORY_NONE));
				game->SetGameNumber(currentGameNumberExpected);
				AssertResult2((int)game->GetGameNumber(), currentGameNumberExpected); // Verify data.
				unique_ptr<CGameEndData> gameData(new CGameEndData(game, 0, 0, 0, NULL, 1));

				// Perform operation(s) and test(s).
				EXPECT_CALL(progressiveConnection, SendCardStatus()).Times(1);
				progressiveConnection.ProcessGameData(gameData.get());
				Assert::AreEqual(currentGameNumberExpected, progressiveConnection.m_currentGameNumber);

				Assert::IsTrue(Mock::VerifyAndClearExpectations(&dataConsumer));
				Assert::IsTrue(Mock::VerifyAndClearExpectations(&progressiveConnection));
			}

			// Set back to 0 or no game currently selected.
			{
				// Data.
				const bool changeExpected(true);
				const int currentGameNumberExpected(68);
				CGame *game(new CGame(MEMORY_NONE));
				game->SetGameNumber(currentGameNumberExpected);
				AssertResult2((int)game->GetGameNumber(), currentGameNumberExpected); // Verify data.
				unique_ptr<CGameEndData> gameData(new CGameEndData(game, 0, 0, 0, NULL, 1));

				// Perform operation(s) and test(s).
				EXPECT_CALL(progressiveConnection, SendCardStatus()).Times(1);
				progressiveConnection.ProcessGameData(gameData.get());
				Assert::AreEqual(currentGameNumberExpected, progressiveConnection.m_currentGameNumber);

				Assert::IsTrue(Mock::VerifyAndClearExpectations(&dataConsumer));
				Assert::IsTrue(Mock::VerifyAndClearExpectations(&progressiveConnection));
			}
		}

			/// <summary>
			/// Functional test for:
			///     CProgressiveConnection::CProgressiveConnection()
			///     CProgressiveConnection::ProcessGameData()
			/// Test for non-default value present when gameData->GetCurrentGame() is not nullptr.
			/// Test for setting a different non-default value after setting a non-default value.
			/// </summary>
		TEST_METHOD(CurrentGameNumber_ProcessGameData2_Diff)
		{
			// Setup.
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;
			CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);

			// Set to non-default value.
			{
				// Data.
				const int currentGameNumberExpected(57);
				CGame *game(new CGame(MEMORY_NONE));
				game->SetGameNumber(currentGameNumberExpected);
				AssertResult2((int)game->GetGameNumber(), currentGameNumberExpected); // Verify data.
				unique_ptr<CGameEndData> gameData(new CGameEndData(game, 0, 0, 0, NULL, 1));

				// Perform operation(s) and test(s).
				EXPECT_CALL(progressiveConnection, SendCardStatus()).Times(1);
				progressiveConnection.ProcessGameData(gameData.get());
				Assert::AreEqual(currentGameNumberExpected, progressiveConnection.m_currentGameNumber);

				Assert::IsTrue(Mock::VerifyAndClearExpectations(&dataConsumer));
				Assert::IsTrue(Mock::VerifyAndClearExpectations(&progressiveConnection));
			}

			// Set back to 0 or no game currently selected.
			{
				// Data.
				const int currentGameNumberExpected(68);
				CGame *game(new CGame(MEMORY_NONE));
				game->SetGameNumber(currentGameNumberExpected);
				AssertResult2((int)game->GetGameNumber(), currentGameNumberExpected); // Verify data.
				unique_ptr<CGameEndData> gameData(new CGameEndData(game, 0, 0, 0, NULL, 1));

				// Perform operation(s) and test(s).
				EXPECT_CALL(progressiveConnection, SendCardStatus()).Times(1);
				progressiveConnection.ProcessGameData(gameData.get());
				Assert::AreEqual(currentGameNumberExpected, progressiveConnection.m_currentGameNumber);

				Assert::IsTrue(Mock::VerifyAndClearExpectations(&dataConsumer));
				Assert::IsTrue(Mock::VerifyAndClearExpectations(&progressiveConnection));
			}
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::ProcessGameData()
		/// Test for non-default value present when gameData->GetCurrentGame() is not nullptr.
		/// Test for setting the same non-default value after setting a non-default value.
		/// </summary>
		TEST_METHOD(CurrentGameNumber_ProcessGameData2_Same)
		{
			// Setup.
			const int currentGameNumberExpected(57);
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;
			CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);

			// Set to non-default value.
			{
				// Data.
				CGame *game(new CGame(MEMORY_NONE));
				game->SetGameNumber(currentGameNumberExpected);
				AssertResult2((int)game->GetGameNumber(), currentGameNumberExpected); // Verify data.
				unique_ptr<CGameEndData> gameData(new CGameEndData(game, 0, 0, 0, NULL, 1));

				// Perform operation(s) and test(s).
				EXPECT_CALL(progressiveConnection, SendCardStatus()).Times(1);
				progressiveConnection.ProcessGameData(gameData.get());
				Assert::AreEqual(currentGameNumberExpected, progressiveConnection.m_currentGameNumber);

				Assert::IsTrue(Mock::VerifyAndClearExpectations(&dataConsumer));
				Assert::IsTrue(Mock::VerifyAndClearExpectations(&progressiveConnection));
			}

			// Set back to 0 or no game currently selected.
			{
				// Data.
				CGame *game(new CGame(MEMORY_NONE));
				game->SetGameNumber(currentGameNumberExpected);
				AssertResult2((int)game->GetGameNumber(), currentGameNumberExpected); // Verify data.
				unique_ptr<CGameEndData> gameData(new CGameEndData(game, 0, 0, 0, NULL, 1));

				// Perform operation(s) and test(s).
				EXPECT_CALL(progressiveConnection, SendCardStatus()).Times(0);
				progressiveConnection.ProcessGameData(gameData.get());
				Assert::AreEqual(currentGameNumberExpected, progressiveConnection.m_currentGameNumber);

				Assert::IsTrue(Mock::VerifyAndClearExpectations(&dataConsumer));
				Assert::IsTrue(Mock::VerifyAndClearExpectations(&progressiveConnection));
			}
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::ProcessGameData()
		/// Test a range of values when gameData->GetCurrentGame() is not nullptr.
		/// Use a differnt CProgressiveConnection instance for each test.
		/// </summary>
		TEST_METHOD(CurrentGameNumber_ProcessGameData_Data_DiffObj)
		{
			typedef DWORD TestDataType;
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData((TestDataType)0), &m_testIndex);

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the test data.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);

				// Data.
				const bool changeExpected(true);
				const int currentGameNumberExpected((int)testValue);
				CGame *game(new CGame(MEMORY_NONE));
				game->SetGameNumber(currentGameNumberExpected);
				AssertResult2((int)game->GetGameNumber(), currentGameNumberExpected); // Verify data.
				unique_ptr<CGameEndData> gameData(new CGameEndData(game, 0, 0, 0, NULL, 1));

				// Setup.
				CProgressiveConfig progressiveConfig;
				CProgressiveDataConsumerMock dataConsumer;

				// Perform operation(s) and test(s).
				CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);
				EXPECT_CALL(progressiveConnection, SendCardStatus()).Times(1);
				progressiveConnection.ProcessGameData(gameData.get());
				AssertResult2(progressiveConnection.m_currentGameNumber, currentGameNumberExpected);

				Assert::IsTrue(Mock::VerifyAndClearExpectations(&dataConsumer));
				Assert::IsTrue(Mock::VerifyAndClearExpectations(&progressiveConnection));
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.

		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::ProcessGameData()
		/// Test a range of values when gameData->GetCurrentGame() is not nullptr.
		/// Use a the same CProgressiveConnection instance for each test.
		/// </summary>
		TEST_METHOD(CurrentGameNumber_ProcessGameData_Data_SameObj)
		{
			typedef int TestDataType;
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData((TestDataType)0), &m_testIndex);

			// Setup.
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;
			CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the test data.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);

				// Data.
				const bool changeExpected(true);
				const int currentGameNumberExpected(testValue);
				CGame *game(new CGame(MEMORY_NONE));
				game->SetGameNumber((DWORD)currentGameNumberExpected);
				AssertResult2((int)game->GetGameNumber(), currentGameNumberExpected); // Verify data.
				unique_ptr<CGameEndData> gameData(new CGameEndData(game, 0, 0, 0, NULL, 1));

				// Perform operation(s) and test(s).
				EXPECT_CALL(progressiveConnection, SendCardStatus()).Times(1);
				progressiveConnection.ProcessGameData(gameData.get());
				AssertResult2(progressiveConnection.m_currentGameNumber, currentGameNumberExpected);

				Assert::IsTrue(Mock::VerifyAndClearExpectations(&dataConsumer));
				Assert::IsTrue(Mock::VerifyAndClearExpectations(&progressiveConnection));
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.

		}
// #pragma endregion CProgressiveConnection::m_currentGameNumber

// #pragma region CProgressiveConnection::ProcessBonusingAvailable
		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::ProcessBonusingAvailable()
		/// Test session identifier logic when SessionId is omitted.
		/// </summary>
		TEST_METHOD(ProcessBonusingAvailable_SessionIdNull)
		{
			static const LPCSTR json(
				"{"
				// SessionId is omitted for this test.
				"\"EligibilityDetermined\":true,"
				"\"SourceDeviceId\":1,"
				"\"SourceDeviceConfigVersion\":0,"
				"\"SourceAsset\":\"\","
				"\"Timestamp\":1434039783129,"
				"\"MessageId\":\"909-1\""
				"}");
			OutputDebugStringA("### pJson ###\n" + CStringA(json) + "\n### pJson ###\n");

			try
			{
				// Setup data.
				rapidjson::Document jsonDoc;
				if (jsonDoc.Parse(json).HasParseError())
					throw JsonException(jsonDoc.GetParseError(), nullptr, L"Parse");

				// Setup progressive connection.
				CProgressiveConfig progressiveConfig;
				CProgressiveDataConsumerMock dataConsumer;
				CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);

				// Perform operation(s) and test(s).
				EXPECT_CALL(dataConsumer, ProgressiveBonusingAvailable(_,_,_)).Times(0);
				progressiveConnection.ProcessBonusingAvailable(jsonDoc);
			}
			catch (JsonException& e)
			{
				Assert::Fail(AssertMsg::Format(0, L"Unexpected exception", L"%u: %S: %S: %s", e.code(), e.codestring(), e.what(), e.filename()));
			}
			catch (SentinelException& e)
			{
				Assert::Fail(AssertMsg::Format(0, L"Unexpected exception", L"%u: %S", e.code(), e.what()));
			}
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::ProcessBonusingAvailable()
		/// Test session identifer logic when SessionId is present.
		/// </summary>
		TEST_METHOD(ProcessBonusingAvailable_SessionId)
		{
			// Data.
			const int sessionIdExpected(118);

			CStringA json;
			json.Format(
				"{"
				"\"SessionId\":%d,"
				"\"EligibilityDetermined\":true,"
				"\"SourceDeviceId\":1,"
				"\"SourceDeviceConfigVersion\":0,"
				"\"SourceAsset\":\"\","
				"\"Timestamp\":1434039783129,"
				"\"MessageId\":\"909-1\""
				"}",
				(int)sessionIdExpected);
			OutputDebugStringA("### json ###\n" + CStringA(json) + "\n### json ###\n");

			try
			{
				// Setup data.
				rapidjson::Document jsonDoc;
				if (jsonDoc.Parse(json).HasParseError())
					throw JsonException(jsonDoc.GetParseError(), nullptr, L"Parse");

				// Setup progressive connection.
				CProgressiveConfig progressiveConfig;
				CProgressiveDataConsumerMock dataConsumer;
				CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);

				// Perform operation(s) and test(s).
				EXPECT_CALL(dataConsumer, ProgressiveBonusingAvailable(CheckBonusingAvailableSessionId(sessionIdExpected),_,_)).Times(1);
				progressiveConnection.ProcessBonusingAvailable(jsonDoc);
			}
			catch (JsonException& e)
			{
				Assert::Fail(AssertMsg::Format(0, L"Unexpected exception", L"%u: %S: %S: %s", e.code(), e.codestring(), e.what(), e.filename()));
			}
			catch (SentinelException& e)
			{
				Assert::Fail(AssertMsg::Format(0, L"Unexpected exception", L"%u: %S", e.code(), e.what()));
			}
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::onMessage()
		///     CProgressiveConnection::ProcessBonusingAvailable()
		/// Test that onMessage() correctly calls ProcessBonusingAvailable().
		/// Test ProcessBonusingAvailable session identifier logic when SessionId is present.
		/// </summary>
		TEST_METHOD(onMessage_ProcessingBonusingAvailableTest_SessionId)
		{
			// Data.
			const int sessionIdExpected(118);

			CStringA json;
			json.Format(
				"{"
				"\"SessionId\":%d,"
				"\"EligibilityDetermined\":true,"
				"\"SourceDeviceId\":1,"
				"\"SourceDeviceConfigVersion\":0,"
				"\"SourceAsset\":\"\","
				"\"Timestamp\":1434039783129,"
				"\"MessageId\":\"909-1\""
				"}",
				(int)sessionIdExpected);
			OutputDebugStringA("### json ###\n" + json + "\n### json ###\n");
			string messageBody(json);

			// Setup.
			CProgressiveDataConsumerMock dataConsumer;
			CProgressiveConfig progressiveConfig;
			CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);

			CConnection connection;
			ON_CALL(progressiveConnection, CreateBytesMessage()).WillByDefault(Return(new ActiveMQBytesMessage()));
			progressiveConnection.LoadDeviceConnectionData(&connection);

			// Create the BonusingAvailable message.
			BytesMessage *message =
				progressiveConnection.CProgressiveConnection::CreateBytesMessage(FromOneLink::pBonusingAvailable, messageBody);
			message->reset(); // Make readable.
			
			// Perform operation(s) and test(s).
			EXPECT_CALL(dataConsumer, ProgressiveBonusingAvailable(CheckBonusingAvailableSessionId(sessionIdExpected), _, _)).Times(1);
			progressiveConnection.onMessage(message);
		}
// #pragma endregion CProgressiveConnection::ProcessBonusingAvailable

// #pragma region CProgressiveConnection::ProcessJackpotHit
		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::ProcessJackpotHit()
		/// Test session ID match logic when SessionId is omitted.
		/// </summary>
		TEST_METHOD(ProcessJackpotHit_SessionIdNull)
		{
			// Data.
			const bool hasSessionIdExpected(false);
			const int sessionIdExpected(CPlayerSession::SESSION_ID_DEFAULT);

			static const LPCSTR json(
				"{"
				"\"JackpotId\":909,"
				"\"RemoteJackpotId\":0,"
				"\"WinningDeviceId\":6467,"
				"\"LevelId\":23886,"
				"\"LevelName\":\"JacksonTest\","
				"\"OverageAdjustment\":0,"
				"\"Amount\":3,"
				"\"OriginalAmount\":0,"
				"\"JackpotType\":\"Bonus\","
				"\"JackpotTriggerType\":\"CoinInTrigger\","
				"\"PriorityAward\":false,"
				"\"WinningAssetNumber\":\"113087\","
				"\"Remote\":false,"
				"\"MethodOfPayment\":\"PayToGame\","
				"\"AwardType\":\"CashableCredits\","
				// SessionId is omitted for this test.
				"\"SourceDeviceId\":1,"
				"\"SourceDeviceConfigVersion\":0,"
				"\"SourceAsset\":\"\","
				"\"Timestamp\":1434039783129,"
				"\"MessageId\":\"909-1\""
				"}");

			OutputDebugStringA("### pJson ###\n");
			OutputDebugStringA(json);
			OutputDebugStringA("\n### pJson ###\n");

			try
			{
				// Setup.
				rapidjson::Document jsonDoc;
				if (jsonDoc.Parse(json).HasParseError())
					throw JsonException(jsonDoc.GetParseError(), nullptr, L"Parse");

				// Setup progressive connection.
				CConnection connection;
				CProgressiveConfig progressiveConfig;

				CProgressiveDataConsumerMock dataConsumer;
				EXPECT_CALL(dataConsumer, JackpotHit(CheckSessionId(hasSessionIdExpected, sessionIdExpected))).Times(1);

				CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);
				EXPECT_CALL(progressiveConnection, SendMessageToOneLink(_, _)).Times(0);
				progressiveConnection.LoadDeviceConnectionData(&connection);
				Assert::IsTrue(progressiveConnection.IsMessageFactoryCreated(), L"progressiveConnection.IsMessageFactoryCreated()"); // Verify setup.

				// Perform operation(s) and test(s).
				progressiveConnection.ProcessJackpotHit(jsonDoc);

				Assert::IsTrue(Mock::VerifyAndClearExpectations(&dataConsumer));
				Assert::IsTrue(Mock::VerifyAndClearExpectations(&progressiveConnection));
			}
			catch (JsonException& e)
			{
				Assert::Fail(AssertMsg::Format(0, L"Unexpected exception", L"%u: %S: %S: %s", e.code(), e.codestring(), e.what(), e.filename()));
			}
			catch (SentinelException& e)
			{
				Assert::Fail(AssertMsg::Format(0, L"Unexpected exception", L"%u: %S", e.code(), e.what()));
			}
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::ProcessJackpotHit()
		/// Test session ID match logic when SessionId matches.
		/// </summary>
		TEST_METHOD(ProcessJackpotHit_SessionIdMatch)
		{
			// Data.
			const bool hasSessionIdExpected(true);
			const int sessionIdExpected(393);

			CStringA json;
			json.Format(
				"{"
				"\"JackpotId\":909,"
				"\"RemoteJackpotId\":0,"
				"\"WinningDeviceId\":6467,"
				"\"LevelId\":23886,"
				"\"LevelName\":\"JacksonTest\","
				"\"OverageAdjustment\":0,"
				"\"Amount\":3,"
				"\"OriginalAmount\":0,"
				"\"JackpotType\":\"Bonus\","
				"\"JackpotTriggerType\":\"CoinInTrigger\","
				"\"PriorityAward\":false,"
				"\"WinningAssetNumber\":\"113087\","
				"\"Remote\":false,"
				"\"MethodOfPayment\":\"PayToGame\","
				"\"AwardType\":\"CashableCredits\","
				"\"SessionId\":%d," // Set SessionId to sessionIdExpected.
				"\"SourceDeviceId\":1,"
				"\"SourceDeviceConfigVersion\":0,"
				"\"SourceAsset\":\"\","
				"\"Timestamp\":1434039783129,"
				"\"MessageId\":\"909-1\""
				"}",
				(int)sessionIdExpected);

			OutputDebugStringA("### pJson ###\n");
			OutputDebugStringA(json);
			OutputDebugStringA("\n### pJson ###\n");

			try
			{
				// Setup.
				rapidjson::Document jsonDoc;
				if (jsonDoc.Parse(json).HasParseError())
					throw JsonException(jsonDoc.GetParseError(), nullptr, L"Parse");

				// Setup progressive connection.
				CConnection connection;
				CProgressiveConfig progressiveConfig;

				CProgressiveDataConsumerMock dataConsumer;
				EXPECT_CALL(dataConsumer, JackpotHit(CheckSessionId(hasSessionIdExpected, sessionIdExpected))).Times(1);

				CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);
				EXPECT_CALL(progressiveConnection, SendMessageToOneLink(_, _)).Times(0);
				progressiveConnection.LoadDeviceConnectionData(&connection);
				Assert::IsTrue(progressiveConnection.IsMessageFactoryCreated(), L"progressiveConnection.IsMessageFactoryCreated()"); // Verify setup.
				progressiveConnection.SetPlayer(nullptr, false, sessionIdExpected);

				// Perform operation(s) and test(s).
				progressiveConnection.ProcessJackpotHit(jsonDoc);

				Assert::IsTrue(Mock::VerifyAndClearExpectations(&dataConsumer));
				Assert::IsTrue(Mock::VerifyAndClearExpectations(&progressiveConnection));
			}
			catch (JsonException& e)
			{
				Assert::Fail(AssertMsg::Format(0, L"Unexpected exception", L"%u: %S: %S: %s", e.code(), e.codestring(), e.what(), e.filename()));
			}
			catch (SentinelException& e)
			{
				Assert::Fail(AssertMsg::Format(0, L"Unexpected exception", L"%u: %S", e.code(), e.what()));
			}
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::ProcessJackpotHit()
		/// Test session ID match logic when SessionId is present, but does not match.
		/// </summary>
		TEST_METHOD(ProcessJackpotHit_SessionIdMismatch)
		{
			// Data.
			const int connectionSessionId(394);
			const int awardSessionId(connectionSessionId - 1);
			LPCSTR status("SessionMismatch");
			LPCSTR statusInfo("egmAward does not match current sessionId=394");

			CStringA json;
			json.Format(
				"{"
				"\"JackpotId\":909,"
				"\"RemoteJackpotId\":0,"
				"\"WinningDeviceId\":6467,"
				"\"LevelId\":23886,"
				"\"LevelName\":\"JacksonTest\","
				"\"OverageAdjustment\":0,"
				"\"Amount\":3,"
				"\"OriginalAmount\":0,"
				"\"JackpotType\":\"Bonus\","
				"\"JackpotTriggerType\":\"CoinInTrigger\","
				"\"PriorityAward\":false,"
				"\"WinningAssetNumber\":\"113087\","
				"\"Remote\":false,"
				"\"MethodOfPayment\":\"PayToGame\","
				"\"AwardType\":\"CashableCredits\","
				"\"SessionId\":%d," // Set SessionId to awardSessionId.
				"\"SourceDeviceId\":1,"
				"\"SourceDeviceConfigVersion\":0,"
				"\"SourceAsset\":\"\","
				"\"Timestamp\":1434039783129,"
				"\"MessageId\":\"909-1\""
				"}",
				(int)awardSessionId);

			OutputDebugStringA("### pJson ###\n");
			OutputDebugStringA(json);
			OutputDebugStringA("\n### pJson ###\n");

			try
			{
				// Setup.
				rapidjson::Document jsonDoc;
				if (jsonDoc.Parse(json).HasParseError())
					throw JsonException(jsonDoc.GetParseError(), nullptr, L"Parse");

				// Setup progressive connection.
				CConnection connection;
				CProgressiveConfig progressiveConfig;

				CProgressiveDataConsumerMock dataConsumer;
				EXPECT_CALL(dataConsumer, JackpotHit(_)).Times(0);

				CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);
				EXPECT_CALL(progressiveConnection, SendMessageToOneLink(_, _)).Times(0);
				EXPECT_CALL(progressiveConnection,
					SendMessageToOneLink(ToOneLink::JackpotPaid, CheckJackpotPaidMessage(status, statusInfo))).Times(1);
				progressiveConnection.LoadDeviceConnectionData(&connection);
				Assert::IsTrue(progressiveConnection.IsMessageFactoryCreated(), L"progressiveConnection.IsMessageFactoryCreated()"); // Verify setup.
				progressiveConnection.SetPlayer(nullptr, false, connectionSessionId);

				// Perform operation(s) and test(s).
				progressiveConnection.ProcessJackpotHit(jsonDoc);

				Assert::IsTrue(Mock::VerifyAndClearExpectations(&dataConsumer));
				Assert::IsTrue(Mock::VerifyAndClearExpectations(&progressiveConnection));
			}
			catch (JsonException& e)
			{
				Assert::Fail(AssertMsg::Format(0, L"Unexpected exception", L"%u: %S: %S: %s", e.code(), e.codestring(), e.what(), e.filename()));
			}
			catch (SentinelException& e)
			{
				Assert::Fail(AssertMsg::Format(0, L"Unexpected exception", L"%u: %S", e.code(), e.what()));
			}
		}
// #pragma endregion CProgressiveConnection::ProcessJackpotHit

// #pragma region CProgressiveConnection::SetLastHeartbeatTickCount
		void SetLastHeartbeatTickCount_MessageType(ToOneLink::Type messageType, bool isHeartbeat)
		{
			// Data.
			const DWORD tickCount0(10000);
			const DWORD tickCount1(11000);
			const DWORD tickCountExpected(isHeartbeat ? tickCount1 : tickCount0);

			// Setup.
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;
			COperatingSystemTest operatingSystem; // So we can control the tick count.

			CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath, operatingSystem);
			operatingSystem.m_tickCount32 = tickCount0;
			progressiveConnection.SetLastHeartbeatTickCount(ToOneLink::ICardStatus);
			AssertResult2(progressiveConnection.m_connectionInfo.m_lastHeartbeatMessageTickCount, tickCount0); // Verify setup.

			// Perform operation(s) and test(s).
			operatingSystem.m_tickCount32 = tickCount1;
			progressiveConnection.SetLastHeartbeatTickCount(messageType);
			AssertResult2(progressiveConnection.m_connectionInfo.m_lastHeartbeatMessageTickCount, tickCountExpected);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::SetLastHeartbeatTickCount()
		/// Test with messageType CoinIn5.
		/// </summary>
		TEST_METHOD(SetLastHeartbeatTickCount_CoinIn5)
		{
			SetLastHeartbeatTickCount_MessageType(ToOneLink::CoinIn5, m_coinIn5IsHearbeat);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::SetLastHeartbeatTickCount()
		/// Test with messageType DeviceAcknowledgement.
		/// </summary>
		TEST_METHOD(SetLastHeartbeatTickCount_DeviceAcknowledgement)
		{
			SetLastHeartbeatTickCount_MessageType(ToOneLink::DeviceAcknowledgement, false);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::SetLastHeartbeatTickCount()
		/// Test with messageType EventNotification.
		/// </summary>
		TEST_METHOD(SetLastHeartbeatTickCount_EventNotification)
		{
			SetLastHeartbeatTickCount_MessageType(ToOneLink::EventNotification, false);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::SetLastHeartbeatTickCount()
		/// Test with messageType GameEnd5.
		/// </summary>
		TEST_METHOD(SetLastHeartbeatTickCount_GameEnd5)
		{
			SetLastHeartbeatTickCount_MessageType(ToOneLink::GameEnd5, m_gameEnd5IsHearbeat);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::SetLastHeartbeatTickCount()
		/// Test with messageType ICardConfigurationRequest5.
		/// </summary>
		TEST_METHOD(SetLastHeartbeatTickCount_ICardConfigurationRequest5)
		{
			SetLastHeartbeatTickCount_MessageType(ToOneLink::ICardConfigurationRequest5, false);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::SetLastHeartbeatTickCount()
		/// Test with messageType ICardEventNotification.
		/// </summary>
		TEST_METHOD(SetLastHeartbeatTickCount_ICardEventNotification)
		{
			SetLastHeartbeatTickCount_MessageType(ToOneLink::ICardEventNotification, false);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::SetLastHeartbeatTickCount()
		/// Test with messageType ICardStatus.
		/// </summary>
		TEST_METHOD(SetLastHeartbeatTickCount_ICardStatus)
		{
			SetLastHeartbeatTickCount_MessageType(ToOneLink::ICardStatus, true);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::SetLastHeartbeatTickCount()
		/// Test with messageType JackpotPaid.
		/// </summary>
		TEST_METHOD(SetLastHeartbeatTickCount_JackpotPaid)
		{
			SetLastHeartbeatTickCount_MessageType(ToOneLink::JackpotPaid, false);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::SetLastHeartbeatTickCount()
		/// Test with messageType ProgressiveLevelHit.
		/// </summary>
		TEST_METHOD(SetLastHeartbeatTickCount_ProgressiveLevelHit)
		{
			SetLastHeartbeatTickCount_MessageType(ToOneLink::ProgressiveLevelHit, false);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::SetLastHeartbeatTickCount()
		/// Test with messageType Invalid.
		/// </summary>
		TEST_METHOD(SetLastHeartbeatTickCount_Invalid)
		{
			SetLastHeartbeatTickCount_MessageType(ToOneLink::Invalid, false);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::SetLastHeartbeatTickCount()
		/// Test with various tick count values..
		/// </summary>
		TEST_METHOD(SetLastHeartbeatTickCount_TickCount)
		{
			// Data.
			typedef DWORD TestDataType;
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData((TestDataType)0), &m_testIndex);

			// Setup.
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;
			COperatingSystemTest operatingSystem; // So we can control the tick count.
			CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath, operatingSystem);

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the test data.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);
				DWORD tickCountExpected(testValue);

				// Setup.
				operatingSystem.m_tickCount32 = tickCountExpected;

				// Perform operation(s) and test(s).
				progressiveConnection.SetLastHeartbeatTickCount(ToOneLink::ICardStatus);
				AssertResult2(progressiveConnection.m_connectionInfo.m_lastHeartbeatMessageTickCount, tickCountExpected); // Verify setup.
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}
// #pragma endregion CProgressiveConnection::SetLastHeartbeatTickCount

// #pragma region CProgressiveConnection::SetLastHeartbeatTickCountInternalSendMessageToOneLink
		class CProgressiveConnectionSetup
		{
		public:
			// Static data.
			static const DWORD m_tickCount0 = 10000;
			static const DWORD m_tickCount1 = 11000;

			// Data with default constructors.
			CProgressiveConfig m_progressiveConfig;
			CProgressiveDataConsumerMock m_dataConsumer;
			COperatingSystemTest m_operatingSystem; // So we can control the tick count.
			CConnection m_connection;
			CGames m_games;

			// Data without default constructors.
			const bool m_isHeartbeat;
			const DWORD m_tickCountExpected;
			CProgressiveConnectionMock m_progressiveConnection;

			// Constructor.
			CProgressiveConnectionSetup(bool isHeartbeat, ProgressiveConnectionState connectionState, const int &m_testIndex) :
				m_isHeartbeat(isHeartbeat),
				m_tickCountExpected(m_isHeartbeat ? m_tickCount1 : m_tickCount0),
				m_progressiveConnection(m_dataConsumer, &m_progressiveConfig, paltronicsCertificatePath, m_operatingSystem)
			{
				// Setup.
				ON_CALL(m_progressiveConnection, InternalSendMessageToOneLink(_, _, _)).WillByDefault(Invoke(&m_progressiveConnection, &CProgressiveConnectionMock::MockInternalSendMessageToOneLink));
				ON_CALL(m_progressiveConnection, SendMessageToOneLink(_, _)).WillByDefault(Invoke(&m_progressiveConnection, &CProgressiveConnectionMock::BaseSendMessageToOneLink));
				ON_CALL(m_progressiveConnection, SendCardStatus()).WillByDefault(Invoke(&m_progressiveConnection, &CProgressiveConnectionMock::BaseSendCardStatus));

				m_progressiveConnection.LoadDeviceConnectionData(&m_connection);
				AssertResult2(m_progressiveConnection.IsMessageFactoryCreated(), true); // Verify setup.

				m_progressiveConnection.SetGames(&m_games, nullptr);
				AssertResult2(m_progressiveConnection.IsGamesCreated(), true); // Verify setup.

				m_progressiveConnection.UpdateConnectionState(connectionState, ProgressiveConnectionErrorNone);
				AssertResult2(m_progressiveConnection.m_connectionInfo.GetState(), connectionState); // Verify setup.

				m_operatingSystem.m_tickCount32 = m_tickCount0;
				m_progressiveConnection.SetLastHeartbeatTickCount(ToOneLink::ICardStatus);
				AssertResult2(m_progressiveConnection.m_connectionInfo.m_lastHeartbeatMessageTickCount, m_tickCount0); // Verify setup.

				m_operatingSystem.m_tickCount32 = m_tickCount1;
			}

			~CProgressiveConnectionSetup()
			{
				m_progressiveConnection.UpdateConnectionState(Disconnected);
			}
		} ;

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::SendCardConfigurationRequest()
		///     CProgressiveConnection::SendMessageToOneLink()
		///     CProgressiveConnection::SetLastHeartbeatTickCount()
		/// Test with SendCardConfigurationRequest() while connected.
		/// </summary>
		TEST_METHOD(SetLastHeartbeatTickCount_SendCardConfigurationRequest)
		{
			// Setup.
			unique_ptr<CProgressiveConnectionSetup> pcs(new CProgressiveConnectionSetup(false, Connected, m_testIndex));

			// Perform operation(s) and test(s).
			pcs->m_progressiveConnection.SendCardConfigurationRequest();
			AssertResult2(pcs->m_progressiveConnection.m_connectionInfo.m_lastHeartbeatMessageTickCount, pcs->m_tickCountExpected);

			// Set to disabled so connection shuts down
			pcs->m_progressiveConnection.UpdateConnectionState(Disabled);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::SendICardEvent()
		///     CProgressiveConnection::SendMessageToOneLink()
		///     CProgressiveConnection::SetLastHeartbeatTickCount()
		/// Test with SendICardEvent() while connected.
		/// </summary>
		TEST_METHOD(SetLastHeartbeatTickCount_SendICardEvent)
		{
			// Setup.
			unique_ptr<CProgressiveConnectionSetup> pcs(new CProgressiveConnectionSetup(false, Connected, m_testIndex));

			// Perform operation(s) and test(s).
			pcs->m_progressiveConnection.SendICardEvent(GPE_NO_ACTIVITY);
			AssertResult2(pcs->m_progressiveConnection.m_connectionInfo.m_lastHeartbeatMessageTickCount, pcs->m_tickCountExpected);

			// Set to disabled so connection shuts down
			pcs->m_progressiveConnection.UpdateConnectionState(Disabled);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::SendOfflineCoinInEvent()
		///     CProgressiveConnection::SendMessageToOneLink()
		///     CProgressiveConnection::SetLastHeartbeatTickCount()
		/// Test with SendOfflineCoinInEvent() while connected.
		/// </summary>
		TEST_METHOD(SetLastHeartbeatTickCount_SendOfflineCoinInEvent)
		{
			// Setup.
			unique_ptr<CProgressiveConnectionSetup> pcs(new CProgressiveConnectionSetup(false, Connected, m_testIndex));

			// Perform operation(s) and test(s).
			pcs->m_progressiveConnection.SendOfflineCoinInEvent(1234567);
			AssertResult2(pcs->m_progressiveConnection.m_connectionInfo.m_lastHeartbeatMessageTickCount, pcs->m_tickCountExpected);

			// Set to disabled so connection shuts down
			pcs->m_progressiveConnection.UpdateConnectionState(Disabled);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::SendOfflineCoinInEvent()
		///     CProgressiveConnection::SendMessageToOneLink()
		/// Test with SendOfflineCoinInEvent() while not connected and connected.
		/// </summary>
		TEST_METHOD(SendOfflineCoinInEventTest)
		{
			// Data.
			CConnection connection;
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;
			CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);

			// Setup.
			EXPECT_CALL(progressiveConnection, SendMessageToOneLink(ToOneLink::EventNotification, _)).Times(0);
			progressiveConnection.SendOfflineCoinInEvent(969);
			progressiveConnection.LoadDeviceConnectionData(&connection);
			EXPECT_CALL(progressiveConnection, SendMessageToOneLink(ToOneLink::EventNotification, _)).Times(1);
			progressiveConnection.SendOfflineCoinInEvent(969);			

			// Test
			Assert::IsTrue(Mock::VerifyAndClearExpectations(&dataConsumer));
			Assert::IsTrue(Mock::VerifyAndClearExpectations(&progressiveConnection));
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::SendProgressiveLevelHit()
		///     CProgressiveConnection::SendMessageToOneLink()
		///     CProgressiveConnection::SetLastHeartbeatTickCount()
		/// Test with SendProgressiveLevelHit() while connected.
		/// </summary>
		TEST_METHOD(SetLastHeartbeatTickCount_SendProgressiveLevelHit)
		{
			// Setup.
			unique_ptr<CProgressiveConnectionSetup> pcs(new CProgressiveConnectionSetup(false, Connected, m_testIndex));

			// Perform operation(s) and test(s).
			const CEGMAward egmAward;
			pcs->m_progressiveConnection.SendProgressiveLevelHit(egmAward);
			AssertResult2(pcs->m_progressiveConnection.m_connectionInfo.m_lastHeartbeatMessageTickCount, pcs->m_tickCountExpected);

			// Set to disabled so connection shuts down
			pcs->m_progressiveConnection.UpdateConnectionState(Disabled);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::SendJackpotPaidRequest()
		///     CProgressiveConnection::SendMessageToOneLink()
		///     CProgressiveConnection::SetLastHeartbeatTickCount()
		/// Test with SendJackpotPaidRequest() while connected.
		/// </summary>
		TEST_METHOD(SetLastHeartbeatTickCount_SendJackpotPaidRequest)
		{
			// Setup.
			unique_ptr<CProgressiveConnectionSetup> pcs(new CProgressiveConnectionSetup(false, Connected, m_testIndex));

			// Perform operation(s) and test(s).
			const CEGMAward egmAward;
			pcs->m_progressiveConnection.SendJackpotPaidRequest(egmAward);
			AssertResult2(pcs->m_progressiveConnection.m_connectionInfo.m_lastHeartbeatMessageTickCount, pcs->m_tickCountExpected);

			// Set to disabled so connection shuts down
			pcs->m_progressiveConnection.UpdateConnectionState(Disabled);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::SendCardStatus()
		///     CProgressiveConnection::SendMessageToOneLink()
		///     CProgressiveConnection::SetLastHeartbeatTickCount()
		/// Test with SendCardStatus() while in a range of connection states.
		/// </summary>
		TEST_METHOD(SetLastHeartbeatTickCount_SendCardStatus)
		{
			for (m_testIndex = (int)NotStarted - 1; m_testIndex <= (int)Error + 5; ++m_testIndex)
			{
				// Data.
				ProgressiveConnectionState connectionState((ProgressiveConnectionState)m_testIndex);
				bool isHeartbeat(InDiscovery == connectionState || Connected == connectionState);

				// Setup.
				unique_ptr<CProgressiveConnectionSetup> pcs(new CProgressiveConnectionSetup(isHeartbeat, connectionState, m_testIndex));

				// Perform operation(s) and test(s).
				pcs->m_progressiveConnection.SendCardStatus();
				AssertResult2(pcs->m_progressiveConnection.m_connectionInfo.m_lastHeartbeatMessageTickCount, pcs->m_tickCountExpected);

				// Set to disabled so connection shuts down
				pcs->m_progressiveConnection.UpdateConnectionState(Disabled);
			}
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::SendCoinIn()
		///     CProgressiveConnection::SendMessageToOneLink()
		///     CProgressiveConnection::SetLastHeartbeatTickCount()
		/// Test with SendCoinIn() while connected.
		/// </summary>
		TEST_METHOD(SetLastHeartbeatTickCount_SendCoinInConnected)
		{
			// Setup.
			unique_ptr<CProgressiveConnectionSetup> pcs(new CProgressiveConnectionSetup(m_coinIn5IsHearbeat, Connected, m_testIndex));

			// Perform operation(s) and test(s).
			CGameEndData gameEndData(new CGame(MEMORY_NONE), 0, 0, 0, nullptr, 1);
			pcs->m_progressiveConnection.SendCoinIn(&gameEndData);
			AssertResult2(pcs->m_progressiveConnection.m_connectionInfo.m_lastHeartbeatMessageTickCount, pcs->m_tickCountExpected);

			// Set to disabled so connection shuts down
			pcs->m_progressiveConnection.UpdateConnectionState(Disabled);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::SendCoinIn()
		///     CProgressiveConnection::SendMessageToOneLink()
		///     CProgressiveConnection::SetLastHeartbeatTickCount()
		/// Test with SendCoinIn() while in discovery.
		/// </summary>
		TEST_METHOD(SetLastHeartbeatTickCount_SendCoinInInDiscovery)
		{
			// Setup.
			unique_ptr<CProgressiveConnectionSetup> pcs(new CProgressiveConnectionSetup(false, InDiscovery, m_testIndex));

			// Perform operation(s) and test(s).
			CGameEndData gameEndData(new CGame(MEMORY_NONE), 0, 0, 0, nullptr, 1);
			pcs->m_progressiveConnection.SendCoinIn(&gameEndData);
			AssertResult2(pcs->m_progressiveConnection.m_connectionInfo.m_lastHeartbeatMessageTickCount, pcs->m_tickCountExpected);

			// Set to disabled so connection shuts down
			pcs->m_progressiveConnection.UpdateConnectionState(Disabled);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::SendGameEnd()
		///     CProgressiveConnection::SendMessageToOneLink()
		///     CProgressiveConnection::SetLastHeartbeatTickCount()
		/// Test with SendGameEnd() while connected.
		/// </summary>
		TEST_METHOD(SetLastHeartbeatTickCount_SendGameEndConnected)
		{
			// Setup.
			unique_ptr<CProgressiveConnectionSetup> pcs(new CProgressiveConnectionSetup(m_gameEnd5IsHearbeat, Connected, m_testIndex));

			// Perform operation(s) and test(s).
			CGameEndData gameEndData(new CGame(MEMORY_NONE), 0, 0, 0, nullptr, 1);
			pcs->m_progressiveConnection.SendGameEnd(&gameEndData);
			AssertResult2(pcs->m_progressiveConnection.m_connectionInfo.m_lastHeartbeatMessageTickCount, pcs->m_tickCountExpected);

			// Set to disabled so connection shuts down
			pcs->m_progressiveConnection.UpdateConnectionState(Disabled);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::SendGameEnd()
		///     CProgressiveConnection::SendMessageToOneLink()
		///     CProgressiveConnection::SetLastHeartbeatTickCount()
		/// Test with SendGameEnd() while in discovery.
		/// </summary>
		TEST_METHOD(SetLastHeartbeatTickCount_SendGameEndInDiscovery)
		{
			// Setup.
			unique_ptr<CProgressiveConnectionSetup> pcs(new CProgressiveConnectionSetup(false, InDiscovery, m_testIndex));

			// Perform operation(s) and test(s).
			CGameEndData gameEndData(new CGame(MEMORY_NONE), 0, 0, 0, nullptr, 1);
			pcs->m_progressiveConnection.SendGameEnd(&gameEndData);
			AssertResult2(pcs->m_progressiveConnection.m_connectionInfo.m_lastHeartbeatMessageTickCount, pcs->m_tickCountExpected);

			// Set to disabled so connection shuts down
			pcs->m_progressiveConnection.UpdateConnectionState(Disabled);
		}
// #pragma endregion CProgressiveConnection::SetLastHeartbeatTickCountInternalSendMessageToOneLink

// #pragma region CProgressiveConnectionInfo::IsTimeForPing
		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::UpdateConnectionState()
		///     CProgressiveConnection::IsTimeForPing()
		/// Test with a range of m_connectionState values.
		/// </summary>
		TEST_METHOD(IsTimeForPing_SetState)
		{
			// Data.
			DWORD lastHeartbeat(100000);
			DWORD pingInterval(m_pingIntervalMinimumExpected * 2);
			AssertResult2(m_pingIntervalMinimumExpected <= pingInterval && pingInterval <= m_pingIntervalMaximumExpected, true); // Verify data.

			// Setup.
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;
			COperatingSystemTest operatingSystem; // So we can control the tick count.
			CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath, operatingSystem);

			operatingSystem.m_tickCount32 = lastHeartbeat;
			progressiveConnection.SetLastHeartbeatTickCount(ToOneLink::ICardStatus);
			AssertResult2(progressiveConnection.m_connectionInfo.m_lastHeartbeatMessageTickCount, lastHeartbeat); // Verify setup.

			CProgressiveConfig varProgConfig;
			varProgConfig.SetPingInterval(pingInterval);
			progressiveConnection.LoadProgressiveConfig(&varProgConfig);
			AssertResult2(progressiveConnection.m_connectionInfo.m_pingInterval, pingInterval); // Verify setup.

			for (m_testIndex = (int)NotStarted - 1; m_testIndex <= (int)Error + 5; ++m_testIndex)
			{
				// Data.
				ProgressiveConnectionState connectionState((ProgressiveConnectionState)m_testIndex);

				// Perform operation(s) and test(s).
				progressiveConnection.UpdateConnectionState(connectionState, ProgressiveConnectionErrorNone);
				AssertResult2(progressiveConnection.m_connectionInfo.GetState(), connectionState);

				operatingSystem.m_tickCount32 = lastHeartbeat - 1;
				AssertResult2(progressiveConnection.IsTimeForPing(), Connected == connectionState);

				operatingSystem.m_tickCount32 = lastHeartbeat;
				AssertResult2(progressiveConnection.IsTimeForPing(), false);

				operatingSystem.m_tickCount32 = lastHeartbeat + pingInterval - 1;
				AssertResult2(progressiveConnection.IsTimeForPing(), false);

				operatingSystem.m_tickCount32 = lastHeartbeat + pingInterval ;
				AssertResult2(progressiveConnection.IsTimeForPing(), Connected == connectionState);
			}
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::SetLastHeartbeatMessageTickCount()
		///     CProgressiveConnection::IsTimeForPing()
		/// Test with a range of m_lastHeartbeatMessageTickCount values.
		/// </summary>
		TEST_METHOD(IsTimeForPing_SetLastHeartbeatMessageTickCount)
		{
			// Data.
			ProgressiveConnectionState expirationState(Connected);
			DWORD pingInterval(m_pingIntervalMinimumExpected * 2);
			AssertResult2(m_pingIntervalMinimumExpected <= pingInterval && pingInterval <= m_pingIntervalMaximumExpected, true); // Verify data.

			typedef DWORD TestDataType;
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData((TestDataType)0), &m_testIndex);

			// Setup.
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;
			COperatingSystemTest operatingSystem; // So we can control the tick count.
			CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath, operatingSystem);

			progressiveConnection.UpdateConnectionState(expirationState, ProgressiveConnectionErrorNone);
			AssertResult2(progressiveConnection.m_connectionInfo.GetState(), expirationState);

			CProgressiveConfig varProgConfig;
			varProgConfig.SetPingInterval(pingInterval);
			progressiveConnection.LoadProgressiveConfig(&varProgConfig);
			AssertResult2(progressiveConnection.m_connectionInfo.m_pingInterval, pingInterval); // Verify setup.

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the test data.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);
				DWORD lastHeartbeat(testValue);

				// Perform operation(s) and test(s).
				operatingSystem.m_tickCount32 = lastHeartbeat;
				progressiveConnection.SetLastHeartbeatTickCount(ToOneLink::ICardStatus);
				AssertResult2(progressiveConnection.m_connectionInfo.m_lastHeartbeatMessageTickCount, lastHeartbeat);

				operatingSystem.m_tickCount32 = lastHeartbeat - 1;
				AssertResult2(progressiveConnection.IsTimeForPing(), true);

				operatingSystem.m_tickCount32 = lastHeartbeat;
				AssertResult2(progressiveConnection.IsTimeForPing(), false);

				operatingSystem.m_tickCount32 = lastHeartbeat + pingInterval - 1;
				AssertResult2(progressiveConnection.IsTimeForPing(), false);

				operatingSystem.m_tickCount32 = lastHeartbeat + pingInterval;
				AssertResult2(progressiveConnection.IsTimeForPing(), true);
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.

			// Set to disabled so connection shuts down
			progressiveConnection.UpdateConnectionState(Disabled);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::LoadProgressiveConfig()
		///     CProgressiveConnection::IsTimeForPing()
		/// Test with a range of m_pingInterval values.
		/// </summary>
		TEST_METHOD(IsTimeForPing_PingInterval)
		{
			// Data.
			ProgressiveConnectionState expirationState(Connected);
			DWORD lastHeartbeat(100000);
			DWORD pingIntervalIncrement(m_pingIntervalMinimumExpected + 1000);
			DWORD numberOfPingTests(20);
			AssertResult2((pingIntervalIncrement * numberOfPingTests) / numberOfPingTests, pingIntervalIncrement); // Verify no overflow.

			// Setup.
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;
			COperatingSystemTest operatingSystem; // So we can control the tick count.
			CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath, operatingSystem);

			progressiveConnection.UpdateConnectionState(expirationState, ProgressiveConnectionErrorNone);
			AssertResult2(progressiveConnection.m_connectionInfo.GetState(), expirationState);

			operatingSystem.m_tickCount32 = lastHeartbeat;
			progressiveConnection.SetLastHeartbeatTickCount(ToOneLink::ICardStatus);
			AssertResult2(progressiveConnection.m_connectionInfo.m_lastHeartbeatMessageTickCount, lastHeartbeat); // Verify setup.

			DWORD pingInterval(m_pingIntervalMinimumExpected);
			for (; m_testIndex < (int)numberOfPingTests; ++m_testIndex)
			{
				// Perform operation(s) and test(s).
				CProgressiveConfig varProgConfig;
				varProgConfig.SetPingInterval(pingInterval);
				progressiveConnection.LoadProgressiveConfig(&varProgConfig);
				AssertResult2(progressiveConnection.m_connectionInfo.m_pingInterval, pingInterval);

				operatingSystem.m_tickCount32 = lastHeartbeat - 1;
				AssertResult2(progressiveConnection.IsTimeForPing(), true);

				operatingSystem.m_tickCount32 = lastHeartbeat;
				AssertResult2(progressiveConnection.IsTimeForPing(), false);

				operatingSystem.m_tickCount32 = lastHeartbeat + pingInterval - 1;
				AssertResult2(progressiveConnection.IsTimeForPing(), false);

				operatingSystem.m_tickCount32 = lastHeartbeat + pingInterval;
				AssertResult2(progressiveConnection.IsTimeForPing(), true);

				// Setup for next loop iteration.
				pingInterval += pingIntervalIncrement;
			}

			// Set to disabled so connection shuts down
			progressiveConnection.UpdateConnectionState(Disabled);
		}
// #pragma endregion CProgressiveConnectionInfo::IsTimeForPing

// #pragma region CProgressiveConnectionInfo::IsTimeForPingInternalSendMessageToOneLink
		class CProgressiveConnectionSetupForPing
		{
		public:
			// Static data.
			static const DWORD m_tickCount0 = 10000;
			static const DWORD m_tickCount1 = 11000;
			static const DWORD m_pingInterval = m_pingIntervalMinimumExpected * 2;

			// Data with default constructors.
			CProgressiveConfig m_progressiveConfig;
			CProgressiveDataConsumerMock m_dataConsumer;
			COperatingSystemTest m_operatingSystem; // So we can control the tick count.
			CConnection m_connection;
			CGames m_games;

			// Data without default constructors.
			const bool m_isHeartbeat;
			const DWORD m_lastHeartbeatExpected;
			CProgressiveConnectionMock m_progressiveConnection;

			// Constructor.
			CProgressiveConnectionSetupForPing(bool isHeartbeat, ProgressiveConnectionState connectionState, const int &m_testIndex) :
				m_isHeartbeat(isHeartbeat),
				m_lastHeartbeatExpected(m_isHeartbeat ? m_tickCount1 : m_tickCount0),
				m_progressiveConnection(m_dataConsumer, &m_progressiveConfig, paltronicsCertificatePath, m_operatingSystem)
			{
				// Setup.
				ON_CALL(m_progressiveConnection, InternalSendMessageToOneLink(_, _, _)).WillByDefault(Invoke(&m_progressiveConnection, &CProgressiveConnectionMock::MockInternalSendMessageToOneLink));
				ON_CALL(m_progressiveConnection, SendMessageToOneLink(_, _)).WillByDefault(Invoke(&m_progressiveConnection, &CProgressiveConnectionMock::BaseSendMessageToOneLink));
				ON_CALL(m_progressiveConnection, SendCardStatus()).WillByDefault(Invoke(&m_progressiveConnection, &CProgressiveConnectionMock::BaseSendCardStatus));

				m_progressiveConnection.LoadDeviceConnectionData(&m_connection);
				AssertResult2(m_progressiveConnection.IsMessageFactoryCreated(), true); // Verify setup.

				CProgressiveConfig varProgConfig;
				varProgConfig.SetPingInterval(m_pingInterval);
				m_progressiveConnection.LoadProgressiveConfig(&varProgConfig);
				AssertResult2(m_progressiveConnection.m_connectionInfo.m_pingInterval, m_pingInterval); // Verify setup.

				m_progressiveConnection.SetGames(&m_games, nullptr);
				AssertResult2(m_progressiveConnection.IsGamesCreated(), true); // Verify setup.

				m_progressiveConnection.UpdateConnectionState(connectionState, ProgressiveConnectionErrorNone);
				AssertResult2(m_progressiveConnection.m_connectionInfo.GetState(), connectionState); // Verify setup.

				m_operatingSystem.m_tickCount32 = m_tickCount0;
				m_progressiveConnection.SetLastHeartbeatTickCount(ToOneLink::ICardStatus);
				AssertResult2(m_progressiveConnection.m_connectionInfo.m_lastHeartbeatMessageTickCount, m_tickCount0); // Verify setup.

				m_operatingSystem.m_tickCount32 = m_tickCount1;
			}

			~CProgressiveConnectionSetupForPing()
			{
				m_progressiveConnection.UpdateConnectionState(Disconnected);
			}

			void VerifyIsTimeForPing(ProgressiveConnectionState connectionState, int &m_testIndex)
			{
				// Verify setup.
				AssertResult2(m_progressiveConnection.m_connectionInfo.m_pingInterval, m_pingInterval);
				AssertResult2(m_progressiveConnection.m_connectionInfo.m_lastHeartbeatMessageTickCount, m_lastHeartbeatExpected);

				// Perform operation(s) and test(s).
				m_progressiveConnection.UpdateConnectionState(connectionState, ProgressiveConnectionErrorNone);
				AssertResult2(m_progressiveConnection.m_connectionInfo.GetState(), connectionState); // Verify setup.

				m_operatingSystem.m_tickCount32 = m_lastHeartbeatExpected - 1;
				AssertResult2(m_progressiveConnection.IsTimeForPing(), Connected == connectionState);

				m_operatingSystem.m_tickCount32 = m_lastHeartbeatExpected;
				AssertResult2(m_progressiveConnection.IsTimeForPing(), false);

				m_operatingSystem.m_tickCount32 = m_lastHeartbeatExpected + m_pingInterval - 1;
				AssertResult2(m_progressiveConnection.IsTimeForPing(), false);

				m_operatingSystem.m_tickCount32 = m_lastHeartbeatExpected + m_pingInterval;
				AssertResult2(m_progressiveConnection.IsTimeForPing(), Connected == connectionState);
			}
		};

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::LoadProgressiveConfig()
		///     CProgressiveConnection::SendCardConfigurationRequest()
		///     CProgressiveConnection::SendMessageToOneLink()
		///     CProgressiveConnection::SetLastHeartbeatTickCount()
		///     CProgressiveConnection::IsTimeForPing()
		/// Test with SendCardConfigurationRequest() while connected.
		/// </summary>
		TEST_METHOD(IsTimeForPing_SendCardConfigurationRequest)
		{
			// Setup.
			unique_ptr<CProgressiveConnectionSetupForPing> pcs(new CProgressiveConnectionSetupForPing(false, Connected, m_testIndex));

			// Perform operation(s) and test(s).
			pcs->m_progressiveConnection.SendCardConfigurationRequest();
			pcs->VerifyIsTimeForPing(Connected, m_testIndex);

			// Set to disbaled so connection shuts down
			pcs->m_progressiveConnection.UpdateConnectionState(Disabled);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::LoadProgressiveConfig()
		///     CProgressiveConnection::SendICardEvent()
		///     CProgressiveConnection::SendMessageToOneLink()
		///     CProgressiveConnection::SetLastHeartbeatTickCount()
		///     CProgressiveConnection::IsTimeForPing()
		/// Test with SendICardEvent() while connected.
		/// </summary>
		TEST_METHOD(IsTimeForPing_SendICardEvent)
		{
			// Setup.
			unique_ptr<CProgressiveConnectionSetupForPing> pcs(new CProgressiveConnectionSetupForPing(false, Connected, m_testIndex));

			// Perform operation(s) and test(s).
			pcs->m_progressiveConnection.SendICardEvent(GPE_NO_ACTIVITY);
			pcs->VerifyIsTimeForPing(Connected, m_testIndex);

			// Set to disbaled so connection shuts down
			pcs->m_progressiveConnection.UpdateConnectionState(Disabled);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::LoadProgressiveConfig()
		///     CProgressiveConnection::SendOfflineCoinInEvent()
		///     CProgressiveConnection::SendMessageToOneLink()
		///     CProgressiveConnection::SetLastHeartbeatTickCount()
		///     CProgressiveConnection::IsTimeForPing()
		/// Test with SendOfflineCoinInEvent() while connected.
		/// </summary>
		TEST_METHOD(IsTimeForPing_SendOfflineCoinInEvent)
		{
			// Setup.
			unique_ptr<CProgressiveConnectionSetupForPing> pcs(new CProgressiveConnectionSetupForPing(false, Connected, m_testIndex));

			// Perform operation(s) and test(s).
			pcs->m_progressiveConnection.SendOfflineCoinInEvent(1234567);
			pcs->VerifyIsTimeForPing(Connected, m_testIndex);

			// Set to disbaled so connection shuts down
			pcs->m_progressiveConnection.UpdateConnectionState(Disabled);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::LoadProgressiveConfig()
		///     CProgressiveConnection::SendProgressiveLevelHit()
		///     CProgressiveConnection::SendMessageToOneLink()
		///     CProgressiveConnection::SetLastHeartbeatTickCount()
		///     CProgressiveConnection::IsTimeForPing()
		/// Test with SendProgressiveLevelHit() while connected.
		/// </summary>
		TEST_METHOD(IsTimeForPing_SendProgressiveLevelHit)
		{
			// Setup.
			unique_ptr<CProgressiveConnectionSetupForPing> pcs(new CProgressiveConnectionSetupForPing(false, Connected, m_testIndex));

			// Perform operation(s) and test(s).
			const CEGMAward egmAward;
			pcs->m_progressiveConnection.SendProgressiveLevelHit(egmAward);
			pcs->VerifyIsTimeForPing(Connected, m_testIndex);

			// Set to disabled so connection shuts down
			pcs->m_progressiveConnection.UpdateConnectionState(Disabled);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::LoadProgressiveConfig()
		///     CProgressiveConnection::SendJackpotPaidRequest()
		///     CProgressiveConnection::SendMessageToOneLink()
		///     CProgressiveConnection::SetLastHeartbeatTickCount()
		///     CProgressiveConnection::IsTimeForPing()
		/// Test with SendJackpotPaidRequest() while connected.
		/// </summary>
		TEST_METHOD(IsTimeForPing_SendJackpotPaidRequest)
		{
			// Setup.
			unique_ptr<CProgressiveConnectionSetupForPing> pcs(new CProgressiveConnectionSetupForPing(false, Connected, m_testIndex));

			// Perform operation(s) and test(s).
			const CEGMAward egmAward;
			pcs->m_progressiveConnection.SendJackpotPaidRequest(egmAward);
			pcs->VerifyIsTimeForPing(Connected, m_testIndex);

			// Set to disabled so connection shuts down
			pcs->m_progressiveConnection.UpdateConnectionState(Disabled);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::LoadProgressiveConfig()
		///     CProgressiveConnection::SendCardStatus()
		///     CProgressiveConnection::SendMessageToOneLink()
		///     CProgressiveConnection::SetLastHeartbeatTickCount()
		///     CProgressiveConnection::IsTimeForPing()
		/// Test IsTimeForPing while connected after SendCardStatus() while connected.
		/// </summary>
		TEST_METHOD(IsTimeForPingConnected_SendCardStatusConnected)
		{
			// Setup.
			unique_ptr<CProgressiveConnectionSetupForPing> pcs(new CProgressiveConnectionSetupForPing(true, Connected, m_testIndex));

			// Perform operation(s) and test(s).
			pcs->m_progressiveConnection.SendCardStatus();
			pcs->VerifyIsTimeForPing(Connected, m_testIndex);

			// Set to disabled so connection shuts down
			pcs->m_progressiveConnection.UpdateConnectionState(Disabled);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::LoadProgressiveConfig()
		///     CProgressiveConnection::SendCardStatus()
		///     CProgressiveConnection::SendMessageToOneLink()
		///     CProgressiveConnection::SetLastHeartbeatTickCount()
		///     CProgressiveConnection::IsTimeForPing()
		/// Test IsTimeForPing while not connected after SendCardStatus() while connected.
		/// </summary>
		TEST_METHOD(IsTimeForPingNotConnected_SendCardStatusConnected)
		{
			// Setup.
			unique_ptr<CProgressiveConnectionSetupForPing> pcs(new CProgressiveConnectionSetupForPing(true, Connected, m_testIndex));

			// Perform operation(s) and test(s).
			pcs->m_progressiveConnection.SendCardStatus();
			pcs->VerifyIsTimeForPing(InDiscovery, m_testIndex);

			// Set to disabled so connection shuts down
			pcs->m_progressiveConnection.UpdateConnectionState(Disabled);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::LoadProgressiveConfig()
		///     CProgressiveConnection::SendCardStatus()
		///     CProgressiveConnection::SendMessageToOneLink()
		///     CProgressiveConnection::SetLastHeartbeatTickCount()
		///     CProgressiveConnection::IsTimeForPing()
		/// Test IsTimeForPing while connected after SendCardStatus() in a range of connection states.
		/// </summary>
		TEST_METHOD(IsTimeForPingConnected_SendCardStatus)
		{
			for (m_testIndex = (int)NotStarted - 1; m_testIndex <= (int)Error + 5; ++m_testIndex)
			{
				// Data.
				ProgressiveConnectionState connectionState((ProgressiveConnectionState)m_testIndex);
				bool isHeartbeat(InDiscovery == connectionState || Connected == connectionState);

				// Setup.
				unique_ptr<CProgressiveConnectionSetupForPing> pcs(new CProgressiveConnectionSetupForPing(isHeartbeat, connectionState, m_testIndex));

				// Perform operation(s) and test(s).
				pcs->m_progressiveConnection.SendCardStatus();
				pcs->VerifyIsTimeForPing(Connected, m_testIndex);

				// Set to disabled so connection shuts down
				pcs->m_progressiveConnection.UpdateConnectionState(Disabled);
			}
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::LoadProgressiveConfig()
		///     CProgressiveConnection::SendCoinIn()
		///     CProgressiveConnection::SendMessageToOneLink()
		///     CProgressiveConnection::SetLastHeartbeatTickCount()
		///     CProgressiveConnection::IsTimeForPing()
		/// Test IsTimeForPing while connected after SendCoinIn() while connected.
		/// </summary>
		TEST_METHOD(IsTimeForPingConnected_SendCoinInConnected)
		{
			// Setup.
			unique_ptr<CProgressiveConnectionSetupForPing> pcs(
				new CProgressiveConnectionSetupForPing(m_coinIn5IsHearbeat, Connected, m_testIndex));

			// Perform operation(s) and test(s).
			CGameEndData gameEndData(new CGame(MEMORY_NONE), 0, 0, 0, nullptr, 1);
			pcs->m_progressiveConnection.SendCoinIn(&gameEndData);
			pcs->VerifyIsTimeForPing(Connected, m_testIndex);

			// Set to disabled so connection shuts down
			pcs->m_progressiveConnection.UpdateConnectionState(Disabled);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::LoadProgressiveConfig()
		///     CProgressiveConnection::SendCoinIn()
		///     CProgressiveConnection::SendMessageToOneLink()
		///     CProgressiveConnection::SetLastHeartbeatTickCount()
		///     CProgressiveConnection::IsTimeForPing()
		/// Test IsTimeForPing while not connected after SendCoinIn() while connected.
		/// </summary>
		TEST_METHOD(IsTimeForPingNotConnected_SendCoinInConnected)
		{
			// Setup.
			unique_ptr<CProgressiveConnectionSetupForPing> pcs(
				new CProgressiveConnectionSetupForPing(m_coinIn5IsHearbeat, Connected, m_testIndex));

			// Perform operation(s) and test(s).
			CGameEndData gameEndData(new CGame(MEMORY_NONE), 0, 0, 0, nullptr, 1);
			pcs->m_progressiveConnection.SendCoinIn(&gameEndData);
			pcs->VerifyIsTimeForPing(InDiscovery, m_testIndex);

			// Set to disabled so connection shuts down
			pcs->m_progressiveConnection.UpdateConnectionState(Disabled);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::LoadProgressiveConfig()
		///     CProgressiveConnection::SendCoinIn()
		///     CProgressiveConnection::SendMessageToOneLink()
		///     CProgressiveConnection::SetLastHeartbeatTickCount()
		///     CProgressiveConnection::IsTimeForPing()
		/// Test IsTimeForPing while connected after SendCoinIn() while not connected.
		/// </summary>
		TEST_METHOD(IsTimeForPingConnected_SendCoinInInDiscovery)
		{
			// Setup.
			unique_ptr<CProgressiveConnectionSetupForPing> pcs(new CProgressiveConnectionSetupForPing(false, InDiscovery, m_testIndex));

			// Perform operation(s) and test(s).
			CGameEndData gameEndData(new CGame(MEMORY_NONE), 0, 0, 0, nullptr, 1);
			pcs->m_progressiveConnection.SendCoinIn(&gameEndData);
			pcs->VerifyIsTimeForPing(Connected, m_testIndex);

			// Set to disabled so connection shuts down
			pcs->m_progressiveConnection.UpdateConnectionState(Disabled);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::LoadProgressiveConfig()
		///     CProgressiveConnection::SendGameEnd()
		///     CProgressiveConnection::SendMessageToOneLink()
		///     CProgressiveConnection::SetLastHeartbeatTickCount()
		///     CProgressiveConnection::IsTimeForPing()
		/// Test IsTimeForPing while connected after SendGameEnd() while connected.
		/// </summary>
		TEST_METHOD(IsTimeForPingConnected_SendGameEndConnected)
		{
			// Setup.
			unique_ptr<CProgressiveConnectionSetupForPing> pcs(
				new CProgressiveConnectionSetupForPing(m_gameEnd5IsHearbeat, Connected, m_testIndex));

			// Perform operation(s) and test(s).
			CGameEndData gameEndData(new CGame(MEMORY_NONE), 0, 0, 0, nullptr, 1);
			pcs->m_progressiveConnection.SendGameEnd(&gameEndData);
			pcs->VerifyIsTimeForPing(Connected, m_testIndex);

			// Set to disabled so connection shuts down
			pcs->m_progressiveConnection.UpdateConnectionState(Disabled);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::LoadProgressiveConfig()
		///     CProgressiveConnection::SendGameEnd()
		///     CProgressiveConnection::SendMessageToOneLink()
		///     CProgressiveConnection::SetLastHeartbeatTickCount()
		///     CProgressiveConnection::IsTimeForPing()
		/// Test IsTimeForPing while not connected after SendGameEnd() while connected.
		/// </summary>
		TEST_METHOD(IsTimeForPingNotConnected_SendGameEndConnected)
		{
			// Setup.
			unique_ptr<CProgressiveConnectionSetupForPing> pcs(
				new CProgressiveConnectionSetupForPing(m_gameEnd5IsHearbeat, Connected, m_testIndex));

			// Perform operation(s) and test(s).
			CGameEndData gameEndData(new CGame(MEMORY_NONE), 0, 0, 0, nullptr, 1);
			pcs->m_progressiveConnection.SendGameEnd(&gameEndData);
			pcs->VerifyIsTimeForPing(InDiscovery, m_testIndex);

			// Set to disabled so connection shuts down
			pcs->m_progressiveConnection.UpdateConnectionState(Disabled);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::LoadProgressiveConfig()
		///     CProgressiveConnection::SendGameEnd()
		///     CProgressiveConnection::SendMessageToOneLink()
		///     CProgressiveConnection::SetLastHeartbeatTickCount()
		///     CProgressiveConnection::IsTimeForPing()
		/// Test IsTimeForPing while connected after SendGameEnd() while in discovery.
		/// </summary>
		TEST_METHOD(IsTimeForPingConnected_SendGameEndInDiscovery)
		{
			// Setup.
			unique_ptr<CProgressiveConnectionSetupForPing> pcs(new CProgressiveConnectionSetupForPing(false, InDiscovery, m_testIndex));

			// Perform operation(s) and test(s).
			CGameEndData gameEndData(new CGame(MEMORY_NONE), 0, 0, 0, nullptr, 1);
			pcs->m_progressiveConnection.SendGameEnd(&gameEndData);
			pcs->VerifyIsTimeForPing(Connected, m_testIndex);

			// Set to disabled so connection shuts down
			pcs->m_progressiveConnection.UpdateConnectionState(Disabled);
		}
// #pragma endregion CProgressiveConnectionInfo::IsTimeForPingInternalSendMessageToOneLink

// #pragma region CProgressiveConnectionInfo::SetDependentCommunicationStatus
		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		/// Test for default values.
		/// </summary>
		TEST_METHOD(SetDependentCommunicationStatus_Default)
		{
			// Setup.
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;
			CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);

			// Perform operation(s) and test(s).
			AssertResult2(progressiveConnection.m_dependentCommunicationsOK, m_dependentCommunicationsOKExpected);
			AssertResult2(progressiveConnection.m_multicastActive, m_multicastActiveExpected);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::SetDependentCommunicationStatus()
		/// Test setting m_dependentCommunicationsOK false and m_multicastActive false.
		/// </summary>
		TEST_METHOD(SetDependentCommunicationStatus_FalseFalse)
		{
			// Data.
			const bool dependentCommunicationsOKExpected(false);
			const  bool multicastActiveExpected(false);

			// Setup.
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;
			CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);

			// Perform operation(s) and test(s).
			progressiveConnection.SetDependentCommunicationStatus(dependentCommunicationsOKExpected, multicastActiveExpected);
			AssertResult2(progressiveConnection.m_dependentCommunicationsOK, dependentCommunicationsOKExpected);
			AssertResult2(progressiveConnection.m_multicastActive, multicastActiveExpected);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::SetDependentCommunicationStatus()
		/// Test setting m_dependentCommunicationsOK false and m_multicastActive true.
		/// </summary>
		TEST_METHOD(SetDependentCommunicationStatus_FalseTrue)
		{
			// Data.
			const bool dependentCommunicationsOKExpected(false);
			const  bool multicastActiveExpected(true);

			// Setup.
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;
			CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);

			// Perform operation(s) and test(s).
			progressiveConnection.SetDependentCommunicationStatus(dependentCommunicationsOKExpected, multicastActiveExpected);
			AssertResult2(progressiveConnection.m_dependentCommunicationsOK, dependentCommunicationsOKExpected);
			AssertResult2(progressiveConnection.m_multicastActive, multicastActiveExpected);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::SetDependentCommunicationStatus()
		/// Test setting m_dependentCommunicationsOK true and m_multicastActive false.
		/// </summary>
		TEST_METHOD(SetDependentCommunicationStatus_TrueFalse)
		{
			// Data.
			const bool dependentCommunicationsOKExpected(true);
			const  bool multicastActiveExpected(false);

			// Setup.
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;
			CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);

			// Perform operation(s) and test(s).
			progressiveConnection.SetDependentCommunicationStatus(dependentCommunicationsOKExpected, multicastActiveExpected);
			AssertResult2(progressiveConnection.m_dependentCommunicationsOK, dependentCommunicationsOKExpected);
			AssertResult2(progressiveConnection.m_multicastActive, multicastActiveExpected);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::SetDependentCommunicationStatus()
		/// Test setting m_dependentCommunicationsOK true and m_multicastActive true.
		/// </summary>
		TEST_METHOD(SetDependentCommunicationStatus_TrueTrue)
		{
			// Data.
			const bool dependentCommunicationsOKExpected(true);
			const  bool multicastActiveExpected(true);

			// Setup.
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;
			CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);

			// Perform operation(s) and test(s).
			progressiveConnection.SetDependentCommunicationStatus(dependentCommunicationsOKExpected, multicastActiveExpected);
			AssertResult2(progressiveConnection.m_dependentCommunicationsOK, dependentCommunicationsOKExpected);
			AssertResult2(progressiveConnection.m_multicastActive, multicastActiveExpected);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::SetDependentCommunicationStatus()
		/// Test setting m_dependentCommunicationsOK false and m_multicastActive false and then changing neither value.
		/// Verify the number of calls to SendCardStatus().
		/// </summary>
		TEST_METHOD(SetDependentCommunicationStatus_FalseFalse_ChangeFalseFalse)
		{
			// Data.
			const bool dependentCommunicationsOKExpected0(false);
			const  bool multicastActiveExpected0(false);
			const bool dependentCommunicationsOKExpected1(dependentCommunicationsOKExpected0);
			const  bool multicastActiveExpected1(multicastActiveExpected0);

			// Setup.
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;
			CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);

			progressiveConnection.SetDependentCommunicationStatus(dependentCommunicationsOKExpected0, multicastActiveExpected0);
			AssertResult2(progressiveConnection.m_dependentCommunicationsOK, dependentCommunicationsOKExpected0);
			AssertResult2(progressiveConnection.m_multicastActive, multicastActiveExpected0);

			Assert::IsTrue(Mock::VerifyAndClearExpectations(&progressiveConnection));
			EXPECT_CALL(progressiveConnection, SendCardStatus()).Times(0);

			// Perform operation(s) and test(s).
			progressiveConnection.SetDependentCommunicationStatus(dependentCommunicationsOKExpected1, multicastActiveExpected1);
			AssertResult2(progressiveConnection.m_dependentCommunicationsOK, dependentCommunicationsOKExpected1);
			AssertResult2(progressiveConnection.m_multicastActive, multicastActiveExpected1);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::SetDependentCommunicationStatus()
		/// Test setting m_dependentCommunicationsOK false and m_multicastActive false and then changing m_multicastActive only.
		/// Verify the number of calls to SendCardStatus().
		/// </summary>
		TEST_METHOD(SetDependentCommunicationStatus_FalseFalse_ChangeFalseTrue)
		{
			// Data.
			const bool dependentCommunicationsOKExpected0(false);
			const  bool multicastActiveExpected0(false);
			const bool dependentCommunicationsOKExpected1(dependentCommunicationsOKExpected0);
			const  bool multicastActiveExpected1(!multicastActiveExpected0);

			// Setup.
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;
			CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);

			progressiveConnection.SetDependentCommunicationStatus(dependentCommunicationsOKExpected0, multicastActiveExpected0);
			AssertResult2(progressiveConnection.m_dependentCommunicationsOK, dependentCommunicationsOKExpected0);
			AssertResult2(progressiveConnection.m_multicastActive, multicastActiveExpected0);

			Assert::IsTrue(Mock::VerifyAndClearExpectations(&progressiveConnection));
			EXPECT_CALL(progressiveConnection, SendCardStatus()).Times(1);

			// Perform operation(s) and test(s).
			progressiveConnection.SetDependentCommunicationStatus(dependentCommunicationsOKExpected1, multicastActiveExpected1);
			AssertResult2(progressiveConnection.m_dependentCommunicationsOK, dependentCommunicationsOKExpected1);
			AssertResult2(progressiveConnection.m_multicastActive, multicastActiveExpected1);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::SetDependentCommunicationStatus()
		/// Test setting m_dependentCommunicationsOK false and m_multicastActive false and then changing m_dependentCommunicationsOK only.
		/// Verify the number of calls to SendCardStatus().
		/// </summary>
		TEST_METHOD(SetDependentCommunicationStatus_FalseFalse_ChangeTrueFalse)
		{
			// Data.
			const bool dependentCommunicationsOKExpected0(false);
			const  bool multicastActiveExpected0(false);
			const bool dependentCommunicationsOKExpected1(!dependentCommunicationsOKExpected0);
			const  bool multicastActiveExpected1(!multicastActiveExpected0);

			// Setup.
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;
			CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);

			progressiveConnection.SetDependentCommunicationStatus(dependentCommunicationsOKExpected0, multicastActiveExpected0);
			AssertResult2(progressiveConnection.m_dependentCommunicationsOK, dependentCommunicationsOKExpected0);
			AssertResult2(progressiveConnection.m_multicastActive, multicastActiveExpected0);

			Assert::IsTrue(Mock::VerifyAndClearExpectations(&progressiveConnection));
			EXPECT_CALL(progressiveConnection, SendCardStatus()).Times(1);

			// Perform operation(s) and test(s).
			progressiveConnection.SetDependentCommunicationStatus(dependentCommunicationsOKExpected1, multicastActiveExpected1);
			AssertResult2(progressiveConnection.m_dependentCommunicationsOK, dependentCommunicationsOKExpected1);
			AssertResult2(progressiveConnection.m_multicastActive, multicastActiveExpected1);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::SetDependentCommunicationStatus()
		/// Test setting m_dependentCommunicationsOK false and m_multicastActive false and then changing both values.
		/// Verify the number of calls to SendCardStatus().
		/// </summary>
		TEST_METHOD(SetDependentCommunicationStatus_FalseFalse_ChangeTrueTrue)
		{
			// Data.
			const bool dependentCommunicationsOKExpected0(false);
			const  bool multicastActiveExpected0(false);
			const bool dependentCommunicationsOKExpected1(!dependentCommunicationsOKExpected0);
			const  bool multicastActiveExpected1(!multicastActiveExpected0);

			// Setup.
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;
			CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);

			progressiveConnection.SetDependentCommunicationStatus(dependentCommunicationsOKExpected0, multicastActiveExpected0);
			AssertResult2(progressiveConnection.m_dependentCommunicationsOK, dependentCommunicationsOKExpected0);
			AssertResult2(progressiveConnection.m_multicastActive, multicastActiveExpected0);

			Assert::IsTrue(Mock::VerifyAndClearExpectations(&progressiveConnection));
			EXPECT_CALL(progressiveConnection, SendCardStatus()).Times(1);

			// Perform operation(s) and test(s).
			progressiveConnection.SetDependentCommunicationStatus(dependentCommunicationsOKExpected1, multicastActiveExpected1);
			AssertResult2(progressiveConnection.m_dependentCommunicationsOK, dependentCommunicationsOKExpected1);
			AssertResult2(progressiveConnection.m_multicastActive, multicastActiveExpected1);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::SetDependentCommunicationStatus()
		/// Test setting m_dependentCommunicationsOK true and m_multicastActive true and then changing neither value.
		/// Verify the number of calls to SendCardStatus().
		/// </summary>
		TEST_METHOD(SetDependentCommunicationStatus_TrueTrue_ChangeFalseFalse)
		{
			// Data.
			const bool dependentCommunicationsOKExpected0(true);
			const  bool multicastActiveExpected0(true);
			const bool dependentCommunicationsOKExpected1(dependentCommunicationsOKExpected0);
			const  bool multicastActiveExpected1(multicastActiveExpected0);

			// Setup.
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;
			CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);

			progressiveConnection.SetDependentCommunicationStatus(dependentCommunicationsOKExpected0, multicastActiveExpected0);
			AssertResult2(progressiveConnection.m_dependentCommunicationsOK, dependentCommunicationsOKExpected0);
			AssertResult2(progressiveConnection.m_multicastActive, multicastActiveExpected0);

			Assert::IsTrue(Mock::VerifyAndClearExpectations(&progressiveConnection));
			EXPECT_CALL(progressiveConnection, SendCardStatus()).Times(0);

			// Perform operation(s) and test(s).
			progressiveConnection.SetDependentCommunicationStatus(dependentCommunicationsOKExpected1, multicastActiveExpected1);
			AssertResult2(progressiveConnection.m_dependentCommunicationsOK, dependentCommunicationsOKExpected1);
			AssertResult2(progressiveConnection.m_multicastActive, multicastActiveExpected1);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::SetDependentCommunicationStatus()
		/// Test setting m_dependentCommunicationsOK true and m_multicastActive true and then changing m_multicastActive only.
		/// Verify the number of calls to SendCardStatus().
		/// </summary>
		TEST_METHOD(SetDependentCommunicationStatus_TrueTrue_ChangeFalseTrue)
		{
			// Data.
			const bool dependentCommunicationsOKExpected0(true);
			const  bool multicastActiveExpected0(true);
			const bool dependentCommunicationsOKExpected1(dependentCommunicationsOKExpected0);
			const  bool multicastActiveExpected1(!multicastActiveExpected0);

			// Setup.
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;
			CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);

			progressiveConnection.SetDependentCommunicationStatus(dependentCommunicationsOKExpected0, multicastActiveExpected0);
			AssertResult2(progressiveConnection.m_dependentCommunicationsOK, dependentCommunicationsOKExpected0);
			AssertResult2(progressiveConnection.m_multicastActive, multicastActiveExpected0);

			Assert::IsTrue(Mock::VerifyAndClearExpectations(&progressiveConnection));
			EXPECT_CALL(progressiveConnection, SendCardStatus()).Times(1);

			// Perform operation(s) and test(s).
			progressiveConnection.SetDependentCommunicationStatus(dependentCommunicationsOKExpected1, multicastActiveExpected1);
			AssertResult2(progressiveConnection.m_dependentCommunicationsOK, dependentCommunicationsOKExpected1);
			AssertResult2(progressiveConnection.m_multicastActive, multicastActiveExpected1);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::SetDependentCommunicationStatus()
		/// Test setting m_dependentCommunicationsOK true and m_multicastActive true and then changing m_dependentCommunicationsOK only.
		/// Verify the number of calls to SendCardStatus().
		/// </summary>
		TEST_METHOD(SetDependentCommunicationStatus_TrueTrue_ChangeTrueFalse)
		{
			// Data.
			const bool dependentCommunicationsOKExpected0(true);
			const  bool multicastActiveExpected0(true);
			const bool dependentCommunicationsOKExpected1(!dependentCommunicationsOKExpected0);
			const  bool multicastActiveExpected1(!multicastActiveExpected0);

			// Setup.
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;
			CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);

			progressiveConnection.SetDependentCommunicationStatus(dependentCommunicationsOKExpected0, multicastActiveExpected0);
			AssertResult2(progressiveConnection.m_dependentCommunicationsOK, dependentCommunicationsOKExpected0);
			AssertResult2(progressiveConnection.m_multicastActive, multicastActiveExpected0);

			Assert::IsTrue(Mock::VerifyAndClearExpectations(&progressiveConnection));
			EXPECT_CALL(progressiveConnection, SendCardStatus()).Times(1);

			// Perform operation(s) and test(s).
			progressiveConnection.SetDependentCommunicationStatus(dependentCommunicationsOKExpected1, multicastActiveExpected1);
			AssertResult2(progressiveConnection.m_dependentCommunicationsOK, dependentCommunicationsOKExpected1);
			AssertResult2(progressiveConnection.m_multicastActive, multicastActiveExpected1);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::SetDependentCommunicationStatus()
		/// Test setting m_dependentCommunicationsOK true and m_multicastActive true and then changing both values.
		/// Verify the number of calls to SendCardStatus().
		/// </summary>
		TEST_METHOD(SetDependentCommunicationStatus_TrueTrue_ChangeTrueTrue)
		{
			// Data.
			const bool dependentCommunicationsOKExpected0(true);
			const  bool multicastActiveExpected0(true);
			const bool dependentCommunicationsOKExpected1(!dependentCommunicationsOKExpected0);
			const  bool multicastActiveExpected1(!multicastActiveExpected0);

			// Setup.
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;
			CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);

			progressiveConnection.SetDependentCommunicationStatus(dependentCommunicationsOKExpected0, multicastActiveExpected0);
			AssertResult2(progressiveConnection.m_dependentCommunicationsOK, dependentCommunicationsOKExpected0);
			AssertResult2(progressiveConnection.m_multicastActive, multicastActiveExpected0);

			Assert::IsTrue(Mock::VerifyAndClearExpectations(&progressiveConnection));
			EXPECT_CALL(progressiveConnection, SendCardStatus()).Times(1);

			// Perform operation(s) and test(s).
			progressiveConnection.SetDependentCommunicationStatus(dependentCommunicationsOKExpected1, multicastActiveExpected1);
			AssertResult2(progressiveConnection.m_dependentCommunicationsOK, dependentCommunicationsOKExpected1);
			AssertResult2(progressiveConnection.m_multicastActive, multicastActiveExpected1);
		}
// #pragma endregion CProgressiveConnectionInfo::SetDependentCommunicationStatus

// #pragma region CProgressiveConnectionInfo::SetDependentCommunicationStatus_SendCardStatus
		void SetDependentCommunicationStatus_SendCardStatus(bool dependentCommunicationsOKExpected, bool multicastActiveExpected)
		{
			// Setup.
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;
			CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);
			ON_CALL(progressiveConnection,
				InternalSendMessageToOneLink(_, _, _)).WillByDefault(Invoke(&progressiveConnection, &CProgressiveConnectionMock::MockInternalSendMessageToOneLink));
			ON_CALL(progressiveConnection,
				SendMessageToOneLink(_, _)).WillByDefault(Invoke(&progressiveConnection, &CProgressiveConnectionMock::BaseSendMessageToOneLink));
			ON_CALL(progressiveConnection,
				SendCardStatus()).WillByDefault(Invoke(&progressiveConnection, &CProgressiveConnectionMock::BaseSendCardStatus));

			CConnection connection;
			progressiveConnection.LoadDeviceConnectionData(&connection);
			AssertResult2(progressiveConnection.IsMessageFactoryCreated(), true); // Verify setup.

			progressiveConnection.UpdateConnectionState(Connected, ProgressiveConnectionErrorNone);
			AssertResult2(progressiveConnection.m_connectionInfo.GetState(), Connected); // Verify setup.

			// SetDependentCommunicationStatus to ensure a change during operation(s) under test.
			progressiveConnection.SetDependentCommunicationStatus(!dependentCommunicationsOKExpected, !multicastActiveExpected);
			AssertResult2(progressiveConnection.m_dependentCommunicationsOK, !dependentCommunicationsOKExpected); // Verify setup.
			AssertResult2(progressiveConnection.m_multicastActive, !multicastActiveExpected);

			EXPECT_CALL(progressiveConnection, SendCardStatus()).Times(1);
			EXPECT_CALL(progressiveConnection, SendMessageToOneLink(_, _)).Times(1);
			EXPECT_CALL(progressiveConnection, InternalSendMessageToOneLink(_, _, _)).Times(1);

			// Perform operation(s) and test(s).
			progressiveConnection.SetDependentCommunicationStatus(dependentCommunicationsOKExpected, multicastActiveExpected);
			AssertResult2(progressiveConnection.m_dependentCommunicationsOK, dependentCommunicationsOKExpected);
			AssertResult2(progressiveConnection.m_multicastActive, multicastActiveExpected);

			// Test that expected calls occurred.
			Assert::IsTrue(Mock::VerifyAndClearExpectations(&progressiveConnection));
			AssertResult2(progressiveConnection.m_lastMessageBodyToOneLink.length() > 0, true);

			{
				// Test that correct values were sent to the concentrator/producer.
				ProgressiveLevelTestHelpers::_ICardConfig5 config5;
				rapidjson::Document jsonDoc = config5.CreateJsonDoc();
				bool result = jsonDoc.Parse(progressiveConnection.m_lastMessageBodyToOneLink.c_str()).HasParseError();
				Assert::IsFalse(result, AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError())));

				JsonGetBoolTest(jsonDoc, pKeyPollingActive, dependentCommunicationsOKExpected);
				JsonGetBoolTest(jsonDoc, pKeyMulticastActive, multicastActiveExpected);
			}

			// Test that values are retained for subsequent SendCardStatus calls.
			EXPECT_CALL(progressiveConnection, SendCardStatus()).Times(1);
			EXPECT_CALL(progressiveConnection, SendMessageToOneLink(_, _)).Times(1);
			EXPECT_CALL(progressiveConnection, InternalSendMessageToOneLink(_, _, _)).Times(1);

			// Perform operation(s).
			progressiveConnection.SendCardStatus();

			// Test that expected calls occurred.
			Assert::IsTrue(Mock::VerifyAndClearExpectations(&progressiveConnection));
			AssertResult2(progressiveConnection.m_lastMessageBodyToOneLink.length() > 0, true);

			{
				// Test that correct values were sent to the concentrator/producer.
				ProgressiveLevelTestHelpers::_ICardConfig5 config5;
				rapidjson::Document jsonDoc = config5.CreateJsonDoc();
				bool result = jsonDoc.Parse(progressiveConnection.m_lastMessageBodyToOneLink.c_str()).HasParseError();
				Assert::IsFalse(result, AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError())));

				JsonGetBoolTest(jsonDoc, pKeyPollingActive, dependentCommunicationsOKExpected);
				JsonGetBoolTest(jsonDoc, pKeyMulticastActive, multicastActiveExpected);
			}

			// Set to disabled so connection shuts down
			progressiveConnection.UpdateConnectionState(Disabled);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::SendCardStatus()
		/// Test setting m_dependentCommunicationsOK false and m_multicastActive false.
		/// Verify values sent to concentrator/producer.
		/// </summary>
		TEST_METHOD(SetDependentCommunicationStatus_FalseFalse_SendCardStatus)
		{
			SetDependentCommunicationStatus_SendCardStatus(false, false);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::SendCardStatus()
		/// Test setting m_dependentCommunicationsOK false and m_multicastActive true.
		/// Verify values sent to concentrator/producer.
		/// </summary>
		TEST_METHOD(SetDependentCommunicationStatus_FalseTrue_SendCardStatus)
		{
			SetDependentCommunicationStatus_SendCardStatus(false, true);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::SendCardStatus()
		/// Test setting m_dependentCommunicationsOK true and m_multicastActive false.
		/// Verify values sent to concentrator/producer.
		/// </summary>
		TEST_METHOD(SetDependentCommunicationStatus_TrueFalse_SendCardStatus)
		{
			SetDependentCommunicationStatus_SendCardStatus(true, false);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::SendCardStatus()
		/// Test setting m_dependentCommunicationsOK true and m_multicastActive true.
		/// Verify values sent to concentrator/producer.
		/// </summary>
		TEST_METHOD(SetDependentCommunicationStatus_TrueTrue_SendCardStatus)
		{
			SetDependentCommunicationStatus_SendCardStatus(true, true);
		}

// #pragma endregion CProgressiveConnectionInfo::SetDependentCommunicationStatus_SendCardStatus
	};

	const CString ProgressiveConnectionUnitTests2::paltronicsCertificatePath;

	CString ProgressiveConnectionUnitTests2::m_gmockFailure;
	ProgressiveConnectionUnitTests2::CTersePrinter ProgressiveConnectionUnitTests2::m_tersePrinter;
}
#endif //TODO-PORT