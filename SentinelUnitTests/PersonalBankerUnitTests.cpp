
#include "stdafx.h"

#include "PersonalBanker.h"
#include "DirectoryManager.h"

// Unit testing includes.
#include "UnitTestingUtils.h"
#include "gtest/gtest.h"

namespace SentinelUnitTests
{
	class PersonalBankerUnitTests : public ::testing::Test
	{
	public:
		// Expected default and non-default values.
		static const byte m_nvramDefault = 0x55; // Typical default value for Braemac standard units from factory.
		static const PersonalBankerState m_stateDefault = PBStateUnknown;
		static const bool m_pbtAuthorizationInProcessDefault = false;

		static const PersonalBankerState m_stateNonDefault = (PersonalBankerState)(m_stateDefault + 5);
		static const bool m_pbtAuthorizationInProcessNonDefault = !m_pbtAuthorizationInProcessDefault;
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
		/// <summary>
		/// Helper method to persist a non-default CPersonalBanker instance.
		/// </summary>
		void CPersonalBanker_PersistNonDefault()
		{
			// Setup.
			CUnitTestingUtils::SetNvramValidAndUpgraded();
			CPersonalBanker obj(MEMORY_NVRAM);
			obj.SetState(m_stateNonDefault);
			obj.SetPbtAuthorizationInProcess(m_pbtAuthorizationInProcessNonDefault);
		}
		void TestDefaultConstructor(CPersonalBanker& personalBanker)
		{
			// Test #1
			CPersonalBankerInfo* pbBankerInfo = personalBanker.GetInfoCopy();
			ASSERT_EQ(pbBankerInfo, nullptr) << L"Info is not NULL, probably corrupted";
			delete pbBankerInfo;

			// Test #2
			ASSERT_FALSE(personalBanker.IsPbtInProgress()) << L"IsPbtInProgress not 'false'";

			// Test #3
			ASSERT_FALSE(personalBanker.IsWaitingForTransferToBeCompleted()) << L"IsWaitingForTransferToBeCompleted not 'false'";

			// Test #4
			ASSERT_FALSE(personalBanker.IsWaitingForDenomCheck()) << L"IsWaitingForDenomCheck not 'false'";

			// Test #5
			ASSERT_TRUE(personalBanker.GetAction() == PBActionNone) << L"Action not set to PBActionNone";

			// Test #6
			ASSERT_TRUE(personalBanker.GetState() == PBStateUnknown) << L"State not set to PBStateUnknown";

			// Test #7
			CPersonalBankerTransfer* pbTransfer = personalBanker.GetTransferCopy();
			ASSERT_EQ(pbTransfer, nullptr) << L"Transfer is not NULL, probably corrupted";
			delete pbTransfer;
		}
		void DoneWithTransfer_AuthorizationInProcessTest();
		void CPersonalBanker_ConfigureOffsets();
		void CPersonalBanker_MemoryDefault();
		void CPersonalBanker_MemoryNone();
		void CPersonalBanker_MemoryNvramNonDefault();
		void CPersonalBanker_MemoryNvramDefault();
		void CPersonalBanker_MemoryInactive();
		void CPersonalBanker_MemoryCleared();
		void GetNvramBuffer();
		void SetFromNvramBuffer1();
        void SetFromNvramBuffer2();  
        void CreateInfo(CPersonalBanker &personalBanker);                                                                                                            

	};

	TEST_F(PersonalBankerUnitTests, TestDefaultConstructor)
	{
		CPersonalBanker obj;
		TestDefaultConstructor(obj);
	}

	TEST_F(PersonalBankerUnitTests, ProcessInitialPersonalBankerUnitTest)
	{
		// Test data.
		const PersonalBankerAction actionExpected(PBActionNone);

		// Setup.
		CPersonalBanker varPersonalBanker;
		CConfig config(false, "");

		// Perform operations to be tested.
		varPersonalBanker.ProcessInitialPersonalBanker(config);
		CPersonalBanker const * const personalBanker = &varPersonalBanker;

		// Test #1.
		const PersonalBankerAction actionResult(personalBanker->GetAction());
		{
			// Report #1.
			bool areEqual = actionResult == actionExpected;
			std::string failedMsg;
			failedMsg = FormatString(_T("FAILED actionResult=%d expected=%d in %s"),
				(int)actionResult,
				(int)actionExpected,
				static_cast<LPCTSTR>(__TFUNCTION__));
			ASSERT_TRUE(areEqual) << failedMsg;
		}
	}

	TEST_F(PersonalBankerUnitTests, PersonalBankerGetSetWaitingForTransferToBeCompletedUnitTest)
	{
		CPersonalBanker personalBanker;
		bool flag = personalBanker.IsWaitingForTransferToBeCompleted();
		personalBanker.SetWaitingForTransferToBeCompleted(!flag);
		ASSERT_TRUE(personalBanker.IsWaitingForTransferToBeCompleted() == !flag) << L"Not able to set and get WaitingForTransferToBeCompleted";
	}

	TEST_F(PersonalBankerUnitTests, PersonalBankerGetSetWaitingForDenomCheckUnitTest)
	{
		CPersonalBanker personalBanker;
		bool flag = personalBanker.IsWaitingForDenomCheck();
		personalBanker.SetWaitingForDenomCheck(!flag);
		ASSERT_TRUE(personalBanker.IsWaitingForDenomCheck() == !flag) << L"Not able to set and get WaitingForDenomCheck";
	}

	TEST_F(PersonalBankerUnitTests, PersonalBankerCreateDeleteTransferUnitTest)
	{
		CPersonalBankerTransfer* pbTransfer = NULL;
		CPersonalBanker personalBanker;

		// Test #1
		personalBanker.CreateTransfer(CUtilities::GetTickCount());
		pbTransfer = personalBanker.GetTransferCopy();
		ASSERT_NE(pbTransfer, nullptr) << L"Default Transfer not created";
		ASSERT_TRUE(pbTransfer->GetMemoryType() == MEMORY_NONE) << L"Default Transfer created has wrong memory type";
		if (NULL != pbTransfer)
		{
			delete pbTransfer;
		}

		// Test #2
		personalBanker.CreateTransfer(CUtilities::GetTickCount(), MEMORY_NVRAM, true);
		pbTransfer = personalBanker.GetTransferCopy();
		ASSERT_NE(pbTransfer, nullptr) << L"Non-Default Transfer not created";
		// 2013-05-28 CPersonalBanker pbTransfer GetMemoryType() property is currently inaccessible.  Copies are NOT guaranteed to have the same memory type.
		ASSERT_FALSE(pbTransfer->GetMemoryType() == MEMORY_NVRAM) << L"Non-Default Transfer created has wrong memory type";
		if (NULL != pbTransfer)
		{
			delete pbTransfer;
		}

		// Test #3
		personalBanker.CreateTransfer(CUtilities::GetTickCount(), MEMORY_NVRAM, false);
		pbTransfer = personalBanker.GetTransferCopy();
		ASSERT_NE(pbTransfer, nullptr) << L"Non-Default Transfer not created";
		// 2013-05-28 CPersonalBanker pbTransfer GetMemoryType() property is currently inaccessible.  Copies are NOT guaranteed to have the same memory type.
		ASSERT_FALSE(pbTransfer->GetMemoryType() == MEMORY_NVRAM) << L"Non-Default Transfer created has wrong memory type";
		if (NULL != pbTransfer)
		{
			delete pbTransfer;
		}
	}

