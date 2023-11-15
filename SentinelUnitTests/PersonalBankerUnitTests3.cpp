

#include "stdafx.h"

#include "PersonalBanker.h"
#include "gtest/gtest.h"


const byte MEMSET_DEFAULT_VALUE = 0xcd;
const int NVRAMCHECKBYTEOFFSET = 4; // From CInternalsFramework::ProcessClearPersistentMemory().

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBanker::CPersonalBanker()
		///     CPersonalBanker::IsTransferRequestCanceled()
		/// Test the default state for CPersonalBanker::IsTransferRequestCanceled().
		/// </summary>
		TEST(PersonalBanker3, IsTransferRequestCanceledDefaultTest)
		{
			// Test data.
			const bool transferRequestCanceledExpected = false;

			// Setup.
			const CPersonalBanker pb;

			// Test #1.
			{
				const bool transferRequestCanceledResult = pb.IsTransferRequestCanceled();

				const bool areEqual = transferRequestCanceledResult == transferRequestCanceledExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED transferRequestCanceledResult=%d expected=%d in %s"),
					(int)transferRequestCanceledResult,
					(int)transferRequestCanceledExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBanker::CPersonalBanker()
		///     CPersonalBanker::CreateTransfer()
		///     CPersonalBanker::IsTransferRequestCanceled()
		/// Test the default state for CPersonalBanker::IsTransferRequestCanceled().
		/// </summary>
		TEST(PersonalBanker3, IsTransferRequestCanceledCreateTransferDefaultTest)
		{
			// Test data.
			const bool transferRequestCanceledExpected = false;

			// Setup.
			CPersonalBanker varPb;

			// Perform operations to be tested.
			varPb.CreateTransfer(CUtilities::GetTickCount());
			const CPersonalBanker &pb(varPb);

			// Test #1.
			{
				const bool transferRequestCanceledResult = pb.IsTransferRequestCanceled();

				bool areEqual = transferRequestCanceledResult == transferRequestCanceledExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED transferRequestCanceledResult=%d expected=%d in %s"),
					(int)transferRequestCanceledResult,
					(int)transferRequestCanceledExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBanker::CPersonalBanker()
		///     CPersonalBanker::CreateTransfer()
		///     CPersonalBanker::SetPersonalBankerType()
		///     CPersonalBanker::IsTransferRequestCanceled()
		/// Test the default state for CPersonalBanker::IsTransferRequestCanceled().
		/// </summary>
		TEST(PersonalBanker3, IsTransferRequestCanceledCreateTransfer1Test)
		{
			// Test data.
			const PersonalBankerType personalBankerType = Unspecified;
			const bool transferRequestCanceledExpected = false;

			// Setup.
			CPersonalBanker varPb;

			// Perform operations to be tested.
			varPb.CreateTransfer(CUtilities::GetTickCount());
			varPb.SetPersonalBankerType(personalBankerType);
			const CPersonalBanker &pb(varPb);

			// Test #1.
			{
				const bool transferRequestCanceledResult = pb.IsTransferRequestCanceled();

				bool areEqual = transferRequestCanceledResult == transferRequestCanceledExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED transferRequestCanceledResult=%d expected=%d in %s"),
					(int)transferRequestCanceledResult,
					(int)transferRequestCanceledExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBanker::CPersonalBanker()
		///     CPersonalBanker::CreateTransfer()
		///     CPersonalBanker::SetPersonalBankerType()
		///     CPersonalBanker::IsTransferRequestCanceled()
		/// Test the default state for CPersonalBanker::IsTransferRequestCanceled() after CPersonalBanker::CreateTransfer().
		/// </summary>
		TEST(PersonalBanker3, IsTransferRequestCanceledCreateTransfer2Test)
		{
			// Test data.
			const PersonalBankerType personalBankerType = PendingResponse;
			const bool transferRequestCanceledExpected = false;

			// Setup.
			CPersonalBanker varPb;

			// Perform operations to be tested.
			varPb.CreateTransfer(CUtilities::GetTickCount());
			varPb.SetPersonalBankerType(personalBankerType);
			const CPersonalBanker &pb(varPb);

			// Test #1.
			{
				const bool transferRequestCanceledResult = pb.IsTransferRequestCanceled();

				bool areEqual = transferRequestCanceledResult == transferRequestCanceledExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED transferRequestCanceledResult=%d expected=%d in %s"),
					(int)transferRequestCanceledResult,
					(int)transferRequestCanceledExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBanker::CPersonalBanker()
		///     CPersonalBanker::CreateTransfer()
		///     CPersonalBanker::SetPersonalBankerType()
		///     CPersonalBanker::IsTransferRequestCanceled()
		/// </summary>
		TEST(PersonalBanker3, IsTransferRequestCanceledCreateTransfer3Test)
		{
			// Test data.
			const PersonalBankerType personalBankerType = CanceledSessionRequest;
			const bool transferRequestCanceledExpected = true;

			// Setup.
			CPersonalBanker varPb;

			// Perform operations to be tested.
			varPb.CreateTransfer(CUtilities::GetTickCount());
			varPb.SetPersonalBankerType(personalBankerType);
			const CPersonalBanker &pb(varPb);

			// Test #1.
			{
				const bool transferRequestCanceledResult = pb.IsTransferRequestCanceled();

				bool areEqual = transferRequestCanceledResult == transferRequestCanceledExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED transferRequestCanceledResult=%d expected=%d in %s"),
					(int)transferRequestCanceledResult,
					(int)transferRequestCanceledExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBanker::CPersonalBanker()
		///     CPersonalBanker::CreateTransfer()
		///     CPersonalBanker::SetPersonalBankerType()
		///     CPersonalBanker::IsTransferRequestCanceled()
		/// </summary>
		TEST(PersonalBanker3, IsTransferRequestCanceledCreateTransfer4Test)
		{
			// Test data.
			const PersonalBankerType personalBankerType = CanceledCashDownload;
			const bool transferRequestCanceledExpected = true;

			// Setup.
			CPersonalBanker varPb;

			// Perform operations to be tested.
			varPb.CreateTransfer(CUtilities::GetTickCount());
			varPb.SetPersonalBankerType(personalBankerType);
			const CPersonalBanker &pb(varPb);

			// Test #1.
			{
				const bool transferRequestCanceledResult = pb.IsTransferRequestCanceled();

				bool areEqual = transferRequestCanceledResult == transferRequestCanceledExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED transferRequestCanceledResult=%d expected=%d in %s"),
					(int)transferRequestCanceledResult,
					(int)transferRequestCanceledExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBanker::CPersonalBanker()
		///     CPersonalBanker::CreateTransfer()
		///     CPersonalBanker::SetPersonalBankerType()
		///     CPersonalBanker::IsTransferRequestCanceled()
		/// </summary>
		TEST(PersonalBanker3, IsTransferRequestCanceledCreateTransfer5Test)
		{
			// Test data.
			const PersonalBankerType personalBankerType = UnknownError;
			const bool transferRequestCanceledExpected = true;

			// Setup.
			CPersonalBanker varPb;

			// Perform operations to be tested.
			varPb.CreateTransfer(CUtilities::GetTickCount());
			varPb.SetPersonalBankerType(personalBankerType);
			const CPersonalBanker &pb(varPb);

			// Test #1.
			{
				const bool transferRequestCanceledResult = pb.IsTransferRequestCanceled();

				bool areEqual = transferRequestCanceledResult == transferRequestCanceledExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED transferRequestCanceledResult=%d expected=%d in %s"),
					(int)transferRequestCanceledResult,
					(int)transferRequestCanceledExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBanker::CPersonalBanker()
		///     CPersonalBanker::CreateTransfer()
		///     CPersonalBanker::SetPersonalBankerType()
		///     CPersonalBanker::IsTransferRequestCanceled()
		/// </summary>
		/// <remarks>
		/// Hard to call CPersonalBankerTransfer::SetPromoPersonalBankerType() using a CPersonalBanker instance.
		/// Hard to set CPersonalBankerTransfer::m_promoPbtType using a CPersonalBanker instance.
		/// </remarks>
		TEST(PersonalBanker3, IsTransferRequestCanceledCreateTransferPromo1Test)
		{
			// Test data.
			const PersonalBankerType personalBankerType = Unspecified;
			const bool transferRequestCanceledExpected = false;

			// Setup.
			CPersonalBanker varPb;

			// Perform operations to be tested.
			varPb.CreateTransfer(CUtilities::GetTickCount());
			varPb.SetPersonalBankerType(personalBankerType);
			const CPersonalBanker &pb(varPb);

			// Test #1.
			{
				const bool transferRequestCanceledResult = pb.IsTransferRequestCanceled();

				bool areEqual = transferRequestCanceledResult == transferRequestCanceledExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED transferRequestCanceledResult=%d expected=%d in %s"),
					(int)transferRequestCanceledResult,
					(int)transferRequestCanceledExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBanker::CPersonalBanker()
		///     CPersonalBanker::SetToReleaseTransferSession()
		/// </summary>
		/// <remarks>
		/// Hard to test side effects of CPersonalBanker::SetToReleaseTransferSession().
		/// </remarks>
		TEST(PersonalBanker3, SetToReleaseTransferSessionDefaultTest)
		{
			// Test data.
			const CPersonalBankerTransfer *pbTransferExpected(NULL);

			// Setup.
			CPersonalBanker varPb;

			// Perform operations to be tested.
			varPb.SetToReleaseTransferSession();
			const CPersonalBanker &pb(varPb);

			// Test #1.
				const CPersonalBankerTransfer *pbTransferResult = pb.GetTransferCopy();

			{
				// Report #1.
				bool areEqual = pbTransferResult == pbTransferExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED pbTransferResult=%p expected=%p in %s"),
					(const void *)pbTransferResult,
					(const void *)pbTransferExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
			delete pbTransferResult;
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBanker::CPersonalBanker()
		///     CPersonalBanker::CreateTransfer()
		///     CPersonalBanker::SetToReleaseTransferSession()
		///     CPersonalBanker::GetTransferCopy()
		///     CPersonalBanker::GetPersonalBankerType()
		///     CPersonalBanker::GetPromoPersonalBankerType()
		///     CPersonalBanker::GetPbStateForUi()
		///     CPersonalBanker::GetState()
		/// </summary>
		/// <remarks>
		/// </remarks>
		TEST(PersonalBanker3, SetToReleaseTransferSessionCreateTransferTest)
		{
			// Test data.
			const PersonalBankerType personalBankerTypeExpected(SessionRelease);
			const PersonalBankerType promoPersonalBankerTypeExpected(SessionRelease);
			const PersonalBankerState pbStateForUiExpected(PBStateUnknown);
			const PersonalBankerState stateExpected(PBStateReleaseSent);

			// Setup.
			CPersonalBanker varPb;
			varPb.CreateTransfer(CUtilities::GetTickCount());

			// Perform operations to be tested.
			varPb.SetToReleaseTransferSession();
			const CPersonalBanker &pb(varPb);

			// Test #1.
			const CPersonalBankerTransfer * const pbTransferResult = pb.GetTransferCopy();

			{
				// Report #1.
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED pbTransferResult=%p expected=NOT NULL in %s"),
					(const void *)pbTransferResult,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_NE(nullptr, pbTransferResult) << failedMsg;
			}

			// Test #2.
			const PersonalBankerType personalBankerTypeResult = pbTransferResult->GetPersonalBankerType();

			{
				// Report #2.
				bool areEqual = personalBankerTypeResult == personalBankerTypeExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED personalBankerTypeResult=%d expected=%d in %s"),
					(int)personalBankerTypeResult,
					(int)personalBankerTypeExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #3.
			const PersonalBankerType promoPersonalBankerTypeResult = pbTransferResult->GetPromoPersonalBankerType();

			{
				// Report #3.
				bool areEqual = promoPersonalBankerTypeResult == promoPersonalBankerTypeExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED promoPersonalBankerTypeResult=%d expected=%d in %s"),
					(int)promoPersonalBankerTypeResult,
					(int)promoPersonalBankerTypeExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #4.
			const PersonalBankerState pbStateForUiResult = pb.GetPbStateForUi();

			{
				// Report #4.
				bool areEqual = pbStateForUiResult == pbStateForUiExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED pbStateForUiResult=%d expected=%d in %s"),
					(int)pbStateForUiResult,
					(int)pbStateForUiExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #5.
			const PersonalBankerState stateResult = pb.GetState();

			{
				// Report #5.
				bool areEqual = stateResult == stateExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED stateResult=%d expected=%d in %s"),
					(int)stateResult,
					(int)stateExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
			delete pbTransferResult;
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBanker::CPersonalBanker()
		///     CPersonalBanker::ProcessInitialPersonalBanker()
		/// </summary>
		TEST(PersonalBanker3, ProcessInitialPersonalBankerDefaultTest)
		{
			// Test data.
			const CPersonalBankerTransfer *pbTransferExpected(NULL);
			const CConfig config(false, "");

			// Setup.
			CMemoryStorage::SetInactiveLocation(NVRAM_PB_TRANSFER_CHECKBYTE_OFFSET, (BYTE)~NVRAM_PB_TRANSFER_CHECKBYTE_VALUE);
			CPersonalBanker varPb;

			// Perform operations to be tested.
			varPb.ProcessInitialPersonalBanker(config);
			const CPersonalBanker &pb(varPb);

			// Test #1.
				const CPersonalBankerTransfer *pbTransferResult = pb.GetTransferCopy();

			{
				// Report #1.
				bool areEqual = pbTransferResult == pbTransferExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED pbTransferResult=%p expected=%p in %s"),
					(const void *)pbTransferResult,
					(const void *)pbTransferExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
			delete pbTransferResult;
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBanker::CPersonalBanker()
		///     CPersonalBanker::ProcessInitialPersonalBanker()
		/// </summary>
		TEST(PersonalBanker3, ProcessInitialPersonalBankerNvramBuildTest)
		{
			// Test data.
			const CPersonalBankerTransfer *pbTransferExpected(NULL);
			const MEMORY_TYPE memoryType(MEMORY_NVRAM);

			// Setup.
			CMemoryStorage::SetSprinkles();
			CMemoryStorage::SetInactiveLocation(NVRAM_PB_TRANSFER_CHECKBYTE_OFFSET, (BYTE)~NVRAM_PB_TRANSFER_CHECKBYTE_VALUE);
			const CConfig config(false, "");
			CPersonalBanker varPb(memoryType);

			// Perform operations to be tested.
			varPb.ProcessInitialPersonalBanker(config);
			const CPersonalBanker &pb(varPb);

			// Test #1.
				const CPersonalBankerTransfer *pbTransferResult = pb.GetTransferCopy();

			{
				// Report #1.
				bool areEqual = pbTransferResult == pbTransferExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED pbTransferResult=%p expected=%p in %s"),
					(const void *)pbTransferResult,
					(const void *)pbTransferExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
			delete pbTransferResult;
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBanker::CPersonalBanker()
		///     CPersonalBanker::ProcessInitialPersonalBanker()
		/// </summary>
		TEST(PersonalBanker3, ProcessInitialPersonalBankerNvramBuildNoInfoTest)
		{
			// Test data. - inputs
			const MEMORY_TYPE memoryType(MEMORY_NVRAM);
			const byte cardNumber[CARD_ID_LEN] = { 0x12, 0x34, 0x56, 0x78, 0x90 };
			const CardType cardType(CARD_PLAYER);
			const DWORD minimumCashUp(123);
			const DWORD maximumCashUp(345);
			const DWORD minimumPromoUp(456);
			const DWORD maximumPromoUp(567);
			const bool gameOnline(true);
			const bool bIsEFT(false);

			// Test data - outputs.
			const CPersonalBankerTransfer *pbTransferExpected(NULL);
			const DWORD minCashUpExpected(0);
			const DWORD maxCashUpExpected(0);
			const DWORD minPromoUpExpected(0);
			const DWORD maxPromoUpExpected(0);


			// Setup.
			CMemoryStorage::SetSprinkles();
			const CConfig config(false, "");
			CCard varCard(CUtilities::GetCurrentTime(), CUtilities::GetCurrentTime());
			varCard.SetCardInfo(cardType, cardNumber, sizeof(cardNumber));
			const CCard &card(varCard);

			CPersonalBanker varPb(memoryType);
			varPb.CreateTransfer(CUtilities::GetTickCount(), memoryType);
			varPb.SetTransferCardNumber(&card, CUtilities::GetCurrentTime());

			CPersonalBankerInfo *pbInfo = new CPersonalBankerInfo();
			pbInfo->SetCardNumber(cardNumber);
			pbInfo->SetMinimumCashUp(minimumCashUp);
			pbInfo->SetMaximumCashUp(maximumCashUp);
			pbInfo->SetMinimumPromoUp(minimumPromoUp);
			pbInfo->SetMaximumPromoUp(maximumPromoUp);
			CMemoryStorage::SetInactiveLocation(NVRAM_PB_INFO_CHECKBYTE_OFFSET, (BYTE)~NVRAM_PB_INFO_CHECKBYTE_VALUE);
			// Skip the info.
			//varPb.ProcessPersonalBankerInfo(pbInfo,
			//	config,
			//	gameOnline,
			//	bIsEFT);

			// Perform operations to be tested.
			varPb.ProcessInitialPersonalBanker(config);
			const CPersonalBanker &pb(varPb);

			// Test #1.
			const CPersonalBankerTransfer * const pbTransferResult = pb.GetTransferCopy();

			{
				// Report #1.
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED pbTransferResult=%p expected=NOT NULL in %s"),
					(const void *)pbTransferResult,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_NE(nullptr, pbTransferResult) << failedMsg;
			}

			// Test #2.
			const DWORD minCashUpResult = pbTransferResult->GetMinCashUp();

			{
				// Report #2.
				bool areEqual = minCashUpResult == minCashUpExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED minCashUpResult=%u expected=%u in %s"),
					(unsigned)minCashUpResult,
					(unsigned)minCashUpExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #3.
			const DWORD maxCashUpResult = pbTransferResult->GetMaxCashUp();

			{
				// Report #3.
				bool areEqual = maxCashUpResult == maxCashUpExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED maxCashUpResult=%u expected=%u in %s"),
					(unsigned)maxCashUpResult,
					(unsigned)maxCashUpExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #4.
			const DWORD minPromoUpResult = pbTransferResult->GetMinPromoUp();

			{
				// Report #4.
				bool areEqual = minPromoUpResult == minPromoUpExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED minPromoUpResult=%u expected=%u in %s"),
					(unsigned)minPromoUpResult,
					(unsigned)minPromoUpExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #5.
			const DWORD maxPromoUpResult = pbTransferResult->GetMaxPromoUp();

			{
				// Report #5.
				bool areEqual = maxPromoUpResult == maxPromoUpExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED maxPromoUpResult=%u expected=%u in %s"),
					(unsigned)maxPromoUpResult,
					(unsigned)maxPromoUpExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #6.
			const CPersonalBankerInfo * const pbInfoResult = pb.GetInfoCopy();

			{
				// Report #6.
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED pbInfoResult=%p expected=NULL in %s"),
					(const void *)pbInfoResult,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_EQ(nullptr, pbInfoResult) << failedMsg;
			}

			// Cleanup.
			delete pbTransferResult;
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBanker::GetMeterToUpdate() -- Indirectly.
		/// Also calls:
		///     CGames::CGames();
		///     CPersonalBanker::CPersonalBanker()
		///     CPersonalBanker::CreateTransfer()
		///     CPersonalBanker::SetPersonalBankerType()
		///     CPersonalBanker::SetState()
		///     CPersonalBanker::GetTransferCopy()
		///     CPersonalBanker::ProcessPersonalBankerTransferComplete()
		/// </summary>
		/// <remarks>
		/// Full code path coverage test.
		/// Full functional test.
		/// Picked PersonalBankerType values to test edge conditions.
		/// </remarks>
		TEST(PersonalBanker3, GetMeterToUpdateTest)
		{
			typedef struct
			{
				// Inputs.
				const bool m_wasTransferCompleteExpected;
				const bool m_isEFT;
				const byte m_meterNumberToAddTo;
				const PersonalBankerType m_originalPBType;

				// Expected results.
				const byte m_meterNumberToAddToExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				// m_originalPBType == Unspecified is harder to setup when calling GetMeterToUpdate() indirectly,
				// because when CPersonalBankerTransfer::m_pbtType == Unspecified
				// then CPersonalBanker::GetOriginalPBType() returns CPersonalBankerTransfer::m_promoPbtType
				// -- and CPersonalBankerTransfer::m_promoPbtType is harder to setup.
				{ false, false, 0xcd, SessionRequest, mNone },
				{ false, true, 0xcd, SessionRequest, mNone },
				{ true, false, 0xcd, SessionRequest, mNone },
				{ true, true, 0xcd, SessionRequest, mNone },

				{ false, false, 0xcd, PromoCreditsToGame, mNone },
				{ false, true, 0xcd, PromoCreditsToGame, mNone },
				{ true, false, 0xcd, PromoCreditsToGame, mNone },
				{ true, true, 0xcd, PromoCreditsToGame, mNone },

				{ false, false, 0xcd, PromoCreditsToCash, mNone },
				{ false, true, 0xcd, PromoCreditsToCash, mNone },
				{ true, false, 0xcd, PromoCreditsToCash, mPBn },
				{ true, true, 0xcd, PromoCreditsToCash, mPBn },

				{ false, false, 0xcd, PointRedemptionToCash, mNone },
				{ false, true, 0xcd, PointRedemptionToCash, mNone },
				{ true, false, 0xcd, PointRedemptionToCash, mNone },
				{ true, true, 0xcd, PointRedemptionToCash, mPtR },

				{ false, false, 0xcd, CashUpload, mNone },
				{ false, true, 0xcd, CashUpload, mNone },
				{ true, false, 0xcd, CashUpload, mNone },
				{ true, true, 0xcd, CashUpload, mNone },

				{ false, false, 0xcd, ChangePinAtSentinel, mNone },
				{ false, true, 0xcd, ChangePinAtSentinel, mNone },
				{ true, false, 0xcd, ChangePinAtSentinel, mNone },
				{ true, true, 0xcd, ChangePinAtSentinel, mNone },

				{ false, false, 0xcd, PointRedemptionToPromoCredits, mNone },
				{ false, true, 0xcd, PointRedemptionToPromoCredits, mNone },
				{ true, false, 0xcd, PointRedemptionToPromoCredits, mPtR },
				{ true, true, 0xcd, PointRedemptionToPromoCredits, mPtR },

				{ false, false, 0xcd, PointCashUpload, mNone },
				{ false, true, 0xcd, PointCashUpload, mNone },
				{ true, false, 0xcd, PointCashUpload, mNone },
				{ true, true, 0xcd, PointCashUpload, mNone },

				{ false, false, 0xcd, UnknownError, mNone },
				{ false, true, 0xcd, UnknownError, mNone },
				{ true, false, 0xcd, UnknownError, mNone },
				{ true, true, 0xcd, UnknownError, mNone },

			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CGames games;
				CPersonalBanker varPb;
				varPb.CreateTransfer(CUtilities::GetTickCount());
				varPb.SetPersonalBankerType(TEST_DATA[testIndex].m_originalPBType);

				if (TEST_DATA[testIndex].m_wasTransferCompleteExpected)
				{
					varPb.SetState(PBStateTransferingFromToGame);
				}
				else
				{
					varPb.SetState(PBStateUnknown);
				}

				// Perform operations to be tested.
				CPersonalBankerTransfer *pbTransfer = varPb.GetTransferCopy();
				byte meterNumberToAddTo = MEMSET_DEFAULT_VALUE;
				bool isDuplicate = false;
				varPb.ProcessPersonalBankerTransferComplete(pbTransfer,
					games,
					meterNumberToAddTo,
					isDuplicate,
					TEST_DATA[testIndex].m_isEFT);
				// 2013-06-11 GetMeterToUpdate() is a private method, so we use indirect methods above for setup and test, instead of calling directly.
				//varPb.GetMeterToUpdate(TEST_DATA[testIndex].m_wasTransferCompleteExpected,
				//	TEST_DATA[testIndex].m_isEFT,
				//	meterNumberToAddTo,
				//	TEST_DATA[testIndex].m_originalPBType);

				{
					// Report #1.
					bool areEqual = meterNumberToAddTo == TEST_DATA[testIndex].m_meterNumberToAddToExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED meterNumberToAddTo=%d expected=%d testIndex=%d (%d %d %d) in %s"),
						(int)meterNumberToAddTo,
						(int)TEST_DATA[testIndex].m_meterNumberToAddToExpected,
						(int)testIndex,
						(int)TEST_DATA[testIndex].m_wasTransferCompleteExpected,
						(int)TEST_DATA[testIndex].m_isEFT,
						(int)TEST_DATA[testIndex].m_originalPBType,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
				delete pbTransfer;
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBanker::GetMeterToUpdate() -- Indirectly.
		///     CPersonalBanker::GetOriginalPBType() -- Indirectly.
		/// Also calls:
		///     CGames::CGames();
		///     CPersonalBanker::CPersonalBanker()
		///     CPersonalBanker::CreateTransfer()
		///     CPersonalBanker::SetPersonalBankerType()
		///     CPersonalBanker::SetState()
		///     CPersonalBanker::GetTransferCopy()
		///     CPersonalBanker::ProcessPersonalBankerTransferComplete()
		/// </summary>
		/// <remarks>
		/// Full code path coverage test.
		/// Full functional test for GetMeterToUpdate(), mostly code coverage for GetOriginalPBType().
		/// Picked PersonalBankerType values to test edge conditions for GetMeterToUpdate().
		/// Need to get an entry into m_pbSuccessLog.GetLastLog() in order to test more side effects of CPersonalBanker::GetOriginalPBType(). 
		/// </remarks>
		TEST(PersonalBanker3, GetOriginalPBType_CoverageTest)
		{
			typedef struct
			{
				// Inputs.
				const bool m_wasTransferCompleteExpected;
				const bool m_isEFT;
				const byte m_meterNumberToAddTo;
				const PersonalBankerType m_pbType;
				const PersonalBankerType m_promoPBType;

				// Expected results.
				const byte m_meterNumberToAddToExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				// m_originalPBType == Unspecified is harder to setup when calling GetMeterToUpdate() indirectly,
				// because when CPersonalBankerTransfer::m_pbtType == Unspecified
				// then CPersonalBanker::GetOriginalPBType() returns CPersonalBankerTransfer::m_promoPbtType
				// -- and CPersonalBankerTransfer::m_promoPbtType is harder to setup.
				{ false, false, 0xcd, Unspecified, Unspecified, mNone },
				{ false, true, 0xcd, Unspecified, Unspecified, mNone },
				{ true, false, 0xcd, Unspecified, Unspecified, mNone },
				{ true, true, 0xcd, Unspecified, Unspecified, mNone },

				{ false, false, 0xcd, Unspecified, SessionRelease, mNone },
				{ false, true, 0xcd, Unspecified, SessionRelease, mNone },
				{ true, false, 0xcd, Unspecified, SessionRelease, mNone },
				{ true, true, 0xcd, Unspecified, SessionRelease, mNone },

				{ false, false, 0xcd, Unspecified, PromoCreditsUpload, mNone },
				{ false, true, 0xcd, Unspecified, PromoCreditsUpload, mNone },
				{ true, false, 0xcd, Unspecified, PromoCreditsUpload, mNone },
				{ true, true, 0xcd, Unspecified, PromoCreditsUpload, mNone },

				{ false, false, 0xcd, PromoCreditsToGame, Unspecified, mNone },
				{ false, true, 0xcd, PromoCreditsToGame, Unspecified, mNone },
				{ true, false, 0xcd, PromoCreditsToGame, Unspecified, mNone },
				{ true, true, 0xcd, PromoCreditsToGame, Unspecified, mNone },

				{ false, false, 0xcd, PromoCreditsToCash, Unspecified, mNone },
				{ false, true, 0xcd, PromoCreditsToCash, Unspecified, mNone },
				{ true, false, 0xcd, PromoCreditsToCash, Unspecified, mPBn },
				{ true, true, 0xcd, PromoCreditsToCash, Unspecified, mPBn },

				{ false, false, 0xcd, PointRedemptionToCash, Unspecified, mNone },
				{ false, true, 0xcd, PointRedemptionToCash, Unspecified, mNone },
				{ true, false, 0xcd, PointRedemptionToCash, Unspecified, mNone },
				{ true, true, 0xcd, PointRedemptionToCash, Unspecified, mPtR },

				{ false, false, 0xcd, CashUpload, Unspecified, mNone },
				{ false, true, 0xcd, CashUpload, Unspecified, mNone },
				{ true, false, 0xcd, CashUpload, Unspecified, mNone },
				{ true, true, 0xcd, CashUpload, Unspecified, mNone },

				{ false, false, 0xcd, ChangePinAtSentinel, Unspecified, mNone },
				{ false, true, 0xcd, ChangePinAtSentinel, Unspecified, mNone },
				{ true, false, 0xcd, ChangePinAtSentinel, Unspecified, mNone },
				{ true, true, 0xcd, ChangePinAtSentinel, Unspecified, mNone },

				{ false, false, 0xcd, PointRedemptionToPromoCredits, Unspecified, mNone },
				{ false, true, 0xcd, PointRedemptionToPromoCredits, Unspecified, mNone },
				{ true, false, 0xcd, PointRedemptionToPromoCredits, Unspecified, mPtR },
				{ true, true, 0xcd, PointRedemptionToPromoCredits, Unspecified, mPtR },

				{ false, false, 0xcd, PointCashUpload, Unspecified, mNone },
				{ false, true, 0xcd, PointCashUpload, Unspecified, mNone },
				{ true, false, 0xcd, PointCashUpload, Unspecified, mNone },
				{ true, true, 0xcd, PointCashUpload, Unspecified, mNone },

				{ false, false, 0xcd, UnknownError, Unspecified, mNone },
				{ false, true, 0xcd, UnknownError, Unspecified, mNone },
				{ true, false, 0xcd, UnknownError, Unspecified, mNone },
				{ true, true, 0xcd, UnknownError, Unspecified, mNone },

			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CGames games;
				CPersonalBanker varPb;
				varPb.CreateTransfer(CUtilities::GetTickCount());

				switch (TEST_DATA[testIndex].m_promoPBType)
				{
				case SessionRelease:
					varPb.SetToReleaseTransferSession();
					break;

				case PromoCreditsUpload:
					{
						const DWORD amountInDollars = 1;
						byte pbEFTNumber = 1;
						varPb.SetState(PBStateInfoReceived);
						varPb.BeginPersonalBankerTransfer(games, PromoDollarsFromGame, amountInDollars, pbEFTNumber);
					}

				case Unspecified:
					// Do nothing. Leave set to default value.
					break;

				default:
					// Error in test data.
					{
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED m_promoPBType=%d NOT SUPPORTED testIndex=%d in %s"),
							(int)TEST_DATA[testIndex].m_promoPBType,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(false) << failedMsg;
					}
					break;
				}

				// Set type AFTER SetToReleaseTransferSession(), etc.
				varPb.SetPersonalBankerType(TEST_DATA[testIndex].m_pbType);

				if (TEST_DATA[testIndex].m_wasTransferCompleteExpected)
				{
					varPb.SetState(PBStateTransferingFromToGame);
				}
				else
				{
					varPb.SetState(PBStateUnknown);
				}


				// Perform operations to be tested.
				CPersonalBankerTransfer *pbTransfer = varPb.GetTransferCopy();
				byte meterNumberToAddTo = MEMSET_DEFAULT_VALUE;
				bool isDuplicate = false;
				varPb.ProcessPersonalBankerTransferComplete(pbTransfer,
					games,
					meterNumberToAddTo,
					isDuplicate,
					TEST_DATA[testIndex].m_isEFT);

				{
					// Report #1.
					bool areEqual = meterNumberToAddTo == TEST_DATA[testIndex].m_meterNumberToAddToExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED meterNumberToAddTo=%d expected=%d testIndex=%d (%d %d %d %d) in %s"),
						(int)meterNumberToAddTo,
						(int)TEST_DATA[testIndex].m_meterNumberToAddToExpected,
						(int)testIndex,
						(int)TEST_DATA[testIndex].m_wasTransferCompleteExpected,
						(int)TEST_DATA[testIndex].m_isEFT,
						(int)TEST_DATA[testIndex].m_pbType,
						(int)TEST_DATA[testIndex].m_promoPBType,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #2

				// Cleanup.
				delete pbTransfer;
			}
		}

		/// <summary>
		/// Get current log index information.
		/// 2013-06-13 Indexes are currently zero-based.  I.e, minLogIndex=0, maxLogIndex=maxLogsExpected-1.
		/// NOT a test method.
		/// </summary>
		void LogTransfer_GetLogIndexInfo(int &maxLogs,
			int &currentLogIndex,
			int &maxSuccessLogs,
			int &currentSuccessLogIndex)
		{
			// Set defaults. Set info to zero in case log information is not available.
			maxLogs = 0;
			currentLogIndex = 0;
			maxSuccessLogs = 0;
			currentSuccessLogIndex = 0;

			// Get info as available.
			CPersonalBanker pb;

			CLogs *pbLog = pb.GetLogCopy();
			if (pbLog != NULL)
			{
				maxLogs = pbLog->GetMaxLogs();
				currentLogIndex = pbLog->GetIndex();
			}

			CLogs *pbSuccessLog = pb.GetSuccessLogCopy();
			if (pbSuccessLog != NULL)
			{
				maxSuccessLogs = pbSuccessLog->GetMaxLogs();
				currentSuccessLogIndex = pbSuccessLog->GetIndex();
			}

			ASSERT_TRUE(maxLogs >= 0) << _T("FAILED maxLogs >= 0 in LogTransfer_GetLogIndexInfo");
			ASSERT_TRUE(currentLogIndex >= 0) << _T("FAILED currentLogIndex >= 0 in LogTransfer_GetLogIndexInfo");
			ASSERT_TRUE(maxSuccessLogs >= 0) << _T("FAILED maxSuccessLogs >= 0 in LogTransfer_GetLogIndexInfo");
			ASSERT_TRUE(currentSuccessLogIndex >= 0) << _T("FAILED currentSuccessLogIndex >= 0 in LogTransfer_GetLogIndexInfo");

			// Cleanup.
			delete pbLog;
			delete pbSuccessLog;
		}

		typedef struct
		{
			int m_maxLogsExpected;
			int m_logIndexStart; // Actually, historical information that can be used if reporting an error.
			int m_logIndexExpected;
			int m_maxSuccessLogsExpected;
			int m_successLogIndexStart; // Actually, historical information that can be used if reporting an error.
			int m_successLogIndexExpected;
		} LogIndexInfoExpected;

		/// <summary>
		/// Calculate expected log index information.
		/// 2013-06-13 Indexes are currently zero-based.  I.e, minLogIndex=0, maxLogIndex=maxLogsExpected-1.
		/// NOT a test method.
		/// </summary>
		void LogTransfer_GetLogIndexInfoExpected(LogIndexInfoExpected &logIndexInfoExpected,
			int logsExpected,
			int successLogsExpected)
		{
			ASSERT_TRUE(logsExpected >= 0) << _T("FAILED logsExpected >= 0 in LogTransfer_GetLogIndexInfoExpected");
			ASSERT_TRUE(successLogsExpected >= 0) << _T("FAILED logsExpected >= 0 in LogTransfer_GetLogIndexInfoExpected");

			LogTransfer_GetLogIndexInfo(logIndexInfoExpected.m_maxLogsExpected,
				logIndexInfoExpected.m_logIndexStart,
				logIndexInfoExpected.m_maxSuccessLogsExpected,
				logIndexInfoExpected.m_successLogIndexStart);

			logIndexInfoExpected.m_logIndexExpected = logIndexInfoExpected.m_logIndexStart + logsExpected;
			if (logIndexInfoExpected.m_maxLogsExpected > 0)
			{
				logIndexInfoExpected.m_logIndexExpected %= logIndexInfoExpected.m_maxLogsExpected;
			}

			logIndexInfoExpected.m_successLogIndexExpected = logIndexInfoExpected.m_successLogIndexStart + successLogsExpected;
			if (logIndexInfoExpected.m_maxSuccessLogsExpected > 0)
			{
				logIndexInfoExpected.m_successLogIndexExpected %= logIndexInfoExpected.m_maxSuccessLogsExpected;
			}
		}

		/// <summary>
		/// Assert expected log index information.
		/// 2013-06-13 Indexes are currently zero-based.  I.e, minLogIndex=0, maxLogIndex=maxLogsExpected-1.
		/// NOT a test method.
		/// </summary>
		void LogTransfer_AssertLogInfoExpected(	CLogs *pbLog,
			CLogs *pbSuccessLog,
			const LogIndexInfoExpected &logIndexInfoExpected,
			int testIndex,
			LPCTSTR caller)
		{
			// Test #1.  Verify that pbLog is NOT null.

			{
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED pbLog=%p expected=NOT NULL testIndex=%d from %s"),
					(const void *)pbLog,
					(int)testIndex,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_NE(nullptr, pbLog) << failedMsg;
			}

			// Test #2.  Verify that the number of records per log is still the same.
			const int maxLogsResult = pbLog->GetMaxLogs();

			{
				bool areEqual = maxLogsResult == logIndexInfoExpected.m_maxLogsExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED maxLogsResult=%d expected=%d testIndex=%d from %s"),
					(int)maxLogsResult,
					(int)logIndexInfoExpected.m_maxLogsExpected,
					(int)testIndex,
					static_cast<LPCTSTR>(caller));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #3.  Verify that the correct number of log records were added.
			const int lastLogIndexResult = pbLog->GetIndex();

			{
				// CLogs allows the index to be out of range when index==maxLogs.
				bool areEqual = lastLogIndexResult == logIndexInfoExpected.m_logIndexExpected ||
					(lastLogIndexResult == maxLogsResult && logIndexInfoExpected.m_logIndexExpected == 0);

				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED lastLogIndexResult=%d expected=%d start=%d max=%d testIndex=%d from %s"),
					(int)lastLogIndexResult,
					(int)logIndexInfoExpected.m_logIndexExpected,
					(int)logIndexInfoExpected.m_logIndexStart,
					(int)logIndexInfoExpected.m_maxLogsExpected,
					(int)testIndex,
					static_cast<LPCTSTR>(caller));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #4.  Verify that pbSuccessLog is NOT null.

			{
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED successLogResult=%p expected=NOT NULL testIndex=%d from %s"),
					(const void *)pbSuccessLog,
					(int)testIndex,
					static_cast<LPCTSTR>(caller));
				ASSERT_NE(nullptr, pbSuccessLog) << failedMsg;
			}

			// Test #5.  Verify that the number of records per success log is still the same.
			const int maxSuccessLogsResult = pbSuccessLog->GetMaxLogs();

			{
				bool areEqual = maxSuccessLogsResult == logIndexInfoExpected.m_maxSuccessLogsExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED maxSuccessLogsResult=%d expected=%d testIndex=%d from %s"),
					(int)maxSuccessLogsResult,
					(int)logIndexInfoExpected.m_maxSuccessLogsExpected,
					(int)testIndex,
					static_cast<LPCTSTR>(caller));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #6.  Verify that the correct number of success log records were added.
			const int lastSuccessLogIndexResult = pbSuccessLog->GetIndex();

			{
				// CLogs allows the index to be out of range when index==maxLogs.
				bool areEqual = lastSuccessLogIndexResult == logIndexInfoExpected.m_successLogIndexExpected ||
					(lastSuccessLogIndexResult == maxSuccessLogsResult && logIndexInfoExpected.m_successLogIndexExpected == 0);

				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED lastSuccessLogIndexResult=%d expected=%d start=%d max=%d testIndex=%d from %s"),
					(int)lastSuccessLogIndexResult,
					(int)logIndexInfoExpected.m_successLogIndexExpected,
					(int)logIndexInfoExpected.m_successLogIndexStart,
					(int)logIndexInfoExpected.m_maxSuccessLogsExpected,
					(int)testIndex,
					static_cast<LPCTSTR>(caller));
				ASSERT_TRUE(areEqual) << failedMsg;
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBanker::GetLastLog()
		/// Code coverage test after CPersonalBanker::CPersonalBanker().
		/// </summary>
		TEST(PersonalBanker3, GetLastLog1Test)
		{
			// Test data.
			const CPersonalBankerLog * const lastLogExpected(NULL);

			// More test data - expected results (that are the same for each iteration).
			const int numberOfLogRecordsAdded(0);
			const int numberOfSuccessLogRecordsAdded(0);

			// More test data - expected results that are dependent on previous tests.
			LogIndexInfoExpected logIndexInfoExpected;
			LogTransfer_GetLogIndexInfoExpected(logIndexInfoExpected,
				numberOfLogRecordsAdded,
				numberOfSuccessLogRecordsAdded);

			// Setup.
			const int testIndex = 0;
			CPersonalBanker varPersonalBanker;

			// Perform operations to be tested.
			const CPersonalBankerLog * const lastLogResult = varPersonalBanker.GetLastLogCopy();
			const CPersonalBanker * const personalBanker = &varPersonalBanker;

			// Test log and log indexes info.
			CLogs *pbLog = varPersonalBanker.GetLogCopy();
			CLogs *pbSuccessLog = varPersonalBanker.GetSuccessLogCopy();
			LogTransfer_AssertLogInfoExpected(pbLog,
				pbSuccessLog,
				logIndexInfoExpected,
				testIndex,
				__TFUNCTION__);

			// Cleanup.
			if(lastLogResult != NULL)
			{
				delete lastLogResult;
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBanker::GetLastLog()
		/// Code coverage test after CPersonalBanker::LogTransfer() with player == NULL.
		/// </summary>
		TEST(PersonalBanker3, GetLastLog2Test)
		{
			// Test data.
			const CPersonalBankerLog * const lastLogExpected(NULL);
			const CConfig config(false, "");
			
			// More test data - expected results (that are the same for each iteration).
			const int numberOfLogRecordsAdded(0);
			const int numberOfSuccessLogRecordsAdded(0);

			// More test data - expected results that are dependent on previous tests.
			LogIndexInfoExpected logIndexInfoExpected;
			LogTransfer_GetLogIndexInfoExpected(logIndexInfoExpected,
				numberOfLogRecordsAdded,
				numberOfSuccessLogRecordsAdded);

			// Setup.
			const int testIndex = 0;
			CPersonalBanker varPersonalBanker;
			CPlayer *player(NULL);

			// Perform operations to be tested.
			varPersonalBanker.LogTransfer(player, config, CUtilities::GetCurrentTime());
			const CPersonalBankerLog * const lastLogResult = varPersonalBanker.GetLastLogCopy();
			const CPersonalBanker * const personalBanker = &varPersonalBanker;

			// Test log and log indexes info.
			CLogs *pbLog = varPersonalBanker.GetLogCopy();
			CLogs *pbSuccessLog = varPersonalBanker.GetSuccessLogCopy();
			LogTransfer_AssertLogInfoExpected(pbLog,
				pbSuccessLog,
				logIndexInfoExpected,
				testIndex,
				__TFUNCTION__);

			// Cleanup.
			if(lastLogResult != NULL)
			{
				delete lastLogResult;
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBanker::GetLastLog()
		/// Code coverage test after CPersonalBanker::LogTransfer() with m_pbTransfer == NULL.
		/// </summary>
		TEST(PersonalBanker3, GetLastLog3Test)
		{
			// Test data.
			const CPersonalBankerLog * const lastLogExpected(NULL);
			const CConfig config(false, "");
			
			// More test data - expected results (that are the same for each iteration).
			const int numberOfLogRecordsAdded(0);
			const int numberOfSuccessLogRecordsAdded(0);

			// More test data - expected results that are dependent on previous tests.
			LogIndexInfoExpected logIndexInfoExpected;
			LogTransfer_GetLogIndexInfoExpected(logIndexInfoExpected,
				numberOfLogRecordsAdded,
				numberOfSuccessLogRecordsAdded);

			// Setup.
			const int testIndex = 0;
			CPersonalBanker varPersonalBanker;
			varPersonalBanker.DoneWithTransfer();
			CPlayer player;

			// Perform operations to be tested.
			varPersonalBanker.LogTransfer(&player, config, CUtilities::GetCurrentTime());
			const CPersonalBankerLog * const lastLogResult = varPersonalBanker.GetLastLogCopy();
			const CPersonalBanker * const personalBanker = &varPersonalBanker;

			// Test log and log indexes info.
			CLogs *pbLog = varPersonalBanker.GetLogCopy();
			CLogs *pbSuccessLog = varPersonalBanker.GetSuccessLogCopy();
			LogTransfer_AssertLogInfoExpected(pbLog,
				pbSuccessLog,
				logIndexInfoExpected,
				testIndex,
				__TFUNCTION__);

			// Cleanup.
			if(lastLogResult != NULL)
			{
				delete lastLogResult;
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBanker::LogTransfer()
		///     CPersonalBanker::GetLastLog()
		///     CPersonalBanker::GetSuccessLogCopy()
		/// </summary>
		/// <remarks>
		/// Test with player NULL.
		/// </remarks>
		TEST(PersonalBanker3, LogTransfer_NullPlayerTest)
		{
			// Test data - inputs.
			
			// More test data - expected results that are the same for each iteration.
			const int numberOfLogRecordsAdded(0);
			const int numberOfSuccessLogRecordsAdded(0);

			// More test data - expected results that are dependent on previous tests.
			LogIndexInfoExpected logIndexInfoExpected;
			LogTransfer_GetLogIndexInfoExpected(logIndexInfoExpected,
				numberOfLogRecordsAdded,
				numberOfSuccessLogRecordsAdded);

			// Setup.
			const int testIndex = 0;
			CPlayer *player(NULL);
			const CConfig config(false, "");
			CPersonalBanker varPb;

			// Perform operations to be tested.
			varPb.LogTransfer(player, config, CUtilities::GetCurrentTime());
			const CPersonalBanker &pb(varPb);

			// Test log and log indexes info.
			CLogs *pbLog = varPb.GetLogCopy();
			CLogs *pbSuccessLog = varPb.GetSuccessLogCopy();
			LogTransfer_AssertLogInfoExpected(pbLog,
				pbSuccessLog,
				logIndexInfoExpected,
				testIndex,
				__TFUNCTION__);

			// Cleanup.
			delete pbLog;
			delete pbSuccessLog;
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBanker::LogTransfer()
		///     CPersonalBanker::GetLastLog()
		///     CPersonalBanker::GetSuccessLogCopy()
		/// </summary>
		/// <remarks>
		/// Test with CPersonalBanker::m_pbTransfer NULL.
		/// </remarks>
		TEST(PersonalBanker3, LogTransfer_NullTransferTest)
		{
			
			// More test data - expected results that are the same for each iteration.
			const int numberOfLogRecordsAdded(0);
			const int numberOfSuccessLogRecordsAdded(0);

			// More test data - expected results that are dependent on previous tests.
			LogIndexInfoExpected logIndexInfoExpected;
			LogTransfer_GetLogIndexInfoExpected(logIndexInfoExpected,
				numberOfLogRecordsAdded,
				numberOfSuccessLogRecordsAdded);

			// Setup.
			const int testIndex = 0;
			CPlayer player;
			const CConfig config(false, "");
			CPersonalBanker varPb;

			// Perform operations to be tested.
			varPb.LogTransfer(&player, config, CUtilities::GetCurrentTime());
			const CPersonalBanker &pb(varPb);

			// Test log and log indexes info.
			CLogs *pbLog = varPb.GetLogCopy();
			CLogs *pbSuccessLog = varPb.GetSuccessLogCopy();
			LogTransfer_AssertLogInfoExpected(pbLog,
				pbSuccessLog,
				logIndexInfoExpected,
				testIndex,
				__TFUNCTION__);

			// Cleanup.
			delete pbLog;
			delete pbSuccessLog;
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBanker::LogTransfer()
		///     CPersonalBanker::GetLastLog()
		///     CPersonalBanker::GetSuccessLogCopy()
		/// </summary>
		/// <remarks>
		/// Test with CPersonalBanker::m_pbTransfer default.
		/// </remarks>
		TEST(PersonalBanker3, LogTransfer_TransferDefaultTest)
		{
			// More test data - expected results that are the same for each iteration.
			const int numberOfLogRecordsAdded(0);
			const int numberOfSuccessLogRecordsAdded(0);

			// More test data - expected results that are dependent on previous tests.
			LogIndexInfoExpected logIndexInfoExpected;
			LogTransfer_GetLogIndexInfoExpected(logIndexInfoExpected,
				numberOfLogRecordsAdded,
				numberOfSuccessLogRecordsAdded);

			// Setup.
			const int testIndex = 0;
			CPlayer player;
			const CConfig config(false, "");
			CPersonalBanker varPb;
			varPb.CreateTransfer(CUtilities::GetTickCount());

			// Perform operations to be tested.
			varPb.LogTransfer(&player, config, CUtilities::GetCurrentTime());
			const CPersonalBanker &pb(varPb);

			// Test log and log indexes info.
			CLogs *pbLog = varPb.GetLogCopy();
			CLogs *pbSuccessLog = varPb.GetSuccessLogCopy();
			LogTransfer_AssertLogInfoExpected(pbLog,
				pbSuccessLog,
				logIndexInfoExpected,
				testIndex,
				__TFUNCTION__);

			// Cleanup.
			delete pbLog;
			delete pbSuccessLog;
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBanker::LogTransfer()
		///     CPersonalBanker::GetLastLog()
		///     CPersonalBanker::GetSuccessLogCopy()
		/// </summary>
		/// <remarks>
		/// Test with canceled cash transfers.
		/// Does NOT do a full equality check between the transfer instance the the log instance.
		/// Note: Code used for detection of log records is complicated by what other tests do to the logs.
		/// </remarks>
		TEST(PersonalBanker3, LogTransfer_TransferCanceledCashTest)
		{
			typedef struct
			{
				// Inputs.
				const PersonalBankerTransferType m_transferType;
                const time_t m_currentTime;
				const DWORD m_amountInPennies;

				// Expected results.
				const int m_lastSourceExpected;
				const time_t m_lastDateTimeExpected;
				const __int64 m_lastAmountExpected;

			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				// Non-zero cash amounts.
				{ Undeclared, time_t(123), 100, CashFromGame, time_t(123), 100 },
				{ CashToGame, time_t(1232), 200, CashToGame, time_t(1232), 200 },
				{ PointsToGame, time_t(1233), 300, PointsToGame, time_t(1233), 300 },
				{ PromoDollarsToGame, time_t(1234), 400, PromoDollarsToGame, time_t(1234), 400 },
				{ CashFromGame, time_t(12345), 500, CashFromGame, time_t(12345), 500 },
				{ PointsFromGame, time_t(123456), 600, CashFromGame, time_t(123456), 600 },
				{ PromoDollarsFromGame, time_t(1234567), 700, CashFromGame, time_t(1234567), 700 },
				{ AllFromGame, time_t(12345678), 800, CashFromGame, time_t(12345678), 800 },
				{ (PersonalBankerTransferType)(ExternalCredit+1), time_t(12345677), 7700, CashFromGame, time_t(12345677), 7700 },
				{ (PersonalBankerTransferType)-1, time_t(123456789), 900, CashFromGame, time_t(123456789), 900 },

				// Zero cash amounts.
				{ Undeclared, time_t(123), 0, CashFromGame, time_t(123), 0 },
				{ CashToGame, time_t(1232), 0, CashToGame, time_t(1232), 0 },
				{ PointsToGame, time_t(1233), 0, PointsToGame, time_t(1233), 0 },
				{ PromoDollarsToGame, time_t(1234), 0, PromoDollarsToGame, time_t(1234), 0 },
				{ CashFromGame, time_t(12345), 0, CashFromGame, time_t(12345), 0 },
				{ PointsFromGame, time_t(123456), 0, CashFromGame, time_t(123456), 0 },
				{ PromoDollarsFromGame, time_t(1234567), 0, CashFromGame, time_t(1234567), 0 },
				{ AllFromGame, time_t(12345678), 0, CashFromGame, time_t(12345678), 0 },
				{ (PersonalBankerTransferType)(ExternalCredit+1), time_t(12345677), 0, CashFromGame, time_t(12345677), 0 },
				{ (PersonalBankerTransferType)-1, time_t(123456789), 0, CashFromGame, time_t(123456789), 0 }
			};

			// More test data - expected results that are the same for each iteration.
			const int numberOfLogRecordsAdded(1);
			const int numberOfSuccessLogRecordsAdded(0);
			const bool lastSuccessfulExpected(false);
			const bool lastPendingExpected(true);

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// More test data - expected results that are dependent on previous tests.
				LogIndexInfoExpected logIndexInfoExpected;
				LogTransfer_GetLogIndexInfoExpected(logIndexInfoExpected,
					numberOfLogRecordsAdded,
					numberOfSuccessLogRecordsAdded);

				// Setup.
				CGames games;
				CPlayer player;
				const CConfig config(false, "");
				CPersonalBanker varPb;
				varPb.CreateTransfer(CUtilities::GetTickCount());

				// SetState() to affect BeginPersonalBankerTransfer().
				varPb.SetState(PBStateInfoReceived);
				byte pbEFTNumber = 1;
				varPb.BeginPersonalBankerTransfer(games,
					TEST_DATA[testIndex].m_transferType,
					TEST_DATA[testIndex].m_amountInPennies,
					pbEFTNumber);
				varPb.SetPersonalBankerType(CanceledSessionRequest);

				// Perform operations to be tested.
				varPb.LogTransfer(&player, config, TEST_DATA[testIndex].m_currentTime);
				const CPersonalBanker &pb(varPb);

				// Test log and log indexes info.
				CLogs *pbLog = varPb.GetLogCopy();
				CLogs *pbSuccessLog = varPb.GetSuccessLogCopy();
				LogTransfer_AssertLogInfoExpected(pbLog,
					pbSuccessLog,
					logIndexInfoExpected,
					testIndex,
					__TFUNCTION__);


				// Test #1.
				CPersonalBankerLog *lastLog((CPersonalBankerLog *)pbLog->GetLastLog());

				{
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED lastLog=%p expected=NOT NULL testIndex=%d in %s"),
						(const void *)lastLog,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_NE(nullptr, lastLog) << failedMsg;
				}

				// Could do a full equality check between the transfer instance the the log instance.
				// Test #2.
				const time_t lastDateTimeResult = lastLog->GetDateTime();

				{
					bool areEqual = lastDateTimeResult == TEST_DATA[testIndex].m_lastDateTimeExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED lastDateTimeResult=%lld expected=%lld testIndex=%d in %s"),
						(long long)lastDateTimeResult,
						(long long)TEST_DATA[testIndex].m_lastDateTimeExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #3.
				const __int64 lastAmountResult = lastLog->GetAmount();

				{
					bool areEqual = lastAmountResult == TEST_DATA[testIndex].m_lastAmountExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED lastAmountResult=%lld expected=%lld testIndex=%d in %s"),
						(long long)lastAmountResult,
						(long long)TEST_DATA[testIndex].m_lastAmountExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #4.
				const int lastSourceResult = lastLog->GetSource();

				{
					bool areEqual = lastSourceResult == TEST_DATA[testIndex].m_lastSourceExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED lastSourceResult=%d expected=%d testIndex=%d in %s"),
						(int)lastSourceResult,
						(int)TEST_DATA[testIndex].m_lastSourceExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #5.
				const bool lastSuccessfulResult = lastLog->IsSuccessful();

				{
					bool areEqual = lastSuccessfulResult == lastSuccessfulExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED lastSuccessfulResult=%d expected=%d testIndex=%d in %s"),
						(int)lastSuccessfulResult,
						(int)lastSuccessfulExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #6.
				const bool lastPendingResult = lastLog->IsPending();

				{
					bool areEqual = lastPendingResult == lastPendingExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED lastPendingResult=%d expected=%d testIndex=%d in %s"),
						(int)lastPendingResult,
						(int)lastPendingExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
				delete pbLog;
				delete pbSuccessLog;
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBanker::LogTransfer()
		///     CPersonalBanker::GetLastLog()
		///     CPersonalBanker::GetSuccessLogCopy()
		/// </summary>
		/// <remarks>
		/// Test with canceled cash and non-restricted transfers.
		/// Does NOT do a full equality check between the transfer instance the the log instance.
		/// Note: Code used for detection of log records is complicated by what other tests do to the logs.
		/// </remarks>
		TEST(PersonalBanker3, LogTransfer_TransferCanceledNonRestrictedTest)
		{
			typedef struct
			{
				// Inputs.
				const PersonalBankerTransferType m_transferType;
				const time_t m_currentTime;
				const DWORD m_amountInDollars;
				const __int64 m_nonRestrictedAmount;

				// Expected results.
				const int m_numberOfLogRecordsAddedExpected;
				const int m_cashSourceExpected;
				const time_t m_lastDateTimeExpected;
				const __int64 m_cashAmountExpected;
				const __int64 m_cepAmountExpected;
				const int m_cepSourceExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				// Non-zero cash amounts.
				{ Undeclared, time_t(123), 1, 10, 2, CashFromGame, time_t(123), 100, 10, PointsFromGame },
				{ CashToGame, time_t(1232), 2, 1, 1, CashToGame, time_t(1232), 200, 0, PointsFromGame },
				{ PointsToGame, time_t(1233), 3, 23, 1, PointsToGame, time_t(1233), 300, 0, PointsFromGame },
				{ PromoDollarsToGame, time_t(1234), 4, 34, 1, PromoDollarsToGame, time_t(1234), 400, 0, PointsFromGame },
				{ CashFromGame, time_t(12345), 5, 45, 2, CashFromGame, time_t(12345), 500, 45, PointsFromGame },
				{ PointsFromGame, time_t(123456), 6, 56, 2, CashFromGame, time_t(123456), 600, 56, PointsFromGame },
				{ PromoDollarsFromGame, time_t(1234567), 7, 67, 2, CashFromGame, time_t(1234567), 700, 67, PointsFromGame },
				{ AllFromGame, time_t(12345678), 8, 78, 2, CashFromGame, time_t(12345678), 800, 78, PointsFromGame },
				{ (PersonalBankerTransferType)(AllFromGame+1), time_t(12345677), 77, 777, 2, CashFromGame, time_t(12345677), 7700, 777, PointsFromGame },
				{ (PersonalBankerTransferType)-1, time_t(123456789), 9, 999, 2, CashFromGame, time_t(123456789), 900, 999, PointsFromGame },

				// Zero cash amounts.
				{ Undeclared, time_t(123), 0, 12, 2, CashFromGame, time_t(123), 0, 12, PointsFromGame },
				{ CashToGame, time_t(1232), 0, 23, 1, CashToGame, time_t(1232), 0, 0, PointsFromGame },
				{ PointsToGame, time_t(1233), 0, 34, 1, PointsToGame, time_t(1233), 0, 0, PointsFromGame },
				{ PromoDollarsToGame, time_t(1234), 0, 45, 1, PromoDollarsToGame, time_t(1234), 0, 0, PointsFromGame },
				{ CashFromGame, time_t(12345), 0, 56, 2, CashFromGame, time_t(12345), 0, 56, PointsFromGame },
				{ PointsFromGame, time_t(123456), 0, 67, 2, CashFromGame, time_t(123456), 0, 67, PointsFromGame },
				{ PromoDollarsFromGame, time_t(1234567), 0, 78, 2, CashFromGame, time_t(1234567), 0, 78, PointsFromGame },
				{ AllFromGame, time_t(12345678), 0, 89, 2, CashFromGame, time_t(12345678), 0, 89, PointsFromGame },
				{ (PersonalBankerTransferType)(AllFromGame+1), time_t(12345677), 0, 901, 2, CashFromGame, time_t(12345677), 0, 901, PointsFromGame },
				{ (PersonalBankerTransferType)-1, time_t(123456789), 0, 902, 2, CashFromGame, time_t(123456789), 0, 902, PointsFromGame },
			};

			// More test data - expected results that are the same for each iteration.
			const int numberOfSuccessLogRecordsAdded(0);
			const bool lastSuccessfulExpected(false);
			const bool lastPendingExpected(true);

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// More test data - expected results that are dependent on previous tests.
				LogIndexInfoExpected logIndexInfoExpected;
				LogTransfer_GetLogIndexInfoExpected(logIndexInfoExpected,
					TEST_DATA[testIndex].m_numberOfLogRecordsAddedExpected,
					numberOfSuccessLogRecordsAdded);

				// Setup.
				CGames games;
				CPlayer player;
				const CConfig config(false, "");
				CPersonalBanker varPb;
				varPb.CreateTransfer(CUtilities::GetTickCount());

				// SetState() to affect BeginPersonalBankerTransfer().
				varPb.SetState(PBStateInfoReceived);
				byte pbEFTNumber = 1;
				varPb.BeginPersonalBankerTransfer(games,
					TEST_DATA[testIndex].m_transferType,
					TEST_DATA[testIndex].m_amountInDollars,
					pbEFTNumber);

				// SetState() to affect ProcessPersonalBankerTransferComplete().
				varPb.SetState(PBStateTransferingFromToGame);

				CPersonalBankerTransfer *pbTransfer = varPb.GetTransferCopy();
				ASSERT_NE(nullptr, pbTransfer) << _T("FAILED pbTransfer NULL, expected NOT NULL");
				pbTransfer->SetAmount(TEST_DATA[testIndex].m_amountInDollars * ONE_DOLLAR_IN_CENTS);
				pbTransfer->SetNonrestrictedAmount(TEST_DATA[testIndex].m_nonRestrictedAmount);
				pbTransfer->SetRestrictedAmount(0);
				byte meterNumberToAddTo(0xff);
				bool isDuplicate(false);
				const bool isEFT(false);
				varPb.ProcessPersonalBankerTransferComplete(pbTransfer,
					games,
					meterNumberToAddTo,
					isDuplicate,
					isEFT);

				varPb.SetPersonalBankerType(CanceledSessionRequest);

				// Perform operations to be tested.
				varPb.LogTransfer(&player, config, TEST_DATA[testIndex].m_currentTime);
				const CPersonalBanker &pb(varPb);

				// Test log and log indexes info.
				CLogs *pbLog = varPb.GetLogCopy();
				CLogs *pbSuccessLog = varPb.GetSuccessLogCopy();
				LogTransfer_AssertLogInfoExpected(pbLog,
					pbSuccessLog,
					logIndexInfoExpected,
					testIndex,
					__TFUNCTION__);

				// Get log records for subsequent tests.
				CPersonalBankerLog *cepLog(NULL);
				CPersonalBankerLog *cashLog(NULL);
				if (1 == TEST_DATA[testIndex].m_numberOfLogRecordsAddedExpected)
				{
					cashLog = (CPersonalBankerLog *)pbLog->GetLastLog();
				}
				else if (2 == TEST_DATA[testIndex].m_numberOfLogRecordsAddedExpected)
				{
					cepLog = (CPersonalBankerLog *)pbLog->GetLastLog();
					cashLog = (CPersonalBankerLog *)pbLog->GetPreviousLog();
				}
				else
				{
					ASSERT_TRUE(false) << _T("FAILED m_numberOfLogRecordsAddedExpected not in 1..2");
				}

				// Test #A.
				{
					bool areEqual = !!cepLog == !!TEST_DATA[testIndex].m_cepAmountExpected;
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED cepLog=%p m_cepAmountExpected=%lld testIndex=%d in %s"),
							(const void *)cepLog,
							(long long)TEST_DATA[testIndex].m_cepAmountExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #1.
				{
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED cashLog=%p expected=NOT NULL testIndex=%d in %s"),
						(const void *)cashLog,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_NE(nullptr, cashLog) << failedMsg;
				}

				// Could do a full equality check between the transfer instance the the log instance.
				// Test #2.
				const time_t cashDateTimeResult = cashLog->GetDateTime();

				{
					bool areEqual = cashDateTimeResult == TEST_DATA[testIndex].m_lastDateTimeExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED cashDateTimeResult=%lld expected=%lld testIndex=%d in %s"),
						(long long)cashDateTimeResult,
						(long long)TEST_DATA[testIndex].m_lastDateTimeExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #3.
				const __int64 cashAmountResult = cashLog->GetAmount();

				{
					bool areEqual = cashAmountResult == TEST_DATA[testIndex].m_cashAmountExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED cashAmountResult=%lld expected=%lld testIndex=%d in %s"),
						(long long)cashAmountResult,
						(long long)TEST_DATA[testIndex].m_cashAmountExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #4.
				const int cashSourceResult = cashLog->GetSource();

				{
					bool areEqual = cashSourceResult == TEST_DATA[testIndex].m_cashSourceExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED cashSourceResult=%d expected=%d testIndex=%d in %s"),
						(int)cashSourceResult,
						(int)TEST_DATA[testIndex].m_cashAmountExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #5.
				const bool cashSuccessfulResult = cashLog->IsSuccessful();

				{
					bool areEqual = cashSuccessfulResult == lastSuccessfulExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED cashSuccessfulResult=%d expected=%d testIndex=%d in %s"),
						(int)cashSuccessfulResult,
						(int)lastSuccessfulExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #6.
				const bool cashPendingResult = cashLog->IsPending();

				{
					bool areEqual = cashPendingResult == lastPendingExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED cashPendingResult=%d expected=%d testIndex=%d in %s"),
						(int)cashPendingResult,
						(int)lastPendingExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}
				
				if (TEST_DATA[testIndex].m_numberOfLogRecordsAddedExpected >= 2)
				{
					// Test #7.
					{
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED cepLog=%p expected=NOT NULL testIndex=%d in %s"),
							(const void *)cepLog,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_NE(nullptr, cepLog) << failedMsg;
					}

					// Could do a full equality check between the transfer instance the the log instance.
					// Test #8.
					const time_t cepDateTimeResult = cepLog->GetDateTime();

					{
						bool areEqual = cepDateTimeResult == TEST_DATA[testIndex].m_lastDateTimeExpected;
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED cepDateTimeResult=%lld expected=%lld testIndex=%d in %s"),
							(long long)cepDateTimeResult,
							(long long)TEST_DATA[testIndex].m_lastDateTimeExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}

					// Test #9.
					const __int64 cepAmountResult = cepLog->GetAmount();

					{
						bool areEqual = cepAmountResult == TEST_DATA[testIndex].m_cepAmountExpected;
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED cepAmountResult=%lld expected=%lld testIndex=%d in %s"),
							(long long)cepAmountResult,
							(long long)TEST_DATA[testIndex].m_cepAmountExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}

					// Test #10.
					const int cepSourceResult = cepLog->GetSource();

					{
						bool areEqual = cepSourceResult == TEST_DATA[testIndex].m_cepSourceExpected;
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED cepSourceResult=%d expected=%d testIndex=%d in %s"),
							(int)cepSourceResult,
							(int)TEST_DATA[testIndex].m_cepSourceExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}

					// Test #11.
					const bool cepSuccessfulResult = cepLog->IsSuccessful();

					{
						bool areEqual = cepSuccessfulResult == lastSuccessfulExpected;
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED cepSuccessfulResult=%d expected=%d testIndex=%d in %s"),
							(int)cepSuccessfulResult,
							(int)lastSuccessfulExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}

					// Test #12.
					const bool cepPendingResult = cepLog->IsPending();

					{
						bool areEqual = cepPendingResult == lastPendingExpected;
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED cepPendingResult=%d expected=%d testIndex=%d in %s"),
							(int)cepPendingResult,
							(int)lastPendingExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}
				}

				// Cleanup.
				delete pbLog;
				delete pbSuccessLog;
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBanker::LogTransfer()
		///     CPersonalBanker::GetLastLog()
		///     CPersonalBanker::GetSuccessLogCopy()
		/// </summary>
		/// <remarks>
		/// Test with canceled cash and restricted transfers.
		/// Does NOT do a full equality check between the transfer instance the the log instance.
		/// Note: Code used for detection of log records is complicated by what other tests do to the logs.
		/// </remarks>
		TEST(PersonalBanker3, LogTransfer_TransferCanceledRestrictedTest)
		{
			typedef struct
			{
				// Inputs.
				const PersonalBankerTransferType m_transferType;
				const time_t m_currentTime;
				const DWORD m_amountInDollars;
				const __int64 m_restrictedAmount;

				// Expected results.
				const int m_numberOfLogRecordsAddedExpected;
				const int m_cashSourceExpected;
				const time_t m_lastDateTimeExpected;
				const __int64 m_cashAmountExpected;
				const __int64 m_ncepAmountExpected;
				const int m_ncepSourceExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				// Non-zero cash amounts.
				{ Undeclared, time_t(123), 1, 10, 2, CashFromGame, time_t(123), 100, 10, PromoDollarsFromGame },
				{ CashToGame, time_t(1232), 2, 1, 1, CashToGame, time_t(1232), 200, 0, PromoDollarsFromGame },
				{ PointsToGame, time_t(1233), 3, 23, 1, PointsToGame, time_t(1233), 300, 0, PromoDollarsFromGame },
				{ PromoDollarsToGame, time_t(1234), 4, 34, 1, PromoDollarsToGame, time_t(1234), 400, 0, PromoDollarsFromGame },
				{ CashFromGame, time_t(12345), 5, 45, 2, CashFromGame, time_t(12345), 500, 45, PromoDollarsFromGame },
				{ PointsFromGame, time_t(123456), 6, 56, 2, CashFromGame, time_t(123456), 600, 56, PromoDollarsFromGame },
				{ PromoDollarsFromGame, time_t(1234567), 7, 67, 2, CashFromGame, time_t(1234567), 700, 67, PromoDollarsFromGame },
				{ AllFromGame, time_t(12345678), 8, 78, 2, CashFromGame, time_t(12345678), 800, 78, PromoDollarsFromGame },
				{ (PersonalBankerTransferType)(AllFromGame+1), time_t(12345677), 77, 777, 2, CashFromGame, time_t(12345677), 7700, 777, PromoDollarsFromGame },
				{ (PersonalBankerTransferType)-1, time_t(123456789), 9, 999, 2, CashFromGame, time_t(123456789), 900, 999, PromoDollarsFromGame },

				// Zero cash amounts.
				{ Undeclared, time_t(123), 0, 12, 2, CashFromGame, time_t(123), 0, 12, PromoDollarsFromGame },
				{ CashToGame, time_t(1232), 0, 23, 1, CashToGame, time_t(1232), 0, 0, PromoDollarsFromGame },
				{ PointsToGame, time_t(1233), 0, 34, 1, PointsToGame, time_t(1233), 0, 0, PromoDollarsFromGame },
				{ PromoDollarsToGame, time_t(1234), 0, 45, 1, PromoDollarsToGame, time_t(1234), 0, 0, PromoDollarsFromGame },
				{ CashFromGame, time_t(12345), 0, 56, 2, CashFromGame, time_t(12345), 0, 56, PromoDollarsFromGame },
				{ PointsFromGame, time_t(123456), 0, 67, 2, CashFromGame, time_t(123456), 0, 67, PromoDollarsFromGame },
				{ PromoDollarsFromGame, time_t(1234567), 0, 78, 2, CashFromGame, time_t(1234567), 0, 78, PromoDollarsFromGame },
				{ AllFromGame, time_t(12345678), 0, 89, 2, CashFromGame, time_t(12345678), 0, 89, PromoDollarsFromGame },
				{ (PersonalBankerTransferType)(AllFromGame+1), time_t(12345677), 0, 901, 2, CashFromGame, time_t(12345677), 0, 901, PromoDollarsFromGame },
				{ (PersonalBankerTransferType)-1, time_t(123456789), 0, 902, 2, CashFromGame, time_t(123456789), 0, 902, PromoDollarsFromGame },
			};

			// More test data - expected results that are the same for each iteration.
			const int numberOfSuccessLogRecordsAdded(0);
			const bool lastSuccessfulExpected(false);
			const bool lastPendingExpected(true);

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// More test data - expected results that are dependent on previous tests.
				LogIndexInfoExpected logIndexInfoExpected;
				LogTransfer_GetLogIndexInfoExpected(logIndexInfoExpected,
					TEST_DATA[testIndex].m_numberOfLogRecordsAddedExpected,
					numberOfSuccessLogRecordsAdded);

				// Setup.
				CGames games;
				CPlayer player;
				const CConfig config(false, "");
				CPersonalBanker varPb;
				varPb.CreateTransfer(CUtilities::GetTickCount());

				// SetState() to affect BeginPersonalBankerTransfer().
				varPb.SetState(PBStateInfoReceived);
				byte pbEFTNumber = 1;
				varPb.BeginPersonalBankerTransfer(games,
					TEST_DATA[testIndex].m_transferType,
					TEST_DATA[testIndex].m_amountInDollars,
					pbEFTNumber);

				// SetState() to affect ProcessPersonalBankerTransferComplete().
				varPb.SetState(PBStateTransferingFromToGame);

				CPersonalBankerTransfer *pbTransfer = varPb.GetTransferCopy();
				ASSERT_NE(nullptr, pbTransfer) << _T("FAILED pbTransfer NULL, expected NOT NULL");
				pbTransfer->SetAmount(TEST_DATA[testIndex].m_amountInDollars * ONE_DOLLAR_IN_CENTS);
				pbTransfer->SetNonrestrictedAmount(0);
				pbTransfer->SetRestrictedAmount(TEST_DATA[testIndex].m_restrictedAmount);
				byte meterNumberToAddTo(0xff);
				bool isDuplicate(false);
				const bool isEFT(false);
				varPb.ProcessPersonalBankerTransferComplete(pbTransfer,
					games,
					meterNumberToAddTo,
					isDuplicate,
					isEFT);

				varPb.SetPersonalBankerType(CanceledSessionRequest);

				// Perform operations to be tested.
				varPb.LogTransfer(&player, config, TEST_DATA[testIndex].m_currentTime);
				const CPersonalBanker &pb(varPb);

				// Test log and log indexes info.
				CLogs *pbLog = varPb.GetLogCopy();
				CLogs *pbSuccessLog = varPb.GetSuccessLogCopy();
				LogTransfer_AssertLogInfoExpected(pbLog,
					pbSuccessLog,
					logIndexInfoExpected,
					testIndex,
					__TFUNCTION__);

				// Get log records for subsequent tests.
				CPersonalBankerLog *ncepLog(NULL);
				CPersonalBankerLog *cashLog(NULL);
				if (1 == TEST_DATA[testIndex].m_numberOfLogRecordsAddedExpected)
				{
					cashLog = (CPersonalBankerLog *)pbLog->GetLastLog();
				}
				else if (2 == TEST_DATA[testIndex].m_numberOfLogRecordsAddedExpected)
				{
					ncepLog = (CPersonalBankerLog *)pbLog->GetLastLog();
					cashLog = (CPersonalBankerLog *)pbLog->GetPreviousLog();
				}
				else
				{
					ASSERT_TRUE(false) << _T("FAILED m_numberOfLogRecordsAddedExpected not in 1..2");
				}

				// Test #A.
				{
					bool areEqual = !!ncepLog == !!TEST_DATA[testIndex].m_ncepAmountExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED ncepLog=%p m_ncepAmountExpected=%lld testIndex=%d in %s"),
						(const void *)ncepLog,
						(long long)TEST_DATA[testIndex].m_ncepAmountExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #1.
				{
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED cashLog=%p expected=NOT NULL testIndex=%d in %s"),
						(const void *)cashLog,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_NE(nullptr, cashLog) << failedMsg;
				}

				// Could do a full equality check between the transfer instance the the log instance.
				// Test #2.
				const time_t cashDateTimeResult = cashLog->GetDateTime();

				{
					bool areEqual = cashDateTimeResult == TEST_DATA[testIndex].m_lastDateTimeExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED cashDateTimeResult=%lld expected=%lld testIndex=%d in %s"),
						(long long)cashDateTimeResult,
						(long long)TEST_DATA[testIndex].m_lastDateTimeExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #3.
				const __int64 cashAmountResult = cashLog->GetAmount();

				{
					bool areEqual = cashAmountResult == TEST_DATA[testIndex].m_cashAmountExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED cashAmountResult=%lld expected=%lld testIndex=%d in %s"),
						(long long)cashAmountResult,
						(long long)TEST_DATA[testIndex].m_cashAmountExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #4.
				const int cashSourceResult = cashLog->GetSource();

				{
					bool areEqual = cashSourceResult == TEST_DATA[testIndex].m_cashSourceExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED cashSourceResult=%d expected=%d testIndex=%d in %s"),
						(int)cashSourceResult,
						(int)TEST_DATA[testIndex].m_cashAmountExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #5.
				const bool cashSuccessfulResult = cashLog->IsSuccessful();

				{
					bool areEqual = cashSuccessfulResult == lastSuccessfulExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED cashSuccessfulResult=%d expected=%d testIndex=%d in %s"),
						(int)cashSuccessfulResult,
						(int)lastSuccessfulExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #6.
				const bool cashPendingResult = cashLog->IsPending();

				{
					bool areEqual = cashPendingResult == lastPendingExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED cashPendingResult=%d expected=%d testIndex=%d in %s"),
						(int)cashPendingResult,
						(int)lastPendingExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}
				
				if (TEST_DATA[testIndex].m_numberOfLogRecordsAddedExpected >= 2)
				{
					// Test #7.
					{
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED cepLog=%p expected=NOT NULL testIndex=%d in %s"),
							(const void *)ncepLog,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_NE(nullptr, ncepLog) << failedMsg;
					}

					// Could do a full equality check between the transfer instance the the log instance.
					// Test #8.
					const time_t ncepDateTimeResult = ncepLog->GetDateTime();

					{
						bool areEqual = ncepDateTimeResult == TEST_DATA[testIndex].m_lastDateTimeExpected;
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED cepDateTimeResult=%lld expected=%lld testIndex=%d in %s"),
							(long long)ncepDateTimeResult,
							(long long)TEST_DATA[testIndex].m_lastDateTimeExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}

					// Test #9.
					const __int64 ncepAmountResult = ncepLog->GetAmount();

					{
						bool areEqual = ncepAmountResult == TEST_DATA[testIndex].m_ncepAmountExpected;
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED ncepAmountResult=%lld expected=%lld testIndex=%d in %s"),
							(long long)ncepAmountResult,
							(long long)TEST_DATA[testIndex].m_ncepAmountExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}

					// Test #10.
					const int ncepSourceResult = ncepLog->GetSource();

					{
						bool areEqual = ncepSourceResult == TEST_DATA[testIndex].m_ncepSourceExpected;
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED ncepSourceResult=%d expected=%d testIndex=%d in %s"),
							(int)ncepSourceResult,
							(int)TEST_DATA[testIndex].m_ncepSourceExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}

					// Test #11.
					const bool ncepSuccessfulResult = ncepLog->IsSuccessful();

					{
						bool areEqual = ncepSuccessfulResult == lastSuccessfulExpected;
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED ncepSuccessfulResult=%d expected=%d testIndex=%d in %s"),
							(int)ncepSuccessfulResult,
							(int)lastSuccessfulExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}

					// Test #12.
					const bool ncepPendingResult = ncepLog->IsPending();

					{
						bool areEqual = ncepPendingResult == lastPendingExpected;
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED ncepPendingResult=%d expected=%d testIndex=%d in %s"),
							(int)ncepPendingResult,
							(int)lastPendingExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}
				}

				// Cleanup.
				delete pbLog;
				delete pbSuccessLog;
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBanker::LogTransfer()
		///     CPersonalBanker::GetLastLog()
		///     CPersonalBanker::GetSuccessLogCopy()
		/// </summary>
		/// <remarks>
		/// Test with canceled cash, non-restricted, and restricted transfers.
		/// Does NOT do a full equality check between the transfer instance the the log instance.
		/// Note: Code used for detection of log records is complicated by what other tests do to the logs.
		/// </remarks>
		TEST(PersonalBanker3, LogTransfer_TransferCanceledAllTest)
		{
			typedef struct
			{
				// Inputs.
				const PersonalBankerTransferType m_transferType;
				const time_t m_currentTime;
				const DWORD m_amountInDollars;
				const __int64 m_nonRestrictedAmount;
				const __int64 m_restrictedAmount;

				// Expected results.
				const int m_numberOfLogRecordsAddedExpected;
				const int m_cashSourceExpected;
				const time_t m_lastDateTimeExpected;
				const __int64 m_cashAmountExpected;
				const __int64 m_cepAmountExpected;
				const __int64 m_ncepAmountExpected;
				const int m_cepSourceExpected;
				const int m_ncepSourceExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				// Non-zero cash amounts.
				{ Undeclared, time_t(123), 1, 10, 7, 3, CashFromGame, time_t(123), 100, 10, 7, PointsFromGame, PromoDollarsFromGame },
				{ CashToGame, time_t(1232), 2, 1, 11,  1, CashToGame, time_t(1232), 200, 0, 0, PointsFromGame, PromoDollarsFromGame },
				{ PointsToGame, time_t(1233), 3, 23, 231, 1, PointsToGame, time_t(1233), 300, 0, 0, PointsFromGame, PromoDollarsFromGame },
				{ PromoDollarsToGame, time_t(1234), 4, 34, 341, 1, PromoDollarsToGame, time_t(1234), 400, 0, 0, PointsFromGame, PromoDollarsFromGame },
				{ CashFromGame, time_t(12345), 5, 45, 451, 3, CashFromGame, time_t(12345), 500, 45, 451, PointsFromGame, PromoDollarsFromGame },
				{ PointsFromGame, time_t(123456), 6, 56, 561, 3, CashFromGame, time_t(123456), 600, 56, 561, PointsFromGame, PromoDollarsFromGame },
				{ PromoDollarsFromGame, time_t(1234567), 7, 67, 671, 3, CashFromGame, time_t(1234567), 700, 67, 671, PointsFromGame, PromoDollarsFromGame },
				{ AllFromGame, time_t(12345678), 8, 78, 781, 3, CashFromGame, time_t(12345678), 800, 78, 781, PointsFromGame, PromoDollarsFromGame },
				{ (PersonalBankerTransferType)(AllFromGame+1), time_t(12345677), 77, 777, 7771, 3, CashFromGame, time_t(12345677), 7700, 777, 7771, PointsFromGame, PromoDollarsFromGame },
				{ (PersonalBankerTransferType)-1, time_t(123456789), 9, 999, 9991, 3, CashFromGame, time_t(123456789), 900, 999, 9991, PointsFromGame, PromoDollarsFromGame },

				// Zero cash amounts.
				{ Undeclared, time_t(123), 0, 12, 121, 3, CashFromGame, time_t(123), 0, 12, 121, PointsFromGame, PromoDollarsFromGame },
				{ CashToGame, time_t(1232), 0, 23, 231, 1, CashToGame, time_t(1232), 0, 0, 0, PointsFromGame, PromoDollarsFromGame },
				{ PointsToGame, time_t(1233), 0, 34, 341, 1, PointsToGame, time_t(1233), 0, 0, 0, PointsFromGame, PromoDollarsFromGame },
				{ PromoDollarsToGame, time_t(1234), 0, 45, 451, 1, PromoDollarsToGame, time_t(1234), 0, 0, 0, PointsFromGame, PromoDollarsFromGame },
				{ CashFromGame, time_t(12345), 0, 56, 561, 3, CashFromGame, time_t(12345), 0, 56, 561, PointsFromGame, PromoDollarsFromGame },
				{ PointsFromGame, time_t(123456), 0, 67, 671, 3, CashFromGame, time_t(123456), 0, 67, 671, PointsFromGame, PromoDollarsFromGame },
				{ PromoDollarsFromGame, time_t(1234567), 0, 78, 781, 3, CashFromGame, time_t(1234567), 0, 78, 781, PointsFromGame, PromoDollarsFromGame },
				{ AllFromGame, time_t(12345678), 0, 89, 891, 3, CashFromGame, time_t(12345678), 0, 89, 891, PointsFromGame, PromoDollarsFromGame },
				{ (PersonalBankerTransferType)(AllFromGame+1), time_t(12345677), 0, 901, 9011, 3, CashFromGame, time_t(12345677), 0, 901, 9011, PointsFromGame, PromoDollarsFromGame },
				{ (PersonalBankerTransferType)-1, time_t(123456789), 0, 902, 9021, 3, CashFromGame, time_t(123456789), 0, 902, 9021, PointsFromGame, PromoDollarsFromGame },
			};

			// More test data - expected results that are the same for each iteration.
			const int numberOfSuccessLogRecordsAdded(0);
			const bool lastSuccessfulExpected(false);
			const bool lastPendingExpected(true);

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// More test data - expected results that are dependent on previous tests.
				LogIndexInfoExpected logIndexInfoExpected;
				LogTransfer_GetLogIndexInfoExpected(logIndexInfoExpected,
					TEST_DATA[testIndex].m_numberOfLogRecordsAddedExpected,
					numberOfSuccessLogRecordsAdded);

				// Setup.
				CGames games;
				CPlayer player;
				const CConfig config(false, "");
				CPersonalBanker varPb;
				varPb.CreateTransfer(CUtilities::GetTickCount());

				// SetState() to affect BeginPersonalBankerTransfer().
				varPb.SetState(PBStateInfoReceived);
				byte pbEFTNumber = 1;
				varPb.BeginPersonalBankerTransfer(games,
					TEST_DATA[testIndex].m_transferType,
					TEST_DATA[testIndex].m_amountInDollars,
					pbEFTNumber);

				// SetState() to affect ProcessPersonalBankerTransferComplete().
				varPb.SetState(PBStateTransferingFromToGame);

				CPersonalBankerTransfer *pbTransfer = varPb.GetTransferCopy();
				ASSERT_NE(nullptr, pbTransfer) <<_T("FAILED pbTransfer NULL, expected NOT NULL");
				pbTransfer->SetAmount(TEST_DATA[testIndex].m_amountInDollars * ONE_DOLLAR_IN_CENTS);
				pbTransfer->SetNonrestrictedAmount(TEST_DATA[testIndex].m_nonRestrictedAmount);
				pbTransfer->SetRestrictedAmount(TEST_DATA[testIndex].m_restrictedAmount);
				byte meterNumberToAddTo(0xff);
				bool isDuplicate(false);
				const bool isEFT(false);
				varPb.ProcessPersonalBankerTransferComplete(pbTransfer,
					games,
					meterNumberToAddTo,
					isDuplicate,
					isEFT);

				varPb.SetPersonalBankerType(CanceledSessionRequest);

				// Perform operations to be tested.
				varPb.LogTransfer(&player, config, TEST_DATA[testIndex].m_currentTime);
				const CPersonalBanker &pb(varPb);

				// Test log and log indexes info.
				CLogs *pbLog = varPb.GetLogCopy();
				CLogs *pbSuccessLog = varPb.GetSuccessLogCopy();
				LogTransfer_AssertLogInfoExpected(pbLog,
					pbSuccessLog,
					logIndexInfoExpected,
					testIndex,
					__TFUNCTION__);

				// Get log records for subsequent tests.
				CPersonalBankerLog *cepLog(NULL);
				CPersonalBankerLog *ncepLog(NULL);
				CPersonalBankerLog *cashLog(NULL);
				if (1 == TEST_DATA[testIndex].m_numberOfLogRecordsAddedExpected)
				{
					cashLog = (CPersonalBankerLog *)pbLog->GetLastLog();
				}
				else if (3 == TEST_DATA[testIndex].m_numberOfLogRecordsAddedExpected)
				{
					ncepLog = (CPersonalBankerLog *)pbLog->GetLastLog();
					cepLog = (CPersonalBankerLog *)pbLog->GetPreviousLog();
					cashLog = (CPersonalBankerLog *)pbLog->GetPreviousLog();
				}
				else
				{
					ASSERT_TRUE(false) << _T("FAILED m_numberOfLogRecordsAddedExpected not in 1..3");
				}

				// Test #A.
				{
					bool areEqual = !!cepLog == !!TEST_DATA[testIndex].m_cepAmountExpected;
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED cepLog=%p m_cepAmountExpected=%lld testIndex=%d in %s"),
							(const void *)cepLog,
							(long long)TEST_DATA[testIndex].m_cepAmountExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #B.
				{
					bool areEqual = !!ncepLog == !!TEST_DATA[testIndex].m_ncepAmountExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED ncepLog=%p m_ncepAmountExpected=%lld testIndex=%d in %s"),
						(const void *)ncepLog,
						(long long)TEST_DATA[testIndex].m_ncepAmountExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #1.
				{
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED cashLog=%p expected=NOT NULL testIndex=%d in %s"),
						(const void *)cashLog,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_NE(nullptr, cashLog) << failedMsg;
				}

				// Could do a full equality check between the transfer instance the the log instance.
				// Test #2.
				const time_t cashDateTimeResult = cashLog->GetDateTime();

				{
					bool areEqual = cashDateTimeResult == TEST_DATA[testIndex].m_lastDateTimeExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED cashDateTimeResult=%lld expected=%lld testIndex=%d in %s"),
						(long long)cashDateTimeResult,
						(long long)TEST_DATA[testIndex].m_lastDateTimeExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #3.
				const __int64 cashAmountResult = cashLog->GetAmount();

				{
					bool areEqual = cashAmountResult == TEST_DATA[testIndex].m_cashAmountExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED cashAmountResult=%lld expected=%lld testIndex=%d in %s"),
						(long long)cashAmountResult,
						(long long)TEST_DATA[testIndex].m_cashAmountExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #4.
				const int cashSourceResult = cashLog->GetSource();

				{
					bool areEqual = cashSourceResult == TEST_DATA[testIndex].m_cashSourceExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED cashSourceResult=%d expected=%d testIndex=%d in %s"),
						(int)cashSourceResult,
						(int)TEST_DATA[testIndex].m_cashAmountExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #5.
				const bool cashSuccessfulResult = cashLog->IsSuccessful();

				{
					bool areEqual = cashSuccessfulResult == lastSuccessfulExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED cashSuccessfulResult=%d expected=%d testIndex=%d in %s"),
						(int)cashSuccessfulResult,
						(int)lastSuccessfulExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #6.
				const bool cashPendingResult = cashLog->IsPending();

				{
					bool areEqual = cashPendingResult == lastPendingExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED cashPendingResult=%d expected=%d testIndex=%d in %s"),
						(int)cashPendingResult,
						(int)lastPendingExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}
				
				if (TEST_DATA[testIndex].m_numberOfLogRecordsAddedExpected >= 3)
				{
					// Test #7.
					{
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED cepLog=%p expected=NOT NULL testIndex=%d in %s"),
							(const void *)cepLog,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_NE(nullptr, cepLog) << failedMsg;
					}

					// Could do a full equality check between the transfer instance the the log instance.
					// Test #8.
					const time_t cepDateTimeResult = cepLog->GetDateTime();

					{
						bool areEqual = cepDateTimeResult == TEST_DATA[testIndex].m_lastDateTimeExpected;
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED cepDateTimeResult=%lld expected=%lld testIndex=%d in %s"),
							(long long)cepDateTimeResult,
							(long long)TEST_DATA[testIndex].m_lastDateTimeExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}

					// Test #9.
					const __int64 cepAmountResult = cepLog->GetAmount();

					{
						bool areEqual = cepAmountResult == TEST_DATA[testIndex].m_cepAmountExpected;
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED cepAmountResult=%lld expected=%lld testIndex=%d in %s"),
							(long long)cepAmountResult,
							(long long)TEST_DATA[testIndex].m_cepAmountExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}

					// Test #10.
					const int cepSourceResult = cepLog->GetSource();

					{
						bool areEqual = cepSourceResult == TEST_DATA[testIndex].m_cepSourceExpected;
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED cepSourceResult=%d expected=%d testIndex=%d in %s"),
							(int)cepSourceResult,
							(int)TEST_DATA[testIndex].m_cepSourceExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}

					// Test #11.
					const bool cepSuccessfulResult = cepLog->IsSuccessful();

					{
						bool areEqual = cepSuccessfulResult == lastSuccessfulExpected;
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED cepSuccessfulResult=%d expected=%d testIndex=%d in %s"),
							(int)cepSuccessfulResult,
							(int)lastSuccessfulExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}

					// Test #12.
					const bool cepPendingResult = cepLog->IsPending();

					{
						bool areEqual = cepPendingResult == lastPendingExpected;
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED cepPendingResult=%d expected=%d testIndex=%d in %s"),
							(int)cepPendingResult,
							(int)lastPendingExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}

					// Test #13.
					{
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED cepLog=%p expected=NOT NULL testIndex=%d in %s"),
							(const void *)ncepLog,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_NE(nullptr, ncepLog) << failedMsg;
					}

					// Could do a full equality check between the transfer instance the the log instance.
					// Test #14.
					const time_t ncepDateTimeResult = ncepLog->GetDateTime();

					{
						bool areEqual = ncepDateTimeResult == TEST_DATA[testIndex].m_lastDateTimeExpected;
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED cepDateTimeResult=%lld expected=%lld testIndex=%d in %s"),
							(long long)ncepDateTimeResult,
							(long long)TEST_DATA[testIndex].m_lastDateTimeExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}

					// Test #15.
					const __int64 ncepAmountResult = ncepLog->GetAmount();

					{
						bool areEqual = ncepAmountResult == TEST_DATA[testIndex].m_ncepAmountExpected;
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED ncepAmountResult=%lld expected=%lld testIndex=%d in %s"),
							(long long)ncepAmountResult,
							(long long)TEST_DATA[testIndex].m_ncepAmountExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}

					// Test #16.
					const int ncepSourceResult = ncepLog->GetSource();

					{
						bool areEqual = ncepSourceResult == TEST_DATA[testIndex].m_ncepSourceExpected;
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED ncepSourceResult=%d expected=%d testIndex=%d in %s"),
							(int)ncepSourceResult,
							(int)TEST_DATA[testIndex].m_ncepSourceExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}

					// Test #17.
					const bool ncepSuccessfulResult = ncepLog->IsSuccessful();

					{
						bool areEqual = ncepSuccessfulResult == lastSuccessfulExpected;
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED ncepSuccessfulResult=%d expected=%d testIndex=%d in %s"),
							(int)ncepSuccessfulResult,
							(int)lastSuccessfulExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}

					// Test #18.
					const bool ncepPendingResult = ncepLog->IsPending();

					{
						bool areEqual = ncepPendingResult == lastPendingExpected;
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED ncepPendingResult=%d expected=%d testIndex=%d in %s"),
							(int)ncepPendingResult,
							(int)lastPendingExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}
				}

				// Cleanup.
				delete pbLog;
				delete pbSuccessLog;
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBanker::LogTransfer()
		///     CPersonalBanker::GetLastLog()
		///     CPersonalBanker::GetSuccessLogCopy()
		///     CPlayerInfo::GetPoints()
		/// </summary>
		/// <remarks>
		/// Test with non-canceled cash transfers.
		/// Does NOT do a full equality check between the transfer instance the the log instance.
		/// Note: Code used for detection of log records is complicated by what other tests do to the logs.
		/// </remarks>
		TEST(PersonalBanker3, LogTransfer_TransferNotCanceledCashTest)
		{
			typedef struct
			{
				// Inputs.
				const PersonalBankerTransferType m_transferType;
				const time_t m_currentTime;
				const bool m_hasPlayerInfo;
				const DWORD m_playerInfoPoints;
				const DWORD m_playerInfoCountdown;
				const DWORD m_amountInPennies;

				// Expected results.
				const int m_numberOfLogRecordsAddedExpected;
				const int m_numberOfSuccessLogRecordsAddedExpected;
				const int m_lastSourceExpected;
				const time_t m_lastDateTimeExpected;
				const __int64 m_lastAmountExpected;
				const DWORD m_playerInfoPointsExpected;
				const DWORD m_playerInfoCountdownExpected;

			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				// Non-zero cash amounts without player info.
				{ Undeclared, time_t(123), false, 5000, 100, 100, 1, 1, CashFromGame, time_t(123), 100, 5000, 100 },
				{ CashToGame, time_t(1232), false, 5001, 101, 200, 1, 1, CashToGame, time_t(1232), 200, 5001, 101 },
				{ PointsToGame, time_t(1233), false, 5002, 102, 300, 1, 1, PointsToGame, time_t(1233), 300, 5002, 102 },
				{ PromoDollarsToGame, time_t(1234), false, 5003, 103, 400, 1, 1, PromoDollarsToGame, time_t(1234), 400, 5003, 103 },
				{ CashFromGame, time_t(12345), false, 5004, 104, 500, 1, 1, CashFromGame, time_t(12345), 500, 5004, 104 },
				{ PointsFromGame, time_t(123456), false, 5005, 105, 600, 1, 1, CashFromGame, time_t(123456), 600, 5005, 105 },
				{ PromoDollarsFromGame, time_t(1234567), false, 5006, 106, 700, 1, 1, CashFromGame, time_t(1234567), 700, 5006, 106 },
				{ AllFromGame, time_t(12345678), false, 5007, 107, 800, 1, 1, CashFromGame, time_t(12345678), 800, 5007, 107 },
				{ (PersonalBankerTransferType)(ExternalCredit+1), time_t(12345677), false, 5008, 108, 7700, 1, 1, CashFromGame, time_t(12345677), 7700, 5008, 108 },
				{ (PersonalBankerTransferType)-1, time_t(123456789), false, 5009, 109, 900, 1, 1, CashFromGame, time_t(123456789), 900, 5009, 109 },

				// Zero cash amounts without player info.
				{ Undeclared, time_t(123), false, 5010, 110, 0, 0, 0, CashFromGame, time_t(123), 0, 5010, 110 },
				{ CashToGame, time_t(1232), false, 5011, 111, 0, 0, 0, CashToGame, time_t(1232), 0, 5011, 111 },
				{ PointsToGame, time_t(1233), false, 5012, 112, 0, 0, 0, PointsToGame, time_t(1233), 0, 5012, 112 },
				{ PromoDollarsToGame, time_t(1234), false, 5013, 113, 0, 0, 0, PromoDollarsToGame, time_t(1234), 0, 5013, 113 },
				{ CashFromGame, time_t(12345), false, 5014, 114, 0, 0, 0, CashFromGame, time_t(12345), 0, 5015, 114 },
				{ PointsFromGame, time_t(123456), false, 5015, 115, 0, 0, 0, CashFromGame, time_t(123456), 0, 5015, 115 },
				{ PromoDollarsFromGame, time_t(1234567), false, 5016, 116, 0, 0, 0, CashFromGame, time_t(1234567), 0, 5016, 116 },
				{ AllFromGame, time_t(12345678), false, 5017, 117, 0, 0, 0, CashFromGame, time_t(12345678), 0, 5017, 117 },
				{ (PersonalBankerTransferType)(ExternalCredit+1), time_t(12345677), false, 5018, 118, 0, 0, 0, CashFromGame, time_t(12345677), 0, 5018, 118 },
				{ (PersonalBankerTransferType)-1, time_t(123456789), false, 5019, 119, 0, 0, 0, CashFromGame, time_t(123456789), 0, 5010, 119 },

				// Non-zero cash amounts with player info.
				{ Undeclared, time_t(123), true, 5020, 120, 100, 1, 1, CashFromGame, time_t(123), 100, 5020, 120 },
				{ CashToGame, time_t(1232), true, 5021, 121, 200, 1, 1, CashToGame, time_t(1232), 200, 5021, 121 },
				{ PointsToGame, time_t(1233), true, 5022, 122, 300, 1, 1, PointsToGame, time_t(1233), 300, 4722, 122 },
				{ PromoDollarsToGame, time_t(1234), true, 5023, 123, 400, 1, 1, PromoDollarsToGame, time_t(1234), 400, 5023, 123 },
				{ CashFromGame, time_t(12345), true, 5024, 124, 500, 1, 1, CashFromGame, time_t(12345), 500, 5024, 124 },
				{ PointsFromGame, time_t(123456), true, 5025, 125, 600, 1, 1, CashFromGame, time_t(123456), 600, 5025, 125 },
				{ PromoDollarsFromGame, time_t(1234567), true, 5026, 126, 700, 1, 1, CashFromGame, time_t(1234567), 700, 5026, 126 },
				{ AllFromGame, time_t(12345678), true, 5027, 127, 800, 1, 1, CashFromGame, time_t(12345678), 800, 5027, 127 },
				{ (PersonalBankerTransferType)(ExternalCredit+1), time_t(12345677), true, 5028, 128, 7700, 1, 1, CashFromGame, time_t(12345677), 7700, 5028, 128 },
				{ (PersonalBankerTransferType)-1, time_t(123456789), true, 5029, 129, 900, 1, 1, CashFromGame, time_t(123456789), 900, 5029, 129 },

				// Zero cash amounts with player info.
				{ Undeclared, time_t(123), true, 5030, 130, 0, 0, 0, CashFromGame, time_t(123), 0, 5030, 130 },
				{ CashToGame, time_t(1232), true, 5031, 131, 0, 0, 0, CashToGame, time_t(1232), 0, 5031, 131 },
				{ PointsToGame, time_t(1233), true, 5032, 132, 0, 0, 0, PointsToGame, time_t(1233), 0, 5032, 132 },
				{ PromoDollarsToGame, time_t(1234), true, 5033, 133, 0, 0, 0, PromoDollarsToGame, time_t(1234), 0, 5033, 133 },
				{ CashFromGame, time_t(12345), true, 5034, 134, 0, 0, 0, CashFromGame, time_t(12345), 0, 5034, 134 },
				{ PointsFromGame, time_t(123456), true, 5035, 135, 0, 0, 0, CashFromGame, time_t(123456), 0, 5035, 135 },
				{ PromoDollarsFromGame, time_t(1234567), true, 5036, 136, 0, 0, 0, CashFromGame, time_t(1234567), 0, 5036, 136 },
				{ AllFromGame, time_t(12345678), true, 5037, 137, 0, 0, 0, CashFromGame, time_t(12345678), 0, 5037, 137 },
				{ (PersonalBankerTransferType)(ExternalCredit+1), time_t(12345677), true, 5038, 138, 0, 0, 0, CashFromGame, time_t(12345677), 0, 5038, 138 },
				{ (PersonalBankerTransferType)-1, time_t(123456789), true, 5039, 139, 0, 0, 0, CashFromGame, time_t(123456789), 0, 5039, 139 },

				// Non-zero cash amounts with player info and PointsToGame.
				{ PointsToGame, time_t(1233), true, 101, 40, 100, 1, 1, PointsToGame, time_t(1233), 100, 1, 40 },
				{ PointsToGame, time_t(1233), true, 100, 41, 100, 1, 1, PointsToGame, time_t(1233), 100, 0, 41 },
				{ PointsToGame, time_t(1233), true, 99, 42, 100, 1, 1, PointsToGame, time_t(1233), 100, 0, 42 },
				{ PointsToGame, time_t(1233), true, 201, 43, 200, 1, 1, PointsToGame, time_t(1233), 200, 1, 43 },
				{ PointsToGame, time_t(1233), true, 200, 44, 200, 1, 1, PointsToGame, time_t(1233), 200, 0, 44 },
				{ PointsToGame, time_t(1233), true, 199, 45, 200, 1, 1, PointsToGame, time_t(1233), 200, 0, 45 },
			};

			// More test data - expected results that are the same for each iteration.
			const long pointsPerDollar = 100;
			const bool lastSuccessfulExpected(true);
			const bool lastPendingExpected(true);
			const DWORD playerInfoPointsExpected(0);

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// More test data - expected results that are dependent on previous tests.
				LogIndexInfoExpected logIndexInfoExpected;
				LogTransfer_GetLogIndexInfoExpected(logIndexInfoExpected,
					TEST_DATA[testIndex].m_numberOfLogRecordsAddedExpected,
					TEST_DATA[testIndex].m_numberOfSuccessLogRecordsAddedExpected);

				// Setup.
				CPlayer player;
				if (TEST_DATA[testIndex].m_hasPlayerInfo)
				{
					CPlayerInfo *playerInfo = new CPlayerInfo();
					playerInfo->SetPointsAndCountdown(
						TEST_DATA[testIndex].m_playerInfoPoints,
						TEST_DATA[testIndex].m_playerInfoCountdown, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);
					player.SetPlayerInfo(playerInfo);
				}

				CConfig varConfig(false, "");
				varConfig.SetPointsPerDollar(pointsPerDollar);
				const CConfig &config(varConfig);

				CGames games;

				CPersonalBanker varPb;
				varPb.CreateTransfer(CUtilities::GetTickCount());

				// SetState() to affect BeginPersonalBankerTransfer().
				varPb.SetState(PBStateInfoReceived);
				byte pbEFTNumber = 1;
				varPb.BeginPersonalBankerTransfer(games,
					TEST_DATA[testIndex].m_transferType,
					TEST_DATA[testIndex].m_amountInPennies,
					pbEFTNumber);

				// Perform operations to be tested.
				varPb.LogTransfer(&player, config, TEST_DATA[testIndex].m_currentTime);
				const CPersonalBanker &pb(varPb);

				// Test log and log indexes info.
				CLogs *pbLog = varPb.GetLogCopy();
				CLogs *pbSuccessLog = varPb.GetSuccessLogCopy();
				LogTransfer_AssertLogInfoExpected(pbLog,
					pbSuccessLog,
					logIndexInfoExpected,
					testIndex,
					__TFUNCTION__);


				if (TEST_DATA[testIndex].m_numberOfLogRecordsAddedExpected >= 1)
				{
					// Test #1.
					CPersonalBankerLog *lastLog((CPersonalBankerLog *)pbLog->GetLastLog());

					{
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED lastLog=%p expected=NOT NULL testIndex=%d in %s"),
							(const void *)lastLog,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_NE(nullptr, lastLog) << failedMsg;
					}

					// Could do a full equality check between the transfer instance the the log instance.
					// Test #2.
					const time_t lastDateTimeResult = lastLog->GetDateTime();

					{
						bool areEqual = lastDateTimeResult == TEST_DATA[testIndex].m_lastDateTimeExpected;
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED lastDateTimeResult=%lld expected=%lld testIndex=%d in %s"),
							(long long)lastDateTimeResult,
							(long long)TEST_DATA[testIndex].m_lastDateTimeExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}

					// Test #3.
					const __int64 lastAmountResult = lastLog->GetAmount();

					{
						bool areEqual = lastAmountResult == TEST_DATA[testIndex].m_lastAmountExpected;
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED lastAmountResult=%lld expected=%lld testIndex=%d in %s"),
							(long long)lastAmountResult,
							(long long)TEST_DATA[testIndex].m_lastAmountExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}

					// Test #4.
					const int lastSourceResult = lastLog->GetSource();

					{
						bool areEqual = lastSourceResult == TEST_DATA[testIndex].m_lastSourceExpected;
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED lastSourceResult=%d expected=%d testIndex=%d in %s"),
							(int)lastSourceResult,
							(int)TEST_DATA[testIndex].m_lastSourceExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}

					// Test #5.
					const bool lastSuccessfulResult = lastLog->IsSuccessful();

					{
						bool areEqual = lastSuccessfulResult == lastSuccessfulExpected;
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED lastSuccessfulResult=%d expected=%d testIndex=%d in %s"),
							(int)lastSuccessfulResult,
							(int)lastSuccessfulExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}

					// Test #6.
					const bool lastPendingResult = lastLog->IsPending();

					{
						bool areEqual = lastPendingResult == lastPendingExpected;
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED lastPendingResult=%d expected=%d testIndex=%d in %s"),
							(int)lastPendingResult,
							(int)lastPendingExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}
				}

				if (TEST_DATA[testIndex].m_numberOfSuccessLogRecordsAddedExpected >= 1)
				{
					// Test #7.
					CPersonalBankerLog *lastSuccessLog((CPersonalBankerLog *)pbSuccessLog->GetLastLog());

					{
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED lastSuccessLog=%p expected=NOT NULL testIndex=%d in %s"),
							(const void *)lastSuccessLog,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_NE(nullptr, lastSuccessLog) << failedMsg;
					}

					// Could do a full equality check between the transfer instance the the log instance.
					// Test #8.
					const time_t lastDateTimeResult = lastSuccessLog->GetDateTime();

					{
						bool areEqual = lastDateTimeResult == TEST_DATA[testIndex].m_lastDateTimeExpected;
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED slog lastDateTimeResult=%lld expected=%lld testIndex=%d in %s"),
							(long long)lastDateTimeResult,
							(long long)TEST_DATA[testIndex].m_lastDateTimeExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}

					// Test #9.
					const __int64 lastAmountResult = lastSuccessLog->GetAmount();

					{
						bool areEqual = lastAmountResult == TEST_DATA[testIndex].m_lastAmountExpected;
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED slog lastAmountResult=%lld expected=%lld testIndex=%d in %s"),
							(long long)lastAmountResult,
							(long long)TEST_DATA[testIndex].m_lastAmountExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}

					// Test #10.
					const int lastSourceResult = lastSuccessLog->GetSource();

					{
						bool areEqual = lastSourceResult == TEST_DATA[testIndex].m_lastSourceExpected;
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED slog lastSourceResult=%d expected=%d testIndex=%d in %s"),
							(int)lastSourceResult,
							(int)TEST_DATA[testIndex].m_lastSourceExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}

					// Test #11.
					const bool lastSuccessfulResult = lastSuccessLog->IsSuccessful();

					{
						bool areEqual = lastSuccessfulResult == lastSuccessfulExpected;
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED slog lastSuccessfulResult=%d expected=%d testIndex=%d in %s"),
							(int)lastSuccessfulResult,
							(int)lastSuccessfulExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}

					// Test #12.
					const bool lastPendingResult = lastSuccessLog->IsPending();

					{
						bool areEqual = lastPendingResult == lastPendingExpected;
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED slog lastPendingResult=%d expected=%d testIndex=%d in %s"),
							(int)lastPendingResult,
							(int)lastPendingExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}
				}

				if (TEST_DATA[testIndex].m_hasPlayerInfo)
				{
					// Test #13.
					const CPlayerInfo * const playerInfo = player.GetPlayerInfo();

					{
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED playerInfo=%p expected=NOT NULL testIndex=%d in %s"),
							(const void *)playerInfo,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_NE(nullptr, playerInfo) << failedMsg;
					}

					// Test #14.
					const DWORD playerInfoPointsResult = playerInfo->GetPoints();
						
					{
						bool areEqual = playerInfoPointsResult == TEST_DATA[testIndex].m_playerInfoPointsExpected;
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED slog playerInfoPointsResult=%u expected=%u testIndex=%d in %s"),
							(unsigned)playerInfoPointsResult,
							(unsigned)TEST_DATA[testIndex].m_playerInfoPointsExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}

					// Test #15.
					const DWORD playerInfoCountdownResult = playerInfo->GetCountdown();
						
					{
						bool areEqual = playerInfoCountdownResult == TEST_DATA[testIndex].m_playerInfoCountdownExpected;
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED slog playerInfoCountdownResult=%u expected=%u testIndex=%d in %s"),
							(unsigned)playerInfoCountdownResult,
							(unsigned)TEST_DATA[testIndex].m_playerInfoCountdownExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}
				}

				// Cleanup.
				delete pbLog;
				delete pbSuccessLog;
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBanker::LogTransfer()
		///     CPersonalBanker::GetLastLog()
		///     CPersonalBanker::GetSuccessLogCopy()
		///     CPlayerInfo::GetPoints()
		/// </summary>
		/// <remarks>
		/// Test with non-canceled cash and non-restricted transfers.
		/// Does NOT do a full equality check between the transfer instance the the log instance.
		/// Note: Code used for detection of log records is complicated by what other tests do to the logs.
		/// </remarks>
		TEST(PersonalBanker3, LogTransfer_TransferNotCanceledNonRestrictedTest)
		{
			typedef struct
			{
				// Inputs.
				const PersonalBankerTransferType m_transferType;
				const time_t m_currentTime;
				const bool m_hasPlayerInfo;
				const DWORD m_playerInfoPoints;
				const DWORD m_playerInfoCountdown;
				const DWORD m_amountInDollars;
				const __int64 m_nonRestrictedAmount;

				// Expected results.
				const int m_numberOfLogRecordsAddedExpected;
				const int m_numberOfSuccessLogRecordsAddedExpected;
				const int m_cashSourceExpected;
				const time_t m_lastDateTimeExpected;
				const __int64 m_cashAmountExpected;
				const DWORD m_playerInfoPointsExpected;
				const DWORD m_playerInfoCountdownExpected;
				const __int64 m_cepAmountExpected;
				const int m_cepSourceExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				// Non-zero cash amounts without player info.
				{ Undeclared, time_t(123), false, 5000, 100, 1, 10, 2, 2, CashFromGame, time_t(123), 100, 5000, 100, 10, PointsFromGame },
				{ CashToGame, time_t(1232), false, 5001, 101, 2, 1, 1, 1, CashToGame, time_t(1232), 200, 5001, 101, 0, PointsFromGame },
				{ PointsToGame, time_t(1233), false, 5002, 102, 3, 23, 1, 1, PointsToGame, time_t(1233), 300, 5002, 102, 0, PointsFromGame },
				{ PromoDollarsToGame, time_t(1234), false, 5003, 103, 4, 34, 1, 1, PromoDollarsToGame, time_t(1234), 400, 5003, 103, 0, PointsFromGame },
				{ CashFromGame, time_t(12345), false, 5004, 104, 5, 45, 2, 2, CashFromGame, time_t(12345), 500, 5004, 104, 45, PointsFromGame },
				{ PointsFromGame, time_t(123456), false, 5005, 105, 6, 56, 2, 2, CashFromGame, time_t(123456), 600, 5005, 105, 56, PointsFromGame },
				{ PromoDollarsFromGame, time_t(1234567), false, 5006, 106, 7, 67, 2, 2, CashFromGame, time_t(1234567), 700, 5006, 106, 67, PointsFromGame },
				{ AllFromGame, time_t(12345678), false, 5007, 107, 8, 78, 2, 2, CashFromGame, time_t(12345678), 800, 5007, 107, 78, PointsFromGame },
				{ (PersonalBankerTransferType)(AllFromGame+1), time_t(12345677), false, 5008, 108, 77, 777, 2, 2, CashFromGame, time_t(12345677), 7700, 5008, 108, 777, PointsFromGame },
				{ (PersonalBankerTransferType)-1, time_t(123456789), false, 5009, 109, 9, 999, 2, 2, CashFromGame, time_t(123456789), 900, 5009, 109, 999, PointsFromGame },

				// Zero cash amounts without player info.
				{ Undeclared, time_t(123), false, 5010, 110, 0, 12, 1, 1, CashFromGame, time_t(123), 0, 5010, 110, 12, PointsFromGame },
				{ CashToGame, time_t(1232), false, 5011, 111, 0, 23, 0, 0, CashToGame, time_t(1232), 0, 5011, 111, 23, PointsFromGame },
				{ PointsToGame, time_t(1233), false, 5012, 112, 0, 34, 0, 0, PointsToGame, time_t(1233), 0, 5012, 112, 34, PointsFromGame },
				{ PromoDollarsToGame, time_t(1234), false, 5013, 113, 0, 45, 0, 0, PromoDollarsToGame, time_t(1234), 0, 5013, 113, 45, PointsFromGame },
				{ CashFromGame, time_t(12345), false, 5014, 114, 0, 56, 1, 1, CashFromGame, time_t(12345), 0, 5015, 114, 56, PointsFromGame },
				{ PointsFromGame, time_t(123456), false, 5015, 115, 0, 67, 1, 1, CashFromGame, time_t(123456), 0, 5015, 115, 67, PointsFromGame },
				{ PromoDollarsFromGame, time_t(1234567), false, 5016, 116, 0, 78, 1, 1, CashFromGame, time_t(1234567), 0, 5016, 116, 78, PointsFromGame },
				{ AllFromGame, time_t(12345678), false, 5017, 117, 0, 89, 1, 1, CashFromGame, time_t(12345678), 0, 5017, 117, 89, PointsFromGame },
				{ (PersonalBankerTransferType)(AllFromGame+1), time_t(12345677), false, 5018, 118, 0, 901, 1, 1, CashFromGame, time_t(12345677), 0, 5018, 118, 901, PointsFromGame },
				{ (PersonalBankerTransferType)-1, time_t(123456789), false, 5019, 119, 0, 902, 1, 1, CashFromGame, time_t(123456789), 0, 5010, 119, 902, PointsFromGame },

				// Non-zero cash amounts with player info.
				{ Undeclared, time_t(123), true, 5020, 120, 1, 10, 2, 2, CashFromGame, time_t(123), 100, 5020, 120, 10, PointsFromGame },
				{ CashToGame, time_t(1232), true, 5021, 121, 2, 1, 1, 1, CashToGame, time_t(1232), 200, 5021, 121, 0, PointsFromGame },
				{ PointsToGame, time_t(1233), true, 5022, 122, 3, 23, 1, 1, PointsToGame, time_t(1233), 300, 4699, 122, 0, PointsFromGame },
				{ PromoDollarsToGame, time_t(1234), true, 5023, 123, 4, 34, 1, 1, PromoDollarsToGame, time_t(1234), 400, 5023, 123, 0, PointsFromGame },
				{ CashFromGame, time_t(12345), true, 5024, 124, 5, 45, 2, 2, CashFromGame, time_t(12345), 500, 5024, 124, 45, PointsFromGame },
				{ PointsFromGame, time_t(123456), true, 5025, 125, 6, 56, 2, 2, CashFromGame, time_t(123456), 600, 5025, 125, 56, PointsFromGame },
				{ PromoDollarsFromGame, time_t(1234567), true, 5026, 126, 7, 67, 2, 2, CashFromGame, time_t(1234567), 700, 5026, 126, 67, PointsFromGame },
				{ AllFromGame, time_t(12345678), true, 5027, 127, 8, 78, 2, 2, CashFromGame, time_t(12345678), 800, 5027, 127, 78, PointsFromGame },
				{ (PersonalBankerTransferType)(AllFromGame+1), time_t(12345677), true, 5028, 128, 77, 777, 2, 2, CashFromGame, time_t(12345677), 7700, 5028, 128, 777, PointsFromGame },
				{ (PersonalBankerTransferType)-1, time_t(123456789), true, 5029, 129, 9, 999, 2, 2, CashFromGame, time_t(123456789), 900, 5029, 129, 999, PointsFromGame },

				// Zero cash amounts with player info.
				{ Undeclared, time_t(123), true, 5030, 130, 0, 12, 1, 1, CashFromGame, time_t(123), 0, 5030, 130, 12, PointsFromGame },
				{ CashToGame, time_t(1232), true, 5031, 131, 0, 23, 0, 0, CashToGame, time_t(1232), 0, 5031, 131, 23, PointsFromGame },
				{ PointsToGame, time_t(1233), true, 5032, 132, 0, 34, 0, 0, PointsToGame, time_t(1233), 0, 5032, 132, 34, PointsFromGame },
				{ PromoDollarsToGame, time_t(1234), true, 5033, 133, 0, 45, 0, 0, PromoDollarsToGame, time_t(1234), 0, 5033, 133, 45, PointsFromGame },
				{ CashFromGame, time_t(12345), true, 5034, 134, 0, 56, 1, 1, CashFromGame, time_t(12345), 0, 5034, 134, 56, PointsFromGame },
				{ PointsFromGame, time_t(123456), true, 5035, 135, 0, 67, 1, 1, CashFromGame, time_t(123456), 0, 5035, 135, 67, PointsFromGame },
				{ PromoDollarsFromGame, time_t(1234567), true, 5036, 136, 0, 78, 1, 1, CashFromGame, time_t(1234567), 0, 5036, 136, 78, PointsFromGame },
				{ AllFromGame, time_t(12345678), true, 5037, 137, 0, 89, 1, 1, CashFromGame, time_t(12345678), 0, 5037, 137, 89, PointsFromGame },
				{ (PersonalBankerTransferType)(AllFromGame+1), time_t(12345677), true, 5038, 138, 0, 901, 1, 1, CashFromGame, time_t(12345677), 0, 5038, 138, 901, PointsFromGame },
				{ (PersonalBankerTransferType)-1, time_t(123456789), true, 5039, 139, 0, 902, 1, 1, CashFromGame, time_t(123456789), 0, 5039, 139, 902, PointsFromGame },

				// Non-zero cash amounts with player info and PointsToGame.
				{ PointsToGame, time_t(1233), true, 113, 40, 1, 12, 1, 1, PointsToGame, time_t(1233), 100, 1, 40, 0, PointsFromGame },
				{ PointsToGame, time_t(1233), true, 112, 41, 1, 12, 1, 1, PointsToGame, time_t(1233), 100, 0, 41, 0, PointsFromGame },
				{ PointsToGame, time_t(1233), true, 111, 42, 1, 12, 1, 1, PointsToGame, time_t(1233), 100, 0, 42, 0, PointsFromGame },
				{ PointsToGame, time_t(1233), true, 235, 43, 2, 34, 1, 1, PointsToGame, time_t(1233), 200, 1, 43, 0, PointsFromGame },
				{ PointsToGame, time_t(1233), true, 234, 44, 2, 34, 1, 1, PointsToGame, time_t(1233), 200, 0, 44, 0, PointsFromGame },
				{ PointsToGame, time_t(1233), true, 233, 45, 2, 34, 1, 1, PointsToGame, time_t(1233), 200, 0, 45, 0, PointsFromGame },

#if 000//TODO-PORT
				// Use rollover to test amountTransfered=0 and pointsTransfered=0 in CPersonalBanker::LogTransfer()
				{ PointsToGame, time_t(1233), true, 233, 46, 0xffffffff, (__int64)0xffffffffffffffff - __int64((DWORD)(0xffffffff * ONE_DOLLAR_IN_CENTS) +1),
				1, 1, PointsToGame, time_t(1233), (DWORD)(0xffffffff * ONE_DOLLAR_IN_CENTS), 233, 46, 0, PointsFromGame },
#endif
			};

			// More test data - expected results that are the same for each iteration.
			const long pointsPerDollar = 100;
			const bool lastSuccessfulExpected(true);
			const bool lastPendingExpected(true);
			const DWORD playerInfoPointsExpected(0);

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// More test data - expected results that are dependent on previous tests.
				LogIndexInfoExpected logIndexInfoExpected;
				LogTransfer_GetLogIndexInfoExpected(logIndexInfoExpected,
					TEST_DATA[testIndex].m_numberOfLogRecordsAddedExpected,
					TEST_DATA[testIndex].m_numberOfSuccessLogRecordsAddedExpected);

				// Setup.
				CPlayer player;
				if (TEST_DATA[testIndex].m_hasPlayerInfo)
				{
					CPlayerInfo *playerInfo = new CPlayerInfo();
					playerInfo->SetPointsAndCountdown(
						TEST_DATA[testIndex].m_playerInfoPoints,
						TEST_DATA[testIndex].m_playerInfoCountdown, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);
					player.SetPlayerInfo(playerInfo);
				}

				CConfig varConfig(false, "");
				varConfig.SetPointsPerDollar(pointsPerDollar);
				const CConfig &config(varConfig);

				CGames games;

				CPersonalBanker varPb;
				varPb.CreateTransfer(CUtilities::GetTickCount());

				// SetState() to affect BeginPersonalBankerTransfer().
				varPb.SetState(PBStateInfoReceived);
				byte pbEFTNumber = 1;
				varPb.BeginPersonalBankerTransfer(games,
					TEST_DATA[testIndex].m_transferType,
					TEST_DATA[testIndex].m_amountInDollars,
					pbEFTNumber);

				// SetState() to affect ProcessPersonalBankerTransferComplete().
				varPb.SetState(PBStateTransferingFromToGame);

				CPersonalBankerTransfer *pbTransfer = varPb.GetTransferCopy();
				ASSERT_NE(nullptr, pbTransfer) << _T("FAILED pbTransfer NULL, expected NOT NULL");
				pbTransfer->SetAmount(TEST_DATA[testIndex].m_amountInDollars * ONE_DOLLAR_IN_CENTS);
				pbTransfer->SetNonrestrictedAmount(TEST_DATA[testIndex].m_nonRestrictedAmount);
				pbTransfer->SetRestrictedAmount(0);
				byte meterNumberToAddTo(0xff);
				bool isDuplicate(false);
				const bool isEFT(false);
				varPb.ProcessPersonalBankerTransferComplete(pbTransfer,
					games,
					meterNumberToAddTo,
					isDuplicate,
					isEFT);

				// Perform operations to be tested.
				varPb.LogTransfer(&player, config, TEST_DATA[testIndex].m_currentTime);
				const CPersonalBanker &pb(varPb);

				// Test log and log indexes info.
				CLogs *pbLog = varPb.GetLogCopy();
				CLogs *pbSuccessLog = varPb.GetSuccessLogCopy();
				LogTransfer_AssertLogInfoExpected(pbLog,
					pbSuccessLog,
					logIndexInfoExpected,
					testIndex,
					__TFUNCTION__);


				if (TEST_DATA[testIndex].m_numberOfLogRecordsAddedExpected >= 1)
				{
					// Get log records for subsequent tests.
					CPersonalBankerLog *cepLog(NULL);
					CPersonalBankerLog *cashLog(NULL);
					if (1 == TEST_DATA[testIndex].m_numberOfLogRecordsAddedExpected)
					{
						if (TEST_DATA[testIndex].m_cashAmountExpected > 0)
						{
							cashLog = (CPersonalBankerLog *)pbLog->GetLastLog();
						}
						else
						{
							cepLog = (CPersonalBankerLog *)pbLog->GetLastLog();
						}
					}
					else if (2 == TEST_DATA[testIndex].m_numberOfLogRecordsAddedExpected)
					{
						cepLog = (CPersonalBankerLog *)pbLog->GetLastLog();
						cashLog = (CPersonalBankerLog *)pbLog->GetPreviousLog();
					}
					else
					{
						ASSERT_TRUE(false) << _T("FAILED m_numberOfLogRecordsAddedExpected not in 1..2");
					}

					// Test #A.
					{
						bool areEqual = !!cepLog == !!TEST_DATA[testIndex].m_cepAmountExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED cepLog=%p m_cepAmountExpected=%lld testIndex=%d in %s"),
								(const void *)cepLog,
								(long long)TEST_DATA[testIndex].m_cepAmountExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}

					// Test #B.
					{
						bool areEqual = !!cashLog == !!TEST_DATA[testIndex].m_cashAmountExpected;
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED cashLog=%p m_cashAmountExpected=%lld testIndex=%d in %s"),
							(const void *)cashLog,
							(long long)TEST_DATA[testIndex].m_cashAmountExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}

					if (TEST_DATA[testIndex].m_cashAmountExpected > 0)
					{
						// Test #1.

						{
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED lastLog=%p expected=NOT NULL testIndex=%d in %s"),
								(const void *)cashLog,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_NE(nullptr, cashLog) << failedMsg;
						}

						// Could do a full equality check between the transfer instance the the log instance.
						// Test #2.
						const time_t lastDateTimeResult = cashLog->GetDateTime();

						{
							bool areEqual = lastDateTimeResult == TEST_DATA[testIndex].m_lastDateTimeExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED lastDateTimeResult=%lld expected=%lld testIndex=%d in %s"),
								(long long)lastDateTimeResult,
								(long long)TEST_DATA[testIndex].m_lastDateTimeExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #3.
						const __int64 cashAmountResult = cashLog->GetAmount();

						{
							bool areEqual = cashAmountResult == TEST_DATA[testIndex].m_cashAmountExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED cashAmountResult=%lld expected=%lld testIndex=%d in %s"),
								(long long)cashAmountResult,
								(long long)TEST_DATA[testIndex].m_cashAmountExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #4.
						const int cashSourceResult = cashLog->GetSource();

						{
							bool areEqual = cashSourceResult == TEST_DATA[testIndex].m_cashSourceExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED cashSourceResult=%d expected=%d testIndex=%d in %s"),
								(int)cashSourceResult,
								(int)TEST_DATA[testIndex].m_cashSourceExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #5.
						const bool cashSuccessfulResult = cashLog->IsSuccessful();

						{
							bool areEqual = cashSuccessfulResult == lastSuccessfulExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED cashSuccessfulResult=%d expected=%d testIndex=%d in %s"),
								(int)cashSuccessfulResult,
								(int)lastSuccessfulExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #6.
						const bool cashPendingResult = cashLog->IsPending();

						{
							bool areEqual = cashPendingResult == lastPendingExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED cashPendingResult=%d expected=%d testIndex=%d in %s"),
								(int)cashPendingResult,
								(int)lastPendingExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}
					}

					if (TEST_DATA[testIndex].m_cepAmountExpected > 0)
					{
						// Test #7.
						{
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED cepLog=%p expected=NOT NULL testIndex=%d in %s"),
								(const void *)cepLog,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_NE(nullptr, cepLog) << failedMsg;
						}

						// Could do a full equality check between the transfer instance the the log instance.
						// Test #8.
						const time_t cepDateTimeResult = cepLog->GetDateTime();

						{
							bool areEqual = cepDateTimeResult == TEST_DATA[testIndex].m_lastDateTimeExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED cepDateTimeResult=%lld expected=%lld testIndex=%d in %s"),
								(long long)cepDateTimeResult,
								(long long)TEST_DATA[testIndex].m_lastDateTimeExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #9.
						const __int64 cepAmountResult = cepLog->GetAmount();

						{
							bool areEqual = cepAmountResult == TEST_DATA[testIndex].m_cepAmountExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED cepAmountResult=%lld expected=%lld testIndex=%d in %s"),
								(long long)cepAmountResult,
								(long long)TEST_DATA[testIndex].m_cepAmountExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #10.
						const int cepSourceResult = cepLog->GetSource();

						{
							bool areEqual = cepSourceResult == TEST_DATA[testIndex].m_cepSourceExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED cepSourceResult=%d expected=%d testIndex=%d in %s"),
								(int)cepSourceResult,
								(int)TEST_DATA[testIndex].m_cepSourceExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #11.
						const bool cepSuccessfulResult = cepLog->IsSuccessful();

						{
							bool areEqual = cepSuccessfulResult == lastSuccessfulExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED cepSuccessfulResult=%d expected=%d testIndex=%d in %s"),
								(int)cepSuccessfulResult,
								(int)lastSuccessfulExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #12.
						const bool cepPendingResult = cepLog->IsPending();

						{
							bool areEqual = cepPendingResult == lastPendingExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED cepPendingResult=%d expected=%d testIndex=%d in %s"),
								(int)cepPendingResult,
								(int)lastPendingExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}
					}
				}

				if (TEST_DATA[testIndex].m_numberOfSuccessLogRecordsAddedExpected >= 1)
				{
					// Get log records for subsequent tests.
					CPersonalBankerLog *cepSuccessLog(NULL);
					CPersonalBankerLog *cashSuccessLog(NULL);
					if (1 == TEST_DATA[testIndex].m_numberOfLogRecordsAddedExpected)
					{
						if (TEST_DATA[testIndex].m_cashAmountExpected > 0)
						{
							cashSuccessLog = (CPersonalBankerLog *)pbSuccessLog->GetLastLog();
						}
						else
						{
							cepSuccessLog = (CPersonalBankerLog *)pbSuccessLog->GetLastLog();
						}
					}
					else if (2 == TEST_DATA[testIndex].m_numberOfLogRecordsAddedExpected)
					{
						cepSuccessLog = (CPersonalBankerLog *)pbSuccessLog->GetLastLog();
						cashSuccessLog = (CPersonalBankerLog *)pbSuccessLog->GetPreviousLog();
					}
					else
					{
						ASSERT_TRUE(false) << _T("FAILED m_numberOfLogRecordsAddedExpected not in 1..2");
					}

					// Test #C.
					{
						bool areEqual = !!cepSuccessLog == !!TEST_DATA[testIndex].m_cepAmountExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED cepSuccessLog=%p m_cepAmountExpected=%lld testIndex=%d in %s"),
								(const void *)cepSuccessLog,
								(long long)TEST_DATA[testIndex].m_cepAmountExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}

					// Test #D.
					{
						bool areEqual = !!cashSuccessLog == !!TEST_DATA[testIndex].m_cashAmountExpected;
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED cashSuccessLog=%p m_cashAmountExpected=%lld testIndex=%d in %s"),
							(const void *)cashSuccessLog,
							(long long)TEST_DATA[testIndex].m_cashAmountExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}

					if (TEST_DATA[testIndex].m_cashAmountExpected > 0)
					{
						// Test #13.

						{
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED cashSuccessLog=%p expected=NOT NULL testIndex=%d in %s"),
								(const void *)cashSuccessLog,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_NE(nullptr, cashSuccessLog) << failedMsg;
						}

						// Could do a full equality check between the transfer instance the the log instance.
						// Test #14.
						const time_t cashDateTimeResult = cashSuccessLog->GetDateTime();

						{
							bool areEqual = cashDateTimeResult == TEST_DATA[testIndex].m_lastDateTimeExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED slog cashDateTimeResult=%lld expected=%lld testIndex=%d in %s"),
								(long long)cashDateTimeResult,
								(long long)TEST_DATA[testIndex].m_lastDateTimeExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #15.
						const __int64 cashAmountResult = cashSuccessLog->GetAmount();

						{
							bool areEqual = cashAmountResult == TEST_DATA[testIndex].m_cashAmountExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED slog cashAmountResult=%lld expected=%lld testIndex=%d in %s"),
								(long long)cashAmountResult,
								(long long)TEST_DATA[testIndex].m_cashAmountExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #16.
						const int cashSourceResult = cashSuccessLog->GetSource();

						{
							bool areEqual = cashSourceResult == TEST_DATA[testIndex].m_cashSourceExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED slog cashSourceResult=%d expected=%d testIndex=%d in %s"),
								(int)cashSourceResult,
								(int)TEST_DATA[testIndex].m_cashSourceExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #17.
						const bool cashSuccessfulResult = cashSuccessLog->IsSuccessful();

						{
							bool areEqual = cashSuccessfulResult == lastSuccessfulExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED slog cashSuccessfulResult=%d expected=%d testIndex=%d in %s"),
								(int)cashSuccessfulResult,
								(int)lastSuccessfulExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #18.
						const bool cashPendingResult = cashSuccessLog->IsPending();

						{
							bool areEqual = cashPendingResult == lastPendingExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED slog cashPendingResult=%d expected=%d testIndex=%d in %s"),
								(int)cashPendingResult,
								(int)lastPendingExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}
					}

					if (TEST_DATA[testIndex].m_cepAmountExpected > 0)
					{
						// Test #19.
						{
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED cepSuccessLog=%p expected=NOT NULL testIndex=%d in %s"),
								(const void *)cepSuccessLog,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_NE(nullptr, cepSuccessLog) << failedMsg;
						}

						// Could do a full equality check between the transfer instance the the log instance.
						// Test #20.
						const time_t cepDateTimeResult = cepSuccessLog->GetDateTime();

						{
							bool areEqual = cepDateTimeResult == TEST_DATA[testIndex].m_lastDateTimeExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED cepDateTimeResult=%lld expected=%lld testIndex=%d in %s"),
								(long long)cepDateTimeResult,
								(long long)TEST_DATA[testIndex].m_lastDateTimeExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #21.
						const __int64 cepAmountResult = cepSuccessLog->GetAmount();

						{
							bool areEqual = cepAmountResult == TEST_DATA[testIndex].m_cepAmountExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED cepAmountResult=%lld expected=%lld testIndex=%d in %s"),
								(long long)cepAmountResult,
								(long long)TEST_DATA[testIndex].m_cepAmountExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #22.
						const int cepSourceResult = cepSuccessLog->GetSource();

						{
							bool areEqual = cepSourceResult == TEST_DATA[testIndex].m_cepSourceExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED cepSourceResult=%d expected=%d testIndex=%d in %s"),
								(int)cepSourceResult,
								(int)TEST_DATA[testIndex].m_cepSourceExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #23.
						const bool cepSuccessfulResult = cepSuccessLog->IsSuccessful();

						{
							bool areEqual = cepSuccessfulResult == lastSuccessfulExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED cepSuccessfulResult=%d expected=%d testIndex=%d in %s"),
								(int)cepSuccessfulResult,
								(int)lastSuccessfulExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #24.
						const bool cepPendingResult = cepSuccessLog->IsPending();

						{
							bool areEqual = cepPendingResult == lastPendingExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED cepPendingResult=%d expected=%d testIndex=%d in %s"),
								(int)cepPendingResult,
								(int)lastPendingExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}
					}
				}
				if (TEST_DATA[testIndex].m_hasPlayerInfo)
				{
					// Test #25.
					const CPlayerInfo * const playerInfo = player.GetPlayerInfo();

					{
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED playerInfo=%p expected=NOT NULL testIndex=%d in %s"),
							(const void *)playerInfo,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_NE(nullptr, playerInfo) << failedMsg;
					}

					// Test #26.
					const DWORD playerInfoPointsResult = playerInfo->GetPoints();

					{
						bool areEqual = playerInfoPointsResult == TEST_DATA[testIndex].m_playerInfoPointsExpected;
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED slog playerInfoPointsResult=%u expected=%u testIndex=%d in %s"),
							(unsigned)playerInfoPointsResult,
							(unsigned)TEST_DATA[testIndex].m_playerInfoPointsExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}

					// Test #27.
					const DWORD playerInfoCountdownResult = playerInfo->GetCountdown();

					{
						bool areEqual = playerInfoCountdownResult == TEST_DATA[testIndex].m_playerInfoCountdownExpected;
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED slog playerInfoCountdownResult=%u expected=%u testIndex=%d in %s"),
							(unsigned)playerInfoCountdownResult,
							(unsigned)TEST_DATA[testIndex].m_playerInfoCountdownExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}
				}

				// Cleanup.
				delete pbLog;
				delete pbSuccessLog;
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBanker::LogTransfer()
		///     CPersonalBanker::GetLastLog()
		///     CPersonalBanker::GetSuccessLogCopy()
		///     CPlayerInfo::GetPoints()
		/// </summary>
		/// <remarks>
		/// Test with non-canceled cash and restricted transfers.
		/// Does NOT do a full equality check between the transfer instance the the log instance.
		/// Note: Code used for detection of log records is complicated by what other tests do to the logs.
		/// </remarks>
		TEST(PersonalBanker3, LogTransfer_TransferNotCanceledRestrictedTest)
		{
			typedef struct
			{
				// Inputs.
				const PersonalBankerTransferType m_transferType;
				const time_t m_currentTime;
				const bool m_hasPlayerInfo;
				const DWORD m_playerInfoPoints;
				const DWORD m_playerInfoCountdown;
				const DWORD m_amountInDollars;
				const __int64 m_restrictedAmount;

				// Expected results.
				const int m_numberOfLogRecordsAddedExpected;
				const int m_numberOfSuccessLogRecordsAddedExpected;
				const int m_cashSourceExpected;
				const time_t m_lastDateTimeExpected;
				const __int64 m_cashAmountExpected;
				const DWORD m_playerInfoPointsExpected;
				const DWORD m_playerInfoCountdownExpected;
				const __int64 m_ncepAmountExpected;
				const int m_ncepSourceExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				// Non-zero cash amounts without player info.
				{ Undeclared, time_t(123), false, 5000, 100, 1, 10, 2, 2, CashFromGame, time_t(123), 100, 5000, 100, 10, PromoDollarsFromGame },
				{ CashToGame, time_t(1232), false, 5001, 101, 2, 1, 1, 1, CashToGame, time_t(1232), 200, 5001, 101, 0, PromoDollarsFromGame },
				{ PointsToGame, time_t(1233), false, 5002, 102, 3, 23, 1, 1, PointsToGame, time_t(1233), 300, 5002, 102, 0, PromoDollarsFromGame },
				{ PromoDollarsToGame, time_t(1234), false, 5003, 103, 4, 34, 1, 1, PromoDollarsToGame, time_t(1234), 400, 5003, 103, 0, PromoDollarsFromGame },
				{ CashFromGame, time_t(12345), false, 5004, 104, 5, 45, 2, 2, CashFromGame, time_t(12345), 500, 5004, 104, 45, PromoDollarsFromGame },
				{ PointsFromGame, time_t(123456), false, 5005, 105, 6, 56, 2, 2, CashFromGame, time_t(123456), 600, 5005, 105, 56, PromoDollarsFromGame },
				{ PromoDollarsFromGame, time_t(1234567), false, 5006, 106, 7, 67, 2, 2, CashFromGame, time_t(1234567), 700, 5006, 106, 67, PromoDollarsFromGame },
				{ AllFromGame, time_t(12345678), false, 5007, 107, 8, 78, 2, 2, CashFromGame, time_t(12345678), 800, 5007, 107, 78, PromoDollarsFromGame },
				{ (PersonalBankerTransferType)(AllFromGame+1), time_t(12345677), false, 5008, 108, 77, 777, 2, 2, CashFromGame, time_t(12345677), 7700, 5008, 108, 777, PromoDollarsFromGame },
				{ (PersonalBankerTransferType)-1, time_t(123456789), false, 5009, 109, 9, 999, 2, 2, CashFromGame, time_t(123456789), 900, 5009, 109, 999, PromoDollarsFromGame },

				// Zero cash amounts without player info.
				{ Undeclared, time_t(123), false, 5010, 110, 0, 12, 1, 1, CashFromGame, time_t(123), 0, 5010, 110, 12, PromoDollarsFromGame },
				{ CashToGame, time_t(1232), false, 5011, 111, 0, 23, 0, 0, CashToGame, time_t(1232), 0, 5011, 111, 23, PromoDollarsFromGame },
				{ PointsToGame, time_t(1233), false, 5012, 112, 0, 34, 0, 0, PointsToGame, time_t(1233), 0, 5012, 112, 34, PromoDollarsFromGame },
				{ PromoDollarsToGame, time_t(1234), false, 5013, 113, 0, 45, 0, 0, PromoDollarsToGame, time_t(1234), 0, 5013, 113, 45, PromoDollarsFromGame },
				{ CashFromGame, time_t(12345), false, 5014, 114, 0, 56, 1, 1, CashFromGame, time_t(12345), 0, 5015, 114, 56, PromoDollarsFromGame },
				{ PointsFromGame, time_t(123456), false, 5015, 115, 0, 67, 1, 1, CashFromGame, time_t(123456), 0, 5015, 115, 67, PromoDollarsFromGame },
				{ PromoDollarsFromGame, time_t(1234567), false, 5016, 116, 0, 78, 1, 1, CashFromGame, time_t(1234567), 0, 5016, 116, 78, PromoDollarsFromGame },
				{ AllFromGame, time_t(12345678), false, 5017, 117, 0, 89, 1, 1, CashFromGame, time_t(12345678), 0, 5017, 117, 89, PromoDollarsFromGame },
				{ (PersonalBankerTransferType)(AllFromGame+1), time_t(12345677), false, 5018, 118, 0, 901, 1, 1, CashFromGame, time_t(12345677), 0, 5018, 118, 901, PromoDollarsFromGame },
				{ (PersonalBankerTransferType)-1, time_t(123456789), false, 5019, 119, 0, 902, 1, 1, CashFromGame, time_t(123456789), 0, 5010, 119, 902, PromoDollarsFromGame },

				// Non-zero cash amounts with player info.
				{ Undeclared, time_t(123), true, 5020, 120, 1, 10, 2, 2, CashFromGame, time_t(123), 100, 5020, 120, 10, PromoDollarsFromGame },
				{ CashToGame, time_t(1232), true, 5021, 121, 2, 1, 1, 1, CashToGame, time_t(1232), 200, 5021, 121, 0, PromoDollarsFromGame },
				{ PointsToGame, time_t(1233), true, 5022, 122, 3, 23, 1, 1, PointsToGame, time_t(1233), 300, 4699, 122, 0, PromoDollarsFromGame },
				{ PromoDollarsToGame, time_t(1234), true, 5023, 123, 4, 34, 1, 1, PromoDollarsToGame, time_t(1234), 400, 5023, 123, 0, PromoDollarsFromGame },
				{ CashFromGame, time_t(12345), true, 5024, 124, 5, 45, 2, 2, CashFromGame, time_t(12345), 500, 5024, 124, 45, PromoDollarsFromGame },
				{ PointsFromGame, time_t(123456), true, 5025, 125, 6, 56, 2, 2, CashFromGame, time_t(123456), 600, 5025, 125, 56, PromoDollarsFromGame },
				{ PromoDollarsFromGame, time_t(1234567), true, 5026, 126, 7, 67, 2, 2, CashFromGame, time_t(1234567), 700, 5026, 126, 67, PromoDollarsFromGame },
				{ AllFromGame, time_t(12345678), true, 5027, 127, 8, 78, 2, 2, CashFromGame, time_t(12345678), 800, 5027, 127, 78, PromoDollarsFromGame },
				{ (PersonalBankerTransferType)(AllFromGame+1), time_t(12345677), true, 5028, 128, 77, 777, 2, 2, CashFromGame, time_t(12345677), 7700, 5028, 128, 777, PromoDollarsFromGame },
				{ (PersonalBankerTransferType)-1, time_t(123456789), true, 5029, 129, 9, 999, 2, 2, CashFromGame, time_t(123456789), 900, 5029, 129, 999, PromoDollarsFromGame },

				// Zero cash amounts with player info.
				{ Undeclared, time_t(123), true, 5030, 130, 0, 12, 1, 1, CashFromGame, time_t(123), 0, 5030, 130, 12, PromoDollarsFromGame },
				{ CashToGame, time_t(1232), true, 5031, 131, 0, 23, 0, 0, CashToGame, time_t(1232), 0, 5031, 131, 23, PromoDollarsFromGame },
				{ PointsToGame, time_t(1233), true, 5032, 132, 0, 34, 0, 0, PointsToGame, time_t(1233), 0, 5032, 132, 34, PromoDollarsFromGame },
				{ PromoDollarsToGame, time_t(1234), true, 5033, 133, 0, 45, 0, 0, PromoDollarsToGame, time_t(1234), 0, 5033, 133, 45, PromoDollarsFromGame },
				{ CashFromGame, time_t(12345), true, 5034, 134, 0, 56, 1, 1, CashFromGame, time_t(12345), 0, 5034, 134, 56, PromoDollarsFromGame },
				{ PointsFromGame, time_t(123456), true, 5035, 135, 0, 67, 1, 1, CashFromGame, time_t(123456), 0, 5035, 135, 67, PromoDollarsFromGame },
				{ PromoDollarsFromGame, time_t(1234567), true, 5036, 136, 0, 78, 1, 1, CashFromGame, time_t(1234567), 0, 5036, 136, 78, PromoDollarsFromGame },
				{ AllFromGame, time_t(12345678), true, 5037, 137, 0, 89, 1, 1, CashFromGame, time_t(12345678), 0, 5037, 137, 89, PromoDollarsFromGame },
				{ (PersonalBankerTransferType)(AllFromGame+1), time_t(12345677), true, 5038, 138, 0, 901, 1, 1, CashFromGame, time_t(12345677), 0, 5038, 138, 901, PromoDollarsFromGame },
				{ (PersonalBankerTransferType)-1, time_t(123456789), true, 5039, 139, 0, 902, 1, 1, CashFromGame, time_t(123456789), 0, 5039, 139, 902, PromoDollarsFromGame },

				// Non-zero cash amounts with player info and PointsToGame.
				{ PointsToGame, time_t(1233), true, 113, 40, 1, 12, 1, 1, PointsToGame, time_t(1233), 100, 1, 40, 0, PromoDollarsFromGame },
				{ PointsToGame, time_t(1233), true, 112, 41, 1, 12, 1, 1, PointsToGame, time_t(1233), 100, 0, 41, 0, PromoDollarsFromGame },
				{ PointsToGame, time_t(1233), true, 111, 42, 1, 12, 1, 1, PointsToGame, time_t(1233), 100, 0, 42, 0, PromoDollarsFromGame },
				{ PointsToGame, time_t(1233), true, 235, 43, 2, 34, 1, 1, PointsToGame, time_t(1233), 200, 1, 43, 0, PromoDollarsFromGame },
				{ PointsToGame, time_t(1233), true, 234, 44, 2, 34, 1, 1, PointsToGame, time_t(1233), 200, 0, 44, 0, PromoDollarsFromGame },
				{ PointsToGame, time_t(1233), true, 233, 45, 2, 34, 1, 1, PointsToGame, time_t(1233), 200, 0, 45, 0, PromoDollarsFromGame },
#if 000//TODO-PORT
				// Use rollover to test amountTransfered=0 and pointsTransfered=0 in CPersonalBanker::LogTransfer()
				{ PointsToGame, time_t(1233), true, 233, 46, 0xffffffff, (__int64)0xffffffffffffffff - __int64((DWORD)(0xffffffff * ONE_DOLLAR_IN_CENTS) +1),
					1, 1, PointsToGame, time_t(1233), (DWORD)(0xffffffff * ONE_DOLLAR_IN_CENTS), 233, 46, 0, PromoDollarsFromGame },
#endif
			};

			// More test data - expected results that are the same for each iteration.
			const long pointsPerDollar = 100;
			const __int64 nonRestrictedAmount = 0;
			const bool lastSuccessfulExpected(true);
			const bool lastPendingExpected(true);
			const DWORD playerInfoPointsExpected(0);

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// More test data - expected results that are dependent on previous tests.
				LogIndexInfoExpected logIndexInfoExpected;
				LogTransfer_GetLogIndexInfoExpected(logIndexInfoExpected,
					TEST_DATA[testIndex].m_numberOfLogRecordsAddedExpected,
					TEST_DATA[testIndex].m_numberOfSuccessLogRecordsAddedExpected);

				// Setup.
				CPlayer player;
				if (TEST_DATA[testIndex].m_hasPlayerInfo)
				{
					CPlayerInfo *playerInfo = new CPlayerInfo();
					playerInfo->SetPointsAndCountdown(
						TEST_DATA[testIndex].m_playerInfoPoints,
						TEST_DATA[testIndex].m_playerInfoCountdown, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);
					player.SetPlayerInfo(playerInfo);
				}

				CConfig varConfig(false, "");
				varConfig.SetPointsPerDollar(pointsPerDollar);
				const CConfig &config(varConfig);

				CGames games;

				CPersonalBanker varPb;
				varPb.CreateTransfer(CUtilities::GetTickCount());

				// SetState() to affect BeginPersonalBankerTransfer().
				varPb.SetState(PBStateInfoReceived);
				byte pbEFTNumber = 1;
				varPb.BeginPersonalBankerTransfer(games,
					TEST_DATA[testIndex].m_transferType,
					TEST_DATA[testIndex].m_amountInDollars,
					pbEFTNumber);

				// SetState() to affect ProcessPersonalBankerTransferComplete().
				varPb.SetState(PBStateTransferingFromToGame);

				CPersonalBankerTransfer *pbTransfer = varPb.GetTransferCopy();
				ASSERT_NE(nullptr, pbTransfer) << _T("FAILED pbTransfer NULL, expected NOT NULL");
				pbTransfer->SetAmount(TEST_DATA[testIndex].m_amountInDollars * ONE_DOLLAR_IN_CENTS);
				pbTransfer->SetNonrestrictedAmount(nonRestrictedAmount);
				pbTransfer->SetRestrictedAmount(TEST_DATA[testIndex].m_restrictedAmount);
				byte meterNumberToAddTo(0xff);
				bool isDuplicate(false);
				const bool isEFT(false);
				varPb.ProcessPersonalBankerTransferComplete(pbTransfer,
					games,
					meterNumberToAddTo,
					isDuplicate,
					isEFT);

				// Perform operations to be tested.
				varPb.LogTransfer(&player, config, TEST_DATA[testIndex].m_currentTime);
				const CPersonalBanker &pb(varPb);

				// Test log and log indexes info.
				CLogs *pbLog = varPb.GetLogCopy();
				CLogs *pbSuccessLog = varPb.GetSuccessLogCopy();
				LogTransfer_AssertLogInfoExpected(pbLog,
					pbSuccessLog,
					logIndexInfoExpected,
					testIndex,
					__TFUNCTION__);


				if (TEST_DATA[testIndex].m_numberOfLogRecordsAddedExpected >= 1)
				{
					// Get log records for subsequent tests.
					CPersonalBankerLog *ncepLog(NULL);
					CPersonalBankerLog *cashLog(NULL);
					if (1 == TEST_DATA[testIndex].m_numberOfLogRecordsAddedExpected)
					{
						if (TEST_DATA[testIndex].m_cashAmountExpected > 0)
						{
							cashLog = (CPersonalBankerLog *)pbLog->GetLastLog();
						}
						else
						{
							ncepLog = (CPersonalBankerLog *)pbLog->GetLastLog();
						}
					}
					else if (2 == TEST_DATA[testIndex].m_numberOfLogRecordsAddedExpected)
					{
						ncepLog = (CPersonalBankerLog *)pbLog->GetLastLog();
						cashLog = (CPersonalBankerLog *)pbLog->GetPreviousLog();
					}
					else
					{
						ASSERT_TRUE(false) << _T("FAILED m_numberOfLogRecordsAddedExpected not in 1..2");
					}

					// Test #A.
					{
						bool areEqual = !!ncepLog == !!TEST_DATA[testIndex].m_ncepAmountExpected;
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED ncepLog=%p m_ncepAmountExpected=%lld testIndex=%d in %s"),
							(const void *)ncepLog,
							(long long)TEST_DATA[testIndex].m_ncepAmountExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}

					// Test #B.
					{
						bool areEqual = !!cashLog == !!TEST_DATA[testIndex].m_cashAmountExpected;
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED cashLog=%p m_cashAmountExpected=%lld testIndex=%d in %s"),
							(const void *)cashLog,
							(long long)TEST_DATA[testIndex].m_cashAmountExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}

					if (TEST_DATA[testIndex].m_cashAmountExpected > 0)
					{
						// Test #1.

						{
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED lastLog=%p expected=NOT NULL testIndex=%d in %s"),
								(const void *)cashLog,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_NE(nullptr, cashLog) << failedMsg;
						}

						// Could do a full equality check between the transfer instance the the log instance.
						// Test #2.
						const time_t lastDateTimeResult = cashLog->GetDateTime();

						{
							bool areEqual = lastDateTimeResult == TEST_DATA[testIndex].m_lastDateTimeExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED lastDateTimeResult=%lld expected=%lld testIndex=%d in %s"),
								(long long)lastDateTimeResult,
								(long long)TEST_DATA[testIndex].m_lastDateTimeExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #3.
						const __int64 cashAmountResult = cashLog->GetAmount();

						{
							bool areEqual = cashAmountResult == TEST_DATA[testIndex].m_cashAmountExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED cashAmountResult=%lld expected=%lld testIndex=%d in %s"),
								(long long)cashAmountResult,
								(long long)TEST_DATA[testIndex].m_cashAmountExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #4.
						const int cashSourceResult = cashLog->GetSource();

						{
							bool areEqual = cashSourceResult == TEST_DATA[testIndex].m_cashSourceExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED cashSourceResult=%d expected=%d testIndex=%d in %s"),
								(int)cashSourceResult,
								(int)TEST_DATA[testIndex].m_cashSourceExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #5.
						const bool cashSuccessfulResult = cashLog->IsSuccessful();

						{
							bool areEqual = cashSuccessfulResult == lastSuccessfulExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED cashSuccessfulResult=%d expected=%d testIndex=%d in %s"),
								(int)cashSuccessfulResult,
								(int)lastSuccessfulExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #6.
						const bool cashPendingResult = cashLog->IsPending();

						{
							bool areEqual = cashPendingResult == lastPendingExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED cashPendingResult=%d expected=%d testIndex=%d in %s"),
								(int)cashPendingResult,
								(int)lastPendingExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}
					}

					if (TEST_DATA[testIndex].m_ncepAmountExpected > 0)
					{
						// Test #7.
						{
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED ncepLog=%p expected=NOT NULL testIndex=%d in %s"),
								(const void *)ncepLog,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_NE(nullptr, ncepLog) << failedMsg;
						}

						// Could do a full equality check between the transfer instance the the log instance.
						// Test #8.
						const time_t ncepDateTimeResult = ncepLog->GetDateTime();

						{
							bool areEqual = ncepDateTimeResult == TEST_DATA[testIndex].m_lastDateTimeExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED ncepDateTimeResult=%lld expected=%lld testIndex=%d in %s"),
								(long long)ncepDateTimeResult,
								(long long)TEST_DATA[testIndex].m_lastDateTimeExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #9.
						const __int64 ncepAmountResult = ncepLog->GetAmount();

						{
							bool areEqual = ncepAmountResult == TEST_DATA[testIndex].m_ncepAmountExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED ncepAmountResult=%lld expected=%lld testIndex=%d in %s"),
								(long long)ncepAmountResult,
								(long long)TEST_DATA[testIndex].m_ncepAmountExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #10.
						const int ncepSourceResult = ncepLog->GetSource();

						{
							bool areEqual = ncepSourceResult == TEST_DATA[testIndex].m_ncepSourceExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED ncepSourceResult=%d expected=%d testIndex=%d in %s"),
								(int)ncepSourceResult,
								(int)TEST_DATA[testIndex].m_ncepSourceExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #11.
						const bool ncepSuccessfulResult = ncepLog->IsSuccessful();

						{
							bool areEqual = ncepSuccessfulResult == lastSuccessfulExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED ncepSuccessfulResult=%d expected=%d testIndex=%d in %s"),
								(int)ncepSuccessfulResult,
								(int)lastSuccessfulExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #12.
						const bool ncepPendingResult = ncepLog->IsPending();

						{
							bool areEqual = ncepPendingResult == lastPendingExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED ncepPendingResult=%d expected=%d testIndex=%d in %s"),
								(int)ncepPendingResult,
								(int)lastPendingExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}
					}
				}

				if (TEST_DATA[testIndex].m_numberOfSuccessLogRecordsAddedExpected >= 1)
				{
					// Get log records for subsequent tests.
					CPersonalBankerLog *ncepSuccessLog(NULL);
					CPersonalBankerLog *cashSuccessLog(NULL);
					if (1 == TEST_DATA[testIndex].m_numberOfLogRecordsAddedExpected)
					{
						if (TEST_DATA[testIndex].m_cashAmountExpected > 0)
						{
							cashSuccessLog = (CPersonalBankerLog *)pbSuccessLog->GetLastLog();
						}
						else
						{
							ncepSuccessLog = (CPersonalBankerLog *)pbSuccessLog->GetLastLog();
						}
					}
					else if (2 == TEST_DATA[testIndex].m_numberOfLogRecordsAddedExpected)
					{
						ncepSuccessLog = (CPersonalBankerLog *)pbSuccessLog->GetLastLog();
						cashSuccessLog = (CPersonalBankerLog *)pbSuccessLog->GetPreviousLog();
					}
					else
					{
						ASSERT_TRUE(false) << _T("FAILED m_numberOfLogRecordsAddedExpected not in 1..2");
					}

					// Test #C.
					{
						bool areEqual = !!ncepSuccessLog == !!TEST_DATA[testIndex].m_ncepAmountExpected;
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED ncepSuccessLog=%p m_ncepAmountExpected=%lld testIndex=%d in %s"),
							(const void *)ncepSuccessLog,
							(long long)TEST_DATA[testIndex].m_ncepAmountExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}

					// Test #D.
					{
						bool areEqual = !!cashSuccessLog == !!TEST_DATA[testIndex].m_cashAmountExpected;
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED cashSuccessLog=%p m_cashAmountExpected=%lld testIndex=%d in %s"),
							(const void *)cashSuccessLog,
							(long long)TEST_DATA[testIndex].m_cashAmountExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}

					if (TEST_DATA[testIndex].m_cashAmountExpected > 0)
					{
						// Test #13.

						{
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED cashSuccessLog=%p expected=NOT NULL testIndex=%d in %s"),
								(const void *)cashSuccessLog,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_NE(nullptr, cashSuccessLog) << failedMsg;
						}

						// Could do a full equality check between the transfer instance the the log instance.
						// Test #14.
						const time_t cashDateTimeResult = cashSuccessLog->GetDateTime();

						{
							bool areEqual = cashDateTimeResult == TEST_DATA[testIndex].m_lastDateTimeExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED slog cashDateTimeResult=%lld expected=%lld testIndex=%d in %s"),
								(long long)cashDateTimeResult,
								(long long)TEST_DATA[testIndex].m_lastDateTimeExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #15.
						const __int64 cashAmountResult = cashSuccessLog->GetAmount();

						{
							bool areEqual = cashAmountResult == TEST_DATA[testIndex].m_cashAmountExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED slog cashAmountResult=%lld expected=%lld testIndex=%d in %s"),
								(long long)cashAmountResult,
								(long long)TEST_DATA[testIndex].m_cashAmountExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #16.
						const int cashSourceResult = cashSuccessLog->GetSource();

						{
							bool areEqual = cashSourceResult == TEST_DATA[testIndex].m_cashSourceExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED slog cashSourceResult=%d expected=%d testIndex=%d in %s"),
								(int)cashSourceResult,
								(int)TEST_DATA[testIndex].m_cashSourceExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #17.
						const bool cashSuccessfulResult = cashSuccessLog->IsSuccessful();

						{
							bool areEqual = cashSuccessfulResult == lastSuccessfulExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED slog cashSuccessfulResult=%d expected=%d testIndex=%d in %s"),
								(int)cashSuccessfulResult,
								(int)lastSuccessfulExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #18.
						const bool cashPendingResult = cashSuccessLog->IsPending();

						{
							bool areEqual = cashPendingResult == lastPendingExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED slog cashPendingResult=%d expected=%d testIndex=%d in %s"),
								(int)cashPendingResult,
								(int)lastPendingExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}
					}

					if (TEST_DATA[testIndex].m_ncepAmountExpected > 0)
					{
						// Test #19.
						{
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED ncepSuccessLog=%p expected=NOT NULL testIndex=%d in %s"),
								(const void *)ncepSuccessLog,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_NE(nullptr, ncepSuccessLog) << failedMsg;
						}

						// Could do a full equality check between the transfer instance the the log instance.
						// Test #20.
						const time_t ncepDateTimeResult = ncepSuccessLog->GetDateTime();

						{
							bool areEqual = ncepDateTimeResult == TEST_DATA[testIndex].m_lastDateTimeExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED ncepDateTimeResult=%lld expected=%lld testIndex=%d in %s"),
								(long long)ncepDateTimeResult,
								(long long)TEST_DATA[testIndex].m_lastDateTimeExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #21.
						const __int64 ncepAmountResult = ncepSuccessLog->GetAmount();

						{
							bool areEqual = ncepAmountResult == TEST_DATA[testIndex].m_ncepAmountExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED ncepAmountResult=%lld expected=%lld testIndex=%d in %s"),
								(long long)ncepAmountResult,
								(long long)TEST_DATA[testIndex].m_ncepAmountExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #22.
						const int ncepSourceResult = ncepSuccessLog->GetSource();

						{
							bool areEqual = ncepSourceResult == TEST_DATA[testIndex].m_ncepSourceExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED ncepSourceResult=%d expected=%d testIndex=%d in %s"),
								(int)ncepSourceResult,
								(int)TEST_DATA[testIndex].m_ncepSourceExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #23.
						const bool ncepSuccessfulResult = ncepSuccessLog->IsSuccessful();

						{
							bool areEqual = ncepSuccessfulResult == lastSuccessfulExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED ncepSuccessfulResult=%d expected=%d testIndex=%d in %s"),
								(int)ncepSuccessfulResult,
								(int)lastSuccessfulExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #24.
						const bool ncepPendingResult = ncepSuccessLog->IsPending();

						{
							bool areEqual = ncepPendingResult == lastPendingExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED ncepPendingResult=%d expected=%d testIndex=%d in %s"),
								(int)ncepPendingResult,
								(int)lastPendingExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}
					}
				}
				if (TEST_DATA[testIndex].m_hasPlayerInfo)
				{
					// Test #25.
					const CPlayerInfo * const playerInfo = player.GetPlayerInfo();

					{
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED playerInfo=%p expected=NOT NULL testIndex=%d in %s"),
							(const void *)playerInfo,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_NE(nullptr, playerInfo) << failedMsg;
					}

					// Test #26.
					const DWORD playerInfoPointsResult = playerInfo->GetPoints();

					{
						bool areEqual = playerInfoPointsResult == TEST_DATA[testIndex].m_playerInfoPointsExpected;
                        std::string failedMsg;
						failedMsg = FormatString(_T("FAILED slog playerInfoPointsResult=%u expected=%u testIndex=%d in %s"),
							(unsigned)playerInfoPointsResult,
							(unsigned)TEST_DATA[testIndex].m_playerInfoPointsExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}

					// Test #27.
					const DWORD playerInfoCountdownResult = playerInfo->GetCountdown();

					{
						bool areEqual = playerInfoCountdownResult == TEST_DATA[testIndex].m_playerInfoCountdownExpected;
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED slog playerInfoCountdownResult=%u expected=%u testIndex=%d in %s"),
							(unsigned)playerInfoCountdownResult,
							(unsigned)TEST_DATA[testIndex].m_playerInfoCountdownExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}
				}

				// Cleanup.
				delete pbLog;
				delete pbSuccessLog;
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBanker::LogTransfer()
		///     CPersonalBanker::GetLastLog()
		///     CPersonalBanker::GetSuccessLogCopy()
		///     CPlayerInfo::GetPoints()
		/// </summary>
		/// <remarks>
		/// Test with non-canceled cash, non-restricted and restricted transfers.
		/// Does NOT do a full equality check between the transfer instance the the log instance.
		/// Note: Code used for detection of log records is complicated by what other tests do to the logs.
		/// </remarks>
		TEST(PersonalBanker3, LogTransfer_TransferNotCanceledAllTest)
		{
			typedef struct
			{
				// Inputs.
				const PersonalBankerTransferType m_transferType;
				const time_t m_currentTime;
				const bool m_hasPlayerInfo;
				const DWORD m_playerInfoPoints;
				const DWORD m_playerInfoCountdown;
				const DWORD m_amountInDollars;
				const __int64 m_nonRestrictedAmount;
				const __int64 m_restrictedAmount;

				// Expected results.
				const int m_numberOfLogRecordsAddedExpected;
				const int m_numberOfSuccessLogRecordsAddedExpected;
				const int m_cashSourceExpected;
				const time_t m_lastDateTimeExpected;
				const __int64 m_cashAmountExpected;
				const DWORD m_playerInfoPointsExpected;
				const DWORD m_playerInfoCountdownExpected;
				const __int64 m_cepAmountExpected;
				const __int64 m_ncepAmountExpected;
				const int m_cepSourceExpected;
				const int m_ncepSourceExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				// Non-zero cash amounts without player info.
				{ Undeclared, time_t(123), false, 5000, 100, 1, 10, 7, 3, 3, CashFromGame, time_t(123), 100, 5000, 100, 10, 7, PointsFromGame, PromoDollarsFromGame },
				{ CashToGame, time_t(1232), false, 5001, 101, 2, 1, 11, 1, 1, CashToGame, time_t(1232), 200, 5001, 101, 0, 0, PointsFromGame, PromoDollarsFromGame },
				{ PointsToGame, time_t(1233), false, 5002, 102, 3, 23, 231, 1, 1, PointsToGame, time_t(1233), 300, 5002, 102, 0, 0, PointsFromGame, PromoDollarsFromGame },
				{ PromoDollarsToGame, time_t(1234), false, 5003, 103, 4, 34, 341, 1, 1, PromoDollarsToGame, time_t(1234), 400, 5003, 103, 0, 0, PointsFromGame, PromoDollarsFromGame },
				{ CashFromGame, time_t(12345), false, 5004, 104, 5, 45, 451, 3, 3, CashFromGame, time_t(12345), 500, 5004, 104, 45, 451, PointsFromGame, PromoDollarsFromGame },
				{ PointsFromGame, time_t(123456), false, 5005, 105, 6, 56, 561, 3, 3, CashFromGame, time_t(123456), 600, 5005, 105, 56, 561, PointsFromGame, PromoDollarsFromGame },
				{ PromoDollarsFromGame, time_t(1234567), false, 5006, 106, 7, 67, 671, 3, 3, CashFromGame, time_t(1234567), 700, 5006, 106, 67, 671, PointsFromGame, PromoDollarsFromGame },
				{ AllFromGame, time_t(12345678), false, 5007, 107, 8, 78, 781, 3, 3, CashFromGame, time_t(12345678), 800, 5007, 107, 78, 781, PointsFromGame, PromoDollarsFromGame },
				{ (PersonalBankerTransferType)(AllFromGame+1), time_t(12345677), false, 5008, 108, 77, 777, 7771, 3, 3, CashFromGame, time_t(12345677), 7700, 5008, 108, 777, 7771, PointsFromGame, PromoDollarsFromGame },
				{ (PersonalBankerTransferType)-1, time_t(123456789), false, 5009, 109, 9, 999, 9991, 3, 3, CashFromGame, time_t(123456789), 900, 5009, 109, 999, 9991, PointsFromGame, PromoDollarsFromGame },

				// Zero cash amounts without player info.
				{ Undeclared, time_t(123), false, 5010, 110, 0, 12, 121, 2, 2, CashFromGame, time_t(123), 0, 5010, 110, 12, 121, PointsFromGame, PromoDollarsFromGame },
				{ CashToGame, time_t(1232), false, 5011, 111, 0, 23, 231, 0, 0, CashToGame, time_t(1232), 0, 5011, 111, 23, 231, PointsFromGame, PromoDollarsFromGame },
				{ PointsToGame, time_t(1233), false, 5012, 112, 0, 34, 341, 0, 0, PointsToGame, time_t(1233), 0, 5012, 112, 34, 341, PointsFromGame, PromoDollarsFromGame },
				{ PromoDollarsToGame, time_t(1234), false, 5013, 113, 0, 45, 451, 0, 0, PromoDollarsToGame, time_t(1234), 0, 5013, 113, 45, 451, PointsFromGame, PromoDollarsFromGame },
				{ CashFromGame, time_t(12345), false, 5014, 114, 0, 56, 561, 2, 2, CashFromGame, time_t(12345), 0, 5015, 114, 56, 561, PointsFromGame, PromoDollarsFromGame },
				{ PointsFromGame, time_t(123456), false, 5015, 115, 0, 67, 671, 2, 2, CashFromGame, time_t(123456), 0, 5015, 115, 67, 671, PointsFromGame, PromoDollarsFromGame },
				{ PromoDollarsFromGame, time_t(1234567), false, 5016, 116, 0, 78, 781, 2, 2, CashFromGame, time_t(1234567), 0, 5016, 116, 78, 781, PointsFromGame, PromoDollarsFromGame },
				{ AllFromGame, time_t(12345678), false, 5017, 117, 0, 89, 891, 2, 2, CashFromGame, time_t(12345678), 0, 5017, 117, 89, 891, PointsFromGame, PromoDollarsFromGame },
				{ (PersonalBankerTransferType)(AllFromGame+1), time_t(12345677), false, 5018, 118, 0, 901, 9011, 2, 2, CashFromGame, time_t(12345677), 0, 5018, 118, 901, 9011, PointsFromGame, PromoDollarsFromGame },
				{ (PersonalBankerTransferType)-1, time_t(123456789), false, 5019, 119, 0, 902, 9021, 2, 2, CashFromGame, time_t(123456789), 0, 5010, 119, 902, 9021, PointsFromGame, PromoDollarsFromGame },

				// Non-zero cash amounts with player info.
				{ Undeclared, time_t(123), true, 5020, 120, 1, 10, 7, 3, 3, CashFromGame, time_t(123), 100, 5020, 120, 10, 7, PointsFromGame, PromoDollarsFromGame },
				{ CashToGame, time_t(1232), true, 5021, 121, 2, 1, 11, 1, 1, CashToGame, time_t(1232), 200, 5021, 121, 0, 0, PointsFromGame, PromoDollarsFromGame },
				{ PointsToGame, time_t(1233), true, 5022, 122, 3, 23, 231, 1, 1, PointsToGame, time_t(1233), 300, 4468, 122, 0, 0, PointsFromGame, PromoDollarsFromGame },
				{ PromoDollarsToGame, time_t(1234), true, 5023, 123, 4, 34, 341, 1, 1, PromoDollarsToGame, time_t(1234), 400, 5023, 123, 0, 0, PointsFromGame, PromoDollarsFromGame },
				{ CashFromGame, time_t(12345), true, 5024, 124, 5, 45, 451, 3, 3, CashFromGame, time_t(12345), 500, 5024, 124, 45, 451, PointsFromGame, PromoDollarsFromGame },
				{ PointsFromGame, time_t(123456), true, 5025, 125, 6, 56, 561, 3, 3, CashFromGame, time_t(123456), 600, 5025, 125, 56, 561, PointsFromGame, PromoDollarsFromGame },
				{ PromoDollarsFromGame, time_t(1234567), true, 5026, 126, 7, 67, 671, 3, 3, CashFromGame, time_t(1234567), 700, 5026, 126, 67, 671, PointsFromGame, PromoDollarsFromGame },
				{ AllFromGame, time_t(12345678), true, 5027, 127, 8, 78, 781, 3, 3, CashFromGame, time_t(12345678), 800, 5027, 127, 78, 781, PointsFromGame, PromoDollarsFromGame },
				{ (PersonalBankerTransferType)(AllFromGame+1), time_t(12345677), true, 5028, 128, 77, 777, 7771, 3, 3, CashFromGame, time_t(12345677), 7700, 5028, 128, 777, 7771, PointsFromGame, PromoDollarsFromGame },
				{ (PersonalBankerTransferType)-1, time_t(123456789), true, 5029, 129, 9, 999, 9991, 3, 3, CashFromGame, time_t(123456789), 900, 5029, 129, 999, 9991, PointsFromGame, PromoDollarsFromGame },

				// Zero cash amounts with player info.
				{ Undeclared, time_t(123), true, 5030, 130, 0, 12, 121, 2, 2, CashFromGame, time_t(123), 0, 5030, 130, 12, 121, PointsFromGame, PromoDollarsFromGame },
				{ CashToGame, time_t(1232), true, 5031, 131, 0, 23, 231, 0, 0, CashToGame, time_t(1232), 0, 5031, 131, 23, 0, PointsFromGame, PromoDollarsFromGame },
				{ PointsToGame, time_t(1233), true, 5032, 132, 0, 34, 341, 0, 0, PointsToGame, time_t(1233), 0, 5032, 132, 34, 0, PointsFromGame, PromoDollarsFromGame },
				{ PromoDollarsToGame, time_t(1234), true, 5033, 133, 0, 45, 451, 0, 0, PromoDollarsToGame, time_t(1234), 0, 5033, 133, 45, 0, PointsFromGame, PromoDollarsFromGame },
				{ CashFromGame, time_t(12345), true, 5034, 134, 0, 56, 561, 2, 2, CashFromGame, time_t(12345), 0, 5034, 134, 56, 561, PointsFromGame, PromoDollarsFromGame },
				{ PointsFromGame, time_t(123456), true, 5035, 135, 0, 67, 671, 2, 2, CashFromGame, time_t(123456), 0, 5035, 135, 67, 671, PointsFromGame, PromoDollarsFromGame },
				{ PromoDollarsFromGame, time_t(1234567), true, 5036, 136, 0, 78, 781, 2, 2, CashFromGame, time_t(1234567), 0, 5036, 136, 78, 781, PointsFromGame, PromoDollarsFromGame },
				{ AllFromGame, time_t(12345678), true, 5037, 137, 0, 89, 891, 2, 2, CashFromGame, time_t(12345678), 0, 5037, 137, 89, 891, PointsFromGame, PromoDollarsFromGame },
				{ (PersonalBankerTransferType)(AllFromGame+1), time_t(12345677), true, 5038, 138, 0, 901, 9011, 2, 2, CashFromGame, time_t(12345677), 0, 5038, 138, 901, 9011, PointsFromGame, PromoDollarsFromGame },
				{ (PersonalBankerTransferType)-1, time_t(123456789), true, 5039, 139, 0, 902, 9021, 2, 2, CashFromGame, time_t(123456789), 0, 5039, 139, 902, 9021, PointsFromGame, PromoDollarsFromGame },

				// Non-zero cash amounts with player info and PointsToGame.
				{ PointsToGame, time_t(1233), true, 126, 40, 1, 12, 13, 1, 1, PointsToGame, time_t(1233), 100, 1, 40, 0, 0, PointsFromGame, PromoDollarsFromGame },
				{ PointsToGame, time_t(1233), true, 125, 41, 1, 12, 13, 1, 1, PointsToGame, time_t(1233), 100, 0, 41, 0, 0, PointsFromGame, PromoDollarsFromGame },
				{ PointsToGame, time_t(1233), true, 124, 42, 1, 12, 13, 1, 1, PointsToGame, time_t(1233), 100, 0, 42, 0, 0, PointsFromGame, PromoDollarsFromGame },
				{ PointsToGame, time_t(1233), true, 267, 43, 2, 34, 32, 1, 1, PointsToGame, time_t(1233), 200, 1, 43, 0, 0, PointsFromGame, PromoDollarsFromGame },
				{ PointsToGame, time_t(1233), true, 266, 44, 2, 34, 32, 1, 1, PointsToGame, time_t(1233), 200, 0, 44, 0, 0, PointsFromGame, PromoDollarsFromGame },
				{ PointsToGame, time_t(1233), true, 265, 45, 2, 34, 32, 1, 1, PointsToGame, time_t(1233), 200, 0, 45, 0, 0, PointsFromGame, PromoDollarsFromGame },

				// Use rollover to test amountTransfered=0 and pointsTransfered=0 in CPersonalBanker::LogTransfer()
				{ PointsToGame, time_t(1233), true, 233, 46, 0xffffffff, 0x7fffffffffffffff - (DWORD)(0xffffffff * ONE_DOLLAR_IN_CENTS) +1, (__int64)0x8000000000000000,
     				1, 1, PointsToGame, time_t(1233), (DWORD)(0xffffffff * ONE_DOLLAR_IN_CENTS), 233, 46, 0, 0, PointsFromGame, PromoDollarsFromGame },
			};

			// More test data - expected results that are the same for each iteration.
			const long pointsPerDollar = 100;
			const bool lastSuccessfulExpected(true);
			const bool lastPendingExpected(true);
			const DWORD playerInfoPointsExpected(0);

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// More test data - expected results that are dependent on previous tests.
				LogIndexInfoExpected logIndexInfoExpected;
				LogTransfer_GetLogIndexInfoExpected(logIndexInfoExpected,
					TEST_DATA[testIndex].m_numberOfLogRecordsAddedExpected,
					TEST_DATA[testIndex].m_numberOfSuccessLogRecordsAddedExpected);

				// Setup.
				CPlayer player;
				if (TEST_DATA[testIndex].m_hasPlayerInfo)
				{
					CPlayerInfo *playerInfo = new CPlayerInfo();
					playerInfo->SetPointsAndCountdown(
						TEST_DATA[testIndex].m_playerInfoPoints,
						TEST_DATA[testIndex].m_playerInfoCountdown, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);
					player.SetPlayerInfo(playerInfo);
				}

				CConfig varConfig(false, "");
				varConfig.SetPointsPerDollar(pointsPerDollar);
				const CConfig &config(varConfig);

				CGames games;

				CPersonalBanker varPb;
				varPb.CreateTransfer(CUtilities::GetTickCount());

				// SetState() to affect BeginPersonalBankerTransfer().
				varPb.SetState(PBStateInfoReceived);
				byte pbEFTNumber = 1;
				varPb.BeginPersonalBankerTransfer(games,
					TEST_DATA[testIndex].m_transferType,
					TEST_DATA[testIndex].m_amountInDollars,
					pbEFTNumber);

				// SetState() to affect ProcessPersonalBankerTransferComplete().
				varPb.SetState(PBStateTransferingFromToGame);

				CPersonalBankerTransfer *pbTransfer = varPb.GetTransferCopy();
				ASSERT_NE(nullptr, pbTransfer) << _T("FAILED pbTransfer NULL, expected NOT NULL");
				pbTransfer->SetAmount(TEST_DATA[testIndex].m_amountInDollars * ONE_DOLLAR_IN_CENTS);
				pbTransfer->SetNonrestrictedAmount(TEST_DATA[testIndex].m_nonRestrictedAmount);
				pbTransfer->SetRestrictedAmount(TEST_DATA[testIndex].m_restrictedAmount);
				byte meterNumberToAddTo(0xff);
				bool isDuplicate(false);
				const bool isEFT(false);
				varPb.ProcessPersonalBankerTransferComplete(pbTransfer,
					games,
					meterNumberToAddTo,
					isDuplicate,
					isEFT);

				// Perform operations to be tested.
				varPb.LogTransfer(&player, config, TEST_DATA[testIndex].m_currentTime);
				const CPersonalBanker &pb(varPb);

				// Test log and log indexes info.
				CLogs *pbLog = varPb.GetLogCopy();
				CLogs *pbSuccessLog = varPb.GetSuccessLogCopy();
				LogTransfer_AssertLogInfoExpected(pbLog,
					pbSuccessLog,
					logIndexInfoExpected,
					testIndex,
					__TFUNCTION__);


				if (TEST_DATA[testIndex].m_numberOfLogRecordsAddedExpected >= 1)
				{
					// Get log records for subsequent tests.
					CPersonalBankerLog *cepLog(NULL);
					CPersonalBankerLog *ncepLog(NULL);
					CPersonalBankerLog *cashLog(NULL);
					if (1 == TEST_DATA[testIndex].m_numberOfLogRecordsAddedExpected)
					{
						cashLog = (CPersonalBankerLog *)pbLog->GetLastLog();
					}
					else if (2 == TEST_DATA[testIndex].m_numberOfLogRecordsAddedExpected)
					{
						ncepLog = (CPersonalBankerLog *)pbLog->GetLastLog();
						cepLog = (CPersonalBankerLog *)pbLog->GetPreviousLog();
					}
					else if (3 == TEST_DATA[testIndex].m_numberOfLogRecordsAddedExpected)
					{
						ncepLog = (CPersonalBankerLog *)pbLog->GetLastLog();
						cepLog = (CPersonalBankerLog *)pbLog->GetPreviousLog();
						cashLog = (CPersonalBankerLog *)pbLog->GetPreviousLog();
					}
					else
					{
						ASSERT_TRUE(false) << _T("FAILED m_numberOfLogRecordsAddedExpected not in 1..3");
					}

					// Test #A.
					{
						bool areEqual = !!cepLog == !!TEST_DATA[testIndex].m_cepAmountExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED cepLog=%p m_cepAmountExpected=%lld testIndex=%d in %s"),
								(const void *)cepLog,
								(long long)TEST_DATA[testIndex].m_cepAmountExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}

					// Test #B.
					{
						bool areEqual = !!ncepLog == !!TEST_DATA[testIndex].m_ncepAmountExpected;
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED ncepLog=%p m_ncepAmountExpected=%lld testIndex=%d in %s"),
							(const void *)ncepLog,
							(long long)TEST_DATA[testIndex].m_ncepAmountExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}

					// Test #C.
					{
						bool areEqual = !!cashLog == !!TEST_DATA[testIndex].m_cashAmountExpected;
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED cashLog=%p m_cashAmountExpected=%lld testIndex=%d in %s"),
							(const void *)cashLog,
							(long long)TEST_DATA[testIndex].m_cashAmountExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}

					if (TEST_DATA[testIndex].m_cashAmountExpected > 0)
					{
						// Test #1.

						{
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED lastLog=%p expected=NOT NULL testIndex=%d in %s"),
								(const void *)cashLog,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_NE(nullptr, cashLog) << failedMsg;
						}

						// Could do a full equality check between the transfer instance the the log instance.
						// Test #2.
						const time_t lastDateTimeResult = cashLog->GetDateTime();

						{
							bool areEqual = lastDateTimeResult == TEST_DATA[testIndex].m_lastDateTimeExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED lastDateTimeResult=%lld expected=%lld testIndex=%d in %s"),
								(long long)lastDateTimeResult,
								(long long)TEST_DATA[testIndex].m_lastDateTimeExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #3.
						const __int64 cashAmountResult = cashLog->GetAmount();

						{
							bool areEqual = cashAmountResult == TEST_DATA[testIndex].m_cashAmountExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED cashAmountResult=%lld expected=%lld testIndex=%d in %s"),
								(long long)cashAmountResult,
								(long long)TEST_DATA[testIndex].m_cashAmountExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #4.
						const int cashSourceResult = cashLog->GetSource();

						{
							bool areEqual = cashSourceResult == TEST_DATA[testIndex].m_cashSourceExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED cashSourceResult=%d expected=%d testIndex=%d in %s"),
								(int)cashSourceResult,
								(int)TEST_DATA[testIndex].m_cashSourceExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #5.
						const bool cashSuccessfulResult = cashLog->IsSuccessful();

						{
							bool areEqual = cashSuccessfulResult == lastSuccessfulExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED cashSuccessfulResult=%d expected=%d testIndex=%d in %s"),
								(int)cashSuccessfulResult,
								(int)lastSuccessfulExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #6.
						const bool cashPendingResult = cashLog->IsPending();

						{
							bool areEqual = cashPendingResult == lastPendingExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED cashPendingResult=%d expected=%d testIndex=%d in %s"),
								(int)cashPendingResult,
								(int)lastPendingExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}
					}

					if (TEST_DATA[testIndex].m_cepAmountExpected > 0)
					{
						// Test #7.
						{
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED cepLog=%p expected=NOT NULL testIndex=%d in %s"),
								(const void *)cepLog,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_NE(nullptr, cepLog) << failedMsg;
						}

						// Could do a full equality check between the transfer instance the the log instance.
						// Test #8.
						const time_t cepDateTimeResult = cepLog->GetDateTime();

						{
							bool areEqual = cepDateTimeResult == TEST_DATA[testIndex].m_lastDateTimeExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED cepDateTimeResult=%lld expected=%lld testIndex=%d in %s"),
								(long long)cepDateTimeResult,
								(long long)TEST_DATA[testIndex].m_lastDateTimeExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #9.
						const __int64 cepAmountResult = cepLog->GetAmount();

						{
							bool areEqual = cepAmountResult == TEST_DATA[testIndex].m_cepAmountExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED cepAmountResult=%lld expected=%lld testIndex=%d in %s"),
								(long long)cepAmountResult,
								(long long)TEST_DATA[testIndex].m_cepAmountExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #10.
						const int cepSourceResult = cepLog->GetSource();

						{
							bool areEqual = cepSourceResult == TEST_DATA[testIndex].m_cepSourceExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED cepSourceResult=%d expected=%d testIndex=%d in %s"),
								(int)cepSourceResult,
								(int)TEST_DATA[testIndex].m_cepSourceExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #11.
						const bool cepSuccessfulResult = cepLog->IsSuccessful();

						{
							bool areEqual = cepSuccessfulResult == lastSuccessfulExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED cepSuccessfulResult=%d expected=%d testIndex=%d in %s"),
								(int)cepSuccessfulResult,
								(int)lastSuccessfulExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #12.
						const bool cepPendingResult = cepLog->IsPending();

						{
							bool areEqual = cepPendingResult == lastPendingExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED cepPendingResult=%d expected=%d testIndex=%d in %s"),
								(int)cepPendingResult,
								(int)lastPendingExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}
					}

					if (TEST_DATA[testIndex].m_ncepAmountExpected > 0)
					{
						// Test #13.
						{
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED ncepLog=%p expected=NOT NULL testIndex=%d in %s"),
								(const void *)ncepLog,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_NE(nullptr, ncepLog) << failedMsg;
						}

						// Could do a full equality check between the transfer instance the the log instance.
						// Test #14.
						const time_t ncepDateTimeResult = ncepLog->GetDateTime();

						{
							bool areEqual = ncepDateTimeResult == TEST_DATA[testIndex].m_lastDateTimeExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED ncepDateTimeResult=%lld expected=%lld testIndex=%d in %s"),
								(long long)ncepDateTimeResult,
								(long long)TEST_DATA[testIndex].m_lastDateTimeExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #15.
						const __int64 ncepAmountResult = ncepLog->GetAmount();

						{
							bool areEqual = ncepAmountResult == TEST_DATA[testIndex].m_ncepAmountExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED ncepAmountResult=%lld expected=%lld testIndex=%d in %s"),
								(long long)ncepAmountResult,
								(long long)TEST_DATA[testIndex].m_ncepAmountExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #16.
						const int ncepSourceResult = ncepLog->GetSource();

						{
							bool areEqual = ncepSourceResult == TEST_DATA[testIndex].m_ncepSourceExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED ncepSourceResult=%d expected=%d testIndex=%d in %s"),
								(int)ncepSourceResult,
								(int)TEST_DATA[testIndex].m_ncepSourceExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #17.
						const bool ncepSuccessfulResult = ncepLog->IsSuccessful();

						{
							bool areEqual = ncepSuccessfulResult == lastSuccessfulExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED ncepSuccessfulResult=%d expected=%d testIndex=%d in %s"),
								(int)ncepSuccessfulResult,
								(int)lastSuccessfulExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #18.
						const bool ncepPendingResult = ncepLog->IsPending();

						{
							bool areEqual = ncepPendingResult == lastPendingExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED ncepPendingResult=%d expected=%d testIndex=%d in %s"),
								(int)ncepPendingResult,
								(int)lastPendingExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}
					}
				}

				if (TEST_DATA[testIndex].m_numberOfSuccessLogRecordsAddedExpected >= 1)
				{
					// Get log records for subsequent tests.
					CPersonalBankerLog *cepSuccessLog(NULL);
					CPersonalBankerLog *ncepSuccessLog(NULL);
					CPersonalBankerLog *cashSuccessLog(NULL);
					if (1 == TEST_DATA[testIndex].m_numberOfLogRecordsAddedExpected)
					{
						cashSuccessLog = (CPersonalBankerLog *)pbSuccessLog->GetLastLog();
					}
					else if (2 == TEST_DATA[testIndex].m_numberOfLogRecordsAddedExpected)
					{
						ncepSuccessLog = (CPersonalBankerLog *)pbSuccessLog->GetLastLog();
						cepSuccessLog = (CPersonalBankerLog *)pbSuccessLog->GetPreviousLog();
					}
					else if (3 == TEST_DATA[testIndex].m_numberOfLogRecordsAddedExpected)
					{
						ncepSuccessLog = (CPersonalBankerLog *)pbSuccessLog->GetLastLog();
						cepSuccessLog = (CPersonalBankerLog *)pbSuccessLog->GetPreviousLog();
						cashSuccessLog = (CPersonalBankerLog *)pbSuccessLog->GetPreviousLog();
					}
					else
					{
						ASSERT_TRUE(false) << _T("FAILED slog m_numberOfLogRecordsAddedExpected not in 1..3");
					}

					// Test #D.
					{
						bool areEqual = !!cepSuccessLog == !!TEST_DATA[testIndex].m_cepAmountExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED cepSuccessLog=%p m_cepAmountExpected=%lld testIndex=%d in %s"),
								(const void *)cepSuccessLog,
								(long long)TEST_DATA[testIndex].m_cepAmountExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}

					// Test #E.
					{
						bool areEqual = !!ncepSuccessLog == !!TEST_DATA[testIndex].m_ncepAmountExpected;
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED ncepSuccessLog=%p m_ncepAmountExpected=%lld testIndex=%d in %s"),
							(const void *)ncepSuccessLog,
							(long long)TEST_DATA[testIndex].m_ncepAmountExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}

					// Test #F.
					{
						bool areEqual = !!cashSuccessLog == !!TEST_DATA[testIndex].m_cashAmountExpected;
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED cashSuccessLog=%p m_cashAmountExpected=%lld testIndex=%d in %s"),
							(const void *)cashSuccessLog,
							(long long)TEST_DATA[testIndex].m_cashAmountExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}

					if (TEST_DATA[testIndex].m_cashAmountExpected > 0)
					{
						// Test #19.

						{
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED cashSuccessLog=%p expected=NOT NULL testIndex=%d in %s"),
								(const void *)cashSuccessLog,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_NE(nullptr, cashSuccessLog) << failedMsg;
						}

						// Could do a full equality check between the transfer instance the the log instance.
						// Test #20.
						const time_t cashDateTimeResult = cashSuccessLog->GetDateTime();

						{
							bool areEqual = cashDateTimeResult == TEST_DATA[testIndex].m_lastDateTimeExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED slog cashDateTimeResult=%lld expected=%lld testIndex=%d in %s"),
								(long long)cashDateTimeResult,
								(long long)TEST_DATA[testIndex].m_lastDateTimeExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #21.
						const __int64 cashAmountResult = cashSuccessLog->GetAmount();

						{
							bool areEqual = cashAmountResult == TEST_DATA[testIndex].m_cashAmountExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED slog cashAmountResult=%lld expected=%lld testIndex=%d in %s"),
								(long long)cashAmountResult,
								(long long)TEST_DATA[testIndex].m_cashAmountExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #22.
						const int cashSourceResult = cashSuccessLog->GetSource();

						{
							bool areEqual = cashSourceResult == TEST_DATA[testIndex].m_cashSourceExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED slog cashSourceResult=%d expected=%d testIndex=%d in %s"),
								(int)cashSourceResult,
								(int)TEST_DATA[testIndex].m_cashSourceExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #23.
						const bool cashSuccessfulResult = cashSuccessLog->IsSuccessful();

						{
							bool areEqual = cashSuccessfulResult == lastSuccessfulExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED slog cashSuccessfulResult=%d expected=%d testIndex=%d in %s"),
								(int)cashSuccessfulResult,
								(int)lastSuccessfulExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #24.
						const bool cashPendingResult = cashSuccessLog->IsPending();

						{
							bool areEqual = cashPendingResult == lastPendingExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED slog cashPendingResult=%d expected=%d testIndex=%d in %s"),
								(int)cashPendingResult,
								(int)lastPendingExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}
					}

					if (TEST_DATA[testIndex].m_cepAmountExpected > 0)
					{
						// Test #25.
						{
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED cepSuccessLog=%p expected=NOT NULL testIndex=%d in %s"),
								(const void *)cepSuccessLog,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_NE(nullptr, cepSuccessLog) << failedMsg;
						}

						// Could do a full equality check between the transfer instance the the log instance.
						// Test #26.
						const time_t cepDateTimeResult = cepSuccessLog->GetDateTime();

						{
							bool areEqual = cepDateTimeResult == TEST_DATA[testIndex].m_lastDateTimeExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED cepDateTimeResult=%lld expected=%lld testIndex=%d in %s"),
								(long long)cepDateTimeResult,
								(long long)TEST_DATA[testIndex].m_lastDateTimeExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #27.
						const __int64 cepAmountResult = cepSuccessLog->GetAmount();

						{
							bool areEqual = cepAmountResult == TEST_DATA[testIndex].m_cepAmountExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED cepAmountResult=%lld expected=%lld testIndex=%d in %s"),
								(long long)cepAmountResult,
								(long long)TEST_DATA[testIndex].m_cepAmountExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #28.
						const int cepSourceResult = cepSuccessLog->GetSource();

						{
							bool areEqual = cepSourceResult == TEST_DATA[testIndex].m_cepSourceExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED cepSourceResult=%d expected=%d testIndex=%d in %s"),
								(int)cepSourceResult,
								(int)TEST_DATA[testIndex].m_cepSourceExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #29.
						const bool cepSuccessfulResult = cepSuccessLog->IsSuccessful();

						{
							bool areEqual = cepSuccessfulResult == lastSuccessfulExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED cepSuccessfulResult=%d expected=%d testIndex=%d in %s"),
								(int)cepSuccessfulResult,
								(int)lastSuccessfulExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #30.
						const bool cepPendingResult = cepSuccessLog->IsPending();

						{
							bool areEqual = cepPendingResult == lastPendingExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED cepPendingResult=%d expected=%d testIndex=%d in %s"),
								(int)cepPendingResult,
								(int)lastPendingExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}
					}

					if (TEST_DATA[testIndex].m_ncepAmountExpected > 0)
					{
						// Test #31.
						{
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED ncepSuccessLog=%p expected=NOT NULL testIndex=%d in %s"),
								(const void *)ncepSuccessLog,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_NE(nullptr, ncepSuccessLog) << failedMsg;
						}

						// Could do a full equality check between the transfer instance the the log instance.
						// Test #32.
						const time_t ncepDateTimeResult = ncepSuccessLog->GetDateTime();

						{
							bool areEqual = ncepDateTimeResult == TEST_DATA[testIndex].m_lastDateTimeExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED ncepDateTimeResult=%lld expected=%lld testIndex=%d in %s"),
								(long long)ncepDateTimeResult,
								(long long)TEST_DATA[testIndex].m_lastDateTimeExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #33.
						const __int64 ncepAmountResult = ncepSuccessLog->GetAmount();

						{
							bool areEqual = ncepAmountResult == TEST_DATA[testIndex].m_ncepAmountExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED ncepAmountResult=%lld expected=%lld testIndex=%d in %s"),
								(long long)ncepAmountResult,
								(long long)TEST_DATA[testIndex].m_ncepAmountExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #34.
						const int ncepSourceResult = ncepSuccessLog->GetSource();

						{
							bool areEqual = ncepSourceResult == TEST_DATA[testIndex].m_ncepSourceExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED ncepSourceResult=%d expected=%d testIndex=%d in %s"),
								(int)ncepSourceResult,
								(int)TEST_DATA[testIndex].m_ncepSourceExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #35.
						const bool ncepSuccessfulResult = ncepSuccessLog->IsSuccessful();

						{
							bool areEqual = ncepSuccessfulResult == lastSuccessfulExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED ncepSuccessfulResult=%d expected=%d testIndex=%d in %s"),
								(int)ncepSuccessfulResult,
								(int)lastSuccessfulExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}

						// Test #36.
						const bool ncepPendingResult = ncepSuccessLog->IsPending();

						{
							bool areEqual = ncepPendingResult == lastPendingExpected;
							std::string failedMsg;
							failedMsg = FormatString(_T("FAILED ncepPendingResult=%d expected=%d testIndex=%d in %s"),
								(int)ncepPendingResult,
								(int)lastPendingExpected,
								(int)testIndex,
								static_cast<LPCTSTR>(__TFUNCTION__));
							ASSERT_TRUE(areEqual) << failedMsg;
						}
					}
				}

				if (TEST_DATA[testIndex].m_hasPlayerInfo)
				{
					// Test #37.
					const CPlayerInfo * const playerInfo = player.GetPlayerInfo();

					{
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED playerInfo=%p expected=NOT NULL testIndex=%d in %s"),
							(const void *)playerInfo,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_NE(nullptr, playerInfo) << failedMsg;
					}

					// Test #38.
					const DWORD playerInfoPointsResult = playerInfo->GetPoints();

					{
						bool areEqual = playerInfoPointsResult == TEST_DATA[testIndex].m_playerInfoPointsExpected;
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED slog playerInfoPointsResult=%u expected=%u testIndex=%d in %s"),
							(unsigned)playerInfoPointsResult,
							(unsigned)TEST_DATA[testIndex].m_playerInfoPointsExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}

					// Test #39.
					const DWORD playerInfoCountdownResult = playerInfo->GetCountdown();

					{
						bool areEqual = playerInfoCountdownResult == TEST_DATA[testIndex].m_playerInfoCountdownExpected;
						std::string failedMsg;
						failedMsg = FormatString(_T("FAILED slog playerInfoCountdownResult=%u expected=%u testIndex=%d in %s"),
							(unsigned)playerInfoCountdownResult,
							(unsigned)TEST_DATA[testIndex].m_playerInfoCountdownExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						ASSERT_TRUE(areEqual) << failedMsg;
					}
				}

				// Cleanup.
				delete pbLog;
				delete pbSuccessLog;
			}
		}

