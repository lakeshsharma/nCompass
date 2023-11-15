#pragma once

class CQueuingConfig
{
public:
	CQueuingConfig();

	/// <summary>
	/// Copy constructor. Initializes a new instance of the <see cref="CQueuingConfig"/> class.
	/// </summary>
	/// <param name="rhs">The source instance.</param>
	CQueuingConfig(const CQueuingConfig &rhs);

	/// <summary>
	/// Assignment operator.
	/// </summary>
	/// <param name="rhs">IN - The source instance.</param>
	/// <returns>A reference to the target of the assignment.</returns>
	CQueuingConfig& operator=(const CQueuingConfig &rhs);

	/// <summary>
	/// Finalizes an instance of the <see cref="CQueuingConfig"/> class.
	/// </summary>
	~CQueuingConfig();

	/// <summary>
	/// Sets the defaults.
	/// </summary>
	void SetDefaults();

	/// <summary>
	/// Sets the equal priority interrupt.
	/// </summary>
	/// <param name="value">if set to <c>true</c> [value].</param>
	void SetEqualPriorityInterrupt(bool value);

	/// <summary>
	/// Gets the equal priority interrupt.
	/// </summary>
	/// <returns></returns>
	bool GetEqualPriorityInterrupt();

	/// <summary>
	/// Gets the initial interactive starve time.
	/// </summary>
	/// <returns></returns>
	DWORD GetInitialInteractiveStarveTime();

	/// <summary>
	/// Sets the initial interactive starve time.
	/// </summary>
	/// <param name="value">The value.</param>
	void SetInitialInteractiveStarveTime(DWORD value);

	/// <summary>
	/// Gets the interactive content starve time.
	/// </summary>
	/// <returns></returns>
	DWORD GetInteractiveContentStarveTime();

	/// <summary>
	/// Sets the interactive content starve time.
	/// </summary>
	/// <param name="value">The value.</param>
	void SetInteractiveContentStarveTime(DWORD value);

	/// <summary>
	/// Gets the UJP Timeout Value
	/// </summary>
	/// <returns>UJP Timeout Value</returns>
	DWORD GetUjpTimeout();

	/// <summary>
	/// Sets the UJP Timeout value.
	/// </summary>
	/// <param name="value">UJP timeout value.</param>
	void SetUjpTimeout(DWORD value);

	/// <summary>
	/// Gets the user screen time.
	/// </summary>
	/// <returns></returns>
	DWORD GetUserScreenTime();

	/// <summary>
	/// Sets the user screen time.
	/// </summary>
	/// <param name="value">The value.</param>
	void SetUserScreenTime(DWORD value);

	/// <summary>
	/// Gets the PBT screens.
	/// </summary>
	/// <returns></returns>
	int GetPbtScreens();

	/// <summary>
	/// Sets the PBT screens.
	/// </summary>
	/// <param name="value">The value.</param>
	void SetPbtScreens(int value);

	/// <summary>
	/// Gets the player initiated.
	/// </summary>
	/// <returns></returns>
	int GetPlayerInitiated();

	/// <summary>
	/// Sets the player initiated.
	/// </summary>
	/// <param name="value">The value.</param>
	void SetPlayerInitiated(int value);

	/// <summary>
	/// Gets the player initiated TTL.
	/// </summary>
	/// <returns></returns>
	DWORD GetPlayerInitiatedTTL();

	/// <summary>
	/// Sets the player initiated TTL.
	/// </summary>
	/// <param name="value">The value.</param>
	void SetPlayerInitiatedTTL(DWORD value);

	/// <summary>
	/// Gets the content of the interactive.
	/// </summary>
	/// <returns></returns>
	int GetInteractiveContent();

	/// <summary>
	/// Sets the content of the interactive.
	/// </summary>
	/// <param name="value">The value.</param>
	void SetInteractiveContent(int value);

	/// <summary>
	/// Gets the interactive content TTL.
	/// </summary>
	/// <returns></returns>
	DWORD GetInteractiveContentTTL();

	/// <summary>
	/// Sets the interactive content TTL.
	/// </summary>
	/// <param name="value">The value.</param>
	void SetInteractiveContentTTL(DWORD value);

