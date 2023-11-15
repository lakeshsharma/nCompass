#include "stdafx.h"
#include "QueuingConfig.h"

const LPCTSTR CQueuingConfig::EQUAL_PRIORITY_INTERRUPT(_T("EqualPriorityInterrupt"));
const LPCTSTR CQueuingConfig::PRIORITY_INITIALINTERACTIVESTARVETIME(_T("InitialInteractiveStarveTime"));
const LPCTSTR CQueuingConfig::PRIORITY_INTERACTIVECONTENTTOTALSTARVETIME(_T("InteractiveContentStarveTime"));
const LPCTSTR CQueuingConfig::PRIORITY_USERSCREENTIME(_T("UserScreenTime"));
const LPCTSTR CQueuingConfig::PRIORITY_UJPTIMEOUT(_T("UJPTimeout"));
const LPCTSTR CQueuingConfig::PRIORITY_PBTSCREENS(_T("PBTScreens"));
const LPCTSTR CQueuingConfig::PRIORITY_PLAYERINITIATED(_T("PlayerInitiatedPriority"));
const LPCTSTR CQueuingConfig::PRIORITY_PLAYERINITIATEDTTL(_T("PlayerInitiatedTTL"));
const LPCTSTR CQueuingConfig::PRIORITY_INTERACTIVECONTENT(_T("InteractiveContentPriority"));
const LPCTSTR CQueuingConfig::PRIORITY_INTERACTIVECONTENTTTL(_T("InteractiveContentTTL"));
const LPCTSTR CQueuingConfig::PRIORITY_WINNERMESSAGE(_T("WinnerMessagePriority"));
const LPCTSTR CQueuingConfig::PRIORITY_WINNERTTL(_T("WinnerMessageTTL"));
const LPCTSTR CQueuingConfig::PRIORITY_SMPROMO(_T("SMPromoPriority"));
const LPCTSTR CQueuingConfig::PRIORITY_SMPROMOTTL(_T("SMPromoTTL"));
const LPCTSTR CQueuingConfig::PRIORITY_CONGRATS(_T("CongratsPriority"));
const LPCTSTR CQueuingConfig::PRIORITY_CONGRATSTTL(_T("CongratsTTL"));
const LPCTSTR CQueuingConfig::PRIORITY_SCHEDULEEND(_T("ScheduleEndPriority"));
const LPCTSTR CQueuingConfig::PRIORITY_SCHEDULEENDTTL(_T("ScheduleEndTTL"));
const LPCTSTR CQueuingConfig::PRIORITY_WINNERANTICIPATION(_T("WinnerAnticipationPriority"));
const LPCTSTR CQueuingConfig::PRIORITY_WINNERANTICIPATIONTTL(_T("WinnerAnticipationTTL"));
const LPCTSTR CQueuingConfig::PRIORITY_ANNOUNCEMENTS(_T("AnnouncementsPriority"));
const LPCTSTR CQueuingConfig::PRIORITY_ANNOUNCEMENTSTTL(_T("AnnouncementsTTL"));
const LPCTSTR CQueuingConfig::PRIORITY_BIRTHDAY(_T("BirthdayPriority"));
const LPCTSTR CQueuingConfig::PRIORITY_BIRTHDAYTTL(_T("BirthdayTTL"));
const LPCTSTR CQueuingConfig::PRIORITY_ANNIVERSARY(_T("AnniversaryPriority"));
const LPCTSTR CQueuingConfig::PRIORITY_ANNIVERSARYTTL(_T("AnniversaryTTL"));
const LPCTSTR CQueuingConfig::PRIORITY_MASSMARKETING(_T("MassMarketing"));
const LPCTSTR CQueuingConfig::PRIORITY_MASSMARKETINGTTL(_T("MassMarketingTTL"));
const LPCTSTR CQueuingConfig::PRIORITY_CDM(_T("CDM"));
const LPCTSTR CQueuingConfig::PRIORITY_CDMTTL(_T("CDMTTL"));
const LPCTSTR CQueuingConfig::PRIORITY_UNATTENDED_JACKPOT(_T("UnattendedJackpot"));
const LPCTSTR CQueuingConfig::PRIORITY_UNATTENDED_JACKPOT_TTL(_T("UnattendedJackpotTTL"));
const LPCTSTR CQueuingConfig::PRIORITY_UNATTENDED_JACKPOT_MEDIA(_T("UnattendedJackpotMedia"));
const LPCTSTR CQueuingConfig::PRIORITY_UNATTENDED_JACKPOT_MEDIA_TTL(_T("UnattendedJackpotMediaTTL"));
const LPCTSTR CQueuingConfig::PRIORITY_INVITED_GROUPS_ANTICIPATION(_T("InvitedGroupsAnticipation"));
const LPCTSTR CQueuingConfig::PRIORITY_INVITED_GROUPS_ANTICIPATIONTTL(_T("InvitedGroupsAnticipationTTL"));
const LPCTSTR CQueuingConfig::PRIORITY_INVITED_GROUPS_LEVELUNLOCKED(_T("InvitedGroupsLevelUnlocked"));
const LPCTSTR CQueuingConfig::PRIORITY_INVITED_GROUPS_LEVELUNLOCKEDTTL(_T("InvitedGroupsLevelUnlockedTTL"));
const LPCTSTR CQueuingConfig::PRIORITY_INVITED_GROUPS_FINALRESULTS(_T("InvitedGroupsFinalResults"));
const LPCTSTR CQueuingConfig::PRIORITY_INVITED_GROUPS_FINALRESULTSTTL(_T("InvitedGroupsFinalResultsTTL"));
const LPCTSTR CQueuingConfig::PRIORITY_INVITED_GROUPS_ENDINGSOON(_T("InvitedGroupsEndingSoon"));
const LPCTSTR CQueuingConfig::PRIORITY_INVITED_GROUPS_ENDINGSOONTTL(_T("InvitedGroupsEndingSoonTTL"));
const LPCTSTR CQueuingConfig::PRIORITY_INVITED_GROUPS_LEADERBOARD(_T("InvitedGroupsLeaderBoard"));
const LPCTSTR CQueuingConfig::PRIORITY_INVITED_GROUPS_LEADERBOARDTTL(_T("InvitedGroupsLeaderBoardTTL"));
const LPCTSTR CQueuingConfig::PRIORITY_PROGRESSIVEPRIZE(_T("ProgressivePrizePriority"));
const LPCTSTR CQueuingConfig::PRIORITY_PROGRESSIVEPRIZETTL(_T("ProgressivePrizeTTL"));
const LPCTSTR CQueuingConfig::PRIORITY_PROMOGAME(_T("PromoGamePriority"));
const LPCTSTR CQueuingConfig::PRIORITY_PROMOGAMETTL(_T("PromoGameTTL"));

