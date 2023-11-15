#include "stdafx.h"
#include "ProcessDump/WebDiagnostics.h"
#include "Diagnostics.h"
#include "resource.h"
#include "Util/String.h"
#include "Utilities.h"
#include "Util/Path.h"

using namespace std;
void CWebDiagnostics::UpdateProgressive(const CWebDiagnosticsCommonInfo &commonInfo,
	DWORD tickCount,
	const SYSTEMTIME &localTime,
	const CProgressiveConfig &progConfig,
	const CProgressiveConnectionInfo &progInfo,
	const CProgressiveHost &progHost,
	const CEGMAward& progAward,
	const std::shared_ptr<const CEGMAward> &lastStandardWin,
	const std::vector<std::shared_ptr<CEGMAward>> &standardWins,
	const CPlayerSession &playerSession,
	const CEGMAwardManagerState& progState,
	const ProgressiveLevelManager& progressiveLevelManager)
{
	// Get the path of the dump file
	std::string dumpFileName;
	dumpFileName = Path::AddSlash(CDirectoryManager::Instance().HtmlFilePath().c_str()) + ProgressiveInfoFilename;
	std::shared_ptr<std::ostream> file = m_operatingSystem.CreateFile(dumpFileName, ios_base::out | ios_base::trunc);
	SaveProgressiveData(file,
		commonInfo,
		tickCount,
		localTime,
		progConfig,
		progInfo,
		progHost,
		progAward,
		lastStandardWin,
		standardWins,
		playerSession,
		progState,
		progressiveLevelManager);
}

void CWebDiagnostics::SaveProgressiveData(std::shared_ptr<ostream> &file,
	const CWebDiagnosticsCommonInfo &commonInfo,
	DWORD tickCount,
	const SYSTEMTIME &localTime,
	const CProgressiveConfig &progConfig,
	const CProgressiveConnectionInfo &progInfo,
	const CProgressiveHost &progHost,
	const CEGMAward& progAward,
	const std::shared_ptr<const CEGMAward> &lastStandardWin,
	const std::vector<std::shared_ptr<CEGMAward>> &standardWins,
	const CPlayerSession &playerSession,
	const CEGMAwardManagerState& progState,
	const ProgressiveLevelManager& progressiveLevelManager)
{
	try
	{
        if ( !file.get()->fail())
		{
			std::string pageHtml;
            std::stringstream sStream;
            sStream << "<html><META Http-equiv=\"refresh\" Content=\"" << (int)PROGRESSIVE_INFO_REFRESH_SECONDS << "\">";
            pageHtml = sStream.str();
			Write(file, pageHtml);
			Write(file, GetCommonHeaderHtml(commonInfo));

			Write(file, GetProgressiveConfigHtml(progConfig));
			Write(file, GetProgressiveConnectionInfoHtml(progConfig, progInfo, progHost));
			Write(file, GetProgressiveHostListHtml(progConfig));
			Write(file, GetProgressiveHostAddressesHtml(progHost));
			Write(file, GetLastProgressiveAwardHtml(tickCount, progAward, progState, lastStandardWin, standardWins));
			Write(file, GetProgressivePlayerInfoHtml(playerSession));
			Write(file, GetProgressiveLevelsHtml(tickCount, localTime, progressiveLevelManager));
			Write(file, GetProgressiveDownInfoHtml(localTime, progressiveLevelManager));

			Write(file, CUtilities::LoadResourceString(IDS_LAST_BONUS_AWARD) + BR_TAG);
			Write(file, GetBonusAwardHtml(tickCount, progAward, progState));

			Write(file, CUtilities::LoadResourceString(IDS_PREVIOUS_STANDARD_SAS_PROGRESSIVE_WIN) + +BR_TAG);
			Write(file, GetStandardProgressiveWinHtml(lastStandardWin));

			Write(file, CUtilities::LoadResourceString(IDS_CURRENT_STANDARD_SAS_PROGRESSIVE_WINS) + +BR_TAG);
			Write(file, GetStandardProgressiveWinsHtml(standardWins));
			Write(file, HTML_TRAILER);
		}
	}
	catch (...)
	{
		// We do not want to throw to calling thread.
	}
}

