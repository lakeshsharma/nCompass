
#include "stdafx.h"
#include "PersonalBanker.h"

// Unit testing includes.
#include "gtest/gtest.h"


class PersonalBanker2 : public ::testing::Test
{
public:
	static const DWORD TEN_DAYS_IN_MILLISECONDS = 10 * MILLISECONDS_PER_DAY;

	// Use about 1/4 of the 49.7-day period of timer as a time that should not expire, without testing signed/unsigned (1/2 range) issues.
	static const DWORD m_longTimeoutValueInMilliseconds = TEN_DAYS_IN_MILLISECONDS;

	DWORD m_sessionTimeoutValueInMilliseconds;
	
    /// <summary>
	/// Initialize class member(s) before each test method is run.
	/// </summary>
	virtual void SetUp()
    {
        m_sessionTimeoutValueInMilliseconds = 100;
    }
    /// <summary>
	/// Cleanup class member(s) before each test method is run.
	/// </summary>
	virtual void TearDown()
	{

	}

    void CreateInfo(CPersonalBanker &personalBanker)        
    {
        personalBanker.CreateInfo(MEMORY_NONE, false);
    }

	bool GetUnlockEgmDuringTransfer(PersonalBankerTransferType type)
	{
		bool unlockEgmDuringTransfer(false);
		const bool gameOnline(false);
		const bool bIsEFT(false);
		LPCTSTR pbPin = _T("1234");
		CCard *card = new CCard(CUtilities::GetCurrentTime(),CUtilities::GetUTCTime());//(CTime(123456001), CTime(123456002));
		card->SetCardID(_T("12345"));
		CPlayerInfo* playerInfo(new CPlayerInfo());
		CConfig config(false,"");
		const CEGMConfig egmConfig(MEMORY_NONE);
		CGames games;
		DWORD amountInDollars(66);
		byte pbEFTNumber(1);
		CPlayer player(card, playerInfo, config, egmConfig, CUtilities::GetCurrentTime(), false, &games);
		CPersonalBankerInfo pbInfo;
		int cardLength(0);
		pbInfo.SetCardNumber(const_cast<byte*>(card->GetCardID(cardLength)));

		// Setup.	
		CPersonalBanker varPersonalBanker;

		// Perform operations to be tested.
        CreateInfo(varPersonalBanker);
		varPersonalBanker.BeginPersonalBankerSession(&player,
			pbPin,
			config);

		// Perform operations to be tested.
		varPersonalBanker.ProcessPersonalBankerInfo(&pbInfo,
			config,
			gameOnline,
			bIsEFT);

		if(varPersonalBanker.BeginPersonalBankerTransfer(games, type, amountInDollars, pbEFTNumber))
		{
			unlockEgmDuringTransfer = varPersonalBanker.GetUnlockEgmDuringTransfer();
		}

		return unlockEgmDuringTransfer;
	}
};
/// <summary>
/// Test the implementation of:
///     CPersonalBanker::CPersonalBanker()
///     CPersonalBanker::GetAction()
/// Test the default state for CPersonalBanker::GetAction().
/// </summary>
TEST_F(PersonalBanker2, GetActionDefaultTest)
{
	// Setup.
	const CPersonalBanker pb;

	// Test #1.
	// The PersonalBankerAction is NOT set by CPersonalBanker::CPersonalBanker().
	const PersonalBankerAction actionResult = pb.GetAction();
	const PersonalBankerAction actionExpected = actionResult;

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

	// Cleanup.
}