	TEST_F(PersonalBankerUnitTests, DoneWithTransferUnitTest)
	{
		// Setup.
		CPersonalBanker varPersonalBanker;
		varPersonalBanker.CreateTransfer(CUtilities::GetTickCount());

		// Perform operations to be tested.
		CPersonalBanker const * const personalBanker = &varPersonalBanker;

		//Tests
		{
			std::string failedMsg;
			failedMsg = FormatString(_T("FAILED CreateTransfer failed in %s"),
				static_cast<LPCTSTR>(__TFUNCTION__));
			ASSERT_TRUE(varPersonalBanker.HasTransfer()) << failedMsg;

			varPersonalBanker.DoneWithTransfer();
			failedMsg = FormatString(_T("FAILED DoneWithTransfer failed in %s"),
				static_cast<LPCTSTR>(__TFUNCTION__));
			ASSERT_FALSE(varPersonalBanker.HasTransfer()) << failedMsg;
		}
	}

	/// <summary>
	/// Functional test for:
	///     CPersonalBanker::DoneWithTransfer()
	/// Verify that DoneWithTransfer() sets m_pbtAuthorizationInProcess=false and persists the false value.
	/// </summary>
	TEST_F(PersonalBankerUnitTests, DoneWithTransfer_AuthorizationInProcessTest)
	{
		DoneWithTransfer_AuthorizationInProcessTest();
	}
	void PersonalBankerUnitTests::DoneWithTransfer_AuthorizationInProcessTest()
	{	// Setup.
		CUnitTestingUtils::SetNvramValidAndUpgraded();

		// Don't delete the first instance until the test completes, because nCompass does not delete the instance.
		CPersonalBanker obj1(MEMORY_NVRAM);
		obj1.CreateTransfer(CUtilities::GetTickCount());
		obj1.SetPbtAuthorizationInProcess(true);
		ASSERT_TRUE(obj1.m_pbtAuthorizationInProcess) << L"m_pbtAuthorizationInProcess setup";

		// Perform operation(s) and test(s).
		obj1.DoneWithTransfer();
		ASSERT_FALSE(obj1.m_pbtAuthorizationInProcess) << L"m_pbtAuthorizationInProcess DoneWithTransfer";

		{
			// Perform operation(s) and test(s).
			CPersonalBanker obj(MEMORY_NVRAM);
			ASSERT_FALSE(obj.m_pbtAuthorizationInProcess) << L"m_pbtAuthorizationInProcess DoneWithTransfer";
		}
	}

	TEST_F(PersonalBankerUnitTests, BeginPersonalBankerAutoSessionUnitTest)
	{
		// Setup.
		std::string failedMsg;
		CPersonalBanker varPersonalBanker;
		bool sendToPoller(false);
		CConfig config(false, "");
		DWORD autoUploadPin(PBT_AUTO_UPLOAD_PIN);

		CCard card(CUtilities::GetCurrentTime(), CUtilities::GetUTCTime());
		card.SetCardType(CARD_PLAYER);
		varPersonalBanker.RebuildCard(&card);

		bool bNeedToSendToPoller = varPersonalBanker.BeginPersonalBankerAutoSession(sendToPoller, config, autoUploadPin, CUtilities::GetTickCount());

		//Tests
		{
			failedMsg = FormatString(_T("FAILED BeginPersonalBankerAutoSession failed in %s"),
				static_cast<LPCTSTR>(__TFUNCTION__));
			ASSERT_TRUE(bNeedToSendToPoller == sendToPoller) << failedMsg;
			failedMsg = FormatString(_T("FAILED BeginPersonalBankerAutoSession failed in %s.  State was not setup properly"),
				static_cast<LPCTSTR>(__TFUNCTION__));
			ASSERT_TRUE(PBStateWaitingForUploadToStart == varPersonalBanker.GetState()) << failedMsg;

			varPersonalBanker.DoneWithTransfer();
			sendToPoller = true;
			bNeedToSendToPoller = varPersonalBanker.BeginPersonalBankerAutoSession(sendToPoller, config, autoUploadPin, CUtilities::GetTickCount());

			failedMsg = FormatString(_T("FAILED BeginPersonalBankerAutoSession failed in %s"),
				static_cast<LPCTSTR>(__TFUNCTION__));
			ASSERT_TRUE(bNeedToSendToPoller == sendToPoller) << failedMsg;
			failedMsg = FormatString(_T("FAILED BeginPersonalBankerAutoSession failed in %s.  State was not setup properly"),
				static_cast<LPCTSTR>(__TFUNCTION__));
			ASSERT_TRUE(PBStateInquireSent == varPersonalBanker.GetState()) << failedMsg;

			varPersonalBanker.BeginPersonalBankerAutoSession(sendToPoller, config, autoUploadPin, CUtilities::GetTickCount());
			failedMsg = FormatString(_T("FAILED BeginPersonalBankerAutoSession failed in %s. Auto upload variables were not setup properly"),
				static_cast<LPCTSTR>(__TFUNCTION__));
			ASSERT_TRUE(varPersonalBanker.IsAutoTransferPending()) << failedMsg;

		}
	}

	TEST_F(PersonalBankerUnitTests, BeginPersonalBankerAutoSessionWithExistingTransferUnitTest)
	{
		// Setup.
		std::string failedMsg;
		CPersonalBanker varPersonalBanker;
		bool sendToPoller(false);
		CConfig config(false, "");
		DWORD autoUploadPin(PBT_AUTO_UPLOAD_PIN);

		varPersonalBanker.CreateTransfer(CUtilities::GetTickCount());
		varPersonalBanker.BeginPersonalBankerAutoSession(sendToPoller, config, autoUploadPin, CUtilities::GetTickCount());

		//Tests
		{
			failedMsg = FormatString(_T("FAILED BeginPersonalBankerAutoSession failed in %s. Transfer should exist."),
				static_cast<LPCTSTR>(__TFUNCTION__));
			ASSERT_TRUE(varPersonalBanker.HasTransfer()) << failedMsg;

			failedMsg = FormatString(_T("FAILED BeginPersonalBankerAutoSession failed in %s.  Auto NCEP out should be pending."),
				static_cast<LPCTSTR>(__TFUNCTION__));
			ASSERT_TRUE(varPersonalBanker.IsAutoUploadPending()) << failedMsg;
		}
	}

