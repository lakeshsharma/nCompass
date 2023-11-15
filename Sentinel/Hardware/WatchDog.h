#pragma once
#include "ProcessDump/OperatingSystem.h"
#include "WatchdogThreadInfo.h"

const UINT WATCHDOG_HARDWARE_TIMEOUT = 5; 	// 5 second timeout for the hardware watchdog.
const UINT WATCHDOG_HARDWARE_TIMEOUT_WHILE_RESTARTING = 122; //122 second timeout for the hardware watchdog while restarting;

class CWatchDog
{
public:
	///	<summary>
	/// Gets a reference to the global instance of CWatchDog.
	///	</summary>
	///	<returns>A reference to the global CWatchDog instance.</returns>	
	/// <remarks>
	/// The (first) caller is responsible for successfully calling S5_DLL_Init() and initializing the logger.
	/// </remarks>
	static CWatchDog &Instance();

	static void CleanupInstance();

	/// <summary>
	/// Deconstructor
	/// </summary>
	virtual ~CWatchDog(void);

	/// <summary>
	/// Register an existing thread for purposes of monitoring.
	/// </summary>
	/// <param name="name">Name of the thread.</param>
	/// <param name="threadId">Id of the thread.</param>
	/// <param name="timeOutSeconds">How long to wait until the thread is considered 'frozen'.</param>
	/// <remarks>Sets the checkin time to the current tick count.</remarks>
	void RegisterThread(const std::string &name, std::thread::id threadID, int timeOutSeconds, pthread_t nativeHandle);

	/// <summary>
	/// Deregister the current thread.
	/// </summary>
	/// <remarks>Has no effect if the thread was not previously registered.</remarks>
	void DeregisterThread();

	/// <summary>
	/// Deregister a register thread.
	/// </summary>
	/// <param name="threadId">Id of the thread.</param>
	/// <remarks>Effectively stops monitoring.</remarks>
	/// <remarks>Has no effect if the thread was not previously registered.</remarks>
	void DeregisterThread(std::thread::id threadID);

	/// <summary>
	/// Begin the shutdown process.
	/// </summary>
	/// <remarks>If DEBUG or _UNIT_TEST then no call is made to reboot.</remarks>
	/// <remarks>Has no effect if a shutdown has already begun.</remarks>
	void BeginShutdown(const std::string& description);

	/// <summary>
	/// Has the shutdown process begun.
	/// </summary>
	/// <returns>'true' if shutdown has begun.</returns>
	bool IsTimeToShutdown(void);

	/// <summary>
	/// Resets the checkin time to the current tick count for the current thread.
	/// </summary>
	/// <remarks>This function has no effect if the thread is not registered.</remarks>
	/// <remarks>A long-running function may checkin without knowing whether it was called by a registered thread.</remarks>
	void Checkin();

	/// <summary>
	/// Resets the checkin time to the current tick count for the given thread.
	/// </summary>
	/// <param name="threadId">Id of thread.</param>
	/// <remarks>This function has no effect if the thread is not registered.</remarks>
	/// <remarks>A long-running function may checkin without knowing whether it was called by a registered thread.</remarks>
	void Checkin(std::thread::id threadId);


	/// <summary>
	/// Logs the registered threads.
	/// </summary>
	/// <param name="loggingid">IN - The logging identifier.</param>
	void LogRegisteredThreads(const std::string &loggingid);

	/// <summary>
	/// Gets the callstack of all registerd threads and writes to string buffer passed.
	/// </summary>
	/// <param name="threadId">Id of thread to be singled out.</param>
	/// <param name="name">Name of the thread to be singled out.</param>
	/// <param name="contextRecord">Context record of the primary thread to be singled out. Can be NULL.</param>
	/// <param name="buffer">OUT - buffer to receive callstack.</param>
	/// <param name="bufferSize">Size of buffer passed.</param>
	/// <param name="logErrors">If 'true', the caller sees the circumstances safe to log error and the method will do so.</param>
	/// <remarks>
	/// If 'contextRecord' is NULL, calls ::GetThreadContext using 'thread' handle to get the context record.
	/// </remarks>
	void ReportThreadCallstacks(std::thread::id threadId, const std::string &name, char* buffer, size_t bufferSize, bool logErrors);