CQueuingConfig::CQueuingConfig()
{
	SetDefaults();
}


CQueuingConfig::~CQueuingConfig()
{
}

void CQueuingConfig::SetDefaults()
{
	m_equalPriorityInterrupt = DEFAULT_EQUAL_PRIORITY_INTERRUPT;
	m_initialInteractiveStarveTime = DEFAULT_PRIORITY_INITIALINTERACTIVESTARVETIME;
	m_interactiveContentStarveTime = DEFAULT_PRIORITY_INTERACTIVECONTENTTOTALSTARVETIME;
	m_userScreenTime = DEFAULT_PRIORITY_USERSCREENTIME;
	m_ujpTimeout = DEFAULT_UJP_TIMEOUT;
	m_pbtScreens = DEFAULT_PRIORITY_PBTSCREENS;
	m_playerInitiated = DEFAULT_PRIORITY_PLAYERINITIATED;
	m_playerInitiatedTTL = DEFAULT_PRIORITY_PLAYERINITIATEDTTL;
	m_interactiveContent = DEFAULT_PRIORITY_INTERACTIVECONTENT;
	m_interactiveContentTTL = DEFAULT_PRIORITY_INTERACTIVECONTENTTTL;
	m_winnerMessage = DEFAULT_PRIORITY_WINNERMESSAGE;
	m_winnerMessageTTL = DEFAULT_PRIORITY_WINNERTTL;
	m_smPromo = DEFAULT_PRIORITY_SMPROMO;
	m_smPromoTTL = DEFAULT_PRIORITY_SMPROMOTTL;
	m_congrats = DEFAULT_PRIORITY_CONGRATS;
	m_congratsTTL = DEFAULT_PRIORITY_CONGRATSTTL;
	m_scheduleEnd = DEFAULT_PRIORITY_SCHEDULEEND;
	m_scheduleEndTTL = DEFAULT_PRIORITY_SCHEDULEENDTTL;
	m_winnerAnticipation = DEFAULT_PRIORITY_WINNERANTICIPATION;
	m_winnerAnticipationTTL = DEFAULT_PRIORITY_WINNERANTICIPATIONTTL;
	m_announcements = DEFAULT_PRIORITY_ANNOUNCEMENTS;
	m_announcementsTTL = DEFAULT_PRIORITY_ANNOUNCEMENTSTTL;
	m_birthday = DEFAULT_PRIORITY_BIRTHDAY;
	m_birthdayTTL = DEFAULT_PRIORITY_BIRTHDAYTTL;
	m_anniversary = DEFAULT_PRIORITY_ANNIVERSARY;
	m_anniversaryTTL = DEFAULT_PRIORITY_ANNIVERSARYTTL;
	m_massMarketing = DEFAULT_PRIORITY_MASSMARKETING;
	m_massMarketingTTL = DEFAULT_PRIORITY_MASSMARKETINGTTL;
	m_CDM = DEFAULT_PRIORITY_CDM;
	m_CDMTTL = DEFAULT_PRIORITY_CDMTTL;
	m_unattendedJackpot = DEFAULT_PRIORITY_UNATTENDED_JACKPOT;
	m_unattendedJackpotTTL = DEFAULT_PRIORITY_UNATTENDED_JACKPOT_TTL;
	m_unattendedJackpotMedia = DEFAULT_PRIORITY_UNATTENDED_JACKPOT_MEDIA;
	m_unattendedJackpotMediaTTL = DEFAULT_PRIORITY_UNATTENDED_JACKPOT_MEDIA_TTL;
	m_invitedGroupsAnticipation = DEFAULT_PRIORITY_INVITED_GROUPS_ANTICIPATION;
	m_invitedGroupsAnticipationTTL = DEFAULT_PRIORITY_INVITED_GROUPS_ANTICIPATIONTTL;
	m_invitedGroupsEndingSoon = DEFAULT_PRIORITY_INVITED_GROUPS_ENDINGSOON;
	m_invitedGroupsEndingSoonTTL = DEFAULT_PRIORITY_INVITED_GROUPS_ENDINGSOONTTL;
	m_invitedGroupsFinalResults = DEFAULT_PRIORITY_INVITED_GROUPS_FINALRESULTS;
	m_invitedGroupsFinalResultsTTL = DEFAULT_PRIORITY_INVITED_GROUPS_FINALRESULTSTTL;
	m_invitedGroupsLeaderBoard = DEFAULT_PRIORITY_INVITED_GROUPS_LEADERBOARD;
	m_invitedGroupsLeaderBoardTTL = DEFAULT_PRIORITY_INVITED_GROUPS_LEADERBOARDTTL;
	m_invitedGroupsLevelUnlocked = DEFAULT_PRIORITY_INVITED_GROUPS_LEVELUNLOCKED;
	m_invitedGroupsLevelUnlockedTTL = DEFAULT_PRIORITY_INVITED_GROUPS_LEVELUNLOCKEDTTL;
	m_progressivePrize = DEFAULT_PRIORITY_PROGRESSIVEPRIZE;
	m_progressivePrizeTTL = DEFAULT_PRIORITY_PROGRESSIVEPRIZETTL;
	m_promoGame = DEFAULT_PRIORITY_PROMOGAME;
	m_promoGameTTL = DEFAULT_PRIORITY_PROMOGAMETTL;

	m_hasConfigFile = DEFAULT_HAS_CONFIG_FILE;
}