	TEST_F(PersonalBankerUnitTests, BeginPersonalBankerTransferUnitTest)
	{
		// Setup.
		std::string failedMsg;
		CPersonalBanker varPersonalBanker;
		CPersonalBankerTransfer* transferExpected(NULL);
		CGames games;
		PersonalBankerTransferType type(PromoDollarsToGame);
		DWORD amountInPennies(96 * ONE_DOLLAR_IN_CENTS);
		const byte STARTING_EFT_NUMBER = 1;
		byte pbEFTNumber(STARTING_EFT_NUMBER);
		bool success(false);
            CreateInfo(varPersonalBanker);

		//Tests
		{
			//#1
			//This test to see if BeginPersonalBankerTransfer returns a false when there is no transfer object
			success = varPersonalBanker.BeginPersonalBankerTransfer(games, type, amountInPennies, pbEFTNumber);
			failedMsg = FormatString(_T("FAILED BeginPersonalBankerTransfer failed in %s. Transfer does not exist so BeginPersonalBankerTransfer should fail."),
				static_cast<LPCTSTR>(__TFUNCTION__));
			ASSERT_FALSE(success) << failedMsg;

			//#2
			//This test to see if BeginPersonalBankerTransfer returns a false when there is transfer object but the state
			//is not set to PBStateInfoReceived.
			varPersonalBanker.CreateTransfer(CUtilities::GetTickCount());
			success = varPersonalBanker.BeginPersonalBankerTransfer(games, type, amountInPennies, pbEFTNumber);
			failedMsg = FormatString(_T("FAILED BeginPersonalBankerTransfer failed in %s. State is not set to PBStateInfoReceived so BeginPersonalBankerTransfer should fail."),
				static_cast<LPCTSTR>(__TFUNCTION__));
			ASSERT_FALSE(success) << failedMsg;

			//#3
			//This is the happy path test.
			varPersonalBanker.SetState(PBStateInfoReceived);
			varPersonalBanker.BeginPersonalBankerTransfer(games, type, amountInPennies, pbEFTNumber);
			transferExpected = varPersonalBanker.GetTransferCopy();

			failedMsg = FormatString(_T("FAILED BeginPersonalBankerTransfer failed in %s. State was not changed properly"),
				static_cast<LPCTSTR>(__TFUNCTION__));
			ASSERT_TRUE(PBStateTransferingFromToGame == varPersonalBanker.GetState()) << failedMsg;

			failedMsg = FormatString(_T("FAILED BeginPersonalBankerTransfer failed in %s.  Banker Transfer Type was not setup properly"),
				static_cast<LPCTSTR>(__TFUNCTION__));
			ASSERT_TRUE(transferExpected->GetPersonalBankerTransferType() == type) << failedMsg;

			failedMsg = FormatString(_T("FAILED BeginPersonalBankerTransfer failed in %s.  Amount was not setup properly"),
				static_cast<LPCTSTR>(__TFUNCTION__));
			ASSERT_TRUE(transferExpected->GetAmount() == amountInPennies) << failedMsg;

			failedMsg = FormatString(_T("FAILED BeginPersonalBankerTransfer failed in %s.  EFT Number was not setup properly"),
				static_cast<LPCTSTR>(__TFUNCTION__));
			ASSERT_TRUE(transferExpected->GetEFTNumber() == STARTING_EFT_NUMBER) << failedMsg;

				//#4 ExternalCredit test #1
			//This test to see if BeginPersonalBankerTransfer returns a false when there is transfer object.
				type = ExternalCredit;
			varPersonalBanker.CreateTransfer(CUtilities::GetTickCount());
			varPersonalBanker.SetState(PBStateInfoReceived);
			success = varPersonalBanker.BeginPersonalBankerTransfer(games, type, amountInPennies, pbEFTNumber);
			failedMsg = FormatString(_T("FAILED BeginPersonalBankerTransfer failed in %s. State is not set to PBStateInfoReceived so BeginPersonalBankerTransfer should fail."),
				static_cast<LPCTSTR>(__TFUNCTION__));
			ASSERT_FALSE(success) << failedMsg;

			CPersonalBankerTransfer* transferCopy(varPersonalBanker.GetTransferCopy());

				//#4 ExternalCredit test #2
			//This checks if PersonalBankerType is set to CreditTransfer.
			bool areEqual = transferCopy->GetPersonalBankerType() == CreditTransfer;
			failedMsg = FormatString(_T("FAILED PersonalBankerType=%d expected=%d in %s"),
				(int)transferCopy->GetPersonalBankerType(),
					(int)PromoCreditsToCash,
				static_cast<LPCTSTR>(__TFUNCTION__));
			ASSERT_TRUE(areEqual) << failedMsg;

				//#4 ExternalCredit test #3
			//This checks if cancelled type is set to Unknown.
			areEqual = transferCopy->GetCancelledType() == _Unknown;
			failedMsg = FormatString(_T("FAILED CancelledType=%d expected=%d in %s"),
				(int)transferCopy->GetCancelledType(),
				(int)_Unknown,
				static_cast<LPCTSTR>(__TFUNCTION__));
			ASSERT_TRUE(areEqual) << failedMsg;

			delete transferCopy;
		}

		delete transferExpected;
	}

	TEST_F(PersonalBankerUnitTests, PersonalBankerGetTransferAmountUnitTest)
	{
		CPersonalBanker personalBanker;
		__int64 amount = 0;

		amount = personalBanker.GetTransferAmount();
		ASSERT_TRUE(0 == amount) << L"Transfer amount not 0 before Transfer initialize";

		personalBanker.CreateTransfer(CUtilities::GetTickCount());
		amount = personalBanker.GetTransferAmount();
		ASSERT_TRUE(0 == amount) << L"Transfer amount not 0 when Transfer just created";

		CPersonalBankerTransfer* pbTransfer = personalBanker.GetTransferCopy();
		pbTransfer->SetAmount(1000);
		amount = personalBanker.GetTransferAmount();
		ASSERT_TRUE(0 == amount) << L"Transfer amount not 0 after copy is set";

		CGames games;
		byte pbEFTNumber(1);

		personalBanker.SetState(PBStateInfoReceived);
            CreateInfo(personalBanker);
			personalBanker.BeginPersonalBankerTransfer(games, PromoDollarsToGame, (DWORD)(pbTransfer->GetAmount()), pbEFTNumber);
		amount = personalBanker.GetTransferAmount();
		ASSERT_FALSE(0 == amount) << L"Transfer amount not changed by BeginPersonalBankerTransfer";
		ASSERT_TRUE(pbTransfer->GetAmount() == amount) << L"Transfer amount not matching amount passed";

		delete pbTransfer;
	}

