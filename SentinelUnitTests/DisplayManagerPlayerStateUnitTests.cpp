
#include "stdafx.h"

#include "DisplayManagerPlayerState.h"
#include "gtest/gtest.h"

//
//static const byte MEMSET_DEFAULT_VALUE = 0xcd; // Value unlikely to be used for other purposes.
//static const time_t TIME_DEFAULT_VALUE(0); // Arbitrary value when any valid value is okay.

class DisplayManagerPlayerState : public ::testing::Test
{
protected:
	virtual void SetUp()
	{
	}

	virtual void TearDown()
	{
	}

};

// GetHasFirstPlayTest
// Test the implementation of:
//     DisplayManagerPlayerState::SetHasFirstPlay().
//     DisplayManagerPlayerState::GetHasFirstPlay().
TEST_F(DisplayManagerPlayerState, GetHasFirstPlayTest)
{
	typedef struct
	{
		// Inputs.
		const DWORD m_hasFirstPlay;

		// Expected results.
		const DWORD m_hasFirstPlayExpected;
	} TestDataType;

	const TestDataType TEST_DATA[] =
	{
		{ true, true },
		{ false, false },
		{ true, true },
		{ false, false },
	};

	// For each pass run through entire data set with a single instance.
	// But start each pass with with a different m_inProgress value for the first call to SetHasFirstPlay().
	for (int passIndex = 0; passIndex <= 1; ++passIndex)
	{
		// Setup.
		CDisplayManagerPlayerState playerState(*CMediaWindowConfig::Instance());

		for (int dataIndex = 0; dataIndex < _countof(TEST_DATA); ++dataIndex)
		{
			int testIndex = (passIndex + dataIndex) % _countof(TEST_DATA);
			// Perform operation(s) to be tested.
			playerState.SetHasFirstPlay(TEST_DATA[testIndex].m_hasFirstPlay);

			// Test #1.
			DWORD hasFirstPlayResult = playerState.GetHasFirstPlay();
			{
				// Report #1.
				EXPECT_EQ(TEST_DATA[testIndex].m_hasFirstPlayExpected, hasFirstPlayResult);
			}
		}

		// Cleanup.
	}
}

// GetGameInProgressTest
// Test the implementation of:
//     DisplayManagerPlayerState::SetGameInProgress().
//     DisplayManagerPlayerState::GetGameInProgress().
TEST_F(DisplayManagerPlayerState, GetGameInProgressTest)
{
	typedef struct
	{
		// Inputs.
		const DWORD m_inProgress;
		const DWORD m_tickCount;

		// Expected results.
		const DWORD m_inProgressExpected;
	} TestDataType;

	const TestDataType TEST_DATA[] =
	{
		{ true, 0, true },
		{ false, 0, false },
		{ true, 1, true },
		{ false, 1, false },
	};

	// For each pass run through entire data set with a single instance.
	// But start each pass with with a different m_inProgress value for the first call to SetGameInProgress().
	for (int passIndex = 0; passIndex <= 1; ++passIndex)
	{
		// Setup.
		CDisplayManagerPlayerState playerState(*CMediaWindowConfig::Instance());

		for (int dataIndex = 0; dataIndex < _countof(TEST_DATA); ++dataIndex)
		{
			int testIndex = (passIndex + dataIndex) % _countof(TEST_DATA);
			// Perform operation(s) to be tested.
			playerState.SetGameInProgress(TEST_DATA[testIndex].m_inProgress, TEST_DATA[testIndex].m_tickCount);

			// Test #1.
			DWORD inProgressResult = playerState.GetGameInProgress();

			{
				// Report #1.
				EXPECT_EQ(TEST_DATA[testIndex].m_inProgressExpected, inProgressResult);
			}
		}

		// Cleanup.
	}
}

