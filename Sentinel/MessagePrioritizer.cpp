#include "stdafx.h"
#include "MessagePrioritizer.h"
#include "UI/ScreenDefs.h"
#include "ProgressiveProtocol/ProgressiveWinnerPick.h"
#include "ProgressiveProtocol/ProgressiveWinnerPicked.h"

using namespace std;

const int EMPLOYEE_SCREEN(3000);
const int DEFAULT_MEDIA_MIN_SECONDS = 10;
const int DEFAULT_MEDIA_MAX_SECONDS = 20;
const int DEFAULT_SPLASHDOWN_START_TTL = 10000;
const int UJP_PIN_TIMEOUT = 60000;
const int UJP_TEASER_TIMEOUT = 300000;

CMessagePrioritizer::CMessagePrioritizer() :
m_nRichAwardInProgress(false)
{
	BuildMessagePriorityMap();
}

CMessagePrioritizer::~CMessagePrioritizer()
{
	m_messagePriorityMap.clear();
}

void CMessagePrioritizer::BuildMessagePriorityMap()
{
	lock_guard<recursive_mutex> lock(m_criticalSection);
	CQueuingConfig *config = LoadConfigFile();
	m_queuingConfig = *config;
	delete config;

	m_messagePriorityMap.clear();
	int pbtscreens = m_queuingConfig.GetPbtScreens();
	DWORD userscreens = m_queuingConfig.GetUserScreenTime();
	m_smpromo = m_queuingConfig.GetSmPromo();
	m_smpromottl = m_queuingConfig.GetSmPromoTTL();
	int announcement = m_queuingConfig.GetAnnouncements();
	DWORD announcementttl = m_queuingConfig.GetAnnouncementsTTL();
	int playerinitiated = m_queuingConfig.GetPlayerInitiated();
	DWORD playerinitiatedttl = m_queuingConfig.GetPlayerInitiatedTTL();
	int congrats = m_queuingConfig.GetCongrats();
	//DWORD congratsttl = m_queuingConfig.GetCongratsTTL(); not currently used. Should perhaps the CONGRATSTTL reference below be changed to this?
	int birthday = m_queuingConfig.GetBirthday();
	DWORD birthdayttl = m_queuingConfig.GetBirthdayTTL();
	int anniversary = m_queuingConfig.GetAnniversary();
	DWORD anniversaryttl = m_queuingConfig.GetAnniversaryTTL();
	//int interactivecontent = m_queuingConfig.GetInteractiveContent(); not currently used
	//DWORD interactivecontentstarvetime = m_queuingConfig.GetInteractiveContentStarveTime(); not currently used
	//DWORD interactivecontentttl = m_queuingConfig.GetInteractiveContentTTL(); not currently used
	DWORD ujpPinTimeout = UJP_PIN_TIMEOUT;
	int winner = m_queuingConfig.GetWinnerMessage();
	DWORD winnerttl = m_queuingConfig.GetWinnerMessageTTL();
	int scheduleEnd = m_queuingConfig.GetScheduleEnd();
	DWORD scheduleEndTTL = m_queuingConfig.GetScheduleEndTTL();
	int anticipation = m_queuingConfig.GetWinnerAnticipation();
	DWORD anticipationttl = m_queuingConfig.GetWinnerAnticipationTTL();
	DWORD scheduleendttl = m_queuingConfig.GetWinnerAnticipationTTL() * 3;//TODO we can revisit all these TTL and move them all to Extensible Settings.
	int massMarketing = m_queuingConfig.GetMassMarketing();
	DWORD massMarketingttl = m_queuingConfig.GetMassMarketingTTL();
	int cdm = m_queuingConfig.GetCDM();
	//DWORD cdmttl = m_queuingConfig.GetCDMTTL(); not currently used
	int unattendedJackpot = m_queuingConfig.GetUnattendedJackpot();
	DWORD unattendedJackpotTTL = m_queuingConfig.GetUnattendedJackpotTTL();
	int unattendedJackpotMedia = m_queuingConfig.GetUnattendedJackpotMedia();
	DWORD unattendedJackpotMediaTTL = m_queuingConfig.GetUnattendedJackpotMediaTTL();
	int invitedGroupsAnticipation = m_queuingConfig.GetInvitedGroupsAnticipation();
	DWORD invitedGroupsAnticipationTTL = m_queuingConfig.GetInvitedGroupsAnticipationTTL();
	int invitedGroupsEndingSoon = m_queuingConfig.GetInvitedGroupsEndingSoon();
	DWORD invitedGroupsEndingSoonTTL = m_queuingConfig.GetInvitedGroupsEndingSoonTTL();
	int invitedGroupsFinalResults = m_queuingConfig.GetInvitedGroupsFinalResults();
	DWORD invitedGroupsFinalResultsTTL = m_queuingConfig.GetInvitedGroupsFinalResultsTTL();
	int invitedGroupsLeaderBoard = m_queuingConfig.GetInvitedGroupsLeaderBoard();
	DWORD invitedGroupsLeaderBoardTTL = m_queuingConfig.GetInvitedGroupsLeaderBoardTTL();
	int invitedGroupsLevelUnlocked = m_queuingConfig.GetInvitedGroupsLevelUnlocked();
	DWORD invitedGroupsLevelUnlockedTTL = m_queuingConfig.GetInvitedGroupsLevelUnlockedTTL();
	int progressivePrizePriority = m_queuingConfig.GetProgressivePrize();
	DWORD progressivePrizeTTL = m_queuingConfig.GetProgressivePrizeTTL();
	int promoGamePriority = m_queuingConfig.GetPromoGame();
	DWORD promoGameTTL = m_queuingConfig.GetPromoGameTTL();

	// Promotions
	//This is an exception to the rules (we want INT_MSG_SPLASHDOWN_START to expire
	//at a HARD CODED value of DEFAULT_SPLASHDOWN_START_TTL)
	BuildPriorityInfo(INT_MSG_SPLASHDOWN_START, m_smpromo, m_smpromo, PERSIST_CONTENT_WINDOW, DEFAULT_SPLASHDOWN_START_TTL, false, POPUPDISPLAYUNLIMITED);
	BuildPriorityInfo(INT_MSG_SPLASHDOWN_AWARD, m_smpromo, m_smpromo, PERSIST_CONTENT_WINDOW, m_smpromottl, false, POPUPDISPLAYUNLIMITED);
	BuildPriorityInfo(INT_MSG_RICOCHET_TRIGGER, m_smpromo, m_smpromo, PERSIST_CONTENT_WINDOW, m_smpromottl, false, POPUPDISPLAYUNLIMITED);
	BuildPriorityInfo(INT_MSG_RICOCHET_AWARD, m_smpromo, m_smpromo, PERSIST_CONTENT_WINDOW, m_smpromottl, false, POPUPDISPLAYUNLIMITED);
	BuildPriorityInfo(INT_MSG_CASINO_CHALLENGE_INCREMENT, m_smpromo, m_smpromo, PERSIST_CONTENT_WINDOW, m_smpromottl, false, 1);//NO STARVATION
	BuildPriorityInfo(INT_MSG_CASINO_CHALLENGE_END, m_smpromo, m_smpromo, PERSIST_CONTENT_WINDOW, m_smpromottl, false, POPUPDISPLAYUNLIMITED);
	BuildPriorityInfo(INT_MSG_CASINO_CHALLENGE_AWARD, m_smpromo, m_smpromo, PERSIST_CONTENT_WINDOW, m_smpromottl, false, POPUPDISPLAYUNLIMITED);
	BuildPriorityInfo(INT_MSG_CASINO_CHALLENGE_FIVE_MINUTE_END, m_smpromo, m_smpromo, PERSIST_CONTENT_WINDOW, m_smpromottl, false, POPUPDISPLAYUNLIMITED);
	BuildPriorityInfo(INT_MSG_CASINO_CHALLENGE_TRIGGER, m_smpromo, m_smpromo, PERSIST_CONTENT_WINDOW, m_smpromottl, false, POPUPDISPLAYUNLIMITED);
	BuildPriorityInfo(INT_MSG_PROGRESSIVE_JACKPOT_ANNOUNCEMENT, announcement, announcement, PERSIST_CONTENT_WINDOW, announcementttl, false, POPUPDISPLAYUNLIMITED);
	BuildPriorityInfo(INT_MSG_PROGRESSIVE_TOTALIZER_ANNOUNCEMENT, announcement, announcement, PERSIST_CONTENT_WINDOW, announcementttl, false, POPUPDISPLAYUNLIMITED);
	BuildPriorityInfo(INT_MSG_SPLASHDOWN_WINNER_END, m_smpromo, m_smpromo, PERSIST_CONTENT_WINDOW, m_smpromottl, false, POPUPDISPLAYUNLIMITED);
	BuildPriorityInfo(INT_MSG_SPLASHDOWN_END, m_smpromo, m_smpromo, PERSIST_CONTENT_WINDOW, m_smpromottl, false, POPUPDISPLAYUNLIMITED);
	BuildPriorityInfo(INT_MSG_PROGRESSIVE_WINNER_PICKED, winner, winner, PERSIST_CONTENT_WINDOW, winnerttl, false, POPUPDISPLAYUNLIMITED);
	BuildPriorityInfo(INT_MSG_DISPLAY_PTE_AWARD, winner, winner, PERSIST_CONTENT_WINDOW, winnerttl, false, POPUPDISPLAYUNLIMITED);
	BuildPriorityInfo(INT_MSG_DISPLAY_PTE_ENDCOUNTDOWN, invitedGroupsEndingSoon, invitedGroupsEndingSoon, PERSIST_CONTENT_WINDOW, invitedGroupsEndingSoonTTL, false, POPUPDISPLAYUNLIMITED);
	BuildPriorityInfo(INT_MSG_DISPLAY_PTE_LEVELUP, invitedGroupsLevelUnlocked, invitedGroupsLevelUnlocked, PERSIST_CONTENT_WINDOW, invitedGroupsLevelUnlockedTTL, false, POPUPDISPLAYUNLIMITED);
	BuildPriorityInfo(INT_MSG_DISPLAY_PTE_LEADERBOARD, invitedGroupsLeaderBoard, invitedGroupsLeaderBoard, PERSIST_CONTENT_WINDOW, invitedGroupsLeaderBoardTTL, false, POPUPDISPLAYUNLIMITED);
	BuildPriorityInfo(INT_MSG_DISPLAY_PTE_RESULTS, invitedGroupsFinalResults, invitedGroupsFinalResults, PERSIST_CONTENT_WINDOW, invitedGroupsFinalResultsTTL, false, POPUPDISPLAYUNLIMITED);
	BuildPriorityInfo(INT_MSG_PROGRESSIVE_SCHEDULE_END, scheduleEnd, scheduleEnd, PERSIST_CONTENT_WINDOW, scheduleEndTTL, false, POPUPDISPLAYUNLIMITED);
	BuildPriorityInfo(INT_MSG_PROGRESSIVE_WINNER_PICK, anticipation, anticipation, PERSIST_CONTENT_WINDOW, anticipationttl, false, POPUPDISPLAYUNLIMITED);	
	BuildPriorityInfo(INT_MSG_PROGRESSIVE_CLAIMABLE_AWARD, massMarketing, massMarketing, PERSIST_CONTENT_WINDOW, massMarketingttl, false, POPUPDISPLAYUNLIMITED);
	BuildPriorityInfo(INT_MSG_DISPLAY_PTE_ANTICIPATION, invitedGroupsAnticipation, invitedGroupsAnticipation, PERSIST_CONTENT_WINDOW, invitedGroupsAnticipationTTL, false, POPUPDISPLAYUNLIMITED);
	BuildPriorityInfo(INT_MSG_PROGRESSIVE_PRIZE, progressivePrizePriority, progressivePrizePriority, PERSIST_CONTENT_WINDOW, progressivePrizeTTL, false, POPUPDISPLAYUNLIMITED);
	BuildPriorityInfo(INT_MSG_PROGRESSIVE_PROMO_GAME, promoGamePriority, promoGamePriority, PERSIST_CONTENT_WINDOW, promoGameTTL, false, POPUPDISPLAYUNLIMITED);

	const int INTERRUPT_PBT_POPUPDISPLAYUNLIMITED = pbtscreens - 1;
	const int PERSIST_PBT_POPUP = pbtscreens - 2;
	// Sending with priority INTERRUPT_PBT_POPUPDISPLAYUNLIMITED will allow POPUPDISPLAYUNLIMITED time to be interrupted.
	// Starving POPUPDISPLAY time at PERSIST_PBT_POPUP will ensure it gets displayed for the entire time.

	// Honor PBT starvation, but no new starvation
	BuildPriorityInfo(INT_MSG_ACTIVE_SESSION, pbtscreens, pbtscreens, PERSIST_CONTENT_WINDOW, 0, false, 0);
	BuildPriorityInfo(INT_MSG_CLEAR_PBT, pbtscreens, INTERRUPT_PBT_POPUPDISPLAYUNLIMITED, PERSIST_CONTENT_WINDOW, 0, false, 0);

	//POPUP DISPLAYTIME
	BuildPriorityInfo(INT_MSG_PB_COMPLETE, PERSIST_PBT_POPUP, INTERRUPT_PBT_POPUPDISPLAYUNLIMITED, PERSIST_CONTENT_WINDOW, 0, false, POPUPDISPLAYTIME);
	BuildPriorityInfo(INT_MSG_PB_NCEPOUT_FAIL, PERSIST_PBT_POPUP, INTERRUPT_PBT_POPUPDISPLAYUNLIMITED, PERSIST_CONTENT_WINDOW, 0, false, POPUPDISPLAYTIME);
	BuildPriorityInfo(INT_MSG_AUTO_TRANSFER_CANCELLED, PERSIST_PBT_POPUP, INTERRUPT_PBT_POPUPDISPLAYUNLIMITED, PERSIST_CONTENT_WINDOW, 0, false, POPUPDISPLAYTIME);
	BuildPriorityInfo(INT_MSG_PB_INFO, pbtscreens, INTERRUPT_PBT_POPUPDISPLAYUNLIMITED, PERSIST_CONTENT_WINDOW, 0, false, POPUPDISPLAYTIME);
	BuildPriorityInfo(INT_MSG_SHOW_OTHER_BUTTON_ERROR, PERSIST_PBT_POPUP, INTERRUPT_PBT_POPUPDISPLAYUNLIMITED, PERSIST_CONTENT_WINDOW, 0, false, POPUPDISPLAYTIME);
	BuildPriorityInfo(INT_MSG_DOLLAR_BUTTON_ERROR, PERSIST_PBT_POPUP, INTERRUPT_PBT_POPUPDISPLAYUNLIMITED, PERSIST_CONTENT_WINDOW, 0, false, POPUPDISPLAYTIME);
	BuildPriorityInfo(INT_MSG_PROBE_PBT_RESULT, PERSIST_PBT_POPUP, INTERRUPT_PBT_POPUPDISPLAYUNLIMITED, PERSIST_CONTENT_WINDOW, 0, false, POPUPDISPLAYTIME);
	BuildPriorityInfo(INT_MSG_PB_PIN_RESET_RESULT, PERSIST_PBT_POPUP, INTERRUPT_PBT_POPUPDISPLAYUNLIMITED, PERSIST_CONTENT_WINDOW, 0, false, POPUPDISPLAYTIME);	
	BuildPriorityInfo(INT_MSG_DISPLAY_CASHOUT_PBT_ERROR, PERSIST_PBT_POPUP, INTERRUPT_PBT_POPUPDISPLAYUNLIMITED, PERSIST_CONTENT_WINDOW, 0, false, POPUPDISPLAYTIME);
	BuildPriorityInfo(INT_MSG_DISPLAY_AUTOUPLOAD_ERROR, PERSIST_PBT_POPUP, INTERRUPT_PBT_POPUPDISPLAYUNLIMITED, PERSIST_CONTENT_WINDOW, 0, false, POPUPDISPLAYTIME);
	BuildPriorityInfo(INT_MSG_CHECK_UPLOADABLE_CREDITS, PERSIST_PBT_POPUP, INTERRUPT_PBT_POPUPDISPLAYUNLIMITED, PERSIST_CONTENT_WINDOW, 0, false, POPUPDISPLAYTIME_ONESECOND);

	//POPUP DISPLAYUNLIMITED	
	BuildPriorityInfo(INT_MSG_START_PB_STARVATION, pbtscreens, INTERRUPT_PBT_POPUPDISPLAYUNLIMITED, PERSIST_CONTENT_WINDOW, 0, false, POPUPDISPLAYUNLIMITED);
	BuildPriorityInfo(INT_MSG_BAD_CARD_READ, pbtscreens, INTERRUPT_PBT_POPUPDISPLAYUNLIMITED, PERSIST_CONTENT_WINDOW, 0, false, POPUPDISPLAYUNLIMITED);
	BuildPriorityInfo(INT_MSG_PB_CHECK_DENOM, pbtscreens, INTERRUPT_PBT_POPUPDISPLAYUNLIMITED, PERSIST_CONTENT_WINDOW, 0, false, POPUPDISPLAYUNLIMITED);
	BuildPriorityInfo(INT_MSG_PROCESS_BUTTON_PRESS, pbtscreens, INTERRUPT_PBT_POPUPDISPLAYUNLIMITED, PERSIST_CONTENT_WINDOW, 0, false, POPUPDISPLAYUNLIMITED);
	BuildPriorityInfo(INT_MSG_DOLLAR_BUTTON_SUCCESS, pbtscreens, INTERRUPT_PBT_POPUPDISPLAYUNLIMITED, PERSIST_CONTENT_WINDOW, 0, false, POPUPDISPLAYUNLIMITED);
	BuildPriorityInfo(INT_MSG_SHOW_CREDIT_TRANSFER_ENTRY_SCREEN, pbtscreens, INTERRUPT_PBT_POPUPDISPLAYUNLIMITED, PERSIST_CONTENT_WINDOW, 0, false, POPUPDISPLAYUNLIMITED);
	BuildPriorityInfo(INT_MSG_SHOW_NON_CREDIT_TRANSFER_ENTRY_SCREEN, pbtscreens, INTERRUPT_PBT_POPUPDISPLAYUNLIMITED, PERSIST_CONTENT_WINDOW, 0, false, POPUPDISPLAYUNLIMITED);
	BuildPriorityInfo(INT_MSG_CARDED_SESSION_IN_PROGRESS, pbtscreens, INTERRUPT_PBT_POPUPDISPLAYUNLIMITED, PERSIST_CONTENT_WINDOW, 0, false, POPUPDISPLAYUNLIMITED);
	BuildPriorityInfo(INT_MSG_TRANSFER_PENDING, pbtscreens, INTERRUPT_PBT_POPUPDISPLAYUNLIMITED, PERSIST_CONTENT_WINDOW, 0, false, POPUPDISPLAYUNLIMITED);
	BuildPriorityInfo(INT_MSG_DISPLAY_BANNED_PLAYER, PERSIST_PBT_POPUP, INTERRUPT_PBT_POPUPDISPLAYUNLIMITED, PERSIST_CONTENT_WINDOW, 0, false, POPUPDISPLAYUNLIMITED);
	BuildPriorityInfo(INT_MSG_DISPLAY_ACCESSING_ACCOUNT, pbtscreens, INTERRUPT_PBT_POPUPDISPLAYUNLIMITED, PERSIST_CONTENT_WINDOW, 0, false, POPUPDISPLAYUNLIMITED);
	BuildPriorityInfo(INT_MSG_NEW_PIN_REQUIRED, pbtscreens, INTERRUPT_PBT_POPUPDISPLAYUNLIMITED, PERSIST_CONTENT_WINDOW, 0, false, POPUPDISPLAYUNLIMITED);
	BuildPriorityInfo(INT_MSG_EGM_AWARD_STATUS, pbtscreens, INTERRUPT_PBT_POPUPDISPLAYUNLIMITED, PERSIST_CONTENT_WINDOW, 0, false, POPUPDISPLAYTIME);

	// Player	
	BuildPriorityInfo(INT_MSG_DISPLAY_ACCOUNT_BALANCE, playerinitiated, playerinitiated - 1, PERSIST_CONTENT_WINDOW, playerinitiatedttl, false, POPUPDISPLAYUNLIMITED);
	BuildPriorityInfo(INT_MSG_PLAY_CONGRATS, congrats, congrats, PERSIST_CONTENT_WINDOW, CONGRATSTTL, false, POPUPDISPLAYTIME);
	BuildPriorityInfo(INT_MSG_SHOW_ANNIVERSARY, anniversary, anniversary, PERSIST_CONTENT_WINDOW, anniversaryttl, false, POPUPDISPLAYTIME);
	BuildPriorityInfo(INT_MSG_SHOW_BIRTHDAY, birthday, birthday, PERSIST_CONTENT_WINDOW, birthdayttl, false, POPUPDISPLAYTIME);
	BuildPriorityInfo(INT_MSG_SHOW_BIRTHDAY_AND_ANNIVERSARY, birthday, birthday, PERSIST_CONTENT_WINDOW, birthdayttl, false, POPUPDISPLAYTIME * 2);
	BuildPriorityInfo(INT_MSG_DISPLAY_UJP, playerinitiated, playerinitiated - 1, PERSIST_CONTENT_WINDOW, 0, false, POPUPDISPLAYUNLIMITED);
	BuildPriorityInfo(INT_MSG_DISPLAY_UJP_INVALID_PIN, playerinitiated, playerinitiated - 1, PERSIST_CONTENT_WINDOW, 0, false, POPUPDISPLAYTIME);
	BuildPriorityInfo(INT_MSG_DISPLAY_PTE_LEADERBOARD_PLAYER_INITIATED, playerinitiated, playerinitiated - 1, PERSIST_CONTENT_WINDOW, playerinitiatedttl, false, POPUPDISPLAYUNLIMITED);
	BuildPriorityInfo(INT_MSG_DISPLAY_AUTOCARDOUT, playerinitiated, playerinitiated - 1, PERSIST_CONTENT_WINDOW, playerinitiatedttl, false, POPUPDISPLAYUNLIMITED);	
	
	// Employee
	const int PERSIST_EMPLOYEE_SCREEN(PERSIST_PBT_POPUP - 1);
	const int INTERRUPT_EMPLOYEE_POPUPDISPLAYUNLIMITED(INTERRUPT_PBT_POPUPDISPLAYUNLIMITED - 1);

	BuildPriorityInfo(INT_MSG_EMPLOYEE_CARD_IN, PERSIST_EMPLOYEE_SCREEN, INTERRUPT_EMPLOYEE_POPUPDISPLAYUNLIMITED, PERSIST_CONTENT_WINDOW, 0, false, 0);
	BuildPriorityInfo(INT_MSG_DISPLAY_EMPLOYEE_SCREEN, PERSIST_EMPLOYEE_SCREEN, INTERRUPT_EMPLOYEE_POPUPDISPLAYUNLIMITED, PERSIST_CONTENT_WINDOW, 0, false, POPUPDISPLAYUNLIMITED);
}

