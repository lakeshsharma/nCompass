#include "stdafx.h"
#include "Card.h"
#include "Utilities.h"
#include "TestUtil.h"
#include "gtest/gtest.h"


static const byte MEMSET_DEFAULT_VALUE = 0xcd; // Value unlikely to be used for other purposes.
static const time_t CTIME_DEFAULT_VALUE(0); // Arbitrary value when any valid value is okay.
static const byte MINIMUM_CONFIG_COMM_ID = 0;
static const byte MAXIMUM_CONFIG_COMM_ID = 255; // 15 is the typical maximum configuration value used.
static const byte MINIMUM_CARD_COMM_ID = 0;
static const unsigned MAXIMUM_CARD_COMM_ID = 999; // 15 is the typical maximum value used on magnetic strip cards.

// Same values as private constants in CCard.
static const int GLOBAL_GLOBAL_CARD_LEN = 5;
static const byte GLOBAL_GLOBAL_CARD_NUM[GLOBAL_GLOBAL_CARD_LEN] = { 0x69, 0x09, 0xff, 0x00, 0x01 };

class CardUnitTests_RawData : public ::testing::Test
{
protected:
	virtual void SetUp()
	{
	}

	virtual void TearDown()
	{
	}

};
// SetCardInfoFromRawDataTest_UniversalGlobalCard
// Test CCard::SetCardInfoFromRawData() for the global setup cards supported only by Sentinel III and nCompass.
// AKA Global Card.  AKA Global Global.
TEST_F(CardUnitTests_RawData, SetCardInfoFromRawDataTest_UniversalGlobalCard)
{
	typedef struct
	{
		// Inputs.
		const byte * const m_rawData;
		const int m_rawDataLen;

		// Expected results.
		const bool m_isValidReadExpected;
		const int m_cardIDLenExpected;
		const byte m_cardIDExpected[CARD_ID_LEN];
		const int m_memcmpExpected;
		const bool m_checkCommIDExpected;
	} TestDataType;

	CCardConfig config;

	// Track 1 ORIGINAL global setup card.
	const byte rawData0[20] = {
		'%', // Start sentinel for track 1.
		'0', '0', '1',  // CommID.  Other Oasis components use this to generate/validate the XOR check nibble.  Normal range is 0..15.
		'1', '0', '5',  // Card prefix.  Normal range is 0..255.  0x69 == 105.
		'0', '0',  // Check nibble for XOR of last 6 hexadecimal digits of OCR value XORed with CommID.  Normal range is 0..15.
		'0', '9',  // Check nibble for high bit count of last 6 hexadecimal digits of OCR value.  Normal range is 0..15.
		'1', '6', '7', '1', '1', '6', '8', '1',  // Last 6 hexadecimal digits of OCR value as 8 decimal digits. 0xFF0001 == 16711681.  Normal range is 0..16777215 (AKA 2^24 - 1).
		'?', // End sentinel.
		// LRC (longitudinal redudancy check) value omitted.  Most card readers do not report this value.
	};

	// Track 2 ORIGINAL global setup card.
	const byte rawData1[20] = {
		';', // Start sentinel for track 2.
		'0', '0', '1',  // CommID.  Other Oasis components use this to generate/validate the XOR check nibble.  Normal range is 0..15.
		'1', '0', '5',  // Card prefix.  Normal range is 0..255.  0x69 == 105.
		'0', '0',  // Check nibble for XOR of last 6 hexadecimal digits of OCR value XORed with CommID.  Normal range is 0..15.
		'0', '9',  // Check nibble for high bit count of last 6 hexadecimal digits of OCR value.  Normal range is 0..15.
		'1', '6', '7', '1', '1', '6', '8', '1',  // Last 6 hexadecimal digits of OCR value as 8 decimal digits. 0xFF0001 == 16711681.  Normal range is 0..16777215 (AKA 2^24 - 1).
		'?', // End sentinel.
		// LRC (longitudinal redudancy check) value omitted.  Most card readers do not report this value.
	};

	const TestDataType TEST_DATA[] =
	{
		{ rawData0, _countof(rawData0), true, CARD_ID_LEN, { 0x69, 0x09, 0xFF, 0x00, 0x01 }, MEMCMP_EQUAL, true },
		{ rawData1, _countof(rawData1), true, CARD_ID_LEN, { 0x69, 0x09, 0xFF, 0x00, 0x01 }, MEMCMP_EQUAL, true },
	};

	for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
	{
		// Setup.
		CCard card(CTIME_DEFAULT_VALUE, CTIME_DEFAULT_VALUE);

		// Perform operation(s) to be tested.
		card.SetCardInfoFromRawData(TEST_DATA[testIndex].m_rawData, TEST_DATA[testIndex].m_rawDataLen, config);

		// Test #1.
		bool isValidReadResult = card.IsValidRead();

		// Test #2.
		int cardIDLen;
		memset(&cardIDLen, MEMSET_DEFAULT_VALUE, sizeof(cardIDLen));
		const byte *cardID = card.GetCardID(cardIDLen);

		// Test #3.
		bool cardIDCorrect = false;
		if (NULL == cardID && NULL == TEST_DATA[testIndex].m_cardIDExpected)
		{
			cardIDCorrect = true;
		}
		else if (NULL != cardID && NULL != TEST_DATA[testIndex].m_cardIDExpected)
		{
			cardIDCorrect = true;
		}
		else
		{
			cardIDCorrect = false;
		}

		// Test #4.
		int memcmpResult = TEST_DATA[testIndex].m_memcmpExpected; // memcmp() result doesn't matter if cardIDLen is wrong.
		if (cardIDLen == TEST_DATA[testIndex].m_cardIDLenExpected)
		{
			memcmpResult = memcmp(cardID, TEST_DATA[testIndex].m_cardIDExpected, TEST_DATA[testIndex].m_cardIDLenExpected);
		}

		// Test #5.
		// For global setup cards, CheckCommID needs to return true for all commID values.
		bool checkCommIDResult = true;
		byte checkCommIDCommID = 0; // Not used unless checkCommIDResult = false.
		// This for loop has maximum value same as maximum for byte type.
		for (byte commID = MINIMUM_CONFIG_COMM_ID; ; ++commID)
		{
			if (!card.CheckCommID(commID))
			{
				checkCommIDCommID = commID;
				checkCommIDResult = false;
				break;
			}

			if (commID == MAXIMUM_CONFIG_COMM_ID)
			{
				break;
			}
		}

		// Report #1.
		ASSERT_EQ(isValidReadResult, TEST_DATA[testIndex].m_isValidReadExpected);
		// Report #2.
		ASSERT_EQ(cardIDLen, TEST_DATA[testIndex].m_cardIDLenExpected);
		// Report #3.
		ASSERT_TRUE(cardIDCorrect);
		// Report #4.
		ASSERT_EQ(memcmpResult, TEST_DATA[testIndex].m_memcmpExpected);
		// Report #5.
		ASSERT_EQ(checkCommIDResult, TEST_DATA[testIndex].m_checkCommIDExpected);

		// Cleanup.
	}
}

