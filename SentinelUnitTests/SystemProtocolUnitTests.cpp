#include "stdafx.h"
#include "SystemProtocol.h"

// Unit testing includes.
#include "gmock/gmock.h"
#include "Utility/AssertMsg.h"
#include "Utility/UnitTestingMatchers.h"
#include "UnitTestingOperatingSystem.h"

using namespace std;
using ::testing::_;
using ::testing::InSequence;
using ::testing::Invoke;
using ::testing::Mock;

using namespace SentinelNativeUnitTests; // Access matchers in UnitTestingMatchers.h.

	class SystemProtocol : public ::testing::Test
	{
	public:
		static const string m_testThreadName;
		static const string m_testQueueName;

		static string m_gmockFailure;

		class CTersePrinter : public ::testing::EmptyTestEventListener {
			// Called after a failed assertion or a SUCCEED() invocation.
			virtual void OnTestPartResult(const ::testing::TestPartResult& test_part_result) {
				if (test_part_result.failed())
				{
					string failure;
					AssertMsg::FormatOnly("FAILED in %s:%d\n%s",
						test_part_result.file_name(),
						test_part_result.line_number(),
						test_part_result.summary());
					m_gmockFailure = failure;
				}
			}
		};

		static CTersePrinter m_tersePrinter;

		/// <summary>
		/// Initialize and/or test static class members.
		/// </summary>
		SystemProtocol()
		{
			// Class initialization code.
			::testing::UnitTest::GetInstance()->listeners().Append(&m_tersePrinter);
		}

		/// <summary>
		/// Cleanup static class member(s).
		/// </summary>
		~SystemProtocol()
		{
			// Class cleanup code.
			::testing::UnitTest::GetInstance()->listeners().Release(&m_tersePrinter);
		}

		/// <summary>
		/// Initialize class member(s) before each test method is run.
		/// </summary>
		virtual void SetUp()
		{
			// Test method initialization code.
			m_gmockFailure.clear();

		}

		/// <summary>
		/// Cleanup class member(s) before each test method is run.
		/// </summary>
		virtual void TearDown()
		{

			// Report a gmock failure reported by our event listener, if one occurred.
			if (!m_gmockFailure.empty())
			{
				ASSERT_TRUE(false) << m_gmockFailure;
			}

			// Test method cleanup code.
		}
	};

// #pragma region Test and mock classes
		class CProtocol : public CSystemProtocol
		{
		public:
			CProtocol(CQueueList &scfToInternalsQueue)
				: 
				CSystemProtocol(scfToInternalsQueue, SystemProtocol::m_testThreadName, SystemProtocol::m_testQueueName)
			{
			}

			virtual ~CProtocol(void)
			{
			}

			virtual void ProcessMessageFromInternals(CInternalMessage *msg)
			{
				delete msg;
			}

			void SetTimeToClose(bool timeToClose)
			{
				m_timeToClose = timeToClose;
			}
		};

		class CProtocolTest : public CSystemProtocol
		{
		public:
			static const InternalMsgType msgType1 = (InternalMsgType)2525;
			static const DWORD dwordValue1 = 25252525;

			static const int m_performTimedEventLimit = 10;
			int m_performTimedEventQueueSelf; // Queue a message to self when the count matches.
			int m_performTimedEventStdException; // Throw a std::exception when the count matches.
			int m_performTimedEventAccessViolation; // Throw a memory access violation when the count matches.
			int m_performTimedEventCount;

			CProtocolTest(CQueueList &scfToInternalsQueue, IOperatingSystem &operatingSystem)
				:
				CSystemProtocol(scfToInternalsQueue, SystemProtocol::m_testThreadName, SystemProtocol::m_testQueueName, CSystemProtocol::HEARTBEAT_TIME, operatingSystem),
				m_performTimedEventQueueSelf(-1),
				m_performTimedEventStdException(-1),
				m_performTimedEventAccessViolation(-1),
				m_performTimedEventCount(0)
			{
			}

			virtual ~CProtocolTest(void)
			{
			}

			virtual void PerformTimedEvents()
			{
				++m_performTimedEventCount;
				if (m_performTimedEventQueueSelf == m_performTimedEventCount)
				{
					GetInternalsQueue()->Add(new CInternalMessage(msgType1, dwordValue1), QUEUE_PRIORITY_NORMAL);
				}
				if (m_performTimedEventStdException == m_performTimedEventCount)
				{
					throw std::exception();
				}
				if (m_performTimedEventAccessViolation == m_performTimedEventCount)
				{
					*(volatile char *)nullptr = 0; // Try to generate an invalid memory access.
				}
				if (m_performTimedEventLimit == m_performTimedEventCount)
				{
					m_timeToClose = true;
				}
			}

			virtual void ProcessMessageFromInternals(CInternalMessage *msg)
			{
				delete msg;
			}
		};

		class CProtocolMock :
			public CProtocolTest
		{
		public:
			CProtocolMock(CQueueList &scfToInternalsQueue, IOperatingSystem &operatingSystem) :
				CProtocolTest(scfToInternalsQueue, operatingSystem)
			{
			}

			MOCK_METHOD0(PerformTimedEvents, void());
			MOCK_METHOD1(ProcessMessageFromInternals, void(CInternalMessage *msg));

			// Use these to call base (parent) methods.
			void BasePerformTimedEvents() { CProtocolTest::PerformTimedEvents(); }
			void BaseProcessMessageFromInternals(CInternalMessage *msg) { CProtocolTest::ProcessMessageFromInternals(msg); }
		};

		class COperatingSystemMock : public COperatingSystemTest
		{
		public:
			MOCK_METHOD2(WaitForSingleObject, bool(binsem &, DWORD));

			// Use these to call base (parent) methods.
			bool BaseWaitForSingleObject(binsem &hHandle, DWORD dwMilliseconds) { return COperatingSystemTest::WaitForSingleObject(hHandle, dwMilliseconds); }
		};
// #pragma endregion Test and mock classes

