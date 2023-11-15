#include "stdafx.h"
#include "Hardware/MemoryStorageEx.h"

// Unit testing includes.
#include "gtest/gtest.h"
#include "AssertMsg.h"
#if 000//TODO-PORT
#include "UnitTestingToString.h"
#endif//TODO-PORT
#include "UnitTestingUtils.h"

using namespace std;

	class MemoryStorageEx : public ::testing::Test
	{
	public:

		// Use a concrete class to test CMemoryStorageEx.
		class CStorage : public CMemoryStorageEx
		{
		public:
			CStorage() {}

			CStorage(MEMORY_TYPE memoryType, DWORD dataOffset, DWORD dataSize, DWORD activeOffset, BYTE activeValue)
				:
				CMemoryStorageEx(memoryType, dataOffset, dataSize, activeOffset, activeValue)
			{
			}

			void SetFromNvramBuffer(const BYTE *buffer, UINT bufferSize)
			{
				UNREFERENCED_PARAMETER(buffer);
				UNREFERENCED_PARAMETER(bufferSize);
			}

			UINT GetNvramBuffer(BYTE *buffer, UINT bufferSize, bool& active) const
			{
				UNREFERENCED_PARAMETER(buffer);
				UNREFERENCED_PARAMETER(bufferSize);
				UNREFERENCED_PARAMETER(active);
				return 0;
			}
		};

		// Example NVRAM memory map for testing CStorage.
		static const DWORD m_activeOffset = NVRAM_GAMES_OFFSET;
		static const BYTE m_activeValue = 6;
		static const DWORD m_dataOffset = m_activeOffset + 1;
		static const DWORD m_dataSize = 4;
	};

