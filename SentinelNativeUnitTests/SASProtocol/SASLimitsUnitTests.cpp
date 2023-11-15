#if 000//TODO-PORT
#include "stdafx.h"
#include "SASProtocol/SASLimits.h"

// Unit testing includes.
#include "UnitTestingToString.h"
#include "UnitTestingUtils.h"
#include "UnitTestingXmlData.h"
#include "UnitTestingXmlLite.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

// AssertResult macros for calling CUnitTestingUtils from a test method.
#define AssertResult3(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, m_testIndex, __TFUNCTION__, __LINE__);
#define AssertResult2(result, expected) AssertResult3(result, expected, _T(#result))

// AssertResult macro(s) for calling CUnitTestingUtils from a method with a "testValue".
#define AssertResult2t(result, expected) AssertResult3(result, expected, CStringW(_T("for testValue=")) + ToString(testValue).c_str() + _T(" ") + _T(#result))

// AssertResult macros for calling CUnitTestingUtils from a class initialization method.
#define AssertResult3i(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, 0, __TFUNCTION__, __LINE__);
#define AssertResult2i(result, expected) AssertResult3i(result, expected, _T(#result))

namespace SentinelNativeUnitTests
{
	/// <summary>
	/// Test CSASLimits.
	/// </summary>
	TEST_CLASS(SASLimits)
	{
// #pragma region ClassLevel
		// Expected value(s).
		static const DWORD m_maxReceivedDelayTickCountExpected = 6000;
		static const int m_minLevelExpected = 1;
		static const int m_maxLevelExpected = 32;
		static const int m_minGroupExpected = 1;
		static const int m_maxGroupExpected = 255;
		static const int m_levelCountExpected = 1;

		// Class member variable(s).
		int m_testIndex;

		/// <summary>
		/// Test static class members.
		/// </summary>
		void TestStaticMembers()
		{
			AssertResult2i(CProgressiveLevelForward::MaxInterval, m_maxReceivedDelayTickCountExpected);
			AssertResult2i(CSASLimits::MIN_SAS_PROGRESSIVE_LEVEL, m_minLevelExpected);
			AssertResult2i(CSASLimits::MAX_SAS_PROGRESSIVE_LEVEL, m_maxLevelExpected);
			AssertResult2i(CSASLimits::MIN_SAS_PROGRESSIVE_GROUP, m_minGroupExpected);
			AssertResult2i(CSASLimits::MAX_SAS_PROGRESSIVE_GROUP, m_maxGroupExpected);
		}

		/// <summary>
		/// Initialize class member(s) before each test method is run.
		/// </summary>
		TEST_METHOD_INITIALIZE(Initialize)
		{
			// Test method initialization code.
			m_testIndex = 0;

			// Perform static tests here so each test method gets a useful error message.
			TestStaticMembers();
		}

		/// <summary>
		/// Cleanup class member(s) before each test method is run.
		/// </summary>
		TEST_METHOD_CLEANUP(Cleanup)
		{
			// Test method cleanup code.
		}

		struct TestLevel
		{
			BYTE sasLevel;
			__int64 amount;
			ProgressiveLevelType::Type type;
			DWORD receivedTickCount;
			bool expectedResult;

			void Verify(const CProgressiveLevelForward::Level& level)
			{
				Assert::IsTrue(expectedResult, L"expectedResult");
				Assert::AreEqual(sasLevel, level.sasLevel, L"SAS Level");
				Assert::AreEqual((long)amount, (long)level.amount, L"amount");
				Assert::AreEqual((int)type, (int)level.type, L"type");
			}
		};
// #pragma endregion ClassLevel

		/// <summary>
		/// Test:
		///      CSASLimits::IsValidSasProgressiveGroup()
		/// Test various progressiveGroup values.
		/// </summary>
		TEST_METHOD(IsValidSasProgressiveGroupTest)
		{
			// Data.
			typedef int TestDataType;
			const TestDataType minValidValue(m_minGroupExpected);
			const TestDataType maxValidValue(m_maxGroupExpected);
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData(minValidValue, maxValidValue), &m_testIndex);
			auto isValidExpected = [=](TestDataType value) { return minValidValue <= value && value <= maxValidValue; };

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); m_testIndex++)
			{
				// Get the test data.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);

				// Perform operation(s) and test(s).
				AssertResult2t(CSASLimits::IsValidSasProgressiveGroup(testValue), isValidExpected(testValue));
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Test:
		///      CSASLimits::IsValidSasProgressiveLevel()
		/// Test various progressiveLevel values.
		/// </summary>
		TEST_METHOD(IsValidSasProgressiveLevelTest)
		{
			// Data.
			typedef BYTE TestDataType;
			const TestDataType minValidValue(m_minLevelExpected);
			const TestDataType maxValidValue(m_maxLevelExpected);
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData(minValidValue, maxValidValue), &m_testIndex);
			auto isValidExpected = [=](TestDataType value) { return minValidValue <= value && value <= maxValidValue; };

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); m_testIndex++)
			{
				// Get the test data.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);

				// Perform operation(s) and test(s).
				AssertResult2t(CSASLimits::IsValidSasProgressiveLevel(testValue), isValidExpected(testValue));
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Test:
		///      CSASLimits::IsValidSasProgressiveType()
		/// Test various progressiveType values.
		/// </summary>
		TEST_METHOD(IsValidSasProgressiveTypeTest)
		{
			// Data.
			typedef BYTE TestDataType;
			const TestDataType validValue((TestDataType)ProgressiveLevelType::Standard);
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData(validValue), &m_testIndex);
			auto isValidExpected = [=](TestDataType value) { return validValue == value; };

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); m_testIndex++)
			{
				// Get the test data.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);

				// Perform operation(s) and test(s).
				AssertResult2t(CSASLimits::IsValidSasProgressiveType((ProgressiveLevelType::Type)testValue), isValidExpected(testValue));
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Test:
		///      CSASLimits::IsValidSasProgressiveLevelReceivedTickCount()
		/// Test various receivedTickCount values.
		/// </summary>
		TEST_METHOD(IsValidSasProgressiveLevelReceivedTickCount_ReceivedTest)
		{
			// Data.
			typedef DWORD TestDataType;
			const TestDataType maxValidElapsedTickCount(m_maxReceivedDelayTickCountExpected);
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData(maxValidElapsedTickCount), &m_testIndex);

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); m_testIndex++)
			{
				// Get the test data.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);

				// Perform operation(s) and test(s).
				AssertResult2t(CSASLimits::IsValidSasProgressiveLevelReceivedTickCount(testValue, testValue - 1), false);
				AssertResult2t(CSASLimits::IsValidSasProgressiveLevelReceivedTickCount(testValue, testValue + 0), true);
				AssertResult2t(CSASLimits::IsValidSasProgressiveLevelReceivedTickCount(testValue, testValue + 1), true);
				AssertResult2t(CSASLimits::IsValidSasProgressiveLevelReceivedTickCount(testValue, testValue + maxValidElapsedTickCount - 1), true);
				AssertResult2t(CSASLimits::IsValidSasProgressiveLevelReceivedTickCount(testValue, testValue + maxValidElapsedTickCount + 0), true);
				AssertResult2t(CSASLimits::IsValidSasProgressiveLevelReceivedTickCount(testValue, testValue + maxValidElapsedTickCount + 1), false);
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Test:
		///      CSASLimits::IsValidSasProgressiveLevelReceivedTickCount()
		/// Test various current tickCount values.
		/// </summary>
		TEST_METHOD(IsValidSasProgressiveLevelReceivedTickCount_CurrentTest)
		{
			// Data.
			typedef DWORD TestDataType;
			const TestDataType maxValidElapsedTickCount(m_maxReceivedDelayTickCountExpected);
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData(maxValidElapsedTickCount), &m_testIndex);

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); m_testIndex++)
			{
				// Get the test data.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);

				// Perform operation(s) and test(s).
				AssertResult2t(CSASLimits::IsValidSasProgressiveLevelReceivedTickCount((testValue - maxValidElapsedTickCount) - 1, testValue), false);
				AssertResult2t(CSASLimits::IsValidSasProgressiveLevelReceivedTickCount((testValue - maxValidElapsedTickCount) + 0, testValue), true);
				AssertResult2t(CSASLimits::IsValidSasProgressiveLevelReceivedTickCount((testValue - maxValidElapsedTickCount) + 1, testValue), true);
				AssertResult2t(CSASLimits::IsValidSasProgressiveLevelReceivedTickCount(testValue - 1, testValue), true);
				AssertResult2t(CSASLimits::IsValidSasProgressiveLevelReceivedTickCount(testValue + 0, testValue), true);
				AssertResult2t(CSASLimits::IsValidSasProgressiveLevelReceivedTickCount(testValue + 1, testValue), false);
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Test:
		///      CSASLimits::IsValidSasProgressiveLevel()
		/// Test an otherwise valid level record with various progressiveLevel values.
		/// </summary>
		TEST_METHOD(IsValidSasProgressiveLevelRecord_LevelTest)
		{
			// Data.
			const __int64 amount(1000);
			const ProgressiveLevelType::Type type(ProgressiveLevelType::Standard);
			const DWORD receivedTickCount(10000);
			const DWORD tickCount(receivedTickCount);

			typedef BYTE TestDataType;
			const TestDataType minValidValue(m_minLevelExpected);
			const TestDataType maxValidValue(m_maxLevelExpected);
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData(minValidValue, maxValidValue), &m_testIndex);
			auto isValidExpected = [=](TestDataType value) { return minValidValue <= value && value <= maxValidValue; };

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); m_testIndex++)
			{
				// Get the test data.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);

				// Setup.
				const CProgressiveLevelForward::Level levelRecord(testValue, amount, type, receivedTickCount);

				// Perform operation(s) and test(s).
				AssertResult2t(CSASLimits::IsValidSasProgressiveLevelRecord(levelRecord, tickCount), isValidExpected(testValue));
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Test:
		///      CSASLimits::IsValidSasProgressiveLevel()
		/// Test an otherwise valid level record with various progressiveType values.
		/// </summary>
		TEST_METHOD(IsValidSasProgressiveLevelRecord_TypeTest)
		{
			// Data.
			const BYTE sasLevel(10);
			const __int64 amount(1000);
			const DWORD receivedTickCount(10000);
			const DWORD tickCount(receivedTickCount);

			typedef BYTE TestDataType;
			const TestDataType validValue((TestDataType)ProgressiveLevelType::Standard);
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData(validValue), &m_testIndex);
			auto isValidExpected = [=](TestDataType value) { return validValue == value; };

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); m_testIndex++)
			{
				// Get the test data.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);

				// Setup.
				const CProgressiveLevelForward::Level levelRecord(sasLevel, amount, (ProgressiveLevelType::Type)testValue, receivedTickCount);

				// Perform operation(s) and test(s).
				AssertResult2t(CSASLimits::IsValidSasProgressiveLevelRecord(levelRecord, tickCount), isValidExpected(testValue));
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Test:
		///      CSASLimits::IsValidSasProgressiveLevel()
		/// Test an otherwise valid level record with various receivedTickCount values.
		/// </summary>
		TEST_METHOD(IsValidSasProgressiveLevelRecord_ReceivedTickCountTest)
		{
			// Data.
			const BYTE sasLevel(10);
			const __int64 amount(1000);
			const ProgressiveLevelType::Type type(ProgressiveLevelType::Standard);

			typedef DWORD TestDataType;
			const TestDataType maxValidElapsedTickCount(m_maxReceivedDelayTickCountExpected);
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData(0 - maxValidElapsedTickCount), &m_testIndex);

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); m_testIndex++)
			{
				// Get the test data.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);

				// Setup.
				const CProgressiveLevelForward::Level levelRecord(sasLevel, amount, type, testValue);

				// Perform operation(s) and test(s).
				AssertResult2t(CSASLimits::IsValidSasProgressiveLevelRecord(levelRecord, testValue - 1), false);
				AssertResult2t(CSASLimits::IsValidSasProgressiveLevelRecord(levelRecord, testValue + 0), true);
				AssertResult2t(CSASLimits::IsValidSasProgressiveLevelRecord(levelRecord, testValue + 1), true);
				AssertResult2t(CSASLimits::IsValidSasProgressiveLevelRecord(levelRecord, testValue + maxValidElapsedTickCount - 1), true);
				AssertResult2t(CSASLimits::IsValidSasProgressiveLevelRecord(levelRecord, testValue + maxValidElapsedTickCount + 0), true);
				AssertResult2t(CSASLimits::IsValidSasProgressiveLevelRecord(levelRecord, testValue + maxValidElapsedTickCount + 1), false);
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Test:
		///      CSASLimits::IsValidSasProgressiveLevel()
		/// Test an otherwise valid level record with various (current) tickCount values.
		/// </summary>
		TEST_METHOD(IsValidSasProgressiveLevelRecord_CurrentTickCountTest)
		{
			// Data.
			const BYTE sasLevel(10);
			const __int64 amount(1000);
			const ProgressiveLevelType::Type type(ProgressiveLevelType::Standard);

			typedef DWORD TestDataType;
			const TestDataType maxValidElapsedTickCount(m_maxReceivedDelayTickCountExpected);
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData(maxValidElapsedTickCount), &m_testIndex);

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); m_testIndex++)
			{
				// Get the test data.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);

				// Setup.
				const CProgressiveLevelForward::Level levelRecord1(sasLevel, amount, type, (testValue - maxValidElapsedTickCount) - 1);
				const CProgressiveLevelForward::Level levelRecord2(sasLevel, amount, type, (testValue - maxValidElapsedTickCount) + 0);
				const CProgressiveLevelForward::Level levelRecord3(sasLevel, amount, type, (testValue - maxValidElapsedTickCount) + 1);
				const CProgressiveLevelForward::Level levelRecord4(sasLevel, amount, type, testValue - 1);
				const CProgressiveLevelForward::Level levelRecord5(sasLevel, amount, type, testValue + 0);
				const CProgressiveLevelForward::Level levelRecord6(sasLevel, amount, type, testValue + 1);

				// Perform operation(s) and test(s).
				AssertResult2t(CSASLimits::IsValidSasProgressiveLevelRecord(levelRecord1, testValue), false);
				AssertResult2t(CSASLimits::IsValidSasProgressiveLevelRecord(levelRecord2, testValue), true);
				AssertResult2t(CSASLimits::IsValidSasProgressiveLevelRecord(levelRecord3, testValue), true);
				AssertResult2t(CSASLimits::IsValidSasProgressiveLevelRecord(levelRecord4, testValue), true);
				AssertResult2t(CSASLimits::IsValidSasProgressiveLevelRecord(levelRecord5, testValue), true);
				AssertResult2t(CSASLimits::IsValidSasProgressiveLevelRecord(levelRecord6, testValue), false);
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Test:
		///     CSASLimits::ValidateProgressiveLevelForward()
		/// Test "default" data with no levels.
		/// </summary>
		TEST_METHOD(ValidateProgressiveLevelForward_Default)
		{
			const int groupId = 0;
			const bool useAftLockOnTransfers = false;
			const DWORD tickCount(0);
			const size_t numLevels(0);

			CProgressiveLevelForward data(groupId, useAftLockOnTransfers);

			// Perform operation(s) and test(s).
			CSASLimits::ValidateSasProgressiveLevels(data, tickCount);

			Assert::AreEqual(numLevels, data.GetNumLevels(), L"GetNumLevels()");
			Assert::IsTrue(data.cend() == data.cbegin(), L"cend cbegin");
			Assert::AreEqual(groupId, data.GetProgressiveGroupId(), L"GetProgressiveGroupId");
			Assert::AreEqual(useAftLockOnTransfers, data.UseAftLockOnTransfers(), L"UseAftLockOnTransfers");
		}

		/// <summary>
		/// Test:
		///      CSASLimits::ValidateProgressiveLevelForward()
		/// Test data with no levels and various progressiveGroupId values.
		/// </summary>
		TEST_METHOD(ValidateProgressiveLevelForward_Group0)
		{
			// Data.
			const bool useAftLockOnTransfers = true;
			const DWORD tickCount(25000);
			const size_t numLevels(0);

			typedef int TestDataType;
			const TestDataType minValidValue(m_minGroupExpected);
			const TestDataType maxValidValue(m_maxGroupExpected);
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData(minValidValue, maxValidValue), &m_testIndex);

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); m_testIndex++)
			{
				// Get the test data.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);
				
				// Setup.
				CProgressiveLevelForward data(testValue, useAftLockOnTransfers);

				// Perform operation(s) and test(s).
				CSASLimits::ValidateSasProgressiveLevels(data, tickCount);

				Assert::AreEqual(numLevels, data.GetNumLevels(), L"data.GetNumLevels()");
				Assert::IsTrue(data.cend() == data.cbegin(), L"cend cbegin");
				Assert::AreEqual(testValue, data.GetProgressiveGroupId(), L"GetProgressiveGroupId");
				Assert::AreEqual(useAftLockOnTransfers, data.UseAftLockOnTransfers(), L"UseAftLockOnTransfers");
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Test:
		///     CSASLimits::ValidateProgressiveLevelForward()
		/// Test data with two levels and various progressiveGroupId values.
		/// </summary>
		TEST_METHOD(ValidateProgressiveLevelForward_Group2)
		{
			// Data.
			const bool useAftLockOnTransfers = true;
			const DWORD tickCount(25000);
			const ProgressiveLevelType::Type type(ProgressiveLevelType::Standard);

			const size_t numLevels(2);
			const BYTE sasLevel0(10);
			const __int64 amount0(1000);
			const DWORD receivedTickCount0(tickCount - 10);
			const BYTE sasLevel1(11);
			const __int64 amount1(1100);
			const DWORD receivedTickCount1(tickCount - 11);

			typedef int TestDataType;
			const TestDataType minValidValue(m_minGroupExpected);
			const TestDataType maxValidValue(m_maxGroupExpected);
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData(minValidValue, maxValidValue), &m_testIndex);
			auto numLevelsExpected = [=](TestDataType value) { return minValidValue <= value && value <= maxValidValue ? numLevels : 0; };

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); m_testIndex++)
			{
				// Get the test data.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);

				// Setup.
				CProgressiveLevelForward data(testValue, useAftLockOnTransfers);
				data.AddLevel(sasLevel0, amount0, type, receivedTickCount0);
				data.AddLevel(sasLevel1, amount1, type, receivedTickCount1);

				// Perform operation(s) and test(s).
				CSASLimits::ValidateSasProgressiveLevels(data, tickCount);

				Assert::AreEqual(numLevelsExpected(testValue), data.GetNumLevels(), L"data.GetNumLevels()");
				Assert::AreEqual(testValue, data.GetProgressiveGroupId(), L"GetProgressiveGroupId");
				Assert::AreEqual(useAftLockOnTransfers, data.UseAftLockOnTransfers(), L"UseAftLockOnTransfers");

				if (0 == numLevelsExpected(testValue))
				{
					Assert::IsTrue(data.cend() == data.cbegin(), L"cend cbegin");
				}
				else
				{
					Assert::AreEqual(sasLevel0, data.cbegin()[0].sasLevel, L"sasLevel");
					Assert::AreEqual(amount0, data.cbegin()[0].amount, L"amount");
					Assert::AreEqual(type, data.cbegin()[0].type, L"type");
					Assert::AreEqual(receivedTickCount0, data.cbegin()[0].updatedTickCount, L"type");

					Assert::AreEqual(sasLevel1, data.cbegin()[1].sasLevel, L"sasLevel");
					Assert::AreEqual(amount1, data.cbegin()[1].amount, L"amount");
					Assert::AreEqual(type, data.cbegin()[1].type, L"type");
					Assert::AreEqual(receivedTickCount1, data.cbegin()[1].updatedTickCount, L"type");
				}
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Test:
		/// CSASLimits::ValidateProgressiveLevelForward()
		/// Test data with two levels where the 1st level is invalid or stale.
		/// </summary>
		TEST_METHOD(ValidateProgressiveLevelForward_Erase1of2)
		{
			// Data.
			const int progressiveGroupId(55);
			const bool useAftLockOnTransfers = true;
			const DWORD tickCount(10000);
			const DWORD receivedTickCount(tickCount);

			// Invalid sasLevel.
			const BYTE sasLevel0(0);
			const __int64 amount0(1000);
			const ProgressiveLevelType::Type type0(ProgressiveLevelType::Standard);
			const DWORD receivedTickCount0(tickCount - 10);

			// Valid.
			const BYTE sasLevel1(11);
			const __int64 amount1(1100);
			const ProgressiveLevelType::Type type1(ProgressiveLevelType::Standard);
			const DWORD receivedTickCount1(tickCount - 11);

			const size_t numLevelsExpected(1);

			// Setup.
			CProgressiveLevelForward data(progressiveGroupId, useAftLockOnTransfers);
			data.AddLevel(sasLevel0, amount0, type0, receivedTickCount0);
			data.AddLevel(sasLevel1, amount1, type1, receivedTickCount1);

			// Perform operation(s) and test(s).
			CSASLimits::ValidateSasProgressiveLevels(data, tickCount);

			Assert::AreEqual(numLevelsExpected, data.GetNumLevels(), L"GetNumLevels()");
			Assert::AreEqual(progressiveGroupId, data.GetProgressiveGroupId(), L"GetProgressiveGroupId");
			Assert::AreEqual(useAftLockOnTransfers, data.UseAftLockOnTransfers(), L"UseAftLockOnTransfers");

			Assert::AreEqual(sasLevel1, data.cbegin()[0].sasLevel, L"sasLevel");
			Assert::AreEqual(amount1, data.cbegin()[0].amount, L"amount");
			Assert::AreEqual(type1, data.cbegin()[0].type, L"type");
			Assert::AreEqual(receivedTickCount1, data.cbegin()[0].updatedTickCount, L"type");
		}

		/// <summary>
		/// Test:
		/// CSASLimits::ValidateProgressiveLevelForward()
		/// Test data with two levels where the 2nd level is invalid or stale.
		/// </summary>
		TEST_METHOD(ValidateProgressiveLevelForward_Erase2of2)
		{
			// Data.
			const int progressiveGroupId(55);
			const bool useAftLockOnTransfers = true;
			const DWORD tickCount(10000);
			const DWORD receivedTickCount(tickCount);

			// Valid.
			const BYTE sasLevel0(10);
			const __int64 amount0(1000);
			const ProgressiveLevelType::Type type0(ProgressiveLevelType::Standard);
			const DWORD receivedTickCount0(tickCount - 10);

			// Invalid type.
			const BYTE sasLevel1(11);
			const __int64 amount1(1100);
			const ProgressiveLevelType::Type type1(ProgressiveLevelType::Bonus);
			const DWORD receivedTickCount1(tickCount - 11);

			const size_t numLevelsExpected(1);

			// Setup.
			CProgressiveLevelForward data(progressiveGroupId, useAftLockOnTransfers);
			data.AddLevel(sasLevel0, amount0, type0, receivedTickCount0);
			data.AddLevel(sasLevel1, amount1, type1, receivedTickCount1);

			// Perform operation(s) and test(s).
			CSASLimits::ValidateSasProgressiveLevels(data, tickCount);

			Assert::AreEqual(numLevelsExpected, data.GetNumLevels(), L"GetNumLevels()");
			Assert::AreEqual(progressiveGroupId, data.GetProgressiveGroupId(), L"GetProgressiveGroupId");
			Assert::AreEqual(useAftLockOnTransfers, data.UseAftLockOnTransfers(), L"UseAftLockOnTransfers");

			Assert::AreEqual(sasLevel0, data.cbegin()[0].sasLevel, L"sasLevel");
			Assert::AreEqual(amount0, data.cbegin()[0].amount, L"amount");
			Assert::AreEqual(type0, data.cbegin()[0].type, L"type");
			Assert::AreEqual(receivedTickCount0, data.cbegin()[0].updatedTickCount, L"type");
		}

		/// <summary>
		/// Test:
		/// CSASLimits::ValidateProgressiveLevelForward()
		/// Test data with two levels where the 1st and 2nd levels are invalid or stale.
		/// </summary>
		TEST_METHOD(ValidateProgressiveLevelForward_Erase12of2)
		{
			// Data.
			const int progressiveGroupId(55);
			const bool useAftLockOnTransfers = true;
			const DWORD tickCount(10000);
			const DWORD receivedTickCount(tickCount);

			// Invalid sasLevel.
			const BYTE sasLevel0(0);
			const __int64 amount0(1000);
			const ProgressiveLevelType::Type type0(ProgressiveLevelType::Standard);
			const DWORD receivedTickCount0(tickCount - 10);

			// Invalid type.
			const BYTE sasLevel1(11);
			const __int64 amount1(1100);
			const ProgressiveLevelType::Type type1(ProgressiveLevelType::Bonus);
			const DWORD receivedTickCount1(tickCount - 11);

			const size_t numLevelsExpected(0);

			// Setup.
			CProgressiveLevelForward data(progressiveGroupId, useAftLockOnTransfers);
			data.AddLevel(sasLevel0, amount0, type0, receivedTickCount0);
			data.AddLevel(sasLevel1, amount1, type1, receivedTickCount1);

			// Perform operation(s) and test(s).
			CSASLimits::ValidateSasProgressiveLevels(data, tickCount);

			Assert::AreEqual(numLevelsExpected, data.GetNumLevels(), L"GetNumLevels()");
			Assert::AreEqual(progressiveGroupId, data.GetProgressiveGroupId(), L"GetProgressiveGroupId");
			Assert::AreEqual(useAftLockOnTransfers, data.UseAftLockOnTransfers(), L"UseAftLockOnTransfers");
		}

		/// <summary>
		/// Test:
		/// CSASLimits::ValidateProgressiveLevelForward()
		/// Test data with five levels where the 1st, 3rd, and 5th levels are invalid or stale.
		/// </summary>
		TEST_METHOD(ValidateProgressiveLevelForward_Erase135of5)
		{
			// Data.
			const int progressiveGroupId(55);
			const bool useAftLockOnTransfers = true;
			const DWORD tickCount(10000);
			const DWORD receivedTickCount(tickCount);

			// Invalid sasLevel.
			const BYTE sasLevel0(0);
			const __int64 amount0(1000);
			const ProgressiveLevelType::Type type0(ProgressiveLevelType::Standard);
			const DWORD receivedTickCount0(tickCount - 10);

			// Valid.
			const BYTE sasLevel1(11);
			const __int64 amount1(1100);
			const ProgressiveLevelType::Type type1(ProgressiveLevelType::Standard);
			const DWORD receivedTickCount1(tickCount - 11);

			// Stale receivedTickCount.
			const BYTE sasLevel2(12);
			const __int64 amount2(1200);
			const ProgressiveLevelType::Type type2(ProgressiveLevelType::Standard);
			const DWORD receivedTickCount2(tickCount - (m_maxReceivedDelayTickCountExpected + 100));

			// Valid.
			const BYTE sasLevel3(13);
			const __int64 amount3(1300);
			const ProgressiveLevelType::Type type3(ProgressiveLevelType::Standard);
			const DWORD receivedTickCount3(tickCount - 13);

			// Invalid type.
			const BYTE sasLevel4(14);
			const __int64 amount4(1400);
			const ProgressiveLevelType::Type type4(ProgressiveLevelType::Bonus);
			const DWORD receivedTickCount4(tickCount - 14);

			const size_t numLevelsExpected(2);

			// Setup.
			CProgressiveLevelForward data(progressiveGroupId, useAftLockOnTransfers);
			data.AddLevel(sasLevel0, amount0, type0, receivedTickCount0);
			data.AddLevel(sasLevel1, amount1, type1, receivedTickCount1);
			data.AddLevel(sasLevel2, amount2, type2, receivedTickCount2);
			data.AddLevel(sasLevel3, amount3, type3, receivedTickCount3);
			data.AddLevel(sasLevel4, amount4, type4, receivedTickCount4);

			// Perform operation(s) and test(s).
			CSASLimits::ValidateSasProgressiveLevels(data, tickCount);

			Assert::AreEqual(numLevelsExpected, data.GetNumLevels(), L"GetNumLevels()");
			Assert::AreEqual(progressiveGroupId, data.GetProgressiveGroupId(), L"GetProgressiveGroupId");
			Assert::AreEqual(useAftLockOnTransfers, data.UseAftLockOnTransfers(), L"UseAftLockOnTransfers");

			Assert::AreEqual(sasLevel1, data.cbegin()[0].sasLevel, L"sasLevel");
			Assert::AreEqual(amount1, data.cbegin()[0].amount, L"amount");
			Assert::AreEqual(type1, data.cbegin()[0].type, L"type");
			Assert::AreEqual(receivedTickCount1, data.cbegin()[0].updatedTickCount, L"type");

			Assert::AreEqual(sasLevel3, data.cbegin()[1].sasLevel, L"sasLevel");
			Assert::AreEqual(amount3, data.cbegin()[1].amount, L"amount");
			Assert::AreEqual(type3, data.cbegin()[1].type, L"type");
			Assert::AreEqual(receivedTickCount3, data.cbegin()[1].updatedTickCount, L"type");
		}

		/// <summary>
		/// Test:
		/// CSASLimits::ValidateProgressiveLevelForward()
		/// Test data with five levels where the 2nd and 4th levels are invalid or stale.
		/// </summary>
		TEST_METHOD(ValidateProgressiveLevelForward_Erase24of5)
		{
			// Data.
			const int progressiveGroupId(55);
			const bool useAftLockOnTransfers = true;
			const DWORD tickCount(10000);
			const DWORD receivedTickCount(tickCount);

			// Valid.
			const BYTE sasLevel0(10);
			const __int64 amount0(1000);
			const ProgressiveLevelType::Type type0(ProgressiveLevelType::Standard);
			const DWORD receivedTickCount0(tickCount - 10);

			// Invalid sasLevel.
			const BYTE sasLevel1(0);
			const __int64 amount1(1100);
			const ProgressiveLevelType::Type type1(ProgressiveLevelType::Standard);
			const DWORD receivedTickCount1(tickCount - 11);

			// Valid.
			const BYTE sasLevel2(12);
			const __int64 amount2(1200);
			const ProgressiveLevelType::Type type2(ProgressiveLevelType::Standard);
			const DWORD receivedTickCount2(tickCount - 12);

			// Invalid type.
			const BYTE sasLevel3(13);
			const __int64 amount3(1300);
			const ProgressiveLevelType::Type type3(ProgressiveLevelType::Bonus);
			const DWORD receivedTickCount3(tickCount - 13);

			// Valid.
			const BYTE sasLevel4(14);
			const __int64 amount4(1400);
			const ProgressiveLevelType::Type type4(ProgressiveLevelType::Standard);
			const DWORD receivedTickCount4(tickCount - 14);

			const size_t numLevelsExpected(3);

			// Setup.
			CProgressiveLevelForward data(progressiveGroupId, useAftLockOnTransfers);
			data.AddLevel(sasLevel0, amount0, type0, receivedTickCount0);
			data.AddLevel(sasLevel1, amount1, type1, receivedTickCount1);
			data.AddLevel(sasLevel2, amount2, type2, receivedTickCount2);
			data.AddLevel(sasLevel3, amount3, type3, receivedTickCount3);
			data.AddLevel(sasLevel4, amount4, type4, receivedTickCount4);

			// Perform operation(s) and test(s).
			CSASLimits::ValidateSasProgressiveLevels(data, tickCount);

			Assert::AreEqual(numLevelsExpected, data.GetNumLevels(), L"GetNumLevels()");
			Assert::AreEqual(progressiveGroupId, data.GetProgressiveGroupId(), L"GetProgressiveGroupId");
			Assert::AreEqual(useAftLockOnTransfers, data.UseAftLockOnTransfers(), L"UseAftLockOnTransfers");

			Assert::AreEqual(sasLevel0, data.cbegin()[0].sasLevel, L"sasLevel");
			Assert::AreEqual(amount0, data.cbegin()[0].amount, L"amount");
			Assert::AreEqual(type0, data.cbegin()[0].type, L"type");
			Assert::AreEqual(receivedTickCount0, data.cbegin()[0].updatedTickCount, L"type");

			Assert::AreEqual(sasLevel2, data.cbegin()[1].sasLevel, L"sasLevel");
			Assert::AreEqual(amount2, data.cbegin()[1].amount, L"amount");
			Assert::AreEqual(type2, data.cbegin()[1].type, L"type");
			Assert::AreEqual(receivedTickCount2, data.cbegin()[1].updatedTickCount, L"type");

			Assert::AreEqual(sasLevel4, data.cbegin()[2].sasLevel, L"sasLevel");
			Assert::AreEqual(amount4, data.cbegin()[2].amount, L"amount");
			Assert::AreEqual(type4, data.cbegin()[2].type, L"type");
			Assert::AreEqual(receivedTickCount4, data.cbegin()[2].updatedTickCount, L"type");
		}

		/// <summary>
		/// Test:
		/// CSASLimits::ValidateProgressiveLevelForward()
		/// Test data with five levels where the 2nd, 3rd, and 4th levels are invalid or stale.
		/// </summary>
		TEST_METHOD(ValidateProgressiveLevelForward_Erase234of5)
		{
			// Data.
			const int progressiveGroupId(55);
			const bool useAftLockOnTransfers = true;
			const DWORD tickCount(10000);
			const DWORD receivedTickCount(tickCount);

			// Valid.
			const BYTE sasLevel0(10);
			const __int64 amount0(1000);
			const ProgressiveLevelType::Type type0(ProgressiveLevelType::Standard);
			const DWORD receivedTickCount0(tickCount - 10);

			// Invalid sasLevel.
			const BYTE sasLevel1(0);
			const __int64 amount1(1100);
			const ProgressiveLevelType::Type type1(ProgressiveLevelType::Standard);
			const DWORD receivedTickCount1(tickCount - 11);

			// Stale receivedTickCount.
			const BYTE sasLevel2(12);
			const __int64 amount2(1200);
			const ProgressiveLevelType::Type type2(ProgressiveLevelType::Standard);
			const DWORD receivedTickCount2(tickCount - (m_maxReceivedDelayTickCountExpected + 100));

			// Invalid type.
			const BYTE sasLevel3(13);
			const __int64 amount3(1300);
			const ProgressiveLevelType::Type type3(ProgressiveLevelType::Bonus);
			const DWORD receivedTickCount3(tickCount - 13);

			// Valid.
			const BYTE sasLevel4(14);
			const __int64 amount4(1400);
			const ProgressiveLevelType::Type type4(ProgressiveLevelType::Standard);
			const DWORD receivedTickCount4(tickCount - 14);

			const size_t numLevelsExpected(2);

			// Setup.
			CProgressiveLevelForward data(progressiveGroupId, useAftLockOnTransfers);
			data.AddLevel(sasLevel0, amount0, type0, receivedTickCount0);
			data.AddLevel(sasLevel1, amount1, type1, receivedTickCount1);
			data.AddLevel(sasLevel2, amount2, type2, receivedTickCount2);
			data.AddLevel(sasLevel3, amount3, type3, receivedTickCount3);
			data.AddLevel(sasLevel4, amount4, type4, receivedTickCount4);

			// Perform operation(s) and test(s).
			CSASLimits::ValidateSasProgressiveLevels(data, tickCount);

			Assert::AreEqual(numLevelsExpected, data.GetNumLevels(), L"GetNumLevels()");
			Assert::AreEqual(progressiveGroupId, data.GetProgressiveGroupId(), L"GetProgressiveGroupId");
			Assert::AreEqual(useAftLockOnTransfers, data.UseAftLockOnTransfers(), L"UseAftLockOnTransfers");

			Assert::AreEqual(sasLevel0, data.cbegin()[0].sasLevel, L"sasLevel");
			Assert::AreEqual(amount0, data.cbegin()[0].amount, L"amount");
			Assert::AreEqual(type0, data.cbegin()[0].type, L"type");
			Assert::AreEqual(receivedTickCount0, data.cbegin()[0].updatedTickCount, L"type");

			Assert::AreEqual(sasLevel4, data.cbegin()[1].sasLevel, L"sasLevel");
			Assert::AreEqual(amount4, data.cbegin()[1].amount, L"amount");
			Assert::AreEqual(type4, data.cbegin()[1].type, L"type");
			Assert::AreEqual(receivedTickCount4, data.cbegin()[1].updatedTickCount, L"type");
		}
	};
}
#endif //TODO-PORT