#include "stdafx.h"
#include "Hardware/WatchDog.h"

// Sentinel includes.
#include "GlobalDefs.h"
#include "Utilities.h"

// Unit testing includes.
#include "gtest/gtest.h"
#include "AssertMsg.h"
#include "UnitTestingOperatingSystem.h"
#include "DirectoryManager.h"

using namespace std;

class WatchDogUnitTests : public ::testing::Test
{
public:
	static const int WAITFORLOOP = 3000;
	static const int SAFETY = 30000;

	struct WatchDogData
	{
		WatchDogData(CWatchDog* watchdog, wchar_t* buf, size_t sz) : obj(watchdog), buffer(buf), size(sz) {}

		CWatchDog* obj;
		wchar_t* buffer;
		size_t size;
	};

	struct ThreadData
	{
		enum Sentinel
		{
			RUNNING = 0,
			DONE
		};

		ThreadData() : stop(false), count(0), sentinel(RUNNING), watchdog(NULL), threadName(NULL) {}

		bool stop;
		uint count;
		Sentinel sentinel;
		WatchDogData* watchdog;
		LPCSTR threadName;
	};

	void AssertThreadCheckInTime(const CWatchDog &watchdog, thread::id threadID, DWORD checkInTime, LPCWSTR desc);
};
		
// #pragma region Instance
/// <summary>
/// Functional test for:
///     CWatchDog::Instance()
/// Verify that the same instance is returned for each call.
/// </summary>
TEST_F(WatchDogUnitTests, Instance2)
{
	// Perform operation(s) and test(s).
	CWatchDog *watchdog(&CWatchDog::Instance());
	ASSERT_EQ((void *)watchdog, (void *)&CWatchDog::Instance()) << L"Instance";
}
// #pragma endregion Instance

// #pragma region CWatchDog
/// <summary>
/// Test that CWatchDog constants are correct.
/// </summary>
TEST_F(WatchDogUnitTests, CWatchDog_Constants)
{
	// Perform test(s).
	ASSERT_EQ((DWORD)30000, CWatchDog::ALLOWED_SHUTDOWN_MILLISECONDS) << L"ALLOWED_SHUTDOWN_MILLISECONDS";
	ASSERT_EQ((DWORD)300, CWatchDog::PING_WATCHDOG_MILLISECONDS) << L"PING_WATCHDOG_MILLISECONDS";
}

/// <summary>
/// Functional test for:
///     CWatchDog::CWatchDog()
/// Code coverage for:
///     CWatchDog::~CWatchDog()
/// Test that the (default) constructor sets the correct value(s).
/// </summary>
TEST_F(WatchDogUnitTests, WatchDogDefaultConstructorTest)
{
	CWatchDog watchdog;
	ASSERT_FALSE(watchdog.m_timeToClose) << L"m_timeToClose";
	ASSERT_FALSE(watchdog.m_shuttingDown) << L"m_shuttingDown";
	ASSERT_EQ((DWORD)0, watchdog.m_beginShutdownTickCount) << L"m_ShutdownStart";
	ASSERT_EQ((void *)&COperatingSystem::m_operatingSystem, (void *)&watchdog.m_operatingSystem) << L"m_operatingSystem";
	ASSERT_EQ((void *)nullptr, (void *)watchdog.m_thread.get()) << L"m_thread";

	ASSERT_NE(FALSE, watchdog.m_threadList.empty()) << L"m_threadList.empty()";
	ASSERT_EQ((size_t)0, watchdog.m_threadList.size()) << L"m_threadList.size()";
}

/// <summary>
/// Functional test for:
///     CWatchDog::CWatchDog(IOperatingSystem &)
/// Code coverage for:
///     CWatchDog::~CWatchDog()
/// Test that the (non-default) constructor sets the correct value(s).
/// </summary>
TEST_F(WatchDogUnitTests, WatchDogNonDefaultConstructorTest)
{
	// Perform operation(s) and test(s).
	COperatingSystemTest operatingSystem;
	CWatchDog watchdog(operatingSystem);
	ASSERT_FALSE(watchdog.m_timeToClose) << L"m_timeToClose";
	ASSERT_FALSE(watchdog.m_shuttingDown) << L"m_shuttingDown";
	ASSERT_EQ((DWORD)0, watchdog.m_beginShutdownTickCount) << L"m_ShutdownStart";
	ASSERT_EQ((void *)&operatingSystem, (void *)&watchdog.m_operatingSystem) << L"m_operatingSystem";
	ASSERT_EQ((void *)nullptr, (void *)watchdog.m_thread.get()) << L"m_thread";

	ASSERT_NE(false, watchdog.m_threadList.empty()) << L"m_threadList.empty()";
	ASSERT_EQ((size_t)0, watchdog.m_threadList.size()) << L"m_threadList.size()";
}
// #pragma endregion CWatchDog

// #pragma region StartStop
/// <summary>
/// Functional test for:
///     CWatchDog::Start()
///     CWatchDog::Stop()
/// Test that the instance thread is created and destroyed.
/// </summary>
TEST_F(WatchDogUnitTests, StartStopTest)
{
	// Setup.
	CWatchDog watchdog;
	ASSERT_EQ((void *)nullptr, (void *)watchdog.m_thread.get()) << L"m_thread setup";

	// Perform operation(s) and test(s).
	watchdog.Start();
	ASSERT_NE((void *)nullptr, (void *)watchdog.m_thread.get()) << L"m_thread start";
	//We now detach the watchdog thread so that we shut it down cleanly.
	//That means it isn't and no longer should be "joinable".
	//ASSERT_EQ(true, watchdog.m_thread.get()->joinable()) << L"m_thread start";

	watchdog.Stop();
	ASSERT_EQ((void *)nullptr, (void *)watchdog.m_thread.get()) << L"m_thread stop";

}
// #pragma endregion StartStop

// #pragma region BeginShutdown
/// <summary>
/// Functional test for:
///     CWatchDog::BeginShutdown()
/// Test that the function sets the correct value(s).
/// </summary>
TEST_F(WatchDogUnitTests, BeginShutdown_DefaultTest)
{
	// Data.
	const DWORD tickCount(12345);

	// Setup.
	COperatingSystemTest operatingSystem;
	CWatchDog watchdog(operatingSystem);
	ASSERT_FALSE(watchdog.m_shuttingDown) << L"m_shuttingDown setup";

	// Perform operation(s) and test(s).
	operatingSystem.m_tickCount32 = tickCount;
	watchdog.BeginShutdown("");
	ASSERT_EQ(true, watchdog.m_shuttingDown) << L"m_shuttingDown";
	ASSERT_EQ(tickCount, watchdog.m_beginShutdownTickCount) << L"m_beginShutdownTickCount";
}

/// <summary>
/// Functional test for:
///     CWatchDog::BeginShutdown()
/// Test that the function sets the correct value(s).
/// </summary>
TEST_F(WatchDogUnitTests, BeginShutdown_FalseTest)
{
	// Data.
	const DWORD tickCount(12345);

	// Setup.
	COperatingSystemTest operatingSystem;
	CWatchDog watchdog(operatingSystem);
	ASSERT_FALSE(watchdog.m_shuttingDown) << L"m_shuttingDown setup";

	// Perform operation(s) and test(s).
	operatingSystem.m_tickCount32 = tickCount;
	watchdog.BeginShutdown("");
	ASSERT_EQ(true, watchdog.m_shuttingDown) << L"m_shuttingDown";
	ASSERT_EQ(tickCount, watchdog.m_beginShutdownTickCount) << L"m_beginShutdownTickCount";
}

/// <summary>
/// Functional test for:
///     CWatchDog::BeginShutdown()
/// Test that the function sets the correct value(s).
/// </summary>
TEST_F(WatchDogUnitTests, BeginShutdown_TrueTest)
{
	// Data.
	const DWORD tickCount(12345);

	// Setup.
	COperatingSystemTest operatingSystem;
	CWatchDog watchdog(operatingSystem);
	ASSERT_FALSE(watchdog.m_shuttingDown) << L"m_shuttingDown setup";

	// Perform operation(s) and test(s).
	operatingSystem.m_tickCount32 = tickCount;
	watchdog.BeginShutdown("");
	ASSERT_EQ(true, watchdog.m_shuttingDown) << L"m_shuttingDown";
	ASSERT_EQ(tickCount, watchdog.m_beginShutdownTickCount) << L"m_beginShutdownTickCount";
}

/// <summary>
/// Functional test for:
///     CWatchDog::BeginShutdown()
/// Test that the value(s) are retained from the first function call when called again.
/// </summary>
TEST_F(WatchDogUnitTests, BeginShutdown_FalseTrueTest)
{
	// Data.
	const DWORD tickCount(12345);

	// Setup.
	COperatingSystemTest operatingSystem;
	CWatchDog watchdog(operatingSystem);
	operatingSystem.m_tickCount32 = tickCount;
	watchdog.BeginShutdown("");
	ASSERT_EQ(true, watchdog.m_shuttingDown) << L"m_shuttingDown setup";
	ASSERT_EQ(tickCount, watchdog.m_beginShutdownTickCount) << L"m_beginShutdownTickCount setup";

	// Perform operation(s) and test(s).
	operatingSystem.m_tickCount32 = tickCount + 5000;
	watchdog.BeginShutdown("");
	ASSERT_EQ(true, watchdog.m_shuttingDown) << L"m_shuttingDown";
	ASSERT_EQ(tickCount, watchdog.m_beginShutdownTickCount) << L"m_beginShutdownTickCount";
}

/// <summary>
/// Functional test for:
///     CWatchDog::BeginShutdown()
/// Test that the value(s) are retained from the first function call when called again.
/// </summary>
TEST_F(WatchDogUnitTests, BeginShutdown_TrueFalseTest)
{
	// Data.
	const DWORD tickCount(12345);

	// Setup.
	COperatingSystemTest operatingSystem;
	CWatchDog watchdog(operatingSystem);
	operatingSystem.m_tickCount32 = tickCount;
	watchdog.BeginShutdown("");
	ASSERT_EQ(true, watchdog.m_shuttingDown) << L"m_shuttingDown setup";
	ASSERT_EQ(tickCount, watchdog.m_beginShutdownTickCount) << L"m_beginShutdownTickCount setup";

	// Perform operation(s) and test(s).
	operatingSystem.m_tickCount32 = tickCount + 5000;
	watchdog.BeginShutdown("");
	ASSERT_EQ(true, watchdog.m_shuttingDown) << L"m_shuttingDown";
	ASSERT_EQ(tickCount, watchdog.m_beginShutdownTickCount) << L"m_beginShutdownTickCount";
}
// #pragma endregion BeginShutdown