// #pragma region CSystemProtocol
		/// <summary>
		/// Functional test for:
		///     CSystemProtocol::CSystemProtocol()
		/// Test for initial values with default operating system.
		/// </summary>
		TEST_F(SystemProtocol, CSystemProtocol_Default)
		{
			// Setup.
			CQueueList scfToInternalsQueue;

			// Perform operation(s) and test(s).
			CProtocol obj(scfToInternalsQueue);
			ASSERT_EQ((void *)&scfToInternalsQueue, (void *)&obj.m_scfToInternalsQueue) << L"scfToInternalsQueue";
			ASSERT_EQ((void *)nullptr, (void *)obj.m_internalsThread.get()) << L"m_internalsThread";
			ASSERT_EQ(SystemProtocol::m_testThreadName, obj.m_threadName) << L"m_threadName";
			ASSERT_EQ((void *)&COperatingSystem::m_operatingSystem, (void *)&obj.m_operatingSystem) << L"m_operatingSystem";
		}

		/// <summary>
		/// Functional test for:
		///     CSystemProtocol::CSystemProtocol()
		/// Test for initial values with non-default operating system.
		/// </summary>
		TEST_F(SystemProtocol, CSystemProtocol_NonDefault)
		{
			// Setup.
			CQueueList scfToInternalsQueue;
			COperatingSystemTest operatingSystem;

			// Perform operation(s) and test(s).
			CProtocolTest obj(scfToInternalsQueue, operatingSystem);
			ASSERT_EQ((void *)&scfToInternalsQueue, (void *)&obj.m_scfToInternalsQueue) << L"scfToInternalsQueue";
			ASSERT_EQ((void *)nullptr, (void *)obj.m_internalsThread.get()) << L"m_internalsThread";
			ASSERT_EQ(SystemProtocol::m_testThreadName, obj.m_threadName) << L"m_threadName";
			ASSERT_EQ((void *)&operatingSystem, (void *)&obj.m_operatingSystem) << L"m_operatingSystem";
		}

		/// <summary>
		/// Code coverage test for:
		///     CSystemProtocol::Init()
		/// Test that Init() has no effect.
		/// </summary>
		TEST_F(SystemProtocol, CSystemProtocol_NonDefaultInit)
		{
			// Setup.
			CQueueList scfToInternalsQueue;
			CProtocol obj(scfToInternalsQueue);

			// Perform operation(s) and test(s).
			obj.Init();
			ASSERT_EQ((void *)&scfToInternalsQueue, (void *)&obj.m_scfToInternalsQueue) << L"scfToInternalsQueue";
			ASSERT_EQ((void *)nullptr, (void *)obj.m_internalsThread.get()) << L"m_internalsThread";
			ASSERT_EQ(SystemProtocol::m_testThreadName, obj.m_threadName) << L"m_threadName";
			ASSERT_EQ((void *)&COperatingSystem::m_operatingSystem, (void *)&obj.m_operatingSystem) << L"m_operatingSystem";
		}

		/// <summary>
		/// Functional test for:
		///     CSystemProtocol::CSystemProtocol()
		///     CSystemProtocol::GetInternalsQueue()
		/// Test return value for GetInternalsQueue().
		/// </summary>
		TEST_F(SystemProtocol, CSystemProtocol_GetInternalsQueue)
		{
			// Setup.
			CQueueList scfToInternalsQueue;

			// Perform operation(s) and test(s).
			CProtocol obj(scfToInternalsQueue);
			ASSERT_NE((void *)nullptr, (void *)obj.GetInternalsQueue()) << L"GetInternalsQueue not null";
			ASSERT_EQ((void *)&obj.m_internalsToSCFQueue, (void *)obj.GetInternalsQueue()) << L"GetInternalsQueue value";
		}

		/// <summary>
		/// Functional test for:
		///     CSystemProtocol::CSystemProtocol()
		///     CSystemProtocol::IsTimeToClose()
		/// Test IsTimeToClose() for default value(s).
		/// </summary>
		TEST_F(SystemProtocol, CSystemProtocol_IsTimeToClose)
		{
			// Setup.
			CQueueList scfToInternalsQueue;

			// Perform operation(s) and test(s).
			CProtocol obj(scfToInternalsQueue);
			ASSERT_FALSE(obj.IsTimeToClose()) << L"IsTimeToClose";
		}
// #pragma endregion CSystemProtocol

		/// <summary>
		/// Functional test for:
		///     CSystemProtocol::IsTimeToClose()
		/// Test for setting value from a derived class.
		/// </summary>
		TEST_F(SystemProtocol, IsTimeToClose)
		{
			// Setup.
			CQueueList scfToInternalsQueue;
			CProtocol obj(scfToInternalsQueue);

			// Perform operation(s) and test(s).
			obj.SetTimeToClose(true);
			ASSERT_EQ(true, obj.IsTimeToClose()) << L"IsTimeToClose";

			obj.SetTimeToClose(false);
			ASSERT_FALSE(obj.IsTimeToClose()) << L"IsTimeToClose";

			obj.SetTimeToClose(true);
			ASSERT_EQ(true, obj.IsTimeToClose()) << L"IsTimeToClose";
		}

		/// <summary>
		/// Functional test for:
		///     CSystemProtocol::SendMessageToInternals()
		/// Test sending an outgoing message (using the public method).
		/// </summary>
		TEST_F(SystemProtocol, SendMessageToInternals)
		{
			// Data.
			const InternalMsgType msgType(INT_MSG_PROGRESSIVE_BONUSING_AVAILABLE);
			const DWORD dwordValue(1234567);

			// Setup.
			CQueueList scfToInternalsQueue;
			CProtocol obj(scfToInternalsQueue);
			ASSERT_EQ((void *)nullptr, (void *)scfToInternalsQueue.Remove()) << L"Remove setup"; // Verify setup.

			CInternalMessage *msg = new CInternalMessage(msgType, dwordValue);

			// Perform operation(s) and test(s).
			obj.SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);
			{
				CInternalMessage *msg(static_cast<CInternalMessage *>(scfToInternalsQueue.Remove()));
				ASSERT_NE((void *)nullptr, (void *)msg) << L"msg";
				ASSERT_EQ(msgType, msg->GetMessageType()) << L"GetMessageType";
				ASSERT_EQ(dwordValue, msg->GetDWordData()) << L"GetDWordData";
			}
			ASSERT_EQ((void *)nullptr, (void *)scfToInternalsQueue.Remove()) << L"Remove final"; // Verify that the queue is now empty.
		}

		/// <summary>
		/// Functional test for:
		///     CSystemProtocol::SendToInternals()
		/// Test sending an outgoing message (using the protected method).
		/// </summary>
		TEST_F(SystemProtocol, SendToInternals)
		{
			// Data.
			const InternalMsgType msgType(INT_MSG_PROGRESSIVE_BONUSING_AVAILABLE);
			const DWORD dwordValue(1234567);

			// Setup.
			CQueueList scfToInternalsQueue;
			CProtocol obj(scfToInternalsQueue);
			ASSERT_EQ((void *)nullptr, (void *)scfToInternalsQueue.Remove()) << L"Remove setup"; // Verify setup.

			CInternalMessage *msg = new CInternalMessage(msgType, dwordValue);

			// Perform operation(s) and test(s).
			obj.SendToInternals(msg, QUEUE_PRIORITY_NORMAL);
			{
				CInternalMessage *msg(static_cast<CInternalMessage *>(scfToInternalsQueue.Remove()));
				ASSERT_NE((void *)nullptr, (void *)msg) << L"msg";
				ASSERT_EQ(msgType, msg->GetMessageType()) << L"GetMessageType";
				ASSERT_EQ(dwordValue, msg->GetDWordData()) << L"GetDWordData";
			}
			ASSERT_EQ((void *)nullptr, (void *)scfToInternalsQueue.Remove()) << L"Remove final"; // Verify that the queue is now empty.
		}

