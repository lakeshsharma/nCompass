#include "stdafx.h"
#include "SASProtocol.h"

// Sentinel includes.
#include "Logging/LogString.h"
#include "Hardware/WatchDog.h"
#include "SASPollMessage.h"
#include "LongPoll01Message.h"
#include "LongPoll02Message.h"
#include "LongPoll09Message.h"
#include "LongPoll19Message.h"
#include "LongPoll1AMessage.h"
#include "LongPoll1EMessage.h"
#include "LongPoll1FMessage.h"
#include "LongPoll28Message.h"
#include "LongPoll2FMessage.h"
#include "LongPoll20Message.h"
#include "LongPoll31Message.h"
#include "LongPoll39Message.h"
#include "LongPoll53Message.h"
#include "LongPoll54Message.h"
#include "LongPoll55Message.h"
#include "LongPoll56Message.h"
#include "LongPoll51Message.h"
#include "LongPoll6AMessage.h"
#include "LongPoll83Message.h"
#include "LongPoll94Message.h"
#include "LongPollA0Message.h"
#include "LongPollA8Message.h"
#include "LongPollB1Message.h"
#include "LongPollB2Message.h"
#include "LongPollB056Message.h"
#include "SASGeneralPoll.h"
#include "../QueueList.h"
#include "../Meters.h"
#include "../Games.h"
#include "../Game.h"
#include "../Sentinel.h"
#include "LongPoll6FMessage.h"
#include "LongPollB6Message.h"
#include "LongPollB5Message.h"
#include "LongPollB0B5Message.h"

const int B5RETRIES = 5;

void CSASProtocol::EstablishEGMCommunications()
{
	LogString(SASGENERAL, "Begin EstablishEGMCommunications");
	try
	{
		

		m_sasPollQueue->SetQueueStarvation(QUEUE_PRIORITY_HIGHEST);

		// Find the EGM address. This will also tell us if we have communications.
		bool bHaveCommunications = FindEGM();
		
		if ( !bHaveCommunications )
		{
			// Tell internals to clear consecutive game comm errors.
			CInternalMessage *msg = m_sas->SetGameComStatus(false);
			SendMessageToInternals(msg,QUEUE_PRIORITY_NORMAL);

			// Loop until we have established communications.
			while ( !bHaveCommunications && !IsTimeToClose() )
			{
				CWatchDog::Instance().Checkin();
				CUtilities::Sleep( 1000 );
				bHaveCommunications = FindEGM();
			}
		}

		if( bHaveCommunications )
		{
			// Tell Internals Game comm up and clear game comm errors
			CInternalMessage *msg = m_sas->SetGameComStatus(true);
			SendMessageToInternals(msg,QUEUE_PRIORITY_NORMAL);

			// Send 0 error count to internals
			msg = new CInternalMessage( INT_MSG_GAME_COMM_ERROR, (WORD)0 ); 
			SendMessageToInternals( msg, QUEUE_PRIORITY_NORMAL );

			CWatchDog::Instance().Checkin();

			GetSASVersion();
			CWatchDog::Instance().Checkin();

			GetEGMCapabilities();
			CWatchDog::Instance().Checkin();

			// Send a copy of m_sasInfo ( CGames with no game or meters ) to internals to get merged into main games object
			// Use high priority to insure it gets to internals before games list.
			CGames *sasInfo = new CGames(*m_sasInfo);
			CInternalMessage *sasInfoMsg = new CInternalMessage(INT_MSG_UPDATE_SAS_INFO, sasInfo);
			SendMessageToInternals(sasInfoMsg, QUEUE_PRIORITY_HIGH);

			EnableDisableEGM();
			CWatchDog::Instance().Checkin();

			DisableRealTimeEvents();
			CWatchDog::Instance().Checkin();

			SetGameTime();
			CWatchDog::Instance().Checkin();

			LogString(GAMES, "EstablishEGMCommunications");
			GetGameList(false, false);

			// offline play check is active
			CInternalMessage* offlineMsg = new CInternalMessage(INT_MSG_FINISHED_OFFLINE_PASS, (WORD)false);
			SendMessageToInternals(offlineMsg, QUEUE_PRIORITY_NORMAL);

			// Speed up initial sub game collection so ProgressiveProtocol can get on with its business.
			m_gameList.SetFirstPass();

			CWatchDog::Instance().Checkin();

			m_sasPollQueue->ClearQueueStarvation();
		}

	}
	catch (...)
	{
	}

}

void CSASProtocol::GetSASVersion()
{
	// Get SAS version
	try
	{
		CLongPoll54Message *lp54 = new CLongPoll54Message(m_gameAddress);
		lp54->Fill();
		m_sasPollQueue->Add(lp54,QUEUE_PRIORITY_HIGHEST);
		lp54->WaitForResponse();

		if (lp54->GetReturnValue() == SASPOLL_SUCCESS && *lp54->GetResponseBuffer() == m_gameAddress)
		{
			// received valid response
			// report sas version to internals
			m_sas->SetVersion(lp54->version);
			m_sasInfo->SetSerialNumber(lp54->GetSerialNumber());

			if (m_sas->GetMajorVersion() == SASVERSION5)
			{
				// SAS version 5 something, cause 6F's to not happen
				CMeterStepdown *meterStepdown = &m_mainGameMeterStepdown;
				meterStepdown->DeactivateForSAS5x();
			}
		}
		else
		{
			// report sas version to internals
			m_sas->SetVersion(nullptr);
		}

		// clean up long poll 54
		delete lp54;

		m_sasInfo->SetSASVersion(m_sas->GetVersionString());
	}
	catch(...)	
	{
	}
}

bool CSASProtocol::FindEGM()
{
	// poll at machine address 0x01.
	byte gameAddress = 1;
	bool foundGame = false;
	m_sas->SetFindingEGMStatus(true); // prevents sas polls from retrying.
	
	// reuse poll message
	CLongPoll19Message *lp19;
	CLongPoll20Message *lp20;

	// Send general poll to address 0 (80), don't wait for response
	CSASGeneralPoll *gp = new CSASGeneralPoll(0x00);
	gp->m_bWaitForSASResponse = false;
	gp->Fill();
	m_sasPollQueue->Add(gp,QUEUE_PRIORITY_HIGHEST);
	gp->WaitForResponse();
	delete gp;

	// initialize buffer's
		
	// set up long poll sendData
	lp19 = new CLongPoll19Message(gameAddress, NULL);
	lp19->Fill();
	m_sasPollQueue->Add(lp19,QUEUE_PRIORITY_HIGHEST);

	// blocks until SAS response is received
	lp19->WaitForResponse();

	// valid data returned?
	if (lp19->GetReturnValue() == SASPOLL_SUCCESS && lp19->GetResponseBuffer()[0] == (byte)m_gameAddress)
	{
		foundGame = true;
	}
	else
	{
		// no response, try poll 20
		//
		lp20 = new CLongPoll20Message(gameAddress);
		lp20->Fill();
		m_sasPollQueue->Add(lp20,QUEUE_PRIORITY_HIGHEST);

		// blocks until SAS response is received
		lp20->WaitForResponse();

		// valid data returned?
		if (lp20->GetReturnValue() == SASPOLL_SUCCESS && lp20->GetResponseBuffer()[0] == (BYTE)m_gameAddress)
		{
			foundGame = true;
		}
		delete lp20;
	}
	delete lp19;
		
	m_sas->SetFindingEGMStatus(false);
	
	return foundGame;
}

