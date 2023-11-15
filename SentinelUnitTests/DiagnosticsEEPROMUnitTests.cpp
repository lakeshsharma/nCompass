
#include "stdafx.h"
#include "DiagnosticsEEPROM.h"


#include "gtest/gtest.h"


namespace SentinelNativeUnitTests
{
		TEST(DiagnosticsEEPROMUnitTests, DiagnosticsEEPROMConstructorTest)
		{
			CDiagnosticsEEPROM storage;
			ASSERT_TRUE(MEMORY_EEPROM == storage.GetMemoryType()) << "Intialized to wrong memory type.";
		}

		/*
		Right now setting data in the EEPROM is too problematic when we are running on the build server.
		Will bring back when we figure out why.

		TEST(DiagnosticsEEPROMUnitTests, DiagnosticseEEPROMSetGetRebootTiltCodeTest)
		{
			TiltCode expected = tltRebootButton;
			TiltCode original = tltUnknown;
			TiltCode result = tltUnknown;

			// Get and set in different scopes so we read NVRAM\EEPROM and not local members
			{
				CDiagnosticsEEPROM storage;
				expected = tltRebootButton;
				original = storage.GetRebootTiltCode();
				storage.SetRebootTiltCode(expected);
			}

			{
				CDiagnosticsEEPROM storage;
				result = storage.GetRebootTiltCode();
			}

			// Restore code before we begin testing
			CDiagnosticsEEPROM storage;
			storage.SetRebootTiltCode(original);
			ASSERT_TRUE(original == storage.GetRebootTiltCode()) << "Unable to restore Reboot Tilt Code back to original.";

			ASSERT_TRUE(result == expected) << "Reboot Tilt code set did not return as expected.";
		}*/
}