	TEST_F(PersonalBankerUnitTests, PersonalBankerTransferHasAmountUnitTest)
	{
		CPersonalBanker personalBanker;
		bool has = false;

		has = personalBanker.TransferHasAmount();
		ASSERT_FALSE(has) << L"Reports Transfer has amount before Transfer initialize";

		personalBanker.CreateTransfer(CUtilities::GetTickCount());
		has = personalBanker.TransferHasAmount();
		ASSERT_FALSE(has) << L"Reports Transfer has amount when Transfer just created";

		CPersonalBankerTransfer* pbTransfer = personalBanker.GetTransferCopy();
		pbTransfer->SetAmount(1000);
		has = personalBanker.TransferHasAmount();
		ASSERT_FALSE(has) << L"Reports Transfer has amount after copy is set";

		CGames games;
		byte pbEFTNumber(1);

            CreateInfo(personalBanker);
		personalBanker.SetState(PBStateInfoReceived);
		personalBanker.BeginPersonalBankerTransfer(games, PromoDollarsToGame, (DWORD)(pbTransfer->GetAmount() / ONE_DOLLAR_IN_CENTS), pbEFTNumber);
		has = personalBanker.TransferHasAmount();
		ASSERT_TRUE(has) << L"Reports Transfer still has no amount when changed by BeginPersonalBankerTransfer";

		delete pbTransfer;
	}

	TEST_F(PersonalBankerUnitTests, PersonalBankerIsPersonalBankerTransferInProgressUnitTest)
	{
		CPersonalBanker personalBanker;
		bool inProgress = false;

		inProgress = personalBanker.IsPersonalBankerTransferInProgress();
		ASSERT_FALSE(inProgress) << L"Reports Transfer in progress before Transfer initialize";

		personalBanker.CreateTransfer(CUtilities::GetTickCount());
		inProgress = personalBanker.IsPersonalBankerTransferInProgress();
		ASSERT_FALSE(inProgress) << L"Reports Transfer in progress when Transfer just created";

		CGames games;
		byte pbEFTNumber(1);

            CreateInfo(personalBanker);
		personalBanker.SetState(PBStateInfoReceived);
		personalBanker.BeginPersonalBankerTransfer(games, PromoDollarsToGame, 100, pbEFTNumber);
		inProgress = personalBanker.IsPersonalBankerTransferInProgress();
		ASSERT_TRUE(inProgress) << L"Reports Transfer NOT in progress when changed by BeginPersonalBankerTransfer";
	}

	TEST_F(PersonalBankerUnitTests, PersonalBankerIsPersonalBankerTransferToGameUnitTest)
	{
		CPersonalBanker personalBanker;
		bool toGame = false;

		toGame = personalBanker.IsPersonalBankerTransferToGame();
		ASSERT_FALSE(toGame) << L"Reports Transfer To Game before Transfer initialize";

		personalBanker.CreateTransfer(CUtilities::GetTickCount());
		toGame = personalBanker.IsPersonalBankerTransferToGame();
		ASSERT_FALSE(toGame) << L"Reports Transfer To Game when Transfer just created";

		CGames games;
		byte pbEFTNumber(1);

            CreateInfo(personalBanker);
		personalBanker.SetState(PBStateInfoReceived);
		personalBanker.BeginPersonalBankerTransfer(games, PromoDollarsToGame, 100, pbEFTNumber);
		toGame = personalBanker.IsPersonalBankerTransferToGame();
		ASSERT_TRUE(toGame) << L"Does NOT report Transfer To Game when changed by BeginPersonalBankerTransfer";
	}

	/// <summary>
	/// Functional test for:
	///     CProgressiveConnection::VerifyTransferedAmounts(CPersonalBankerTransfer *transfer)
	/// Test with null transfer parameter and null m_pbTransfer member.
	/// </summary>
	TEST_F(PersonalBankerUnitTests, PersonalBankerVerifyAmounts_NullPBTransfer)
	{
		// Setup.
		CPersonalBanker personalBanker;
		CPersonalBankerTransfer *transfer(nullptr);

		// Perform operation(s) and test(s).
		ASSERT_EQ(tltUnknown, personalBanker.VerifyTransferedAmounts(transfer)) << L"VerifyTransferedAmounts";
	}

	/// <summary>
	/// Functional test for:
	///     CProgressiveConnection::VerifyTransferedAmounts(CPersonalBankerTransfer *transfer)
	/// Test with non-null transfer parameter and null m_pbTransfer member.
	/// </summary>
	TEST_F(PersonalBankerUnitTests, PersonalBankerVerifyAmounts_NullNull)
	{
		// Setup.
		CPersonalBanker personalBanker;
		CPersonalBankerTransfer transfer(0);
		transfer.SetAmount(INT_MAX);

		// Perform operation(s) and test(s).
		ASSERT_EQ(tltUnknown, personalBanker.VerifyTransferedAmounts(&transfer)) << L"VerifyTransferedAmounts";
	}

	/// <summary>
	/// Functional test for:
	///     CProgressiveConnection::VerifyTransferedAmounts(CPersonalBankerTransfer *transfer)
	/// Test with null transfer parameter and non-null m_pbTransfer member.
	/// </summary>
	TEST_F(PersonalBankerUnitTests, PersonalBankerVerifyAmounts_NullTransfer)
	{
		// Setup.
		CPersonalBanker personalBanker;
		personalBanker.CreateTransfer(0);
		CPersonalBankerTransfer *transfer(nullptr);

		// Perform operation(s) and test(s).
		ASSERT_EQ(tltUnknown, personalBanker.VerifyTransferedAmounts(transfer)) << L"VerifyTransferedAmounts";
	}