std::string CWebDiagnostics::GetProgressiveConfigHtml(const CProgressiveConfig &progConfig)
{
	std::string html;
    std::stringstream SStream;

    SStream << "Configuration File Present: " << (progConfig.HasConfigFile() ? LPCTSTR_TRUE : LPCTSTR_FALSE) <<"<br>";
	SStream << "Progressive Configuration: " << (progConfig.GetActive() ? LPCTSTR_ENABLED : LPCTSTR_DISABLED) << "<br>",
	SStream << "DNS Lookup: "<< (progConfig.GetDNSLookup() ? LPCTSTR_TRUE : LPCTSTR_FALSE) << "<br>";
	SStream << "Ping Interval(ms): " << (unsigned)progConfig.GetPingInterval() << "<br>";

	SStream << "<br>";
    html = SStream.str();
	return html;
}

std::string CWebDiagnostics::GetProgressiveConnectionInfoHtml(const CProgressiveConfig &progConfig,
	const CProgressiveConnectionInfo &progInfo,
	const CProgressiveHost &progHost)
{
	std::string html;
    std::stringstream sStream_html;
	sStream_html << "Progressive Connection Info<br>";

	std::string hostName(progHost.GetName());
	sStream_html << "DNS Hostname: " << (progConfig.GetDNSLookup() ? hostName : LPCTSTR_NA) << "<br>";

	std::string currentIP(LPCTSTR_NA);
	if (!progHost.GetIPAddress().IsEmpty())
	{
		progHost.GetIPAddress().GetIP(currentIP);
	}
	sStream_html << "Current Host IP: " << currentIP << "<br>";
	sStream_html << "Connection State: " << progInfo.GetConnectionStateString() << "<br>";

	currentIP = LPCTSTR_NA;
	if (!progInfo.GetLastMessageIP().IsEmpty())
	{
		progInfo.GetLastMessageIP().GetIP(currentIP);
	}

    int counter  = progInfo.GetDisconnectCount();
    string str = LPCTSTR_NA;
	if (counter)
	{
		str = CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(progInfo.GetDisconnectTimeStampSinceBoot());
	}
	sStream_html << "Last Message IP: " << currentIP << "<br>";
	sStream_html << "Last Message Time: " << progInfo.GetLastMessageTimestampString().c_str() <<"<br>";
	sStream_html << "Failed One Link Call Count: " << counter << "<br>" ;
	sStream_html << "Last failed One Link Call Timestamp: " << str.c_str() <<"<br>";
	int failedMulticastCount(0);
	sStream_html << "Last failed Multicast Timestamp: " << CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(progInfo.GetLastMulticastSocketFailedTimeStamp(failedMulticastCount)).c_str() << "<br>";
	sStream_html << "Failed Multicast Count: " << failedMulticastCount << "<br>";

	sStream_html << "<br>";
    html = sStream_html.str();
	return html;
}

std::string CWebDiagnostics::GetProgressiveHostListHtml(const CProgressiveConfig &progConfig)
{
	std::string html;
    std::stringstream sStream_html;

	sStream_html << "Progressive List<br>";

	int endpointCount(progConfig.GetEndpointCount());
	for (int index = 0; index < endpointCount; ++index)
	{
		sStream_html << "Host: " << progConfig.GetEndpoint(index) << "<br>";
	}

	sStream_html << "<br>";
    html = sStream_html.str();
    return html;
}

std::string CWebDiagnostics::GetProgressiveHostAddressesHtml(const CProgressiveHost &progHost)
{
	std::string html;
    std::stringstream sStream_html;

	sStream_html << "Progressive Addresses<br>";
	std::string hostIP;

	const int endpointCount(progHost.GetIPAddressCount());
	for (int index = 0; index < endpointCount; ++index)
	{
		progHost.GetIPAddress(index).GetIP(hostIP);
		sStream_html << "Host IP: " << hostIP << "<br>";
	}

	sStream_html << "<br>";
    html = sStream_html.str();
	return html;
}

