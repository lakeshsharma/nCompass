#if 000//TODO-PORT
#include "stdafx.h"
#include "ProgressiveProtocol/EGMAwardManager.h"
#include "ProgressiveProtocol/StandardProgressiveWins.h"
#include "ProgressiveWinsMock.h"
#include "CumulativeProgressiveManagerMock.h"
#include "ProgressiveProtocol/ProgressiveCriticalText.h"
#include "ProgressiveLevelTestHelpers.h"
#include "UnitTestingToString.h"
#include "UnitTestingUtils.h"
#include "TestUtil.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using ::testing::Return;
using ::testing::Mock;
using ::testing::_;

namespace SentinelNativeUnitTests
{
	using namespace ProgressiveLevelTestHelpers;

// AssertResult macros for calling CUnitTestingUtils from a test method.
#define AssertResult3(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, testIndex, __TFUNCTION__, __LINE__);
#define AssertResult2(result, expected) AssertResult3(result, expected, _T(#result))

// AssertResult macros for calling CUnitTestingUtils from a helper method.
#define AssertResult3h(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, testIndex, functionName, lineNumber, extraIndex);
#define AssertResult2h(result, expected) AssertResult3h(result, expected, _T(#result))

// AssertResult macro for calling CUnitTestingUtils (with an extra index) from a test method.
#define AssertResult3x(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, testIndex, __TFUNCTION__, __LINE__, &extraIndex);
#define AssertResult2x(result, expected) AssertResult3x(result, expected, _T(#result))

	TEST_CLASS(EGMAwardManager)
	{
		// Default expected result(s).
		static const __int64 m_awardAmountPaidExpected = 0;
		static const EGMAwardState m_awardStateExpected = (EGMAwardState)0; // Use the value required for backwards compatibility with NVRAM.
		static const int m_levelIdExpected = 0;
		static const LONG64 m_jackpotIdExpected = -1;
		static const LONG64 m_remoteJackpotIdExpected = -1;
		static const __int64 m_awardAmountHitExpected = 0;
		static const __int64 m_originalMachineBonusMeterValueExpected = -1;
		static const __int64 m_originalAttendantBonusingMeterValueExpected = -1;
		static const int m_sentToGameCountExpected = 0;
		static const SYSTEMTIME m_sentToGameTimeExpected;
		static const SYSTEMTIME m_receivedFromSystemTimeExpected;
		static const DWORD m_duplicateHitCountExpected = 0;
		static const DWORD m_otherHitCountExpected = 0;
		static const bool m_inProgressExpected = false;
		static const CString m_logInfoStringExpected;

		// Default expected CMemoryStorage member values.
		static const MEMORY_TYPE m_memoryTypeExpected = MEMORY_NONE;
		static const DWORD m_dwAllowedSizeExpected = 710226; // Use the value required for backwards compatibility with NVRAM.
		static const DWORD m_dwOffsetExpected = 256; // Use the value required for backwards compatibility with NVRAM.
		static const bool m_bOffsetIsSetExpected = true;

		void AssertEqual(const CEGMAward &obj,
			__int64 awardAmountPaidExpected,
			EGMAwardState awardStateExpected,
			int levelIdExpected,
			LONG64 jackpotIdExpected,
			LONG64 remoteJackpotIdExpected,
			__int64 awardAmountHitExpected,
			__int64 originalMachineBonusMeterValueExpected,
			__int64 originalAttendantBonusingMeterValueExpected,
			int sentToGameCountExpected,
			const SYSTEMTIME& sentToGameTimeExpected,
			const SYSTEMTIME& receivedFromSystemTimeExpected,
			DWORD duplicateHitCountExpected,
			DWORD otherHitCountExpected,
			bool inProgressExpected,
			const CString& logInfoStringExpected,
			MEMORY_TYPE memoryTypeExpected,
			int testIndex,
			LPCTSTR functionName,
			int lineNumber,
			const int* extraIndex = NULL)
		{
			AssertResult2h(obj.GetAwardAmountPaid(), awardAmountPaidExpected);
			AssertResult2h(obj.GetAwardState(), awardStateExpected);
			AssertResult2h(obj.GetLevelId(), levelIdExpected);
			AssertResult2h(obj.GetJackpotId(), jackpotIdExpected);
			AssertResult2h(obj.GetRemoteJackpotId(), remoteJackpotIdExpected);
			AssertResult2h(obj.GetAwardAmountHit(), awardAmountHitExpected);
			AssertResult2h(obj.GetOriginalMachineBonusMeterValue(), originalMachineBonusMeterValueExpected);
			AssertResult2h(obj.GetOriginalAttendantBonusingMeterValue(), originalAttendantBonusingMeterValueExpected);
			AssertResult2h(obj.GetSentToGameCount(), sentToGameCountExpected);
			AssertResult2h(obj.GetSentToGameTime(), sentToGameTimeExpected);
			AssertResult2h(obj.GetReceivedFromSystemTime(), receivedFromSystemTimeExpected);
			AssertResult2h(obj.GetDuplicateHitCount(), duplicateHitCountExpected);
			AssertResult2h(obj.GetOtherHitCount(), otherHitCountExpected);
			AssertResult2h(obj.InProgress(), inProgressExpected);
			AssertResult2h(obj.GetLogInfoString(_T("")), logInfoStringExpected);
			AssertResult2h(obj.GetMemoryType(), memoryTypeExpected);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CEGMAward::SetInactiveLocation
		///     EGMAwardManager::EGMAwardManager
		///     CEGMAward::CEGMAward()
		///     CEGMAward::GetAwardAmountPaid()
		///     CEGMAward::GetAwardState()
		///     CEGMAward::GetLevelId()
		///     CEGMAward::GetJackpotId()
		///     CEGMAward::GetRemoteJackpotId()
		///     CEGMAward::GetAwardAmountHit()
		///     CEGMAward::GetOriginalMachineBonusMeterValue()
		///     CEGMAward::GetOriginalAttendantBonusingMeterValue()
		///     CEGMAward::GetSentToGameCount()
		///     CEGMAward::GetSentToGameTime()
		///     CEGMAward::GetDuplicateHitCount()
		///     CEGMAward::GetOtherHitCount()
		///     CEGMAward::GetLogInfoString()
		///     CEGMAward::GetMemoryType()
		/// Code coverage:
		///     CEGMAward::~CEGMAward()
		/// </summary>
		TEST_METHOD(ProgressiveConfigTest)
		{
			// Setup.
			const int testIndex(0);

			// Perform operation(s) to be tested.
			CEGMAward::SetInactiveLocation();
			const ProgressiveLevelManager progressiveLevelManager;
			CEGMAwardManager egmAwardManager(progressiveLevelManager);
			const CEGMAward *egmAward(egmAwardManager.GetEGMAwardCopy());

			// Test(s).
			AssertEqual(*egmAward,
				m_awardAmountPaidExpected,
				m_awardStateExpected,
				m_levelIdExpected,
				m_jackpotIdExpected,
				m_remoteJackpotIdExpected,
				m_awardAmountHitExpected,
				m_originalMachineBonusMeterValueExpected,
				m_originalAttendantBonusingMeterValueExpected,
				m_sentToGameCountExpected,
				m_sentToGameTimeExpected,
				m_receivedFromSystemTimeExpected,
				m_duplicateHitCountExpected,
				m_otherHitCountExpected,
				m_inProgressExpected,
				m_logInfoStringExpected,
				m_memoryTypeExpected,
				testIndex, __TFUNCTION__, __LINE__);
		}

		/// <summary>
		/// Functional test for:
		///     CEGMAwardManager::CEGMAwardManager(MEMORY_TYPE ...)
		/// Test constructor for correct m_egmAward memory storage parameters.
		/// </summary>
		TEST_METHOD(CEGMAwardManager_MemoryStorageParameters)
		{
			// Data.
			const MEMORY_TYPE memoryType(MEMORY_NVRAM);

			const MEMORY_TYPE memoryTypeExpected(memoryType);
			const DWORD nvramActiveOffsetExpected(NVRAM_EGM_AWARD_CHECKBYTE_OFFSET);
			const DWORD nvramDataOffsetExpected(nvramActiveOffsetExpected + NVRAM_EGM_AWARD_CHECKBYTE_SIZE);
			const DWORD nvramDataSizeExpected(CEGMAward::NVRAM_DATA_SIZE);
			const BYTE nvramActiveValueExpected(CEGMAward::NVRAM_ACTIVE_VALUE);

			// Perform operation(s) and test(s).
			const ProgressiveLevelManager progressiveLevelManager;
			CEGMAwardManager obj(progressiveLevelManager, memoryType, false);
			Assert::AreEqual(memoryTypeExpected, obj.m_egmAward.m_type, L"obj.m_type");
			Assert::AreEqual(nvramDataOffsetExpected, obj.m_egmAward.m_dwOffset, L"obj.m_dwOffset");
			Assert::AreEqual(nvramDataSizeExpected, obj.m_egmAward.m_dwAllowedSize, L"obj.m_dwAllowedSize");
			Assert::AreEqual(nvramActiveOffsetExpected, obj.m_egmAward.m_activeOffset, L"obj.m_activeOffset");
			Assert::AreEqual(nvramActiveValueExpected, obj.m_egmAward.m_activeValue, L"obj.m_activeValue");
		}

		/// <summary>
		/// Functional test for:
		///     CEGMAwardManager::ProcessMessage()
		/// Test that an award received from One Link with various MethodOfPayment values is processed correctly.
		/// </summary>
		void ProcessMessage_ReceivedFromSystemTest(MethodOfPayment::Type methodOfPayment, LPCSTR pMethodOfPayment)
		{
			// Data.
			const SYSTEMTIME receivedTime = { 2001, 2, 0, 3, 12, 34, 56, 789 };
			CStringA json;
			json.Format(
				"{"
				"\"LevelId\":1,"
				"\"JackpotId\":2,"
				"\"RemoteJackpotId\":3,"
				"\"Amount\":4.44,"
				"\"JackpotType\":\"Bonus\","
				"\"MethodOfPayment\":\"%s\""
				"}", pMethodOfPayment);
			rapidjson::Document jsonDoc;
			bool result = jsonDoc.Parse(json).HasParseError();
			Assert::IsFalse(result, AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError())));

			// Setup.
			CEGMAward *award = new CEGMAward(receivedTime, jsonDoc);
			Assert::AreEqual(methodOfPayment, award->GetMethodOfPayment(), L"GetMethodOfPayment setup");

			const ProgressiveLevelManager progressiveLevelManager;
			CEGMAwardManager egmAwardManager(progressiveLevelManager, MEMORY_NONE, false);
			CInternalMessage msg(INT_MSG_EGM_AWARD, award);
			CInternalMessage* targetMsg(NULL);
			MessageQueueId target(mqNone);
			CInternalMessage* targetSecondMsg(NULL);
			MessageQueueId targetSecond(mqNone);
			std::vector<MsgTargetPair*> *returnMessages = new std::vector<MsgTargetPair*>();

			SYSTEMTIME systemTime;
			GetSystemTime(&systemTime);
			CGames *games = new CGames(MEMORY_NONE);
			games->SetAftBonusEnabled(true);
			games->SetAccountingDenom(1);

			// Perform operation(s) and test(s).
			egmAwardManager.ProcessMessage(GetTickCount(), systemTime, *games,
				false,
				&msg,
				mqProgressive,
				targetMsg,
				target,
				returnMessages);

			if (MethodOfPayment::PayToGame == methodOfPayment || MethodOfPayment::PayToGameWithHandPay == methodOfPayment)
			{
				Assert::AreEqual(mqGCF, target, L"target");
				Assert::IsNotNull(targetMsg, L"targetMsg");
				Assert::AreEqual(INT_MSG_EGM_AWARD, targetMsg->GetMessageType(), L"GetMessageType");
				CEGMAward *egmAward(targetMsg->GetEGMAward());

				Assert::IsNotNull(egmAward, L"egmAward");
				Assert::AreEqual((int)1, egmAward->GetLevelId(), L"GetLevelId");
				Assert::AreEqual((__int64)2, egmAward->GetJackpotId(), L"GetJackpotId");
				Assert::AreEqual((__int64)444, egmAward->GetAwardAmount(), L"GetAwardAmount");
				Assert::AreEqual(methodOfPayment, egmAward->GetMethodOfPayment(), L"GetMethodOfPayment");
				Assert::AreEqual(WaitForMetersInitial, egmAward->GetAwardState(), L"GetAwardState");
			}
			else
			{
				Assert::AreEqual(mqProgressive, target, L"target");
				Assert::IsNotNull(targetMsg, L"targetMsg");
				Assert::AreEqual(INT_MSG_EGM_AWARD, targetMsg->GetMessageType(), L"GetMessageType");
				CEGMAward *egmAward(targetMsg->GetEGMAward());

				Assert::IsNotNull(egmAward, L"egmAward");
				Assert::AreEqual((int)1, egmAward->GetLevelId(), L"GetLevelId");
				Assert::AreEqual((__int64)2, egmAward->GetJackpotId(), L"GetJackpotId");
				Assert::AreEqual((__int64)444, egmAward->GetAwardAmount(), L"GetAwardAmount");
				Assert::AreEqual(methodOfPayment, egmAward->GetMethodOfPayment(), L"GetMethodOfPayment");
				Assert::AreEqual(AwardToGameIneligible, egmAward->GetAwardState(), L"GetAwardState");
				Assert::AreNotEqual(CString(_T("")), egmAward->GetStatusInfo(), L"GetStatusInfo");

			}

			Assert::AreEqual((size_t)0, returnMessages->size(), L"returnMessages->size");

			// Cleanup.
			delete targetMsg;

			for (auto awardIterator = returnMessages->cbegin(); awardIterator != returnMessages->cend();/*no need to in iterator*/)
			{
				if (nullptr != (*awardIterator)->msg)
				{
					delete (*awardIterator)->msg; //Delete the message that was created
				}
				awardIterator = returnMessages->erase(awardIterator);	//remove the item from the vector and repair the iterator
			}
			delete returnMessages;

		}

		/// <summary>
		/// Functional test for:
		///     CEGMAwardManager::ProcessMessage()
		/// Test that an award received from One Link with MethodOfPayment::PayToGame is accepted.
		/// </summary>
		TEST_METHOD(ProcessMessage_ReceivedFromSystem_PayToGame)
		{
			ProcessMessage_ReceivedFromSystemTest(MethodOfPayment::PayToGame, MethodOfPayment::pPayToGame);
		}

