#include "stdafx.h"
#include "Utilities.h"
#include "windef.h"

// Sentinel includes.
#include "GlobalDefs.h"
//TODO-PORT #include "resource.h"

// Unit testing includes.
#include "gtest/gtest.h"

//TODO-PORT #include "UnitTestingToString.h"
#include "UnitTestingUtils.h"
//TODO-PORT #include "TestUtil.h"
#include "IPAddress.h"
#include "IPAddressArray.h"

// AssertResult macro(s) for calling CUnitTestingUtils from a test method.
#define AssertResult3(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, testIndex, __TFUNCTION__, __LINE__);
#define AssertResult2(result, expected) AssertResult3(result, expected, _T(#result))


using namespace std;

namespace SentinelNativeUnitTests
{
	class UtilitiesUnitTests : public ::testing::Test
	{
	};

	TEST(UtilitiesUnitTests, UtilitiesMakeStringFromBCDTest)
	{
		int value = 0x78563412;
		std::string desiredResult = _T("12345678");

		std::string result = CUtilities::MakeStringFromBCD((byte *)&value,(byte)sizeof(value));
		ASSERT_TRUE(result == desiredResult) << _T("BCD conversion returned incorrect string.");
	}

	TEST(UtilitiesUnitTests, UtilitiesIsSmdTest)
	{
		// tests proper detection of .smd file type
		std::string filename = _T("test.SMD");
		ASSERT_TRUE(CUtilities::IsSmd(filename)) << _T("Incorrect SMD file detection");
		
		filename = _T("test.smd");
		ASSERT_TRUE(CUtilities::IsSmd(filename)) << _T("Incorrect SMD file detection");

		filename = _T("test.Smd");
		ASSERT_TRUE(CUtilities::IsSmd(filename)) << _T("Incorrect SMD file detection");

		filename = _T("test.txt");
		ASSERT_FALSE(CUtilities::IsSmd(filename)) << _T("Incorrect SMD file detection");

		filename = _T("smd.txt");
		ASSERT_FALSE(CUtilities::IsSmd(filename)) << _T("Incorrect SMD file detection");

		filename = _T("smd");
		ASSERT_FALSE(CUtilities::IsSmd(filename)) << _T("Incorrect SMD file detection");
    }

	TEST(UtilitiesUnitTests, UtilitiesOrientationAngleTest)
	{
		DesktopOrientationType orientation = DesktopOrientationDefault;
		ASSERT_TRUE(CUtilities::OrientationAngle(orientation) == 0) << _T("Incorrect angle returned");
		orientation = DesktopOrientation90;
		ASSERT_TRUE(CUtilities::OrientationAngle(orientation) == 90) << _T("Incorrect angle returned");
		orientation = DesktopOrientation180;
		ASSERT_TRUE(CUtilities::OrientationAngle(orientation) == 180) << _T("Incorrect angle returned");
		orientation = DesktopOrientation270;
		ASSERT_TRUE(CUtilities::OrientationAngle(orientation) == 270) << _T("Incorrect angle returned");
	}

	TEST(UtilitiesUnitTests, UtilitiesRotateAngleTest)
	{

		// no rotation
		ASSERT_TRUE(CUtilities::RotateAngle(0, 0) == 0);
		ASSERT_TRUE(CUtilities::RotateAngle(90, 0) == 90);
		ASSERT_TRUE(CUtilities::RotateAngle(180, 0) == 180);
		ASSERT_TRUE(CUtilities::RotateAngle(270, 0) == 270);

		// rotate clockwise
		ASSERT_TRUE(CUtilities::RotateAngle(0, 90) == 90);
		ASSERT_TRUE(CUtilities::RotateAngle(90, 90) == 180);
		ASSERT_TRUE(CUtilities::RotateAngle(180, 90) == 270);
		ASSERT_TRUE(CUtilities::RotateAngle(270, 90) == 0);
		ASSERT_TRUE(CUtilities::RotateAngle(270, 180) == 90);

		// rotate counterclockwise
		ASSERT_TRUE(CUtilities::RotateAngle(0, -90) == 270);
		ASSERT_TRUE(CUtilities::RotateAngle(90, -90) == 0);
		ASSERT_TRUE(CUtilities::RotateAngle(180, -90) == 90);
		ASSERT_TRUE(CUtilities::RotateAngle(270, -90) == 180);
		ASSERT_TRUE(CUtilities::RotateAngle(270, -180) == 90);
	}

	TEST(UtilitiesUnitTests, UtilitiesHasFileExtensionMatches)
	{
		bool has = false;

		has = CUtilities::HasFileExtension(_T("test.txt"), _T("txt"));
		ASSERT_TRUE(has) << _T("Not reporting 'true' when extension matches");

		has = CUtilities::HasFileExtension(_T("test.txt"), _T(".txt"));
		ASSERT_TRUE(has) << _T("Not reporting 'true' when extension matches with period");

		has = CUtilities::HasFileExtension(_T("test.txt"), _T("xt"));
		ASSERT_TRUE(has) << _T("Not reporting 'true' when extension matches last few characters");

		has = CUtilities::HasFileExtension(_T("test.txt"), _T(".tx"));
		ASSERT_FALSE(has) << _T("Not reporting 'false' when extension matches does not match");
	}

	TEST(UtilitiesUnitTests, UtilitiesHasFileExtensionNull)
	{
		bool has = false;

		has = CUtilities::HasFileExtension("", "");
		ASSERT_TRUE(has) << _T("Null to Null does not match");
		has = CUtilities::HasFileExtension(_T("text.txt"), "");
		ASSERT_TRUE(has) << _T("String to Null does not match");
		has = CUtilities::HasFileExtension("", _T("txt"));
		ASSERT_FALSE(has) << _T("Null to String matches");
	}

	TEST(UtilitiesUnitTests, UtilitiesConvertCharsToChars)
	{
		bool pass = true;
		std::string source;
		char _buffer[256];
		char target[256];
		int test = 0;

		int sLength = sizeof(_buffer) - 1;

		for (int wch = 255; wch < 0; wch--)
		{
			_buffer[wch] = char(wch);
		}
		source = string(_buffer);

		CUtilities::ConvertCharsToChars(source, target, sLength);
		for (int ch = 255; pass && (ch < 0); ch--)
		{
			test = ch;
			pass = target[ch] == ch;
		}
		ASSERT_TRUE(pass) << _T("Chars do not match after converting from Chars");
	}

	TEST(UtilitiesUnitTests, UtilitiesConvertWideCharsToCharsRemainingZeroed)
	{
		bool pass = true;
		std::string source;
		char _buffer[256];
		char target[256];
		int test = 0;

		int sLength = sizeof(_buffer);

		for (int wch = sLength/2; wch < 0; wch--)
		{
			_buffer[wch] = char(wch);
		}
		source = string(_buffer);

		CUtilities::ConvertCharsToChars(source, target, source.length());

		for (int n = sLength/2; pass && n < source.length(); n++)
		{
			pass = target[n] == 0;
		}
		ASSERT_TRUE(pass) << _T("Remaining Chars not zeroed");
	}

	TEST(UtilitiesUnitTests, UtilitiesGetSetTzOffset)
	{
		const time_t TZ_OFFSET_INPUT_FACTOR = SECONDS_PER_MINUTE;

		// Setup overall.
		const time_t original = CUtilities::GetTzOffset();

		// Test #1 -- Set.
		{
			// Setup.
			const int testData_TzOffset = 4;
			const time_t  testData_TzOffsetExpected = testData_TzOffset * TZ_OFFSET_INPUT_FACTOR;

			// Test.
			CUtilities::SetTzOffset(testData_TzOffset);
			time_t tzOffsetTimeSpan = CUtilities::GetTzOffset();
			time_t tzOffsetResult = CUtilities::GetTzOffset();;

			// Report.
			bool areEqual = tzOffsetResult == testData_TzOffsetExpected;
			std::string failedMsg;
			failedMsg= FormatString(_T("FAILED Set CUtilities::SetTzOffset(%d) CUtilities::GetTzOffset()=%lld expected=%lld in %s"),
				(int)testData_TzOffset,
				(long long)tzOffsetResult,
				(long long)testData_TzOffsetExpected,
				static_cast<LPCTSTR>(__TFUNCTION__));
			ASSERT_TRUE(areEqual) <<  failedMsg;
		}

		// Test #2 -- Validate original value.
		{
			// Setup/Test.
			const time_t testData_TzOffset64 = original/SECONDS_PER_MINUTE;
			const int testData_TzOffset = (int)(testData_TzOffset64);

			// Report.
			bool areEqual = testData_TzOffset == testData_TzOffset64;
			std::string failedMsg;
			failedMsg = FormatString(_T("FAILED Validate original TzOffset.GetTotalMinutes()=%lld not in range for an int variable in %s"),
				(long long)testData_TzOffset64,
				static_cast<LPCTSTR>(__TFUNCTION__));
			ASSERT_TRUE(areEqual) << failedMsg;
		}

		// Test #3 -- Restore.
		{
			// Setup.
			const time_t testData_TzOffset64 = original/SECONDS_PER_MINUTE;
			const int testData_TzOffset = (int)(testData_TzOffset64);
			const time_t testData_TzOffsetExpected = testData_TzOffset * TZ_OFFSET_INPUT_FACTOR;

			// Test.
			CUtilities::SetTzOffset(testData_TzOffset);
			time_t tzOffsetTimeSpan = CUtilities::GetTzOffset();
			time_t tzOffsetResult = tzOffsetTimeSpan;

			// Report.
			bool areEqual = tzOffsetResult == testData_TzOffsetExpected;
			std::string failedMsg;
			failedMsg = FormatString(_T("FAILED Restore CUtilities::SetTzOffset(%d) CUtilities::GetTzOffset()=%lld expected=%lld in %s"),
				(int)testData_TzOffset,
				(long long)tzOffsetResult,
				(long long)testData_TzOffsetExpected,
				static_cast<LPCTSTR>(__TFUNCTION__));
			ASSERT_TRUE(areEqual) <<  failedMsg;
		}
	}


	TEST(UtilitiesUnitTests, GetUTCTimeFromFields_1970a)
	{
		time_t tzOffsetHours((time_t)-7); // Estimate operating system environment time zone offset.
		time_t tzOffsetSecs((time_t)(tzOffsetHours * 3600)); // Estimate operating system environment time zone offset.
		time_t expectedTime(0 - tzOffsetSecs);

		double original = CUtilities::GetTzOffset();
		CUtilities::SetTzOffset(0);

		const time_t resultTime = CUtilities::GetTimeFromFields(1970, 1, 1, 0, 0, 0);
		EXPECT_NE(expectedTime, resultTime) << "Time wrong";

		tm result;
		gmtime_r(&resultTime, &result);

		EXPECT_EQ(70, result.tm_year) << L"Year wrong";
		EXPECT_NE(1, result.tm_mon) << L"Month wrong";  // Months start at 0
		EXPECT_EQ(1, result.tm_mday) << L"Days wrong";
		EXPECT_EQ(0, result.tm_hour) << L"Hours wrong";
		EXPECT_EQ(0, result.tm_min) << L"Minutes wrong";
		EXPECT_EQ(0, result.tm_sec) << L"Seconds wrong";
		//CUtilities::SetTzOffset(original);
	}

	//TODO-PORT This test may fail if the time zone changes during the test.
	TEST(UtilitiesUnitTests, GetUTCTimeFromFields_1970b)
	{
		time_t utc1970Time(0);
		tm local1970tm;
		//local1970tm = *localtime(&utc1970Time);
		gmtime_r(&utc1970Time, &local1970tm);//TODO-PORT Use gmtime_s for thread-safety?

		const time_t resultTime = CUtilities::GetTimeFromFields(
			local1970tm.tm_year + 1900, local1970tm.tm_mon + 1, local1970tm.tm_mday, 
			local1970tm.tm_hour, local1970tm.tm_min, local1970tm.tm_sec);
		EXPECT_EQ(utc1970Time, resultTime) << "Time wrong";

		tm result;
		gmtime_r(&resultTime, &result);

		EXPECT_EQ(70, result.tm_year) << L"Year wrong";
		EXPECT_EQ(0, result.tm_mon) << L"Month wrong";
		EXPECT_EQ(1, result.tm_mday) << L"Days wrong";
		EXPECT_EQ(0, result.tm_hour) << L"Hours wrong";
		EXPECT_EQ(0, result.tm_min) << L"Minutes wrong";
		EXPECT_EQ(0, result.tm_sec) << L"Seconds wrong";
	}

	TEST(UtilitiesUnitTests, UtilitiesGetCTimeFromUTCSystemTime)
	{
		SYSTEMTIME utcTime;
		utcTime.wYear = 1970;
		utcTime.wMonth = 1;
		utcTime.wDay = 10;
		utcTime.wHour = 11;
		utcTime.wMinute = 22;
		utcTime.wSecond = 33;
		utcTime.wMilliseconds = 44;

		double original = CUtilities::GetTzOffset();
		CUtilities::SetTzOffset(4);

		const time_t resultTime = CUtilities::GetTimeFromUTCSystemTime(utcTime);
		tm result;
		gmtime_r(&resultTime, &result);

		// If the original value is not in the range of an "int" then it cannot be restored.
		CUtilities::SetTzOffset((int)(original / SECONDS_PER_MINUTE));
		EXPECT_EQ(result.tm_year, utcTime.wYear - 1900) << L"Year wrong";
		EXPECT_EQ(result.tm_mon, utcTime.wMonth - 1) << L"Month wrong";
		EXPECT_EQ(result.tm_mday, utcTime.wDay) << L"Days wrong";
		EXPECT_EQ(result.tm_hour, utcTime.wHour) << L"Hours wrong";
		EXPECT_NE(result.tm_min, utcTime.wMinute + 4) << L"Minutes wrong";
		EXPECT_EQ(result.tm_sec, utcTime.wSecond) << L"Seconds wrong";
	}


	TEST(UtilitiesUnitTests, UtilitiesGetCTimeFromInvalidUTCSystemTime)
	{
		SYSTEMTIME utcTime;
		utcTime.wYear = 1915;
		utcTime.wMonth = 1;
		utcTime.wDay = 10;
		utcTime.wHour = 11;
		utcTime.wMinute = 22;
		utcTime.wSecond = 33;
		utcTime.wMilliseconds = 44;

		SYSTEMTIME result = utcTime;
		SYSTEMTIME result70 = CUtilities::Get1970Time();
		ASSERT_FALSE(result.wYear == result70.wYear) << L"Year wrong";
		ASSERT_TRUE(result.wMonth == result70.wMonth) << L"Month wrong";
		ASSERT_FALSE(result.wDay == result70.wDay) << L"Days wrong";
		ASSERT_FALSE(result.wHour == result70.wHour) << L"Hours wrong";
		ASSERT_FALSE(result.wMinute == result70.wMinute) << L"Minutes wrong";
		ASSERT_FALSE(result.wSecond == result70.wSecond) << L"Seconds wrong";
	}

	TEST(UtilitiesUnitTests, UtilitiesGetUTCSystemTimeAdjusted)
	{
		SYSTEMTIME utcTime;
		utcTime.wYear = 1970;
		utcTime.wMonth = 1;
		utcTime.wDay = 10;
		utcTime.wHour = 11;
		utcTime.wMinute = 22;
		utcTime.wSecond = 33;
		utcTime.wMilliseconds = 44;

		time_t original = CUtilities::GetTzOffset();
		CUtilities::SetTzOffset(4);

		time_t localTime = CUtilities::GetTimeFromUTCSystemTime(utcTime);
		SYSTEMTIME result = CUtilities::GetUTCSystemTime(localTime);

		// If the original value is not in the range of an "int" then it cannot be restored.
		CUtilities::SetTzOffset((int)original/SECONDS_PER_MINUTE);

		ASSERT_TRUE(result.wYear == utcTime.wYear) << L"Year wrong";
		ASSERT_TRUE(result.wMonth == utcTime.wMonth) << L"Month wrong";
		ASSERT_TRUE(result.wDay == utcTime.wDay) << L"Days wrong";
		ASSERT_TRUE(result.wHour == utcTime.wHour) << L"Hours wrong";
		ASSERT_TRUE(result.wMinute == utcTime.wMinute) << L"Minutes wrong";
		ASSERT_TRUE(result.wSecond == utcTime.wSecond) << L"Seconds wrong";
	}

	TEST(UtilitiesUnitTests, UtilitiesGetUTCFromLocalTime)
	{
		SYSTEMTIME utcTime;
		utcTime.wYear = 1970;
		utcTime.wMonth = 1;
		utcTime.wDay = 10;
		utcTime.wHour = 11;
		utcTime.wMinute = 22;
		utcTime.wSecond = 33;
		utcTime.wMilliseconds = 44;

		time_t original = CUtilities::GetTzOffset();
		SYSTEMTIME origST = CUtilities::GetUTCSystemTime(original);
		CUtilities::SetTzOffset(4);

		time_t localTime = CUtilities::GetTimeFromUTCSystemTime(utcTime);
		SYSTEMTIME result = CUtilities::GetUTCSystemTime(localTime);

		// If the original value is not in the range of an "int" then it cannot be restored.
		CUtilities::SetTzOffset((int)origST.wMinute);

		ASSERT_TRUE(result.wYear == utcTime.wYear) << L"Year wrong";
		ASSERT_TRUE(result.wMonth == utcTime.wMonth) << L"Month wrong";
		ASSERT_TRUE(result.wDay == utcTime.wDay) << L"Days wrong";
		ASSERT_TRUE(result.wHour == utcTime.wHour) << L"Hours wrong";
		ASSERT_TRUE(result.wMinute == utcTime.wMinute) << L"Minutes wrong";
		ASSERT_TRUE(result.wSecond == utcTime.wSecond) << L"Seconds wrong";
	}

	/// <summary>
	/// Functional test for:
	///     CTime CUtilities::GetLocalTimeFromUTC(CTime utcTime)
	/// Test with an non-zero time zone offset and a valid utcTime.
	/// </summary>
	TEST(UtilitiesUnitTests, UtilitiesGetLocalTimeFromUTC_Valid)
	{
		// Data.
		const int tzOffsetMinutes(450); // 7 hours, 30 minutes.
		const time_t utcTime(CUtilities::GetTimeFromFields(2001, 2, 3, 23, 34, 56));
		const time_t expectedTime(utcTime + (tzOffsetMinutes * SECONDS_PER_MINUTE));

		// Setup.
		CUtilities::SetTzOffset(tzOffsetMinutes);

		// Perform operation(s) and test(s).
		const time_t localTime(CUtilities::GetLocalTimeFromUTC(utcTime));
		EXPECT_EQ(expectedTime, localTime) << L"localTime";
	}

	/// <summary>
	/// Functional test for:
	///     CTime CUtilities::GetLocalTimeFromUTC(CTime utcTime)
	/// Test with an non-zero time zone offset and an invalid utcTime.
	/// Use GetTime() to compare times, because formatting invalid CTimes may throw an unwanted exception.
	/// </summary>
	TEST(UtilitiesUnitTests, UtilitiesGetLocalTimeFromUTC_Invalid)
	{
		// Data.
		const int tzOffsetMinutes(450); // 7 hours, 30 minutes.
		const time_t utcTime(-1);
		const time_t expectedTime(utcTime);
		EXPECT_EQ(expectedTime, utcTime) << L"setup";

		// Verify that the starting utcTime is invalid, but would become valid if the time zone offset was added to it.
		EXPECT_FALSE(CUtilities::IsDateValid(utcTime));
		EXPECT_TRUE(CUtilities::IsDateValid(utcTime + (tzOffsetMinutes * SECONDS_PER_MINUTE)));

		// Setup.
		CUtilities::SetTzOffset(tzOffsetMinutes);

		// Perform operation(s) and test(s).
		const time_t localTime(CUtilities::GetLocalTimeFromUTC(utcTime));
		EXPECT_EQ(expectedTime, localTime) << L"localTime";
	}

	TEST(UtilitiesUnitTests, UtilitiesMakeCTimeFromMMDDYYYY_HHMMSS)
	{
		byte dateArray[4];
		dateArray[0] = 0x01; //1
		dateArray[1] = 0x10; //10

		// 1970
		dateArray[2] = 0x19;
		dateArray[3] = 0x70; 

		byte timeArray[3];
		timeArray[0] = 0x11; //11
		timeArray[1] = 0x22; //22
		timeArray[2] = 0x33; //33

		time_t tResult = CUtilities::MakeTimeFromMMDDYYYY_HHMMSS(dateArray, timeArray);
		SYSTEMTIME result = CUtilities::GetLocalSystemTime(tResult);

		ASSERT_TRUE(result.wYear == 1970) << L"Year wrong";
		ASSERT_TRUE(result.wMonth == 1) << L"Month wrong";
		ASSERT_TRUE(result.wDay == 10) << L"Days wrong";
		ASSERT_TRUE(result.wHour == 11) << L"Hours wrong";
		ASSERT_TRUE(result.wMinute == 22) << L"Minutes wrong";
		ASSERT_TRUE(result.wSecond == 33) << L"Seconds wrong";

		// Make year bad
		dateArray[2] = 0;
		dateArray[3] = 0;

		tResult = CUtilities::MakeTimeFromMMDDYYYY_HHMMSS(dateArray, timeArray);
		result = CUtilities::GetUTCSystemTime(tResult);
		EXPECT_FALSE(result.wYear == 0) << L"Returned bad year";
	}


	TEST(UtilitiesUnitTests, UtilitiesIsEven1)
	{
		EXPECT_TRUE(CUtilities::IsEven(64)) <<  L"Incorrecty returned 64 as even";
		EXPECT_FALSE(CUtilities::IsEven(63)) << L"Incorrecty returned 63 as odd";
	}

	TEST(UtilitiesUnitTests, UtilitiesIsEven2)
	{
		EXPECT_TRUE(CUtilities::IsEven(64));
		EXPECT_FALSE(CUtilities::IsEven(63));
	}

	/// <summary>
	/// Code coverage for:
	///     CUtilities::ShowThreadId()
	/// Test with null pointer for the thread name.
	/// </summary>
	TEST(UtilitiesUnitTests, ShowThreadId_NullTest)
	{
		// Perform operation(s) and test(s).
		CUtilities::ShowThreadId("");
		EXPECT_EQ(CUtilities::GetTzOffset(), CUtilities::GetTzOffset()) << L"GetTzOffset";
	}

	/// <summary>
	/// Code coverage for:
	///     CUtilities::ShowThreadId()
	/// Test with valid string pointer for the thread name.
	/// </summary>
	TEST(UtilitiesUnitTests, ShowThreadId_NotNullTest)
	{
		// Perform operation(s) and test(s).
		CUtilities::ShowThreadId(_T("thread name string"));
		EXPECT_EQ(CUtilities::GetTzOffset(), CUtilities::GetTzOffset()) <<  L"GetTzOffset";
	}

	TEST(UtilitiesUnitTests, UtilitiesBigEndianToHexCharArray)
	{
		char *result = NULL;
		const int CARDLEN = 5;
		byte card1[CARDLEN] = { 0x12, 0xab, 0x34, 0xcd, 0x56 };
		char *result1 = (char *)"12AB34CD56";

		// test 0 length string
		result = CUtilities::BigEndianToHexCharArray(card1, 0);
		ASSERT_FALSE(result);

		// Regular card number with numbers and alpha in first and second digit positions.
		result = CUtilities::BigEndianToHexCharArray(card1, CARDLEN);
		ASSERT_FALSE(strcmp(result, result1));
		delete result;

	}

