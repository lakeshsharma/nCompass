#include "stdafx.h"
#include "QueuingConfigFile.h"
#include "Logging/LogString.h"

const LPCTSTR CQueuingConfigFile::CONFIG_FILENAME("/QueuingConfig.txt");
const LPCTSTR CQueuingConfigFile::HAS_CONFIG_FILE("HasConfigFile");

CQueuingConfigFile::CQueuingConfigFile(IOperatingSystem &operatingSystem) :
CConfigFile(0, operatingSystem)
{
	SetDefaults();
}

CQueuingConfigFile::~CQueuingConfigFile()
{
}

bool CQueuingConfigFile::Start()
{
	bool success(LoadConfigFile(CONFIG_FILENAME));
	return success;
}

void CQueuingConfigFile::RefreshConfig(bool refresh)
{
	UNREFERENCED_PARAMETER(refresh);
	Start();
}

void CQueuingConfigFile::SetDefaults()
{
	m_queuingConfig.SetDefaults();
}

bool CQueuingConfigFile::InterpretNameValue(const std::string& name, const std::string& value)
{
	bool retVal(false);

	if (CQueuingConfig::EQUAL_PRIORITY_INTERRUPT == name)
	{
		bool boolValue;
		retVal = InterpretBool(value, boolValue);
		if (retVal)
		{
			m_queuingConfig.SetEqualPriorityInterrupt(boolValue);
		}
	}
	else
	{
		DWORD dwordValue;
		retVal = InterpretDWORD(value, dwordValue);
		if (retVal)
		{
			if (CQueuingConfig::PRIORITY_INITIALINTERACTIVESTARVETIME == name)
			{
				m_queuingConfig.SetInitialInteractiveStarveTime(dwordValue);
			}
			else if (CQueuingConfig::PRIORITY_INTERACTIVECONTENTTOTALSTARVETIME == name)
			{
				m_queuingConfig.SetInteractiveContentStarveTime(dwordValue);
			}
			else if (CQueuingConfig::PRIORITY_USERSCREENTIME == name)
			{
				m_queuingConfig.SetUserScreenTime(dwordValue);
			}
			else if (CQueuingConfig::PRIORITY_UJPTIMEOUT == name)
			{
				m_queuingConfig.SetUjpTimeout(dwordValue);
			}				
			else if (CQueuingConfig::PRIORITY_PBTSCREENS == name)
			{
				m_queuingConfig.SetPbtScreens(dwordValue);
			}
			else if (CQueuingConfig::PRIORITY_PLAYERINITIATED == name)
			{
				m_queuingConfig.SetPlayerInitiated(dwordValue);
			}
			else if (CQueuingConfig::PRIORITY_PLAYERINITIATEDTTL == name)
			{
				m_queuingConfig.SetPlayerInitiatedTTL(dwordValue);
			}
			else if (CQueuingConfig::PRIORITY_INTERACTIVECONTENT == name)
			{
				m_queuingConfig.SetInteractiveContent(dwordValue);
			}
			else if (CQueuingConfig::PRIORITY_INTERACTIVECONTENTTTL == name)
			{
				m_queuingConfig.SetInteractiveContentTTL(dwordValue);
			}
			else if (CQueuingConfig::PRIORITY_WINNERMESSAGE == name)
			{
				m_queuingConfig.SetWinnerMessage(dwordValue);
			}
			else if (CQueuingConfig::PRIORITY_WINNERTTL == name)
			{
				m_queuingConfig.SetWinnerMessageTTL(dwordValue);
			}
			else if (CQueuingConfig::PRIORITY_SMPROMO == name)
			{
				m_queuingConfig.SetSmPromo(dwordValue);
			}
			else if (CQueuingConfig::PRIORITY_SMPROMOTTL == name)
			{
				m_queuingConfig.SetSmPromoTTL(dwordValue);
			}
			else if (CQueuingConfig::PRIORITY_CONGRATS == name)
			{
				m_queuingConfig.SetCongrats(dwordValue);
			}
			else if (CQueuingConfig::PRIORITY_CONGRATSTTL == name)
			{
				m_queuingConfig.SetCongratsTTL(dwordValue);
			}
			else if (CQueuingConfig::PRIORITY_SCHEDULEEND == name)
			{
				m_queuingConfig.SetScheduleEnd(dwordValue);
			}
			else if (CQueuingConfig::PRIORITY_SCHEDULEENDTTL == name)
			{
				m_queuingConfig.SetScheduleEndTTL(dwordValue);
			}
			else if (CQueuingConfig::PRIORITY_WINNERANTICIPATION == name)
			{
				m_queuingConfig.SetWinnerAnticipation(dwordValue);
			}
			else if (CQueuingConfig::PRIORITY_WINNERANTICIPATIONTTL == name)
			{
				m_queuingConfig.SetWinnerAnticipationTTL(dwordValue);
			}
			else if (CQueuingConfig::PRIORITY_ANNOUNCEMENTS == name)
			{
				m_queuingConfig.SetAnnouncements(dwordValue);
			}
			else if (CQueuingConfig::PRIORITY_ANNOUNCEMENTSTTL == name)
			{
				m_queuingConfig.SetAnnouncementsTTL(dwordValue);
			}
			else if (CQueuingConfig::PRIORITY_BIRTHDAY == name)
			{
				m_queuingConfig.SetBirthday(dwordValue);
			}
			else if (CQueuingConfig::PRIORITY_BIRTHDAYTTL == name)
			{
				m_queuingConfig.SetBirthdayTTL(dwordValue);
			}
			else if (CQueuingConfig::PRIORITY_ANNIVERSARY == name)
			{
				m_queuingConfig.SetAnniversary(dwordValue);
			}
			else if (CQueuingConfig::PRIORITY_ANNIVERSARYTTL == name)
			{
				m_queuingConfig.SetAnniversaryTTL(dwordValue);
			}
			else if (CQueuingConfig::PRIORITY_MASSMARKETING == name)
			{
				m_queuingConfig.SetMassMarketing(dwordValue);
			}
			else if (CQueuingConfig::PRIORITY_MASSMARKETINGTTL == name)
			{
				m_queuingConfig.SetMassMarketingTTL(dwordValue);
			}
			else if (CQueuingConfig::PRIORITY_CDM == name)
			{
				m_queuingConfig.SetCDM(dwordValue);
			}
			else if (CQueuingConfig::PRIORITY_CDMTTL == name)
			{
				m_queuingConfig.SetCDMTTL(dwordValue);
			}
			else if (CQueuingConfig::PRIORITY_UNATTENDED_JACKPOT == name)
			{
				m_queuingConfig.SetUnattendedJackpot(dwordValue);
			}
			else if (CQueuingConfig::PRIORITY_UNATTENDED_JACKPOT_TTL == name)
			{
				m_queuingConfig.SetUnattendedJackpotTTL(dwordValue);
			}
			else if (CQueuingConfig::PRIORITY_UNATTENDED_JACKPOT_MEDIA == name)
			{
				m_queuingConfig.SetUnattendedJackpotMedia(dwordValue);
			}
			else if (CQueuingConfig::PRIORITY_UNATTENDED_JACKPOT_MEDIA_TTL == name)
			{
				m_queuingConfig.SetUnattendedJackpotMediaTTL(dwordValue);
			}
			else if (CQueuingConfig::PRIORITY_INVITED_GROUPS_ANTICIPATION == name)
			{
				m_queuingConfig.SetInvitedGroupsAnticipation(dwordValue);
			}
			else if (CQueuingConfig::PRIORITY_INVITED_GROUPS_ENDINGSOON == name)
			{
				m_queuingConfig.SetInvitedGroupsEndingSoon(dwordValue);
			}
			else if (CQueuingConfig::PRIORITY_INVITED_GROUPS_FINALRESULTS == name)
			{
				m_queuingConfig.SetInvitedGroupsFinalResults(dwordValue);
			}
			else if (CQueuingConfig::PRIORITY_INVITED_GROUPS_LEADERBOARD == name)
			{
				m_queuingConfig.SetInvitedGroupsLeaderBoard(dwordValue);
			}
			else if (CQueuingConfig::PRIORITY_INVITED_GROUPS_LEVELUNLOCKED == name)
			{
				m_queuingConfig.SetInvitedGroupsLevelUnlocked(dwordValue);
			}
			else if (CQueuingConfig::PRIORITY_INVITED_GROUPS_ANTICIPATIONTTL == name)
			{
				m_queuingConfig.SetInvitedGroupsAnticipationTTL(dwordValue);
			}
			else if (CQueuingConfig::PRIORITY_INVITED_GROUPS_ENDINGSOONTTL == name)
			{
				m_queuingConfig.SetInvitedGroupsEndingSoonTTL(dwordValue);
			}
			else if (CQueuingConfig::PRIORITY_INVITED_GROUPS_FINALRESULTSTTL == name)
			{
				m_queuingConfig.SetInvitedGroupsFinalResultsTTL(dwordValue);
			}
			else if (CQueuingConfig::PRIORITY_INVITED_GROUPS_LEADERBOARDTTL == name)
			{
				m_queuingConfig.SetInvitedGroupsLeaderBoardTTL(dwordValue);
			}
			else if (CQueuingConfig::PRIORITY_INVITED_GROUPS_LEVELUNLOCKEDTTL == name)
			{
				m_queuingConfig.SetInvitedGroupsLevelUnlockedTTL(dwordValue);
			}
			else if (CQueuingConfig::PRIORITY_PROGRESSIVEPRIZE == name)
			{
				m_queuingConfig.SetProgressivePrize(dwordValue);
			}
			else if (CQueuingConfig::PRIORITY_PROGRESSIVEPRIZETTL == name)
			{
				m_queuingConfig.SetProgressivePrizeTTL(dwordValue);
			}
			else if (CQueuingConfig::PRIORITY_PROMOGAMETTL == name)
			{
				m_queuingConfig.SetPromoGameTTL(dwordValue);
			}
			else if (CQueuingConfig::PRIORITY_PROMOGAME == name)
			{
				m_queuingConfig.SetPromoGame(dwordValue);
			}
		}
	}

	return retVal;
}

