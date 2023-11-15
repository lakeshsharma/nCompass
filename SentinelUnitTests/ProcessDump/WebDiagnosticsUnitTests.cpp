#include "stdafx.h"
#include "ProcessDump/WebDiagnostics.h"

// Sentinel includes.
#include "Diagnostics.h"
#include "Utilities.h"

// Unit testing includes.
#include "gtest/gtest.h"
#include "UnitTestingOperatingSystem.h"
#include "UnitTestingUtils.h"
#include "../State/SentinelState.h"
#include "../ExcessiveVoucherOut.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	class WebDiagnostics : public ::testing::Test
	{
	public:

		static const UINT m_maximumErrorHtmlSize = 4096 * 16 + 1024;
		static const int m_statusDumpRefreshSeconds = 30;
		static const int m_memoryDumpRefreshSeconds = 30;
		static const int m_errorDumpRefreshSeconds = 60;
		static const string m_errorDumpFileName;
		static const string m_errorFileNameExpected;
		static const string m_statusFileNameExpected;
		static const string m_memoryFileNameExpected;
		static const string m_stackFileNameExpected;

		static const ios::openmode m_errorOpenFlagsExpected;
		static const ios::openmode m_dumpOpenFlagsExpected;

		// Example error files contents.
		static const string m_errorFileName;
		static const char m_errorFileContents1[];
		static const UINT m_errorFileContentsLength1;
		static const char m_errorFileContents2[];
		static const UINT m_errorFileContentsLength2;
		static const char m_errorFileContents3[];
		static const UINT m_errorFileContentsLength3;
		static const char m_errorFileContents4[];
		static const UINT m_errorFileContentsLength4;
		
		static const DWORD CURRENT_MULTIPLIER = 23;
		static const DWORD CURRENT_DIVIDER = 34;

		// Web content simulator.  Compare actual results with simulator results.
		string CalcPageHeaderHtml(int refreshSeconds)
		{
			string html;
			html = FormatString("<html><META Http-equiv=\"refresh\" Content=\"%d\">", (int)refreshSeconds);

			return html;
		}

		// Web content simulator.  Compare actual results with simulator results.
		string CalcPageTrailerHtml()
		{
			return _T("</html>");
		}

		// Web content simulator.  Compare actual results with simulator results.
		string CalcCommonHeaderHtml(const CWebDiagnosticsCommonInfo &commonInfo)
		{
			std::string activeSkinName, activeSkinVersion, activeSkinRevision;
			std::string pendingSkinName, pendingSkinVersion, pendingSkinRevision, pendingSkinMediaCount;

			activeSkinName = commonInfo.m_activeSkinInfo.GetName();
			if (!activeSkinName.empty())
			{
				activeSkinVersion = commonInfo.m_activeSkinInfo.GetVersion();
				activeSkinRevision = std::to_string(commonInfo.m_activeSkinInfo.GetRevision());
			}

			pendingSkinName = commonInfo.m_pendingSkinInfo.GetName();
			if (!pendingSkinName.empty())
			{
				pendingSkinVersion = commonInfo.m_pendingSkinInfo.GetVersion();
				pendingSkinRevision = std::to_string(commonInfo.m_pendingSkinInfo.GetRevision());
				pendingSkinMediaCount = std::to_string(commonInfo.m_pendingSkinInfo.GetPendingMediaCount());
			}

			std::string activeCampaignName, activeCampaignId, activeCampaignRevision;
			std::string pendingCampaignName, pendingCampaignId, pendingCampaignRevision, pendingCampaignMediaCount;

			activeCampaignName = commonInfo.m_activeCampaignInfo.GetName();
			if (!activeCampaignName.empty())
			{
				activeCampaignId = std::to_string(commonInfo.m_activeCampaignInfo.GetId());
				activeCampaignRevision = std::to_string(commonInfo.m_activeCampaignInfo.GetRevision());
			}

			pendingCampaignName = commonInfo.m_pendingCampaignInfo.GetName();
			if (!pendingCampaignName.empty())
			{
				pendingCampaignId = std::to_string(commonInfo.m_pendingCampaignInfo.GetId());
				pendingCampaignRevision = std::to_string(commonInfo.m_pendingCampaignInfo.GetRevision());
				pendingCampaignMediaCount = std::to_string(commonInfo.m_pendingCampaignInfo.GetPendingMediaCount());
			}

			string html;
			html += FormatString(_T("Startup Time: %s<br>"), static_cast<LPCTSTR>(CUtilities::FormatDateTimeString(commonInfo.m_startupTime, "%m/%d/%Y %I:%M:%S %p").c_str()));
			html += FormatString(_T("Current Time: %s<br>"), static_cast<LPCTSTR>(CUtilities::FormatDateTimeString(commonInfo.m_currentTime, "%m/%d/%Y %I:%M:%S %p").c_str()));
			html += FormatString(_T("SlotMast ID: %ld<br>"), (long)commonInfo.m_slotMastID);
			html += FormatString(_T("Machine Number:%ld<br>"), (long)commonInfo.m_machineNumber); // No space after ':' for backwards compatibility.
			html += FormatString(_T("Sentinel Version: %s<br>"), static_cast<LPCTSTR>(commonInfo.m_sentinelVersion.c_str()));
			html += FormatString(_T("OS Image Version: %s<br>"), static_cast<LPCTSTR>(commonInfo.m_osImageVersion.c_str()));
			html += FormatString(_T("Active Skin Name: %s<br>"), static_cast<LPCTSTR>(activeSkinName.c_str()));
			html += FormatString(_T("Active Skin Version: %s<br>"), static_cast<LPCTSTR>(activeSkinVersion.c_str()));
			html += FormatString(_T("Active Skin Revision: %s<br>"), static_cast<LPCTSTR>(activeSkinRevision.c_str()));

			if (!pendingSkinName.empty())
			{
				html += FormatString(_T("Pending Skin Name: %s<br>"), static_cast<LPCTSTR>(pendingSkinName.c_str()));
				html += FormatString(_T("Pending Skin Version: %s<br>"), static_cast<LPCTSTR>(pendingSkinVersion.c_str()));
				html += FormatString(_T("Pending Skin Revision: %s<br>"), static_cast<LPCTSTR>(pendingSkinRevision.c_str()));
				html += FormatString(_T("Pending Skin Media Count Still To Download: %s<br>"), static_cast<LPCTSTR>(pendingSkinMediaCount.c_str()));
			}

			html += FormatString(_T("Active Campaign Name: %s<br>"), static_cast<LPCTSTR>(activeCampaignName.c_str()));
			html += FormatString(_T("Active Campaign Id: %s<br>"), static_cast<LPCTSTR>(activeCampaignId.c_str()));
			html += FormatString(_T("Active Campaign Revision: %s<br>"), static_cast<LPCTSTR>(activeCampaignRevision.c_str()));

			if (!pendingCampaignName.empty())
			{
				html += FormatString(_T("Pending Campaign Name: %s<br>"), static_cast<LPCTSTR>(pendingCampaignName.c_str()));
				html += FormatString(_T("Pending Campaign Id: %s<br>"), static_cast<LPCTSTR>(pendingCampaignId.c_str()));
				html += FormatString(_T("Pending Campaign Revision: %s<br>"), static_cast<LPCTSTR>(pendingCampaignRevision.c_str()));
				html += FormatString(_T("Pending Campaign Media Count Still To Download: %s<br>"), static_cast<LPCTSTR>(pendingCampaignMediaCount.c_str()));
			}

			html += FormatString(_T("Carrier Version: %s<br>"), static_cast<LPCTSTR>(commonInfo.m_carrierVersion.c_str()));
			html += FormatString(_T("VM Version: %s<br>"), static_cast<LPCTSTR>(commonInfo.m_vmVersion.c_str()));
			html += FormatString(_T("Battery Status: %s<br>"), static_cast<LPCTSTR>(commonInfo.m_batteryStatus.c_str()));
            html += FormatString(_T("CPU Temperature (Celsius): %.2f<br>"), ((double)(commonInfo.m_lCPUtemp))/1000);
			html += FormatString(_T("BIOS Version: %s<br>"), static_cast<LPCTSTR>("BIOS"));
			html += FormatString(_T("Processor Type: %s<br>"), static_cast<LPCTSTR>("CPU"));
			html += "Ethernet Type: unknown<br>";
			html += "MCU Type: unknown<br>";
			html += "Touchscreen Type: unknown<br>";
			
			string sizeOS, usedOS, availableOS;
			CUtilities::GetPathMemoryUsageInformation(CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_OS), sizeOS, usedOS, availableOS);
			html += FormatString(_T("Disk Space Info SD Card1 (OS) Size:%s<br>"), sizeOS.c_str());
			html += FormatString(_T("Disk Space Info SD Card1 (OS) Available:%s<br>"), availableOS.c_str());
			html += FormatString(_T("Disk Space Info SD Card1 (OS) Used:%s<br>"), usedOS.c_str());

			string sizeSD, usedSD, availableSD;
			CUtilities::GetPathMemoryUsageInformation(string(CDirectoryManager::SENTINEL_DATA_SD_PATH), sizeSD, usedSD, availableSD);
			html += FormatString(_T("Disk Space Info SD Card1 (SentinelData) Size:%s<br>"), sizeSD.c_str());
			html += FormatString(_T("Disk Space Info SD Card1 (SentinelData) Available:%s<br>"), availableSD.c_str());
			html += FormatString(_T("Disk Space Info SD Card1 (SentinelData) Used:%s<br>"), usedSD.c_str());

			string sizeCT, usedCT, availableCT;
			CUtilities::GetPathMemoryUsageInformation(string(CDirectoryManager::CONTENT_SD_PATH), sizeCT, usedCT, availableCT);
			html += FormatString(_T("Disk Space Info SD Card2 (Content) Size:%s<br>"), sizeCT.c_str());
			html += FormatString(_T("Disk Space Info SD Card2 (Content) Available:%s<br>"), availableCT.c_str());
			html += FormatString(_T("Disk Space Info SD Card2 (Content) Used:%s<br>"), usedCT.c_str());

            string usbPath(CSentinelState::GetUsbFilePath());
        	if (!usbPath.empty())
        	{
				string sizeUSB, usedUSB, availableUSB;
				CUtilities::GetPathMemoryUsageInformation(usbPath, sizeUSB, usedUSB, availableUSB);        		
        		html += FormatString(_T("Disk Space Info USB Size:%s<br>"), sizeUSB.c_str());
        		html += FormatString(_T("Disk Space Info USB Available:%s<br>"), availableUSB.c_str());
        		html += FormatString(_T("Disk Space Info USB Used:%s<br>"), usedUSB.c_str());
        	}
			html += _T("<br>");

			return html;
		}

		string CalcCommonHeaderExHtml(const CWebDiagnosticsCommonInfo &info)
		{
			string html(CalcCommonHeaderHtml(info));

			return html;
		}


		string CalcDiagnosticHtml(const CWebDiagnosticsCommonInfo &info)
		{
			string html;
			html += FormatString(_T("Available Physical Memory(KB): %u<BR>"), (unsigned)(info.m_availablePhysicalMemory / 1024));
			html += FormatString(_T("Poller IP: %s<BR>"), info.m_pollerIP.c_str());
			html += FormatString(_T("Poller Packet Sent Verify Errors: %u<BR>"), (unsigned)info.m_pollerPacketSentVerifyErrors);
			html += FormatString(_T("Poller Packet Version: %u<BR>"), (unsigned)info.m_pollerPacketVersion);
			html += FormatString(_T("Online Status: %s"), info.m_onlineStateStr.c_str());			
			
			return html;
		}

		string CalcDownloadStatusHtml(CFlashStatus downloadStatus)
		{
			std::string filename(downloadStatus.GetFilename());
			std::string html;

			html += FormatString(_T("Download Information<BR>"));
			if (filename.empty())
			{
				html += FormatString(_T("N/A<BR>"));
			}
			else
			{
				html += FormatString(_T("Filename: %s<br>"), filename.c_str());
				html += FormatString(_T("Url: %s<br>"), downloadStatus.GetUrl().c_str());
				html += FormatString(_T("Checksum: %s<br>"), downloadStatus.GetChecksum().c_str());
				html += FormatString(_T("Timestamp: %s<br>"), downloadStatus.GetTimestamp().c_str());

				std::string status = downloadStatus.GetStatus();
				if (status == DOWNLOAD_STATUS_INCOMPATIBLE_FILE_TYPE)
				{
					status = "Failure - Incompatible File Type";
				}
				else if (status == DOWNLOAD_STATUS_MEDIA_CARD_ERROR)
				{
					status = "Failure - Media card missing or full";
				}
				else if (status == DOWNLOAD_STATUS_DOWNLOAD_ERROR)
				{
					status = "Failure - Download Error";
				}
				else if (status == DOWNLOAD_STATUS_CHECKSUM_VALIDATION)
				{
					status = "Validating Checksum...";
				}
				else if (status == DOWNLOAD_STATUS_CHECKSUM_ERROR)
				{
					status = "Failure - Checksum Mismatch";
				}
				else if (status == DOWNLOAD_STATUS_EXTRACTING_CONTENTS)
				{
					status = "Extracting Contents...";
				}
				else if (status == DOWNLOAD_STATUS_EXTRACTION_ERROR)
				{
					status = "Failure - Extraction Error";
				}
				else if (status == DOWNLOAD_STATUS_SUCCESS)
				{
					status = "Success";
				}
				else if (status.empty())
				{
					status = "In Progress";
				}
				else
				{
					status = "Unknown";
				}

				html += FormatString(_T("Status: %s<br>"), status.c_str());
			}

			html += _T("<br>");
			return html;
		}

		string CalcErrorsFoundHtml(bool errorsFound)
		{
			string html;
			if (errorsFound)
			{
				html += FormatString(_T("ERRORS FOUND - See %s<BR>"), static_cast<LPCTSTR>(m_errorDumpFileName.c_str()));
			}

			return html;
		}

		string CalcCommonTrailerHtml(const CWebDiagnosticsCommonInfo &info, bool errorsFound)
		{
			string html(CalcDiagnosticHtml(info));
			html += _T("<BR><BR>");
			html += CalcErrorsFoundHtml(errorsFound);

			return html;
		}

		std::string GetPendingActionText(const std::string pendingAction)
		{
			std::string localizedPendingAction;

			if (pendingAction == "NO_PENDING_ACTION")
			{
				localizedPendingAction = "None";
			}
			else if (pendingAction == "RESTART_UI")
			{
				localizedPendingAction = "Restart UI";
			}
			else if (pendingAction == "REBOOT_AFTER_DROP")
			{
				localizedPendingAction = "Reboot after drop  - met memory threshold criteria";
			}
			else if (pendingAction == "REBOOT_ASAP")
			{
				localizedPendingAction = "Reboot ASAP - met memory threshold criteria";
			}
			else if (pendingAction == "OS_UPGRADE")
			{
				localizedPendingAction = "OS Upgrade";
			}
			else if (pendingAction == "REMOTE_REBOOT")
			{
				localizedPendingAction = "Remote Reboot";
			}
			else if (pendingAction == "REFRESH_UI")
			{
				localizedPendingAction = "Refresh UI";
			}

			return localizedPendingAction;
		}

		string CalcStatusHtml(const CWebDiagnosticsStatusInfo &info)
		{
			string timeSinceFloorLogixUpdate(_T("N/A"));
			if (info.m_tickCountOfLastFloorLogixTimeUpdateAvailable)
			{
				DWORD elapsedTickCount = info.m_currentTickCount - info.m_tickCountOfLastFloorLogixTimeUpdate;
				const time_t timeSpan(elapsedTickCount / TICK_COUNTS_PER_SECOND);
				timeSinceFloorLogixUpdate = CUtilities::FormatTimeSpanString(timeSpan);
			}

			string timeSinceFloorLogixConfigString(_T("N/A"));
			if (info.m_timeOfLastFloorLogixConfigAvailable)
			{
				const time_t timeSpan(info.m_currentTime - info.m_timeOfLastFloorLogixConfig);
				timeSinceFloorLogixConfigString = CUtilities::FormatTimeSpanString(timeSpan);
			}

			string lastUIRestartTimeString(_T("N/A"));
			if (info.m_uiRestartCount > 0)
			{
				lastUIRestartTimeString = CUtilities::FormatDateTimeString(info.m_lastUIRestartTime, "%m/%d/%Y %I:%M:%S %p");
			}
			
			string lastUIRefreshTimeString(_T("N/A"));
			if (info.m_uiRefreshCount > 0)
			{
				lastUIRefreshTimeString = CUtilities::FormatDateTimeString(info.m_lastUIRefreshTime, "%m/%d/%Y %I:%M:%S %p");
			}

			string html;
			html += FormatString("Time Since FloorLogix Time Update: %s<br>", static_cast<LPCTSTR>(timeSinceFloorLogixUpdate.c_str()));
			html += FormatString("Time Since FloorLogix Config: %s<br>", static_cast<LPCTSTR>(timeSinceFloorLogixConfigString.c_str()));
			html += FormatString("Maintenance Mode: %s<br>", info.m_maintenanceMode ? "Yes" : "No");
			html += FormatString("Time zone offset: %d:%2.2u<br>",
				(unsigned)(info.m_timeZoneOffsetInMinutes / 60),
				(unsigned)(info.m_timeZoneOffsetInMinutes % 60));
			html += FormatString("Code For Last Shutdown: %d<br>", (int)info.m_tiltCodeForLastShutdown);
			html += FormatString("Reason For Last Shutdown: %s<br>", static_cast<LPCTSTR>(info.TiltCodeToString(info.m_tiltCodeForLastShutdown).c_str()));
			html += FormatString("Last UI Restart: %s<br>", static_cast<LPCTSTR>(lastUIRestartTimeString.c_str()));
			html += FormatString("UI Restart Count: %u<br>", (unsigned)info.m_uiRestartCount);
			html += FormatString("Last UI Refresh: %s<br>", static_cast<LPCTSTR>(lastUIRefreshTimeString.c_str()));
			html += FormatString("UI Refresh Count: %u<br>", (unsigned)info.m_uiRefreshCount);
			html += FormatString("MAC Address1: %s<br>", static_cast<LPCTSTR>(info.m_macAddress.c_str()));
			html += FormatString("MAC Address2: %s<br>", static_cast<LPCTSTR>(info.m_macAddress2.c_str()));
			html += FormatString("Maximum PBT Amount: %s<br>", static_cast<LPCTSTR>(CUnitTestingUtils::GetFormattedCashString(info.m_maxPBTAmount).c_str()));
			html += FormatString("Maximum Ticket Amount: %s<br>", static_cast<LPCTSTR>(CUnitTestingUtils::GetFormattedCashString(info.m_maxTicketAmount).c_str()));
			html += FormatString("LCD Model: %s<br>", static_cast<LPCTSTR>(info.m_lcdModel.c_str()));
			html += FormatString("Pending Action: %s<br>", static_cast<LPCTSTR>(GetPendingActionText(info.m_pendingAction).c_str()));

			// Display memory thresholds last, as the memory available could be displayed next.
			html += FormatString("Available Physical Memory Threshold for Reboot After Drop(KB): %u<br>", (unsigned)info.m_rebootAfterDropThreshold * 1024);
			html += FormatString("Available Physical Memory Threshold for Reboot ASAP(KB): %u<br>", (unsigned)info.m_rebootASAPThreshold * 1024);
			html += FormatString("Available Physical Memory Threshold for UI Restart After Drop(KB): %u<br>", (unsigned)info.m_uiRestartAfterDropThreshold * 1024);
			html += FormatString("Available Physical Memory Threshold for UI Restart ASAP(KB): %u<br>", (unsigned)info.m_uiRestartAsapThreshold * 1024);
			html += FormatString("<br>Total Number of Games: %u<br>", (unsigned)info.m_numberSubgames);
			html += FormatString("Total Number of Subgames Enabled: %u<br>", (unsigned)info.m_numberSubgamesEnabled);			
			if (info.m_isAllGamesFetched)
			{
				string lastGamelistTimestampString = CUtilities::FormatDateTimeString(info.m_lastGamelistTimestamp, "%m/%d/%Y %I:%M:%S %p");
				html += FormatString("Last Games List Gathered Timestamp : %s", lastGamelistTimestampString.c_str());
			}
			else
			{
				html += FormatString("Last Games List Gathered Timestamp : In-Progress");
			}
			html += FormatString("<br>Game List Chunks Sent : %u out of %u<br>", info.m_currentGameChunkIndex, info.m_totalGameListChunks);
			html += _T("<br>");

			return html;
		}
		
		string CalcMultiplierAndDivider(const CWebDiagnosticsStatusInfo &statusInfo)
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

		string CalcStatusPageHtml(const CWebDiagnosticsCommonInfo &commonInfo,
			const CWebDiagnosticsStatusInfo &statusInfo,
			const CPlayerSession &playerSession,
			const CEGMConfig &egmConfig,
			const CExcessiveVoucherOut& excessiveVoucherOut,
			bool errorsFound)
		{
			string html;
			html += CalcPageHeaderHtml(m_statusDumpRefreshSeconds);
			html += CalcCommonHeaderExHtml(commonInfo);
			html += CalcStatusHtml(statusInfo);
			html += CalcDiagnosticHtml(commonInfo);
			html += _T("<BR><BR>");
			html += CalcLastGameInfo(playerSession);
			html += CalcMultiplierAndDivider(statusInfo);
			html += CalcEgmConfigHtml(statusInfo, egmConfig);
			html += excessiveVoucherOut.GetHtmlString();
			html += CalcDownloadStatusHtml(statusInfo.m_downloadStatus);
			html += CalcErrorsFoundHtml(errorsFound);
			html += CalcPageTrailerHtml();

			return html;
		}

		string CalcProcessHtml(const CProcess &process)
		{
			string output;
			const CProcess *proc(&process);

			// Convert and report CPU time using the full FILETIME range,
			// as the low DWORD of a FILETIME only has a range of about 429 seconds.
			DWORD cpuHours;
			DWORD cpuMinutes;
			DWORD cpuSeconds;
			DWORD cpuMilliseconds;
			CUtilities::FileTimeToComponents(proc->m_totalProcTime, cpuHours, cpuMinutes, cpuSeconds, cpuMilliseconds);

			output = FormatString("<tr>" // No comma.
				"<td>Process:</td><td> %-20s</td>" // No comma.
				"<td>CPU Usage(%%):</td><td align=\"right\"> %d</td>" // No comma.
				"<td>Module Size(KB):</td><td align=\"right\"> %-10u</td>" // No comma.
				"<td>VM Size(KB):</td><td align=\"right\"> %-10u</td>" // No comma.
				"<td>CPU Time:</td><td align=\"right\"> %u:%02u:%02u.%03u</td>" // No comma.
				"<td>Handles:</td><td align=\"right\"> %-10u</td>"  // No comma.
				"</tr>",
				// Data arguments.
				proc->m_processName.c_str(),
				proc->m_cpuUsage,
				proc->m_moduleSize,
				proc->m_vmSize,
				(unsigned)cpuHours, (unsigned)cpuMinutes, (unsigned)cpuSeconds, (unsigned)cpuMilliseconds,
				(unsigned)proc->m_handleCount);

			return output;
		}

		string CalcTableHeaderHtml()
		{
			string html;
			html += _T("<table cellspacing=\"10\">");

			return html;
		}

		string CalcTableTrailerHtml()
		{
			string html;
			html += _T("</table>");

			return html;
		}

		string CalcProcessPageHtml(const CWebDiagnosticsCommonInfo &commonInfo,
			const CProcess &process1,
			const CProcess &process2,
			bool errorsFound)
		{
			string html;
			html += CalcPageHeaderHtml(m_memoryDumpRefreshSeconds);
			html += CalcCommonHeaderExHtml(commonInfo);

			html += CalcTableHeaderHtml();
			html += CalcProcessHtml(process1);
			html += CalcProcessHtml(process2);
			html += CalcTableTrailerHtml();

			html += CalcCommonTrailerHtml(commonInfo, errorsFound);
			html += CalcPageTrailerHtml();

			return html;
		}

		string CalcErrorFileHtml(const std::shared_ptr<std::istream> &errorFile, UINT bufferSize)
		{
			string html;

			if (CUnitTestingUtils::GetLength(errorFile) > 0)
			{
				ios::pos_type filePosition(errorFile->tellg());

				// Build the html string from the back the file and then reverse the results.
				errorFile->seekg(0, ios::end);
				while (errorFile->tellg() > 0 && bufferSize > 0)
				{
					errorFile->seekg(-1, ios::cur);
					char ch;
					if (errorFile->read(&ch, sizeof(ch)))
					{
						errorFile->seekg(-1, ios::cur);
						if ('\n' == ch)
						{
							// Is there enough room to convert the linefeed to a tag?
							if (bufferSize >= 5)
							{
								// Add the tag in reverse order.
								html += ">RB<";
								html.push_back(ch);
								bufferSize -= 5;

							}
							else
							{
								// No room for the tag.
								bufferSize = 0;
							}
						}
						else
						{
							html += ch;
							--bufferSize;
						}
						
					}
					else
					{
						bufferSize = 0; // Unexpected error reading from file.
					}

				}
				errorFile->seekg(filePosition, ios::beg);

				std::reverse(html.begin(), html.end());
			}

			return html;
		}

		string CalcErrorFileHtml(const char *errorFileContents,
			UINT errorFileContentsLength,
			UINT bufferSize = m_maximumErrorHtmlSize)
		{
			bool partial;
			return CalcErrorFileHtml(errorFileContents, errorFileContentsLength, bufferSize, partial);
		}
			
		string CalcErrorFileHtml(const char *errorFileContents,
			UINT errorFileContentsLength,
			UINT bufferSize,
			bool &partial)
		{
			string html;

			if (nullptr == errorFileContents)
			{
				errorFileContentsLength = 0;
			}
			else
			{
				// Build the html string from the back the file and then reverse the results.
				errorFileContents += errorFileContentsLength;
			}

			// Build the html string from the back the file and then reverse the results.
			while (errorFileContentsLength > 0 && bufferSize > 0)
			{
				--errorFileContentsLength;
				char ch((char)*--errorFileContents);
				if ('\n' == ch)
				{
					// Is there enough room to convert the linefeed to a tag?
					if (bufferSize >= 5)
					{
						// Add the tag in reverse order.
						html += ">RB<";
						html.push_back(ch);
						bufferSize -= 5;
					}
					else
					{
						// No room for the tag.
						bufferSize = 0;
					}
				}
				else
				{
					html += ch;
					--bufferSize;
				}
			}

			std::reverse(html.begin(), html.end());
			partial = errorFileContentsLength > 0; // true if only part of the error file contents is return;
			return html;
		}

		string CalcErrorPageHtml(const CWebDiagnosticsCommonInfo &commonInfo, const std::shared_ptr<std::istream> &errorFile, UINT bufferSize)
		{
			string html;
			html += "<html>";
			html += CalcCommonHeaderHtml(commonInfo);
			html += CalcDiagnosticHtml(commonInfo);
			html += _T("<br><br>");
			html += _T("Debug data:<br>");
			html += CalcErrorFileHtml(errorFile, bufferSize);
			html += CalcPageTrailerHtml();

			return html;
		}

		string CalcErrorPageHtml(const CWebDiagnosticsCommonInfo &commonInfo,
			const char *errorFileContents,
			UINT errorFileContentsLength,
			UINT bufferSize = m_maximumErrorHtmlSize)
		{
			string html;
			html += "<html>";
			html += CalcCommonHeaderHtml(commonInfo);
			html += CalcDiagnosticHtml(commonInfo);
			html += _T("<br><br>");

			bool partial;
			string errorFileHtml = CalcErrorFileHtml(errorFileContents, errorFileContentsLength, bufferSize, partial);
			html += partial ? _T("Debug data (partial):<br>") : _T("Debug data:<br>");
			html += errorFileHtml;
			html += CalcPageTrailerHtml();

			return html;
		}

		string CalcLastGameInfo(const CPlayerSession &playerSession);
		string CalcEgmProtocolString(EGMProtocol egmProtocol);
		string CalcEgmTypeString(EGMType egmType);
		string CalcEgmConfigHtml(const CWebDiagnosticsStatusInfo &statusInfo, const CEGMConfig &egmConfig);

	};

		/// <summary>
		/// Code coverage and functional test for:
		/// CWebDiagnostics::Write()
		/// </summary>
		TEST_F(WebDiagnostics, WriteTest)
		{
			// Inputs.
			LPCTSTR string(_T("string value"));

			// Expected results.
			const UINT bufferLengthExpected(12);
			const BYTE bufferExpected[bufferLengthExpected + 1] = { "string value" };

			// Setup.
			shared_ptr<stringstream> file(new stringstream);
			CWebDiagnostics wb;

			// Perform operation(s) to be tested.
			wb.Write(file, string);

			// Test(s).
			ASSERT_TRUE(CUnitTestingUtils::IsEqual(file, bufferExpected, bufferLengthExpected)) << _T("Write() IsEqual");
		}

		/// <summary>
		/// Code coverage and functional test for:
		/// CWebDiagnostics::Write()
		/// </summary>
		TEST_F(WebDiagnostics, WriteStringTest)
		{
			// Inputs.
			std::string str(_T("string value"));

			// Expected results.
			const UINT bufferLengthExpected(12);
			const BYTE bufferExpected[bufferLengthExpected + 1] = { "string value" };

			// Setup.
			shared_ptr<stringstream> file(new stringstream);
			CWebDiagnostics wb;

			// Perform operation(s) to be tested.
			wb.Write(file, str);

			// Test(s).
			ASSERT_TRUE(CUnitTestingUtils::IsEqual(file, bufferExpected, bufferLengthExpected)) << _T("Write() IsEqual");
		}

		/// <summary>
		/// Code coverage and functional test for:
		/// CWebDiagnostics::GetDiagString()
		/// </summary>
		TEST_F(WebDiagnostics, GetDiagStringTest)
		{
			// Inputs.
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
				"Sentinel is offline",
				"",
				"",
				"Low/no battery",
				0.0);

			// Expected results.
			const string htmlExpected(CalcDiagnosticHtml(commonInfo));

			// Setup.
			CWebDiagnostics wb;

			// Perform operation(s) to be tested.
			string htmlResult(wb.GetDiagString(commonInfo));

			// Test(s).
			ASSERT_EQ(htmlExpected, htmlResult) << _T("GetDiagString() hmtl");
		}

		/// <summary>
		/// Code coverage and functional test for:
		/// CWebDiagnostics::GetCommonHeaderHtml()
		/// </summary>
		TEST_F(WebDiagnostics, GetCommonHeaderHtmlTest)
		{
			// Inputs.
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
				"Sentinel is offline",
                "", 
				"",
				"OK",
				0.0);

			// Expected results.
			const string htmlExpected(CalcCommonHeaderHtml(commonInfo));

			// Setup.
			CWebDiagnostics wb;

			// Perform operation(s) to be tested.
			string htmlResult(wb.GetCommonHeaderHtml(commonInfo));

			// Test(s).
			ASSERT_EQ(htmlExpected, htmlResult) << _T("GetCommonHeaderHtml() hmtl");
		}

		/// <summary>
		/// Code coverage and functional test for:
		/// CWebDiagnostics::GetCommonHeaderHtml()
		/// </summary>
		TEST_F(WebDiagnostics, GetCommonHeaderHtml_woSmiTest)
		{
			// Inputs.
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
				"Sentinel is offline",
				"",
				"",
				"",
				0.0);

			// Expected results.
			const string htmlExpected(CalcCommonHeaderExHtml(commonInfo));

			// Setup.
			CWebDiagnostics wb;

			// Perform operation(s) to be tested.
			string htmlResult(wb.GetCommonHeaderHtml(commonInfo));

			// Test(s).
			ASSERT_EQ(htmlExpected, htmlResult) << _T("GetCommonHeaderHtml() hmtl");
		}

		/// <summary>
		/// Code coverage and functional test for:
		/// CWebDiagnostics::GetCommonHeaderHtml()
		/// </summary>
		TEST_F(WebDiagnostics, GetCommonHeaderHtml_wSmiTest)
		{
			// Inputs.
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
				8, "Sentinel is offline", "", "", "", 0.0);

			// Expected results.  Note the use of military time instead of AM/PM for SpeedMediaInfo.
			const string htmlExpected(CalcCommonHeaderExHtml(commonInfo));

			// Setup.
			CWebDiagnostics wb;

			// Perform operation(s) to be tested.
			string htmlResult(wb.GetCommonHeaderHtml(commonInfo));

			// Test(s).
			ASSERT_EQ(htmlExpected, htmlResult) << _T("GetCommonHeaderHtml() hmtl");
		}

		/// <summary>
		/// Code coverage and functional test for:
		/// CWebDiagnostics::GetCommonTrailerHtml()
		/// </summary>
		TEST_F(WebDiagnostics, GetCommonTrailerHtml_NoErrorsTest)
		{
			// Inputs.
			const time_t startupTime(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12));
			const time_t currentTime(CUtilities::GetTimeFromFields(2014, 1, 27, 13, 14, 15));
			const bool errorsFound(false);

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
				8, "Sentinel is offline", "", "", "", 0.0);

			// Expected results.
			const string htmlExpected(CalcCommonTrailerHtml(commonInfo, errorsFound));

			// Setup.
			CWebDiagnostics wb;

			// Perform operation(s) to be tested.
			string htmlResult(wb.GetCommonTrailerHtml(commonInfo, errorsFound));

			// Test(s).
			ASSERT_EQ(htmlExpected, htmlResult) << _T("GetCommonTrailerHtml() hmtl");
		}

		/// <summary>
		/// Code coverage and functional test for:
		/// CWebDiagnostics::GetCommonTrailerHtml()
		/// </summary>
		TEST_F(WebDiagnostics, GetCommonTrailerHtml_ErrorsTest)
		{
			// Inputs.
			const bool errorsFound(true);

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
				8, "Sentinel is offline", "", "", "", 0.0);

			// Expected results.
			const string htmlExpected(CalcCommonTrailerHtml(commonInfo, errorsFound));

			// Setup.
			CWebDiagnostics wb;

			// Perform operation(s) to be tested.
			string htmlResult(wb.GetCommonTrailerHtml(commonInfo, errorsFound));

			// Test(s).
			ASSERT_EQ(htmlExpected, htmlResult) << _T("GetCommonTrailerHtml() hmtl");
		}

		/// <summary>
		/// Code coverage and functional test for:
		/// CWebDiagnostics::GetStatusHtml()
		/// </summary>
		TEST_F(WebDiagnostics, GetStatusHtmlTest_1)
		{
			// Inputs.
			const CFlashStatus downloadStatus(FlashType::FLASH_NONE, 0, "", "", "", "");
			const CWebDiagnosticsStatusInfo statusInfo(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12),
				1000000 + (1 * SECONDS_PER_MINUTE + 2) * TICK_COUNTS_PER_SECOND,
				true,
				1000000,
				true,
				CUtilities::GetTimeFromFields(2014, 1, 27, 10, 01, 03),
				-480,
				(TiltCode)0x900,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				0,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				0,
				_T("20-6F-EC-00-08-09"),
				_T("20-6F-EC-00-08-08"),
				125,
				75,
				225,
				175,
				-3000 * 100,
				0,
				_T("Kyocera"),
				_T("NO_PENDING_ACTION"),
				_T("602"),
				true,
				false,
                CURRENT_MULTIPLIER,
                CURRENT_DIVIDER,
                2,
				1,
                0,
                true,
				EGMTransferMode_AFT,
				"SAS_AFT_BONUS",
                2,
                3,
                false,
				downloadStatus);

			// Expected results.
			const string htmlExpected(CalcStatusHtml(statusInfo));

			// Setup.
			CWebDiagnostics wb;

			// Perform operation(s) to be tested.
			string htmlResult(wb.GetStatusHtml(statusInfo));

			// Test(s).
			ASSERT_EQ(htmlExpected, htmlResult) << _T("GetStatusHtml() hmtl");
		}

		/// <summary>
		/// Code coverage and functional test for:
		/// CWebDiagnostics::GetStatusHtml()
		/// </summary>
		TEST_F(WebDiagnostics, GetStatusHtmlTest_2)
		{
			// Inputs.
			const CFlashStatus downloadStatus(FlashType::FLASH_NONE, 50, "a1b2c3d4", "10:24 AM", "os.zip", "http://127.12.1.14/os.zip");
			const CWebDiagnosticsStatusInfo statusInfo(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12),
				1000000 + (1 * SECONDS_PER_MINUTE + 2) * TICK_COUNTS_PER_SECOND,
				true,
				1000000,
				true,
				CUtilities::GetTimeFromFields(2014, 1, 27, 10, 01, 03),
				-480,
				(TiltCode)0x900,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				0,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				0,
				_T("20-6F-EC-00-08-09"),
				_T("20-6F-EC-00-08-08"),
				125,
				75,
				225,
				175,
				-3000 * 100,
				0,
				_T("Kyocera"),
				_T("REMOTE_REBOOT"),
				_T("602"),
				true,
				false,
				INVALID_MULTIPLIER,
				INVALID_DIVIDER,
				2,
				1,
				0,
				true,
				EGMTransferMode_AFT,
				"SAS_LEGACY_BONUS",
				2,
				3,
				false,
				downloadStatus);

			// Expected results.
			const string htmlExpected(CalcStatusHtml(statusInfo));

			// Setup.
			CWebDiagnostics wb;

			// Perform operation(s) to be tested.
			string htmlResult(wb.GetStatusHtml(statusInfo));

			// Test(s).
			ASSERT_EQ(htmlExpected, htmlResult) << _T("GetStatusHtml() hmtl");
		}

		/// <summary>
		/// Code coverage and functional test for:
		/// CWebDiagnostics::GetStatusHtml()
		/// </summary>
		TEST_F(WebDiagnostics, GetStatusHtmlTest_3)
		{
			// Inputs.
			CFlashStatus downloadStatus(FlashType::FLASH_NONE, 100, "a1b2c3d4", "10:24 AM", "os.zip", "http://127.12.1.14/os.zip");
			downloadStatus.SetStatus(DOWNLOAD_STATUS_SUCCESS);
			const CWebDiagnosticsStatusInfo statusInfo(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12),
				1000000 + (1 * SECONDS_PER_MINUTE + 2) * TICK_COUNTS_PER_SECOND,
				true,
				1000000,
				true,
				CUtilities::GetTimeFromFields(2014, 1, 27, 10, 01, 03),
				-480,
				(TiltCode)0x900,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				0,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				0,
				_T("20-6F-EC-00-08-09"),
				_T("20-6F-EC-00-08-08"),
				125,
				75,
				225,
				175,
				-3000 * 100,
				0,
				_T("Kyocera"),
				_T("REMOTE_REBOOT"),
				_T("602"),
				true,
				false,
				INVALID_MULTIPLIER,
				INVALID_DIVIDER,
				2,
				1,
				0,
				true,
				EGMTransferMode_AFT,
				"SAS_LEGACY_BONUS",
				2,
				3,
				false,
				downloadStatus);

			// Expected results.
			const string htmlExpected(CalcStatusHtml(statusInfo));

			// Setup.
			CWebDiagnostics wb;

			// Perform operation(s) to be tested.
			string htmlResult(wb.GetStatusHtml(statusInfo));

			// Test(s).
			ASSERT_EQ(htmlExpected, htmlResult) << _T("GetStatusHtml() hmtl");
		}

		/// <summary>
		/// Code coverage and functional test for:
		/// CWebDiagnostics::GetStatusHtml()
		/// </summary>
		TEST_F(WebDiagnostics, GetStatusHtmlTest_4)
		{
			// Inputs.
			CFlashStatus downloadStatus(FlashType::FLASH_NONE, 100, "a1b2c3d4", "10:24 AM", "os.zip", "http://127.12.1.14/os.zip");
			downloadStatus.SetStatus(DOWNLOAD_STATUS_DOWNLOAD_ERROR);
			const CWebDiagnosticsStatusInfo statusInfo(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12),
				1000000 + (1 * SECONDS_PER_MINUTE + 2) * TICK_COUNTS_PER_SECOND,
				true,
				1000000,
				true,
				CUtilities::GetTimeFromFields(2014, 1, 27, 10, 01, 03),
				-480,
				(TiltCode)0x900,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				0,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				0,
				_T("20-6F-EC-00-08-09"),
				_T("20-6F-EC-00-08-08"),
				125,
				75,
				225,
				175,
				-3000 * 100,
				0,
				_T("Kyocera"),
				_T("REMOTE_REBOOT"),
				_T("602"),
				true,
				false,
				INVALID_MULTIPLIER,
				INVALID_DIVIDER,
				2,
				1,
				0,
				true,
				EGMTransferMode_AFT,
				"SAS_LEGACY_BONUS",
				2,
				3,
				false,
				downloadStatus);

			// Expected results.
			const string htmlExpected(CalcStatusHtml(statusInfo));

			// Setup.
			CWebDiagnostics wb;

			// Perform operation(s) to be tested.
			string htmlResult(wb.GetStatusHtml(statusInfo));

			// Test(s).
			ASSERT_EQ(htmlExpected, htmlResult) << _T("GetStatusHtml() hmtl");
		}

		string WebDiagnostics::CalcLastGameInfo(const CPlayerSession &playerSession)
		{
			string html;

			html += "Last Game Played Information<br>";

			html += FormatString("Game In Progress: %s<br>",
				static_cast<LPCTSTR>(CUnitTestingUtils::GetBoolString(playerSession.GetGameInProgress())));
			html += FormatString("Last Game Start: %s<br>",
				static_cast<LPCTSTR>(CUnitTestingUtils::FormatLocalMilitaryDateTimeString(playerSession.GetLastGameStarted()).c_str()));
			html += FormatString("Last Game End: %s<br>",
				static_cast<LPCTSTR>(CUnitTestingUtils::FormatLocalMilitaryDateTimeString(playerSession.GetLastGameEnd()).c_str()));

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
				static_cast<LPCTSTR>(CUnitTestingUtils::GetBoolString(playerSession.GetEgmSendsGameEnd())));

			html += _T("<br>");

			return html;
		}

		string WebDiagnostics::CalcEgmProtocolString(EGMProtocol egmProtocol)
		{
			string egmProtocolString(_T("N/A"));
			switch (egmProtocol)
			{
			case IGT_SAS_3xx:
				egmProtocolString = _T("SAS 3.xx / 4.xx");
				break;
			case SAS5xx:
				egmProtocolString = _T("SAS 5.xx");
				break;
			case SAS6xx:
				egmProtocolString = _T("SAS 6.xx");
				break;
			default:
				egmProtocolString = _T("N/A");
				break;
			}

			return egmProtocolString;
		}

		string WebDiagnostics::CalcEgmTypeString(EGMType egmType)
		{
			string egmTypeString(_T("N/A"));
			switch (egmType)
			{
			case STANDARD:
				egmTypeString = _T("STANDARD");
				break;
			case BALLY:
				egmTypeString = _T("BALLY");
				break;
			case WMS:
				egmTypeString = _T("WMS");
				break;
			case CUSTOM:
				egmTypeString = _T("CUSTOM");
				break;
			case IGTS2000:
				egmTypeString = _T("IGT S2000");
				break;
			case NO_EGM:
				egmTypeString = _T("NONE");
				break;
			}

			return egmTypeString;
		}

		string WebDiagnostics::CalcEgmConfigHtml(const CWebDiagnosticsStatusInfo &statusInfo, const CEGMConfig &egmConfig)
		{
			string html;

			html += "EGM Configuration<br>";
			html += FormatString("EGM Type: %s<br>", static_cast<LPCTSTR>(CalcEgmTypeString(egmConfig.GetEGMType()).c_str()));


			int pollingSpeed = egmConfig.GetEGMPollingSpeed() == MS200 ? 200 : 40; // Same interpretation as CSASProtocol.
			html += FormatString("Polling speed(ms): %d<br>", (int)pollingSpeed);
			html += FormatString("EGM Protocol: %s<br>", static_cast<LPCTSTR>(CalcEgmProtocolString(egmConfig.GetEGMProtocol()).c_str()));

			string reportedEgmProtocolVersion(statusInfo.m_reportedEgmProtocolVersion);
			if (reportedEgmProtocolVersion.empty())
			{
				reportedEgmProtocolVersion = CUnitTestingUtils::LPCTSTR_NA;
			}
			html += FormatString("Reported EGM Protocol Version: %s<br>", static_cast<LPCTSTR>(reportedEgmProtocolVersion.c_str()));

			// For ticket expiration, display general human understandable information, not confidential (non-disclosure) SAS protocol details.
			string ticketExpirationSupport(_T("N/A"));
			if (statusInfo.m_egmSupportsLP7B_Available)
			{
				ticketExpirationSupport = CUnitTestingUtils::GetBoolString(statusInfo.m_egmSupportsLP7B);
			}
			html += FormatString(_T("EGM Supports SAS Long Poll 7B Ticket Expiration &gt 255 Days: %s<br>"),
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

		/// <summary>
		/// Code coverage and functional test for:
		///     CWebDiagnostics::GetLastGameInfo()
		/// Test with default value -- game not in progress and no game start and no game end.
		/// </summary>
		TEST_F(WebDiagnostics, GetLastGameInfo_DefaultTest)
		{
			// Setup.
			const CPlayerSession playerSession;

			// Expected results.
			const string htmlExpected(CalcLastGameInfo(playerSession));

			// Perform operation(s) and test(s).
			ASSERT_EQ(CWebDiagnostics::GetLastGameInfo(playerSession), htmlExpected);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CWebDiagnostics::GetLastGameInfo()
		/// Test with default value, plus game in progress.
		/// </summary>
		TEST_F(WebDiagnostics, GetLastGameInfo_DefaultGameInProgressTest)
		{
			// Setup.
			CConfig config(false, "");
			config.SetGameInProgress(true);
			CPlayerSession playerSession;
			playerSession.UpdateConfigItems(config);

			// Expected results.
			const string htmlExpected(CalcLastGameInfo(playerSession));

			// Perform operation(s) and test(s).
			ASSERT_EQ(CWebDiagnostics::GetLastGameInfo(playerSession), htmlExpected);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CWebDiagnostics::GetLastGameInfo()
		/// Test with default value, plus game not in progress.
		/// </summary>
		TEST_F(WebDiagnostics, GetLastGameInfo_DefaultNotGameInProgressTest)
		{
			// Setup.
			const int testIndex(0);
			CConfig config(false, "");
			config.SetGameInProgress(false);
			CPlayerSession playerSession;
			playerSession.UpdateConfigItems(config);

			// Expected results.
			const string htmlExpected(CalcLastGameInfo(playerSession));

			// Perform operation(s) and test(s).
			ASSERT_EQ(CWebDiagnostics::GetLastGameInfo(playerSession), htmlExpected);
		}

//		/// <summary>
//		/// Code coverage and functional test for:
//		///     CWebDiagnostics::GetLastGameInfo()
//		/// Test with game started, but not game ended.
//		/// </summary>
//		TEST_F(WebDiagnostics, GetLastGameInfo_GameStartedTest)
//		{
//			// Setup.
//			CPlayerSession playerSession;
//			playerSession.GameStarted();
//
//			// Expected results.
//			const string htmlExpected(CalcLastGameInfo(playerSession));
//
//			// Perform operation(s) and test(s).
//			ASSERT_EQ(CWebDiagnostics::GetLastGameInfo(playerSession), htmlExpected);
//		}
//
//		/// <summary>
//		/// Code coverage and functional test for:
//		///     CWebDiagnostics::GetLastGameInfo()
//		/// Test with game ended, but not game started.
//		/// </summary>
//		TEST_F(WebDiagnostics, GetLastGameInfo_GameEndedTest)
//		{
//			// Setup.
//			CPlayerSession playerSession;
//			playerSession.GameEnded();
//
//			// Expected results.
//			const string htmlExpected(CalcLastGameInfo(playerSession));
//
//			// Perform operation(s) and test(s).
//			ASSERT_EQ(CWebDiagnostics::GetLastGameInfo(playerSession), htmlExpected);
//		}
//
//		/// <summary>
//		/// Code coverage and functional test for:
//		///     CWebDiagnostics::GetLastGameInfo()
//		/// Test with game started and then game ended.
//		/// </summary>
//		TEST_F(WebDiagnostics, GetLastGameInfo_GameStartedGameEndedTest)
//		{
//			// Setup.
//			CPlayerSession playerSession;
//			playerSession.GameStarted();
//			playerSession.GameEnded();
//
//			// Expected results.
//			const string htmlExpected(CalcLastGameInfo(playerSession));
//
//			// Perform operation(s) and test(s).
//			ASSERT_EQ(CWebDiagnostics::GetLastGameInfo(playerSession), htmlExpected);
//		}
//
//		/// <summary>
//		/// Code coverage and functional test for:
//		///     CWebDiagnostics::GetLastGameInfo()
//		/// Test with game ended and then game started.
//		/// </summary>
//		TEST_F(WebDiagnostics, GetLastGameInfo_GameEndedGameStartedTest)
//		{
//			// Setup.
//			const int testIndex(0);
//			CPlayerSession playerSession;
//			playerSession.GameEnded();
//			playerSession.GameStarted();
//
//			// Expected results.
//			const string htmlExpected(CalcLastGameInfo(playerSession));
//
//			// Perform operation(s) and test(s).
//			ASSERT_EQ(CWebDiagnostics::GetLastGameInfo(playerSession), htmlExpected);
//		}
//
//		/// <summary>
//		/// Code coverage and functional test for:
//		///     CWebDiagnostics::GetLastGameInfo()
//		/// Test with consecutive game starts so GetEgmSendsGameEnd() returns false.
//		/// </summary>
//		TEST_F(WebDiagnostics, GetLastGameInfo_GetEgmSendsGameEndedFalse)
//		{
//			// Setup.
//			CPlayerSession playerSession;
//			playerSession.GameStarted();
//			playerSession.GameStarted();
//			ASSERT_EQ(playerSession.GetEgmSendsGameEnd(), false); // Verify setup.
//
//			// Expected results.
//			const string htmlExpected(CalcLastGameInfo(playerSession));
//
//			// Perform operation(s) and test(s).
//			ASSERT_EQ(CWebDiagnostics::GetLastGameInfo(playerSession), htmlExpected);
//		}
//
//		/// <summary>
//		/// Code coverage and functional test for:
//		///     CWebDiagnostics::GetLastGameInfo()
//		/// Test with game start followed by game end so GetEgmSendsGameEnd() returns true.
//		/// </summary>
//		TEST_F(WebDiagnostics, GetLastGameInfo_GetEgmSendsGameEndedTrue)
//		{
//			// Setup.
//			CPlayerSession playerSession;
//			playerSession.GameStarted();
//			playerSession.GameEnded();
//			ASSERT_EQ(playerSession.GetEgmSendsGameEnd(), true); // Verify setup.
//
//			// Expected results.
//			const string htmlExpected(CalcLastGameInfo(playerSession));
//
//			// Perform operation(s) and test(s).
//			ASSERT_EQ(CWebDiagnostics::GetLastGameInfo(playerSession), htmlExpected);
//		}
//
//// #pragma region GetEGMProtocolString
//		/// <summary>
//		/// Functional test for:
//		///     CWebDiagnostics::GetEGMProtocolString()
//		/// Test specific enumerations of interest.
//		/// </summary>
//		TEST_F(WebDiagnostics, GetEGMProtocolString_EnumTest)
//		{
//			ASSERT_TRUE(CalcEgmProtocolString(IGT_SAS_3xx) == CWebDiagnostics::GetEgmProtocolString(IGT_SAS_3xx)) << L"IGT_SAS_3xx";
//			ASSERT_TRUE(CalcEgmProtocolString(SAS5xx) == CWebDiagnostics::GetEgmProtocolString(SAS5xx)) << L"SAS5xx";
//			ASSERT_TRUE(CalcEgmProtocolString(SAS6xx) == CWebDiagnostics::GetEgmProtocolString(SAS6xx)) << L"SAS6xx";
//		}
//
//		/// <summary>
//		/// Functional test for:
//		///     CWebDiagnostics::GetEGMProtocolString()
//		/// Test a range of possible values.
//		/// </summary>
//		TEST_F(WebDiagnostics, GetEGMProtocolString_RangeTest)
//		{
//			// Perform operation(s) and test(s).
//			for (int testIndex(-1); testIndex < SAS6xx + 1; ++testIndex)
//			{
//				EGMProtocol egmProtocol((EGMProtocol)testIndex);
//				ASSERT_TRUE(CalcEgmProtocolString(egmProtocol) == CWebDiagnostics::GetEgmProtocolString(egmProtocol)) << L"GetEgmProtocolString";
//			}
//		}
// #pragma endregion GetEGMProtocolString

// #pragma region GetEgmConfigHtml_PollingSpeed
		/// <summary>
		/// Functional test for:
		///     CWebDiagnostics::GetEgmConfigHtml()
		/// Test with CEGMConfig::SetPollingSpeed(MS40).
		/// </summary>
		TEST_F(WebDiagnostics, GetEgmConfigHtml_PollingSpeedMS40)
		{
			// Setup.
			const CFlashStatus downloadStatus(FlashType::FLASH_NONE, 0, "", "", "", "");
			const CWebDiagnosticsStatusInfo statusInfo(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12),
				1000000 + (1 * SECONDS_PER_MINUTE + 2) * TICK_COUNTS_PER_SECOND,
				true,
				1000000,
				true,
				CUtilities::GetTimeFromFields(2014, 1, 27, 10, 01, 03),
				-480,
				(TiltCode)0x900,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				0,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				0,
				_T("20-6F-EC-00-08-09"),
				_T("20-6F-EC-00-08-08"),
				125,
				75,
				225,
				175,
				-3000 * 100,
				LONG_MIN,
				_T("Kyocera"),
				_T("Action"),
				_T("602"),
				true,
				false,
                CURRENT_MULTIPLIER,
                CURRENT_DIVIDER,
                2,
				1,
                0,
                false,
				EGMTransferMode_AFT,
				"SAS_AFT_BONUS",
                2,
                3,
                false,
				downloadStatus);

			CEGMConfig egmConfig(MEMORY_NONE);
			egmConfig.SetEGMPollingSpeed(MS40);
			egmConfig.SetEGMType(STANDARD);

			// Expected results.
			const string htmlExpected(CalcEgmConfigHtml(statusInfo, egmConfig));

			// Perform operation(s) and test(s).
			ASSERT_EQ(CWebDiagnostics::GetEgmConfigHtml(statusInfo, egmConfig), htmlExpected);
		}

		/// <summary>
		/// Functional test for:
		///     CWebDiagnostics::GetEgmConfigHtml()
		/// Test with CEGMConfig::SetPollingSpeed(MS200).
		/// </summary>
		TEST_F(WebDiagnostics, GetEgmConfigHtml_PollingSpeedMS200)
		{
			// Setup.
			const CFlashStatus downloadStatus(FlashType::FLASH_NONE, 0, "", "", "", "");
			const CWebDiagnosticsStatusInfo statusInfo(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12),
				1000000 + (1 * SECONDS_PER_MINUTE + 2) * TICK_COUNTS_PER_SECOND,
				true,
				1000000,
				true,
				CUtilities::GetTimeFromFields(2014, 1, 27, 10, 01, 03),
				-480,
				(TiltCode)0x900,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				0,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				0,
				_T("20-6F-EC-00-08-09"),
				_T("20-6F-EC-00-08-08"),
				125,
				75,
				225,
				175,
				-3000 * 100,
				LONG_MIN,
				_T("Kyocera"),
				_T("Action"),
				_T("602"),
				true,
				false,
                CURRENT_MULTIPLIER,
                CURRENT_DIVIDER,
                2,
				1,
                0,
                false,
				EGMTransferMode_AFT,
				"SAS_AFT_BONUS",
                2,
                3,
                false,
				downloadStatus);

			CEGMConfig egmConfig(MEMORY_NONE);
			egmConfig.SetEGMPollingSpeed(MS200);

			// Expected results.
			const string htmlExpected(CalcEgmConfigHtml(statusInfo, egmConfig));

			// Perform operation(s) and test(s).
			ASSERT_EQ(CWebDiagnostics::GetEgmConfigHtml(statusInfo, egmConfig), htmlExpected);
		}
// #pragma endregion GetEgmConfigHtml_PollingSpeed

// #pragma region GetEgmConfigHtml_EgmProtocol
		/// <summary>
		/// Functional test for:
		///     CWebDiagnostics::GetEgmConfigHtml()
		/// Test specific enumerations of interest.
		/// </summary>
		TEST_F(WebDiagnostics, GetEgmConfigHtml_EgmProtocol_EnumTest)
		{
			// Setup.
			const CFlashStatus downloadStatus(FlashType::FLASH_NONE, 0, "", "", "", "");
			const CWebDiagnosticsStatusInfo statusInfo(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12),
				1000000 + (1 * SECONDS_PER_MINUTE + 2) * TICK_COUNTS_PER_SECOND,
				true,
				1000000,
				true,
				CUtilities::GetTimeFromFields(2014, 1, 27, 10, 01, 03),
				-480,
				(TiltCode)0x900,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				0,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				0,
				_T("20-6F-EC-00-08-09"),
				_T("20-6F-EC-00-08-08"),
				125,
				75,
				225,
				175,
				-3000 * 100,
				LONG_MIN,
				_T("Kyocera"),
				_T("Action"),
				_T("602"),
				true,
				false,
                CURRENT_MULTIPLIER,
                CURRENT_DIVIDER,
                2,
				1,
                0,
                false,
				EGMTransferMode_AFT,
				"SAS_AFT_BONUS",
                2,
                3,
                false,
				downloadStatus);

			CEGMConfig egmConfig(MEMORY_NONE);

			// Perform operation(s) and test(s).
			egmConfig.SetEGMProtocol(IGT_SAS_3xx);
			ASSERT_TRUE(
				CalcEgmConfigHtml(statusInfo, egmConfig) == CWebDiagnostics::GetEgmConfigHtml(statusInfo, egmConfig)) <<
				L"IGT_SAS_3xx";

			egmConfig.SetEGMProtocol(SAS5xx);
			ASSERT_TRUE(
				CalcEgmConfigHtml(statusInfo, egmConfig) == CWebDiagnostics::GetEgmConfigHtml(statusInfo, egmConfig)) <<
				L"SAS5xx";

			egmConfig.SetEGMProtocol(SAS6xx);
			ASSERT_TRUE(
				CalcEgmConfigHtml(statusInfo, egmConfig) == CWebDiagnostics::GetEgmConfigHtml(statusInfo, egmConfig)) <<
				L"SAS6xx";
		}

		/// <summary>
		/// Functional test for:
		///     CWebDiagnostics::GetEgmConfigHtml()
		/// Test a range of possible values.
		/// </summary>
		TEST_F(WebDiagnostics, GetEgmConfigHtml_EgmProtocol_RangeTest)
		{
			// Setup.
			const CFlashStatus downloadStatus(FlashType::FLASH_NONE, 0, "", "", "", "");
			const CWebDiagnosticsStatusInfo statusInfo(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12),
				1000000 + (1 * SECONDS_PER_MINUTE + 2) * TICK_COUNTS_PER_SECOND,
				true,
				1000000,
				true,
				CUtilities::GetTimeFromFields(2014, 1, 27, 10, 01, 03),
				-480,
				(TiltCode)0x900,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				0,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				0,
				_T("20-6F-EC-00-08-09"),
				_T("20-6F-EC-00-08-08"),
				125,
				75,
				225,
				175,
				-3000 * 100,
				LONG_MIN,
				_T("Kyocera"),
				_T("Action"),
				_T("602"),
				true,
				false,
                CURRENT_MULTIPLIER,
                CURRENT_DIVIDER,
                2,
				1,
                0,
                false,
				EGMTransferMode_EFT,
				"SAS_AFT_BONUS",
                2,
                3,
                false,
				downloadStatus);

			CEGMConfig egmConfig(MEMORY_NONE);

			// Perform operation(s) and test(s).
			for (int testIndex(-1); testIndex < SAS6xx + 1; ++testIndex)
			{
				EGMProtocol egmProtocol((EGMProtocol)testIndex);
				egmConfig.SetEGMProtocol(IGT_SAS_3xx);
				ASSERT_TRUE(
					CalcEgmConfigHtml(statusInfo, egmConfig) == CWebDiagnostics::GetEgmConfigHtml(statusInfo, egmConfig)) <<
					L"GetEgmProtocolString";
			}
		}
