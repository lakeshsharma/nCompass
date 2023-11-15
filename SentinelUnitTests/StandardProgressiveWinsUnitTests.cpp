#include "stdafx.h"
#include "ProgressiveProtocol/EGMAwardManager.h"
#include "ProgressiveProtocol/StandardProgressiveWins.h"
#include "../SentinelLib/ProgressiveWinsMock.h"
#include "MsgTargetPair.h"
//#include "UnitTestingToString.h"
#include "UnitTestingUtils.h"
#include "gtest/gtest.h"

using ::testing::Return;
using ::testing::Mock;
using ::testing::_;

namespace SentinelUnitTests
{

class StandardProgressiveWinsFriend : public StandardProgressiveWins
{

public:
   std::shared_ptr<CEGMAward> GetLastAwardProcessed() { return m_lastAwardProcessed; }
   std::vector<std::shared_ptr<CEGMAward>> GetAwards() { return m_Awards; }
   std::queue<DWORD> &GetAvailableNvramActiveOffsets() { return m_availableNvramActiveOffsets; }
   StandardProgressiveWinsFriend(bool buildFromNvram = false);
   std::shared_ptr<CEGMAward> FindAward(__int64 awardAmount, int levelId);
   void RemoveAward(__int64 awardAmount, int levelId);
};

class CProgressiveLevelFriend : public CProgressiveLevel
{
public:
   CProgressiveLevelFriend() = default;
   CProgressiveLevelFriend(ProgressiveLevelInfo& levelInfo, DWORD updatedTickCount);
   int64_t GetID() { return m_id; }
   void SetID(int64_t id) { m_id = id; }
   void SetName(std::string name) { m_name = name; }
   std::string GetName() { return m_name; }
   void SetURL(std::string url) { m_url = url; }
   std::string GetURL() { return m_url; }
};

class ProgressiveLevelInfoFriend : public ProgressiveLevelInfo
{
public:
   ProgressiveLevelInfoFriend(int64_t levelId, ProgressiveLevelType::Type type, const rapidjson::Value& level);
};

class StandardProgressiveWinsTests : public ::testing::Test
{
public:
   virtual void SetUp()
   {
   }
   virtual void TearDown()
   {
   }
};

/// <summary>
/// Code coverage and functional test for:
/// ProgressiveWins::AddProgressiveWin
/// ProgressiveWins::HasUnsentProgressiveWins
/// </summary>
TEST_F(StandardProgressiveWinsTests, StandardProgressive_Wins_Has_Unsent_Add_Progressive_Win)
{
   //Arrange
   StandardProgressiveWins progWins;
   shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());
   award->SetAwardAmountHit(12398); //123.98
   award->SetLevelId(9);

   //Act
   progWins.AddProgressiveWin(award);

   //Assert
   ASSERT_TRUE(true == progWins.HasUnsentProgressiveWins());       //make sure items exist
}

/// <summary>
/// Code coverage and functional test for:
/// StandardProgressiveWins::HasUnsentProgressiveWins
/// </summary>
TEST_F(StandardProgressiveWinsTests, StandardProgressive_Wins_Has_No_Unsent_by_Default)
{
   //Arrange
   StandardProgressiveWins progWins;
   shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());
   award->SetAwardAmountHit(12398); //123.98
   award->SetLevelId(9);

   //Act
   //Nothing to act on

   //Assert
   ASSERT_TRUE(false == progWins.HasUnsentProgressiveWins());      //make sure items exist
}

/// <summary>
/// Code coverage and functional test for:
/// StandardProgressiveWins::AddProgressiveWin
/// </summary>
TEST_F(StandardProgressiveWinsTests, StandardProgressive_Wins_Add_Progressive_Win_Fail)
{
   //Arrange
   StandardProgressiveWins progWins;
   bool didAdd = progWins.AddProgressiveWin(NULL);
   //Act
   //Nothing to act on

   //Assert
   ASSERT_TRUE(false == didAdd);   //should not have added a NULL
}

// Need to fix friend class issues
/// <summary>
/// Code coverage and functional test for:
/// StandardProgressiveWins::MarkState
/// </summary>
TEST_F(StandardProgressiveWinsTests, StandardProgressive_MarkState_Exists)
{
   //Arrange
   StandardProgressiveWinsFriend progWins;
   shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());
   award->SetAwardAmountHit(12398); //123.98
   award->SetLevelId(9);
   award->SetAwardState(SentToSystem);

   //Act
   progWins.AddProgressiveWin(award);
   progWins.MarkState(award, Signaled);

   shared_ptr<CEGMAward> foundAward(progWins.FindAward(award->GetAwardAmountHit(), award->GetLevelId()));


   //Assert
   ASSERT_TRUE(foundAward->GetAwardState() == Signaled);
}

/// <summary>
/// Code coverage and functional test for:
/// StandardProgressiveWins::MarkState
/// </summary>
TEST_F(StandardProgressiveWinsTests, StandardProgressive_MarkState_Non_Exists)
{
   //Arrange
   StandardProgressiveWins progWins;
   shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());
   award->SetAwardAmountHit(12398); //123.98
   award->SetLevelId(9);
   award->SetAwardState(SentToSystem);

   //Act
   bool found = progWins.MarkState(award, Signaled);

   //Assert
   ASSERT_FALSE(found);
}

/// <summary>
/// Code coverage and functional test for:
/// StandardProgressiveWins::ProcessEGMAwardStandardFromProgressive
/// StandardProgressiveWins::ProcessEGMAwardWithEGMPaidSignaled
/// </summary>
TEST_F(StandardProgressiveWinsTests, StandardProgressive_ProcessEGMAward_EgmPaidSignaled)
{
   //Arrange
   CGames games;
   __int64 amountHit = 12398; //123.98
   int levelID = 9;
   __int64 jackpotID = 5;
   __int64 remoteJackpotId = 1111;
   std::vector<MsgTargetPair *> *returnMessages = new std::vector<MsgTargetPair *>();

   StandardProgressiveWins progWins;
   shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());    //This one is used to Add to progWins
   award->SetAwardAmountHit(amountHit);
   award->SetLevelId(levelID);
   award->SetAwardState(SentToSystem);
   award->setEgmPaidSignaled(true);
   progWins.AddProgressiveWin(award);      //Add to progWins

   shared_ptr<CEGMAward>awardFromOneLink = shared_ptr<CEGMAward>(new CEGMAward()); //This one is used to emulate the EGM award coming back from oneLink
   awardFromOneLink->SetAwardAmountHit(amountHit);
   awardFromOneLink->SetLevelId(levelID);
   awardFromOneLink->SetAwardState(ReceivedFromSystem);
   awardFromOneLink->SetJackpotId(jackpotID);
   awardFromOneLink->SetRemoteJackpotId(remoteJackpotId);
   awardFromOneLink->SetAwardAmountPaid(amountHit);

   //Act
   progWins.ProcessEGMAwardStandardFromProgressive(0, games, false, *awardFromOneLink.get(), returnMessages);      //Process the award


   //Assert
   ASSERT_EQ((int)returnMessages->size(), 1);
   ASSERT_EQ(progWins.HasUnsentProgressiveWins(), false);

   for (auto awardIterator = returnMessages->cbegin(); awardIterator != returnMessages->cend(); /*no need to in iterator*/)
   {
      ASSERT_EQ((*awardIterator)->msg->GetMessageType(), INT_MSG_EGM_AWARD);          //Make sure we are sending an EGM award back.
      ASSERT_EQ((*awardIterator)->target, mqProgressive);                                                             //Make sure we are sending it to OneLink.
      ASSERT_EQ((*awardIterator)->msg->GetEGMAward()->GetAwardState(), AwardToGameSuccess);   //Make sure it is in an AwardToGameSuccess(Paid) state.
      ASSERT_EQ((*awardIterator)->msg->GetEGMAward()->GetAwardAmountHit(), amountHit);                //Make sure it is the proper amount.
      ASSERT_EQ((*awardIterator)->msg->GetEGMAward()->GetLevelId(), levelID);                         //Make sure it is the proper level.
      ASSERT_EQ((*awardIterator)->msg->GetEGMAward()->GetJackpotId(), jackpotID);                     //Make sure it is the proper Jackpot ID.
	  ASSERT_EQ((*awardIterator)->msg->GetEGMAward()->GetRemoteJackpotId(), remoteJackpotId);         //Make sure it is the proper Remote Jackpot ID.
	  ASSERT_EQ((*awardIterator)->msg->GetEGMAward()->GetAwardAmountPaid(), amountHit);               //Make sure it is the proper Award Amount Paid.
	  

      if (nullptr != (*awardIterator)->msg)
      {
         delete (*awardIterator)->msg; //Delete the message that was created
      }
      awardIterator = returnMessages->erase(awardIterator);   //remove the item from the vector and repair the iterator
   }
   delete returnMessages;
}

/// <summary>
/// Code coverage and functional test for:
/// StandardProgressiveWins::ProcessEGMAwardStandardFromProgressive
/// StandardProgressiveWins::ProcessEGMAwardWithNoEGMPaidSignaled
/// </summary>
TEST_F(StandardProgressiveWinsTests, StandardProgressive_ProcessEGMAward_EgmPaid_NOT_Signaled_HandPaid_IsSignaled)
{
   //Arrange
   CGames games;
   __int64 amountHit = 12398; //123.98
   int levelID = 9;
   __int64 jackpotID = 5;
   __int64 remoteJackpotId = 1111;
   std::vector<MsgTargetPair *> *returnMessages = new std::vector<MsgTargetPair *>();

   //________________This section sets the award int the list____________________________________
   StandardProgressiveWins progWins;
   shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());    //This one is used to Add to progWins
   award->SetAwardAmountHit(amountHit);
   award->SetLevelId(levelID);
   award->SetAwardState(SentToSystem);
   award->setEgmPaidSignaled(false);       //NOT signaled
   award->setHandpayPendingSignaled(true); //YES signaled
   progWins.AddProgressiveWin(award);      //Add to progWins

   //________________This section creates the award to emulate getting the award back from One Link____________________________________
   shared_ptr<CEGMAward>awardFromOneLink = shared_ptr<CEGMAward>(new CEGMAward()); //This one is used to emulate the EGM award coming back from oneLink
   awardFromOneLink->SetAwardAmountHit(amountHit);
   awardFromOneLink->SetLevelId(levelID);
   awardFromOneLink->SetAwardState(ReceivedFromSystem);
   awardFromOneLink->SetJackpotId(jackpotID);
   awardFromOneLink->SetRemoteJackpotId(remoteJackpotId);
   awardFromOneLink->SetAwardAmountPaid(amountHit);

   //Act
   progWins.ProcessEGMAwardStandardFromProgressive(0, games, false, *awardFromOneLink.get(), returnMessages);      //Process the award

   //Assert
   ASSERT_EQ((int)returnMessages->size(), 1);
   ASSERT_EQ(progWins.HasUnsentProgressiveWins(), false);

   for (auto awardIterator = returnMessages->cbegin(); awardIterator != returnMessages->cend(); /*no need to in iterator*/)
   {
      ASSERT_EQ((*awardIterator)->msg->GetMessageType(), INT_MSG_EGM_AWARD);          //Make sure we are sending an EGM award back.
      ASSERT_EQ((*awardIterator)->target, mqProgressive);                                                             //Make sure we are sending it to OneLink.
      ASSERT_EQ((*awardIterator)->msg->GetEGMAward()->GetAwardState(), HandpayPending);       //Make sure it is in an AwardToGameSuccess(Paid) state.
      ASSERT_EQ((*awardIterator)->msg->GetEGMAward()->GetAwardAmountHit(), amountHit);                //Make sure it is the proper amount.
      ASSERT_EQ((*awardIterator)->msg->GetEGMAward()->GetLevelId(), levelID);                         //Make sure it is the proper level.
      ASSERT_EQ((*awardIterator)->msg->GetEGMAward()->GetJackpotId(), jackpotID);                     //Make sure it is the proper Jackpot ID.
	  ASSERT_EQ((*awardIterator)->msg->GetEGMAward()->GetRemoteJackpotId(), remoteJackpotId);         //Make sure it is the proper Remote Jackpot ID.
	  ASSERT_EQ((*awardIterator)->msg->GetEGMAward()->GetAwardAmountPaid(), amountHit);               //Make sure it is the proper Award Amount Paid.

      if (nullptr != (*awardIterator)->msg)
      {
         delete (*awardIterator)->msg; //Delete the message that was created
      }
      awardIterator = returnMessages->erase(awardIterator);   //remove the item from the vector and repair the iterator
   }
   delete returnMessages;
}

/// <summary>
/// Code coverage and functional test for:
/// StandardProgressiveWins::ProcessEGMAwardStandardFromProgressive
/// StandardProgressiveWins::ProcessEGMAwardWithNoEGMPaidSignaled
/// </summary>
TEST_F(StandardProgressiveWinsTests, StandardProgressive_ProcessEGMAward_EgmPaid_NOT_Signaled_HandPaid_Is_NOT_Signaled)
{
   //Arrange
   CGames games;
   __int64 amountHit = 12398; //123.98
   int levelID = 9;
   __int64 jackpotID = 5;
   std::vector<MsgTargetPair *> *returnMessages = new std::vector<MsgTargetPair *>();

   //________________This section sets the award int the list____________________________________
   StandardProgressiveWins progWins;
   shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());    //This one is used to Add to progWins
   award->SetAwardAmountHit(amountHit);
   award->SetLevelId(levelID);
   award->SetAwardState(SentToSystem);
   award->setEgmPaidSignaled(false);       //NOT signaled
   award->setHandpayPendingSignaled(false); //NOT signaled
   progWins.AddProgressiveWin(award);      //Add to progWins

   //________________This section creates the award to emulate getting the award back from One Link____________________________________
   shared_ptr<CEGMAward>awardFromOneLink = shared_ptr<CEGMAward>(new CEGMAward()); //This one is used to emulate the EGM award coming back from oneLink
   awardFromOneLink->SetAwardAmountHit(amountHit);
   awardFromOneLink->SetLevelId(levelID);
   awardFromOneLink->SetAwardState(ReceivedFromSystem);
   awardFromOneLink->SetJackpotId(jackpotID);

   //Act
   progWins.ProcessEGMAwardStandardFromProgressive(0, games, false, *awardFromOneLink.get(), returnMessages);      //Process the award

   //Assert
   ASSERT_EQ((int)returnMessages->size(), 0);
   ASSERT_EQ(progWins.HasUnsentProgressiveWins(), false);

   for (auto awardIterator = returnMessages->cbegin(); awardIterator != returnMessages->cend(); /*no need to in iterator*/)
   {
      if (nullptr != (*awardIterator)->msg)
      {
         delete (*awardIterator)->msg; //Delete the message that was created
      }
      awardIterator = returnMessages->erase(awardIterator);   //remove the item from the vector and repair the iterator
   }
   delete returnMessages;
}

/// <summary>
/// Code coverage and functional test for:
/// StandardProgressiveWins::SetHandPayPending
/// </summary>
TEST_F(StandardProgressiveWinsTests, StandardProgressive_SetHandPayPending_HandPayPending_NOT_signaled_Hit_Was_Received)
{
   //Arrange
   CGames games;
   CInternalMessage *targetMsg(nullptr);
   MessageQueueId target = mqNone;
   __int64 amountHit = 12398; //123.98
   int levelID = 9;
   __int64 jackpotID = 5;
   std::vector<MsgTargetPair *> *returnMessages = new std::vector<MsgTargetPair *>();

   //________________This section sets the award int the list____________________________________
   StandardProgressiveWins progWins;
   shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());    //This one is used to Add to progWins
   award->SetAwardAmountHit(amountHit);
   award->SetLevelId(levelID);
   award->SetSASLevelId(levelID);
   award->SetAwardState(SentToSystem);
   award->SetHitReceived(true);    //YES signaled
   award->setEgmPaidSignaled(false);       //NOT signaled
   award->setHandpayPendingSignaled(false); //NOT signaled
   progWins.AddProgressiveWin(award);      //Add to progWins

   //Act
   progWins.SetHandPayPending(amountHit, levelID, returnMessages);

   //Assert
   ASSERT_EQ((int)returnMessages->size(), 1);

   for (auto awardIterator = returnMessages->cbegin(); awardIterator != returnMessages->cend(); /*no need to in iterator*/)
   {
      ASSERT_EQ((*awardIterator)->msg->GetEGMAward()->isHandpayPendingSignaled(), true);
      ASSERT_EQ((*awardIterator)->msg->GetEGMAward()->GetAwardState(), HandpayPending);       //Make sure it is in a pending state.
      ASSERT_EQ((*awardIterator)->msg->GetMessageType(), INT_MSG_EGM_AWARD);          //Make sure we are sending an EGM award back.
      ASSERT_EQ((*awardIterator)->target, mqProgressive);

      if (nullptr != (*awardIterator)->msg)
      {
         delete (*awardIterator)->msg; //Delete the message that was created
      }
      awardIterator = returnMessages->erase(awardIterator);   //remove the item from the vector and repair the iterator
   }
   delete returnMessages;
}

