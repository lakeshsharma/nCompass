#if 000//TODO-PORT
#include "stdafx.h"
#include "SASProtocol/SASGeneralPoll.h"

// Unit testing includes.
#include "UnitTestingUtils.h"
#include "UnitTestingXmlData.h"
#include "UnitTestingXmlLite.h"

// AssertResult macros for calling CUnitTestingUtils from a test method.
#define AssertResult3(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, m_testIndex, __TFUNCTION__, __LINE__);
#define AssertResult2(result, expected) AssertResult3(result, expected, _T(#result))

// AssertResult macros for calling CUnitTestingUtils from a class initialization method.
#define AssertResult3i(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, 0, __TFUNCTION__, __LINE__);
#define AssertResult2i(result, expected) AssertResult3i(result, expected, _T(#result))

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SentinelNativeUnitTests
{
	TEST_CLASS(SASGeneralPoll)
	{
		// Default expected result(s).
		static const BYTE m_pollValueExpected = (BYTE)-1;

		// Class member(s) used by test (data) methods.
		int m_testIndex;

		/// <summary>
		/// Initialize and/or test static class members.
		/// </summary>
		TEST_CLASS_INITIALIZE(ClassInitialize)
		{
			// Test static default value(s).
			AssertResult2i(CSASGeneralPoll::POLL_VALUE, m_pollValueExpected);
		}

		/// <summary>
		/// Cleanup static class member(s).
		/// </summary>
		TEST_CLASS_CLEANUP(ClassCleanup)
		{
			// Class cleanup code.
		}

		/// <summary>
		/// Initialize class member(s) before each test method is run.
		/// </summary>
		TEST_METHOD_INITIALIZE(Initialize)
		{
			// Test method initialization code.
			m_testIndex = 0;
		}

		/// <summary>
		/// Cleanup class member(s) before each test method is run.
		/// </summary>
		TEST_METHOD_CLEANUP(Cleanup)
		{
			// Test method cleanup code.
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASGeneralPoll::IsGeneralPoll()
		/// Test return value.
		/// </summary>
		TEST_METHOD(IsGeneralPollTest)
		{
			// Data.
			const bool isGeneralPollExpected(true);

			typedef BYTE TestDataType;
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData((TestDataType)0), &m_testIndex);

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); m_testIndex++)
			{
				// Get the test data.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);

				// Setup.
				CSASGeneralPoll obj(testValue);

				// Perform operation(s) and test(s).
				AssertResult2(obj.IsGeneralPoll(), isGeneralPollExpected);
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASGeneralPoll::GetPollValue()
		/// Test for a correct return value.
		/// </summary>
		TEST_METHOD(GetPollValueTest)
		{
			typedef BYTE TestDataType;
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData((TestDataType)0), &m_testIndex);

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); m_testIndex++)
			{
				// Get the test data.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);

				// Setup.
				CSASGeneralPoll obj(testValue);

				// Perform operation(s) and test(s).
				AssertResult2(obj.GetPollValue(), m_pollValueExpected);
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		TEST_METHOD(SwapPollValueTest)
		{
			typedef BYTE TestDataType;
			TestDataType testValue(1);
			CSASGeneralPoll obj(testValue);

			bool expected = false;
			bool actual = obj.SwapPollValue();

			Assert::IsTrue(expected == actual, _T("Swap Poll Value returned incorrect value"));
		}
	};
}
#endif //TODO-PORT