// #pragma region HandleIncomingMessage
		/// <summary>
		/// Code coverage test for:
		///     CSystemProtocol::PerformTimedEvents()
		/// Test calling the HandleIncomingMessage() method when there is no incoming message and
		/// when PerformTimedEvents is not overridden by the derived class.
		/// ProcessMessageFromInternals() should not be called, but CSystemProtocol::PerformTimedEvents() should be called.
		/// </summary>
		TEST_F(SystemProtocol, HandleIncomingMessage_PerformTimedEvents)
		{
			// Setup.
			CQueueList scfToInternalsQueue;
			CProtocol obj(scfToInternalsQueue);
			ASSERT_EQ((void *)nullptr, (void *)(obj.GetInternalsQueue())->Remove()) << L"Remove setup"; // Verify setup.

			InSequence s;

			// Perform operation(s) and test(s).
			obj.HandleIncomingMessage();
			ASSERT_EQ((void *)nullptr, (void *)(obj.GetInternalsQueue())->Remove()) << L"Remove final"; // Verify setup.
		}

		/// <summary>
		/// Functional test for:
		///     CSystemProtocol::HandleIncomingMessage()
		/// Test calling the HandleIncomingMessage() method when there is no incoming message.
		/// ProcessMessageFromInternals() should not be called, but PerformTimedEvents() should be called.
		/// </summary>
		TEST_F(SystemProtocol, HandleIncomingMessage_None)
		{
			// Setup.
			CQueueList scfToInternalsQueue;
			COperatingSystemTest operatingSystem;
			CProtocolMock obj(scfToInternalsQueue, operatingSystem);
			ON_CALL(obj, PerformTimedEvents()).WillByDefault(Invoke(&obj, &CProtocolMock::BasePerformTimedEvents));
			ON_CALL(obj, ProcessMessageFromInternals(_)).WillByDefault(Invoke(&obj, &CProtocolMock::BaseProcessMessageFromInternals));
			ASSERT_EQ((void *)nullptr, (void *)(obj.GetInternalsQueue())->Remove()) << L"Remove setup"; // Verify setup.

			InSequence s;

			// Perform operation(s) and test(s).
			EXPECT_CALL(obj, ProcessMessageFromInternals(_)).Times(0);
			EXPECT_CALL(obj, PerformTimedEvents()).Times(1);
			obj.HandleIncomingMessage();
		}

		/// <summary>
		/// Functional test for:
		///     CSystemProtocol::HandleIncomingMessage()
		/// Test calling the HandleIncomingMessage() method when there is one incoming message.
		/// ProcessMessageFromInternals() should be called and then PerformTimedEvents() should be called.
		/// </summary>
		TEST_F(SystemProtocol, HandleIncomingMessage_One)
		{
			// Data.
			const InternalMsgType msgType1(InternalMsgType(111));
			const DWORD dwordValue1(111111);

			// Setup.
			CQueueList scfToInternalsQueue;
			COperatingSystemTest operatingSystem;
			CProtocolMock obj(scfToInternalsQueue, operatingSystem);
			ON_CALL(obj, PerformTimedEvents()).WillByDefault(Invoke(&obj, &CProtocolMock::BasePerformTimedEvents));
			ON_CALL(obj, ProcessMessageFromInternals(_)).WillByDefault(Invoke(&obj, &CProtocolMock::BaseProcessMessageFromInternals));
			ASSERT_EQ((void *)nullptr, (void *)(obj.GetInternalsQueue())->Remove()) << L"Remove setup"; // Verify setup.

			(obj.GetInternalsQueue())->Add(new CInternalMessage(msgType1, dwordValue1), QUEUE_PRIORITY_NORMAL);

			InSequence s;

			// Perform operation(s) and test(s).
			EXPECT_CALL(obj, ProcessMessageFromInternals(CheckMessage(msgType1, dwordValue1))).Times(1);
			EXPECT_CALL(obj, PerformTimedEvents()).Times(1);
			obj.HandleIncomingMessage();

			ASSERT_EQ((void *)nullptr, (void *)(obj.GetInternalsQueue())->Remove()) << L"Remove final"; // Verify that the queue is now empty.
		}

		/// <summary>
		/// Functional test for:
		///     CSystemProtocol::HandleIncomingMessage()
		/// Test calling the HandleIncomingMessage() method when there are two incoming messages.
		/// ProcessMessageFromInternals() should be called and then PerformTimedEvents() should be called.
		/// Verify expectations then HandleIncomingMessage() again and verify expectations again.
		/// </summary>
		TEST_F(SystemProtocol, HandleIncomingMessage_Two)
		{
			// Data.
			const InternalMsgType msgType1(InternalMsgType(111));
			const DWORD dwordValue1(111111);
			const InternalMsgType msgType2(InternalMsgType(2222));
			const DWORD dwordValue2(22222222);

			// Setup.
			CQueueList scfToInternalsQueue;
			COperatingSystemTest operatingSystem;
			CProtocolMock obj(scfToInternalsQueue, operatingSystem);
			ON_CALL(obj, PerformTimedEvents()).WillByDefault(Invoke(&obj, &CProtocolMock::BasePerformTimedEvents));
			ON_CALL(obj, ProcessMessageFromInternals(_)).WillByDefault(Invoke(&obj, &CProtocolMock::BaseProcessMessageFromInternals));
			ASSERT_EQ((void *)nullptr, (void *)(obj.GetInternalsQueue())->Remove()) << L"Remove setup"; // Verify setup.

			(obj.GetInternalsQueue())->Add(new CInternalMessage(msgType1, dwordValue1), QUEUE_PRIORITY_NORMAL);
			(obj.GetInternalsQueue())->Add(new CInternalMessage(msgType2, dwordValue2), QUEUE_PRIORITY_NORMAL);

			InSequence s;

			// Perform operation(s) and test(s).
			EXPECT_CALL(obj, ProcessMessageFromInternals(CheckMessage(msgType1, dwordValue1))).Times(1);
			EXPECT_CALL(obj, PerformTimedEvents()).Times(1);
			obj.HandleIncomingMessage();
			ASSERT_TRUE(Mock::VerifyAndClearExpectations(&obj));

			EXPECT_CALL(obj, ProcessMessageFromInternals(CheckMessage(msgType2, dwordValue2))).Times(1);
			EXPECT_CALL(obj, PerformTimedEvents()).Times(1);
			obj.HandleIncomingMessage();

			ASSERT_EQ((void *)nullptr, (void *)(obj.GetInternalsQueue())->Remove()) << L"Remove final"; // Verify that the queue is now empty.
		}
