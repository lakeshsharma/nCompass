#include "stdafx.h"
#include "ProgressiveProtocol/CumulativeProgressiveManager.h"

// Unit testing includes.
//#include "UnitTestingToString.h"
#include "UnitTestingUtils.h"


#include "gtest/gtest.h"


namespace SentinelNativeUnitTests
{
	// Data for CumulativeProgressiveManager_PersistNonDefault().
	static const __int64 persistedCurrentCumulativeMeterValue = 13579;
	static const bool persistedHasCumulativeMeterAlready = true;

    class CumulativeProgressiveManagerTests :public ::testing::Test
	{
	public:
		/// <summary>
		/// Persists a non-default CumulativeProgressiveManager instance.
		/// </summary>
		void CumulativeProgressiveManager_PersistNonDefault()
		{
			CUnitTestingUtils::SetNvramValidAndUpgraded();
			CumulativeProgressiveManager obj(MEMORY_NVRAM);
			obj.ClearHasCumulativeMeterStatus(); // Clear status so GetCumulativeDelta() has a known effect.
			obj.GetCumulativeDelta(persistedCurrentCumulativeMeterValue);
			ASSERT_EQ(persistedHasCumulativeMeterAlready, obj.HasCumulativeMeterAlready()) << "HasAlready Setup"; // Verify Setup.
			ASSERT_EQ(persistedCurrentCumulativeMeterValue, obj.m_currentCumulativeMeterValue) << "MeterValue Setup";
        }

        MEMORY_TYPE	GetMemoryType(CumulativeProgressiveManager& obj)
        {
            return obj.m_type;
        }

        DWORD GetDWOffset(CumulativeProgressiveManager& obj)
        {
            return obj.m_dwOffset;
        }

        DWORD GetDWAllowedSize(CumulativeProgressiveManager& obj)
        { 
            return obj.m_dwAllowedSize;
        }

        DWORD GetActiveOffset(CumulativeProgressiveManager& obj)
        {
            return  obj.m_activeOffset;
        }

        BYTE GetActiveValue(CumulativeProgressiveManager& obj)
		{
            return obj.m_activeValue;
        }

        __int64 GetCurrentCumulativeMeterValue(CumulativeProgressiveManager& obj)
		{
            return obj.m_currentCumulativeMeterValue;
        }
	

    };

