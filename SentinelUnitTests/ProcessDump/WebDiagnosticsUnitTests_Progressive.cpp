#include "stdafx.h"
#include "ProcessDump/WebDiagnostics.h"
#include "SASProtocol/SASProtocol.h"
#include "EGMConfig.h"

// Unit testing includes.
#include "UnitTestingOperatingSystem.h"
#include "UnitTestingUtils.h"
#include "ProgressiveProtocol/ProgressiveLevelTestHelpers.h"

using namespace std;

// AssertResult macro(s) for calling CUnitTestingUtils from a test method.
#define AssertResult5(result, resultLength, expected, expectedLength, resultName) \
	CUnitTestingUtils::AssertEqual(result, resultLength, expected, expectedLength, resultName, testIndex, __TFUNCTION__, __LINE__);
#define AssertResult4(result, resultLength, expected, expectedLength) \
	AssertResult5(result, resultLength, expected, expectedLength, #result)

#define AssertResult3(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, testIndex, __TFUNCTION__, __LINE__);
#define AssertResult2(result, expected) AssertResult3(result, expected, #result)

#define AssertResult3m(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, m_testIndex, __TFUNCTION__, __LINE__);
#define AssertResult2m(result, expected) AssertResult3m(result, expected, #result)

    #define AssertResult4f(result, expected, expectedLength, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, expectedLength, resultName, testIndex, __TFUNCTION__, __LINE__);
#define AssertResult3f(result, expected, expectedLength) AssertResult4f(result, expected, expectedLength, #result)

// AssertResult macros for calling CUnitTestingUtils from a helper method.
#define AssertResult3h(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, testIndex, functionName, lineNumber, extraIndex);
#define AssertResult2h(result, expected) AssertResult3h(result, expected, #result)

// AssertResult macro for calling CUnitTestingUtils (with an extra index) from a test method.
#define AssertResult3x(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, testIndex, __TFUNCTION__, __LINE__, &extraIndex);
#define AssertResult2x(result, expected) AssertResult3x(result, expected, #result)

#include "gtest/gtest.h"


namespace SentinelNativeUnitTests
{
    using namespace ProgressiveLevelTestHelpers;
    class WebDiagnostics_Progressive : public ::testing::Test
    {
        public:
		// Class member variable(s).
		int m_testIndex;

		string CalcBonusAwardHtml(DWORD tickCount,
			const CEGMAward& progAward,
			const CEGMAwardManagerState& progState)
		{
			string html;

			if (progAward.GetAwardState() == NoAward)
			{
				html += FormatString("%s<br>", CUnitTestingUtils::LPCTSTR_NA);
			}
			else
			{
				html += FormatString("Received from System Time: %s<br>",
					CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(progAward.GetReceivedFromSystemTime()).c_str());
				html += FormatString("Level ID: %d<br>", (int)progAward.GetLevelId());
				html += FormatString("Jackpot ID: %lld<br>", (long long)progAward.GetJackpotId());

				string sessionIdString(CUnitTestingUtils::LPCTSTR_NA);
				int sessionId;
				bool hasSessionId(progAward.GetSessionId(sessionId));
				if (hasSessionId)
				{
					sessionIdString+=FormatString("%d", (int)sessionId);
				}
				html += FormatString("Session ID: %s<br>", sessionIdString.c_str());

				html += FormatString("Award Amount Hit: %s<br>",
					CUtilities::GetFormattedCashString(progAward.GetAwardAmountHit()).c_str());
				html += FormatString("Award Amount Paid: %s<br>",
					CUtilities::GetFormattedCashString(progAward.GetAwardAmountPaid()).c_str());
				html += FormatString("Award State: %s<br>",
					progAward.GetAwardStateString().c_str());
				html += FormatString("Sent to Game Count: %d<br>", (int)progAward.GetSentToGameCount());

				string sentToGameTime(CUnitTestingUtils::LPCTSTR_NA);
				if (progAward.GetSentToGameCount() != 0)
				{
					sentToGameTime = CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(progAward.GetSentToGameTime());
				}
				html += FormatString("Last Sent to Game Time: %s<br>", sentToGameTime.c_str());

				html += FormatString("Original Machine Bonus Meter Value: %lld<br>", (long long)progAward.GetOriginalMachineBonusMeterValue());
				html += FormatString("Original Attendant Bonusing Meter Value: %lld<br>", (long long)progAward.GetOriginalAttendantBonusingMeterValue());
				html += FormatString("Original Legacy Bonus Meter Value: %lld <br>", (long long)progAward.GetOriginalLegacyBonusMeterValue());
				html += FormatString("Duplicate Hits Received from System: %d<br>", (int)progAward.GetDuplicateHitCount());
				html += FormatString("Other Hits Received from System: %d<br>", (int)progAward.GetOtherHitCount());

				bool inProgress(progState.m_waitingForGCF || progAward.InProgress());
				html += FormatString("Award Processing in Progress: %s<br>",
					static_cast<LPCTSTR>(inProgress ? CUnitTestingUtils::TEXT_TRUE : CUnitTestingUtils::TEXT_FALSE));

				html += FormatString("Waiting for Game Communications: %s<br>",
					static_cast<LPCTSTR>(progState.m_waitingForGCF ? CUnitTestingUtils::TEXT_TRUE : CUnitTestingUtils::TEXT_FALSE));

				{
					string timeSpanString(CUnitTestingUtils::LPCTSTR_NA);
					if (inProgress)
					{
						if (progState.m_awardExpired)
						{
							timeSpanString = "Expirable";
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
									const time_t timeSpan(remainingSeconds);
									timeSpanString = CUtilities::FormatDateTimeString(timeSpan, "%H:%M:%S");
								}
							}
						}
					}
					html += FormatString("Time Until Award Expires: %s<br>", timeSpanString.c_str());
				}
			}

			html += "<br>";
			return html;
		}

		string CalcStandardProgressiveWinHtml(const std::shared_ptr<const CEGMAward> &standardWin)
		{
			string html;

			if (standardWin.get() != nullptr && standardWin->GetAwardState() != NoAward)
			{
				html += FormatString("Received from EGM Time: %s<br>",
					CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(standardWin->GetReceivedFromEGMTime()).c_str());
				html += FormatString("SAS Level ID: %d<br>", (int)standardWin->GetSASLevelId());
				html += FormatString("System Level ID: %d<br>", (int)standardWin->GetLevelId());
				html += FormatString("Jackpot ID: %lld<br>", (long long)standardWin->GetJackpotId());
				html += FormatString("Award Amount Hit: %s<br>",
					CUtilities::GetFormattedCashString(standardWin->GetAwardAmountHit()).c_str());
				html += FormatString("Award State: %s<br>",
					standardWin->GetAwardStateString().c_str());
				html += FormatString("Hit Received from System: %s<br>",
					standardWin->isHitReceived() ? CWebDiagnostics::LPCTSTR_TRUE : CWebDiagnostics::LPCTSTR_FALSE);
				html += FormatString("Hand Pay Pending Signaled: %s<br>",
					standardWin->isHandpayPendingSignaled() ? CWebDiagnostics::LPCTSTR_TRUE : CWebDiagnostics::LPCTSTR_FALSE);
				html += FormatString("EGM Paid Signaled: %s<br>",
					standardWin->isEgmPaidSignaled() ? CWebDiagnostics::LPCTSTR_TRUE : CWebDiagnostics::LPCTSTR_FALSE);
			}
			else
			{
				html += FormatString("%s<br>", CUnitTestingUtils::LPCTSTR_NA);
			}

			html += "<br>";
			return html;
		}

		string CalcStandardProgressiveWinsHtml(const std::vector<std::shared_ptr<CEGMAward>> &standardWins)
		{
			string html;

			for (auto awardIterator = standardWins.cbegin(); awardIterator != standardWins.cend(); ++awardIterator)
			{
				if (*awardIterator != nullptr && (*awardIterator)->GetAwardState() != NoAward)
				{
					html += CalcStandardProgressiveWinHtml(*awardIterator);
				}
			}

			if (html.empty())
			{
				html += FormatString("%s<br>", CUnitTestingUtils::LPCTSTR_NA);
				html += "<br>";
			}
			return html;
		}

		std::shared_ptr<const CEGMAward> CalcNewestStandardProgressiveWin(const std::shared_ptr<const CEGMAward> &lastStandardWin,
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

		string CalcLastProgressiveAwardHtml(DWORD tickCount,
			const CEGMAward &progAward,
			const CEGMAwardManagerState &progState,
			const std::shared_ptr<const CEGMAward> &lastStandardWin,
			const std::vector<std::shared_ptr<CEGMAward>> &standardWins)
		{
			string html;

			std::shared_ptr<const CEGMAward> newestStandardWin(CalcNewestStandardProgressiveWin(lastStandardWin, standardWins));

			if (newestStandardWin.get() != nullptr && newestStandardWin->GetAwardState() != NoAward)
			{
				if (progAward.GetAwardState() == NoAward ||
					CUtilities::CompareSysTimes(progAward.GetReceivedFromSystemTime(),
						newestStandardWin.get()->GetReceivedFromEGMTime()) <= 0)
				{
					// Times are the same, so use the standard SAS progressive win.
					html = CalcStandardProgressiveWinHtml(newestStandardWin);
				}
			}
			if (html.empty())
			{
				html = CalcBonusAwardHtml(tickCount, progAward, progState);
			}

			html.insert(0, "Last Progressive Award<br>");
			return html;
		}

		string CalcProgressivePlayerInfoHtml(const CPlayerSession &playerSession)
		{
			string html("Progressive Player Info<br><div style=\"margin-left:6px;\">");
            std::stringstream html_stream; 

			bool sessionActive;
			bool cardedPlayer;
			int sessionId;
			playerSession.GetSessionActiveInfo(sessionActive, cardedPlayer, sessionId);
			html += FormatString("Session Active: %s<br>",  CUnitTestingUtils::GetBoolString(sessionActive));
			html += FormatString("Carded Player: %s<br>", cardedPlayer? "true": "false");
			html += FormatString("Session ID: %d<br>", (int)sessionId);
			html += FormatString("Last Session ID Increment Time: %s<br>",
				CUtilities::FormatMilitaryDateTimeString(playerSession.GetLastSessionIdIncrementTime()).c_str());
			html += FormatString("Session ID Match Status: %s<br>",
				CUnitTestingUtils::GetBoolString(playerSession.GetSessionIdMatchStatus()));

			html += "</div><br>";
			return html;
		}

		LPCTSTR GetProgressiveLevelTypeString(ProgressiveLevelType::Type type) const
		{
			LPCTSTR pTypeStr;
			switch (type)
			{
				case ProgressiveLevelType::Standard:
					pTypeStr = "Standard";
					break;

				case ProgressiveLevelType::Bonus:
					pTypeStr = "Bonus";
					break;

				case ProgressiveLevelType::AccountingOnly:
					pTypeStr = "Accounting Only";
					break;

				case ProgressiveLevelType::PalcomX:
					pTypeStr = "PalcomX";
					break;

				default:
					pTypeStr = "Invalid";
					break;
			}
			return pTypeStr;
		}

		LPCTSTR GetShowOdometerString(bool showOdometer, bool isPTE) const
		{
			const LPCTSTR ShowNoOdometer = "false";
			const LPCTSTR ShowStandardOdometer = "true";
			const LPCTSTR ShowPteOdometer = "PTE Progress";

			LPCTSTR showOdometerStr = ShowNoOdometer;

			if (showOdometer)
			{
				if (isPTE)
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

		/// <summary>
		/// Web content simulator for the configuration extensions info.
		/// Compare actual results with simulator results.
		/// </summary>
		/// <param name="tickCount">IN - The current tick count.</param>
		/// <param name="localTime">IN - The current LOCAL time.</param>
		/// <param name="pLevel">IN - Progressive level to be dumped.</param>
		/// <param name="localTime">IN - The LOCAL time.</param>
		/// <returns>A string with the expected content for the progressive data.</returns>
		string CalcProgressiveLevelHtml(DWORD tickCount, const SYSTEMTIME& localTime, const CProgressiveLevel* pLevel)
		{
			string html;

			if (pLevel)
			{
				double updatedSeconds = (double) (tickCount - pLevel->m_updatedTickCount) / MILLISECONDS_PER_SECOND;

				html += FormatString("<tr><td>%lld</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td>",
					(long long int)pLevel->m_id,
					pLevel->m_name.c_str(),
					CUtilities::GetFormattedCashString(pLevel->m_value).c_str(),
					GetProgressiveLevelTypeString(pLevel->m_type),
					CWebDiagnostics::GetBoolString(pLevel->IsEligible()),
					GetShowOdometerString(pLevel->ShowOdometer(), pLevel->IsPte())
					);

				if (pLevel->m_state == CProgressiveLevel::Stale)
				{
					html += FormatString("<td><font color=\"red\">%0.1f</font></td>", updatedSeconds);
				}
				else
				{
					html += FormatString("<td>%0.1f</td>", updatedSeconds);
				}

				CountHistory::Data today;
				CountHistory::Data yesterday;
				pLevel->m_staleCount.GetDataNoRoll(&today, &yesterday);

                time_t retVal = time(0);
                SYSTEMTIME utcTime= pLevel->m_staleCount.GetLastSetTime();
                if (utcTime.wYear >= MINCTIMEYEAR && utcTime.wYear <= MAXCTIMEYEAR)
                {
                    retVal = CUtilities::GetTimeFromFields(utcTime.wYear, utcTime.wMonth, utcTime.wDay, utcTime.wHour, utcTime.wMinute, utcTime.wSecond);
                }

				if (pLevel->m_staleCount.IsLastSetTimeValid())
				{
					string timeStr = CUtilities::FormatMilitaryDateTimeString(retVal);
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
				if (!pLevel->m_sasLevels.empty())
				{
					html += FormatString("%d", pLevel->m_sasLevels[0]);

					for (size_t i = 1; i < pLevel->m_sasLevels.size(); i++)
						html += FormatString(", %d", pLevel->m_sasLevels[i]);
				}
				html += "</td>";

				html += FormatString("<td>%s</td>", pLevel->m_url.c_str());

				html += "</tr>";
			}
			return html;
		}

		string CalcProgressiveDownInfoHtml(const SYSTEMTIME& localTime, const ProgressiveLevelManager& levelManager)
		{
			string html("Progressive Down Alert Info:<br><div style=\"margin-left:6px;\">");

			CountHistory::Data today;
			CountHistory::Data yesterday;
			levelManager.m_downCount.GetDataNoRoll(&today, &yesterday, &localTime);

            html += FormatString("Is Down: %s<br>", CUnitTestingUtils::GetBoolString(today.IsSet()));
			if (levelManager.m_downCount.IsLastSetTimeValid())
			{
				html += FormatString("Last Down Time: %s<br>",
					CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(levelManager.m_downCount.GetLastSetTime()).c_str());

				if (levelManager.m_downCount.IsLastResetTimeValid())
				{
					html += FormatString("Last Up Time: %s<br>",
						CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(levelManager.m_downCount.GetLastResetTime()).c_str());
				}
				else
				{
					html += "Last Up Time:<br>";
				}
			}
			else
			{
				html += "Last Down Time:<br>";
				html += "Last Up Time:<br>";
			}

			html += FormatString("Number of Times Down Today: %u<br>", today.GetCount());
			html += FormatString("Total Milliseconds Down Today: %u<br>", today.GetMilliseconds());

			html += FormatString("Number of Times Down Yesterday: %u<br>", yesterday.GetCount());
			html += FormatString("Total Milliseconds Down Yesterday: %u<br>", yesterday.GetMilliseconds());

			html += "</div><br>";

			return html;
		}

		string CalcLastGameInfo(const CPlayerSession &playerSession)
		{
			string html;

			html += "Last Game Played Information<br>";

			html += FormatString("Game In Progress: %s<br>",
				CUnitTestingUtils::GetBoolString(playerSession.GetGameInProgress()));
			html += FormatString("Last Game Start: %s<br>",
				CUnitTestingUtils::FormatLocalMilitaryDateTimeString(playerSession.GetLastGameStarted()).c_str());
			html += FormatString("Last Game End: %s<br>",
				CUnitTestingUtils::FormatLocalMilitaryDateTimeString(playerSession.GetLastGameEnd()).c_str());

			if (playerSession.GetLastGameNumber() < 0)
			{
				html += FormatString("Last Game Number: %s<br>", CUnitTestingUtils::LPCTSTR_NA);
			}
			else
			{
				html += FormatString("Last Game Number: %d<br>", playerSession.GetLastGameNumber());
			}

			if (playerSession.GetLastCoinsPlayed() < 0)
			{
				html += FormatString("Last Coins Played: %s<br>", CUnitTestingUtils::LPCTSTR_NA);
			}
			else
			{
				html += FormatString("Last Coins Played: %ld<br>", playerSession.GetLastCoinsPlayed());
			}

			if (playerSession.GetLastDenom() < 0)
			{
				html += FormatString("Last Denomination: %s<br>", CUnitTestingUtils::LPCTSTR_NA);
			}
			else
			{
				html += FormatString("Last Denomination: %d<br>", playerSession.GetLastDenom());
			}

			html += FormatString("EGM Sends Game End: %s<br>",
				CUnitTestingUtils::GetBoolString(playerSession.GetEgmSendsGameEnd()));

			html += "<br>";
			return html;
		}

		string InsertLastGameInfo(const string &html,
			const CPlayerSession &playerSession)
		{
			string lastGameInfo(CalcLastGameInfo(playerSession));
			string updatedHtml(html);
			updatedHtml.insert(updatedHtml.length() - _tcslen("</html>"), lastGameInfo);
			return updatedHtml;
		}

		/// <summary>
		/// Web content simulator for the configuration extensions info.
		/// Compare actual results with simulator results.
		/// </summary>
		/// <param name="tickCount">IN - The current tick count.</param>
		/// <param name="pLevels">IN - Progressive levels to be dumped.</param>
		/// <returns>A string with the expected content for the progressive data.</returns>
		string CalcProgressiveLevelsHtml(DWORD tickCount, const SYSTEMTIME& localTime, const ProgressiveLevelManager* pLevels)
		{
			string html;
            std::stringstream strString; 

			if (pLevels)
			{
				html += "Progressive Level Info:<br><div style=\"margin-left:6px;\">";
				if (pLevels->m_progressiveGroupId == 0 && pLevels->HasLevelType(ProgressiveLevelType::Standard))
				{
					html += FormatString("<font color=\"red\">Group ID: %d -- WARNING: This must be non-zero for standard SAS progressives!</font><br>", pLevels->m_progressiveGroupId);
				}
				else
				{
					html += FormatString("Group ID: %d<br>", pLevels->m_progressiveGroupId);
				}
				html += FormatString("Updated Time: %s<br>",
					CUnitTestingUtils::FormatLocalMilitaryDateTimeString(pLevels->m_receivedTime).c_str());
				html += FormatString("Last Eligibility Received Time: %s<br>",
					CUnitTestingUtils::FormatLocalMilitaryDateTimeString(pLevels->m_eligibilityReceivedTime).c_str());
				html += FormatString("Eligibility Determined: %s<br>", CUnitTestingUtils::GetBoolString(pLevels->m_eligibilityDetermined));
				html += FormatString("Eligibility Session ID: %d<br>", pLevels->m_eligibilitySessionId);
				html += FormatString("Query Cards: %s<br>",
					CUnitTestingUtils::GetBoolString(pLevels->QueryPokerCards()));

				if (pLevels->m_levels.size() != 0)
				{
					html += "Levels:<br>";
					html += "<style>table{border-width:1px;border-style:solid;border-collapse:collapse;}"
						"th,td{border-width:1px;border-style:solid;padding:3px 7px 2px 7px;}</style>"
						"<table>"
						"<tr><th>ID</th><th>Name</th><th>Value</th><th>Type</th><th>Eligible</th><th>Show Odometer</th><th>Last Updated (seconds)</th>"
						"<th>Last Stale Time</th><th>Stale Count Today</th><th>Stale Count Yesterday</th><th>SAS IDs</th><th>URL</th></tr>";

					for (const auto& level : pLevels->m_levels)
						{
						html += CalcProgressiveLevelHtml(tickCount, localTime, level.second.get());
					}

					html += "</table>";
				}

				html += "</div><br>";
			}
			return html;
		}

		string CalcProgressiveLevelsHtml(DWORD tickCount, const SYSTEMTIME& localTime, const ProgressiveLevelManager& levelManager)
		{
			return CalcProgressiveLevelsHtml(tickCount, localTime, &levelManager);
		}

		string InsertProgressiveDataHtml(const string &html, DWORD tickCount, const SYSTEMTIME& localTime, const ProgressiveLevelManager* pLevels)
		{
			string progressiveDataHtml(CalcProgressiveLevelsHtml(tickCount, localTime, pLevels));
			string updatedHtml(html);
            updatedHtml.insert(updatedHtml.length() - _tcslen("</html>"), progressiveDataHtml);
			return updatedHtml;
		}

		string InsertProgressiveDataHtml(const string &html,
			const CPlayerSession &playerSession,
			DWORD tickCount,
			const SYSTEMTIME& localTime,
			const CEGMAwardManagerState& state,
			const ProgressiveLevelManager& levelManager,
			const CEGMAward &progAward,
			const std::shared_ptr<CEGMAward> &lastStandardWin,
			const std::vector<std::shared_ptr<CEGMAward>> &standardWins)
		{
			string progressiveDataHtml(CalcProgressivePlayerInfoHtml(playerSession));
			progressiveDataHtml += CalcProgressiveLevelsHtml(tickCount, localTime, levelManager);
			progressiveDataHtml += CalcProgressiveDownInfoHtml(localTime, levelManager);

			progressiveDataHtml += "Last Bonus Award<br>";
			progressiveDataHtml += CalcBonusAwardHtml(tickCount, progAward, state);

			progressiveDataHtml += "Previous Standard SAS Progressive Win<br>";
			progressiveDataHtml += CalcStandardProgressiveWinHtml(lastStandardWin);

			progressiveDataHtml += "Current Standard SAS Progressive Win(s)<br>";
			progressiveDataHtml += CalcStandardProgressiveWinsHtml(standardWins);

			string updatedHtml(html);
			updatedHtml.insert(updatedHtml.length() - _tcslen("</html>"), progressiveDataHtml);
			return updatedHtml;
		}

        	string PrepareProgressiveDataHtml(
			const CPlayerSession &playerSession,
			DWORD tickCount,
			const SYSTEMTIME& localTime,
			const CEGMAwardManagerState& state,
			const ProgressiveLevelManager& levelManager,
			const CEGMAward &progAward,
			const std::shared_ptr<CEGMAward> &lastStandardWin,
			const std::vector<std::shared_ptr<CEGMAward>> &standardWins)
		{
			string progressiveDataHtml(CalcProgressivePlayerInfoHtml(playerSession));
			progressiveDataHtml += CalcProgressiveLevelsHtml(tickCount, localTime, levelManager);
			progressiveDataHtml += CalcProgressiveDownInfoHtml(localTime, levelManager);

			progressiveDataHtml += "Last Bonus Award<br>";
			progressiveDataHtml += CalcBonusAwardHtml(tickCount, progAward, state);

			progressiveDataHtml += "Previous Standard SAS Progressive Win<br>";
			progressiveDataHtml += CalcStandardProgressiveWinHtml(lastStandardWin);

			progressiveDataHtml += "Current Standard SAS Progressive Win(s)<br>";
			progressiveDataHtml += CalcStandardProgressiveWinsHtml(standardWins);

			return progressiveDataHtml;
		}

        string PrepareProgressiveDataHtml(DWORD tickCount, const SYSTEMTIME& localTime, const ProgressiveLevelManager* pLevels)
		{
			string progressiveDataHtml(CalcProgressiveLevelsHtml(tickCount, localTime, pLevels));
			return progressiveDataHtml;
		}


        void f_GetProgressiveConfigHtml_DefaultTest( void )
        {
            // Setup.
			const CProgressiveConfig progConfig;

			// Expected results.
			const string htmlExpected(
				"Configuration File Present: false<br>"
				"Progressive Configuration: Disabled<br>"
				"DNS Lookup: true<br>"
				"Ping Interval(ms): 15000<br>"
				"<br>");

			// Setup.
			const int testIndex(0);

			// Test(s).
			AssertResult2(CWebDiagnostics::GetProgressiveConfigHtml(progConfig), htmlExpected);

        }

        void f_GetProgressiveConfigHtmlTest(void)
        {
            // Input(s).
            LPCTSTR endpoint1("abcde.xyz:12345");
            LPCTSTR endpoint2("123.234.56.78:23456");

            typedef struct
            {
                // Input(s).
                const bool m_hasConfigFile;
                const bool m_active;
                const bool m_dnsLookup;
                const DWORD m_pingInterval;

                // Expected result(s).
                const string m_htmlExpected;
            } TestDataType;

            const TestDataType TEST_DATA[] =
            {
                { false, false, false, 0,
                    "Configuration File Present: false<br>"
                    "Progressive Configuration: Disabled<br>"
                    "DNS Lookup: false<br>"
                    "Ping Interval(ms): 15000<br>"
                    "<br>"
                },

                { false, false, true, 6000,
                    "Configuration File Present: false<br>"
                    "Progressive Configuration: Disabled<br>"
                    "DNS Lookup: true<br>"
                    "Ping Interval(ms): 6000<br>"
                    "<br>"
                },

                { false, true, false, 1234567890,
                    "Configuration File Present: false<br>"
                    "Progressive Configuration: Enabled<br>"
                    "DNS Lookup: false<br>"
                    "Ping Interval(ms): 1234567890<br>"
                    "<br>"
                },

                { false, true, true, 4294967295,
                    "Configuration File Present: false<br>"
                    "Progressive Configuration: Enabled<br>"
                    "DNS Lookup: true<br>"
                    "Ping Interval(ms): 4294967295<br>"
                    "<br>"
                },

                { true, false, false, 0,
                "Configuration File Present: true<br>"
                "Progressive Configuration: Disabled<br>"
                "DNS Lookup: false<br>"
                "Ping Interval(ms): 15000<br>"
                "<br>"
                },

                { true, false, true, 6000,
                "Configuration File Present: true<br>"
                "Progressive Configuration: Disabled<br>"
                "DNS Lookup: true<br>"
                "Ping Interval(ms): 6000<br>"
                "<br>"
                },

                { true, true, false, 1234567890,
                "Configuration File Present: true<br>"
                "Progressive Configuration: Enabled<br>"
                "DNS Lookup: false<br>"
                "Ping Interval(ms): 1234567890<br>"
                "<br>"
                },

                { true, true, true, 4294967295,
                "Configuration File Present: true<br>"
                "Progressive Configuration: Enabled<br>"
                "DNS Lookup: true<br>"
                "Ping Interval(ms): 4294967295<br>"
                "<br>"
                },
            };

            for (int testIndex(0); testIndex < _countof(TEST_DATA); ++testIndex)
            {
                // Setup.
                CProgressiveConfig varProgConfig;
                varProgConfig.SetHasConfigFile(TEST_DATA[testIndex].m_hasConfigFile);
                varProgConfig.SetActive(TEST_DATA[testIndex].m_active);
                varProgConfig.SetDNSLookup(TEST_DATA[testIndex].m_dnsLookup);
                varProgConfig.SetPingInterval(TEST_DATA[testIndex].m_pingInterval);
                varProgConfig.AddEndpoint(endpoint1);
                varProgConfig.AddEndpoint(endpoint2);
                const CProgressiveConfig& progConfig(varProgConfig);

                // Test(s).
                AssertResult2(CWebDiagnostics::GetProgressiveConfigHtml(progConfig), TEST_DATA[testIndex].m_htmlExpected);
            }
        }

        void f_GetProgressiveConnectionInfoHtml_Default0Test(void)
        {
            // Input(s).
            // Expected results.
            const string htmlExpected(
                "Progressive Connection Info<br>"
                "DNS Hostname: <br>"
                "Current Host IP: N/A<br>"
                "Connection State: Not Started<br>"
                "Last Message IP: N/A<br>"
                "Last Message Time: N/A<br>" // Local time.
                "Failed One Link Call Count: 0<br>"
				"Last failed One Link Call Timestamp: N/A<br>"
                "Last failed Multicast Timestamp: N/A<br>"
                "Failed Multicast Count: 0<br>"
                "<br>");

            // Setup.
            const int testIndex(0);
            const CProgressiveConfig progConfig;
            const CProgressiveConnectionInfo progInfo;
            const CProgressiveHost progHost("",333);

            // Test(s).
            AssertResult2(CWebDiagnostics::GetProgressiveConnectionInfoHtml(progConfig, progInfo, progHost), htmlExpected);
        }
        
        void f_GetProgressiveConnectionInfoHtml_Default1Test(void)
        {
            // Expected results.
            const string htmlExpected(
                "Progressive Connection Info<br>"
                "DNS Hostname: <br>"
                "Current Host IP: N/A<br>"
                "Connection State: Not Started<br>"
                "Last Message IP: N/A<br>"
                "Last Message Time: N/A<br>" // Local time.
                "Failed One Link Call Count: 0<br>"
				"Last failed One Link Call Timestamp: N/A<br>"
                "Last failed Multicast Timestamp: N/A<br>"
                "Failed Multicast Count: 0<br>"
                "<br>");

            // Setup.
            const int testIndex(0);
            const CProgressiveConfig progConfig;
            const CProgressiveConnectionInfo progInfo;
            CIPAddressArray ipAddressArray;
            const CProgressiveHost progHost(ipAddressArray);

            // Test(s).
            AssertResult2(CWebDiagnostics::GetProgressiveConnectionInfoHtml(progConfig, progInfo, progHost), htmlExpected);
        }


        void f_GetProgressiveConnectionInfoHtml_Default2Test(void)
        {
            // Expected results.
            const string htmlExpected(
                "Progressive Connection Info<br>"
                "DNS Hostname: <br>"
                "Current Host IP: 1.2.3.4<br>"
                "Connection State: Not Started<br>"
                "Last Message IP: N/A<br>"
                "Last Message Time: N/A<br>" // Local time.
                "Failed One Link Call Count: 0<br>"
				"Last failed One Link Call Timestamp: N/A<br>"
                "Last failed Multicast Timestamp: N/A<br>"
                "Failed Multicast Count: 0<br>"
                "<br>");

            // Setup.
            const int testIndex(0);
            const CProgressiveConfig progConfig;
            const CProgressiveConnectionInfo progInfo;
            CIPAddressArray ipAddressArray;
            ipAddressArray.Add("1.2.3.4");
            ipAddressArray.Add("1.2.3.4");
            const CProgressiveHost progHost(ipAddressArray);

            // Test(s).
            AssertResult2(CWebDiagnostics::GetProgressiveConnectionInfoHtml(progConfig, progInfo, progHost), htmlExpected);
        }

        void f_GetProgressiveConnectionInfoHtmlTest(void)
            {
                // Input(s).
                const int utilitiesTimeZoneOffset(0);
                LPCTSTR const currentHostIP("11.22.33.44");

                typedef struct {
                    LPCTSTR const m_ipAddressString;
                    int m_port;
                } TIPAddressStringPort;

                typedef struct
                {
                    // Input(s) for CProgressiveConfig.
                    const bool m_hasConfigFile;
                    const bool m_active;
                    const bool m_dnsLookup;
                    const DWORD m_pingInterval;
                    const LPCTSTR* const m_endpoints;
                    const int m_endpointCount;

                    // Input(s) for CProgressiveConnectionInfo.
                    const TIPAddressStringPort m_lastMessageIP;
                    const SYSTEMTIME m_lastMessageTimestamp;
                    const ProgressiveConnectionState m_state;
                    const ProgressiveConnectionError m_error;
                    const int DisconnectCount;

                    // Input(s) for CProgressiveHost.
                    LPCTSTR const m_hostName;
                    const int m_hostPort;

                    // Expected result(s).
                    const string m_htmlExpected;
                } TestDataType;

                // Input(s).
                LPCTSTR endpoints1[] = {
                    "abcde.xyz:12345",
                };
                LPCTSTR endpoints2[] = {
                    "abcde.xyz:12345",
                    "   abcdefghijk.lmnop.xyz:1234567890   ",
                };
                LPCTSTR endpoints3[] = {
                    "abcde.xyz:12345",
                    "abcdefghijk.lmnop.xyz:1234567890",
                    " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLM NOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~:123456789012345678901234567890 ",
                };

                const TestDataType TEST_DATA[] =
                {
                    { false, false, false, 0, NULL, 0,
					{ _T("101.104.105.0"), 10000 }, { 2011, 2, 0, 3, 14, 25, 0, 789 }, (ProgressiveConnectionState)-1, CouldNotFindServer,0,
					_T("ab"), 2,
					_T("Progressive Connection Info<br>")
					_T("DNS Hostname: N/A<br>")
					_T("Current Host IP: N/A<br>")
					_T("Connection State: N/A<br>")
					_T("Last Message IP: 101.104.105.0<br>")
					_T("Last Message Time: 02/03/2011 14:25:00<br>")
					_T("Failed One Link Call Count: 0<br>")
					_T("Last failed One Link Call Timestamp: N/A<br>") // Local time.
                    _T("Last failed Multicast Timestamp: N/A<br>")
                    _T("Failed Multicast Count: 0<br>")
					_T("<br>")
				},
				{ false, false, true, 6000, endpoints1, _countof(endpoints1),
				{ _T("101.104.105.1"), 10001 }, { 2011, 2, 0, 3, 14, 25, 1, 789 }, (ProgressiveConnectionState)(NotStarted - 1), CouldNotFindServer,0,
				_T("abc"), 3,
				_T("Progressive Connection Info<br>")
				_T("DNS Hostname: abc<br>")
				_T("Current Host IP: N/A<br>")
				_T("Connection State: N/A<br>")
				_T("Last Message IP: 101.104.105.1<br>")
				_T("Last Message Time: 02/03/2011 14:25:01<br>")
				_T("Failed One Link Call Count: 0<br>")
				_T("Last failed One Link Call Timestamp: N/A<br>") // Local time.
                _T("Last failed Multicast Timestamp: N/A<br>")
                _T("Failed Multicast Count: 0<br>")
				_T("<br>")
				},
				{ false, true, false, 1234567890, endpoints2, _countof(endpoints2),
				{ _T("101.104.105.2"), 10002 }, { 2011, 2, 0, 3, 14, 25, 2, 789 }, NotStarted, CouldNotFindServer,0,
				_T("abcd"), 4,
				_T("Progressive Connection Info<br>")
				_T("DNS Hostname: N/A<br>")
				_T("Current Host IP: N/A<br>")
				_T("Connection State: Not Started<br>")
				_T("Last Message IP: 101.104.105.2<br>")
				_T("Last Message Time: 02/03/2011 14:25:02<br>")
				_T("Failed One Link Call Count: 0<br>")
				_T("Last failed One Link Call Timestamp: N/A<br>") // Local time.
                _T("Last failed Multicast Timestamp: N/A<br>")
                _T("Failed Multicast Count: 0<br>")
				_T("<br>")
				},
				{ false, true, true, 4294967295, endpoints3, _countof(endpoints3),
				{ _T("101.104.105.3"), 10003 }, { 2011, 2, 0, 3, 14, 25, 3, 789 }, Disabled, CouldNotFindServer,0,
				_T(""), 5,
				_T("Progressive Connection Info<br>")
				_T("DNS Hostname: <br>")
				_T("Current Host IP: 11.22.33.44<br>")
				_T("Connection State: Disabled<br>")
				_T("Last Message IP: 101.104.105.3<br>")
				_T("Last Message Time: 02/03/2011 14:25:03<br>")
				_T("Failed One Link Call Count: 0<br>")
				_T("Last failed One Link Call Timestamp: N/A<br>") // Local time.
                _T("Last failed Multicast Timestamp: N/A<br>")
                _T("Failed Multicast Count: 0<br>")
				_T("<br>") },
				{ true, false, false, 0, NULL, 0,
				{ _T("101.104.105.4"), 10004 }, { 2011, 2, 0, 3, 14, 25, 4, 789 }, WaitingForInfo, CouldNotFindServer,0,
				_T("ABCDE"), 5,
				_T("Progressive Connection Info<br>")
				_T("DNS Hostname: N/A<br>")
				_T("Current Host IP: N/A<br>")
				_T("Connection State: Waiting For Info<br>")
				_T("Last Message IP: 101.104.105.4<br>")
				_T("Last Message Time: 02/03/2011 14:25:04<br>")
				_T("Failed One Link Call Count: 0<br>")
				_T("Last failed One Link Call Timestamp: N/A<br>") // Local time.
                _T("Last failed Multicast Timestamp: N/A<br>")
                _T("Failed Multicast Count: 0<br>")
				_T("<br>") },
				{ true, false, true, 6000, endpoints1, _countof(endpoints1),
				{ _T("101.104.105.5"), 10005 }, { 2011, 2, 0, 3, 14, 25, 5, 789 }, InDiscovery, CouldNotFindServer,0,
				_T("abcdef"), 6,
				_T("Progressive Connection Info<br>")
				_T("DNS Hostname: abcdef<br>")
				_T("Current Host IP: N/A<br>")
				_T("Connection State: In Discovery<br>")
				_T("Last Message IP: 101.104.105.5<br>")
				_T("Last Message Time: 02/03/2011 14:25:05<br>")
				_T("Failed One Link Call Count: 0<br>")
				_T("Last failed One Link Call Timestamp: N/A<br>") // Local time.
                _T("Last failed Multicast Timestamp: N/A<br>")
                _T("Failed Multicast Count: 0<br>")
				_T("<br>") },
				{ true, true, false, 1234567890, endpoints2, _countof(endpoints2),
				{ _T("101.104.105.6"), 10006 }, { 2011, 2, 0, 3, 14, 25, 6, 789 }, Connected, CouldNotFindServer,0,
				_T("abcdefg"), 7,
				_T("Progressive Connection Info<br>")
				_T("DNS Hostname: N/A<br>")
				_T("Current Host IP: N/A<br>")
				_T("Connection State: Connected<br>")
				_T("Last Message IP: 101.104.105.6<br>")
				_T("Last Message Time: 02/03/2011 14:25:06<br>")
				_T("Failed One Link Call Count: 0<br>")
				_T("Last failed One Link Call Timestamp: N/A<br>") // Local time.
                _T("Last failed Multicast Timestamp: N/A<br>")
                _T("Failed Multicast Count: 0<br>")
				_T("<br>") },
				{ true, true, true, 4294967295, endpoints3, _countof(endpoints3),
				{ _T("101.104.105.7"), 10007 }, { 2011, 2, 0, 3, 14, 25, 7, 789 }, Disconnected, CouldNotFindServer,0,
				_T("abcdefgh"), 8,
				_T("Progressive Connection Info<br>")
				_T("DNS Hostname: abcdefgh<br>")
				_T("Current Host IP: N/A<br>")
				_T("Connection State: Disconnected<br>")
				_T("Last Message IP: 101.104.105.7<br>")
				_T("Last Message Time: 02/03/2011 14:25:07<br>")
				_T("Failed One Link Call Count: 0<br>")
				_T("Last failed One Link Call Timestamp: N/A<br>") // Local time.
                _T("Last failed Multicast Timestamp: N/A<br>")
                _T("Failed Multicast Count: 0<br>")
				_T("<br>") },

				{ false, false, false, 0, NULL, 0,
				{ _T("101.104.105.8"), 10008 }, { 2011, 2, 0, 3, 14, 25, 8, 789 }, Error, (ProgressiveConnectionError)-1,0,
				_T("abcdefghi"), 9,
				_T("Progressive Connection Info<br>")
				_T("DNS Hostname: N/A<br>")
				_T("Current Host IP: N/A<br>")
				_T("Connection State: Error<br>")
				_T("Last Message IP: 101.104.105.8<br>")
				_T("Last Message Time: 02/03/2011 14:25:08<br>")
				_T("Failed One Link Call Count: 0<br>")
				_T("Last failed One Link Call Timestamp: N/A<br>") // Local time.
                _T("Last failed Multicast Timestamp: N/A<br>")
                _T("Failed Multicast Count: 0<br>")
				_T("<br>") },
				{ false, false, true, 6000, endpoints1, _countof(endpoints1),
				{ _T("101.104.105.9"), 10009 }, { 2011, 2, 0, 3, 14, 25, 9, 789 }, Error, (ProgressiveConnectionError)(ProgressiveConnectionErrorNone - 1),0,
				_T("abcdefghij"), 10,
				_T("Progressive Connection Info<br>")
				_T("DNS Hostname: abcdefghij<br>")
				_T("Current Host IP: N/A<br>")
				_T("Connection State: Error<br>")
				_T("Last Message IP: 101.104.105.9<br>")
				_T("Last Message Time: 02/03/2011 14:25:09<br>")
				_T("Failed One Link Call Count: 0<br>")
				_T("Last failed One Link Call Timestamp: N/A<br>") // Local time.
                _T("Last failed Multicast Timestamp: N/A<br>")
                _T("Failed Multicast Count: 0<br>")
				_T("<br>") },
				{ false, true, false, 1234567890, endpoints2, _countof(endpoints2),
				{ _T("101.104.105.10"), 10010 }, { 2011, 2, 0, 3, 14, 25, 10, 789 }, Error, ProgressiveConnectionErrorNone,0,
				_T("abcdefghijk"), 11,
				_T("Progressive Connection Info<br>")
				_T("DNS Hostname: N/A<br>")
				_T("Current Host IP: N/A<br>")
				_T("Connection State: Error<br>")
				_T("Last Message IP: 101.104.105.10<br>")
				_T("Last Message Time: 02/03/2011 14:25:10<br>")
				_T("Failed One Link Call Count: 0<br>")
				_T("Last failed One Link Call Timestamp: N/A<br>") // Local time.
                _T("Last failed Multicast Timestamp: N/A<br>")
                _T("Failed Multicast Count: 0<br>")
				_T("<br>") },
				{ false, true, true, 4294967295, endpoints3, _countof(endpoints3),
				{ _T("101.104.105.11"), 10011 }, { 2011, 2, 0, 3, 14, 25, 11, 789 }, Error, DiscoveryConnectionError,1,
				_T("abcdefghijkL"), 12,
				_T("Progressive Connection Info<br>")
				_T("DNS Hostname: abcdefghijkL<br>")
				_T("Current Host IP: N/A<br>")
				_T("Connection State: Error: Discovery Connection<br>")
				_T("Last Message IP: 101.104.105.11<br>")
				_T("Last Message Time: 02/03/2011 14:25:11<br>")
				_T("Failed One Link Call Count: 1<br>")
				_T("Last failed One Link Call Timestamp: 02/03/2011 14:25:11<br>") // Local time.
                _T("Last failed Multicast Timestamp: N/A<br>")
                _T("Failed Multicast Count: 0<br>")
				_T("<br>") },
				{ true, false, false, 0, NULL, 0,
				{ _T("101.104.105.12"), 10012 }, { 2011, 2, 0, 3, 14, 25, 12, 789 }, Error, MessageTransmissionError,1,
				_T("abcdefghijkLm"), 13,
				_T("Progressive Connection Info<br>")
				_T("DNS Hostname: N/A<br>")
				_T("Current Host IP: N/A<br>")
				_T("Connection State: Error: Message Transmission<br>")
				_T("Last Message IP: 101.104.105.12<br>")
				_T("Last Message Time: 02/03/2011 14:25:12<br>")
				_T("Failed One Link Call Count: 1<br>")
				_T("Last failed One Link Call Timestamp: 02/03/2011 14:25:12<br>") // Local time.
                _T("Last failed Multicast Timestamp: N/A<br>")
                _T("Failed Multicast Count: 0<br>")
				_T("<br>") },
				{ true, false, true, 6000, endpoints1, _countof(endpoints1),
				{ _T("101.104.105.13"), 10013 }, { 2011, 2, 0, 3, 14, 25, 13, 789 }, Error, ConnectFailed,1,
				_T("abcdefghijkLmn"), 14,
				_T("Progressive Connection Info<br>")
				_T("DNS Hostname: abcdefghijkLmn<br>")
				_T("Current Host IP: N/A<br>")
				_T("Connection State: Error: Connect Failed<br>")
				_T("Last Message IP: 101.104.105.13<br>")
				_T("Last Message Time: 02/03/2011 14:25:13<br>")
				_T("Failed One Link Call Count: 1<br>")
				_T("Last failed One Link Call Timestamp: 02/03/2011 14:25:13<br>") // Local time.
                _T("Last failed Multicast Timestamp: N/A<br>")
                _T("Failed Multicast Count: 0<br>")
				_T("<br>") },
				{ true, true, false, 1234567890, endpoints2, _countof(endpoints2),
				{ _T("101.104.105.14"), 10014 }, { 2011, 2, 0, 3, 14, 25, 14, 789 }, Error, DisconnectFailed,1,
				_T(""), 15,
				_T("Progressive Connection Info<br>")
				_T("DNS Hostname: N/A<br>")
				_T("Current Host IP: 11.22.33.44<br>")
				_T("Connection State: Error: Disconnect Failed<br>")
				_T("Last Message IP: 101.104.105.14<br>")
				_T("Last Message Time: 02/03/2011 14:25:14<br>")
				_T("Failed One Link Call Count: 1<br>")
				_T("Last failed One Link Call Timestamp: 02/03/2011 14:25:14<br>") // Local time.
                _T("Last failed Multicast Timestamp: N/A<br>")
                _T("Failed Multicast Count: 0<br>")
				_T("<br>") },
				{ true, true, true, 4294967295, endpoints3, _countof(endpoints3),
				{ _T("101.104.105.15"), 10015 }, { 2011, 2, 0, 3, 14, 25, 15, 789 }, Error, (ProgressiveConnectionError)(DisconnectFailed + 1),0,
				_T("abcdefghijkLmnop"), 16,
				_T("Progressive Connection Info<br>")
				_T("DNS Hostname: abcdefghijkLmnop<br>")
				_T("Current Host IP: N/A<br>")
				_T("Connection State: Error<br>")
				_T("Last Message IP: 101.104.105.15<br>")
				_T("Last Message Time: 02/03/2011 14:25:15<br>")
				_T("Failed One Link Call Count: 0<br>")
				_T("Last failed One Link Call Timestamp: N/A<br>") // Local time.
                _T("Last failed Multicast Timestamp: N/A<br>")
                _T("Failed Multicast Count: 0<br>")
				_T("<br>") },
				{ false, false, false, 0, NULL, 0,
				{ _T("101.104.105.16"), 10016 }, { 2011, 2, 0, 3, 14, 25, 16, 789 }, Error, (ProgressiveConnectionError)INT_MAX,0,
				_T("abcdefghijkLmnopq"), 17,
				_T("Progressive Connection Info<br>")
				_T("DNS Hostname: N/A<br>")
				_T("Current Host IP: N/A<br>")
				_T("Connection State: Error<br>")
				_T("Last Message IP: 101.104.105.16<br>")
				_T("Last Message Time: 02/03/2011 14:25:16<br>")
				_T("Failed One Link Call Count: 0<br>")
				_T("Last failed One Link Call Timestamp: N/A<br>") // Local time.
                _T("Last failed Multicast Timestamp: N/A<br>")
                _T("Failed Multicast Count: 0<br>")
				_T("<br>") },
				{ false, false, true, 6000, endpoints1, _countof(endpoints1),
				{ _T("101.104.105.17"), 10017 }, { 2011, 2, 0, 3, 14, 25, 17, 789 }, Error, (ProgressiveConnectionError)DWORD_MAX,0,
				_T("abcdefghijkLmnopqr"), 18,
				_T("Progressive Connection Info<br>")
				_T("DNS Hostname: abcdefghijkLmnopqr<br>")
				_T("Current Host IP: N/A<br>")
				_T("Connection State: Error<br>")
				_T("Last Message IP: 101.104.105.17<br>")
				_T("Last Message Time: 02/03/2011 14:25:17<br>")
				_T("Failed One Link Call Count: 0<br>")
				_T("Last failed One Link Call Timestamp: N/A<br>") // Local time.
                _T("Last failed Multicast Timestamp: N/A<br>")
                _T("Failed Multicast Count: 0<br>")
				_T("<br>") },

				{ false, true, false, 1234567890, endpoints2, _countof(endpoints2),
				{ _T("101.104.105.18"), 10018 }, { 2011, 2, 0, 3, 14, 25, 18, 789 }, (ProgressiveConnectionState)(Error + 1), CouldNotFindServer,0,
				_T("abcdefghijkLmnopqrs"), 19,
				_T("Progressive Connection Info<br>")
				_T("DNS Hostname: N/A<br>")
				_T("Current Host IP: N/A<br>")
				_T("Connection State: N/A<br>")
				_T("Last Message IP: 101.104.105.18<br>")
				_T("Last Message Time: 02/03/2011 14:25:18<br>")
				_T("Failed One Link Call Count: 0<br>")
				_T("Last failed One Link Call Timestamp: N/A<br>") // Local time.
                _T("Last failed Multicast Timestamp: N/A<br>")
                _T("Failed Multicast Count: 0<br>")
				_T("<br>") },
				{ false, true, true, 4294967295, endpoints3, _countof(endpoints3),
				{ _T("101.104.105.19"), 10019 }, { 2011, 2, 0, 3, 14, 25, 19, 789 }, (ProgressiveConnectionState)INT_MAX, CouldNotFindServer,0,
				_T("abcdefghijkLmnopqrst"), 20,
				_T("Progressive Connection Info<br>")
				_T("DNS Hostname: abcdefghijkLmnopqrst<br>")
				_T("Current Host IP: N/A<br>")
				_T("Connection State: N/A<br>")
				_T("Last Message IP: 101.104.105.19<br>")
				_T("Last Message Time: 02/03/2011 14:25:19<br>")
				_T("Failed One Link Call Count: 0<br>")
				_T("Last failed One Link Call Timestamp: N/A<br>") // Local time.
                _T("Last failed Multicast Timestamp: N/A<br>")
                _T("Failed Multicast Count: 0<br>")
                _T("<br>") },
				{ true, false, false, 0, NULL, 0,
				{ _T("101.104.105.20"), 10020 }, { 2011, 2, 0, 3, 14, 25, 20, 789 }, (ProgressiveConnectionState)INT_MIN, CouldNotFindServer,0,
				_T("abcdefghijkLmnopqrstu"), 21,
				_T("Progressive Connection Info<br>")
				_T("DNS Hostname: N/A<br>")
				_T("Current Host IP: N/A<br>")
				_T("Connection State: N/A<br>")
				_T("Last Message IP: 101.104.105.20<br>")
				_T("Last Message Time: 02/03/2011 14:25:20<br>")
				_T("Failed One Link Call Count: 0<br>")
				_T("Last failed One Link Call Timestamp: N/A<br>") // Local time.
                _T("Last failed Multicast Timestamp: N/A<br>")
                _T("Failed Multicast Count: 0<br>")
				_T("<br>") },
				{ true, false, true, 6000, endpoints1, _countof(endpoints1),
				{ _T("101.104.105.21"), 10021 }, { 2011, 2, 0, 3, 14, 25, 21, 789 }, (ProgressiveConnectionState)DWORD_MAX, CouldNotFindServer,0,
				_T("abcdefghijkLmnopqrstuv"), 22,
				_T("Progressive Connection Info<br>")
				_T("DNS Hostname: abcdefghijkLmnopqrstuv<br>")
				_T("Current Host IP: N/A<br>")
				_T("Connection State: N/A<br>")
				_T("Last Message IP: 101.104.105.21<br>")
				_T("Last Message Time: 02/03/2011 14:25:21<br>")
				_T("Failed One Link Call Count: 0<br>")
				_T("Last failed One Link Call Timestamp: N/A<br>") // Local time.
                _T("Last failed Multicast Timestamp: N/A<br>")
                _T("Failed Multicast Count: 0<br>")
				_T("<br>") },                };

                // Setup. Set the nCompass time zone to match the expected results.
                CUtilities::SetTzOffset(utilitiesTimeZoneOffset);

                for (int testIndex(0); testIndex < _countof(TEST_DATA); ++testIndex)
                {
    				// Setup.
    				CProgressiveConfig varProgConfig;
    				varProgConfig.SetHasConfigFile(TEST_DATA[testIndex].m_hasConfigFile);
    				varProgConfig.SetActive(TEST_DATA[testIndex].m_active);
    				varProgConfig.SetDNSLookup(TEST_DATA[testIndex].m_dnsLookup);
    				varProgConfig.SetPingInterval(TEST_DATA[testIndex].m_pingInterval);
    				for (int index(0); index < TEST_DATA[testIndex].m_endpointCount; ++index)
    				{
    					varProgConfig.AddEndpoint(TEST_DATA[testIndex].m_endpoints[index]);
    				}
    				const CProgressiveConfig& progConfig(varProgConfig);

    				// Perform CProgressiveConnectionInfo operations in different orders on different instances.
    				CProgressiveConnectionInfo varProgInfoA;
    				CProgressiveConnectionInfo varProgInfoB;

    				CIPAddress ipAddress;
    				ipAddress.SetIP(TEST_DATA[testIndex].m_lastMessageIP.m_ipAddressString);
    				ipAddress.SetPort(TEST_DATA[testIndex].m_lastMessageIP.m_port);

    				varProgInfoA.SetLastMessageIP(ipAddress);
    				varProgInfoA.SetLastMessageTimestamp(TEST_DATA[testIndex].m_lastMessageTimestamp);
    				varProgInfoA.SetState(TEST_DATA[testIndex].m_state);
    				varProgInfoA.SetLastError(TEST_DATA[testIndex].m_error);
    				varProgInfoA.SetDisconnectCount(TEST_DATA[testIndex].DisconnectCount);
    				varProgInfoA.SetDisconnectTimeStampSinceBoot(TEST_DATA[testIndex].m_lastMessageTimestamp);

    				varProgInfoB.SetLastError(TEST_DATA[testIndex].m_error);
    				varProgInfoB.SetState(TEST_DATA[testIndex].m_state);
    				varProgInfoB.SetLastMessageTimestamp(TEST_DATA[testIndex].m_lastMessageTimestamp);
    				varProgInfoB.SetLastMessageIP(ipAddress);
    				varProgInfoB.SetDisconnectCount(TEST_DATA[testIndex].DisconnectCount);
    				varProgInfoB.SetDisconnectTimeStampSinceBoot(TEST_DATA[testIndex].m_lastMessageTimestamp);

    				const CProgressiveConnectionInfo& progInfoA(varProgInfoA);
    				const CProgressiveConnectionInfo& progInfoB(varProgInfoB);

    				CProgressiveHost varProgHost(TEST_DATA[testIndex].m_hostName, TEST_DATA[testIndex].m_hostPort);
    				if (0 == _tcscmp(_T(""), TEST_DATA[testIndex].m_hostName))
    				{
    					CIPAddressArray ipAddressArray;
    					ipAddressArray.Add(currentHostIP);
    					varProgHost = CProgressiveHost(ipAddressArray);
    				}
    				const CProgressiveHost& progHost(varProgHost);

    				// Test(s).
    				AssertResult2(CWebDiagnostics::GetProgressiveConnectionInfoHtml(progConfig, progInfoA, progHost),
    					TEST_DATA[testIndex].m_htmlExpected);
    				AssertResult2(CWebDiagnostics::GetProgressiveConnectionInfoHtml(progConfig, progInfoB, progHost),
    					TEST_DATA[testIndex].m_htmlExpected);
            	}
		}

        void f_GetProgressiveHostListHtml_DefaultTest(void)
        {
            // Input(s).

            // Setup.
            const CProgressiveConfig progConfig;

            // Expected results.
            const string htmlExpected(
                "Progressive List<br>"
                "Host: onelinkdiscossl:61614<br>"
                "Host: onelinkdisco:61613<br>"
                "<br>");

            // Setup.
            const int testIndex(0);

            // Test(s).
            AssertResult2(CWebDiagnostics::GetProgressiveHostListHtml(progConfig), htmlExpected);
        }


        void f_GetProgressiveHostListHtmlTest(void)
        {
            typedef struct
            {
                // Input(s).
                const bool m_hasConfigFile;
                const bool m_active;
                const bool m_dnsLookup;
                const DWORD m_pingInterval;
                const LPCTSTR* const m_endpoints;
                const int m_endpointCount;

                // Expected result(s).
                const string m_htmlExpected;
            } TestDataType;

            // Input(s).
            LPCTSTR endpoints1[] = {
                "abcde.xyz:12345",
            };
            LPCTSTR endpoints2[] = {
                "abcde.xyz:12345",
                "   abcdefghijk.lmnop.xyz:1234567890   ",
            };
            LPCTSTR endpoints3[] = {
                "abcde.xyz:12345",
                "abcdefghijk.lmnop.xyz:1234567890",
                " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLM NOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~:123456789012345678901234567890 ",
            };

            const TestDataType TEST_DATA[] =
            {
                { false, false, false, 0, NULL, 0,
                    "Progressive List<br>"
                    "Host: onelinkdiscossl:61614<br>"
                    "Host: onelinkdisco:61613<br>"
                    "<br>"
                },

                { false, false, true, 6000, endpoints1, _countof(endpoints1),
                    "Progressive List<br>"
                    "Host: abcde.xyz:12345<br>"
                    "<br>"
                },

                { false, true, false, 1234567890, endpoints2, _countof(endpoints2),
                    "Progressive List<br>"
                    "Host: abcde.xyz:12345<br>"
                    "Host:    abcdefghijk.lmnop.xyz:1234567890   <br>"
                    "<br>"
                },

                { false, true, true, 4294967295, endpoints3, _countof(endpoints3),
                    "Progressive List<br>"
                    "Host: abcde.xyz:12345<br>"
                    "Host: abcdefghijk.lmnop.xyz:1234567890<br>"
                    "Host:  !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLM NOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~:123456789012345678901234567890 <br>"
                    "<br>"
                },

                { true, false, false, 0, NULL, 0,
                "Progressive List<br>"
                "Host: onelinkdiscossl:61614<br>"
                "Host: onelinkdisco:61613<br>"
                "<br>"
                },

                { true, false, true, 6000, endpoints1, _countof(endpoints1),
                "Progressive List<br>"
                "Host: abcde.xyz:12345<br>"
                "<br>"
                },

                { true, true, false, 1234567890, endpoints2, _countof(endpoints2),
                "Progressive List<br>"
                "Host: abcde.xyz:12345<br>"
                "Host:    abcdefghijk.lmnop.xyz:1234567890   <br>"
                "<br>"
                },

                { true, true, true, 4294967295, endpoints3, _countof(endpoints3),
                "Progressive List<br>"
                "Host: abcde.xyz:12345<br>"
                "Host: abcdefghijk.lmnop.xyz:1234567890<br>"
                "Host:  !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLM NOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~:123456789012345678901234567890 <br>"
                "<br>"
                },
            };

            for (int testIndex(0); testIndex < _countof(TEST_DATA); ++testIndex)
            {
                // Setup.
                CProgressiveConfig varProgConfig;
                varProgConfig.SetHasConfigFile(TEST_DATA[testIndex].m_hasConfigFile);
                varProgConfig.SetActive(TEST_DATA[testIndex].m_active);
                varProgConfig.SetDNSLookup(TEST_DATA[testIndex].m_dnsLookup);
                varProgConfig.SetPingInterval(TEST_DATA[testIndex].m_pingInterval);
                for (int index(0); index < TEST_DATA[testIndex].m_endpointCount; ++index)
                {
                    varProgConfig.AddEndpoint(TEST_DATA[testIndex].m_endpoints[index]);
                }
                const CProgressiveConfig& progConfig(varProgConfig);

                // Test(s).
                AssertResult2(CWebDiagnostics::GetProgressiveHostListHtml(progConfig), TEST_DATA[testIndex].m_htmlExpected);
            }
        }


        void f_GetProgressiveLevelsHtml_Test(void)
        {
            const DWORD tickCount = 1000;
            const SYSTEMTIME levelReceivedTime = { 2001, 2, 0, 3, 12, 34, 56, 789 };
            const SYSTEMTIME lastStaleTime = { 2002, 3, 0, 4, 15, 16, 17, 890 };

            _ICardConfig5 cardConfig;
            shared_ptr<ProgressiveLevelManager> levelManager = cardConfig.CreateLevelManager(tickCount);
            levelManager->m_receivedTime = levelReceivedTime;

            // Perform operation(s) and test(s).
            string generatedHtml = CWebDiagnostics::GetProgressiveLevelsHtml(tickCount, lastStaleTime, *levelManager);
            string expectedHtml = CalcProgressiveLevelsHtml(tickCount, lastStaleTime, *levelManager);

            string OutputDebugString;

            OutputDebugString += "\n---generated---:\n";
            OutputDebugString += generatedHtml;
            OutputDebugString += "\n---expected---:\n";
            OutputDebugString += expectedHtml;

            ASSERT_EQ(expectedHtml, generatedHtml) << OutputDebugString.c_str();
        }

        void f_GetProgressiveLevelsHtml_Test2(void)
        {
            const DWORD tickCount = 1000;
            const SYSTEMTIME levelReceivedTime = { 2001, 2, 0, 3, 12, 34, 56, 789 };
            const SYSTEMTIME lastStaleTime = { 2002, 3, 0, 4, 15, 16, 17, 890 };

            _ICardConfig5 cardConfig;
            shared_ptr<ProgressiveLevelManager> levelManager = cardConfig.CreateLevelManager(tickCount, &levelReceivedTime);

            levelManager->m_levels[5]->m_updatedTickCount = 1250;
            levelManager->m_levels[5]->m_staleCount.m_lastSetTime = lastStaleTime;
            levelManager->m_levels[5]->m_staleCount.m_today.m_count = 1;
            levelManager->m_levels[5]->m_staleCount.m_yesterday.m_count = 1;

            // Perform operation(s) and test(s).
            string generatedHtml = CWebDiagnostics::GetProgressiveLevelsHtml(tickCount, lastStaleTime, *levelManager);
            string expectedHtml = CalcProgressiveLevelsHtml(tickCount, lastStaleTime, *levelManager);

            string OutputDebugString;
            OutputDebugString = "\n---generated---\n";
            OutputDebugString += generatedHtml;
            OutputDebugString += "\n---expected---\n";
            OutputDebugString  += expectedHtml;

            ASSERT_EQ(expectedHtml, generatedHtml) << OutputDebugString.c_str();
        }

                    void f_GetProgressiveLevelsHtml_Test3(void)
            {
                const DWORD tickCount = 1000;
                const SYSTEMTIME levelReceivedTime = { 2001, 2, 0, 3, 12, 34, 56, 789 };
                const SYSTEMTIME lastStaleTime = { 2002, 3, 0, 4, 15, 16, 17, 890 };

                _ICardConfig5 cardConfig;
                shared_ptr<ProgressiveLevelManager> levelManager = cardConfig.CreateLevelManager(tickCount);
                levelManager->m_receivedTime = levelReceivedTime;

                // Perform operation(s) and test(s).
                string generatedHtml = CWebDiagnostics::GetProgressiveLevelsHtml(tickCount, lastStaleTime, *levelManager);
                string expectedHtml = CalcProgressiveLevelsHtml(tickCount, lastStaleTime, *levelManager);

                string OutputDebugString;
                OutputDebugString = "\n---generated---\n";
                OutputDebugString += generatedHtml;
                OutputDebugString += "\n---expected---\n";
                OutputDebugString  += expectedHtml;
                OutputDebugString += "\n---expected---\n";

                ASSERT_EQ(expectedHtml, generatedHtml)<< OutputDebugString.c_str();
            }

            void f_GetProgressiveLevelsHtml_Test4(void)
            {
                const DWORD tickCount = 1000;
                const SYSTEMTIME levelReceivedTime = { 2001, 2, 0, 3, 12, 34, 56, 789 };
                const SYSTEMTIME lastStaleTime = { 2002, 3, 0, 4, 15, 16, 17, 890 };

                _ICardConfig5 cardConfig;
                cardConfig.ProgressiveGroupId = 0;
                shared_ptr<ProgressiveLevelManager> levelManager = cardConfig.CreateLevelManager(tickCount, &levelReceivedTime);

                try
                {
                    int64_t levelId = levelManager->GetLevelIdFromSASLevel(1);
                    shared_ptr<CProgressiveLevel> level = levelManager->m_levels.at(levelId);

                    level->m_staleCount.Set(0, 1, 10, false);
                    level->m_staleCount.m_lastSetTime = lastStaleTime;
                    level->m_staleCount.m_lastResetTime = lastStaleTime;
                    level->m_staleCount.m_lastRolledTime = lastStaleTime;
                    level->m_staleCount.m_lastIntervalTime = lastStaleTime;

                    // Perform operation(s) and test(s).
                    string generatedHtml = CWebDiagnostics::GetProgressiveLevelsHtml(tickCount, lastStaleTime, *levelManager);
                    string expectedHtml = CalcProgressiveLevelsHtml(tickCount, lastStaleTime, *levelManager);

                    string OutputDebugString;
                    OutputDebugString = "\n---generated---\n";
                    OutputDebugString += generatedHtml;
                    OutputDebugString += "\n---expected---\n";
                    OutputDebugString  += expectedHtml;
                    OutputDebugString += "\n---expected---\n";

                    ASSERT_EQ(expectedHtml, generatedHtml) << OutputDebugString.c_str();
                }
                catch (exception&)
                {
                    FAIL() << "Unexpected exception";
                }
            }

           void f_GetProgressiveHostAddressesHtml_Default0Test(void)
            {
                // Input(s).

                // Setup.
                const CProgressiveHost progHost("rdgfdgfg", 127);

                // Expected results.
                const string htmlExpected(
                    "Progressive Addresses<br>"
                    "<br>");

                // Setup.
                const int testIndex(0);

                // Perform operation(s) to be tested.

                // Test(s).
                AssertResult2(CWebDiagnostics::GetProgressiveHostAddressesHtml(progHost), htmlExpected);
            }


            void f_GetProgressiveHostAddressesHtml_Default1Test(void)
            {
                // Input(s).

                // Setup.
                CIPAddressArray ipAddressArray;
                const CProgressiveHost progHost(ipAddressArray);

                // Expected results.
                const string htmlExpected(
                    "Progressive Addresses<br>"
                    "<br>");

                // Setup.
                const int testIndex(0);

                // Test(s).
                AssertResult2(CWebDiagnostics::GetProgressiveHostAddressesHtml(progHost), htmlExpected);
            }

            void f_GetProgressiveHostAddressesHtmlTest(void)
            {
                typedef BYTE TIPAddressBytes [4];

                typedef struct
                {
                    // Input(s).
                    const TIPAddressBytes* const m_ipAddressBytes;
                    const int m_ipAddressCount;

                    // Expected result(s).
                    const string m_htmlExpected;
                } TestDataType;

                // Use the same IP address for each element, so we can predict the randomized results.
                TIPAddressBytes addr1[] = {
                    { 1, 2, 3, 4 },
                };

                TIPAddressBytes addr2[] = {
                    { 0, 0, 0, 0 },
                    { 0, 0, 0, 0 },
                };

                TIPAddressBytes addr3[] = {
                    { 1, 12, 34, 246 },
                    { 1, 12, 34, 246 },
                    { 1, 12, 34, 246 },
                };

                TIPAddressBytes addr4[] = {
                    { 123, 34, 245, 6 },
                    { 123, 34, 245, 6 },
                    { 123, 34, 245, 6 },
                    { 123, 34, 245, 6 },
                };

                TIPAddressBytes addr5[] = {
                    { 255, 255, 255, 255 },
                    { 255, 255, 255, 255 },
                    { 255, 255, 255, 255 },
                    { 255, 255, 255, 255 },
                    { 255, 255, 255, 255 },
                };

                const TestDataType TEST_DATA[] =
                {
                    { NULL, 0,
                        "Progressive Addresses<br>"
                        "<br>"
                    },

                    { addr1, _countof(addr1),
                        "Progressive Addresses<br>"
                        "Host IP: 1.2.3.4<br>"
                        "<br>"
                    },

                    { addr2, _countof(addr2),
                        "Progressive Addresses<br>"
                        "Host IP: 0.0.0.0<br>"
                        "Host IP: 0.0.0.0<br>"
                        "<br>"
                    },

                    { addr3, _countof(addr3),
                        "Progressive Addresses<br>"
                        "Host IP: 1.12.34.246<br>"
                        "Host IP: 1.12.34.246<br>"
                        "Host IP: 1.12.34.246<br>"
                        "<br>"
                    },

                    { addr4, _countof(addr4),
                        "Progressive Addresses<br>"
                        "Host IP: 123.34.245.6<br>"
                        "Host IP: 123.34.245.6<br>"
                        "Host IP: 123.34.245.6<br>"
                        "Host IP: 123.34.245.6<br>"
                        "<br>"
                    },

                    { addr5, _countof(addr5),
                        "Progressive Addresses<br>"
                        "Host IP: 255.255.255.255<br>"
                        "Host IP: 255.255.255.255<br>"
                        "Host IP: 255.255.255.255<br>"
                        "Host IP: 255.255.255.255<br>"
                        "Host IP: 255.255.255.255<br>"
                        "<br>"
                    },
                };

                for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
                {
                    // Setup.
                    CIPAddressArray ipAddressArray;
                    for (int index(0); index < TEST_DATA[testIndex].m_ipAddressCount; ++index)
                    {
                        ipAddressArray.Add(TEST_DATA[testIndex].m_ipAddressBytes[index]);
                    }
                    const CProgressiveHost progHost(ipAddressArray);

                    // Test(s).
                    AssertResult2(CWebDiagnostics::GetProgressiveHostAddressesHtml(progHost), TEST_DATA[testIndex].m_htmlExpected);
                }
            }


            void f_GetLastGameInfo_DefaultTest(void)
            {
                // Setup.
                const int testIndex(0);
                const CPlayerSession playerSession;

                // Expected results.
                const string htmlExpected(CalcLastGameInfo(playerSession));

                // Perform operation(s) and test(s).
                AssertResult2(CWebDiagnostics::GetLastGameInfo(playerSession), htmlExpected);
            }

            void f_GetLastGameInfo_DefaultGameInProgressTest(void)
            {
                // Setup.
                const int testIndex(0);
                CConfig config(true,"1.1.0");
                config.SetGameInProgress(true);
                CPlayerSession playerSession;
                playerSession.UpdateConfigItems(config);

                // Expected results.
                const string htmlExpected(CalcLastGameInfo(playerSession));

                // Perform operation(s) and test(s).
                AssertResult2(CWebDiagnostics::GetLastGameInfo(playerSession), htmlExpected);
            }

            void f_GetLastGameInfo_DefaultNotGameInProgressTest(void)
            {
                // Setup.
                const int testIndex(0);
                CConfig config(true,"1.1.0");
                config.SetGameInProgress(false);
                CPlayerSession playerSession;
                playerSession.UpdateConfigItems(config);

                // Expected results.
                const string htmlExpected(CalcLastGameInfo(playerSession));

                // Perform operation(s) and test(s).
                AssertResult2(CWebDiagnostics::GetLastGameInfo(playerSession), htmlExpected);
            }

            void f_GetLastGameInfo_GameEndedTest(void)
            {
                // Setup.
                const int testIndex(0);
                CPlayerSession playerSession;
                playerSession.GameEnded();

                // Expected results.
                const string htmlExpected(CalcLastGameInfo(playerSession));

                // Perform operation(s) and test(s).
                AssertResult2(CWebDiagnostics::GetLastGameInfo(playerSession), htmlExpected);
            }

            void f_GetLastGameInfo_GameStartedTest(void)
            {
                // Setup.
                const int testIndex(0);
                CPlayerSession playerSession;
                playerSession.GameStarted();

                // Expected results.
                const string htmlExpected(CalcLastGameInfo(playerSession));

                // Perform operation(s) and test(s).
                AssertResult2(CWebDiagnostics::GetLastGameInfo(playerSession), htmlExpected);
            }

    };

	/// <summary>
	/// Code coverage and functional test for:
	///     CWebDiagnostics::GetProgressiveConfigHtml()
	/// Test with default values.
	/// </summary>
	TEST_F(WebDiagnostics_Progressive, GetProgressiveConfigHtml_DefaultTest)
	{
		f_GetProgressiveConfigHtml_DefaultTest();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CWebDiagnostics::GetProgressiveConfigHtml()
	/// </summary>
	TEST_F(WebDiagnostics_Progressive, GetProgressiveConfigHtmlTest)
	{
		f_GetProgressiveConfigHtmlTest();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CWebDiagnostics::GetProgressiveConnectionInfoHtml()
	/// Test with empty host name and host port zero.
	/// </summary>
	TEST_F(WebDiagnostics_Progressive, GetProgressiveConnectionInfoHtml_Default0Test)
	{
		f_GetProgressiveConnectionInfoHtml_Default0Test();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CWebDiagnostics::GetProgressiveConnectionInfoHtml()
	/// Test with empty CIPAddressArray.
	/// </summary>
	TEST_F(WebDiagnostics_Progressive, GetProgressiveConnectionInfoHtml_Default1Test)
	{
		// Input(s).
		f_GetProgressiveConnectionInfoHtml_Default1Test();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CWebDiagnostics::GetProgressiveConnectionInfoHtml()
	/// Test with non-empty CIPAddressArray.
	/// </summary>
	TEST_F(WebDiagnostics_Progressive, GetProgressiveConnectionInfoHtml_Default2Test)
	{
		f_GetProgressiveConnectionInfoHtml_Default2Test();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CWebDiagnostics::GetProgressiveConnectionInfoHtml()
	/// Each iteration tests a (different) state and/or error condition.
	/// </summary>
	TEST_F(WebDiagnostics_Progressive, GetProgressiveConnectionInfoHtmlTest)
	{
		f_GetProgressiveConnectionInfoHtmlTest();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CWebDiagnostics::GetProgressiveHostListHtml()
	/// Test with default values.
	/// </summary>
	TEST_F(WebDiagnostics_Progressive, GetProgressiveHostListHtml_DefaultTest)
	{
		f_GetProgressiveHostListHtml_DefaultTest();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CWebDiagnostics::GetProgressiveConfigHtml()
	/// Test with default values.
	/// </summary>
	TEST_F(WebDiagnostics_Progressive, GetProgressiveHostListHtmlTest)
	{
		f_GetProgressiveHostListHtmlTest();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CWebDiagnostics::GetProgressiveDataHtml()
	/// Test with default values.
	/// </summary>
	TEST_F(WebDiagnostics_Progressive, GetProgressiveLevelsHtml_Test)
	{
		f_GetProgressiveLevelsHtml_Test();
	}

	TEST_F(WebDiagnostics_Progressive, GetProgressiveLevelsHtml_Test2)
	{
		f_GetProgressiveLevelsHtml_Test2();
	}


	TEST_F(WebDiagnostics_Progressive, GetProgressiveLevelsHtml_Test3)
	{
		f_GetProgressiveLevelsHtml_Test3();
	}

	TEST_F(WebDiagnostics_Progressive, GetProgressiveLevelsHtml_Test4)
	{
		f_GetProgressiveLevelsHtml_Test4();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CWebDiagnostics::GetProgressiveHostAddressesHtml()
	/// Test with empty host name and host port zero.
	/// </summary>
	TEST_F(WebDiagnostics_Progressive, GetProgressiveHostAddressesHtml_Default0Test)
	{
		f_GetProgressiveHostAddressesHtml_Default0Test();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CWebDiagnostics::GetProgressiveHostAddressesHtml()
	/// Test with empty CIPAddressArray.
	/// </summary>
	TEST_F(WebDiagnostics_Progressive, GetProgressiveHostAddressesHtml_Default1Test)
	{
		f_GetProgressiveHostAddressesHtml_Default1Test();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CWebDiagnostics::GetProgressiveHostAddressesHtml()
	/// Test with CIPAddressArray.
	/// </summary>
	TEST_F(WebDiagnostics_Progressive, GetProgressiveHostAddressesHtmlTest)
	{
		f_GetProgressiveHostAddressesHtmlTest();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CWebDiagnostics::GetLastGameInfo()
	/// Test with default value -- game not in progress and no game start and no game end.
	/// </summary>
	TEST_F(WebDiagnostics_Progressive, GetLastGameInfo_DefaultTest)
	{
		f_GetLastGameInfo_DefaultTest();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CWebDiagnostics::GetLastGameInfo()
	/// Test with default value, plus game in progress.
	/// </summary>
	TEST_F(WebDiagnostics_Progressive, GetLastGameInfo_DefaultGameInProgressTest)
	{
		f_GetLastGameInfo_DefaultGameInProgressTest();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CWebDiagnostics::GetLastGameInfo()
	/// Test with default value, plus game not in progress.
	/// </summary>
	TEST_F(WebDiagnostics_Progressive, GetLastGameInfo_DefaultNotGameInProgressTest)
	{
		f_GetLastGameInfo_DefaultNotGameInProgressTest();
	}

	///// <summary>
	///// Code coverage and functional test for:
	/////     CWebDiagnostics::GetLastGameInfo()
	///// Test with game started, but not game ended.
	///// </summary>
	//TEST_F(WebDiagnostics_Progressive, GetLastGameInfo_GameStartedTest)
	//{
	//	f_GetLastGameInfo_GameStartedTest();
	//}

	///// <summary>
	///// Code coverage and functional test for:
	/////     CWebDiagnostics::GetLastGameInfo()
	///// Test with game ended, but not game started.
	///// </summary>
	//TEST_F(WebDiagnostics_Progressive, GetLastGameInfo_GameEndedTest)
	//{
	//	f_GetLastGameInfo_GameEndedTest();
	//}

 //   /// <summary>
 //   /// Code coverage and functional test for:
 //   ///     CWebDiagnostics::GetLastGameInfo()
 //   /// Test with game started and then game ended.
 //   /// </summary>
 //   TEST_F(WebDiagnostics_Progressive, GetLastGameInfo_GameStartedGameEndedTest)
 //   {
 //       // Setup.
 //       const int testIndex(0);
 //       CPlayerSession playerSession;
 //       playerSession.GameStarted();
 //       playerSession.GameEnded();

 //       // Expected results.
 //       const string htmlExpected(CalcLastGameInfo(playerSession));

 //       // Perform operation(s) and test(s).
 //       AssertResult2(CWebDiagnostics::GetLastGameInfo(playerSession), htmlExpected);
 //   }

 //   /// <summary>
 //   /// Code coverage and functional test for:
 //   ///     CWebDiagnostics::GetLastGameInfo()
 //   /// Test with game ended and then game started.
 //   /// </summary>
 //   TEST_F(WebDiagnostics_Progressive, GetLastGameInfo_GameEndedGameStartedTest)
 //   {
 //       // Setup.
 //       const int testIndex(0);
 //       CPlayerSession playerSession;
 //       playerSession.GameEnded();
 //       playerSession.GameStarted();

 //       // Expected results.
 //       const string htmlExpected(CalcLastGameInfo(playerSession));

 //       // Perform operation(s) and test(s).
 //       AssertResult2(CWebDiagnostics::GetLastGameInfo(playerSession), htmlExpected);
 //   }

 //   /// <summary>
 //   /// Code coverage and functional test for:
 //   ///     CWebDiagnostics::GetLastGameInfo()
 //   /// Test with consecutive game starts so GetEgmSendsGameEnd() returns false.
 //   /// </summary>
 //   TEST_F(WebDiagnostics_Progressive, GetLastGameInfo_GetEgmSendsGameEndedFalse)
 //   {
 //       // Setup.
 //       const int testIndex(0);
 //       CPlayerSession playerSession;
 //       playerSession.GameStarted();
 //       playerSession.GameStarted();
 //       AssertResult2(playerSession.GetEgmSendsGameEnd(), false); // Verify setup.

 //       // Expected results.
 //       const string htmlExpected(CalcLastGameInfo(playerSession));

 //       // Perform operation(s) and test(s).
 //       AssertResult2(CWebDiagnostics::GetLastGameInfo(playerSession), htmlExpected);
 //   }

 //   /// <summary>
 //   /// Code coverage and functional test for:
 //   ///     CWebDiagnostics::GetLastGameInfo()
 //   /// Test with game start followed by game end so GetEgmSendsGameEnd() returns true.
 //   /// </summary>
 //   TEST_F(WebDiagnostics_Progressive, GetLastGameInfo_GetEgmSendsGameEndedTrue)
 //   {
 //       // Setup.
 //       const int testIndex(0);
 //       CPlayerSession playerSession;
 //       playerSession.GameStarted();
 //       playerSession.GameEnded();
 //       AssertResult2(playerSession.GetEgmSendsGameEnd(), true); // Verify setup.

 //       // Expected results.
 //       const string htmlExpected(CalcLastGameInfo(playerSession));

 //       // Perform operation(s) and test(s).
 //       AssertResult2(CWebDiagnostics::GetLastGameInfo(playerSession), htmlExpected);
 //   }
    
// #pragma region GetBonusAwardHtml

    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetBonusAwardHtml()
    /// Test with default award.
    /// </summary>
    TEST_F(WebDiagnostics_Progressive, GetBonusAwardHtml_DefaultTest)
    {
        // Data.
        const DWORD tickCount(0);

        // Setup.
        const CEGMAward progAward;
        const CEGMAwardManagerState progState;

        // Perform operation(s) and test(s).
        ASSERT_EQ(CalcBonusAwardHtml(tickCount, progAward, progState),
            CWebDiagnostics::GetBonusAwardHtml(tickCount, progAward, progState)) << "GetBonusAwardHtml";
    }

    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetBonusAwardHtml()
    /// Test with non-default award.
    /// </summary>
    TEST_F(WebDiagnostics_Progressive, GetBonusAwardHtml_AwardTest)
    {
        // Data.
        const DWORD tickCount(0);

        // Setup.
        CEGMAward progAward;
        progAward.SetAwardAmountPaid(12345);
        progAward.SetAwardState(AwardToGameSuccess);
        progAward.SetSentToGameCount(2);
        const SYSTEMTIME sentToGameTime = { 2001, 2, 0, 3, 12, 34, 56, 789 };
        progAward.SetSentToGameTime(sentToGameTime);
        const SYSTEMTIME receivedFromSystemTime = { 2001, 2, 0, 3, 12, 34, 55, 678 };
        progAward.SetReceivedFromSystemTime(receivedFromSystemTime);
        progAward.SetAwardTransferType(EGMAwardTransferType_SASAFTBonusingCash);
        progAward.SetDuplicateHitCount(3);
        progAward.SetOtherHitCount(4);
        progAward.SetOriginalMachineBonusMeterValue(2345);
        progAward.SetOriginalAttendantBonusingMeterValue(34567);
        progAward.SetLevelId(678);
        progAward.SetJackpotId(123456789012);
        progAward.SetRemoteJackpotId(2345678901);
        progAward.SetAwardAmountHit(12344);

        const CEGMAwardManagerState progState;

        // Perform operation(s) and test(s).
        ASSERT_EQ(CalcBonusAwardHtml(tickCount, progAward, progState),
            CWebDiagnostics::GetBonusAwardHtml(tickCount, progAward, progState)) << "GetBonusAwardHtml";
    }

    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetBonusAwardHtml()
    /// Test with non-default award and waiting for GCF.
    /// </summary>
    TEST_F(WebDiagnostics_Progressive, GetBonusAwardHtml_WaitingForGCFTest)
    {
        // Data.
        const DWORD tickCount(0);

        // Setup.
        CEGMAward progAward;
        progAward.SetAwardAmountPaid(12345);
        progAward.SetAwardState(AwardToGameSuccess);
        progAward.SetSentToGameCount(2);
        const SYSTEMTIME sentToGameTime = { 2001, 2, 0, 3, 12, 34, 56, 789 };
        progAward.SetSentToGameTime(sentToGameTime);
        const SYSTEMTIME receivedFromSystemTime = { 2001, 2, 0, 3, 12, 34, 55, 678 };
        progAward.SetReceivedFromSystemTime(receivedFromSystemTime);
        progAward.SetAwardTransferType(EGMAwardTransferType_SASAFTBonusingCash);
        progAward.SetDuplicateHitCount(3);
        progAward.SetOtherHitCount(4);
        progAward.SetOriginalMachineBonusMeterValue(2345);
        progAward.SetOriginalAttendantBonusingMeterValue(34567);
        progAward.SetLevelId(678);
        progAward.SetJackpotId(123456789012);
        progAward.SetRemoteJackpotId(2345678901);
        progAward.SetAwardAmountHit(12344);

        CEGMAwardManagerState progState;
        progState.m_waitingForGCF = true;

        // Perform operation(s) and test(s).
        ASSERT_EQ(CalcBonusAwardHtml(tickCount, progAward, progState),
            CWebDiagnostics::GetBonusAwardHtml(tickCount, progAward, progState)) << "GetBonusAwardHtml";
    }


    /// <summary>
    /// Functional test for:
    /// 	CWebDiagnostics::GetBonusAwardHtml()
    /// Test with non-default award and (award) in progress.
    /// </summary>
    TEST_F(WebDiagnostics_Progressive, GetBonusAwardHtml_InProgressTest)
    {
        // Data.
        const DWORD tickCount(0);

        // Setup.
        CEGMAward progAward;
        progAward.SetAwardAmountPaid(12345);
        progAward.SetAwardState(AftLockPending);
        progAward.SetSentToGameCount(2);
        const SYSTEMTIME sentToGameTime = { 2001, 2, 0, 3, 12, 34, 56, 789 };
        progAward.SetSentToGameTime(sentToGameTime);
        const SYSTEMTIME receivedFromSystemTime = { 2001, 2, 0, 3, 12, 34, 55, 678 };
        progAward.SetReceivedFromSystemTime(receivedFromSystemTime);
        progAward.SetAwardTransferType(EGMAwardTransferType_SASAFTBonusingCash);
        progAward.SetDuplicateHitCount(3);
        progAward.SetOtherHitCount(4);
        progAward.SetOriginalMachineBonusMeterValue(2345);
        progAward.SetOriginalAttendantBonusingMeterValue(34567);
        progAward.SetLevelId(678);
        progAward.SetJackpotId(123456789012);
        progAward.SetRemoteJackpotId(2345678901);
        progAward.SetAwardAmountHit(12344);

        CEGMAwardManagerState progState;
        progState.m_waitingForGCF = false;

        // Perform operation(s) and test(s).
        ASSERT_EQ(CalcBonusAwardHtml(tickCount, progAward, progState),
            CWebDiagnostics::GetBonusAwardHtml(tickCount, progAward, progState)) << "GetBonusAwardHtml";
    }

    /// <summary>
    /// Functional test for:
    /// 	CWebDiagnostics::GetBonusAwardHtml()
    /// Test with non-default award and (the award) in progress and expirable.
    /// </summary>
    TEST_F(WebDiagnostics_Progressive, GetBonusAwardHtml_ExpirableTest)
    {
        // Data.
        const DWORD tickCount(0);

        // Setup.
        CEGMAward progAward;
        progAward.SetAwardAmountPaid(12345);
        progAward.SetAwardState(AftLockPending);
        progAward.SetSentToGameCount(2);
        const SYSTEMTIME sentToGameTime = { 2001, 2, 0, 3, 12, 34, 56, 789 };
        progAward.SetSentToGameTime(sentToGameTime);
        const SYSTEMTIME receivedFromSystemTime = { 2001, 2, 0, 3, 12, 34, 55, 678 };
        progAward.SetReceivedFromSystemTime(receivedFromSystemTime);
        progAward.SetAwardTransferType(EGMAwardTransferType_SASAFTBonusingCash);
        progAward.SetDuplicateHitCount(3);
        progAward.SetOtherHitCount(4);
        progAward.SetOriginalMachineBonusMeterValue(2345);
        progAward.SetOriginalAttendantBonusingMeterValue(34567);
        progAward.SetLevelId(678);
        progAward.SetJackpotId(123456789012);
        progAward.SetRemoteJackpotId(2345678901);
        progAward.SetAwardAmountHit(12344);

        CEGMAwardManagerState progState;
        progState.m_waitingForGCF = false;
        progState.m_awardExpired = true;

        // Perform operation(s) and test(s).
        ASSERT_EQ(CalcBonusAwardHtml(tickCount, progAward, progState),
            CWebDiagnostics::GetBonusAwardHtml(tickCount, progAward, progState)) << "GetBonusAwardHtml";
    }
// #pragma endregion GetBonusAwardHtml

/// <summary>
/// Code coverage and functional test for:
///     CWebDiagnostics::SaveProgressiveData()
/// Test with empty host name and host port zero.
/// </summary> 
TEST_F(WebDiagnostics_Progressive, SaveProgressiveData_DefaultTest)
    {
        // Input(s).
        const int testIndex(0);
        const string dumpFileName(_T("/tmp/ProgressiveInfo.html"));
        const DWORD tickCount(0);

		time_t startupTime(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12));
		time_t currentTime(CUtilities::GetTimeFromFields(2014, 1, 27, 13, 14, 15));

        CPackageInfo activeSkin(10, "ActiveSkinName", "10", 2, 0);
        CPackageInfo pendingSkin(11, "PendingSkinName", "11", 3, 2);
        CPackageInfo activeCampaign(101, "ActiveCampaignName", "101", 4, 0);
        CPackageInfo pendingCampaign(102, "PendingCampaignName", "102", 5, 3);

        const CWebDiagnosticsCommonInfo commonInfo(startupTime,
			currentTime,
			941,
			5141,
			_T("1.12.2.0"),
			_T("1.21.0"),
            activeSkin,
            pendingSkin,
            activeCampaign,
            pendingCampaign,
            161028 * 1024,
			_T("192.168.1.12"),
			0,
			8, 
			"Sentinel is online", "", "", "", 0.0);

        // Buffer length expected does not include the trailing NUL character.
        const BYTE bufferExpected[] = {
            "<html>"
            "<META Http-equiv=\"refresh\" Content=\"30\">Startup Time: 01/27/2014 10:11:12 AM"
            "<br>Current Time: 01/27/2014 01:14:15 PM"
            "<br>SlotMast ID: 941"
            "<br>Machine Number:5141"
            "<br>Sentinel Version: 1.12.2.0"
            "<br>OS Image Version: 1.21.0"
            "<br>Skin Version: 1.2"
            "<br>Skin Name: Skin"
            "<br>Campaign Name: Campaign"
            "<br>Carrier Version: 1.23"
            "<br>VM Version: 1.2"
            "<br>Battery Status: 100"
            "<br>CPU Temperature (Celsius): 0.02"
            "<br>Disk Space Info SD Card1 (OS) Size:"
            "<br>Disk Space Info SD Card1 (OS) Available:"
            "<br>Disk Space Info SD Card1 (OS) Used:"
            "<br>Disk Space Info SD Card1 (SentinelData) Size:"
            "<br>Disk Space Info SD Card1 (SentinelData) Available:"
            "<br>Disk Space Info SD Card1 (SentinelData) Used:"
            "<br>Disk Space Info SD Card2 (Content) Size:"
            "<br>Disk Space Info SD Card2 (Content) Available:"
            "<br>Disk Space Info SD Card2 (Content) Used:"
            "<br>"
            "<br>Configuration File Present: false"
            "<br>Progressive Configuration: Disabled"
            "<br>DNS Lookup: true "
            "<br>Ping Interval(ms): 15000"
            "<br>"
            "<br>Progressive Connection Info"
            "<br>DNS Hostname:"
            "<br>Current Host IP: N/A"
            "<br>Connection State: Not Started"
            "<br>Last Message IP: N/A"
            "<br>Last Message Time: N/A"
            "<br>Failed One Link Call Count: 0"
            "<br>Last failed One Link Call Timestamp: N/A"
            "<br>"
            "<br>Progressive List"
            "<br>Host: onelinkdiscossl:61614"
            "<br>Host: onelinkdisco:61613"
            "<br>"
            "<br>Progressive Addresses"
            "<br>"
            "<br>Last Progressive Award"
            "<br>N/A"
            "<br>"
            "<br>"
            "</html>"
        };

        // Setup.
        const int m_testIndex(0);
        const CProgressiveConfig progConfig;
        const CProgressiveConnectionInfo progInfo;
        const CProgressiveHost progHost(_T(""), 0);
        const CEGMAward progAward;
        const std::shared_ptr<CEGMAward> lastStandardWin;
        const std::vector<std::shared_ptr<CEGMAward>> standardWins;
        const CPlayerSession playerSession;
        CEGMAwardManagerState progState;
        SYSTEMTIME levelReceivedTime = { 2001, 2, 0, 3, 12, 34, 56, 789 };

        _ICardConfig5 cardConfig;
        shared_ptr<ProgressiveLevelManager> levelManager = cardConfig.CreateLevelManager(tickCount, &levelReceivedTime);

        const SYSTEMTIME localTime = { 2015, 7, 5, 10, 12, 0, 0, 0 };

        COperatingSystemTest operatingSystem;
        std::shared_ptr<std::ostream> file = shared_ptr<iostream>(new fstream(dumpFileName, ios_base::out | ios_base::trunc));

        // Perform operation(s) to be tested.
        CWebDiagnostics::SaveProgressiveData(file,
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
            *levelManager);

        // Test(s).
        string oldHtml(reinterpret_cast<const char *>(bufferExpected), sizeof(bufferExpected) - 1);

        string expected(InsertProgressiveDataHtml(oldHtml, playerSession, tickCount, localTime, progState, *levelManager,
            progAward, lastStandardWin, standardWins));

		std::ifstream infile(dumpFileName);
		std::stringstream strStream;
		strStream << infile.rdbuf();
		string filestr = strStream.str();
		bool result = expected.compare(strStream.str());
		AssertResult3(result, true, "ProgressiveData_DefaultTest");
    }


    /// <summary>
    /// Code coverage and functional test for:
    ///     CWebDiagnostics::SaveProgressiveData()
    /// Each iteration tests a (different) state and/or error condition.
    /// </summary> 
 
    TEST_F(WebDiagnostics_Progressive, SaveProgressiveDataTest)
    {
        // Input(s).
        const int utilitiesTimeZoneOffset(0);
        LPCTSTR const currentHostIP(_T("11.22.33.44"));
        const std::string dumpFileName(_T("/tmp/ProgressiveInfo.html"));
        const DWORD tickCount(0);
        CPlayerSession playerSession;

		time_t startupTime(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12));
		time_t currentTime(CUtilities::GetTimeFromFields(2014, 1, 27, 13, 14, 15));

        CPackageInfo activeSkin(10, "ActiveSkinName", "10", 2, 0);
        CPackageInfo pendingSkin(11, "PendingSkinName", "11", 3, 2);
        CPackageInfo activeCampaign(101, "ActiveCampaignName", "101", 4, 0);
        CPackageInfo pendingCampaign(102, "PendingCampaignName", "102", 5, 3);
        
        const CWebDiagnosticsCommonInfo commonInfo(startupTime,
			currentTime,
			941,
			5141,
			_T("1.12.2.0"),
			_T("1.21.0"),
            activeSkin,
            pendingSkin,
            activeCampaign,
            pendingCampaign,
            161028 * 1024,
			_T("192.168.1.12"),
			0,
			8, 
			"Sentinel is online", "", "", "", 0.0);
		
        typedef struct {
            LPCTSTR const m_ipAddressString;
            int m_port;
        } TIPAddressStringPort;

        typedef struct
        {
            // Input(s) for CProgressiveConfig.
            const bool m_hasConfigFile;
            const bool m_active;
            const bool m_dnsLookup;
            const DWORD m_pingInterval;
            const LPCTSTR* const m_endpoints;
            const int m_endpointCount;

            // Input(s) for CProgressiveConnectionInfo.
            const TIPAddressStringPort m_lastMessageIP;
            const SYSTEMTIME m_lastMessageTimestamp;
            const ProgressiveConnectionState m_state;
            const ProgressiveConnectionError m_error;

            // Input(s) for CProgressiveHost.
            LPCTSTR const m_hostName;
            const int m_hostPort;

            // Expected result(s).
            const BYTE* const m_bufferExpected;
            const UINT m_bufferLengthExpected;
        } TestDataType;

        // Input(s).
        LPCTSTR endpoints1[] = {
            _T("abcde.xyz:12345"),
        };
        LPCTSTR endpoints2[] = {
            _T("abcde.xyz:12345"),
            _T("   abcdefghijk.lmnop.xyz:1234567890   "),
        };
        LPCTSTR endpoints3[] = {
            _T("abcde.xyz:12345"),
            _T("abcdefghijk.lmnop.xyz:1234567890"),
            _T("!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~:123456789012345678901234567890 "),
        };

        // Buffer length expected does not include the trailing NUL character.
        const BYTE bufferExpected0[] = {
            "<html><META Http-equiv=\"refresh\" Content=\"30\">"
            "Startup Time: 01/27/2014 10:11:12 AM<br>"
            "Current Time: 01/27/2014 01:14:15 PM<br>"
            "SlotMast ID: 941<br>"
            "Machine Number:5141<br>"
            "Sentinel Version: 1.12.2.0<br>"
            "OS Image Version: 1.21.0<br>"
            "<br>"
            "Configuration File Present: false<br>"
            "Progressive Configuration: Disabled<br>"
            "DNS Lookup: false<br>"
            "Ping Interval(ms): 15000<br>"
            "<br>"
            "Progressive Connection Info<br>"
            "DNS Hostname: N/A<br>"
            "Current Host IP: 1.2.3.4<br>"
            "Connection State: N/A<br>"
            "Last Message IP: 101.104.105.0<br>"
            "Last Message Time: 02/03/2011 14:25:00<br>"
            "<br>"
            "Progressive List<br>"
            "Host: onelinkdiscossl:61614<br>"
            "Host: onelinkdisco:61613<br>"
            "<br>"
            "Progressive Addresses<br>"
            "Host IP: 1.2.3.4<br>"
            "<br>"
            "Last Progressive Award<br>"
            "N/A<br>"
            "<br>"
            "</html>"
        };

        const BYTE bufferExpected1[] = {
            "<html><META Http-equiv=\"refresh\" Content=\"30\">"
            "Startup Time: 01/27/2014 10:11:12 AM<br>"
            "Current Time: 01/27/2014 01:14:15 PM<br>"
            "SlotMast ID: 941<br>"
            "Machine Number:5141<br>"
            "Sentinel Version: 1.12.2.0<br>"
            "OS Image Version: 1.21.0<br>"
            "<br>"
            "Configuration File Present: false<br>"
            "Progressive Configuration: Disabled<br>"
            "DNS Lookup: true<br>"
            "Ping Interval(ms): 6000<br>"
            "<br>"
            "Progressive Connection Info<br>"
            "DNS Hostname: invalid.<br>"
            "Current Host IP: N/A<br>"
            "Connection State: N/A<br>"
            "Last Message IP: 101.104.105.1<br>"
            "Last Message Time: 02/03/2011 14:25:01<br>"
            "<br>"
            "Progressive List<br>"
            "Host: abcde.xyz:12345<br>"
            "<br>"
            "Progressive Addresses<br>"
            "<br>"
            "Last Progressive Award<br>"
            "N/A<br>"
            "<br>"
            "</html>"
        };

        const BYTE bufferExpected2[] = {
            "<html><META Http-equiv=\"refresh\" Content=\"30\">"
            "Startup Time: 01/27/2014 10:11:12 AM<br>"
            "Current Time: 01/27/2014 01:14:15 PM<br>"
            "SlotMast ID: 941<br>"
            "Machine Number:5141<br>"
            "Sentinel Version: 1.12.2.0<br>"
            "OS Image Version: 1.21.0<br>"
            "<br>"
            "Configuration File Present: false<br>"
            "Progressive Configuration: Enabled<br>"
            "DNS Lookup: false<br>"
            "Ping Interval(ms): 1234567890<br>"
            "<br>"
            "Progressive Connection Info<br>"
            "DNS Hostname: N/A<br>"
            "Current Host IP: 1.2.3.4<br>"
            "Connection State: Not Started<br>"
            "Last Message IP: 101.104.105.2<br>"
            "Last Message Time: 02/03/2011 14:25:02<br>"
            "<br>"
            "Progressive List<br>"
            "Host: abcde.xyz:12345<br>"
            "Host:    abcdefghijk.lmnop.xyz:1234567890   <br>"
            "<br>"
            "Progressive Addresses<br>"
            "Host IP: 1.2.3.4<br>"
            "Host IP: 1.2.3.4<br>"
            "Host IP: 1.2.3.4<br>"
            "<br>"
            "Last Progressive Award<br>"
            "N/A<br>"
            "<br>"
            "</html>"
        };

        const BYTE bufferExpected3[] = {
            "<html><META Http-equiv=\"refresh\" Content=\"30\">"
            "Startup Time: 01/27/2014 10:11:12 AM<br>"
            "Current Time: 01/27/2014 01:14:15 PM<br>"
            "SlotMast ID: 941<br>"
            "Machine Number:5141<br>"
            "Sentinel Version: 1.12.2.0<br>"
            "OS Image Version: 1.21.0<br>"
            "<br>"
            "Configuration File Present: false<br>"
            "Progressive Configuration: Enabled<br>"
            "DNS Lookup: true<br>"
            "Ping Interval(ms): 4294967295<br>"
            "<br>"
            "Progressive Connection Info<br>"
            "DNS Hostname: localhost.<br>"
            "Current Host IP: 127.0.0.1<br>"
            "Connection State: Disabled<br>"
            "Last Message IP: 101.104.105.3<br>"
            "Last Message Time: 02/03/2011 14:25:03<br>"
            "<br>"
            "Progressive List<br>"
            "Host: abcde.xyz:12345<br>"
            "Host: abcdefghijk.lmnop.xyz:1234567890<br>"
            "Host: !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~:123456789012345678901234567890 <br>"
            "<br>"
            "Progressive Addresses<br>"
            "Host IP: 127.0.0.1<br>"
            "<br>"
            "Last Progressive Award<br>"
            "N/A<br>"
            "<br>"
            "</html>"
        };

        const BYTE bufferExpected4[] = {
            "<html><META Http-equiv=\"refresh\" Content=\"30\">"
            "Startup Time: 01/27/2014 10:11:12 AM<br>"
            "Current Time: 01/27/2014 01:14:15 PM<br>"
            "SlotMast ID: 941<br>"
            "Machine Number:5141<br>"
            "Sentinel Version: 1.12.2.0<br>"
            "OS Image Version: 1.21.0<br>"
            "<br>"
            "Configuration File Present: true<br>"
            "Progressive Configuration: Disabled<br>"
            "DNS Lookup: false<br>"
            "Ping Interval(ms): 15000<br>"
            "<br>"
            "Progressive Connection Info<br>"
            "DNS Hostname: N/A<br>"
            "Current Host IP: 1.2.3.4<br>"
            "Connection State: Waiting For Info<br>"
            "Last Message IP: 101.104.105.4<br>"
            "Last Message Time: 02/03/2011 14:25:04<br>"
            "<br>"
            "Progressive List<br>"
            "Host: onelinkdiscossl:61614<br>"
            "Host: onelinkdisco:61613<br>"
            "<br>"
            "Progressive Addresses<br>"
            "Host IP: 1.2.3.4<br>"
            "Host IP: 1.2.3.4<br>"
            "Host IP: 1.2.3.4<br>"
            "Host IP: 1.2.3.4<br>"
            "Host IP: 1.2.3.4<br>"
            "<br>"
            "Last Progressive Award<br>"
            "N/A<br>"
            "<br>"
            "</html>"
        };

        const BYTE bufferExpected5[] = {
            "<html><META Http-equiv=\"refresh\" Content=\"30\">"
            "Startup Time: 01/27/2014 10:11:12 AM<br>"
            "Current Time: 01/27/2014 01:14:15 PM<br>"
            "SlotMast ID: 941<br>"
            "Machine Number:5141<br>"
            "Sentinel Version: 1.12.2.0<br>"
            "OS Image Version: 1.21.0<br>"
            "<br>"
            "Configuration File Present: true<br>"
            "Progressive Configuration: Disabled<br>"
            "DNS Lookup: true<br>"
            "Ping Interval(ms): 6000<br>"
            "<br>"
            "Progressive Connection Info<br>"
            "DNS Hostname: invalid.<br>"
            "Current Host IP: N/A<br>"
            "Connection State: In Discovery<br>"
            "Last Message IP: 101.104.105.5<br>"
            "Last Message Time: 02/03/2011 14:25:05<br>"
            "<br>"
            "Progressive List<br>"
            "Host: abcde.xyz:12345<br>"
            "<br>"
            "Progressive Addresses<br>"
            "<br>"
            "Last Progressive Award<br>"
            "N/A<br>"
            "<br>"
            "</html>"
        };

        const BYTE bufferExpected6[] = {
            "<html><META Http-equiv=\"refresh\" Content=\"30\">"
            "Startup Time: 01/27/2014 10:11:12 AM<br>"
            "Current Time: 01/27/2014 01:14:15 PM<br>"
            "SlotMast ID: 941<br>"
            "Machine Number:5141<br>"
            "Sentinel Version: 1.12.2.0<br>"
            "OS Image Version: 1.21.0<br>"
            "<br>"
            "Configuration File Present: true<br>"
            "Progressive Configuration: Enabled<br>"
            "DNS Lookup: false<br>"
            "Ping Interval(ms): 1234567890<br>"
            "<br>"
            "Progressive Connection Info<br>"
            "DNS Hostname: N/A<br>"
            "Current Host IP: 1.2.3.4<br>"
            "Connection State: Connected<br>"
            "Last Message IP: 101.104.105.6<br>"
            "Last Message Time: 02/03/2011 14:25:06<br>"
            "<br>"
            "Progressive List<br>"
            "Host: abcde.xyz:12345<br>"
            "Host:    abcdefghijk.lmnop.xyz:1234567890   <br>"
            "<br>"
            "Progressive Addresses<br>"
            "Host IP: 1.2.3.4<br>"
            "Host IP: 1.2.3.4<br>"
            "<br>"
            "Last Progressive Award<br>"
            "N/A<br>"
            "<br>"
            "</html>"
        };

        const BYTE bufferExpected7[] = {
            "<html><META Http-equiv=\"refresh\" Content=\"30\">"
            "Startup Time: 01/27/2014 10:11:12 AM<br>"
            "Current Time: 01/27/2014 01:14:15 PM<br>"
            "SlotMast ID: 941<br>"
            "Machine Number:5141<br>"
            "Sentinel Version: 1.12.2.0<br>"
            "OS Image Version: 1.21.0<br>"
            "<br>"
            "Configuration File Present: true<br>"
            "Progressive Configuration: Enabled<br>"
            "DNS Lookup: true<br>"
            "Ping Interval(ms): 4294967295<br>"
            "<br>"
            "Progressive Connection Info<br>"
            "DNS Hostname: localhost.<br>"
            "Current Host IP: 127.0.0.1<br>"
            "Connection State: Disconnected<br>"
            "Last Message IP: 101.104.105.7<br>"
            "Last Message Time: 02/03/2011 14:25:07<br>"
            "<br>"
            "Progressive List<br>"
            "Host: abcde.xyz:12345<br>"
            "Host: abcdefghijk.lmnop.xyz:1234567890<br>"
            "Host: !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~:123456789012345678901234567890 <br>"
            "<br>"
            "Progressive Addresses<br>"
            "Host IP: 127.0.0.1<br>"
            "<br>"
            "Last Progressive Award<br>"
            "N/A<br>"
            "<br>"
            "</html>"
        };

        const BYTE bufferExpected8[] = {
            "<html><META Http-equiv=\"refresh\" Content=\"30\">"
            "Startup Time: 01/27/2014 10:11:12 AM<br>"
            "Current Time: 01/27/2014 01:14:15 PM<br>"
            "SlotMast ID: 941<br>"
            "Machine Number:5141<br>"
            "Sentinel Version: 1.12.2.0<br>"
            "OS Image Version: 1.21.0<br>"
            "<br>"
            "Configuration File Present: false<br>"
            "Progressive Configuration: Disabled<br>"
            "DNS Lookup: false<br>"
            "Ping Interval(ms): 15000<br>"
            "<br>"
            "Progressive Connection Info<br>"
            "DNS Hostname: N/A<br>"
            "Current Host IP: 1.2.3.4<br>"
            "Connection State: Error<br>"
            "Last Message IP: 101.104.105.8<br>"
            "Last Message Time: 02/03/2011 14:25:08<br>"
            "<br>"
            "Progressive List<br>"
            "Host: onelinkdiscossl:61614<br>"
            "Host: onelinkdisco:61613<br>"
            "<br>"
            "Progressive Addresses<br>"
            "Host IP: 1.2.3.4<br>"
            "Host IP: 1.2.3.4<br>"
            "Host IP: 1.2.3.4<br>"
            "Host IP: 1.2.3.4<br>"
            "<br>"
            "Last Progressive Award<br>"
            "N/A<br>"
            "<br>"
            "</html>"
        };

        const BYTE bufferExpected9[] = {
            "<html><META Http-equiv=\"refresh\" Content=\"30\">"
            "Startup Time: 01/27/2014 10:11:12 AM<br>"
            "Current Time: 01/27/2014 01:14:15 PM<br>"
            "SlotMast ID: 941<br>"
            "Machine Number:5141<br>"
            "Sentinel Version: 1.12.2.0<br>"
            "OS Image Version: 1.21.0<br>"
            "<br>"
            "Configuration File Present: false<br>"
            "Progressive Configuration: Disabled<br>"
            "DNS Lookup: true<br>"
            "Ping Interval(ms): 6000<br>"
            "<br>"
            "Progressive Connection Info<br>"
            "DNS Hostname: invalid.<br>"
            "Current Host IP: N/A<br>"
            "Connection State: Error<br>"
            "Last Message IP: 101.104.105.9<br>"
            "Last Message Time: 02/03/2011 14:25:09<br>"
            "<br>"
            "Progressive List<br>"
            "Host: abcde.xyz:12345<br>"
            "<br>"
            "Progressive Addresses<br>"
            "<br>"
            "Last Progressive Award<br>"
            "N/A<br>"
            "<br>"
            "</html>"
        };

        const BYTE bufferExpected10[] = {
            "<html><META Http-equiv=\"refresh\" Content=\"30\">"
            "Startup Time: 01/27/2014 10:11:12 AM<br>"
            "Current Time: 01/27/2014 01:14:15 PM<br>"
            "SlotMast ID: 941<br>"
            "Machine Number:5141<br>"
            "Sentinel Version: 1.12.2.0<br>"
            "OS Image Version: 1.21.0<br>"
            "<br>"
            "Configuration File Present: false<br>"
            "Progressive Configuration: Enabled<br>"
            "DNS Lookup: false<br>"
            "Ping Interval(ms): 1234567890<br>"
            "<br>"
            "Progressive Connection Info<br>"
            "DNS Hostname: N/A<br>"
            "Current Host IP: 1.2.3.4<br>"
            "Connection State: Error<br>"
            "Last Message IP: 101.104.105.10<br>"
            "Last Message Time: 02/03/2011 14:25:10<br>"
            "<br>"
            "Progressive List<br>"
            "Host: abcde.xyz:12345<br>"
            "Host:    abcdefghijk.lmnop.xyz:1234567890   <br>"
            "<br>"
            "Progressive Addresses<br>"
            "Host IP: 1.2.3.4<br>"
            "<br>"
            "Last Progressive Award<br>"
            "N/A<br>"
            "<br>"
            "</html>"
        };

        const BYTE bufferExpected11[] = {
            "<html><META Http-equiv=\"refresh\" Content=\"30\">"
            "Startup Time: 01/27/2014 10:11:12 AM<br>"
            "Current Time: 01/27/2014 01:14:15 PM<br>"
            "SlotMast ID: 941<br>"
            "Machine Number:5141<br>"
            "Sentinel Version: 1.12.2.0<br>"
            "OS Image Version: 1.21.0<br>"
            "<br>"
            "Configuration File Present: false<br>"
            "Progressive Configuration: Enabled<br>"
            "DNS Lookup: true<br>"
            "Ping Interval(ms): 4294967295<br>"
            "<br>"
            "Progressive Connection Info<br>"
            "DNS Hostname: localhost.<br>"
            "Current Host IP: 127.0.0.1<br>"
            "Connection State: Error: Discovery Connection<br>"
            "Last Message IP: 101.104.105.11<br>"
            "Last Message Time: 02/03/2011 14:25:11<br>"
            "<br>"
            "Progressive List<br>"
            "Host: abcde.xyz:12345<br>"
            "Host: abcdefghijk.lmnop.xyz:1234567890<br>"
            "Host: !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~:123456789012345678901234567890 <br>"
            "<br>"
            "Progressive Addresses<br>"
            "Host IP: 127.0.0.1<br>"
            "<br>"
            "Last Progressive Award<br>"
            "N/A<br>"
            "<br>"
            "</html>"
        };

        const BYTE bufferExpected12[] = {
            "<html><META Http-equiv=\"refresh\" Content=\"30\">"
            "Startup Time: 01/27/2014 10:11:12 AM<br>"
            "Current Time: 01/27/2014 01:14:15 PM<br>"
            "SlotMast ID: 941<br>"
            "Machine Number:5141<br>"
            "Sentinel Version: 1.12.2.0<br>"
            "OS Image Version: 1.21.0<br>"
            "<br>"
            "Configuration File Present: true<br>"
            "Progressive Configuration: Disabled<br>"
            "DNS Lookup: false<br>"
            "Ping Interval(ms): 15000<br>"
            "<br>"
            "Progressive Connection Info<br>"
            "DNS Hostname: N/A<br>"
            "Current Host IP: 1.2.3.4<br>"
            "Connection State: Error: Message Transmission<br>"
            "Last Message IP: 101.104.105.12<br>"
            "Last Message Time: 02/03/2011 14:25:12<br>"
            "<br>"
            "Progressive List<br>"
            "Host: onelinkdiscossl:61614<br>"
            "Host: onelinkdisco:61613<br>"
            "<br>"
            "Progressive Addresses<br>"
            "Host IP: 1.2.3.4<br>"
            "Host IP: 1.2.3.4<br>"
            "Host IP: 1.2.3.4<br>"
            "<br>"
            "Last Progressive Award<br>"
            "N/A<br>"
            "<br>"
            "</html>"
        };

        const BYTE bufferExpected13[] = {
            "<html><META Http-equiv=\"refresh\" Content=\"30\">"
            "Startup Time: 01/27/2014 10:11:12 AM<br>"
            "Current Time: 01/27/2014 01:14:15 PM<br>"
            "SlotMast ID: 941<br>"
            "Machine Number:5141<br>"
            "Sentinel Version: 1.12.2.0<br>"
            "OS Image Version: 1.21.0<br>"
            "<br>"
            "Configuration File Present: true<br>"
            "Progressive Configuration: Disabled<br>"
            "DNS Lookup: true<br>"
            "Ping Interval(ms): 6000<br>"
            "<br>"
            "Progressive Connection Info<br>"
            "DNS Hostname: invalid.<br>"
            "Current Host IP: N/A<br>"
            "Connection State: Error: Connect Failed<br>"
            "Last Message IP: 101.104.105.13<br>"
            "Last Message Time: 02/03/2011 14:25:13<br>"
            "<br>"
            "Progressive List<br>"
            "Host: abcde.xyz:12345<br>"
            "<br>"
            "Progressive Addresses<br>"
            "<br>"
            "Last Progressive Award<br>"
            "N/A<br>"
            "<br>"
            "</html>"
        };

        const BYTE bufferExpected14[] = {
            "<html><META Http-equiv=\"refresh\" Content=\"30\">"
            "Startup Time: 01/27/2014 10:11:12 AM<br>"
            "Current Time: 01/27/2014 01:14:15 PM<br>"
            "SlotMast ID: 941<br>"
            "Machine Number:5141<br>"
            "Sentinel Version: 1.12.2.0<br>"
            "OS Image Version: 1.21.0<br>"
            "<br>"
            "Configuration File Present: true<br>"
            "Progressive Configuration: Enabled<br>"
            "DNS Lookup: false<br>"
            "Ping Interval(ms): 1234567890<br>"
            "<br>"
            "Progressive Connection Info<br>"
            "DNS Hostname: N/A<br>"
            "Current Host IP: 1.2.3.4<br>"
            "Connection State: Error: Disconnect Failed<br>"
            "Last Message IP: 101.104.105.14<br>"
            "Last Message Time: 02/03/2011 14:25:14<br>"
            "<br>"
            "Progressive List<br>"
            "Host: abcde.xyz:12345<br>"
            "Host:    abcdefghijk.lmnop.xyz:1234567890   <br>"
            "<br>"
            "Progressive Addresses<br>"
            "Host IP: 1.2.3.4<br>"
            "Host IP: 1.2.3.4<br>"
            "Host IP: 1.2.3.4<br>"
            "Host IP: 1.2.3.4<br>"
            "Host IP: 1.2.3.4<br>"
            "<br>"
            "Last Progressive Award<br>"
            "N/A<br>"
            "<br>"
            "</html>"
        };

        const BYTE bufferExpected15[] = {
            "<html><META Http-equiv=\"refresh\" Content=\"30\">"
            "Startup Time: 01/27/2014 10:11:12 AM<br>"
            "Current Time: 01/27/2014 01:14:15 PM<br>"
            "SlotMast ID: 941<br>"
            "Machine Number:5141<br>"
            "Sentinel Version: 1.12.2.0<br>"
            "OS Image Version: 1.21.0<br>"
            "<br>"
            "Configuration File Present: true<br>"
            "Progressive Configuration: Enabled<br>"
            "DNS Lookup: true<br>"
            "Ping Interval(ms): 4294967295<br>"
            "<br>"
            "Progressive Connection Info<br>"
            "DNS Hostname: localhost.<br>"
            "Current Host IP: 127.0.0.1<br>"
            "Connection State: Error<br>"
            "Last Message IP: 101.104.105.15<br>"
            "Last Message Time: 02/03/2011 14:25:15<br>"
            "<br>"
            "Progressive List<br>"
            "Host: abcde.xyz:12345<br>"
            "Host: abcdefghijk.lmnop.xyz:1234567890<br>"
            "Host: !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~:123456789012345678901234567890 <br>"
            "<br>"
            "Progressive Addresses<br>"
            "Host IP: 127.0.0.1<br>"
            "<br>"
            "Last Progressive Award<br>"
            "N/A<br>"
            "<br>"
            "</html>"
        };

        const BYTE bufferExpected16[] = {
            "<html><META Http-equiv=\"refresh\" Content=\"30\">"
            "Startup Time: 01/27/2014 10:11:12 AM<br>"
            "Current Time: 01/27/2014 01:14:15 PM<br>"
            "SlotMast ID: 941<br>"
            "Machine Number:5141<br>"
            "Sentinel Version: 1.12.2.0<br>"
            "OS Image Version: 1.21.0<br>"
            "<br>"
            "Configuration File Present: false<br>"
            "Progressive Configuration: Disabled<br>"
            "DNS Lookup: false<br>"
            "Ping Interval(ms): 15000<br>"
            "<br>"
            "Progressive Connection Info<br>"
            "DNS Hostname: N/A<br>"
            "Current Host IP: 1.2.3.4<br>"
            "Connection State: Error<br>"
            "Last Message IP: 101.104.105.16<br>"
            "Last Message Time: 02/03/2011 14:25:16<br>"
            "<br>"
            "Progressive List<br>"
            "Host: onelinkdiscossl:61614<br>"
            "Host: onelinkdisco:61613<br>"
            "<br>"
            "Progressive Addresses<br>"
            "Host IP: 1.2.3.4<br>"
            "Host IP: 1.2.3.4<br>"
            "<br>"
            "Last Progressive Award<br>"
            "N/A<br>"
            "<br>"
            "</html>"
        };

        const BYTE bufferExpected17[] = {
            "<html><META Http-equiv=\"refresh\" Content=\"30\">"
            "Startup Time: 01/27/2014 10:11:12 AM<br>"
            "Current Time: 01/27/2014 01:14:15 PM<br>"
            "SlotMast ID: 941<br>"
            "Machine Number:5141<br>"
            "Sentinel Version: 1.12.2.0<br>"
            "OS Image Version: 1.21.0<br>"
            "<br>"
            "Configuration File Present: false<br>"
            "Progressive Configuration: Disabled<br>"
            "DNS Lookup: true<br>"
            "Ping Interval(ms): 6000<br>"
            "<br>"
            "Progressive Connection Info<br>"
            "DNS Hostname: invalid.<br>"
            "Current Host IP: N/A<br>"
            "Connection State: Error<br>"
            "Last Message IP: 101.104.105.17<br>"
            "Last Message Time: 02/03/2011 14:25:17<br>"
            "<br>"
            "Progressive List<br>"
            "Host: abcde.xyz:12345<br>"
            "<br>"
            "Progressive Addresses<br>"
            "<br>"
            "Last Progressive Award<br>"
            "N/A<br>"
            "<br>"
            "</html>"
        };

        const BYTE bufferExpected18[] = {
            "<html><META Http-equiv=\"refresh\" Content=\"30\">"
            "Startup Time: 01/27/2014 10:11:12 AM<br>"
            "Current Time: 01/27/2014 01:14:15 PM<br>"
            "SlotMast ID: 941<br>"
            "Machine Number:5141<br>"
            "Sentinel Version: 1.12.2.0<br>"
            "OS Image Version: 1.21.0<br>"
            "<br>"
            "Configuration File Present: false<br>"
            "Progressive Configuration: Enabled<br>"
            "DNS Lookup: false<br>"
            "Ping Interval(ms): 1234567890<br>"
            "<br>"
            "Progressive Connection Info<br>"
            "DNS Hostname: N/A<br>"
            "Current Host IP: 1.2.3.4<br>"
            "Connection State: N/A<br>"
            "Last Message IP: 101.104.105.18<br>"
            "Last Message Time: 02/03/2011 14:25:18<br>"
            "<br>"
            "Progressive List<br>"
            "Host: abcde.xyz:12345<br>"
            "Host:    abcdefghijk.lmnop.xyz:1234567890   <br>"
            "<br>"
            "Progressive Addresses<br>"
            "Host IP: 1.2.3.4<br>"
            "Host IP: 1.2.3.4<br>"
            "Host IP: 1.2.3.4<br>"
            "Host IP: 1.2.3.4<br>"
            "<br>"
            "Last Progressive Award<br>"
            "N/A<br>"
            "<br>"
            "</html>"
        };

        const BYTE bufferExpected19[] = {
            "<html><META Http-equiv=\"refresh\" Content=\"30\">"
            "Startup Time: 01/27/2014 10:11:12 AM<br>"
            "Current Time: 01/27/2014 01:14:15 PM<br>"
            "SlotMast ID: 941<br>"
            "Machine Number:5141<br>"
            "Sentinel Version: 1.12.2.0<br>"
            "OS Image Version: 1.21.0<br>"
            "<br>"
            "Configuration File Present: false<br>"
            "Progressive Configuration: Enabled<br>"
            "DNS Lookup: true<br>"
            "Ping Interval(ms): 4294967295<br>"
            "<br>"
            "Progressive Connection Info<br>"
            "DNS Hostname: localhost.<br>"
            "Current Host IP: 127.0.0.1<br>"
            "Connection State: N/A<br>"
            "Last Message IP: 101.104.105.19<br>"
            "Last Message Time: 02/03/2011 14:25:19<br>"
            "<br>"
            "Progressive List<br>"
            "Host: abcde.xyz:12345<br>"
            "Host: abcdefghijk.lmnop.xyz:1234567890<br>"
            "Host: !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~:123456789012345678901234567890 <br>"
            "<br>"
            "Progressive Addresses<br>"
            "Host IP: 127.0.0.1<br>"
            "<br>"
            "Last Progressive Award<br>"
            "N/A<br>"
            "<br>"
            "</html>"
        };

        const BYTE bufferExpected20[] = {
            "<html><META Http-equiv=\"refresh\" Content=\"30\">"
            "Startup Time: 01/27/2014 10:11:12 AM<br>"
            "Current Time: 01/27/2014 01:14:15 PM<br>"
            "SlotMast ID: 941<br>"
            "Machine Number:5141<br>"
            "Sentinel Version: 1.12.2.0<br>"
            "OS Image Version: 1.21.0<br>"
            "<br>"
            "Configuration File Present: true<br>"
            "Progressive Configuration: Disabled<br>"
            "DNS Lookup: false<br>"
            "Ping Interval(ms): 15000<br>"
            "<br>"
            "Progressive Connection Info<br>"
            "DNS Hostname: N/A<br>"
            "Current Host IP: 1.2.3.4<br>"
            "Connection State: N/A<br>"
            "Last Message IP: 101.104.105.20<br>"
            "Last Message Time: 02/03/2011 14:25:20<br>"
            "<br>"
            "Progressive List<br>"
            "Host: onelinkdiscossl:61614<br>"
            "Host: onelinkdisco:61613<br>"
            "<br>"
            "Progressive Addresses<br>"
            "Host IP: 1.2.3.4<br>"
            "<br>"
            "Last Progressive Award<br>"
            "N/A<br>"
            "<br>"
            "</html>"
        };

        const BYTE bufferExpected21[] = {
            "<html><META Http-equiv=\"refresh\" Content=\"30\">"
            "Startup Time: 01/27/2014 10:11:12 AM<br>"
            "Current Time: 01/27/2014 01:14:15 PM<br>"
            "SlotMast ID: 941<br>"
            "Machine Number:5141<br>"
            "Sentinel Version: 1.12.2.0<br>"
            "OS Image Version: 1.21.0<br>"
            "<br>"
            "Configuration File Present: true<br>"
            "Progressive Configuration: Disabled<br>"
            "DNS Lookup: true<br>"
            "Ping Interval(ms): 6000<br>"
            "<br>"
            "Progressive Connection Info<br>"
            "DNS Hostname: invalid.<br>"
            "Current Host IP: N/A<br>"
            "Connection State: N/A<br>"
            "Last Message IP: 101.104.105.21<br>"
            "Last Message Time: 02/03/2011 14:25:21<br>"
            "<br>"
            "Progressive List<br>"
            "Host: abcde.xyz:12345<br>"
            "<br>"
            "Progressive Addresses<br>"
            "<br>"
            "Last Progressive Award<br>"
            "N/A<br>"
            "<br>"
            "</html>"
        };

        const TestDataType TEST_DATA[] =
        {
            { false, false, false, 0, NULL, 0,
            { _T("101.104.105.0"), 20000 },{ 2011, 2, 0, 3, 14, 25, 0, 789 }, (ProgressiveConnectionState)-1, CouldNotFindServer,
            _T("1.2.3.4:12345"), 10000,
            bufferExpected0, sizeof(bufferExpected0) - 1
            },
            { false, false, true, 6000, endpoints1, _countof(endpoints1),
            { _T("101.104.105.1"), 20001 },{ 2011, 2, 0, 3, 14, 25, 1, 789 }, (ProgressiveConnectionState)(NotStarted - 1), CouldNotFindServer,
            _T("invalid."), 10001,
            bufferExpected1, sizeof(bufferExpected1) - 1
            },
            { false, true, false, 1234567890, endpoints2, _countof(endpoints2),
            { _T("101.104.105.2"), 20002 },{ 2011, 2, 0, 3, 14, 25, 2, 789 }, NotStarted, CouldNotFindServer,
            _T("1.2.3.4:12345"), 50002,
            bufferExpected2, sizeof(bufferExpected2) - 1
            },
            { false, true, true, 4294967295, endpoints3, _countof(endpoints3),
            { _T("101.104.105.3"), 20003 },{ 2011, 2, 0, 3, 14, 25, 3, 789 }, Disabled, CouldNotFindServer,
            _T("localhost."), 10003,
            bufferExpected3, sizeof(bufferExpected3) - 1
            },
            { true, false, false, 0, NULL, 0,
            { _T("101.104.105.4"), 20004 },{ 2011, 2, 0, 3, 14, 25, 4, 789 }, WaitingForInfo, CouldNotFindServer,
            _T("1.2.3.4:12345"), 10004,
            bufferExpected4, sizeof(bufferExpected4) - 1
            },
            { true, false, true, 6000, endpoints1, _countof(endpoints1),
            { _T("101.104.105.5"), 20005 },{ 2011, 2, 0, 3, 14, 25, 5, 789 }, InDiscovery, CouldNotFindServer,
            _T("invalid."), 10005,
            bufferExpected5, sizeof(bufferExpected5) - 1
            },
            { true, true, false, 1234567890, endpoints2, _countof(endpoints2),
            { _T("101.104.105.6"), 20006 },{ 2011, 2, 0, 3, 14, 25, 6, 789 }, Connected, CouldNotFindServer,
            _T("1.2.3.4:12345"), 10006,
            bufferExpected6, sizeof(bufferExpected6) - 1
            },
            { true, true, true, 4294967295, endpoints3, _countof(endpoints3),
            { _T("101.104.105.7"), 20007 },{ 2011, 2, 0, 3, 14, 25, 7, 789 }, Disconnected, CouldNotFindServer,
            _T("localhost."), 10007,
            bufferExpected7, sizeof(bufferExpected7) - 1
            },

            { false, false, false, 0, NULL, 0,
            { _T("101.104.105.8"), 20008 },{ 2011, 2, 0, 3, 14, 25, 8, 789 }, Error, (ProgressiveConnectionError)-1,
            _T("1.2.3.4:12345"), 10008,
            bufferExpected8, sizeof(bufferExpected8) - 1
            },
            { false, false, true, 6000, endpoints1, _countof(endpoints1),
            { _T("101.104.105.9"), 20009 },{ 2011, 2, 0, 3, 14, 25, 9, 789 }, Error, (ProgressiveConnectionError)(ProgressiveConnectionErrorNone - 1),
            _T("invalid."), 10009,
            bufferExpected9, sizeof(bufferExpected9) - 1
            },
            { false, true, false, 1234567890, endpoints2, _countof(endpoints2),
            { _T("101.104.105.10"), 20010 },{ 2011, 2, 0, 3, 14, 25, 10, 789 }, Error, ProgressiveConnectionErrorNone,
            _T("1.2.3.4:12345"), 10010,
            bufferExpected10, sizeof(bufferExpected10) - 1
            },
            { false, true, true, 4294967295, endpoints3, _countof(endpoints3),
            { _T("101.104.105.11"), 20011 },{ 2011, 2, 0, 3, 14, 25, 11, 789 }, Error, DiscoveryConnectionError,
            _T("localhost."), 10011,
            bufferExpected11, sizeof(bufferExpected11) - 1
            },
            { true, false, false, 0, NULL, 0,
            { _T("101.104.105.12"), 20011 },{ 2011, 2, 0, 3, 14, 25, 12, 789 }, Error, MessageTransmissionError,
            _T("1.2.3.4:12345"), 10012,
            bufferExpected12, sizeof(bufferExpected12) - 1
            },
            { true, false, true, 6000, endpoints1, _countof(endpoints1),
            { _T("101.104.105.13"), 20013 },{ 2011, 2, 0, 3, 14, 25, 13, 789 }, Error, ConnectFailed,
            _T("invalid."), 10013,
            bufferExpected13, sizeof(bufferExpected13) - 1
            },
            { true, true, false, 1234567890, endpoints2, _countof(endpoints2),
            { _T("101.104.105.14"), 20014 },{ 2011, 2, 0, 3, 14, 25, 14, 789 }, Error, DisconnectFailed,
            _T("1.2.3.4:12345"), 10014,
            bufferExpected14, sizeof(bufferExpected14) - 1
            },
            { true, true, true, 4294967295, endpoints3, _countof(endpoints3),
            { _T("101.104.105.15"), 20015 },{ 2011, 2, 0, 3, 14, 25, 15, 789 }, Error, (ProgressiveConnectionError)(DisconnectFailed + 1),
            _T("localhost."), 10015,
            bufferExpected15, sizeof(bufferExpected15) - 1
            },
            { false, false, false, 0, NULL, 0,
            { _T("101.104.105.16"), 20016 },{ 2011, 2, 0, 3, 14, 25, 16, 789 }, Error, (ProgressiveConnectionError)INT_MAX,
            _T("1.2.3.4:12345"), 10016,
            bufferExpected16, sizeof(bufferExpected16) - 1
            },
            { false, false, true, 6000, endpoints1, _countof(endpoints1),
            { _T("101.104.105.17"), 20017 },{ 2011, 2, 0, 3, 14, 25, 17, 789 }, Error, (ProgressiveConnectionError)DWORD_MAX,
            _T("invalid."), 10017,
            bufferExpected17, sizeof(bufferExpected17) - 1
            },

            { false, true, false, 1234567890, endpoints2, _countof(endpoints2),
            { _T("101.104.105.18"), 20018 },{ 2011, 2, 0, 3, 14, 25, 18, 789 }, (ProgressiveConnectionState)(Error + 1), CouldNotFindServer,
            _T("1.2.3.4:12345"), 10018,
            bufferExpected18, sizeof(bufferExpected18) - 1
            },
            { false, true, true, 4294967295, endpoints3, _countof(endpoints3),
            { _T("101.104.105.19"), 20019 },{ 2011, 2, 0, 3, 14, 25, 19, 789 }, (ProgressiveConnectionState)INT_MAX, CouldNotFindServer,
            _T("localhost."), 10019,
            bufferExpected19, sizeof(bufferExpected19) - 1
            },
            { true, false, false, 0, NULL, 0,
            { _T("101.104.105.20"), 20020 },{ 2011, 2, 0, 3, 14, 25, 20, 789 }, (ProgressiveConnectionState)INT_MIN, CouldNotFindServer,
            _T("1.2.3.4:12345"), 10020,
            bufferExpected20, sizeof(bufferExpected20) - 1
            },
            { true, false, true, 6000, endpoints1, _countof(endpoints1),
            { _T("101.104.105.21"), 20021 },{ 2011, 2, 0, 3, 14, 25, 21, 789 }, (ProgressiveConnectionState)DWORD_MAX, CouldNotFindServer,
            _T("invalid."), 10021,
            bufferExpected21, sizeof(bufferExpected21) - 1
            },
        };

        // Setup. Set the nCompass time zone to match the expected results.
        CUtilities::SetTzOffset(utilitiesTimeZoneOffset);

        for (int testIndex(0); testIndex < _countof(TEST_DATA); ++testIndex)
        {
            // Setup.
            CProgressiveConfig varProgConfig;
            varProgConfig.SetHasConfigFile(TEST_DATA[testIndex].m_hasConfigFile);
            varProgConfig.SetActive(TEST_DATA[testIndex].m_active);
            varProgConfig.SetDNSLookup(TEST_DATA[testIndex].m_dnsLookup);
            varProgConfig.SetPingInterval(TEST_DATA[testIndex].m_pingInterval);
            for (int index(0); index < TEST_DATA[testIndex].m_endpointCount; ++index)
            {
                varProgConfig.AddEndpoint(TEST_DATA[testIndex].m_endpoints[index]);
            }
            const CProgressiveConfig& progConfig(varProgConfig);

            CIPAddress ipAddress;
            ipAddress.SetIP(TEST_DATA[testIndex].m_lastMessageIP.m_ipAddressString);
            ipAddress.SetPort(TEST_DATA[testIndex].m_lastMessageIP.m_port);
            CProgressiveConnectionInfo varProgInfo;
            varProgInfo.SetLastMessageIP(ipAddress);
            varProgInfo.SetLastMessageTimestamp(TEST_DATA[testIndex].m_lastMessageTimestamp);
            varProgInfo.SetState(TEST_DATA[testIndex].m_state);
            varProgInfo.SetLastError(TEST_DATA[testIndex].m_error);
            const CProgressiveConnectionInfo& progInfo(varProgInfo);

            CProgressiveHost varProgHost(TEST_DATA[testIndex].m_hostName, TEST_DATA[testIndex].m_hostPort);
            if (!TEST_DATA[testIndex].m_dnsLookup)
            {
                CIPAddressArray ipAddressArray;
                for (int index(0); index < (testIndex % 5) + 1; ++index)
                {
                    ipAddressArray.Add(TEST_DATA[testIndex].m_hostName);
                }
                varProgHost = CProgressiveHost(ipAddressArray);
            }
            const CProgressiveHost& progHost(varProgHost);

            const CEGMAward progAward;
            const std::shared_ptr<CEGMAward> lastStandardWin;
            const std::vector<std::shared_ptr<CEGMAward>> standardWins;
            const CPlayerSession playerSession;
            CEGMAwardManagerState progState;

            SYSTEMTIME levelReceivedTime = { 2001, 2, 0, 3, 12, 34, 56, 789 };

            _ICardConfig5 cardConfig;
            shared_ptr<ProgressiveLevelManager> levelManager = cardConfig.CreateLevelManager(nullptr, &levelReceivedTime);

			std::shared_ptr<std::ostream> file = shared_ptr<iostream>(new fstream(dumpFileName, ios_base::out | ios_base::trunc));

            const SYSTEMTIME localTime = { 2015, 7, 5, 10, 12, 0, 0, 0 };

            // Perform operation(s) to be tested.
            CWebDiagnostics::SaveProgressiveData(file,
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
                *levelManager);


            std::string oldHtml(reinterpret_cast<const char *>(TEST_DATA[testIndex].m_bufferExpected), TEST_DATA[testIndex].m_bufferLengthExpected);
            std::string expected(InsertProgressiveDataHtml(oldHtml, playerSession, tickCount, localTime, progState, *levelManager,
                progAward, lastStandardWin, standardWins));

			std::ifstream infile(dumpFileName);
			std::stringstream strStream;
			strStream << infile.rdbuf();
			string filestr = strStream.str();
			bool result = expected.compare(strStream.str());
			AssertResult3(result, true, "ProgressiveData_DefaultTest");
        }
    }

    // #pragma region GetStandardProgressiveWinHtml
    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetStandardProgressiveWinHtml()
    /// Test with nullptr.
    /// </summary>
    TEST_F(WebDiagnostics_Progressive, GetStandardProgressiveWinHtml_NullTest)
    {
        // Setup.
        const std::shared_ptr<const CEGMAward> standardWin(nullptr);

        // Perform operation(s) and test(s).
        ASSERT_EQ(CalcStandardProgressiveWinHtml(standardWin),
            CWebDiagnostics::GetStandardProgressiveWinHtml(standardWin)) <<
            "GetStandardProgressiveWinHtml_NullTest";
    }

    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetStandardProgressiveWinHtml()
    /// Test with default standard win.
    /// </summary>
    TEST_F(WebDiagnostics_Progressive, GetStandardProgressiveWinHtml_DefaultTest)
    {
        // Setup.
        const std::shared_ptr<const CEGMAward> standardWin(new CEGMAward);

        // Perform operation(s) and test(s).
        ASSERT_EQ(CalcStandardProgressiveWinHtml(standardWin),
            CWebDiagnostics::GetStandardProgressiveWinHtml(standardWin)) <<
            "GetStandardProgressiveWinHtml_DefaultTest";
    }

    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetStandardProgressiveWinHtml()
    /// Test standard win with hit received.
    /// </summary>
    TEST_F(WebDiagnostics_Progressive, GetStandardProgressiveWinHtml_HitReceivedTest)
    {
        // Setup.
        std::shared_ptr<CEGMAward> standardWin(new CEGMAward);
        const SYSTEMTIME receivedFromEGMTime = { 2001, 2, 0, 3, 12, 34, 55, 678 };
        standardWin->SetReceivedFromEGMTime(receivedFromEGMTime);
        standardWin->SetSASLevelId(23);
        standardWin->SetLevelId(678);
        standardWin->SetJackpotId(123456789012);
        standardWin->SetAwardAmountPaid(12345);
        standardWin->SetAwardState(ReceivedFromGame);
        standardWin->SetHitReceived(true);
        standardWin->setHandpayPendingSignaled(false);
        standardWin->setEgmPaidSignaled(false);

        // Perform operation(s) and test(s).
        ASSERT_EQ(CalcStandardProgressiveWinHtml(standardWin),
            CWebDiagnostics::GetStandardProgressiveWinHtml(standardWin)) <<
            "GetStandardProgressiveWinHtml_HitReceivedTest";
    }

    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetStandardProgressiveWinHtml()
    /// Test standard win with hit received.
    /// </summary>
    TEST_F(WebDiagnostics_Progressive, GetStandardProgressiveWinHtml_HandpayPendingTest)
    {
        // Setup.
        std::shared_ptr<CEGMAward> standardWin(new CEGMAward);
        const SYSTEMTIME receivedFromEGMTime = { 2001, 2, 0, 3, 12, 34, 55, 678 };
        standardWin->SetReceivedFromEGMTime(receivedFromEGMTime);
        standardWin->SetSASLevelId(23);
        standardWin->SetLevelId(678);
        standardWin->SetJackpotId(123456789012);
        standardWin->SetAwardAmountPaid(12345);
        standardWin->SetAwardState(ReceivedFromGame);
        standardWin->SetHitReceived(false);
        standardWin->setHandpayPendingSignaled(true);
        standardWin->setEgmPaidSignaled(false);

        // Perform operation(s) and test(s).
        ASSERT_EQ(CalcStandardProgressiveWinHtml(standardWin),
            CWebDiagnostics::GetStandardProgressiveWinHtml(standardWin)) <<
            "GetStandardProgressiveWinHtml_HandpayPendingTest";
    }

    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetStandardProgressiveWinHtml()
    /// Test standard win with paid signaled.
    /// </summary>
    TEST_F(WebDiagnostics_Progressive, GetStandardProgressiveWinHtml_PaidSignaledTest)
    {
        // Setup.
        std::shared_ptr<CEGMAward> standardWin(new CEGMAward);
        const SYSTEMTIME receivedFromEGMTime = { 2001, 2, 0, 3, 12, 34, 55, 678 };
        standardWin->SetReceivedFromEGMTime(receivedFromEGMTime);
        standardWin->SetSASLevelId(23);
        standardWin->SetLevelId(678);
        standardWin->SetJackpotId(123456789012);
        standardWin->SetAwardAmountPaid(12345);
        standardWin->SetAwardState(ReceivedFromGame);
        standardWin->SetHitReceived(false);
        standardWin->setHandpayPendingSignaled(false);
        standardWin->setEgmPaidSignaled(true);

        // Perform operation(s) and test(s).
        ASSERT_EQ(CalcStandardProgressiveWinHtml(standardWin),
            CWebDiagnostics::GetStandardProgressiveWinHtml(standardWin)) <<
            "GetStandardProgressiveWinHtml_PaidSignaledTest";
    }
    // #pragma endregion GetStandardProgressiveWinHtml

    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetStandardProgressiveWinsHtml()
    /// Test with empty standard wins vector.
    /// </summary>
    TEST_F(WebDiagnostics_Progressive, GetStandardProgressiveWinsHtml_EmptyTest)
    {
        // Setup.
        const std::vector<std::shared_ptr<CEGMAward>> standardWins;

        // Perform operation(s) and test(s).
        ASSERT_EQ(CalcStandardProgressiveWinsHtml(standardWins),
            CWebDiagnostics::GetStandardProgressiveWinsHtml(standardWins)) <<
            "GetStandardProgressiveWinsHtml_EmptyTest";
    }

    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetStandardProgressiveWinsHtml()
    /// Test with one nullptr element.
    /// </summary>
    TEST_F(WebDiagnostics_Progressive, GetStandardProgressiveWinsHtml_NullTest)
    {
        // Setup.
        std::vector<std::shared_ptr<CEGMAward>> standardWins;
        standardWins.push_back(nullptr);

        // Perform operation(s) and test(s).
        ASSERT_EQ(CalcStandardProgressiveWinsHtml(standardWins),
            CWebDiagnostics::GetStandardProgressiveWinsHtml(standardWins)) <<
            "GetStandardProgressiveWinsHtml_NullTest";
    }

    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetStandardProgressiveWinsHtml()
    /// Test with two nullptr elements.
    /// </summary>
    TEST_F(WebDiagnostics_Progressive, GetStandardProgressiveWinsHtml_NullNullTest)
    {
        // Setup.
        std::vector<std::shared_ptr<CEGMAward>> standardWins;
        standardWins.push_back(nullptr);
        standardWins.push_back(nullptr);

        // Perform operation(s) and test(s).
        ASSERT_EQ(CalcStandardProgressiveWinsHtml(standardWins),
            CWebDiagnostics::GetStandardProgressiveWinsHtml(standardWins)) <<
            "GetStandardProgressiveWinsHtml_NullNullTest";
    }

    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetStandardProgressiveWinsHtml()
    /// Test with one nullptr award element.
    /// </summary>
    TEST_F(WebDiagnostics_Progressive, GetStandardProgressiveWinsHtml_NullAwardTest)
    {
        // Setup.
        std::vector<std::shared_ptr<CEGMAward>> standardWins;
        std::shared_ptr<CEGMAward> standardWin(nullptr);
        standardWins.push_back(standardWin);

        // Perform operation(s) and TEST_F(s).
        ASSERT_EQ(CalcStandardProgressiveWinsHtml(standardWins),
            CWebDiagnostics::GetStandardProgressiveWinsHtml(standardWins)) <<
            "GetStandardProgressiveWinsHtml_NullAwardTest";
    }

    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetStandardProgressiveWinsHtml()
    /// Test with two nullptr award elements.
    /// </summary>
    TEST_F(WebDiagnostics_Progressive, GetStandardProgressiveWinsHtml_NullAwardNullAwardTest)
    {
        // Setup.
        std::vector<std::shared_ptr<CEGMAward>> standardWins;
        std::shared_ptr<CEGMAward> standardWin(nullptr);
        standardWins.push_back(standardWin);
        standardWins.push_back(standardWin);

        // Perform operation(s) and TEST_F(s).
        ASSERT_EQ(CalcStandardProgressiveWinsHtml(standardWins),
            CWebDiagnostics::GetStandardProgressiveWinsHtml(standardWins)) <<
            "GetStandardProgressiveWinsHtml_NullAwardNullAwardTest";
    }

    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetStandardProgressiveWinsHtml()
    /// Test with one default award element.
    /// </summary>
    TEST_F(WebDiagnostics_Progressive, GetStandardProgressiveWinsHtml_DefaultTest)
    {
        // Setup.
        std::vector<std::shared_ptr<CEGMAward>> standardWins;
        std::shared_ptr<CEGMAward> standardWin(new CEGMAward);
        standardWins.push_back(standardWin);

        // Perform operation(s) and test(s).
        ASSERT_EQ(CalcStandardProgressiveWinsHtml(standardWins),
            CWebDiagnostics::GetStandardProgressiveWinsHtml(standardWins)) << 
            "GetStandardProgressiveWinsHtml";
    }

    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetStandardProgressiveWinsHtml()
    /// Test with two default award elements.
    /// </summary>
    TEST_F(WebDiagnostics_Progressive, GetStandardProgressiveWinsHtml_DefaultDefaultTest)
    {
        // Setup.
        std::vector<std::shared_ptr<CEGMAward>> standardWins;
        std::shared_ptr<CEGMAward> standardWin(new CEGMAward);
        standardWins.push_back(standardWin);
        standardWins.push_back(standardWin);

        // Perform operation(s) and test(s).
        ASSERT_EQ(CalcStandardProgressiveWinsHtml(standardWins),
            CWebDiagnostics::GetStandardProgressiveWinsHtml(standardWins)) << 
            "GetStandardProgressiveWinsHtml";
    }


    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetStandardProgressiveWinsHtml()
    /// Test with one non-default award element.
    /// </summary>
    TEST_F(WebDiagnostics_Progressive, GetStandardProgressiveWinsHtml_NonDefaultTest)
    {
        // Setup.
        std::vector<std::shared_ptr<CEGMAward>> standardWins;
        std::shared_ptr<CEGMAward> standardWin(new CEGMAward);
        standardWin->SetAwardState(ReceivedFromGame);
        standardWins.push_back(standardWin);

        // Perform operation(s) and test(s).
        ASSERT_EQ(CalcStandardProgressiveWinsHtml(standardWins),
            CWebDiagnostics::GetStandardProgressiveWinsHtml(standardWins)) 
            << "GetStandardProgressiveWinsHtml";
    }

    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetStandardProgressiveWinsHtml()
    /// Test with two non-default award elements.
    /// </summary>
    TEST_F(WebDiagnostics_Progressive, GetStandardProgressiveWinsHtml_NonDefaultNonDefaultTest)
    {
        // Setup.
        std::vector<std::shared_ptr<CEGMAward>> standardWins;

        std::shared_ptr<CEGMAward> standardWin1(new CEGMAward);
        standardWin1->SetAwardState(ReceivedFromGame);
        standardWins.push_back(standardWin1);

        std::shared_ptr<CEGMAward> standardWin2(new CEGMAward);
        standardWin2->SetAwardState(SentToSystem);
        standardWin2->SetHitReceived(true);
        standardWins.push_back(standardWin2);

        // Perform operation(s) and test(s).
        ASSERT_EQ(CalcStandardProgressiveWinsHtml(standardWins),
            CWebDiagnostics::GetStandardProgressiveWinsHtml(standardWins))
            << "GetStandardProgressiveWinsHtml";
    }
// #pragma endregion GetStandardProgressiveWinsHtml

// #pragma region GetNewestStandardProgressiveWin
    // Possible test cases for GetNewestStandardProgressiveWin.
    // lastStandardWin permutations.
    //    lastStandardWin.get() == nullptr.
    //    lastStandardWin.get() != nullptr && lastStandardWin.get()->GetAwardState() == NoAward)
    //    lastStandardWin.get() != nullptr && lastStandardWin.get()->GetAwardState() != NoAward)
    // standardWins permutations.
    //    standardWins contains 0, 1, or 2 items.
    //    items permutations as above for lastStandardWin permutations.
    //    For 2 items with GetAwardState() != NoAward, date/times in both orders or same date/time.
    //    For items with GetAwardState() != NoAward, date/times in both orders or same date/time compared to lastStandardWin.

    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetNewestStandardProgressiveWin()
    /// Test with lastStandardWin nullptr and empty standardWins.
    /// </summary>
    TEST_F(WebDiagnostics_Progressive, GetNewestStandardProgressiveWin_Null_EmptyTest)
    {
        // Setup.
        std::shared_ptr<CEGMAward> lastStandardWin(nullptr);

        std::vector<std::shared_ptr<CEGMAward>> standardWins;

        // Perform operation(s) and test(s).
        std::shared_ptr<const CEGMAward> newestStandardWin(CWebDiagnostics::GetNewestStandardProgressiveWin(lastStandardWin, standardWins));
        std::shared_ptr<const CEGMAward> calcNewestStandardWin(CalcNewestStandardProgressiveWin(lastStandardWin, standardWins));
        ASSERT_EQ(newestStandardWin.get(), nullptr) << "GetNewestStandardProgressiveWin";
        ASSERT_EQ(calcNewestStandardWin.get(), nullptr) << "CalcNewestStandardProgressiveWin";
    }

    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetNewestStandardProgressiveWin()
    /// Test with lastStandardWin nullptr and standardWins with one nullptr.
    /// </summary>
    TEST_F(WebDiagnostics_Progressive, GetNewestStandardProgressiveWin_Null_NullTest)
    {
        // Setup.
        std::shared_ptr<CEGMAward> lastStandardWin(nullptr);

        std::vector<std::shared_ptr<CEGMAward>> standardWins;
        std::shared_ptr<CEGMAward> standardWin1(nullptr);
        standardWins.push_back(standardWin1);

        // Perform operation(s) and test(s).
        std::shared_ptr<const CEGMAward> newestStandardWin(CWebDiagnostics::GetNewestStandardProgressiveWin(lastStandardWin, standardWins));
        std::shared_ptr<const CEGMAward> calcNewestStandardWin(CalcNewestStandardProgressiveWin(lastStandardWin, standardWins));
        ASSERT_EQ(newestStandardWin.get(), nullptr) << "GetNewestStandardProgressiveWin";
        ASSERT_EQ(calcNewestStandardWin.get(), nullptr) << "CalcNewestStandardProgressiveWin";
    }

    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetNewestStandardProgressiveWin()
    /// Test with lastStandardWin nullptr and standardWins with one NoAward.
    /// </summary>
    TEST_F(WebDiagnostics_Progressive, GetNewestStandardProgressiveWin_Null_NoAwardTest)
    {
        // Setup.
        std::shared_ptr<CEGMAward> lastStandardWin(nullptr);

        std::vector<std::shared_ptr<CEGMAward>> standardWins;
        std::shared_ptr<CEGMAward> standardWin1(new CEGMAward());
        standardWins.push_back(standardWin1);

        // Perform operation(s) and test(s).
        std::shared_ptr<const CEGMAward> newestStandardWin(CWebDiagnostics::GetNewestStandardProgressiveWin(lastStandardWin, standardWins));
        std::shared_ptr<const CEGMAward> calcNewestStandardWin(CalcNewestStandardProgressiveWin(lastStandardWin, standardWins));
        ASSERT_EQ(newestStandardWin.get(), nullptr) << "GetNewestStandardProgressiveWin";
        ASSERT_EQ(calcNewestStandardWin.get(), nullptr) << "CalcNewestStandardProgressiveWin";
    }

    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetNewestStandardProgressiveWin()
    /// Test with lastStandardWin nullptr and standardWins with one Award.
    /// </summary>
    TEST_F(WebDiagnostics_Progressive, GetNewestStandardProgressiveWin_Null_AwardTest)
    {
        // Setup.
        std::shared_ptr<CEGMAward> lastStandardWin(nullptr);

        std::vector<std::shared_ptr<CEGMAward>> standardWins;
        std::shared_ptr<CEGMAward> standardWin1(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTime1 = { 2001, 2, 0, 3, 02, 02, 02, 002 };
        standardWin1->SetReceivedFromEGMTime(receivedFromEGMTime1);
        standardWin1->SetAwardState(ReceivedFromGame);
        standardWins.push_back(standardWin1);

        // Perform operation(s) and test(s).
        std::shared_ptr<const CEGMAward> newestStandardWin(CWebDiagnostics::GetNewestStandardProgressiveWin(lastStandardWin, standardWins));
        std::shared_ptr<const CEGMAward> calcNewestStandardWin(CalcNewestStandardProgressiveWin(lastStandardWin, standardWins));
        ASSERT_EQ((void *)standardWin1.get(), (void *)newestStandardWin.get()) << "GetNewestStandardProgressiveWin";
        ASSERT_EQ((void *)standardWin1.get(), (void *)calcNewestStandardWin.get()) << "CalcNewestStandardProgressiveWin";
    }

    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetNewestStandardProgressiveWin()
    /// Test with lastStandardWin nullptr and standardWins with two Awards newer and older.
    /// </summary>
    TEST_F(WebDiagnostics_Progressive, GetNewestStandardProgressiveWin_Null_AwardNewerOlderTest)
    {
        // Setup.
        std::shared_ptr<CEGMAward> lastStandardWin(nullptr);

        std::vector<std::shared_ptr<CEGMAward>> standardWins;
        std::shared_ptr<CEGMAward> standardWin1(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTime1 = { 2001, 2, 0, 3, 03, 03, 03, 003 };
        standardWin1->SetReceivedFromEGMTime(receivedFromEGMTime1);
        standardWin1->SetAwardState(ReceivedFromGame);
        standardWins.push_back(standardWin1);

        std::shared_ptr<CEGMAward> standardWin2(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTime2 = { 2001, 2, 0, 3, 01, 01, 01, 001 };
        standardWin2->SetReceivedFromEGMTime(receivedFromEGMTime2);
        standardWin2->SetAwardState(ReceivedFromGame);
        standardWins.push_back(standardWin2);

        // Perform operation(s) and test(s).
        std::shared_ptr<const CEGMAward> newestStandardWin(CWebDiagnostics::GetNewestStandardProgressiveWin(lastStandardWin, standardWins));
        std::shared_ptr<const CEGMAward> calcNewestStandardWin(CalcNewestStandardProgressiveWin(lastStandardWin, standardWins));
        ASSERT_EQ((void *)standardWin1.get(), (void *)newestStandardWin.get()) << "GetNewestStandardProgressiveWin";
        ASSERT_EQ((void *)standardWin1.get(), (void *)calcNewestStandardWin.get()) << "CalcNewestStandardProgressiveWin";
    }

    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetNewestStandardProgressiveWin()
    /// Test with lastStandardWin nullptr and standardWins with two Awards with the same newer date/times.
    /// </summary>
    TEST_F(WebDiagnostics_Progressive, GetNewestStandardProgressiveWin_Null_AwardNewerNewerTest)
    {
        // Setup.
        std::shared_ptr<CEGMAward> lastStandardWin(nullptr);

        std::vector<std::shared_ptr<CEGMAward>> standardWins;
        std::shared_ptr<CEGMAward> standardWin1(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTime1 = { 2001, 2, 0, 3, 03, 03, 03, 003 };
        standardWin1->SetReceivedFromEGMTime(receivedFromEGMTime1);
        standardWin1->SetAwardState(ReceivedFromGame);
        standardWins.push_back(standardWin1);

        std::shared_ptr<CEGMAward> standardWin2(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTime2 = { 2001, 2, 0, 3, 03, 03, 03, 003 };
        standardWin2->SetReceivedFromEGMTime(receivedFromEGMTime2);
        standardWin2->SetAwardState(ReceivedFromGame);
        standardWins.push_back(standardWin2);

        // Perform operation(s) and test(s).
        std::shared_ptr<const CEGMAward> newestStandardWin(CWebDiagnostics::GetNewestStandardProgressiveWin(lastStandardWin, standardWins));
        std::shared_ptr<const CEGMAward> calcNewestStandardWin(CalcNewestStandardProgressiveWin(lastStandardWin, standardWins));
        ASSERT_EQ((void *)standardWin2.get(), (void *)newestStandardWin.get()) << "GetNewestStandardProgressiveWin";
        ASSERT_EQ((void *)standardWin2.get(), (void *)calcNewestStandardWin.get()) << "CalcNewestStandardProgressiveWin";
    }

    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetNewestStandardProgressiveWin()
    /// Test with lastStandardWin nullptr and standardWins with two Awards older and newer.
    /// </summary>
    TEST_F(WebDiagnostics_Progressive, GetNewestStandardProgressiveWin_Null_AwardOlderNewerTest)
    {
        // Setup.
        std::shared_ptr<CEGMAward> lastStandardWin(nullptr);

        std::vector<std::shared_ptr<CEGMAward>> standardWins;
        std::shared_ptr<CEGMAward> standardWin1(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTime1 = { 2001, 2, 0, 3, 01, 01, 01, 001 };
        standardWin1->SetReceivedFromEGMTime(receivedFromEGMTime1);
        standardWin1->SetAwardState(ReceivedFromGame);
        standardWins.push_back(standardWin1);

        std::shared_ptr<CEGMAward> standardWin2(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTime2 = { 2001, 2, 0, 3, 03, 03, 03, 003 };
        standardWin2->SetReceivedFromEGMTime(receivedFromEGMTime2);
        standardWin2->SetAwardState(ReceivedFromGame);
        standardWins.push_back(standardWin2);

        // Perform operation(s) and test(s).
        std::shared_ptr<const CEGMAward> newestStandardWin(CWebDiagnostics::GetNewestStandardProgressiveWin(lastStandardWin, standardWins));
        std::shared_ptr<const CEGMAward> calcNewestStandardWin(CalcNewestStandardProgressiveWin(lastStandardWin, standardWins));
        ASSERT_EQ((void *)standardWin2.get(), (void *)newestStandardWin.get()) << "GetNewestStandardProgressiveWin";
        ASSERT_EQ((void *)standardWin2.get(), (void *)calcNewestStandardWin.get()) << "CalcNewestStandardProgressiveWin";
    }

    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetNewestStandardProgressiveWin()
    /// Test with lastStandardWin=NoAward and empty standardWins.
    /// </summary>
    TEST_F(WebDiagnostics_Progressive, GetNewestStandardProgressiveWin_NoAward_EmptyTest)
    {
        // Setup.
        std::shared_ptr<CEGMAward> lastStandardWin(new CEGMAward());
        std::vector<std::shared_ptr<CEGMAward>> standardWins;

        // Perform operation(s) and test(s).
        std::shared_ptr<const CEGMAward> newestStandardWin(CWebDiagnostics::GetNewestStandardProgressiveWin(lastStandardWin, standardWins));
        std::shared_ptr<const CEGMAward> calcNewestStandardWin(CalcNewestStandardProgressiveWin(lastStandardWin, standardWins));
        ASSERT_EQ((void *)lastStandardWin.get(), (void *)newestStandardWin.get()) << "GetNewestStandardProgressiveWin";
        ASSERT_EQ((void *)lastStandardWin.get(), (void *)calcNewestStandardWin.get()) <<  "CalcNewestStandardProgressiveWin";
    }

    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetNewestStandardProgressiveWin()
    /// Test with lastStandardWin=NoAward and standardWins with one nullptr.
    /// </summary>
    TEST_F(WebDiagnostics_Progressive, GetNewestStandardProgressiveWin_NoAward_NullTest)
    {
        // Setup.
        std::shared_ptr<CEGMAward> lastStandardWin(new CEGMAward());

        std::vector<std::shared_ptr<CEGMAward>> standardWins;
        std::shared_ptr<CEGMAward> standardWin1(nullptr);
        standardWins.push_back(standardWin1);

        // Perform operation(s) and test(s).
        std::shared_ptr<const CEGMAward> newestStandardWin(CWebDiagnostics::GetNewestStandardProgressiveWin(lastStandardWin, standardWins));
        std::shared_ptr<const CEGMAward> calcNewestStandardWin(CalcNewestStandardProgressiveWin(lastStandardWin, standardWins));
        ASSERT_EQ((void *)lastStandardWin.get(), (void *)newestStandardWin.get() ) << "GetNewestStandardProgressiveWin";
        ASSERT_EQ((void *)lastStandardWin.get(), (void *)calcNewestStandardWin.get()) << "CalcNewestStandardProgressiveWin";
    }

    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetNewestStandardProgressiveWin()
    /// Test with lastStandardWin=NoAward and standardWins with one NoAward.
    /// </summary>
    TEST_F(WebDiagnostics_Progressive, GetNewestStandardProgressiveWin_NoAward_NoAwardTest)
    {
        // Setup.
        std::shared_ptr<CEGMAward> lastStandardWin(new CEGMAward());

        std::vector<std::shared_ptr<CEGMAward>> standardWins;
        std::shared_ptr<CEGMAward> standardWin1(new CEGMAward());
        standardWins.push_back(standardWin1);

        // Perform operation(s) and test(s).
        std::shared_ptr<const CEGMAward> newestStandardWin(CWebDiagnostics::GetNewestStandardProgressiveWin(lastStandardWin, standardWins));
        std::shared_ptr<const CEGMAward> calcNewestStandardWin(CalcNewestStandardProgressiveWin(lastStandardWin, standardWins));
        ASSERT_EQ((void *)lastStandardWin.get(), (void *)newestStandardWin.get()) << "GetNewestStandardProgressiveWin";
        ASSERT_EQ((void *)lastStandardWin.get(), (void *)calcNewestStandardWin.get()) << "CalcNewestStandardProgressiveWin";
    }

    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetNewestStandardProgressiveWin()
    /// Test with lastStandardWin=NoAward and standardWins with one Award.
    /// </summary>
    TEST_F(WebDiagnostics_Progressive, GetNewestStandardProgressiveWin_NoAward_AwardTest)
    {
        // Setup.
        std::shared_ptr<CEGMAward> lastStandardWin(new CEGMAward());

        std::vector<std::shared_ptr<CEGMAward>> standardWins;
        std::shared_ptr<CEGMAward> standardWin1(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTime1 = { 2001, 2, 0, 3, 02, 02, 02, 002 };
        standardWin1->SetReceivedFromEGMTime(receivedFromEGMTime1);
        standardWin1->SetAwardState(ReceivedFromGame);
        standardWins.push_back(standardWin1);

        // Perform operation(s) and test(s).
        std::shared_ptr<const CEGMAward> newestStandardWin(CWebDiagnostics::GetNewestStandardProgressiveWin(lastStandardWin, standardWins));
        std::shared_ptr<const CEGMAward> calcNewestStandardWin(CalcNewestStandardProgressiveWin(lastStandardWin, standardWins));
        ASSERT_EQ((void *)standardWin1.get(), (void *)newestStandardWin.get()) << "GetNewestStandardProgressiveWin";
        ASSERT_EQ((void *)standardWin1.get(), (void *)calcNewestStandardWin.get()) <<  "CalcNewestStandardProgressiveWin";
    }

    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetNewestStandardProgressiveWin()
    /// Test with lastStandardWin=NoAward and standardWins with two Awards newer and older.
    /// </summary>
    TEST_F(WebDiagnostics_Progressive, GetNewestStandardProgressiveWin_NoAward_AwardNewerOlderTest)
    {
        // Setup.
        std::shared_ptr<CEGMAward> lastStandardWin(new CEGMAward());

        std::vector<std::shared_ptr<CEGMAward>> standardWins;
        std::shared_ptr<CEGMAward> standardWin1(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTime1 = { 2001, 2, 0, 3, 03, 03, 03, 003 };
        standardWin1->SetReceivedFromEGMTime(receivedFromEGMTime1);
        standardWin1->SetAwardState(ReceivedFromGame);
        standardWins.push_back(standardWin1);

        std::shared_ptr<CEGMAward> standardWin2(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTime2 = { 2001, 2, 0, 3, 01, 01, 01, 001 };
        standardWin2->SetReceivedFromEGMTime(receivedFromEGMTime2);
        standardWin2->SetAwardState(ReceivedFromGame);
        standardWins.push_back(standardWin2);

        // Perform operation(s) and test(s).
        std::shared_ptr<const CEGMAward> newestStandardWin(CWebDiagnostics::GetNewestStandardProgressiveWin(lastStandardWin, standardWins));
        std::shared_ptr<const CEGMAward> calcNewestStandardWin(CalcNewestStandardProgressiveWin(lastStandardWin, standardWins));
        ASSERT_EQ((void *)standardWin1.get(), (void *)newestStandardWin.get()) << "GetNewestStandardProgressiveWin";
        ASSERT_EQ((void *)standardWin1.get(), (void *)calcNewestStandardWin.get()) <<"CalcNewestStandardProgressiveWin";
    }

    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetNewestStandardProgressiveWin()
    /// Test with lastStandardWin=NoAward and standardWins with two Awards with the same newer date/times.
    /// </summary>
    TEST_F(WebDiagnostics_Progressive, GetNewestStandardProgressiveWin_NoAward_AwardNewerNewerTest)
    {
        // Setup.
        std::shared_ptr<CEGMAward> lastStandardWin(new CEGMAward());

        std::vector<std::shared_ptr<CEGMAward>> standardWins;
        std::shared_ptr<CEGMAward> standardWin1(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTime1 = { 2001, 2, 0, 3, 03, 03, 03, 003 };
        standardWin1->SetReceivedFromEGMTime(receivedFromEGMTime1);
        standardWin1->SetAwardState(ReceivedFromGame);
        standardWins.push_back(standardWin1);

        std::shared_ptr<CEGMAward> standardWin2(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTime2 = { 2001, 2, 0, 3, 03, 03, 03, 003 };
        standardWin2->SetReceivedFromEGMTime(receivedFromEGMTime2);
        standardWin2->SetAwardState(ReceivedFromGame);
        standardWins.push_back(standardWin2);

        // Perform operation(s) and test(s).
        std::shared_ptr<const CEGMAward> newestStandardWin(CWebDiagnostics::GetNewestStandardProgressiveWin(lastStandardWin, standardWins));
        std::shared_ptr<const CEGMAward> calcNewestStandardWin(CalcNewestStandardProgressiveWin(lastStandardWin, standardWins));
        ASSERT_EQ((void *)standardWin2.get(), (void *)newestStandardWin.get()) << "GetNewestStandardProgressiveWin";
        ASSERT_EQ((void *)standardWin2.get(), (void *)calcNewestStandardWin.get()) <<  "CalcNewestStandardProgressiveWin";
    }

    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetNewestStandardProgressiveWin()
    /// Test with lastStandardWin=NoAward and standardWins with two Awards older and newer.
    /// </summary>
    TEST_F(WebDiagnostics_Progressive, GetNewestStandardProgressiveWin_NoAward_AwardOlderNewerTest)
    {
        // Setup.
        std::shared_ptr<CEGMAward> lastStandardWin(new CEGMAward());

        std::vector<std::shared_ptr<CEGMAward>> standardWins;
        std::shared_ptr<CEGMAward> standardWin1(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTime1 = { 2001, 2, 0, 3, 01, 01, 01, 001 };
        standardWin1->SetReceivedFromEGMTime(receivedFromEGMTime1);
        standardWin1->SetAwardState(ReceivedFromGame);
        standardWins.push_back(standardWin1);

        std::shared_ptr<CEGMAward> standardWin2(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTime2 = { 2001, 2, 0, 3, 03, 03, 03, 003 };
        standardWin2->SetReceivedFromEGMTime(receivedFromEGMTime2);
        standardWin2->SetAwardState(ReceivedFromGame);
        standardWins.push_back(standardWin2);

        // Perform operation(s) and test(s).
        std::shared_ptr<const CEGMAward> newestStandardWin(CWebDiagnostics::GetNewestStandardProgressiveWin(lastStandardWin, standardWins));
        std::shared_ptr<const CEGMAward> calcNewestStandardWin(CalcNewestStandardProgressiveWin(lastStandardWin, standardWins));
        ASSERT_EQ((void *)standardWin2.get(), (void *)newestStandardWin.get()) << "GetNewestStandardProgressiveWin";
        ASSERT_EQ((void *)standardWin2.get(), (void *)calcNewestStandardWin.get()) <<  "CalcNewestStandardProgressiveWin";
    }

    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetNewestStandardProgressiveWin()
    /// Test with lastStandardWin=Award and empty standardWins.
    /// </summary>
    TEST_F(WebDiagnostics_Progressive, GetNewestStandardProgressiveWin_Award_EmptyTest)
    {
        // Setup.
        std::shared_ptr<CEGMAward> lastStandardWin(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTime = { 2001, 2, 0, 3, 02, 02, 02, 002 };
        lastStandardWin->SetReceivedFromEGMTime(receivedFromEGMTime);
        lastStandardWin->SetAwardState(ReceivedFromGame);
        lastStandardWin->setEgmPaidSignaled(true);

        std::vector<std::shared_ptr<CEGMAward>> standardWins;

        // Perform operation(s) and test(s).
        std::shared_ptr<const CEGMAward> newestStandardWin(CWebDiagnostics::GetNewestStandardProgressiveWin(lastStandardWin, standardWins));
        std::shared_ptr<const CEGMAward> calcNewestStandardWin(CalcNewestStandardProgressiveWin(lastStandardWin, standardWins));
        ASSERT_EQ((void *)lastStandardWin.get(), (void *)newestStandardWin.get()) << "GetNewestStandardProgressiveWin";
        ASSERT_EQ((void *)lastStandardWin.get(), (void *)calcNewestStandardWin.get()) << "CalcNewestStandardProgressiveWin";
    }

    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetNewestStandardProgressiveWin()
    /// Test with lastStandardWin=Award and standardWins with one nullptr.
    /// </summary>
    TEST_F(WebDiagnostics_Progressive, GetNewestStandardProgressiveWin_Award_NullTest)
    {
        // Setup.
        std::shared_ptr<CEGMAward> lastStandardWin(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTime = { 2001, 2, 0, 3, 02, 02, 02, 002 };
        lastStandardWin->SetReceivedFromEGMTime(receivedFromEGMTime);
        lastStandardWin->SetAwardState(ReceivedFromGame);
        lastStandardWin->setEgmPaidSignaled(true);

        std::vector<std::shared_ptr<CEGMAward>> standardWins;
        std::shared_ptr<CEGMAward> standardWin1(nullptr);
        standardWins.push_back(standardWin1);

        // Perform operation(s) and test(s).
        std::shared_ptr<const CEGMAward> newestStandardWin(CWebDiagnostics::GetNewestStandardProgressiveWin(lastStandardWin, standardWins));
        std::shared_ptr<const CEGMAward> calcNewestStandardWin(CalcNewestStandardProgressiveWin(lastStandardWin, standardWins));
        ASSERT_EQ((void *)lastStandardWin.get(), (void *)newestStandardWin.get()) << "GetNewestStandardProgressiveWin";
        ASSERT_EQ((void *)lastStandardWin.get(), (void *)calcNewestStandardWin.get()) << "CalcNewestStandardProgressiveWin";
    }

    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetNewestStandardProgressiveWin()
    /// Test with lastStandardWin=Award and standardWins with one NoAward.
    /// </summary>
    TEST_F(WebDiagnostics_Progressive, GetNewestStandardProgressiveWin_Award_NoAwardTest)
    {
        // Setup.
        std::shared_ptr<CEGMAward> lastStandardWin(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTime = { 2001, 2, 0, 3, 02, 02, 02, 002 };
        lastStandardWin->SetReceivedFromEGMTime(receivedFromEGMTime);
        lastStandardWin->SetAwardState(ReceivedFromGame);
        lastStandardWin->setEgmPaidSignaled(true);

        std::vector<std::shared_ptr<CEGMAward>> standardWins;
        std::shared_ptr<CEGMAward> standardWin1(new CEGMAward());
        standardWins.push_back(standardWin1);

        // Perform operation(s) and test(s).
        std::shared_ptr<const CEGMAward> newestStandardWin(CWebDiagnostics::GetNewestStandardProgressiveWin(lastStandardWin, standardWins));
        std::shared_ptr<const CEGMAward> calcNewestStandardWin(CalcNewestStandardProgressiveWin(lastStandardWin, standardWins));
        ASSERT_EQ((void *)lastStandardWin.get(), (void *)newestStandardWin.get() )<< "GetNewestStandardProgressiveWin";
        ASSERT_EQ((void *)lastStandardWin.get(), (void *)calcNewestStandardWin.get()) << "CalcNewestStandardProgressiveWin";
    }

    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetNewestStandardProgressiveWin()
    /// Test with lastStandardWin=Award and standardWins with one Award with an older time.
    /// </summary>
    TEST_F(WebDiagnostics_Progressive, GetNewestStandardProgressiveWin_Award_AwardOlderTest)
    {
        // Setup.
        std::shared_ptr<CEGMAward> lastStandardWin(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTime = { 2001, 2, 0, 3, 02, 02, 02, 002 };
        lastStandardWin->SetReceivedFromEGMTime(receivedFromEGMTime);
        lastStandardWin->SetAwardState(ReceivedFromGame);
        lastStandardWin->setEgmPaidSignaled(true);

        std::vector<std::shared_ptr<CEGMAward>> standardWins;
        std::shared_ptr<CEGMAward> standardWin1(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTime1 = { 2001, 2, 0, 3, 01, 01, 01, 001 };
        standardWin1->SetReceivedFromEGMTime(receivedFromEGMTime1);
        standardWin1->SetAwardState(ReceivedFromGame);
        standardWins.push_back(standardWin1);

        // Perform operation(s) and test(s).
        std::shared_ptr<const CEGMAward> newestStandardWin(CWebDiagnostics::GetNewestStandardProgressiveWin(lastStandardWin, standardWins));
        std::shared_ptr<const CEGMAward> calcNewestStandardWin(CalcNewestStandardProgressiveWin(lastStandardWin, standardWins));
        ASSERT_EQ((void *)lastStandardWin.get(), (void *)newestStandardWin.get()) << "GetNewestStandardProgressiveWin";
        ASSERT_EQ((void *)lastStandardWin.get(), (void *)calcNewestStandardWin.get()) <<  L"CalcNewestStandardProgressiveWin";
    }

    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetNewestStandardProgressiveWin()
    /// Test with lastStandardWin=Award and standardWins with one Award with the same time.
    /// </summary>
    TEST_F(WebDiagnostics_Progressive, GetNewestStandardProgressiveWin_Award_AwardSameTest)
    {
        // Setup.
        std::shared_ptr<CEGMAward> lastStandardWin(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTime = { 2001, 2, 0, 3, 02, 02, 02, 002 };
        lastStandardWin->SetReceivedFromEGMTime(receivedFromEGMTime);
        lastStandardWin->SetAwardState(ReceivedFromGame);
        lastStandardWin->setEgmPaidSignaled(true);

        std::vector<std::shared_ptr<CEGMAward>> standardWins;
        std::shared_ptr<CEGMAward> standardWin1(new CEGMAward());
        standardWin1->SetReceivedFromEGMTime(receivedFromEGMTime);
        standardWin1->SetAwardState(ReceivedFromGame);
        standardWins.push_back(standardWin1);

        // Perform operation(s) and test(s).
        std::shared_ptr<const CEGMAward> newestStandardWin(CWebDiagnostics::GetNewestStandardProgressiveWin(lastStandardWin, standardWins));
        std::shared_ptr<const CEGMAward> calcNewestStandardWin(CalcNewestStandardProgressiveWin(lastStandardWin, standardWins));
        ASSERT_EQ((void *)standardWin1.get(), (void *)newestStandardWin.get()) << "GetNewestStandardProgressiveWin";
        ASSERT_EQ((void *)standardWin1.get(), (void *)calcNewestStandardWin.get()) << "CalcNewestStandardProgressiveWin";
    }

    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetNewestStandardProgressiveWin()
    /// Test with lastStandardWin=Award and standardWins with one Award with an newer time.
    /// </summary>
    TEST_F(WebDiagnostics_Progressive, GetNewestStandardProgressiveWin_Award_AwardNewerTest)
    {
        // Setup.
        std::shared_ptr<CEGMAward> lastStandardWin(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTime = { 2001, 2, 0, 3, 02, 02, 02, 002 };
        lastStandardWin->SetReceivedFromEGMTime(receivedFromEGMTime);
        lastStandardWin->SetAwardState(ReceivedFromGame);
        lastStandardWin->setEgmPaidSignaled(true);

        std::vector<std::shared_ptr<CEGMAward>> standardWins;
        std::shared_ptr<CEGMAward> standardWin1(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTime1 = { 2001, 2, 0, 3, 03, 03, 03, 003 };
        standardWin1->SetReceivedFromEGMTime(receivedFromEGMTime1);
        standardWin1->SetAwardState(ReceivedFromGame);
        standardWins.push_back(standardWin1);

        // Perform operation(s) and test(s).
        std::shared_ptr<const CEGMAward> newestStandardWin(CWebDiagnostics::GetNewestStandardProgressiveWin(lastStandardWin, standardWins));
        std::shared_ptr<const CEGMAward> calcNewestStandardWin(CalcNewestStandardProgressiveWin(lastStandardWin, standardWins));
        ASSERT_EQ((void *)standardWin1.get(), (void *)newestStandardWin.get()) << "GetNewestStandardProgressiveWin";
        ASSERT_EQ((void *)standardWin1.get(), (void *)calcNewestStandardWin.get()) <<  "CalcNewestStandardProgressiveWin";
    }

     /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetNewestStandardProgressiveWin()
    /// Test with lastStandardWin=Award and standardWins with two Awards oldest and older.
    /// </summary>
    TEST_F(WebDiagnostics_Progressive, GetNewestStandardProgressiveWin_Award_AwardOldestOlderTest)
    {
        // Setup.
        std::shared_ptr<CEGMAward> lastStandardWin(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTime = { 2001, 2, 0, 3, 02, 02, 02, 002 };
        lastStandardWin->SetReceivedFromEGMTime(receivedFromEGMTime);
        lastStandardWin->SetAwardState(ReceivedFromGame);
        lastStandardWin->setEgmPaidSignaled(true);

        std::vector<std::shared_ptr<CEGMAward>> standardWins;
        std::shared_ptr<CEGMAward> standardWin1(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTime1 = { 2001, 2, 0, 3, 00, 00, 00, 000 };
        standardWin1->SetReceivedFromEGMTime(receivedFromEGMTime1);
        standardWin1->SetAwardState(ReceivedFromGame);
        standardWins.push_back(standardWin1);

        std::shared_ptr<CEGMAward> standardWin2(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTime2 = { 2001, 2, 0, 3, 01, 01, 01, 001 };
        standardWin2->SetReceivedFromEGMTime(receivedFromEGMTime2);
        standardWin2->SetAwardState(ReceivedFromGame);
        standardWins.push_back(standardWin2);

        // Perform operation(s) and test(s).
        std::shared_ptr<const CEGMAward> newestStandardWin(CWebDiagnostics::GetNewestStandardProgressiveWin(lastStandardWin, standardWins));
        std::shared_ptr<const CEGMAward> calcNewestStandardWin(CalcNewestStandardProgressiveWin(lastStandardWin, standardWins));
        ASSERT_EQ((void *)lastStandardWin.get(), (void *)newestStandardWin.get()) << "GetNewestStandardProgressiveWin";
        ASSERT_EQ((void *)lastStandardWin.get(), (void *)calcNewestStandardWin.get()) << "CalcNewestStandardProgressiveWin";
    }

    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetNewestStandardProgressiveWin()
    /// Test with lastStandardWin=Award and standardWins with two Awards same date as lastStandardWin and older.
    /// </summary>
    TEST_F(WebDiagnostics_Progressive, GetNewestStandardProgressiveWin_Award_AwardSameOlderTest)
    {
        // Setup.
        std::shared_ptr<CEGMAward> lastStandardWin(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTime = { 2001, 2, 0, 3, 02, 02, 02, 002 };
        lastStandardWin->SetReceivedFromEGMTime(receivedFromEGMTime);
        lastStandardWin->SetAwardState(ReceivedFromGame);
        lastStandardWin->setEgmPaidSignaled(true);

        std::vector<std::shared_ptr<CEGMAward>> standardWins;
        std::shared_ptr<CEGMAward> standardWin1(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTime1 = receivedFromEGMTime;
        standardWin1->SetReceivedFromEGMTime(receivedFromEGMTime1);
        standardWin1->SetAwardState(ReceivedFromGame);
        standardWins.push_back(standardWin1);

        std::shared_ptr<CEGMAward> standardWin2(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTime2 = { 2001, 2, 0, 3, 01, 01, 01, 001 };
        standardWin2->SetReceivedFromEGMTime(receivedFromEGMTime2);
        standardWin2->SetAwardState(ReceivedFromGame);
        standardWins.push_back(standardWin2);

        // Perform operation(s) and test(s).
        std::shared_ptr<const CEGMAward> newestStandardWin(CWebDiagnostics::GetNewestStandardProgressiveWin(lastStandardWin, standardWins));
        std::shared_ptr<const CEGMAward> calcNewestStandardWin(CalcNewestStandardProgressiveWin(lastStandardWin, standardWins));
        ASSERT_EQ((void *)standardWin1.get(), (void *)newestStandardWin.get()) << "GetNewestStandardProgressiveWin";
        ASSERT_EQ((void *)standardWin1.get(), (void *)calcNewestStandardWin.get()) << "CalcNewestStandardProgressiveWin";
    }

    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetNewestStandardProgressiveWin()
    /// Test with lastStandardWin=Award and standardWins with two Awards oldest and older.
    /// </summary>
    TEST_F(WebDiagnostics_Progressive, GetNewestStandardProgressiveWin_Award_AwardOlderSameTest)
    {
        // Setup.
        std::shared_ptr<CEGMAward> lastStandardWin(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTime = { 2001, 2, 0, 3, 02, 02, 02, 002 };
        lastStandardWin->SetReceivedFromEGMTime(receivedFromEGMTime);
        lastStandardWin->SetAwardState(ReceivedFromGame);
        lastStandardWin->setEgmPaidSignaled(true);

        std::vector<std::shared_ptr<CEGMAward>> standardWins;
        std::shared_ptr<CEGMAward> standardWin1(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTime1 = { 2001, 2, 0, 3, 00, 00, 00, 000 };
        standardWin1->SetReceivedFromEGMTime(receivedFromEGMTime1);
        standardWin1->SetAwardState(ReceivedFromGame);
        standardWins.push_back(standardWin1);

        std::shared_ptr<CEGMAward> standardWin2(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTime2 = receivedFromEGMTime;
        standardWin2->SetReceivedFromEGMTime(receivedFromEGMTime2);
        standardWin2->SetAwardState(ReceivedFromGame);
        standardWins.push_back(standardWin2);

        // Perform operation(s) and test(s).
        std::shared_ptr<const CEGMAward> newestStandardWin(CWebDiagnostics::GetNewestStandardProgressiveWin(lastStandardWin, standardWins));
        std::shared_ptr<const CEGMAward> calcNewestStandardWin(CalcNewestStandardProgressiveWin(lastStandardWin, standardWins));
        ASSERT_EQ((void *)standardWin2.get(), (void *)newestStandardWin.get()) << "GetNewestStandardProgressiveWin";
        ASSERT_EQ((void *)standardWin2.get(), (void *)calcNewestStandardWin.get()) << "CalcNewestStandardProgressiveWin";
    }

    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetNewestStandardProgressiveWin()
    /// Test with lastStandardWin=Award and standardWins with two Awards both same date as lastStandardWin.
    /// </summary>
    TEST_F(WebDiagnostics_Progressive, GetNewestStandardProgressiveWin_Award_AwardSameSameTest)
    {
        // Setup.
        std::shared_ptr<CEGMAward> lastStandardWin(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTime = { 2001, 2, 0, 3, 02, 02, 02, 002 };
        lastStandardWin->SetReceivedFromEGMTime(receivedFromEGMTime);
        lastStandardWin->SetAwardState(ReceivedFromGame);
        lastStandardWin->setEgmPaidSignaled(true);

        std::vector<std::shared_ptr<CEGMAward>> standardWins;
        std::shared_ptr<CEGMAward> standardWin1(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTime1 = receivedFromEGMTime;
        standardWin1->SetReceivedFromEGMTime(receivedFromEGMTime1);
        standardWin1->SetAwardState(ReceivedFromGame);
        standardWins.push_back(standardWin1);

        std::shared_ptr<CEGMAward> standardWin2(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTime2 = receivedFromEGMTime;
        standardWin2->SetReceivedFromEGMTime(receivedFromEGMTime2);
        standardWin2->SetAwardState(ReceivedFromGame);
        standardWins.push_back(standardWin2);

        // Perform operation(s) and test(s).
        std::shared_ptr<const CEGMAward> newestStandardWin(CWebDiagnostics::GetNewestStandardProgressiveWin(lastStandardWin, standardWins));
        std::shared_ptr<const CEGMAward> calcNewestStandardWin(CalcNewestStandardProgressiveWin(lastStandardWin, standardWins));
        ASSERT_EQ((void *)standardWin2.get(), (void *)newestStandardWin.get()) << "GetNewestStandardProgressiveWin";
        ASSERT_EQ((void *)standardWin2.get(), (void *)calcNewestStandardWin.get()) << "CalcNewestStandardProgressiveWin";
    }

    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetNewestStandardProgressiveWin()
    /// Test with lastStandardWin=Award and standardWins with two Awards with older and newer date/times.
    /// </summary>
    TEST_F(WebDiagnostics_Progressive, GetNewestStandardProgressiveWin_Award_AwardOlderNewerTest)
    {
        // Setup.
        std::shared_ptr<CEGMAward> lastStandardWin(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTime = { 2001, 2, 0, 3, 02, 02, 02, 002 };
        lastStandardWin->SetReceivedFromEGMTime(receivedFromEGMTime);
        lastStandardWin->SetAwardState(ReceivedFromGame);
        lastStandardWin->setEgmPaidSignaled(true);

        std::vector<std::shared_ptr<CEGMAward>> standardWins;
        std::shared_ptr<CEGMAward> standardWin1(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTime1 = { 2001, 2, 0, 3, 01, 01, 01, 001 };
        standardWin1->SetReceivedFromEGMTime(receivedFromEGMTime1);
        standardWin1->SetAwardState(ReceivedFromGame);
        standardWins.push_back(standardWin1);

        std::shared_ptr<CEGMAward> standardWin2(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTime2 = { 2001, 2, 0, 3, 03, 03, 03, 003 };
        standardWin2->SetReceivedFromEGMTime(receivedFromEGMTime2);
        standardWin2->SetAwardState(ReceivedFromGame);
        standardWins.push_back(standardWin2);

        // Perform operation(s) and test(s).
        std::shared_ptr<const CEGMAward> newestStandardWin(CWebDiagnostics::GetNewestStandardProgressiveWin(lastStandardWin, standardWins));
        std::shared_ptr<const CEGMAward> calcNewestStandardWin(CalcNewestStandardProgressiveWin(lastStandardWin, standardWins));
        ASSERT_EQ((void *)standardWin2.get(), (void *)newestStandardWin.get()) << "GetNewestStandardProgressiveWin";
        ASSERT_EQ((void *)standardWin2.get(), (void *)calcNewestStandardWin.get()) << "CalcNewestStandardProgressiveWin";
    }

    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetNewestStandardProgressiveWin()
    /// Test with lastStandardWin=Award and standardWins with two Awards with newer and older date/times.
    /// </summary>
    TEST_F(WebDiagnostics_Progressive, GetNewestStandardProgressiveWin_Award_AwardNewerOlderTest)
    {
        // Setup.
        std::shared_ptr<CEGMAward> lastStandardWin(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTime = { 2001, 2, 0, 3, 02, 02, 02, 002 };
        lastStandardWin->SetReceivedFromEGMTime(receivedFromEGMTime);
        lastStandardWin->SetAwardState(ReceivedFromGame);
        lastStandardWin->setEgmPaidSignaled(true);

        std::vector<std::shared_ptr<CEGMAward>> standardWins;
        std::shared_ptr<CEGMAward> standardWin1(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTime1 = { 2001, 2, 0, 3, 03, 03, 03, 003 };
        standardWin1->SetReceivedFromEGMTime(receivedFromEGMTime1);
        standardWin1->SetAwardState(ReceivedFromGame);
        standardWins.push_back(standardWin1);

        std::shared_ptr<CEGMAward> standardWin2(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTime2 = { 2001, 2, 0, 3, 01, 01, 01, 001 };
        standardWin2->SetReceivedFromEGMTime(receivedFromEGMTime2);
        standardWin2->SetAwardState(ReceivedFromGame);
        standardWins.push_back(standardWin2);

        // Perform operation(s) and test(s).
        std::shared_ptr<const CEGMAward> newestStandardWin(CWebDiagnostics::GetNewestStandardProgressiveWin(lastStandardWin, standardWins));
        std::shared_ptr<const CEGMAward> calcNewestStandardWin(CalcNewestStandardProgressiveWin(lastStandardWin, standardWins));
        ASSERT_EQ((void *)standardWin1.get(), (void *)newestStandardWin.get()) << "GetNewestStandardProgressiveWin";
        ASSERT_EQ((void *)standardWin1.get(), (void *)calcNewestStandardWin.get()) << "CalcNewestStandardProgressiveWin";
    }

    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetNewestStandardProgressiveWin()
    /// Test with lastStandardWin=Award and standardWins with two Awards with the same newer date/times.
    /// </summary>
    TEST_F(WebDiagnostics_Progressive, GetNewestStandardProgressiveWin_Award_AwardNewerNewerTest)
    {
        // Setup.
        std::shared_ptr<CEGMAward> lastStandardWin(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTime = { 2001, 2, 0, 3, 02, 02, 02, 002 };
        lastStandardWin->SetReceivedFromEGMTime(receivedFromEGMTime);
        lastStandardWin->SetAwardState(ReceivedFromGame);
        lastStandardWin->setEgmPaidSignaled(true);

        std::vector<std::shared_ptr<CEGMAward>> standardWins;
        std::shared_ptr<CEGMAward> standardWin1(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTime1 = { 2001, 2, 0, 3, 03, 03, 03, 003 };
        standardWin1->SetReceivedFromEGMTime(receivedFromEGMTime1);
        standardWin1->SetAwardState(ReceivedFromGame);
        standardWins.push_back(standardWin1);

        std::shared_ptr<CEGMAward> standardWin2(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTime2 = { 2001, 2, 0, 3, 03, 03, 03, 003 };
        standardWin2->SetReceivedFromEGMTime(receivedFromEGMTime2);
        standardWin2->SetAwardState(ReceivedFromGame);
        standardWins.push_back(standardWin2);

        // Perform operation(s) and test(s).
        std::shared_ptr<const CEGMAward> newestStandardWin(CWebDiagnostics::GetNewestStandardProgressiveWin(lastStandardWin, standardWins));
        std::shared_ptr<const CEGMAward> calcNewestStandardWin(CalcNewestStandardProgressiveWin(lastStandardWin, standardWins));
        ASSERT_EQ((void *)standardWin2.get(), (void *)newestStandardWin.get()) << "GetNewestStandardProgressiveWin";
        ASSERT_EQ((void *)standardWin2.get(), (void *)calcNewestStandardWin.get()) << "CalcNewestStandardProgressiveWin";
    }

    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetNewestStandardProgressiveWin()
    /// Test with lastStandardWin=Award and standardWins with two Awards with newer and newest date/times.
    /// </summary>
    TEST_F(WebDiagnostics_Progressive, GetNewestStandardProgressiveWin_Award_AwardNewerNewestTest)
    {
        // Setup.
        std::shared_ptr<CEGMAward> lastStandardWin(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTime = { 2001, 2, 0, 3, 02, 02, 02, 002 };
        lastStandardWin->SetReceivedFromEGMTime(receivedFromEGMTime);
        lastStandardWin->SetAwardState(ReceivedFromGame);
        lastStandardWin->setEgmPaidSignaled(true);

        std::vector<std::shared_ptr<CEGMAward>> standardWins;
        std::shared_ptr<CEGMAward> standardWin1(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTime1 = { 2001, 2, 0, 3, 03, 03, 03, 003 };
        standardWin1->SetReceivedFromEGMTime(receivedFromEGMTime1);
        standardWin1->SetAwardState(ReceivedFromGame);
        standardWins.push_back(standardWin1);

        std::shared_ptr<CEGMAward> standardWin2(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTime2 = { 2001, 2, 0, 3, 04, 04, 04, 004 };
        standardWin2->SetReceivedFromEGMTime(receivedFromEGMTime2);
        standardWin2->SetAwardState(ReceivedFromGame);
        standardWins.push_back(standardWin2);

        // Perform operation(s) and test(s).
        std::shared_ptr<const CEGMAward> newestStandardWin(CWebDiagnostics::GetNewestStandardProgressiveWin(lastStandardWin, standardWins));
        std::shared_ptr<const CEGMAward> calcNewestStandardWin(CalcNewestStandardProgressiveWin(lastStandardWin, standardWins));
        ASSERT_EQ((void *)standardWin2.get(), (void *)newestStandardWin.get()) << "GetNewestStandardProgressiveWin";
        ASSERT_EQ((void *)standardWin2.get(), (void *)calcNewestStandardWin.get()) << "CalcNewestStandardProgressiveWin";
    }

    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetNewestStandardProgressiveWin()
    /// Test with lastStandardWin=Award and standardWins with two Awards with newest and newer date/times.
    /// </summary>
    TEST_F(WebDiagnostics_Progressive, GetNewestStandardProgressiveWin_Award_AwardNewestNewerTest)
    {
        // Setup.
        std::shared_ptr<CEGMAward> lastStandardWin(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTime = { 2001, 2, 0, 3, 02, 02, 02, 002 };
        lastStandardWin->SetReceivedFromEGMTime(receivedFromEGMTime);
        lastStandardWin->SetAwardState(ReceivedFromGame);
        lastStandardWin->setEgmPaidSignaled(true);

        std::vector<std::shared_ptr<CEGMAward>> standardWins;
        std::shared_ptr<CEGMAward> standardWin1(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTime1 = { 2001, 2, 0, 3, 04, 04, 04, 004 };
        standardWin1->SetReceivedFromEGMTime(receivedFromEGMTime1);
        standardWin1->SetAwardState(ReceivedFromGame);
        standardWins.push_back(standardWin1);

        std::shared_ptr<CEGMAward> standardWin2(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTime2 = { 2001, 2, 0, 3, 03, 03, 03, 003 };
        standardWin2->SetReceivedFromEGMTime(receivedFromEGMTime2);
        standardWin2->SetAwardState(ReceivedFromGame);
        standardWins.push_back(standardWin2);

        // Perform operation(s) and test(s).
        std::shared_ptr<const CEGMAward> newestStandardWin(CWebDiagnostics::GetNewestStandardProgressiveWin(lastStandardWin, standardWins));
        std::shared_ptr<const CEGMAward> calcNewestStandardWin(CalcNewestStandardProgressiveWin(lastStandardWin, standardWins));
        ASSERT_EQ((void *)standardWin1.get(), (void *)newestStandardWin.get()) << "GetNewestStandardProgressiveWin";
        ASSERT_EQ((void *)standardWin1.get(), (void *)calcNewestStandardWin.get()) << "CalcNewestStandardProgressiveWin";
    }
// #pragma endregion GetNewestStandardProgressiveWin

// #pragma region GetLastProgressiveAwardHtml
    // Possible test cases for GetLastProgressiveAwardHtml.
    // progAward
    //    progAward.GetAwardState() == NoAward)
    //    progAward.GetAwardState() != NoAward)
    //    progAward.GetAwardState() != NoAward, date/times in all orders compared to lastStandardWin and standardWins.
    // lastStandardWin permutations.
    //    lastStandardWin.get() == nullptr.
    //    lastStandardWin.get() != nullptr && lastStandardWin.get()->GetAwardState() == NoAward)
    //    lastStandardWin.get() != nullptr && lastStandardWin.get()->GetAwardState() != NoAward)
    // standardWins permutations.
    //    standardWins contains 0, 1, or 2 items.
    //    items permutations as above for lastStandardWin permutations.
    //    For 2 items with GetAwardState() != NoAward, date/times in both orders or the same date/time.
    //    For items with GetAwardState() != NoAward, date/times in both orders or same date/time compared to lastStandardWin.

    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetLastProgressiveAwardHtml()
    /// Test with progAward=NoAward and lastStandardWin nullptr and empty standardWins.
    /// </summary>
     TEST_F(WebDiagnostics_Progressive, GetLastProgressiveAwardHtml_NoAward_Null_EmptyTest)
    {
        // Setup.
        const DWORD tickCount(0);
        CEGMAward progAward;
        CEGMAwardManagerState progState;

        std::shared_ptr<CEGMAward> lastStandardWin(nullptr);

        std::vector<std::shared_ptr<CEGMAward>> standardWins;

        // Perform operation(s) and test(s).
       ASSERT_EQ(CalcLastProgressiveAwardHtml(tickCount, progAward, progState, lastStandardWin, standardWins),
            CWebDiagnostics::GetLastProgressiveAwardHtml(tickCount, progAward, progState, lastStandardWin, standardWins)) << "GetLastProgressiveAwardHtml";
        ASSERT_EQ(CalcLastProgressiveAwardHtml(tickCount, progAward, progState, lastStandardWin, standardWins),
            CWebDiagnostics::GetLastProgressiveAwardHtml(tickCount, progAward, progState, lastStandardWin, standardWins)) 
            << "GetLastProgressiveAwardHtml";
    }

    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetLastProgressiveAwardHtml()
    /// Test with progAward=NoAward and lastStandardWin=NoAward and empty standardWins.
    /// </summary>
     TEST_F(WebDiagnostics_Progressive,GetLastProgressiveAwardHtml_NoAward_NoAward_EmptyTest)
    {
        // Setup.
        const DWORD tickCount(0);
        CEGMAward progAward;
        CEGMAwardManagerState progState;

        std::shared_ptr<CEGMAward> lastStandardWin(new CEGMAward());

        std::vector<std::shared_ptr<CEGMAward>> standardWins;

        // Perform operation(s) and test(s).
       ASSERT_EQ(CalcLastProgressiveAwardHtml(tickCount, progAward, progState, lastStandardWin, standardWins),
            CWebDiagnostics::GetLastProgressiveAwardHtml(tickCount, progAward, progState, lastStandardWin, standardWins)) << "GetLastProgressiveAwardHtml";
        ASSERT_EQ(CalcLastProgressiveAwardHtml(tickCount, progAward, progState, lastStandardWin, standardWins),
            CWebDiagnostics::GetLastProgressiveAwardHtml(tickCount, progAward, progState, lastStandardWin, standardWins))
            << "GetLastProgressiveAwardHtml";
    }

// #pragma endregion GetNewestStandardProgressiveWin

// #pragma region GetLastProgressiveAwardHtml
    // Possible test cases for GetLastProgressiveAwardHtml.
    // progAward
    //    progAward.GetAwardState() == NoAward)
    //    progAward.GetAwardState() != NoAward)
    //    progAward.GetAwardState() != NoAward, date/times in all orders compared to lastStandardWin and standardWins.
    // lastStandardWin permutations.
    //    lastStandardWin.get() == nullptr.
    //    lastStandardWin.get() != nullptr && lastStandardWin.get()->GetAwardState() == NoAward)
    //    lastStandardWin.get() != nullptr && lastStandardWin.get()->GetAwardState() != NoAward)
    // standardWins permutations.
    //    standardWins contains 0, 1, or 2 items.
    //    items permutations as above for lastStandardWin permutations.
    //    For 2 items with GetAwardState() != NoAward, date/times in both orders or the same date/time.
    //    For items with GetAwardState() != NoAward, date/times in both orders or same date/time compared to lastStandardWin.

    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetLastProgressiveAwardHtml()
    /// Test with progAward=NoAward and lastStandardWin=Award and empty standardWins.
    /// </summary>
     TEST_F(WebDiagnostics_Progressive,GetLastProgressiveAwardHtml_NoAward_Award_EmptyTest)
    {
        // Setup.
        const DWORD tickCount(0);
        CEGMAward progAward;
        CEGMAwardManagerState progState;

        std::shared_ptr<CEGMAward> lastStandardWin(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTimeLastWin = { 2001, 2, 0, 3, 02, 02, 02, 002 };
        lastStandardWin->SetReceivedFromEGMTime(receivedFromEGMTimeLastWin);
        lastStandardWin->SetSASLevelId(23);
        lastStandardWin->SetLevelId(678);
        lastStandardWin->SetJackpotId(123456789012);
        lastStandardWin->SetAwardAmountPaid(12345);
        lastStandardWin->SetAwardState(ReceivedFromGame);
        lastStandardWin->SetHitReceived(true);
        lastStandardWin->setHandpayPendingSignaled(false);
        lastStandardWin->setEgmPaidSignaled(false);

        std::vector<std::shared_ptr<CEGMAward>> standardWins;

        // Perform operation(s) and test(s).
       ASSERT_EQ(CalcLastProgressiveAwardHtml(tickCount, progAward, progState, lastStandardWin, standardWins),
            CWebDiagnostics::GetLastProgressiveAwardHtml(tickCount, progAward, progState, lastStandardWin, standardWins)) << "GetLastProgressiveAwardHtml";
        ASSERT_EQ(CalcLastProgressiveAwardHtml(tickCount, progAward, progState, lastStandardWin, standardWins),
            CWebDiagnostics::GetLastProgressiveAwardHtml(tickCount, progAward, progState, lastStandardWin, standardWins))
            << "GetLastProgressiveAwardHtml";
    }

    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetLastProgressiveAwardHtml()
    /// Test with progAward=Award and lastStandardWin=Award with older date/time and empty standardWins.
    /// </summary>
     TEST_F(WebDiagnostics_Progressive,GetLastProgressiveAwardHtml_Award_AwardOlder_EmptyTest)
    {
        // Setup.
        const DWORD tickCount(0);
        CEGMAward progAward;
        progAward.SetAwardState(ReceivedFromSystem);
        const SYSTEMTIME receivedFromSystemTime = { 2001, 2, 0, 3, 02, 02, 02, 002 };
        progAward.SetReceivedFromSystemTime(receivedFromSystemTime);
        progAward.SetAwardTransferType(EGMAwardTransferType_SASAFTBonusingCash);
        progAward.SetLevelId(678);
        progAward.SetJackpotId(123456789012);
        progAward.SetRemoteJackpotId(2345678901);
        progAward.SetAwardAmountHit(12344);
        CEGMAwardManagerState progState;

        std::shared_ptr<CEGMAward> lastStandardWin(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTimeLastWin = { 2001, 2, 0, 3, 01, 01, 01, 001 };
        lastStandardWin->SetReceivedFromEGMTime(receivedFromEGMTimeLastWin);
        lastStandardWin->SetSASLevelId(19);
        lastStandardWin->SetLevelId(7890);
        lastStandardWin->SetJackpotId(2345678901234);
        lastStandardWin->SetAwardAmountPaid(234567);
        lastStandardWin->SetAwardState(ReceivedFromGame);
        lastStandardWin->SetHitReceived(false);
        lastStandardWin->setHandpayPendingSignaled(false);
        lastStandardWin->setEgmPaidSignaled(false);

        std::vector<std::shared_ptr<CEGMAward>> standardWins;

        // Perform operation(s) and test(s).
        ASSERT_EQ(CalcLastProgressiveAwardHtml(tickCount, progAward, progState, lastStandardWin, standardWins),
            CWebDiagnostics::GetLastProgressiveAwardHtml(tickCount, progAward, progState, lastStandardWin, standardWins)) 
            << "GetLastProgressiveAwardHtml";
    }

    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetLastProgressiveAwardHtml()
    /// Test with progAward=Award and lastStandardWin=Award with same date/time as progAward and empty standardWins.
    /// </summary>
     TEST_F(WebDiagnostics_Progressive, GetLastProgressiveAwardHtml_Award_AwardSame_EmptyTest)
    {
        // Setup.
        const DWORD tickCount(0);
        CEGMAward progAward;
        progAward.SetAwardState(ReceivedFromSystem);
        const SYSTEMTIME receivedFromSystemTime = { 2001, 2, 0, 3, 02, 02, 02, 002 };
        progAward.SetReceivedFromSystemTime(receivedFromSystemTime);
        progAward.SetAwardTransferType(EGMAwardTransferType_SASAFTBonusingCash);
        progAward.SetLevelId(678);
        progAward.SetJackpotId(123456789012);
        progAward.SetRemoteJackpotId(2345678901);
        progAward.SetAwardAmountHit(12344);
        CEGMAwardManagerState progState;

        std::shared_ptr<CEGMAward> lastStandardWin(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTimeLastWin = { 2001, 2, 0, 3, 02, 02, 02, 002 };
        lastStandardWin->SetReceivedFromEGMTime(receivedFromEGMTimeLastWin);
        lastStandardWin->SetSASLevelId(19);
        lastStandardWin->SetLevelId(7890);
        lastStandardWin->SetJackpotId(2345678901234);
        lastStandardWin->SetAwardAmountPaid(234567);
        lastStandardWin->SetAwardState(ReceivedFromGame);
        lastStandardWin->SetHitReceived(false);
        lastStandardWin->setHandpayPendingSignaled(false);
        lastStandardWin->setEgmPaidSignaled(false);

        std::vector<std::shared_ptr<CEGMAward>> standardWins;

        // Perform operation(s) and test(s).
        ASSERT_EQ(CalcLastProgressiveAwardHtml(tickCount, progAward, progState, lastStandardWin, standardWins),
            CWebDiagnostics::GetLastProgressiveAwardHtml(tickCount, progAward, progState, lastStandardWin, standardWins))
            << "GetLastProgressiveAwardHtml";
    }

    /// <summary>
    /// Functional test for:
    ///     CWebDiagnostics::GetLastProgressiveAwardHtml()
    /// Test with progAward=Award and lastStandardWin=Award with same date/time as progAward and empty standardWins.
    /// </summary>
     TEST_F(WebDiagnostics_Progressive, GetLastProgressiveAwardHtml_Award_AwardNewer_EmptyTest)
    {
        // Setup.
        const DWORD tickCount(0);
        CEGMAward progAward;
        progAward.SetAwardState(ReceivedFromSystem);
        const SYSTEMTIME receivedFromSystemTime = { 2001, 2, 0, 3, 02, 02, 02, 002 };
        progAward.SetReceivedFromSystemTime(receivedFromSystemTime);
        progAward.SetAwardTransferType(EGMAwardTransferType_SASAFTBonusingCash);
        progAward.SetLevelId(678);
        progAward.SetJackpotId(123456789012);
        progAward.SetRemoteJackpotId(2345678901);
        progAward.SetAwardAmountHit(12344);
        CEGMAwardManagerState progState;

        std::shared_ptr<CEGMAward> lastStandardWin(new CEGMAward());
        const SYSTEMTIME receivedFromEGMTimeLastWin = { 2001, 2, 0, 3, 03, 03, 03, 003 };
        lastStandardWin->SetReceivedFromEGMTime(receivedFromEGMTimeLastWin);
        lastStandardWin->SetSASLevelId(19);
        lastStandardWin->SetLevelId(7890);
        lastStandardWin->SetJackpotId(2345678901234);
        lastStandardWin->SetAwardAmountPaid(234567);
        lastStandardWin->SetAwardState(ReceivedFromGame);
        lastStandardWin->SetHitReceived(false);
        lastStandardWin->setHandpayPendingSignaled(false);
        lastStandardWin->setEgmPaidSignaled(false);

        std::vector<std::shared_ptr<CEGMAward>> standardWins;

        // Perform operation(s) and test(s).
        ASSERT_EQ(CalcLastProgressiveAwardHtml(tickCount, progAward, progState, lastStandardWin, standardWins),
            CWebDiagnostics::GetLastProgressiveAwardHtml(tickCount, progAward, progState, lastStandardWin, standardWins))
            << "GetLastProgressiveAwardHtml";
    }

// #pragma endregion GetLastProgressiveAwardHtml

    /// <summary>
    /// Code coverage and functional test for:
    ///     CWebDiagnostics::SaveProgressiveData()
    /// Test with non-default award.
    /// </summary>
     TEST_F(WebDiagnostics_Progressive, SaveProgressiveData_AwardTest)
    {
        // Input(s).
        LPCTSTR const currentHostIP(_T("11.22.33.44"));
        std::string dumpFileName(_T("/tmp/ProgressiveInfo.html"));
        const DWORD tickCount(0);
        CPlayerSession playerSession;


		time_t startupTime(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12));
		time_t currentTime(CUtilities::GetTimeFromFields(2014, 1, 27, 13, 14, 15));

        CPackageInfo activeSkin(10, "ActiveSkinName", "10", 2, 0);
        CPackageInfo pendingSkin(11, "PendingSkinName", "11", 3, 2);
        CPackageInfo activeCampaign(101, "ActiveCampaignName", "101", 4, 0);
        CPackageInfo pendingCampaign(102, "PendingCampaignName", "102", 5, 3);
        
        const CWebDiagnosticsCommonInfo commonInfo(startupTime,
			currentTime,
			941,
			5141,
			_T("1.12.2.0"),
			_T("1.21.0"),
            activeSkin,
            pendingSkin,
            activeCampaign,
            pendingCampaign,
            161028 * 1024,
			_T("192.168.1.12"),
			0,
			8, 
			"Sentinel is online", "", "", "", 0.0);

    

        typedef struct {
            LPCTSTR const m_ipAddressString;
            int m_port;
        } TIPAddressStringPort;

        typedef struct
        {
            // Input(s) for CProgressiveConfig.
            const bool m_hasConfigFile;
            const bool m_active;
            const bool m_dnsLookup;
            const DWORD m_pingInterval;
            const LPCTSTR* const m_endpoints;
            const int m_endpointCount;

            // Input(s) for CProgressiveConnectionInfo.
            const TIPAddressStringPort m_lastMessageIP;
            const SYSTEMTIME m_lastMessageTimestamp;
            const ProgressiveConnectionState m_state;
            const ProgressiveConnectionError m_error;

            // Input(s) for CProgressiveHost.
            LPCTSTR const m_hostName;
            const int m_hostPort;

            // Expected result(s).
            const string m_bufferExpected;
            const UINT m_bufferLengthExpected;
        } TestDataType;

        // Input(s).
        LPCTSTR endpoints1[] = {
            _T("abcde.xyz:12345"),
        };
        LPCTSTR endpoints2[] = {
            _T("abcde.xyz:12345"),
            _T("   abcdefghijk.lmnop.xyz:1234567890   "),
        };
        LPCTSTR endpoints3[] = {
            _T("abcde.xyz:12345"),
            _T("abcdefghijk.lmnop.xyz:1234567890"),
            _T("!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~:123456789012345678901234567890 "),
        };

        // Buffer length expected does not include the trailing NUL character.
        string bufferExpected0 = (
            "<html><META Http-equiv=\"refresh\" Content=\"30\">"
            "Startup Time: 01/27/2014 10:11:12 AM<br>"
            "Current Time: 01/27/2014 01:14:15 PM<br>"
            "SlotMast ID: 941<br>"
            "Machine Number:5141<br>"
            "Sentinel Version: 1.12.2.0<br>"
            "OS Image Version: 1.21.0<br>"
            "<br>"
            "Configuration File Present: false<br>"
            "Progressive Configuration: Disabled<br>"
            "DNS Lookup: false<br>"
            "Ping Interval(ms): 15000<br>"
            "<br>"
            "Progressive Connection Info<br>"
            "DNS Hostname: N/A<br>"
            "Current Host IP: 1.2.3.4<br>"
            "Connection State: N/A<br>"
            "Last Message IP: 101.104.105.0<br>"
            "Last Message Time: 02/03/2011 14:25:00<br>"
            "<br>"
            "Progressive List<br>"
            "Host: onelinkdiscossl:61614<br>"
            "Host: onelinkdisco:61613<br>"
            "<br>"
            "Progressive Addresses<br>"
            "Host IP: 1.2.3.4<br>"
            "<br>"
            "</html>"
        );

        const TestDataType TEST_DATA[] =
        {
            { false, false, false, 0, NULL, 0,
            { _T("101.104.105.0"), 20000 }, { 2011, 2, 0, 3, 14, 25, 0, 789 }, (ProgressiveConnectionState)-1, CouldNotFindServer,
            _T("1.2.3.4:12345"), 10000,
            bufferExpected0, sizeof(bufferExpected0)-1
            },
        };

        for (int testIndex(0); testIndex < _countof(TEST_DATA); ++testIndex)
        {
            // Setup.
            CProgressiveConfig varProgConfig;
            varProgConfig.SetHasConfigFile(TEST_DATA[testIndex].m_hasConfigFile);
            varProgConfig.SetActive(TEST_DATA[testIndex].m_active);
            varProgConfig.SetDNSLookup(TEST_DATA[testIndex].m_dnsLookup);
            varProgConfig.SetPingInterval(TEST_DATA[testIndex].m_pingInterval);
            for (int index(0); index < TEST_DATA[testIndex].m_endpointCount; ++index)
            {
                varProgConfig.AddEndpoint(TEST_DATA[testIndex].m_endpoints[index]);
            }
            const CProgressiveConfig& progConfig(varProgConfig);

            CIPAddress ipAddress;
            ipAddress.SetIP(TEST_DATA[testIndex].m_lastMessageIP.m_ipAddressString);
            ipAddress.SetPort(TEST_DATA[testIndex].m_lastMessageIP.m_port);
            CProgressiveConnectionInfo varProgInfo;
            varProgInfo.SetLastMessageIP(ipAddress);
            varProgInfo.SetLastMessageTimestamp(TEST_DATA[testIndex].m_lastMessageTimestamp);
            varProgInfo.SetState(TEST_DATA[testIndex].m_state);
            varProgInfo.SetLastError(TEST_DATA[testIndex].m_error);
            const CProgressiveConnectionInfo& progInfo(varProgInfo);

            CProgressiveHost varProgHost(TEST_DATA[testIndex].m_hostName, TEST_DATA[testIndex].m_hostPort);
            if (!TEST_DATA[testIndex].m_dnsLookup)
            {
                CIPAddressArray ipAddressArray;
                for (int index(0); index < (testIndex % 5) + 1; ++index)
                {
                    ipAddressArray.Add(TEST_DATA[testIndex].m_hostName);
                }
                varProgHost = CProgressiveHost(ipAddressArray);
            }
            const CProgressiveHost& progHost(varProgHost);

            CEGMAward varProgAward;
            varProgAward.SetAwardAmountPaid(12345);
            varProgAward.SetAwardState(AwardToGameSuccess);
            varProgAward.SetSentToGameCount(2);
            const SYSTEMTIME sentToGameTime = { 2001, 2, 0, 3, 12, 34, 56, 789 };
            varProgAward.SetSentToGameTime(sentToGameTime);
            const SYSTEMTIME receivedFromSystemTime = { 2001, 2, 0, 3, 12, 34, 55, 678 };
            varProgAward.SetReceivedFromSystemTime(receivedFromSystemTime);
            varProgAward.SetAwardTransferType(EGMAwardTransferType_SASAFTBonusingCash);
            varProgAward.SetDuplicateHitCount(3);
            varProgAward.SetOtherHitCount(4);
            varProgAward.SetOriginalMachineBonusMeterValue(2345);
            varProgAward.SetOriginalAttendantBonusingMeterValue(34567);
            varProgAward.SetOriginalLegacyBonusMeterValue(6789);
            varProgAward.SetLevelId(678);
            varProgAward.SetJackpotId(123456789012);
            varProgAward.SetRemoteJackpotId(2345678901);
            varProgAward.SetAwardAmountHit(12344);
            const CEGMAward& progAward(varProgAward);

            const std::shared_ptr<CEGMAward> lastStandardWin;
            const std::vector<std::shared_ptr<CEGMAward>> standardWins;

            const CPlayerSession playerSession;
            CEGMAwardManagerState progState;

            SYSTEMTIME levelReceivedTime = { 2001, 2, 0, 3, 12, 34, 56, 789 };
            _ICardConfig5 cardConfig;
            shared_ptr<ProgressiveLevelManager> levelManager = cardConfig.CreateLevelManager(nullptr, &levelReceivedTime);

			std::shared_ptr<std::ostream> file = std::shared_ptr<iostream>(new fstream(dumpFileName, ios_base::out | ios_base::trunc));

            const SYSTEMTIME localTime = { 2015, 7, 5, 10, 12, 0, 0, 0 };

            // Perform operation(s) to be tested.
            CWebDiagnostics::SaveProgressiveData(file,
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
                *levelManager);

            // Test(s).
            std::string oldHtml(TEST_DATA[testIndex].m_bufferExpected);
            oldHtml.insert(oldHtml.length() - _tcslen(_T("</html>")),
                CalcLastProgressiveAwardHtml(tickCount, progAward, progState, lastStandardWin, standardWins));
            std::string expected(InsertProgressiveDataHtml(oldHtml, playerSession, tickCount, localTime, progState, *levelManager,
                progAward, lastStandardWin, standardWins)); 

			std::ifstream infile(dumpFileName);
			std::stringstream strStream;
            strStream << infile.rdbuf();
            string filestr = strStream.str();
			bool result = expected.compare(strStream.str());
            AssertResult3(result, true, "ProgressiveDataResult");

        }
    }

    /// <summary>
    /// Code coverage and functional test for:
    ///     CWebDiagnostics::SaveProgressiveData()
    /// Test with non-default award manager state.
    /// </summary>
     TEST_F(WebDiagnostics_Progressive, SaveProgressiveData_StateTest)
    {
        // Input(s).
        LPCTSTR const currentHostIP(_T("11.22.33.44"));
        std::string dumpFileName(_T("/tmp/ProgressiveInfo.html"));
        const DWORD tickCount(20000000);

        CPlayerSession playerSession;

		time_t startupTime(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12));
		time_t currentTime(CUtilities::GetTimeFromFields(2014, 1, 27, 13, 14, 15));

        CPackageInfo activeSkin(10, "ActiveSkinName", "10", 2, 0);
        CPackageInfo pendingSkin(11, "PendingSkinName", "11", 3, 2);
        CPackageInfo activeCampaign(101, "ActiveCampaignName", "101", 4, 0);
        CPackageInfo pendingCampaign(102, "PendingCampaignName", "102", 5, 3);

        const CWebDiagnosticsCommonInfo commonInfo(startupTime,
			currentTime,
			941,
			5141,
			_T("1.12.2.0"),
			_T("1.21.0"),
            activeSkin,
            pendingSkin,
            activeCampaign,
            pendingCampaign,
            161028 * 1024,
			_T("192.168.1.12"),
			0,
			8, 
			"Sentinel is online", "", "", "", 0.0);


        typedef struct {
            LPCTSTR const m_ipAddressString;
            int m_port;
        } TIPAddressStringPort;

        typedef struct
        {
            // Input(s) for CProgressiveConfig.
            const bool m_hasConfigFile;
            const bool m_active;
            const bool m_dnsLookup;
            const DWORD m_pingInterval;
            const LPCTSTR* const m_endpoints;
            const int m_endpointCount;

            // Input(s) for CProgressiveConnectionInfo.
            const TIPAddressStringPort m_lastMessageIP;
            const SYSTEMTIME m_lastMessageTimestamp;
            const ProgressiveConnectionState m_state;
            const ProgressiveConnectionError m_error;

            // Input(s) for CProgressiveHost.
            LPCTSTR const m_hostName;
            const int m_hostPort;

            // Expected result(s).
            string m_bufferExpected;
            const UINT m_bufferLengthExpected;
        } TestDataType;

        // Input(s).
        LPCTSTR endpoints1[] = {
            _T("abcde.xyz:12345"),
        };
        LPCTSTR endpoints2[] = {
            _T("abcde.xyz:12345"),
            _T("   abcdefghijk.lmnop.xyz:1234567890   "),
        };
        LPCTSTR endpoints3[] = {
            _T("abcde.xyz:12345"),
            _T("abcdefghijk.lmnop.xyz:1234567890"),
            _T("!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~:123456789012345678901234567890 "),
        };

        // Buffer length expected does not include the trailing NUL character.
        string bufferExpected0 = (
            "<html><META Http-equiv=\"refresh\" Content=\"30\">"
            "Startup Time: 01/27/2014 10:11:12 AM<br>"
            "Current Time: 01/27/2014 01:14:15 PM<br>"
            "SlotMast ID: 941<br>"
            "Machine Number:5141<br>"
            "Sentinel Version: 1.12.2.0<br>"
            "OS Image Version: 1.21.0<br>"
            "<br>"
            "Configuration File Present: false<br>"
            "Progressive Configuration: Disabled<br>"
            "DNS Lookup: false<br>"
            "Ping Interval(ms): 15000<br>"
            "<br>"
            "Progressive Connection Info<br>"
            "DNS Hostname: N/A<br>"
            "Current Host IP: 1.2.3.4<br>"
            "Connection State: N/A<br>"
            "Last Message IP: 101.104.105.0<br>"
            "Last Message Time: 02/03/2011 14:25:00<br>"
            "<br>"
            "Progressive List<br>"
            "Host: onelinkdiscossl:61614<br>"
            "Host: onelinkdisco:61613<br>"
            "<br>"
            "Progressive Addresses<br>"
            "Host IP: 1.2.3.4<br>"
            "<br>"
            "</html>"
        );

        const TestDataType TEST_DATA[] =
        {
            { false, false, false, 0, NULL, 0,
            { _T("101.104.105.0"), 20000 }, { 2011, 2, 0, 3, 14, 25, 0, 789 }, (ProgressiveConnectionState)-1, CouldNotFindServer,
            _T("1.2.3.4:12345"), 10000,
            bufferExpected0, sizeof(bufferExpected0)-1
            },
        };

        for (int testIndex(0); testIndex < _countof(TEST_DATA); ++testIndex)
        {
            // Setup.
            CProgressiveConfig varProgConfig;
            varProgConfig.SetHasConfigFile(TEST_DATA[testIndex].m_hasConfigFile);
            varProgConfig.SetActive(TEST_DATA[testIndex].m_active);
            varProgConfig.SetDNSLookup(TEST_DATA[testIndex].m_dnsLookup);
            varProgConfig.SetPingInterval(TEST_DATA[testIndex].m_pingInterval);
            for (int index(0); index < TEST_DATA[testIndex].m_endpointCount; ++index)
            {
                varProgConfig.AddEndpoint(TEST_DATA[testIndex].m_endpoints[index]);
            }
            const CProgressiveConfig& progConfig(varProgConfig);

            CIPAddress ipAddress;
            ipAddress.SetIP(TEST_DATA[testIndex].m_lastMessageIP.m_ipAddressString);
            ipAddress.SetPort(TEST_DATA[testIndex].m_lastMessageIP.m_port);
            CProgressiveConnectionInfo varProgInfo;
            varProgInfo.SetLastMessageIP(ipAddress);
            varProgInfo.SetLastMessageTimestamp(TEST_DATA[testIndex].m_lastMessageTimestamp);
            varProgInfo.SetState(TEST_DATA[testIndex].m_state);
            varProgInfo.SetLastError(TEST_DATA[testIndex].m_error);
            const CProgressiveConnectionInfo& progInfo(varProgInfo);

            CProgressiveHost varProgHost(TEST_DATA[testIndex].m_hostName, TEST_DATA[testIndex].m_hostPort);
            if (!TEST_DATA[testIndex].m_dnsLookup)
            {
                CIPAddressArray ipAddressArray;
                for (int index(0); index < (testIndex % 5) + 1; ++index)
                {
                    ipAddressArray.Add(TEST_DATA[testIndex].m_hostName);
                }
                varProgHost = CProgressiveHost(ipAddressArray);
            }
            const CProgressiveHost& progHost(varProgHost);

            // Want in-progress award in order to report the award manager state.
            CEGMAward varProgAward;
            varProgAward.SetAwardAmountPaid(0);
            varProgAward.SetAwardState(Signaled);
            varProgAward.SetSentToGameCount(2);
            const SYSTEMTIME sentToGameTime = { 2001, 2, 0, 3, 12, 34, 56, 789 };
            varProgAward.SetSentToGameTime(sentToGameTime);
            const SYSTEMTIME receivedFromSystemTime = { 2001, 2, 0, 3, 12, 34, 55, 678 };
            varProgAward.SetReceivedFromSystemTime(receivedFromSystemTime);
            varProgAward.SetAwardTransferType(EGMAwardTransferType_SASAFTBonusingCash);
            varProgAward.SetDuplicateHitCount(3);
            varProgAward.SetOtherHitCount(4);
            varProgAward.SetOriginalMachineBonusMeterValue(2345);
            varProgAward.SetOriginalAttendantBonusingMeterValue(34567);
            varProgAward.SetOriginalLegacyBonusMeterValue(6789);
            varProgAward.SetLevelId(678);
            varProgAward.SetJackpotId(123456789012);
            varProgAward.SetRemoteJackpotId(2345678901);
            varProgAward.SetAwardAmountHit(12344);
            const CEGMAward& progAward(varProgAward);

            const std::shared_ptr<CEGMAward> lastStandardWin;
            const std::vector<std::shared_ptr<CEGMAward>> standardWins;

            const CPlayerSession playerSession;
            CEGMAwardManagerState varProgState;
            varProgState.m_awardTickCountStart = tickCount - (2 * 60000  + 34 * 1000);
            varProgState.m_awardExpired = false;
            varProgState.m_waitingForGCF = true;
            varProgState.m_gcfDelayTickCountStart = tickCount - 500;
            varProgState.m_gcfDelayExpired = false;

            SYSTEMTIME levelReceivedTime = { 2001, 2, 0, 3, 12, 34, 56, 789 };

            _ICardConfig5 cardConfig;
            shared_ptr<ProgressiveLevelManager> levelManager = cardConfig.CreateLevelManager(nullptr, &levelReceivedTime);

            const CEGMAwardManagerState progState(varProgState);

			std::shared_ptr<std::ostream> file = std::shared_ptr<iostream>(new fstream(dumpFileName, ios_base::out | ios_base::trunc));
			
            const SYSTEMTIME localTime = { 2015, 7, 5, 10, 12, 0, 0, 0 };

            // Perform operation(s) to be tested.
            CWebDiagnostics::SaveProgressiveData(file,
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
                *levelManager);	

            std::string oldHtml(TEST_DATA[testIndex].m_bufferExpected);
            oldHtml.insert(oldHtml.length() - _tcslen(_T("</html>")),
                CalcLastProgressiveAwardHtml(tickCount, progAward, progState, lastStandardWin, standardWins));
            std::string expected(InsertProgressiveDataHtml(oldHtml, playerSession, tickCount, localTime, progState, *levelManager,
                progAward, lastStandardWin, standardWins)); 

			std::ifstream infile(dumpFileName);
			std::stringstream strStream;
            strStream << infile.rdbuf();
            string filestr = strStream.str();
			bool result = expected.compare(strStream.str());
            AssertResult3(result, true, "ProgressiveDataResult");
			
        }
    }

    /// <summary>
    /// Code coverage and functional test for:
    ///     CWebDiagnostics::UpdateProgressive()
    /// Test with empty host name and host port zero.
    /// </summary>
     TEST_F(WebDiagnostics_Progressive,UpdateProgressive_DefaultTest)
    {
        // Input(s).
        const DWORD tickCount(0);

        CPackageInfo activeSkin(10, "ActiveSkinName", "10", 2, 0);
        CPackageInfo pendingSkin(11, "PendingSkinName", "11", 3, 2);
        CPackageInfo activeCampaign(101, "ActiveCampaignName", "101", 4, 0);
        CPackageInfo pendingCampaign(102, "PendingCampaignName", "102", 5, 3);

        const CWebDiagnosticsCommonInfo commonInfo(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12),
            CUtilities::GetTimeFromFields(2014, 1, 27, 13, 14, 15),
                    941,
                5141,
                _T("1.12.2.0"),
                _T("1.21.0"),
                activeSkin,
                pendingSkin,
                activeCampaign,
                pendingCampaign,
                161028 * 1024,
				_T("192.168.1.12"),
                0,
                8, "Sentinel is online", "", "", "", 0.0);

        // Expected result(s).
        // totalFileCreateCountExpected currently includes:
        //     an input access for ErrorDump.html via CWebDiagnostics::Start(), and
        //     an output access for ProgressiveInfo.html.
        const DWORD totalFileCreateCountExpected(2); // Expect number of creates == opens == finalizes.
        const int totalGetCurrentTimeCount(0);

        // Bufferg length expected does not include the trailing NUL character.
        const string bufferExpected0 = {
           "Progressive Player Info<br><div style=\"margin-left:6px;\">Session Active: false<br>Carded Player: false<br>Session ID: 0<br>Last Session ID Increment Time: 03/27/2018 03:02:16<br>Session ID Match Status: false<br></div><br>Progressive Level Info:<br><div style=\"margin-left:6px;\">Group ID: 0<br>Updated Time: N/A<br>Last Eligibility Received Time: N/A<br>Eligibility Determined: false<br>Eligibility Session ID: 0<br>Query Cards: false<br></div><br>Progressive Down Alert Info:<br><div style=\"margin-left:6px;\">Is Down: false<br>Last Down Time:<br>Last Up Time:<br>Number of Times Down Today: 0<br>Total Milliseconds Down Today: 0<br>Number of Times Down Yesterday: 0<br>Total Milliseconds Down Yesterday: 0<br></div><br>Last Bonus Award<br>N/A<br><br>Previous Standard SAS Progressive Win<br>N/A<br><br>Current Standard SAS Progressive Win(s)<br>N/A<br><br>"
        };

        // Setup.
        const int testIndex(0);
        const CProgressiveConfig progConfig;
        const CProgressiveConnectionInfo progInfo;
        const CProgressiveHost progHost(_T(""), 0);
        const CEGMAward progAward;
        const std::shared_ptr<CEGMAward> lastStandardWin;
        const std::vector<std::shared_ptr<CEGMAward>> standardWins;
        const CPlayerSession playerSession;
        const CEGMAwardManagerState progState;
        ProgressiveLevelManager levelManager;
        CEGMConfig egmConfig(MEMORY_NONE);

		// Setup.
        string dumpFileName("44545454");
        std::shared_ptr<std::ostream> file = shared_ptr<iostream>(new fstream(dumpFileName, ios_base::out | ios_base::trunc));
        COperatingSystemTest operatingSystem(dumpFileName, std::ios::out,"",0);

        CWebDiagnostics wb(operatingSystem);
        wb.Start();

        const SYSTEMTIME localTime = { 2015, 7, 5, 10, 12, 0, 0, 0 };

		// Perform operation(s) to be tested.
        wb.UpdateProgressive(commonInfo,
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
			levelManager);

        ASSERT_EQ(operatingSystem.m_file1.m_fileName, dumpFileName)<< _T("UpdateProgressive() m_fileName");
        ASSERT_TRUE(sizeof(operatingSystem.m_file1.m_finalFileContents)>0)<< _T("UpdateProgressive() length");
        // Verify that the output file is as expected.
        std::string updatedhtml = PrepareProgressiveDataHtml(playerSession, tickCount, localTime, progState, levelManager,
        progAward, lastStandardWin, standardWins);
           // Perform operation(s) and test(s)
        ASSERT_TRUE( updatedhtml.length() > 0 ) << "UpdateProgressive_DefaultTest";

    }
    /// <summary>
    /// Code coverage and functional test for:
    ///     CWebDiagnostics::UpdateProgressive()
    /// Each iteration tests a (different) state and/or error condition
    /// </summary>
     TEST_F(WebDiagnostics_Progressive,UpdateProgressiveTest)
    {
        // Input(s).
        const int utilitiesTimeZoneOffset(0);
        const DWORD tickCount(0);

        CPackageInfo activeSkin(10, "ActiveSkinName", "10", 2, 0);
        CPackageInfo pendingSkin(11, "PendingSkinName", "11", 3, 2);
        CPackageInfo activeCampaign(101, "ActiveCampaignName", "101", 4, 0);
        CPackageInfo pendingCampaign(102, "PendingCampaignName", "102", 5, 3);

        const CWebDiagnosticsCommonInfo commonInfo(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12),
            CUtilities::GetTimeFromFields(2014, 1, 27, 13, 14, 15),
                    941,
                5141,
                _T("1.12.2.0"),
                _T("1.21.0"),
                activeSkin, 
                pendingSkin,
                activeCampaign, 
                pendingCampaign,
                161028 * 1024,
				_T("192.168.1.12"),
                0,
                8, "Sentinel is online", "", "", "", 0.0);

        // Expected result(s).
        // totalFileCreateCountExpected currently includes:
        //     an input access for ErrorDump.html via CWebDiagnostics::Start(), and
        //     an output access for ProgressiveInfo.html.
        const DWORD totalFileCreateCountExpected(2); // Expect number of creates == opens == finalizes.
        const int totalGetCurrentTimeCount(0);

        typedef struct {
            LPCTSTR const m_ipAddressString;
            int m_port;
        } TIPAddressStringPort;

        typedef struct
        {
            // Input(s) for CProgressiveConfig.
            const bool m_hasConfigFile;
            const bool m_active;
            const bool m_dnsLookup;
            const DWORD m_pingInterval;
            const LPCTSTR* const m_endpoints;
            const int m_endpointCount;

            // Input(s) for CProgressiveConnectionInfo.
            const TIPAddressStringPort m_lastMessageIP;
            const SYSTEMTIME m_lastMessageTimestamp;
            const ProgressiveConnectionState m_state;
            const ProgressiveConnectionError m_error;

            // Input(s) for CProgressiveHost.
            LPCTSTR const m_hostName;
            const int m_hostPort;

            // Expected result(s).
            const std::string  m_bufferExpected;
            const UINT m_bufferLengthExpected;
        } TestDataType;

        // Input(s).
        LPCTSTR endpoints1[] = {
            _T("abcde.xyz:12345"),
        };
        LPCTSTR endpoints2[] = {
            _T("abcde.xyz:12345"),
            _T("   abcdefghijk.lmnop.xyz:1234567890   "),
        };
        LPCTSTR endpoints3[] = {
            _T("abcde.xyz:12345"),
            _T("abcdefghijk.lmnop.xyz:1234567890"),
            _T("!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~:123456789012345678901234567890 "),
        };

        // Buffer length expected does not include the trailing NUL character.
        const string bufferExpected0= {
            "Progressive Player Info<br><div style=\"margin-left:6px;\">Session Active: false<br>Carded Player: false<br>Session ID: 0<br>Last Session ID Increment Time: 03/27/2018 03:06:09<br>Session ID Match Status: false<br></div><br>Progressive Level Info:<br><div style=\"margin-left:6px;\">Group ID: 0<br>Updated Time: N/A<br>Last Eligibility Received Time: N/A<br>Eligibility Determined: false<br>Eligibility Session ID: 0<br>Query Cards: false<br></div><br>Progressive Down Alert Info:<br><div style=\"margin-left:6px;\">Is Down: false<br>Last Down Time:<br>Last Up Time:<br>Number of Times Down Today: 0<br>Total Milliseconds Down Today: 0<br>Number of Times Down Yesterday: 0<br>Total Milliseconds Down Yesterday: 0<br></div><br>Last Bonus Award<br>N/A<br><br>Previous Standard SAS Progressive Win<br>N/A<br><br>Current Standard SAS Progressive Win(s)<br>N/A<br><br>"
        };

       const TestDataType TEST_DATA[] =
        {
            false, false, false, 0, NULL, 0,
            { _T("101.104.105.0"), 20000 }, { 2011, 2, 0, 3, 14, 25, 0, 789 }, (ProgressiveConnectionState)-1, CouldNotFindServer,
            _T("1.2.3.4:12345"), 10000,
            bufferExpected0, sizeof(bufferExpected0) - 1
       
       };

        // Setup. Set the nCompass time zone to match the expected results.
        CUtilities::SetTzOffset(utilitiesTimeZoneOffset);

        for (int testIndex(0); testIndex < _countof(TEST_DATA); ++testIndex)
        {
            // Setup.
            CProgressiveConfig varProgConfig;
            varProgConfig.SetHasConfigFile(TEST_DATA[testIndex].m_hasConfigFile);
            varProgConfig.SetActive(TEST_DATA[testIndex].m_active);
            varProgConfig.SetDNSLookup(TEST_DATA[testIndex].m_dnsLookup);
            varProgConfig.SetPingInterval(TEST_DATA[testIndex].m_pingInterval);
            for (int index(0); index < TEST_DATA[testIndex].m_endpointCount; ++index)
            {
                varProgConfig.AddEndpoint(TEST_DATA[testIndex].m_endpoints[index]);
            }
            const CProgressiveConfig& progConfig(varProgConfig);

            CIPAddress ipAddress;
            ipAddress.SetIP(TEST_DATA[testIndex].m_lastMessageIP.m_ipAddressString);
            ipAddress.SetPort(TEST_DATA[testIndex].m_lastMessageIP.m_port);
            CProgressiveConnectionInfo varProgInfo;
            varProgInfo.SetLastMessageIP(ipAddress);
            varProgInfo.SetLastMessageTimestamp(TEST_DATA[testIndex].m_lastMessageTimestamp);
            varProgInfo.SetState(TEST_DATA[testIndex].m_state);
            varProgInfo.SetLastError(TEST_DATA[testIndex].m_error);
            const CProgressiveConnectionInfo& progInfo(varProgInfo);

            CProgressiveHost varProgHost(TEST_DATA[testIndex].m_hostName, TEST_DATA[testIndex].m_hostPort);
            if (!TEST_DATA[testIndex].m_dnsLookup)
            {
                CIPAddressArray ipAddressArray;
                for (int index(0); index < (testIndex % 5) + 1; ++index)
                {
                    ipAddressArray.Add(TEST_DATA[testIndex].m_hostName);
                }
                varProgHost = CProgressiveHost(ipAddressArray);
            }
            const CProgressiveHost& progHost(varProgHost);

            const CEGMAward progAward;
            const std::shared_ptr<CEGMAward> lastStandardWin;
            const std::vector<std::shared_ptr<CEGMAward>> standardWins;
            const CPlayerSession playerSession;
            const CEGMAwardManagerState progState;
            ProgressiveLevelManager levelManager;
            CEGMConfig egmConfig(MEMORY_NONE);

			// Setup.
        string dumpFileName("44545454");
        std::shared_ptr<std::ostream> file = shared_ptr<iostream>(new fstream(dumpFileName, ios_base::out | ios_base::trunc));
        COperatingSystemTest operatingSystem(dumpFileName, std::ios::out,"",0);

            CWebDiagnostics wb(operatingSystem);
            wb.Start();

            const SYSTEMTIME localTime = { 2015, 7, 5, 10, 12, 0, 0, 0 };

            // Perform operation(s) to be tested.
            wb.UpdateProgressive(commonInfo,
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
                levelManager);

            ASSERT_EQ(operatingSystem.m_file1.m_fileName, dumpFileName)<< _T("UpdateProgressive() m_fileName");
            ASSERT_TRUE(sizeof(operatingSystem.m_file1.m_finalFileContents)>0)<< _T("UpdateProgressive() length");

            std::string updatedhtml = PrepareProgressiveDataHtml(playerSession, tickCount, localTime, progState, levelManager,
            progAward, lastStandardWin, standardWins);
            // Perform operation(s) and test(s)
            //There is a time stamp on page, which will vary
            ASSERT_TRUE(updatedhtml.length() > 0) << "UpdateProgressiveTest";

        }
    }

}
