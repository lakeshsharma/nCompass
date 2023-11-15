#include "stdafx.h"

#include <iostream>
#include <dlfcn.h>
#include <execinfo.h>
#include <typeinfo>
#include <string>
#include <memory>
#include <cxxabi.h>
#include <cstdlib>
#if !defined(_UNIT_TEST)
#include <libunwind.h>
#include <unwind.h>
#endif
#include "WatchDog.h"
#include "Platform.h"

// Sentinel includes.
#include "../Diagnostics.h"
#include "DirectoryManager.h"
#include "DisplayManager.h"
#include "Logging/LogString.h"
#include "Logging/Logger.h"
#include "Utilities.h"
#include "GlobalDefs.h"
#include "RestartManager.h"
#include "ProcessDump/WebDiagnosticsStatusInfo.h"

using namespace std;

const UINT CALLSTACK_GATHERING_TIMEOUT_SECS = 30;    // 30 secs timeout for gathering callstack of a single thread.

CWatchDog *CWatchDog::m_instance(NULL);
std::recursive_mutex CWatchDog::m_instanceCS;
const DWORD CWatchDog::ALLOWED_SHUTDOWN_MILLISECONDS = 30000; // 30000 milliseconds == 30 seconds.
const DWORD CWatchDog::PING_WATCHDOG_MILLISECONDS = 300; // 300 milliseconds == 0.3 seconds.

CWatchDog &CWatchDog::Instance()
{
	lock_guard<recursive_mutex> lock(m_instanceCS);
	if (m_instance == NULL)
	{
		m_instance = new CWatchDog();
		m_instance->Start();
	}

	return *m_instance;
}

void CWatchDog::CleanupInstance()
{
	lock_guard<recursive_mutex> lock(m_instanceCS);
	if (NULL != m_instance)
	{
		delete m_instance;
		m_instance = NULL;
	}
}

CWatchDog::CWatchDog(IOperatingSystem &operatingSystem)
	:
	m_watchdogEnabled(true),
	m_timeToClose(false),
	m_shuttingDown(false),
	m_threadTimedOut(false),
	m_signalTriggered(false),
	m_shutdownInProcess(false),
	m_beginShutdownTickCount(0),
	m_timeout(0),
	m_operatingSystem(operatingSystem)
{
}

void CWatchDog::RunInit()
{
#if !defined(_UNIT_TEST)
	if (IsWatchdogEnabled())
	{
		cout << "WatchDog is enabled.\n";
		if (S7LITE_Watchdog_Enable() != S7DLL_STATUS_OK)
		{
			LogString(ERROR_LOG, _T("Failed to enable the Watchdog"));
		}

		SetWatchdogTimeout(WATCHDOG_HARDWARE_TIMEOUT);

		if (S7LITE_Watchdog_Kick() != S7DLL_STATUS_OK)
		{
			LogString(ERROR_LOG, _T("Failed initial kick of the Watchdog"));
		}
	}
#endif
}

CWatchDog::~CWatchDog(void)
{
	Stop();
}

void CWatchDog::Start()
{
	struct stat status;
	if (stat(DISABLE_WATCHDOG.c_str(), &status) == 0)
	{
		cout << "********************DISABLE_WATCHDOG exists********************\n";
		m_watchdogEnabled = false;
	}

	// Start the incoming queue thread.
	m_thread.reset(new thread(ThreadFunction, this));
	m_thread->detach();
}

void CWatchDog::Stop()
{
	lock_guard<recursive_mutex> lock(m_criticalSection);
	m_timeToClose = true;
	if (m_thread.get() != nullptr)
	{
		while (m_thread != NULL)
		{
			CUtilities::Sleep(SHUTDOWN_LOOP_SLEEP_PERIOD);
		}
	}
}

void CWatchDog::SetThreadClosed()
{
	m_thread = NULL;
}

bool CWatchDog::IsThreadTimedOut() const
{
	return m_threadTimedOut;
}

