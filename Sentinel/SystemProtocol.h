#pragma once

#include "QueueList.h"
#include "InternalMessage.h"
#include "ProcessDump/OperatingSystem.h"

// Declare friend class(es) for unit test access.
namespace SentinelNativeUnitTests { class ProgressiveProtocol; }
namespace SentinelUnitTests { class SASProtocol; }

/// <summary>
/// Abstract class for handling incoming (from Internals) and outgoing (to Internals) messages.
/// </summary>
class CSystemProtocol
{
public:
	static const DWORD HEARTBEAT_TIME = 5000;	// heartbeat time in milliseconds
									// This timeout also affects the watchdog checkin
	static const DWORD POLLER_SOCKET_READ_TIMEOUT = HEARTBEAT_TIME * 3;
    static const DWORD POLLER_CHEKIN_TIME_DURING_SHUTDOWN = 1000;	

	/// <summary>
	/// Standard constructor. Initializes a new instance of the <see cref="CSystemProtocol"/> class.
	/// </summary>
	/// <param name="scfToInternalsQueue">IN/OUT - The outgoing (to internals) queue.</param>
	/// <param name="threadName">IN - The name for the thread that reads the incoming queue.</param>
	/// <param name="readInternalsTimeout">IN - Period of time to wait for a message from internals.</param>
	/// <param name="operatingSystem">IN/OUT - The operating system.</param>
	/// <remarks>
	/// If no message is received within readInternalsTimeout then PeformTimedEvents() is called.
	/// </remarks>
	CSystemProtocol(CQueueList &scfToInternalsQueue,
		const std::string &threadName,
		const std::string &messageQueueLogIdentifier,
		DWORD readInternalsTimeout = HEARTBEAT_TIME,
		IOperatingSystem &operatingSystem = COperatingSystem::m_operatingSystem);

	/// <summary>
	/// Destructor. Finalizes an instance of the <see cref=""/> class.
	/// </summary>
	virtual ~CSystemProtocol(void);

	/// <summary>
	/// Gets a pointer to the queue for incoming messages from Internals.
	/// </summary>
	/// <returns>A pointer to the queue for incoming messages from Internals. Does not return nullptr.</returns>
	CQueueList * GetInternalsQueue(void);

	/// <summary>
	/// Initializer.  Intended to be called immediately after the constructor.
	/// </summary>
	/// <remarks>
	/// It is intended that the constructor be kept lightweight and that this method be used for heavyweight intialization.
	/// This allows a test/mock enviroment to be updated after the constructor completes in order to test initialization
	/// at a more granular level.
	/// </remarks>
	virtual void Init();

	/// <summary>
	/// Starts the thread for this instance.
	/// </summary>
	/// <remarks>
	/// Wait until derived constructors and initializers complete before starting this instance.
	/// </remarks>
	virtual void Start();

	/// <summary>
	/// Called to perform timed event processing.
	/// </summary>
	/// <remarks>
	/// Called after each message processed and when there are no messages to process.
	/// Default method does nothing.
	/// </remarks>
	virtual void PerformTimedEvents();

	/// <summary>
	/// Called to process each message received from Internals.
	/// </summary>
	virtual void ProcessMessageFromInternals( CInternalMessage *msg ) = 0;

	/// <summary>
	/// Stops the thread if the thread is running and returns when the thread is stopped.
	/// </summary>
	void Stop();

	/// <summary>
	/// Returns true if it is time to stop processing (and destruct the instance), returns false otherwise.
	/// </summary>
	/// <returns>true if it is time to stop processing (and destruct the instance), false otherwise.</returns>
	bool IsTimeToClose();

	/// <summary>
	/// Sends the message to internals.
	/// </summary>
	/// <param name="msg">The message.</param>
	/// <param name="priorityLevel">IN - The priority level.</param>
	/// <remarks>
	/// The caller MUST NOT access msg after calling this routine.  The receiver is responsible for deleting the message.
	/// </remarks>
	void SendMessageToInternals( CInternalMessage *msg, int priorityLevel );

	virtual void LowerTimeoutsForShutdown();
protected:


	// The queue for messages coming from Internals
	CQueueList m_internalsToSCFQueue;

	// The external queue for messages going to the internals
	CQueueList &m_scfToInternalsQueue;

	// The timeout in miliseconds in the internals queue read thread;
	DWORD m_readInternalsTimeout;
	bool m_timeToClose;
	std::unique_ptr<std::thread> m_internalsThread;

	IOperatingSystem &m_operatingSystem;

	DWORD m_pollerSocketReadTimeout;

	void SendToInternals( CInternalMessage *msg, int priorityLevel );

private:
	const std::string m_threadName;

	bool HandleIncomingMessage(); // Handle one queued message and perform timed events.
	void HandleAllIncomingMessages(); // Return if m_timeToClose, else handle all queued messages and perform timed events.
	void WaitForMessage(); // Wait up to m_readIncomingTimeout for an incoming event or message.
	void ReadInternalsQueue(); // Return if m_timeToClose, else wait for a message, handle messages, and perform timed events.
	static UINT AFX_CDECL ReadInternalsQueueThread(LPVOID threadParam);

	// Grant "friend" access for testing.
    friend class SentinelNativeUnitTests::ProgressiveProtocol;
	friend class SentinelUnitTests::SASProtocol;
	FRIEND_TEST(SystemProtocol, CSystemProtocol_Default);
	FRIEND_TEST(SystemProtocol, CSystemProtocol_NonDefault);
	FRIEND_TEST(SystemProtocol, CSystemProtocol_NonDefaultInit);
	FRIEND_TEST(SystemProtocol, WaitForMessage);
	FRIEND_TEST(SystemProtocol, CSystemProtocol_GetInternalsQueue);
	FRIEND_TEST(SystemProtocol, SendToInternals);
	FRIEND_TEST(SystemProtocol, HandleIncomingMessage_PerformTimedEvents);
	FRIEND_TEST(SystemProtocol, HandleIncomingMessage_None);
	FRIEND_TEST(SystemProtocol, HandleIncomingMessage_One);
	FRIEND_TEST(SystemProtocol, HandleIncomingMessage_Two);
	FRIEND_TEST(SystemProtocol, HandleAllIncomingMessages_None);
	FRIEND_TEST(SystemProtocol, HandleAllIncomingMessages_One);
	FRIEND_TEST(SystemProtocol, HandleAllIncomingMessages_Two);
	FRIEND_TEST(SystemProtocol, ReadInternalsQueue_None);
	FRIEND_TEST(SystemProtocol, ReadInternalsQueue_One);
	FRIEND_TEST(SystemProtocol, ReadInternalsQueue_Two);
	FRIEND_TEST(SystemProtocol, ReadInternalsQueue_QueueToSelf);
	FRIEND_TEST(SystemProtocol, ReadInternalsQueueThread_QueueToSelf);
	FRIEND_TEST(SystemProtocol, ReadInternalsQueueThread_StdException);
	FRIEND_TEST(SystemProtocol, ReadInternalsQueueThread_AccessViolation);
	FRIEND_TEST(SystemProtocol, Start);
	FRIEND_TEST(SystemProtocol, Start_QueueToSelf);
};
