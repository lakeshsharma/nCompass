#include "stdafx.h"
#include "ConfigUniverseEnabledField.h"

// Sentinel includes.
#include "Utilities.h"

// Unit testing includes.
#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	class ConfigUniverseEnabledField : public ::testing::Test
	{
	public:
	};

		/// <summary>
		/// Test the implementation of:
		///     CUniverseEnabledField::CUniverseEnabledField().
		/// </summary>
		TEST(ConfigUniverseEnabledField, UniverseEnabledFieldTest)
		{
			// Setup.
			const CUniverseEnabledField configField;
			const FieldTypes typeExpected = UNIVERSE_ENABLED;

			// Test #1.
			const FieldTypes typeResult = configField.GetType();

			{
				// Report #1.
				bool areEqual = typeResult == typeExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED typeResult=%d expected=%d in %s"),
					(int)typeResult,
					(int)typeExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Test the implementation of:
		///     CUniverseEnabledField::SetUniverseFields(),
		///     CUniverseEnabledField::GetUniverseEnabled(), and
		///     CUniverseEnabledField::GetMixedModeEnabled().
		/// </summary>
		TEST(ConfigUniverseEnabledField, SetUniverseFieldsTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_buffer;
				const int m_length;

				// Expected results.
				const bool m_universeEnabledExpected;
				const bool m_m_mixedModeEnabledExpected;
			} TestDataType;

			const byte buffer1[] = { 0, 0 };
			const byte buffer2[] = { 0, 1};
			const byte buffer3[] = { 1, 0 };
			const byte buffer4[] = { 1, 1 };
			const byte buffer5[] = { 2, 0 };
			const byte buffer6[] = { 0, 2 };
			const byte buffer7[] = { 0, 0, 1, 2 };
			const byte buffer8[] = { 0xff, 0xff };
			const byte buffer9[] = { 0xff, 0xfe };
			const byte buffer10[] = { 0xfe, 0xfe };
			const byte buffer11[] = { 0xfe, 0xff };
			const byte buffer12[] = { 0, 1, 2 };
			const byte buffer13[] = { 1, 2 , 3, 4};

			const TestDataType TEST_DATA[] =
			{
				{ NULL, 0, false, false },
				{ buffer1, sizeof(buffer1), false, false },
				{ buffer2, sizeof(buffer2), false, true },
				{ buffer3, sizeof(buffer3), true, false },
				{ buffer4, sizeof(buffer4), true, true },
				{ buffer5, sizeof(buffer5), false, false },
				{ buffer6, sizeof(buffer6), false, false },
				{ buffer7, sizeof(buffer7), false, false },
				{ buffer8, sizeof(buffer8), false, false },
				{ buffer9, sizeof(buffer9), false, false },
				{ buffer10, sizeof(buffer10), false, false },
				{ buffer11, sizeof(buffer11), false, false },
				{ buffer12, sizeof(buffer12), false, true },
				{ buffer13, sizeof(buffer13), true, false },
				{ NULL, sizeof(buffer13), false, false },
				{ NULL, -33, false, false },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CUniverseEnabledField varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetUniverseFields(TEST_DATA[testIndex].m_buffer, TEST_DATA[testIndex].m_length);
				const CUniverseEnabledField &configField(varConfigField);

				// Test #1.
				const bool universeEnabledResult = configField.GetUniverseEnabled();

				{
					// Report #1.
					bool areEqual = universeEnabledResult == TEST_DATA[testIndex].m_universeEnabledExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED SetUniverseFields() universeEnabledResult=%d expected=%d testIndex=%d in %s"),
						(int)universeEnabledResult,
						(int)TEST_DATA[testIndex].m_universeEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #2.
				const bool mixedModeResult = configField.GetMixedModeEnabled();

				{
					// Report #2.
					bool areEqual = mixedModeResult == TEST_DATA[testIndex].m_m_mixedModeEnabledExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED SetUniverseFields() mixedModeResult=%d expected=%d testIndex=%d in %s"),
						(int)mixedModeResult,
						(int)TEST_DATA[testIndex].m_m_mixedModeEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}
}