bool CWatchDog::IsSignalTriggered() const
{
	return m_signalTriggered;
}

void CWatchDog::SignalTriggered()
{
	m_signalTriggered = true;
}

bool CWatchDog::IsShutdownInProcess() const
{
	return m_shutdownInProcess;
}

void CWatchDog::SetShutdownInProcess()
{
	m_shutdownInProcess = true;
}

UINT AFX_CDECL CWatchDog::ThreadFunction(LPVOID threadParam)
{
	CWatchDog *watchdog((CWatchDog *)threadParam);

	CUtilities::SetThreadName(CUtilities::GetCurrentThreadId(), THREAD_WATCHDOG);
	CUtilities::ShowThreadId(_T(THREAD_WATCHDOG));

    try
	{
		watchdog->RunInit();
		watchdog->Run();
	}
    catch (...) 
    {
		CRestartManager::ReportThreadExceptionAndRequestAReboot(THREAD_WATCHDOG);
    }

	watchdog->SetThreadClosed();
	
	return 0;
}

void CWatchDog::Run(void)
{
#ifndef _UNIT_TEST
	while (!m_timeToClose && !ShutdownTimedOut())
	{
		PingWatchdog();
		
		CheckForTimeout();

		CUtilities::Sleep(PING_WATCHDOG_MILLISECONDS);
	}

	if (IsWatchdogEnabled())
	{
		LogString(GENERAL, _T("Resetting the the watchdog timeout to %d sec before reboot system command is called."), WATCHDOG_HARDWARE_TIMEOUT_WHILE_RESTARTING);

		// Try to give us enough time to restart the Sentinel without a hardware watchdog reset.
		SetWatchdogTimeout(WATCHDOG_HARDWARE_TIMEOUT_WHILE_RESTARTING);

		LogString(GENERAL, _T("Calling the reboot system command."));
		const string RebootSystemString = CPlatform::Instance().IsLiteDevice() ? "reboot" : "/sbin/reboot";
		string rebootResultString = CUtilities::GetSystemResultString(RebootSystemString);
		LogString(GENERAL, _T("reboot command result string: %s"), rebootResultString.c_str());

		// Once the reboot command is called we can go back to the default watchdog timeout
		SetWatchdogTimeout(WATCHDOG_HARDWARE_TIMEOUT);		
	}
	else
	{
		LogString(GENERAL, _T("Exiting application instead of rebooting because watchdog is not enabled"));

		if (IsSignalTriggered())
		{
			cout << "**************************************************************************\n";
			cout << "************ Exception occured: exiting application instead of rebooting because watchdog is not enabled\n";
			cout << "**************************************************************************\n";
			exit(1);
		}
	}

#endif // _UNIT_TEST

	// Otherwise, let the hardware watchdog timeout (after about an additional WATCHDOG_HARDWARE_TIMEOUT) and do a full hardware reset.
}

void CWatchDog::BeginShutdown(const string& description)
{
	if (!m_shuttingDown)
	{
		LogString(GENERAL, "Reboot localtime=%s", CUtilities::FormatDateTimeString(CUtilities::GetCurrentTime()).c_str());
		cout << "BeginShutdown before WriteLogsToMediaDisk." << endl;
		// Try to give us enough time to write the logs without a hardware watchdog reset.
		SetWatchdogTimeout(WATCHDOG_HARDWARE_TIMEOUT_WHILE_RESTARTING);
		CWatchDog::Instance().Checkin();
#ifndef _UNIT_TEST
		CSmartDataLogger::Instance().WriteLogsToMediaDisk(ZipLogActionType::Shutdown, "", description);
#endif
		cout << "BeginShutdown after WriteLogsToMediaDisk." << endl;
		CWatchDog::Instance().Checkin();
		SYSTEMTIME utcSystemTime(CUtilities::GetUTCSystemTime(CUtilities::GetCurrentTime()));
		CNVRAMConfig nvramConfig;
		nvramConfig.Set(SYSTEM_TIME_OFFSET, (BYTE*)&utcSystemTime, sizeof(utcSystemTime));
		SetWatchdogTimeout(WATCHDOG_HARDWARE_TIMEOUT);
		cout << "BeginShutdown after SetWatchdogTimeout(WATCHDOG_HARDWARE_TIMEOUT)" << endl;
	}

	lock_guard<recursive_mutex> lock(m_criticalSection);

	// Only begin shutdown once.
	if (!m_shuttingDown)
	{
		cout << "BeginShutdown.\n";
		m_shuttingDown = true;
		m_beginShutdownTickCount = m_operatingSystem.GetTickCount32();
	}
	m_timeToClose = true;
}