void CQueuingConfigFile::LogConfigValues(LPCTSTR sourceDesc, bool valid)
{
	m_queuingConfig.SetHasConfigFile(valid);

	LogString("GENERAL", "%s: %s",
		static_cast<LPCTSTR>(sourceDesc),
		static_cast<LPCTSTR>(valid ? "valid" : "invalid"));

	LogString("GENERAL", "%s: %s=\"%s\"",
		static_cast<LPCTSTR>(sourceDesc),
		static_cast<LPCTSTR>(HAS_CONFIG_FILE),
		static_cast<LPCTSTR>(m_queuingConfig.HasConfigFile() ? TRUE_STRING : FALSE_STRING));

	LogString("GENERAL", "%s: %s=\"%u\"",
		static_cast<LPCTSTR>(sourceDesc),
		static_cast<LPCTSTR>(CQueuingConfig::EQUAL_PRIORITY_INTERRUPT),
		m_queuingConfig.GetEqualPriorityInterrupt());

	LogString("GENERAL", "%s: %s=\"%u\"",
		static_cast<LPCTSTR>(sourceDesc),
		static_cast<LPCTSTR>(CQueuingConfig::PRIORITY_INITIALINTERACTIVESTARVETIME),
		m_queuingConfig.GetInitialInteractiveStarveTime());

	LogString("GENERAL", "%s: %s=\"%u\"",
		static_cast<LPCTSTR>(sourceDesc),
		static_cast<LPCTSTR>(CQueuingConfig::PRIORITY_INTERACTIVECONTENTTOTALSTARVETIME),
		m_queuingConfig.GetInitialInteractiveStarveTime());

	LogString("GENERAL", "%s: %s=\"%u\"",
		static_cast<LPCTSTR>(sourceDesc),
		static_cast<LPCTSTR>(CQueuingConfig::PRIORITY_USERSCREENTIME),
		m_queuingConfig.GetUserScreenTime());

	LogString("GENERAL", "%s: %s=\"%u\"",
		static_cast<LPCTSTR>(sourceDesc),
		static_cast<LPCTSTR>(CQueuingConfig::PRIORITY_UJPTIMEOUT),
		m_queuingConfig.GetUjpTimeout());

	LogString("GENERAL", "%s: %s=\"%u\"",
		static_cast<LPCTSTR>(sourceDesc),
		static_cast<LPCTSTR>(CQueuingConfig::PRIORITY_PBTSCREENS),
		m_queuingConfig.GetPbtScreens());

	LogString("GENERAL", "%s: %s=\"%u\"",
		static_cast<LPCTSTR>(sourceDesc),
		static_cast<LPCTSTR>(CQueuingConfig::PRIORITY_PLAYERINITIATED),
		m_queuingConfig.GetPlayerInitiated());

	LogString("GENERAL", "%s: %s=\"%u\"",
		static_cast<LPCTSTR>(sourceDesc),
		static_cast<LPCTSTR>(CQueuingConfig::PRIORITY_PLAYERINITIATEDTTL),
		m_queuingConfig.GetPlayerInitiatedTTL());

	LogString("GENERAL", "%s: %s=\"%u\"",
		static_cast<LPCTSTR>(sourceDesc),
		static_cast<LPCTSTR>(CQueuingConfig::PRIORITY_INTERACTIVECONTENT),
		m_queuingConfig.GetInteractiveContent());

	LogString("GENERAL", "%s: %s=\"%u\"",
		static_cast<LPCTSTR>(sourceDesc),
		static_cast<LPCTSTR>(CQueuingConfig::PRIORITY_INTERACTIVECONTENTTTL),
		m_queuingConfig.GetInteractiveContentTTL());

	LogString("GENERAL", "%s: %s=\"%u\"",
		static_cast<LPCTSTR>(sourceDesc),
		static_cast<LPCTSTR>(CQueuingConfig::PRIORITY_WINNERMESSAGE),
		m_queuingConfig.GetWinnerMessage());

	LogString(_T("GENERAL"), _T("%s: %s=\"%u\""),
		static_cast<LPCTSTR>(sourceDesc),
		static_cast<LPCTSTR>(CQueuingConfig::PRIORITY_WINNERTTL),
		m_queuingConfig.GetWinnerMessageTTL());

	LogString("GENERAL", "%s: %s=\"%u\"",
		static_cast<LPCTSTR>(sourceDesc),
		static_cast<LPCTSTR>(CQueuingConfig::PRIORITY_SMPROMO),
		m_queuingConfig.GetSmPromo());

	LogString("GENERAL", "%s: %s=\"%u\"",
		static_cast<LPCTSTR>(sourceDesc),
		static_cast<LPCTSTR>(CQueuingConfig::PRIORITY_SMPROMOTTL),
		m_queuingConfig.GetSmPromoTTL());

	LogString("GENERAL", "%s: %s=\"%u\"",
		static_cast<LPCTSTR>(sourceDesc),
		static_cast<LPCTSTR>(CQueuingConfig::PRIORITY_CONGRATS),
		m_queuingConfig.GetCongrats());

	LogString("GENERAL", "%s: %s=\"%u\"",
		static_cast<LPCTSTR>(sourceDesc),
		static_cast<LPCTSTR>(CQueuingConfig::PRIORITY_CONGRATSTTL),
		m_queuingConfig.GetCongratsTTL());

	LogString("GENERAL", "%s: %s=\"%u\"",
		static_cast<LPCTSTR>(sourceDesc),
		static_cast<LPCTSTR>(CQueuingConfig::PRIORITY_WINNERANTICIPATION),
		m_queuingConfig.GetWinnerAnticipation());

	LogString(_T("GENERAL"), _T("%s: %s=\"%u\""),
		static_cast<LPCTSTR>(sourceDesc),
		static_cast<LPCTSTR>(CQueuingConfig::PRIORITY_WINNERANTICIPATIONTTL),
		m_queuingConfig.GetWinnerAnticipationTTL());

	LogString("GENERAL", "%s: %s=\"%u\"",
		static_cast<LPCTSTR>(sourceDesc),
		static_cast<LPCTSTR>(CQueuingConfig::PRIORITY_ANNOUNCEMENTS),
		m_queuingConfig.GetAnnouncements());

	LogString("GENERAL", "%s: %s=\"%u\"",
		static_cast<LPCTSTR>(sourceDesc),
		static_cast<LPCTSTR>(CQueuingConfig::PRIORITY_ANNOUNCEMENTSTTL),
		m_queuingConfig.GetAnnouncementsTTL());

	LogString("GENERAL", "%s: %s=\"%u\"",
		static_cast<LPCTSTR>(sourceDesc),
		static_cast<LPCTSTR>(CQueuingConfig::PRIORITY_BIRTHDAY),
		m_queuingConfig.GetBirthday());

	LogString("GENERAL", "%s: %s=\"%u\"",
		static_cast<LPCTSTR>(sourceDesc),
		static_cast<LPCTSTR>(CQueuingConfig::PRIORITY_BIRTHDAYTTL),
		m_queuingConfig.GetBirthdayTTL());

	LogString("GENERAL", "%s: %s=\"%u\"",
		static_cast<LPCTSTR>(sourceDesc),
		static_cast<LPCTSTR>(CQueuingConfig::PRIORITY_ANNIVERSARY),
		m_queuingConfig.GetAnniversary());

	LogString("GENERAL", "%s: %s=\"%u\"",
		static_cast<LPCTSTR>(sourceDesc),
		static_cast<LPCTSTR>(CQueuingConfig::PRIORITY_ANNIVERSARYTTL),
		m_queuingConfig.GetAnniversaryTTL());

	LogString("GENERAL", "%s: %s=\"%u\"",
		static_cast<LPCTSTR>(sourceDesc),
		static_cast<LPCTSTR>(CQueuingConfig::PRIORITY_MASSMARKETING),
		m_queuingConfig.GetMassMarketing());

	LogString("GENERAL", "%s: %s=\"%u\"",
		static_cast<LPCTSTR>(sourceDesc),
		static_cast<LPCTSTR>(CQueuingConfig::PRIORITY_MASSMARKETINGTTL),
		m_queuingConfig.GetMassMarketingTTL());

	LogString("GENERAL", "%s: %s=\"%u\"",
		static_cast<LPCTSTR>(sourceDesc),
		static_cast<LPCTSTR>(CQueuingConfig::PRIORITY_CDM),
		m_queuingConfig.GetCDM());

	LogString("GENERAL", "%s: %s=\"%u\"",
		static_cast<LPCTSTR>(sourceDesc),
		static_cast<LPCTSTR>(CQueuingConfig::PRIORITY_CDMTTL),
		m_queuingConfig.GetCDMTTL());

	LogString("GENERAL", "%s: %s=\"%u\"",
		static_cast<LPCTSTR>(sourceDesc),
		static_cast<LPCTSTR>(CQueuingConfig::PRIORITY_UNATTENDED_JACKPOT),
		m_queuingConfig.GetUnattendedJackpot());

	LogString("GENERAL", "%s: %s=\"%u\"",
		static_cast<LPCTSTR>(sourceDesc),
		static_cast<LPCTSTR>(CQueuingConfig::PRIORITY_UNATTENDED_JACKPOT_TTL),
		m_queuingConfig.GetUnattendedJackpotTTL());

	LogString("GENERAL", "%s: %s=\"%u\"",
		static_cast<LPCTSTR>(sourceDesc),
		static_cast<LPCTSTR>(CQueuingConfig::PRIORITY_UNATTENDED_JACKPOT_MEDIA),
		m_queuingConfig.GetUnattendedJackpotMedia());

	LogString("GENERAL", "%s: %s=\"%u\"",
		static_cast<LPCTSTR>(sourceDesc),
		static_cast<LPCTSTR>(CQueuingConfig::PRIORITY_UNATTENDED_JACKPOT_MEDIA_TTL),
		m_queuingConfig.GetUnattendedJackpotMediaTTL());

	LogString("GENERAL", "%s: %s=\"%u\"",
		static_cast<LPCTSTR>(sourceDesc),
		static_cast<LPCTSTR>(CQueuingConfig::PRIORITY_INVITED_GROUPS_ANTICIPATION),
		m_queuingConfig.GetInvitedGroupsAnticipation());

	LogString("GENERAL", "%s: %s=\"%u\"",
		static_cast<LPCTSTR>(sourceDesc),
		static_cast<LPCTSTR>(CQueuingConfig::PRIORITY_INVITED_GROUPS_ANTICIPATIONTTL),
		m_queuingConfig.GetInvitedGroupsAnticipationTTL());

	LogString("GENERAL", "%s: %s=\"%u\"",
		static_cast<LPCTSTR>(sourceDesc),
		static_cast<LPCTSTR>(CQueuingConfig::PRIORITY_INVITED_GROUPS_ENDINGSOON),
		m_queuingConfig.GetInvitedGroupsEndingSoon());

	LogString("GENERAL", "%s: %s=\"%u\"",
		static_cast<LPCTSTR>(sourceDesc),
		static_cast<LPCTSTR>(CQueuingConfig::PRIORITY_INVITED_GROUPS_ENDINGSOONTTL),
		m_queuingConfig.GetInvitedGroupsEndingSoonTTL());

	LogString("GENERAL", "%s: %s=\"%u\"",
		static_cast<LPCTSTR>(sourceDesc),
		static_cast<LPCTSTR>(CQueuingConfig::PRIORITY_INVITED_GROUPS_FINALRESULTS),
		m_queuingConfig.GetInvitedGroupsFinalResults());

	LogString("GENERAL", "%s: %s=\"%u\"",
		static_cast<LPCTSTR>(sourceDesc),
		static_cast<LPCTSTR>(CQueuingConfig::PRIORITY_INVITED_GROUPS_FINALRESULTSTTL),
		m_queuingConfig.GetInvitedGroupsFinalResultsTTL());

	LogString("GENERAL", "%s: %s=\"%u\"",
		static_cast<LPCTSTR>(sourceDesc),
		static_cast<LPCTSTR>(CQueuingConfig::PRIORITY_INVITED_GROUPS_LEADERBOARD),
		m_queuingConfig.GetInvitedGroupsLeaderBoard());

	LogString("GENERAL", "%s: %s=\"%u\"",
		static_cast<LPCTSTR>(sourceDesc),
		static_cast<LPCTSTR>(CQueuingConfig::PRIORITY_INVITED_GROUPS_LEADERBOARDTTL),
		m_queuingConfig.GetInvitedGroupsLeaderBoardTTL());

	LogString("GENERAL", "%s: %s=\"%u\"",
		static_cast<LPCTSTR>(sourceDesc),
		static_cast<LPCTSTR>(CQueuingConfig::PRIORITY_INVITED_GROUPS_LEVELUNLOCKED),
		m_queuingConfig.GetInvitedGroupsLevelUnlocked());

	LogString("GENERAL", "%s: %s=\"%u\"",
		static_cast<LPCTSTR>(sourceDesc),
		static_cast<LPCTSTR>(CQueuingConfig::PRIORITY_INVITED_GROUPS_LEVELUNLOCKEDTTL),
		m_queuingConfig.GetInvitedGroupsLevelUnlockedTTL());

	LogString("GENERAL", "%s: %s=\"%u\"",
		static_cast<LPCTSTR>(sourceDesc),
		static_cast<LPCTSTR>(CQueuingConfig::PRIORITY_PROGRESSIVEPRIZE),
		m_queuingConfig.GetProgressivePrize());

	LogString("GENERAL", "%s: %s=\"%u\"",
		static_cast<LPCTSTR>(sourceDesc),
		static_cast<LPCTSTR>(CQueuingConfig::PRIORITY_PROGRESSIVEPRIZETTL),
		m_queuingConfig.GetProgressivePrizeTTL());

	LogString("GENERAL", "%s: %s=\"%u\"",
		static_cast<LPCTSTR>(sourceDesc),
		static_cast<LPCTSTR>(CQueuingConfig::PRIORITY_PROMOGAME),
		m_queuingConfig.GetPromoGame());

	LogString("GENERAL", "%s: %s=\"%u\"",
		static_cast<LPCTSTR>(sourceDesc),
		static_cast<LPCTSTR>(CQueuingConfig::PRIORITY_PROMOGAMETTL),
		m_queuingConfig.GetPromoGameTTL());
}

CQueuingConfig* CQueuingConfigFile::GetQueuingConfigCopy() const
{
	CQueuingConfig* queuingConfig(new CQueuingConfig(m_queuingConfig));
	return queuingConfig;
}

