#include "stdafx.h"
#include "Config/EGMConfig.h"

// Unit testing includes.
#include "gtest/gtest.h"
#include "AssertMsg.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	class EGMConfigTests : public ::testing::Test
	{
	public:
	};
// #pragma region GetEGMPollingSpeed
		/// <summary>
		/// Functional test for:
		///     CEGMConfig
		/// Test static const values.  Test persisted values for forwards and backwards compatibility.
		/// </summary>
		TEST(EGMConfigTests, GetEGMPollingSpeed_StaticConst)
		{
			ASSERT_EQ((EGMPollingSpeed)0, MS40) << L"MS40";
			ASSERT_EQ((EGMPollingSpeed)1, MS200) << L"MS200";
			ASSERT_EQ((byte)MS40, EEPROM_EGM_POLLING_SPEED_DEFAULT) << L"EEPROM_EGM_POLLING_SPEED_DEFAULT";
			ASSERT_EQ((DWORD)93, EEPROM_EGM_POLLING_SPEED_OFFSET) << L"EEPROM_EGM_POLLING_SPEED_OFFSET";
			ASSERT_EQ((DWORD)1, EEPROM_EGM_POLLING_SPEED_SIZE) << L"EEPROM_EGM_POLLING_SPEED_SIZE";
		}

		/// <summary>
		/// Functional test for:
		///     CEGMConfig::CEGMConfig()
		///     CEGMConfig::GetEGMPollingSpeed()
		/// Test constructor for default value(s).
		/// </summary>
		TEST(EGMConfigTests, GetEGMPollingSpeed_Default)
		{
			// Perform operation(s) and test(s).
			CEGMConfig obj(MEMORY_NONE);
			ASSERT_EQ(MS40, obj.GetEGMPollingSpeed()) << L"GetEGMPollingSpeed";
		}

		/// <summary>
		/// Functional test for:
		///     CEGMConfig::SetEGMPollingSpeed()
		///     CEGMConfig::GetEGMPollingSpeed()
		/// Test setting non-default value(s).
		/// </summary>
		TEST(EGMConfigTests, GetEGMPollingSpeed_Set)
		{
			// Data.
			const EGMPollingSpeed pollingSpeed1(MS200);
			const EGMPollingSpeed pollingSpeed2(MS40);

			// Setup.
			CEGMConfig obj(MEMORY_NONE);
			ASSERT_NE(pollingSpeed1, obj.GetEGMPollingSpeed()) << L"GetEGMPollingSpeed setup"; // Verify setup.

			// Perform operation(s) and test(s).
			obj.SetEGMPollingSpeed(pollingSpeed1);
			ASSERT_EQ(pollingSpeed1, obj.GetEGMPollingSpeed()) << L"GetEGMPollingSpeed 1";

			obj.SetEGMPollingSpeed(pollingSpeed2);
			ASSERT_EQ(pollingSpeed2, obj.GetEGMPollingSpeed()) << L"GetEGMPollingSpeed 2";
		}

		/// <summary>
		/// Functional test for:
		///     CEGMConfig::CEGMConfig(const CEGMConfig &)
		///     CEGMConfig::GetEGMPollingSpeed()
		/// Test copy constructor with non-default value(s).
		/// </summary>
		TEST(EGMConfigTests, GetEGMPollingSpeed_Copy)
		{
			// Data.
			const EGMPollingSpeed pollingSpeed(MS200);

			// Setup.
			CEGMConfig sourceObj(MEMORY_NONE);
			sourceObj.SetEGMPollingSpeed(pollingSpeed);

			// Perform operation(s) and test(s).
			CEGMConfig obj(sourceObj);
			ASSERT_EQ(pollingSpeed, obj.GetEGMPollingSpeed()) << L"GetEGMPollingSpeed";
		}

		/// <summary>
		/// Functional test for:
		///     CEGMConfig::operator=()
		///     CEGMConfig::GetEGMPollingSpeed()
		/// Test assignment operator with non-default value(s).
		/// </summary>
		TEST(EGMConfigTests, GetEGMPollingSpeed_Assignment)
		{
			// Data.
			const EGMPollingSpeed pollingSpeed(MS200);

			// Setup.
			CEGMConfig sourceObj(MEMORY_NONE);
			sourceObj.SetEGMPollingSpeed(pollingSpeed);

			CEGMConfig obj(MEMORY_NONE);
			ASSERT_NE(pollingSpeed, obj.GetEGMPollingSpeed()) << L"GetEGMPollingSpeed setup"; // Verify setup.

			// Perform operation(s) and test(s).
			obj = sourceObj;
			ASSERT_EQ(pollingSpeed, obj.GetEGMPollingSpeed()) << L"GetEGMPollingSpeed";
		}

		/// <summary>
		/// Functional test for:
		///     CEGMConfig::CEGMConfig()
		///     CEGMConfig::BuildYourself()
		///     CEGMConfig::SetEGMPollingSpeed()
		///     CEGMConfig::GetEGMPollingSpeed()
		/// Test setting and getting persisted value(s) from EEPROM.
		/// </summary>
		TEST(EGMConfigTests, GetEGMPollingSpeed_Eeprom)
		{
			// Data.
			const EGMPollingSpeed pollingSpeed(MS200);

			// Perform operation(s) and test(s).
			{
				CEGMConfig obj(MEMORY_EEPROM);
				obj.SetEGMPollingSpeed(pollingSpeed);
			}

			CEGMConfig obj(MEMORY_EEPROM);
			obj.BuildYourself();
			ASSERT_EQ(pollingSpeed, obj.GetEGMPollingSpeed()) << L"GetEGMPollingSpeed";
		}

		/// <summary>
		/// Functional test for:
		///     CEGMConfig::CEGMConfig()
		///     CEGMConfig::SetEGMPollingSpeed()
		/// Test setting persisted value(s) to EEPROM for forwards and backwards compatibility.
		/// </summary>
		TEST(EGMConfigTests, GetEGMPollingSpeed_EepromSet)
		{
			// Data.
			const EGMPollingSpeed pollingSpeed(MS200);

			// Perform operation(s) and test(s).
			{
				CEGMConfig obj(MEMORY_EEPROM);
				obj.SetEGMPollingSpeed(pollingSpeed);
			}

			// Read from the shadow buffer.  The driver may be used to update the shadow buffer.
			CEEPROMConfig eepromConfig;
			BYTE eepromPollingSpeed((BYTE)~pollingSpeed);
			ASSERT_EQ((DWORD)sizeof(eepromPollingSpeed), EEPROM_EGM_POLLING_SPEED_SIZE) << L"sizeof(eepromPollingSpeed)";
			ASSERT_EQ(true, eepromConfig.Get(EEPROM_EGM_POLLING_SPEED_OFFSET, &eepromPollingSpeed, sizeof(eepromPollingSpeed))) << L"GetData";
			ASSERT_EQ((BYTE)pollingSpeed, eepromPollingSpeed) << L"eepromPollingSpeed";
		}

		/// <summary>
		/// Functional test for:
		///     CEGMConfig::CEGMConfig()
		///     CEGMConfig::GetEGMPollingSpeed()
		/// Test getting persisted value(s) from EEPROM for forwards and backwards compatibility.
		/// </summary>
		TEST(EGMConfigTests, GetEGMPollingSpeed_EepromGet)
		{
			// Data.
			const EGMPollingSpeed pollingSpeed(MS200);

			// Setup.
			// Use a CEEPROMConfig instance to set the data, so the checksum is valid and the shadow buffer is written through.
			{
				CEEPROMConfig eepromConfig;
				BYTE eepromPollingSpeed((BYTE)pollingSpeed);
				ASSERT_EQ((DWORD)sizeof(eepromPollingSpeed), EEPROM_EGM_POLLING_SPEED_SIZE) << L"EEPROM_EGM_POLLING_SPEED_SIZE";
				// Don't check the return value from CEEPROMConfig::Set(), in case the driver isn't operational.
				eepromConfig.Set(EEPROM_EGM_POLLING_SPEED_OFFSET, &eepromPollingSpeed, sizeof(eepromPollingSpeed));
			}

			// Perform operation(s) and test(s).
			CEGMConfig obj(MEMORY_EEPROM);
			obj.BuildYourself();
			ASSERT_EQ(pollingSpeed, obj.GetEGMPollingSpeed()) << L"GetEGMPollingSpeed";
		}