std::string CWebDiagnostics::GetBonusAwardHtml(DWORD tickCount,
	const CEGMAward& progAward,
	const CEGMAwardManagerState& progState)
{
	std::string html;
    std::stringstream sStream_html;

	if (progAward.GetAwardState() == NoAward)
	{
			sStream_html << LPCTSTR_NA << "<br>" ;
	}
	else
	{
		sStream_html << "Received from System Time: " << CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(progAward.GetReceivedFromSystemTime()) <<"<br>";
		sStream_html << "Level ID: " << (int)progAward.GetLevelId()  << "<br>";
		sStream_html << "Jackpot ID: " << (long long)progAward.GetJackpotId() << "<br>";

		std::string sessionIdString(CUtilities::LoadResourceString(IDS_NA));
		int sessionId;
		bool hasSessionId(progAward.GetSessionId(sessionId));
		if (hasSessionId)
		{
            sessionIdString = std::to_string(sessionId);
		}
		sStream_html << CUtilities::LoadResourceString(IDS_WEB_SESSION_ID) << ": " << sessionIdString << "<br>";
		sStream_html << "Award Amount Hit: " << CUtilities::GetFormattedCashString(progAward.GetAwardAmountHit()) << "<br>";
		sStream_html << "Award Amount Paid: "<< CUtilities::GetFormattedCashString(progAward.GetAwardAmountPaid()) << "<br>";
		sStream_html << "Award State: "<< progAward.GetAwardStateString() << "<br>";
		sStream_html << "Sent to Game Count: " << (int)progAward.GetSentToGameCount() <<"<br>";

		std::string sentToGameTime(LPCTSTR_NA);
		if (progAward.GetSentToGameCount() != 0)
		{
			sentToGameTime = CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(progAward.GetSentToGameTime());
		}
		sStream_html << "Last Sent to Game Time: " << sentToGameTime << "<br>";

		sStream_html << "Original Machine Bonus Meter Value: "<< (long long)progAward.GetOriginalMachineBonusMeterValue() << "<br>";
		sStream_html << "Original Attendant Bonusing Meter Value: " << (long long)progAward.GetOriginalAttendantBonusingMeterValue() << "<br>";
		sStream_html << "Original Legacy Bonus Meter Value: " << (long long)progAward.GetOriginalLegacyBonusMeterValue() << " <br>";
		sStream_html << "Duplicate Hits Received from System: "<< (int)progAward.GetDuplicateHitCount() << "<br>";
		sStream_html << "Other Hits Received from System: "<< (int)progAward.GetOtherHitCount() << "<br>";

		bool inProgress(progState.m_waitingForGCF || progAward.InProgress());
		sStream_html << "Award Processing in Progress: " <<  (inProgress ? LPCTSTR_TRUE : LPCTSTR_FALSE) << "<br>";
		sStream_html << "Waiting for Game Communications: "<<  (progState.m_waitingForGCF ? LPCTSTR_TRUE : LPCTSTR_FALSE)  <<"<br>";

		{
			std::string timeSpanString(LPCTSTR_NA);
			if (inProgress)
			{
				if (progState.m_awardExpired)
				{
					timeSpanString = _T("Expirable");
				}
				else
				{
					DWORD elapsedTickCount = tickCount - progState.m_awardTickCountStart;
					if (elapsedTickCount < CEGMAwardManagerState::AWARD_TICK_COUNT_EXPIRED)
					{
						DWORD remainingTickCount = CEGMAwardManagerState::AWARD_TICK_COUNT_EXPIRED - elapsedTickCount;
						// Round up to nearest second.
						DWORD remainingSeconds = (remainingTickCount + TICK_COUNTS_PER_SECOND - 1) / TICK_COUNTS_PER_SECOND;
						if (remainingSeconds > 0)
						{
                            UINT Hour = UINT(remainingSeconds/3600);
                            UINT Minutes = UINT(( remainingSeconds%3600 )/60);
                            UINT Seconds =  UINT(( remainingSeconds%3600 )%60);
							std::stringstream timeStringStream;
							timeStringStream << std::setw(2) << std::setfill('0') << Hour << ":" << std::setw(2) << std::setfill('0') << Minutes << ":" << std::setw(2) << std::setfill('0') << Seconds;
							timeSpanString = timeStringStream.str();
						} 
					}
				}
			}
			sStream_html << "Time Until Award Expires: "<< timeSpanString <<"<br>";
		}
	}

	sStream_html <<  "<br>";
    html = sStream_html.str();
	return html;
}

