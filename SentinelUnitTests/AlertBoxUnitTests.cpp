#include "stdafx.h"
#include "GlobalDefs.h"
#include "gtest/gtest.h"
#include "UI/AlertBox.h"

// Unit testing includes.
#include "UnitTestingOperatingSystem.h"
#include "UnitTestingUtils.h"

// Unit testing includes.
using namespace std;

// AssertResult macros for calling CUnitTestingUtils from a test method.
#define AssertResult3(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, m_testIndex, __TFUNCTION__, __LINE__);
#define AssertResult2(result, expected) AssertResult3(result, expected, _T(#result))

namespace SentinelNativeUnitTests
{
	// Test data.
	static const DWORD DWORD_TEST_DATA[] =
	{
		// Smaller values.
		0,
		1,
		1234567890,
		0x12345678,

		// Signed data thresholds.
		DWORD_MAX / 2 - 1, // INT32_MAX - 1.
		DWORD_MAX / 2, // INT32_MAX.
		DWORD_MAX / 2 + 1, // INT32_MIN.
		DWORD_MAX / 2 + 2, // INT32_MIN + 1.

		// Larger values.
		(DWORD)-1234567890,
		0xFEDCBA98,
		DWORD_MAX - 1,
		DWORD_MAX,
	};

	static const string CSTRING_TEST_DATA[] =
	{
		// Leading and/or trailing whitespace or lack thereof.
		_T(""),
		_T(" "),
		_T("  "),
		_T(" ab "),

		_T("abc "),
		_T("abc\t "),
		_T("abc \t"),
		_T("abc\n "),
		_T("abc \n"),

		_T("abc def"),
		_T("abc\t def"),
		_T("abc \tdef"),
		_T("abc\n def"),
		_T("abc \ndef"),

		_T(" def"),
		_T("\t def"),
		_T(" \tdef"),
		_T("\n def"),
		_T(" \ndef"),

		// All displayable ASCII characters, with imbedded quote.
		_T(" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"),

		// All displayable ASCII characters, with trailing quote.
		_T(" !#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\""),

		// 300 characters.
		_T("123456789.123456789.123456789.123456789.123456789.") _T("123456789.123456789.123456789.123456789.123456789.")
		_T("123456789.123456789.123456789.123456789.123456789.") _T("123456789.123456789.123456789.123456789.123456789.")
		_T("123456789.123456789.123456789.123456789.123456789.") _T("123456789.123456789.123456789.123456789.123456789."),
	};

	// Default expected result(s).
	const DWORD m_startingUpTickCountExpected = 5000;
	const bool m_startingUpExpected = false;
	const bool m_rebootingExpected = false;
	const bool m_systemBusyExpected = false;
	const string m_progressiveTextExpected;
	const string m_getTextExpected;
	const bool m_checkTimedEventsExpected = false;

	// Non-default expected results.
	const string m_startingUpMemoryOkTextExpected(_T("Rebooted...Memory OK."));
	const string m_startingUpMemoryNotOkTextExpected(_T("Rebooted...Memory error."));
	const string m_rebootingTextExpected(_T("Reboot in progress..."));
	const string m_systemBusyTextExpected(_T("System Busy"));
	const string m_progressiveDownTextExpected(_T("NOT participating in one or more Progressive promotion(s)."));
	const string m_eligibilityCannotBeDeterminedTextExpected(_T("Bonus eligibility not determined. Please reinsert card."));

	// Class member(s) used by test (data) methods.
	int m_testIndex = 0;

