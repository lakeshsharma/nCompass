
#include "TimerThread.h"

#ifndef INFINITE
#define INFINITE            0xFFFFFFFF  // Infinite timeout
#endif
#define MAX_TIME			60000		//60 seconds.

static int m_retryCount;

TimerThread::TimerThread()
{
};

TimerThread::~TimerThread()
{
};

void TimerThread::mainThread()
{
	try
	{
		if (m_waitUntil) 
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(m_waitUntil));
		}
		m_func();
	}
	catch (exception &ex) 
	{
		char x_buffer[1024];
		snprintf(x_buffer, sizeof(x_buffer), "ERROR: (mainThread) unexpected thread exception: %s\n", ex.what());
		prtf((char *)"%s\n", (char *)"\r", x_buffer);
	}
	m_running = false;
}

void TimerThread::setThread(void (*func)(void))
{
	m_func=func;
	pt1 = NULL;
}

void TimerThread::SetTimer(int timeVal)
{
	try
	{
		m_retryCount = 0;
		m_waitUntil = timeVal;
		m_running = true;
		if (pt1 != NULL) 
		{
			pt1->detach();
			delete pt1;
			pt1 = NULL;
		}
		pt1 = new std::thread([this] {this->mainThread(); });
	}
	catch (exception &ex) 
	{
		char x_buffer[1024];
		snprintf(x_buffer, sizeof(x_buffer), "ERROR: (setTimer) unexpected thread exception: %s\n", ex.what());
		prtf((char *)"%s\n", (char *)"\r", x_buffer);
	}
}

void TimerThread::StopTimer()
{
	try
	{
		if (m_running)
		{
			m_running = false;
			pt1->detach();
			delete pt1;
			pt1 = NULL;
		}
	}
	catch (exception &ex) 
	{
		char x_buffer[1024];
		snprintf(x_buffer, sizeof(x_buffer), "ERROR: (stopTimer) unexpected thread exception: %s\n", ex.what());
		prtf((char *)"%s\n", (char *)"\r", x_buffer);
	}
}