		/// <summary>
		/// Functional test for:
		///     CEGMAwardManager::ProcessMessage()
		/// Test that an award received from One Link with MethodOfPayment::PayToGameWithHandPay is accepted.
		/// </summary>
		TEST_METHOD(ProcessMessage_ReceivedFromSystem_PayToGameWithHandPay)
		{
			ProcessMessage_ReceivedFromSystemTest(MethodOfPayment::PayToGameWithHandPay, MethodOfPayment::pPayToGameWithHandPay);
		}

		/// <summary>
		/// Functional test for:
		///     CEGMAwardManager::ProcessMessage()
		/// Test that an award received from One Link with MethodOfPayment::Invalid is accepted.
		/// </summary>
		TEST_METHOD(ProcessMessage_ReceivedFromSystem_Invalid)
		{
			ProcessMessage_ReceivedFromSystemTest(MethodOfPayment::Invalid, "");
		}

		/// <summary>
		/// Code coverage and functional test for:
		/// EGMAwardManager::ProcessProgressiveWinHit
		/// </summary>
		TEST_METHOD(ProcessProgressiveWinHit_Has_Unsent)
		{
			//Arrange
			const ProgressiveLevelManager progressiveLevelManager;
			CEGMAwardManager egmAwardManager(progressiveLevelManager, MEMORY_NONE, false);
			ProgressiveWinsMock mock;
			
			ON_CALL(mock, HasUnsentProgressiveWins()).WillByDefault(Return(true)); //has Unsent should return true to say its waiting
			EXPECT_CALL(mock, HasUnsentProgressiveWins()).Times(1);		//Make sure HasUnsent was called once

			shared_ptr<IStandardProgressiveWins> mockShared(&mock,
				[](IStandardProgressiveWins* ot)
			{
				//do not remove this.
				//this is here because the gmock interfaces will not take a pointer
				//so mock is referenced. This shared_ptr tries to delete it on exit of unit
				//test. This custom deleter does not delete 
			}
			);
			egmAwardManager.SetProgressiveWins(mockShared);

			CGames m_games;
			CProgressiveCriticalText m_progressiveCriticalText;
			MessageQueueId source(mqGCF);

			CInternalMessage *msg = new CInternalMessage(INT_MSG_PROGRESSIVE_WIN_HIT);

			CInternalMessage* targetMsg(NULL);
			CInternalMessage* targetSecondMsg(NULL);
			MessageQueueId target(mqNone);
			MessageQueueId targetSecond(mqNone);
			SYSTEMTIME systemTime;
			GetSystemTime(&systemTime);
			std::vector<MsgTargetPair*> *returnMessages = new std::vector<MsgTargetPair*>();

			//Act
			egmAwardManager.ProcessMessage(GetTickCount(),
				systemTime,
				m_games,
				m_progressiveCriticalText.IsDown(),
				msg,
				source,
				targetMsg,
				target,
				returnMessages);

			//Assert
			Assert::IsTrue(NULL == targetMsg);								//make sure nothing was returned
			Assert::IsTrue(mqNone == target);								//make sure not trying to send to anything
			Assert::IsTrue(Mock::VerifyAndClearExpectations(&mock));		//Required for GMOCK to check calls
		}
		
		/// <summary>
		/// Code coverage and functional test for:
		/// EGMAwardManager::ProcessProgressiveWinHit
		/// </summary>
		TEST_METHOD(ProcessProgressiveWinHit_Has_No_Unsent)
		{
			//Arrange
			const ProgressiveLevelManager progressiveLevelManager;
			CEGMAwardManager egmAwardManager(progressiveLevelManager, MEMORY_NONE, false);
			ProgressiveWinsMock mock;

			ON_CALL(mock, HasUnsentProgressiveWins()).WillByDefault(Return(false));			//has Unsent should return false to say nothing is pending
			EXPECT_CALL(mock, HasUnsentProgressiveWins()).Times(1);								//Make sure HasUnsent was called once

			shared_ptr<IStandardProgressiveWins> mockShared(&mock,
				[](IStandardProgressiveWins* ot)
				{
					//do not remove this.
					//this is here because the gmock interfaces will not take a pointer
					//so mock is referenced. This shared_ptr tries to delete it on exit of unit
					//test. This custom deleter does not delete 
				}
			);
			egmAwardManager.SetProgressiveWins(mockShared);

			CGames m_games;
			CProgressiveCriticalText m_progressiveCriticalText;
			MessageQueueId source(mqGCF);

			CInternalMessage *msg = new CInternalMessage(INT_MSG_PROGRESSIVE_WIN_HIT);

			CInternalMessage* targetMsg(NULL);
			CInternalMessage* targetSecondMsg(NULL);
			MessageQueueId target(mqNone);
			MessageQueueId targetSecond(mqNone);
			SYSTEMTIME systemTime;
			GetSystemTime(&systemTime);

			std::vector<MsgTargetPair*> *returnMessages = new std::vector<MsgTargetPair*>();

			//Act
			egmAwardManager.ProcessMessage(GetTickCount(),
				systemTime,
				m_games,
				m_progressiveCriticalText.IsDown(),
				msg,
				source,
				targetMsg,
				target,
				returnMessages);

			//Assert
			Assert::AreEqual((int)returnMessages->size(), 1);
			for (auto awardIterator = returnMessages->cbegin(); awardIterator != returnMessages->cend();/*no need to in iterator*/)
			{
				Assert::IsTrue(NULL != (*awardIterator)->msg);													//Target msg should be filled out
				Assert::IsTrue(INT_MSG_GET_PROGRESSIVE_WIN_INFO == (*awardIterator)->msg->GetMessageType());	//target msg should be a win info msg
				Assert::IsTrue(mqGCF == (*awardIterator)->target);													//This msg should be on its way to gcf
				Assert::IsTrue(Mock::VerifyAndClearExpectations(&mock));							//Required for GMOCK to check calls

				if (nullptr != (*awardIterator)->msg)
				{
					delete (*awardIterator)->msg; //Delete the message that was created
				}
				awardIterator = returnMessages->erase(awardIterator);	//remove the item from the vector and repair the iterator
			}
			delete returnMessages;
		}

		/// <summary>
		/// Code coverage and functional test for:
		/// EGMAwardManager::ProcessProgressiveWinHit
		/// </summary>
		TEST_METHOD(ProcessProgressiveWinHitPaidToEGM)
		{
			//Arrange
			const ProgressiveLevelManager progressiveLevelManager;
			CEGMAwardManager egmAwardManager(progressiveLevelManager, MEMORY_NONE, false);
			ProgressiveWinsMock mock;

			shared_ptr<IStandardProgressiveWins> mockShared(&mock,
				[](IStandardProgressiveWins* ot)
			{
				//do not remove this.
				//this is here because the gmock interfaces will not take a pointer
				//so mock is referenced. This shared_ptr tries to delete it on exit of unit
				//test. This custom deleter does not delete 
			}
			);
			egmAwardManager.SetProgressiveWins(mockShared);

			CGames m_games;
			CProgressiveCriticalText m_progressiveCriticalText;
			MessageQueueId source(mqGCF);

			CInternalMessage *msg = new CInternalMessage(INT_MSG_PROGRESSIVE_HOPPER_PAID_HIT);

			CInternalMessage* targetMsg(NULL);
			CInternalMessage* targetSecondMsg(NULL);
			MessageQueueId target(mqNone);
			MessageQueueId targetSecond(mqNone);
			SYSTEMTIME systemTime;
			GetSystemTime(&systemTime);

			std::vector<MsgTargetPair*> *returnMessages = new std::vector<MsgTargetPair*>();

			//Act
			egmAwardManager.ProcessMessage(GetTickCount(),
				systemTime,
				m_games,
				m_progressiveCriticalText.IsDown(),
				msg,
				source,
				targetMsg,
				target,
				returnMessages);

			//Assert
			Assert::AreEqual((int)returnMessages->size(), 1);
			for (auto awardIterator = returnMessages->cbegin(); awardIterator != returnMessages->cend();/*no need to in iterator*/)
			{
				Assert::IsTrue(NULL != (*awardIterator)->msg);													//Target msg should be filled out
				Assert::IsTrue(INT_MSG_GET_PROGRESSIVE_HOPPER_PAID_INFO == (*awardIterator)->msg->GetMessageType());	//target msg should be a win info msg
				Assert::IsTrue(mqGCF == (*awardIterator)->target);													//This msg should be on its way to gcf
				Assert::IsTrue(Mock::VerifyAndClearExpectations(&mock));							//Required for GMOCK to check calls

				if (nullptr != (*awardIterator)->msg)
				{
					delete (*awardIterator)->msg; //Delete the message that was created
				}
				awardIterator = returnMessages->erase(awardIterator);	//remove the item from the vector and repair the iterator
			}
			delete returnMessages;
		}

#if 0 // Depends on egmAwardManager.m_progressiveLevelManager initialization.
		/// <summary>
		/// Code coverage and functional test for:
		/// EGMAwardManager::ProcessProgressiveWin
		/// EGMAwardManager::ProcessMessage
		/// </summary>
		TEST_METHOD(ProcessProgressiveWin)
		{
			//Arrange
			const ProgressiveLevelManager progressiveLevelManager;
			CEGMAwardManager egmAwardManager(progressiveLevelManager, MEMORY_NONE, false);
			ProgressiveWinsMock mock;	
			const SYSTEMTIME receivedTime = { 2001, 2, 0, 3, 12, 34, 56, 789 };
			const DWORD receivedTickCount = 100;
			_ICardConfig5 cardConfig;
			rapidjson::Document jsonDoc = cardConfig.CreateJsonDoc();
			vector<string> urls;
			CProgressiveLevels *levels = new CProgressiveLevels(receivedTickCount, jsonDoc, &urls, &receivedTime);
			CGames m_games;
			CProgressiveCriticalText m_progressiveCriticalText;

			
			//___________________Setup the levels for standard progressives_____________________________
			SentinelNativeUnitTests::ProgressiveLevelTestHelpers::LevelData data[] =
			{
				//TickCount, id, type, pName, pUrl, numSasLevels, sasLevels[3], expectedNumSasLevels;

				{ 123, 1000, ProgressiveLevelType::Standard, "name0", "url0", 2, { 2, 5 }, 2 },
				{ 456, 1001, ProgressiveLevelType::Bonus, "", "url1", 0, {}, 0 },
				{ 789, 1002, ProgressiveLevelType::Standard, nullptr, "url2", 1, { 31 }, 1 },
				{ 951, 1003, ProgressiveLevelType::AccountingOnly, "ac", "url3", 1, { 1 }, 1 }
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				levels->AddLevel(data[i].updatedTickCount, data[i].id, data[i].type, data[i].pName, data[i].pUrl);

				for (size_t s = 0; s < data[i].numSasLevels; s++)
				{
					CProgressiveLevel* pLevel = levels->FindLevel(data[i].id);
					Assert::IsNotNull(pLevel, AssertMsg::Format(__LINE__, L"pLevel", L"data[%d].m_id", i));
					pLevel->AddSasLevel(data[i].sasLevels[s]);
				}
			}
			CInternalMessage *msgLevelConfig = new CInternalMessage(INT_MSG_PROGRESSIVE_LEVELS_INFO, levels);

			CInternalMessage* targetMsg1(NULL);
			CInternalMessage* targetSecondMsg1(NULL);
			MessageQueueId target1(mqNone);
			MessageQueueId targetSecond1(mqNone);
			SYSTEMTIME systemTime1;
			GetSystemTime(&systemTime1);
			MessageQueueId source1(mqProgressive);

			std::vector<MsgTargetPair*> *returnMessages = new std::vector<MsgTargetPair*>();

			//Act
			egmAwardManager.ProcessMessage(GetTickCount(),
				systemTime1,
				m_games,
				m_progressiveCriticalText.IsDown(),
				msgLevelConfig,
				source1,
				targetMsg1,
				target1,
				returnMessages); //send levels to egmawardmanager, it will maintain state
			//__________________________________________________________________________________________


			shared_ptr<IStandardProgressiveWins> mockShared(&mock,
				[](IStandardProgressiveWins* ot)
			{
				//do not remove this.
				//this is here because the gmock interfaces will not take a pointer
				//so mock is referenced. This shared_ptr tries to delete it on exit of unit
				//test. This custom deleter does not delete 
			}
			);
			egmAwardManager.SetProgressiveWins(mockShared);	//Setup the mock progwins list

			//___________________Setup the Award as if it came from SAS_________________________________
			CEGMAward *award = new CEGMAward();
			award->SetAwardAmountHit(12398); //123.98
			award->SetSASLevelId(31);
			CInternalMessage *msgProgWin = new CInternalMessage(INT_MSG_PROGRESSIVE_WIN, award);

			EXPECT_CALL(mock, SetProgressiveWin(_, _, _)).Times(1); //Make sure this is called

			CInternalMessage* targetMsg2(NULL);
			CInternalMessage* targetSecondMsg2(NULL);
			MessageQueueId target2(mqNone);
			MessageQueueId targetSecond2(mqNone);
			SYSTEMTIME systemTime2;
			GetSystemTime(&systemTime2);
			MessageQueueId source2(mqGCF);

			egmAwardManager.ProcessMessage(GetTickCount(),
				systemTime2,
				m_games,
				m_progressiveCriticalText.IsDown(),
				msgProgWin,
				source2,
				targetMsg2,
				target2,
				returnMessages); //Send up the egm award which inturn should call SetProgressiveWin
			//__________________________________________________________________________________________

			//Assert
			Assert::IsTrue(Mock::VerifyAndClearExpectations(&mock));//Required for GMOCK to check calls
			
			delete msgProgWin;
			delete msgLevelConfig;
			delete returnMessages;
		}
#endif

		/// <summary>
		/// Code coverage and functional test for:
		/// EGMAwardManager::ProcessProgressiveWin
		/// </summary>
		TEST_METHOD(ProcessProgressiveWin_No_Award)
		{
			//Arrange
			const ProgressiveLevelManager progressiveLevelManager;
			CEGMAwardManager egmAwardManager(progressiveLevelManager, MEMORY_NONE, false);
			ProgressiveWinsMock mock;

			shared_ptr<IStandardProgressiveWins> mockShared(&mock,
				[](IStandardProgressiveWins* ot)
			{
				//do not remove this.
				//this is here because the gmock interfaces will not take a pointer
				//so mock is referenced. This shared_ptr tries to delete it on exit of unit
				//test. This custom deleter does not delete 
			}
			);
			egmAwardManager.SetProgressiveWins(mockShared);

			CGames m_games;
			CProgressiveCriticalText m_progressiveCriticalText;
			MessageQueueId source(mqGCF);
			
			CInternalMessage *msg = new CInternalMessage(INT_MSG_PROGRESSIVE_WIN);

			CInternalMessage* targetMsg(NULL);
			CInternalMessage* targetSecondMsg(NULL);
			MessageQueueId target(mqNone);
			MessageQueueId targetSecond(mqNone);
			SYSTEMTIME systemTime;
			GetSystemTime(&systemTime);
			
			std::vector<MsgTargetPair*> *returnMessages = new std::vector<MsgTargetPair*>();

			//Act
			egmAwardManager.ProcessMessage(GetTickCount(),
				systemTime,
				m_games,
				m_progressiveCriticalText.IsDown(),
				msg,
				source,
				targetMsg,
				target,
				returnMessages);

			//Assert
			Assert::IsTrue(NULL == targetMsg);										//Target msg should be filled out
			Assert::IsTrue(mqNone == target);										//This msg should be on its way to prog protocol
			Assert::IsTrue(Mock::VerifyAndClearExpectations(&mock));				//Required for GMOCK to check calls
		}