    /// <summary>
    /// Write a callstack buffer of the current exception to a file 
    /// in as safe a way as possilbe. 
    /// </summary>
    /// <param name="name">Name of the thread.</param>
    /// <returns>Number of bytes written to file.</returns>
    void ReportThreadCallstackToDisk(const std::string &name, bool isThreadTimeout);

    /// <summary>
	/// Provide a safe 'sleep' time where checkins/pings are done to keep the watchdog from kicking.
	/// </summary>
	/// <param name="threadId">Id of thread to be singled out.</param>
	/// <param name="wait">Hold to wait or 'sleep'.</param>
	void SafeSleep(std::thread::id threadId, DWORD wait);

    void SetExceptionInformation(std::thread::id ThreadID, std::string exceptionType, std::string exceptionStackTrace);
    std::string GetExceptionType(std::thread::id ThreadID);
    std::string GetExceptionStackTrace(std::thread::id ThreadID);
    std::string GetThreadName(std::thread::id ThreadID);

	/// <summary>
	/// Append all the threads' call stack to the string exceptionString.
	/// </summary>
	/// <param name="exceptionString">OUT - string that holds all the exception information</param>
	/// <param name="otherThan">IN - thread not to get callstack info on </param>
	/// <param name="writeHeaderFooter">IN - flag that indicates to write header and footer</param>
	void AppendAllThreadCallStacks(std::string &exceptionString, std::thread::id otherThan, bool writeHeaderFooter);

	/// <summary>
	/// Determines whether watchdog is enabled.
	/// </summary>
	/// <returns>true if enabled, false otherwise.</returns>
	bool IsWatchdogEnabled() const;

	void SetThreadClosed();

	/// <summary>
	/// Determines whether a thread timed out.
	/// </summary>
	/// <returns>true if a thread timed out, false otherwise.</returns>
	bool IsThreadTimedOut() const;

	/// <summary>
	/// Determines whether a signal is triggered.
	/// </summary>
	/// <returns>true if a signal is triggered, false otherwise.</returns>
	bool IsSignalTriggered() const;

	/// <summary>
	/// Sets the signal triggered flag.
	/// </summary>
	void SignalTriggered();

	/// <summary>
	/// Determines whether a shutdown is in progress.
	/// </summary>
	/// <returns>true if a shutdown is in progress, false otherwise.</returns>
	bool IsShutdownInProcess() const;

	/// <summary>
	/// Sets the shutdown is in progress flag.
	/// </summary>
	void SetShutdownInProcess();

	/// <summary>
	/// To suspend check-in requirement for thread. Required for blocking lib calls like ActiveMQ.
	/// </summary>
	void SuspendCheckin();

	/// <summary>
	///  To suspend check-in requirement for thread. Required for blocking lib calls like ActiveMQ.
	/// </summary>
	/// <param name="threadId">Id of thread.</param>
	void SuspendCheckin(std::thread::id ThreadID);

	/// <summary>
	///  To resume check-in requirement for thread. Required for blocking lib calls like ActiveMQ.
	/// </summary>
	/// <param name="threadId">Id of thread.</param>
	void ResumeCheckin();

	/// <summary>
	///  To resume check-in requirement for thread. Required for blocking lib calls like ActiveMQ.
	/// </summary>
	/// <param name="threadId">Id of thread.</param>
	void ResumeCheckin(std::thread::id ThreadID);

	/// <summary>
	///  Sets the watchdog timeout value.
	/// </summary>
	/// <param name="timeout">timeout value of the watchdog.</param>
	void SetWatchdogTimeout(UINT timeout);

private:
	// The singleton instance.
	static CWatchDog *m_instance;
	static std::recursive_mutex m_instanceCS;

	// Allowed shutdown time for threads to stop running.
	// Deleting resources while a thread is still using them may cause the thread to crash.
	static const DWORD ALLOWED_SHUTDOWN_MILLISECONDS;

