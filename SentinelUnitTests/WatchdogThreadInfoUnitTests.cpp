#include "stdafx.h"
#include "Hardware/WatchdogThreadInfo.h"

// Sentinel includes.
#include "GlobalDefs.h"
#include "Utilities.h"

// Unit testing includes.
#include "gtest/gtest.h"
#include "AssertMsg.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	class WatchdogThreadInfoUnitTests : public ::testing::Test
	{
    public:
		// Data.
		static const string m_nameDefault;
		static const DWORD m_timeOutMillisecondsDefault;
		static const DWORD m_checkInTimeDefault;
		static const bool m_isSuspendedDefault;
    };
		
		TEST_F(WatchdogThreadInfoUnitTests, WatchdogThreadInfoDefaultConstructorTest)
		{
			CWatchdogThreadInfo threadInfo;
			ASSERT_EQ(m_nameDefault, threadInfo.GetName()) << L"CWatchdogThreadInfo does not have an empty name.";
			ASSERT_FALSE(threadInfo.IsSuspended()) << L"CWatchdogThreadInfo has suspended state 'true'.";
			ASSERT_TRUE(threadInfo.HasTimedOut(CUtilities::GetTickCount())) << L"CWatchdogThreadInfo reports no timeout when timeout duration not yet set.";
		}

		TEST_F(WatchdogThreadInfoUnitTests, WatchdogThreadInfoGetSetNameTest)
		{
			CWatchdogThreadInfo threadInfo;
			ASSERT_EQ(m_nameDefault, threadInfo.GetName()) << L"CWatchdogThreadInfo does not have an empty name.";

			CWatchdogThreadInfo threadInfo2(_T("test"), 0, 0, 0);
			threadInfo = threadInfo2;
			ASSERT_EQ(string("test"), threadInfo.GetName()) << L"CWatchdogThreadInfo did not return the same name it was set to.";
		}

		TEST_F(WatchdogThreadInfoUnitTests, WatchdogThreadInfoCopyConstructorTest)
		{
			CWatchdogThreadInfo threadInfo(_T("test"), 0, 0, 0);
			threadInfo.Suspend();

			CWatchdogThreadInfo copied = threadInfo;
			ASSERT_EQ(string("test"), copied.GetName()) << L"Copied CWatchdogThreadInfo did not have the same name.";
			ASSERT_TRUE(copied.IsSuspended()) << L"Copied CWatchdogThreadInfo did not have the same suspended state.";
		}

		TEST_F(WatchdogThreadInfoUnitTests, WatchdogThreadInfoTimeoutTest)
		{
			CWatchdogThreadInfo threadInfo(_T(""), 10 * MILLISECONDS_PER_SECOND, CUtilities::GetTickCount(), 0);

			ASSERT_FALSE(threadInfo.HasTimedOut(CUtilities::GetTickCount())) << L"CWatchdogThreadInfo should not have timed out immediately.";

			CUtilities::Sleep(11 * 1000);

			ASSERT_TRUE(threadInfo.HasTimedOut(CUtilities::GetTickCount())) << L"CWatchdogThreadInfo not shown has timed out when time is passed.";
		}

		TEST_F(WatchdogThreadInfoUnitTests, WatchdogThreadInfoCheckinTest)
		{
			CWatchdogThreadInfo threadInfo(_T(""), 10 * MILLISECONDS_PER_SECOND, CUtilities::GetTickCount(), 0);
			ASSERT_FALSE(threadInfo.HasTimedOut(CUtilities::GetTickCount())) << L"CWatchdogThreadInfo should not have timed out immediately.";

			CUtilities::Sleep(6*1000);
			threadInfo.CheckIn(CUtilities::GetTickCount());
			CUtilities::Sleep(6*1000);
			ASSERT_FALSE(threadInfo.HasTimedOut(CUtilities::GetTickCount())) << L"CWatchdogThreadInfo shown as timed out when timely 'checkin' done.";

			threadInfo.CheckIn(CUtilities::GetTickCount()); // Now that we know checkin works, we use it to prepare next test.
			CUtilities::Sleep(1 *1000);
			threadInfo.CheckIn(CUtilities::GetTickCount());
			ASSERT_FALSE(threadInfo.HasTimedOut(CUtilities::GetTickCount())) << L"CWatchdogThreadInfo checkin did not reset time out.";
		}

		TEST_F(WatchdogThreadInfoUnitTests, WatchdogThreadInfoSuspendResumeTest)
		{
			CWatchdogThreadInfo threadInfo(_T(""), 0, CUtilities::GetTickCount(), 0);
			threadInfo.Suspend();

			ASSERT_TRUE(threadInfo.IsSuspended()) << L"CWatchdogThreadInfo not shown as suspended.";
			ASSERT_FALSE(threadInfo.HasTimedOut(CUtilities::GetTickCount())) << L"CWatchdogThreadInfo show as timed out while suspended..";

			threadInfo.Resume(CUtilities::GetTickCount());

			ASSERT_FALSE(threadInfo.IsSuspended()) << L"CWatchdogThreadInfo shown as suspended after 'resume'.";
			ASSERT_TRUE(threadInfo.HasTimedOut(CUtilities::GetTickCount())) << L"CWatchdogThreadInfo should timeout after resume and not timeout duration set.";

			CWatchdogThreadInfo threadInfo2(_T(""), 10 * MILLISECONDS_PER_SECOND, CUtilities::GetTickCount(), 0);
			threadInfo = threadInfo2;
			ASSERT_FALSE(threadInfo.HasTimedOut(CUtilities::GetTickCount())) << L"CWatchdogThreadInfo should not have timed out immediately.";

			threadInfo.Suspend();
			CUtilities::Sleep(11*1000);
			threadInfo.Resume(CUtilities::GetTickCount());
			ASSERT_FALSE(threadInfo.HasTimedOut(CUtilities::GetTickCount())) << L"CWatchdogThreadInfo resume did not reset timout out.";

			threadInfo.Suspend();
			CUtilities::Sleep(6*1000);
			threadInfo.Resume(CUtilities::GetTickCount());
			CUtilities::Sleep(6*1000);
			ASSERT_FALSE(threadInfo.HasTimedOut(CUtilities::GetTickCount())) << L"CWatchdogThreadInfo shown as timed out when timely 'resume' done.";
		}