void CSASProtocol::GetEGMCapabilities()
{
	// Check to see if Long Poll AF is supported
	m_sas->SetAfSupported(false);
	if (m_sasState.GetEGMConfig().GetEGMProtocol() != IGT_SAS_3xx)
	{
		CMeters *meters = new CMeters(MEMORY_NONE);
		meters->SetMeter(mCrd);
		CLongPoll6FMessage lpAf(m_gameAddress, meters);
		lpAf.SetGameNumber(EGM_GAME_NUMBER);
		lpAf.Fill();
		lpAf.SwapPollValue();
		m_sasPollQueue->Add(&lpAf, QUEUE_PRIORITY_HIGHEST);
		lpAf.WaitForResponse();
		if (lpAf.GetReturnValue() == SASPOLL_SUCCESS)
		{
			m_sas->SetAfSupported(true);
		}

		delete meters;
	}

	GetAFTAndBonusCapabilities(m_sasInfo);

	if (!m_bIsAFTSupported) 
	{
		// Tracker #21312: only proceed if pbt is enabled
		LogString(PBT_LOG, "Getting PBT capabilities, AFT not supported, m_PBTEnabled = %d", m_PBTEnabled);

		// use LP28 to get last EFT transaction # used
		CLongPoll28Message lp28(m_gameAddress);
		lp28.Fill();
		m_sasPollQueue->Add(&lp28, QUEUE_PRIORITY_HIGHEST);
		lp28.WaitForResponse();
		int lp28Response = lp28.GetReturnValue();
		LogString(PBT_LOG, "Long poll 28 response = %d ", lp28Response);
		if (lp28Response == SASPOLL_SUCCESS)
		{
			m_bIsEFTSupported = true;
			m_bEFTCashFromGame = true;
			m_bEFTCashToGame = true;

			m_sasInfo->SetEftTransferFromEnabled(m_bEFTCashFromGame);
			m_sasInfo->SetEftTransferToEnabled(m_bEFTCashToGame);

			CInternalMessage *eftMsg = new CInternalMessage(INT_MSG_EFT_SEED, (WORD)lp28.GetEFTSeed());
			SendMessageToInternals(eftMsg, QUEUE_PRIORITY_NORMAL);

			// use LP6A to confirm EFT support for ncompass webpage display purpose
			CLongPoll6AMessage lp6A(m_gameAddress);
			lp6A.Fill();
			m_sasPollQueue->Add(&lp6A, QUEUE_PRIORITY_HIGHEST);
			lp6A.WaitForResponse();
			int lp6AResponse = lp6A.GetReturnValue();
			LogString(PBT_LOG, "Long poll 6A response = %d ", lp6AResponse);
			if (lp6AResponse == SASPOLL_SUCCESS)
			{
				LogString(PBT_LOG, "Setting EGM transfer mode to EFT");
				m_egmTransferMode = EGMTransferMode_EFT;
			}
			else  // LP6A failure, so keep the EGM transfer mode as it is
			{
				if (m_egmTransferMode == EGMTransferMode_PENDING)
				{
					LogString(PBT_LOG, "By default, EGM transfer mode is PENDING. It will be resolved when PBT happens.");
				}
			}
		}
		else  // LP28 failure
		{
			LogString(PBT_LOG, "Setting EGM transfer mode to NONE");
			m_egmTransferMode = EGMTransferMode_NONE;
		}
	}
	else  // AFT supported
	{
		LogString(PBT_LOG, "Setting EGM transfer mode to AFT");
		m_egmTransferMode = EGMTransferMode_AFT;

		// AFT supported, check game's Asset #
		CLongPoll74Message lp74(m_gameAddress);
		lp74.SetLockCode(LOCK_CODE_QUERY_STATUS);
		// Tracker #018406
		lp74.SetLockTimeout(0);
		lp74.Fill();
		m_sasPollQueue->Add(&lp74, QUEUE_PRIORITY_HIGHEST);
		lp74.WaitForResponse();

		if (lp74.GetReturnValue() == SASPOLL_SUCCESS)
		{
			m_sasState.SetCashoutToHostEnabledOnEGM(lp74.ResponseCashoutToHostEnabled());

			SendMessageToInternals(new CInternalMessage(INT_MSG_CASHOUT_TO_HOST_STATUS,
				(WORD)lp74.m_hostCashoutStatus), QUEUE_PRIORITY_NORMAL);

			m_assetNumber = lp74.m_assetNumber;

			LogString(SASGENERAL, "[%s] CashoutToHostEnabledOnEgm %d HostCashoutStatus: 0x%x "
				"AssetNumber: %d", __FUNCTION__, m_sasState.GetCashoutToHostEnabledOnEGM(), 
				lp74.m_hostCashoutStatus, m_assetNumber);

			if (lp74.m_assetNumber == 0)
			{
				// Send tilt
				SendTiltToInternals(tltGameAssetZero);
			}
		}
	}

	unsigned char sasVersion = m_sas->GetMajorVersion();
	if (sasVersion < SASVERSION5)
	{
		// for SAS version 3 & 4, PBT is not supported. So override transfer mode to NONE.
		LogString(PBT_LOG, "SAS version = %d (< %d), PBT not supported. Override EGM transfer mode to NONE", sasVersion, SASVERSION5);
		m_egmTransferMode = EGMTransferMode_NONE;
	}

	// set EGM transfer mode to CGames instance so that it can be reflected to ncompass webpage via timed events.
	m_sasInfo->SetEGMTransferMode(m_egmTransferMode);

	GetGameDenomInfo();
	m_billMeterSuccess = CheckForBillMeterPolls();
}

void CSASProtocol::GetAFTAndBonusCapabilities(CGames *sasInfo)
{
	if (sasInfo != NULL)
	{
		CLongPollA0Message *lpa0 = new CLongPollA0Message(m_gameAddress);
		lpa0->SetGameNumber(EGM_GAME_NUMBER);
		lpa0->Fill();

		m_sasPollQueue->Add(lpa0, QUEUE_PRIORITY_HIGHEST);

		// blocks until SAS response is received
		lpa0->WaitForResponse();

		// Tracker ?, need to verify support for multidenom extensions ie long poll b0 56
		if (lpa0->GetReturnValue() == SASPOLL_SUCCESS)
		{
			m_sasState.SetSupportMultiDenomExtensions(lpa0->m_bMultiDenomExtensions);
		}
		else
		{
			m_sasState.SetSupportMultiDenomExtensions(false);
		}
		sasInfo->SetMultiDenomEnabled(m_sasState.GetSupportMultiDenomExtensions());

		// Set enhanced ticketing flag
		if (lpa0->m_cValidationStyle == 2)
			m_sasState.SetIsEnhancedTicketing(true);

		// Set validation extenstions flag
		if (lpa0->m_bValidationExtensions)
			m_sasState.SetValidationExtensions(true);
		else
			m_sasState.SetValidationExtensions(false);
		sasInfo->SetValidationExtensionsEnabled(m_sasState.GetValidationExtensions());

		if (lpa0->GetReturnValue() == SASPOLL_SUCCESS)
		{
			// Set AFT flag.
			m_bIsAFTSupported = lpa0->m_bAFT;
		}

		// Tracker #21944
		if (m_sas->GetMajorVersion() <= SASVERSION5)
			m_bIsAFTSupported = false;

		sasInfo->SetAftEnabled(m_bIsAFTSupported);
		if (lpa0->GetReturnValue() == SASPOLL_SUCCESS)
		{
			sasInfo->SetLegacyBonusEnabled(lpa0->m_bLegacyBonusAwards);
			sasInfo->SetAftBonusEnabled(lpa0->m_bAFTBonusAwards);
			sasInfo->SetExtendedMetersEnabled(lpa0->m_bExtendedMeters);
		}
		else
		{
			sasInfo->SetLegacyBonusEnabled(false);
			sasInfo->SetAftBonusEnabled(false);
			sasInfo->SetExtendedMetersEnabled(false);
		}
		delete lpa0;
	}
}

void CSASProtocol::GetGameDenomInfo()
{
	// issue 1f long poll
	CLongPoll1FMessage *lp1f = new CLongPoll1FMessage(m_gameAddress);
	lp1f->Fill();
	m_sasPollQueue->Add(lp1f,QUEUE_PRIORITY_HIGHEST);
	lp1f->WaitForResponse();

	WORD denomMult = INVALID_DENOM;
	if ( lp1f->GetReturnValue() == SASPOLL_SUCCESS )
	{
		SetDenomMult((WORD)denomCodeMap[lp1f->m_denomination].multiplier);
		denomMult = m_denomMult;
		m_sasInfo->SetAccountingDenom(m_denomMult);
		m_sasInfo->SetGameId(lp1f->m_gameID);
		m_sasInfo->SetProgressiveGroup(lp1f->m_progressiveGroup);
	}
	// For m_sasInfo, keep the previous accounting denom and game ID if the 1f poll was not successful.

	// Always send the denom message. Value will be 0 if poll not successful.
	CInternalMessage *msg = new CInternalMessage( INT_MSG_SET_DENOM, denomMult );
	SendMessageToInternals( msg, QUEUE_PRIORITY_NORMAL );

	delete lp1f;
}

