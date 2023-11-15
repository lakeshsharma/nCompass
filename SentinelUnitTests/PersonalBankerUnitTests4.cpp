#include "stdafx.h"

#include "PersonalBanker.h"


#include "gtest/gtest.h"

using namespace std;

const time_t CTIME_DEFAULT_VALUE(0); // Arbitrary value when any valid value is okay.
const byte MEMSET_DEFAULT_VALUE = 0xcd;
const int NVRAMCHECKBYTEOFFSET = 4; // From CInternalsFramework::ProcessClearPersistentMemory().

class PersonalBanker4 : public ::testing::Test
{
	public:
        void CreateInfo(CPersonalBanker &personalBanker)        
    {
        personalBanker.CreateInfo(MEMORY_NONE, false);
    }
};

/// Test the implementation of:
///     CPersonalBanker::ProcessPersonalBankerInfo()
/// Code coverage test with parameter pbInfo==NULL.
/// </summary>
TEST_F(PersonalBanker4, ProcessPersonalBankerInfo_NullTest)
{
	// Test data - inputs.
	const CConfig config(false, "");
	const bool gameOnline(false);
	const bool bIsEFT(false);

	// Test data - expected results.
	const PersonalBankerAction actionExpected = PBActionNone;

	// Setup.
	CPersonalBankerInfo * const pbInfo = NULL;
	CPersonalBanker varPb;
	varPb.ClearPersonalBankerAction();

	// Perform operations to be tested.
	varPb.ProcessPersonalBankerInfo(pbInfo, config, gameOnline, bIsEFT);
	const CPersonalBanker &pb(varPb);

	// Test #1.
	{
		const PersonalBankerAction action = pb.GetAction();

		const bool areEqual = action == actionExpected;
		string failedMsg;
		failedMsg = FormatString("FAILED action=%d expected=%d in %s",
			action,
			actionExpected,
			(__TFUNCTION__));
		ASSERT_TRUE(areEqual) <<  failedMsg;
	}

	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::ProcessPersonalBankerInfo()
/// Code coverage test with CPersonalBanker:m_pbtAuthorizationInProcess == true.
/// </summary>
TEST_F(PersonalBanker4, ProcessPersonalBankerInfo_AuthorizationInProcessTest)
{
	// Test data - inputs.
	const PersonalBankerState state(PBStateInfoReceived);
	const PersonalBankerTransferType type(PointsToGame);
	const DWORD amountInDollars(0);
	const bool atomicRedemptions(true);
	const CConfig config(false, "");
	const bool gameOnline(false);
	const bool bIsEFT(false);

	// Test data - expected results.
	const bool setupPbtAuthorizationInProcessExpected = true;
	const PersonalBankerAction actionExpected = PBActionNone;

	// Setup.
	const CPersonalBankerInfo pbInfo;
	CPersonalBanker varPb;
         
    CreateInfo(varPb);
	varPb.CreateTransfer(CUtilities::GetTickCount());
	varPb.SetState(state);
	const bool setupPbtAuthorizationInProcess = varPb.CheckIfAuthorizationIsNeeded(type, amountInDollars, atomicRedemptions);
	varPb.ClearPersonalBankerAction();

	// Verify setup #A.
	{
		const bool areEqual = setupPbtAuthorizationInProcess == setupPbtAuthorizationInProcessExpected;
		string failedMsg;
		failedMsg = FormatString("FAILED setupPbtAuthorizationInProcess=%d expected=%d in %s",
			setupPbtAuthorizationInProcess,
			setupPbtAuthorizationInProcessExpected,
			(__TFUNCTION__));
		ASSERT_TRUE(areEqual) <<  failedMsg;
	}

	// Perform operations to be tested.
	varPb.ProcessPersonalBankerInfo(&pbInfo, config, gameOnline, bIsEFT);
	const CPersonalBanker &pb(varPb);

	// Test #1.
	{
		const PersonalBankerAction action = pb.GetAction();

		const bool areEqual = action == actionExpected;
		string failedMsg;
		failedMsg = FormatString("FAILED action=%d expected=%d in %s",
			action,
			actionExpected,
			(__TFUNCTION__));
		ASSERT_TRUE(areEqual) <<  failedMsg;
	}

	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::ProcessPersonalBankerInfo()
/// Code coverage test with CPersonalBanker:m_pbTransfer == NULL.
/// </summary>
TEST_F(PersonalBanker4, ProcessPersonalBankerInfo_TransferNullTest)
{
	// Test data - inputs.
	//const PersonalBankerState state(PBStateInfoReceived);
	//const PersonalBankerTransferType type(PointsToGame);
	//const DWORD amountInDollars(0);
	//const bool atomicRedemptions(true);
	const CConfig config(false, "");
	const bool gameOnline(false);
	const bool bIsEFT(false);

	// Test data - expected results.
	const PersonalBankerAction actionExpected = PBActionNone;

	// Setup.
	const CPersonalBankerInfo pbInfo;
	CPersonalBanker varPb;
         
    CreateInfo(varPb);
	varPb.DoneWithTransfer();
	varPb.ClearPersonalBankerAction();

	// Perform operations to be tested.
	varPb.ProcessPersonalBankerInfo(&pbInfo, config, gameOnline, bIsEFT);
	const CPersonalBanker &pb(varPb);

	// Test #1.
	{
		const PersonalBankerAction action = pb.GetAction();

		const bool areEqual = action == actionExpected;
		string failedMsg;
		failedMsg = FormatString("FAILED action=%d expected=%d in %s",
			action,
			actionExpected,
			(__TFUNCTION__));
		ASSERT_TRUE(areEqual) <<  failedMsg;
	}

	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::ProcessPersonalBankerInfo()
/// Code coverage test with CPersonalBanker:m_pbTransfer.m_pbtType == SessionRelease.
/// </summary>
TEST_F(PersonalBanker4, ProcessPersonalBankerInfo_TransferSessionReleaseTest)
{
	// Test data - inputs.
	const PersonalBankerType pbType(SessionRelease);
	const CConfig config(false, "");
	const bool gameOnline(false);
	const bool bIsEFT(false);

	// Test data - expected results.
	const PersonalBankerAction actionExpected = PBActionNone;

	// Setup.
	const CPersonalBankerInfo pbInfo;
	CPersonalBanker varPb;
         
    CreateInfo(varPb);
	varPb.CreateTransfer(CUtilities::GetTickCount());
	varPb.SetPersonalBankerType(pbType);
	varPb.ClearPersonalBankerAction();

	// Perform operations to be tested.
	varPb.ProcessPersonalBankerInfo(&pbInfo, config, gameOnline, bIsEFT);
	const CPersonalBanker &pb(varPb);

	// Test #1.
	{
		const PersonalBankerAction action = pb.GetAction();

		const bool areEqual = action == actionExpected;
		string failedMsg;
		failedMsg = FormatString("FAILED action=%d expected=%d in %s",
			action,
			actionExpected,
			(__TFUNCTION__));
		ASSERT_TRUE(areEqual) <<  failedMsg;
	}

	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::ProcessPersonalBankerInfo()
/// Code coverage test with CPersonalBanker:m_previousTransferId value the same as the pbInfo.m_PBTID value.
/// </summary>
TEST_F(PersonalBanker4, ProcessPersonalBankerInfo_PreviousTransferIdSameTest)
{
	// Test data - inputs.
	const int pbtIDLength = 1;
	const char pbtID[pbtIDLength] = { '1' };
	const PersonalBankerType pbType(Unspecified);
	const PersonalBankerState pbState(PBStateTransferingFromToGame);
	const CConfig config(false, "");
	const bool gameOnline(false);
	const bool bIsEFT(false);
	const bool isEFT(false);

	// Test data - expected results.
	const PersonalBankerAction actionExpected = PBActionNone;

	// Setup.
	CPersonalBankerInfo varPbInfo;
	varPbInfo.SetPersonalBankerID(pbtID, pbtIDLength);
	const CPersonalBankerInfo &pbInfo(varPbInfo);
	CPersonalBanker varPb;
         
    CreateInfo(varPb);
	varPb.DoneWithTransfer();
	varPb.SetPersonalBankerType(pbType);

	CPersonalBankerTransfer pbTransfer(CUtilities::GetTickCount());
	pbTransfer.SetID(pbtID, pbtIDLength);
	CGames games;
	byte meterNumberToAddTo(mNone);
	bool isDuplicate(false);
	varPb.ProcessPersonalBankerTransferComplete(&pbTransfer,
		games,
		meterNumberToAddTo,
		isDuplicate,
		isEFT);

	varPb.SetState(pbState);
	WORD pbSequence = 1;
	varPb.PreparePersonalBankerTransfer(pbSequence);

	varPb.ClearPersonalBankerAction();

	// Perform operations to be tested.
	varPb.ProcessPersonalBankerInfo(&pbInfo, config, gameOnline, bIsEFT);
	const CPersonalBanker &pb(varPb);

	// Test #1.
	{
		const PersonalBankerAction action = pb.GetAction();

		const bool areEqual = action == actionExpected;
		string failedMsg;
		failedMsg = FormatString("FAILED action=%d expected=%d in %s",
			action,
			actionExpected,
			(__TFUNCTION__));
		ASSERT_TRUE(areEqual) <<  failedMsg;
	}

	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::ProcessPersonalBankerInfo()
/// Code coverage test with CPersonalBanker:m_previousTransferIdLength == 0.
/// </summary>
TEST_F(PersonalBanker4, ProcessPersonalBankerInfo_PreviousTransferIdLengthZeroTest)
{
	// Test data - inputs.
	const char* pbtID = NULL;
	const int pbtIDLength = 0;
	const PersonalBankerType pbType(Unspecified);
	const CConfig config(false, "");
	const bool gameOnline(false);
	const bool bIsEFT(false);
	const bool isEFT(false);

	// Test data - expected results.
	const PersonalBankerAction actionExpected = PBActionGetEGMBalances;

	// Setup.
	const CPersonalBankerInfo pbInfo;
	CPersonalBanker varPb;
         
    CreateInfo(varPb);
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
		isEFT);

	varPb.SetPersonalBankerType(pbType);
	varPb.ClearPersonalBankerAction();

	// Perform operations to be tested.
	varPb.ProcessPersonalBankerInfo(&pbInfo, config, gameOnline, bIsEFT);
	const CPersonalBanker &pb(varPb);

	// Test #1.
	{
		const PersonalBankerAction action = pb.GetAction();

		const bool areEqual = action == actionExpected;
		string failedMsg;
		failedMsg = FormatString("FAILED action=%d expected=%d in %s",
			action,
			actionExpected,
			(__TFUNCTION__));
		ASSERT_TRUE(areEqual) <<  failedMsg;
	}

	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::ProcessPersonalBankerInfo()
/// Code coverage test with CPersonalBanker:m_previousTransferIdLength == 1 and pbInfo.m_PBIDLength == 0.
/// </summary>
TEST_F(PersonalBanker4, ProcessPersonalBankerInfo_InfoTransferIdLengthZeroTest)
{
	// Test data - inputs.
	const int pbtIDLength = 1;
	const char pbtID[pbtIDLength] = { '1' };
	const PersonalBankerType pbType(Unspecified);
	const PersonalBankerState pbState(PBStateTransferingFromToGame);
	const CConfig config(false, "");
	const bool gameOnline(false);
	const bool bIsEFT(false);
	const bool isEFT(false);

	// Test data - expected results.
	const PersonalBankerAction actionExpected = PBActionGetEGMBalances;

	// Setup.
	CPersonalBankerInfo pbInfo;
	CPersonalBanker varPb;
         
    CreateInfo(varPb);
	varPb.DoneWithTransfer();
	varPb.SetPersonalBankerType(pbType);

	CPersonalBankerTransfer pbTransfer(CUtilities::GetTickCount());
	pbTransfer.SetID(pbtID, pbtIDLength);
	CGames games;
	byte meterNumberToAddTo(mNone);
	bool isDuplicate(false);
	varPb.ProcessPersonalBankerTransferComplete(&pbTransfer,
		games,
		meterNumberToAddTo,
		isDuplicate,
		isEFT);

	varPb.SetState(pbState);
	WORD pbSequence = 1;
	varPb.PreparePersonalBankerTransfer(pbSequence);

	varPb.ClearPersonalBankerAction();

	// Perform operations to be tested.
	varPb.ProcessPersonalBankerInfo(&pbInfo, config, gameOnline, bIsEFT);
	const CPersonalBanker &pb(varPb);

	// Test #1.
	{
		const PersonalBankerAction action = pb.GetAction();

		const bool areEqual = action == actionExpected;
		string failedMsg;
		failedMsg = FormatString("FAILED action=%d expected=%d in %s",
			action,
			actionExpected,
			(__TFUNCTION__));
		ASSERT_TRUE(areEqual) <<  failedMsg;
	}

	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::ProcessPersonalBankerInfo()
/// Code coverage test with CPersonalBanker:m_previousTransferIdLength == pbInfo.m_PBIDLength,
/// but with CPersonalBanker:m_previousTransferId value different than the pbInfo.m_PBTID value.
/// </summary>
TEST_F(PersonalBanker4,  ProcessPersonalBankerInfo_InfoTransferIdDifferentTest)
{
	// Test data - inputs.
	const int pbtIDLength = 1;
	const char pbtID[pbtIDLength] = { '1' };
	const int infoPbtIDLength = pbtIDLength;
	const char infoPbtID[infoPbtIDLength] = { '2' };
	const PersonalBankerType pbType(Unspecified);
	const PersonalBankerState pbState(PBStateTransferingFromToGame);
	const CConfig config(false, "");
	const bool gameOnline(false);
	const bool bIsEFT(false);
	const bool isEFT(false);

	// Test data - expected results.
	const PersonalBankerAction actionExpected = PBActionGetEGMBalances;

	// Setup.
	CPersonalBankerInfo varPbInfo;
	varPbInfo.SetPersonalBankerID(infoPbtID, infoPbtIDLength);
	const CPersonalBankerInfo &pbInfo(varPbInfo);
	CPersonalBanker varPb;
         
    CreateInfo(varPb);
	varPb.DoneWithTransfer();
	varPb.SetPersonalBankerType(pbType);

	CPersonalBankerTransfer pbTransfer(CUtilities::GetTickCount());
	pbTransfer.SetID(pbtID, pbtIDLength);
	CGames games;
	byte meterNumberToAddTo(mNone);
	bool isDuplicate(false);
	varPb.ProcessPersonalBankerTransferComplete(&pbTransfer,
		games,
		meterNumberToAddTo,
		isDuplicate,
		isEFT);

	varPb.SetState(pbState);
	WORD pbSequence = 1;
	varPb.PreparePersonalBankerTransfer(pbSequence);

	varPb.ClearPersonalBankerAction();

	// Perform operations to be tested.
	varPb.ProcessPersonalBankerInfo(&pbInfo, config, gameOnline, bIsEFT);
	const CPersonalBanker &pb(varPb);

	// Test #1.
	{
		const PersonalBankerAction action = pb.GetAction();

		const bool areEqual = action == actionExpected;
		string failedMsg;
		failedMsg = FormatString("FAILED action=%d expected=%d in %s",
			action,
			actionExpected,
			(__TFUNCTION__));
		ASSERT_TRUE(areEqual) <<  failedMsg;
	}

	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::ProcessPersonalBankerInfo()
/// Code coverage test with CPersonalBanker:m_pbTransfer->m_cardNumber value different than pbInfo.m_cardNumber value.
/// </summary>
TEST_F(PersonalBanker4, ProcessPersonalBankerInfo_CardNumberDifferentTest)
{
	// Test data - inputs.
	const char* pbtID = NULL;
	const int pbtIDLength = 0;
	const CardType cardType = CARD_UNKNOWN;
	const byte transferCardNumber[CARD_ID_LEN] = {0xab, 0x34, 0x56, 0x78, 0x90 };
	const byte infoCardNumber[CARD_ID_LEN] = {0x12, 0x34, 0x56, 0x78, 0x90 };
	const PersonalBankerType pbType(Unspecified);
	const CConfig config(false, "");
	const bool gameOnline(false);
	const bool bIsEFT(false);
	const bool isEFT(false);

	// Test data - expected results.
	const PersonalBankerAction actionExpected = PBActionNone;

	// Setup.
	CPersonalBankerInfo varPbInfo;
	varPbInfo.SetCardNumber(infoCardNumber);
	const CPersonalBankerInfo &pbInfo(varPbInfo);

	CPersonalBanker varPb;
         
    CreateInfo(varPb);
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
		isEFT);

	CCard varTransferCard(CTIME_DEFAULT_VALUE, CTIME_DEFAULT_VALUE);
	varTransferCard.SetCardInfo(cardType, transferCardNumber, sizeof(transferCardNumber));
	const CCard &transferCard(varTransferCard);
	varPb.SetTransferCardNumber(&transferCard, CTIME_DEFAULT_VALUE);

	varPb.SetPersonalBankerType(pbType);
	varPb.ClearPersonalBankerAction();

	// Perform operations to be tested.
	varPb.ProcessPersonalBankerInfo(&pbInfo, config, gameOnline, bIsEFT);
	const CPersonalBanker &pb(varPb);

	// Test #1.
	{
		const PersonalBankerAction action = pb.GetAction();

		const bool areEqual = action == actionExpected;
		string failedMsg;
		failedMsg = FormatString("FAILED action=%d expected=%d in %s",
			action,
			actionExpected,
			(__TFUNCTION__));
		ASSERT_TRUE(areEqual) <<  failedMsg;
	}

	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::ProcessPersonalBankerInfo()
/// Code coverage test with CPersonalBanker:m_pbTransfer->m_cardNumber value the same as pbInfo.m_cardNumber value
/// and !m_autoUpload.IsAutoInProcess() and CPersonalBanker:m_pbInfo == NULL.
/// </summary>
TEST_F(PersonalBanker4, ProcessPersonalBankerInfo_NotAutoInfoNullTest)
{
	// Test data - inputs.
	const char* pbtID = NULL;
	const int pbtIDLength = 0;
	const CardType cardType = CARD_UNKNOWN;
	const byte transferCardNumber[CARD_ID_LEN] = {0x12, 0x34, 0x56, 0x78, 0x90 };
	const byte infoCardNumber[CARD_ID_LEN] = {0x12, 0x34, 0x56, 0x78, 0x90 };
	const PersonalBankerType pbType(Unspecified);
	const CConfig config(false, "");
	const bool gameOnline(false);
	const bool bIsEFT(false);
	const bool isEFT(false);

	// Test data - expected results.
	const PersonalBankerAction actionExpected = PBActionGetEGMBalances;

	// Setup.
	CPersonalBankerInfo varPbInfo;
	varPbInfo.SetCardNumber(infoCardNumber);
	const CPersonalBankerInfo &pbInfo(varPbInfo);

	CPersonalBanker varPb;
         
    CreateInfo(varPb);
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
		isEFT);

	CCard varTransferCard(CTIME_DEFAULT_VALUE, CTIME_DEFAULT_VALUE);
	varTransferCard.SetCardInfo(cardType, transferCardNumber, sizeof(transferCardNumber));
	const CCard &transferCard(varTransferCard);
	varPb.SetTransferCardNumber(&transferCard, CTIME_DEFAULT_VALUE);

	varPb.SetAutoInProcess(false);
	varPb.SetPersonalBankerType(pbType);
	varPb.ClearPersonalBankerAction();

	// Perform operations to be tested.
	varPb.ProcessPersonalBankerInfo(&pbInfo, config, gameOnline, bIsEFT);
	const CPersonalBanker &pb(varPb);

	// Test #1.
	{
		const PersonalBankerAction action = pb.GetAction();

		const bool areEqual = action == actionExpected;
		string failedMsg;
		failedMsg = FormatString("FAILED action=%d expected=%d in %s",
			action,
			actionExpected,
			(__TFUNCTION__));
		ASSERT_TRUE(areEqual) <<  failedMsg;
	}

	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::ProcessPersonalBankerInfo()
/// Code coverage test with CPersonalBanker:m_pbTransfer->m_cardNumber value the same as pbInfo.m_cardNumber value
/// and !m_autoUpload.IsAutoInProcess() and CPersonalBanker:m_pbInfo != NULL.
/// </summary>
TEST_F(PersonalBanker4, ProcessPersonalBankerInfo_NotAutoInfoTest)
{
	// Test data - inputs.
	const char* pbtID = NULL;
	const int pbtIDLength = 0;
	const CardType cardType = CARD_UNKNOWN;
	const byte transferCardNumber[CARD_ID_LEN] = {0x12, 0x34, 0x56, 0x78, 0x90 };
	const byte infoCardNumber[CARD_ID_LEN] = {0x12, 0x34, 0x56, 0x78, 0x90 };
	const PersonalBankerType pbType(Unspecified);
	const CConfig config(false, "");
	const bool gameOnline(false);
	const bool bIsEFT(false);
	const bool isEFT(false);

	// Test data - expected results.
	const PersonalBankerAction actionExpected = PBActionGetEGMBalances;

	// Setup.
	CPersonalBankerInfo varPbInfo;
	varPbInfo.SetCardNumber(infoCardNumber);
	const CPersonalBankerInfo &pbInfo(varPbInfo);

	CPersonalBanker varPb;
         
    CreateInfo(varPb);
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
		isEFT);

	CCard varTransferCard(CTIME_DEFAULT_VALUE, CTIME_DEFAULT_VALUE);
	varTransferCard.SetCardInfo(cardType, transferCardNumber, sizeof(transferCardNumber));
	const CCard &transferCard(varTransferCard);
	varPb.SetTransferCardNumber(&transferCard, CTIME_DEFAULT_VALUE);

	varPb.SetAutoInProcess(false);
	varPb.SetPersonalBankerType(pbType);
	varPb.ClearPersonalBankerAction();

	// Perform operations to be tested.
	// First with CPersonalBanker:m_pbInfo == NULL.
	// Then with CPersonalBanker:m_pbInfo != NULL.
	varPb.ProcessPersonalBankerInfo(&pbInfo, config, gameOnline, bIsEFT);
	varPb.ClearPersonalBankerAction();
	varPb.ProcessPersonalBankerInfo(&pbInfo, config, gameOnline, bIsEFT);
	const CPersonalBanker &pb(varPb);

	// Test #1.
	{
		const PersonalBankerAction action = pb.GetAction();

		const bool areEqual = action == actionExpected;
		string failedMsg;
		failedMsg = FormatString("FAILED action=%d expected=%d in %s",
			action,
			actionExpected,
			(__TFUNCTION__));
		ASSERT_TRUE(areEqual) <<  failedMsg;
	}

	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::ProcessPersonalBankerInfo()
/// Code coverage test with CPersonalBanker:m_pbTransfer->m_cardNumber value the same as pbInfo.m_cardNumber value
/// and m_autoUpload.IsAutoInProcess() and CPersonalBanker:m_pbInfo == NULL and pbInfo.m_ErrorCode != NoError.
/// </summary>
TEST_F(PersonalBanker4, ProcessPersonalBankerInfo_AutoInfoNullErrorTest)
{
	// Test data - inputs.
	const char* pbtID(NULL);
	const int pbtIDLength(0);
	const CardType cardType(CARD_UNKNOWN);
	const byte transferCardNumber[CARD_ID_LEN] = {0x12, 0x34, 0x56, 0x78, 0x90 };
	const byte infoCardNumber[CARD_ID_LEN] = {0x12, 0x34, 0x56, 0x78, 0x90 };
	const PBError errorCode(AccessDenied);
	const PersonalBankerType pbType(Unspecified);
	const CConfig config(false, "");
	const bool gameOnline(false);
	const bool bIsEFT(false);
	const bool isEFT(false);

	// Test data - expected results.
	const PersonalBankerAction actionExpected1 = PBActionSendCardSessionInProgressToUI;
	const PersonalBankerAction actionExpected2 = PBActionNone;

	// Setup.
	CPersonalBankerInfo varPbInfo;
	varPbInfo.SetCardNumber(infoCardNumber);
	varPbInfo.SetErrorCode(errorCode);
	const CPersonalBankerInfo &pbInfo(varPbInfo);

	CPersonalBanker varPb;
         
    CreateInfo(varPb);
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
		isEFT);

	CCard varTransferCard(CTIME_DEFAULT_VALUE, CTIME_DEFAULT_VALUE);
	varTransferCard.SetCardInfo(cardType, transferCardNumber, sizeof(transferCardNumber));
	const CCard &transferCard(varTransferCard);
	varPb.SetTransferCardNumber(&transferCard, CTIME_DEFAULT_VALUE);

	varPb.SetAutoInProcess(true);
	varPb.SetPersonalBankerType(pbType);
	varPb.ClearPersonalBankerAction();

	// Perform operations to be tested.
	varPb.ProcessPersonalBankerInfo(&pbInfo, config, gameOnline, bIsEFT);
	const CPersonalBanker &pb(varPb);

	// Test #1.
	{
		const PersonalBankerAction action = pb.GetAction();

		string failedMsg;
		failedMsg = FormatString("FAILED action=%d expected=%d or %d in %s",
			action,
			actionExpected1,
			actionExpected2,
			(__TFUNCTION__));
		ASSERT_TRUE(pb.GetAction() == actionExpected1 || pb.GetAction() == actionExpected2) <<  failedMsg;
	}

	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::ProcessPersonalBankerInfo()
/// Code coverage test with CPersonalBanker:m_pbTransfer->m_cardNumber value the same as pbInfo.m_cardNumber value
/// and m_autoUpload.IsAutoInProcess() and CPersonalBanker:m_pbInfo != NULL and pbInfo.m_ErrorCode != NoError.
/// </summary>
TEST_F(PersonalBanker4, ProcessPersonalBankerInfo_AutoInfoErrorTest)
{
	// Test data - inputs.
	const char* pbtID(NULL);
	const int pbtIDLength(0);
	const CardType cardType(CARD_UNKNOWN);
	const byte transferCardNumber[CARD_ID_LEN] = {0x12, 0x34, 0x56, 0x78, 0x90 };
	const byte infoCardNumber[CARD_ID_LEN] = {0x12, 0x34, 0x56, 0x78, 0x90 };
	const PBError errorCode(AccessDenied);
	const PersonalBankerType pbType(Unspecified);
	const CConfig config(false, "");
	const bool gameOnline(false);
	const bool bIsEFT(false);
	const bool isEFT(false);

	// Test data - expected results.
	const PersonalBankerAction actionExpected1 = PBActionSendCardSessionInProgressToUI;
	const PersonalBankerAction actionExpected2 = PBActionNone;

	// Setup.
	CPersonalBankerInfo varPbInfo;
	varPbInfo.SetCardNumber(infoCardNumber);
	varPbInfo.SetErrorCode(errorCode);
	const CPersonalBankerInfo &pbInfo(varPbInfo);

	CPersonalBanker varPb;
         
    CreateInfo(varPb);
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
		isEFT);

	CCard varTransferCard(CTIME_DEFAULT_VALUE, CTIME_DEFAULT_VALUE);
	varTransferCard.SetCardInfo(cardType, transferCardNumber, sizeof(transferCardNumber));
	const CCard &transferCard(varTransferCard);
	varPb.SetTransferCardNumber(&transferCard, CTIME_DEFAULT_VALUE);

	varPb.SetAutoInProcess(true);
	varPb.SetPersonalBankerType(pbType);
	varPb.ClearPersonalBankerAction();

	// First call to ProcessPersonalBankerInfo() with CPersonalBanker:m_pbInfo == NULL.
	varPb.ProcessPersonalBankerInfo(&pbInfo, config, gameOnline, bIsEFT);

	// Update setup again.
	varPb.CreateTransfer(CUtilities::GetTickCount());
	pbTransfer.SetID(pbtID, pbtIDLength);
	varPb.ProcessPersonalBankerTransferComplete(&pbTransfer,
		games,
		meterNumberToAddTo,
		isDuplicate,
		isEFT);

	varPb.SetTransferCardNumber(&transferCard, CTIME_DEFAULT_VALUE);

	varPb.SetAutoInProcess(true);
	varPb.SetPersonalBankerType(pbType);
	varPb.ClearPersonalBankerAction();

	// Perform operations to be tested.
	// This time with CPersonalBanker:m_pbInfo != NULL.
	varPb.ProcessPersonalBankerInfo(&pbInfo, config, gameOnline, bIsEFT);
	const CPersonalBanker &pb(varPb);

	// Test #1.
	{
		const PersonalBankerAction action = pb.GetAction();

		string failedMsg;
		failedMsg = FormatString("FAILED action=%d expected=%d or %d in %s",
			action,
			actionExpected1,
			actionExpected2,
			(__TFUNCTION__));
		ASSERT_TRUE(pb.GetAction() == actionExpected1 || pb.GetAction() == actionExpected2) <<  failedMsg;
	}

	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::ProcessPersonalBankerInfo()
/// Code coverage test with CPersonalBanker:m_pbTransfer->m_cardNumber value the same as pbInfo.m_cardNumber value
/// and m_autoUpload.IsAutoInProcess() and CPersonalBanker:m_pbInfo == NULL and pbInfo.m_ErrorCode == NoError
// and CPersonalBanker:m_state == PBStateInquireSent and CPersonalBanker:m_pbInfo.m_PBIDLength = 0.
/// </summary>
TEST_F(PersonalBanker4, ProcessPersonalBankerInfo_AutoInfoNullNoErrorNoInfoIDTest)
{
	// Test data - inputs.
	const char* pbtID(NULL);
	const int pbtIDLength(0);
	const char* infoPbtID(NULL);
	const int infoPbtIDLength = 0;
	const CardType cardType(CARD_UNKNOWN);
	const byte transferCardNumber[CARD_ID_LEN] = {0x12, 0x34, 0x56, 0x78, 0x90 };
	const byte infoCardNumber[CARD_ID_LEN] = {0x12, 0x34, 0x56, 0x78, 0x90 };
	const PBError errorCode(NoError);
	const PersonalBankerType pbType(Unspecified);
	const PersonalBankerState state(PBStateInquireSent);
	const CConfig config(false, "");
	const bool gameOnline(true);
	const bool bIsEFT(false);
	const bool isEFT(false);

	// Test data - expected results.
	const PersonalBankerAction actionExpected = PBActionSendDisplayPBAnimationToUI;

	// Setup.
	CPersonalBankerInfo varPbInfo;
	varPbInfo.SetCardNumber(infoCardNumber);
	varPbInfo.SetPersonalBankerID(infoPbtID, infoPbtIDLength);
	varPbInfo.SetErrorCode(errorCode);
	const CPersonalBankerInfo &pbInfo(varPbInfo);

	CPersonalBanker varPb;
         
    CreateInfo(varPb);
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
		isEFT);

	CCard varTransferCard(CTIME_DEFAULT_VALUE, CTIME_DEFAULT_VALUE);
	varTransferCard.SetCardInfo(cardType, transferCardNumber, sizeof(transferCardNumber));
	const CCard &transferCard(varTransferCard);
	varPb.SetTransferCardNumber(&transferCard, CTIME_DEFAULT_VALUE);

	varPb.SetAutoInProcess(true);
	varPb.SetPersonalBankerType(pbType);
	varPb.SetState(state);
	varPb.ClearPersonalBankerAction();

	// Perform operations to be tested.
	varPb.ProcessPersonalBankerInfo(&pbInfo, config, gameOnline, bIsEFT);
	const CPersonalBanker &pb(varPb);

	// Test #1.
	{
		const PersonalBankerAction action = pb.GetAction();

		const bool areEqual = action == actionExpected;
		string failedMsg;
		failedMsg = FormatString("FAILED action=%d expected=%d in %s",
			action,
			actionExpected,
			(__TFUNCTION__));
		ASSERT_TRUE(areEqual) <<  failedMsg;
	}

	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::ProcessPersonalBankerInfo()
/// Code coverage test with CPersonalBanker:m_pbTransfer->m_cardNumber value the same as pbInfo.m_cardNumber value
/// and m_autoUpload.IsAutoInProcess() and CPersonalBanker:m_pbInfo == NULL and pbInfo.m_ErrorCode == NoError
// and CPersonalBanker:m_state == PBStateInquireSent and CPersonalBanker:m_pbInfo.m_PBIDLength >= 0.
/// </summary>
TEST_F(PersonalBanker4, ProcessPersonalBankerInfo_AutoInfoNullNoErrorInfoIDTestPoller_12_11_6000_or_Higher)
{
	// Test data - inputs.
	const char* pbtID(NULL);
	const int pbtIDLength(0);
	const int infoPbtIDLength = 1;
	const char infoPbtID[infoPbtIDLength] = { '2' };
	const CardType cardType(CARD_UNKNOWN);
	const byte transferCardNumber[CARD_ID_LEN] = {0x12, 0x34, 0x56, 0x78, 0x90 };
	const byte infoCardNumber[CARD_ID_LEN] = {0x12, 0x34, 0x56, 0x78, 0x90 };
	const PBError errorCode(NoError);
	const PersonalBankerType pbType(Unspecified);
	const PersonalBankerState state(PBStateInquireSent);
	CConfig config(false, "");
	config.SetPoller_12_11_6000_or_Higher(true);
	config.SetSenOption2Raw(SO2_WAT_AUTOPLOAD);
	config.SetPBFromGameEnabled(true);
	config.SetPBTCashEnabled_Global(true);
	const bool gameOnline(true);
	const bool bIsEFT(false);
	const bool isEFT(false);

	// Test data - expected results.
	const PersonalBankerAction actionExpected = PBActionCancelAutoUploadDueToCashErrorSessionRelease;

	// Setup.
	CPersonalBankerInfo varPbInfo;
	varPbInfo.SetConfigData(config, true, false);
	varPbInfo.SetCardNumber(infoCardNumber);
	varPbInfo.SetPersonalBankerID(infoPbtID, infoPbtIDLength);
	varPbInfo.SetErrorCode(errorCode);
	u_PBFlags pbFlags;
	pbFlags.flags.CashInactive = true;
	varPbInfo.SetPBFlags(pbFlags.uValue);
	const CPersonalBankerInfo &pbInfo(varPbInfo);

	CPersonalBanker varPb;
	varPb.SetGuestPlayer(false);
         
    CreateInfo(varPb);
	varPb.RemoveSession();
	varPb.CreateTransfer(CUtilities::GetTickCount());

	CPersonalBankerTransfer pbTransfer(CUtilities::GetTickCount());
	pbTransfer.SetID(pbtID, pbtIDLength);
	pbTransfer.SetMaxCashUp(1000);
	CGames games;
	byte meterNumberToAddTo(mNone);
	bool isDuplicate(false);
	varPb.ProcessPersonalBankerTransferComplete(&pbTransfer,
		games,
		meterNumberToAddTo,
		isDuplicate,
		isEFT);

	CCard varTransferCard(CTIME_DEFAULT_VALUE, CTIME_DEFAULT_VALUE);
	varTransferCard.SetCardInfo(cardType, transferCardNumber, sizeof(transferCardNumber));
	const CCard &transferCard(varTransferCard);
	varPb.SetTransferCardNumber(&transferCard, CTIME_DEFAULT_VALUE);

	varPb.SetAutoInProcess(true);
	varPb.SetPersonalBankerType(pbType);
	varPb.SetState(state);
	varPb.ClearPersonalBankerAction();
	varPb.m_autoUpload.SetPendingAutoWatAmount(100);

	// Perform operations to be tested.
	varPb.ProcessPersonalBankerInfo(&pbInfo, config, gameOnline, bIsEFT);
	const CPersonalBanker &pb(varPb);

	// Test #1.
	{
		const PersonalBankerAction action = pb.GetAction();

		const bool areEqual = action == actionExpected;
		string failedMsg;
		failedMsg = FormatString("FAILED action=%d expected=%d in %s",
			action,
			actionExpected,
			(__TFUNCTION__));
		ASSERT_TRUE(areEqual) <<  failedMsg;
	}

	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::ProcessPersonalBankerInfo()
/// Code coverage test with CPersonalBanker:m_pbTransfer->m_cardNumber value the same as pbInfo.m_cardNumber value
/// and m_autoUpload.IsAutoInProcess() and CPersonalBanker:m_pbInfo == NULL and pbInfo.m_ErrorCode == NoError
// and CPersonalBanker:m_state == PBStateInquireSent and CPersonalBanker:m_pbInfo.m_PBIDLength >= 0.
/// </summary>
TEST_F(PersonalBanker4, ProcessPersonalBankerInfo_AutoInfoNullNoErrorInfoGuest)
{
	// Test data - inputs.
	const char* pbtID(NULL);
	const int pbtIDLength(0);
	const int infoPbtIDLength = 1;
	const char infoPbtID[infoPbtIDLength] = { '2' };
	const CardType cardType(CARD_UNKNOWN);
	const byte transferCardNumber[CARD_ID_LEN] = { 0x12, 0x34, 0x56, 0x78, 0x90 };
	const byte infoCardNumber[CARD_ID_LEN] = { 0x12, 0x34, 0x56, 0x78, 0x90 };
	const PBError errorCode(NoError);
	const PersonalBankerType pbType(Unspecified);
	const PersonalBankerState state(PBStateInquireSent);
	CConfig config(false, "");
	config.SetPoller_12_11_6000_or_Higher(true);
	const bool gameOnline(true);
	const bool bIsEFT(false);
	const bool isEFT(false);

	// Test data - expected results.
	const PersonalBankerAction actionExpected = PBActionProcessRequestPlayerResume;

	// Setup.
	CPersonalBankerInfo varPbInfo;
	varPbInfo.SetCardNumber(infoCardNumber);
	varPbInfo.SetPersonalBankerID(infoPbtID, infoPbtIDLength);
	varPbInfo.SetErrorCode(errorCode);
	const CPersonalBankerInfo &pbInfo(varPbInfo);

	CPersonalBanker varPb;
	varPb.SetGuestPlayer(true);

	CreateInfo(varPb);
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
		isEFT);

	CCard varTransferCard(CTIME_DEFAULT_VALUE, CTIME_DEFAULT_VALUE);
	varTransferCard.SetCardInfo(cardType, transferCardNumber, sizeof(transferCardNumber));
	const CCard &transferCard(varTransferCard);
	varPb.SetTransferCardNumber(&transferCard, CTIME_DEFAULT_VALUE);

	varPb.SetAutoInProcess(true);
	varPb.SetPersonalBankerType(pbType);
	varPb.SetState(state);
	varPb.ClearPersonalBankerAction();

	// Perform operations to be tested.
	varPb.ProcessPersonalBankerInfo(&pbInfo, config, gameOnline, bIsEFT);
	const CPersonalBanker &pb(varPb);

	// Test #1.
	{
		const PersonalBankerAction action = pb.GetAction();

		const bool areEqual = action == actionExpected;
		string failedMsg;
		failedMsg = FormatString("FAILED action=%d expected=%d in %s",
			action,
			actionExpected,
			(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Cleanup.
}


/// <summary>
/// Test the implementation of:
///     CPersonalBanker::ProcessPersonalBankerInfo()
/// Code coverage test with CPersonalBanker:m_pbTransfer->m_cardNumber value the same as pbInfo.m_cardNumber value
/// and m_autoUpload.IsAutoInProcess() and CPersonalBanker:m_pbInfo == NULL and pbInfo.m_ErrorCode == NoError
// and CPersonalBanker:m_state == PBStateInquireSent and CPersonalBanker:m_pbInfo.m_PBIDLength >= 0.
/// </summary>
TEST_F(PersonalBanker4, ProcessPersonalBankerInfo_AutoInfoNullNoErrorInfoIDTestLowerThanPoller_12_11_6000)
{
	// Test data - inputs.
	const char* pbtID(NULL);
	const int pbtIDLength(0);
	const int infoPbtIDLength = 1;
	const char infoPbtID[infoPbtIDLength] = { '2' };
	const CardType cardType(CARD_UNKNOWN);
	const byte transferCardNumber[CARD_ID_LEN] = { 0x12, 0x34, 0x56, 0x78, 0x90 };
	const byte infoCardNumber[CARD_ID_LEN] = { 0x12, 0x34, 0x56, 0x78, 0x90 };
	const PBError errorCode(NoError);
	const PersonalBankerType pbType(Unspecified);
	const PersonalBankerState state(PBStateInquireSent);
	CConfig config(false, "");
	config.SetPoller_12_11_6000_or_Higher(false);
	const bool gameOnline(true);
	const bool bIsEFT(false);
	const bool isEFT(false);

	// Test data - expected results.
	const PersonalBankerAction actionExpected = PBActionSendDisplayPBAnimationToUIAndTransferToEGM;

	// Setup.
	CPersonalBankerInfo varPbInfo;
	varPbInfo.SetCardNumber(infoCardNumber);
	varPbInfo.SetPersonalBankerID(infoPbtID, infoPbtIDLength);
	varPbInfo.SetErrorCode(errorCode);
	const CPersonalBankerInfo &pbInfo(varPbInfo);

	CPersonalBanker varPb;

	CreateInfo(varPb);
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
		isEFT);

	CCard varTransferCard(CTIME_DEFAULT_VALUE, CTIME_DEFAULT_VALUE);
	varTransferCard.SetCardInfo(cardType, transferCardNumber, sizeof(transferCardNumber));
	const CCard &transferCard(varTransferCard);
	varPb.SetTransferCardNumber(&transferCard, CTIME_DEFAULT_VALUE);

	varPb.SetAutoInProcess(true);
	varPb.SetPersonalBankerType(pbType);
	varPb.SetState(state);
	varPb.ClearPersonalBankerAction();

	// Perform operations to be tested.
	varPb.ProcessPersonalBankerInfo(&pbInfo, config, gameOnline, bIsEFT);
	const CPersonalBanker &pb(varPb);

	// Test #1.
	{
		const PersonalBankerAction action = pb.GetAction();

		const bool areEqual = action == actionExpected;
		string failedMsg;
		failedMsg = FormatString("FAILED action=%d expected=%d in %s",
			action,
			actionExpected,
			(__TFUNCTION__));
		ASSERT_TRUE(areEqual) << failedMsg;
	}

	// Cleanup.
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::ProcessPersonalBankerInfo()
///     CPersonalBanker::ProcessPersonalBankerInfoNotAuto()
/// Functional test of CPersonalBanker::ProcessPersonalBankerInfoNotAuto().
/// </summary>
TEST_F(PersonalBanker4, ProcessPersonalBankerInfoNotAuto)
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
		const int m_transferPbtIDLengthExpected;
		const char m_transferPbtIDExpected[MAX_PBID_LENGTH];
		const bool m_infoExpected;
		const DWORD m_transferMinCashUpExpected;
		const DWORD m_transferMaxCashUpExpected;
		const DWORD m_transferMinPromoUpExpected;
		const DWORD m_transferMaxPromoUpExpected;
		const PersonalBankerAction m_actionExpected;
		const bool m_autoTransferPendingExpected;
	} TestDataType;

	const TestDataType TEST_DATA[] =
	{
		{ true, true, true, true, false, false, true, true, false, false, false,
			{ 0x12, 0x34, 0x56, 0x78, 0x90 }, { 0x12, 0x34, 0x56, 0x78, 0x90 },
			0, { 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 },
			true, 0, 0, 0, 0,
			true, true, true, true, false, true, false,
			0, 0, 0, 0, PBStateInfoReceived,
			0, { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
			true, 0, 0, 0, 0, PBActionGetEGMBalances, false },

		{ true, true, true, true, false, false, true, true, false, true, false,
			{ 0x12, 0x34, 0x56, 0x78, 0x90 }, { 0x12, 0x34, 0x56, 0x78, 0x90 },
			20, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
			true, 0, 0, 0, 0,
			true, true, true, true, false, true, false,
			0, 0, 0, 0, PBStateInfoReceived,
			20, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
			true, 0, 0, 0, 0, PBActionGetEGMBalances, false },

		{ true, true, true, true, false, false, true, true, true, false, false,
			{ 0x12, 0x34, 0x56, 0x78, 0x90 }, { 0x12, 0x34, 0x56, 0x78, 0x90 },
			3, { 0, 0, 0, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 },
			true, 0, 0, 0, 0,
			true, true, true, true, false, true, true,
			0, 0, 0, 0, PBStateInfoReceived,
			3, { 0, 0, 0, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
			true, 0, 0, 0, 0, PBActionGetEGMBalances, false },

		{ true, true, true, true, false, false, true, true, true, true, false,
			{ 0x12, 0x34, 0x56, 0x78, 0x90 }, { 0x12, 0x34, 0x56, 0x78, 0x90 },
			4, { 'a', 'b', 'c', 'd', 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 },
			true, 1, 2, 3, 4,
			true, true, true, true, false, true, true,
			1, 2, 3, 4, PBStateInfoReceived,
			4, 	{ 'a', 'b', 'c', 'd', 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
			true, 1, 2, 3, 4, PBActionGetEGMBalances, false },

		{ true, true, true, true, false, false, true, true, false, false, true,
			{ 0x12, 0x34, 0x56, 0x78, 0x90 }, { 0x12, 0x34, 0x56, 0x78, 0x90 },
			5, { '!', '@', '#', '$', '%', 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 },
			true, 1, 2, 3, 4,
			true, true, true, true, false, true, false,
			1, 2, 3, 4, PBStateInfoReceived,
			5, 	{ '!', '@', '#', '$', '%', 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
			true, 1, 2, 3, 4, PBActionGetEGMBalances, false },

		{ true, true, true, true, false, false, true, true, false, true, true,
			{ 0x12, 0x34, 0x56, 0x78, 0x90 }, { 0x12, 0x34, 0x56, 0x78, 0x90 },
			20, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 },
			true, 1, 2, 3, 4,
			true, true, true, true, false, true, false,
			1, 2, 3, 4, PBStateInfoReceived,
			20, 	{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 },
			true, 1, 2, 3, 4, PBActionGetEGMBalances, false },

		{ true, true, true, true, false, false, true, true, true, false, true,
			{ 0x12, 0x34, 0x56, 0x78, 0x90 }, { 0x12, 0x34, 0x56, 0x78, 0x90 },
			3, { '1', '2', '3', 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 },
			true, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
			true, true, true, true, false, true, true,
			0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, PBStateInfoReceived,
			3, { '1', '2', '3', 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
			true, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, PBActionGetEGMBalances, false },

		{ true, true, true, true, false, false, true, true, true, true, true,
			{ 0x12, 0x34, 0x56, 0x78, 0x90 }, { 0x12, 0x34, 0x56, 0x78, 0x90 },
			20, { (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff,
					(char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff },
			true, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
			true, true, true, true, false, true, true,
			0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, PBStateInfoReceived,
			20, { (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff,
					(char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff },
			true, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, PBActionGetEGMBalances, false },

		// Call ProcessPersonalBankerInfo() with pbInfo=NULL.
		{ true, true, true, true, false, false, true, true, true, true, false,
			{ 0x12, 0x34, 0x56, 0x78, 0x90 }, { 0x12, 0x34, 0x56, 0x78, 0x90 },
			4, { 'a', 'b', 'c', 'd', 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 },
			false, 1, 2, 3, 4,
			true, true, true, true, false, true, true,
			1, 2, 3, 4, PBStateUnknown,
			0, 	{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
			false, 0, 0, 0, 0, PBActionNone, false },

		// Call ProcessPersonalBankerInfo() with different pbInfo.m_cardNumber.
		{ true, true, true, true, false, false, true, true, true, true, false,
			{ 0x12, 0x34, 0x56, 0x78, 0x90 }, { 0x12, 0x34, 0x56, 0x78, 0x99 },
			6, { 'a', 'b', 'c', 'd', 'e', 'f', 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 },
			true, 1, 2, 3, 4,
			true, true, true, true, false, true, true,
			1, 2, 3, 4, PBStateUnknown,
			0, 	{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
			false, 0, 0, 0, 0, PBActionNone, false },
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
         
        CreateInfo(varPb);
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

		CCard varTransferCard(CTIME_DEFAULT_VALUE, CTIME_DEFAULT_VALUE);
		varTransferCard.SetCardInfo(cardType,
			TEST_DATA[testIndex].m_transferCardNumber,
			sizeof(TEST_DATA[testIndex].m_transferCardNumber));
		const CCard &transferCard(varTransferCard);
		varPb.SetTransferCardNumber(&transferCard, CTIME_DEFAULT_VALUE);

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

		// Test #1.
		const CPersonalBankerInfo *pbInfoResult = pb.GetInfoCopy();

		{
			bool areEqual = ((pbInfoResult != NULL) == TEST_DATA[testIndex].m_infoExpected);

			string failedMsg;
			failedMsg = FormatString("FAILED pbInfoResult=%p expected=%s testIndex=%d in %s",
				(const void *)pbInfoResult,
				(TEST_DATA[testIndex].m_infoExpected ? "NOT NUL" :  _T("NULL")) ,
				testIndex,
				(__TFUNCTION__));
			ASSERT_TRUE(areEqual) <<  failedMsg;
		}

		if (TEST_DATA[testIndex].m_infoExpected)
		{
			// Test #2.
			{
				const DWORD infoMinCashUp = pbInfoResult->GetMinimumCashUp();

				bool areEqual = infoMinCashUp == TEST_DATA[testIndex].m_infoMinCashUpExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED infoMinCashUp=%u expected=%u testIndex=%d in %s",
					infoMinCashUp,
					TEST_DATA[testIndex].m_infoMinCashUpExpected,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) <<  failedMsg;
			}

			// Test #3.
			{
				const DWORD infoMaxCashUp = pbInfoResult->GetMaximumCashUp();

				bool areEqual = infoMaxCashUp == TEST_DATA[testIndex].m_infoMaxCashUpExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED infoMaxCashUp=%u expected=%u testIndex=%d in %s",
					infoMaxCashUp,
					TEST_DATA[testIndex].m_infoMaxCashUpExpected,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) <<  failedMsg;
			}

			// Test #4.
			{
				const DWORD infoMinPromoUp = pbInfoResult->GetMinimumPromoUp();

				bool areEqual = infoMinPromoUp == TEST_DATA[testIndex].m_infoMinPromoUpExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED infoMinPromoUp=%u expected=%u testIndex=%d in %s",
					infoMinPromoUp,
					TEST_DATA[testIndex].m_infoMinPromoUpExpected,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) <<  failedMsg;
			}

			// Test #5.
			{
				const DWORD infoMaxPromoUp = pbInfoResult->GetMaximumPromoUp();

				bool areEqual = infoMaxPromoUp == TEST_DATA[testIndex].m_infoMaxPromoUpExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED infoMaxPromoUp=%u expected=%u testIndex=%d in %s",
					infoMaxPromoUp,
					TEST_DATA[testIndex].m_infoMaxPromoUpExpected,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) <<  failedMsg;
			}

			// Test #6.
			{
				const bool bTransferCashToGameEnabled = pbInfoResult->IsTransferCashToGameEnabled();

				bool areEqual = bTransferCashToGameEnabled == TEST_DATA[testIndex].m_bTransferCashToGameEnabledExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED bTransferCashToGameEnabled=%d expected=%d testIndex=%d in %s",
					bTransferCashToGameEnabled,
					TEST_DATA[testIndex].m_bTransferCashToGameEnabledExpected,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) <<  failedMsg;
			}

			// Test #7.
			{
				const bool bTransferPointsToGameEnabled = pbInfoResult->IsTransferPointsToGameEnabled();

				bool areEqual = bTransferPointsToGameEnabled == TEST_DATA[testIndex].m_bTransferPointsToGameEnabledExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED bTransferPointsToGameEnabled=%d expected=%d testIndex=%d (%d %d %d %d %d %d) in %s",
					bTransferPointsToGameEnabled,
					TEST_DATA[testIndex].m_bTransferPointsToGameEnabledExpected,
					testIndex,
					TEST_DATA[testIndex].m_bTransferPointsToGameEnabled,
					config.PBTPointsEnabledGlobal(),
					config.PBTPointsEnabled(),
					TEST_DATA[testIndex].m_bIsEFT,
					config.PBTPointsAsPromo(),
					pbInfo.IsTransferPointsToGameEnabled(),
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) <<  failedMsg;
			}

			// Test #8.
			{
				const bool bTransferPromoDollarsToGameEnabled = pbInfoResult->IsTransferPromoDollarsToGameEnabled();

				bool areEqual = bTransferPromoDollarsToGameEnabled == TEST_DATA[testIndex].m_bTransferPromoDollarsToGameEnabledExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED bTransferPromoDollarsToGameEnabled=%d expected=%d testIndex=%d in %s",
					bTransferPromoDollarsToGameEnabled,
					TEST_DATA[testIndex].m_bTransferPromoDollarsToGameEnabledExpected,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) <<  failedMsg;
			}

			// Test #9.
			{
				const bool bTransferCashFromGameEnabled = pbInfoResult->IsTransferCashFromGameEnabled();

				bool areEqual = bTransferCashFromGameEnabled == TEST_DATA[testIndex].m_bTransferCashFromGameEnabledExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED bTransferCashFromGameEnabled=%d expected=%d testIndex=%d in %s",
					bTransferCashFromGameEnabled,
					TEST_DATA[testIndex].m_bTransferCashFromGameEnabledExpected,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) <<  failedMsg;
			}

			// Test #10.
			{
				const bool bGameDenomMatches = pbInfoResult->DoesGameDenomMatch();

				bool areEqual = bGameDenomMatches == TEST_DATA[testIndex].m_bGameDenomMatchesExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED bGameDenomMatches=%d expected=%d testIndex=%d in %s",
					bGameDenomMatches,
					TEST_DATA[testIndex].m_bGameDenomMatchesExpected,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) <<  failedMsg;
			}

			// Test #11.
			{
				const bool bTransferPromoFromGameEnabled = pbInfoResult->IsTransferPromoFromGameEnabled();

				bool areEqual = bTransferPromoFromGameEnabled == TEST_DATA[testIndex].m_bTransferPromoFromGameEnabledExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED bTransferPromoFromGameEnabled=%d expected=%d testIndex=%d in %s",
					bTransferPromoFromGameEnabled,
					TEST_DATA[testIndex].m_bTransferPromoFromGameEnabledExpected,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) <<  failedMsg;
			}

			// Test #12.
			{
				const bool bTransferToGameEnabled = pbInfoResult->IsTransferToGameEnabled();

				bool areEqual = bTransferToGameEnabled == TEST_DATA[testIndex].m_bTransferToGameEnabledExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED bTransferToGameEnabled=%d expected=%d testIndex=%d (%d %d %d %d) in %s",
					bTransferToGameEnabled,
					TEST_DATA[testIndex].m_bTransferToGameEnabledExpected,
					testIndex,
					TEST_DATA[testIndex].m_bTransferCashToGameEnabledExpected,
					TEST_DATA[testIndex].m_bTransferPointsToGameEnabledExpected,
					TEST_DATA[testIndex].m_bTransferPromoDollarsToGameEnabledExpected,
					TEST_DATA[testIndex].m_bEGMInitialized,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) <<  failedMsg;
			}
		}

		// Test #13.
		{
			const PersonalBankerState pbStateResult = pb.GetState();

			bool areEqual = pbStateResult == TEST_DATA[testIndex].m_pbStateExpected;
			string failedMsg;
			failedMsg = FormatString("FAILED pbStateResult=%d expected=%d testIndex=%d in %s",
				pbStateResult,
				TEST_DATA[testIndex].m_pbStateExpected,
				testIndex,
				(__TFUNCTION__));
			ASSERT_TRUE(areEqual) <<  failedMsg;
		}


		// Test #14.
		const CPersonalBankerTransfer *pbTransferResult = pb.GetTransferCopy();

		{
			string failedMsg;
			failedMsg = FormatString("FAILED pbTransferResult=%p expected=NOT NULL testIndex=%d in %s",
				(const void *)pbTransferResult,
				testIndex,
				(__TFUNCTION__));
			ASSERT_TRUE(pbTransferResult != NULL) << failedMsg;
		}

		// Test #15.
		{
			const int transferPbtIDLength = pbTransferResult->GetPBTIDLength();

			bool areEqual = transferPbtIDLength == TEST_DATA[testIndex].m_transferPbtIDLengthExpected;
			string failedMsg;
			failedMsg = FormatString("FAILED transferPbtIDLength=%d expected=%d testIndex=%d in %s",
				transferPbtIDLength,
				TEST_DATA[testIndex].m_transferPbtIDLengthExpected,
				testIndex,
				(__TFUNCTION__));
			ASSERT_TRUE(areEqual) <<  failedMsg;
		}

		// Test #16.
		const char *transferPbtID = pbTransferResult->GetID();

		{
			string failedMsg;
			failedMsg = FormatString("FAILED transferPbtID=%p expected=NOT NULL testIndex=%d in %s",
				(const void *)transferPbtID,
				testIndex,
				(__TFUNCTION__));
			ASSERT_TRUE(transferPbtID != NULL) << failedMsg;
		}

		// Test #17.

		{
			size_t memcmpSize = (size_t)TEST_DATA[testIndex].m_transferPbtIDLengthExpected;
			bool areEqual = 0 == memcmp(transferPbtID,
				TEST_DATA[testIndex].m_transferPbtIDExpected,
				memcmpSize);

			string resultString = CUtilities::ToHexString(reinterpret_cast<const BYTE *>(transferPbtID),
				(DWORD)memcmpSize);
			string expectedString = 
				CUtilities::ToHexString(reinterpret_cast<const BYTE *>(TEST_DATA[testIndex].m_transferPbtIDExpected),
					(DWORD)memcmpSize);
			string failedMsg;
			failedMsg = FormatString("FAILED transferPbtID={%s} expected={%s} testIndex=%d in %s",
				(resultString).c_str(),
				(expectedString).c_str(),
				testIndex,
				static_cast<LPCTSTR>(__TFUNCTION__));
			ASSERT_TRUE(areEqual) <<  failedMsg;
		}

		// Test #18.
		{
			const DWORD transferMinCashUp = pbTransferResult->GetMinCashUp();

			bool areEqual = transferMinCashUp == TEST_DATA[testIndex].m_transferMinCashUpExpected;
			string failedMsg;
			failedMsg = FormatString("FAILED transferMinCashUp=%u expected=%u testIndex=%d in %s",
				transferMinCashUp,
				TEST_DATA[testIndex].m_transferMinCashUpExpected,
				testIndex,
				(__TFUNCTION__));
			ASSERT_TRUE(areEqual) <<  failedMsg;
		}

		// Test #19.
		{
			const DWORD transferMaxCashUp = pbTransferResult->GetMaxCashUp();

			bool areEqual = transferMaxCashUp == TEST_DATA[testIndex].m_transferMaxCashUpExpected;
			string failedMsg;
			failedMsg = FormatString("FAILED transferMaxCashUp=%u expected=%u testIndex=%d in %s",
				transferMaxCashUp,
				TEST_DATA[testIndex].m_transferMaxCashUpExpected,
				testIndex,
				(__TFUNCTION__));
			ASSERT_TRUE(areEqual) <<  failedMsg;
		}

		// Test #20.
		{
			const DWORD transferMinPromoUp = pbTransferResult->GetMinPromoUp();

			bool areEqual = transferMinPromoUp == TEST_DATA[testIndex].m_transferMinPromoUpExpected;
			string failedMsg;
			failedMsg = FormatString("FAILED transferMinPromoUp=%u expected=%u testIndex=%d in %s",
				transferMinPromoUp,
				TEST_DATA[testIndex].m_transferMinPromoUpExpected,
				testIndex,
				(__TFUNCTION__));
			ASSERT_TRUE(areEqual) <<  failedMsg;
		}

		// Test #21.
		{
			const DWORD transferMaxPromoUp = pbTransferResult->GetMaxPromoUp();

			bool areEqual = transferMaxPromoUp == TEST_DATA[testIndex].m_transferMaxPromoUpExpected;
			string failedMsg;
			failedMsg = FormatString("FAILED transferMaxPromoUp=%u expected=%u testIndex=%d in %s",
				transferMaxPromoUp,
				TEST_DATA[testIndex].m_transferMaxPromoUpExpected,
				testIndex,
				(__TFUNCTION__));
			ASSERT_TRUE(areEqual) <<  failedMsg;
		}

		// Test #22.
		{
			const PersonalBankerAction action = pb.GetAction();

			const bool areEqual = action == TEST_DATA[testIndex].m_actionExpected;
			string failedMsg;
			failedMsg = FormatString("FAILED action=%d expected=%d in testIndex=%d in %s",
				action,
				TEST_DATA[testIndex].m_actionExpected,
				testIndex,
				(__TFUNCTION__));
			ASSERT_TRUE(areEqual) <<  failedMsg;
		}

		// Test #23.
			const bool autoTransferPending = varPb.IsAutoTransferPending();
		{

			const bool areEqual = autoTransferPending == TEST_DATA[testIndex].m_autoTransferPendingExpected;
			string failedMsg;
			failedMsg = FormatString("FAILED autoTransferPending=%d expected=%d in testIndex=%d in %s(%d)",
				autoTransferPending,
				TEST_DATA[testIndex].m_autoTransferPendingExpected,
				testIndex,
				(__TFUNCTION__), __LINE__);
			ASSERT_TRUE(areEqual) <<  failedMsg;
		}

		// Cleanup.
		delete pbInfoResult;
	}
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::ProcessPersonalBankerInfo()
///     CPersonalBanker::ProcessPersonalBankerInfoAuto()
/// Functional test of CPersonalBanker::ProcessPersonalBankerInfoAuto().
/// </summary>
TEST_F(PersonalBanker4, ProcessPersonalBankerInfoAutoPoller_12_11_6000_or_Higher)
{
	// Test data - inputs.
	const char* pbtID = NULL;
	const int pbtIDLength = 0;
	const CardType cardType = CARD_UNKNOWN;
	const PersonalBankerType pbType(Unspecified);
	const PersonalBankerState pbStateTransferComplete(PBStateUnknown);

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
		const PersonalBankerState m_pbState;

		const byte m_transferCardNumber[CARD_ID_LEN];
		const byte m_infoCardNumber[CARD_ID_LEN];

		const int m_infoPbtIDLength;
		const char m_infoPbtID[MAX_PBID_LENGTH];
		const bool m_useInfo;
		const PBError m_errorCode;
		const DWORD m_minCashUp;
		const DWORD m_maxCashUp;
		const DWORD m_minPromoUp;
		const DWORD m_maxPromoUp;

		const PersonalBankerTransferType m_pbTransferType;
		const PersonalBankerType m_pbType;
		const PersonalBankerTransferType m_pbPromoTransferType; // 2013-06-24 Not directly settable.
		const PersonalBankerType m_pbPromoType; // 2013-06-24 Not directly settable.

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
		const int m_transferPbtIDLengthExpected;
		const char m_transferPbtIDExpected[MAX_PBID_LENGTH];
		const bool m_infoExpected;
		const bool m_transferExpected;
		const DWORD m_transferMinCashUpExpected;
		const DWORD m_transferMaxCashUpExpected;
		const DWORD m_transferMinPromoUpExpected;
		const DWORD m_transferMaxPromoUpExpected;
		const PersonalBankerAction m_actionExpected;
		const bool m_autoTransferPendingExpected;

		const PersonalBankerTransferType m_pbTransferTypeExpected;
		const PersonalBankerType m_pbTypeExpected;
		const PersonalBankerTransferType m_pbPromoTransferTypeExpected;
		const PersonalBankerType m_pbPromoTypeExpected;
	} TestDataType;

	const TestDataType TEST_DATA[] =
	{
		// Index #0. Call ProcessPersonalBankerInfo() with pbInfo.HasError() true.
		{ true, true, true, true, false, false, true, true, true, true, false, PBStateInquireSent,
			{ 0x12, 0x34, 0x56, 0x78, 0x90 }, { 0x12, 0x34, 0x56, 0x78, 0x90 },
			7, { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 },
			true, AccessDenied, 1, 2, 3, 4,
			CashToGame, SessionRequest, PointsToGame, CashDownload,
			true, true, true, true, false, true, true,
			1, 2, 3, 4, PBStateInquireSent,
			0, 	{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
			true, false, 0, 0, 0, 0, PBActionSendCardSessionInProgressToUI, true,
			Undeclared, Unspecified, PromoDollarsFromGame, PromoCreditsUploadAuto },

		// Index #1. Call ProcessPersonalBankerInfo() with pbInfo.HasError() true.
		{ true, true, true, true, false, false, true, true, true, true, false, PBStateInquireSent,
			{ 0x12, 0x34, 0x56, 0x78, 0x90 }, { 0x12, 0x34, 0x56, 0x78, 0x90 },
			7, { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 },
			true, InvalidID, 1, 2, 3, 4,
			PromoDollarsToGame, PointRedemptionToCash, CashFromGame, CashUpload,
			true, true, true, true, false, true, true,
			1, 2, 3, 4, PBStateInquireSent,
			0, 	{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
			true, false, 0, 0, 0, 0, PBActionSendCardSessionInProgressToUI, true,
			Undeclared, Unspecified, PromoDollarsFromGame, PromoCreditsUploadAuto },

		// Index #2. Call ProcessPersonalBankerInfo() with m_state == PBStateUnknown.
		{ true, true, true, true, false, false, true, true, true, true, false, PBStateUnknown,
			{ 0x12, 0x34, 0x56, 0x78, 0x90 }, { 0x12, 0x34, 0x56, 0x78, 0x90 },
			8, { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 },
			true, NoError, 1, 2, 3, 4,
			CashFromGame, ChangePinAtSentinel, PointsFromGame, PointRedemptionToPromoCredits,
			true, true, true, true, false, true, true,
			1, 2, 3, 4, PBStateUnknown,
			0, 	{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
			true, true, 0, 0, 0, 0, PBActionSendDisplayPBAnimationToUI, true,
			Undeclared, Unspecified, Undeclared, Unspecified },

		// Index #3. Call ProcessPersonalBankerInfo() with m_state == PBStateInfoReceived.
		{ true, true, true, true, false, false, true, true, true, false, false, PBStateInfoReceived,
			{ 0x12, 0x34, 0x56, 0x78, 0x90 }, { 0x12, 0x34, 0x56, 0x78, 0x90 },
			0, { 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 },
			true, NoError, 0, 0, 0, 0,
			PointsFromGame, TransferCreditsFromGame, AllFromGame, SessionRelease,
			true, true, true, true, false, true, true,
			0, 0, 0, 0, PBStateInfoReceived,
			0, { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
			true, true, 0, 0, 0, 0, PBActionSendDisplayPBAnimationToUI, true,
			Undeclared, Unspecified, Undeclared, Unspecified },

		// Index #4. Call ProcessPersonalBankerInfo() with m_state == PBStateInfoReceived and pbInfo.m_PBIDLength == 0.
		{ true, true, true, true, false, false, true, true, true, true, false, PBStateInquireSent,
			{ 0x12, 0x34, 0x56, 0x78, 0x90 }, { 0x12, 0x34, 0x56, 0x78, 0x90 },
			0, { 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 },
			true, NoError, 0, 0, 0, 0,
			PromoDollarsFromGame, CanceledSessionRequest, Undeclared, CanceledCashDownload,
			true, true, true, true, false, true, true,
			0, 0, 0, 0, PBStateInquireSent,
			0, { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
			true, true, 0, 0, 0, 0, PBActionSendDisplayPBAnimationToUI, true,
			Undeclared, Unspecified, Undeclared, Unspecified },

		// Index #5. Call ProcessPersonalBankerInfo() with m_state == PBStateInfoReceived and pbInfo.m_PBIDLength == 0.
		{ true, true, true, true, false, false, true, true, true, true, false, PBStateInquireSent,
			{ 0x12, 0x34, 0x56, 0x78, 0x90 }, { 0x12, 0x34, 0x56, 0x78, 0x90 },
			20, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
			true, NoError, 0, 0, 0, 0,
			AllFromGame, CanceledPromoCreditsToGame, CashToGame, CanceledPromoCreditsToCash,
			true, true, true, true, false, true, true,
			0, 0, 0, 0, PBStateTransferingFromToGame,
			20, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
			true, true, 0, 0, 0, 0, PBActionCancelAutoUploadDueToCashErrorSessionRelease, false,
			Undeclared, Unspecified, Undeclared, Unspecified },

		// Index #6.
		{ true, true, true, true, false, false, true, true, true, false, false, PBStateInquireSent,
			{ 0x12, 0x34, 0x56, 0x78, 0x90 }, { 0x12, 0x34, 0x56, 0x78, 0x90 },
			3, { 0, 0, 0, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 },
			true, NoError, 0, 10, 0, 0,
			CashToGame, CanceledPointRedemption, PointsToGame, CanceledCashUpload,
			true, true, true, true, false, true, true,
			0, 10, 0, 0, PBStateTransferingFromToGame,
			3, { 0, 0, 0, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
			true, true, 0, 10, 0, 0, PBActionSendDisplayPBAnimationToUIAndTransferToEGM, true,
			CashFromGame, CashUploadAuto, Undeclared, Unspecified },

		// Index #7.
		{ true, true, true, true, false, false, true, true, true, true, false, PBStateInquireSent,
			{ 0x12, 0x34, 0x56, 0x78, 0x90 }, { 0x12, 0x34, 0x56, 0x78, 0x90 },
			4, { 'a', 'b', 'c', 'd', 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 },
			true, NoError, 1, 20, 3, 4,
			PointsToGame, CanceledTransferCreditsFromGame, PromoDollarsToGame, CanceledSessionRelease,
			true, true, true, true, false, true, true,
			1, 20, 3, 4, PBStateTransferingFromToGame,
			4, 	{ 'a', 'b', 'c', 'd', 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
			true, true, 1, 20, 3, 4, PBActionSendDisplayPBAnimationToUIAndTransferToEGM, true,
			CashFromGame, CashUploadAuto, Undeclared, Unspecified },

		// Index #8.
		{ true, true, true, true, false, false, true, true, true, false, true, PBStateInquireSent,
			{ 0x12, 0x34, 0x56, 0x78, 0x90 }, { 0x12, 0x34, 0x56, 0x78, 0x90 },
			5, { '!', '@', '#', '$', '%', 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 },
			true, NoError, 1, 20, 3, 4,
			PromoDollarsToGame, CanceledChangePinAtSentinel, CashFromGame, CanceledPointRedemptionToPromoCredits,
			true, true, true, true, false, true, true,
			1, 20, 3, 4, PBStateTransferingFromToGame,
			5, 	{ '!', '@', '#', '$', '%', 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
			true, true, 1, 20, 3, 4, PBActionSendDisplayPBAnimationToUIAndTransferToEGM, true,
			CashFromGame, CashUploadAuto, Undeclared, Unspecified },

		// Index #9.
		{ true, true, true, true, false, false, true, true, true, true, true, PBStateInquireSent,
			{ 0x12, 0x34, 0x56, 0x78, 0x90 }, { 0x12, 0x34, 0x56, 0x78, 0x90 },
			20, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 },
			true, NoError, 1, 100, 3, 4,
			CashFromGame, CanceledPointCashUpload, PointsFromGame, CanceledPromoCreditsUpload,
			true, true, true, true, false, true, true,
			1, 100, 3, 4, PBStateTransferingFromToGame,
			20, 	{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 },
			true, true, 1, 100, 3, 4, PBActionSendDisplayPBAnimationToUIAndTransferToEGM, true,
			CashFromGame, CashUploadAuto, Undeclared, Unspecified },

		// Index #10.
		{ true, true, true, true, false, false, true, true, true, false, true, PBStateInquireSent,
			{ 0x12, 0x34, 0x56, 0x78, 0x90 }, { 0x12, 0x34, 0x56, 0x78, 0x90 },
			3, { '1', '2', '3', 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 },
			true, NoError, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
			PointsFromGame, CanceledPromoCreditsUploadAuto, AllFromGame, AuthorizeCashToGame,
			true, true, true, true, false, true, true,
			0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, PBStateTransferingFromToGame,
			3, { '1', '2', '3', 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
			true, true, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, PBActionCancelAutoUploadDueToCashErrorSessionRelease, false,
			Undeclared, Unspecified, Undeclared, Unspecified },

		// Index #11.
		{ true, true, true, true, false, false, true, true, true, true, true, PBStateInquireSent,
			{ 0x12, 0x34, 0x56, 0x78, 0x90 }, { 0x12, 0x34, 0x56, 0x78, 0x90 },
			20, { (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff,
					(char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff },
			true, NoError, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
			PromoDollarsFromGame, AuthorizePromoToGame, Undeclared, AuthorizePointsToGameCash,
			true, true, true, true, false, true, true,
			0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, PBStateTransferingFromToGame,
			20, { (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff,
					(char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff },
			true, true, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, PBActionCancelAutoUploadDueToCashErrorSessionRelease, false,
			Undeclared, Unspecified, Undeclared, Unspecified },

		// Index #12. Call ProcessPersonalBankerInfo() with pbInfo=NULL.
		{ true, true, true, true, false, false, true, true, true, true, false, PBStateInquireSent,
			{ 0x12, 0x34, 0x56, 0x78, 0x90 }, { 0x12, 0x34, 0x56, 0x78, 0x90 },
			4, { 'a', 'b', 'c', 'd', 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 },
			false, NoError, 1, 2, 3, 4,
			AllFromGame, AuthorizePointsToGamePromo, CashToGame, UnknownCreditSource,
			true, true, true, true, false, true, true,
			1, 2, 3, 4, PBStateInquireSent,
			0, 	{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
			false, true, 0, 0, 0, 0, PBActionNone, true,
				Undeclared, Unspecified, Undeclared, Unspecified },

		// Index #13. Call ProcessPersonalBankerInfo() with different pbInfo.m_cardNumber.
		{ true, true, true, true, false, false, true, true, true, true, false, PBStateInquireSent,
			{ 0x12, 0x34, 0x56, 0x78, 0x90 }, { 0x12, 0x34, 0x56, 0x78, 0x99 },
			6, { 'a', 'b', 'c', 'd', 'e', 'f', 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 },
			true, NoError, 1, 2, 3, 4,
			CashToGame, InvalidTransIDFromSystem, PointsToGame, NonMatchingTransactionID,
			true, true, true, true, false, true, true,
			1, 2, 3, 4, PBStateInquireSent,
			0, 	{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
			false, true, 0, 0, 0, 0, PBActionNone, true,
			Undeclared, Unspecified, Undeclared, Unspecified },
	};


	for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
	{
		CConfig varConfig(false, "");
		varConfig.SetPoller_12_11_6000_or_Higher(true);
		varConfig.SetAutoUploadCreditsGlobal(true);
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
		if (varConfig.PBFromGameEnabled())
		{
			varConfig.SetSenOption2Raw(varConfig.GetSenOption2Raw() + SO2_WAT_AUTOPLOAD);					
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
		varPbInfo.SetErrorCode(TEST_DATA[testIndex].m_errorCode);
		const CPersonalBankerInfo &pbInfo(TEST_DATA[testIndex].m_useInfo ? varPbInfo : *(CPersonalBankerInfo *)NULL);

		CPersonalBanker varPb;
		varPb.SetGuestPlayer(false);
		varPb.m_autoUpload.SetPendingAutoNcepAmount(5);
         
        CreateInfo(varPb);
		varPb.RemoveSession();
		varPb.CreateTransfer(CUtilities::GetTickCount());

		// SetState() to affect BeginPersonalBankerTransfer().
		varPb.SetState(PBStateInfoReceived);
		CGames games;
		const DWORD amountInDollars = 5;
		byte pbEFTNumber = 1;
		varPb.BeginPersonalBankerTransfer(games,
			TEST_DATA[testIndex].m_pbTransferType,
			amountInDollars,
			pbEFTNumber);

		// SetState() to affect ProcessPersonalBankerTransferComplete().
		varPb.SetState(pbStateTransferComplete);

		CPersonalBankerTransfer pbTransfer(CUtilities::GetTickCount());
		pbTransfer.SetID(pbtID, pbtIDLength);
		byte meterNumberToAddTo(mNone);
		bool isDuplicate(false);
		varPb.ProcessPersonalBankerTransferComplete(&pbTransfer,
			games,
			meterNumberToAddTo,
			isDuplicate,
			TEST_DATA[testIndex].m_isEFT);

		CCard varTransferCard(CTIME_DEFAULT_VALUE, CTIME_DEFAULT_VALUE);
		varTransferCard.SetCardInfo(cardType,
			TEST_DATA[testIndex].m_transferCardNumber,
			sizeof(TEST_DATA[testIndex].m_transferCardNumber));
		const CCard &transferCard(varTransferCard);
		varPb.SetTransferCardNumber(&transferCard, CTIME_DEFAULT_VALUE);

		varPb.RebuildCard();
		varPb.SetAutoInProcess(true);
		varPb.SetPersonalBankerType(pbType);
		varPb.SetState(TEST_DATA[testIndex].m_pbState);
		varPb.ClearPersonalBankerAction();
		varPb.m_autoUpload.SetPendingAutoWatAmount(500);

		// Perform operations to be tested.
		varPb.ProcessPersonalBankerInfo(&pbInfo,
			config,
			TEST_DATA[testIndex].m_gameOnline,
			TEST_DATA[testIndex].m_bIsEFT);
		const CPersonalBanker &pb(varPb);

		// Test #1.
		const CPersonalBankerInfo *pbInfoResult = pb.GetInfoCopy();

		{
			bool areEqual = ((pbInfoResult != NULL) == TEST_DATA[testIndex].m_infoExpected);

			string failedMsg;
			failedMsg = FormatString("FAILED pbInfoResult=%p expected=%s testIndex=%d in %s",
				(const void *)pbInfoResult,
				(TEST_DATA[testIndex].m_infoExpected ? "NOT NUL" :  _T("NULL")) ,
				testIndex,
				(__TFUNCTION__));
			ASSERT_TRUE(areEqual) <<  failedMsg;
		}

		if (TEST_DATA[testIndex].m_infoExpected)
		{
			// Test #2.
			{
				const DWORD infoMinCashUp = pbInfoResult->GetMinimumCashUp();

				bool areEqual = infoMinCashUp == TEST_DATA[testIndex].m_infoMinCashUpExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED infoMinCashUp=%u expected=%u testIndex=%d in %s",
					infoMinCashUp,
					TEST_DATA[testIndex].m_infoMinCashUpExpected,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) <<  failedMsg;
			}

			// Test #3.
			{
				const DWORD infoMaxCashUp = pbInfoResult->GetMaximumCashUp();

				bool areEqual = infoMaxCashUp == TEST_DATA[testIndex].m_infoMaxCashUpExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED infoMaxCashUp=%u expected=%u testIndex=%d in %s",
					infoMaxCashUp,
					TEST_DATA[testIndex].m_infoMaxCashUpExpected,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) <<  failedMsg;
			}

			// Test #4.
			{
				const DWORD infoMinPromoUp = pbInfoResult->GetMinimumPromoUp();

				bool areEqual = infoMinPromoUp == TEST_DATA[testIndex].m_infoMinPromoUpExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED infoMinPromoUp=%u expected=%u testIndex=%d in %s",
					infoMinPromoUp,
					TEST_DATA[testIndex].m_infoMinPromoUpExpected,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) <<  failedMsg;
			}

			// Test #5.
			{
				const DWORD infoMaxPromoUp = pbInfoResult->GetMaximumPromoUp();

				bool areEqual = infoMaxPromoUp == TEST_DATA[testIndex].m_infoMaxPromoUpExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED infoMaxPromoUp=%u expected=%u testIndex=%d in %s",
					infoMaxPromoUp,
					TEST_DATA[testIndex].m_infoMaxPromoUpExpected,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) <<  failedMsg;
			}

			// Test #6.
			{
				const bool bTransferCashToGameEnabled = pbInfoResult->IsTransferCashToGameEnabled();

				bool areEqual = bTransferCashToGameEnabled == TEST_DATA[testIndex].m_bTransferCashToGameEnabledExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED bTransferCashToGameEnabled=%d expected=%d testIndex=%d in %s",
					bTransferCashToGameEnabled,
					TEST_DATA[testIndex].m_bTransferCashToGameEnabledExpected,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) <<  failedMsg;
			}

			// Test #7.
			{
				const bool bTransferPointsToGameEnabled = pbInfoResult->IsTransferPointsToGameEnabled();

				bool areEqual = bTransferPointsToGameEnabled == TEST_DATA[testIndex].m_bTransferPointsToGameEnabledExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED bTransferPointsToGameEnabled=%d expected=%d testIndex=%d (%d %d %d %d %d %d) in %s",
					bTransferPointsToGameEnabled,
					TEST_DATA[testIndex].m_bTransferPointsToGameEnabledExpected,
					testIndex,
					TEST_DATA[testIndex].m_bTransferPointsToGameEnabled,
					config.PBTPointsEnabledGlobal(),
					config.PBTPointsEnabled(),
					TEST_DATA[testIndex].m_bIsEFT,
					config.PBTPointsAsPromo(),
					pbInfo.IsTransferPointsToGameEnabled(),
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) <<  failedMsg;
			}

			// Test #8.
			{
				const bool bTransferPromoDollarsToGameEnabled = pbInfoResult->IsTransferPromoDollarsToGameEnabled();

				bool areEqual = bTransferPromoDollarsToGameEnabled == TEST_DATA[testIndex].m_bTransferPromoDollarsToGameEnabledExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED bTransferPromoDollarsToGameEnabled=%d expected=%d testIndex=%d in %s",
					bTransferPromoDollarsToGameEnabled,
					TEST_DATA[testIndex].m_bTransferPromoDollarsToGameEnabledExpected,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) <<  failedMsg;
			}

			// Test #9.
			{
				const bool bTransferCashFromGameEnabled = pbInfoResult->IsTransferCashFromGameEnabled();

				bool areEqual = bTransferCashFromGameEnabled == TEST_DATA[testIndex].m_bTransferCashFromGameEnabledExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED bTransferCashFromGameEnabled=%d expected=%d testIndex=%d in %s",
					bTransferCashFromGameEnabled,
					TEST_DATA[testIndex].m_bTransferCashFromGameEnabledExpected,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) <<  failedMsg;
			}

			// Test #10.
			{
				const bool bGameDenomMatches = pbInfoResult->DoesGameDenomMatch();

				bool areEqual = bGameDenomMatches == TEST_DATA[testIndex].m_bGameDenomMatchesExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED bGameDenomMatches=%d expected=%d testIndex=%d in %s",
					bGameDenomMatches,
					TEST_DATA[testIndex].m_bGameDenomMatchesExpected,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) <<  failedMsg;
			}

			// Test #11.
			{
				const bool bTransferPromoFromGameEnabled = pbInfoResult->IsTransferPromoFromGameEnabled();

				bool areEqual = bTransferPromoFromGameEnabled == TEST_DATA[testIndex].m_bTransferPromoFromGameEnabledExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED bTransferPromoFromGameEnabled=%d expected=%d testIndex=%d in %s",
					bTransferPromoFromGameEnabled,
					TEST_DATA[testIndex].m_bTransferPromoFromGameEnabledExpected,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) <<  failedMsg;
			}

			// Test #12.
			{
				const bool bTransferToGameEnabled = pbInfoResult->IsTransferToGameEnabled();

				bool areEqual = bTransferToGameEnabled == TEST_DATA[testIndex].m_bTransferToGameEnabledExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED bTransferToGameEnabled=%d expected=%d testIndex=%d (%d %d %d %d) in %s",
					bTransferToGameEnabled,
					TEST_DATA[testIndex].m_bTransferToGameEnabledExpected,
					testIndex,
					TEST_DATA[testIndex].m_bTransferCashToGameEnabledExpected,
					TEST_DATA[testIndex].m_bTransferPointsToGameEnabledExpected,
					TEST_DATA[testIndex].m_bTransferPromoDollarsToGameEnabledExpected,
					TEST_DATA[testIndex].m_bEGMInitialized,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) <<  failedMsg;
			}
		}

		// Test #13.
		{
			const PersonalBankerState pbStateResult = pb.GetState();

			bool areEqual = pbStateResult == TEST_DATA[testIndex].m_pbStateExpected;
			string failedMsg;
			failedMsg = FormatString("FAILED pbStateResult=%d expected=%d testIndex=%d in %s",
				pbStateResult,
				TEST_DATA[testIndex].m_pbStateExpected,
				testIndex,
				(__TFUNCTION__));
			ASSERT_TRUE(areEqual) <<  failedMsg;
		}


		// Test #14.
		const CPersonalBankerTransfer *pbTransferResult = pb.GetTransferCopy();

		{
			bool areEqual = ((pbTransferResult != NULL) == TEST_DATA[testIndex].m_transferExpected);

			string failedMsg;
			failedMsg = FormatString("FAILED pbTransferResult=%p expected=%s testIndex=%d in %s(%d)",
				(const void *)pbTransferResult,
				(TEST_DATA[testIndex].m_transferExpected ? "NOT NUL" :  _T("NULL")) ,
				testIndex,
				(__TFUNCTION__), __LINE__);
			ASSERT_TRUE(areEqual) <<  failedMsg;
		}

		if (TEST_DATA[testIndex].m_transferExpected)
		{
			// Test #15.
			{
				const int transferPbtIDLength = pbTransferResult->GetPBTIDLength();

				bool areEqual = transferPbtIDLength == TEST_DATA[testIndex].m_transferPbtIDLengthExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED transferPbtIDLength=%d expected=%d testIndex=%d in %s",
					transferPbtIDLength,
					TEST_DATA[testIndex].m_transferPbtIDLengthExpected,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) <<  failedMsg;
			}

			// Test #16.
			const char *transferPbtID = pbTransferResult->GetID();

			{
				string failedMsg;
				failedMsg = FormatString("FAILED transferPbtID=%p expected=NOT NULL testIndex=%d in %s",
					(const void *)transferPbtID,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(transferPbtID != NULL) <<  failedMsg;
			}

			// Test #17.

			{
				size_t memcmpSize = (size_t)TEST_DATA[testIndex].m_transferPbtIDLengthExpected;
				bool areEqual = 0 == memcmp(transferPbtID,
					TEST_DATA[testIndex].m_transferPbtIDExpected,
					memcmpSize);

				string resultString = CUtilities::ToHexString(reinterpret_cast<const BYTE *>(transferPbtID),
					(DWORD)memcmpSize);
				string expectedString = 
					CUtilities::ToHexString(reinterpret_cast<const BYTE *>(TEST_DATA[testIndex].m_transferPbtIDExpected),
						(DWORD)memcmpSize);
				string failedMsg;
				failedMsg = FormatString("FAILED transferPbtID={%s} expected={%s} testIndex=%d in %s",
					(resultString).c_str(),
					(expectedString).c_str(),
					testIndex,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) <<  failedMsg;
			}

			// Test #18.
			{
				const DWORD transferMinCashUp = pbTransferResult->GetMinCashUp();

				bool areEqual = transferMinCashUp == TEST_DATA[testIndex].m_transferMinCashUpExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED transferMinCashUp=%u expected=%u testIndex=%d in %s",
					transferMinCashUp,
					TEST_DATA[testIndex].m_transferMinCashUpExpected,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) <<  failedMsg;
			}

			// Test #19.
			{
				const DWORD transferMaxCashUp = pbTransferResult->GetMaxCashUp();

				bool areEqual = transferMaxCashUp == TEST_DATA[testIndex].m_transferMaxCashUpExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED transferMaxCashUp=%u expected=%u testIndex=%d in %s",
					transferMaxCashUp,
					TEST_DATA[testIndex].m_transferMaxCashUpExpected,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) <<  failedMsg;
			}

			// Test #20.
			{
				const DWORD transferMinPromoUp = pbTransferResult->GetMinPromoUp();

				bool areEqual = transferMinPromoUp == TEST_DATA[testIndex].m_transferMinPromoUpExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED transferMinPromoUp=%u expected=%u testIndex=%d in %s",
					transferMinPromoUp,
					TEST_DATA[testIndex].m_transferMinPromoUpExpected,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) <<  failedMsg;
			}

			// Test #21.
			{
				const DWORD transferMaxPromoUp = pbTransferResult->GetMaxPromoUp();

				bool areEqual = transferMaxPromoUp == TEST_DATA[testIndex].m_transferMaxPromoUpExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED transferMaxPromoUp=%u expected=%u testIndex=%d in %s",
					transferMaxPromoUp,
					TEST_DATA[testIndex].m_transferMaxPromoUpExpected,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) <<  failedMsg;
			}

			// Test #22.
			{
				const PersonalBankerTransferType pbTransferType = pbTransferResult->GetPersonalBankerTransferType();

				bool areEqual = pbTransferType == TEST_DATA[testIndex].m_pbTransferTypeExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED pbTransferType=%u expected=%u testIndex=%d in %s",
					pbTransferType,
					TEST_DATA[testIndex].m_pbTransferTypeExpected,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) <<  failedMsg;
			}

			// Test #23.
			{
				const PersonalBankerType pbType = pbTransferResult->GetPersonalBankerType();

				bool areEqual = pbType == TEST_DATA[testIndex].m_pbTypeExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED pbType=%u expected=%u testIndex=%d in %s",
					pbType,
					TEST_DATA[testIndex].m_pbTypeExpected,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) <<  failedMsg;
			}

			// Test #24.
			{
				const PersonalBankerTransferType pbPromoTransferType = pbTransferResult->GetPromoPersonalBankerTransferType();

				bool areEqual = pbPromoTransferType == TEST_DATA[testIndex].m_pbPromoTransferTypeExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED pbPromoTransferType=%u expected=%u testIndex=%d in %s",
					pbPromoTransferType,
					TEST_DATA[testIndex].m_pbPromoTransferTypeExpected,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) <<  failedMsg;
			}

			// Test #25.
			{
				const PersonalBankerType pbPromoType = pbTransferResult->GetPromoPersonalBankerType();

				bool areEqual = pbPromoType == TEST_DATA[testIndex].m_pbPromoTypeExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED pbPromoType=%u expected=%u testIndex=%d in %s",
					pbPromoType,
					TEST_DATA[testIndex].m_pbPromoTypeExpected,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) <<  failedMsg;
			}
		}

		// Test #26.
		{
			const PersonalBankerAction action = pb.GetAction();

			// Tests #0 and #1 have two possible expected values
			if ( testIndex > 1)
			{
				const bool areEqual = action == TEST_DATA[testIndex].m_actionExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED action=%d expected=%d in testIndex=%d in %s",
					action,
					TEST_DATA[testIndex].m_actionExpected,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) <<  failedMsg;
			}
			else
			{
				string failedMsg;
						
				failedMsg = FormatString("FAILED action=%d expected=%d or %d in testIndex=%d in %s",
					action,
					TEST_DATA[testIndex].m_actionExpected,
					PBActionNone,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(action == PBActionSendCardSessionInProgressToUI || action == PBActionNone) <<  failedMsg;
			}
		}

		// Test #27.
			const bool autoTransferPending = varPb.IsAutoTransferPending();
		{

			const bool areEqual = autoTransferPending == TEST_DATA[testIndex].m_autoTransferPendingExpected;
			string failedMsg;
			failedMsg = FormatString("FAILED autoTransferPending=%d expected=%d in testIndex=%d in %s(%d)",
				autoTransferPending,
				TEST_DATA[testIndex].m_autoTransferPendingExpected,
				testIndex,
				(__TFUNCTION__), __LINE__);
			ASSERT_TRUE(areEqual) <<  failedMsg;
		}

		// Cleanup.
		delete pbInfoResult;
	}
}

/// <summary>
/// Test the implementation of:
///     CPersonalBanker::ProcessPersonalBankerInfo()
///     CPersonalBanker::ProcessPersonalBankerInfoAuto()
/// Functional test of CPersonalBanker::ProcessPersonalBankerInfoAuto().
/// </summary>
TEST_F(PersonalBanker4, ProcessPersonalBankerInfoAutoLowerThanPoller_12_11_6000)
{
	// Test data - inputs.
	const char* pbtID = NULL;
	const int pbtIDLength = 0;
	const CardType cardType = CARD_UNKNOWN;
	const PersonalBankerType pbType(Unspecified);
	const PersonalBankerState pbStateTransferComplete(PBStateUnknown);

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
		const PersonalBankerState m_pbState;

		const byte m_transferCardNumber[CARD_ID_LEN];
		const byte m_infoCardNumber[CARD_ID_LEN];

		const int m_infoPbtIDLength;
		const char m_infoPbtID[MAX_PBID_LENGTH];
		const bool m_useInfo;
		const PBError m_errorCode;
		const DWORD m_minCashUp;
		const DWORD m_maxCashUp;
		const DWORD m_minPromoUp;
		const DWORD m_maxPromoUp;

		const PersonalBankerTransferType m_pbTransferType;
		const PersonalBankerType m_pbType;
		const PersonalBankerTransferType m_pbPromoTransferType; // 2013-06-24 Not directly settable.
		const PersonalBankerType m_pbPromoType; // 2013-06-24 Not directly settable.

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
		const int m_transferPbtIDLengthExpected;
		const char m_transferPbtIDExpected[MAX_PBID_LENGTH];
		const bool m_infoExpected;
		const bool m_transferExpected;
		const DWORD m_transferMinCashUpExpected;
		const DWORD m_transferMaxCashUpExpected;
		const DWORD m_transferMinPromoUpExpected;
		const DWORD m_transferMaxPromoUpExpected;
		const PersonalBankerAction m_actionExpected;
		const bool m_autoTransferPendingExpected;

		const PersonalBankerTransferType m_pbTransferTypeExpected;
		const PersonalBankerType m_pbTypeExpected;
		const PersonalBankerTransferType m_pbPromoTransferTypeExpected;
		const PersonalBankerType m_pbPromoTypeExpected;
	} TestDataType;

	const TestDataType TEST_DATA[] =
	{
		// Index #0. Call ProcessPersonalBankerInfo() with pbInfo.HasError() true.
		{ true, true, true, true, false, false, true, true, true, true, false, PBStateInquireSent,
		{ 0x12, 0x34, 0x56, 0x78, 0x90 },{ 0x12, 0x34, 0x56, 0x78, 0x90 },
		7,{ 'a', 'b', 'c', 'd', 'e', 'f', 'g', 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 },
		true, AccessDenied, 1, 2, 3, 4,
		CashToGame, SessionRequest, PointsToGame, CashDownload,
		true, true, true, true, false, true, true,
		1, 2, 3, 4, PBStateInquireSent,
		0,{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
		true, false, 0, 0, 0, 0, PBActionSendCardSessionInProgressToUI, true,
		Undeclared, Unspecified, PromoDollarsFromGame, PromoCreditsUploadAuto },

		// Index #1. Call ProcessPersonalBankerInfo() with pbInfo.HasError() true.
		{ true, true, true, true, false, false, true, true, true, true, false, PBStateInquireSent,
		{ 0x12, 0x34, 0x56, 0x78, 0x90 },{ 0x12, 0x34, 0x56, 0x78, 0x90 },
		7,{ 'a', 'b', 'c', 'd', 'e', 'f', 'g', 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 },
		true, InvalidID, 1, 2, 3, 4,
		PromoDollarsToGame, PointRedemptionToCash, CashFromGame, CashUpload,
		true, true, true, true, false, true, true,
		1, 2, 3, 4, PBStateInquireSent,
		0,{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
		true, false, 0, 0, 0, 0, PBActionSendCardSessionInProgressToUI, true,
		Undeclared, Unspecified, PromoDollarsFromGame, PromoCreditsUploadAuto },

		// Index #2. Call ProcessPersonalBankerInfo() with m_state == PBStateUnknown.
		{ true, true, true, true, false, false, true, true, true, true, false, PBStateUnknown,
		{ 0x12, 0x34, 0x56, 0x78, 0x90 },{ 0x12, 0x34, 0x56, 0x78, 0x90 },
		8,{ 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 },
		true, NoError, 1, 2, 3, 4,
		CashFromGame, ChangePinAtSentinel, PointsFromGame, PointRedemptionToPromoCredits,
		true, true, true, true, false, true, true,
		1, 2, 3, 4, PBStateUnknown,
		0,{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
		true, true, 0, 0, 0, 0, PBActionSendDisplayPBAnimationToUI, true,
		Undeclared, Unspecified, Undeclared, Unspecified },

		// Index #3. Call ProcessPersonalBankerInfo() with m_state == PBStateInfoReceived.
		{ true, true, true, true, false, false, true, true, true, false, false, PBStateInfoReceived,
		{ 0x12, 0x34, 0x56, 0x78, 0x90 },{ 0x12, 0x34, 0x56, 0x78, 0x90 },
		0,{ 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 },
		true, NoError, 0, 0, 0, 0,
		PointsFromGame, TransferCreditsFromGame, AllFromGame, SessionRelease,
		true, true, true, true, false, true, true,
		0, 0, 0, 0, PBStateInfoReceived,
		0,{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
		true, true, 0, 0, 0, 0, PBActionSendDisplayPBAnimationToUI, true,
		Undeclared, Unspecified, Undeclared, Unspecified },

		// Index #4. Call ProcessPersonalBankerInfo() with m_state == PBStateInfoReceived and pbInfo.m_PBIDLength == 0.
		{ true, true, true, true, false, false, true, true, true, true, false, PBStateInquireSent,
		{ 0x12, 0x34, 0x56, 0x78, 0x90 },{ 0x12, 0x34, 0x56, 0x78, 0x90 },
		0,{ 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 },
		true, NoError, 0, 0, 0, 0,
		PromoDollarsFromGame, CanceledSessionRequest, Undeclared, CanceledCashDownload,
		true, true, true, true, false, true, true,
		0, 0, 0, 0, PBStateInquireSent,
		0,{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
		true, true, 0, 0, 0, 0, PBActionSendDisplayPBAnimationToUI, true,
		Undeclared, Unspecified, Undeclared, Unspecified },

		// Index #5. Call ProcessPersonalBankerInfo() with m_state == PBStateInfoReceived and pbInfo.m_PBIDLength == 0.
		{ true, true, true, true, false, false, true, true, true, true, false, PBStateInquireSent,
		{ 0x12, 0x34, 0x56, 0x78, 0x90 },{ 0x12, 0x34, 0x56, 0x78, 0x90 },
		20,{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		true, NoError, 0, 0, 0, 0,
		AllFromGame, CanceledPromoCreditsToGame, CashToGame, CanceledPromoCreditsToCash,
		true, true, true, true, false, true, true,
		0, 0, 0, 0, PBStateTransferingFromToGame,
		20,{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		true, true, 0, 0, 0, 0, PBActionSendDisplayPBAnimationToUIAndTransferToEGM, true,
		Undeclared, Unspecified, PromoDollarsFromGame, PromoCreditsUploadAuto },

		// Index #6.
		{ true, true, true, true, false, false, true, true, true, false, false, PBStateInquireSent,
		{ 0x12, 0x34, 0x56, 0x78, 0x90 },{ 0x12, 0x34, 0x56, 0x78, 0x90 },
		3,{ 0, 0, 0, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 },
		true, NoError, 0, 0, 0, 0,
		CashToGame, CanceledPointRedemption, PointsToGame, CanceledCashUpload,
		true, true, true, true, false, true, true,
		0, 0, 0, 0, PBStateTransferingFromToGame,
		3,{ 0, 0, 0, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
		true, true, 0, 0, 0, 0, PBActionSendDisplayPBAnimationToUIAndTransferToEGM, true,
		Undeclared, Unspecified, PromoDollarsFromGame, PromoCreditsUploadAuto },

		// Index #7.
		{ true, true, true, true, false, false, true, true, true, true, false, PBStateInquireSent,
		{ 0x12, 0x34, 0x56, 0x78, 0x90 },{ 0x12, 0x34, 0x56, 0x78, 0x90 },
		4,{ 'a', 'b', 'c', 'd', 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 },
		true, NoError, 1, 2, 3, 4,
		PointsToGame, CanceledTransferCreditsFromGame, PromoDollarsToGame, CanceledSessionRelease,
		true, true, true, true, false, true, true,
		1, 2, 3, 4, PBStateTransferingFromToGame,
		4,{ 'a', 'b', 'c', 'd', 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
		true, true, 1, 2, 3, 4, PBActionSendDisplayPBAnimationToUIAndTransferToEGM, true,
		Undeclared, Unspecified, PromoDollarsFromGame, PromoCreditsUploadAuto },

		// Index #8.
		{ true, true, true, true, false, false, true, true, true, false, true, PBStateInquireSent,
		{ 0x12, 0x34, 0x56, 0x78, 0x90 },{ 0x12, 0x34, 0x56, 0x78, 0x90 },
		5,{ '!', '@', '#', '$', '%', 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 },
		true, NoError, 1, 2, 3, 4,
		PromoDollarsToGame, CanceledChangePinAtSentinel, CashFromGame, CanceledPointRedemptionToPromoCredits,
		true, true, true, true, false, true, true,
		1, 2, 3, 4, PBStateTransferingFromToGame,
		5,{ '!', '@', '#', '$', '%', 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
		true, true, 1, 2, 3, 4, PBActionSendDisplayPBAnimationToUIAndTransferToEGM, true,
		Undeclared, Unspecified, PromoDollarsFromGame, PromoCreditsUploadAuto },

		// Index #9.
		{ true, true, true, true, false, false, true, true, true, true, true, PBStateInquireSent,
		{ 0x12, 0x34, 0x56, 0x78, 0x90 },{ 0x12, 0x34, 0x56, 0x78, 0x90 },
		20,{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 },
		true, NoError, 1, 2, 3, 4,
		CashFromGame, CanceledPointCashUpload, PointsFromGame, CanceledPromoCreditsUpload,
		true, true, true, true, false, true, true,
		1, 2, 3, 4, PBStateTransferingFromToGame,
		20,{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 },
		true, true, 1, 2, 3, 4, PBActionSendDisplayPBAnimationToUIAndTransferToEGM, true,
		Undeclared, Unspecified, PromoDollarsFromGame, PromoCreditsUploadAuto },

		// Index #10.
		{ true, true, true, true, false, false, true, true, true, false, true, PBStateInquireSent,
		{ 0x12, 0x34, 0x56, 0x78, 0x90 },{ 0x12, 0x34, 0x56, 0x78, 0x90 },
		3,{ '1', '2', '3', 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 },
		true, NoError, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
		PointsFromGame, CanceledPromoCreditsUploadAuto, AllFromGame, AuthorizeCashToGame,
		true, true, true, true, false, true, true,
		0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, PBStateTransferingFromToGame,
		3,{ '1', '2', '3', 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
		true, true, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, PBActionSendDisplayPBAnimationToUIAndTransferToEGM, true,
		Undeclared, Unspecified, PromoDollarsFromGame, PromoCreditsUploadAuto },

		// Index #11.
		{ true, true, true, true, false, false, true, true, true, true, true, PBStateInquireSent,
		{ 0x12, 0x34, 0x56, 0x78, 0x90 },{ 0x12, 0x34, 0x56, 0x78, 0x90 },
		20,{ (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff,
		(char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff },
		true, NoError, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
		PromoDollarsFromGame, AuthorizePromoToGame, Undeclared, AuthorizePointsToGameCash,
		true, true, true, true, false, true, true,
		0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, PBStateTransferingFromToGame,
		20,{ (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff,
		(char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff },
		true, true, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, PBActionSendDisplayPBAnimationToUIAndTransferToEGM, true,
		Undeclared, Unspecified, PromoDollarsFromGame, PromoCreditsUploadAuto },

		// Index #12. Call ProcessPersonalBankerInfo() with pbInfo=NULL.
		{ true, true, true, true, false, false, true, true, true, true, false, PBStateInquireSent,
		{ 0x12, 0x34, 0x56, 0x78, 0x90 },{ 0x12, 0x34, 0x56, 0x78, 0x90 },
		4,{ 'a', 'b', 'c', 'd', 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 },
		false, NoError, 1, 2, 3, 4,
		AllFromGame, AuthorizePointsToGamePromo, CashToGame, UnknownCreditSource,
		true, true, true, true, false, true, true,
		1, 2, 3, 4, PBStateInquireSent,
		0,{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
		false, true, 0, 0, 0, 0, PBActionNone, true,
		Undeclared, Unspecified, Undeclared, Unspecified },

		// Index #13. Call ProcessPersonalBankerInfo() with different pbInfo.m_cardNumber.
		{ true, true, true, true, false, false, true, true, true, true, false, PBStateInquireSent,
		{ 0x12, 0x34, 0x56, 0x78, 0x90 },{ 0x12, 0x34, 0x56, 0x78, 0x99 },
		6,{ 'a', 'b', 'c', 'd', 'e', 'f', 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 },
		true, NoError, 1, 2, 3, 4,
		CashToGame, InvalidTransIDFromSystem, PointsToGame, NonMatchingTransactionID,
		true, true, true, true, false, true, true,
		1, 2, 3, 4, PBStateInquireSent,
		0,{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
		false, true, 0, 0, 0, 0, PBActionNone, true,
		Undeclared, Unspecified, Undeclared, Unspecified },
	};


	for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
	{
		CConfig varConfig(false, "");
		varConfig.SetPoller_12_11_6000_or_Higher(false);
		varConfig.SetAutoUploadCreditsGlobal(true);
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
		varPbInfo.SetErrorCode(TEST_DATA[testIndex].m_errorCode);
		const CPersonalBankerInfo &pbInfo(TEST_DATA[testIndex].m_useInfo ? varPbInfo : *(CPersonalBankerInfo *)NULL);

		CPersonalBanker varPb;

		CreateInfo(varPb);
		varPb.RemoveSession();
		varPb.CreateTransfer(CUtilities::GetTickCount());

		// SetState() to affect BeginPersonalBankerTransfer().
		varPb.SetState(PBStateInfoReceived);
		CGames games;
		const DWORD amountInDollars = 5;
		byte pbEFTNumber = 1;
		varPb.BeginPersonalBankerTransfer(games,
			TEST_DATA[testIndex].m_pbTransferType,
			amountInDollars,
			pbEFTNumber);

		// SetState() to affect ProcessPersonalBankerTransferComplete().
		varPb.SetState(pbStateTransferComplete);

		CPersonalBankerTransfer pbTransfer(CUtilities::GetTickCount());
		pbTransfer.SetID(pbtID, pbtIDLength);
		byte meterNumberToAddTo(mNone);
		bool isDuplicate(false);
		varPb.ProcessPersonalBankerTransferComplete(&pbTransfer,
			games,
			meterNumberToAddTo,
			isDuplicate,
			TEST_DATA[testIndex].m_isEFT);

		CCard varTransferCard(CTIME_DEFAULT_VALUE, CTIME_DEFAULT_VALUE);
		varTransferCard.SetCardInfo(cardType,
			TEST_DATA[testIndex].m_transferCardNumber,
			sizeof(TEST_DATA[testIndex].m_transferCardNumber));
		const CCard &transferCard(varTransferCard);
		varPb.SetTransferCardNumber(&transferCard, CTIME_DEFAULT_VALUE);

		varPb.RebuildCard();
		varPb.SetAutoInProcess(true);
		varPb.SetPersonalBankerType(pbType);
		varPb.SetState(TEST_DATA[testIndex].m_pbState);
		varPb.ClearPersonalBankerAction();

		// Perform operations to be tested.
		varPb.ProcessPersonalBankerInfo(&pbInfo,
			config,
			TEST_DATA[testIndex].m_gameOnline,
			TEST_DATA[testIndex].m_bIsEFT);
		const CPersonalBanker &pb(varPb);

		// Test #1.
		const CPersonalBankerInfo *pbInfoResult = pb.GetInfoCopy();

		{
			bool areEqual = ((pbInfoResult != NULL) == TEST_DATA[testIndex].m_infoExpected);

			string failedMsg;
			failedMsg = FormatString("FAILED pbInfoResult=%p expected=%s testIndex=%d in %s",
				(const void *)pbInfoResult,
				(TEST_DATA[testIndex].m_infoExpected ? "NOT NUL" : _T("NULL")),
				testIndex,
				(__TFUNCTION__));
			ASSERT_TRUE(areEqual) << failedMsg;
		}

		if (TEST_DATA[testIndex].m_infoExpected)
		{
			// Test #2.
			{
				const DWORD infoMinCashUp = pbInfoResult->GetMinimumCashUp();

				bool areEqual = infoMinCashUp == TEST_DATA[testIndex].m_infoMinCashUpExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED infoMinCashUp=%u expected=%u testIndex=%d in %s",
					infoMinCashUp,
					TEST_DATA[testIndex].m_infoMinCashUpExpected,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #3.
			{
				const DWORD infoMaxCashUp = pbInfoResult->GetMaximumCashUp();

				bool areEqual = infoMaxCashUp == TEST_DATA[testIndex].m_infoMaxCashUpExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED infoMaxCashUp=%u expected=%u testIndex=%d in %s",
					infoMaxCashUp,
					TEST_DATA[testIndex].m_infoMaxCashUpExpected,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #4.
			{
				const DWORD infoMinPromoUp = pbInfoResult->GetMinimumPromoUp();

				bool areEqual = infoMinPromoUp == TEST_DATA[testIndex].m_infoMinPromoUpExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED infoMinPromoUp=%u expected=%u testIndex=%d in %s",
					infoMinPromoUp,
					TEST_DATA[testIndex].m_infoMinPromoUpExpected,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #5.
			{
				const DWORD infoMaxPromoUp = pbInfoResult->GetMaximumPromoUp();

				bool areEqual = infoMaxPromoUp == TEST_DATA[testIndex].m_infoMaxPromoUpExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED infoMaxPromoUp=%u expected=%u testIndex=%d in %s",
					infoMaxPromoUp,
					TEST_DATA[testIndex].m_infoMaxPromoUpExpected,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #6.
			{
				const bool bTransferCashToGameEnabled = pbInfoResult->IsTransferCashToGameEnabled();

				bool areEqual = bTransferCashToGameEnabled == TEST_DATA[testIndex].m_bTransferCashToGameEnabledExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED bTransferCashToGameEnabled=%d expected=%d testIndex=%d in %s",
					bTransferCashToGameEnabled,
					TEST_DATA[testIndex].m_bTransferCashToGameEnabledExpected,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #7.
			{
				const bool bTransferPointsToGameEnabled = pbInfoResult->IsTransferPointsToGameEnabled();

				bool areEqual = bTransferPointsToGameEnabled == TEST_DATA[testIndex].m_bTransferPointsToGameEnabledExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED bTransferPointsToGameEnabled=%d expected=%d testIndex=%d (%d %d %d %d %d %d) in %s",
					bTransferPointsToGameEnabled,
					TEST_DATA[testIndex].m_bTransferPointsToGameEnabledExpected,
					testIndex,
					TEST_DATA[testIndex].m_bTransferPointsToGameEnabled,
					config.PBTPointsEnabledGlobal(),
					config.PBTPointsEnabled(),
					TEST_DATA[testIndex].m_bIsEFT,
					config.PBTPointsAsPromo(),
					pbInfo.IsTransferPointsToGameEnabled(),
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #8.
			{
				const bool bTransferPromoDollarsToGameEnabled = pbInfoResult->IsTransferPromoDollarsToGameEnabled();

				bool areEqual = bTransferPromoDollarsToGameEnabled == TEST_DATA[testIndex].m_bTransferPromoDollarsToGameEnabledExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED bTransferPromoDollarsToGameEnabled=%d expected=%d testIndex=%d in %s",
					bTransferPromoDollarsToGameEnabled,
					TEST_DATA[testIndex].m_bTransferPromoDollarsToGameEnabledExpected,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #9.
			{
				const bool bTransferCashFromGameEnabled = pbInfoResult->IsTransferCashFromGameEnabled();

				bool areEqual = bTransferCashFromGameEnabled == TEST_DATA[testIndex].m_bTransferCashFromGameEnabledExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED bTransferCashFromGameEnabled=%d expected=%d testIndex=%d in %s",
					bTransferCashFromGameEnabled,
					TEST_DATA[testIndex].m_bTransferCashFromGameEnabledExpected,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #10.
			{
				const bool bGameDenomMatches = pbInfoResult->DoesGameDenomMatch();

				bool areEqual = bGameDenomMatches == TEST_DATA[testIndex].m_bGameDenomMatchesExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED bGameDenomMatches=%d expected=%d testIndex=%d in %s",
					bGameDenomMatches,
					TEST_DATA[testIndex].m_bGameDenomMatchesExpected,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #11.
			{
				const bool bTransferPromoFromGameEnabled = pbInfoResult->IsTransferPromoFromGameEnabled();

				bool areEqual = bTransferPromoFromGameEnabled == TEST_DATA[testIndex].m_bTransferPromoFromGameEnabledExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED bTransferPromoFromGameEnabled=%d expected=%d testIndex=%d in %s",
					bTransferPromoFromGameEnabled,
					TEST_DATA[testIndex].m_bTransferPromoFromGameEnabledExpected,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #12.
			{
				const bool bTransferToGameEnabled = pbInfoResult->IsTransferToGameEnabled();

				bool areEqual = bTransferToGameEnabled == TEST_DATA[testIndex].m_bTransferToGameEnabledExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED bTransferToGameEnabled=%d expected=%d testIndex=%d (%d %d %d %d) in %s",
					bTransferToGameEnabled,
					TEST_DATA[testIndex].m_bTransferToGameEnabledExpected,
					testIndex,
					TEST_DATA[testIndex].m_bTransferCashToGameEnabledExpected,
					TEST_DATA[testIndex].m_bTransferPointsToGameEnabledExpected,
					TEST_DATA[testIndex].m_bTransferPromoDollarsToGameEnabledExpected,
					TEST_DATA[testIndex].m_bEGMInitialized,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}
		}

		// Test #13.
		{
			const PersonalBankerState pbStateResult = pb.GetState();

			bool areEqual = pbStateResult == TEST_DATA[testIndex].m_pbStateExpected;
			string failedMsg;
			failedMsg = FormatString("FAILED pbStateResult=%d expected=%d testIndex=%d in %s",
				pbStateResult,
				TEST_DATA[testIndex].m_pbStateExpected,
				testIndex,
				(__TFUNCTION__));
			ASSERT_TRUE(areEqual) << failedMsg;
		}


		// Test #14.
		const CPersonalBankerTransfer *pbTransferResult = pb.GetTransferCopy();

		{
			bool areEqual = ((pbTransferResult != NULL) == TEST_DATA[testIndex].m_transferExpected);

			string failedMsg;
			failedMsg = FormatString("FAILED pbTransferResult=%p expected=%s testIndex=%d in %s(%d)",
				(const void *)pbTransferResult,
				(TEST_DATA[testIndex].m_transferExpected ? "NOT NUL" : _T("NULL")),
				testIndex,
				(__TFUNCTION__), __LINE__);
			ASSERT_TRUE(areEqual) << failedMsg;
		}

		if (TEST_DATA[testIndex].m_transferExpected)
		{
			// Test #15.
			{
				const int transferPbtIDLength = pbTransferResult->GetPBTIDLength();

				bool areEqual = transferPbtIDLength == TEST_DATA[testIndex].m_transferPbtIDLengthExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED transferPbtIDLength=%d expected=%d testIndex=%d in %s",
					transferPbtIDLength,
					TEST_DATA[testIndex].m_transferPbtIDLengthExpected,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #16.
			const char *transferPbtID = pbTransferResult->GetID();

			{
				string failedMsg;
				failedMsg = FormatString("FAILED transferPbtID=%p expected=NOT NULL testIndex=%d in %s",
					(const void *)transferPbtID,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(transferPbtID != NULL) << failedMsg;
			}

			// Test #17.

			{
				size_t memcmpSize = (size_t)TEST_DATA[testIndex].m_transferPbtIDLengthExpected;
				bool areEqual = 0 == memcmp(transferPbtID,
					TEST_DATA[testIndex].m_transferPbtIDExpected,
					memcmpSize);

				string resultString = CUtilities::ToHexString(reinterpret_cast<const BYTE *>(transferPbtID),
					(DWORD)memcmpSize);
				string expectedString =
					CUtilities::ToHexString(reinterpret_cast<const BYTE *>(TEST_DATA[testIndex].m_transferPbtIDExpected),
					(DWORD)memcmpSize);
				string failedMsg;
				failedMsg = FormatString("FAILED transferPbtID={%s} expected={%s} testIndex=%d in %s",
					(resultString).c_str(),
					(expectedString).c_str(),
					testIndex,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #18.
			{
				const DWORD transferMinCashUp = pbTransferResult->GetMinCashUp();

				bool areEqual = transferMinCashUp == TEST_DATA[testIndex].m_transferMinCashUpExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED transferMinCashUp=%u expected=%u testIndex=%d in %s",
					transferMinCashUp,
					TEST_DATA[testIndex].m_transferMinCashUpExpected,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #19.
			{
				const DWORD transferMaxCashUp = pbTransferResult->GetMaxCashUp();

				bool areEqual = transferMaxCashUp == TEST_DATA[testIndex].m_transferMaxCashUpExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED transferMaxCashUp=%u expected=%u testIndex=%d in %s",
					transferMaxCashUp,
					TEST_DATA[testIndex].m_transferMaxCashUpExpected,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #20.
			{
				const DWORD transferMinPromoUp = pbTransferResult->GetMinPromoUp();

				bool areEqual = transferMinPromoUp == TEST_DATA[testIndex].m_transferMinPromoUpExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED transferMinPromoUp=%u expected=%u testIndex=%d in %s",
					transferMinPromoUp,
					TEST_DATA[testIndex].m_transferMinPromoUpExpected,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #21.
			{
				const DWORD transferMaxPromoUp = pbTransferResult->GetMaxPromoUp();

				bool areEqual = transferMaxPromoUp == TEST_DATA[testIndex].m_transferMaxPromoUpExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED transferMaxPromoUp=%u expected=%u testIndex=%d in %s",
					transferMaxPromoUp,
					TEST_DATA[testIndex].m_transferMaxPromoUpExpected,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #22.
			{
				const PersonalBankerTransferType pbTransferType = pbTransferResult->GetPersonalBankerTransferType();

				bool areEqual = pbTransferType == TEST_DATA[testIndex].m_pbTransferTypeExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED pbTransferType=%u expected=%u testIndex=%d in %s",
					pbTransferType,
					TEST_DATA[testIndex].m_pbTransferTypeExpected,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #23.
			{
				const PersonalBankerType pbType = pbTransferResult->GetPersonalBankerType();

				bool areEqual = pbType == TEST_DATA[testIndex].m_pbTypeExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED pbType=%u expected=%u testIndex=%d in %s",
					pbType,
					TEST_DATA[testIndex].m_pbTypeExpected,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #24.
			{
				const PersonalBankerTransferType pbPromoTransferType = pbTransferResult->GetPromoPersonalBankerTransferType();

				bool areEqual = pbPromoTransferType == TEST_DATA[testIndex].m_pbPromoTransferTypeExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED pbPromoTransferType=%u expected=%u testIndex=%d in %s",
					pbPromoTransferType,
					TEST_DATA[testIndex].m_pbPromoTransferTypeExpected,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #25.
			{
				const PersonalBankerType pbPromoType = pbTransferResult->GetPromoPersonalBankerType();

				bool areEqual = pbPromoType == TEST_DATA[testIndex].m_pbPromoTypeExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED pbPromoType=%u expected=%u testIndex=%d in %s",
					pbPromoType,
					TEST_DATA[testIndex].m_pbPromoTypeExpected,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}
		}

		// Test #26.
		{
			const PersonalBankerAction action = pb.GetAction();

			// Tests #0 and #1 have two possible expected values
			if (testIndex > 1)
			{
				const bool areEqual = action == TEST_DATA[testIndex].m_actionExpected;
				string failedMsg;
				failedMsg = FormatString("FAILED action=%d expected=%d in testIndex=%d in %s",
					action,
					TEST_DATA[testIndex].m_actionExpected,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}
			else
			{
				string failedMsg;

				failedMsg = FormatString("FAILED action=%d expected=%d or %d in testIndex=%d in %s",
					action,
					TEST_DATA[testIndex].m_actionExpected,
					PBActionNone,
					testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(action == PBActionSendCardSessionInProgressToUI || action == PBActionNone) << failedMsg;
			}
		}

		// Test #27.
		const bool autoTransferPending = varPb.IsAutoTransferPending();
		{

			const bool areEqual = autoTransferPending == TEST_DATA[testIndex].m_autoTransferPendingExpected;
			string failedMsg;
			failedMsg = FormatString("FAILED autoTransferPending=%d expected=%d in testIndex=%d in %s(%d)",
				autoTransferPending,
				TEST_DATA[testIndex].m_autoTransferPendingExpected,
				testIndex,
				(__TFUNCTION__), __LINE__);
			ASSERT_TRUE(areEqual) << failedMsg;
		}

		// Cleanup.
		delete pbInfoResult;
	}
}

TEST_F(PersonalBanker4, GetSubscriptionDataResponse_PBTransferTest1)
{
	// Test data - inputs.
	const int pbtIDLength = 1;
	const char pbtID[pbtIDLength] = { '1' };
	CConfig config(false, "");
	config.SetShowPointsVsAccountBalance(true);
	const bool gameOnline(false);
	const bool bIsEFT(false);

	// Test data - expected results.
	string expectedResponse;
	expectedResponse = R"({"Successful":true,)";
	expectedResponse += R"("Error":"",)";
	expectedResponse += R"("TransferType":"CashFromGame",)";
	expectedResponse += R"("Cash":0.2,)";
	expectedResponse += R"("Promo":0.0,)";
	expectedResponse += R"("CashoutErrorFlags":0,)";
	expectedResponse += R"("AutoUploadErrorFlags":0})";

	// Setup.
	CPersonalBankerInfo *pbInfo = new CPersonalBankerInfo();
	pbInfo->SetPersonalBankerID(pbtID, pbtIDLength);
	pbInfo->SetErrorCode(PBError::NoError);
	CPersonalBanker varPb;
	varPb.CreateTransfer(CUtilities::GetTickCount());
	varPb.ProcessPersonalBankerCreditTransfer(pbInfo);
	varPb.ProcessPersonalBankerInfo(pbInfo, config, gameOnline, bIsEFT);
	varPb.ProcessBalancesFromEGM(2000, 0, 0, false, false, config);
	CGames games;
	byte pbEFTNumber = 10;
	varPb.BeginPersonalBankerTransfer(games, PersonalBankerTransferType::CashFromGame, 20, pbEFTNumber);
	varPb.SetPersonalBankerAction(PBActionSendCompleteToUI);
	varPb.m_pbStateForUi = PBStateTransferComplete;

	// Perform operations to be tested.
	std::string response = varPb.GetSubscriptionDataResponse("PBTransfer", "");
	ASSERT_EQ(response, expectedResponse) << "Incorrect response data";
}

TEST_F(PersonalBanker4, GetSubscriptionDataResponse_PBTransferTest2)
{
	// Test data - inputs.
	const int pbtIDLength = 1;
	const char pbtID[pbtIDLength] = { '1' };

	// Test data - expected results.
	string expectedResponse;
	expectedResponse = R"({"Successful":false,)";
	expectedResponse += R"("Error":"authorizationDenied"})";

	// Setup.
	CPersonalBankerInfo *pbInfo = new CPersonalBankerInfo();
	pbInfo->SetPersonalBankerID(pbtID, pbtIDLength);
	pbInfo->SetErrorCode(PBError::NoError);
	CPersonalBanker varPb;
	varPb.CreateTransfer(CUtilities::GetTickCount());
	varPb.ProcessPersonalBankerCreditTransfer(pbInfo);
	varPb.SetPersonalBankerAction(PBActionSendCompleteToUI);
	varPb.m_pbStateForUi = PBStateAuthDenied;

	// Perform operations to be tested.
	std::string response = varPb.GetSubscriptionDataResponse("PBTransfer", "");
	ASSERT_EQ(response, expectedResponse) << "Incorrect response data";
}

TEST_F(PersonalBanker4, GetSubscriptionDataResponse_PBTransferTest3)
{
	// Test data - inputs.
	const int pbtIDLength = 1;
	const char pbtID[pbtIDLength] = { '1' };

	// Test data - expected results.
	string expectedResponse;
	expectedResponse = R"({"Successful":false,)";
	expectedResponse += R"("Error":"doorOpen"})";

	// Setup.
	CPersonalBankerInfo *pbInfo = new CPersonalBankerInfo();
	pbInfo->SetPersonalBankerID(pbtID, pbtIDLength);
	pbInfo->SetErrorCode(PBError::NoError);
	CPersonalBanker varPb;
	varPb.CreateTransfer(CUtilities::GetTickCount());
	varPb.ProcessPersonalBankerCreditTransfer(pbInfo);
	varPb.SetPersonalBankerAction(PBActionSendCompleteToUI);
	varPb.m_pbTransfer->SetPersonalBankerType(PersonalBankerType::CanceledCashDownload);
	varPb.m_pbTransfer->SetCancelledType(PollerCancellationType::DoorOpen);
	varPb.m_pbStateForUi = PBStateTransferComplete;

	// Perform operations to be tested.
	std::string response = varPb.GetSubscriptionDataResponse("PBTransfer", "");
	ASSERT_EQ(response, expectedResponse) << "Incorrect response data";
}