/// <summary>
/// Code coverage and functional test for:
/// StandardProgressiveWins::SetHandPayPending
/// </summary>
TEST_F(StandardProgressiveWinsTests, StandardProgressive_SetHandPayPending_HandPayPending_NOT_signaled_Hit_Was_Received_More_Than_One)
{
   //Arrange
   CGames games;
   CInternalMessage *targetMsg(nullptr);
   MessageQueueId target = mqNone;
   __int64 amountHit = 12398; //123.98
   int levelID = 9;
   __int64 amountHit2 = 89321; //893.21
   int levelID2 = 2;
   __int64 jackpotID = 5;
   std::vector<MsgTargetPair *> *returnMessages = new std::vector<MsgTargetPair *>();

   //________________This section sets the award int the list____________________________________
   StandardProgressiveWins progWins;
   shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());    //This one is used to Add to progWins
   award->SetAwardAmountHit(amountHit);
   award->SetLevelId(levelID);
   award->SetSASLevelId(levelID);
   award->SetAwardState(SentToSystem);
   award->SetHitReceived(true);    //YES signaled
   award->setEgmPaidSignaled(false);       //NOT signaled
   award->setHandpayPendingSignaled(false); //NOT signaled
   progWins.AddProgressiveWin(award);      //Add to progWins

   shared_ptr<CEGMAward>award2 = shared_ptr<CEGMAward>(new CEGMAward());   //This one is used to Add to progWins
   award2->SetAwardAmountHit(amountHit2);
   award2->SetLevelId(levelID2);
   award2->SetSASLevelId(levelID2);
   award2->SetAwardState(SentToSystem);
   award2->SetHitReceived(true);   //YES signaled
   award2->setEgmPaidSignaled(false);      //NOT signaled
   award2->setHandpayPendingSignaled(false); //NOT signaled
   progWins.AddProgressiveWin(award2);     //Add to progWins

   //Act
   progWins.SetHandPayPending(amountHit, levelID, returnMessages);

   //Assert
   ASSERT_EQ((int)returnMessages->size(), 2);

   for (auto awardIterator = returnMessages->cbegin(); awardIterator != returnMessages->cend(); /*no need to in iterator*/)
   {
      ASSERT_EQ((*awardIterator)->msg->GetEGMAward()->isHandpayPendingSignaled(), true);
      ASSERT_EQ((*awardIterator)->msg->GetEGMAward()->GetAwardState(), HandpayPending);       //Make sure it is in a pending state.
      ASSERT_EQ((*awardIterator)->msg->GetMessageType(), INT_MSG_EGM_AWARD);          //Make sure we are sending an EGM award back.
      ASSERT_EQ((*awardIterator)->target, mqProgressive);

      if (nullptr != (*awardIterator)->msg)
      {
         delete (*awardIterator)->msg; //Delete the message that was created
      }
      awardIterator = returnMessages->erase(awardIterator);   //remove the item from the vector and repair the iterator
   }
   delete returnMessages;
}

/// <summary>
/// Code coverage and functional test for:
/// StandardProgressiveWins::SetHandPayPending
/// </summary>
TEST_F(StandardProgressiveWinsTests, StandardProgressive_SetHandPayPending_HandPayPending_NOT_signaled_Hit_Was_NOT_Received)
{
   //Arrange
   CGames games;
   CInternalMessage *targetMsg(nullptr);
   MessageQueueId target = mqNone;
   __int64 amountHit = 12398; //123.98
   int levelID = 9;
   __int64 jackpotID = 5;
   std::vector<MsgTargetPair *> *returnMessages = new std::vector<MsgTargetPair *>();

   //________________This section sets the award int the list____________________________________
   StandardProgressiveWins progWins;
   shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());    //This one is used to Add to progWins
   award->SetAwardAmountHit(amountHit);
   award->SetLevelId(levelID);
   award->SetSASLevelId(levelID);
   award->SetAwardState(SentToSystem);
   award->SetHitReceived(false);   //YES signaled
   award->setEgmPaidSignaled(false);       //NOT signaled
   award->setHandpayPendingSignaled(false); //NOT signaled
   progWins.AddProgressiveWin(award);      //Add to progWins

   //Act
   progWins.SetHandPayPending(amountHit, levelID, returnMessages);

   //Assert
   ASSERT_EQ(award->isHandpayPendingSignaled(), true);
   ASSERT_EQ(award->GetAwardState(), HandpayPending);      //Make sure it is in a pending state.
   ASSERT_TRUE(targetMsg == nullptr);
   ASSERT_EQ(target, mqNone);

   if (nullptr != targetMsg) delete targetMsg;

   delete returnMessages;
}

/// <summary>
/// Code coverage and functional test for:
/// StandardProgressiveWins::SetHandPayReset
/// </summary>
TEST_F(StandardProgressiveWinsTests, StandardProgressive_SetHandPayReset_Hit_Received_HandPay_Pending)
{
   //Arrange
   CGames games;
   __int64 amountHit = 12398; //123.98
   int levelID = 9;
   __int64 jackpotID = 5;
   std::vector<MsgTargetPair *> *returnMessages = new std::vector<MsgTargetPair *>();

   //________________This section sets the award int the list____________________________________
   StandardProgressiveWins progWins;
   shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());    //This one is used to Add to progWins
   award->SetAwardAmountHit(amountHit);
   award->SetLevelId(levelID);
   award->SetSASLevelId(levelID);
   award->SetAwardState(HandpayPending);
   award->SetHitReceived(true);
   award->setEgmPaidSignaled(false);
   award->setHandpayPendingSignaled(true);
   progWins.AddProgressiveWin(award);      //Add to progWins

   //Act
   progWins.SetHandPayReset(returnMessages);

   //Assert
   ASSERT_EQ((int)returnMessages->size(), 1);

   for (auto awardIterator = returnMessages->cbegin(); awardIterator != returnMessages->cend(); /*no need to in iterator*/)
   {
      ASSERT_EQ((*awardIterator)->msg->GetEGMAward()->isEgmPaidSignaled(), true);
      ASSERT_EQ((*awardIterator)->msg->GetEGMAward()->GetAwardState(), AwardToGameSuccess);
      ASSERT_EQ((*awardIterator)->msg->GetMessageType(), INT_MSG_EGM_AWARD);          //Make sure we are sending an EGM award back.
      ASSERT_EQ((*awardIterator)->target, mqProgressive);                                                             //Make sure we are sending it to OneLink.
      ASSERT_EQ(progWins.HasUnsentProgressiveWins(), false);

      if (nullptr != (*awardIterator)->msg)
      {
         delete (*awardIterator)->msg; //Delete the message that was created
      }
      awardIterator = returnMessages->erase(awardIterator);   //remove the item from the vector and repair the iterator
   }
   delete returnMessages;
}

/// <summary>
/// Code coverage and functional test for:
/// StandardProgressiveWins::SetHandPayReset
/// </summary>
TEST_F(StandardProgressiveWinsTests, StandardProgressive_SetHandPayReset_Hit_Received_HandPay_Pending_More_Than_One)
{
   //Arrange
   CGames games;
   __int64 amountHit = 12398; //123.98
   int levelID = 9;
   __int64 amountHit2 = 89321; //893.21
   int levelID2 = 2;

   __int64 jackpotID = 5;
   std::vector<MsgTargetPair *> *returnMessages = new std::vector<MsgTargetPair *>();

   //________________This section sets the award int the list____________________________________
   StandardProgressiveWins progWins;
   shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());    //This one is used to Add to progWins
   award->SetAwardAmountHit(amountHit);
   award->SetLevelId(levelID);
   award->SetSASLevelId(levelID);
   award->SetAwardState(HandpayPending);
   award->SetHitReceived(true);
   award->setEgmPaidSignaled(false);
   award->setHandpayPendingSignaled(true);
   progWins.AddProgressiveWin(award);      //Add to progWins

   shared_ptr<CEGMAward>award2 = shared_ptr<CEGMAward>(new CEGMAward());   //This one is used to Add to progWins
   award2->SetAwardAmountHit(amountHit);
   award2->SetLevelId(levelID);
   award2->SetSASLevelId(levelID);
   award2->SetAwardState(HandpayPending);
   award2->SetHitReceived(true);
   award2->setEgmPaidSignaled(false);
   award2->setHandpayPendingSignaled(true);
   progWins.AddProgressiveWin(award2);     //Add to progWins

   //Act
   progWins.SetHandPayReset(returnMessages);

   //Assert
   ASSERT_EQ((int)returnMessages->size(), 2);

   for (auto awardIterator = returnMessages->cbegin(); awardIterator != returnMessages->cend(); /*no need to in iterator*/)
   {
      ASSERT_EQ((*awardIterator)->msg->GetEGMAward()->isEgmPaidSignaled(), true);
      ASSERT_EQ((*awardIterator)->msg->GetEGMAward()->GetAwardState(), AwardToGameSuccess);
      ASSERT_EQ((*awardIterator)->msg->GetMessageType(), INT_MSG_EGM_AWARD);          //Make sure we are sending an EGM award back.
      ASSERT_EQ((*awardIterator)->target, mqProgressive);                                                             //Make sure we are sending it to OneLink.
      ASSERT_EQ(progWins.HasUnsentProgressiveWins(), false);

      if (nullptr != (*awardIterator)->msg)
      {
         delete (*awardIterator)->msg; //Delete the message that was created
      }
      awardIterator = returnMessages->erase(awardIterator);   //remove the item from the vector and repair the iterator
   }
   delete returnMessages;
}

/// <summary>
/// Code coverage and functional test for:
/// StandardProgressiveWins::SetHandPayReset
/// </summary>
TEST_F(StandardProgressiveWinsTests, StandardProgressive_SetHandPayReset_Hit_NOT_Received_HandPay_Pending)
{
   //Arrange
   CGames games;
   __int64 amountHit = 12398; //123.98
   int levelID = 9;
   __int64 jackpotID = 5;
   std::vector<MsgTargetPair *> *returnMessages = new std::vector<MsgTargetPair *>();

   //________________This section sets the award int the list____________________________________
   StandardProgressiveWins progWins;
   shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());    //This one is used to Add to progWins
   award->SetAwardAmountHit(amountHit);
   award->SetLevelId(levelID);
   award->SetSASLevelId(levelID);
   award->SetAwardState(HandpayPending);
   award->SetHitReceived(false);
   award->setEgmPaidSignaled(false);
   award->setHandpayPendingSignaled(true);
   progWins.AddProgressiveWin(award);      //Add to progWins

   //Act
   progWins.SetHandPayReset(returnMessages);

   //Assert
   ASSERT_EQ((int)returnMessages->size(), 0);
   ASSERT_EQ(award->isEgmPaidSignaled(), true);
   ASSERT_EQ(progWins.HasUnsentProgressiveWins(), true);

   for (auto awardIterator = returnMessages->cbegin(); awardIterator != returnMessages->cend(); /*no need to in iterator*/)
   {
      if (nullptr != (*awardIterator)->msg)
      {
         delete (*awardIterator)->msg; //Delete the message that was created
      }
      awardIterator = returnMessages->erase(awardIterator);   //remove the item from the vector and repair the iterator
   }
   delete returnMessages;
}

/// <summary>
/// Code coverage and functional test for:
/// StandardProgressiveWins::SetHandPayReset
/// </summary>
TEST_F(StandardProgressiveWinsTests, StandardProgressive_SetHandPayReset_No_Pending_In_List)
{
   //Arrange
   StandardProgressiveWins progWins;
   std::vector<MsgTargetPair *> *returnMessages = new std::vector<MsgTargetPair *>();

   //Act
   progWins.SetHandPayReset(returnMessages);

   //Assert
   ASSERT_EQ((int)returnMessages->size(), 0);

   for (auto awardIterator = returnMessages->cbegin(); awardIterator != returnMessages->cend(); /*no need to in iterator*/)
   {
      if (nullptr != (*awardIterator)->msg)
      {
         delete (*awardIterator)->msg; //Delete the message that was created
      }
      awardIterator = returnMessages->erase(awardIterator);   //remove the item from the vector and repair the iterator
   }
   delete returnMessages;
}

/// <summary>
/// Code coverage and functional test for:
/// StandardProgressiveWins::SetProgressivePaidByEGM
/// </summary>
TEST_F(StandardProgressiveWinsTests, StandardProgressive_SetProgressivePaidByEGM_Hit_Received)
{
   //Arrange
   __int64 amountHit = 12398; //123.98
   int levelID = 9;
   __int64 jackpotID = 5;
   std::vector<MsgTargetPair *> *returnMessages = new std::vector<MsgTargetPair *>();

   //________________This section sets the award int the list____________________________________
   StandardProgressiveWins progWins;
   shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());    //This one is used to Add to progWins
   award->SetAwardAmountHit(amountHit);
   award->SetLevelId(levelID);
   award->SetJackpotId(jackpotID);
   award->SetAwardState(SentToSystem);
   award->SetHitReceived(true);
   award->setEgmPaidSignaled(false);
   award->setHandpayPendingSignaled(false);
   progWins.AddProgressiveWin(award);      //Add to progWins

   //Act
   progWins.SetProgressivePaidByEGM(award, returnMessages);

   //Assert
   ASSERT_EQ((int)returnMessages->size(), 1);

   for (auto awardIterator = returnMessages->cbegin(); awardIterator != returnMessages->cend(); /*no need to in iterator*/)
   {
      ASSERT_EQ((*awardIterator)->msg->GetEGMAward()->isEgmPaidSignaled(), true);
      ASSERT_EQ((*awardIterator)->msg->GetMessageType(), INT_MSG_EGM_AWARD);          //Make sure we are sending an EGM award back.
      ASSERT_EQ((*awardIterator)->target, mqProgressive);                                                             //Make sure we are sending it to OneLink.
      ASSERT_EQ((*awardIterator)->msg->GetEGMAward()->GetAwardState(), AwardToGameSuccess);   //Make sure it is in a received from system state.
      ASSERT_EQ((*awardIterator)->msg->GetEGMAward()->GetAwardAmountHit(), amountHit);                //Make sure it is the proper amount.
      ASSERT_EQ((*awardIterator)->msg->GetEGMAward()->GetLevelId(), levelID);                         //Make sure it is the proper level.
      ASSERT_EQ((*awardIterator)->msg->GetEGMAward()->GetJackpotId(), jackpotID);                     //Make sure it is the proper Jackpot ID.
      ASSERT_EQ(progWins.HasUnsentProgressiveWins(), false);

      if (nullptr != (*awardIterator)->msg)
      {
         delete (*awardIterator)->msg; //Delete the message that was created
      }
      awardIterator = returnMessages->erase(awardIterator);   //remove the item from the vector and repair the iterator
   }
   delete returnMessages;
}

/// <summary>
/// Code coverage and functional test for:
/// StandardProgressiveWins::SetProgressivePaidByEGM
/// </summary>
TEST_F(StandardProgressiveWinsTests, StandardProgressive_SetProgressivePaidByEGM_No_Hit_Received)
{
   //Arrange
   __int64 amountHit = 12398; //123.98
   int levelID = 9;
   __int64 jackpotID = 5;
   std::vector<MsgTargetPair *> *returnMessages = new std::vector<MsgTargetPair *>();

   //________________This section sets the award int the list____________________________________
   StandardProgressiveWins progWins;
   shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());    //This one is used to Add to progWins
   award->SetAwardAmountHit(amountHit);
   award->SetLevelId(levelID);
   award->SetJackpotId(jackpotID);
   award->SetAwardState(SentToSystem);
   award->SetHitReceived(false);
   award->setEgmPaidSignaled(false);
   award->setHandpayPendingSignaled(false);
   progWins.AddProgressiveWin(award);      //Add to progWins

   //Act
   progWins.SetProgressivePaidByEGM(award, returnMessages);

   //Assert
   ASSERT_EQ((int)returnMessages->size(), 0);
   ASSERT_EQ(award->isEgmPaidSignaled(), true);
   ASSERT_EQ(progWins.HasUnsentProgressiveWins(), true);

   for (auto awardIterator = returnMessages->cbegin(); awardIterator != returnMessages->cend(); /*no need to in iterator*/)
   {
      if (nullptr != (*awardIterator)->msg)
      {
         delete (*awardIterator)->msg; //Delete the message that was created
      }
      awardIterator = returnMessages->erase(awardIterator);   //remove the item from the vector and repair the iterator
   }
   delete returnMessages;
}

/// <summary>
/// Code coverage and functional test for:
/// StandardProgressiveWins::SetProgressivePaidByEGM
/// </summary>
TEST_F(StandardProgressiveWinsTests, StandardProgressive_SetProgressivePaidByEGM_No_awards_in_List)
{
   //Arrange
   StandardProgressiveWins progWins;
   shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());    //This one is used to Add to progWins
   std::vector<MsgTargetPair *> *returnMessages = new std::vector<MsgTargetPair *>();

   //Act
   progWins.SetProgressivePaidByEGM(award, returnMessages);

   //Assert
   // The following tests dead code left in StandardProgressiveWins::SetProgressivePaidByEGM().
   // I am not sure what purpose it served but for Accounting only on some EGMs it was causing an
   // endless loop of doing a LPx84. The code was commented out instead of removing so when this is refactored we can determine
   // what's its purpose was. AGS 10/28/15
   ASSERT_EQ((int)returnMessages->size(), 0);
   // ASSERT_EQ((int)returnMessages->size(), 1);

   for (auto awardIterator = returnMessages->cbegin(); awardIterator != returnMessages->cend(); /*no need to in iterator*/)
   {
      ASSERT_EQ((*awardIterator)->msg->GetMessageType(), INT_MSG_GET_PROGRESSIVE_HOPPER_PAID_INFO); //Make sure it told gcf to do an 84.
      ASSERT_EQ((*awardIterator)->target, mqGCF);

      if (nullptr != (*awardIterator)->msg)
      {
         delete (*awardIterator)->msg; //Delete the message that was created
      }
      awardIterator = returnMessages->erase(awardIterator);   //remove the item from the vector and repair the iterator
   }
   delete returnMessages;
}

// Need to fix friend class issues
/// <summary>
/// Code coverage and functional test for:
/// StandardProgressiveWins::SetProgressiveWin
/// StandardProgressiveWins::AddProgressiveWin
/// </summary>
TEST_F(StandardProgressiveWinsTests, StandardProgressive_SetProgressiveWin_Not_Duplicate)
{
#if 000
   //Arrange
   StandardProgressiveWinsFriend progWins;
   __int64 amountHit = 12398; //123.98
   int levelID = 9;
   __int64 jackpotID = 5;
   string test = "Test";

   ProgressiveLevelInfo levinfo(levelID, ProgressiveLevelType::Standard, test.c_str(), test.c_str());
   std::map<std::string, CSlotsContentFile> slotsContentFiles;
   CProgressiveLevelFriend progressiveLevel(levinfo, 0, slotsContentFiles);

   std::vector<MsgTargetPair *> returnMessages;

   CEGMAward *progressiveWin = new CEGMAward();
   progressiveWin->SetAwardAmountHit(amountHit);
   progressiveWin->SetSASLevelId(levelID);
   progressiveWin->SetAwardState(ReceivedFromGame);
   progressiveWin->SetHitReceived(false);
   progressiveWin->setEgmPaidSignaled(false);
   progressiveWin->setHandpayPendingSignaled(false);

   CInternalMessage msg(INT_MSG_PROGRESSIVE_WIN, progressiveWin);

   //Act
   progWins.SetProgressiveWin(&msg, &returnMessages, progressiveLevel.GetID());

   //Assert
   ASSERT_EQ((size_t)1, returnMessages.size());

   for (auto awardIterator = returnMessages.begin(); awardIterator != returnMessages.end(); /*no need to in iterator*/)
   {
      ASSERT_EQ((*awardIterator)->msg->GetEGMAward()->GetLevelId(), levelID);
      ASSERT_EQ((*awardIterator)->msg->GetEGMAward()->GetAwardAmountHit(), amountHit);
      ASSERT_EQ((*awardIterator)->msg->GetMessageType(), INT_MSG_PROGRESSIVE_WIN);            //Make sure we are sending an EGM award back.
      ASSERT_EQ((*awardIterator)->target, mqProgressive);                                                             //Make sure we are sending it to OneLink.
      ASSERT_EQ(progWins.HasUnsentProgressiveWins(), true);

      if (nullptr != (*awardIterator)->msg)
      {
         delete (*awardIterator)->msg; //Delete the message that was created
      }
      awardIterator = returnMessages.erase(awardIterator);    //remove the item from the vector and repair the iterator
   }
#endif
}

