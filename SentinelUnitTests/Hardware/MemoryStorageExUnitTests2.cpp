#include "stdafx.h"
#include "Hardware/MemoryStorageEx.h"

// Unit testing includes.
#include "gmock/gmock.h"
#include "UnitTestingUtils.h"

using namespace std;
using ::testing::_;
using ::testing::Invoke;

	/// <summary>
	/// Class intended primarily for GMock-related tests.
	/// </summary>
	class MemoryStorageEx2 : public ::testing::Test
	{
	public:
		// Example NVRAM memory map for testing CStorage.
		static const DWORD m_activeOffset = NVRAM_GAMES_OFFSET;
		static const BYTE m_activeValue = 6;
		static const DWORD m_dataOffset = m_activeOffset + 1;
		static const DWORD m_dataSize = 4;

		// Class member(s) used by test (data) methods.
		static string m_gmockFailure;

		class CTersePrinter;

		static CTersePrinter m_tersePrinter;

		MemoryStorageEx2();
		~MemoryStorageEx2();
		virtual void SetUp();
		virtual void TearDown();

		class CStorageMock1;
		class CStorageMock2;
		class CStorageMock3;
	};

// #pragma region NonGmockTests
		/// <summary>
		/// Ensure that the first test method of each module does NOT generate a gmock error.
		/// The build server can stop reporting before indicating that a gmock test failed, but indicate that the prior method passed.
		/// Therefore, it is very useful to ensure that the prior method is in the same module.
		/// </summary>
		TEST_F(MemoryStorageEx2, MemoryStorageEx2_NotNullTest)
		{
			// Type(s).
			class CStorageLocal : public CMemoryStorageEx
			{
			public:
				CStorageLocal() {}

				CStorageLocal(MEMORY_TYPE memoryType, DWORD dataOffset, DWORD dataSize, DWORD activeOffset, BYTE activeValue)
					:
					CMemoryStorageEx(memoryType, dataOffset, dataSize, activeOffset, activeValue)
				{
				}

				UINT GetNvramBuffer(BYTE *buffer, UINT bufferSize, bool& active) const
				{
					UNREFERENCED_PARAMETER(buffer);
					UNREFERENCED_PARAMETER(bufferSize);
					UNREFERENCED_PARAMETER(active);
					return 0;
				}
			};

			// Setup.
			CStorageLocal obj;

			// Test.
			ASSERT_NE((void *)nullptr, (void *)&obj) << _T("&obj");
		}
// #pragma endregion NonGmockTests

		class MemoryStorageEx2::CTersePrinter : public ::testing::EmptyTestEventListener {
			// Called after a failed assertion or a SUCCEED() invocation.
			virtual void OnTestPartResult(const ::testing::TestPartResult& test_part_result) {
				if (test_part_result.failed())
				{
					string failure;
					failure = FormatString("FAILED in %s:%d\n%s",
						test_part_result.file_name(),
						test_part_result.line_number(),
						test_part_result.summary());
					m_gmockFailure = failure;
				}
			}
		};

		/// <summary>
		/// Initialize and/or test static class members.
		/// </summary>
		MemoryStorageEx2::MemoryStorageEx2()
		{
			// Class initialization code.
			::testing::UnitTest::GetInstance()->listeners().Append(&m_tersePrinter);
		}

		/// <summary>
		/// Cleanup static class member(s).
		/// </summary>
		MemoryStorageEx2::~MemoryStorageEx2()
		{
			// Class cleanup code.
			::testing::UnitTest::GetInstance()->listeners().Release(&m_tersePrinter);
		}

		/// <summary>
		/// Initialize class member(s) before each test method is run.
		/// </summary>
		void MemoryStorageEx2::SetUp()
		{
			// Test method initialization code.
			m_gmockFailure.clear();
		}

		/// <summary>
		/// Cleanup class member(s) before each test method is run.
		/// </summary>
		void MemoryStorageEx2::TearDown()
		{
			// Report a gmock failure reported by our event listener, if one occurred.
			if (!m_gmockFailure.empty())
			{
				ASSERT_TRUE(false) << m_gmockFailure;
			}
		}