// SetCardInfoFromRawDataTest_Sentinel3GlobalCard
// Test CCard::SetCardInfoFromRawData() for the global setup cards supported only by Sentinel III and nCompass.
// Sentinel III and nCompass support any CommID for global setup cards.
// Note that some older documentation refers to the CommID field as a Site Number field.
//     - As of 2013-03-12, only CommID values have been stored in the field.
TEST_F(CardUnitTests_RawData, SetCardInfoFromRawDataTest_Sentinel3GlobalCard)
{
	typedef struct
	{
		// Inputs.
		const byte * const m_rawData;
		const int m_rawDataLen;

		// Expected results.
		const bool m_isValidReadExpected;
		const int m_cardIDLenExpected;
		const byte m_cardIDExpected[CARD_ID_LEN];
		const int m_memcmpExpected;
		const bool m_checkCommIDExpected;
	} TestDataType;

	CCardConfig config;

	// Track 1 Sentinel III global setup card.
	const byte rawData0[20] = {
		'%', // Start sentinel for track 1.
		// For the CommID field, substitute values 000..999 at runtime.
		'0', '0', '0',  // CommID.  Other Oasis components use this to generate/validate the XOR check nibble.  Normal range is 0..15.
		'1', '0', '5',  // Card prefix.  Normal range is 0..255.  0x69 == 105.
		'0', '0',  // Check nibble for XOR of last 6 hexadecimal digits of OCR value XORed with CommID.  Normal range is 0..15.
		'0', '9',  // Check nibble for high bit count of last 6 hexadecimal digits of OCR value.  Normal range is 0..15.
		'1', '6', '7', '1', '1', '6', '8', '1',  // Last 6 hexadecimal digits of OCR value as 8 decimal digits. 0xFF0001 == 16711681.  Normal range is 0..16777215 (AKA 2^24 - 1).
		'?', // End sentinel.
		// LRC (longitudinal redudancy check) value omitted.  Most card readers do not report this value.
	};

	// Track 2 Sentinel III global setup card.
	const byte rawData1[20] = {
		';', // Start sentinel for track 2.
		// For the CommID field, substitute values 000..999 at runtime.
		'0', '0', '0',  // CommID.  Other Oasis components use this to generate/validate the XOR check nibble.  Normal range is 0..15.
		'1', '0', '5',  // Card prefix.  Normal range is 0..255.  0x69 == 105.
		'0', '0',  // Check nibble for XOR of last 6 hexadecimal digits of OCR value XORed with CommID.  Normal range is 0..15.
		'0', '9',  // Check nibble for high bit count of last 6 hexadecimal digits of OCR value.  Normal range is 0..15.
		'1', '6', '7', '1', '1', '6', '8', '1',  // Last 6 hexadecimal digits of OCR value as 8 decimal digits. 0xFF0001 == 16711681.  Normal range is 0..16777215 (AKA 2^24 - 1).
		'?', // End sentinel.
		// LRC (longitudinal redudancy check) value omitted.  Most card readers do not report this value.
	};

	const TestDataType TEST_DATA[] =
	{
		{ rawData0, _countof(rawData0), true, CARD_ID_LEN, { 0x69, 0x09, 0xFF, 0x00, 0x01 }, MEMCMP_EQUAL, true },
		{ rawData1, _countof(rawData1), true, CARD_ID_LEN, { 0x69, 0x09, 0xFF, 0x00, 0x01 }, MEMCMP_EQUAL, true },
	};

	for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
	{
		// Test each CommID value that the Oasis magnetic card format supports.
		for (unsigned testCardCommID = MINIMUM_CARD_COMM_ID; testCardCommID < MAXIMUM_CARD_COMM_ID; ++testCardCommID)
		{
			// Setup.
			CCard card(CTIME_DEFAULT_VALUE, CTIME_DEFAULT_VALUE);

			byte *rawData = new byte[TEST_DATA[testIndex].m_rawDataLen];
			memcpy(rawData, TEST_DATA[testIndex].m_rawData, TEST_DATA[testIndex].m_rawDataLen);
			rawData[1] = (byte)('0' + testCardCommID / 100 % 10); // Most significant digit of CommID field.
			rawData[2] = (byte)('0' + testCardCommID / 10 % 10); // Middle digit of CommID field.
			rawData[3] = (byte)('0' + testCardCommID % 10); // Least significant digit of CommID field.

			// Perform operation(s) to be tested.
			card.SetCardInfoFromRawData(rawData, TEST_DATA[testIndex].m_rawDataLen, config);

			ASSERT_TRUE(card.IsValidRead());

			int cardIDLen = 0;
			const byte *cardID = card.GetCardID(cardIDLen);
			ASSERT_TRUE(cardID != NULL);
			ASSERT_EQ(TEST_DATA[testIndex].m_cardIDLenExpected, cardIDLen);

			int result = memcmp(cardID, TEST_DATA[testIndex].m_cardIDExpected, TEST_DATA[testIndex].m_cardIDLenExpected);
			ASSERT_EQ(0, result);

			// For global setup cards, CheckCommID needs to return true for all commID values.
			// This for loop has maximum value same as maximum for byte type.
			for (unsigned commID = MINIMUM_CONFIG_COMM_ID; commID <= MAXIMUM_CONFIG_COMM_ID; ++commID)
			{
				ASSERT_TRUE(card.CheckCommID((byte)commID));
			}

			// Cleanup.
			delete[] rawData;
		}
	}
}

