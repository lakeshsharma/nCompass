#include "stdafx.h"
#include "ProcessHandler.h"

#include "GlobalDefs.h"
#include "Logging/LogString.h"
#include "Utilities.h"

using namespace std;

const DWORD DIAGNOSTICS_SLEEP_YIELD_MILLISECONDS = 10;

CProcessHandler::CProcessHandler() :
	m_lastDumpTime(CUtilities::GetTickCount()),
	m_numberOfProcessors(DEFAULT_NUMBER_OF_PROCESSORS),
	m_pageSize(sysconf(_SC_PAGE_SIZE)),
	m_scClockTicksPerSecond(sysconf(_SC_CLK_TCK))
{
}

CProcessHandler::~CProcessHandler(void)
{
	CleanUp();
}

void CProcessHandler::Start()
{
	// Get the number of Processors.
	unsigned dwNumberOfProcessors = thread::hardware_concurrency();
	if (dwNumberOfProcessors > 0)
	{
		m_numberOfProcessors = dwNumberOfProcessors;
	}
}


void CProcessHandler::CleanUp()
{
	IteratorType procPos = m_processes.cbegin();
	CProcess *proc = NULL;
	while (procPos != m_processes.cend())
	{
		proc = (*procPos++).second;

		if(proc != NULL)
			delete proc;

		proc = NULL;
		CUtilities::Sleep(DIAGNOSTICS_SLEEP_YIELD_MILLISECONDS);
	}
	m_processes.clear();

}

const CProcess * CProcessHandler::GetFirstProcess(IteratorType &processPosition) const
{
	processPosition = m_processes.cbegin();
	return GetNextProcess(processPosition);
}

const CProcess * CProcessHandler::GetNextProcess(IteratorType &processPosition) const
{
	CProcess *process(NULL);
	if (processPosition != m_processes.cend())
	{
		process = (*processPosition++).second;
	}

	return process;
}

bool CProcessHandler::AddProcess(const CProcess &process)
{
	bool ok(false);

	CProcess *proc(NULL);
	IteratorType iter = m_processes.find(process.m_processId);
	if (iter == m_processes.end())
	{
		//create the process object add it to our list
		proc = new CProcess(process);
		m_processes[proc->m_processId] = proc;
		ok = true;
	}

	return ok;
}

void CProcessHandler::GetProcesses()
{
	try
	{
		{
			pid_t procID;

			// set all existing processes as not active
			IteratorType procPos = m_processes.cbegin();
			CProcess *proc = NULL;
			while (procPos != m_processes.cend())
			{
				proc = (*procPos++).second;
				proc->m_currentActive = false;
				CUtilities::Sleep(DIAGNOSTICS_SLEEP_YIELD_MILLISECONDS);
			}

			DIR *directory(opendir("/proc"));
			if (nullptr == directory)
			{
				LogString(ERROR_LOG, "Error: failed to open directory \"/proc\"");
			}
			else
			{
				dirent *procEntry(readdir(directory));
				while (procEntry != nullptr)
				{
					// Directory entry names that start with a digit should be PIDs (process IDs).
					if (isdigit(*procEntry->d_name))
					{
						//find process in array
						proc = NULL;
						pid_t processId = (pid_t)atol(procEntry->d_name);
						IteratorType procPos = m_processes.find(processId);
						if (procPos != m_processes.end())
						{
							proc = procPos->second;
						}
						else
						{
							//create the process object add it to our list
							proc = new CProcess();
							proc->m_processId = processId;
							m_processes[proc->m_processId] = proc;
						}
						proc->m_processName = GetProcessName(proc->m_processId);
						proc->m_processId = processId;
						proc->m_currentActive = true;

						GetMemoryInfo(proc->m_processId, proc->m_memUsage, proc->m_vmSize, proc->m_moduleSize);
					}

					//Get the next process;
					procEntry = readdir(directory);

					CUtilities::Sleep(DIAGNOSTICS_SLEEP_YIELD_MILLISECONDS);
				}
				closedir(directory);
			}
			
			// remove any non active processes, start at end and work to beginning.
			procPos = m_processes.cbegin();
			proc = NULL;
			while (procPos != m_processes.cend())
			{
				procID = (*procPos).first;
				proc = (*procPos++).second;
				if (!proc->m_currentActive)
				{
					m_processes.erase(procID);
					delete proc;
				}
				CUtilities::Sleep(DIAGNOSTICS_SLEEP_YIELD_MILLISECONDS);
			}

		}
	}
	catch(...)
	{
	}
}
	