CMessagePersist* CMessagePrioritizer::PrioritizeMessage(CInternalMessage *messageIn, int &priority, __int64 flushID)
{
	std::string flushIDString = std::to_string(flushID);

	return PrioritizeMessage(messageIn, priority, flushIDString);
}

CMessagePersist* CMessagePrioritizer::PrioritizeMessage(CInternalMessage *messageIn, int &priority, const std::string &flushID)
{
	CMessagePersist *newPersist = NULL;

	if (NULL != messageIn)
	{
		lock_guard<recursive_mutex> lock(m_criticalSection);
		MessagePriorityInfo priorityInfo;
		FindMessagePriority(messageIn->GetMessageType(), priorityInfo);

		priority = priorityInfo.messagePriority;
		DWORD interactiveTicks(priorityInfo.queueStarve == 0 ? 0 : m_queuingConfig.GetInitialInteractiveStarveTime());
		int interactivePriority = m_queuingConfig.GetInteractiveContent();

		// Check if this needs to be an interactive message
		if (priorityInfo.interactive)
		{
			interactiveTicks = m_queuingConfig.GetInteractiveContentStarveTime();
		}

		newPersist = new CMessagePersist(false, priorityInfo.queueStarve, priorityInfo.persistPriority, priorityInfo.persistType, interactiveTicks, interactivePriority);

		// Set the TTL for the message
		messageIn->SetMessageTTL(priorityInfo.ttl);

		// Set the flush type of the persist object
		MessageFlushType flushType = GetMessageFlushType(messageIn->GetMessageType());
		if (NoFlushType != flushType)
		{
			if (PBFlushType == flushType)
			{
				newPersist->SetFlushType(PBFlushType);
			}
			else
			{
				newPersist->SetFlushTypeAndId(flushType, flushID);
			}
		}

		// See if this message is a flusher type message
		if (IsFlusherMessage(messageIn->GetMessageType()))
		{
			newPersist->SetAsFlusher();
		}

	}

	return newPersist;
}

