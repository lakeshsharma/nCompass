
#include "stdafx.h"
#include "PersonalBankerTransfer.h"
#include "Card.h"
#include "DirectoryManager.h"
#include "gtest/gtest.h"


	class PersonalBankerTransferUnitTests : public ::testing::Test
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
    };
		TEST_F(PersonalBankerTransferUnitTests,PersonalBankerTransferConstructionTest)
		{

			CPersonalBankerTransfer *pbt = new CPersonalBankerTransfer(CUtilities::GetTickCount(), MEMORY_NONE);

			ASSERT_NE(pbt, nullptr) << L"Constructor returned null.";
			CCard *card = new CCard(CUtilities::GetCurrentTime(),CUtilities::GetUTCTime());
			card->SetCardID(std::string("A531000002"));

			// copy constructor
			int cardIdLength = 0;
			const int PBTAMOUNT = 1000;
			const byte *cardID = card->GetCardID(cardIdLength);

			
			pbt->SetAmount(PBTAMOUNT);
			pbt->SetRestrictedAmount(PBTAMOUNT);
			pbt->SetNonrestrictedAmount(PBTAMOUNT);
			pbt->SetPersonalBankerType(CashDownload);
			pbt->SetPromoPersonalBankerTransferType(CashToGame);
			pbt->SetCardNumber(cardID, cardIdLength);

			CPersonalBankerTransfer *pbtCopy = new CPersonalBankerTransfer(*pbt);
			ASSERT_NE(pbtCopy, nullptr) << L"Copy constructor returned NULL.";
			ASSERT_TRUE(pbt->GetAmount() == pbtCopy->GetAmount())  << L"pbt amounts differ";
			ASSERT_TRUE(pbt->GetRestrictedAmount() == pbtCopy->GetRestrictedAmount()) << L"pbt restricted amounts differ";
			ASSERT_TRUE(pbt->GetNonrestrictedAmount() == pbtCopy->GetNonrestrictedAmount()) << L"pbt nonrestricted amounts differ";
			ASSERT_TRUE(pbt->GetPersonalBankerType() == pbtCopy->GetPersonalBankerType()) << L"pbt types differ";
			ASSERT_TRUE(pbt->GetPersonalBankerTransferType() == pbtCopy->GetPersonalBankerTransferType()) << L"pbt transfer types differ";
			ASSERT_TRUE(pbt->GetCardNumberLen() == pbtCopy->GetCardNumberLen()) << L"Card number lengths differ";
			const byte *origCardID = pbt->GetCardNumber(cardIdLength);
			const byte *cpyCardID = pbtCopy->GetCardNumber(cardIdLength);
			ASSERT_TRUE(memcmp(origCardID,cpyCardID, cardIdLength) == 0) << L"Card numbers differ";

			delete pbt;
			delete pbtCopy;
			delete card;

		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBankerTransfer::SetPromoPersonalBankerType()
		///     CPersonalBankerTransfer::SetCancelledType()
		///     CPersonalBankerTransfer::SetPromoCancelledType()
		///     CPersonalBankerTransfer::GetCancelledToken()
		/// </summary>
		TEST_F(PersonalBankerTransferUnitTests,GetCancelledTokenPath1Test)
		{
			// Test data.
			const LPCTSTR cancelledTokenExpected = PB_CANCELLATION_TOKEN[_Unknown];

			// Setup.
			CPersonalBankerTransfer varPbt(CUtilities::GetTickCount());
			varPbt.SetPromoPersonalBankerType(PromoCreditsUpload);
			varPbt.SetPromoCancelledType(NoGameDenomination);
			varPbt.SetCancelledType(DoorOpen);
			varPbt.SetCancelledType(_Unknown);
			const CPersonalBankerTransfer * const pbt = const_cast<const CPersonalBankerTransfer *>(&varPbt);
			
			// Test #1.
			{
				const LPCTSTR cancelledTokenResult = pbt->GetCancelledToken();

				bool areEqual = cancelledTokenResult ==  cancelledTokenExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED cancelledTokenResult=\"%s\" expected=\"%s\" in %s"),
					static_cast<LPCTSTR>(cancelledTokenResult),
					static_cast<LPCTSTR>(cancelledTokenExpected),
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBankerTransfer::SetCancelledType()
		///     CPersonalBankerTransfer::SetPromoCancelledType()
		///     CPersonalBankerTransfer::SetPersonalBankerTransferType()
		///     CPersonalBankerTransfer::GetCancelledToken()
		/// </summary>
		TEST_F(PersonalBankerTransferUnitTests,GetCancelledTokenPath2Test)
		{
			// Test data.
			const LPCTSTR cancelledTokenExpected = PB_CANCELLATION_TOKEN[DoorOpen];

			// Setup.
			CPersonalBankerTransfer varPbt(CUtilities::GetTickCount());
			varPbt.SetPromoCancelledType(NoGameDenomination);
			varPbt.SetCancelledType(DoorOpen);
			varPbt.SetPersonalBankerTransferType(Undeclared);
			const CPersonalBankerTransfer * const pbt = const_cast<const CPersonalBankerTransfer *>(&varPbt);
			
			// Test #1.
			{
				const LPCTSTR cancelledTokenResult = pbt->GetCancelledToken();

				bool areEqual = cancelledTokenResult ==  cancelledTokenExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED cancelledTokenResult=\"%s\" expected=\"%s\" in %s"),
					static_cast<LPCTSTR>(cancelledTokenResult),
					static_cast<LPCTSTR>(cancelledTokenExpected),
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBankerTransfer::SetCancelledType()
		///     CPersonalBankerTransfer::SetPromoCancelledType()
		///     CPersonalBankerTransfer::SetPersonalBankerTransferType()
		///     CPersonalBankerTransfer::GetCancelledToken()
		/// </summary>
		TEST_F(PersonalBankerTransferUnitTests,GetCancelledTokenPath3Test)
		{
			// Test data.
			const LPCTSTR cancelledTokenExpected = PB_CANCELLATION_TOKEN[NoGameDenomination];

			// Setup.
			CPersonalBankerTransfer varPbt(CUtilities::GetTickCount());
			varPbt.SetPromoCancelledType(NoGameDenomination);
			varPbt.SetCancelledType(_Unknown);
			varPbt.SetPersonalBankerTransferType(Undeclared);
			const CPersonalBankerTransfer * const pbt = const_cast<const CPersonalBankerTransfer *>(&varPbt);
			
			// Test #1.
			{
				const LPCTSTR cancelledTokenResult = pbt->GetCancelledToken();

				bool areEqual = cancelledTokenResult ==  cancelledTokenExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED cancelledTokenResult=\"%s\" expected=\"%s\" in %s"),
					static_cast<LPCTSTR>(cancelledTokenResult),
					static_cast<LPCTSTR>(cancelledTokenExpected),
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBankerTransfer::SetCancelledType()
		///     CPersonalBankerTransfer::SetPromoCancelledType()
		///     CPersonalBankerTransfer::SetPersonalBankerTransferType()
		///     CPersonalBankerTransfer::GetCancelledToken()
		/// <remarks>
		/// If PollerCancellationType is an unsigned enumeration type then this tests the same path as GetCancelledTokenPath5Test.
		/// </remarks>
		/// </summary>
		TEST_F(PersonalBankerTransferUnitTests,GetCancelledTokenPath4Test)
		{
			// Test data.
			const LPCTSTR cancelledTokenExpected = PB_CANCELLATION_TOKEN[_Unknown];

			// Setup.
			CPersonalBankerTransfer varPbt(CUtilities::GetTickCount());
			varPbt.SetPromoPersonalBankerType(PromoCreditsUpload);
			varPbt.SetPromoCancelledType(NoGameDenomination);
			varPbt.SetCancelledType(DoorOpen);
			varPbt.SetCancelledType((PollerCancellationType)-1);
			const CPersonalBankerTransfer * const pbt = const_cast<const CPersonalBankerTransfer *>(&varPbt);
			
			// Test #1.
			{
				const LPCTSTR cancelledTokenResult = pbt->GetCancelledToken();

				bool areEqual = cancelledTokenResult ==  cancelledTokenExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED cancelledTokenResult=\"%s\" expected=\"%s\" in %s"),
					static_cast<LPCTSTR>(cancelledTokenResult),
					static_cast<LPCTSTR>(cancelledTokenExpected),
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBankerTransfer::SetCancelledType()
		///     CPersonalBankerTransfer::SetPromoCancelledType()
		///     CPersonalBankerTransfer::SetPersonalBankerTransferType()
		///     CPersonalBankerTransfer::GetCancelledToken()
		/// </summary>
		TEST_F(PersonalBankerTransferUnitTests,GetCancelledTokenPath5Test)
		{
			// Test data.
			const LPCTSTR cancelledTokenExpected = PB_CANCELLATION_TOKEN[_Unknown];

			// Setup.
			CPersonalBankerTransfer varPbt(CUtilities::GetTickCount());
			varPbt.SetPromoCancelledType(NoGameDenomination);
			varPbt.SetCancelledType((PollerCancellationType)_countof(PB_CANCELLATION_TOKEN));
			varPbt.SetPersonalBankerTransferType(Undeclared);
			const CPersonalBankerTransfer * const pbt = const_cast<const CPersonalBankerTransfer *>(&varPbt);
			
			// Test #1.
			{
				const LPCTSTR cancelledTokenResult = pbt->GetCancelledToken();

				bool areEqual = cancelledTokenResult ==  cancelledTokenExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED cancelledTokenResult=\"%s\" expected=\"%s\" in %s"),
					static_cast<LPCTSTR>(cancelledTokenResult),
					static_cast<LPCTSTR>(cancelledTokenExpected),
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}
		}


		/// <summary>
		/// Test the implementation of:
		///     CPersonalBankerTransfer::SetCancelledType()
		///     CPersonalBankerTransfer::GetPersonalBankerType()
		/// </summary>
		TEST_F(PersonalBankerTransferUnitTests,GetPersonalBankerTypeTest)
		{
			typedef struct
			{
				// Inputs.
				const PersonalBankerType m_type;

				// Expected results.
				const PersonalBankerType m_personalBankerTypeExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ Unspecified, Unspecified },
				{ SessionRequest, SessionRequest },
				{ CashDownload, CashDownload },
				{ PromoCreditsToGame, PromoCreditsToGame },
				{ PromoCreditsToCash, PromoCreditsToCash },
				{ PointRedemptionToCash, PointRedemptionToCash },
				{ CashUpload, CashUpload },
				{ TransferCreditsFromGame, TransferCreditsFromGame },
				{ SessionRelease, SessionRelease },
				{ ChangePinAtSentinel, ChangePinAtSentinel },
				{ PointRedemptionToPromoCredits, PointRedemptionToPromoCredits },
				{ PointCashUpload, PointCashUpload },
				{ PromoCreditsUpload, PromoCreditsUpload },
				{ PromoCreditsUploadAuto, PromoCreditsUploadAuto },
				{ PendingResponse, PendingResponse },
				{ CanceledSessionRequest, CanceledSessionRequest },
				{ CanceledCashDownload, CanceledCashDownload },
				{ CanceledTransferCreditsFromGame, CanceledTransferCreditsFromGame },
				{ CanceledSessionRelease, CanceledSessionRelease },
				{ CanceledChangePinAtSentinel, CanceledChangePinAtSentinel },
				{ CanceledPointRedemptionToPromoCredits, CanceledPointRedemptionToPromoCredits },
				{ CanceledPointCashUpload, CanceledPointCashUpload },
				{ CanceledPromoCreditsUpload, CanceledPromoCreditsUpload },
				{ CanceledPromoCreditsUploadAuto, CanceledPromoCreditsUploadAuto },
				{ AuthorizeCashToGame, AuthorizeCashToGame },
				{ AuthorizePromoToGame, AuthorizePromoToGame },
				{ AuthorizePointsToGameCash, AuthorizePointsToGameCash },
				{ AuthorizePointsToGamePromo, AuthorizePointsToGamePromo },
				{ UnknownCreditSource, UnknownCreditSource },
				{ InvalidTransIDFromSystem, InvalidTransIDFromSystem },
				{ NonMatchingTransactionID, NonMatchingTransactionID },
				{ UnexpectedRestrictedToGame, UnexpectedRestrictedToGame },
				{ UnexpectedUnRestrictedToGame, UnexpectedUnRestrictedToGame },
				{ UnexpectedCashToGame, UnexpectedCashToGame },
				{ UnexpectedRestrictedFromGame, UnexpectedRestrictedFromGame },
				{ UnexpectedRestrictedFromGame, UnexpectedRestrictedFromGame },
				{ UnexpectedUnRestrictedFromGame, UnexpectedUnRestrictedFromGame },
				{ UnexpectedCashFromGame, UnexpectedCashFromGame },
				{ UnexpectedRestrictedToGameErr, UnexpectedRestrictedToGameErr },
				{ UnexpectedUnrestrictedToGameErr, UnexpectedUnrestrictedToGameErr },
				{ UnexpectedCashToGameErr, UnexpectedCashToGameErr },
				{ UnexpectedUnrestrictedFromGameErr, UnexpectedUnrestrictedFromGameErr },
				{ UnexpectedCashFromGameErr, UnexpectedCashFromGameErr },
				{ UnknownError, UnknownError },
				{ (PersonalBankerType)-2, (PersonalBankerType)-2 },
				{ (PersonalBankerType)-1, (PersonalBankerType)-1 },
				{ (PersonalBankerType)0, (PersonalBankerType)0 },
				{ (PersonalBankerType)-1, (PersonalBankerType)-1 },
				{ (PersonalBankerType)2, (PersonalBankerType)2 },
				{ (PersonalBankerType)99, (PersonalBankerType)99 },
				{ (PersonalBankerType)0x7e, (PersonalBankerType)0x7e },
				{ (PersonalBankerType)0x7f, (PersonalBankerType)0x7f },
				{ (PersonalBankerType)0x80, (PersonalBankerType)0x80 },
				{ (PersonalBankerType)0x81, (PersonalBankerType)0x81 },
				{ (PersonalBankerType)199, (PersonalBankerType)199 },
				{ (PersonalBankerType)199, (PersonalBankerType)199 },
				{ (PersonalBankerType)0xfe, (PersonalBankerType)0xfe },
				{ (PersonalBankerType)0xff, (PersonalBankerType)0xff },
				{ (PersonalBankerType)0x100, (PersonalBankerType)0x100 },
				{ (PersonalBankerType)0x101, (PersonalBankerType)0x101 },
				{ (PersonalBankerType)0xfe, (PersonalBankerType)0xfe },
				{ (PersonalBankerType)0xff, (PersonalBankerType)0xff },

				{ (PersonalBankerType)(INT_MAX - 1), (PersonalBankerType)(INT_MAX - 1) },
				{ (PersonalBankerType)INT_MAX, (PersonalBankerType)INT_MAX },
				{ (PersonalBankerType)INT_MIN, (PersonalBankerType)INT_MIN },
				{ (PersonalBankerType)(INT_MIN + 1), (PersonalBankerType)(INT_MIN + 1) },
				{ (PersonalBankerType)(UINT_MAX - 2), (PersonalBankerType)(UINT_MAX - 2) },
				{ (PersonalBankerType)(UINT_MAX - 1), (PersonalBankerType)(UINT_MAX - 1) },
				{ (PersonalBankerType)UINT_MAX, (PersonalBankerType)UINT_MAX },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CPersonalBankerTransfer varPbt(CUtilities::GetTickCount());
				varPbt.SetPersonalBankerType(TEST_DATA[testIndex].m_type);
				const CPersonalBankerTransfer * const pbt = const_cast<const CPersonalBankerTransfer *>(&varPbt);
			
				// Test #1.
				{
					const PersonalBankerType personalBankerTypeResult = pbt->GetPersonalBankerType();

					bool areEqual = personalBankerTypeResult ==  TEST_DATA[testIndex].m_personalBankerTypeExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED personalBankerTypeResult=%d expected=%d testIndex=%d in %s"),
						(int)personalBankerTypeResult,
						(int)TEST_DATA[testIndex].m_personalBankerTypeExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBankerTransfer::SetCancelledType()
		///     CPersonalBankerTransfer::SetPromoCancelledType()
		///     CPersonalBankerTransfer::SetPersonalBankerTransferType()
		///     CPersonalBankerTransfer::GetCancelledToken()
		/// </summary>
		TEST_F(PersonalBankerTransferUnitTests,GetCancelledTokenTest)
		{
			typedef struct
			{
				// Inputs.
				const PollerCancellationType m_type;

				// Expected results.
				const LPCTSTR m_cancelledTokenExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ _Unknown, PB_CANCELLATION_TOKEN[_Unknown]},
				{ NoGameDenomination, PB_CANCELLATION_TOKEN[NoGameDenomination]},
				{ DoorOpen, PB_CANCELLATION_TOKEN[DoorOpen]},
				{ UnderTransferLimit, PB_CANCELLATION_TOKEN[UnderTransferLimit]},
				{ OverTransferLimit, PB_CANCELLATION_TOKEN[OverTransferLimit]},
				{ SASInvalidTransactionNum, PB_CANCELLATION_TOKEN[SASInvalidTransactionNum]},
				{ NonCreditMode, PB_CANCELLATION_TOKEN[NonCreditMode]},
				{ MachineTilt, PB_CANCELLATION_TOKEN[MachineTilt]},
				{ SASInvalidAckMessage, PB_CANCELLATION_TOKEN[SASInvalidAckMessage]},
				{ GameInProgress, PB_CANCELLATION_TOKEN[GameInProgress]},
				{ SASNonBCDDataError, PB_CANCELLATION_TOKEN[SASNonBCDDataError]},
				{ SASDuplicateTransferNum, PB_CANCELLATION_TOKEN[SASDuplicateTransferNum]},
				{ MachineOutOfService, PB_CANCELLATION_TOKEN[MachineOutOfService]},
				{ SASMachineBusy, PB_CANCELLATION_TOKEN[SASMachineBusy]},
				{ Unknown15, PB_CANCELLATION_TOKEN[Unknown15]},
				{ SASTimeout, PB_CANCELLATION_TOKEN[SASTimeout]},
				{ BartGameDenomMismatch, PB_CANCELLATION_TOKEN[BartGameDenomMismatch]},
				{ LockReject, PB_CANCELLATION_TOKEN[LockReject]},
				{ CanceledBySystem, PB_CANCELLATION_TOKEN[CanceledBySystem]},
				{ CanceledBySystem, PB_CANCELLATION_TOKEN[CanceledBySystem]},
				{ PartialTransfer, PB_CANCELLATION_TOKEN[PartialTransfer]},
				{ GameNotRegistered, PB_CANCELLATION_TOKEN[GameNotRegistered]},
				{ MismatchedRegistrationNum, PB_CANCELLATION_TOKEN[MismatchedRegistrationNum]},
				{ NoCredits, PB_CANCELLATION_TOKEN[NoCredits]},
				{ TicketReceiptError, PB_CANCELLATION_TOKEN[TicketReceiptError]},
				{ GameNotLocked, PB_CANCELLATION_TOKEN[GameNotLocked]},
				{ GameAssetNumError, PB_CANCELLATION_TOKEN[GameAssetNumError]},
				{ InvalidTransCode, PB_CANCELLATION_TOKEN[InvalidTransCode]},
				{ AmountNotEvenMult, PB_CANCELLATION_TOKEN[AmountNotEvenMult]},
				{ NoPosID, PB_CANCELLATION_TOKEN[NoPosID]},
				{ RestrictedPoolIDMismatch, PB_CANCELLATION_TOKEN[RestrictedPoolIDMismatch]},
				{ GeneralError, PB_CANCELLATION_TOKEN[GeneralError]},
				{ InvalidTransID, PB_CANCELLATION_TOKEN[InvalidTransID]},
				{ GameCommDown, PB_CANCELLATION_TOKEN[GameCommDown]},
				{ AllowableRangeExceeded, PB_CANCELLATION_TOKEN[AllowableRangeExceeded]},
				{ MismatchTransID, PB_CANCELLATION_TOKEN[MismatchTransID]},
				{ IncompatiblePoll, PB_CANCELLATION_TOKEN[IncompatiblePoll]},
				{ UnsupportedTransCode, PB_CANCELLATION_TOKEN[UnsupportedTransCode]},
				{ TransInfoUnavailable, PB_CANCELLATION_TOKEN[TransInfoUnavailable]},
				{ AutomaticUploadCanceledBySystem, PB_CANCELLATION_TOKEN[AutomaticUploadCanceledBySystem]},
				{ AutomaticUploadCanceledByUser, PB_CANCELLATION_TOKEN[AutomaticUploadCanceledByUser]},
				{ (PollerCancellationType)-2, PB_CANCELLATION_TOKEN[_Unknown]},
				{ (PollerCancellationType)-1, PB_CANCELLATION_TOKEN[_Unknown]},
				{ (PollerCancellationType)_countof(PB_CANCELLATION_TOKEN), PB_CANCELLATION_TOKEN[_Unknown]},
				{ (PollerCancellationType)(_countof(PB_CANCELLATION_TOKEN) + 1), PB_CANCELLATION_TOKEN[_Unknown]},
				{ (PollerCancellationType)_countof(PB_CANCELLATION_TOKEN), PB_CANCELLATION_TOKEN[_Unknown]},
				{ (PollerCancellationType)(_countof(PB_CANCELLATION_TOKEN) + 1), PB_CANCELLATION_TOKEN[_Unknown]},
				{ (PollerCancellationType)(INT_MAX - 1), PB_CANCELLATION_TOKEN[_Unknown]},
				{ (PollerCancellationType)INT_MAX, PB_CANCELLATION_TOKEN[_Unknown]},
				{ (PollerCancellationType)INT_MIN, PB_CANCELLATION_TOKEN[_Unknown]},
				{ (PollerCancellationType)(INT_MIN + 1), PB_CANCELLATION_TOKEN[_Unknown]},
				{ (PollerCancellationType)INT_MIN, PB_CANCELLATION_TOKEN[_Unknown]},
				{ (PollerCancellationType)(UINT_MAX - 2), PB_CANCELLATION_TOKEN[_Unknown]},
				{ (PollerCancellationType)(UINT_MAX - 1), PB_CANCELLATION_TOKEN[_Unknown]},
				{ (PollerCancellationType)UINT_MAX, PB_CANCELLATION_TOKEN[_Unknown]},
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CPersonalBankerTransfer varPbt(CUtilities::GetTickCount());
				varPbt.SetPromoCancelledType(TEST_DATA[testIndex].m_type);
				varPbt.SetCancelledType(_Unknown);
				varPbt.SetPersonalBankerTransferType(Undeclared);
				const CPersonalBankerTransfer * const pbt = const_cast<const CPersonalBankerTransfer *>(&varPbt);
			
				// Test #1.
				{
					const LPCTSTR cancelledTokenResult = pbt->GetCancelledToken();

					bool areEqual = cancelledTokenResult ==  TEST_DATA[testIndex].m_cancelledTokenExpected;
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED cancelledTokenResult=\"%s\" expected=\"%s\" testIndex=%d in %s"),
						static_cast<LPCTSTR>(cancelledTokenResult),
						static_cast<LPCTSTR>(TEST_DATA[testIndex].m_cancelledTokenExpected),
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBankerTransfer::SetPIN()
		///     CPersonalBankerTransfer::IsAutoUploadPIN()
		/// </summary>
		TEST_F(PersonalBankerTransferUnitTests,IsAutoUploadPINFalseTest)
		{
			// Test data.
			const bool isAutoUploadPINExpected = false;

			// Setup.
			CPersonalBankerTransfer varPbt(CUtilities::GetTickCount());
			varPbt.SetPIN(~PBT_AUTO_UPLOAD_PIN);
			const CPersonalBankerTransfer * const pbt = const_cast<const CPersonalBankerTransfer *>(&varPbt);
			
			// Test #1.
			{
				bool isAutoUploadPINResult = pbt->IsAutoUploadPIN();

				bool areEqual = isAutoUploadPINResult ==  isAutoUploadPINExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED isAutoUploadPINResult=%d expected=%d in %s"),
					(int)isAutoUploadPINResult,
					(int)isAutoUploadPINExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBankerTransfer::SetPIN()
		///     CPersonalBankerTransfer::IsAutoUploadPIN()
		/// </summary>
		TEST_F(PersonalBankerTransferUnitTests,IsAutoUploadPINTrueTest)
		{
			// Test data.
			const bool isAutoUploadPINExpected = true;

			// Setup.
			CPersonalBankerTransfer varPbt(CUtilities::GetTickCount());
			varPbt.SetPIN(PBT_AUTO_UPLOAD_PIN);
			const CPersonalBankerTransfer * const pbt = const_cast<const CPersonalBankerTransfer *>(&varPbt);
			
			// Test #1.
			{
				bool isAutoUploadPINResult = pbt->IsAutoUploadPIN();

				bool areEqual = isAutoUploadPINResult ==  isAutoUploadPINExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED isAutoUploadPINResult=%d expected=%d in %s"),
					(int)isAutoUploadPINResult,
					(int)isAutoUploadPINExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBankerTransfer::SetPersonalBankerType()
		///     CPersonalBankerTransfer::ChangePersonalBankerTypeToAuthorization()
		/// </summary>
		TEST_F(PersonalBankerTransferUnitTests,ChangePersonalBankerTypeToAuthorizationTest)
		{
			// Test data.
			std::string failedMsg;

			// Setup.
			CPersonalBankerTransfer varPbt(CUtilities::GetTickCount());
			
			// Test #1.
			{
				varPbt.SetPersonalBankerType(CashDownload);
				const PersonalBankerType expectedPersonalBankerType(AuthorizeCashToGame);
				varPbt.ChangePersonalBankerTypeToAuthorization();
				bool areEqual = varPbt.GetPersonalBankerType() ==  expectedPersonalBankerType;				
				failedMsg = FormatString(_T("FAILED PersonalBankerType=%d expected=%d in %s"),
					(int)varPbt.GetPersonalBankerType(),
					(int)expectedPersonalBankerType,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #2.
			{
				varPbt.SetPersonalBankerType(PromoCreditsToCash);
				const PersonalBankerType expectedPersonalBankerType(AuthorizePromoToGame);				
				varPbt.ChangePersonalBankerTypeToAuthorization();
				bool areEqual = varPbt.GetPersonalBankerType() ==  expectedPersonalBankerType;				
				failedMsg = FormatString(_T("FAILED PersonalBankerType=%d expected=%d in %s"),
					(int)varPbt.GetPersonalBankerType(),
					(int)expectedPersonalBankerType,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #3.
			{
				varPbt.SetPersonalBankerType(PointRedemptionToCash);
				const PersonalBankerType expectedPersonalBankerType(AuthorizePointsToGameCash);				
				varPbt.ChangePersonalBankerTypeToAuthorization();
				bool areEqual = varPbt.GetPersonalBankerType() ==  expectedPersonalBankerType;				
				failedMsg = FormatString(_T("FAILED PersonalBankerType=%d expected=%d in %s"),
					(int)varPbt.GetPersonalBankerType(),
					(int)expectedPersonalBankerType,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #4.
			{
				varPbt.SetPersonalBankerType(PointRedemptionToPromoCredits);
				const PersonalBankerType expectedPersonalBankerType(AuthorizePointsToGamePromo);				
				varPbt.ChangePersonalBankerTypeToAuthorization();
				bool areEqual = varPbt.GetPersonalBankerType() ==  expectedPersonalBankerType;				
				failedMsg = FormatString(_T("FAILED PersonalBankerType=%d expected=%d in %s"),
					(int)varPbt.GetPersonalBankerType(),
					(int)expectedPersonalBankerType,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #5. This test should not change Personal Banker Type because only transfers to game
			// need authorization.
			{
				const PersonalBankerType expectedPersonalBankerType(CashUpload);
				varPbt.SetPersonalBankerType(expectedPersonalBankerType);				
				varPbt.ChangePersonalBankerTypeToAuthorization();
				bool areEqual = varPbt.GetPersonalBankerType() ==  expectedPersonalBankerType;				
				failedMsg = FormatString(_T("FAILED PersonalBankerType=%d expected=%d in %s"),
					(int)varPbt.GetPersonalBankerType(),
					(int)expectedPersonalBankerType,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #6. This test should not change Personal Banker Type because only transfers to game
			// need authorization.
			{
				const PersonalBankerType expectedPersonalBankerType(TransferCreditsFromGame);
				varPbt.SetPersonalBankerType(expectedPersonalBankerType);				
				varPbt.ChangePersonalBankerTypeToAuthorization();
				bool areEqual = varPbt.GetPersonalBankerType() ==  expectedPersonalBankerType;				
				failedMsg = FormatString(_T("FAILED PersonalBankerType=%d expected=%d in %s"),
					(int)varPbt.GetPersonalBankerType(),
					(int)expectedPersonalBankerType,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #7. This test should not change Personal Banker Type because only transfers to game
			// need authorization.
			{
				const PersonalBankerType expectedPersonalBankerType(PointCashUpload);
				varPbt.SetPersonalBankerType(expectedPersonalBankerType);				
				varPbt.ChangePersonalBankerTypeToAuthorization();
				bool areEqual = varPbt.GetPersonalBankerType() ==  expectedPersonalBankerType;				
				failedMsg = FormatString(_T("FAILED PersonalBankerType=%d expected=%d in %s"),
					(int)varPbt.GetPersonalBankerType(),
					(int)expectedPersonalBankerType,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #8. This test should not change Personal Banker Type because only transfers to game
			// need authorization.
			{
				const PersonalBankerType expectedPersonalBankerType(PromoCreditsUpload);
				varPbt.SetPersonalBankerType(expectedPersonalBankerType);				
				varPbt.ChangePersonalBankerTypeToAuthorization();
				bool areEqual = varPbt.GetPersonalBankerType() ==  expectedPersonalBankerType;				
				failedMsg = FormatString(_T("FAILED PersonalBankerType=%d expected=%d in %s"),
					(int)varPbt.GetPersonalBankerType(),
					(int)expectedPersonalBankerType,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #9. This test should not change Personal Banker Type because only transfers to game
			// need authorization.
			{
				const PersonalBankerType expectedPersonalBankerType(PromoCreditsUploadAuto);
				varPbt.SetPersonalBankerType(expectedPersonalBankerType);				
				varPbt.ChangePersonalBankerTypeToAuthorization();
				bool areEqual = varPbt.GetPersonalBankerType() ==  expectedPersonalBankerType;				
				failedMsg = FormatString(_T("FAILED PersonalBankerType=%d expected=%d in %s"),
					(int)varPbt.GetPersonalBankerType(),
					(int)expectedPersonalBankerType,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}
		}

		TEST_F(PersonalBankerTransferUnitTests,PersonalBankerTransferVerifyMaxAmounts)
		{
			CDirectoryManager::MockInstance();
			const int MAXPBTAMOUNT = 300000; // $3,000 in pennies
			CPersonalBankerTransfer *transfer = new CPersonalBankerTransfer(CUtilities::GetTickCount());

			// amounts all valid
			transfer->SetAmount(1000);
			transfer->SetRestrictedAmount(1000);
			transfer->SetNonrestrictedAmount(1000);
			ASSERT_TRUE(transfer->VerifyMaxAmounts());

			//// invalid amount
			transfer->SetAmount(MAXPBTAMOUNT + 1);
			ASSERT_FALSE(transfer->VerifyMaxAmounts());

			//// invalid restricted amount
			transfer->SetAmount(1000);
			transfer->SetRestrictedAmount(MAXPBTAMOUNT + 1);
			ASSERT_FALSE(transfer->VerifyMaxAmounts());

			//// invalid nonrestricted amount
			transfer->SetRestrictedAmount(1000);
			transfer->SetNonrestrictedAmount(MAXPBTAMOUNT + 1);
			ASSERT_FALSE(transfer->VerifyMaxAmounts());

			//// amounts all maximum valid
			transfer->SetAmount(MAXPBTAMOUNT);
			transfer->SetRestrictedAmount(MAXPBTAMOUNT);
			transfer->SetNonrestrictedAmount(MAXPBTAMOUNT);
			ASSERT_TRUE(transfer->VerifyMaxAmounts());

			delete transfer;

		}

		TEST_F(PersonalBankerTransferUnitTests,PersonalBankerTransferVerifyID)
		{
			CPersonalBankerTransfer pbt(CUtilities::GetTickCount());
			const char *pbtID = "01001123456123456789";
			ASSERT_TRUE(CPersonalBankerTransfer::IsValidID(123456, pbtID, strlen(pbtID)));

			// Too short
			const char *pbtID2 = "01001123456";
			ASSERT_FALSE(CPersonalBankerTransfer::IsValidID(123456, pbtID2, strlen(pbtID2)));

			// Incorrect Slotmast
			const char *pbtID3 = "01001223456123456789";
			ASSERT_FALSE(CPersonalBankerTransfer::IsValidID(123456, pbtID3, strlen(pbtID3)));

			// Incorrect Invalid characters
			const char *pbtID4 = "0100112345H123456789";
			ASSERT_FALSE(CPersonalBankerTransfer::IsValidID(12345, pbtID4, strlen(pbtID3)));

			const char *pbtID5 = "01001.12345123456789";
			ASSERT_FALSE(CPersonalBankerTransfer::IsValidID(12345, pbtID5, strlen(pbtID3)));
		}

		TEST_F(PersonalBankerTransferUnitTests,PersonalBankerTransferVerifyID2)
		{
			// Normal tests.
			ASSERT_TRUE(CPersonalBankerTransfer::IsValidID(123, "abcde000123abcdefghi", MAX_PBID_LENGTH));
			ASSERT_TRUE(CPersonalBankerTransfer::IsValidID(123456, "abcde123456abcdefghi", MAX_PBID_LENGTH));
			ASSERT_FALSE(CPersonalBankerTransfer::IsValidID(123457, "AT000123456000000000", MAX_PBID_LENGTH));

			// Adverse tests.
			ASSERT_FALSE(CPersonalBankerTransfer::IsValidID(1234567, "AT000123456000000000", MAX_PBID_LENGTH)); // A 7-digit slotmastID is invalid.
			ASSERT_FALSE(CPersonalBankerTransfer::IsValidID(1234567, "AT000123456700000000", MAX_PBID_LENGTH)); // A 7-digit slotmastID is invalid.
			ASSERT_FALSE(CPersonalBankerTransfer::IsValidID(1234567, "AT000234567000000000", MAX_PBID_LENGTH)); // A 7-digit slotmastID is invalid.
			ASSERT_FALSE(CPersonalBankerTransfer::IsValidID(123, "AT000123@@@000000000", MAX_PBID_LENGTH));
			ASSERT_FALSE(CPersonalBankerTransfer::IsValidID(123, "AT000 +123@000000000", MAX_PBID_LENGTH));
			ASSERT_FALSE(CPersonalBankerTransfer::IsValidID(123, "AT000123\0" "45000000000", MAX_PBID_LENGTH));
			ASSERT_FALSE(CPersonalBankerTransfer::IsValidID(123, "AT000123O45000000000", MAX_PBID_LENGTH));

			ASSERT_TRUE(CPersonalBankerTransfer::IsValidID(-123, "abcde-00123abcdefghi", MAX_PBID_LENGTH));
			ASSERT_FALSE(CPersonalBankerTransfer::IsValidID(-123, "AT000  -123000000000", MAX_PBID_LENGTH));
			ASSERT_FALSE(CPersonalBankerTransfer::IsValidID(-123, "AT000 -123@000000000", MAX_PBID_LENGTH));
			ASSERT_FALSE(CPersonalBankerTransfer::IsValidID(-123, "AT000-123@@000000000", MAX_PBID_LENGTH));
			ASSERT_FALSE(CPersonalBankerTransfer::IsValidID(-123, "AT000-123\0" "4000000000", MAX_PBID_LENGTH));
			ASSERT_FALSE(CPersonalBankerTransfer::IsValidID(-123, "AT000-123O4000000000", MAX_PBID_LENGTH));
		}

		TEST_F(PersonalBankerTransferUnitTests, PersonalBankerTransferOpEqCardNumber)
		{
			CPersonalBankerTransfer pbt(CUtilities::GetTickCount());

			byte cardNum[CARD_ID_LEN]{ 1, 2, 3, 4, 5 };
			pbt.SetCardNumber(cardNum, CARD_ID_LEN);

			CPersonalBankerTransfer cpyPbt(CUtilities::GetTickCount());
			cpyPbt = pbt;

			// Test card number
			int cardLen = 5;
			const byte *newCardNumber = cpyPbt.GetCardNumber(cardLen);
			ASSERT_FALSE(memcmp(cardNum, newCardNumber, CARD_ID_LEN));

		}

		TEST_F(PersonalBankerTransferUnitTests, PersonalBankerTransferOpEqZeroCardNumber)
		{
			CPersonalBankerTransfer pbt(CUtilities::GetTickCount());

			byte cardNum[CARD_ID_LEN]{ 0, 0, 0, 0, 0 };
			pbt.SetCardNumber(cardNum, CARD_ID_LEN);

			CPersonalBankerTransfer newPbt(CUtilities::GetTickCount());
			byte newCardNum[CARD_ID_LEN]{ 1, 2, 3, 4, 5 };
			newPbt.SetCardNumber(newCardNum, CARD_ID_LEN);

			// Assign pbt to newPbt
			newPbt = pbt;

			// Test card number didn't get overwritten with 0s
			int cardLen = 5;
			const byte *getCardNumber = newPbt.GetCardNumber(cardLen);
			ASSERT_FALSE(memcmp(newCardNum, getCardNumber, CARD_ID_LEN));

		}

		TEST_F(PersonalBankerTransferUnitTests, CashoutErrorFlags_Get_Set_Test)
		{
			CPersonalBankerTransfer pbt(CUtilities::GetTickCount());

			WORD cashoutErrorFlags = pbt.GetCashoutErrorFlags();
			WORD cashoutErrorFlagsExpected = 0;
			ASSERT_EQ(cashoutErrorFlags, cashoutErrorFlagsExpected) << "Default value is incorrect.";

			pbt.AppendCashoutErrorFlags(CashoutError::CASHOUT_ERROR_CASH_FUNCTIONALITY_UNAVAILABLE);
			cashoutErrorFlags = pbt.GetCashoutErrorFlags();
			cashoutErrorFlagsExpected = cashoutErrorFlagsExpected | CashoutError::CASHOUT_ERROR_CASH_FUNCTIONALITY_UNAVAILABLE;
			ASSERT_EQ(cashoutErrorFlags, cashoutErrorFlagsExpected) << "Incorrect value for CASHOUT_ERROR_CASH_FUNCTIONALITY_UNAVAILABLE append.";

			pbt.AppendCashoutErrorFlags(CashoutError::CASHOUT_ERROR_PROMO_OVER_TRANSFER_LIMIT);
			cashoutErrorFlags = pbt.GetCashoutErrorFlags();
			cashoutErrorFlagsExpected = cashoutErrorFlagsExpected | CashoutError::CASHOUT_ERROR_PROMO_OVER_TRANSFER_LIMIT;
			ASSERT_EQ(cashoutErrorFlags, cashoutErrorFlagsExpected)	<< "Incorrect value for CASHOUT_ERROR_PROMO_OVER_TRANSFER_LIMIT append.";
		}

		TEST_F(PersonalBankerTransferUnitTests, AutoUploadErrorFlags_Get_Set_Test)
		{
			CPersonalBankerTransfer pbt(CUtilities::GetTickCount());

			WORD autoUploadErrorFlags = pbt.GetAutoUploadErrorFlags();
			WORD autoUploadErrorFlagsExpected = 0;
			ASSERT_EQ(autoUploadErrorFlags, autoUploadErrorFlagsExpected) << "Default value is incorrect.";

			pbt.AppendAutoUploadErrorFlags(AutoUploadError::AUTOUPLOAD_ERROR_CASH_FUNCTIONALITY_UNAVAILABLE);
			autoUploadErrorFlags = pbt.GetAutoUploadErrorFlags();
			autoUploadErrorFlagsExpected = autoUploadErrorFlagsExpected | AutoUploadError::AUTOUPLOAD_ERROR_CASH_FUNCTIONALITY_UNAVAILABLE;
			ASSERT_EQ(autoUploadErrorFlags, autoUploadErrorFlagsExpected) << "Incorrect value for AUTOUPLOAD_ERROR_CASH_FUNCTIONALITY_UNAVAILABLE append.";

			pbt.AppendAutoUploadErrorFlags(AutoUploadError::AUTOUPLOAD_ERROR_CASH_OVER_TRANSFER_LIMIT);
			autoUploadErrorFlags = pbt.GetAutoUploadErrorFlags();
			autoUploadErrorFlagsExpected = autoUploadErrorFlagsExpected | AutoUploadError::AUTOUPLOAD_ERROR_CASH_OVER_TRANSFER_LIMIT;
			ASSERT_EQ(autoUploadErrorFlags, autoUploadErrorFlagsExpected) << "Incorrect value for AUTOUPLOAD_ERROR_CASH_OVER_TRANSFER_LIMIT append.";
		}