		/// <summary>
		/// Code coverage and functional test for:
		/// EGMAwardManager::ProcessProgressiveWin
		/// </summary>
		TEST_METHOD(ProcessProgressiveWin_No_SAS_Level_Found)
		{
			//Arrange
			const ProgressiveLevelManager progressiveLevelManager;
			CEGMAwardManager egmAwardManager(progressiveLevelManager, MEMORY_NONE, false);
			ProgressiveWinsMock mock;
			const SYSTEMTIME receivedTime = { 2001, 2, 0, 3, 12, 34, 56, 789 };
			const DWORD receivedTickCount = 100;

			EXPECT_CALL(mock, SetProgressiveWin(_,_,_)).Times(0);

			shared_ptr<IStandardProgressiveWins> mockShared(&mock,
				[](IStandardProgressiveWins* ot)
			{
				//do not remove this.
				//this is here because the gmock interfaces will not take a pointer
				//so mock is referenced. This shared_ptr tries to delete it on exit of unit
				//test. This custom deleter does not delete 
			}
			);
			egmAwardManager.SetProgressiveWins(mockShared);

			CGames m_games;
			CProgressiveCriticalText m_progressiveCriticalText;

			CEGMAward *award = new CEGMAward();
			award->SetAwardAmountHit(12398); //123.98
			award->SetSASLevelId(31);

			CInternalMessage *msgProgWin = new CInternalMessage(INT_MSG_PROGRESSIVE_WIN, award);
			EXPECT_CALL(mock, SetProgressiveWin(_, _, _)).Times(0);

			CInternalMessage* targetMsg2(NULL);
			CInternalMessage* targetSecondMsg2(NULL);
			MessageQueueId target2(mqNone);
			MessageQueueId targetSecond2(mqNone);
			SYSTEMTIME systemTime2;
			GetSystemTime(&systemTime2);
			MessageQueueId source2(mqGCF);

			std::vector<MsgTargetPair*> *returnMessages = new std::vector<MsgTargetPair*>();

			egmAwardManager.ProcessMessage(GetTickCount(),
				systemTime2,
				m_games,
				m_progressiveCriticalText.IsDown(),
				msgProgWin,
				source2,
				targetMsg2,
				target2,
				returnMessages);

			//Assert
			Assert::IsTrue(Mock::VerifyAndClearExpectations(&mock));						//Required for GMOCK to check calls
		}

		/// <summary>
		/// Code coverage and functional test for:
		/// EGMAwardManager::ProcessEGMAwardFromProgressive
		/// </summary>
		TEST_METHOD(EGMAwardManager_ProcessEGMAwardFromProgressive_IsNOTStandard)
		{
			//Arrange
			const ProgressiveLevelManager progressiveLevelManager;
			CEGMAwardManager egmAwardManager(progressiveLevelManager, MEMORY_NONE, false);
			ProgressiveWinsMock mock;

			EXPECT_CALL(mock, ProcessEGMAwardStandardFromProgressive(_, _, _, _, _)).Times(0);

			shared_ptr<IStandardProgressiveWins> mockShared(&mock,
				[](IStandardProgressiveWins* ot)
			{
				//do not remove this.
				//this is here because the gmock interfaces will not take a pointer
				//so mock is referenced. This shared_ptr tries to delete it on exit of unit
				//test. This custom deleter does not delete 
			}
			);
			egmAwardManager.SetProgressiveWins(mockShared);

			CGames m_games;
			CProgressiveCriticalText m_progressiveCriticalText;
			MessageQueueId source(mqProgressive);

			CEGMAward *award = new CEGMAward();
			award->SetAwardAmountHit(12398); //123.98
			award->SetLevelId(9);

			CInternalMessage *msg = new CInternalMessage(INT_MSG_EGM_AWARD, award);

			CInternalMessage* targetMsg(NULL);
			CInternalMessage* targetSecondMsg(NULL);
			MessageQueueId target(mqNone);
			MessageQueueId targetSecond(mqNone);
			SYSTEMTIME systemTime;
			GetSystemTime(&systemTime);

			std::vector<MsgTargetPair*> *returnMessages = new std::vector<MsgTargetPair*>();

			//Act
			egmAwardManager.ProcessMessage(GetTickCount(),
				systemTime,
				m_games,
				m_progressiveCriticalText.IsDown(),
				msg,
				source,
				targetMsg,
				target,
				returnMessages);

			//Assert
			Assert::IsTrue(Mock::VerifyAndClearExpectations(&mock));				//Required for GMOCK to check calls
		}

		/// <summary>
		/// Code coverage and functional test for:
		/// EGMAwardManager::ProcessHandPayReset
		/// </summary>
		TEST_METHOD(ProcessHandPayReset)
		{
			//Arrange
			const ProgressiveLevelManager progressiveLevelManager;
			CEGMAwardManager egmAwardManager(progressiveLevelManager, MEMORY_NONE, false);
			ProgressiveWinsMock mock;

			EXPECT_CALL(mock, SetHandPayReset(_,_)).Times(1);

			shared_ptr<IStandardProgressiveWins> mockShared(&mock,
				[](IStandardProgressiveWins* ot)
			{
				//do not remove this.
				//this is here because the gmock interfaces will not take a pointer
				//so mock is referenced. This shared_ptr tries to delete it on exit of unit
				//test. This custom deleter does not delete 
			}
			);
			egmAwardManager.SetProgressiveWins(mockShared);

			CGames m_games;
			CProgressiveCriticalText m_progressiveCriticalText;
			MessageQueueId source(mqGCF);

			CInternalMessage *msg = new CInternalMessage(INT_MSG_HANDPAY_RESET);

			CInternalMessage* targetMsg(NULL);
			CInternalMessage* targetSecondMsg(NULL);
			MessageQueueId target(mqNone);
			MessageQueueId targetSecond(mqNone);
			SYSTEMTIME systemTime;
			GetSystemTime(&systemTime);

			std::vector<MsgTargetPair*> *returnMessages = new std::vector<MsgTargetPair*>();

			//Act
			egmAwardManager.ProcessMessage(GetTickCount(),
				systemTime,
				m_games,
				m_progressiveCriticalText.IsDown(),
				msg,
				source,
				targetMsg,
				target,
				returnMessages);

			//Assert
			Assert::IsTrue(Mock::VerifyAndClearExpectations(&mock));							//Required for GMOCK to check calls
		}

		/// <summary>
		/// Code coverage and functional test for:
		/// EGMAwardManager::ProcessCumulativeProgressiveWinFromGCF
		/// </summary>
		TEST_METHOD(ProcessCumulativeProgressiveWinFromGCF_NO_Delta)
		{
			//Arrange
			const ProgressiveLevelManager progressiveLevelManager;
			CEGMAwardManager egmAwardManager(progressiveLevelManager, MEMORY_NONE, false);
			CumulativeProgressiveManagerMock mock;

			EXPECT_CALL(mock, GetCumulativeDelta(_)).Times(1);

			shared_ptr<ICumulativeProgressiveManager> mockShared(&mock,
				[](ICumulativeProgressiveManager* ot)
			{
				//do not remove this.
				//this is here because the gmock interfaces will not take a pointer
				//so mock is referenced. This shared_ptr tries to delete it on exit of unit
				//test. This custom deleter does not delete 
			}
			);
			egmAwardManager.SetCumulativeProgressiveManager(mockShared);

			CGames m_games;
			CProgressiveCriticalText m_progressiveCriticalText;
			MessageQueueId source(mqGCF);

			CInternalMessage *msg = new CInternalMessage(INT_MSG_CUMULATIVE_PROGRESSIVE_WIN);

			CInternalMessage* targetMsg(NULL);
			CInternalMessage* targetSecondMsg(NULL);
			MessageQueueId target(mqNone);
			MessageQueueId targetSecond(mqNone);
			SYSTEMTIME systemTime;
			GetSystemTime(&systemTime);

			std::vector<MsgTargetPair*> *returnMessages = new std::vector<MsgTargetPair*>();

			//Act
			egmAwardManager.ProcessMessage(GetTickCount(),
				systemTime,
				m_games,
				m_progressiveCriticalText.IsDown(),
				msg,
				source,
				targetMsg,
				target,
				returnMessages);

			//Assert
			Assert::IsTrue(Mock::VerifyAndClearExpectations(&mock));							//Required for GMOCK to check calls
		}

		/// <summary>
		/// Code coverage and functional test for:
		/// EGMAwardManager::ProcessCumulativeProgressiveWinFromGCF
		/// </summary>
		TEST_METHOD(ProcessCumulativeProgressiveWinFromGCF_Delta)
		{
			//Arrange
			const ProgressiveLevelManager progressiveLevelManager;
			CEGMAwardManager egmAwardManager(progressiveLevelManager, MEMORY_NONE, false);
			CumulativeProgressiveManagerMock mock;

			ON_CALL(mock, GetCumulativeDelta(_)).WillByDefault(Return(100));
			EXPECT_CALL(mock, GetCumulativeDelta(_)).Times(1);

			shared_ptr<ICumulativeProgressiveManager> mockShared(&mock,
				[](ICumulativeProgressiveManager* ot)
			{
				//do not remove this.
				//this is here because the gmock interfaces will not take a pointer
				//so mock is referenced. This shared_ptr tries to delete it on exit of unit
				//test. This custom deleter does not delete 
			}
			);
			egmAwardManager.SetCumulativeProgressiveManager(mockShared);

			CGames m_games;
			CProgressiveCriticalText m_progressiveCriticalText;
			MessageQueueId source(mqGCF);

			CInternalMessage *msg = new CInternalMessage(INT_MSG_CUMULATIVE_PROGRESSIVE_WIN);

			CInternalMessage* targetMsg(NULL);
			CInternalMessage* targetSecondMsg(NULL);
			MessageQueueId target(mqNone);
			MessageQueueId targetSecond(mqNone);
			SYSTEMTIME systemTime;
			GetSystemTime(&systemTime);

			std::vector<MsgTargetPair*> *returnMessages = new std::vector<MsgTargetPair*>();

			//Act
			egmAwardManager.ProcessMessage(GetTickCount(),
				systemTime,
				m_games,
				m_progressiveCriticalText.IsDown(),
				msg,
				source,
				targetMsg,
				target,
				returnMessages);

			//Assert
			Assert::AreEqual((int)returnMessages->size(), 1);
			for (auto awardIterator = returnMessages->cbegin(); awardIterator != returnMessages->cend();/*no need to in iterator*/)
			{
				Assert::AreEqual((int)INT_MSG_GET_PROGRESSIVE_HOPPER_PAID_INFO, (int)(*awardIterator)->msg->GetMessageType(), L"targetMsg->GetMessageType()");
				Assert::AreEqual(mqGCF, (*awardIterator)->target);
				Assert::IsTrue(Mock::VerifyAndClearExpectations(&mock));							//Required for GMOCK to check calls

				if (nullptr != (*awardIterator)->msg)
				{
					delete (*awardIterator)->msg; //Delete the message that was created
				}
				awardIterator = returnMessages->erase(awardIterator);	//remove the item from the vector and repair the iterator
			}
			delete returnMessages;
		}

		/// <summary>
		/// Code coverage and functional test for:
		/// EGMAwardManager::ProcessJackpotInfoMessageFromGCF
		/// </summary>
		TEST_METHOD(ProcessJackpotInfoMessageFromGCF_SetHandPay)
		{
			//Arrange
			const ProgressiveLevelManager progressiveLevelManager;
			CEGMAwardManager egmAwardManager(progressiveLevelManager, MEMORY_NONE, false);
			ProgressiveWinsMock mock;
			std::vector<MsgTargetPair*> *returnMessages = new std::vector<MsgTargetPair*>();

			EXPECT_CALL(mock, SetHandPayPending(_,_,_)).Times(1);

			shared_ptr<IStandardProgressiveWins> mockShared(&mock,
				[](IStandardProgressiveWins* ot)
			{
				//do not remove this.
				//this is here because the gmock interfaces will not take a pointer
				//so mock is referenced. This shared_ptr tries to delete it on exit of unit
				//test. This custom deleter does not delete 
			}
			);
			egmAwardManager.SetProgressiveWins(mockShared);

			CGames m_games;
			CProgressiveCriticalText m_progressiveCriticalText;
			MessageQueueId source(mqGCF);

			CJackpot *jackpot = new CJackpot(MEMORY_NONE);
			jackpot->SetJackpotType(JT_NONE);
			jackpot->SetJackpotAmount(500);
			jackpot->SetProgressiveLevel(1);

			CInternalMessage *msg = new CInternalMessage(INT_MSG_JACKPOT_INFO, jackpot);

			CInternalMessage* targetMsg(NULL);
			CInternalMessage* targetSecondMsg(NULL);
			MessageQueueId target(mqNone);
			MessageQueueId targetSecond(mqNone);
			SYSTEMTIME systemTime;
			GetSystemTime(&systemTime);

			//Act
			egmAwardManager.ProcessMessage(GetTickCount(),
				systemTime,
				m_games,
				m_progressiveCriticalText.IsDown(),
				msg,
				source,
				targetMsg,
				target,
				returnMessages);

			//Assert
			Assert::IsTrue(Mock::VerifyAndClearExpectations(&mock));							//Required for GMOCK to check calls
		}

