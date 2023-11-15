#include "stdafx.h"
#include "ProcessDump/WebDiagnostics.h"
#include "Diagnostics.h"
#include "resource.h"
#include "Util/String.h"
#include "Utilities.h"
#include "Util/Path.h"

using namespace std;

static const std::string NO_PLAYER_AVAILABLE(" No Player available.");
static const std::string NO_BUCKETS_AVAILABLE(" No extended bucket countdowns available for current subgame.");
static const std::string ENABLED("Enabled");
static const std::string DISABLED("Disabled");
static const std::string CONNECTING("Connecting");
static const std::string CONNECTED("Connected");
static const std::string DISCONNECTED("Disconnected");

void CWebDiagnostics::UpdatePlayerInfo(const CWebDiagnosticsCommonInfo &commonInfo,
			const CPlayer *player,
			const CPairingInformation &pairingInfo,
			const CPlayerSession &playerSession,
			const IBeacon* beacon,
			uint8_t transmitPower
)

{
	// Get the path of the dump file
	std::string dumpFileName;
	dumpFileName = Path::AddSlash(CDirectoryManager::Instance().HtmlFilePath().c_str()) + PlayerInfoFilename;
	std::shared_ptr<std::ostream> file = m_operatingSystem.CreateFile(dumpFileName, ios_base::out | ios_base::trunc);
	SavePlayerData(file,
		commonInfo,
		player,
		pairingInfo,
		playerSession,
		beacon,
		transmitPower
	);
}

void CWebDiagnostics::SavePlayerData(std::shared_ptr<ostream> &file,
	const CWebDiagnosticsCommonInfo &commonInfo,
	const CPlayer *player,
	const CPairingInformation &pairingInfo,
	const CPlayerSession &playerSession,
	const IBeacon* beacon,
	uint8_t transmitPower
	)
{
	try
	{
		bool mobileConnected = ((player) ? player->IsMobilePlayer() : false);
		
        if ( !file.get()->fail())
		{
			std::string pageHtml;
            std::stringstream sStream;
            sStream << "<html><META Http-equiv=\"refresh\" Content=\"" << (int)PLAYER_INFO_REFRESH_SECONDS << "\">";
            pageHtml = sStream.str();
			Write(file, pageHtml);
			Write(file, GetCommonHeaderHtml(commonInfo));

			Write(file, GetPlayerInformationHtml(player, playerSession));
			Write(file, GetPairingInformationHtml(pairingInfo, mobileConnected));
			Write(file, GetBeaconInformationHtml(beacon, transmitPower, pairingInfo.GetMeasuredPower()));
			Write(file, HTML_TRAILER);
		}
	}
	catch (...)
	{
		// We do not want to throw to calling thread.
	}
}

