#pragma once

#include "stdafx.h"
#include "TimerThread.h"
#include "NVRAMTester.h"

using namespace std;

class DelayExecution
{
public:
    template <class callable, class... arguments>
    DelayExecution(int after, bool async, callable&& f, arguments&&... args)
    {
        std::function<typename std::result_of<callable(arguments...)>::type()> task(std::bind(std::forward<callable>(f), std::forward<arguments>(args)...));

        if (async)
        {
            std::thread([after, task]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(after));
                task();
            }).detach();
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(after));
            task();
        }
    }
};


class TimerThread
{
public:
	TimerThread();
	~TimerThread();
	void SetTimer(int timeVal);
	void StopTimer();
	void setThread(void (*func)(void));

private:
	// Private methods
	void mainThread();

	// Member variables
	thread *pt1;
	thread *pt2;
	int m_waitUntil;
	void (*m_func)(void);
	bool m_running;
};
