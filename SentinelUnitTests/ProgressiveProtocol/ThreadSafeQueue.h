#pragma once
#include "binsem.h"
#include <atomic>
#include <limits>

#define WAIT_OBJECT_0 0
#define WAIT_TIMEOUT -1
#define INFINITE INT_MAX

///	<summary>
/// A thread safe queue.
///	</summary>
///	<remarks>
/// ThreadSafeQueue uses a semaphore for adding items to the queue which ensures
///	that no messages are lost. In addition, the wait on this semaphore also monitors
///	multiple events allowing the wait to be canceled.
///	</remarks>
template<class T_VAL>
class ThreadSafeQueue
{
private:
	std::queue<T_VAL> m_queue;
	long   m_maxItems;
	binsem m_hCloseEvent;
	binsem m_hFilledSemaphore;
	binsem m_hEmptySemaphore;
    std::recursive_mutex m_queueCS;
	std::atomic< unsigned long> m_filledCount;
	std::atomic< unsigned long> m_emptyCount;

public:
	///	<summary>
	/// An enumeration representing the possible return values from
	/// <paramref name="Push"/> or <paramref name="Pop"/>.
	///	</summary>
	enum Error
	{
		/// <summary>The wait timed out.</summary>
		Timeout = -3,
		/// <summary>The wait was canceled due to the queue being closed.</summary>
		Closed = -2,
		/// <summary>The wait was canceled by the optional handle.</summary>
		Canceled = -1,
		/// <summary>The operation succeeded.</summary>
		Success = 0
	};

private:
	ThreadSafeQueue(const ThreadSafeQueue&);
	void operator=(const ThreadSafeQueue&);

public:
	///	<summary>
	/// ThreadSafeQueue constructor.
	///	</summary>
    /// <param name="maxItems">The maximum number of items
    /// thahCancelt can be queued.</param>
	///	<param name="open">If <b>true</b> queue will be constructed in the open state.</param>
	ThreadSafeQueue(long maxItems, bool open = true)
		: m_maxItems(maxItems <= 0 ? 1 : maxItems)
		, m_filledCount(0)
		, m_emptyCount(m_maxItems)
	{
        m_hFilledSemaphore.wait();
		m_hEmptySemaphore.signal(false);//the queue is empty in starting 
        m_hCloseEvent.wait();
	}

	///	<summary>
	/// ThreadSafeQueue destructor.
	///	</summary>
	~ThreadSafeQueue()
	{
		Close();

        m_hFilledSemaphore.signal(false);
		m_hEmptySemaphore.signal(false);
        m_hCloseEvent.signal(false);
	}

	///	<summary>
	///	Indicates whether items can be added or removed from the queue.
	///	</summary>
	///	<returns><b>true</b> if items can be added or removed, else <b>false</b>.</returns>
	bool IsOpen()
	{
        return ( m_hCloseEvent.try_wait_for(0) == true ); //Wait for signal on the semaphore
	}

	///	<summary>
	///	Opens the queue, allowing items to be added or removed.
	///	</summary>
	/// <remarks>
	/// This does not create the queue, but rather resets the close event.
	/// </remarks>
	void Open()
	{
        m_hCloseEvent.wait();
        CUtilities::Sleep(0);
	}

	///	<summary>
	///	Closes the queue, canceling all waits.
	///	</summary>
	/// <remarks>
	/// This should generally only be called by the consumer when it is shutting down.
	/// By calling this as early as possible, the senders have time to exit the
	/// <paramref name="Push"/> and either start terminating immediately or be in
	/// state to gracefully terminate.
	/// </remarks>
	void Close()
	{
		m_hCloseEvent.signal(false);//Signal the semaphore
        CUtilities::Sleep(0);
	}

	///	<summary>
	/// Adds an item to the queue.
	///	</summary>
	///	<param name="item">The message to add. On Success, the pointer in this
	///		parameter will be moved to the queue and will become invalid.</param>
	///	<param name="timeout">How long to wait before returning.</param>
	///	<param name="hCancel">An optional, additional handle on which to wait.</param>
	///	<returns>Zero on Success, else one of the values from the Error enumeration.</returns>
	///	<remarks>
	/// The most flexible way to exit a thread gracefully is to monitor an event
	///	handle. Pass this handle to this method and check the return every loop.
	///	</remarks>
	Error Push(const T_VAL& item, DWORD timeout = INFINITE, binsem  *hCancel = nullptr, DWORD thread = 0)
	{
		const int maxHandles = 3;
		binsem handles[maxHandles];
		DWORD  numHandles = 0;

        bool signalled  = m_hCloseEvent.try_wait();
        if ( signalled ) 
        {
             return Closed ;
        }

        // The order is important since windows returns the index of the first signaled handle.
        if (hCancel!=nullptr) {
    
            signalled  = hCancel->try_wait();
            if ( signalled ) 
            {
               return Canceled;
            }
        }
        
		signalled  = m_hEmptySemaphore.try_wait();
        if (!signalled ) 
        {
           return Timeout;
        }

        m_emptyCount-- ;//decrement the count 
		// Even if hCloseEvent or hCancel are signaled after the above wait, processing
		// is continued to maintain the integrity of the semaphores.

        std::unique_lock<std::recursive_mutex> queueCS(m_queueCS); //unique_lock on mutex m_queueCS

		m_queue.push(move(item));

        m_hFilledSemaphore.signal(false); //increment the m_hFilledSemaphore by 1 , so one more semaphore is added to critical section
		m_filledCount++;
		return Success;
	}

	///	<summary>
	/// Removes an item from the queue.
	///	</summary>
	///	<param name="item">The message to remove. On Success, any pointer in this
	///		parameter will be destructed and replaced by the pointer from the
	///		queue.</param>
	///	<param name="timeout">How long to wait before returning.</param>
	///	<param name="hCancel">An optional, additional handle on which to wait.</param>
	///	<returns>Zero on Success, else one of the values from the Error enumeration.</returns>
	///	<remarks>
	/// The most flexible way to exit a thread gracefully is to monitor an event
	///	handle. Pass this handle to this method and check the return every loop.
	///	</remarks>
	Error Pop(T_VAL& item, DWORD timeout = INFINITE, binsem *hCancel = nullptr)
	{
		const int maxHandles = 3;
		binsem handles[maxHandles];
		DWORD  numHandles = 0;
        bool signalled  = m_hCloseEvent.try_wait();
        if ( signalled ) 
        {
             return Closed ;
        }
        // The order is important since windows returns the index of the first signaled handle.
		if (hCancel!=nullptr)
			 signalled  = hCancel->try_wait();
        if ( signalled ) 
        {
           return Canceled;
        }
        
        signalled  =  m_hFilledSemaphore.try_wait();
        if ( !signalled ) 
        {
           return Timeout;
        }
		m_filledCount--;

		// Even if hCloseEvent or hCancel are signaled after the above wait, processing
		// is continued to maintain the integrity of the semaphores.

        std::unique_lock<std::recursive_mutex> queueCS(m_queueCS); //unique_lock on mutex m_queueCS
		item = move(m_queue.front());
		m_queue.pop();

        m_hEmptySemaphore.signal(false);
        m_emptyCount++;
		return Success;
	}
};