	static const DWORD PING_WATCHDOG_MILLISECONDS;

	/// <summary>
	/// Standard/Default constructor. Initializes a new instance of the <see cref="CWatchDog"/> class.
	/// </summary>
	/// <param name="operatingSystem">IN/OUT - An interface to operating system functionality.</param>
	CWatchDog(IOperatingSystem &operatingSystem = COperatingSystem::m_operatingSystem);

	/// <summary>
	/// Starts the thread for this instance.
	/// </summary>
	/// <remarks>
	/// Wait until derived constructors and initializers complete before starting this instance.
	/// The instance is only intended to be started once.
	/// </remarks>
	virtual void Start();

	/// <summary>
	/// Performs initialization for the run-time thread.
	/// </summary>
	/// <remarks>If DEBUG or _UNIT_TEST then the hardware watchdog is not activated.</remarks>
	void RunInit();

	/// <summary>
	/// Performs a loop checking for 'frozen' registered threads, until told to shutdown and reboot the nCompass unit.
	/// </summary>
	/// <remarks>If DEBUG or _UNIT_TEST then no call is made to reboot.</remarks>
	void Run(void);

	/// <summary>
	/// Stops the thread if the thread is running and returns when the thread is not running.
	/// </summary>
	void Stop();

	static UINT ThreadFunction(LPVOID threadParam);

	typedef std::map<std::thread::id, CWatchdogThreadInfo> WatchdogThreadMap;
	WatchdogThreadMap m_threadList;

	bool m_watchdogEnabled;
	bool m_timeToClose;
	bool m_shuttingDown;
	bool m_threadTimedOut;
	bool m_signalTriggered;
	bool m_shutdownInProcess;
	DWORD m_beginShutdownTickCount;
	DWORD m_timeout;
	IOperatingSystem &m_operatingSystem;
	std::unique_ptr<std::thread> m_thread;

	std::recursive_mutex m_criticalSection;

	bool CheckForTimeout(void);
	void PingWatchdog(void);
	bool ShutdownTimedOut();
	