/// <summary>
/// Code coverage and functional test for:
/// StandardProgressiveWins::SetProgressiveWin
/// StandardProgressiveWins::AddProgressiveWin
/// </summary>
TEST_F(StandardProgressiveWinsTests, StandardProgressive_SetProgressiveWin_Duplicate)
{
   //Arrange
   StandardProgressiveWins progWins;
   __int64 amountHit = 12398; //123.98
   int levelID = 9;
   __int64 jackpotID = 5;
   string test = "Test";

   CProgressiveLevelFriend progressiveLevel;

   progressiveLevel.SetID(levelID);
   progressiveLevel.SetName(test);
   progressiveLevel.SetURL(test);

   std::vector<MsgTargetPair *> returnMessages;

   shared_ptr<CEGMAward>progressiveWin = shared_ptr<CEGMAward>(new CEGMAward());   //This one is used to Add to progWins
   progressiveWin->SetAwardAmountHit(amountHit);
   progressiveWin->SetSASLevelId(levelID);
   progressiveWin->SetAwardState(ReceivedFromGame);
   progressiveWin->SetHitReceived(false);
   progressiveWin->setEgmPaidSignaled(false);
   progressiveWin->setHandpayPendingSignaled(false);
   progWins.AddProgressiveWin(progressiveWin);

   CInternalMessage msg(INT_MSG_PROGRESSIVE_WIN, *progressiveWin);

   //Act
   progWins.SetProgressiveWin(&msg, &returnMessages, progressiveLevel.GetID());
   msg.SetData((CEGMAward *)nullptr);

   //Assert
   ASSERT_EQ((size_t)0, returnMessages.size()) << "returnMessages.size()";
   ASSERT_TRUE(progWins.HasUnsentProgressiveWins()) << "progWins.HasUnsentProgressiveWins()";

   for (auto awardIterator = returnMessages.begin(); awardIterator != returnMessages.end(); /*no need to in iterator*/)
   {
      if (nullptr != (*awardIterator)->msg)
      {
         delete (*awardIterator)->msg; //Delete the message that was created
      }
      awardIterator = returnMessages.erase(awardIterator);    //remove the item from the vector and repair the iterator
   }
}

/// <summary>
/// Code coverage and functional test for:
/// StandardProgressiveWins::SetProgressiveWin
/// StandardProgressiveWins::AddProgressiveWin
/// </summary>
TEST_F(StandardProgressiveWinsTests, StandardProgressive_SetProgressiveWin_Duplicate_EGM_Paid)
{
   //Arrange
   StandardProgressiveWins progWins;
   __int64 amountHit = 12398; //123.98
   int levelID = 9;
   __int64 jackpotID = 5;
   string test = "Test";

   CProgressiveLevelFriend progressiveLevel;

   progressiveLevel.SetID(levelID);
   progressiveLevel.SetName(test);
   progressiveLevel.SetURL(test);

   std::vector<MsgTargetPair *> returnMessages;

   CEGMAward *progressiveWin = new CEGMAward();    //This one is used to Add to progWins
   progressiveWin->SetAwardAmountHit(amountHit);
   progressiveWin->SetSASLevelId(levelID);
   progressiveWin->SetAwardState(ReceivedFromGame);
   progressiveWin->SetHitReceived(true);
   progressiveWin->setEgmPaidSignaled(true);
   progressiveWin->setHandpayPendingSignaled(false);

   shared_ptr<CEGMAward>progressiveWinDupe = shared_ptr<CEGMAward>(new CEGMAward(*progressiveWin));        //This one is used to Add to progWins
   progWins.AddProgressiveWin(progressiveWinDupe);

   CInternalMessage msg(INT_MSG_PROGRESSIVE_WIN, progressiveWin);

   //Act
   progWins.SetProgressiveWin(&msg, &returnMessages, progressiveLevel.GetID());

   //Assert
   ASSERT_EQ((size_t)1, returnMessages.size());

   for (auto awardIterator = returnMessages.begin(); awardIterator != returnMessages.end(); /*no need to in iterator*/)
   {
      ASSERT_EQ((*awardIterator)->msg->GetEGMAward()->GetSASLevelId(), levelID);
      ASSERT_EQ((*awardIterator)->msg->GetEGMAward()->GetAwardAmountHit(), amountHit);
      ASSERT_EQ((*awardIterator)->msg->GetMessageType(), INT_MSG_EGM_AWARD);
      ASSERT_EQ((*awardIterator)->target, mqProgressive);                                                             //Make sure we are sending it to OneLink.
      ASSERT_EQ((*awardIterator)->msg->GetEGMAward()->GetAwardState(), AwardToGameSuccess);
      ASSERT_EQ(progWins.HasUnsentProgressiveWins(), false);

      if (nullptr != (*awardIterator)->msg)
      {
         delete (*awardIterator)->msg; //Delete the message that was created
      }
      awardIterator = returnMessages.erase(awardIterator);    //remove the item from the vector and repair the iterator
   }
}

/// <summary>
/// Code coverage and functional test for:
/// StandardProgressiveWins::SetProgressiveWin
/// StandardProgressiveWins::AddProgressiveWin
/// </summary>
TEST_F(StandardProgressiveWinsTests, StandardProgressive_SetProgressiveWin_No_Level)
{
   //Arrange
   StandardProgressiveWins progWins;
   __int64 amountHit = 12398; //123.98
   int levelID = 9;
   __int64 jackpotID = 5;

   std::vector<MsgTargetPair *> returnMessages;

   CEGMAward *progressiveWin = new CEGMAward();
   progressiveWin->SetAwardAmountHit(amountHit);
   progressiveWin->SetSASLevelId(levelID);
   progressiveWin->SetAwardState(ReceivedFromGame);
   progressiveWin->SetHitReceived(false);
   progressiveWin->setEgmPaidSignaled(false);
   progressiveWin->setHandpayPendingSignaled(false);

   CInternalMessage msg(INT_MSG_PROGRESSIVE_WIN, progressiveWin);

   //Act
   progWins.SetProgressiveWin(&msg, &returnMessages, InvalidProgressiveLevelId);

   //Assert
   ASSERT_EQ((size_t)0, returnMessages.size());
   ASSERT_EQ(progWins.HasUnsentProgressiveWins(), false);

   for (auto awardIterator = returnMessages.begin(); awardIterator != returnMessages.end(); /*no need to in iterator*/)
   {
      if (nullptr != (*awardIterator)->msg)
      {
         delete (*awardIterator)->msg; //Delete the message that was created
      }
      awardIterator = returnMessages.erase(awardIterator);    //remove the item from the vector and repair the iterator
   }
}

/// <summary>
/// Code coverage and functional test for:
/// StandardProgressiveWins::IsThisADuplicate
/// StandardProgressiveWins::AddProgressiveWin
/// </summary>
TEST_F(StandardProgressiveWinsTests, StandardProgressive_IsThisADuplicate_Dupe)
{
   //Arrange
   StandardProgressiveWins progWins;
   __int64 amountHit = 12398; //123.98
   int levelID = 9;

   shared_ptr<CEGMAward>progressiveWin = shared_ptr<CEGMAward>(new CEGMAward());
   progressiveWin->SetAwardAmountHit(amountHit);
   progressiveWin->SetSASLevelId(levelID);
   progressiveWin->SetAwardState(ReceivedFromGame);
   progressiveWin->SetHitReceived(false);
   progressiveWin->setEgmPaidSignaled(false);
   progressiveWin->setHandpayPendingSignaled(false);
   progWins.AddProgressiveWin(progressiveWin);

   //Act
   bool isDupe = progWins.IsThisADuplicate(amountHit, levelID);

   //Assert
   ASSERT_EQ(true, isDupe);
}

/// <summary>
/// Code coverage and functional test for:
/// StandardProgressiveWins::IsThisADuplicate
/// StandardProgressiveWins::AddProgressiveWin
/// </summary>
TEST_F(StandardProgressiveWinsTests, StandardProgressive_IsThisADuplicate_NOT_Dupe)
{
   //Arrange
   StandardProgressiveWins progWins;
   __int64 amountHit = 12398; //123.98
   int levelID = 9;
   __int64 amountHit2 = 22398; //123.98
   int levelID2 = 2;

   shared_ptr<CEGMAward>progressiveWin = shared_ptr<CEGMAward>(new CEGMAward());
   progressiveWin->SetAwardAmountHit(amountHit);
   progressiveWin->SetSASLevelId(levelID);
   progressiveWin->SetAwardState(ReceivedFromGame);
   progressiveWin->SetHitReceived(false);
   progressiveWin->setEgmPaidSignaled(false);
   progressiveWin->setHandpayPendingSignaled(false);
   progWins.AddProgressiveWin(progressiveWin);

   //Act
   bool isDupe = progWins.IsThisADuplicate(amountHit2, levelID2);

   //Assert
   ASSERT_FALSE(isDupe);
}

// Need to fix friend class issues
/// <summary>
/// Code coverage and functional test for:
/// StandardProgressiveWins::IsThisADuplicate
/// StandardProgressiveWins::AddProgressiveWin
/// </summary>
TEST_F(StandardProgressiveWinsTests, StandardProgressive_IsThisADuplicate_Dupe_Last_Award)
{
   //Arrange
   StandardProgressiveWinsFriend progWins;
   __int64 amountHit = 12398; //123.98
   int levelID = 9;

   shared_ptr<CEGMAward>progressiveWin = shared_ptr<CEGMAward>(new CEGMAward());
   progressiveWin->SetAwardAmountHit(amountHit);
   progressiveWin->SetSASLevelId(levelID);
   progressiveWin->SetAwardState(ReceivedFromGame);
   progressiveWin->SetHitReceived(false);
   progressiveWin->setEgmPaidSignaled(false);
   progressiveWin->setHandpayPendingSignaled(false);

   //Act
   progWins.AddProgressiveWin(progressiveWin);
   progWins.RemoveAward(progressiveWin->GetAwardAmountHit(), progressiveWin->GetLevelId());

   bool isDupe = progWins.IsThisADuplicate(amountHit, levelID);

   //Assert
   ASSERT_EQ(true, isDupe);
}

/// <summary>
/// Code coverage and functional test for:
/// StandardProgressiveWins::SetProgressiveWin
/// StandardProgressiveWins::AddProgressiveWin
/// </summary>
TEST_F(StandardProgressiveWinsTests, StandardProgressive_Double_Hit_Do_Both_Still_Exist)
{
   //Arrange
   StandardProgressiveWins progWins;
   std::vector<MsgTargetPair *> returnMessages;

   __int64 amountHit1 = 12398; //123.98
   int levelID1 = 9;
   __int64 jackpotID1 = 5;

   __int64 amountHit2 = 3456; //34.56
   int levelID2 = 8;
   __int64 jackpotID2 = 6;

   string test = "Test";

   CProgressiveLevelFriend progressiveLevel1;

   progressiveLevel1.SetID(levelID1);
   progressiveLevel1.SetName(test);
   progressiveLevel1.SetURL(test);

   CProgressiveLevelFriend progressiveLevel2;

   progressiveLevel2.SetID(levelID2);
   progressiveLevel2.SetName(test);
   progressiveLevel2.SetURL(test);

   CEGMAward *progressiveWin1 = new CEGMAward();
   progressiveWin1->SetAwardAmountHit(amountHit1);
   progressiveWin1->SetSASLevelId(levelID1);
   progressiveWin1->SetAwardState(ReceivedFromGame);
   progressiveWin1->SetHitReceived(false);
   progressiveWin1->setEgmPaidSignaled(false);
   progressiveWin1->setHandpayPendingSignaled(false);

   CInternalMessage msg(INT_MSG_PROGRESSIVE_WIN, progressiveWin1);

   CEGMAward *progressiveWin2 = new CEGMAward();
   progressiveWin2->SetAwardAmountHit(amountHit2);
   progressiveWin2->SetSASLevelId(levelID2);
   progressiveWin2->SetAwardState(ReceivedFromGame);
   progressiveWin2->SetHitReceived(false);
   progressiveWin2->setEgmPaidSignaled(false);
   progressiveWin2->setHandpayPendingSignaled(false);

   CInternalMessage msg2(INT_MSG_PROGRESSIVE_WIN, progressiveWin2);

   //Act
   progWins.SetProgressiveWin(&msg, &returnMessages, progressiveLevel1.GetID());
   progWins.SetProgressiveWin(&msg2, &returnMessages, progressiveLevel2.GetID());

   bool found1 = progWins.IsThisADuplicate(amountHit1, levelID1);
   bool found2 = progWins.IsThisADuplicate(amountHit2, levelID2);

   //Assert
   ASSERT_EQ(true, found1);
   ASSERT_EQ(true, found2);
   ASSERT_EQ((size_t)2, returnMessages.size());
   int count = 1;

   for (auto awardIterator = returnMessages.begin(); awardIterator != returnMessages.end(); /*no need to in iterator*/)
   {
      if (count == 1)
      {
         ASSERT_EQ((*awardIterator)->msg->GetEGMAward()->GetLevelId(), levelID1);
         ASSERT_EQ((*awardIterator)->msg->GetEGMAward()->GetAwardAmountHit(), amountHit1);
         count++;
      }
      else
      {
         ASSERT_EQ((*awardIterator)->msg->GetEGMAward()->GetLevelId(), levelID2);
         ASSERT_EQ((*awardIterator)->msg->GetEGMAward()->GetAwardAmountHit(), amountHit2);
      }

      ASSERT_EQ((*awardIterator)->msg->GetMessageType(), INT_MSG_PROGRESSIVE_WIN);            //Make sure we are sending an EGM award back.
      ASSERT_EQ((*awardIterator)->target, mqProgressive);                                                             //Make sure we are sending it to OneLink.
      ASSERT_EQ(progWins.HasUnsentProgressiveWins(), true);

      if (nullptr != (*awardIterator)->msg)
      {
         delete (*awardIterator)->msg; //Delete the message that was created
      }
      awardIterator = returnMessages.erase(awardIterator);    //remove the item from the vector and repair the iterator
   }
}

/// <summary>
/// Code coverage and functional test for:
/// StandardProgressiveWins::Pulse
/// </summary>
TEST_F(StandardProgressiveWinsTests, StandardProgressive_Pulse_ProgUP_With_Awards_Elapse)
{
   //Arrange
   CGames games;
   __int64 amountHit = 12398; //123.98
   int levelID = 9;
   __int64 jackpotID = 5;
   ProgressiveWinsMock mock;
   std::vector<MsgTargetPair *> *returnMessages = new std::vector<MsgTargetPair *>();

   //________________This section sets the award int the list____________________________________
   StandardProgressiveWins progWins;
   shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());    //This one is used to Add to progWins
   award->SetAwardAmountHit(amountHit);
   award->SetLevelId(levelID);
   award->SetSASLevelId(levelID);
   award->SetAwardState(HandpayPending);
   award->SetHitReceived(true);
   award->setEgmPaidSignaled(false);
   award->setHandpayPendingSignaled(true);
   progWins.AddProgressiveWin(award);      //Add to progWins

   //Act
   DWORD tickCount = CUtilities::GetTickCount();
   progWins.Pulse(tickCount, false, returnMessages); //pulse once to prime
   returnMessages->clear();
   tickCount -= (15 * MILLISECONDS_PER_SECOND);
   progWins.Pulse(tickCount, false, returnMessages);

   //Assert
   ASSERT_EQ((int)returnMessages->size(), 1);
   ASSERT_TRUE(Mock::VerifyAndClearExpectations(&mock));                                                   //Required for GMOCK to check calls

   for (auto awardIterator = returnMessages->cbegin(); awardIterator != returnMessages->cend(); /*no need to in iterator*/)
   {
      if (nullptr != (*awardIterator)->msg)
      {
         delete (*awardIterator)->msg; //Delete the message that was created
      }
      awardIterator = returnMessages->erase(awardIterator);   //remove the item from the vector and repair the iterator
   }
   delete returnMessages;
}

/// <summary>
/// Code coverage and functional test for:
/// StandardProgressiveWins::Pulse
/// </summary>
TEST_F(StandardProgressiveWinsTests, StandardProgressive_Pulse_ProgUP_With_Awards_No_Elapse)
{
   //Arrange
   CGames games;
   __int64 amountHit = 12398; //123.98
   int levelID = 9;
   __int64 jackpotID = 5;
   ProgressiveWinsMock mock;
   std::vector<MsgTargetPair *> *returnMessages = new std::vector<MsgTargetPair *>();

   //________________This section sets the award int the list____________________________________
   StandardProgressiveWins progWins;
   shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());    //This one is used to Add to progWins
   award->SetAwardAmountHit(amountHit);
   award->SetLevelId(levelID);
   award->SetSASLevelId(levelID);
   award->SetAwardState(HandpayPending);
   award->SetHitReceived(true);
   award->setEgmPaidSignaled(false);
   award->setHandpayPendingSignaled(true);
   progWins.AddProgressiveWin(award);      //Add to progWins

   //Act
   DWORD tickCount = CUtilities::GetTickCount();
   progWins.Pulse(tickCount, false, returnMessages); //pulse once to prime
   returnMessages->clear(); //clear list
   progWins.Pulse(tickCount, false, returnMessages);

   //Assert
   ASSERT_EQ((int)returnMessages->size(), 0);
   ASSERT_TRUE(Mock::VerifyAndClearExpectations(&mock)); //Required for GMOCK to check calls

   for (auto awardIterator = returnMessages->cbegin(); awardIterator != returnMessages->cend(); /*no need to in iterator*/)
   {
      if (nullptr != (*awardIterator)->msg)
      {
         delete (*awardIterator)->msg; //Delete the message that was created
      }
      awardIterator = returnMessages->erase(awardIterator);   //remove the item from the vector and repair the iterator
   }
   delete returnMessages;
}