// #pragma region ShutdownNow
/// <summary>
/// Functional test for:
///     CWatchDog::ShutdownNow()
/// Test that the function sets the correct value(s).
/// </summary>
TEST_F(WatchDogUnitTests, ShutdownNowTest)
{
	// Setup.
	CWatchDog watchdog;
	ASSERT_FALSE(watchdog.m_timeToClose) << L"m_timeToClose setup";

	// Perform operation(s) and test(s).
	watchdog.BeginShutdown("exception");
	ASSERT_EQ(true, watchdog.m_timeToClose) << L"m_timeToClose";
}
// #pragma endregion ShutdownNow

// #pragma region IsTimeToShutdown
/// <summary>
/// Functional test for:
///     CWatchDog::ShutdownNow()
///     CWatchDog::IsTimeToShutdown()
/// Test that ShutdownNow() has no affect on the return value forIsTimeToShutdown().
/// </summary>
TEST_F(WatchDogUnitTests, IsTimeToShutdown_ShutdownNowTest)
{
	// Setup.
	CWatchDog watchdog;
	ASSERT_FALSE(watchdog.IsTimeToShutdown()) << L"IsTimeToShutdown() setup";

	// Perform operation(s) and test(s).
	watchdog.BeginShutdown("exception");
	ASSERT_TRUE(watchdog.IsTimeToShutdown()) << L"IsTimeToShutdown()";
}

/// <summary>
/// Functional test for:
///     CWatchDog::BeginShutdown()
///     CWatchDog::IsTimeToShutdown()
/// Test that BeginShutdown() default affects the return value for IsTimeToShutdown().
/// </summary>
TEST_F(WatchDogUnitTests, IsTimeToShutdown_BeginShutdownDefaultTest)
{
	// Setup.
	CWatchDog watchdog;
	ASSERT_FALSE(watchdog.IsTimeToShutdown()) << L"IsTimeToShutdown() setup";

	// Perform operation(s) and test(s).
	watchdog.BeginShutdown("exception");
	ASSERT_EQ(true, watchdog.IsTimeToShutdown()) << L"IsTimeToShutdown()";
}

/// <summary>
/// Functional test for:
///     CWatchDog::BeginShutdown(false)
///     CWatchDog::IsTimeToShutdown()
/// Test that BeginShutdown(false) affects the return value for IsTimeToShutdown().
/// </summary>
TEST_F(WatchDogUnitTests, IsTimeToShutdown_BeginShutdownFalseTest)
{
	// Setup.
	CWatchDog watchdog;
	ASSERT_FALSE(watchdog.IsTimeToShutdown()) << L"IsTimeToShutdown() setup";

	// Perform operation(s) and test(s).
	watchdog.BeginShutdown("exception");
	ASSERT_EQ(true, watchdog.IsTimeToShutdown()) << L"IsTimeToShutdown()";
}

/// <summary>
/// Functional test for:
///     CWatchDog::BeginShutdown(true)
///     CWatchDog::IsTimeToShutdown()
/// Test that BeginShutdown(true) affects the return value for IsTimeToShutdown().
/// </summary>
TEST_F(WatchDogUnitTests, IsTimeToShutdown_BeginShutdownTrueTest)
{
	// Setup.
	CWatchDog watchdog;
	ASSERT_FALSE(watchdog.IsTimeToShutdown()) << L"IsTimeToShutdown() setup";

	// Perform operation(s) and test(s).
	watchdog.BeginShutdown("");
	ASSERT_EQ(true, watchdog.IsTimeToShutdown()) << L"IsTimeToShutdown()";
}
// #pragma endregion IsTimeToShutdown

// #pragma region ShutdownTimedOut_DefaultOS
/// <summary>
/// Functional test for:
///     CWatchDog::CWatchDog()
///     CWatchDog::ShutdownTimedOut()
/// Test returned value after the (default) constructor completes.
/// </summary>
TEST_F(WatchDogUnitTests, ShutdownTimedOut_DefaultOSTest)
{
	// Perform operation(s) and test(s).
	CWatchDog watchdog;
	ASSERT_FALSE(watchdog.ShutdownTimedOut()) << L"ShutdownTimedOut()";
}

/// <summary>
/// Functional test for:
///     CWatchDog::CWatchDog()
///     CWatchDog::BeginShutdown()
///     CWatchDog::ShutdownTimedOut()
/// Test returned value after calling BeginShutdown(false).
/// </summary>
TEST_F(WatchDogUnitTests, ShutdownTimedOut_BeginShutdownFalse_DefaultOSTest)
{
	// Setup.
	CWatchDog watchdog;
	ASSERT_FALSE(watchdog.ShutdownTimedOut()) << L"ShutdownTimedOut() setup";

	// Perform operation(s) and test(s).
	watchdog.BeginShutdown("exception");
	ASSERT_FALSE(watchdog.ShutdownTimedOut()) << L"ShutdownTimedOut()";
}

/// <summary>
/// Functional test for:
///     CWatchDog::CWatchDog()
///     CWatchDog::BeginShutdown()
///     CWatchDog::ShutdownTimedOut()
/// Test returned value after calling BeginShutdown(true).
/// </summary>
TEST_F(WatchDogUnitTests, ShutdownTimedOut_BeginShutdownTrue_DefaultOSTest)
{
	// Setup.
	CWatchDog watchdog;
	ASSERT_FALSE(watchdog.ShutdownTimedOut()) << L"ShutdownTimedOut() setup";

	// Perform operation(s) and test(s).
	watchdog.BeginShutdown("");
	ASSERT_FALSE(watchdog.ShutdownTimedOut()) << L"ShutdownTimedOut()";
}

/// <summary>
/// Functional test for:
///     CWatchDog::CWatchDog()
///     CWatchDog::BeginShutdown()
///     CWatchDog::ShutdownTimedOut()
/// Test returned values after calling BeginShutdown().
/// </summary>
TEST_F(WatchDogUnitTests, ShutdownTimedOut_True_DefaultOSTest)
{
	// Setup.
	CWatchDog watchdog;
	ASSERT_FALSE(watchdog.ShutdownTimedOut()) << L"ShutdownTimedOut() setup";

	// Perform operation(s) and test(s).
	watchdog.BeginShutdown("exception");
	ASSERT_FALSE(watchdog.ShutdownTimedOut()) << L"ShutdownTimedOut()";

	CUtilities::Sleep(CWatchDog::ALLOWED_SHUTDOWN_MILLISECONDS + 50);
	ASSERT_EQ(true, watchdog.ShutdownTimedOut()) << L"ShutdownTimedOut()";
}

/// <summary>
/// Functional test for:
///     CWatchDog::CWatchDog()
///     CWatchDog::ShutdownNow()
///     CWatchDog::ShutdownTimedOut()
/// Test returned value after calling ShutdownNow().
/// </summary>
TEST_F(WatchDogUnitTests, ShutdownTimedOut_ShutdownNow_DefaultOSTest)
{
	// Setup.
	CWatchDog watchdog;
	ASSERT_FALSE(watchdog.ShutdownTimedOut()) << L"ShutdownTimedOut() setup";

	// Perform operation(s) and test(s).
	watchdog.BeginShutdown("exception");
	ASSERT_FALSE(watchdog.ShutdownTimedOut()) << L"ShutdownTimedOut()";
}

// #pragma endregion ShutdownTimedOut_DefaultOS

// #pragma region ShutdownTimedOut
/// <summary>
/// Functional test for:
///     CWatchDog::CWatchDog(IOperatingSystem &)
///     CWatchDog::ShutdownTimedOut()
/// Test returned values after the (non-default) constructor completes.
/// </summary>
TEST_F(WatchDogUnitTests, ShutdownTimedOut_NonDefaultTest)
{
	// Data.
	const DWORD tickCount0 = 10000;
	const DWORD tickCount2 = tickCount0 + 2 * CWatchDog::ALLOWED_SHUTDOWN_MILLISECONDS;

	// Setup.
	COperatingSystemTest operatingSystem; // So we can control the tick count.

	// Perform operation(s) and test(s).
	operatingSystem.m_tickCount32 = tickCount0;
	CWatchDog watchdog(operatingSystem);
	ASSERT_FALSE(watchdog.ShutdownTimedOut()) << L"ShutdownTimedOut() 0";

	operatingSystem.m_tickCount32 = tickCount2;
	ASSERT_FALSE(watchdog.ShutdownTimedOut()) << L"ShutdownTimedOut() 2";
}

/// <summary>
/// Functional test for:
///     CWatchDog::CWatchDog()
///     CWatchDog::BeginShutdown()
///     CWatchDog::ShutdownTimedOut()
/// Test returned values after calling BeginShutdown(false).
/// </summary>
TEST_F(WatchDogUnitTests, ShutdownTimedOut_BeginShutdownFalseTest)
{
	// Data.
	const DWORD tickCount0 = 10000;
	const DWORD tickCountBeginShutdown = 12000;
	const DWORD tickCountMaxAllowed = tickCountBeginShutdown + CWatchDog::ALLOWED_SHUTDOWN_MILLISECONDS;
	const DWORD tickCountTimedOut = tickCountMaxAllowed + +1;

	// Setup.
	COperatingSystemTest operatingSystem;
	operatingSystem.m_tickCount32 = tickCount0;
	CWatchDog watchdog(operatingSystem);
	ASSERT_FALSE(watchdog.ShutdownTimedOut()) << L"ShutdownTimedOut() setup";

	// Perform operation(s) and test(s).
	operatingSystem.m_tickCount32 = tickCountBeginShutdown;
	watchdog.BeginShutdown("exception");
	ASSERT_FALSE(watchdog.ShutdownTimedOut()) << L"ShutdownTimedOut() tickCountBeginShutdown";

	operatingSystem.m_tickCount32 = tickCountMaxAllowed;
	ASSERT_FALSE(watchdog.ShutdownTimedOut()) << L"ShutdownTimedOut() tickCountMaxAllowed";
	operatingSystem.m_tickCount32 = tickCountTimedOut;
	ASSERT_EQ(true, watchdog.ShutdownTimedOut()) << L"ShutdownTimedOut() tickCountTimedOut";
}

