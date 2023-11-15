
#include "stdafx.h"
#include "DiagnosticsPersistance.h"

// Unit testing includes.
#include "UnitTestingUtils.h"

#include "gtest/gtest.h"


namespace SentinelNativeUnitTests
{

		TEST(DiagnosticsPersistanceUnitTests, DiagnosticPersistanceInitialize)
		{
			CUnitTestingUtils::SetNvramValid();
			CDiagnosticsPersistance config;
			config.ClearReboot();
		}

		TEST(DiagnosticsPersistanceUnitTests, DiagnosticPersistanceCleanup)
		{
			CDiagnosticsPersistance config;
			config.ClearReboot();
		}

		/*
		Right now setting data in the EEPROM is too problematic when we are running on the build server.
		Will bring back when we figure out why.

		TEST(DiagnosticsPersistanceUnitTests, DiagnosticPersistanceSetGetRebootTiltCodeBothTest)
		{
			TiltCode expected = tltRebootAsap;
			TiltCode result = tltUnknown;

			// Get and set in different scopes so we read NVRAM\EEPROM and not local members
			{
				CDiagnosticsPersistance config;
				config.ClearReboot();
				config.SetRebootTiltCode(expected, DIAGNOSTICS_PERSIST_BOTH);
			}

			{
				CDiagnosticsPersistance config;
				result = config.GetRebootTiltCode();
			}

			// Cleanup code before we begin testing
			CDiagnosticsPersistance config;
			config.ClearReboot();
			ASSERT_TRUE(tltUnknown == config.GetRebootTiltCode()) << "Unable to restore Reboot Tilt Code back to original.";

			ASSERT_TRUE(result == expected) << "Reboot Tilt code set did not return as expected.";
		}
		*/

		TEST(DiagnosticsPersistanceUnitTests, DiagnosticPersistanceSetGetRebootTiltCodeNVRAMTest)
		{
			TiltCode expected = tltRebootAsap;
			TiltCode result = tltUnknown;

			// Get and set in different scopes so we read NVRAM\EEPROM and not local members
			{
				CDiagnosticsPersistance config;
				config.SetRebootTiltCode(expected, DIAGNOSTICS_PERSIST_NVRAM_ONLY);
			}

			{
				CDiagnosticsPersistance config;
				result = config.GetRebootTiltCode();
			}

			// Cleanup code before we begin testing
			CDiagnosticsPersistance config;
			config.ClearReboot();
			ASSERT_TRUE(tltUnknown == config.GetRebootTiltCode()) << "Unable to restore Reboot Tilt Code back to original.";

			ASSERT_TRUE(result == expected) << "Reboot Tilt code set did not return as expected.";
		}

		TEST(DiagnosticsPersistanceUnitTests, DiagnosticPersistanceSetGetRebootTiltCodeTwiceTest)
		{
			TiltCode expected = tltRebootAsap;
			TiltCode result = tltUnknown;

			// Get and set in different scopes so we read NVRAM\EEPROM and not local members
			{
				CDiagnosticsPersistance config;
				config.SetRebootTiltCode(expected, DIAGNOSTICS_PERSIST_NVRAM_ONLY);
			}

			{
				CDiagnosticsPersistance config;
				// 2nd set of tilt should be ignored
				config.SetRebootTiltCode(tltRebootUnknown, DIAGNOSTICS_PERSIST_NVRAM_ONLY);
			}

			{
				CDiagnosticsPersistance config;
				result = config.GetRebootTiltCode();
			}

			// Cleanup code before we begin testing
			CDiagnosticsPersistance config;
			config.ClearReboot();
			ASSERT_TRUE(tltUnknown == config.GetRebootTiltCode()) << "Unable to restore Reboot Tilt Code back to original.";

			ASSERT_FALSE(result == tltRebootUnknown) << "2nd set overwrote first.";
			ASSERT_TRUE(result == expected) << "Reboot Tilt code set did not return as expected.";
		}
}