/// <summary>
/// Code coverage and functional test for:
/// StandardProgressiveWins::Pulse
/// </summary>
TEST_F(StandardProgressiveWinsTests, StandardProgressive_Pulse_ProgDOWN_With_Awards)
{
   //Arrange
   CGames games;
   __int64 amountHit = 12398; //123.98
   int levelID = 9;
   __int64 jackpotID = 5;
   ProgressiveWinsMock mock;
   std::vector<MsgTargetPair *> *returnMessages = new std::vector<MsgTargetPair *>();

   DWORD tickCount = CUtilities::GetTickCount() - (15 * MILLISECONDS_PER_SECOND);

   //________________This section sets the award int the list____________________________________
   StandardProgressiveWins progWins;
   shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());    //This one is used to Add to progWins
   award->SetAwardAmountHit(amountHit);
   award->SetLevelId(levelID);
   award->SetSASLevelId(levelID);
   award->SetAwardState(HandpayPending);
   award->SetHitReceived(true);
   award->setEgmPaidSignaled(false);
   award->setHandpayPendingSignaled(true);
   progWins.AddProgressiveWin(award);      //Add to progWins

   //Act
   progWins.Pulse(tickCount, true, returnMessages);

   //Assert
   ASSERT_EQ((int)returnMessages->size(), 0);
   ASSERT_TRUE(Mock::VerifyAndClearExpectations(&mock));                                                   //Required for GMOCK to check calls

   for (auto awardIterator = returnMessages->cbegin(); awardIterator != returnMessages->cend(); /*no need to in iterator*/)
   {
      if (nullptr != (*awardIterator)->msg)
      {
         delete (*awardIterator)->msg; //Delete the message that was created
      }
      awardIterator = returnMessages->erase(awardIterator);   //remove the item from the vector and repair the iterator
   }
   delete returnMessages;
}

/// <summary>
/// Code coverage and functional test for:
/// StandardProgressiveWins::Pulse
/// </summary>
TEST_F(StandardProgressiveWinsTests, StandardProgressive_Pulse_ProgUP_With_NO_Awards)
{
   //Arrange
   CGames games;
   __int64 amountHit = 12398; //123.98
   int levelID = 9;
   __int64 jackpotID = 5;
   ProgressiveWinsMock mock;
   std::vector<MsgTargetPair *> *returnMessages = new std::vector<MsgTargetPair *>();

   DWORD tickCount = CUtilities::GetTickCount() - (15 * MILLISECONDS_PER_SECOND);
   StandardProgressiveWins progWins;

   //Act
   progWins.Pulse(tickCount, true, returnMessages);

   //Assert
   ASSERT_EQ((int)returnMessages->size(), 0);
   ASSERT_TRUE(Mock::VerifyAndClearExpectations(&mock));                                                   //Required for GMOCK to check calls

   for (auto awardIterator = returnMessages->cbegin(); awardIterator != returnMessages->cend(); /*no need to in iterator*/)
   {
      if (nullptr != (*awardIterator)->msg)
      {
         delete (*awardIterator)->msg; //Delete the message that was created
      }
      awardIterator = returnMessages->erase(awardIterator);   //remove the item from the vector and repair the iterator
   }
   delete returnMessages;
}

/// <summary>
/// Code coverage and functional test for:
/// StandardProgressiveWins::BuildAwardResendList
/// </summary>
TEST_F(StandardProgressiveWinsTests, StandardProgressive_BuildAwardResendList_Award_HandPay)
{
   //Arrange
   CGames games;
   __int64 amountHit = 12398; //123.98
   int levelID = 9;
   __int64 jackpotID = 5;
   std::vector<MsgTargetPair *> *returnMessages = new std::vector<MsgTargetPair *>();

   //________________This section sets the award int the list____________________________________
   StandardProgressiveWins progWins;
   shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());    //This one is used to Add to progWins
   award->SetAwardAmountHit(amountHit);
   award->SetLevelId(levelID);
   award->SetSASLevelId(levelID);
   award->SetJackpotId(jackpotID);
   award->SetAwardState(HandpayPending);
   award->SetHitReceived(true);
   award->setEgmPaidSignaled(false);
   award->setHandpayPendingSignaled(true);
   progWins.AddProgressiveWin(award);      //Add to progWins

   //Act
   progWins.BuildAwardResendList(returnMessages);

   //Assert
   ASSERT_EQ((int)returnMessages->size(), 1);

   for (auto awardIterator = returnMessages->cbegin(); awardIterator != returnMessages->cend(); /*no need to in iterator*/)
   {
      ASSERT_EQ((*awardIterator)->msg->GetMessageType(), INT_MSG_EGM_AWARD);          //Make sure we are sending an EGM award back.
      ASSERT_EQ((*awardIterator)->target, mqProgressive);                                                             //Make sure we are sending it to OneLink.
      ASSERT_EQ((*awardIterator)->msg->GetEGMAward()->GetAwardState(), HandpayPending);
      ASSERT_EQ((*awardIterator)->msg->GetEGMAward()->GetAwardAmountHit(), amountHit);                //Make sure it is the proper amount.
      ASSERT_EQ((*awardIterator)->msg->GetEGMAward()->GetLevelId(), levelID);                         //Make sure it is the proper level.
      ASSERT_EQ((*awardIterator)->msg->GetEGMAward()->GetJackpotId(), jackpotID);                     //Make sure it is the proper Jackpot ID.
      ASSERT_EQ(progWins.HasUnsentProgressiveWins(), false);

      if (nullptr != (*awardIterator)->msg)
      {
         delete (*awardIterator)->msg; //Delete the message that was created
      }
      awardIterator = returnMessages->erase(awardIterator);   //remove the item from the vector and repair the iterator
   }
   delete returnMessages;
}

// Need to fix friend class issues
/// <summary>
/// Code coverage and functional test for:
/// StandardProgressiveWins::BuildAwardResendList
/// </summary>
TEST_F(StandardProgressiveWinsTests, StandardProgressive_BuildAwardResendList_SentToSystem_Old)
{
   //Arrange
   CGames games;
   __int64 amountHit = 12398; //123.98
   int levelID = 9;
   __int64 jackpotID = 5;
   std::vector<MsgTargetPair *> *returnMessages = new std::vector<MsgTargetPair *>();

   //________________This section sets the award int the list____________________________________
   StandardProgressiveWins progWins;
   shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());    //This one is used to Add to progWins
   award->SetAwardAmountHit(amountHit);
   award->SetLevelId(levelID);
   award->SetSASLevelId(levelID);

   SYSTEMTIME currentTime;
   time_t utcTime = CUtilities::GetUTCTime() - 600;
   currentTime = CUtilities::GetUTCSystemTime(utcTime);
   award->SetReceivedFromEGMTime(currentTime); //should get it 10 minutes old...

   award->SetAwardState(SentToSystem);
   award->SetHitReceived(false);
   award->setEgmPaidSignaled(false);
   award->setHandpayPendingSignaled(false);
   progWins.AddProgressiveWin(award);      //Add to progWins

   //Act
   progWins.BuildAwardResendList(returnMessages);

   //Assert
   ASSERT_EQ((int)returnMessages->size(), 1);

   for (auto awardIterator = returnMessages->cbegin(); awardIterator != returnMessages->cend(); /*no need to in iterator*/)
   {
      ASSERT_EQ((*awardIterator)->msg->GetMessageType(), INT_MSG_EGM_AWARD);          //Make sure we are sending an EGM award back.
      ASSERT_EQ((*awardIterator)->target, mqProgressive);                                                             //Make sure we are sending it to OneLink.
      ASSERT_EQ((*awardIterator)->msg->GetEGMAward()->GetAwardState(), SentToSystem);
      ASSERT_EQ((*awardIterator)->msg->GetEGMAward()->GetAwardAmountHit(), (__int64)0);               //Make sure it is the proper amount.
      ASSERT_EQ((*awardIterator)->msg->GetEGMAward()->GetLevelId(), 0);                               //Make sure it is the proper level.
      ASSERT_EQ(progWins.HasUnsentProgressiveWins(), false);

      if (nullptr != (*awardIterator)->msg)
      {
         delete (*awardIterator)->msg; //Delete the message that was created
      }
      awardIterator = returnMessages->erase(awardIterator);   //remove the item from the vector and repair the iterator
   }
   delete returnMessages;
}

/// <summary>
/// Code coverage and functional test for:
/// StandardProgressiveWins::BuildAwardResendList
/// </summary>
TEST_F(StandardProgressiveWinsTests, StandardProgressive_BuildAwardResendList_HandPay_And_SentToSystem_Old)
{
   //Arrange
   CGames games;
   __int64 amountHit = 12398; //123.98
   int levelID = 9;
   __int64 amountHit2 = 89321; //893.21
   int levelID2 = 2;
   __int64 jackpotID2 = 5;
   std::vector<MsgTargetPair *> *returnMessages = new std::vector<MsgTargetPair *>();

   //________________This section sets the award int the list____________________________________
   StandardProgressiveWins progWins;
   shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());    //This one is used to Add to progWins
   award->SetAwardAmountHit(amountHit);
   award->SetLevelId(levelID);
   award->SetSASLevelId(levelID);

   SYSTEMTIME currentTime;
   time_t utcTime = CUtilities::GetUTCTime() - 600;
   currentTime = CUtilities::GetUTCSystemTime(utcTime);
   award->SetReceivedFromEGMTime(currentTime); //should get it 10 minutes old...

   award->SetAwardState(SentToSystem);
   award->SetHitReceived(false);
   award->setEgmPaidSignaled(false);
   award->setHandpayPendingSignaled(false);
   progWins.AddProgressiveWin(award);      //Add to progWins

   shared_ptr<CEGMAward>award2 = shared_ptr<CEGMAward>(new CEGMAward());   //This one is used to Add to progWins
   award2->SetAwardAmountHit(amountHit2);
   award2->SetLevelId(levelID2);
   award2->SetSASLevelId(levelID2);
   utcTime = CUtilities::GetUTCTime() - 600;
   currentTime = CUtilities::GetUTCSystemTime(utcTime);
   award2->SetReceivedFromEGMTime(currentTime); //should get it 10 minutes old...
   award2->SetAwardState(HandpayPending);
   award2->SetHitReceived(true);
   award2->setEgmPaidSignaled(false);
   award2->setHandpayPendingSignaled(true);
   progWins.AddProgressiveWin(award2);     //Add to progWins

   //Act
   progWins.BuildAwardResendList(returnMessages);

   //Assert
   ASSERT_EQ((int)returnMessages->size(), 2);

   int counter = 1;
   for (auto awardIterator = returnMessages->cbegin(); awardIterator != returnMessages->cend(); /*no need to in iterator*/)
   {
      if (counter == 1)
      {
         ASSERT_EQ((*awardIterator)->msg->GetMessageType(), INT_MSG_EGM_AWARD);          //Make sure we are sending an EGM award back.
         ASSERT_EQ((*awardIterator)->target, mqProgressive);                                                             //Make sure we are sending it to OneLink.
         ASSERT_EQ((*awardIterator)->msg->GetEGMAward()->GetAwardState(), SentToSystem);
         ASSERT_EQ((*awardIterator)->msg->GetEGMAward()->GetAwardAmountHit(), (__int64)0);               //Make sure it is the proper amount.
         ASSERT_EQ((*awardIterator)->msg->GetEGMAward()->GetLevelId(), 0);                               //Make sure it is the proper level.
         ASSERT_EQ(progWins.HasUnsentProgressiveWins(), false);
         counter++;
      }
      else
      {
         ASSERT_EQ((*awardIterator)->msg->GetMessageType(), INT_MSG_EGM_AWARD);          //Make sure we are sending an EGM award back.
         ASSERT_EQ((*awardIterator)->target, mqProgressive);                                                             //Make sure we are sending it to OneLink.
         ASSERT_EQ((*awardIterator)->msg->GetEGMAward()->GetAwardState(), HandpayPending);
         ASSERT_EQ((*awardIterator)->msg->GetEGMAward()->GetAwardAmountHit(), amountHit2);               //Make sure it is the proper amount.
         ASSERT_EQ((*awardIterator)->msg->GetEGMAward()->GetLevelId(), levelID2);                                //Make sure it is the proper level.
         ASSERT_EQ(progWins.HasUnsentProgressiveWins(), false);
      }

      if (nullptr != (*awardIterator)->msg)
      {
         delete (*awardIterator)->msg; //Delete the message that was created
      }
      awardIterator = returnMessages->erase(awardIterator);   //remove the item from the vector and repair the iterator
   }
   delete returnMessages;
}

// #pragma region NvramMap
/// <summary>
/// Test that the NVRAM memory map has not changed across versions.
/// If the NVRAM memory map does change across versions then memory map upgrades should be tested.
/// </summary>
TEST_F(StandardProgressiveWinsTests, StandardStandardProgressiveWins_NvramMap)
{
   // Test NVRAM memory map offset calculations.
   ASSERT_EQ(NVRAM_PLAYER_SESSION2_DATA_OFFSET + NVRAM_PLAYER_SESSION2_DATA_SIZE,
             NVRAM_EGM_AWARD_STANDARD_CHECKBYTE_OFFSET) << "NVRAM_EGM_AWARD_STANDARD_CHECKBYTE_OFFSET calc";

   // Perform test(s) for forwards and backwards compatibility.
   ASSERT_EQ((DWORD)713739, NVRAM_EGM_AWARD_STANDARD_CHECKBYTE_OFFSET) << "NVRAM_EGM_AWARD_STANDARD_CHECKBYTE_OFFSET";
   ASSERT_EQ((DWORD)32, NVRAM_EGM_AWARD_STANDARD_DATA_COUNT) << "NVRAM_EGM_AWARD_STANDARD_DATA_COUNT";
   ASSERT_EQ((DWORD)257, NVRAM_EGM_AWARD_STANDARD_ITEM_SIZE) << "NVRAM_EGM_AWARD_STANDARD_ITEM_SIZE";
   ASSERT_EQ((DWORD)0x2020, NVRAM_EGM_AWARD_STANDARD_SIZE) << "NVRAM_EGM_AWARD_STANDARD_SIZE";

   ASSERT_EQ((DWORD)1, NVRAM_EGM_AWARD_CHECKBYTE_SIZE) << "NVRAM_EGM_AWARD_CHECKBYTE_SIZE";
   ASSERT_EQ((BYTE)0x1c, NVRAM_EGM_AWARD_CHECKBYTE_VALUE) << "NVRAM_EGM_AWARD_CHECKBYTE_VALUE";
}
// #pragma endregion NvramMap

// #pragma region SetInactiveLocation
/// <summary>
/// Functional test for:
///     StandardProgressiveWins::SetInactiveLocation()
/// Test for correct NVRAM changes and version compatibility.
/// </summary>
TEST_F(StandardProgressiveWinsTests, StandardProgressive_Wins_SetInactiveLocation)
{
   // Setup.
   CUnitTestingUtils::SetNvramValidAndUpgraded();
   CNVRAMConfig nvramConfig;

   // Perform operation(s) and test(s).
   for (DWORD index(0); index < NVRAM_EGM_AWARD_STANDARD_DATA_COUNT; ++index)
   {
      BYTE activeValue(NVRAM_EGM_AWARD_CHECKBYTE_VALUE);
      DWORD nvramActiveOffset(NVRAM_EGM_AWARD_STANDARD_CHECKBYTE_OFFSET + index * NVRAM_EGM_AWARD_STANDARD_ITEM_SIZE);
      nvramConfig.Set(nvramActiveOffset, &activeValue, sizeof(activeValue));
   }
   BYTE setStdActiveValue(NVRAM_LAST_STD_AWARD_CHECKBYTE_VALUE);
   nvramConfig.Set(NVRAM_LAST_STD_AWARD_CHECKBYTE_OFFSET, &setStdActiveValue, sizeof(setStdActiveValue));

   StandardProgressiveWins::SetInactiveLocation();

   for (DWORD index(0); index < NVRAM_EGM_AWARD_STANDARD_DATA_COUNT; ++index)
   {
      BYTE activeValue(NVRAM_EGM_AWARD_CHECKBYTE_VALUE);
      DWORD nvramActiveOffset(NVRAM_EGM_AWARD_STANDARD_CHECKBYTE_OFFSET + index * NVRAM_EGM_AWARD_STANDARD_ITEM_SIZE);
      nvramConfig.Get(nvramActiveOffset, &activeValue, sizeof(activeValue));
      ASSERT_EQ((BYTE)~NVRAM_EGM_AWARD_CHECKBYTE_VALUE, activeValue) << "activeValue";
   }
   BYTE getStdActiveValue(NVRAM_LAST_STD_AWARD_CHECKBYTE_VALUE);
   nvramConfig.Get(NVRAM_LAST_STD_AWARD_CHECKBYTE_OFFSET, &getStdActiveValue, sizeof(getStdActiveValue));
   ASSERT_EQ((BYTE)~NVRAM_LAST_STD_AWARD_CHECKBYTE_VALUE, getStdActiveValue) << "getStdActiveValue";
}
// #pragma endregion SetInactiveLocation