// #pragma endregion GetEGMPollingSpeed

// #pragma region GetEGMProtocol
		/// <summary>
		/// Functional test for:
		///     CEGMConfig
		/// Test static const values.  Test persisted values for forwards and backwards compatibility.
		/// </summary>
		TEST(EGMConfigTests, GetEGMProtocol_StaticConst)
		{
			ASSERT_EQ((EGMProtocol)0, NO_EGM_PROTOCOL) << L"NO_EGM_PROTOCOL";
			ASSERT_EQ((EGMProtocol)1, IGT_SAS) << L"IGT_SAS";
			ASSERT_EQ((EGMProtocol)2, BALLY_PROTOCOL) << L"BALLY_PROTOCOL";
			ASSERT_EQ((EGMProtocol)3, WILLIAMS) << L"WILLIAMS";
			ASSERT_EQ((EGMProtocol)4, ARISTOCRAT) << L"ARISTOCRAT";
			ASSERT_EQ((EGMProtocol)5, CDS_GAMING_DEVICE_ACCOUNTING_PROTOCOL) << L"CDS_GAMING_DEVICE_ACCOUNTING_PROTOCOL";
			ASSERT_EQ((EGMProtocol)6, BALLY_EXTENDED_SIMPLE_SERIAL) << L"BALLY_EXTENDED_SIMPLE_SERIAL";
			ASSERT_EQ((EGMProtocol)7, AUTOTOTE) << L"AUTOTOTE";
			ASSERT_EQ((EGMProtocol)8, IGT_SAS_WITH_SLOW_POLLING) << L"IGT_SAS_WITH_SLOW_POLLING";
			ASSERT_EQ((EGMProtocol)9, IGT_SAS_3xx) << L"IGT_SAS_3xx";
			ASSERT_EQ((EGMProtocol)10, ARISTOCRAT_GAMPRO) << L"ARISTOCRAT_GAMPRO";
			ASSERT_EQ((EGMProtocol)11, SAS5xx) << L"SAS5xx";
			ASSERT_EQ((EGMProtocol)12, SAS6xx) << L"SAS6xx";
			ASSERT_EQ((byte)SAS6xx, EEPROM_EGM_PROTOCOL_DEFAULT) << L"EEPROM_EGM_PROTOCOL_DEFAULT";
			ASSERT_EQ((DWORD)94, EEPROM_EGM_PROTOCOL_OFFSET) << L"EEPROM_EGM_PROTOCOL_OFFSET";
			ASSERT_EQ((DWORD)1, EEPROM_EGM_PROTOCOL_SIZE) << L"EEPROM_EGM_PROTOCOL_SIZE";
		}

		/// <summary>
		/// Functional test for:
		///     CEGMConfig::CEGMConfig()
		///     CEGMConfig::GetEGMProtocol()
		/// Test constructor for default value(s).
		/// </summary>
		TEST(EGMConfigTests, GetEGMProtocol_Default)
		{
			// Perform operation(s) and test(s).
			CEGMConfig obj(MEMORY_NONE);
			ASSERT_EQ(SAS6xx, obj.GetEGMProtocol()) << L"GetEGMProtocol";
		}

		/// <summary>
		/// Functional test for:
		///     CEGMConfig::SetEGMProtocol()
		///     CEGMConfig::GetEGMProtocol()
		/// Test setting non-default value(s).
		/// </summary>
		TEST(EGMConfigTests, GetEGMProtocol_Set)
		{
			// Data.
			const EGMProtocol protocolMin(NO_EGM_PROTOCOL);
			const EGMProtocol protocolMax(SAS6xx);

			// Setup.
			CEGMConfig obj(MEMORY_NONE);

			for (int testIndex = protocolMin - 1; testIndex <= protocolMax + 1; ++testIndex)
			{
				// Perform operation(s) and test(s).
				EGMProtocol protocol((EGMProtocol)testIndex);
				obj.SetEGMProtocol(protocol);
				ASSERT_EQ(protocol, obj.GetEGMProtocol()) << AssertMsg::FormatOnly(L"testIndex=%d", (int)testIndex);
			}
		}

		/// <summary>
		/// Functional test for:
		///     CEGMConfig::CEGMConfig(const CEGMConfig &)
		///     CEGMConfig::GetEGMProtocol()
		/// Test copy constructor with non-default value(s).
		/// </summary>
		TEST(EGMConfigTests, GetEGMProtocol_Copy)
		{
			// Data.
			const EGMProtocol protocol(IGT_SAS_3xx);

			// Setup.
			CEGMConfig sourceObj(MEMORY_NONE);
			sourceObj.SetEGMProtocol(protocol);

			// Perform operation(s) and test(s).
			CEGMConfig obj(sourceObj);
			ASSERT_EQ(protocol, obj.GetEGMProtocol()) << L"GetEGMProtocol";
		}

		/// <summary>
		/// Functional test for:
		///     CEGMConfig::operator=()
		///     CEGMConfig::GetEGMProtocol()
		/// Test assignment operator with non-default value(s).
		/// </summary>
		TEST(EGMConfigTests, GetEGMProtocol_Assignment)
		{
			// Data.
			const EGMProtocol protocol(IGT_SAS_3xx);

			// Setup.
			CEGMConfig sourceObj(MEMORY_NONE);
			sourceObj.SetEGMProtocol(protocol);

			CEGMConfig obj(MEMORY_NONE);
			ASSERT_NE(protocol, obj.GetEGMProtocol()) << L"GetEGMProtocol setup"; // Verify setup.

			// Perform operation(s) and test(s).
			obj = sourceObj;
			ASSERT_EQ(protocol, obj.GetEGMProtocol()) << L"GetEGMProtocol";
		}

		/// <summary>
		/// Functional test for:
		///     CEGMConfig::CEGMConfig()
		///     CEGMConfig::BuildYourself()
		///     CEGMConfig::SetEGMProtocol()
		///     CEGMConfig::GetEGMProtocol()
		/// Test setting and getting persisted value(s) from EEPROM.
		/// </summary>
		void GetEGMProtocol_EepromTest(EGMProtocol protocol)
		{
			// Perform operation(s) and test(s).
			{
				CEGMConfig obj(MEMORY_EEPROM);
				obj.SetEGMProtocol(protocol);
			}

			CEGMConfig obj(MEMORY_EEPROM);
			obj.BuildYourself();
			ASSERT_EQ(protocol, obj.GetEGMProtocol()) << L"GetEGMProtocolA";
		}

		TEST(EGMConfigTests, GetEGMProtocol_Eeprom)
		{
			GetEGMProtocol_EepromTest(IGT_SAS_3xx);
			GetEGMProtocol_EepromTest(SAS5xx);
		}

		/// <summary>
		/// Functional test for:
		///     CEGMConfig::CEGMConfig()
		///     CEGMConfig::SetEGMProtocol()
		/// Test setting persisted value(s) to EEPROM for forwards and backwards compatibility.
		/// </summary>
		void GetEGMProtocol_EepromSetTest(EGMProtocol protocol)
		{
			// Perform operation(s) and test(s).
			{
				CEGMConfig obj(MEMORY_EEPROM);
				obj.SetEGMProtocol(protocol);
			}

			// Read from the shadow buffer.  The driver may be used to update the shadow buffer.
			CEEPROMConfig eepromConfig;
			BYTE eepromProtocol((BYTE)~protocol);
			ASSERT_EQ((DWORD)sizeof(eepromProtocol), EEPROM_EGM_PROTOCOL_SIZE) << L"sizeof(eepromProtocol)";
			ASSERT_EQ(true, eepromConfig.Get(EEPROM_EGM_PROTOCOL_OFFSET, &eepromProtocol, sizeof(eepromProtocol))) << L"GetData";
			ASSERT_EQ((BYTE)protocol, eepromProtocol) << L"eepromProtocol";
		}

		TEST(EGMConfigTests, GetEGMProtocol_EepromSet)
		{
			GetEGMProtocol_EepromSetTest(IGT_SAS_3xx);
			GetEGMProtocol_EepromSetTest(SAS5xx);
		}

		/// <summary>
		/// Functional test for:
		///     CEGMConfig::CEGMConfig()
		///     CEGMConfig::GetEGMProtocol()
		/// Test getting persisted value(s) from EEPROM for forwards and backwards compatibility.
		/// </summary>
		void GetEGMProtocol_EepromGetTest(EGMProtocol protocol)
		{
			// Setup.
			// Use a CEEPROMConfig instance to set the data, so the checksum is valid and the shadow buffer is written through.
			{
				CEEPROMConfig eepromConfig;
				BYTE eepromProtocol((BYTE)protocol);
				ASSERT_EQ((DWORD)sizeof(eepromProtocol), EEPROM_EGM_PROTOCOL_SIZE) << L"EEPROM_EGM_PROTOCOL_SIZE";
				// Don't check the return value from CEEPROMConfig::Set(), in case the driver isn't operational.
				eepromConfig.Set(EEPROM_EGM_PROTOCOL_OFFSET, &eepromProtocol, sizeof(eepromProtocol));
			}

			// Perform operation(s) and test(s).
			CEGMConfig obj(MEMORY_EEPROM);
			obj.BuildYourself();
			ASSERT_EQ(protocol, obj.GetEGMProtocol()) << L"GetEGMProtocol";
		}

		TEST(EGMConfigTests, GetEGMProtocol_EepromGet)
		{
			GetEGMProtocol_EepromGetTest(IGT_SAS_3xx);
			GetEGMProtocol_EepromGetTest(SAS5xx);
		}