// // #pragma region GetName
		/// <summary>
		/// Functional test for:
		///     CWatchdogThreadInfo::CWatchdogThreadInfo()
		///     CWatchdogThreadInfo::GetName()
		/// Code coverage for:
		///     CWatchdogThreadInfo::~CWatchdogThreadInfo()
		/// Test the value from the default constructor.
		/// </summary>
		TEST_F(WatchdogThreadInfoUnitTests, GetName_Default)
		{
			// Perform operation(s) and test(s).
			CWatchdogThreadInfo obj;
			ASSERT_EQ(m_nameDefault, obj.GetName()) << L"GetName()";
		}

		/// <summary>
		/// Functional test for:
		///     CWatchdogThreadInfo::CWatchdogThreadInfo(LPCTSTR, DWORD)
		///     CWatchdogThreadInfo::GetName()
		/// Test the value from the parameterized constructor.
		/// </summary>
		TEST_F(WatchdogThreadInfoUnitTests, GetName_Constructor)
		{
			// Data.
			const string name(_T("thread name"));
			const DWORD timeOutMilliseconds(100000);
			const DWORD tickCount(200000);

			// Perform operation(s) and test(s).
			CWatchdogThreadInfo obj(name, timeOutMilliseconds, tickCount, 0);
			ASSERT_EQ(name, obj.GetName()) << L"GetName()";
		}

		/// <summary>
		/// Functional test for:
		///     CWatchdogThreadInfo::CWatchdogThreadInfo(const CWatchdogThreadInfo);
		///     CWatchdogThreadInfo::GetName()
		/// Test the value from the copy constructor.
		/// </summary>
		TEST_F(WatchdogThreadInfoUnitTests, GetName_Copy)
		{
			// Data.
			const string name(_T("thread name"));
			const DWORD timeOutMilliseconds(100000);
			const DWORD tickCount(200000);

			// Setup.
			CWatchdogThreadInfo obj2(name, timeOutMilliseconds, tickCount, 0);

			// Perform operation(s) and test(s).
			CWatchdogThreadInfo obj(obj2);
			ASSERT_EQ(name, obj.GetName()) << L"GetName()";
		}

		/// <summary>
		/// Functional test for:
		///     CWatchdogThreadInfo::operator=()
		///     CWatchdogThreadInfo::GetName()
		/// Test the value from the asignment operator.
		/// </summary>
		TEST_F(WatchdogThreadInfoUnitTests, GetName_Assign)
		{
			// Data.
			const string name(_T("thread name"));
			const DWORD timeOutMilliseconds(100000);
			const DWORD tickCount(200000);

			// Setup.
			CWatchdogThreadInfo obj;
			CWatchdogThreadInfo obj2(name, timeOutMilliseconds, tickCount, 0);
			ASSERT_EQ(m_nameDefault, obj.GetName()) << L"GetName() setup";

			// Perform operation(s) and test(s).
			obj = obj2;
			ASSERT_EQ(name, obj.GetName()) << L"GetName()";
		}
// // #pragma endregion GetName

// // #pragma region GetTimeOutMilliseconds
		/// <summary>
		/// Functional test for:
		///     CWatchdogThreadInfo::CWatchdogThreadInfo()
		///     CWatchdogThreadInfo::GetTimeOutMilliseconds()
		/// Code coverage for:
		///     CWatchdogThreadInfo::~CWatchdogThreadInfo()
		/// Test the value from the default constructor.
		/// </summary>
		TEST_F(WatchdogThreadInfoUnitTests, GetTimeOutMilliseconds_Default)
		{
			// Perform operation(s) and test(s).
			CWatchdogThreadInfo obj;
			ASSERT_EQ(m_timeOutMillisecondsDefault, obj.GetTimeOutMilliseconds()) << L"GetTimeOutMilliseconds()";
		}

		/// <summary>
		/// Functional test for:
		///     CWatchdogThreadInfo::CWatchdogThreadInfo(LPCTSTR, DWORD)
		///     CWatchdogThreadInfo::GetTimeOutMilliseconds()
		/// Test the value from the parameterized constructor.
		/// </summary>
		TEST_F(WatchdogThreadInfoUnitTests, GetTimeOutMilliseconds_Constructor)
		{
			// Data.
			const string name(_T("thread name"));
			const DWORD timeOutMilliseconds(100000);
			const DWORD tickCount(200000);

			// Perform operation(s) and test(s).
			CWatchdogThreadInfo obj(name, timeOutMilliseconds, tickCount, 0);
			ASSERT_EQ(timeOutMilliseconds, obj.GetTimeOutMilliseconds()) << L"GetTimeOutMilliseconds()";
		}

		/// <summary>
		/// Functional test for:
		///     CWatchdogThreadInfo::CWatchdogThreadInfo(const CWatchdogThreadInfo);
		///     CWatchdogThreadInfo::GetTimeOutMilliseconds()
		/// Test the value from the copy constructor.
		/// </summary>
		TEST_F(WatchdogThreadInfoUnitTests, GetTimeOutMilliseconds_Copy)
		{
			// Data.
			const string name(_T("thread name"));
			const DWORD timeOutMilliseconds(100000);
			const DWORD tickCount(200000);

			// Setup.
			CWatchdogThreadInfo obj2(name, timeOutMilliseconds, tickCount, 0);

			// Perform operation(s) and test(s).
			CWatchdogThreadInfo obj(obj2);
			ASSERT_EQ(timeOutMilliseconds, obj.GetTimeOutMilliseconds()) << L"GetTimeOutMilliseconds()";
		}

		/// <summary>
		/// Functional test for:
		///     CWatchdogThreadInfo::operator=()
		///     CWatchdogThreadInfo::GetTimeOutMilliseconds()
		/// Test the value from the asignment operator.
		/// </summary>
		TEST_F(WatchdogThreadInfoUnitTests, GetTimeOutMilliseconds_Assign)
		{
			// Data.
			const string name(_T("thread name"));
			const DWORD timeOutMilliseconds(100000);
			const DWORD tickCount(200000);

			// Setup.
			CWatchdogThreadInfo obj;
			CWatchdogThreadInfo obj2(name, timeOutMilliseconds, tickCount, 0);
			ASSERT_EQ(m_timeOutMillisecondsDefault, obj.GetTimeOutMilliseconds()) << L"GetTimeOutMilliseconds() setup";

			// Perform operation(s) and test(s).
			obj = obj2;
			ASSERT_EQ(timeOutMilliseconds, obj.GetTimeOutMilliseconds()) << L"GetTimeOutMilliseconds()";
		}