	// Grant "friend" access for testing.
	// Most of the WatchDogUnitTests use the private constructor.
	friend class WatchDogUnitTests;
	FRIEND_TEST(WatchDogUnitTests, CWatchDog_Constants);
	FRIEND_TEST(WatchDogUnitTests, WatchDogDefaultConstructorTest);
	FRIEND_TEST(WatchDogUnitTests, WatchDogNonDefaultConstructorTest);
	FRIEND_TEST(WatchDogUnitTests, StartStopTest);
	FRIEND_TEST(WatchDogUnitTests, BeginShutdown_DefaultTest);
	FRIEND_TEST(WatchDogUnitTests, BeginShutdown_FalseTest);
	FRIEND_TEST(WatchDogUnitTests, BeginShutdown_TrueTest);
	FRIEND_TEST(WatchDogUnitTests, BeginShutdown_FalseTrueTest);
	FRIEND_TEST(WatchDogUnitTests, BeginShutdown_TrueFalseTest);
	FRIEND_TEST(WatchDogUnitTests, ShutdownNowTest);
	FRIEND_TEST(WatchDogUnitTests, IsTimeToShutdown_ShutdownNowTest);
	FRIEND_TEST(WatchDogUnitTests, IsTimeToShutdown_BeginShutdownDefaultTest);
	FRIEND_TEST(WatchDogUnitTests, IsTimeToShutdown_BeginShutdownFalseTest);
	FRIEND_TEST(WatchDogUnitTests, IsTimeToShutdown_BeginShutdownTrueTest);
	FRIEND_TEST(WatchDogUnitTests, ShutdownTimedOut_DefaultOSTest);
	FRIEND_TEST(WatchDogUnitTests, ShutdownTimedOut_BeginShutdownFalse_DefaultOSTest);
	FRIEND_TEST(WatchDogUnitTests, ShutdownTimedOut_BeginShutdownTrue_DefaultOSTest);
	FRIEND_TEST(WatchDogUnitTests, ShutdownTimedOut_True_DefaultOSTest);
	FRIEND_TEST(WatchDogUnitTests, ShutdownTimedOut_ShutdownNow_DefaultOSTest);
	FRIEND_TEST(WatchDogUnitTests, ShutdownTimedOut_NonDefaultTest);
	FRIEND_TEST(WatchDogUnitTests, ShutdownTimedOut_BeginShutdownFalseTest);
	FRIEND_TEST(WatchDogUnitTests, ShutdownTimedOut_BeginShutdownTrueTest);
	FRIEND_TEST(WatchDogUnitTests, ShutdownTimedOut_ShutdownNowTest);
	FRIEND_TEST(WatchDogUnitTests, RegisterThread_CurrentIdTest);
	FRIEND_TEST(WatchDogUnitTests, RegisterThread_ThreadIdTest);
	FRIEND_TEST(WatchDogUnitTests, RegisterThread_ThreadIdDupTest);
	FRIEND_TEST(WatchDogUnitTests, RegisterThread_ThreadId12Test);
	FRIEND_TEST(WatchDogUnitTests, DeregisterThread_CurrentIdNoneTest);
	FRIEND_TEST(WatchDogUnitTests, DeregisterThread_CurrentId0Test);
	FRIEND_TEST(WatchDogUnitTests, DeregisterThread_CurrentId1Test);
	FRIEND_TEST(WatchDogUnitTests, DeregisterThread_CurrentId12Test);
	FRIEND_TEST(WatchDogUnitTests, DeregisterThread_CurrentId21Test);
	FRIEND_TEST(WatchDogUnitTests, DeregisterThread_ThreadIdNoneTest);
	FRIEND_TEST(WatchDogUnitTests, DeregisterThread_ThreadId0Test);
	FRIEND_TEST(WatchDogUnitTests, DeregisterThread_ThreadId1Test);
	FRIEND_TEST(WatchDogUnitTests, DeregisterThread_ThreadId12Test);
	FRIEND_TEST(WatchDogUnitTests, DeregisterThread_ThreadId21Test);
	FRIEND_TEST(WatchDogUnitTests, DeregisterThread_ThreadId22Test);
	FRIEND_TEST(WatchDogUnitTests, DeregisterThread_ThreadId22ReverseTest);
	FRIEND_TEST(WatchDogUnitTests, CheckIn_CurrentIdNoneTest);
	FRIEND_TEST(WatchDogUnitTests, CheckIn_CurrentId0Test);
	FRIEND_TEST(WatchDogUnitTests, CheckIn_CurrentId1Test);
	FRIEND_TEST(WatchDogUnitTests, CheckIn_ThreadIdNoneTest);
	FRIEND_TEST(WatchDogUnitTests, CheckIn_ThreadId0Test);
	FRIEND_TEST(WatchDogUnitTests, CheckIn_ThreadId1Test);
	FRIEND_TEST(WatchDogUnitTests, CheckForTimeout_0Test);
	FRIEND_TEST(WatchDogUnitTests, CheckForTimeout_1Test);
	FRIEND_TEST(WatchDogUnitTests, CheckForTimeout_2Test);
	FRIEND_TEST(WatchDogUnitTests, CheckForTimeout_12Test);
	FRIEND_TEST(WatchDogUnitTests, CheckForTimeout_Checkin1Test);
	FRIEND_TEST(WatchDogUnitTests, CheckForTimeout_Checkin2Test);
	FRIEND_TEST(WatchDogUnitTests, CheckForTimeout_Checkin12Test);
	FRIEND_TEST(WatchDogUnitTests, SetWatchdogTimeoutTests);	
	FRIEND_TEST(WatchDogUnitTests, WatchDogSafeSleepTest);
	FRIEND_TEST(WatchDogUnitTests, WatchDogReportThreadCallstacksTest);
};

void GlobalSignalHandler(int sig);


void ReportSingleCallstack(std::string callStackReason);

int backtrace2(void** trace, int size, int skip);