#if 0 // 2013-05-24 SetPersonalBankerAction is currently a private function.
/// <summary>
/// Test the implementation of:
///     CPersonalBanker::SetPersonalBankerAction()
///     CPersonalBanker::GetAction()
/// </summary>
TEST_F(PersonalBanker2, SetActionTest)
{
	typedef struct
	{
		// Inputs.
		const PersonalBankerAction m_action;

		// Expected results.
		const PersonalBankerAction m_actionExpected;
	} TestDataType;

	const TestDataType TEST_DATA[] =
	{
		{ PBActionNone, PBActionNone },
		{ PBActionGetEGMBalances, PBActionGetEGMBalances },
		{ PBActionPerformTransfer, PBActionPerformTransfer },
		{ PBActionSendTransferTransaction, PBActionSendTransferTransaction },
		{ PBActionReleaseSession, PBActionReleaseSession },
		{ (PersonalBankerAction)0, (PersonalBankerAction)0 },
		{ (PersonalBankerAction)1, (PersonalBankerAction)1 },
		{ (PersonalBankerAction)(INT_MAX - 1), (PersonalBankerAction)(INT_MAX - 1) },
		{ (PersonalBankerAction)INT_MAX, (PersonalBankerAction)INT_MAX },
		{ (PersonalBankerAction)INT_MIN, (PersonalBankerAction)INT_MIN },
		{ (PersonalBankerAction)(INT_MIN + 1), (PersonalBankerAction)(INT_MIN + 1) },
		{ (PersonalBankerAction)-2, (PersonalBankerAction)-2 },
		{ (PersonalBankerAction)-1, (PersonalBankerAction)-1 },
	};

	for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
	{
		// Setup.
		CPersonalBanker varPb;
		varPb.SetPersonalBankerAction(TEST_DATA[testIndex].m_action);
		// 2013-05-16 Copy constructor is probably NOT supported for this test.
		CPersonalBanker const * const pb = const_cast<CPersonalBanker *>(&varPb);

		// Test #1.
		const PersonalBankerAction actionResult = pb->GetAction();

		{
			// Report #1.
			bool areEqual = actionResult == TEST_DATA[testIndex].m_actionExpected;
			std::string failedMsg;
			failedMsg = FormatString(_T("FAILED actionResult=%d expected=%d in %s"),
				(int)actionResult,
				(int)TEST_DATA[testIndex].m_actionExpected,
				static_cast<LPCTSTR>(__TFUNCTION__));
			ASSERT_TRUE(areEqual) << failedMsg;
		}

		// Cleanup.
	}
}
#endif // 2013-05-24 SetPersonalBankerAction is currently a private function.

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::SetPersonalBankerAction()
/// Code coverage (and functional) test.
/// </summary>
TEST_F(PersonalBanker2, SetAction1Test)
{
	// Test data -- expected results.
	const bool transferExpected(true);
	const PersonalBankerAction pbActionExpected(PBActionReleaseSession);

	// Setup.
	const int testIndex(0);
	const CConfig config(false, "");
	CPersonalBanker varPb;

	// Perform operations to be tested.
	varPb.CreateTransfer(CUtilities::GetTickCount());
	varPb.SetState(PBStateInquireSent);
	varPb.EndActivePlayerSession(config);
	const CPersonalBanker &pb(varPb);

	// Test #1.
	const CPersonalBankerTransfer *pbTransferResult = pb.GetTransferCopy();

	{
		bool areEqual = (pbTransferResult != NULL) == transferExpected;

		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED pbTransferResult=%p expected=%s testIndex=%d in %s(%d)"),
			(const void *)pbTransferResult,
			static_cast<LPCTSTR>(transferExpected ? _T("NOT NULL") :  _T("NULL")),
			(int)testIndex,
			static_cast<LPCTSTR>(__TFUNCTION__), __LINE__);
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Test #2.
	const PersonalBankerAction pbAction = pb.GetAction();

	{
		bool areEqual = pbAction == pbActionExpected;

		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED pbAction=%d expected=%d testIndex=%d in %s(%d)"),
			(int)pbAction,
			(int)pbActionExpected,
			(int)testIndex,
			static_cast<LPCTSTR>(__TFUNCTION__), __LINE__);
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Cleanup.
	delete pbTransferResult;
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::SetPersonalBankerAction()
/// Code coverage (and functional) test.
/// </summary>
TEST_F(PersonalBanker2, SetAction2Test)
{
	// Test data -- expected results.
	const bool transferExpected(true);
	const PersonalBankerAction pbActionExpected(PBActionReleaseSession);

	// Setup.
	const int testIndex(0);
	const CConfig config(false, "");
	CPersonalBanker varPb;

	// Perform operations to be tested.
	varPb.CreateTransfer(CUtilities::GetTickCount());
	varPb.SetState(PBStateInquireSent);
	varPb.EndActivePlayerSession(config);

	varPb.SetState(PBStateInfoReceived);
	varPb.EndActivePlayerSession(config);
	const CPersonalBanker &pb(varPb);

	// Test #1.
	const CPersonalBankerTransfer *pbTransferResult = pb.GetTransferCopy();

	{
		bool areEqual = (pbTransferResult != NULL) == transferExpected;

		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED pbTransferResult=%p expected=%s testIndex=%d in %s(%d)"),
			(const void *)pbTransferResult,
			static_cast<LPCTSTR>(transferExpected ? _T("NOT NULL") :  _T("NULL")),
			(int)testIndex,
			static_cast<LPCTSTR>(__TFUNCTION__), __LINE__);
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Test #2.
	const PersonalBankerAction pbAction = pb.GetAction();

	{
		bool areEqual = pbAction == pbActionExpected;

		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED pbAction=%d expected=%d testIndex=%d in %s(%d)"),
			(int)pbAction,
			(int)pbActionExpected,
			(int)testIndex,
			static_cast<LPCTSTR>(__TFUNCTION__), __LINE__);
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Cleanup.
	delete pbTransferResult;
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::CPersonalBanker()
///     CPersonalBanker::GetState()
/// Test the default state for CPersonalBanker::GetState().
/// </summary>
TEST_F(PersonalBanker2, GetStateDefaultTest)
{
	// Setup.
	const CPersonalBanker pb;
	const PersonalBankerState stateExpected = PBStateUnknown;

	// Test #1.
	const PersonalBankerState stateResult = pb.GetState();

	{
		// Report #1.
		bool areEqual = stateResult == stateExpected;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED stateResult=%d expected=%d in %s"),
			(int)stateResult,
			(int)stateExpected,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::SetPersonalBankerState()
///     CPersonalBanker::GetState()
/// </summary>
TEST_F(PersonalBanker2, SetStateTest)
{
	typedef struct
	{
		// Inputs.
		const PersonalBankerState m_state;

		// Expected results.
		const PersonalBankerState m_stateExpected;
	} TestDataType;

	const TestDataType TEST_DATA[] =
	{
		{ PBStateUnknown, PBStateUnknown },
		{ PBStateInquireSent, PBStateInquireSent },
		{ PBStateInfoReceived, PBStateInfoReceived },
		{ PBStateTransferingFromToGame, PBStateTransferingFromToGame },
		{ PBStateTransferComplete, PBStateTransferComplete },
		{ PBStateReleaseSent, PBStateReleaseSent },
		{ PBStateWaitingForUploadToStart, PBStateWaitingForUploadToStart },
		{ PBStateIdle_NotUsed, PBStateIdle_NotUsed },
		{ PBStateSessionRequested_NotUsed, PBStateSessionRequested_NotUsed },
		{ PBStateAuthRequested_NotUsed, PBStateAuthRequested_NotUsed },
		{ PBStateAuthGranted_NotUsed, PBStateAuthGranted_NotUsed },
		{ PBStateAuthDenied, PBStateAuthDenied },
		{ PBStateSendingTransferTransaction_NotUsed, PBStateSendingTransferTransaction_NotUsed },
		{ PBStateTransferTransactionSent_NotUsed, PBStateTransferTransactionSent_NotUsed },
		{ PBStateSendingRelease_NotUsed, PBStateSendingRelease_NotUsed },
		{ (PersonalBankerState)0, (PersonalBankerState)0 },
		{ (PersonalBankerState)1, (PersonalBankerState)1 },
		{ (PersonalBankerState)(INT_MAX - 1), (PersonalBankerState)(INT_MAX - 1) },
		{ (PersonalBankerState)INT_MAX, (PersonalBankerState)INT_MAX },
		{ (PersonalBankerState)INT_MIN, (PersonalBankerState)INT_MIN },
		{ (PersonalBankerState)(INT_MIN + 1), (PersonalBankerState)(INT_MIN + 1) },
		{ (PersonalBankerState)-2, (PersonalBankerState)-2 },
		{ (PersonalBankerState)-1, (PersonalBankerState)-1 },
	};

	for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
	{
		// Setup.
		CPersonalBanker varPb;
		varPb.SetState(TEST_DATA[testIndex].m_state);
		// 2013-05-16 Copy constructor is NOT supported for this test.
		CPersonalBanker const * const pb = const_cast<CPersonalBanker *>(&varPb);

		// Test #1.
		const PersonalBankerState stateResult = pb->GetState();

		{
			// Report #1.
			bool areEqual = stateResult == TEST_DATA[testIndex].m_stateExpected;
			std::string failedMsg;
			failedMsg = FormatString(_T("FAILED stateResult=%d expected=%d in %s"),
				(int)stateResult,
				(int)TEST_DATA[testIndex].m_stateExpected,
				static_cast<LPCTSTR>(__TFUNCTION__));
			ASSERT_TRUE(areEqual) << failedMsg;
		}

		// Cleanup.
	}
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::CPersonalBanker()
///     CPersonalBanker::IsPbtInProgress()
/// Test the default state for CPersonalBanker::IsPbtInProgress().
/// </summary>
TEST_F(PersonalBanker2, IsPbtInProgressDefaultTest)
{
	// Setup.
	const CPersonalBanker pb;
	const bool pbtInProgressExpected = false;

	// Test #1.
	const bool pbtInProgressResult = pb.IsPbtInProgress();

	{
		// Report #1.
		bool areEqual = pbtInProgressResult == pbtInProgressExpected;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED pbtInProgressResult=%d expected=%d in %s"),
			(int)pbtInProgressResult,
			(int)pbtInProgressExpected,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::CPersonalBanker()
///     CPersonalBanker::CreatePersonalBankerTransfer()
///     CPersonalBanker::IsPbtInProgress()
/// </summary>
TEST_F(PersonalBanker2, IsPbtInProgressDefaultTransferTest)
{
	// Setup.
	CPersonalBanker varPb;
	// 2013-05-16 Copy constructor is probably NOT supported for this test.
	CPersonalBanker const * const pb1 = const_cast<CPersonalBanker *>(&varPb);
	const bool pbtInProgress1Expected = false;
	const bool pbtInProgress2Expected = true;
	const bool pbtInProgress3Expected = false;

	// Test #1.
	const bool pbtInProgress1Result = pb1->IsPbtInProgress();

	{
		// Report #1.
		bool areEqual = pbtInProgress1Result == pbtInProgress1Expected;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED pbtInProgress1Result=%d expected=%d in %s"),
			(int)pbtInProgress1Result,
			(int)pbtInProgress1Expected,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::CPersonalBanker()
///     CPersonalBanker::AutoUpload()->SetAutoInProcess()
///     CPersonalBanker::IsPbtInProgress()
/// </summary>
TEST_F(PersonalBanker2, IsPbtInProgressDefaultAutoInProcessTest)
{
	// Setup.
	CPersonalBanker varPb;
	// 2013-05-16 Copy constructor is probably NOT supported for this test.
	CPersonalBanker const * const pb1 = const_cast<CPersonalBanker *>(&varPb);
	const bool pbtInProgress1Expected = false;
	const bool pbtInProgress2Expected = true;
	const bool pbtInProgress3Expected = false;

	// Test #1.
	const bool pbtInProgress1Result = pb1->IsPbtInProgress();

	{
		// Report #1.
		bool areEqual = pbtInProgress1Result == pbtInProgress1Expected;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED pbtInProgress1Result=%d expected=%d in %s"),
			(int)pbtInProgress1Result,
			(int)pbtInProgress1Expected,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Perform operation(s) to be tested.
	varPb.SetAutoInProcess(true);
	// 2013-05-16 Copy constructor is probably NOT supported for this test.
	CPersonalBanker const * const pb2 = const_cast<CPersonalBanker *>(&varPb);

	// Test #2.
	const bool pbtInProgress2Result = pb2->IsPbtInProgress();

	{
		// Report #2.
		bool areEqual = pbtInProgress2Result == pbtInProgress2Expected;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED pbtInProgress2Result=%d expected=%d in %s"),
			(int)pbtInProgress2Result,
			(int)pbtInProgress2Expected,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Perform more operation(s) to be tested.
	varPb.SetAutoInProcess(false);
	// 2013-05-16 Copy constructor is probably NOT supported for this test.
	CPersonalBanker const * const pb3 = const_cast<CPersonalBanker *>(&varPb);

	// Test #2.
	const bool pbtInProgress3Result = pb3->IsPbtInProgress();

	{
		// Report #2.
		bool areEqual = pbtInProgress3Result == pbtInProgress3Expected;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED pbtInProgress3Result=%d expected=%d in %s"),
			(int)pbtInProgress3Result,
			(int)pbtInProgress3Expected,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::CPersonalBanker()
///     CPersonalBanker::AutoUpload()->SetAutoUploadPending()
///     CPersonalBanker::IsPbtInProgress()
/// </summary>
TEST_F(PersonalBanker2, IsPbtInProgressDefaultAutoNcepOutPendingTest)
{
	// Setup.
	CPersonalBanker varPb;
	// 2013-05-16 Copy constructor is probably NOT supported for this test.
	CPersonalBanker const * const pb1 = const_cast<CPersonalBanker *>(&varPb);
	const bool pbtInProgress1Expected = false;
	const bool pbtInProgress2Expected = true;
	const bool pbtInProgress3Expected = false;

	// Test #1.
	const bool pbtInProgress1Result = pb1->IsPbtInProgress();

	{
		// Report #1.
		bool areEqual = pbtInProgress1Result == pbtInProgress1Expected;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED pbtInProgress1Result=%d expected=%d in %s"),
			(int)pbtInProgress1Result,
			(int)pbtInProgress1Expected,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Perform operation(s) to be tested.
	varPb.MakeAutoUploadPending();
	// 2013-05-16 Copy constructor is probably NOT supported for this test.
	CPersonalBanker const * const pb2 = const_cast<CPersonalBanker *>(&varPb);

	// Test #2.
	const bool pbtInProgress2Result = pb2->IsPbtInProgress();

	{
		// Report #2.
		bool areEqual = pbtInProgress2Result == pbtInProgress2Expected;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED pbtInProgress2Result=%d expected=%d in %s"),
			(int)pbtInProgress2Result,
			(int)pbtInProgress2Expected,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Perform more operation(s) to be tested.
	varPb.ProcessCancelPersonalBankerSession(NULL);
	// 2013-05-16 Copy constructor is probably NOT supported for this test.
	CPersonalBanker const * const pb3 = const_cast<CPersonalBanker *>(&varPb);

	// Test #2.
	const bool pbtInProgress3Result = pb3->IsPbtInProgress();

	{
		// Report #2.
		bool areEqual = pbtInProgress3Result == pbtInProgress3Expected;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED pbtInProgress3Result=%d expected=%d in %s"),
			(int)pbtInProgress3Result,
			(int)pbtInProgress3Expected,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::CPersonalBanker()
///     CPersonalBanker::RebuildCard(const CCard*)
/// Test using a NULL CCard pointer.
/// </summary>
TEST_F(PersonalBanker2, RebuildCardNullTest)
{
	// Setup.
	CPersonalBanker varPb;
	CCard const * const cardExpected = varPb.GetAutoUploadCard();

	// Perform operation(s) to be tested.
	varPb.RebuildCard(NULL);

	// Test #1.
	CCard const * const cardResult = varPb.GetAutoUploadCard();

	{
		// Report #1.
		bool areEqual = cardResult == cardExpected;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED cardResult=%p expected=%p in %s"),
			(void *)cardResult,
			(void *)cardExpected,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::CPersonalBanker()
///     CPersonalBanker::RebuildCard(const CCard*)
///     CPersonalBanker::AutoUpload()->GetCard()
/// </summary>
TEST_F(PersonalBanker2, RebuildCardTest)
{
	typedef struct
	{
		// Inputs.
		CardType m_type;

		// Expected results.
		CardType m_cardTypeExpected;
	} TestDataType;

	const TestDataType TEST_DATA[] =
	{
		{ CARD_UNKNOWN, CARD_UNKNOWN },
		{ CARD_PLAYER, CARD_PLAYER },
		{ CARD_MECHANIC, CARD_MECHANIC },
		{ CARD_GLOBAL, CARD_GLOBAL },
		{ CARD_FLOOR, CARD_FLOOR },
		{ CARD_CURRENCY, CARD_CURRENCY },
		{ CARD_METER, CARD_METER },
		{ CARD_TEMP, CARD_TEMP },
		{ (CardType)0, (CardType)0 },
		{ (CardType)1, (CardType)1 },
		{ (CardType)(INT_MAX - 1), (CardType)(INT_MAX - 1) },
		{ (CardType)INT_MAX, (CardType)INT_MAX },
		{ (CardType)INT_MIN, (CardType)INT_MIN },
		{ (CardType)(INT_MIN + 1), (CardType)(INT_MIN + 1) },
		{ (CardType)-2, (CardType)-2 },
		{ (CardType)-1, (CardType)-1 },
	};

	for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
	{

		// Setup.
		CPersonalBanker varPb;
		CCard varCard(CUtilities::GetCurrentTime(),CUtilities::GetUTCTime());
		varCard.SetCardType(TEST_DATA[testIndex].m_type);
		const CCard card(varCard);

		// Perform operation(s) to be tested.
		varPb.RebuildCard(&card);

		// Test #1.
		CCard const * const cardResult = varPb.GetAutoUploadCard();

		{
			// Report #1.
			std::string failedMsg;
			failedMsg = FormatString(_T("FAILED cardResult=%p expected=NOT NULL in %s"),
				(void *)cardResult,
				static_cast<LPCTSTR>(__TFUNCTION__));
			ASSERT_TRUE(cardResult != NULL) << failedMsg;
		}

		// Test #2.  Verify value of GetCard()->GetCardType().
		const CardType cardTypeResult = cardResult->GetCardType();

		{
			// Report #2.
			bool areEqual = cardTypeResult == TEST_DATA[testIndex].m_cardTypeExpected;
			std::string failedMsg;
			failedMsg = FormatString(_T("FAILED cardTypeResult=%d expected=%d testIndex=%d in %s"),
				(int)cardTypeResult,
				(int)TEST_DATA[testIndex].m_cardTypeExpected,
				(int)testIndex,
				static_cast<LPCTSTR>(__TFUNCTION__));
			ASSERT_TRUE(areEqual) << failedMsg;
		}

		// Cleanup.
	}
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::CPersonalBanker()
///     CPersonalBanker::RebuildCard()
///     CPersonalBanker::AutoUpload()->GetCard()
/// </summary>
TEST_F(PersonalBanker2, RebuildCardDefaultTest)
{
	typedef struct
	{
		// Inputs.
		CardType m_type;

		// Expected results.
		CardType m_cardTypeExpected;
	} TestDataType;

	const TestDataType TEST_DATA[] =
	{
		{ CARD_UNKNOWN, CARD_PLAYER },
		{ CARD_PLAYER, CARD_PLAYER },
		{ CARD_MECHANIC, CARD_PLAYER },
		{ CARD_GLOBAL, CARD_PLAYER },
		{ CARD_FLOOR, CARD_PLAYER },
		{ CARD_CURRENCY, CARD_PLAYER },
		{ CARD_METER, CARD_PLAYER },
		{ CARD_TEMP, CARD_PLAYER },
		{ (CardType)0, CARD_PLAYER },
		{ (CardType)1, CARD_PLAYER },
		{ (CardType)(INT_MAX - 1), CARD_PLAYER },
		{ (CardType)INT_MAX, CARD_PLAYER },
		{ (CardType)INT_MIN, CARD_PLAYER },
		{ (CardType)(INT_MIN + 1), CARD_PLAYER },
		{ (CardType)-2, CARD_PLAYER },
		{ (CardType)-1, CARD_PLAYER },
	};

	const byte CARD_ID[5] = {0x29,0x28,0x00,0x01,0xdf};
	const std::string cardStr = _T("29280001DF");


	for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
	{
		// Setup.
		CPersonalBanker varPb;
		varPb.CreateTransfer(CUtilities::GetTickCount(), MEMORY_NONE, false);
		CCard varCard(CUtilities::GetCurrentTime(),CUtilities::GetUTCTime());
		varCard.SetCardType(TEST_DATA[testIndex].m_type);
		const CCard card(varCard);
		varPb.SetTransferCardNumber(&card, CUtilities::GetCurrentTime());

		// Perform operation(s) to be tested.
		varPb.RebuildCard();

		// Test #1.
		CCard const * const cardResult = varPb.GetAutoUploadCard();

		{
			// Report #1.
			std::string failedMsg;
			failedMsg = FormatString(_T("FAILED cardResult=%p expected=NOT NULL in %s"),
				(void *)cardResult,
				static_cast<LPCTSTR>(__TFUNCTION__));
			ASSERT_TRUE(cardResult != NULL) << failedMsg;
		}

		// Test #2.  Verify value of GetCard()->GetCardType().
		const CardType cardTypeResult = cardResult->GetCardType();

		{
			// Report #2.
			bool areEqual = cardTypeResult == TEST_DATA[testIndex].m_cardTypeExpected;
			std::string failedMsg;
			failedMsg = FormatString(_T("FAILED cardTypeResult=%d expected=%d testIndex=%d in %s"),
				(int)cardTypeResult,
				(int)TEST_DATA[testIndex].m_cardTypeExpected,
				(int)testIndex,
				static_cast<LPCTSTR>(__TFUNCTION__));
			ASSERT_TRUE(areEqual) << failedMsg;
		}

	}

	// Test #3. Verify value of CardID
	{
		CPersonalBanker varPb;
		varPb.CreateTransfer(CUtilities::GetTickCount(), MEMORY_NONE, false);
		CCard varCard(CUtilities::GetCurrentTime(),CUtilities::GetUTCTime());
		varCard.SetCardType(TEST_DATA[1].m_type);
		varCard.SetCardID( cardStr );
		const CCard card(varCard);
		varPb.SetTransferCardNumber(&card, CUtilities::GetCurrentTime());

		// Perform operation(s) to be tested.
		varPb.RebuildCard();

		// Test #1.
		CCard *rebuiltCard = varPb.GetAutoUploadCard();

		// Report #3.
		int idLen = 0;
		const byte *cardId = rebuiltCard->GetCardID(idLen);
		bool areEqual = !memcmp(cardId,CARD_ID, idLen);

		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED cardID not expected"));
		ASSERT_TRUE(areEqual) << failedMsg;
	}
	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::HasTransfer()
/// </summary>
TEST_F(PersonalBanker2, HasTransferFalseTest)
{
	// Test data.
	bool hasTransferExpected(false);

	// Setup
	CPersonalBanker personalBanker;

	// Test.
	bool hasTransferResult = personalBanker.HasTransfer();

	{
		// Report #1.
		bool areEqual = hasTransferResult == hasTransferExpected;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED hasTransferResult=%d expected=%d in %s"),
			(int)hasTransferResult,
			(int)hasTransferExpected,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::HasTransfer()
/// </summary>
TEST_F(PersonalBanker2, HasTransferTrueTest)
{
	// Test data.
	bool hasTransferExpected(true);

	// Setup
	CPersonalBanker personalBanker;
	personalBanker.CreateTransfer(CUtilities::GetTickCount());

	// Test.
	bool hasTransferResult = personalBanker.HasTransfer();

	{
		// Report #1.
		bool areEqual = hasTransferResult == hasTransferExpected;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED hasTransferResult=%d expected=%d in %s"),
			(int)hasTransferResult,
			(int)hasTransferExpected,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::HasTransferForDifferentCardNumber()
/// </summary>
TEST_F(PersonalBanker2, HasTransferForDifferentCardNumberTest)
{
	typedef struct
	{
		// Inputs.
		const bool m_hasTransfer;
		const CardType m_pbCardType;
		const byte *m_cardNumber;
		const int m_cardNumberLen;
		const CardType m_cardType;
		const byte *m_cardID;
		const int m_cardIDLen;

		// Expected results.
		const bool m_hasTransferForDifferentCardNumberExpected;
	} TestDataType;

	const byte cardNumber0[] = {'\0'};
	const int cardNumberLen0 = 0;
	const byte *cardID0 = NULL;
	const int cardIDLen0 = 0;

	const byte cardNumber1[] = {'\0'};
	const byte cardNumberLen1 =0;
	const byte cardID1[] = { 0xa5, 0x23, 0x45, 0x67, 0x89 };
	const int cardIDLen1 = sizeof(cardID1);

	const byte cardNumber2[] =  { 0xa5, 0x23, 0x45, 0x67, 0x89 };
	const byte cardNumberLen2 = sizeof(cardNumber2);
	const byte cardID2[] = { 0xa5, 0x23, 0x45, 0x67, 0x89 };
	const int cardIDLen2 = sizeof(cardID2);

	const byte cardNumber3[] =  { 0xa5, 0x23, 0x45, 0x67, 0x89 };
	const byte cardNumberLen3 = sizeof(cardNumber3);
	const byte cardID3[] = { 0xa5, 0x23, 0x45, 0x67, 0x89 };
	const int cardIDLen3 = sizeof(cardID3);

	const byte cardNumber4[] = {'\0'};
	const byte cardNumberLen4 = 0;
	const byte cardID4[] = { 0xa5, 0x23, 0x45, 0x67, 0x89 };
	const int cardIDLen4 = sizeof(cardID4);

	const byte cardNumber5[] = {'\0'};
	const byte cardNumberLen5 = 33;
	const byte cardID5[] = { 0xa5, 0x23, 0x45, 0x67, 0x89 };
	const int cardIDLen5 = sizeof(cardID5);

	const byte cardNumber6[] = { 0xff };
	const byte cardNumberLen6 = sizeof(cardNumber6);
	const byte cardID6[] = { 0xa5, 0x23, 0x45, 0x67, 0x89 };
	const int cardIDLen6 = sizeof(cardID6);

	const byte cardNumber7[] = { 0xff, 0xee, 0xdd, 0xcc };
	const byte cardNumberLen7 = sizeof(cardNumber7);
	const byte cardID7[] = { 0xa5, 0x23, 0x45, 0x67, 0x89 };
	const int cardIDLen7 = sizeof(cardID7);

	const byte cardNumber8[] = { 0xa5, 0x23, 0x45, 0x67, 0x89 };
	const byte cardNumberLen8 = sizeof(cardNumber8);
	const byte cardID8[] = { 0xa5 };
	const int cardIDLen8 = sizeof(cardID8);

	const byte cardNumber9[] = { 0xa5, 0x23, 0x45, 0x67, 0x89 };
	const byte cardNumberLen9 = sizeof(cardNumber9);
	const byte cardID9[] = { 0xa5, 0x23, 0x45, 0x67 };
	const int cardIDLen9 = sizeof(cardID9);

	const TestDataType TEST_DATA[] =
	{
		{ false, (CardType)-1, cardNumber0, cardNumberLen0, CARD_UNKNOWN, cardID0, cardIDLen0, false },
		{ false, CARD_UNKNOWN, cardNumber1, cardNumberLen1, CARD_PLAYER, cardID1, cardIDLen1, false },
		{ false, CARD_PLAYER, cardNumber2, cardNumberLen2, CARD_PLAYER, cardID2, cardIDLen2, false },
		{ true, CARD_PLAYER, cardNumber3, cardNumberLen3, CARD_MECHANIC, cardID3, cardIDLen3, false },
		{ true, CARD_MECHANIC, cardNumber4, cardNumberLen4, CARD_GLOBAL, cardID4, cardIDLen4, true },
		{ true, CARD_GLOBAL, cardNumber5, cardNumberLen5, CARD_FLOOR, cardID5, cardIDLen5, true },
		{ true, CARD_FLOOR, cardNumber6, cardNumberLen6, CARD_CURRENCY, cardID6, cardIDLen6, true },
		{ true, CARD_CURRENCY, cardNumber7, cardNumberLen7, CARD_METER, cardID7, cardIDLen7, true },
		{ true, CARD_METER, cardNumber8, cardNumberLen8, CARD_TEMP, cardID8, cardIDLen8, false },
		{ true, CARD_TEMP, cardNumber9, cardNumberLen9, (CardType)0x7f, cardID9, cardIDLen9, false },
	};

	for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
	{
		CPersonalBanker personalBanker;
		if (TEST_DATA[testIndex].m_hasTransfer)
		{
			CCard varCard(CUtilities::GetCurrentTime(), CUtilities::GetCurrentTime());
			varCard.SetCardInfo(TEST_DATA[testIndex].m_cardType, TEST_DATA[testIndex].m_cardNumber, TEST_DATA[testIndex].m_cardNumberLen);
			const CCard card(varCard);

			personalBanker.CreateTransfer(CUtilities::GetTickCount());

			personalBanker.SetTransferCardNumber(&card, CUtilities::GetCurrentTime());
		}

		CCard varCard(CUtilities::GetCurrentTime(), CUtilities::GetCurrentTime());
		varCard.SetCardInfo(TEST_DATA[testIndex].m_cardType, TEST_DATA[testIndex].m_cardID, TEST_DATA[testIndex].m_cardIDLen);
		const CCard card(varCard);

		// Test.
		bool hasTransferForDifferentCardNumberResult = personalBanker.HasTransferForDifferentCardNumber(card);

		{
			// Report.
			bool areEqual = hasTransferForDifferentCardNumberResult == TEST_DATA[testIndex].m_hasTransferForDifferentCardNumberExpected;
			std::string failedMsg;
			failedMsg = FormatString(_T("FAILED result=%d expected=%d testIndex=%d in %s"),
				(int)hasTransferForDifferentCardNumberResult,
				(int)TEST_DATA[testIndex].m_hasTransferForDifferentCardNumberExpected,
				(int)testIndex,
				static_cast<LPCTSTR>(__TFUNCTION__));
			ASSERT_TRUE(areEqual) << failedMsg;
		}

		// Cleanup.
	}
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::ProcessBalancesFromEGM()
/// </summary>
TEST_F(PersonalBanker2, ProcessBalancesFromEGMTest)
{
	typedef struct
	{
		// Inputs.
		DWORD cashOnGame;
		DWORD promoOnGame;
		bool isPoller_12_11_6000_or_Higher;
		bool guestPlayer;
		bool canTransferOff;
		bool callToGameFailed;
		bool autoInProcess;
		bool createInfo;

		// Expected results.
		PersonalBankerAction actionExpected;
	} TestDataType;

	const TestDataType TEST_DATA[] =
	{
		{ 0, 1, true, false, true, false, true, false, PBActionStartTransferOnly},
		{ 0, 1, true, true, true, false, true, false, PBActionStartTransferOnly },
		{ 1, 0, true, false, true, false, true, false, PBActionStartTransferOnly },
		{ 1, 0, true, true, true, false, true, false, PBActionSendAutoTransferCancelledToUI },
		{ 1, 0, false, false, true, false, true, false, PBActionProcessRequestPlayerResume },
		{ 0, 0, true, false, false, false, true, false, PBActionProcessRequestPlayerResume },
		{ 0, 0, true, false, true, true, true, false, PBActionProcessRequestPlayerResume},
		{ 0, 0, true, false, false, false, false, true, PBActionSendPersonalBankerInfoToUI},
		{ 0, 0, true, true, false, false, true, false, PBActionProcessRequestPlayerResume },
		{ 0, 0, true, true, true, true, true, false, PBActionProcessRequestPlayerResume },
		{ 0, 0, true, true, false, false, false, true, PBActionSendPersonalBankerInfoToUI },
	};

	CPersonalBanker varPersonalBanker;
	CConfig config(false, "");
	config.SetAutoUploadCreditsGlobal(true);
	config.SetSenOption2Raw(config.GetSenOption2Raw() + SO2_NCEP_AUTO_UPLOAD);
	config.SetSenOption2Raw(config.GetSenOption2Raw() + SO2_NCEP_OUT_ENABLED);
	config.SetSenOption2Raw(config.GetSenOption2Raw() + SO2_WAT_AUTOPLOAD);
	config.SetPBTPromoEnabled_Global(true);
	config.SetPBFromGameEnabled(true);
	config.SetPBTCashEnabled_Global(true);
	for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
	{
		config.SetPoller_12_11_6000_or_Higher(TEST_DATA[testIndex].isPoller_12_11_6000_or_Higher);
		varPersonalBanker.SetGuestPlayer(TEST_DATA[testIndex].guestPlayer);
		if(TEST_DATA[testIndex].createInfo)
		{
			CPersonalBankerInfo* pbInfo(new CPersonalBankerInfo());
			varPersonalBanker.CreateTransfer(CUtilities::GetTickCount());
			varPersonalBanker.ProcessPersonalBankerInfo(pbInfo, config, false, false);
			delete pbInfo;
		}

		varPersonalBanker.SetAutoInProcess(TEST_DATA[testIndex].autoInProcess);
		// Perform operations to be tested.
		varPersonalBanker.ProcessBalancesFromEGM(TEST_DATA[testIndex].cashOnGame,
			0,
			TEST_DATA[testIndex].promoOnGame,
			TEST_DATA[testIndex].canTransferOff,
			TEST_DATA[testIndex].callToGameFailed,
			config);

		// Test.
		const PersonalBankerAction actionResult(varPersonalBanker.GetAction());
		{
			// Repor.
			bool areEqual = actionResult == TEST_DATA[testIndex].actionExpected;
			std::string failedMsg;
			failedMsg = FormatString(_T("FAILED index=%d actionResult=%d expected=%d in %s"),
				testIndex,
				(int)actionResult,
				(int)TEST_DATA[testIndex].actionExpected,
				static_cast<LPCTSTR>(__TFUNCTION__));
			ASSERT_TRUE(areEqual) << failedMsg;
		}
	}
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::PerformTimedEvents()
/// </summary>
TEST_F(PersonalBanker2, PerformTimedEvents1Test)
{
	// Test data.
	const bool haveInternalsToGCFQueue(false);
	const bool haveEmployee1(false);
	const PersonalBankerAction actionExpected(PBActionNone);
	
	// Setup.
	CPersonalBanker varPersonalBanker;
	CConfig config(false, "");

	// Perform operations to be tested.
	varPersonalBanker.PerformTimedEvents(haveInternalsToGCFQueue,
		haveEmployee1,
		config,
		CUtilities::GetTickCount());
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

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::ProcessPersonalBankerInfo()
/// </summary>
TEST_F(PersonalBanker2, ProcessPersonalBankerInfo1Test)
{
	// Test data.
	const bool gameOnline(false);
	const bool bIsEFT(false);
	const PersonalBankerAction actionExpected(PBActionNone);
	
	// Setup.
	CPersonalBanker varPersonalBanker;
	CPersonalBankerInfo pbInfo;
	CConfig config(false, "");

	// Perform operations to be tested.
	varPersonalBanker.ProcessPersonalBankerInfo(&pbInfo,
		config,
		gameOnline,
		bIsEFT);
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

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::EndActivePlayerSession()
/// </summary>
TEST_F(PersonalBanker2, EndActivePlayerSession1Test)
{
	// Test data.
	const PersonalBankerAction actionExpected(PBActionNone);
	
	// Setup.
	CPersonalBanker varPersonalBanker;
	CPersonalBankerInfo pbInfo;
	CConfig config(false, "");

	// Perform operations to be tested.
	varPersonalBanker.EndActivePlayerSession(config);
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

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::EndActivePlayerSession()
/// </summary>
TEST_F(PersonalBanker2, EndActivePlayerSessionWhenStateIsEqualToPBStateInfoReceivedTest)
{
	// Test data.
	const PersonalBankerAction actionExpected(PBActionReleaseSession);
	CPersonalBanker varPersonalBanker;
	CPersonalBankerInfo pbInfo;
	CConfig config(false, "");

	// Setup
	varPersonalBanker.CreateTransfer(CUtilities::GetTickCount());
	varPersonalBanker.SetState(PBStateInfoReceived);
	varPersonalBanker.EndActivePlayerSession(config);			

	// Test #1.
	{
		const PersonalBankerAction actionResult(varPersonalBanker.GetAction());
				
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

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::EndActivePlayerSession()
/// </summary>
TEST_F(PersonalBanker2, EndActivePlayerSessionWhenStateIsEqualToPBStateInquireSentTest)
{
	// Test data.
	const PersonalBankerAction actionExpected(PBActionReleaseSession);
	CPersonalBanker varPersonalBanker;
	CPersonalBankerInfo pbInfo;
	CConfig config(false, "");

	// Setup.
	varPersonalBanker.CreateTransfer(CUtilities::GetTickCount());
	varPersonalBanker.SetState(PBStateInquireSent);
	varPersonalBanker.EndActivePlayerSession(config);	

	// Test
	{
		const PersonalBankerAction actionResult(varPersonalBanker.GetAction());
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

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::EndActivePlayerSession()
/// Functional test with m_pbTransfer != NULL.
/// </summary>
TEST_F(PersonalBanker2, EndActivePlayerSession_TransferNotNullTest)
{
	typedef struct
	{
		// Inputs.
		const PersonalBankerState m_pbState;

		// Expected results.
		const PersonalBankerAction m_pbActionExpected;
	} TestDataType;

	const TestDataType TEST_DATA[] =
	{
		{ PBStateUnknown, PBActionNone },
		{ PBStateInquireSent, PBActionReleaseSession },
		{ PBStateInfoReceived, PBActionReleaseSession },
		{ PBStateTransferingFromToGame, PBActionNone },
		{ PBStateTransferComplete, PBActionNone },
		{ PBStateReleaseSent, PBActionNone },
		{ PBStateWaitingForUploadToStart, PBActionNone },
		{ PBStateIdle_NotUsed, PBActionNone },
		{ PBStateSessionRequested_NotUsed, PBActionNone },
		{ PBStateAuthRequested_NotUsed, PBActionNone },
		{ PBStateAuthGranted_NotUsed, PBActionNone },
		{ PBStateAuthDenied, PBActionNone },
		{ PBStateSendingTransferTransaction_NotUsed, PBActionNone },
		{ PBStateTransferTransactionSent_NotUsed, PBActionNone },
		{ PBStateSendingRelease_NotUsed, PBActionNone },
		{ (PersonalBankerState)(PBStateSendingRelease_NotUsed + 1), PBActionNone },
		{ (PersonalBankerState)0, PBActionNone },
		{ (PersonalBankerState)INT_MAX, PBActionNone },
		//{ (PersonalBankerState)INT_MIN, PBActionNone },
		//{ (PersonalBankerState)DWORD_MAX, PBActionNone },
	};

	for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
	{
		// Setup.
		const CConfig config(false, "");

		CPersonalBanker varPb;
		varPb.CreateTransfer(CUtilities::GetTickCount());
		varPb.SetState(TEST_DATA[testIndex].m_pbState);
		varPb.ClearPersonalBankerAction();

		// Perform operations to be tested.
		varPb.EndActivePlayerSession(config);
		const CPersonalBanker &pb(varPb);

		// Test #1.
		const PersonalBankerAction pbAction = pb.GetAction();

		{
			const bool areEqual = pbAction == TEST_DATA[testIndex].m_pbActionExpected;

			std::string failedMsg;
			failedMsg = FormatString(_T("FAILED pbAction=%d expected=%d testIndex=%d in %s(%d)"),
				(int)pbAction,
				(int)TEST_DATA[testIndex].m_pbActionExpected,
				(int)testIndex,
				static_cast<LPCTSTR>(__TFUNCTION__), __LINE__);
			ASSERT_TRUE(areEqual) << failedMsg;
		}
	}
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::EndActivePlayerSession()
/// Functional test with m_pbTransfer == NULL
/// and CPersonalBankerTransfer::DoesPersonalBankerTransferExistInNVRAM() set to return false.
/// </summary>
TEST_F(PersonalBanker2, EndActivePlayerSession_TransferNotExistTest)
{
	typedef struct
	{
		// Inputs.
		const PersonalBankerState m_pbState;

		// Expected results.
		const PersonalBankerAction m_pbActionExpected;
	} TestDataType;

	const TestDataType TEST_DATA[] =
	{
		{ PBStateUnknown, PBActionNone },
		{ PBStateInquireSent, PBActionNone },
		{ PBStateInfoReceived, PBActionNone },
		{ PBStateTransferingFromToGame, PBActionNone },
		{ PBStateTransferComplete, PBActionNone },
		{ PBStateReleaseSent, PBActionNone },
		{ PBStateWaitingForUploadToStart, PBActionNone },
		{ PBStateIdle_NotUsed, PBActionNone },
		{ PBStateSessionRequested_NotUsed, PBActionNone },
		{ PBStateAuthRequested_NotUsed, PBActionNone },
		{ PBStateAuthGranted_NotUsed, PBActionNone },
		{ PBStateAuthDenied, PBActionNone },
		{ PBStateSendingTransferTransaction_NotUsed, PBActionNone },
		{ PBStateTransferTransactionSent_NotUsed, PBActionNone },
		{ PBStateSendingRelease_NotUsed, PBActionNone },
		{ (PersonalBankerState)(PBStateSendingRelease_NotUsed + 1), PBActionNone },
		{ (PersonalBankerState)0, PBActionNone },
		{ (PersonalBankerState)INT_MAX, PBActionNone },
		{ (PersonalBankerState)INT_MIN, PBActionNone },
		{ (PersonalBankerState)DWORD_MAX, PBActionNone },
	};

	// Test data -- more inputs, NOT dependent on test iteration.
	const bool transferBuildFromNVRAM = false;

	// Test data -- more expected results, NOT dependent on test iteration.
	const bool pbTransferInNvramExpected(false);

	// Set CPersonalBankerTransfer::DoesPersonalBankerTransferExistInNVRAM() to return false,
	// by calling CPersonalBankerTransfer::~CPersonalBankerTransfer() with m_type == MEMORY_NVRAM.
	{
		CPersonalBankerTransfer pbt(CUtilities::GetTickCount(), MEMORY_NVRAM, transferBuildFromNVRAM);
	}

	BYTE varInactiveCheckByte((BYTE)~NVRAM_PB_TRANSFER_CHECKBYTE_VALUE);
	CNVRAMinator nvram;
	nvram.SetData(NVRAM_PB_TRANSFER_CHECKBYTE_OFFSET, &varInactiveCheckByte, sizeof(varInactiveCheckByte));

	for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
	{
		// Setup.
		const CConfig config(false, "");

		CPersonalBanker varPb;
		varPb.DoneWithTransfer();
		varPb.SetState(TEST_DATA[testIndex].m_pbState);
		varPb.ClearPersonalBankerAction();

		// Test #A -- verify setup.
		{
			const bool pbTransferInNvram = CPersonalBankerTransfer::DoesPersonalBankerTransferExistInNVRAM();

			const bool areEqual = pbTransferInNvram == pbTransferInNvramExpected;
			std::string failedMsg;
			failedMsg = FormatString(_T("FAILED pbTransferInNvram=%d expected=%d testIndex=%d in %s(%d)"),
				(int)pbTransferInNvram,
				(int)pbTransferInNvramExpected,
				(int)testIndex,
				static_cast<LPCTSTR>(__TFUNCTION__), __LINE__);
			ASSERT_TRUE(areEqual) << failedMsg;
		}

		// Perform operations to be tested.
		varPb.EndActivePlayerSession(config);
		const CPersonalBanker &pb(varPb);

		// Test #1.
		const PersonalBankerAction pbAction = pb.GetAction();

		{
			const bool areEqual = pbAction == TEST_DATA[testIndex].m_pbActionExpected;

			std::string failedMsg;
			failedMsg = FormatString(_T("FAILED pbAction=%d expected=%d testIndex=%d in %s(%d)"),
				(int)pbAction,
				(int)TEST_DATA[testIndex].m_pbActionExpected,
				(int)testIndex,
				static_cast<LPCTSTR>(__TFUNCTION__), __LINE__);
			ASSERT_TRUE(areEqual) << failedMsg;
		}
	}
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::EndActivePlayerSession()
/// Functional test with m_pbTransfer == NULL
/// and CPersonalBankerTransfer::DoesPersonalBankerTransferExistInNVRAM() set to return true.
/// and CPersonalBankerInfo::DoesPersonalBankerInfoExistInNVRAM() set to return false.
/// </summary>
TEST_F(PersonalBanker2, EndActivePlayerSession_InfoNotExistTest)
{
	typedef struct
	{
		// Inputs.
		const PersonalBankerState m_pbState;

		// Expected results.
		const PersonalBankerAction m_pbActionExpected;
	} TestDataType;

	const TestDataType TEST_DATA[] =
	{
		{ PBStateUnknown, PBActionNone },
		{ PBStateInquireSent, PBActionReleaseSession },
		{ PBStateInfoReceived, PBActionReleaseSession },
		{ PBStateTransferingFromToGame, PBActionNone },
		{ PBStateTransferComplete, PBActionNone },
		{ PBStateReleaseSent, PBActionNone },
		{ PBStateWaitingForUploadToStart, PBActionNone },
		{ PBStateIdle_NotUsed, PBActionNone },
		{ PBStateSessionRequested_NotUsed, PBActionNone },
		{ PBStateAuthRequested_NotUsed, PBActionNone },
		{ PBStateAuthGranted_NotUsed, PBActionNone },
		{ PBStateAuthDenied, PBActionNone },
		{ PBStateSendingTransferTransaction_NotUsed, PBActionNone },
		{ PBStateTransferTransactionSent_NotUsed, PBActionNone },
		{ PBStateSendingRelease_NotUsed, PBActionNone },
		{ (PersonalBankerState)(PBStateSendingRelease_NotUsed + 1), PBActionNone },
		{ (PersonalBankerState)0, PBActionNone },
		{ (PersonalBankerState)INT_MAX, PBActionNone },
		//{ (PersonalBankerState)INT_MIN, PBActionNone },
		//{ (PersonalBankerState)DWORD_MAX, PBActionNone },
	};

	// Test data -- more inputs, NOT dependent on test iteration.
	const bool transferBuildFromNVRAM = false;

	// Test data -- more expected results, NOT dependent on test iteration.
	const bool pbTransferInNvramExpected(true);
	const bool pbInfoInNvramExpected(false);

	// Setup for CPersonalBankerTransfer::DoesPersonalBankerTransferExistInNVRAM() to return true.
	CMemoryStorage::SetSprinkles();

	// Set CPersonalBankerInfo::DoesPersonalBankerInfoExistInNVRAM() to return false,
	CMemoryStorage::SetInactiveLocation(NVRAM_PB_INFO_CHECKBYTE_OFFSET, (BYTE)~NVRAM_PB_INFO_CHECKBYTE_VALUE);

	for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
	{
		// Setup.
		const CConfig config(false, "");

		CPersonalBanker varPb;
		varPb.DoneWithTransfer();
		varPb.SetState(TEST_DATA[testIndex].m_pbState);
		varPb.ClearPersonalBankerAction();

		// Setup for CPersonalBankerTransfer::DoesPersonalBankerTransferExistInNVRAM() to return true,
		// for the lifetime of this CPersonalBankerTransfer instance or until set otherwise.
		CPersonalBankerTransfer pbt(CUtilities::GetTickCount(), MEMORY_NVRAM, transferBuildFromNVRAM);

		// Test #A -- verify setup.
		{
			const bool pbTransferInNvram = CPersonalBankerTransfer::DoesPersonalBankerTransferExistInNVRAM();

			const bool areEqual = pbTransferInNvram == pbTransferInNvramExpected;
			std::string failedMsg;
			failedMsg = FormatString(_T("FAILED pbTransferInNvram=%d expected=%d testIndex=%d in %s(%d)"),
				(int)pbTransferInNvram,
				(int)pbTransferInNvramExpected,
				(int)testIndex,
				static_cast<LPCTSTR>(__TFUNCTION__), __LINE__);
			ASSERT_TRUE(areEqual) << failedMsg;
		}

		// Test #B -- verify setup.
		{
			const bool pbInfoInNvram = CPersonalBankerInfo::DoesPersonalBankerInfoExistInNVRAM();

			const bool areEqual = pbInfoInNvram == pbInfoInNvramExpected;
			std::string failedMsg;
			failedMsg = FormatString(_T("FAILED pbInfoInNvram=%d expected=%d testIndex=%d in %s(%d)"),
				(int)pbInfoInNvram,
				(int)pbInfoInNvramExpected,
				(int)testIndex,
				static_cast<LPCTSTR>(__TFUNCTION__), __LINE__);
			ASSERT_TRUE(areEqual) << failedMsg;
		}

		// Perform operations to be tested.
		varPb.EndActivePlayerSession(config);
		const CPersonalBanker &pb(varPb);

		// Test #1.
		const PersonalBankerAction pbAction = pb.GetAction();

		{
			const bool areEqual = pbAction == TEST_DATA[testIndex].m_pbActionExpected;

			std::string failedMsg;
			failedMsg = FormatString(_T("FAILED pbAction=%d expected=%d testIndex=%d in %s(%d)"),
				(int)pbAction,
				(int)TEST_DATA[testIndex].m_pbActionExpected,
				(int)testIndex,
				static_cast<LPCTSTR>(__TFUNCTION__), __LINE__);
			ASSERT_TRUE(areEqual) << failedMsg;
		}
	}
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::EndActivePlayerSession()
/// Functional test with m_pbTransfer == NULL
/// and CPersonalBankerTransfer::DoesPersonalBankerTransferExistInNVRAM() set to return true.
/// and CPersonalBankerInfo::DoesPersonalBankerInfoExistInNVRAM() set to return false.
/// </summary>
TEST_F(PersonalBanker2, EndActivePlayerSession_InfoNotExist2Test)
{
	typedef struct
	{
		// Inputs.
		const bool m_bTransferCashToGameEnabled;
		const bool m_bTransferPointsToGameEnabled;
		const bool m_bTransferPromoDollarsToGameEnabled;
		const bool m_bTransferCashFromGameEnabled;
		const bool m_bGameDenomMatches;
		const bool m_bEGMInitialized;
		const bool m_bTransferPromoFromGameEnabled;
		const bool m_bTransferPromoFromGameAutoEnabled;
		const bool m_gameOnline;
		const bool m_bIsEFT;
		const bool m_isEFT;
		const PersonalBankerState m_pbState;

		const DWORD m_minCashUp;
		const DWORD m_maxCashUp;
		const DWORD m_minPromoUp;
		const DWORD m_maxPromoUp;

		// Expected results.
		const bool m_bTransferCashToGameEnabledExpected;
		const bool m_bTransferPointsToGameEnabledExpected;
		const bool m_bTransferPromoDollarsToGameEnabledExpected;
		const bool m_bTransferCashFromGameEnabledExpected;
		const bool m_bGameDenomMatchesExpected;
		// const bool m_bEGMInitializedExpected; // 2013-06-18 Not directly accessible.
		const bool m_bTransferPromoFromGameEnabledExpected;
		// const bool m_bTransferPromoFromGameAutoEnabled; // 2013-06-18 Not publicly accessible.
		const bool m_bTransferToGameEnabledExpected; // Test m_bEGMInitialized indirectly.

		const PersonalBankerAction m_pbActionExpected;
		const bool m_infoExpected;
		const DWORD m_infoMinCashUpExpected;
		const DWORD m_infoMaxCashUpExpected;
		const DWORD m_infoMinPromoUpExpected;
		const DWORD m_infoMaxPromoUpExpected;
		const bool m_transferExpected;
		const DWORD m_transferMinCashUpExpected;
		const DWORD m_transferMaxCashUpExpected;
		const DWORD m_transferMinPromoUpExpected;
		const DWORD m_transferMaxPromoUpExpected;
	} TestDataType;

	const TestDataType TEST_DATA[] =
	{
		// Index #0.
		{ true, true, true, true, false, false, true, true, true, true, false, PBStateUnknown,
			1, 2, 3, 4,
			true, true, true, true, false, true, true,
			PBActionNone, false, 0, 0, 0, 0, true, 0, 0, 0, 0 },

		// Index #1.
		{ true, true, true, true, false, false, true, true, true, true, false, PBStateInquireSent,
			1, 2, 3, 4,
			true, true, true, true, false, true, true,
			PBActionReleaseSession, false, 0, 0, 0, 0, true, 0, 0, 0, 0 },

		// Index #2.
		{ true, true, true, true, false, false, true, true, true, true, false, PBStateInfoReceived,
			1, 2, 3, 4,
			true, true, true, true, false, true, true,
			PBActionReleaseSession, false, 0, 0, 0, 0, true, 0, 0, 0, 0 },

		// Index #3.
		{ true, true, true, true, false, false, true, true, true, true, false, PBStateTransferingFromToGame,
			1, 2, 3, 4,
			true, true, true, true, false, true, true,
			PBActionNone, false, 0, 0, 0, 0, true, 0, 0, 0, 0 },

		// Index #4.
		{ true, true, true, true, false, false, true, true, false, false, false, PBStateTransferComplete,
			1, 2, 3, 4,
			true, true, true, true, false, true, true,
			PBActionNone, false, 0, 0, 0, 0, true, 0, 0, 0, 0 },

		// Index #5.
		{ true, true, true, true, false, false, true, true, false, true, false, PBStateReleaseSent,
			1, 2, 3, 4,
			true, true, true, true, false, true, true,
			PBActionNone, false, 0, 0, 0, 0, true, 0, 0, 0, 0 },

		// Index #6.
		{ true, true, true, true, false, false, true, true, false, true, false, PBStateIdle_NotUsed,
			1, 2, 3, 4,
			true, true, true, true, false, true, true,
			PBActionNone, false, 0, 0, 0, 0, true, 0, 0, 0, 0 },

		// Index #7.
		{ true, true, true, true, false, false, true, true, true, false, false, PBStateSessionRequested_NotUsed,
			1, 2, 3, 4,
			true, true, true, true, false, true, true,
			PBActionNone, false, 0, 0, 0, 0, true, 0, 0, 0, 0 },

		// Index #8.
		{ true, true, true, true, false, false, true, true, true, true, false, PBStateAuthRequested_NotUsed,
			1, 2, 3, 4,
			true, true, true, true, false, true, true,
			PBActionNone, false, 0, 0, 0, 0, true, 0, 0, 0, 0 },

		// Index #9.
		{ true, true, true, true, false, false, true, true, false, false, true, PBStateAuthGranted_NotUsed,
			1, 2, 3, 4,
			true, true, true, true, false, true, true,
			PBActionNone, false, 0, 0, 0, 0, true, 0, 0, 0, 0 },

		// Index #10.
		{ true, true, true, true, false, false, true, true, false, true, true, PBStateAuthDenied,
			1, 2, 3, 4,
			true, true, true, true, false, true, true,
			PBActionNone, false, 0, 0, 0, 0, true, 0, 0, 0, 0 },

		// Index #11.
		{ true, true, true, true, false, false, true, true, true, false, true, PBStateSendingTransferTransaction_NotUsed,
			1, 2, 3, 4,
			true, true, true, true, false, true, true,
			PBActionNone, false, 0, 0, 0, 0, true, 0, 0, 0, 0 },

		// Index #12.
		{ true, true, true, true, false, false, true, true, true, true, true, PBStateTransferTransactionSent_NotUsed,
			1, 2, 3, 4,
			true, true, true, true, false, true, true,
			PBActionNone, false, 0, 0, 0, 0, true, 0, 0, 0, 0 },

		// Index #13.
		{ true, true, true, true, false, false, true, true, true, true, false, PBStateSendingRelease_NotUsed,
			1, 2, 3, 4,
			true, true, true, true, false, true, true,
			PBActionNone, false, 0, 0, 0, 0, true, 0, 0, 0, 0 },

		// Index #14.
		{ true, true, true, true, false, false, true, true, true, true, false, (PersonalBankerState)(PBStateSendingRelease_NotUsed + 1),
			1, 2, 3, 4,
			true, true, true, true, false, true, true,
			PBActionNone, false, 0, 0, 0, 0, true, 0, 0, 0, 0 },

		// Index #15.
		{ true, true, true, true, false, false, true, true, true, true, false, (PersonalBankerState)0,
			1, 2, 3, 4,
			true, true, true, true, false, true, true,
			PBActionNone, false, 0, 0, 0, 0, true, 0, 0, 0, 0 },

		// Index #16.
		{ true, true, true, true, false, false, true, true, true, true, false, (PersonalBankerState)INT_MAX,
			1, 2, 3, 4,
			true, true, true, true, false, true, true,
			PBActionNone, false, 0, 0, 0, 0, true, 0, 0, 0, 0 },

		//// Index #17.
		//{ true, true, true, true, false, false, true, true, true, true, false, (PersonalBankerState)INT_MIN,
		//	1, 2, 3, 4,
		//	true, true, true, true, false, true, true,
		//	PBActionNone, false, 0, 0, 0, 0, true, 0, 0, 0, 0 },

		//// Index #18.
		//{ true, true, true, true, false, false, true, true, true, true, false, (PersonalBankerState)DWORD_MAX,
		//	1, 2, 3, 4,
		//	true, true, true, true, false, true, true,
		//	PBActionNone, false, 0, 0, 0, 0, true, 0, 0, 0, 0 },
	};

	// Test data -- more inputs, NOT dependent on test iteration.
	const bool transferBuildFromNVRAM = false;
	const bool pbBuildFromNVRAM = false;
	const bool pbInfo1BuildFromNVRAM = false;
	const bool pbInfo2BuildFromNVRAM = false;

	// Test data -- more expected results, NOT dependent on test iteration.
	const bool pbTransferInNvramExpected(true);
	const bool pbInfoInNvramExpected(false);

	// Setup for CPersonalBankerTransfer::DoesPersonalBankerTransferExistInNVRAM() to return true
	/// and for CPersonalBankerInfo::DoesPersonalBankerInfoExistInNVRAM() to return true.
	CMemoryStorage::SetSprinkles();

	// Set CPersonalBankerInfo::DoesPersonalBankerInfoExistInNVRAM() to return false,
	CMemoryStorage::SetInactiveLocation(NVRAM_PB_INFO_CHECKBYTE_OFFSET, (BYTE)~NVRAM_PB_INFO_CHECKBYTE_VALUE);

	for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
	{
		// Setup.
		CConfig varConfig(false, "");
		varConfig.SetPBTCashEnabled_Global(TEST_DATA[testIndex].m_bTransferCashToGameEnabled);
		varConfig.SetPBTCashEnabled_local(TEST_DATA[testIndex].m_bTransferCashToGameEnabled);
		varConfig.SetPBTPointsEnabled_Global(TEST_DATA[testIndex].m_bTransferPointsToGameEnabled);
		varConfig.SetPBTPointsAsPromo(TEST_DATA[testIndex].m_bTransferPointsToGameEnabled);
		varConfig.SetPBTPromoEnabled_Global(TEST_DATA[testIndex].m_bTransferPromoDollarsToGameEnabled);
		varConfig.SetPBTPromoEnabled_local(TEST_DATA[testIndex].m_bTransferPromoDollarsToGameEnabled);
		varConfig.SetPBFromGameEnabled(TEST_DATA[testIndex].m_bTransferCashFromGameEnabled);
		varConfig.SetGameDenom(1);
		varConfig.SetDenomination(TEST_DATA[testIndex].m_bGameDenomMatches ? 1 : 5);
		if (TEST_DATA[testIndex].m_bTransferPromoFromGameEnabled)
		{
			varConfig.SetSenOption2Raw(varConfig.GetSenOption2Raw() + SO2_NCEP_OUT_ENABLED);
		}
		if (TEST_DATA[testIndex].m_bTransferPromoFromGameAutoEnabled)
		{
			varConfig.SetSenOption2Raw(varConfig.GetSenOption2Raw() + SO2_NCEP_AUTO_UPLOAD);
		}
		const CConfig &config(varConfig);

		CPersonalBanker varPb;
		varPb.RemoveSession();
		varPb.SetState(TEST_DATA[testIndex].m_pbState);
		varPb.ClearPersonalBankerAction();

		// Setup for CPersonalBankerTransfer::DoesPersonalBankerTransferExistInNVRAM() to return true,
		// for the lifetime of this CPersonalBankerTransfer instance or until set otherwise.
		CPersonalBankerTransfer pbt(CUtilities::GetTickCount(), MEMORY_NVRAM, transferBuildFromNVRAM);

		const CPersonalBankerInfo pbInfo;

		// Test #A -- verify setup.
		{
			const bool pbTransferInNvram = CPersonalBankerTransfer::DoesPersonalBankerTransferExistInNVRAM();

			const bool areEqual = pbTransferInNvram == pbTransferInNvramExpected;
			std::string failedMsg;
			failedMsg = FormatString(_T("FAILED pbTransferInNvram=%d expected=%d testIndex=%d in %s(%d)"),
				(int)pbTransferInNvram,
				(int)pbTransferInNvramExpected,
				(int)testIndex,
				static_cast<LPCTSTR>(__TFUNCTION__), __LINE__);
			ASSERT_TRUE(areEqual) << failedMsg;
		}

		// Test #B -- verify setup.
		{
			const bool pbInfoInNvram = CPersonalBankerInfo::DoesPersonalBankerInfoExistInNVRAM();

			const bool areEqual = pbInfoInNvram == pbInfoInNvramExpected;
			std::string failedMsg;
			failedMsg = FormatString(_T("FAILED pbInfoInNvram=%d expected=%d testIndex=%d in %s(%d)"),
				(int)pbInfoInNvram,
				(int)pbInfoInNvramExpected,
				(int)testIndex,
				static_cast<LPCTSTR>(__TFUNCTION__), __LINE__);
			ASSERT_TRUE(areEqual) << failedMsg;
		}

		// Perform operations to be tested.
		varPb.EndActivePlayerSession(config);
		const CPersonalBanker &pb(varPb);

		// Test #1.
		const CPersonalBankerInfo *pbInfoResult = pb.GetInfoCopy();

		{
			bool areEqual = (pbInfoResult != NULL) == TEST_DATA[testIndex].m_infoExpected;

			std::string failedMsg;
			failedMsg = FormatString(_T("FAILED pbInfoResult=%p expected=%s testIndex=%d in %s"),
				(const void *)pbInfoResult,
				static_cast<LPCTSTR>(TEST_DATA[testIndex].m_infoExpected ? _T("NOT NULL") :  _T("NULL")),
				(int)testIndex,
				static_cast<LPCTSTR>(__TFUNCTION__));
			ASSERT_TRUE(areEqual) << failedMsg;
		}

		if (TEST_DATA[testIndex].m_infoExpected)
		{
			// Test #2.
			{
				const DWORD infoMinCashUp = pbInfoResult->GetMinimumCashUp();

				bool areEqual = infoMinCashUp == TEST_DATA[testIndex].m_infoMinCashUpExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED infoMinCashUp=%u expected=%u testIndex=%d in %s"),
					(int)infoMinCashUp,
					(int)TEST_DATA[testIndex].m_infoMinCashUpExpected,
					(int)testIndex,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #3.
			{
				const DWORD infoMaxCashUp = pbInfoResult->GetMaximumCashUp();

				bool areEqual = infoMaxCashUp == TEST_DATA[testIndex].m_infoMaxCashUpExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED infoMaxCashUp=%u expected=%u testIndex=%d in %s"),
					(int)infoMaxCashUp,
					(int)TEST_DATA[testIndex].m_infoMaxCashUpExpected,
					(int)testIndex,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #4.
			{
				const DWORD infoMinPromoUp = pbInfoResult->GetMinimumPromoUp();

				bool areEqual = infoMinPromoUp == TEST_DATA[testIndex].m_infoMinPromoUpExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED infoMinPromoUp=%u expected=%u testIndex=%d in %s"),
					(int)infoMinPromoUp,
					(int)TEST_DATA[testIndex].m_infoMinPromoUpExpected,
					(int)testIndex,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #5.
			{
				const DWORD infoMaxPromoUp = pbInfoResult->GetMaximumPromoUp();

				bool areEqual = infoMaxPromoUp == TEST_DATA[testIndex].m_infoMaxPromoUpExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED infoMaxPromoUp=%u expected=%u testIndex=%d in %s"),
					(int)infoMaxPromoUp,
					(int)TEST_DATA[testIndex].m_infoMaxPromoUpExpected,
					(int)testIndex,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #6.
			{
				const bool bTransferCashToGameEnabled = pbInfoResult->IsTransferCashToGameEnabled();

				bool areEqual = bTransferCashToGameEnabled == TEST_DATA[testIndex].m_bTransferCashToGameEnabledExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED bTransferCashToGameEnabled=%d expected=%d testIndex=%d in %s"),
					(int)bTransferCashToGameEnabled,
					(int)TEST_DATA[testIndex].m_bTransferCashToGameEnabledExpected,
					(int)testIndex,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #7.
			{
				const bool bTransferPointsToGameEnabled = pbInfoResult->IsTransferPointsToGameEnabled();

				bool areEqual = bTransferPointsToGameEnabled == TEST_DATA[testIndex].m_bTransferPointsToGameEnabledExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED bTransferPointsToGameEnabled=%d expected=%d testIndex=%d (%d %d %d %d %d %d) in %s"),
					(int)bTransferPointsToGameEnabled,
					(int)TEST_DATA[testIndex].m_bTransferPointsToGameEnabledExpected,
					(int)testIndex,
					(int)TEST_DATA[testIndex].m_bTransferPointsToGameEnabled,
					(int)config.PBTPointsEnabledGlobal(),
					(int)config.PBTPointsEnabled(),
					(int)TEST_DATA[testIndex].m_bIsEFT,
					(int)config.PBTPointsAsPromo(),
					(int)pbInfo.IsTransferPointsToGameEnabled(),
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #8.
			{
				const bool bTransferPromoDollarsToGameEnabled = pbInfoResult->IsTransferPromoDollarsToGameEnabled();

				bool areEqual = bTransferPromoDollarsToGameEnabled == TEST_DATA[testIndex].m_bTransferPromoDollarsToGameEnabledExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED bTransferPromoDollarsToGameEnabled=%d expected=%d testIndex=%d in %s"),
					(int)bTransferPromoDollarsToGameEnabled,
					(int)TEST_DATA[testIndex].m_bTransferPromoDollarsToGameEnabledExpected,
					(int)testIndex,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #9.
			{
				const bool bTransferCashFromGameEnabled = pbInfoResult->IsTransferCashFromGameEnabled();

				bool areEqual = bTransferCashFromGameEnabled == TEST_DATA[testIndex].m_bTransferCashFromGameEnabledExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED bTransferCashFromGameEnabled=%d expected=%d testIndex=%d in %s"),
					(int)bTransferCashFromGameEnabled,
					(int)TEST_DATA[testIndex].m_bTransferCashFromGameEnabledExpected,
					(int)testIndex,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #10.
			{
				const bool bGameDenomMatches = pbInfoResult->DoesGameDenomMatch();

				bool areEqual = bGameDenomMatches == TEST_DATA[testIndex].m_bGameDenomMatchesExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED bGameDenomMatches=%d expected=%d testIndex=%d in %s"),
					(int)bGameDenomMatches,
					(int)TEST_DATA[testIndex].m_bGameDenomMatchesExpected,
					(int)testIndex,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #11.
			{
				const bool bTransferPromoFromGameEnabled = pbInfoResult->IsTransferPromoFromGameEnabled();

				bool areEqual = bTransferPromoFromGameEnabled == TEST_DATA[testIndex].m_bTransferPromoFromGameEnabledExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED bTransferPromoFromGameEnabled=%d expected=%d testIndex=%d in %s"),
					(int)bTransferPromoFromGameEnabled,
					(int)TEST_DATA[testIndex].m_bTransferPromoFromGameEnabledExpected,
					(int)testIndex,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #12.
			{
				const bool bTransferToGameEnabled = pbInfoResult->IsTransferToGameEnabled();

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
		}

		// Test #14.
		const CPersonalBankerTransfer *pbTransferResult = pb.GetTransferCopy();

		{
			bool areEqual = (pbTransferResult != NULL) == TEST_DATA[testIndex].m_transferExpected;

			std::string failedMsg;
			failedMsg = FormatString(_T("FAILED pbTransferResult=%p expected=%s testIndex=%d in %s(%d)"),
				(const void *)pbTransferResult,
				static_cast<LPCTSTR>(TEST_DATA[testIndex].m_transferExpected ? _T("NOT NULL") :  _T("NULL")),
				(int)testIndex,
				static_cast<LPCTSTR>(__TFUNCTION__), __LINE__);
			ASSERT_TRUE(areEqual) << failedMsg;

		}

		if (TEST_DATA[testIndex].m_transferExpected)
		{
			// Test #18.
			{
				const DWORD transferMinCashUp = pbTransferResult->GetMinCashUp();

				bool areEqual = transferMinCashUp == TEST_DATA[testIndex].m_transferMinCashUpExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED transferMinCashUp=%u expected=%u testIndex=%d in %s"),
					(int)transferMinCashUp,
					(int)TEST_DATA[testIndex].m_transferMinCashUpExpected,
					(int)testIndex,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #19.
			{
				const DWORD transferMaxCashUp = pbTransferResult->GetMaxCashUp();

				bool areEqual = transferMaxCashUp == TEST_DATA[testIndex].m_transferMaxCashUpExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED transferMaxCashUp=%u expected=%u testIndex=%d in %s"),
					(int)transferMaxCashUp,
					(int)TEST_DATA[testIndex].m_transferMaxCashUpExpected,
					(int)testIndex,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #20.
			{
				const DWORD transferMinPromoUp = pbTransferResult->GetMinPromoUp();

				bool areEqual = transferMinPromoUp == TEST_DATA[testIndex].m_transferMinPromoUpExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED transferMinPromoUp=%u expected=%u testIndex=%d in %s"),
					(int)transferMinPromoUp,
					(int)TEST_DATA[testIndex].m_transferMinPromoUpExpected,
					(int)testIndex,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #21.
			{
				const DWORD transferMaxPromoUp = pbTransferResult->GetMaxPromoUp();

				bool areEqual = transferMaxPromoUp == TEST_DATA[testIndex].m_transferMaxPromoUpExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED transferMaxPromoUp=%u expected=%u testIndex=%d in %s"),
					(int)transferMaxPromoUp,
					(int)TEST_DATA[testIndex].m_transferMaxPromoUpExpected,
					(int)testIndex,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}
		}

		// Test #1.
		const PersonalBankerAction pbAction = pb.GetAction();

		{
			const bool areEqual = pbAction == TEST_DATA[testIndex].m_pbActionExpected;

			std::string failedMsg;
			failedMsg = FormatString(_T("FAILED pbAction=%d expected=%d testIndex=%d in %s(%d)"),
				(int)pbAction,
				(int)TEST_DATA[testIndex].m_pbActionExpected,
				(int)testIndex,
				static_cast<LPCTSTR>(__TFUNCTION__), __LINE__);
			ASSERT_TRUE(areEqual) << failedMsg;
		}
	}
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::EndActivePlayerSession()
/// Functional test with m_pbTransfer == NULL
/// and CPersonalBankerTransfer::DoesPersonalBankerTransferExistInNVRAM() set to return true.
/// and CPersonalBankerInfo::DoesPersonalBankerInfoExistInNVRAM() set to return true.
/// </summary>
TEST_F(PersonalBanker2, EndActivePlayerSession_InfoExistsTest)
{
	typedef struct
	{
		// Inputs.
		const bool m_bTransferCashToGameEnabled;
		const bool m_bTransferPointsToGameEnabled;
		const bool m_bTransferPromoDollarsToGameEnabled;
		const bool m_bTransferCashFromGameEnabled;
		const bool m_bGameDenomMatches;
		const bool m_bEGMInitialized;
		const bool m_bTransferPromoFromGameEnabled;
		const bool m_bTransferPromoFromGameAutoEnabled;
		const bool m_gameOnline;
		const bool m_bIsEFT;
		const bool m_isEFT;
		const PersonalBankerState m_pbState;

		const DWORD m_minCashUp;
		const DWORD m_maxCashUp;
		const DWORD m_minPromoUp;
		const DWORD m_maxPromoUp;

		// Expected results.
		const bool m_bTransferCashToGameEnabledExpected;
		const bool m_bTransferPointsToGameEnabledExpected;
		const bool m_bTransferPromoDollarsToGameEnabledExpected;
		const bool m_bTransferCashFromGameEnabledExpected;
		const bool m_bGameDenomMatchesExpected;
		// const bool m_bEGMInitializedExpected; // 2013-06-18 Not directly accessible.
		const bool m_bTransferPromoFromGameEnabledExpected;
		// const bool m_bTransferPromoFromGameAutoEnabled; // 2013-06-18 Not publicly accessible.
		const bool m_bTransferToGameEnabledExpected; // Test m_bEGMInitialized indirectly.

		const PersonalBankerAction m_pbActionExpected;
		const bool m_infoExpected;
		const DWORD m_infoMinCashUpExpected;
		const DWORD m_infoMaxCashUpExpected;
		const DWORD m_infoMinPromoUpExpected;
		const DWORD m_infoMaxPromoUpExpected;
		const bool m_transferExpected;
		const DWORD m_transferMinCashUpExpected;
		const DWORD m_transferMaxCashUpExpected;
		const DWORD m_transferMinPromoUpExpected;
		const DWORD m_transferMaxPromoUpExpected;
	} TestDataType;

	const TestDataType TEST_DATA[] =
	{
		// Index #0.
		{ true, true, true, true, false, false, true, true, true, true, false, PBStateUnknown,
			1, 2, 3, 4,
			true, true, true, true, false, true, true,
			PBActionNone, true, 1, 2, 3, 4, true, 1, 2, 3, 4 },

		// Index #1.
		{ true, true, true, true, false, false, true, true, true, true, false, PBStateInquireSent,
			1, 2, 3, 4,
			true, true, true, true, false, true, true,
			PBActionReleaseSession, true, 1, 2, 3, 4, true, 1, 2, 3, 4 },

		// Index #2.
		{ true, true, true, true, false, false, true, true, true, true, false, PBStateInfoReceived,
			1, 2, 3, 4,
			true, true, true, true, false, true, true,
			PBActionReleaseSession, true, 1, 2, 3, 4, true, 1, 2, 3, 4 },

		// Index #3.
		{ true, true, true, true, false, false, true, true, true, true, false, PBStateTransferingFromToGame,
			1, 2, 3, 4,
			true, true, true, true, false, true, true,
			PBActionNone, true, 1, 2, 3, 4, true, 1, 2, 3, 4 },

		// Index #4.
		{ true, true, true, true, false, false, true, true, false, false, false, PBStateTransferComplete,
			1, 2, 3, 4,
			true, true, true, true, false, true, true,
			PBActionNone, true, 1, 2, 3, 4, true, 1, 2, 3, 4 },

		// Index #5.
		{ true, true, true, true, false, false, true, true, false, true, false, PBStateReleaseSent,
			1, 2, 3, 4,
			true, true, true, true, false, true, true,
			PBActionNone, true, 1, 2, 3, 4, true, 1, 2, 3, 4 },

		// Index #6.
		{ true, true, true, true, false, false, true, true, false, true, false, PBStateIdle_NotUsed,
			1, 2, 3, 4,
			true, true, true, true, false, true, true,
			PBActionNone, true, 1, 2, 3, 4, true, 1, 2, 3, 4 },

		// Index #7.
		{ true, true, true, true, false, false, true, true, true, false, false, PBStateSessionRequested_NotUsed,
			1, 2, 3, 4,
			true, true, true, true, false, true, true,
			PBActionNone, true, 1, 2, 3, 4, true, 1, 2, 3, 4 },

		// Index #8.
		{ true, true, true, true, false, false, true, true, true, true, false, PBStateAuthRequested_NotUsed,
			1, 2, 3, 4,
			true, true, true, true, false, true, true,
			PBActionNone, true, 1, 2, 3, 4, true, 1, 2, 3, 4 },

		// Index #9.
		{ true, true, true, true, false, false, true, true, false, false, true, PBStateAuthGranted_NotUsed,
			1, 2, 3, 4,
			true, true, true, true, false, true, true,
			PBActionNone, true, 1, 2, 3, 4, true, 1, 2, 3, 4 },

		// Index #10.
		{ true, true, true, true, false, false, true, true, false, true, true, PBStateAuthDenied,
			1, 2, 3, 4,
			true, true, true, true, false, true, true,
			PBActionNone, true, 1, 2, 3, 4, true, 1, 2, 3, 4 },

		// Index #11.
		{ true, true, true, true, false, false, true, true, true, false, true, PBStateSendingTransferTransaction_NotUsed,
			1, 2, 3, 4,
			true, true, true, true, false, true, true,
			PBActionNone, true, 1, 2, 3, 4, true, 1, 2, 3, 4 },

		// Index #12.
		{ true, true, true, true, false, false, true, true, true, true, true, PBStateTransferTransactionSent_NotUsed,
			1, 2, 3, 4,
			true, true, true, true, false, true, true,
			PBActionNone, true, 1, 2, 3, 4, true, 1, 2, 3, 4 },

		// Index #13.
		{ true, true, true, true, false, false, true, true, true, true, false, PBStateSendingRelease_NotUsed,
			1, 2, 3, 4,
			true, true, true, true, false, true, true,
			PBActionNone, true, 1, 2, 3, 4, true, 1, 2, 3, 4 },

		// Index #14.
		{ true, true, true, true, false, false, true, true, true, true, false, (PersonalBankerState)(PBStateSendingRelease_NotUsed + 1),
			1, 2, 3, 4,
			true, true, true, true, false, true, true,
			PBActionNone, true, 1, 2, 3, 4, true, 1, 2, 3, 4 },

		// Index #15.
		{ true, true, true, true, false, false, true, true, true, true, false, (PersonalBankerState)0,
			1, 2, 3, 4,
			true, true, true, true, false, true, true,
			PBActionNone, true, 1, 2, 3, 4, true, 1, 2, 3, 4 },

		// Index #16.
		{ true, true, true, true, false, false, true, true, true, true, false, (PersonalBankerState)INT_MAX,
			1, 2, 3, 4,
			true, true, true, true, false, true, true,
			PBActionNone, true, 1, 2, 3, 4, true, 1, 2, 3, 4 },

		//// Index #17.
		//{ true, true, true, true, false, false, true, true, true, true, false, (PersonalBankerState)INT_MIN,
		//	1, 2, 3, 4,
		//	true, true, true, true, false, true, true,
		//	PBActionNone, true, 1, 2, 3, 4, true, 1, 2, 3, 4 },

		//// Index #18.
		//{ true, true, true, true, false, false, true, true, true, true, false, (PersonalBankerState)DWORD_MAX,
		//	1, 2, 3, 4,
		//	true, true, true, true, false, true, true,
		//	PBActionNone, true, 1, 2, 3, 4, true, 1, 2, 3, 4 },
	};

	// Test data -- more inputs, NOT dependent on test iteration.
	const bool transferBuildFromNVRAM = false;
	const bool pbBuildFromNVRAM = false;
	const bool pbInfo1BuildFromNVRAM = false;
	const bool pbInfo2BuildFromNVRAM = false;

	// Test data -- more expected results, NOT dependent on test iteration.
	const bool pbTransferInNvramExpected(true);
	const bool pbInfoInNvramExpected(true);

	// Setup for CPersonalBankerTransfer::DoesPersonalBankerTransferExistInNVRAM() to return true
	/// and for CPersonalBankerInfo::DoesPersonalBankerInfoExistInNVRAM() to return true.
	CMemoryStorage::SetSprinkles();

	for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
	{
		// Setup.
		CConfig varConfig(false, "");
		varConfig.SetPBTCashEnabled_Global(TEST_DATA[testIndex].m_bTransferCashToGameEnabled);
		varConfig.SetPBTCashEnabled_local(TEST_DATA[testIndex].m_bTransferCashToGameEnabled);
		varConfig.SetPBTPointsEnabled_Global(TEST_DATA[testIndex].m_bTransferPointsToGameEnabled);
		varConfig.SetPBTPointsAsPromo(TEST_DATA[testIndex].m_bTransferPointsToGameEnabled);
		varConfig.SetPBTPromoEnabled_Global(TEST_DATA[testIndex].m_bTransferPromoDollarsToGameEnabled);
		varConfig.SetPBTPromoEnabled_local(TEST_DATA[testIndex].m_bTransferPromoDollarsToGameEnabled);
		varConfig.SetPBFromGameEnabled(TEST_DATA[testIndex].m_bTransferCashFromGameEnabled);
		varConfig.SetGameDenom(1);
		varConfig.SetDenomination(TEST_DATA[testIndex].m_bGameDenomMatches ? 1 : 5);
		if (TEST_DATA[testIndex].m_bTransferPromoFromGameEnabled)
		{
			varConfig.SetSenOption2Raw(varConfig.GetSenOption2Raw() + SO2_NCEP_OUT_ENABLED);
		}
		if (TEST_DATA[testIndex].m_bTransferPromoFromGameAutoEnabled)
		{
			varConfig.SetSenOption2Raw(varConfig.GetSenOption2Raw() + SO2_NCEP_AUTO_UPLOAD);
		}				
		const CConfig &config(varConfig);

		CPersonalBanker varPb;
		varPb.RemoveSession();
		varPb.SetState(TEST_DATA[testIndex].m_pbState);
		varPb.ClearPersonalBankerAction();

		// Setup for CPersonalBankerTransfer::DoesPersonalBankerTransferExistInNVRAM() to return true,
		// for the lifetime of this CPersonalBankerTransfer instance or until set otherwise.
		CPersonalBankerTransfer pbt(CUtilities::GetTickCount(), MEMORY_NVRAM, transferBuildFromNVRAM);

		// Setup for CPersonalBankerInfo::DoesPersonalBankerInfoExistInNVRAM() to return true,
		// for the lifetime of this CPersonalBankerInfo instance or until set otherwise.
		CPersonalBankerInfo varPbInfo(MEMORY_NVRAM, pbInfo1BuildFromNVRAM);
		varPbInfo.SetMaximumCashUp(TEST_DATA[testIndex].m_maxCashUp);
		varPbInfo.SetMinimumCashUp(TEST_DATA[testIndex].m_minCashUp);
		varPbInfo.SetMinimumPromoUp(TEST_DATA[testIndex].m_minPromoUp);
		varPbInfo.SetMaximumPromoUp(TEST_DATA[testIndex].m_maxPromoUp);

		// Invoke CPersonalBankerInfo assignment operator so min/max changes get saved to NVRAM.
		CPersonalBankerInfo varPbInfo2(MEMORY_NVRAM, pbInfo2BuildFromNVRAM);
		varPbInfo2 = varPbInfo;
		const CPersonalBankerInfo &pbInfo(varPbInfo2);

		// Test #A -- verify setup.
		{
			const bool pbTransferInNvram = CPersonalBankerTransfer::DoesPersonalBankerTransferExistInNVRAM();

			const bool areEqual = pbTransferInNvram == pbTransferInNvramExpected;
			std::string failedMsg;
			failedMsg = FormatString(_T("FAILED pbTransferInNvram=%d expected=%d testIndex=%d in %s(%d)"),
				(int)pbTransferInNvram,
				(int)pbTransferInNvramExpected,
				(int)testIndex,
				static_cast<LPCTSTR>(__TFUNCTION__), __LINE__);
			ASSERT_TRUE(areEqual) << failedMsg;
		}

		// Test #B -- verify setup.
		{
			const bool pbInfoInNvram = CPersonalBankerInfo::DoesPersonalBankerInfoExistInNVRAM();

			const bool areEqual = pbInfoInNvram == pbInfoInNvramExpected;
			std::string failedMsg;
			failedMsg = FormatString(_T("FAILED pbInfoInNvram=%d expected=%d testIndex=%d in %s(%d)"),
				(int)pbInfoInNvram,
				(int)pbInfoInNvramExpected,
				(int)testIndex,
				static_cast<LPCTSTR>(__TFUNCTION__), __LINE__);
			ASSERT_TRUE(areEqual) << failedMsg;
		}

		// Perform operations to be tested.
		varPb.EndActivePlayerSession(config);
		const CPersonalBanker &pb(varPb);

		// Test #1.
		const CPersonalBankerInfo *pbInfoResult = pb.GetInfoCopy();

		{
			bool areEqual = (pbInfoResult != NULL) == TEST_DATA[testIndex].m_infoExpected;

			std::string failedMsg;
			failedMsg = FormatString(_T("FAILED pbInfoResult=%p expected=%s testIndex=%d in %s"),
				(const void *)pbInfoResult,
				static_cast<LPCTSTR>(TEST_DATA[testIndex].m_infoExpected ? _T("NOT NULL") :  _T("NULL")),
				(int)testIndex,
				static_cast<LPCTSTR>(__TFUNCTION__));
			ASSERT_TRUE(areEqual) << failedMsg;
		}

		if (TEST_DATA[testIndex].m_infoExpected)
		{
			// Test #2.
			{
				const DWORD infoMinCashUp = pbInfoResult->GetMinimumCashUp();

				bool areEqual = infoMinCashUp == TEST_DATA[testIndex].m_infoMinCashUpExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED infoMinCashUp=%u expected=%u testIndex=%d in %s"),
					(int)infoMinCashUp,
					(int)TEST_DATA[testIndex].m_infoMinCashUpExpected,
					(int)testIndex,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #3.
			{
				const DWORD infoMaxCashUp = pbInfoResult->GetMaximumCashUp();

				bool areEqual = infoMaxCashUp == TEST_DATA[testIndex].m_infoMaxCashUpExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED infoMaxCashUp=%u expected=%u testIndex=%d in %s"),
					(int)infoMaxCashUp,
					(int)TEST_DATA[testIndex].m_infoMaxCashUpExpected,
					(int)testIndex,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #4.
			{
				const DWORD infoMinPromoUp = pbInfoResult->GetMinimumPromoUp();

				bool areEqual = infoMinPromoUp == TEST_DATA[testIndex].m_infoMinPromoUpExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED infoMinPromoUp=%u expected=%u testIndex=%d in %s"),
					(int)infoMinPromoUp,
					(int)TEST_DATA[testIndex].m_infoMinPromoUpExpected,
					(int)testIndex,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #5.
			{
				const DWORD infoMaxPromoUp = pbInfoResult->GetMaximumPromoUp();

				bool areEqual = infoMaxPromoUp == TEST_DATA[testIndex].m_infoMaxPromoUpExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED infoMaxPromoUp=%u expected=%u testIndex=%d in %s"),
					(int)infoMaxPromoUp,
					(int)TEST_DATA[testIndex].m_infoMaxPromoUpExpected,
					(int)testIndex,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #6.
			{
				const bool bTransferCashToGameEnabled = pbInfoResult->IsTransferCashToGameEnabled();

				bool areEqual = bTransferCashToGameEnabled == TEST_DATA[testIndex].m_bTransferCashToGameEnabledExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED bTransferCashToGameEnabled=%d expected=%d testIndex=%d in %s"),
					(int)bTransferCashToGameEnabled,
					(int)TEST_DATA[testIndex].m_bTransferCashToGameEnabledExpected,
					(int)testIndex,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #7.
			{
				const bool bTransferPointsToGameEnabled = pbInfoResult->IsTransferPointsToGameEnabled();

				bool areEqual = bTransferPointsToGameEnabled == TEST_DATA[testIndex].m_bTransferPointsToGameEnabledExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED bTransferPointsToGameEnabled=%d expected=%d testIndex=%d (%d %d %d %d %d %d) in %s"),
					(int)bTransferPointsToGameEnabled,
					(int)TEST_DATA[testIndex].m_bTransferPointsToGameEnabledExpected,
					(int)testIndex,
					(int)TEST_DATA[testIndex].m_bTransferPointsToGameEnabled,
					(int)config.PBTPointsEnabledGlobal(),
					(int)config.PBTPointsEnabled(),
					(int)TEST_DATA[testIndex].m_bIsEFT,
					(int)config.PBTPointsAsPromo(),
					(int)pbInfo.IsTransferPointsToGameEnabled(),
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #8.
			{
				const bool bTransferPromoDollarsToGameEnabled = pbInfoResult->IsTransferPromoDollarsToGameEnabled();

				bool areEqual = bTransferPromoDollarsToGameEnabled == TEST_DATA[testIndex].m_bTransferPromoDollarsToGameEnabledExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED bTransferPromoDollarsToGameEnabled=%d expected=%d testIndex=%d in %s"),
					(int)bTransferPromoDollarsToGameEnabled,
					(int)TEST_DATA[testIndex].m_bTransferPromoDollarsToGameEnabledExpected,
					(int)testIndex,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #9.
			{
				const bool bTransferCashFromGameEnabled = pbInfoResult->IsTransferCashFromGameEnabled();

				bool areEqual = bTransferCashFromGameEnabled == TEST_DATA[testIndex].m_bTransferCashFromGameEnabledExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED bTransferCashFromGameEnabled=%d expected=%d testIndex=%d in %s"),
					(int)bTransferCashFromGameEnabled,
					(int)TEST_DATA[testIndex].m_bTransferCashFromGameEnabledExpected,
					(int)testIndex,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #10.
			{
				const bool bGameDenomMatches = pbInfoResult->DoesGameDenomMatch();

				bool areEqual = bGameDenomMatches == TEST_DATA[testIndex].m_bGameDenomMatchesExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED bGameDenomMatches=%d expected=%d testIndex=%d in %s"),
					(int)bGameDenomMatches,
					(int)TEST_DATA[testIndex].m_bGameDenomMatchesExpected,
					(int)testIndex,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #11.
			{
				const bool bTransferPromoFromGameEnabled = pbInfoResult->IsTransferPromoFromGameEnabled();

				bool areEqual = bTransferPromoFromGameEnabled == TEST_DATA[testIndex].m_bTransferPromoFromGameEnabledExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED bTransferPromoFromGameEnabled=%d expected=%d testIndex=%d in %s"),
					(int)bTransferPromoFromGameEnabled,
					(int)TEST_DATA[testIndex].m_bTransferPromoFromGameEnabledExpected,
					(int)testIndex,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #12.
			{
				const bool bTransferToGameEnabled = pbInfoResult->IsTransferToGameEnabled();

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
		}

		// Test #14.
		const CPersonalBankerTransfer *pbTransferResult = pb.GetTransferCopy();

		{
			bool areEqual = (pbTransferResult != NULL) == TEST_DATA[testIndex].m_transferExpected;

			std::string failedMsg;
			failedMsg = FormatString(_T("FAILED pbTransferResult=%p expected=%s testIndex=%d in %s(%d)"),
				(const void *)pbTransferResult,
				static_cast<LPCTSTR>(TEST_DATA[testIndex].m_transferExpected ? _T("NOT NULL") :  _T("NULL")),
				(int)testIndex,
				static_cast<LPCTSTR>(__TFUNCTION__), __LINE__);
			ASSERT_TRUE(areEqual) << failedMsg;

		}

		if (TEST_DATA[testIndex].m_transferExpected)
		{
			// Test #18.
			{
				const DWORD transferMinCashUp = pbTransferResult->GetMinCashUp();

				bool areEqual = transferMinCashUp == TEST_DATA[testIndex].m_transferMinCashUpExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED transferMinCashUp=%u expected=%u testIndex=%d in %s"),
					(int)transferMinCashUp,
					(int)TEST_DATA[testIndex].m_transferMinCashUpExpected,
					(int)testIndex,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #19.
			{
				const DWORD transferMaxCashUp = pbTransferResult->GetMaxCashUp();

				bool areEqual = transferMaxCashUp == TEST_DATA[testIndex].m_transferMaxCashUpExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED transferMaxCashUp=%u expected=%u testIndex=%d in %s"),
					(int)transferMaxCashUp,
					(int)TEST_DATA[testIndex].m_transferMaxCashUpExpected,
					(int)testIndex,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #20.
			{
				const DWORD transferMinPromoUp = pbTransferResult->GetMinPromoUp();

				bool areEqual = transferMinPromoUp == TEST_DATA[testIndex].m_transferMinPromoUpExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED transferMinPromoUp=%u expected=%u testIndex=%d in %s"),
					(int)transferMinPromoUp,
					(int)TEST_DATA[testIndex].m_transferMinPromoUpExpected,
					(int)testIndex,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #21.
			{
				const DWORD transferMaxPromoUp = pbTransferResult->GetMaxPromoUp();

				bool areEqual = transferMaxPromoUp == TEST_DATA[testIndex].m_transferMaxPromoUpExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED transferMaxPromoUp=%u expected=%u testIndex=%d in %s"),
					(int)transferMaxPromoUp,
					(int)TEST_DATA[testIndex].m_transferMaxPromoUpExpected,
					(int)testIndex,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}
		}

		// Test #1.
		const PersonalBankerAction pbAction = pb.GetAction();

		{
			const bool areEqual = pbAction == TEST_DATA[testIndex].m_pbActionExpected;

			std::string failedMsg;
			failedMsg = FormatString(_T("FAILED pbAction=%d expected=%d testIndex=%d in %s(%d)"),
				(int)pbAction,
				(int)TEST_DATA[testIndex].m_pbActionExpected,
				(int)testIndex,
				static_cast<LPCTSTR>(__TFUNCTION__), __LINE__);
			ASSERT_TRUE(areEqual) << failedMsg;
		}
	}
}

/// <summary>
/// Test various CPersonalBanker functions
/// </summary>
TEST_F(PersonalBanker2, PersonalBankerFunctionsTest)
{
	// Setup.
	CPlayer* player(new CPlayer());
	CLogs* logs(NULL);
	CLogs* successLogs(NULL);
	CPersonalBanker varPersonalBanker;
	CConfig config(false, "");

	varPersonalBanker.LogTransfer(player, config, CUtilities::GetCurrentTime());

	// Test #1.
	logs = varPersonalBanker.GetLogCopy();
	{
		ASSERT_NE(logs, nullptr) << _T("Info is NULL");
	}

	successLogs = varPersonalBanker.GetSuccessLogCopy();
	{
		ASSERT_NE(successLogs, nullptr) << _T("Info is NULL");
	}
			 
	//Cleanup
	delete logs;
	delete successLogs;
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::BeginPersonalBankerSession()
/// Code coverage test for player == NULL.
/// </summary>
TEST_F(PersonalBanker2, BeginPersonalBankerSession1Test)
{
	// Test data.
	LPCTSTR pbPin = _T("1234");
	CPlayer * const player = NULL;
	const PersonalBankerAction actionExpected(PBActionNone);
	const PersonalBankerState stateExpected(PBStateUnknown);
	
	// Setup.
	CPersonalBanker varPersonalBanker;
	CConfig config(false, "");
     
    CreateInfo(varPersonalBanker);

	// Perform operations to be tested.
	varPersonalBanker.BeginPersonalBankerSession(player,
		pbPin,
		config);
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

	// Test #2.
	const PersonalBankerState stateResult(personalBanker->GetState());

	{
		// Report #2.
		bool areEqual = stateResult == stateExpected;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED stateResult=%d expected=%d in %s"),
			(int)stateResult,
			(int)stateExpected,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::BeginPersonalBankerSession()
/// Code coverage test for player != NULL and m_pbTransfer == NULL.
/// </summary>
TEST_F(PersonalBanker2, BeginPersonalBankerSession2Test)
{
	// Test data -- inputs.
	LPCTSTR pbPin = _T("1234");
	const PersonalBankerState pbState(PBStateInfoReceived);

	// Test data -- expected results.
	const PersonalBankerAction actionExpected(PBActionNone);
	const PersonalBankerState stateExpected(PBStateInquireSent);
	
	// Setup.
	CPersonalBanker varPersonalBanker;
    CreateInfo(varPersonalBanker);
	varPersonalBanker.DoneWithTransfer();
	varPersonalBanker.SetState(pbState);
	CCard *card = new CCard(CUtilities::GetCurrentTime(), CUtilities::GetCurrentTime());
	CPlayerInfo *playerInfo = new CPlayerInfo();
	CConfig config(false, "");
	const CEGMConfig egmConfig(MEMORY_NONE);
    CGames games;
	CPlayer player(card, playerInfo, config, egmConfig, CUtilities::GetCurrentTime(), false, &games);

	// Perform operations to be tested.
	varPersonalBanker.BeginPersonalBankerSession(&player,
		pbPin,
		config);
	const CPersonalBanker &personalBanker(varPersonalBanker);

	// Test #1.
	const PersonalBankerAction actionResult(personalBanker.GetAction());

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

	// Test #2.
	const PersonalBankerState stateResult(personalBanker.GetState());

	{
		// Report #2.
		bool areEqual = stateResult == stateExpected;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED stateResult=%d expected=%d in %s"),
			(int)stateResult,
			(int)stateExpected,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::BeginPersonalBankerSession()
/// Code coverage test for player != NULL and m_pbTransfer != NULL and m_state != PBStateInfoReceived.
/// </summary>
TEST_F(PersonalBanker2, BeginPersonalBankerSession3Test)
{
	// Test data.
	LPCTSTR pbPin = _T("1234");
	const PersonalBankerState pbState(PBStateInquireSent);

	// Test data -- expected results.
	const PersonalBankerAction actionExpected(PBActionNone);
	const PersonalBankerState stateExpected(PBStateInquireSent);
	
	// Setup.
	CPersonalBanker varPersonalBanker;
    CreateInfo(varPersonalBanker);
	varPersonalBanker.CreateTransfer(CUtilities::GetTickCount());
	varPersonalBanker.SetState(pbState);
	CCard *card = new CCard(CUtilities::GetCurrentTime(), CUtilities::GetCurrentTime());
	CPlayerInfo *playerInfo = new CPlayerInfo();
	CConfig config(false, "");
	const CEGMConfig egmConfig(MEMORY_NONE);
    CGames games;
	CPlayer player(card, playerInfo, config, egmConfig, CUtilities::GetCurrentTime(), false, &games);

	// Perform operations to be tested.
	varPersonalBanker.BeginPersonalBankerSession(&player,
		pbPin,
		config);
	const CPersonalBanker &personalBanker(varPersonalBanker);

	// Test #1.
	const PersonalBankerAction actionResult(personalBanker.GetAction());

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

	// Test #2.
	const PersonalBankerState stateResult(personalBanker.GetState());

	{
		// Report #2.
		bool areEqual = stateResult == stateExpected;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED stateResult=%d expected=%d in %s"),
			(int)stateResult,
			(int)stateExpected,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::BeginPersonalBankerSession()
/// Code coverage test for player != NULL and m_pbTransfer != NULL and m_state == PBStateInfoReceived.
/// </summary>
TEST_F(PersonalBanker2, BeginPersonalBankerSession4Test)
{
	// Test data -- inputs.
	LPCTSTR pbPin = _T("1234");
	const PersonalBankerState pbState(PBStateInfoReceived);

	// Test data -- expected results.
	const PersonalBankerAction actionExpected(PBActionNone);
	const PersonalBankerState stateExpected(PBStateInquireSent);
	
	// Setup.
	CPersonalBanker varPersonalBanker;
    CreateInfo(varPersonalBanker);
	varPersonalBanker.CreateTransfer(CUtilities::GetTickCount());
	varPersonalBanker.SetState(pbState);
	CCard *card = new CCard(CUtilities::GetCurrentTime(), CUtilities::GetCurrentTime());
	CPlayerInfo *playerInfo = new CPlayerInfo();
	CConfig config(false, "");
	const CEGMConfig egmConfig(MEMORY_NONE);
    CGames games;
	CPlayer player(card, playerInfo, config, egmConfig, CUtilities::GetCurrentTime(), false, &games);

	// Perform operations to be tested.
	varPersonalBanker.BeginPersonalBankerSession(&player,
		pbPin,
		config);
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

	// Test #2.
	const PersonalBankerState stateResult(personalBanker->GetState());

	{
		// Report #2.
		bool areEqual = stateResult == stateExpected;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED stateResult=%d expected=%d in %s"),
			(int)stateResult,
			(int)stateExpected,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::ClearPersonalBankerAction()
/// </summary>
TEST_F(PersonalBanker2, ClearPersonalBankerAction1Test)
{
	// Test data.
	const PersonalBankerAction actionExpected(PBActionNone);
	
	// Setup.
	CPersonalBanker varPersonalBanker;

	// Perform operations to be tested.
	varPersonalBanker.ClearPersonalBankerAction();
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

	// Cleanup.
}

#ifdef _DEBUG
/// <summary>
/// Test the implementation of:
///     CPersonalBanker::DeleteTransferAndInfo()
/// </summary>
TEST_F(PersonalBanker2, DeleteTransferAndInfo1Test)
{
	// Test data.
	const CPersonalBankerTransfer * const transferExpected = NULL;
	const CPersonalBankerInfo * const infoExpected = NULL;

	// Setup.
	CPersonalBanker varPersonalBanker;

	// Perform operations to be tested.
	varPersonalBanker.DeleteTransferAndInfo();
	CPersonalBanker const * const personalBanker = &varPersonalBanker;

	// Test #1.
	CPersonalBankerTransfer *transferResult = personalBanker->GetTransferCopy();

	{
		// Report #1.
		bool areEqual = transferResult == transferExpected;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED transferResult=%p expected=%p in %s"),
			(void *)transferResult,
			(void *)transferExpected,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Test #2.
	CPersonalBankerInfo *infoResult = personalBanker->GetInfoCopy();

	{
		// Report #2.
		bool areEqual = infoResult == infoExpected;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED infoResult=%p expected=%p in %s"),
			(void *)infoResult,
			(void *)infoExpected,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Cleanup.
	delete transferResult;
	delete infoResult;
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::DeleteTransferAndInfo()
/// </summary>
TEST_F(PersonalBanker2, DeleteTransferAndInfo2Test)
{
	// Test data.
	const CPersonalBankerTransfer * const transferExpected = NULL;
	const CPersonalBankerInfo * const infoExpected = NULL;

	// Setup.
	CPersonalBanker varPersonalBanker;
	varPersonalBanker.CreateTransfer(CUtilities::GetTickCount());

	// Verify setup.
	ASSERT_TRUE(varPersonalBanker.HasTransfer()) << _T("varPersonalBanker.HasTransfer()");

	// Perform operations to be tested.
	varPersonalBanker.DeleteTransferAndInfo();
	CPersonalBanker const * const personalBanker = &varPersonalBanker;

	// Test #1.
	const PersonalBankerAction actionResult(personalBanker->GetAction());
	CPersonalBankerTransfer *transferResult = personalBanker->GetTransferCopy();

	{
		// Report #1.
		bool areEqual = transferResult == transferExpected;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED transferResult=%p expected=%p in %s"),
			(void *)transferResult,
			(void *)transferExpected,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Test #2.
	CPersonalBankerInfo *infoResult = personalBanker->GetInfoCopy();

	{
		// Report #2.
		bool areEqual = infoResult == infoExpected;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED infoResult=%p expected=%p in %s"),
			(void *)infoResult,
			(void *)infoExpected,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Cleanup.
	delete transferResult;
	delete infoResult;
}
#endif // _DEBUG.

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::GetInfoCopy()
/// </summary>
TEST_F(PersonalBanker2, GetInfoCopy1Test)
{
	// Test data.
	const CPersonalBankerInfo * const infoExpected = NULL;

	// Setup.
	CPersonalBanker varPersonalBanker;

	// Perform operations to be tested.
	CPersonalBanker const * const personalBanker = &varPersonalBanker;

	// Test #1.
	CPersonalBankerInfo *infoResult = personalBanker->GetInfoCopy();

	{
		// Report #1.
		bool areEqual = infoResult == infoExpected;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED infoResult=%p expected=%p in %s"),
			(void *)infoResult,
			(void *)infoExpected,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Cleanup.
	delete infoResult;
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::GetInfoCopy()
/// </summary>
TEST_F(PersonalBanker2, GetInfoCopy2Test)
{
	// Test data.
	const CPersonalBankerInfo * const infoExpected = NULL;
	const CConfig config(false, "");
	const bool gameOnline(false);
	const bool bIsEFT(false);

	// Setup.
	CPersonalBanker varPersonalBanker;
	CPersonalBankerInfo *pbInfo = NULL;

	// Perform operations to be tested.
	varPersonalBanker.ProcessPersonalBankerInfo(pbInfo,
		config,
		gameOnline,
		bIsEFT);
	CPersonalBanker const * const personalBanker = &varPersonalBanker;

	// Test #1.
	CPersonalBankerInfo *infoResult = personalBanker->GetInfoCopy();

	{
		// Report #1.
		bool areEqual = infoResult == infoExpected;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED infoResult=%p expected=%p in %s"),
			(void *)infoResult,
			(void *)infoExpected,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Cleanup.
	delete infoResult;
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::GetInfoCopy()
/// </summary>
TEST_F(PersonalBanker2, GetInfoCopy3Test)
{
	// Test data.
	const CPersonalBankerInfo * const infoExpected = NULL;
	const CConfig config(false, "");
	const bool gameOnline(false);
	const bool bIsEFT(false);

	// Setup.
	CPersonalBanker varPersonalBanker;
	CPersonalBankerInfo pbInfo;

	// Perform operations to be tested.
	varPersonalBanker.ProcessPersonalBankerInfo(&pbInfo,
		config,
		gameOnline,
		bIsEFT);
	CPersonalBanker const * const personalBanker = &varPersonalBanker;

	// Test #1.
	CPersonalBankerInfo *infoResult = personalBanker->GetInfoCopy();

	{
		// Report #1.
		bool areEqual = infoResult == infoExpected;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED infoResult=%p expected=%p in %s"),
			(void *)infoResult,
			(void *)infoExpected,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Cleanup.
	delete infoResult;
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::GetInfoPersonalBankerID()
/// </summary>
TEST_F(PersonalBanker2, GetInfoPersonalBankerID1Test)
{
	// Test data.
	const std::string infoPersonalBankerIDExpected;

	// Setup.
	CPersonalBanker varPersonalBanker;

	// Perform operations to be tested.
	CPersonalBanker const * const personalBanker = &varPersonalBanker;

	// Test #1.
	std::string infoPersonalBankerIDResult = personalBanker->GetInfoPersonalBankerID();

	{
		// Report #1.
		bool areEqual = infoPersonalBankerIDResult == infoPersonalBankerIDExpected;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED infoResult=\"%s\" expected=\"%s\" in %s"),
			static_cast<LPCTSTR>(infoPersonalBankerIDResult.c_str()),
			static_cast<LPCTSTR>(infoPersonalBankerIDExpected.c_str()),
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::GetInfoPersonalBankerID()
/// </summary>
TEST_F(PersonalBanker2, GetInfoPersonalBankerID2Test)
{
	// Test data.
	const std::string infoPersonalBankerIDExpected;
	const CConfig config(false, "");
	const bool gameOnline(false);
	const bool bIsEFT(false);

	// Setup.
	CPersonalBanker varPersonalBanker;
	CPersonalBankerInfo *pbInfo = NULL;

	// Perform operations to be tested.
	varPersonalBanker.ProcessPersonalBankerInfo(pbInfo,
		config,
		gameOnline,
		bIsEFT);
	CPersonalBanker const * const personalBanker = &varPersonalBanker;

	// Test #1.
	std::string infoPersonalBankerIDResult = personalBanker->GetInfoPersonalBankerID();

	{
		// Report #1.
		bool areEqual = infoPersonalBankerIDResult == infoPersonalBankerIDExpected;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED infoResult=\"%s\" expected=\"%s\" in %s"),
			static_cast<LPCTSTR>(infoPersonalBankerIDResult.c_str()),
			static_cast<LPCTSTR>(infoPersonalBankerIDExpected.c_str()),
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::GetInfoPersonalBankerID()
/// </summary>
TEST_F(PersonalBanker2, GetInfoPersonalBankerID3Test)
{
	// Test data.
	const std::string infoPersonalBankerIDExpected;
	const CConfig config(false, "");
	const bool gameOnline(false);
	const bool bIsEFT(false);

	// Setup.
	CPersonalBanker varPersonalBanker;
	CPersonalBankerInfo pbInfo;

	// Perform operations to be tested.
	varPersonalBanker.ProcessPersonalBankerInfo(&pbInfo,
		config,
		gameOnline,
		bIsEFT);
	CPersonalBanker const * const personalBanker = &varPersonalBanker;

	// Test #1.
	std::string infoPersonalBankerIDResult = personalBanker->GetInfoPersonalBankerID();

	{
		// Report #1.
		bool areEqual = infoPersonalBankerIDResult == infoPersonalBankerIDExpected;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED infoResult=\"%s\" expected=\"%s\" in %s"),
			static_cast<LPCTSTR>(infoPersonalBankerIDResult.c_str()),
			static_cast<LPCTSTR>(infoPersonalBankerIDExpected.c_str()),
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::ProcessPersonalBankerInfo()
///     CPersonalBanker::ProcessPersonalBankerInfoNotAuto()
///     CPersonalBanker::GetInfoPersonalBankerID()
/// Functional test of CPersonalBanker::ProcessPersonalBankerInfoNotAuto().
/// Functional test of CPersonalBanker::GetInfoPersonalBankerID().
/// </summary>
TEST_F(PersonalBanker2, GetInfoPersonalBankerID4Test)
{
	// Test data - inputs.
	const char* pbtID = NULL;
	const int pbtIDLength = 0;
	const CardType cardType = CARD_UNKNOWN;
	const PersonalBankerType pbType(Unspecified);
	const PersonalBankerState pbState(PBStateUnknown);

	// Test data - expected results.

	typedef struct
	{
		// Inputs.
		const bool m_bTransferCashToGameEnabled;
		const bool m_bTransferPointsToGameEnabled;
		const bool m_bTransferPromoDollarsToGameEnabled;
		const bool m_bTransferCashFromGameEnabled;
		const bool m_bGameDenomMatches;
		const bool m_bEGMInitialized;
		const bool m_bTransferPromoFromGameEnabled;
		const bool m_bTransferPromoFromGameAutoEnabled;
		const bool m_gameOnline;
		const bool m_bIsEFT;
		const bool m_isEFT;

		const byte m_transferCardNumber[CARD_ID_LEN];
		const byte m_infoCardNumber[CARD_ID_LEN];

		const int m_infoPbtIDLength;
		const char m_infoPbtID[MAX_PBID_LENGTH];
		const bool m_useInfo;
		const DWORD m_minCashUp;
		const DWORD m_maxCashUp;
		const DWORD m_minPromoUp;
		const DWORD m_maxPromoUp;

		// Expected results.
		const bool m_bTransferCashToGameEnabledExpected;
		const bool m_bTransferPointsToGameEnabledExpected;
		const bool m_bTransferPromoDollarsToGameEnabledExpected;
		const bool m_bTransferCashFromGameEnabledExpected;
		const bool m_bGameDenomMatchesExpected;
		// const bool m_bEGMInitializedExpected; // 2013-06-18 Not directly accessible.
		const bool m_bTransferPromoFromGameEnabledExpected;
		// const bool m_bTransferPromoFromGameAutoEnabled; // 2013-06-18 Not publicly accessible.
		const bool m_bTransferToGameEnabledExpected; // Test m_bEGMInitialized indirectly.

		const DWORD m_infoMinCashUpExpected;
		const DWORD m_infoMaxCashUpExpected;
		const DWORD m_infoMinPromoUpExpected;
		const DWORD m_infoMaxPromoUpExpected;
		const PersonalBankerState m_pbStateExpected;
		const int m_infoPbtIDLengthExpected;
		const char m_infoPbtIDExpected[MAX_PBID_LENGTH];
		const int m_transferPbtIDLengthExpected;
		const char m_transferPbtIDExpected[MAX_PBID_LENGTH];
		const bool m_infoExpected;
		const DWORD m_transferMinCashUpExpected;
		const DWORD m_transferMaxCashUpExpected;
		const DWORD m_transferMinPromoUpExpected;
		const DWORD m_transferMaxPromoUpExpected;
		const PersonalBankerAction m_actionExpected;
		const bool m_autoTransferPendingExpected;
		const bool m_skipStringTest;
	} TestDataType;

	const TestDataType TEST_DATA[] =
	{
		// Index #0.
		{ true, true, true, true, false, false, true, true, false, false, false,
			{ 0x12, 0x34, 0x56, 0x78, 0x90 }, { 0x12, 0x34, 0x56, 0x78, 0x90 },
			0, { 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 },
			true, 0, 0, 0, 0,
			true, true, true, true, false, true, false,
			0, 0, 0, 0, PBStateInfoReceived,
			0, { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
			0, { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
			true, 0, 0, 0, 0, PBActionGetEGMBalances, false, false },

		// Index #1.
		{ true, true, true, true, false, false, true, true, false, true, false,
			{ 0x12, 0x34, 0x56, 0x78, 0x90 }, { 0x12, 0x34, 0x56, 0x78, 0x90 },
			20, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
			true, 0, 0, 0, 0,
			true, true, true, true, false, true, false,
			0, 0, 0, 0, PBStateInfoReceived,
			20, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
			20, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
			true, 0, 0, 0, 0, PBActionGetEGMBalances, false, true },

		// Index #2.
		{ true, true, true, true, false, false, true, true, true, false, false,
			{ 0x12, 0x34, 0x56, 0x78, 0x90 }, { 0x12, 0x34, 0x56, 0x78, 0x90 },
			3, { 0, 0, 0, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 },
			true, 0, 0, 0, 0,
			true, true, true, true, false, true, true,
			0, 0, 0, 0, PBStateInfoReceived,
			3, { 0, 0, 0, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
			3, { 0, 0, 0, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
			true, 0, 0, 0, 0, PBActionGetEGMBalances, false, true },

		// Index #3.
		{ true, true, true, true, false, false, true, true, true, true, false,
			{ 0x12, 0x34, 0x56, 0x78, 0x90 }, { 0x12, 0x34, 0x56, 0x78, 0x90 },
			4, { 'a', 'b', 'c', 'd', 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 },
			true, 1, 2, 3, 4,
			true, true, true, true, false, true, true,
			1, 2, 3, 4, PBStateInfoReceived,
			4, 	{ 'a', 'b', 'c', 'd', 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
			4, 	{ 'a', 'b', 'c', 'd', 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
			true, 1, 2, 3, 4, PBActionGetEGMBalances, false, false },

		// Index #4.
		{ true, true, true, true, false, false, true, true, false, false, true,
			{ 0x12, 0x34, 0x56, 0x78, 0x90 }, { 0x12, 0x34, 0x56, 0x78, 0x90 },
			5, { '!', '@', '#', '$', '%', 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 },
			true, 1, 2, 3, 4,
			true, true, true, true, false, true, false,
			1, 2, 3, 4, PBStateInfoReceived,
			5, 	{ '!', '@', '#', '$', '%', 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
			5, 	{ '!', '@', '#', '$', '%', 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
			true, 1, 2, 3, 4, PBActionGetEGMBalances, false, false },

		// Index #5.
		{ true, true, true, true, false, false, true, true, false, true, true,
			{ 0x12, 0x34, 0x56, 0x78, 0x90 }, { 0x12, 0x34, 0x56, 0x78, 0x90 },
			20, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 },
			true, 1, 2, 3, 4,
			true, true, true, true, false, true, false,
			1, 2, 3, 4, PBStateInfoReceived,
			20, 	{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 },
			20, 	{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 },
			true, 1, 2, 3, 4, PBActionGetEGMBalances, false, true },

		// Index #6.
		{ true, true, true, true, false, false, true, true, true, false, true,
			{ 0x12, 0x34, 0x56, 0x78, 0x90 }, { 0x12, 0x34, 0x56, 0x78, 0x90 },
			3, { '1', '2', '3', 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 },
			true, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
			true, true, true, true, false, true, true,
			0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, PBStateInfoReceived,
			3, { '1', '2', '3', 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
			3, { '1', '2', '3', 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
			true, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, PBActionGetEGMBalances, false, false },

		// Index #7.
		{ true, true, true, true, false, false, true, true, true, true, true,
			{ 0x12, 0x34, 0x56, 0x78, 0x90 }, { 0x12, 0x34, 0x56, 0x78, 0x90 },
			20, { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J' },
			true, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
			true, true, true, true, false, true, true,
			0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, PBStateInfoReceived,
			20, { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J' },
			20, { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J' },
			true, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, PBActionGetEGMBalances, false, true },

		// Index #8.
		{ true, true, true, true, false, false, true, true, true, true, true,
			{ 0x12, 0x34, 0x56, 0x78, 0x90 }, { 0x12, 0x34, 0x56, 0x78, 0x90 },
			20, { (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff,
					(char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff },
			true, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
			true, true, true, true, false, true, true,
			0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, PBStateInfoReceived,
			20, { (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff,
					(char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff },
			20, { (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff,
					(char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff },
			true, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, PBActionGetEGMBalances, false, true },

		// Index #9.  Call ProcessPersonalBankerInfo() with pbInfo=NULL.
		{ true, true, true, true, false, false, true, true, true, true, false,
			{ 0x12, 0x34, 0x56, 0x78, 0x90 }, { 0x12, 0x34, 0x56, 0x78, 0x90 },
			4, { 'a', 'b', 'c', 'd', 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 },
			false, 1, 2, 3, 4,
			true, true, true, true, false, true, true,
			1, 2, 3, 4, PBStateUnknown,
			0, 	{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
			0, 	{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
			false, 0, 0, 0, 0, PBActionNone, false, false },

		// Index #10.  Call ProcessPersonalBankerInfo() with different pbInfo.m_cardNumber.
		{ true, true, true, true, false, false, true, true, true, true, false,
			{ 0x12, 0x34, 0x56, 0x78, 0x90 }, { 0x12, 0x34, 0x56, 0x78, 0x99 },
			6, { 'a', 'b', 'c', 'd', 'e', 'f', 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 },
			true, 1, 2, 3, 4,
			true, true, true, true, false, true, true,
			1, 2, 3, 4, PBStateUnknown,
			0, 	{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
			0, 	{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
			false, 0, 0, 0, 0, PBActionNone, false, false },
	};

	for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
	{
		CConfig varConfig(false, "");
		varConfig.SetPBTCashEnabled_Global(TEST_DATA[testIndex].m_bTransferCashToGameEnabled);
		varConfig.SetPBTCashEnabled_local(TEST_DATA[testIndex].m_bTransferCashToGameEnabled);
		varConfig.SetPBTPointsEnabled_Global(TEST_DATA[testIndex].m_bTransferPointsToGameEnabled);
		varConfig.SetPBTPointsAsPromo(TEST_DATA[testIndex].m_bTransferPointsToGameEnabled);
		varConfig.SetPBTPromoEnabled_Global(TEST_DATA[testIndex].m_bTransferPromoDollarsToGameEnabled);
		varConfig.SetPBTPromoEnabled_local(TEST_DATA[testIndex].m_bTransferPromoDollarsToGameEnabled);
		varConfig.SetPBFromGameEnabled(TEST_DATA[testIndex].m_bTransferCashFromGameEnabled);
		varConfig.SetGameDenom(1);
		varConfig.SetDenomination(TEST_DATA[testIndex].m_bGameDenomMatches ? 1 : 5);
		if (TEST_DATA[testIndex].m_bTransferPromoFromGameEnabled)
		{
			varConfig.SetSenOption2Raw(varConfig.GetSenOption2Raw() + SO2_NCEP_OUT_ENABLED);
		}
		if (TEST_DATA[testIndex].m_bTransferPromoFromGameAutoEnabled)
		{
			varConfig.SetSenOption2Raw(varConfig.GetSenOption2Raw() + SO2_NCEP_AUTO_UPLOAD);
		}
		const CConfig &config(varConfig);

		// Setup.
		CPersonalBankerInfo varPbInfo;
		varPbInfo.SetCardNumber(TEST_DATA[testIndex].m_infoCardNumber);
		varPbInfo.SetPersonalBankerID(TEST_DATA[testIndex].m_infoPbtID, TEST_DATA[testIndex].m_infoPbtIDLength);
		varPbInfo.SetMaximumCashUp(TEST_DATA[testIndex].m_maxCashUp);
		varPbInfo.SetMinimumCashUp(TEST_DATA[testIndex].m_minCashUp);
		varPbInfo.SetMinimumPromoUp(TEST_DATA[testIndex].m_minPromoUp);
		varPbInfo.SetMaximumPromoUp(TEST_DATA[testIndex].m_maxPromoUp);
		const CPersonalBankerInfo &pbInfo(TEST_DATA[testIndex].m_useInfo ? varPbInfo : *(CPersonalBankerInfo *)NULL);

		CPersonalBanker varPb;
		varPb.RemoveSession();
		varPb.CreateTransfer(CUtilities::GetTickCount());

		CPersonalBankerTransfer pbTransfer(CUtilities::GetTickCount());
		pbTransfer.SetID(pbtID, pbtIDLength);
		CGames games;
		byte meterNumberToAddTo(mNone);
		bool isDuplicate(false);
		varPb.ProcessPersonalBankerTransferComplete(&pbTransfer,
			games,
			meterNumberToAddTo,
			isDuplicate,
			TEST_DATA[testIndex].m_isEFT);

		CCard varTransferCard(CUtilities::GetCurrentTime(), CUtilities::GetCurrentTime());
		varTransferCard.SetCardInfo(cardType,
			TEST_DATA[testIndex].m_transferCardNumber,
			(int)sizeof(TEST_DATA[testIndex].m_transferCardNumber));
		const CCard &transferCard(varTransferCard);
		varPb.SetTransferCardNumber(&transferCard, CUtilities::GetCurrentTime());

		varPb.SetAutoInProcess(false);
		varPb.SetPersonalBankerType(pbType);
		varPb.SetState(pbState);
		varPb.ClearPersonalBankerAction();

		// Perform operations to be tested.
		varPb.ProcessPersonalBankerInfo(&pbInfo,
			config,
			TEST_DATA[testIndex].m_gameOnline,
			TEST_DATA[testIndex].m_bIsEFT);
		const CPersonalBanker &pb(varPb);
		std::string infoPersonalBankerIDResult = pb.GetInfoPersonalBankerID();

		// Test #1.
		const CPersonalBankerInfo *pbInfoResult = pb.GetInfoCopy();

		{
			bool areEqual = (pbInfoResult != NULL) == TEST_DATA[testIndex].m_infoExpected;

			std::string failedMsg;
			failedMsg = FormatString(_T("FAILED pbInfoResult=%p expected=%s testIndex=%d in %s"),
				(const void *)pbInfoResult,
				static_cast<LPCTSTR>(TEST_DATA[testIndex].m_infoExpected ? _T("NOT NULL") :  _T("NULL")),
				(int)testIndex,
				static_cast<LPCTSTR>(__TFUNCTION__));
			ASSERT_TRUE(areEqual) << failedMsg;
		}

		if (TEST_DATA[testIndex].m_infoExpected)
		{
			// Test #2.
			{
				const DWORD infoMinCashUp = pbInfoResult->GetMinimumCashUp();

				bool areEqual = infoMinCashUp == TEST_DATA[testIndex].m_infoMinCashUpExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED infoMinCashUp=%u expected=%u testIndex=%d in %s"),
					(int)infoMinCashUp,
					(int)TEST_DATA[testIndex].m_infoMinCashUpExpected,
					(int)testIndex,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #3.
			{
				const DWORD infoMaxCashUp = pbInfoResult->GetMaximumCashUp();

				bool areEqual = infoMaxCashUp == TEST_DATA[testIndex].m_infoMaxCashUpExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED infoMaxCashUp=%u expected=%u testIndex=%d in %s"),
					(int)infoMaxCashUp,
					(int)TEST_DATA[testIndex].m_infoMaxCashUpExpected,
					(int)testIndex,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #4.
			{
				const DWORD infoMinPromoUp = pbInfoResult->GetMinimumPromoUp();

				bool areEqual = infoMinPromoUp == TEST_DATA[testIndex].m_infoMinPromoUpExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED infoMinPromoUp=%u expected=%u testIndex=%d in %s"),
					(int)infoMinPromoUp,
					(int)TEST_DATA[testIndex].m_infoMinPromoUpExpected,
					(int)testIndex,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #5.
			{
				const DWORD infoMaxPromoUp = pbInfoResult->GetMaximumPromoUp();

				bool areEqual = infoMaxPromoUp == TEST_DATA[testIndex].m_infoMaxPromoUpExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED infoMaxPromoUp=%u expected=%u testIndex=%d in %s"),
					(int)infoMaxPromoUp,
					(int)TEST_DATA[testIndex].m_infoMaxPromoUpExpected,
					(int)testIndex,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #6.
			{
				const bool bTransferCashToGameEnabled = pbInfoResult->IsTransferCashToGameEnabled();

				bool areEqual = bTransferCashToGameEnabled == TEST_DATA[testIndex].m_bTransferCashToGameEnabledExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED bTransferCashToGameEnabled=%d expected=%d testIndex=%d in %s"),
					(int)bTransferCashToGameEnabled,
					(int)TEST_DATA[testIndex].m_bTransferCashToGameEnabledExpected,
					(int)testIndex,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #7.
			{
				const bool bTransferPointsToGameEnabled = pbInfoResult->IsTransferPointsToGameEnabled();

				bool areEqual = bTransferPointsToGameEnabled == TEST_DATA[testIndex].m_bTransferPointsToGameEnabledExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED bTransferPointsToGameEnabled=%d expected=%d testIndex=%d (%d %d %d %d %d %d) in %s"),
					(int)bTransferPointsToGameEnabled,
					(int)TEST_DATA[testIndex].m_bTransferPointsToGameEnabledExpected,
					(int)testIndex,
					(int)TEST_DATA[testIndex].m_bTransferPointsToGameEnabled,
					(int)config.PBTPointsEnabledGlobal(),
					(int)config.PBTPointsEnabled(),
					(int)TEST_DATA[testIndex].m_bIsEFT,
					(int)config.PBTPointsAsPromo(),
					(int)pbInfo.IsTransferPointsToGameEnabled(),
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #8.
			{
				const bool bTransferPromoDollarsToGameEnabled = pbInfoResult->IsTransferPromoDollarsToGameEnabled();

				bool areEqual = bTransferPromoDollarsToGameEnabled == TEST_DATA[testIndex].m_bTransferPromoDollarsToGameEnabledExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED bTransferPromoDollarsToGameEnabled=%d expected=%d testIndex=%d in %s"),
					(int)bTransferPromoDollarsToGameEnabled,
					(int)TEST_DATA[testIndex].m_bTransferPromoDollarsToGameEnabledExpected,
					(int)testIndex,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #9.
			{
				const bool bTransferCashFromGameEnabled = pbInfoResult->IsTransferCashFromGameEnabled();

				bool areEqual = bTransferCashFromGameEnabled == TEST_DATA[testIndex].m_bTransferCashFromGameEnabledExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED bTransferCashFromGameEnabled=%d expected=%d testIndex=%d in %s"),
					(int)bTransferCashFromGameEnabled,
					(int)TEST_DATA[testIndex].m_bTransferCashFromGameEnabledExpected,
					(int)testIndex,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #10.
			{
				const bool bGameDenomMatches = pbInfoResult->DoesGameDenomMatch();

				bool areEqual = bGameDenomMatches == TEST_DATA[testIndex].m_bGameDenomMatchesExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED bGameDenomMatches=%d expected=%d testIndex=%d in %s"),
					(int)bGameDenomMatches,
					(int)TEST_DATA[testIndex].m_bGameDenomMatchesExpected,
					(int)testIndex,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #11.
			{
				const bool bTransferPromoFromGameEnabled = pbInfoResult->IsTransferPromoFromGameEnabled();

				bool areEqual = bTransferPromoFromGameEnabled == TEST_DATA[testIndex].m_bTransferPromoFromGameEnabledExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED bTransferPromoFromGameEnabled=%d expected=%d testIndex=%d in %s"),
					(int)bTransferPromoFromGameEnabled,
					(int)TEST_DATA[testIndex].m_bTransferPromoFromGameEnabledExpected,
					(int)testIndex,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #12.
			{
				const bool bTransferToGameEnabled = pbInfoResult->IsTransferToGameEnabled();

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

			// Test #12a.
			{
				const int infoPbtIDLength = pbInfoResult->GetPBIDLength();

				bool areEqual = infoPbtIDLength == TEST_DATA[testIndex].m_infoPbtIDLengthExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED infoPbtIDLength=%d expected=%d testIndex=%d in %s"),
					(int)infoPbtIDLength,
					(int)TEST_DATA[testIndex].m_infoPbtIDLengthExpected,
					(int)testIndex,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #12b.
			const char *infoPbtID = pbInfoResult->GetPersonalBankerID();

			{
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED infoPbtID=%p expected=NOT NULL testIndex=%d in %s"),
					(const void *)infoPbtID,
					(int)testIndex,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_NE(nullptr, infoPbtID) << failedMsg;
			}

			// Test #12c.

			{
				size_t memcmpSize = (size_t)TEST_DATA[testIndex].m_infoPbtIDLengthExpected;
				bool areEqual = 0 == memcmp(infoPbtID,
					TEST_DATA[testIndex].m_infoPbtIDExpected,
					memcmpSize);

				std::string resultString = CUtilities::ToHexString(reinterpret_cast<const BYTE *>(infoPbtID),
					(DWORD)memcmpSize);
				std::string expectedString = 
					CUtilities::ToHexString(reinterpret_cast<const BYTE *>(TEST_DATA[testIndex].m_infoPbtIDExpected),
						(DWORD)memcmpSize);
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED infoPbtID={%s} expected={%s} testIndex=%d in %s"),
					static_cast<LPCTSTR>(resultString.c_str()),
					static_cast<LPCTSTR>(expectedString.c_str()),
					(int)testIndex,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}
		}

		// Test #13.
		{
			const PersonalBankerState pbStateResult = pb.GetState();

			bool areEqual = pbStateResult == TEST_DATA[testIndex].m_pbStateExpected;
			std::string failedMsg;
			failedMsg = FormatString(_T("FAILED pbStateResult=%d expected=%d testIndex=%d in %s"),
				(int)pbStateResult,
				(int)TEST_DATA[testIndex].m_pbStateExpected,
				(int)testIndex,
				static_cast<LPCTSTR>(__TFUNCTION__));
			ASSERT_TRUE(areEqual) << failedMsg;
		}


		// Test #14.
		const CPersonalBankerTransfer *pbTransferResult = pb.GetTransferCopy();

		{
			std::string failedMsg;
			failedMsg = FormatString(_T("FAILED pbTransferResult=%p expected=NOT NULL testIndex=%d in %s"),
				(const void *)pbTransferResult,
				(int)testIndex,
				static_cast<LPCTSTR>(__TFUNCTION__));
			ASSERT_NE(nullptr, pbTransferResult) << failedMsg;
		}

		// Test #15.
		{
			const int transferPbtIDLength = pbTransferResult->GetPBTIDLength();

			bool areEqual = transferPbtIDLength == TEST_DATA[testIndex].m_transferPbtIDLengthExpected;
			std::string failedMsg;
			failedMsg = FormatString(_T("FAILED transferPbtIDLength=%d expected=%d testIndex=%d in %s"),
				(int)transferPbtIDLength,
				(int)TEST_DATA[testIndex].m_transferPbtIDLengthExpected,
				(int)testIndex,
				static_cast<LPCTSTR>(__TFUNCTION__));
			ASSERT_TRUE(areEqual) << failedMsg;
		}

		// Test #16.
		const char *transferPbtID = pbTransferResult->GetID();

		{
			std::string failedMsg;
			failedMsg = FormatString(_T("FAILED transferPbtID=%p expected=NOT NULL testIndex=%d in %s"),
				(const void *)transferPbtID,
				(int)testIndex,
				static_cast<LPCTSTR>(__TFUNCTION__));
			ASSERT_NE(nullptr, transferPbtID) << failedMsg;
		}

		// Test #17.

		{
			size_t memcmpSize = (size_t)TEST_DATA[testIndex].m_transferPbtIDLengthExpected;
			bool areEqual = 0 == memcmp(transferPbtID,
				TEST_DATA[testIndex].m_transferPbtIDExpected,
				memcmpSize);

			std::string resultString = CUtilities::ToHexString(reinterpret_cast<const BYTE *>(transferPbtID),
				(DWORD)memcmpSize);
			std::string expectedString = 
				CUtilities::ToHexString(reinterpret_cast<const BYTE *>(TEST_DATA[testIndex].m_transferPbtIDExpected),
					(DWORD)memcmpSize);
			std::string failedMsg;
			failedMsg = FormatString(_T("FAILED transferPbtID={%s} expected={%s} testIndex=%d in %s"),
				static_cast<LPCTSTR>(resultString.c_str()),
				static_cast<LPCTSTR>(expectedString.c_str()),
				(int)testIndex,
				static_cast<LPCTSTR>(__TFUNCTION__));
			ASSERT_TRUE(areEqual) << failedMsg;
		}

		// Test #18.
		{
			const DWORD transferMinCashUp = pbTransferResult->GetMinCashUp();

			bool areEqual = transferMinCashUp == TEST_DATA[testIndex].m_transferMinCashUpExpected;
			std::string failedMsg;
			failedMsg = FormatString(_T("FAILED transferMinCashUp=%u expected=%u testIndex=%d in %s"),
				(int)transferMinCashUp,
				(int)TEST_DATA[testIndex].m_transferMinCashUpExpected,
				(int)testIndex,
				static_cast<LPCTSTR>(__TFUNCTION__));
			ASSERT_TRUE(areEqual) << failedMsg;
		}

		// Test #19.
		{
			const DWORD transferMaxCashUp = pbTransferResult->GetMaxCashUp();

			bool areEqual = transferMaxCashUp == TEST_DATA[testIndex].m_transferMaxCashUpExpected;
			std::string failedMsg;
			failedMsg = FormatString(_T("FAILED transferMaxCashUp=%u expected=%u testIndex=%d in %s"),
				(int)transferMaxCashUp,
				(int)TEST_DATA[testIndex].m_transferMaxCashUpExpected,
				(int)testIndex,
				static_cast<LPCTSTR>(__TFUNCTION__));
			ASSERT_TRUE(areEqual) << failedMsg;
		}

		// Test #20.
		{
			const DWORD transferMinPromoUp = pbTransferResult->GetMinPromoUp();

			bool areEqual = transferMinPromoUp == TEST_DATA[testIndex].m_transferMinPromoUpExpected;
			std::string failedMsg;
			failedMsg = FormatString(_T("FAILED transferMinPromoUp=%u expected=%u testIndex=%d in %s"),
				(int)transferMinPromoUp,
				(int)TEST_DATA[testIndex].m_transferMinPromoUpExpected,
				(int)testIndex,
				static_cast<LPCTSTR>(__TFUNCTION__));
			ASSERT_TRUE(areEqual) << failedMsg;
		}

		// Test #21.
		{
			const DWORD transferMaxPromoUp = pbTransferResult->GetMaxPromoUp();

			bool areEqual = transferMaxPromoUp == TEST_DATA[testIndex].m_transferMaxPromoUpExpected;
			std::string failedMsg;
			failedMsg = FormatString(_T("FAILED transferMaxPromoUp=%u expected=%u testIndex=%d in %s"),
				(int)transferMaxPromoUp,
				(int)TEST_DATA[testIndex].m_transferMaxPromoUpExpected,
				(int)testIndex,
				static_cast<LPCTSTR>(__TFUNCTION__));
			ASSERT_TRUE(areEqual) << failedMsg;
		}

		// Test #22.
		{
			const PersonalBankerAction action = pb.GetAction();

			const bool areEqual = action == TEST_DATA[testIndex].m_actionExpected;
			std::string failedMsg;
			failedMsg = FormatString(_T("FAILED action=%d expected=%d in testIndex=%d in %s"),
				(int)action,
				(int)TEST_DATA[testIndex].m_actionExpected,
				(int)testIndex,
				static_cast<LPCTSTR>(__TFUNCTION__));
			ASSERT_TRUE(areEqual) << failedMsg;
		}

		// Test #23.
		{
			const bool autoTransferPending = varPb.IsAutoTransferPending();

			const bool areEqual = autoTransferPending == TEST_DATA[testIndex].m_autoTransferPendingExpected;
			std::string failedMsg;
			failedMsg = FormatString(_T("FAILED autoTransferPending=%d expected=%d in testIndex=%d in %s(%d)"),
				(int)autoTransferPending,
				(int)TEST_DATA[testIndex].m_autoTransferPendingExpected,
				(int)testIndex,
				static_cast<LPCTSTR>(__TFUNCTION__), __LINE__);
			ASSERT_TRUE(areEqual) << failedMsg;
		}

		if (!TEST_DATA[testIndex].m_skipStringTest)
		{
			// Test #24.
			const int infoPersonalBankerIDResultLength = infoPersonalBankerIDResult.length();

			{
				const bool areEqual = infoPersonalBankerIDResultLength == TEST_DATA[testIndex].m_infoPbtIDLengthExpected;
				std::string failedMsg;
				failedMsg = FormatString(_T("FAILED infoPersonalBankerIDResultLength=%d expected=%d in testIndex=%d in %s(%d)"),
					(int)infoPersonalBankerIDResultLength,
					(int)TEST_DATA[testIndex].m_infoPbtIDLengthExpected,
					(int)testIndex,
					static_cast<LPCTSTR>(__TFUNCTION__), __LINE__);
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #25.
			for (int idIndex = 0; idIndex < TEST_DATA[testIndex].m_infoPbtIDLengthExpected; ++idIndex)
			{
				const TCHAR idChar = infoPersonalBankerIDResult[idIndex];

				{
					const bool areEqual = idChar == TEST_DATA[testIndex].m_infoPbtIDExpected[idIndex];
					std::string failedMsg;
					failedMsg = FormatString(_T("FAILED idChar=%d expected=%d at idIndex=%d in testIndex=%d in %s(%d)"),
						(int)idChar,
						(int)TEST_DATA[testIndex].m_infoPbtIDExpected[idIndex],
						(int)idIndex,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__), __LINE__);
					ASSERT_TRUE(areEqual) << failedMsg;
				}
			}
		}

		// Cleanup.
		delete pbInfoResult;
	}
}

/// <summary>
///     CPersonalBanker::GetPbStateForUi()
/// </summary>
TEST_F(PersonalBanker2, GetPbStateForUi1Test)
{
	// Test data.
	const PersonalBankerState pbStateForUiExpected = PBStateUnknown;

	// Setup.
	CPersonalBanker varPersonalBanker;

	// Perform operations to be tested.
	const CPersonalBanker * const personalBanker = &varPersonalBanker;

	// Test #1.
	const PersonalBankerState pbStateForUiResult = personalBanker->GetPbStateForUi();

	{
		// Report #1.
		bool areEqual = pbStateForUiResult == pbStateForUiExpected;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED pbStateForUiResult=%d expected=%d in %s"),
			(int)pbStateForUiResult,
			(int)pbStateForUiExpected,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::GetPbStateForUi()
/// </summary>
TEST_F(PersonalBanker2, GetPbStateForUi2Test)
{
	// Test data.
	const PersonalBankerState pbStateForUiExpected = PBStateUnknown;

	// Setup.
	CPersonalBanker varPersonalBanker;

	// Perform operations to be tested.
	varPersonalBanker.ClearPersonalBankerAction();
	const CPersonalBanker * const personalBanker = &varPersonalBanker;

	// Test #1.
	const PersonalBankerState pbStateForUiResult = personalBanker->GetPbStateForUi();

	{
		// Report #1.
		bool areEqual = pbStateForUiResult == pbStateForUiExpected;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED pbStateForUiResult=%d expected=%d in %s"),
			(int)pbStateForUiResult,
			(int)pbStateForUiExpected,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::GetPbStateForUi()
/// </summary>
TEST_F(PersonalBanker2, GetPbStateForUi3Test)
{
	// Test data.
	const PersonalBankerState pbStateForUiExpected = PBStateUnknown;
	const CConfig config(false, "");
	const bool canceledFromUI(false);

	// Setup.
	CPersonalBanker varPersonalBanker;

	// Perform operations to be tested.
	varPersonalBanker.ReleasePersonalBankerSession(config, canceledFromUI);
	const CPersonalBanker * const personalBanker = &varPersonalBanker;

	// Test #1.
	const PersonalBankerState pbStateForUiResult = personalBanker->GetPbStateForUi();

	{
		// Report #1.
		bool areEqual = pbStateForUiResult == pbStateForUiExpected;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED pbStateForUiResult=%d expected=%d in %s"),
			(int)pbStateForUiResult,
			(int)pbStateForUiExpected,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::GetPbStateForUi()
/// </summary>
TEST_F(PersonalBanker2, GetPbStateForUi4Test)
{
	// Test data.
	const PersonalBankerState pbStateForUiExpected = PBStateUnknown;

	// Setup.
	CPersonalBanker varPersonalBanker;

	// Perform operations to be tested.
	varPersonalBanker.SetToReleaseTransferSession();
	const CPersonalBanker * const personalBanker = &varPersonalBanker;

	// Test #1.
	const PersonalBankerState pbStateForUiResult = personalBanker->GetPbStateForUi();

	{
		// Report #1.
		bool areEqual = pbStateForUiResult == pbStateForUiExpected;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED pbStateForUiResult=%d expected=%d in %s"),
			(int)pbStateForUiResult,
			(int)pbStateForUiExpected,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::GetUnlockEgmDuringTransfer()
/// </summary>
TEST_F(PersonalBanker2, GetUnlockEgmDuringTransfer1Test)
{
	// Test data.
	const bool unlockEgmDuringTransferExpected = false;

	// Setup.
	CPersonalBanker varPersonalBanker;

	// Perform operations to be tested.
	const CPersonalBanker * const personalBanker = &varPersonalBanker;

	// Test #1.
	const bool unlockEgmDuringTransferResult = personalBanker->GetUnlockEgmDuringTransfer();

	{
		// Report #1.
		bool areEqual = unlockEgmDuringTransferResult == unlockEgmDuringTransferExpected;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED unlockEgmDuringTransferResult=%d expected=%d in %s"),
			(int)unlockEgmDuringTransferResult,
			(int)unlockEgmDuringTransferExpected,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::GetUnlockEgmDuringTransfer()
/// </summary>
TEST_F(PersonalBanker2, GetUnlockEgmDuringTransfer2Test)
{
	// Test data.
	const bool unlockEgmDuringTransferExpected = false;
	const DWORD cashOnGame(0);
	const DWORD promoOnGame(0);
	const bool canTransferOff(false);
	const bool callToEGMFailed(false);
	const CConfig config(false, "");

	// Setup.
	CPersonalBanker varPersonalBanker;

	// Perform operations to be tested.
	varPersonalBanker.ProcessBalancesFromEGM(cashOnGame,
		0,
		promoOnGame,
		canTransferOff,
		callToEGMFailed,
		config);
	const CPersonalBanker * const personalBanker = &varPersonalBanker;

	// Test #1.
	const bool unlockEgmDuringTransferResult = personalBanker->GetUnlockEgmDuringTransfer();

	{
		// Report #1.
		bool areEqual = unlockEgmDuringTransferResult == unlockEgmDuringTransferExpected;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED unlockEgmDuringTransferResult=%d expected=%d in %s"),
			(int)unlockEgmDuringTransferResult,
			(int)unlockEgmDuringTransferExpected,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::GetUnlockEgmDuringTransfer()
/// </summary>
TEST_F(PersonalBanker2, GetUnlockEgmDuringTransferCashFromGameTest)
{
	PersonalBankerTransferType type(CashFromGame); 
	const bool unlockEgmDuringTransferExpectedResult(true);
	bool unlockEgmDuringTransferActualResult(false);

	//Test
	{
		unlockEgmDuringTransferActualResult = GetUnlockEgmDuringTransfer(type);
		bool areEqual = unlockEgmDuringTransferExpectedResult == unlockEgmDuringTransferActualResult;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED unlockEgmDuringTransferResult=%d expected=%d in %s"),
			(int)unlockEgmDuringTransferActualResult,
			(int)unlockEgmDuringTransferExpectedResult,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::GetUnlockEgmDuringTransfer()
/// </summary>
TEST_F(PersonalBanker2, GetUnlockEgmDuringTransferPointsFromGameTest)
{
	PersonalBankerTransferType type(PointsFromGame);
	const bool unlockEgmDuringTransferExpectedResult(true);
	bool unlockEgmDuringTransferActualResult(false);

	//Test
	{
		unlockEgmDuringTransferActualResult = GetUnlockEgmDuringTransfer(type);
		bool areEqual = unlockEgmDuringTransferExpectedResult == unlockEgmDuringTransferActualResult;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED unlockEgmDuringTransferResult=%d expected=%d in %s"),
			(int)unlockEgmDuringTransferActualResult,
			(int)unlockEgmDuringTransferExpectedResult,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::GetUnlockEgmDuringTransfer()
/// </summary>
TEST_F(PersonalBanker2, GetUnlockEgmDuringTransferPromoDollarsFromGameTest)
{
	PersonalBankerTransferType type(PromoDollarsFromGame); 
	const bool unlockEgmDuringTransferExpectedResult(true);
	bool unlockEgmDuringTransferActualResult(false);

	//Test
	{
		unlockEgmDuringTransferActualResult = GetUnlockEgmDuringTransfer(type);
		bool areEqual = unlockEgmDuringTransferExpectedResult == unlockEgmDuringTransferActualResult;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED unlockEgmDuringTransferResult=%d expected=%d in %s"),
			(int)unlockEgmDuringTransferActualResult,
			(int)unlockEgmDuringTransferExpectedResult,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::GetUnlockEgmDuringTransfer()
/// </summary>
TEST_F(PersonalBanker2, GetUnlockEgmDuringTransferAllFromGameTest)
{
	PersonalBankerTransferType type(AllFromGame); 
	const bool unlockEgmDuringTransferExpectedResult(true);
	bool unlockEgmDuringTransferActualResult(false);

	//Test
	{
		unlockEgmDuringTransferActualResult = GetUnlockEgmDuringTransfer(type);
		bool areEqual = unlockEgmDuringTransferExpectedResult == unlockEgmDuringTransferActualResult;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED unlockEgmDuringTransferResult=%d expected=%d in %s"),
			(int)unlockEgmDuringTransferActualResult,
			(int)unlockEgmDuringTransferExpectedResult,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::GetUnlockEgmDuringTransfer()
/// </summary>
TEST_F(PersonalBanker2, GetUnlockEgmDuringTransferCashToGameTest)
{
	PersonalBankerTransferType type(CashToGame);
	const bool unlockEgmDuringTransferExpectedResult(false);
	bool unlockEgmDuringTransferActualResult(false);

	//Test #1
	{
		unlockEgmDuringTransferActualResult = GetUnlockEgmDuringTransfer(type);
		bool areEqual = unlockEgmDuringTransferExpectedResult == unlockEgmDuringTransferActualResult;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED unlockEgmDuringTransferResult=%d expected=%d in %s"),
			(int)unlockEgmDuringTransferActualResult,
			(int)unlockEgmDuringTransferExpectedResult,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::GetUnlockEgmDuringTransfer()
/// </summary>
TEST_F(PersonalBanker2, GetUnlockEgmDuringTransferPointsToGameTest)
{
	PersonalBankerTransferType type(PointsToGame);
	const bool unlockEgmDuringTransferExpectedResult(false);
	bool unlockEgmDuringTransferActualResult(false);

	//Test
	{
		unlockEgmDuringTransferActualResult = GetUnlockEgmDuringTransfer(type);
		bool areEqual = unlockEgmDuringTransferExpectedResult == unlockEgmDuringTransferActualResult;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED unlockEgmDuringTransferResult=%d expected=%d in %s"),
			(int)unlockEgmDuringTransferActualResult,
			(int)unlockEgmDuringTransferExpectedResult,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::GetUnlockEgmDuringTransfer()
/// </summary>
TEST_F(PersonalBanker2, GetUnlockEgmDuringTransferPromoDollarsToGameTest)
{
	PersonalBankerTransferType type(PromoDollarsToGame); 
	const bool unlockEgmDuringTransferExpectedResult(false);
	bool unlockEgmDuringTransferActualResult(false);

	//Test
	{
		unlockEgmDuringTransferActualResult = GetUnlockEgmDuringTransfer(type);
		bool areEqual = unlockEgmDuringTransferExpectedResult == unlockEgmDuringTransferActualResult;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED unlockEgmDuringTransferResult=%d expected=%d in %s"),
			(int)unlockEgmDuringTransferActualResult,
			(int)unlockEgmDuringTransferExpectedResult,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::GetUnlockEgmDuringTransfer()
/// </summary>
TEST_F(PersonalBanker2, GetUnlockEgmDuringTransferAutoNcepTest)
{
	const bool unlockEgmDuringTransferExpectedResult(true);
	bool unlockEgmDuringTransferActualResult(false);
	CPersonalBanker varPersonalBanker;
	bool sendToPoller(true);
	CConfig config(false, "");
	config.SetAutoUploadCreditsGlobal(true);
	config.SetSenOption2Raw(config.GetSenOption2Raw() + SO2_NCEP_AUTO_UPLOAD);
	config.SetSenOption2Raw(config.GetSenOption2Raw() + SO2_NCEP_OUT_ENABLED);
	config.SetPBTPromoEnabled_Global(true);
	const CEGMConfig egmConfig(MEMORY_NONE);
	DWORD autoUploadPin(PBT_AUTO_UPLOAD_PIN);
	DWORD cashOnGame(0);
	DWORD promoOnGame(100);
	bool canTransferOff(false);
	bool callToEGMFailed(false);
	LPCTSTR pbPin = _T("1234");
	CCard *card = new CCard(CUtilities::GetCurrentTime(),CUtilities::GetUTCTime());
	card->SetCardID(_T("12345"));
	CPlayerInfo* playerInfo(new CPlayerInfo());
	CGames games;
	DWORD amountInDollars(66);
	byte pbEFTNumber(1);
	CPlayer player(card, playerInfo, config, egmConfig, CUtilities::GetCurrentTime(), false, &games);
	varPersonalBanker.BeginPersonalBankerSession(&player, pbPin, config);
	bool bNeedToSendToPoller = varPersonalBanker.BeginPersonalBankerAutoSession(sendToPoller, config, autoUploadPin, CUtilities::GetTickCount());
	varPersonalBanker.ProcessBalancesFromEGM(cashOnGame,
		0,
		promoOnGame,
		canTransferOff,
		callToEGMFailed,
		config);

	unlockEgmDuringTransferActualResult = varPersonalBanker.GetUnlockEgmDuringTransfer();

	//Test
	{
		bool areEqual = unlockEgmDuringTransferExpectedResult == unlockEgmDuringTransferActualResult;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED unlockEgmDuringTransferResult=%d expected=%d in %s"),
			(int)unlockEgmDuringTransferActualResult,
			(int)unlockEgmDuringTransferExpectedResult,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::IsSessionReleasedDueToTimeout()
/// </summary>
TEST_F(PersonalBanker2, SessionIsReleasedDueToTimeoutTest)
{
	const bool isSessionReleasedDueToTimeoutExpectedResult(true);
	bool isSessionReleasedDueToTimeoutActualResult(false);
	const bool haveInternalsToGCFQueue(false);
	const bool haveEmployee1(false);
	CPersonalBanker varPersonalBanker;
	CConfig config(false, "");
         
    CreateInfo(varPersonalBanker);
	const CEGMConfig egmConfig(MEMORY_NONE);
	LPCTSTR pbPin = _T("1234");
	CCard *card = new CCard(CUtilities::GetCurrentTime(),CUtilities::GetUTCTime());
	card->SetCardID(_T("12345"));
	CPlayerInfo* playerInfo(new CPlayerInfo());
	CGames games;
	CPlayer player(card, playerInfo, config, egmConfig, CUtilities::GetCurrentTime(), false, &games);

	varPersonalBanker.BeginPersonalBankerSession(&player, pbPin, config);

	usleep(m_sessionTimeoutValueInMilliseconds*2*1000);

	varPersonalBanker.PerformTimedEvents(haveInternalsToGCFQueue,
		haveEmployee1,
		config,
		CUtilities::GetTickCount(),
		m_sessionTimeoutValueInMilliseconds);

	isSessionReleasedDueToTimeoutActualResult = varPersonalBanker.IsSessionReleasedDueToTimeout();

	//Test
	{
		bool areEqual = isSessionReleasedDueToTimeoutExpectedResult == isSessionReleasedDueToTimeoutActualResult;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED isSessionReleasedDueToTimeoutResult=%d expected=%d in %s"),
			(int)isSessionReleasedDueToTimeoutActualResult,
			(int)isSessionReleasedDueToTimeoutExpectedResult,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::ISessionReleasedDueToTimeout()
/// </summary>
TEST_F(PersonalBanker2, SessionIsNotReleasedDueToTimeoutTest)
{
	const bool isSessionReleasedDueToTimeoutExpectedResult(false);
	bool isSessionReleasedDueToTimeoutActualResult(false);
	const bool haveInternalsToGCFQueue(false);
	const bool haveEmployee1(false);
	CPersonalBanker varPersonalBanker;
         
    CreateInfo(varPersonalBanker);
	CConfig config(false, "");
	const CEGMConfig egmConfig(MEMORY_NONE);
	LPCTSTR pbPin = _T("1234");
	CCard *card = new CCard(CUtilities::GetCurrentTime(),CUtilities::GetUTCTime());
	card->SetCardID(_T("12345"));
	CPlayerInfo* playerInfo(new CPlayerInfo());
	CGames games;
	CPlayer player(card, playerInfo, config, egmConfig, CUtilities::GetCurrentTime(), false, &games);

	varPersonalBanker.BeginPersonalBankerSession(&player, pbPin, config);

	// Test that CPersonalBanker calculates the timeout condition correctly.
	// Do NOT test the amount of time that elapses during the test.
	varPersonalBanker.PerformTimedEvents(haveInternalsToGCFQueue,
		haveEmployee1,
		config,
		CUtilities::GetTickCount(),
		m_longTimeoutValueInMilliseconds);
			
	isSessionReleasedDueToTimeoutActualResult = varPersonalBanker.IsSessionReleasedDueToTimeout();

	//Test
	{
		bool areEqual = isSessionReleasedDueToTimeoutExpectedResult == isSessionReleasedDueToTimeoutActualResult;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED isSessionReleasedDueToTimeoutResult=%d expected=%d in %s"),
			(int)isSessionReleasedDueToTimeoutActualResult,
			(int)isSessionReleasedDueToTimeoutExpectedResult,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}
}

/// <summary>
/// Functional test for:
///     CPersonalBanker::PerformTimedEvents()
///     CPersonalBanker::PerformTimedEventsTransfer()
///     CPersonalBanker::IsSessionReleasedDueToTimeout()
/// Test that an authorization timeout does occur when it should, overriding parameters.
/// </summary>
TEST_F(PersonalBanker2, AuthorizationIsReleasedDueToTimeout_OverrideTest)
{
	// Data.
	const DWORD amountInDollars(7);
	const bool atomicRedemptions(true);
	const bool haveInternalsToGCFQueue(false);
	const bool haveEmployee1(false);

	DWORD tickCountCreateTransfer = 10000;
	DWORD sessionTimeout = 20000;
	DWORD authorizationTimeout = 90000;

	// Setup.
	CPersonalBanker varPersonalBanker;
	CConfig config(false, "");
         
    CreateInfo(varPersonalBanker);
	const CEGMConfig egmConfig(MEMORY_NONE);
	LPCTSTR pbPin = _T("1234");
	CCard *card = new CCard(CUtilities::GetCurrentTime(),CUtilities::GetUTCTime());
	card->SetCardID(_T("12345"));
	CPlayerInfo* playerInfo(new CPlayerInfo());
	CGames games;
	CPlayer player(card, playerInfo, config, egmConfig, CUtilities::GetCurrentTime(), false, &games);

	varPersonalBanker.BeginPersonalBankerSession(&player, pbPin, config);
	varPersonalBanker.CreateTransfer(tickCountCreateTransfer);
	varPersonalBanker.SetState(PBStateInfoReceived);

	// Perform operation(s) and test(s).
	ASSERT_EQ(true, varPersonalBanker.CheckIfAuthorizationIsNeeded(CashToGame, amountInDollars, atomicRedemptions))
		<< L"CheckIfAuthorizationIsNeeded";

	// Verify that timeout has not occured for elapsed time equal to the maximum non-timeout.
	varPersonalBanker.PerformTimedEvents(haveInternalsToGCFQueue,
		haveEmployee1,
		config,
		tickCountCreateTransfer + authorizationTimeout - 1,
		sessionTimeout,
		authorizationTimeout);
	ASSERT_FALSE( varPersonalBanker.IsSessionReleasedDueToTimeout());

	// Verify that timeout has occured for elapsed time equal to the minimum timeout.
	varPersonalBanker.PerformTimedEvents(haveInternalsToGCFQueue,
		haveEmployee1,
		config,
		tickCountCreateTransfer + authorizationTimeout,
		sessionTimeout,
		authorizationTimeout);
	ASSERT_EQ(true, varPersonalBanker.IsSessionReleasedDueToTimeout());
	ASSERT_EQ(PBActionReleaseSession, varPersonalBanker.GetAction());
}

/// <summary>
/// Functional test for:
///     CPersonalBanker::PerformTimedEvents()
///     CPersonalBanker::PerformTimedEventsTransfer()
///     CPersonalBanker::IsSessionReleasedDueToTimeout()
/// Test that an authorization timeout does occur when it should using default parameters.
/// </summary>
TEST_F(PersonalBanker2, AuthorizationIsReleasedDueToTimeout_DefaultTest)
{
	// Data.
	const DWORD amountInDollars(7);
	const bool atomicRedemptions(true);
	const bool haveInternalsToGCFQueue(false);
	const bool haveEmployee1(false);

	DWORD tickCountCreateTransfer = 10000;
	DWORD sessionTimeout = 30000;
	DWORD authorizationTimeout = 120000;
	ASSERT_EQ(sessionTimeout, (DWORD)CPersonalBanker::PB_SESSION_TIMEOUT) << L"PB_SESSION_TIMEOUT";
	ASSERT_EQ(authorizationTimeout, (DWORD)CPersonalBanker::PB_AUTHORIZATION_TIMEOUT) << L"PB_AUTHORIZATION_TIMEOUT";

	// Setup.
	CPersonalBanker varPersonalBanker;
	CConfig config(false, "");
         
    CreateInfo(varPersonalBanker);
	const CEGMConfig egmConfig(MEMORY_NONE);
	LPCTSTR pbPin = _T("1234");
	CCard *card = new CCard(CUtilities::GetCurrentTime(),CUtilities::GetUTCTime());
	card->SetCardID(_T("12345"));
	CPlayerInfo* playerInfo(new CPlayerInfo());
	CGames games;
	CPlayer player(card, playerInfo, config, egmConfig, CUtilities::GetCurrentTime(), false, &games);

	varPersonalBanker.BeginPersonalBankerSession(&player, pbPin, config);
	varPersonalBanker.CreateTransfer(tickCountCreateTransfer);
	varPersonalBanker.SetState(PBStateInfoReceived);

	// Perform operation(s) and test(s).
	ASSERT_EQ(true, varPersonalBanker.CheckIfAuthorizationIsNeeded(CashToGame, amountInDollars, atomicRedemptions))
		<< L"CheckIfAuthorizationIsNeeded";

	// Verify that timeout has not occured for elapsed time equal to the maximum non-timeout.
	varPersonalBanker.PerformTimedEvents(haveInternalsToGCFQueue,
		haveEmployee1,
		config,
		tickCountCreateTransfer + authorizationTimeout - 1);
	ASSERT_FALSE( varPersonalBanker.IsSessionReleasedDueToTimeout());

	// Verify that timeout has occured for elapsed time equal to the minimum timeout.
	varPersonalBanker.PerformTimedEvents(haveInternalsToGCFQueue,
		haveEmployee1,
		config,
		tickCountCreateTransfer + authorizationTimeout);
	ASSERT_EQ(true, varPersonalBanker.IsSessionReleasedDueToTimeout());
	ASSERT_EQ(PBActionReleaseSession, varPersonalBanker.GetAction());
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::ResetTransferSessionStartTime()
/// </summary>
TEST_F(PersonalBanker2, ResetTransferSessionStartTimeTest)
{
	const bool isSessionReleasedDueToTimeoutExpectedResult(false);
	bool isSessionReleasedDueToTimeoutActualResult(false);
	const bool haveInternalsToGCFQueue(false);
	const bool haveEmployee1(false);
	CPersonalBanker varPersonalBanker;
	CConfig config(false, "");
         
    CreateInfo(varPersonalBanker);
	const CEGMConfig egmConfig(MEMORY_NONE);
	LPCTSTR pbPin = _T("1234");
	CCard *card = new CCard(CUtilities::GetCurrentTime(),CUtilities::GetUTCTime());
	card->SetCardID(_T("12345"));
	CPlayerInfo* playerInfo(new CPlayerInfo());
	CGames games;
	CPlayer player(card, playerInfo, config, egmConfig, CUtilities::GetCurrentTime(), false, &games);

	varPersonalBanker.BeginPersonalBankerSession(&player, pbPin, config);

	usleep(m_sessionTimeoutValueInMilliseconds*1000);

	varPersonalBanker.ResetTransferSessionStartTime(CUtilities::GetTickCount());

	varPersonalBanker.PerformTimedEvents(haveInternalsToGCFQueue,
		haveEmployee1,
		config,
		CUtilities::GetTickCount(),
		m_sessionTimeoutValueInMilliseconds);

	isSessionReleasedDueToTimeoutActualResult = varPersonalBanker.IsSessionReleasedDueToTimeout();

	//Test
	{
		bool areEqual = isSessionReleasedDueToTimeoutExpectedResult == isSessionReleasedDueToTimeoutActualResult;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED isSessionReleasedDueToTimeoutResult=%d expected=%d in %s"),
			(int)isSessionReleasedDueToTimeoutActualResult,
			(int)isSessionReleasedDueToTimeoutExpectedResult,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::PreparePersonalBankerTransfer()
/// </summary>
TEST_F(PersonalBanker2, PreparePersonalBankerTransferTest)
{
	WORD sequenceInitialValue(1);
	WORD sequenceNumber(sequenceInitialValue);
	CPersonalBanker varPersonalBanker;
	CConfig config(false, "");
	const CEGMConfig egmConfig(MEMORY_NONE);
         
    CreateInfo(varPersonalBanker);
	LPCTSTR pbPin = _T("1234");
	CCard *card = new CCard(CUtilities::GetCurrentTime(),CUtilities::GetUTCTime());
	card->SetCardID(_T("12345"));
	CPlayerInfo* playerInfo(new CPlayerInfo());
    CGames games;
	CPlayer player(card, playerInfo, config, egmConfig, CUtilities::GetCurrentTime(), false, &games);

	varPersonalBanker.BeginPersonalBankerSession(&player, pbPin, config);
	varPersonalBanker.PreparePersonalBankerTransfer(sequenceNumber);

	//Test
	{
		bool areEqual = sequenceInitialValue + 1 ==  sequenceNumber;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED sequenceNumber=%d expected=%d in %s"),
			sequenceNumber,
			sequenceInitialValue + 1,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::RemoveSession()
/// </summary>
TEST_F(PersonalBanker2, RemoveSessionTest)
{
	CPersonalBanker varPersonalBanker;
         
    CreateInfo(varPersonalBanker);
	std::string failedMsg;
	bool areEqual(false);
	const bool expectedAutoNcepOutPending = true;
	const DWORD expectedAutoUploadPin = PBT_AUTO_UPLOAD_PIN;
	CConfig config(false, "");
	const CEGMConfig egmConfig(MEMORY_NONE);
	LPCTSTR pbPin = _T("1234");
	CCard *card = new CCard(CUtilities::GetCurrentTime(),CUtilities::GetUTCTime());
	card->SetCardID(_T("12345"));
	CPlayerInfo* playerInfo(new CPlayerInfo());
    CGames games;
	CPlayer player(card, playerInfo, config, egmConfig, CUtilities::GetCurrentTime(), false, &games);

	//Tests
	{
		varPersonalBanker.BeginPersonalBankerSession(&player, pbPin, config);
		failedMsg = FormatString(_T("FAILED Transfer object should exist in %s"),
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(varPersonalBanker.HasTransfer()) << failedMsg;

		varPersonalBanker.RemoveSession();
		failedMsg = FormatString(_T("FAILED Transfer object should not exist in %s"),
			static_cast<LPCTSTR>(__TFUNCTION__));

		//Make the state set to PBStateWaitingForUploadToStart
		varPersonalBanker.BeginPersonalBankerAutoSession(false, config, expectedAutoUploadPin, CUtilities::GetTickCount());

		//Make Personal Banker Type set to AutomaticUploadCanceledByUser
		varPersonalBanker.ReleasePersonalBankerSession(config, true);

		//Make Auto Ncep Out Pending to true
		varPersonalBanker.BeginPersonalBankerAutoSession(false, config, expectedAutoUploadPin, CUtilities::GetTickCount());

		//The call below will the cause the if statement to be true
		//if((m_autoUpload.IsAutoUploadPending() || m_autoUpload.IsAutoInProcess()) && 
		//	!m_pbTransfer->IsTransferTypeToGame() && m_pbTransfer->IsTransferRequestCanceled())
		varPersonalBanker.ReleasePersonalBankerSession(config, false);

		//After the line of code above StartUploadOver should be set to true
		varPersonalBanker.RemoveSession();
		failedMsg = FormatString(_T("FAILED Transfer object should not exist in %s"),
			static_cast<LPCTSTR>(__TFUNCTION__));

		bool areEqual = expectedAutoNcepOutPending ==  varPersonalBanker.IsAutoUploadPending();
		failedMsg = FormatString(_T("FAILED AutoNcepOutPending=%d expected=%d in %s"),
			(int)varPersonalBanker.IsAutoUploadPending(),
			(int)expectedAutoNcepOutPending,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;

		// Cleanup.
	}
}


/// <summary>
/// Test the implementation of:
///     CPersonalBanker::ProcessPersonalBankerTransferComplete()
/// </summary>
TEST_F(PersonalBanker2, ProcessPersonalBankerTransferCompleteTest)
{
	PersonalBankerAction action(PBActionNone);
	CPersonalBanker varPersonalBanker;
         
    CreateInfo(varPersonalBanker);
	CConfig config(false, "");
	const CEGMConfig egmConfig(MEMORY_NONE);
	LPCTSTR pbPin = _T("1234");
	CCard *card = new CCard(CUtilities::GetCurrentTime(),CUtilities::GetUTCTime());
	card->SetCardID(_T("12345"));
	CPlayerInfo* playerInfo(new CPlayerInfo());
    CGames games;
	CPlayer player(card, playerInfo, config, egmConfig, CUtilities::GetCurrentTime(), false, &games);

	bool isDuplicate(false);
	byte meterNumberToAddTo(mNone);

	varPersonalBanker.BeginPersonalBankerSession(&player, pbPin, config);

	CPersonalBankerTransfer* transferCopy(varPersonalBanker.GetTransferCopy());

	varPersonalBanker.ProcessPersonalBankerTransferComplete(transferCopy,
		games,
		meterNumberToAddTo,
		isDuplicate,
		false);

	//Tests
	{
		bool areEqual = action ==  varPersonalBanker.GetAction();
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED action=%d expected=%d in %s"),
			(int)varPersonalBanker.GetAction(),
			(int)action,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	delete transferCopy;
}


/// <summary>
/// Test the implementation of:
///     CPersonalBanker::CheckIfPersonalBankerTypeHasToBeChanged() through ProcessPersonalBankerTransferComplete
/// Perform Code Coverage: CheckIfTransferMatchWhatWeHaveInMemory and CreatePreviousTransferId
/// </summary>
TEST_F(PersonalBanker2, CheckIfPersonalBankerTypeHasToBeChangedIsDuplicateTest)
{
	const char FIRST_ID[MAX_PBID_LENGTH] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j' };
	const char SECOND_ID[MAX_PBID_LENGTH] = { '1', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j' };
	// Test data.
	typedef struct
	{
		// Input
		const char* const ID1;
		const char* const ID2;
		PersonalBankerTransferType personalBankerTransferType;
		PersonalBankerType bankerType;
		bool callProcessPersonalBankerInfo;

		// Expected result
		bool expectedIsDuplicate;
	} TestDataType;

	const TestDataType TEST_DATA[] =
	{
		{FIRST_ID, FIRST_ID, CashToGame, CanceledSessionRequest, false, true},
		{FIRST_ID, SECOND_ID, PromoDollarsFromGame, CanceledSessionRequest, false, false},
		{FIRST_ID, SECOND_ID, PromoDollarsFromGame, CanceledSessionRequest, true, false},
	};

	CPersonalBanker varPersonalBanker;
	CConfig config(false, "");
         
    CreateInfo(varPersonalBanker);
	const CEGMConfig egmConfig(MEMORY_NONE);
	CCard *card = new CCard(CUtilities::GetCurrentTime(),CUtilities::GetUTCTime());
	card->SetCardID(_T("12345"));
	CPlayerInfo* playerInfo(new CPlayerInfo());
    CGames games;
	CPlayer player(card, playerInfo, config, egmConfig, CUtilities::GetCurrentTime(), false, &games);
	bool isDuplicate(false);
	byte meterNumberToAddTo(mNone);
	const DWORD amountInDollars = 5; 
	byte pbEFTNumber(1);
	const PersonalBankerTransferType type = PointsToGame;			

	CPersonalBankerTransfer* transfer(new CPersonalBankerTransfer(CUtilities::GetTickCount()));
	CPersonalBankerInfo *pbInfo(new CPersonalBankerInfo());
	transfer->SetPIN(PBT_AUTO_UPLOAD_PIN);
	for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
	{
		transfer->SetID(TEST_DATA[testIndex].ID1, MAX_PBID_LENGTH);
		varPersonalBanker.ProcessPersonalBankerTransferComplete(transfer,
			games,
			meterNumberToAddTo,
			isDuplicate,
			false);
		varPersonalBanker.SetState(PBStateInfoReceived);
		varPersonalBanker.BeginPersonalBankerTransfer(games, type, amountInDollars, pbEFTNumber);
		varPersonalBanker.LogTransfer(&player, config, CUtilities::GetCurrentTime());
		varPersonalBanker.DoneWithTransfer();
		transfer->SetID(TEST_DATA[testIndex].ID2, MAX_PBID_LENGTH);
		varPersonalBanker.SetState(PBStateInfoReceived);
		transfer->SetPersonalBankerTransferType(TEST_DATA[testIndex].personalBankerTransferType);
		transfer->SetPersonalBankerType(TEST_DATA[testIndex].bankerType);
		transfer->SetPromoPersonalBankerTransferType(TEST_DATA[testIndex].personalBankerTransferType);

		if(TEST_DATA[testIndex].callProcessPersonalBankerInfo)
		{
			//Code coverage setup for CheckIfTransferMatchWhatWeHaveInMemory so that
			// if(m_pbTransfer->GetPromoPersonalBankerTransferType() == PromoDollarsFromGame && m_pbTransfer->GetPIN() == -1)
			// can be met
			WORD pbSequence(0);
			const char PBTID[MAX_PBID_LENGTH] = {'1'};
			varPersonalBanker.CreateTransfer(CUtilities::GetTickCount());
			varPersonalBanker.SetAutoInProcess(true);
			pbInfo->SetPersonalBankerID(PBTID, MAX_PBID_LENGTH);
			pbInfo->SetErrorCode(NoError);
			varPersonalBanker.SetState(PBStateInquireSent);
			varPersonalBanker.ProcessPersonalBankerInfo(pbInfo,
				config,
				false,
				false);
			varPersonalBanker.SetState(PBStateWaitingForUploadToStart);
			varPersonalBanker.ReleasePersonalBankerSession(config, true);

			//Code coverage for CreatePreviousTransferId
			varPersonalBanker.SetState(PBStateTransferingFromToGame);
			varPersonalBanker.PreparePersonalBankerTransfer(pbSequence);
		}

		varPersonalBanker.ProcessPersonalBankerTransferComplete(transfer,
			games,
			meterNumberToAddTo,
			isDuplicate,
			false);

		//Test
		{
			bool areEqual = TEST_DATA[testIndex].expectedIsDuplicate ==  isDuplicate;
			std::string failedMsg;
			failedMsg = FormatString(_T("FAILED PersonalBankerType=%d expected=%d testIndex=%d in %s"),
				(int)isDuplicate,
				(int)TEST_DATA[testIndex].expectedIsDuplicate,
				testIndex,
				static_cast<LPCTSTR>(__TFUNCTION__));
			ASSERT_TRUE(areEqual) << failedMsg;
		}
	}

	//Cleanup
	delete transfer;
	delete pbInfo;
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::CheckIfPersonalBankerTypeHasToBeChanged() through ProcessPersonalBankerTransferComplete
/// </summary>
TEST_F(PersonalBanker2, CheckIfPersonalBankerTypeHasToBeChangedTest)
{
	// Test data.
	CPersonalBanker varPersonalBanker;
	CConfig config(false, "");
	const CEGMConfig egmConfig(MEMORY_NONE);
	CCard *card = new CCard(CUtilities::GetCurrentTime(),CUtilities::GetUTCTime());
	card->SetCardID(_T("12345"));
	CPlayerInfo* playerInfo(new CPlayerInfo());
    CGames games;
	CPlayer player(card, playerInfo, config, egmConfig, CUtilities::GetCurrentTime(), false, &games);
	bool isDuplicate(false);
	byte meterNumberToAddTo(mNone);
	const DWORD amountInDollars = 5; 
	byte pbEFTNumber(1);
	const PersonalBankerTransferType type = PointsToGame;			
	CPersonalBankerTransfer* transfer(new CPersonalBankerTransfer(CUtilities::GetTickCount()));
	const char FIRST_ID[MAX_PBID_LENGTH] = 
		{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j' };
	const char SECOND_ID[MAX_PBID_LENGTH] = 
		{ '1', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j' };
	transfer->SetID(FIRST_ID, MAX_PBID_LENGTH);
	varPersonalBanker.ProcessPersonalBankerTransferComplete(transfer,
		games,
		meterNumberToAddTo,
		isDuplicate,
		false);
	varPersonalBanker.SetState(PBStateInfoReceived);
	varPersonalBanker.BeginPersonalBankerTransfer(games, type, amountInDollars, pbEFTNumber);
	varPersonalBanker.LogTransfer(&player, config, CUtilities::GetCurrentTime());
	varPersonalBanker.DoneWithTransfer();
	transfer->SetID(SECOND_ID, MAX_PBID_LENGTH);
	varPersonalBanker.ProcessPersonalBankerTransferComplete(transfer,
		games,
		meterNumberToAddTo,
		isDuplicate,
		false);

	typedef struct
	{
		// Input
		const PersonalBankerType originalType;
				
		// Expected result
		const PersonalBankerType expextedType;

	} TestDataType;

	const TestDataType TEST_DATA[] =
	{
		{CashUpload, UnexpectedCashFromGame},
		{CanceledCashUpload, UnexpectedCashFromGameErr},
		{CashDownload, UnexpectedCashToGame},
		{CanceledCashDownload, UnexpectedCashToGameErr},
		{PromoCreditsToCash, UnexpectedRestrictedToGame},
		{CanceledPromoCreditsToCash, UnexpectedRestrictedToGameErr},
		{PointRedemptionToCash, UnexpectedUnRestrictedToGame},
		{CanceledPointRedemptionToPromoCredits, UnexpectedRestrictedToGameErr},
		{PointRedemptionToPromoCredits, UnexpectedRestrictedToGame},
		{CanceledPointRedemption, UnexpectedUnrestrictedToGameErr},
		{PromoCreditsUpload, UnexpectedRestrictedFromGame},
		{CanceledPromoCreditsUpload, UnexpectedRestrictedFromGame},
		{PromoCreditsUploadAuto, UnexpectedRestrictedFromGame},
		{CanceledPromoCreditsUploadAuto, UnexpectedRestrictedFromGame},
	};

	varPersonalBanker.SetState(PBStateInfoReceived);
	for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
	{
		// Setup.
		transfer->SetPersonalBankerType(TEST_DATA[testIndex].originalType);
		varPersonalBanker.ProcessPersonalBankerTransferComplete(transfer,
			games,
			meterNumberToAddTo,
			isDuplicate,
			false);

		//Test
		{
			CPersonalBankerTransfer* transferCopy(varPersonalBanker.GetTransferCopy());
			bool areEqual = TEST_DATA[testIndex].expextedType ==  transferCopy->GetPersonalBankerType();
			std::string failedMsg;
			failedMsg = FormatString(_T("FAILED PersonalBankerType=%d expected=%d testIndex=%d in %s"),
				(int)varPersonalBanker.GetPersonalBankerTransferType(),
				(int)TEST_DATA[testIndex].expextedType,
				testIndex,
				static_cast<LPCTSTR>(__TFUNCTION__));
			ASSERT_TRUE(areEqual) << failedMsg;

			//Cleanup
			delete transferCopy;
		}

	}
	delete transfer;
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::SetToReleaseTransferSession()
/// </summary>
TEST_F(PersonalBanker2, SetToReleaseTransferSessionTest)
{
	// Test data.
	const PersonalBankerAction action = PBActionNone;

	// Setup.
	CPersonalBanker varPersonalBanker;
	varPersonalBanker.SetToReleaseTransferSession();
			
	//Test
	{
		bool areEqual = action ==  varPersonalBanker.GetAction();
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED action=%d expected=%d in %s"),
			(int)varPersonalBanker.GetAction(),
			(int)action,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::ReleasePersonalBankerSession()
/// </summary>
TEST_F(PersonalBanker2, ReleasePersonalBankerSessionTest)
{
	// Test data.
	const PersonalBankerAction action = PBActionNone;
	CConfig config(false, "");
	DWORD autoUploadPin(PBT_AUTO_UPLOAD_PIN);
	bool canceledFromUI(false);			
	// Setup.
	CPersonalBanker varPersonalBanker;

	//Tests
	{
		//#1 No Transfer object test
		varPersonalBanker.ReleasePersonalBankerSession(config, canceledFromUI);
		bool areEqual = action ==  varPersonalBanker.GetAction();
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED action=%d expected=%d in %s"),
			(int)varPersonalBanker.GetAction(),
			(int)action,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
		failedMsg = FormatString(_T("FAILED Transfer object should not exist in %s"),
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_FALSE(varPersonalBanker.HasTransfer()) << failedMsg;
	}
	// Cleanup.
}

/// <summary>
/// Test ClearPersonalBankerAction after session released.
/// </summary>
TEST_F(PersonalBanker2, ReleasePersonalBankerSessionAndClearPersonalBankerActionTest)
{
	// Test data.
	const PersonalBankerAction action = PBActionNone;
	const PersonalBankerState stateForUi = PBStateUnknown;
	const bool canceledFromUI = false;
	CConfig config(false, "");
	DWORD autoUploadPin(0);

	// Setup.
	CPersonalBanker varPersonalBanker;

	//Tests
	{
		//#1 Release an existing transfer with Auto NCEP pending
		varPersonalBanker.BeginPersonalBankerAutoSession(false, config, autoUploadPin, CUtilities::GetTickCount());
		varPersonalBanker.ReleasePersonalBankerSession(config, canceledFromUI);
		varPersonalBanker.ClearPersonalBankerAction();
		bool areEqual = action ==  varPersonalBanker.GetAction();
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED action=%d expected=%d in %s"),
			(int)varPersonalBanker.GetAction(),
			(int)action,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
		failedMsg = FormatString(_T("FAILED SessionReleaseDueToTimeout should be 'false' in %s"),
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_FALSE(varPersonalBanker.IsSessionReleasedDueToTimeout()) << failedMsg;
		areEqual = stateForUi ==  varPersonalBanker.GetPbStateForUi();
		failedMsg = FormatString(_T("FAILED stateForUi=%d expected=%d in %s"),
			(int)varPersonalBanker.GetPbStateForUi(),
			(int)stateForUi,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}
	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::ReleasePersonalBankerSession()
/// </summary>
TEST_F(PersonalBanker2, ReleasePersonalBankerSessionAutoUploadPendingTest)
{
	// Test data.
	const bool expectedAutoNcep = true;
	const bool canceledFromUI = false;
	CConfig config(false, "");
	DWORD autoUploadPin(PBT_AUTO_UPLOAD_PIN);

	// Setup.
	CPersonalBanker varPersonalBanker;

	//Tests
	{
		//#1 Release an existing transfer with Auto NCEP pending
		varPersonalBanker.BeginPersonalBankerAutoSession(false, config, autoUploadPin, CUtilities::GetTickCount());
		varPersonalBanker.ReleasePersonalBankerSession(config, canceledFromUI);
		bool areEqual = expectedAutoNcep ==  varPersonalBanker.IsAutoUploadPending();
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED action=%d expected=%d in %s"),
			(int)varPersonalBanker.IsAutoUploadPending(),
			(int)expectedAutoNcep,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}
	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::ReleasePersonalBankerSession()
/// </summary>
TEST_F(PersonalBanker2, ReleasePersonalBankerSessionCancelledFromUiTest)
{
	// Test data.
	std::string failedMsg;
	bool areEqual(false);
	CPersonalBankerTransfer* transferCopy(NULL);
	const PersonalBankerAction expectedAction = PBActionSendAutoUploadCancelledMessage;
	const bool expectedAutoNcepOutPending = false;
	const PollerCancellationType expectedPollerCancellationType = AutomaticUploadCanceledByUser;
	const PersonalBankerType expectedPersonalBankerType = SessionRequest;
	const PersonalBankerState expectedPersonalBankerState = PBStateTransferComplete;
	const DWORD expectedAutoUploadPin = PBT_AUTO_UPLOAD_PIN;	
	const WORD expectedDenom = 1;
	const bool canceledFromUI = true;
	const bool sendToPoller = false;
			
	CConfig config(false, "");
			
	// Setup.
	CPersonalBanker varPersonalBanker;
	config.SetDenomination(expectedDenom);			

	//Tests
	{
		//#1 Make sure action gets set to PBActionSendAutoUploadCancelledMessage after calling ReleasePersonalBankerSession
		//Calling the next line will set the state to PBStateWaitingForUploadToStart
		varPersonalBanker.BeginPersonalBankerAutoSession(sendToPoller, config, expectedAutoUploadPin, CUtilities::GetTickCount());
		varPersonalBanker.ReleasePersonalBankerSession(config, canceledFromUI);
		areEqual = expectedAction ==  varPersonalBanker.GetAction();
		failedMsg = FormatString(_T("FAILED action=%d expected=%d in %s"),
			(int)varPersonalBanker.GetAction(),
			(int)expectedAction,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;

		//#2
		areEqual = expectedAutoNcepOutPending == varPersonalBanker.IsAutoUploadPending();
		failedMsg = FormatString(_T("FAILED AutoNcepOutPending=%d expected=%d in %s"),
			(int)varPersonalBanker.IsAutoUploadPending(),
			(int)expectedAutoNcepOutPending,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;

		transferCopy = varPersonalBanker.GetTransferCopy();

		//#3
		areEqual = expectedPollerCancellationType == transferCopy->GetCancelledType();
		failedMsg = FormatString(_T("FAILED PollerCancellationType=%d expected=%d in %s"),
			(int)transferCopy->GetCancelledType(),
			(int)expectedPollerCancellationType,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;

		//#4
		areEqual = expectedPersonalBankerType == transferCopy->GetPersonalBankerType();
		failedMsg = FormatString(_T("FAILED PersonalBankerType=%d expected=%d in %s"),
			(int)transferCopy->GetPersonalBankerType(),
			(int)expectedPersonalBankerType,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;

		//#5
		areEqual = expectedPersonalBankerType == transferCopy->GetPersonalBankerType();
		failedMsg = FormatString(_T("FAILED PersonalBankerType=%d expected=%d in %s"),
			(int)transferCopy->GetPersonalBankerType(),
			(int)expectedPersonalBankerType,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;

		//#6
		areEqual = expectedPersonalBankerState == varPersonalBanker.GetState();
		failedMsg = FormatString(_T("FAILED PersonalBankerState=%d expected=%d in %s"),
			(int)varPersonalBanker.GetState(),
			(int)expectedPersonalBankerState,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
				
		//#7
		areEqual = expectedAutoUploadPin == transferCopy->GetPIN();
		failedMsg = FormatString(_T("FAILED AutoUploadPin=%d expected=%d in %s"),
			(int)transferCopy->GetPIN(),
			(int)expectedAutoUploadPin,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;

		//#8
		areEqual = expectedDenom == transferCopy->GetDenomination();
		failedMsg = FormatString(_T("FAILED Denom=%d expected=%d in %s"),
			(int)transferCopy->GetDenomination(),
			(int)expectedAutoUploadPin,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Cleanup.
	delete transferCopy;
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::ReleasePersonalBankerSession()
/// </summary>
TEST_F(PersonalBanker2, ReleasePersonalBankerSessionSendToUiTest)
{
	// Setup
	CPersonalBanker varPersonalBanker;
	CConfig config(false, "");
	const CEGMConfig egmConfig(MEMORY_NONE);
	std::string failedMsg;
	bool areEqual(false);
	const PersonalBankerAction expectedAction = PBActionSendCompleteToUI;
	const bool expectedWaitingForCompleteFromUI = true;
	const PersonalBankerState expectedStateForUi = PBStateInquireSent;
	const bool canceledFromUI = false;


	LPCTSTR pbPin = _T("1234");
	CCard *card = new CCard(CUtilities::GetCurrentTime(),CUtilities::GetUTCTime());
	card->SetCardID(_T("12345"));
	CPlayerInfo* playerInfo(new CPlayerInfo());
    CGames games;
	CPlayer player(card, playerInfo, config, egmConfig, CUtilities::GetCurrentTime(), false, &games);
	varPersonalBanker.BeginPersonalBankerSession(&player, pbPin, config);
	varPersonalBanker.ReleasePersonalBankerSession(config, canceledFromUI);

	//Tests
	{
		//#1
		areEqual = expectedAction ==  varPersonalBanker.GetAction();
		failedMsg = FormatString(_T("FAILED action=%d expected=%d in %s"),
			(int)varPersonalBanker.GetAction(),
			(int)expectedAction,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;

		//#2
		areEqual = expectedStateForUi ==  varPersonalBanker.GetPbStateForUi();
		failedMsg = FormatString(_T("FAILED StateForUi=%d expected=%d in %s"),
			(int)varPersonalBanker.GetPbStateForUi(),
			(int)expectedStateForUi,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::ProcessPersonalBankerTransactionAcked()
/// </summary>
TEST_F(PersonalBanker2, ProcessPersonalBankerTransactionAckedTest)
{
	// Test data.
	const PersonalBankerAction action = PBActionNone;
	WORD sequenceNumber(0);
	// Setup.
	CPersonalBanker varPersonalBanker;
	varPersonalBanker.ProcessPersonalBankerTransactionAcked(sequenceNumber);
			
	//Test
	{
		bool areEqual = action ==  varPersonalBanker.GetAction();
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED action=%d expected=%d in %s"),
			(int)varPersonalBanker.GetAction(),
			(int)action,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;

		failedMsg = FormatString(_T("FAILED Transfer object should not exist in %s"),
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_FALSE(varPersonalBanker.HasTransfer()) << failedMsg;
	}

	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::IsTransferRequestCanceled()
/// </summary>
TEST_F(PersonalBanker2, IsTransferRequestCanceledTest)
{
	// Test data.
	const PersonalBankerAction action = PBActionNone;
	WORD sequenceNumber(0);

	// Setup.
	CPersonalBanker varPersonalBanker;
			
	//Test
	{
		bool areEqual = varPersonalBanker.HasTransfer() ==  varPersonalBanker.IsTransferRequestCanceled();
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED Tranfer should not exist in %s"),
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Cleanup.
}
		
/// <summary>
/// Test the implementation of:
///     CPersonalBanker::GetPersonalBankerTransferType()
/// </summary>
/// <remarks>
/// </remarks>
TEST_F(PersonalBanker2, GetPersonalBankerTransferTypeDefaultTest)
{
	// Test data.
	const PersonalBankerTransferType personalBankerTransferTypeExpected = Undeclared;

	// Setup.
	CPersonalBanker varPersonalBanker;
	const CPersonalBanker * const personalBanker = const_cast<const CPersonalBanker *>(&varPersonalBanker);
			
	// Test #1.
	{
		bool personalBankerTransferTypeResult = personalBanker->GetPersonalBankerTransferType();


		bool areEqual = personalBankerTransferTypeResult ==  personalBankerTransferTypeExpected;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED personalBankerTransferTypeResult=%d expected=%d in %s"),
			(int)personalBankerTransferTypeResult,
			(int)personalBankerTransferTypeExpected,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::GetPersonalBankerTransferType()
/// </summary>
/// <remarks>
/// </remarks>
TEST_F(PersonalBanker2, GetPersonalBankerTransferTypeWithTransferTest)
{
	// Test data.
	const PersonalBankerTransferType personalBankerTransferTypeExpected = Undeclared;

	// Setup.
	CPersonalBanker varPersonalBanker;
	varPersonalBanker.CreateTransfer(CUtilities::GetTickCount());
	const CPersonalBanker * const personalBanker = const_cast<const CPersonalBanker *>(&varPersonalBanker);
			
	// Test #1.
	{
		bool personalBankerTransferTypeResult = personalBanker->GetPersonalBankerTransferType();


		bool areEqual = personalBankerTransferTypeResult ==  personalBankerTransferTypeExpected;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED personalBankerTransferTypeResult=%d expected=%d in %s"),
			(int)personalBankerTransferTypeResult,
			(int)personalBankerTransferTypeExpected,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::GetPersonalBankerTransferType()
/// </summary>
/// <remarks>
/// </remarks>
TEST_F(PersonalBanker2, GetPersonalBankerTransferTypeWithoutTransferTest)
{
	// Test data.
	const PersonalBankerTransferType personalBankerTransferTypeExpected = Undeclared;

	// Setup.
	CPersonalBanker varPersonalBanker;
	varPersonalBanker.DoneWithTransfer();
	const CPersonalBanker * const personalBanker = const_cast<const CPersonalBanker *>(&varPersonalBanker);
			
	// Test #1.
	{
		bool personalBankerTransferTypeResult = personalBanker->GetPersonalBankerTransferType();


		bool areEqual = personalBankerTransferTypeResult ==  personalBankerTransferTypeExpected;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED personalBankerTransferTypeResult=%d expected=%d in %s"),
			(int)personalBankerTransferTypeResult,
			(int)personalBankerTransferTypeExpected,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::SetPersonalBankerType()
/// </summary>
/// <remarks>
/// The side effects of CPersonalBanker::SetPersonalBankerType() are not directly testable.
/// </remarks>
TEST_F(PersonalBanker2, SetPersonalBankerTypeDefaultTest)
{
	// Test data.
	PersonalBankerType type = Unspecified;
	const bool autoNcepOutPendingExpected = false;

	// Setup.
	CPersonalBanker varPersonalBanker;
	varPersonalBanker.SetPersonalBankerType(type);
	const CPersonalBanker * const personalBanker = const_cast<const CPersonalBanker *>(&varPersonalBanker);
			
	// Test #1.
	{
		bool autoNcepOutPendingResult = personalBanker->IsAutoUploadPending();

		bool areEqual = autoNcepOutPendingResult ==  autoNcepOutPendingExpected;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED autoNcepOutPendingResult=%d expected=%d in %s"),
			(int)autoNcepOutPendingResult,
			(int)autoNcepOutPendingExpected,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::SetPersonalBankerType()
/// </summary>
/// <remarks>
/// The side effects of CPersonalBanker::SetPersonalBankerType() are not directly testable.
/// </remarks>
TEST_F(PersonalBanker2, SetPersonalBankerTypeWithTransferTest)
{
	// Test data.
	PersonalBankerType type = Unspecified;
	const bool autoNcepOutPendingExpected = false;

	// Setup.
	CPersonalBanker varPersonalBanker;
	varPersonalBanker.CreateTransfer(CUtilities::GetTickCount());
	varPersonalBanker.SetPersonalBankerType(type);
	const CPersonalBanker * const personalBanker = const_cast<const CPersonalBanker *>(&varPersonalBanker);
			
	// Test #1.
	{
		bool autoNcepOutPendingResult = personalBanker->IsAutoUploadPending();

		bool areEqual = autoNcepOutPendingResult ==  autoNcepOutPendingExpected;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED autoNcepOutPendingResult=%d expected=%d in %s"),
			(int)autoNcepOutPendingResult,
			(int)autoNcepOutPendingExpected,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::SetPersonalBankerType()
/// </summary>
/// <remarks>
/// The side effects of CPersonalBanker::SetPersonalBankerType() are not directly testable.
/// </remarks>
TEST_F(PersonalBanker2, SetPersonalBankerTypeWithoutTransferTest)
{
	// Test data.
	PersonalBankerType type = Unspecified;
	const bool autoNcepOutPendingExpected = false;

	// Setup.
	CPersonalBanker varPersonalBanker;
	varPersonalBanker.DoneWithTransfer();
	varPersonalBanker.SetPersonalBankerType(type);
	const CPersonalBanker * const personalBanker = const_cast<const CPersonalBanker *>(&varPersonalBanker);
			
	// Test #1.
	{
		bool autoNcepOutPendingResult = personalBanker->IsAutoUploadPending();

		bool areEqual = autoNcepOutPendingResult ==  autoNcepOutPendingExpected;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED autoNcepOutPendingResult=%d expected=%d in %s"),
			(int)autoNcepOutPendingResult,
			(int)autoNcepOutPendingExpected,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}
	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::SetAutoUploadTimerExpired()
/// </summary>
/// <remarks>
/// The side effects of CPersonalBanker::SetAutoUploadTimerExpired() are not directly testable.
/// </remarks>
TEST_F(PersonalBanker2, SetAutoUploadTimerExpiredTest)
{
	// Test data.
	const bool autoNcepOutPendingExpected = false;

	// Setup.
	CPersonalBanker varPersonalBanker;
	varPersonalBanker.SetAutoUploadTimerExpired();
	const CPersonalBanker * const personalBanker = const_cast<const CPersonalBanker *>(&varPersonalBanker);
			
	// Test #1.
	{
		bool autoNcepOutPendingResult = personalBanker->IsAutoUploadPending();

		bool areEqual = autoNcepOutPendingResult ==  autoNcepOutPendingExpected;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED autoNcepOutPendingResult=%d expected=%d in %s"),
			(int)autoNcepOutPendingResult,
			(int)autoNcepOutPendingExpected,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::IsAutoUploadPending()
/// </summary>
TEST_F(PersonalBanker2, IsAutoNcepOutPendingDefaultTest)
{
	// Test data.
	const bool autoNcepOutPendingExpected = false;

	// Setup.
	const CPersonalBanker personalBanker;
			
	// Test #1.
	{
		bool autoNcepOutPendingResult = personalBanker.IsAutoUploadPending();

		bool areEqual = autoNcepOutPendingResult ==  autoNcepOutPendingExpected;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED autoNcepOutPendingResult=%d expected=%d in %s"),
			(int)autoNcepOutPendingResult,
			(int)autoNcepOutPendingExpected,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::MakeAutoUploadPending()
///     CPersonalBanker::IsAutoUploadPending()
/// </summary>
TEST_F(PersonalBanker2, IsAutoNcepOutPendingTrueTest)
{
	// Test data.
	const bool autoNcepOutPendingExpected = true;

	// Setup.
	CPersonalBanker varPersonalBanker;
	varPersonalBanker.MakeAutoUploadPending();
	const CPersonalBanker * const personalBanker = const_cast<const CPersonalBanker *>(&varPersonalBanker);
			
	// Test #1.
	{
		bool autoNcepOutPendingResult = personalBanker->IsAutoUploadPending();

		bool areEqual = autoNcepOutPendingResult ==  autoNcepOutPendingExpected;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED autoNcepOutPendingResult=%d expected=%d in %s"),
			(int)autoNcepOutPendingResult,
			(int)autoNcepOutPendingExpected,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::MakeAutoUploadPending()
///     CPersonalBanker::ProcessCancelPersonalBankerSession()
///     CPersonalBanker::IsAutoUploadPending()
/// </summary>
TEST_F(PersonalBanker2, IsAutoNcepOutPendingFalseTest)
{
	// Test data.
	const bool autoNcepOutPendingExpected = false;

	// Setup.
	CPersonalBanker varPersonalBanker;
	varPersonalBanker.MakeAutoUploadPending();
	varPersonalBanker.ProcessCancelPersonalBankerSession(NULL);
	const CPersonalBanker * const personalBanker = const_cast<const CPersonalBanker *>(&varPersonalBanker);
			
	// Test #1.
	{
		bool autoNcepOutPendingResult = personalBanker->IsAutoUploadPending();

		bool areEqual = autoNcepOutPendingResult ==  autoNcepOutPendingExpected;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED autoNcepOutPendingResult=%d expected=%d in %s"),
			(int)autoNcepOutPendingResult,
			(int)autoNcepOutPendingExpected,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::ProcessCancelPersonalBankerSession()
///     with existing Transfer and in PBStateWaitingForUploadToStart state
/// </summary>
TEST_F(PersonalBanker2, ProcessCancelPersonalBankerSessionWithPBStateWaitingForUploadToStartTest)
{
	// Test data.
	const bool releasePersonalBankerSessionExpected = true;

	// Setup.
	CPersonalBanker varPersonalBanker;
	varPersonalBanker.CreateTransfer(CUtilities::GetTickCount());
	varPersonalBanker.SetState(PBStateWaitingForUploadToStart);
	bool releasePersonalBankerSessionResult = varPersonalBanker.ProcessCancelPersonalBankerSession(NULL);
			
	// Test #1.
	{
		bool areEqual = releasePersonalBankerSessionResult ==  releasePersonalBankerSessionExpected;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED autoNcepOutPendingResult=%d expected=%d in %s"),
			(int)releasePersonalBankerSessionResult,
			(int)releasePersonalBankerSessionExpected,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::CheckIfAuthorizationIsNeeded()
/// </summary>
TEST_F(PersonalBanker2, CheckIfAuthorizationIsNeededIfTransferringToGameTest)
{
	// Test data.
	const DWORD amountInDollars = 5; 
	const bool atomicRedemptions = true;
	const bool expectedAuthorizationIsNeeded = true;
	bool AuthorizationIsNeeded = false;
			
	// Setup.
	CPersonalBanker varPersonalBanker;
         
    CreateInfo(varPersonalBanker);
	varPersonalBanker.CreateTransfer(CUtilities::GetTickCount());
	varPersonalBanker.SetState(PBStateInfoReceived);
						
	// Test #1.
	{
		const PersonalBankerTransferType type = PointsToGame;
		AuthorizationIsNeeded = varPersonalBanker.CheckIfAuthorizationIsNeeded(type, amountInDollars, atomicRedemptions);
		bool areEqual = AuthorizationIsNeeded ==  expectedAuthorizationIsNeeded;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED AuthorizationIsNeeded=%d expected=%d in %s"),
			(int)AuthorizationIsNeeded,
			(int)expectedAuthorizationIsNeeded,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Test #2.
	{
		const PersonalBankerTransferType type = CashToGame;
		AuthorizationIsNeeded = varPersonalBanker.CheckIfAuthorizationIsNeeded(type, amountInDollars, atomicRedemptions);
		bool areEqual = AuthorizationIsNeeded ==  expectedAuthorizationIsNeeded;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED AuthorizationIsNeeded=%d expected=%d in %s"),
			(int)AuthorizationIsNeeded,
			(int)expectedAuthorizationIsNeeded,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Test #3.
	{
		const PersonalBankerTransferType type = PromoDollarsToGame;
		AuthorizationIsNeeded = varPersonalBanker.CheckIfAuthorizationIsNeeded(type, amountInDollars, atomicRedemptions);
		bool areEqual = AuthorizationIsNeeded ==  expectedAuthorizationIsNeeded;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED AuthorizationIsNeeded=%d expected=%d in %s"),
			(int)AuthorizationIsNeeded,
			(int)expectedAuthorizationIsNeeded,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::CheckIfAuthorizationIsNeeded()
/// </summary>
TEST_F(PersonalBanker2, CheckIfAuthorizationIsNeededIfTransferringFromGamedTest)
{
	// Test data.
	const DWORD amountInDollars = 5; 
	const bool atomicRedemptions = true;
	const bool expectedAuthorizationIsNeeded = false;
	bool AuthorizationIsNeeded = false;
			
	// Setup.
	CPersonalBanker varPersonalBanker;
         
    CreateInfo(varPersonalBanker);
	varPersonalBanker.CreateTransfer(CUtilities::GetTickCount());
	varPersonalBanker.SetState(PBStateInfoReceived);
						
	// Test #1.
	{
		const PersonalBankerTransferType type = Undeclared;
		AuthorizationIsNeeded = varPersonalBanker.CheckIfAuthorizationIsNeeded(type, amountInDollars, atomicRedemptions);
		bool areEqual = AuthorizationIsNeeded ==  expectedAuthorizationIsNeeded;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED AuthorizationIsNeeded=%d expected=%d in %s"),
			(int)AuthorizationIsNeeded,
			(int)expectedAuthorizationIsNeeded,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Test #2.
	{
		const PersonalBankerTransferType type = CashFromGame;
		AuthorizationIsNeeded = varPersonalBanker.CheckIfAuthorizationIsNeeded(type, amountInDollars, atomicRedemptions);
		bool areEqual = AuthorizationIsNeeded ==  expectedAuthorizationIsNeeded;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED AuthorizationIsNeeded=%d expected=%d in %s"),
			(int)AuthorizationIsNeeded,
			(int)expectedAuthorizationIsNeeded,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Test #3.
	{
		const PersonalBankerTransferType type = PointsFromGame;
		AuthorizationIsNeeded = varPersonalBanker.CheckIfAuthorizationIsNeeded(type, amountInDollars, atomicRedemptions);
		bool areEqual = AuthorizationIsNeeded ==  expectedAuthorizationIsNeeded;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED AuthorizationIsNeeded=%d expected=%d in %s"),
			(int)AuthorizationIsNeeded,
			(int)expectedAuthorizationIsNeeded,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Test #4.
	{
		const PersonalBankerTransferType type = PromoDollarsFromGame;
		AuthorizationIsNeeded = varPersonalBanker.CheckIfAuthorizationIsNeeded(type, amountInDollars, atomicRedemptions);
		bool areEqual = AuthorizationIsNeeded ==  expectedAuthorizationIsNeeded;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED AuthorizationIsNeeded=%d expected=%d in %s"),
			(int)AuthorizationIsNeeded,
			(int)expectedAuthorizationIsNeeded,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Test #5.
	{
		const PersonalBankerTransferType type = AllFromGame;
		AuthorizationIsNeeded = varPersonalBanker.CheckIfAuthorizationIsNeeded(type, amountInDollars, atomicRedemptions);
		bool areEqual = AuthorizationIsNeeded ==  expectedAuthorizationIsNeeded;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED AuthorizationIsNeeded=%d expected=%d in %s"),
			(int)AuthorizationIsNeeded,
			(int)expectedAuthorizationIsNeeded,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::CheckIfAuthorizationIsNeeded()
/// </summary>
TEST_F(PersonalBanker2, CheckIfAuthorizationIsNeededIfTransferIsNullTest)
{
	// Test data.
	const DWORD amountInDollars = 5; 
	const bool atomicRedemptions = true;
	const bool expectedAuthorizationIsNeeded = false;
	bool AuthorizationIsNeeded = false;
			
	// Setup.
	CPersonalBanker varPersonalBanker;
    CreateInfo(varPersonalBanker);
						
	// Test #1.
	{
		const PersonalBankerTransferType type = PointsToGame;
		AuthorizationIsNeeded = varPersonalBanker.CheckIfAuthorizationIsNeeded(type, amountInDollars, atomicRedemptions);
		bool areEqual = AuthorizationIsNeeded ==  expectedAuthorizationIsNeeded;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED AuthorizationIsNeeded=%d expected=%d in %s"),
			(int)AuthorizationIsNeeded,
			(int)expectedAuthorizationIsNeeded,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}
	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::CheckIfAuthorizationIsNeeded()
/// </summary>
TEST_F(PersonalBanker2, CheckIfAuthorizationIsNeededIfAtomicRedemptionsIsFalseTest)
{
	// Test data.
	const DWORD amountInDollars = 5; 
	const bool atomicRedemptions = false;
	const bool expectedAuthorizationIsNeeded = false;
	bool AuthorizationIsNeeded = false;
			
	// Setup.
	CPersonalBanker varPersonalBanker;
         
    CreateInfo(varPersonalBanker);
	varPersonalBanker.CreateTransfer(CUtilities::GetTickCount());
	varPersonalBanker.SetState(PBStateInfoReceived);
						
	// Test #1.
	{
		const PersonalBankerTransferType type = PointsToGame;
		AuthorizationIsNeeded = varPersonalBanker.CheckIfAuthorizationIsNeeded(type, amountInDollars, atomicRedemptions);
		bool areEqual = AuthorizationIsNeeded ==  expectedAuthorizationIsNeeded;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED AuthorizationIsNeeded=%d expected=%d in %s"),
			(int)AuthorizationIsNeeded,
			(int)expectedAuthorizationIsNeeded,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}
	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::CheckIfAuthorizationIsNeeded()
/// </summary>
TEST_F(PersonalBanker2, CheckIfAuthorizationIsNeededIfStateIsNotInfoReceivedTest)
{
	// Test data.
	const DWORD amountInDollars = 5; 
	const bool atomicRedemptions = true;
	const bool expectedAuthorizationIsNeeded = false;
	bool AuthorizationIsNeeded = false;
			
	// Setup.
	CPersonalBanker varPersonalBanker;
         
    CreateInfo(varPersonalBanker);
	varPersonalBanker.CreateTransfer(CUtilities::GetTickCount());
	varPersonalBanker.SetState(PBStateInquireSent);
						
	// Test #1.
	{
		const PersonalBankerTransferType type = PointsToGame;
		AuthorizationIsNeeded = varPersonalBanker.CheckIfAuthorizationIsNeeded(type, amountInDollars, atomicRedemptions);
		bool areEqual = AuthorizationIsNeeded ==  expectedAuthorizationIsNeeded;
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED AuthorizationIsNeeded=%d expected=%d in %s"),
			(int)AuthorizationIsNeeded,
			(int)expectedAuthorizationIsNeeded,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}
	// Cleanup.
}		

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::ProcessPersonalBankerCreditTransfer()
/// </summary>
TEST_F(PersonalBanker2, ProcessPersonalBankerCreditTransferNullTest)
{
	// Test data.
	const PersonalBankerState expectedState = PBStateUnknown;
			
	// Setup.
	CPersonalBanker varPersonalBanker;
         
    CreateInfo(varPersonalBanker);
	CPersonalBankerInfo *pbInfo(new CPersonalBankerInfo());
	varPersonalBanker.SetState(PBStateUnknown);
	pbInfo->SetErrorCode(CreditTransferCommserror);
						
	// Test
	{
		varPersonalBanker.ProcessPersonalBankerCreditTransfer(pbInfo);
		bool areEqual = expectedState ==  varPersonalBanker.GetState();
		std::string failedMsg;
		failedMsg = FormatString(_T("FAILED State=%d expected=%d in %s"),
			(int)varPersonalBanker.GetState(),
			(int)expectedState,
			static_cast<LPCTSTR>(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}
	// Cleanup.
	delete pbInfo;
}	

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::ProcessPersonalBankerCreditTransfer()
/// </summary>
TEST_F(PersonalBanker2, ProcessPersonalBankerCreditTransferErrorTests)
{
	//Test Data
	typedef struct
	{
		// Inputs.
		PersonalBankerState stateInput;
		PBError pbErrorInput;

		// Expected results.
		PollerCancellationType cancellationTypeExpected;
		PersonalBankerState stateExpected;
		PersonalBankerAction actionExpected;
	} TestDataType;

	const TestDataType TEST_DATA[] =
	{
		{ PBStateUnknown, CreditTransferCommserror, CreditTransferErrorComms, PBStateTransferComplete, PBActionSendCompleteToUI},		
		{ PBStateUnknown, CreditTransferInsufficientCredits, CreditTransferErrorInsufficientCredits, PBStateTransferComplete, PBActionSendCompleteToUI},
		{ PBStateUnknown, CreditTransferOtherIssue, CreditTransferErrorOther, PBStateTransferComplete, PBActionSendCompleteToUI},
		{ PBStateUnknown, InvalidID, CreditTransferErrorUnknown, PBStateTransferComplete, PBActionSendCompleteToUI},
	};
			
	for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
	{
		// Setup.
		bool areEqual(false);
		CPersonalBanker varPersonalBanker;
         
        CreateInfo(varPersonalBanker);
		varPersonalBanker.CreateTransfer(CUtilities::GetTickCount());
		CPersonalBankerTransfer* personalBankerTransfer(NULL);
		CPersonalBankerInfo *pbInfo(new CPersonalBankerInfo());
		varPersonalBanker.SetPersonalBankerType(CreditTransfer);
		varPersonalBanker.SetState(PBStateUnknown);
		pbInfo->SetErrorCode(TEST_DATA[testIndex].pbErrorInput);
		varPersonalBanker.ProcessPersonalBankerCreditTransfer(pbInfo);

		// Test #1
		{
			areEqual = TEST_DATA[testIndex].stateExpected ==  varPersonalBanker.GetState();
			std::string failedMsg;
			failedMsg = FormatString(_T("FAILED State=%d expected=%d in %s"),
				(int)varPersonalBanker.GetState(),
				(int)TEST_DATA[testIndex].stateExpected,
				static_cast<LPCTSTR>(__TFUNCTION__));
			ASSERT_TRUE(areEqual) << failedMsg;
		}

		// Test #2
		{
			personalBankerTransfer = varPersonalBanker.GetTransferCopy();
			areEqual = TEST_DATA[testIndex].cancellationTypeExpected ==  personalBankerTransfer->GetCancelledType();
			std::string failedMsg;
			failedMsg = FormatString(_T("FAILED CancellationType=%d expected=%d in %s"),
				(int)personalBankerTransfer->GetCancelledType(),
				(int)TEST_DATA[testIndex].cancellationTypeExpected,
				static_cast<LPCTSTR>(__TFUNCTION__));
			ASSERT_TRUE(areEqual) << failedMsg;
		}

		// Cleanup.
		delete pbInfo;
		delete personalBankerTransfer;
	}
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::ProcessCancelPersonalBankerSession()
/// </summary>
TEST_F(PersonalBanker2, ProcessCancelPersonalBankerSessionForCredit)
{
	//Test Data
	typedef struct
	{
		// Inputs.
		CPersonalBankerTransfer* inputTransfer;
		bool inputDisplayCancelSession;
		PersonalBankerType inputPersonalBankerType;
		PersonalBankerTransferType inputPersonalBankerTransfer;				

		// Expected results.
		PersonalBankerType expectedPersonalBankerType;
		bool expectedReleaseFlag;
	} TestDataType;

	const TestDataType TEST_DATA[] =
	{
		{ NULL, false, CanceledCreditTransfer, ExternalCredit, Unspecified, false},
		{ new CPersonalBankerTransfer(0), true, CanceledCreditTransfer, ExternalCredit, CreditTransfer, true},
	};
			
	for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
	{
		// Setup.
		CPersonalBanker varPersonalBanker;
		varPersonalBanker.CreateTransfer(0);

		if(TEST_DATA[testIndex].inputTransfer != NULL)
		{
			TEST_DATA[testIndex].inputTransfer->SetPersonalBankerTransferType(ExternalCredit);
			TEST_DATA[testIndex].inputTransfer->SetPersonalBankerType(CanceledCreditTransfer);
		}

		bool releaseFlag = varPersonalBanker.ProcessCancelPersonalBankerSession(TEST_DATA[testIndex].inputTransfer, 
			TEST_DATA[testIndex].inputDisplayCancelSession);

		// Tests
		{
			bool areEqual = TEST_DATA[testIndex].expectedReleaseFlag == releaseFlag;

			std::string failedMsg;
			failedMsg = FormatString(_T("FAILED Release Flag=%d expected=%d in %s"),
				(int)releaseFlag,
				(int)TEST_DATA[testIndex].expectedReleaseFlag,
				static_cast<LPCTSTR>(__TFUNCTION__));
			ASSERT_TRUE(areEqual) << failedMsg;

			if(releaseFlag)
			{
				CPersonalBankerTransfer* transferCopy(varPersonalBanker.GetTransferCopy());
				if(transferCopy != NULL)
				{
					areEqual =  TEST_DATA[testIndex].expectedPersonalBankerType == transferCopy->GetPersonalBankerType();
					failedMsg = FormatString(_T("FAILED Release Flag=%d expected=%d in %s"),
						(int)transferCopy->GetPersonalBankerType(),
						(int)TEST_DATA[testIndex].expectedPersonalBankerType,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;

					delete transferCopy;
				}
			}

			// Cleanup.
			if(TEST_DATA[testIndex].inputTransfer != NULL)
			{
				delete TEST_DATA[testIndex].inputTransfer;	
			}
		}
	}
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::ProcessCancelAutoUPloadPersonalBankerSession()
/// </summary>
TEST_F(PersonalBanker2, ProcessCancelAutoUPloadPersonalBankerSession)
{
	// Setup
	CPersonalBanker varPersonalBanker;
	CConfig config(false, "");

	//Tests
	{
		//Make the state set to PBStateWaitingForUploadToStart
		varPersonalBanker.BeginPersonalBankerAutoSession(false, config, PBT_AUTO_UPLOAD_PIN, CUtilities::GetTickCount());
		//Make Auto Ncep Out Pending to true
		varPersonalBanker.BeginPersonalBankerAutoSession(false, config, PBT_AUTO_UPLOAD_PIN, CUtilities::GetTickCount());
		//The call below will the cause the if statement to be true
		//if((m_autoUpload.IsAutoUploadPending() || m_autoUpload.IsAutoInProcess()) && 
		//	!m_pbTransfer->IsTransferTypeToGame() && m_pbTransfer->IsTransferRequestCanceled())
		varPersonalBanker.ReleasePersonalBankerSession(config, false);
		ASSERT_TRUE(varPersonalBanker.IsAutoUploadPending());

		//Make the state set to PBStateWaitingForUploadToStart
		varPersonalBanker.RemoveSession();
		varPersonalBanker.BeginPersonalBankerAutoSession(false, config, PBT_AUTO_UPLOAD_PIN, CUtilities::GetTickCount());
		//Make Auto Ncep Out Pending to true
		varPersonalBanker.BeginPersonalBankerAutoSession(false, config, PBT_AUTO_UPLOAD_PIN, CUtilities::GetTickCount());
		//The call below will the cause the if statement to be true
		//if((m_autoUpload.IsAutoUploadPending() || m_autoUpload.IsAutoInProcess()) && 
		//	!m_pbTransfer->IsTransferTypeToGame() && m_pbTransfer->IsTransferRequestCanceled())
		varPersonalBanker.m_pbTransfer->SetPersonalBankerTransferType(CashFromGame);
		varPersonalBanker.m_pbTransfer->SetPersonalBankerType(CanceledCashUploadAuto);
		bool sendToUI(false);
		varPersonalBanker.ReleasePersonalBankerSessionAutoUpload(true, sendToUI);
		ASSERT_TRUE(varPersonalBanker.IsAutoUploadPending());
		ASSERT_TRUE(varPersonalBanker.m_autoUpload.IsStartUploadOver());
		ASSERT_FALSE(sendToUI);
	}
}

TEST_F(PersonalBanker2, PerformTimedEventsAutoCancelPendingAutoUpload)
{
	// Setup
	CPersonalBanker varPersonalBanker;
	CConfig config(false, "");
	const PersonalBankerAction expectedAction = PBActionSendAutoTransferCancelledToUI;

	//Tests
	{
		varPersonalBanker.SetToDelayAutoUpload();
		varPersonalBanker.PerformTimedEventsAuto(false, false, config, 3000);
		ASSERT_FALSE(varPersonalBanker.m_readyAction == expectedAction);
		varPersonalBanker.m_autoUploadPendingFlags.m_flags.m_cancelPendingAutoUpload = true;
		varPersonalBanker.m_autoUploadPendingFlags.m_flags.m_cancellable = true;
		varPersonalBanker.PerformTimedEventsAuto(false, false, config, 3000);
		ASSERT_TRUE(varPersonalBanker.m_readyAction == expectedAction);
	}

	// Cleanup.
}

TEST_F(PersonalBanker2, SetAutoUploadDelayValueTest)
{
	// Setup
	CPersonalBanker varPersonalBanker;
	const DWORD expectedDelayValue(3);
	varPersonalBanker.SetAutoUploadDelayValue(expectedDelayValue);

	//Tests
	{
		ASSERT_TRUE(expectedDelayValue == varPersonalBanker.m_autoUploadDelaySeconds);
	}

	// Cleanup.
}

TEST_F(PersonalBanker2, SetToDelayAutoUploadCashTest)
{
	// Setup
	CPersonalBanker varPersonalBanker;
	varPersonalBanker.SetToDelayAutoUpload();

	//Tests
	{
		ASSERT_TRUE(varPersonalBanker.m_autoUploadPendingFlags.m_flags.m_delayAutoUpload);
		ASSERT_FALSE(varPersonalBanker.m_autoUploadPendingFlags.m_flags.m_cancelPendingAutoUpload);
		ASSERT_FALSE(varPersonalBanker.m_autoUploadPendingFlags.m_flags.m_cancellable);
	}

	// Cleanup.
}

TEST_F(PersonalBanker2, CancelPendingAutoUploadIfNecessaryTest)
{
	// Setup
	CPersonalBanker varPersonalBanker;
	varPersonalBanker.SetToDelayAutoUpload();

	//Tests
	{
		ASSERT_TRUE(varPersonalBanker.m_autoUploadPendingFlags.m_flags.m_delayAutoUpload);
		ASSERT_FALSE(varPersonalBanker.m_autoUploadPendingFlags.m_flags.m_cancelPendingAutoUpload);
		ASSERT_FALSE(varPersonalBanker.m_autoUploadPendingFlags.m_flags.m_cancellable);
		varPersonalBanker.CancelPendingAutoUploadIfNecessary(0);
		ASSERT_TRUE(varPersonalBanker.m_autoUploadPendingFlags.m_flags.m_delayAutoUpload);
		ASSERT_TRUE(varPersonalBanker.m_autoUploadPendingFlags.m_flags.m_cancelPendingAutoUpload);
		ASSERT_FALSE(varPersonalBanker.m_autoUploadPendingFlags.m_flags.m_cancellable);
	}

	// Cleanup.
}

TEST_F(PersonalBanker2, DelayAutoUploadTest)
{
	// Setup
	CPersonalBanker varPersonalBanker;
	const DWORD expectedDelayValue(1);
	varPersonalBanker.SetAutoUploadDelayValue(expectedDelayValue);
	varPersonalBanker.SetToDelayAutoUpload();

	//Tests
	{
		ASSERT_TRUE(expectedDelayValue == varPersonalBanker.m_autoUploadDelaySeconds);
		ASSERT_TRUE(varPersonalBanker.m_autoUploadPendingFlags.m_flags.m_delayAutoUpload);
		ASSERT_FALSE(varPersonalBanker.m_autoUploadPendingFlags.m_flags.m_cancelPendingAutoUpload);
		ASSERT_FALSE(varPersonalBanker.m_autoUploadPendingFlags.m_flags.m_cancellable);
		ASSERT_TRUE(varPersonalBanker.DelayAutoUpload());
		CUtilities::Sleep(2 * expectedDelayValue * MILLISECONDS_PER_SECOND);
		ASSERT_FALSE(varPersonalBanker.DelayAutoUpload());
	}

	// Cleanup.
}

TEST_F(PersonalBanker2, ProcessBalancesFromEGMDelayAutoUploadTest)
{
	// Setup
	CPersonalBanker varPersonalBanker;
	CConfig config(false, "");
	varPersonalBanker.m_guestPlayer = false;
	varPersonalBanker.m_cashInactive = false;	
	config.SetPoller_12_11_6000_or_Higher(true);
	config.SetSenOption2Raw(0xffffffff);
	config.SetPBFromGameEnabled(true);
	config.SetPBTCashEnabled_Global(true);	

	const DWORD expectedWat(1);
	const DWORD expectedCep(2);
	const DWORD expectedNcep(0);

	varPersonalBanker.SetToDelayAutoUpload();

	//Tests
	{
		varPersonalBanker.ProcessBalancesFromEGM(expectedWat, expectedCep, expectedNcep, true, false, config, false);
		ASSERT_TRUE(varPersonalBanker.m_autoUpload.GetPendingAutoWatAmount() == expectedWat);
		ASSERT_TRUE(varPersonalBanker.m_autoUpload.GetPendingAutoCepAmount() == expectedCep);
		ASSERT_TRUE(varPersonalBanker.m_autoUpload.GetPendingAutoNcepAmount() == expectedNcep);
		ASSERT_TRUE(varPersonalBanker.m_autoUpload.IsAutoUploadPending());
		ASSERT_FALSE(varPersonalBanker.m_autoUpload.IsAutoInProcess());
		ASSERT_TRUE(varPersonalBanker.m_autoUpload.IsAutoStartedOnce());
	}

	// Cleanup.
}


/// <summary>
/// Test the implementation of:
///     CPersonalBanker::GetCashoutErrorFlags()
/// </summary>
TEST_F(PersonalBanker2, GetCashoutErrorFlags_DefaultTest)
{
	// Setup.
	CPersonalBanker varPersonalBanker;

	// Default value test
	ASSERT_EQ(varPersonalBanker.GetCashoutErrorFlags(), 0) << "Default value is incorrect.";
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::GetAutoUploadErrorFlags()
/// </summary>
TEST_F(PersonalBanker2, GetAutoUploadErrorFlags_DefaultTest)
{
	// Setup.
	CPersonalBanker varPersonalBanker;

	// Default value test
	ASSERT_EQ(varPersonalBanker.GetAutoUploadErrorFlags(), 0) << "Default value is incorrect.";
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::ValidateCreditsForCashoutToHost()
/// </summary>
TEST_F(PersonalBanker2, ValidateCreditsForCashoutToHost_NullPlayer)
{
	// Setup.
	CPersonalBanker varPersonalBanker;
	DWORD eligibleCash = 100;
	DWORD eligiblePromo = 200;

	// Operate
	WORD cashoutErrorFlag = varPersonalBanker.ValidateCreditsForCashoutToHost(NULL, eligibleCash, eligiblePromo);

	// Verify
	ASSERT_EQ(cashoutErrorFlag, 0) << "Cashout error flag is incorrect.";
	ASSERT_EQ(eligibleCash, 100) << "Eligible cash value is incorrect.";
	ASSERT_EQ(eligiblePromo, 200) << "Eligible promo value is incorrect.";
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::ValidateCreditsForCashoutToHost()
/// </summary>
TEST_F(PersonalBanker2, ValidateCreditsForCashoutToHost_MaxCardLimitReached)
{
	// Setup.
	CPersonalBanker varPersonalBanker;
	DWORD eligibleCash = 0;
	DWORD eligiblePromo = 200;
	CPlayerInfo *playerInfo = new CPlayerInfo();
	playerInfo->MaxCardLimitReached(true);
	CPlayer *player = new CPlayer();
	player->SetPlayerInfo(playerInfo);

	// Operate
	WORD cashoutErrorFlag = varPersonalBanker.ValidateCreditsForCashoutToHost(player, eligibleCash, eligiblePromo);

	// Verify
	ASSERT_EQ(cashoutErrorFlag, CashoutError::CASHOUT_ERROR_MAX_CARD_LIMIT) << "Cashout error flag is incorrect.";
	ASSERT_EQ(eligibleCash, 0) << "Eligible cash value is incorrect.";
	ASSERT_EQ(eligiblePromo, 0) << "Eligible promo value is incorrect.";

	// Cleanup
	delete player;
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::ValidateCreditsForCashoutToHost()
/// </summary>
TEST_F(PersonalBanker2, ValidateCreditsForCashoutToHost_MaxCardLimitReached_CashOnGame)
{
	// Setup.
	CPersonalBanker varPersonalBanker;
	DWORD eligibleCash = 100;
	DWORD eligiblePromo = 200;
	CPlayerInfo *playerInfo = new CPlayerInfo();
	playerInfo->MaxCardLimitReached(true);
	CPlayer *player = new CPlayer();
	player->SetPlayerInfo(playerInfo);

	// Operate
	WORD cashoutErrorFlag = varPersonalBanker.ValidateCreditsForCashoutToHost(player, eligibleCash, eligiblePromo);

	// Verify
	ASSERT_EQ(cashoutErrorFlag, CashoutError::CASHOUT_ERROR_MAX_CARD_LIMIT_CASH_ON_GAME) << "Cashout error flag is incorrect.";
	ASSERT_EQ(eligibleCash, 0) << "Eligible cash value is incorrect.";
	ASSERT_EQ(eligiblePromo, 0) << "Eligible promo value is incorrect.";

	// Cleanup
	delete player;
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::ValidateCreditsForCashoutToHost()
/// </summary>
TEST_F(PersonalBanker2, ValidateCreditsForCashoutToHost_GuestPlayer)
{
	// Setup.
	CPersonalBanker varPersonalBanker;
	DWORD eligibleCash = 0;
	DWORD eligiblePromo = 200;
	CPlayer player;

	// Operate
	WORD cashoutErrorFlag = varPersonalBanker.ValidateCreditsForCashoutToHost(&player, eligibleCash, eligiblePromo);

	// Verify
	ASSERT_EQ(cashoutErrorFlag, CashoutError::CASHOUT_ERROR_GUEST_PLAYER) << "Cashout error flag is incorrect.";
	ASSERT_EQ(eligibleCash, 0) << "Eligible cash value is incorrect.";
	ASSERT_EQ(eligiblePromo, 0) << "Eligible promo value is incorrect.";
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::ValidateCreditsForCashoutToHost()
/// </summary>
TEST_F(PersonalBanker2, ValidateCreditsForCashoutToHost_GuestPlayer_CashOnGame)
{
	// Setup.
	CPersonalBanker varPersonalBanker;
	DWORD eligibleCash = 100;
	DWORD eligiblePromo = 200;
	CPlayer player;

	// Operate
	WORD cashoutErrorFlag = varPersonalBanker.ValidateCreditsForCashoutToHost(&player, eligibleCash, eligiblePromo);

	// Verify
	ASSERT_EQ(cashoutErrorFlag, CashoutError::CASHOUT_ERROR_GUEST_PLAYER_CASH_ON_GAME) << "Cashout error flag is incorrect.";
	ASSERT_EQ(eligibleCash, 0) << "Eligible cash value is incorrect.";
	ASSERT_EQ(eligiblePromo, 0) << "Eligible promo value is incorrect.";
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::ValidateCreditsForCashoutToHost()
/// </summary>
TEST_F(PersonalBanker2, ValidateCreditsForCashoutToHost_CashInactive)
{
	// Setup.
	CPersonalBanker varPersonalBanker;
	varPersonalBanker.m_cashInactive = true;
	DWORD eligibleCash = 100;
	DWORD eligiblePromo = 200;
	CPlayerInfo *playerInfo = new CPlayerInfo();
	playerInfo->SetCardNumber((byte*)"1234567890", CARD_ID_LEN);
	playerInfo->SetName("DummyName");
	CPlayer *player = new CPlayer();
	player->SetPlayerInfo(playerInfo);

	// Operate
	WORD cashoutErrorFlag = varPersonalBanker.ValidateCreditsForCashoutToHost(player, eligibleCash, eligiblePromo);

	// Verify
	ASSERT_EQ(cashoutErrorFlag, CashoutError::CASHOUT_ERROR_CASH_INACTIVE) << "Cashout error flag is incorrect.";
	ASSERT_EQ(eligibleCash, 0) << "Eligible cash value is incorrect.";
	ASSERT_EQ(eligiblePromo, 200) << "Eligible promo value is incorrect.";

	// Cleanup
	delete player;
}
