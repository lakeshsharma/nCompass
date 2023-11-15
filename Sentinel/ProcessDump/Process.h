#pragma once

class CProcess
{
public:
	/// <summary>
	/// Default constructor. Initializes a new instance of the <see cref="CProcess"/> class.
	/// </summary>
	CProcess(void);

	/// <summary>
	/// Copy constructor. Initializes a new instance of the <see cref="CProcess"/> class.
	/// </summary>
	/// <param name="rhs">The RHS.</param>
	CProcess(const CProcess &rhs);

	/// <summary>
	/// Destructor. Finalizes an instance of the <see cref="CProcess"/> class.
	/// </summary>
	~CProcess(void);

	/// <summary>
	/// Copy operator.
	/// </summary>
	/// <param name="rhs">IN - The instance to copy from.</param>
	/// <returns></returns>
	CProcess& operator=(const CProcess &rhs);

	std::string m_processName;
	pid_t m_processId;
	DWORD m_memUsage; // 2014-01-24 This matches the Task Manager "Mem Usage" column.
	DWORD m_vmSize; // 2014-01-24 This matches the Task Manager "VM Size" column.
	DWORD m_moduleSize;
	ULONGLONG m_totalProcTime;
	ULONGLONG m_prevProcTime;
	bool m_currentActive;
	short m_cpuUsage;
	short m_peakCpuUsage; // Highest value from any measurement period.
	DWORD m_handleCount;

private:
};
