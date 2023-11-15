#include "stdafx.h"
#include "Card.h"

#include "gtest/gtest.h"

#include "Hardware/MCRWrapper.h"
#include "Utilities.h"


static const time_t CTIME_DEFAULT_VALUE(0); // Arbitrary value when any valid value is okay.

class CardUnitTests_Rccl : public ::testing::Test
{
protected:
	virtual void SetUp()
	{
	}

	virtual void TearDown()
	{
	}

};

#if 000//TODO-PORT
/// Functional and code coverage test for:
///     CCard::SetCardInfoFromRawRcclData
/// Test with a new CCard instance with CCard::m_cardID == NULL.
/// </summary>
TEST_F(CardUnitTests_Rccl, SetCardInfoFromRawRcclDataTest)
{
	typedef struct
	{
		// Inputs.
		const byte* const m_rawData;
		const int m_rawDataLen;
		const byte m_rcclCardType;
		const byte m_siteNumber;
		const byte m_commID;

		// Expected results.
		const bool m_isValidReadExpected;
		const int m_cardIDLenExpected;
		const byte* const m_cardIDExpected;
		const bool m_checkCommIDExpected;
	} TestDataType;

	CCardConfig config;
	config.SetRcclCardsEnabled(true);
	const int rawDataLen0 = 0;
	const byte* const rawData0 = NULL;
	const int cardIDLenExpected0 = 0;
	const byte* const cardIDExpected0 = NULL;

	const int rawDataLen1 = 19;
	const byte* const rawData1 = NULL;
	const int cardIDLenExpected1 = 0;
	const byte* const cardIDExpected1 = NULL;

	const int rawDataLen2 = 0;
	const byte rawData2[rawDataLen2 + 1] = { 0 };
	const int cardIDLenExpected2 = 0;
	const byte* const cardIDExpected2 = NULL;

	const int rawDataLen3 = 1;
	const byte rawData3[rawDataLen3] = { 0xff };
	const int cardIDLenExpected3 = 0;
	const byte* const cardIDExpected3 = NULL;

	const int rawDataLen4 = 2;
	const byte rawData4[rawDataLen4] = { 0xff, 0xfe };
	const int cardIDLenExpected4 = 0;
	const byte* const cardIDExpected4 = NULL;

	const int rawDataLen5 = 2;
	const byte rawData5[rawDataLen5] = { ';', 0xfe };
	const int cardIDLenExpected5 = 0;
	const byte* const cardIDExpected5 = NULL;

	const int rawDataLen6 = 18;
	const byte rawData6[rawDataLen6 + 1] = { ";abcdefghij1234567" };
	const int cardIDLenExpected6 = 0;
	const byte* const cardIDExpected6 = NULL;

	const int rawDataLen7 = 19;
	const byte rawData7[rawDataLen7 + 1] = { ";abcdefgh12345678=?" };
	const int cardIDLenExpected7 = 5;
	const byte cardIDExpected7[cardIDLenExpected7] = { 0xaa, 0x12, 0x34, 0x56, 0x78 };

	const int rawDataLen8 = 19;
	const byte rawData8[rawDataLen8 + 1] = { ";abcdefgh12345678=?" };
	const int cardIDLenExpected8 = 5;
	const byte cardIDExpected8[cardIDLenExpected8] = { 0xaa, 0x12, 0x34, 0x56, 0x78 };

	const int rawDataLen9 = 19;
	const byte rawData9[rawDataLen9 + 1] = { "%abcdefgh12345678=?" }; // Track 1 start character.
	const int cardIDLenExpected9 = 0;
	const byte* const cardIDExpected9 = NULL;

	const int rawDataLen10 = 19;
	const byte rawData10[rawDataLen10 + 1] = { ";abcdefgh12345678^?" }; // Track 1 field separator.
	const int cardIDLenExpected10 = 0;
	const byte* const cardIDExpected10 = NULL;

	const int rawDataLen11 = 19;
	const byte rawData11[rawDataLen11 + 1] = { ";abcdefgh12345678==" }; // Invalid end sentinel.
	const int cardIDLenExpected11 = 0;
	const byte* const cardIDExpected11 = NULL;

	const int rawDataLen12 = 19;
	const byte rawData12[rawDataLen12 + 1] = { ";abcdefgh12:;<=>?=?" }; // Valid track 2 characters in numeric fields.
	const int cardIDLenExpected12 = 5;
	const byte cardIDExpected12[cardIDLenExpected12] = { 0x01, 0x12, 0xb1, 0xd3, 0xf5 };

	const int rawDataLen13 = 19;
	const byte rawData13[rawDataLen13 + 1] = { ";abcdefghXYZ[\\]^_=?" }; // Valid track 1 characters in numeric fields.
	const int cardIDLenExpected13 = 5;
	const byte cardIDExpected13[cardIDLenExpected13] = { 0xff, 0x25, 0x47, 0x69, 0x91 };

	const int rawDataLen14 = 19;
	const byte rawData14[rawDataLen14 + 1] = { ";abcdefghstuvwxyz=?" }; // Non-track 1&2 characters in numeric fields.
	const int cardIDLenExpected14 = 5;
	const byte cardIDExpected14[cardIDLenExpected14] = { 0xaa, 0x66, 0x88, 0x14, 0x36 };

	const int rawDataLen15 = 19;
	const byte rawData15[rawDataLen15 + 1] = { ";abcdefgh12345678=?" };
	const int cardIDLenExpected15 = 0;
	const byte* const cardIDExpected15 = NULL;

	const int rawDataLen16 = 20;
	const byte rawData16[rawDataLen16 + 1] = { ";abcdefghij12345678=" }; // Enough data for old RCCL cards, but no end sentinel.
	const int cardIDLenExpected16 = 0;
	const byte* const cardIDExpected16 = NULL;

	const int rawDataLen17 = 21;
	const byte rawData17[rawDataLen17 + 1] = { ";abcdefghij12345678^?" }; // Minimum length for old RCCL cards, but track 1 field separator.
	const int cardIDLenExpected17 = 0;
	const byte* const cardIDExpected17 = NULL;

	const int rawDataLen18 = 21;
	const byte rawData18[rawDataLen18 + 1] = { ";abcdefghij12345678==" }; // Minimum length for old RCCL cards, but invalid end sentinel.
	const int cardIDLenExpected18 = 0;
	const byte* const cardIDExpected18 = NULL;

	const int rawDataLen19 = 21;
	const byte rawData19[rawDataLen19 + 1] = { ";abcdefghij12345678=?" }; // Minimum length for old RCCL cards.
	const int cardIDLenExpected19 = 5;
	const byte cardIDExpected19[cardIDLenExpected19] = { 0xaa, 0x12, 0x34, 0x56, 0x78 };

	const int rawDataLen20 = 21;
	const byte rawData20[rawDataLen20 + 1] = { "%abcdefghij12345678=?" }; // Track 1 start character.
	const int cardIDLenExpected20 = 0;
	const byte* const cardIDExpected20 = NULL;

	const int rawDataLen21 = 21;
	const byte rawData21[rawDataLen21 + 1] = { ";abcdefghij12345678^?" }; // Track 1 field separator.
	const int cardIDLenExpected21 = 0;
	const byte* const cardIDExpected21 = NULL;

	const int rawDataLen22 = 21;
	const byte rawData22[rawDataLen22 + 1] = { ";abcdefghij12345678==" }; // Invalid end sentinel.
	const int cardIDLenExpected22 = 0;
	const byte* const cardIDExpected22 = NULL;

	const int rawDataLen23 = 21;
	const byte rawData23[rawDataLen23 + 1] = { ";abcdefghij12:;<=>?=?" }; // Valid track 2 characters in numeric fields.
	const int cardIDLenExpected23 = 5;
	const byte cardIDExpected23[cardIDLenExpected23] = { 0x01, 0x12, 0xb1, 0xd3, 0xf5 };

	const int rawDataLen24 = 21;
	const byte rawData24[rawDataLen24 + 1] = { ";abcdefghijXYZ[\\]^_=?" }; // Valid track 1 characters in numeric fields.
	const int cardIDLenExpected24 = 5;
	const byte cardIDExpected24[cardIDLenExpected24] = { 0xff, 0x25, 0x47, 0x69, 0x91 };

	const int rawDataLen25 = 21;
	const byte rawData25[rawDataLen25 + 1] = { ";abcdefghijstuvwxyz=?" }; // Non-track 1&2 characters in numeric fields.
	const int cardIDLenExpected25 = 5;
	const byte cardIDExpected25[cardIDLenExpected25] = { 0xaa, 0x66, 0x88, 0x14, 0x36 };

	const int rawDataLen26 = 21;
	const byte rawData26[rawDataLen26 + 1] = { ";abcdefghij12345678=?" };
	const int cardIDLenExpected26 = 0;
	const byte* const cardIDExpected26 = NULL;

	const int rawDataLen27 = 22;
	const byte rawData27[rawDataLen27 + 1] = { ";abcdefghijstuvwxyz=?X" }; // Length for old RCCL cards, plus 1.
	const int cardIDLenExpected27 = 5;
	const byte cardIDExpected27[cardIDLenExpected27] = { 0xaa, 0x66, 0x88, 0x14, 0x36 };

	const int rawDataLen28 = 23;
	const byte rawData28[rawDataLen28 + 1] = { ";abcdefghijstuvwxyz=?XY" }; // Length for old RCCL cards, plus 2.
	const int cardIDLenExpected28 = 5;
	const byte cardIDExpected28[cardIDLenExpected28] = { 0xaa, 0x66, 0x88, 0x14, 0x36 };

	const int rawDataLen29 = 49;
	const byte rawData29[rawDataLen29 + 1] = { ";abcdefghijstuvwxyz=?Geoffery the Super Player!!!" }; // Length for old RCCL cards, plus 28.
	const int cardIDLenExpected29 = 5;
	const byte cardIDExpected29[cardIDLenExpected29] = { 0xaa, 0x66, 0x88, 0x14, 0x36 };

	const int rawDataLen30 = 176;
	const byte rawData30[rawDataLen30 + 1] = { 04, 0x5f, 0xa1, 0x72, 0x12, 0x83, 0x32, 0x80, 0x23, 0x48, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00 ,00, 00, 00,
		00, 00, 00, 00, 00, 0xe4, 0x1d, 0xe5, 0x60, 0x56, 0xf6, 0x22, 00, 0xec, 0x51, 0x33, 0x01, 0x32, 0x36, 0x38, 0x35, 0x39, 0x33, 0x38, 0x38, 0x5e, 0x5e, 0x4f, 0x41, 0x5e, 0x43, 0x33, 0x5e, 0x32, 0x30, 0x31, 0x34, 0x30, 0x35, 0x30, 0x33, 0x5e, 0x53, 0x41, 0x4e, 0x54, 0x49, 0x41, 0x47, 0x4f, 0x5e, 0x41, 0x42, 0x52, 0x41, 0x48, 0x41, 0x4d, 0x5e,
		00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 0x30, 00, 00, 00, 00, 00, 00, 00, 00 }; // Length for old RCCL cards, plus 2.
	const int cardIDLenExpected30 = 5;
	const byte cardIDExpected30[cardIDLenExpected30] = { 0xa5, 0x61, 0x41, 0x07, 0x88 };

	const int rawDataLen31 = 176;
	const byte rawData31[rawDataLen31 + 1] = { 04, 0x5f, 0xa1, 0x72, 0x12, 0x83, 0x32, 0x80, 0x23, 0x48, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
		00, 00, 00, 00, 00, 0, 0, 0, 0, 0, 0, 0, 00, 0xec, 0x51, 0x33, 0x01, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x4f, 0x41, 0x5e, 0x43, 0x33, 0x5e, 0x32, 0x30, 0x31, 0x34, 0x30, 0x35, 0x30, 0x33, 0x5e, 0x53, 0x41, 0x4e, 0x54, 0x49, 0x41, 0x47, 0x4f, 0x5e, 0x41, 0x42, 0x52, 0x41, 0x48, 0x41, 0x4d, 0x5e,
		00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 0x30, 00, 00, 00, 00, 00, 00, 00, 00 }; // Length for old RCCL cards, plus 2.
	const int cardIDLenExpected31 = 0;
	const byte* const cardIDExpected31 = NULL;
	const TestDataType TEST_DATA[] =
	{
		// Test non-RCCL cards -- length < 19.
		{ rawData0, rawDataLen0, 0x11, 0x22, 0x33, false, cardIDLenExpected0, cardIDExpected0, false }, // NULL data & zero length.
		{ rawData1, rawDataLen1, 0x11, 0x22, 0x22, false, cardIDLenExpected1, cardIDExpected1, false }, // NULL data & non-zero length.
		{ rawData2, rawDataLen2, 0x44, 0x55, 0x66, false, cardIDLenExpected2, cardIDExpected2, false }, // Non-NULL data & zero length
		{ rawData3, rawDataLen3, 0x77, 0x88, 0, false, cardIDLenExpected3, cardIDExpected3, false }, // Data length of 1, too short to check start char.
		{ rawData4, rawDataLen4, 0x77, 0x88, 0, false, cardIDLenExpected4, cardIDExpected4, false }, // Data length of 2, invalid start char.
		{ rawData5, rawDataLen5, 0x77, 0x88, 0, false, cardIDLenExpected5, cardIDExpected5, false }, // Data length of 2, valid start char.
		{ rawData6, rawDataLen6, 0x77, 0x88, 0, false, cardIDLenExpected6, cardIDExpected6, true }, // Almost enough data.

		// Test RCCL type 2 cards -- length == 19.
		{ rawData7, rawDataLen7, 0xaa, 0xbb, 0xcc, true, cardIDLenExpected7, cardIDExpected7, false }, // Fail CheckCommID().
		{ rawData8, rawDataLen8, 0xaa, 0xbb, 0xbb, true, cardIDLenExpected8, cardIDExpected8, true },
		{ rawData9, rawDataLen9, 0xaa, 0xbb, 0, false, cardIDLenExpected9, cardIDExpected9, true },
		{ rawData10, rawDataLen10, 0xaa, 0xbb, 0, false, cardIDLenExpected10, cardIDExpected10, true },
		{ rawData11, rawDataLen11, 0xaa, 0xbb, 0, false, cardIDLenExpected11, cardIDExpected11, true },
		{ rawData12, rawDataLen12, 0x01, 0x00, 0x00, true, cardIDLenExpected12, cardIDExpected12, true },
		{ rawData13, rawDataLen13, 0xff, 0x01, 0x01, true, cardIDLenExpected13, cardIDExpected13, true },
		{ rawData14, rawDataLen14, 0xaa, 0xff, 0xff, true, cardIDLenExpected14, cardIDExpected14, true },
		{ rawData15, rawDataLen15, 0, 0xbb, 0, false, cardIDLenExpected15, cardIDExpected15, true }, // RCCL card type 0.

		// Test RCCL non-type 2 cards -- length == 20.
		{ rawData16, rawDataLen16, 0xaa, 0xbb, 0, false, cardIDLenExpected16, cardIDExpected16, true },

		// Test RCCL non-type 2 cards -- length == 21.
		{ rawData17, rawDataLen17, 0xaa, 0xbb, 0, false, cardIDLenExpected17, cardIDExpected17, true },
		{ rawData18, rawDataLen18, 0xaa, 0xbb, 0, false, cardIDLenExpected18, cardIDExpected18, true },
		{ rawData19, rawDataLen19, 0xaa, 0xbb, 0xbb, true, cardIDLenExpected19, cardIDExpected19, true },
		{ rawData20, rawDataLen20, 0xaa, 0xbb, 0, false, cardIDLenExpected20, cardIDExpected20, true },
		{ rawData21, rawDataLen21, 0xaa, 0xbb, 0, false, cardIDLenExpected21, cardIDExpected21, true },
		{ rawData22, rawDataLen22, 0xaa, 0xbb, 0, false, cardIDLenExpected22, cardIDExpected22, true },
		{ rawData23, rawDataLen23, 0x01, 0x00, 0x00, true, cardIDLenExpected23, cardIDExpected23, true },
		{ rawData24, rawDataLen24, 0xff, 0x01, 0x01, true, cardIDLenExpected24, cardIDExpected24, true },
		{ rawData25, rawDataLen25, 0xaa, 0xff, 0xff, true, cardIDLenExpected25, cardIDExpected25, true },
		{ rawData26, rawDataLen26, 0, 0xbb, 0, false, cardIDLenExpected26, cardIDExpected26, true }, // RCCL card type 0.

		// Test RCCL non-type 2 cards -- length > 21.
		{ rawData27, rawDataLen27, 0xaa, 0xbb, 0xbb, true, cardIDLenExpected27, cardIDExpected27, true },
		{ rawData28, rawDataLen28, 0xaa, 0xbb, 0xbb, true, cardIDLenExpected28, cardIDExpected29, true },
		{ rawData29, rawDataLen29, 0xaa, 0xbb, 0xbb, true, cardIDLenExpected29, cardIDExpected29, true },

		// Test RCCL RFID player card
		{ rawData30, rawDataLen30, 0xa5, 0x01, 0x01, true, cardIDLenExpected30, cardIDExpected30, true },
		{ rawData31, rawDataLen31, 0xa5, 0x01, 0x01, false, cardIDLenExpected31, cardIDExpected31, false },
	};

	for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
	{
		// Setup.
		CCard varCard(CTIME_DEFAULT_VALUE, CTIME_DEFAULT_VALUE);

		config.SetCommID(TEST_DATA[testIndex].m_siteNumber);
		config.SetPlayerCardPrefixes(&(TEST_DATA[testIndex].m_rcclCardType), 1);

		// Perform operation(s) to be tested.
		varCard.SetCardInfoFromRawData(TEST_DATA[testIndex].m_rawData,
			TEST_DATA[testIndex].m_rawDataLen, config);
		const CCard &card(varCard);

		// Test #1.
		const bool isValidReadResult = card.IsValidRead();
        EXPECT_EQ(TEST_DATA[testIndex].m_isValidReadExpected, isValidReadResult);

		int varCardIDLenResult(0xdddddddd);
		const byte *cardIDResult = card.GetCardID(varCardIDLenResult);
		const int cardIDLenResult(varCardIDLenResult);

		// Test #2.
		const bool equal(cardIDLenResult == TEST_DATA[testIndex].m_cardIDLenExpected);
        EXPECT_EQ(TEST_DATA[testIndex].m_cardIDLenExpected, cardIDLenResult);

		// Test #3.
		{
			bool correct = false;
			if (NULL == cardIDResult && NULL == TEST_DATA[testIndex].m_cardIDExpected)
			{
				correct = true;
			}
			else if (NULL != cardIDResult && NULL != TEST_DATA[testIndex].m_cardIDExpected)
			{
				correct = true;
			}
			else
			{
				correct = false;
			}

            EXPECT_TRUE(correct);
		}

		// Test #4.
		EXPECT_EQ(0, memcmp(cardIDResult, TEST_DATA[testIndex].m_cardIDExpected, TEST_DATA[testIndex].m_cardIDLenExpected));

		// Test #5.
		EXPECT_EQ(TEST_DATA[testIndex].m_checkCommIDExpected, card.CheckCommID(TEST_DATA[testIndex].m_commID));

		// Cleanup.
	}
}