// #pragma region BuildYourself1
		// Use a concrete class with a 1-parameter BuildYourself() method to test CMemoryStorageEx.
		class MemoryStorageEx2::CStorageMock1 : public CMemoryStorageEx
		{
		public:
			CStorageMock1() {}

			CStorageMock1(MEMORY_TYPE memoryType, DWORD dataOffset, DWORD dataSize, DWORD activeOffset, BYTE activeValue)
				:
				CMemoryStorageEx(memoryType, dataOffset, dataSize, activeOffset, activeValue)
			{
			}

			MOCK_METHOD1(BuildYourself, void(bool));

			UINT GetNvramBuffer(BYTE *buffer, UINT bufferSize, bool& active) const
			{
				UNREFERENCED_PARAMETER(buffer);
				UNREFERENCED_PARAMETER(bufferSize);
				UNREFERENCED_PARAMETER(active);
				return 0;
			}
		};

		/// <summary>
		/// Functional test for:
		///     CMemoryStorageEx::BuildYourself()
		///  Verify that the 1-parameter BuildYourself() mock function is called as expected fromt the 0-parameter BuildYourself() function.
		/// </summary>
		TEST_F(MemoryStorageEx2, BuildYourself1_BuildYourself0Test)
		{
			// Data.
			const bool buildInactive(false);

			// Setup.
			CStorageMock1 obj;
			EXPECT_CALL(obj, BuildYourself(testing::_)).Times(0);
			EXPECT_CALL(obj, BuildYourself(buildInactive)).Times(1);

			// Perform operation(s) and test(s).
			obj.CMemoryStorageEx::BuildYourself();
		}
// #pragma endregion BuildYourself1