// #pragma endregion GetEGMProtocol

// #pragma region GetEGMProtocolState
		/// <summary>
		/// Functional test for:
		///     CEGMConfig
		/// Test static const values.  Test persisted values for forwards and backwards compatibility.
		/// </summary>
		TEST(EGMConfigTests, GetEGMProtocolState_StaticConst)
		{
			ASSERT_EQ((EGMProtocolState)0, protocolStateEnabled) << L"protocolStateEnabled";
			ASSERT_EQ((EGMProtocolState)1, protocolStateDisabled) << L"protocolStateDisabled";
			ASSERT_EQ((EGMProtocolState)2, protocolStateDisableTransactionsFull) << L"protocolStateDisableTransactionsFull";
			ASSERT_EQ((byte)protocolStateDisabled, EEPROM_EGM_PROTOCOL_STATE_DEFAULT) << L"EEPROM_EGM_PROTOCOL_STATE_DEFAULT";
			ASSERT_EQ((DWORD)97, EEPROM_EGM_PROTOCOL_STATE_OFFSET) << L"EEPROM_EGM_PROTOCOL_STATE_OFFSET";
			ASSERT_EQ((DWORD)1, EEPROM_EGM_PROTOCOL_STATE_SIZE) << L"EEPROM_EGM_PROTOCOL_STATE_SIZE";
		}

		/// <summary>
		/// Functional test for:
		///     CEGMConfig::CEGMConfig()
		///     CEGMConfig::GetEGMProtocolState()
		/// Test constructor for default value(s).
		/// </summary>
		TEST(EGMConfigTests, GetEGMProtocolState_Default)
		{
			// Perform operation(s) and test(s).
			CEGMConfig obj(MEMORY_NONE);
			ASSERT_EQ(protocolStateDisabled, obj.GetEGMProtocolState()) << L"GetEGMProtocolState";
		}

		/// <summary>
		/// Functional test for:
		///     CEGMConfig::SetEGMProtocolState()
		///     CEGMConfig::GetEGMProtocolState()
		/// Test setting non-default value(s).
		/// </summary>
		TEST(EGMConfigTests, GetEGMProtocolState_Set)
		{
			// Data.
			const EGMProtocolState egmProtocolStateMin(protocolStateEnabled);
			const EGMProtocolState egmProtocolStateMax(protocolStateDisableTransactionsFull);

			// Setup.
			CEGMConfig obj(MEMORY_NONE);

			for (int testIndex = egmProtocolStateMin - 1; testIndex <= egmProtocolStateMax + 1; ++testIndex)
			{
				// Perform operation(s) and test(s).
				EGMProtocolState egmProtocolState((EGMProtocolState)testIndex);
				obj.SetEGMProtocolState(egmProtocolState);
				ASSERT_EQ(egmProtocolState, obj.GetEGMProtocolState()) << AssertMsg::FormatOnly(L"testIndex=%d", (int)testIndex);
			}
		}

		/// <summary>
		/// Functional test for:
		///     CEGMConfig::CEGMConfig(const CEGMConfig &)
		///     CEGMConfig::GetEGMProtocolState()
		/// Test copy constructor with non-default value(s).
		/// </summary>
		TEST(EGMConfigTests, GetEGMProtocolState_Copy)
		{
			// Data.
			const EGMProtocolState egmProtocolState(protocolStateDisableTransactionsFull);
			ASSERT_NE(egmProtocolState, (EGMProtocolState)EEPROM_EGM_PROTOCOL_STATE_DEFAULT) << L"Not default";

			// Setup.
			CEGMConfig sourceObj(MEMORY_NONE);
			sourceObj.SetEGMProtocolState(egmProtocolState);

			// Perform operation(s) and test(s).
			CEGMConfig obj(sourceObj);
			ASSERT_EQ(egmProtocolState, obj.GetEGMProtocolState()) << L"GetEGMProtocolState";
		}

		/// <summary>
		/// Functional test for:
		///     CEGMConfig::operator=()
		///     CEGMConfig::GetEGMProtocolState()
		/// Test assignment operator with non-default value(s).
		/// </summary>
		TEST(EGMConfigTests, GetEGMProtocolState_Assignment)
		{
			// Data.
			const EGMProtocolState egmProtocolState(protocolStateDisableTransactionsFull);

			// Setup.
			CEGMConfig sourceObj(MEMORY_NONE);
			sourceObj.SetEGMProtocolState(egmProtocolState);

			CEGMConfig obj(MEMORY_NONE);
			ASSERT_NE(egmProtocolState, obj.GetEGMProtocolState()) << L"GetEGMProtocolState setup"; // Verify setup.

			// Perform operation(s) and test(s).
			obj = sourceObj;
			ASSERT_EQ(egmProtocolState, obj.GetEGMProtocolState()) << L"GetEGMProtocolState";
		}

		/// <summary>
		/// Functional test for:
		///     CEGMConfig::CEGMConfig()
		///     CEGMConfig::BuildYourself()
		///     CEGMConfig::SetEGMProtocolState()
		///     CEGMConfig::GetEGMProtocolState()
		/// Test setting and getting persisted value(s) from EEPROM.
		/// </summary>
		void GetEGMProtocolState_EepromTest(EGMProtocolState egmProtocolState)
		{
			// Perform operation(s) and test(s).
			{
				CEGMConfig obj(MEMORY_EEPROM);
				obj.SetEGMProtocolState(egmProtocolState);
			}

			CEGMConfig obj(MEMORY_EEPROM);
			obj.BuildYourself();
			ASSERT_EQ(egmProtocolState, obj.GetEGMProtocolState()) << L"GetEGMProtocolStateA";
		}

		TEST(EGMConfigTests, GetEGMProtocolState_Eeprom)
		{
			GetEGMProtocolState_EepromTest(protocolStateEnabled);
			GetEGMProtocolState_EepromTest(protocolStateDisabled);
			GetEGMProtocolState_EepromTest(protocolStateDisableTransactionsFull);
		}

		/// <summary>
		/// Functional test for:
		///     CEGMConfig::CEGMConfig()
		///     CEGMConfig::SetEGMProtocolState()
		/// Test setting persisted value(s) to EEPROM for forwards and backwards compatibility.
		/// </summary>
		void GetEGMProtocolState_EepromSetTest(EGMProtocolState egmProtocolState)
		{
			// Perform operation(s) and test(s).
			{
				CEGMConfig obj(MEMORY_EEPROM);
				obj.SetEGMProtocolState(egmProtocolState);
			}

			// Read from the shadow buffer.  The driver may be used to update the shadow buffer.
			CEEPROMConfig eepromConfig;
			BYTE eepromEGMProtocolState((BYTE)~egmProtocolState);
			ASSERT_EQ((DWORD)sizeof(eepromEGMProtocolState), EEPROM_EGM_PROTOCOL_STATE_SIZE) << L"sizeof(eepromEGMProtocolState)";
			ASSERT_EQ(true,
				eepromConfig.Get(EEPROM_EGM_PROTOCOL_STATE_OFFSET, &eepromEGMProtocolState, sizeof(eepromEGMProtocolState))) << L"GetData";
			ASSERT_EQ((BYTE)egmProtocolState, eepromEGMProtocolState) << L"eepromEGMProtocolState";
		}

		TEST(EGMConfigTests, GetEGMProtocolState_EepromSet)
		{
			GetEGMProtocolState_EepromSetTest(protocolStateEnabled);
			GetEGMProtocolState_EepromSetTest(protocolStateDisabled);
			GetEGMProtocolState_EepromSetTest(protocolStateDisableTransactionsFull);
		}

		/// <summary>
		/// Functional test for:
		///     CEGMConfig::CEGMConfig()
		///     CEGMConfig::GetEGMProtocolState()
		/// Test getting persisted value(s) from EEPROM for forwards and backwards compatibility.
		/// </summary>
		void GetEGMProtocolState_EepromGetTest(EGMProtocolState egmProtocolState)
		{
			// Setup.
			// Use a CEEPROMConfig instance to set the data, so the checksum is valid and the shadow buffer is written through.
			{
				CEEPROMConfig eepromConfig;
				BYTE eepromEGMProtocolState((BYTE)egmProtocolState);
				ASSERT_EQ((DWORD)sizeof(eepromEGMProtocolState), EEPROM_EGM_PROTOCOL_STATE_SIZE) << L"sizeof(eepromEGMProtocolState)";
				// Don't check the return value from CEEPROMConfig::Set(), in case the driver isn't operational.
				eepromConfig.Set(EEPROM_EGM_PROTOCOL_STATE_OFFSET, &eepromEGMProtocolState, sizeof(eepromEGMProtocolState));
			}

			// Perform operation(s) and test(s).
			CEGMConfig obj(MEMORY_EEPROM);
			obj.BuildYourself();
			ASSERT_EQ(egmProtocolState, obj.GetEGMProtocolState()) << L"GetEGMProtocolState";
		}

		TEST(EGMConfigTests, GetEGMProtocolState_EepromGet)
		{
			GetEGMProtocolState_EepromGetTest(protocolStateEnabled);
			GetEGMProtocolState_EepromGetTest(protocolStateDisabled);
			GetEGMProtocolState_EepromGetTest(protocolStateDisableTransactionsFull);
		}