	TEST_F(PersonalBankerUnitTests, PersonalBankerVerifyAmountsUnitTest)
	{
		CDirectoryManager::MockInstance();
		CPersonalBanker personalBanker;
		__int64 amount = 0;

		personalBanker.CreateTransfer(CUtilities::GetTickCount());

		CPersonalBankerTransfer* pbTransfer = personalBanker.GetTransferCopy();
		pbTransfer->SetAmount(1000);

		CGames games;
		byte pbEFTNumber(1);

		personalBanker.SetState(PBStateInfoReceived);
                CreateInfo(personalBanker);
		personalBanker.BeginPersonalBankerTransfer(games, CashToGame, (DWORD)(pbTransfer->GetAmount()), pbEFTNumber);
		pbTransfer->SetPersonalBankerTransferType(CashToGame);
		ASSERT_TRUE(personalBanker.VerifyTransferedAmounts(pbTransfer) == tltUnknown);
		pbTransfer->SetAmount(1001);
		ASSERT_TRUE(personalBanker.VerifyTransferedAmounts(pbTransfer) == tltInvalidPbAmount);
		pbTransfer->SetAmount(1000001);
		ASSERT_TRUE(personalBanker.VerifyTransferedAmounts(pbTransfer) == tltTransferOvrLmtAftr);
		delete pbTransfer;

		// Restricted
		CPersonalBanker rPersonalBanker;
		rPersonalBanker.SetState(PBStateInfoReceived);
		rPersonalBanker.CreateTransfer(CUtilities::GetTickCount());
		pbTransfer = rPersonalBanker.GetTransferCopy();
		pbTransfer->SetPersonalBankerTransferType(PromoDollarsToGame);
		pbTransfer->SetAmount(1000);


                CreateInfo(rPersonalBanker);
		rPersonalBanker.BeginPersonalBankerTransfer(games, PromoDollarsToGame, (DWORD)(pbTransfer->GetAmount()), pbEFTNumber);
		ASSERT_TRUE(rPersonalBanker.VerifyTransferedAmounts(pbTransfer) == tltUnknown);
		pbTransfer->SetAmount(1001);
		ASSERT_TRUE(rPersonalBanker.VerifyTransferedAmounts(pbTransfer) == tltInvalidPbAmount);
		pbTransfer->SetAmount(1000001);
		ASSERT_TRUE(rPersonalBanker.VerifyTransferedAmounts(pbTransfer) == tltTransferOvrLmtAftr);
		delete pbTransfer;

		// Non restricted
		CPersonalBanker nrPersonalBanker;
		nrPersonalBanker.SetState(PBStateInfoReceived);
		nrPersonalBanker.CreateTransfer(CUtilities::GetTickCount());
                CreateInfo(nrPersonalBanker);
		pbTransfer = nrPersonalBanker.GetTransferCopy();
		pbTransfer->SetPersonalBankerTransferType(PromoDollarsToGame);
		pbTransfer->SetAmount(1000);
		nrPersonalBanker.BeginPersonalBankerTransfer(games, PromoDollarsToGame, (DWORD)(pbTransfer->GetAmount()), pbEFTNumber);
		ASSERT_TRUE(nrPersonalBanker.VerifyTransferedAmounts(pbTransfer) == tltUnknown);
		pbTransfer->SetAmount(1001);
		ASSERT_TRUE(nrPersonalBanker.VerifyTransferedAmounts(pbTransfer) == tltInvalidPbAmount);
		pbTransfer->SetAmount(1000001);
		ASSERT_TRUE(nrPersonalBanker.VerifyTransferedAmounts(pbTransfer) == tltTransferOvrLmtAftr);
		delete pbTransfer;
	}

	TEST_F(PersonalBankerUnitTests, PersonalBankerIsPinValidLengthUnitTest)
	{
		std::string pin;
		ASSERT_FALSE(CPersonalBanker::IsPinValidLength(pin));
		pin = (_T("1"));
		ASSERT_FALSE(CPersonalBanker::IsPinValidLength(pin));
		pin = (_T("11"));
		ASSERT_FALSE(CPersonalBanker::IsPinValidLength(pin));
		pin = (_T("111"));
		ASSERT_FALSE(CPersonalBanker::IsPinValidLength(pin));
		pin = (_T("1111"));
		ASSERT_TRUE(CPersonalBanker::IsPinValidLength(pin));
		pin = (_T("11111"));
		ASSERT_FALSE(CPersonalBanker::IsPinValidLength(pin));
	}

// #pragma region NvramMap
	/// <summary>
	/// Test that the NVRAM memory map has not changed across versions.
	/// If the NVRAM memory map does change across versions then memory map upgrades should be tested.
	/// </summary>
	TEST_F(PersonalBankerUnitTests, CPersonalBanker_NvramMap)
	{	// Test NVRAM memory map offset calculations.
		ASSERT_EQ(NVRAM_MONITOR_RESOLUTION_CHECK_OFFSET + NVRAM_MONITOR_RESOLUTION_CHECK_SIZE,
			NVRAM_PB_OFFSET) << L"NVRAM_PB_OFFSET calc";
		ASSERT_EQ(NVRAM_PB_OFFSET,
			NVRAM_PB_CHECKBYTE_OFFSET) << L"NVRAM_PB_CHECKBYTE_OFFSET calc";
		ASSERT_EQ(NVRAM_PB_CHECKBYTE_OFFSET + NVRAM_PB_CHECKBYTE_SIZE,
			NVRAM_PB_DATA_OFFSET) << L"NVRAM_PB_DATA_OFFSET calc";

		// Test NVRAM memory map for backwards and forwards memory compatibility.
		ASSERT_EQ((DWORD)709455, NVRAM_PB_OFFSET) << L"NVRAM_PB_OFFSET";
		ASSERT_EQ((DWORD)709455, NVRAM_PB_CHECKBYTE_OFFSET) << L"NVRAM_PB_CHECKBYTE_OFFSET";
		ASSERT_EQ((DWORD)1, NVRAM_PB_CHECKBYTE_SIZE) << L"NVRAM_CUMULATIVE_PROGRESSIVE_CHECKBYTE_SIZE";
		ASSERT_EQ((BYTE)0x1a, NVRAM_PB_CHECKBYTE_VALUE) << L"NVRAM_PB_CHECKBYTE_VALUE";
		ASSERT_EQ((DWORD)192, NVRAM_PB_SIZE) << L"NVRAM_PB_SIZE";
		ASSERT_EQ((DWORD)709456, NVRAM_PB_DATA_OFFSET) << L"NVRAM_PB_DATA_OFFSET";
		ASSERT_EQ((DWORD)191, NVRAM_PB_DATA_SIZE) << L"NVRAM_PB_DATA_SIZE";
		ASSERT_EQ((BYTE)0x9e, NVRAM_PB_CHECKBYTE2_VALUE) << L"NVRAM_PB_CHECKBYTE_VALUE";

		// Test NVRAM-related class members.
		ASSERT_EQ((DWORD)4, (DWORD)CPersonalBanker::NVRAM_PB_STATE_SIZE) << L"NVRAM_PB_STATE_SIZE";
		ASSERT_EQ(NVRAM_PB_DATA_OFFSET, (DWORD)CPersonalBanker::NVRAM_PB_STATE_OFFSET) << L"NVRAM_PB_STATE_OFFSET";
		ASSERT_EQ(NVRAM_PB_CHECKBYTE_OFFSET, (DWORD)CPersonalBanker::NVRAM_ACTIVE_OFFSET) << L"NVRAM_ACTIVE_OFFSET";
		ASSERT_EQ(NVRAM_PB_CHECKBYTE_VALUE, (DWORD)CPersonalBanker::NVRAM_ACTIVE_VALUE) << L"NVRAM_ACTIVE_VALUE";
		ASSERT_EQ(NVRAM_PB_DATA_OFFSET, (DWORD)CPersonalBanker::NVRAM_DATA_OFFSET) << L"NVRAM_DATA_OFFSET";
		ASSERT_EQ(NVRAM_PB_DATA_SIZE, (DWORD)CPersonalBanker::NVRAM_DATA_SIZE) << L"NVRAM_DATA_SIZE";
	}
// #pragma endregion NvramMap

// #pragma region NVRAM
        