// #pragma endregion GetEgmConfigHtml_EgmProtocol

// #pragma region GetEgmConfigHtml_ReportedEgmProtocolVersion
		/// <summary>
		/// Functional test for:
		///     CWebDiagnostics::GetEgmConfigHtml()
		/// Test with CEGMConfig::m_reportedEgmProtocolVersion set to "".
		/// </summary>
		TEST_F(WebDiagnostics, GetEgmConfigHtml_ReportedEgmProtocolVersion_Empty)
		{
			// Setup.
			const CFlashStatus downloadStatus(FlashType::FLASH_NONE, 0, "", "", "", "");
			const CWebDiagnosticsStatusInfo statusInfo(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12),
				1000000 + (1 * SECONDS_PER_MINUTE + 2) * TICK_COUNTS_PER_SECOND,
				true,
				1000000,
				true,
				CUtilities::GetTimeFromFields(2014, 1, 27, 10, 01, 03),
				-480,
				(TiltCode)0x900,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				0,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				0,
				_T("20-6F-EC-00-08-09"),
				_T("20-6F-EC-00-08-08"),
				125,
				75,
				225,
				175,
				-3000 * 100,
				LONG_MIN,
				_T("Kyocera"),
				_T("Action"),
				_T(""),
				true,
				false,
                CURRENT_MULTIPLIER,
                CURRENT_DIVIDER,
                2,
				1,
                0,
                false,
				EGMTransferMode_EFT,
				"SAS_AFT_BONUS",
                2,
                3,
				false,
				downloadStatus);

			CEGMConfig egmConfig(MEMORY_NONE);

			// Perform operation(s) and test(s).
			ASSERT_TRUE(
				CalcEgmConfigHtml(statusInfo, egmConfig) == CWebDiagnostics::GetEgmConfigHtml(statusInfo, egmConfig)) <<
				L"m_reportedEgmProtocolVersion";
		}

		/// <summary>
		/// Functional test for:
		///     CWebDiagnostics::GetEgmConfigHtml()
		/// Test with CEGMConfig::m_reportedEgmProtocolVersion set to a value shorter than normal.
		/// </summary>
		TEST_F(WebDiagnostics, GetEgmConfigHtml_ReportedEgmProtocolVersion_Short)
		{
			// Setup.
			const CFlashStatus downloadStatus(FlashType::FLASH_NONE, 0, "", "", "", "");
			const CWebDiagnosticsStatusInfo statusInfo(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12),
				1000000 + (1 * SECONDS_PER_MINUTE + 2) * TICK_COUNTS_PER_SECOND,
				true,
				1000000,
				true,
				CUtilities::GetTimeFromFields(2014, 1, 27, 10, 01, 03),
				-480,
				(TiltCode)0x900,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				0,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				0,
				_T("20-6F-EC-00-08-09"),
				_T("20-6F-EC-00-08-08"),
				125,
				75,
				225,
				175,
				-3000 * 100,
				LONG_MIN,
				_T("Kyocera"),
				_T("Action"),
				_T("4"),
				true,
				false,
                CURRENT_MULTIPLIER,
                CURRENT_DIVIDER,
                2,
				1,
                0,
                false,
				EGMTransferMode_EFT,
				"SAS_AFT_BONUS",
                2,
                3,
				false,
				downloadStatus);

			CEGMConfig egmConfig(MEMORY_NONE);

			// Perform operation(s) and test(s).
			ASSERT_TRUE(
				CalcEgmConfigHtml(statusInfo, egmConfig) == CWebDiagnostics::GetEgmConfigHtml(statusInfo, egmConfig)) <<
				L"m_reportedEgmProtocolVersion";
		}

		/// <summary>
		/// Functional test for:
		///     CWebDiagnostics::GetEgmConfigHtml()
		/// Test with CEGMConfig::m_reportedEgmProtocolVersion set to a normal 3-digit value.
		/// </summary>
		TEST_F(WebDiagnostics, GetEgmConfigHtml_ReportedEgmProtocolVersion_Normal)
		{
			// Setup.
			const CFlashStatus downloadStatus(FlashType::FLASH_NONE, 0, "", "", "", "");
			const CWebDiagnosticsStatusInfo statusInfo(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12),
				1000000 + (1 * SECONDS_PER_MINUTE + 2) * TICK_COUNTS_PER_SECOND,
				true,
				1000000,
				true,
				CUtilities::GetTimeFromFields(2014, 1, 27, 10, 01, 03),
				-480,
				(TiltCode)0x900,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				0,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				0,
				_T("20-6F-EC-00-08-09"),
				_T("20-6F-EC-00-08-08"),
				125,
				75,
				225,
				175,
				-3000 * 100,
				LONG_MIN,
				_T("Kyocera"),
				_T("Action"),
				_T("510"),
				true,
				false,
                CURRENT_MULTIPLIER,
                CURRENT_DIVIDER,
                2,
				1,
                0,
                false,
				EGMTransferMode_NONE,
				"SAS_AFT_BONUS",
                2,
                3,
				false,
				downloadStatus);

			CEGMConfig egmConfig(MEMORY_NONE);

			// Perform operation(s) and test(s).
			ASSERT_TRUE(
				CalcEgmConfigHtml(statusInfo, egmConfig) == CWebDiagnostics::GetEgmConfigHtml(statusInfo, egmConfig)) <<
				L"m_reportedEgmProtocolVersion";
		}

		/// <summary>
		/// Functional test for:
		///     CWebDiagnostics::GetEgmConfigHtml()
		/// Test with CEGMConfig::m_reportedEgmProtocolVersion set to a value longer than normal.
		/// </summary>
		TEST_F(WebDiagnostics, GetEgmConfigHtml_ReportedEgmProtocolVersion_Long)
		{
			// Setup.
			const CFlashStatus downloadStatus(FlashType::FLASH_NONE, 0, "", "", "", "");
			const CWebDiagnosticsStatusInfo statusInfo(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12),
				1000000 + (1 * SECONDS_PER_MINUTE + 2) * TICK_COUNTS_PER_SECOND,
				true,
				1000000,
				true,
				CUtilities::GetTimeFromFields(2014, 1, 27, 10, 01, 03),
				-480,
				(TiltCode)0x900,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				0,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				0,
				_T("20-6F-EC-00-08-09"),
				_T("20-6F-EC-00-08-08"),
				125,
				75,
				225,
				175,
				-3000 * 100,
				LONG_MIN,
				_T("Kyocera"),
				_T("Action"),
				_T("12345"),
				true,
				false,
                CURRENT_MULTIPLIER,
                CURRENT_DIVIDER,
                2,
				1,
                0,
                false,
				EGMTransferMode_NONE,
				"SAS_AFT_BONUS",
                2,
                3,
				false,
				downloadStatus);

			CEGMConfig egmConfig(MEMORY_NONE);

			// Perform operation(s) and test(s).
			ASSERT_TRUE(
				CalcEgmConfigHtml(statusInfo, egmConfig) == CWebDiagnostics::GetEgmConfigHtml(statusInfo, egmConfig)) <<
				L"m_reportedEgmProtocolVersion";
		}

		/// <summary>
		/// Functional test for:
		///     CWebDiagnostics::GetEgmConfigHtml()
		/// Test with CEGMConfig::m_reportedEgmProtocolVersion set to an alphabetic value.
		/// </summary>
		TEST_F(WebDiagnostics, GetEgmConfigHtml_ReportedEgmProtocolVersion_Alpha)
		{
			// Setup.
			const CFlashStatus downloadStatus(FlashType::FLASH_NONE, 0, "", "", "", "");
			const CWebDiagnosticsStatusInfo statusInfo(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12),
				1000000 + (1 * SECONDS_PER_MINUTE + 2) * TICK_COUNTS_PER_SECOND,
				true,
				1000000,
				true,
				CUtilities::GetTimeFromFields(2014, 1, 27, 10, 01, 03),
				-480,
				(TiltCode)0x900,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				0,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				0,
				_T("20-6F-EC-00-08-09"),
				_T("20-6F-EC-00-08-08"),
				125,
				75,
				225,
				175,
				-3000 * 100,
				LONG_MIN,
				_T("Kyocera"),
				_T("Action"),
				_T("Abc"),
				true,
				false,
                CURRENT_MULTIPLIER,
                CURRENT_DIVIDER,
                2,
				1,
                0,
                false,
				EGMTransferMode_NONE,
				"SAS_AFT_BONUS",
                2,
                3,
				false,
				downloadStatus);

			CEGMConfig egmConfig(MEMORY_NONE);

			// Perform operation(s) and test(s).
			ASSERT_TRUE(
				CalcEgmConfigHtml(statusInfo, egmConfig) == CWebDiagnostics::GetEgmConfigHtml(statusInfo, egmConfig)) <<
				L"m_reportedEgmProtocolVersion";
		}