CMessagePersist *CMessagePrioritizer::PrioritizeMessage(CInternalMessage *messageIn)
{
	int priority = QUEUE_PRIORITY_NORMAL;
	CMessagePersist *newPersist = PrioritizeMessage(messageIn, priority);

	return newPersist;
}

CMessagePersist *CMessagePrioritizer::PrioritizeAnnouncementMessage(CInternalMessage *messageIn, const std::string& screenType, int &priority, __int64 flushID)
{
	CMessagePersist *newPersist = PrioritizeMessage(messageIn, priority, flushID);

	// Adjust for promotion screen types that are treated as announcements
	if (messageIn->GetMessageType() == INT_MSG_JACKPOT_ANNOUNCEMENT)
	{
		if (screenType == CARD_IN_PROMOTION ||
			screenType == CARDIN_ELIGIBILITY_SCREEN ||
			screenType == PROXIMITY_SCREEN ||
			screenType == PNW_END_SCREEN ||
			screenType == EVENT_TRIGGERED_SCREEN ||
			screenType == NONWINNER_NOTIFICATION_SCREEN)
		{
			newPersist->SetMainStarvation(m_smpromo, POPUPDISPLAYUNLIMITED);
			messageIn->SetMessageTTL(m_smpromottl);
			priority = m_smpromo;
		}
	}

	// See if this is a flusher message.
	// This has to be checked again because we have to account for the screen ID.
	if (IsFlusherMessage(messageIn->GetMessageType(), screenType))
	{
		newPersist->SetAsFlusher();
	}

	return newPersist;
}

