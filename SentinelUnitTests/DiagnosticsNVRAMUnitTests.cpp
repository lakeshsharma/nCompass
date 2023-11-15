
#include "stdafx.h"
#include "DiagnosticsNvram.h"


#include "gtest/gtest.h"


namespace SentinelNativeUnitTests
{
		
		TEST(DiagnosticsNVRAMUnitTests, DiagnosticNVRAMInitialize)
		{
			CMemoryStorage::SetSprinkles();
			CMemoryStorage::UpgradeNVRAM();
		}

		TEST(DiagnosticsNVRAMUnitTests, DiagnosticNVRAMCleanup)
		{
			// Ensure presistent memory (NVRAM) is left in clean state for other tests
			CMemoryStorage::SetSprinkles();
			CMemoryStorage::UpgradeNVRAM();
		}

		TEST(DiagnosticsNVRAMUnitTests, DiagnosticNVRAMConstructorTest)
		{
			CDiagnosticsNVRAM storage;
			ASSERT_TRUE(MEMORY_NVRAM == storage.GetMemoryType()) << "Intialized to wrong memory type.";
		}

		TEST(DiagnosticsNVRAMUnitTests, DiagnosticNVRAMSetGetRebootTiltCodeTest)
		{
			TiltCode expected = tltRebootAsap;
			TiltCode original = tltUnknown;
			TiltCode result = tltUnknown;

			// Get and set in different scopes so we read NVRAM\EEPROM and not local members
			{
				CDiagnosticsNVRAM storage;
				expected = tltRebootAsap;
				original = storage.GetRebootTiltCode();
				storage.SetRebootTiltCode(expected);
			}

			{
				CDiagnosticsNVRAM storage;
				result = storage.GetRebootTiltCode();
			}

			// Restore code before we begin testing
			CDiagnosticsNVRAM storage;
			storage.SetRebootTiltCode(original);
			ASSERT_TRUE(original == storage.GetRebootTiltCode()) << "Unable to restore Reboot Tilt Code back to original.";

			ASSERT_TRUE(result == expected) << "Reboot Tilt code set did not return as expected.";
		}
		
		TEST(DiagnosticsNVRAMUnitTests, DiagnosticNVRAMSetGetRebootTiltCodeTestAfterNVRAMClear)
		{
			TiltCode expected = tltRebootMemoryClear;			
			TiltCode result = tltUnknown;

			// Get and set in different scopes so we read NVRAM\EEPROM and not local members
			{
				CDiagnosticsNVRAM storage;
				storage.SetRebootTiltCode(expected);
				CMemoryStorage::SetInactiveLocation(SPRINKLE1_OFFSET);
			}

			{
				CDiagnosticsNVRAM storage;
				result = storage.GetRebootTiltCode();
			}

			ASSERT_TRUE(result == expected) << "Reboot Tilt code set did not return as expected.";
		}
}