// #pragma region StandardProgressiveWins_Persist
/// <summary>
/// Functional test for:
///     StandardProgressiveWins::StandardProgressiveWins()
/// Test with buildFromNvram = false.
/// </summary>
TEST_F(StandardProgressiveWinsTests, StandardProgressiveWins_NoBuild)
{
   // Perform operation(s) and test(s).
   StandardProgressiveWinsFriend wins(false);

   // Test awards recovered from NVRAM.
   ASSERT_EQ(0, wins.GetAwards().size()) << "m_Awards.size";

   // Test available offsets.
   ASSERT_EQ((size_t)NVRAM_EGM_AWARD_STANDARD_DATA_COUNT, wins.GetAvailableNvramActiveOffsets().size()) << "m_availableNvramActiveOffsets.size()";
   for (DWORD index(0); index < NVRAM_EGM_AWARD_STANDARD_DATA_COUNT; ++index)
   {
      DWORD nvramActiveOffset(NVRAM_EGM_AWARD_STANDARD_CHECKBYTE_OFFSET + index * NVRAM_EGM_AWARD_STANDARD_ITEM_SIZE);

      ASSERT_FALSE(wins.GetAvailableNvramActiveOffsets().empty()) << "m_availableNvramActiveOffsets.empty() false";
      ASSERT_EQ(nvramActiveOffset, wins.GetAvailableNvramActiveOffsets().front()) << "m_availableNvramActiveOffsets.front()";
      wins.GetAvailableNvramActiveOffsets().pop();
   }
   ASSERT_EQ(true, wins.GetAvailableNvramActiveOffsets().empty()) << "m_availableNvramActiveOffsets.empty() true";
}

/// <summary>
/// Functional test for:
///     StandardProgressiveWins::SetInactiveLocation()
///     StandardProgressiveWins::StandardProgressiveWins()
/// Test constructor with buildFromNvram = true.
/// </summary>
TEST_F(StandardProgressiveWinsTests, StandardProgressiveWins_SetInactiveLocation)
{
   // Setup.
   CUnitTestingUtils::SetNvramValidAndUpgraded();

   // Perform operation(s) and test(s).
   StandardProgressiveWins::SetInactiveLocation();
   StandardProgressiveWinsFriend wins(true);

   // Test awards recovered from NVRAM.
   ASSERT_EQ((size_t)0, wins.GetAwards().size()) << "m_Awards.size";

   // Test available offsets.
   ASSERT_EQ((size_t)NVRAM_EGM_AWARD_STANDARD_DATA_COUNT, wins.GetAvailableNvramActiveOffsets().size()) << "m_availableNvramActiveOffsets.size()";
   for (DWORD index(0); index < NVRAM_EGM_AWARD_STANDARD_DATA_COUNT; ++index)
   {
      DWORD nvramActiveOffset(NVRAM_EGM_AWARD_STANDARD_CHECKBYTE_OFFSET + index * NVRAM_EGM_AWARD_STANDARD_ITEM_SIZE);

      ASSERT_FALSE(wins.GetAvailableNvramActiveOffsets().empty()) << "m_availableNvramActiveOffsets.empty() false";
      ASSERT_EQ(nvramActiveOffset, wins.GetAvailableNvramActiveOffsets().front()) << "m_availableNvramActiveOffsets.front()";
      wins.GetAvailableNvramActiveOffsets().pop();
   }
   ASSERT_EQ(true, wins.GetAvailableNvramActiveOffsets().empty()) << "m_availableNvramActiveOffsets.empty() true";
}

/// <summary>
/// Functional test for:
///     StandardProgressiveWins::GetNextNvramActiveOffset()
///     StandardProgressiveWins::AddProgressiveWin()
///     StandardProgressiveWins::StandardProgressiveWins()
/// Test constructor with buildFromNvram = true with one win stored at the first offset.
/// </summary>
TEST_F(StandardProgressiveWinsTests, StandardProgressiveWins_Build1a)
{
   // Data.
   const __int64 amountHit(123456789012);
   const int levelId(1234567890);
   const EGMAwardState awardState(ReceivedFromGame);

   // Setup.
   CUnitTestingUtils::SetNvramValidAndUpgraded();

   // Perform operation(s) and test(s).
   {
      StandardProgressiveWins::SetInactiveLocation();
      StandardProgressiveWinsFriend wins;

      shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());
      award->SetAwardAmountHit(amountHit);
      award->SetLevelId(levelId);
      award->SetAwardState(awardState);
      wins.AddProgressiveWin(award);
   }

   {
      // Perform operation(s) and test(s).
      StandardProgressiveWinsFriend wins(true);

      // Test awards recovered from NVRAM.
      ASSERT_EQ((size_t)1, wins.GetAwards().size()) << "m_Awards.size";
      auto award = wins.FindAward(amountHit, levelId);
      ASSERT_TRUE(NULL != award.get()) << "award";
      ASSERT_EQ(amountHit, award->GetAwardAmountHit()) << "GetAwardAmountHit";
      ASSERT_EQ(levelId, award->GetLevelId()) << "GetLevelId";
      ASSERT_EQ(awardState, award->GetAwardState()) << "GetAwardState";
      ASSERT_EQ(NVRAM_EGM_AWARD_STANDARD_CHECKBYTE_OFFSET, award->GetNvramActiveOffset()) << "GetNvramActiveOffset";

      // Test available offsets.
      ASSERT_EQ((size_t)(NVRAM_EGM_AWARD_STANDARD_DATA_COUNT - 1), wins.GetAvailableNvramActiveOffsets().size()) << "m_availableNvramActiveOffsets.size()";
      for (DWORD index(1); index < NVRAM_EGM_AWARD_STANDARD_DATA_COUNT; ++index)
      {
         DWORD nvramActiveOffset(NVRAM_EGM_AWARD_STANDARD_CHECKBYTE_OFFSET + index * NVRAM_EGM_AWARD_STANDARD_ITEM_SIZE);

         ASSERT_FALSE(wins.GetAvailableNvramActiveOffsets().empty()) << "m_availableNvramActiveOffsets.empty() false";
         ASSERT_EQ(nvramActiveOffset, wins.GetAvailableNvramActiveOffsets().front()) << "m_availableNvramActiveOffsets.front()";
         wins.GetAvailableNvramActiveOffsets().pop();
      }
      ASSERT_EQ(true, wins.GetAvailableNvramActiveOffsets().empty()) << "m_availableNvramActiveOffsets.empty() true";
   }
}

/// <summary>
/// Functional test for:
///     StandardProgressiveWins::GetNextNvramActiveOffset()
///     StandardProgressiveWins::AddProgressiveWin()
///     StandardProgressiveWins::RemoveAward()
///     StandardProgressiveWins::RecoverNvramActiveOffset()
///     StandardProgressiveWins::StandardProgressiveWins()
/// Test constructor with buildFromNvram = true with one win stored at the second offset.
/// </summary>
TEST_F(StandardProgressiveWinsTests, StandardProgressiveWins_Build1b)
{
   // Data.
   const __int64 amountHit(123456789012);
   const int levelId(1234567890);
   const EGMAwardState awardState(ReceivedFromGame);

   // Setup.
   CUnitTestingUtils::SetNvramValidAndUpgraded();

   // Perform operation(s) and test(s).
   {
      StandardProgressiveWins::SetInactiveLocation();
      StandardProgressiveWinsFriend wins;

      {
         // Add and remove a different award.
         shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());
         award->SetAwardAmountHit(amountHit + 11);
         award->SetLevelId(levelId + 22);
         award->SetAwardState(awardState);
         wins.AddProgressiveWin(award);
         wins.RemoveAward(amountHit + 11, levelId + 22);
      }

      {
         // Add the award of interest.
         shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());
         award->SetAwardAmountHit(amountHit);
         award->SetLevelId(levelId);
         award->SetAwardState(awardState);
         wins.AddProgressiveWin(award);
      }

      // Verify number of awards and available offsets.
      ASSERT_EQ((size_t)1, wins.GetAwards().size()) << "m_Awards.size() 3";
      ASSERT_EQ((size_t)(NVRAM_EGM_AWARD_STANDARD_DATA_COUNT - 1), wins.GetAvailableNvramActiveOffsets().size()) << "m_availableNvramActiveOffsets.size() 3";
   }

   {
      // Perform operation(s) and test(s).
      StandardProgressiveWinsFriend wins(true);

      // Test awards recovered from NVRAM.
      ASSERT_EQ((size_t)1, wins.GetAwards().size()) << "m_Awards.size";
      auto award = wins.FindAward(amountHit, levelId);
      ASSERT_TRUE(NULL != award.get()) << "award";
      ASSERT_EQ(amountHit, award->GetAwardAmountHit()) << "GetAwardAmountHit";
      ASSERT_EQ(levelId, award->GetLevelId()) << "GetLevelId";
      ASSERT_EQ(awardState, award->GetAwardState()) << "GetAwardState";
      ASSERT_EQ(NVRAM_EGM_AWARD_STANDARD_CHECKBYTE_OFFSET + NVRAM_EGM_AWARD_STANDARD_ITEM_SIZE, award->GetNvramActiveOffset()) << "GetNvramActiveOffset";

      // Test available offsets.
      ASSERT_EQ((size_t)(NVRAM_EGM_AWARD_STANDARD_DATA_COUNT - 1), wins.GetAvailableNvramActiveOffsets().size()) << "m_availableNvramActiveOffsets.size()";
      for (DWORD index(0); index < NVRAM_EGM_AWARD_STANDARD_DATA_COUNT; ++index)
      {
         if (index != 1)
         {
            DWORD nvramActiveOffset(NVRAM_EGM_AWARD_STANDARD_CHECKBYTE_OFFSET + index * NVRAM_EGM_AWARD_STANDARD_ITEM_SIZE);

            ASSERT_FALSE(wins.GetAvailableNvramActiveOffsets().empty()) << "m_availableNvramActiveOffsets.empty() false";
            ASSERT_EQ(nvramActiveOffset, wins.GetAvailableNvramActiveOffsets().front()) << "m_availableNvramActiveOffsets.front()";
            wins.GetAvailableNvramActiveOffsets().pop();
         }
      }
      ASSERT_EQ(true, wins.GetAvailableNvramActiveOffsets().empty()) << "m_availableNvramActiveOffsets.empty() true";
   }
}

/// <summary>
/// Functional test for:
///     StandardProgressiveWins::GetNextNvramActiveOffset()
///     StandardProgressiveWins::AddProgressiveWin()
///     StandardProgressiveWins::RemoveAward()
///     StandardProgressiveWins::RecoverNvramActiveOffset()
///     StandardProgressiveWins::StandardProgressiveWins()
/// Test constructor with buildFromNvram = true after adding NVRAM_EGM_AWARD_STANDARD_DATA_COUNT+3 awards,
/// Also test removing the last award that there was not room to store.
/// </summary>
TEST_F(StandardProgressiveWinsTests, StandardProgressiveWins_Build35_1)
{
   // Data.
   const __int64 amountHit(123456789000);
   const int levelId(1234567000);
   const EGMAwardState awardState(ReceivedFromGame);

   // Setup.
   CUnitTestingUtils::SetNvramValidAndUpgraded();

   // Perform operation(s) and test(s).
   {
      StandardProgressiveWins::SetInactiveLocation();
      StandardProgressiveWinsFriend wins;

      __int64 lastAmountHit(123456789000);
      int lastLevelId(1234567000);
      for (int index(0); index < (int)NVRAM_EGM_AWARD_STANDARD_DATA_COUNT + 3; ++index)
      {
         // Add a different award each loop iteration.
         shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());
         award->SetAwardAmountHit(lastAmountHit = amountHit + index);
         award->SetLevelId(lastLevelId = levelId + index);
         award->SetAwardState(awardState);
         wins.AddProgressiveWin(award);
      }
      ASSERT_EQ((size_t)(NVRAM_EGM_AWARD_STANDARD_DATA_COUNT + 3), wins.GetAwards().size()) << "m_Awards.size() 3";
      ASSERT_EQ((size_t)0, wins.GetAvailableNvramActiveOffsets().size()) << "m_availableNvramActiveOffsets.size() 3";

      // Verify that the last 3 awards added are not persisted.
      for (int index((int)NVRAM_EGM_AWARD_STANDARD_DATA_COUNT); index < (int)NVRAM_EGM_AWARD_STANDARD_DATA_COUNT + 3; ++index)
      {
         auto award = wins.FindAward(amountHit + index, levelId + index);
         ASSERT_TRUE(NULL != award.get()) << "award 3";
         ASSERT_EQ(amountHit + index, award->GetAwardAmountHit()) << "GetAwardAmountHit 3";
         ASSERT_EQ(levelId + index, award->GetLevelId()) << "GetLevelId 3";
         ASSERT_EQ(awardState, award->GetAwardState()) << "GetAwardState 3";
         //ASSERT_EQ(CMemoryStorage::NVRAM_DEFAULT_OFFSET, award->GetNvramActiveOffset()) << "GetNvramActiveOffset 3";
         ASSERT_EQ(MEMORY_NONE, award->GetMemoryType()) << "GetNvramActiveOffset 3";
      }

      // Remove the last award added.
      wins.RemoveAward(lastAmountHit, lastLevelId);
      ASSERT_EQ((size_t)(NVRAM_EGM_AWARD_STANDARD_DATA_COUNT + 2), wins.GetAwards().size()) << "m_Awards.size() 2";
   }

   {
      // Perform operation(s) and test(s).
      StandardProgressiveWinsFriend wins(true);

      // Test awards recovered from NVRAM.
      ASSERT_EQ((size_t)NVRAM_EGM_AWARD_STANDARD_DATA_COUNT, wins.GetAwards().size()) << "m_Awards.size";
      for (int index(0); index < (int)NVRAM_EGM_AWARD_STANDARD_DATA_COUNT; ++index)
      {
         auto award = wins.FindAward(amountHit + index, levelId + index);
         ASSERT_TRUE(NULL != award.get()) << "award";
         ASSERT_EQ(amountHit + index, award->GetAwardAmountHit()) << "GetAwardAmountHit";
         ASSERT_EQ(levelId + index, award->GetLevelId()) << "GetLevelId";
         ASSERT_EQ(awardState, award->GetAwardState()) << "GetAwardState";
         DWORD nvramActiveOffset(NVRAM_EGM_AWARD_STANDARD_CHECKBYTE_OFFSET + (DWORD)index * NVRAM_EGM_AWARD_STANDARD_ITEM_SIZE);
         ASSERT_EQ(nvramActiveOffset, award->GetNvramActiveOffset()) << "GetNvramActiveOffset";
      }

      // Test available offsets.
      ASSERT_EQ((size_t)0, wins.GetAvailableNvramActiveOffsets().size()) << "m_availableNvramActiveOffsets.size()";
   }
}
// #pragma endregion StandardProgressiveWins_Persist

/// <summary>
/// Verifies that there are zero awards present.
/// </summary>
/// <param name="wins">IN - The Standard progressive wins.</param>
void VerifyZeroAwardsPresent(StandardProgressiveWinsFriend& wins)
{
   // Test number of awards present.
   ASSERT_EQ((size_t)0, wins.GetAwards().size()) << "m_Awards.size";

   // Test number of available offsets.
   ASSERT_EQ((size_t)NVRAM_EGM_AWARD_STANDARD_DATA_COUNT, wins.GetAvailableNvramActiveOffsets().size()) << "m_availableNvramActiveOffsets.size()";
}

/// <summary>
/// Verifies the one award present.
/// </summary>
/// <param name="wins">IN - The Standard progressive wins.</param>
/// <param name="amountHit">IN - The amount hit.</param>
/// <param name="levelId">IN - The One Link level identifier.</param>
/// <param name="sasLevelId">IN - The SAS level identifier.</param>
/// <param name="awardState">IN - The award state.</param>
/// <param name="isHitReceived">IN - true if a hit received indication was received from One Link, false otherwise.</param>
/// <param name="isHandpayPendingSignaled">IN - true if a hand pay pending indication was received from the EGM, false otherwise.</param>
/// <param name="isEgmPaidSignaled">IN - true if an EGM paid indication was received from the EGM, false otherwise.</param>
void VerifyOneAwardPresent(StandardProgressiveWinsFriend& wins,
                           __int64 amountHit,
                           int levelId,
                           int sasLevelId,
                           EGMAwardState awardState,
                           bool isHitReceived,
                           bool isHandpayPendingSignaled,
                           bool isEgmPaidSignaled)
{
   // Test awards recovered from NVRAM.
   ASSERT_EQ((size_t)1, wins.GetAwards().size()) << "m_Awards.size";
   auto award = wins.FindAward(amountHit, levelId);
   ASSERT_TRUE(NULL != award.get()) << "award";
   ASSERT_EQ(amountHit, award->GetAwardAmountHit()) << "GetAwardAmountHit";
   ASSERT_EQ(levelId, award->GetLevelId()) << "GetLevelId";
   ASSERT_EQ(sasLevelId, award->GetSASLevelId()) << "GetSASLevelId";
   ASSERT_EQ(awardState, award->GetAwardState()) << "GetAwardState";
   ASSERT_EQ(isHitReceived, award->isHitReceived()) << "isHitReceived";
   ASSERT_EQ(isHandpayPendingSignaled, award->isHandpayPendingSignaled()) << "isHandpayPendingSignaled";
   ASSERT_EQ(isEgmPaidSignaled, award->isEgmPaidSignaled()) << "isEgmPaidSignaled";

   // Test number of available offsets.
   ASSERT_EQ((size_t)(NVRAM_EGM_AWARD_STANDARD_DATA_COUNT - 1), wins.GetAvailableNvramActiveOffsets().size()) << "m_availableNvramActiveOffsets.size()";
}

