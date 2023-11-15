#include "stdafx.h"

#include "GlobalDefs.h"
#include "../SpeedMedia/SpeedMediaEvent.h"


#include "gtest/gtest.h"


namespace SentinelUnitTests
{

		TEST(SpeedMediaEventUnitTests, SpeedMediaEventDefaultConstructor)
		{
			//Setup
			CSpeedMediaEvent *smEvent = new CSpeedMediaEvent();

			//Tests
			ASSERT_TRUE( smEvent->GetEventID() == -1) << "EventID is incorrect" ;
			ASSERT_TRUE( smEvent->GetEventStartTrigger() == NULL) << "EventStartTrigger is incorrect" ;
			int cardIDLen;
			ASSERT_TRUE( smEvent->GetCardID( cardIDLen ) == NULL) << "Card ID is incorrect" ;
			ASSERT_TRUE( cardIDLen == 0) << "Card ID Len is incorrect" ;
			ASSERT_TRUE( smEvent->GetCardIDLen() == 0) << "Card ID Len is incorrect" ;
			ASSERT_TRUE( smEvent->GetContentID() == -1) << "ContentID is incorrect";
			ASSERT_TRUE( smEvent->GetLastTriggeredEventID() == -1) << "LastTriggeredEventID is incorrect";
			ASSERT_TRUE( smEvent->GetIncludeCarded()) << "IncludeCarded flag incorrect" ;
			ASSERT_FALSE( smEvent->GetIncludeUncarded()) << "IncludeUncarded flag incorrect" ;
			ASSERT_FALSE( smEvent->IncludeInRandomSelection()) << "IncludeInRandomSelection flag incorrect" ;
			ASSERT_TRUE(smEvent->GetNumberOfGoalLevels() == -1) << "m_numberOfGoalLevels is incorrect";

			//Cleanup
			delete smEvent;
		}

		
		TEST(SpeedMediaEventUnitTests, SpeedMediaEventSetGet)
		{
			//Setup
			CSpeedMediaEvent *smEvent = new CSpeedMediaEvent();
			const int EventID = 1234;
			const int numberOfGoalLevels = 3;
			smEvent->SetEventID( EventID );
			CSpeedMediaTriggers *startTrigger = new CSpeedMediaTriggers();
			smEvent->SetEventStartTrigger( startTrigger );
			const int CardIDLen = 5;
			byte cardID[CardIDLen] = {1,2,3,4,5};
			smEvent->SetCardID( cardID, CardIDLen );
			const int contentID = 576212;
			smEvent->SetContentID( contentID );
			const int LastTriggeredEventID = 891234;
			smEvent->SetLastTriggeredEventID( LastTriggeredEventID );
			smEvent->SetIncludeCarded( false );
			smEvent->SetIncludeUncarded( true );
			smEvent->SetIncludeInRandomSelection( true );
			smEvent->SetNumberOfGoalLevels(numberOfGoalLevels);

			//Tests
			ASSERT_TRUE( smEvent->GetEventID() == EventID) << "EventID is incorrect" ;
			ASSERT_TRUE( smEvent->GetEventStartTrigger() == startTrigger) << "EventStartTrigger is incorrect" ;
			int cardIDLen;
			ASSERT_TRUE( memcmp( smEvent->GetCardID( cardIDLen ), cardID, CardIDLen) == 0) << "Card ID is incorrect" ;
			ASSERT_TRUE( cardIDLen == CardIDLen) << "Card ID Len is incorrect" ;
			ASSERT_TRUE( smEvent->GetCardIDLen() == CardIDLen) << "Card ID Len is incorrect" ;
			ASSERT_TRUE( smEvent->GetContentID() == contentID) << "ContentID is incorrect";
			ASSERT_TRUE( smEvent->GetLastTriggeredEventID() == LastTriggeredEventID) << "LastTriggeredEventID is incorrect";
			ASSERT_FALSE( smEvent->GetIncludeCarded()) << "IncludeCarded flag incorrect" ;
			ASSERT_TRUE( smEvent->GetIncludeUncarded()) << "IncludeUncarded flag incorrect" ;
			ASSERT_TRUE( smEvent->IncludeInRandomSelection()) << "IncludeInRandomSelection flag incorrect" ;
			ASSERT_TRUE(smEvent->GetNumberOfGoalLevels() == numberOfGoalLevels) << "m_numberOfGoalLevels is incorrect";

			//Cleanup
			delete smEvent;
		}