// // #pragma endregion GetTimeOutMilliseconds

// // #pragma region GetCheckInTime
		/// <summary>
		/// Functional test for:
		///     CWatchdogThreadInfo::CWatchdogThreadInfo()
		///     CWatchdogThreadInfo::GetCheckInTime()
		/// Code coverage for:
		///     CWatchdogThreadInfo::~CWatchdogThreadInfo()
		/// Test the value from the default constructor.
		/// </summary>
		TEST_F(WatchdogThreadInfoUnitTests, GetCheckInTime_Default)
		{
			// Perform operation(s) and test(s).
			CWatchdogThreadInfo obj;
			ASSERT_EQ(m_checkInTimeDefault, obj.GetCheckInTime()) << L"GetCheckInTime()";
		}

		/// <summary>
		/// Functional test for:
		///     CWatchdogThreadInfo::CWatchdogThreadInfo(LPCTSTR, DWORD)
		///     CWatchdogThreadInfo::GetCheckInTime()
		/// Test the value from the parameterized constructor.
		/// </summary>
		TEST_F(WatchdogThreadInfoUnitTests, GetCheckInTime_Constructor)
		{
			// Data.
			const string name(_T("thread name"));
			const DWORD timeOutMilliseconds(100000);
			const DWORD tickCount(200000);
			const DWORD checkInTime(tickCount);

			// Perform operation(s) and test(s).
			CWatchdogThreadInfo obj(name, timeOutMilliseconds, tickCount, 0);
			ASSERT_EQ(checkInTime, obj.GetCheckInTime()) << L"GetCheckInTime()";
		}

		/// <summary>
		/// Functional test for:
		///     CWatchdogThreadInfo::CWatchdogThreadInfo(const CWatchdogThreadInfo);
		///     CWatchdogThreadInfo::GetCheckInTime()
		/// Test the value from the copy constructor.
		/// </summary>
		TEST_F(WatchdogThreadInfoUnitTests, GetCheckInTime_Copy)
		{
			// Data.
			const string name(_T("thread name"));
			const DWORD timeOutMilliseconds(100000);
			const DWORD tickCount(200000);
			const DWORD checkInTime(tickCount);

			// Setup.
			CWatchdogThreadInfo obj2(name, timeOutMilliseconds, tickCount, 0);

			// Perform operation(s) and test(s).
			CWatchdogThreadInfo obj(obj2);
			ASSERT_EQ(checkInTime, obj.GetCheckInTime()) << L"GetCheckInTime()";
		}

		/// <summary>
		/// Functional test for:
		///     CWatchdogThreadInfo::operator=()
		///     CWatchdogThreadInfo::GetCheckInTime()
		/// Test the value from the asignment operator.
		/// </summary>
		TEST_F(WatchdogThreadInfoUnitTests, GetCheckInTime_Assign)
		{
			// Data.
			const string name(_T("thread name"));
			const DWORD timeOutMilliseconds(100000);
			const DWORD tickCount(200000);
			const DWORD checkInTime(tickCount);

			// Setup.
			CWatchdogThreadInfo obj;
			CWatchdogThreadInfo obj2(name, timeOutMilliseconds, tickCount, 0);
			ASSERT_EQ(m_checkInTimeDefault, obj.GetCheckInTime()) << L"GetCheckInTime()";

			// Perform operation(s) and test(s).
			obj = obj2;
			ASSERT_EQ(checkInTime, obj.GetCheckInTime()) << L"GetCheckInTime()";
		}

		void TestCheckInTickCount(CWatchdogThreadInfo &obj, DWORD tickCount)
		{
			// Perform operation(s) and test(s).
			obj.CheckIn(tickCount);
			ASSERT_EQ(tickCount, obj.GetCheckInTime()) << AssertMsg::FormatOnly(L"CheckIn(%u)", (unsigned)tickCount);
		}

		/// <summary>
		/// Functional test for:
		///     CWatchdogThreadInfo::CheckIn()
		///     CWatchdogThreadInfo::GetCheckInTime()
		/// Test setting the value.
		/// </summary>
		TEST_F(WatchdogThreadInfoUnitTests, GetCheckInTime_CheckIn)
		{
			// Setup.
			CWatchdogThreadInfo obj;

			// Perform operation(s) and test(s).
			TestCheckInTickCount(obj, 0);
			TestCheckInTickCount(obj, 1);
			TestCheckInTickCount(obj, (DWORD)(INT_MAX - 1));
			TestCheckInTickCount(obj, (DWORD)INT_MAX);
			TestCheckInTickCount(obj, (DWORD)INT_MIN);
			TestCheckInTickCount(obj, (DWORD)(INT_MIN + 1));
			TestCheckInTickCount(obj, (DWORD)(DWORD_MAX - 1));
			TestCheckInTickCount(obj, (DWORD)DWORD_MAX);
		}

		void TestResumeTickCount(CWatchdogThreadInfo &obj, DWORD tickCount)
		{
			// Perform operation(s) and test(s).
			obj.Resume(tickCount);
			ASSERT_EQ(tickCount, obj.GetCheckInTime()) << AssertMsg::FormatOnly(L"Resume(%u)", (unsigned)tickCount);
		}

		/// <summary>
		/// Functional test for:
		///     CWatchdogThreadInfo::Resume()
		///     CWatchdogThreadInfo::GetCheckInTime()
		/// Test setting the value.
		/// </summary>
		TEST_F(WatchdogThreadInfoUnitTests, GetCheckInTime_Resume)
		{
			// Setup.
			CWatchdogThreadInfo obj;

			// Perform operation(s) and test(s).
			TestResumeTickCount(obj, 0);
			TestResumeTickCount(obj, 1);
			TestResumeTickCount(obj, (DWORD)(INT_MAX - 1));
			TestResumeTickCount(obj, (DWORD)INT_MAX);
			TestResumeTickCount(obj, (DWORD)INT_MIN);
			TestResumeTickCount(obj, (DWORD)(INT_MIN + 1));
			TestResumeTickCount(obj, (DWORD)(DWORD_MAX - 1));
			TestResumeTickCount(obj, (DWORD)DWORD_MAX);
		}