	/// <summary>
	/// Gets the winner message.
	/// </summary>
	/// <returns></returns>
	int GetWinnerMessage();

	/// <summary>
	/// Sets the winner message.
	/// </summary>
	/// <param name="value">The value.</param>
	void SetWinnerMessage(int value);

	/// <summary>
	/// Gets the winner message TTL.
	/// </summary>
	/// <returns></returns>
	DWORD GetWinnerMessageTTL();

	/// <summary>
	/// Sets the winner message TTL.
	/// </summary>
	/// <param name="value">The value.</param>
	void SetWinnerMessageTTL(DWORD value);

	/// <summary>
	/// Gets the sm promo.
	/// </summary>
	/// <returns></returns>
	int GetSmPromo();

	/// <summary>
	/// Sets the sm promo.
	/// </summary>
	/// <param name="value">The value.</param>
	void SetSmPromo(int value);

	/// <summary>
	/// Gets the sm promo TTL.
	/// </summary>
	/// <returns></returns>
	DWORD GetSmPromoTTL();

	/// <summary>
	/// Sets the sm promo TTL.
	/// </summary>
	/// <param name="value">The value.</param>
	void SetSmPromoTTL(DWORD value);

	/// <summary>
	/// Gets the congrats.
	/// </summary>
	/// <returns></returns>
	int GetCongrats();

	/// <summary>
	/// Sets the congrats.
	/// </summary>
	/// <param name="value">The value.</param>
	void SetCongrats(int value);

	/// <summary>
	/// Gets the congrats TTL.
	/// </summary>
	/// <returns></returns>
	DWORD GetCongratsTTL();

	/// <summary>
	/// Sets the congrats TTL.
	/// </summary>
	/// <param name="value">The value.</param>
	void SetCongratsTTL(DWORD value);

	/// <summary>
	/// Gets the schedule end priority.
	/// </summary>
	/// <returns>schedule end priority</returns>
	int GetScheduleEnd();

	/// <summary>
	/// Sets the schedule end priority.
	/// </summary>
	/// <param name="value">schedule end priority.</param>
	void SetScheduleEnd(int value);

	/// <summary>
	/// Gets the schedule end TTL.
	/// </summary>
	/// <returns>schedule end TTL</returns>
	DWORD GetScheduleEndTTL();

	/// <summary>
	/// Sets the schedule end TTL.
	/// </summary>
	/// <param name="value">schedule end TTL.</param>
	void SetScheduleEndTTL(DWORD value);

	/// <summary>
	/// Gets the winner anticipation.
	/// </summary>
	/// <returns></returns>
	int GetWinnerAnticipation();

	/// <summary>
	/// Sets the winner anticipation.
	/// </summary>
	/// <param name="value">The value.</param>
	void SetWinnerAnticipation(int value);

	/// <summary>
	/// Gets the winner anticipation TTL.
	/// </summary>
	/// <returns></returns>
	DWORD GetWinnerAnticipationTTL();

	/// <summary>
	/// Sets the winner anticipation TTL.
	/// </summary>
	/// <param name="value">The value.</param>
	void SetWinnerAnticipationTTL(DWORD value);

	/// <summary>
	/// Gets the announcements.
	/// </summary>
	/// <returns></returns>
	int GetAnnouncements();

	/// <summary>
	/// Sets the announcements.
	/// </summary>
	/// <param name="value">The value.</param>
	void SetAnnouncements(int value);

	/// <summary>
	/// Gets the announcements TTL.
	/// </summary>
	/// <returns></returns>
	DWORD GetAnnouncementsTTL();

	/// <summary>
	/// Sets the announcements TTL.
	/// </summary>
	/// <param name="value">The value.</param>
	void SetAnnouncementsTTL(DWORD value);

	/// <summary>
	/// Gets the birthday.
	/// </summary>
	/// <returns></returns>
	int GetBirthday();

	/// <summary>
	/// Sets the birthday.
	/// </summary>
	/// <param name="value">The value.</param>
	void SetBirthday(int value);

	/// <summary>
	/// Gets the birthday TTL.
	/// </summary>
	/// <returns></returns>
	DWORD GetBirthdayTTL();

	/// <summary>
	/// Sets the birthday TTL.
	/// </summary>
	/// <param name="value">The value.</param>
	void SetBirthdayTTL(DWORD value);

