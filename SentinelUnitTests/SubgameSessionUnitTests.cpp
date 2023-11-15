#include "stdafx.h"
#include "SubgameSession.h"
#include "GlobalDefs.h"

// Unit testing includes.
#include "UnitTestingOperatingSystem.h"
#include "UnitTestingUtils.h"
#include "metersdef.h"
#include "gtest/gtest.h"

using namespace std;

namespace SentinelUnitTests
{
    class CSubgameSessionMock : public CSubgameSession
    {
    public:
        CSubgameSessionMock(const SubgameSessionParameters& SubgameSessionParameters) : CSubgameSession(SubgameSessionParameters){}
        inline DWORD GetSessionGameStartCount(){return m_sessionGameStartCount;}
    };

	class SubgameSessionUnitTests : public ::testing::Test
	{
    public:
        virtual void SetUp() {}

        virtual void TearDown() {}  

		/// <summary>
		/// Code coverage and functional test for:
		///     SubgameSessionParameters::SubgameSessionParameters()
		/// </summary>
		void SubgameSessionParametersConstructorTest()
		{   
            // Setup.  Use the same object for each test.
            SubgameSessionParameters subgameSessionParameters;

			// Perform operation(s) and test(s).
			ASSERT_TRUE(subgameSessionParameters.m_gameSessionSequenceNumber == SESSION_ID_DEFAULT);
            ASSERT_TRUE(subgameSessionParameters.m_multiplier == DEFAULT_MULTIPLIER);
            ASSERT_TRUE(subgameSessionParameters.m_divider == DEFAULT_COUNTDOWN);
            ASSERT_TRUE(subgameSessionParameters.m_playSessionID == GAME_SESSION_ID_DEFAULT);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSubgameSession::CSubgameSession()
		/// </summary>
		void SubgameSessionConstructorTest()
		{			
			// Setup.  Use the same object for each test.
			SubgameSessionParameters subgameSessionParameters;
            subgameSessionParameters.m_divider = 1;
            subgameSessionParameters.m_gameSessionSequenceNumber = 2;
            subgameSessionParameters.m_multiplier = 3;
            subgameSessionParameters.m_playSessionID = 4;
			CSubgameSessionMock subgameSession(subgameSessionParameters);

			// Perform operation(s) and test(s).
            ASSERT_TRUE(subgameSessionParameters.m_gameSessionSequenceNumber == subgameSession.GetSubgameSessionParameters().m_gameSessionSequenceNumber);
            ASSERT_TRUE(subgameSessionParameters.m_multiplier == subgameSession.GetSubgameSessionParameters().m_multiplier);
            ASSERT_TRUE(subgameSessionParameters.m_divider == subgameSession.GetSubgameSessionParameters().m_divider);
            ASSERT_TRUE(subgameSessionParameters.m_playSessionID == subgameSession.GetSubgameSessionParameters().m_playSessionID);
            ASSERT_TRUE(SubgameSessionStatus::ActiveWaitingForParameters == subgameSession.GetStatus());
				bool tilted = false;
            ASSERT_TRUE(0 == subgameSession.GetMeterDelta(tilted));
            ASSERT_TRUE(0 == subgameSession.GetSessionGameStartCount());
		}

       
		/// <summary>
		/// Code coverage and functional test for:
        ///     CSubgameSession::CSubgameSession()
        ///     CSubgameSession::ApplySubgameSessionParameters()
		/// </summary>
		void ApplySubgameSessionParametersActiveWaitingForParametersTest()
		{			
			// Setup.  Use the same object for each test.
			SubgameSessionParameters subgameSessionParameters;
            subgameSessionParameters.m_divider = 1;
            subgameSessionParameters.m_gameSessionSequenceNumber = 2;
            subgameSessionParameters.m_multiplier = 3;
            subgameSessionParameters.m_playSessionID = 4;
			CSubgameSessionMock subgameSession(subgameSessionParameters);

			// Perform operation(s) and test(s).
            ASSERT_TRUE(subgameSessionParameters.m_gameSessionSequenceNumber == subgameSession.GetSubgameSessionParameters().m_gameSessionSequenceNumber);
            ASSERT_TRUE(subgameSessionParameters.m_multiplier == subgameSession.GetSubgameSessionParameters().m_multiplier);
            ASSERT_TRUE(subgameSessionParameters.m_divider == subgameSession.GetSubgameSessionParameters().m_divider);
            ASSERT_TRUE(subgameSessionParameters.m_playSessionID == subgameSession.GetSubgameSessionParameters().m_playSessionID);
            ASSERT_TRUE(SubgameSessionStatus::ActiveWaitingForParameters == subgameSession.GetStatus());
				bool tilted = false;
            ASSERT_TRUE(0 == subgameSession.GetMeterDelta(tilted));
            ASSERT_TRUE(0 == subgameSession.GetSessionGameStartCount());

            //change m_gameSessionSequenceNumber to 99;
            subgameSessionParameters.m_gameSessionSequenceNumber = 99;
            ASSERT_FALSE(subgameSession.ApplySubgameSessionParameters(subgameSessionParameters));

            //Test to make it goes to active
            subgameSessionParameters.m_divider = 10;
            subgameSessionParameters.m_gameSessionSequenceNumber = 2;
            subgameSessionParameters.m_multiplier = 20;
            subgameSessionParameters.m_playSessionID = 30;
            ASSERT_TRUE(subgameSession.ApplySubgameSessionParameters(subgameSessionParameters));
            ASSERT_TRUE(subgameSessionParameters.m_multiplier == subgameSession.GetSubgameSessionParameters().m_multiplier);
            ASSERT_TRUE(subgameSessionParameters.m_divider == subgameSession.GetSubgameSessionParameters().m_divider);
            ASSERT_TRUE(subgameSessionParameters.m_playSessionID == subgameSession.GetSubgameSessionParameters().m_playSessionID);
            ASSERT_TRUE(SubgameSessionStatus::Active == subgameSession.GetStatus());
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSubgameSession::CSubgameSession()
		///     CSubgameSession::ApplySubgameSessionParameters()
		///     CSubgameSession::CloseSession
		/// </summary>
		void ApplySubgameSessionParametersClosedWaitingForParametersTest()
		{
			// Setup.  Use the same object for each test.
			SubgameSessionParameters subgameSessionParameters;
			subgameSessionParameters.m_divider = 1;
			subgameSessionParameters.m_gameSessionSequenceNumber = 2;
			subgameSessionParameters.m_multiplier = 3;
			subgameSessionParameters.m_playSessionID = 4;
			CSubgameSessionMock subgameSession(subgameSessionParameters);

			// Perform operation(s) and test(s).
			ASSERT_TRUE(subgameSessionParameters.m_gameSessionSequenceNumber == subgameSession.GetSubgameSessionParameters().m_gameSessionSequenceNumber);
			ASSERT_TRUE(subgameSessionParameters.m_multiplier == subgameSession.GetSubgameSessionParameters().m_multiplier);
			ASSERT_TRUE(subgameSessionParameters.m_divider == subgameSession.GetSubgameSessionParameters().m_divider);
			ASSERT_TRUE(subgameSessionParameters.m_playSessionID == subgameSession.GetSubgameSessionParameters().m_playSessionID);
			ASSERT_TRUE(SubgameSessionStatus::ActiveWaitingForParameters == subgameSession.GetStatus());
			bool tilted = false;
			ASSERT_TRUE(0 == subgameSession.GetMeterDelta(tilted));
			ASSERT_TRUE(0 == subgameSession.GetSessionGameStartCount());

			//change m_gameSessionSequenceNumber to 99;
			subgameSessionParameters.m_gameSessionSequenceNumber = 99;
			ASSERT_FALSE(subgameSession.CloseSession(subgameSessionParameters.m_gameSessionSequenceNumber));
			ASSERT_FALSE(subgameSession.ApplySubgameSessionParameters(subgameSessionParameters));

			//Test to make it goes from ClosedWaitingForParameters to Closed
			subgameSessionParameters.m_divider = 10;
			subgameSessionParameters.m_gameSessionSequenceNumber = 2;
			subgameSessionParameters.m_multiplier = 20;
			subgameSessionParameters.m_playSessionID = 30;
			ASSERT_TRUE(subgameSession.CloseSession(subgameSessionParameters.m_gameSessionSequenceNumber));
			ASSERT_TRUE(subgameSession.ApplySubgameSessionParameters(subgameSessionParameters));
			ASSERT_TRUE(subgameSessionParameters.m_multiplier == subgameSession.GetSubgameSessionParameters().m_multiplier);
			ASSERT_TRUE(subgameSessionParameters.m_divider == subgameSession.GetSubgameSessionParameters().m_divider);
			ASSERT_TRUE(subgameSessionParameters.m_playSessionID == subgameSession.GetSubgameSessionParameters().m_playSessionID);
			ASSERT_TRUE(SubgameSessionStatus::Closed == subgameSession.GetStatus());
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSubgameSession::CSubgameSession()
		///     CSubgameSession::ApplyMeterChange()
		/// </summary>
		void ApplyMeterChangeToNonActiveSessionTest()
		{
			// Setup.  Use the same object for each test.
			SubgameSessionParameters subgameSessionParameters;
			subgameSessionParameters.m_divider = 1;
			subgameSessionParameters.m_gameSessionSequenceNumber = 2;
			subgameSessionParameters.m_multiplier = 3;
			subgameSessionParameters.m_playSessionID = 4;
			CSubgameSessionMock subgameSession(subgameSessionParameters);

			// Perform operation(s) and test(s).
			ASSERT_TRUE(subgameSessionParameters.m_gameSessionSequenceNumber == subgameSession.GetSubgameSessionParameters().m_gameSessionSequenceNumber);
			ASSERT_TRUE(subgameSessionParameters.m_multiplier == subgameSession.GetSubgameSessionParameters().m_multiplier);
			ASSERT_TRUE(subgameSessionParameters.m_divider == subgameSession.GetSubgameSessionParameters().m_divider);
			ASSERT_TRUE(subgameSessionParameters.m_playSessionID == subgameSession.GetSubgameSessionParameters().m_playSessionID);
			ASSERT_TRUE(SubgameSessionStatus::ActiveWaitingForParameters == subgameSession.GetStatus());
			bool tilted = false;
			ASSERT_TRUE(0 == subgameSession.GetMeterDelta(tilted));
			ASSERT_TRUE(0 == subgameSession.GetSessionGameStartCount());

			//change m_gameSessionSequenceNumber to 99;
			subgameSessionParameters.m_gameSessionSequenceNumber = 99;
			ASSERT_FALSE(subgameSession.CloseSession(subgameSessionParameters.m_gameSessionSequenceNumber));
			ASSERT_FALSE(subgameSession.ApplySubgameSessionParameters(subgameSessionParameters));

			//Test ApplyMeterChange
			subgameSessionParameters.m_divider = 10;
			subgameSessionParameters.m_gameSessionSequenceNumber = 2;
			subgameSessionParameters.m_multiplier = 20;
			subgameSessionParameters.m_playSessionID = 30;
			ASSERT_TRUE(subgameSession.CloseSession(subgameSessionParameters.m_gameSessionSequenceNumber));
			DWORD meterDelta(100);
			CMeters *meters = new CMeters(MEMORY_NONE);
			CMeters *orig = new CMeters(MEMORY_NONE);
			orig->SetMeter(gCI, 1, true, false);
			meters->SetMeter(gCI, 1 + meterDelta, true, false);
			ASSERT_FALSE(subgameSession.ApplyMeterChange(meters, orig));
			delete meters;
			delete orig;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSubgameSession::CSubgameSession()
		///     CSubgameSession::ApplyMeterChange()
		/// </summary>
		void ApplyMeterChangeToActiveSessionTest()
		{
			// Setup.  Use the same object for each test.
			SubgameSessionParameters subgameSessionParameters;
			subgameSessionParameters.m_divider = 1;
			subgameSessionParameters.m_gameSessionSequenceNumber = 2;
			subgameSessionParameters.m_multiplier = 3;
			subgameSessionParameters.m_playSessionID = 4;
			CSubgameSessionMock subgameSession(subgameSessionParameters);

			// Perform operation(s) and test(s).
			ASSERT_TRUE(subgameSessionParameters.m_gameSessionSequenceNumber == subgameSession.GetSubgameSessionParameters().m_gameSessionSequenceNumber);
			ASSERT_TRUE(subgameSessionParameters.m_multiplier == subgameSession.GetSubgameSessionParameters().m_multiplier);
			ASSERT_TRUE(subgameSessionParameters.m_divider == subgameSession.GetSubgameSessionParameters().m_divider);
			ASSERT_TRUE(subgameSessionParameters.m_playSessionID == subgameSession.GetSubgameSessionParameters().m_playSessionID);
			ASSERT_TRUE(SubgameSessionStatus::ActiveWaitingForParameters == subgameSession.GetStatus());
			bool tilted = false;
			ASSERT_TRUE(0 == subgameSession.GetMeterDelta(tilted));
			ASSERT_TRUE(0 == subgameSession.GetSessionGameStartCount());

			//Test ApplyMeterChange
			DWORD meterDelta(100);
			CMeters *meters = new CMeters(MEMORY_NONE);
			CMeters *orig = new CMeters(MEMORY_NONE);
			orig->SetMeter(gCI, 1, true, false);
			meters->SetMeter(gCI, 1 + meterDelta, true, false);
			ASSERT_TRUE(subgameSession.ApplyMeterChange(meters, orig));
			delete meters;
			delete orig;
			ASSERT_TRUE(meterDelta == subgameSession.GetMeterDelta(tilted));
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSubgameSession::CSubgameSession()
		///     CSubgameSession::IncrementGameStart()
		/// </summary>
		void NonActiveIncrementGameStartTest()
		{
			// Setup.  Use the same object for each test.
			SubgameSessionParameters subgameSessionParameters;
			subgameSessionParameters.m_divider = 1;
			subgameSessionParameters.m_gameSessionSequenceNumber = 2;
			subgameSessionParameters.m_multiplier = 3;
			subgameSessionParameters.m_playSessionID = 4;
			CSubgameSessionMock subgameSession(subgameSessionParameters);

			// Perform operation(s) and test(s).
			ASSERT_TRUE(subgameSessionParameters.m_gameSessionSequenceNumber == subgameSession.GetSubgameSessionParameters().m_gameSessionSequenceNumber);
			ASSERT_TRUE(subgameSessionParameters.m_multiplier == subgameSession.GetSubgameSessionParameters().m_multiplier);
			ASSERT_TRUE(subgameSessionParameters.m_divider == subgameSession.GetSubgameSessionParameters().m_divider);
			ASSERT_TRUE(subgameSessionParameters.m_playSessionID == subgameSession.GetSubgameSessionParameters().m_playSessionID);
			ASSERT_TRUE(SubgameSessionStatus::ActiveWaitingForParameters == subgameSession.GetStatus());
			bool tilted = false;
			ASSERT_TRUE(0 == subgameSession.GetMeterDelta(tilted));
			ASSERT_TRUE(0 == subgameSession.GetSessionGameStartCount());

			//Test ApplyMeterChange
			ASSERT_TRUE(subgameSession.CloseSession(subgameSessionParameters.m_gameSessionSequenceNumber));
			ASSERT_FALSE(subgameSession.IncrementGameStart());
			ASSERT_TRUE(0 == subgameSession.GetSessionGameStartCount());
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSubgameSession::CSubgameSession()
		///     CSubgameSession::IncrementGameStart()
		/// </summary>
		void ActiveIncrementGameStartTest()
		{
			// Setup.  Use the same object for each test.
			SubgameSessionParameters subgameSessionParameters;
			subgameSessionParameters.m_divider = 1;
			subgameSessionParameters.m_gameSessionSequenceNumber = 2;
			subgameSessionParameters.m_multiplier = 3;
			subgameSessionParameters.m_playSessionID = 4;
			CSubgameSessionMock subgameSession(subgameSessionParameters);

			// Perform operation(s) and test(s).
			ASSERT_TRUE(subgameSessionParameters.m_gameSessionSequenceNumber == subgameSession.GetSubgameSessionParameters().m_gameSessionSequenceNumber);
			ASSERT_TRUE(subgameSessionParameters.m_multiplier == subgameSession.GetSubgameSessionParameters().m_multiplier);
			ASSERT_TRUE(subgameSessionParameters.m_divider == subgameSession.GetSubgameSessionParameters().m_divider);
			ASSERT_TRUE(subgameSessionParameters.m_playSessionID == subgameSession.GetSubgameSessionParameters().m_playSessionID);
			ASSERT_TRUE(SubgameSessionStatus::ActiveWaitingForParameters == subgameSession.GetStatus());
			bool tilted = false;
			ASSERT_TRUE(0 == subgameSession.GetMeterDelta(tilted));
			ASSERT_TRUE(0 == subgameSession.GetSessionGameStartCount());

			//Test ApplyMeterChange
			ASSERT_TRUE(subgameSession.IncrementGameStart());
			ASSERT_TRUE(1 == subgameSession.GetSessionGameStartCount());
			ASSERT_FALSE(subgameSession.IncrementGameStart());
			ASSERT_TRUE(2 == subgameSession.GetSessionGameStartCount());
		}
    };

	/// <summary>
	/// Code coverage and functional test for:
	///     SubgameSessionParameters::SubgameSessionParameters()
	/// </summary>
	TEST_F(SubgameSessionUnitTests, SubgameSessionParametersConstructorTest)
	{
		SubgameSessionParametersConstructorTest();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CSubgameSession::CSubgameSession()
	/// </summary>
	TEST_F(SubgameSessionUnitTests, SubgameSessionConstructorTest)
	{
		SubgameSessionConstructorTest();
	}

   	/// <summary>
	/// Code coverage and functional test for:
	///     CSubgameSession::CSubgameSession()
	///     CSubgameSession::ApplySubgameSessionParameters()
	/// </summary>
	TEST_F(SubgameSessionUnitTests, ApplySubgameSessionParametersActiveWaitingForParametersTest)
	{
		ApplySubgameSessionParametersActiveWaitingForParametersTest();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CSubgameSession::CSubgameSession()
	///     CSubgameSession::ApplySubgameSessionParameters()
	///     CSubgameSession::CloseSession
	/// </summary>
	TEST_F(SubgameSessionUnitTests, ApplySubgameSessionParametersClosedWaitingForParametersTest)
	{
		ApplySubgameSessionParametersClosedWaitingForParametersTest();
	}

	
	/// <summary>
	/// Code coverage and functional test for:
	///     CSubgameSession::CSubgameSession()
	///     CSubgameSession::ApplyMeterChange()
	/// </summary>
	TEST_F(SubgameSessionUnitTests, ApplyMeterChangeToNonActiveSessionTest)
	{
		ApplyMeterChangeToNonActiveSessionTest();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CSubgameSession::CSubgameSession()
	///     CSubgameSession::ApplyMeterChange()
	/// </summary>
	TEST_F(SubgameSessionUnitTests, ApplyMeterChangeToActiveSessionTest)
	{
		ApplyMeterChangeToActiveSessionTest();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CSubgameSession::CSubgameSession()
	///     CSubgameSession::IncrementGameStart()
	/// </summary>
	TEST_F(SubgameSessionUnitTests, NonActiveIncrementGameStartTest)
	{
		NonActiveIncrementGameStartTest();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CSubgameSession::CSubgameSession()
	///     CSubgameSession::IncrementGameStart()
	/// </summary>
	TEST_F(SubgameSessionUnitTests, ActiveIncrementGameStartTest)
	{
		ActiveIncrementGameStartTest();
	}
}