/// <summary>
/// Functional test for:
///     CWatchDog::CWatchDog()
///     CWatchDog::BeginShutdown()
///     CWatchDog::ShutdownTimedOut()
/// Test returned values after calling BeginShutdown(true).
/// </summary>
TEST_F(WatchDogUnitTests, ShutdownTimedOut_BeginShutdownTrueTest)
{
	// Data.
	const DWORD tickCount0 = 10000;
	const DWORD tickCountBeginShutdown = 12000;
	const DWORD tickCountMaxAllowed = tickCountBeginShutdown + CWatchDog::ALLOWED_SHUTDOWN_MILLISECONDS;
	const DWORD tickCountTimedOut = tickCountMaxAllowed + +1;

	// Setup.
	COperatingSystemTest operatingSystem;
	operatingSystem.m_tickCount32 = tickCount0;
	CWatchDog watchdog(operatingSystem);
	ASSERT_FALSE(watchdog.ShutdownTimedOut()) << L"ShutdownTimedOut() setup";

	// Perform operation(s) and test(s).
	operatingSystem.m_tickCount32 = tickCountBeginShutdown;
	watchdog.BeginShutdown("");
	ASSERT_FALSE(watchdog.ShutdownTimedOut()) << L"ShutdownTimedOut() tickCountBeginShutdown";

	operatingSystem.m_tickCount32 = tickCountMaxAllowed;
	ASSERT_FALSE(watchdog.ShutdownTimedOut()) << L"ShutdownTimedOut() tickCountMaxAllowed";
	operatingSystem.m_tickCount32 = tickCountTimedOut;
	ASSERT_EQ(true, watchdog.ShutdownTimedOut()) << L"ShutdownTimedOut() tickCountTimedOut";
}

/// <summary>
/// Functional test for:
///     CWatchDog::CWatchDog()
///     CWatchDog::ShutdownNow()
///     CWatchDog::ShutdownTimedOut()
/// Test returned values after calling ShutdownNow().
/// </summary>
TEST_F(WatchDogUnitTests, ShutdownTimedOut_ShutdownNowTest)
{
	// Data.
	const DWORD tickCount0 = 10000;
	const DWORD tickCount1 = 12000;
	const DWORD tickCount2 = tickCount1 + 2 * CWatchDog::ALLOWED_SHUTDOWN_MILLISECONDS;

	// Setup.
	COperatingSystemTest operatingSystem; // So we can control the tick count.
	operatingSystem.m_tickCount32 = tickCount0;
	CWatchDog watchdog(operatingSystem);
	ASSERT_FALSE(watchdog.ShutdownTimedOut()) << L"ShutdownTimedOut() setup";

	// Perform operation(s) and test(s).
	operatingSystem.m_tickCount32 = tickCount1;
	watchdog.BeginShutdown("exception");
	ASSERT_FALSE(watchdog.ShutdownTimedOut()) << L"ShutdownTimedOut() 1";

	operatingSystem.m_tickCount32 = tickCount2;
	ASSERT_TRUE(watchdog.ShutdownTimedOut()) << L"ShutdownTimedOut() 2";
}
// #pragma endregion ShutdownTimedOut

// #pragma region RegisterThread
/// <summary>
/// Functional test for:
///     CWatchDog::RegisterThread(LPCTSTR, int)
/// Test that the function sets the correct value(s).
/// </summary>
TEST_F(WatchDogUnitTests, RegisterThread_CurrentIdTest)
{
	// Data.
	const LPCTSTR name(_T("threadNameOne"));
	const thread::id threadID(CUtilities::GetCurrentThreadId());
	const int timeOutSeconds(150);
	const DWORD timeOutMilliseconds((DWORD)timeOutSeconds * MILLISECONDS_PER_SECOND);
	const DWORD tickCount0 = 10000;
	const DWORD tickCountRegister = 12000;

	// Setup.
	COperatingSystemTest operatingSystem; // So we can control the tick count.
	operatingSystem.m_tickCount32 = tickCount0;
	CWatchDog watchdog(operatingSystem);
	ASSERT_EQ((size_t)0, watchdog.m_threadList.size()) << L"m_threadList.size() setup";

	// Perform operation(s) and test(s).
	operatingSystem.m_tickCount32 = tickCountRegister;
	watchdog.RegisterThread(name, CUtilities::GetCurrentThreadId(), timeOutSeconds, 0);
	ASSERT_EQ((size_t)1, watchdog.m_threadList.size()) << L"m_threadList.size()";
	CWatchDog::WatchdogThreadMap::const_iterator pair(watchdog.m_threadList.find(threadID));
	ASSERT_NE(watchdog.m_threadList.cend(), pair) << L"pair";
	ASSERT_EQ(threadID, pair->first) << L"key";
	ASSERT_EQ(string(name), string(pair->second.GetName())) << L"GetName() string";
	ASSERT_EQ(timeOutMilliseconds, pair->second.GetTimeOutMilliseconds()) << L"GetTimeOutMilliseconds()";
	ASSERT_EQ(tickCountRegister, pair->second.GetCheckInTime()) << L"GetCheckInTime()";
}

/// <summary>
/// Functional test for:
///     CWatchDog::RegisterThread(LPCTSTR, DWORD, int)
/// Test that the function sets the correct value(s).
/// </summary>
TEST_F(WatchDogUnitTests, RegisterThread_ThreadIdTest)
{
	// Data.
	const LPCTSTR name(_T("threadNameOne"));
	const thread::id threadID(0x12345);
	const int timeOutSeconds(150);
	const DWORD timeOutMilliseconds((DWORD)timeOutSeconds * MILLISECONDS_PER_SECOND);
	const DWORD tickCount0 = 10000;
	const DWORD tickCountRegister = 12000;

	// Setup.
	COperatingSystemTest operatingSystem; // So we can control the tick count.
	operatingSystem.m_tickCount32 = tickCount0;
	CWatchDog watchdog(operatingSystem);
	ASSERT_EQ((size_t)0, watchdog.m_threadList.size()) << L"m_threadList.size() setup";

	// Perform operation(s) and test(s).
	operatingSystem.m_tickCount32 = tickCountRegister;
	watchdog.RegisterThread(name, threadID, timeOutSeconds, 0);
	ASSERT_EQ((size_t)1, watchdog.m_threadList.size()) << L"m_threadList.size()";
	CWatchDog::WatchdogThreadMap::const_iterator pair(watchdog.m_threadList.find(threadID));
	ASSERT_NE(watchdog.m_threadList.cend(), pair) << L"pair";
	ASSERT_EQ(threadID, pair->first) << L"key";
	ASSERT_EQ(string(name), string(pair->second.GetName())) << L"GetName() string";
	ASSERT_EQ(timeOutMilliseconds, pair->second.GetTimeOutMilliseconds()) << L"GetTimeOutMilliseconds()";
	ASSERT_EQ(tickCountRegister, pair->second.GetCheckInTime()) << L"GetCheckInTime()";
}

/// <summary>
/// Functional test for:
///     CWatchDog::RegisterThread(LPCTSTR, DWORD, int)
/// Test that the function overwrites the previous value(s) when the threadID matches.
/// </summary>
TEST_F(WatchDogUnitTests, RegisterThread_ThreadIdDupTest)
{
	// Data.
	const LPCTSTR name(_T("threadNameOne"));
	const thread::id threadID(0x12345);
	const int timeOutSeconds(150);
	const DWORD timeOutMilliseconds((DWORD)timeOutSeconds * MILLISECONDS_PER_SECOND);
	const DWORD tickCount0 = 10000;
	const DWORD tickCountRegister = 12000;

	const LPCTSTR name2(_T("threadNameTwoIsLonger"));
	const int timeOutSeconds2(200);
	const DWORD timeOutMilliseconds2((DWORD)timeOutSeconds2 * MILLISECONDS_PER_SECOND);
	const DWORD tickCount2 = 14000;

	// Setup.
	COperatingSystemTest operatingSystem; // So we can control the tick count.
	operatingSystem.m_tickCount32 = tickCount0;
	CWatchDog watchdog(operatingSystem);

	operatingSystem.m_tickCount32 = tickCountRegister;
	watchdog.RegisterThread(name, threadID, timeOutSeconds, 0);
	{
		ASSERT_EQ((size_t)1, watchdog.m_threadList.size()) << L"m_threadList.size() setup";
		CWatchDog::WatchdogThreadMap::const_iterator pair(watchdog.m_threadList.find(threadID));
		ASSERT_NE(watchdog.m_threadList.cend(), pair) << L"pair";
		ASSERT_EQ(threadID, pair->first) << L"key";
		ASSERT_EQ(string(name), string(pair->second.GetName())) << L"GetName() string";
		ASSERT_EQ(timeOutMilliseconds, pair->second.GetTimeOutMilliseconds()) << L"GetTimeOutMilliseconds()";
		ASSERT_EQ(tickCountRegister, pair->second.GetCheckInTime()) << L"GetCheckInTime()";
	}

	// Perform operation(s) and test(s).
	operatingSystem.m_tickCount32 = tickCount2;
	watchdog.RegisterThread(name2, threadID, timeOutSeconds2, 0);
	{
		ASSERT_EQ((size_t)1, watchdog.m_threadList.size()) << L"m_threadList.size()";
		CWatchDog::WatchdogThreadMap::const_iterator pair(watchdog.m_threadList.find(threadID));
		ASSERT_NE(watchdog.m_threadList.cend(), pair) << L"pair";
		ASSERT_EQ(threadID, pair->first) << L"key";
		ASSERT_EQ(string(name2), string(pair->second.GetName())) << L"GetName() string";
		ASSERT_EQ(timeOutMilliseconds2, pair->second.GetTimeOutMilliseconds()) << L"GetTimeOutMilliseconds()";
		ASSERT_EQ(tickCount2, pair->second.GetCheckInTime()) << L"GetCheckInTime()";
	}
}