std::string CWebDiagnostics::GetStandardProgressiveWinHtml(const std::shared_ptr<const CEGMAward> &standardWin)
{
	std::string html;
    std::string str_format;

	if (standardWin.get() != nullptr && standardWin->GetAwardState() != NoAward)
	{
        str_format = CUtilities::LoadResourceString(IDS_FMT_RECEIVED_FROM_EGM_TIME) + BR_TAG;
		html += FormatString( str_format.c_str(),
			CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(standardWin->GetReceivedFromEGMTime()).c_str());

        str_format = CUtilities::LoadResourceString(IDS_FMT_SAS_LEVEL_ID) + BR_TAG;
		html += FormatString( str_format.c_str(), (int)standardWin->GetSASLevelId());

        str_format = CUtilities::LoadResourceString(IDS_FMT_SYSTEM_LEVEL_ID) + BR_TAG;
		html += FormatString( str_format.c_str(), (int)standardWin->GetLevelId());

        str_format = CUtilities::LoadResourceString(IDS_FMT_JACKPOT_ID) + BR_TAG;
		html += FormatString( str_format.c_str(), (long long)standardWin->GetJackpotId() );

        str_format = CUtilities::LoadResourceString(IDS_FMT_AWARD_AMOUNT_HIT) + BR_TAG;
        html += FormatString( str_format.c_str(),
			CUtilities::GetFormattedCashString(standardWin->GetAwardAmountHit()).c_str());

        str_format = CUtilities::LoadResourceString(IDS_FMT_AWARD_STATE) + BR_TAG;
        html += FormatString( str_format.c_str(),
			standardWin->GetAwardStateString().c_str());

        // Don't indicate the IsStandardProgressive() state, as it is not set reliably for standard wins.
        str_format =  CUtilities::LoadResourceString(IDS_FMT_HIT_RECEIVED_FROM_SYSTEM) + BR_TAG;
		html += FormatString( str_format.c_str(),
			(standardWin->isHitReceived() ? CWebDiagnostics::LPCTSTR_TRUE : CWebDiagnostics::LPCTSTR_FALSE));

        str_format = CUtilities::LoadResourceString(IDS_FMT_HAND_PAY_PENDING_SIGNALED) + BR_TAG;
        html += FormatString( str_format.c_str(),
			(standardWin->isHandpayPendingSignaled() ? CWebDiagnostics::LPCTSTR_TRUE : CWebDiagnostics::LPCTSTR_FALSE));

        str_format = CUtilities::LoadResourceString(IDS_FMT_EGM_PAID_SIGNALED) + BR_TAG;
        html += FormatString( str_format.c_str(),
			(standardWin->isEgmPaidSignaled() ? CWebDiagnostics::LPCTSTR_TRUE : CWebDiagnostics::LPCTSTR_FALSE));

	}
	else
	{
        str_format = CUtilities::LoadResourceString(IDS_NA) + BR_TAG;
		html += str_format;
	}

	html += BR_TAG;
	return html;
}

std::string CWebDiagnostics::GetStandardProgressiveWinsHtml(const std::vector<std::shared_ptr<CEGMAward>> &standardWins)
{
	std::string html;

	for (auto awardIterator = standardWins.cbegin(); awardIterator != standardWins.cend(); ++awardIterator)
	{
		if (*awardIterator != nullptr)
		{
			if (awardIterator->get() != nullptr && awardIterator->get()->GetAwardState() != NoAward)
			{
				html += GetStandardProgressiveWinHtml(*awardIterator);
			}
		}
	}

    if (html.empty())
	{
		html += CUtilities::LoadResourceString(IDS_NA) + BR_TAG ;
		html += BR_TAG;
	}
	return html;
}

std::shared_ptr<const CEGMAward> CWebDiagnostics::GetNewestStandardProgressiveWin(
	const std::shared_ptr<const CEGMAward> &lastStandardWin,
	const std::vector<std::shared_ptr<CEGMAward>> &standardWins)

{
	std::shared_ptr<const CEGMAward> newest(lastStandardWin);

	for (auto it = standardWins.cbegin(); it != standardWins.cend(); ++it)
	{
		if (*it != nullptr && it->get() != nullptr && it->get()->GetAwardState() != NoAward)
		{
			if (newest.get() == nullptr ||
				newest->GetAwardState() == NoAward ||
				CUtilities::CompareSysTimes(newest.get()->GetReceivedFromEGMTime(), it->get()->GetReceivedFromEGMTime()) <= 0)
			{
				// If times are the same, keep the last one found.
				newest = *it;
			}
		}
	}

	return newest;
}

std::string CWebDiagnostics::GetLastProgressiveAwardHtml(DWORD tickCount,
	const CEGMAward &progAward,
	const CEGMAwardManagerState &progState,
	const std::shared_ptr<const CEGMAward> &lastStandardWin,
	const std::vector<std::shared_ptr<CEGMAward>> &standardWins)
{
	std::string html;
	std::shared_ptr<const CEGMAward> newestStandardWin(GetNewestStandardProgressiveWin(lastStandardWin, standardWins));

	if (newestStandardWin.get() != nullptr && newestStandardWin->GetAwardState() != NoAward)
	{
		if (progAward.GetAwardState() == NoAward ||
			CUtilities::CompareSysTimes(progAward.GetReceivedFromSystemTime(),
			newestStandardWin.get()->GetReceivedFromEGMTime()) <= 0)
		{
			// Times are the same, so use the standard SAS progressive win.
			html = GetStandardProgressiveWinHtml(newestStandardWin);
		}
	}

	if (html.empty())
	{
		html = GetBonusAwardHtml(tickCount, progAward, progState);
	}

	html.insert(0, CUtilities::LoadResourceString(IDS_LAST_PROGRESSIVE_AWARD) + BR_TAG);
	return html;
}