		// Default expected result(s).
		static const __int64 m_currentCumulativeMeterValueDefault = 0;
		static const bool m_hasCumulativeMeterAlreadyDefault = false;

// #pragma region NvramMap
		/// <summary>
		/// Test that the NVRAM memory map has not changed across versions.
		/// If the NVRAM memory map does change across versions then memory map upgrades should be tested.
		/// </summary>
		TEST_F(CumulativeProgressiveManagerTests, CumulativeProgressiveManager_NvramMap)
		{
			// Test NVRAM memory map offset calculations.
			ASSERT_EQ(NVRAM_EGM_AWARD_STANDARD_CHECKBYTE_OFFSET + NVRAM_EGM_AWARD_STANDARD_SIZE,
				NVRAM_CUMULATIVE_PROGRESSIVE_CHECKBYTE_OFFSET) << "NVRAM_CUMULATIVE_PROGRESSIVE_CHECKBYTE_OFFSET calc";
			ASSERT_EQ(NVRAM_CUMULATIVE_PROGRESSIVE_CHECKBYTE_OFFSET + NVRAM_CUMULATIVE_PROGRESSIVE_CHECKBYTE_SIZE,
				NVRAM_CUMULATIVE_PROGRESSIVE_DATA_OFFSET) << "NVRAM_CUMULATIVE_PROGRESSIVE_DATA_OFFSET calc";

			// Test NVRAM memory map for backwards and forwards memory compatibility.
			ASSERT_EQ((DWORD)721963, NVRAM_CUMULATIVE_PROGRESSIVE_CHECKBYTE_OFFSET) << "NVRAM_CUMULATIVE_PROGRESSIVE_CHECKBYTE_OFFSET";
			ASSERT_EQ((DWORD)1, NVRAM_CUMULATIVE_PROGRESSIVE_CHECKBYTE_SIZE) << "NVRAM_CUMULATIVE_PROGRESSIVE_CHECKBYTE_SIZE";
			ASSERT_EQ((BYTE)0x13, NVRAM_CUMULATIVE_PROGRESSIVE_CHECKBYTE_VALUE) << "NVRAM_CUMULATIVE_PROGRESSIVE_CHECKBYTE_VALUE";
			ASSERT_EQ((DWORD)721964, NVRAM_CUMULATIVE_PROGRESSIVE_DATA_OFFSET) << "NVRAM_CUMULATIVE_PROGRESSIVE_DATA_OFFSET";
			ASSERT_EQ((DWORD)64, NVRAM_CUMULATIVE_PROGRESSIVE_DATA_SIZE) << "NVRAM_CUMULATIVE_PROGRESSIVE_DATA_SIZE";

			// Test NVRAM-related class members.
			ASSERT_EQ(NVRAM_CUMULATIVE_PROGRESSIVE_CHECKBYTE_OFFSET, CumulativeProgressiveManager::NVRAM_ACTIVE_OFFSET) << "NVRAM_ACTIVE_OFFSET";
			ASSERT_EQ(NVRAM_CUMULATIVE_PROGRESSIVE_CHECKBYTE_VALUE, CumulativeProgressiveManager::NVRAM_ACTIVE_VALUE) << "NVRAM_ACTIVE_VALUE";
			ASSERT_EQ(NVRAM_CUMULATIVE_PROGRESSIVE_DATA_OFFSET, CumulativeProgressiveManager::NVRAM_DATA_OFFSET) << "NVRAM_DATA_OFFSET";
			ASSERT_EQ(NVRAM_CUMULATIVE_PROGRESSIVE_DATA_SIZE, CumulativeProgressiveManager::NVRAM_DATA_SIZE) << "NVRAM_DATA_SIZE";
		}
// #pragma endregion NvramMap

// #pragma region NVRAM
		/// <summary>
		/// Functional test for:
		///     CumulativeProgressiveManager::CumulativeProgressiveManager()
		/// Test for correctly configured offsets, data size, and active value.
		/// </summary>
		TEST_F(CumulativeProgressiveManagerTests, CumulativeProgressiveManager_ConfigureOffsets)
		{
			// Data.
			const MEMORY_TYPE memoryTypeExpected(MEMORY_NVRAM);

			// Perform operation(s) and test(s).
			CumulativeProgressiveManager obj(memoryTypeExpected);
			ASSERT_EQ(memoryTypeExpected, GetMemoryType(obj) ) << "m_type";
			ASSERT_EQ(NVRAM_CUMULATIVE_PROGRESSIVE_DATA_OFFSET, GetDWOffset(obj)) << "m_dwOffset";
			ASSERT_EQ(NVRAM_CUMULATIVE_PROGRESSIVE_DATA_SIZE, GetDWAllowedSize(obj)) << "m_dwAllowedSize";
			ASSERT_EQ(NVRAM_CUMULATIVE_PROGRESSIVE_CHECKBYTE_OFFSET, GetActiveOffset(obj)) << "m_activeOffset";
			ASSERT_EQ(NVRAM_CUMULATIVE_PROGRESSIVE_CHECKBYTE_VALUE, GetActiveValue(obj)) << "m_activeValue";
		}

		/// <summary>
		/// Functional test for:
		///     CumulativeProgressiveManager::CumulativeProgressiveManager()
		/// Test constructor with default memory type -- expect MEMORY_NONE.
		/// </summary>
		TEST_F(CumulativeProgressiveManagerTests, CumulativeProgressiveManager_MemoryDefault)
		{
			// Setup.
			CumulativeProgressiveManager_PersistNonDefault();

			// Perform operation(s) and test(s).
			CumulativeProgressiveManager obj;
			ASSERT_EQ(m_hasCumulativeMeterAlreadyDefault, obj.HasCumulativeMeterAlready()) << "HasCumulativeMeterAlready";
			ASSERT_EQ(m_currentCumulativeMeterValueDefault, GetCurrentCumulativeMeterValue(obj)) << "m_currentCumulativeMeterValue";
		}