// #pragma endregion GetEGMProtocolState

// #pragma region GetEGMProtocolType
		/// <summary>
		/// Functional test for:
		///     CEGMConfig::SetEGMProtocol()
		///     CEGMConfig::SetEGMProtocolState()
		///     CEGMConfig::GetEGMProtocolType()
		/// Test setting non-default value(s).
		/// </summary>
		TEST(EGMConfigTests, GetEGMProtocolType)
		{
			// Data.
			const EGMProtocol egmProtocolMin(NO_EGM_PROTOCOL);
			const EGMProtocol egmProtocolMax(SAS6xx);

			// Setup.
			CEGMConfig obj(MEMORY_NONE);

			for (int testIndex = egmProtocolMin; testIndex <= egmProtocolMax; ++testIndex)
			{
				// Perform operation(s) and test(s).
				EGMProtocol egmProtocol((EGMProtocol)testIndex);
				obj.SetEGMProtocol(egmProtocol);

				obj.SetEGMProtocolState(protocolStateDisabled);
				ASSERT_EQ(NO_EGM_PROTOCOL,
					obj.GetEGMProtocolType()) <<
					AssertMsg::FormatOnly(L"testIndex=%d protocolStateDisabled", (int)testIndex);

				obj.SetEGMProtocolState(protocolStateEnabled);
				ASSERT_EQ(egmProtocol,
					obj.GetEGMProtocolType()) <<
					AssertMsg::FormatOnly(L"testIndex=%d protocolStateEnabled", (int)testIndex);

				obj.SetEGMProtocolState(protocolStateDisableTransactionsFull);
				ASSERT_EQ(NO_EGM_PROTOCOL,
					obj.GetEGMProtocolType()) <<
					AssertMsg::FormatOnly(L"testIndex=%d protocolStateDisableTransactionsFull", (int)testIndex);
			}
		}