void CSASProtocol::SendSubGameMeters()
{
	// Get the next game number we have to get meters for
	bool sendInfo(false);
	WORD gameNumber = m_gameList.GetNextGame(sendInfo);
	if (gameNumber != EGM_GAME_NUMBER)
	{
		if (!sendInfo && !m_gameList.IsEnabledGameCountsReached() && m_denomGames->GetGame(gameNumber) == nullptr)
		{
			sendInfo = true;
		}
		SendSubGameMeters(gameNumber, sendInfo, false);
	}

	if (m_gameList.IsFirstPass())
	{
		const WORD NumberOfAddtionalSubgamesToSend(4);
		WORD numberOfSubgamesSent(0);
		do
		{
			gameNumber = m_gameList.GetNextGame(sendInfo);
			if (gameNumber != EGM_GAME_NUMBER)
			{
				if (!sendInfo && !m_gameList.IsEnabledGameCountsReached() && m_denomGames->GetGame(gameNumber) == nullptr)
				{
					sendInfo = true;
				}
				LogString(GAMES, "SendSubGameMeters gameNumber=%u sendInfo=%u", gameNumber, sendInfo);

				SendSubGameMeters(gameNumber, sendInfo, false);
			}
		}  while ((m_gameList.IsFirstPass()) && (++numberOfSubgamesSent < NumberOfAddtionalSubgamesToSend));
	}
}

void CSASProtocol::SendSubGameMeters( WORD gameNumber, bool sendInfo, bool bMeterSnapshot )
{
	// Do we have to send the info for this game first?
	if (sendInfo)
	{
		LogString(GAMES, "SendSubGameMeters-SendSubGameInfo gameNumber=%u", gameNumber);
		SendSubGameInfo(gameNumber, !bMeterSnapshot);
	}

	CMeters *meters = new CMeters(MEMORY_NONE);

	// Add the meters we need.
	meters->SetMeter( SUBGAME_METER_COIN_IN );			// Coin in
	meters->SetMeter( SUBGAME_METER_GAMES_PLAYED );		// Games played
	meters->SetMeter( SUBGAME_METER_GAMES_WON );		// Games won
	meters->SetMeter( SUBGAME_METER_COIN_OUT );			// Coin Out
	meters->SetMeter( SUBGAME_METER_MACH_PAID_PROG );	// Machine Pay Progressive payout
	meters->SetMeter( SUBGAME_METER_MACH_PAID_EXT_BONUS );	// External Bonusing machine payout
	meters->SetMeter( SUBGAME_METER_ATT_PAID_JACKPOT );	// Jackpot
	meters->SetMeter( SUBGAME_METER_ATT_PAID_PROG );	// Attendant Pay Progressive payout
	meters->SetMeter( SUBGAME_METER_ATT_PAID_EXT_BONUS );	// External Bonusing Attendant payout
	meters->SetMeter( SUBGAME_METER_WTPP );				// Weighted Theoretical Payback Percentage

	GetMeters( *meters, gameNumber, bMeterSnapshot, !bMeterSnapshot ); // if meter snapshot, then tryOnce parameter needs to be false.  True otherwise.

	CMeter* coinInMeter(meters->GetMeterByNumber(SUBGAME_METER_COIN_IN));
	if (coinInMeter && coinInMeter->GetValue() > 0 && !m_gameList.IsGameEnabled(gameNumber))
	{
		m_gameList.AddPreviouslyEnabledGame(gameNumber);
	}

	SendMessageToInternals(new CInternalMessage(INT_MSG_SET_METERS, meters, gameNumber, bMeterSnapshot), bMeterSnapshot ? QUEUE_PRIORITY_HIGHEST : QUEUE_PRIORITY_NORMAL);

	// Check if firstPass and just retrieved last enabled game
	if (m_gameList.IsEnabledGameCountsReached() && !m_gameList.IsFirstPassEnabledGameListSent())
	{
		// Tell internals new bonus capabilities
		CGames *newSASInfo = new CGames();
		GetAFTAndBonusCapabilities(newSASInfo);

		// Copy new values to m_sasInfo
		m_sasInfo->SetAftBonusEnabled(newSASInfo->GetAftBonusEnabled());
		m_sasInfo->SetLegacyBonusEnabled(newSASInfo->GetLegacyBonusEnabled());

		CInternalMessage *msg = new CInternalMessage(INT_MSG_UPDATE_BONUS_TYPES, newSASInfo);
		SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);

		// Tell internals first pass through subgames is complete
		// also need totCoinOutCredits 
		CMeters *coMeters = new CMeters(MEMORY_NONE);
		coMeters->SetMeter(gCO);
		GetMeters(*coMeters, EGM_GAME_NUMBER, false, true);
		LogString(GAMES, "SendSubGameMeters-INT_MSG_FINISHED_GAMESLIST_FIRSTPASS gameNumber=%u", gameNumber);

		msg = new CInternalMessage(INT_MSG_FINISHED_GAMESLIST_FIRSTPASS);

		msg->SetData(coMeters, EGM_GAME_NUMBER);
		msg->SetData((WORD)m_gameList.IsError());
		CGames *copyDenomGames = new CGames(*m_denomGames);
		msg->SetData(copyDenomGames);
		SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);

		SendCurrentSelectedGameToInternals(false);

		m_gameList.SetFirstPassEnabledGameListSentFlag();
	}

	if (m_gameList.IsLastEnabledGameOnFirstPass())
	{
		// offline play check is completed
		SendMessageToInternals(new CInternalMessage(INT_MSG_FINISHED_OFFLINE_PASS, (WORD)true), QUEUE_PRIORITY_NORMAL);
	}
}

// Get the current selected game number at HIGHEST priority, because this can be called from GetGameList which can be called from 
// EstablishEGMCommunications which starves at HIGHEST
bool CSASProtocol::GetSelectedGameNumber(int &gameNumber)
{
	bool isValid = false;
	gameNumber = EGM_GAME_NUMBER;

	CLongPoll55Message *lp55 = new CLongPoll55Message(m_gameAddress);
	lp55->Fill();
	m_sasPollQueue->Add(lp55,QUEUE_PRIORITY_HIGHEST);
	lp55->WaitForResponse();
	if (lp55->GetReturnValue() == SASPOLL_SUCCESS)
	{
		gameNumber = lp55->m_selectedGameNumber;
		isValid = true;
	}
	
	delete lp55;

	return isValid;
}

void CSASProtocol::SetGame(int gameNumber, byte enable)
{
	// remove this stuff
	CLongPoll09Message *lp09 = new CLongPoll09Message(m_gameAddress);
	lp09->SetGameNumber(gameNumber);
	lp09->Fill(enable);
	m_sasPollQueue->Add(lp09,QUEUE_PRIORITY_HIGHEST);
	lp09->WaitForResponse();
	delete lp09;

}

bool CSASProtocol::GetCurrentDenom(byte &denomCode)
{
	bool isValid = false;

	// get current selected denom POLL B1
	CLongPollB1Message *lpb1 = new CLongPollB1Message(m_gameAddress);
	lpb1->Fill();
	m_sasPollQueue->Add(lpb1, QUEUE_PRIORITY_HIGHEST);
	lpb1->WaitForResponse();
	if (lpb1->GetReturnValue() == SASPOLL_SUCCESS)
	{
		denomCode = lpb1->m_currentDenom;
		isValid = true;
	}
	delete lpb1;

	return isValid;
}	