/// Functional and code coverage test for:
///     CCard::SetCardInfoFromRawRcclData
/// Test with a new CCard instance with CCard::m_cardID != NULL to get code coverage on the delete[] for CCard::m_cardID.
/// </summary>
TEST_F(CardUnitTests_Rccl, SetCardInfoFromRawRcclData_DeleteTest)
{
	typedef struct
	{
		// Inputs.
		const byte* const m_rawData;
		const int m_rawDataLen;
		const byte m_rcclCardType;
		const byte m_siteNumber;
		const byte m_commID;

		// Expected results.
		const bool m_isValidReadExpected;
		const int m_cardIDLenExpected;
		const byte* const m_cardIDExpected;
		const bool m_checkCommIDExpected;
	} TestDataType;

	CCardConfig config;
	config.SetRcclCardsEnabled(true);
	const int rawDataLen0 = 0;
	const byte* const rawData0 = NULL;
	const int cardIDLenExpected0 = 0;
	const byte* const cardIDExpected0 = NULL;

	const int rawDataLen1 = 19;
	const byte* const rawData1 = NULL;
	const int cardIDLenExpected1 = 0;
	const byte* const cardIDExpected1 = NULL;

	const int rawDataLen2 = 0;
	const byte rawData2[rawDataLen2 + 1] = { 0 };
	const int cardIDLenExpected2 = 0;
	const byte* const cardIDExpected2 = NULL;

	const int rawDataLen3 = 1;
	const byte rawData3[rawDataLen3] = { 0xff };
	const int cardIDLenExpected3 = 0;
	const byte* const cardIDExpected3 = NULL;

	const int rawDataLen4 = 2;
	const byte rawData4[rawDataLen4] = { 0xff, 0xfe };
	const int cardIDLenExpected4 = 0;
	const byte* const cardIDExpected4 = NULL;

	const int rawDataLen5 = 2;
	const byte rawData5[rawDataLen5] = { ';', 0xfe };
	const int cardIDLenExpected5 = 0;
	const byte* const cardIDExpected5 = NULL;

	const int rawDataLen6 = 18;
	const byte rawData6[rawDataLen6 + 1] = { ";abcdefghij1234567" };
	const int cardIDLenExpected6 = 0;
	const byte* const cardIDExpected6 = NULL;


	const int rawDataLen7 = 19;
	const byte rawData7[rawDataLen7 + 1] = { ";abcdefgh12345678=?" };
	const int cardIDLenExpected7 = 5;
	const byte cardIDExpected7[cardIDLenExpected7] = { 0xaa, 0x12, 0x34, 0x56, 0x78 };

	const int rawDataLen8 = 19;
	const byte rawData8[rawDataLen8 + 1] = { ";abcdefgh12345678=?" };
	const int cardIDLenExpected8 = 5;
	const byte cardIDExpected8[cardIDLenExpected8] = { 0xaa, 0x12, 0x34, 0x56, 0x78 };

	const int rawDataLen9 = 19;
	const byte rawData9[rawDataLen9 + 1] = { "%abcdefgh12345678=?" }; // Track 1 start character.
	const int cardIDLenExpected9 = 0;
	const byte* const cardIDExpected9 = NULL;

	const int rawDataLen10 = 19;
	const byte rawData10[rawDataLen10 + 1] = { ";abcdefgh12345678^?" }; // Track 1 field separator.
	const int cardIDLenExpected10 = 0;
	const byte* const cardIDExpected10 = NULL;

	const int rawDataLen11 = 19;
	const byte rawData11[rawDataLen11 + 1] = { ";abcdefgh12345678==" }; // Invalid end sentinel.
	const int cardIDLenExpected11 = 0;
	const byte* const cardIDExpected11 = NULL;

	const int rawDataLen12 = 19;
	const byte rawData12[rawDataLen12 + 1] = { ";abcdefgh12:;<=>?=?" }; // Valid track 2 characters in numeric fields.
	const int cardIDLenExpected12 = 5;
	const byte cardIDExpected12[cardIDLenExpected12] = { 0x01, 0x12, 0xb1, 0xd3, 0xf5 };

	const int rawDataLen13 = 19;
	const byte rawData13[rawDataLen13 + 1] = { ";abcdefghXYZ[\\]^_=?" }; // Valid track 1 characters in numeric fields.
	const int cardIDLenExpected13 = 5;
	const byte cardIDExpected13[cardIDLenExpected13] = { 0xff, 0x25, 0x47, 0x69, 0x91 };

	const int rawDataLen14 = 19;
	const byte rawData14[rawDataLen14 + 1] = { ";abcdefghstuvwxyz=?" }; // Non-track 1&2 characters in numeric fields.
	const int cardIDLenExpected14 = 5;
	const byte cardIDExpected14[cardIDLenExpected14] = { 0xaa, 0x66, 0x88, 0x14, 0x36 };

	const int rawDataLen15 = 19;
	const byte rawData15[rawDataLen15 + 1] = { ";abcdefgh12345678=?" };
	const int cardIDLenExpected15 = 0;
	const byte* const cardIDExpected15 = NULL;

	const int rawDataLen16 = 20;
	const byte rawData16[rawDataLen16 + 1] = { ";abcdefghij12345678=" }; // Enough data for old RCCL cards, but no end sentinel.
	const int cardIDLenExpected16 = 0;
	const byte* const cardIDExpected16 = NULL;

	const int rawDataLen17 = 21;
	const byte rawData17[rawDataLen17 + 1] = { ";abcdefghij12345678^?" }; // Minimum length for old RCCL cards, but track 1 field separator.
	const int cardIDLenExpected17 = 0;
	const byte* const cardIDExpected17 = NULL;

	const int rawDataLen18 = 21;
	const byte rawData18[rawDataLen18 + 1] = { ";abcdefghij12345678==" }; // Minimum length for old RCCL cards, but invalid end sentinel.
	const int cardIDLenExpected18 = 0;
	const byte* const cardIDExpected18 = NULL;

	const int rawDataLen19 = 21;
	const byte rawData19[rawDataLen19 + 1] = { ";abcdefghij12345678=?" }; // Minimum length for old RCCL cards.
	const int cardIDLenExpected19 = 5;
	const byte cardIDExpected19[cardIDLenExpected19] = { 0xaa, 0x12, 0x34, 0x56, 0x78 };

	const int rawDataLen20 = 21;
	const byte rawData20[rawDataLen20 + 1] = { "%abcdefghij12345678=?" }; // Track 1 start character.
	const int cardIDLenExpected20 = 0;
	const byte* const cardIDExpected20 = NULL;

	const int rawDataLen21 = 21;
	const byte rawData21[rawDataLen21 + 1] = { ";abcdefghij12345678^?" }; // Track 1 field separator.
	const int cardIDLenExpected21 = 0;
	const byte* const cardIDExpected21 = NULL;

	const int rawDataLen22 = 21;
	const byte rawData22[rawDataLen22 + 1] = { ";abcdefghij12345678==" }; // Invalid end sentinel.
	const int cardIDLenExpected22 = 0;
	const byte* const cardIDExpected22 = NULL;

	const int rawDataLen23 = 21;
	const byte rawData23[rawDataLen23 + 1] = { ";abcdefghij12:;<=>?=?" }; // Valid track 2 characters in numeric fields.
	const int cardIDLenExpected23 = 5;
	const byte cardIDExpected23[cardIDLenExpected23] = { 0x01, 0x12, 0xb1, 0xd3, 0xf5 };

	const int rawDataLen24 = 21;
	const byte rawData24[rawDataLen24 + 1] = { ";abcdefghijXYZ[\\]^_=?" }; // Valid track 1 characters in numeric fields.
	const int cardIDLenExpected24 = 5;
	const byte cardIDExpected24[cardIDLenExpected24] = { 0xff, 0x25, 0x47, 0x69, 0x91 };

	const int rawDataLen25 = 21;
	const byte rawData25[rawDataLen25 + 1] = { ";abcdefghijstuvwxyz=?" }; // Non-track 1&2 characters in numeric fields.
	const int cardIDLenExpected25 = 5;
	const byte cardIDExpected25[cardIDLenExpected25] = { 0xaa, 0x66, 0x88, 0x14, 0x36 };

	const int rawDataLen26 = 21;
	const byte rawData26[rawDataLen26 + 1] = { ";abcdefghij12345678=?" };
	const int cardIDLenExpected26 = 0;
	const byte* const cardIDExpected26 = NULL;

	const int rawDataLen27 = 22;
	const byte rawData27[rawDataLen27 + 1] = { ";abcdefghijstuvwxyz=?X" }; // Length for old RCCL cards, plus 1.
	const int cardIDLenExpected27 = 5;
	const byte cardIDExpected27[cardIDLenExpected27] = { 0xaa, 0x66, 0x88, 0x14, 0x36 };

	const int rawDataLen28 = 23;
	const byte rawData28[rawDataLen28 + 1] = { ";abcdefghijstuvwxyz=?XY" }; // Length for old RCCL cards, plus 2.
	const int cardIDLenExpected28 = 5;
	const byte cardIDExpected28[cardIDLenExpected28] = { 0xaa, 0x66, 0x88, 0x14, 0x36 };

	const int rawDataLen29 = 49;
	const byte rawData29[rawDataLen29 + 1] = { ";abcdefghijstuvwxyz=?Geoffery the Super Player!!!" }; // Length for old RCCL cards, plus 28.
	const int cardIDLenExpected29 = 5;
	const byte cardIDExpected29[cardIDLenExpected29] = { 0xaa, 0x66, 0x88, 0x14, 0x36 };

	const int rawDataLen30 = 176;
	const byte rawData30[rawDataLen30 + 1] = { 04, 0x5f, 0xa1, 0x72, 0x12, 0x83, 0x32, 0x80, 0x23, 0x48, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
		00, 00, 00, 00, 00, 0xe4, 0x1d, 0xe5, 0x60, 0x56, 0xf6, 0x22, 00, 0xec, 0x51, 0x33, 0x01, 0x32, 0x36, 0x38, 0x35, 0x39, 0x33, 0x38, 0x38, 0x5e, 0x5e, 0x4f, 0x41, 0x5e, 0x43, 0x33, 0x5e, 0x32, 0x30, 0x31, 0x34, 0x30, 0x35, 0x30, 0x33, 0x5e, 0x53, 0x41, 0x4e, 0x54, 0x49, 0x41, 0x47, 0x4f, 0x5e, 0x41, 0x42, 0x52, 0x41, 0x48, 0x41, 0x4d, 0x5e,
		00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 0x30, 00, 00, 00, 00, 00, 00, 00, 00 }; // Length for old RCCL cards, plus 2.
	const int cardIDLenExpected30 = 5;
	const byte cardIDExpected30[cardIDLenExpected30] = { 0xa5, 0x61, 0x41, 0x07, 0x88 };

	const TestDataType TEST_DATA[] =
	{
		{ rawData0, rawDataLen0, 0x11, 0x22, 0x33, false, cardIDLenExpected0, cardIDExpected0, false }, // NULL data & zero length.
		{ rawData1, rawDataLen1, 0x11, 0x22, 0x22, false, cardIDLenExpected1, cardIDExpected1, false }, // NULL data & non-zero length.
		{ rawData2, rawDataLen2, 0x44, 0x55, 0x66, false, cardIDLenExpected2, cardIDExpected2, false }, // Non-NULL data & zero length
		{ rawData3, rawDataLen3, 0x77, 0x88, 0, false, cardIDLenExpected3, cardIDExpected3, true }, // Data length of 1, too short to check start char.
		{ rawData4, rawDataLen4, 0x77, 0x88, 0, false, cardIDLenExpected4, cardIDExpected4, true }, // Data length of 2, invalid start char.
		{ rawData5, rawDataLen5, 0x77, 0x88, 0, false, cardIDLenExpected5, cardIDExpected5, true }, // Data length of 2, valid start char.
		{ rawData6, rawDataLen6, 0x77, 0x88, 0, false, cardIDLenExpected6, cardIDExpected6, true }, // Almost enough data.

		// Test RCCL type 2 cards -- length == 19.
		{ rawData7, rawDataLen7, 0xaa, 0xbb, 0xcc, true, cardIDLenExpected7, cardIDExpected7, false }, // Fail CheckCommID().
		{ rawData8, rawDataLen8, 0xaa, 0xbb, 0xbb, true, cardIDLenExpected8, cardIDExpected8, true },
		{ rawData9, rawDataLen9, 0xaa, 0xcc, 0xbb, false, cardIDLenExpected9, cardIDExpected9, true },
		{ rawData10, rawDataLen10, 0xaa, 0xcc, 0xbb, false, cardIDLenExpected10, cardIDExpected10, true },
		{ rawData11, rawDataLen11, 0xaa, 0xcc, 0xbb, false, cardIDLenExpected11, cardIDExpected11, true },
		{ rawData12, rawDataLen12, 0x01, 0x00, 0x00, true, cardIDLenExpected12, cardIDExpected12, true },
		{ rawData13, rawDataLen13, 0xff, 0x01, 0x01, true, cardIDLenExpected13, cardIDExpected13, true },
		{ rawData14, rawDataLen14, 0xaa, 0xff, 0xff, true, cardIDLenExpected14, cardIDExpected14, true },
		{ rawData15, rawDataLen15, 0, 0xbb, 0xff, false, cardIDLenExpected15, cardIDExpected15, true }, // RCCL card type 0.

		// Test RCCL non-type 2 cards -- length == 20.
		{ rawData16, rawDataLen16, 0xaa, 0xbb, 0xff, false, cardIDLenExpected16, cardIDExpected16, true },

		// Test RCCL non-type 2 cards -- length == 21.
		{ rawData17, rawDataLen17, 0xaa, 0xbb, 0xff, false, cardIDLenExpected17, cardIDExpected17, true },
		{ rawData18, rawDataLen18, 0xaa, 0xbb, 0xff, false, cardIDLenExpected18, cardIDExpected18, true },
		{ rawData19, rawDataLen19, 0xaa, 0xbb, 0xbb, true, cardIDLenExpected19, cardIDExpected19, true },
		{ rawData20, rawDataLen20, 0xaa, 0xcc, 0xbb, false, cardIDLenExpected20, cardIDExpected20, true },
		{ rawData21, rawDataLen21, 0xaa, 0xcc, 0xbb, false, cardIDLenExpected21, cardIDExpected21, true },
		{ rawData22, rawDataLen22, 0xaa, 0xcc, 0xbb, false, cardIDLenExpected22, cardIDExpected22, true },
		{ rawData23, rawDataLen23, 0x01, 0x00, 0x00, true, cardIDLenExpected23, cardIDExpected23, true },
		{ rawData24, rawDataLen24, 0xff, 0x01, 0x01, true, cardIDLenExpected24, cardIDExpected24, true },
		{ rawData25, rawDataLen25, 0xaa, 0xff, 0xff, true, cardIDLenExpected25, cardIDExpected25, true },
		{ rawData26, rawDataLen26, 0, 0xbb, 0xff, false, cardIDLenExpected26, cardIDExpected26, true }, // RCCL card type 0.

		// Test RCCL non-type 2 cards -- length > 21.
		{ rawData27, rawDataLen27, 0xaa, 0xbb, 0xbb, true, cardIDLenExpected27, cardIDExpected27, true },
		{ rawData28, rawDataLen28, 0xaa, 0xbc, 0xbc, true, cardIDLenExpected28, cardIDExpected29, true },
		{ rawData29, rawDataLen29, 0xaa, 0xbd, 0xbd, true, cardIDLenExpected29, cardIDExpected29, true },

		// Test RCCL RFID player card
		{ rawData30, rawDataLen30, 0xa5, 0x01, 0x01, true, cardIDLenExpected30, cardIDExpected30, true },
	};


	for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
	{
		// Setup.
		CCard varCard(CTIME_DEFAULT_VALUE, CTIME_DEFAULT_VALUE);

		config.SetCommID(TEST_DATA[testIndex].m_siteNumber);
		config.SetPlayerCardPrefixes(&(TEST_DATA[testIndex].m_rcclCardType), 1);

		// Perform operation(s) to be tested.
		varCard.SetCardInfoFromRawData(TEST_DATA[testIndex].m_rawData,
			TEST_DATA[testIndex].m_rawDataLen, config);
		const CCard &card(varCard);

		// Test #1.
		    EXPECT_EQ(TEST_DATA[testIndex].m_isValidReadExpected, card.IsValidRead());

		int varCardIDLenResult(0xdddddddd);
		const byte *cardIDResult = card.GetCardID(varCardIDLenResult);
		const int cardIDLenResult(varCardIDLenResult);

		// Test #2.
			EXPECT_EQ(TEST_DATA[testIndex].m_cardIDLenExpected, cardIDLenResult);

		// Test #3.
		{
			bool correct = false;
			if (NULL == cardIDResult && NULL == TEST_DATA[testIndex].m_cardIDExpected)
			{
				correct = true;
			}
			else if (NULL != cardIDResult && NULL != TEST_DATA[testIndex].m_cardIDExpected)
			{
				correct = true;
			}
			else
			{
				correct = false;
			}

            EXPECT_TRUE(correct);
		}

		// Test #4.
			EXPECT_EQ(0, memcmp(cardIDResult, TEST_DATA[testIndex].m_cardIDExpected, TEST_DATA[testIndex].m_cardIDLenExpected));

		// Test #5.
		{
			if (card.IsValidRead())
			{
                EXPECT_EQ(TEST_DATA[testIndex].m_checkCommIDExpected, card.CheckCommID(TEST_DATA[testIndex].m_commID));
			}
		}

		// Cleanup.
	}

}
#endif//TODO-PORT