bool CMessagePrioritizer::FindMessagePriority(InternalMsgType msgType, MessagePriorityInfo &priorityInfo)
{
	if (m_messagePriorityMap.find(msgType) != m_messagePriorityMap.end())
	{
		priorityInfo = m_messagePriorityMap.find(msgType)->second;
		return true;
	}
	else // InternalMsgType was not found in the map so set Default values
	{
		priorityInfo.persistPriority = QUEUE_PRIORITY_LOWEST;
		priorityInfo.messagePriority = QUEUE_PRIORITY_NORMAL;
		priorityInfo.persistType = PERSIST_CONTENT_WINDOW;
		priorityInfo.interactive = false;
		priorityInfo.queueStarve = -1;
		priorityInfo.ttl = 0;
		return false;
	}
}

void CMessagePrioritizer::BuildPriorityInfo(InternalMsgType msgType,
												int persistPriority,
												int messagePriority,
												MessagePersistType persistType,
												DWORD ttl,
												bool interactive,
												int queueStarve )
{
	MessagePriorityInfo pinfo;
	pinfo.persistPriority = persistPriority;
	pinfo.messagePriority = messagePriority;
	pinfo.persistType = persistType;
	pinfo.queueStarve = queueStarve;
	pinfo.interactive = interactive;
	pinfo.ttl = ttl;
	m_messagePriorityMap.insert(std::pair<InternalMsgType, MessagePriorityInfo>(msgType, pinfo));
}