// SetCardInfoFromRawDataTest_NullPointer
// Test CCard::SetCardInfoFromRawData() passing a NULL pointer for  the rawData parameter.
TEST_F(CardUnitTests_RawData, SetCardInfoFromRawDataTest_NullPointer)
{
	typedef struct
	{
		// Inputs.
		const byte * const m_rawData;
		const int m_rawDataLen;

		// Expected results.
		const bool m_isValidReadExpected;
		const int m_cardIDLenExpected;
		const byte *m_cardIDExpected;
		const int m_memcmpExpected;
		const byte m_checkCommIDCommIDTrue; // CommID expected to return true.  All other CommID values are expected to return false.
	} TestDataType;

	CCardConfig config;

	const TestDataType TEST_DATA[] =
	{
		{ NULL, 0, false, 0, NULL, MEMCMP_EQUAL, 0 },
	};

	for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
	{
		// Setup.
		CCard card(CTIME_DEFAULT_VALUE, CTIME_DEFAULT_VALUE);

		// Perform operation(s) to be tested.
		card.SetCardInfoFromRawData(TEST_DATA[testIndex].m_rawData, TEST_DATA[testIndex].m_rawDataLen, config);

		// Test #1.
		bool isValidReadResult = card.IsValidRead();

		// Test #2.
		int cardIDLen;
		memset(&cardIDLen, MEMSET_DEFAULT_VALUE, sizeof(cardIDLen));
		const byte *cardID = card.GetCardID(cardIDLen);

		// Test #3.
		bool cardIDCorrect = false;
		if (NULL == cardID && NULL == TEST_DATA[testIndex].m_cardIDExpected)
		{
			cardIDCorrect = true;
		}
		else if (NULL != cardID && NULL != TEST_DATA[testIndex].m_cardIDExpected)
		{
			cardIDCorrect = true;
		}
		else
		{
			cardIDCorrect = false;
		}

		// Test #4.
		int memcmpResult = TEST_DATA[testIndex].m_memcmpExpected; // memcmp() result doesn't matter if cardIDLen is wrong.
		if (cardIDLen == TEST_DATA[testIndex].m_cardIDLenExpected)
		{
			memcmpResult = memcmp(cardID, TEST_DATA[testIndex].m_cardIDExpected, TEST_DATA[testIndex].m_cardIDLenExpected);
		}

		// Test #5.
		bool checkCommIDExpected = true;
		bool checkCommIDResult = true;
		byte checkCommIDCommID = 0; // Not used unless checkCommIDResult = false.
		if (TEST_DATA[testIndex].m_isValidReadExpected)
		{
			// For global setup cards, CheckCommID needs to return true for all commID values.
			checkCommIDExpected = true;
			// This for loop has maximum value same as maximum for byte type.
			for (byte commID = MINIMUM_CONFIG_COMM_ID; ; ++commID)
			{
				if (!card.CheckCommID(commID))
				{
					checkCommIDCommID = commID;
					checkCommIDResult = false;
					break;
				}

				if (commID == MAXIMUM_CONFIG_COMM_ID)
				{
					break;
				}
			}
		}
		else
		{
			// For cards other than global setup cards, CheckCommID needs to return true only for the correct commID value.
			// This for loop has its maximum value same as the maximum for the byte type.
			for (byte commID = MINIMUM_CONFIG_COMM_ID; ; ++commID)
			{
				checkCommIDCommID = commID;
				checkCommIDExpected = commID == TEST_DATA[testIndex].m_checkCommIDCommIDTrue;
				checkCommIDResult = card.CheckCommID(commID);

				if (checkCommIDResult != checkCommIDExpected)
				{
					break;
				}

				if (commID == MAXIMUM_CONFIG_COMM_ID)
				{
					break;
				}
			}
		}
		// Report #1.
		ASSERT_EQ(isValidReadResult, TEST_DATA[testIndex].m_isValidReadExpected);
		// Report #2.
		ASSERT_EQ(cardIDLen, TEST_DATA[testIndex].m_cardIDLenExpected);
		// Report #3.
		ASSERT_TRUE(cardIDCorrect);
		// Report #4.
		ASSERT_EQ(memcmpResult, TEST_DATA[testIndex].m_memcmpExpected);
		// Report #5.
		ASSERT_EQ(checkCommIDResult, checkCommIDExpected);

		// Cleanup.
	}
}