// #pragma endregion HandleIncomingMessage

// #pragma region HandleAllIncomingMessages
		/// <summary>
		/// Functional test for:
		///     CSystemProtocol::HandleAllIncomingMessages()
		/// Test calling the HandleAllIncomingMessages() method when there is no incoming message.
		/// ProcessMessageFromInternals() should not be called, but PerformTimedEvents() should be called.
		/// </summary>
		TEST_F(SystemProtocol, HandleAllIncomingMessages_None)
		{
			// Setup.
			CQueueList scfToInternalsQueue;
			COperatingSystemTest operatingSystem;
			CProtocolMock obj(scfToInternalsQueue, operatingSystem);
			ON_CALL(obj, PerformTimedEvents()).WillByDefault(Invoke(&obj, &CProtocolMock::BasePerformTimedEvents));
			ON_CALL(obj, ProcessMessageFromInternals(_)).WillByDefault(Invoke(&obj, &CProtocolMock::BaseProcessMessageFromInternals));
			ASSERT_EQ((void *)nullptr, (void *)(obj.GetInternalsQueue())->Remove()) << L"Remove setup"; // Verify setup.

			InSequence s;

			// Perform operation(s) and test(s).
			EXPECT_CALL(obj, ProcessMessageFromInternals(_)).Times(0);
			EXPECT_CALL(obj, PerformTimedEvents()).Times(1);
			obj.HandleAllIncomingMessages();
		}

		/// <summary>
		/// Functional test for:
		///     CSystemProtocol::HandleAllIncomingMessages()
		/// Test calling the HandleAllIncomingMessages() method when there is one incoming message.
		/// ProcessMessageFromInternals() should be called and then PerformTimedEvents() should be called.
		/// And then PerformTimedEvents() should be called again as it handles the zero messages remaining.
		/// </summary>
		TEST_F(SystemProtocol, HandleAllIncomingMessages_One)
		{
			// Data.
			const InternalMsgType msgType1(InternalMsgType(111));
			const DWORD dwordValue1(111111);

			// Setup.
			CQueueList scfToInternalsQueue;
			COperatingSystemTest operatingSystem;
			CProtocolMock obj(scfToInternalsQueue, operatingSystem);
			ON_CALL(obj, PerformTimedEvents()).WillByDefault(Invoke(&obj, &CProtocolMock::BasePerformTimedEvents));
			ON_CALL(obj, ProcessMessageFromInternals(_)).WillByDefault(Invoke(&obj, &CProtocolMock::BaseProcessMessageFromInternals));
			ASSERT_EQ((void *)nullptr, (void *)(obj.GetInternalsQueue())->Remove()) << L"Remove setup"; // Verify setup.

			(obj.GetInternalsQueue())->Add(new CInternalMessage(msgType1, dwordValue1), QUEUE_PRIORITY_NORMAL);

			InSequence s;

			// Perform operation(s) and test(s).
			EXPECT_CALL(obj, ProcessMessageFromInternals(CheckMessage(msgType1, dwordValue1))).Times(1);
			EXPECT_CALL(obj, PerformTimedEvents()).Times(1);
			EXPECT_CALL(obj, PerformTimedEvents()).Times(1);
			obj.HandleAllIncomingMessages();

			ASSERT_EQ((void *)nullptr, (void *)(obj.GetInternalsQueue())->Remove()) << L"Remove final"; // Verify that the queue is now empty.
		}

		/// <summary>
		/// Functional test for:
		///     CSystemProtocol::HandleAllIncomingMessages()
		/// Test calling the HandleAllIncomingMessages() method when there are two incoming messages.
		/// ProcessMessageFromInternals() should be called and then PerformTimedEvents() should be called for the first message.
		/// Then ProcessMessageFromInternals() should be called and then PerformTimedEvents() should be called for the second message.
		/// And then PerformTimedEvents() should be called again as it handles the zero messages remaining.
		/// </summary>
		TEST_F(SystemProtocol, HandleAllIncomingMessages_Two)
		{
			// Data.
			const InternalMsgType msgType1(InternalMsgType(111));
			const DWORD dwordValue1(111111);
			const InternalMsgType msgType2(InternalMsgType(2222));
			const DWORD dwordValue2(22222222);

			// Setup.
			CQueueList scfToInternalsQueue;
			COperatingSystemTest operatingSystem;
			CProtocolMock obj(scfToInternalsQueue, operatingSystem);
			ON_CALL(obj, PerformTimedEvents()).WillByDefault(Invoke(&obj, &CProtocolMock::BasePerformTimedEvents));
			ON_CALL(obj, ProcessMessageFromInternals(_)).WillByDefault(Invoke(&obj, &CProtocolMock::BaseProcessMessageFromInternals));
			ASSERT_EQ((void *)nullptr, (void *)(obj.GetInternalsQueue())->Remove()) << L"Remove setup"; // Verify setup.

			(obj.GetInternalsQueue())->Add(new CInternalMessage(msgType1, dwordValue1), QUEUE_PRIORITY_NORMAL);
			(obj.GetInternalsQueue())->Add(new CInternalMessage(msgType2, dwordValue2), QUEUE_PRIORITY_NORMAL);

			InSequence s;

			// Perform operation(s) and test(s).
			EXPECT_CALL(obj, ProcessMessageFromInternals(CheckMessage(msgType1, dwordValue1))).Times(1);
			EXPECT_CALL(obj, PerformTimedEvents()).Times(1);
			EXPECT_CALL(obj, ProcessMessageFromInternals(CheckMessage(msgType2, dwordValue2))).Times(1);
			EXPECT_CALL(obj, PerformTimedEvents()).Times(1);
			EXPECT_CALL(obj, PerformTimedEvents()).Times(1);
			obj.HandleAllIncomingMessages();

			ASSERT_EQ((void *)nullptr, (void *)(obj.GetInternalsQueue())->Remove()) << L"Remove final"; // Verify that the queue is now empty.
		}
