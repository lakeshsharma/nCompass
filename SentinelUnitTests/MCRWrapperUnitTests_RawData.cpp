
#include "stdafx.h"
#include "Hardware/MCRWrapper.h"
#include "DirectoryManager.h"
#include "TestUtil.h"


#include "gtest/gtest.h"


namespace SentinelNativeUnitTests
{
	typedef struct
	{
		// Inputs.
		const byte* const m_rawData;
		const int m_rawDataLen;
		const byte m_rcclCardType;
		const byte m_siteNumber; // CMCRWrapper:m_siteNumber setup via CConfig::SetCommID().
		const byte m_commID; // Input to CCard::CheckCommID() test.
	
		// Expected results.
		const bool m_isValidReadExpected;
		const int m_cardIDLenExpected;
		const byte* const m_cardIDExpected;
		const bool m_checkCommIDExpected;
	} TestDataType;
	

	class MCRWrapperUnitTests_RawData : public testing::Test
	{
	public:
 
	  virtual void SetUp()
	  {
	  }
	 
	  virtual void TearDown()
	  {
	  }
	  void ConfigMCRWrapperTest(CMCRWrapper& mcrWrapper, const CConfig &config){
	       mcrWrapper.ProcessWaitForEvent(&config); // Configure mcrWrapper for test(s).
	  }

	  
	  CCard* GetCardFromRawData(CMCRWrapper& mcrWrapper, const TestDataType TEST_DATA[], int testIndex){
		  return mcrWrapper.GetCardFromRawData(TEST_DATA[testIndex].m_rawData, TEST_DATA[testIndex].m_rawDataLen);
	  }
					
	};

	// Use MCRWrapperUnitTests for the test class for friend access to CMCRWrapper.
		TEST_F(MCRWrapperUnitTests_RawData, MCRWrapperUnitTests_RawData_Cleanup)
		{
			CDirectoryManager::MockInstanceCleanup();
		}

		/// <summary>
		/// Functional and code coverage test for RCCL portion of:
		///     CMCRWrapper::GetCardFromRawData()
		/// </summary>
		TEST_F(MCRWrapperUnitTests_RawData, GetCardFromRawData_RcclTest)
		{
			CDirectoryManager::MockInstance();

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
			};

			// More input(s).
			const BYTE cardReaderTrackNumberMinimum(1);
			const BYTE cardReaderTrackNumberMaximum(2);

			// Setup.
			CConfig varConfig(false, "");

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				for (BYTE cardReaderTrackNumber = cardReaderTrackNumberMinimum;
					cardReaderTrackNumber <= cardReaderTrackNumberMaximum;
					++cardReaderTrackNumber)
				{
					// Setup.
					varConfig.SetNclCardsEnabled(false);
					varConfig.SetRcclCardsEnabled(true);
					varConfig.SetUniverseEnabled(false);
					varConfig.SetCardReaderTrackNumber(cardReaderTrackNumber);
					varConfig.SetCommID(TEST_DATA[testIndex].m_siteNumber);
                    vector<BYTE> prefixes;
                    prefixes.push_back(TEST_DATA[testIndex].m_rcclCardType);
					varConfig.SetPlayerCardPrefixes(prefixes);
					const CConfig &config(varConfig);
					CMCRWrapper mcrWrapper(varConfig);;
					ConfigMCRWrapperTest(mcrWrapper, config);

					// Perform operation(s) to be tested.
					const CCard* const cardPtr = GetCardFromRawData(mcrWrapper, TEST_DATA, testIndex);
					
					// Test #0.
					{
						bool correct = cardPtr != NULL;
						if (!correct)
						{
							string failedMsg;
							failedMsg = FormatString("cardPtr=%p expected=NotNull testIndex=%d/%d in %s(%d)",
								(void *)cardPtr,
								testIndex, cardReaderTrackNumber,
								(__TFUNCTION__), __LINE__);
							ASSERT_TRUE(false) <<  failedMsg;
						}
					}

					const CCard &card(*cardPtr);

					// Test #1.
					{
						const bool isValidReadResult = card.IsValidRead();
						const bool equal(isValidReadResult == TEST_DATA[testIndex].m_isValidReadExpected);
						if (!equal)
						{
							string failedMsg;
							failedMsg = FormatString("isValidReadResult=%d expected=%d testIndex=%d/%d in %s(%d)",
								isValidReadResult,
								TEST_DATA[testIndex].m_isValidReadExpected,
								testIndex, cardReaderTrackNumber,
								(__TFUNCTION__), __LINE__);
							ASSERT_TRUE(false) <<  failedMsg;
						}
					}

					int varCardIDLenResult(0xdddddddd);
					const byte *cardIDResult = card.GetCardID(varCardIDLenResult);
					const int cardIDLenResult(varCardIDLenResult);

					// Test #2.
					{
						const bool equal(cardIDLenResult == TEST_DATA[testIndex].m_cardIDLenExpected);
						if (!equal)
						{
							string failedMsg;
							failedMsg = FormatString("cardIDLenResult=%d expected=%d testIndex=%d/%d in %s(%d)",
								cardIDLenResult,
								TEST_DATA[testIndex].m_cardIDLenExpected,
								testIndex, cardReaderTrackNumber,
								(__TFUNCTION__), __LINE__);
							ASSERT_TRUE(false) <<  failedMsg;
						}
					}

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

						if (!correct)
						{
							string failedMsg;
							failedMsg = FormatString("cardIDResult=%d expected=%s testIndex=%d/%d in %s(%d)",
								cardIDLenResult,
								(NULL == TEST_DATA[testIndex].m_cardIDExpected ? "NULL" : "NotNull"),
								testIndex, cardReaderTrackNumber,
								__FUNCTION__, __LINE__);
							ASSERT_TRUE(false) <<  failedMsg;
						}
					}

					// Test #4.
					{
						const bool equal(0 == memcmp(cardIDResult, TEST_DATA[testIndex].m_cardIDExpected, TEST_DATA[testIndex].m_cardIDLenExpected));
						if (!equal)
						{
							string cardIDResultStr = CUtilities::ToHexString(cardIDResult, cardIDLenResult);
							string cardIDExpectedStr = CUtilities::ToHexString(TEST_DATA[testIndex].m_cardIDExpected, TEST_DATA[testIndex].m_cardIDLenExpected);
							string failedMsg;
							failedMsg = FormatString("cardIDResult=\"%s\" expected=\"%s\" testIndex=%d/%d in %s(%d)",
								(cardIDResultStr.c_str()),
								(cardIDExpectedStr.c_str()),
								testIndex, cardReaderTrackNumber,
								__FUNCTION__, __LINE__);
							ASSERT_TRUE(false) <<  failedMsg;
						}
					}

					// Test #5.
					{
						const bool checkCommIDResult = card.CheckCommID(TEST_DATA[testIndex].m_commID);
						const bool equal(checkCommIDResult == TEST_DATA[testIndex].m_checkCommIDExpected);
						if (!equal)
						{
							string failedMsg;
							failedMsg = FormatString("card.CheckCommID(%d)=%d expected=%d testIndex=%d/%d in %s(%d)",
								TEST_DATA[testIndex].m_commID,
								checkCommIDResult,
								TEST_DATA[testIndex].m_checkCommIDExpected,
								testIndex, cardReaderTrackNumber,
								(__TFUNCTION__), __LINE__);
							ASSERT_TRUE(false) <<  failedMsg;
						}
					}

					// Cleanup.
				}
			}
		}
}