// CCardSetCardInfoFromRawData_InvalidGlobalCards
// Test CCard::SetCardInfoFromRawData().
TEST_F(CardUnitTests_RawData, SetCardInfoFromRawDataTest_InvalidGlobalCards)
{
	typedef struct
	{
		// Inputs.
		const byte * const m_rawData;
		const int m_rawDataLen;

		// Expected results.
		const bool m_isValidReadExpected;
		const int m_cardIDLenExpected;
		const byte *m_cardIDExpected;
		const int m_memcmpExpected;
		const byte m_checkCommIDCommIDTrue; // CommID expected to return true.  All other CommID values are expected to return false.
	} TestDataType;

	CCardConfig config;

	const byte rawData0[1] = {}; // Test using data length of 0.

	const byte rawData1[20] = { // Track 2 ORIGINAL global setup card with wrong start sentinel.
		'?', // Wrong start sentinel for any track.
		'0', '0', '1', // CommID.  Other Oasis components use this to generate/validate the XOR check nibble.  Normal range is 0..15.
		'1', '0', '5', // Card prefix.  Normal range is 0..255.  0x69 == 105.
		'0', '0', // Check nibble for XOR of last 6 hexadecimal digits of OCR value XORed with CommID.  Normal range is 0..15.
		'0', '9', // Check nibble for high bit count of last 6 hexadecimal digits of OCR value.  Normal range is 0..15.
		'1', '6', '7', '1', '1', '6', '8', '1', // Last 6 hexadecimal digits of OCR value as 8 decimal digits. 0xFF0001 == 16711681.  Normal range is 0..16777215 (AKA 2^24 - 1).
		'?', // End sentinel.
		// LRC (longitudinal redudancy check) value omitted.  Most card readers do not report this value.
	};

	// For Sentinel III/nCompass all CommID values are valid for global setup cards.
	const byte rawData2[20] = { // Track 2 ORIGINAL global setup card with wrong CommID.
		';', // Start sentinel for track 2.
		'0', '0', '2',  // CommID.  Other Oasis components use this to generate/validate the XOR check nibble.  Normal range is 0..15.
		'1', '0', '5',  // Card prefix.  Normal range is 0..255.  0x69 == 105.
		'0', '0',  // Check nibble for XOR of last 6 hexadecimal digits of OCR value XORed with CommID.  Normal range is 0..15.
		'0', '9',  // Check nibble for high bit count of last 6 hexadecimal digits of OCR value.  Normal range is 0..15.
		'1', '6', '7', '1', '1', '6', '8', '1',  // Last 6 hexadecimal digits of OCR value as 8 decimal digits. 0xFF0001 == 16711681.  Normal range is 0..16777215 (AKA 2^24 - 1).
		'?', // End sentinel.
		// LRC (longitudinal redudancy check) value omitted.  Most card readers do not report this value.
	};
	const byte cardIDExpected2[CARD_ID_LEN] = { 0x69, 0x09, 0xFF, 0x00, 0x01 };

	// For Sentinel III/nCompass, wrap around card prefixes are valid.
	const byte rawData3[20] = { // Track 2 ORIGINAL global setup card with wrap around card prefix.
		';', // Start sentinel for track 2.
		'0', '0', '1',  // CommID.  Other Oasis components use this to generate/validate the XOR check nibble.  Normal range is 0..15.
		'3', '6', '1',  // Card prefix.  Normal range is 0..255.  0x69 == 105.
		'0', '0',  // Check nibble for XOR of last 6 hexadecimal digits of OCR value XORed with CommID.  Normal range is 0..15.
		'0', '9',  // Check nibble for high bit count of last 6 hexadecimal digits of OCR value.  Normal range is 0..15.
		'1', '6', '7', '1', '1', '6', '8', '1',  // Last 6 hexadecimal digits of OCR value as 8 decimal digits. 0xFF0001 == 16711681.  Normal range is 0..16777215 (AKA 2^24 - 1).
		'?', // End sentinel.
		// LRC (longitudinal redudancy check) value omitted.  Most card readers do not report this value.
	};
	const byte cardIDExpected3[CARD_ID_LEN] = { 0x69, 0x09, 0xFF, 0x00, 0x01 };

	// The Oasis magnetic card stripe information does not have any checks for card prefix.
	// For Oasis magnetic card stripe information, card prefixes are checked against a list of valid prefixes at the
	// (Sentinel/nCompass) application level.
	const byte rawData4[20] = { // Track 2 ORIGINAL global setup card with wrong card prefix.
		';', // Start sentinel for track 2.
		'0', '0', '1',  // CommID.  Other Oasis components use this to generate/validate the XOR check nibble.  Normal range is 0..15.
		'1', '0', '6',  // Card prefix.  Normal range is 0..255.  0x69 == 105.
		'0', '0',  // Check nibble for XOR of last 6 hexadecimal digits of OCR value XORed with CommID.  Normal range is 0..15.
		'0', '9',  // Check nibble for high bit count of last 6 hexadecimal digits of OCR value.  Normal range is 0..15.
		'1', '6', '7', '1', '1', '6', '8', '1',  // Last 6 hexadecimal digits of OCR value as 8 decimal digits. 0xFF0001 == 16711681.  Normal range is 0..16777215 (AKA 2^24 - 1).
		'?', // End sentinel.
		// LRC (longitudinal redudancy check) value omitted.  Most card readers do not report this value.
	};
	const byte cardIDExpected4[CARD_ID_LEN] = { 0x6a, 0x09, 0xFF, 0x00, 0x01 };

	// For Sentinel III/nCompass, all "check nibble for XOR values" (high nibble of check byte) are valid for Sentinel III/nCompass global setup cards.
	const byte rawData5[20] = { // Track 2 ORIGINAL global setup card with wrong Check nibble for XOR.
		';', // Start sentinel for track 2.
		'0', '0', '1',  // CommID.  Other Oasis components use this to generate/validate the XOR check nibble.  Normal range is 0..15.
		'1', '0', '5',  // Card prefix.  Normal range is 0..255.  0x69 == 105.
		'0', '1',  // Check nibble for XOR of last 6 hexadecimal digits of OCR value XORed with CommID.  Normal range is 0..15.
		'0', '9',  // Check nibble for high bit count of last 6 hexadecimal digits of OCR value.  Normal range is 0..15.
		'1', '6', '7', '1', '1', '6', '8', '1',  // Last 6 hexadecimal digits of OCR value as 8 decimal digits. 0xFF0001 == 16711681.  Normal range is 0..16777215 (AKA 2^24 - 1).
		'?', // End sentinel.
		// LRC (longitudinal redudancy check) value omitted.  Most card readers do not report this value.
	};
	const byte cardIDExpected5[CARD_ID_LEN] = { 0x69, 0x19, 0xFF, 0x00, 0x01 };

	// For Sentinel III/nCompass, wrap around "check nibble for XOR" values are valid.
	// For Sentinel III/nCompass, all "check nibble for XOR" values (high nibble of check byte) are valid for Sentinel III/nCompass global setup cards.
	const byte rawData6[20] = { // Track 2 ORIGINAL global setup card with wrong Check nibble for XOR.
		';', // Start sentinel for track 2.
		'0', '0', '1',  // CommID.  Other Oasis components use this to generate/validate the XOR check nibble.  Normal range is 0..15.
		'1', '0', '5',  // Card prefix.  Normal range is 0..255.  0x69 == 105.
		'6', '4',  // Check nibble for XOR of last 6 hexadecimal digits of OCR value XORed with CommID.  Normal range is 0..15.
		'0', '9',  // Check nibble for high bit count of last 6 hexadecimal digits of OCR value.  Normal range is 0..15.
		'1', '6', '7', '1', '1', '6', '8', '1',  // Last 6 hexadecimal digits of OCR value as 8 decimal digits. 0xFF0001 == 16711681.  Normal range is 0..16777215 (AKA 2^24 - 1).
		'?', // End sentinel.
		// LRC (longitudinal redudancy check) value omitted.  Most card readers do not report this value.
	};
	const byte cardIDExpected6[CARD_ID_LEN] = { 0x69, 0x09, 0xFF, 0x00, 0x01 };

	const byte rawData7[20] = { // Track 2 ORIGINAL global setup card with wrong check nibble for high bit count.
		';', // Start sentinel for track 2.
		'0', '0', '1',  // CommID.  Other Oasis components use this to generate/validate the XOR check nibble.  Normal range is 0..15.
		'1', '0', '5',  // Card prefix.  Normal range is 0..255.  0x69 == 105.
		'0', '0',  // Check nibble for XOR of last 6 hexadecimal digits of OCR value XORed with CommID.  Normal range is 0..15.
		'0', '8',  // Check nibble for high bit count of last 6 hexadecimal digits of OCR value.  Normal range is 0..15.
		'1', '6', '7', '1', '1', '6', '8', '1',  // Last 6 hexadecimal digits of OCR value as 8 decimal digits. 0xFF0001 == 16711681.  Normal range is 0..16777215 (AKA 2^24 - 1).
		'?', // End sentinel.
		// LRC (longitudinal redudancy check) value omitted.  Most card readers do not report this value.
	};

	// For Sentinel III/nCompass, wrap around "check nibble for high bit count" values are valid.
	const byte rawData8[20] = { // Track 2 ORIGINAL global setup card with wrap around check nibble for high bit count.
		';', // Start sentinel for track 2.
		'0', '0', '1',  // CommID.  Other Oasis components use this to generate/validate the XOR check nibble.  Normal range is 0..15.
		'1', '0', '5',  // Card prefix.  Normal range is 0..255.  0x69 == 105.
		'6', '4',  // Check nibble for XOR of last 6 hexadecimal digits of OCR value XORed with CommID.  Normal range is 0..15.
		'0', '9',  // Check nibble for high bit count of last 6 hexadecimal digits of OCR value.  Normal range is 0..15.
		'1', '6', '7', '1', '1', '6', '8', '1',  // Last 6 hexadecimal digits of OCR value as 8 decimal digits. 0xFF0001 == 16711681.  Normal range is 0..16777215 (AKA 2^24 - 1).
		'?', // End sentinel.
		// LRC (longitudinal redudancy check) value omitted.  Most card readers do not report this value.
	};
	const byte cardIDExpected8[CARD_ID_LEN] = { 0x69, 0x09, 0xFF, 0x00, 0x01 };

	const byte rawData9[20] = { // Track 2 ORIGINAL global setup card with wrong digit in last 6 hexadecimal digits.
		';', // Start sentinel for track 2.
		'0', '0', '1',  // CommID.  Other Oasis components use this to generate/validate the XOR check nibble.  Normal range is 0..15.
		'1', '0', '5',  // Card prefix.  Normal range is 0..255.  0x69 == 105.
		'0', '0',  // Check nibble for XOR of last 6 hexadecimal digits of OCR value XORed with CommID.  Normal range is 0..15.
		'0', '9',  // Check nibble for high bit count of last 6 hexadecimal digits of OCR value.  Normal range is 0..15.
		'1', '6', '7', '1', '1', '6', '8', '0',  // Last 6 hexadecimal digits of OCR value as 8 decimal digits. 0xFF0001 == 16711681.  Normal range is 0..16777215 (AKA 2^24 - 1).
		'?', // End sentinel.
		// LRC (longitudinal redudancy check) value omitted.  Most card readers do not report this value.
	};

	const byte rawData10[20] = { // Track 2 ORIGINAL global setup card with wrong end sentinel.
		';', // Start sentinel for track 2.
		'0', '0', '1',  // CommID.  Other Oasis components use this to generate/validate the XOR check nibble.  Normal range is 0..15.
		'1', '0', '5',  // Card prefix.  Normal range is 0..255.  0x69 == 105.
		'0', '0',  // Check nibble for XOR of last 6 hexadecimal digits of OCR value XORed with CommID.  Normal range is 0..15.
		'0', '9',  // Check nibble for high bit count of last 6 hexadecimal digits of OCR value.  Normal range is 0..15.
		'1', '6', '7', '1', '1', '6', '8', '1',  // Last 6 hexadecimal digits of OCR value as 8 decimal digits. 0xFF0001 == 16711681.  Normal range is 0..16777215 (AKA 2^24 - 1).
		';', // End sentinel.
		// LRC (longitudinal redudancy check) value omitted.  Most card readers do not report this value.
	};

	const byte rawData11[21] = { // Track 2 ORIGINAL global setup card with extra data before end sentinel.
		';', // Start sentinel for track 2.
		'0', '0', '1',  // CommID.  Other Oasis components use this to generate/validate the XOR check nibble.  Normal range is 0..15.
		'1', '0', '5',  // Card prefix.  Normal range is 0..255.  0x69 == 105.
		'0', '0',  // Check nibble for XOR of last 6 hexadecimal digits of OCR value XORed with CommID.  Normal range is 0..15.
		'0', '9',  // Check nibble for high bit count of last 6 hexadecimal digits of OCR value.  Normal range is 0..15.
		'1', '6', '7', '1', '1', '6', '8', '1',  // Last 6 hexadecimal digits of OCR value as 8 decimal digits. 0xFF0001 == 16711681.  Normal range is 0..16777215 (AKA 2^24 - 1).
		'1', // Extra data.
		'?', // End sentinel.
		// LRC (longitudinal redudancy check) value omitted.  Most card readers do not report this value.
	};

	// For Sentinel III/nCompass, extra data after end sentinel is valid.
	const byte rawData12[21] = { // Track 2 ORIGINAL global setup card with extra data after end sentinel.
		';', // Start sentinel for track 2.
		'0', '0', '1',  // CommID.  Other Oasis components use this to generate/validate the XOR check nibble.  Normal range is 0..15.
		'1', '0', '5',  // Card prefix.  Normal range is 0..255.  0x69 == 105.
		'0', '0',  // Check nibble for XOR of last 6 hexadecimal digits of OCR value XORed with CommID.  Normal range is 0..15.
		'0', '9',  // Check nibble for high bit count of last 6 hexadecimal digits of OCR value.  Normal range is 0..15.
		'1', '6', '7', '1', '1', '6', '8', '1',  // Last 6 hexadecimal digits of OCR value as 8 decimal digits. 0xFF0001 == 16711681.  Normal range is 0..16777215 (AKA 2^24 - 1).
		'?', // End sentinel.
		'?', // Extra data.
		// LRC (longitudinal redudancy check) value omitted.  Most card readers do not report this value.
	};
	const byte cardIDExpected12[CARD_ID_LEN] = { 0x69, 0x09, 0xFF, 0x00, 0x01 };

	const TestDataType TEST_DATA[] =
	{
		{ rawData0 + 1, 0, false, 0, NULL, MEMCMP_EQUAL, 0 }, // m_rawData points to where zero bytes of data are left.
		{ rawData1, _countof(rawData1), false, 0, NULL, MEMCMP_EQUAL, 0 },
		{ rawData2, _countof(rawData2), true, _countof(cardIDExpected2), cardIDExpected2, MEMCMP_EQUAL, 0 },
		{ rawData3, _countof(rawData3), true, _countof(cardIDExpected3), cardIDExpected3, MEMCMP_EQUAL, 0 },
		// Data for test index 4 is not a gobal setup card, so only CheckCommID() is only valid for one CommID value.
		{ rawData4, _countof(rawData4), true, _countof(cardIDExpected4), cardIDExpected4, MEMCMP_EQUAL, 1 },
		// Data for test index 5 is not a gobal setup card, so only CheckCommID() is only valid for one CommID value.
		{ rawData5, _countof(rawData5), true, _countof(cardIDExpected5), cardIDExpected5, MEMCMP_EQUAL, 1 },
		{ rawData6, _countof(rawData6), true, _countof(cardIDExpected6), cardIDExpected6, MEMCMP_EQUAL, 0 },
		{ rawData7, _countof(rawData7), false, 0, NULL, MEMCMP_EQUAL, 0 },
		{ rawData8, _countof(rawData8), true, _countof(cardIDExpected8), cardIDExpected8, MEMCMP_EQUAL, 0 },
		{ rawData9, _countof(rawData9), false, 0, NULL, MEMCMP_EQUAL, 0 },
		{ rawData10, _countof(rawData10), false, 0, NULL, MEMCMP_EQUAL, 0 },
		{ rawData11, _countof(rawData11), false, 0, NULL, MEMCMP_EQUAL, 0 },
		{ rawData12, _countof(rawData12), true, _countof(cardIDExpected12), cardIDExpected12, MEMCMP_EQUAL, 0 },
	};

	for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
	{
		// Setup.
		CCard card(CTIME_DEFAULT_VALUE, CTIME_DEFAULT_VALUE);

		// Perform operation(s) to be tested.
		card.SetCardInfoFromRawData(TEST_DATA[testIndex].m_rawData, TEST_DATA[testIndex].m_rawDataLen, config);

		// Test #1.
		bool isValidReadResult = card.IsValidRead();

		// Test #2.
		int cardIDLen;
		memset(&cardIDLen, MEMSET_DEFAULT_VALUE, sizeof(cardIDLen));
		const byte *cardID = card.GetCardID(cardIDLen);

		// Test #3.
		bool cardIDCorrect = false;
		if (NULL == cardID && NULL == TEST_DATA[testIndex].m_cardIDExpected)
		{
			cardIDCorrect = true;
		}
		else if (NULL != cardID && NULL != TEST_DATA[testIndex].m_cardIDExpected)
		{
			cardIDCorrect = true;
		}
		else
		{
			cardIDCorrect = false;
		}

		// Test #4.
		int memcmpResult = TEST_DATA[testIndex].m_memcmpExpected; // memcmp() result doesn't matter if cardIDLen is wrong.
		if (cardIDLen == TEST_DATA[testIndex].m_cardIDLenExpected)
		{
			memcmpResult = memcmp(cardID, TEST_DATA[testIndex].m_cardIDExpected, TEST_DATA[testIndex].m_cardIDLenExpected);
		}

		// Test #5.
		bool checkCommIDExpected = true;
		bool checkCommIDResult = true;
		byte checkCommIDCommID = 0; // Not used unless checkCommIDResult = false.
		// Was a valid global card expected?
		if (GLOBAL_GLOBAL_CARD_LEN == TEST_DATA[testIndex].m_cardIDLenExpected &&
			MEMCMP_EQUAL == memcmp(TEST_DATA[testIndex].m_cardIDExpected, GLOBAL_GLOBAL_CARD_NUM, GLOBAL_GLOBAL_CARD_LEN))
		{
			// For global setup cards, CheckCommID needs to return true for all commID values.
			checkCommIDExpected = true;
			// This for loop has maximum value same as maximum for byte type.
			for (byte commID = MINIMUM_CONFIG_COMM_ID; ; ++commID)
			{
				if (!card.CheckCommID(commID))
				{
					checkCommIDCommID = commID;
					checkCommIDResult = false;
					break;
				}

				if (commID == MAXIMUM_CONFIG_COMM_ID)
				{
					break;
				}
			}
		}
		else
		{
			// For cards other than global setup cards, CheckCommID needs to return true only for the correct commID value.
			// This for loop has its maximum value same as the maximum for the byte type.
			for (byte commID = MINIMUM_CONFIG_COMM_ID; ; ++commID)
			{
				checkCommIDCommID = commID;
				checkCommIDExpected = commID == TEST_DATA[testIndex].m_checkCommIDCommIDTrue;
				checkCommIDResult = card.CheckCommID(commID);

				if (checkCommIDResult != checkCommIDExpected)
				{
					break;
				}

				if (commID == MAXIMUM_CONFIG_COMM_ID)
				{
					break;
				}
			}
		}
		// Report #1.
		ASSERT_EQ(isValidReadResult, TEST_DATA[testIndex].m_isValidReadExpected);
		// Report #2.
		ASSERT_EQ(cardIDLen, TEST_DATA[testIndex].m_cardIDLenExpected);
		// Report #3.
		ASSERT_TRUE(cardIDCorrect);
		// Report #4.
		ASSERT_EQ(memcmpResult, TEST_DATA[testIndex].m_memcmpExpected);
		// Report #5.
		ASSERT_EQ(checkCommIDResult, checkCommIDExpected);

		// Cleanup.
	}
}