CQueuingConfig *CMessagePrioritizer::LoadConfigFile()
{
	CQueuingConfigFile configFile;
	configFile.Start();
	CQueuingConfig* queuingConfig(configFile.GetQueuingConfigCopy());


	return queuingConfig;

}

bool CMessagePrioritizer::GetEqualPriorityInterrupt()
{
	return m_queuingConfig.GetEqualPriorityInterrupt();
}

CMessagePersist *CMessagePrioritizer::PrioritizeWinnerPickedMessage(CInternalMessage *msg, int& priority, CProgressiveWinnerPicked *winner)
{
	CMessagePersist* messagePersist(NULL);

	if (msg != NULL && winner != NULL)
	{
		std::string flushId(to_string(winner->GetLevelId()));
		flushId.append("-" + to_string(winner->GetJackpotId()) + "-" + winner->GetWinAmount());
		messagePersist = PrioritizeMessage(msg, priority, flushId);
		int interactiveStarvation(winner->GetMediaMinSeconds() > 0 ? winner->GetMediaMinSeconds() : DEFAULT_MEDIA_MIN_SECONDS);
		int mainStarvation(winner->GetMediaMaxSeconds() > 0 ? winner->GetMediaMaxSeconds() : DEFAULT_MEDIA_MAX_SECONDS);
		// Adjust message priority based on digits.
		int length = winner->GetWinAmount().length();

		if (priority > length)
		{
			priority -= length;
			if (winner->GetMinorAwardId().empty())
			{
				priority -= 1;
			}
            else
            {
                mainStarvation = DEFAULT_MEDIA_MIN_SECONDS;
            }
		}

		messagePersist->SetInteractiveStarvation(m_queuingConfig.GetInteractiveContent(), interactiveStarvation * TICK_COUNTS_PER_SECOND);
		messagePersist->SetMainStarvation(priority, mainStarvation * TICK_COUNTS_PER_SECOND);
	}
	
	return messagePersist;
}