	/// <summary>
	/// Gets the anniversary.
	/// </summary>
	/// <returns></returns>
	int GetAnniversary();

	/// <summary>
	/// Sets the anniversary.
	/// </summary>
	/// <param name="value">The value.</param>
	void SetAnniversary(int value);

	/// <summary>
	/// Gets the anniversary TTL.
	/// </summary>
	/// <returns>anniversary TTL</returns>
	DWORD GetAnniversaryTTL();

	/// <summary>
	/// Sets the anniversary TTL.
	/// </summary>
	/// <param name="value">The value.</param>
	void SetAnniversaryTTL(DWORD value);

	/// <summary>
	/// Gets the mass marketing.
	/// </summary>
	/// <returns>mass marketing priority</returns>
	int GetMassMarketing() const;

	/// <summary>
	/// Sets the mass marketing.
	/// </summary>
	/// <param name="value">mass marketing priority value.</param>
	void SetMassMarketing(int value);

	/// <summary>
	/// Gets the mass marketing TTL.
	/// </summary>
	/// <returns>mass marketing TTL</returns>
	DWORD GetMassMarketingTTL() const;

	/// <summary>
	/// Sets the mass marketing TTL.
	/// </summary>
	/// <param name="value">mass marketing TTL</param>
	void SetMassMarketingTTL(DWORD value);

	/// <summary>
	/// Gets the CDM priority.
	/// </summary>
	/// <returns>CDM priority</returns>
	int GetCDM() const;

	/// <summary>
	/// Sets the CDM priority.
	/// </summary>
	/// <param name="value">CDM priority.</param>
	void SetCDM(int value);

	/// <summary>
	/// Gets the CDMTTL.
	/// </summary>
	/// <returns>CDM TTL</returns>
	DWORD GetCDMTTL() const;

	/// <summary>
	/// Sets the CDM TTL.
	/// </summary>
	/// <param name="value">CDM TTL</param>
	void SetCDMTTL(DWORD value);

	/// <summary>
	/// Gets the Unattended Jackpot priority.
	/// </summary>
	/// <returns>Unattended Jackpot priority</returns>
	int GetUnattendedJackpot() const;

	/// <summary>
	/// Sets the Unattended Jackpot priority.
	/// </summary>
	/// <param name="value">Unattended Jackpot priority.</param>
	void SetUnattendedJackpot(int value);

	/// <summary>
	/// Gets the Unattended Jackpot TTL.
	/// </summary>
	/// <returns>Unattended Jackpot TTL</returns>
	DWORD GetUnattendedJackpotTTL() const;

	/// <summary>
	/// Sets the Unattended Jackpot TTL.
	/// </summary>
	/// <param name="value">Unattended Jackpot TTL</param>
	void SetUnattendedJackpotTTL(DWORD value);

	/// <summary>
	/// Gets the Unattended Jackpot Media priority.
	/// </summary>
	/// <returns>Unattended Jackpot Media priority</returns>
	int GetUnattendedJackpotMedia() const;

	/// <summary>
	/// Sets the Unattended Jackpot Media priority.
	/// </summary>
	/// <param name="value">Unattended Jackpot Media priority.</param>
	void SetUnattendedJackpotMedia(int value);

	/// <summary>
	/// Gets the Unattended Jackpot Media TTL.
	/// </summary>
	/// <returns>Unattended Jackpot Media TTL</returns>
	DWORD GetUnattendedJackpotMediaTTL() const;

	/// <summary>
	/// Sets the Unattended Jackpot Media TTL.
	/// </summary>
	/// <param name="value">Unattended Jackpot Pin TTL</param>
	void SetUnattendedJackpotMediaTTL(DWORD value);

	/// <summary>
	/// Sets whether settings were received from a configuration file.
	/// </summary>
	/// <param name="hasConfigFile">IN - true if settings were received from a configuration file, false otherwise.</param>
	void SetHasConfigFile(bool hasConfigFile);

	/// <summary>
	/// Returns true if settings were received from a configuration file, false otherwise.
	/// </summary>
	/// <returns>true if settings were received from a configuration file, false otherwise.</returns>
	bool HasConfigFile() const;

	void SetInvitedGroupsAnticipation(int value);

