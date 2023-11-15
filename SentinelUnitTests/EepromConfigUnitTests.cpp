#include "stdafx.h"

#include "EEPROMConfig.h"
#include "Hardware/NonVolatileMemoryMaps.h"

// Unit testing includes.
#include "gtest/gtest.h"

using namespace std;

	static const DWORD SECOND_VALID_OFFSET = EEPROM_SLOTMAST_ID_OFFSET;
	static const DWORD INVALID_OFFSET = EEPROM_USED_SIZE; // Last valid offset was a BYTE
	static const DWORD VALID_BOOL_OFFSET = EEPROM_DHCPAUTO_OFFSET;
	static const DWORD VALID_LONG_OFFSET = EEPROM_SLOT_NUMBER_OFFSET;
	static const DWORD VALID_WORD_OFFSET = EEPROM_GAME_DENOM_OFFSET;
	static const DWORD VALID_DWORD_OFFSET = EEPROM_TSPROTO_BAUD_OFFSET;
	static const DWORD VALID_STRING_OFFSET = EEPROM_HOST_ADDRESS_OFFSET;
	static const DWORD VALID_STRING_OFFSET_SIZE = EEPROM_HOST_ADDRESS_SIZE;

	class EepromConfigUnitTests : public ::testing::Test
	{
	public:

		// Data class used to make sure memory is released by using destructor
		class Data
		{
		public:
			Data(UINT len) : m_len(len)	{ m_data = new BYTE[m_len];	}
			virtual ~Data()	{ delete[] m_data; }

			BYTE* ptr() { return m_data; }
			UINT len() const { return m_len; }

			BYTE get(UINT n) const { return m_data[n]; }
			void set(UINT n, BYTE byte) { m_data[n] = byte; }

		private:
			BYTE* m_data;
			UINT m_len;
		};

		class CTestEEPROMConfig : public CEEPROMConfig
		{
		public:
			void testGet(DWORD offset, UINT len)
			{
				Data data(len);

				bool success = Get(offset, data.ptr(), data.len());
				ASSERT_TRUE(success) << L"Not able to 'Get' data";

				if (success)
				{
					bool matches = true;
					for (UINT n = 0; n < len && matches; n++)
					{
						BYTE shadow = GetEEPROMShadow()[offset + n];
						matches = data.get(n) == shadow;
						ASSERT_TRUE(matches) << L"Shadow EEPROM not same as data from 'Get'";
					}
				}
			}

			void testSet(DWORD offset, UINT len)
			{
				bool restored = false;

				Data original(len);
				bool success = Get(offset, original.ptr(), original.len());
				ASSERT_TRUE(success) << L"Failure getting original data while testing 'Set'";

				if (success)
				{
					Data data(original.len());
					for (UINT n = 0; n < len; n++)
					{
						data.set(n, original.get(n) + 1);
					}

					success = Set(offset, data.ptr(), data.len());
					ASSERT_TRUE(success) << L"Failure setting edited data while testing 'Set'";

					if (success)
					{
						Data check(data.len());

						success = Get(offset, check.ptr(), check.len());
						ASSERT_TRUE(success) << L"Failure getting edited data after doing 'Set'";

						if (success)
						{
							for (UINT n = 0; n < len; n++)
							{
								ASSERT_TRUE(data.get(n) == check.get(n)) << L"Data done by 'Set' not returned by 'Get'";
							}
						}
					}

					// Restore original data
					restored = Set(EEPROM_OFFSET_START, original.ptr(), original.len());
				}
				ASSERT_TRUE(restored) << L"Not able to return data to original value while testing 'Set'";
			}
		};
	};

		// 2017-01-29 The following static members/singletons and classes make unit testing default EEPROM values difficult.
		// Consistent with the Windows XP version, the Linux version writes default values to the configuration file, but does not set member variables to default values directly.
		// static BYTE CEEPROMConfig::m_eepromShadow[EEPROM_OFFSET_END];
		// static CEEPROMinator CMemoryStorage::m_eeprom;
		// static CDirectoryManager *CDirectoryManager::m_instance = NULL; -- For access to std::string CDirectoryManager::EepromBinFilePath().

		TEST_F(EepromConfigUnitTests, EepromConfigConstants)
		{
			// Test offsets and sizes -- to ensure forward and backward compatibility between versions.
			ASSERT_EQ(0x44, EEPROM_HOST_PORT_OFFSET);
			ASSERT_EQ(4, EEPROM_HOST_PORT_SIZE);

			// Test default values.
			ASSERT_EQ(3027, EEPROM_HOST_PORT_DEFAULT);
		}

		TEST_F(EepromConfigUnitTests, EepromConfigGetStartOfDataTest)
		{
			CTestEEPROMConfig eeprom;
			eeprom.testGet(EEPROM_OFFSET_START, 1);
		}

		//TEST_F(EepromConfigUnitTests, EepromConfigSetStartOfDataTest)
		//{
		//	CTestEEPROMConfig eeprom;
		//	eeprom.testSet(EEPROM_OFFSET_START, 1);
		//}

		TEST_F(EepromConfigUnitTests, EepromConfigGetValidOffsetTest)
		{
			CTestEEPROMConfig eeprom;
			eeprom.testGet(SECOND_VALID_OFFSET, 1);
		}

		//TEST_F(EepromConfigUnitTests, EepromConfigSetValidOffsetTest)
		//{
		//	CTestEEPROMConfig eeprom;
		//	eeprom.testSet(SECOND_VALID_OFFSET, 1);
		//}

		TEST_F(EepromConfigUnitTests, EepromConfigGetWordTest)
		{
			CTestEEPROMConfig eeprom;
			eeprom.testGet(VALID_WORD_OFFSET, sizeof(WORD));
		}

		//TEST_F(EepromConfigUnitTests, EepromConfigSetWordTest)
		//{
		//	CTestEEPROMConfig eeprom;
		//	eeprom.testSet(VALID_WORD_OFFSET, sizeof(WORD));
		//}

		TEST_F(EepromConfigUnitTests, EepromConfigGetDWordTest)
		{
			CTestEEPROMConfig eeprom;
			eeprom.testGet(VALID_WORD_OFFSET, sizeof(DWORD));
		}

		//TEST_F(EepromConfigUnitTests, EepromConfigSetDWordTest)
		//{
		//	CTestEEPROMConfig eeprom;
		//	eeprom.testSet(VALID_WORD_OFFSET, sizeof(DWORD));
		//}

		TEST_F(EepromConfigUnitTests, EepromConfigGetLongTest)
		{
			CTestEEPROMConfig eeprom;
			eeprom.testGet(VALID_LONG_OFFSET, sizeof(long));
		}

		//TEST_F(EepromConfigUnitTests, EepromConfigSetLongTest)
		//{
		//	CTestEEPROMConfig eeprom;
		//	eeprom.testSet(VALID_LONG_OFFSET, sizeof(long));
		//}

		TEST_F(EepromConfigUnitTests, EepromConfigGetStringTest)
		{
			CTestEEPROMConfig eeprom;
			eeprom.testGet(VALID_STRING_OFFSET, VALID_STRING_OFFSET_SIZE);
		}

		//TEST_F(EepromConfigUnitTests, EepromConfigSetStringTest)
		//{
		//	CTestEEPROMConfig eeprom;
		//	eeprom.testSet(VALID_STRING_OFFSET, VALID_STRING_OFFSET_SIZE);
		//}

		TEST_F(EepromConfigUnitTests, EepromConfigGetInvalidOffsetTest)
		{
			CEEPROMConfig eeprom;
			BYTE data = 0;
			bool success = eeprom.Get(INVALID_OFFSET, &data, sizeof(data));
			ASSERT_FALSE(success) << L"Able to 'Get' invalid offset of EEPROM data";
		}

		TEST_F(EepromConfigUnitTests, EepromConfigSetInvalidTest)
		{
			CEEPROMConfig eeprom;
			BYTE data = 0;
			bool success = eeprom.Set(INVALID_OFFSET, &data, sizeof(data));
			ASSERT_FALSE(success) << L"Able to 'Set' invalid offset of EEPROM data";
		}

		TEST_F(EepromConfigUnitTests, EepromConfigGetPastDataTest)
		{
			CEEPROMConfig eeprom;

			bool success = false;
			
			BYTE data = 0;

			success = eeprom.Get(EEPROM_USED_SIZE, &data, sizeof(data));
			ASSERT_FALSE(success) << L"Able to 'Get' past end of EEPROM data";
			
			success = eeprom.Get(EEPROM_USED_SIZE + 100, &data, sizeof(data));
			ASSERT_FALSE(success) << L"Able to 'Get' way past end of EEPROM data";
		}

		TEST_F(EepromConfigUnitTests, EepromConfigSetPastDataTest)
		{
			CEEPROMConfig eeprom;

			bool success = false;
			
			BYTE data = 0;

			success = eeprom.Set(EEPROM_USED_SIZE, &data, sizeof(data));
			ASSERT_FALSE(success) << L"Able to 'Set' past end of EEPROM data";
			
			success = eeprom.Set(EEPROM_USED_SIZE + 100, &data, sizeof(data));
			ASSERT_FALSE(success) << L"Able to 'Set' way past end of EEPROM data";
		}

		TEST_F(EepromConfigUnitTests, EepromConfigGetValidOffset_BonusEventPlayerStateTest)
		{
			CTestEEPROMConfig eeprom;
			eeprom.testGet(EEPROM_MW_BONUS_EVENT_PLAYER_STATE_OFFSET, EEPROM_MW_BONUS_EVENT_PLAYER_STATE_SIZE);
		}