std::string CWebDiagnostics::GetProgressivePlayerInfoHtml(const CPlayerSession &playerSession)
{
	std::string html(_T("Progressive Player Info<br><div style=\"margin-left:6px;\">"));

	bool sessionActive;
	bool cardedPlayer;
	int sessionId;
	playerSession.GetSessionActiveInfo(sessionActive, cardedPlayer, sessionId);
	html += FormatString("Session Active: %s<br>", GetBoolString(sessionActive));
	html += FormatString("Player Type: %s<br>", playerSession.GetPlayerType().c_str());
	html += FormatString("Session ID: %d<br>", (int)sessionId);
	html += FormatString("Last Session ID Increment Time: %s<br>",
		CUtilities::FormatMilitaryDateTimeString(playerSession.GetLastSessionIdIncrementTime()).c_str());
	html += FormatString("Session ID Match Status: %s<br>",GetBoolString(playerSession.GetSessionIdMatchStatus()));

	html +="</div><br>";
	return html;
}

std::string CProgressiveLevel::GetLevelTypeString() const
{
	LPCTSTR pTypeStr;
	switch (m_type)
	{
	case ProgressiveLevelType::Standard:
		pTypeStr = _T("Standard");
		break;

	case ProgressiveLevelType::Bonus:
		pTypeStr = _T("Bonus");
		break;

	case ProgressiveLevelType::AccountingOnly:
		pTypeStr = _T("Accounting Only");
		break;

	case ProgressiveLevelType::PalcomX:
		pTypeStr = _T("PalcomX");
		break;

	default:
		pTypeStr = _T("Invalid");
		break;
	}
	return pTypeStr;
}

std::string CProgressiveLevel::GetShowOdometerString() const
{
	const LPCTSTR ShowNoOdometer = _T("false");
	const LPCTSTR ShowStandardOdometer = _T("true");
	const LPCTSTR ShowPteOdometer = _T("PTE Progress");
	const LPCTSTR ShowScatterOdometer = _T("Scatter Progress");
	const LPCTSTR ShowPlayXGetYTrigger = _T("PlayXGetY Progress");

	LPCTSTR showOdometerStr = ShowNoOdometer;

	if (ShowOdometer())
	{
		if (IsScatter())
		{
			showOdometerStr = ShowScatterOdometer;
		}
		else if (IsPlayXGetY())
		{
			showOdometerStr = ShowPlayXGetYTrigger;
		}
		else if (IsPte())
		{
			showOdometerStr = ShowPteOdometer;
		}
		else
		{
			showOdometerStr = ShowStandardOdometer;
		}
	}

	return showOdometerStr;
}

std::string CProgressiveLevel::GetDiagnosticsHtml(DWORD tickCount, const SYSTEMTIME& localTime) const
{
	std::string html;
	double updatedSeconds = (double)(tickCount - m_updatedTickCount) / MILLISECONDS_PER_SECOND;

	html += FormatString("<tr><td>%lld</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td>",
		m_id,
		m_name.c_str(),
		(CUtilities::GetFormattedCashString(m_value)).c_str(),
		GetLevelTypeString().c_str(),
		(CWebDiagnostics::GetBoolString(IsEligible())),
		GetShowOdometerString().c_str()
		);

	if (m_state == Stale)
	{
		html += FormatString("<td><font color=\"red\">%0.1f</font></td>", updatedSeconds);
	}
	else
	{
		html += FormatString("<td>%0.1f</td>", updatedSeconds);
	}

	CountHistory::Data today;
	CountHistory::Data yesterday;
	m_staleCount.GetDataNoRoll(&today, &yesterday, &localTime); // gets only counts, so time isn't needed

	if (m_staleCount.IsLastSetTimeValid())
	{
		std::string timeStr = CUtilities::FormatMilitaryDateTimeString(CUtilities::GetTimeFromUTCSystemTime(m_staleCount.GetLastSetTime()));
		html += FormatString("<td><font color=\"red\">%s</font></td>", timeStr.c_str());
	}
	else
	{
		html += "<td></td>";
	}

	if (today.GetCount() > 0)
	{
		html += FormatString("<td><font color=\"red\">%u (%u ms)</font></td>", today.GetCount(), today.GetMilliseconds());
	}
	else
	{
		html += "<td>0</td>";
	}

	if (yesterday.GetCount() > 0)
	{
		html += FormatString("<td><font color=\"red\">%u (%u ms)</font></td>", yesterday.GetCount(), yesterday.GetMilliseconds());
	}
	else
	{
		html += "<td>0</td>";
	}

	html += "<td>";
	if (!m_sasLevels.empty())
	{
		html += FormatString("%d", m_sasLevels[0]);

		for (size_t i = 1; i < m_sasLevels.size(); i++)
			html += FormatString(", %d", m_sasLevels[i]);
	}
	html += "</td>";

	html += FormatString("<td>%s</td>", m_url.c_str());
	html += "</tr>";

	return html;
}