		TEST(SpeedMediaEventUnitTests, SpeedMediaEventCopyConstructor)
		{
			//Setup
			CSpeedMediaEvent *originalEvent = new CSpeedMediaEvent();
			const int EventID = 1234;
			const int numberOfGoalLevels = 3;
			originalEvent->SetEventID( EventID );
			CSpeedMediaTriggers *startTrigger = new CSpeedMediaTriggers();
			startTrigger->Reset(1,2,3,4,5,6,7,8);
			originalEvent->SetEventStartTrigger( startTrigger );
			const int CardIDLen = 5;
			byte cardID[CardIDLen] = {1,2,3,4,5};
			originalEvent->SetCardID( cardID, CardIDLen );
			const int contentID = 576212;
			originalEvent->SetContentID( contentID );
			const int LastTriggeredEventID = 891234;
			originalEvent->SetLastTriggeredEventID( LastTriggeredEventID );
			originalEvent->SetIncludeCarded( false );
			originalEvent->SetIncludeUncarded( true );
			originalEvent->SetIncludeInRandomSelection( true );
			originalEvent->SetNumberOfGoalLevels(numberOfGoalLevels);

			CSpeedMediaEvent *smEvent = new CSpeedMediaEvent(*originalEvent);

			//Tests
			ASSERT_TRUE( smEvent->GetEventID() == EventID) << "EventID is incorrect" ;
			ASSERT_FALSE( smEvent->GetEventStartTrigger() == startTrigger) << "EventStartTrigger should not be a copy of original" ;
			ASSERT_TRUE( smEvent->GetEventStartTrigger() != NULL) << "EventStartTrigger should not be null after copy" ;
			ASSERT_TRUE( smEvent->GetEventStartTrigger()->GetInitialCoinIn() == 1) << "EventStartTrigger is incorrect after copy" ;
			int cardIDLen;
			ASSERT_TRUE( memcmp( smEvent->GetCardID( cardIDLen ), cardID, CardIDLen) == 0) << "Card ID is incorrect" ;
			ASSERT_TRUE( cardIDLen == CardIDLen) << "Card ID Len is incorrect" ;
			ASSERT_TRUE( smEvent->GetCardIDLen() == CardIDLen) << "Card ID Len is incorrect" ;
			ASSERT_TRUE( smEvent->GetContentID() == contentID) << "ContentID is incorrect";
			ASSERT_TRUE( smEvent->GetLastTriggeredEventID() == LastTriggeredEventID) << "LastTriggeredEventID is incorrect";
			ASSERT_FALSE( smEvent->GetIncludeCarded()) << "IncludeCarded flag incorrect" ;
			ASSERT_TRUE( smEvent->GetIncludeUncarded()) << "IncludeUncarded flag incorrect" ;
			ASSERT_TRUE( smEvent->IncludeInRandomSelection()) << "IncludeInRandomSelection flag incorrect" ;
			ASSERT_TRUE(smEvent->GetNumberOfGoalLevels() == numberOfGoalLevels) << "m_numberOfGoalLevels is incorrect";

			//Cleanup
			delete smEvent;
			delete originalEvent;
		}

		TEST(SpeedMediaEventUnitTests, SetCardIdTest)
		{
			byte cardId[CARD_ID_LEN] = { 0, 0, 0, 0, 0 };
			CSpeedMediaEvent sme;
			sme.SetCardID(cardId, sizeof(cardId));

			int cardIDLen;
			const byte * const cardIdResult(sme.GetCardID(cardIDLen));
			ASSERT_TRUE(cardIDLen == sizeof(cardId)) << "cardIDLen";
			ASSERT_TRUE(cardIdResult != NULL) << "cardIdResult IsNotNull";
			ASSERT_TRUE(0 == memcmp(cardIdResult, cardId, sizeof(cardId))) << "memcmp(cardIdResult)";
		}
}