		/// <summary>
		/// Code coverage and functional test for:
		/// EGMAwardManager::ProcessEGMAwardFromProgressive
		/// </summary>
		TEST_METHOD(EGMAwardManager_ProcessEGMAwardFromProgressive_IsStandard)
		{
			//Arrange
			const ProgressiveLevelManager progressiveLevelManager;
			CEGMAwardManager egmAwardManager(progressiveLevelManager, MEMORY_NONE, false);
			ProgressiveWinsMock mock;

			EXPECT_CALL(mock, ProcessEGMAwardStandardFromProgressive(_, _, _, _, _)).Times(1);

			shared_ptr<IStandardProgressiveWins> mockShared(&mock,
				[](IStandardProgressiveWins* ot)
			{
				//do not remove this.
				//this is here because the gmock interfaces will not take a pointer
				//so mock is referenced. This shared_ptr tries to delete it on exit of unit
				//test. This custom deleter does not delete 
			}
			);
			egmAwardManager.SetProgressiveWins(mockShared);

			CGames m_games;
			CProgressiveCriticalText m_progressiveCriticalText;
			MessageQueueId source(mqProgressive);

			CEGMAward *award = new CEGMAward();
			award->SetAwardAmountHit(12398); //123.98
			award->SetLevelId(9);
			award->SetStandardProgressive(true);

			CInternalMessage *msg = new CInternalMessage(INT_MSG_EGM_AWARD, award);

			CInternalMessage* targetMsg(NULL);
			CInternalMessage* targetSecondMsg(NULL);
			MessageQueueId target(mqNone);
			MessageQueueId targetSecond(mqNone);
			SYSTEMTIME systemTime;
			GetSystemTime(&systemTime);

			std::vector<MsgTargetPair*> *returnMessages = new std::vector<MsgTargetPair*>();

			//Act
			egmAwardManager.ProcessMessage(GetTickCount(),
				systemTime,
				m_games,
				m_progressiveCriticalText.IsDown(),
				msg,
				source,
				targetMsg,
				target,
				returnMessages);

			//Assert
			Assert::IsTrue(Mock::VerifyAndClearExpectations(&mock));				//Required for GMOCK to check calls
		}

		TEST_METHOD(EgmAwardManagerHandpayGameStart)
		{
			const ProgressiveLevelManager progressiveLevelManager;
			CEGMAwardManager egmAM(progressiveLevelManager, MEMORY_NONE, false);
			CEGMAward *award = new CEGMAward(MEMORY_NONE, false, CMemoryStorage::NVRAM_DEFAULT_OFFSET);
			award->SetAwardAmountHit(120101);
			award->SetAwardState(ReceivedFromSystem);
			award->SetOriginalAttendantBonusingMeterValue(INVALID_METER_VALUE);
			award->SetOriginalMachineBonusMeterValue(INVALID_METER_VALUE);
			award->SetAwardTransferType(EGMAwardTransferType_None);
			award->SetJackpotId(1);
			award->SetMethodOfPayment(MethodOfPayment::PayToGame);
			CInternalMessage *msg = new CInternalMessage(INT_MSG_EGM_AWARD, award);
			CInternalMessage* targetMsg(NULL);
			MessageQueueId target(mqNone);
			CInternalMessage* targetSecondMsg(NULL);
			MessageQueueId targetSecond(mqNone);

			CGames *games = new CGames(MEMORY_NONE);
			games->SetAftBonusEnabled(true);
			games->SetAccountingDenom(1);
			
			std::vector<MsgTargetPair*> *returnMessages = new std::vector<MsgTargetPair*>();

			SYSTEMTIME systemTime;
			GetSystemTime(&systemTime);
			egmAM.ProcessMessage(GetTickCount(), systemTime, *games,
				false,
				msg,
				mqProgressive,
				targetMsg,
				target, returnMessages);

			Assert::IsTrue(target == mqGCF);
			delete targetMsg;
			delete msg;

			// Set original meters
			award = egmAM.GetEGMAwardCopy();
			award->SetOriginalAttendantBonusingMeterValue(1);
			award->SetOriginalMachineBonusMeterValue(1);
			msg = new CInternalMessage(INT_MSG_EGM_AWARD, award);
			egmAM.ProcessMessage(GetTickCount(), systemTime, *games,
				false,
				msg,
				mqGCF,
				targetMsg,
				target, returnMessages);
			delete targetMsg;
			delete msg;

			// Set handpay
			CJackpot *jackpot = new CJackpot(MEMORY_NONE);
			jackpot->SetJackpotType(JT_HANDPAY);
			jackpot->SetJackpotAmount(120101);
			msg = new CInternalMessage(INT_MSG_JACKPOT_INFO, jackpot);
			egmAM.ProcessMessage(GetTickCount(), systemTime, *games,
				false,
				msg,
				mqGCF,
				targetMsg,
				target, returnMessages);

			delete targetMsg;
			delete msg;

			// Game Start
			msg = new CInternalMessage(INT_MSG_NOTIFY_GAME_START);
			egmAM.ProcessMessage(GetTickCount(), systemTime, *games,
				false,
				msg,
				mqGCF,
				targetMsg,
				target, returnMessages);

			// Look at targetMsg
			Assert::IsTrue(target == mqProgressive);
			award = targetMsg->GetEGMAward();

			Assert::IsTrue(award != NULL);
			Assert::IsTrue(award->GetAwardAmountPaid() == 120101);
			Assert::IsTrue(award->GetAwardState() == AwardToGameSuccess);

			delete msg;
			delete targetMsg;
			delete games;
		}

		TEST_METHOD(EgmAwardManagerNonHandpayGameStart)
		{
			const ProgressiveLevelManager progressiveLevelManager;
			CEGMAwardManager egmAM(progressiveLevelManager, MEMORY_NONE, false);
			CEGMAward *award = new CEGMAward(MEMORY_NONE, false, CMemoryStorage::NVRAM_DEFAULT_OFFSET);
			award->SetAwardAmountHit(120101);
			award->SetAwardState(ReceivedFromSystem);
			award->SetOriginalAttendantBonusingMeterValue(INVALID_METER_VALUE);
			award->SetOriginalMachineBonusMeterValue(INVALID_METER_VALUE);
			award->SetAwardTransferType(EGMAwardTransferType_None);
			award->SetJackpotId(1);
			award->SetMethodOfPayment(MethodOfPayment::PayToGame);
			CInternalMessage *msg = new CInternalMessage(INT_MSG_EGM_AWARD, award);
			CInternalMessage* targetMsg(NULL);
			CInternalMessage* targetSecondMsg(NULL);
			MessageQueueId target(mqNone);
			MessageQueueId targetSecond(mqNone);
			CGames *games = new CGames(MEMORY_NONE);
			games->SetAftBonusEnabled(true);
			games->SetAccountingDenom(1);

			std::vector<MsgTargetPair*> *returnMessages = new std::vector<MsgTargetPair*>();

			SYSTEMTIME systemTime;
			GetSystemTime(&systemTime);
			egmAM.ProcessMessage(GetTickCount(), systemTime, *games,
				false,
				msg,
				mqProgressive,
				targetMsg,
				target,
				returnMessages);

			Assert::IsTrue(target == mqGCF, L"Target should be GCF");
			delete targetMsg;
			delete msg;

			// Set original meters
			award = egmAM.GetEGMAwardCopy();
			award->SetOriginalAttendantBonusingMeterValue(1);
			award->SetOriginalMachineBonusMeterValue(1);
			msg = new CInternalMessage(INT_MSG_EGM_AWARD, award);
			egmAM.ProcessMessage(GetTickCount(), systemTime, *games,
				false,
				msg,
				mqGCF,
				targetMsg,
				target,
				returnMessages);
			delete targetMsg;
			delete msg;

			// Game Start
			// Since not currently in handpay, this game start doesn't result in a AwardToGameSuccess state.
			msg = new CInternalMessage(INT_MSG_NOTIFY_GAME_START);
			egmAM.ProcessMessage(GetTickCount(), systemTime, *games,
				false,
				msg,
				mqGCF,
				targetMsg,
				target,
				returnMessages);

			// Look at targetMsg
			Assert::IsTrue(target == mqNone, L"Target should be none");

			Assert::IsTrue(targetMsg == NULL, L"Target message should not be NONE");

			delete msg;
			delete games;
		}

		TEST_METHOD(EgmAwardManagerGameCommDownExpire)
		{
			const ProgressiveLevelManager progressiveLevelManager;
			CEGMAwardManager egmAM(progressiveLevelManager, MEMORY_NONE, false);
			CEGMAward *award = new CEGMAward(MEMORY_NONE, false, CMemoryStorage::NVRAM_DEFAULT_OFFSET);
			award->SetAwardAmountHit(120101);
			award->SetAwardState(ReceivedFromSystem);
			award->SetOriginalAttendantBonusingMeterValue(INVALID_METER_VALUE);
			award->SetOriginalMachineBonusMeterValue(INVALID_METER_VALUE);
			award->SetAwardTransferType(EGMAwardTransferType_None);
			award->SetJackpotId(1);
			award->SetMethodOfPayment(MethodOfPayment::PayToGame);
			CInternalMessage *msg = new CInternalMessage(INT_MSG_EGM_AWARD, award);
			CInternalMessage* targetMsg(NULL);
			CInternalMessage* targetSecondMsg(NULL);
			MessageQueueId target(mqNone);
			MessageQueueId targetSecond(mqNone);
			CGames *games = new CGames(MEMORY_NONE);
			games->SetAftBonusEnabled(true);
			games->SetAccountingDenom(1);

			std::vector<MsgTargetPair*> *returnMessages = new std::vector<MsgTargetPair*>();

			SYSTEMTIME systemTime;
			GetSystemTime(&systemTime);
			egmAM.ProcessMessage(GetTickCount(), systemTime, *games,
				false,
				msg,
				mqProgressive,
				targetMsg,
				target,
				returnMessages,
				false);

			delete targetMsg;
			delete msg;

			// With game comm down, the award should expire after 20 seconds.
			Sleep(21000);

			target = mqNone;

			GetSystemTime(&systemTime);
			egmAM.ProcessMessage(GetTickCount(), systemTime, *games,
				false,
				NULL,
				mqNone,
				targetMsg,
				target,
				returnMessages,
				false);

			Assert::IsTrue(target == mqGCF, L"Target after expire should be GCF");

			Assert::IsFalse(targetMsg == NULL, L"Should have a target message after expire");

			if (targetMsg != NULL)
			{
				award = targetMsg->GetEGMAward();
				Assert::IsFalse(award == NULL, L"Should have a award after expire");
				if (award != NULL)
				{
					Assert::IsTrue(award->GetAwardState() == AwardToGameIneligible, L"award state should be ineligible after expire");
				}
				delete targetMsg;
			}


			award = new CEGMAward(MEMORY_NONE, false, CMemoryStorage::NVRAM_DEFAULT_OFFSET);
			award->SetAwardAmountHit(120101);
			award->SetAwardState(AwardToGameIneligible);
			award->SetOriginalAttendantBonusingMeterValue(INVALID_METER_VALUE);
			award->SetOriginalMachineBonusMeterValue(INVALID_METER_VALUE);
			award->SetAwardTransferType(EGMAwardTransferType_None);
			award->SetJackpotId(1);
			msg = new CInternalMessage(INT_MSG_EGM_AWARD, award);
			targetMsg = NULL;
			target = mqNone;
			games = new CGames(MEMORY_NONE);
			games->SetAftBonusEnabled(true);
			games->SetAccountingDenom(1);

			GetSystemTime(&systemTime);
			egmAM.ProcessMessage(GetTickCount(), systemTime, *games,
				false,
				msg,
				mqGCF,
				targetMsg,
				target,
				returnMessages,
				false);

			Assert::IsTrue(target == mqProgressive, L"Target after GCF sends in ineligible should be progressive");

			Assert::IsFalse(targetMsg == NULL, L"Should have a target message after GCF sends in ineligible");

			if (targetMsg != NULL)
			{
				award = targetMsg->GetEGMAward();
				Assert::IsFalse(award == NULL, L"Should have a award after expire");
				if (award != NULL)
				{
					Assert::IsTrue(award->GetAwardState() == AwardToGameIneligible, L"award state should be ineligible after GCF sends in ineligible");
				}
				delete targetMsg;
			}

			delete msg;

		}

#if 0 // Test functionality that should be in ProgressiveLevelManagerTests and/or ProgressiveLevelManagerTests2 .
// #pragma region CEGMAwardManager_ProcessLevelBroadcast_Return
		/// <summary>
		/// Functional test for:
		///     CProgressiveLevels::CProgressiveLevels()
		///     CProgressiveLevels::ProcessLevelBroadcast()
		/// Test the return value(s) from ProcessLevelBroadcast().
		/// Test setting Standard levels where both have greater values than the previous values.
		/// </summary>
		TEST_METHOD(ProcessLevelBroadcast_StandardGreater)
		{
			const bool needForwardExpected1(false);
			const bool needForwardExpected2(true);

			const DWORD tickCountConfig(5000);
			const DWORD tickCountBroadcast1(8000);
			const DWORD tickCountBroadcast2(10000);

			const ProgressiveLevelManager progressiveLevelManager;
			CEGMAwardManager egmAwardManager(progressiveLevelManager);
			CGames m_games;
			const bool progressiveDown(false);

			// Configure egmAwardManager.
			{
				_ICardConfig5 cardConfig;
				rapidjson::Document jsonDoc = cardConfig.CreateJsonDoc();
				CProgressiveLevels *levels(new CProgressiveLevels(tickCountConfig, jsonDoc, nullptr));
				unique_ptr<CInternalMessage> msg(new CInternalMessage(INT_MSG_PROGRESSIVE_LEVELS_INFO, levels));
				const MessageQueueId source(mqProgressive);

				CInternalMessage *targetMsg(nullptr);
				MessageQueueId target(mqNone);
				CInternalMessage *targetSecondMsg(nullptr);
				MessageQueueId targetSecond(mqNone);
				SYSTEMTIME systemTime1;
				GetSystemTime(&systemTime1);

				std::vector<MsgTargetPair*> *returnMessages = new std::vector<MsgTargetPair*>();

				egmAwardManager.ProcessMessage(tickCountConfig,
					systemTime1,
					m_games,
					progressiveDown,
					msg.get(),
					source,
					targetMsg,
					target,
					returnMessages);
			}

			// Perform operation(s) and test(s).
			{
				static const char *pJson =
					"{\"LevelValues\":["
					"{\"LevelId\":5009,\"Value\":12.34,\"Name\":\"\"},"
					"{\"LevelId\":61234,\"Value\":56.78,\"Name\":\"z\"}"
					"]}";
				rapidjson::Document jsonDoc;
				bool result = jsonDoc.Parse(pJson).HasParseError();
				Assert::IsFalse(result, AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError())));

				CProgressiveLevelBroadcast *levelBroadcast(new CProgressiveLevelBroadcast(tickCountBroadcast1, jsonDoc));
				unique_ptr<CInternalMessage> msg(new CInternalMessage(INT_MSG_PROGRESSIVE_LEVEL_BROADCAST, levelBroadcast));
				const MessageQueueId source(mqProgressive);

				CInternalMessage *targetMsg(nullptr);
				MessageQueueId target(mqNone);
				CInternalMessage *targetSecondMsg(nullptr);
				MessageQueueId targetSecond(mqNone);
				SYSTEMTIME systemTime;
				GetSystemTime(&systemTime);