	/// <summary>
	/// Functional test for:
	///     CPersonalBanker::CPersonalBanker()
	/// Test for correctly configured offsets, data size, and active value.
	/// </summary>
	TEST_F(PersonalBankerUnitTests, CPersonalBanker_ConfigureOffsets)
	{
		CPersonalBanker_ConfigureOffsets();
	}
	void PersonalBankerUnitTests::CPersonalBanker_ConfigureOffsets()
	{	// Data.
		const MEMORY_TYPE memoryTypeExpected(MEMORY_NVRAM);

		// Perform operation(s) and test(s).
		CPersonalBanker obj(memoryTypeExpected);
		ASSERT_EQ(memoryTypeExpected, obj.m_type) << L"m_type";
		ASSERT_EQ(NVRAM_PB_DATA_OFFSET, obj.m_dwOffset) << L"m_dwOffset";
		ASSERT_EQ(NVRAM_PB_DATA_SIZE, obj.m_dwAllowedSize) << L"m_dwAllowedSize";
		ASSERT_EQ(NVRAM_PB_CHECKBYTE_OFFSET, obj.m_activeOffset) << L"m_activeOffset";
		ASSERT_EQ(NVRAM_PB_CHECKBYTE_VALUE, obj.m_activeValue) << L"m_activeValue";
	}

	/// <summary>
	/// Functional test for:
	///     CPersonalBanker::CPersonalBanker()
	/// Test constructor with default memory type -- expect MEMORY_NONE.
	/// </summary>
	TEST_F(PersonalBankerUnitTests, CPersonalBanker_MemoryDefault)
	{
		CPersonalBanker_MemoryDefault();
	}
	void PersonalBankerUnitTests::CPersonalBanker_MemoryDefault()
	{	// Setup.
		CPersonalBanker_PersistNonDefault();

		// Perform operation(s) and test(s).
		CPersonalBanker obj;
		ASSERT_EQ((int)m_stateDefault, obj.m_state) << L"m_state";
		ASSERT_EQ((int)m_stateDefault, obj.GetState()) << L"GetState()";
		ASSERT_EQ((int)m_pbtAuthorizationInProcessDefault, (int)obj.m_pbtAuthorizationInProcess) << L"m_pbtAuthorizationInProcess";
	}

	/// <summary>
	/// Functional test for:
	///     CPersonalBanker::CPersonalBanker()
	/// Test constructor with memory type MEMORY_NONE.
	/// </summary>
	TEST_F(PersonalBankerUnitTests, CPersonalBanker_MemoryNone)
	{
		CPersonalBanker_MemoryNone();
	}
	void PersonalBankerUnitTests::CPersonalBanker_MemoryNone()
	{	// Setup.
		CPersonalBanker_PersistNonDefault();

		// Perform operation(s) and test(s).
		CPersonalBanker obj(MEMORY_NONE);
		ASSERT_EQ((int)m_stateDefault, obj.m_state) << L"m_state";
		ASSERT_EQ((int)m_stateDefault, obj.GetState()) << L"GetState()";
		ASSERT_EQ((int)m_pbtAuthorizationInProcessDefault, (int)obj.m_pbtAuthorizationInProcess) << L"m_pbtAuthorizationInProcess";
	}

	/// <summary>
	/// Functional test for:
	///     CPersonalBanker::CPersonalBanker()
	///     CPersonalBanker::SetFromNvramBuffer()
	/// Test constructor with memory type MEMORY_NVRAM, where the destructor was called for the previous instance.
	/// </summary>
	TEST_F(PersonalBankerUnitTests, CPersonalBanker_MemoryNvramDefault)
	{
		CPersonalBanker_MemoryNvramDefault();
	}
	void PersonalBankerUnitTests::CPersonalBanker_MemoryNvramDefault()
	{	// Setup.
		CPersonalBanker_PersistNonDefault();

		// Perform operation(s) and test(s).
		CPersonalBanker obj(MEMORY_NVRAM);
		ASSERT_EQ((int)m_stateDefault, obj.m_state) << L"m_state";
		ASSERT_EQ((int)m_stateDefault, obj.GetState()) << L"GetState()";
		ASSERT_EQ((int)m_pbtAuthorizationInProcessDefault, (int)obj.m_pbtAuthorizationInProcess) << L"m_pbtAuthorizationInProcess";
	}

	/// <summary>
	/// Functional test for:
	///     CPersonalBanker::CPersonalBanker()
	///     CPersonalBanker::SetFromNvramBuffer()
	/// Test constructor with memory type MEMORY_NVRAM, where the destructor was not called for the previous instance.
	/// </summary>
	TEST_F(PersonalBankerUnitTests, CPersonalBanker_MemoryNvramNonDefault)
	{
		CPersonalBanker_MemoryNvramNonDefault();
	}
	void PersonalBankerUnitTests::CPersonalBanker_MemoryNvramNonDefault()
	{	// Setup.  Should be equivalent to CPersonalBanker_PersistNonDefault(), except for when the destructor is called.
		CUnitTestingUtils::SetNvramValidAndUpgraded();
		CPersonalBanker obj(MEMORY_NVRAM);
		obj.SetState(m_stateNonDefault);
		obj.SetPbtAuthorizationInProcess(m_pbtAuthorizationInProcessNonDefault);

		{
			// Perform operation(s) and test(s).
			CPersonalBanker obj(MEMORY_NVRAM);
			ASSERT_EQ((int)m_stateNonDefault, obj.m_state) << L"m_state";
			ASSERT_EQ((int)m_stateNonDefault, obj.GetState()) << L"GetState()";
			ASSERT_EQ((int)m_pbtAuthorizationInProcessNonDefault, (int)obj.m_pbtAuthorizationInProcess) << L"m_pbtAuthorizationInProcess";
		}
	}