CQueuingConfig::CQueuingConfig(const CQueuingConfig &rhs)
{
	*this = rhs;
}

CQueuingConfig& CQueuingConfig::operator=(const CQueuingConfig &rhs)
{
	if (this != &rhs)
	{
		SetDefaults();

		m_equalPriorityInterrupt = rhs.m_equalPriorityInterrupt;
		m_initialInteractiveStarveTime = rhs.m_initialInteractiveStarveTime;
		m_interactiveContentStarveTime = rhs.m_interactiveContentStarveTime;
		m_userScreenTime = rhs.m_userScreenTime;
		m_ujpTimeout = rhs.m_ujpTimeout;
		m_pbtScreens = rhs.m_pbtScreens;
		m_playerInitiated = rhs.m_playerInitiated;
		m_playerInitiatedTTL = rhs.m_playerInitiatedTTL;
		m_interactiveContent = rhs.m_interactiveContent;
		m_interactiveContentTTL = rhs.m_interactiveContentTTL;
		m_winnerMessage = rhs.m_winnerMessage;
		m_winnerMessageTTL = rhs.m_winnerMessageTTL;
		m_smPromo = rhs.m_smPromo;
		m_smPromoTTL = rhs.m_smPromoTTL;
		m_congrats = rhs.m_congrats;
		m_congratsTTL = rhs.m_congratsTTL;
		m_winnerAnticipation = rhs.m_winnerAnticipation;
		m_winnerAnticipationTTL = rhs.m_winnerAnticipationTTL;
		m_announcements = rhs.m_announcements;
		m_announcementsTTL = rhs.m_announcementsTTL;
		m_birthday = rhs.m_birthday;
		m_birthdayTTL = rhs.m_birthdayTTL;
		m_anniversary = rhs.m_anniversary;
		m_anniversaryTTL = rhs.m_anniversaryTTL;
		m_massMarketing = rhs.m_massMarketing;
		m_massMarketingTTL = rhs.m_massMarketingTTL;
		m_CDM = rhs.m_CDM;
		m_CDMTTL = rhs.m_CDMTTL;
		m_unattendedJackpot = rhs.m_unattendedJackpot;
		m_unattendedJackpotTTL = rhs.m_unattendedJackpotTTL;
		m_unattendedJackpotMedia = rhs.m_unattendedJackpotMedia;
		m_unattendedJackpotMediaTTL = rhs.m_unattendedJackpotMediaTTL;
		m_invitedGroupsAnticipation = rhs.m_invitedGroupsAnticipation;
		m_invitedGroupsAnticipationTTL = rhs.m_invitedGroupsAnticipationTTL;
		m_invitedGroupsEndingSoon = rhs.m_invitedGroupsEndingSoon;
		m_invitedGroupsEndingSoonTTL = rhs.m_invitedGroupsEndingSoonTTL;
		m_invitedGroupsFinalResults = rhs.m_invitedGroupsFinalResults;
		m_invitedGroupsFinalResultsTTL = rhs.m_invitedGroupsFinalResultsTTL;
		m_invitedGroupsLeaderBoard = rhs.m_invitedGroupsLeaderBoard;
		m_invitedGroupsLeaderBoardTTL = rhs.m_invitedGroupsLeaderBoardTTL;
		m_invitedGroupsLevelUnlocked = rhs.m_invitedGroupsLevelUnlocked;
		m_invitedGroupsLevelUnlockedTTL = rhs.m_invitedGroupsLevelUnlockedTTL;
		m_progressivePrize = rhs.m_progressivePrize;
		m_progressivePrizeTTL = rhs.m_progressivePrizeTTL;
		m_promoGame = rhs.m_promoGame;
		m_promoGameTTL = rhs.m_promoGameTTL;
	}

	return *this;
}

