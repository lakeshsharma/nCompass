#include "stdafx.h"
#include "UI/Log.h"

// Sentinel includes.
#include "Utilities.h"

// Unit testing includes.
#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	class LogUI : public ::testing::Test
	{
	};

		/// <summary>
		/// Code coverage and functional test for:
		///     CLog::CLog()
		/// Code coverage:
		///     CLog::GetDateTimeAsString()
		///     CLog::GetDateTime()
		///     CLog::~CLog()
		/// Test for correct default values.
		/// </summary>
		TEST(LogUI, CLogTest)
		{
			// Expected results.
			const string dateTimeAsStringExpected(_T("01/01/1970 00:00AM")); // Expected for UTC/GMT timezone.
			const time_t dateTimeExpected(0);

			// Setup.
			const int testIndex(0);

			// Perform operation(s) to be tested.
			const CLog log;

			// Test #1.
			{
				const time_t dateTimeResult(log.GetDateTime());
				const bool success(dateTimeResult == dateTimeExpected);
				if (!success)
				{
					string failedMsg;
					failedMsg = FormatString(_T("dateTimeResult=%lld expected=%lld testIndex=%d in %s(%d)"),
						(long long)dateTimeResult,
						(long long)dateTimeExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
					ASSERT_TRUE(success) << failedMsg;
				}
			}

			// Test #2.
			{
				// Don't try to calculate the expected result using the local time zone.
				// Just verify that the result string is the correct length.
				const string dateTimeAsStringResult(log.GetDateTimeAsString());
				const bool success(dateTimeAsStringResult.length() == dateTimeAsStringExpected.length());
				if (!success)
				{
					string failedMsg;
					failedMsg = FormatString(_T("dateTimeAsStringResult=\"%s\" expected=\"%s\" testIndex=%d in %s(%d)"),
						static_cast<LPCTSTR>(dateTimeAsStringResult.c_str()),
						static_cast<LPCTSTR>(dateTimeAsStringExpected.c_str()),
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
					ASSERT_TRUE(success) << failedMsg;
				}
			}

			// Cleanup.
		}
}
