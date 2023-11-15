#pragma once

///	<summary>
/// Watchdog thread information class.
///	</summary>
/// <remarks>
/// Uses default copy constructor, assignment operator, and destructor.
/// </remarks>
class CWatchdogThreadInfo
{
public:
	/// <summary>
	/// Default constructor. Initializes a new instance of the <see cref="CWatchdogThreadInfo"/> class.
	/// </summary>
	CWatchdogThreadInfo();

	/// <summary>
	/// Standard parameterized constructor. Initializes a new instance of the <see cref="CWatchdogThreadInfo"/> class.
	/// </summary>
	/// <param name="name">IN - Name of the thread.</param>
	/// <param name="timeoutTickCount">IN - Timeout period in tick counts for the thread is considered 'frozen'.</param>
    /// <param name="tickCount">IN - The current tick count.</param> 
    /// <param name="nativeHandle">IN - The O/S handle for the 
    /// thread.</param> 
	/// <remarks>Sets the checkin time to the current tick count.</remarks>
	CWatchdogThreadInfo(const std::string &name, DWORD timeoutTickCount, DWORD tickCount, pthread_t nativeHandle);

	/// <summary>
	/// Gets the tick count for the last check in for the thread.
	/// </summary>
	/// <returns>The tick count for the last check in for the thread.</returns>
	DWORD GetCheckInTime() const;

	/// <summary>
	/// Gets the time out period in tick counts for the thread.
	/// </summary>
	/// <returns>The time out period in tick counts for the thread.</returns>
	DWORD GetTimeOutMilliseconds() const;

	/// <summary>
	/// Public access to the 'name' property.
	/// </summary>
	/// <returns>Name string.</returns>
	std::string GetName() const;

	/// <summary>
	/// Public access to the 'suspended' property.
	/// </summary>
	/// <returns>Whether or not the thread is recorded as in a suspended state.</returns>
	bool IsSuspended() const;

	/// <summary>
	/// Test if the timeout duration has been exceeded since th last checkin.
	/// </summary>
	/// <returns>'true' if the timeout duration has been exceeded.</returns>
	/// <param name="tickCount">IN - The current tick count.</param>
	/// <remarks>
	/// If the thread is recorded as 'suspended', 'false' is returned.
	/// </remarks>
	bool HasTimedOut(DWORD tickCount) const;

	/// <summary>
	/// Resets the checkin time to the current tick count.
	/// </summary>
	/// <param name="tickCount">IN - The current tick count.</param>
	void CheckIn(DWORD tickCount);

	/// <summary>
	/// Sets the suspended state back to false and resets the checkin time.
	/// </summary>
	/// <param name="tickCount">IN - The current tick count.</param>
	void Resume(DWORD tickCount);

	/// <summary>
	/// Sets the suspended state as 'true'.
	/// </summary>
	/// <remarks>
	/// Will result in 'HasTimedOut' as always returning 'false'.
	/// </remarks>
	void Suspend();

    void SetExceptionInformation(const std::string& exceptionType, const std::string& exceptionStackTrace);
    std::string GetExceptionType();
    std::string GetExceptionStackTrace();
    std::string GetThreadName();
    pthread_t GetNativeHandle();

private:
	DWORD m_checkInTime;
	DWORD m_timeOutMiliSeconds;
    std::string m_name;
    bool m_suspended;
    pthread_t m_nativeHandle;
    std::string m_exception_type;
    std::string m_exception_stack_trace;
};