void CQueuingConfig::SetEqualPriorityInterrupt(bool value)
{
	m_equalPriorityInterrupt = value;
}

bool CQueuingConfig::GetEqualPriorityInterrupt()
{
	return m_equalPriorityInterrupt;
}

DWORD CQueuingConfig::GetInitialInteractiveStarveTime()
{
	return m_initialInteractiveStarveTime;
}

void CQueuingConfig::SetInitialInteractiveStarveTime(DWORD value)
{
	m_initialInteractiveStarveTime = value;
}

DWORD CQueuingConfig::GetInteractiveContentStarveTime()
{
	return m_interactiveContentStarveTime;
}

void CQueuingConfig::SetInteractiveContentStarveTime(DWORD value)
{
	m_interactiveContentStarveTime = value;
}

DWORD CQueuingConfig::GetUjpTimeout()
{
	return m_ujpTimeout;
}

void CQueuingConfig::SetUjpTimeout(DWORD value)
{
	m_ujpTimeout = value;
}

DWORD CQueuingConfig::GetUserScreenTime()
{
	return m_userScreenTime;
}

void CQueuingConfig::SetUserScreenTime(DWORD value)
{
	m_userScreenTime = value;
}

int CQueuingConfig::GetPbtScreens()
{
	return m_pbtScreens;
}