MessageFlushType CMessagePrioritizer::GetMessageFlushType(InternalMsgType msgType)
{
	MessageFlushType flushType = NoFlushType;

	if (IsPBTMessage(msgType))
	{
		flushType = PBFlushType;
	}
	else if (IsPromotionMessage(msgType))
	{
		flushType = PromotionFlushType;
	}
	else if (IsJackpotMessage(msgType))
	{
		flushType = JackpotFlushType;
	}
	else if (IsPteFlushtype(msgType))
	{
		flushType = PteFlushType;
	}
	else if (IsEmployeeFlushtype(msgType))
	{
		flushType = EmployeeFlushType;
	}

	return flushType;
}

bool CMessagePrioritizer::IsFlusherMessage(InternalMsgType msgType, const string& screeenType)
{
	bool isFlusherMessage = false;

	// Removed INT_MSG_JACKPOT_ANNOUNCEMENT case where screentType == PNW_END_SCREEN because it caused FWMB celebration to be flushed.
	switch (msgType)
	{
	case INT_MSG_PB_COMPLETE:
	case INT_MSG_DISPLAY_PTE_RESULTS:
	case INT_MSG_PROGRESSIVE_WINNER_PICKED:
	case INT_MSG_EMPLOYEE_CARD_IN:
		isFlusherMessage = true;
		break;
	default:
		isFlusherMessage = false;
		break;
	}

	return isFlusherMessage;
}