	int GetInvitedGroupsAnticipation() const;

	void SetInvitedGroupsAnticipationTTL(DWORD value);

	DWORD GetInvitedGroupsAnticipationTTL() const;

	void SetInvitedGroupsLevelUnlocked(int value);

	int GetInvitedGroupsLevelUnlocked() const;

	void SetInvitedGroupsLevelUnlockedTTL(DWORD value);

	DWORD GetInvitedGroupsLevelUnlockedTTL() const;

	void SetInvitedGroupsLeaderBoard(int value);

	int GetInvitedGroupsLeaderBoard() const;

	void SetInvitedGroupsLeaderBoardTTL(DWORD value);

	DWORD GetInvitedGroupsLeaderBoardTTL() const;

	void SetInvitedGroupsFinalResults(int value);

	int GetInvitedGroupsFinalResults() const;

	void SetInvitedGroupsFinalResultsTTL(DWORD value);

	DWORD GetInvitedGroupsFinalResultsTTL() const;

	void SetInvitedGroupsEndingSoon(int value);

	int GetInvitedGroupsEndingSoon() const;

	void SetInvitedGroupsEndingSoonTTL(DWORD value);

	DWORD GetInvitedGroupsEndingSoonTTL() const;

	void SetProgressivePrize(int value);

	int GetProgressivePrize() const;

	void SetProgressivePrizeTTL(DWORD value);

	DWORD GetProgressivePrizeTTL() const;

	void SetPromoGame(int value);

	int GetPromoGame() const;

	void SetPromoGameTTL(DWORD value);

	DWORD GetPromoGameTTL() const;

	// Queueing names
	static const LPCTSTR EQUAL_PRIORITY_INTERRUPT;
	static const LPCTSTR PRIORITY_INITIALINTERACTIVESTARVETIME;
	static const LPCTSTR PRIORITY_INTERACTIVECONTENTTOTALSTARVETIME;
	static const LPCTSTR PRIORITY_USERSCREENTIME;
	static const LPCTSTR PRIORITY_UJPTIMEOUT;	
	static const LPCTSTR PRIORITY_PBTSCREENS;
	static const LPCTSTR PRIORITY_PLAYERINITIATED;
	static const LPCTSTR PRIORITY_PLAYERINITIATEDTTL;
	static const LPCTSTR PRIORITY_INTERACTIVECONTENT;
	static const LPCTSTR PRIORITY_INTERACTIVECONTENTTTL;
	static const LPCTSTR PRIORITY_WINNERMESSAGE;
	static const LPCTSTR PRIORITY_WINNERTTL;
	static const LPCTSTR PRIORITY_SMPROMO;
	static const LPCTSTR PRIORITY_SMPROMOTTL;
	static const LPCTSTR PRIORITY_CONGRATS;
	static const LPCTSTR PRIORITY_CONGRATSTTL;
	static const LPCTSTR PRIORITY_SCHEDULEEND;
	static const LPCTSTR PRIORITY_SCHEDULEENDTTL;
	static const LPCTSTR PRIORITY_WINNERANTICIPATION;
	static const LPCTSTR PRIORITY_WINNERANTICIPATIONTTL;
	static const LPCTSTR PRIORITY_ANNOUNCEMENTS;
	static const LPCTSTR PRIORITY_ANNOUNCEMENTSTTL;
	static const LPCTSTR PRIORITY_BIRTHDAY;
	static const LPCTSTR PRIORITY_BIRTHDAYTTL;
	static const LPCTSTR PRIORITY_ANNIVERSARY;
	static const LPCTSTR PRIORITY_ANNIVERSARYTTL;
	static const LPCTSTR PRIORITY_MASSMARKETING;
	static const LPCTSTR PRIORITY_MASSMARKETINGTTL;
	static const LPCTSTR PRIORITY_CDM;
	static const LPCTSTR PRIORITY_CDMTTL;
	static const LPCTSTR PRIORITY_UNATTENDED_JACKPOT;
	static const LPCTSTR PRIORITY_UNATTENDED_JACKPOT_TTL;
	static const LPCTSTR PRIORITY_UNATTENDED_JACKPOT_MEDIA;
	static const LPCTSTR PRIORITY_UNATTENDED_JACKPOT_MEDIA_TTL;
	static const LPCTSTR PRIORITY_INVITED_GROUPS_ANTICIPATION;
	static const LPCTSTR PRIORITY_INVITED_GROUPS_ANTICIPATIONTTL;
	static const LPCTSTR PRIORITY_INVITED_GROUPS_LEADERBOARD;
	static const LPCTSTR PRIORITY_INVITED_GROUPS_LEADERBOARDTTL;
	static const LPCTSTR PRIORITY_INVITED_GROUPS_FINALRESULTS;
	static const LPCTSTR PRIORITY_INVITED_GROUPS_FINALRESULTSTTL;
	static const LPCTSTR PRIORITY_INVITED_GROUPS_ENDINGSOON;
	static const LPCTSTR PRIORITY_INVITED_GROUPS_ENDINGSOONTTL;
	static const LPCTSTR PRIORITY_INVITED_GROUPS_LEVELUNLOCKED;
	static const LPCTSTR PRIORITY_INVITED_GROUPS_LEVELUNLOCKEDTTL;
	static const LPCTSTR PRIORITY_PROGRESSIVEPRIZE;
	static const LPCTSTR PRIORITY_PROGRESSIVEPRIZETTL;
	static const LPCTSTR PRIORITY_PROMOGAME;
	static const LPCTSTR PRIORITY_PROMOGAMETTL;