// #pragma endregion GetEgmConfigHtml_ReportedEgmProtocolVersion

// #pragma region GetEgmConfigHtml_EgmSupportsLP7B
		/// <summary>
		/// Functional test for:
		///     CWebDiagnostics::GetEgmConfigHtml()
		/// Test with CEGMConfig::m_egmSupportsLP7B_Available = false
		/// and with CEGMConfig::m_egmSupportsLP7B = false.
		/// </summary>
		TEST_F(WebDiagnostics, GetEgmConfigHtml_EgmSupportsLP7B_FalseFalse)
		{
			// Setup.
			const CFlashStatus downloadStatus(FlashType::FLASH_NONE, 0, "", "", "", "");
			const CWebDiagnosticsStatusInfo statusInfo(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12),
				1000000 + (1 * SECONDS_PER_MINUTE + 2) * TICK_COUNTS_PER_SECOND,
				true,
				1000000,
				true,
				CUtilities::GetTimeFromFields(2014, 1, 27, 10, 01, 03),
				-480,
				(TiltCode)0x900,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				0,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				0,
				_T("20-6F-EC-00-08-09"),
				_T("20-6F-EC-00-08-08"),
				125,
				75,
				225,
				175,
				-3000 * 100,
				LONG_MIN,
				_T("Kyocera"),
				_T("Action"),
				_T("602"),
				false,
				false,
                CURRENT_MULTIPLIER,
                CURRENT_DIVIDER,
                2,
				1,
                0,
                false,
				EGMTransferMode_PENDING,
				"SAS_AFT_BONUS",
                2,
                3,
				false,
				downloadStatus);

			CEGMConfig egmConfig(MEMORY_NONE);

			// Perform operation(s) and test(s).
			ASSERT_TRUE(
				CalcEgmConfigHtml(statusInfo, egmConfig) == CWebDiagnostics::GetEgmConfigHtml(statusInfo, egmConfig)) <<
				L"m_egmSupportsLP7B";
		}

		/// <summary>
		/// Functional test for:
		///     CWebDiagnostics::GetEgmConfigHtml()
		/// Test with CEGMConfig::m_egmSupportsLP7B_Available = false
		/// and with CEGMConfig::m_egmSupportsLP7B = true.
		/// </summary>
		TEST_F(WebDiagnostics, GetEgmConfigHtml_EgmSupportsLP7B_FalseTrue)
		{
			// Setup.
			const CFlashStatus downloadStatus(FlashType::FLASH_NONE, 0, "", "", "", "");
			const CWebDiagnosticsStatusInfo statusInfo(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12),
				1000000 + (1 * SECONDS_PER_MINUTE + 2) * TICK_COUNTS_PER_SECOND,
				true,
				1000000,
				true,
				CUtilities::GetTimeFromFields(2014, 1, 27, 10, 01, 03),
				-480,
				(TiltCode)0x900,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				0,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				0,
				_T("20-6F-EC-00-08-09"),
				_T("20-6F-EC-00-08-08"),
				125,
				75,
				225,
				175,
				-3000 * 100,
				LONG_MIN,
				_T("Kyocera"),
				_T("Action"),
				_T("602"),
				false,
				true,
                CURRENT_MULTIPLIER,
                CURRENT_DIVIDER,
                2,
				1,
                0,
                false,
				EGMTransferMode_PENDING,
				"SAS_AFT_BONUS",
                2,
                3,
				false,
				downloadStatus);

			CEGMConfig egmConfig(MEMORY_NONE);

			// Perform operation(s) and test(s).
			ASSERT_TRUE(
				CalcEgmConfigHtml(statusInfo, egmConfig) == CWebDiagnostics::GetEgmConfigHtml(statusInfo, egmConfig)) <<
				L"m_egmSupportsLP7B";
		}

		/// <summary>
		/// Functional test for:
		///     CWebDiagnostics::GetEgmConfigHtml()
		/// Test with CEGMConfig::m_egmSupportsLP7B_Available = true
		/// and with CEGMConfig::m_egmSupportsLP7B = false.
		/// </summary>
		TEST_F(WebDiagnostics, GetEgmConfigHtml_EgmSupportsLP7B_TrueFalse)
		{
			// Setup.
			const CFlashStatus downloadStatus(FlashType::FLASH_NONE, 0, "", "", "", "");
			const CWebDiagnosticsStatusInfo statusInfo(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12),
				1000000 + (1 * SECONDS_PER_MINUTE + 2) * TICK_COUNTS_PER_SECOND,
				true,
				1000000,
				true,
				CUtilities::GetTimeFromFields(2014, 1, 27, 10, 01, 03),
				-480,
				(TiltCode)0x900,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				0,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				0,
				_T("20-6F-EC-00-08-09"),
				_T("20-6F-EC-00-08-08"),
				125,
				75,
				225,
				175,
				-3000 * 100,
				LONG_MIN,
				_T("Kyocera"),
				_T("Action"),
				_T("602"),
				true,
				false,
                CURRENT_MULTIPLIER,
                CURRENT_DIVIDER,
                2,
				1,
                0,
                false,
				EGMTransferMode_PENDING,
				"SAS_AFT_BONUS",
                2,
                3,
				false,
				downloadStatus);

			CEGMConfig egmConfig(MEMORY_NONE);

			// Perform operation(s) and test(s).
			ASSERT_TRUE(
				CalcEgmConfigHtml(statusInfo, egmConfig) == CWebDiagnostics::GetEgmConfigHtml(statusInfo, egmConfig)) <<
				L"m_egmSupportsLP7B";
		}

		/// <summary>
		/// Functional test for:
		///     CWebDiagnostics::GetEgmConfigHtml()
		/// Test with CEGMConfig::m_egmSupportsLP7B_Available = true
		/// and with CEGMConfig::m_egmSupportsLP7B = true.
		/// </summary>
		TEST_F(WebDiagnostics, GetEgmConfigHtml_EgmSupportsLP7B_TrueTrue)
		{
			// Setup.
			const CFlashStatus downloadStatus(FlashType::FLASH_NONE, 0, "", "", "", "");
			const CWebDiagnosticsStatusInfo statusInfo(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12),
				1000000 + (1 * SECONDS_PER_MINUTE + 2) * TICK_COUNTS_PER_SECOND,
				true,
				1000000,
				true,
				CUtilities::GetTimeFromFields(2014, 1, 27, 10, 01, 03),
				-480,
				(TiltCode)0x900,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				0,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				0,
				_T("20-6F-EC-00-08-09"),
				_T("20-6F-EC-00-08-08"),
				125,
				75,
				225,
				175,
				-3000 * 100,
				LONG_MIN,
				_T("Kyocera"),
				_T("Action"),
				_T("602"),
				true,
				true,
                CURRENT_MULTIPLIER,
                CURRENT_DIVIDER,
                2,
				1,
                0,
                false,
				EGMTransferMode_PENDING,
				"SAS_AFT_BONUS",
                2,
                3,
				false,
				downloadStatus);

			CEGMConfig egmConfig(MEMORY_NONE);

			// Perform operation(s) and test(s).
			ASSERT_TRUE(
				CalcEgmConfigHtml(statusInfo, egmConfig) == CWebDiagnostics::GetEgmConfigHtml(statusInfo, egmConfig)) <<
				L"m_egmSupportsLP7B";
		}