bool CMessagePrioritizer::IsPBTMessage(InternalMsgType msgType)
{
	bool retVal = false;

	switch (msgType)
	{
	case INT_MSG_CLEAR_PBT:
	case INT_MSG_PB_CHECK_DENOM:
	case INT_MSG_PB_COMPLETE:
	case INT_MSG_PB_PIN_RESET_RESULT:
	case INT_MSG_NEW_PIN_REQUIRED:
	case INT_MSG_PB_NCEPOUT_FAIL:
	case INT_MSG_AUTO_TRANSFER_CANCELLED:
	case INT_MSG_PB_INFO:
	case INT_MSG_SHOW_OTHER_BUTTON_ERROR:
		retVal = true;
		break;
	default:
		retVal = false;
		break;
	}

	return retVal;
}

bool CMessagePrioritizer::IsPromotionMessage(InternalMsgType msgType)
{
	bool retVal = false;

	switch (msgType)
	{
	case INT_MSG_CASINO_CHALLENGE_END:
	case INT_MSG_CASINO_CHALLENGE_TRIGGER:
	case INT_MSG_CASINO_CHALLENGE_INCREMENT:
	case INT_MSG_CASINO_CHALLENGE_AWARD:
	case INT_MSG_CASINO_CHALLENGE_FIVE_MINUTE_END:
	case INT_MSG_SPLASHDOWN_START:
	case INT_MSG_SPLASHDOWN_AWARD:
	case INT_MSG_SPLASHDOWN_END:
	case INT_MSG_SPLASHDOWN_WINNER_END:
	case INT_MSG_RICOCHET_TRIGGER:
	case INT_MSG_RICOCHET_AWARD:
	case INT_MSG_JACKPOT_ANNOUNCEMENT:
		retVal = true;
		break;
	default:
		retVal = false;
		break;
	}

	return retVal;
}