// GetGameNotInProgressTickCountTest
// Test the implementation of:
//     DisplayManagerPlayerState::SetGameInProgress().
//     DisplayManagerPlayerState::GetGameNotInProgressTickCount().
TEST_F(DisplayManagerPlayerState, GetGameNotInProgressTickCountTest)
{
	typedef struct
	{
		// Inputs.
		const DWORD m_tickCount;

		// Expected results.
		const DWORD m_tickCountExpected;
	} TestDataType;

	// Test typical edge values +- 1.
	const TestDataType TEST_DATA[] =
	{
		{ 0, 0 },
		{ 1, 1 },
		{ 0x7ffffffe,  0x7ffffffe },
		{ 0x7fffffff,  0x7fffffff },
		{ 0x80000000, 0x80000000 },
		{ 0x80000001, 0x80000001 },
		{ 0xfffffffe,  0xfffffffe },
		{ 0xffffffff,  0xffffffff },
	};

	for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
	{
		// Setup.
		CDisplayManagerPlayerState playerState(*CMediaWindowConfig::Instance());

		// Perform operation(s) to be tested.
		playerState.SetGameInProgress(false, TEST_DATA[testIndex].m_tickCount);

		// Test #1.
		DWORD tickCountResult = playerState.GetGameNotInProgressTickCount();

		{
			// Report #1.
			EXPECT_EQ(TEST_DATA[testIndex].m_tickCountExpected, tickCountResult);
		}

		// Cleanup.
	}
}

// IsExpiredGameNotInProgressBeginSessionDelayTest
// Test the implementation of DisplayManagerPlayerState::IsExpiredGameNotInProgressBeginSessionDelay().
TEST_F(DisplayManagerPlayerState, IsExpiredGameNotInProgressBeginSessionDelayTest)
{
	// Test typical edge values +- 1.
	const DWORD TEST_DATA[] =
	{
		0,
		1,
		0x7ffffffe,
		0x7fffffff,
		0x80000000,
		0x80000001,
		0xfffffffe,
		0xffffffff,
	};

	// Test data for boolean values.
	const bool TEST_DATA_BOOL[] =
	{
		false,
		true,
	};

	// Test expiration with TEST_DATA for init
	//     and TEST_DATA+TEST_DATA for thresholds?
	//     plus TEST_DATA+TEST_DATA+GetGameNotInProgressBeginSessionDelayInMilliseconds() for thresholds
	// Calculate the expected results at runtime.
	//     because GetGameNotInProgressBeginSessionDelayInMilliseconds is NOT configurable at run-time.
	//     because GetGameNotInProgressBeginSessionDelayInMilliseconds is NOT controlled by the test data at compile-time.
	//     and if GetGameNotInProgressBeginSessionDelayInMilliseconds was known at compile-time
	//         then the same "run-time" calculations would need to be performed at compile-time.
	for (int testIndexInitial = 0; testIndexInitial < _countof(TEST_DATA); ++testIndexInitial)
	{
		for (int testIndexElapsed = 0; testIndexElapsed < _countof(TEST_DATA); ++testIndexElapsed)
		{
			for (int testIndexAddExpired = 0; testIndexAddExpired < _countof(TEST_DATA_BOOL); ++testIndexAddExpired)
			{
				// Setup.
				CDisplayManagerPlayerState playerState(*CMediaWindowConfig::Instance());

				// Calculate test parameters.
				DWORD expiredTickCount = playerState.GetGameNotInProgressBeginSessionDelayInMilliseconds();
				DWORD testDataElapsedTickCount = TEST_DATA[testIndexElapsed];
				testDataElapsedTickCount += testIndexAddExpired ? expiredTickCount : 0;
				// Calculate testDataIsExpiredExpected.  If unsigned expiredTickCount==0 then timer is always expired.
				bool testDataIsExpiredExpected = testDataElapsedTickCount >= expiredTickCount;
				DWORD testDataCurrentTickCount = TEST_DATA[testIndexInitial] + testDataElapsedTickCount;

				// Perform operation(s) to be tested.
				playerState.SetGameInProgress(false, TEST_DATA[testIndexInitial]);

				// Test #1.
				bool isExpiredResult = playerState.IsExpiredGameNotInProgressBeginSessionDelay(testDataCurrentTickCount);

				{
					// Report #1.
					EXPECT_EQ(testDataIsExpiredExpected, isExpiredResult);
				}

				// Cleanup.
			}
		}
	}
}
