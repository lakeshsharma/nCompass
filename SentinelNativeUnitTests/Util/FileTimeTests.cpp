#if 000//TODO-PORT
#include "stdafx.h"
#include "Util/FileTime.h"
#include "TestUtil/TestUtil.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace SentinelNativeUnitTests
{
//	milliseconds = 1436226873744

	static SYSTEMTIME utcSystemTime = { 2015, 7, 1, 6, 23, 54, 33, 744 };
	static SYSTEMTIME localSystemTime = { 2015, 7, 1, 6, 16, 54, 33, 744 };

	static SYSTEMTIME systemTime2 = { 2017, 1, 0, 1, 0, 0, 1, 0 };

	// These following were obtained by converting the above through an online tool and then adding the milliseconds
	// http://www.silisoftware.com/tools/date.php?inputdate=130806752737440000&inputformat=filetime
	
	static uint64_t utcInterval = 130807004737440000;
	static uint64_t localInterval = 130806752737440000;
	static uint64_t interval2 = 131277024010000000;

	// These tests were converted from another test framework
	TEST_CLASS(Util_FileTimeTests)
	{
		TEST_METHOD(Util_FileTime_ToUint64_HMSM)
		{
			struct
			{
				int line;
				WORD hour;
				WORD minute;
				WORD second;
				WORD millisecond;
				uint64_t expected;
			}
			static data[] =
			{
				{ __LINE__, 1, 0, 0, 0, 36000000000 },
				{ __LINE__, 0, 1, 0, 0,   600000000 },
				{ __LINE__, 0, 0, 1, 0,    10000000 },
				{ __LINE__, 0, 0, 0, 1,       10000 }
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				uint64_t val = FileTime::ToUint64(data[i].hour, data[i].minute, data[i].second, data[i].millisecond);
				Assert::AreEqual(data[i].expected, val, AssertMsg::Format(data[i].line, nullptr));
			}
		}

		void VerifyTime(const SYSTEMTIME& expectedTime, const SYSTEMTIME& actualTime, int line = 0)
		{
			// I didn't force this formatting, it was due to how I cut and pasted items using column select (Alt-RightMouse button)
			Assert::AreEqual((int) expectedTime.wYear			, (int) actualTime.wYear		, AssertMsg::Format(line, L"wYear"			));
			Assert::AreEqual((int) expectedTime.wMonth			, (int) actualTime.wMonth		, AssertMsg::Format(line, L"wMonth"			));
			Assert::AreEqual((int) expectedTime.wDay			, (int) actualTime.wDay			, AssertMsg::Format(line, L"wDay"			));
			Assert::AreEqual((int) expectedTime.wHour			, (int) actualTime.wHour		, AssertMsg::Format(line, L"wHour"			));
			Assert::AreEqual((int) expectedTime.wMinute			, (int) actualTime.wMinute		, AssertMsg::Format(line, L"wMinute"		));
			Assert::AreEqual((int) expectedTime.wSecond			, (int) actualTime.wSecond		, AssertMsg::Format(line, L"wSecond"		));
			Assert::AreEqual((int) expectedTime.wMilliseconds	, (int) actualTime.wMilliseconds, AssertMsg::Format(line, L"wMilliseconds"	));	
		}

		TEST_METHOD(Util_FileTime_ToSystemTime_Local)
		{
			SYSTEMTIME systemTime = FileTime::ToSystemTime(localInterval);
			VerifyTime(localSystemTime, systemTime, __LINE__);
		}

		TEST_METHOD(Util_FileTime_ToSystemTime_Utc)
		{
			SYSTEMTIME systemTime = FileTime::ToSystemTime(utcInterval);
			VerifyTime(utcSystemTime, systemTime, __LINE__);
		}

		TEST_METHOD(Util_FileTime_ToSystemTime2)
		{
			SYSTEMTIME systemTime = FileTime::ToSystemTime(interval2);
			VerifyTime(systemTime2, systemTime, __LINE__);
		}

		TEST_METHOD(Util_FileTime_ToUint64_SYSTEMTIME_Local)
		{
			uint64_t val = FileTime::ToUint64(localSystemTime);
			Assert::AreEqual(localInterval, val);
		}

		TEST_METHOD(Util_FileTime_ToUint64_SYSTEMTIME_Utc)
		{
			uint64_t val = FileTime::ToUint64(utcSystemTime);
			Assert::AreEqual(utcInterval, val);
		}

		TEST_METHOD(Util_FileTime_ToUint64_SYSTEMTIME_2)
		{
			uint64_t val = FileTime::ToUint64(systemTime2);
			Assert::AreEqual(interval2, val);
		}

		TEST_METHOD(Util_FileTime_GetCurTime_local)
		{
			SYSTEMTIME localTime;
			GetLocalTime(&localTime);
			uint64_t timeVal = FileTime::ToUint64(localTime);

			uint64_t val = FileTime::GetCurTime(true);
			Assert::AreEqual(timeVal, val);
		}

		TEST_METHOD(Util_FileTime_GetCurTime_utc)
		{
			FILETIME fileTime;
			GetSystemTimeAsFileTime(&fileTime);
			uint64_t timeVal = FileTime::ToUint64(fileTime);

			uint64_t val = FileTime::GetCurTime(false);
			Assert::AreEqual(timeVal, val);
		}

		TEST_METHOD(Util_FileTime_ToUtil_FileTime_Err)
		{
			SYSTEMTIME badtime;
			memset(&badtime, 0xff, sizeof(badtime));
			FILETIME filetime = FileTime::ToFileTime(badtime);
			Assert::IsTrue(TestUtil::IsZeroes(filetime));
		}

		TEST_METHOD(Util_FileTime_ToSystemTime_Err)
		{
			FILETIME badtime;
			badtime.dwLowDateTime = 0xffffffff;
			badtime.dwHighDateTime = 0xffffffff;
			SYSTEMTIME systemTime = FileTime::ToSystemTime(badtime);
			Assert::IsTrue(TestUtil::IsZeroes(systemTime));
		}

		TEST_METHOD(Util_FileTime_ToUtil_FileTime_interval)
		{
			FILETIME fileTime = FileTime::ToFileTime(localInterval);
			uint64_t val = FileTime::ToUint64(fileTime);

			Assert::AreEqual(localInterval, val);
		}
		
		TEST_METHOD(Util_FileTime_ToUtil_FileTime_SYSTEMTIME)
		{
			FILETIME fileTime = FileTime::ToFileTime(localSystemTime);
			uint64_t val = FileTime::ToUint64(fileTime);

			Assert::AreEqual(localInterval, val);
		}

		TEST_METHOD(Util_FileTime_Diff)
		{
			SYSTEMTIME midnight(localSystemTime);

			midnight.wHour = 0;
			midnight.wMinute = 0;
			midnight.wSecond = 0;
			midnight.wMilliseconds = 0;

			int64_t val = FileTime::Diff(localSystemTime, midnight);
			uint64_t val2 = FileTime::ToUint64(localSystemTime.wHour, localSystemTime.wMinute, localSystemTime.wSecond, localSystemTime.wMilliseconds);

			Assert::AreEqual((uint64_t) val2, (uint64_t) val);
		}

		TEST_METHOD(Util_FileTime_Diff_neg)
		{
			SYSTEMTIME midnight(localSystemTime);

			midnight.wHour = 0;
			midnight.wMinute = 0;
			midnight.wSecond = 0;
			midnight.wMilliseconds = 0;

			int64_t val = FileTime::Diff(midnight, localSystemTime);
			int64_t val2 = (int64_t) FileTime::ToUint64(localSystemTime.wHour, localSystemTime.wMinute, localSystemTime.wSecond, localSystemTime.wMilliseconds);

			Assert::IsTrue(val < 0, L"val < 0");
			Assert::AreEqual((uint64_t) val2, (uint64_t) -val);
		}

		TEST_METHOD(Util_FileTime_Add)
		{
			const SYSTEMTIME time = { 2015, 7, 5, 10, 12, 0, 0, 0 };
			const SYSTEMTIME expectedTime = { 2015, 7, 5, 10, 12, 0, 0, 567 };

			SYSTEMTIME actualTime = FileTime::Add(time, 567);

			VerifyTime(expectedTime, actualTime);
		}

		TEST_METHOD(Util_FileTime_Add2)
		{
			const SYSTEMTIME time = { 2015, 7, 5, 10, 12, 0, 0, 0 };
			const SYSTEMTIME expectedTime = { 2015, 7, 5, 10, 13, 2, 3, 456 };

			SYSTEMTIME actualTime = FileTime::Add(time, (1 * 3600000) +	(2 * 60000) + (3 * 1000) + 456);

			VerifyTime(expectedTime, actualTime);
		}
	};
}
#endif //TODO-PORT