void CWatchDog::Checkin()
{
	Checkin(CUtilities::GetCurrentThreadId());
}

void CWatchDog::Checkin(std::thread::id ThreadID)
{
	lock_guard<recursive_mutex> lock(m_criticalSection);
	{
		WatchdogThreadMap::iterator pair(m_threadList.find(ThreadID));

		if (pair != m_threadList.cend())
		{
			pair->second.CheckIn(m_operatingSystem.GetTickCount32());
		}
	}
}

void CWatchDog::SuspendCheckin()
{
	SuspendCheckin(CUtilities::GetCurrentThreadId());
}

void CWatchDog::SuspendCheckin(std::thread::id ThreadID)
{
	lock_guard<recursive_mutex> lock(m_criticalSection);
	{
		WatchdogThreadMap::iterator pair(m_threadList.find(ThreadID));

		if (pair != m_threadList.cend())
		{
			pair->second.Suspend();
		}
	}
}

void CWatchDog::ResumeCheckin()
{
	ResumeCheckin(CUtilities::GetCurrentThreadId());
}

void CWatchDog::ResumeCheckin(std::thread::id ThreadID)
{
	lock_guard<recursive_mutex> lock(m_criticalSection);
	{
		WatchdogThreadMap::iterator pair(m_threadList.find(ThreadID));

		if (pair != m_threadList.cend())
		{
			pair->second.Resume(m_operatingSystem.GetTickCount32());
		}
	}
}

bool CWatchDog::IsWatchdogEnabled() const
{
	return m_watchdogEnabled;
}

void CWatchDog::SetExceptionInformation(std::thread::id ThreadID, std::string exceptionType, std::string exceptionStackTrace)
{
    lock_guard<recursive_mutex> lock(m_criticalSection);
    {
        WatchdogThreadMap::iterator pair(m_threadList.find(ThreadID));

        if (pair != m_threadList.cend())
        {
            pair->second.SetExceptionInformation(exceptionType, exceptionStackTrace);
        }
    }
}

std::string CWatchDog::GetThreadName(std::thread::id ThreadID)
{
    lock_guard<recursive_mutex> lock(m_criticalSection);
    {
        WatchdogThreadMap::iterator pair(m_threadList.find(ThreadID));

        if (pair != m_threadList.cend())
        {
            return pair->second.GetThreadName();
        }
    }
    return "<unregistered thread>";
}

std::string CWatchDog::GetExceptionType(std::thread::id ThreadID)
{
    lock_guard<recursive_mutex> lock(m_criticalSection);
    {
        WatchdogThreadMap::iterator pair(m_threadList.find(ThreadID));

        if (pair != m_threadList.cend())
        {
            return pair->second.GetExceptionType();
        }
    }
    return "";
}

std::string CWatchDog::GetExceptionStackTrace(std::thread::id ThreadID)
{
    string threadName;
    lock_guard<recursive_mutex> lock(m_criticalSection);
    {
        WatchdogThreadMap::iterator pair(m_threadList.find(ThreadID));

        if (pair != m_threadList.cend())
        {
            threadName = pair->second.GetExceptionStackTrace();
        }
    }
    return threadName;;
}