// // #pragma endregion GetCheckInTime

// // #pragma region IsSuspended
		/// <summary>
		/// Functional test for:
		///     CWatchdogThreadInfo::CWatchdogThreadInfo()
		///     CWatchdogThreadInfo::IsSuspended()
		/// Code coverage for:
		///     CWatchdogThreadInfo::~CWatchdogThreadInfo()
		/// Test the value from the default constructor.
		/// </summary>
		TEST_F(WatchdogThreadInfoUnitTests, IsSuspended_Default)
		{
			// Perform operation(s) and test(s).
			CWatchdogThreadInfo obj;
			ASSERT_EQ(m_isSuspendedDefault, obj.IsSuspended()) << L"IsSuspended()";
		}

		/// <summary>
		/// Functional test for:
		///     CWatchdogThreadInfo::CWatchdogThreadInfo(LPCTSTR, DWORD)
		///     CWatchdogThreadInfo::IsSuspended()
		/// Test the value from the parameterized constructor.
		/// </summary>
		TEST_F(WatchdogThreadInfoUnitTests, IsSuspended_Constructor)
		{
			// Data.
			const string name(_T("thread name"));
			const DWORD timeOutMilliseconds(100000);
			const DWORD tickCount(200000);

			// Perform operation(s) and test(s).
			CWatchdogThreadInfo obj(name, timeOutMilliseconds, tickCount, 0);
			ASSERT_EQ(m_isSuspendedDefault, obj.IsSuspended()) << L"IsSuspended()";
		}

		/// <summary>
		/// Functional test for:
		///     CWatchdogThreadInfo::CWatchdogThreadInfo(const CWatchdogThreadInfo);
		///     CWatchdogThreadInfo::IsSuspended()
		/// Test the value from the copy constructor.
		/// </summary>
		TEST_F(WatchdogThreadInfoUnitTests, IsSuspended_CopyFalse)
		{
			// Setup.
			CWatchdogThreadInfo obj2;
			ASSERT_FALSE(obj2.IsSuspended()) << L"IsSuspended() setup";

			// Perform operation(s) and test(s).
			CWatchdogThreadInfo obj(obj2);
			ASSERT_FALSE(obj.IsSuspended()) << L"IsSuspended()";
		}

		/// <summary>
		/// Functional test for:
		///     CWatchdogThreadInfo::CWatchdogThreadInfo(const CWatchdogThreadInfo);
		///     CWatchdogThreadInfo::IsSuspended()
		/// Test the value from the copy constructor.
		/// </summary>
		TEST_F(WatchdogThreadInfoUnitTests, IsSuspended_CopyTrue)
		{
			// Setup.
			CWatchdogThreadInfo obj2;
			obj2.Suspend();
			ASSERT_EQ(true, obj2.IsSuspended()) << L"IsSuspended() setup";

			// Perform operation(s) and test(s).
			CWatchdogThreadInfo obj(obj2);
			ASSERT_EQ(true, obj.IsSuspended()) << L"IsSuspended()";
		}

		/// <summary>
		/// Functional test for:
		///     CWatchdogThreadInfo::operator=()
		///     CWatchdogThreadInfo::IsSuspended()
		/// Test the value from the asignment operator.
		/// </summary>
		TEST_F(WatchdogThreadInfoUnitTests, IsSuspended_AssignFalse)
		{
			// Setup.
			CWatchdogThreadInfo obj;
			obj.Suspend();
			CWatchdogThreadInfo obj2;
			ASSERT_EQ(true, obj.IsSuspended()) << L"IsSuspended() setup";
			ASSERT_FALSE(obj2.IsSuspended()) << L"IsSuspended() setup2";

			// Perform operation(s) and test(s).
			obj = obj2;
			ASSERT_FALSE(obj.IsSuspended()) << L"IsSuspended()";
		}

		/// <summary>
		/// Functional test for:
		///     CWatchdogThreadInfo::operator=()
		///     CWatchdogThreadInfo::IsSuspended()
		/// Test the value from the asignment operator.
		/// </summary>
		TEST_F(WatchdogThreadInfoUnitTests, IsSuspended_AssignTrue)
		{
			// Setup.
			CWatchdogThreadInfo obj;
			CWatchdogThreadInfo obj2;
			obj2.Suspend();
			ASSERT_FALSE(obj.IsSuspended()) << L"IsSuspended() setup";
			ASSERT_EQ(true, obj2.IsSuspended()) << L"IsSuspended() setup2";

			// Perform operation(s) and test(s).
			obj = obj2;
			ASSERT_EQ(true, obj.IsSuspended()) << L"IsSuspended()";
		}

		/// <summary>
		/// Functional test for:
		///     CWatchdogThreadInfo::Suspend()
		///     CWatchdogThreadInfo::IsSuspended()
		/// Test the value after Suspend().
		/// </summary>
		TEST_F(WatchdogThreadInfoUnitTests, IsSuspended_Suspend)
		{
			// Setup.
			CWatchdogThreadInfo obj;

			// Perform operation(s) and test(s).
			obj.Suspend();
			ASSERT_EQ(true, obj.IsSuspended()) << L"IsSuspended()";
		}

		/// <summary>
		/// Functional test for:
		///     CWatchdogThreadInfo::Resume()
		///     CWatchdogThreadInfo::IsSuspended()
		/// Test the value after Resume().
		/// </summary>
		TEST_F(WatchdogThreadInfoUnitTests, IsSuspended_Resume)
		{
			// Setup.
			CWatchdogThreadInfo obj;

			// Perform operation(s) and test(s).
			obj.Resume(0);
			ASSERT_FALSE(obj.IsSuspended()) << L"IsSuspended()";
		}

		/// <summary>
		/// Functional test for:
		///     CWatchdogThreadInfo::Suspend()
		///     CWatchdogThreadInfo::Resume()
		///     CWatchdogThreadInfo::IsSuspended()
		/// Test the value from the asignment operator.
		/// </summary>
		TEST_F(WatchdogThreadInfoUnitTests, IsSuspended_SuspendResume)
		{
			// Setup.
			CWatchdogThreadInfo obj;

			// Perform operation(s) and test(s).
			obj.Suspend();
			ASSERT_EQ(true, obj.IsSuspended()) << L"IsSuspended()";
			obj.Resume(0);
			ASSERT_FALSE(obj.IsSuspended()) << L"IsSuspended()";
			obj.Suspend();
			ASSERT_EQ(true, obj.IsSuspended()) << L"IsSuspended()";
			obj.Resume(0);
			ASSERT_FALSE(obj.IsSuspended()) << L"IsSuspended()";
		}