// #pragma region ProcessEGMAwardStandardFromProgressive_Persist
/// <summary>
/// Functional test for:
///     StandardProgressiveWins::AddProgressiveWin()
///     StandardProgressiveWins::ProcessEGMAwardStandardFromProgressive()
///     StandardProgressiveWins::StandardProgressiveWins()
/// Test changing the hit received state of an award that is already persisted.
/// Test constructor with buildFromNvram = true with one win stored.
/// </summary>
TEST_F(StandardProgressiveWinsTests, ProcessEGMAwardStandardFromProgressive_Build1)
{
   // Data.
   const __int64 amountHit(123456789012);
   const int levelId(1234567890);
   const EGMAwardState awardState(ReceivedFromGame);

   // Setup.
   CUnitTestingUtils::SetNvramValidAndUpgraded();

   {
      // Setup.
      StandardProgressiveWins::SetInactiveLocation();
      StandardProgressiveWinsFriend wins;

      // Add award.
      {
         shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());
         award->SetAwardAmountHit(amountHit);
         award->SetLevelId(levelId);
         award->SetAwardState(awardState);
         wins.AddProgressiveWin(award);
      }

      // Perform operation(s) and test(s).
      // Process EGM award standard from progressive.
      {
         CGames games;
         shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());
         award->SetAwardAmountHit(amountHit);
         award->SetLevelId(levelId);
         std::vector<MsgTargetPair *> *returnMessages = new std::vector<MsgTargetPair *>();
         wins.ProcessEGMAwardStandardFromProgressive(0, games, false, *award.get(), returnMessages);
         for (auto awardIterator = returnMessages->cbegin(); awardIterator != returnMessages->cend(); /*no need to in iterator*/)
         {
            if (nullptr != (*awardIterator)->msg)
            {
               delete (*awardIterator)->msg; //Delete the message that was created
            }
            awardIterator = returnMessages->erase(awardIterator);   //remove the item from the vector and repair the iterator
         }
         delete returnMessages;
      }

      // Verify state change.
      VerifyOneAwardPresent(wins, amountHit, levelId, InvalidSasLevel, awardState, true, false, false);
   }

   // Recover EGM awards (if any) from NVRAM.
   {
      // Perform operation(s) and test(s).
      StandardProgressiveWinsFriend wins(true);
      VerifyOneAwardPresent(wins, amountHit, levelId, InvalidSasLevel, awardState, true, false, false);
   }
}

/// <summary>
/// Functional test for:
///     StandardProgressiveWins::AddProgressiveWin()
///     StandardProgressiveWins::ProcessEGMAwardStandardFromProgressive()
///     StandardProgressiveWins::RemoveAward()
///     StandardProgressiveWins::StandardProgressiveWins()
/// Test changing the hit received state of an award that is already persisted and EGM paid signaled.
/// Test constructor with buildFromNvram = true with no wins stored.
/// </summary>
TEST_F(StandardProgressiveWinsTests, ProcessEGMAwardStandardFromProgressive_Build0)
{
   // Data.
   const __int64 amountHit(123456789012);
   const int levelId(1234567890);
   const EGMAwardState awardState(ReceivedFromGame);

   // Setup.
   CUnitTestingUtils::SetNvramValidAndUpgraded();

   {
      // Setup.
      StandardProgressiveWins::SetInactiveLocation();
      StandardProgressiveWinsFriend wins;

      // Add award.
      {
         shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());
         award->SetAwardAmountHit(amountHit);
         award->SetLevelId(levelId);
         award->SetAwardState(awardState);
         wins.AddProgressiveWin(award);
      }

      // Process progressive paid by EGM.
      {
         shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());
         award->SetLevelId(levelId);
         award->SetAwardAmountHit(amountHit);
         std::vector<MsgTargetPair *> *returnMessages = new std::vector<MsgTargetPair *>();
         wins.SetProgressivePaidByEGM(award, returnMessages);
         for (auto awardIterator = returnMessages->cbegin(); awardIterator != returnMessages->cend(); /*no need to in iterator*/)
         {
            if (nullptr != (*awardIterator)->msg)
            {
               delete (*awardIterator)->msg; //Delete the message that was created
            }
            awardIterator = returnMessages->erase(awardIterator);   //remove the item from the vector and repair the iterator
         }
         delete returnMessages;
      }

      // Verify state change.
      VerifyOneAwardPresent(wins, amountHit, levelId, InvalidSasLevel, awardState, false, false, true);

      // Perform operation(s) and test(s).
      // Process EGM award standard from progressive.
      {
         CGames games;
         shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());
         award->SetAwardAmountHit(amountHit);
         award->SetLevelId(levelId);
         std::vector<MsgTargetPair *> *returnMessages = new std::vector<MsgTargetPair *>();
         wins.ProcessEGMAwardStandardFromProgressive(0, games, false, *award.get(), returnMessages);
         for (auto awardIterator = returnMessages->cbegin(); awardIterator != returnMessages->cend(); /*no need to in iterator*/)
         {
            if (nullptr != (*awardIterator)->msg)
            {
               delete (*awardIterator)->msg; //Delete the message that was created
            }
            awardIterator = returnMessages->erase(awardIterator);   //remove the item from the vector and repair the iterator
         }
         delete returnMessages;
      }

      {
         shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());
         award->SetAwardAmountHit(amountHit);
         award->SetLevelId(levelId);
         wins.ProgressivePaidSentReceived(award);
      }

      // Verify no state change.
      VerifyZeroAwardsPresent(wins);
   }

   // Recover EGM awards (if any) from NVRAM.
   {
      // Perform operation(s) and test(s).
      StandardProgressiveWinsFriend wins(true);
      VerifyZeroAwardsPresent(wins);
   }
}
// #pragma endregion ProcessEGMAwardStandardFromProgressive_Persist

// #pragma region MarkState_Persist
/// <summary>
/// Functional test for:
///     StandardProgressiveWins::AddProgressiveWin()
///     StandardProgressiveWins::MarkState()
///     StandardProgressiveWins::StandardProgressiveWins()
/// Test changing the award state of an award that is already persisted.
/// Test constructor with buildFromNvram = true with one win stored at the first offset.
/// </summary>
TEST_F(StandardProgressiveWinsTests, MarkState_Build1)
{
   // Data.
   const __int64 amountHit(123456789012);
   const int levelId(1234567890);
   const EGMAwardState awardStateInit(ReceivedFromGame);
   const EGMAwardState awardState(SentToSystem);

   // Setup.
   CUnitTestingUtils::SetNvramValidAndUpgraded();

   {
      // Setup.
      StandardProgressiveWins::SetInactiveLocation();
      StandardProgressiveWinsFriend wins;

      // Add award.
      {
         shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());
         award->SetAwardAmountHit(amountHit);
         award->SetLevelId(levelId);
         award->SetAwardState(awardStateInit);
         wins.AddProgressiveWin(award);
      }

      // Perform operation(s) and test(s).
      // Mark award.
      {
         shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());
         award->SetAwardAmountHit(amountHit);
         award->SetLevelId(levelId);
         wins.MarkState(award, awardState);
      }

      // Verify state change.
      VerifyOneAwardPresent(wins, amountHit, levelId, InvalidSasLevel, awardState, false, false, false);
   }

   // Recover EGM awards (if any) from NVRAM.
   {
      // Perform operation(s) and test(s).
      StandardProgressiveWinsFriend wins(true);
      VerifyOneAwardPresent(wins, amountHit, levelId, InvalidSasLevel, awardState, false, false, false);
   }
}
// #pragma endregion MarkState_Persist

// #pragma region SetHandPayPending_Persist
/// <summary>
/// Functional test for:
///     StandardProgressiveWins::AddProgressiveWin()
///     StandardProgressiveWins::SetHandPayPending()
///     StandardProgressiveWins::StandardProgressiveWins()
/// Test changing the hand pay pending state of an award that is already persisted.
/// Test constructor with buildFromNvram = true with one win stored at the first offset.
/// </summary>
TEST_F(StandardProgressiveWinsTests, SetHandPayPending_Build1)
{
   // Data.
   const __int64 amountHit(123456789012);
   const int levelId(1234567890);
   const int sasLevelId(21);
   const EGMAwardState awardStateInit(ReceivedFromGame);
   const EGMAwardState awardState(HandpayPending);

   // Setup.
   CUnitTestingUtils::SetNvramValidAndUpgraded();

   {
      // Setup.
      StandardProgressiveWins::SetInactiveLocation();
      StandardProgressiveWinsFriend wins;

      // Add award.
      {
         shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());
         award->SetAwardAmountHit(amountHit);
         award->SetLevelId(levelId);
         award->SetSASLevelId(sasLevelId);
         award->SetAwardState(awardStateInit);
         wins.AddProgressiveWin(award);
      }

      // Perform operation(s) and test(s).
      // Set hand pay pending.
      {
         std::vector<MsgTargetPair *> *returnMessages = new std::vector<MsgTargetPair *>();
         wins.SetHandPayPending(amountHit, sasLevelId, returnMessages);
         delete returnMessages;
      }

      // Verify state change.
      VerifyOneAwardPresent(wins, amountHit, levelId, sasLevelId, awardState, false, true, false);
   }

   // Recover EGM awards (if any) from NVRAM.
   {
      // Perform operation(s) and test(s).
      StandardProgressiveWinsFriend wins(true);
      VerifyOneAwardPresent(wins, amountHit, levelId, sasLevelId, awardState, false, true, false);
   }
}
// #pragma endregion SetHandPayPending_Persist

// #pragma region SetHandPayReset_Persist
/// <summary>
/// Functional test for:
///     StandardProgressiveWins::AddProgressiveWin()
///     StandardProgressiveWins::SetProgressivePaidByEGM()
///     StandardProgressiveWins::StandardProgressiveWins()
/// Test changing the EGM paid signaled state of an award that is already persisted.
/// Test constructor with buildFromNvram = true with one win stored.
/// </summary>
TEST_F(StandardProgressiveWinsTests, SetHandPayReset_Build1)
{
   // Data.
   const __int64 amountHit(123456789012);
   const int levelId(1234567890);
   const EGMAwardState awardState(ReceivedFromGame);
   const EGMAwardState awardState2(AwardToGameSuccess);


   // Setup.
   CUnitTestingUtils::SetNvramValidAndUpgraded();

   {
      // Setup.
      StandardProgressiveWins::SetInactiveLocation();
      StandardProgressiveWinsFriend wins;

      // Add award.
      {
         shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());
         award->SetAwardAmountHit(amountHit);
         award->SetLevelId(levelId);
         award->SetAwardState(awardState);
         wins.AddProgressiveWin(award);
      }

      // Perform operation(s) and test(s).
      // Set hand pay reset.
      {
         std::vector<MsgTargetPair *> *returnMessages = new std::vector<MsgTargetPair *>();
         wins.SetHandPayReset(returnMessages);
         for (auto awardIterator = returnMessages->cbegin(); awardIterator != returnMessages->cend(); /*no need to in iterator*/)
         {
            if (nullptr != (*awardIterator)->msg)
            {
               delete (*awardIterator)->msg; //Delete the message that was created
            }
            awardIterator = returnMessages->erase(awardIterator);   //remove the item from the vector and repair the iterator
         }
         delete returnMessages;
      }

      // Verify state change.
      VerifyOneAwardPresent(wins, amountHit, levelId, InvalidSasLevel, awardState2, false, false, true);
   }

   // Recover EGM awards (if any) from NVRAM.
   {
      // Perform operation(s) and test(s).
      StandardProgressiveWinsFriend wins(true);
      VerifyOneAwardPresent(wins, amountHit, levelId, InvalidSasLevel, awardState2, false, false, true);
   }
}

/// <summary>
/// Functional test for:
///     StandardProgressiveWins::AddProgressiveWin()
///     StandardProgressiveWins::SetProgressivePaidByEGM()
///     StandardProgressiveWins::RemoveAward()
///     StandardProgressiveWins::StandardProgressiveWins()
/// Test changing the EGM paid signaled state of an award that is already persisted.
/// Test constructor with buildFromNvram = true with no wins stored.
/// </summary>
TEST_F(StandardProgressiveWinsTests, SetHandPayReset_Build0)
{
   // Data.
   const __int64 amountHit(123456789012);
   const int levelId(1234567890);
   const int sasLevelId(21);
   const EGMAwardState awardStateInit(ReceivedFromGame);
   const EGMAwardState awardState(HandpayPending);

   // Setup.
   CUnitTestingUtils::SetNvramValidAndUpgraded();

   {
      // Setup.
      StandardProgressiveWins::SetInactiveLocation();
      StandardProgressiveWinsFriend wins;

      // Add award.
      {
         shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());
         award->SetAwardAmountHit(amountHit);
         award->SetLevelId(levelId);
         award->SetSASLevelId(sasLevelId);
         award->SetAwardState(awardStateInit);
         wins.AddProgressiveWin(award);
      }

      // Process EGM award standard from progressive.
      {
         CGames games;
         shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());
         award->SetAwardAmountHit(amountHit);
         award->SetLevelId(levelId);
         std::vector<MsgTargetPair *> *returnMessages = new std::vector<MsgTargetPair *>();
         wins.ProcessEGMAwardStandardFromProgressive(0, games, false, *award.get(), returnMessages);
         for (auto awardIterator = returnMessages->cbegin(); awardIterator != returnMessages->cend(); /*no need to in iterator*/)
         {
            if (nullptr != (*awardIterator)->msg)
            {
               delete (*awardIterator)->msg; //Delete the message that was created
            }
            awardIterator = returnMessages->erase(awardIterator);   //remove the item from the vector and repair the iterator
         }
         delete returnMessages;
      }

      // Verify state change.
      VerifyOneAwardPresent(wins, amountHit, levelId, sasLevelId, awardStateInit, true, false, false);

      // Set hand pay pending.
      {
         std::vector<MsgTargetPair *> *returnMessages = new std::vector<MsgTargetPair *>();
         wins.SetHandPayPending(amountHit, sasLevelId, returnMessages);
         delete returnMessages;
      }

      // Verify state change.
      VerifyOneAwardPresent(wins, amountHit, levelId, sasLevelId, awardState, true, true, false);

      // Perform operation(s) and test(s).
      // Set hand pay reset.
      {
         std::vector<MsgTargetPair *> *returnMessages = new std::vector<MsgTargetPair *>();
         wins.SetHandPayReset(returnMessages);
         for (auto awardIterator = returnMessages->cbegin(); awardIterator != returnMessages->cend(); /*no need to in iterator*/)
         {
            if (nullptr != (*awardIterator)->msg)
            {
               delete (*awardIterator)->msg; //Delete the message that was created
            }
            awardIterator = returnMessages->erase(awardIterator);   //remove the item from the vector and repair the iterator
         }
         delete returnMessages;
      }

      {
         shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());
         award->SetAwardAmountHit(amountHit);
         award->SetLevelId(levelId);
         wins.ProgressivePaidSentReceived(award);
      }

      // Verify state change.
      VerifyZeroAwardsPresent(wins);
   }

   // Recover EGM awards (if any) from NVRAM.
   {
      // Perform operation(s) and test(s).
      StandardProgressiveWinsFriend wins(true);
      VerifyZeroAwardsPresent(wins);
   }
}
// #pragma endregion SetHandPayReset_Persist

// #pragma region SetProgressivePaidByEGM_Persist
/// <summary>
/// Functional test for:
///     StandardProgressiveWins::AddProgressiveWin()
///     StandardProgressiveWins::SetProgressivePaidByEGM()
///     StandardProgressiveWins::StandardProgressiveWins()
/// Test changing the EGM paid signaled state of an award that is already persisted.
/// Test constructor with buildFromNvram = true with one win stored.
/// </summary>
TEST_F(StandardProgressiveWinsTests, SetProgressivePaidByEGM_Build1)
{
   // Data.
   const __int64 amountHit(123456789012);
   const int levelId(1234567890);
   const EGMAwardState awardState(ReceivedFromGame);

   // Setup.
   CUnitTestingUtils::SetNvramValidAndUpgraded();

   {
      // Setup.
      StandardProgressiveWins::SetInactiveLocation();
      StandardProgressiveWinsFriend wins;

      // Add award.
      {
         shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());
         award->SetAwardAmountHit(amountHit);
         award->SetLevelId(levelId);
         award->SetAwardState(awardState);
         wins.AddProgressiveWin(award);
      }

      // Perform operation(s) and test(s).
      // Process progressive paid by EGM.
      {
         shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());
         award->SetAwardAmountHit(amountHit);
         award->SetLevelId(levelId);
         std::vector<MsgTargetPair *> *returnMessages = new std::vector<MsgTargetPair *>();
         wins.SetProgressivePaidByEGM(award, returnMessages);
         for (auto awardIterator = returnMessages->cbegin(); awardIterator != returnMessages->cend(); /*no need to in iterator*/)
         {
            if (nullptr != (*awardIterator)->msg)
            {
               delete (*awardIterator)->msg; //Delete the message that was created
            }
            awardIterator = returnMessages->erase(awardIterator);   //remove the item from the vector and repair the iterator
         }
         delete returnMessages;
      }

      // Verify state change.
      VerifyOneAwardPresent(wins, amountHit, levelId, InvalidSasLevel, awardState, false, false, true);
   }

   // Recover EGM awards (if any) from NVRAM.
   {
      // Perform operation(s) and test(s).
      StandardProgressiveWinsFriend wins(true);
      VerifyOneAwardPresent(wins, amountHit, levelId, InvalidSasLevel, awardState, false, false, true);
   }
}