				std::vector<MsgTargetPair*> *returnMessages = new std::vector<MsgTargetPair*>();

				egmAwardManager.ProcessMessage(tickCountConfig,
					systemTime,
					m_games,
					progressiveDown,
					msg.get(),
					source,
					targetMsg,
					target,
					returnMessages);

				Assert::IsNull(targetMsg, L"targetMsg 1");
				Assert::AreEqual(mqNone, target, L"target 1");
				Assert::IsNull(targetSecondMsg, L"targetSecondMsg 1");
				Assert::AreEqual(mqNone, targetSecond, L"targetSecond 1");
			}

			// Set a lesser value.
			{
				static const char *pJson =
					"{\"LevelValues\":["
					"{\"LevelId\":5009,\"Value\":12.35,\"Name\":\"\"},"
					"{\"LevelId\":61234,\"Value\":56.79,\"Name\":\"z\"}"
					"]}";
				rapidjson::Document jsonDoc;
				bool result = jsonDoc.Parse(pJson).HasParseError();
				Assert::IsFalse(result, AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError())));

				CProgressiveLevelBroadcast *levelBroadcast(new CProgressiveLevelBroadcast(tickCountBroadcast2, jsonDoc));
				unique_ptr<CInternalMessage> msg(new CInternalMessage(INT_MSG_PROGRESSIVE_LEVEL_BROADCAST, levelBroadcast));
				const MessageQueueId source(mqProgressive);

				CInternalMessage *targetMsg(nullptr);
				MessageQueueId target(mqNone);
				CInternalMessage *targetSecondMsg(nullptr);
				MessageQueueId targetSecond(mqNone);
				SYSTEMTIME systemTime;
				GetSystemTime(&systemTime);

				std::vector<MsgTargetPair*> *returnMessages = new std::vector<MsgTargetPair*>();

				egmAwardManager.ProcessMessage(tickCountConfig,
					systemTime,
					m_games,
					progressiveDown,
					msg.get(),
					source,
					targetMsg,
					target,
					returnMessages);

				Assert::IsNull(targetMsg, L"targetMsg 2");
				Assert::AreEqual(mqNone, target, L"target 2");
				Assert::IsNull(targetSecondMsg, L"targetSecondMsg 2");
				Assert::AreEqual(mqNone, targetSecond, L"targetSecond 2");
			}
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveLevels::CProgressiveLevels()
		///     CProgressiveLevels::ProcessLevelBroadcast()
		/// Test the return value(s) from ProcessLevelBroadcast().
		/// Test setting Standard levels where the first level has a smaller value than the previous value.
		/// </summary>
		TEST_METHOD(ProcessLevelBroadcast_StandardLesser)
		{
			const bool needForwardExpected1(false);
			const bool needForwardExpected2(true);

			const DWORD tickCountConfig(5000);
			const DWORD tickCountBroadcast1(8000);
			const DWORD tickCountBroadcast2(10000);

			const ProgressiveLevelManager progressiveLevelManager;
			CEGMAwardManager egmAwardManager(progressiveLevelManager);
			CGames m_games;
			const bool progressiveDown(false);

			// Configure egmAwardManager.
			{
				_ICardConfig5 cardConfig;
				rapidjson::Document jsonDoc = cardConfig.CreateJsonDoc();
				CProgressiveLevels *levels(new CProgressiveLevels(tickCountConfig, jsonDoc, nullptr));
				unique_ptr<CInternalMessage> msg(new CInternalMessage(INT_MSG_PROGRESSIVE_LEVELS_INFO, levels));
				const MessageQueueId source(mqProgressive);

				CInternalMessage *targetMsg(nullptr);
				MessageQueueId target(mqNone);
				CInternalMessage *targetSecondMsg(nullptr);
				MessageQueueId targetSecond(mqNone);
				SYSTEMTIME systemTime1;
				GetSystemTime(&systemTime1);

				std::vector<MsgTargetPair*> *returnMessages = new std::vector<MsgTargetPair*>();

				egmAwardManager.ProcessMessage(tickCountConfig,
					systemTime1,
					m_games,
					progressiveDown,
					msg.get(),
					source,
					targetMsg,
					target,
					returnMessages);
			}

			// Perform operation(s) and test(s).
			{
				static const char *pJson =
					"{\"LevelValues\":["
						"{\"LevelId\":5009,\"Value\":12.34,\"Name\":\"\"},"
						"{\"LevelId\":61234,\"Value\":56.78,\"Name\":\"z\"}"
					"]}";
				rapidjson::Document jsonDoc;
				bool result = jsonDoc.Parse(pJson).HasParseError();
				Assert::IsFalse(result, AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError())));

				CProgressiveLevelBroadcast *levelBroadcast(new CProgressiveLevelBroadcast(tickCountBroadcast1, jsonDoc));
				unique_ptr<CInternalMessage> msg(new CInternalMessage(INT_MSG_PROGRESSIVE_LEVEL_BROADCAST, levelBroadcast));
				const MessageQueueId source(mqProgressive);

				CInternalMessage *targetMsg(nullptr);
				MessageQueueId target(mqNone);
				CInternalMessage *targetSecondMsg(nullptr);
				MessageQueueId targetSecond(mqNone);
				SYSTEMTIME systemTime;
				GetSystemTime(&systemTime);

				std::vector<MsgTargetPair*> *returnMessages = new std::vector<MsgTargetPair*>();

				egmAwardManager.ProcessMessage(tickCountConfig,
					systemTime,
					m_games,
					progressiveDown,
					msg.get(),
					source,
					targetMsg,
					target,
					returnMessages);

				Assert::IsNull(targetMsg, L"targetMsg 1");
				Assert::AreEqual(mqNone, target, L"target 1");
				Assert::IsNull(targetSecondMsg, L"targetSecondMsg 1");
				Assert::AreEqual(mqNone, targetSecond, L"targetSecond 1");
			}

			// Set the first level to a lesser value.
			{
				static const char *pJson =
					"{\"LevelValues\":["
						"{\"LevelId\":5009,\"Value\":12.33,\"Name\":\"\"},"
						"{\"LevelId\":61234,\"Value\":56.78,\"Name\":\"z\"}"
					"]}";
				rapidjson::Document jsonDoc;
				bool result = jsonDoc.Parse(pJson).HasParseError();
				Assert::IsFalse(result, AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError())));

				CProgressiveLevelBroadcast *levelBroadcast(new CProgressiveLevelBroadcast(tickCountBroadcast2, jsonDoc));
				unique_ptr<CInternalMessage> msg(new CInternalMessage(INT_MSG_PROGRESSIVE_LEVEL_BROADCAST, levelBroadcast));
				const MessageQueueId source(mqProgressive);

				CInternalMessage *targetMsg(nullptr);
				MessageQueueId target(mqNone);
				CInternalMessage *targetSecondMsg(nullptr);
				MessageQueueId targetSecond(mqNone);
				SYSTEMTIME systemTime;
				GetSystemTime(&systemTime);

				std::vector<MsgTargetPair*> *returnMessages = new std::vector<MsgTargetPair*>();

				egmAwardManager.ProcessMessage(tickCountConfig,
					systemTime,
					m_games,
					progressiveDown,
					msg.get(),
					source,
					targetMsg,
					target,
					returnMessages);

				Assert::IsNotNull(targetMsg, L"targetMsg 2");
				Assert::AreEqual(INT_MSG_SEND_PROGRESSIVE_LEVEL_FORWARD, targetMsg->GetMessageType(), L"targetMsg 2");
				Assert::AreEqual(mqInternals, target, L"target 2");
				Assert::IsNull(targetSecondMsg, L"targetSecondMsg 2");
				Assert::AreEqual(mqNone, targetSecond, L"targetSecond 2");
			}
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveLevels::CProgressiveLevels()
		///     CProgressiveLevels::ProcessLevelBroadcast()
		/// Test the return value(s) from ProcessLevelBroadcast().
		/// Test setting Standard levels where the first level has a smaller value than the previous value
		/// and then set the levels again where the second level has a greater value than the previous value.
		/// </summary>
		TEST_METHOD(ProcessLevelBroadcast_StandardLesserGreater)
		{
			const bool needForwardExpected1(false);
			const bool needForwardExpected2(true);

			const DWORD tickCountConfig(5000);
			const DWORD tickCountBroadcast1(8000);
			const DWORD tickCountBroadcast2(10000);

			const ProgressiveLevelManager progressiveLevelManager;
			CEGMAwardManager egmAwardManager(progressiveLevelManager);
			CGames m_games;
			const bool progressiveDown(false);

			// Configure egmAwardManager.
			{
				_ICardConfig5 cardConfig;
				rapidjson::Document jsonDoc = cardConfig.CreateJsonDoc();
				CProgressiveLevels *levels(new CProgressiveLevels(tickCountConfig, jsonDoc, nullptr));
				unique_ptr<CInternalMessage> msg(new CInternalMessage(INT_MSG_PROGRESSIVE_LEVELS_INFO, levels));
				const MessageQueueId source(mqProgressive);

				CInternalMessage *targetMsg(nullptr);
				MessageQueueId target(mqNone);
				CInternalMessage *targetSecondMsg(nullptr);
				MessageQueueId targetSecond(mqNone);
				SYSTEMTIME systemTime1;
				GetSystemTime(&systemTime1);

				std::vector<MsgTargetPair*> *returnMessages = new std::vector<MsgTargetPair*>();

				egmAwardManager.ProcessMessage(tickCountConfig,
					systemTime1,
					m_games,
					progressiveDown,
					msg.get(),
					source,
					targetMsg,
					target,
					returnMessages);
			}

			// Perform operation(s) and test(s).
			{
				static const char *pJson =
					"{\"LevelValues\":["
					"{\"LevelId\":5009,\"Value\":12.34,\"Name\":\"\"},"
					"{\"LevelId\":61234,\"Value\":56.78,\"Name\":\"z\"}"
					"]}";
				rapidjson::Document jsonDoc;
				bool result = jsonDoc.Parse(pJson).HasParseError();
				Assert::IsFalse(result, AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError())));

				CProgressiveLevelBroadcast *levelBroadcast(new CProgressiveLevelBroadcast(tickCountBroadcast1, jsonDoc));
				unique_ptr<CInternalMessage> msg(new CInternalMessage(INT_MSG_PROGRESSIVE_LEVEL_BROADCAST, levelBroadcast));
				const MessageQueueId source(mqProgressive);

				CInternalMessage *targetMsg(nullptr);
				MessageQueueId target(mqNone);
				CInternalMessage *targetSecondMsg(nullptr);
				MessageQueueId targetSecond(mqNone);
				SYSTEMTIME systemTime;
				GetSystemTime(&systemTime);

				std::vector<MsgTargetPair*> *returnMessages = new std::vector<MsgTargetPair*>();

				egmAwardManager.ProcessMessage(tickCountConfig,
					systemTime,
					m_games,
					progressiveDown,
					msg.get(),
					source,
					targetMsg,
					target,
					returnMessages);

				Assert::IsNull(targetMsg, L"targetMsg 1");
				Assert::AreEqual(mqNone, target, L"target 1");
				Assert::IsNull(targetSecondMsg, L"targetSecondMsg 1");
				Assert::AreEqual(mqNone, targetSecond, L"targetSecond 1");
			}

			// Set the first level to a lesser value.
			{
				static const char *pJson =
					"{\"LevelValues\":["
					"{\"LevelId\":5009,\"Value\":12.33,\"Name\":\"\"},"
					"{\"LevelId\":61234,\"Value\":56.78,\"Name\":\"z\"}"
					"]}";
				rapidjson::Document jsonDoc;
				bool result = jsonDoc.Parse(pJson).HasParseError();
				Assert::IsFalse(result, AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError())));

				CProgressiveLevelBroadcast *levelBroadcast(new CProgressiveLevelBroadcast(tickCountBroadcast2, jsonDoc));
				unique_ptr<CInternalMessage> msg(new CInternalMessage(INT_MSG_PROGRESSIVE_LEVEL_BROADCAST, levelBroadcast));
				const MessageQueueId source(mqProgressive);

				CInternalMessage *targetMsg(nullptr);
				MessageQueueId target(mqNone);
				CInternalMessage *targetSecondMsg(nullptr);
				MessageQueueId targetSecond(mqNone);
				SYSTEMTIME systemTime;
				GetSystemTime(&systemTime);

				std::vector<MsgTargetPair*> *returnMessages = new std::vector<MsgTargetPair*>();

				egmAwardManager.ProcessMessage(tickCountConfig,
					systemTime,
					m_games,
					progressiveDown,
					msg.get(),
					source,
					targetMsg,
					target,
					returnMessages);

				Assert::IsNotNull(targetMsg, L"targetMsg 2");
				Assert::AreEqual(INT_MSG_SEND_PROGRESSIVE_LEVEL_FORWARD, targetMsg->GetMessageType(), L"targetMsg 2");
				Assert::AreEqual(mqInternals, target, L"target 2");
				Assert::IsNull(targetSecondMsg, L"targetSecondMsg 2");
				Assert::AreEqual(mqNone, targetSecond, L"targetSecond 2");
			}

			// Set the second level to a greater value.
			{
				static const char *pJson =
					"{\"LevelValues\":["
					"{\"LevelId\":5009,\"Value\":12.33,\"Name\":\"\"},"
					"{\"LevelId\":61234,\"Value\":56.79,\"Name\":\"z\"}"
					"]}";
				rapidjson::Document jsonDoc;
				bool result = jsonDoc.Parse(pJson).HasParseError();
				Assert::IsFalse(result, AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError())));

				CProgressiveLevelBroadcast *levelBroadcast(new CProgressiveLevelBroadcast(tickCountBroadcast1, jsonDoc));
				unique_ptr<CInternalMessage> msg(new CInternalMessage(INT_MSG_PROGRESSIVE_LEVEL_BROADCAST, levelBroadcast));
				const MessageQueueId source(mqProgressive);

				CInternalMessage *targetMsg(nullptr);
				MessageQueueId target(mqNone);
				CInternalMessage *targetSecondMsg(nullptr);
				MessageQueueId targetSecond(mqNone);
				SYSTEMTIME systemTime;
				GetSystemTime(&systemTime);

				std::vector<MsgTargetPair*> *returnMessages = new std::vector<MsgTargetPair*>();

				egmAwardManager.ProcessMessage(tickCountConfig,
					systemTime,
					m_games,
					progressiveDown,
					msg.get(),
					source,
					targetMsg,
					target,
					returnMessages);

				Assert::IsNull(targetMsg, L"targetMsg 3");
				Assert::AreEqual(mqNone, target, L"target 3");
				Assert::IsNull(targetSecondMsg, L"targetSecondMsg 3");
				Assert::AreEqual(mqNone, targetSecond, L"targetSecond 3");
			}
		}
