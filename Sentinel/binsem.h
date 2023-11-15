#pragma once
#include <stdafx.h>
// binary semaphore
class binsem {
public:
    explicit binsem(int init_count = CountMax);
#ifndef _UNIT_TEST
    ~binsem();
#endif

    // P-operation / acquire
    void wait();
    bool try_wait();

	/// <summary>
	/// Waits until this instance is in the signaled state or the time-out interval elapses.
	/// </summary>
	/// <param name="ms">IN - The time-out interval, in milliseconds.</param>
   /// <param name="resetTimeOutSignal">IN - should signal as 
   /// timeout be reset</param> 

	/// <returns>true indicates that this instance became signaled, false indicates that the interval elapsed.</returns>
	/// <remarks>This instance is set to non-signaled when true is returned.</remarks>
    bool try_wait_for(uint timeout, bool resetTimeoutSignal = true);

    // V-operation / release
    void signal(bool fakeTimeout);

    /// <summary>
    /// Allocates list of the binsem objects
    /// </summary>
    static void AllocateBinSemList();

    /// <summary>
    /// Disposes list of the binsem objects
    /// </summary>
    static void DisposeBinSemList();

    /// <summary>
    /// Signal all of the binsem objects
    /// </summary>
    /// <param name="fakeTimeout">Signal all binsems as a 
    /// timeout</param> 
    static void SignalAllBinSems(bool signalAsTimeout);

    // Lockable requirements
    void lock();
    bool try_lock();
    void unlock();

private:
    static const int CountMax = 1;
    int m_count;
    std::mutex m_mutex;
    std::condition_variable m_conditionVariable;
    bool m_signalAsTimeout;
};