// #pragma endregion GetEgmConfigHtml_EgmSupportsLP7B
		
		/// <summary>
		/// Functional test for:
		///     CWebDiagnostics::GetEgmConfigHtml_CashoutToHostEnabled()
		/// Test with cashout to host feature enabled configuration.
		/// </summary>
		TEST_F(WebDiagnostics, GetEgmConfigHtml_CashoutToHostEnabled)
		{
			// Setup.
			const CFlashStatus downloadStatus(FlashType::FLASH_NONE, 0, "", "", "", "");
			const CWebDiagnosticsStatusInfo statusInfo(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12),
				1000000 + (1 * SECONDS_PER_MINUTE + 2) * TICK_COUNTS_PER_SECOND,
				true,
				1000000,
				true,
				CUtilities::GetTimeFromFields(2014, 1, 27, 10, 01, 03),
				-480,
				(TiltCode)0x900,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				0,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				0,
				_T("20-6F-EC-00-08-09"),
				_T("20-6F-EC-00-08-08"),
				125,
				75,
				225,
				175,
				-3000 * 100,
				LONG_MIN,
				_T("Kyocera"),
				_T("Action"),
				_T("510"),
				true,
				false,
                CURRENT_MULTIPLIER,
                CURRENT_DIVIDER,
                2,
				1,
                0,
                false,
				EGMTransferMode_NONE,
				"SAS_AFT_BONUS",
                2,
                3,
                false,
				downloadStatus,
				true,
				0x3,
				3,
				1);

			CEGMConfig egmConfig(MEMORY_NONE);

			// Perform operation(s) and test(s).
			ASSERT_TRUE(
				CalcEgmConfigHtml(statusInfo, egmConfig) == CWebDiagnostics::GetEgmConfigHtml(statusInfo, egmConfig)) <<
				L"CashoutToHostEnabled";
		}

		/// <summary>
		/// Code coverage and functional test for:
		/// CWebDiagnostics::SaveStatusData()
		/// </summary>
		TEST_F(WebDiagnostics, SaveStatusData_NoSmiNoErrorsTest)
		{
			// Inputs.
			string dumpFileName(_T("C:\\Inetpub\\wwwroot\\StatusDump.html"));
			const bool errorsFound(false);
			const CPlayerSession playerSession;
			const CEGMConfig egmConfig(MEMORY_NONE);
			const CExcessiveVoucherOut excessiveVoucherOut;
			const CFlashStatus downloadStatus(FlashType::FLASH_NONE, 0, "", "", "", "");

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
				8, "Sentinel is offline", "", "", "", 0.0);

			const CWebDiagnosticsStatusInfo statusInfo(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12),
				1000000 + (1 * SECONDS_PER_MINUTE + 2) * TICK_COUNTS_PER_SECOND,
				true,
				1000000,
				true,
				CUtilities::GetTimeFromFields(2014, 1, 27, 10, 01, 03),
				-480,
				(TiltCode)0x900,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				1,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				1,
				_T("20-6F-EC-00-08-09"),
				_T("20-6F-EC-00-08-08"),
				125,
				75,
				225,
				175,
				3000 * 100,
				7000 * 100,
				_T("Kyocera"),
				_T("Action"),
				_T("602"),
				true,
				false,
                CURRENT_MULTIPLIER,
                CURRENT_DIVIDER,
                2,
				1,
                0,
                false,
				EGMTransferMode_PENDING,
				"SAS_AFT_BONUS",
                2,
                3,
                false,
				downloadStatus);


			// Expected results.
			const string contentsExpected(CalcStatusPageHtml(commonInfo, statusInfo, playerSession, egmConfig, excessiveVoucherOut, errorsFound));

			// Setup.
			shared_ptr<stringstream> file(new stringstream);
			CWebDiagnostics wb;

			// Perform operation(s) to be tested.
			wb.SaveStatusData(file, commonInfo, statusInfo, playerSession, egmConfig, excessiveVoucherOut, errorsFound);

			// Test(s).
			ASSERT_TRUE(CUnitTestingUtils::IsEqual(file, contentsExpected)) << _T("SaveStatusData() IsEqual");
		}

		/// <summary>
		/// Code coverage and functional test for:
		/// CWebDiagnostics::SaveStatusData()
		/// </summary>
		TEST_F(WebDiagnostics, SaveStatusData_NoSmiErrorsTest)
		{
			// Inputs.
			string dumpFileName(_T("C:\\Inetpub\\wwwroot\\StatusDump.html"));
			const bool errorsFound(true);
			const CPlayerSession playerSession;
			const CEGMConfig egmConfig(MEMORY_NONE);
			const CExcessiveVoucherOut excessiveVoucherOut;
			const CFlashStatus downloadStatus(FlashType::FLASH_NONE, 0, "", "", "", "");

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
				8, "Sentinel is offline", "", "", "", 0.0);

			const CWebDiagnosticsStatusInfo statusInfo(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12),
				1000000 + (1 * SECONDS_PER_MINUTE + 2) * TICK_COUNTS_PER_SECOND,
				true,
				1000000,
				true,
				CUtilities::GetTimeFromFields(2014, 1, 27, 10, 01, 03),
				-480,
				(TiltCode)0x900,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				1,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				1,
				_T("20-6F-EC-00-08-09"),
				_T("20-6F-EC-00-08-08"),
				125,
				75,
				225,
				175,
				3000 * 100,
				7000 * 100,
				_T("Kyocera"),
				_T("Action"),
				_T("602"),
				true,
				false,
                CURRENT_MULTIPLIER,
                CURRENT_DIVIDER,
                2,
				1,
                0,
                false,
				EGMTransferMode_AFT,
				"SAS_AFT_BONUS",
                2,
                3,
				false,
				downloadStatus);


			// Expected results.
			const string contentsExpected(CalcStatusPageHtml(commonInfo, statusInfo, playerSession, egmConfig, excessiveVoucherOut, errorsFound));

			// Setup.
			shared_ptr<stringstream> file(new stringstream);
			CWebDiagnostics wb;

			// Perform operation(s) to be tested.
			wb.SaveStatusData(file, commonInfo, statusInfo, playerSession, egmConfig, excessiveVoucherOut, errorsFound);

			// Test(s).
			ASSERT_TRUE(CUnitTestingUtils::IsEqual(file, contentsExpected)) << _T("SaveStatusData() IsEqual");
		}

		/// <summary>
		/// Code coverage and functional test for:
		/// CWebDiagnostics::SaveStatusData()
		/// </summary>
		TEST_F(WebDiagnostics, SaveStatusData_SmiNoErrorsTest)
		{
			// Inputs.
			string dumpFileName(_T("C:\\Inetpub\\wwwroot\\StatusDump.html"));
			const bool errorsFound(false);
			const CPlayerSession playerSession;
			const CEGMConfig egmConfig(MEMORY_NONE);
			const CExcessiveVoucherOut excessiveVoucherOut;
			const CFlashStatus downloadStatus(FlashType::FLASH_NONE, 0, "", "", "", "");

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
				8, "Sentinel is offline", "", "", "", 0.0);

			const CWebDiagnosticsStatusInfo statusInfo(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12),
				1000000 + (1 * SECONDS_PER_MINUTE + 2) * TICK_COUNTS_PER_SECOND,
				true,
				1000000,
				true,
				CUtilities::GetTimeFromFields(2014, 1, 27, 10, 01, 03),
				-480,
				(TiltCode)0x900,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				1,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				1,
				_T("20-6F-EC-00-08-09"),
				_T("20-6F-EC-00-08-08"),
				125,
				75,
				225,
				175,
				3000 * 100,
				7000 * 100,
				_T("Kyocera"),
				_T("Action"),
				_T("602"),
				true,
				false,
                CURRENT_MULTIPLIER,
                CURRENT_DIVIDER,
                2,
				1,
                0,
                false,
				EGMTransferMode_AFT,
				"SAS_AFT_BONUS",
                2,
                3,
				false,
				downloadStatus);


			// Expected results.
			const string contentsExpected(CalcStatusPageHtml(commonInfo, statusInfo, playerSession, egmConfig, excessiveVoucherOut, errorsFound));

			// Setup.
			shared_ptr<stringstream> file(new stringstream);
			CWebDiagnostics wb;

			// Perform operation(s) to be tested.
			wb.SaveStatusData(file, commonInfo, statusInfo, playerSession, egmConfig, excessiveVoucherOut, errorsFound);

			// Test(s).
			ASSERT_TRUE(CUnitTestingUtils::IsEqual(file, contentsExpected)) << _T("SaveStatusData() IsEqual");
		}

		/// <summary>
		/// Code coverage and functional test for:
		/// CWebDiagnostics::SaveStatusData()
		/// </summary>
		TEST_F(WebDiagnostics, SaveStatusData_SmiErrorsTest)
		{
			// Inputs.
			string dumpFileName(_T("C:\\Inetpub\\wwwroot\\StatusDump.html"));
			const bool errorsFound(true);
			const CPlayerSession playerSession;
			const CEGMConfig egmConfig(MEMORY_NONE);
			const CExcessiveVoucherOut excessiveVoucherOut;
			const CFlashStatus downloadStatus(FlashType::FLASH_NONE, 0, "", "", "", "");

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
				8, "Sentinel is offline", "", "", "", 0.0);

			const CWebDiagnosticsStatusInfo statusInfo(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12),
				1000000 + (1 * SECONDS_PER_MINUTE + 2) * TICK_COUNTS_PER_SECOND,
				true,
				1000000,
				true,
				CUtilities::GetTimeFromFields(2014, 1, 27, 10, 01, 03),
				-480,
				(TiltCode)0x900,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				1,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				1,
				_T("20-6F-EC-00-08-09"),
				_T("20-6F-EC-00-08-08"),
				125,
				75,
				225,
				175,
				3000 * 100,
				7000 * 100,
				_T("Kyocera"),
				_T("Action"),
				_T("602"),
				true,
				false,
                CURRENT_MULTIPLIER,
                CURRENT_DIVIDER,
                2,
				1,
                0,
                false,
				EGMTransferMode_AFT,
				"SAS_AFT_BONUS",
                2,
                3,
				false,
				downloadStatus);


			// Expected results.
			const string contentsExpected(CalcStatusPageHtml(commonInfo, statusInfo, playerSession, egmConfig, excessiveVoucherOut, errorsFound));

			// Setup.
			shared_ptr<stringstream> file(new stringstream);
			CWebDiagnostics wb;

			// Perform operation(s) to be tested.
			wb.SaveStatusData(file, commonInfo, statusInfo, playerSession, egmConfig, excessiveVoucherOut, errorsFound);

			// Test(s).
			ASSERT_TRUE(CUnitTestingUtils::IsEqual(file, contentsExpected)) << _T("SaveStatusData() IsEqual");
		}

		/// <summary>
		/// Code coverage and functional test for:
		/// CWebDiagnostics::GetProcessHtml()
		/// </summary>
		TEST_F(WebDiagnostics, GetProcessHtml1Test)
		{
			// Inputs.
			CProcess varProcess;
			varProcess.m_processName = _T("ContentWindow.exe");
			varProcess.m_cpuUsage = 22;
			varProcess.m_moduleSize = 23456;
			varProcess.m_vmSize = 34567;
			varProcess.m_totalProcTime = (((11 * 60 + 2) * 60 + 3) * 1000 + 4) * (ULONGLONG)10000;
			varProcess.m_handleCount = 456;
			const CProcess &process(varProcess);

			// Expected results.
			const string htmlExpected(CalcProcessHtml(process));

			// Setup.
			CWebDiagnostics wb;

			// Perform operation(s) to be tested.
			string htmlResult(wb.GetProcessHtml(process));

			// Test(s).
			ASSERT_TRUE(htmlResult == htmlExpected) << _T("GetProcessHtml() hmtl");
		}

		/// <summary>
		/// Code coverage and functional test for:
		/// CWebDiagnostics::GetProcessHtml()
		/// </summary>
		TEST_F(WebDiagnostics, GetProcessHtml2Test)
		{
			// Inputs.
			CProcess varProcess;
			varProcess.m_processName = _T("Sentinel.exe");
			varProcess.m_cpuUsage = 3;
			varProcess.m_moduleSize = 116792;
			varProcess.m_vmSize = 6300;
			varProcess.m_totalProcTime = (((0 * 60 + 3) * 60 + 45) * 1000 + 678) * (ULONGLONG)10000;
			varProcess.m_handleCount = 1678;
			const CProcess &process(varProcess);

			// Expected results.
			const string htmlExpected(CalcProcessHtml(process));

			// Setup.
			CWebDiagnostics wb;

			// Perform operation(s) to be tested.
			string htmlResult(wb.GetProcessHtml(process));

			// Test(s).
			ASSERT_TRUE(htmlResult == htmlExpected) << _T("GetProcessHtml() hmtl");
		}

		/// <summary>
		/// Code coverage and functional test for:
		/// CWebDiagnostics::SaveMemoryData()
		/// </summary>
		TEST_F(WebDiagnostics, SaveMemoryData_ErrorsTest)
		{
			// Inputs.
			string dumpFileName(_T("C:\\Inetpub\\wwwroot\\MemoryDump.html"));
			const bool errorsFound(true);

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
				8, "Sentinel is offline", "", "", "", 0.0);

			CProcess process1;
			process1.m_processId = 111;
			process1.m_processName = _T("ContentWindow.exe");
			process1.m_cpuUsage = 22;
			process1.m_moduleSize = 23456;
			process1.m_vmSize = 34567;
			process1.m_totalProcTime = (((11 * 60 + 2) * 60 + 3) * 1000 + 4) * (ULONGLONG)10000;
			process1.m_currentActive = true;
			process1.m_handleCount = 456;

			CProcess process2;
			process2.m_processId = 222;
			process2.m_processName = _T("Sentinel.exe");
			process2.m_cpuUsage = 3;
			process2.m_moduleSize = 116792;
			process2.m_vmSize = 6300;
			process2.m_totalProcTime = (((0 * 60 + 3) * 60 + 45) * 1000 + 678) * (ULONGLONG)10000;
			process2.m_currentActive = true;
			process2.m_handleCount = 1678;

			CProcessHandler varProcessHandler;
			varProcessHandler.AddProcess(process1);
			varProcessHandler.AddProcess(process2);
			const CProcessHandler &processHandler(varProcessHandler);

			// Expected results.
			// Processes can be reported in any order, so check file for both valid representations.
			const string contentsExpected1(CalcProcessPageHtml(commonInfo, process1, process2, errorsFound));
			const string contentsExpected2(CalcProcessPageHtml(commonInfo, process2, process1, errorsFound));

			// Setup.
			shared_ptr<stringstream> file(new stringstream);
			CWebDiagnostics wb;

			// Perform operation(s) to be tested.
			wb.SaveMemoryData(file, commonInfo, processHandler, errorsFound);

			// Test(s).

			// Processes can be reported in any order, so check file for both valid representations.
			bool fileIsEqual1 = CUnitTestingUtils::IsEqual(file, contentsExpected1);
			bool fileIsEqual2 = CUnitTestingUtils::IsEqual(file, contentsExpected2);
			ASSERT_TRUE(fileIsEqual1 || fileIsEqual2) << _T("SaveMemoryData() IsEqual");
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CWebDiagnostics::LoadErrorFile()
		/// Test with a file size of zero and a target buffer of zero length.
		/// </summary>
		TEST_F(WebDiagnostics, LoadErrorFile00Test)
		{
			// Inputs.
			const char *errorFileContents(nullptr);
			const UINT errorFileContentsLength(0);
			const std::shared_ptr<std::istream> errorFile(new stringstream(string(errorFileContents, errorFileContentsLength)));

			const UINT bufferSize(0);

			// Expected results.
			const string contentsExpected(CalcErrorFileHtml(errorFile, bufferSize));

			// Setup.
			BYTE bufferResult[bufferSize + 1];
			CWebDiagnostics wb;

			// Perform operation(s) to be tested.
			UINT errorFileSizeUsed(12345);
			ULONGLONG errorFileSize(1234567);
			UINT bufferLengthResult = wb.LoadErrorFile(errorFile, errorFileSizeUsed, errorFileSize, bufferResult, bufferSize);

			// Test(s).
			errorFile->clear(); // Clear any errors/eof, so that seeks and reads will work again.
			ASSERT_EQ((UINT)CUnitTestingUtils::GetLength(errorFile), errorFileSizeUsed) << _T("LoadErrorFile() errorFileSizeUsed");
			ASSERT_EQ(CUnitTestingUtils::GetLength(errorFile), errorFileSize) << _T("LoadErrorFile() errorFileSize");
			ASSERT_EQ((UINT)contentsExpected.length(), bufferLengthResult) << _T("LoadErrorFile() bufferLengthResult");
			ASSERT_TRUE(0 == memcmp(bufferResult, contentsExpected.c_str(), contentsExpected.length())) << _T("LoadErrorFile() memcmp");

			// Verify that error file is unchanged.
			ASSERT_TRUE(CUnitTestingUtils::IsEqual(errorFile, errorFileContents, errorFileContentsLength)) << _T("LoadErrorFile() IsEqual");
		}

		/// <summary>
		/// Code coverage and functional test for:
		/// CWebDiagnostics::LoadErrorFile()
		/// </summary>
		TEST_F(WebDiagnostics, LoadErrorFile0Test)
		{
			// Inputs.
			const char *errorFileContents(nullptr);
			const UINT errorFileContentsLength(0);
			shared_ptr<stringstream> file(new stringstream);

			const std::shared_ptr<std::istream> errorFile(new stringstream(string(errorFileContents, errorFileContentsLength)));

			const UINT bufferSize(1024);

			// Expected results.
			const string contentsExpected(CalcErrorFileHtml(errorFile, bufferSize));

			// Expected results.
			const UINT bufferLengthExpected(0);
			const BYTE bufferExpected[bufferLengthExpected + 1] = {};

			// Setup.
			BYTE bufferResult[bufferSize];
			CWebDiagnostics wb;

			// Perform operation(s) to be tested.
			UINT errorFileSizeUsed(12345);
			ULONGLONG errorFileSize(1234567);
			UINT bufferLengthResult = wb.LoadErrorFile(errorFile, errorFileSizeUsed, errorFileSize, bufferResult, bufferSize);

			// Test(s).
			errorFile->clear(); // Clear any errors/eof, so that seeks and reads will work again.
			ASSERT_EQ((UINT)CUnitTestingUtils::GetLength(errorFile), errorFileSizeUsed) << _T("LoadErrorFile() errorFileSizeUsed");
			ASSERT_EQ(CUnitTestingUtils::GetLength(errorFile), errorFileSize) << _T("LoadErrorFile() errorFileSize");
			ASSERT_EQ((UINT)contentsExpected.length(), bufferLengthResult) << _T("LoadErrorFile() bufferLengthResult");
			ASSERT_TRUE(0 == memcmp(bufferResult, static_cast<LPCSTR>(contentsExpected.c_str()), contentsExpected.length())) << _T("LoadErrorFile() memcmp");

			// Verify that error file is unchanged.
			ASSERT_TRUE(CUnitTestingUtils::IsEqual(errorFile, errorFileContents, errorFileContentsLength)) << _T("LoadErrorFile() IsEqual");
		}

		/// <summary>
		/// Code coverage and functional test for:
		/// CWebDiagnostics::SaveStatusData()
		/// </summary>
		TEST_F(WebDiagnostics, SaveErrorData0Test)
		{
			// Inputs.
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
				8, "Sentinel is offline", "", "", "", 0.0);

			const char *errorFileContents(nullptr);
			const UINT errorFileContentsLength(0);
			const std::shared_ptr<std::istream> errorFile(new stringstream(string(errorFileContents, errorFileContentsLength)));

			const UINT bufferSize(1024);

			// Expected results.
			const string contentsExpected(CalcErrorPageHtml(commonInfo, errorFile, bufferSize));

			// Setup.
			shared_ptr<stringstream> file(new stringstream);
			CWebDiagnostics wb;

			// Perform operation(s) to be tested.
			UINT errorFileSizeUsed(12345);
			ULONGLONG errorFileSize(1234567);
			BYTE bufferResult[bufferSize];
			UINT bufferLengthResult = wb.LoadErrorFile(errorFile, errorFileSizeUsed, errorFileSize, bufferResult, bufferSize);

			wb.SaveErrorData(file, commonInfo, errorFileSizeUsed, errorFileSize, bufferResult, bufferLengthResult);

			// Test(s).
			ASSERT_TRUE(CUnitTestingUtils::IsEqual(file, contentsExpected)) << _T("SaveErrorData() IsEqual");
		}

		/// <summary>
		/// Code coverage and functional test for:
		/// CWebDiagnostics::LoadErrorFile()
		/// </summary>
		TEST_F(WebDiagnostics, LoadErrorFile1Test)
		{
			// Inputs.
			const std::shared_ptr<std::istream> errorFile(new stringstream(string(m_errorFileContents3, m_errorFileContentsLength3)));
			const UINT bufferSize(1024);

			// Expected results.
			const string contentsExpected(CalcErrorFileHtml(errorFile, bufferSize));

			// Setup.
			CWebDiagnostics wb;

			// Perform operation(s) to be tested.
			UINT errorFileSizeUsed(12345);
			ULONGLONG errorFileSize(1234567);
			BYTE bufferResult[bufferSize];
			UINT bufferLengthResult = wb.LoadErrorFile(errorFile, errorFileSizeUsed, errorFileSize, bufferResult, bufferSize);

			// Test(s).
			errorFile->clear(); // Clear any errors/eof, so that seeks and reads will work again.
			ASSERT_EQ((UINT)CUnitTestingUtils::GetLength(errorFile), errorFileSizeUsed) << _T("LoadErrorFile() errorFileSizeUsed");
			ASSERT_EQ(CUnitTestingUtils::GetLength(errorFile), errorFileSize) << _T("LoadErrorFile() errorFileSize");

			ASSERT_EQ((UINT)contentsExpected.length(), bufferLengthResult) << _T("LoadErrorFile() bufferLengthResult");
			ASSERT_TRUE(0 == memcmp(bufferResult, static_cast<LPCSTR>(contentsExpected.c_str()), contentsExpected.length())) << _T("LoadErrorFile() memcmp");

			// Verify that file is unchanged.
			ASSERT_TRUE(CUnitTestingUtils::IsEqual(errorFile, m_errorFileContents3, m_errorFileContentsLength3)) << _T("LoadErrorFile() IsEqual");
		}

		/// <summary>
		/// Code coverage and functional test for:
		/// CWebDiagnostics::SaveStatusData()
		/// </summary>
		TEST_F(WebDiagnostics, SaveErrorData1Test)
		{
			// Inputs.

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
				8, "Sentinel is offline", "", "", "", 0.0);

			const std::shared_ptr<std::istream> errorFile(new stringstream(string(m_errorFileContents3, m_errorFileContentsLength3)));
			const UINT bufferSize(1024);

			// Expected results.
			const string contentsExpected(CalcErrorPageHtml(commonInfo, errorFile, bufferSize));

			// Setup.
			shared_ptr<stringstream> file(new stringstream);
			CWebDiagnostics wb;

			// Perform operation(s) to be tested.
			UINT errorFileSizeUsed(12345);
			ULONGLONG errorFileSize(1234567);
			BYTE bufferResult[bufferSize];
			UINT bufferLengthResult = wb.LoadErrorFile(errorFile, errorFileSizeUsed, errorFileSize, bufferResult, bufferSize);

			wb.SaveErrorData(file, commonInfo, errorFileSizeUsed, errorFileSize, bufferResult, bufferLengthResult);

			// Test(s).
			ASSERT_TRUE(CUnitTestingUtils::IsEqual(file, contentsExpected)) << _T("SaveErrorData() IsEqual");
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CWebDiagnostics::LoadErrorFile()
		/// Test with target buffer size (41) smaller than error file.
		/// </summary>
		TEST_F(WebDiagnostics, LoadErrorFile1aTest)
		{
			// Inputs.
			const std::shared_ptr<std::istream> errorFile(new stringstream(string(m_errorFileContents3, m_errorFileContentsLength3)));
			const UINT bufferSize(1024);

			// Expected results.
			const string contentsExpected(CalcErrorFileHtml(errorFile, bufferSize));

			// Setup.
			CWebDiagnostics wb;

			// Perform operation(s) to be tested.
			UINT errorFileSizeUsed(12345);
			ULONGLONG errorFileSize(1234567);
			BYTE bufferResult[bufferSize];
			UINT bufferLengthResult = wb.LoadErrorFile(errorFile, errorFileSizeUsed, errorFileSize, bufferResult, bufferSize);

			// Test(s).
			errorFile->clear(); // Clear any errors/eof, so that seeks and reads will work again.

			ASSERT_EQ((UINT)54, errorFileSizeUsed) << _T("LoadErrorFile() errorFileSizeUsed");
			ASSERT_EQ(CUnitTestingUtils::GetLength(errorFile), errorFileSize) << _T("LoadErrorFile() errorFileSize");
			ASSERT_EQ((UINT)contentsExpected.length(), bufferLengthResult) << _T("LoadErrorFile() bufferLengthResult");
            ASSERT_TRUE(0 == memcmp(bufferResult, static_cast<LPCSTR>(contentsExpected.c_str()), contentsExpected.length())) << _T("LoadErrorFile() memcmp");

			// Verify that file is unchanged.
			ASSERT_TRUE(CUnitTestingUtils::IsEqual(errorFile, m_errorFileContents3, m_errorFileContentsLength3)) << _T("LoadErrorFile() IsEqual");
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CWebDiagnostics::LoadErrorFile()
		/// </summary>
		TEST_F(WebDiagnostics, LoadErrorFile1bTest)
		{
			// Inputs.
			const std::shared_ptr<std::istream> errorFile(new stringstream(string(m_errorFileContents3, m_errorFileContentsLength3)));
			const UINT bufferSize(1024);

			// Expected results.
			const string contentsExpected(CalcErrorFileHtml(errorFile, bufferSize));

			// Setup.
			CWebDiagnostics wb;

			// Perform operation(s) to be tested.
			UINT errorFileSizeUsed(12345);
			ULONGLONG errorFileSize(1234567);
			BYTE bufferResult[bufferSize];
			UINT bufferLengthResult = wb.LoadErrorFile(errorFile, errorFileSizeUsed, errorFileSize, bufferResult, bufferSize);

			// Test(s).
			errorFile->clear(); // Clear any errors/eof, so that seeks and reads will work again.

			ASSERT_EQ((UINT)54, errorFileSizeUsed) << _T("LoadErrorFile() errorFileSizeUsed");
			ASSERT_EQ(CUnitTestingUtils::GetLength(errorFile), errorFileSize) << _T("LoadErrorFile() errorFileSize");
			ASSERT_EQ((UINT)contentsExpected.length(), bufferLengthResult) << _T("LoadErrorFile() bufferLengthResult");
			ASSERT_TRUE(0 == memcmp(bufferResult, static_cast<LPCSTR>(contentsExpected.c_str()), contentsExpected.length())) << _T("LoadErrorFile() memcmp");

			// Verify that file is unchanged.
			ASSERT_TRUE(CUnitTestingUtils::IsEqual(errorFile, m_errorFileContents3, m_errorFileContentsLength3)) << _T("LoadErrorFile() IsEqual");
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CWebDiagnostics::LoadErrorFile()
		/// </summary>
		TEST_F(WebDiagnostics, LoadErrorFile1cTest)
		{
			// Inputs.
			const std::shared_ptr<std::istream> errorFile(new stringstream(string(m_errorFileContents3, m_errorFileContentsLength3)));
			const UINT bufferSize(1024);

			// Expected results.
			const string contentsExpected(CalcErrorFileHtml(errorFile, bufferSize));

			// Setup.
			CWebDiagnostics wb;

			// Perform operation(s) to be tested.
			UINT errorFileSizeUsed(12345);
			ULONGLONG errorFileSize(1234567);
			BYTE bufferResult[bufferSize];
			UINT bufferLengthResult = wb.LoadErrorFile(errorFile, errorFileSizeUsed, errorFileSize, bufferResult, bufferSize);

			// Test(s).
			errorFile->clear(); // Clear any errors/eof, so that seeks and reads will work again.

			ASSERT_EQ((UINT)54, errorFileSizeUsed) << _T("LoadErrorFile() errorFileSizeUsed");
			ASSERT_EQ(CUnitTestingUtils::GetLength(errorFile), errorFileSize) << _T("LoadErrorFile() errorFileSize");
			ASSERT_EQ((UINT)contentsExpected.length(), bufferLengthResult) << _T("LoadErrorFile() bufferLengthResult");
			ASSERT_TRUE(0 == memcmp(bufferResult, static_cast<LPCSTR>(contentsExpected.c_str()), contentsExpected.length())) << _T("LoadErrorFile() memcmp");

			// Verify that file is unchanged.
			ASSERT_TRUE(CUnitTestingUtils::IsEqual(errorFile, m_errorFileContents3, m_errorFileContentsLength3)) << _T("LoadErrorFile() IsEqual");
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CWebDiagnostics::LoadErrorFile()
		/// </summary>
		TEST_F(WebDiagnostics, LoadErrorFile1dTest)
		{
			// Inputs.
			const std::shared_ptr<std::istream> errorFile(new stringstream(string(m_errorFileContents3, m_errorFileContentsLength3)));
			const UINT bufferSize(1024);

			// Expected results.
			const string contentsExpected(CalcErrorFileHtml(errorFile, bufferSize));

			// Setup.
			CWebDiagnostics wb;

			// Perform operation(s) to be tested.
			UINT errorFileSizeUsed(12345);
			ULONGLONG errorFileSize(1234567);
			BYTE bufferResult[bufferSize];
			UINT bufferLengthResult = wb.LoadErrorFile(errorFile, errorFileSizeUsed, errorFileSize, bufferResult, bufferSize);

			// Test(s).
			errorFile->clear(); // Clear any errors/eof, so that seeks and reads will work again.

			ASSERT_EQ((UINT)54, errorFileSizeUsed) << _T("LoadErrorFile() errorFileSizeUsed");
			ASSERT_EQ(CUnitTestingUtils::GetLength(errorFile), errorFileSize) << _T("LoadErrorFile() errorFileSize");
			ASSERT_EQ((UINT)contentsExpected.length(), bufferLengthResult) << _T("LoadErrorFile() bufferLengthResult");
			ASSERT_TRUE(0 == memcmp(bufferResult, static_cast<LPCSTR>(contentsExpected.c_str()), contentsExpected.length())) << _T("LoadErrorFile() memcmp");

			// Verify that file is unchanged.
			ASSERT_TRUE(CUnitTestingUtils::IsEqual(errorFile, m_errorFileContents3, m_errorFileContentsLength3)) << _T("LoadErrorFile() IsEqual");
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CWebDiagnostics::LoadErrorFile()
		/// </summary>
		TEST_F(WebDiagnostics, LoadErrorFile1eTest)
		{
			// Inputs.
			const std::shared_ptr<std::istream> errorFile(new stringstream(string(m_errorFileContents3, m_errorFileContentsLength3)));
			const UINT bufferSize(1024);

			// Expected results.
			const string contentsExpected(CalcErrorFileHtml(errorFile, bufferSize));

			// Setup.
			CWebDiagnostics wb;

			// Perform operation(s) to be tested.
			UINT errorFileSizeUsed(12345);
			ULONGLONG errorFileSize(1234567);
			BYTE bufferResult[bufferSize];
			UINT bufferLengthResult = wb.LoadErrorFile(errorFile, errorFileSizeUsed, errorFileSize, bufferResult, bufferSize);

			// Test(s).
			errorFile->clear(); // Clear any errors/eof, so that seeks and reads will work again.

			ASSERT_EQ((UINT)54, errorFileSizeUsed) << _T("LoadErrorFile() errorFileSizeUsed");
			ASSERT_EQ(CUnitTestingUtils::GetLength(errorFile), errorFileSize) << _T("LoadErrorFile() errorFileSize");
			ASSERT_EQ((UINT)contentsExpected.length(), bufferLengthResult) << _T("LoadErrorFile() bufferLengthResult");
			ASSERT_TRUE(0 == memcmp(bufferResult, static_cast<LPCSTR>(contentsExpected.c_str()), contentsExpected.length())) << _T("LoadErrorFile() memcmp");

			// Verify that file is unchanged.
			ASSERT_TRUE(CUnitTestingUtils::IsEqual(errorFile, m_errorFileContents3, m_errorFileContentsLength3)) << _T("LoadErrorFile() IsEqual");
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CWebDiagnostics::LoadErrorFile()
		/// </summary>
		TEST_F(WebDiagnostics, LoadErrorFile1fTest)
		{
			// Inputs.
			const std::shared_ptr<std::istream> errorFile(new stringstream(string(m_errorFileContents3, m_errorFileContentsLength3)));
			const UINT bufferSize(1024);

			// Expected results.
			const string contentsExpected(CalcErrorFileHtml(errorFile, bufferSize));

			// Setup.
			CWebDiagnostics wb;

			// Perform operation(s) to be tested.
			UINT errorFileSizeUsed(12345);
			ULONGLONG errorFileSize(1234567);
			BYTE bufferResult[bufferSize];
			UINT bufferLengthResult = wb.LoadErrorFile(errorFile, errorFileSizeUsed, errorFileSize, bufferResult, bufferSize);

			// Test(s).
			errorFile->clear(); // Clear any errors/eof, so that seeks and reads will work again.
			ASSERT_EQ((UINT)54, errorFileSizeUsed) << _T("LoadErrorFile() errorFileSizeUsed");
			ASSERT_EQ(CUnitTestingUtils::GetLength(errorFile), errorFileSize) << _T("LoadErrorFile() errorFileSize");
			ASSERT_EQ((UINT)contentsExpected.length(), bufferLengthResult) << _T("LoadErrorFile() bufferLengthResult");
			ASSERT_TRUE(0 == memcmp(bufferResult, static_cast<LPCSTR>(contentsExpected.c_str()), contentsExpected.length())) << _T("LoadErrorFile() memcmp");

			// Verify that file is unchanged.
			ASSERT_TRUE(CUnitTestingUtils::IsEqual(errorFile, m_errorFileContents3, m_errorFileContentsLength3)) << _T("LoadErrorFile() IsEqual");
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CWebDiagnostics::LoadErrorFile()
		/// </summary>
		TEST_F(WebDiagnostics, LoadErrorFile1gTest)
		{

			// Inputs.
			const std::shared_ptr<std::istream> errorFile(new stringstream(string(m_errorFileContents3, m_errorFileContentsLength3)));
			const UINT bufferSize(1024);

			// Expected results.
			const string contentsExpected(CalcErrorFileHtml(errorFile, bufferSize));

			// Setup.
			CWebDiagnostics wb;

			// Perform operation(s) to be tested.
			UINT errorFileSizeUsed(12345);
			ULONGLONG errorFileSize(1234567);
			BYTE bufferResult[bufferSize];
			UINT bufferLengthResult = wb.LoadErrorFile(errorFile, errorFileSizeUsed, errorFileSize, bufferResult, bufferSize);

			// Test(s).
			errorFile->clear(); // Clear any errors/eof, so that seeks and reads will work again.
			ASSERT_EQ((UINT)CUnitTestingUtils::GetLength(errorFile), errorFileSizeUsed) << _T("LoadErrorFile() errorFileSizeUsed");
			ASSERT_EQ(CUnitTestingUtils::GetLength(errorFile), errorFileSize) << _T("LoadErrorFile() errorFileSize");
			ASSERT_EQ((UINT)contentsExpected.length(), bufferLengthResult) << _T("LoadErrorFile() bufferLengthResult");
			ASSERT_TRUE(0 == memcmp(bufferResult, static_cast<LPCSTR>(contentsExpected.c_str()), contentsExpected.length())) << _T("LoadErrorFile() memcmp");

			// Verify that file is unchanged.
			ASSERT_TRUE(CUnitTestingUtils::IsEqual(errorFile, m_errorFileContents3, m_errorFileContentsLength3)) << _T("LoadErrorFile() IsEqual");
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CWebDiagnostics::LoadErrorFile()
		/// </summary>
		TEST_F(WebDiagnostics, LoadErrorFile1hTest)
		{
			// Inputs.
			const std::shared_ptr<std::istream> errorFile(new stringstream(string(m_errorFileContents3, m_errorFileContentsLength3)));
			const UINT bufferSize(1024);

			// Expected results.
			const string contentsExpected(CalcErrorFileHtml(errorFile, bufferSize));

			// Setup.
			CWebDiagnostics wb;

			// Perform operation(s) to be tested.
			UINT errorFileSizeUsed(12345);
			ULONGLONG errorFileSize(1234567);
			BYTE bufferResult[bufferSize];
			UINT bufferLengthResult = wb.LoadErrorFile(errorFile, errorFileSizeUsed, errorFileSize, bufferResult, bufferSize);

			// Test(s).
			errorFile->clear(); // Clear any errors/eof, so that seeks and reads will work again.
			ASSERT_EQ((UINT)CUnitTestingUtils::GetLength(errorFile), errorFileSizeUsed) << _T("LoadErrorFile() errorFileSizeUsed");
			ASSERT_EQ(CUnitTestingUtils::GetLength(errorFile), errorFileSize) << _T("LoadErrorFile() errorFileSize");
			ASSERT_EQ((UINT)contentsExpected.length(), bufferLengthResult) << _T("LoadErrorFile() bufferLengthResult");
			ASSERT_TRUE(0 == memcmp(bufferResult, static_cast<LPCSTR>(contentsExpected.c_str()), contentsExpected.length())) << _T("LoadErrorFile() memcmp");

			// Verify that file is unchanged.
			ASSERT_TRUE(CUnitTestingUtils::IsEqual(errorFile, m_errorFileContents3, m_errorFileContentsLength3)) << _T("LoadErrorFile() IsEqual");
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CWebDiagnostics::LoadErrorFile()
		/// </summary>
		TEST_F(WebDiagnostics, LoadErrorFile1iTest)
		{
			// Inputs.
			const std::shared_ptr<std::istream> errorFile(new stringstream(string(m_errorFileContents3, m_errorFileContentsLength3)));
			const UINT bufferSize(1024);

			// Expected results.
			const string contentsExpected(CalcErrorFileHtml(errorFile, bufferSize));

			// Setup.
			CWebDiagnostics wb;

			// Perform operation(s) to be tested.
			UINT errorFileSizeUsed(12345);
			ULONGLONG errorFileSize(1234567);
			BYTE bufferResult[bufferSize];
			UINT bufferLengthResult = wb.LoadErrorFile(errorFile, errorFileSizeUsed, errorFileSize, bufferResult, bufferSize);

			// Test(s).
			errorFile->clear(); // Clear any errors/eof, so that seeks and reads will work again.
			ASSERT_EQ((UINT)CUnitTestingUtils::GetLength(errorFile), errorFileSizeUsed) << _T("LoadErrorFile() errorFileSizeUsed");
			ASSERT_EQ(CUnitTestingUtils::GetLength(errorFile), errorFileSize) << _T("LoadErrorFile() errorFileSize");
			ASSERT_EQ((UINT)contentsExpected.length(), bufferLengthResult) << _T("LoadErrorFile() bufferLengthResult");
			ASSERT_TRUE(0 == memcmp(bufferResult, static_cast<LPCSTR>(contentsExpected.c_str()), contentsExpected.length())) << _T("LoadErrorFile() memcmp");

			// Verify that file is unchanged.
			ASSERT_TRUE(CUnitTestingUtils::IsEqual(errorFile, m_errorFileContents3, m_errorFileContentsLength3)) << _T("LoadErrorFile() IsEqual");
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CWebDiagnostics::LoadErrorFile()
		/// </summary>
		TEST_F(WebDiagnostics, LoadErrorFile1jTest)
		{
			// Inputs.
			const std::shared_ptr<std::istream> errorFile(new stringstream(string(m_errorFileContents3, m_errorFileContentsLength3)));
			const UINT bufferSize(1024);

			// Expected results.
			const string contentsExpected(CalcErrorFileHtml(errorFile, bufferSize));

			// Setup.
			CWebDiagnostics wb;

			// Perform operation(s) to be tested.
			UINT errorFileSizeUsed(12345);
			ULONGLONG errorFileSize(1234567);
			BYTE bufferResult[bufferSize];
			UINT bufferLengthResult = wb.LoadErrorFile(errorFile, errorFileSizeUsed, errorFileSize, bufferResult, bufferSize);

			// Test(s).
			errorFile->clear(); // Clear any errors/eof, so that seeks and reads will work again.
			ASSERT_EQ((UINT)CUnitTestingUtils::GetLength(errorFile), errorFileSizeUsed) << _T("LoadErrorFile() errorFileSizeUsed");
			ASSERT_EQ(CUnitTestingUtils::GetLength(errorFile), errorFileSize) << _T("LoadErrorFile() errorFileSize");
			ASSERT_EQ((UINT)contentsExpected.length(), bufferLengthResult) << _T("LoadErrorFile() bufferLengthResult");
			ASSERT_TRUE(0 == memcmp(bufferResult, static_cast<LPCSTR>(contentsExpected.c_str()), contentsExpected.length())) << _T("LoadErrorFile() memcmp");

			// Verify that file is unchanged.
			ASSERT_TRUE(CUnitTestingUtils::IsEqual(errorFile, m_errorFileContents3, m_errorFileContentsLength3)) << _T("LoadErrorFile() IsEqual");
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CWebDiagnostics::LoadErrorFile()
		/// </summary>
		TEST_F(WebDiagnostics, LoadErrorFile1kTest)
		{
			// Inputs.
			const std::shared_ptr<std::istream> errorFile(new stringstream(string(m_errorFileContents3, m_errorFileContentsLength3)));
			const UINT bufferSize(1024);

			// Expected results.
			const string contentsExpected(CalcErrorFileHtml(errorFile, bufferSize));

			// Setup.
			CWebDiagnostics wb;

			// Perform operation(s) to be tested.
			UINT errorFileSizeUsed(12345);
			ULONGLONG errorFileSize(1234567);
			BYTE bufferResult[bufferSize];
			UINT bufferLengthResult = wb.LoadErrorFile(errorFile, errorFileSizeUsed, errorFileSize, bufferResult, bufferSize);

			// Test(s).
			errorFile->clear(); // Clear any errors/eof, so that seeks and reads will work again.
			ASSERT_EQ((UINT)CUnitTestingUtils::GetLength(errorFile), errorFileSizeUsed) << _T("LoadErrorFile() errorFileSizeUsed");
			ASSERT_EQ(CUnitTestingUtils::GetLength(errorFile), errorFileSize) << _T("LoadErrorFile() errorFileSize");
			ASSERT_EQ((UINT)contentsExpected.length(), bufferLengthResult) << _T("LoadErrorFile() bufferLengthResult");
			ASSERT_TRUE(0 == memcmp(bufferResult, static_cast<LPCSTR>(contentsExpected.c_str()), contentsExpected.length())) << _T("LoadErrorFile() memcmp");

			// Verify that file is unchanged.
			ASSERT_TRUE(CUnitTestingUtils::IsEqual(errorFile, m_errorFileContents3, m_errorFileContentsLength3)) << _T("LoadErrorFile() IsEqual");
		}

		/// <summary>
		/// Code coverage and functional test for:
		/// CWebDiagnostics::CWebDiagnostics()
		/// Test with an empty error file.
		/// </summary>
		TEST_F(WebDiagnostics, CWebDiagnostics0Test)
		{
			// Inputs.
			const char *errorFileContents(nullptr);
			const UINT errorFileContentsLength(0);

			// Expected results.
			const string contentsExpected(CalcErrorFileHtml(errorFileContents, errorFileContentsLength));

			// Setup.
			COperatingSystemTest operatingSystem(m_errorFileNameExpected,
				m_errorOpenFlagsExpected,
				errorFileContents,
				errorFileContentsLength);
			CWebDiagnostics wb(operatingSystem);

			// Perform operation(s) to be tested.
			wb.Start();

			// Test(s).
			ASSERT_EQ(m_errorFileNameExpected, operatingSystem.m_file1.m_fileName) << _T("CWebDiagnostics() m_fileName");
#if 000//TODO-PORT
			ASSERT_EQ(m_errorOpenFlagsExpected, operatingSystem.m_file1.m_nOpenFlagsUsed) << _T("LoadErrorFile() m_nOpenFlags");
			ASSERT_EQ((DWORD)1, operatingSystem.m_file1.m_openCount) << _T("CWebDiagnostics() m_openCount");
			ASSERT_EQ((DWORD)1, operatingSystem.m_file1.m_openSuccessCount) << _T("CWebDiagnostics() m_openSuccessCount");
			ASSERT_EQ((DWORD)0, operatingSystem.m_file1.m_closeCount) << _T("CWebDiagnostics() m_closeCount");
			ASSERT_EQ((DWORD)1, operatingSystem.m_file1.m_finalizeCount) << _T("CWebDiagnostics() m_finalizeCount");
			ASSERT_EQ(errorFileContentsLength, operatingSystem.m_file1.m_finalFileLengthRead) << _T("CWebDiagnostics() m_fileLengthOnClose");

			int bufferLengthResult;
			const BYTE *bufferResult = wb.GetErrorBuffer(bufferLengthResult);
			ASSERT_EQ(contentsExpected.length(), bufferLengthResult) << _T("CWebDiagnostics() bufferLengthResult");
			ASSERT_TRUE(0 == memcmp(bufferResult, contentsExpected.c_str(), contentsExpected.length())) << _T("CWebDiagnostics() memcmp");

			// Verify that error file is unchanged.
			ASSERT_TRUE(CUnitTestingUtils::IsEqual(operatingSystem.m_file1.m_finalFileContents, operatingSystem.m_file1.m_finalFileLength,
				errorFileContents, errorFileContentsLength)) << _T("errorFile IsEqual");
				
#endif//TODO-PORT
		}

		/// <summary>
		/// Code coverage and functional test for:
		/// CWebDiagnostics::CWebDiagnostics()
		/// Test with an error file with 1 line.
		/// </summary>
		TEST_F(WebDiagnostics, CWebDiagnostics1Test)
		{
			// Expected results.
			const string contentsExpected(CalcErrorFileHtml(m_errorFileContents1, m_errorFileContentsLength1));

			// Setup.
			COperatingSystemTest operatingSystem(m_errorFileNameExpected,
				m_errorOpenFlagsExpected,
				m_errorFileContents1,
				m_errorFileContentsLength1);
			CWebDiagnostics wb(operatingSystem);

			// Perform operation(s) to be tested.
			wb.Start();

			// Test(s).
			ASSERT_EQ(m_errorFileNameExpected, operatingSystem.m_file1.m_fileName) << _T("CWebDiagnostics() m_fileName");
#if 000//TODO-PORT
			ASSERT_EQ(m_errorOpenFlagsExpected, operatingSystem.m_file1.m_nOpenFlagsUsed) << _T("LoadErrorFile() m_nOpenFlags");
			ASSERT_EQ((DWORD)1, operatingSystem.m_file1.m_openCount) << _T("CWebDiagnostics() m_openCount");
			ASSERT_EQ((DWORD)1, operatingSystem.m_file1.m_openSuccessCount) << _T("CWebDiagnostics() m_openSuccessCount");
			ASSERT_EQ((DWORD)0, operatingSystem.m_file1.m_closeCount) << _T("CWebDiagnostics() m_closeCount");
			ASSERT_EQ((DWORD)1, operatingSystem.m_file1.m_finalizeCount) << _T("CWebDiagnostics() m_finalizeCount");
			ASSERT_EQ(m_errorFileContentsLength1, operatingSystem.m_file1.m_finalFileLengthRead) << _T("CWebDiagnostics() m_fileLengthOnClose");

			int bufferLengthResult;
			const BYTE *bufferResult = wb.GetErrorBuffer(bufferLengthResult);
			ASSERT_EQ(contentsExpected.length(), bufferLengthResult) << _T("CWebDiagnostics() bufferLengthResult");
			ASSERT_TRUE(0 == memcmp(bufferResult, contentsExpected.c_str(), contentsExpected.length())) << _T("CWebDiagnostics() memcmp");

			// Verify that error file is unchanged.
			ASSERT_TRUE(CUnitTestingUtils::IsEqual(operatingSystem.m_file1.m_finalFileContents, operatingSystem.m_file1.m_finalFileLength,
				m_errorFileContents1, m_errorFileContentsLength1)) << _T("errorFile IsEqual");
			
#endif//TODO-PORT
		}

		/// <summary>
		/// Code coverage and functional test for:
		/// CWebDiagnostics::CWebDiagnostics()
		/// Test with an error file with 2 lines.
		/// </summary>
		TEST_F(WebDiagnostics, CWebDiagnostics2Test)
		{
			// Expected results.
			const string contentsExpected(CalcErrorFileHtml(m_errorFileContents2, m_errorFileContentsLength2));

			// Setup.
			COperatingSystemTest operatingSystem(m_errorFileNameExpected,
				m_errorOpenFlagsExpected,
				m_errorFileContents2,
				m_errorFileContentsLength2);
			CWebDiagnostics wb(operatingSystem);

			// Perform operation(s) to be tested.
			wb.Start();

			// Test(s).

			int errorBufferLengthResult;
			const BYTE *errorBufferResult = wb.GetErrorBuffer(errorBufferLengthResult);
#if 000//TODO-PORT
			ASSERT_EQ(contentsExpected.length(), errorBufferLengthResult) << _T("CWebDiagnostics() bufferLengthResult");
			ASSERT_TRUE(0 == memcmp(errorBufferResult, contentsExpected.c_str(), contentsExpected.length())) << _T("CWebDiagnostics() memcmp");

			// Verify that the error file was accessed as expected.
			ASSERT_EQ(m_errorFileNameExpected, operatingSystem.m_file1.m_fileName) << _T("CWebDiagnostics() m_fileName");
			ASSERT_EQ(m_errorOpenFlagsExpected, operatingSystem.m_file1.m_nOpenFlagsUsed) << _T("LoadErrorFile() m_nOpenFlags");
			ASSERT_FALSE(operatingSystem.m_file1.m_isOpen) << _T("CWebDiagnostics() m_isOpen");
			ASSERT_EQ((DWORD)1, operatingSystem.m_file1.m_openCount) << _T("CWebDiagnostics() m_openCount");
			ASSERT_EQ((DWORD)1, operatingSystem.m_file1.m_openSuccessCount) << _T("CWebDiagnostics() m_openSuccessCount");
			ASSERT_EQ((DWORD)0, operatingSystem.m_file1.m_closeCount) << _T("CWebDiagnostics() m_closeCount");
			ASSERT_EQ((DWORD)1, operatingSystem.m_file1.m_finalizeCount) << _T("CWebDiagnostics() m_finalizeCount");
			ASSERT_EQ((ULONGLONG)m_errorFileContentsLength2, operatingSystem.m_file1.m_finalFileGetLength) << _T("CWebDiagnostics() m_finalFileGetLength");
			ASSERT_EQ(m_errorFileContentsLength2, operatingSystem.m_file1.m_finalFileLength) << _T("CWebDiagnostics() m_finalFileLength");
			ASSERT_EQ(m_errorFileContentsLength2, operatingSystem.m_file1.m_finalFileLengthRead) << _T("CWebDiagnostics() m_fileLengthOnClose");

			// Verify that the error file was not changed.
			ASSERT_TRUE(CUnitTestingUtils::IsEqual(operatingSystem.m_file1.m_finalFileContents,
				operatingSystem.m_file1.m_finalFileLength,
				m_errorFileContents2,
				m_errorFileContentsLength2)) << _T("CWebDiagnostics() IsEqual");

			// Check for unexpected accesses.
			ASSERT_EQ((DWORD)1, operatingSystem.m_totalFileCreateCount) << _T("CWebDiagnostics() m_totalFileCreateCount");
			ASSERT_EQ((DWORD)1, operatingSystem.m_totalFileOpenCount) << _T("CWebDiagnostics() m_totalFileOpenCount");
			ASSERT_EQ((DWORD)0, operatingSystem.m_totalFileCloseCount) << _T("CWebDiagnostics() m_totalFileCloseCount");
			ASSERT_EQ((DWORD)1, operatingSystem.m_totalFileFinalizeCount) << _T("CWebDiagnostics() m_totalFileFinalizeCount");
#endif//TODO-PORT			
		}

		/// <summary>
		/// Code coverage and functional test for:
		/// CWebDiagnostics::CWebDiagnostics()
		/// CWebDiagnostics::Start()
		/// CWebDiagnostics::Update()
		/// Test using:
		/// Callback into COperatingSystemTest layer.
		/// Error.txt with 3 lines.
		/// Verify error buffer after LoadErrorFile() was called.
		/// Verify output to StatusDump.html.
		/// Verify some output to MemoryDump.html.
		/// Verify output to ErrorDump.html.
		/// </summary>
		TEST_F(WebDiagnostics, CWebDiagnostics3Test)
		{
			// Inputs.
			const bool errorsFound(m_errorFileContentsLength3 > 0);
			const CPlayerSession playerSession;
			const CEGMConfig egmConfig(MEMORY_NONE);
			const CExcessiveVoucherOut excessiveVoucherOut;
			const CFlashStatus downloadStatus(FlashType::FLASH_NONE, 0, "", "", "", "");

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
				8, "Sentinel is offline", "", "", "", 0.0);

			const CWebDiagnosticsStatusInfo statusInfo(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12),
				1000000 + (1 * SECONDS_PER_MINUTE + 2) * TICK_COUNTS_PER_SECOND,
				true,
				1000000,
				true,
				CUtilities::GetTimeFromFields(2014, 1, 27, 10, 01, 03),
				-480,
				(TiltCode)0x900,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				1,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				1,
				_T("20-6F-EC-00-08-09"),
				_T("20-6F-EC-00-08-08"),
				125,
				75,
				225,
				175,
				3000 * 100,
				7000 * 100,
				_T("Kyocera"),
				_T("Action"),
				_T("602"),
				true,
				false,
                CURRENT_MULTIPLIER,
                CURRENT_DIVIDER,
                2,
				1,
                0,
                false,
				EGMTransferMode_EFT,
				"SAS_AFT_BONUS",
                2,
                3,
				false,
				downloadStatus);

			// Expected results for error buffer.
			const string errorBufferExpected(CalcErrorFileHtml(m_errorFileContents3, m_errorFileContentsLength3));

			// Expected results for status dump file.
			const string statusContentsExpected(CalcStatusPageHtml(commonInfo, statusInfo, playerSession, egmConfig, excessiveVoucherOut, errorsFound));

			// Expected results for the memory dump file.
			// The list of processes is not controlled in this test, so just verify the beginning and end of the file.
			const string memoryHeaderExpected(CalcPageHeaderHtml(m_memoryDumpRefreshSeconds) +
				CalcCommonHeaderExHtml(commonInfo) +
				CalcTableHeaderHtml());
			const string memoryTrailerExpected(CalcTableTrailerHtml() +
				CalcCommonTrailerHtml(commonInfo, errorsFound) +
				CalcPageTrailerHtml());

			// Expected results for the (ErrorDump.html call) stack file.
			const string stackContentsExpected(CalcErrorPageHtml(commonInfo, m_errorFileContents3, m_errorFileContentsLength3));

			// Setup.
			COperatingSystemTest operatingSystem(
				m_errorFileNameExpected, m_errorOpenFlagsExpected, m_errorFileContents3, m_errorFileContentsLength3,
				m_statusFileNameExpected, m_dumpOpenFlagsExpected, nullptr, 0,
				m_memoryFileNameExpected, m_dumpOpenFlagsExpected, nullptr, 0,
				m_stackFileNameExpected, m_dumpOpenFlagsExpected, nullptr, 0);
			CWebDiagnostics wb(operatingSystem);

			// Perform operation(s) to be tested.
			wb.Start();
			wb.Update(commonInfo, statusInfo, playerSession, egmConfig, excessiveVoucherOut);

			// Test(s).

			int errorBufferLengthResult;
			const BYTE *errorBufferResult = wb.GetErrorBuffer(errorBufferLengthResult);
#if 000//TODO-PORT
			ASSERT_EQ(errorBufferExpected.length(), errorBufferLengthResult) << _T("CWebDiagnostics() bufferLengthResult - error");
			ASSERT_TRUE(0 == memcmp(errorBufferResult, errorBufferExpected.c_str(), errorBufferExpected.length())) << _T("CWebDiagnostics() memcmp - error");

			// Verify that the error file was accessed as expected.
			ASSERT_EQ(m_errorFileNameExpected, operatingSystem.m_file1.m_fileName) << _T("CWebDiagnostics() m_fileName - error");
			ASSERT_EQ(m_errorOpenFlagsExpected, operatingSystem.m_file1.m_nOpenFlagsUsed) << _T("CWebDiagnostics() m_nOpenFlagsUsed - error");
			ASSERT_FALSE(operatingSystem.m_file1.m_isOpen) << _T("CWebDiagnostics() m_IsOpen - error");
			ASSERT_EQ((DWORD)1, operatingSystem.m_file1.m_openCount) << _T("CWebDiagnostics() m_openCount - error");
			ASSERT_EQ((DWORD)1, operatingSystem.m_file1.m_openSuccessCount) << _T("CWebDiagnostics() m_openSuccessCount - error");
			ASSERT_EQ((DWORD)0, operatingSystem.m_file1.m_closeCount) << _T("CWebDiagnostics() m_closeCount - error");
			ASSERT_EQ((DWORD)1, operatingSystem.m_file1.m_finalizeCount) << _T("CWebDiagnostics() m_finalizeCount - error");
			ASSERT_EQ((ULONGLONG)m_errorFileContentsLength3, operatingSystem.m_file1.m_finalFileGetLength) << _T("CWebDiagnostics() m_finalFileGetLength - error");
			ASSERT_EQ(m_errorFileContentsLength3, operatingSystem.m_file1.m_finalFileLength) << _T("CWebDiagnostics() m_finalFileLength - error");
			ASSERT_EQ(m_errorFileContentsLength3, operatingSystem.m_file1.m_finalFileLengthRead) << _T("CWebDiagnostics() m_finalFileLengthRead - error");

			// Verify that the error file was not changed.
			ASSERT_TRUE(
				CUnitTestingUtils::IsEqual(m_errorFileContents3,
					m_errorFileContentsLength3,
					operatingSystem.m_file1.m_finalFileContents,
					operatingSystem.m_file1.m_finalFileLength)) <<
				_T("CWebDiagnostics() IsEqual - error");

			// Verify that the status file was accessed as expected.
			ASSERT_EQ(m_statusFileNameExpected, operatingSystem.m_file2.m_fileName) << _T("CWebDiagnostics() m_fileName - status");
			ASSERT_EQ(m_dumpOpenFlagsExpected, operatingSystem.m_file2.m_nOpenFlagsUsed) << _T("CWebDiagnostics() m_nOpenFlagsUsed - status");
			ASSERT_FALSE(operatingSystem.m_file2.m_isOpen) << _T("CWebDiagnostics() m_IsOpen - status");
			ASSERT_EQ((DWORD)1, operatingSystem.m_file2.m_openCount) << _T("CWebDiagnostics() m_openCount - status");
			ASSERT_EQ((DWORD)1, operatingSystem.m_file2.m_openSuccessCount) << _T("CWebDiagnostics() m_openSuccessCount - status");
			ASSERT_EQ((DWORD)0, operatingSystem.m_file2.m_closeCount) << _T("CWebDiagnostics() m_closeCount - status");
			ASSERT_EQ((DWORD)1, operatingSystem.m_file2.m_finalizeCount) << _T("CWebDiagnostics() m_finalizeCount - status");
			ASSERT_EQ((ULONGLONG)statusContentsExpected.length(), operatingSystem.m_file2.m_finalFileGetLength) << _T("CWebDiagnostics() m_finalFileGetLength - status");
			ASSERT_EQ((UINT)statusContentsExpected.length(), operatingSystem.m_file2.m_finalFileLength) << _T("CWebDiagnostics() m_finalFileLength - status");
			ASSERT_EQ((UINT)statusContentsExpected.length(), operatingSystem.m_file2.m_finalFileLengthRead) << _T("CWebDiagnostics() m_finalFileLengthRead - status");

			// Verify the final contents of the status file.
			ASSERT_TRUE(
				CUnitTestingUtils::IsEqual(operatingSystem.m_file2.m_finalFileContents,
					operatingSystem.m_file2.m_finalFileLength,
					statusContentsExpected.c_str(),
					statusContentsExpected.length())) <<
				_T("CWebDiagnostics() IsEqual - status");

			// Verify that the memory (dump) file was accessed as expected.
			ASSERT_EQ(m_memoryFileNameExpected, operatingSystem.m_file3.m_fileName) << _T("CWebDiagnostics() m_fileName - memory");
			ASSERT_EQ(m_dumpOpenFlagsExpected, operatingSystem.m_file3.m_nOpenFlagsUsed) << _T("CWebDiagnostics() m_nOpenFlagsUsed - memory");
			ASSERT_FALSE(operatingSystem.m_file3.m_isOpen) << _T("CWebDiagnostics() m_IsOpen - memory");
			ASSERT_EQ((DWORD)1, operatingSystem.m_file3.m_openCount) << _T("CWebDiagnostics() m_openCount - memory");
			ASSERT_EQ((DWORD)1, operatingSystem.m_file3.m_openSuccessCount) << _T("CWebDiagnostics() m_openSuccessCount - memory");
			ASSERT_EQ((DWORD)0, operatingSystem.m_file3.m_closeCount) << _T("CWebDiagnostics() m_closeCount - memory");
			ASSERT_EQ((DWORD)1, operatingSystem.m_file3.m_finalizeCount) << _T("CWebDiagnostics() m_finalizeCount - memory");
			ASSERT_TRUE(operatingSystem.m_file3.m_finalFileGetLength >= memoryHeaderExpected.length() + memoryTrailerExpected.length()) <<
				_T("CWebDiagnostics() m_finalFileGetLength - memory");
			ASSERT_EQ((UINT)operatingSystem.m_file3.m_finalFileGetLength, operatingSystem.m_file3.m_finalFileLength) << _T("CWebDiagnostics() m_finalFileLength - memory");
			ASSERT_EQ((UINT)operatingSystem.m_file3.m_finalFileGetLength, operatingSystem.m_file3.m_finalFileLengthRead) << _T("CWebDiagnostics() m_finalFileLengthRead - memory");

			// Verify the final contents of the (beginning and end of the) memory (dump) file.
			ASSERT_TRUE(
				CUnitTestingUtils::IsEqual(operatingSystem.m_file3.m_finalFileContents,
				memoryHeaderExpected.length(),
				memoryHeaderExpected.c_str(),
				memoryHeaderExpected.length())) <<
				_T("CWebDiagnostics() IsEqual - memory header");
			ASSERT_TRUE(
				CUnitTestingUtils::IsEqual(
					operatingSystem.m_file3.m_finalFileContents + operatingSystem.m_file3.m_finalFileLength - memoryTrailerExpected.length(),
				memoryTrailerExpected.length(),
				memoryTrailerExpected.c_str(),
				memoryTrailerExpected.length())) <<
				_T("CWebDiagnostics() IsEqual - memory trailer");

			// Verify that the (ErrorDump.html call) stack file was accessed as expected.
			ASSERT_EQ(m_stackFileNameExpected, operatingSystem.m_file4.m_fileName) << _T("CWebDiagnostics() m_fileName - stack");
			ASSERT_EQ(m_dumpOpenFlagsExpected, operatingSystem.m_file4.m_nOpenFlagsUsed) << _T("CWebDiagnostics() m_nOpenFlagsUsed - stack");
			ASSERT_FALSE(operatingSystem.m_file4.m_isOpen) << _T("CWebDiagnostics() m_IsOpen - stack");
			ASSERT_EQ((DWORD)1, operatingSystem.m_file4.m_openCount) << _T("CWebDiagnostics() m_openCount - stack");
			ASSERT_EQ((DWORD)1, operatingSystem.m_file4.m_openSuccessCount) << _T("CWebDiagnostics() m_openSuccessCount - stack");
			ASSERT_EQ((DWORD)0, operatingSystem.m_file4.m_closeCount) << _T("CWebDiagnostics() m_closeCount - stack");
			ASSERT_EQ((DWORD)1, operatingSystem.m_file4.m_finalizeCount) << _T("CWebDiagnostics() m_finalizeCount - stack");
			ASSERT_EQ((ULONGLONG)stackContentsExpected.length(), operatingSystem.m_file4.m_finalFileGetLength) << _T("CWebDiagnostics() m_finalFileGetLength - stack");
			ASSERT_EQ((UINT)stackContentsExpected.length(), operatingSystem.m_file4.m_finalFileLength) << _T("CWebDiagnostics() m_finalFileLength - stack");
			ASSERT_EQ((UINT)stackContentsExpected.length(), operatingSystem.m_file4.m_finalFileLengthRead) << _T("CWebDiagnostics() m_fileLengthOnClose - stack");

			// Verify the final contents of the (ErrorDump.html call) stack file.
			ASSERT_TRUE(
				CUnitTestingUtils::IsEqual(operatingSystem.m_file4.m_finalFileContents,
				operatingSystem.m_file4.m_finalFileLength,
				stackContentsExpected.c_str(),
				stackContentsExpected.length())) <<
				_T("CWebDiagnostics() IsEqual - stack");

			// Check for unexpected accesses.
			ASSERT_EQ((DWORD)4, operatingSystem.m_totalFileCreateCount) << _T("CWebDiagnostics() m_totalFileCreateCount");
			ASSERT_EQ((DWORD)4, operatingSystem.m_totalFileOpenCount) << _T("CWebDiagnostics() m_totalFileOpenCount");
			ASSERT_EQ((DWORD)0, operatingSystem.m_totalFileCloseCount) << _T("CWebDiagnostics() m_totalFileCloseCount");
			ASSERT_EQ((DWORD)4, operatingSystem.m_totalFileFinalizeCount) << _T("CWebDiagnostics() m_totalFileFinalizeCount");
#endif//TODO-PORT			
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CWebDiagnostics::CWebDiagnostics()
		///     CWebDiagnostics::Start()
		///     CWebDiagnostics::Update()
		/// Test using:
		///     Callback into COperatingSystemTest layer.
		///     Error.txt with 3 lines, but we only allocate enough buffer for 2 of the lines.
		///     Verify error buffer after LoadErrorFile() was called.
		///     Verify output to StatusDump.html.
		///     Verify some output to MemoryDump.html.
		///     Verify output to ErrorDump.html.
		/// </summary>
		TEST_F(WebDiagnostics, CWebDiagnostics3bTest)
		{
			// Inputs.
			const bool errorsFound(true);
			const CPlayerSession playerSession;
			const CEGMConfig egmConfig(MEMORY_NONE);
			const CExcessiveVoucherOut excessiveVoucherOut;
			const CFlashStatus downloadStatus(FlashType::FLASH_NONE, 0, "", "", "", "");

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
				8, "Sentinel is offline", "", "", "", 0.0);

			const CWebDiagnosticsStatusInfo statusInfo(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12),
				1000000 + (1 * SECONDS_PER_MINUTE + 2) * TICK_COUNTS_PER_SECOND,
				true,
				1000000,
				true,
				CUtilities::GetTimeFromFields(2014, 1, 27, 10, 01, 03),
				-480,
				(TiltCode)0x900,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				1,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				1,
				_T("20-6F-EC-00-08-09"),
				_T("20-6F-EC-00-08-08"),
				125,
				75,
				225,
				175,
				3000 * 100,
				7000 * 100,
				_T("Kyocera"),
				_T("Action"),
				_T("602"),
				true,
				false,
                CURRENT_MULTIPLIER,
                CURRENT_DIVIDER,
                2,
				1,
                0,
                false,
				EGMTransferMode_EFT,
				"SAS_AFT_BONUS",
                2,
                3,
				false,
				downloadStatus);

			const UINT bufferSize(42);

			// Expected results for error buffer.
			const string errorBufferExpected(CalcErrorFileHtml(m_errorFileContents3, m_errorFileContentsLength3, bufferSize));

			// Expected results for status dump file.
			const string statusContentsExpected(CalcStatusPageHtml(commonInfo, statusInfo, playerSession, egmConfig, excessiveVoucherOut, errorsFound));

			// Expected results for the memory dump file.
			// The list of processes is not controlled in this test, so just verify the beginning and end of the file.
			const string memoryHeaderExpected(CalcPageHeaderHtml(m_memoryDumpRefreshSeconds) +
				CalcCommonHeaderExHtml(commonInfo) +
				CalcTableHeaderHtml());
			const string memoryTrailerExpected(CalcTableTrailerHtml() +
				CalcCommonTrailerHtml(commonInfo, errorsFound) +
				CalcPageTrailerHtml());

			// Expected results for the (ErrorDump.html call) stack file.
			const string stackContentsExpected(CalcErrorPageHtml(commonInfo, m_errorFileContents3, m_errorFileContentsLength3, bufferSize));

			// Setup.
			COperatingSystemTest operatingSystem(
				m_errorFileNameExpected, m_errorOpenFlagsExpected, m_errorFileContents3, m_errorFileContentsLength3,
				m_statusFileNameExpected, m_dumpOpenFlagsExpected, nullptr, 0,
				m_memoryFileNameExpected, m_dumpOpenFlagsExpected, nullptr, 0,
				m_stackFileNameExpected, m_dumpOpenFlagsExpected, nullptr, 0);
			CWebDiagnostics wb(operatingSystem, bufferSize);

			// Perform operation(s) to be tested.
			wb.Start();
			wb.Update(commonInfo, statusInfo, playerSession, egmConfig, excessiveVoucherOut);

			// Test(s).

			int errorBufferLengthResult;
			const BYTE *errorBufferResult = wb.GetErrorBuffer(errorBufferLengthResult);
#if 000//TODO-PORT
			ASSERT_EQ(errorBufferExpected.length(), errorBufferLengthResult) << _T("CWebDiagnostics() bufferLengthResult - error");
			ASSERT_TRUE(0 == memcmp(errorBufferResult, errorBufferExpected.c_str(), errorBufferExpected.length())) << _T("CWebDiagnostics() memcmp - error");

			// Verify that the error file was accessed as expected.
			ASSERT_EQ(m_errorFileNameExpected, operatingSystem.m_file1.m_fileName) << _T("CWebDiagnostics() m_fileName - error");
			ASSERT_EQ(m_errorOpenFlagsExpected, operatingSystem.m_file1.m_nOpenFlagsUsed) << _T("CWebDiagnostics() m_nOpenFlagsUsed - error");
			ASSERT_FALSE(operatingSystem.m_file1.m_isOpen) << _T("CWebDiagnostics() m_IsOpen - error");
			ASSERT_EQ((DWORD)1, operatingSystem.m_file1.m_openCount) << _T("CWebDiagnostics() m_openCount - error");
			ASSERT_EQ((DWORD)1, operatingSystem.m_file1.m_openSuccessCount) << _T("CWebDiagnostics() m_openSuccessCount - error");
			ASSERT_EQ((DWORD)0, operatingSystem.m_file1.m_closeCount) << _T("CWebDiagnostics() m_closeCount - error");
			ASSERT_EQ((DWORD)1, operatingSystem.m_file1.m_finalizeCount) << _T("CWebDiagnostics() m_finalizeCount - error");
			ASSERT_EQ((ULONGLONG)m_errorFileContentsLength3, operatingSystem.m_file1.m_finalFileGetLength) << _T("CWebDiagnostics() m_finalFileGetLength - error");
			ASSERT_EQ(m_errorFileContentsLength3, operatingSystem.m_file1.m_finalFileLength) << _T("CWebDiagnostics() m_finalFileLength - error");
			ASSERT_EQ(m_errorFileContentsLength3, operatingSystem.m_file1.m_finalFileLengthRead) << _T("CWebDiagnostics() m_finalFileLengthRead - error");

			// Verify that the error file was not changed.
			ASSERT_TRUE(
				CUnitTestingUtils::IsEqual(m_errorFileContents3,
					m_errorFileContentsLength3,
					operatingSystem.m_file1.m_finalFileContents,
					operatingSystem.m_file1.m_finalFileLength)) <<
				_T("CWebDiagnostics() IsEqual - error");

			// Verify that the status file was accessed as expected.
			ASSERT_EQ(m_statusFileNameExpected, operatingSystem.m_file2.m_fileName) << _T("CWebDiagnostics() m_fileName - status");
			ASSERT_FALSE(operatingSystem.m_file2.m_isOpen) << _T("CWebDiagnostics() m_IsOpen - status");
			ASSERT_EQ((DWORD)1, operatingSystem.m_file2.m_openCount) << _T("CWebDiagnostics() m_openCount - status");
			ASSERT_EQ((DWORD)1, operatingSystem.m_file2.m_openSuccessCount) << _T("CWebDiagnostics() m_openSuccessCount - status");
			ASSERT_EQ((DWORD)0, operatingSystem.m_file2.m_closeCount) << _T("CWebDiagnostics() m_closeCount - status");
			ASSERT_EQ((DWORD)1, operatingSystem.m_file2.m_finalizeCount) << _T("CWebDiagnostics() m_finalizeCount - status");
			ASSERT_EQ((ULONGLONG)statusContentsExpected.length(), operatingSystem.m_file2.m_finalFileGetLength) << _T("CWebDiagnostics() m_finalFileGetLength - status");
			ASSERT_EQ((UINT)statusContentsExpected.length(), operatingSystem.m_file2.m_finalFileLength) << _T("CWebDiagnostics() m_finalFileLength - status");
			ASSERT_EQ((UINT)statusContentsExpected.length(), operatingSystem.m_file2.m_finalFileLengthRead) << _T("CWebDiagnostics() m_finalFileLengthRead - status");

			// Verify the final contents of the status file.
			ASSERT_TRUE(
				CUnitTestingUtils::IsEqual(operatingSystem.m_file2.m_finalFileContents,
					operatingSystem.m_file2.m_finalFileLength,
					statusContentsExpected.c_str(),
					statusContentsExpected.length())) <<
				_T("CWebDiagnostics() IsEqual - status");

			// Verify that the memory (dump) file was accessed as expected.
			ASSERT_EQ(m_memoryFileNameExpected, operatingSystem.m_file3.m_fileName) << _T("CWebDiagnostics() m_fileName - memory");
			ASSERT_FALSE(operatingSystem.m_file3.m_isOpen) << _T("CWebDiagnostics() m_IsOpen - memory");
			ASSERT_EQ((DWORD)1, operatingSystem.m_file3.m_openCount) << _T("CWebDiagnostics() m_openCount - memory");
			ASSERT_EQ((DWORD)1, operatingSystem.m_file3.m_openSuccessCount) << _T("CWebDiagnostics() m_openCount - memory");
			ASSERT_EQ((DWORD)0, operatingSystem.m_file3.m_closeCount) << _T("CWebDiagnostics() m_closeCount - memory");
			ASSERT_EQ((DWORD)1, operatingSystem.m_file3.m_finalizeCount) << _T("CWebDiagnostics() m_finalizeCount - memory");
			ASSERT_TRUE(operatingSystem.m_file3.m_finalFileGetLength >= memoryHeaderExpected.length() + memoryTrailerExpected.length()) <<
				_T("CWebDiagnostics() m_finalFileGetLength - memory");
			ASSERT_EQ((UINT)operatingSystem.m_file3.m_finalFileGetLength, operatingSystem.m_file3.m_finalFileLength) << _T("CWebDiagnostics() m_finalFileLength - memory");
			ASSERT_EQ((UINT)operatingSystem.m_file3.m_finalFileGetLength, operatingSystem.m_file3.m_finalFileLengthRead) << _T("CWebDiagnostics() m_finalFileLengthRead - memory");

			// Verify the final contents of the (beginning and end of the) memory (dump) file.
			ASSERT_TRUE(
				CUnitTestingUtils::IsEqual(operatingSystem.m_file3.m_finalFileContents,
				memoryHeaderExpected.length(),
				memoryHeaderExpected.c_str(),
				memoryHeaderExpected.length())) <<
				_T("CWebDiagnostics() IsEqual - memory header");
			ASSERT_TRUE(
				CUnitTestingUtils::IsEqual(
					operatingSystem.m_file3.m_finalFileContents + operatingSystem.m_file3.m_finalFileLength - memoryTrailerExpected.length(),
				memoryTrailerExpected.length(),
				memoryTrailerExpected.c_str(),
				memoryTrailerExpected.length())) <<
				_T("CWebDiagnostics() IsEqual - memory trailer");

			// Verify that the (ErrorDump.html call) stack file was accessed as expected.
			ASSERT_EQ(m_stackFileNameExpected, operatingSystem.m_file4.m_fileName) << _T("CWebDiagnostics() m_fileName - stack");
			ASSERT_FALSE(operatingSystem.m_file4.m_isOpen) << _T("CWebDiagnostics() m_IsOpen - stack");
			ASSERT_EQ((DWORD)1, operatingSystem.m_file4.m_openCount) << _T("CWebDiagnostics() m_openCount - stack");
			ASSERT_EQ((DWORD)1, operatingSystem.m_file4.m_openSuccessCount) << _T("CWebDiagnostics() m_openCount - stack");
			ASSERT_EQ((DWORD)0, operatingSystem.m_file4.m_closeCount) << _T("CWebDiagnostics() m_closeCount - stack");
			ASSERT_EQ((DWORD)1, operatingSystem.m_file4.m_finalizeCount) << _T("CWebDiagnostics() m_finalizeCount - stack");
			ASSERT_EQ((ULONGLONG)stackContentsExpected.length(), operatingSystem.m_file4.m_finalFileGetLength) << _T("CWebDiagnostics() m_finalFileGetLength - stack");
			ASSERT_EQ((UINT)stackContentsExpected.length(), operatingSystem.m_file4.m_finalFileLength) << _T("CWebDiagnostics() m_finalFileLength - stack");
			ASSERT_EQ((UINT)stackContentsExpected.length(), operatingSystem.m_file4.m_finalFileLengthRead) << _T("CWebDiagnostics() m_finalFileLengthRead - stack");

			// Verify the final contents of the (ErrorDump.html call) stack file.
			ASSERT_TRUE(
				CUnitTestingUtils::IsEqual(operatingSystem.m_file4.m_finalFileContents,
				operatingSystem.m_file4.m_finalFileLength,
				stackContentsExpected.c_str(),
				stackContentsExpected.length())) <<
				_T("CWebDiagnostics() IsEqual - stack");

			// Check for unexpected accesses.
			ASSERT_EQ((DWORD)4, operatingSystem.m_totalFileCreateCount) << _T("CWebDiagnostics() m_totalFileCreateCount");
			ASSERT_EQ((DWORD)4, operatingSystem.m_totalFileOpenCount) << _T("CWebDiagnostics() m_totalFileOpenCount");
			ASSERT_EQ((DWORD)0, operatingSystem.m_totalFileCloseCount) << _T("CWebDiagnostics() m_totalFileCloseCount");
			ASSERT_EQ((DWORD)4, operatingSystem.m_totalFileFinalizeCount) << _T("CWebDiagnostics() m_totalFileFinalizeCount");
#endif//TODO-PORT			
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CWebDiagnostics::CWebDiagnostics()
		///     CWebDiagnostics::Start()
		///     CWebDiagnostics::Update()
		/// Test using:
		/// Callback into COperatingSystemTest3 layer.
		/// Error.txt with 3 lines.
		/// Verify error buffer each time after LoadErrorFile() was called.
		/// Verify output to StatusDump.html.
		/// Verify some output to MemoryDump.html.
		/// Verify output to ErrorDump.html.
		/// Verify that the Error.txt file was not changed by code under test.
		/// </summary>----------------
		TEST_F(WebDiagnostics, CWebDiagnostics_FileUpdateTest)
		{
			// Inputs.
			const bool errorsFound(true);
			const CPlayerSession playerSession;
			const CEGMConfig egmConfig(MEMORY_NONE);
            const CExcessiveVoucherOut excessiveVoucherOut;
			const CFlashStatus downloadStatus(FlashType::FLASH_NONE, 0, "", "", "", "");

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
				8, "Sentinel is offline", "", "", "", 0.0);

			const CWebDiagnosticsStatusInfo statusInfo(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12),
				1000000 + (1 * SECONDS_PER_MINUTE + 2) * TICK_COUNTS_PER_SECOND,
				true,
				1000000,
				true,
				CUtilities::GetTimeFromFields(2014, 1, 27, 10, 01, 03),
				-480,
				(TiltCode)0x900,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				1,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				1,
				_T("20-6F-EC-00-08-09"),
				_T("20-6F-EC-00-08-08"),
				125,
				75,
				225,
				175,
				3000 * 100,
				7000 * 100,
				_T("Kyocera"),
				_T("Action"),
				_T("602"),
				true,
				false,
                CURRENT_MULTIPLIER,
                CURRENT_DIVIDER,
                2,
				1,
                0,
                false,
				EGMTransferMode_NONE,
				"SAS_AFT_BONUS",
                2,
                3,
				false,
				downloadStatus);

			// Expected results for error buffer, for the 1st and 2nd sets of tests.
			const string errorBufferExpected(CalcErrorFileHtml(m_errorFileContents3, m_errorFileContentsLength3));

			// Expected results for error buffer, for the 3rd set of tests.
			const string errorBufferExpected3(CalcErrorFileHtml(m_errorFileContents4, m_errorFileContentsLength4));

			// Expected results for status dump file.
			const string statusContentsExpected(CalcStatusPageHtml(commonInfo, statusInfo, playerSession, egmConfig, excessiveVoucherOut, errorsFound));

			// Expected results for the memory dump file.
			// The list of processes is not controlled in this test, so just verify the beginning and end of the file.
			const string memoryHeaderExpected(CalcPageHeaderHtml(m_memoryDumpRefreshSeconds) +
				CalcCommonHeaderExHtml(commonInfo) +
				CalcTableHeaderHtml());
			const string memoryTrailerExpected(CalcTableTrailerHtml() +
				CalcCommonTrailerHtml(commonInfo, errorsFound) +
				CalcPageTrailerHtml());

			// Expected results for the (ErrorDump.html call) stack file, for the 1st and 2nd sets of tests.
			const string stackContentsExpected(CalcErrorPageHtml(commonInfo, m_errorFileContents3, m_errorFileContentsLength3));

			// Expected results for the (ErrorDump.html call) stack file, for the 3rd set of tests.
			const string stackContentsExpected3(CalcErrorPageHtml(commonInfo, m_errorFileContents4, m_errorFileContentsLength4));

			// Setup.
			COperatingSystemTest operatingSystem(
				m_errorFileNameExpected, m_errorOpenFlagsExpected, m_errorFileContents3, m_errorFileContentsLength3,
				m_statusFileNameExpected, m_dumpOpenFlagsExpected, nullptr, 0,
				m_memoryFileNameExpected, m_dumpOpenFlagsExpected, nullptr, 0,
				m_stackFileNameExpected, m_dumpOpenFlagsExpected, nullptr, 0);
			CWebDiagnostics wb(operatingSystem);

			// Perform operation(s) to be tested.
			wb.Start();
			wb.Update(commonInfo, statusInfo, playerSession, egmConfig, excessiveVoucherOut);

			// Test(s).

			// First set of tests for error-related updates.
			// Verify that the error buffer was updated as expected.
			{
				int errorBufferLengthResult;
				const BYTE *errorBufferResult = wb.GetErrorBuffer(errorBufferLengthResult);
#if 000//TODO-PORT
				ASSERT_EQ(errorBufferExpected.length(), errorBufferLengthResult) << _T("bufferLengthResult - error");
				ASSERT_TRUE(0 == memcmp(errorBufferResult, errorBufferExpected.c_str(), errorBufferExpected.length())) << _T("memcmp - error");
#endif//TODO-PORT				
			}

			// Verify that the error file was accessed as expected.
#if 000//TODO-PORT
			ASSERT_EQ(m_errorFileNameExpected, operatingSystem.m_file1.m_fileName) << _T("m_fileName - error");
			ASSERT_EQ(m_errorOpenFlagsExpected, operatingSystem.m_file1.m_nOpenFlagsUsed) << _T("CWebDiagnostics() m_nOpenFlagsUsed - error");
			ASSERT_FALSE(operatingSystem.m_file1.m_isOpen) << _T("m_isOpen - error");
			ASSERT_EQ((DWORD)1, operatingSystem.m_file1.m_openCount) << _T("m_openCount - error");
			ASSERT_EQ((DWORD)1, operatingSystem.m_file1.m_openSuccessCount) << _T("m_openSuccessCount -- error");
			ASSERT_EQ((DWORD)0, operatingSystem.m_file1.m_closeCount) << _T("m_closeCount - error");
			ASSERT_EQ((DWORD)1, operatingSystem.m_file1.m_finalizeCount) << _T("m_finalizeCount - error");
			ASSERT_EQ((ULONGLONG)m_errorFileContentsLength3, operatingSystem.m_file1.m_finalFileGetLength) << _T("m_finalFileGetLength - error");
			ASSERT_EQ(m_errorFileContentsLength3, operatingSystem.m_file1.m_finalFileLength) << _T("m_finalFileLength - error");
			ASSERT_EQ(m_errorFileContentsLength3, operatingSystem.m_file1.m_finalFileLengthRead) << _T("m_finalFileLengthRead - error");

			// Verify that the error file was not changed.
			ASSERT_TRUE(
				CUnitTestingUtils::IsEqual(m_errorFileContents3,
				m_errorFileContentsLength3,
				operatingSystem.m_file1.m_finalFileContents,
				operatingSystem.m_file1.m_finalFileLength)) <<
				_T("IsEqual - error");

			// Verify that the (ErrorDump.html call) stack file was accessed as expected.
			ASSERT_EQ(m_stackFileNameExpected, operatingSystem.m_file4.m_fileName) << _T("m_fileName - stack");
			ASSERT_FALSE(operatingSystem.m_file4.m_isOpen) << _T("m_IsOpen - stack");
			ASSERT_EQ((DWORD)1, operatingSystem.m_file4.m_openCount) << _T("m_openCount - stack");
			ASSERT_EQ((DWORD)1, operatingSystem.m_file4.m_openSuccessCount) << _T("m_openSuccessCount - stack");
			ASSERT_EQ((DWORD)0, operatingSystem.m_file4.m_closeCount) << _T("m_closeCount - stack");
			ASSERT_EQ((DWORD)1, operatingSystem.m_file4.m_finalizeCount) << _T("m_finalizeCount - stack");
			ASSERT_EQ((ULONGLONG)stackContentsExpected.length(), operatingSystem.m_file4.m_finalFileGetLength) << _T("CWebDiagnostics() m_finalFileGetLength - stack");
			ASSERT_EQ((UINT)stackContentsExpected.length(), operatingSystem.m_file4.m_finalFileLength) << _T("m_finalFileLength - stack");
			ASSERT_EQ((UINT)stackContentsExpected.length(), operatingSystem.m_file4.m_finalFileLengthRead) << _T("m_finalFileLengthRead - stack");

			// Verify the "final" contents of the (ErrorDump.html call) stack file.
			ASSERT_TRUE(
				CUnitTestingUtils::IsEqual(operatingSystem.m_file4.m_finalFileContents,
				operatingSystem.m_file4.m_finalFileLength,
				stackContentsExpected.c_str(),
				stackContentsExpected.length())) <<
				_T("CWebDiagnostics() IsEqual - stack");



			// Change the error file and update the outputs again.
			// Could change the current time and verify all the files for each pass.
			operatingSystem.m_file1.SetContents(m_errorFileContents4, m_errorFileContentsLength4);
			wb.Update(commonInfo, statusInfo, playerSession, egmConfig, nullptr, 0);

			// Second set of tests for error-related updates.
			// Verify that the error buffer is unchanged as expected.
			{
				int errorBufferLengthResult;
				const BYTE *errorBufferResult = wb.GetErrorBuffer(errorBufferLengthResult);
				ASSERT_EQ(errorBufferExpected.length(), errorBufferLengthResult) << _T("CWebDiagnostics() bufferLengthResult - error2");
				ASSERT_TRUE(0 == memcmp(errorBufferResult, errorBufferExpected.c_str(), errorBufferExpected.length())) << _T("CWebDiagnostics() memcmp - error2");
			}

			// Verify that the error file was not accessed again, as expected.
			ASSERT_EQ(m_errorFileNameExpected, operatingSystem.m_file1.m_fileName) << _T("m_fileName - error2");
			ASSERT_EQ(m_errorOpenFlagsExpected, operatingSystem.m_file1.m_nOpenFlagsUsed) << _T("CWebDiagnostics() m_nOpenFlagsUsed - error");
			ASSERT_FALSE(operatingSystem.m_file1.m_isOpen) << _T("m_IsOpen - error2");
			ASSERT_EQ((DWORD)1, operatingSystem.m_file1.m_openCount) << _T("m_openCount - error2");
			ASSERT_EQ((DWORD)1, operatingSystem.m_file1.m_openSuccessCount) << _T("m_openSuccessCount");
			ASSERT_EQ((DWORD)0, operatingSystem.m_file1.m_closeCount) << _T("m_closeCount - error2");
			ASSERT_EQ((DWORD)1, operatingSystem.m_file1.m_finalizeCount) << _T("m_finalizeCount - error2");
			ASSERT_EQ((ULONGLONG)m_errorFileContentsLength3, operatingSystem.m_file1.m_finalFileGetLength) << _T("m_finalFileGetLength - error2");
			ASSERT_EQ(m_errorFileContentsLength4, operatingSystem.m_file1.m_finalFileLength) << _T("m_finalFileLength - error2");
			ASSERT_EQ(m_errorFileContentsLength3, operatingSystem.m_file1.m_finalFileLengthRead) << _T("m_finalFileLengthRead - error2");

			// Verify that the error file was not changed.
			ASSERT_TRUE(
				CUnitTestingUtils::IsEqual(m_errorFileContents4,
				m_errorFileContentsLength4,
				operatingSystem.m_file1.m_finalFileContents,
				operatingSystem.m_file1.m_finalFileLength)) <<
				_T("IsEqual - error2");

			// Verify that the (ErrorDump.html call) stack file was accessed as expected.
			ASSERT_EQ(m_stackFileNameExpected, operatingSystem.m_file4.m_fileName) << _T("_fileName - stack2");
			ASSERT_FALSE(operatingSystem.m_file4.m_isOpen) << _T("m_IsOpen - stack2");
			ASSERT_EQ((DWORD)2, operatingSystem.m_file4.m_openCount) << _T("m_openCount - stack2");
			ASSERT_EQ((DWORD)2, operatingSystem.m_file4.m_openSuccessCount) << _T("m_openSuccessCount - stack2");
			ASSERT_EQ((DWORD)0, operatingSystem.m_file4.m_closeCount) << _T("m_closeCount - stack2");
			ASSERT_EQ((DWORD)2, operatingSystem.m_file4.m_finalizeCount) << _T("m_finalizeCount - stack2");
			ASSERT_EQ((ULONGLONG)stackContentsExpected.length(), operatingSystem.m_file4.m_finalFileGetLength) << _T("m_finalFileGetLength - stack2");
			ASSERT_EQ((UINT)stackContentsExpected.length(), operatingSystem.m_file4.m_finalFileLength) << _T("m_finalFileLength - stack2");
			ASSERT_EQ((UINT)stackContentsExpected.length(), operatingSystem.m_file4.m_finalFileLengthRead) << _T("m_finalFileLengthRead - stack2");

			// Verify the "final" contents of the (ErrorDump.html call) stack file, again.
			ASSERT_TRUE(
				CUnitTestingUtils::IsEqual(operatingSystem.m_file4.m_finalFileContents,
				operatingSystem.m_file4.m_finalFileLength,
				stackContentsExpected.c_str(),
				stackContentsExpected.length())) <<
				_T("IsEqual - stack2");



			// Change the diagnostic file update count and update the outputs again.
			// Could change the current time and verify all the files for each pass.
			CDiagnostics::ReportRebootToFile("", tltUnknown, _T(""));
			wb.Update(commonInfo, statusInfo, playerSession, egmConfig, nullptr, 0);

			// Second set of tests for error-related updates.
			// Verify that the error buffer is unchanged as expected.
			{
				int errorBufferLengthResult;
				const BYTE *errorBufferResult = wb.GetErrorBuffer(errorBufferLengthResult);
				ASSERT_EQ(errorBufferExpected3.length(), errorBufferLengthResult) << _T("bufferLengthResult - error3");
				ASSERT_TRUE(0 == memcmp(errorBufferResult, errorBufferExpected3.c_str(), errorBufferExpected3.length())) << _T("memcmp - error3");
			}

			// Verify that the error file was accessed again, as expected.
			ASSERT_EQ(m_errorFileNameExpected, operatingSystem.m_file1.m_fileName) << _T("m_fileName - error3");
			ASSERT_EQ(m_errorOpenFlagsExpected, operatingSystem.m_file1.m_nOpenFlagsUsed) << _T("CWebDiagnostics() m_nOpenFlagsUsed - error");
			ASSERT_FALSE(operatingSystem.m_file1.m_isOpen) << _T("m_IsOpen - error3");
			ASSERT_EQ((DWORD)2, operatingSystem.m_file1.m_openCount) << _T("m_openCount - error3");
			ASSERT_EQ((DWORD)2, operatingSystem.m_file1.m_openSuccessCount) << _T("m_openSuccessCount - error3");
			ASSERT_EQ((DWORD)0, operatingSystem.m_file1.m_closeCount) << _T("m_closeCount - error3");
			ASSERT_EQ((DWORD)2, operatingSystem.m_file1.m_finalizeCount) << _T("m_finalizeCount - error3");
			ASSERT_EQ((ULONGLONG)m_errorFileContentsLength4, operatingSystem.m_file1.m_finalFileGetLength) << _T("m_finalFileGetLength - error3");
			ASSERT_EQ(m_errorFileContentsLength4, operatingSystem.m_file1.m_finalFileLength) << _T("m_finalFileLength - error3");
			ASSERT_EQ(m_errorFileContentsLength4, operatingSystem.m_file1.m_finalFileLengthRead) << _T("m_finalFileLengthRead - error3");

			// Verify that the error file was not changed.
			ASSERT_TRUE(
				CUnitTestingUtils::IsEqual(m_errorFileContents4,
				m_errorFileContentsLength4,
				operatingSystem.m_file1.m_finalFileContents,
				operatingSystem.m_file1.m_finalFileLength)) <<
				_T("IsEqual - error3");

			// Verify that the (ErrorDump.html call) stack file was accessed as expected.
			ASSERT_EQ(m_stackFileNameExpected, operatingSystem.m_file4.m_fileName) << _T("m_fileName - stack3");
			ASSERT_FALSE(operatingSystem.m_file4.m_isOpen) << _T("m_IsOpen - stack3");
			ASSERT_EQ((DWORD)3, operatingSystem.m_file4.m_openCount) << _T("m_openCount - stack3");
			ASSERT_EQ((DWORD)3, operatingSystem.m_file4.m_openSuccessCount) << _T("m_openSuccessCount - stack3");
			ASSERT_EQ((DWORD)0, operatingSystem.m_file4.m_closeCount) << _T("m_closeCount - stack3");
			ASSERT_EQ((DWORD)3, operatingSystem.m_file4.m_finalizeCount) << _T("m_finalizeCount - stack3");
			ASSERT_EQ((ULONGLONG)stackContentsExpected3.length(), operatingSystem.m_file4.m_finalFileGetLength) << _T("m_finalFileGetLength - stack3");
			ASSERT_EQ((UINT)stackContentsExpected3.length(), operatingSystem.m_file4.m_finalFileLength) << _T("m_finalFileLength - stack3");
			ASSERT_EQ((UINT)stackContentsExpected3.length(), operatingSystem.m_file4.m_finalFileLengthRead) << _T("m_finalFileLengthRead - stack3");

			// Verify the "final" contents of the (ErrorDump.html call) stack file, again.
			ASSERT_TRUE(
				CUnitTestingUtils::IsEqual(operatingSystem.m_file4.m_finalFileContents,
				operatingSystem.m_file4.m_finalFileLength,
				stackContentsExpected3.c_str(),
				stackContentsExpected3.length())) <<
				_T("CWebDiagnostics() IsEqual - stack3");


			// Set of tests for non-error-related updates.
			// Third set of tests for error-related updates.
			// Verify that the status file was accessed as expected.
			ASSERT_EQ(m_statusFileNameExpected, operatingSystem.m_file2.m_fileName) << _T("m_fileName - status3");
			ASSERT_FALSE(operatingSystem.m_file2.m_isOpen) << _T("m_IsOpen - status3");
			ASSERT_EQ((DWORD)3, operatingSystem.m_file2.m_openCount) << _T("m_openCount - status3");
			ASSERT_EQ((DWORD)3, operatingSystem.m_file2.m_openSuccessCount) << _T("m_openSuccessCount - status3");
			ASSERT_EQ((DWORD)0, operatingSystem.m_file2.m_closeCount) << _T("m_closeCount - status3");
			ASSERT_EQ((DWORD)3, operatingSystem.m_file2.m_finalizeCount) << _T("m_finalizeCount - status3");
			ASSERT_EQ((ULONGLONG)statusContentsExpected.length(), operatingSystem.m_file2.m_finalFileGetLength) << _T("m_finalFileGetLength - status3");
			ASSERT_EQ((UINT)statusContentsExpected.length(), operatingSystem.m_file2.m_finalFileLength) << _T("m_finalFileLength - status3");
			ASSERT_EQ((UINT)statusContentsExpected.length(), operatingSystem.m_file2.m_finalFileLengthRead) << _T("m_finalFileLengthRead - status3");

			// Verify the final contents of the status file.
			ASSERT_TRUE(
				CUnitTestingUtils::IsEqual(operatingSystem.m_file2.m_finalFileContents,
				operatingSystem.m_file2.m_finalFileLength,
				statusContentsExpected.c_str(),
				statusContentsExpected.length())) <<
				_T("IsEqual - status3");

			// Verify that the memory (dump) file was accessed as expected.
			ASSERT_EQ(m_memoryFileNameExpected, operatingSystem.m_file3.m_fileName) << _T("m_fileName - memory3");
			ASSERT_FALSE(operatingSystem.m_file3.m_isOpen) << _T("m_IsOpen - memory3");
			ASSERT_EQ((DWORD)3, operatingSystem.m_file3.m_openCount) << _T("m_openCount - memory3");
			ASSERT_EQ((DWORD)3, operatingSystem.m_file3.m_openSuccessCount) << _T("m_openSuccessCount - memory3");
			ASSERT_EQ((DWORD)0, operatingSystem.m_file3.m_closeCount) << _T("m_closeCount - memory3");
			ASSERT_EQ((DWORD)3, operatingSystem.m_file3.m_finalizeCount) << _T("m_finalizeCount - memory3");
			ASSERT_TRUE(operatingSystem.m_file3.m_finalFileGetLength >= memoryHeaderExpected.length() + memoryTrailerExpected.length()) <<
				_T("m_finalFileGetLength - memory3");
			ASSERT_EQ((UINT)operatingSystem.m_file3.m_finalFileGetLength, operatingSystem.m_file3.m_finalFileLength) << _T("m_finalFileLength - memory3");
			ASSERT_EQ((UINT)operatingSystem.m_file3.m_finalFileGetLength, operatingSystem.m_file3.m_finalFileLengthRead) << _T("m_finalFileLengthRead - memory3");

			// Verify the final contents of the (beginning and end of the) memory (dump) file.
			ASSERT_TRUE(
				CUnitTestingUtils::IsEqual(operatingSystem.m_file3.m_finalFileContents,
				memoryHeaderExpected.length(),
				memoryHeaderExpected.c_str(),
				memoryHeaderExpected.length())) <<
				_T("CWebDiagnostics() IsEqual - memory header3");
			bool ok = CUnitTestingUtils::IsEqual(
					operatingSystem.m_file3.m_finalFileContents + operatingSystem.m_file3.m_finalFileLength - memoryTrailerExpected.length(),
				memoryTrailerExpected.length(),
				memoryTrailerExpected.c_str(),
				memoryTrailerExpected.length());
			ASSERT_TRUE(ok) <<
				_T("CWebDiagnostics() IsEqual - memory trailer3 actual=\"") +
				string(static_cast<LPCSTR>(operatingSystem.m_file3.m_finalFileContents) + operatingSystem.m_file3.m_finalFileLength - memoryTrailerExpected.length(),
					memoryTrailerExpected.length()) +
				_T("\" expected=\"") + string(memoryTrailerExpected) + _T("\"");

			// Check for unexpected accesses.
			ASSERT_EQ((DWORD)11, operatingSystem.m_totalFileCreateCount) << _T("CWebDiagnostics() m_totalFileCreateCount");
			ASSERT_EQ((DWORD)11, operatingSystem.m_totalFileOpenCount) << _T("CWebDiagnostics() m_totalFileOpenCount");
			ASSERT_EQ((DWORD)0, operatingSystem.m_totalFileCloseCount) << _T("CWebDiagnostics() m_totalFileCloseCount");
			ASSERT_EQ((DWORD)11, operatingSystem.m_totalFileFinalizeCount) << _T("CWebDiagnostics() m_totalFileFinalizeCount");
#endif//TODO-PORT			
		}

	// Declarations and const/default values for TEST_CLASS static members.
	const string WebDiagnostics::m_errorDumpFileName(_T("ErrorDump.html"));
	const string WebDiagnostics::m_errorFileNameExpected(_T("/opt/ncompass/media") _T("/ErrorFile") _T("/Error.txt"));
	const string WebDiagnostics::m_statusFileNameExpected(_T("/opt/ncompass/www/") _T("StatusDump.html"));
	const string WebDiagnostics::m_memoryFileNameExpected(_T("/opt/ncompass/www/") _T("MemoryDump.html"));
	const string WebDiagnostics::m_stackFileNameExpected(_T("/opt/ncompass/www/") _T("ErrorDump.html"));

	const ios::openmode WebDiagnostics::m_errorOpenFlagsExpected = ios::in;
	const ios::openmode WebDiagnostics::m_dumpOpenFlagsExpected = ios_base::out | ios_base::trunc;

#if 000//TODO-PORT - Use "/media" instead "mediadisk" when the Sentinel executable stops using "media" for other purposes.
	const string WebDiagnostics::m_errorFileName(_T("/opt/ncompass/media") _T("/ErrorFile") _T("/Error.txt"));
#else//TODO-PORT - Use "/media" instead "mediadisk".
	const string WebDiagnostics::m_errorFileName(_T("/opt/ncompass/mediadisk") _T("/ErrorFile") _T("/Error.txt"));
#endif//TODO-PORT - Use "/media" instead "mediadisk".

	const char WebDiagnostics::m_errorFileContents1[] = {
		"Error File Line 1\n"
	};
	const UINT WebDiagnostics::m_errorFileContentsLength1(sizeof(WebDiagnostics::m_errorFileContents1) - 1);

	const char WebDiagnostics::m_errorFileContents2[] = {
		"Error File Line 1\n"
		"Error File Line 2\n"
	};
	const UINT WebDiagnostics::m_errorFileContentsLength2(sizeof(WebDiagnostics::m_errorFileContents2) - 1);

	const char WebDiagnostics::m_errorFileContents3[] = {
		"Error File Line 1\n"
		"Error File Line 2\n"
		"Error File Line 3\n"
	};
	const UINT WebDiagnostics::m_errorFileContentsLength3(sizeof(WebDiagnostics::m_errorFileContents3) - 1);

	const char WebDiagnostics::m_errorFileContents4[] = {
		"Error File Line 1\n"
		"Error File Line 2\n"
		"Error File Line 3\n"
		"Error File Line 4\n"
	};
	const UINT WebDiagnostics::m_errorFileContentsLength4(sizeof(WebDiagnostics::m_errorFileContents4) - 1);
}
