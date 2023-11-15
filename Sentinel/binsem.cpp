#include "stdafx.h"
#include "binsem.h"
#include "Utilities.h"

#ifdef LOGGING
#include "Logging/LogString.h"
#endif

#ifndef _UNIT_TEST
using namespace std;
static std::list<binsem*>* binsemList = nullptr;
static std::recursive_mutex m_criticalSection;
#endif

binsem::binsem(int init_count)
  : m_count(init_count) 
{
    m_signalAsTimeout = false;
#ifndef _UNIT_TEST
    if (binsemList != nullptr) 
    {        
        lock_guard<recursive_mutex> lock(m_criticalSection);
        binsemList->push_back(this);
    }
#endif
}

#ifndef _UNIT_TEST
binsem::~binsem()
{
    if (binsemList != nullptr) 
    {
        lock_guard<recursive_mutex> lock(m_criticalSection);
        binsemList->remove(this);
    }
}
#endif

// P-operation / acquire
void binsem::wait()
{
	std::unique_lock<std::mutex> lk(m_mutex);
	m_conditionVariable.wait(lk, [=]{ return 0 < m_count; });
	--m_count;
}
bool binsem::try_wait()
{
	std::lock_guard<std::mutex> lk(m_mutex);
	if (0 < m_count) 
    {
		--m_count;
		return true;
	} 
    else 
    {
		return false;
	}
}

bool binsem::try_wait_for(uint timeout, bool resetTimeoutSignal)
{
	bool lockGranted = false;
    std::unique_lock<std::mutex> lk(m_mutex);

    if (resetTimeoutSignal)
    {
       m_signalAsTimeout = false;
    }

    // now wait
    if (!m_signalAsTimeout && m_conditionVariable.wait_for(lk, std::chrono::milliseconds(timeout), [=] { return 0 < m_count; }))
    {
       if (!m_signalAsTimeout)
       {
          if (0 < m_count)
           {
              lockGranted = true;
               --m_count;
           }
           else
           {
              // Spurious wakeup? consider it a timeout
              lockGranted = false;
           }
       }
    }

    // timeout
    else
    {
       lockGranted = false;
    }

    m_signalAsTimeout = false;
	
	return lockGranted;
}

// V-operation / release
void binsem::signal(bool signalAsTimeout)
{
	std::lock_guard<std::mutex> lk(m_mutex);
	if (m_count < CountMax) 
    {
        if (signalAsTimeout) 
        {
#ifdef LOGGING
            LogString(GENERAL, "binsem Signal to be called as a timeout");
#endif
        }
        m_signalAsTimeout = signalAsTimeout;
		++m_count;
		m_conditionVariable.notify_one();
	}
}




void binsem::AllocateBinSemList()
{
#ifndef _UNIT_TEST
    binsemList = new std::list<binsem*>;
#endif
}

void binsem::DisposeBinSemList()
{

#ifndef _UNIT_TEST
    if (binsemList != nullptr) 
    {
        delete binsemList;
        binsemList = nullptr;
    }
#endif
}

void binsem::SignalAllBinSems(bool signalAsTimeout)
{

#ifndef _UNIT_TEST
    if (binsemList != nullptr) 
    {

#ifdef LOGGING
        LogString(GENERAL, "Signaling all binsems %s.", signalAsTimeout ? "to be called as a timeout" : "");
#endif

    	for (auto bs : *binsemList) 
    	{
            bs->signal(signalAsTimeout);
    	}
    }
#endif // _UNIT_TEST
}


// Lockable requirements
void binsem::lock() { wait(); }
bool binsem::try_lock() { return try_wait(); }
void binsem::unlock() { signal(false); }

