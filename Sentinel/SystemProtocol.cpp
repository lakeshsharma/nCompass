#include "stdafx.h"
#include "SystemProtocol.h"

// Sentinel includes.
#include "Hardware/WatchDog.h"
#include "Sentinel.h"

using namespace std;

CSystemProtocol::CSystemProtocol(CQueueList &scfToInternalsQueue,
	const std::string &threadName,
	const std::string &messageQueueLogIdentifier,
	DWORD readInternalsTimeout,
	IOperatingSystem &operatingSystem)
	:
m_internalsToSCFQueue(messageQueueLogIdentifier),
m_scfToInternalsQueue(scfToInternalsQueue),
	m_operatingSystem(operatingSystem),
	m_threadName(threadName)
{
	m_pollerSocketReadTimeout = POLLER_SOCKET_READ_TIMEOUT;
	m_readInternalsTimeout = readInternalsTimeout;
	m_timeToClose = false;
}

CSystemProtocol::~CSystemProtocol(void)
{
	Stop();
}

void CSystemProtocol::Init()
{
}

bool CSystemProtocol::IsTimeToClose()
{
	return m_timeToClose;
}

void CSystemProtocol::Start(void)
{
	// Start the incoming queue thread.
	m_internalsThread.reset(new thread(ReadInternalsQueueThread, this));
    CWatchDog::Instance().RegisterThread(m_threadName, m_internalsThread->get_id(), INTERNALS_TO_SCF_QUEUE_TIMEOUT, m_internalsThread->native_handle());
}

void CSystemProtocol::Stop(void)
{
	m_timeToClose = true;
	m_internalsToSCFQueue.GetAddedItemEventHandle().signal(false); // Wake up the thread.
	if (m_internalsThread.get() != nullptr)
	{
		m_internalsThread->join();
		delete m_internalsThread.release(); // Need release to prevent gtest unit tests from hanging or misbehaving if destructor is called during an assert failure.
	}
}

void CSystemProtocol::SendToInternals(CInternalMessage *msg, int priorityLevel)
{
	SendMessageToInternals(msg, priorityLevel);
}

void CSystemProtocol::SendMessageToInternals(CInternalMessage *msg, int priorityLevel)
{
	m_scfToInternalsQueue.Add(msg, priorityLevel);
}

CQueueList *CSystemProtocol::GetInternalsQueue()
{
	return &m_internalsToSCFQueue;
}

void CSystemProtocol::PerformTimedEvents()
{
}

bool CSystemProtocol::HandleIncomingMessage()
{
	bool haveMessages(false);

	auto msg = (CInternalMessage *)m_internalsToSCFQueue.Remove();
	if (msg != nullptr)
	{
		CWatchDog::Instance().Checkin();
		ProcessMessageFromInternals(msg);
		haveMessages = true;
	}

	CWatchDog::Instance().Checkin();
	PerformTimedEvents();

	return haveMessages;
}

void CSystemProtocol::HandleAllIncomingMessages()
{
	bool haveMessages(true);
	while (!m_timeToClose && haveMessages)
	{
		haveMessages = HandleIncomingMessage();
	}
}

void CSystemProtocol::WaitForMessage()
{
	CWatchDog::Instance().Checkin();
	m_operatingSystem.WaitForSingleObject(m_internalsToSCFQueue.GetAddedItemEventHandle(), m_readInternalsTimeout);
}

void CSystemProtocol::ReadInternalsQueue()
{
	while (!m_timeToClose)
	{
		WaitForMessage();
		HandleAllIncomingMessages();
	}
}

UINT AFX_CDECL CSystemProtocol::ReadInternalsQueueThread( LPVOID threadParam )
{
	CSystemProtocol *systemProtocol = (CSystemProtocol *)threadParam;

	CUtilities::SetThreadName(CUtilities::GetCurrentThreadId(), systemProtocol->m_threadName);
	CUtilities::ShowThreadId(systemProtocol->m_threadName);

	try
	{
		systemProtocol->ReadInternalsQueue();
	}
    catch (...) 
    {
        CRestartManager::ReportThreadExceptionAndRequestAReboot(systemProtocol->m_threadName);
    }

	CWatchDog::Instance().DeregisterThread();
	return 0;
}

void CSystemProtocol::LowerTimeoutsForShutdown()
{
	m_pollerSocketReadTimeout = POLLER_CHEKIN_TIME_DURING_SHUTDOWN;
}
