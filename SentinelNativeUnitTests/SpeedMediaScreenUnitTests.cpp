#if 000//TODO-PORT
#include "stdafx.h"
#include "UnitTestingUtils.h"
#include "SpeedMediaScreen.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SentinelNativeUnitTests
{
	static const LPCTSTR DefaultConnection = L"AttractorConnection";

	TEST_CLASS(SpeedMediaScreenUnitTests)
	{
	public:
		TEST_METHOD(SpeedMediaAnnouncementScreenTest)
		{
			CString screenTypeString(ANNOUNCEMENT_SCREEN);
			TriggerLessType triggerLessType(InvalidTriggerLessType);
			int screenId(screenNone);
			CSpeedMediaScreen speedMediaScreenId(screenTypeString, _T(""), screenId, triggerLessType);
			Assert::IsTrue(screenTriggerlessAnnouncer == screenId, L"screenID not setup properly.");
			Assert::IsTrue(InvalidTriggerLessType == triggerLessType, L"TriggerLessType not setup properly.");
		}

		TEST_METHOD(SpeedMediaCardinPromotionScreenTest)
		{
			CString screenTypeString(CARD_IN_PROMOTION);
			TriggerLessType triggerLessType(InvalidTriggerLessType);
			int screenId(screenNone);
			CSpeedMediaScreen speedMediaScreenId(screenTypeString, _T(""), screenId, triggerLessType);
			Assert::IsTrue(screenTriggerlessAnnouncer == screenId, L"screenID not setup properly.");
			Assert::IsTrue(CardinPromotion == triggerLessType, L"TriggerLessType not setup properly.");
		}

		TEST_METHOD(SpeedMediaPlayerAnnouncemenScreenTest)
		{
			CString screenTypeString(PLAYER_ANNOUNCMENT);
			TriggerLessType triggerLessType(InvalidTriggerLessType);
			int screenId(screenNone);
			CSpeedMediaScreen speedMediaScreenId(screenTypeString, _T(""), screenId, triggerLessType);
			Assert::IsTrue(screenTriggerlessAnnouncer == screenId, L"screenID not setup properly.");
			Assert::IsTrue(PlayerAnnouncement == triggerLessType, L"TriggerLessType not setup properly.");
		}

		TEST_METHOD(SpeedMediaTotalizerScreenTest)
		{
			CString screenTypeString(TOTALIZER_SCREEN);
			TriggerLessType triggerLessType(InvalidTriggerLessType);
			int screenId(screenNone);
			CSpeedMediaScreen speedMediaScreenId(screenTypeString, _T(""), screenId, triggerLessType);
			Assert::IsTrue(screenTriggerlessAnnouncer == screenId, L"screenID not setup properly.");
			Assert::IsTrue(Totalizer == triggerLessType, L"TriggerLessType not setup properly.");
		}

		TEST_METHOD(SpeedMediaCardInEligibilityScreenTest)
		{
			CString screenTypeString(CARDIN_ELIGIBILITY_SCREEN);
			TriggerLessType triggerLessType(InvalidTriggerLessType);
			int screenId(screenNone);
			CSpeedMediaScreen speedMediaScreenId(screenTypeString, _T(""), screenId, triggerLessType);
			Assert::IsTrue(screenCardInEligibility == screenId, L"screenID not setup properly.");
			Assert::IsTrue(InvalidTriggerLessType == triggerLessType, L"TriggerLessType not setup properly.");
		}

		TEST_METHOD(SpeedMediaProximityScreenTest)
		{
			CString screenTypeString(PROXIMITY_SCREEN);
			TriggerLessType triggerLessType(InvalidTriggerLessType);
			int screenId(screenNone);
			CSpeedMediaScreen speedMediaScreenId(screenTypeString, _T(""), screenId, triggerLessType);
			Assert::IsTrue(screenProximity == screenId, L"screenID not setup properly.");
			Assert::IsTrue(InvalidTriggerLessType == triggerLessType, L"TriggerLessType not setup properly.");
		}

		TEST_METHOD(SpeedMediaJackpotAnnouncerCardedScreenTest)
		{
			CString screenTypeString(JPACARDED_SCREEN);
			TriggerLessType triggerLessType(InvalidTriggerLessType);
			int screenId(screenNone);
			CSpeedMediaScreen speedMediaScreenId(screenTypeString, _T(""), screenId, triggerLessType);
			Assert::IsTrue(screenJackpotAnnouncerCarded == screenId, L"screenID not setup properly.");
			Assert::IsTrue(InvalidTriggerLessType == triggerLessType, L"TriggerLessType not setup properly.");
		}

		TEST_METHOD(SpeedMediaJackpotAnnouncerNonCardedScreenTest)
		{
			CString screenTypeString(JPANONCARDED_SCREEN);
			TriggerLessType triggerLessType(InvalidTriggerLessType);
			int screenId(screenNone);
			CSpeedMediaScreen speedMediaScreenId(screenTypeString, _T(""), screenId, triggerLessType);
			Assert::IsTrue(screenJackpotAnnouncerNonCarded == screenId, L"screenID not setup properly.");
			Assert::IsTrue(InvalidTriggerLessType == triggerLessType, L"TriggerLessType not setup properly.");
		}

		TEST_METHOD(SpeedMediaScheduleEndScreenTest)
		{
			CString screenTypeString(PNW_END_SCREEN);
			TriggerLessType triggerLessType(InvalidTriggerLessType);
			int screenId(screenNone);
			CSpeedMediaScreen speedMediaScreenId(screenTypeString, _T(""), screenId, triggerLessType);
			Assert::IsTrue(screenScheduleEnd == screenId, L"screenID not setup properly.");
			Assert::IsTrue(InvalidTriggerLessType == triggerLessType, L"TriggerLessType not setup properly.");
		}

		TEST_METHOD(SpeedMediaEventTriggeredScreenTest)
		{
			CString screenTypeString(EVENT_TRIGGERED_SCREEN);
			TriggerLessType triggerLessType(InvalidTriggerLessType);
			int screenId(screenNone);
			CSpeedMediaScreen speedMediaScreenId(screenTypeString, _T(""), screenId, triggerLessType);
			Assert::IsTrue(screenEventTriggered == screenId, L"screenID not setup properly.");
			Assert::IsTrue(InvalidTriggerLessType == triggerLessType, L"TriggerLessType not setup properly.");
		}

		TEST_METHOD(SpeedMediaNonWinnerMessageScreenTest)
		{
			CString screenTypeString(NONWINNER_NOTIFICATION_SCREEN);
			TriggerLessType triggerLessType(InvalidTriggerLessType);
			int screenId(screenNone);
			CSpeedMediaScreen speedMediaScreenId(screenTypeString, _T(""), screenId, triggerLessType);
			Assert::IsTrue(screenNonWinnerMessage == screenId, L"screenID not setup properly.");
			Assert::IsTrue(InvalidTriggerLessType == triggerLessType, L"TriggerLessType not setup properly.");
		}

		TEST_METHOD(SpeedMediaShowScreenWithoutAPlayerTest)
		{
			CString screenTypeString(PROXIMITY_SCREEN);
			TriggerLessType triggerLessType(InvalidTriggerLessType);
			int screenId(screenNone);
			CSpeedMediaScreen speedMediaScreenId(screenTypeString, _T(""), screenId, triggerLessType);
			Assert::IsTrue(screenProximity == screenId, L"screenID not setup properly.");
			Assert::IsTrue(InvalidTriggerLessType == triggerLessType, L"TriggerLessType not setup properly.");
			Assert::IsTrue(speedMediaScreenId.ShowScreen(NULL), L"ShowScreen should be true.");
		}

		TEST_METHOD(SpeedMediaShowScreenWithOcrWithoutAPlayerTest)
		{
			CString screenTypeString(PROXIMITY_SCREEN);
			TriggerLessType triggerLessType(InvalidTriggerLessType);
			int screenId(screenNone);
			CSpeedMediaScreen speedMediaScreenId(screenTypeString, _T("12345678"), screenId, triggerLessType);
			Assert::IsTrue(screenProximity == screenId, L"screenID not setup properly.");
			Assert::IsTrue(InvalidTriggerLessType == triggerLessType, L"TriggerLessType not setup properly.");
			Assert::IsFalse(speedMediaScreenId.ShowScreen(NULL), L"ShowScreen should be true.");
		}

		TEST_METHOD(SpeedMediaShowScreenWithOcrWithAMatchingPlayerOcrTest)
		{
			// Setup.
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			const byte cardId[CARD_ID_LEN] = { 0x15, 0x26, 0x37, 0x48, 0x59 };
			const CString cardIdStringExpected(_T("1526374859"));
			CCard *card(new CCard(CTime(0), CTime(0)));
			card->SetCardInfo(CARD_PLAYER, cardId, CARD_ID_LEN);
			CPlayerInfo *playerInfo(new CPlayerInfo);			
			CPlayer player(card, playerInfo, config, egmConfig, CTime(0), false);

			CString screenTypeString(PROXIMITY_SCREEN);
			TriggerLessType triggerLessType(InvalidTriggerLessType);
			int screenId(screenNone);
			CSpeedMediaScreen speedMediaScreenId(screenTypeString, cardIdStringExpected, screenId, triggerLessType);
			Assert::IsTrue(screenProximity == screenId, L"screenID not setup properly.");
			Assert::IsTrue(InvalidTriggerLessType == triggerLessType, L"TriggerLessType not setup properly.");
			Assert::IsTrue(speedMediaScreenId.ShowScreen(&player), L"ShowScreen should be true.");
		}

		TEST_METHOD(SpeedMediaShowScreenWithOcrWithAMatchingBannedPlayerOcrTest)
		{
			// Setup.
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			const byte cardId[CARD_ID_LEN] = { 0x15, 0x26, 0x37, 0x48, 0x59 };
			const CString cardIdStringExpected(_T("1526374859"));
			CCard *card(new CCard(CTime(0), CTime(0)));
			card->SetCardInfo(CARD_PLAYER, cardId, CARD_ID_LEN);
			CPlayerInfo *playerInfo(new CPlayerInfo);
			playerInfo->SetFlags(false, false, true);//make it banned player
			CPlayer player(card, playerInfo, config, egmConfig, CTime(0), false);

			CString screenTypeString(PROXIMITY_SCREEN);
			TriggerLessType triggerLessType(InvalidTriggerLessType);
			int screenId(screenNone);
			CSpeedMediaScreen speedMediaScreenId(screenTypeString, cardIdStringExpected, screenId, triggerLessType);
			Assert::IsTrue(screenProximity == screenId, L"screenID not setup properly.");
			Assert::IsTrue(InvalidTriggerLessType == triggerLessType, L"TriggerLessType not setup properly.");
			Assert::IsFalse(speedMediaScreenId.ShowScreen(&player), L"ShowScreen should be false.");
		}

		TEST_METHOD(SpeedMediaShowScreenWithOcrWithAMatchingMaxCardLimitReachedPlayerOcrTest)
		{
			// Setup.
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			const byte cardId[CARD_ID_LEN] = { 0x15, 0x26, 0x37, 0x48, 0x59 };
			const CString cardIdStringExpected(_T("1526374859"));
			CCard *card(new CCard(CTime(0), CTime(0)));
			card->SetCardInfo(CARD_PLAYER, cardId, CARD_ID_LEN);
			CPlayerInfo *playerInfo(new CPlayerInfo);
			playerInfo->MaxCardLimitReached(true);//make it MaxCardLimitReached player
			CPlayer player(card, playerInfo, config, egmConfig, CTime(0), false);

			CString screenTypeString(PROXIMITY_SCREEN);
			TriggerLessType triggerLessType(InvalidTriggerLessType);
			int screenId(screenNone);
			CSpeedMediaScreen speedMediaScreenId(screenTypeString, cardIdStringExpected, screenId, triggerLessType);
			Assert::IsTrue(screenProximity == screenId, L"screenID not setup properly.");
			Assert::IsTrue(InvalidTriggerLessType == triggerLessType, L"TriggerLessType not setup properly.");
			Assert::IsFalse(speedMediaScreenId.ShowScreen(&player), L"ShowScreen should be false.");
		}
	};
}
#endif //TODO-PORT