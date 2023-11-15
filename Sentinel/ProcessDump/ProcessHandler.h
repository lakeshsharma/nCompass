#pragma once

#include "Process.h"

/// <summary>
/// Class used for gathering information about processes.
/// Note that unless the caller has the SeProfileSingleProcessPrivilege (or better),
/// information (other than PID and process name) cannot be gathered for many of the system processes.
/// </summary>
class CProcessHandler
{
public:
	typedef std::map<pid_t, CProcess *> MapType;
	typedef MapType::const_iterator IteratorType;

	/// <summary>
	/// Default constructor. Initializes a new instance of the <see cref="CProcessHandler"/> class.
	/// </summary>
	CProcessHandler();

	/// <summary>
	/// Destructor. Finalizes an instance of the <see cref="CProcessHandler"/> class.
	/// </summary>
	~CProcessHandler();

	/// <summary>
	/// Starts this instance.
	/// </summary>
	void Start();

	/// <summary>
	/// Gets the first process.
	/// </summary>
	/// <param name="processPosition">OUT - The process position used for iterating though the list of processes.</param>
	/// <returns>A pointer to the first process, NULL if there are no processes.</returns>
	const CProcess * GetFirstProcess(IteratorType &processPosition) const;

	/// <summary>
	/// Gets the next process.
	/// </summary>
	/// <param name="processPosition">IN/OUT - The process position used for iterating though the list of processes.</param>
	/// <returns>A pointer to the next process, NULL if there are no more processes.</returns>
	const CProcess * GetNextProcess(IteratorType &processPosition) const;

	/// <summary>
	/// Adds the process to the list of processes.
	/// </summary>
	/// <param name="process">IN - The process.</param>
	/// <returns>true if the process was added, false otherwise</returns>
	/// <remarks>
	/// Return false if the process is already present.
	/// </remarks>
	bool AddProcess(const CProcess &process);

	bool ProcessExist( LPCTSTR processName );
	bool WaitForProcess( LPCTSTR processName, unsigned int seconds );

	/// <summary>
	/// Updates the list of currently active processes.
	/// In general, does not update the information associated with the processes.
	/// </summary>
	void GetProcesses();

	/// <summary>
	/// Updates the information associated with each process.
	/// </summary>
	void Update();

	/// <summary>
	/// Gets the process name for the specified process identifier.
	/// </summary>
	/// <param name="processId">IN - The process identifier.</param>
	std::string GetProcessName(pid_t processId);

	/// <summary>
	/// Gets memory information for the specified process.
	/// </summary>
	/// <param name="processId">IN - The process identifier.</param>
	/// <param name="memUsage">OUT - The physical memory used by the process. This matches the Task Manager "Mem Usage" column.</param>
	/// <param name="vmSize">OUT - The virtual memory used by the process. This matches the Task Manager "VM Size" column.</param>
	/// <param name="moduleSize">OUT - The (read only / code) size of the process modules.</param>
	void GetMemoryInfo(pid_t processId, DWORD &memUsage, DWORD &vmSize, DWORD &moduleSize);

	void CleanUp();
	void GetThreadTime();
	ULONGLONG AddTimes(const ULONGLONG& ftA, const ULONGLONG& ftB);
	void GetHandleCount();

private:
	// Disable default copy constructor, and default assignment operator.
	CProcessHandler(const CProcessHandler &rhs);
	CProcessHandler& operator=(const CProcessHandler &rhs);

	static const DWORD DEFAULT_NUMBER_OF_PROCESSORS = 2;

	MapType m_processes;
	DWORD m_lastDumpTime;
	unsigned m_numberOfProcessors;
	long m_pageSize;
	long m_scClockTicksPerSecond;
};