void CProcessHandler::Update()
{
	//First get the processes
	GetProcesses();
	// get elapsed time since last process dump.
	ULONGLONG totalTime;
	DWORD fileTime;
	fileTime = CUtilities::GetTickCount();
	totalTime = (fileTime - m_lastDumpTime) * FILETIME_UNITS_PER_MILLISECOND;
	m_lastDumpTime = fileTime;

	if (totalTime == 0)
	{
		// prevent divide by zero by quitting now
		return;
	}

	//Now for each process get all of the heaps
	GetThreadTime();
	GetHandleCount();

	IteratorType procPos = m_processes.cbegin();
	while (procPos != m_processes.cend())
	{
		CProcess *proc(NULL);
		proc = (*procPos++).second;
		if (proc != NULL)
		{

			// calculate total time used since last update
			ULONGLONG timeUsedSinceLastUpdate = proc->m_totalProcTime - proc->m_prevProcTime;
			proc->m_cpuUsage = short(timeUsedSinceLastUpdate * 100 / totalTime / m_numberOfProcessors);
			if (proc->m_cpuUsage < 0)
			{
				//TRACE(L"time = %d: %X - %s \n",proc->m_cpuUsage, proc->m_processId, proc->m_processName);
				proc->m_cpuUsage = 0;
			}

			if (proc->m_peakCpuUsage < proc->m_cpuUsage)
			{
				proc->m_peakCpuUsage = proc->m_cpuUsage;
			}

			CUtilities::Sleep(DIAGNOSTICS_SLEEP_YIELD_MILLISECONDS);
		}
	}

}

void CProcessHandler::GetMemoryInfo(pid_t processId, DWORD &memUsage, DWORD &vmSize, DWORD &moduleSize)
{
	memUsage = 0;
	vmSize = 0;

	string fileName(FormatString("/proc/%u/statm", (unsigned)processId));
	FILE *file = fopen(fileName.c_str(), "r");
	if (file != nullptr)
	{
		unsigned long statmSize(0);
		unsigned long statmText(0);
		unsigned long statmData(0);
		fscanf(file, "%lu %*u %*u %lu %*u %lu", &statmSize, &statmText, &statmData);
		fclose(file);

		memUsage = (DWORD)(statmData * (ULONGLONG)m_pageSize / BYTES_PER_KB);
		vmSize = (DWORD)(statmSize * (ULONGLONG)m_pageSize / BYTES_PER_KB);
		moduleSize = (DWORD)(statmText * (ULONGLONG)m_pageSize / BYTES_PER_KB);
	}
}

std::string CProcessHandler::GetProcessName(pid_t processId)
{
	// Get the process name from "/proc/[pid]/comm" - the process name can be truncated to 16 characters with this source.
	// Alternatively, "/proc/[pid]/cmdline" should begin with the full pathname for the command.
	// Alternatively, "/proc/[pid]/stat" should have the process name for the command as the 2nd value parenthesized.
	// - Filenames and pathnames that contain whitespace, parentheses, and/or other special characters are generally considered problemnatic. 
	string fileName(FormatString("/proc/%u/comm", (unsigned)processId));
	ifstream file(fileName, ios::in);
	string processName;
	getline(file, processName);

	return processName;
}