std::string ProgressiveLevelManager::GetDownInfoDiagnosticsHtml(const SYSTEMTIME& localTime) const
{
	std::unique_lock<std::recursive_mutex> lock(m_criticalSection);
	std::string html = _T("Progressive Down Alert Info:<br><div style=\"margin-left:6px;\">");

	CountHistory::Data today;
	CountHistory::Data yesterday;
	m_downCount.GetDataNoRoll(&today, &yesterday, &localTime);

	html += FormatString("Is Down: %s<br>", CWebDiagnostics::GetBoolString(today.IsSet()));
	if (m_downCount.IsLastSetTimeValid())
	{
		html += FormatString("Last Down Time: %s<br>",
			CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(m_downCount.GetLastSetTime()).c_str());
        if (m_downCount.IsLastResetTimeValid())
		{
            html += FormatString("Last Up Time: %s<br>",
				CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(m_downCount.GetLastResetTime()).c_str());
		}
		else
		{
			html += "Last Up Time:<br>";
		}
	}
	else
	{
		html += _T("Last Down Time:<br>");
		html += _T("Last Up Time:<br>");
	}

	html += FormatString("Number of Times Down Today: %u<br>", today.GetCount());
	html += FormatString("Total Milliseconds Down Today: %u<br>", today.GetMilliseconds());

	html += FormatString("Number of Times Down Yesterday: %u<br>", yesterday.GetCount());
	html += FormatString("Total Milliseconds Down Yesterday: %u<br>", yesterday.GetMilliseconds());

	html += "</div><br>";

	return html;
}

std::string ProgressiveLevelManager::GetDiagnosticsHtml(DWORD tickCount, const SYSTEMTIME& localTime) const
{
	std::unique_lock<std::recursive_mutex> lock(m_criticalSection);
	std::string html(_T("Progressive Level Info:<br><div style=\"margin-left:6px;\">"));

	if (m_progressiveGroupId == 0 && HasLevelType(ProgressiveLevelType::Standard))
	{
		html += FormatString("<font color=\"red\">Group ID: %d -- WARNING: This must be non-zero for standard SAS progressives!</font><br>",
			m_progressiveGroupId);
	}
	else
	{
		html += FormatString("Group ID: %d<br>", m_progressiveGroupId);
	}

	html += FormatString("Updated Time: %s<br>",
		CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(m_receivedTime).c_str());
	html += FormatString("Last Eligibility Received Time: %s<br>",
		CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(m_eligibilityReceivedTime).c_str());
	html += FormatString("Eligibility Determined: %s<br>", CWebDiagnostics::GetBoolString(m_eligibilityDetermined));
	html += FormatString("Eligibility Session ID: %d<br>", m_eligibilitySessionId);
	html += FormatString("Query Cards: %s<br>", CWebDiagnostics::GetBoolString(m_queryPokerCards));

	if (m_levels.size() != 0)
	{
		html +=  std::string("Levels:<br>");
		html +=  std::string("<style>table{border-width:1px;border-style:solid;border-collapse:collapse;}") + 
			"th,td{border-width:1px;border-style:solid;padding:3px 7px 2px 7px;}</style>" + 
			"<table>" + 
			"<tr><th>ID</th><th>Name</th><th>Value</th><th>Type</th><th>Eligible</th><th>Show Odometer</th><th>Last Updated (seconds)</th>" + 
			"<th>Last Stale Time</th><th>Stale Count Today</th><th>Stale Count Yesterday</th><th>SAS IDs</th><th>URL</th></tr>";

		for (const auto& level : m_levels)
		{
			html += level.second->GetDiagnosticsHtml(tickCount, localTime);
		}

		html += "</table>";
	}

	html += "</div><br>";
	return html;
}