// #pragma endregion HandleIncomingMessages

// #pragma region WaitForMessage
		/// <summary>
		/// Functional test for:
		///     CSystemProtocol::WaitForMessage()
		/// Test calling the WaitForMessage() method using different timeouts.
		/// </summary>
		TEST_F(SystemProtocol, WaitForMessage)
		{
			// Data.
			const DWORD readIntervalTimeoutDefault(CSystemProtocol::HEARTBEAT_TIME);
			const DWORD readIntervalTimeout2(2 * CSystemProtocol::HEARTBEAT_TIME);
			const DWORD readIntervalTimeout3(3 * CSystemProtocol::HEARTBEAT_TIME);

			const DWORD tickCount0(10000); // Initial.
			const DWORD tickCount1(tickCount0 + readIntervalTimeoutDefault);
			const DWORD tickCount2(tickCount1 + readIntervalTimeout2);
			const DWORD tickCount3(tickCount2 + readIntervalTimeout3);

			// Setup.
			CQueueList scfToInternalsQueue;
			COperatingSystemMock os;
			ON_CALL(os, WaitForSingleObject(_, _)).WillByDefault(Invoke(&os, &COperatingSystemMock::BaseWaitForSingleObject));
			CProtocolTest obj(scfToInternalsQueue, os);

			InSequence s;

			// Perform operation(s) and test(s).
			os.m_tickCount32 = tickCount0;
			EXPECT_CALL(os,
				WaitForSingleObject(MatchRef(&obj.m_internalsToSCFQueue.GetAddedItemEventHandle()), readIntervalTimeoutDefault)).Times(1);
			obj.WaitForMessage();
			ASSERT_EQ(tickCount1, os.m_tickCount32) << L"tickCount1";
			ASSERT_TRUE(Mock::VerifyAndClearExpectations(&obj));

			EXPECT_CALL(os,
				WaitForSingleObject(MatchRef(&obj.m_internalsToSCFQueue.GetAddedItemEventHandle()), readIntervalTimeout2)).Times(1);
			obj.m_readInternalsTimeout = readIntervalTimeout2;
			obj.WaitForMessage();
			ASSERT_EQ(tickCount2, os.m_tickCount32) << L"tickCount2";
			ASSERT_TRUE(Mock::VerifyAndClearExpectations(&obj));

			EXPECT_CALL(os,
				WaitForSingleObject(MatchRef(&obj.m_internalsToSCFQueue.GetAddedItemEventHandle()), readIntervalTimeout3)).Times(1);
			obj.m_readInternalsTimeout = readIntervalTimeout3;
			obj.WaitForMessage();
			ASSERT_EQ(tickCount3, os.m_tickCount32) << L"tickCount3";
		}
// #pragma endregion WaitForMessage