bool CWatchDog::CheckForTimeout(void)
{
	string threadName, threadId;
	{
		lock_guard<recursive_mutex> lock(m_criticalSection);
		// Iterate through list.
		WatchdogThreadMap::iterator pThreadItem;
		pThreadItem = m_threadList.begin();

		while ((m_threadList.end() != pThreadItem) && !m_threadTimedOut)
		{
			if (pThreadItem->second.HasTimedOut(m_operatingSystem.GetTickCount32()))
			{
				m_threadTimedOut = true;
				ostringstream threadIdStream;
				threadIdStream << pThreadItem->first;
				threadId = threadIdStream.str();
				threadName = pThreadItem->second.GetName();
				LogString(ERROR_LOG, _T("%s threadID=%s timed out."), threadName.c_str(), threadId.c_str());
			}
			else
			{
				++pThreadItem;
			}
		}
	}

	//We don't control reboot based on other threads not checking in for more than 2 minutes
	//when m_timeout is set to other value besides WATCHDOG_HARDWARE_TIMEOUT because
	//we are only changing m_timeout to other value knowing that we have to a let an event or function to finish and not care about other threads, search 
	//WATCHDOG_HARDWARE_TIMEOUT_WHILE_RESTARTING for example.
	//If you need to change this value other than WATCHDOG_HARDWARE_TIMEOUT_WHILE_RESTARTING and still require the regular thread timeout functionality in placed,
	//you need to edit the next line below to do so.
	m_threadTimedOut = m_threadTimedOut && WATCHDOG_HARDWARE_TIMEOUT == m_timeout;

	if (m_threadTimedOut)
	{
		CDiagnostics::Instance().PersistReboot(tltRebootThreadTimeout, ERROR_LOG);
		LogString(ERROR_LOG, _T("%s threadID=%s Has Killed The Watchdog."), threadName.c_str(), threadId.c_str());
		cout << "- timeout occurred on thread " << threadName << endl;		
		ReportThreadCallstackToDisk(threadName, true);
		CUtilities::FlushVolume();
		BeginShutdown(CWebDiagnosticsStatusInfo::TiltCodeToString(tltRebootThreadTimeout));
	}

	return m_threadTimedOut;
}

void CWatchDog::PingWatchdog(void)
{
#if !defined(_UNIT_TEST)
	if (IsWatchdogEnabled())
	{
		S7LITE_Watchdog_Kick();
	}
#endif
}

void CWatchDog::RegisterThread(const std::string &name, std::thread::id threadID, int timeOutSeconds,pthread_t nativeHandle)
{
	ostringstream threadIdString;
	threadIdString << threadID;
	LogString(GENERAL, "REGISTERING %s - threadID %s timeOutSeconds=%d",
		name.c_str(),
        threadIdString.str().c_str(),
		(int)timeOutSeconds);

	CWatchdogThreadInfo wti(name, timeOutSeconds * MILLISECONDS_PER_SECOND, m_operatingSystem.GetTickCount32(), nativeHandle);

	lock_guard<recursive_mutex> lock(m_criticalSection);
	{
		m_threadList[threadID] = wti;
	}
}

void CWatchDog::DeregisterThread()
{
	DeregisterThread(CUtilities::GetCurrentThreadId());
};

void CWatchDog::DeregisterThread(std::thread::id threadID)
{
	CWatchdogThreadInfo wti;
	BOOL found;

	{
		lock_guard<recursive_mutex> lock(m_criticalSection);
		WatchdogThreadMap::const_iterator iter(m_threadList.find(threadID));
		found = m_threadList.cend() != iter;
		if (found)
		{
			wti = iter->second;
		}
		m_threadList.erase(threadID);
	}

	ostringstream threadIdString;
	threadIdString << threadID;
	if (found)
	{
		LogString(GENERAL, _T("DEREGISTERING %s, Thread ID = %s"), static_cast<LPCTSTR>(wti.GetName().c_str()), static_cast<LPCTSTR>(threadIdString.str().c_str()));
	}
	else
	{
		LogString(ERROR_LOG, _T("threadID=%s NOT REGISTERED with the watchdog"), static_cast<LPCTSTR>(threadIdString.str().c_str()));
	}
}