// // #pragma endregion IsSuspended

// // #pragma region HasTimedOut
		// Expects timeOutMilliseconds to be greater than 1 and less than INT_MAX.
		void TestHasTimedOut(CWatchdogThreadInfo &obj, DWORD timeOutMilliseconds, bool isSuspended, DWORD checkInTickCount)
		{
			wstring msg; msg.resize(512);
			int len = swprintf(&msg.front(), msg.capacity(), L" -- timeOutMilliseconds=%u isSuspended=%d checkInTickCount=%u",
				(unsigned)timeOutMilliseconds, (int)isSuspended, (unsigned)checkInTickCount);
			if (len >= 0) msg.resize((size_t)len);

			// Perform operation(s) and test(s).
			ASSERT_EQ(timeOutMilliseconds, obj.GetTimeOutMilliseconds()) << L"GetTimeOutMilliseconds()" + msg;
			ASSERT_EQ(checkInTickCount, obj.GetCheckInTime()) << L"GetCheckInTime()" + msg;
			ASSERT_EQ(isSuspended, obj.IsSuspended()) << L"IsSuspended()" + msg;

			if (isSuspended)
			{
				ASSERT_FALSE(obj.HasTimedOut(checkInTickCount - 1)) << L"HasTimedOut(checkInTickCount - 1)" + msg;
				ASSERT_FALSE(obj.HasTimedOut(checkInTickCount + 0)) << L"HasTimedOut(checkInTickCount + 0)" + msg;
				ASSERT_FALSE(obj.HasTimedOut(checkInTickCount + 1)) << L"HasTimedOut(checkInTickCount + 1)" + msg;

				ASSERT_FALSE(obj.HasTimedOut(checkInTickCount + timeOutMilliseconds - 1)) << L"HasTimedOut(checkInTickCount + timeOutMilliseconds - 1)" + msg;
				ASSERT_FALSE(obj.HasTimedOut(checkInTickCount + timeOutMilliseconds + 0)) << L"HasTimedOut(checkInTickCount + timeOutMilliseconds + 0)" + msg;
				ASSERT_FALSE(obj.HasTimedOut(checkInTickCount + timeOutMilliseconds + 1)) << L"HasTimedOut(checkInTickCount + timeOutMilliseconds + 1)" + msg;

				ASSERT_FALSE(obj.HasTimedOut(checkInTickCount + INT_MAX - 1)) << L"HasTimedOut(checkInTickCount + INT_MAX - 1)" + msg;
				ASSERT_FALSE(obj.HasTimedOut(checkInTickCount + INT_MAX + 0)) << L"HasTimedOut(checkInTickCount + INT_MAX + 0)" + msg;
				ASSERT_FALSE(obj.HasTimedOut(checkInTickCount + INT_MAX + 1)) << L"HasTimedOut(checkInTickCount + INT_MAX + 1)" + msg;
			}
			else
			{
				ASSERT_EQ(true, obj.HasTimedOut(checkInTickCount - 1)) << L"HasTimedOut(checkInTickCount - 1)" + msg;
				ASSERT_EQ(false || timeOutMilliseconds <= 0, obj.HasTimedOut(checkInTickCount + 0)) << L"HasTimedOut(checkInTickCount + 0)" + msg;
				ASSERT_EQ(false || timeOutMilliseconds <= 1, obj.HasTimedOut(checkInTickCount + 1)) << L"HasTimedOut(checkInTickCount + 1)" + msg;

				ASSERT_EQ(false || timeOutMilliseconds <= 0, obj.HasTimedOut(checkInTickCount + timeOutMilliseconds - 1)) << L"HasTimedOut(checkInTickCount + timeOutMilliseconds - 1)" + msg;
				ASSERT_EQ(true, obj.HasTimedOut(checkInTickCount + timeOutMilliseconds + 0)) << L"HasTimedOut(checkInTickCount + timeOutMilliseconds + 0)" + msg;
				ASSERT_EQ(true, obj.HasTimedOut(checkInTickCount + timeOutMilliseconds + 1)) << L"HasTimedOut(checkInTickCount + timeOutMilliseconds + 1)" + msg;

				ASSERT_EQ(true, obj.HasTimedOut(checkInTickCount + INT_MAX - 1)) << L"HasTimedOut(checkInTickCount + INT_MAX - 1)" + msg;
				ASSERT_EQ(true, obj.HasTimedOut(checkInTickCount + INT_MAX + 0)) << L"HasTimedOut(checkInTickCount + INT_MAX + 0)" + msg;
				ASSERT_EQ(true, obj.HasTimedOut(checkInTickCount + INT_MAX + 1)) << L"HasTimedOut(checkInTickCount + INT_MAX + 1)" + msg;
			}
		}

		/// <summary>
		/// Functional test for:
		///     CWatchdogThreadInfo::CWatchdogThreadInfo()
		///     CWatchdogThreadInfo::HasTimedOut()
		/// Code coverage for:
		///     CWatchdogThreadInfo::~CWatchdogThreadInfo()
		/// Test HasTimedOut() after using the default constructor.
		/// </summary>
		TEST_F(WatchdogThreadInfoUnitTests, HasTimedOut_Default)
		{
			// Perform operation(s) and test(s).
			CWatchdogThreadInfo obj;
			TestHasTimedOut(obj, m_timeOutMillisecondsDefault, m_isSuspendedDefault, m_checkInTimeDefault);
		}

		void TestHasTimedOut_Constructor(DWORD timeOutMilliseconds, DWORD tickCount)
		{
			// Data.
			const string name(_T("thread name"));

			// Perform operation(s) and test(s).
			CWatchdogThreadInfo obj(name, timeOutMilliseconds, tickCount, 0);
			TestHasTimedOut(obj, timeOutMilliseconds, WatchdogThreadInfoUnitTests::m_isSuspendedDefault, tickCount);
		}

		/// <summary>
		/// Functional test for:
		///     CWatchdogThreadInfo::CWatchdogThreadInfo(LPCTSTR, DWORD)
		///     CWatchdogThreadInfo::HasTimedOut()
		/// Test HasTimedOut() after using the parameterized constructor.
		/// </summary>
		TEST_F(WatchdogThreadInfoUnitTests, HasTimedOut_Constructor)
		{
			const DWORD timeOutMilliseconds(100000);

			// Perform operation(s) and test(s).
			TestHasTimedOut_Constructor(timeOutMilliseconds, 0);
			TestHasTimedOut_Constructor(timeOutMilliseconds, 1);
			TestHasTimedOut_Constructor(timeOutMilliseconds, INT_MAX - 1);
			TestHasTimedOut_Constructor(timeOutMilliseconds, INT_MAX);
			TestHasTimedOut_Constructor(timeOutMilliseconds, (DWORD)INT_MIN);
			TestHasTimedOut_Constructor(timeOutMilliseconds, (DWORD)(INT_MIN + 1));
			TestHasTimedOut_Constructor(timeOutMilliseconds, DWORD_MAX - 1);
			TestHasTimedOut_Constructor(timeOutMilliseconds, DWORD_MAX);
		}

		void TestHasTimedOut_Copy(DWORD timeOutMilliseconds, bool isSuspended, DWORD tickCount)
		{
			// Data.
			const string name(_T("thread name"));

			// Setup.
			CWatchdogThreadInfo obj2(name, timeOutMilliseconds, tickCount, 0);
			if (isSuspended)
			{
				obj2.Suspend();
			}
			ASSERT_EQ(isSuspended, obj2.IsSuspended()) << L"IsSuspended() setup2";

			// Perform operation(s) and test(s).
			CWatchdogThreadInfo obj(obj2);
			TestHasTimedOut(obj, timeOutMilliseconds, isSuspended, tickCount);
		}

		/// <summary>
		/// Functional test for:
		///     CWatchdogThreadInfo::CWatchdogThreadInfo(const CWatchdogThreadInfo);
		///     CWatchdogThreadInfo::HasTimedOut()
		/// Test HasTimedOut() after using the copy constructor.
		/// </summary>
		TEST_F(WatchdogThreadInfoUnitTests, HasTimedOut_Copy)
		{
			// Data.
			const DWORD timeOutMilliseconds(100000);

			// Perform operation(s) and test(s).
			TestHasTimedOut_Copy(timeOutMilliseconds, false, 0);
			TestHasTimedOut_Copy(timeOutMilliseconds, false, 1);
			TestHasTimedOut_Copy(timeOutMilliseconds, false, INT_MAX - 1);
			TestHasTimedOut_Copy(timeOutMilliseconds, false, INT_MAX);
			TestHasTimedOut_Copy(timeOutMilliseconds, false, (DWORD)INT_MIN);
			TestHasTimedOut_Copy(timeOutMilliseconds, false, (DWORD)(INT_MIN + 1));
			TestHasTimedOut_Copy(timeOutMilliseconds, false, DWORD_MAX - 1);
			TestHasTimedOut_Copy(timeOutMilliseconds, false, DWORD_MAX);

			TestHasTimedOut_Copy(timeOutMilliseconds, true, 0);
			TestHasTimedOut_Copy(timeOutMilliseconds, true, 1);
			TestHasTimedOut_Copy(timeOutMilliseconds, true, INT_MAX - 1);
			TestHasTimedOut_Copy(timeOutMilliseconds, true, INT_MAX);
			TestHasTimedOut_Copy(timeOutMilliseconds, true, (DWORD)INT_MIN);
			TestHasTimedOut_Copy(timeOutMilliseconds, true, (DWORD)(INT_MIN + 1));
			TestHasTimedOut_Copy(timeOutMilliseconds, true, DWORD_MAX - 1);
			TestHasTimedOut_Copy(timeOutMilliseconds, true, DWORD_MAX);
		}

		void TestHasTimedOut_Assign(DWORD timeOutMilliseconds, bool isSuspended, DWORD tickCount)
		{
			// Data.
			const string name(_T("thread name"));

			// Setup.
			CWatchdogThreadInfo obj;
			if (!isSuspended)
			{
				obj.Suspend();
			}
			CWatchdogThreadInfo obj2(name, timeOutMilliseconds, tickCount, 0);
			if (isSuspended)
			{
				obj2.Suspend();
			}
			ASSERT_EQ(!isSuspended, obj.IsSuspended()) << L"IsSuspended() setup";
			ASSERT_EQ(isSuspended, obj2.IsSuspended()) << L"IsSuspended() setup2";

			// Perform operation(s) and test(s).
			obj = obj2;
			TestHasTimedOut(obj, timeOutMilliseconds, isSuspended, tickCount);
		}

		/// <summary>
		/// Functional test for:
		///     CWatchdogThreadInfo::operator=()
		///     CWatchdogThreadInfo::HasTimedOut()
		/// Test HasTimedOut() after using the asignment operator.
		/// </summary>
		TEST_F(WatchdogThreadInfoUnitTests, HasTimedOut_Assign)
		{
			// Data.
			const DWORD timeOutMilliseconds(100000);

			// Perform operation(s) and test(s).
			TestHasTimedOut_Assign(timeOutMilliseconds, false, 0);
			TestHasTimedOut_Assign(timeOutMilliseconds, false, 1);
			TestHasTimedOut_Assign(timeOutMilliseconds, false, INT_MAX - 1);
			TestHasTimedOut_Assign(timeOutMilliseconds, false, INT_MAX);
			TestHasTimedOut_Assign(timeOutMilliseconds, false, (DWORD)INT_MIN);
			TestHasTimedOut_Assign(timeOutMilliseconds, false, (DWORD)(INT_MIN + 1));
			TestHasTimedOut_Assign(timeOutMilliseconds, false, DWORD_MAX - 1);
			TestHasTimedOut_Assign(timeOutMilliseconds, false, DWORD_MAX);

			TestHasTimedOut_Assign(timeOutMilliseconds, true, 0);
			TestHasTimedOut_Assign(timeOutMilliseconds, true, 1);
			TestHasTimedOut_Assign(timeOutMilliseconds, true, INT_MAX - 1);
			TestHasTimedOut_Assign(timeOutMilliseconds, true, INT_MAX);
			TestHasTimedOut_Assign(timeOutMilliseconds, true, (DWORD)INT_MIN);
			TestHasTimedOut_Assign(timeOutMilliseconds, true, (DWORD)(INT_MIN + 1));
			TestHasTimedOut_Assign(timeOutMilliseconds, true, DWORD_MAX - 1);
			TestHasTimedOut_Assign(timeOutMilliseconds, true, DWORD_MAX);
		}

		void TestHasTimedOut_CheckIn(DWORD timeOutMilliseconds, bool isSuspended, DWORD tickCount)
		{
			// Data.
			const string name(_T("thread name"));

			// Setup.
			CWatchdogThreadInfo obj(name, timeOutMilliseconds, tickCount, 0);
			if (isSuspended)
			{
				obj.Suspend();
			}
			ASSERT_EQ(isSuspended, obj.IsSuspended()) << L"IsSuspended() setup";

			// Perform operation(s) and test(s).
			obj.CheckIn(tickCount);
			TestHasTimedOut(obj, timeOutMilliseconds, isSuspended, tickCount);
		}

		/// <summary>
		/// Functional test for:
		///     CWatchdogThreadInfo::CheckIn()
		///     CWatchdogThreadInfo::HasTimedOut()
		/// Test HasTimedOut() after calling CheckIn().
		/// </summary>
		TEST_F(WatchdogThreadInfoUnitTests, HasTimedOut_CheckIn)
		{
			// Data.
			const DWORD timeOutMilliseconds(100000);

			// Perform operation(s) and test(s).
			TestHasTimedOut_CheckIn(timeOutMilliseconds, false, 0);
			TestHasTimedOut_CheckIn(timeOutMilliseconds, false, 1);
			TestHasTimedOut_CheckIn(timeOutMilliseconds, false, INT_MAX - 1);
			TestHasTimedOut_CheckIn(timeOutMilliseconds, false, INT_MAX);
			TestHasTimedOut_CheckIn(timeOutMilliseconds, false, (DWORD)INT_MIN);
			TestHasTimedOut_CheckIn(timeOutMilliseconds, false, (DWORD)(INT_MIN + 1));
			TestHasTimedOut_CheckIn(timeOutMilliseconds, false, DWORD_MAX - 1);
			TestHasTimedOut_CheckIn(timeOutMilliseconds, false, DWORD_MAX);

			TestHasTimedOut_CheckIn(timeOutMilliseconds, true, 0);
			TestHasTimedOut_CheckIn(timeOutMilliseconds, true, 1);
			TestHasTimedOut_CheckIn(timeOutMilliseconds, true, INT_MAX - 1);
			TestHasTimedOut_CheckIn(timeOutMilliseconds, true, INT_MAX);
			TestHasTimedOut_CheckIn(timeOutMilliseconds, true, (DWORD)INT_MIN);
			TestHasTimedOut_CheckIn(timeOutMilliseconds, true, (DWORD)(INT_MIN + 1));
			TestHasTimedOut_CheckIn(timeOutMilliseconds, true, DWORD_MAX - 1);
			TestHasTimedOut_CheckIn(timeOutMilliseconds, true, DWORD_MAX);
		}

		void TestHasTimedOut_Suspend(DWORD timeOutMilliseconds, bool isSuspended, DWORD tickCount)
		{
			// Data.
			const string name(_T("thread name"));

			// Setup.
			CWatchdogThreadInfo obj(name, timeOutMilliseconds, tickCount, 0);
			if (isSuspended)
			{
				obj.Suspend();
			}
			ASSERT_EQ(isSuspended, obj.IsSuspended()) << L"IsSuspended() setup";

			// Perform operation(s) and test(s).
			obj.Suspend();
			TestHasTimedOut(obj, timeOutMilliseconds, true, tickCount);
		}

		/// <summary>
		/// Functional test for:
		///     CWatchdogThreadInfo::Suspend()
		///     CWatchdogThreadInfo::HasTimedOut()
		/// Test HasTimedOut() after calling Suspend().
		/// </summary>
		TEST_F(WatchdogThreadInfoUnitTests, HasTimedOut_Suspend)
		{
			// Data.
			const DWORD timeOutMilliseconds(100000);

			// Perform operation(s) and test(s).
			TestHasTimedOut_Suspend(timeOutMilliseconds, false, 0);
			TestHasTimedOut_Suspend(timeOutMilliseconds, false, 1);
			TestHasTimedOut_Suspend(timeOutMilliseconds, false, INT_MAX - 1);
			TestHasTimedOut_Suspend(timeOutMilliseconds, false, INT_MAX);
			TestHasTimedOut_Suspend(timeOutMilliseconds, false, (DWORD)INT_MIN);
			TestHasTimedOut_Suspend(timeOutMilliseconds, false, (DWORD)(INT_MIN + 1));
			TestHasTimedOut_Suspend(timeOutMilliseconds, false, DWORD_MAX - 1);
			TestHasTimedOut_Suspend(timeOutMilliseconds, false, DWORD_MAX);

			TestHasTimedOut_Suspend(timeOutMilliseconds, true, 0);
			TestHasTimedOut_Suspend(timeOutMilliseconds, true, 1);
			TestHasTimedOut_Suspend(timeOutMilliseconds, true, INT_MAX - 1);
			TestHasTimedOut_Suspend(timeOutMilliseconds, true, INT_MAX);
			TestHasTimedOut_Suspend(timeOutMilliseconds, true, (DWORD)INT_MIN);
			TestHasTimedOut_Suspend(timeOutMilliseconds, true, (DWORD)(INT_MIN + 1));
			TestHasTimedOut_Suspend(timeOutMilliseconds, true, DWORD_MAX - 1);
			TestHasTimedOut_Suspend(timeOutMilliseconds, true, DWORD_MAX);
		}

		void TestHasTimedOut_Resume(DWORD timeOutMilliseconds, bool isSuspended, DWORD tickCount)
		{
			// Data.
			const string name(_T("thread name"));

			// Setup.
			CWatchdogThreadInfo obj(name, timeOutMilliseconds, tickCount, 0);
			if (isSuspended)
			{
				obj.Suspend();
			}
			ASSERT_EQ(isSuspended, obj.IsSuspended()) << L"IsSuspended() setup";

			// Perform operation(s) and test(s).
			obj.Resume(tickCount);
			TestHasTimedOut(obj, timeOutMilliseconds, false, tickCount);
		}

		/// <summary>
		/// Functional test for:
		///     CWatchdogThreadInfo::Resume()
		///     CWatchdogThreadInfo::HasTimedOut()
		/// Test HasTimedOut() after calling Resume().
		/// </summary>
		TEST_F(WatchdogThreadInfoUnitTests, HasTimedOut_Resume)
		{
			// Data.
			const DWORD timeOutMilliseconds(100000);

			// Perform operation(s) and test(s).
			TestHasTimedOut_Resume(timeOutMilliseconds, false, 0);
			TestHasTimedOut_Resume(timeOutMilliseconds, false, 1);
			TestHasTimedOut_Resume(timeOutMilliseconds, false, INT_MAX - 1);
			TestHasTimedOut_Resume(timeOutMilliseconds, false, INT_MAX);
			TestHasTimedOut_Resume(timeOutMilliseconds, false, (DWORD)INT_MIN);
			TestHasTimedOut_Resume(timeOutMilliseconds, false, (DWORD)(INT_MIN + 1));
			TestHasTimedOut_Resume(timeOutMilliseconds, false, DWORD_MAX - 1);
			TestHasTimedOut_Resume(timeOutMilliseconds, false, DWORD_MAX);

			TestHasTimedOut_Resume(timeOutMilliseconds, true, 0);
			TestHasTimedOut_Resume(timeOutMilliseconds, true, 1);
			TestHasTimedOut_Resume(timeOutMilliseconds, true, INT_MAX - 1);
			TestHasTimedOut_Resume(timeOutMilliseconds, true, INT_MAX);
			TestHasTimedOut_Resume(timeOutMilliseconds, true, (DWORD)INT_MIN);
			TestHasTimedOut_Resume(timeOutMilliseconds, true, (DWORD)(INT_MIN + 1));
			TestHasTimedOut_Resume(timeOutMilliseconds, true, DWORD_MAX - 1);
			TestHasTimedOut_Resume(timeOutMilliseconds, true, DWORD_MAX);
		}

		void TestHasTimedOut_SuspendResume(DWORD timeOutMilliseconds, DWORD tickCount)
		{
			// Data.
			const string name(_T("thread name"));

			// Setup.
			DWORD tickCount0(tickCount + INT_MAX / 2);
			CWatchdogThreadInfo obj(name, timeOutMilliseconds, tickCount0, 0);

			// Perform operation(s) and test(s).
			obj.Suspend();
			TestHasTimedOut(obj, timeOutMilliseconds, true, tickCount0);
			obj.Resume(tickCount);
			TestHasTimedOut(obj, timeOutMilliseconds, false, tickCount);

			obj.Suspend();
			TestHasTimedOut(obj, timeOutMilliseconds, true, tickCount);
			DWORD tickCount2(tickCount + INT_MAX / 2);
			obj.Resume(tickCount2);
			TestHasTimedOut(obj, timeOutMilliseconds, false, tickCount2);
		}

		/// <summary>
		/// Functional test for:
		///     CWatchdogThreadInfo::Suspend()
		///     CWatchdogThreadInfo::Resume()
		///     CWatchdogThreadInfo::HasTimedOut()
		/// Test HasTimedOut() after calling Suspend() and Resume().
		/// </summary>
		TEST_F(WatchdogThreadInfoUnitTests, HasTimedOut_SuspendResume)
		{
			// Data.
			const DWORD timeOutMilliseconds(100000);

			// Perform operation(s) and test(s).
			TestHasTimedOut_SuspendResume(timeOutMilliseconds, 0);
			TestHasTimedOut_SuspendResume(timeOutMilliseconds, 1);
			TestHasTimedOut_SuspendResume(timeOutMilliseconds, INT_MAX - 1);
			TestHasTimedOut_SuspendResume(timeOutMilliseconds, INT_MAX);
			TestHasTimedOut_SuspendResume(timeOutMilliseconds, (DWORD)INT_MIN);
			TestHasTimedOut_SuspendResume(timeOutMilliseconds, (DWORD)(INT_MIN + 1));
			TestHasTimedOut_SuspendResume(timeOutMilliseconds, DWORD_MAX - 1);
			TestHasTimedOut_SuspendResume(timeOutMilliseconds, DWORD_MAX);
		}
// // #pragma endregion HasTimedOut

	const string WatchdogThreadInfoUnitTests::m_nameDefault(_T(""));
	const DWORD WatchdogThreadInfoUnitTests::m_timeOutMillisecondsDefault = 0;
	const DWORD WatchdogThreadInfoUnitTests::m_checkInTimeDefault = 0;
	const bool WatchdogThreadInfoUnitTests::m_isSuspendedDefault = false;
}