// #pragma region ReadInternalsQueue
		/// <summary>
		/// Functional test for:
		///     CSystemProtocol::ReadInternalsQueue()
		/// Test calling the ReadInternalsQueue() method when there is no incoming message.
		/// ProcessMessageFromInternals() should not be called, but PerformTimedEvents() should be called.
		/// After 10 calls to PerformTimedEvents(), m_timeToClose is set true by the derived instance.
		/// </summary>
		TEST_F(SystemProtocol, ReadInternalsQueue_None)
		{
			// Data.
			const DWORD readIntervalTimeout(1000);
			const DWORD tickCount0(10000); // Initial.
			const DWORD tickCount1(tickCount0 + readIntervalTimeout * CProtocolMock::m_performTimedEventLimit); // Final.

			// Setup.
			CQueueList scfToInternalsQueue;
			COperatingSystemMock os;
			EXPECT_CALL(os, WaitForSingleObject(_, _)).WillRepeatedly(Invoke(&os, &COperatingSystemMock::BaseWaitForSingleObject));

			CProtocolMock obj(scfToInternalsQueue, os);
			ON_CALL(obj, PerformTimedEvents()).WillByDefault(Invoke(&obj, &CProtocolMock::BasePerformTimedEvents));
			ON_CALL(obj, ProcessMessageFromInternals(_)).WillByDefault(Invoke(&obj, &CProtocolMock::BaseProcessMessageFromInternals));
			obj.m_readInternalsTimeout = readIntervalTimeout;

			InSequence s;

			// Perform operation(s) and test(s).
			os.m_tickCount32 = tickCount0;
			EXPECT_CALL(obj, ProcessMessageFromInternals(_)).Times(0);
			EXPECT_CALL(obj, PerformTimedEvents()).Times(CProtocolMock::m_performTimedEventLimit);
			obj.ReadInternalsQueue();
			ASSERT_EQ(tickCount1, os.m_tickCount32) << L"tickCount1";
		}

		/// <summary>
		/// Functional test for:
		///     CSystemProtocol::ReadInternalsQueue()
		/// Test calling the ReadInternalsQueue() method when there is one incoming message.
		/// ProcessMessageFromInternals() should be called and then PerformTimedEvents() should be called.
		/// And then PerformTimedEvents() should be called again as it handles the zero messages remaining.
		/// After a total of 10 calls to PerformTimedEvents(), m_timeToClose is set true by the derived instance.
		/// </summary>
		TEST_F(SystemProtocol, ReadInternalsQueue_One)
		{
			// Data.
			const InternalMsgType msgType1(InternalMsgType(111));
			const DWORD dwordValue1(111111);

			const DWORD readIntervalTimeout(1000);
			const DWORD tickCount0(10000); // Initial.
			const DWORD tickCount1(tickCount0 + readIntervalTimeout * (CProtocolMock::m_performTimedEventLimit - 2)); // Final.

			// Setup.
			CQueueList scfToInternalsQueue;
			COperatingSystemMock os;
			EXPECT_CALL(os, WaitForSingleObject(_, _)).WillRepeatedly(Invoke(&os, &COperatingSystemMock::BaseWaitForSingleObject));

			CProtocolMock obj(scfToInternalsQueue, os);
			ON_CALL(obj, PerformTimedEvents()).WillByDefault(Invoke(&obj, &CProtocolMock::BasePerformTimedEvents));
			ON_CALL(obj, ProcessMessageFromInternals(_)).WillByDefault(Invoke(&obj, &CProtocolMock::BaseProcessMessageFromInternals));
			obj.m_readInternalsTimeout = readIntervalTimeout;

			(obj.GetInternalsQueue())->Add(new CInternalMessage(msgType1, dwordValue1), QUEUE_PRIORITY_NORMAL);

			InSequence s;

			// Perform operation(s) and test(s).
			os.m_tickCount32 = tickCount0;
			EXPECT_CALL(obj, ProcessMessageFromInternals(CheckMessage(msgType1, dwordValue1))).Times(1);
			EXPECT_CALL(obj, PerformTimedEvents()).Times(CProtocolMock::m_performTimedEventLimit);
			obj.ReadInternalsQueue();
			ASSERT_EQ(tickCount1, os.m_tickCount32) << tickCount1 + "," + os.m_tickCount32;
		}

		/// <summary>
		/// Functional test for:
		///     CSystemProtocol::ReadInternalsQueue()
		/// Test calling the HandleAllIncomingMessages() method when there are two incoming messages.
		/// ProcessMessageFromInternals() should be called and then PerformTimedEvents() should be called for the first message.
		/// Then ProcessMessageFromInternals() should be called and then PerformTimedEvents() should be called for the second message.
		/// And then PerformTimedEvents() should be called again as it handles the zero messages remaining.
		/// After a total of 10 calls to PerformTimedEvents(), m_timeToClose is set true by the derived instance.
		/// </summary>
		TEST_F(SystemProtocol, ReadInternalsQueue_Two)
		{
			// Data.
			const InternalMsgType msgType1(InternalMsgType(111));
			const DWORD dwordValue1(111111);
			const InternalMsgType msgType2(InternalMsgType(2222));
			const DWORD dwordValue2(22222222);

			const DWORD readIntervalTimeout(1000);
			const DWORD tickCount0(10000); // Initial.
			const DWORD tickCount1(tickCount0 + readIntervalTimeout * (CProtocolMock::m_performTimedEventLimit - 3)); // Final.

			// Setup.
			CQueueList scfToInternalsQueue;
			COperatingSystemMock os;
			EXPECT_CALL(os, WaitForSingleObject(_, _)).WillRepeatedly(Invoke(&os, &COperatingSystemMock::BaseWaitForSingleObject));

			CProtocolMock obj(scfToInternalsQueue, os);
			ON_CALL(obj, PerformTimedEvents()).WillByDefault(Invoke(&obj, &CProtocolMock::BasePerformTimedEvents));
			ON_CALL(obj, ProcessMessageFromInternals(_)).WillByDefault(Invoke(&obj, &CProtocolMock::BaseProcessMessageFromInternals));
			obj.m_readInternalsTimeout = readIntervalTimeout;

			(obj.GetInternalsQueue())->Add(new CInternalMessage(msgType1, dwordValue1), QUEUE_PRIORITY_NORMAL);
			(obj.GetInternalsQueue())->Add(new CInternalMessage(msgType2, dwordValue2), QUEUE_PRIORITY_NORMAL);

			InSequence s;

			// Perform operation(s) and test(s).
			os.m_tickCount32 = tickCount0;
			EXPECT_CALL(obj, ProcessMessageFromInternals(CheckMessage(msgType1, dwordValue1))).Times(1);
			EXPECT_CALL(obj, PerformTimedEvents()).Times(1);
			EXPECT_CALL(obj, ProcessMessageFromInternals(CheckMessage(msgType2, dwordValue2))).Times(1);
			EXPECT_CALL(obj, PerformTimedEvents()).Times(CProtocolMock::m_performTimedEventLimit - 1);
			obj.ReadInternalsQueue();
			ASSERT_EQ(tickCount1, os.m_tickCount32) << L"tickCount1";
		}

		/// <summary>
		/// Functional test for:
		///     CSystemProtocol::ReadInternalsQueue()
		/// Test calling the ReadInternalsQueue() method when there is no incoming message until PerformTimedEvents() has been called 5 times.
		/// Then ProcessMessageFromInternals() should be called and then PerformTimedEvents() should be called.
		/// And then PerformTimedEvents() should be called again as it handles the zero messages remaining.
		/// After a total of 10 calls to PerformTimedEvents(), m_timeToClose is set true by the derived instance.
		/// </summary>
		TEST_F(SystemProtocol, ReadInternalsQueue_QueueToSelf)
		{
			// Data.
			const DWORD readIntervalTimeout(1000);
			const DWORD tickCount0(10000); // Initial.
			const DWORD tickCount1(tickCount0 + readIntervalTimeout * (CProtocolMock::m_performTimedEventLimit - 2)); // Final.

			// Setup.
			CQueueList scfToInternalsQueue;
			COperatingSystemMock os;
			EXPECT_CALL(os, WaitForSingleObject(_, _)).WillRepeatedly(Invoke(&os, &COperatingSystemMock::BaseWaitForSingleObject));

			CProtocolMock obj(scfToInternalsQueue, os);
			ON_CALL(obj, PerformTimedEvents()).WillByDefault(Invoke(&obj, &CProtocolMock::BasePerformTimedEvents));
			ON_CALL(obj, ProcessMessageFromInternals(_)).WillByDefault(Invoke(&obj, &CProtocolMock::BaseProcessMessageFromInternals));
			obj.m_readInternalsTimeout = readIntervalTimeout;
			obj.m_performTimedEventQueueSelf = CProtocolMock::m_performTimedEventLimit / 2;

			InSequence s;

			// Perform operation(s) and test(s).
			os.m_tickCount32 = tickCount0;
			EXPECT_CALL(obj, PerformTimedEvents()).Times(CProtocolMock::m_performTimedEventLimit / 2);
			EXPECT_CALL(obj, ProcessMessageFromInternals(CheckMessage(CProtocolTest::msgType1, CProtocolTest::dwordValue1))).Times(1);
			EXPECT_CALL(obj, PerformTimedEvents()).Times((CProtocolMock::m_performTimedEventLimit + 1) / 2);
			obj.ReadInternalsQueue();
			ASSERT_EQ(tickCount1, os.m_tickCount32) << L"tickCount1";
		}