// #pragma region MemoryStorageParameters
		/// <summary>
		/// Functional test for:
		///     CMemoryStorageEx::CMemoryStorageEx()
		/// Test for default value(s).
		/// </summary>
		TEST_F(MemoryStorageEx, SetMemoryStorageParameters_Default)
		{
			// Data.
			const MEMORY_TYPE memoryTypeExpected(MEMORY_NONE);
			const DWORD nvramDataOffsetExpected(CMemoryStorage::NVRAM_DEFAULT_OFFSET);
			const DWORD nvramDataSizeExpected(CMemoryStorage::NVRAM_DEFAULT_SIZE);
			const DWORD nvramActiveOffsetExpected(CMemoryStorage::NVRAM_DEFAULT_OFFSET);
			const BYTE nvramActiveValueExpected(CMemoryStorageEx::NVRAM_DEFAULT_ACTIVE_VALUE);

			// Perform operation(s) and test(s).
			CStorage obj;
			ASSERT_EQ(memoryTypeExpected, obj.m_type) << L"obj.m_type";
			ASSERT_EQ(nvramDataOffsetExpected, obj.m_dwOffset) << L"obj.m_dwOffset";
			ASSERT_EQ(nvramDataSizeExpected, obj.m_dwAllowedSize) << L"obj.m_dwAllowedSize";
			ASSERT_EQ(nvramActiveOffsetExpected, obj.m_activeOffset) << L"obj.m_activeOffset";
			ASSERT_EQ(nvramActiveValueExpected, obj.m_activeValue) << L"obj.m_activeValue";
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorageEx::CMemoryStorageEx(MEMORY_TYPE ...)
		/// Test for non-default value(s) set using a constructor.
		/// </summary>
		TEST_F(MemoryStorageEx, SetMemoryStorageParameters_CMemoryStorageEx)
		{
			// Data.
			const MEMORY_TYPE memoryType(MEMORY_NVRAM);
			const DWORD nvramDataOffset(3);
			const DWORD nvramDataSize(4);
			const DWORD nvramActiveOffset(5);
			const BYTE nvramActiveValue(6);

			const MEMORY_TYPE memoryTypeExpected(memoryType);
			const DWORD nvramDataOffsetExpected(nvramDataOffset);
			const DWORD nvramDataSizeExpected(nvramDataSize);
			const DWORD nvramActiveOffsetExpected(nvramActiveOffset);
			const BYTE nvramActiveValueExpected(nvramActiveValue);

			// Perform operation(s) and test(s).
			CStorage obj(memoryType, nvramDataOffset, nvramDataSize, nvramActiveOffset, nvramActiveValue);
			ASSERT_EQ(memoryTypeExpected, obj.m_type) << L"obj.m_type";
			ASSERT_EQ(nvramDataOffsetExpected, obj.m_dwOffset) << L"obj.m_dwOffset";
			ASSERT_EQ(nvramDataSizeExpected, obj.m_dwAllowedSize) << L"obj.m_dwAllowedSize";
			ASSERT_EQ(nvramActiveOffsetExpected, obj.m_activeOffset) << L"obj.m_activeOffset";
			ASSERT_EQ(nvramActiveValueExpected, obj.m_activeValue) << L"obj.m_activeValue";
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorageEx::SetMemoryStorageParameters()
		/// Test for non-default value(s).
		/// </summary>
		TEST_F(MemoryStorageEx, SetMemoryStorageParameters)
		{
			// Data.
			const MEMORY_TYPE memoryType(MEMORY_NVRAM);
			const DWORD nvramDataOffset(3);
			const DWORD nvramDataSize(4);
			const DWORD nvramActiveOffset(5);
			const BYTE nvramActiveValue(6);

			const MEMORY_TYPE memoryTypeExpected(memoryType);
			const DWORD nvramDataOffsetExpected(nvramDataOffset);
			const DWORD nvramDataSizeExpected(nvramDataSize);
			const DWORD nvramActiveOffsetExpected(nvramActiveOffset);
			const BYTE nvramActiveValueExpected(nvramActiveValue);

			// Setup.
			CStorage obj;

			// Perform operation(s) and test(s).
			obj.SetMemoryStorageParameters(memoryType, nvramDataOffset, nvramDataSize, nvramActiveOffset, nvramActiveValue);
			ASSERT_EQ(memoryTypeExpected, obj.m_type) << L"obj.m_type";
			ASSERT_EQ(nvramDataOffsetExpected, obj.m_dwOffset) << L"obj.m_dwOffset";
			ASSERT_EQ(nvramDataSizeExpected, obj.m_dwAllowedSize) << L"obj.m_dwAllowedSize";
			ASSERT_EQ(nvramActiveOffsetExpected, obj.m_activeOffset) << L"obj.m_activeOffset";
			ASSERT_EQ(nvramActiveValueExpected, obj.m_activeValue) << L"obj.m_activeValue";
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorageEx::ClearMemoryStorageParameters()
		/// Test for default value(s) after clearing.
		/// </summary>
		TEST_F(MemoryStorageEx, ClearMemoryStorageParameters)
		{
			// Data.
			const MEMORY_TYPE memoryType(MEMORY_NVRAM);
			const DWORD nvramDataOffset(3);
			const DWORD nvramDataSize(4);
			const DWORD nvramActiveOffset(5);
			const BYTE nvramActiveValue(6);

			const MEMORY_TYPE memoryTypeExpected(MEMORY_NONE);
			const DWORD nvramDataOffsetExpected(CMemoryStorage::NVRAM_DEFAULT_OFFSET);
			const DWORD nvramDataSizeExpected(CMemoryStorage::NVRAM_DEFAULT_SIZE);
			const DWORD nvramActiveOffsetExpected(CMemoryStorage::NVRAM_DEFAULT_OFFSET);
			const BYTE nvramActiveValueExpected(CMemoryStorageEx::NVRAM_DEFAULT_ACTIVE_VALUE);

			// Setup.
			CStorage obj;
			obj.SetMemoryStorageParameters(memoryType, nvramDataOffset, nvramDataSize, nvramActiveOffset, nvramActiveValue);

			// Verify setup.
			ASSERT_EQ(memoryType, obj.m_type) << L"obj.m_type";
			ASSERT_EQ(nvramDataOffset, obj.m_dwOffset) << L"obj.m_dwOffset";
			ASSERT_EQ(nvramDataSize, obj.m_dwAllowedSize) << L"obj.m_dwAllowedSize";
			ASSERT_EQ(nvramActiveOffset, obj.m_activeOffset) << L"obj.m_activeOffset";
			ASSERT_EQ(nvramActiveValue, obj.m_activeValue) << L"obj.m_activeValue";

			// Perform operation(s) and test(s).
			obj.ClearMemoryStorageParameters();
			ASSERT_EQ(memoryTypeExpected, obj.m_type) << L"obj.m_type";
			ASSERT_EQ(nvramDataOffsetExpected, obj.m_dwOffset) << L"obj.m_dwOffset";
			ASSERT_EQ(nvramDataSizeExpected, obj.m_dwAllowedSize) << L"obj.m_dwAllowedSize";
			ASSERT_EQ(nvramActiveOffsetExpected, obj.m_activeOffset) << L"obj.m_activeOffset";
			ASSERT_EQ(nvramActiveValueExpected, obj.m_activeValue) << L"obj.m_activeValue";
		}
// #pragma endregion MemoryStorageParameters

// #pragma region GetNvramActiveOffset
		/// <summary>
		/// Functional test for:
		///     CMemoryStorageEx::CMemoryStorageEx()
		///     CMemoryStorageEx::GetNvramActiveOffset()
		/// Test for default value(s).
		/// </summary>
		TEST_F(MemoryStorageEx, GetNvramActiveOffset_Default)
		{
			// Data.
			const DWORD nvramActiveOffsetExpected(CMemoryStorage::NVRAM_DEFAULT_OFFSET);

			// Perform operation(s) and test(s).
			CStorage obj;
			ASSERT_EQ(nvramActiveOffsetExpected, obj.GetNvramActiveOffset()) << L"obj.GetNvramActiveOffset()";
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorageEx::SetMemoryStorageParameters()
		///     CMemoryStorageEx::GetNvramActiveOffset()
		/// Test for non-default value.
		/// </summary>
		TEST_F(MemoryStorageEx, GetNvramActiveOffset_SetMemoryStorageParameters)
		{
			// Data.
			const DWORD nvramDataOffset(3);
			const DWORD nvramDataSize(4);
			const DWORD nvramActiveOffset(5);
			const BYTE nvramActiveValue(6);

			const DWORD nvramActiveOffsetExpected(nvramActiveOffset);

			// Setup.
			CStorage obj;

			// Perform operation(s) and test(s).
			obj.SetMemoryStorageParameters(MEMORY_NVRAM, nvramDataOffset, nvramDataSize, nvramActiveOffset, nvramActiveValue);
			ASSERT_EQ(nvramActiveOffsetExpected, obj.GetNvramActiveOffset()) << L"obj.GetNvramActiveOffset()";
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorageEx::SetMemoryStorageParameters()
		///     CMemoryStorageEx::GetNvramActiveOffset()
		/// Test for default value.
		/// </summary>
		TEST_F(MemoryStorageEx, GetNvramActiveOffset_ClearMemoryStorageParameters)
		{
			// Data.
			const DWORD nvramDataOffset(3);
			const DWORD nvramDataSize(4);
			const DWORD nvramActiveOffset(5);
			const BYTE nvramActiveValue(6);

			const DWORD nvramActiveOffsetExpected(CMemoryStorage::NVRAM_DEFAULT_OFFSET);

			// Setup.
			CStorage obj;
			obj.SetMemoryStorageParameters(MEMORY_NVRAM, nvramDataOffset, nvramDataSize, nvramActiveOffset, nvramActiveValue);

			// Verify setup.
			ASSERT_EQ(nvramActiveOffset, obj.m_activeOffset) << L"obj.m_activeOffset";

			// Perform operation(s) and test(s).
			obj.ClearMemoryStorageParameters();
			ASSERT_EQ(nvramActiveOffsetExpected, obj.GetNvramActiveOffset()) << L"obj.GetNvramActiveOffset()";
		}
// #pragma endregion GetNvramActiveOffset


// #pragma region ActiveLocation
		/// <summary>
		/// Functional test for:
		///     CMemoryStorageEx::CMemoryStorageEx()
		///     CMemoryStorage::SetActiveLocation()
		/// Test for active value in the active location.
		/// </summary>
		TEST_F(MemoryStorageEx, SetActiveLocation_True)
		{
			// Setup.
			CMemoryStorage::SetInactiveLocation(m_activeOffset, (BYTE)(m_activeValue + 55)); // Set the active location to a different value.

			// Perform operation(s) and test(s).
			CStorage obj(MEMORY_NVRAM, m_dataOffset, m_dataSize, m_activeOffset, m_activeValue);
			obj.SetActiveLocation(true);
			ASSERT_EQ(true, CMemoryStorage::IsActiveLocation(m_activeOffset, m_activeValue)) << L"IsActiveLocation";
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorageEx::CMemoryStorageEx()
		///     CMemoryStorage::SetActiveLocation()
		/// Test for inactive value in the active location.
		/// </summary>
		TEST_F(MemoryStorageEx, SetActiveLocation_False)
		{
			// Setup.
			CMemoryStorage::SetInactiveLocation(m_activeOffset, m_activeValue); // Set the active location to active.

			// Perform operation(s) and test(s).
			CStorage obj(MEMORY_NVRAM, m_dataOffset, m_dataSize, m_activeOffset, m_activeValue);
			obj.SetActiveLocation(false);
			ASSERT_FALSE(CMemoryStorage::IsActiveLocation(m_activeOffset, m_activeValue)) << L"IsActiveLocation";
		}
// #pragma endregion ActiveLocation

// #pragma region NvramMap
		/// <summary>
		/// Test that the NVRAM memory map has not changed across versions.
		/// If the NVRAM memory map does change across versions then memory map upgrades should be tested.
		/// </summary>
		TEST_F(MemoryStorageEx, CMemoryStorage_NvramMap)
		{
			// Test NVRAM memory map for backwards and forwards memory compatibility.
			ASSERT_EQ((DWORD)0, NVRAM_MAP_VERSION_OFFSET) << L"NVRAM_MAP_VERSION_OFFSET";
			ASSERT_EQ((DWORD)4, NVRAM_MAP_VERSION_SIZE) << L"NVRAM_MAP_VERSION_SIZE";
			ASSERT_EQ((DWORD)4, SPRINKLE1_OFFSET) << L"SPRINKLE1_OFFSET";
		}
// #pragma endregion NvramMap

// #pragma region BuildYourself
		/// <summary>
		/// Functional test for:
		///     CMemoryStorageEx::SetMemoryStorageParameters()
		///     CMemoryStorageEx::BuildYourself()
		/// Test for BuildYourself() setting the active location to inactive when the NVRAM integrity check fails.
		/// </summary>
		TEST_F(MemoryStorageEx, BuildYourself_IntegrityCheckFailed)
		{
			// Setup.
			CUnitTestingUtils::SetNvramInvalid();
			CMemoryStorage::SetInactiveLocation(m_activeOffset, m_activeValue); // Set the active location to active.

			CStorage obj;
			obj.SetMemoryStorageParameters(MEMORY_NVRAM, m_dataOffset, m_dataSize, m_activeOffset, m_activeValue);

			// Perform operation(s) and test(s).
			obj.BuildYourself();
			ASSERT_FALSE(CMemoryStorage::IsActiveLocation(m_activeOffset, m_activeValue)) << L"IsActiveLocation";
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorageEx::SetMemoryStorageParameters()
		///     CMemoryStorageEx::BuildYourself()
		/// Test for BuildYourself() leaving the active location as active when the NVRAM integrity check passes.
		/// </summary>
		TEST_F(MemoryStorageEx, BuildYourself_IntegrityCheckPassed)
		{
			// Setup.
			CUnitTestingUtils::SetNvramValidAndUpgraded();
			CMemoryStorage::SetInactiveLocation(m_activeOffset, m_activeValue); // Set the active location to active.

			CStorage obj;
			obj.SetMemoryStorageParameters(MEMORY_NVRAM, m_dataOffset, m_dataSize, m_activeOffset, m_activeValue);

			// Perform operation(s) and test(s).
			obj.BuildYourself();
			ASSERT_EQ(true, CMemoryStorage::IsActiveLocation(m_activeOffset, m_activeValue)) << L"IsActiveLocation";
		}
// #pragma endregion BuildYourself