// CCardSetCardInfoFromRawData_NotEnoughData
// Test CCard::SetCardInfoFromRawData().
TEST_F(CardUnitTests_RawData, CCardSetCardInfoFromRawData_NotEnoughData)
{
	typedef struct
	{
		// Inputs.
		const byte * const m_rawData;
		const int m_rawDataLen;

		// Expected results.
		const bool m_isValidReadExpected;
		const int m_cardIDLenExpected;
		const byte *m_cardIDExpected;
		const int m_memcmpExpected;
		const byte m_checkCommIDCommIDTrue; // CommID expected to return true.  All other CommID values are expected to return false.
	} TestDataType;

	CCardConfig config;

	// Track 1 ORIGINAL global setup card.
	const byte rawData0[20] = {
		'%', // Start sentinel for track 1.
		'0', '0', '1',  // CommID.  Other Oasis components use this to generate/validate the XOR check nibble.  Normal range is 0..15.
		'1', '0', '5',  // Card prefix.  Normal range is 0..255.  0x69 == 105.
		'0', '0',  // Check nibble for XOR of last 6 hexadecimal digits of OCR value XORed with CommID.  Normal range is 0..15.
		'0', '9',  // Check nibble for high bit count of last 6 hexadecimal digits of OCR value.  Normal range is 0..15.
		'1', '6', '7', '1', '1', '6', '8', '1',  // Last 6 hexadecimal digits of OCR value as 8 decimal digits. 0xFF0001 == 16711681.  Normal range is 0..16777215 (AKA 2^24 - 1).
		'?', // End sentinel.
		// LRC (longitudinal redudancy check) value omitted.  Most card readers do not report this value.
	};

	// Track 2 ORIGINAL global setup card.
	const byte rawData1[20] = {
		';', // Start sentinel for track 2.
		'0', '0', '1',  // CommID.  Other Oasis components use this to generate/validate the XOR check nibble.  Normal range is 0..15.
		'1', '0', '5',  // Card prefix.  Normal range is 0..255.  0x69 == 105.
		'0', '0',  // Check nibble for XOR of last 6 hexadecimal digits of OCR value XORed with CommID.  Normal range is 0..15.
		'0', '9',  // Check nibble for high bit count of last 6 hexadecimal digits of OCR value.  Normal range is 0..15.
		'1', '6', '7', '1', '1', '6', '8', '1',  // Last 6 hexadecimal digits of OCR value as 8 decimal digits. 0xFF0001 == 16711681.  Normal range is 0..16777215 (AKA 2^24 - 1).
		'?', // End sentinel.
		// LRC (longitudinal redudancy check) value omitted.  Most card readers do not report this value.
	};

	const TestDataType TEST_DATA[] =
	{
		{ rawData0, _countof(rawData0), false, 0, NULL, MEMCMP_EQUAL, 0 },
		{ rawData1, _countof(rawData1), false, 0, NULL, MEMCMP_EQUAL, 0 },
	};

	for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
	{
		for (int rawDataLen = 0; rawDataLen < TEST_DATA[testIndex].m_rawDataLen; ++rawDataLen)
		{
			// Setup.
			CCard card(CTIME_DEFAULT_VALUE, CTIME_DEFAULT_VALUE);

			// Perform operation(s) to be tested.
			card.SetCardInfoFromRawData(TEST_DATA[testIndex].m_rawData, rawDataLen, config);

			// Test #1.
			bool isValidReadResult = card.IsValidRead();

			// Test #2.
			int cardIDLen;
			memset(&cardIDLen, MEMSET_DEFAULT_VALUE, sizeof(cardIDLen));
			const byte *cardID = card.GetCardID(cardIDLen);

			// Test #3.
			bool cardIDCorrect = false;
			if (NULL == cardID && NULL == TEST_DATA[testIndex].m_cardIDExpected)
			{
				cardIDCorrect = true;
			}
			else if (NULL != cardID && NULL != TEST_DATA[testIndex].m_cardIDExpected)
			{
				cardIDCorrect = true;
			}
			else
			{
				cardIDCorrect = false;
			}

			// Test #4.
			int memcmpResult = TEST_DATA[testIndex].m_memcmpExpected; // memcmp() result doesn't matter if cardIDLen is wrong.
			if (cardIDLen == TEST_DATA[testIndex].m_cardIDLenExpected)
			{
				memcmpResult = memcmp(cardID, TEST_DATA[testIndex].m_cardIDExpected, TEST_DATA[testIndex].m_cardIDLenExpected);
			}

			// Test #5.
			bool checkCommIDExpected = true;
			bool checkCommIDResult = true;
			byte checkCommIDCommID = 0; // Not used unless checkCommIDResult = false.
			// Was a valid global card expected?
			if (GLOBAL_GLOBAL_CARD_LEN == TEST_DATA[testIndex].m_cardIDLenExpected &&
				MEMCMP_EQUAL == memcmp(TEST_DATA[testIndex].m_cardIDExpected, GLOBAL_GLOBAL_CARD_NUM, GLOBAL_GLOBAL_CARD_LEN))
			{
				// For global setup cards, CheckCommID needs to return true for all commID values.
				checkCommIDExpected = true;
				// This for loop has maximum value same as maximum for byte type.
				for (byte commID = MINIMUM_CONFIG_COMM_ID; ; ++commID)
				{
					if (!card.CheckCommID(commID))
					{
						checkCommIDCommID = commID;
						checkCommIDResult = false;
						break;
					}

					if (commID == MAXIMUM_CONFIG_COMM_ID)
					{
						break;
					}
				}
			}
			else
			{
				// For cards other than global setup cards, CheckCommID needs to return true only for the correct commID value.
				// This for loop has its maximum value same as the maximum for the byte type.
				for (byte commID = MINIMUM_CONFIG_COMM_ID; ; ++commID)
				{
					checkCommIDCommID = commID;
					checkCommIDExpected = commID == TEST_DATA[testIndex].m_checkCommIDCommIDTrue;
					checkCommIDResult = card.CheckCommID(commID);

					if (checkCommIDResult != checkCommIDExpected)
					{
						break;
					}

					if (commID == MAXIMUM_CONFIG_COMM_ID)
					{
						break;
					}
				}
			}
			// Report #1.
			ASSERT_EQ(isValidReadResult, TEST_DATA[testIndex].m_isValidReadExpected);
			// Report #2.
			ASSERT_EQ(cardIDLen, TEST_DATA[testIndex].m_cardIDLenExpected);
			// Report #3.
			ASSERT_TRUE(cardIDCorrect);
			// Report #4.
			ASSERT_EQ(memcmpResult, TEST_DATA[testIndex].m_memcmpExpected);
			// Report #5.
			ASSERT_EQ(checkCommIDResult, checkCommIDExpected);

			// Cleanup.
		}
	}
}