std::string CWebDiagnostics::GetPlayerInformationHtml(const CPlayer *player, const CPlayerSession &playerSession)
{
	std::string html;
    std::stringstream SStream;

	SStream << "Player Info <br>";
	SStream << "Player Type: " << playerSession.GetPlayerType() << "<br>";
	
	if (player)
	{
		SStream << "Player OCR: " << player->GetCard()->MakeCStringCardNumber(player->GetCard()) << "<br>";
		SStream << "Player ID: " << std::to_string(player->GetPlayerInfo()->GetPlayerId()) << "<br>";
		SStream << "Player Rank ID: " << std::to_string(player->GetRankId()) << "<br>";
		SStream << "Player Rank Name: " << player->GetPlayerInfo()->GetRankName() << "<br>",
		SStream << "In Virtual Session: " << std::boolalpha << player->IsVirtual() << "<br>";
		SStream << "Is Guest: " << std::boolalpha << player->IsGuest() << "<br>";

		bool walletEnabled = player->GetPlayerInfo()->IsEnrolledInWallet_2_0();
		SStream << "Centralized Wallet Enrolled: " << std::boolalpha << walletEnabled << "<br>";
		if (walletEnabled)
		{
			std::string cashTransferStatusStr;
			switch (player->GetLeftoverCashTransferStatus())
			{
			case LeftoverCashTransferStatus::LEFTOVER_CASH_TRANSFER_NA:
				cashTransferStatusStr = "N/A";
				break;
			case LeftoverCashTransferStatus::LEFTOVER_CASH_TRANSFER_SUCCEED:
				cashTransferStatusStr = "Success";
				break;
			case LeftoverCashTransferStatus::LEFTOVER_CASH_TRANSFER_FAILED:
				cashTransferStatusStr = "Failure";
				break;
			}
			SStream << "Leftover Cash Transfer Status from other Casino during last PBT Session: " << cashTransferStatusStr.c_str() << "<br>";
		}
	}
	else
	{
		SStream << NO_PLAYER_AVAILABLE << "<br>";
	}

	SStream << "<br>Session Info <br>";
	SStream << "Session ID: " << playerSession.GetSessionId() << "<br>";

	if (player)
	{
		SStream << "Countdown: " << std::to_string(player->GetCurrentCountdown()) << "<br>";
		SStream << "Session Points: " << std::to_string(player->GetSessionAndBonusPoints()) << "<br>";
		SStream << "Total Points: " << std::to_string(player->GetTotalPoints()) << "<br>";
		
		std::vector<std::unique_ptr<BucketDisplayData>> displayData;
		DWORD gameNumber = 0;
		int denom = 0;
		player->GetBucketsDisplayData(displayData, gameNumber, denom);

		if (displayData.size())
		{
			SStream << "Extended bucket countdown details for current subgame (Game Number: " << gameNumber << ", Denom: " << denom << "): <br>";
			for (auto &displayDataItr : displayData)
			{
				SStream << "Bucket ID: " << std::to_string(displayDataItr->id);
				SStream << ", Bucket Name: " << displayDataItr->bucketName;
				SStream << ", Display Name: " << displayDataItr->displayName;
				SStream << ", Currency: " << displayDataItr->currency;
				SStream << ", Countdown: " << std::to_string(displayDataItr->countdown);
				SStream << ", Session Points: " << std::to_string(displayDataItr->sessionPoints) << "<br>";
			}
		}
		else
		{
			SStream << NO_BUCKETS_AVAILABLE << "<br>";
		}
	}

	SStream << "<br>";
    html = SStream.str();
	return html;
}


std::string CWebDiagnostics::GetPairingInformationHtml(const CPairingInformation &pairingInfo, bool mobileConnected)
{
	std::string html;
    std::stringstream SStream;
	bool pairingEnabled = pairingInfo.IsPairingEnabled();
	string state = ((pairingEnabled) ? ENABLED : DISABLED); 

	if (pairingInfo.IsWaitingForPairing() || pairingInfo.IsWaitingForRequestCardChange())
		state = CONNECTING;
	else if (mobileConnected)
		state = CONNECTED;
		
    SStream << "Mobile State: " << state << "<br>";
	SStream << "Mobile Pairing Type: " << (pairingEnabled ? pairingInfo.GetPairingType(): LPCTSTR_NA) << "<br>",
	SStream << "Mobile Pairing Timeout(ms): " << std::to_string(pairingInfo.GetPairingTimeOutMs()) << "<br>";

	SStream << "<br>";
    html = SStream.str();
	return html;
}

std::string CWebDiagnostics::GetBeaconInformationHtml(const IBeacon *beacon, uint8_t transmitPower, uint8_t measuredPower)
{
	std::string html;
    std::stringstream SStream;
	
	SStream << "Beacon Info" << "<br>";
	if ((nullptr != beacon) && beacon->IsConnected())
    	SStream << "Beacon State: " << CONNECTED << "<br>";
	else
		SStream << "Beacon State: " << DISCONNECTED << "<br>";	
	SStream << FormatString(_T("Transmit Power:  %u<br>"), transmitPower);
	SStream << FormatString(_T("Measured Power:  %u<br>"), measuredPower);

	SStream << "<br>";
    html = SStream.str();
	return html;
}