bool CMessagePrioritizer::IsPteFlushtype(InternalMsgType msgType)
{
	bool retVal = false;

	switch (msgType)
	{
	case INT_MSG_DISPLAY_PTE_ENDCOUNTDOWN:
	case INT_MSG_DISPLAY_PTE_ANTICIPATION:
	case INT_MSG_DISPLAY_PTE_LEADERBOARD:
	case INT_MSG_DISPLAY_PTE_LEVELUP:
	case INT_MSG_DISPLAY_PTE_RESULTS:
		retVal = true;
		break;
	default:
		retVal = false;
		break;
	}

	return retVal;
}

bool CMessagePrioritizer::IsEmployeeFlushtype(InternalMsgType msgType)
{
	bool retVal = false;

	switch (msgType)
	{
	case INT_MSG_EMPLOYEE_CARD_IN:
	case INT_MSG_DISPLAY_EMPLOYEE_SCREEN:
		retVal = true;
		break;
	default:
		retVal = false;
		break;
	}

	return retVal;
}

bool CMessagePrioritizer::IsJackpotMessage(InternalMsgType msgType)
{
	bool retVal = false;

	switch (msgType)
	{
	case INT_MSG_PROGRESSIVE_SCHEDULE_END:
	case INT_MSG_PROGRESSIVE_WINNER_PICK:
	case INT_MSG_PROGRESSIVE_WINNER_PICKED:
		retVal = true;
		break;
	default:
		retVal = false;
		break;
	}

	return retVal;
}

CQueuingConfig& CMessagePrioritizer::GetQueuingConfig()
{
	return m_queuingConfig;
}