void CSASProtocol::GetGameList(bool gpeOptionsChanged, bool scheduledTask)
{
	if ((!m_bInSelfTest && !m_bInUserMenu) || gpeOptionsChanged)
	{
		LogString(GAMES, "Getting Game List gpeOptionsChanged=%u scheduledTask=%u", gpeOptionsChanged, scheduledTask);
		vector<DWORD>* vEnabledGameNumbers(new vector<DWORD>());
		// Retry counts
		const int TOTALRETRIES = 2;
		int currentRetry = 0;
		int returnVal = SASPOLL_NACK;

		m_gameList.Clear();
		CGames *games = new CGames(MEMORY_NONE);
		if (m_denomGames != nullptr)
		{
			delete m_denomGames;
		}
		m_denomGames = new CGames(MEMORY_NONE);
		CGame *game = nullptr;

		// issue 1f long poll/with retry
		while (returnVal != SASPOLL_SUCCESS && currentRetry++ < TOTALRETRIES)
		{
			CLongPoll1FMessage *lp1f = new CLongPoll1FMessage(m_gameAddress);
			lp1f->Fill();
			m_sasPollQueue->Add(lp1f, QUEUE_PRIORITY_HIGHEST);
			lp1f->WaitForResponse();
			returnVal = lp1f->GetReturnValue();
			if (returnVal == SASPOLL_SUCCESS)
			{
				memcpy(m_gameID, lp1f->m_gameID, 2);
				m_gameID[2] = 0;

				// put game config into games
				game = new CGame(MEMORY_NONE, true);
				game->SetGameNumber(EGM_GAME_NUMBER);
				games->AddGame(game);
				game->SetProgressiveGroup(lp1f->m_progressiveGroup);
				game->SetDenom(denomCodeMap[lp1f->m_denomination].multiplier, US_DOLLARS);
				game->SetMfgID(lp1f->m_gameID);
				game->Enable();
				vEnabledGameNumbers->push_back(EGM_GAME_NUMBER);
				game->SetAdditionalID(lp1f->m_additionalID);
				game->SetTheoreticalPayback(lp1f->m_basePercent);

				game->SetPaytableIDLength(strlen(lp1f->m_payTable));
				game->SetPaytableID(lp1f->m_payTable);
				game->SetGamenameLength(game->GetDefaultGameName().size());
				game->SetGameName(game->GetDefaultGameName().c_str());
				game->SetInfoFetchedFlag(true);
				
				GetInfoFromLongPollB5(*game);

				SetDenomMult(denomCodeMap[lp1f->m_denomination].multiplier);

				CInternalMessage* msg = new CInternalMessage(INT_MSG_SET_DENOM, (WORD)denomCodeMap[lp1f->m_denomination].multiplier);
				SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);
			}

			delete lp1f;
		}


		// get implemented game count
		WORD implementedGames = 0;
		if (returnVal == SASPOLL_SUCCESS)
		{
			LogString(GAMES, "LP_1F SASPOLL_SUCCESS");
			CLongPoll51Message *lp51 = new CLongPoll51Message(m_gameAddress);
			const WORD MAX_NUMBER_LP51_ATTEMPTS(3);
			WORD numberOfLP51Attempts(1);
			int lp51Response(SASPOLL_NACK);
			do
			{
				LogString(GAMES, "LP_51");
				lp51->Fill();
				m_sasPollQueue->Add(lp51, QUEUE_PRIORITY_HIGHEST);
				lp51->WaitForResponse();
				lp51Response = lp51->GetReturnValue();
				if (lp51Response != SASPOLL_SUCCESS)
				{					 
					LogString(GAMES, "LP_51 SASPOLL_NACK numberOfLP51Attempts=%u", numberOfLP51Attempts);
					CLongPoll1AMessage *lp1A = new CLongPoll1AMessage(m_gameAddress, nullptr);
					lp1A->Fill();
					m_sasPollQueue->Add(lp1A, QUEUE_PRIORITY_HIGHEST);
					lp1A->WaitForResponse();
					if (lp1A->GetReturnValue() != SASPOLL_SUCCESS)
					{
						LogString(GAMES, "LP_51-SASPOLL_NACK SASPOLL_NACK LP_1A-SASPOLL_NACK numberOfLP51Attempts=%u", numberOfLP51Attempts);
						numberOfLP51Attempts = MAX_NUMBER_LP51_ATTEMPTS;
					}
					delete lp1A;					
					CUtilities::Sleep(SLEEP_YIELD_MILLISECONDS);
					CWatchDog::Instance().Checkin();
					
					if (numberOfLP51Attempts < MAX_NUMBER_LP51_ATTEMPTS)
					{
						lp51->ResetResponse();
					}
				}
			} while (lp51Response != SASPOLL_SUCCESS && ++numberOfLP51Attempts <= MAX_NUMBER_LP51_ATTEMPTS);

			if (lp51->GetReturnValue() == SASPOLL_SUCCESS)
			{
				LogString(GAMES, "LP_51 SASPOLL_SUCCESS");
				implementedGames = (WORD)lp51->m_numberSubGamesImplemented;
				LogString(GAMES, "SAS reports %d implemented subgames", implementedGames);

				// get list of enabled denoms
				byte *enabledDenomList = new byte[NUM_DENOM_CODE_MAPPINGS]; // enough space for 27 denoms?
				CLongPollB2Message *lpb2 = new CLongPollB2Message(m_gameAddress);
				lpb2->SetEnabledDenomList(enabledDenomList, NUM_DENOM_CODE_MAPPINGS);
				lpb2->Fill();
				m_sasPollQueue->Add(lpb2, QUEUE_PRIORITY_HIGHEST);
				lpb2->WaitForResponse();
				if (lpb2->GetReturnValue() == SASPOLL_SUCCESS)
				{
					LogString(GAMES, "LP_B2 SASPOLL_SUCCESS");
					// SAS 5 egm's won't get here because b2 and b0 polls not supported
					byte enabledDenoms = lpb2->m_numberDenoms;
					m_gameList.SetEnabledDenomCount(enabledDenoms);
					if (1 == enabledDenoms)
					{
						m_gameList.SetCurrentDenomCode(true, false, *enabledDenomList);
					}

					WORD *denomGameList = new WORD[implementedGames];

					if (m_sasState.GetSupportMultiDenomExtensions()) // Tracker # 19334
					{
						LogString(GAMES, "Multi Denom Extensions Supported enabledDenoms=%u", enabledDenoms);
						int b5RetryCount = 0;

						CLongPollB056Message *lp56;
						// for each enabled denom, do b0 poll
						for (byte i = 0; i < enabledDenoms; i++)
						{
							lp56 = new CLongPollB056Message(m_gameAddress);
							lp56->SetDenom(enabledDenomList[i]);
							lp56->SetEnabledGamesList(denomGameList);
							lp56->SetImplementedGames(implementedGames);
							lp56->Fill();
							m_sasPollQueue->Add(lp56, QUEUE_PRIORITY_HIGHEST);
							lp56->WaitForResponse();

							// for each enabled game, set game enabled
							if (lp56->GetReturnValue() == SASPOLL_SUCCESS)
							{
								LogString(GAMES, "LP_B056 SASPOLL_SUCCESS");
								for (DWORD x = 0; x < lp56->m_enabledGames; x++)
								{

									WORD gameNumber = denomGameList[x];
									m_gameList.SetGameEnabled(gameNumber);
									CGame *denomGame = new CGame(MEMORY_NONE, false);
									denomGame->SetDenomInfo(enabledDenomList[i],(WORD)denomCodeMap[enabledDenomList[i]].multiplier);
									denomGame->SetGameNumber(gameNumber);
									denomGame->Enable();
									m_denomGames->AddGame(denomGame);
									char* paytableName = nullptr;
									WORD paytableLength = 0;
									char* gameName = nullptr;
									WORD gameLength = 0;
									if (m_sasState.GetEGMConfig().GetEGMType() != CUSTOM)
									{
										// Attempt to get progressive level info
										CLongPollB0B5Message* lpB5 = new CLongPollB0B5Message(m_gameAddress, gameNumber);
										lpB5->SetDenom(denomGame->GetDenomCode());
										lpB5->Fill();
										m_sasPollQueue->Add(lpB5, QUEUE_PRIORITY_HIGHEST);
										lpB5->WaitForResponse();

										if (lpB5->GetReturnValue() != SASPOLL_SUCCESS && b5RetryCount < B5RETRIES)
										{
											// Take one more shot at getting this game
											delete lpB5;
											LogString(GAMES, "LP B0B5 failure, retrying. B5 retry count %d", b5RetryCount);
											lpB5 = new CLongPollB0B5Message(m_gameAddress, gameNumber);
											lpB5->SetDenom(denomGame->GetDenomCode());
											lpB5->Fill();
											m_sasPollQueue->Add(lpB5, QUEUE_PRIORITY_HIGHEST);
											lpB5->WaitForResponse();
											b5RetryCount++;
										}

										if (lpB5->GetReturnValue() != SASPOLL_NACK)
										{
											LogString(GAMES, "LP B0B5 lpB5->GetReturnValue() != SASPOLL_NACK");
											// Have successful B5, reset b5RetryCount
											b5RetryCount = 0;

											denomGame->SetMaxBet(lpB5->GetMaxBet());

											// Paytable ID
											paytableLength = lpB5->GetPaytableName(&paytableName);
											if (paytableLength > 0)
											{
												denomGame->SetPaytableIDLength(paytableLength);
												denomGame->SetPaytableID(paytableName);
												delete[] paytableName;
											}

											// Game Name
											gameLength = lpB5->GetGameName(&gameName);
											if (gameLength > 0)
											{
												denomGame->SetGamenameLength(gameLength);
												denomGame->SetGameName(gameName);
												delete[] gameName;
											}

											// Progressive Group
											denomGame->SetProgressiveGroup(lpB5->GetProgressiveGroup());

											// Progressive Level IDs
											denomGame->SetLevelIds(lpB5->GetLevelIds());
											delete lpB5;
										}
										else
										{
											SendTiltToInternals(tltSubgameCaptureFailed);
										}

									}
									else
									{
										LogString(SASGENERAL, "Skipping progressive level ID query using long poll B0B5 due to CUSTOM EGM Type.");
									}

									// if B0B5 game name length or paytable length is 0, then need to fallback to B5 poll to get paytable and
									// use mfgID to create gamename. 
									if (paytableLength == 0 || gameLength == 0)
									{
										LogString(SASGENERAL, "Incomplete name or paytable from B0B5 or skipped B0B5, issue B5.");

										// Do "other" poll to allow ATI game to properly handle plain B5 poll
										CMeters lp1AMeters(MEMORY_NONE);
										CLongPoll1AMessage lp1A(m_gameAddress, &lp1AMeters);
										lp1A.Fill();
										m_sasPollQueue->Add(&lp1A, QUEUE_PRIORITY_HIGHEST);
										lp1A.WaitForResponse();

										CLongPollB5Message* lpB5Redo = new CLongPollB5Message(m_gameAddress);
										lpB5Redo->SetGameNumber(gameNumber);
										lpB5Redo->Fill();
										m_sasPollQueue->Add(lpB5Redo, QUEUE_PRIORITY_HIGHEST);
										lpB5Redo->WaitForResponse();
										if (lpB5Redo->GetReturnValue() == SASPOLL_SUCCESS)
										{
											paytableLength = lpB5Redo->GetPaytableName(&paytableName);
											if (paytableLength > 0)
											{
												denomGame->SetPaytableIDLength(paytableLength);
												denomGame->SetPaytableID(paytableName);
												delete[] paytableName;
											}

											gameLength = lpB5Redo->GetGameName(&gameName);
											if (gameLength > 0)
											{
												denomGame->SetGamenameLength(gameLength);
												denomGame->SetGameName(gameName);
												delete[] gameName;
											}
										}

										delete lpB5Redo;
									}

									if (vEnabledGameNumbers->end() == std::find(vEnabledGameNumbers->begin(), vEnabledGameNumbers->end(), gameNumber))
									{
										LogString(GAMES, "LP_B056 SetGameEnabled(%u)", gameNumber);
										vEnabledGameNumbers->push_back(gameNumber);
									}
								}
							}
							else
							{
								LogString(GAMES, "LP_B056 SASPOLL_NACK");
								// lp56 failed, set returnVal error
								returnVal = SASPOLL_NACK;
							}
							delete lp56;
						}

						m_gameList.ClearFirstPassEnabledGameListSentFlag();
						m_gameList.ClearError();
						m_gameList.SetFirstPass();
					}
					else
					{
						LogString(GAMES, "Multi Denom Extensions Not Supported");
						CLongPoll56Message *lp56 = new CLongPoll56Message(m_gameAddress);


						lp56->SetEnabledGamesList(denomGameList);
						lp56->SetImplementedGames(implementedGames);
						lp56->Fill();
						m_sasPollQueue->Add(lp56, QUEUE_PRIORITY_HIGHEST);
						lp56->WaitForResponse();

						// for each enabled game, set game enabled
						if (lp56->GetReturnValue() == SASPOLL_SUCCESS)
						{
							LogString(GAMES, "LP_56 SASPOLL_SUCCESS");
							for (DWORD x = 0; x < lp56->m_enabledGames; x++)
							{

								WORD gameNumber = denomGameList[x];
								m_gameList.SetGameEnabled(gameNumber);
								CGame *denomGame = new CGame(MEMORY_NONE, false);
								denomGame->SetDenom(m_denomMult);
								denomGame->SetGameNumber(gameNumber);
								denomGame->Enable();

								if (!GetInfoFromLongPollB5(*denomGame))
								{
									// B5 failed, fall back to 53
									CLongPoll53Message* lp53 = new CLongPoll53Message(m_gameAddress);
									lp53->SetGameNumber(gameNumber);
									lp53->Fill();
									m_sasPollQueue->Add(lp53, QUEUE_PRIORITY_HIGHEST);
									lp53->WaitForResponse();
									if (lp53->GetReturnValue() == SASPOLL_SUCCESS)
									{
										denomGame->SetMfgID(lp53->m_gameID);
										WORD paytableLength = strlen(lp53->m_payTable);
										if (paytableLength > 0)
										{
											denomGame->SetPaytableIDLength(paytableLength);
											denomGame->SetPaytableID(lp53->m_payTable);
										}
										denomGame->SetGamenameLength(denomGame->GetDefaultGameName().size());
										denomGame->SetGameName(denomGame->GetDefaultGameName().c_str());
									}

									delete lp53;

								}

								m_denomGames->AddGame(denomGame);
								if (vEnabledGameNumbers->end() == std::find(vEnabledGameNumbers->begin(), vEnabledGameNumbers->end(), gameNumber))
								{
									LogString(GAMES, "LP_56 SetGameEnabled(%u)", gameNumber);
									vEnabledGameNumbers->push_back(gameNumber);
								}
							}
						}
						else
						{
							LogString(GAMES, "LP_56 SASPOLL_NACK");
							// lp56 failed, set returnVal error
							returnVal = SASPOLL_NACK;
						}

						delete lp56;
					}
					delete[] denomGameList;
				}
				else
				{
					LogString(GAMES, "LP_B2 SASPOLL_NACK");
					// Tracker# 018377
					// Only if more than 1 game is implemented do we want to get subgame information
					if (implementedGames > 1)
					{
						// Tracker# 018327
						// Get enabled games the old fashioned way lp56.
						WORD *enabledGameList;
						enabledGameList = new WORD[implementedGames];
						CLongPoll56Message *lp56;
						lp56 = new CLongPoll56Message(m_gameAddress);
						lp56->SetEnabledGamesList(enabledGameList);
						lp56->SetImplementedGames(implementedGames);
						lp56->Fill();
						m_sasPollQueue->Add(lp56, QUEUE_PRIORITY_HIGHEST);
						lp56->WaitForResponse();

						// for each enabled game, set game enabled
						if (lp56->GetReturnValue() == SASPOLL_SUCCESS)
						{
							LogString(GAMES, "LP_56 SASPOLL_SUCCESS");
							for (DWORD x = 0; x < lp56->m_enabledGames; x++)
							{

								WORD gameNumber = enabledGameList[x];
								m_gameList.SetGameEnabled(gameNumber);
								CGame *denomGame = new CGame(MEMORY_NONE, false);
								denomGame->SetDenom(m_denomMult);
								denomGame->SetGameNumber(gameNumber);
								denomGame->Enable();
								m_denomGames->AddGame(denomGame);
								if (vEnabledGameNumbers->end() == std::find(vEnabledGameNumbers->begin(), vEnabledGameNumbers->end(), gameNumber))
								{
									LogString(GAMES, "LP_56 SetGameEnabled(%u)", gameNumber);
									vEnabledGameNumbers->push_back(gameNumber);
								}
							}
						}
						else
						{
							LogString(GAMES, "LP_56 SASPOLL_NACK");
							returnVal = SASPOLL_NACK;
						}

						delete lp56;
					}
					else
					{
						if (m_sasState.GetEGMConfig().GetEGMType() != CUSTOM)
						{
							// 1 sub game, look for progressive level ids
							CLongPollB5Message* lpb5 = new CLongPollB5Message(m_gameAddress);
							lpb5->SetGameNumber(EGM_GAME_NUMBER);
							lpb5->Fill();
							m_sasPollQueue->Add(lpb5, QUEUE_PRIORITY_HIGHEST);
							lpb5->WaitForResponse();
							if (lpb5->GetReturnValue() != SASPOLL_NACK)
							{
								CGame* egmgame = games->GetEGM();
								if (egmgame != nullptr)
								{
									egmgame->SetLevelIds(lpb5->GetLevelIds());
								}
							}
							delete lpb5;
						}
						else
						{
							LogString(SASGENERAL, "Skipping progressive level ID query using long poll B5 due to CUSTOM EGM Type.");
						}
					}
				}

				delete lpb2;
				delete[] enabledDenomList;
			}
			else
			{
				LogString(GAMES, "LP_51 SASPOLL_NACK");
				m_gameList.SetError();
			}
			
			delete lp51;
		}
		else
		{
			LogString(GAMES, "LP_1F SASPOLL_NACK");
		}

		// Set error condition
		if (returnVal != SASPOLL_SUCCESS)
		{
			m_gameList.SetError();
		}

		UpdateCurrentGame(false);
		UpdateCurrentDenom(false);

		if (game != nullptr)
		{
			game->ChangeCurrentGameStatus(EGM_GAME_NUMBER == m_gameList.GetCurrentGame());
		}

		// Keep copy of egm game
		CGame* pTmpEgmGame = games->GetEGM();
		CGame *egmGame = pTmpEgmGame != nullptr ? new CGame(*pTmpEgmGame) : nullptr;

		// Set our game list object 
		// Tracker 18377 - we need to not set the implemented games because ATI games report a single game as a sub-game.

		CInternalMessage* finishedGamelistFirstPassMessage(nullptr);
		if (implementedGames > 1 || (implementedGames == 1 && m_denomGames->GetGameCount() <= 1))
		{
			LogString(GAMES, "%s %d implementedGames=%u DenomGamesCount=%u", __FUNCTION__, __LINE__, implementedGames, m_denomGames->GetGameCount());
			m_gameList.SetCount(implementedGames);

			// if this is a single subgame then the game needs to be enabled in m_gameList or it won't be part of data going to progressive protocol
			if (implementedGames == SINGLE_GAME_NUMBER)
			{
				LogString(GAMES, "%s %d SINGLE_GAME_NUMBER", __FUNCTION__, __LINE__);
				m_gameList.SetGameEnabled(SINGLE_GAME_NUMBER);
				if (vEnabledGameNumbers->end() == std::find(vEnabledGameNumbers->begin(), vEnabledGameNumbers->end(), SINGLE_GAME_NUMBER))
				{
					LogString(GAMES, "%s %d", __FUNCTION__, __LINE__);
					vEnabledGameNumbers->push_back(SINGLE_GAME_NUMBER);
				}
			}
			else
			{
				LogString(GAMES, "%s %d", __FUNCTION__, __LINE__);
				//Try if this game supports lp53
				CLongPoll53Message *lp53 = new CLongPoll53Message(m_gameAddress);
				lp53->SetGameNumber((DWORD)SINGLE_GAME_NUMBER);
				lp53->Fill();
				m_sasPollQueue->Add(lp53, QUEUE_PRIORITY_HIGHEST);
				lp53->WaitForResponse();
				if (lp53->GetReturnValue() == SASPOLL_SUCCESS)
				{
					//This check is for some games like WMS, which incorrectly reports the number of implemented subgames
					//We check if LP53 fails twice and LP1A is successful in between.  If this is the case, we delete the disabled subgame number.
					LogString(GAMES, "GetGameList implementedGames=%u m_numberOfSubgamesMismatchCheck=%u", implementedGames, m_numberOfSubgamesMismatchCheck);
					if (implementedGames < m_numberOfSubgamesMismatchCheck)
					{
						bool sendTltSubgameCountMismatch(false);
						vector<DWORD> nonEnabledGames(m_gameList.GetNonEnabledGames());
						auto gameNumberIter = nonEnabledGames.begin();
						while (gameNumberIter != nonEnabledGames.end())
						{
							bool deleteNonEnabledGame(false);
							DWORD gameNumber(*gameNumberIter);
							lp53->ResetResponse();
							lp53->SetGameNumber(gameNumber);
							lp53->Fill();
							m_sasPollQueue->Add(lp53, QUEUE_PRIORITY_HIGHEST);
							lp53->WaitForResponse();
							if (lp53->GetReturnValue() != SASPOLL_SUCCESS)
							{
								LogString(GAMES, "Implemented disabled gameNumber=%u (LP53 failed on 1st try)", gameNumber);
								CLongPoll1AMessage *lp1A = new CLongPoll1AMessage(m_gameAddress, nullptr);
								lp1A->Fill();
								m_sasPollQueue->Add(lp1A, QUEUE_PRIORITY_HIGHEST);
								lp1A->WaitForResponse();
								if (lp1A->GetReturnValue() == SASPOLL_SUCCESS)
								{
									LogString(GAMES, "Implemented disabled gameNumber=%u (LP1A successful on 1st try)", gameNumber);
									lp53->ResetResponse();
									lp53->SetGameNumber(gameNumber);
									lp53->Fill();
									m_sasPollQueue->Add(lp53, QUEUE_PRIORITY_HIGHEST);
									lp53->WaitForResponse();
									if (lp53->GetReturnValue() != SASPOLL_SUCCESS)
									{
										LogString(GAMES, "Implemented disabled gameNumber=%u (LP53  failed on 2nd try)", gameNumber);
										lp1A->ResetResponse();
										lp1A->Fill();
										m_sasPollQueue->Add(lp1A, QUEUE_PRIORITY_HIGHEST);
										lp1A->WaitForResponse();
										if (lp1A->GetReturnValue() == SASPOLL_SUCCESS)
										{
											LogString(GAMES, "Implemented disabled gameNumber=%u (LP1A successful on 2nd try)", gameNumber);
											lp53->ResetResponse();
											lp53->SetGameNumber(gameNumber);
											lp53->Fill();
											m_sasPollQueue->Add(lp53, QUEUE_PRIORITY_HIGHEST);
											lp53->WaitForResponse();
											if (lp53->GetReturnValue() != SASPOLL_SUCCESS)
											{
												LogString(GAMES, "Implemented disabled gameNumber=%u (LP53 failed on failed on 3rd try)", gameNumber);
												deleteNonEnabledGame = true;
											}
										}
									}
								}
								delete lp1A;
							}				

							if (deleteNonEnabledGame)
							{
								LogString(GAMES, "Deleting disabled implemented game number = %u", gameNumber);
								gameNumberIter = nonEnabledGames.erase(gameNumberIter);
								sendTltSubgameCountMismatch = true;
							}
							else
							{
								++gameNumberIter;
							}

							CWatchDog::Instance().Checkin();
							CUtilities::Sleep(SLEEP_YIELD_MILLISECONDS);
						}

						if (sendTltSubgameCountMismatch)
						{
							m_gameList.SetNonEnabledGames(nonEnabledGames);
							SendTiltToInternals(tltSubgameCountMismatch);
						}
					}
				}
				else
				{
					LogString(GAMES, "%s %d LP_53 SASPOLL_NACK", __FUNCTION__, __LINE__);
				}
				delete lp53;
			}

			// Not sending egmGame, delete it
			delete egmGame;
		}
		else
		{
			LogString(GAMES, "%s %d Sending EGM game in INT_MSG_FINISHED_GAMESLIST_FIRSTPASS", __FUNCTION__, __LINE__);
			// Tell internals first pass through subgames is complete
			// also need totCoinOutCredits 
			CMeters *coMeters = new CMeters(MEMORY_NONE);
			coMeters->SetMeter(gCO);
			GetMeters(*coMeters, EGM_GAME_NUMBER, true, true);

			// Include EGM_GAME in m_denomGames
			finishedGamelistFirstPassMessage = new CInternalMessage(INT_MSG_FINISHED_GAMESLIST_FIRSTPASS);
			finishedGamelistFirstPassMessage->SetData(coMeters, EGM_GAME_NUMBER);
			if (egmGame != nullptr)
			{
				LogString(GAMES, "%s %d", __FUNCTION__, __LINE__);
				m_denomGames->AddGame(egmGame);
				finishedGamelistFirstPassMessage->SetData((WORD)false); // no error
			}
			else
			{
				LogString(GAMES, "%s %d", __FUNCTION__, __LINE__);
				finishedGamelistFirstPassMessage->SetData((WORD)true); //isError = true
			}
			CGames *copyDenomGames = new CGames(*m_denomGames);
			finishedGamelistFirstPassMessage->SetData(copyDenomGames);
		}

		// send egm to internals
		CInternalMessage *msg = new CInternalMessage(INT_MSG_SET_GAME_LIST, games);
		implementedGames = m_gameList.GetCount();
		WORD implemtedGamesPlusEgm0(implementedGames + 1);
		LogString(GAMES, "Number of Implemented is %u.", implemtedGamesPlusEgm0);
		msg->SetVectorOfWords(vEnabledGameNumbers);
		msg->SetData(implemtedGamesPlusEgm0);
		SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);

		LogString(GAMES, _T("Number Of Enabled Games = %u"), vEnabledGameNumbers->size());
		SendMessageToInternals(new CInternalMessage(INT_MSG_SET_ENABLED_GAMES, vEnabledGameNumbers), QUEUE_PRIORITY_NORMAL);

		if (finishedGamelistFirstPassMessage)
		{
			SendMessageToInternals(finishedGamelistFirstPassMessage, QUEUE_PRIORITY_NORMAL);
		}	

		// Send the currently selected game and number of games to internals as meters
		CMeters *selGameAndCountMeters = new CMeters(MEMORY_NONE);
		selGameAndCountMeters->SetMeter(mSelG, m_gameList.GetCurrentGame());
		selGameAndCountMeters->SetMeter(mNumG, implementedGames);
		CInternalMessage *selGameAndCountMsg = new CInternalMessage(INT_MSG_SET_METERS, selGameAndCountMeters, EGM_GAME_NUMBER);
		SendMessageToInternals(selGameAndCountMsg, QUEUE_PRIORITY_NORMAL);

	}

	return;
}
void CSASProtocol::GetCumulativeProgressiveWin()
{
	CLongPoll83Message *lp83 = new CLongPoll83Message(m_gameAddress);
	lp83->Fill();
	m_sasPollQueue->Add(lp83, QUEUE_PRIORITY_HIGHEST); // Chose highest because other polls occuring at this time are also highest
	lp83->WaitForResponse();
	if (lp83->GetReturnValue() == SASPOLL_SUCCESS)
	{
		__int64 amount = lp83->GetAmount() * m_denomMult;
		LogString(SASGENERAL, "GetCumulativeProgressiveWin LP83 amount=%lld", (long long)amount);
		SendCumulativeProgressiveWinToInternals(amount);
	}
	else
	{
		LogString(SASGENERAL, "GetCumulativeProgressiveWin LP83 fail=%d", (int)lp83->GetReturnValue());
	}
	delete lp83;
}