void CQueuingConfig::SetPbtScreens(int value)
{
	m_pbtScreens = value;
}

int CQueuingConfig::GetPlayerInitiated()
{
	return m_playerInitiated;
}

void CQueuingConfig::SetPlayerInitiated(int value)
{
	m_playerInitiated = value;
}

DWORD CQueuingConfig::GetPlayerInitiatedTTL()
{
	return m_playerInitiatedTTL;
}

void CQueuingConfig::SetPlayerInitiatedTTL(DWORD value)
{
	m_playerInitiatedTTL = value;
}

int CQueuingConfig::GetInteractiveContent()
{
	return m_interactiveContent;
}

void CQueuingConfig::SetInteractiveContent(int value)
{
	m_interactiveContent = value;
}

DWORD CQueuingConfig::GetInteractiveContentTTL()
{
	return m_interactiveContentTTL;
}

void CQueuingConfig::SetInteractiveContentTTL(DWORD value)
{
	m_interactiveContentTTL = value;
}

int CQueuingConfig::GetWinnerMessage()
{
	return m_winnerMessage;
}

void CQueuingConfig::SetWinnerMessage(int value)
{
	m_winnerMessage = value;
}

DWORD CQueuingConfig::GetWinnerMessageTTL()
{
	return m_winnerMessageTTL;
}

void CQueuingConfig::SetWinnerMessageTTL(DWORD value)
{
	m_winnerMessageTTL = value;
}

int CQueuingConfig::GetSmPromo()
{
	return m_smPromo;
}

void CQueuingConfig::SetSmPromo(int value)
{
	m_smPromo = value;
}

DWORD CQueuingConfig::GetSmPromoTTL()
{
	return m_smPromoTTL;
}

void CQueuingConfig::SetSmPromoTTL(DWORD value)
{
	m_smPromoTTL = value;
}

int CQueuingConfig::GetCongrats()
{
	return m_congrats;
}

void CQueuingConfig::SetCongrats(int value)
{
	m_congrats = value;
}

DWORD CQueuingConfig::GetCongratsTTL()
{
	return m_congratsTTL;
}

void CQueuingConfig::SetCongratsTTL(DWORD value)
{
	m_congratsTTL = value;
}

int CQueuingConfig::GetScheduleEnd()
{
	return m_scheduleEnd;
}

void CQueuingConfig::SetScheduleEnd(int value)
{
	m_scheduleEnd = value;
}