// #pragma endregion CEGMAwardManager_ProcessLevelBroadcast_Return

// #pragma region MulticastActive_GetProgressiveLevelForward
		/// <summary>
		/// Functional test for:
		///     CEGMAwardManager::CEGMAwardManager()
		///     CEGMAwardManager::GetProgressiveLevelForward()
		/// Test multicastActive value(s) after calling GetProgressiveLevelForward().
		/// Test with default value(s).
		/// </summary>
		TEST_METHOD(MulticastActive_GetProgressiveLevelForward_Default)
		{
			// Data.
			const DWORD tickCountConfig(10000);
			const DWORD tickCountForward1(tickCountConfig + CProgressiveLevelForward::StrobeInterval);
			const DWORD tickCountForward2(tickCountConfig + CProgressiveLevelForward::MaxInterval * 2);

			// Setup.
			const int testIndex(0);
			AssertResult2(CProgressiveLevels::DefaultMulticastActive, true);

			// Perform operation(s) and test(s).
			const ProgressiveLevelManager progressiveLevelManager;
			CEGMAwardManager egmAwardManager(progressiveLevelManager);
			AssertResult2(egmAwardManager.GetMulticastActive(), true);

			bool multicastActiveChanged;
			unique_ptr<CProgressiveLevelForward> levelForward1(
				egmAwardManager.GetProgressiveLevelForward(tickCountForward1, false, multicastActiveChanged));
			AssertResult2(egmAwardManager.GetMulticastActive(), true);
			AssertResult2(multicastActiveChanged, false);

			unique_ptr<CProgressiveLevelForward> levelForward2(
				egmAwardManager.GetProgressiveLevelForward(tickCountForward2, false, multicastActiveChanged));
			AssertResult2(egmAwardManager.GetMulticastActive(), true);
			AssertResult2(multicastActiveChanged, false);
		}

		/// <summary>
		/// Functional test for:
		///     CEGMAwardManager::CEGMAwardManager()
		///     CEGMAwardManager::GetProgressiveLevelForward()
		/// Test multicastActive value(s) after calling GetProgressiveLevelForward().
		/// Test where GetMulticastActive() value remains true.
		/// </summary>
		TEST_METHOD(MulticastActive_GetProgressiveLevelForward_TrueTrue)
		{
			// Data.
			const DWORD tickCountConfig(10000);
			const DWORD tickCountForward1(tickCountConfig + CProgressiveLevelForward::StrobeInterval / 2);
			const DWORD tickCountForward2(tickCountConfig + CProgressiveLevelForward::StrobeInterval);

			// Setup.
			const int testIndex(0);
			const ProgressiveLevelManager progressiveLevelManager;
			CEGMAwardManager egmAwardManager(progressiveLevelManager);
			CGames m_games;
			const bool progressiveDown(false);

			// Configure egmAwardManager.
			{
				_ICardConfig5 cardConfig;
				rapidjson::Document jsonDoc = cardConfig.CreateJsonDoc();
				CProgressiveLevels *levels(new CProgressiveLevels(tickCountConfig, jsonDoc, nullptr));
				unique_ptr<CInternalMessage> msg(new CInternalMessage(INT_MSG_PROGRESSIVE_LEVELS_INFO, levels));
				const MessageQueueId source(mqProgressive);

				CInternalMessage *targetMsg(nullptr);
				MessageQueueId target(mqNone);
				CInternalMessage *targetSecondMsg(nullptr);
				MessageQueueId targetSecond(mqNone);
				SYSTEMTIME systemTime1;
				GetSystemTime(&systemTime1);

				std::vector<MsgTargetPair*> *returnMessages = new std::vector<MsgTargetPair*>();

				egmAwardManager.ProcessMessage(tickCountConfig,
					systemTime1,
					m_games,
					progressiveDown,
					msg.get(),
					source,
					targetMsg,
					target,
					returnMessages);

				// Cleanup
				delete targetMsg;
				delete targetSecondMsg;
			}

			AssertResult2(egmAwardManager.GetMulticastActive(), true); // Verify setup.

			// Perform operation(s) and test(s).
			bool multicastActiveChanged;
			unique_ptr<CProgressiveLevelForward> levelForward1(
				egmAwardManager.GetProgressiveLevelForward(tickCountForward1, false, multicastActiveChanged));
			AssertResult2(egmAwardManager.GetMulticastActive(), true);
			AssertResult2(multicastActiveChanged, false);

			unique_ptr<CProgressiveLevelForward> levelForward2(
				egmAwardManager.GetProgressiveLevelForward(tickCountForward2, false, multicastActiveChanged));
			AssertResult2(egmAwardManager.GetMulticastActive(), true);
			AssertResult2(multicastActiveChanged, false);
		}

		/// <summary>
		/// Functional test for:
		///     CEGMAwardManager::CEGMAwardManager()
		///     CEGMAwardManager::GetProgressiveLevelForward()
		/// Test multicastActive value(s) after calling GetProgressiveLevelForward().
		/// Test where GetMulticastActive() value changes from true to false.
		/// </summary>
		TEST_METHOD(MulticastActive_GetProgressiveLevelForward_TrueFalse)
		{
			// Data.
			const DWORD tickCountConfig(10000);
			const DWORD tickCountForward1(tickCountConfig + CProgressiveLevelForward::StrobeInterval);
			const DWORD tickCountForward2(tickCountConfig + CProgressiveLevelForward::MaxInterval * 2);

			// Setup.
			const int testIndex(0);
			const ProgressiveLevelManager progressiveLevelManager;
			CEGMAwardManager egmAwardManager(progressiveLevelManager);
			CGames m_games;
			const bool progressiveDown(false);

			// Configure egmAwardManager.
			{
				_ICardConfig5 cardConfig;
				rapidjson::Document jsonDoc = cardConfig.CreateJsonDoc();
				CProgressiveLevels *levels(new CProgressiveLevels(tickCountConfig, jsonDoc, nullptr));
				unique_ptr<CInternalMessage> msg(new CInternalMessage(INT_MSG_PROGRESSIVE_LEVELS_INFO, levels));
				const MessageQueueId source(mqProgressive);

				CInternalMessage *targetMsg(nullptr);
				MessageQueueId target(mqNone);
				CInternalMessage *targetSecondMsg(nullptr);
				MessageQueueId targetSecond(mqNone);
				SYSTEMTIME systemTime1;
				GetSystemTime(&systemTime1);

				std::vector<MsgTargetPair*> *returnMessages = new std::vector<MsgTargetPair*>();

				egmAwardManager.ProcessMessage(tickCountConfig,
					systemTime1,
					m_games,
					progressiveDown,
					msg.get(),
					source,
					targetMsg,
					target,
					returnMessages);

				// Cleanup
				delete targetMsg;
				delete targetSecondMsg;
			}

			AssertResult2(egmAwardManager.GetMulticastActive(), true); // Verify setup.

			// Perform operation(s) and test(s).
			bool multicastActiveChanged;
			unique_ptr<CProgressiveLevelForward> levelForward1(
				egmAwardManager.GetProgressiveLevelForward(tickCountForward1, false, multicastActiveChanged));
			AssertResult2(egmAwardManager.GetMulticastActive(), true);
			AssertResult2(multicastActiveChanged, false);

			unique_ptr<CProgressiveLevelForward> levelForward2(
				egmAwardManager.GetProgressiveLevelForward(tickCountForward2, false, multicastActiveChanged));
			AssertResult2(egmAwardManager.GetMulticastActive(), false);
			AssertResult2(multicastActiveChanged, true);
		}

		/// <summary>
		/// Functional test for:
		///     CEGMAwardManager::CEGMAwardManager()
		///     CEGMAwardManager::GetProgressiveLevelForward()
		/// Test multicastActive value(s) after calling GetProgressiveLevelForward().
		/// Test where GetMulticastActive() value remains false.
		/// </summary>
		TEST_METHOD(MulticastActive_GetProgressiveLevelForward_FalseFalse)
		{
			// Data.
			const DWORD tickCountConfig(10000);
			const DWORD tickCountForward0(tickCountConfig + CProgressiveLevelForward::MaxInterval * 2);
			const DWORD tickCountForward1(tickCountConfig + CProgressiveLevelForward::MaxInterval * 3);
			const DWORD tickCountForward2(tickCountConfig + CProgressiveLevelForward::MaxInterval * 4);

			// Setup.
			const int testIndex(0);
			const ProgressiveLevelManager progressiveLevelManager;
			CEGMAwardManager egmAwardManager(progressiveLevelManager);
			CGames m_games;
			const bool progressiveDown(false);

			// Configure egmAwardManager.
			{
				_ICardConfig5 cardConfig;
				rapidjson::Document jsonDoc = cardConfig.CreateJsonDoc();
				CProgressiveLevels *levels(new CProgressiveLevels(tickCountConfig, jsonDoc, nullptr));
				unique_ptr<CInternalMessage> msg(new CInternalMessage(INT_MSG_PROGRESSIVE_LEVELS_INFO, levels));
				const MessageQueueId source(mqProgressive);

				CInternalMessage *targetMsg(nullptr);
				MessageQueueId target(mqNone);
				CInternalMessage *targetSecondMsg(nullptr);
				MessageQueueId targetSecond(mqNone);
				SYSTEMTIME systemTime1;
				GetSystemTime(&systemTime1);

				std::vector<MsgTargetPair*> *returnMessages = new std::vector<MsgTargetPair*>();

				egmAwardManager.ProcessMessage(tickCountConfig,
					systemTime1,
					m_games,
					progressiveDown,
					msg.get(),
					source,
					targetMsg,
					target,
					returnMessages);

				// Cleanup
				delete targetMsg;
				delete targetSecondMsg;
			}

			AssertResult2(egmAwardManager.GetMulticastActive(), true); // Verify setup.

			bool multicastActiveChanged;
			unique_ptr<CProgressiveLevelForward> levelForward0(
				egmAwardManager.GetProgressiveLevelForward(tickCountForward0, false, multicastActiveChanged));
			AssertResult2(egmAwardManager.GetMulticastActive(), false);
			AssertResult2(multicastActiveChanged, true);

			// Perform operation(s) and test(s).
			unique_ptr<CProgressiveLevelForward> levelForward1(
				egmAwardManager.GetProgressiveLevelForward(tickCountForward1, false, multicastActiveChanged));
			AssertResult2(egmAwardManager.GetMulticastActive(), false);
			AssertResult2(multicastActiveChanged, false);

			unique_ptr<CProgressiveLevelForward> levelForward2(
				egmAwardManager.GetProgressiveLevelForward(tickCountForward2, false, multicastActiveChanged));
			AssertResult2(egmAwardManager.GetMulticastActive(), false);
			AssertResult2(multicastActiveChanged, false);
		}

		/// <summary>
		/// Functional test for:
		///     CEGMAwardManager::CEGMAwardManager()
		///     CEGMAwardManager::GetProgressiveLevelForward()
		/// Test multicastActive value(s) after calling GetProgressiveLevelForward().
		/// Test where GetMulticastActive() value changes from false to true.
		/// </summary>
		TEST_METHOD(MulticastActive_GetProgressiveLevelForward_FalseTrue)
		{
			// Data.
			const DWORD tickCountConfig(10000);
			const DWORD tickCountForward0(tickCountConfig + CProgressiveLevelForward::MaxInterval * 2);
			const DWORD tickCountForward1(tickCountConfig + CProgressiveLevelForward::MaxInterval * 3);
			const DWORD tickCountBroadcast2(tickCountConfig + CProgressiveLevelForward::MaxInterval * 4);
			const DWORD tickCountForward2(tickCountBroadcast2 + CProgressiveLevelForward::StrobeInterval);

			// Setup.
			const int testIndex(0);
			const ProgressiveLevelManager progressiveLevelManager;
			CEGMAwardManager egmAwardManager(progressiveLevelManager);
			CGames m_games;
			const bool progressiveDown(false);

			// Configure egmAwardManager.
			{
				_ICardConfig5 cardConfig;
				rapidjson::Document jsonDoc = cardConfig.CreateJsonDoc();
				CProgressiveLevels *levels(new CProgressiveLevels(tickCountConfig, jsonDoc, nullptr));
				unique_ptr<CInternalMessage> msg(new CInternalMessage(INT_MSG_PROGRESSIVE_LEVELS_INFO, levels));
				const MessageQueueId source(mqProgressive);

				CInternalMessage *targetMsg(nullptr);
				MessageQueueId target(mqNone);
				CInternalMessage *targetSecondMsg(nullptr);
				MessageQueueId targetSecond(mqNone);
				SYSTEMTIME systemTime1;
				GetSystemTime(&systemTime1);

				std::vector<MsgTargetPair*> *returnMessages = new std::vector<MsgTargetPair*>();

				egmAwardManager.ProcessMessage(tickCountConfig,
					systemTime1,
					m_games,
					progressiveDown,
					msg.get(),
					source,
					targetMsg,
					target,
					returnMessages);

				// Cleanup.
				delete targetMsg;
				delete targetSecondMsg;
			}

			AssertResult2(egmAwardManager.GetMulticastActive(), true); // Verify setup.

			bool multicastActiveChanged;
			unique_ptr<CProgressiveLevelForward> levelForward0(
				egmAwardManager.GetProgressiveLevelForward(tickCountForward0, false, multicastActiveChanged));
			AssertResult2(egmAwardManager.GetMulticastActive(), false);
			AssertResult2(multicastActiveChanged, true);

			// Perform operation(s) and test(s).
			unique_ptr<CProgressiveLevelForward> levelForward1(
				egmAwardManager.GetProgressiveLevelForward(tickCountForward1, false, multicastActiveChanged));
			AssertResult2(egmAwardManager.GetMulticastActive(), false);
			AssertResult2(multicastActiveChanged, false);

			// Update the levels using a level broadcast.
			{
				static const char *pJson =
					"{\"LevelValues\":["
					"{\"LevelId\":5,\"Value\":5.43,\"Name\":\"\"},"
					"{\"LevelId\":5009,\"Value\":12.34,\"Name\":\"\"},"
					"{\"LevelId\":61234,\"Value\":56.78,\"Name\":\"z\"}"
					"]}";
				rapidjson::Document jsonDoc;
				bool result = jsonDoc.Parse(pJson).HasParseError();
				Assert::IsFalse(result, AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError())));

				CProgressiveLevelBroadcast *levelBroadcast(new CProgressiveLevelBroadcast(tickCountBroadcast2, jsonDoc));
				unique_ptr<CInternalMessage> msg(new CInternalMessage(INT_MSG_PROGRESSIVE_LEVEL_BROADCAST, levelBroadcast));
				const MessageQueueId source(mqProgressive);

				CInternalMessage *targetMsg(nullptr);
				MessageQueueId target(mqNone);
				CInternalMessage *targetSecondMsg(nullptr);
				MessageQueueId targetSecond(mqNone);
				SYSTEMTIME systemTime;
				GetSystemTime(&systemTime);

				std::vector<MsgTargetPair*> *returnMessages = new std::vector<MsgTargetPair*>();

				egmAwardManager.ProcessMessage(tickCountConfig,
					systemTime,
					m_games,
					progressiveDown,
					msg.get(),
					source,
					targetMsg,
					target,
					returnMessages);

				// Cleanup.
				delete targetMsg;
				delete targetSecondMsg;
			}

			unique_ptr<CProgressiveLevelForward> levelForward2(
				egmAwardManager.GetProgressiveLevelForward(tickCountForward2, false, multicastActiveChanged));
			AssertResult2(egmAwardManager.GetMulticastActive(), true);
			AssertResult2(multicastActiveChanged, true);
		}