/// <summary>
/// Functional test for:
///     CWatchDog::RegisterThread(LPCTSTR, DWORD, int)
/// Test that the function sets the correct value(s) for two different threads.
/// </summary>
TEST_F(WatchDogUnitTests, RegisterThread_ThreadId12Test)
{
	// Data.
	const LPCTSTR name1(_T("threadNameOne"));
	const thread::id threadID1(0x12345);
	const int timeOutSeconds1(150);
	const DWORD timeOutMilliseconds1((DWORD)timeOutSeconds1 * MILLISECONDS_PER_SECOND);
	const DWORD tickCount0 = 10000;
	const DWORD tickCount1 = 12000;

	const LPCTSTR name2(_T("threadNameTwoIsLonger"));
	const thread::id threadID2(0x23456);
	const int timeOutSeconds2(200);
	const DWORD timeOutMilliseconds2((DWORD)timeOutSeconds2 * MILLISECONDS_PER_SECOND);
	const DWORD tickCount2 = 14000;

	// Setup.
	COperatingSystemTest operatingSystem; // So we can control the tick count.
	operatingSystem.m_tickCount32 = tickCount0;
	CWatchDog watchdog(operatingSystem);

	// Perform operation(s) and test(s).
	operatingSystem.m_tickCount32 = tickCount1;
	watchdog.RegisterThread(name1, threadID1, timeOutSeconds1, 0);
	operatingSystem.m_tickCount32 = tickCount2;
	watchdog.RegisterThread(name2, threadID2, timeOutSeconds2, 0);
	ASSERT_EQ((size_t)2, watchdog.m_threadList.size()) << L"m_threadList.size()";

	{
		CWatchDog::WatchdogThreadMap::const_iterator pair(watchdog.m_threadList.find(threadID1));
		ASSERT_NE(watchdog.m_threadList.cend(), pair) << L"pair 1";
		ASSERT_EQ(threadID1, pair->first) << L"key 1";
		ASSERT_EQ(string(name1), string(pair->second.GetName())) << L"GetName() string 1";
		ASSERT_EQ(timeOutMilliseconds1, pair->second.GetTimeOutMilliseconds()) << L"GetTimeOutMilliseconds() 1";
		ASSERT_EQ(tickCount1, pair->second.GetCheckInTime()) << L"GetCheckInTime() 1";
	}

	{
		CWatchDog::WatchdogThreadMap::const_iterator pair(watchdog.m_threadList.find(threadID2));
		ASSERT_NE(watchdog.m_threadList.cend(), pair) << L"pair 2";
		ASSERT_EQ(threadID2, pair->first) << L"key 2";
		ASSERT_EQ(string(name2), string(pair->second.GetName())) << L"GetName() string 2";
		ASSERT_EQ(timeOutMilliseconds2, pair->second.GetTimeOutMilliseconds()) << L"GetTimeOutMilliseconds() 2";
		ASSERT_EQ(tickCount2, pair->second.GetCheckInTime()) << L"GetCheckInTime() 2";
	}
}
// #pragma endregion RegisterThread

// #pragma region DeregisterThread_CurrentId
/// <summary>
/// Functional test for:
///     CWatchDog::DeregisterThread()
/// Test deregistering the current thread when there are no threads registered.
/// </summary>
TEST_F(WatchDogUnitTests, DeregisterThread_CurrentIdNoneTest)
{
	// Data.
	const LPCTSTR name(_T("threadNameOne"));
	const thread::id threadID(CUtilities::GetCurrentThreadId());
	const int timeOutSeconds(150);

	// Setup.
	CWatchDog watchdog;
	ASSERT_EQ((size_t)0, watchdog.m_threadList.size()) << L"m_threadList.size() setup";

	// Perform operation(s) and test(s).
	watchdog.DeregisterThread();
	ASSERT_EQ((size_t)0, watchdog.m_threadList.size()) << L"m_threadList.size()";
}

/// <summary>
/// Functional test for:
///     CWatchDog::DeregisterThread()
/// Test deregistering the current thread when the thread is not registered.
/// </summary>
TEST_F(WatchDogUnitTests, DeregisterThread_CurrentId0Test)
{
	// Data.
	const thread::id threadID(CUtilities::GetCurrentThreadId());

	const LPCTSTR name1(_T("threadNameOne"));
	const thread::id threadID1(0x12345678);
	const int timeOutSeconds1(150);

	// Setup.
	CWatchDog watchdog;
	watchdog.RegisterThread(name1, threadID1, timeOutSeconds1, 0);
	ASSERT_EQ((size_t)1, watchdog.m_threadList.size()) << L"m_threadList.size() setup";

	// Perform operation(s) and test(s).
	watchdog.DeregisterThread();
	ASSERT_EQ((size_t)1, watchdog.m_threadList.size()) << L"m_threadList.size()";
	CWatchDog::WatchdogThreadMap::const_iterator pair(watchdog.m_threadList.find(threadID1));
	ASSERT_NE(watchdog.m_threadList.cend(), pair) << L"pair 1";
	ASSERT_EQ(threadID1, pair->first) << L"key 1";
	ASSERT_EQ(string(name1), string(pair->second.GetName())) << L"GetName() string 1";
}

/// <summary>
/// Functional test for:
///     CWatchDog::DeregisterThread()
/// Test deregistering the current thread when the thread is registered.
/// </summary>
TEST_F(WatchDogUnitTests, DeregisterThread_CurrentId1Test)
{
	// Data.
	const LPCTSTR name1(_T("threadNameOne"));
	const thread::id threadID1(CUtilities::GetCurrentThreadId());
	const int timeOutSeconds1(150);

	// Setup.
	CWatchDog watchdog;
	watchdog.RegisterThread(name1, threadID1, timeOutSeconds1, 0);
	ASSERT_EQ((size_t)1, watchdog.m_threadList.size()) << L"m_threadList.size() setup";

	// Perform operation(s) and test(s).
	watchdog.DeregisterThread();
	ASSERT_EQ((size_t)0, watchdog.m_threadList.size()) << L"m_threadList.size()";
}

/// <summary>
/// Functional test for:
///     CWatchDog::DeregisterThread()
/// Test deregistering the current thread when it was the first of two threads registered.
/// </summary>
TEST_F(WatchDogUnitTests, DeregisterThread_CurrentId12Test)
{
	// Data.
	const LPCTSTR name1(_T("threadNameOne"));
	const thread::id threadID1(CUtilities::GetCurrentThreadId());
	const int timeOutSeconds1(150);

	const LPCTSTR name2(_T("threadNameTwoIsLonger"));
	const thread::id threadID2(0x23456789);
	const int timeOutSeconds2(200);

	// Setup.
	CWatchDog watchdog;
	watchdog.RegisterThread(name1, threadID1, timeOutSeconds1, 0);
	watchdog.RegisterThread(name2, threadID2, timeOutSeconds2, 0);
	ASSERT_EQ((size_t)2, watchdog.m_threadList.size()) << L"m_threadList.size() setup";

	// Perform operation(s) and test(s).
	watchdog.DeregisterThread();
	ASSERT_EQ((size_t)1, watchdog.m_threadList.size()) << L"m_threadList.size()";
	CWatchDog::WatchdogThreadMap::const_iterator pair(watchdog.m_threadList.find(threadID2));
	ASSERT_NE(watchdog.m_threadList.cend(), pair) << L"pair 2";
	ASSERT_EQ(threadID2, pair->first) << L"key 2";
	ASSERT_EQ(string(name2), string(pair->second.GetName())) << L"GetName() string 2";
}

/// <summary>
/// Functional test for:
///     CWatchDog::DeregisterThread()
/// Test deregistering the current thread when it was the second of two threads registered.
/// </summary>
TEST_F(WatchDogUnitTests, DeregisterThread_CurrentId21Test)
{
	// Data.
	const LPCTSTR name2(_T("threadNameTwoIsLonger"));
	const thread::id threadID2(CUtilities::GetCurrentThreadId());
	const int timeOutSeconds2(200);

	const LPCTSTR name1(_T("threadNameOne"));
	const thread::id threadID1(0x12345678);
	const int timeOutSeconds1(150);

	// Setup.
	CWatchDog watchdog;
	watchdog.RegisterThread(name1, threadID1, timeOutSeconds1, 0);
	watchdog.RegisterThread(name2, threadID2, timeOutSeconds2, 0);
	ASSERT_EQ((size_t)2, watchdog.m_threadList.size()) << L"m_threadList.size() setup";

	// Perform operation(s) and test(s).
	watchdog.DeregisterThread();
	ASSERT_EQ((size_t)1, watchdog.m_threadList.size()) << L"m_threadList.size()";
	CWatchDog::WatchdogThreadMap::const_iterator pair(watchdog.m_threadList.find(threadID1));
	ASSERT_NE(watchdog.m_threadList.cend(), pair) << L"pair 1";
	ASSERT_EQ(threadID1, pair->first) << L"key 1";
	ASSERT_EQ(string(name1), string(pair->second.GetName())) << L"GetName() string 1";
}
// #pragma endregion DeregisterThread_CurrentId

// #pragma region DeregisterThread_ThreadId
/// <summary>
/// Functional test for:
///     CWatchDog::DeregisterThread(DWORD)
/// Test deregistering the specified thread when there are no threads registered.
/// </summary>
TEST_F(WatchDogUnitTests, DeregisterThread_ThreadIdNoneTest)
{
	// Data.
	const LPCTSTR name(_T("threadNameOne"));
	const thread::id threadID(0x12345);
	const int timeOutSeconds(150);

	// Setup.
	CWatchDog watchdog;
	ASSERT_EQ((size_t)0, watchdog.m_threadList.size()) << L"m_threadList.size() setup";

	// Perform operation(s) and test(s).
	watchdog.DeregisterThread(threadID);
	ASSERT_EQ((size_t)0, watchdog.m_threadList.size()) << L"m_threadList.size()";
}

/// <summary>
/// Functional test for:
///     CWatchDog::DeregisterThread(DWORD)
/// Test deregistering the specified thread when the thread is not registered.
/// </summary>
TEST_F(WatchDogUnitTests, DeregisterThread_ThreadId0Test)
{
	// Data.
	const thread::id threadID(0x12345);

	const LPCTSTR name1(_T("threadNameOne"));
	const thread::id threadID1(0x12345678);
	const int timeOutSeconds1(150);

	// Setup.
	CWatchDog watchdog;
	watchdog.RegisterThread(name1, threadID1, timeOutSeconds1, 0);
	ASSERT_EQ((size_t)1, watchdog.m_threadList.size()) << L"m_threadList.size() setup";

	// Perform operation(s) and test(s).
	watchdog.DeregisterThread(threadID);
	ASSERT_EQ((size_t)1, watchdog.m_threadList.size()) << L"m_threadList.size()";
	CWatchDog::WatchdogThreadMap::const_iterator pair(watchdog.m_threadList.find(threadID1));
	ASSERT_NE(watchdog.m_threadList.cend(), pair) << L"pair 1";
	ASSERT_EQ(threadID1, pair->first) << L"key 1";
	ASSERT_EQ(string(name1), string(pair->second.GetName())) << L"GetName() string 1";
}