		/// <summary>
		/// Functional test for:
		///     CumulativeProgressiveManager::CumulativeProgressiveManager()
		/// Test constructor with memory type MEMORY_NONE.
		/// </summary>
		TEST_F(CumulativeProgressiveManagerTests, CumulativeProgressiveManager_MemoryNone)
		{
			// Setup.
			CumulativeProgressiveManager_PersistNonDefault();

			// Perform operation(s) and test(s).
			CumulativeProgressiveManager obj(MEMORY_NONE);
			ASSERT_EQ(m_hasCumulativeMeterAlreadyDefault, obj.HasCumulativeMeterAlready()) << "HasCumulativeMeterAlready";
			ASSERT_EQ(m_currentCumulativeMeterValueDefault, GetCurrentCumulativeMeterValue(obj)) << "m_currentCumulativeMeterValue";
		}

		/// <summary>
		/// Functional test for:
		///     CumulativeProgressiveManager::CumulativeProgressiveManager()
		///     CumulativeProgressiveManager::SetFromNvramBuffer()
		/// Test constructor with memory type MEMORY_NVRAM.
		/// </summary>
		TEST_F(CumulativeProgressiveManagerTests, CumulativeProgressiveManager_MemoryNvram)
		{
			// Setup.
			CumulativeProgressiveManager_PersistNonDefault();

			// Perform operation(s) and test(s).
			CumulativeProgressiveManager obj(MEMORY_NVRAM);
			ASSERT_EQ(persistedHasCumulativeMeterAlready, obj.HasCumulativeMeterAlready()) << "HasCumulativeMeterAlready";
			ASSERT_EQ(persistedCurrentCumulativeMeterValue, GetCurrentCumulativeMeterValue(obj)) << "m_currentCumulativeMeterValue";
		}

		/// <summary>
		/// Functional test for:
		///     CumulativeProgressiveManager::CumulativeProgressiveManager()
		/// Test constructor with persisted instance cleared.
		/// </summary>
		TEST_F(CumulativeProgressiveManagerTests, CumulativeProgressiveManager_MemoryClear)
		{
			// Setup.
			CumulativeProgressiveManager_PersistNonDefault();
			CMemoryStorage::SetInactiveLocation(NVRAM_CUMULATIVE_PROGRESSIVE_CHECKBYTE_OFFSET,
				(BYTE)~NVRAM_CUMULATIVE_PROGRESSIVE_CHECKBYTE_VALUE);

			// Perform operation(s) and test(s).
			CumulativeProgressiveManager obj(MEMORY_NVRAM);
			ASSERT_EQ(m_hasCumulativeMeterAlreadyDefault, obj.HasCumulativeMeterAlready()) << "HasCumulativeMeterAlready";
			ASSERT_EQ(m_currentCumulativeMeterValueDefault, GetCurrentCumulativeMeterValue(obj)) << "m_currentCumulativeMeterValue";
		}