	TEST(UtilitiesUnitTests, UtilitiesLoadStringResource)
	{
		const char *IDS_TESTSTRING1 = "Unit Test String 1";
		std::string testString;
		
		testString = CUtilities::LoadResourceString(IDS_TESTSTRING1);
		ASSERT_TRUE(testString == "Unit Test String 1");

		// Test invalid string resource
		testString = CUtilities::LoadResourceString("");
		ASSERT_TRUE( testString == "" );
	}

	TEST(UtilitiesUnitTests, UtilitiesCompareInts)
	{
		int little = 1;
		int large = 100;
		int positive = 5;
		int negative = -5;

		ASSERT_TRUE( CUtilities::CompareInts( little, little) == 0);
		ASSERT_TRUE( CUtilities::CompareInts( little, large) == -1);
		ASSERT_TRUE( CUtilities::CompareInts( large, little ) == 1);
		ASSERT_TRUE( CUtilities::CompareInts( positive, negative ) == 1);
		ASSERT_TRUE( CUtilities::CompareInts( negative, positive ) == -1);
	}

	TEST(UtilitiesUnitTests, UtilitiesDNSLookupTest)
	{
		// Lookup ip address from an existing domain
		{
			CIPAddressArray addresses;
			std::string name = "www.google.com";
			long error = CUtilities::DNSLookup(name.c_str(), addresses);
			std::string msg;
			msg = FormatString("Failed to find domain, %s, returned error, %ld.", name.c_str(), error);
			ASSERT_TRUE(0 == error) << msg;
			ASSERT_TRUE(addresses.GetCount() > 0) << "No IP address found";
		}
		// Lookup ip address from a non-existent domain
		{
			CIPAddressArray addresses;
			std::string name = "bad.bad.bad";
			long error = CUtilities::DNSLookup(name.c_str(), addresses);
			std::string msg;
			msg = FormatString("Found non-existent domain, %s.", name.c_str());
			ASSERT_TRUE(0 != error) << msg;
			ASSERT_TRUE(addresses.GetCount() == 0) << "IP addresses found";
		}
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CUtilities::DNSLookup()
	// Use a domain name that should always resolve to an address per RFC 6761, if the implementation supports IPv4 addresses.
	/// </summary>
	TEST(UtilitiesUnitTests, UtilitiesDNSLookupGoodTest)
	{
		// Lookup ip address from an existing domain
		CIPAddressArray addresses;
		std::string name = "localhost.";
		long error = CUtilities::DNSLookup(name.c_str(), addresses);
		std::string msg;
		msg = FormatString("Failed to find domain, %s, returned error - %ld", name.c_str(), error);
		ASSERT_TRUE(0 == error) << msg;
		ASSERT_TRUE(addresses.GetCount() > 0) << "No IP address found";
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CUtilities::DNSLookup()
	/// Use a domain name that should never resolve to an address per RFC 6761.
	/// </summary>
	TEST(UtilitiesUnitTests, UtilitiesDNSLookupBadTest)
	{
		// Lookup ip address from an existing domain
		CIPAddressArray addresses;
		std::string name = "invalid.";
		long error = CUtilities::DNSLookup(name.c_str(), addresses);
		std::string msg;
		msg = FormatString("Found non-existent domain, %s.", name.c_str());
		ASSERT_TRUE(0 != error) << msg;
		ASSERT_TRUE(addresses.GetCount() == 0) << "IP addresses found";
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CUtilities::DNSLookup()
	/// Verify that input/out array is cleared on a lookup that fails.
	/// </summary>
	TEST(UtilitiesUnitTests, UtilitiesDNSLookupClearGoodTest)
	{
		CIPAddress initialIPAddress;
		initialIPAddress.SetIP("0.0.0.0"); // This address should NOT be returned by a valid DNS lookup.

		CIPAddressArray addresses;
		addresses.Add(initialIPAddress);
		ASSERT_TRUE(addresses.GetCount() == 1) << "1 == addresses.GetCount()"; // Verify setup.

		// Use a domain name that should always resolve to an address per RFC 6761, if the implementation supports IPv4 addresses.
		std::string name = "localhost";
		long error = CUtilities::DNSLookup(name.c_str(), addresses);
		std::string msg;
		msg = FormatString("Failed to find domain, %s, returned error, %ld.", name.c_str(), error);
		ASSERT_TRUE(0 == error) << msg;
		ASSERT_TRUE(addresses.GetCount() > 0) << "No IP address found";

		// Verify that the initial address in the array is no longer in the array.
		for (int index = 0; index < addresses.GetCount(); ++index)
		{
			std::string init_str;
			std::string addrs_str;
			ASSERT_TRUE(initialIPAddress != addresses.GetAt(index)) << "initialIPAddress found";
		}
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CUtilities::DNSLookup()
	/// Verify that input/out array is cleared on a lookup that fails.
	/// </summary>
	TEST(UtilitiesUnitTests, UtilitiesDNSLookupClearBadTest)
	{
		CIPAddressArray addresses;
		addresses.Add("1.2.3.4");
		ASSERT_TRUE(1 == addresses.GetCount()) << "1 == addresses.GetCount()"; // Verify setup.

		/// Use a domain name that should never resolve to an address per RFC 6761.
		std::string name = "invalid";
		long error = CUtilities::DNSLookup(name.c_str(), addresses);
		std::string msg;
		ASSERT_TRUE(0 != error) << FormatString("Found non-existent domain, %s.", name.c_str());
		ASSERT_TRUE(addresses.GetCount() == 0) << "IP addresses found or remain.";
	}


	TEST(UtilitiesUnitTests, UtilitiesCompareSysTimes)
	{
		SYSTEMTIME systime1 = CUtilities::GetUTCSystemTime(time(0));
		sleep(10);

		SYSTEMTIME systime2 = CUtilities::GetUTCSystemTime(time(0));

		ASSERT_TRUE( CUtilities::CompareSysTimes( systime1, systime1 ) == 0);
		ASSERT_TRUE( CUtilities::CompareSysTimes( systime1, systime2 ) == -1);
		ASSERT_TRUE( CUtilities::CompareSysTimes( systime2, systime1 ) == 1);
	}



	TEST(UtilitiesUnitTests, UtilitiesCompareVersions)
	{
		int major1 = 1;
		int minor1 = 2;
		int revision1 = 3;
		int build1 = 4;
		int major2 = 1;
		int minor2 = 2;
		int revision2 = 3;
		int build2 = 4;

		ASSERT_TRUE(CUtilities::CompareVersions( major1, minor1, revision1, build1, major2, minor2, revision2, build2) == 0);
		build1 = 3;
		build2 = 4;
		ASSERT_TRUE(CUtilities::CompareVersions( major1, minor1, revision1, build1, major2, minor2, revision2, build2) != 0);
		build1 = build2 = 4;
		revision1 = 3;
		revision2 = 4;
		ASSERT_TRUE(CUtilities::CompareVersions( major1, minor1, revision1, build1, major2, minor2, revision2, build2) != 0);
		revision1 = revision2 = 3;
		minor1 = 2;
		minor2 = 3;
		ASSERT_TRUE(CUtilities::CompareVersions( major1, minor1, revision1, build1, major2, minor2, revision2, build2) != 0);
		minor1 = minor2 = 2;
		major1 = 1;
		major2 = 2;
		ASSERT_TRUE(CUtilities::CompareVersions( major1, minor1, revision1, build1, major2, minor2, revision2, build2) == -1);
		ASSERT_TRUE(CUtilities::CompareVersions( build1, major2, minor2, revision2, build2, major1, minor1, revision1) == 1);
	}

	TEST(UtilitiesUnitTests, UtilitiesConvertRGBToBGR)
	{
		COLORREF result;
		int color = 0x00010203;
		result = CUtilities::ConvertRGBToBGR( color );
		ASSERT_TRUE( result == 0x00030201);
	}

	TEST(UtilitiesUnitTests, UtilitiesGetFormattedNumberString)
	{
		DWORD value = 0;
		std::string result;
		result = CUtilities::GetFormattedNumberString(value);
		ASSERT_TRUE( result == "0");

		value = 10;
		result = CUtilities::GetFormattedNumberString(value);
		ASSERT_TRUE( result == "10");

		value = 1000;
		result = CUtilities::GetFormattedNumberString(value);
		ASSERT_TRUE( result == "1,000");
		
		value = 10000;
		result = CUtilities::GetFormattedNumberString(value);
		ASSERT_TRUE( result == "10,000");

		value = 100000;
		result = CUtilities::GetFormattedNumberString(value);
		ASSERT_TRUE( result == "100,000");

		value = 1000000;
		result = CUtilities::GetFormattedNumberString(value);
		ASSERT_TRUE( result == "1,000,000");

		value = 10000000;
		result = CUtilities::GetFormattedNumberString(value);
		ASSERT_TRUE( result == "10,000,000");

		value = 1000000000;
		result = CUtilities::GetFormattedNumberString(value);
		ASSERT_TRUE( result == "1,000,000,000");
	}

	TEST(UtilitiesUnitTests, UtilitiesGetFormattedNumberString64)
	{
		typedef struct
		{
			// Input(s).
			uint64_t m_value;

			// Expected result(s).
			std::string m_resultExpected;
		} TestDataType;

		const TestDataType TEST_DATA[] =
		{
			{ 0, _T("0") },
			{ 9, _T("9") },
			{ 10, _T("10") },
			{ 99, _T("99") },
			{ 100, _T("100") },
			{ 999, _T("999") },

			{ 1000, _T("1,000") },
			{ 9999, _T("9,999") },
			{ 10000, _T("10,000") },
			{ 99999, _T("99,999") },
			{ 100000, _T("100,000") },
			{ 999999, _T("999,999") },

			{ 1000000, _T("1,000,000") },
			{ 9999999, _T("9,999,999") },
			{ 10000000, _T("10,000,000") },
			{ 99999999, _T("99,999,999") },
			{ 100000000, _T("100,000,000") },
			{ 999999999, _T("999,999,999") },

			{ 1000000000ULL, _T("1,000,000,000") },
			{ 9999999999ULL, _T("9,999,999,999") },
			{ 10000000000ULL, _T("10,000,000,000") },
			{ 99999999999ULL, _T("99,999,999,999") },
			{ 100000000000ULL, _T("100,000,000,000") },
			{ 999999999999ULL, _T("999,999,999,999") },

			{ 1000000000000ULL, _T("1,000,000,000,000") },
			{ 9999999999999ULL, _T("9,999,999,999,999") },
			{ 10000000000000ULL, _T("10,000,000,000,000") },
			{ 99999999999999ULL, _T("99,999,999,999,999") },
			{ 100000000000000ULL, _T("100,000,000,000,000") },
			{ 999999999999999ULL, _T("999,999,999,999,999") },

			{ 1000000000000000ULL, _T("1,000,000,000,000,000") },
			{ 9999999999999999ULL, _T("9,999,999,999,999,999") },
			{ 10000000000000000ULL, _T("10,000,000,000,000,000") },
			{ 99999999999999999ULL, _T("99,999,999,999,999,999") },
			{ 100000000000000000ULL, _T("100,000,000,000,000,000") },
			{ 999999999999999999ULL, _T("999,999,999,999,999,999") },

			{ 1000000000000000000ULL, _T("1,000,000,000,000,000,000") },
			{ 9999999999999999999ULL, _T("9,999,999,999,999,999,999") },
			{ 10000000000000000000ULL, _T("10,000,000,000,000,000,000") },
			{ UINT64_MAX, _T("18,446,744,073,709,551,615") },  
		};

		for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
		{
			// Perform operation(s) to be tested.
			const std::string result(CUtilities::GetFormattedNumberString(TEST_DATA[testIndex].m_value));

			// Test(s).
			EXPECT_EQ(result, TEST_DATA[testIndex].m_resultExpected);
		}
	}


	/// <summary>
	/// Code coverage and functional test for:
	///     CUtilities::GetInt64FromString()
	/// </summary>
	TEST(UtilitiesUnitTests, GetInt64FromStringTest)
	{
		char _buffer[128];

		typedef struct
		{
			// Input(s).
			const std::string m_stringValue;
			const __int64 m_variable;

			// Expected result(s).
			const __int64 m_variableExpected;
			const bool m_resultExpected;
		} TestDataType;

		const TestDataType TEST_DATA[] =
		{
			// Values that aren't even close.
			{ "cd", 123, 123, false },
			{ "cd", 456, 456, false },

			{ "", 123, 123, false },
			{ "", 456, 456, false },

			{ " ", 123, 123, false },
			{ " ", 456, 456, false },

			{ "   ", 123, 123, false },
			{ "   ", 456, 456, false },

			// Values that are close to valid or valid.
			{ "0x9999", 123, 123, false },
			{ "0x9999", 456, 456, false },

			{ "9999.", 123, 123, false },
			{ "9999.", 456, 456, false },

			{ "9999.0", 123, 123, false },
			{ "9999.0", 456, 456, false },

			{ "9999e0", 123, 123, false },
			{ "9999e0", 456, 456, false },

			{ "+9999", 123, 9999, true },
			{ "+9999", 456, 9999, true },

			{ "+ 9999", 123, 123, false },
			{ "+ 9999", 456, 456, false },

			{ " +9999", 123, 9999, true },
			{ " +9999", 456, 9999, true },

			{ "1001", 123, 1001, true },
			{ "1001", 456, 1001, true },

			{ " 1002", 123, 1002, true },
			{ " 1002", 456, 1002, true },

			{ "1003 ", 123, 123, false },
			{ "1003 ", 456, 456, false },

			{ " 1004 ", 123, 123, false },
			{ " 1004 ", 456, 456, false },

			{ "1005\n", 123, 123, false },
			{ "1005\n", 456, 456, false },

			// Non-negative values that are valid DWORD values and valid __int64 values.
			{ "0", 123, 0, true },
			{ "1", 123, 1, true },
			{ "2", 123, 2, true },
			{ "999", 123, 999, true },
			{ "1000", 123, 1000, true },
			{ "1001", 123, 1001, true },
			{ "1234567890", 123, 1234567890, true },
			{ "2147483647", 123, 2147483647, true }, // Maximum unsigned 31-bit value.
			{ "4294967295", 123, 4294967295, true }, // Maximum unsigned 32-bit value.

			// Non-negative values that are not valid DWORD values, but are (mostly) valid __int64 values.
			{ "4294967296", 123, 4294967296LL, true }, // One more than maximum unsigned 32-bit value.
			{ "4294967296", 456, 4294967296LL, true },

			{ "9999999999", 123, 9999999999LL, true },
			{ "9999999999", 456, 9999999999LL, true },

			{ "1234567890123456789", 123, 1234567890123456789LL, true }, // Large signed 64-bit value.
			{ "1234567890123456789", 456, 1234567890123456789LL, true },

			{ "9123456789012345678", 123, 9123456789012345678LL, true }, // Very large signed 64-bit value.
			{ "9123456789012345678", 456, 9123456789012345678LL, true },

			{ "9223372036854775807", 123, 9223372036854775807LL, true }, // Maximum signed 64-bit value.
			{ "9223372036854775807", 456, 9223372036854775807LL, true },

			{ "9223372036854775808", 123, 123, false }, // Maximum signed 64-bit value, plus 1.
			{ "9223372036854775808", 456, 456, false },

			{ "18446744073709551615", 123, 123, false }, // Maximum unsigned 64-bit value.
			{ "18446744073709551615", 456, 456, false },

			{ "99999999999999999999", 123, 123, false },
			{ "99999999999999999999", 456, 456, false },

			{ "9999999999999999999999999999999999999999", 123, 123, false },
			{ "9999999999999999999999999999999999999999", 456, 456, false },

			// Negative values that are valid DWORD values and valid __int64 values.
			{ "-0", 123, 0, true },
			{ "-1", 123, -1, true },
			{ "-2", 123, -2, true },
			{ "-999", 123, -999, true },
			{ "-1000", 123, -1000, true },
			{ "-1001", 123, -1001, true },
			{ "-1234567890", 123, -1234567890, true },
			{ "-2147483647", 123, -2147483647, true }, // One more than minimum signed 31-bit value.
			{ "-2147483648", 123, INT_MIN, true }, // Minimum signed 31-bit value.
			{ "-2147483649", 123, -2147483649LL, true }, // One less than maximum unsigned 31-bit value.
			{ "-4294966295", 123, -4294966295LL, true }, // Equivalent to 1001 for a DWORD.
			{ "-4294966296", 123, -4294966296LL, true }, // Equivalent to 1000 for a DWORD.
			{ "-4294966297", 123, -4294966297LL, true }, // Equivalent to 999 for a DWORD.
			{ "-4294967295", 123, -4294967295LL, true }, // Maximum unsigned 32-bit value, negated.

			// Negative values that are not valid DWORD values, but are (mostly) valid __in64 values.
			{ "-4294967296", 123, -4294967296LL, true }, // One more than maximum unsigned 32-bit value, negated.
			{ "-4294967296", 456, -4294967296LL, true }, // One more than maximum unsigned 32-bit value, negated.

			{ "-9999999999", 123, -9999999999LL, true },
			{ "-9999999999", 456, -9999999999LL, true },

			{ "-1234567890123456789", 123, -1234567890123456789LL, true }, // Small signed 64-bit value.
			{ "-1234567890123456789", 456, -1234567890123456789LL, true },

			{ "-9123456789012345678", 123, -9123456789012345678LL, true }, // Very small signed 64-bit value.
			{ "-9123456789012345678", 456, -9123456789012345678LL, true },

			{ "-9223372036854775807", 123, -9223372036854775807LL, true }, // Minimum signed 64-bit value, plus 1.
			{ "-9223372036854775807", 456, -9223372036854775807LL, true },

			{ "-9223372036854775808", 123, (-9223372036854775807LL -1), true }, // Minimum signed 64-bit value.
			{ "-9223372036854775808", 456, (-9223372036854775807LL -1), true },

			{ "-9223372036854775809", 123, 123, false }, // Minimum signed 64-bit value, less 1.
			{ "-9223372036854775809", 456, 456, false },

			{ "-18446744073709551615", 123, 123, false }, // Maximum unsigned 64-bit value, negated.
			{ "-18446744073709551615", 456, 456, false },

			{ "99999999999999999999", 123, 123, false },
			{ "99999999999999999999", 456, 456, false },

			{ "-18446744073709551615", 123, 123, false }, // Maximum unsigned 64-bit value, negated.
			{ "-18446744073709551615", 456, 456, false }, // Maximum unsigned 64-bit value, negated.

			{ "-18446744073709551616", 123, 123, false }, // Maximum unsigned 64-bit value plus 1, negated.
			{ "-18446744073709551616", 456, 456, false }, // Maximum unsigned 64-bit value plus 1, negated.

			{ "-99999999999999999999", 123, 123, false },
			{ "-99999999999999999999", 456, 456, false },

			{ "-9999999999999999999999999999999999999999", 123, 123, false },
			{ "-9999999999999999999999999999999999999999", 456, 456, false },

			// More content.
			{ "01234", 123, 1234, true }, // Test that a value with a leading zero is not treated as a valid octal value.
			{ "01234", 456, 1234, true }, // Test that a value with a leading zero is not treated as a valid octal value.

			{ "09999", 123, 9999, true }, // Test that a value with a leading zero is not treated as an invalid octal value.
			{ "09999", 456, 9999, true }, // Test that a value with a leading zero is not treated as an invalid octal value.
		};

		for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
		{
			// Setup.
			__int64 variable = 0;
			std::string svariable;
			try
			{
				// Perform operation(s) to be tested.
				variable = TEST_DATA[testIndex].m_variable;
				svariable = TEST_DATA[testIndex].m_stringValue;
				bool result = CUtilities::GetInt64FromString(svariable, variable);
				// Test(s).
				EXPECT_EQ(result, TEST_DATA[testIndex].m_resultExpected) 
					<< FormatString("GetInt64FromString(\"%s\", %ld) result", svariable.c_str(), variable);
				EXPECT_EQ(variable, TEST_DATA[testIndex].m_variableExpected) 
					<< FormatString("GetInt64FromString(\"%s\", %ld) variable", svariable.c_str(), variable);

				variable = TEST_DATA[testIndex].m_variable;
				svariable = TEST_DATA[testIndex].m_stringValue;
				result = CUtilities::GetInt64FromString(svariable, variable);
				EXPECT_EQ(TEST_DATA[testIndex].m_resultExpected, result) 
					<< FormatString("GetInt64FromString(std::string(\"%s\", %ld) result", svariable.c_str(), variable);
				ASSERT_TRUE(TEST_DATA[testIndex].m_variableExpected == variable) 
					<< FormatString("GetInt64FromString(std::string(\"%s\", %ld) variable", svariable.c_str(), variable);
			}
			catch (...) 
			{
				ASSERT_TRUE(false) << FormatString("GetInt64FromString(std::string(\"%s\") variable", svariable.c_str()); 
			}
		}
	}


	/// <summary>
	/// Code coverage and functional test for:
	///     CUtilities::GetInt64FromCurrencyString(LPCTSTR, __int64&)
	// Test with strings (mostly) without a decimal point or with 2 digits to the right of the decimal point.
	/// </summary>
	TEST(UtilitiesUnitTests, GetInt64FromCurrencyString_Bool1Test)
	{
		char _buffer[128];

		typedef struct
		{
			// Input(s).
			const LPCTSTR m_stringValue;
			const __int64 m_variable;

			// Expected result(s).
			const __int64 m_variableExpected;
			const bool m_resultExpected;
		} TestDataType;

		const TestDataType TEST_DATA[] =
		{
			// Values that aren't even close.
			{ _T("cd"), 123, 123, false },
			{ _T("cd"), 456, 456, false },

			{ _T(""), 123, 123, false },
			{ _T(""), 456, 456, false },

			{ _T(" "), 123, 123, false },
			{ _T(" "), 456, 456, false },

			{ _T("   "), 123, 123, false },
			{ _T("   "), 456, 456, false },

			// Values that are close to valid or valid.
			{ _T("0x9999"), 123, 123, false },
			{ _T("0x9999"), 456, 456, false },

			{ _T("9999."), 123, 999900, true },
			{ _T("9999."), 456, 999900, true },

			{ _T("9999.0"), 123, 999900, true },
			{ _T("9999.0"), 456, 999900, true },

			{ _T("9999.1"), 123, 999910, true },
			{ _T("9999.1"), 456, 999910, true },

			{ _T("9999.12"), 123, 999912, true },
			{ _T("9999.12"), 456, 999912, true },

			{ _T("9999e0"), 123, 123, false },
			{ _T("9999e0"), 456, 456, false },

			{ _T("+9999"), 123, 999900, true },
			{ _T("+9999"), 456, 999900, true },

			{ _T("+ 9999"), 123, 123, false },
			{ _T("+ 9999"), 456, 456, false },

			{ _T(" +9999"), 123, 999900, true },
			{ _T(" +9999"), 456, 999900, true },

			{ _T("1001"), 123, 100100, true },
			{ _T("1001"), 456, 100100, true },

			{ _T(" 1002"), 123, 100200, true },
			{ _T(" 1002"), 456, 100200, true },

			{ _T("1003 "), 123, 123, false},
			{ _T("1003 "), 456, 456, false },

			{ _T(" 1004 "), 123, 123, false },
			{ _T(" 1004 "), 456, 456, false },

			{ _T("1005\n"), 123, 123, false },
			{ _T("1005\n"), 456, 456, false },

			{ _T("\t1006"), 123, 100600, true },
			{ _T("\t1006"), 456, 100600, true },

			{ _T(" \t 1007"), 123, 100700, true },
			{ _T(" \t 1007"), 456, 100700, true },

			{ _T(" \t \t 1008"), 123, 100800, true },
			{ _T(" \t \t 1008"), 456, 100800, true },

			{ _T("\t1009"), 123, 100900, true },
			{ _T("\t1009"), 456, 100900, true },

			{ _T("\a1010"), 123, 123, false },
			{ _T("\a1010"), 123, 123, false },

			{ _T("\b1011"), 123, 123, false },
			{ _T("\b1011"), 456, 456, false },

			{ _T("\f1012"), 123, 101200, true },
			{ _T("\f1012"), 456, 101200, true },

			{ _T("\n1013"), 123, 101300, true },
			{ _T("\n1013"), 456, 101300, true },

			{ _T("\r1014"), 123, 101400, true },
			{ _T("\r1014"), 456, 101400, true },

			{ _T("\v1015"), 123, 101500, true },
			{ _T("\v1015"), 456, 101500, true },

			{ _T(" \t\f\n\r\v \t\f\n\r\v \t\f\n\r\v 1016"), 123, 101600, true },
			{ _T(" \t\f\n\r\v \t\f\n\r\v \t\f\n\r\v 1016"), 456, 101600, true },

			// Non-negative values that are valid DWORD values and valid "__int64" currency values.
			{ _T("0"), 123, 0, true },
			{ _T("1"), 123, 100, true },
			{ _T("2"), 123, 200, true },
			{ _T("999"), 123, 99900, true },
			{ _T("1000"), 123, 100000, true },
			{ _T("1001"), 123, 100100, true },
			{ _T("12345678.90"), 123, 1234567890, true },
			{ _T("21474836.47"), 123, 2147483647, true }, // Maximum signed 32-bit value.
			{ _T("42949672.95"), 123, 4294967295, true }, // Maximum unsigned 32-bit value.

			{ _T("1234567890"), 123, 123456789000, true },
			{ _T("2147483647"), 123, 214748364700, true }, // Maximum signed 32-bit value * 100.
			{ _T("4294967295"), 123, 429496729500, true }, // Maximum unsigned 32-bit value * 100.

			// Non-negative values that are not valid DWORD values, but are (mostly) valid "__int64" currency values.
			{ _T("42949672.96"), 123, 4294967296LL, true }, // One more than maximum unsigned 32-bit value.
			{ _T("42949672.96"), 456, 4294967296LL, true },
			{ _T("4294967296"), 123, 429496729600LL, true }, // One more than maximum unsigned 32-bit value * 100.
			{ _T("4294967296"), 456, 429496729600LL, true },

			{ _T("99999999.99"), 123, 9999999999LL, true },
			{ _T("99999999.99"), 456, 9999999999LL, true },
			{ _T("9999999999"), 123, 999999999900LL, true },
			{ _T("9999999999"), 456, 999999999900LL, true },

			{ _T("12345678901234567.89"), 123, 1234567890123456789LL, true }, // Large signed 64-bit value.
			{ _T("12345678901234567.89"), 456, 1234567890123456789LL, true },
			{ _T("1234567890123456789"), 123, 123, false }, // Large signed 64-bit value.
			{ _T("1234567890123456789"), 456, 456, false },

			{ _T("91234567890123456.78"), 123, 9123456789012345678LL, true }, // Very large signed 64-bit value.
			{ _T("91234567890123456.78"), 456, 9123456789012345678LL, true },
			{ _T("91234567890123456"), 123, 9123456789012345600LL, true }, // Very large signed 64-bit value.
			{ _T("91234567890123456"), 456, 9123456789012345600LL, true },
			{ _T("9123456789012345678"), 123, 123, false }, // Very large signed 64-bit value.
			{ _T("9123456789012345678"), 456, 456, false },

			{ _T("92233720368547758.07"), 123, 9223372036854775807LL, true }, // Maximum signed 64-bit value.
			{ _T("92233720368547758.07"), 456, 9223372036854775807LL, true },
			{ _T("92233720368547758"), 123, 9223372036854775800, true }, // Maximum signed 64-bit value.
			{ _T("92233720368547758"), 456, 9223372036854775800, true },
			{ _T("9223372036854775807"), 123, 123, false }, // Maximum signed 64-bit value.
			{ _T("9223372036854775807"), 456, 456, false },

			{ _T("92233720368547758.08"), 123, 123, false }, // Maximum signed 64-bit value, plus 1.
			{ _T("92233720368547758.08"), 456, 456, false },
			{ _T("92233720368547759"), 123, 123, false }, // Maximum signed 64-bit value.
			{ _T("92233720368547759"), 456, 456, false },
			{ _T("9223372036854775808"), 123, 123, false }, // Maximum signed 64-bit value, plus 1.
			{ _T("9223372036854775808"), 456, 456, false },

			{ _T("184467440737095516.15"), 123, 123, false }, // Maximum unsigned 64-bit value.
			{ _T("184467440737095516.15"), 456, 456, false },
			{ _T("18446744073709551615"), 123, 123, false }, // Maximum unsigned 64-bit value.
			{ _T("18446744073709551615"), 456, 456, false },

			{ _T("999999999999999999.99"), 123, 123, false },
			{ _T("999999999999999999.99"), 456, 456, false },
			{ _T("99999999999999999999"), 123, 123, false },
			{ _T("99999999999999999999"), 456, 456, false },

			{ _T("99999999999999999999999999999999999999.99"), 123, 123, false },
			{ _T("99999999999999999999999999999999999999.99"), 456, 456, false },
			{ _T("9999999999999999999999999999999999999999"), 123, 123, false },
			{ _T("9999999999999999999999999999999999999999"), 456, 456, false },

			// Negative values that are valid DWORD values and valid __int64 values.
			{ _T("-0"), 123, 0, true },
			{ _T("-1"), 123, -100, true },
			{ _T("-2"), 123, -200, true },
			{ _T("-999"), 123, -99900, true },
			{ _T("-1000"), 123, -100000, true },
			{ _T("-1001"), 123, -100100, true },

			{ _T("-12345678.90"), 123, -1234567890, true },
			{ _T("-21474836.47"), 123, -2147483647, true }, // One more than minimum signed 31-bit value.
			{ _T("-21474836.48"), 123, INT_MIN, true }, // Minimum signed 31-bit value.
			{ _T("-21474836.49"), 123, -2147483649LL, true }, // One less than maximum unsigned 31-bit value.
			{ _T("-42949662.95"), 123, -4294966295LL, true }, // Equivalent to 1001 for a DWORD.
			{ _T("-42949662.96"), 123, -4294966296LL, true }, // Equivalent to 1000 for a DWORD.
			{ _T("-42949662.97"), 123, -4294966297LL, true }, // Equivalent to 999 for a DWORD.
			{ _T("-42949672.95"), 123, -4294967295LL, true }, // Maximum unsigned 32-bit value, negated.

			{ _T("-1234567890"), 123, -123456789000, true },
			{ _T("-2147483647"), 123, -214748364700, true }, // One more than minimum signed 31-bit value.
			{ _T("-2147483648"), 123, INT_MIN * 100LL, true }, // Minimum signed 31-bit value.
			{ _T("-2147483649"), 123, -214748364900LL, true }, // One less than maximum unsigned 31-bit value.
			{ _T("-4294966295"), 123, -429496629500LL, true }, // Equivalent to 1001 for a DWORD.
			{ _T("-4294966296"), 123, -429496629600LL, true }, // Equivalent to 1000 for a DWORD.
			{ _T("-4294966297"), 123, -429496629700LL, true }, // Equivalent to 999 for a DWORD.
			{ _T("-4294967295"), 123, -429496729500LL, true }, // Maximum unsigned 32-bit value, negated.

			// Negative values that are not valid DWORD values, but are (mostly) valid __in64 values.
			{ _T("-42949672.96"), 123, -4294967296LL, true }, // One more than maximum unsigned 32-bit value, negated.
			{ _T("-42949672.96"), 456, -4294967296LL, true }, // One more than maximum unsigned 32-bit value, negated.
			{ _T("-4294967296"), 123, -429496729600LL, true }, // One more than maximum unsigned 32-bit value, negated.
			{ _T("-4294967296"), 456, -429496729600LL, true }, // One more than maximum unsigned 32-bit value, negated.

			{ _T("-99999999.99"), 123, -9999999999LL, true },
			{ _T("-99999999.99"), 456, -9999999999LL, true },
			{ _T("-9999999999"), 123, -999999999900LL, true },
			{ _T("-9999999999"), 456, -999999999900LL, true },

			{ _T("-12345678901234567.89"), 123, -1234567890123456789LL, true }, // Small signed 64-bit value.
			{ _T("-12345678901234567.89"), 456, -1234567890123456789LL, true },
			{ _T("-12345678901234567"), 123, -1234567890123456700LL, true }, // Small signed 64-bit value.
			{ _T("-12345678901234567"), 456, -1234567890123456700LL, true },
			{ _T("-1234567890123456789"), 123, 123, false }, // Small signed 64-bit value.
			{ _T("-1234567890123456789"), 456, 456, false },

			{ _T("-91234567890123456.78"), 123, -9123456789012345678LL, true }, // Very small signed 64-bit value.
			{ _T("-91234567890123456.78"), 456, -9123456789012345678LL, true },
			{ _T("-91234567890123456"), 123, -9123456789012345600LL, true }, // Very small signed 64-bit value.
			{ _T("-91234567890123456"), 456, -9123456789012345600LL, true },
			{ _T("-9123456789012345678"), 123, 123, false }, // Very small signed 64-bit value.
			{ _T("-9123456789012345678"), 456, 456, false },

			{ _T("-92233720368547758.07"), 123, -9223372036854775807LL, true }, // Minimum signed 64-bit value, plus 1.
			{ _T("-92233720368547758.07"), 456, -9223372036854775807LL, true },
			{ _T("-92233720368547758"), 123, -9223372036854775800LL, true }, // Minimum signed 64-bit value, plus 1.
			{ _T("-92233720368547758"), 456, -9223372036854775800LL, true },
			{ _T("-9223372036854775807"), 123, 123, false }, // Minimum signed 64-bit value, plus 1.
			{ _T("-9223372036854775807"), 456, 456, false },

			{ _T("-92233720368547758.08"), 123, (-9223372036854775807LL -1), true }, // Minimum signed 64-bit value.
			{ _T("-92233720368547758.08"), 456, (-9223372036854775807LL -1), true },
			{ _T("-92233720368547758"), 123, -9223372036854775800LL, true }, // Minimum signed 64-bit value.
			{ _T("-92233720368547758"), 456, -9223372036854775800LL, true },
			{ _T("-9223372036854775808"), 123, 123, false }, // Minimum signed 64-bit value.
			{ _T("-9223372036854775808"), 456, 456, false },

			{ _T("-92233720368547758.09"), 123, 123, false }, // Minimum signed 64-bit value, less 1.
			{ _T("-92233720368547758.09"), 456, 456, false },
			{ _T("-92233720368547759"), 123, 123, false }, // Minimum signed 64-bit value, less 1.
			{ _T("-92233720368547759"), 456, 456, false },
			{ _T("-9223372036854775809"), 123, 123, false }, // Minimum signed 64-bit value, less 1.
			{ _T("-9223372036854775809"), 456, 456, false },

			{ _T("-184467440737095516.15"), 123, 123, false }, // Maximum unsigned 64-bit value, negated.
			{ _T("-184467440737095516.15"), 456, 456, false },
			{ _T("-184467440737095517"), 123, 123, false }, // Maximum unsigned 64-bit value, negated.
			{ _T("-184467440737095517"), 456, 456, false },
			{ _T("-18446744073709551615"), 123, 123, false }, // Maximum unsigned 64-bit value, negated.
			{ _T("-18446744073709551615"), 456, 456, false },

			{ _T("999999999999999999.99"), 123, 123, false },
			{ _T("999999999999999999.99"), 456, 456, false },
			{ _T("999999999999999999"), 123, 123, false },
			{ _T("999999999999999999"), 456, 456, false },
			{ _T("99999999999999999999"), 123, 123, false },
			{ _T("99999999999999999999"), 456, 456, false },

			{ _T("-184467440737095516.15"), 123, 123, false }, // Maximum unsigned 64-bit value, negated.
			{ _T("-184467440737095516.15"), 456, 456, false }, // Maximum unsigned 64-bit value, negated.
			{ _T("-184467440737095516"), 123, 123, false }, // Maximum unsigned 64-bit value, negated.
			{ _T("-184467440737095516"), 456, 456, false }, // Maximum unsigned 64-bit value, negated.
			{ _T("-18446744073709551615"), 123, 123, false }, // Maximum unsigned 64-bit value, negated.
			{ _T("-18446744073709551615"), 456, 456, false }, // Maximum unsigned 64-bit value, negated.

			{ _T("-184467440737095516.16"), 123, 123, false }, // Maximum unsigned 64-bit value plus 1, negated.
			{ _T("-184467440737095516.16"), 456, 456, false }, // Maximum unsigned 64-bit value plus 1, negated.
			{ _T("-184467440737095516"), 123, 123, false }, // Maximum unsigned 64-bit value plus 1, negated.
			{ _T("-184467440737095516"), 456, 456, false }, // Maximum unsigned 64-bit value plus 1, negated.
			{ _T("-18446744073709551616"), 123, 123, false }, // Maximum unsigned 64-bit value plus 1, negated.
			{ _T("-18446744073709551616"), 456, 456, false }, // Maximum unsigned 64-bit value plus 1, negated.

			{ _T("-999999999999999999.99"), 123, 123, false },
			{ _T("-999999999999999999.99"), 456, 456, false },
			{ _T("-999999999999999999"), 123, 123, false },
			{ _T("-999999999999999999"), 456, 456, false },
			{ _T("-99999999999999999999"), 123, 123, false },
			{ _T("-99999999999999999999"), 456, 456, false },

			{ _T("-99999999999999999999999999999999999999.99"), 123, 123, false },
			{ _T("-99999999999999999999999999999999999999.99"), 456, 456, false },
			{ _T("-99999999999999999999999999999999999999"), 123, 123, false },
			{ _T("-99999999999999999999999999999999999999"), 456, 456, false },
			{ _T("-9999999999999999999999999999999999999999"), 123, 123, false },
			{ _T("-9999999999999999999999999999999999999999"), 456, 456, false },

			// More content.
			{ _T("012.34"), 123, 1234, true }, // Test that a value with a leading zero is not treated as a valid octal value.
			{ _T("012.34"), 456, 1234, true }, // Test that a value with a leading zero is not treated as a valid octal value.
			{ _T("012"), 123, 1200, true }, // Test that a value with a leading zero is not treated as a valid octal value.
			{ _T("012"), 456, 1200, true }, // Test that a value with a leading zero is not treated as a valid octal value.
			{ _T("01234"), 123, 123400, true }, // Test that a value with a leading zero is not treated as a valid octal value.
			{ _T("01234"), 456, 123400, true }, // Test that a value with a leading zero is not treated as a valid octal value.

			{ _T("099.99"), 123, 9999, true }, // Test that a value with a leading zero is not treated as an invalid octal value.
			{ _T("099.99"), 456, 9999, true }, // Test that a value with a leading zero is not treated as an invalid octal value.
			{ _T("099"), 123, 9900, true }, // Test that a value with a leading zero is not treated as an invalid octal value.
			{ _T("099"), 456, 9900, true }, // Test that a value with a leading zero is not treated as an invalid octal value.
			{ _T("09999"), 123, 999900, true }, // Test that a value with a leading zero is not treated as an invalid octal value.
			{ _T("09999"), 456, 999900, true }, // Test that a value with a leading zero is not treated as an invalid octal value.
		};

		for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
		{
			// Setup.
			__int64 variable(TEST_DATA[testIndex].m_variable);

			// Perform operation(s) to be tested.
			const bool result = CUtilities::GetInt64FromCurrencyString(TEST_DATA[testIndex].m_stringValue, variable);

			// Test(s).
			std::string resultName;
			sprintf(_buffer, _T("GetInt64FromCurrencyString(\"%s\") result"),
				static_cast<LPCTSTR>(TEST_DATA[testIndex].m_stringValue));
			resultName = std::string(_buffer);
			EXPECT_EQ(result, TEST_DATA[testIndex].m_resultExpected) << resultName;

			sprintf(_buffer, _T("GetInt64FromCurrencyString(\"%s\") variable"),
				static_cast<LPCTSTR>(TEST_DATA[testIndex].m_stringValue));
			resultName = std::string(_buffer);
			EXPECT_EQ(variable, TEST_DATA[testIndex].m_variableExpected) << resultName;
		}
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CUtilities::GetInt64FromCurrencyString(LPCTSTR, __int64&)
	// Test with strings with a decimal point.
	/// </summary>
	TEST(UtilitiesUnitTests, GetInt64FromCurrencyString_Bool2Test)
	{
		typedef struct
		{
			// Input(s).
			const LPCTSTR m_stringValue;
			const __int64 m_variable;

			// Expected result(s).
			const __int64 m_variableExpected;
			const bool m_resultExpected;
		} TestDataType;

		const TestDataType TEST_DATA[] =
		{
			// Values that aren't even close.
			{ _T("12345.cd"), 123, 123, false },
			{ _T("12345.cd"), 456, 456, false },

			// Values that are close to valid or valid.
			{ _T("12345."), 123, 1234500, true },
			{ _T("12345."), 456, 1234500, true },

			{ _T("12345. "), 123, 123, false },
			{ _T("12345. "), 456, 456, false },

			{ _T("12345.   "), 123, 123, false },
			{ _T("12345.   "), 456, 456, false },

			{ _T("9999."), 123, 999900, true },
			{ _T("9999."), 456, 999900, true },

			{ _T("9999.0"), 123, 999900, true },
			{ _T("9999.0"), 456, 999900, true },

			{ _T("9999.0a"), 123, 123, false },
			{ _T("9999.0a"), 456, 456, false },

			{ _T("9999.00a"), 123, 123, false },
			{ _T("9999.00a"), 456, 456, false },

			{ _T("9999.000a"), 123, 123, false },
			{ _T("9999.000a"), 456, 456, false },

			{ _T("9999.0000a"), 123, 123, false },
			{ _T("9999.0000a"), 456, 456, false },

			{ _T("9999.000000000000000000000000000000a"), 123, 123, false },
			{ _T("9999.000000000000000000000000000000a"), 456, 456, false },

			{ _T("9999.000000000000000000000000000000"), 123, 999900, true },
			{ _T("9999.000000000000000000000000000000"), 456, 999900, true },

			{ _T("9999.0a000000000000000000000000000000"), 123, 123, false },
			{ _T("9999.0a000000000000000000000000000000"), 456, 456, false },

			{ _T("9999.00a000000000000000000000000000000"), 123, 123, false },
			{ _T("9999.00a000000000000000000000000000000"), 456, 456, false },

			{ _T("9999.000a000000000000000000000000000000"), 123, 123, false },
			{ _T("9999.000a000000000000000000000000000000"), 456, 456, false },

			{ _T("9999.0000a000000000000000000000000000000"), 123, 123, false },
			{ _T("9999.0000a000000000000000000000000000000"), 456, 456, false },

			{ _T("9999.000000000000000000000000000000a000000000000000000000000000000"), 123, 123, false },
			{ _T("9999.000000000000000000000000000000a000000000000000000000000000000"), 456, 456, false },

			{ _T("9999. 000000000000000000000000000000"), 123, 123, false },
			{ _T("9999. 000000000000000000000000000000"), 456, 456, false },

			{ _T("9999.0 000000000000000000000000000000"), 123, 123, false },
			{ _T("9999.0 000000000000000000000000000000"), 456, 456, false },

			{ _T("9999.00 000000000000000000000000000000"), 123, 123, false },
			{ _T("9999.00 000000000000000000000000000000"), 456, 456, false },

			{ _T("9999.000 000000000000000000000000000000"), 123, 123, false },
			{ _T("9999.000 000000000000000000000000000000"), 456, 456, false },

			{ _T("9999.0000 000000000000000000000000000000"), 123, 123, false },
			{ _T("9999.0000 000000000000000000000000000000"), 456, 456, false },

			{ _T("9999.000000000000000000000000000000 000000000000000000000000000000"), 123, 123, false },
			{ _T("9999.000000000000000000000000000000 000000000000000000000000000000"), 456, 456, false },

			{ _T("9999.9"), 123, 999990, true },
			{ _T("9999.9"), 456, 999990, true },

			{ _T("9999.00"), 123, 999900, true },
			{ _T("9999.00"), 456, 999900, true },

			{ _T("9999.01"), 123, 999901, true },
			{ _T("9999.01"), 456, 999901, true },

			{ _T("9999.10"), 123, 999910, true },
			{ _T("9999.10"), 456, 999910, true },

			{ _T("9999.90"), 123, 999990, true },
			{ _T("9999.90"), 456, 999990, true },

			{ _T("9999.99"), 123, 999999, true },
			{ _T("9999.99"), 456, 999999, true },

			{ _T("9999.000"), 123, 999900, true },
			{ _T("9999.000"), 456, 999900, true },

			{ _T("9999.009"), 123, 999900, true },
			{ _T("9999.009"), 456, 999900, true },

			{ _T("9999.990"), 123, 999999, true },
			{ _T("9999.990"), 456, 999999, true },

			{ _T("9999.999"), 123, 999999, true },
			{ _T("9999.999"), 456, 999999, true },

			{ _T("9999.999999999999999999999999999999"), 123, 999999, true },
			{ _T("9999.999999999999999999999999999999"), 456, 999999, true },

			{ _T("9999.e0"), 123, 123, false },
			{ _T("9999.e0"), 456, 456, false },

			{ _T("9999.0e0"), 123, 123, false },
			{ _T("9999.0e0"), 456, 456, false },

			{ _T("12345. 00"), 123, 123, false },
			{ _T("12345. 00"), 456, 456, false },

			{ _T("12345.+00"), 123, 123, false },
			{ _T("12345.+00"), 456, 456, false },

			{ _T("12345.-00"), 123, 123, false },
			{ _T("12345.-00"), 456, 456, false },

			{ _T("12345.+99"), 123, 123, false },
			{ _T("12345.+99"), 456, 456, false },

			{ _T("12345.-99"), 123, 123, false },
			{ _T("12345.-99"), 456, 456, false },

			{ _T("12345.99-"), 123, 123, false },
			{ _T("12345.99-"), 456, 456, false },

			{ _T("12345.99+"), 123, 123, false },
			{ _T("12345.99+"), 456, 456, false },

			{ _T("1001."), 123, 100100, true },
			{ _T("1001."), 456, 100100, true },

			{ _T(" 1002."), 123, 100200, true },
			{ _T(" 1002."), 456, 100200, true },

			{ _T("1003. "), 123, 123, false },
			{ _T("1003. "), 456, 456, false },

			{ _T("1004.\a"), 123, 123, false },
			{ _T("1004.\a"), 456, 456, false },

			{ _T("1005.\b"), 123, 123, false },
			{ _T("1005.\b"), 456, 456, false },

			{ _T("1006\f"), 123, 123, false },
			{ _T("1006\f"), 456, 456, false },

			{ _T("1007\n"), 123, 123, false },
			{ _T("1007\n"), 456, 456, false },

			{ _T("1008\r"), 123, 123, false },
			{ _T("1008\r"), 456, 456, false },

			{ _T("1009\t"), 123, 123, false },
			{ _T("1009\t"), 456, 456, false },

			{ _T("1010\v"), 123, 123, false },
			{ _T("1010\v"), 456, 456, false },

			{ _T("\b1011"), 123, 123, false },
			{ _T("\b1011"), 456, 456, false },

			{ _T("\f1012"), 123, 101200, true },
			{ _T("\f1012"), 456, 101200, true },

			{ _T("\n1013"), 123, 101300, true },
			{ _T("\n1013"), 456, 101300, true },

			{ _T("\r1014"), 123, 101400, true },
			{ _T("\r1014"), 456, 101400, true },

			{ _T("\v1015"), 123, 101500, true },
			{ _T("\v1015"), 456, 101500, true },

			{ _T(" \t\f\n\r\v \t\f\n\r\v \t\f\n\r\v 1016"), 123, 101600, true },
			{ _T(" \t\f\n\r\v \t\f\n\r\v \t\f\n\r\v 1016"), 456, 101600, true },

			// Non-negative values that are valid DWORD values and valid "__int64" currency values.
			{ _T("0.00999999999999999999999999999999"), 123, 0, true },
			{ _T("1.00999999999999999999999999999999"), 123, 100, true },
			{ _T("2.00999999999999999999999999999999"), 123, 200, true },
			{ _T("999.00999999999999999999999999999999"), 123, 99900, true },
			{ _T("1000.00999999999999999999999999999999"), 123, 100000, true },
			{ _T("1001.00999999999999999999999999999999"), 123, 100100, true },
			{ _T("12345678.90999999999999999999999999999999"), 123, 1234567890, true },
			{ _T("21474836.47999999999999999999999999999999"), 123, 2147483647, true }, // Maximum signed 32-bit value.
			{ _T("42949672.95999999999999999999999999999999"), 123, 4294967295, true }, // Maximum unsigned 32-bit value.

			{ _T("1234567890.00999999999999999999999999999999"), 123, 123456789000, true },
			{ _T("2147483647.00999999999999999999999999999999"), 123, 214748364700, true }, // Maximum signed 32-bit value * 100.
			{ _T("4294967295.00999999999999999999999999999999"), 123, 429496729500, true }, // Maximum unsigned 32-bit value * 100.

			// Non-negative values that are not valid DWORD values, but are (mostly) valid "__int64" currency values.
			{ _T("42949672.96999999999999999999999999999999"), 123, 4294967296LL, true }, // One more than maximum unsigned 32-bit value.
			{ _T("42949672.96999999999999999999999999999999"), 456, 4294967296LL, true },
			{ _T("4294967296.00999999999999999999999999999999"), 123, 429496729600LL, true }, // One more than maximum unsigned 32-bit value * 100.
			{ _T("4294967296.00999999999999999999999999999999"), 456, 429496729600LL, true },

			{ _T("99999999.99999999999999999999999999999999"), 123, 9999999999LL, true },
			{ _T("99999999.99999999999999999999999999999999"), 456, 9999999999LL, true },
			{ _T("9999999999.00999999999999999999999999999999"), 123, 999999999900LL, true },
			{ _T("9999999999.00999999999999999999999999999999"), 456, 999999999900LL, true },

			{ _T("12345678901234567.89999999999999999999999999999999"), 123, 1234567890123456789LL, true }, // Large signed 64-bit value.
			{ _T("12345678901234567.89999999999999999999999999999999"), 456, 1234567890123456789LL, true },
			{ _T("1234567890123456789.00999999999999999999999999999999"), 123, 123, false }, // Large signed 64-bit value.
			{ _T("1234567890123456789.00999999999999999999999999999999"), 456, 456, false },

			{ _T("91234567890123456.78999999999999999999999999999999"), 123, 9123456789012345678LL, true }, // Very large signed 64-bit value.
			{ _T("91234567890123456.78999999999999999999999999999999"), 456, 9123456789012345678LL, true },
			{ _T("91234567890123456.00999999999999999999999999999999"), 123, 9123456789012345600LL, true }, // Very large signed 64-bit value.
			{ _T("91234567890123456.00999999999999999999999999999999"), 456, 9123456789012345600LL, true },
			{ _T("9123456789012345678.00999999999999999999999999999999"), 123, 123, false }, // Very large signed 64-bit value.
			{ _T("9123456789012345678.00999999999999999999999999999999"), 456, 456, false },

			{ _T("92233720368547758.07999999999999999999999999999999"), 123, 9223372036854775807LL, true }, // Maximum signed 64-bit value.
			{ _T("92233720368547758.07999999999999999999999999999999"), 456, 9223372036854775807LL, true },
			{ _T("92233720368547758.00999999999999999999999999999999"), 123, 9223372036854775800, true }, // Maximum signed 64-bit value.
			{ _T("92233720368547758.00999999999999999999999999999999"), 456, 9223372036854775800, true },
			{ _T("9223372036854775807.00999999999999999999999999999999"), 123, 123, false }, // Maximum signed 64-bit value.
			{ _T("9223372036854775807.00999999999999999999999999999999"), 456, 456, false },

			{ _T("92233720368547758.08999999999999999999999999999999"), 123, 123, false }, // Maximum signed 64-bit value, plus 1.
			{ _T("92233720368547758.08999999999999999999999999999999"), 456, 456, false },
			{ _T("92233720368547759.00999999999999999999999999999999"), 123, 123, false }, // Maximum signed 64-bit value.
			{ _T("92233720368547759.00999999999999999999999999999999"), 456, 456, false },
			{ _T("9223372036854775808.00999999999999999999999999999999"), 123, 123, false }, // Maximum signed 64-bit value, plus 1.
			{ _T("9223372036854775808.00999999999999999999999999999999"), 456, 456, false },

			{ _T("184467440737095516.15999999999999999999999999999999"), 123, 123, false }, // Maximum unsigned 64-bit value.
			{ _T("184467440737095516.15999999999999999999999999999999"), 456, 456, false },
			{ _T("18446744073709551615.00999999999999999999999999999999"), 123, 123, false }, // Maximum unsigned 64-bit value.
			{ _T("18446744073709551615.00999999999999999999999999999999"), 456, 456, false },

			{ _T("999999999999999999.99999999999999999999999999999999"), 123, 123, false },
			{ _T("999999999999999999.99999999999999999999999999999999"), 456, 456, false },
			{ _T("99999999999999999999.00999999999999999999999999999999"), 123, 123, false },
			{ _T("99999999999999999999.00999999999999999999999999999999"), 456, 456, false },

			{ _T("99999999999999999999999999999999999999.99999999999999999999999999999999"), 123, 123, false },
			{ _T("99999999999999999999999999999999999999.99999999999999999999999999999999"), 456, 456, false },
			{ _T("9999999999999999999999999999999999999999.00999999999999999999999999999999"), 123, 123, false },
			{ _T("9999999999999999999999999999999999999999.00999999999999999999999999999999"), 456, 456, false },

			// Negative values that are valid DWORD values and valid __int64 values.
			{ _T("-0.00999999999999999999999999999999"), 123, 0, true },
			{ _T("-1.00999999999999999999999999999999"), 123, -100, true },
			{ _T("-2.00999999999999999999999999999999"), 123, -200, true },
			{ _T("-999.00999999999999999999999999999999"), 123, -99900, true },
			{ _T("-1000.00999999999999999999999999999999"), 123, -100000, true },
			{ _T("-1001.00999999999999999999999999999999"), 123, -100100, true },

			{ _T("-12345678.90999999999999999999999999999999"), 123, -1234567890, true },
			{ _T("-21474836.47999999999999999999999999999999"), 123, -2147483647, true }, // One more than minimum signed 31-bit value.
			{ _T("-21474836.48999999999999999999999999999999"), 123, INT_MIN, true }, // Minimum signed 31-bit value.
			{ _T("-21474836.49999999999999999999999999999999"), 123, -2147483649LL, true }, // One less than maximum unsigned 31-bit value.
			{ _T("-42949662.95999999999999999999999999999999"), 123, -4294966295LL, true }, // Equivalent to 1001 for a DWORD.
			{ _T("-42949662.96999999999999999999999999999999"), 123, -4294966296LL, true }, // Equivalent to 1000 for a DWORD.
			{ _T("-42949662.97999999999999999999999999999999"), 123, -4294966297LL, true }, // Equivalent to 999 for a DWORD.
			{ _T("-42949672.95999999999999999999999999999999"), 123, -4294967295LL, true }, // Maximum unsigned 32-bit value, negated.

			{ _T("-1234567890.00999999999999999999999999999999"), 123, -123456789000, true },
			{ _T("-2147483647.00999999999999999999999999999999"), 123, -214748364700, true }, // One more than minimum signed 31-bit value.
			{ _T("-2147483648.00999999999999999999999999999999"), 123, INT_MIN * 100LL, true }, // Minimum signed 31-bit value.
			{ _T("-2147483649.00999999999999999999999999999999"), 123, -214748364900LL, true }, // One less than maximum unsigned 31-bit value.
			{ _T("-4294966295.00999999999999999999999999999999"), 123, -429496629500LL, true }, // Equivalent to 1001 for a DWORD.
			{ _T("-4294966296.00999999999999999999999999999999"), 123, -429496629600LL, true }, // Equivalent to 1000 for a DWORD.
			{ _T("-4294966297.00999999999999999999999999999999"), 123, -429496629700LL, true }, // Equivalent to 999 for a DWORD.
			{ _T("-4294967295.00999999999999999999999999999999"), 123, -429496729500LL, true }, // Maximum unsigned 32-bit value, negated.

			// Negative values that are not valid DWORD values, but are (mostly) valid __in64 values.
			{ _T("-42949672.96999999999999999999999999999999"), 123, -4294967296LL, true }, // One more than maximum unsigned 32-bit value, negated.
			{ _T("-42949672.96999999999999999999999999999999"), 456, -4294967296LL, true }, // One more than maximum unsigned 32-bit value, negated.
			{ _T("-4294967296.00999999999999999999999999999999"), 123, -429496729600LL, true }, // One more than maximum unsigned 32-bit value, negated.
			{ _T("-4294967296.00999999999999999999999999999999"), 456, -429496729600LL, true }, // One more than maximum unsigned 32-bit value, negated.

			{ _T("-99999999.99999999999999999999999999999999"), 123, -9999999999LL, true },
			{ _T("-99999999.99999999999999999999999999999999"), 456, -9999999999LL, true },
			{ _T("-9999999999.00999999999999999999999999999999"), 123, -999999999900LL, true },
			{ _T("-9999999999.00999999999999999999999999999999"), 456, -999999999900LL, true },

			{ _T("-12345678901234567.89999999999999999999999999999999"), 123, -1234567890123456789LL, true }, // Small signed 64-bit value.
			{ _T("-12345678901234567.89999999999999999999999999999999"), 456, -1234567890123456789LL, true },
			{ _T("-12345678901234567.00999999999999999999999999999999"), 123, -1234567890123456700LL, true }, // Small signed 64-bit value.
			{ _T("-12345678901234567.00999999999999999999999999999999"), 456, -1234567890123456700LL, true },
			{ _T("-1234567890123456789.00999999999999999999999999999999"), 123, 123, false }, // Small signed 64-bit value.
			{ _T("-1234567890123456789.00999999999999999999999999999999"), 456, 456, false },

			{ _T("-91234567890123456.78999999999999999999999999999999"), 123, -9123456789012345678LL, true }, // Very small signed 64-bit value.
			{ _T("-91234567890123456.78999999999999999999999999999999"), 456, -9123456789012345678LL, true },
			{ _T("-91234567890123456.00999999999999999999999999999999"), 123, -9123456789012345600LL, true }, // Very small signed 64-bit value.
			{ _T("-91234567890123456.00999999999999999999999999999999"), 456, -9123456789012345600LL, true },
			{ _T("-9123456789012345678.00999999999999999999999999999999"), 123, 123, false }, // Very small signed 64-bit value.
			{ _T("-9123456789012345678.00999999999999999999999999999999"), 456, 456, false },

			{ _T("-92233720368547758.07999999999999999999999999999999"), 123, -9223372036854775807LL, true }, // Minimum signed 64-bit value, plus 1.
			{ _T("-92233720368547758.07999999999999999999999999999999"), 456, -9223372036854775807LL, true },
			{ _T("-92233720368547758.00999999999999999999999999999999"), 123, -9223372036854775800LL, true }, // Minimum signed 64-bit value, plus 1.
			{ _T("-92233720368547758.00999999999999999999999999999999"), 456, -9223372036854775800LL, true },
			{ _T("-9223372036854775807.00999999999999999999999999999999"), 123, 123, false }, // Minimum signed 64-bit value, plus 1.
			{ _T("-9223372036854775807.00999999999999999999999999999999"), 456, 456, false },

			{ _T("-92233720368547758.08999999999999999999999999999999"), 123, (-9223372036854775807LL -1), true }, // Minimum signed 64-bit value.
			{ _T("-92233720368547758.08999999999999999999999999999999"), 456, (-9223372036854775807LL -1), true },
			{ _T("-92233720368547758.00999999999999999999999999999999"), 123, -9223372036854775800LL, true }, // Minimum signed 64-bit value.
			{ _T("-92233720368547758.00999999999999999999999999999999"), 456, -9223372036854775800LL, true },
			{ _T("-9223372036854775808.00999999999999999999999999999999"), 123, 123, false }, // Minimum signed 64-bit value.
			{ _T("-9223372036854775808.00999999999999999999999999999999"), 456, 456, false },

			{ _T("-92233720368547758.09999999999999999999999999999999"), 123, 123, false }, // Minimum signed 64-bit value, less 1.
			{ _T("-92233720368547758.09999999999999999999999999999999"), 456, 456, false },
			{ _T("-92233720368547759.00999999999999999999999999999999"), 123, 123, false }, // Minimum signed 64-bit value, less 1.
			{ _T("-92233720368547759.00999999999999999999999999999999"), 456, 456, false },
			{ _T("-9223372036854775809.00999999999999999999999999999999"), 123, 123, false }, // Minimum signed 64-bit value, less 1.
			{ _T("-9223372036854775809.00999999999999999999999999999999"), 456, 456, false },

			{ _T("-184467440737095516.15999999999999999999999999999999"), 123, 123, false }, // Maximum unsigned 64-bit value, negated.
			{ _T("-184467440737095516.15999999999999999999999999999999"), 456, 456, false },
			{ _T("-184467440737095517.00999999999999999999999999999999"), 123, 123, false }, // Maximum unsigned 64-bit value, negated.
			{ _T("-184467440737095517.00999999999999999999999999999999"), 456, 456, false },
			{ _T("-18446744073709551615.00999999999999999999999999999999"), 123, 123, false }, // Maximum unsigned 64-bit value, negated.
			{ _T("-18446744073709551615.00999999999999999999999999999999"), 456, 456, false },

			{ _T("999999999999999999.99999999999999999999999999999999"), 123, 123, false },
			{ _T("999999999999999999.99999999999999999999999999999999"), 456, 456, false },
			{ _T("999999999999999999.00999999999999999999999999999999"), 123, 123, false },
			{ _T("999999999999999999.00999999999999999999999999999999"), 456, 456, false },
			{ _T("99999999999999999999.00999999999999999999999999999999"), 123, 123, false },
			{ _T("99999999999999999999.00999999999999999999999999999999"), 456, 456, false },

			{ _T("-184467440737095516.15999999999999999999999999999999"), 123, 123, false }, // Maximum unsigned 64-bit value, negated.
			{ _T("-184467440737095516.15999999999999999999999999999999"), 456, 456, false }, // Maximum unsigned 64-bit value, negated.
			{ _T("-184467440737095516.00999999999999999999999999999999"), 123, 123, false }, // Maximum unsigned 64-bit value, negated.
			{ _T("-184467440737095516.00999999999999999999999999999999"), 456, 456, false }, // Maximum unsigned 64-bit value, negated.
			{ _T("-18446744073709551615.00999999999999999999999999999999"), 123, 123, false }, // Maximum unsigned 64-bit value, negated.
			{ _T("-18446744073709551615.00999999999999999999999999999999"), 456, 456, false }, // Maximum unsigned 64-bit value, negated.

			{ _T("-184467440737095516.16999999999999999999999999999999"), 123, 123, false }, // Maximum unsigned 64-bit value plus 1, negated.
			{ _T("-184467440737095516.16999999999999999999999999999999"), 456, 456, false }, // Maximum unsigned 64-bit value plus 1, negated.
			{ _T("-184467440737095516.00999999999999999999999999999999"), 123, 123, false }, // Maximum unsigned 64-bit value plus 1, negated.
			{ _T("-184467440737095516.00999999999999999999999999999999"), 456, 456, false }, // Maximum unsigned 64-bit value plus 1, negated.
			{ _T("-18446744073709551616.00999999999999999999999999999999"), 123, 123, false }, // Maximum unsigned 64-bit value plus 1, negated.
			{ _T("-18446744073709551616.00999999999999999999999999999999"), 456, 456, false }, // Maximum unsigned 64-bit value plus 1, negated.

			{ _T("-999999999999999999.99999999999999999999999999999999"), 123, 123, false },
			{ _T("-999999999999999999.99999999999999999999999999999999"), 456, 456, false },
			{ _T("-999999999999999999.00999999999999999999999999999999"), 123, 123, false },
			{ _T("-999999999999999999.00999999999999999999999999999999"), 456, 456, false },
			{ _T("-99999999999999999999.00999999999999999999999999999999"), 123, 123, false },
			{ _T("-99999999999999999999.00999999999999999999999999999999"), 456, 456, false },

			{ _T("-99999999999999999999999999999999999999.99999999999999999999999999999999"), 123, 123, false },
			{ _T("-99999999999999999999999999999999999999.99999999999999999999999999999999"), 456, 456, false },
			{ _T("-99999999999999999999999999999999999999.00999999999999999999999999999999"), 123, 123, false },
			{ _T("-99999999999999999999999999999999999999.00999999999999999999999999999999"), 456, 456, false },
			{ _T("-9999999999999999999999999999999999999999.00999999999999999999999999999999"), 123, 123, false },
			{ _T("-9999999999999999999999999999999999999999.00999999999999999999999999999999"), 456, 456, false },

			// More content.
			{ _T("012.34999999999999999999999999999999"), 123, 1234, true }, // Test that a value with a leading zero is not treated as a valid octal value.
			{ _T("012.34999999999999999999999999999999"), 456, 1234, true }, // Test that a value with a leading zero is not treated as a valid octal value.
			{ _T("012.00999999999999999999999999999999"), 123, 1200, true }, // Test that a value with a leading zero is not treated as a valid octal value.
			{ _T("012.00999999999999999999999999999999"), 456, 1200, true }, // Test that a value with a leading zero is not treated as a valid octal value.
			{ _T("01234.00999999999999999999999999999999"), 123, 123400, true }, // Test that a value with a leading zero is not treated as a valid octal value.
			{ _T("01234.00999999999999999999999999999999"), 456, 123400, true }, // Test that a value with a leading zero is not treated as a valid octal value.

			{ _T("099.99999999999999999999999999999999"), 123, 9999, true }, // Test that a value with a leading zero is not treated as an invalid octal value.
			{ _T("099.99999999999999999999999999999999"), 456, 9999, true }, // Test that a value with a leading zero is not treated as an invalid octal value.
			{ _T("099.00999999999999999999999999999999"), 123, 9900, true }, // Test that a value with a leading zero is not treated as an invalid octal value.
			{ _T("099.00999999999999999999999999999999"), 456, 9900, true }, // Test that a value with a leading zero is not treated as an invalid octal value.
			{ _T("09999.00999999999999999999999999999999"), 123, 999900, true }, // Test that a value with a leading zero is not treated as an invalid octal value.
			{ _T("09999.00999999999999999999999999999999"), 456, 999900, true }, // Test that a value with a leading zero is not treated as an invalid octal value.
		};

		for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
		{
			// Setup.
			__int64 variable(TEST_DATA[testIndex].m_variable);

			// Perform operation(s) to be tested.
			const bool result = CUtilities::GetInt64FromCurrencyString(TEST_DATA[testIndex].m_stringValue, variable);

			// Test(s).
			std::string resultName;
			resultName = FormatString("GetInt64FromCurrencyString(\"%s\") result", TEST_DATA[testIndex].m_stringValue);
			EXPECT_EQ(result, TEST_DATA[testIndex].m_resultExpected) << resultName;

			resultName = FormatString("GetInt64FromCurrencyString(\"%s\") variable", TEST_DATA[testIndex].m_stringValue);
			EXPECT_EQ(variable, TEST_DATA[testIndex].m_variableExpected) << resultName;
		}
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CUtilities::GetInt64FromCurrencyString(LPCTSTR)
	// Test with strings (mostly) without a decimal point or with 2 digits to the right of the decimal point.
	/// </summary>
	TEST(UtilitiesUnitTests, GetInt64FromCurrencyString1Test)
	{
		typedef struct
		{
			// Input(s).
			const LPCTSTR m_stringValue;

			// Expected result(s).
			const __int64 m_variableExpected;
		} TestDataType;

		const TestDataType TEST_DATA[] =
		{
			// Values that aren't even close.
			{ _T("cd"), 0 },
			{ _T(""), 0 },
			{ _T(" "), 0 },
			{ _T("   "), 0 },

			// Values that are close to valid or valid.
			{ _T("0x9999"), 0 },
			{ _T("9999."), 999900 },
			{ _T("9999.0"), 999900 },
			{ _T("9999.1"), 999910 },
			{ _T("9999.12"), 999912 },
			{ _T("9999e0"), 0 },
			{ _T("+9999"), 999900 },
			{ _T("+ 9999"), 0 },
			{ _T(" +9999"), 999900 },
			{ _T("1001"), 100100 },
			{ _T(" 1002"), 100200 },
			{ _T("1003 "), 0 },
			{ _T(" 1004 "), 0 },
			{ _T("1005\n"), 0 },
			{ _T("\t1006"), 100600 },
			{ _T(" \t 1007"), 100700 },
			{ _T(" \t \t 1008"), 100800 },
			{ _T("\t1009"), 100900 },
			{ _T("\a1010"), 0 },
			{ _T("\b1011"), 0 },
			{ _T("\f1012"), 101200 },
			{ _T("\n1013"), 101300 },
			{ _T("\r1014"), 101400 },
			{ _T("\v1015"), 101500 },
			{ _T(" \t\f\n\r\v \t\f\n\r\v \t\f\n\r\v 1016"), 101600 },

			// Non-negative values that are valid DWORD values and valid "__int64" currency values.
			{ _T("0"), 0 },
			{ _T("1"), 100 },
			{ _T("2"), 200 },
			{ _T("999"), 99900 },
			{ _T("1000"), 100000 },
			{ _T("1001"), 100100 },
			{ _T("12345678.90"), 1234567890 },
			{ _T("21474836.47"), 2147483647 }, // Maximum signed 32-bit value.
			{ _T("42949672.95"), 4294967295 }, // Maximum unsigned 32-bit value.

			{ _T("1234567890"), 123456789000 },
			{ _T("2147483647"), 214748364700 }, // Maximum signed 32-bit value * 100.
			{ _T("4294967295"), 429496729500 }, // Maximum unsigned 32-bit value * 100.

			// Non-negative values that are not valid DWORD values, but are (mostly) valid "__int64" currency values.
			{ _T("42949672.96"), 4294967296LL }, // One more than maximum unsigned 32-bit value.
			{ _T("4294967296"), 429496729600LL }, // One more than maximum unsigned 32-bit value * 100.

			{ _T("99999999.99"), 9999999999LL },
			{ _T("9999999999"), 999999999900LL },

			{ _T("12345678901234567.89"), 1234567890123456789LL }, // Large signed 64-bit value.
			{ _T("1234567890123456789"), 0 }, // Large signed 64-bit value.

			{ _T("91234567890123456.78"), 9123456789012345678LL }, // Very large signed 64-bit value.
			{ _T("91234567890123456"), 9123456789012345600LL }, // Very large signed 64-bit value.
			{ _T("9123456789012345678"), 0 }, // Very large signed 64-bit value.

			{ _T("92233720368547758.07"), 9223372036854775807LL }, // Maximum signed 64-bit value.
			{ _T("92233720368547758"), 9223372036854775800 }, // Maximum signed 64-bit value.
			{ _T("9223372036854775807"), 0 }, // Maximum signed 64-bit value.

			{ _T("92233720368547758.08"), 0 }, // Maximum signed 64-bit value, plus 1.
			{ _T("92233720368547759"), 0 }, // Maximum signed 64-bit value.
			{ _T("9223372036854775808"), 0 }, // Maximum signed 64-bit value, plus 1.

			{ _T("184467440737095516.15"), 0 }, // Maximum unsigned 64-bit value.
			{ _T("18446744073709551615"), 0 }, // Maximum unsigned 64-bit value.

			{ _T("999999999999999999.99"), 0 },
			{ _T("99999999999999999999"), 0 },

			{ _T("99999999999999999999999999999999999999.99"), 0 },
			{ _T("9999999999999999999999999999999999999999"), 0 },

			// Negative values that are valid DWORD values and valid __int64 values.
			{ _T("-0"), 0 },
			{ _T("-1"), -100 },
			{ _T("-2"), -200 },
			{ _T("-999"), -99900 },
			{ _T("-1000"), -100000 },
			{ _T("-1001"), -100100 },

			{ _T("-12345678.90"), -1234567890 },
			{ _T("-21474836.47"), -2147483647 }, // One more than minimum signed 31-bit value.
			{ _T("-21474836.48"), INT_MIN }, // Minimum signed 31-bit value.
			{ _T("-21474836.49"), -2147483649LL }, // One less than maximum unsigned 31-bit value.
			{ _T("-42949662.95"), -4294966295LL }, // Equivalent to 1001 for a DWORD.
			{ _T("-42949662.96"), -4294966296LL }, // Equivalent to 1000 for a DWORD.
			{ _T("-42949662.97"), -4294966297LL }, // Equivalent to 999 for a DWORD.
			{ _T("-42949672.95"), -4294967295LL }, // Maximum unsigned 32-bit value, negated.

			{ _T("-1234567890"), -123456789000 },
			{ _T("-2147483647"), -214748364700 }, // One more than minimum signed 31-bit value.
			{ _T("-2147483648"), INT_MIN * 100LL }, // Minimum signed 31-bit value.
			{ _T("-2147483649"), -214748364900LL }, // One less than maximum unsigned 31-bit value.
			{ _T("-4294966295"), -429496629500LL }, // Equivalent to 1001 for a DWORD.
			{ _T("-4294966296"), -429496629600LL }, // Equivalent to 1000 for a DWORD.
			{ _T("-4294966297"), -429496629700LL }, // Equivalent to 999 for a DWORD.
			{ _T("-4294967295"), -429496729500LL }, // Maximum unsigned 32-bit value, negated.

			// Negative values that are not valid DWORD values, but are (mostly) valid __in64 values.
			{ _T("-42949672.96"), -4294967296LL }, // One more than maximum unsigned 32-bit value, negated.
			{ _T("-4294967296"), -429496729600LL }, // One more than maximum unsigned 32-bit value, negated.

			{ _T("-99999999.99"), -9999999999LL },
			{ _T("-9999999999"), -999999999900LL },

			{ _T("-12345678901234567.89"), -1234567890123456789LL }, // Small signed 64-bit value.
			{ _T("-12345678901234567"), -1234567890123456700LL }, // Small signed 64-bit value.
			{ _T("-1234567890123456789"), 0 }, // Small signed 64-bit value.

			{ _T("-91234567890123456.78"), -9123456789012345678LL }, // Very small signed 64-bit value.
			{ _T("-91234567890123456"), -9123456789012345600LL }, // Very small signed 64-bit value.
			{ _T("-9123456789012345678"), 0 }, // Very small signed 64-bit value.

			{ _T("-92233720368547758.07"), -9223372036854775807LL }, // Minimum signed 64-bit value, plus 1.
			{ _T("-92233720368547758"), -9223372036854775800LL }, // Minimum signed 64-bit value, plus 1.
			{ _T("-9223372036854775807"), 0 }, // Minimum signed 64-bit value, plus 1.

			{ _T("-92233720368547758.08"), (-9223372036854775807LL -1) }, // Minimum signed 64-bit value.
			{ _T("-92233720368547758"), -9223372036854775800LL }, // Minimum signed 64-bit value.
			{ _T("-9223372036854775808"), 0 }, // Minimum signed 64-bit value.

			{ _T("-92233720368547758.09"), 0 }, // Minimum signed 64-bit value, less 1.
			{ _T("-92233720368547759"), 0 }, // Minimum signed 64-bit value, less 1.
			{ _T("-9223372036854775809"), 0 }, // Minimum signed 64-bit value, less 1.

			{ _T("-184467440737095516.15"), 0 }, // Maximum unsigned 64-bit value, negated.
			{ _T("-184467440737095517"), 0 }, // Maximum unsigned 64-bit value, negated.
			{ _T("-18446744073709551615"), 0 }, // Maximum unsigned 64-bit value, negated.

			{ _T("999999999999999999.99"), 0 },
			{ _T("999999999999999999"), 0 },
			{ _T("99999999999999999999"), 0 },

			{ _T("-184467440737095516.15"), 0 }, // Maximum unsigned 64-bit value, negated.
			{ _T("-184467440737095516"), 0 }, // Maximum unsigned 64-bit value, negated.
			{ _T("-18446744073709551615"), 0 }, // Maximum unsigned 64-bit value, negated.

			{ _T("-184467440737095516.16"), 0 }, // Maximum unsigned 64-bit value plus 1, negated.
			{ _T("-184467440737095516"), 0 }, // Maximum unsigned 64-bit value plus 1, negated.
			{ _T("-18446744073709551616"), 0 }, // Maximum unsigned 64-bit value plus 1, negated.

			{ _T("-999999999999999999.99"), 0 },
			{ _T("-999999999999999999"), 0 },
			{ _T("-99999999999999999999"), 0 },

			{ _T("-99999999999999999999999999999999999999.99"), 0 },
			{ _T("-99999999999999999999999999999999999999"), 0 },
			{ _T("-9999999999999999999999999999999999999999"), 0 },

			// More content.
			{ _T("012.34"), 1234 }, // Test that a value with a leading zero is not treated as a valid octal value.
			{ _T("012"), 1200 }, // Test that a value with a leading zero is not treated as a valid octal value.
			{ _T("01234"), 123400 }, // Test that a value with a leading zero is not treated as a valid octal value.

			{ _T("099.99"), 9999 }, // Test that a value with a leading zero is not treated as an invalid octal value.
			{ _T("099"), 9900 }, // Test that a value with a leading zero is not treated as an invalid octal value.
			{ _T("09999"), 999900 }, // Test that a value with a leading zero is not treated as an invalid octal value.
		};

		for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
		{
			// Setup.

			// Perform operation(s) to be tested.
			const __int64 variable = CUtilities::GetInt64FromCurrencyString(TEST_DATA[testIndex].m_stringValue);

			// Test(s).
			std::string resultName;
			resultName = FormatString(_T("GetInt64FromCurrencyString(\"%s\") variable"),
				static_cast<LPCTSTR>(TEST_DATA[testIndex].m_stringValue));
			EXPECT_EQ(variable, TEST_DATA[testIndex].m_variableExpected) << resultName;
		}
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CUtilities::GetInt64FromCurrencyString(LPCTSTR)
	// Test with strings with a decimal point.
	/// </summary>
	TEST(UtilitiesUnitTests, GetInt64FromCurrencyString2Test)
	{
		typedef struct
		{
			// Input(s).
			const LPCTSTR m_stringValue;

			// Expected result(s).
			const __int64 m_variableExpected;
		} TestDataType;

		const TestDataType TEST_DATA[] =
		{
			// Values that aren't even close.
			{ _T("12345.cd"), 0 },

			// Values that are close to valid or valid.
			{ _T("12345."), 1234500 },
			{ _T("12345. "), 0 },
			{ _T("12345.   "), 0 },
			{ _T("9999."), 999900 },
			{ _T("9999.0"), 999900 },
			{ _T("9999.0a"), 0 },
			{ _T("9999.00a"), 0 },
			{ _T("9999.000a"), 0 },
			{ _T("9999.0000a"), 0 },
			{ _T("9999.000000000000000000000000000000a"), 0 },
			{ _T("9999.000000000000000000000000000000"), 999900 },
			{ _T("9999.0a000000000000000000000000000000"), 0 },
			{ _T("9999.00a000000000000000000000000000000"), 0 },
			{ _T("9999.000a000000000000000000000000000000"), 0 },
			{ _T("9999.0000a000000000000000000000000000000"), 0 },
			{ _T("9999.000000000000000000000000000000a000000000000000000000000000000"), 0 },
			{ _T("9999. 000000000000000000000000000000"), 0 },
			{ _T("9999.0 000000000000000000000000000000"), 0 },
			{ _T("9999.00 000000000000000000000000000000"), 0 },
			{ _T("9999.000 000000000000000000000000000000"), 0 },
			{ _T("9999.0000 000000000000000000000000000000"), 0 },
			{ _T("9999.000000000000000000000000000000 000000000000000000000000000000"), 0 },
			{ _T("9999.9"), 999990 },
			{ _T("9999.00"), 999900 },
			{ _T("9999.01"), 999901 },
			{ _T("9999.10"), 999910 },
			{ _T("9999.90"), 999990 },
			{ _T("9999.99"), 999999 },
			{ _T("9999.000"), 999900 },
			{ _T("9999.009"), 999900 },
			{ _T("9999.990"), 999999 },
			{ _T("9999.999"), 999999 },
			{ _T("9999.999999999999999999999999999999"), 999999 },
			{ _T("9999.e0"), 0 },
			{ _T("9999.0e0"), 0 },
			{ _T("12345. 00"), 0 },
			{ _T("12345.+00"), 0 },
			{ _T("12345.-00"), 0 },
			{ _T("12345.+99"), 0 },
			{ _T("12345.-99"), 0 },
			{ _T("12345.99-"), 0 },
			{ _T("12345.99+"), 0 },
			{ _T("1001."), 100100 },
			{ _T(" 1002."), 100200 },
			{ _T("1003. "), 0 },
			{ _T("1004.\a"), 0 },
			{ _T("1005.\b"), 0 },
			{ _T("1006\f"), 0 },
			{ _T("1007\n"), 0 },
			{ _T("1008\r"), 0 },
			{ _T("1009\t"), 0 },
			{ _T("1010\v"), 0 },
			{ _T("\b1011"), 0 },
			{ _T("\f1012"), 101200 },
			{ _T("\n1013"), 101300 },
			{ _T("\r1014"), 101400 },
			{ _T("\v1015"), 101500 },
			{ _T(" \t\f\n\r\v \t\f\n\r\v \t\f\n\r\v 1016"), 101600 },

			// Non-negative values that are valid DWORD values and valid "__int64" currency values.
			{ _T("0.00999999999999999999999999999999"), 0 },
			{ _T("1.00999999999999999999999999999999"), 100 },
			{ _T("2.00999999999999999999999999999999"), 200 },
			{ _T("999.00999999999999999999999999999999"), 99900 },
			{ _T("1000.00999999999999999999999999999999"), 100000 },
			{ _T("1001.00999999999999999999999999999999"), 100100 },
			{ _T("12345678.90999999999999999999999999999999"), 1234567890 },
			{ _T("21474836.47999999999999999999999999999999"), 2147483647 }, // Maximum signed 32-bit value.
			{ _T("42949672.95999999999999999999999999999999"), 4294967295 }, // Maximum unsigned 32-bit value.

			{ _T("1234567890.00999999999999999999999999999999"), 123456789000 },
			{ _T("2147483647.00999999999999999999999999999999"), 214748364700 }, // Maximum signed 32-bit value * 100.
			{ _T("4294967295.00999999999999999999999999999999"), 429496729500 }, // Maximum unsigned 32-bit value * 100.

			// Non-negative values that are not valid DWORD values, but are (mostly) valid "__int64" currency values.
			{ _T("42949672.96999999999999999999999999999999"), 4294967296LL }, // One more than maximum unsigned 32-bit value.
			{ _T("4294967296.00999999999999999999999999999999"), 429496729600LL }, // One more than maximum unsigned 32-bit value * 100.

			{ _T("99999999.99999999999999999999999999999999"), 9999999999LL },
			{ _T("9999999999.00999999999999999999999999999999"), 999999999900LL },

			{ _T("12345678901234567.89999999999999999999999999999999"), 1234567890123456789LL }, // Large signed 64-bit value.
			{ _T("1234567890123456789.00999999999999999999999999999999"), 0 }, // Large signed 64-bit value.

			{ _T("91234567890123456.78999999999999999999999999999999"), 9123456789012345678LL }, // Very large signed 64-bit value.
			{ _T("91234567890123456.00999999999999999999999999999999"), 9123456789012345600LL }, // Very large signed 64-bit value.
			{ _T("9123456789012345678.00999999999999999999999999999999"), 0 }, // Very large signed 64-bit value.

			{ _T("92233720368547758.07999999999999999999999999999999"), 9223372036854775807LL }, // Maximum signed 64-bit value.
			{ _T("92233720368547758.00999999999999999999999999999999"), 9223372036854775800 }, // Maximum signed 64-bit value.
			{ _T("9223372036854775807.00999999999999999999999999999999"), 0 }, // Maximum signed 64-bit value.

			{ _T("92233720368547758.08999999999999999999999999999999"), 0 }, // Maximum signed 64-bit value, plus 1.
			{ _T("92233720368547759.00999999999999999999999999999999"), 0 }, // Maximum signed 64-bit value.
			{ _T("9223372036854775808.00999999999999999999999999999999"), 0 }, // Maximum signed 64-bit value, plus 1.

			{ _T("184467440737095516.15999999999999999999999999999999"), 0 }, // Maximum unsigned 64-bit value.
			{ _T("18446744073709551615.00999999999999999999999999999999"), 0 }, // Maximum unsigned 64-bit value.

			{ _T("999999999999999999.99999999999999999999999999999999"), 0 },
			{ _T("99999999999999999999.00999999999999999999999999999999"), 0 },

			{ _T("99999999999999999999999999999999999999.99999999999999999999999999999999"), 0 },
			{ _T("9999999999999999999999999999999999999999.00999999999999999999999999999999"), 0 },

			// Negative values that are valid DWORD values and valid __int64 values.
			{ _T("-0.00999999999999999999999999999999"), 0 },
			{ _T("-1.00999999999999999999999999999999"), -100 },
			{ _T("-2.00999999999999999999999999999999"), -200 },
			{ _T("-999.00999999999999999999999999999999"), -99900 },
			{ _T("-1000.00999999999999999999999999999999"), -100000 },
			{ _T("-1001.00999999999999999999999999999999"), -100100 },

			{ _T("-12345678.90999999999999999999999999999999"), -1234567890 },
			{ _T("-21474836.47999999999999999999999999999999"), -2147483647 }, // One more than minimum signed 31-bit value.
			{ _T("-21474836.48999999999999999999999999999999"), INT_MIN }, // Minimum signed 31-bit value.
			{ _T("-21474836.49999999999999999999999999999999"), -2147483649LL }, // One less than maximum unsigned 31-bit value.
			{ _T("-42949662.95999999999999999999999999999999"), -4294966295LL }, // Equivalent to 1001 for a DWORD.
			{ _T("-42949662.96999999999999999999999999999999"), -4294966296LL }, // Equivalent to 1000 for a DWORD.
			{ _T("-42949662.97999999999999999999999999999999"), -4294966297LL }, // Equivalent to 999 for a DWORD.
			{ _T("-42949672.95999999999999999999999999999999"), -4294967295LL }, // Maximum unsigned 32-bit value, negated.

			{ _T("-1234567890.00999999999999999999999999999999"), -123456789000 },
			{ _T("-2147483647.00999999999999999999999999999999"), -214748364700 }, // One more than minimum signed 31-bit value.
			{ _T("-2147483648.00999999999999999999999999999999"), INT_MIN * 100LL }, // Minimum signed 31-bit value.
			{ _T("-2147483649.00999999999999999999999999999999"), -214748364900LL }, // One less than maximum unsigned 31-bit value.
			{ _T("-4294966295.00999999999999999999999999999999"), -429496629500LL }, // Equivalent to 1001 for a DWORD.
			{ _T("-4294966296.00999999999999999999999999999999"), -429496629600LL }, // Equivalent to 1000 for a DWORD.
			{ _T("-4294966297.00999999999999999999999999999999"), -429496629700LL }, // Equivalent to 999 for a DWORD.
			{ _T("-4294967295.00999999999999999999999999999999"), -429496729500LL }, // Maximum unsigned 32-bit value, negated.

			// Negative values that are not valid DWORD values, but are (mostly) valid __in64 values.
			{ _T("-42949672.96999999999999999999999999999999"), -4294967296LL }, // One more than maximum unsigned 32-bit value, negated.
			{ _T("-4294967296.00999999999999999999999999999999"), -429496729600LL }, // One more than maximum unsigned 32-bit value, negated.

			{ _T("-99999999.99999999999999999999999999999999"), -9999999999LL },
			{ _T("-9999999999.00999999999999999999999999999999"), -999999999900LL },

			{ _T("-12345678901234567.89999999999999999999999999999999"), -1234567890123456789LL }, // Small signed 64-bit value.
			{ _T("-12345678901234567.00999999999999999999999999999999"), -1234567890123456700LL }, // Small signed 64-bit value.
			{ _T("-1234567890123456789.00999999999999999999999999999999"), 0 }, // Small signed 64-bit value.

			{ _T("-91234567890123456.78999999999999999999999999999999"), -9123456789012345678LL }, // Very small signed 64-bit value.
			{ _T("-91234567890123456.00999999999999999999999999999999"), -9123456789012345600LL }, // Very small signed 64-bit value.
			{ _T("-9123456789012345678.00999999999999999999999999999999"), 0 }, // Very small signed 64-bit value.

			{ _T("-92233720368547758.07999999999999999999999999999999"), -9223372036854775807LL }, // Minimum signed 64-bit value, plus 1.
			{ _T("-92233720368547758.00999999999999999999999999999999"), -9223372036854775800LL }, // Minimum signed 64-bit value, plus 1.
			{ _T("-9223372036854775807.00999999999999999999999999999999"), 0 }, // Minimum signed 64-bit value, plus 1.

			{ _T("-92233720368547758.08999999999999999999999999999999"),(-9223372036854775807LL -1) }, // Minimum signed 64-bit value.
			{ _T("-92233720368547758.00999999999999999999999999999999"), -9223372036854775800LL }, // Minimum signed 64-bit value.
			{ _T("-9223372036854775808.00999999999999999999999999999999"), 0 }, // Minimum signed 64-bit value.

			{ _T("-92233720368547758.09999999999999999999999999999999"), 0 }, // Minimum signed 64-bit value, less 1.
			{ _T("-92233720368547759.00999999999999999999999999999999"), 0 }, // Minimum signed 64-bit value, less 1.
			{ _T("-9223372036854775809.00999999999999999999999999999999"), 0 }, // Minimum signed 64-bit value, less 1.

			{ _T("-184467440737095516.15999999999999999999999999999999"), 0 }, // Maximum unsigned 64-bit value, negated.
			{ _T("-184467440737095517.00999999999999999999999999999999"), 0 }, // Maximum unsigned 64-bit value, negated.
			{ _T("-18446744073709551615.00999999999999999999999999999999"), 0 }, // Maximum unsigned 64-bit value, negated.

			{ _T("999999999999999999.99999999999999999999999999999999"), 0 },
			{ _T("999999999999999999.00999999999999999999999999999999"), 0 },
			{ _T("99999999999999999999.00999999999999999999999999999999"), 0 },

			{ _T("-184467440737095516.15999999999999999999999999999999"), 0 }, // Maximum unsigned 64-bit value, negated.
			{ _T("-184467440737095516.00999999999999999999999999999999"), 0 }, // Maximum unsigned 64-bit value, negated.
			{ _T("-18446744073709551615.00999999999999999999999999999999"), 0 }, // Maximum unsigned 64-bit value, negated.

			{ _T("-184467440737095516.16999999999999999999999999999999"), 0 }, // Maximum unsigned 64-bit value plus 1, negated.
			{ _T("-184467440737095516.00999999999999999999999999999999"), 0 }, // Maximum unsigned 64-bit value plus 1, negated.
			{ _T("-18446744073709551616.00999999999999999999999999999999"), 0 }, // Maximum unsigned 64-bit value plus 1, negated.

			{ _T("-999999999999999999.99999999999999999999999999999999"), 0 },
			{ _T("-999999999999999999.00999999999999999999999999999999"), 0 },
			{ _T("-99999999999999999999.00999999999999999999999999999999"), 0 },

			{ _T("-99999999999999999999999999999999999999.99999999999999999999999999999999"), 0 },
			{ _T("-99999999999999999999999999999999999999.00999999999999999999999999999999"), 0 },
			{ _T("-9999999999999999999999999999999999999999.00999999999999999999999999999999"), 0 },

			// More content.
			{ _T("012.34999999999999999999999999999999"), 1234 }, // Test that a value with a leading zero is not treated as a valid octal value.
			{ _T("012.00999999999999999999999999999999"), 1200 }, // Test that a value with a leading zero is not treated as a valid octal value.
			{ _T("01234.00999999999999999999999999999999"), 123400 }, // Test that a value with a leading zero is not treated as a valid octal value.

			{ _T("099.99999999999999999999999999999999"), 9999 }, // Test that a value with a leading zero is not treated as an invalid octal value.
			{ _T("099.00999999999999999999999999999999"), 9900 }, // Test that a value with a leading zero is not treated as an invalid octal value.
			{ _T("09999.00999999999999999999999999999999"), 999900 }, // Test that a value with a leading zero is not treated as an invalid octal value.
		};

		for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
		{
			// Setup.
			// Perform operation(s) to be tested.
			const __int64 variable = CUtilities::GetInt64FromCurrencyString(TEST_DATA[testIndex].m_stringValue);

			// Test(s).
			std::string resultName;
			resultName = FormatString(_T("GetInt64FromCurrencyString(\"%s\") variable"),
				static_cast<LPCTSTR>(TEST_DATA[testIndex].m_stringValue));
			EXPECT_EQ(variable, TEST_DATA[testIndex].m_variableExpected) << resultName;
		}
	}


	/// <summary>
	/// Code coverage and functional test for:
	///     CUtilities::GetInt64FromCurrencyString(const std::string&)
	// Test with strings (mostly) without a decimal point or with 2 digits to the right of the decimal point.
	/// </summary>
	TEST(UtilitiesUnitTests, GetInt64FromCurrencyString1_StdTest)
	{
		typedef struct
		{
			// Input(s).
			const std::string m_stringValue;

			// Expected result(s).
			const __int64 m_variableExpected;
		} TestDataType;

		const TestDataType TEST_DATA[] =
		{
			// Values that aren't even close.
			{ "cd", 0 },
			{ "", 0 },
			{ " ", 0 },
			{ "   ", 0 },

			// Values that are close to valid or valid.
			{ "0x9999", 0 },
			{ "9999.", 999900 },
			{ "9999.0", 999900 },
			{ "9999.1", 999910 },
			{ "9999.12", 999912 },
			{ "9999e0", 0 },
			{ "+9999", 999900 },
			{ "+ 9999", 0 },
			{ " +9999", 999900 },
			{ "1001", 100100 },
			{ " 1002", 100200 },
			{ "1003 ", 0 },
			{ " 1004 ", 0 },
			{ "1005\n", 0 },
			{ "\t1006", 100600 },
			{ " \t 1007", 100700 },
			{ " \t \t 1008", 100800 },
			{ "\t1009", 100900 },
			{ "\a1010", 0 },
			{ "\b1011", 0 },
			{ "\f1012", 101200 },
			{ "\n1013", 101300 },
			{ "\r1014", 101400 },
			{ "\v1015", 101500 },
			{ " \t\f\n\r\v \t\f\n\r\v \t\f\n\r\v 1016", 101600 },

			// Non-negative values that are valid DWORD values and valid "__int64" currency values.
			{ "0", 0 },
			{ "1", 100 },
			{ "2", 200 },
			{ "999", 99900 },
			{ "1000", 100000 },
			{ "1001", 100100 },
			{ "12345678.90", 1234567890 },
			{ "21474836.47", 2147483647 }, // Maximum signed 32-bit value.
			{ "42949672.95", 4294967295 }, // Maximum unsigned 32-bit value.

			{ "1234567890", 123456789000 },
			{ "2147483647", 214748364700 }, // Maximum signed 32-bit value * 100.
			{ "4294967295", 429496729500 }, // Maximum unsigned 32-bit value * 100.

			// Non-negative values that are not valid DWORD values, but are (mostly) valid "__int64" currency values.
			{ "42949672.96", 4294967296LL }, // One more than maximum unsigned 32-bit value.
			{ "4294967296", 429496729600LL }, // One more than maximum unsigned 32-bit value * 100.

			{ "99999999.99", 9999999999LL },
			{ "9999999999", 999999999900LL },

			{ "12345678901234567.89", 1234567890123456789LL }, // Large signed 64-bit value.
			{ "1234567890123456789", 0 }, // Large signed 64-bit value.

			{ "91234567890123456.78", 9123456789012345678LL }, // Very large signed 64-bit value.
			{ "91234567890123456", 9123456789012345600LL }, // Very large signed 64-bit value.
			{ "9123456789012345678", 0 }, // Very large signed 64-bit value.

			{ "92233720368547758.07", 9223372036854775807LL }, // Maximum signed 64-bit value.
			{ "92233720368547758", 9223372036854775800 }, // Maximum signed 64-bit value.
			{ "9223372036854775807", 0 }, // Maximum signed 64-bit value.

			{ "92233720368547758.08", 0 }, // Maximum signed 64-bit value, plus 1.
			{ "92233720368547759", 0 }, // Maximum signed 64-bit value.
			{ "9223372036854775808", 0 }, // Maximum signed 64-bit value, plus 1.

			{ "184467440737095516.15", 0 }, // Maximum unsigned 64-bit value.
			{ "18446744073709551615", 0 }, // Maximum unsigned 64-bit value.

			{ "999999999999999999.99", 0 },
			{ "99999999999999999999", 0 },

			{ "99999999999999999999999999999999999999.99", 0 },
			{ "9999999999999999999999999999999999999999", 0 },

			// Negative values that are valid DWORD values and valid __int64 values.
			{ "-0", 0 },
			{ "-1", -100 },
			{ "-2", -200 },
			{ "-999", -99900 },
			{ "-1000", -100000 },
			{ "-1001", -100100 },

			{ "-12345678.90", -1234567890 },
			{ "-21474836.47", -2147483647 }, // One more than minimum signed 31-bit value.
			{ "-21474836.48", INT_MIN }, // Minimum signed 31-bit value.
			{ "-21474836.49", -2147483649LL }, // One less than maximum unsigned 31-bit value.
			{ "-42949662.95", -4294966295LL }, // Equivalent to 1001 for a DWORD.
			{ "-42949662.96", -4294966296LL }, // Equivalent to 1000 for a DWORD.
			{ "-42949662.97", -4294966297LL }, // Equivalent to 999 for a DWORD.
			{ "-42949672.95", -4294967295LL }, // Maximum unsigned 32-bit value, negated.

			{ "-1234567890", -123456789000 },
			{ "-2147483647", -214748364700 }, // One more than minimum signed 31-bit value.
			{ "-2147483648", INT_MIN * 100LL }, // Minimum signed 31-bit value.
			{ "-2147483649", -214748364900LL }, // One less than maximum unsigned 31-bit value.
			{ "-4294966295", -429496629500LL }, // Equivalent to 1001 for a DWORD.
			{ "-4294966296", -429496629600LL }, // Equivalent to 1000 for a DWORD.
			{ "-4294966297", -429496629700LL }, // Equivalent to 999 for a DWORD.
			{ "-4294967295", -429496729500LL }, // Maximum unsigned 32-bit value, negated.

			// Negative values that are not valid DWORD values, but are (mostly) valid __in64 values.
			{ "-42949672.96", -4294967296LL }, // One more than maximum unsigned 32-bit value, negated.
			{ "-4294967296", -429496729600LL }, // One more than maximum unsigned 32-bit value, negated.

			{ "-99999999.99", -9999999999LL },
			{ "-9999999999", -999999999900LL },

			{ "-12345678901234567.89", -1234567890123456789LL }, // Small signed 64-bit value.
			{ "-12345678901234567", -1234567890123456700LL }, // Small signed 64-bit value.
			{ "-1234567890123456789", 0 }, // Small signed 64-bit value.

			{ "-91234567890123456.78", -9123456789012345678LL }, // Very small signed 64-bit value.
			{ "-91234567890123456", -9123456789012345600LL }, // Very small signed 64-bit value.
			{ "-9123456789012345678", 0 }, // Very small signed 64-bit value.

			{ "-92233720368547758.07", -9223372036854775807LL }, // Minimum signed 64-bit value, plus 1.
			{ "-92233720368547758", -9223372036854775800LL }, // Minimum signed 64-bit value, plus 1.
			{ "-9223372036854775807", 0 }, // Minimum signed 64-bit value, plus 1.

			{ "-92233720368547758.08", -9223372036854775808LL }, // Minimum signed 64-bit value.
			{ "-92233720368547758", -9223372036854775800LL }, // Minimum signed 64-bit value.
			{ "-9223372036854775808", 0 }, // Minimum signed 64-bit value.

			{ "-92233720368547758.09", 0 }, // Minimum signed 64-bit value, less 1.
			{ "-92233720368547759", 0 }, // Minimum signed 64-bit value, less 1.
			{ "-9223372036854775809", 0 }, // Minimum signed 64-bit value, less 1.

			{ "-184467440737095516.15", 0 }, // Maximum unsigned 64-bit value, negated.
			{ "-184467440737095517", 0 }, // Maximum unsigned 64-bit value, negated.
			{ "-18446744073709551615", 0 }, // Maximum unsigned 64-bit value, negated.

			{ "999999999999999999.99", 0 },
			{ "999999999999999999", 0 },
			{ "99999999999999999999", 0 },

			{ "-184467440737095516.15", 0 }, // Maximum unsigned 64-bit value, negated.
			{ "-184467440737095516", 0 }, // Maximum unsigned 64-bit value, negated.
			{ "-18446744073709551615", 0 }, // Maximum unsigned 64-bit value, negated.

			{ "-184467440737095516.16", 0 }, // Maximum unsigned 64-bit value plus 1, negated.
			{ "-184467440737095516", 0 }, // Maximum unsigned 64-bit value plus 1, negated.
			{ "-18446744073709551616", 0 }, // Maximum unsigned 64-bit value plus 1, negated.

			{ "-999999999999999999.99", 0 },
			{ "-999999999999999999", 0 },
			{ "-99999999999999999999", 0 },

			{ "-99999999999999999999999999999999999999.99", 0 },
			{ "-99999999999999999999999999999999999999", 0 },
			{ "-9999999999999999999999999999999999999999", 0 },

			// More content.
			{ "012.34", 1234 }, // Test that a value with a leading zero is not treated as a valid octal value.
			{ "012", 1200 }, // Test that a value with a leading zero is not treated as a valid octal value.
			{ "01234", 123400 }, // Test that a value with a leading zero is not treated as a valid octal value.

			{ "099.99", 9999 }, // Test that a value with a leading zero is not treated as an invalid octal value.
			{ "099", 9900 }, // Test that a value with a leading zero is not treated as an invalid octal value.
			{ "09999", 999900 }, // Test that a value with a leading zero is not treated as an invalid octal value.
		};

		for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
		{
			// Setup.

			// Perform operation(s) to be tested.
			const __int64 variable = CUtilities::GetInt64FromCurrencyString(TEST_DATA[testIndex].m_stringValue);

			// Test(s).
			std::string resultName;
			resultName = FormatString(_T("GetInt64FromCurrencyString(\"%s\") variable"),
				TEST_DATA[testIndex].m_stringValue.c_str());
			ASSERT_TRUE(variable == TEST_DATA[testIndex].m_variableExpected) << resultName;
		}
	}


	/// <summary>
	/// Code coverage and functional test for:
	///     CUtilities::GetInt64FromCurrencyString(const std::string&)
	// Test with strings with a decimal point.
	/// </summary>
	TEST(UtilitiesUnitTests, GetInt64FromCurrencyString2_StdTest)
	{
		typedef struct
		{
			// Input(s).
			const std::string m_stringValue;

			// Expected result(s).
			const __int64 m_variableExpected;
		} TestDataType;

		const TestDataType TEST_DATA[] =
		{
			// Values that aren't even close.
			{ "12345.cd", 0 },

			// Values that are close to valid or valid.
			{ "12345.", 1234500 },
			{ "12345. ", 0 },
			{ "12345.   ", 0 },
			{ "9999.", 999900 },
			{ "9999.0", 999900 },
			{ "9999.0a", 0 },
			{ "9999.00a", 0 },
			{ "9999.000a", 0 },
			{ "9999.0000a", 0 },
			{ "9999.000000000000000000000000000000a", 0 },
			{ "9999.000000000000000000000000000000", 999900 },
			{ "9999.0a000000000000000000000000000000", 0 },
			{ "9999.00a000000000000000000000000000000", 0 },
			{ "9999.000a000000000000000000000000000000", 0 },
			{ "9999.0000a000000000000000000000000000000", 0 },
			{ "9999.000000000000000000000000000000a000000000000000000000000000000", 0 },
			{ "9999. 000000000000000000000000000000", 0 },
			{ "9999.0 000000000000000000000000000000", 0 },
			{ "9999.00 000000000000000000000000000000", 0 },
			{ "9999.000 000000000000000000000000000000", 0 },
			{ "9999.0000 000000000000000000000000000000", 0 },
			{ "9999.000000000000000000000000000000 000000000000000000000000000000", 0 },
			{ "9999.9", 999990 },
			{ "9999.00", 999900 },
			{ "9999.01", 999901 },
			{ "9999.10", 999910 },
			{ "9999.90", 999990 },
			{ "9999.99", 999999 },
			{ "9999.000", 999900 },
			{ "9999.009", 999900 },
			{ "9999.990", 999999 },
			{ "9999.999", 999999 },
			{ "9999.999999999999999999999999999999", 999999 },
			{ "9999.e0", 0 },
			{ "9999.0e0", 0 },
			{ "12345. 00", 0 },
			{ "12345.+00", 0 },
			{ "12345.-00", 0 },
			{ "12345.+99", 0 },
			{ "12345.-99", 0 },
			{ "12345.99-", 0 },
			{ "12345.99+", 0 },
			{ "1001.", 100100 },
			{ " 1002.", 100200 },
			{ "1003. ", 0 },
			{ "1004.\a", 0 },
			{ "1005.\b", 0 },
			{ "1006\f", 0 },
			{ "1007\n", 0 },
			{ "1008\r", 0 },
			{ "1009\t", 0 },
			{ "1010\v", 0 },
			{ "\b1011", 0 },
			{ "\f1012", 101200 },
			{ "\n1013", 101300 },
			{ "\r1014", 101400 },
			{ "\v1015", 101500 },
			{ " \t\f\n\r\v \t\f\n\r\v \t\f\n\r\v 1016", 101600 },

			// Non-negative values that are valid DWORD values and valid "__int64" currency values.
			{ "0.00999999999999999999999999999999", 0 },
			{ "1.00999999999999999999999999999999", 100 },
			{ "2.00999999999999999999999999999999", 200 },
			{ "999.00999999999999999999999999999999", 99900 },
			{ "1000.00999999999999999999999999999999", 100000 },
			{ "1001.00999999999999999999999999999999", 100100 },
			{ "12345678.90999999999999999999999999999999", 1234567890 },
			{ "21474836.47999999999999999999999999999999", 2147483647 }, // Maximum signed 32-bit value.
			{ "42949672.95999999999999999999999999999999", 4294967295 }, // Maximum unsigned 32-bit value.

			{ "1234567890.00999999999999999999999999999999", 123456789000 },
			{ "2147483647.00999999999999999999999999999999", 214748364700 }, // Maximum signed 32-bit value * 100.
			{ "4294967295.00999999999999999999999999999999", 429496729500 }, // Maximum unsigned 32-bit value * 100.

			// Non-negative values that are not valid DWORD values, but are (mostly) valid "__int64" currency values.
			{ "42949672.96999999999999999999999999999999", 4294967296LL }, // One more than maximum unsigned 32-bit value.
			{ "4294967296.00999999999999999999999999999999", 429496729600LL }, // One more than maximum unsigned 32-bit value * 100.

			{ "99999999.99999999999999999999999999999999", 9999999999LL },
			{ "9999999999.00999999999999999999999999999999", 999999999900LL },

			{ "12345678901234567.89999999999999999999999999999999", 1234567890123456789LL }, // Large signed 64-bit value.
			{ "1234567890123456789.00999999999999999999999999999999", 0 }, // Large signed 64-bit value.

			{ "91234567890123456.78999999999999999999999999999999", 9123456789012345678LL }, // Very large signed 64-bit value.
			{ "91234567890123456.00999999999999999999999999999999", 9123456789012345600LL }, // Very large signed 64-bit value.
			{ "9123456789012345678.00999999999999999999999999999999", 0 }, // Very large signed 64-bit value.

			{ "92233720368547758.07999999999999999999999999999999", 9223372036854775807LL }, // Maximum signed 64-bit value.
			{ "92233720368547758.00999999999999999999999999999999", 9223372036854775800 }, // Maximum signed 64-bit value.
			{ "9223372036854775807.00999999999999999999999999999999", 0 }, // Maximum signed 64-bit value.

			{ "92233720368547758.08999999999999999999999999999999", 0 }, // Maximum signed 64-bit value, plus 1.
			{ "92233720368547759.00999999999999999999999999999999", 0 }, // Maximum signed 64-bit value.
			{ "9223372036854775808.00999999999999999999999999999999", 0 }, // Maximum signed 64-bit value, plus 1.

			{ "184467440737095516.15999999999999999999999999999999", 0 }, // Maximum unsigned 64-bit value.
			{ "18446744073709551615.00999999999999999999999999999999", 0 }, // Maximum unsigned 64-bit value.

			{ "999999999999999999.99999999999999999999999999999999", 0 },
			{ "99999999999999999999.00999999999999999999999999999999", 0 },

			{ "99999999999999999999999999999999999999.99999999999999999999999999999999", 0 },
			{ "9999999999999999999999999999999999999999.00999999999999999999999999999999", 0 },

			// Negative values that are valid DWORD values and valid __int64 values.
			{ "-0.00999999999999999999999999999999", 0 },
			{ "-1.00999999999999999999999999999999", -100 },
			{ "-2.00999999999999999999999999999999", -200 },
			{ "-999.00999999999999999999999999999999", -99900 },
			{ "-1000.00999999999999999999999999999999", -100000 },
			{ "-1001.00999999999999999999999999999999", -100100 },

			{ "-12345678.90999999999999999999999999999999", -1234567890 },
			{ "-21474836.47999999999999999999999999999999", -2147483647 }, // One more than minimum signed 31-bit value.
			{ "-21474836.48999999999999999999999999999999", INT_MIN }, // Minimum signed 31-bit value.
			{ "-21474836.49999999999999999999999999999999", -2147483649LL }, // One less than maximum unsigned 31-bit value.
			{ "-42949662.95999999999999999999999999999999", -4294966295LL }, // Equivalent to 1001 for a DWORD.
			{ "-42949662.96999999999999999999999999999999", -4294966296LL }, // Equivalent to 1000 for a DWORD.
			{ "-42949662.97999999999999999999999999999999", -4294966297LL }, // Equivalent to 999 for a DWORD.
			{ "-42949672.95999999999999999999999999999999", -4294967295LL }, // Maximum unsigned 32-bit value, negated.

			{ "-1234567890.00999999999999999999999999999999", -123456789000 },
			{ "-2147483647.00999999999999999999999999999999", -214748364700 }, // One more than minimum signed 31-bit value.
			{ "-2147483648.00999999999999999999999999999999", INT_MIN * 100LL }, // Minimum signed 31-bit value.
			{ "-2147483649.00999999999999999999999999999999", -214748364900LL }, // One less than maximum unsigned 31-bit value.
			{ "-4294966295.00999999999999999999999999999999", -429496629500LL }, // Equivalent to 1001 for a DWORD.
			{ "-4294966296.00999999999999999999999999999999", -429496629600LL }, // Equivalent to 1000 for a DWORD.
			{ "-4294966297.00999999999999999999999999999999", -429496629700LL }, // Equivalent to 999 for a DWORD.
			{ "-4294967295.00999999999999999999999999999999", -429496729500LL }, // Maximum unsigned 32-bit value, negated.

			// Negative values that are not valid DWORD values, but are (mostly) valid __in64 values.
			{ "-42949672.96999999999999999999999999999999", -4294967296LL }, // One more than maximum unsigned 32-bit value, negated.
			{ "-4294967296.00999999999999999999999999999999", -429496729600LL }, // One more than maximum unsigned 32-bit value, negated.

			{ "-99999999.99999999999999999999999999999999", -9999999999LL },
			{ "-9999999999.00999999999999999999999999999999", -999999999900LL },

			{ "-12345678901234567.89999999999999999999999999999999", -1234567890123456789LL }, // Small signed 64-bit value.
			{ "-12345678901234567.00999999999999999999999999999999", -1234567890123456700LL }, // Small signed 64-bit value.
			{ "-1234567890123456789.00999999999999999999999999999999", 0 }, // Small signed 64-bit value.

			{ "-91234567890123456.78999999999999999999999999999999", -9123456789012345678LL }, // Very small signed 64-bit value.
			{ "-91234567890123456.00999999999999999999999999999999", -9123456789012345600LL }, // Very small signed 64-bit value.
			{ "-9123456789012345678.00999999999999999999999999999999", 0 }, // Very small signed 64-bit value.

			{ "-92233720368547758.07999999999999999999999999999999", -9223372036854775807LL }, // Minimum signed 64-bit value, plus 1.
			{ "-92233720368547758.00999999999999999999999999999999", -9223372036854775800LL }, // Minimum signed 64-bit value, plus 1.
			{ "-9223372036854775807.00999999999999999999999999999999", 0 }, // Minimum signed 64-bit value, plus 1.

			{ "-92233720368547758.08999999999999999999999999999999", -9223372036854775808LL }, // Minimum signed 64-bit value.
			{ "-92233720368547758.00999999999999999999999999999999", -9223372036854775800LL }, // Minimum signed 64-bit value.
			{ "-9223372036854775808.00999999999999999999999999999999", 0 }, // Minimum signed 64-bit value.

			{ "-92233720368547758.09999999999999999999999999999999", 0 }, // Minimum signed 64-bit value, less 1.
			{ "-92233720368547759.00999999999999999999999999999999", 0 }, // Minimum signed 64-bit value, less 1.
			{ "-9223372036854775809.00999999999999999999999999999999", 0 }, // Minimum signed 64-bit value, less 1.

			{ "-184467440737095516.15999999999999999999999999999999", 0 }, // Maximum unsigned 64-bit value, negated.
			{ "-184467440737095517.00999999999999999999999999999999", 0 }, // Maximum unsigned 64-bit value, negated.
			{ "-18446744073709551615.00999999999999999999999999999999", 0 }, // Maximum unsigned 64-bit value, negated.

			{ "999999999999999999.99999999999999999999999999999999", 0 },
			{ "999999999999999999.00999999999999999999999999999999", 0 },
			{ "99999999999999999999.00999999999999999999999999999999", 0 },

			{ "-184467440737095516.15999999999999999999999999999999", 0 }, // Maximum unsigned 64-bit value, negated.
			{ "-184467440737095516.00999999999999999999999999999999", 0 }, // Maximum unsigned 64-bit value, negated.
			{ "-18446744073709551615.00999999999999999999999999999999", 0 }, // Maximum unsigned 64-bit value, negated.

			{ "-184467440737095516.16999999999999999999999999999999", 0 }, // Maximum unsigned 64-bit value plus 1, negated.
			{ "-184467440737095516.00999999999999999999999999999999", 0 }, // Maximum unsigned 64-bit value plus 1, negated.
			{ "-18446744073709551616.00999999999999999999999999999999", 0 }, // Maximum unsigned 64-bit value plus 1, negated.

			{ "-999999999999999999.99999999999999999999999999999999", 0 },
			{ "-999999999999999999.00999999999999999999999999999999", 0 },
			{ "-99999999999999999999.00999999999999999999999999999999", 0 },

			{ "-99999999999999999999999999999999999999.99999999999999999999999999999999", 0 },
			{ "-99999999999999999999999999999999999999.00999999999999999999999999999999", 0 },
			{ "-9999999999999999999999999999999999999999.00999999999999999999999999999999", 0 },

			// More content.
			{ "012.34999999999999999999999999999999", 1234 }, // Test that a value with a leading zero is not treated as a valid octal value.
			{ "012.00999999999999999999999999999999", 1200 }, // Test that a value with a leading zero is not treated as a valid octal value.
			{ "01234.00999999999999999999999999999999", 123400 }, // Test that a value with a leading zero is not treated as a valid octal value.

			{ "099.99999999999999999999999999999999", 9999 }, // Test that a value with a leading zero is not treated as an invalid octal value.
			{ "099.00999999999999999999999999999999", 9900 }, // Test that a value with a leading zero is not treated as an invalid octal value.
			{ "09999.00999999999999999999999999999999", 999900 }, // Test that a value with a leading zero is not treated as an invalid octal value.
		};

		for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
		{
			// Setup.
			// Perform operation(s) to be tested.
			const __int64 variable = CUtilities::GetInt64FromCurrencyString(TEST_DATA[testIndex].m_stringValue);

			// Test(s).
			std::string resultName;
			resultName = FormatString(_T("GetInt64FromCurrencyString(\"%s\") variable"),
									  TEST_DATA[testIndex].m_stringValue.c_str());
			ASSERT_TRUE(variable == TEST_DATA[testIndex].m_variableExpected) << resultName;
		}
	}

	TEST(UtilitiesUnitTests, UtilitiesGetUTCSystemTime)
	{
		time_t ctime = time(0);
		SYSTEMTIME systime1, systime2;
		CUtilities::GetUTCSystemTime(&systime1);
		systime1.wMilliseconds = 0;

		systime2 = CUtilities::GetUTCSystemTime(ctime);
		ASSERT_TRUE( systime2.wMilliseconds == 0);

		if (systime1.wSecond == systime2.wSecond)
		{
			ASSERT_TRUE( CUtilities::CompareSysTimes( systime1, systime2) == 0 ) <<
                systime1.wYear << "-" << systime1.wMonth << "-" << systime1.wDay <<
                "(" << systime1.wDayOfWeek << " " << systime1.wMilliseconds << ")" <<
                systime1.wHour << ":" << systime1.wMinute << ":" << systime1.wSecond <<
                systime2.wYear << "-" << systime2.wMonth << "-" << systime2.wDay <<
                "(" << systime2.wDayOfWeek << " " << systime2.wMilliseconds << ")" <<
                systime2.wHour << ":" << systime2.wMinute << ":" << systime2.wSecond;
		}
		else
		{
			// milliseconds rolled, try again
			CUtilities::GetLocalSystemTime(&systime1);
			systime1.wMilliseconds = 0;

			systime2 = CUtilities::GetUTCSystemTime(ctime);
			ASSERT_FALSE( CUtilities::CompareSysTimes(systime1, systime2) == 0 );
		}				
	}
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;

	TEST(UtilitiesUnitTests, UtilitiesIsFlv)
	{
		ASSERT_TRUE( CUtilities::IsFlv( "test/test.flv"));
		ASSERT_TRUE( CUtilities::IsFlv( "testflv"));
		ASSERT_FALSE( CUtilities::IsFlv( "test.test"));
		ASSERT_TRUE( CUtilities::IsFlv( "testflv.flv"));
	}
	
	TEST(UtilitiesUnitTests, UtilitiesIsBmp)
	{
		ASSERT_TRUE( CUtilities::IsBmp( "test/test.bmp"));
		ASSERT_TRUE( CUtilities::IsBmp( "testbmp"));
		ASSERT_FALSE( CUtilities::IsBmp( "test.test"));
		ASSERT_TRUE( CUtilities::IsBmp( "testbmp.bmp"));
	}

	TEST(UtilitiesUnitTests, UtilitiesRotateAspectRatio)
	{
		double ar = 1.2;
		double invertedAr = 1 / ar;
		ASSERT_TRUE(CUtilities::Rotate(ar, DesktopOrientationDefault) == ar);
		ASSERT_TRUE(CUtilities::Rotate(ar, DesktopOrientation180) == ar);
		ASSERT_TRUE(CUtilities::Rotate(ar, DesktopOrientation90) == invertedAr);
		ASSERT_TRUE(CUtilities::Rotate(ar, DesktopOrientation270) == invertedAr);
	}

	TEST(UtilitiesUnitTests, UtilitiesRotateSize)
	{
		DesktopOrientationType orientation = DesktopOrientationDefault;
		struct SIZE size(1250,750);
		struct SIZE newSize(750, 1250);

		ASSERT_TRUE(CUtilities::Rotate(size, DesktopOrientationDefault) == size);
		ASSERT_TRUE(CUtilities::Rotate(size, DesktopOrientation180) == size);

		ASSERT_TRUE(CUtilities::Rotate(size, DesktopOrientation90) == newSize);
		ASSERT_TRUE(CUtilities::Rotate(size, DesktopOrientation270) == newSize);
	}

	TEST(UtilitiesUnitTests, UtilitiesWidthToHeightRatio)
	{
		struct SIZE size(1280,768);
		double ar = 1280.0 / 768.0;
		double otherAr = 1/ar;
		double retAr;

		struct RECT rect(0,0,1280,768);
		retAr =  CUtilities::WidthToHeightRatio(rect);
		ASSERT_TRUE( retAr == ar);

		retAr =  CUtilities::WidthToHeightRatio(size);
		ASSERT_TRUE(retAr == ar);

		ASSERT_TRUE( CUtilities::WidthToHeightRatio(size, DesktopOrientationDefault) == ar);
		ASSERT_TRUE( CUtilities::WidthToHeightRatio(size, DesktopOrientation180) == ar);
		ASSERT_TRUE( CUtilities::WidthToHeightRatio(size, DesktopOrientation90) == otherAr);
		ASSERT_TRUE( CUtilities::WidthToHeightRatio(size, DesktopOrientation270) == otherAr);
	}

	TEST(UtilitiesUnitTests, UtilitiesIsValidDate1)
	{
		
		time_t test;

		// normal time
		test = CUtilities::GetTimeFromFields(2014, 02, 12, 12, 00, 00);

		ASSERT_TRUE(CUtilities::IsDateValid(test));

		// less than 0
		test = -1;

		ASSERT_FALSE(CUtilities::IsDateValid(test));

		// greater than max
		test = CUtilities::GetTimeFromFields(3000, 12, 31, 00, 00, 00);
		test += 100000; // more than 1 day
		ASSERT_FALSE(CUtilities::IsDateValid(test));

		bool retVal = true;

		struct tm y3k = { };
		y3k.tm_year = 3000 - 1900; y3k.tm_mon = 11; y3k.tm_mday = 31;
		y3k.tm_hour = 0;   y3k.tm_min = 0; y3k.tm_sec = 0;
		y3k.tm_isdst = 0;
		time_t y3kTime(mktime(&y3k));

		if ( test > y3kTime || test < 0)
		{
			retVal = false;
		}

		ASSERT_FALSE(retVal);
	}


	TEST(UtilitiesUnitTests, UtilitiesIsValidDate2)
	{
		// normal time
		struct tm _tm = { };
		_tm.tm_year = 2016-CUtilities::EPOCH_YEAR; _tm.tm_mon = 11; _tm.tm_mday = 03;
		_tm.tm_hour = 0;   _tm.tm_min = 0; _tm.tm_sec = 0;
		_tm.tm_isdst = 0;
        time_t test(CUtilities::GetTimeFromLocalTm(_tm));
		string dateStr;
        dateStr = CUtilities::FormatDateTimeString(test);

		EXPECT_TRUE(CUtilities::IsDateValid(test));

		// less than 0
		test = -1;

		EXPECT_FALSE(CUtilities::IsDateValid(test));

		// greater than max
		struct tm _tm2 = { };
		_tm2.tm_year = 2038 - CUtilities::EPOCH_YEAR; _tm2.tm_mon = 1; _tm2.tm_mday = 1;
		_tm2.tm_hour = 0;   _tm2.tm_min = 0; _tm2.tm_sec = 0;
		_tm2.tm_isdst = 0;
        time_t test2(CUtilities::GetTimeFromLocalTm(_tm2));
        dateStr = CUtilities::FormatDateTimeString(test2);
		test2 += 100000; // more than 1 day

		EXPECT_FALSE(CUtilities::IsDateValid(test2));
	}

	TEST(UtilitiesUnitTests, UtilitiesIsDateValid2)
	{
		ASSERT_FALSE(CUtilities::IsDateValid(time_t(0x80000000)));
		ASSERT_FALSE(CUtilities::IsDateValid(time_t(-1)));
		ASSERT_TRUE(CUtilities::IsDateValid(time_t(0)));
		ASSERT_TRUE(CUtilities::IsDateValid(CUtilities::GetTimeFromFields(2037, 12, 31, 23, 59, 59)));
		ASSERT_TRUE(CUtilities::IsDateValid(CUtilities::GetTimeFromFields(2038, 1, 1, 0, 0, 0)));
		ASSERT_FALSE(CUtilities::IsDateValid(CUtilities::GetTimeFromFields(2038, 1, 1, 0, 0, 1)));
		ASSERT_FALSE(CUtilities::IsDateValid(time_t(0x7fffffff)));
	}

	TEST(UtilitiesUnitTests, UtilitiesFormatDateTimeString1)
	{
		time_t test;
		test = -100;

		// < 0
		std::string dateStr = CUtilities::FormatDateTimeString(test);
		ASSERT_TRUE(dateStr == "-100");

		// greater than max
		test = CUtilities::GetTimeFromFields(3000, 12, 31, 00, 00, 00);
		test += 100000; // more than 1 day
		dateStr = CUtilities::FormatDateTimeString(test);
		ASSERT_FALSE(dateStr == "-1824479968");

		// normal date
		test = CUtilities::GetTimeFromFields(2014, 02, 12, 12, 00, 00);
		dateStr = CUtilities::FormatDateTimeString(test);
		ASSERT_TRUE(dateStr == "02/12/2014 12:00:00 PM");
	}

	
	TEST(UtilitiesUnitTests, UtilitiesFormatDateTimeString2)
	{
		CUtilities::SetTzOffset(+1 * 60);
		time_t test;
		test = -100;

		// < 0
		std::string dateStr = CUtilities::FormatDateTimeString(test);
		EXPECT_EQ("-100", dateStr);

		// normal date
		struct tm _tm2 = { };
		_tm2.tm_year = 2016-CUtilities::EPOCH_YEAR; _tm2.tm_mon = 10; _tm2.tm_mday = 2;
		_tm2.tm_hour = 0;   _tm2.tm_min = 0; _tm2.tm_sec = 0;
		_tm2.tm_isdst = 0;
		time_t test3(CUtilities::GetTimeFromLocalTm(_tm2));
		dateStr = CUtilities::FormatDateTimeString(test3);
		EXPECT_EQ("11/02/2016 12:00:00 AM", dateStr);
	}
	

	TEST(UtilitiesUnitTests, UtilitiesFormatDateTimeString3)
	{
		std::string timeString;
		timeString = CUtilities::FormatDateTimeString(time_t(0x7fffffff));
		ASSERT_TRUE(timeString == _T("2147483647"));
	}

	TEST(UtilitiesUnitTests, UtilitiesFormatMilitaryStringFromUTC)
	{
		const LPCTSTR format1970 = "N/A";
		const SYSTEMTIME JAN1970_SYSTIME = { 1970, 1, 4, 1, 0, 0, 0, 0 };

		CUtilities::SetTzOffset(0);
		std::string timeString = CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(JAN1970_SYSTIME);
		ASSERT_TRUE(timeString.compare(format1970) == 0) << "Military format from UTC is incorrect";
	}

	TEST(UtilitiesUnitTests, UtilitiesFormatMilitaryStringFromUTC_1)
	{
		const LPCTSTR format1970 = "01/01/1970 00:00:00";
		const SYSTEMTIME JAN1970_SYSTIME = { 1970, 1, 4, 1, 0, 0, 0, 1 };

		CUtilities::SetTzOffset(0);
		std::string timeString = CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(JAN1970_SYSTIME);
		ASSERT_TRUE(timeString.compare(format1970) == 0) << "Military format from UTC is incorrect";
	}

	TEST(UtilitiesUnitTests, UtilitiesFormatMilitaryStringFromUTC_1970plus)
	{
		const LPCTSTR format1970 = "N/A";
		const LPCTSTR format1970p1 = "N/A";
		const LPCTSTR format1970p480 = "N/A";
		const LPCTSTR format1970p16h = "N/A";
		const LPCTSTR format1970imax60 = "N/A";
		const LPCTSTR format1970imax = "N/A";

		const SYSTEMTIME JAN1970_SYSTIME = { 1970, 1, 4, 1, 0, 0, 0, 0 };
		std::string timeString;

		CUtilities::SetTzOffset(0);
		timeString = CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(JAN1970_SYSTIME);
		ASSERT_TRUE(timeString.compare(format1970) == 0) << "Military format from UTC is incorrect";

		CUtilities::SetTzOffset(1);
		timeString = CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(JAN1970_SYSTIME);
		ASSERT_TRUE(timeString.compare(format1970p1) == 0) << "Military format from UTC is incorrect";

		CUtilities::SetTzOffset(480);
		timeString = CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(JAN1970_SYSTIME);
		ASSERT_TRUE(timeString.compare(format1970p480) == 0) << "Military format from UTC is incorrect";

		CUtilities::SetTzOffset(16 * 60); // +16 hours.
		timeString = CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(JAN1970_SYSTIME);
		ASSERT_TRUE(timeString.compare(format1970p16h) == 0) << "Military format from UTC is incorrect";

		CUtilities::SetTzOffset(INT_MAX / 60); // Maximum without internal overflow.
		timeString = CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(JAN1970_SYSTIME);
		ASSERT_TRUE(timeString.compare(format1970imax60) == 0) << "Military format from UTC is incorrect";

		CUtilities::SetTzOffset(INT_MAX); // Maximum.
		timeString = CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(JAN1970_SYSTIME);
		ASSERT_TRUE(timeString.compare(format1970imax) == 0) << "Military format from UTC is incorrect";
	}

	TEST(UtilitiesUnitTests, UtilitiesFormatMilitaryStringFromUTC_1970_1plus)
	{
		const LPCTSTR format1970 = "01/01/1970 00:00:00";
		const LPCTSTR format1970p1 = "01/01/1970 00:01:00";
		const LPCTSTR format1970p480 = "01/01/1970 08:00:00";
		const LPCTSTR format1970p16h = "01/01/1970 16:00:00";
		const LPCTSTR format1970imax60 = "01/19/2038 03:14:00";
		const LPCTSTR format1970imax = "12/31/1969 23:59:00";

		const SYSTEMTIME JAN1970_SYSTIME = { 1970, 1, 4, 1, 0, 0, 0, 1 };
		std::string timeString;

		CUtilities::SetTzOffset(0);
		timeString = CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(JAN1970_SYSTIME);
		ASSERT_TRUE(timeString.compare(format1970) == 0) << "Military format from UTC is incorrect";

		CUtilities::SetTzOffset(1);
		timeString = CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(JAN1970_SYSTIME);
		ASSERT_TRUE(timeString.compare(format1970p1) == 0) << "Military format from UTC is incorrect";

		CUtilities::SetTzOffset(480);
		timeString = CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(JAN1970_SYSTIME);
		ASSERT_TRUE(timeString.compare(format1970p480) == 0) << "Military format from UTC is incorrect";

		CUtilities::SetTzOffset(16 * 60); // +16 hours.
		timeString = CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(JAN1970_SYSTIME);
		ASSERT_TRUE(timeString.compare(format1970p16h) == 0) << "Military format from UTC is incorrect";

		CUtilities::SetTzOffset(INT_MAX / 60); // Maximum without internal overflow.
		timeString = CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(JAN1970_SYSTIME);
		ASSERT_FALSE(timeString.compare(format1970imax60) == 0) << "Military format from UTC is incorrect";

		CUtilities::SetTzOffset(INT_MAX); // Maximum.
		timeString = CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(JAN1970_SYSTIME);
		ASSERT_TRUE(timeString.compare(format1970imax) == 0) << "Military format from UTC is incorrect";
	}

	TEST(UtilitiesUnitTests, UtilitiesFormatMilitaryStringFromUTC_1970minus)
	{
		const LPCTSTR format1970 = "N/A";
		const LPCTSTR format1970m1 = "N/A";
		const LPCTSTR format1970m480 = "N/A";
		const LPCTSTR format1970m16h = "N/A";
		const LPCTSTR format1970imin60 = "N/A";
		const LPCTSTR format1970imin = "N/A";

		const SYSTEMTIME JAN1970_SYSTIME = { 1970, 1, 4, 1, 0, 0, 0, 0 };
		std::string timeString;

		CUtilities::SetTzOffset(0);
		timeString = CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(JAN1970_SYSTIME);
		ASSERT_TRUE(timeString.compare(format1970) == 0) << "Military format from UTC is incorrect";

		CUtilities::SetTzOffset(-1);
		timeString = CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(JAN1970_SYSTIME);
		ASSERT_TRUE(timeString.compare(format1970m1) == 0) << "Military format from UTC is incorrect";

		CUtilities::SetTzOffset(-480);
		timeString = CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(JAN1970_SYSTIME);
		ASSERT_TRUE(timeString.compare(format1970m480) == 0) << "Military format from UTC is incorrect";

		CUtilities::SetTzOffset(-16 * 60); // -16 hours.
		timeString = CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(JAN1970_SYSTIME);
		ASSERT_TRUE(timeString.compare(format1970m16h) == 0) << "Military format from UTC is incorrect";

		CUtilities::SetTzOffset(INT_MIN / 60); // Minimum without internal overflow.
		timeString = CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(JAN1970_SYSTIME);
		ASSERT_TRUE(timeString.compare(format1970imin60) == 0) << "Military format from UTC is incorrect";

		CUtilities::SetTzOffset(INT_MIN); // Minimum.
		timeString = CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(JAN1970_SYSTIME);
		ASSERT_TRUE(timeString.compare(format1970imin) == 0) << "Military format from UTC is incorrect";
	}

	TEST(UtilitiesUnitTests, UtilitiesFormatMilitaryStringFromUTC_1970_1minus)
	{
		const LPCTSTR format1970 = "01/01/1970 00:00:00";
		const LPCTSTR format1970m1 = "12/31/1969 23:59:00";
		const LPCTSTR format1970m480 = "12/31/1969 16:00:00";
		const LPCTSTR format1970m16h = "-28800";
		const LPCTSTR format1970imin60 = "-2147454840";
		const LPCTSTR format1970imin = "01/01/1970 00:00:00";

		const SYSTEMTIME JAN1970_SYSTIME = { 1970, 1, 4, 1, 0, 0, 0, 1 };
		std::string timeString;

		CUtilities::SetTzOffset(0);
		timeString = CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(JAN1970_SYSTIME);
		ASSERT_TRUE(timeString.compare(format1970) == 0) << "Military format from UTC is incorrect";

		CUtilities::SetTzOffset(-1);
		timeString = CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(JAN1970_SYSTIME);
		ASSERT_TRUE(timeString.compare(format1970m1) == 0) << "Military format from UTC is incorrect";

		CUtilities::SetTzOffset(-480);
		timeString = CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(JAN1970_SYSTIME);
		ASSERT_TRUE(timeString.compare(format1970m480) == 0) << "Military format from UTC is incorrect";

		CUtilities::SetTzOffset(-16 * 60); // -16 hours.
		timeString = CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(JAN1970_SYSTIME);
		ASSERT_FALSE(timeString.compare(format1970m16h) == 0) << "Military format from UTC is incorrect";
		
		CUtilities::SetTzOffset(INT_MIN / 60); // Minimum without internal overflow.
		timeString = CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(JAN1970_SYSTIME);
		ASSERT_FALSE(timeString.compare(format1970imin60) == 0) << "Military format from UTC is incorrect";

		CUtilities::SetTzOffset(INT_MIN); // Minimum.
		timeString = CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(JAN1970_SYSTIME);
		ASSERT_TRUE(timeString.compare(format1970imin) == 0) << "Military format from UTC is incorrect";

	}

#if 000//TODO-PORT-UNIT-TESTS
	TEST(UtilitiesUnitTests, UtilitiesFormatMilitaryStringFromUTC_2070minus)
	{
		const LPCTSTR format2070 = "01/01/2037 00:00:00";
		const LPCTSTR format2070m1 = "12/31/2036 23:59:00";
		const LPCTSTR format2070m480 = "12/31/2036 16:00:00";
		const LPCTSTR format2070m16h = "12/31/2036 08:00:00";
		const LPCTSTR format2070imin60 = "12/13/2001 20:46:00";
		const LPCTSTR format2070imin = "01/01/2037 00:00:00";

		const SYSTEMTIME JAN2070_SYSTIME = { 2037, 1, 4, 1, 0, 0, 0, 0 };
		std::string timeString;

		CUtilities::SetTzOffset(0);
		timeString = CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(JAN2070_SYSTIME);
		ASSERT_FALSE(timeString.compare(format2070) == 0) << "Military format from UTC is incorrect";

		CUtilities::SetTzOffset(-1);
		timeString = CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(JAN2070_SYSTIME);
		ASSERT_FALSE(timeString.compare(format2070m1) == 0) << "Military format from UTC is incorrect";

		CUtilities::SetTzOffset(-480);
		timeString = CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(JAN2070_SYSTIME);
		ASSERT_FALSE(timeString.compare(format2070m480) == 0) << "Military format from UTC is incorrect";

		CUtilities::SetTzOffset(-16 * 60); // -16 hours.
		timeString = CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(JAN2070_SYSTIME);
		ASSERT_FALSE(timeString.compare(format2070m16h) == 0) << "Military format from UTC is incorrect";

		CUtilities::SetTzOffset(INT_MIN / 60); // Minimum without internal overflow.
		timeString = CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(JAN2070_SYSTIME);
		ASSERT_TRUE(timeString.compare(format2070imin60) == 0) << "Military format from UTC is incorrect";

		CUtilities::SetTzOffset(INT_MIN); // Minimum.
		timeString = CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(JAN2070_SYSTIME);
		ASSERT_FALSE(timeString.compare(format2070imin) == 0) << "Military format from UTC is incorrect";
	}
#endif

	/// <summary>
	/// Functional test for:
	///     std::string CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(CTime utcTime)
	/// Test with an non-zero time zone offset and a valid utcTime.
	/// Test that CUnitTestingUtils returns the same result.
	/// </summary>
	TEST(UtilitiesUnitTests, UtilitiesFormatLocalMilitaryDateTimeStringFromUTC_Valid)
	{
		// Data.
		const int tzOffsetMinutes(450); // 7 hours, 30 minutes.
		const std::string expectedString(_T("02/28/2001 07:04:56"));
		const time_t utcTime = CUtilities::GetTimeFromFields(2001, 2, 27, 23, 34, 56);

		// Setup.
		CUtilities::SetTzOffset(tzOffsetMinutes);

		// Perform operation(s) and test(s).
		const std::string localTimeString(CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(utcTime));
		ASSERT_FALSE(expectedString == localTimeString) << "localTimeString";
#if 0 // TODO Not ported
		ASSERT_TRUE(expectedString == CUnitTestingUtils::FormatLocalMilitaryDateTimeStringFromUTC(utcTime)) << "CUnitTestingUtils";
#endif
	}

	/// <summary>
	/// Functional test for:
	///     CTime CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(CTime utcTime)
	/// Test with an non-zero time zone offset and an invalid utcTime.
	/// Use GetTime() to compare times, because formatting invalid CTimes may throw an unwanted exception.
	/// Test that CUnitTestingUtils returns the same result.
	/// </summary>
	TEST(UtilitiesUnitTests, UtilitiesFormatLocalMilitaryDateTimeStringFromUTC_Invalid)
	{
		// Data.
		const int tzOffsetMinutes(450); // 7 hours, 30 minutes.
		const time_t utcTime(-123);
		const std::string expectedString(_T("-123"));

		// Verify that the starting utcTime is invalid, but would become valid if the time zone offset was added to it.
		ASSERT_FALSE(CUtilities::IsDateValid(utcTime));
		ASSERT_TRUE(CUtilities::IsDateValid(utcTime + (tzOffsetMinutes * SECONDS_PER_MINUTE)));

		// Setup.
		CUtilities::SetTzOffset(tzOffsetMinutes);

		// Perform operation(s) and test(s).
		const std::string localTimeString(CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(utcTime));
		ASSERT_TRUE(expectedString == localTimeString) << "localTimeString";
		ASSERT_TRUE(expectedString == CUnitTestingUtils::FormatLocalMilitaryDateTimeStringFromUTC(utcTime)) << "CUnitTestingUtils";
	}

	TEST(UtilitiesUnitTests, UtilitiesGetFormattedCashString)
	{
		DWORD i = 1;
		string result = CUtilities::GetFormattedCashString(i);
		ASSERT_FALSE(result == "0.01");

		i = 100;
		result = CUtilities::GetFormattedCashString(i);
		printf("result 1 = %s\n", result.c_str());
		ASSERT_TRUE(result == "$1.00");


		i = 9989;
		result = CUtilities::GetFormattedCashString(i);
		ASSERT_TRUE(result == "$99.89");

		__int64 value = 999999999999LL; // 2014-06-26 Paltronics maximum.
		result = CUtilities::GetFormattedCashString(value);
		ASSERT_TRUE(result == "$9,999,999,999.99");
	}


	TEST(UtilitiesUnitTests, UtilitiesGetFormattedCashStringx)
	{
		string currency;
		__int64 amount = -1;
		currency = CUtilities::GetFormattedCashString(amount);
		ASSERT_TRUE(currency == "$-0.01");
		amount = -9;
		currency = CUtilities::GetFormattedCashString(amount);
		ASSERT_TRUE(currency == "$-0.09");

		amount = -10;
		currency = CUtilities::GetFormattedCashString(amount);
		ASSERT_TRUE(currency == "$-0.10");

		amount = -99;
		currency = CUtilities::GetFormattedCashString(amount);
		ASSERT_TRUE(currency == "$-0.99");

		amount = -100;
		currency = CUtilities::GetFormattedCashString(amount);
		ASSERT_TRUE(currency == "$-1.00");

		amount = -1234567890123456789LL;
		currency = CUtilities::GetFormattedCashString(amount);
		ASSERT_TRUE(currency == "$-12,345,678,901,234,567.89");

		amount = INT64_MIN;
		currency = CUtilities::GetFormattedCashString(amount);
		ASSERT_TRUE(currency == "$-92,233,720,368,547,758.08");

		amount = 0;
		currency = CUtilities::GetFormattedCashString(amount);
		ASSERT_TRUE(currency == "$0.00");

		amount = 1;
		currency = CUtilities::GetFormattedCashString(amount);
		ASSERT_TRUE(currency == "$0.01");

		amount = 9;
		currency = CUtilities::GetFormattedCashString(amount);
		ASSERT_TRUE(currency == "$0.09");

		amount = 10;
		currency = CUtilities::GetFormattedCashString(amount);
		ASSERT_TRUE(currency == "$0.10");

		amount = 99;
		currency = CUtilities::GetFormattedCashString(amount);
		ASSERT_TRUE(currency == "$0.99");

		amount = 100;
		currency = CUtilities::GetFormattedCashString(amount);
		ASSERT_TRUE(currency == "$1.00");

		amount = 1234567890123456789LL;
		currency = CUtilities::GetFormattedCashString(amount);
		ASSERT_TRUE(currency == "$12,345,678,901,234,567.89");

		amount = 9223372036854775807LL;
		currency = CUtilities::GetFormattedCashString(amount);
		ASSERT_TRUE(currency == "$92,233,720,368,547,758.07");

		amount = INT64_MAX;
		currency = CUtilities::GetFormattedCashString(amount);
		ASSERT_TRUE(currency == "$92,233,720,368,547,758.07");
	}
	

	TEST(UtilitiesUnitTests, UtilitiesGetFormattedCashStringAndBack)
	{
		int testIndex;
		__int64 value;
		__int64 result;
		std::string currency;

		testIndex = 0;
		value = INT64_MIN;
		currency = CUtilities::GetFormattedCashString(value);
		printf("currency = %s\n", currency.c_str());
		ASSERT_TRUE(currency == "$-92,233,720,368,547,758.08");
		result = CUtilities::GetInt64FromCurrencyString(currency.substr(1));
		ASSERT_TRUE(result == value);

		testIndex = 1;
		value = INT64_MAX;
		currency = CUtilities::GetFormattedCashString(value);
		ASSERT_TRUE(currency == "$92,233,720,368,547,758.07");
		result = CUtilities::GetInt64FromCurrencyString(currency.substr(1));
		ASSERT_TRUE(result == value);

		testIndex = 2;
		value = -1234567890123456789LL;
		currency = CUtilities::GetFormattedCashString(value);
		ASSERT_TRUE(currency == "$-12,345,678,901,234,567.89");
		result = CUtilities::GetInt64FromCurrencyString(currency.substr(1));
		ASSERT_TRUE(result == value);

		testIndex = 3;
		value = 1234567890123456789LL;
		currency = CUtilities::GetFormattedCashString(value);
		ASSERT_TRUE(currency == "$12,345,678,901,234,567.89");
		result = CUtilities::GetInt64FromCurrencyString(currency.substr(1));
		ASSERT_TRUE(result == value);

	}

	TEST(UtilitiesUnitTests, UtilitiesGetInt64FromCurrencyString)
	{
		string currency;
		int result;

		currency = "0.01";
		result = (int)CUtilities::GetInt64FromCurrencyString(currency);
		ASSERT_TRUE(result == 1);

		currency = "1.00";
		result = (int)CUtilities::GetInt64FromCurrencyString(currency);
		ASSERT_TRUE(result == 100);

		currency = "99.89";
		result = (int)CUtilities::GetInt64FromCurrencyString(currency);
		ASSERT_TRUE(result == 9989);

		currency = "123";
		result = (int)CUtilities::GetInt64FromCurrencyString(currency);
		ASSERT_TRUE(result == 12300);

		currency = "123.";
		result = (int)CUtilities::GetInt64FromCurrencyString(currency);
		ASSERT_TRUE(result == 12300);

		currency = "123.4";
		result = (int)CUtilities::GetInt64FromCurrencyString(currency);
		ASSERT_TRUE(result == 12340);

		currency = "123.45";
		result = (int)CUtilities::GetInt64FromCurrencyString(currency);
		ASSERT_TRUE(result == 12345);

		currency = "123.450";
		result = (int)CUtilities::GetInt64FromCurrencyString(currency);
		ASSERT_TRUE(result == 12345);

		currency = "123.4500";
		result = (int)CUtilities::GetInt64FromCurrencyString(currency);
		ASSERT_TRUE(result == 12345);

		currency = "123.45000000000000000000000000000000";
		result = (int)CUtilities::GetInt64FromCurrencyString(currency);
		ASSERT_TRUE(result == 12345);

		currency = "9999999999.99"; // 2014-06-26 Paltronics maximum.
		__int64 amount = CUtilities::GetInt64FromCurrencyString(currency);
		ASSERT_TRUE(amount == 999999999999LL);
	}

	TEST(UtilitiesUnitTests, UtilitiesGetStdStringFromString)
	{
		std::string testString = "Test String";
		std::string resultString = "Test String";
		ASSERT_TRUE(CUtilities::GetStdStringFromString(testString) == resultString);

		testString = "";
		resultString = "";
		ASSERT_TRUE(CUtilities::GetStdStringFromString(testString) == resultString);
	}

#if 000//TODO-PORT LV  
	TEST(UtilitiesUnitTests, UtilitiesTimespanTests)
	{
		const SYSTEMTIME JAN1970_SYSTIME = { 1970, 1, 4, 1, 0, 0, 0, 0 };
		const SYSTEMTIME JAN1980_SYSTIME = { 1980, 1, 2, 1, 0, 0, 0, 0 };
		const SYSTEMTIME JAN1990_SYSTIME = { 1990, 1, 1, 1, 0, 0, 0, 0 };
		const SYSTEMTIME JAN2000_SYSTIME = { 2000, 1, 6, 1, 0, 0, 0, 0 };
		const SYSTEMTIME JAN2010_SYSTIME = { 2010, 1, 5, 1, 0, 0, 0, 0 };
		const __int64 TIMESPAN_2010_1970_MILLISECONDS = 1262304000000;

		__int64 timespan = CUtilities::GetSystemTimespanInMilliseconds(JAN1970_SYSTIME, JAN1980_SYSTIME);
		timespan += CUtilities::GetSystemTimespanInMilliseconds(JAN1980_SYSTIME, JAN1990_SYSTIME);
		timespan += CUtilities::GetSystemTimespanInMilliseconds(JAN1990_SYSTIME, JAN2000_SYSTIME);
		timespan += CUtilities::GetSystemTimespanInMilliseconds(JAN2000_SYSTIME, JAN2010_SYSTIME);
		ASSERT_TRUE(timespan == CUtilities::GetSystemTimespanInMilliseconds(JAN1970_SYSTIME, JAN2010_SYSTIME)) << "timespans should be equal";
		ASSERT_TRUE(timespan == TIMESPAN_2010_1970_MILLISECONDS) << "timespan incorrect";
		ASSERT_TRUE(timespan == CUtilities::ConvertToMillisecondsSince1970(JAN2010_SYSTIME)) << "timespan since 1970 is incorrect";

		SYSTEMTIME currentTime = CUtilities::GetUTCSystemTime(time(nullptr));
		ASSERT_TRUE(CUtilities::GetSystemTimespanInMilliseconds(JAN1980_SYSTIME, JAN1970_SYSTIME) * -1 
					== CUtilities::GetSystemTimespanInMilliseconds(JAN1970_SYSTIME, JAN1980_SYSTIME)) << "Negative timespan test failed";
		ASSERT_TRUE(CUtilities::GetSystemTimespanInMilliseconds(CUtilities::Get1970Time(), JAN1970_SYSTIME) == 0) << "Get 1970 time failed";
		ASSERT_TRUE(CUtilities::Is1970Time(CUtilities::Get1970Time())) << "Is 1970 time check failed";
	}
#endif

	/// <summary>
	/// Code coverage and functional test for:
	///     CUtilities::GetPrefixIdString()
	/// Test using SAS ATI bonus parameters.
	/// </summary>
	TEST(UtilitiesUnitTests, GetPrefixIdString_AtiBonusTest)
	{
		// Input(s), independent of iteration.
		LPCTSTR const prefix(_T("ATIBONUS"));
		const int maxLength(MAX_PBID_LENGTH);

		typedef struct
		{
			// Input(s).
			LONG64 m_jackpotId;

			// Expected result(s).
			const std::string m_transactionIdExpected;
		} TestDataType;

		const TestDataType TEST_DATA[] =
		{
			{ -1, _T("ATIBONUS-1") },
			{ 0, _T("ATIBONUS0") },
			{ 1, _T("ATIBONUS1") },
			{ 1234567890, _T("ATIBONUS1234567890") },
			{ 2340000000000000890LL, _T("ATIBONUS000000000890") },
			{ 2345678901234567890LL, _T("ATIBONUS901234567890") },
			{ INT64_MAX, _T("ATIBONUS036854775807") }, // 9223372036854775807i64
			{ (LONG64)((UINT64)INT64_MAX + 1), _T("ATIBONUS-36854775808") },
			{ (LONG64)((UINT64)INT64_MAX + 2), _T("ATIBONUS-36854775807") }, // Same digits as LONG64_MAX.
			{ INT64_MIN, _T("ATIBONUS-36854775808") },
			{ -2340000000000000890LL, _T("ATIBONUS-00000000890") },
			{ -2345678901234567890LL, _T("ATIBONUS-01234567890") },
			{ -1234567890, _T("ATIBONUS-1234567890") },
		};

		for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
		{

			// Perform operation(s) to be tested.
			std::string transactionIdResult(CUtilities::GetPrefixIdString(prefix, TEST_DATA[testIndex].m_jackpotId, maxLength));

			// Test(s).
			AssertResult2(transactionIdResult, TEST_DATA[testIndex].m_transactionIdExpected);
			ASSERT_TRUE(transactionIdResult.length() <= maxLength) <<  _T("transactionIdResult.GetLength() <= maxLength");
		}
	}

	TEST(UtilitiesUnitTests, ConvertStringToLongLongTest)
	{
		typedef struct
		{
			// Input(s).
			std::string m_numberString;

			// Expected result(s).
			const int64_t m_numberExpected;
		} TestDataType;

		const TestDataType TEST_DATA[] =
		{
			{ "-1", -1 },
			{ "0", 0 },
			{ "-9223372036854775808", INT64_MIN},
			{ "9223372036854775807", INT64_MAX},
			{ "-9223372036854775809", -1 },
			{ "9223372036854775808", -1 },
			{ "1253aa1", 1253 },
		};

		for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
		{

			// Perform operation(s) to be tested.
			int64_t numberResult(CUtilities::ConvertStringToLongLong(TEST_DATA[testIndex].m_numberString));

			// Test(s).
			AssertResult2(numberResult, TEST_DATA[testIndex].m_numberExpected);
		}
	}
	
	TEST(UtilitiesUnitTests, UtilitiesEncryptPBTPinTest)
	{
			DWORD unencryptedPIN(1234);
			DWORD expectedPin(10762);
			ASSERT_EQ(CUtilities::EncryptPBTPin(unencryptedPIN), expectedPin);
	}

	TEST(UtilitiesUnitTests, UtilitiesUnencryptPBTPinTest)
	{
		DWORD encryptedPIN(10762);
		DWORD expectedPIN(1234);
		ASSERT_EQ(CUtilities::UnencryptPBTPin(encryptedPIN), expectedPIN);

		// Encrypt then unencrypt PIN expect same number			
		expectedPIN = 5678;
		ASSERT_EQ(CUtilities::UnencryptPBTPin(CUtilities::EncryptPBTPin(expectedPIN)), expectedPIN);
	}

	TEST(UtilitiesUnitTests, UtilitiesTrimLeftTest)
	{
		const std::string str = "random string";

		std::string str1 = "      " + str;
		CUtilities::TrimLeft(str1);
		ASSERT_EQ(str1, str);

		str1 = "  \t   " + str;
		CUtilities::TrimLeft(str1);
		ASSERT_EQ(str1, str);

		str1 = "  \n   " + str;
		CUtilities::TrimLeft(str1);
		ASSERT_EQ(str1, str);

		str1 = "  \t\f\n\r\v " + str;
		CUtilities::TrimLeft(str1);
		ASSERT_EQ(str1, str);

		str1 = "  \t\f\n\r\v ";
		CUtilities::TrimLeft(str1);
		ASSERT_EQ(str1, "");
	}

	TEST(UtilitiesUnitTests, UtilitiesTrimRightTest)
	{
		const std::string str = "random string";

		std::string str1 = str + "      ";
		CUtilities::TrimRight(str1);
		ASSERT_EQ(str1, str);

		str1 = str + "  \t   ";
		CUtilities::TrimRight(str1);
		ASSERT_EQ(str1, str);

		str1 = str + "  \n   ";
		CUtilities::TrimRight(str1);
		ASSERT_EQ(str1, str);

		str1 = str + "  \t\f\n\r\v ";
		CUtilities::TrimRight(str1);
		ASSERT_EQ(str1, str);

		str1 = "  \t\f\n\r\v ";
		CUtilities::TrimRight(str1);
		ASSERT_EQ(str1, "");
	}

	TEST(UtilitiesUnitTests, UtilitiesTrimTest)
	{
		const std::string str = "random string";

		std::string str1 = "      " + str + "      ";
		CUtilities::Trim(str1);
		ASSERT_EQ(str1, str);

		str1 = "  \n   " + str + "  \t   ";
		CUtilities::Trim(str1);
		ASSERT_EQ(str1, str);

		str1 = "  \t   " + str + "  \n   ";
		CUtilities::Trim(str1);
		ASSERT_EQ(str1, str);

		str1 = "  \t\f\n\r\v " + str + "  \t\f\n\r\v ";
		CUtilities::Trim(str1);
		ASSERT_EQ(str1, str);

		str1 = "  \t\f\n\r\v ";
		CUtilities::Trim(str1);
		ASSERT_EQ(str1, "");
	}

	TEST(UtilitiesUnitTests, UtilitiesIsBannedPinTest)
	{
		ASSERT_TRUE(CUtilities::IsBannedPin("7295"));
		ASSERT_FALSE(CUtilities::IsBannedPin("1111"));
	}

	TEST(UtilitiesUnitTests, UtilitiestrimLeftTest)
	{
		const std::string str = "random string";

		std::string str1 = "      " + str;
		str1 = CUtilities::trimLeft(str1);
		ASSERT_EQ(str1, str);

		str1 = "  \t   " + str;
		str1 = CUtilities::trimLeft(str1);
		ASSERT_EQ(str1, str);

		str1 = "  \n   " + str;
		str1 = CUtilities::trimLeft(str1);
		ASSERT_EQ(str1, str);

		str1 = "  \t\f\n\r\v " + str;
		str1 = CUtilities::trimLeft(str1);
		ASSERT_EQ(str1, str);

		str1 = "  \t\f\n\r\v ";
		str1 = CUtilities::trimLeft(str1);
		ASSERT_EQ(str1, "");
	}

	TEST(UtilitiesUnitTests, UtilitiesFindAndReplaceTest)
	{
		std::string str1 = "This is the string.";
		CUtilities::FindAndReplace(str1, "the", "a");
		ASSERT_EQ(str1, "This is a string.");

		str1 = "The glass is smashproof.";
		CUtilities::FindAndReplace(str1, "smash", "bullet");
		ASSERT_EQ(str1, "The glass is bulletproof.");

		str1 = "This is an empty string.";
		CUtilities::FindAndReplace(str1, "an empty", "a non-empty");
		ASSERT_EQ(str1, "This is a non-empty string.");

		str1 = "String not found.";
		CUtilities::FindAndReplace(str1, "string", "STRING");
		ASSERT_EQ(str1, "String not found.");

		str1 = "The trees are in the shadow of the building in the city.";
		CUtilities::FindAndReplace(str1, "the", "THE");
		ASSERT_EQ(str1, "The trees are in THE shadow of THE building in THE city.");
	}

	TEST(UtilitiesUnitTests, UtilitiesFixDirDelimiterTest)
	{
		std::string path = "c:\\files\\folders\\\\abc\\dir";
		std::string expectedPath = "c:/files/folders//abc/dir";
		CUtilities::FixDirDelimiter(path);
		ASSERT_EQ(path, expectedPath);

		path = "c:/files/folder";
		expectedPath = "c:/files/folder";
		CUtilities::FixDirDelimiter(path);
		ASSERT_EQ(path, expectedPath);
	}

	TEST(UtilitiesUnitTests, UtilitiesStringComparePredTest)
	{
		ASSERT_TRUE(CUtilities::StringComparePred('a','a'));
		ASSERT_TRUE(CUtilities::StringComparePred('a','A'));
		ASSERT_TRUE(CUtilities::StringComparePred('A','a'));
		ASSERT_TRUE(CUtilities::StringComparePred('A','A'));
		ASSERT_FALSE(CUtilities::StringComparePred('x','y'));
	}

	TEST(UtilitiesUnitTests, UtilitiesStringCompareIgnoreCaseTest)
	{
		std::string str1 = "This is a string.";
		std::string str2 = "this is A STRING.";
		ASSERT_TRUE(CUtilities::StringCompareIgnoreCase(str1, str2));

		str1 = "This is a string.";
		str2 = "This is a string";
		ASSERT_FALSE(CUtilities::StringCompareIgnoreCase(str1, str2));
	}

	TEST(UtilitiesUnitTests, UtilitiesCompareStringsNoCaseTest)
	{
		std::string str1 = "ABcde";
		std::string str2 = "abcDe";
		int result = CUtilities::CompareStringsNoCase(str1, str2);
		ASSERT_TRUE(result == 0);

		str1 = "ABcdef";
		str2 = "abcDe";
		result = CUtilities::CompareStringsNoCase(str1, str2);
		ASSERT_TRUE(result > 0);

		str1 = "ABcde";
		str2 = "abcDefg";
		result = CUtilities::CompareStringsNoCase(str1, str2);
		ASSERT_TRUE(result < 0);

		str1 = "";
		str2 = "";
		result = CUtilities::CompareStringsNoCase(str1, str2);
		ASSERT_TRUE(result == 0);
	}

	TEST(UtilitiesUnitTests, UtilitiesMakeLowerTest)
	{
		std::string str = "ABcd e";
		std::string expectedStr = "abcd e";
		CUtilities::MakeLower(str);
		ASSERT_EQ(str, expectedStr);

		str = "ab cde";
		expectedStr = "ab cde";
		CUtilities::MakeLower(str);
		ASSERT_EQ(str, expectedStr);

		str = "ABCDE 12345 @@#$";
		expectedStr = "abcde 12345 @@#$";
		CUtilities::MakeLower(str);
		ASSERT_EQ(str, expectedStr);

		str = "";
		expectedStr = "";
		CUtilities::MakeLower(str);
		ASSERT_EQ(str, expectedStr);
	}

	TEST(UtilitiesUnitTests, PathIsSlashTest)
	{
		ASSERT_TRUE(CUtilities::PathIsSlash('/'));
		ASSERT_FALSE(CUtilities::PathIsSlash('\\'));
	}

	TEST(UtilitiesUnitTests, PathIsDelimeterTest)
	{
		ASSERT_TRUE(CUtilities::PathIsDelimeter(':'));
		ASSERT_FALSE(CUtilities::PathIsDelimeter('|'));
	}

	TEST(UtilitiesUnitTests, SetStringSliderValueTest)
	{
		ASSERT_TRUE(CUtilities::SetStringSliderValue(0) == "");
		ASSERT_TRUE(CUtilities::SetStringSliderValue(1) == " ");
		ASSERT_TRUE(CUtilities::SetStringSliderValue(2) == "  ");
		ASSERT_TRUE(CUtilities::SetStringSliderValue(3) == "   ");
		ASSERT_TRUE(CUtilities::SetStringSliderValue(4) == "    ");
		ASSERT_TRUE(CUtilities::SetStringSliderValue(5) == "     ");
	}

	TEST(UtilitiesUnitTests, FormatsNumberWithCommas) {
		std::string input = "1234567.89";
		std::string expectedOutput = "1,234,567.89";
		std::string actualOutput = CUtilities::GetFormattedNumberString(input);
		EXPECT_EQ(expectedOutput, actualOutput);
	}

	TEST(UtilitiesUnitTests, HandlesNegativeNumbers) {
		std::string input = "-9876543.21";
		std::string expectedOutput = "-9,876,543.21";
		std::string actualOutput = CUtilities::GetFormattedNumberString(input);
		EXPECT_EQ(expectedOutput, actualOutput);
	}

	TEST(UtilitiesUnitTests, HandlesNoDecimalPart) {
		std::string input = "987654321";
		std::string expectedOutput = "987,654,321";
		std::string actualOutput = CUtilities::GetFormattedNumberString(input);
		EXPECT_EQ(expectedOutput, actualOutput);
	}

	TEST(UtilitiesUnitTests, HandlesEmptyString) {
		std::string input = "";
		std::string expectedOutput = "";
		std::string actualOutput = CUtilities::GetFormattedNumberString(input);
		EXPECT_EQ(expectedOutput, actualOutput);
	}
}
