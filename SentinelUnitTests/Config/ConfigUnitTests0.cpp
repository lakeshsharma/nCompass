#include "stdafx.h"
#include "Config.h"

// Unit testing includes.
#include "gtest/gtest.h"
#include "UnitTestingUtils.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	class Config0 : public ::testing::Test
	{
	public:
		/// <summary>
		/// Determine if MACAddressAsString format is valid.
		/// </summary>
		/// <returns>true if format is valid, otherwise false.</returns>
		bool IsValidMACAddressAsString(LPCTSTR macAddressAsString)
		{
			bool ok(false);
			size_t length = _tcslen(macAddressAsString);

			if (length == 0)
			{
				ok = true;
			}
			else if (length == 17)
			{
				ok = true; // Good so far.

				for (size_t index = 0; index < length; ++index)
				{
					TCHAR ch = macAddressAsString[index];
					if (index % 3 + 1 == 3)
					{
						ok = ch == TCHAR('-');
					}
					else
					{
						ok = (TCHAR('0') <= ch && ch <= TCHAR('9')) ||
							(TCHAR('A') <= ch && ch <= TCHAR('F'));
					}

					if (!ok)
					{
						break; // Out of "for" loop.
					}
				}
			}

			return ok;
		}
	};

		/// <summary>
		/// Code coverage and functional test for:
		///     CConfig::CConfig()
		///     CConfig::GetMacAddressAsBytes()
		/// Test default state.
		/// </summary>
		TEST_F(Config0, GetMacAddressAsBytes_DefaultTest)
		{
			// Inputs.

			// Expected results.
			const size_t macAddressAsBytesLength(SIZE_OF_MAC_ADDRESS); // Estimate current sizeof(CConfig::m_macAddress).
			const BYTE macAddressAsBytesExpected[] = { 0, 0, 0, 0, 0, 0 };

			// Setup.
			ASSERT_TRUE(sizeof(macAddressAsBytesExpected) == macAddressAsBytesLength) << _T("Setup lengths");

			// Perform operations to be tested.
			const CConfig config(false, "");
			const BYTE* macAddressAsBytesResult = config.GetMacAddressAsBytes();

			// Test
			ASSERT_TRUE(0 == memcmp(macAddressAsBytesResult, macAddressAsBytesExpected, macAddressAsBytesLength)) << _T("memcmp");
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CConfig::CConfig()
		///     CConfig::GetMacAddress2AsBytes()
		/// Test default state.
		/// </summary>
		TEST_F(Config0, GetMacAddress2AsBytes_DefaultTest)
		{
			// Inputs.

			// Expected results.
			const size_t macAddress2AsBytesLength(SIZE_OF_MAC_ADDRESS); // Estimate current sizeof(CConfig::m_macAddress).
			const BYTE macAddress2AsBytesExpected[] = { 0, 0, 0, 0, 0, 0 };

			// Setup.
			ASSERT_TRUE(sizeof(macAddress2AsBytesExpected) == macAddress2AsBytesLength) << _T("Setup lengths");

			// Perform operations to be tested.
			const CConfig config(false, "");
			const BYTE* macAddress2AsBytesResult = config.GetMacAddress2AsBytes();

			// Test
			ASSERT_TRUE(0 == memcmp(macAddress2AsBytesResult, macAddress2AsBytesExpected, macAddress2AsBytesLength)) << _T("memcmp");

			// Cleanup.
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CConfig::CConfig()
		///     CConfig::GetMACAddressAsString()
		/// Test default state.
		/// </summary>
		TEST_F(Config0, GetMACAddressAsString_DefaultTest)
		{
			// Inputs.

			// Expected results.
			LPCTSTR macAddressAsStringExpected(_T(""));

			// Setup.

			// Perform operations to be tested.
			const CConfig config(false, "");
			LPCTSTR macAddressAsStringResult = config.GetMACAddressAsString().c_str();

			// Test
			ASSERT_TRUE(0 == _tcscmp(macAddressAsStringResult, macAddressAsStringExpected)) << _T("strcmp");
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CConfig::CConfig()
		///     CConfig::BuildYourself()
		///     CConfig::InitializeMACAddress()
		///     CConfig::GetMACAddressAsString()
		/// Test initial state.
		/// </summary>
		TEST_F(Config0, GetMACAddressAsString_InitialTest)
		{
			// Inputs.

			// Expected results.

			// Setup.

			// Perform operations to be tested.
			CConfig varConfig(false, "");
			varConfig.BuildYourself();
			const CConfig &config(varConfig);
			LPCTSTR macAddressAsStringResult = config.GetMACAddressAsString().c_str();

			// Test
			ASSERT_TRUE(IsValidMACAddressAsString(macAddressAsStringResult)) << _T("is valid");
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CConfig::CConfig()
		///     CConfig::GetActualIP()
		///     CConfig::InitializeMACAddress()
		///     CConfig::GetMACAddressAsString()
		/// Test actual state.
		/// </summary>
		TEST_F(Config0, GetMACAddressAsString_ActualTest)
		{
			// Inputs.

			// Expected results.

			// Setup.

			// Perform operations to be tested.
			CConfig varConfig(false, "");
			varConfig.GetActualIP();
			const CConfig &config(varConfig);
			LPCTSTR macAddressAsStringResult = config.GetMACAddressAsString().c_str();

			// Test
			ASSERT_TRUE(IsValidMACAddressAsString(macAddressAsStringResult)) << _T("is valid");
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CConfig::CConfig()
		///     CConfig::GetMACAddress2AsString()
		/// Test default state.
		/// </summary>
		TEST_F(Config0, GetMACAddress2AsString_DefaultTest)
		{
			// Inputs.

			// Expected results.
			LPCTSTR macAddress2AsStringExpected(_T(""));

			// Setup.

			// Perform operations to be tested.
			const CConfig config(false, "");
			LPCTSTR macAddress2AsStringResult = config.GetMACAddress2AsString().c_str();

			// Test
			ASSERT_TRUE(0 == _tcscmp(macAddress2AsStringResult, macAddress2AsStringExpected)) << _T("strcmp");
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CConfig::CConfig()
		///     CConfig::BuildYourself()
		///     CConfig::InitializeMACAddress()
		///     CConfig::GetMACAddress2AsString()
		/// Test initial state.
		/// </summary>
		TEST_F(Config0, GetMACAddress2AsString_InitialTest)
		{
			// Inputs.

			// Expected results.

			// Setup.

			// Perform operations to be tested.
			CConfig varConfig(false, "");
			varConfig.BuildYourself();
			const CConfig &config(varConfig);
			LPCTSTR macAddress2AsStringResult = config.GetMACAddress2AsString().c_str();

			// Test
			ASSERT_TRUE(IsValidMACAddressAsString(macAddress2AsStringResult)) << _T("is valid");
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CConfig::CConfig()
		///     CConfig::GetActualIP()
		///     CConfig::InitializeMACAddress()
		///     CConfig::GetMACAddress2AsString()
		/// Test actual state.
		/// </summary>
		TEST_F(Config0, GetMACAddress2AsString_ActualTest)
		{
			// Inputs.

			// Expected results.

			// Setup.

			// Perform operations to be tested.
			CConfig varConfig(false, "");
			varConfig.GetActualIP();
			const CConfig &config(varConfig);
			LPCTSTR macAddress2AsStringResult = config.GetMACAddress2AsString().c_str();

			// Test
			ASSERT_TRUE(IsValidMACAddressAsString(macAddress2AsStringResult)) << _T("is valid");
		}

		/// </summary>
		TEST_F(Config0, GetDomainTest)
		{
			// Inputs.
			string domain("devnext.onelink.dev.local");
			CConfig varConfig(false, "");
			
			// Test
			varConfig.SetDomain("");
			ASSERT_TRUE("NA" == varConfig.GetDomain());

			varConfig.SetDomain(domain);
			ASSERT_TRUE(domain == varConfig.GetDomain());
		}
}