	/// <summary>
	/// Functional test for:
	///     CPersonalBanker::CPersonalBanker()
	/// Test constructor with memory type MEMORY_NVRAM and persisted instance inactive.
	/// </summary>
	TEST_F(PersonalBankerUnitTests, CPersonalBanker_MemoryInactive)
	{
		CPersonalBanker_MemoryInactive();
	}
	void PersonalBankerUnitTests::CPersonalBanker_MemoryInactive()
	{	// Setup.
		CPersonalBanker_PersistNonDefault();
		CMemoryStorage::SetInactiveLocation(CPersonalBanker::NVRAM_ACTIVE_OFFSET, (BYTE)~CPersonalBanker::NVRAM_ACTIVE_VALUE);

		// Perform operation(s) and test(s).
		CPersonalBanker obj(MEMORY_NVRAM);
		ASSERT_EQ((int)m_stateDefault, obj.m_state) << L"m_state";
		ASSERT_EQ((int)m_stateDefault, obj.GetState()) << L"GetState()";
		ASSERT_EQ((int)m_pbtAuthorizationInProcessDefault, (int)obj.m_pbtAuthorizationInProcess) << L"m_pbtAuthorizationInProcess";
	}

	/// <summary>
	/// Functional test for:
	///     CPersonalBanker::CPersonalBanker()
	/// Test constructor with NVRAM cleared as if via the UI and CInternalsFramework::ProcessClearPersistentMemory().
	/// </summary>
	TEST_F(PersonalBankerUnitTests, CPersonalBanker_MemoryCleared)
	{
		CPersonalBanker_MemoryCleared();
	}
	void PersonalBankerUnitTests::CPersonalBanker_MemoryCleared()
	{	// Setup.
		CPersonalBanker_PersistNonDefault();
		CUnitTestingUtils::SetNvramInvalid();

		// Perform operation(s) and test(s).
		CPersonalBanker obj(MEMORY_NVRAM);
		ASSERT_EQ((int)m_stateDefault, obj.m_state) << L"m_state";
		ASSERT_EQ((int)m_stateDefault, obj.GetState()) << L"GetState()";
		ASSERT_EQ((int)m_pbtAuthorizationInProcessDefault, (int)obj.m_pbtAuthorizationInProcess) << L"m_pbtAuthorizationInProcess";
	}


	/// <summary>
	/// Functional test for:
	///     CPersonalBanker::GetNvramBuffer()
	/// Test GetNvramBuffer() with non-default values for forwards and backwards compatibility.
	/// </summary>
	TEST_F(PersonalBankerUnitTests, GetNvramBuffer)
	{
		GetNvramBuffer();
	}
	void PersonalBankerUnitTests::GetNvramBuffer()
	{	// Data.
		static const PersonalBankerState state((PersonalBankerState)0x05040302);
		static const bool pbtAuthorizationInProcess(true);

		static const bool activeExpected(true);
		static const UINT bufferSizeUsedExpected(6);
		static const BYTE bufferExpected[CPersonalBanker::NVRAM_DATA_SIZE] =
		{
			0x02, 0x03, 0x04, 0x05, // m_state.
			0x9e, // checkByte2.
			0x01, // m_pbtAuthorizationInProcess.
		};

		// Setup.
		CPersonalBanker obj;
		obj.m_state = state;
		obj.m_pbtAuthorizationInProcess = pbtAuthorizationInProcess;

		// Perform operation(s) and test(s).
		BYTE buffer[CPersonalBanker::NVRAM_DATA_SIZE];
		bool active(!activeExpected);
		UINT bufferSizeUsed = obj.GetNvramBuffer(buffer, sizeof(buffer), active);
		ASSERT_EQ(activeExpected, active) << L"active";
		ASSERT_EQ(bufferSizeUsedExpected, bufferSizeUsed) << L"bufferSizeUsed";
		ASSERT_EQ(0, memcmp(buffer, bufferExpected, bufferSizeUsedExpected)) << L"memcmp";
	}

	/// <summary>
	/// Functional test for:
	///     CPersonalBanker::SetFromNvramBuffer()
	/// Test SetFromNvramBuffer() with non-default values for forwards and backwards compatibility.
	/// Test with checkByte2 not valid.
	/// </summary>
	TEST_F(PersonalBankerUnitTests, SetFromNvramBuffer1)
	{
		SetFromNvramBuffer1();
	}
	void PersonalBankerUnitTests::SetFromNvramBuffer1()
	{	// Data.
		static const PersonalBankerState state((PersonalBankerState)0x05040302);

		static const UINT bufferSizeUsed(4);
		BYTE buffer[CPersonalBanker::NVRAM_DATA_SIZE] =
		{
			0x02, 0x03, 0x04, 0x05, // m_state.
		};
		memset(buffer + bufferSizeUsed, m_nvramDefault, sizeof(buffer) - bufferSizeUsed);

		// Setup.
		CPersonalBanker obj;

		// Perform operation(s) and test(s).
		obj.SetFromNvramBuffer(buffer, sizeof(buffer));
		ASSERT_EQ(state, obj.m_state) << L"m_state";
		ASSERT_EQ(state, obj.GetState()) << L"GetState()";
		ASSERT_EQ((int)m_pbtAuthorizationInProcessDefault, (int)obj.m_pbtAuthorizationInProcess) << L"m_pbtAuthorizationInProcess";
	}

	/// <summary>
	/// Functional test for:
	///     CPersonalBanker::SetFromNvramBuffer()
	/// Test SetFromNvramBuffer() with non-default values for forwards and backwards compatibility.
	/// Test with checkByte2 valid.
	/// </summary>
	TEST_F(PersonalBankerUnitTests, SetFromNvramBuffer2)
	{
		SetFromNvramBuffer2();
	}
	void PersonalBankerUnitTests::SetFromNvramBuffer2()
	{	// Data.
		static const PersonalBankerState state((PersonalBankerState)0x05040302);
		static const bool pbtAuthorizationInProcess(true);

		static const UINT bufferSizeUsed(6);
		BYTE buffer[CPersonalBanker::NVRAM_DATA_SIZE] =
		{
			0x02, 0x03, 0x04, 0x05, // m_state.
			0x9e, // checkByte2.
			0x01, // m_pbtAuthorizationInProcess.
		};
		memset(buffer + bufferSizeUsed, m_nvramDefault, sizeof(buffer) - bufferSizeUsed);

		// Setup.
		CPersonalBanker obj;

		// Perform operation(s) and test(s).
		obj.SetFromNvramBuffer(buffer, sizeof(buffer));
		ASSERT_EQ(state, obj.m_state) << L"m_state";
		ASSERT_EQ(state, obj.GetState()) << L"GetState()";
		ASSERT_EQ(pbtAuthorizationInProcess, obj.m_pbtAuthorizationInProcess) << L"m_pbtAuthorizationInProcess";
	}