// #pragma region SetFromNvramBuffer2
		// Use a concrete class with a 2-parameter SetFromNvramBuffer() to test CMemoryStorageEx.
		class MemoryStorageEx2::CStorageMock2 : public CMemoryStorageEx
		{
		public:
			CStorageMock2() {}

			CStorageMock2(MEMORY_TYPE memoryType, DWORD dataOffset, DWORD dataSize, DWORD activeOffset, BYTE activeValue)
				:
				CMemoryStorageEx(memoryType, dataOffset, dataSize, activeOffset, activeValue)
			{
			}

			MOCK_METHOD2(SetFromNvramBuffer, void(const BYTE *, UINT));

			UINT GetNvramBuffer(BYTE *buffer, UINT bufferSize, bool& active) const
			{
				UNREFERENCED_PARAMETER(buffer);
				UNREFERENCED_PARAMETER(bufferSize);
				UNREFERENCED_PARAMETER(active);
				return 0;
			}
		};

		/// <summary>
		/// Functional test for:
		///     CMemoryStorageEx::SetFromNvramBuffer(const BYTE *buffer, UINT bufferSize)
		///  Verify that the 2-parameter SetFromNvramBuffer() mock function is called as expected.
		/// </summary>
		TEST_F(MemoryStorageEx2, SetFromNvramBuffer2_MockTest)
		{
			// Data.
			const UINT bufferSize(123);
			const BYTE buffer[bufferSize] = {};

			// Setup.
			CStorageMock2 obj;
			EXPECT_CALL(obj, SetFromNvramBuffer(testing::_, testing::_)).Times(0);
			EXPECT_CALL(obj, SetFromNvramBuffer(buffer, bufferSize)).Times(1);

			// Perform operation(s) and test(s).
			obj.SetFromNvramBuffer(buffer, bufferSize);
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorageEx::SetFromNvramBuffer(const BYTE *buffer, UINT bufferSize)
		///  Verify that the 2-parameter SetFromNvramBuffer() mock function is called
		/// from the 3-parameter SetFromNvramBuffer() function when active is true.
		/// </summary>
		TEST_F(MemoryStorageEx2, SetFromNvramBuffer2_SetFromNvramBuffer3ActiveTrueTest)
		{
			// Data.
			const UINT bufferSize(123);
			const BYTE buffer[bufferSize] = {};
			const bool active(true);

			// Setup.
			CStorageMock2 obj;
			EXPECT_CALL(obj, SetFromNvramBuffer(testing::_, testing::_)).Times(0);
			EXPECT_CALL(obj, SetFromNvramBuffer(buffer, bufferSize)).Times(1);

			// Perform operation(s) and test(s).
			obj.CMemoryStorageEx::SetFromNvramBuffer(buffer, bufferSize, active);
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorageEx::SetFromNvramBuffer(const BYTE *buffer, UINT bufferSize)
		///  Verify that the 2-parameter SetFromNvramBuffer() mock function is not called
		/// from the 3-parameter SetFromNvramBuffer() function when active is false.
		/// </summary>
		TEST_F(MemoryStorageEx2, SetFromNvramBuffer2_SetFromNvramBuffer3ActiveFalseTest)
		{
			// Data.
			const UINT bufferSize(123);
			const BYTE buffer[bufferSize] = {};
			const bool active(false);

			// Setup.
			CStorageMock2 obj;
			EXPECT_CALL(obj, SetFromNvramBuffer(testing::_, testing::_)).Times(0);
			EXPECT_CALL(obj, SetFromNvramBuffer(buffer, bufferSize)).Times(0);

			// Perform operation(s) and test(s).
			obj.CMemoryStorageEx::SetFromNvramBuffer(buffer, bufferSize, active);
		}
// #pragma endregion SetFromNvramBuffer2

// #pragma region SetFromNvramBuffer3
		// Use a concrete class with a 3-parameter SetFromNvramBuffer() to test CMemoryStorageEx.
		class MemoryStorageEx2::CStorageMock3 : public CMemoryStorageEx
		{
		public:
			CStorageMock3() {}

			CStorageMock3(MEMORY_TYPE memoryType, DWORD dataOffset, DWORD dataSize, DWORD activeOffset, BYTE activeValue)
				:
				CMemoryStorageEx(memoryType, dataOffset, dataSize, activeOffset, activeValue)
			{
			}

			MOCK_METHOD3(SetFromNvramBuffer, void(const BYTE *, UINT, bool));

			UINT GetNvramBuffer(BYTE *buffer, UINT bufferSize, bool& active) const
			{
				UNREFERENCED_PARAMETER(buffer);
				UNREFERENCED_PARAMETER(bufferSize);
				UNREFERENCED_PARAMETER(active);
				return 0;
			}
		};

		/// <summary>
		/// Functional test for:
		///     CMemoryStorageEx::SetFromNvramBuffer(const BYTE *buffer, UINT bufferSize)
		///  Verify that the 3-parameter SetFromNvramBuffer() mock function is called as expected.
		/// </summary>
		TEST_F(MemoryStorageEx2, SetFromNvramBuffer3_MockTest)
		{
			const UINT bufferSize(123);
			const BYTE buffer[bufferSize] = {};
			const bool active(true);

			// Setup.
			CStorageMock3 obj;
			EXPECT_CALL(obj, SetFromNvramBuffer(testing::_, testing::_, testing::_)).Times(0);
			EXPECT_CALL(obj, SetFromNvramBuffer(buffer, bufferSize, active)).Times(1);

			// Perform operation(s) and test(s).
			obj.SetFromNvramBuffer(buffer, bufferSize, active);
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorageEx::BuildYourself()
		///  Verify that the 3-parameter SetFromNvramBuffer() mock function is called from the BuildYourself() function when location is active.
		/// </summary>
		TEST_F(MemoryStorageEx2, SetFromNvramBuffer3_BuildYourself_LocationActiveTest)
		{
			// Data.
			const UINT bufferSize(123);
			const BYTE buffer[bufferSize] = {};
			const bool active(true);

			// Setup.
			CUnitTestingUtils::SetNvramValidAndUpgraded();
			CMemoryStorage::SetInactiveLocation(m_activeOffset, active ? m_activeValue : (BYTE)~m_activeValue);

			CStorageMock3 obj(MEMORY_NVRAM, m_dataOffset, m_dataSize, m_activeOffset, m_activeValue);
			EXPECT_CALL(obj, SetFromNvramBuffer(testing::_, testing::_, testing::_)).Times(0);
			EXPECT_CALL(obj, SetFromNvramBuffer(testing::Ne(nullptr), m_dataSize, active)).Times(1);

			// Perform operation(s) and test(s).
			obj.CMemoryStorageEx::BuildYourself();
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorageEx::BuildYourself(bool)
		///  Verify that the 3-parameter SetFromNvramBuffer() mock function is called from the BuildYourself(false) function when location is active.
		/// </summary>
		TEST_F(MemoryStorageEx2, SetFromNvramBuffer3_BuildYourselfFalse_LocationActiveTest)
		{
			// Data.
			const UINT bufferSize(123);
			const BYTE buffer[bufferSize] = {};
			const bool active(true);

			// Setup.
			CUnitTestingUtils::SetNvramValidAndUpgraded();
			CMemoryStorage::SetInactiveLocation(m_activeOffset, active ? m_activeValue : (BYTE)~m_activeValue);

			CStorageMock3 obj(MEMORY_NVRAM, m_dataOffset, m_dataSize, m_activeOffset, m_activeValue);
			EXPECT_CALL(obj, SetFromNvramBuffer(testing::_, testing::_, testing::_)).Times(0);
			EXPECT_CALL(obj, SetFromNvramBuffer(testing::Ne(nullptr), m_dataSize, active)).Times(1);

			// Perform operation(s) and test(s).
			obj.CMemoryStorageEx::BuildYourself(false);
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorageEx::BuildYourself(bool)
		///  Verify that the 3-parameter SetFromNvramBuffer() mock function is called from the BuildYourself(true) function when location is active.
		/// </summary>
		TEST_F(MemoryStorageEx2, SetFromNvramBuffer3_BuildYourselfTrue_LocationActiveTest)
		{
			// Data.
			const UINT bufferSize(123);
			const BYTE buffer[bufferSize] = {};
			const bool active(true);

			// Setup.
			CUnitTestingUtils::SetNvramValidAndUpgraded();
			CMemoryStorage::SetInactiveLocation(m_activeOffset, active ? m_activeValue : (BYTE)~m_activeValue);

			CStorageMock3 obj(MEMORY_NVRAM, m_dataOffset, m_dataSize, m_activeOffset, m_activeValue);
			EXPECT_CALL(obj, SetFromNvramBuffer(testing::_, testing::_, testing::_)).Times(0);
			EXPECT_CALL(obj, SetFromNvramBuffer(testing::Ne(nullptr), m_dataSize, active)).Times(1);

			// Perform operation(s) and test(s).
			obj.CMemoryStorageEx::BuildYourself(true);
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorageEx::BuildYourself()
		///  Verify that the 3-parameter SetFromNvramBuffer() mock function is not called from the BuildYourself() function when location is inactive.
		/// </summary>
		TEST_F(MemoryStorageEx2, SetFromNvramBuffer3_BuildYourself_LocationInactiveTest)
		{
			// Data.
			const UINT bufferSize(123);
			const BYTE buffer[bufferSize] = {};
			const bool active(false);

			// Setup.
			CUnitTestingUtils::SetNvramValidAndUpgraded();
			CMemoryStorage::SetInactiveLocation(m_activeOffset, active ? m_activeValue : (BYTE)~m_activeValue);

			CStorageMock3 obj(MEMORY_NVRAM, m_dataOffset, m_dataSize, m_activeOffset, m_activeValue);
			EXPECT_CALL(obj, SetFromNvramBuffer(testing::_, testing::_, testing::_)).Times(0);

			// Perform operation(s) and test(s).
			obj.CMemoryStorageEx::BuildYourself();
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorageEx::BuildYourself(bool)
		///  Verify that the 3-parameter SetFromNvramBuffer() mock function is not called from the BuildYourself(false) function when location is inactive.
		/// </summary>
		TEST_F(MemoryStorageEx2, SetFromNvramBuffer3_BuildYourselfFalse_LocationInactiveTest)
		{
			// Data.
			const UINT bufferSize(123);
			const BYTE buffer[bufferSize] = {};
			const bool active(false);

			// Setup.
			CUnitTestingUtils::SetNvramValidAndUpgraded();
			CMemoryStorage::SetInactiveLocation(m_activeOffset, active ? m_activeValue : (BYTE)~m_activeValue);

			CStorageMock3 obj(MEMORY_NVRAM, m_dataOffset, m_dataSize, m_activeOffset, m_activeValue);
			EXPECT_CALL(obj, SetFromNvramBuffer(testing::_, testing::_, testing::_)).Times(0);

			// Perform operation(s) and test(s).
			obj.CMemoryStorageEx::BuildYourself(false);
		}
		/// <summary>
		/// Functional test for:
		///     CMemoryStorageEx::BuildYourself(bool)
		///  Verify that the 3-parameter SetFromNvramBuffer() mock function is called from the BuildYourself(true) function when location is inactive.
		/// </summary>
		TEST_F(MemoryStorageEx2, SetFromNvramBuffer3_BuildYourselfActiveFalseTest)
		{
			// Data.
			const UINT bufferSize(123);
			const BYTE buffer[bufferSize] = {};
			const bool active(false);

			// Setup.
			CUnitTestingUtils::SetNvramValidAndUpgraded();
			CMemoryStorage::SetInactiveLocation(m_activeOffset, active ? m_activeValue : (BYTE)~m_activeValue);

			CStorageMock3 obj(MEMORY_NVRAM, m_dataOffset, m_dataSize, m_activeOffset, m_activeValue);
			EXPECT_CALL(obj, SetFromNvramBuffer(testing::_, testing::_, testing::_)).Times(0);
			EXPECT_CALL(obj, SetFromNvramBuffer(testing::Ne(nullptr), m_dataSize, active)).Times(1);

			// Perform operation(s) and test(s).
			obj.CMemoryStorageEx::BuildYourself(true);
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorageEx::BuildYourself(bool)
		///  Verify that the 3-parameter SetFromNvramBuffer() mock function is not called from the BuildYourself() function when NVRAM is invalid
		///  and that the location is set to not active.
		/// </summary>
		TEST_F(MemoryStorageEx2, SetFromNvramBuffer3_BuildYourself_NvramInvalidTest)
		{
			// Data.
			const UINT bufferSize(123);
			const BYTE buffer[bufferSize] = {};
			const bool active(true);

			// Setup.
			CUnitTestingUtils::SetNvramInvalid();
			CMemoryStorage::SetInactiveLocation(m_activeOffset, active ? m_activeValue : (BYTE)~m_activeValue);

			CStorageMock3 obj(MEMORY_NVRAM, m_dataOffset, m_dataSize, m_activeOffset, m_activeValue);
			EXPECT_EQ(true, obj.IsActiveLocation(obj.m_activeOffset, obj.m_activeValue)); // Verify setup.
			EXPECT_CALL(obj, SetFromNvramBuffer(testing::_, testing::_, testing::_)).Times(0);

			// Perform operation(s) and test(s).
			obj.CMemoryStorageEx::BuildYourself();
			EXPECT_FALSE(obj.IsActiveLocation(obj.m_activeOffset, obj.m_activeValue));
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorageEx::BuildYourself(bool)
		///  Verify that the 3-parameter SetFromNvramBuffer() mock function is not called from the BuildYourself(false) function when NVRAM is invalid
		///  and that the location is set to not active.
		/// </summary>
		TEST_F(MemoryStorageEx2, SetFromNvramBuffer3_BuildYourselfFalse_NvramInvalidTest)
		{
			// Data.
			const UINT bufferSize(123);
			const BYTE buffer[bufferSize] = {};
			const bool active(true);

			// Setup.
			CUnitTestingUtils::SetNvramInvalid();
			CMemoryStorage::SetInactiveLocation(m_activeOffset, active ? m_activeValue : (BYTE)~m_activeValue);

			CStorageMock3 obj(MEMORY_NVRAM, m_dataOffset, m_dataSize, m_activeOffset, m_activeValue);
			EXPECT_EQ(true, obj.IsActiveLocation(obj.m_activeOffset, obj.m_activeValue)); // Verify setup.
			EXPECT_CALL(obj, SetFromNvramBuffer(testing::_, testing::_, testing::_)).Times(0);

			// Perform operation(s) and test(s).
			obj.CMemoryStorageEx::BuildYourself(false);
			EXPECT_FALSE(obj.IsActiveLocation(obj.m_activeOffset, obj.m_activeValue));
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorageEx::BuildYourself(bool)
		///  Verify that the 3-parameter SetFromNvramBuffer() mock function is not called from the BuildYourself(true) function when NVRAM is invalid
		///  and that the location is set to not active.
		/// </summary>
		TEST_F(MemoryStorageEx2, SetFromNvramBuffer3_BuildYourselfTrue_NvramInvalidTest)
		{
			// Data.
			const UINT bufferSize(123);
			const BYTE buffer[bufferSize] = {};
			const bool active(true);

			// Setup.
			CUnitTestingUtils::SetNvramInvalid();
			CMemoryStorage::SetInactiveLocation(m_activeOffset, active ? m_activeValue : (BYTE)~m_activeValue);

			CStorageMock3 obj(MEMORY_NVRAM, m_dataOffset, m_dataSize, m_activeOffset, m_activeValue);
			EXPECT_EQ(true, obj.IsActiveLocation(obj.m_activeOffset, obj.m_activeValue)); // Verify setup.
			EXPECT_CALL(obj, SetFromNvramBuffer(testing::_, testing::_, testing::_)).Times(0);

			// Perform operation(s) and test(s).
			obj.CMemoryStorageEx::BuildYourself(true);
			EXPECT_FALSE(obj.IsActiveLocation(obj.m_activeOffset, obj.m_activeValue));
		}
// #pragma endregion SetFromNvramBuffer3

	string MemoryStorageEx2::m_gmockFailure;
	MemoryStorageEx2::CTersePrinter MemoryStorageEx2::m_tersePrinter;