// #pragma endregion GetEGMProtocolType

// #pragma region GetProgressivesSupportedByEGM
		/// <summary>
		/// Functional test for:
		///     CEGMConfig::SetEGMProtocol()
		///     CEGMConfig::SetEGMProtocolState()
		///     CEGMConfig::GetProgressivesSupportedByEGM()
		/// Test setting non-default value(s).
		/// </summary>
		TEST(EGMConfigTests, GetProgressivesSupportedByEGM)
		{
			// Data.
			const EGMProtocol egmProtocolMin(NO_EGM_PROTOCOL);
			const EGMProtocol egmProtocolMax(SAS6xx);

			// Setup.
			CEGMConfig obj(MEMORY_NONE);

			for (int testIndex = egmProtocolMin; testIndex <= egmProtocolMax; ++testIndex)
			{
				// Perform operation(s) and test(s).
				EGMProtocol egmProtocol((EGMProtocol)testIndex);
				obj.SetEGMProtocol(egmProtocol);
				bool progressivesSupportedByEGM(SAS5xx == egmProtocol || SAS6xx == egmProtocol);

				obj.SetEGMProtocolState(protocolStateDisabled);
				ASSERT_FALSE(
					obj.GetProgressivesSupportedByEGM()) <<
					AssertMsg::FormatOnly(L"testIndex=%d protocolStateDisabled", (int)testIndex);

				obj.SetEGMProtocolState(protocolStateEnabled);
				ASSERT_EQ(progressivesSupportedByEGM,
					obj.GetProgressivesSupportedByEGM()) <<
					AssertMsg::FormatOnly(L"testIndex=%d protocolStateEnabled", (int)testIndex);

				obj.SetEGMProtocolState(protocolStateDisableTransactionsFull);
				ASSERT_FALSE(
					obj.GetProgressivesSupportedByEGM()) <<
					AssertMsg::FormatOnly(L"testIndex=%d protocolStateDisableTransactionsFull", (int)testIndex);
			}
		}