	// Queueing values
	static const bool DEFAULT_EQUAL_PRIORITY_INTERRUPT = false;
	static const DWORD DEFAULT_PRIORITY_INITIALINTERACTIVESTARVETIME = 3000;
	static const DWORD DEFAULT_PRIORITY_INTERACTIVECONTENTTOTALSTARVETIME = 10000;
	static const DWORD DEFAULT_PRIORITY_USERSCREENTIME = 300000;
	static const DWORD DEFAULT_UJP_TIMEOUT = 60000;	
	static const DWORD DEFAULT_PRIORITY_PBTSCREENS = 1000;
	static const DWORD DEFAULT_PRIORITY_PLAYERINITIATED = 2000;
	static const DWORD DEFAULT_PRIORITY_PLAYERINITIATEDTTL = MAXDWORD;
	static const DWORD DEFAULT_PRIORITY_INTERACTIVECONTENT = 2500;
	static const DWORD DEFAULT_PRIORITY_INTERACTIVECONTENTTTL = MAXDWORD;
	static const DWORD DEFAULT_PRIORITY_WINNERMESSAGE = 3000;
	static const DWORD DEFAULT_PRIORITY_WINNERTTL = 5000;
	static const DWORD DEFAULT_PRIORITY_PROGRESSIVEPRIZE = 4000;
	static const DWORD DEFAULT_PRIORITY_PROGRESSIVEPRIZETTL = 5000;
	static const DWORD DEFAULT_PRIORITY_PROMOGAME = 4000;
	static const DWORD DEFAULT_PRIORITY_PROMOGAMETTL = 5000;
	static const DWORD DEFAULT_PRIORITY_SMPROMO = 5000;
	static const DWORD DEFAULT_PRIORITY_SMPROMOTTL = MAXDWORD;
	static const DWORD DEFAULT_PRIORITY_CONGRATS = 7000;
	static const DWORD DEFAULT_PRIORITY_CONGRATSTTL = 15000;
	static const DWORD DEFAULT_PRIORITY_SCHEDULEEND = 9000;
	static const DWORD DEFAULT_PRIORITY_SCHEDULEENDTTL = 60000;
	static const DWORD DEFAULT_PRIORITY_WINNERANTICIPATION = 9000;
	static const DWORD DEFAULT_PRIORITY_WINNERANTICIPATIONTTL = 15000;
	static const DWORD DEFAULT_PRIORITY_ANNOUNCEMENTS = 9000;
	static const DWORD DEFAULT_PRIORITY_ANNOUNCEMENTSTTL = 60000;
	static const DWORD DEFAULT_PRIORITY_BIRTHDAY = 11000;
	static const DWORD DEFAULT_PRIORITY_BIRTHDAYTTL = MAXDWORD;
	static const DWORD DEFAULT_PRIORITY_ANNIVERSARY = 12000;
	static const DWORD DEFAULT_PRIORITY_ANNIVERSARYTTL = MAXDWORD;
	static const DWORD DEFAULT_PRIORITY_MASSMARKETING = 6000;
	static const DWORD DEFAULT_PRIORITY_MASSMARKETINGTTL = MAXDWORD;
	static const DWORD DEFAULT_PRIORITY_CDM = 6500;
	static const DWORD DEFAULT_PRIORITY_CDMTTL = MAXDWORD;
	static const DWORD DEFAULT_PRIORITY_UNATTENDED_JACKPOT = 6500;
	static const DWORD DEFAULT_PRIORITY_UNATTENDED_JACKPOT_TTL = MAXDWORD;
	static const DWORD DEFAULT_PRIORITY_UNATTENDED_JACKPOT_MEDIA = 6000;
	static const DWORD DEFAULT_PRIORITY_UNATTENDED_JACKPOT_MEDIA_TTL = 30000;
	static const DWORD DEFAULT_PRIORITY_INVITED_GROUPS_ANTICIPATION = 8000;
	static const DWORD DEFAULT_PRIORITY_INVITED_GROUPS_ANTICIPATIONTTL = 10000;
	static const DWORD DEFAULT_PRIORITY_INVITED_GROUPS_LEADERBOARD = 7500;
	static const DWORD DEFAULT_PRIORITY_INVITED_GROUPS_LEADERBOARDTTL = MAXWORD;
	static const DWORD DEFAULT_PRIORITY_INVITED_GROUPS_FINALRESULTS = 7500;
	static const DWORD DEFAULT_PRIORITY_INVITED_GROUPS_FINALRESULTSTTL = MAXWORD;
	static const DWORD DEFAULT_PRIORITY_INVITED_GROUPS_ENDINGSOON = 8000;
	static const DWORD DEFAULT_PRIORITY_INVITED_GROUPS_ENDINGSOONTTL = MAXWORD;
	static const DWORD DEFAULT_PRIORITY_INVITED_GROUPS_LEVELUNLOCKED = 5500;
	static const DWORD DEFAULT_PRIORITY_INVITED_GROUPS_LEVELUNLOCKEDTTL = MAXWORD;

private:

	static const bool DEFAULT_HAS_CONFIG_FILE = false;
	bool m_hasConfigFile;
	bool m_equalPriorityInterrupt;
	DWORD m_initialInteractiveStarveTime;
	DWORD m_interactiveContentStarveTime;
	DWORD m_userScreenTime;
	DWORD m_ujpTimeout;
	int m_pbtScreens;
	int m_playerInitiated;
	DWORD m_playerInitiatedTTL;
	int m_interactiveContent;
	DWORD m_interactiveContentTTL;
	int m_winnerMessage;
	DWORD m_winnerMessageTTL;
	int m_smPromo;
	DWORD m_smPromoTTL;
	int m_congrats;
	DWORD m_congratsTTL;
	int m_scheduleEnd;
	DWORD m_scheduleEndTTL;
	int m_winnerAnticipation;
	DWORD m_winnerAnticipationTTL;
	int m_announcements;
	DWORD m_announcementsTTL;
	int m_birthday;
	DWORD m_birthdayTTL;
	int m_anniversary;
	DWORD m_anniversaryTTL;
	int m_massMarketing;
	DWORD m_massMarketingTTL;
	int m_CDM;
	DWORD m_CDMTTL;
	int m_unattendedJackpot;
	DWORD m_unattendedJackpotTTL;
	int m_unattendedJackpotMedia;
	DWORD m_unattendedJackpotMediaTTL;
	int m_invitedGroupsAnticipation;
	DWORD m_invitedGroupsAnticipationTTL;
	int m_invitedGroupsLevelUnlocked;
	DWORD m_invitedGroupsLevelUnlockedTTL;
	int m_invitedGroupsFinalResults;
	DWORD m_invitedGroupsFinalResultsTTL;
	int m_invitedGroupsEndingSoon;
	DWORD m_invitedGroupsEndingSoonTTL;
	int m_invitedGroupsLeaderBoard;
	DWORD m_invitedGroupsLeaderBoardTTL;
	int m_progressivePrize;
	DWORD m_progressivePrizeTTL;
	int m_promoGame;
	DWORD m_promoGameTTL;
};