// #pragma endregion ReadInternalsQueue

// #pragma region ReadInternalsQueueThread
		/// <summary>
		/// Functional test for:
		///     CSystemProtocol::ReadInternalsQueueThread()
		/// Test calling the ReadInternalsQueueThread() method when there is no incoming message until PerformTimedEvents() has been called 5 times.
		/// Then ProcessMessageFromInternals() should be called and then PerformTimedEvents() should be called.
		/// And then PerformTimedEvents() should be called again as it handles the zero messages remaining.
		/// After a total of 10 calls to PerformTimedEvents(), m_timeToClose is set true by the derived instance.
		/// </summary>
		TEST_F(SystemProtocol, ReadInternalsQueueThread_QueueToSelf)
		{
			// Data.
			const DWORD readIntervalTimeout(1000);
			const DWORD tickCount0(10000); // Initial.
			const DWORD tickCount1(tickCount0 + readIntervalTimeout * (CProtocolMock::m_performTimedEventLimit - 2)); // Final.

			// Setup.
			CQueueList scfToInternalsQueue;
			COperatingSystemMock os;
			EXPECT_CALL(os, WaitForSingleObject(_, _)).WillRepeatedly(Invoke(&os, &COperatingSystemMock::BaseWaitForSingleObject));

			CProtocolMock obj(scfToInternalsQueue, os);
			ON_CALL(obj, PerformTimedEvents()).WillByDefault(Invoke(&obj, &CProtocolMock::BasePerformTimedEvents));
			ON_CALL(obj, ProcessMessageFromInternals(_)).WillByDefault(Invoke(&obj, &CProtocolMock::BaseProcessMessageFromInternals));
			obj.m_readInternalsTimeout = readIntervalTimeout;
			obj.m_performTimedEventQueueSelf = CProtocolMock::m_performTimedEventLimit / 2;

			InSequence s;

			// Perform operation(s) and test(s).
			os.m_tickCount32 = tickCount0;
			EXPECT_CALL(obj, PerformTimedEvents()).Times(CProtocolMock::m_performTimedEventLimit / 2);
			EXPECT_CALL(obj, ProcessMessageFromInternals(CheckMessage(CProtocolTest::msgType1, CProtocolTest::dwordValue1))).Times(1);
			EXPECT_CALL(obj, PerformTimedEvents()).Times((CProtocolMock::m_performTimedEventLimit + 1) / 2);
			CProtocolMock::ReadInternalsQueueThread(&obj);
			ASSERT_EQ(tickCount1, os.m_tickCount32) << L"tickCount1";
		}

#if 000//TODO-PORT This test fails unless the exception is caught and handled.
		/// <summary>
		/// Functional test for:
		///     CSystemProtocol::ReadInternalsQueueThread()
		/// Test calling the ReadInternalsQueueThread() method when there is no incoming message..
		/// After a total of 3 calls to PerformTimedEvents(), an exception is thrown.
		/// </summary>
		TEST_F(SystemProtocol, ReadInternalsQueueThread_StdException)
		{
			// Data.
			const int performTimedEventCount = 3; // Throw an exception after this many calls to PerformTimedEvents().
			const DWORD readIntervalTimeout(1000);
			const DWORD tickCount0(10000); // Initial.
			const DWORD tickCount1(tickCount0 + readIntervalTimeout * performTimedEventCount); // Final.

			// Setup.
			CQueueList scfToInternalsQueue(true);
			COperatingSystemMock os;
			EXPECT_CALL(os, WaitForSingleObject(_, _)).WillRepeatedly(Invoke(&os, &COperatingSystemMock::BaseWaitForSingleObject));

			CProtocolMock obj(scfToInternalsQueue, os);
			ON_CALL(obj, PerformTimedEvents()).WillByDefault(Invoke(&obj, &CProtocolMock::BasePerformTimedEvents));
			ON_CALL(obj, ProcessMessageFromInternals(_)).WillByDefault(Invoke(&obj, &CProtocolMock::BaseProcessMessageFromInternals));
			obj.m_readInternalsTimeout = readIntervalTimeout;
			obj.m_performTimedEventStdException = performTimedEventCount;

			InSequence s;

			// Perform operation(s) and test(s).
			os.m_tickCount32 = tickCount0;
			EXPECT_CALL(obj, ProcessMessageFromInternals(_)).Times(0);
			EXPECT_CALL(obj, PerformTimedEvents()).Times(performTimedEventCount);
			CProtocolMock::ReadInternalsQueueThread(&obj);
			ASSERT_EQ(tickCount1, os.m_tickCount32) << L"tickCount1";
		}
#endif//TODO-PORT