		/// <summary>
		/// Functional test for:
		///     CumulativeProgressiveManager::CumulativeProgressiveManager()
		/// Test constructor with all NVRAM lost.
		/// </summary>
		TEST_F(CumulativeProgressiveManagerTests, CumulativeProgressiveManager_MemoryLost)
		{
			// Setup.
			CumulativeProgressiveManager_PersistNonDefault();
			CUnitTestingUtils::SetNvramInvalid();

			// Perform operation(s) and test(s).
			CumulativeProgressiveManager obj(MEMORY_NVRAM);
			ASSERT_EQ(m_hasCumulativeMeterAlreadyDefault, obj.HasCumulativeMeterAlready()) << "HasCumulativeMeterAlready";
			ASSERT_EQ(m_currentCumulativeMeterValueDefault, GetCurrentCumulativeMeterValue(obj)) << "m_currentCumulativeMeterValue";
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorage::UpgradeNVRAM()
		///     CumulativeProgressiveManager::CumulativeProgressiveManager()
		/// Test CMemoryStorage::UpgradeNVRAM() when NVRAM needs to be upgraded.
		/// </summary>
		TEST_F(CumulativeProgressiveManagerTests, CumulativeProgressiveManager_MemoryUpgrade)
		{
			// Setup.
			CumulativeProgressiveManager_PersistNonDefault();
			CUnitTestingUtils::SetNvramVersion(NVRAM_CUMULATIVE_PROGRESSIVE_VERSION - 1);

			// Perform operation(s) and test(s).
			CMemoryStorage::UpgradeNVRAM();
			CumulativeProgressiveManager obj(MEMORY_NVRAM);
			ASSERT_EQ(m_hasCumulativeMeterAlreadyDefault, obj.HasCumulativeMeterAlready()) << "HasCumulativeMeterAlready";
			ASSERT_EQ(m_currentCumulativeMeterValueDefault, GetCurrentCumulativeMeterValue(obj)) << "m_currentCumulativeMeterValue";
		}

		/// <summary>
		/// Functional test for:
		///     CumulativeProgressiveManager::GetNvramBuffer()
		/// Test GetNvramBuffer() with non-default values for forwards and backwards compatibility.
		/// </summary>
		TEST_F(CumulativeProgressiveManagerTests, GetNvramBuffer)
		{
			// Data.
			static const __int64 currentCumulativeMeterValue(0x0908070605040302);

			static const bool activeExpected(true);
			static const UINT bufferSizeUsedExpected(9);
			static const BYTE bufferExpected[NVRAM_CUMULATIVE_PROGRESSIVE_DATA_SIZE] =
			{
				0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, // m_currentCumulativeMeterValue.
				0x01, // m_hasCumulativeMeterAlready.
			};

			// Setup.
			CumulativeProgressiveManager obj;
			obj.GetCumulativeDelta(currentCumulativeMeterValue);

			// Perform operation(s) and test(s).
			BYTE buffer[NVRAM_CUMULATIVE_PROGRESSIVE_DATA_SIZE];
			bool active(!activeExpected);
			UINT bufferSizeUsed = obj.GetNvramBuffer(buffer, sizeof(buffer), active);
			ASSERT_EQ(activeExpected, active) << "active";
			ASSERT_EQ(bufferSizeUsedExpected, bufferSizeUsed) << "bufferSizeUsed";
			ASSERT_EQ(0, memcmp(buffer, bufferExpected, bufferSizeUsedExpected)) << "memcmp";
		}

		/// <summary>
		/// Functional test for:
		///     CumulativeProgressiveManager::SetFromNvramBuffer()
		/// Test SetFromNvramBuffer() with non-default values for forwards and backwards compatibility.
		/// </summary>
		TEST_F(CumulativeProgressiveManagerTests, SetFromNvramBuffer)
		{
			// Data.
			static const __int64 currentCumulativeMeterValue(0x0908070605040302);
			static const bool hasCumulativeMeterAlready(true);

			static const UINT bufferSizeUsed(9);
			BYTE buffer[NVRAM_CUMULATIVE_PROGRESSIVE_DATA_SIZE] =
			{
				0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, // m_currentCumulativeMeterValue.
				0x01, // m_hasCumulativeMeterAlready.
			};
			memset(buffer + bufferSizeUsed, ~CumulativeProgressiveManager::NVRAM_ACTIVE_VALUE, sizeof(buffer) - bufferSizeUsed);

			// Setup.
			CumulativeProgressiveManager obj;

			// Perform operation(s) and test(s).
			obj.SetFromNvramBuffer(buffer, sizeof(buffer));
			ASSERT_EQ(currentCumulativeMeterValue, GetCurrentCumulativeMeterValue(obj)) << "m_currentCumulativeMeterValue";
			ASSERT_EQ(hasCumulativeMeterAlready, obj.HasCumulativeMeterAlready()) << "HasCumulativeMeterAlready";
		}

		/// <summary>
		/// Functional test for:
		///     CumulativeProgressiveManager::ClearHasCumulativeMeterStatus()
		///     CumulativeProgressiveManager::CumulativeProgressiveManager()
		/// Test that ClearHasCumulativeMeterStatus() clears NVRAM when memoryType is MEMORY_NVRAM.
		/// </summary>
		TEST_F(CumulativeProgressiveManagerTests, ClearHasCumulativeMeterStatus_MemoryNvram)
		{
			// Setup.
			CumulativeProgressiveManager_PersistNonDefault();
			CUnitTestingUtils::SetNvramInvalid();

			// Perform operation(s) and test(s).
			{
				CumulativeProgressiveManager obj(MEMORY_NVRAM);
				obj.ClearHasCumulativeMeterStatus();
			}

			CumulativeProgressiveManager obj(MEMORY_NVRAM);
			ASSERT_EQ(m_hasCumulativeMeterAlreadyDefault, obj.HasCumulativeMeterAlready()) << "HasCumulativeMeterAlready";
			ASSERT_EQ(m_currentCumulativeMeterValueDefault, GetCurrentCumulativeMeterValue(obj)) << "m_currentCumulativeMeterValue";
		}
// #pragma endregion NVRAM
}
