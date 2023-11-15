#include "stdafx.h"
#include "../Sentinel/Hardware/MCRAutoTest.h"

// Unit testing includes.
#include "UnitTestingOperatingSystem.h"
#include "UnitTestingUtils.h"
#include "DirectoryManager.h"
#include "gtest/gtest.h"

using namespace std;

namespace SentinelUnitTests
{
// AssertResult macro(s) for calling CUnitTestingUtils from a test method.
#define AssertResult3(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, testIndex, __TFUNCTION__, __LINE__);
#define AssertResult2(result, expected) AssertResult3(result, expected, #result)

// AssertResult macros for calling CUnitTestingUtils from a helper method.
#define AssertResult3h(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, testIndex, functionName, lineNumber, extraIndex);
#define AssertResult2h(result, expected) AssertResult3h(result, expected, #result)

const LPCTSTR m_globalGlobalCardNumber("6909FF0001");

class McrAutoTest : public ::testing::Test
	{
    public:
		/// <summary>
		/// Initialize class member(s) before each test method is run.
		/// </summary>
		virtual void SetUp()
        {
            CDirectoryManager::MockInstance();
        }
        /// <summary>
		/// Cleanup class member(s) before each test method is run.
		/// </summary>
		virtual void TearDown()
		{
		}

        void McrAutoTest_Test();
        void GetCardEvent1Test();
        void GetCardEvent4Test();
        void GetCardEvent_TimerWrapCardInTest();
        void GetCardEvent_TimerWrapCardOutTest();
        void GetCardEvent_Universe1Test();
        void GetCardEvent_Universe4Test();
        void RefreshConfigTest();

		// Assert when CMCRAutoTest::GetCardEvent() should return a NULL card.
		void AssertEqual(
			bool bInserted,
			bool bRemoved,
			bool bTimeout,
			const CCard *card,
			bool bInsertedExpected,
			bool bRemovedExpected,
			bool bTimeoutExpected,
			int testIndex,
			LPCTSTR functionName,
			int lineNumber,
			const int* extraIndex = NULL)
		{
			AssertResult2h(bInserted, bInsertedExpected);
			AssertResult2h(bRemoved, bRemovedExpected);
			AssertResult2h(bTimeout, bTimeoutExpected);
			AssertResult2h(card, NULL);
		}

		// Assert when CMCRAutoTest::GetCardEvent() should return a non-NULL card.
		void AssertEqual(
			bool bInserted,
			bool bRemoved,
			bool bTimeout,
			const CCard *card,
			bool bInsertedExpected,
			bool bRemovedExpected,
			bool bTimeoutExpected,
			LPCTSTR cardNumberExpected,
			time_t cardInTimeExpected,
			time_t utcCardInTimeExpected,
			BYTE commIDExpected,
			bool isUniverseCardExpected,
			int testIndex,
			LPCTSTR functionName,
			int lineNumber,
			const int* extraIndex = NULL)
		{
			AssertResult2h(bInserted, bInsertedExpected);
			AssertResult2h(bRemoved, bRemovedExpected);
			AssertResult2h(bTimeout, bTimeoutExpected);
			CUnitTestingUtils::AssertNotNull(card, "card", testIndex, functionName, lineNumber, extraIndex);

			int cardIDLen(0);
			AssertResult2h(CUtilities::MakeStringFromCardID(card->GetCardID(cardIDLen)), std::string(cardNumberExpected));
			AssertResult2h(card->Compare(cardNumberExpected), true);
			AssertResult2h((DWORD)card->GetCardInTime(), (DWORD)cardInTimeExpected);
			AssertResult2h((DWORD)card->GetUTCCardInTime(), (DWORD)utcCardInTimeExpected);

			AssertResult2h(card->CheckCommID(commIDExpected), true); // Try to check the commID.
			AssertResult2h(card->GetIsUniversalCard(), isUniverseCardExpected);
			if (!isUniverseCardExpected)
			{
				if (0 != _tcscmp(cardNumberExpected, m_globalGlobalCardNumber))
				{
					AssertResult2h(card->CheckCommID((BYTE)(commIDExpected + 1)), false);
				}
			}
		}