	/// <summary>
	/// Initialize and/or test static class members.
	/// </summary>
	TEST(AlertBoxUnitTests, ClassInitialize)
	{
		// Test static default value(s).
		ASSERT_TRUE(m_startingUpTickCountExpected == 5000);
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CAlertBox::CAlertBox()
	///     CAlertBox::GetText()
	///     CAlertBox::CheckTimedEvents()
	/// Code coverage:
	///     CAlertBox::~CAlertBox()
	/// Test for correct default values.
	/// </summary>
	TEST(AlertBoxUnitTests, CAlertBoxTest)
	{
		// Perform operation(s) to be tested.
		CAlertBox varObj;
		const CAlertBox &obj(varObj);

		// Test(s).
		AssertResult2(obj.GetText(), m_getTextExpected);

		AssertResult2(varObj.CheckTimedEvents(), m_checkTimedEventsExpected);
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CAlertBox::CAlertBox()
	///     CAlertBox::SetStartingUp(true)
	///     CAlertBox::GetText()
	/// </summary>
	TEST(AlertBoxUnitTests, SetStartingUpMemoryOkTest)
	{
		// Setup.
		COperatingSystemTest operatingSystem; // So we can control the tick count.
		CAlertBox obj(operatingSystem);

		// Perform operation(s) and test(s).
		obj.SetStartingUp(true);
		AssertResult2(obj.GetText(), m_startingUpMemoryOkTextExpected);
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CAlertBox::CAlertBox()
	///     CAlertBox::SetStartingUp(false)
	///     CAlertBox::SetStartingUp(true)
	///     CAlertBox::GetText()
	/// </summary>
	TEST(AlertBoxUnitTests, SetStartingUpMemoryOk2Test)
	{
		// Setup.
		COperatingSystemTest operatingSystem; // So we can control the tick count.
		CAlertBox obj(operatingSystem);

		// Perform operation(s) and test(s).
		obj.SetStartingUp(false);
		AssertResult2(obj.GetText(), m_startingUpMemoryNotOkTextExpected);
		obj.SetStartingUp(true);
		AssertResult2(obj.GetText(), m_startingUpMemoryOkTextExpected);
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CAlertBox::CAlertBox()
	///     CAlertBox::SetStartingUp(false)
	///     CAlertBox::GetText()
	/// </summary>
	TEST(AlertBoxUnitTests, SetStartingUpMemoryNotOkTest)
	{
		// Setup.
		COperatingSystemTest operatingSystem; // So we can control the tick count.
		CAlertBox obj(operatingSystem);

		// Perform operation(s) and test(s).
		obj.SetStartingUp(false);
		AssertResult2(obj.GetText(), m_startingUpMemoryNotOkTextExpected);
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CAlertBox::CAlertBox()
	///     CAlertBox::SetStartingUp(true)
	///     CAlertBox::SetStartingUp(false)
	///     CAlertBox::GetText()
	/// </summary>
	TEST(AlertBoxUnitTests, SetStartingUpMemoryNotOk2Test)
	{
		// Setup.
		COperatingSystemTest operatingSystem; // So we can control the tick count.
		CAlertBox obj(operatingSystem);

		// Perform operation(s) and test(s).
		obj.SetStartingUp(true);
		AssertResult2(obj.GetText(), m_startingUpMemoryOkTextExpected);
		obj.SetStartingUp(false);
		AssertResult2(obj.GetText(), m_startingUpMemoryNotOkTextExpected);
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CAlertBox::CAlertBox()
	///     CAlertBox::SetStartingUp(true)
	///     CAlertBox::CheckTimedEvents()
	///     CAlertBox::GetText()
	/// </summary>
	TEST(AlertBoxUnitTests, SetStartingUpMemoryOkTimerMinTest)
	{
		// Data.
		typedef DWORD TestDataType[(_countof(DWORD_TEST_DATA))];
		const TestDataType &TEST_DATA(DWORD_TEST_DATA);

		// Setup.  Use the same object for each test.
		COperatingSystemTest operatingSystemA; // So we can control the tick count.
		CAlertBox objA(operatingSystemA);

		for (; m_testIndex < _countof(TEST_DATA); m_testIndex++)
		{
			// Perform operation(s) and test(s).
			operatingSystemA.m_tickCount32 = TEST_DATA[m_testIndex];
			objA.SetStartingUp(true);
			AssertResult2(objA.GetText(), m_startingUpMemoryOkTextExpected);
			AssertResult2(objA.CheckTimedEvents(), m_checkTimedEventsExpected);
			AssertResult2(objA.GetText(), m_startingUpMemoryOkTextExpected);

			operatingSystemA.m_tickCount32 = TEST_DATA[m_testIndex] + m_startingUpTickCountExpected - 1;
			AssertResult2(objA.GetText(), m_startingUpMemoryOkTextExpected);
			AssertResult2(objA.CheckTimedEvents(), m_checkTimedEventsExpected);
			AssertResult2(objA.GetText(), m_startingUpMemoryOkTextExpected);

			// Expire the timer by the minimum amount.
			operatingSystemA.m_tickCount32 = TEST_DATA[m_testIndex] + m_startingUpTickCountExpected;
			AssertResult2(objA.GetText(), m_startingUpMemoryOkTextExpected);
			AssertResult2(objA.CheckTimedEvents(), !m_checkTimedEventsExpected);
			AssertResult2(objA.GetText(), m_getTextExpected);

			// Setup.  Use a different object for each test sequence.
			COperatingSystemTest operatingSystemB; // So we can control the tick count.
			CAlertBox objB(operatingSystemB);

			// Perform operation(s) and test(s).
			operatingSystemB.m_tickCount32 = TEST_DATA[m_testIndex];
			objB.SetStartingUp(true);
			AssertResult2(objB.GetText(), m_startingUpMemoryOkTextExpected);
			AssertResult2(objB.CheckTimedEvents(), m_checkTimedEventsExpected);
			AssertResult2(objB.GetText(), m_startingUpMemoryOkTextExpected);

			operatingSystemB.m_tickCount32 = TEST_DATA[m_testIndex] + m_startingUpTickCountExpected - 1;
			AssertResult2(objB.GetText(), m_startingUpMemoryOkTextExpected);
			AssertResult2(objB.CheckTimedEvents(), m_checkTimedEventsExpected);
			AssertResult2(objB.GetText(), m_startingUpMemoryOkTextExpected);

			// Expire the timer by the minimum amount.
			operatingSystemB.m_tickCount32 = TEST_DATA[m_testIndex] + m_startingUpTickCountExpected;
			AssertResult2(objB.GetText(), m_startingUpMemoryOkTextExpected);
			AssertResult2(objB.CheckTimedEvents(), !m_checkTimedEventsExpected);
			AssertResult2(objB.GetText(), m_getTextExpected);
		}
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CAlertBox::CAlertBox()
	///     CAlertBox::SetStartingUp(true)
	///     CAlertBox::CheckTimedEvents()
	///     CAlertBox::GetText()
	/// </summary>
	TEST(AlertBoxUnitTests, SetStartingUpMemoryNotOkTimerMaxTest)
	{
		// Data.
		typedef DWORD TestDataType[(_countof(DWORD_TEST_DATA))];
		const TestDataType &TEST_DATA(DWORD_TEST_DATA);

		// Setup.  Use the same object for each test.
		COperatingSystemTest operatingSystemA; // So we can control the tick count.
		CAlertBox objA(operatingSystemA);

		for (; m_testIndex < _countof(TEST_DATA); m_testIndex++)
		{
			// Perform operation(s) and test(s).
			operatingSystemA.m_tickCount32 = TEST_DATA[m_testIndex];
			objA.SetStartingUp(false);
			AssertResult2(objA.GetText(), m_startingUpMemoryNotOkTextExpected);
			AssertResult2(objA.CheckTimedEvents(), m_checkTimedEventsExpected);
			AssertResult2(objA.GetText(), m_startingUpMemoryNotOkTextExpected);

			operatingSystemA.m_tickCount32 = TEST_DATA[m_testIndex] + m_startingUpTickCountExpected - 1;
			AssertResult2(objA.GetText(), m_startingUpMemoryNotOkTextExpected);
			AssertResult2(objA.CheckTimedEvents(), m_checkTimedEventsExpected);
			AssertResult2(objA.GetText(), m_startingUpMemoryNotOkTextExpected);

			// Expire the timer by the maximum amount.
			operatingSystemA.m_tickCount32 = (DWORD)(TEST_DATA[m_testIndex] - 1);
			AssertResult2(objA.GetText(), m_startingUpMemoryNotOkTextExpected);
			AssertResult2(objA.CheckTimedEvents(), !m_checkTimedEventsExpected);
			AssertResult2(objA.GetText(), m_getTextExpected);

			// Setup.  Use a different object for each test sequence.
			COperatingSystemTest operatingSystemB; // So we can control the tick count.
			CAlertBox objB(operatingSystemB);

			// Perform operation(s) and test(s).
			operatingSystemB.m_tickCount32 = TEST_DATA[m_testIndex];
			objB.SetStartingUp(false);
			AssertResult2(objB.GetText(), m_startingUpMemoryNotOkTextExpected);
			AssertResult2(objB.CheckTimedEvents(), m_checkTimedEventsExpected);
			AssertResult2(objB.GetText(), m_startingUpMemoryNotOkTextExpected);

			operatingSystemB.m_tickCount32 = TEST_DATA[m_testIndex] + m_startingUpTickCountExpected - 1;
			AssertResult2(objB.GetText(), m_startingUpMemoryNotOkTextExpected);
			AssertResult2(objB.CheckTimedEvents(), m_checkTimedEventsExpected);
			AssertResult2(objB.GetText(), m_startingUpMemoryNotOkTextExpected);

			// Expire the timer by the maximum amount.
			operatingSystemB.m_tickCount32 = (DWORD)(TEST_DATA[m_testIndex] - 1);
			AssertResult2(objB.GetText(), m_startingUpMemoryNotOkTextExpected);
			AssertResult2(objB.CheckTimedEvents(), !m_checkTimedEventsExpected);
			AssertResult2(objB.GetText(), m_getTextExpected);
		}
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CAlertBox::CAlertBox()
	///     CAlertBox::SetStartingUp(true)
	///     CAlertBox::CheckTimedEvents()
	///     CAlertBox::GetText()
	/// Test CheckTimedEvents with one call to SetStartingUp.
	/// Test that updating the operating system tick count does not change the state until CheckTimedEvents is called.
	/// </summary>
	TEST(AlertBoxUnitTests, CheckTimedEvents1Test)
	{
		// Setup.
		COperatingSystemTest operatingSystem; // So we can control the tick count.
		CAlertBox obj(operatingSystem);

		// Perform operation(s) and test(s).
		operatingSystem.m_tickCount32 = 0;
		obj.SetStartingUp(true);
		AssertResult2(obj.GetText(), m_startingUpMemoryOkTextExpected);
		AssertResult2(obj.CheckTimedEvents(), m_checkTimedEventsExpected);
		AssertResult2(obj.GetText(), m_startingUpMemoryOkTextExpected);

		operatingSystem.m_tickCount32 = m_startingUpTickCountExpected - 1;
		AssertResult2(obj.GetText(), m_startingUpMemoryOkTextExpected);
		AssertResult2(obj.CheckTimedEvents(), m_checkTimedEventsExpected);
		AssertResult2(obj.GetText(), m_startingUpMemoryOkTextExpected);

		// Expire the timer by the minimum amount.
		operatingSystem.m_tickCount32 = m_startingUpTickCountExpected;
		AssertResult2(obj.GetText(), m_startingUpMemoryOkTextExpected);
		AssertResult2(obj.CheckTimedEvents(), !m_checkTimedEventsExpected);
		AssertResult2(obj.GetText(), m_getTextExpected);
		AssertResult2(obj.CheckTimedEvents(), m_checkTimedEventsExpected);
		AssertResult2(obj.GetText(), m_getTextExpected);

		// Expire the timer by even more.
		operatingSystem.m_tickCount32 = 3 * m_startingUpTickCountExpected;
		AssertResult2(obj.CheckTimedEvents(), m_checkTimedEventsExpected);
		AssertResult2(obj.GetText(), m_getTextExpected);
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CAlertBox::CAlertBox()
	///     CAlertBox::SetStartingUp(true)
	///     CAlertBox::CheckTimedEvents()
	///     CAlertBox::GetText()
	/// Test CheckTimedEvents with two calls to SetStartingUp.
	/// Test that updating the operating system tick count does not change the state until CheckTimedEvents is called.
	/// </summary>
	TEST(AlertBoxUnitTests, CheckTimedEvents2Test)
	{
		// Data.
		const DWORD startingUpTickCount2(m_startingUpTickCountExpected / 2);

		// Setup.
		COperatingSystemTest operatingSystem; // So we can control the tick count.
		CAlertBox obj(operatingSystem);

		// Perform operation(s) and test(s).
		operatingSystem.m_tickCount32 = 0;
		obj.SetStartingUp(true);
		AssertResult2(obj.GetText(), m_startingUpMemoryOkTextExpected);
		AssertResult2(obj.CheckTimedEvents(), m_checkTimedEventsExpected);
		AssertResult2(obj.GetText(), m_startingUpMemoryOkTextExpected);

		// Call SetStartingUp() again.
		operatingSystem.m_tickCount32 = startingUpTickCount2;
		obj.SetStartingUp(false);
		AssertResult2(obj.GetText(), m_startingUpMemoryNotOkTextExpected);
		AssertResult2(obj.CheckTimedEvents(), m_checkTimedEventsExpected);
		AssertResult2(obj.GetText(), m_startingUpMemoryNotOkTextExpected);

		// Expire the original (overwritten) timer by the minimum amount.
		operatingSystem.m_tickCount32 = m_startingUpTickCountExpected;
		AssertResult2(obj.GetText(), m_startingUpMemoryNotOkTextExpected);
		AssertResult2(obj.CheckTimedEvents(), m_checkTimedEventsExpected);
		AssertResult2(obj.GetText(), m_startingUpMemoryNotOkTextExpected);

		// Test maximum tick count that does not expire the timer.
		operatingSystem.m_tickCount32 = startingUpTickCount2 + m_startingUpTickCountExpected - 1;
		AssertResult2(obj.GetText(), m_startingUpMemoryNotOkTextExpected);
		AssertResult2(obj.CheckTimedEvents(), m_checkTimedEventsExpected);
		AssertResult2(obj.GetText(), m_startingUpMemoryNotOkTextExpected);

		// Expire the timer by the minimum amount.
		operatingSystem.m_tickCount32 = startingUpTickCount2 + m_startingUpTickCountExpected;
		AssertResult2(obj.GetText(), m_startingUpMemoryNotOkTextExpected);
		AssertResult2(obj.CheckTimedEvents(), !m_checkTimedEventsExpected);
		AssertResult2(obj.GetText(), m_getTextExpected);
		AssertResult2(obj.CheckTimedEvents(), m_checkTimedEventsExpected);
		AssertResult2(obj.GetText(), m_getTextExpected);

		// Expire the timer by even more.
		operatingSystem.m_tickCount32 = startingUpTickCount2 + 3 * m_startingUpTickCountExpected;
		AssertResult2(obj.CheckTimedEvents(), m_checkTimedEventsExpected);
		AssertResult2(obj.GetText(), m_getTextExpected);
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CAlertBox::CAlertBox()
	///     CAlertBox::SetRebooting()
	///     CAlertBox::GetText()
	/// </summary>
	TEST(AlertBoxUnitTests, SetRebootingTest)
	{
		// Setup.
		CAlertBox obj;

		// Perform operation(s) and test(s).
		obj.SetRebooting();
		AssertResult2(obj.GetText(), m_rebootingTextExpected);
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CAlertBox::CAlertBox()
	///     CAlertBox::SetStartingUp(true)
	///     CAlertBox::SetRebooting()
	///     CAlertBox::GetText()
	/// </summary>
	TEST(AlertBoxUnitTests, SetStartingUpSetRebootingTest)
	{
		// Setup.
		COperatingSystemTest operatingSystem; // So we can control the tick count.
		CAlertBox obj(operatingSystem);

		// Perform operation(s) and test(s).
		obj.SetStartingUp(true);
		AssertResult2(obj.GetText(), m_startingUpMemoryOkTextExpected);

		obj.SetRebooting();
		AssertResult2(obj.GetText(), m_rebootingTextExpected);
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CAlertBox::CAlertBox()
	///     CAlertBox::SetRebooting()
	///     CAlertBox::SetStartingUp(false)
	///     CAlertBox::GetText()
	/// </summary>
	TEST(AlertBoxUnitTests, SetRebootingSetStartingUpTest)
	{
		// Setup.
		COperatingSystemTest operatingSystem; // So we can control the tick count.
		CAlertBox obj(operatingSystem);

		// Perform operation(s) and test(s).
		obj.SetRebooting();
		AssertResult2(obj.GetText(), m_rebootingTextExpected);

		obj.SetStartingUp(false);
		AssertResult2(obj.GetText(), m_rebootingTextExpected);
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CAlertBox::CAlertBox()
	///     CAlertBox::SetSystemBusy()
	///     CAlertBox::GetText()
	/// </summary>
	TEST(AlertBoxUnitTests, SetSystemBusyTest)
	{
		// Setup.
		CAlertBox obj;

		// Perform operation(s) and test(s).
		obj.SetSystemBusy(true, _T("System Busy"));
		AssertResult2(obj.GetText(), m_systemBusyTextExpected);

		obj.SetSystemBusy(false, _T("System Busy"));
		AssertResult2(obj.GetText(), m_getTextExpected);
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CAlertBox::CAlertBox()
	///     CAlertBox::SetSystemBusy()
	///     CAlertBox::SetRebooting()
	///     CAlertBox::GetText()
	/// </summary>
	TEST(AlertBoxUnitTests, SetSystemBusySetRebootingTest)
	{
		// Setup.
		CAlertBox obj;

		// Perform operation(s) and test(s).
		obj.SetSystemBusy(true, _T("System Busy"));
		AssertResult2(obj.GetText(), m_systemBusyTextExpected);

		obj.SetRebooting();
		AssertResult2(obj.GetText(), m_rebootingTextExpected);

		obj.SetSystemBusy(false, _T("System Busy"));
		AssertResult2(obj.GetText(), m_rebootingTextExpected);
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CAlertBox::CAlertBox()
	///     CAlertBox::SetSystemBusy()
	///     CAlertBox::SetProgressiveStatus()
	///     CAlertBox::GetText()
	/// </summary>
	TEST(AlertBoxUnitTests, SetSystemBusySetProgressiveDownTest)
	{
		// Setup.
		CAlertBox obj;

		// Perform operation(s) and test(s).
		obj.SetSystemBusy(true, _T("System Busy"));
		AssertResult2(obj.GetText(), m_systemBusyTextExpected);

		u_ProgressiveCriticalText progressiveCriticalText(3);
		obj.SetProgressiveStatus(progressiveCriticalText);
		AssertResult2(obj.GetText(), m_progressiveDownTextExpected);

		progressiveCriticalText.m_data.progressiveEnabled = 0;
		progressiveCriticalText.m_data.progressiveDown = 0;
		obj.SetProgressiveStatus(progressiveCriticalText);
		AssertResult2(obj.GetText(), m_systemBusyTextExpected);
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CAlertBox::CAlertBox()
	///     CAlertBox::SetSystemBusy()
	///     CAlertBox::SetProgressiveStatus()
	///     CAlertBox::GetText()
	/// </summary>
	TEST(AlertBoxUnitTests, SetSystemBusySetProgressiveDownClearTest)
	{
		// Setup.
		CAlertBox obj;

		// Perform operation(s) and test(s).
		u_ProgressiveCriticalText progressiveCriticalText(3);
		obj.SetProgressiveStatus(progressiveCriticalText);

		obj.SetSystemBusy(true, _T("System Busy"));
		AssertResult2(obj.GetText(), m_progressiveDownTextExpected);

		progressiveCriticalText.m_data.progressiveEnabled = 0;
		progressiveCriticalText.m_data.progressiveDown = 0;
		obj.SetProgressiveStatus(progressiveCriticalText);
		AssertResult2(obj.GetText(), m_systemBusyTextExpected);
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CAlertBox::CAlertBox()
	///     CAlertBox::SetSystemBusy()
	///     CAlertBox::SetStartingUp(true)
	///     CAlertBox::CheckTimedEvents()
	///     CAlertBox::GetText()
	/// Test for starting up precedence over progressive text.
	/// </summary>
	TEST(AlertBoxUnitTests, SetSystemBusySetStartingUpTest)
	{
		// Setup.
		COperatingSystemTest operatingSystem; // So we can control the tick count.
		CAlertBox obj(operatingSystem);

		// Perform operation(s) and test(s).
		obj.SetSystemBusy(true, _T("System Busy"));
		AssertResult2(obj.GetText(), m_systemBusyTextExpected);

		operatingSystem.m_tickCount32 = 0;
		obj.SetStartingUp(true);
		AssertResult2(obj.GetText(), m_startingUpMemoryOkTextExpected);

		operatingSystem.m_tickCount32 = m_startingUpTickCountExpected;
		AssertResult2(obj.GetText(), m_startingUpMemoryOkTextExpected);
		AssertResult2(obj.CheckTimedEvents(), !m_checkTimedEventsExpected);
		AssertResult2(obj.GetText(), m_systemBusyTextExpected);
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CAlertBox::CAlertBox()
	///     CAlertBox::SetSystemBusy()
	///     CAlertBox::SetStartingUp(true)
	///     CAlertBox::CheckTimedEvents()
	///     CAlertBox::GetText()
	/// Test for clearing progressive text while starting up.
	/// </summary>
	TEST(AlertBoxUnitTests, SetSystemBusySetStartingUpClearTest)
	{
		// Setup.
		COperatingSystemTest operatingSystem; // So we can control the tick count.
		CAlertBox obj(operatingSystem);

		// Perform operation(s) and test(s).
		obj.SetSystemBusy(true, _T("System Busy"));
		AssertResult2(obj.GetText(), m_systemBusyTextExpected);

		operatingSystem.m_tickCount32 = 0;
		obj.SetStartingUp(true);
		AssertResult2(obj.GetText(), m_startingUpMemoryOkTextExpected);

		obj.SetSystemBusy(false, _T("System Busy"));
		AssertResult2(obj.GetText(), m_startingUpMemoryOkTextExpected);

		operatingSystem.m_tickCount32 = m_startingUpTickCountExpected;
		AssertResult2(obj.GetText(), m_startingUpMemoryOkTextExpected);
		AssertResult2(obj.CheckTimedEvents(), !m_checkTimedEventsExpected);
		AssertResult2(obj.GetText(), m_getTextExpected);
	}


	/// <summary>
	/// Code coverage and functional test for:
	///     CAlertBox::CAlertBox()
	///     CAlertBox::SetProgressiveStatus()
	///     CAlertBox::SetRebooting()
	///     CAlertBox::GetText()
	/// </summary>
	TEST(AlertBoxUnitTests, SetProgressiveDownSetRebootingTest)
	{
		// Setup.
		CAlertBox obj;

		// Perform operation(s) and test(s).
		u_ProgressiveCriticalText progressiveCriticalText(3);
		obj.SetProgressiveStatus(progressiveCriticalText);
		AssertResult2(obj.GetText(), m_progressiveDownTextExpected);

		obj.SetRebooting();
		AssertResult2(obj.GetText(), m_rebootingTextExpected);
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CAlertBox::CAlertBox()
	///     CAlertBox::SetRebooting()
	///     CAlertBox::SetProgressiveStatus()
	///     CAlertBox::GetText()
	/// </summary>
	TEST(AlertBoxUnitTests, SetRebootingSetProgressiveDownTest)
	{
		// Setup.
		CAlertBox obj;

		// Perform operation(s) and test(s).
		obj.SetRebooting();
		AssertResult2(obj.GetText(), m_rebootingTextExpected);

		u_ProgressiveCriticalText progressiveCriticalText(3);
		obj.SetProgressiveStatus(progressiveCriticalText);
		AssertResult2(obj.GetText(), m_rebootingTextExpected);
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CAlertBox::CAlertBox()
	///     CAlertBox::SetRebooting()
	///     CAlertBox::SetSystemBusy(true)
	///     CAlertBox::GetText()
	/// </summary>
	TEST(AlertBoxUnitTests, SetRebootingSetSystemBusyTest)
	{
		// Setup.
		CAlertBox obj;

		// Perform operation(s) and test(s).
		obj.SetRebooting();
		AssertResult2(obj.GetText(), m_rebootingTextExpected);

		obj.SetSystemBusy(true, _T("System Busy"));
		AssertResult2(obj.GetText(), m_rebootingTextExpected);
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CAlertBox::CAlertBox()
	///     CAlertBox::SetProgressiveStatus()
	///     CAlertBox::GetText()
	/// </summary>
	TEST(AlertBoxUnitTests, SetProgressiveDownTest)
	{
		// Setup.
		CAlertBox obj;

		// Perform operation(s) and test(s).
		u_ProgressiveCriticalText progressiveCriticalText(3);
		obj.SetProgressiveStatus(progressiveCriticalText);
		AssertResult2(obj.GetText(), m_progressiveDownTextExpected);

		progressiveCriticalText.m_data.progressiveEnabled = 0;
		progressiveCriticalText.m_data.progressiveDown = 0;
		obj.SetProgressiveStatus(progressiveCriticalText);
		AssertResult2(obj.GetText(), m_getTextExpected);
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CAlertBox::CAlertBox()
	///     CAlertBox::SetProgressiveStatus()
	///     CAlertBox::GetText()
	/// </summary>
	TEST(AlertBoxUnitTests, SetEligibilityCannotBeDeterminedTest)
	{
		// Setup.
		CAlertBox obj;

		// Perform operation(s) and test(s).
		u_ProgressiveCriticalText progressiveCriticalText(15);
		obj.SetProgressiveStatus(progressiveCriticalText);
		AssertResult2(obj.GetText(), m_progressiveDownTextExpected);

		progressiveCriticalText.m_data.progressiveEnabled = 0;
		obj.SetProgressiveStatus(progressiveCriticalText);
		AssertResult2(obj.GetText(), m_getTextExpected);

		progressiveCriticalText.m_data.progressiveEnabled = 1;
		progressiveCriticalText.m_data.progressiveDown = 0;
		progressiveCriticalText.m_data.eligibilityDetermined = 0;
		obj.SetProgressiveStatus(progressiveCriticalText);
		AssertResult2(obj.GetText(), m_eligibilityCannotBeDeterminedTextExpected);
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CAlertBox::CAlertBox()
	///     CAlertBox::SetProgressiveStatus()
	///     CAlertBox::GetText()
	/// </summary>
	TEST(AlertBoxUnitTests, SetProgressiveDownNullTest)
	{
		// Setup.
		CAlertBox obj;

		// Perform operation(s) and test(s).
		u_ProgressiveCriticalText progressiveCriticalText(3);
		obj.SetProgressiveStatus(progressiveCriticalText);
		AssertResult2(obj.GetText(), m_progressiveDownTextExpected);

		progressiveCriticalText.m_data.progressiveEnabled = 0;
		progressiveCriticalText.m_data.progressiveDown = 0;
		obj.SetProgressiveStatus(progressiveCriticalText);
		AssertResult2(obj.GetText(), m_getTextExpected);
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CAlertBox::CAlertBox()
	///     CAlertBox::SetProgressiveStatus()
	///     CAlertBox::SetStartingUp(true)
	///     CAlertBox::CheckTimedEvents()
	///     CAlertBox::GetText()
	/// Test for starting up precedence over progressive text.
	/// </summary>
	TEST(AlertBoxUnitTests, SetProgressiveDownSetStartingUpTest)
	{
		// Setup.
		COperatingSystemTest operatingSystem; // So we can control the tick count.
		CAlertBox obj(operatingSystem);

		// Perform operation(s) and test(s).
		u_ProgressiveCriticalText progressiveCriticalText(3);
		obj.SetProgressiveStatus(progressiveCriticalText);
		AssertResult2(obj.GetText(), m_progressiveDownTextExpected);

		operatingSystem.m_tickCount32 = 0;
		obj.SetStartingUp(true);
		AssertResult2(obj.GetText(), m_startingUpMemoryOkTextExpected);

		operatingSystem.m_tickCount32 = m_startingUpTickCountExpected;
		AssertResult2(obj.GetText(), m_startingUpMemoryOkTextExpected);
		AssertResult2(obj.CheckTimedEvents(), !m_checkTimedEventsExpected);
		AssertResult2(obj.GetText(), m_progressiveDownTextExpected);
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CAlertBox::CAlertBox()
	///     CAlertBox::SetProgressiveStatus()
	///     CAlertBox::SetStartingUp(true)
	///     CAlertBox::CheckTimedEvents()
	///     CAlertBox::GetText()
	/// Test for clearing progressive text while starting up.
	/// </summary>
	TEST(AlertBoxUnitTests, SetProgressiveDownSetStartingUpClearTest)
	{
		// Setup.
		COperatingSystemTest operatingSystem; // So we can control the tick count.
		CAlertBox obj(operatingSystem);

		// Perform operation(s) and test(s).
		u_ProgressiveCriticalText progressiveCriticalText(3);
		obj.SetProgressiveStatus(progressiveCriticalText);
		AssertResult2(obj.GetText(), m_progressiveDownTextExpected);

		operatingSystem.m_tickCount32 = 0;
		obj.SetStartingUp(true);
		AssertResult2(obj.GetText(), m_startingUpMemoryOkTextExpected);

		progressiveCriticalText.m_data.progressiveEnabled = 0;
		progressiveCriticalText.m_data.progressiveDown = 0;
		obj.SetProgressiveStatus(progressiveCriticalText);
		AssertResult2(obj.GetText(), m_startingUpMemoryOkTextExpected);

		operatingSystem.m_tickCount32 = m_startingUpTickCountExpected;
		AssertResult2(obj.GetText(), m_startingUpMemoryOkTextExpected);
		AssertResult2(obj.CheckTimedEvents(), !m_checkTimedEventsExpected);
		AssertResult2(obj.GetText(), m_getTextExpected);
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CAlertBox::CAlertBox()
	///     CAlertBox::SetProgressiveStatus()
	///     CAlertBox::SetStartingUp(true)
	///     CAlertBox::CheckTimedEvents()
	///     CAlertBox::GetText()
	/// Test for setting progressive text while starting up.
	/// </summary>
	TEST(AlertBoxUnitTests, SetStartingUpSetProgressiveDownTest)
	{
		// Setup.
		COperatingSystemTest operatingSystem; // So we can control the tick count.
		CAlertBox obj(operatingSystem);

		// Perform operation(s) and test(s).
		operatingSystem.m_tickCount32 = 0;
		obj.SetStartingUp(true);
		AssertResult2(obj.GetText(), m_startingUpMemoryOkTextExpected);

		u_ProgressiveCriticalText progressiveCriticalText(3);
		obj.SetProgressiveStatus(progressiveCriticalText);
		AssertResult2(obj.GetText(), m_startingUpMemoryOkTextExpected);

		operatingSystem.m_tickCount32 = m_startingUpTickCountExpected;
		AssertResult2(obj.GetText(), m_startingUpMemoryOkTextExpected);
		AssertResult2(obj.CheckTimedEvents(), !m_checkTimedEventsExpected);
		AssertResult2(obj.GetText(), m_progressiveDownTextExpected);
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CAlertBox::CAlertBox()
	///     CAlertBox::SetSystemBusy()
	///     CAlertBox::SetStartingUp(true)
	///     CAlertBox::CheckTimedEvents()
	///     CAlertBox::GetText()
	/// Test for setting progressive text while starting up.
	/// </summary>
	TEST(AlertBoxUnitTests, SetStartingUpSetSystemBusyTest)
	{
		// Setup.
		COperatingSystemTest operatingSystem; // So we can control the tick count.
		CAlertBox obj(operatingSystem);

		// Perform operation(s) and test(s).
		operatingSystem.m_tickCount32 = 0;
		obj.SetStartingUp(true);
		AssertResult2(obj.GetText(), m_startingUpMemoryOkTextExpected);

		obj.SetSystemBusy(true, _T("System Busy"));
		AssertResult2(obj.GetText(), m_startingUpMemoryOkTextExpected);

		operatingSystem.m_tickCount32 = m_startingUpTickCountExpected;
		AssertResult2(obj.GetText(), m_startingUpMemoryOkTextExpected);
		AssertResult2(obj.CheckTimedEvents(), !m_checkTimedEventsExpected);
		AssertResult2(obj.GetText(), m_systemBusyTextExpected);
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CAlertBox::CAlertBox()
	///     CAlertBox::SetProgressiveStatus()
	///     CAlertBox::SetSystemBusy()
	///     CAlertBox::GetText()
	/// </summary>
	TEST(AlertBoxUnitTests, SetProgressiveDownSetSystemBusyTest)
	{
		// Setup.
		CAlertBox obj;

		// Perform operation(s) and test(s).
		u_ProgressiveCriticalText progressiveCriticalText(3);
		obj.SetProgressiveStatus(progressiveCriticalText);
		AssertResult2(obj.GetText(), m_progressiveDownTextExpected);

		obj.SetSystemBusy(true, _T("System Busy"));
		AssertResult2(obj.GetText(), m_progressiveDownTextExpected);

		obj.SetSystemBusy(false, _T("System Busy"));
		AssertResult2(obj.GetText(), m_progressiveDownTextExpected);
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CAlertBox::CAlertBox()
	///     CAlertBox::SetProgressiveStatus()
	///     CAlertBox::SetSystemBusy()
	///     CAlertBox::GetText()
	/// </summary>
	TEST(AlertBoxUnitTests, SetProgressiveDownSetSystemBusyClearTest)
	{
		// Setup.
		CAlertBox obj;

		// Perform operation(s) and test(s).
		u_ProgressiveCriticalText progressiveCriticalText(3);
		obj.SetProgressiveStatus(progressiveCriticalText);
		AssertResult2(obj.GetText(), m_progressiveDownTextExpected);

		obj.SetSystemBusy(true, _T("System Busy"));
		AssertResult2(obj.GetText(), m_progressiveDownTextExpected);

		progressiveCriticalText.m_data.progressiveEnabled = 0;
		progressiveCriticalText.m_data.progressiveDown = 0;
		obj.SetProgressiveStatus(progressiveCriticalText);
		AssertResult2(obj.GetText(), m_systemBusyTextExpected);
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CAlertBox::CAlertBox()
	///     CAlertBox::SetBackgroundMeterSnapshotInProgress()
	///     CAlertBox::GetText()
	/// </summary>
	TEST(AlertBoxUnitTests, SetBackgroundMeterSnapshotInProgressTest)
	{
		// Setup.
		CAlertBox obj;		
		const string expectedText("Card reader disabled. Sub-game capture in progress (55%). Do not reboot.");

		// Perform operation(s) and test(s).
		obj.SetBackgroundMeterSnapshotInProgress(true, 55);

		// Test(s).
		AssertResult2(obj.GetText(), expectedText);
		obj.SetBackgroundMeterSnapshotInProgress(false, 55);
		AssertResult2(obj.GetText(), "");
	}
}