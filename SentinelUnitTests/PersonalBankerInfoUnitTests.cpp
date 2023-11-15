

#include "stdafx.h"

#include "PersonalBankerInfo.h"
#include "gtest/gtest.h"


    class PersonalBankerInfoUnitTests : public ::testing::Test
	{
    public:
			/// <summary>
		/// Initialize class member(s) before each test method is run.
		/// </summary>
		virtual void SetUp()
		{
		}

		/// <summary>
		/// Cleanup class member(s) before each test method is run.
		/// </summary>
		virtual void TearDown()
		{

		}
        void DisplayTransferCashToGameTest();
        void DisplayTransferCashToGame_BannedFromRedemptionsTest();
        void DisplayTransferCashToGame_TransferEnabledTest();
        void DisplayTransferCashToGame_MinTest();
        void DisplayTransferCashToGame_MaxTest();
        void DisplayTransferCashToGame_MinMaxTest();
        void DisplayTransferCashToGame_MinBalanceTest();
        void DisplayTransferCashToGame_MaxBalanceTest();
        void DisplayTransferCashToGame_BalanceTest();
        void DisplayTransferPointsToGameTest();
        void DisplayTransferPointsToGame_BannedFromRedemptionsTest();
        void DisplayTransferPointsToGame_TransferEnabledTest();
        void DisplayTransferPointsToGame_MinTest();
        void DisplayTransferPointsToGame_MaxTest();
        void DisplayTransferPointsToGame_MinMaxTest();
        void DisplayTransferPointsToGame_MinBalanceTest();
        void DisplayTransferPointsToGame_MinBalancePointsPerDollarTest();
        void DisplayTransferPointsToGame_MaxBalanceTest();
        void DisplayTransferPointsToGame_MaxBalancePointsPerDollarTest();
        void DisplayTransferPointsToGame_BalanceTest();
        void DisplayTransferPromoToGameTest();
        void DisplayTransferPromoToGame_BannedFromRedemptionsTest();
        void DisplayTransferPromoToGame_TransferEnabledTest();
        void DisplayTransferPromoToGame_MinTest();
        void DisplayTransferPromoToGame_MaxTest();
        void DisplayTransferPromoToGame_MinMaxTest();
        void DisplayTransferPromoToGame_MinBalanceTest();
        void DisplayTransferPromoToGame_MaxBalanceTest();
        void DisplayTransferPromoToGame_BalanceTest();
        void GetTransferToGameScreenWithAllEnabledTest();
        void GetTransferToGameScreenWithCashPointsEnabledTest();
        void GetTransferToGameScreenWithCashPromoEnabledTest();
        void GetTransferToGameScreenWithPointsPromoEnabledTest();
        void GetTransferToGameScreenWithCashEnabledTest();
        void GetTransferToGameScreenWithPointsEnabledTest();
        void GetTransferToGameScreenWithPromoEnabledTest();
        void GetTransferToGameScreenWithNoneEnabledTest();
        void DisplayTransferCashFromGameTest();
        void DisplayTransferCashFromGameTest_Max_Cash_Not_Enough();
        void DisplayTransferCashFromGameTest_Transfer_Cash_From_Game_Not_Enabled();
        void DisplayTransferCashFromGameTest_Min_Cash_Not_Enough();
        void DisplayTransferCashFromGameTest_No_Cash_Balance_On_Game();
        void DisplayTransferPromoFromGameTest();
        void DisplayTransferCashFromGameTest_Max_Promo_To_Much();
        void DisplayTransferCashFromGameTest_Transfer_Promo_From_Game_Not_Enabled();
        void DisplayTransferCashFromGameTest_Min_Promo_Not_Enough();
        void DisplayTransferCashFromGameTest_No_Promo_Balance_On_Game();
        void SetShowToGameButtonFlag_WithAllEnabledTest();
        void SetShowToGameButtonFlag_WithCashPointsEnabledTest();
        void SetShowToGameButtonFlag_WithCashPromoEnabledTest();
        void SetShowToGameButtonFlag_WithPointsPromoEnabledTest();
        void SetShowToGameButtonFlag_WithCashEnabledTest();
        void SetShowToGameButtonFlag_WithPointsEnabledTest();
        void SetShowToGameButtonFlag_WithPromoEnabledTest();
        void SetShowToGameButtonFlag_WithNoneEnabledTest();
        void GetPersonalBankerMainScreenTest();
        void GetCreditTransferScreenTest();
        void SetShowToGameButtonTest();
        void GetMinimumPointsDollarValueDownload_DefaultTest();
        void GetMinimumPointsDollarValueDownload_1Test();
        void GetMinimumPointsDollarValueDownload_2Test();
        void GetMinimumPointsDollarValueDownload_500Test();
    };
		/// <summary>
		/// Test the implementation of:
		///     CPersonalBankerInfo::SetPBFlags()
		///     CPersonalBankerInfo::GetPBFlags()
		/// </summary>
		/// <remarks>
		/// Full code path coverage test.
		/// Full functional test.
		/// Test data includes input values to test edge conditions.
		/// </remarks>
		TEST_F(PersonalBankerInfoUnitTests, SetPBFlagsTest)
		{
			typedef struct
			{
				// Inputs.
				const DWORD m_pbFlags;

				// Expected results.
				const DWORD m_pbFlagsExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ 0, 0 },
				{ 1, 1 },
				{ 0x7f, 0x7f },
				{ 0x80, 0x80 },
				{ 0xff, 0xff },
				{ 0x100, 0x100 },
				{ 0x7fffffff, 0x7fffffff },
				{ 0x80000000, 0x80000000 },
				{ 0xffffffff, 0xffffffff },
				{ 0x00000001, 0x00000001 },
				{ 0x00000002, 0x00000002 },
				{ 0x00000004, 0x00000004 },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CPersonalBankerInfo varPBInfo;

				// Perform operations to be tested.
				varPBInfo.SetPBFlags(TEST_DATA[testIndex].m_pbFlags);
				const CPersonalBankerInfo &pbInfo(varPBInfo);

				// Test #1.
				const DWORD pbFlagsResult = pbInfo.GetPBFlags();

				{
					const bool areEqual = pbFlagsResult == TEST_DATA[testIndex].m_pbFlagsExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED pbFlagsResult=%8.8x expected=%8.8x testIndex=%d in %s"),
						(unsigned)pbFlagsResult,
						(unsigned)TEST_DATA[testIndex].m_pbFlagsExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBankerInfo::SetPBFlags()
		///     CPersonalBankerInfo::ArePropertiesOffline()
		/// </summary>
		/// <remarks>
		/// Full code path coverage test.
		/// Full functional test.
		/// Test data includes input values to test edge conditions.
		/// </remarks>
		TEST_F(PersonalBankerInfoUnitTests, ArePropertiesOfflineTest)
		{
			typedef struct
			{
				// Inputs.
				const DWORD m_pbFlags;

				// Expected results.
				const bool m_propertiesOfflineExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ 0, false },
				{ 0x00000001, true },
				{ 0xffffffff & ~0x00000001, false },
				{ 0xffffffff & ~0x00000002, true },
				{ 0xffffffff & ~0x00000004, true },
				{ 0xffffffff, true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CPersonalBankerInfo varPBInfo;

				// Perform operations to be tested.
				varPBInfo.SetPBFlags(TEST_DATA[testIndex].m_pbFlags);
				const CPersonalBankerInfo &pbInfo(varPBInfo);

				// Test #1.
				const bool propertiesOfflineResult = pbInfo.ArePropertiesOffline();

				{
					const bool areEqual = propertiesOfflineResult == TEST_DATA[testIndex].m_propertiesOfflineExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED propertiesOfflineResult=%d expected=%d testIndex=%d (%8.8x) in %s"),
						(int)propertiesOfflineResult,
						(int)TEST_DATA[testIndex].m_propertiesOfflineExpected,
						(int)testIndex,
						(int)TEST_DATA[testIndex].m_pbFlags,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBankerInfo::SetPBFlags()
		///     CPersonalBankerInfo::IsBannedFromRedemptions()
		/// </summary>
		/// <remarks>
		/// Full code path coverage test.
		/// Full functional test.
		/// Test data includes input values to test edge conditions.
		/// </remarks>
        TEST_F(PersonalBankerInfoUnitTests,IsBannedFromRedemptionsTest)
		{
			typedef struct
			{
				// Inputs.
				const DWORD m_pbFlags;

				// Expected results.
				const bool m_bannedFromRedemptionsExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ 0, false },
				{ 0x00000002, true },
				{ 0xffffffff & ~0x00000001, true },
				{ 0xffffffff & ~0x00000002, false },
				{ 0xffffffff & ~0x00000004, true },
				{ 0xffffffff, true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CPersonalBankerInfo varPBInfo;

				// Perform operations to be tested.
				varPBInfo.SetPBFlags(TEST_DATA[testIndex].m_pbFlags);
				const CPersonalBankerInfo &pbInfo(varPBInfo);

				// Test #1.
				const bool bannedFromRedemptionsResult = pbInfo.IsBannedFromRedemptions();

				{
					const bool areEqual = bannedFromRedemptionsResult == TEST_DATA[testIndex].m_bannedFromRedemptionsExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED bannedFromRedemptionsResult=%d expected=%d testIndex=%d (%8.8x) in %s"),
						(int)bannedFromRedemptionsResult,
						(int)TEST_DATA[testIndex].m_bannedFromRedemptionsExpected,
						(int)testIndex,
						(int)TEST_DATA[testIndex].m_pbFlags,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBankerInfo::SetPBFlags()
		///     CPersonalBankerInfo::IsUnableToMoveLeftoverCash()
		/// </summary>
		TEST_F(PersonalBankerInfoUnitTests, IsUnableToMoveLeftoverCash)
		{
			typedef struct
			{
				// Inputs.
				const DWORD m_pbFlags;

				// Expected results.
				const bool m_unableToMoveLeftoverCashExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ 0, false },
				{ 0x00000010, true },
				{ 0xffffffff & ~0x00000010, false },
				{ 0xffffffff & ~0x00000020, true },
				{ 0xffffffff, true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CPersonalBankerInfo varPBInfo;

				// Perform operations to be tested.
				varPBInfo.SetPBFlags(TEST_DATA[testIndex].m_pbFlags);
				const CPersonalBankerInfo &pbInfo(varPBInfo);

				// Test #1.
				const bool unableToMoveLeftoverCashResult = pbInfo.IsUnableToMoveLeftoverCash();

				{
					const bool areEqual = unableToMoveLeftoverCashResult == TEST_DATA[testIndex].m_unableToMoveLeftoverCashExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED unableToMoveLeftoverCashResult=%d expected=%d testIndex=%d (%8.8x) in %s"),
						(int)unableToMoveLeftoverCashResult,
						(int)TEST_DATA[testIndex].m_unableToMoveLeftoverCashExpected,
						(int)testIndex,
						(int)TEST_DATA[testIndex].m_pbFlags,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBankerInfo::SetErrorCode()
		///     CPersonalBankerInfo::GetErrorCode()
		///     CPersonalBankerInfo::HasError()
		/// </summary>
		/// <remarks>
		/// Full code path coverage test.
		/// Full functional test.
		/// Test data includes input values to test edge conditions.
		/// </remarks>
        TEST_F(PersonalBankerInfoUnitTests,SetErrorCodeTest)
		{
			typedef struct
			{
				// Inputs.
				const PBError m_code;

				// Expected results.
				const PBError m_errorCodeExpected;
				const bool m_hasErrorExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ (PBError)0, (PBError)0, true },
				{ (PBError)1, (PBError)1, true },
				{ (PBError)0x7f, (PBError)0x7f, true },
				{ (PBError)0x80, (PBError)0x80, true },
				{ (PBError)0xff, (PBError)0xff, false },
				{ (PBError)0x100, (PBError)0x100, true },
				{ (PBError)0x7fffffff, (PBError)0x7fffffff, true },
				{ (PBError)0x80000000, (PBError)0x80000000, true },
				{ (PBError)0xffffffff, (PBError)0xffffffff, true },
				{ AccessDenied, AccessDenied, true },
				{ InvalidPin, InvalidPin, true },
				{ AccountInUse, AccountInUse, true },
				{ AccountLocked, AccountLocked, true },
				{ NewPinRequired, NewPinRequired, true },
				{ AccessError, AccessError, true },
				{ AcctHasNoPin, AcctHasNoPin, true },
				{ NoError, NoError, false },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CPersonalBankerInfo varPBInfo;

				// Perform operations to be tested.
				varPBInfo.SetErrorCode(TEST_DATA[testIndex].m_code);
				const CPersonalBankerInfo &pbInfo(varPBInfo);

				// Test #1.
				const PBError errorCodeResult = pbInfo.GetErrorCode();

				{
					const bool areEqual = errorCodeResult == TEST_DATA[testIndex].m_errorCodeExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED errorCodeResult=%d expected=%d testIndex=%d in %s"),
						(int)errorCodeResult,
						(int)TEST_DATA[testIndex].m_errorCodeExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #2.
				const bool hasErrorResult = pbInfo.HasError();

				{
					const bool areEqual = hasErrorResult == TEST_DATA[testIndex].m_hasErrorExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED hasErrorResult=%d expected=%d testIndex=%d (%d) in %s"),
						(int)hasErrorResult,
						(int)TEST_DATA[testIndex].m_hasErrorExpected,
						(int)testIndex,
						(int)TEST_DATA[testIndex].m_code,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// </summary>
        TEST_F(PersonalBankerInfoUnitTests,PropertiesAreOfflineTest)
		{
			//Setup
			const bool expectedPropertiesAreOffline = true;
			CPersonalBankerInfo PersonalBankerInfo;
			DWORD pbFlags(0x00000001);
			PersonalBankerInfo.SetPBFlags(pbFlags);
			
			// Test #1.
			{
				bool areEqual = expectedPropertiesAreOffline ==  PersonalBankerInfo.ArePropertiesOffline();
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED propertiesAreOffline=%d expected=%d in %s"),
					(int)PersonalBankerInfo.ArePropertiesOffline(),
					(int)expectedPropertiesAreOffline,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBankerInfo::SetPBFlags()
		///     CPersonalBankerInfo::ArePropertiesOffline()
		/// </summary>
        TEST_F(PersonalBankerInfoUnitTests,PropertiesAreOnlineTest)
		{
			//Setup
			const bool expectedPropertiesAreOffline = false;
			CPersonalBankerInfo PersonalBankerInfo;
			DWORD pbFlags(0);//Properties are online
			PersonalBankerInfo.SetPBFlags(pbFlags);
			
			// Test #1.
			{
				bool areEqual = expectedPropertiesAreOffline ==  PersonalBankerInfo.ArePropertiesOffline();
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED propertiesAreOffline=%d expected=%d in %s"),
					(int)PersonalBankerInfo.ArePropertiesOffline(),
					(int)expectedPropertiesAreOffline,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}
		}

		// Derive a class from CPersonalBankerInfo to access protected members.
		class CPersonalBankerInfoEx:
			public CPersonalBankerInfo
		{
			public:
				DWORD GetOffset() const { return m_dwOffset; }
				DWORD GetAllowedSize() const { return m_dwAllowedSize; }
				bool GetOffsetIsSet() const { return m_bOffsetIsSet; }
		};

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBankerInfo::CPersonalBankerInfo()
		/// Test with default parameters.
		/// </summary>
        TEST_F(PersonalBankerInfoUnitTests,CPersonalBankerInfoTest)
		{
			typedef struct
			{
				// Inputs.

				// Expected results.
				const MEMORY_TYPE m_typeExpected;
				const DWORD m_dwOffsetExpected;
				const DWORD m_dwAllowedSizeExpected;
				const bool m_bOffsetIsSetExpected;

				const byte m_cardNumberExpected[CARD_ID_LEN];
				PBError m_errorCodeExpected;

				const DWORD m_cashBalanceExpected;
				const DWORD m_pbFlagsExpected;
				const DWORD m_promo2BalanceExpected;
				const DWORD m_pointsAvailableExpected;
				const DWORD m_pointsPerDollarExpected;
				const DWORD m_sequenceNumberExpected;
				const DWORD m_minimumPointsExpected;
				const DWORD m_maximumPointsExpected;
				const DWORD m_minimumCashDownExpected;
				const DWORD m_maximumCashDownExpected;
				const DWORD m_minimumCashUpExpected;
				const DWORD m_maximumCashUpExpected;
				const DWORD m_minimumPromoDownExpected;
				const DWORD m_maximumPromoDownExpected;
				const DWORD m_minimumPromoUpExpected;
				const DWORD m_maximumPromoUpExpected;
				const DWORD m_promoBalanceOnGameExpected;
				const DWORD m_cashBalanceOnGameExpected;

				const int m_PBIDLengthExpected;
				const char m_PBTIDExpected[MAX_PBID_LENGTH];

				const bool m_bTransferCashToGameEnabledExpected;
				const bool m_bTransferPointsToGameEnabledExpected;
				const bool m_bTransferPromoDollarsToGameEnabledExpected;
				const bool m_bTransferCashFromGameEnabledExpected;
				const bool m_bGameDenomMatchesExpected;
				// const bool m_bEGMInitializedExpected; // 2013-06-18 Not directly accessible.
				const bool m_bTransferPromoFromGameEnabledExpected;
				// const bool m_bTransferPromoFromGameAutoEnabled; // 2013-06-18 Not publicly accessible.
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ MEMORY_NONE, NVRAM_PB_INFO_OFFSET, NVRAM_PB_INFO_SIZE , true,
					{ 0, 0, 0, 0, 0 }, AccessDenied,
					0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
					false, false, false, false, true, false
				},
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				//Setup

				// Perform operations to be tested.
				const CPersonalBankerInfo pbInfo;
				const CPersonalBankerInfoEx pbInfoEx; // Default constructor invokes constructors for base classes.

				// Test #A.
				{
					const MEMORY_TYPE type = pbInfo.GetMemoryType();

					bool areEqual = type == TEST_DATA[testIndex].m_typeExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED type=%d expected=%d testIndex=%d in %s"),
						(int)type,
						(int)TEST_DATA[testIndex].m_typeExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #B.
				{
					const DWORD dwOffset = pbInfoEx.GetOffset();

					bool areEqual = dwOffset == TEST_DATA[testIndex].m_dwOffsetExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED dwOffset=%u expected=%u testIndex=%d in %s"),
						(unsigned)dwOffset,
						(unsigned)TEST_DATA[testIndex].m_dwOffsetExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #C.
				{
					const DWORD dwAllowedSize = pbInfoEx.GetAllowedSize();

					bool areEqual = dwAllowedSize == TEST_DATA[testIndex].m_dwAllowedSizeExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED dwAllowedSize=%u expected=%u testIndex=%d in %s"),
						(unsigned)dwAllowedSize,
						(unsigned)TEST_DATA[testIndex].m_dwAllowedSizeExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #D.
				{
					const bool bOffsetIsSet = pbInfoEx.GetOffsetIsSet();

					bool areEqual = bOffsetIsSet == TEST_DATA[testIndex].m_bOffsetIsSetExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED bOffsetIsSet=%d expected=%d testIndex=%d in %s"),
						(int)bOffsetIsSet,
						(int)TEST_DATA[testIndex].m_bOffsetIsSetExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #1.
				const byte * const cardNumber = pbInfo.GetCardNumber();

				{

					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED cardNumber=%p expected=NOT NULL testIndex=%d in %s"),
						(const void *)cardNumber,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_NE(cardNumber, nullptr) << failedMsg;
				}

				// Test #2.

				{
					bool areEqual = 0 == memcmp(cardNumber,
						TEST_DATA[testIndex].m_cardNumberExpected,
						sizeof(TEST_DATA[testIndex].m_cardNumberExpected));

					std::string cardNumberString = CUtilities::ToHexString(reinterpret_cast<const BYTE *>(cardNumber),
						sizeof(TEST_DATA[testIndex].m_cardNumberExpected));
					std::string cardNumberExpectedString =
						CUtilities::ToHexString(reinterpret_cast<const BYTE *>(TEST_DATA[testIndex].m_cardNumberExpected),
							sizeof(TEST_DATA[testIndex].m_cardNumberExpected));
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED cardNumber={%s} expected={%s} testIndex=%d in %s"),
						static_cast<LPCTSTR>(cardNumberString.c_str()),
						static_cast<LPCTSTR>(cardNumberExpectedString.c_str()),
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #3.
				{
					const PBError errorCode = pbInfo.GetErrorCode();

					bool areEqual = errorCode == TEST_DATA[testIndex].m_errorCodeExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED errorCode=%d expected=%d testIndex=%d in %s"),
						(int)errorCode,
						(int)TEST_DATA[testIndex].m_errorCodeExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #4.
				{
					const DWORD cashBalance = pbInfo.GetCashBalance();
	
					bool areEqual = cashBalance == TEST_DATA[testIndex].m_cashBalanceExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED cashBalance=%u expected=%u testIndex=%d in %s"),
						(unsigned)cashBalance,
						(unsigned)TEST_DATA[testIndex].m_cashBalanceExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #5.
				{
					const DWORD pbFlags = pbInfo.GetPBFlags();

					bool areEqual = pbFlags == TEST_DATA[testIndex].m_pbFlagsExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED pbFlags=%u expected=%u testIndex=%d in %s"),
						(unsigned)pbFlags,
						(unsigned)TEST_DATA[testIndex].m_pbFlagsExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}


				// Test #6.
				{
					const DWORD promo2Balance = pbInfo.GetPromo2Balance();

					bool areEqual = promo2Balance == TEST_DATA[testIndex].m_promo2BalanceExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED promo2Balance=%u expected=%u testIndex=%d in %s"),
						(unsigned)promo2Balance,
						(unsigned)TEST_DATA[testIndex].m_promo2BalanceExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #7.
				{
					const DWORD pointsAvailable = pbInfo.GetPointsAvailable();

					bool areEqual = pointsAvailable == TEST_DATA[testIndex].m_pointsAvailableExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED pointsAvailable=%u expected=%u testIndex=%d in %s"),
						(unsigned)pointsAvailable,
						(unsigned)TEST_DATA[testIndex].m_pointsAvailableExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #8.
				{
					const DWORD pointsPerDollar = pbInfo.GetPointsPerDollar();

					bool areEqual = pointsPerDollar == TEST_DATA[testIndex].m_pointsPerDollarExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED pointsPerDollar=%u expected=%u testIndex=%d in %s"),
						(unsigned)pointsPerDollar,
						(unsigned)TEST_DATA[testIndex].m_pointsPerDollarExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #9.
				{
					const DWORD sequenceNumber = pbInfo.GetSequenceNumber();

					bool areEqual = sequenceNumber == TEST_DATA[testIndex].m_sequenceNumberExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED sequenceNumber=%u expected=%u testIndex=%d in %s"),
						(unsigned)sequenceNumber,
						(unsigned)TEST_DATA[testIndex].m_sequenceNumberExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #10.
				{
					const DWORD minimumPoints = pbInfo.GetMinimumPoints();

					bool areEqual = minimumPoints == TEST_DATA[testIndex].m_minimumPointsExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED minimumPoints=%u expected=%u testIndex=%d in %s"),
						(unsigned)minimumPoints,
						(unsigned)TEST_DATA[testIndex].m_minimumPointsExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #11.
				{
					const DWORD maximumPoints = pbInfo.GetMaximumPoints();

					bool areEqual = maximumPoints == TEST_DATA[testIndex].m_maximumPointsExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED maximumPoints=%u expected=%u testIndex=%d in %s"),
						(unsigned)maximumPoints,
						(unsigned)TEST_DATA[testIndex].m_maximumPointsExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #12.
				{
					const DWORD minimumCashDown = pbInfo.GetMinimumCashDown();

					bool areEqual = minimumCashDown == TEST_DATA[testIndex].m_minimumCashDownExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED minimumCashDown=%u expected=%u testIndex=%d in %s"),
						(unsigned)minimumCashDown,
						(unsigned)TEST_DATA[testIndex].m_minimumCashDownExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #13.
				{
					const DWORD maximumCashDown = pbInfo.GetMaximumCashDown();

					bool areEqual = maximumCashDown == TEST_DATA[testIndex].m_maximumCashDownExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED maximumCashDown=%u expected=%u testIndex=%d in %s"),
						(unsigned)maximumCashDown,
						(unsigned)TEST_DATA[testIndex].m_maximumCashDownExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #14.
				{
					const DWORD minimumCashUp = pbInfo.GetMinimumCashUp();

					bool areEqual = minimumCashUp == TEST_DATA[testIndex].m_minimumCashUpExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED minimumCashUp=%u expected=%u testIndex=%d in %s"),
						(unsigned)minimumCashUp,
						(unsigned)TEST_DATA[testIndex].m_minimumCashUpExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #15.
				{
					const DWORD maximumCashUp = pbInfo.GetMaximumCashUp();

					bool areEqual = maximumCashUp == TEST_DATA[testIndex].m_maximumCashUpExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED maximumCashUp=%u expected=%u testIndex=%d in %s"),
						(unsigned)maximumCashUp,
						(unsigned)TEST_DATA[testIndex].m_maximumCashUpExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #16.
				{
					const DWORD minimumPromoDown = pbInfo.GetMinimumPromoDown();

					bool areEqual = minimumPromoDown == TEST_DATA[testIndex].m_minimumPromoDownExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED minimumPromoDown=%u expected=%u testIndex=%d in %s"),
						(unsigned)minimumPromoDown,
						(unsigned)TEST_DATA[testIndex].m_minimumPromoDownExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #17.
				{
					const DWORD maximumPromoDown = pbInfo.GetMaximumPromoDown();

					bool areEqual = maximumPromoDown == TEST_DATA[testIndex].m_maximumPromoDownExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED maximumPromoDown=%u expected=%u testIndex=%d in %s"),
						(unsigned)maximumPromoDown,
						(unsigned)TEST_DATA[testIndex].m_maximumPromoDownExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #18.
				{
					const DWORD minimumPromoUp = pbInfo.GetMinimumPromoUp();

					bool areEqual = minimumPromoUp == TEST_DATA[testIndex].m_minimumPromoUpExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED minimumPromoUp=%u expected=%u testIndex=%d in %s"),
						(unsigned)minimumPromoUp,
						(unsigned)TEST_DATA[testIndex].m_minimumPromoUpExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #19.
				{
					const DWORD maximumPromoUp = pbInfo.GetMaximumPromoUp();

					bool areEqual = maximumPromoUp == TEST_DATA[testIndex].m_maximumPromoUpExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED maximumPromoUp=%u expected=%u testIndex=%d in %s"),
						(unsigned)maximumPromoUp,
						(unsigned)TEST_DATA[testIndex].m_maximumPromoUpExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #20.
				{
					const DWORD promoBalanceOnGame = pbInfo.GetPromoBalanceOnGame();

					bool areEqual = promoBalanceOnGame == TEST_DATA[testIndex].m_promoBalanceOnGameExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED promoBalanceOnGame=%u expected=%u testIndex=%d in %s"),
						(unsigned)promoBalanceOnGame,
						(unsigned)TEST_DATA[testIndex].m_promoBalanceOnGameExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #21.
				{
					const DWORD cashBalanceOnGame = pbInfo.GetCashBalanceOnGame();

					bool areEqual = cashBalanceOnGame == TEST_DATA[testIndex].m_cashBalanceOnGameExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED cashBalanceOnGame=%u expected=%u testIndex=%d in %s"),
						(unsigned)cashBalanceOnGame,
						(unsigned)TEST_DATA[testIndex].m_cashBalanceOnGameExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #22.
				{
					const int PBIDLength = pbInfo.GetPBIDLength();

					bool areEqual = PBIDLength == TEST_DATA[testIndex].m_PBIDLengthExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED PBIDLength=%d expected=%d testIndex=%d in %s"),
						(int)PBIDLength,
						(int)TEST_DATA[testIndex].m_PBIDLengthExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #23.
				const char * const PBTID = pbInfo.GetPersonalBankerID();

				{

					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED pbID=%p expected=NOT NULL testIndex=%d in %s"),
						(const void *)PBTID,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_NE(PBTID,nullptr) << failedMsg;
				}

				// Test #24.

				{
					bool areEqual = 0 == memcmp(PBTID,
						TEST_DATA[testIndex].m_PBTIDExpected,
						TEST_DATA[testIndex].m_PBIDLengthExpected);

					std::string PBTIDString = CUtilities::ToHexString(reinterpret_cast<const BYTE *>(PBTID), TEST_DATA[testIndex].m_PBIDLengthExpected);
					std::string PBTIDExpectedString = CUtilities::ToHexString(reinterpret_cast<const BYTE *>(TEST_DATA[testIndex].m_PBTIDExpected), TEST_DATA[testIndex].m_PBIDLengthExpected);
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED pbID={%s} expected={%s} in %s"),
						static_cast<LPCTSTR>(PBTIDString.c_str()),
						static_cast<LPCTSTR>(PBTIDExpectedString.c_str()),
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #25.
				{
					const bool bTransferCashToGameEnabled = pbInfo.IsTransferCashToGameEnabled();

					bool areEqual = bTransferCashToGameEnabled == TEST_DATA[testIndex].m_bTransferCashToGameEnabledExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED bTransferCashToGameEnabled=%d expected=%d testIndex=%d in %s"),
						(int)bTransferCashToGameEnabled,
						(int)TEST_DATA[testIndex].m_bTransferCashToGameEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #26.
				{
					const bool bTransferPointsToGameEnabled = pbInfo.IsTransferPointsToGameEnabled();

					bool areEqual = bTransferPointsToGameEnabled == TEST_DATA[testIndex].m_bTransferPointsToGameEnabledExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED bTransferPointsToGameEnabled=%d expected=%d testIndex=%d in %s"),
						(int)bTransferPointsToGameEnabled,
						(int)TEST_DATA[testIndex].m_bTransferPointsToGameEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #27.
				{
					const bool bTransferPromoDollarsToGameEnabled = pbInfo.IsTransferPromoDollarsToGameEnabled();

					bool areEqual = bTransferPromoDollarsToGameEnabled == TEST_DATA[testIndex].m_bTransferPromoDollarsToGameEnabledExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED bTransferPromoDollarsToGameEnabled=%d expected=%d testIndex=%d in %s"),
						(int)bTransferPromoDollarsToGameEnabled,
						(int)TEST_DATA[testIndex].m_bTransferPromoDollarsToGameEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #28.
				{
					const bool bTransferCashFromGameEnabled = pbInfo.IsTransferCashFromGameEnabled();

					bool areEqual = bTransferCashFromGameEnabled == TEST_DATA[testIndex].m_bTransferCashFromGameEnabledExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED bTransferCashFromGameEnabled=%d expected=%d testIndex=%d in %s"),
						(int)bTransferCashFromGameEnabled,
						(int)TEST_DATA[testIndex].m_bTransferCashFromGameEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #29.
				{
					const bool bGameDenomMatches = pbInfo.DoesGameDenomMatch();

					bool areEqual = bGameDenomMatches == TEST_DATA[testIndex].m_bGameDenomMatchesExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED bGameDenomMatches=%d expected=%d testIndex=%d in %s"),
						(int)bGameDenomMatches,
						(int)TEST_DATA[testIndex].m_bGameDenomMatchesExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #30.
				{
					const bool bTransferPromoFromGameEnabled = pbInfo.IsTransferPromoFromGameEnabled();

					bool areEqual = bTransferPromoFromGameEnabled == TEST_DATA[testIndex].m_bTransferPromoFromGameEnabledExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED bTransferPromoFromGameEnabled=%d expected=%d testIndex=%d in %s"),
						(int)bTransferPromoFromGameEnabled,
						(int)TEST_DATA[testIndex].m_bTransferPromoFromGameEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBankerInfo::CPersonalBankerInfo(MEMORY_NVRAM, false)
		///     CPersonalBankerInfo::CPersonalBankerInfo(MEMORY_NVRAM, true)
		///     CPersonalBankerInfo::StoreToNVRAM()
		///     CPersonalBankerInfo::BuildYourself()
		/// </summary>
        TEST_F(PersonalBankerInfoUnitTests,CPersonalBankerInfoNvramTest)
		{
			typedef struct
			{
				// Inputs.

				// Expected results.
				const MEMORY_TYPE m_typeExpected;

				const byte m_cardNumberExpected[CARD_ID_LEN];
				PBError m_errorCodeExpected;

				const DWORD m_cashBalanceExpected;
				const DWORD m_pbFlagsExpected;
				const DWORD m_promo2BalanceExpected;
				const DWORD m_pointsAvailableExpected;
				const DWORD m_pointsPerDollarExpected;
				const DWORD m_sequenceNumberExpected;
				const DWORD m_minimumPointsExpected;
				const DWORD m_maximumPointsExpected;
				const DWORD m_minimumCashDownExpected;
				const DWORD m_maximumCashDownExpected;
				const DWORD m_minimumCashUpExpected;
				const DWORD m_maximumCashUpExpected;
				const DWORD m_minimumPromoDownExpected;
				const DWORD m_maximumPromoDownExpected;
				const DWORD m_minimumPromoUpExpected;
				const DWORD m_maximumPromoUpExpected;
				const DWORD m_promoBalanceOnGameExpected;
				const DWORD m_cashBalanceOnGameExpected;

				const int m_PBIDLengthExpected;
				const char m_PBTIDExpected[MAX_PBID_LENGTH];

				const bool m_bTransferCashToGameEnabledExpected;
				const bool m_bTransferPointsToGameEnabledExpected;
				const bool m_bTransferPromoDollarsToGameEnabledExpected;
				const bool m_bTransferCashFromGameEnabledExpected;
				const bool m_bGameDenomMatchesExpected;
				// const bool m_bEGMInitializedExpected; // 2013-06-18 Not directly accessible.
				const bool m_bTransferPromoFromGameEnabledExpected;
				// const bool m_bTransferPromoFromGameAutoEnabled; // 2013-06-18 Not publicly accessible.
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ MEMORY_NVRAM,
					{ 0, 0, 0, 0, 0 }, AccessDenied,
					0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
					false, false, false, false, true, false
				},
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				//Setup

				// Perform operations to be tested.
				const CPersonalBankerInfo pbInfo1(MEMORY_NVRAM, false);
				const CPersonalBankerInfo pbInfo(MEMORY_NVRAM, true);

				// Test #A.
				{
					const MEMORY_TYPE type = pbInfo.GetMemoryType();

					bool areEqual = type == TEST_DATA[testIndex].m_typeExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED type=%d expected=%d testIndex=%d in %s"),
						(int)type,
						(int)TEST_DATA[testIndex].m_typeExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #1.
				const byte * const cardNumber = pbInfo.GetCardNumber();

				{

					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED cardNumber=%p expected=NOT NULL testIndex=%d in %s"),
						(const void *)cardNumber,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_NE(cardNumber,nullptr) << failedMsg;
				}

				// Test #2.

				{
					bool areEqual = 0 == memcmp(cardNumber,
						TEST_DATA[testIndex].m_cardNumberExpected,
						sizeof(TEST_DATA[testIndex].m_cardNumberExpected));

					std::string cardNumberString = CUtilities::ToHexString(reinterpret_cast<const BYTE *>(cardNumber),
						sizeof(TEST_DATA[testIndex].m_cardNumberExpected));
					std::string cardNumberExpectedString =
						CUtilities::ToHexString(reinterpret_cast<const BYTE *>(TEST_DATA[testIndex].m_cardNumberExpected),
							sizeof(TEST_DATA[testIndex].m_cardNumberExpected));
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED cardNumber={%s} expected={%s} testIndex=%d in %s"),
						static_cast<LPCTSTR>(cardNumberString.c_str()),
						static_cast<LPCTSTR>(cardNumberExpectedString.c_str()),
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #3.
				{
					const PBError errorCode = pbInfo.GetErrorCode();

					bool areEqual = errorCode == TEST_DATA[testIndex].m_errorCodeExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED errorCode=%d expected=%d testIndex=%d in %s"),
						(int)errorCode,
						(int)TEST_DATA[testIndex].m_errorCodeExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #4.
				{
					const DWORD cashBalance = pbInfo.GetCashBalance();
	
					bool areEqual = cashBalance == TEST_DATA[testIndex].m_cashBalanceExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED cashBalance=%u expected=%u testIndex=%d in %s"),
						(unsigned)cashBalance,
						(unsigned)TEST_DATA[testIndex].m_cashBalanceExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #5.
				{
					const DWORD pbFlags = pbInfo.GetPBFlags();

					bool areEqual = pbFlags == TEST_DATA[testIndex].m_pbFlagsExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED pbFlags=%u expected=%u testIndex=%d in %s"),
						(unsigned)pbFlags,
						(unsigned)TEST_DATA[testIndex].m_pbFlagsExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}


				// Test #6.
				{
					const DWORD promo2Balance = pbInfo.GetPromo2Balance();

					bool areEqual = promo2Balance == TEST_DATA[testIndex].m_promo2BalanceExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED promo2Balance=%u expected=%u testIndex=%d in %s"),
						(unsigned)promo2Balance,
						(unsigned)TEST_DATA[testIndex].m_promo2BalanceExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #7.
				{
					const DWORD pointsAvailable = pbInfo.GetPointsAvailable();

					bool areEqual = pointsAvailable == TEST_DATA[testIndex].m_pointsAvailableExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED pointsAvailable=%u expected=%u testIndex=%d in %s"),
						(unsigned)pointsAvailable,
						(unsigned)TEST_DATA[testIndex].m_pointsAvailableExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #8.
				{
					const DWORD pointsPerDollar = pbInfo.GetPointsPerDollar();

					bool areEqual = pointsPerDollar == TEST_DATA[testIndex].m_pointsPerDollarExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED pointsPerDollar=%u expected=%u testIndex=%d in %s"),
						(unsigned)pointsPerDollar,
						(unsigned)TEST_DATA[testIndex].m_pointsPerDollarExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #9.
				{
					const DWORD sequenceNumber = pbInfo.GetSequenceNumber();

					bool areEqual = sequenceNumber == TEST_DATA[testIndex].m_sequenceNumberExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED sequenceNumber=%u expected=%u testIndex=%d in %s"),
						(unsigned)sequenceNumber,
						(unsigned)TEST_DATA[testIndex].m_sequenceNumberExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #10.
				{
					const DWORD minimumPoints = pbInfo.GetMinimumPoints();

					bool areEqual = minimumPoints == TEST_DATA[testIndex].m_minimumPointsExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED minimumPoints=%u expected=%u testIndex=%d in %s"),
						(unsigned)minimumPoints,
						(unsigned)TEST_DATA[testIndex].m_minimumPointsExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #11.
				{
					const DWORD maximumPoints = pbInfo.GetMaximumPoints();

					bool areEqual = maximumPoints == TEST_DATA[testIndex].m_maximumPointsExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED maximumPoints=%u expected=%u testIndex=%d in %s"),
						(unsigned)maximumPoints,
						(unsigned)TEST_DATA[testIndex].m_maximumPointsExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #12.
				{
					const DWORD minimumCashDown = pbInfo.GetMinimumCashDown();

					bool areEqual = minimumCashDown == TEST_DATA[testIndex].m_minimumCashDownExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED minimumCashDown=%u expected=%u testIndex=%d in %s"),
						(unsigned)minimumCashDown,
						(unsigned)TEST_DATA[testIndex].m_minimumCashDownExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #13.
				{
					const DWORD maximumCashDown = pbInfo.GetMaximumCashDown();

					bool areEqual = maximumCashDown == TEST_DATA[testIndex].m_maximumCashDownExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED maximumCashDown=%u expected=%u testIndex=%d in %s"),
						(unsigned)maximumCashDown,
						(unsigned)TEST_DATA[testIndex].m_maximumCashDownExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #14.
				{
					const DWORD minimumCashUp = pbInfo.GetMinimumCashUp();

					bool areEqual = minimumCashUp == TEST_DATA[testIndex].m_minimumCashUpExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED minimumCashUp=%u expected=%u testIndex=%d in %s"),
						(unsigned)minimumCashUp,
						(unsigned)TEST_DATA[testIndex].m_minimumCashUpExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #15.
				{
					const DWORD maximumCashUp = pbInfo.GetMaximumCashUp();

					bool areEqual = maximumCashUp == TEST_DATA[testIndex].m_maximumCashUpExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED maximumCashUp=%u expected=%u testIndex=%d in %s"),
						(unsigned)maximumCashUp,
						(unsigned)TEST_DATA[testIndex].m_maximumCashUpExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #16.
				{
					const DWORD minimumPromoDown = pbInfo.GetMinimumPromoDown();

					bool areEqual = minimumPromoDown == TEST_DATA[testIndex].m_minimumPromoDownExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED minimumPromoDown=%u expected=%u testIndex=%d in %s"),
						(unsigned)minimumPromoDown,
						(unsigned)TEST_DATA[testIndex].m_minimumPromoDownExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #17.
				{
					const DWORD maximumPromoDown = pbInfo.GetMaximumPromoDown();

					bool areEqual = maximumPromoDown == TEST_DATA[testIndex].m_maximumPromoDownExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED maximumPromoDown=%u expected=%u testIndex=%d in %s"),
						(unsigned)maximumPromoDown,
						(unsigned)TEST_DATA[testIndex].m_maximumPromoDownExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #18.
				{
					const DWORD minimumPromoUp = pbInfo.GetMinimumPromoUp();

					bool areEqual = minimumPromoUp == TEST_DATA[testIndex].m_minimumPromoUpExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED minimumPromoUp=%u expected=%u testIndex=%d in %s"),
						(unsigned)minimumPromoUp,
						(unsigned)TEST_DATA[testIndex].m_minimumPromoUpExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #19.
				{
					const DWORD maximumPromoUp = pbInfo.GetMaximumPromoUp();

					bool areEqual = maximumPromoUp == TEST_DATA[testIndex].m_maximumPromoUpExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED maximumPromoUp=%u expected=%u testIndex=%d in %s"),
						(unsigned)maximumPromoUp,
						(unsigned)TEST_DATA[testIndex].m_maximumPromoUpExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #20.
				{
					const DWORD promoBalanceOnGame = pbInfo.GetPromoBalanceOnGame();

					bool areEqual = promoBalanceOnGame == TEST_DATA[testIndex].m_promoBalanceOnGameExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED promoBalanceOnGame=%u expected=%u testIndex=%d in %s"),
						(unsigned)promoBalanceOnGame,
						(unsigned)TEST_DATA[testIndex].m_promoBalanceOnGameExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #21.
				{
					const DWORD cashBalanceOnGame = pbInfo.GetCashBalanceOnGame();

					bool areEqual = cashBalanceOnGame == TEST_DATA[testIndex].m_cashBalanceOnGameExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED cashBalanceOnGame=%u expected=%u testIndex=%d in %s"),
						(unsigned)cashBalanceOnGame,
						(unsigned)TEST_DATA[testIndex].m_cashBalanceOnGameExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #22.
				{
					const int PBIDLength = pbInfo.GetPBIDLength();

					bool areEqual = PBIDLength == TEST_DATA[testIndex].m_PBIDLengthExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED PBIDLength=%d expected=%d testIndex=%d in %s"),
						(int)PBIDLength,
						(int)TEST_DATA[testIndex].m_PBIDLengthExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #23.
				const char * const PBTID = pbInfo.GetPersonalBankerID();

				{

					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED pbID=%p expected=NOT NULL testIndex=%d in %s"),
						(const void *)PBTID,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_NE(PBTID,nullptr) << failedMsg;
				}

				// Test #24.

				{
					bool areEqual = 0 == memcmp(PBTID,
						TEST_DATA[testIndex].m_PBTIDExpected,
						TEST_DATA[testIndex].m_PBIDLengthExpected);

					std::string PBTIDString = CUtilities::ToHexString(reinterpret_cast<const BYTE *>(PBTID), TEST_DATA[testIndex].m_PBIDLengthExpected);
					std::string PBTIDExpectedString = CUtilities::ToHexString(reinterpret_cast<const BYTE *>(TEST_DATA[testIndex].m_PBTIDExpected), TEST_DATA[testIndex].m_PBIDLengthExpected);
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED pbID={%s} expected={%s} in %s"),
						static_cast<LPCTSTR>(PBTIDString.c_str()),
						static_cast<LPCTSTR>(PBTIDExpectedString.c_str()),
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #25.
				{
					const bool bTransferCashToGameEnabled = pbInfo.IsTransferCashToGameEnabled();

					bool areEqual = bTransferCashToGameEnabled == TEST_DATA[testIndex].m_bTransferCashToGameEnabledExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED bTransferCashToGameEnabled=%d expected=%d testIndex=%d in %s"),
						(int)bTransferCashToGameEnabled,
						(int)TEST_DATA[testIndex].m_bTransferCashToGameEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #26.
				{
					const bool bTransferPointsToGameEnabled = pbInfo.IsTransferPointsToGameEnabled();

					bool areEqual = bTransferPointsToGameEnabled == TEST_DATA[testIndex].m_bTransferPointsToGameEnabledExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED bTransferPointsToGameEnabled=%d expected=%d testIndex=%d in %s"),
						(int)bTransferPointsToGameEnabled,
						(int)TEST_DATA[testIndex].m_bTransferPointsToGameEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #27.
				{
					const bool bTransferPromoDollarsToGameEnabled = pbInfo.IsTransferPromoDollarsToGameEnabled();

					bool areEqual = bTransferPromoDollarsToGameEnabled == TEST_DATA[testIndex].m_bTransferPromoDollarsToGameEnabledExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED bTransferPromoDollarsToGameEnabled=%d expected=%d testIndex=%d in %s"),
						(int)bTransferPromoDollarsToGameEnabled,
						(int)TEST_DATA[testIndex].m_bTransferPromoDollarsToGameEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #28.
				{
					const bool bTransferCashFromGameEnabled = pbInfo.IsTransferCashFromGameEnabled();

					bool areEqual = bTransferCashFromGameEnabled == TEST_DATA[testIndex].m_bTransferCashFromGameEnabledExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED bTransferCashFromGameEnabled=%d expected=%d testIndex=%d in %s"),
						(int)bTransferCashFromGameEnabled,
						(int)TEST_DATA[testIndex].m_bTransferCashFromGameEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #29.
				{
					const bool bGameDenomMatches = pbInfo.DoesGameDenomMatch();

					bool areEqual = bGameDenomMatches == TEST_DATA[testIndex].m_bGameDenomMatchesExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED bGameDenomMatches=%d expected=%d testIndex=%d in %s"),
						(int)bGameDenomMatches,
						(int)TEST_DATA[testIndex].m_bGameDenomMatchesExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #30.
				{
					const bool bTransferPromoFromGameEnabled = pbInfo.IsTransferPromoFromGameEnabled();

					bool areEqual = bTransferPromoFromGameEnabled == TEST_DATA[testIndex].m_bTransferPromoFromGameEnabledExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED bTransferPromoFromGameEnabled=%d expected=%d testIndex=%d in %s"),
						(int)bTransferPromoFromGameEnabled,
						(int)TEST_DATA[testIndex].m_bTransferPromoFromGameEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBankerInfo::CPersonalBankerInfo(MEMORY_NVRAM, false)
		///     CPersonalBankerInfo::CPersonalBankerInfo(const CPersonalBankerInfo&)
		/// </summary>
        TEST_F(PersonalBankerInfoUnitTests,CPersonalBankerInfoCopyTest)
		{
			typedef struct
			{
				// Inputs.

				// Expected results.
				const MEMORY_TYPE m_typeExpected;

				const byte m_cardNumberExpected[CARD_ID_LEN];
				PBError m_errorCodeExpected;

				const DWORD m_cashBalanceExpected;
				const DWORD m_pbFlagsExpected;
				const DWORD m_promo2BalanceExpected;
				const DWORD m_pointsAvailableExpected;
				const DWORD m_pointsPerDollarExpected;
				const DWORD m_sequenceNumberExpected;
				const DWORD m_minimumPointsExpected;
				const DWORD m_maximumPointsExpected;
				const DWORD m_minimumCashDownExpected;
				const DWORD m_maximumCashDownExpected;
				const DWORD m_minimumCashUpExpected;
				const DWORD m_maximumCashUpExpected;
				const DWORD m_minimumPromoDownExpected;
				const DWORD m_maximumPromoDownExpected;
				const DWORD m_minimumPromoUpExpected;
				const DWORD m_maximumPromoUpExpected;
				const DWORD m_promoBalanceOnGameExpected;
				const DWORD m_cashBalanceOnGameExpected;

				const int m_PBIDLengthExpected;
				const char m_PBTIDExpected[MAX_PBID_LENGTH];

				const bool m_bTransferCashToGameEnabledExpected;
				const bool m_bTransferPointsToGameEnabledExpected;
				const bool m_bTransferPromoDollarsToGameEnabledExpected;
				const bool m_bTransferCashFromGameEnabledExpected;
				const bool m_bGameDenomMatchesExpected;
				// const bool m_bEGMInitializedExpected; // 2013-06-18 Not directly accessible.
				const bool m_bTransferPromoFromGameEnabledExpected;
				// const bool m_bTransferPromoFromGameAutoEnabled; // 2013-06-18 Not publicly accessible.
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ MEMORY_NONE,
					{ 0, 0, 0, 0, 0 }, AccessDenied,
					0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
					false, false, false, false, true, false
				},
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				//Setup

				// Perform operations to be tested.
				const CPersonalBankerInfo pbInfo1(MEMORY_NVRAM, false);
				const CPersonalBankerInfo pbInfo(pbInfo1);

				// Test #A.
				{
					const MEMORY_TYPE type = pbInfo.GetMemoryType();

					bool areEqual = type == TEST_DATA[testIndex].m_typeExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED type=%d expected=%d testIndex=%d in %s"),
						(int)type,
						(int)TEST_DATA[testIndex].m_typeExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #1.
				const byte * const cardNumber = pbInfo.GetCardNumber();

				{

					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED cardNumber=%p expected=NOT NULL testIndex=%d in %s"),
						(const void *)cardNumber,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_NE(cardNumber,nullptr) << failedMsg;
				}

				// Test #2.

				{
					bool areEqual = 0 == memcmp(cardNumber,
						TEST_DATA[testIndex].m_cardNumberExpected,
						sizeof(TEST_DATA[testIndex].m_cardNumberExpected));

					std::string cardNumberString = CUtilities::ToHexString(reinterpret_cast<const BYTE *>(cardNumber),
						sizeof(TEST_DATA[testIndex].m_cardNumberExpected));
					std::string cardNumberExpectedString =
						CUtilities::ToHexString(reinterpret_cast<const BYTE *>(TEST_DATA[testIndex].m_cardNumberExpected),
							sizeof(TEST_DATA[testIndex].m_cardNumberExpected));
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED cardNumber={%s} expected={%s} testIndex=%d in %s"),
						static_cast<LPCTSTR>(cardNumberString.c_str()),
						static_cast<LPCTSTR>(cardNumberExpectedString.c_str()),
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #3.
				{
					const PBError errorCode = pbInfo.GetErrorCode();

					bool areEqual = errorCode == TEST_DATA[testIndex].m_errorCodeExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED errorCode=%d expected=%d testIndex=%d in %s"),
						(int)errorCode,
						(int)TEST_DATA[testIndex].m_errorCodeExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #4.
				{
					const DWORD cashBalance = pbInfo.GetCashBalance();
	
					bool areEqual = cashBalance == TEST_DATA[testIndex].m_cashBalanceExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED cashBalance=%u expected=%u testIndex=%d in %s"),
						(unsigned)cashBalance,
						(unsigned)TEST_DATA[testIndex].m_cashBalanceExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #5.
				{
					const DWORD pbFlags = pbInfo.GetPBFlags();

					bool areEqual = pbFlags == TEST_DATA[testIndex].m_pbFlagsExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED pbFlags=%u expected=%u testIndex=%d in %s"),
						(unsigned)pbFlags,
						(unsigned)TEST_DATA[testIndex].m_pbFlagsExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}


				// Test #6.
				{
					const DWORD promo2Balance = pbInfo.GetPromo2Balance();

					bool areEqual = promo2Balance == TEST_DATA[testIndex].m_promo2BalanceExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED promo2Balance=%u expected=%u testIndex=%d in %s"),
						(unsigned)promo2Balance,
						(unsigned)TEST_DATA[testIndex].m_promo2BalanceExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #7.
				{
					const DWORD pointsAvailable = pbInfo.GetPointsAvailable();

					bool areEqual = pointsAvailable == TEST_DATA[testIndex].m_pointsAvailableExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED pointsAvailable=%u expected=%u testIndex=%d in %s"),
						(unsigned)pointsAvailable,
						(unsigned)TEST_DATA[testIndex].m_pointsAvailableExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #8.
				{
					const DWORD pointsPerDollar = pbInfo.GetPointsPerDollar();

					bool areEqual = pointsPerDollar == TEST_DATA[testIndex].m_pointsPerDollarExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED pointsPerDollar=%u expected=%u testIndex=%d in %s"),
						(unsigned)pointsPerDollar,
						(unsigned)TEST_DATA[testIndex].m_pointsPerDollarExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #9.
				{
					const DWORD sequenceNumber = pbInfo.GetSequenceNumber();

					bool areEqual = sequenceNumber == TEST_DATA[testIndex].m_sequenceNumberExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED sequenceNumber=%u expected=%u testIndex=%d in %s"),
						(unsigned)sequenceNumber,
						(unsigned)TEST_DATA[testIndex].m_sequenceNumberExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #10.
				{
					const DWORD minimumPoints = pbInfo.GetMinimumPoints();

					bool areEqual = minimumPoints == TEST_DATA[testIndex].m_minimumPointsExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED minimumPoints=%u expected=%u testIndex=%d in %s"),
						(unsigned)minimumPoints,
						(unsigned)TEST_DATA[testIndex].m_minimumPointsExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #11.
				{
					const DWORD maximumPoints = pbInfo.GetMaximumPoints();

					bool areEqual = maximumPoints == TEST_DATA[testIndex].m_maximumPointsExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED maximumPoints=%u expected=%u testIndex=%d in %s"),
						(unsigned)maximumPoints,
						(unsigned)TEST_DATA[testIndex].m_maximumPointsExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #12.
				{
					const DWORD minimumCashDown = pbInfo.GetMinimumCashDown();

					bool areEqual = minimumCashDown == TEST_DATA[testIndex].m_minimumCashDownExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED minimumCashDown=%u expected=%u testIndex=%d in %s"),
						(unsigned)minimumCashDown,
						(unsigned)TEST_DATA[testIndex].m_minimumCashDownExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #13.
				{
					const DWORD maximumCashDown = pbInfo.GetMaximumCashDown();

					bool areEqual = maximumCashDown == TEST_DATA[testIndex].m_maximumCashDownExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED maximumCashDown=%u expected=%u testIndex=%d in %s"),
						(unsigned)maximumCashDown,
						(unsigned)TEST_DATA[testIndex].m_maximumCashDownExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #14.
				{
					const DWORD minimumCashUp = pbInfo.GetMinimumCashUp();

					bool areEqual = minimumCashUp == TEST_DATA[testIndex].m_minimumCashUpExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED minimumCashUp=%u expected=%u testIndex=%d in %s"),
						(unsigned)minimumCashUp,
						(unsigned)TEST_DATA[testIndex].m_minimumCashUpExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #15.
				{
					const DWORD maximumCashUp = pbInfo.GetMaximumCashUp();

					bool areEqual = maximumCashUp == TEST_DATA[testIndex].m_maximumCashUpExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED maximumCashUp=%u expected=%u testIndex=%d in %s"),
						(unsigned)maximumCashUp,
						(unsigned)TEST_DATA[testIndex].m_maximumCashUpExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #16.
				{
					const DWORD minimumPromoDown = pbInfo.GetMinimumPromoDown();

					bool areEqual = minimumPromoDown == TEST_DATA[testIndex].m_minimumPromoDownExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED minimumPromoDown=%u expected=%u testIndex=%d in %s"),
						(unsigned)minimumPromoDown,
						(unsigned)TEST_DATA[testIndex].m_minimumPromoDownExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #17.
				{
					const DWORD maximumPromoDown = pbInfo.GetMaximumPromoDown();

					bool areEqual = maximumPromoDown == TEST_DATA[testIndex].m_maximumPromoDownExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED maximumPromoDown=%u expected=%u testIndex=%d in %s"),
						(unsigned)maximumPromoDown,
						(unsigned)TEST_DATA[testIndex].m_maximumPromoDownExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #18.
				{
					const DWORD minimumPromoUp = pbInfo.GetMinimumPromoUp();

					bool areEqual = minimumPromoUp == TEST_DATA[testIndex].m_minimumPromoUpExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED minimumPromoUp=%u expected=%u testIndex=%d in %s"),
						(unsigned)minimumPromoUp,
						(unsigned)TEST_DATA[testIndex].m_minimumPromoUpExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #19.
				{
					const DWORD maximumPromoUp = pbInfo.GetMaximumPromoUp();

					bool areEqual = maximumPromoUp == TEST_DATA[testIndex].m_maximumPromoUpExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED maximumPromoUp=%u expected=%u testIndex=%d in %s"),
						(unsigned)maximumPromoUp,
						(unsigned)TEST_DATA[testIndex].m_maximumPromoUpExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #20.
				{
					const DWORD promoBalanceOnGame = pbInfo.GetPromoBalanceOnGame();

					bool areEqual = promoBalanceOnGame == TEST_DATA[testIndex].m_promoBalanceOnGameExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED promoBalanceOnGame=%u expected=%u testIndex=%d in %s"),
						(unsigned)promoBalanceOnGame,
						(unsigned)TEST_DATA[testIndex].m_promoBalanceOnGameExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #21.
				{
					const DWORD cashBalanceOnGame = pbInfo.GetCashBalanceOnGame();

					bool areEqual = cashBalanceOnGame == TEST_DATA[testIndex].m_cashBalanceOnGameExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED cashBalanceOnGame=%u expected=%u testIndex=%d in %s"),
						(unsigned)cashBalanceOnGame,
						(unsigned)TEST_DATA[testIndex].m_cashBalanceOnGameExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #22.
				{
					const int PBIDLength = pbInfo.GetPBIDLength();

					bool areEqual = PBIDLength == TEST_DATA[testIndex].m_PBIDLengthExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED PBIDLength=%d expected=%d testIndex=%d in %s"),
						(int)PBIDLength,
						(int)TEST_DATA[testIndex].m_PBIDLengthExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #23.
				const char * const PBTID = pbInfo.GetPersonalBankerID();

				{

					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED pbID=%p expected=NOT NULL testIndex=%d in %s"),
						(const void *)PBTID,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_NE(PBTID,nullptr) << failedMsg;
				}

				// Test #24.

				{
					bool areEqual = 0 == memcmp(PBTID,
						TEST_DATA[testIndex].m_PBTIDExpected,
						TEST_DATA[testIndex].m_PBIDLengthExpected);

					std::string PBTIDString = CUtilities::ToHexString(reinterpret_cast<const BYTE *>(PBTID), TEST_DATA[testIndex].m_PBIDLengthExpected);
					std::string PBTIDExpectedString = CUtilities::ToHexString(reinterpret_cast<const BYTE *>(TEST_DATA[testIndex].m_PBTIDExpected), TEST_DATA[testIndex].m_PBIDLengthExpected);
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED pbID={%s} expected={%s} in %s"),
						static_cast<LPCTSTR>(PBTIDString.c_str()),
						static_cast<LPCTSTR>(PBTIDExpectedString.c_str()),
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #25.
				{
					const bool bTransferCashToGameEnabled = pbInfo.IsTransferCashToGameEnabled();

					bool areEqual = bTransferCashToGameEnabled == TEST_DATA[testIndex].m_bTransferCashToGameEnabledExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED bTransferCashToGameEnabled=%d expected=%d testIndex=%d in %s"),
						(int)bTransferCashToGameEnabled,
						(int)TEST_DATA[testIndex].m_bTransferCashToGameEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #26.
				{
					const bool bTransferPointsToGameEnabled = pbInfo.IsTransferPointsToGameEnabled();

					bool areEqual = bTransferPointsToGameEnabled == TEST_DATA[testIndex].m_bTransferPointsToGameEnabledExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED bTransferPointsToGameEnabled=%d expected=%d testIndex=%d in %s"),
						(int)bTransferPointsToGameEnabled,
						(int)TEST_DATA[testIndex].m_bTransferPointsToGameEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #27.
				{
					const bool bTransferPromoDollarsToGameEnabled = pbInfo.IsTransferPromoDollarsToGameEnabled();

					bool areEqual = bTransferPromoDollarsToGameEnabled == TEST_DATA[testIndex].m_bTransferPromoDollarsToGameEnabledExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED bTransferPromoDollarsToGameEnabled=%d expected=%d testIndex=%d in %s"),
						(int)bTransferPromoDollarsToGameEnabled,
						(int)TEST_DATA[testIndex].m_bTransferPromoDollarsToGameEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #28.
				{
					const bool bTransferCashFromGameEnabled = pbInfo.IsTransferCashFromGameEnabled();

					bool areEqual = bTransferCashFromGameEnabled == TEST_DATA[testIndex].m_bTransferCashFromGameEnabledExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED bTransferCashFromGameEnabled=%d expected=%d testIndex=%d in %s"),
						(int)bTransferCashFromGameEnabled,
						(int)TEST_DATA[testIndex].m_bTransferCashFromGameEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #29.
				{
					const bool bGameDenomMatches = pbInfo.DoesGameDenomMatch();

					bool areEqual = bGameDenomMatches == TEST_DATA[testIndex].m_bGameDenomMatchesExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED bGameDenomMatches=%d expected=%d testIndex=%d in %s"),
						(int)bGameDenomMatches,
						(int)TEST_DATA[testIndex].m_bGameDenomMatchesExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #30.
				{
					const bool bTransferPromoFromGameEnabled = pbInfo.IsTransferPromoFromGameEnabled();

					bool areEqual = bTransferPromoFromGameEnabled == TEST_DATA[testIndex].m_bTransferPromoFromGameEnabledExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED bTransferPromoFromGameEnabled=%d expected=%d testIndex=%d in %s"),
						(int)bTransferPromoFromGameEnabled,
						(int)TEST_DATA[testIndex].m_bTransferPromoFromGameEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBankerInfo::CPersonalBankerInfo(MEMORY_NVRAM, false)
		///     CPersonalBankerInfo::CPersonalBankerInfo(const CPersonalBankerInfo&)
		/// </summary>
        TEST_F(PersonalBankerInfoUnitTests,CPersonalBankerInfoCopy2Test)
		{
			typedef struct
			{
				// Inputs.
				const MEMORY_TYPE m_type;

				const byte m_cardNumber[CARD_ID_LEN];
				PBError m_errorCode;

				const DWORD m_cashBalance;
				const DWORD m_pbFlags;
				const DWORD m_promo2Balance;
				const DWORD m_pointsAvailable;
				const DWORD m_pointsPerDollar;
				const DWORD m_sequenceNumber;
				const DWORD m_minimumPoints;
				const DWORD m_maximumPoints;
				const DWORD m_minimumCashDown;
				const DWORD m_maximumCashDown;
				const DWORD m_minimumCashUp;
				const DWORD m_maximumCashUp;
				const DWORD m_minimumPromoDown;
				const DWORD m_maximumPromoDown;
				const DWORD m_minimumPromoUp;
				const DWORD m_maximumPromoUp;
				const DWORD m_promoBalanceOnGame;
				const DWORD m_cashBalanceOnGame;

				const int m_PBIDLength;
				const char m_PBTID[MAX_PBID_LENGTH];

				const bool m_bTransferCashToGameEnabled;
				const bool m_bTransferPointsToGameEnabled;
				const bool m_bTransferPromoDollarsToGameEnabled;
				const bool m_bTransferCashFromGameEnabled;
				const bool m_bGameDenomMatches;
				const bool m_bEGMInitialized;
				const bool m_bTransferPromoFromGameEnabled;
				const bool m_bTransferPromoFromGameAutoEnabled;

				// Expected results.
				const MEMORY_TYPE m_typeExpected;

				const byte m_cardNumberExpected[CARD_ID_LEN];
				PBError m_errorCodeExpected;

				const DWORD m_cashBalanceExpected;
				const DWORD m_pbFlagsExpected;
				const DWORD m_promo2BalanceExpected;
				const DWORD m_pointsAvailableExpected;
				const DWORD m_pointsPerDollarExpected;
				const DWORD m_sequenceNumberExpected;
				const DWORD m_minimumPointsExpected;
				const DWORD m_maximumPointsExpected;
				const DWORD m_minimumCashDownExpected;
				const DWORD m_maximumCashDownExpected;
				const DWORD m_minimumCashUpExpected;
				const DWORD m_maximumCashUpExpected;
				const DWORD m_minimumPromoDownExpected;
				const DWORD m_maximumPromoDownExpected;
				const DWORD m_minimumPromoUpExpected;
				const DWORD m_maximumPromoUpExpected;
				const DWORD m_promoBalanceOnGameExpected;
				const DWORD m_cashBalanceOnGameExpected;

				const int m_PBIDLengthExpected;
				const char m_PBTIDExpected[MAX_PBID_LENGTH];

				const bool m_bTransferCashToGameEnabledExpected;
				const bool m_bTransferPointsToGameEnabledExpected;
				const bool m_bTransferPromoDollarsToGameEnabledExpected;
				const bool m_bTransferCashFromGameEnabledExpected;
				const bool m_bGameDenomMatchesExpected;
				// const bool m_bEGMInitializedExpected; // 2013-06-18 Not directly accessible.
				const bool m_bTransferPromoFromGameEnabledExpected;
				// const bool m_bTransferPromoFromGameAutoEnabled; // 2013-06-18 Not publicly accessible.
				const bool m_bTransferToGameEnabledExpected; // Test m_bEGMInitialized indirectly.
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ MEMORY_EEPROM,
					{ 1, 2, 3, 4, 5 }, AccountInUse,
					1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
					20, { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
					true, true, true, true, false, false, true, true,
					MEMORY_NONE,
					{ 1, 2, 3, 4, 5 }, AccountInUse,
					1, 2, 3, 4, 5, 6, 7, 4, 9, 0, 11, 12, 13, 0, 15, 16, 17, 18,
					20, { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
					true, true, true, true, false, true, false
				},
			};

			// More test data - expected results that are the same for each iteration.
			const bool bIsEFT(true);

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				//Setup
				CPersonalBankerInfo pbInfo1(TEST_DATA[testIndex].m_type, false);
				pbInfo1.SetCardNumber(TEST_DATA[testIndex].m_cardNumber);
				pbInfo1.SetErrorCode(TEST_DATA[testIndex].m_errorCode);
				pbInfo1.SetCashBalance(TEST_DATA[testIndex].m_cashBalance);
				pbInfo1.SetPBFlags(TEST_DATA[testIndex].m_pbFlags);
				pbInfo1.SetPromo2Balance(TEST_DATA[testIndex].m_promo2Balance);
				pbInfo1.SetPointsAvailable(TEST_DATA[testIndex].m_pointsAvailable);
				pbInfo1.SetPointsPerDollar(TEST_DATA[testIndex].m_pointsPerDollar);
				pbInfo1.SetSequenceNumber(TEST_DATA[testIndex].m_sequenceNumber);
				pbInfo1.SetMinimumPoints(TEST_DATA[testIndex].m_minimumPoints);
				pbInfo1.SetMaximumPoints(TEST_DATA[testIndex].m_maximumPoints);
				pbInfo1.SetMinimumCashDown(TEST_DATA[testIndex].m_minimumCashDown);
				pbInfo1.SetMaximumCashDown(TEST_DATA[testIndex].m_maximumCashDown);
				pbInfo1.SetMinimumCashUp(TEST_DATA[testIndex].m_minimumCashUp);
				pbInfo1.SetMaximumCashUp(TEST_DATA[testIndex].m_maximumCashUp);
				pbInfo1.SetMinimumPromoDown(TEST_DATA[testIndex].m_minimumPromoDown);
				pbInfo1.SetMaximumPromoDown(TEST_DATA[testIndex].m_maximumPromoDown);
				pbInfo1.SetMinimumPromoUp(TEST_DATA[testIndex].m_minimumPromoUp);
				pbInfo1.SetMaximumPromoUp(TEST_DATA[testIndex].m_maximumPromoUp);
				pbInfo1.SetPromoBalanceOnGame(TEST_DATA[testIndex].m_promoBalanceOnGame);
				pbInfo1.SetCashBalanceOnGame(TEST_DATA[testIndex].m_cashBalanceOnGame);
				pbInfo1.SetCashBalanceOnGame(TEST_DATA[testIndex].m_cashBalanceOnGame);

				pbInfo1.SetPersonalBankerID(TEST_DATA[testIndex].m_PBTID, TEST_DATA[testIndex].m_PBIDLength);

				CConfig config(false, "");
				config.SetPBTCashEnabled_Global(TEST_DATA[testIndex].m_bTransferCashToGameEnabled);
				config.SetPBTCashEnabled_local(TEST_DATA[testIndex].m_bTransferCashToGameEnabled);
				config.SetPBTPointsEnabled_Global(TEST_DATA[testIndex].m_bTransferPointsToGameEnabled);
				config.SetPBTPointsAsPromo(TEST_DATA[testIndex].m_bTransferPointsToGameEnabled);
				config.SetPBTPromoEnabled_Global(TEST_DATA[testIndex].m_bTransferPromoDollarsToGameEnabled);
				config.SetPBTPromoEnabled_local(TEST_DATA[testIndex].m_bTransferPromoDollarsToGameEnabled);
				config.SetPBFromGameEnabled(TEST_DATA[testIndex].m_bTransferCashFromGameEnabled);
				config.SetGameDenom(1);
				config.SetDenomination(TEST_DATA[testIndex].m_bGameDenomMatches ? 1 : 5);
				if (TEST_DATA[testIndex].m_bTransferPromoFromGameEnabled)
				{
					config.SetSenOption2Raw(config.GetSenOption2Raw() + SO2_NCEP_OUT_ENABLED);
				}
				if (TEST_DATA[testIndex].m_bTransferPromoFromGameAutoEnabled)
				{
					config.SetSenOption2Raw(config.GetSenOption2Raw() + SO2_NCEP_AUTO_UPLOAD);
				}
				pbInfo1.SetConfigData(config, TEST_DATA[testIndex].m_bEGMInitialized, bIsEFT);

				// Perform operations to be tested.
				const CPersonalBankerInfo pbInfo(pbInfo1);

				// Test #A.
				{
					const MEMORY_TYPE type = pbInfo.GetMemoryType();

					bool areEqual = type == TEST_DATA[testIndex].m_typeExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED type=%d expected=%d testIndex=%d in %s"),
						(int)type,
						(int)TEST_DATA[testIndex].m_typeExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #1.
				const byte * const cardNumber = pbInfo.GetCardNumber();

				{

					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED cardNumber=%p expected=NOT NULL testIndex=%d in %s"),
						(const void *)cardNumber,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_NE(cardNumber, nullptr) << failedMsg;
				}

				// Test #2.

				{
					bool areEqual = 0 == memcmp(cardNumber,
						TEST_DATA[testIndex].m_cardNumberExpected,
						sizeof(TEST_DATA[testIndex].m_cardNumberExpected));

					std::string cardNumberString = CUtilities::ToHexString(reinterpret_cast<const BYTE *>(cardNumber),
						sizeof(TEST_DATA[testIndex].m_cardNumberExpected));
					std::string cardNumberExpectedString =
						CUtilities::ToHexString(reinterpret_cast<const BYTE *>(TEST_DATA[testIndex].m_cardNumberExpected),
							sizeof(TEST_DATA[testIndex].m_cardNumberExpected));
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED cardNumber={%s} expected={%s} testIndex=%d in %s"),
						static_cast<LPCTSTR>(cardNumberString.c_str()),
						static_cast<LPCTSTR>(cardNumberExpectedString.c_str()),
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #3.
				{
					const PBError errorCode = pbInfo.GetErrorCode();

					bool areEqual = errorCode == TEST_DATA[testIndex].m_errorCodeExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED errorCode=%d expected=%d testIndex=%d in %s"),
						(int)errorCode,
						(int)TEST_DATA[testIndex].m_errorCodeExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #4.
				{
					const DWORD cashBalance = pbInfo.GetCashBalance();
	
					bool areEqual = cashBalance == TEST_DATA[testIndex].m_cashBalanceExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED cashBalance=%u expected=%u testIndex=%d in %s"),
						(unsigned)cashBalance,
						(unsigned)TEST_DATA[testIndex].m_cashBalanceExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #5.
				{
					const DWORD pbFlags = pbInfo.GetPBFlags();

					bool areEqual = pbFlags == TEST_DATA[testIndex].m_pbFlagsExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED pbFlags=%u expected=%u testIndex=%d in %s"),
						(unsigned)pbFlags,
						(unsigned)TEST_DATA[testIndex].m_pbFlagsExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}


				// Test #6.
				{
					const DWORD promo2Balance = pbInfo.GetPromo2Balance();

					bool areEqual = promo2Balance == TEST_DATA[testIndex].m_promo2BalanceExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED promo2Balance=%u expected=%u testIndex=%d in %s"),
						(unsigned)promo2Balance,
						(unsigned)TEST_DATA[testIndex].m_promo2BalanceExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #7.
				{
					const DWORD pointsAvailable = pbInfo.GetPointsAvailable();

					bool areEqual = pointsAvailable == TEST_DATA[testIndex].m_pointsAvailableExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED pointsAvailable=%u expected=%u testIndex=%d in %s"),
						(unsigned)pointsAvailable,
						(unsigned)TEST_DATA[testIndex].m_pointsAvailableExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #8.
				{
					const DWORD pointsPerDollar = pbInfo.GetPointsPerDollar();

					bool areEqual = pointsPerDollar == TEST_DATA[testIndex].m_pointsPerDollarExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED pointsPerDollar=%u expected=%u testIndex=%d in %s"),
						(unsigned)pointsPerDollar,
						(unsigned)TEST_DATA[testIndex].m_pointsPerDollarExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #9.
				{
					const DWORD sequenceNumber = pbInfo.GetSequenceNumber();

					bool areEqual = sequenceNumber == TEST_DATA[testIndex].m_sequenceNumberExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED sequenceNumber=%u expected=%u testIndex=%d in %s"),
						(unsigned)sequenceNumber,
						(unsigned)TEST_DATA[testIndex].m_sequenceNumberExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #10.
				{
					const DWORD minimumPoints = pbInfo.GetMinimumPoints();

					bool areEqual = minimumPoints == TEST_DATA[testIndex].m_minimumPointsExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED minimumPoints=%u expected=%u testIndex=%d in %s"),
						(unsigned)minimumPoints,
						(unsigned)TEST_DATA[testIndex].m_minimumPointsExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #11.
				{
					const DWORD maximumPoints = pbInfo.GetMaximumPoints();

					bool areEqual = maximumPoints == TEST_DATA[testIndex].m_maximumPointsExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED maximumPoints=%u expected=%u testIndex=%d in %s"),
						(unsigned)maximumPoints,
						(unsigned)TEST_DATA[testIndex].m_maximumPointsExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #12.
				{
					const DWORD minimumCashDown = pbInfo.GetMinimumCashDown();

					bool areEqual = minimumCashDown == TEST_DATA[testIndex].m_minimumCashDownExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED minimumCashDown=%u expected=%u testIndex=%d in %s"),
						(unsigned)minimumCashDown,
						(unsigned)TEST_DATA[testIndex].m_minimumCashDownExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #13.
				{
					const DWORD maximumCashDown = pbInfo.GetMaximumCashDown();

					bool areEqual = maximumCashDown == TEST_DATA[testIndex].m_maximumCashDownExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED maximumCashDown=%u expected=%u testIndex=%d in %s"),
						(unsigned)maximumCashDown,
						(unsigned)TEST_DATA[testIndex].m_maximumCashDownExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #14.
				{
					const DWORD minimumCashUp = pbInfo.GetMinimumCashUp();

					bool areEqual = minimumCashUp == TEST_DATA[testIndex].m_minimumCashUpExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED minimumCashUp=%u expected=%u testIndex=%d in %s"),
						(unsigned)minimumCashUp,
						(unsigned)TEST_DATA[testIndex].m_minimumCashUpExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #15.
				{
					const DWORD maximumCashUp = pbInfo.GetMaximumCashUp();

					bool areEqual = maximumCashUp == TEST_DATA[testIndex].m_maximumCashUpExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED maximumCashUp=%u expected=%u testIndex=%d in %s"),
						(unsigned)maximumCashUp,
						(unsigned)TEST_DATA[testIndex].m_maximumCashUpExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #16.
				{
					const DWORD minimumPromoDown = pbInfo.GetMinimumPromoDown();

					bool areEqual = minimumPromoDown == TEST_DATA[testIndex].m_minimumPromoDownExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED minimumPromoDown=%u expected=%u testIndex=%d in %s"),
						(unsigned)minimumPromoDown,
						(unsigned)TEST_DATA[testIndex].m_minimumPromoDownExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #17.
				{
					const DWORD maximumPromoDown = pbInfo.GetMaximumPromoDown();

					bool areEqual = maximumPromoDown == TEST_DATA[testIndex].m_maximumPromoDownExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED maximumPromoDown=%u expected=%u testIndex=%d in %s"),
						(unsigned)maximumPromoDown,
						(unsigned)TEST_DATA[testIndex].m_maximumPromoDownExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #18.
				{
					const DWORD minimumPromoUp = pbInfo.GetMinimumPromoUp();

					bool areEqual = minimumPromoUp == TEST_DATA[testIndex].m_minimumPromoUpExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED minimumPromoUp=%u expected=%u testIndex=%d in %s"),
						(unsigned)minimumPromoUp,
						(unsigned)TEST_DATA[testIndex].m_minimumPromoUpExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #19.
				{
					const DWORD maximumPromoUp = pbInfo.GetMaximumPromoUp();

					bool areEqual = maximumPromoUp == TEST_DATA[testIndex].m_maximumPromoUpExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED maximumPromoUp=%u expected=%u testIndex=%d in %s"),
						(unsigned)maximumPromoUp,
						(unsigned)TEST_DATA[testIndex].m_maximumPromoUpExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #20.
				{
					const DWORD promoBalanceOnGame = pbInfo.GetPromoBalanceOnGame();

					bool areEqual = promoBalanceOnGame == TEST_DATA[testIndex].m_promoBalanceOnGameExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED promoBalanceOnGame=%u expected=%u testIndex=%d in %s"),
						(unsigned)promoBalanceOnGame,
						(unsigned)TEST_DATA[testIndex].m_promoBalanceOnGameExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #21.
				{
					const DWORD cashBalanceOnGame = pbInfo.GetCashBalanceOnGame();

					bool areEqual = cashBalanceOnGame == TEST_DATA[testIndex].m_cashBalanceOnGameExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED cashBalanceOnGame=%u expected=%u testIndex=%d in %s"),
						(unsigned)cashBalanceOnGame,
						(unsigned)TEST_DATA[testIndex].m_cashBalanceOnGameExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #22.
				{
					const int PBIDLength = pbInfo.GetPBIDLength();

					bool areEqual = PBIDLength == TEST_DATA[testIndex].m_PBIDLengthExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED PBIDLength=%d expected=%d testIndex=%d in %s"),
						(int)PBIDLength,
						(int)TEST_DATA[testIndex].m_PBIDLengthExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #23.
				const char * const PBTID = pbInfo.GetPersonalBankerID();

				{

					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED pbID=%p expected=NOT NULL testIndex=%d in %s"),
						(const void *)PBTID,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_NE(PBTID,nullptr) << failedMsg;
				}

				// Test #24.

				{
					bool areEqual = 0 == memcmp(PBTID,
						TEST_DATA[testIndex].m_PBTIDExpected,
						TEST_DATA[testIndex].m_PBIDLengthExpected);

					std::string PBTIDString = CUtilities::ToHexString(reinterpret_cast<const BYTE *>(PBTID), TEST_DATA[testIndex].m_PBIDLengthExpected);
					std::string PBTIDExpectedString = CUtilities::ToHexString(reinterpret_cast<const BYTE *>(TEST_DATA[testIndex].m_PBTIDExpected), TEST_DATA[testIndex].m_PBIDLengthExpected);
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED pbID={%s} expected={%s} in %s"),
						static_cast<LPCTSTR>(PBTIDString.c_str()),
						static_cast<LPCTSTR>(PBTIDExpectedString.c_str()),
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #25.
				{
					const bool bTransferCashToGameEnabled = pbInfo.IsTransferCashToGameEnabled();

					bool areEqual = bTransferCashToGameEnabled == TEST_DATA[testIndex].m_bTransferCashToGameEnabledExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED bTransferCashToGameEnabled=%d expected=%d testIndex=%d in %s"),
						(int)bTransferCashToGameEnabled,
						(int)TEST_DATA[testIndex].m_bTransferCashToGameEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #26.
				{
					const bool bTransferPointsToGameEnabled = pbInfo.IsTransferPointsToGameEnabled();

					bool areEqual = bTransferPointsToGameEnabled == TEST_DATA[testIndex].m_bTransferPointsToGameEnabledExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED bTransferPointsToGameEnabled=%d expected=%d testIndex=%d (%d %d %d %d %d %d) in %s"),
						(int)bTransferPointsToGameEnabled,
						(int)TEST_DATA[testIndex].m_bTransferPointsToGameEnabledExpected,
						(int)testIndex,
						(int)TEST_DATA[testIndex].m_bTransferPointsToGameEnabled,
						(int)config.PBTPointsEnabledGlobal(),
						(int)config.PBTPointsEnabled(),
						(int)bIsEFT,
						(int)config.PBTPointsAsPromo(),
						(int)pbInfo.IsTransferPointsToGameEnabled(),
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #27.
				{
					const bool bTransferPromoDollarsToGameEnabled = pbInfo.IsTransferPromoDollarsToGameEnabled();

					bool areEqual = bTransferPromoDollarsToGameEnabled == TEST_DATA[testIndex].m_bTransferPromoDollarsToGameEnabledExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED bTransferPromoDollarsToGameEnabled=%d expected=%d testIndex=%d in %s"),
						(int)bTransferPromoDollarsToGameEnabled,
						(int)TEST_DATA[testIndex].m_bTransferPromoDollarsToGameEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #28.
				{
					const bool bTransferCashFromGameEnabled = pbInfo.IsTransferCashFromGameEnabled();

					bool areEqual = bTransferCashFromGameEnabled == TEST_DATA[testIndex].m_bTransferCashFromGameEnabledExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED bTransferCashFromGameEnabled=%d expected=%d testIndex=%d in %s"),
						(int)bTransferCashFromGameEnabled,
						(int)TEST_DATA[testIndex].m_bTransferCashFromGameEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #29.
				{
					const bool bGameDenomMatches = pbInfo.DoesGameDenomMatch();

					bool areEqual = bGameDenomMatches == TEST_DATA[testIndex].m_bGameDenomMatchesExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED bGameDenomMatches=%d expected=%d testIndex=%d in %s"),
						(int)bGameDenomMatches,
						(int)TEST_DATA[testIndex].m_bGameDenomMatchesExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #30.
				{
					const bool bTransferPromoFromGameEnabled = pbInfo.IsTransferPromoFromGameEnabled();

					bool areEqual = bTransferPromoFromGameEnabled == TEST_DATA[testIndex].m_bTransferPromoFromGameEnabledExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED bTransferPromoFromGameEnabled=%d expected=%d testIndex=%d in %s"),
						(int)bTransferPromoFromGameEnabled,
						(int)TEST_DATA[testIndex].m_bTransferPromoFromGameEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #31.
				{
					const bool bTransferToGameEnabled = pbInfo.IsTransferToGameEnabled();

					bool areEqual = bTransferToGameEnabled == TEST_DATA[testIndex].m_bTransferToGameEnabledExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED bTransferToGameEnabled=%d expected=%d testIndex=%d (%d %d %d %d) in %s"),
						(int)bTransferToGameEnabled,
						(int)TEST_DATA[testIndex].m_bTransferToGameEnabledExpected,
						(int)testIndex,
						(int)TEST_DATA[testIndex].m_bTransferCashToGameEnabledExpected,
						(int)TEST_DATA[testIndex].m_bTransferPointsToGameEnabledExpected,
						(int)TEST_DATA[testIndex].m_bTransferPromoDollarsToGameEnabledExpected,
						(int)TEST_DATA[testIndex].m_bEGMInitialized,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBankerInfo::CPersonalBankerInfo(MEMORY_NVRAM, false)
		///     CPersonalBankerInfo::StoreToNVRAM()
		///     CPersonalBankerInfo::BuildYourself()
		///     CPersonalBankerInfo::CPersonalBankerInfo(MEMORY_NVRAM, true)
		/// </summary>
        TEST_F(PersonalBankerInfoUnitTests,CPersonalBankerInfoNvram2Test)
		{
			typedef struct
			{
				// Inputs.
				const MEMORY_TYPE m_type;

				const byte m_cardNumber[CARD_ID_LEN];
				PBError m_errorCode;

				const DWORD m_cashBalance;
				const DWORD m_pbFlags;
				const DWORD m_promo2Balance;
				const DWORD m_pointsAvailable;
				const DWORD m_pointsPerDollar;
				const DWORD m_sequenceNumber;
				const DWORD m_minimumPoints;
				const DWORD m_maximumPoints;
				const DWORD m_minimumCashDown;
				const DWORD m_maximumCashDown;
				const DWORD m_minimumCashUp;
				const DWORD m_maximumCashUp;
				const DWORD m_minimumPromoDown;
				const DWORD m_maximumPromoDown;
				const DWORD m_minimumPromoUp;
				const DWORD m_maximumPromoUp;
				const DWORD m_promoBalanceOnGame;
				const DWORD m_cashBalanceOnGame;

				const int m_PBIDLength;
				const char m_PBTID[MAX_PBID_LENGTH];

				const bool m_bTransferCashToGameEnabled;
				const bool m_bTransferPointsToGameEnabled;
				const bool m_bTransferPromoDollarsToGameEnabled;
				const bool m_bTransferCashFromGameEnabled;
				const bool m_bGameDenomMatches;
				const bool m_bEGMInitialized;
				const bool m_bTransferPromoFromGameEnabled;
				const bool m_bTransferPromoFromGameAutoEnabled;

				// Expected results.
				const MEMORY_TYPE m_typeExpected;

				const byte m_cardNumberExpected[CARD_ID_LEN];
				PBError m_errorCodeExpected;

				const DWORD m_cashBalanceExpected;
				const DWORD m_pbFlagsExpected;
				const DWORD m_promo2BalanceExpected;
				const DWORD m_pointsAvailableExpected;
				const DWORD m_pointsPerDollarExpected;
				const DWORD m_sequenceNumberExpected;
				const DWORD m_minimumPointsExpected;
				const DWORD m_maximumPointsExpected;
				const DWORD m_minimumCashDownExpected;
				const DWORD m_maximumCashDownExpected;
				const DWORD m_minimumCashUpExpected;
				const DWORD m_maximumCashUpExpected;
				const DWORD m_minimumPromoDownExpected;
				const DWORD m_maximumPromoDownExpected;
				const DWORD m_minimumPromoUpExpected;
				const DWORD m_maximumPromoUpExpected;
				const DWORD m_promoBalanceOnGameExpected;
				const DWORD m_cashBalanceOnGameExpected;

				const int m_PBIDLengthExpected;
				const char m_PBTIDExpected[MAX_PBID_LENGTH];

				const bool m_bTransferCashToGameEnabledExpected;
				const bool m_bTransferPointsToGameEnabledExpected;
				const bool m_bTransferPromoDollarsToGameEnabledExpected;
				const bool m_bTransferCashFromGameEnabledExpected;
				const bool m_bGameDenomMatchesExpected;
				// const bool m_bEGMInitializedExpected; // 2013-06-18 Not directly accessible.
				const bool m_bTransferPromoFromGameEnabledExpected;
				// const bool m_bTransferPromoFromGameAutoEnabled; // 2013-06-18 Not publicly accessible.
				const bool m_bTransferToGameEnabledExpected; // Test m_bEGMInitialized indirectly.
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ MEMORY_NVRAM,
					{ 1, 2, 3, 4, 5 }, AccountLocked,
					1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
					20, { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
					true, true, true, true, false, false, true, true,
					MEMORY_NVRAM,
					{ 1, 2, 3, 4, 5 }, AccountLocked,
					1, 2, 3, 4, 5, 6, 7, 4, 9, 0, 11, 12, 13, 0, 15, 16, 0, 0,
					20, { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
					false, false, false, false, true, false, false
				},
			};

			// More test data - expected results that are the same for each iteration.
			const bool bIsEFT(true);

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				//Setup
				CMemoryStorage::SetSprinkles();
				CPersonalBankerInfo pbInfo1(TEST_DATA[testIndex].m_type, false);
				pbInfo1.SetCardNumber(TEST_DATA[testIndex].m_cardNumber);
				pbInfo1.SetErrorCode(TEST_DATA[testIndex].m_errorCode);
				pbInfo1.SetCashBalance(TEST_DATA[testIndex].m_cashBalance);
				pbInfo1.SetPBFlags(TEST_DATA[testIndex].m_pbFlags);
				pbInfo1.SetPromo2Balance(TEST_DATA[testIndex].m_promo2Balance);
				pbInfo1.SetPointsAvailable(TEST_DATA[testIndex].m_pointsAvailable);
				pbInfo1.SetPointsPerDollar(TEST_DATA[testIndex].m_pointsPerDollar);
				pbInfo1.SetSequenceNumber(TEST_DATA[testIndex].m_sequenceNumber);
				pbInfo1.SetMinimumPoints(TEST_DATA[testIndex].m_minimumPoints);
				pbInfo1.SetMaximumPoints(TEST_DATA[testIndex].m_maximumPoints);
				pbInfo1.SetMinimumCashDown(TEST_DATA[testIndex].m_minimumCashDown);
				pbInfo1.SetMaximumCashDown(TEST_DATA[testIndex].m_maximumCashDown);
				pbInfo1.SetMinimumCashUp(TEST_DATA[testIndex].m_minimumCashUp);
				pbInfo1.SetMaximumCashUp(TEST_DATA[testIndex].m_maximumCashUp);
				pbInfo1.SetMinimumPromoDown(TEST_DATA[testIndex].m_minimumPromoDown);
				pbInfo1.SetMaximumPromoDown(TEST_DATA[testIndex].m_maximumPromoDown);
				pbInfo1.SetMinimumPromoUp(TEST_DATA[testIndex].m_minimumPromoUp);
				pbInfo1.SetMaximumPromoUp(TEST_DATA[testIndex].m_maximumPromoUp);
				pbInfo1.SetPromoBalanceOnGame(TEST_DATA[testIndex].m_promoBalanceOnGame);
				pbInfo1.SetCashBalanceOnGame(TEST_DATA[testIndex].m_cashBalanceOnGame);
				pbInfo1.SetCashBalanceOnGame(TEST_DATA[testIndex].m_cashBalanceOnGame);

				pbInfo1.SetPersonalBankerID(TEST_DATA[testIndex].m_PBTID, TEST_DATA[testIndex].m_PBIDLength);

				CConfig config(false, "");
				config.SetPBTCashEnabled_Global(TEST_DATA[testIndex].m_bTransferCashToGameEnabled);
				config.SetPBTCashEnabled_local(TEST_DATA[testIndex].m_bTransferCashToGameEnabled);
				config.SetPBTPointsEnabled_Global(TEST_DATA[testIndex].m_bTransferPointsToGameEnabled);
				config.SetPBTPointsAsPromo(TEST_DATA[testIndex].m_bTransferPointsToGameEnabled);
				config.SetPBTPromoEnabled_Global(TEST_DATA[testIndex].m_bTransferPromoDollarsToGameEnabled);
				config.SetPBTPromoEnabled_local(TEST_DATA[testIndex].m_bTransferPromoDollarsToGameEnabled);
				config.SetPBFromGameEnabled(TEST_DATA[testIndex].m_bTransferCashFromGameEnabled);
				config.SetGameDenom(1);
				config.SetDenomination(TEST_DATA[testIndex].m_bGameDenomMatches ? 1 : 5);
				if (TEST_DATA[testIndex].m_bTransferPromoFromGameEnabled)
				{
					config.SetSenOption2Raw(config.GetSenOption2Raw() + SO2_NCEP_OUT_ENABLED);
				}
				if (TEST_DATA[testIndex].m_bTransferPromoFromGameAutoEnabled)
				{
					config.SetSenOption2Raw(config.GetSenOption2Raw() + SO2_NCEP_AUTO_UPLOAD);
				}
				pbInfo1.SetConfigData(config, TEST_DATA[testIndex].m_bEGMInitialized, bIsEFT);

				// Perform operations to be tested.
				CPersonalBankerInfo pbInfo2(TEST_DATA[testIndex].m_type, false);
				pbInfo2 = pbInfo1;
				CPersonalBankerInfo pbInfo(TEST_DATA[testIndex].m_type, true);

				// Test #A.
				{
					const MEMORY_TYPE type = pbInfo.GetMemoryType();

					bool areEqual = type == TEST_DATA[testIndex].m_typeExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED type=%d expected=%d testIndex=%d in %s"),
						(int)type,
						(int)TEST_DATA[testIndex].m_typeExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #1.
				const byte * const cardNumber = pbInfo.GetCardNumber();

				{

					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED cardNumber=%p expected=NOT NULL testIndex=%d in %s"),
						(const void *)cardNumber,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_NE(cardNumber, nullptr) << failedMsg;
				}

				// Test #2.

				{
					bool areEqual = 0 == memcmp(cardNumber,
						TEST_DATA[testIndex].m_cardNumberExpected,
						sizeof(TEST_DATA[testIndex].m_cardNumberExpected));

					std::string cardNumberString = CUtilities::ToHexString(reinterpret_cast<const BYTE *>(cardNumber),
						sizeof(TEST_DATA[testIndex].m_cardNumberExpected));
					std::string cardNumberExpectedString =
						CUtilities::ToHexString(reinterpret_cast<const BYTE *>(TEST_DATA[testIndex].m_cardNumberExpected),
							sizeof(TEST_DATA[testIndex].m_cardNumberExpected));
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED cardNumber={%s} expected={%s} testIndex=%d in %s"),
						static_cast<LPCTSTR>(cardNumberString.c_str()),
						static_cast<LPCTSTR>(cardNumberExpectedString.c_str()),
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #3.
				{
					const PBError errorCode = pbInfo.GetErrorCode();

					bool areEqual = errorCode == TEST_DATA[testIndex].m_errorCodeExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED errorCode=%d expected=%d testIndex=%d in %s"),
						(int)errorCode,
						(int)TEST_DATA[testIndex].m_errorCodeExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #4.
				{
					const DWORD cashBalance = pbInfo.GetCashBalance();
	
					bool areEqual = cashBalance == TEST_DATA[testIndex].m_cashBalanceExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED cashBalance=%u expected=%u testIndex=%d in %s"),
						(unsigned)cashBalance,
						(unsigned)TEST_DATA[testIndex].m_cashBalanceExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #5.
				{
					const DWORD pbFlags = pbInfo.GetPBFlags();

					bool areEqual = pbFlags == TEST_DATA[testIndex].m_pbFlagsExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED pbFlags=%u expected=%u testIndex=%d in %s"),
						(unsigned)pbFlags,
						(unsigned)TEST_DATA[testIndex].m_pbFlagsExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}


				// Test #6.
				{
					const DWORD promo2Balance = pbInfo.GetPromo2Balance();

					bool areEqual = promo2Balance == TEST_DATA[testIndex].m_promo2BalanceExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED promo2Balance=%u expected=%u testIndex=%d in %s"),
						(unsigned)promo2Balance,
						(unsigned)TEST_DATA[testIndex].m_promo2BalanceExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #7.
				{
					const DWORD pointsAvailable = pbInfo.GetPointsAvailable();

					bool areEqual = pointsAvailable == TEST_DATA[testIndex].m_pointsAvailableExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED pointsAvailable=%u expected=%u testIndex=%d in %s"),
						(unsigned)pointsAvailable,
						(unsigned)TEST_DATA[testIndex].m_pointsAvailableExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #8.
				{
					const DWORD pointsPerDollar = pbInfo.GetPointsPerDollar();

					bool areEqual = pointsPerDollar == TEST_DATA[testIndex].m_pointsPerDollarExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED pointsPerDollar=%u expected=%u testIndex=%d in %s"),
						(unsigned)pointsPerDollar,
						(unsigned)TEST_DATA[testIndex].m_pointsPerDollarExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #9.
				{
					const DWORD sequenceNumber = pbInfo.GetSequenceNumber();

					bool areEqual = sequenceNumber == TEST_DATA[testIndex].m_sequenceNumberExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED sequenceNumber=%u expected=%u testIndex=%d in %s"),
						(unsigned)sequenceNumber,
						(unsigned)TEST_DATA[testIndex].m_sequenceNumberExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #10.
				{
					const DWORD minimumPoints = pbInfo.GetMinimumPoints();

					bool areEqual = minimumPoints == TEST_DATA[testIndex].m_minimumPointsExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED minimumPoints=%u expected=%u testIndex=%d in %s"),
						(unsigned)minimumPoints,
						(unsigned)TEST_DATA[testIndex].m_minimumPointsExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #11.
				{
					const DWORD maximumPoints = pbInfo.GetMaximumPoints();

					bool areEqual = maximumPoints == TEST_DATA[testIndex].m_maximumPointsExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED maximumPoints=%u expected=%u testIndex=%d in %s"),
						(unsigned)maximumPoints,
						(unsigned)TEST_DATA[testIndex].m_maximumPointsExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #12.
				{
					const DWORD minimumCashDown = pbInfo.GetMinimumCashDown();

					bool areEqual = minimumCashDown == TEST_DATA[testIndex].m_minimumCashDownExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED minimumCashDown=%u expected=%u testIndex=%d in %s"),
						(unsigned)minimumCashDown,
						(unsigned)TEST_DATA[testIndex].m_minimumCashDownExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #13.
				{
					const DWORD maximumCashDown = pbInfo.GetMaximumCashDown();

					bool areEqual = maximumCashDown == TEST_DATA[testIndex].m_maximumCashDownExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED maximumCashDown=%u expected=%u testIndex=%d in %s"),
						(unsigned)maximumCashDown,
						(unsigned)TEST_DATA[testIndex].m_maximumCashDownExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #14.
				{
					const DWORD minimumCashUp = pbInfo.GetMinimumCashUp();

					bool areEqual = minimumCashUp == TEST_DATA[testIndex].m_minimumCashUpExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED minimumCashUp=%u expected=%u testIndex=%d in %s"),
						(unsigned)minimumCashUp,
						(unsigned)TEST_DATA[testIndex].m_minimumCashUpExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #15.
				{
					const DWORD maximumCashUp = pbInfo.GetMaximumCashUp();

					bool areEqual = maximumCashUp == TEST_DATA[testIndex].m_maximumCashUpExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED maximumCashUp=%u expected=%u testIndex=%d in %s"),
						(unsigned)maximumCashUp,
						(unsigned)TEST_DATA[testIndex].m_maximumCashUpExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #16.
				{
					const DWORD minimumPromoDown = pbInfo.GetMinimumPromoDown();

					bool areEqual = minimumPromoDown == TEST_DATA[testIndex].m_minimumPromoDownExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED minimumPromoDown=%u expected=%u testIndex=%d in %s"),
						(unsigned)minimumPromoDown,
						(unsigned)TEST_DATA[testIndex].m_minimumPromoDownExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #17.
				{
					const DWORD maximumPromoDown = pbInfo.GetMaximumPromoDown();

					bool areEqual = maximumPromoDown == TEST_DATA[testIndex].m_maximumPromoDownExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED maximumPromoDown=%u expected=%u testIndex=%d in %s"),
						(unsigned)maximumPromoDown,
						(unsigned)TEST_DATA[testIndex].m_maximumPromoDownExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #18.
				{
					const DWORD minimumPromoUp = pbInfo.GetMinimumPromoUp();

					bool areEqual = minimumPromoUp == TEST_DATA[testIndex].m_minimumPromoUpExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED minimumPromoUp=%u expected=%u testIndex=%d in %s"),
						(unsigned)minimumPromoUp,
						(unsigned)TEST_DATA[testIndex].m_minimumPromoUpExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #19.
				{
					const DWORD maximumPromoUp = pbInfo.GetMaximumPromoUp();

					bool areEqual = maximumPromoUp == TEST_DATA[testIndex].m_maximumPromoUpExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED maximumPromoUp=%u expected=%u testIndex=%d in %s"),
						(unsigned)maximumPromoUp,
						(unsigned)TEST_DATA[testIndex].m_maximumPromoUpExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #20.
				{
					const DWORD promoBalanceOnGame = pbInfo.GetPromoBalanceOnGame();

					bool areEqual = promoBalanceOnGame == TEST_DATA[testIndex].m_promoBalanceOnGameExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED promoBalanceOnGame=%u expected=%u testIndex=%d in %s"),
						(unsigned)promoBalanceOnGame,
						(unsigned)TEST_DATA[testIndex].m_promoBalanceOnGameExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #21.
				{
					const DWORD cashBalanceOnGame = pbInfo.GetCashBalanceOnGame();

					bool areEqual = cashBalanceOnGame == TEST_DATA[testIndex].m_cashBalanceOnGameExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED cashBalanceOnGame=%u expected=%u testIndex=%d in %s"),
						(unsigned)cashBalanceOnGame,
						(unsigned)TEST_DATA[testIndex].m_cashBalanceOnGameExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #22.
				{
					const int PBIDLength = pbInfo.GetPBIDLength();

					bool areEqual = PBIDLength == TEST_DATA[testIndex].m_PBIDLengthExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED PBIDLength=%d expected=%d testIndex=%d in %s"),
						(int)PBIDLength,
						(int)TEST_DATA[testIndex].m_PBIDLengthExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #23.
				const char * const PBTID = pbInfo.GetPersonalBankerID();

				{

					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED pbID=%p expected=NOT NULL testIndex=%d in %s"),
						(const void *)PBTID,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_NE(PBTID,nullptr) << failedMsg;
				}

				// Test #24.

				{
					bool areEqual = 0 == memcmp(PBTID,
						TEST_DATA[testIndex].m_PBTIDExpected,
						TEST_DATA[testIndex].m_PBIDLengthExpected);

					std::string PBTIDString = CUtilities::ToHexString(reinterpret_cast<const BYTE *>(PBTID), TEST_DATA[testIndex].m_PBIDLengthExpected);
					std::string PBTIDExpectedString = CUtilities::ToHexString(reinterpret_cast<const BYTE *>(TEST_DATA[testIndex].m_PBTIDExpected), TEST_DATA[testIndex].m_PBIDLengthExpected);
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED pbID={%s} expected={%s} in %s"),
						static_cast<LPCTSTR>(PBTIDString.c_str()),
						static_cast<LPCTSTR>(PBTIDExpectedString.c_str()),
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #25.
				{
					const bool bTransferCashToGameEnabled = pbInfo.IsTransferCashToGameEnabled();

					bool areEqual = bTransferCashToGameEnabled == TEST_DATA[testIndex].m_bTransferCashToGameEnabledExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED bTransferCashToGameEnabled=%d expected=%d testIndex=%d in %s"),
						(int)bTransferCashToGameEnabled,
						(int)TEST_DATA[testIndex].m_bTransferCashToGameEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #26.
				{
					const bool bTransferPointsToGameEnabled = pbInfo.IsTransferPointsToGameEnabled();

					bool areEqual = bTransferPointsToGameEnabled == TEST_DATA[testIndex].m_bTransferPointsToGameEnabledExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED bTransferPointsToGameEnabled=%d expected=%d testIndex=%d (%d %d %d %d %d %d) in %s"),
						(int)bTransferPointsToGameEnabled,
						(int)TEST_DATA[testIndex].m_bTransferPointsToGameEnabledExpected,
						(int)testIndex,
						(int)TEST_DATA[testIndex].m_bTransferPointsToGameEnabled,
						(int)config.PBTPointsEnabledGlobal(),
						(int)config.PBTPointsEnabled(),
						(int)bIsEFT,
						(int)config.PBTPointsAsPromo(),
						(int)pbInfo.IsTransferPointsToGameEnabled(),
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #27.
				{
					const bool bTransferPromoDollarsToGameEnabled = pbInfo.IsTransferPromoDollarsToGameEnabled();

					bool areEqual = bTransferPromoDollarsToGameEnabled == TEST_DATA[testIndex].m_bTransferPromoDollarsToGameEnabledExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED bTransferPromoDollarsToGameEnabled=%d expected=%d testIndex=%d in %s"),
						(int)bTransferPromoDollarsToGameEnabled,
						(int)TEST_DATA[testIndex].m_bTransferPromoDollarsToGameEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #28.
				{
					const bool bTransferCashFromGameEnabled = pbInfo.IsTransferCashFromGameEnabled();

					bool areEqual = bTransferCashFromGameEnabled == TEST_DATA[testIndex].m_bTransferCashFromGameEnabledExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED bTransferCashFromGameEnabled=%d expected=%d testIndex=%d in %s"),
						(int)bTransferCashFromGameEnabled,
						(int)TEST_DATA[testIndex].m_bTransferCashFromGameEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #29.
				{
					const bool bGameDenomMatches = pbInfo.DoesGameDenomMatch();

					bool areEqual = bGameDenomMatches == TEST_DATA[testIndex].m_bGameDenomMatchesExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED bGameDenomMatches=%d expected=%d testIndex=%d in %s"),
						(int)bGameDenomMatches,
						(int)TEST_DATA[testIndex].m_bGameDenomMatchesExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #30.
				{
					const bool bTransferPromoFromGameEnabled = pbInfo.IsTransferPromoFromGameEnabled();

					bool areEqual = bTransferPromoFromGameEnabled == TEST_DATA[testIndex].m_bTransferPromoFromGameEnabledExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED bTransferPromoFromGameEnabled=%d expected=%d testIndex=%d in %s"),
						(int)bTransferPromoFromGameEnabled,
						(int)TEST_DATA[testIndex].m_bTransferPromoFromGameEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #31.
				{
					const bool bTransferToGameEnabled = pbInfo.IsTransferToGameEnabled();

					bool areEqual = bTransferToGameEnabled == TEST_DATA[testIndex].m_bTransferToGameEnabledExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED bTransferToGameEnabled=%d expected=%d testIndex=%d (%d %d %d %d) in %s"),
						(int)bTransferToGameEnabled,
						(int)TEST_DATA[testIndex].m_bTransferToGameEnabledExpected,
						(int)testIndex,
						(int)TEST_DATA[testIndex].m_bTransferCashToGameEnabledExpected,
						(int)TEST_DATA[testIndex].m_bTransferPointsToGameEnabledExpected,
						(int)TEST_DATA[testIndex].m_bTransferPromoDollarsToGameEnabledExpected,
						(int)TEST_DATA[testIndex].m_bEGMInitialized,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBankerInfo::BuildYourself()
		/// </summary>
        TEST_F(PersonalBankerInfoUnitTests,BuildYourselfTest)
		{
			// Input(s).
			const DWORD nvramDataOffset(NVRAM_PB_INFO_OFFSET + NVRAM_PB_INFO_CHECKBYTE_SIZE);
			const DWORD nvramDataLength(NVRAM_PB_INFO_SIZE - NVRAM_PB_INFO_CHECKBYTE_SIZE);

			typedef struct
			{
				// Input(s).
				const BYTE m_nvramData[nvramDataLength];

				// Expected result(s).
				const MEMORY_TYPE m_typeExpected;

				const byte m_cardNumberExpected[CARD_ID_LEN];
				PBError m_errorCodeExpected;

				const DWORD m_cashBalanceExpected;
				const DWORD m_pbFlagsExpected;
				const DWORD m_promo2BalanceExpected;
				const DWORD m_pointsAvailableExpected;
				const DWORD m_pointsPerDollarExpected;
				const DWORD m_sequenceNumberExpected;
				const DWORD m_minimumPointsExpected;
				const DWORD m_maximumPointsExpected;
				const DWORD m_minimumCashDownExpected;
				const DWORD m_maximumCashDownExpected;
				const DWORD m_minimumCashUpExpected;
				const DWORD m_maximumCashUpExpected;
				const DWORD m_minimumPromoDownExpected;
				const DWORD m_maximumPromoDownExpected;
				const DWORD m_minimumPromoUpExpected;
				const DWORD m_maximumPromoUpExpected;
				const DWORD m_promoBalanceOnGameExpected;
				const DWORD m_cashBalanceOnGameExpected;

				const int m_PBIDLengthExpected;
				const char m_PBTIDExpected[MAX_PBID_LENGTH];

				const bool m_bTransferCashToGameEnabledExpected;
				const bool m_bTransferPointsToGameEnabledExpected;
				const bool m_bTransferPromoDollarsToGameEnabledExpected;
				const bool m_bTransferCashFromGameEnabledExpected;
				const bool m_bGameDenomMatchesExpected;
				// const bool m_bEGMInitializedExpected; // 2013-06-18 Not directly accessible.
				const bool m_bTransferPromoFromGameEnabledExpected;
				// const bool m_bTransferPromoFromGameAutoEnabled; // 2013-06-18 Not publicly accessible.
				const bool m_bTransferToGameEnabledExpected; // Test m_bEGMInitialized indirectly.
				const bool m_externalCreditProviderEnabledExpected;
				const DWORD m_creditFeeExpected;
				const bool m_creditFeeFixedExpected;

			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				// Different values for each member.
				{
					{ 1, 2, 3, 4, 5, // cardNumber.
						3, 0, 0, 0, // errorCode.
						1, 0, 0, 0, // cashBalance.
						2, 0, 0, 0, // pbFlags.
						3, 0, 0, 0, // promo2Balance.
						4, 0, 0, 0, // pointsAvailable.
						5, 0, 0, 0, // pointsPerDollar.
						6, 0, 0, 0, // sequenceNumber.
						7, 0, 0, 0, // minimumPoints.
						8, 0, 0, 0, // maximumPoints.
						9, 0, 0, 0, // minimumCashDown.
						10, 0, 0, 0, // maximumCashDown.
						11, 0, 0, 0, // minimumCashUp.
						12, 0, 0, 0, // maximumCashUp.
						13, 0, 0, 0, // minimumPromoDown.
						14, 0, 0, 0, // maximumPromoDown.
						20, 0, 0, 0, // PBIDLength.
						1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, // PBTID[MAX_PBID_LENGTH].
						15, 0, 0, 0, // minimumPromoUp.
						16, 0, 0, 0, // maximumPromoUp.
						17, // externalCreditProviderEnabledExpected.
						18, 0, 0, 0, // creditFeeExpected.
						19, // creditFeeFixedExpected.
					},
					MEMORY_NVRAM,
					{ 1, 2, 3, 4, 5 }, AccountLocked,
					1, 2, 3, 4, 5, 6, 7, 4, 9, 0, 11, 12, 13, 0, 15, 16, 0, 0,
					20, { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
					false, false, false, false, true, false, false, false, 18, false
				},

				// Minimum (unsigned) values for each member.
				{
					{ 0, 0, 0, 0, 0, // cardNumber.
						0, 0, 0, 0, // errorCode.
						0, 0, 0, 0, // cashBalance.
						0, 0, 0, 0, // pbFlags.
						0, 0, 0, 0, // promo2Balance.
						0, 0, 0, 0, // pointsAvailable.
						0, 0, 0, 0, // pointsPerDollar.
						0, 0, 0, 0, // sequenceNumber.
						0, 0, 0, 0, // minimumPoints.
						0, 0, 0, 0, // maximumPoints.
						0, 0, 0, 0, // minimumCashDown.
						0, 0, 0, 0, // maximumCashDown.
						0, 0, 0, 0, // minimumCashUp.
						0, 0, 0, 0, // maximumCashUp.
						0, 0, 0, 0, // minimumPromoDown.
						0, 0, 0, 0, // maximumPromoDown.
						0, 0, 0, 0, // PBIDLength.
						0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // PBTID[MAX_PBID_LENGTH].
						0, 0, 0, 0, // minimumPromoUp.
						0, 0, 0, 0, // maximumPromoUp.
						0, // externalCreditProviderEnabledExpected.
						0, 0, 0, 0, // creditFeeExpected.
						0, // creditFeeFixedExpected.
					},
					MEMORY_NVRAM,
					{ 0, 0, 0, 0, 0 }, AccessDenied,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
					false, false, false, false, true, false, false, false, 0, false
				},

				// Maximum (unsigned) values for each member.
				{
					{ 0xff, 0xff, 0xff, 0xff, 0xff, // cardNumber.
						0xff, 0xff, 0xff, 0xff, // errorCode.
						0xff, 0xff, 0xff, 0xff, // cashBalance.
						0xff, 0xff, 0xff, 0xff, // pbFlags.
						0xff, 0xff, 0xff, 0xff, // promo2Balance.
						0xff, 0xff, 0xff, 0xff, // pointsAvailable.
						0xff, 0xff, 0xff, 0xff, // pointsPerDollar.
						0xff, 0xff, 0xff, 0xff, // sequenceNumber.
						0xff, 0xff, 0xff, 0xff, // minimumPoints.
						0xff, 0xff, 0xff, 0xff, // maximumPoints.
						0xff, 0xff, 0xff, 0xff, // minimumCashDown.
						0xff, 0xff, 0xff, 0xff, // maximumCashDown.
						0xff, 0xff, 0xff, 0xff, // minimumCashUp.
						0xff, 0xff, 0xff, 0xff, // maximumCashUp.
						0xff, 0xff, 0xff, 0xff, // minimumPromoDown.
						0xff, 0xff, 0xff, 0xff, // maximumPromoDown.
						0xff, 0xff, 0xff, 0xff, // PBIDLength.
						0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // PBTID[MAX_PBID_LENGTH].
						0xff, 0xff, 0xff, 0xff, // minimumPromoUp.
						0xff, 0xff, 0xff, 0xff, // maximumPromoUp.
						0xff, // externalCreditProviderEnabledExpected.
						0xff, 0xff, 0xff, 0xff, // creditFeeExpected.
						0xff, // creditFeeFixedExpected.
					},
					MEMORY_NVRAM,
					{ 0xff, 0xff, 0xff, 0xff, 0xff }, (PBError)0xffffffff,
					0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff / 100,
					0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff / 100, 0xffffffff, 0xffffffff, 0, 0,
					0, { (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff,
							(char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff },
					false, false, false, false, true, false, false, false, 0xffffffff, false
				},

				// Maximum (unsigned) values for each member, except errorCode=0x7fffffff and PBIDLength=0x7fffffff.
				{
					{ 0xff, 0xff, 0xff, 0xff, 0xff, // cardNumber.
					0xff, 0xff, 0xff, 0x7f, // errorCode.
					0xff, 0xff, 0xff, 0xff, // cashBalance.
					0xff, 0xff, 0xff, 0xff, // pbFlags.
					0xff, 0xff, 0xff, 0xff, // promo2Balance.
					0xff, 0xff, 0xff, 0xff, // pointsAvailable.
					0xff, 0xff, 0xff, 0xff, // pointsPerDollar.
					0xff, 0xff, 0xff, 0xff, // sequenceNumber.
					0xff, 0xff, 0xff, 0xff, // minimumPoints.
					0xff, 0xff, 0xff, 0xff, // maximumPoints.
					0xff, 0xff, 0xff, 0xff, // minimumCashDown.
					0xff, 0xff, 0xff, 0xff, // maximumCashDown.
					0xff, 0xff, 0xff, 0xff, // minimumCashUp.
					0xff, 0xff, 0xff, 0xff, // maximumCashUp.
					0xff, 0xff, 0xff, 0xff, // minimumPromoDown.
					0xff, 0xff, 0xff, 0xff, // maximumPromoDown.
					0xff, 0xff, 0xff, 0x7f, // PBIDLength.
					0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // PBTID[MAX_PBID_LENGTH].
					0xff, 0xff, 0xff, 0xff, // minimumPromoUp.
					0xff, 0xff, 0xff, 0xff, // maximumPromoUp.
					0xff, // externalCreditProviderEnabledExpected.
					0xff, 0xff, 0xff, 0xff, // creditFeeExpected.
					0xff, // creditFeeFixedExpected.
					},
					MEMORY_NVRAM,
					{ 0xff, 0xff, 0xff, 0xff, 0xff }, (PBError)0x7fffffff,
					0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff / 100,
					0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff / 100, 0xffffffff, 0xffffffff, 0, 0,
					20, { (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff,
					(char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff },
					false, false, false, false, true, false, false, false, 0xffffffff, false
				},

				// Maximum (unsigned) values for each member, except errorCode=0x80000000 and PBIDLength=0x80000000.
				{
					{ 0xff, 0xff, 0xff, 0xff, 0xff, // cardNumber.
					0x00, 0x00, 0x00, 0x80, // errorCode.
					0xff, 0xff, 0xff, 0xff, // cashBalance.
					0xff, 0xff, 0xff, 0xff, // pbFlags.
					0xff, 0xff, 0xff, 0xff, // promo2Balance.
					0xff, 0xff, 0xff, 0xff, // pointsAvailable.
					0xff, 0xff, 0xff, 0xff, // pointsPerDollar.
					0xff, 0xff, 0xff, 0xff, // sequenceNumber.
					0xff, 0xff, 0xff, 0xff, // minimumPoints.
					0xff, 0xff, 0xff, 0xff, // maximumPoints.
					0xff, 0xff, 0xff, 0xff, // minimumCashDown.
					0xff, 0xff, 0xff, 0xff, // maximumCashDown.
					0xff, 0xff, 0xff, 0xff, // minimumCashUp.
					0xff, 0xff, 0xff, 0xff, // maximumCashUp.
					0xff, 0xff, 0xff, 0xff, // minimumPromoDown.
					0xff, 0xff, 0xff, 0xff, // maximumPromoDown.
					0x00, 0x00, 0x00, 0x80, // PBIDLength.
					0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // PBTID[MAX_PBID_LENGTH].
					0xff, 0xff, 0xff, 0xff, // minimumPromoUp.
					0xff, 0xff, 0xff, 0xff, // maximumPromoUp.
					0xff, // externalCreditProviderEnabledExpected.
					0xff, 0xff, 0xff, 0xff, // creditFeeExpected.
					0xff, // creditFeeFixedExpected.
					},
					MEMORY_NVRAM,
					{ 0xff, 0xff, 0xff, 0xff, 0xff }, (PBError)0x80000000,
					0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff / 100,
					0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff / 100, 0xffffffff, 0xffffffff, 0, 0,
					0, { (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff,
					(char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff },
					false, false, false, false, true, false, false, false, 0xffffffff, false
				},
			};

			// More test data - expected results that are the same for each iteration.
			const bool bIsEFT(true);

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				//Setup
				CMemoryStorage::SetSprinkles();
				CPersonalBankerInfo pbInfo(MEMORY_NVRAM, false); // Set instance to default values.

				// Set NVRAM.
				CNVRAMConfig nvramConfig;
				BYTE nvramData[nvramDataLength]; // CNVRAMConfig::Set() requires a non-const READ/WRITE buffer.
				memcpy(nvramData, TEST_DATA[testIndex].m_nvramData, nvramDataLength);
				nvramConfig.Set(nvramDataOffset, nvramData, nvramDataLength);

				// Build from NVRAM.
				pbInfo.BuildYourself();

				// Test #A.
				{
					const MEMORY_TYPE type = pbInfo.GetMemoryType();

					bool areEqual = type == TEST_DATA[testIndex].m_typeExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED type=%d expected=%d testIndex=%d in %s"),
						(int)type,
						(int)TEST_DATA[testIndex].m_typeExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #1.
				const byte * const cardNumber = pbInfo.GetCardNumber();

				{

					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED cardNumber=%p expected=NOT NULL testIndex=%d in %s"),
						(const void *)cardNumber,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_NE(cardNumber, nullptr) << failedMsg;
				}

				// Test #2.

				{
					bool areEqual = 0 == memcmp(cardNumber,
						TEST_DATA[testIndex].m_cardNumberExpected,
						sizeof(TEST_DATA[testIndex].m_cardNumberExpected));

					std::string cardNumberString = CUtilities::ToHexString(reinterpret_cast<const BYTE *>(cardNumber),
						sizeof(TEST_DATA[testIndex].m_cardNumberExpected));
					std::string cardNumberExpectedString =
						CUtilities::ToHexString(reinterpret_cast<const BYTE *>(TEST_DATA[testIndex].m_cardNumberExpected),
						sizeof(TEST_DATA[testIndex].m_cardNumberExpected));
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED cardNumber={%s} expected={%s} testIndex=%d in %s"),
						static_cast<LPCTSTR>(cardNumberString.c_str()),
						static_cast<LPCTSTR>(cardNumberExpectedString.c_str()),
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #3.
				{
					const PBError errorCode = pbInfo.GetErrorCode();

					bool areEqual = errorCode == TEST_DATA[testIndex].m_errorCodeExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED errorCode=%d expected=%d testIndex=%d in %s"),
						(int)errorCode,
						(int)TEST_DATA[testIndex].m_errorCodeExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #4.
				{
					const DWORD cashBalance = pbInfo.GetCashBalance();

					bool areEqual = cashBalance == TEST_DATA[testIndex].m_cashBalanceExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED cashBalance=%u expected=%u testIndex=%d in %s"),
						(unsigned)cashBalance,
						(unsigned)TEST_DATA[testIndex].m_cashBalanceExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #5.
				{
					const DWORD pbFlags = pbInfo.GetPBFlags();

					bool areEqual = pbFlags == TEST_DATA[testIndex].m_pbFlagsExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED pbFlags=%u expected=%u testIndex=%d in %s"),
						(unsigned)pbFlags,
						(unsigned)TEST_DATA[testIndex].m_pbFlagsExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}


				// Test #6.
				{
					const DWORD promo2Balance = pbInfo.GetPromo2Balance();

					bool areEqual = promo2Balance == TEST_DATA[testIndex].m_promo2BalanceExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED promo2Balance=%u expected=%u testIndex=%d in %s"),
						(unsigned)promo2Balance,
						(unsigned)TEST_DATA[testIndex].m_promo2BalanceExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #7.
				{
					const DWORD pointsAvailable = pbInfo.GetPointsAvailable();

					bool areEqual = pointsAvailable == TEST_DATA[testIndex].m_pointsAvailableExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED pointsAvailable=%u expected=%u testIndex=%d in %s"),
						(unsigned)pointsAvailable,
						(unsigned)TEST_DATA[testIndex].m_pointsAvailableExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #8.
				{
					const DWORD pointsPerDollar = pbInfo.GetPointsPerDollar();

					bool areEqual = pointsPerDollar == TEST_DATA[testIndex].m_pointsPerDollarExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED pointsPerDollar=%u expected=%u testIndex=%d in %s"),
						(unsigned)pointsPerDollar,
						(unsigned)TEST_DATA[testIndex].m_pointsPerDollarExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #9.
				{
					const DWORD sequenceNumber = pbInfo.GetSequenceNumber();

					bool areEqual = sequenceNumber == TEST_DATA[testIndex].m_sequenceNumberExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED sequenceNumber=%u expected=%u testIndex=%d in %s"),
						(unsigned)sequenceNumber,
						(unsigned)TEST_DATA[testIndex].m_sequenceNumberExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #10.
				{
					const DWORD minimumPoints = pbInfo.GetMinimumPoints();

					bool areEqual = minimumPoints == TEST_DATA[testIndex].m_minimumPointsExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED minimumPoints=%u expected=%u testIndex=%d in %s"),
						(unsigned)minimumPoints,
						(unsigned)TEST_DATA[testIndex].m_minimumPointsExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #11.
				{
					const DWORD maximumPoints = pbInfo.GetMaximumPoints();

					bool areEqual = maximumPoints == TEST_DATA[testIndex].m_maximumPointsExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED maximumPoints=%u expected=%u testIndex=%d in %s"),
						(unsigned)maximumPoints,
						(unsigned)TEST_DATA[testIndex].m_maximumPointsExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #12.
				{
					const DWORD minimumCashDown = pbInfo.GetMinimumCashDown();

					bool areEqual = minimumCashDown == TEST_DATA[testIndex].m_minimumCashDownExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED minimumCashDown=%u expected=%u testIndex=%d in %s"),
						(unsigned)minimumCashDown,
						(unsigned)TEST_DATA[testIndex].m_minimumCashDownExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #13.
				{
					const DWORD maximumCashDown = pbInfo.GetMaximumCashDown();

					bool areEqual = maximumCashDown == TEST_DATA[testIndex].m_maximumCashDownExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED maximumCashDown=%u expected=%u testIndex=%d in %s"),
						(unsigned)maximumCashDown,
						(unsigned)TEST_DATA[testIndex].m_maximumCashDownExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #14.
				{
					const DWORD minimumCashUp = pbInfo.GetMinimumCashUp();

					bool areEqual = minimumCashUp == TEST_DATA[testIndex].m_minimumCashUpExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED minimumCashUp=%u expected=%u testIndex=%d in %s"),
						(unsigned)minimumCashUp,
						(unsigned)TEST_DATA[testIndex].m_minimumCashUpExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #15.
				{
					const DWORD maximumCashUp = pbInfo.GetMaximumCashUp();

					bool areEqual = maximumCashUp == TEST_DATA[testIndex].m_maximumCashUpExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED maximumCashUp=%u expected=%u testIndex=%d in %s"),
						(unsigned)maximumCashUp,
						(unsigned)TEST_DATA[testIndex].m_maximumCashUpExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #16.
				{
					const DWORD minimumPromoDown = pbInfo.GetMinimumPromoDown();

					bool areEqual = minimumPromoDown == TEST_DATA[testIndex].m_minimumPromoDownExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED minimumPromoDown=%u expected=%u testIndex=%d in %s"),
						(unsigned)minimumPromoDown,
						(unsigned)TEST_DATA[testIndex].m_minimumPromoDownExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #17.
				{
					const DWORD maximumPromoDown = pbInfo.GetMaximumPromoDown();

					bool areEqual = maximumPromoDown == TEST_DATA[testIndex].m_maximumPromoDownExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED maximumPromoDown=%u expected=%u testIndex=%d in %s"),
						(unsigned)maximumPromoDown,
						(unsigned)TEST_DATA[testIndex].m_maximumPromoDownExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #18.
				{
					const DWORD minimumPromoUp = pbInfo.GetMinimumPromoUp();

					bool areEqual = minimumPromoUp == TEST_DATA[testIndex].m_minimumPromoUpExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED minimumPromoUp=%u expected=%u testIndex=%d in %s"),
						(unsigned)minimumPromoUp,
						(unsigned)TEST_DATA[testIndex].m_minimumPromoUpExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #19.
				{
					const DWORD maximumPromoUp = pbInfo.GetMaximumPromoUp();

					bool areEqual = maximumPromoUp == TEST_DATA[testIndex].m_maximumPromoUpExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED maximumPromoUp=%u expected=%u testIndex=%d in %s"),
						(unsigned)maximumPromoUp,
						(unsigned)TEST_DATA[testIndex].m_maximumPromoUpExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #20.
				{
					const DWORD promoBalanceOnGame = pbInfo.GetPromoBalanceOnGame();

					bool areEqual = promoBalanceOnGame == TEST_DATA[testIndex].m_promoBalanceOnGameExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED promoBalanceOnGame=%u expected=%u testIndex=%d in %s"),
						(unsigned)promoBalanceOnGame,
						(unsigned)TEST_DATA[testIndex].m_promoBalanceOnGameExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #21.
				{
					const DWORD cashBalanceOnGame = pbInfo.GetCashBalanceOnGame();

					bool areEqual = cashBalanceOnGame == TEST_DATA[testIndex].m_cashBalanceOnGameExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED cashBalanceOnGame=%u expected=%u testIndex=%d in %s"),
						(unsigned)cashBalanceOnGame,
						(unsigned)TEST_DATA[testIndex].m_cashBalanceOnGameExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #22.
				{
					const int PBIDLength = pbInfo.GetPBIDLength();

					bool areEqual = PBIDLength == TEST_DATA[testIndex].m_PBIDLengthExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED PBIDLength=%d expected=%d testIndex=%d in %s"),
						(int)PBIDLength,
						(int)TEST_DATA[testIndex].m_PBIDLengthExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #23.
				const char * const PBTID = pbInfo.GetPersonalBankerID();

				{

					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED pbID=%p expected=NOT NULL testIndex=%d in %s"),
						(const void *)PBTID,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_NE(PBTID,nullptr) << failedMsg;
				}

				// Test #24.

				{
					bool areEqual = 0 == memcmp(PBTID,
						TEST_DATA[testIndex].m_PBTIDExpected,
						TEST_DATA[testIndex].m_PBIDLengthExpected);

					std::string PBTIDString = CUtilities::ToHexString(reinterpret_cast<const BYTE *>(PBTID), TEST_DATA[testIndex].m_PBIDLengthExpected);
					std::string PBTIDExpectedString = CUtilities::ToHexString(reinterpret_cast<const BYTE *>(TEST_DATA[testIndex].m_PBTIDExpected), TEST_DATA[testIndex].m_PBIDLengthExpected);
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED pbID={%s} expected={%s} in %s"),
						static_cast<LPCTSTR>(PBTIDString.c_str()),
						static_cast<LPCTSTR>(PBTIDExpectedString.c_str()),
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #25.
				{
					const bool bTransferCashToGameEnabled = pbInfo.IsTransferCashToGameEnabled();

					bool areEqual = bTransferCashToGameEnabled == TEST_DATA[testIndex].m_bTransferCashToGameEnabledExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED bTransferCashToGameEnabled=%d expected=%d testIndex=%d in %s"),
						(int)bTransferCashToGameEnabled,
						(int)TEST_DATA[testIndex].m_bTransferCashToGameEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #26.
				{
					const bool bTransferPointsToGameEnabled = pbInfo.IsTransferPointsToGameEnabled();

					bool areEqual = bTransferPointsToGameEnabled == TEST_DATA[testIndex].m_bTransferPointsToGameEnabledExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED bTransferPointsToGameEnabled=%d expected=%d testIndex=%d in %s"),
						(int)bTransferPointsToGameEnabled,
						(int)TEST_DATA[testIndex].m_bTransferPointsToGameEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #27.
				{
					const bool bTransferPromoDollarsToGameEnabled = pbInfo.IsTransferPromoDollarsToGameEnabled();

					bool areEqual = bTransferPromoDollarsToGameEnabled == TEST_DATA[testIndex].m_bTransferPromoDollarsToGameEnabledExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED bTransferPromoDollarsToGameEnabled=%d expected=%d testIndex=%d in %s"),
						(int)bTransferPromoDollarsToGameEnabled,
						(int)TEST_DATA[testIndex].m_bTransferPromoDollarsToGameEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #28.
				{
					const bool bTransferCashFromGameEnabled = pbInfo.IsTransferCashFromGameEnabled();

					bool areEqual = bTransferCashFromGameEnabled == TEST_DATA[testIndex].m_bTransferCashFromGameEnabledExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED bTransferCashFromGameEnabled=%d expected=%d testIndex=%d in %s"),
						(int)bTransferCashFromGameEnabled,
						(int)TEST_DATA[testIndex].m_bTransferCashFromGameEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #29.
				{
					const bool bGameDenomMatches = pbInfo.DoesGameDenomMatch();

					bool areEqual = bGameDenomMatches == TEST_DATA[testIndex].m_bGameDenomMatchesExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED bGameDenomMatches=%d expected=%d testIndex=%d in %s"),
						(int)bGameDenomMatches,
						(int)TEST_DATA[testIndex].m_bGameDenomMatchesExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #30.
				{
					const bool bTransferPromoFromGameEnabled = pbInfo.IsTransferPromoFromGameEnabled();

					bool areEqual = bTransferPromoFromGameEnabled == TEST_DATA[testIndex].m_bTransferPromoFromGameEnabledExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED bTransferPromoFromGameEnabled=%d expected=%d testIndex=%d in %s"),
						(int)bTransferPromoFromGameEnabled,
						(int)TEST_DATA[testIndex].m_bTransferPromoFromGameEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #31.
				{
					const bool bTransferToGameEnabled = pbInfo.IsTransferToGameEnabled();

					bool areEqual = bTransferToGameEnabled == TEST_DATA[testIndex].m_bTransferToGameEnabledExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED bTransferToGameEnabled=%d expected=%d testIndex=%d in %s"),
						(int)bTransferToGameEnabled,
						(int)TEST_DATA[testIndex].m_bTransferToGameEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #32.
				{
					const bool externalCreditProviderEnabled = pbInfo.ExternalCreditProviderEnabled();

					bool areEqual = externalCreditProviderEnabled == TEST_DATA[testIndex].m_externalCreditProviderEnabledExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED externalCreditProviderEnabled=%d expected=%d testIndex=%d in %s"),
						(int)externalCreditProviderEnabled,
						(int)TEST_DATA[testIndex].m_externalCreditProviderEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #33.
				{
					const DWORD creditFee = pbInfo.CreditFee();

					bool areEqual = creditFee == TEST_DATA[testIndex].m_creditFeeExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED creditFee=%u expected=%u testIndex=%d in %s"),
						(unsigned)creditFee,
						(unsigned)TEST_DATA[testIndex].m_creditFeeExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #32.
				{
					const bool creditFeeFixed = pbInfo.CreditFeeFixed();

					bool areEqual = creditFeeFixed == TEST_DATA[testIndex].m_creditFeeFixedExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED creditFeeFixed=%d expected=%d testIndex=%d in %s"),
						(int)creditFeeFixed,
						(int)TEST_DATA[testIndex].m_creditFeeFixedExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}
 
		/// <summary>
		/// Test the implementation of:
		///     CPersonalBankerInfo::CPersonalBankerInfo(MEMORY_NVRAM, false)
		///     CPersonalBankerInfo & CPersonalBankerInfo::operator=(const CPersonalBankerInfo &)
		/// </summary>
        TEST_F(PersonalBankerInfoUnitTests,CPersonalBankerInfoAssign2Test)
		{
			typedef struct
			{
				// Inputs.
				const MEMORY_TYPE m_type;

				const byte m_cardNumber[CARD_ID_LEN];
				PBError m_errorCode;

				const DWORD m_cashBalance;
				const DWORD m_pbFlags;
				const DWORD m_promo2Balance;
				const DWORD m_pointsAvailable;
				const DWORD m_pointsPerDollar;
				const DWORD m_sequenceNumber;
				const DWORD m_minimumPoints;
				const DWORD m_maximumPoints;
				const DWORD m_minimumCashDown;
				const DWORD m_maximumCashDown;
				const DWORD m_minimumCashUp;
				const DWORD m_maximumCashUp;
				const DWORD m_minimumPromoDown;
				const DWORD m_maximumPromoDown;
				const DWORD m_minimumPromoUp;
				const DWORD m_maximumPromoUp;
				const DWORD m_promoBalanceOnGame;
				const DWORD m_cashBalanceOnGame;

				const int m_PBIDLength;
				const char m_PBTID[MAX_PBID_LENGTH];

				const bool m_bTransferCashToGameEnabled;
				const bool m_bTransferPointsToGameEnabled;
				const bool m_bTransferPromoDollarsToGameEnabled;
				const bool m_bTransferCashFromGameEnabled;
				const bool m_bGameDenomMatches;
				const bool m_bEGMInitialized;
				const bool m_bTransferPromoFromGameEnabled;
				const bool m_bTransferPromoFromGameAutoEnabled;

				// Expected results.
				const MEMORY_TYPE m_typeExpected;

				const byte m_cardNumberExpected[CARD_ID_LEN];
				PBError m_errorCodeExpected;

				const DWORD m_cashBalanceExpected;
				const DWORD m_pbFlagsExpected;
				const DWORD m_promo2BalanceExpected;
				const DWORD m_pointsAvailableExpected;
				const DWORD m_pointsPerDollarExpected;
				const DWORD m_sequenceNumberExpected;
				const DWORD m_minimumPointsExpected;
				const DWORD m_maximumPointsExpected;
				const DWORD m_minimumCashDownExpected;
				const DWORD m_maximumCashDownExpected;
				const DWORD m_minimumCashUpExpected;
				const DWORD m_maximumCashUpExpected;
				const DWORD m_minimumPromoDownExpected;
				const DWORD m_maximumPromoDownExpected;
				const DWORD m_minimumPromoUpExpected;
				const DWORD m_maximumPromoUpExpected;
				const DWORD m_promoBalanceOnGameExpected;
				const DWORD m_cashBalanceOnGameExpected;

				const int m_PBIDLengthExpected;
				const char m_PBTIDExpected[MAX_PBID_LENGTH];

				const bool m_bTransferCashToGameEnabledExpected;
				const bool m_bTransferPointsToGameEnabledExpected;
				const bool m_bTransferPromoDollarsToGameEnabledExpected;
				const bool m_bTransferCashFromGameEnabledExpected;
				const bool m_bGameDenomMatchesExpected;
				// const bool m_bEGMInitializedExpected; // 2013-06-18 Not directly accessible.
				const bool m_bTransferPromoFromGameEnabledExpected;
				// const bool m_bTransferPromoFromGameAutoEnabled; // 2013-06-18 Not publicly accessible.
				const bool m_bTransferToGameEnabledExpected; // Test m_bEGMInitialized indirectly.
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ MEMORY_NVRAM,
					{ 1, 2, 3, 4, 5 }, AccountLocked,
					1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
					20, { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
					true, true, true, true, false, false, true, true,
					MEMORY_NONE,
					{ 1, 2, 3, 4, 5 }, AccountLocked,
					1, 2, 3, 4, 5, 6, 7, 4, 9, 0, 11, 12, 13, 0, 15, 16, 17, 18,
					20, { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
					true, true, true, true, false, true, false
				},
			};

			// More test data - expected results that are the same for each iteration.
			const bool bIsEFT(true);

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				//Setup
				CMemoryStorage::SetSprinkles();
				CPersonalBankerInfo pbInfo1(TEST_DATA[testIndex].m_type, false);
				pbInfo1.SetCardNumber(TEST_DATA[testIndex].m_cardNumber);
				pbInfo1.SetErrorCode(TEST_DATA[testIndex].m_errorCode);
				pbInfo1.SetCashBalance(TEST_DATA[testIndex].m_cashBalance);
				pbInfo1.SetPBFlags(TEST_DATA[testIndex].m_pbFlags);
				pbInfo1.SetPromo2Balance(TEST_DATA[testIndex].m_promo2Balance);
				pbInfo1.SetPointsAvailable(TEST_DATA[testIndex].m_pointsAvailable);
				pbInfo1.SetPointsPerDollar(TEST_DATA[testIndex].m_pointsPerDollar);
				pbInfo1.SetSequenceNumber(TEST_DATA[testIndex].m_sequenceNumber);
				pbInfo1.SetMinimumPoints(TEST_DATA[testIndex].m_minimumPoints);
				pbInfo1.SetMaximumPoints(TEST_DATA[testIndex].m_maximumPoints);
				pbInfo1.SetMinimumCashDown(TEST_DATA[testIndex].m_minimumCashDown);
				pbInfo1.SetMaximumCashDown(TEST_DATA[testIndex].m_maximumCashDown);
				pbInfo1.SetMinimumCashUp(TEST_DATA[testIndex].m_minimumCashUp);
				pbInfo1.SetMaximumCashUp(TEST_DATA[testIndex].m_maximumCashUp);
				pbInfo1.SetMinimumPromoDown(TEST_DATA[testIndex].m_minimumPromoDown);
				pbInfo1.SetMaximumPromoDown(TEST_DATA[testIndex].m_maximumPromoDown);
				pbInfo1.SetMinimumPromoUp(TEST_DATA[testIndex].m_minimumPromoUp);
				pbInfo1.SetMaximumPromoUp(TEST_DATA[testIndex].m_maximumPromoUp);
				pbInfo1.SetPromoBalanceOnGame(TEST_DATA[testIndex].m_promoBalanceOnGame);
				pbInfo1.SetCashBalanceOnGame(TEST_DATA[testIndex].m_cashBalanceOnGame);
				pbInfo1.SetCashBalanceOnGame(TEST_DATA[testIndex].m_cashBalanceOnGame);

				pbInfo1.SetPersonalBankerID(TEST_DATA[testIndex].m_PBTID, TEST_DATA[testIndex].m_PBIDLength);

				CConfig config(false, "");
				config.SetPBTCashEnabled_Global(TEST_DATA[testIndex].m_bTransferCashToGameEnabled);
				config.SetPBTCashEnabled_local(TEST_DATA[testIndex].m_bTransferCashToGameEnabled);
				config.SetPBTPointsEnabled_Global(TEST_DATA[testIndex].m_bTransferPointsToGameEnabled);
				config.SetPBTPointsAsPromo(TEST_DATA[testIndex].m_bTransferPointsToGameEnabled);
				config.SetPBTPromoEnabled_Global(TEST_DATA[testIndex].m_bTransferPromoDollarsToGameEnabled);
				config.SetPBTPromoEnabled_local(TEST_DATA[testIndex].m_bTransferPromoDollarsToGameEnabled);
				config.SetPBFromGameEnabled(TEST_DATA[testIndex].m_bTransferCashFromGameEnabled);
				config.SetGameDenom(1);
				config.SetDenomination(TEST_DATA[testIndex].m_bGameDenomMatches ? 1 : 5);
				if (TEST_DATA[testIndex].m_bTransferPromoFromGameEnabled)
				{
					config.SetSenOption2Raw(config.GetSenOption2Raw() + SO2_NCEP_OUT_ENABLED);
				}
				if (TEST_DATA[testIndex].m_bTransferPromoFromGameAutoEnabled)
				{
					config.SetSenOption2Raw(config.GetSenOption2Raw() + SO2_NCEP_AUTO_UPLOAD);
				}
				pbInfo1.SetConfigData(config, TEST_DATA[testIndex].m_bEGMInitialized, bIsEFT);

				// Perform operations to be tested.
				CPersonalBankerInfo pbInfo;
				pbInfo = pbInfo1;

				// Test #A.
				{
					const MEMORY_TYPE type = pbInfo.GetMemoryType();

					bool areEqual = type == TEST_DATA[testIndex].m_typeExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED type=%d expected=%d testIndex=%d in %s"),
						(int)type,
						(int)TEST_DATA[testIndex].m_typeExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #1.
				const byte * const cardNumber = pbInfo.GetCardNumber();

				{

					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED cardNumber=%p expected=NOT NULL testIndex=%d in %s"),
						(const void *)cardNumber,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_NE(cardNumber, nullptr) << failedMsg;
				}

				// Test #2.

				{
					bool areEqual = 0 == memcmp(cardNumber,
						TEST_DATA[testIndex].m_cardNumberExpected,
						sizeof(TEST_DATA[testIndex].m_cardNumberExpected));

					std::string cardNumberString = CUtilities::ToHexString(reinterpret_cast<const BYTE *>(cardNumber),
						sizeof(TEST_DATA[testIndex].m_cardNumberExpected));
					std::string cardNumberExpectedString =
						CUtilities::ToHexString(reinterpret_cast<const BYTE *>(TEST_DATA[testIndex].m_cardNumberExpected),
							sizeof(TEST_DATA[testIndex].m_cardNumberExpected));
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED cardNumber={%s} expected={%s} testIndex=%d in %s"),
						static_cast<LPCTSTR>(cardNumberString.c_str()),
						static_cast<LPCTSTR>(cardNumberExpectedString.c_str()),
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #3.
				{
					const PBError errorCode = pbInfo.GetErrorCode();

					bool areEqual = errorCode == TEST_DATA[testIndex].m_errorCodeExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED errorCode=%d expected=%d testIndex=%d in %s"),
						(int)errorCode,
						(int)TEST_DATA[testIndex].m_errorCodeExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #4.
				{
					const DWORD cashBalance = pbInfo.GetCashBalance();
	
					bool areEqual = cashBalance == TEST_DATA[testIndex].m_cashBalanceExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED cashBalance=%u expected=%u testIndex=%d in %s"),
						(unsigned)cashBalance,
						(unsigned)TEST_DATA[testIndex].m_cashBalanceExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #5.
				{
					const DWORD pbFlags = pbInfo.GetPBFlags();

					bool areEqual = pbFlags == TEST_DATA[testIndex].m_pbFlagsExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED pbFlags=%u expected=%u testIndex=%d in %s"),
						(unsigned)pbFlags,
						(unsigned)TEST_DATA[testIndex].m_pbFlagsExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}


				// Test #6.
				{
					const DWORD promo2Balance = pbInfo.GetPromo2Balance();

					bool areEqual = promo2Balance == TEST_DATA[testIndex].m_promo2BalanceExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED promo2Balance=%u expected=%u testIndex=%d in %s"),
						(unsigned)promo2Balance,
						(unsigned)TEST_DATA[testIndex].m_promo2BalanceExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #7.
				{
					const DWORD pointsAvailable = pbInfo.GetPointsAvailable();

					bool areEqual = pointsAvailable == TEST_DATA[testIndex].m_pointsAvailableExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED pointsAvailable=%u expected=%u testIndex=%d in %s"),
						(unsigned)pointsAvailable,
						(unsigned)TEST_DATA[testIndex].m_pointsAvailableExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #8.
				{
					const DWORD pointsPerDollar = pbInfo.GetPointsPerDollar();

					bool areEqual = pointsPerDollar == TEST_DATA[testIndex].m_pointsPerDollarExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED pointsPerDollar=%u expected=%u testIndex=%d in %s"),
						(unsigned)pointsPerDollar,
						(unsigned)TEST_DATA[testIndex].m_pointsPerDollarExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #9.
				{
					const DWORD sequenceNumber = pbInfo.GetSequenceNumber();

					bool areEqual = sequenceNumber == TEST_DATA[testIndex].m_sequenceNumberExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED sequenceNumber=%u expected=%u testIndex=%d in %s"),
						(unsigned)sequenceNumber,
						(unsigned)TEST_DATA[testIndex].m_sequenceNumberExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #10.
				{
					const DWORD minimumPoints = pbInfo.GetMinimumPoints();

					bool areEqual = minimumPoints == TEST_DATA[testIndex].m_minimumPointsExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED minimumPoints=%u expected=%u testIndex=%d in %s"),
						(unsigned)minimumPoints,
						(unsigned)TEST_DATA[testIndex].m_minimumPointsExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #11.
				{
					const DWORD maximumPoints = pbInfo.GetMaximumPoints();

					bool areEqual = maximumPoints == TEST_DATA[testIndex].m_maximumPointsExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED maximumPoints=%u expected=%u testIndex=%d in %s"),
						(unsigned)maximumPoints,
						(unsigned)TEST_DATA[testIndex].m_maximumPointsExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #12.
				{
					const DWORD minimumCashDown = pbInfo.GetMinimumCashDown();

					bool areEqual = minimumCashDown == TEST_DATA[testIndex].m_minimumCashDownExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED minimumCashDown=%u expected=%u testIndex=%d in %s"),
						(unsigned)minimumCashDown,
						(unsigned)TEST_DATA[testIndex].m_minimumCashDownExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #13.
				{
					const DWORD maximumCashDown = pbInfo.GetMaximumCashDown();

					bool areEqual = maximumCashDown == TEST_DATA[testIndex].m_maximumCashDownExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED maximumCashDown=%u expected=%u testIndex=%d in %s"),
						(unsigned)maximumCashDown,
						(unsigned)TEST_DATA[testIndex].m_maximumCashDownExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #14.
				{
					const DWORD minimumCashUp = pbInfo.GetMinimumCashUp();

					bool areEqual = minimumCashUp == TEST_DATA[testIndex].m_minimumCashUpExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED minimumCashUp=%u expected=%u testIndex=%d in %s"),
						(unsigned)minimumCashUp,
						(unsigned)TEST_DATA[testIndex].m_minimumCashUpExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #15.
				{
					const DWORD maximumCashUp = pbInfo.GetMaximumCashUp();

					bool areEqual = maximumCashUp == TEST_DATA[testIndex].m_maximumCashUpExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED maximumCashUp=%u expected=%u testIndex=%d in %s"),
						(unsigned)maximumCashUp,
						(unsigned)TEST_DATA[testIndex].m_maximumCashUpExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #16.
				{
					const DWORD minimumPromoDown = pbInfo.GetMinimumPromoDown();

					bool areEqual = minimumPromoDown == TEST_DATA[testIndex].m_minimumPromoDownExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED minimumPromoDown=%u expected=%u testIndex=%d in %s"),
						(unsigned)minimumPromoDown,
						(unsigned)TEST_DATA[testIndex].m_minimumPromoDownExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #17.
				{
					const DWORD maximumPromoDown = pbInfo.GetMaximumPromoDown();

					bool areEqual = maximumPromoDown == TEST_DATA[testIndex].m_maximumPromoDownExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED maximumPromoDown=%u expected=%u testIndex=%d in %s"),
						(unsigned)maximumPromoDown,
						(unsigned)TEST_DATA[testIndex].m_maximumPromoDownExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #18.
				{
					const DWORD minimumPromoUp = pbInfo.GetMinimumPromoUp();

					bool areEqual = minimumPromoUp == TEST_DATA[testIndex].m_minimumPromoUpExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED minimumPromoUp=%u expected=%u testIndex=%d in %s"),
						(unsigned)minimumPromoUp,
						(unsigned)TEST_DATA[testIndex].m_minimumPromoUpExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #19.
				{
					const DWORD maximumPromoUp = pbInfo.GetMaximumPromoUp();

					bool areEqual = maximumPromoUp == TEST_DATA[testIndex].m_maximumPromoUpExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED maximumPromoUp=%u expected=%u testIndex=%d in %s"),
						(unsigned)maximumPromoUp,
						(unsigned)TEST_DATA[testIndex].m_maximumPromoUpExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #20.
				{
					const DWORD promoBalanceOnGame = pbInfo.GetPromoBalanceOnGame();

					bool areEqual = promoBalanceOnGame == TEST_DATA[testIndex].m_promoBalanceOnGameExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED promoBalanceOnGame=%u expected=%u testIndex=%d in %s"),
						(unsigned)promoBalanceOnGame,
						(unsigned)TEST_DATA[testIndex].m_promoBalanceOnGameExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #21.
				{
					const DWORD cashBalanceOnGame = pbInfo.GetCashBalanceOnGame();

					bool areEqual = cashBalanceOnGame == TEST_DATA[testIndex].m_cashBalanceOnGameExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED cashBalanceOnGame=%u expected=%u testIndex=%d in %s"),
						(unsigned)cashBalanceOnGame,
						(unsigned)TEST_DATA[testIndex].m_cashBalanceOnGameExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #22.
				{
					const int PBIDLength = pbInfo.GetPBIDLength();

					bool areEqual = PBIDLength == TEST_DATA[testIndex].m_PBIDLengthExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED PBIDLength=%d expected=%d testIndex=%d in %s"),
						(int)PBIDLength,
						(int)TEST_DATA[testIndex].m_PBIDLengthExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #23.
				const char * const PBTID = pbInfo.GetPersonalBankerID();

				{

					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED pbID=%p expected=NOT NULL testIndex=%d in %s"),
						(const void *)PBTID,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_NE(PBTID,nullptr) << failedMsg;
				}

				// Test #24.

				{
					bool areEqual = 0 == memcmp(PBTID,
						TEST_DATA[testIndex].m_PBTIDExpected,
						TEST_DATA[testIndex].m_PBIDLengthExpected);

					std::string PBTIDString = CUtilities::ToHexString(reinterpret_cast<const BYTE *>(PBTID), TEST_DATA[testIndex].m_PBIDLengthExpected);
					std::string PBTIDExpectedString = CUtilities::ToHexString(reinterpret_cast<const BYTE *>(TEST_DATA[testIndex].m_PBTIDExpected), TEST_DATA[testIndex].m_PBIDLengthExpected);
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED pbID={%s} expected={%s} in %s"),
						static_cast<LPCTSTR>(PBTIDString.c_str()),
						static_cast<LPCTSTR>(PBTIDExpectedString.c_str()),
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #25.
				{
					const bool bTransferCashToGameEnabled = pbInfo.IsTransferCashToGameEnabled();

					bool areEqual = bTransferCashToGameEnabled == TEST_DATA[testIndex].m_bTransferCashToGameEnabledExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED bTransferCashToGameEnabled=%d expected=%d testIndex=%d in %s"),
						(int)bTransferCashToGameEnabled,
						(int)TEST_DATA[testIndex].m_bTransferCashToGameEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #26.
				{
					const bool bTransferPointsToGameEnabled = pbInfo.IsTransferPointsToGameEnabled();

					bool areEqual = bTransferPointsToGameEnabled == TEST_DATA[testIndex].m_bTransferPointsToGameEnabledExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED bTransferPointsToGameEnabled=%d expected=%d testIndex=%d (%d %d %d %d %d %d) in %s"),
						(int)bTransferPointsToGameEnabled,
						(int)TEST_DATA[testIndex].m_bTransferPointsToGameEnabledExpected,
						(int)testIndex,
						(int)TEST_DATA[testIndex].m_bTransferPointsToGameEnabled,
						(int)config.PBTPointsEnabledGlobal(),
						(int)config.PBTPointsEnabled(),
						(int)bIsEFT,
						(int)config.PBTPointsAsPromo(),
						(int)pbInfo.IsTransferPointsToGameEnabled(),
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #27.
				{
					const bool bTransferPromoDollarsToGameEnabled = pbInfo.IsTransferPromoDollarsToGameEnabled();

					bool areEqual = bTransferPromoDollarsToGameEnabled == TEST_DATA[testIndex].m_bTransferPromoDollarsToGameEnabledExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED bTransferPromoDollarsToGameEnabled=%d expected=%d testIndex=%d in %s"),
						(int)bTransferPromoDollarsToGameEnabled,
						(int)TEST_DATA[testIndex].m_bTransferPromoDollarsToGameEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #28.
				{
					const bool bTransferCashFromGameEnabled = pbInfo.IsTransferCashFromGameEnabled();

					bool areEqual = bTransferCashFromGameEnabled == TEST_DATA[testIndex].m_bTransferCashFromGameEnabledExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED bTransferCashFromGameEnabled=%d expected=%d testIndex=%d in %s"),
						(int)bTransferCashFromGameEnabled,
						(int)TEST_DATA[testIndex].m_bTransferCashFromGameEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #29.
				{
					const bool bGameDenomMatches = pbInfo.DoesGameDenomMatch();

					bool areEqual = bGameDenomMatches == TEST_DATA[testIndex].m_bGameDenomMatchesExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED bGameDenomMatches=%d expected=%d testIndex=%d in %s"),
						(int)bGameDenomMatches,
						(int)TEST_DATA[testIndex].m_bGameDenomMatchesExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #30.
				{
					const bool bTransferPromoFromGameEnabled = pbInfo.IsTransferPromoFromGameEnabled();

					bool areEqual = bTransferPromoFromGameEnabled == TEST_DATA[testIndex].m_bTransferPromoFromGameEnabledExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED bTransferPromoFromGameEnabled=%d expected=%d testIndex=%d in %s"),
						(int)bTransferPromoFromGameEnabled,
						(int)TEST_DATA[testIndex].m_bTransferPromoFromGameEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #31.
				{
					const bool bTransferToGameEnabled = pbInfo.IsTransferToGameEnabled();

					bool areEqual = bTransferToGameEnabled == TEST_DATA[testIndex].m_bTransferToGameEnabledExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED bTransferToGameEnabled=%d expected=%d testIndex=%d (%d %d %d %d) in %s"),
						(int)bTransferToGameEnabled,
						(int)TEST_DATA[testIndex].m_bTransferToGameEnabledExpected,
						(int)testIndex,
						(int)TEST_DATA[testIndex].m_bTransferCashToGameEnabledExpected,
						(int)TEST_DATA[testIndex].m_bTransferPointsToGameEnabledExpected,
						(int)TEST_DATA[testIndex].m_bTransferPromoDollarsToGameEnabledExpected,
						(int)TEST_DATA[testIndex].m_bEGMInitialized,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

        // #pragma region DisplayTransferCashToGame
        TEST_F(PersonalBankerInfoUnitTests,DisplayTransferCashToGameTest)
		{
            DisplayTransferCashToGameTest();
        }

        void PersonalBankerInfoUnitTests::DisplayTransferCashToGameTest()
        {
			// Setup.
            CPersonalBankerInfo personalBankerInfo;                
            personalBankerInfo.m_bTransferCashToGameEnabled = true;
            personalBankerInfo.m_bEGMInitialized = true;           
            personalBankerInfo.m_minimumCashDown = 1;              
            personalBankerInfo.m_maximumCashDown = 1000;           
            personalBankerInfo.m_cashBalance = 300;                

			//Test
			ASSERT_TRUE(personalBankerInfo.DisplayTransferCashToGame()) << _T("FAILED DisplayTransferCashToGame returned the wrong value");
		}

		/// <summary>
		/// Functional test for:
		///     CPersonalBankerInfo::DisplayTransferCashToGame()
		/// Verify that the result depends on the IsBannedFromRedemptions() value.
		/// </summary>
        TEST_F(PersonalBankerInfoUnitTests,DisplayTransferCashToGame_BannedFromRedemptionsTest)
		{
            DisplayTransferCashToGame_BannedFromRedemptionsTest();
        }
        void PersonalBankerInfoUnitTests::DisplayTransferCashToGame_BannedFromRedemptionsTest()
		{	// Setup.
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bTransferCashToGameEnabled = true;
			personalBankerInfo.m_bEGMInitialized = true;
			personalBankerInfo.m_minimumCashDown = 10;
			personalBankerInfo.m_maximumCashDown = 100;
			personalBankerInfo.m_cashBalance = 3000;

			//Test
			personalBankerInfo.m_pbFlags.flags.IsBannedFromRedemptions = 0;
			ASSERT_TRUE(personalBankerInfo.DisplayTransferCashToGame()) << _T("DisplayTransferCashToGame true");
			personalBankerInfo.m_pbFlags.flags.IsBannedFromRedemptions = 1;
			ASSERT_FALSE(personalBankerInfo.DisplayTransferCashToGame()) << _T("DisplayTransferCashToGame false");
		}

		/// <summary>
		/// Functional test for:
		///     CPersonalBankerInfo::DisplayTransferCashToGame()
		/// Verify that the result depends on the m_bTransferCashToGameEnabled value.
		/// </summary>
        TEST_F(PersonalBankerInfoUnitTests,DisplayTransferCashToGame_TransferEnabledTest)
		{
            DisplayTransferCashToGame_TransferEnabledTest();
        }
        void PersonalBankerInfoUnitTests::DisplayTransferCashToGame_TransferEnabledTest()
        {
			// Setup.
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bEGMInitialized = true;
			personalBankerInfo.m_minimumCashDown = 10;
			personalBankerInfo.m_maximumCashDown = 100;
			personalBankerInfo.m_cashBalance = 3000;

			//Test
			personalBankerInfo.m_bTransferCashToGameEnabled = true;
			ASSERT_TRUE(personalBankerInfo.DisplayTransferCashToGame()) << _T("DisplayTransferCashToGame true");
			personalBankerInfo.m_bTransferCashToGameEnabled = false;
			ASSERT_FALSE(personalBankerInfo.DisplayTransferCashToGame()) << _T("DisplayTransferCashToGame false");
		}

		/// <summary>
		/// Functional test for:
		///     CPersonalBankerInfo::DisplayTransferCashToGame()
		/// Verify that the result does not depend on the m_minimumCashDown value alone.
		/// </summary>
        TEST_F(PersonalBankerInfoUnitTests,DisplayTransferCashToGame_MinTest)
		{
            DisplayTransferCashToGame_MinTest();
        }
        void PersonalBankerInfoUnitTests::DisplayTransferCashToGame_MinTest()
        {
			// Setup.
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bTransferCashToGameEnabled = true;
			personalBankerInfo.m_bEGMInitialized = true;
			personalBankerInfo.m_maximumCashDown = 100;
			personalBankerInfo.m_cashBalance = 3000;

			//Test
			personalBankerInfo.m_minimumCashDown = 2;
			ASSERT_TRUE(personalBankerInfo.DisplayTransferCashToGame()) << _T("DisplayTransferCashToGame 2");
			personalBankerInfo.m_minimumCashDown = 1;
			ASSERT_TRUE(personalBankerInfo.DisplayTransferCashToGame()) << _T("DisplayTransferCashToGame 1");
			personalBankerInfo.m_minimumCashDown = 0;
			ASSERT_TRUE(personalBankerInfo.DisplayTransferCashToGame()) << _T("DisplayTransferCashToGame 0");
		}

		/// <summary>
		/// Functional test for:
		///     CPersonalBankerInfo::DisplayTransferCashToGame()
		/// Verify that the result depends on the m_maximumCashDown value.
		/// </summary>
        TEST_F(PersonalBankerInfoUnitTests,DisplayTransferCashToGame_MaxTest)
		{
            DisplayTransferCashToGame_MaxTest();
        }
        void PersonalBankerInfoUnitTests::DisplayTransferCashToGame_MaxTest()
        {
			// Setup.
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bTransferCashToGameEnabled = true;
			personalBankerInfo.m_bEGMInitialized = true;
			personalBankerInfo.m_minimumCashDown = 0;
			personalBankerInfo.m_cashBalance = 3000;

			//Test
			personalBankerInfo.m_maximumCashDown = 2;
			ASSERT_TRUE(personalBankerInfo.DisplayTransferCashToGame()) << _T("DisplayTransferCashToGame 2");
			personalBankerInfo.m_maximumCashDown = 1;
			ASSERT_TRUE(personalBankerInfo.DisplayTransferCashToGame()) << _T("DisplayTransferCashToGame 1");
			personalBankerInfo.m_maximumCashDown = 0;
			ASSERT_FALSE(personalBankerInfo.DisplayTransferCashToGame()) <<  _T("DisplayTransferCashToGame 0");
		}

		/// <summary>
		/// Functional test for:
		///     CPersonalBankerInfo::DisplayTransferCashToGame()
		/// Verify that the result depends on the relative m_minimumCashDown to m_maximumCashDown values.
		/// </summary>
        TEST_F(PersonalBankerInfoUnitTests,DisplayTransferCashToGame_MinMaxTest)
		{
            DisplayTransferCashToGame_MinMaxTest();
        }
        void PersonalBankerInfoUnitTests::DisplayTransferCashToGame_MinMaxTest()
        {
			// Setup.
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bTransferCashToGameEnabled = true;
			personalBankerInfo.m_bEGMInitialized = true;
			personalBankerInfo.m_minimumCashDown = 10;
			personalBankerInfo.m_cashBalance = 3000;

			//Test
			personalBankerInfo.m_maximumCashDown = personalBankerInfo.m_minimumCashDown + 1;
			ASSERT_TRUE(personalBankerInfo.DisplayTransferCashToGame()) << _T("DisplayTransferCashToGame gt");
			personalBankerInfo.m_maximumCashDown = personalBankerInfo.m_minimumCashDown;
			ASSERT_TRUE(personalBankerInfo.DisplayTransferCashToGame()) << _T("DisplayTransferCashToGame eq");
			personalBankerInfo.m_maximumCashDown = personalBankerInfo.m_minimumCashDown - 1;
			ASSERT_FALSE(personalBankerInfo.DisplayTransferCashToGame()) << _T("DisplayTransferCashToGame lt");
		}

		/// <summary>
		/// Functional test for:
		///     CPersonalBankerInfo::DisplayTransferCashToGame()
		/// Verify that the result depends on the relative m_minimumCashDown to m_cashBalance values.
		/// </summary>
        TEST_F(PersonalBankerInfoUnitTests,DisplayTransferCashToGame_MinBalanceTest)
		{
            DisplayTransferCashToGame_MinBalanceTest();
        }
        void PersonalBankerInfoUnitTests::DisplayTransferCashToGame_MinBalanceTest()
        {
			// Setup.
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bTransferCashToGameEnabled = true;
			personalBankerInfo.m_bEGMInitialized = true;
			personalBankerInfo.m_minimumCashDown = 10;
			personalBankerInfo.m_maximumCashDown = 100;
			ASSERT_EQ((DWORD)100, ONE_DOLLAR_IN_CENTS) << _T("ONE_DOLLAR_IN_CENTS");

			//Test
			personalBankerInfo.m_cashBalance = (personalBankerInfo.m_minimumCashDown + 1) * ONE_DOLLAR_IN_CENTS;
			ASSERT_TRUE(personalBankerInfo.DisplayTransferCashToGame()) << _T("DisplayTransferCashToGame gt");
			personalBankerInfo.m_cashBalance = (personalBankerInfo.m_minimumCashDown + 0) * ONE_DOLLAR_IN_CENTS;
			ASSERT_TRUE(personalBankerInfo.DisplayTransferCashToGame()) << _T("DisplayTransferCashToGame eq");
			personalBankerInfo.m_cashBalance = (personalBankerInfo.m_minimumCashDown - 1) * ONE_DOLLAR_IN_CENTS;
			ASSERT_FALSE(personalBankerInfo.DisplayTransferCashToGame()) << _T("DisplayTransferCashToGame lt");
		}

		/// <summary>
		/// Functional test for:
		///     CPersonalBankerInfo::DisplayTransferCashToGame()
		/// Verify that the result does not depend on the relative m_maximumCashDown to m_cashBalance values.
		/// </summary>
        TEST_F(PersonalBankerInfoUnitTests,DisplayTransferCashToGame_MaxBalanceTest)
		{
            DisplayTransferCashToGame_MaxBalanceTest();
        }
        void PersonalBankerInfoUnitTests::DisplayTransferCashToGame_MaxBalanceTest()
        {
			// Setup.
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bTransferCashToGameEnabled = true;
			personalBankerInfo.m_bEGMInitialized = true;
			personalBankerInfo.m_minimumCashDown = 10;
			personalBankerInfo.m_maximumCashDown = 100;
			ASSERT_EQ((DWORD)100, ONE_DOLLAR_IN_CENTS) << _T("ONE_DOLLAR_IN_CENTS");

			//Test
			personalBankerInfo.m_cashBalance = (personalBankerInfo.m_maximumCashDown + 1) * ONE_DOLLAR_IN_CENTS;
			ASSERT_TRUE(personalBankerInfo.DisplayTransferCashToGame()) << _T("DisplayTransferCashToGame gt");
			personalBankerInfo.m_cashBalance = (personalBankerInfo.m_maximumCashDown + 0) * ONE_DOLLAR_IN_CENTS;
			ASSERT_TRUE(personalBankerInfo.DisplayTransferCashToGame()) << _T("DisplayTransferCashToGame eq");
			personalBankerInfo.m_cashBalance = (personalBankerInfo.m_maximumCashDown - 1) * ONE_DOLLAR_IN_CENTS;
			ASSERT_TRUE(personalBankerInfo.DisplayTransferCashToGame()) << _T("DisplayTransferCashToGame lt");
		}

		/// <summary>
		/// Functional test for:
		///     CPersonalBankerInfo::DisplayTransferCashToGame()
		/// Verify that the result depends on the m_cashBalance value alone.
		/// </summary>
        TEST_F(PersonalBankerInfoUnitTests,DisplayTransferCashToGame_BalanceTest)
		{
            DisplayTransferCashToGame_BalanceTest();
        }
        void PersonalBankerInfoUnitTests::DisplayTransferCashToGame_BalanceTest()
		{	// Setup.
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bTransferCashToGameEnabled = true;
			personalBankerInfo.m_bEGMInitialized = true;
			personalBankerInfo.m_minimumCashDown = 0;
			personalBankerInfo.m_maximumCashDown = 100;

			//Test
			personalBankerInfo.m_cashBalance = ONE_DOLLAR_IN_CENTS + 1;
			ASSERT_TRUE(personalBankerInfo.DisplayTransferCashToGame()) << _T("DisplayTransferCashToGame gt");
			personalBankerInfo.m_cashBalance = ONE_DOLLAR_IN_CENTS + 0;
			ASSERT_TRUE(personalBankerInfo.DisplayTransferCashToGame()) << _T("DisplayTransferCashToGame eq");
			personalBankerInfo.m_cashBalance = ONE_DOLLAR_IN_CENTS - 1;
			ASSERT_FALSE(personalBankerInfo.DisplayTransferCashToGame()) << _T("DisplayTransferCashToGame lt");
		}
// #pragma endregion DisplayTransferCashToGame

// #pragma region DisplayTransferPointsToGame
        TEST_F(PersonalBankerInfoUnitTests,DisplayTransferPointsToGameTest)
		{
            DisplayTransferPointsToGameTest();
        }
        void PersonalBankerInfoUnitTests::DisplayTransferPointsToGameTest()
		{	// Setup.
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bTransferPointsToGameEnabled = true;
			personalBankerInfo.m_bEGMInitialized = true;
			personalBankerInfo.m_minimumPoints = 100;
			personalBankerInfo.m_maximumPoints = 1000;
			personalBankerInfo.m_pointsPerDollar = 100;
			personalBankerInfo.m_pointsAvailable = 300;

			//Test
			ASSERT_TRUE(personalBankerInfo.DisplayTransferPointsToGame()) <<  _T("FAILED DisplayTransferPointsToGame returned the wrong value");
		}

		/// <summary>
		/// Functional test for:
		///     CPersonalBankerInfo::DisplayTransferPointsToGame()
		/// Verify that the result depends on the IsBannedFromRedemptions() value.
		/// </summary>
        TEST_F(PersonalBankerInfoUnitTests,DisplayTransferPointsToGame_BannedFromRedemptionsTest)
		{
            DisplayTransferPointsToGame_BannedFromRedemptionsTest();
        }
        void PersonalBankerInfoUnitTests::DisplayTransferPointsToGame_BannedFromRedemptionsTest()
        {
			// Setup.
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bTransferPointsToGameEnabled = true;
			personalBankerInfo.m_bEGMInitialized = true;
			personalBankerInfo.m_minimumPoints = 100;
			personalBankerInfo.m_maximumPoints = 1000;
			personalBankerInfo.m_pointsPerDollar = 10;
			personalBankerInfo.m_pointsAvailable = 300;

			//Test
			personalBankerInfo.m_pbFlags.flags.IsBannedFromRedemptions = 0;
			ASSERT_TRUE(personalBankerInfo.DisplayTransferPointsToGame()) << _T("DisplayTransferPointsToGame true");
			personalBankerInfo.m_pbFlags.flags.IsBannedFromRedemptions = 1;
			ASSERT_FALSE(personalBankerInfo.DisplayTransferPointsToGame()) << _T("DisplayTransferPointsToGame false");
		}

		/// <summary>
		/// Functional test for:
		///     CPersonalBankerInfo::DisplayTransferPointsToGame()
		/// Verify that the result depends on the m_bTransferPointsToGameEnabled value.
		/// </summary>
        TEST_F(PersonalBankerInfoUnitTests,DisplayTransferPointsToGame_TransferEnabledTest)
		{
            DisplayTransferPointsToGame_TransferEnabledTest();
        }
        void PersonalBankerInfoUnitTests::DisplayTransferPointsToGame_TransferEnabledTest()
		{	// Setup.
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bEGMInitialized = true;
			personalBankerInfo.m_minimumPoints = 100;
			personalBankerInfo.m_maximumPoints = 1000;
			personalBankerInfo.m_pointsPerDollar = 10;
			personalBankerInfo.m_pointsAvailable = 300;

			//Test
			personalBankerInfo.m_bTransferPointsToGameEnabled = true;
			ASSERT_TRUE(personalBankerInfo.DisplayTransferPointsToGame()) << _T("DisplayTransferPointsToGame true");
			personalBankerInfo.m_bTransferPointsToGameEnabled = false;
			ASSERT_FALSE(personalBankerInfo.DisplayTransferPointsToGame()) << _T("DisplayTransferPointsToGame false");
		}

		/// <summary>
		/// Functional test for:
		///     CPersonalBankerInfo::DisplayTransferPointsToGame()
		/// Verify that the result does not depend on the m_minimumPoints value alone.
		/// </summary>
        TEST_F(PersonalBankerInfoUnitTests,DisplayTransferPointsToGame_MinTest)
		{
            DisplayTransferPointsToGame_MinTest();
        }
        void PersonalBankerInfoUnitTests::DisplayTransferPointsToGame_MinTest()
        {
			// Setup.
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bTransferPointsToGameEnabled = true;
			personalBankerInfo.m_bEGMInitialized = true;
			personalBankerInfo.m_maximumPoints = 1000;
			personalBankerInfo.m_pointsPerDollar = 10;
			personalBankerInfo.m_pointsAvailable = 300;

			//Test
			personalBankerInfo.m_minimumPoints = personalBankerInfo.m_pointsPerDollar * 2;
			ASSERT_TRUE(personalBankerInfo.DisplayTransferPointsToGame()) << _T("DisplayTransferPointsToGame *2");
			personalBankerInfo.m_minimumPoints = personalBankerInfo.m_pointsPerDollar + 1;
			ASSERT_TRUE(personalBankerInfo.DisplayTransferPointsToGame()) << _T("DisplayTransferPointsToGame +1");
			personalBankerInfo.m_minimumPoints = personalBankerInfo.m_pointsPerDollar + 0;
			ASSERT_TRUE(personalBankerInfo.DisplayTransferPointsToGame()) << _T("DisplayTransferPointsToGame +0");
			personalBankerInfo.m_minimumPoints = personalBankerInfo.m_pointsPerDollar - 1;
			ASSERT_TRUE(personalBankerInfo.DisplayTransferPointsToGame()) << _T("DisplayTransferPointsToGame -1");
			personalBankerInfo.m_minimumPoints = 0;
			ASSERT_TRUE(personalBankerInfo.DisplayTransferPointsToGame()) << _T("DisplayTransferPointsToGame 0");
		}

		/// <summary>
		/// Functional test for:
		///     CPersonalBankerInfo::DisplayTransferPointsToGame()
		/// Verify that the result depends on the m_maximumPromoDown value.
		/// </summary>
        TEST_F(PersonalBankerInfoUnitTests,DisplayTransferPointsToGame_MaxTest)
		{
            DisplayTransferPointsToGame_MaxTest();
        }
        void PersonalBankerInfoUnitTests::DisplayTransferPointsToGame_MaxTest()
		{	// Setup.
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bTransferPointsToGameEnabled = true;
			personalBankerInfo.m_bEGMInitialized = true;
			personalBankerInfo.m_minimumPoints = 0;
			personalBankerInfo.m_pointsPerDollar = 10;
			personalBankerInfo.m_pointsAvailable = 300;

			//Test
			personalBankerInfo.m_maximumPoints = personalBankerInfo.m_pointsPerDollar * 2;
			ASSERT_TRUE( personalBankerInfo.DisplayTransferPointsToGame()) << _T("DisplayTransferPointsToGame *2");
			personalBankerInfo.m_maximumPoints = personalBankerInfo.m_pointsPerDollar + 1;
			ASSERT_TRUE( personalBankerInfo.DisplayTransferPointsToGame()) << _T("DisplayTransferPointsToGame +1");
			personalBankerInfo.m_maximumPoints = personalBankerInfo.m_pointsPerDollar + 0;
			ASSERT_TRUE( personalBankerInfo.DisplayTransferPointsToGame()) << _T("DisplayTransferPointsToGame +0");
			personalBankerInfo.m_maximumPoints = personalBankerInfo.m_pointsPerDollar - 1;
			ASSERT_FALSE( personalBankerInfo.DisplayTransferPointsToGame()) << _T("DisplayTransferPointsToGame -1");
			personalBankerInfo.m_maximumPoints = 0;
			ASSERT_FALSE( personalBankerInfo.DisplayTransferPointsToGame()) << _T("DisplayTransferPointsToGame 0");
		}

		/// <summary>
		/// Functional test for:
		///     CPersonalBankerInfo::DisplayTransferPointsToGame()
		/// Verify that the result depends on the relative m_minimumPromoDown to m_maximumPromoDown values.
		/// </summary>
        TEST_F(PersonalBankerInfoUnitTests,DisplayTransferPointsToGame_MinMaxTest)
		{
            DisplayTransferPointsToGame_MinMaxTest();
        }
        void PersonalBankerInfoUnitTests::DisplayTransferPointsToGame_MinMaxTest()
		{	// Setup.
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bTransferPointsToGameEnabled = true;
			personalBankerInfo.m_bEGMInitialized = true;
			personalBankerInfo.m_minimumPoints = 100;
			personalBankerInfo.m_pointsPerDollar = 10;
			personalBankerInfo.m_pointsAvailable = 300;

			//Test
			personalBankerInfo.m_maximumPoints = personalBankerInfo.m_minimumPoints + 1;
			ASSERT_TRUE( personalBankerInfo.DisplayTransferPointsToGame()) << _T("DisplayTransferPointsToGame gt");
			personalBankerInfo.m_maximumPoints = personalBankerInfo.m_minimumPoints;
			ASSERT_TRUE( personalBankerInfo.DisplayTransferPointsToGame()) << _T("DisplayTransferPointsToGame eq");
			personalBankerInfo.m_maximumPoints = personalBankerInfo.m_minimumPoints - 1;
			ASSERT_FALSE( personalBankerInfo.DisplayTransferPointsToGame()) << _T("DisplayTransferPointsToGame lt");
		}

		/// <summary>
		/// Functional test for:
		///     CPersonalBankerInfo::DisplayTransferPointsToGame()
		/// Verify that the result depends on the relative m_minimumPoints to m_pointsAvailable values.
		/// </summary>
        TEST_F(PersonalBankerInfoUnitTests,DisplayTransferPointsToGame_MinBalanceTest)
		{
            DisplayTransferPointsToGame_MinBalanceTest();
        }
        void PersonalBankerInfoUnitTests::DisplayTransferPointsToGame_MinBalanceTest()
		{	// Setup.
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bTransferPointsToGameEnabled = true;
			personalBankerInfo.m_minimumPoints = 100;
			personalBankerInfo.m_maximumPoints = 1000;
			personalBankerInfo.m_pointsPerDollar = 10;

			//Test
			personalBankerInfo.m_pointsAvailable = personalBankerInfo.m_minimumPoints + 1;
			ASSERT_TRUE( personalBankerInfo.DisplayTransferPointsToGame()) << _T("DisplayTransferPointsToGame gt");
			personalBankerInfo.m_pointsAvailable = personalBankerInfo.m_minimumPoints + 0;
			ASSERT_TRUE( personalBankerInfo.DisplayTransferPointsToGame()) << _T("DisplayTransferPointsToGame eq");
			personalBankerInfo.m_pointsAvailable = personalBankerInfo.m_minimumPoints - 1;
			ASSERT_FALSE( personalBankerInfo.DisplayTransferPointsToGame()) << _T("DisplayTransferPointsToGame lt");
		}

		/// <summary>
		/// Functional test for:
		///     CPersonalBankerInfo::DisplayTransferPointsToGame()
		/// Verify that the result depends on the relative m_minimumPoints to m_pointsAvailable to m_pointsPerDollar values.
		/// </summary>
        TEST_F(PersonalBankerInfoUnitTests,DisplayTransferPointsToGame_MinBalancePointsPerDollarTest)
		{
            DisplayTransferPointsToGame_MinBalancePointsPerDollarTest();
        }
        void PersonalBankerInfoUnitTests::DisplayTransferPointsToGame_MinBalancePointsPerDollarTest()
		{	// Setup.
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bTransferPointsToGameEnabled = true;
			personalBankerInfo.m_minimumPoints = 15;
			personalBankerInfo.m_maximumPoints = 1000;
			personalBankerInfo.m_pointsPerDollar = 10;

			//Test
			personalBankerInfo.m_pointsAvailable = personalBankerInfo.m_pointsPerDollar * 2;
			ASSERT_TRUE( personalBankerInfo.DisplayTransferPointsToGame()) << _T("DisplayTransferPointsToGame *2");
			personalBankerInfo.m_pointsAvailable = personalBankerInfo.m_pointsPerDollar * 2 - 1;
			ASSERT_FALSE( personalBankerInfo.DisplayTransferPointsToGame()) << _T("DisplayTransferPointsToGame *2 -1");
			personalBankerInfo.m_pointsAvailable = personalBankerInfo.m_minimumPoints + 1;
			ASSERT_FALSE( personalBankerInfo.DisplayTransferPointsToGame()) << _T("DisplayTransferPointsToGame gt");
			personalBankerInfo.m_pointsAvailable = personalBankerInfo.m_minimumPoints + 0;
			ASSERT_FALSE( personalBankerInfo.DisplayTransferPointsToGame()) << _T("DisplayTransferPointsToGame eq");
			personalBankerInfo.m_pointsAvailable = personalBankerInfo.m_minimumPoints - 1;
			ASSERT_FALSE( personalBankerInfo.DisplayTransferPointsToGame()) << _T("DisplayTransferPointsToGame lt");
		}

		/// <summary>
		/// Functional test for:
		///     CPersonalBankerInfo::DisplayTransferPointsToGame()
		/// Verify that the result does not depend on the relative m_maximumPoints to m_pointsAvailable values.
		/// </summary>
        TEST_F(PersonalBankerInfoUnitTests,DisplayTransferPointsToGame_MaxBalanceTest)
		{
            DisplayTransferPointsToGame_MaxBalanceTest();
        }
        void PersonalBankerInfoUnitTests::DisplayTransferPointsToGame_MaxBalanceTest()
        {
			// Setup.
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bTransferPointsToGameEnabled = true;
			personalBankerInfo.m_bEGMInitialized = true;
			personalBankerInfo.m_minimumPoints = 100;
			personalBankerInfo.m_maximumPoints = 1000;
			personalBankerInfo.m_pointsPerDollar = 10;

			//Test
			personalBankerInfo.m_pointsAvailable = personalBankerInfo.m_maximumPoints + 1;
			ASSERT_TRUE( personalBankerInfo.DisplayTransferPointsToGame()) << _T("DisplayTransferPointsToGame gt");
			personalBankerInfo.m_pointsAvailable = personalBankerInfo.m_maximumPoints + 0;
			ASSERT_TRUE( personalBankerInfo.DisplayTransferPointsToGame()) << _T("DisplayTransferPointsToGame eq");
			personalBankerInfo.m_pointsAvailable = personalBankerInfo.m_maximumPoints - 1;
			ASSERT_TRUE( personalBankerInfo.DisplayTransferPointsToGame()) << _T("DisplayTransferPointsToGame lt");
		}

		/// <summary>
		/// Functional test for:
		///     CPersonalBankerInfo::DisplayTransferPointsToGame()
		/// Verify that the result depends on the relative m_maximumPoints to m_pointsAvailable to m_pointsPerDollar values.
		/// </summary>
        TEST_F(PersonalBankerInfoUnitTests,DisplayTransferPointsToGame_MaxBalancePointsPerDollarTest)
		{
            DisplayTransferPointsToGame_MaxBalancePointsPerDollarTest();
        }
        void PersonalBankerInfoUnitTests::DisplayTransferPointsToGame_MaxBalancePointsPerDollarTest()
		{	// Setup.
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bTransferPointsToGameEnabled = true;
			personalBankerInfo.m_minimumPoints = 15;
			personalBankerInfo.m_pointsPerDollar = 10;
			personalBankerInfo.m_pointsAvailable = 300;

			//Test
			personalBankerInfo.m_maximumPoints = personalBankerInfo.m_pointsPerDollar * 2;
			ASSERT_TRUE( personalBankerInfo.DisplayTransferPointsToGame()) << _T("DisplayTransferPointsToGame *2");
			personalBankerInfo.m_maximumPoints = personalBankerInfo.m_pointsPerDollar * 2 - 1;
			ASSERT_FALSE( personalBankerInfo.DisplayTransferPointsToGame()) << _T("DisplayTransferPointsToGame *2 -1");
			personalBankerInfo.m_maximumPoints = personalBankerInfo.m_minimumPoints + 1;
			ASSERT_FALSE( personalBankerInfo.DisplayTransferPointsToGame()) << _T("DisplayTransferPointsToGame gt");
			personalBankerInfo.m_maximumPoints = personalBankerInfo.m_minimumPoints + 0;
			ASSERT_FALSE( personalBankerInfo.DisplayTransferPointsToGame()) << _T("DisplayTransferPointsToGame eq");
			personalBankerInfo.m_maximumPoints = personalBankerInfo.m_minimumPoints - 1;
			ASSERT_FALSE( personalBankerInfo.DisplayTransferPointsToGame()) << _T("DisplayTransferPointsToGame lt");
		}

		/// <summary>
		/// Functional test for:
		///     CPersonalBankerInfo::DisplayTransferPointsToGame()
		/// Verify that the result depends on the m_pointsAvailable value alone.
		/// </summary>
        TEST_F(PersonalBankerInfoUnitTests,DisplayTransferPointsToGame_BalanceTest)
		{
            DisplayTransferPointsToGame_BalanceTest();
        }
        void PersonalBankerInfoUnitTests::DisplayTransferPointsToGame_BalanceTest()
		{	// Setup.
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bTransferPointsToGameEnabled = true;
			personalBankerInfo.m_bEGMInitialized = true;
			personalBankerInfo.m_minimumPoints = 0;
			personalBankerInfo.m_maximumPoints = 1000;
			personalBankerInfo.m_pointsPerDollar = 10;

			//Test
			personalBankerInfo.m_pointsAvailable = personalBankerInfo.m_pointsPerDollar + 1;
			ASSERT_TRUE( personalBankerInfo.DisplayTransferPointsToGame()) << _T("DisplayTransferPointsToGame gt");
			personalBankerInfo.m_pointsAvailable = personalBankerInfo.m_pointsPerDollar + 0;
			ASSERT_TRUE( personalBankerInfo.DisplayTransferPointsToGame()) << _T("DisplayTransferPointsToGame eq");
			personalBankerInfo.m_pointsAvailable = personalBankerInfo.m_pointsPerDollar - 1;
			ASSERT_FALSE( personalBankerInfo.DisplayTransferPointsToGame()) << _T("DisplayTransferPointsToGame lt");
		}
// #pragma endregion DisplayTransferPointsToGame

// #pragma region DisplayTransferPromoToGame
        TEST_F(PersonalBankerInfoUnitTests,DisplayTransferPromoToGameTest)
		{
            DisplayTransferPromoToGameTest();
        }
        void PersonalBankerInfoUnitTests::DisplayTransferPromoToGameTest()
		{	// Setup.
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bTransferPromoDollarsToGameEnabled = true;
			personalBankerInfo.m_bEGMInitialized = true;
			personalBankerInfo.m_minimumPromoDown = 1;
			personalBankerInfo.m_maximumPromoDown = 1000;
			personalBankerInfo.m_promo2Balance = 300;

			//Test
			ASSERT_TRUE(personalBankerInfo.DisplayTransferPromoToGame()) << _T("FAILED DisplayTransferPromoToGame returned the wrong value");
		}

		/// <summary>
		/// Functional test for:
		///     CPersonalBankerInfo::DisplayTransferPromoToGame()
		/// Verify that the result depends on the IsBannedFromRedemptions() value.
		/// </summary>
        TEST_F(PersonalBankerInfoUnitTests,DisplayTransferPromoToGame_BannedFromRedemptionsTest)
		{
            DisplayTransferPromoToGame_BannedFromRedemptionsTest();
        }
        void PersonalBankerInfoUnitTests::DisplayTransferPromoToGame_BannedFromRedemptionsTest()
		{	// Setup.
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bTransferPromoDollarsToGameEnabled = true;
			personalBankerInfo.m_bEGMInitialized = true;
			personalBankerInfo.m_minimumPromoDown = 10;
			personalBankerInfo.m_maximumPromoDown = 100;
			personalBankerInfo.m_promo2Balance = 3000;

			//Test
			personalBankerInfo.m_pbFlags.flags.IsBannedFromRedemptions = 0;
			ASSERT_TRUE( personalBankerInfo.DisplayTransferPromoToGame()) << _T("DisplayTransferPromoToGame true");
			personalBankerInfo.m_pbFlags.flags.IsBannedFromRedemptions = 1;
			ASSERT_FALSE( personalBankerInfo.DisplayTransferPromoToGame()) << _T("DisplayTransferPromoToGame false");
		}

		/// <summary>
		/// Functional test for:
		///     CPersonalBankerInfo::DisplayTransferPromoToGame()
		/// Verify that the result depends on the m_bTransferPromoDollarsToGameEnabled value.
		/// </summary>
        TEST_F(PersonalBankerInfoUnitTests,DisplayTransferPromoToGame_TransferEnabledTest)
		{
            DisplayTransferPromoToGame_TransferEnabledTest();
        }
        void PersonalBankerInfoUnitTests::DisplayTransferPromoToGame_TransferEnabledTest()
		{	// Setup.
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bEGMInitialized = true;
			personalBankerInfo.m_minimumPromoDown = 10;
			personalBankerInfo.m_maximumPromoDown = 100;
			personalBankerInfo.m_promo2Balance = 3000;

			//Test
			personalBankerInfo.m_bTransferPromoDollarsToGameEnabled = true;
			ASSERT_TRUE( personalBankerInfo.DisplayTransferPromoToGame()) << _T("DisplayTransferPromoToGame true");
			personalBankerInfo.m_bTransferPromoDollarsToGameEnabled = false;
			ASSERT_FALSE( personalBankerInfo.DisplayTransferPromoToGame()) << _T("DisplayTransferPromoToGame false");
		}

		/// <summary>
		/// Functional test for:
		///     CPersonalBankerInfo::DisplayTransferPromoToGame()
		/// Verify that the result does not depend on the m_minimumPromoDown value alone.
		/// </summary>
        TEST_F(PersonalBankerInfoUnitTests,DisplayTransferPromoToGame_MinTest)
		{
            DisplayTransferPromoToGame_MinTest();
        }
        void PersonalBankerInfoUnitTests::DisplayTransferPromoToGame_MinTest()
		{	// Setup.
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bTransferPromoDollarsToGameEnabled = true;
			personalBankerInfo.m_bEGMInitialized = true;
			personalBankerInfo.m_maximumPromoDown = 100;
			personalBankerInfo.m_promo2Balance = 3000;

			//Test
			personalBankerInfo.m_minimumPromoDown = 2;
			ASSERT_TRUE( personalBankerInfo.DisplayTransferPromoToGame()) << _T("DisplayTransferPromoToGame 2");
			personalBankerInfo.m_minimumPromoDown = 1;
			ASSERT_TRUE( personalBankerInfo.DisplayTransferPromoToGame()) << _T("DisplayTransferPromoToGame 1");
			personalBankerInfo.m_minimumPromoDown = 0;
			ASSERT_TRUE( personalBankerInfo.DisplayTransferPromoToGame()) << _T("DisplayTransferPromoToGame 0");
		}

		/// <summary>
		/// Functional test for:
		///     CPersonalBankerInfo::DisplayTransferPromoToGame()
		/// Verify that the result depends on the m_maximumPromoDown value.
		/// </summary>
        TEST_F(PersonalBankerInfoUnitTests,DisplayTransferPromoToGame_MaxTest)
		{
            DisplayTransferPromoToGame_MaxTest();
        }
        void PersonalBankerInfoUnitTests::DisplayTransferPromoToGame_MaxTest()
		{	// Setup.
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bTransferPromoDollarsToGameEnabled = true;
			personalBankerInfo.m_bEGMInitialized = true;
			personalBankerInfo.m_minimumPromoDown = 0;
			personalBankerInfo.m_promo2Balance = 3000;

			//Test
			personalBankerInfo.m_maximumPromoDown = 2;
			ASSERT_TRUE( personalBankerInfo.DisplayTransferPromoToGame()) << _T("DisplayTransferPromoToGame 2");
			personalBankerInfo.m_maximumPromoDown = 1;
			ASSERT_TRUE( personalBankerInfo.DisplayTransferPromoToGame()) << _T("DisplayTransferPromoToGame 1");
			personalBankerInfo.m_maximumPromoDown = 0;
			ASSERT_FALSE( personalBankerInfo.DisplayTransferPromoToGame()) << _T("DisplayTransferPromoToGame 0");
		}

		/// <summary>
		/// Functional test for:
		///     CPersonalBankerInfo::DisplayTransferPromoToGame()
		/// Verify that the result depends on the relative m_minimumPromoDown to m_maximumPromoDown values.
		/// </summary>
        TEST_F(PersonalBankerInfoUnitTests,DisplayTransferPromoToGame_MinMaxTest)
		{
            DisplayTransferPromoToGame_MinMaxTest();
        }
        void PersonalBankerInfoUnitTests::DisplayTransferPromoToGame_MinMaxTest()
		{	// Setup.
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bTransferPromoDollarsToGameEnabled = true;
			personalBankerInfo.m_bEGMInitialized = true;
			personalBankerInfo.m_minimumPromoDown = 10;
			personalBankerInfo.m_promo2Balance = 3000;

			//Test
			personalBankerInfo.m_maximumPromoDown = personalBankerInfo.m_minimumPromoDown + 1;
			ASSERT_TRUE( personalBankerInfo.DisplayTransferPromoToGame()) << _T("DisplayTransferPromoToGame gt");
			personalBankerInfo.m_maximumPromoDown = personalBankerInfo.m_minimumPromoDown;
			ASSERT_TRUE( personalBankerInfo.DisplayTransferPromoToGame()) << _T("DisplayTransferPromoToGame eq");
			personalBankerInfo.m_maximumPromoDown = personalBankerInfo.m_minimumPromoDown - 1;
			ASSERT_FALSE( personalBankerInfo.DisplayTransferPromoToGame()) << _T("DisplayTransferPromoToGame lt");
		}

		/// <summary>
		/// Functional test for:
		///     CPersonalBankerInfo::DisplayTransferPromoToGame()
		/// Verify that the result depends on the relative m_minimumPromoDown to m_promo2Balance values.
		/// </summary>
        TEST_F(PersonalBankerInfoUnitTests,DisplayTransferPromoToGame_MinBalanceTest)
		{
            DisplayTransferPromoToGame_MinBalanceTest();
        }
        void PersonalBankerInfoUnitTests::DisplayTransferPromoToGame_MinBalanceTest()
		{	// Setup.
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bTransferPromoDollarsToGameEnabled = true;
			personalBankerInfo.m_bEGMInitialized = true;
			personalBankerInfo.m_minimumPromoDown = 10;
			personalBankerInfo.m_maximumPromoDown = 100;
			ASSERT_EQ((DWORD)100, ONE_DOLLAR_IN_CENTS) << _T("ONE_DOLLAR_IN_CENTS");

			//Test
			personalBankerInfo.m_promo2Balance = (personalBankerInfo.m_minimumPromoDown + 1) * ONE_DOLLAR_IN_CENTS;
			ASSERT_TRUE( personalBankerInfo.DisplayTransferPromoToGame()) << _T("DisplayTransferPromoToGame gt");
			personalBankerInfo.m_promo2Balance = (personalBankerInfo.m_minimumPromoDown + 0) * ONE_DOLLAR_IN_CENTS;
			ASSERT_TRUE( personalBankerInfo.DisplayTransferPromoToGame()) << _T("DisplayTransferPromoToGame eq");
			personalBankerInfo.m_promo2Balance = (personalBankerInfo.m_minimumPromoDown - 1) * ONE_DOLLAR_IN_CENTS;
			ASSERT_FALSE( personalBankerInfo.DisplayTransferPromoToGame()) << _T("DisplayTransferPromoToGame lt");
		}

		/// <summary>
		/// Functional test for:
		///     CPersonalBankerInfo::DisplayTransferPromoToGame()
		/// Verify that the result does not depend on the relative m_maximumPromoDown to m_promo2Balance values.
		/// </summary>
        TEST_F(PersonalBankerInfoUnitTests,DisplayTransferPromoToGame_MaxBalanceTest)
		{
            DisplayTransferPromoToGame_MaxBalanceTest();
        }
        void PersonalBankerInfoUnitTests::DisplayTransferPromoToGame_MaxBalanceTest()
		{	// Setup.
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bTransferPromoDollarsToGameEnabled = true;
			personalBankerInfo.m_bEGMInitialized = true;
			personalBankerInfo.m_minimumPromoDown = 10;
			personalBankerInfo.m_maximumPromoDown = 100;
			ASSERT_EQ((DWORD)100, ONE_DOLLAR_IN_CENTS) << _T("ONE_DOLLAR_IN_CENTS");

			//Test
			personalBankerInfo.m_promo2Balance = (personalBankerInfo.m_maximumPromoDown + 1) * ONE_DOLLAR_IN_CENTS;
			ASSERT_TRUE( personalBankerInfo.DisplayTransferPromoToGame()) << _T("DisplayTransferPromoToGame gt");
			personalBankerInfo.m_promo2Balance = (personalBankerInfo.m_maximumPromoDown + 0) * ONE_DOLLAR_IN_CENTS;
			ASSERT_TRUE( personalBankerInfo.DisplayTransferPromoToGame()) << _T("DisplayTransferPromoToGame eq");
			personalBankerInfo.m_promo2Balance = (personalBankerInfo.m_maximumPromoDown - 1) * ONE_DOLLAR_IN_CENTS;
			ASSERT_TRUE( personalBankerInfo.DisplayTransferPromoToGame()) << _T("DisplayTransferPromoToGame lt");
		}

		/// <summary>
		/// Functional test for:
		///     CPersonalBankerInfo::DisplayTransferPromoToGame()
		/// Verify that the result depends on the m_promo2Balance value alone.
		/// </summary>
        TEST_F(PersonalBankerInfoUnitTests,DisplayTransferPromoToGame_BalanceTest)
		{
            DisplayTransferPromoToGame_BalanceTest();
        }
        void PersonalBankerInfoUnitTests::DisplayTransferPromoToGame_BalanceTest()
		{	// Setup.
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bTransferPromoDollarsToGameEnabled = true;
			personalBankerInfo.m_bEGMInitialized = true;
			personalBankerInfo.m_minimumPromoDown = 0;
			personalBankerInfo.m_maximumPromoDown = 100;

			//Test
			personalBankerInfo.m_promo2Balance = ONE_DOLLAR_IN_CENTS + 1;
			ASSERT_TRUE( personalBankerInfo.DisplayTransferPromoToGame()) << _T("DisplayTransferPromoToGame gt");
			personalBankerInfo.m_promo2Balance = ONE_DOLLAR_IN_CENTS + 0;
			ASSERT_TRUE( personalBankerInfo.DisplayTransferPromoToGame()) << _T("DisplayTransferPromoToGame eq");
			personalBankerInfo.m_promo2Balance = ONE_DOLLAR_IN_CENTS - 1;
			ASSERT_FALSE( personalBankerInfo.DisplayTransferPromoToGame()) << _T("DisplayTransferPromoToGame lt");
		}
// #pragma endregion DisplayTransferPromoToGame

// #pragma region GetTransferToGameScreen
        TEST_F(PersonalBankerInfoUnitTests,GetTransferToGameScreenWithAllEnabledTest)
		{
            GetTransferToGameScreenWithAllEnabledTest();
        }
        void PersonalBankerInfoUnitTests::GetTransferToGameScreenWithAllEnabledTest()
		{	// Setup.
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bTransferCashToGameEnabled = true;
			personalBankerInfo.m_bEGMInitialized = true;
			personalBankerInfo.m_minimumCashDown = 1;
			personalBankerInfo.m_maximumCashDown = 1000;
			personalBankerInfo.m_cashBalance = 300;
			personalBankerInfo.m_bTransferPointsToGameEnabled = true;
			personalBankerInfo.m_minimumPoints = 100;
			personalBankerInfo.m_maximumPoints = 1000;
			personalBankerInfo.m_pointsPerDollar = 100;
			personalBankerInfo.m_pointsAvailable = 300;
			personalBankerInfo.m_bTransferPromoDollarsToGameEnabled = true;
			personalBankerInfo.m_minimumPromoDown = 1;
			personalBankerInfo.m_maximumPromoDown = 1000;
			personalBankerInfo.m_promo2Balance = 300;

			//Test
			ASSERT_TRUE(screenPlayerPBToGame == personalBankerInfo.GetTransferToGameScreen()) << _T("FAILED GetTransferToGameScreen returned the wrong value");
		}

        TEST_F(PersonalBankerInfoUnitTests,GetTransferToGameScreenWithCashPointsEnabledTest)
		{
            GetTransferToGameScreenWithCashPointsEnabledTest();
        }
        void PersonalBankerInfoUnitTests::GetTransferToGameScreenWithCashPointsEnabledTest()
		{	// Setup.
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bTransferCashToGameEnabled = true;
			personalBankerInfo.m_bEGMInitialized = true;
			personalBankerInfo.m_minimumCashDown = 1;
			personalBankerInfo.m_maximumCashDown = 1000;
			personalBankerInfo.m_cashBalance = 300;
			personalBankerInfo.m_bTransferPointsToGameEnabled = true;
			personalBankerInfo.m_minimumPoints = 100;
			personalBankerInfo.m_maximumPoints = 1000;
			personalBankerInfo.m_pointsPerDollar = 100;
			personalBankerInfo.m_pointsAvailable = 300;
			personalBankerInfo.m_bTransferPromoDollarsToGameEnabled = false;

			//Test
			ASSERT_TRUE(screenPlayerPBToGameCashPoints == personalBankerInfo.GetTransferToGameScreen()) << _T("FAILED GetTransferToGameScreen returned the wrong value");
		}

        TEST_F(PersonalBankerInfoUnitTests,GetTransferToGameScreenWithCashPromoEnabledTest)
		{
            GetTransferToGameScreenWithCashPromoEnabledTest();
        }
        void PersonalBankerInfoUnitTests::GetTransferToGameScreenWithCashPromoEnabledTest()
		{	// Setup.
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bTransferCashToGameEnabled = true;
			personalBankerInfo.m_bEGMInitialized = true;
			personalBankerInfo.m_minimumCashDown = 1;
			personalBankerInfo.m_maximumCashDown = 1000;
			personalBankerInfo.m_cashBalance = 300;
			personalBankerInfo.m_bTransferPointsToGameEnabled = false;
			personalBankerInfo.m_bTransferPromoDollarsToGameEnabled = true;
			personalBankerInfo.m_minimumPromoDown = 1;
			personalBankerInfo.m_maximumPromoDown = 1000;
			personalBankerInfo.m_promo2Balance = 300;

			//Test
			ASSERT_TRUE(screenPlayerPBToGameCashPromo == personalBankerInfo.GetTransferToGameScreen()) << _T("FAILED GetTransferToGameScreen returned the wrong value");
		}

		
        TEST_F(PersonalBankerInfoUnitTests,GetTransferToGameScreenWithPointsPromoEnabledTest)
		{
            GetTransferToGameScreenWithPointsPromoEnabledTest();
        }
        void PersonalBankerInfoUnitTests::GetTransferToGameScreenWithPointsPromoEnabledTest()
		{	// Setup.
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bTransferCashToGameEnabled = false;
			personalBankerInfo.m_bEGMInitialized = true;
			personalBankerInfo.m_bTransferPointsToGameEnabled = true;
			personalBankerInfo.m_minimumPoints = 100;
			personalBankerInfo.m_maximumPoints = 1000;
			personalBankerInfo.m_pointsPerDollar = 100;
			personalBankerInfo.m_pointsAvailable = 300;
			personalBankerInfo.m_bTransferPromoDollarsToGameEnabled = true;
			personalBankerInfo.m_minimumPromoDown = 1;
			personalBankerInfo.m_maximumPromoDown = 1000;
			personalBankerInfo.m_promo2Balance = 300;

			//Test
			ASSERT_TRUE(screenPlayerPBToGamePointsPromo == personalBankerInfo.GetTransferToGameScreen()) << _T("FAILED GetTransferToGameScreen returned the wrong value");
		}

        TEST_F(PersonalBankerInfoUnitTests,GetTransferToGameScreenWithCashEnabledTest)
		{
            GetTransferToGameScreenWithCashEnabledTest();
        }
        void PersonalBankerInfoUnitTests::GetTransferToGameScreenWithCashEnabledTest()
		{	// Setup.
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bTransferCashToGameEnabled = true;
			personalBankerInfo.m_bEGMInitialized = true;
			personalBankerInfo.m_minimumCashDown = 1;
			personalBankerInfo.m_maximumCashDown = 1000;
			personalBankerInfo.m_cashBalance = 300;

			//Test
			ASSERT_TRUE(screenPlayerPBToGameCash == personalBankerInfo.GetTransferToGameScreen()) << _T("FAILED GetTransferToGameScreen returned the wrong value");
		}

        TEST_F(PersonalBankerInfoUnitTests,GetTransferToGameScreenWithPointsEnabledTest)
		{
            GetTransferToGameScreenWithPointsEnabledTest();
        }
        void PersonalBankerInfoUnitTests::GetTransferToGameScreenWithPointsEnabledTest()
		{	// Setup.
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bEGMInitialized = true;
			personalBankerInfo.m_bTransferPointsToGameEnabled = true;
			personalBankerInfo.m_minimumPoints = 100;
			personalBankerInfo.m_maximumPoints = 1000;
			personalBankerInfo.m_pointsPerDollar = 100;
			personalBankerInfo.m_pointsAvailable = 300;

			//Test
			ASSERT_TRUE(screenPlayerPBToGamePoints == personalBankerInfo.GetTransferToGameScreen()) << _T("FAILED GetTransferToGameScreen returned the wrong value");
		}

        TEST_F(PersonalBankerInfoUnitTests,GetTransferToGameScreenWithPromoEnabledTest)
		{
            GetTransferToGameScreenWithPromoEnabledTest();
        }
        void PersonalBankerInfoUnitTests::GetTransferToGameScreenWithPromoEnabledTest()
		{	// Setup.
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bEGMInitialized = true;
			personalBankerInfo.m_bTransferPromoDollarsToGameEnabled = true;
			personalBankerInfo.m_minimumPromoDown = 1;
			personalBankerInfo.m_maximumPromoDown = 1000;
			personalBankerInfo.m_promo2Balance = 300;

			//Test
			ASSERT_TRUE(screenPlayerPBToGamePromo == personalBankerInfo.GetTransferToGameScreen()) << _T("FAILED GetTransferToGameScreen returned the wrong value");
		}

        TEST_F(PersonalBankerInfoUnitTests,GetTransferToGameScreenWithNoneEnabledTest)
		{
            GetTransferToGameScreenWithNoneEnabledTest();
        }
        void PersonalBankerInfoUnitTests::GetTransferToGameScreenWithNoneEnabledTest()
		{	// Setup.
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bEGMInitialized = true;

			//Test
			ASSERT_TRUE(screenPlayerPBTNone == personalBankerInfo.GetTransferToGameScreen()) << _T("FAILED GetTransferToGameScreen returned the wrong value");
		}
// #pragma endregion GetTransferToGameScreen

        TEST_F(PersonalBankerInfoUnitTests,DisplayTransferCashFromGameTest)
		{
            DisplayTransferCashFromGameTest();
        }
        void PersonalBankerInfoUnitTests::DisplayTransferCashFromGameTest()
		{	//Arrange
			bool actual = false;
			bool expected = true;
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bTransferCashFromGameEnabled = true;
			personalBankerInfo.m_bEGMInitialized = true;
			personalBankerInfo.m_cashBalanceOnGame = 500;
			personalBankerInfo.m_maximumCashUp = 10;

			//Act
			actual = personalBankerInfo.DisplayTransferCashFromGame();

			//Assert
			ASSERT_TRUE(actual == expected) << _T("FAILED DisplayTransferCashFromGame returned the wrong value");
		}

        TEST_F(PersonalBankerInfoUnitTests,DisplayTransferCashFromGameTest_Max_Cash_Not_Enough)
		{
            DisplayTransferCashFromGameTest_Max_Cash_Not_Enough();
        }
        void PersonalBankerInfoUnitTests::DisplayTransferCashFromGameTest_Max_Cash_Not_Enough()
		{	//Arrange
			bool actual = false;
			bool expected = false;
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bTransferCashFromGameEnabled = true;
			personalBankerInfo.m_bEGMInitialized = true;
			personalBankerInfo.m_cashBalanceOnGame = 500;
			personalBankerInfo.m_maximumCashUp = 4;

			//Act
			actual = personalBankerInfo.DisplayTransferCashFromGame();

			//Assert
			ASSERT_TRUE(actual == expected) << _T("FAILED DisplayTransferCashFromGame returned the wrong value");
		}

        TEST_F(PersonalBankerInfoUnitTests,DisplayTransferCashFromGameTest_Transfer_Cash_From_Game_Not_Enabled)
		{
            DisplayTransferCashFromGameTest_Transfer_Cash_From_Game_Not_Enabled();
        }
        void PersonalBankerInfoUnitTests::DisplayTransferCashFromGameTest_Transfer_Cash_From_Game_Not_Enabled()
		{	//Arrange
			bool actual = false;
			bool expected = false;
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bTransferCashFromGameEnabled = false;
			personalBankerInfo.m_bEGMInitialized = true;
			personalBankerInfo.m_cashBalanceOnGame = 500;
			personalBankerInfo.m_maximumCashUp = 4;

			//Act
			actual = personalBankerInfo.DisplayTransferCashFromGame();

			//Assert
			ASSERT_TRUE(actual == expected) << _T("FAILED DisplayTransferCashFromGame returned the wrong value");
		}

        TEST_F(PersonalBankerInfoUnitTests,DisplayTransferCashFromGameTest_Min_Cash_Not_Enough)
		{
            DisplayTransferCashFromGameTest_Min_Cash_Not_Enough();
        }
        void PersonalBankerInfoUnitTests::DisplayTransferCashFromGameTest_Min_Cash_Not_Enough()
		{	//Arrange
			bool actual = false;
			bool expected = false;
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bTransferCashFromGameEnabled = true;
			personalBankerInfo.m_bEGMInitialized = true;
			personalBankerInfo.m_cashBalanceOnGame = 500;
			personalBankerInfo.m_maximumCashUp = 5;
			personalBankerInfo.m_minimumCashUp = 10;

			//Act
			actual = personalBankerInfo.DisplayTransferCashFromGame();

			//Assert
			ASSERT_TRUE(actual == expected) << _T("FAILED DisplayTransferCashFromGame returned the wrong value");
		}

        TEST_F(PersonalBankerInfoUnitTests,DisplayTransferCashFromGameTest_No_Cash_Balance_On_Game)
		{
            DisplayTransferCashFromGameTest_No_Cash_Balance_On_Game();
        }
        void PersonalBankerInfoUnitTests::DisplayTransferCashFromGameTest_No_Cash_Balance_On_Game()
		{	//Arrange
			bool actual = false;
			bool expected = false;
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bTransferCashFromGameEnabled = true;
			personalBankerInfo.m_bEGMInitialized = true;
			personalBankerInfo.m_cashBalanceOnGame = 0;
			personalBankerInfo.m_maximumCashUp = 5;
			personalBankerInfo.m_minimumCashUp = 0;

			//Act
			actual = personalBankerInfo.DisplayTransferCashFromGame();

			//Assert
			ASSERT_TRUE(actual == expected) << _T("FAILED DisplayTransferCashFromGame returned the wrong value");
		}

        TEST_F(PersonalBankerInfoUnitTests,DisplayTransferPromoFromGameTest)
		{
            DisplayTransferPromoFromGameTest();
        }
        void PersonalBankerInfoUnitTests::DisplayTransferPromoFromGameTest()
        {
			//Arrange
			bool actual = false;
			bool expected = true;
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bTransferPromoFromGameEnabled = true;
			personalBankerInfo.m_bEGMInitialized = true;
			personalBankerInfo.m_promoBalanceOnGame = 500;
			personalBankerInfo.m_maximumPromoUp = 5;

			//Act
			actual = personalBankerInfo.DisplayTransferPromoFromGame();

			//Assert
			ASSERT_TRUE(actual == expected) << _T("FAILED DisplayTransferPromoFromGame returned the wrong value");
		}

        TEST_F(PersonalBankerInfoUnitTests,DisplayTransferCashFromGameTest_Max_Promo_To_Much)
		{
            DisplayTransferCashFromGameTest_Max_Promo_To_Much();
        }
        void PersonalBankerInfoUnitTests::DisplayTransferCashFromGameTest_Max_Promo_To_Much()
		{	//Arrange
			bool actual = false;
			bool expected = false;
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bTransferPromoFromGameEnabled = true;
			personalBankerInfo.m_bEGMInitialized = true;
			personalBankerInfo.m_promoBalanceOnGame = 500;
			personalBankerInfo.m_maximumPromoUp = 4;

			//Act
			actual = personalBankerInfo.DisplayTransferPromoFromGame();

			//Assert
			ASSERT_TRUE(actual == expected) << _T("FAILED DisplayTransferPromoFromGame returned the wrong value");
		}

        TEST_F(PersonalBankerInfoUnitTests,DisplayTransferCashFromGameTest_Transfer_Promo_From_Game_Not_Enabled)
		{
            DisplayTransferCashFromGameTest_Transfer_Promo_From_Game_Not_Enabled();
        }
        void PersonalBankerInfoUnitTests::DisplayTransferCashFromGameTest_Transfer_Promo_From_Game_Not_Enabled()
		{	//Arrange
			bool actual = false;
			bool expected = false;
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bTransferPromoFromGameEnabled = false;
			personalBankerInfo.m_bEGMInitialized = true;
			personalBankerInfo.m_promoBalanceOnGame = 500;
			personalBankerInfo.m_maximumPromoUp = 4;

			//Act
			actual = personalBankerInfo.DisplayTransferPromoFromGame();

			//Assert
			ASSERT_TRUE(actual == expected) << _T("FAILED DisplayTransferPromoFromGame returned the wrong value");
		}

        TEST_F(PersonalBankerInfoUnitTests,DisplayTransferCashFromGameTest_Min_Promo_Not_Enough)
		{
            DisplayTransferCashFromGameTest_Min_Promo_Not_Enough();
        }
        void PersonalBankerInfoUnitTests::DisplayTransferCashFromGameTest_Min_Promo_Not_Enough()
		{	//Arrange
			bool actual = false;
			bool expected = false;
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bTransferPromoFromGameEnabled = true;
			personalBankerInfo.m_bEGMInitialized = true;
			personalBankerInfo.m_promoBalanceOnGame = 500;
			personalBankerInfo.m_maximumPromoUp = 4;
			personalBankerInfo.m_minimumPromoUp = 6;

			//Act
			actual = personalBankerInfo.DisplayTransferPromoFromGame();

			//Assert
			ASSERT_TRUE(actual == expected) << _T("FAILED DisplayTransferPromoFromGame returned the wrong value");
		}

        TEST_F(PersonalBankerInfoUnitTests,DisplayTransferCashFromGameTest_No_Promo_Balance_On_Game)
		{
            DisplayTransferCashFromGameTest_No_Promo_Balance_On_Game();
        }
        void PersonalBankerInfoUnitTests::DisplayTransferCashFromGameTest_No_Promo_Balance_On_Game()
		{	//Arrange
			bool actual = false;
			bool expected = false;
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bTransferPromoFromGameEnabled = true;
			personalBankerInfo.m_bEGMInitialized = true;
			personalBankerInfo.m_promoBalanceOnGame = 0;
			personalBankerInfo.m_maximumPromoUp = 4;
			personalBankerInfo.m_minimumPromoUp = 0;

			//Act
			actual = personalBankerInfo.DisplayTransferPromoFromGame();

			//Assert
			ASSERT_TRUE(actual == expected) << _T("FAILED DisplayTransferPromoFromGame returned the wrong value");
		}

// #pragma region SetShowFromGameAndToGameButtonFlags
        TEST_F(PersonalBankerInfoUnitTests,SetShowToGameButtonFlag_WithAllEnabledTest)
		{
            SetShowToGameButtonFlag_WithAllEnabledTest();
        }
        void PersonalBankerInfoUnitTests::SetShowToGameButtonFlag_WithAllEnabledTest()
		{	// Setup.
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bTransferCashToGameEnabled = true;
			personalBankerInfo.m_bEGMInitialized = true;
			personalBankerInfo.m_minimumCashDown = 1;
			personalBankerInfo.m_maximumCashDown = 1000;
			personalBankerInfo.m_cashBalance = 300;
			personalBankerInfo.m_bTransferPointsToGameEnabled = true;
			personalBankerInfo.m_minimumPoints = 100;
			personalBankerInfo.m_maximumPoints = 1000;
			personalBankerInfo.m_pointsPerDollar = 100;
			personalBankerInfo.m_pointsAvailable = 300;
			personalBankerInfo.m_bTransferPromoDollarsToGameEnabled = true;
			personalBankerInfo.m_minimumPromoDown = 1;
			personalBankerInfo.m_maximumPromoDown = 1000;
			personalBankerInfo.m_promo2Balance = 300;

			// Perform operation(s) and test(s).
			personalBankerInfo.SetShowFromGameAndToGameButtonFlags();
			ASSERT_TRUE( personalBankerInfo.m_showToGameButton) << _T("SetShowFromGameAndToGameButtonFlags m_showToGameButton");
		}

        TEST_F(PersonalBankerInfoUnitTests,SetShowToGameButtonFlag_WithCashPointsEnabledTest)
		{
            SetShowToGameButtonFlag_WithCashPointsEnabledTest();
        }
        void PersonalBankerInfoUnitTests::SetShowToGameButtonFlag_WithCashPointsEnabledTest()
		{	// Setup.
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bTransferCashToGameEnabled = true;
			personalBankerInfo.m_bEGMInitialized = true;
			personalBankerInfo.m_minimumCashDown = 1;
			personalBankerInfo.m_maximumCashDown = 1000;
			personalBankerInfo.m_cashBalance = 300;
			personalBankerInfo.m_bTransferPointsToGameEnabled = true;
			personalBankerInfo.m_minimumPoints = 100;
			personalBankerInfo.m_maximumPoints = 1000;
			personalBankerInfo.m_pointsPerDollar = 100;
			personalBankerInfo.m_pointsAvailable = 300;
			personalBankerInfo.m_bTransferPromoDollarsToGameEnabled = false;

			// Perform operation(s) and test(s).
			personalBankerInfo.SetShowFromGameAndToGameButtonFlags();
			ASSERT_TRUE( personalBankerInfo.m_showToGameButton) << _T("SetShowFromGameAndToGameButtonFlags m_showToGameButton");
		}

        TEST_F(PersonalBankerInfoUnitTests,SetShowToGameButtonFlag_WithCashPromoEnabledTest)
		{
            SetShowToGameButtonFlag_WithCashPromoEnabledTest();
        }
        void PersonalBankerInfoUnitTests::SetShowToGameButtonFlag_WithCashPromoEnabledTest()
		{	// Setup.
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bTransferCashToGameEnabled = true;
			personalBankerInfo.m_bEGMInitialized = true;
			personalBankerInfo.m_minimumCashDown = 1;
			personalBankerInfo.m_maximumCashDown = 1000;
			personalBankerInfo.m_cashBalance = 300;
			personalBankerInfo.m_bTransferPointsToGameEnabled = false;
			personalBankerInfo.m_bTransferPromoDollarsToGameEnabled = true;
			personalBankerInfo.m_minimumPromoDown = 1;
			personalBankerInfo.m_maximumPromoDown = 1000;
			personalBankerInfo.m_promo2Balance = 300;

			// Perform operation(s) and test(s).
			personalBankerInfo.SetShowFromGameAndToGameButtonFlags();
			ASSERT_TRUE( personalBankerInfo.m_showToGameButton) << _T("SetShowFromGameAndToGameButtonFlags m_showToGameButton");
		}


        TEST_F(PersonalBankerInfoUnitTests,SetShowToGameButtonFlag_WithPointsPromoEnabledTest)
		{
            SetShowToGameButtonFlag_WithPointsPromoEnabledTest();
        }
        void PersonalBankerInfoUnitTests::SetShowToGameButtonFlag_WithPointsPromoEnabledTest()
		{	// Setup.
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bTransferCashToGameEnabled = false;
			personalBankerInfo.m_bEGMInitialized = true;
			personalBankerInfo.m_bTransferPointsToGameEnabled = true;
			personalBankerInfo.m_minimumPoints = 100;
			personalBankerInfo.m_maximumPoints = 1000;
			personalBankerInfo.m_pointsPerDollar = 100;
			personalBankerInfo.m_pointsAvailable = 300;
			personalBankerInfo.m_bTransferPromoDollarsToGameEnabled = true;
			personalBankerInfo.m_minimumPromoDown = 1;
			personalBankerInfo.m_maximumPromoDown = 1000;
			personalBankerInfo.m_promo2Balance = 300;

			// Perform operation(s) and test(s).
			personalBankerInfo.SetShowFromGameAndToGameButtonFlags();
			ASSERT_TRUE( personalBankerInfo.m_showToGameButton) << _T("SetShowFromGameAndToGameButtonFlags m_showToGameButton");
		}

        TEST_F(PersonalBankerInfoUnitTests,SetShowToGameButtonFlag_WithCashEnabledTest)
		{
                SetShowToGameButtonFlag_WithCashEnabledTest();
        }
        void PersonalBankerInfoUnitTests::SetShowToGameButtonFlag_WithCashEnabledTest()
		{	// Setup.
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bTransferCashToGameEnabled = true;
			personalBankerInfo.m_bEGMInitialized = true;
			personalBankerInfo.m_minimumCashDown = 1;
			personalBankerInfo.m_maximumCashDown = 1000;
			personalBankerInfo.m_cashBalance = 300;

			// Perform operation(s) and test(s).
			personalBankerInfo.SetShowFromGameAndToGameButtonFlags();
			ASSERT_TRUE( personalBankerInfo.m_showToGameButton) << _T("SetShowFromGameAndToGameButtonFlags m_showToGameButton");
		}

        TEST_F(PersonalBankerInfoUnitTests,SetShowToGameButtonFlag_WithPointsEnabledTest)
		{
                SetShowToGameButtonFlag_WithPointsEnabledTest();
        }
        void PersonalBankerInfoUnitTests::SetShowToGameButtonFlag_WithPointsEnabledTest()
		{	// Setup.
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bEGMInitialized = true;
			personalBankerInfo.m_bTransferPointsToGameEnabled = true;
			personalBankerInfo.m_minimumPoints = 100;
			personalBankerInfo.m_maximumPoints = 1000;
			personalBankerInfo.m_pointsPerDollar = 100;
			personalBankerInfo.m_pointsAvailable = 300;

			// Perform operation(s) and test(s).
			personalBankerInfo.SetShowFromGameAndToGameButtonFlags();
			ASSERT_TRUE( personalBankerInfo.m_showToGameButton) << _T("SetShowFromGameAndToGameButtonFlags m_showToGameButton");
		}

        TEST_F(PersonalBankerInfoUnitTests,SetShowToGameButtonFlag_WithPromoEnabledTest)
		{
            SetShowToGameButtonFlag_WithPromoEnabledTest();
        }
        void PersonalBankerInfoUnitTests::SetShowToGameButtonFlag_WithPromoEnabledTest()
		{	// Setup.
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bEGMInitialized = true;
			personalBankerInfo.m_bTransferPromoDollarsToGameEnabled = true;
			personalBankerInfo.m_minimumPromoDown = 1;
			personalBankerInfo.m_maximumPromoDown = 1000;
			personalBankerInfo.m_promo2Balance = 300;

			// Perform operation(s) and test(s).
			personalBankerInfo.SetShowFromGameAndToGameButtonFlags();
			ASSERT_TRUE( personalBankerInfo.m_showToGameButton) << _T("SetShowFromGameAndToGameButtonFlags m_showToGameButton");
		}

        TEST_F(PersonalBankerInfoUnitTests,SetShowToGameButtonFlag_WithNoneEnabledTest)
		{
            SetShowToGameButtonFlag_WithNoneEnabledTest();
        }
        void PersonalBankerInfoUnitTests::SetShowToGameButtonFlag_WithNoneEnabledTest()
		{	// Setup.
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_bEGMInitialized = true;

			// Perform operation(s) and test(s).
			personalBankerInfo.SetShowFromGameAndToGameButtonFlags();
			ASSERT_FALSE( personalBankerInfo.m_showToGameButton) << _T("SetShowFromGameAndToGameButtonFlags m_showToGameButton");
		}
// #pragma endregion SetShowFromGameAndToGameButtonFlags

        TEST_F(PersonalBankerInfoUnitTests,GetPersonalBankerMainScreenTest)
		{
            GetPersonalBankerMainScreenTest();
        }
        void PersonalBankerInfoUnitTests::GetPersonalBankerMainScreenTest()
		{	CPersonalBankerInfo personalBankerInfo;

			//Tests

			//#1
			personalBankerInfo.m_showToGameButton = true, personalBankerInfo.m_showFromGameButton = true;
			ASSERT_TRUE(screenPlayerPBMain == personalBankerInfo.GetPersonalBankerMainScreen()) <<
				_T("FAILED GetPersonalBankerMainScreen returned the wrong value");

			//#2
			personalBankerInfo.m_showToGameButton = true, personalBankerInfo.m_showFromGameButton = false;
			ASSERT_TRUE(screenPlayerPBHomeWithToGame == personalBankerInfo.GetPersonalBankerMainScreen()) <<
				_T("FAILED GetPersonalBankerMainScreen returned the wrong value");

			//#3
			personalBankerInfo.m_showToGameButton = false, personalBankerInfo.m_showFromGameButton = true;
			ASSERT_TRUE(screenPlayerPBHomeWithFromGame == personalBankerInfo.GetPersonalBankerMainScreen()) <<
				_T("FAILED GetPersonalBankerMainScreen returned the wrong value");

			//#4
			personalBankerInfo.m_showToGameButton = false, personalBankerInfo.m_showFromGameButton = false;
			ASSERT_TRUE(screenPlayerPBHomeWithBalance == personalBankerInfo.GetPersonalBankerMainScreen()) <<
				_T("FAILED GetPersonalBankerMainScreen returned the wrong value");
		}

        TEST_F(PersonalBankerInfoUnitTests,GetCreditTransferScreenTest)
		{
            GetCreditTransferScreenTest();
        }
        void PersonalBankerInfoUnitTests::GetCreditTransferScreenTest()
		{	CPersonalBankerInfo personalBankerInfo;

			//Tests

			//#1
			personalBankerInfo.m_showToGameButton = true, personalBankerInfo.m_showFromGameButton = true;
			ASSERT_TRUE(screenPlayerPBCreditTransfersWithFromToGame == personalBankerInfo.GetCreditTransferScreen()) <<
				_T("FAILED GetCreditTransferScreen returned the wrong value");

			//#2
			personalBankerInfo.m_showToGameButton = true, personalBankerInfo.m_showFromGameButton = false;
			ASSERT_TRUE(screenPlayerPBCreditTransfersWithToGame == personalBankerInfo.GetCreditTransferScreen()) <<
				_T("FAILED GetCreditTransferScreen returned the wrong value");

			//#3
			personalBankerInfo.m_showToGameButton = false, personalBankerInfo.m_showFromGameButton = true;
			ASSERT_TRUE(screenPlayerPBCreditTransfersWithFromGame == personalBankerInfo.GetCreditTransferScreen()) <<
				_T("FAILED GetCreditTransferScreen returned the wrong value");

			//#4
			personalBankerInfo.m_showToGameButton = false, personalBankerInfo.m_showFromGameButton = false;
			ASSERT_TRUE(screenPlayerPBCreditTransfersWithOutGame == personalBankerInfo.GetCreditTransferScreen()) <<
				_T("FAILED GetCreditTransferScreen returned the wrong value");
		}

        TEST_F(PersonalBankerInfoUnitTests,SetShowToGameButtonTest)
		{
            SetShowToGameButtonTest();
        }
        void PersonalBankerInfoUnitTests::SetShowToGameButtonTest()
		{	CPersonalBankerInfo personalBankerInfo;

			//Tests

			//#1
			ASSERT_FALSE(personalBankerInfo.m_showToGameButton) <<
				_T("FAILED m_showToGameButton is not set properly");

			//#2
			personalBankerInfo.SetShowToGameButton(false);
			ASSERT_FALSE(personalBankerInfo.m_showToGameButton) <<
				_T("FAILED m_showToGameButton is not set properly");

			//#3
			personalBankerInfo.SetShowToGameButton(false);
			ASSERT_FALSE(personalBankerInfo.m_showToGameButton) <<
				_T("FAILED m_showToGameButton is not set properly");
		}

// #pragma region GetMinimumPointsDollarValueDownload
		/// <summary>
		/// Functional test for:
		///     CPersonalBankerInfo::GetMinimumPointsDollarValueDownload()
		/// Test the default value.
		/// </summary>
        TEST_F(PersonalBankerInfoUnitTests,GetMinimumPointsDollarValueDownload_DefaultTest)
		{
            GetMinimumPointsDollarValueDownload_DefaultTest();
        }
        void PersonalBankerInfoUnitTests::GetMinimumPointsDollarValueDownload_DefaultTest()
		{	// Perform operation(s) and test(s).
			CPersonalBankerInfo personalBankerInfo;
			ASSERT_EQ((DWORD)0, personalBankerInfo.GetMinimumPointsDollarValueDownload()) << _T("GetMinimumPointsDollarValueDownload default");
		}

		/// <summary>
		/// Functional test for:
		///     CPersonalBankerInfo::GetMinimumPointsDollarValueDownload()
		/// Test returned value when m_pointsPerDollar = 1.
		/// </summary>
        TEST_F(PersonalBankerInfoUnitTests,GetMinimumPointsDollarValueDownload_1Test)
		{
            GetMinimumPointsDollarValueDownload_1Test();
        }
        void PersonalBankerInfoUnitTests::GetMinimumPointsDollarValueDownload_1Test()
		{	// Setup.
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_pointsPerDollar = 1;

			// Perform operation(s) and test(s).
			personalBankerInfo.m_minimumPoints = 0;
			ASSERT_EQ((DWORD)0, personalBankerInfo.GetMinimumPointsDollarValueDownload()) << _T("GetMinimumPointsDollarValueDownload 0");

			personalBankerInfo.m_minimumPoints = 1;
			ASSERT_EQ((DWORD)1, personalBankerInfo.GetMinimumPointsDollarValueDownload()) << _T("GetMinimumPointsDollarValueDownload 1");

			personalBankerInfo.m_minimumPoints = 500;
			ASSERT_EQ((DWORD)500, personalBankerInfo.GetMinimumPointsDollarValueDownload()) << _T("GetMinimumPointsDollarValueDownload 500");

			personalBankerInfo.m_minimumPoints = 1000 * 1000;
			ASSERT_EQ((DWORD)(1000 * 1000), personalBankerInfo.GetMinimumPointsDollarValueDownload()) << _T("GetMinimumPointsDollarValueDownload 1000 * 1000");
		}

		/// <summary>
		/// Functional test for:
		///     CPersonalBankerInfo::GetMinimumPointsDollarValueDownload()
		/// Test returned value when m_pointsPerDollar = 2.
		/// </summary>
        TEST_F(PersonalBankerInfoUnitTests,GetMinimumPointsDollarValueDownload_2Test)
		{
            GetMinimumPointsDollarValueDownload_2Test();
        }
        void PersonalBankerInfoUnitTests::GetMinimumPointsDollarValueDownload_2Test()
		{	// Setup.
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_pointsPerDollar = 2;

			// Perform operation(s) and test(s).
			personalBankerInfo.m_minimumPoints = 0;
			ASSERT_EQ((DWORD)0, personalBankerInfo.GetMinimumPointsDollarValueDownload()) << _T("GetMinimumPointsDollarValueDownload 0");

			personalBankerInfo.m_minimumPoints = 1;
			ASSERT_EQ((DWORD)1, personalBankerInfo.GetMinimumPointsDollarValueDownload()) << _T("GetMinimumPointsDollarValueDownload 1");

			personalBankerInfo.m_minimumPoints = 2;
			ASSERT_EQ((DWORD)1, personalBankerInfo.GetMinimumPointsDollarValueDownload()) << _T("GetMinimumPointsDollarValueDownload 2");

			personalBankerInfo.m_minimumPoints = 3;
			ASSERT_EQ((DWORD)2, personalBankerInfo.GetMinimumPointsDollarValueDownload()) << _T("GetMinimumPointsDollarValueDownload 3");

			personalBankerInfo.m_minimumPoints = 4;
			ASSERT_EQ((DWORD)2, personalBankerInfo.GetMinimumPointsDollarValueDownload()) << _T("GetMinimumPointsDollarValueDownload 4");

			personalBankerInfo.m_minimumPoints = 5;
			ASSERT_EQ((DWORD)3, personalBankerInfo.GetMinimumPointsDollarValueDownload()) << _T("GetMinimumPointsDollarValueDownload 5");

			personalBankerInfo.m_minimumPoints = 500;
			ASSERT_EQ((DWORD)250, personalBankerInfo.GetMinimumPointsDollarValueDownload()) << _T("GetMinimumPointsDollarValueDownload 500");

			personalBankerInfo.m_minimumPoints = 1000 * 1000;
			ASSERT_EQ((DWORD)(500 * 1000), personalBankerInfo.GetMinimumPointsDollarValueDownload()) << _T("GetMinimumPointsDollarValueDownload 1000 * 1000");
		}

		/// <summary>
		/// Functional test for:
		///     CPersonalBankerInfo::GetMinimumPointsDollarValueDownload()
		/// Test returned value when m_pointsPerDollar = 500.
		/// </summary>
        TEST_F(PersonalBankerInfoUnitTests,GetMinimumPointsDollarValueDownload_500Test)
		{
            GetMinimumPointsDollarValueDownload_500Test();
        }
        void PersonalBankerInfoUnitTests::GetMinimumPointsDollarValueDownload_500Test()
		{	// Setup.
			CPersonalBankerInfo personalBankerInfo;
			personalBankerInfo.m_pointsPerDollar = 500;

			// Perform operation(s) and test(s).
			personalBankerInfo.m_minimumPoints = 0;
			ASSERT_EQ((DWORD)0, personalBankerInfo.GetMinimumPointsDollarValueDownload()) << _T("GetMinimumPointsDollarValueDownload 0");

			personalBankerInfo.m_minimumPoints = 1;
			ASSERT_EQ((DWORD)1, personalBankerInfo.GetMinimumPointsDollarValueDownload()) << _T("GetMinimumPointsDollarValueDownload 1");

			personalBankerInfo.m_minimumPoints = 499;
			ASSERT_EQ((DWORD)1, personalBankerInfo.GetMinimumPointsDollarValueDownload()) << _T("GetMinimumPointsDollarValueDownload 499");

			personalBankerInfo.m_minimumPoints = 500;
			ASSERT_EQ((DWORD)1, personalBankerInfo.GetMinimumPointsDollarValueDownload()) << _T("GetMinimumPointsDollarValueDownload 500");

			personalBankerInfo.m_minimumPoints = 501;
			ASSERT_EQ((DWORD)2, personalBankerInfo.GetMinimumPointsDollarValueDownload()) << _T("GetMinimumPointsDollarValueDownload 501");

			personalBankerInfo.m_minimumPoints = 999;
			ASSERT_EQ((DWORD)2, personalBankerInfo.GetMinimumPointsDollarValueDownload()) << _T("GetMinimumPointsDollarValueDownload 999");

			personalBankerInfo.m_minimumPoints = 1000;
			ASSERT_EQ((DWORD)2, personalBankerInfo.GetMinimumPointsDollarValueDownload()) << _T("GetMinimumPointsDollarValueDownload 1000");

			personalBankerInfo.m_minimumPoints = 1000 * 1000;
			ASSERT_EQ((DWORD)(2 * 1000), personalBankerInfo.GetMinimumPointsDollarValueDownload()) << _T("GetMinimumPointsDollarValueDownload 1000 * 1000");
		}
// #pragma endregion GetMinimumPointsDollarValueDownload