		// Assert when CMCRAutoTest::GetCardEvent() should return a card in and a card out.
		void AssertCardInCardOut(
			CMCRAutoTest &obj,
			DWORD tickCountInExpected,
			DWORD tickCountOutExpected,
			LPCTSTR cardNumberExpected,
			time_t cardInTimeExpected,
			time_t utcCardInTimeExpected,
			BYTE commIDExpected,
			bool isUniverseCardExpected,
			int testIndex,
			LPCTSTR functionName,
			int lineNumber)
		{
			int extraIndex(0);
			{
				// Check that card in has not occurred yet.
				bool bInserted(false);
				bool bRemoved(false);
				bool bTimeout(false);
				CCard *card = obj.GetCardEvent(bInserted, bRemoved, bTimeout, tickCountInExpected - 1);
				AssertEqual(bInserted, bRemoved, bTimeout, card, false, false, true,
					testIndex, functionName, lineNumber, &extraIndex);
				delete card;
			}
 
			++extraIndex;
			{
				// Check that card in has occurred.
				bool bInserted(false);
				bool bRemoved(false);
				bool bTimeout(false);
				CCard *card = obj.GetCardEvent(bInserted, bRemoved, bTimeout, tickCountInExpected);
				AssertEqual(bInserted, bRemoved, bTimeout, card, true, false, false,
					cardNumberExpected, cardInTimeExpected, utcCardInTimeExpected, commIDExpected, isUniverseCardExpected,
					testIndex, functionName, lineNumber, &extraIndex);
				delete card;
			}

			++extraIndex;
			{
				// Check that card out has not occurred yet.
				bool bInserted(false);
				bool bRemoved(false);
				bool bTimeout(false);
				CCard *card = obj.GetCardEvent(bInserted, bRemoved, bTimeout, tickCountOutExpected - 1);
				AssertEqual(bInserted, bRemoved, bTimeout, card, false, false, true,
					testIndex, functionName, lineNumber, &extraIndex);
				delete card;
			}

			++extraIndex;
			{
				// Check that card out has occurred.
				bool bInserted(false);
				bool bRemoved(false);
				bool bTimeout(false);
				CCard *card = obj.GetCardEvent(bInserted, bRemoved, bTimeout, tickCountOutExpected);
				AssertEqual(bInserted, bRemoved, bTimeout, card, false, true, false,
					testIndex, functionName, lineNumber, &extraIndex);
				delete card;
			}

		}

    };
		