std::string CWebDiagnostics::GetProgressiveLevelsHtml(
	const DWORD tickCount,
	const SYSTEMTIME &localTime,
	const ProgressiveLevelManager& levelManager)
{
	return levelManager.GetDiagnosticsHtml(tickCount, localTime);
}

std::string CWebDiagnostics::GetProgressiveDownInfoHtml(const SYSTEMTIME& localTime, const ProgressiveLevelManager& levelManager)
{
	return levelManager.GetDownInfoDiagnosticsHtml(localTime);
}
//#endif //TODO-PORT

std::string CWebDiagnostics::GetLastGameInfo(const CPlayerSession &playerSession)
{
	string html;

	html += "Last Game Played Information<br>";

	html += FormatString("Game In Progress: %s<br>",
		static_cast<LPCTSTR>(GetBoolString(playerSession.GetGameInProgress())));
	html += FormatString("Last Game Start: %s<br>",
		static_cast<LPCTSTR>(CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(playerSession.GetLastGameStarted()).c_str()));
	html += FormatString("Last Game End: %s<br>",
		static_cast<LPCTSTR>(CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(playerSession.GetLastGameEnd()).c_str()));

	if (playerSession.GetLastGameNumber() < 0)
	{
		html += FormatString("Last Game Number: %s<br>", LPCTSTR_NA);
	}
	else
	{
		html += FormatString("Last Game Number: %d<br>", playerSession.GetLastGameNumber());
	}

	if (playerSession.GetLastCoinsPlayed() < 0)
	{
		html += FormatString("Last Coins Played: %s<br>", LPCTSTR_NA);
	}
	else
	{
		html += FormatString("Last Coins Played: %ld<br>", playerSession.GetLastCoinsPlayed());
	}

	if (playerSession.GetLastDenom() < 0)
	{
		html += FormatString("Last Denomination: %s<br>", LPCTSTR_NA);
	}
	else
	{
		html += FormatString("Last Denomination: %d<br>", playerSession.GetLastDenom());
	}

	html += FormatString("EGM Sends Game End: %s<br>",
		static_cast<LPCTSTR>(GetBoolString(playerSession.GetEgmSendsGameEnd())));

	html += _T("<br>");
	return html;
}

std::string CWebDiagnostics::GetEgmTypeString(const EGMType egmType)
{
	LPCTSTR id(IDS_NA);
	switch (egmType)
	{
	case STANDARD:
		id = IDS_STANDARD;
		break;
	case BALLY:
		id = IDS_BALLY;
		break;
	case WMS:
		id = IDS_WMS;
		break;
	case IGTS2000:
		id = IDS_IGTS2000;
		break;
	case CUSTOM:
		id = IDS_CUSTOM;
		break;
	case NO_EGM:
	default:
		id = IDS_NONE;;
		break;
	}

	return CUtilities::LoadResourceString(id);
}


std::string CWebDiagnostics::GetEgmProtocolString(EGMProtocol egmProtocol)
{
	LPCTSTR id(IDS_NA);
	switch (egmProtocol)
	{
	case IGT_SAS_3xx:
		id = IDS_SAS_3XX;
		break;
	case SAS5xx:
		id = IDS_SAS_5XX;
		break;
	case SAS6xx:
		id = IDS_SAS_6XX;
		break;
	default:
		id = IDS_NA;
		break;
	}

	return CUtilities::LoadResourceString(id);
}

std::string CWebDiagnostics::GetCurrentMultiplierAndDivider(const CWebDiagnosticsStatusInfo &statusInfo)
{
	string html;

	html += "Session Multiplier and Divider<br>";

	if (statusInfo.m_currentMultiplier == INVALID_MULTIPLIER)
	{
		html += FormatString("Current Multiplier: N/A<br>");
	}
	else
	{
		html += FormatString("Current Multiplier: %u<br>", statusInfo.m_currentMultiplier);
	}

	if (statusInfo.m_currentDivider == INVALID_DIVIDER)
	{
		html += FormatString("Current Divider: N/A<br>");
	}
	else
	{
		html += FormatString("Current Divider: %u<br>", statusInfo.m_currentDivider);
	}

	html += _T("<br>");
	return html;
}

