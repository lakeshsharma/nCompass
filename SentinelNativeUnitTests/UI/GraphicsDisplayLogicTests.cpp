#if 000//TODO-PORT
#include "stdafx.h"
#include "UI\GraphicsDisplayLogic.h"
#include "TestUtil.h"
#include "Util\Convert.h"
#include "Util\FileSys.h"

using namespace pugi;
using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace TestUtil;

namespace SentinelNativeUnitTests
{
	class CGraphicsDisplayLogicTest : public CGraphicsDisplayLogic
	{
	private:
		bool m_tftMode;
		ContentWindowOrientation m_ontentWindowOrientation;

	public:
		CGraphicsDisplayLogicTest(LPCTSTR pDataDrive, bool tftMode, ContentWindowOrientation ServiceWindowOrientation)
			: CGraphicsDisplayLogic(pDataDrive)
			, m_tftMode(tftMode)
			, m_ontentWindowOrientation(ServiceWindowOrientation)
		{
		}

	protected:
		bool IsTFTMode()
		{
			return m_tftMode;
		}

		ContentWindowOrientation GetServiceWindowOrientation()
		{
			return m_ontentWindowOrientation;
		}
	};

	static map<int, int> typeMap;
	static map<CString, int> externalTypeMap;
	const int NullType = -1;
	const int InvalidType = -2;