/// <summary>
/// Functional test for:
///     CWatchDog::DeregisterThread(DWORD)
/// Test deregistering the specified thread when the thread is registered.
/// </summary>
TEST_F(WatchDogUnitTests, DeregisterThread_ThreadId1Test)
{
	// Data.
	const LPCTSTR name1(_T("threadNameOne"));
	const thread::id threadID1(0x12345);
	const int timeOutSeconds1(150);

	// Setup.
	CWatchDog watchdog;
	watchdog.RegisterThread(name1, threadID1, timeOutSeconds1, 0);
	ASSERT_EQ((size_t)1, watchdog.m_threadList.size()) << L"m_threadList.size() setup";

	// Perform operation(s) and test(s).
	watchdog.DeregisterThread(threadID1);
	ASSERT_EQ((size_t)0, watchdog.m_threadList.size()) << L"m_threadList.size()";
}

/// <summary>
/// Functional test for:
///     CWatchDog::DeregisterThread(DWORD)
/// Test deregistering the specified thread when it was the first of two threads registered.
/// </summary>
TEST_F(WatchDogUnitTests, DeregisterThread_ThreadId12Test)
{
	// Data.
	const LPCTSTR name1(_T("threadNameOne"));
	const thread::id threadID1(0x12345);
	const int timeOutSeconds1(150);

	const LPCTSTR name2(_T("threadNameTwoIsLonger"));
	const thread::id threadID2(0x23456789);
	const int timeOutSeconds2(200);

	// Setup.
	CWatchDog watchdog;
	watchdog.RegisterThread(name1, threadID1, timeOutSeconds1, 0);
	watchdog.RegisterThread(name2, threadID2, timeOutSeconds2, 0);
	ASSERT_EQ((size_t)2, watchdog.m_threadList.size()) << L"m_threadList.size() setup";

	// Perform operation(s) and test(s).
	watchdog.DeregisterThread(threadID1);
	ASSERT_EQ((size_t)1, watchdog.m_threadList.size()) << L"m_threadList.size()";
	CWatchDog::WatchdogThreadMap::const_iterator pair(watchdog.m_threadList.find(threadID2));
	ASSERT_NE(watchdog.m_threadList.cend(), pair) << L"pair 2";
	ASSERT_EQ(threadID2, pair->first) << L"key 2";
	ASSERT_EQ(string(name2), string(pair->second.GetName())) << L"GetName() string 2";
}

/// <summary>
/// Functional test for:
///     CWatchDog::DeregisterThread(DWORD)
/// Test deregistering the specified thread when it was the second of two threads registered.
/// </summary>
TEST_F(WatchDogUnitTests, DeregisterThread_ThreadId21Test)
{
	// Data.
	const LPCTSTR name2(_T("threadNameTwoIsLonger"));
	const thread::id threadID2(0x12345);
	const int timeOutSeconds2(200);

	const LPCTSTR name1(_T("threadNameOne"));
	const thread::id threadID1(0x12345678);
	const int timeOutSeconds1(150);

	// Setup.
	CWatchDog watchdog;
	watchdog.RegisterThread(name1, threadID1, timeOutSeconds1, 0);
	watchdog.RegisterThread(name2, threadID2, timeOutSeconds2, 0);
	ASSERT_EQ((size_t)2, watchdog.m_threadList.size()) << L"m_threadList.size() setup";

	// Perform operation(s) and test(s).
	watchdog.DeregisterThread(threadID2);
	ASSERT_EQ((size_t)1, watchdog.m_threadList.size()) << L"m_threadList.size()";
	CWatchDog::WatchdogThreadMap::const_iterator pair(watchdog.m_threadList.find(threadID1));
	ASSERT_NE(watchdog.m_threadList.cend(), pair) << L"pair 1";
	ASSERT_EQ(threadID1, pair->first) << L"key 1";
	ASSERT_EQ(string(name1), string(pair->second.GetName())) << L"GetName() string 1";
}

/// <summary>
/// Functional test for:
///     CWatchDog::DeregisterThread(DWORD)
/// Test deregistering two threads in the same order they were registered.
/// </summary>
TEST_F(WatchDogUnitTests, DeregisterThread_ThreadId22Test)
{
	// Data.
	const LPCTSTR name1(_T("threadNameOne"));
	const thread::id threadID1(0x12345);
	const int timeOutSeconds1(150);

	const LPCTSTR name2(_T("threadNameTwoIsLonger"));
	const thread::id threadID2(0x23456789);
	const int timeOutSeconds2(200);

	// Setup.
	CWatchDog watchdog;
	watchdog.RegisterThread(name1, threadID1, timeOutSeconds1, 0);
	watchdog.RegisterThread(name2, threadID2, timeOutSeconds2, 0);
	ASSERT_EQ((size_t)2, watchdog.m_threadList.size()) << L"m_threadList.size() setup";

	// Perform operation(s) and test(s).
	watchdog.DeregisterThread(threadID1);
	ASSERT_EQ((size_t)1, watchdog.m_threadList.size()) << L"m_threadList.size() 1";
	CWatchDog::WatchdogThreadMap::const_iterator pair(watchdog.m_threadList.find(threadID2));
	ASSERT_NE(watchdog.m_threadList.cend(), pair) << L"pair 2";
	ASSERT_EQ(threadID2, pair->first) << L"key 2";
	ASSERT_EQ(string(name2), string(pair->second.GetName())) << L"GetName() string 2";

	watchdog.DeregisterThread(threadID2);
	ASSERT_EQ((size_t)0, watchdog.m_threadList.size()) << L"m_threadList.size() 0";
}

/// <summary>
/// Functional test for:
///     CWatchDog::DeregisterThread(DWORD)
/// Test deregistering two threads in the reverse order they were registered.
/// </summary>
TEST_F(WatchDogUnitTests, DeregisterThread_ThreadId22ReverseTest)
{
	// Data.
	const LPCTSTR name1(_T("threadNameOne"));
	const thread::id threadID1(0x12345);
	const int timeOutSeconds1(150);

	const LPCTSTR name2(_T("threadNameTwoIsLonger"));
	const thread::id threadID2(0x23456789);
	const int timeOutSeconds2(200);

	// Setup.
	CWatchDog watchdog;
	watchdog.RegisterThread(name1, threadID1, timeOutSeconds1, 0);
	watchdog.RegisterThread(name2, threadID2, timeOutSeconds2, 0);
	ASSERT_EQ((size_t)2, watchdog.m_threadList.size()) << L"m_threadList.size() setup";

	// Perform operation(s) and test(s).
	watchdog.DeregisterThread(threadID2);
	ASSERT_EQ((size_t)1, watchdog.m_threadList.size()) << L"m_threadList.size() 1";
	CWatchDog::WatchdogThreadMap::const_iterator pair(watchdog.m_threadList.find(threadID1));
	ASSERT_NE(watchdog.m_threadList.cend(), pair) << L"pair 1";
	ASSERT_EQ(threadID1, pair->first) << L"key 1";
	ASSERT_EQ(string(name1), string(pair->second.GetName())) << L"GetName() string 1";

	watchdog.DeregisterThread(threadID1);
	ASSERT_EQ((size_t)0, watchdog.m_threadList.size()) << L"m_threadList.size() 0";
}
// #pragma endregion DeregisterThread_ThreadId

void WatchDogUnitTests::AssertThreadCheckInTime(const CWatchDog &watchdog, thread::id threadID, DWORD checkInTime, LPCWSTR desc)
{
	const CWatchDog::WatchdogThreadMap::const_iterator pair(watchdog.m_threadList.find(threadID));
	ASSERT_NE(watchdog.m_threadList.cend(), pair) << L"pair " + wstring(desc);
	ASSERT_EQ(threadID, pair->first) << L"key " + wstring(desc);
	ASSERT_EQ(checkInTime, pair->second.GetCheckInTime()) << L"GetCheckInTime() " + wstring(desc);
}

// #pragma region Checkin_CurrentId
/// <summary>
/// Code coverage for:
///     CWatchDog::Checkin()
/// Checkin with the current threadId when no threads are registered.
/// </summary>
TEST_F(WatchDogUnitTests, CheckIn_CurrentIdNoneTest)
{
	// Setup.
	CWatchDog watchdog;

	// Perform operation(s) and test(s).
	watchdog.Checkin();
	ASSERT_EQ((size_t)0, watchdog.m_threadList.size()) << L"m_threadList.size()";
}

/// <summary>
/// Code coverage for:
///     CWatchDog::Checkin()
/// Checkin with the current threadId when the thread is not registered.
/// </summary>
TEST_F(WatchDogUnitTests, CheckIn_CurrentId0Test)
{
	// Data.
	const thread::id threadID(CUtilities::GetCurrentThreadId());

	const LPCTSTR name1(_T("threadNameOne"));
	const thread::id threadID1(0x12345678);
	const int timeOutSeconds1(150);
	const DWORD tickCount0 = 10000;
	const DWORD tickCount1 = 12000;
	const DWORD tickCount2 = 14000;

	// Setup.
	COperatingSystemTest operatingSystem; // So we can control the tick count.
	operatingSystem.m_tickCount32 = tickCount0;
	CWatchDog watchdog(operatingSystem);

	operatingSystem.m_tickCount32 = tickCount1;
	watchdog.RegisterThread(name1, threadID1, timeOutSeconds1, 0);
	ASSERT_EQ((size_t)1, watchdog.m_threadList.size()) << L"m_threadList.size() setup";
	AssertThreadCheckInTime(watchdog, threadID1, tickCount1, L"setup");

	// Perform operation(s) and test(s).
	operatingSystem.m_tickCount32 = tickCount2;
	watchdog.Checkin(threadID);
	ASSERT_EQ((size_t)1, watchdog.m_threadList.size()) << L"m_threadList.size()";
	AssertThreadCheckInTime(watchdog, threadID1, tickCount1, L"2");
}