DWORD CQueuingConfig::GetScheduleEndTTL()
{
	return m_scheduleEndTTL;
}

void CQueuingConfig::SetScheduleEndTTL(DWORD value)
{
	m_scheduleEndTTL = value;
}

int CQueuingConfig::GetWinnerAnticipation()
{
	return m_winnerAnticipation;
}

void CQueuingConfig::SetWinnerAnticipation(int value)
{
	m_winnerAnticipation = value;
}

DWORD CQueuingConfig::GetWinnerAnticipationTTL()
{
	return m_winnerAnticipationTTL;
}

void CQueuingConfig::SetWinnerAnticipationTTL(DWORD value)
{
	m_winnerAnticipationTTL = value;
}

int CQueuingConfig::GetAnnouncements()
{
	return m_announcements;
}

void CQueuingConfig::SetAnnouncements(int value)
{
	m_announcements = value;
}

DWORD CQueuingConfig::GetAnnouncementsTTL()
{
	return m_announcementsTTL;
}

void CQueuingConfig::SetAnnouncementsTTL(DWORD value)
{
	m_announcementsTTL = value;
}

int CQueuingConfig::GetBirthday()
{
	return m_birthday;
}

void CQueuingConfig::SetBirthday(int value)
{
	m_birthday = value;
}

DWORD CQueuingConfig::GetBirthdayTTL()
{
	return m_birthdayTTL;
}

void CQueuingConfig::SetBirthdayTTL(DWORD value)
{
	m_birthdayTTL = value;
}

int CQueuingConfig::GetAnniversary()
{
	return m_anniversary;
}

void CQueuingConfig::SetAnniversary(int value)
{
	m_anniversary = value;
}

DWORD CQueuingConfig::GetAnniversaryTTL()
{
	return m_anniversaryTTL;
}

void CQueuingConfig::SetAnniversaryTTL(DWORD value)
{
	m_anniversaryTTL = value;
}

int CQueuingConfig::GetMassMarketing() const
{
	return m_massMarketing;
}

void CQueuingConfig::SetMassMarketing(int value)
{
	m_massMarketing = value;
}

DWORD CQueuingConfig::GetMassMarketingTTL() const
{
	return m_massMarketingTTL;
}

void CQueuingConfig::SetMassMarketingTTL(DWORD value)
{
	m_massMarketingTTL = value;
}

int CQueuingConfig::GetCDM() const
{
	return m_CDM;
}

void CQueuingConfig::SetCDM(int value)
{
	m_CDM = value;
}

DWORD CQueuingConfig::GetCDMTTL() const
{
	return m_CDMTTL;
}

void CQueuingConfig::SetCDMTTL(DWORD value)
{
	m_CDMTTL = value;
}

int CQueuingConfig::GetUnattendedJackpot() const
{
	return m_unattendedJackpot;
}

void CQueuingConfig::SetUnattendedJackpot(int value)
{
	m_unattendedJackpot = value;
}

DWORD CQueuingConfig::GetUnattendedJackpotTTL() const
{
	return m_unattendedJackpotTTL;
}

void CQueuingConfig::SetUnattendedJackpotTTL(DWORD value)
{
	m_unattendedJackpotTTL = value;
}

int CQueuingConfig::GetUnattendedJackpotMedia() const
{
	return m_unattendedJackpotMedia;
}

void CQueuingConfig::SetUnattendedJackpotMedia(int value)
{
	m_unattendedJackpotMedia = value;
}

DWORD CQueuingConfig::GetUnattendedJackpotMediaTTL() const
{
	return m_unattendedJackpotMediaTTL;
}

void CQueuingConfig::SetUnattendedJackpotMediaTTL(DWORD value)
{
	m_unattendedJackpotMediaTTL = value;
}

void CQueuingConfig::SetHasConfigFile(bool hasConfigFile)
{
	m_hasConfigFile = hasConfigFile;
}