/// <summary>
/// Functional test for:
///     StandardProgressiveWins::AddProgressiveWin()
///     StandardProgressiveWins::SetProgressivePaidByEGM()
///     StandardProgressiveWins::RemoveAward()
///     StandardProgressiveWins::StandardProgressiveWins()
/// Test changing the hit received state of an award that is already persisted and EGM paid signaled.
/// Test constructor with buildFromNvram = true with no wins stored.
/// </summary>
TEST_F(StandardProgressiveWinsTests, SetProgressivePaidByEGM_Build0)
{
   // Data.
   const __int64 amountHit(123456789012);
   const int levelId(1234567890);
   const EGMAwardState awardState(ReceivedFromGame);

   // Setup.
   CUnitTestingUtils::SetNvramValidAndUpgraded();

   {
      // Setup.
      StandardProgressiveWins::SetInactiveLocation();
      StandardProgressiveWinsFriend wins;

      // Add award.
      {
         shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());
         award->SetAwardAmountHit(amountHit);
         award->SetLevelId(levelId);
         award->SetAwardState(awardState);
         wins.AddProgressiveWin(award);
      }

      // Process EGM award standard from progressive.
      {
         CGames games;
         shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());
         award->SetAwardAmountHit(amountHit);
         award->SetLevelId(levelId);
         std::vector<MsgTargetPair *> *returnMessages = new std::vector<MsgTargetPair *>();
         wins.ProcessEGMAwardStandardFromProgressive(0, games, false, *award.get(), returnMessages);
         for (auto awardIterator = returnMessages->cbegin(); awardIterator != returnMessages->cend(); /*no need to in iterator*/)
         {
            if (nullptr != (*awardIterator)->msg)
            {
               delete (*awardIterator)->msg; //Delete the message that was created
            }
            awardIterator = returnMessages->erase(awardIterator);   //remove the item from the vector and repair the iterator
         }
         delete returnMessages;
      }

      // Verify state change.
      VerifyOneAwardPresent(wins, amountHit, levelId, InvalidSasLevel, awardState, true, false, false);

      // Perform operation(s) and test(s).
      // Process progressive paid by EGM.
      {
         shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());
         award->SetAwardAmountHit(amountHit);
         award->SetLevelId(levelId);
         std::vector<MsgTargetPair *> *returnMessages = new std::vector<MsgTargetPair *>();
         wins.SetProgressivePaidByEGM(award, returnMessages);
         for (auto awardIterator = returnMessages->cbegin(); awardIterator != returnMessages->cend(); /*no need to in iterator*/)
         {
            if (nullptr != (*awardIterator)->msg)
            {
               delete (*awardIterator)->msg; //Delete the message that was created
            }
            awardIterator = returnMessages->erase(awardIterator);   //remove the item from the vector and repair the iterator
         }
         delete returnMessages;
      }

      {
         shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());
         award->SetAwardAmountHit(amountHit);
         award->SetLevelId(levelId);
         wins.ProgressivePaidSentReceived(award);
      }

      // Verify state change.
      VerifyZeroAwardsPresent(wins);
   }

   // Recover EGM awards (if any) from NVRAM.
   {
      // Perform operation(s) and test(s).
      StandardProgressiveWinsFriend wins(true);
      VerifyZeroAwardsPresent(wins);
   }
}
// #pragma endregion SetProgressivePaidByEGM_Persist

// #pragma region LastAwardProcessed_Persist
/// <summary>
/// Functional test for:
///     StandardProgressiveWins
/// Test that the NVRAM memory map has not changed across versions.
/// If the NVRAM memory map does change across versions then memory map upgrades should be tested.
/// </summary>
TEST_F(StandardProgressiveWinsTests, LastAwardProcessed_NvramMap)
{
   // Test NVRAM memory map offset calculations.
   ASSERT_EQ(NVRAM_CUMULATIVE_PROGRESSIVE_DATA_OFFSET + NVRAM_CUMULATIVE_PROGRESSIVE_DATA_SIZE, NVRAM_LAST_STD_AWARD_CHECKBYTE_OFFSET) << "NVRAM_LAST_STD_AWARD_CHECKBYTE_OFFSET calc";
   ASSERT_EQ(NVRAM_LAST_STD_AWARD_CHECKBYTE_OFFSET + NVRAM_LAST_STD_AWARD_CHECKBYTE_SIZE, NVRAM_LAST_STD_AWARD_DATA_OFFSET) << "NVRAM_LAST_STD_AWARD_DATA_OFFSET calc";

   // Test NVRAM memory map for backwards and forwards memory compatibility.
   ASSERT_EQ((DWORD)722028, NVRAM_LAST_STD_AWARD_CHECKBYTE_OFFSET) << "NVRAM_LAST_STD_AWARD_CHECKBYTE_OFFSET";
   ASSERT_EQ((DWORD)1, NVRAM_LAST_STD_AWARD_CHECKBYTE_SIZE) << "NVRAM_LAST_STD_AWARD_CHECKBYTE_SIZE";
   ASSERT_EQ((BYTE)0x15, NVRAM_LAST_STD_AWARD_CHECKBYTE_VALUE) << "NVRAM_LAST_STD_AWARD_CHECKBYTE_VALUE";
   ASSERT_EQ((DWORD)722029, NVRAM_LAST_STD_AWARD_DATA_OFFSET) << "NVRAM_LAST_STD_AWARD_DATA_OFFSET";
   ASSERT_EQ((DWORD)256, NVRAM_LAST_STD_AWARD_DATA_SIZE) << "NVRAM_LAST_STD_AWARD_DATA_SIZE";
}

void DeleteReturnMessages(std::vector<MsgTargetPair *> *returnMessages)
{
   for (auto awardIterator = returnMessages->cbegin(); awardIterator != returnMessages->cend(); /*no need to in iterator*/)
   {
      if (nullptr != (*awardIterator)->msg)
      {
         delete (*awardIterator)->msg; //Delete the message that was created
      }
      awardIterator = returnMessages->erase(awardIterator);   //remove the item from the vector and repair the iterator
   }
   delete returnMessages;
}

// Need to fix friend class issues
/// <summary>
/// Verifies that there is no last award processed present.
/// </summary>
/// <param name="wins">IN - The Standard progressive wins.</param>
/// <param name="desc">IN - A description of when or why these tests are run.  Should be unique per test method.</param>
void VerifyLastAwardNotPresent(StandardProgressiveWinsFriend& wins, LPCTSTR desc)
{
   ASSERT_TRUE(NULL == wins.GetLastAwardProcessed().get()) << "m_lastAwardProcessed " + string(desc);

   // Verify that NVRAM location is consistent with no last award present.
   ASSERT_FALSE(CMemoryStorage::IsActiveLocation(NVRAM_LAST_STD_AWARD_CHECKBYTE_OFFSET, NVRAM_LAST_STD_AWARD_CHECKBYTE_VALUE)) <<
      "IsActiveLocation " << string(desc);
}

/// <summary>
/// Verifies the last award processed is present and has correct values.
/// </summary>
/// <param name="wins">IN - The Standard progressive wins.</param>
/// <param name="amountHit">IN - The amount hit.</param>
/// <param name="levelId">IN - The One Link level identifier.</param>
/// <param name="sasLevelId">IN - The SAS level identifier.</param>
/// <param name="awardState">IN - The award state.</param>
/// <param name="isHitReceived">IN - true if a hit received indication was received from One Link, false otherwise.</param>
/// <param name="isHandpayPendingSignaled">IN - true if a hand pay pending indication was received from the EGM, false otherwise.</param>
/// <param name="isEgmPaidSignaled">IN - true if an EGM paid indication was received from the EGM, false otherwise.</param>
/// <param name="desc">IN - A description of when or why these tests are run.  Should be unique per test method.</param>
void VerifyLastAwardPresent(StandardProgressiveWinsFriend& wins,
                            __int64 amountHit,
                            int levelId,
                            int sasLevelId,
                            EGMAwardState awardState,
                            bool isHitReceived,
                            bool isHandpayPendingSignaled,
                            bool isEgmPaidSignaled,
                            bool isActive,
                            LPCTSTR desc)
{
   // Verify that NVRAM location is consistent with last award present.
   ASSERT_EQ(isActive,
             CMemoryStorage::IsActiveLocation(NVRAM_LAST_STD_AWARD_CHECKBYTE_OFFSET, NVRAM_LAST_STD_AWARD_CHECKBYTE_VALUE)) <<
      "IsActiveLocation " << string(desc);

   ASSERT_TRUE(NULL != wins.GetLastAwardProcessed().get()) << "m_lastAwardProcessed " + string(desc);
   CEGMAward *award(wins.GetLastAwardProcessed().get()); // 2015-08-05 Need non-const instance for bool getters.
   ASSERT_EQ(amountHit, award->GetAwardAmountHit()) << "m_last GetAwardAmountHit " + string(desc);
   ASSERT_EQ(levelId, award->GetLevelId()) << "m_last GetLevelId " + string(desc);
   ASSERT_EQ(sasLevelId, award->GetSASLevelId()) << "m_last GetSASLevelId " + string(desc);
   ASSERT_EQ(awardState, award->GetAwardState()) << "GetAwardState " + string(desc);
   ASSERT_EQ(isHitReceived, award->isHitReceived()) << "isHitReceived " + string(desc);
   ASSERT_EQ(isHandpayPendingSignaled, award->isHandpayPendingSignaled()) << "isHandpayPendingSignaled " + string(desc);
   ASSERT_EQ(isEgmPaidSignaled, award->isEgmPaidSignaled()) << "isEgmPaidSignaled " + string(desc);
   ASSERT_EQ(isActive, award->IsActive()) << "IsActive " + string(desc);

   ASSERT_EQ(isActive, wins.IsThisADuplicate(amountHit, sasLevelId)) << "IsThisADuplicate " + string(desc);
}
/// <summary>
/// Functional test for:
///     StandardProgressiveWins::ProcessEGMAwardStandardFromProgressive()
///     StandardProgressiveWins::ProcessEGMAwardWithEGMPaidSignaled()
///     StandardProgressiveWins::RemoveAward()
///     StandardProgressiveWins::StandardProgressiveWins()
/// Test that the last EGM award processed is persisted to NVRAM memory when it is removed from the list of active records.
/// Test constructor with buildFromNvram = true and verify last award processed.
/// </summary>
TEST_F(StandardProgressiveWinsTests, LastAwardProcessed_RemoveAward_ProcessEGMAward)
{
   // Data.
   const __int64 amountHit(123456789012);
   const int levelId(1234567890);
   const int sasLevelId(21);
   const EGMAwardState awardState(ReceivedFromGame);

   // Setup.
   CUnitTestingUtils::SetNvramValidAndUpgraded();

   {
      // Setup.
      StandardProgressiveWins::SetInactiveLocation();
      StandardProgressiveWinsFriend wins;
      VerifyLastAwardNotPresent(wins, "setup");

      // Add award with m_isEgmPaidSignaled=true.
      {
         shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());
         award->SetAwardAmountHit(amountHit);
         award->SetLevelId(levelId);
         award->SetSASLevelId(sasLevelId);
         award->SetAwardState(awardState);
         award->setEgmPaidSignaled(true);
         wins.AddProgressiveWin(award);
      }

      // Perform operation(s) and test(s).
      // Process EGM award standard from progressive.
      {
         CGames games;
         shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());
         award->SetAwardAmountHit(amountHit);
         award->SetLevelId(levelId);
         std::vector<MsgTargetPair *> *returnMessages = new std::vector<MsgTargetPair *>();
         wins.ProcessEGMAwardStandardFromProgressive(0, games, false, *award.get(), returnMessages);
         DeleteReturnMessages(returnMessages);
      }

      //Act as if you are the prog protocol and call back the paid
      {
         shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());
         award->SetAwardAmountHit(amountHit);
         award->SetLevelId(levelId);
         wins.ProgressivePaidSentReceived(award);
      }

      // Verify state change.
      VerifyZeroAwardsPresent(wins);
      VerifyLastAwardPresent(wins, amountHit, levelId, sasLevelId, awardState, true, false, true, true, "award removed");
   }

   // Recover EGM awards (if any) from NVRAM.
   {
      // Perform operation(s) and test(s).
      StandardProgressiveWinsFriend wins(true);
      VerifyZeroAwardsPresent(wins);
      VerifyLastAwardPresent(wins, amountHit, levelId, sasLevelId, awardState, true, false, true, true, "recover");
   }
}

/// <summary>
/// Functional test for:
///     StandardProgressiveWins::ProcessEGMAwardStandardFromProgressive()
///     StandardProgressiveWins::ProcessEGMAwardWithEGMPaidSignaled()
///     StandardProgressiveWins::RemoveAward()
///     StandardProgressiveWins::StandardProgressiveWins()
/// Test that the last EGM award processed is persisted to NVRAM memory when it is removed from the list of active records.
/// Test constructor with buildFromNvram = false and verify last award processed is not present.
/// </summary>
TEST_F(StandardProgressiveWinsTests, LastAwardProcessed_RemoveAward_ProcessEGMAward_BuildFalse)
{
   // Data.
   const __int64 amountHit(123456789012);
   const int levelId(1234567890);
   const int sasLevelId(21);
   const EGMAwardState awardState(ReceivedFromGame);

   // Setup.
   CUnitTestingUtils::SetNvramValidAndUpgraded();

   {
      // Setup.
      StandardProgressiveWins::SetInactiveLocation();
      StandardProgressiveWinsFriend wins;
      VerifyLastAwardNotPresent(wins, "setup");

      // Add award with m_isEgmPaidSignaled=true.
      {
         shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());
         award->SetAwardAmountHit(amountHit);
         award->SetLevelId(levelId);
         award->SetSASLevelId(sasLevelId);
         award->SetAwardState(awardState);
         award->setEgmPaidSignaled(true);
         wins.AddProgressiveWin(award);
      }

      // Perform operation(s) and test(s).
      // Process EGM award standard from progressive.
      {
         CGames games;
         shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());
         award->SetAwardAmountHit(amountHit);
         award->SetLevelId(levelId);
         std::vector<MsgTargetPair *> *returnMessages = new std::vector<MsgTargetPair *>();
         wins.ProcessEGMAwardStandardFromProgressive(0, games, false, *award.get(), returnMessages);
         DeleteReturnMessages(returnMessages);
      }

      //Act as if you are the prog protocol and call back the paid
      {
         shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());
         award->SetAwardAmountHit(amountHit);
         award->SetLevelId(levelId);
         wins.ProgressivePaidSentReceived(award);
      }

      // Verify state change.
      VerifyZeroAwardsPresent(wins);
      VerifyLastAwardPresent(wins, amountHit, levelId, sasLevelId, awardState, true, false, true, true, "award removed");
   }

   // Recover EGM awards (if any) from NVRAM.
   {
      // Perform operation(s) and test(s).
      StandardProgressiveWinsFriend wins(false);
      VerifyZeroAwardsPresent(wins);
      ASSERT_TRUE(NULL == wins.GetLastAwardProcessed().get()) << "m_lastAwardProcessed recover";
   }
}

/// <summary>
/// Functional test for:
///     StandardProgressiveWins::SetHandPayReset()
///     StandardProgressiveWins::RemoveAward()
///     StandardProgressiveWins::StandardProgressiveWins()
/// Test that the last EGM award processed is persisted to NVRAM memory when it is removed from the list of active records.
/// Test constructor with buildFromNvram = true and verify last award processed.
/// </summary>
TEST_F(StandardProgressiveWinsTests, LastAwardProcessed_RemoveAward_HandPayReset)
{
   // Data.
   const __int64 amountHit(123456789012);
   const int levelId(1234567890);
   const int sasLevelId(21);
   const EGMAwardState awardState(HandpayPending);
   const EGMAwardState awardState2(AwardToGameSuccess);

   // Setup.
   CUnitTestingUtils::SetNvramValidAndUpgraded();

   {
      // Setup.
      StandardProgressiveWins::SetInactiveLocation();
      StandardProgressiveWinsFriend wins;
      VerifyLastAwardNotPresent(wins, "setup");

      // Add award with m_isHitReceived=true.
      {
         shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());
         award->SetAwardAmountHit(amountHit);
         award->SetLevelId(levelId);
         award->SetSASLevelId(sasLevelId);
         award->SetHitReceived(true);
         award->setHandpayPendingSignaled(true);
         award->SetAwardState(awardState);
         wins.AddProgressiveWin(award);
      }

      // Perform operation(s) and test(s).
      // Process handpay reset.
      {
         std::vector<MsgTargetPair *> *returnMessages = new std::vector<MsgTargetPair *>();
         wins.SetHandPayReset(returnMessages);
      }

      //Act as if you are the prog protocol and call back the paid
      {
         shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());
         award->SetAwardAmountHit(amountHit);
         award->SetLevelId(levelId);
         wins.ProgressivePaidSentReceived(award);
      }

      // Verify state change.
      VerifyZeroAwardsPresent(wins);
      VerifyLastAwardPresent(wins, amountHit, levelId, sasLevelId, awardState2, true, true, true, true, "award removed");
   }

   // Recover EGM awards (if any) from NVRAM.
   {
      // Perform operation(s) and test(s).
      StandardProgressiveWinsFriend wins(true);
      VerifyZeroAwardsPresent(wins);
      VerifyLastAwardPresent(wins, amountHit, levelId, sasLevelId, awardState2, true, true, true, true, "recover");
   }
}

/// <summary>
/// Functional test for:
///     StandardProgressiveWins::SetProgressivePaidByEGM()
///     StandardProgressiveWins::RemoveAward()
///     StandardProgressiveWins::StandardProgressiveWins()
/// Test that the last EGM award processed is persisted to NVRAM memory when it is removed from the list of active records.
/// Test constructor with buildFromNvram = true and verify last award processed.
/// </summary>
TEST_F(StandardProgressiveWinsTests, LastAwardProcessed_RemoveAward_ProgressivePaidByEGM)
{
   // Data.
   const __int64 amountHit(123456789012);
   const int levelId(1234567890);
   const int sasLevelId(21);
   const EGMAwardState awardState(ReceivedFromGame);

   // Setup.
   CUnitTestingUtils::SetNvramValidAndUpgraded();

   {
      // Setup.
      StandardProgressiveWins::SetInactiveLocation();
      StandardProgressiveWinsFriend wins;
      VerifyLastAwardNotPresent(wins, "setup");

      // Add award with m_isHitReceived=true.
      {
         shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());
         award->SetAwardAmountHit(amountHit);
         award->SetLevelId(levelId);
         award->SetSASLevelId(sasLevelId);
         award->SetHitReceived(true);
         award->SetAwardState(awardState);
         wins.AddProgressiveWin(award);
      }

      // Perform operation(s) and test(s).
      // Process progressive paid by EGM.
      {
         shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());
         award->SetAwardAmountHit(amountHit);
         award->SetSASLevelId(sasLevelId);
         std::vector<MsgTargetPair *> *returnMessages = new std::vector<MsgTargetPair *>();
         wins.SetProgressivePaidByEGM(award, returnMessages);
         DeleteReturnMessages(returnMessages);
      }

      //Act as if you are the prog protocol and call back the paid
      {
         shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());
         award->SetAwardAmountHit(amountHit);
         award->SetLevelId(levelId);
         wins.ProgressivePaidSentReceived(award);
      }

      // Verify state change.
      VerifyZeroAwardsPresent(wins);
      VerifyLastAwardPresent(wins, amountHit, levelId, sasLevelId, awardState, true, false, true, true, "award removed");
   }

   // Recover EGM awards (if any) from NVRAM.
   {
      // Perform operation(s) and test(s).
      StandardProgressiveWinsFriend wins(true);
      VerifyZeroAwardsPresent(wins);
      VerifyLastAwardPresent(wins, amountHit, levelId, sasLevelId, awardState, true, false, true, true, "recover");
   }
}