/// <summary>
/// Sends the cumulative progressive win to internals.
/// </summary>
/// <param name="amount">IN - The amount in cents.</param>
void CSASProtocol::SendCumulativeProgressiveWinToInternals(__int64 amount)
{
	CInternalMessage *msg = new CInternalMessage(INT_MSG_CUMULATIVE_PROGRESSIVE_WIN, amount);
	SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);
}

bool CSASProtocol::GetMaxBetAndProgressiveGroup(WORD gameNumber, CGame &game)
{
	bool successful(false);

	CLongPoll53Message *lp53 = new CLongPoll53Message(m_gameAddress);
	lp53->SetGameNumber(gameNumber);
	lp53->Fill();
	m_sasPollQueue->Add(lp53, QUEUE_PRIORITY_HIGHEST);
	lp53->WaitForResponse();
	if (lp53->GetReturnValue() == SASPOLL_SUCCESS)
	{
		successful = true;
		game.SetMaxBet(lp53->m_maxBet);
		game.SetProgressiveGroup(lp53->m_progressiveGroup);
	}
	delete lp53;

	return successful;
}

bool CSASProtocol::SendSubGameInfo(WORD gameNumber)
{
	// long poll 53 get game n configuration
	CLongPoll53Message *lp53 = new CLongPoll53Message(m_gameAddress);
	lp53->SetGameNumber(gameNumber);
	lp53->Fill();
	m_sasPollQueue->Add(lp53, QUEUE_PRIORITY_HIGHEST);
	lp53->WaitForResponse();

	bool subGameInfoSent(false);

	if (lp53->GetReturnValue() == SASPOLL_SUCCESS)
	{
		// get configuration information for subgame
		CGames* games(new CGames(MEMORY_NONE));
		CGame* game(nullptr);

		if (gameNumber == 0)
			game = new CGame(MEMORY_NONE, true);
		else
			game = new CGame(MEMORY_NONE, false);
		games->AddGame(game);
		game->SetGameNumber(gameNumber);
		game->SetDenom(denomCodeMap[lp53->m_denomination].multiplier, US_DOLLARS);
		game->SetMfgID(lp53->m_gameID);
		if (m_gameList.IsGameEnabled(gameNumber))
			game->Enable();
		else
			game->Disable();
		WORD currentGame = m_gameList.GetCurrentGame();
		game->ChangeCurrentGameStatus(currentGame == gameNumber ? true : false);
		game->SetAdditionalID(lp53->m_additionalID);
		game->SetTheoreticalPayback(lp53->m_basePercent);

		game->SetPaytableIDLength(strlen(lp53->m_payTable));
		game->SetPaytableID(lp53->m_payTable);
		game->SetGamenameLength(game->GetDefaultGameName().size());
		game->SetGameName(game->GetDefaultGameName().c_str());
		game->SetInfoFetchedFlag(true);
		LogString(GAMES, "%s:%d ===== %s, %s", __FUNCTION__, __LINE__, lp53->m_payTable, game->GetDefaultGameName().c_str());

		if (GetInfoFromLongPollB5(*game))
		{
			// If this game isn't in m_denomGames and it is game number 1 and list is empty then add it
			if (game->GetGameNumber() == SINGLE_GAME_NUMBER && m_denomGames->GetGameCount() == 0 && m_denomGames->GetGame(game->GetGameNumber()) == nullptr)
			{
				LogString(GAMES, _T("Adding game %d to games denom list"), gameNumber);
				m_denomGames->AddGame(new CGame(*game));
			}

			// send egm to internals
			SendMessageToInternals(new CInternalMessage(INT_MSG_SET_GAME_LIST, games), QUEUE_PRIORITY_HIGHEST);
			subGameInfoSent = true;
			LogString(GAMES, "SASProtocol: Sent Sub Game info for %d", gameNumber);
		}
		else
		{
			LogString(GAMES, "SASProtocol: Sending Sub Game#%d info failed because long poll B5 failed.", gameNumber);
			delete games;
		}
		LogString(GAMES, "%s:%d ===== After B5 code =====%s, %s", __FUNCTION__, __LINE__, lp53->m_payTable, game->GetDefaultGameName().c_str());
	}
	else
	{
		LogString(GAMES, "SASProtocol: Sending Sub Game#%d info failed because long poll 53 failed.", gameNumber);
	}
	delete lp53;

	return subGameInfoSent;
}