	TEST_CLASS(GraphicsDisplayLogicTests)
	{
		TEST_CLASS_INITIALIZE(GraphicsDisplayLogicTests_Init)
		{
			typeMap[SplashdownStartScr] = screenSplashdownStart;
			typeMap[SplashdownAwardScr] = screenSplashdownAward;
			typeMap[SplashdownFinaleScr] = screenSplashdownEnd;
			typeMap[SplashdownWinnerFinaleScr] = screenSplashdownWinnerEnd;
			typeMap[RicochetStartScr] = screenRicochetStart;
			typeMap[RicochetAwardScr] = screenRicochetCelebration;
			typeMap[JackpotAnnouncementForCardedPlayersScr] = screenJackpotAnnouncerCarded;
			typeMap[JackpotAnnouncementForNonCardedPlayersScr] = screenJackpotAnnouncerNonCarded;
			typeMap[Flash] = screenNone;
			typeMap[TotalizerAnnouncementScr] = screenTriggerlessAnnouncer;
			typeMap[CasinoChallengeStartScr] = screenCasinoChallengeStart;
			typeMap[CasinoChallengeAward1Scr] = screenCasinoChallengeAward1;
			typeMap[CasinoChallengeAward2Scr] = screenCasinoChallengeAward2;
			typeMap[CasinoChallengeAward3Scr] = screenCasinoChallengeAward3;
			typeMap[CasinoChallengeFiveMinuteScr] = screenCasinoChallengeFiveMinuteWarningEnd;
			typeMap[CasinoChallengeEndScr] = screenScheduleEnd;
			typeMap[__reserved_23] = screenNone;
			typeMap[__reserved_24] = screenNone;
			typeMap[__reserved_25] = screenNone;
			typeMap[__reserved_26] = screenNone;
			typeMap[CardInScr] = screenTriggerlessAnnouncer;
			typeMap[PlayerMessageScr] = screenTriggerlessAnnouncer;
			typeMap[EventEligibilityScr] = screenCardInEligibility;
			typeMap[EventProximityScr] = screenProximity;
			typeMap[PnWinCelebration] = screenRicochetCelebration;
			typeMap[PnWinScheduleEnd] = screenScheduleEnd;
			typeMap[EventTriggeredScr] = screenEventTriggered;
			typeMap[__reserved_34] = screenNone;
			typeMap[NonWinnerNotificationScr] = screenNonWinnerMessage;

			externalTypeMap[L"Anticipation"] = screenWinnerPick;
			externalTypeMap[L"Celebration"] = screenInvitedGroupsAward;
			externalTypeMap[L"Odometer"] = screenProgressiveOdometer;
			externalTypeMap[L"Event Proximity"] = screenInvitedGroupsAnticipation;
			externalTypeMap[L"Leaderboard"] = screenInvitedGroupsLeaderboard;
			externalTypeMap[L"Bonus Level Unlocked"] = screenInvitedGroupsLevelUp;
			externalTypeMap[L"Player Progressive Indicator"] = screenPteOdometer;
			externalTypeMap[L"Ending Soon"] = screenInvitedGroupsEnd;
			externalTypeMap[L"Final Results"] = screenInvitedGroupsResults;

		}

		TEST_METHOD_CLEANUP(CardReaderTestsMethodCleanup)
		{
			DeleteSentinelDirectories();
		}

		void LoadXml(CGraphicsDisplayLogicTest& graphicsDisplayLogic)
		{
			CDirectoryManager::MockInstance();

			//DeleteOnCloseFile mediaXmlFile(DIRECTORY_DEFAULTMEDIA, L"media.xml", (const Lzma::Header*) MediaXml::data);
			CString mediaXmlPathname(GetnCompassPath(L"DefaultMediaXML") + L"media.xml");
			Assert::IsTrue(FileSys::FileExists(mediaXmlPathname), AssertMsg::FormatOnly(L"media.xml pathname not found: '%s'", (LPCWSTR) mediaXmlPathname));

			HRESULT hr = graphicsDisplayLogic.LoadXML(mediaXmlPathname);
			Assert::AreEqual(S_OK, hr, AssertMsg::FormatOnly(L"graphicsDisplayLogic.LoadXML(%s)", (LPCTSTR) mediaXmlPathname));
		}

		TEST_METHOD(GraphicsDisplayLogic_GetEventMediaScreen)
		{
			screenID screenId = screenNone;
			CGraphicsDisplayLogicTest graphicsDisplayLogic(_T(""), false, CWORIENTATION_LEFT);
			LoadXml(graphicsDisplayLogic);

			for (int screenType = __geteventmediascreen_first__; screenType < __geteventmediascreen_last__; screenType++)
			{
				screenId = graphicsDisplayLogic.GetScreenIDForSpeedMediaScreenType((SpeedMediaScreenTypes)screenType);

				auto it = typeMap.find(screenType);
				Assert::IsTrue(it->second == screenId, AssertMsg::FormatOnly(L"For ScreenType %d expected ScreenID %d, received %d", screenType, it->second, screenId));
			}

			Assert::IsTrue(graphicsDisplayLogic.GetScreenIDForExternalScreen(L"Anticipation") == screenWinnerPick, L"ScreenID for Anticpation incorrect");
			Assert::IsTrue(graphicsDisplayLogic.GetScreenIDForExternalScreen(L"Celebration") == screenInvitedGroupsAward, L"ScreenID for Celebration incorrect");
			Assert::IsTrue(graphicsDisplayLogic.GetScreenIDForExternalScreen(L"Odometer") == screenProgressiveOdometer, L"ScreenID for Odometer incorrect");
			Assert::IsTrue(graphicsDisplayLogic.GetScreenIDForExternalScreen(L"Event Proximity") == screenInvitedGroupsAnticipation, L"ScreenID for Event Proximity incorrect");
			Assert::IsTrue(graphicsDisplayLogic.GetScreenIDForExternalScreen(L"Leaderboard") == screenInvitedGroupsLeaderboard, L"ScreenID for Leaderboard incorrect");
			Assert::IsTrue(graphicsDisplayLogic.GetScreenIDForExternalScreen(L"Bonus Level Unlocked") == screenInvitedGroupsLevelUp, L"ScreenID for Bonus Level Unlocked incorrect");
			Assert::IsTrue(graphicsDisplayLogic.GetScreenIDForExternalScreen(L"Player Progressive Indicator") == screenPteOdometer, L"ScreenID for Player Progressive Indicator incorrect");
			Assert::IsTrue(graphicsDisplayLogic.GetScreenIDForExternalScreen(L"Ending Soon") == screenInvitedGroupsEnd, L"ScreenID for Ending Soon incorrect");
			Assert::IsTrue(graphicsDisplayLogic.GetScreenIDForExternalScreen(L"Final Results") == screenInvitedGroupsResults, L"ScreenID for Final Results incorrect");
			Assert::IsTrue(graphicsDisplayLogic.GetScreenIDForExternalScreen(L"Bogus Screen") == screenGenericExternal, L"ScreenID for unknown screen name should be screenGenericExternal");

			CScreenInfo *screenInfo = graphicsDisplayLogic.CreateEventScreenInfo(screenGenericExternal);
			Assert::IsTrue(screenInfo->GetScreenID() == screenGenericExternal, L"Screen ID in created screen object should be screenGenericExternal");

			CPanelInfo *panel = screenInfo->GetPanelByID(MEDIA_PANEL);
			Assert::IsFalse(panel == NULL, L"Created screen object should contain a media panel");
		}

		TEST_METHOD(GraphicsDisplayLogic_ConvertToIntScreenType)
		{
			struct
			{
				int line;
				const wchar_t* pStr;
				int screenType;
			}
			static data[] = 
			{
				{ __LINE__, L"Splashdown Countdown Trigger"				, SplashdownStartScr		},
				{ __LINE__, L"Splashdown Countdown Celebration"			, SplashdownAwardScr		},
				{ __LINE__, L"Splashdown Countdown Finale"				, SplashdownFinaleScr		},
				{ __LINE__, L"Splashdown Countdown Winner Finale"		, SplashdownWinnerFinaleScr },
				{ __LINE__, L"Ricochet Rewards Trigger"					, RicochetStartScr			},
				{ __LINE__, L"Ricochet Rewards Celebration"				, RicochetAwardScr			},
				{ __LINE__, L"Attractor"								, AttractorScr				},
				{ __LINE__, L"Birthday"									, BirthdayScr				},
				{ __LINE__, L"Congratulations"							, CongratsScr				},
				{ __LINE__, L"Anniversary"								, AnniversaryScr			},
				{ __LINE__, L"In Play-Carded"							, InCardedPlayScr			},
				{ __LINE__, L"Welcome"									, WelcomeScr				},
				{ __LINE__, L"xJackpot Announcement+Carded Players."	, JackpotAnnouncementForCardedPlayersScr	},
				{ __LINE__, L"xJackpot Announcement+Noncarded Players."	, JackpotAnnouncementForNonCardedPlayersScr },

				{ __LINE__, L"Splashdown Countdown Trigger "			, -1 },
				{ __LINE__, L"Splashdown Countdown Celebration "		, -1 },
				{ __LINE__, L"Splashdown Countdown Finale "				, -1 },
				{ __LINE__, L"Splashdown Countdown Winner Finale "		, -1 },
				{ __LINE__, L"Ricochet Rewards Trigger "				, -1 },
				{ __LINE__, L"Ricochet Rewards Celebration "			, -1 },
				{ __LINE__, L"Attractor "								, -1 },
				{ __LINE__, L"Birthday "								, -1 },
				{ __LINE__, L"Congratulations "							, -1 },
				{ __LINE__, L"Anniversary "								, -1 },
				{ __LINE__, L"In Play-Carded "							, -1 },
				{ __LINE__, L"Welcome "									, -1 },
				{ __LINE__, L"xJackpot Announcement+carded Players."	, -1 },
				{ __LINE__, L"xJackpot Announcement+noncarded Players."	, -1 }
			};

			CGraphicsDisplayLogicTest graphicsDisplayLogic(_T(""), false, CWORIENTATION_LEFT);
			LoadXml(graphicsDisplayLogic);

			for (size_t i = 0; i < _countof(data); i++)
			{
				int result = graphicsDisplayLogic.ConvertToIntScreenType(CString(data[i].pStr), false);
				Assert::AreEqual((int) data[i].screenType, result, AssertMsg::Format(data[i].line, data[i].pStr));
			}
		}
	};
}
#endif //TODO-PORT