// #pragma endregion MulticastActive_GetProgressiveLevelForward

// #pragma region CProgressiveBonusingAvailable
		/// <summary>
		/// Functional test for:
		///     CEGMAwardManagerState::CEGMAwardManagerState()
		/// Test constructor for correct m_progressiveBonusingAvailable default value(s).
		/// </summary>
		TEST_METHOD(ProgressiveBonusingAvailable_Default)
		{
			// Perform operation(s) and test(s).
			CEGMAwardManagerState state;
			Assert::AreEqual(0, state.m_progressiveBonusingAvailable->GetSessionId(), L"GetSessionId");
			Assert::AreEqual(false, state.m_progressiveBonusingAvailable->GetEligibilityDetermined(), L"GetEligibilityDetermined");
		}

		/// <summary>
		/// Functional test for:
		///     CEGMAwardManagerState::CEGMAwardManagerState(const CEGMAwardManagerState&)
		/// Test copy constructor for correct m_progressiveBonusingAvailable value(s).
		/// </summary>
		TEST_METHOD(ProgressiveBonusingAvailable_Copy)
		{
			// Data.
			const SYSTEMTIME receivedTime = { 2007, 1, 2, 3, 4, 5, 6, 7 };
			const DWORD receivedTickCount = 9876;
			const int sessionId = 8;
			const bool eligibilityDetermined = true;
			const __int64 eligibleLevel = 445;

			// Setup.
			CProgressiveBonusingAvailable *bonusing(
				new CProgressiveBonusingAvailable(receivedTime, receivedTickCount, sessionId, eligibilityDetermined));
			bonusing->AddEligibleLevel(eligibleLevel, true);

			CEGMAwardManagerState sourceState;
			sourceState.m_progressiveBonusingAvailable.reset(bonusing);

			// Perform operation(s) and test(s).
			CEGMAwardManagerState state(sourceState);
			Assert::AreEqual(sessionId, state.m_progressiveBonusingAvailable->GetSessionId(), L"GetSessionId");
			Assert::AreEqual(eligibilityDetermined, state.m_progressiveBonusingAvailable->GetEligibilityDetermined(), L"GetEligibilityDetermined");
			Assert::AreEqual(true, state.m_progressiveBonusingAvailable->IsLevelEligible(eligibleLevel), L"IsLevelEligible");
		}

		/// <summary>
		/// Functional test for:
		///     CEGMAwardManagerState::CEGMAwardManagerState(const CEGMAwardManagerState&)
		/// Test assignment operator for correct m_progressiveBonusingAvailable value(s).
		/// </summary>
		TEST_METHOD(ProgressiveBonusingAvailable_Assigment)
		{
			// Data.
			const SYSTEMTIME receivedTime = { 2007, 1, 2, 3, 4, 5, 6, 7 };
			const DWORD receivedTickCount = 9876;
			const int sessionId = 8;
			const bool eligibilityDetermined = true;
			const __int64 eligibleLevel = 445;
			const __int64 notEligibleLevel1 = eligibleLevel - 1;
			const __int64 notEligibleLevel2 = eligibleLevel + 1;

			// Setup source state.
			CProgressiveBonusingAvailable *sourceBonusing(
				new CProgressiveBonusingAvailable(receivedTime, receivedTickCount, sessionId, eligibilityDetermined));
			sourceBonusing->AddEligibleLevel(eligibleLevel, true);

			CEGMAwardManagerState sourceState;
			sourceState.m_progressiveBonusingAvailable.reset(sourceBonusing);

			// Setup target state differently from source state.
			CProgressiveBonusingAvailable *bonusing(
				new CProgressiveBonusingAvailable(receivedTime, receivedTickCount, sessionId - 1, !eligibilityDetermined));
			bonusing->AddEligibleLevel(notEligibleLevel1, false);
			bonusing->AddEligibleLevel(notEligibleLevel2, false);

			CEGMAwardManagerState state;
			state.m_progressiveBonusingAvailable.reset(bonusing);

			// Perform operation(s) and test(s).
			state = sourceState;
			Assert::AreEqual(sessionId, state.m_progressiveBonusingAvailable->GetSessionId(), L"GetSessionId");
			Assert::AreEqual(eligibilityDetermined, state.m_progressiveBonusingAvailable->GetEligibilityDetermined(), L"GetEligibilityDetermined");
			Assert::AreEqual(true, state.m_progressiveBonusingAvailable->IsLevelEligible(eligibleLevel), L"IsLevelEligible");
			Assert::AreEqual(false, state.m_progressiveBonusingAvailable->IsLevelEligible(notEligibleLevel1), L"IsLevelEligible N1");
			Assert::AreEqual(false, state.m_progressiveBonusingAvailable->IsLevelEligible(notEligibleLevel2), L"IsLevelEligible N2");
		}

		/// <summary>
		/// Functional test for:
		///     CEGMAwardManagerState::UpdateLevelEligibility()
		/// Test for level eligibility information updated correctly.
		/// </summary>
		TEST_METHOD(ProcessBonusingAvailable_UpdateLevelEligibility)
		{
			// Data.
			const DWORD createdTickCount = 100;
			const DWORD receivedTickCount = 200;
			const SYSTEMTIME receivedTime = { 2001, 2, 0, 3, 12, 34, 56, 789 };
			const bool eligible(true);
			const bool showOdometer(false);

			// Setup.
			CProgressiveLevels *levels(new CProgressiveLevels(createdTickCount));
			levels->AddLevel(createdTickCount, 1, ProgressiveLevelType::Bonus, "", "");

			CProgressiveBonusingAvailable *bonusingAvailable(new CProgressiveBonusingAvailable(receivedTime, receivedTickCount, 1, true));
			bonusingAvailable->AddEligibleLevel(1, false);

			CEGMAwardManagerState state;
			state.m_progressiveLevels.reset(levels);
			state.m_progressiveBonusingAvailable.reset(bonusingAvailable);
			Assert::AreEqual(!eligible, state.m_progressiveLevels->m_levels.front()->m_eligible); // Verify setup.
			Assert::AreEqual(!showOdometer, state.m_progressiveLevels->m_levels.front()->m_showOdometer);

			// Perform operation(s) and test(s).
			state.UpdateLevelEligibility();
			Assert::AreEqual(eligible, state.m_progressiveLevels->m_levels.front()->m_eligible);
			Assert::AreEqual(showOdometer, state.m_progressiveLevels->m_levels.front()->m_showOdometer);
		}

		/// <summary>
		/// Functional test for:
		///     CEGMAwardManager::ProcessMessage()
		///     CEGMAwardManager::ProcessMessageFromProgressive()
		///     CEGMAwardManager::IsWinnerPickValid()
		/// Test for bonusing available message processed and level eligibility information determined correctly.
		/// </summary>
		TEST_METHOD(ProcessBonusingAvailable_IsWinnerPickValid)
		{
			// Data.
			const DWORD receivedTickCount = 200;
			const SYSTEMTIME receivedTime = { 2001, 2, 0, 3, 12, 34, 56, 789 };
			const __int64 eligibleLevel(468);

			// Setup.
			const ProgressiveLevelManager progressiveLevelManager;
			CEGMAwardManager egmAwardManager(progressiveLevelManager);

			// Verify setup. Player is not eligible for WinnerPick.
			Assert::AreEqual(false, egmAwardManager.IsWinnerPickValid(eligibleLevel), L"IsWinnerPickValid setup");

			// Perform operation(s) and test(s).
			// Create bonusing available CInternalMessage.
			CProgressiveBonusingAvailable *bonusingAvailable(new CProgressiveBonusingAvailable(receivedTime, receivedTickCount, 1, true));
			bonusingAvailable->AddEligibleLevel(eligibleLevel, false);
			CInternalMessage msg(INT_MSG_PROGRESSIVE_BONUSING_AVAILABLE, bonusingAvailable);
			MessageQueueId source(mqProgressive);

			// Process bonusing available CInternalMessage.
			CGames m_games;
			bool progressiveDown(false);
			CInternalMessage *targetMsg(nullptr);
			CInternalMessage *targetSecondMsg(nullptr);
			MessageQueueId target(mqNone);
			MessageQueueId targetSecond(mqNone);

			std::vector<MsgTargetPair*> *returnMessages = new std::vector<MsgTargetPair*>();

			egmAwardManager.ProcessMessage(receivedTickCount,
				receivedTime,
				m_games,
				progressiveDown,
				&msg,
				source,
				targetMsg,
				target,
				returnMessages);
			Assert::IsNull(targetMsg, L"targetMsg");
			Assert::IsNull(targetSecondMsg, L"targetSecondMsg");

			// Test if player is eligible for a WinnerPick.
			Assert::AreEqual(true, egmAwardManager.IsWinnerPickValid(eligibleLevel), L"IsWinnerPickValid");
		}
// #pragma endregion CProgressiveBonusingAvailable
#endif

// #pragma region CumulativeProgressiveManager_Nvram
		/// <summary>
		/// Functional test for:
		///     CEGMAwardManager::CEGMAwardManager(MEMORY_TYPE ...)
		/// Test constructor for correct m_cumulativeProgressiveManager memory storage parameters.
		/// </summary>
		TEST_METHOD(CumulativeProgressiveManager_MemoryStorageParameters)
		{
			// Data.
			const MEMORY_TYPE memoryType(MEMORY_NVRAM);

			const MEMORY_TYPE memoryTypeExpected(memoryType);
			const DWORD nvramActiveOffsetExpected(CumulativeProgressiveManager::NVRAM_ACTIVE_OFFSET);
			const DWORD nvramDataOffsetExpected(CumulativeProgressiveManager::NVRAM_DATA_OFFSET);
			const DWORD nvramDataSizeExpected(CumulativeProgressiveManager::NVRAM_DATA_SIZE);
			const BYTE nvramActiveValueExpected(CumulativeProgressiveManager::NVRAM_ACTIVE_VALUE);

			// Perform operation(s) and test(s).
			const ProgressiveLevelManager progressiveLevelManager;
			CEGMAwardManager obj(progressiveLevelManager, memoryType, false);
			Assert::AreEqual(memoryTypeExpected, ((CumulativeProgressiveManager *)obj.m_cumulativeProgressiveManager.get())->m_type, L"m_type");
			Assert::AreEqual(nvramDataOffsetExpected, ((CumulativeProgressiveManager *)obj.m_cumulativeProgressiveManager.get())->m_dwOffset, L"m_dwOffset");
			Assert::AreEqual(nvramDataSizeExpected, ((CumulativeProgressiveManager *)obj.m_cumulativeProgressiveManager.get())->m_dwAllowedSize, L"m_dwAllowedSize");
			Assert::AreEqual(nvramActiveOffsetExpected, ((CumulativeProgressiveManager *)obj.m_cumulativeProgressiveManager.get())->m_activeOffset, L"m_activeOffset");
			Assert::AreEqual(nvramActiveValueExpected, ((CumulativeProgressiveManager *)obj.m_cumulativeProgressiveManager.get())->m_activeValue, L"m_activeValue");
		}

		/// <summary>
		/// Functional test for:
		///     CEGMAwardManager::CEGMAwardManager(MEMORY_TYPE ...)
		///     CumulativeProgressiveManager::GetCumulativeDelta()
		/// Test for m_cumulativeProgressiveManager persisting and restoring from NVRAM.
		/// </summary>
		TEST_METHOD(CumulativeProgressiveManager_Persist)
		{
			// Data.
			static const __int64 currentCumulativeMeterValueDefault(0);
			static const __int64 currentCumulativeMeterValueExpected(123789);

			// Setup.
			CUnitTestingUtils::SetNvramValidAndUpgraded();
			CUnitTestingUtils::SetNvramVersion(NVRAM_CUMULATIVE_PROGRESSIVE_VERSION - 1); // Start with default m_cumulativeProgressiveManager.
			CMemoryStorage::UpgradeNVRAM();

			CMemoryStorage::SetInactiveLocation(NVRAM_CUMULATIVE_PROGRESSIVE_CHECKBYTE_OFFSET,
				(BYTE)~NVRAM_CUMULATIVE_PROGRESSIVE_CHECKBYTE_VALUE); // Want default m_cumulativeProgressiveManager.

			const DWORD receivedTickCount = 200;
			const SYSTEMTIME receivedTime = { 2001, 2, 0, 3, 12, 34, 56, 789 };
			CGames m_games;
			bool progressiveDown(false);
			CInternalMessage msg(INT_MSG_CUMULATIVE_PROGRESSIVE_WIN, currentCumulativeMeterValueExpected);
			MessageQueueId source(mqGCF);

			// Perform operation(s) and test(s).
			{
				const ProgressiveLevelManager progressiveLevelManager;
				CEGMAwardManager egmAwardManager(progressiveLevelManager, MEMORY_NVRAM, true);
				Assert::AreEqual(currentCumulativeMeterValueDefault,
					((CumulativeProgressiveManager *)egmAwardManager.m_cumulativeProgressiveManager.get())->m_currentCumulativeMeterValue,
					L"currentCumulativeMeterValue setup"); // Verify setup.

				// Process INT_MSG_CUMULATIVE_PROGRESSIVE_WIN CInternalMessage.
				CInternalMessage *targetMsg(nullptr);
				CInternalMessage *targetSecondMsg(nullptr);
				MessageQueueId target(mqNone);
				MessageQueueId targetSecond(mqNone);

				std::vector<MsgTargetPair*> *returnMessages = new std::vector<MsgTargetPair*>();

				egmAwardManager.ProcessMessage(receivedTickCount,
					receivedTime,
					m_games,
					progressiveDown,
					&msg,
					source,
					targetMsg,
					target,
					returnMessages);
				delete targetMsg;
				delete targetSecondMsg;

				Assert::AreEqual(currentCumulativeMeterValueExpected,
					((CumulativeProgressiveManager *)egmAwardManager.m_cumulativeProgressiveManager.get())->m_currentCumulativeMeterValue,
					L"currentCumulativeMeterValue operation"); // Verify operation.
			}

			// Perform operation(s) and test(s).
			const ProgressiveLevelManager progressiveLevelManager;
			CEGMAwardManager egmAwardManager(progressiveLevelManager, MEMORY_NVRAM, true);
			Assert::AreEqual(currentCumulativeMeterValueExpected,
				((CumulativeProgressiveManager *)egmAwardManager.m_cumulativeProgressiveManager.get())->m_currentCumulativeMeterValue,
				L"currentCumulativeMeterValue from NVRAM");
		}