/// <summary>
/// Code coverage for:
///     CWatchDog::Checkin()
/// Checkin with the current threadId when the thread is registered.
/// </summary>
TEST_F(WatchDogUnitTests, CheckIn_CurrentId1Test)
{
	// Data.
	const LPCTSTR name1(_T("threadNameOne"));
	const thread::id threadID1(CUtilities::GetCurrentThreadId());
	const int timeOutSeconds1(150);
	const DWORD tickCount0 = 10000;
	const DWORD tickCount1 = 12000;
	const DWORD tickCount2 = 14000;

	// Setup.
	COperatingSystemTest operatingSystem; // So we can control the tick count.
	operatingSystem.m_tickCount32 = tickCount0;
	CWatchDog watchdog(operatingSystem);

	operatingSystem.m_tickCount32 = tickCount1;
	watchdog.RegisterThread(name1, threadID1, timeOutSeconds1, 0);
	ASSERT_EQ((size_t)1, watchdog.m_threadList.size()) << L"m_threadList.size() setup";
	AssertThreadCheckInTime(watchdog, threadID1, tickCount1, L"setup");

	// Perform operation(s) and test(s).
	operatingSystem.m_tickCount32 = tickCount2;
	watchdog.Checkin(threadID1);
	ASSERT_EQ((size_t)1, watchdog.m_threadList.size()) << L"m_threadList.size()";
	AssertThreadCheckInTime(watchdog, threadID1, tickCount2, L"2");
}
// #pragma endregion Checkin_CurrentId

// #pragma region Checkin_ThreadId
/// <summary>
/// Code coverage for:
///     CWatchDog::Checkin()
/// Checkin with the current threadId when no threads are registered.
/// </summary>
TEST_F(WatchDogUnitTests, CheckIn_ThreadIdNoneTest)
{
	// Data.
	const thread::id threadID(0x333);

	// Setup.
	CWatchDog watchdog;

	// Perform operation(s) and test(s).
	watchdog.Checkin(threadID);
	ASSERT_EQ((size_t)0, watchdog.m_threadList.size()) << L"m_threadList.size()";
}

/// <summary>
/// Code coverage for:
///     CWatchDog::Checkin()
/// Checkin with a specific threadId when the thread is not registered.
/// </summary>
TEST_F(WatchDogUnitTests, CheckIn_ThreadId0Test)
{
	// Data.
	const LPCTSTR name1(_T("threadNameOne"));
	const thread::id threadID1(0x12345);
	const int timeOutSeconds1(150);
	const DWORD tickCount0 = 10000;
	const DWORD tickCount1 = 12000;
	const DWORD tickCount2 = 14000;

	const thread::id threadID(0x333);

	// Setup.
	COperatingSystemTest operatingSystem; // So we can control the tick count.
	operatingSystem.m_tickCount32 = tickCount0;
	CWatchDog watchdog(operatingSystem);

	operatingSystem.m_tickCount32 = tickCount1;
	watchdog.RegisterThread(name1, threadID1, timeOutSeconds1, 0);
	ASSERT_EQ((size_t)1, watchdog.m_threadList.size()) << L"m_threadList.size() setup";
	AssertThreadCheckInTime(watchdog, threadID1, tickCount1, L"setup");

	// Perform operation(s) and test(s).
	operatingSystem.m_tickCount32 = tickCount2;
	watchdog.Checkin(threadID);
	ASSERT_EQ((size_t)1, watchdog.m_threadList.size()) << L"m_threadList.size()";
	AssertThreadCheckInTime(watchdog, threadID1, tickCount1, L"2");
}

/// <summary>
/// Code coverage for:
///     CWatchDog::Checkin()
/// Checkin with a specific threadId when the thread is registered.
/// </summary>
TEST_F(WatchDogUnitTests, CheckIn_ThreadId1Test)
{
	// Data.
	const LPCTSTR name1(_T("threadNameOne"));
	const thread::id threadID1(0x12345);
	const int timeOutSeconds1(150);
	const DWORD tickCount0 = 10000;
	const DWORD tickCount1 = 12000;
	const DWORD tickCount2 = 14000;

	// Setup.
	COperatingSystemTest operatingSystem; // So we can control the tick count.
	operatingSystem.m_tickCount32 = tickCount0;
	CWatchDog watchdog(operatingSystem);

	operatingSystem.m_tickCount32 = tickCount1;
	watchdog.RegisterThread(name1, threadID1, timeOutSeconds1, 0);
	ASSERT_EQ((size_t)1, watchdog.m_threadList.size()) << L"m_threadList.size() setup";

	// Perform operation(s) and test(s).
	operatingSystem.m_tickCount32 = tickCount2;
	watchdog.Checkin(threadID1);
	ASSERT_EQ((size_t)1, watchdog.m_threadList.size()) << L"m_threadList.size()";
}
// #pragma endregion Checkin_ThreadId

// #pragma region CheckForTimeout
/// <summary>
/// Code coverage for:
///     CWatchDog::RegisterThread()
///     CWatchDog::CheckForTimeout()
/// Check for timeout when there are 0 threads registered.
/// </summary>
TEST_F(WatchDogUnitTests, CheckForTimeout_0Test)
{
	// Data.
	const DWORD tickCountSetup = 10000;
	const DWORD tickCountTimedOut = 12000 + 1000 * MILLISECONDS_PER_SECOND;

	// Setup.
	COperatingSystemTest operatingSystem; // So we can control the tick count.
	operatingSystem.m_tickCount32 = tickCountSetup;
	CWatchDog watchdog(operatingSystem);

	operatingSystem.m_tickCount32 = tickCountTimedOut;
	ASSERT_FALSE(watchdog.CheckForTimeout()) << L"CheckForTimeout tickCountTimedOut";
	ASSERT_EQ((size_t)0, watchdog.m_threadList.size()) << L"m_threadList.size() tickCountTimedOut";
}

/// <summary>
/// Code coverage for:
///     CWatchDog::RegisterThread()
///     CWatchDog::CheckForTimeout()
/// Check for timeout when 1 thread does not checkin.
/// </summary>
TEST_F(WatchDogUnitTests, CheckForTimeout_1Test)
{
#if 000//T0DO-PORT revisit
	// Data.
	const LPCTSTR name1(_T("threadNameOne"));
	const thread::id threadID1(0x12345);
	const int timeOutSeconds1(150);
	const DWORD tickCountSetup = 10000;
	const DWORD tickCountRegister = 12000;
	const DWORD tickCountMaxAllowed = tickCountRegister + (DWORD)timeOutSeconds1 * MILLISECONDS_PER_SECOND - 1;
	const DWORD tickCountTimedOut = tickCountMaxAllowed + 1;

	// Setup.
	COperatingSystemTest operatingSystem; // So we can control the tick count.
	operatingSystem.m_tickCount32 = tickCountSetup;
	CWatchDog watchdog(operatingSystem);

	operatingSystem.m_tickCount32 = tickCountRegister;
	watchdog.RegisterThread(name1, threadID1, timeOutSeconds1, 0);
	AssertThreadCheckInTime(watchdog, threadID1, tickCountRegister, L"tickCountRegister 1");
	ASSERT_FALSE(watchdog.CheckForTimeout()) << L"CheckForTimeout tickCountRegister";
	ASSERT_EQ((size_t)1, watchdog.m_threadList.size()) << L"m_threadList.size() tickCountRegister";

	// Perform operation(s) and test(s).
	operatingSystem.m_tickCount32 = tickCountMaxAllowed;
	ASSERT_FALSE(watchdog.CheckForTimeout()) << L"CheckForTimeout tickCountMaxAllowed";
	ASSERT_EQ((size_t)1, watchdog.m_threadList.size()) << L"m_threadList.size() tickCountMaxAllowed";

	operatingSystem.m_tickCount32 = tickCountTimedOut;
	ASSERT_EQ(true, watchdog.CheckForTimeout()) << L"CheckForTimeout tickCountTimedOut";
	ASSERT_EQ((size_t)0, watchdog.m_threadList.size()) << L"m_threadList.size() tickCountTimedOut";
#endif
}

/// <summary>
/// Code coverage for:
///     CWatchDog::RegisterThread()
///     CWatchDog::CheckForTimeout()
/// Check for simultaneous timeout when 2 similar threads do not checkin.
/// </summary>
TEST_F(WatchDogUnitTests, CheckForTimeout_2Test)
{
	// Data.
	const LPCTSTR name1(_T("threadNameOne"));
	const thread::id threadID1(0x12345);
	const int timeOutSeconds1(150);
	const DWORD tickCountSetup = 10000;
	const DWORD tickCountRegister = 12000;
	const DWORD tickCountMaxAllowed1 = tickCountRegister + (DWORD)timeOutSeconds1 * MILLISECONDS_PER_SECOND - 1;
	const DWORD tickCountTimedOut1 = tickCountMaxAllowed1 + 1;

	const LPCTSTR name2(_T("threadNameTwoIsLonger"));
	const thread::id threadID2(0x23456);
	const int timeOutSeconds2(timeOutSeconds1);
	const DWORD tickCountMaxAllowed2 = tickCountTimedOut1 + (DWORD)timeOutSeconds2 * MILLISECONDS_PER_SECOND - 1;
	const DWORD tickCountTimedOut2 = tickCountMaxAllowed2 + 1;

	// Setup.
	COperatingSystemTest operatingSystem; // So we can control the tick count.
	operatingSystem.m_tickCount32 = tickCountSetup;
	CWatchDog watchdog(operatingSystem);

	operatingSystem.m_tickCount32 = tickCountRegister;
	watchdog.RegisterThread(name1, threadID1, timeOutSeconds1, 0);
	watchdog.RegisterThread(name2, threadID2, timeOutSeconds2, 0);
	AssertThreadCheckInTime(watchdog, threadID1, tickCountRegister, L"tickCountRegister 1");
	AssertThreadCheckInTime(watchdog, threadID1, tickCountRegister, L"tickCountRegister 2");
	ASSERT_FALSE(watchdog.CheckForTimeout()) << L"CheckForTimeout tickCountRegister";
	ASSERT_EQ((size_t)2, watchdog.m_threadList.size()) << L"m_threadList.size() tickCountRegister";

	// Perform operation(s) and test(s).
	operatingSystem.m_tickCount32 = tickCountMaxAllowed1;
	ASSERT_FALSE(watchdog.CheckForTimeout()) << L"CheckForTimeout tickCountMaxAllowed1";
	ASSERT_EQ((size_t)2, watchdog.m_threadList.size()) << L"m_threadList.size() tickCountMaxAllowed1";

	// Either thread could timeout first.
	operatingSystem.m_tickCount32 = tickCountTimedOut1;
	ASSERT_EQ(true, watchdog.CheckForTimeout()) << L"CheckForTimeout tickCountTimedOut1";
	ASSERT_EQ((size_t)1, watchdog.m_threadList.size()) << L"m_threadList.size() tickCountTimedOut1";

	// When one thread times out, all other threads have checkin performed automatically.
	{
		const CWatchDog::WatchdogThreadMap::const_iterator pair(watchdog.m_threadList.cbegin());
		ASSERT_NE(watchdog.m_threadList.cend(), pair) << L"pair 2";
		thread::id threadID(pair->first);
		AssertThreadCheckInTime(watchdog, threadID, tickCountTimedOut1, L"2");

		operatingSystem.m_tickCount32 = tickCountMaxAllowed2;
		ASSERT_FALSE(watchdog.CheckForTimeout()) << L"CheckForTimeout tickCountMaxAllowed2";
		ASSERT_EQ((size_t)1, watchdog.m_threadList.size()) << L"m_threadList.size() tickCountMaxAllowed2";

		operatingSystem.m_tickCount32 = tickCountTimedOut2;
		ASSERT_EQ(true, watchdog.CheckForTimeout()) << L"CheckForTimeout tickCountTimedOut2";
		ASSERT_EQ((size_t)0, watchdog.m_threadList.size()) << L"m_threadList.size() tickCountTimedOut2";
	}
}