void CSASProtocol::SendSubGameInfo(WORD gameNumber, bool resetToLastPreviousValuesIfPollsFail)
{
	LogString(GAMES, "SASProtocol: Sending Sub Game info for %d", gameNumber);

	//Try sending subgame info for the 2nd time, if it fails the first time
	//We've seen LP53 fails intermittently when a game has a large number of subgames.
	bool subGameInfoSent(SendSubGameInfo(gameNumber));
	if (!subGameInfoSent)
	{
		CLongPoll1AMessage *lp1A = new CLongPoll1AMessage(m_gameAddress, nullptr);
		lp1A->Fill();
		m_sasPollQueue->Add(lp1A, QUEUE_PRIORITY_HIGHEST);
		lp1A->WaitForResponse();
		if (lp1A->GetReturnValue() == SASPOLL_SUCCESS)
		{
			subGameInfoSent = SendSubGameInfo(gameNumber);
		}
		delete lp1A;
	}

	if (!subGameInfoSent && resetToLastPreviousValuesIfPollsFail)
	{
		m_gameList.ResetTheValuesToLastGameWeTriedToSend();
	}
}

bool CSASProtocol::GetInfoFromLongPollB5(CGame &game)
{
	LogString(GAMES, "%s:%d", __FUNCTION__, __LINE__);

	bool successful(false);

	bool bMaxBetAndProgressiveGroupFound = false;
	DWORD gameNumber = game.GetGameNumber();
	
	CLongPollB5Message *lpB5 = new CLongPollB5Message(m_gameAddress);
	lpB5->SetGameNumber(gameNumber);
	lpB5->Fill();
	m_sasPollQueue->Add(lpB5, QUEUE_PRIORITY_HIGHEST); // Chose highest because other polls occuring at this time are also highest
	lpB5->WaitForResponse();
	if (lpB5->GetReturnValue() == SASPOLL_SUCCESS)
	{
		successful = true;
		char *gameName = nullptr;
		char *paytableID = nullptr;
		byte gamenameLength = lpB5->GetGameName(&gameName);
		byte paytableIDLength = lpB5->GetPaytableName(&paytableID);

		if (gamenameLength > 0)
		{
			game.SetGamenameLength(gamenameLength);
			game.SetGameName(gameName);
		}

		if (paytableIDLength > 0)
		{
			game.SetPaytableIDLength(paytableIDLength);
			game.SetPaytableID(paytableID);
		}

		// Get MaxBet using B5 poll
		game.SetMaxBet(lpB5->GetMaxBet());
		game.SetProgressiveGroup(lpB5->GetProgressiveGroup());
		bMaxBetAndProgressiveGroupFound = true;  //// B5 poll got us Max bet and Progressive group

		game.SetLevelIds(lpB5->GetLevelIds());

		DeleteArray(gameName);
		DeleteArray(paytableID);
		
		LogString(GAMES, "%s:%d := gamenumber: %u, gamename: %s(%u), paytableid: %s(%d)", 
			__FUNCTION__, __LINE__,
			gameNumber, game.GetGameName(), game.GetGamenameLength(),
			game.GetPaytableID(), strlen(game.GetPaytableID()));
	}
	delete lpB5;

	if( bMaxBetAndProgressiveGroupFound != true )
	{
		//Otherwise try with LP53	
		successful = GetMaxBetAndProgressiveGroup(gameNumber, game);
	}

	return successful;
}