std::string CWebDiagnostics::GetEgmConfigHtml(const CWebDiagnosticsStatusInfo &statusInfo, const CEGMConfig &egmConfig)
{
	string html;

	html += "EGM Configuration<br>";

	html += FormatString((CUtilities::LoadResourceString(IDS_FMT_EGM_TYPE) + BR_TAG).c_str(),
		GetEgmTypeString(egmConfig.GetEGMType()).c_str());

	int pollingSpeed = egmConfig.GetEGMPollingSpeed() == MS200 ? 200 : 40; // Same interpretation as CSASProtocol.
	html += FormatString("Polling speed(ms): %d<br>", (int)pollingSpeed);
	html += FormatString((CUtilities::LoadResourceString(IDS_FMT_EGM_PROTOCOL) + BR_TAG).c_str(),
		static_cast<LPCTSTR>(GetEgmProtocolString(egmConfig.GetEGMProtocol()).c_str()));

	string reportedEgmProtocolVersion(statusInfo.m_reportedEgmProtocolVersion);
	if (reportedEgmProtocolVersion.empty())
	{
		reportedEgmProtocolVersion = CUtilities::LoadResourceString(IDS_NA);
	}
	html += FormatString((CUtilities::LoadResourceString(IDS_FMT_REPORTED_EGM_PROTOCOL_VERSION) + BR_TAG).c_str(),
		static_cast<LPCTSTR>(reportedEgmProtocolVersion.c_str()));

	// For ticket expiration, display general human understandable information, not confidential (non-disclosure) SAS protocol details.
	string ticketExpirationSupport(LPCTSTR_NA);
	if (statusInfo.m_egmSupportsLP7B_Available)
	{
		ticketExpirationSupport = GetBoolString(statusInfo.m_egmSupportsLP7B);
	}
	html += FormatString((CUtilities::LoadResourceString(IDS_FMT_EGM_SUPPORTS_TICKET_EXPIRATION_GT_255_DAYS) + BR_TAG).c_str(),
		static_cast<LPCTSTR>(ticketExpirationSupport.c_str()));
	
	html += FormatString("A8 Reset to Credit : %s<br>", (statusInfo.m_jackpotA8Reset ? "Yes":"No"));

	html += FormatString("Bonus Payment Method : %s<br>", statusInfo.m_bonusPaymentMethod.c_str());

	if (!statusInfo.m_reportedEgmProtocolVersion.empty())  // means EGM is connected
	{
		// AFT and EFT mode can both be supported but we will display the mode we will be using.

		switch (statusInfo.m_egmTransferMode)
		{
			case EGMTransferMode_AFT:
				html += FormatString("Electronic Transfer Mode : AFT<br>");
			break;
			case EGMTransferMode_EFT:
				html += FormatString("Electronic Transfer Mode : EFT<br>");
			break;
			case EGMTransferMode_PENDING:
				html += FormatString("Electronic Transfer Mode : Pending PBT Transfer<br>");
			break;
			case EGMTransferMode_NONE:
				html += FormatString("Electronic Transfer Mode : None<br>");
			break;
		}
	}
	else
	{
		html += FormatString("Electronic Transfer Mode : N/A<br>");
	}

	html += FormatString("Cashout To Host Enabled From FloorLogix: %s<br>", 
		(statusInfo.m_cashoutToHostEnabled ? "Yes" : "No"));
	html += FormatString("Cashout To Host Enabled on EGM: %s<br>", 
		(statusInfo.m_cashoutToHostStatusOnEGM.m_hostCashoutStatus.m_cashoutEnabled ? "Yes" : "No"));
	html += FormatString("Cashout To Host controlled by host on EGM: %s<br>", 
		(statusInfo.m_cashoutToHostStatusOnEGM.m_hostCashoutStatus.m_cashoutControlledByHost ? "Yes" : "No"));
	if (statusInfo.m_cashoutToHostStatusOnEGM.m_hostCashoutStatus.m_cashoutEnabled)
	{
		html += FormatString("Cashout To Host mode soft on EGM: %s<br>",
			(statusInfo.m_cashoutToHostStatusOnEGM.m_hostCashoutStatus.m_cashoutModeHard ? "No" : "Yes"));
	}
	else
	{
		html += "Cashout To Host mode hard on EGM: N/A<br>";
	}
	html += FormatString("Cashout To Host requests accepted: %d<br>",
		(statusInfo.m_cashoutToHostRequestsAccepted));
	html += FormatString("Cashout To Host requests canceled: %d<br>",
		(statusInfo.m_cashoutToHostRequestsCanceled));

	html += _T("<br>");
	return html;
}