/// <summary>
/// Code coverage for:
///     CWatchDog::RegisterThread()
///     CWatchDog::CheckForTimeout()
/// Check for timeouts when 2 different threads do not checkin.
/// </summary>
TEST_F(WatchDogUnitTests, CheckForTimeout_12Test)
{
	// Data.
	const LPCTSTR name1(_T("threadNameOne"));
	const thread::id threadID1(0x12345);
	const int timeOutSeconds1(150);
	const DWORD tickCountSetup = 10000;
	const DWORD tickCountRegister = 12000;
	const DWORD tickCountMaxAllowed1 = tickCountRegister + (DWORD)timeOutSeconds1 * MILLISECONDS_PER_SECOND - 1;
	const DWORD tickCountTimedOut1 = tickCountMaxAllowed1 + 1;

	const LPCTSTR name2(_T("threadNameTwoIsLonger"));
	const thread::id threadID2(0x23456);
	const int timeOutSeconds2(200);
	const DWORD tickCountMaxAllowed2 = tickCountTimedOut1 + (DWORD)timeOutSeconds2 * MILLISECONDS_PER_SECOND - 1;
	const DWORD tickCountTimedOut2 = tickCountMaxAllowed2 + 1;

	// Setup.
	COperatingSystemTest operatingSystem; // So we can control the tick count.
	operatingSystem.m_tickCount32 = tickCountSetup;
	CWatchDog watchdog(operatingSystem);

	operatingSystem.m_tickCount32 = tickCountRegister;
	watchdog.RegisterThread(name1, threadID1, timeOutSeconds1, 0);
	watchdog.RegisterThread(name2, threadID2, timeOutSeconds2, 0);
	AssertThreadCheckInTime(watchdog, threadID1, tickCountRegister, L"tickCountRegister 1");
	AssertThreadCheckInTime(watchdog, threadID1, tickCountRegister, L"tickCountRegister 2");
	ASSERT_FALSE(watchdog.CheckForTimeout()) << L"CheckForTimeout tickCountRegister";
	ASSERT_EQ((size_t)2, watchdog.m_threadList.size()) << L"m_threadList.size() tickCountRegister";

	// Perform operation(s) and test(s).
	operatingSystem.m_tickCount32 = tickCountMaxAllowed1;
	ASSERT_FALSE(watchdog.CheckForTimeout()) << L"CheckForTimeout tickCountMaxAllowed1";
	ASSERT_EQ((size_t)2, watchdog.m_threadList.size()) << L"m_threadList.size() tickCountMaxAllowed1";

	// First thread should timeout.
	operatingSystem.m_tickCount32 = tickCountTimedOut1;
	ASSERT_EQ(true, watchdog.CheckForTimeout()) << L"CheckForTimeout tickCountTimedOut 1";
	ASSERT_EQ((size_t)1, watchdog.m_threadList.size()) << L"m_threadList.size() tickCountTimedOut 1";

	// When one thread times out, all other threads have checkin performed automatically.
	{
		AssertThreadCheckInTime(watchdog, threadID2, tickCountTimedOut1, L"2");

		operatingSystem.m_tickCount32 = tickCountMaxAllowed2;
		ASSERT_FALSE(watchdog.CheckForTimeout()) << L"CheckForTimeout tickCountMaxAllowed2";
		ASSERT_EQ((size_t)1, watchdog.m_threadList.size()) << L"m_threadList.size() tickCountMaxAllowed2";

		operatingSystem.m_tickCount32 = tickCountTimedOut2;
		ASSERT_EQ(true, watchdog.CheckForTimeout()) << L"CheckForTimeout tickCountTimedOut2";
		ASSERT_EQ((size_t)0, watchdog.m_threadList.size()) << L"m_threadList.size() tickCountTimedOut2";
	}
}

/// <summary>
/// Code coverage for:
///     CWatchDog::Checkin()
///     CWatchDog::CheckForTimeout()
/// Check for timeout when 1 thread does checkin.
/// </summary>
TEST_F(WatchDogUnitTests, CheckForTimeout_Checkin1Test)
{
	// Data.
	const LPCTSTR name1(_T("threadNameOne"));
	const thread::id threadID1(0x12345);
	const int timeOutSeconds1(150);
	const DWORD tickCountSetup = 10000;
	const DWORD tickCountRegister = 12000;
	const DWORD tickCountCheckin1 = tickCountRegister + (DWORD)timeOutSeconds1 * MILLISECONDS_PER_SECOND / 2;
	const DWORD tickCountMaxAllowed1 = tickCountCheckin1 + (DWORD)timeOutSeconds1 * MILLISECONDS_PER_SECOND - 1;
	const DWORD tickCountTimedOut1 = tickCountMaxAllowed1 + 1;

	// Setup.
	COperatingSystemTest operatingSystem; // So we can control the tick count.
	operatingSystem.m_tickCount32 = tickCountSetup;
	CWatchDog watchdog(operatingSystem);

	operatingSystem.m_tickCount32 = tickCountRegister;
	watchdog.RegisterThread(name1, threadID1, timeOutSeconds1, 0);
	AssertThreadCheckInTime(watchdog, threadID1, tickCountRegister, L"tickCountRegister 1");
	ASSERT_FALSE(watchdog.CheckForTimeout()) << L"CheckForTimeout tickCountRegister";
	ASSERT_EQ((size_t)1, watchdog.m_threadList.size()) << L"m_threadList.size() tickCountRegister";

	// Perform operation(s) and test(s).
	operatingSystem.m_tickCount32 = tickCountCheckin1;
	watchdog.Checkin(threadID1);
	AssertThreadCheckInTime(watchdog, threadID1, tickCountCheckin1, L"tickCountCheckin1");
	ASSERT_FALSE(watchdog.CheckForTimeout()) << L"CheckForTimeout tickCountCheckin1";
	ASSERT_EQ((size_t)1, watchdog.m_threadList.size()) << L"m_threadList.size() tickCountCheckin1";

	operatingSystem.m_tickCount32 = tickCountMaxAllowed1;
	ASSERT_FALSE(watchdog.CheckForTimeout()) << L"CheckForTimeout tickCountMaxAllowed1";
	ASSERT_EQ((size_t)1, watchdog.m_threadList.size()) << L"m_threadList.size() tickCountMaxAllowed1";

	operatingSystem.m_tickCount32 = tickCountTimedOut1;
	ASSERT_EQ(true, watchdog.CheckForTimeout()) << L"CheckForTimeout tickCountTimedOut1";
	ASSERT_EQ((size_t)0, watchdog.m_threadList.size()) << L"m_threadList.size() tickCountTimedOut1";
}

/// <summary>
/// Code coverage for:
///     CWatchDog::Checkin()
///     CWatchDog::CheckForTimeout()
/// Check for simultaneous timeout when 2 similar threads do checkin.
/// </summary>
TEST_F(WatchDogUnitTests, CheckForTimeout_Checkin2Test)
{
	// Data.
	const LPCTSTR name1(_T("threadNameOne"));
	const thread::id threadID1(0x12345);
	const int timeOutSeconds1(150);
	const DWORD tickCountSetup = 10000;
	const DWORD tickCountRegister = 12000;
	const DWORD tickCountCheckin1 = tickCountRegister + (DWORD)timeOutSeconds1 * MILLISECONDS_PER_SECOND / 2;
	const DWORD tickCountMaxAllowed1 = tickCountCheckin1 + (DWORD)timeOutSeconds1 * MILLISECONDS_PER_SECOND - 1;
	const DWORD tickCountTimedOut1 = tickCountMaxAllowed1 + 1;

	const LPCTSTR name2(_T("threadNameTwoIsLonger"));
	const thread::id threadID2(0x23456);
	const int timeOutSeconds2(timeOutSeconds1);
	const DWORD tickCountCheckin2 = tickCountTimedOut1 + (DWORD)timeOutSeconds2 * MILLISECONDS_PER_SECOND / 2;
	const DWORD tickCountMaxAllowed2 = tickCountCheckin2 + (DWORD)timeOutSeconds2 * MILLISECONDS_PER_SECOND - 1;
	const DWORD tickCountTimedOut2 = tickCountMaxAllowed2 + 1;

	// Setup.
	COperatingSystemTest operatingSystem; // So we can control the tick count.
	operatingSystem.m_tickCount32 = tickCountSetup;
	CWatchDog watchdog(operatingSystem);

	operatingSystem.m_tickCount32 = tickCountRegister;
	watchdog.RegisterThread(name1, threadID1, timeOutSeconds1, 0);
	watchdog.RegisterThread(name2, threadID2, timeOutSeconds2, 0);
	AssertThreadCheckInTime(watchdog, threadID1, tickCountRegister, L"tickCountRegister 1");
	AssertThreadCheckInTime(watchdog, threadID1, tickCountRegister, L"tickCountRegister 2");
	ASSERT_FALSE(watchdog.CheckForTimeout()) << L"CheckForTimeout tickCountRegister";
	ASSERT_EQ((size_t)2, watchdog.m_threadList.size()) << L"m_threadList.size() tickCountRegister";

	// Perform operation(s) and test(s).
	operatingSystem.m_tickCount32 = tickCountCheckin1;
	watchdog.Checkin(threadID1);
	watchdog.Checkin(threadID2);
	AssertThreadCheckInTime(watchdog, threadID1, tickCountCheckin1, L"tickCountCheckin1 1");
	AssertThreadCheckInTime(watchdog, threadID2, tickCountCheckin1, L"tickCountCheckin1 2");
	ASSERT_FALSE(watchdog.CheckForTimeout()) << L"CheckForTimeout tickCountCheckin1";
	ASSERT_EQ((size_t)2, watchdog.m_threadList.size()) << L"m_threadList.size() tickCountCheckin1";

	operatingSystem.m_tickCount32 = tickCountMaxAllowed1;
	ASSERT_FALSE(watchdog.CheckForTimeout()) << L"CheckForTimeout tickCountMaxAllowed1";
	ASSERT_EQ((size_t)2, watchdog.m_threadList.size()) << L"m_threadList.size() tickCountMaxAllowed1";

	// Either thread could timeout first.
	operatingSystem.m_tickCount32 = tickCountTimedOut1;
	ASSERT_EQ(true, watchdog.CheckForTimeout()) << L"CheckForTimeout tickCountTimedOut1";
	ASSERT_EQ((size_t)1, watchdog.m_threadList.size()) << L"m_threadList.size() tickCountTimedOut1";

	// When one thread times out, all other threads have checkin performed automatically.
	{
		const CWatchDog::WatchdogThreadMap::const_iterator pair(watchdog.m_threadList.cbegin());
		ASSERT_NE(watchdog.m_threadList.cend(), pair) << L"pair 2";
		thread::id threadID(pair->first);
		AssertThreadCheckInTime(watchdog, threadID, tickCountTimedOut1, L"2");

		operatingSystem.m_tickCount32 = tickCountCheckin2;
		watchdog.Checkin(threadID);
		AssertThreadCheckInTime(watchdog, threadID, tickCountCheckin2, L"tickCountCheckin2");
		ASSERT_FALSE(watchdog.CheckForTimeout()) << L"CheckForTimeout tickCountCheckin2";
		ASSERT_EQ((size_t)1, watchdog.m_threadList.size()) << L"m_threadList.size() tickCountCheckin2";

		operatingSystem.m_tickCount32 = tickCountMaxAllowed2;
		ASSERT_FALSE(watchdog.CheckForTimeout()) << L"CheckForTimeout tickCountMaxAllowed2";
		ASSERT_EQ((size_t)1, watchdog.m_threadList.size()) << L"m_threadList.size() tickCountMaxAllowed2";

		operatingSystem.m_tickCount32 = tickCountTimedOut2;
		ASSERT_EQ(true, watchdog.CheckForTimeout()) << L"CheckForTimeout tickCountTimedOut2";
		ASSERT_EQ((size_t)0, watchdog.m_threadList.size()) << L"m_threadList.size() tickCountTimedOut2";
	}
}