void CSASProtocol::ProcessMeterChangeAllow( CInternalMessage *msg )
{
	// Tell EGM whether to allow meter change
	byte bAllow;
	bAllow = msg->GetWordData() == 0 ? false : true;
	
	CLongPollB6Message lpb6(m_gameAddress);

	if (bAllow)
		lpb6.SetReady();
	else
		lpb6.SetUnable();

	lpb6.Fill();
	m_sasPollQueue->Add(&lpb6,QUEUE_PRIORITY_NORMAL);
	lpb6.WaitForResponse();

	// tell internels whether poll was successful or not
	CInternalMessage *intMsg = new CInternalMessage(INT_MSG_METER_CHANGE_ALLOW_SENT);
	if (lpb6.GetReturnValue() == SASPOLL_SUCCESS)
	{
		intMsg->SetData((WORD)true);
	}
	else
	{
		intMsg->SetData((WORD)false);
	}
	SendMessageToInternals(intMsg,QUEUE_PRIORITY_NORMAL);

}

void CSASProtocol::LossLimitBillMeters()
{
	GetBillMeters();

	// notify internals, bill meters done
	CInternalMessage *intMsg = new CInternalMessage(INT_MSG_BILL_METERS_COMPLETE);
	SendMessageToInternals(intMsg,QUEUE_PRIORITY_NORMAL);

	return;
}