bool CWatchDog::IsTimeToShutdown(void)
{
	return m_shuttingDown;
}

bool CWatchDog::ShutdownTimedOut()
{
	bool retVal = false;


	if (m_shuttingDown)
	{
		DWORD currentTick(m_operatingSystem.GetTickCount32());
		//check if we've expired
		if (currentTick - m_beginShutdownTickCount > ALLOWED_SHUTDOWN_MILLISECONDS)
		{
			retVal = true;
		}
	}

	return retVal;
}

void CWatchDog::SetWatchdogTimeout(UINT timeout)
{
	m_timeout = timeout;
	LogString(GENERAL, "SetWatchdogTimeout %u", m_timeout);
#if !defined(_UNIT_TEST)
	if (IsWatchdogEnabled())
	{
		PingWatchdog();
		// Try to give us enough time to write the logs without a hardware watchdog reset.
		if (S7LITE_Watchdog_SetTimeout(timeout) != S7DLL_STATUS_OK)
		{
			LogString(ERROR_LOG, _T("Timeout cannot be updated on the watchdog the Watchdog"));
		}
		PingWatchdog();
	}
#endif // _UNIT_TEST
}

void CWatchDog::LogRegisteredThreads(const std::string &loggingid)
{
	lock_guard<recursive_mutex> lock(m_criticalSection);
	LogString(loggingid, "Currently REGISTERED Threads (Log Header) -- begin");

	// Iterate through the list of registered threads while lock is held.
	WatchdogThreadMap::const_iterator pThreadItem(m_threadList.cbegin());
	while (m_threadList.cend() != pThreadItem)
	{
		ostringstream threadIdString;
		threadIdString << pThreadItem->first;
		LogString(loggingid, "Currently REGISTERED %s, Thread ID = %s",
			static_cast<LPCTSTR>(pThreadItem->second.GetName().c_str()),
			static_cast<LPCTSTR>(threadIdString.str().c_str()));
		++pThreadItem;
	}

	LogString(loggingid, "Currently REGISTERED Threads (Log Header) -- end");
}

void CWatchDog::ReportThreadCallstackToDisk(const std::string &name, bool isThreadTimeout)
{
	SetWatchdogTimeout(WATCHDOG_HARDWARE_TIMEOUT_WHILE_RESTARTING);

    std::thread::id currentThread = CUtilities::GetCurrentThreadId();

    std::string exceptionType = (isThreadTimeout) ? "Thread timeout" : "Exception: " + GetExceptionType(currentThread);

    std::string stackTrace = GetExceptionStackTrace(currentThread);

	std::cout << "ReportThreadCallstackToDisk\n" << stackTrace << std::endl;

    string targetDir = CDirectoryManager::CallstackFilePath();

	CDiagnostics::WriteExceptionToFile(targetDir, name, exceptionType, stackTrace, isThreadTimeout);
}

std::recursive_mutex callstackMutex;
binsem callStackReadyEvent(0);
static pthread_t callingThread(0);
static pthread_t targetThread(0);
static std::string other_thread_exception_stack_trace; 

#define CALLSTACK_SIG SIGUSR2

#if !defined(_UNIT_TEST)
int backtrace2(void** trace, int size, int skip)
{
	int retVal = 0;

	unw_cursor_t cursor; 
	unw_context_t uc;
	unw_word_t ip;

	unw_getcontext(&uc);
	unw_init_local(&cursor, &uc);
	while (unw_step(&cursor) > 0) 
	{
		unw_get_reg(&cursor, UNW_REG_IP, &ip);
		if (skip-- <= 0)
		{
			trace[retVal++] = (void *)ip;
			if (retVal >= size) 
				break;
		}
	}

	return retVal;
}
#endif

