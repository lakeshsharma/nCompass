#include "stdafx.h"
#include "ProgressiveProtocol/ProgressiveProtocol.h"

// Sentinel includes.
#include "InternalMessage.h"

// Unit testing includes.
#include "UnitTestingOperatingSystem.h"


#include "gtest/gtest.h"


namespace SentinelNativeUnitTests
{
    class ProgressiveProtocol : public ::testing::Test {
		protected:
            CQueueList scfToInternalsQueue;
            CProgressiveProtocol obj;

            ProgressiveProtocol():scfToInternalsQueue(""), obj(scfToInternalsQueue){}
            
			virtual void SetUp() {
				CDirectoryManager::MockInstance(NULL);
			}

			virtual void TearDown() 
			{
				CDirectoryManager::MockInstanceCleanup();
			}

		static const string m_testThreadName;

		// Remove all messages and assert that they are the correct default message(s).
		void AssertRemoveDefaultMessages(CQueueList &scfToInternalsQueue)
		{
			// Check first message.
			CQueueItem *item1 = scfToInternalsQueue.Remove();
			ASSERT_TRUE(item1 != NULL) << "item1";
			CInternalMessage *msg1 = dynamic_cast<CInternalMessage *>(item1);
			ASSERT_TRUE(msg1 != NULL) << "msg1";
			ASSERT_EQ(INT_MSG_SET_PROGRESSIVE_CONFIG, msg1->GetMessageType()) << "msg1->GetMessageType()";

			// Verify that there is no second message.
			ASSERT_TRUE(scfToInternalsQueue.Remove() == NULL) << "item2";
		}

        void AssertProtocolDefault()
		{
			ASSERT_EQ((void *)&scfToInternalsQueue, (void *)&obj.m_scfToInternalsQueue) << "scfToInternalsQueue";
			ASSERT_TRUE(obj.m_internalsThread == NULL) << "m_internalsThread";
			//ASSERT_EQ(m_testThreadName, obj.m_threadName) << "m_threadName";
			ASSERT_EQ((void *)&COperatingSystem::m_operatingSystem, (void *)&obj.m_operatingSystem) << "m_operatingSystem";
			ASSERT_TRUE(scfToInternalsQueue.Remove() == NULL) << "Remove final"; // Verify that the queue is now empty.

			ASSERT_FALSE(obj.m_configFileActive) << "m_configFileActive";
			ASSERT_FALSE(obj.m_oneLinkProgressivesSenOptionEnabled) << "m_oneLinkProgressivesSenOptionEnabled";
			ASSERT_FALSE(obj.m_oneLinkProgressivesSupportedByEGM) << "m_oneLinkProgressivesSupportedByEGM";
		}

        void ProgressiveProtocolNonDefault()
		{
			// Setup.
			COperatingSystemTest operatingSystem;

			// Perform operation(s) and test(s).
			CProgressiveProtocol obj(scfToInternalsQueue, "1.1.2", operatingSystem);
			ASSERT_EQ((void *)&scfToInternalsQueue, (void *)&obj.m_scfToInternalsQueue) << "scfToInternalsQueue";
			ASSERT_TRUE(obj.m_internalsThread == NULL) << "m_internalsThread";
			ASSERT_EQ(m_testThreadName, obj.m_threadName) << "m_threadName";
			ASSERT_EQ((void *)&operatingSystem, (void *)&obj.m_operatingSystem) << "m_operatingSystem";

            ASSERT_FALSE(obj.m_configFileActive) << "m_configFileActive";
            ASSERT_FALSE(obj.m_oneLinkProgressivesSenOptionEnabled) << "m_oneLinkProgressivesSenOptionEnabled";
            ASSERT_FALSE(obj.m_oneLinkProgressivesSupportedByEGM) << "m_oneLinkProgressivesSupportedByEGM";
		}

        void ProgressiveProtocolInit()
        {
            ASSERT_EQ((void *)&scfToInternalsQueue, (void *)&obj.m_scfToInternalsQueue) << "scfToInternalsQueue";
            ASSERT_TRUE(obj.m_internalsThread == NULL) << "m_internalsThread";
            ASSERT_EQ(m_testThreadName, obj.m_threadName) << "m_threadName";
            ASSERT_EQ((void *)&COperatingSystem::m_operatingSystem, (void *)&obj.m_operatingSystem) << "m_operatingSystem";
            obj.Init();
            AssertRemoveDefaultMessages(scfToInternalsQueue);
        }

        void ProgressiveProtocolGetInternalsQueue()
        {
            ASSERT_TRUE(obj.GetInternalsQueue() != NULL) << "GetInternalsQueue not null";
            ASSERT_EQ((void *)&obj.m_internalsToSCFQueue, (void *)obj.GetInternalsQueue()) << "GetInternalsQueue value";
        }

        void ProgressiveProtocolIsTimeToClose()
        {
            ASSERT_FALSE(obj.IsTimeToClose()) << "IsTimeToClose";
        }

    };

// #pragma region CProgressiveProtocol
		/// <summary>
		/// Functional test for:
		///     CProgressiveProtocol::CProgressiveProtocol()
		/// Test for initial values with default operating system.
		/// </summary>
    TEST_F(ProgressiveProtocol, CProgressiveProtocol_Default)
    {
        AssertProtocolDefault();
    }


		/// <summary>
		/// Functional test for:
		///     CProgressiveProtocol::CProgressiveProtocol()
		/// Test for initial values with non-default operating system.
		/// </summary>
		TEST_F(ProgressiveProtocol, CProgressiveProtocol_NonDefault)
		{
            ProgressiveProtocolNonDefault();
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveProtocol::Init()
		/// Test for initial values with default operating system.
		/// </summary>

		TEST_F(ProgressiveProtocol, CProgressiveProtocol_Init)
		{
            ProgressiveProtocolInit();
        }

		/// <summary>
		/// Functional test for:
		///     CProgressiveProtocol::CProgressiveProtocol()
		///     CProgressiveProtocol::GetInternalsQueue()
		/// Test return value for GetInternalsQueue().
		/// </summary>
		TEST_F(ProgressiveProtocol, CProgressiveProtocol_GetInternalsQueue)
		{
            ProgressiveProtocolGetInternalsQueue();
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveProtocol::CProgressiveProtocol()
		///     CProgressiveProtocol::IsTimeToClose()
		/// Test IsTimeToClose() for default value(s).
		/// </summary>
		TEST_F(ProgressiveProtocol, CProgressiveProtocol_IsTimeToClose)
		{
            ASSERT_FALSE(obj.IsTimeToClose()) << "IsTimeToClose";
        }
// #pragma endregion CProgressiveProtocol

	const string ProgressiveProtocol::m_testThreadName("ReadFromInternals/ProgressiveProtocol");
}