/// <summary>
/// Functional test for:
///     StandardProgressiveWins::SetHandPayReset()
///     StandardProgressiveWins::StandardProgressiveWins()
/// Test that the last EGM award processed is not removed when a game start event is processed via SetHandPayReset().
/// Test constructor with buildFromNvram = true and verify last award processed is not present.
/// </summary>
TEST_F(StandardProgressiveWinsTests, LastAwardProcessed_GameStart)
{
   // Data.
   const __int64 amountHit(123456789012);
   const int levelId(1234567890);
   const int sasLevelId(21);
   const EGMAwardState awardState(ReceivedFromGame);

   // Setup.
   CUnitTestingUtils::SetNvramValidAndUpgraded();

   {
      // Setup.
      StandardProgressiveWins::SetInactiveLocation();
      StandardProgressiveWinsFriend wins;
      VerifyLastAwardNotPresent(wins, "setup");

      // Add award with m_isHitReceived=true.
      {
         shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());
         award->SetAwardAmountHit(amountHit);
         award->SetLevelId(levelId);
         award->SetSASLevelId(sasLevelId);
         award->SetHitReceived(true);
         award->SetAwardState(awardState);
         wins.AddProgressiveWin(award);
      }

      // Process progressive paid by EGM.
      {
         shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());
         award->SetAwardAmountHit(amountHit);
         award->SetSASLevelId(sasLevelId);
         std::vector<MsgTargetPair *> *returnMessages = new std::vector<MsgTargetPair *>();
         wins.SetProgressivePaidByEGM(award, returnMessages);
         DeleteReturnMessages(returnMessages);
      }

      //Act as if you are the prog protocol and call back the paid
      {
         shared_ptr<CEGMAward>award = shared_ptr<CEGMAward>(new CEGMAward());
         award->SetAwardAmountHit(amountHit);
         award->SetLevelId(levelId);
         wins.ProgressivePaidSentReceived(award);
      }

      // Verify state change.
      VerifyZeroAwardsPresent(wins);
      VerifyLastAwardPresent(wins, amountHit, levelId, sasLevelId, awardState, true, false, true, true, "award removed");

      // Perform operation(s) and test(s).
      // Process game start event.
      {
         std::vector<MsgTargetPair *> *returnMessages = new std::vector<MsgTargetPair *>();
         wins.SetHandPayReset(returnMessages, true);
      }

      VerifyLastAwardPresent(wins, amountHit, levelId, sasLevelId, awardState, true, false, true, false, "game start");
   }

   // Recover EGM awards (if any) from NVRAM.
   {
      // Perform operation(s) and test(s).
      StandardProgressiveWinsFriend wins(true);
      VerifyZeroAwardsPresent(wins);
      VerifyLastAwardPresent(wins, amountHit, levelId, sasLevelId, awardState, true, false, true, false, "recover");
   }
}
// #pragma endregion LastAwardProcessed_Persist

// #pragma region GetLastStandardWin
/// <summary>
/// Functional test for:
///     StandardProgressiveWins::StandardProgressiveWins(false)
///     StandardProgressiveWins::GetLastStandardWin()
/// Test with a default StandardProgressiveWins instance.
/// </summary>
TEST_F(StandardProgressiveWinsTests, GetLastStandardWin_DefaultTest)
{
   // Perform operation(s) and test(s).
   StandardProgressiveWins wins;
   std::shared_ptr<CEGMAward> lastStandardWin;

   wins.GetLastStandardWin(lastStandardWin);
   ASSERT_TRUE(NULL == (void *)lastStandardWin.get()) << "lastStandardWin.get";
}

/// <summary>
/// Functional test for:
///     StandardProgressiveWins::RemoveAward()
///     StandardProgressiveWins::GetLastStandardWin()
/// Test with a non-default StandardProgressiveWins instance.
/// </summary>
TEST_F(StandardProgressiveWinsTests, GetLastStandardWin_OneTest)
{
   // Setup.
   StandardProgressiveWins wins;
   std::shared_ptr<CEGMAward> lastStandardWin;

   shared_ptr<CEGMAward> standardWin1(new CEGMAward());
   const SYSTEMTIME receivedFromEGMTime1 = { 2000, 1, 0, 2, 01, 01, 01, 001 };
   standardWin1->SetReceivedFromEGMTime(receivedFromEGMTime1);
   standardWin1->SetAwardState(ReceivedFromGame);
   standardWin1->SetSASLevelId(11);
   standardWin1->SetLevelId(111);
   standardWin1->SetJackpotId(1111);
   standardWin1->SetAwardAmountPaid(10101);
   ASSERT_EQ(true, wins.AddProgressiveWin(standardWin1)) << "AddProgressiveWin 1";
   wins.ProgressivePaidSentReceived(standardWin1);

   // Perform operation(s) and test(s).
   wins.GetLastStandardWin(lastStandardWin);
   ASSERT_TRUE(NULL != (void *)lastStandardWin.get()) << "lastStandardWin.get";
   CUnitTestingUtils::AssertEqual(lastStandardWin->GetReceivedFromEGMTime(), receivedFromEGMTime1,
                                  _T("GetReceivedFromEGMTime 1"), 0, __TFUNCTION__, __LINE__);
   ASSERT_EQ(ReceivedFromGame, lastStandardWin->GetAwardState()) << "GetAwardState 1";
   ASSERT_EQ((int)11, lastStandardWin->GetSASLevelId()) << "GetSASLevelId 1";
   ASSERT_EQ((int)111, lastStandardWin->GetLevelId()) << "GetLevelId 1";
   ASSERT_EQ((LONG64)1111, lastStandardWin->GetJackpotId()) << "GetJackpotId 1";
   ASSERT_EQ((__int64)10101, lastStandardWin->GetAwardAmountPaid()) << "GetAwardAmountPaid 1";
}

/// <summary>
/// Functional test for:
///     StandardProgressiveWins::StandardProgressiveWins(true)
///     StandardProgressiveWins::GetLastStandardWin()
/// Test with a default StandardProgressiveWins instance recovered from invalid NVRAM.
/// </summary>
TEST_F(StandardProgressiveWinsTests, GetLastStandardWin_NvramInvalidTest)
{
   // Setup.
   CUnitTestingUtils::SetNvramInvalid();

   // Perform operation(s) and test(s).
   StandardProgressiveWins wins(true);
   std::shared_ptr<CEGMAward> lastStandardWin;

   wins.GetLastStandardWin(lastStandardWin);
   ASSERT_TRUE(NULL == (void *)lastStandardWin.get()) << "lastStandardWin.get";
}

/// <summary>
/// Functional test for:
///     StandardProgressiveWins::RemoveAward()
///     StandardProgressiveWins::StandardProgressiveWins(true)
///     StandardProgressiveWins::GetLastStandardWin()
/// Test recoverying a non-default StandardProgressiveWins instance from NVRAM.
/// </summary>
TEST_F(StandardProgressiveWinsTests, GetLastStandardWin_NvramTest)
{
   // Data.
   const SYSTEMTIME receivedFromEGMTime1 = { 2000, 1, 0, 2, 01, 01, 01, 001 };

   {
      // Setup NVRAM.
      CUnitTestingUtils::SetNvramInvalid();
      StandardProgressiveWins wins(true); // Set NVRAM for last standard win to inactive/default value.
      CUnitTestingUtils::SetNvramValidAndUpgraded();
   }

   {
      // Setup a non-default last standard win.
      StandardProgressiveWins wins;
      std::shared_ptr<CEGMAward> lastStandardWin;

      shared_ptr<CEGMAward> standardWin1(new CEGMAward());
      standardWin1->SetReceivedFromEGMTime(receivedFromEGMTime1);
      standardWin1->SetAwardState(ReceivedFromGame);
      standardWin1->SetSASLevelId(11);
      standardWin1->SetLevelId(111);
      standardWin1->SetJackpotId(1111);
      standardWin1->SetAwardAmountPaid(10101);
      ASSERT_EQ(true, wins.AddProgressiveWin(standardWin1)) << "AddProgressiveWin 1";
      wins.ProgressivePaidSentReceived(standardWin1);

      // Verify the non-default last standard win.
      wins.GetLastStandardWin(lastStandardWin);
      ASSERT_TRUE(NULL != (void *)lastStandardWin.get()) << "lastStandardWin.get";
      CUnitTestingUtils::AssertEqual(lastStandardWin->GetReceivedFromEGMTime(), receivedFromEGMTime1,
                                     _T("GetReceivedFromEGMTime 1"), 0, __TFUNCTION__, __LINE__);
      ASSERT_EQ(ReceivedFromGame, lastStandardWin->GetAwardState()) << "GetAwardState 1";
      ASSERT_EQ((int)11, lastStandardWin->GetSASLevelId()) << "GetSASLevelId 1";
      ASSERT_EQ((int)111, lastStandardWin->GetLevelId()) << "GetLevelId 1";
      ASSERT_EQ((LONG64)1111, lastStandardWin->GetJackpotId()) << "GetJackpotId 1";
      ASSERT_EQ((__int64)10101, lastStandardWin->GetAwardAmountPaid()) << "GetAwardAmountPaid 1";
   }

   {
      // Perform operation(s) and test(s).
      // Recover the last standard win from NVRAM.
      StandardProgressiveWins wins(true);
      std::shared_ptr<CEGMAward> lastStandardWin;
      wins.GetLastStandardWin(lastStandardWin);

      // Verify the non-default last standard win.
      ASSERT_TRUE(NULL != (void *)lastStandardWin.get()) << "lastStandardWin.get";
      CUnitTestingUtils::AssertEqual(lastStandardWin->GetReceivedFromEGMTime(), receivedFromEGMTime1,
                                     _T("GetReceivedFromEGMTime 1"), 0, __TFUNCTION__, __LINE__);
      ASSERT_EQ(ReceivedFromGame, lastStandardWin->GetAwardState()) << "GetAwardState 1";
      ASSERT_EQ((int)11, lastStandardWin->GetSASLevelId()) << "GetSASLevelId 1";
      ASSERT_EQ((int)111, lastStandardWin->GetLevelId()) << "GetLevelId 1";
      ASSERT_EQ((LONG64)1111, lastStandardWin->GetJackpotId()) << "GetJackpotId 1";
      ASSERT_EQ((__int64)10101, lastStandardWin->GetAwardAmountPaid()) << "GetAwardAmountPaid 1";
   }
}
// #pragma endregion GetLastStandardWin

// #pragma region GetStandardWins
/// <summary>
/// Functional test for:
///     StandardProgressiveWins::GetStandardWins()
/// Test with a default StandardProgressiveWins instance.
/// </summary>
TEST_F(StandardProgressiveWinsTests, GetStandardWins_DefaultTest)
{
   // Setup.
   StandardProgressiveWins wins;
   std::vector<std::shared_ptr<CEGMAward>> standardWins;

   // Perform operation(s) and test(s).
   wins.GetStandardWins(standardWins);
   ASSERT_EQ((size_t)0, standardWins.size()) << "standardWins.size";
}

/// <summary>
/// Functional test for:
///     StandardProgressiveWins::GetStandardWins()
/// Test with a StandardProgressiveWins instance with one current win.
/// </summary>
TEST_F(StandardProgressiveWinsTests, GetStandardWins_OneTest)
{
   // Setup.
   StandardProgressiveWins wins;
   std::vector<std::shared_ptr<CEGMAward>> standardWins;

   shared_ptr<CEGMAward> standardWin1(new CEGMAward());
   const SYSTEMTIME receivedFromEGMTime1 = { 2000, 1, 0, 2, 01, 01, 01, 001 };
   standardWin1->SetReceivedFromEGMTime(receivedFromEGMTime1);
   standardWin1->SetAwardState(ReceivedFromGame);
   standardWin1->SetSASLevelId(11);
   standardWin1->SetLevelId(111);
   standardWin1->SetJackpotId(1111);
   standardWin1->SetAwardAmountPaid(10101);
   ASSERT_EQ(true, wins.AddProgressiveWin(standardWin1)) << "AddProgressiveWin 1";

   // Perform operation(s) and test(s).
   wins.GetStandardWins(standardWins);
   ASSERT_EQ((size_t)1, standardWins.size()) << "standardWins.size";
   shared_ptr<CEGMAward> win1(standardWins[0]);
   ASSERT_TRUE(NULL != win1.get()) << "get 1";
   CUnitTestingUtils::AssertEqual(win1->GetReceivedFromEGMTime(), receivedFromEGMTime1,
                                  _T("GetReceivedFromEGMTime 1"), 0, __TFUNCTION__, __LINE__);
   ASSERT_EQ(ReceivedFromGame, win1->GetAwardState()) << "GetAwardState 1";
   ASSERT_EQ((int)11, win1->GetSASLevelId()) << "GetSASLevelId 1";
   ASSERT_EQ((int)111, win1->GetLevelId()) << "GetLevelId 1";
   ASSERT_EQ((LONG64)1111, win1->GetJackpotId()) << "GetJackpotId 1";
   ASSERT_EQ((__int64)10101, win1->GetAwardAmountPaid()) << "GetAwardAmountPaid 1";
}

/// <summary>
/// Functional test for:
///     StandardProgressiveWins::GetStandardWins()
/// Test with a StandardProgressiveWins instance with two current wins.
/// </summary>
TEST_F(StandardProgressiveWinsTests, GetStandardWins_OneTwoTest)
{
   // Setup.
   StandardProgressiveWins wins;
   std::vector<std::shared_ptr<CEGMAward>> standardWins;

   shared_ptr<CEGMAward> standardWin1(new CEGMAward());
   const SYSTEMTIME receivedFromEGMTime1 = { 2000, 1, 0, 2, 01, 01, 01, 001 };
   standardWin1->SetReceivedFromEGMTime(receivedFromEGMTime1);
   standardWin1->SetAwardState(ReceivedFromGame);
   standardWin1->SetSASLevelId(11);
   standardWin1->SetLevelId(111);
   standardWin1->SetJackpotId(1111);
   standardWin1->SetAwardAmountPaid(10101);
   ASSERT_EQ(true, wins.AddProgressiveWin(standardWin1)) << "AddProgressiveWin 1";

   shared_ptr<CEGMAward> standardWin2(new CEGMAward());
   const SYSTEMTIME receivedFromEGMTime2 = { 2000, 1, 0, 2, 02, 02, 02, 002 };
   standardWin2->SetReceivedFromEGMTime(receivedFromEGMTime2);
   standardWin2->SetAwardState(HandpayPending);
   standardWin2->SetSASLevelId(22);
   standardWin2->SetLevelId(222);
   standardWin2->SetJackpotId(2222);
   standardWin2->SetAwardAmountPaid(20202);
   ASSERT_EQ(true, wins.AddProgressiveWin(standardWin2)) << "AddProgressiveWin 2";

   // Perform operation(s) and test(s).
   wins.GetStandardWins(standardWins);
   ASSERT_EQ((size_t)2, standardWins.size()) << "standardWins.size";

   shared_ptr<CEGMAward> win1(standardWins[0]);
   ASSERT_TRUE(NULL != win1.get()) << "get 1";
   CUnitTestingUtils::AssertEqual(win1->GetReceivedFromEGMTime(), receivedFromEGMTime1,
                                  _T("GetReceivedFromEGMTime 1"), 0, __TFUNCTION__, __LINE__);
   ASSERT_EQ(ReceivedFromGame, win1->GetAwardState()) << "GetAwardState 1";
   ASSERT_EQ((int)11, win1->GetSASLevelId()) << "GetSASLevelId 1";
   ASSERT_EQ((int)111, win1->GetLevelId()) << "GetLevelId 1";
   ASSERT_EQ((LONG64)1111, win1->GetJackpotId()) << "GetJackpotId 1";
   ASSERT_EQ((__int64)10101, win1->GetAwardAmountPaid()) << "GetAwardAmountPaid 1";

   shared_ptr<CEGMAward> win2(standardWins[1]);
   ASSERT_TRUE(NULL != win2.get()) << "get 2";
   CUnitTestingUtils::AssertEqual(win2->GetReceivedFromEGMTime(), receivedFromEGMTime2,
                                  _T("GetReceivedFromEGMTime 2"), 0, __TFUNCTION__, __LINE__);
   ASSERT_EQ(HandpayPending, win2->GetAwardState()) << "GetAwardState 2";
   ASSERT_EQ((int)22, win2->GetSASLevelId()) << "GetSASLevelId 2";
   ASSERT_EQ((int)222, win2->GetLevelId()) << "GetLevelId 2";
   ASSERT_EQ((LONG64)2222, win2->GetJackpotId()) << "GetJackpotId 2";
   ASSERT_EQ((__int64)20202, win2->GetAwardAmountPaid()) << "GetAwardAmountPaid 2";
}
// #pragma endregion GetStandardWins
};

/// <summary>
/// Finds the award.
/// </summary>
/// <param name="awardAmount">The award amount.</param>
/// <param name="levelId">The level identifier.</param>
/// <returns>if it finds it will return the shared pointer to an egm award</returns>
std::shared_ptr<CEGMAward> SentinelUnitTests::StandardProgressiveWinsFriend::FindAward(__int64 awardAmount, int levelId)
{
   return StandardProgressiveWins::FindAward(awardAmount, levelId);
}

void SentinelUnitTests::StandardProgressiveWinsFriend::RemoveAward(__int64 awardAmount, int levelId)
{
   return StandardProgressiveWins::RemoveAward(awardAmount, levelId);
}

SentinelUnitTests::StandardProgressiveWinsFriend::StandardProgressiveWinsFriend(bool buildFromNvram) : StandardProgressiveWins(buildFromNvram)
{
}

SentinelUnitTests::CProgressiveLevelFriend::CProgressiveLevelFriend(ProgressiveLevelInfo& levelInfo, DWORD updatedTickCount) : CProgressiveLevel(levelInfo, updatedTickCount)
{
}

SentinelUnitTests::ProgressiveLevelInfoFriend::ProgressiveLevelInfoFriend(int64_t levelId, ProgressiveLevelType::Type type, const rapidjson::Value& level) :
   ProgressiveLevelInfo(levelId, type, level)
{
}