#if 000//TODO-PORT This test hangs gtest unless the access violation is caught and handled.
		/// <summary>
		/// Functional test for:
		///     CSystemProtocol::ReadInternalsQueueThread()
		/// Test calling the ReadInternalsQueueThread() method when there is no incoming message..
		/// After a total of 3 calls to PerformTimedEvents(), an exception is thrown.
		/// </summary>
		TEST_F(SystemProtocol, ReadInternalsQueueThread_AccessViolation)
		{
			// Data.
			const int performTimedEventCount = 3; // Throw an exception after this many calls to PerformTimedEvents().
			const DWORD readIntervalTimeout(1000);
			const DWORD tickCount0(10000); // Initial.
			const DWORD tickCount1(tickCount0 + readIntervalTimeout * performTimedEventCount); // Final.

			// Setup.
			CQueueList scfToInternalsQueue(true);
			COperatingSystemMock os;
			EXPECT_CALL(os, WaitForSingleObject(_, _)).WillRepeatedly(Invoke(&os, &COperatingSystemMock::BaseWaitForSingleObject));

			CProtocolMock obj(scfToInternalsQueue, os);
			ON_CALL(obj, PerformTimedEvents()).WillByDefault(Invoke(&obj, &CProtocolMock::BasePerformTimedEvents));
			ON_CALL(obj, ProcessMessageFromInternals(_)).WillByDefault(Invoke(&obj, &CProtocolMock::BaseProcessMessageFromInternals));
			obj.m_readInternalsTimeout = readIntervalTimeout;
			obj.m_performTimedEventAccessViolation = performTimedEventCount;

			InSequence s;

			// Perform operation(s) and test(s).
			os.m_tickCount32 = tickCount0;
			EXPECT_CALL(obj, ProcessMessageFromInternals(_)).Times(0);
			EXPECT_CALL(obj, PerformTimedEvents()).Times(performTimedEventCount);
			CProtocolMock::ReadInternalsQueueThread(&obj);
			ASSERT_EQ(tickCount1, os.m_tickCount32) << L"tickCount1";
		}
#endif//TODO-PORT
// #pragma endregion ReadInternalsQueueThread

// #pragma region Start
		/// <summary>
		/// Functional test for:
		///     CSystemProtocol::Start()
		///     CSystemProtocol::IsTimeToClose()
		///     CSystemProtocol::~CSystemProtocol()
		/// Test starting and stopping the instance's thread.
		/// </summary>
		TEST_F(SystemProtocol, Start)
		{
			{
				// Setup.
				CQueueList scfToInternalsQueue;
				CProtocol obj(scfToInternalsQueue);
				ASSERT_EQ((void *)nullptr, (void *)obj.m_internalsThread.get()) << L"m_internalsThread"; // Verify setup.

				// Perform operation(s) and test(s).
				obj.Start();
				ASSERT_NE((void *)nullptr, (void *)obj.m_internalsThread.get()) << L"m_internalsThread";
				ASSERT_FALSE(obj.IsTimeToClose()) << L"IsTimeToClose";
			}
		}

// #pragma endregion Start
		/// <summary>
		/// Functional test for:
		///     CSystemProtocol::Start()
		/// Test calling the Start() method when there is no incoming message until PerformTimedEvents() has been called 5 times.
		/// Then ProcessMessageFromInternals() should be called and then PerformTimedEvents() should be called.
		/// And then PerformTimedEvents() should be called again as it handles the zero messages remaining.
		/// After a total of 10 calls to PerformTimedEvents(), m_timeToClose is set true by the derived instance.
		/// 2016-10-27 std::thread does not support arbitrary testing for thread completion -- so sleep awhile and then check results.
		/// </summary>
		TEST_F(SystemProtocol, Start_QueueToSelf)
		{
			// Data.
			const DWORD readIntervalTimeout(1000);
			const DWORD tickCount0(10000); // Initial.
			const DWORD tickCount1(tickCount0 + readIntervalTimeout * (CProtocolMock::m_performTimedEventLimit - 2)); // Final.

			// Setup.
			CQueueList scfToInternalsQueue;
			COperatingSystemMock os;
			EXPECT_CALL(os, WaitForSingleObject(_, _)).WillRepeatedly(Invoke(&os, &COperatingSystemMock::BaseWaitForSingleObject));

			CProtocolMock obj(scfToInternalsQueue, os);
			ON_CALL(obj, PerformTimedEvents()).WillByDefault(Invoke(&obj, &CProtocolMock::BasePerformTimedEvents));
			ON_CALL(obj, ProcessMessageFromInternals(_)).WillByDefault(Invoke(&obj, &CProtocolMock::BaseProcessMessageFromInternals));
			obj.m_readInternalsTimeout = readIntervalTimeout;
			obj.m_performTimedEventQueueSelf = CProtocolMock::m_performTimedEventLimit / 2;

			InSequence s;

			// Perform operation(s) and test(s).
			os.m_tickCount32 = tickCount0;
#if 000//TODO-PORT This unit test takes 5+ seconds to run. Need to refactor, add at end of project, and/or run conditionally.
			EXPECT_CALL(obj, PerformTimedEvents()).Times(CProtocolMock::m_performTimedEventLimit / 2);
			EXPECT_CALL(obj, ProcessMessageFromInternals(CheckMessage(CProtocolTest::msgType1, CProtocolTest::dwordValue1))).Times(1);
			EXPECT_CALL(obj, PerformTimedEvents()).Times((CProtocolMock::m_performTimedEventLimit + 1) / 2);

			obj.Start();
			CUtilities::Sleep(5000); // 2015-08-20 Typical run-time is about 50-200 milliseconds.
			ASSERT_EQ(true, obj.IsTimeToClose()) << "IsTimeToClose";

			ASSERT_EQ(tickCount1, os.m_tickCount32) << L"tickCount1";
#endif//TODO-PORT
		}

	const string SystemProtocol::SystemProtocol::m_testThreadName(_T("TestThread"));
	const string SystemProtocol::SystemProtocol::m_testQueueName(_T("TestQueue"));

	string SystemProtocol::m_gmockFailure;
	SystemProtocol::CTersePrinter SystemProtocol::m_tersePrinter;