// #pragma endregion GetProgressivesSupportedByEGM

		TEST(EGMConfigTests, GetSentinelOnlineStateStrTest)
		{
			CEGMConfig obj(MEMORY_NONE);

			obj.SetSentinelOnlineState(sentinelComingOnline, false);
			string expResult = "Sentinel is in the process of coming online";
			string result = obj.GetSentinelOnlineStateStr();
			ASSERT_EQ(expResult, result) << "GetSentinelOnlineStateStr(sentinelComingOnline)";

			obj.SetSentinelOnlineState(sentinelComingOnlineMove, false);
			expResult = "Sentinel is in the process of completing a move";
			result = obj.GetSentinelOnlineStateStr();
			ASSERT_EQ(expResult, result) << "GetSentinelOnlineStateStr(sentinelComingOnlineMove)";

			obj.SetSentinelOnlineState(sentinelGoingOffline, false);
			expResult = "Sentinel is in the process of going offline";
			result = obj.GetSentinelOnlineStateStr();
			ASSERT_EQ(expResult, result) << "GetSentinelOnlineStateStr(sentinelGoingOffline)";

			obj.SetSentinelOnlineState(sentinelGoingOfflineDownload, false);
			expResult = "Sentinel is in the process of going offline because of a download";
			result = obj.GetSentinelOnlineStateStr();
			ASSERT_EQ(expResult, result) << "GetSentinelOnlineStateStr(sentinelGoingOfflineDownload)";

			obj.SetSentinelOnlineState(sentinelGoingOfflineMove, false);
			expResult = "Sentinel is in the process of going offline because of a machine move";
			result = obj.GetSentinelOnlineStateStr();
			ASSERT_EQ(expResult, result) << "GetSentinelOnlineStateStr(sentinelGoingOfflineMove)";

			obj.SetSentinelOnlineState(sentinelOffline, false);
			expResult = "Sentinel is offline";
			result = obj.GetSentinelOnlineStateStr();
			ASSERT_EQ(expResult, result) << "GetSentinelOnlineStateStr(sentinelOffline)";

			obj.SetSentinelOnlineState(sentinelOfflineMove, false);
			expResult = "Sentinel is offline because of a machine move";
			result = obj.GetSentinelOnlineStateStr();
			ASSERT_EQ(expResult, result) << "GetSentinelOnlineStateStr(sentinelOfflineMove)";

			obj.SetSentinelOnlineState(sentinelOnline, false);
			expResult = "Sentinel is currently online";
			result = obj.GetSentinelOnlineStateStr();
			ASSERT_EQ(expResult, result) << "GetSentinelOnlineStateStr(sentinelOnline)";
		}

		
			TEST(EGMConfigTests, GetSubscriptionDataResponseTest)
		{
			CEGMConfig obj(MEMORY_NONE);

			string expResult = "{\"EgmType\":2,\"EgmProtocol\":12,\"EgmPollingSpeed\":0,\"EgmProtocolState\":1,\"CanPerformMachineOnline\":true,\"CanPerformMachineOffline\":false,\"CanBeginMachineMove\":false,\"CanCompleteMachineMove\":false,\"SentinelOnlineState\":\"sentinelOffline\",\"SlotMastIdResponse\":\"\"}";
			string result = obj.GetSubscriptionDataResponse("", "");
			ASSERT_EQ(expResult, result) << "GetSubscriptionDataResponse";

		}
}