std::string Backtrace(int skip = 1)
{
	const DWORD MAX_STACK_LENGTH = 32;
	const DWORD SYMBOL_BUFFER_LENGTH = 1024;

	void *callstack[MAX_STACK_LENGTH];
	const int nMaxFrames = sizeof(callstack) / sizeof(callstack[0]);

	int nFrames = backtrace(callstack, nMaxFrames);

#if !defined(_UNIT_TEST)
	if (nFrames < 3)
	{
		nFrames = backtrace2(callstack, nMaxFrames, 0);
	}
#endif

	char **symbols = backtrace_symbols(callstack, nFrames);

	std::ostringstream trace_buf;

	for (int i = skip; i < nFrames; i++) 
	{
		char buf[SYMBOL_BUFFER_LENGTH];

		Dl_info info;
		if (dladdr(callstack[i], &info) && info.dli_sname) 
		{
			char *demangled = NULL;
			int status = -1;

			if (info.dli_sname[0] == '_')
				demangled = abi::__cxa_demangle(info.dli_sname, NULL, 0, &status);

			snprintf(buf, sizeof(buf), "%-3d [%*p] %s + 0x%zx\n",
					 i, int(2 + sizeof(void*) * 2), callstack[i],
					 status == 0 ? demangled :
					 info.dli_sname == 0 ? symbols[i] : info.dli_sname,
					 (char *)callstack[i] - (char *)info.dli_saddr);

			free(demangled);
		} 
		else 
		{
			snprintf(buf, sizeof(buf), "%-3d [%*p] %s\n",
					 i, int(2 + sizeof(void*) * 2), callstack[i], symbols[i]);
		}

		trace_buf << buf;
	}

	free(symbols);

	if (nFrames == nMaxFrames)
		trace_buf << "[truncated]\n";

	return trace_buf.str();
}

__attribute__((noinline))
static void _callstack_signal_handler(int signr, siginfo_t *info, void *secret) 
{
	UNREFERENCED_PARAMETER(&signr); 
	UNREFERENCED_PARAMETER(&info); 
	UNREFERENCED_PARAMETER(&secret); 

    pthread_t myThread = pthread_self();
	if (myThread != targetThread)
	{
		LogString(ERROR_LOG, _T("Received signal to generate callstack on thread: [0x%lx], expected: [0x%lx]."),
			myThread, targetThread);
		return;
	}
	
    other_thread_exception_stack_trace = Backtrace();
	
	// signal calling thread that callstack is ready
	callStackReadyEvent.signal(false);
}

static void _setup_callstack_signal_handler() 
{
	struct sigaction sa;
	sigfillset(&sa.sa_mask);
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = _callstack_signal_handler;
	sigaction(CALLSTACK_SIG, &sa, NULL);	
}

__attribute__((noinline))
std::string GetCallstack(pthread_t threadId) 
{
	if (threadId == 0)
		return "<<threadId == 0>>";

    lock_guard<recursive_mutex> lock(callstackMutex);
    callingThread = pthread_self(); 
	targetThread = threadId;
    other_thread_exception_stack_trace = "";
	
	_setup_callstack_signal_handler();

	// call _callstack_signal_handler in target thread
	if(pthread_kill(threadId, CALLSTACK_SIG) != 0)
	{
		LogString(ERROR_LOG, _T("Sending CALLSTACK_SIG to thread [0x%lx] failed."), threadId);

		// something failed ...
		return "<<pthread_kill failed>>";
	}

	bool success = callStackReadyEvent.try_wait_for(CALLSTACK_GATHERING_TIMEOUT_SECS * 1000);
	if (!success)
	{
		LogString(ERROR_LOG, _T("Gathering call stack timed out for the thread [0x%lx]."), threadId);
	}



    return other_thread_exception_stack_trace;
}

