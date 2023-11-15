#include "stdafx.h"
#include "SASProtocol/SASProtocol.h"

// Sentinel includes.
#include "InternalMessage.h"

// Unit testing includes.
#include "UnitTestingOperatingSystem.h"
#include "gtest/gtest.h"

namespace SentinelUnitTests
{ 
    const std::string m_testThreadName("ReadInternalsQueueThread/EGMPRotocol");

    class SASProtocol : public ::testing::Test
	{    
		public:   
			virtual void SetUp()  
			{
            } 
			virtual void TearDown()
			{
			}  

            // Remove all messages and assert that they are the correct default message(s).
            void AssertRemoveDefaultMessages(CQueueList &scfToInternalsQueue)
    		{
    			// No messages present by default.

    			// Verify that there are no more messages.
    			ASSERT_EQ(nullptr, scfToInternalsQueue.Remove()) << "Remove default final";
    		}
            void CSASProtocol_Default();
            void CSASProtocol_NonDefault();
            void CSASProtocol_Init();
            void CSASProtocol_GetInternalsQueue();
            void CSASProtocol_IsTimeToClose();
    };

// #pragma region CSASProtocol
		/// <summary>
		/// Functional test for:
		///     CSASProtocol::CSASProtocol()
		/// Test for initial values with default operating system.
		/// </summary>
		TEST_F(SASProtocol, CSASProtocol_Default)
		{
            CSASProtocol_Default();
        }
        void SASProtocol::CSASProtocol_Default()
		{	// Setup.

			CQueueList scfToInternalsQueue;

			// Perform operation(s) and test(s).
			CSASProtocol obj(scfToInternalsQueue);
			//ASSERT_EQ((void *)&scfToInternalsQueue, (void *)&obj.m_scfToInternalsQueue) << "scfToInternalsQueue";
			//ASSERT_TRUE(obj.m_internalsThread == NULL) << "m_internalsThread";
			//ASSERT_EQ(m_testThreadName, obj.m_threadName) << "m_threadName";
			//ASSERT_EQ((void *)&COperatingSystem::m_operatingSystem, (void *)&obj.m_operatingSystem) << "m_operatingSystem";
			//ASSERT_TRUE(scfToInternalsQueue.Remove() == NULL) << "Remove final"; // Verify that the queue is now empty.

			ASSERT_FALSE(obj.IsInitialized()) << "IsInitialized";

		}

		/// <summary>
		/// Functional test for:
		///     CSASProtocol::CSASProtocol()
		/// Test for initial values with non-default operating system.
		/// </summary>
		TEST_F(SASProtocol, CSASProtocol_NonDefault)
		{
            CSASProtocol_NonDefault();
        }
        void SASProtocol::CSASProtocol_NonDefault()
		{	// Setup.

			CQueueList scfToInternalsQueue;
			COperatingSystemTest operatingSystem;

			// Perform operation(s) and test(s).
			CSASProtocol obj(scfToInternalsQueue, operatingSystem);
			//ASSERT_EQ((void *)&scfToInternalsQueue, (void *)&obj.m_scfToInternalsQueue) << "scfToInternalsQueue";
			//ASSERT_TRUE(obj.m_internalsThread == NULL) << "m_internalsThread";
			//ASSERT_EQ(m_testThreadName, obj.m_threadName) << "m_threadName";
			//ASSERT_EQ((void *)&operatingSystem, (void *)&obj.m_operatingSystem) << "m_operatingSystem";

			ASSERT_FALSE(obj.IsInitialized()) << "IsInitialized";

		}

		/// <summary>
		/// Functional test for:
		///     CSASProtocol::Init()
		/// Test for initial values with default operating system.
		/// </summary>
		TEST_F(SASProtocol, CSASProtocol_Init)
		{
            CSASProtocol_Init();
        }
        void SASProtocol::CSASProtocol_Init()
		{	// Setup.
			CQueueList scfToInternalsQueue;
			CSASProtocol obj(scfToInternalsQueue);
			//ASSERT_EQ((void *)&scfToInternalsQueue, (void *)&obj.m_scfToInternalsQueue) << "scfToInternalsQueue";
			//ASSERT_EQ(nullptr, obj.m_internalsThread) << "m_internalsThread";
			//ASSERT_EQ(m_testThreadName, obj.m_threadName) << "m_threadName";
			//ASSERT_EQ((void *)&COperatingSystem::m_operatingSystem, (void *)&obj.m_operatingSystem) << "m_operatingSystem";
			//ASSERT_EQ(nullptr, obj.m_sas) << "m_sas setup";

			// Perform operation(s) and test(s).
#if 000//TODO-PORT
         Test case crashes while creating thread
			obj.Init();
			ASSERT_NE(nullptr, obj.m_sas) << "m_sas";
			ASSERT_NE(nullptr, obj.m_sasPollQueue);
			ASSERT_NE(nullptr, obj.m_sendToEGMThread);
			ASSERT_EQ(nullptr, obj.m_scheduleThread);
#endif
            AssertRemoveDefaultMessages(scfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASProtocol::CSASProtocol()
		///     CSASProtocol::GetInternalsQueue()
		/// Test return value for GetInternalsQueue().
		/// </summary>
		TEST_F(SASProtocol, CSASProtocol_GetInternalsQueue)
		{
            CSASProtocol_GetInternalsQueue();
        }
        void SASProtocol::CSASProtocol_GetInternalsQueue()
		{	// Setup.
			CQueueList scfToInternalsQueue;

			// Perform operation(s) and test(s).
			CSASProtocol obj(scfToInternalsQueue);
			ASSERT_TRUE(obj.GetInternalsQueue() != NULL) << "GetInternalsQueue not null";
			//ASSERT_EQ((void *)&obj.m_internalsToSCFQueue, (void *)obj.GetInternalsQueue()) << "GetInternalsQueue value";
		}

		/// <summary>
		/// Functional test for:
		///     CSASProtocol::CSASProtocol()
		///     CSASProtocol::IsTimeToClose()
		/// Test IsTimeToClose() for default value(s).
		/// </summary>
		TEST_F(SASProtocol, CSASProtocol_IsTimeToClose)
		{
            CSASProtocol_IsTimeToClose();
        }
		void SASProtocol::CSASProtocol_IsTimeToClose()
        {	// Setup.
			CQueueList scfToInternalsQueue;

			// Perform operation(s) and test(s).
			CSASProtocol obj(scfToInternalsQueue);
			ASSERT_FALSE(obj.IsTimeToClose()) << "IsTimeToClose";
		}
// #pragma endregion CSASProtocol
}