/// <summary>
/// Code coverage for:
///     CWatchDog::Checkin()
///     CWatchDog::CheckForTimeout()
/// Check for simultaneous timeout when 2 different threads do checkin.
/// </summary>
TEST_F(WatchDogUnitTests, CheckForTimeout_Checkin12Test)
{
	// Data.
	const LPCTSTR name1(_T("threadNameOne"));
	const thread::id threadID1(0x12345);
	const int timeOutSeconds1(150);
	const DWORD tickCountSetup = 10000;
	const DWORD tickCountRegister = 12000;
	const DWORD tickCountCheckin1 = tickCountRegister + (DWORD)timeOutSeconds1 * MILLISECONDS_PER_SECOND / 2;
	const DWORD tickCountMaxAllowed1 = tickCountCheckin1 + (DWORD)timeOutSeconds1 * MILLISECONDS_PER_SECOND - 1;
	const DWORD tickCountTimedOut1 = tickCountMaxAllowed1 + 1;

	const LPCTSTR name2(_T("threadNameTwoIsLonger"));
	const thread::id threadID2(0x23456);
	const int timeOutSeconds2(200);
	const DWORD tickCountCheckin2a = tickCountRegister + (DWORD)timeOutSeconds2 * MILLISECONDS_PER_SECOND / 2; // tickCountCheckin1 <= Should be <= tickCountMaxAllowed1.
	const DWORD tickCountCheckin2b = tickCountTimedOut1 + (DWORD)timeOutSeconds2 * MILLISECONDS_PER_SECOND / 2;
	const DWORD tickCountMaxAllowed2 = tickCountCheckin2b + (DWORD)timeOutSeconds2 * MILLISECONDS_PER_SECOND - 1;
	const DWORD tickCountTimedOut2 = tickCountMaxAllowed2 + 1;

	// Setup.
	COperatingSystemTest operatingSystem; // So we can control the tick count.
	operatingSystem.m_tickCount32 = tickCountSetup;
	CWatchDog watchdog(operatingSystem);

	operatingSystem.m_tickCount32 = tickCountRegister;
	watchdog.RegisterThread(name1, threadID1, timeOutSeconds1, 0);
	watchdog.RegisterThread(name2, threadID2, timeOutSeconds2, 0);
	AssertThreadCheckInTime(watchdog, threadID1, tickCountRegister, L"tickCountRegister 1");
	AssertThreadCheckInTime(watchdog, threadID1, tickCountRegister, L"tickCountRegister 2");
	ASSERT_FALSE(watchdog.CheckForTimeout()) << L"CheckForTimeout tickCountRegister";
	ASSERT_EQ((size_t)2, watchdog.m_threadList.size()) << L"m_threadList.size() tickCountRegister";

	// Perform operation(s) and test(s).
	operatingSystem.m_tickCount32 = tickCountCheckin1;
	watchdog.Checkin(threadID1);
	AssertThreadCheckInTime(watchdog, threadID1, tickCountCheckin1, L"tickCountCheckin1");
	ASSERT_FALSE(watchdog.CheckForTimeout()) << L"CheckForTimeout tickCountCheckin1";
	ASSERT_EQ((size_t)2, watchdog.m_threadList.size()) << L"m_threadList.size() tickCountCheckin1";

	operatingSystem.m_tickCount32 = tickCountCheckin2a;
	watchdog.Checkin(threadID2);
	AssertThreadCheckInTime(watchdog, threadID2, tickCountCheckin2a, L"tickCountCheckin2a");
	ASSERT_FALSE(watchdog.CheckForTimeout()) << L"CheckForTimeout tickCountCheckin2a";
	ASSERT_EQ((size_t)2, watchdog.m_threadList.size()) << L"m_threadList.size() tickCountCheckin2a";

	operatingSystem.m_tickCount32 = tickCountMaxAllowed1;
	ASSERT_FALSE(watchdog.CheckForTimeout()) << L"CheckForTimeout tickCountMaxAllowed1";
	ASSERT_EQ((size_t)2, watchdog.m_threadList.size()) << L"m_threadList.size() tickCountMaxAllowed1";

	// First thread should timeout.
	operatingSystem.m_tickCount32 = tickCountTimedOut1;
	ASSERT_EQ(true, watchdog.CheckForTimeout()) << L"CheckForTimeout tickCountTimedOut1";
	ASSERT_EQ((size_t)1, watchdog.m_threadList.size()) << L"m_threadList.size() tickCountTimedOut1";

	// When one thread times out, all other threads have checkin performed automatically.
	{
		AssertThreadCheckInTime(watchdog, threadID2, tickCountTimedOut1, L"2");

		operatingSystem.m_tickCount32 = tickCountCheckin2b;
		watchdog.Checkin(threadID2);
		AssertThreadCheckInTime(watchdog, threadID2, tickCountCheckin2b, L"tickCountCheckin2b");
		ASSERT_FALSE(watchdog.CheckForTimeout()) << L"CheckForTimeout tickCountCheckin2b";
		ASSERT_EQ((size_t)1, watchdog.m_threadList.size()) << L"m_threadList.size() tickCountCheckin2b";

		operatingSystem.m_tickCount32 = tickCountMaxAllowed2;
		ASSERT_FALSE(watchdog.CheckForTimeout()) << L"CheckForTimeout tickCountMaxAllowed2";
		ASSERT_EQ((size_t)1, watchdog.m_threadList.size()) << L"m_threadList.size() tickCountMaxAllowed2";

		operatingSystem.m_tickCount32 = tickCountTimedOut2;
		ASSERT_EQ(true, watchdog.CheckForTimeout()) << L"CheckForTimeout tickCountTimedOut2";
		ASSERT_EQ((size_t)0, watchdog.m_threadList.size()) << L"m_threadList.size() tickCountTimedOut2";
	}
}
// #pragma endregion CheckForTimeout

TEST_F(WatchDogUnitTests, WatchDogReportThreadCallstacksTest)
{
	string targetDir = CDirectoryManager::CallstackFilePath();
	CUtilities::DeleteFile(targetDir.c_str(), nullptr, false);

	CWatchDog watchDog;
	const int timeOutSeconds(150);

	const thread::id threadID(0x12345);
	const LPCTSTR threadName(_T("first"));
	watchDog.RegisterThread(threadName, threadID, timeOutSeconds, 0);
	watchDog.ReportThreadCallstackToDisk(threadName, true);

	watchDog.DeregisterThread(threadID);

	ASSERT_TRUE(CUtilities::FileExists(targetDir));
    std::fstream file;
    file.open(targetDir, std::fstream::in);
            
    ASSERT_NE(0, CUtilities::GetFileSize(file));
    file.close();
}

TEST_F(WatchDogUnitTests, WatchDogSafeSleepTest)
{
	CWatchDog watchDog;
	watchDog.RegisterThread("test", thread::id(100), 3, 0);

	int start = CUtilities::GetTickCount();
	watchDog.SafeSleep(thread::id(100), 10*1000);
	int elapse = CUtilities::GetTickCount() - start;

	string msg;
	msg = FormatString("Safe sleep did not last long enough, elaspe=%d", elapse);

	ASSERT_TRUE(9*1000 <= elapse) << msg;
}

TEST_F(WatchDogUnitTests, SetWatchdogTimeoutTests)
{
	CWatchDog watchDog;
	watchDog.SetWatchdogTimeout(WATCHDOG_HARDWARE_TIMEOUT);
	ASSERT_TRUE(watchDog.m_timeout == WATCHDOG_HARDWARE_TIMEOUT);

	watchDog.SetWatchdogTimeout(WATCHDOG_HARDWARE_TIMEOUT_WHILE_RESTARTING);
	ASSERT_TRUE(watchDog.m_timeout == WATCHDOG_HARDWARE_TIMEOUT_WHILE_RESTARTING);
}