		/// <summary>
		/// Code coverage and functional test for:
		///     CMCRAutoTest::CMCRAutoTest()
		///     CMCRAutoTest::GetCardEvent()
		/// Code coverage:
		///     CMCRAutoTest::~CMCRAutoTest()
		/// Test with configuration file not present.
		/// </summary>
		TEST_F(McrAutoTest, McrAutoTest_Test)
		{
            McrAutoTest_Test();
        }
        void McrAutoTest::McrAutoTest_Test()
		{	// Input(s).
			const LPCTSTR fileName("");
			const DWORD tickCount0(0);
			const DWORD tickCount1(INT32_MAX / 2);

			// Setup.
			const int testIndex(0);
			const CConfig config(false, "");
			const DWORD tickCount(0);
			COperatingSystemTest operatingSystem(fileName,
				std::ios::out,
				NULL,
				0);

			// Setup.
			CMCRAutoTest obj(config, tickCount, operatingSystem);

			// Test.
			AssertResult2(obj.m_cardReaderReady, false);

			{
				// Test #1. Are inputs altered?
				bool bInserted(false);
				bool bRemoved(false);
				bool bTimeout(false);
				CCard *card = obj.GetCardEvent(bInserted, bRemoved, bTimeout, tickCount1);
				AssertEqual(bInserted, bRemoved, bTimeout, card, false, false, false,
					testIndex, __TFUNCTION__, __LINE__);

				// Cleanup.
				delete card;
			}

			{
				// Test #2. Are inputs altered?
				bool bInserted(true);
				bool bRemoved(true);
				bool bTimeout(true);
				CCard *card = obj.GetCardEvent(bInserted, bRemoved, bTimeout, tickCount1);
				AssertEqual(bInserted, bRemoved, bTimeout, card, true, true, true,
					testIndex, __TFUNCTION__, __LINE__);

				// Cleanup.
				delete card;
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CMCRAutoTest::CMCRAutoTest()
		///     CMCRAutoTest::GetCardEvent()
		/// Code coverage:
		///     CMCRAutoTest::~CMCRAutoTest()
		/// Test one card in with configuration file present.
		/// </summary>
		TEST_F(McrAutoTest, GetCardEvent1Test)
		{
           GetCardEvent1Test();
        }
        void McrAutoTest::GetCardEvent1Test()
		{	// Input(s).
			const BYTE fileContent[] = "AA01234567BB12345678CC23456789"; // Typical files include trailing carriage-return linefeed.
			const UINT fileLength = sizeof(fileContent) - 1; // Omit trailing NUL character.
			const LPCTSTR fileName("/tmp/ncompass/sys/SentinelData/CardList.txt");
			const BYTE commID(3);
			const bool universeEnabled(false);
			const DWORD tickCount0(0);
			const DWORD tickCount1(INT32_MAX / 2);

			// Setup.
			const int testIndex(0);
			CConfig varConfig(false, "");
			varConfig.SetCommID(commID);
			varConfig.SetUniverseEnabled(universeEnabled);
			const CConfig &config(varConfig);
			COperatingSystemTest operatingSystem(fileName,
				std::ios::out,
				fileContent,
				fileLength);
            // create file to be read by CMCRAutoTest
            {
                ofstream file(fileName);
                if (file.is_open())
                {
                    file.write((char*)fileContent, fileLength);
                }
            }
           	// Setup.
			CMCRAutoTest obj(config, tickCount0, operatingSystem);

			// Test.
			AssertResult2(obj.m_cardReaderReady, true);

			bool bInserted(false);
			bool bRemoved(false);
			bool bTimeout(false);
			CCard *card = obj.GetCardEvent(bInserted, bRemoved, bTimeout, tickCount1);

			// The CMCRAutoTest constructor has already accessed GetCurrentTime() once.
			AssertEqual(bInserted, bRemoved, bTimeout, card, true, false, false,
				"AA01234567", CUtilities::GetCurrentTime() /*CUtilities::GetTimeFromFields(2014, 05, 15, 12, 34, 57)*/, CUtilities::GetUTCTime() /*CUtilities::GetTimeFromFields(2014, 05, 15, 02, 34, 58)*/, commID, universeEnabled,
				testIndex, __TFUNCTION__, __LINE__);

			// Cleanup.
			delete card;
            remove(fileName);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CMCRAutoTest::CMCRAutoTest()
		///     CMCRAutoTest::GetCardEvent()
		/// Code coverage:
		///     CMCRAutoTest::~CMCRAutoTest()
		/// Test 4 card ins and outs with configuration file present.
		/// </summary>
		TEST_F(McrAutoTest, GetCardEvent4Test)
		{
            GetCardEvent4Test();
        }
        void McrAutoTest::GetCardEvent4Test()
		{	// Input(s).
			const BYTE fileContent[] = "AA01234567BB12345678CC23456789"; // Typical files include trailing carriage-return linefeed.
			const UINT fileLength = sizeof(fileContent) - 1; // Omit trailing NUL character.
			const LPCTSTR fileName("/tmp/ncompass/sys/SentinelData/CardList.txt");
			const BYTE commID(3);
			const bool universeEnabled(false);

			const DWORD tickCountsToFirstEvent(30000); // How long until first event.
			const DWORD tickCountsForCardIn(35000); // How long will card in last.
			const DWORD tickCountsForCardOut(20000); // How long will card out last.
			const DWORD tickCount0(0);
			const DWORD tickCountIn1(tickCount0 + tickCountsToFirstEvent); // First card in.
			const DWORD tickCountOut1(tickCountIn1 + tickCountsForCardIn); // First card out.
			const DWORD tickCountIn2(tickCountOut1 + tickCountsForCardOut); // Next card in.
			const DWORD tickCountOut2(tickCountIn2 + tickCountsForCardIn); // Next card out.
			const DWORD tickCountIn3(tickCountOut2 + tickCountsForCardOut); // Next card in.
			const DWORD tickCountOut3(tickCountIn3 + tickCountsForCardIn); // Next card out.

			// Setup.
			CConfig varConfig(false, "");
			varConfig.SetCommID(commID);
			varConfig.SetUniverseEnabled(universeEnabled);
			const CConfig &config(varConfig);
			COperatingSystemTest operatingSystem(fileName,
				std::ios::out,
				fileContent,
				fileLength);
             // create file to be read by CMCRAutoTest
            {
                ofstream file(fileName);
                if (file.is_open())
                {
                    file.write((char*)fileContent, fileLength);
                }
            }
			// Setup.
			CMCRAutoTest obj(config, tickCount0, operatingSystem);

			// Test.
			// The CMCRAutoTest constructor has already accessed GetCurrentTime() once.
			int testIndex(0);
			AssertResult2(obj.m_cardReaderReady, true);

            AssertCardInCardOut(obj, tickCountIn1, tickCountOut1,
				"AA01234567", CUtilities::GetCurrentTime(), CUtilities::GetUTCTime(), commID, universeEnabled,
				testIndex, __TFUNCTION__, __LINE__);

            ++testIndex;
			operatingSystem.m_randRangeOffset = 1;
			AssertCardInCardOut(obj, tickCountIn2, tickCountOut2,
				"BB12345678", CUtilities::GetCurrentTime(), CUtilities::GetUTCTime(), commID, universeEnabled,
				testIndex, __TFUNCTION__, __LINE__);

            ++testIndex;
            operatingSystem.m_randRangeOffset = 2;
			AssertCardInCardOut(obj, tickCountIn3, tickCountOut3,
				"CC23456789", CUtilities::GetCurrentTime(), CUtilities::GetUTCTime(), commID, universeEnabled,
				testIndex, __TFUNCTION__, __LINE__);

            remove(fileName);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CMCRAutoTest::CMCRAutoTest()
		///     CMCRAutoTest::GetCardEvent()
		/// Code coverage:
		///     CMCRAutoTest::~CMCRAutoTest()
		/// Test timer wrap while waiting for a card in event.
		/// </summary>
		TEST_F(McrAutoTest, GetCardEvent_TimerWrapCardInTest)
		{
            GetCardEvent_TimerWrapCardInTest();
        }
        void McrAutoTest::GetCardEvent_TimerWrapCardInTest()
		{	// Input(s).
			const BYTE fileContent[] = "AA01234567BB12345678CC23456789"; // Typical files include trailing carriage-return linefeed.
			const UINT fileLength = sizeof(fileContent) - 1; // Omit trailing NUL character.
			const LPCTSTR fileName("/tmp/ncompass/sys/SentinelData/CardList.txt");
			const BYTE commID(3);
			const bool universeEnabled(false);

			const DWORD tickCountsToFirstEvent(30000); // How long until first event.
			const DWORD tickCountsForCardIn(35000); // How long will card in last.
			const DWORD tickCountsForCardOut(20000); // How long will card out last.
			const DWORD tickCount0(DWORD_MAX - tickCountsToFirstEvent / 2);
			const DWORD tickCountIn1((DWORD)(tickCount0 + (ULONGLONG)tickCountsToFirstEvent)); // First card in.
			const DWORD tickCountOut1(tickCountIn1 + tickCountsForCardIn); // First card out.

			// Setup.
			CConfig varConfig(false, "");
			varConfig.SetCommID(commID);
			varConfig.SetUniverseEnabled(universeEnabled);
			const CConfig &config(varConfig);
			COperatingSystemTest operatingSystem(fileName,
				std::ios::out,
				fileContent,
				fileLength);
             // create file to be read by CMCRAutoTest
            {
                ofstream file(fileName);
                if (file.is_open())
                {
                    file.write((char*)fileContent, fileLength);
                }
            }
			// Setup.
			CMCRAutoTest obj(config, tickCount0, operatingSystem);

			// Test.
			// The CMCRAutoTest constructor has already accessed GetCurrentTime() once.
			int testIndex(0);
			AssertResult2(obj.m_cardReaderReady, true);

			operatingSystem.m_randRangeOffset = 2;
			AssertCardInCardOut(obj, tickCountIn1, tickCountOut1,
				"CC23456789", CUtilities::GetCurrentTime(), CUtilities::GetUTCTime(), commID, universeEnabled,
				testIndex, __TFUNCTION__, __LINE__);

            remove(fileName);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CMCRAutoTest::CMCRAutoTest()
		///     CMCRAutoTest::GetCardEvent()
		/// Code coverage:
		///     CMCRAutoTest::~CMCRAutoTest()
		/// Test timer wrap while waiting for a card out event.
		/// </summary>
		TEST_F(McrAutoTest, GetCardEvent_TimerWrapCardOutTest)
		{
            GetCardEvent_TimerWrapCardOutTest();
        }
        void McrAutoTest::GetCardEvent_TimerWrapCardOutTest()
		{	// Input(s).
			const BYTE fileContent[] = "AA01234567BB12345678CC23456789"; // Typical files include trailing carriage-return linefeed.
			const UINT fileLength = sizeof(fileContent) - 1; // Omit trailing NUL character.
			const LPCTSTR fileName("/tmp/ncompass/sys/SentinelData/CardList.txt");
			const BYTE commID(3);
			const bool universeEnabled(false);

			const DWORD tickCountsToFirstEvent(30000); // How long until first event.
			const DWORD tickCountsForCardIn(35000); // How long will card in last.
			const DWORD tickCountsForCardOut(20000); // How long will card out last.
			const DWORD tickCount0(DWORD_MAX - tickCountsToFirstEvent - tickCountsForCardIn / 2);
			const DWORD tickCountIn1(tickCount0 + tickCountsToFirstEvent); // First card in.
			const DWORD tickCountOut1((DWORD) (tickCountIn1 + (ULONGLONG) tickCountsForCardIn)); // First card out.

			// Setup.
			CConfig varConfig(false, "");
			varConfig.SetCommID(commID);
			varConfig.SetUniverseEnabled(universeEnabled);
			const CConfig &config(varConfig);
			COperatingSystemTest operatingSystem(fileName,
				std::ios::out,
				fileContent,
				fileLength);
             // create file to be read by CMCRAutoTest
            {
                ofstream file(fileName);
                if (file.is_open())
                {
                    file.write((char*)fileContent, fileLength);
                }
            }
			// Setup.
			CMCRAutoTest obj(config, tickCount0, operatingSystem);

			// Test.
			// The CMCRAutoTest constructor has already accessed GetCurrentTime() once.
			int testIndex(0);
			AssertResult2(obj.m_cardReaderReady, true);

			operatingSystem.m_randRangeOffset = 1;
			AssertCardInCardOut(obj, tickCountIn1, tickCountOut1,
				"BB12345678", CUtilities::GetCurrentTime(), CUtilities::GetUTCTime(), commID, universeEnabled,
				testIndex, __TFUNCTION__, __LINE__);

            remove(fileName);
		}

#if 000//TODO-PORT
		/// <summary>
		/// Code coverage and functional test for:
		///     CMCRAutoTest::CMCRAutoTest()
		///     CMCRAutoTest::GetCardEvent()
		/// Code coverage:
		///     CMCRAutoTest::~CMCRAutoTest()
		/// Test one card in with configuration file present and Universe enabled.
		/// </summary>
		TEST_F(McrAutoTest, GetCardEvent_Universe1Test)
		{
            GetCardEvent_Universe1Test();
        }
        void McrAutoTest::GetCardEvent_Universe1Test()
		{	// Input(s).
			const BYTE fileContent[] = "AA01234567BB12345678CC23456789"; // Typical files include trailing carriage-return linefeed.
			const UINT fileLength = sizeof(fileContent) - 1; // Omit trailing NUL character.
			const LPCTSTR fileName("/tmp/ncompass/sys/SentinelData/CardList.txt");
			const BYTE commID(3);
			const BYTE universePrefixes[] = { 0xAA, 0xCC };
			const bool universeEnabled(true);
			const DWORD tickCount0(0);
			const DWORD tickCount1(INT32_MAX / 2);

			// Setup.
			const int testIndex(0);
			CConfig varConfig(false, "");
			varConfig.SetCommID(commID);
			varConfig.SetUniversePrefixes(universePrefixes, sizeof(universePrefixes));
			varConfig.SetUniverseEnabled(universeEnabled);
			const CConfig &config(varConfig);
			COperatingSystemTest operatingSystem(fileName,
				std::ios::out,
				fileContent,
				fileLength);
             // create file to be read by CMCRAutoTest
            {
                ofstream file(fileName);
                if (file.is_open())
                {
                    file.write((char*)fileContent, fileLength);
                }
            }
			// Setup.
			CMCRAutoTest obj(config, tickCount0, operatingSystem);

			// Test.
			AssertResult2(obj.m_cardReaderReady, true);

			bool bInserted(false);
			bool bRemoved(false);
			bool bTimeout(false);
			operatingSystem.m_randRangeOffset = 0; // Explicitly set offset.
			CCard *card = obj.GetCardEvent(bInserted, bRemoved, bTimeout, tickCount1);

			// The CMCRAutoTest constructor has already accessed GetCurrentTime() once.
			AssertEqual(bInserted, bRemoved, bTimeout, card, true, false, false,
				"AA01234567", CUtilities::GetCurrentTime(), CUtilities::GetUTCTime(), commID, universeEnabled,
				testIndex, __TFUNCTION__, __LINE__);

			// Cleanup.
			delete card;
            remove(fileName);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CMCRAutoTest::CMCRAutoTest()
		///     CMCRAutoTest::GetCardEvent()
		/// Code coverage:
		///     CMCRAutoTest::~CMCRAutoTest()
		/// Test 4 card ins and outs with configuration file present and Universe enabled.
		/// </summary>
		TEST_F(McrAutoTest, GetCardEvent_Universe4Test)
		{
            GetCardEvent_Universe4Test();
        }
        void McrAutoTest::GetCardEvent_Universe4Test()
		{	// Input(s).
			const BYTE fileContent[] = "AA01234567BB12345678CC23456789"; // Typical files include trailing carriage-return linefeed.
			const UINT fileLength = sizeof(fileContent) - 1; // Omit trailing NUL character.
			const LPCTSTR fileName("/tmp/ncompass/sys/SentinelData/CardList.txt");
			const BYTE commID(3);
			const BYTE universePrefixes[] = { 0xAA, 0xCC };
			const bool universeEnabled(true);

			const DWORD tickCountsToFirstEvent(30000); // How long until first event.
			const DWORD tickCountsForCardIn(35000); // How long will card in last.
			const DWORD tickCountsForCardOut(20000); // How long will card out last.
			const DWORD tickCount0(0);
			const DWORD tickCountIn1(tickCount0 + tickCountsToFirstEvent); // First card in.
			const DWORD tickCountOut1(tickCountIn1 + tickCountsForCardIn); // First card out.
			const DWORD tickCountIn2(tickCountOut1 + tickCountsForCardOut); // Next card in.
			const DWORD tickCountOut2(tickCountIn2 + tickCountsForCardIn); // Next card out.
			const DWORD tickCountIn3(tickCountOut2 + tickCountsForCardOut); // Next card in.
			const DWORD tickCountOut3(tickCountIn3 + tickCountsForCardIn); // Next card out.

			// Setup.
			CConfig varConfig(false, "");
			varConfig.SetCommID(commID);
			varConfig.SetUniversePrefixes(universePrefixes, sizeof(universePrefixes));
			varConfig.SetUniverseEnabled(universeEnabled);
			const CConfig &config(varConfig);
			COperatingSystemTest operatingSystem(fileName,
				std::ios::out,
				fileContent,
				fileLength);
             // create file to be read by CMCRAutoTest
            {
                ofstream file(fileName);
                if (file.is_open())
                {
                    file.write((char*)fileContent, fileLength);
                }
            }
			// Setup.
			CMCRAutoTest obj(config, tickCount0, operatingSystem);

			// Test.
			// The CMCRAutoTest constructor has already accessed GetCurrentTime() once.
			int testIndex(0);
			AssertResult2(obj.m_cardReaderReady, true);

			operatingSystem.m_randRangeOffset = 2;
			AssertCardInCardOut(obj, tickCountIn1, tickCountOut1,
				"CC23456789", CUtilities::GetCurrentTime(), CUtilities::GetUTCTime(), commID, true,
				testIndex, __TFUNCTION__, __LINE__);

			++testIndex;
			operatingSystem.m_randRangeOffset = 1;
			AssertCardInCardOut(obj, tickCountIn2, tickCountOut2,
				"BB12345678", CUtilities::GetCurrentTime(), CUtilities::GetUTCTime(), commID, false,
				testIndex, __TFUNCTION__, __LINE__);

			++testIndex;
			operatingSystem.m_randRangeOffset = 0;
			AssertCardInCardOut(obj, tickCountIn3, tickCountOut3,
				"AA01234567", CUtilities::GetCurrentTime(), CUtilities::GetUTCTime(), commID, true,
				testIndex, __TFUNCTION__, __LINE__);

            remove(fileName);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CMCRAutoTest::CMCRAutoTest()
		///     CMCRAutoTest::GetCardEvent()
		/// Code coverage:
		///     CMCRAutoTest::~CMCRAutoTest()
		/// Test 4 card ins and outs with configuration file present and Universe configuration varying.
		/// </summary>
		TEST_F(McrAutoTest, RefreshConfigTest)
		{
            RefreshConfigTest();
        }
        void McrAutoTest::RefreshConfigTest()
		{	// Input(s).
			const BYTE fileContent[] = "AA01234567BB12345678CC23456789"; // Typical files include trailing carriage-return linefeed.
			const UINT fileLength = sizeof(fileContent) - 1; // Omit trailing NUL character.
			const LPCTSTR fileName("/tmp/ncompass/sys/SentinelData/CardList.txt");
			const BYTE commID(3);
			const BYTE universePrefixes[] = { 0xAA, 0xCC };
			const bool universeEnabled(true);

			const DWORD tickCountsToFirstEvent(30000); // How long until first event.
			const DWORD tickCountsForCardIn(35000); // How long will card in last.
			const DWORD tickCountsForCardOut(20000); // How long will card out last.
			const DWORD tickCount0(0);
			const DWORD tickCountIn1(tickCount0 + tickCountsToFirstEvent); // First card in.
			const DWORD tickCountOut1(tickCountIn1 + tickCountsForCardIn); // First card out.
			const DWORD tickCountIn2(tickCountOut1 + tickCountsForCardOut); // Next card in.
			const DWORD tickCountOut2(tickCountIn2 + tickCountsForCardIn); // Next card out.
			const DWORD tickCountIn3(tickCountOut2 + tickCountsForCardOut); // Next card in.
			const DWORD tickCountOut3(tickCountIn3 + tickCountsForCardIn); // Next card out.


			// Setup.
			CConfig varConfig(false, "");
			varConfig.SetCommID(commID);
			varConfig.SetUniversePrefixes(universePrefixes, sizeof(universePrefixes));
			varConfig.SetUniverseEnabled(!universeEnabled);
			const CConfig &config(varConfig);
			COperatingSystemTest operatingSystem(fileName,
				std::ios::out,
				fileContent,
				fileLength);
             // create file to be read by CMCRAutoTest
            {
                ofstream file(fileName);
                if (file.is_open())
                {
                    file.write((char*)fileContent, fileLength);
                }
            }
			// Setup.
			CMCRAutoTest obj(config, tickCount0, operatingSystem);

			// Test.
			// The CMCRAutoTest constructor has already accessed GetCurrentTime() once.
			int testIndex(0);
			AssertResult2(obj.m_cardReaderReady, true);

			operatingSystem.m_randRangeOffset = 2;
			AssertCardInCardOut(obj, tickCountIn1, tickCountOut1,
				"CC23456789", CUtilities::GetCurrentTime(), CUtilities::GetUTCTime(), commID, false,
				testIndex, __TFUNCTION__, __LINE__);


            // Reconfigure.                                                                         
            varConfig.SetUniverseEnabled(universeEnabled);                                          
            obj.RefreshConfig(config);                                                              
                                                                                                    
            ++testIndex;                                                                            
            operatingSystem.m_randRangeOffset = 1;                                                  
            AssertCardInCardOut(obj, tickCountIn2, tickCountOut2,                                   
                "BB12345678", CUtilities::GetCurrentTime(), CUtilities::GetUTCTime(), commID, false,
                testIndex, __TFUNCTION__, __LINE__);                                                
                                                                                                    
            ++testIndex;                                                                            
            operatingSystem.m_randRangeOffset = 0;                                                  
            AssertCardInCardOut(obj, tickCountIn3, tickCountOut3,                                   
                "AA01234567", CUtilities::GetCurrentTime(), CUtilities::GetUTCTime(), commID, true, 
                testIndex, __TFUNCTION__, __LINE__);                                                


            remove(fileName);
		}

#endif//TODO-PORT
}