// #pragma endregion CumulativeProgressiveManager_Nvram

// #pragma region GetStandardWins
		/// <summary>
		/// Functional test for:
		///     CEGMAwardManager::GetStandardWins()
		/// Test with a default CEGMAwardManager instance.
		/// </summary>
		TEST_METHOD(GetStandardWins_DefaultTest)
		{
			// Setup.
			ProgressiveLevelManager levelManager;
			CEGMAwardManager egmAwardManager(levelManager, MEMORY_NONE, false);
			std::shared_ptr<CEGMAward> lastStandardWin;
			std::vector<std::shared_ptr<CEGMAward>> standardWins;

			// Perform operation(s) and test(s).
			egmAwardManager.GetStandardWins(lastStandardWin, standardWins);
			Assert::IsNull((void *)lastStandardWin.get(), L"lastStandardWin.get");
			Assert::AreEqual((size_t)0, standardWins.size(), L"standardWins.size");
		}

		void ProcessMessage(CEGMAwardManager &egmAwardManager, CInternalMessage &msg, MessageQueueId source)
		{
			// Data.
			const DWORD receivedTickCount = 200;
			const SYSTEMTIME receivedTime = { 2001, 2, 0, 3, 12, 34, 56, 789 };

			// Process bonusing available CInternalMessage.
			CGames m_games;
			bool progressiveDown(false);
			CInternalMessage *targetMsg(nullptr);
			CInternalMessage *targetSecondMsg(nullptr);
			MessageQueueId target(mqNone);
			MessageQueueId targetSecond(mqNone);
			std::vector<MsgTargetPair*> *returnMessages = new std::vector<MsgTargetPair*>();

			egmAwardManager.ProcessMessage(receivedTickCount,
				receivedTime,
				m_games,
				progressiveDown,
				&msg,
				source,
				targetMsg,
				target,
				returnMessages);
			delete targetMsg;
			delete targetSecondMsg;
			delete returnMessages;
		}

		// Configure an ProgressiveLevelManager instance with progressive levels for testing.
		void ConfigureLevels(ProgressiveLevelManager &levelManager)
		{
			LevelData data[] =
			{
				// Line, tickCount, id, type, pName, pUrl, numSasLevels, sasLevels[3], expectedNumSasLevels.
				{ __LINE__, 123, 222, ProgressiveLevelType::Standard, "name0", "url0", 2, { 2, 5 }, 2 },
				{ __LINE__, 951, 111, ProgressiveLevelType::AccountingOnly, "ac", "url3", 1, { 1 }, 1 }
			};
			for (const auto& level : data)
			{
				levelManager.AddLevel(level.CreateProgressiveLevel());
			}
		}

		/// <summary>
		/// Functional test for:
		///     CEGMAwardManager::GetStandardWins()
		/// Test with no last win and one current win.
		/// </summary>
		TEST_METHOD(GetStandardWins_Null_OneTest)
		{
			// Setup.
			ProgressiveLevelManager levelManager;
			ConfigureLevels(levelManager);
			CEGMAwardManager egmAwardManager(levelManager, MEMORY_NONE, false);

			// Add a first standard progressive win.
			CEGMAward *standardWin1(new CEGMAward());
			const SYSTEMTIME receivedFromEGMTime1 = { 2000, 1, 0, 2, 01, 01, 01, 001 };
			standardWin1->SetReceivedFromEGMTime(receivedFromEGMTime1);
			standardWin1->SetAwardState(ReceivedFromGame);
			standardWin1->SetSASLevelId(1);
			standardWin1->SetAwardAmountPaid(10101);
			CInternalMessage msg1(INT_MSG_PROGRESSIVE_WIN, standardWin1);
			ProcessMessage(egmAwardManager, msg1, mqGCF);

			std::shared_ptr<CEGMAward> lastStandardWin;
			std::vector<std::shared_ptr<CEGMAward>> standardWins;

			// Perform operation(s) and test(s).
			egmAwardManager.GetStandardWins(lastStandardWin, standardWins);

			Assert::IsNull((void *)lastStandardWin.get(), L"lastStandardWin.get");
	
			Assert::AreEqual((size_t)1, standardWins.size(), L"standardWins.size");

			shared_ptr<CEGMAward> win1(standardWins[0]);
			Assert::IsNotNull(win1.get(), L"get 1");
			CUnitTestingUtils::AssertEqual(win1->GetReceivedFromEGMTime(), receivedFromEGMTime1,
				_T("GetReceivedFromEGMTime 1"), 0, __TFUNCTION__, __LINE__);
			Assert::AreEqual(ReceivedFromGame, win1->GetAwardState(), L"GetAwardState 1");
			Assert::AreEqual((int)1, win1->GetSASLevelId(), L"GetSASLevelId 1");
			Assert::AreEqual((int)111, win1->GetLevelId(), L"GetLevelId 1");
			Assert::AreEqual((__int64)10101, win1->GetAwardAmountPaid(), L"GetAwardAmountPaid 1");
		}

		/// <summary>
		/// Functional test for:
		///     CEGMAwardManager::GetStandardWins()
		/// Test with no last win and two current wins.
		/// </summary>
		TEST_METHOD(GetStandardWins_Null_OneTwoTest)
		{
			// Setup.
			ProgressiveLevelManager levelManager;
			ConfigureLevels(levelManager);
			CEGMAwardManager egmAwardManager(levelManager, MEMORY_NONE, false);

			// Add a first standard progressive win.
			CEGMAward *standardWin1(new CEGMAward());
			const SYSTEMTIME receivedFromEGMTime1 = { 2000, 1, 0, 2, 01, 01, 01, 001 };
			standardWin1->SetReceivedFromEGMTime(receivedFromEGMTime1);
			standardWin1->SetAwardState(ReceivedFromGame);
			standardWin1->SetSASLevelId(1);
			standardWin1->SetAwardAmountPaid(10101);
			CInternalMessage msg1(INT_MSG_PROGRESSIVE_WIN, standardWin1);
			ProcessMessage(egmAwardManager, msg1, mqGCF);

			// Add a second standard progressive win.
			CEGMAward *standardWin2(new CEGMAward());
			const SYSTEMTIME receivedFromEGMTime2 = { 2000, 1, 0, 2, 02, 02, 02, 002 };
			standardWin2->SetReceivedFromEGMTime(receivedFromEGMTime2);
			standardWin2->SetAwardState(ReceivedFromGame);
			standardWin2->SetSASLevelId(2);
			standardWin2->SetAwardAmountPaid(20202);
			CInternalMessage msg2(INT_MSG_PROGRESSIVE_WIN, standardWin2);
			ProcessMessage(egmAwardManager, msg2, mqGCF);

			std::shared_ptr<CEGMAward> lastStandardWin;
			std::vector<std::shared_ptr<CEGMAward>> standardWins;

			// Perform operation(s) and test(s).
			egmAwardManager.GetStandardWins(lastStandardWin, standardWins);

			Assert::IsNull((void *)lastStandardWin.get(), L"lastStandardWin.get");

			Assert::AreEqual((size_t)2, standardWins.size(), L"standardWins.size");

			shared_ptr<CEGMAward> win1(standardWins[0]);
			Assert::IsNotNull(win1.get(), L"get 1");
			CUnitTestingUtils::AssertEqual(win1->GetReceivedFromEGMTime(), receivedFromEGMTime1,
				_T("GetReceivedFromEGMTime 1"), 0, __TFUNCTION__, __LINE__);
			Assert::AreEqual(ReceivedFromGame, win1->GetAwardState(), L"GetAwardState 1");
			Assert::AreEqual((int)1, win1->GetSASLevelId(), L"GetSASLevelId 1");
			Assert::AreEqual((int)111, win1->GetLevelId(), L"GetLevelId 1");
			Assert::AreEqual((__int64)10101, win1->GetAwardAmountPaid(), L"GetAwardAmountPaid 1");

			shared_ptr<CEGMAward> win2(standardWins[1]);
			Assert::IsNotNull(win2.get(), L"get 2");
			CUnitTestingUtils::AssertEqual(win2->GetReceivedFromEGMTime(), receivedFromEGMTime2,
				_T("GetReceivedFromEGMTime 2"), 0, __TFUNCTION__, __LINE__);
			Assert::AreEqual(ReceivedFromGame, win2->GetAwardState(), L"GetAwardState 2");
			Assert::AreEqual((int)2, win2->GetSASLevelId(), L"GetSASLevelId 2");
			Assert::AreEqual((int)222, win2->GetLevelId(), L"GetLevelId 2");
			Assert::AreEqual((__int64)20202, win2->GetAwardAmountPaid(), L"GetAwardAmountPaid 2");
		}

		/// <summary>
		/// Functional test for:
		///     CEGMAwardManager::GetStandardWins()
		/// Test with a last win and a current win.
		/// </summary>
		TEST_METHOD(GetStandardWins_One_TwoTest)
		{
			// Setup.
			ProgressiveLevelManager levelManager;
			ConfigureLevels(levelManager);
			CEGMAwardManager egmAwardManager(levelManager, MEMORY_NONE, false);

			// Add a first standard progressive win.
			CEGMAward *standardWin1(new CEGMAward());
			const SYSTEMTIME receivedFromEGMTime1 = { 2000, 1, 0, 2, 01, 01, 01, 001 };
			standardWin1->SetReceivedFromEGMTime(receivedFromEGMTime1);
			standardWin1->SetAwardState(ReceivedFromGame);
			standardWin1->SetSASLevelId(1);
			standardWin1->SetAwardAmountPaid(10101);
			CInternalMessage msg1(INT_MSG_PROGRESSIVE_WIN, standardWin1);
			ProcessMessage(egmAwardManager, msg1, mqGCF);

			// Add a second standard progressive win.
			CEGMAward *standardWin2(new CEGMAward());
			const SYSTEMTIME receivedFromEGMTime2 = { 2000, 1, 0, 2, 02, 02, 02, 002 };
			standardWin2->SetReceivedFromEGMTime(receivedFromEGMTime2);
			standardWin2->SetAwardState(ReceivedFromGame);
			standardWin2->SetSASLevelId(2);
			standardWin2->SetAwardAmountPaid(20202);
			CInternalMessage msg2(INT_MSG_PROGRESSIVE_WIN, standardWin2);
			ProcessMessage(egmAwardManager, msg2, mqGCF);

			// Finish processing the first standard progressive win.
			CEGMAward *standardWin1h(new CEGMAward());
			standardWin1h->SetReceivedFromEGMTime(receivedFromEGMTime1);
			standardWin1h->SetAwardState(ReceivedFromGame);
			standardWin1h->SetSASLevelId(1);
			standardWin1h->SetLevelId(111);
			standardWin1h->SetJackpotId(1111);
			standardWin1h->SetAwardAmountPaid(10101);
			CInternalMessage msg1h(INT_MSG_SET_PROGRESSIVE_PAID_SENT, standardWin1h);
			ProcessMessage(egmAwardManager, msg1h, mqProgressive);

			std::shared_ptr<CEGMAward> lastStandardWin;
			std::vector<std::shared_ptr<CEGMAward>> standardWins;

			// Perform operation(s) and test(s).
			egmAwardManager.GetStandardWins(lastStandardWin, standardWins);

			Assert::IsNotNull((void *)lastStandardWin.get(), L"lastStandardWin.get");
			Assert::IsNotNull(lastStandardWin.get(), L"get 1");
			CUnitTestingUtils::AssertEqual(lastStandardWin->GetReceivedFromEGMTime(), receivedFromEGMTime1,
				_T("GetReceivedFromEGMTime 1"), 0, __TFUNCTION__, __LINE__);
			Assert::AreEqual(ReceivedFromGame, lastStandardWin->GetAwardState(), L"GetAwardState 1");
			Assert::AreEqual((int)1, lastStandardWin->GetSASLevelId(), L"GetSASLevelId 1");
			Assert::AreEqual((int)111, lastStandardWin->GetLevelId(), L"GetLevelId 1");
			Assert::AreEqual((__int64)10101, lastStandardWin->GetAwardAmountPaid(), L"GetAwardAmountPaid 1");

			Assert::AreEqual((size_t)1, standardWins.size(), L"standardWins.size");

			shared_ptr<CEGMAward> win2(standardWins[0]);
			Assert::IsNotNull(win2.get(), L"get 2");
			CUnitTestingUtils::AssertEqual(win2->GetReceivedFromEGMTime(), receivedFromEGMTime2,
				_T("GetReceivedFromEGMTime 2"), 0, __TFUNCTION__, __LINE__);
			Assert::AreEqual(ReceivedFromGame, win2->GetAwardState(), L"GetAwardState 2");
			Assert::AreEqual((int)2, win2->GetSASLevelId(), L"GetSASLevelId 2");
			Assert::AreEqual((int)222, win2->GetLevelId(), L"GetLevelId 2");
			Assert::AreEqual((__int64)20202, win2->GetAwardAmountPaid(), L"GetAwardAmountPaid 2");
		}
// #pragma endregion GetStandardWins
	};

	const SYSTEMTIME EGMAwardManager::m_sentToGameTimeExpected = { 0, 0, 0, 0, 0, 0, 0, 0 };
	const SYSTEMTIME EGMAwardManager::m_receivedFromSystemTimeExpected = { 0, 0, 0, 0, 0, 0, 0, 0 };
	const CString EGMAwardManager::m_logInfoStringExpected(
		_T("AWARD: rcvd=N/A state=\"No Award\" lvl=0 sasLvl=255 std=0 sesId=N/A id=-1 hit=0 paid=0 sent=0 sentTime=N/A")
		_T(" machBnsMtr=-1 attnBnsMtr=-1 legcyBnsMtr=-1 dups=0 other=0")
		_T(" xferType=\"None\" hitRcvd=0 hpPend=0 paidSig=0 fromEgm=N/A")
		_T(" methOfPay=-1")
		_T(" info=\"\" -- "));
}
#endif //TODO-PORT