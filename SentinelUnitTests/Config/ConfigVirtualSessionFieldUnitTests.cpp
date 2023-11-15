#include "stdafx.h"
#include "ConfigVirtualSessionField.h"

// Sentinel includes.
#include "Utilities.h"

// Unit testing includes.
#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	static const byte MEMSET_DEFAULT_VALUE = 0xcd; // Value unlikely to be used for other purposes.

	class ConfigVirtualSessionField : public ::testing::Test
	{
	public:
	};

		// ConfigVirtualSessionFieldTest
		// Test the implementation of:
		//     CConfigVirtualSessionField::CConfigVirtualSessionField().
		TEST(ConfigVirtualSessionField, ConfigVirtualSessionFieldTest)
		{
			// Setup.
			const CConfigVirtualSessionField configField;
			const FieldTypes typeExpected = VIRTUAL_SESSIONS;

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

		// SetVirtualSessionOptionsTest
		// Test the implementation of:
		//     CConfigVirtualSessionField::SetVirtualSessionOptions().
		//     CConfigVirtualSessionField::GetVirtualSessionNoPlayTimeout().
		//     CConfigVirtualSessionField::GetVirtualSessionMaxDuration().
		TEST(ConfigVirtualSessionField, SetVirtualSessionOptionsTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_buffer;
				const int m_length;

				// Expected results.
				const DWORD m_virtualSessionNoPlayTimeoutExpected;
				const DWORD m_virtualSessionMaxDurationExpected;
			} TestDataType;

			const DWORD buffer0[] = { 0, 1 };
			const DWORD buffer1[] = { 1, 0 };
			const DWORD buffer2[] = { 0xffffffff, 0xfffffffe };
			const DWORD buffer3[] = { 0xfffffffe, 0xffffffff };
			const DWORD buffer4[] = { 123456789, 234567890, 345678901 };
			const DWORD buffer5[] = { 987654321, 876543210, 765432109, 654321098 };

			const TestDataType TEST_DATA[] =
			{
				{ (byte *)buffer0, sizeof(buffer0), 0, 1 },
				{ (byte *)buffer1, sizeof(buffer1), 1, 0 },
				{ (byte *)buffer2, sizeof(buffer2), 0xffffffff, 0xfffffffe },
				{ (byte *)buffer3, sizeof(buffer3), 0xfffffffe, 0xffffffff },
				{ (byte *)buffer4, sizeof(buffer4), 123456789, 234567890 },
				{ (byte *)buffer5, sizeof(buffer5), 987654321, 876543210 },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CConfigVirtualSessionField varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetVirtualSessionOptions(TEST_DATA[testIndex].m_buffer, TEST_DATA[testIndex].m_length);
				const CConfigVirtualSessionField &configField(varConfigField);

				// Test #1.
				const DWORD virtualSessionNoPlayTimeoutResult = configField.GetVirtualSessionNoPlayTimeout();

				// Test #2.
				const DWORD virtualSessionMaxDurationResult = configField.GetVirtualSessionMaxDuration();

				{
					// Report #1.
					bool areEqual = virtualSessionNoPlayTimeoutResult == TEST_DATA[testIndex].m_virtualSessionNoPlayTimeoutExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED virtualSessionNoPlayTimeoutResult=%u expected=%u testIndex=%d in %s"),
						(unsigned)virtualSessionNoPlayTimeoutResult,
						(unsigned)TEST_DATA[testIndex].m_virtualSessionNoPlayTimeoutExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				{
					// Report #2.
					bool areEqual = virtualSessionMaxDurationResult == TEST_DATA[testIndex].m_virtualSessionMaxDurationExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED virtualSessionMaxDurationResult=%u expected=%u testIndex=%d in %s"),
						(unsigned)virtualSessionMaxDurationResult,
						(unsigned)TEST_DATA[testIndex].m_virtualSessionMaxDurationExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		// SetVirtualSessionOptionsNullTest
		// Test the implementation of:
		//     CConfigVirtualSessionField::SetVirtualSessionOptions().
		//     CConfigVirtualSessionField::GetVirtualSessionNoPlayTimeout().
		//     CConfigVirtualSessionField::GetVirtualSessionMaxDuration().
		// Specifically test SetVirtualSessionOptions with a NULL buffer pointer and non-zero length.
		TEST(ConfigVirtualSessionField, SetVirtualSessionOptionsNullTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_buffer;
				const int m_length;

				// Expected results.
				const DWORD m_virtualSessionNoPlayTimeoutExpected;
				const DWORD m_virtualSessionMaxDurationExpected;
			} TestDataType;

			const DWORD buffer0[] = { 0, 1 };
			const DWORD buffer1[] = { 1, 0 };
			const DWORD buffer2[] = { 0xffffffff, 0xfffffffe };
			const DWORD buffer3[] = { 0xfffffffe, 0xffffffff };
			const DWORD buffer4[] = { 123456789, 234567890, 345678901 };
			const DWORD buffer5[] = { 987654321, 876543210, 765432109, 654321098 };

			const TestDataType TEST_DATA[] =
			{
				{ (byte *)buffer0, sizeof(buffer0), 0, 1 },
				{ (byte *)buffer1, sizeof(buffer1), 1, 0 },
				{ (byte *)buffer2, sizeof(buffer2), 0xffffffff, 0xfffffffe },
				{ (byte *)buffer3, sizeof(buffer3), 0xfffffffe, 0xffffffff },
				{ (byte *)buffer4, sizeof(buffer4), 123456789, 234567890 },
				{ (byte *)buffer5, sizeof(buffer5), 987654321, 876543210 },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CConfigVirtualSessionField varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetVirtualSessionOptions(TEST_DATA[testIndex].m_buffer, TEST_DATA[testIndex].m_length);
				varConfigField.SetVirtualSessionOptions(NULL, TEST_DATA[testIndex].m_length);
				const CConfigVirtualSessionField &configField(varConfigField);

				// Test #1.
				const DWORD virtualSessionNoPlayTimeoutResult = configField.GetVirtualSessionNoPlayTimeout();

				// Test #2.
				const DWORD virtualSessionMaxDurationResult = configField.GetVirtualSessionMaxDuration();

				{
					// Report #1.
					bool areEqual = virtualSessionNoPlayTimeoutResult == TEST_DATA[testIndex].m_virtualSessionNoPlayTimeoutExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED virtualSessionNoPlayTimeoutResult=%u expected=%u testIndex=%d in %s"),
						(unsigned)virtualSessionNoPlayTimeoutResult,
						(unsigned)TEST_DATA[testIndex].m_virtualSessionNoPlayTimeoutExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				{
					// Report #2.
					bool areEqual = virtualSessionMaxDurationResult == TEST_DATA[testIndex].m_virtualSessionMaxDurationExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED virtualSessionMaxDurationResult=%u expected=%u testIndex=%d in %s"),
						(unsigned)virtualSessionMaxDurationResult,
						(unsigned)TEST_DATA[testIndex].m_virtualSessionMaxDurationExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}
			}
		}

		// SetVirtualSessionOptionsLengthTest
		// Test the implementation of:
		//     CConfigVirtualSessionField::SetVirtualSessionOptions().
		//     CConfigVirtualSessionField::GetVirtualSessionNoPlayTimeout().
		//     CConfigVirtualSessionField::GetVirtualSessionMaxDuration().
		// Specifically test SetVirtualSessionOptions with length values less than 8 == 2 * sizeof(DWORD).
		TEST(ConfigVirtualSessionField, SetVirtualSessionOptionsLengthTest)
		{
			// MINIMUM_VALID_LENGTH =
			//     sizeof(CConfigVirtualSessionField::m_virtualSessionNoPlayTimeout) +
			//     sizeof(CConfigVirtualSessionField::m_virtualSessionMaxDuration);
			const int MINIMUM_VALID_LENGTH = 8;
			const int MINIMUM_TEST_LENGTH = -8; // Arbitrary value less zero.
			const int MAXIMUM_TEST_LENGTH = MINIMUM_VALID_LENGTH - 1;

			typedef struct
			{
				// Inputs.
				const byte *m_buffer;
				const int m_length;

				// Expected results.
				const DWORD m_virtualSessionNoPlayTimeoutExpected;
				const DWORD m_virtualSessionMaxDurationExpected;
			} TestDataType;

			const DWORD buffer0[] = { 0, 1 };
			const DWORD buffer1[] = { 1, 0 };
			const DWORD buffer2[] = { 0xffffffff, 0xfffffffe };
			const DWORD buffer3[] = { 0xfffffffe, 0xffffffff };
			const DWORD buffer4[] = { 123456789, 234567890, 345678901 };
			const DWORD buffer5[] = { 987654321, 876543210, 765432109, 654321098 };
			const byte bufferDefault[MAXIMUM_TEST_LENGTH] = {
				MEMSET_DEFAULT_VALUE,
				MEMSET_DEFAULT_VALUE,
				MEMSET_DEFAULT_VALUE,
				MEMSET_DEFAULT_VALUE,
				MEMSET_DEFAULT_VALUE,
				MEMSET_DEFAULT_VALUE,
				MEMSET_DEFAULT_VALUE,
			};

			const TestDataType TEST_DATA[] =
			{
				{ (byte *)buffer0, sizeof(buffer0), 0, 1 },
				{ (byte *)buffer1, sizeof(buffer1), 1, 0 },
				{ (byte *)buffer2, sizeof(buffer2), 0xffffffff, 0xfffffffe },
				{ (byte *)buffer3, sizeof(buffer3), 0xfffffffe, 0xffffffff },
				{ (byte *)buffer4, sizeof(buffer4), 123456789, 234567890 },
				{ (byte *)buffer5, sizeof(buffer5), 987654321, 876543210 },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CConfigVirtualSessionField varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetVirtualSessionOptions(TEST_DATA[testIndex].m_buffer, TEST_DATA[testIndex].m_length);
				// Set calls with less than the MINIMUM_VALID_LENGTH are currently ignored.
				for (int testLength = MINIMUM_TEST_LENGTH; testLength <= MAXIMUM_TEST_LENGTH; ++testLength)
				{
					// Finish performing operation(s) to be tested.
					varConfigField.SetVirtualSessionOptions(bufferDefault, testLength);
					const CConfigVirtualSessionField &configField(varConfigField);

					// Test #1.
					const DWORD virtualSessionNoPlayTimeoutResult = configField.GetVirtualSessionNoPlayTimeout();

					// Test #2.
					const DWORD virtualSessionMaxDurationResult = configField.GetVirtualSessionMaxDuration();

					{
						// Report #1.
						bool areEqual = virtualSessionNoPlayTimeoutResult == TEST_DATA[testIndex].m_virtualSessionNoPlayTimeoutExpected;
						string failedMsg;
						failedMsg = FormatString(_T("FAILED virtualSessionNoPlayTimeoutResult=%u expected=%u testLength=%d testIndex=%d in %s"),
							(unsigned)virtualSessionNoPlayTimeoutResult,
							(unsigned)TEST_DATA[testIndex].m_virtualSessionNoPlayTimeoutExpected,
							(int)testLength,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}

					{
						// Report #2.
						bool areEqual = virtualSessionMaxDurationResult == TEST_DATA[testIndex].m_virtualSessionMaxDurationExpected;
						string failedMsg;
						failedMsg = FormatString(_T("FAILED virtualSessionMaxDurationResult=%u expected=%u testLength=%d testIndex=%d in %s"),
							(unsigned)virtualSessionMaxDurationResult,
							(unsigned)TEST_DATA[testIndex].m_virtualSessionMaxDurationExpected,
							(int)testLength,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}
				}

				// Cleanup.
			}
		}
}