void CWatchDog::AppendAllThreadCallStacks(std::string &exceptionString, std::thread::id otherThan, bool writeHeaderFooter)
{
	if (writeHeaderFooter)
		CDiagnostics::AppendSecondaryThreadsHeaderToString(exceptionString);

	{
		lock_guard<recursive_mutex> lock(m_criticalSection);
		WatchdogThreadMap::iterator pThreadItem(m_threadList.begin());
		while (m_threadList.cend() != pThreadItem)
		{
			if (pThreadItem->first != otherThan)
			{
				LogString(GENERAL, _T("Appending callstack for the thread with Name [%s], NativeHandle: [0x%lx]"), 
					pThreadItem->second.GetThreadName().c_str(), pThreadItem->second.GetNativeHandle());
				CDiagnostics::AppendThreadCallstackToString(exceptionString, pThreadItem->second.GetThreadName(), "", GetCallstack(pThreadItem->second.GetNativeHandle()));
			}
			++pThreadItem;
		}
	}

	if (writeHeaderFooter)
		CDiagnostics::AppendSecondaryThreadsFooterToString(exceptionString);
}

void CWatchDog::SafeSleep(std::thread::id threadId, DWORD wait)
{
	DWORD start = CUtilities::GetTickCount();
	while (CUtilities::GetTickCount() - start < wait)
	{
		PingWatchdog();
		Checkin(threadId);
		CUtilities::Sleep(WATCHDOG_CHECKIN);
	}
}

std::string demangle(const char *name) 
{
    int status;
    std::unique_ptr<char, void(*)(void*)> realname(abi::__cxa_demangle(name, 0, 0, &status), &std::free);
    return status ? name : &*realname;
}

extern "C" 
{
    void __cxa_throw(void *ex, void *info, void (*dest)(void *)) 
    {
        std::string exception_name = demangle(reinterpret_cast<const std::type_info*>(info)->name());
        std::string exception_stack_trace = Backtrace();

        CWatchDog::Instance().SetExceptionInformation(
            CUtilities::GetCurrentThreadId(),
            exception_name,
            exception_stack_trace
        );

        static void (*const rethrow)(void*,void*,void(*)(void*)) __attribute__ ((noreturn)) = (void (*)(void*,void*,void(*)(void*)))dlsym(RTLD_NEXT, "__cxa_throw");
        rethrow(ex,info,dest);
    }
}

void GlobalSignalHandler(int sig) 
{
	// The "abort" towards the end of this function will cause the kernel to call us again.
	// We don't want to log that recursive call.
	if (!CWatchDog::Instance().IsSignalTriggered() && !CWatchDog::Instance().IsThreadTimedOut() && !CWatchDog::Instance().IsShutdownInProcess())
	{
		CDiagnostics::Instance().PersistReboot(tltRebootException, ERROR_LOG);

		CWatchDog::Instance().SignalTriggered();

		CWatchDog::Instance().SetWatchdogTimeout(WATCHDOG_HARDWARE_TIMEOUT_WHILE_RESTARTING);

    	char *signalDescr = strsignal(sig);

		std::string exception_name(signalDescr);

        std::string exception_stack_trace = Backtrace();

		string threadName = CWatchDog::Instance().GetThreadName(CUtilities::GetCurrentThreadId());

		cout << "GlobalSignalHandler exception name=" << exception_name << " threadName=" << threadName << endl;

        string targetDir = CDirectoryManager::CallstackFilePath();		

		CDiagnostics::WriteExceptionToFile(targetDir, threadName, exception_name, exception_stack_trace, false); 
    	
		CWatchDog::Instance().BeginShutdown(CWebDiagnosticsStatusInfo::TiltCodeToString(tltRebootException));
	}
}

void ReportSingleCallstack(std::string callStackReason)
{
    string stackTrace = Backtrace();
    string targetDir = CDirectoryManager::CallstackFilePath();

    CDiagnostics::ReportThreadCallstackToFile( targetDir, "", callStackReason, stackTrace);
}