bool CSASProtocol::CheckForBillMeterPolls()
{
	bool retVal(false);
	if (m_sasState.GetEGMConfig().GetEGMProtocol() == IGT_SAS_3xx)
	{
		CMeters *meters = new CMeters(MEMORY_NONE);
		CLongPoll1EMessage lp1e(m_gameAddress, meters);
		lp1e.Fill();
		m_sasPollQueue->Add(&lp1e, QUEUE_PRIORITY_HIGHEST);
		lp1e.WaitForResponse();
		if (lp1e.GetReturnValue() == SASPOLL_SUCCESS)
		{
			retVal = true;
		}
		else
		{
			CLongPoll31Message lp31(m_gameAddress, meters);
			lp31.Fill();
			m_sasPollQueue->Add(&lp31, QUEUE_PRIORITY_HIGHEST);
			lp31.WaitForResponse();
			if (lp31.GetReturnValue() == SASPOLL_SUCCESS)
			{
				retVal = true;
			}
			else if (m_sasBills.GetTotalBills() == INVALID_METER_VALUE)
			{
				GetBillMeter(QUEUE_PRIORITY_HIGHEST);
			}
		}
		delete meters;
	}
	else
	{
		retVal = true;
	}

	return retVal;
}

void CSASProtocol::ResetJackpot(CInternalMessage *msg)
{
	CJackpot *jackpot = msg->GetJackpotData();

	if (NULL != jackpot)
	{
		CJackpot::ResetResult KeyToCreditResult = CJackpot::ResetUnavailable;

		// If jackpot set with key-to-credit is not available, don't try to do
		if (!jackpot->IsKeyToCreditAvailable())
		{
			KeyToCreditResult = CJackpot::ResetUnavailable;
		}

		// If set to always-do-key-to-credit and jackpot not marked as key-to-credit unavailable,
		//  assume we already sent the key to credit command.
		else if (m_resetToCredit)
		{
			KeyToCreditResult = CJackpot::ResetSuccess;
		}

		// Otherwise, set to do key-to-credit upon reset
		else
		{
			KeyToCreditResult = ResetToCredit();
		}

		// If the key-to-credit was successful, attempt the remote reset
		CJackpot::ResetResult RemoteResetResult = CJackpot::ResetUnavailable;
		if (KeyToCreditResult == CJackpot::ResetSuccess)
		{
			RemoteResetResult = RemoteReset();

			//Acquire immediately if tax withholding is enabled, no need to check return status because
			//Internals will start the normal PBT flow when it tries to take out the taxes
			if (CJackpot::ResetSuccess == RemoteResetResult && /*UJPTaxWithholding*/(bool)msg->GetWordData())
			{
				// Do a round of GPs to finish off AFT bonus transfer which should be completed now
				DoGeneralPoll();

				// Lock game
				CLongPoll74Message lp74(m_gameAddress);
				lp74.SetLockCode(LOCK_CODE_REQUEST_LOCK);
				lp74.SetLockTimeout(CPersonalBanker::PB_SESSION_TIMEOUT_MOBILE / 10);
				lp74.SetXferFromGame(true);
				lp74.Fill();
				m_sasPollQueue->Add(&lp74, QUEUE_PRIORITY_HIGHEST);
				lp74.WaitForResponse();
				m_sasState.SetLastLockRequest(CUtilities::GetTickCount());
			}
		}

		// Set the unattended reset state in the jackpot
		jackpot->SetUnattendedResetResult(KeyToCreditResult, RemoteResetResult);

		SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);
	}
	else
	{
		delete msg;
	}
}

CJackpot::ResetResult CSASProtocol::ResetToCredit(bool credit)
{
	CJackpot::ResetResult result = CJackpot::ResetFailure;
	LPCTSTR resultLogString("Reset to credit poll not successful");

	// Send A8 poll
	CLongPollA8Message lpA8(m_gameAddress);
	if (credit)
		lpA8.SetCreditReset();
	else
		lpA8.SetStandardHandpay();

	lpA8.Fill();
	m_sasPollQueue->Add(&lpA8, QUEUE_PRIORITY_HIGHEST);
	lpA8.WaitForResponse();

	if (lpA8.GetReturnValue() == SASPOLL_SUCCESS)
	{
		switch (lpA8.GetAckCode())
		{
		case CLongPollA8Message::CreditResetEnabled:
			result = CJackpot::ResetSuccess;
			resultLogString = "Reset to credit reset was successful";
			break;
		case CLongPollA8Message::UnableToResetToCredit:
			result = CJackpot::ResetUnavailable;
			resultLogString = "unable to reset to credit";
			break;
		case CLongPollA8Message::NotInHandpay:
			result = CJackpot::NotInHandpay;
			resultLogString = "reset to credit was not currently in a handpay";
			break;
		default:
			result = CJackpot::ResetFailure;
			resultLogString = "reset to credit result is unknown";
			break;
		}
	}

	LogStringNoFormat(SASGENERAL, resultLogString);

	return result;
}

CJackpot::ResetResult CSASProtocol::RemoteReset()
{
	CJackpot::ResetResult result = CJackpot::ResetFailure;
	LPCTSTR resultLogString("Remote handpay poll not successful");

	CLongPoll94Message lp94(m_gameAddress);
	lp94.Fill();
	m_sasPollQueue->Add(&lp94, QUEUE_PRIORITY_HIGHEST);
	lp94.WaitForResponse();
	if (lp94.GetReturnValue() == SASPOLL_SUCCESS)
	{
		switch (lp94.GetAckCode())
		{
		case CLongPoll94Message::RemoteResetSuccess:
			result = CJackpot::ResetSuccess;
			resultLogString = "Remote handpay reset was successful";
			break;
		case CLongPoll94Message::UnableToRemoteReset:
			result = CJackpot::ResetUnavailable;
			resultLogString = "unable to reset handpay";
			break;
		case CLongPoll94Message::NotInHandpay:
			result = CJackpot::NotInHandpay;
			resultLogString = "Cannot remote handpay. not currently in a handpay";
			break;
		default:
			resultLogString = "Remote handpay reset result is unknown";
			break;
		}
	}

	LogStringNoFormat(SASGENERAL, resultLogString);

	return result;
}