bool CQueuingConfig::HasConfigFile() const
{
	return m_hasConfigFile;
}

void  CQueuingConfig::SetInvitedGroupsAnticipation(int value)
{
	m_invitedGroupsAnticipation = value;
}

int  CQueuingConfig::GetInvitedGroupsAnticipation() const
{
	return m_invitedGroupsAnticipation;
}

void  CQueuingConfig::SetInvitedGroupsAnticipationTTL(DWORD value)
{
	m_invitedGroupsAnticipationTTL = value;
}

DWORD  CQueuingConfig::GetInvitedGroupsAnticipationTTL() const
{
	return m_invitedGroupsAnticipationTTL;
}

void  CQueuingConfig::SetInvitedGroupsLevelUnlocked(int value)
{
	m_invitedGroupsLevelUnlocked = value;
}

int  CQueuingConfig::GetInvitedGroupsLevelUnlocked() const
{
	return m_invitedGroupsLevelUnlocked;
}

void  CQueuingConfig::SetInvitedGroupsLevelUnlockedTTL(DWORD value)
{
	m_invitedGroupsLevelUnlockedTTL = value;
}

DWORD  CQueuingConfig::GetInvitedGroupsLevelUnlockedTTL() const
{
	return m_invitedGroupsLevelUnlockedTTL;
}

void  CQueuingConfig::SetInvitedGroupsLeaderBoard(int value)
{
	m_invitedGroupsLeaderBoard = value;
}

int  CQueuingConfig::GetInvitedGroupsLeaderBoard() const
{
	return m_invitedGroupsLeaderBoard;
}

void  CQueuingConfig::SetInvitedGroupsLeaderBoardTTL(DWORD value)
{
	m_invitedGroupsLeaderBoardTTL = value;
}

DWORD  CQueuingConfig::GetInvitedGroupsLeaderBoardTTL() const
{
	return m_invitedGroupsLeaderBoardTTL;
}

void  CQueuingConfig::SetInvitedGroupsFinalResults(int value)
{
	m_invitedGroupsFinalResults = value;
}

int  CQueuingConfig::GetInvitedGroupsFinalResults() const
{
	return m_invitedGroupsFinalResults;
}

void  CQueuingConfig::SetInvitedGroupsFinalResultsTTL(DWORD value)
{
	m_invitedGroupsFinalResultsTTL = value;
}

DWORD  CQueuingConfig::GetInvitedGroupsFinalResultsTTL() const
{
	return m_invitedGroupsFinalResultsTTL;
}

void  CQueuingConfig::SetInvitedGroupsEndingSoon(int value)
{
	m_invitedGroupsEndingSoon = value;
}

int  CQueuingConfig::GetInvitedGroupsEndingSoon() const
{
	return m_invitedGroupsEndingSoon;
}

void  CQueuingConfig::SetInvitedGroupsEndingSoonTTL(DWORD value)
{
	m_invitedGroupsEndingSoonTTL = value;
}

DWORD  CQueuingConfig::GetInvitedGroupsEndingSoonTTL() const
{
	return m_invitedGroupsEndingSoonTTL;
}

void  CQueuingConfig::SetProgressivePrize(int value)
{
	m_progressivePrize = value;
}

int  CQueuingConfig::GetProgressivePrize() const
{
	return m_progressivePrize;
}

void  CQueuingConfig::SetProgressivePrizeTTL(DWORD value)
{
	m_progressivePrizeTTL = value;
}

DWORD  CQueuingConfig::GetProgressivePrizeTTL() const
{
	return m_progressivePrizeTTL;
}

void  CQueuingConfig::SetPromoGame(int value)
{
	m_promoGame = value;
}

int  CQueuingConfig::GetPromoGame() const
{
	return m_promoGame;
}

void  CQueuingConfig::SetPromoGameTTL(DWORD value)
{
	m_promoGameTTL = value;
}

DWORD  CQueuingConfig::GetPromoGameTTL() const
{
	return m_promoGameTTL;
}