void CProcessHandler::GetThreadTime()
{
	CProcess * proc;

	try
	{
		IteratorType procPos = m_processes.cbegin();
		while (procPos != m_processes.cend())
		{
			proc = (*procPos++).second;
			if (proc)
			{
				{
					ULONGLONG kernelTime;
					ULONGLONG userTime;

					// Get process times directly from the operating system.
					// Summing current thread times does not provide total process times.
					string fileName(FormatString("/proc/%u/stat", (unsigned)proc->m_processId));
					ifstream file(fileName, ios::in);

					if (!file)
					{
						// It is unusual for a process to exit while we are gathering data, but not an error.
						DWORD lastError = CUtilities::GetLastError();
						LogString(WARNING_LOG, _T("fopen(fileName=%s) did not succeed name=%s id=%u lastError=%u in %s(%d)"),
							static_cast<LPCTSTR>(fileName.c_str()),
							static_cast<LPCTSTR>(proc->m_processName.c_str()),
							(unsigned)proc->m_processId,
							(unsigned)lastError,
							static_cast<LPCTSTR>(__TFUNCTION__), __LINE__);

						proc->m_prevProcTime = proc->m_totalProcTime; // Treat as zero CPU time elapsed during this interval.
					}
					else
					{
						// The process for the command should be present as the 2nd value parenthesized.
						// - Filenames and pathnames that contain whitespace, parentheses, and/or other special characters are generally considered problemnatic.
						// - Matching the 2nd value here against the name from "/proc/[pid]/comm" might be reliable.
						string statValue;
						getline(file, statValue);
						LPCSTR endProcessName(") ");
						string::size_type pos(statValue.find(endProcessName)); //Try to find the end of the process name.
						if (string::npos == pos)
						{
							LogString(ERROR_LOG, "Error: invalid format for process name: \"%s\" constains \"%s\"", fileName.c_str(), statValue.c_str());
						}
						else
						{
							unsigned long utime(0); // CPU time scheduled in user mode.
							unsigned long stime(0); // CPU time scheduled in kernel mode.

							pos += strlen(endProcessName);
							sscanf(statValue.substr(pos).c_str(), "%*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu",
								&utime, &stime);
							userTime = utime * (ULONGLONG)(FILETIME_UNITS_PER_SECOND / m_scClockTicksPerSecond);
							kernelTime = stime * (ULONGLONG)(FILETIME_UNITS_PER_SECOND / m_scClockTicksPerSecond);

							proc->m_prevProcTime = proc->m_totalProcTime;
							proc->m_totalProcTime = AddTimes(userTime, kernelTime);
						}
					}
				}
			}
			CUtilities::Sleep(DIAGNOSTICS_SLEEP_YIELD_MILLISECONDS);
		}
	}
	catch(...)
	{
	}
	return ;
}

ULONGLONG CProcessHandler::AddTimes(const ULONGLONG& ftA, const ULONGLONG& ftB)
{
	return ftA + ftB;
}

void CProcessHandler::GetHandleCount()
{
	IteratorType procPos = m_processes.cbegin();
	while (procPos != m_processes.cend())
	{
		CProcess *proc;
		proc = (*procPos++).second;
		if (proc)
		{
			{
				DWORD handleCount(0);

				string directoryName(FormatString("/proc/%u/fd", proc->m_processId));
				DIR *directory(opendir(directoryName.c_str()));
				if (nullptr == directory)
				{
					// It is unusual for a process to exit while we are gathering data, but not an error.
					DWORD lastError = CUtilities::GetLastError();
					LogString(WARNING_LOG, _T("opendir(fileName=%s) did not succeed name=%s id=%u lastError=%u in %s(%d)"),
						static_cast<LPCTSTR>(directoryName.c_str()),
						static_cast<LPCTSTR>(proc->m_processName.c_str()),
						(unsigned)proc->m_processId,
						(unsigned)lastError,
						static_cast<LPCTSTR>(__TFUNCTION__), __LINE__);

					// Retain the previous handle count as useful information.
				}
				else
				{
					dirent *directoryEntry(readdir(directory));
					while (directoryEntry != nullptr)
					{
						// Don't count entries where d_name is "." or ".." -- by either counting d_type=DT_LNK, excluding d_type=DT_DIR, or checking d_name.
						if (directoryEntry->d_type != DT_DIR)
						{
							++handleCount;
						}
						directoryEntry = readdir(directory);
						CUtilities::Sleep(DIAGNOSTICS_SLEEP_YIELD_MILLISECONDS);
					}
					closedir(directory);
					proc->m_handleCount = handleCount;
				}
			}
		}

		CUtilities::Sleep(DIAGNOSTICS_SLEEP_YIELD_MILLISECONDS);
	}
}

bool CProcessHandler::ProcessExist( LPCTSTR processName )
{
	bool processFound = false;

	GetProcesses();

	IteratorType procPos = m_processes.cbegin();
	CProcess *proc = NULL;
	while (procPos != m_processes.cend() && !processFound)
	{
		proc = (*procPos++).second;
		if (CUtilities::CompareStringsNoCase(proc->m_processName.c_str(), processName) == 0)
			processFound = true;
		CUtilities::Sleep(DIAGNOSTICS_SLEEP_YIELD_MILLISECONDS);
	}
	
	return processFound;
}

bool CProcessHandler::WaitForProcess( LPCTSTR processName, unsigned int seconds )
{
	bool processFound = ProcessExist( processName );
	for ( unsigned int n = 0; n < seconds && !processFound; n++ )
	{
		CUtilities::Sleep(1000);
		processFound = ProcessExist( processName );
	}
	return processFound;
}