        void PersonalBankerUnitTests:: CreateInfo(CPersonalBanker &personalBanker)        
        {
            personalBanker.CreateInfo(MEMORY_NONE, false);
        }

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBanker::CPersonalBanker()
		/// Test the default state for CPersonalBanker::ProcessInvalidTransIdIfNecessary().
		/// </summary>
		TEST_F(PersonalBankerUnitTests, ProcessInvalidTransIdIfNecessaryNullIdTest)
		{
			// Test data.
			const string expectedPersonalBankerId;
			string personalBankerId;

			// Setup.
			CPersonalBanker personalBanker;

			// Perform operations to be tested.
			personalBanker.CreateTransfer(CUtilities::GetTickCount());

			CPersonalBankerInfo *personalBankerInfo = new CPersonalBankerInfo(MEMORY_NONE);
			personalBanker.SetState(PBStateInquireSent);
			personalBankerInfo->SetErrorCode(InvalidID);
			// Test #1.
			{
				bool invalidTransferIdProcessed(personalBanker.ProcessInvalidTransIdIfNecessary(personalBankerInfo, 1234, personalBankerId));
				ASSERT_TRUE(invalidTransferIdProcessed);
				ASSERT_TRUE(PBStateTransferComplete == personalBanker.GetState());
				ASSERT_TRUE(personalBankerId.empty());

			}

			// Cleanup.
			delete personalBankerInfo;
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBanker::CPersonalBanker()
		/// Test the default state for CPersonalBanker::ProcessInvalidTransIdIfNecessary().
		/// </summary>
		TEST_F(PersonalBankerUnitTests, ProcessInvalidTransIdIfNecessaryErrorInvalidTest)
		{
			// Test data.
			const string expectedPersonalBankerId("123456");
			string personalBankerId;

			// Setup.
			CPersonalBanker personalBanker;

			// Perform operations to be tested.
			personalBanker.CreateTransfer(CUtilities::GetTickCount());

			CPersonalBankerInfo *personalBankerInfo = new CPersonalBankerInfo(MEMORY_NONE);
			personalBanker.SetState(PBStateInquireSent);
			personalBankerInfo->SetErrorCode(InvalidID);
			personalBankerInfo->SetPersonalBankerID(expectedPersonalBankerId.c_str(), 6);

			// Test #1.
			{
				bool invalidTransferIdProcessed(personalBanker.ProcessInvalidTransIdIfNecessary(personalBankerInfo, 1234, personalBankerId));
				ASSERT_TRUE(invalidTransferIdProcessed);
				ASSERT_TRUE(PBStateTransferComplete == personalBanker.GetState());
				ASSERT_TRUE(expectedPersonalBankerId == personalBankerId);
			}

			// Cleanup.
			delete personalBankerInfo;
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBanker::CPersonalBanker()
		/// Test the default state for CPersonalBanker::ProcessInvalidTransIdIfNecessary().
		/// </summary>
		TEST_F(PersonalBankerUnitTests, ProcessInvalidTransIdIfNecessaryNoErrorTest)
		{
			// Test data.
			const string expectedPersonalBankerId("123456");
			string personalBankerId;

			// Setup.
			CPersonalBanker personalBanker;

			// Perform operations to be tested.
			personalBanker.CreateTransfer(CUtilities::GetTickCount());

			CPersonalBankerInfo *personalBankerInfo = new CPersonalBankerInfo(MEMORY_NONE);
			personalBanker.SetState(PBStateInquireSent);
			personalBankerInfo->SetErrorCode(NoError);
			personalBankerInfo->SetPersonalBankerID(expectedPersonalBankerId.c_str(), 6);

			// Test #1.
			{
				bool invalidTransferIdProcessed(personalBanker.ProcessInvalidTransIdIfNecessary(personalBankerInfo, 1234, personalBankerId));
				ASSERT_TRUE(invalidTransferIdProcessed);
				ASSERT_TRUE(PBStateTransferComplete == personalBanker.GetState());
				ASSERT_TRUE(expectedPersonalBankerId == personalBankerId);
			}

			// Cleanup.
			delete personalBankerInfo;
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBanker::CPersonalBanker()
		/// Test the default state for CPersonalBanker::ProcessInvalidTransIdIfNecessary().
		/// </summary>
		TEST_F(PersonalBankerUnitTests, ProcessInvalidTransIdIfNecessaryNullPersonalBankerInfoTest)
		{
			// Test data.
			string personalBankerId;

			// Setup.
			CPersonalBanker personalBanker;

			// Perform operations to be tested.
			personalBanker.CreateTransfer(CUtilities::GetTickCount());

			personalBanker.SetState(PBStateInquireSent);

			// Test #1.
			{
				bool invalidTransferIdProcessed(personalBanker.ProcessInvalidTransIdIfNecessary(nullptr, 1234, personalBankerId));
				ASSERT_FALSE(invalidTransferIdProcessed);
				ASSERT_TRUE(PBStateInquireSent == personalBanker.GetState());
			}

			// Cleanup.
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBanker::CPersonalBanker()
		/// Test the default state for CPersonalBanker::ProcessInvalidTransIdIfNecessary().
		/// </summary>
		TEST_F(PersonalBankerUnitTests, ProcessInvalidTransIdIfNecessaryValidIdTest)
		{
			// Test data.
			const string expectedPersonalBankerId("12345679009876543210");
			string personalBankerId;

			// Setup.
			CPersonalBanker personalBanker;

			// Perform operations to be tested.
			personalBanker.CreateTransfer(CUtilities::GetTickCount());

			CPersonalBankerInfo *personalBankerInfo = new CPersonalBankerInfo(MEMORY_NONE);
			personalBanker.SetState(PBStateInquireSent);
			personalBankerInfo->SetErrorCode(NoError);
			personalBankerInfo->SetPersonalBankerID(expectedPersonalBankerId.c_str(), expectedPersonalBankerId.size());

			// Test #1.
			{
				bool invalidTransferIdProcessed(personalBanker.ProcessInvalidTransIdIfNecessary(personalBankerInfo, 679009, personalBankerId));
				ASSERT_FALSE(invalidTransferIdProcessed);
			}

			// Cleanup.
			delete personalBankerInfo;
		}
}
