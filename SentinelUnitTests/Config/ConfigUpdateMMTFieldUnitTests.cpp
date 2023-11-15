#include "stdafx.h"
#include "ConfigUpdateMMTField.h"

// Sentinel includes.
#include "Utilities.h"

// Unit testing includes.
#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	class ConfigUpdateMMTField : public ::testing::Test
	{
	public:
	};

		// Test the implementation of:
		//     CConfigUpdateMMTField::CConfigUpdateMMTField().
		TEST(ConfigUpdateMMTField, ConfigUpdateMMTFieldTest)
		{
			// Setup.
			const CConfigUpdateMMTField configField;
			const FieldTypes typeExpected = SEND_FULL_METER_UPDATE;

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

		// SetUpdateMMTTest
		// Test the implementation of:
		//     CConfigUpdateMMTField::SetUpdateMMT().
		//     CConfigUpdateMMTField::GetUpdateMMT().
		TEST(ConfigUpdateMMTField, SetUpdateMMTTest)
		{
			typedef struct
			{
				// Inputs.
				const bool m_updateMMT;

				// Expected results.
				const bool m_updateMMTExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ false, false },
				{ true, true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CConfigUpdateMMTField varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetUpdateMMT(TEST_DATA[testIndex].m_updateMMT);
				const CConfigUpdateMMTField &configField(varConfigField);

				// Test #1.
				const bool updateMMTResult = configField.GetUpdateMMT();

				{
					// Report #1.
					bool areEqual = updateMMTResult == TEST_DATA[testIndex].m_updateMMTExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED GetUpdateMMT()=%d expected=%d testIndex=%d in %s"),
						(int)updateMMTResult,
						(int)TEST_DATA[testIndex].m_updateMMTExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}
}
