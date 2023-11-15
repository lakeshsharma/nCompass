#include "stdafx.h"
#include "InternalsFramework.h"

// Sentinel includes.
#include "Logging/LogString.h"
#include "Logging/Logger.h"
#include "Sentinel.h"
#include "Config/ConfigDenomField.h"
#include "Config/ConfigCasinoNumField.h"
#include "Config/ConfigCardTimeoutField.h"
#include "Config/ConfigCasinoNameField.h"
#include "Config/ConfigCommIDField.h"
#include "Config/ConfigAllowFloorLockoutField.h"
#include "Config/ConfigFloorLockField.h"
#include "Config/ConfigOtherPrefixesField.h"
#include "Config/ConfigPlayerPrefixField.h"
#include "Config/ConfigPlayerTrackingField.h"
#include "Config/ConfigSlotMastRevisionField.h"
#include "Config/ConfigPointsPerDollarField.h"
#include "Config/ConfigSenOption1Field.h"
#include "Config/ConfigSenOption2Field.h"
#include "Config/ConfigSlotNumberField.h"
#include "Config/ConfigTicketingField.h"
#include "Config/ConfigVirtualSessionField.h"
#include "Config/ConfigPBTField.h"
#include "Config/ConfigCashTransferTrackingField.h"
#include "Config/ConfigTimezoneField.h"
#include "Config/ConfigPointsForPromoField.h"
#include "Config/ConfigUniverseEnabledField.h"
#include "Config/ConfigUniverseCardPrefixField.h"
#include "Config/ConfigUniversePropCodesField.h"
#include "Config/ConfigNclEnabledCardTrackField.h"
#include "Config/ConfigSessionPointsTextField.h"
#include "Config/ConfigBalanceRetrievalTextField.h"
#include "Config/ConfigPointsOrCashLabelField.h"
#include "Config/ConfigShowPointsVsAccountBalanceField.h"
#include "Config/ConfigDailyPointsDisplaySuffixField.h"
#include "Config/ConfigTierPointsLabelField.h"
#include "Config/ConfigPlayerProfileButtonField.h"
#include "Config/ConfigMGMDField.h"
#include "Config/ConfigExcessiveVoucherOutEnabledField.h"
#include "Config/ConfigExcessiveVoucherOutThresholdField.h"
#include "Config/ConfigPartialWATOutEnabledField.h"
#include "Config/ConfigExtensibleSystemSettingsField.h"
#include "UI/UnattendedJackpotCommand.h"
#include "Diagnostics.h"
#include "Hardware/WatchDog.h"
#include "MediaPackageManager.h"
#include "PollerProtocol/PollerMGMDData.h"
#include "PollerProtocol/MobileConnectProtocol/MobileConnectProtocolKeys.h"
#include "PollerProtocol/MobileConnectProtocol/CardChangeEvent.h"
#include "PollerProtocol/MobileConnectProtocol/PairingEvent.h"
#include "PollerProtocol/MobileConnectProtocol/MobileConnectProtocolKeys.h"
#include "PollerProtocol/MobileConnectProtocol/PairingRequest.h"

const static char* TIMEOUT = "Timeout";
const unsigned int MINIMUM_POWER = 2;

using namespace std;

void CInternalsFramework::ProcessMessageFromSCF( CInternalMessage *msg )
{
	// Most of the message we will delete below.
	bool bDeleteMsg = true;

	switch ( msg->GetMessageType() )
	{
	case INT_MSG_GET_METERS:
		ProcessGetMetersFromSCF( msg );
		break;
	case INT_MSG_GET_SUBGAME_METERS:
		ProcessGetSubgameMetersFromSCF();
		break;
	case INT_MSG_GET_STATUS:
		ProcessGetStatusFromSCF();
		break;
	case INT_MSG_SET_SYSTEM_CONFIG:
		ProcessSystemConfigFromSCF( msg );
		break;
	case INT_MSG_SET_TIME:
		ProcessSetTimeFromSCF( msg );
		bDeleteMsg = false;
		break;
	case INT_MSG_SET_DISPLAY_MESSAGE:
		ProcessSetDisplayMessageFromSCF( msg );
		break;
	case INT_MSG_SET_GLOBAL_PASSWORD:
		ProcessSetGlobalPasswordFromSCF( msg );
		break;
	case INT_MSG_SET_SLOTMASTID:
		ProcessSetSlotMastIDFromSCF( msg );
		break;
	case INT_MSG_SYSTEM_ONLINE:
		ProcessOnlineFromSCF( msg );
		break;
	case INT_MSG_POLLERID:
		break;
	case INT_MSG_CONNECTIVITY_STATUS:
		ProcessConnectivityStatus( msg );
		break;
	case INT_MSG_POLLER_PACKET_SENT_VERIFY_ERRORS:
		ProcessPollerPacketSentVerifyErrors( msg );
		break;
	case INT_MSG_SEND_TILT:
		ProcessTilt(msg);
		break;
	case INT_MSG_SET_PLAYER_INFO:
		ProcessPlayerInfoFromSCF( msg );
		break;
	case INT_MSG_SET_GLOBAL_BALANCE:
		ProcessPlayerGlobalBalanceFromSCF( msg );
		break;
	case INT_MSG_PLAYER_BONUS_EVENTS:
		ProcessPlayerBonusEventsFromSCF( msg );
		break;
	case INT_MSG_PLAYER_UPDATE_TIME:
		ProcessPlayerUpdateTimeFromSCF( msg );
		break;
	case INT_MSG_TICKET_STORED:
		ProcessTicketStoredFromSCF( msg );
		break;
	case INT_MSG_TICKET_OUT_PENDING_COUNT:
		ProcessTicketOutPendingCountFromSCF( msg );
		break;
	case INT_MSG_TICKET_RESPONSE:
		ProcessTicketResponseFromSCF( msg );
		break;
	case INT_MSG_EMPLOYEE_RESPONSE:
		ProcessEmployeeResponseFromSCF( msg );
		break;
	case INT_MSG_PB_RESPONSE:
		ProcessPBResponseFromSCF( msg );
		break;
	case INT_MSG_PB_STORED:
		ProcessPersonalBankerTransactionStored();
		break;
	case INT_MSG_PB_ACKED:
		ProcessPersonalBankerTransactionAckedFromSCF( msg );
		break;
	case INT_MSG_AUTHORIZATION_GRANTED:
		ProcessPersonalBankerAuthorizationFromSCF( msg , true );
		break;
	case INT_MSG_AUTHORIZATION_DENIED:
		ProcessPersonalBankerAuthorizationFromSCF( msg , false );
		break;
	case INT_MSG_NOTIFY_RESEND_EMPTY:
		ProcessResendEmptyFromSCF(msg);
		break;
	case INT_MSG_JACKPOT_STORED:
		ProcessJackpotTransactionStored();
		break;
	case INT_MSG_POLLER_LIST_OK:
		ProcessTransactionFillStatus(transactionsFillOkay);
		break;
	case INT_MSG_POLLER_LIST_WARNING:
		ProcessTransactionFillStatus(transactionsFillWarning);
		break;
	case INT_MSG_POLLER_LIST_FULL:
		ProcessTransactionFillStatus(transactionsFull);
		break;
	case INT_MSG_LOSS_LIMIT_BALANCE_UPDATE:
		// Loss limit feature no longer supported
		break;
	case INT_MSG_SCF_PROTOCOL_VERSION:
		ProcessSCFProtocolVersion( msg );
		break;
	case INT_MSG_SET_ONE_LINK_PROGRESSIVES_SEN_OPTION_SUPPORT:
		ProcessSetOneLinkProgressivesSenOptionSupport(msg);
		break;
	case INT_MSG_GET_IP:
		SendIPToSCFProtocol();
		break;
	case INT_MSG_RESTART_SERVICEWINDOW:
		ProcessRestartServiceWindow();
		break;
	case INT_MSG_UPGRADE_AVAILABLE:
		SetFirmwareVersion();
		break;
	case INT_MSG_MWCONFIG_FILE_AVAILABLE:
		ProcessMWConfigFileAvailable( msg );
		break;
	case INT_MSG_UNATTENDED_JACKPOT:
		ProcessUJP(msg);
		break;
	case INT_MSG_AUTHORIZE_PIN:
		ProcessUJPAuthorizePin(msg);
		break;
	case INT_MSG_OS_UPGRADE:
		ProcessOsUpgrade();
		break;
	case INT_MSG_SUBGAME_SELECTION:
		ProcessSubgameSelectionResponse(msg);
		break;
	case INT_MSG_GAME_LIST_CHUNKS_SENT:
		ProcessGameListChunksSentInformation(msg);
		break;
	case INT_MSG_REMOTE_REBOOT:
		ProcessRemoteReboot(msg);
		break;
	case INT_MSG_PB_PIN_RESET_RESULT:
		ProcessPBPinResetResponseFromPoller(msg);
		break;
	case INT_MSG_SET_POLLER_IP:
		ProcessSetPollerIPFromSCF(msg);
		break;
	case INT_MSG_PROCESS_TAX_DEDUCTION_COMPLETE:
		ProcessTaxDeductionComplete(msg);
		break;
	case INT_MSG_BAD_PACKET_RECEIVED_FROM_POLLER:
		GenerateTilt(tltBadPacketReceivedFromPoller);
		break;
	case INT_MSG_CALL_MC_PAIRING_SETTINGS_REQUEST:
		ProcessCallMobileConnectPairingSettingsRequest();
		break;
	case INT_MSG_PAIRING_SETTINGS_EVENT:
		ProcessPairingSettingsEventData(msg);
		break;
	case INT_MSG_PAIRING_EVENT:
		ProcessPairingEventData(msg);
		break;
    case INT_MSG_CARD_CHANGE_REQUEST:
        ProcessCardChangeRequestData(msg);
        break;
	case INT_MSG_CREDIT_TOTALS_REQUEST:
		ProcessCreditTotalsRequestData(msg);
        break;
	case INT_MSG_FUNDS_TRANSFER_REQUEST:
		ProcessFundsTransferRequestData(msg);
        break;
	case INT_MSG_PB_CANCEL_SESSION:
		{
			LogString(POLLER_LOG, "Received session.request.endPbtSession from SCF, ending the PBT session and sending back session.event.endPbtSession to Poller/PDS.");
			ProcessCancelPersonalBankerSession(msg);

			if (m_config.MobileConnectCommunicationEnabled())
			{
				CInternalMessage *pollerMsg = PrepareTransactionMessage(INT_MSG_TRANSACTION_MOBILE_CONNECT_PROXIMITY, IsPlayerVirtual());
				pollerMsg->SetData((WORD) ToPlayerDeviceService::EventEndPBTSession);
				SendTransactionMessageToSCF( pollerMsg, QUEUE_PRIORITY_NORMAL );
			}
		}
		break;
	case INT_MSG_PROXIMITY_BEACON_SCAN_LIST_RESPONSE:
		ProcessProximityBeaconScanListResponse(msg);
		break;
	case INT_MSG_CALL_MCP_PROXIMITY_SETTINGS_REQUEST:
		ProcessCallProximitySettingsRequest();
		break;
	case INT_MSG_PROXIMITY_SETTINGS_EVENT:
		ProcessProximitySettingsEventData(msg);
		break;
	default:
		break;
	}

	// If we are sending the message on to somewhere else, we don't want to delete the message.
	if ( bDeleteMsg )
		delete msg;

	CWatchDog::Instance().Checkin();

}

void CInternalsFramework::ProcessTransactionFillStatus(TransactionsFillStatus fillStatus)
{
	m_sentinelState.SetTransactionFillStatus(fillStatus);

	switch (m_sentinelState.GetTransactionFillStatus())
	{
	case transactionsFillOkay:
		SendSystemBusyToUI(false);
		if ( m_EGMConfig.GetEGMProtocolState() == protocolStateDisableTransactionsFull )
		{
			StartGCF();
		}
		break;
	case transactionsFillWarning:
		SendSystemBusyToUI(true);
		if ( m_EGMConfig.GetEGMProtocolState() == protocolStateDisableTransactionsFull )
		{
			StartGCF();
		}		
		ForcePlayerCardOut();
		break;
	case transactionsFull:
		SendSystemBusyToUI(true);
		if ( m_EGMConfig.GetEGMProtocolState() == protocolStateEnabled )
		{
			m_EGMConfig.SetEGMProtocolState( protocolStateDisableTransactionsFull );
			UninitializeGCF();
		}
		ForcePlayerCardOut();
		break;
	}
}

void CInternalsFramework::ProcessGetMetersFromSCF( CInternalMessage *msg )
{
	// Extract the game number whose meters we want
	WORD gameNumber;
	msg->GetMetersData(gameNumber);

	// Send the meters message for the requested game 
	LogString(GAMES, "[%s]: Sending meters for game [%u] to poller.",
		__FUNCTION__, gameNumber);
	SendGameMetersToSCF(m_games.GetGame(gameNumber));
}

void CInternalsFramework::ProcessGetSubgameMetersFromSCF(void)
{
	LogString(GAMES, "[%s]: Starting thread to send subgame meters to poller.",
		__FUNCTION__);
	StartSubgameMetersUpdateThread();
}

void CInternalsFramework::SendGameMetersToSCF(CGame *game)
{
	if (game)
	{
		CGames *scfGames = new CGames();
		CGame *scfGame = new CGame(*game);
		scfGames->AddGame(scfGame);

		if (game->GetGameNumber() == EGM_GAME_NUMBER)
		{
			// Its EGM game meters message we have to send
			if (IsNFDPoller())
			{
				// trim the EGM meters to non-zero valid values
				scfGame->GetMeters()->TrimToNonZeroValidMeters();

				// include current subgame information also
				CGame *activeGame = m_games.GetCurrentGame();
				if (activeGame && !activeGame->IsEGM())
				{
					LogString(GAMES, "[%s]: Sending current game [%u] information along with EGM game meters.",
						__FUNCTION__, activeGame->GetGameNumber());
					scfGame = new CGame(*activeGame);
					scfGames->AddGame(scfGame);
				}
			}
		}

		CInternalMessage *scfMsg = new CInternalMessage(INT_MSG_SET_METERS, scfGames);
		scfMsg->SetData((DWORD)m_playerSession.GetSessionId(), m_playerSession.GetPlayTransactionSequenceNumber());
		SendMessageToSCF(scfMsg, QUEUE_PRIORITY_HIGH);
	}
}


void CInternalsFramework::SendConfigChangeTransactionToSCF( ConfigChangeType type, __int64 previousValue, __int64 currentValue, byte pinNumber )
{
	CInternalMessage *transMsg = PrepareTransactionMessage( INT_MSG_TRANSACTION_CONFIG_CHANGE, IsPlayerVirtual() );

	transMsg->SetData( (WORD)type, (WORD)pinNumber );
	transMsg->SetData( previousValue, currentValue );

	SendTransactionMessageToSCF( transMsg, QUEUE_PRIORITY_NORMAL);
}

void CInternalsFramework::SendTicketOutPendingRequestToSCF( void )
{
	CInternalMessage *msg = new CInternalMessage( INT_MSG_TICKET_OUT_PENDING_COUNT );
	SendMessageToSCF( msg, QUEUE_PRIORITY_NORMAL );	
}

void CInternalsFramework::ProcessGetStatusFromSCF(void)
{
}

void CInternalsFramework::ProcessOnlineFromSCF( CInternalMessage *msg )
{
	LogString(POLLER_LOG, "ProcessOnlineFromSCF-SetConnectivityStatus");
	m_sentinelState.SetConnectivityStatus(msg->GetWordData());

	// We need to do the on-line meter capture after we get the config. tracker 17994
	if ( m_EGMConfig.GetSentinelOnlineState() == sentinelComingOnline )
	{
		ProcessSentinelChangeInProgressAfterEGMStart();
	}

	// We need to update the pairing UI in case Poller/MC communication is down (was up)
	if ( !m_sentinelState.IsSystemOnline() )
	{
		m_notificationBox.SetMobileEnableFlag(m_sentinelState.IsSystemOnline());
		SendUIConfigToUI();
	}

	SendMessageToSCF(new CInternalMessage(INT_MSG_TRYSENDGAMELIST_POLLER_ONLINE, 
					 (WORD)m_sentinelState.IsPollerReadyForSubgameList()), 
					 QUEUE_PRIORITY_NORMAL);
}

void CInternalsFramework::ProcessOsUpgrade()
{
	//Set it back to normal watchdog hardware timeout
	//after all the upgrade files are unpacked to firmware.new folder
	LogString(FLASHING, "WATCHDOG_HARDWARE_TIMEOUT");
	CWatchDog::Instance().Checkin();
	CWatchDog::Instance().SetWatchdogTimeout(WATCHDOG_HARDWARE_TIMEOUT);

	LogString(FLASHING, _T("ProcessOsUpgrade"));

	m_sentinelState.SetPendingAction(OsUpgrade);
	SendDashboardStatusToProgressive();

	m_restartManager.SetToPerformControlledRebootForOsUpgrade();
	SendSubscriptionUpdateIfNecessary();
}

void CInternalsFramework::ProcessConnectivityStatus( CInternalMessage *msg )
{
	LogString(POLLER_LOG, "ProcessConnectivityStatus-SetConnectivityStatus");
	m_sentinelState.SetConnectivityStatus(msg->GetWordData());
}

void CInternalsFramework::ProcessPollerPacketSentVerifyErrors( CInternalMessage *msg )
{
	m_sentinelState.m_pollerPacketSentVerifyErrors = msg->GetDWordData();
}

// CInternalsFramework::ProcessSystemConfigFromSCF()
// Process Sentinel Configuration information from the Poller.
// Loop through the configuration field instances until done.
void CInternalsFramework::ProcessSystemConfigFromSCF(CInternalMessage *msg)
{
	bool gotFullConfig(false); // Only update the last FloorLogix configuration time for a "full" configuration message.
	CConfigFields *configFields = msg->GetConfigData();

	//a subset of the config field will be sent in
	//this extracts that subset, and stores them in permanent storage.
	int fieldsCount = configFields->GetCount();

	bool poller_12_11_6000_or_Higher(false);
	bool refreshConfigs(false);
	for (int i = 0; i < fieldsCount; i++)
	{
		ConfigChangeType configChangeType = ccNone;
		__int64 previousValue = 0;
		__int64 currentValue = 0;

		CConfigField * configField = configFields->GetFieldByIndex(i);

		LogString(POLLER_LOG, "ProcessSystemConfigFromSCF() field=%u", configField->GetType());

		switch (configField->GetType())
		{
		case DENOMINATION:
		{
			WORD denom(((CConfigDenomField*)configField)->GetDenom());
			if (CGames::IsDenomValid(denom))
			{
				currentValue = denom;
				previousValue = m_config.SetDenomination(denom);
				configChangeType = ccDenomination;
			}
			else
			{
				if (!m_config.GetReceivedBadDenomFlag())
				{
					GenerateTilt(tltReceivedBadDenom);
					m_config.SetReceivedBadDenomFlag(true);
					LogString(ERROR_LOG, "ProcessSystemConfigFromSCF() received a bad denom=%u", denom);
					SendMessageToSCF(new CInternalMessage(INT_MSG_REQUEST_CONFIG), QUEUE_PRIORITY_NORMAL);
				}
				else
				{
					LogString(ERROR_LOG, "ProcessSystemConfigFromSCF() received consecutive bad denom=%u", denom);
				}
			}
			break;
		}
		case CASINO_NUMBER:
			currentValue = ((CConfigCasinoNumField*)configField)->GetCasinoNumber();
			previousValue = m_config.SetCasinoNumber(((CConfigCasinoNumField*)configField)->GetCasinoNumber());
			configChangeType = ccCasinoNumber;
			break;
		case COMM_ID:
			currentValue = ((CConfigCommIDField*)configField)->GetCommID();
			previousValue = m_config.SetCommID(((CConfigCommIDField*)configField)->GetCommID());
			configChangeType = ccCommID;
			refreshConfigs = true;
			break;
		case SEN_OPTIONS1:
			previousValue = m_config.GetSenOption1Raw();
			SetSenOptions1((CConfigSenOption1Field*)configField);
			currentValue = m_config.GetSenOption1Raw();
			configChangeType = ccOption0;
			refreshConfigs = true;

			// A "full" configuration message always includes SEN_OPTIONS1.
			// Only update the last FloorLogix configuration time for a "full" configuration message.
			gotFullConfig = true;
			break;
		case SEN_OPTIONS2:
			previousValue = m_config.GetSenOption2Raw();
			SetSenOptions2((CConfigSenOption2Field*)configField);
			currentValue = m_config.GetSenOption2Raw();
			configChangeType = ccOption1;
			refreshConfigs = true;
			break;
		case SLOT_NUMBER:
			currentValue = ((CConfigSlotNumberField*)configField)->GetSlotNumber();
			previousValue = m_config.SetSlotNumber(((CConfigSlotNumberField*)configField)->GetSlotNumber());
			configChangeType = ccSlotNumber;
			SendSlotNumberToGCF();
			break;
		case PLAYER_PREFIXES:
			m_config.SetPlayerCardPrefixes(((CConfigPlayerPrefixField*)configField)->GetPlayerPrefixes());
			refreshConfigs = true;
			break;
		case OTHER_PREFIXES:
			currentValue = ((CConfigOtherPrefixesField*)configField)->GetGlobalCardPrefix();
			previousValue = m_config.SetGlobalCardPrefix(((CConfigOtherPrefixesField*)configField)->GetGlobalCardPrefix());
			if (currentValue != previousValue)
			{
				SendConfigChangeTransactionToSCF(ccGlobalCardPrefix, previousValue, currentValue);
			}
			currentValue = ((CConfigOtherPrefixesField*)configField)->GetMechCardPrefix();
			previousValue = m_config.SetMechanicCardPrefix(((CConfigOtherPrefixesField*)configField)->GetMechCardPrefix());
			if (currentValue != previousValue)
			{
				SendConfigChangeTransactionToSCF(ccMechCardPrefix, previousValue, currentValue);
			}
			currentValue = ((CConfigOtherPrefixesField*)configField)->GetFloorCardPrefix();
			previousValue = m_config.SetFloorCardPrefix(((CConfigOtherPrefixesField*)configField)->GetFloorCardPrefix());
			if (currentValue != previousValue)
			{
				SendConfigChangeTransactionToSCF(ccFloorCardPrefix, previousValue, currentValue);
			}
			currentValue = ((CConfigOtherPrefixesField*)configField)->GetTempCardPrefix();
			previousValue = m_config.SetTempCardPrefix(((CConfigOtherPrefixesField*)configField)->GetTempCardPrefix());
			configChangeType = ccTempCardPrefix;
			refreshConfigs = true;
			break;
		case CASINO_NAME:
			m_config.SetCasinoName(((CConfigCasinoNameField*)configField)->GetCasinoName().c_str());
			break;
		case POINTS_PER_DOLLAR:
			currentValue = ((CConfigPointsPerDollarField*)configField)->GetPointsPerDollar();
			previousValue = m_config.SetPointsPerDollar(((CConfigPointsPerDollarField*)configField)->GetPointsPerDollar());
			configChangeType = ccCashDisplayIncrement;
			break;
		case LOSS_LIMIT:
			// Loss Limit is no longer supported
			break;
		case TICKETING:
			currentValue = ((CConfigTicketingField*)configField)->GetTicketExpirationType();
			previousValue = m_config.SetTicketExpirationType(((CConfigTicketingField*)configField)->GetTicketExpirationType());
			if (currentValue != previousValue)
			{
				SendConfigChangeTransactionToSCF(ccTicketExpirationType, previousValue, currentValue);
			}
			currentValue = ((CConfigTicketingField*)configField)->GetTicketExpirationDuration();
			previousValue = m_config.SetTicketExpirationDuration(((CConfigTicketingField*)configField)->GetTicketExpirationDuration());
			if (currentValue != previousValue)
			{
				SendConfigChangeTransactionToSCF(ccTicketExpirationTime, previousValue, currentValue);
			}
			currentValue = ((CConfigTicketingField*)configField)->TicketingEnabled();
			previousValue = m_config.SetTicketingEnabled(((CConfigTicketingField*)configField)->TicketingEnabled());
			if (currentValue != previousValue)
			{
				SendConfigChangeTransactionToSCF(ccTicketEnabled, previousValue, currentValue);
			}
			currentValue = ((CConfigTicketingField*)configField)->SystemGeneratedTicketValidationNumbers();
			previousValue = m_config.SetSystemGeneratedTicketValidation(((CConfigTicketingField*)configField)->SystemGeneratedTicketValidationNumbers());
			if (currentValue != previousValue)
			{
				SendConfigChangeTransactionToSCF(ccSystemGeneratedValidationNumbers, previousValue, currentValue);
			}
			currentValue = ((CConfigTicketingField*)configField)->TicketBufferingEnabled();
			previousValue = m_config.SetTicketBufferingEnabled(((CConfigTicketingField*)configField)->TicketBufferingEnabled());
			configChangeType = ccTicketBufferingEnabled;
			break;
		case PLAYER_TRACKING:
			currentValue = ((CConfigPlayerTrackingField*)configField)->GetPlayerTrackingMultiplier();
			previousValue = m_config.SetPlayerTrackingMultiplier(((CConfigPlayerTrackingField*)configField)->GetPlayerTrackingMultiplier());
			if (currentValue != previousValue)
			{
				SendConfigChangeTransactionToSCF(ccDefaultMultiplier, previousValue, currentValue);
			}
			currentValue = ((CConfigPlayerTrackingField*)configField)->GetPlayerTrackingDivisor();
			previousValue = m_config.SetPlayerTrackingDivisor(((CConfigPlayerTrackingField*)configField)->GetPlayerTrackingDivisor());
			configChangeType = ccDefaultDivider;
			break;
		case VIRTUAL_SESSIONS:
			currentValue = ((CConfigVirtualSessionField*)configField)->GetVirtualSessionNoPlayTimeout();
			previousValue = m_config.SetVirtualSessionNoPlayTimeout(((CConfigVirtualSessionField*)configField)->GetVirtualSessionNoPlayTimeout());
			if (currentValue != previousValue)
			{
				SendConfigChangeTransactionToSCF(ccVirtualNoPlayTimeout, previousValue, currentValue);
			}
			currentValue = ((CConfigVirtualSessionField*)configField)->GetVirtualSessionMaxDuration();
			previousValue = m_config.SetVirtualSessionMaxDuration(((CConfigVirtualSessionField*)configField)->GetVirtualSessionMaxDuration());
			configChangeType = ccVirtualSessionDuration;
			break;
		case CARD_TIMEOUT:
			currentValue = ((CConfigCardTimeoutField*)configField)->GetCardTimeout();
			previousValue = m_config.SetCardTimeout(((CConfigCardTimeoutField*)configField)->GetCardTimeout());
			configChangeType = ccPlayerCardTimeOut;
			break;
		case FLOOR_LOCK:
			{
				m_config.SetEGMLockoutState(((CConfigFloorLockField*)configField)->GetFloorLock());
				CheckIfEgmNeedsToBeLockedOrUnlocked();
			}
			break;
		case PBT:
			CConfigPBTField::SetPBTOptionsBuffer((byte *)&currentValue, sizeof(currentValue), ((CConfigPBTField*)configField)->PromoEnabled(), ((CConfigPBTField*)configField)->PointsEnabled(), ((CConfigPBTField*)configField)->CashEnabled());
			CConfigPBTField::SetPBTOptionsBuffer((byte *)&previousValue, sizeof(previousValue),
				m_config.SetPBTPromoEnabled_Global(((CConfigPBTField*)configField)->PromoEnabled()),
				m_config.SetPBTPointsEnabled_Global(((CConfigPBTField*)configField)->PointsEnabled()),
				m_config.SetPBTCashEnabled_Global(((CConfigPBTField*)configField)->CashEnabled()));
			configChangeType = ccPBEnabledTransfers;
			break;
		case CASH_TRANSFER_TRACKING:
			currentValue = ((CConfigCashTransferTrackingField*)configField)->CashTransferTracking();
			previousValue = m_config.SetCashTransferTracking(((CConfigCashTransferTrackingField*)configField)->CashTransferTracking());
			configChangeType = ccSendSessionDollars;
			break;
		case TIMEZONE:
			m_config.SetTimezoneOffset(((CConfigTimezoneField*)configField)->GetTimezoneOffset());
			m_config.SetUseDST(((CConfigTimezoneField*)configField)->UseDST());
			SetTimezone();
			break;
		case SLOTMAST_REVISION:
			{
				currentValue = ((CConfigSlotMastRevisionField*)configField)->GetSlotMastRevision();
				previousValue = m_config.SetSlotMastRevision(((CConfigSlotMastRevisionField*)configField)->GetSlotMastRevision());
				configChangeType = ccSlotMastRev;
				CInternalMessage* msg2 = new CInternalMessage(INT_MSG_SET_SLOTMAST_REVISION, m_config.GetSlotMastRevision());
				SendMessageToSCF(msg2, QUEUE_PRIORITY_NORMAL);
			}
			break;
		case ALLOW_FLOOR_LOCKOUT:
			currentValue = ((CConfigAllowFloorLockoutField*)configField)->GetAllowFloorLockout();
			previousValue = m_config.SetAllowFloorLockout(((CConfigAllowFloorLockoutField*)configField)->GetAllowFloorLockout());
			configChangeType = ccGlobalOptions;
			SendAllowFloorLockoutToGCF();
			break;
		case POINTS_FOR_PROMO:
			currentValue = ((CConfigPointsForPromoField*)configField)->GetAccruePointsForNCEP();
			previousValue = m_config.SetAccruePointsForNCEP(((CConfigPointsForPromoField*)configField)->GetAccruePointsForNCEP());
			if (currentValue != previousValue)
			{
				SendConfigChangeTransactionToSCF(ccAccPointsForNCEP, previousValue, currentValue);
			}
			currentValue = ((CConfigPointsForPromoField*)configField)->GetAccrueCompForNCEP();
			previousValue = m_config.SetAccrueCompForNCEP(((CConfigPointsForPromoField*)configField)->GetAccrueCompForNCEP());
			configChangeType = ccAccCompForNCEP;
			//allow code below to send config change transaction
			break;
		case SEND_FULL_METER_UPDATE:
			{
				bool newUpdateMMTState = ((CConfigUpdateMMTField*)configField)->GetUpdateMMT();

				//We only want to update the MMT the first time we get a true in the config.
				//This means that it takes one false in the config to reset our state
				if (newUpdateMMTState != m_updateMMT)
				{
					m_updateMMT = newUpdateMMTState;

					if (m_updateMMT)
					{
						LogString(GAMES, "[%s]: Trigering to send 304/305 messages after receiving "
							"401 Config response (RefreshSubGameMeters).", __FUNCTION__);

						// Send 304 EGM game meters message
						LogString(GAMES, "[%s]: Sending 304 EGM meters to poller.",
							__FUNCTION__);
						SendGameMetersToSCF(m_games.GetGame(EGM_GAME_NUMBER));

						// Send 305 Subgame meters messages for all subgames in a separate thread
						LogString(GAMES, "[%s]: Starting thread to send subgame meters to poller.", 
							__FUNCTION__);
						StartSubgameMetersUpdateThread();
					}
				}
			}
			break;
		case UNIVERSE_ENABLED:
			currentValue = ((CUniverseEnabledField*)configField)->GetUniverseEnabled();
			previousValue = m_config.SetUniverseEnabled(((CUniverseEnabledField*)configField)->GetUniverseEnabled());
			if (currentValue != previousValue)
			{
				SendConfigChangeTransactionToSCF(ccUniverseEnabled, previousValue, currentValue);
			}
			currentValue = ((CUniverseEnabledField*)configField)->GetMixedModeEnabled();
			previousValue = m_config.SetMixedModeEnabled(((CUniverseEnabledField*)configField)->GetMixedModeEnabled());
			configChangeType = ccMixedMode;
			refreshConfigs = true;
			break;
		case UNIVERSE_PLAYER_PREFIXES:
			m_config.SetUniversePrefixes(((CConfigUniverseCardPrefixField*)configField)->GetUniverseCardPrefixes());
			refreshConfigs = true;
			break;
		case UNIVERSE_PROP_CODES:
			m_config.SetUniversePropCodes(((CConfigUniversePropCodesField*)configField)->GetUniversePropCodes());
			refreshConfigs = true;
			break;
		case FEATURE_FLAGS:
			{
				previousValue = m_config.GetFeatureFlagsRaw();
				SetFeatureFlags((CConfigFeatureFlagField*)configField);
				currentValue = m_config.GetFeatureFlagsRaw();
				configChangeType = ccFeatureFlags;
			}
			break;
		case NCL_ENABLED_CARD_TRACK:
			// Process NCL cards enabled sub-field.
			currentValue = ((CNclEnabledCardTrackField*)configField)->GetNclEnabled();
			previousValue = m_config.SetNclCardsEnabled(((CNclEnabledCardTrackField*)configField)->GetNclEnabled());
			if (currentValue != previousValue)
			{
				SendConfigChangeTransactionToSCF(ccNCLEnabled, previousValue, currentValue);
			}

			// Process card reader track number sub-field.
			currentValue = ((CNclEnabledCardTrackField*)configField)->GetCardTrack();
			previousValue = m_config.SetCardReaderTrackNumber(((CNclEnabledCardTrackField*)configField)->GetCardTrack());
			configChangeType = ccCardTrack;
			refreshConfigs = true;
			break;
		case SHOW_POINTS_VS_ACCOUNT_BALANCE:
			m_config.SetShowPointsVsAccountBalance(((const CConfigShowPointsVsAccountBalanceField*)configField)->GetShowPointsVsAccountBalance());
			break;
		case SESSION_POINTS_TEXT:
			m_config.SetSessionPointsMessage(((const CConfigSessionPointsTextField*)configField)->GetSessionPointsText().c_str());
			break;
		case POINTS_OR_CASH_LABEL:
			m_config.SetPointsOrCashLabel(((const CConfigPointsOrCashLabelField*)configField)->GetPointsOrCashLabel().c_str());
			break;
		case BALANCE_RETRIEVAL_TEXT:
			m_config.SetBalanceRetrievalMessage(((const CConfigBalanceRetrievalTextField*)configField)->GetBalanceRetrievalText().c_str());
			break;
		case DAILY_POINTS_DISPLAY_SUFFIX:
			m_config.SetDailyPointsDisplaySuffixMessage(((const CConfigDailyPointsDisplaySuffixField*)configField)->GetDailyPointsDisplaySuffixText().c_str());
			break;
		case TIER_POINTS_LABEL:
			m_config.SetTierPointsLabel(((const CConfigTierPointsLabelField*)configField)->GetTierPointsLabel().c_str());
			break;
		case PLAYER_PROFILE_BUTTON:
			currentValue = ((CConfigPlayerProfileButtonField*)configField)->DisplayPlayerProfileButton();
			previousValue = m_config.SetDisplayPlayerProfileButton(((CConfigPlayerProfileButtonField*)configField)->DisplayPlayerProfileButton());
			configChangeType = ccPlayerProfileButton;
			break;
		case GLOBAL_MGMD_ENABLED:
			currentValue = ((CConfigMGMDField*)configField)->IsGlobalMGMD();
			previousValue = m_config.SetGlobalMGMD(((CConfigMGMDField*)configField)->IsGlobalMGMD());
			configChangeType = ccGlobalMGMD;
			break;
		case EXCESSIVE_VOUCHER_OUT_ENABLED:
			currentValue = ((CConfigExcessiveVoucherOutEnabledField*)configField)->IsEnabled();
			previousValue = m_config.SetGlobalExcessiveVoucherOutEnabledFlag(((CConfigExcessiveVoucherOutEnabledField*)configField)->IsEnabled());
			m_excessiveVoucherOut.SetGlobalExcessiveVoucherOutEnabledFlag(m_config.GetGlobalExcessiveVoucherOutEnabledFlag());
			configChangeType = ccExcessiveVoucherOutEnabled;
			break;
		case EXCESSIVE_VOUCHER_OUT_THRESHOLD:
			currentValue = ((CConfigExcessiveVoucherOutThresholdField*)configField)->GetThreshold();
			previousValue = m_config.SetExcessiveVoucherOutThreshold(((CConfigExcessiveVoucherOutThresholdField*)configField)->GetThreshold());
			m_excessiveVoucherOut.SetExcessiveVoucherOutThreshold(m_config.GetExcessiveVoucherOutThreshold());
			configChangeType = ccExcessiveVoucherOutThreshold;
			break;
		case TOTAL_BILL_IN_THRESHOLD:
			currentValue = ((CConfigExcessiveVoucherOutThresholdField*)configField)->GetThreshold();
			previousValue = m_config.SetTotalBillInThreshold(((CConfigExcessiveVoucherOutThresholdField*)configField)->GetThreshold());
			m_excessiveVoucherOut.SetTotalBillInThreshold(m_config.GetTotalBillInThreshold());
			configChangeType = ccTotalBillInThreshold;
			break;
		case COIN_IN_THRESHOLD:
			currentValue = ((CConfigExcessiveVoucherOutThresholdField*)configField)->GetThreshold();
			previousValue = m_config.SetCoinInThreshold(((CConfigExcessiveVoucherOutThresholdField*)configField)->GetThreshold());
			m_excessiveVoucherOut.SetCoinInThreshold(m_config.GetCoinInThreshold());
			configChangeType = ccCoinInThreshold;
			break;
		case COIN_OUT_THRESHOLD:
			currentValue = ((CConfigExcessiveVoucherOutThresholdField*)configField)->GetThreshold();
			previousValue = m_config.SetCoinOutThreshold(((CConfigExcessiveVoucherOutThresholdField*)configField)->GetThreshold());
			m_excessiveVoucherOut.SetCoinOutThreshold(m_config.GetCoinOutThreshold());
			configChangeType = ccCoinOutThreshold;			
			break;
		case PARTIAL_WAT_OUT_ENABLED:
			currentValue = ((CConfigPartialWATOutEnabledField*)configField)->IsEnabled();
			previousValue = m_config.SetPartialWATOutEnabled_Global(((CConfigPartialWATOutEnabledField*)configField)->IsEnabled());
			configChangeType = ccPartialWATOutEnabled;
			break;			
		case EXTENSIBLE_SYSTEM_SETTINGS:
		{
			poller_12_11_6000_or_Higher = true;

			currentValue = ((CConfigExtensibleSystemSettingsField*)configField)->AutoUploadCredits();
			previousValue = m_config.SetAutoUploadCreditsGlobal(((CConfigExtensibleSystemSettingsField*)configField)->AutoUploadCredits());
			if (currentValue != previousValue)
			{
				SendConfigChangeTransactionToSCF(ccAutoUploadCreditsEnabled, previousValue, currentValue);
			}

			currentValue = ((CConfigExtensibleSystemSettingsField*)configField)->CashoutToHost();
			previousValue = m_config.SetCashoutToHostEnabled_Global(((CConfigExtensibleSystemSettingsField*)configField)->CashoutToHost());
			if (currentValue != previousValue)
			{
				SendConfigChangeTransactionToSCF(ccCashoutToHostEnabled, previousValue, currentValue);
			}
						
			currentValue = ((CConfigExtensibleSystemSettingsField*)configField)->GetTicketInResend();
			previousValue = m_config.SetTicketInResend(((CConfigExtensibleSystemSettingsField*)configField)->GetTicketInResend());
			if (currentValue != previousValue)
			{
				SendConfigChangeTransactionToSCF(ccTicketInResend, previousValue, currentValue);
			}

			currentValue = ((CConfigExtensibleSystemSettingsField*)configField)->GetEnableLocationServices();
			previousValue = m_config.SetEnableLocationServices(((CConfigExtensibleSystemSettingsField*)configField)->GetEnableLocationServices());
#ifdef _DEBUG
			LogString(POLLER_LOG, "enableLocationServices update from 401 config message: currentValue=%d, previousValue=%d",
									currentValue, previousValue);
#endif
			if (currentValue != previousValue)
			{
				SendConfigChangeTransactionToSCF(ccEnableLocationServices, previousValue, currentValue);
			}

			// Issue a call for ProximitySettingsRequest - we can't be sure that we didn't miss beacon active marker while offline
			// Therefore we send the request always when EnableLocationServices is on.
			// Issue a call for ProximityBeaconScanListRequest whenever EnableLocationServices is set true
			if (currentValue)
			{
				LogString(POLLER_LOG, "EnableLocationServices is on. Sending mobile.request.proximitySettings from SCF");
				ProcessCallProximitySettingsRequest();

				/* Do not trigger ProximityBeaconScanListRequest until EGM movement tracking feature is implemented and enabled E2E
				LogString(PROXIMITY_LOG, "EnableLocationServices is on. Triggering ProximityBeaconScanListRequest from SCF");
				ProcessCallProximityRequestBeaconScanList(); */
			}
			else
			{
				// When flag is off, beacon should stop transmitting iBeacon frame accordingly.
				LogString(BEACON_LOG, "Disabling IBeacon frame due to extensible setting EnableLocationServices being False");
				m_beaconManager->InitializeBeaconFrames(EDDYSTONE_FRAME);
			}

			currentValue = ((CConfigExtensibleSystemSettingsField*)configField)->UJPTaxWithholding();
			previousValue = m_config.SetUJPTaxWithholding_Global(((CConfigExtensibleSystemSettingsField*)configField)->UJPTaxWithholding());
			if (currentValue != previousValue)
			{
				SendConfigChangeTransactionToSCF(ccUJPTaxWithholdingEnabled, previousValue, currentValue);
			}

			currentValue = ((CConfigExtensibleSystemSettingsField*)configField)->GetFederalTaxWithholdingPercent();
			previousValue = m_config.SetFederalTaxWithholdingPercent(((CConfigExtensibleSystemSettingsField*)configField)->GetFederalTaxWithholdingPercent());
			if (currentValue != previousValue)
			{
				SendConfigChangeTransactionToSCF(ccPercentFederalTax, previousValue, currentValue);
			}

			currentValue = ((CConfigExtensibleSystemSettingsField*)configField)->GetStateTaxWithholdingPercent();
			previousValue = m_config.SetStateTaxWithholdingPercent(((CConfigExtensibleSystemSettingsField*)configField)->GetStateTaxWithholdingPercent());
			if (currentValue != previousValue)
			{
				SendConfigChangeTransactionToSCF(ccPercentStateTax, previousValue, currentValue);
			}

			// Since we don't store poller load balancing time in config, so don't send config change Tx.
			((CConfigExtensibleSystemSettingsField*)configField)->GetPollerLoadBalanceTime(m_PollerLoadBalanceTime_Hour, m_PollerLoadBalanceTime_Minute);

			// Since we don't store meter rollover config values in config, so don't send config change Tx.
			bool meterRolloverSyncEnabled;
			BYTE meterRolloverTimeHour,  meterRolloverTimeMinute, meterRolloverTimeSecond;
			((CConfigExtensibleSystemSettingsField*)configField)->GetMeterRolloverTime(
				meterRolloverSyncEnabled,meterRolloverTimeHour, meterRolloverTimeMinute, meterRolloverTimeSecond);
			m_sentinelState.SetMeterRolloverConfig(
				meterRolloverSyncEnabled, meterRolloverTimeHour, meterRolloverTimeMinute, meterRolloverTimeSecond);

			// Check if on-demand load balancing reconnect to the poller is requested
			if (((CConfigExtensibleSystemSettingsField*)configField)->PollerReconnect())
			{
				if (m_playerSession.GetPlayerTypeEnum() == PlayerTypeState::PLAYERTYPEIDLE)
				{
					LogString(POLLER_LOG, "[LB] Performing on-demand load-balancing reconnect to the poller.");
					// Load balancing request accepted
					GenerateTilt(tltPollerLoadBalanceReconnect);
					SendMessageToSCF(new CInternalMessage(INT_MSG_PERFORM_LOAD_BALANCING), QUEUE_PRIORITY_HIGHEST);
				}
				else
				{
					// Load balancing request denied
					GenerateTilt(tltPollerLoadBalanceReconnectDenied);
					LogString(POLLER_LOG, "[LB] Skipped on-demand load-balancing reconnect to the poller due to an active session.");
				}
			}

			// Check if sentinel maintenance mode is requested
			if (((CConfigExtensibleSystemSettingsField*)configField)->MaintenanceMode())
			{
				LogString(GAMES, "[%s]: Refreshing maintenance mode", __FUNCTION__);
				m_maintenanceMode = true;
				m_lastMaintenanceModeTickCount = CUtilities::GetTickCount();
			}

			CSmartDataLogger::Instance().SetCombineZipLogs(((CConfigExtensibleSystemSettingsField*)configField)->CombineZipLogs());

			currentValue = ((CConfigExtensibleSystemSettingsField*)configField)->GetAutoCardOutSecToCountdownCredits();
			previousValue = m_config.SetAutoCardOutSecToCountdownCredits(((CConfigExtensibleSystemSettingsField*)configField)->GetAutoCardOutSecToCountdownCredits());
			if (currentValue != previousValue)
			{
				SendConfigChangeTransactionToSCF(ccAutoCardOutSecToCountdownCredits, previousValue, currentValue);
			}

			currentValue = ((CConfigExtensibleSystemSettingsField*)configField)->GetAutoCardOutSecToCountdownNoCrdts();
			previousValue = m_config.SetAutoCardOutSecToCountdownNoCrdts(((CConfigExtensibleSystemSettingsField*)configField)->GetAutoCardOutSecToCountdownNoCrdts());
			if (currentValue != previousValue)
			{
				SendConfigChangeTransactionToSCF(ccAutoCardOutSecToCountdownNoCrdts, previousValue, currentValue);
			}

			currentValue = ((CConfigExtensibleSystemSettingsField*)configField)->GetAutoCardOutCountdownSeconds();
			previousValue = m_config.SetAutoCardOutCountdownSeconds(((CConfigExtensibleSystemSettingsField*)configField)->GetAutoCardOutCountdownSeconds());
			if (currentValue != previousValue)
			{
				SendConfigChangeTransactionToSCF(ccAutoCardOutCountdownSeconds, previousValue, currentValue);
			}

			currentValue = ((CConfigExtensibleSystemSettingsField*)configField)->GetAutoCardOutSecToAlert();
			previousValue = m_config.SetAutoCardOutSecToAlert(((CConfigExtensibleSystemSettingsField*)configField)->GetAutoCardOutSecToAlert());
			if (currentValue != previousValue)
			{
				SendConfigChangeTransactionToSCF(ccAutoCardOutSecToAlert, previousValue, currentValue);
			}

			m_personalBanker->SetAutoUploadDelayValue(((CConfigExtensibleSystemSettingsField*)configField)->GetAutoUploadDelaySeconds());

			currentValue = ((CConfigExtensibleSystemSettingsField*)configField)->GetNumberOfSubgamesMismatchCheck();
			previousValue = m_config.SetNumberOfSubgamesMismatchCheck(((CConfigExtensibleSystemSettingsField*)configField)->GetNumberOfSubgamesMismatchCheck());
			if (currentValue != previousValue)
			{
				SendConfigChangeTransactionToSCF(ccNumberOfSubgamesMismatchCheck, previousValue, currentValue);
			}

			currentValue = ((CConfigExtensibleSystemSettingsField*)configField)->GetCountdownDisplayIntervalSeconds();
			previousValue = m_config.SetCountdownDisplayIntervalInSeconds(((CConfigExtensibleSystemSettingsField*)configField)->GetCountdownDisplayIntervalSeconds());
			if (currentValue != previousValue)
			{
				SendConfigChangeTransactionToSCF(ccCountdownDisplayIntervalInSec, previousValue, currentValue);
			}

			currentValue = ((CConfigExtensibleSystemSettingsField*)configField)->GetPointsCountdownControlledByUniverse();
			previousValue = m_config.SetPointsCountdownControlByUniverse(((CConfigExtensibleSystemSettingsField*)configField)->GetPointsCountdownControlledByUniverse());
			if (currentValue != previousValue)
			{
				SendConfigChangeTransactionToSCF(ccPointsCountdownControlByUniverse, previousValue, currentValue);
			}

			m_config.SetnConnectUrl(((CConfigExtensibleSystemSettingsField*)configField)->GetnConnectUrl());
			m_config.SetnConnectToken(((CConfigExtensibleSystemSettingsField*)configField)->GetnConnectToken());
			m_config.SetAvatarUrl(((CConfigExtensibleSystemSettingsField*)configField)->GetAvatarUrl());
			m_config.SetAvatarToken(((CConfigExtensibleSystemSettingsField*)configField)->GetAvatarToken());
			m_config.SetRequestServiceUrl(((CConfigExtensibleSystemSettingsField*)configField)->GetRequestServiceUrl());
			m_config.SetRequestServiceToken(((CConfigExtensibleSystemSettingsField*)configField)->GetRequestServiceToken());
			m_config.SetSocialDistanceUrl(((CConfigExtensibleSystemSettingsField*)configField)->GetSocialDistanceUrl());
			m_config.SetSocialDistanceToken(((CConfigExtensibleSystemSettingsField*)configField)->GetSocialDistanceToken());
			m_config.SetDrinksOnTapUrl(((CConfigExtensibleSystemSettingsField*)configField)->GetDrinksOnTapUrl());
			m_config.SetDrinksOnTapToken(((CConfigExtensibleSystemSettingsField*)configField)->GetDrinksOnTapToken());
			m_config.SetSocialDistanceCountdownDelay(((CConfigExtensibleSystemSettingsField*)configField)->GetSocialDistanceCountdownDelay());
			m_config.SetSocialDistanceIgnoreDirty(((CConfigExtensibleSystemSettingsField*)configField)->GetSocialDistanceIgnoreDirty());
			m_config.SetSocialDistanceManualCleanEnabled(((CConfigExtensibleSystemSettingsField*)configField)->GetSocialDistanceManualCleanEnabled());

			// Check if on-demand log archiving is requested by passing log description text.
			std::string description = ((CConfigExtensibleSystemSettingsField*)configField)->GetArchiveLogDescriptionStr();
			if (!description.empty())
			{
				TiltCode tiltCode(CSmartDataLogger::Instance().WriteLogsToMediaDisk(ZipLogActionType::ArchiveFromFLM, "", description));
				if (tltUnknown != tiltCode)
				{
					GenerateTilt(tiltCode);
				}
			}

			currentValue = ((CConfigExtensibleSystemSettingsField*)configField)->GetRebootAsapThresholdInMb();
			previousValue = m_config.SetRebootAsapThresholdInMb(((CConfigExtensibleSystemSettingsField*)configField)->GetRebootAsapThresholdInMb());
			if (currentValue != previousValue)
			{
				SendConfigChangeTransactionToSCF(ccRebootAsapThresholdInMb, previousValue, currentValue);
			}

			currentValue = ((CConfigExtensibleSystemSettingsField*)configField)->GetRebootWarningThresholdMb();
			previousValue = m_config.SetRebootWarningThresholdMb(((CConfigExtensibleSystemSettingsField*)configField)->GetRebootWarningThresholdMb());
			if (currentValue != previousValue)
			{
				SendConfigChangeTransactionToSCF(ccRebootWarningThresholdMb, previousValue, currentValue);
			}

			currentValue = ((CConfigExtensibleSystemSettingsField*)configField)->GetRestartUIAsapThresholdMb();
			previousValue = m_config.SetRestartUIAsapThresholdMb(((CConfigExtensibleSystemSettingsField*)configField)->GetRestartUIAsapThresholdMb());
			if (currentValue != previousValue)
			{
				SendConfigChangeTransactionToSCF(ccRestartUIAsapThresholdMb, previousValue, currentValue);
			}

			currentValue = ((CConfigExtensibleSystemSettingsField*)configField)->GetRestartUIAfterDropThresholdMb();
			previousValue = m_config.SetRestartUIAfterDropThresholdMb(((CConfigExtensibleSystemSettingsField*)configField)->GetRestartUIAfterDropThresholdMb());
			if (currentValue != previousValue)
			{
				SendConfigChangeTransactionToSCF(ccRestartUIAfterDropThresholdMb, previousValue, currentValue);
			}

			currentValue = ((CConfigExtensibleSystemSettingsField*)configField)->GetRestartUIDelayHours();
			previousValue = m_config.SetRestartUIDelayHours(((CConfigExtensibleSystemSettingsField*)configField)->GetRestartUIDelayHours());
			m_restartManager.SetRestartUIDelayHours(m_config.GetRestartUIDelayHours());
			if (currentValue != previousValue)
			{
				SendConfigChangeTransactionToSCF(ccRestartUIDelayHours, previousValue, currentValue);
			}

			currentValue = ((CConfigExtensibleSystemSettingsField*)configField)->GetRestartUIAfterxDays();
			previousValue = m_config.SetRestartUIAfterxDays(((CConfigExtensibleSystemSettingsField*)configField)->GetRestartUIAfterxDays());
			m_restartManager.SetRestartUIAfterxDays(m_config.GetRestartUIAfterxDays());
			if (currentValue != previousValue)
			{
				SendConfigChangeTransactionToSCF(ccRestartUIAfterxDays, previousValue, currentValue);
			}

			currentValue = ((CConfigExtensibleSystemSettingsField*)configField)->GetSASReadTimeOutMultiplier();
			previousValue = m_config.SetSASReadTimeOutMultiplier(((CConfigExtensibleSystemSettingsField*)configField)->GetSASReadTimeOutMultiplier());
			if (currentValue != previousValue)
			{
				SendConfigChangeTransactionToSCF(ccReadTimeOutMultiplier, previousValue, currentValue);
				SendMessageToGCF(new CInternalMessage(INT_MSG_UPDATE_SAS_READ_TIMEOUT), QUEUE_PRIORITY_HIGHEST);
			}

			currentValue = ((CConfigExtensibleSystemSettingsField*)configField)->UseSDSCardFormat();
			previousValue = m_config.SetUseSDSCardFormat(((CConfigExtensibleSystemSettingsField*)configField)->UseSDSCardFormat());
			if (currentValue != previousValue)
			{
				SendConfigChangeTransactionToSCF(ccUseSDSCardFormat, previousValue, currentValue);
			}

			m_config.SetExtensibleSystemSettings(((CConfigExtensibleSystemSettingsField*)configField)->GetExtensibleSettingsStr());
		}
			break;
		default:
			break;
		}

		if (configChangeType != ccNone && previousValue != currentValue)
		{
			SendConfigChangeTransactionToSCF(configChangeType, previousValue, currentValue);
		}
		if (configChangeType == ccGlobalMGMD)
		{
			CInternalMessage* msg = new CInternalMessage(INT_MSG_TRYSENDGAMELIST_GLOBAL_MGMD, (WORD)m_config.GetGlobalMGMDEnabled());
			SendMessageToSCF(msg, QUEUE_PRIORITY_NORMAL);
		}
	}

	if (refreshConfigs)
	{
		RefreshConfigs();
	}

	if (gotFullConfig)
	{
		m_config.SetPoller_12_11_6000_or_Higher(poller_12_11_6000_or_Higher);
		if (!poller_12_11_6000_or_Higher)
		{
			m_config.SetExtensibleSystemSettings("");
		}

		// Only update the last FloorLogix configuration time for a "full" configuration message.
		m_sentinelState.m_timeOfLastFloorLogixConfig = CUtilities::GetCurrentTime();
		m_sentinelState.m_timeOfLastFloorLogixConfigAvailable = true;
	}

	SetPlayerConfig();
	SetTicketsConfig();
	SendTicketingInfoToGCF();

	// Cashout to host feature enable/disable state might have changed due to new global and sentinel 
	// options received. Send the notification to GCF, it will do no-op if EGM is already in that state.
	SendEnableCashoutToHostToGCF(m_config.CashoutToHostEnabled());

	// We may be in a 301/401 handshake due to poller reconnect which may happen due to 
	// network jitter or DHCP lease refresh also. In case this resulted in nCompass IP 
	// change also, send updated IP so that immediate 303 sent to poller has that reflected.
	string ip;
	bool ipChanged(false);
	{
		lock_guard<recursive_mutex> lock(m_adapterCriticalSection);
		m_config.GetActualIP().GetIP(ip);
		if (m_sentinelIp != ip)
		{
			LogString(NETWORKING_LOG, "[CInternalsFramework::ProcessSystemConfigFromSCF] Sentinel IP change detected."
				"Old: [%s] New: [%s]", m_sentinelIp.c_str(), ip.c_str());
			m_sentinelIp = ip;
			ipChanged = true;
		}
	}

	// Send a message to send status to poller
	SendMessageToSCF(new CInternalMessage(INT_MSG_SEND_STATUS_AFTER_PARSING_CONFIG,
		(ipChanged ? m_sentinelIp : "")), QUEUE_PRIORITY_NORMAL);

	// We need to ask the SCF how many tickets are pending. 
	// When the response is received, it will go to the GCF.
	SendTicketOutPendingRequestToSCF();

	SendMessageToSCF(new CInternalMessage(INT_MSG_EXTENSIBLE_SETTINGS, (DWORD)m_config.GetExtensibleSettings()), QUEUE_PRIORITY_HIGHEST);

	// Send the connection data to the progressive once we have a config.
	CInternalMessage *newMsg = CreateConnectionDataMessage();
	SendMessageToProgressive(newMsg, QUEUE_PRIORITY_NORMAL);

	// Update the text toggle interval in Notification Box based on countdown display interval.
	m_notificationBox.SetTextToggleInterval(m_config.GetCountdownDisplayIntervalInSeconds() * MILLISECONDS_PER_SECOND / 2);
}

void CInternalsFramework::ProcessSetTimeFromSCF(CInternalMessage *msg)
{
	// Get the system time to set
	size_t size;
	SYSTEMTIME *receivedSystemTime = (SYSTEMTIME *)msg->GetBytesData(size);

	// Set the system time on the unit
	if ( receivedSystemTime != nullptr )
	{
		bool setNewTime = true;

		// Translate the received UTC SYSTEMTIME to a local time.
		time_t receivedLocalTime(CUtilities::GetTimeFromUTCSystemTime(*receivedSystemTime));

		// Determine how long it took to receive this time.
		DWORD oldTickCount = msg->GetDWordData(false);
		DWORD newTickCount = CUtilities::GetTickCount();
		if ( oldTickCount < newTickCount )
		{
			receivedLocalTime += ((newTickCount - oldTickCount) / MILLISECONDS_PER_SECOND);
		}
		else
		{
			// time must have rolled over, use receivedTime as is
		}

		time_t currentLocalTime = CUtilities::GetCurrentTime();
		time_t differenceInTime(receivedLocalTime - currentLocalTime);

		bool doesPlayerExist(DoesPlayerExist());
		if (doesPlayerExist || abs(differenceInTime) <= TimeDriftThresholdSeconds)
		{
			setNewTime = false;
		}

		LogString(GENERAL, "Time difference doesPlayerExist=%d currentLocalTime=%s receivedLocalTime=%s differenceInTime=%ld",
			doesPlayerExist,
			CUtilities::FormatDateTimeString(currentLocalTime).c_str(),
			CUtilities::FormatDateTimeString(receivedLocalTime).c_str(),
			differenceInTime);

		if (setNewTime)
		{
			SYSTEMTIME utcSystemTime = CUtilities::GetUTCSystemTime(receivedLocalTime);
			// Set the RTC
			if (receivedLocalTime >= LOCAL_CERT_TIME_ACTIVATION)
			{
				time_t previousTime(CUtilities::GetCurrentTime());
				S7_Result rtcStatus = S7LITE_Rtc_SetTime(utcSystemTime);
				if (rtcStatus != S7DLL_STATUS_OK)
				{
					LogString(ERROR_LOG, "Could not set real-time clock");
				}
				else
				{
					LogString(GENERAL, "SetSystemTime GetCurrentTime=%s receivedLocalTime=%s previousLocalTime=%s difference=%ld", 
						CUtilities::FormatDateTimeString(CUtilities::GetCurrentTime()).c_str(),
						CUtilities::FormatDateTimeString(receivedLocalTime).c_str(),
						CUtilities::FormatDateTimeString(previousTime).c_str(),
						receivedLocalTime - previousTime);
					m_config.SetSystemTime(utcSystemTime);
				}

				const DWORD ClockVariationAllowanceSeconds = 30;
				double timeDeltaSeconds = abs(difftime(receivedLocalTime, previousTime));
				bool timeMovedBackMorethanAllowableSeconds(receivedLocalTime < previousTime && timeDeltaSeconds > ClockVariationAllowanceSeconds);
				// If the clock was set back a significant amount of time, signal all the binsem objects
				//  so they do not cause a thread timeout.
				if (timeMovedBackMorethanAllowableSeconds)
				{
					LogString(GENERAL, "Abnormal time setback detected. Signaling all binsem semaphores.");
					binsem::SignalAllBinSems(true);
				}
			}
			else
			{
				LogString(ERROR_LOG, "Received an invalid time from FloorLogix %d/%d/%d  %d:%d:%d.  Anything before 05/30/2019 19:54:20 cannot be used to set the system time.",
					utcSystemTime.wMonth, utcSystemTime.wDay, utcSystemTime.wYear, utcSystemTime.wHour, utcSystemTime.wMinute, utcSystemTime.wSecond);
			}
		}

		// Tell the UI that this is the first time that the time is being set.
		// Tracker 027499.
		if (!m_firstTimeSet)
		{
			m_firstTimeSet = true;
			CSmartDataLogger::Instance().FistTimeSetFromPoller();
		}

		m_sentinelState.m_tickCountOfLastFloorLogixTimeUpdate = CUtilities::GetTickCount();
		m_sentinelState.m_tickCountOfLastFloorLogixTimeUpdateAvailable = true;
	}

	// Tell the GCF to set the time on the game.
	SendMessageToGCF(msg, QUEUE_PRIORITY_HIGH);
}

void CInternalsFramework::ProcessSetDisplayMessageFromSCF( CInternalMessage *msg )
{
	DisplayMessageType displayType = (DisplayMessageType)msg->GetWordData();

	switch ( displayType )
	{
	case DISP_MSG_BAD_CARD_READ:
		m_config.SetBadCarReadMessage(msg->GetStringData().c_str());
		break;
	case DISP_MSG_GOOD_CARD_READ:
		m_config.SetGoodCardReadMessage(msg->GetStringData().c_str());
		break;
	case DISP_MSG_CASINO_NAME:
		m_config.SetCasinoName(msg->GetStringData().c_str());
		break;
	case DISP_MSG_ATTRACTOR:
		m_config.SetAttractorMessage(msg->GetStringData().c_str());
		break;
	case DISP_MSG_CONGRATS:
		m_config.SetCongratulationsMessage(msg->GetStringData().c_str());
		break;
	case DISP_MSG_BIRTHDAY:
		m_config.SetBirthdayMessage(msg->GetStringData().c_str());
		break;
	case DISP_MSG_ANNIVERSARY:
		m_config.SetAnniversaryMessage(msg->GetStringData().c_str());
		break;
	case DISP_MSG_WELCOME:
		m_config.SetWelcomeMessage(msg->GetStringData().c_str());
		break;
	case DISP_MSG_ADDRESS1:
		m_config.SetCasinoAddress1(msg->GetStringData().c_str());
		break;
	case DISP_MSG_ADDRESS2:
		m_config.SetCasinoAddress2(msg->GetStringData().c_str());
		break;
	}
}

void CInternalsFramework::ProcessSetGlobalPasswordFromSCF( CInternalMessage *msg )
{
	m_config.SetGlobalCardPassword( msg->GetWordData() );
}

void CInternalsFramework::ProcessSetSlotMastIDFromSCF( CInternalMessage *msg )
{
	SlotmastIDResponse response = (SlotmastIDResponse) msg->GetWordData();

	m_config.SetPendingSlotmast( INVALID_SLOT_MAST_ID );

	SentinelOnlineState onlineState = m_EGMConfig.GetSentinelOnlineState();

	m_EGMConfig.SetSlotmastIDResponse(response);

	// If the slotmast request was successful
	if ( response == CHANGE_SUCCESS)
	{
		long newSlotMast = msg->GetDWordData();

		SendConfigChangeTransactionToSCF( ccSlotMastID, m_config.GetSlotMastID(), newSlotMast );

		m_config.SetSlotMastID( newSlotMast );

		CSmartDataLogger::Instance().SetSlotMastId(newSlotMast);

		// Starting the GCF will do a meter capture
		StartGCF();
	}

	// If the slotmast request was not successful, allow the user to online again.
	else if ( onlineState == sentinelComingOnline )
	{
		m_EGMConfig.SetSentinelOnlineState( sentinelOffline );
	}

	SendSubscriptionUpdateIfNecessary();
}

/// <summary>
/// Process player information that came back from the poller.
/// If player is banned from play, the EGM will be locked.
/// </summary>
/// <param name="msg">Contains player information that came back from the poller</param>
void CInternalsFramework::ProcessPlayerInfoFromSCF( CInternalMessage *msg )
{
	CPlayerInfo *playerInfo = msg->GetPlayerInfoData();
	if ( playerInfo != NULL )
	{
		ProcessPlayerInfo( playerInfo );
			
		// This is so the player data does not get deleted with the message.
		msg->SetData( (CPlayerInfo *)NULL );

		CheckIfEgmNeedsToBeLockedOrUnlocked();
	}
}

void CInternalsFramework::ProcessPlayerGlobalBalanceFromSCF( CInternalMessage *msg )
{
	CPlayerGlobalBalance *globalBalance = msg->GetPlayerGlobalBalanceData();
	if( globalBalance != NULL )
	{
		ProcessPlayerGlobalBalance( globalBalance );
		// This is so the global balance data does not get deleted with the message.
        // I'm not sure why we care at this point since this data goes stale almost immediately.
		msg->SetData( (CPlayerGlobalBalance *)NULL );
	}
}

void CInternalsFramework::ProcessPlayerBonusEventsFromSCF( CInternalMessage *msg )
{
	CBonusEvents *bonusEvents = msg->GetBonusEventsData();
	if ( bonusEvents != NULL )
	{
		SetPlayerBonusEvents( bonusEvents );

		// This is so the bonus event data does not get deleted with the message.
		msg->SetData( (CBonusEvents *)NULL );
	}
}

void CInternalsFramework::ProcessPlayerUpdateTimeFromSCF( CInternalMessage *msg )
{
	lock_guard<recursive_mutex> lock(m_playerCriticalSection);
	time_t *playerUpdateTime(msg->GetTimeData());
	if (nullptr != playerUpdateTime  && nullptr != m_player)
	{
		m_playerUpdateManager.AddPlayerUpdateTime(new time_t(*playerUpdateTime));
	}
	else if(m_player == nullptr)
	{
		LogString(GENERAL, "Received player update without a player.");
	}
}

void CInternalsFramework::ProcessTicketStoredFromSCF( CInternalMessage *msg )
{
	CTicket *ticket = msg->GetTicketData();
	if ( ticket != NULL )
	{
		LogString(TICKETING_LOG, "ProcessTicketStoredFromSCF");
		ProcessTicketStored( *ticket );
	}
}

void CInternalsFramework::ProcessTicketOutPendingCountFromSCF( CInternalMessage *msg )
{
	WORD pendingCount = msg->GetWordData();

	SetTicketOutPendingCount( pendingCount );
}

void CInternalsFramework::ProcessTicketResponseFromSCF( CInternalMessage *msg )
{
	CTicket *ticket = msg->GetTicketData();
	if ( ticket != NULL )
	{
		DWORD scfTicketID = msg->GetDWordData();

		ProcessTicketResponse( *ticket, scfTicketID );
	}
}

void CInternalsFramework::ProcessEmployeeResponseFromSCF( CInternalMessage *msg )
{
	string pin;
	
	pin = FormatString( "%4.4d", msg->GetDWordData() );

	size_t cardIDLen;
	byte *cardID = (byte *)msg->GetBytesData( cardIDLen );

	ProcessEmployeePin( cardID, cardIDLen, pin );
}

void CInternalsFramework::ProcessPersonalBankerTransactionAckedFromSCF( CInternalMessage *msg )
{
	WORD sequenceNumber = msg->GetWordData();

	ProcessPersonalBankerTransactionAcked( sequenceNumber );
}

void CInternalsFramework::ProcessPersonalBankerAuthorizationFromSCF( CInternalMessage *msg, bool granted )
{
	ProcessPersonalBankerAuthorization(msg->GetStringData(), granted);
}

void CInternalsFramework::ProcessPBResponseFromSCF( CInternalMessage *msg )
{
	CPersonalBankerInfo *pbInfo = msg->GetPBInfoData();
#ifdef _DEBUG
	LogString(POLLER_LOG, "%s %d: PBT Cash Balance = %u, Cash upload limit = %u, Cash download limit = %u.",
		__FUNCTION__, __LINE__, pbInfo->GetCashBalance(), pbInfo->GetMaximumCashUp(), pbInfo->GetMaximumCashDown());
#endif

	if (pbInfo != nullptr)
	{
		if( pbInfo->IsBannedFromRedemptions() )
		{
			pbInfo->SetErrorCode(InvalidID);
		}

		lock_guard<recursive_mutex> lock(m_playerCriticalSection);

		if (m_personalBanker->GetState() == PBStateInquireSent &&
			m_player != nullptr && 
			m_player->GetPlayerInfo() != nullptr && 
			m_player->GetPlayerInfo()->IsEnrolledInWallet_2_0())
		{
			LeftoverCashTransferStatus status = LeftoverCashTransferStatus::LEFTOVER_CASH_TRANSFER_SUCCEED;

			if (pbInfo->IsUnableToMoveLeftoverCash())
			{
				LogString(POLLER_LOG, "%s %d: PBT Cash Balance = %u, Cash upload limit = %u, Cash download limit = %u.",
					__FUNCTION__, __LINE__, pbInfo->GetCashBalance(), pbInfo->GetMaximumCashUp(), pbInfo->GetMaximumCashDown());
				LogString(POLLER_LOG, "%s %d: Unable to move leftover cash, so ensuring PBT Cash Balance and Cash upload/download limits to be 0.", __FUNCTION__, __LINE__);

				pbInfo->SetCashBalance(0);
				pbInfo->SetMaximumCashDown(0);
				pbInfo->SetMaximumCashUp(0);

				status = LeftoverCashTransferStatus::LEFTOVER_CASH_TRANSFER_FAILED;
			}

			m_player->SetLeftoverCashTransferStatus(status);
		}
	}

	ProcessPersonalBankerInfo(pbInfo);
	SendSubscriptionUpdateIfNecessary();
}

void CInternalsFramework::ProcessResendEmptyFromSCF( CInternalMessage *msg )
{
	bool isResendListEmpty = (bool) msg->GetWordData();

	if (isResendListEmpty)
	{
		// If we get notification from the SCF that the resend list is empty and we are doing an offline, 
		//   reboot by killing the watchdog.
		SentinelOnlineState onlineState = m_EGMConfig.GetSentinelOnlineState();
		if ( onlineState == sentinelGoingOffline || onlineState == sentinelGoingOfflineMove || onlineState == sentinelGoingOfflineDownload )
		{
			if ( onlineState == sentinelGoingOfflineMove )
			{
				m_EGMConfig.SetSentinelOnlineState( sentinelOfflineMove );
			}
			else
			{
				m_EGMConfig.SetSentinelOnlineState( sentinelOffline );
			}

			CDiagnostics::Instance().PersistReboot(tltRebootOffline, POLLER_LOG);
			CWatchDog::Instance().BeginShutdown(CWebDiagnosticsStatusInfo::TiltCodeToString(tltRebootOffline));
		}
		// If we are doing a controlled reboot, do it now.
		else if (m_restartManager.IsShutdownInProcess())
		{
			LogString(GENERAL, "Received Resend List Empty");
			TiltCode tiltCode(tltRebootUnknown);
			string identifier(MEMORY_LOG);
			if (m_restartManager.NeedToPerformControlledRebootForOsUpgrade())
			{
				LogString(GENERAL, "OS Upgrade reboot");
				m_sentinelState.ClearPendingAction(OsUpgrade);
				tiltCode = tltRebootSoftUpgrade;
				identifier = FLASHING;
			}
			else if (m_restartManager.NeedToPerformControlledRebootAfterNextDrop())
			{
				LogString(GENERAL, "Next Drop reboot");
				m_sentinelState.ClearPendingAction(RebootAfterDrop);
				tiltCode = tltRebootWarning;
			}
			else if (m_restartManager.NeedToPerformControlledRebootAsap())
			{
				LogString(GENERAL, "ASAP reboot");
				m_sentinelState.ClearPendingAction(RebootAsap);
				tiltCode = tltRebootAsap;
			}
			else if (m_restartManager.NeedToPerformControlledRemoteReboot())
			{
				LogString(GENERAL, "Remote Reboot");
				m_sentinelState.ClearPendingAction(RemoteReboot);
				tiltCode = tltRebootRemote;
			}

			LogString(GENERAL, "BeginShutdown");
			CDiagnostics::Instance().PersistReboot(tiltCode, identifier);
			CWatchDog::Instance().BeginShutdown(CWebDiagnosticsStatusInfo::TiltCodeToString(tiltCode));
		}
		else if (m_restartManager.GetRemoteRebootASAP())
		{
			CDiagnostics::Instance().PersistReboot(tltRebootRemote, MEMORY_LOG);
			LogString(GENERAL, "BeginShutdown");
			CWatchDog::Instance().BeginShutdown(CWebDiagnosticsStatusInfo::TiltCodeToString(tltRebootRemote));
		}
	}
	else
	{//applicable only for remote reboot ASAP, when some (bogus) transaction is there in resend-list which is preventing reboot.
		if (m_restartManager.GetRemoteRebootASAP())
		{
			GenerateTilt(tltRebootRemotePending);
		}
	}
}

void CInternalsFramework::SetSenOptions1(CConfigSenOption1Field* senOptions1)
{
	m_config.SetCardInPBT(senOptions1->CardInPBT());
	m_config.SetCoinsInAccounting(senOptions1->CoinsInAccounting());
	m_config.SetCoinsOutAccounting(senOptions1->CoinsOutAccounting());
	m_config.SetCoinsWonAccounting(senOptions1->CoinsWonAccounting());
	m_config.SetCouponPromoInEnabled(senOptions1->CouponPromoInEnabled());
	m_config.SetDisplayComps(senOptions1->DisplayComp());
	m_config.SetDisplayCongrats(senOptions1->DisplayCongrats());
	m_config.SetDisplayCountdownAndSession(senOptions1->DisplayCountdownAndSession());
	m_config.SetDisplayGoodLuckAndSessionPts(senOptions1->DisplayGoodLuckAndSession());
	m_config.SetDisplayNumericPointTotal(senOptions1->DisplayNumericPointTotal());
	m_config.SetDisplayPlayerName(senOptions1->DisplayPlayerName());
	m_config.SetDisplayPlayerPtsTotal(senOptions1->DisplayPlayerPtsTotal());
	m_config.SetDisplayPtsTotalAsCash(senOptions1->DisplayPtsTotalAsCash());
	m_config.SetDisplayTotalPtsSessionPtsAsCash(senOptions1->DisplayTotalPtsSessionPtsAsCash());
	m_config.SetEmployeePinRequired(senOptions1->EmployeePinRequired());
	m_config.SetFillAmount(senOptions1->FillAmount());
	m_config.SetPartialBonusAward(senOptions1->PartialBonusAward());
	m_config.SetPBFromGameEnabled(senOptions1->PBFromGameEnabled());
	m_config.SetPBTCashEnabled_local(senOptions1->PBTCashEnable());
	m_config.SetPBTPointsAsPromo(senOptions1->PBTPointsAsPromo());
	m_config.SetPBTPointsEnabled_local(senOptions1->PBTPointsEnable());
	m_config.SetPBTPromoEnabled_local(senOptions1->PBTPromoEnable());
	m_config.SetTheoreticalAccounting(senOptions1->TheoreticalAccounting());
	m_config.SetTicketInEnabled(senOptions1->TicketInEnabled());
	m_config.SetUseHopperLevel(senOptions1->UseHopper());
	m_config.SetVirtualSessionEnabled(senOptions1->VirtualSessionEnabled());
	m_config.SetVoucherPromoInEnabled(senOptions1->VoucherPromoInEnabled());
	m_config.SetSenOption1Raw(senOptions1->GetSenOptions());
	
}

void CInternalsFramework::SetSenOptions2(CConfigSenOption2Field* senOptions2)
{
	LogString(POLLER_LOG, "%s %d: senoptionRaw %u", __FUNCTION__, __LINE__, senOptions2->GetSenOptions());
	m_config.SetSenOption2Raw(senOptions2->GetSenOptions());
	m_config.SetCouponPromoOutEnabled(senOptions2->CouponPromoOutEnabled());
	m_config.SetDisplayGoodLuckAndSession(senOptions2->DisplayGoodLuckAndSessionPts());
	m_config.SetDisplayPtsTotalAsCashSession(senOptions2->DisplayPtsTotalAsCashSession());
	m_config.SetDisplayTotalAndSessionPtsNum(senOptions2->DisplayTotalAndSessionPtsNum());
	m_config.SetPartialWATOutEnabled(senOptions2->PartialWATOutEnabled());
	m_config.SetPBTPinNotRequired(senOptions2->PBTPinNotRequired());
	m_config.SetVoucherOutEnabled(senOptions2->VoucherOutEnabled());
	m_config.SetRcclCardsEnabled(senOptions2->RcclCardsEnabled());
	m_config.SetMiniVirtualSessionEnabled(senOptions2->MiniVirtualSessionEnabled());
	m_config.SetSenOpAccruePointsForNCEP(senOptions2->AccruePointsForNCEP());
	m_config.SetSenOpAccrueCompForNCEP(senOptions2->AccrueCompForNCEP());
        m_config.SetProbePBTLock(senOptions2->ProbePBTLockEnabled());
	bool oldOneLinkProgressivesEnabled(m_config.SetOneLinkProgressivesEnabled(senOptions2->OneLinkProgressivesEnabled()));
	m_sentinelState.m_oneLinkProgressivesSenOptionSupported = (m_sentinelState.m_pollerPacketVersion >= PACKET_VERSION_ONE_LINK_PROGRESIVES_SEN_OPTION);
	m_sentinelState.m_oneLinkProgressivesEnabled = senOptions2->OneLinkProgressivesEnabled();
	m_config.SetUJPEnabled(senOptions2->UJPEnabled());
	// check mgmd feature flag from FL
	if (m_config.MultiGameMultiDenom())
		m_config.SetMGMDEnabled(senOptions2->MGMDEnabled());

	if (oldOneLinkProgressivesEnabled != senOptions2->OneLinkProgressivesEnabled())
	{
		SendOneLinkProgressivesSupportToProgressive();
	}

	bool oldMobileConnectCommunicationEnabled(m_config.SetMobileConnectCommunicationEnabled(senOptions2->MobileConnectCommunicationEnabled()));

	if (oldMobileConnectCommunicationEnabled != senOptions2->MobileConnectCommunicationEnabled())
	{
		LogString(POLLER_LOG, "%s:%d - MobileConnectCommunicationEnabled(%d) update sent to Poller component", 
			__FUNCTION__, __LINE__, senOptions2->MobileConnectCommunicationEnabled());

		SendMobileConnectCommunicationSupport();
	}

	// Trigger sending the pairingSettings message if the flag is enabled.
	if (senOptions2->MobileConnectCommunicationEnabled())
	{
		LogString(POLLER_LOG, "Send mobile.request.pairingSettings from SCF");
		ProcessCallMobileConnectPairingSettingsRequest();
	}

	bool oldRemoteDebuggingEnabled(m_config.SetRemoteDebuggingEnabled(senOptions2->RemoteDebuggingEnabled()));
	if (oldRemoteDebuggingEnabled != senOptions2->RemoteDebuggingEnabled())
	{
		CSmartDataLogger::Instance().SetRemoteLoggingEnabledFlag(m_config.RemoteDebuggingEnabled());
	}

	m_config.SetPartialWATOutEnabled(senOptions2->PartialWATOutEnabled());
	m_config.SetCashoutToHostEnabled(senOptions2->CashoutToHostEnabled());
}

//This sets the features flag 
void CInternalsFramework::SetFeatureFlags(CConfigFeatureFlagField* featureFlags)
{
	m_config.SetFeatureFlags(featureFlags->AtomicRedemptions(), featureFlags->JackpotExternalBonus(), featureFlags->ExternalCreditToGame(), featureFlags->MultiGameMultiDenom());

	m_config.SetFeatureFlagsRaw(featureFlags->GetFeatureFlags());

	// Update m_egmAwardManager with feature
    m_egmAwardManager.SetJackpotExternalBonusFeature(featureFlags->JackpotExternalBonus());
}

void CInternalsFramework::ProcessSCFProtocolVersion( CInternalMessage *msg )
{
	if (msg)
		m_scfProtocolVersion = msg->GetWordData();

	return;
}


void CInternalsFramework::SendIPToSCFProtocol()
{
	CConnection* connection = new CConnection();
	PopulateConnection(*connection);

	CInternalMessage *msg = new CInternalMessage(INT_MSG_SET_IP, connection);
	SendMessageToSCF( msg, QUEUE_PRIORITY_NORMAL );
}

void CInternalsFramework::ProcessSetOneLinkProgressivesSenOptionSupport(CInternalMessage* msg)
{
	if (msg != NULL)
	{
		LogString(POLLER_LOG, "[%s]", __FUNCTION__);

		WORD newPollerPackerVersion = (WORD)msg->GetDWordData();
		if (newPollerPackerVersion)
		{
			// We get here whenever SCF reports us a non-0 poller packet version. We should update 
			// the frequency to send 304/305 meter update poller messages here - when we connect to  
			// the poller for the first time or when we transition across non-NFD/NFD pollers. 
			WORD oldPollerPacketVersion = m_sentinelState.m_pollerPacketVersion;
			bool isOldPollerNFD = (oldPollerPacketVersion >= PACKET_VERSION_NEW_FLOOR_DESIGN);
			bool isNewPollerNFD = (newPollerPackerVersion >= PACKET_VERSION_NEW_FLOOR_DESIGN);
			m_sentinelState.m_pollerPacketVersion = newPollerPackerVersion;

			// Always keep track of current poller's NFD/non-NFD state at  
			// internals by updating in the persistant config object
			m_config.SetNFDPoller(isNewPollerNFD);

			if ((oldPollerPacketVersion == 0 && newPollerPackerVersion != 0) ||
				(isOldPollerNFD != isNewPollerNFD))
			{
				m_games.RefreshMetersSendFrequency(isNewPollerNFD);
			}
		}

		bool supported = (bool)msg->GetWordData();
		if (m_sentinelState.m_oneLinkProgressivesSenOptionSupported != supported)
		{
			m_sentinelState.m_oneLinkProgressivesSenOptionSupported = supported;
			SendOneLinkProgressivesSupportToProgressive();
		}
	}
}

void CInternalsFramework::ProcessRestartServiceWindow()
{
	LogString(GENERAL, "ProcessRestartServiceWindow - setting flag to restart UI");
	m_sentinelState.SetPendingAction(RestartUI);
	SendDashboardStatusToProgressive();
	m_restartManager.SetPerformUiRestartAsapFlag(true);
	SendSubscriptionUpdateIfNecessary();
}

void CInternalsFramework::ProcessMWConfigFileAvailable(CInternalMessage *msg)
{
	m_mwConfig->SetFromFile(msg->GetStringData(), false);
	COperatingSystem::m_operatingSystem.DeleteFile(msg->GetStringData().c_str());
	m_displayManager->SetMediaWindowConfig(*m_mwConfig);
}

void CInternalsFramework::ProcessUJPAuthorizePin(CInternalMessage *msg)
{
	if (msg != nullptr)
	{
		UnattendedJackpotPINResponseType::Type response = (UnattendedJackpotPINResponseType::Type)msg->GetWordData();

		// Check card number
		CJackpot *jackpot = msg->GetJackpotData();

		lock_guard<recursive_mutex> lock(m_jackpotCriticalSection);
		CJackpot::LogInfo(m_jackpot, _T("ProcessUJPAuthorizePin internal"));
		CJackpot::LogInfo(jackpot, _T("ProcessUJPAuthorizePin msg"));
		if (jackpot != nullptr && m_jackpot != nullptr)
		{
			bool cardIdMatch(!memcmp(jackpot->GetCardID(), m_jackpot->GetCardID(), CARD_ID_LEN));
			if (cardIdMatch && UnattendedJackpotState::UJPPinSent == m_jackpot->GetUnattendedState())
			{
				LogString(JACKPOT, "%s %d %s", __FUNCTION__, __LINE__, UnattendedJackpotPINResponseType::emap.At(response, nullptr));
				switch (response)
				{
				case UnattendedJackpotPINResponseType::UJPPIN_AUTHORIZED:
					{
						m_jackpot->SetUnattendedState(UnattendedJackpotState::UJPAuthorized);
						SendSubscriptionUpdateIfNecessary();
						CInternalMessage* resetJackpotMsg(new CInternalMessage(INT_MSG_RESET_JACKPOT, new CJackpot(*m_jackpot)));
						resetJackpotMsg->SetData((WORD)m_config.UJPTaxWithholding());
						SendMessageToGCF(resetJackpotMsg, QUEUE_PRIORITY_NORMAL);
					}
					break;
				case UnattendedJackpotPINResponseType::UJPPIN_NOTAUTHORIZED:
					{				
						m_jackpot->SetUnattendedState(UnattendedJackpotState::UJPPinError);
						SendSubscriptionUpdateIfNecessary();
					}
					break;
				case UnattendedJackpotPINResponseType::UJPPIN_ACCOUNTLOCKED:
					{
						m_jackpot->SetUnattendedState(UnattendedJackpotState::UJPCanceledAccountLocked);
						SendSubscriptionUpdateIfNecessary();
						// FloorLogix says account locked but doesn't have the info to cancel jackpot.
						SendUJPToPoller(new CJackpot(*m_jackpot));
					}
					break;
				}
			}
			else
			{
				if (cardIdMatch)
				{
					//UJPStatusCanceledTimedOut is sent elsewhere
					LogString(JACKPOT, "%s %d INT_MSG_AUTHORIZE_PIN came back when we are no longer on the Pin screen", __FUNCTION__, __LINE__);
				}
				else
				{
					LogString(JACKPOT, "%s %d cardId mismatch", __FUNCTION__, __LINE__);
					m_jackpot->SetUnattendedState(UnattendedJackpotState::UJPCanceledPlayerIdMismatch);
					SendSubscriptionUpdateIfNecessary();
					SendUJPToPoller(new CJackpot(*m_jackpot));
				}
			}
		}
		CJackpot::LogInfo(m_jackpot, _T("ProcessUJPAuthorizePin done"));
	}
}

void CInternalsFramework::ProcessUJP(CInternalMessage *msg)
{
	if (msg != nullptr)
	{
		lock_guard<recursive_mutex> jackpotLock(m_jackpotCriticalSection); // Get a lock, so value(s) are queued in the order acquired.
		lock_guard<recursive_mutex> playerLock(m_playerCriticalSection); // Get a lock, so value(s) are queued in the order acquired.

		UnattendedJackpotType::Type type = (UnattendedJackpotType::Type)msg->GetDWordData();
		CJackpot *jackpot = new CJackpot(*msg->GetJackpotData());

		// Compare jackpot card/amount with expected values
		if (jackpot && m_jackpot &&
			jackpot->GetJackpotAmount() == m_jackpot->GetJackpotAmount() &&
			!memcmp(jackpot->GetCardID(), m_jackpot->GetCardID(), CARD_ID_LEN))
		{
			LogString(JACKPOT, "%s %d %s", __FUNCTION__, __LINE__, UnattendedJackpotType::emap.At(type, nullptr));

			switch (type)
			{
			case UnattendedJackpotType::UJPTYPE_PROCESS:
			{
				m_jackpot->SetJackpotID(jackpot->GetJackpotID());
				UnattendedJackpotState::Type state(UnattendedJackpotState::UJPPinEntry);
				bool sendUjpStatusToPoller(false);
				if (IsPlayerVirtual())
				{
					sendUjpStatusToPoller = true;
					state = UnattendedJackpotState::UJPCanceledVirtualPlayer;
				}
				ProcessDisplayUJP(state, sendUjpStatusToPoller);
			}
				break;
			case UnattendedJackpotType::UJPTYPE_CARDED_TEASER:
				ProcessDisplayUJP(UnattendedJackpotState::UJPCardedTeaser, false);
				break;
			case UnattendedJackpotType::UJPTYPE_UNCARDED_TEASER:
				ProcessDisplayUJP(UnattendedJackpotState::UJPUncardedTeaser, false);
				break;
			case UnattendedJackpotType::UJPTYPE_CANCEL_INSUFFICIENT_INFO:
				ProcessDisplayUJP(UnattendedJackpotState::UJPCanceledInsufficientInfo);
				break;
			case UnattendedJackpotType::UJPTYPE_CANCEL_EGM_NOT_CAPABLE:
				ProcessDisplayUJP(UnattendedJackpotState::UJPCanceledEgmNotCapable);
				break;
			case UnattendedJackpotType::UJPTYPE_CANCEL_ABOVE_LIMIT:
				ProcessDisplayUJP(UnattendedJackpotState::UJPCanceledAboveLimit);
				break;
			default:
				break;
			}
		}
		else
		{
			LogString(JACKPOT, "%s %d ERROR jackpot=%p amount=%llu m_jackpot=%p amount=%llu ", __FUNCTION__, __LINE__, 
				(void*)jackpot, jackpot ? jackpot->GetJackpotAmount() : 0, (void*)m_jackpot, (void*)m_jackpot ? m_jackpot->GetJackpotAmount() : 0);
		}
		 
		delete jackpot;
	}
}

void CInternalsFramework::ProcessDisplayUJP(UnattendedJackpotState::Type state, bool sendUjpStatusToPoller)
{
	if (m_jackpot)
	{
		m_jackpot->SetUnattendedState(state);
		CUnattendedJackpotCommand unattendedJackpotCommand(m_jackpot, m_msgPriority.GetQueuingConfig().GetUjpTimeout(), MAXIMUM_DISPLAY_DURATION);
		CInternalMessage* uiMsg = new CInternalMessage(INT_MSG_DISPLAY_UJP, unattendedJackpotCommand.getJSON());
		int priority = QUEUE_PRIORITY_NORMAL;
		uiMsg->SetMessagePersist(m_msgPriority.PrioritizeMessage(uiMsg, priority));
		SendMessageToUI(uiMsg, priority);

		if (sendUjpStatusToPoller)
		{
			SendUJPToPoller(new CJackpot(*m_jackpot));
		}
	}
}

void CInternalsFramework::ProcessSubgameSelectionResponse(CInternalMessage *msg)
{
	// reset the countdown display timer
	m_countdownDisplayTickCount = 0;

	SubgameSelectionResponse *response = msg->GetSubgameSelectionResponse();

	if (m_player != nullptr && response != nullptr && m_config.GetGlobalMGMDEnabled() && CPollerMGMDData::Instance().GetMGMDAvailable())
	{
		m_player->ApplySubgameSelectionResponse(response);
		SendPlayerDataToUI(INT_MSG_PLAYER_UPDATE);
		SendMCPlayerSessionInfoToPollerProtocol();
	}
}

void CInternalsFramework::ProcessGameListChunksSentInformation(CInternalMessage *msg)
{
	WORD gameChunkIndexSent = msg->GetWordData();
	WORD numberOfGameChunks = (WORD)msg->GetDWordData();

	LogString(POLLER_LOG, "%s: %d: gameChunkIndexSent %u of %u numberOfGameChunks\n", __FUNCTION__, __LINE__, gameChunkIndexSent, numberOfGameChunks);
	
	if (gameChunkIndexSent != UINT16_MAX)
		m_sentinelState.SetGameChunkIndexSent(gameChunkIndexSent);
}

void CInternalsFramework::ProcessPBPinResetResponseFromPoller(CInternalMessage *pollerMsg)
{
	DWORD currentPlayerId = 0;
	lock_guard<recursive_mutex> lock(m_playerCriticalSection);
	if (m_player != nullptr && m_player->GetPlayerInfo() != nullptr)
	{
		currentPlayerId = m_player->GetPlayerInfo()->GetPlayerId();

		if (m_personalBanker->IsPlayerIdWaitingPinReset(currentPlayerId))
		{
			WORD pinResetResult = pollerMsg->GetWordData();
			LogString(POLLER_LOG, "%s %d pinResetResult: %u", __FUNCTION__, __LINE__, pinResetResult);

			m_personalBanker->SetResetPinResult(pinResetResult);
			SendSubscriptionUpdateIfNecessary();
		}
		else
		{
			LogString(POLLER_LOG, "%s %d Ignoring PB Pin-Reset response message as Player Id(%u) is different/invalid", __FUNCTION__, __LINE__, currentPlayerId);
		}
	}
	else
	{
		LogString(POLLER_LOG, "%s %d Ignoring PB Pin-Reset response message because there is no player or PB timed out", __FUNCTION__, __LINE__);
	}
}

void CInternalsFramework::ProcessSetPollerIPFromSCF(CInternalMessage *msg)
{
	std::string strPollerIP = msg->GetStringData();
	if (m_config.SetHostAddressOrName(strPollerIP))
	{
		LogString(FLASHING, "Poller IP %s persisted into EEPROM memory.", strPollerIP.c_str());
	}
}

void CInternalsFramework::ProcessCallMobileConnectPairingSettingsRequest()
{
	if (m_config.MobileConnectCommunicationEnabled())
	{
		CInternalMessage *transMsg = PrepareTransactionMessage( INT_MSG_TRANSACTION_MOBILE_CONNECT_PROXIMITY, IsPlayerVirtual() );;
		transMsg->SetData((WORD)ToPlayerDeviceService::RequestPairingSettings);

		SendTransactionMessageToSCF( transMsg, QUEUE_PRIORITY_NORMAL);
	}
}

void CInternalsFramework::ProcessPairingSettingsEventData(CInternalMessage *msg)
{
	LogString(POLLER_LOG, "%s %d", __FUNCTION__, __LINE__);
	if (msg != nullptr)
	{
		CPairingSettingsEvent* pairingSettingsEvent(msg->GetPairingSettingsEventData());
		if(pairingSettingsEvent != nullptr)
		{
			m_pairingInformation.UpdateFromPairingSettingsEvent(pairingSettingsEvent);

			// update UI for mobile connect buttons
			m_notificationBox.SetMobileEnableFlag(m_pairingInformation.IsPairingEnabled());
			SendUIConfigToUI();

			// beacon enabled in settings and available
			if (!m_pairingInformation.GetPairingType().compare(BEACON) && m_beaconManager->GetBeacon() && m_beaconManager->GetBeacon()->IsConnected())
			{
				uint8_t transmitPower = 0;
				if (pairingSettingsEvent->GetTransmitPower() != 0xff)
				{
					transmitPower = pairingSettingsEvent->GetTransmitPower();
				}
				else
				{
					transmitPower = MINIMUM_POWER;
				}

				m_beaconManager->SetBeaconPower(transmitPower);

                if (m_beaconManager->SetEddystoneFrameData(m_pairingInformation.GetIdlePairingId(), m_pairingInformation.GetIdlePairingChecksum(), m_pairingInformation.GetMeasuredPower()))
					SendMCPairingStatusToPollerProtocol(true, BEACON_IDLE_SET);
				else
					SendMCPairingStatusToPollerProtocol(false, BEACON_IDLE_NOT_SET);
			}
		}
		else
		{
			LogString(ERROR_LOG, "%s %d: Failed to retrieve CPairingSettingsEvent data from message", __FUNCTION__, __LINE__);
		}
	}
	else
	{
		LogString(ERROR_LOG, "%s %d: Invalid message", __FUNCTION__, __LINE__);
	}
}

void CInternalsFramework::ProcessPairingEventData(CInternalMessage *msg)
{
	LogString(POLLER_LOG, "%s %d", __FUNCTION__, __LINE__);
	std::string status = PAIRING_ID_SET;
	bool success = true;

    if (msg != nullptr)
    {
        CPairingEvent* pairingEvent(msg->GetPairingEventData());
        if(m_pairingInformation.IsWaitingForPairing() && pairingEvent != nullptr)
        {
			LogString(UI_LOG, "%s %d PairingEvent Type: %s, Id: %s, Success: %d Status: %s", 
				__FUNCTION__, __LINE__, pairingEvent->GetStatus().c_str(), pairingEvent->GetPairingId().c_str(), pairingEvent->GetIsSuccess(),pairingEvent->GetStatus().c_str());

			m_pairingInformation.SetWaitingForPairing(false);

			// Either employee or player is 'In'.
			if (DoesEmployeeExist() || m_player != nullptr)
			{
				status = SESSION_IN_PROGRESS;
				success = false;
				m_notificationBox.SetMobileState(MobileState::ErrorOccured, status);
			}
			else if (!m_pairingInformation.IsPairingEnabled())
			{
				status = GAME_NOT_ENABLED;
				success = false;
				m_notificationBox.SetMobileState(MobileState::ErrorOccured, status);
			}
			else
			{
				m_pairingInformation.HandlePairingEvent(pairingEvent, m_notificationBox, m_beaconManager, success, status);
			}
        }
        else
        {
			status = ERROR_STATUS;
			success = false;
            LogString(ERROR_LOG, "%s %d: Failed to retrieve CPairingEvent data from message", __FUNCTION__, __LINE__);
        }
		// Send event.pairingStatus to FS
		SendMCPairingStatusToPollerProtocol(success, status);
		SendSubscriptionUpdateIfNecessary();
    }
    else
    {
        LogString(ERROR_LOG, "%s %d: Invalid message", __FUNCTION__, __LINE__);
    }
}

void CInternalsFramework::HandleCardChangeRequest(const CCardChangeRequest *pCardChangeRequest, CCard *card, bool& bStatus, std::string& statusMsg)
{
	LogString(POLLER_LOG, "%s:%d.", __FUNCTION__, __LINE__);

	if ( m_pairingInformation.IsWaitingForRequestCardChange() && pCardChangeRequest->GetChangeType() == CARD_INSERT)
	{
		LogString(POLLER_LOG, "%s", "Received Insert");
		m_pairingInformation.SetWaitingForRequestCardChange(false);

		// check for existing employee & player
		if ( DoesEmployeeExist())
			statusMsg = EXISTING_EMPLOYEE_SESSION;
		else if (m_player != nullptr)
		{
			if (!m_player->CompareCard(card))
			{// Not same player
				if (m_player->IsVirtual())
					statusMsg = EXISTING_PLAYER_SESSION;
				else
					statusMsg = PLAYER_NOT_CONFIRMED;
			}
			else
			{// same player
				if (!m_player->IsVirtual())
					statusMsg = ALREADY_INSERTED;
			}
		}

		if (statusMsg == UNKNOWN_CARD)
		{
			// Set Idle ID, after successful receive of request.cardChange 
			if (!m_pairingInformation.GetPairingType().compare(BEACON) && m_beaconManager->GetBeacon() && m_beaconManager->GetBeacon()->IsConnected())
			{
				if (m_beaconManager->SetEddystoneFrameData(m_pairingInformation.GetIdlePairingId(), m_pairingInformation.GetIdlePairingChecksum(), m_pairingInformation.GetMeasuredPower()))
					SendMCPairingStatusToPollerProtocol(true, BEACON_IDLE_SET);
				else
					SendMCPairingStatusToPollerProtocol(false, BEACON_IDLE_NOT_SET);
			}

			m_pairingInformation.SetMobileCardedSession(true);
			bStatus = HandleCardInserted(card, true);

			if (bStatus)
				statusMsg.clear();
			else
			{
				m_pairingInformation.SetMobileCardedSession(false);
			}
		}
		LogString(POLLER_LOG, "Status for card Insert : %s ", statusMsg.c_str());
	}
	else if ((m_pairingInformation.IsMobileCardedSession() || m_bannedPlayerCardInserted) && 
				pCardChangeRequest->GetChangeType() == CARD_REMOVE)
	{
		LogString(POLLER_LOG, "%s", "Received Remove");
		HandleCardRemoved(0, false);
		bStatus = true;
		statusMsg.clear();
	}
}

void CInternalsFramework::ProcessCardChangeRequestData(CInternalMessage *msg)
{
    bool bStatus            = false;
    std::string statusMsg   = UNKNOWN_CARD;
    std::string cardId;

	LogString(POLLER_LOG, "%s %d", __FUNCTION__, __LINE__);

    if (msg != nullptr) {
        CCardChangeRequest *pCardChangeRequest(msg->GetCardChangeRequestData());
        if (pCardChangeRequest != nullptr) 
		{
            cardId = pCardChangeRequest->GetCardNumber();

            CCard *card = new CCard(CUtilities::GetCurrentTime(), CUtilities::GetUTCTime());
            CCardConfig cardConfig;
			cardConfig.Setup(m_config);

			card->SetCardID(cardId, &cardConfig);

			m_pairingInformation.SetCorrelationId(RequestCardChange, pCardChangeRequest->GetCorrelationId());

			HandleCardChangeRequest(pCardChangeRequest, card, bStatus, statusMsg);

			if (m_config.MobileConnectCommunicationEnabled())
			{
				CCardChangeEvent *pCardChangeEventPoller = new CCardChangeEvent(statusMsg, bStatus, m_pairingInformation.GetCorrelationId(RequestCardChange));
				CInternalMessage *pollerMsg = PrepareTransactionMessage(INT_MSG_TRANSACTION_MOBILE_CONNECT_PROXIMITY, IsPlayerVirtual());
				pollerMsg->SetData((WORD) ToPlayerDeviceService::EventCardChange);
				pollerMsg->SetData(pCardChangeEventPoller);

				SendTransactionMessageToSCF(pollerMsg, QUEUE_PRIORITY_NORMAL);
			}
        }
        else
        {
            LogString(ERROR_LOG, "%s %d: Failed to retrieve CCardChangeRequest data from message", __FUNCTION__, __LINE__);
        }
    }
	else
	{
        LogString(ERROR_LOG, "%s %d: Invalid message", __FUNCTION__, __LINE__);
    }

}

void CInternalsFramework::ProcessCreditTotalsRequestData(CInternalMessage *msg)
{
	LogString(POLLER_LOG, "%s %d", __FUNCTION__, __LINE__);
	if (m_pairingInformation.IsMobileCardedSession())
	{
		CFundsTransferBalanceRequest *pCreditTotalsRequest(msg->GetFundsTransferBalanceRequestData());
		if (pCreditTotalsRequest != nullptr) 
		{
			m_pairingInformation.SetCorrelationId(RequestFundsTransferBalance, pCreditTotalsRequest->GetCorrelationId());

			if (!IsPBTEnabled())
			{// PBT disabled from FL.
				m_mobilePBTInfo.SetError(PBT_DISABLED);
				SendMCFundsTransferBalanceToPollerProtocol();
			}
			else if (!m_mobilePBTInfo.IsMobilePBSession() && GetPbtInProgress())
			{
				// already device PB session (non-mobile) in progress.
				m_mobilePBTInfo.SetError(PBT_ALREADY_INPROGRESS);
				SendMCFundsTransferBalanceToPollerProtocol();
			}
			else if (DoesEmployeeExist())
			{// Employee card inserted, pbt not allowed
				m_mobilePBTInfo.SetError(EMPLOYEE_SESSION_INPROGRESS);
				SendMCFundsTransferBalanceToPollerProtocol();
			}
			else
			{
				m_mobilePBTInfo.SetMobileFundsTransferBalanceRequest(true);
				m_mobilePBTInfo.SetMobilePBSession(true);

				// check payload version of PDS for wallet 2.0 compatibility.
				float payloadVersion = atof(pCreditTotalsRequest->GetPayloadVersion().c_str());
				m_mobilePBTInfo.SetWallet_2_0_CompatiblePDS(payloadVersion >= 2.0f ? true : false);

				// update UI
				SendMessageToUI(new CInternalMessage(INT_MSG_MONITOR_MOBILE_PB_SESSION), QUEUE_PRIORITY_NORMAL);

				BeginPersonalBankerSession(msg);
			}
		}
	}
	else
	{
		m_mobilePBTInfo.SetError(NO_MOBILE_SESSION);
		SendMCFundsTransferBalanceToPollerProtocol();
	}
}

void CInternalsFramework::ProcessFundsTransferRequestData(CInternalMessage *msg)
{
	LogString(POLLER_LOG, "%s %d", __FUNCTION__, __LINE__);
	if (m_pairingInformation.IsMobileCardedSession())
	{
		m_mobilePBTInfo.SetMobileFundTransferRequest(true);

		CFundsTransferRequest *pFundsTransferRequest(msg->GetFundsTransferRequestData());
		if (pFundsTransferRequest != nullptr) 
		{
			m_mobilePBTInfo.SetMobileTransferType(m_mobilePBTInfo.ConvertToPBTransferType(pFundsTransferRequest->GetFundType(), pFundsTransferRequest->GetTransferDirection()));
			m_mobilePBTInfo.SetMobileAmountInPennies(pFundsTransferRequest->GetAmountInPennies());
			m_mobilePBTInfo.SetMobilePin(pFundsTransferRequest->GetPin());
			m_mobilePBTInfo.SetMobileFundType(pFundsTransferRequest->GetFundType());
			m_mobilePBTInfo.SetMobileTransferDirection(pFundsTransferRequest->GetTransferDirection());

			m_pairingInformation.SetCorrelationId(RequestFundsTransfer, pFundsTransferRequest->GetCorrelationId());
		}

		// add checks for data received, denom check, pbt in-progress, employee card
		if (!GetPbtInProgress())
			SendMCFundsTransferToPollerProtocol(false, SESSION_TIMEOUT, true);
		else if (m_mobilePBTInfo.IsMobileDenomMismatch())
			SendMCFundsTransferToPollerProtocol(false, DENOM_MISMATCH, true);
		else if (m_mobilePBTInfo.IsMobileSASError())
			SendMCFundsTransferToPollerProtocol(false, SAS_ERROR, true);
		if (DoesEmployeeExist())
			SendMCFundsTransferToPollerProtocol(false, EMPLOYEE_SESSION_INPROGRESS, true);
		else if (m_mobilePBTInfo.GetMobileTransferType() == Undeclared)
			SendMCFundsTransferToPollerProtocol(false, INVALID_TRANSFER_TYPE, true);
		else
		{
			// all gud to start pbt transfer
			CPersonalBankerInfo* pbInfo = m_personalBanker->GetInfoCopy();
			if(IsPBTProbeRequiredMobile(pbInfo))
			{
				bool isTransferToGame(!pFundsTransferRequest->GetTransferDirection().compare("ToGame"));
				ProcessProbePBTFromUI(isTransferToGame);
			}
			else
			{
				SendMCFundsTransferToPollerProtocol(false, ERROR, true);// TBD: what can be specific here ?
			}
			delete pbInfo;
		}
	}
	else
	{
		SendMCFundsTransferToPollerProtocol(false, NO_MOBILE_SESSION, true);
	}
}

const CPairingInformation& CInternalsFramework::GetMobilePairingInformation()
{
	return m_pairingInformation;
}

void CInternalsFramework::RequestPairing()
{
	LogString(POLLER_LOG, "%s %d", __FUNCTION__, __LINE__);

	// This should not be sent if Mobile Connect is disabled as a global SenOption
	if (!(m_config.MobileConnectCommunicationEnabled()))
		return;

	if(!m_pairingInformation.IsMobileCardedSession() && !DoesPlayerExist())
	{
		LogString(POLLER_LOG, "Sending PairingRequest to Poller protocol");
		CPairingRequest *pairingRequestPoller = new CPairingRequest();
		CInternalMessage *pollerMsg = PrepareTransactionMessage(INT_MSG_TRANSACTION_MOBILE_CONNECT_PROXIMITY, IsPlayerVirtual());
		pollerMsg->SetData((WORD) ToPlayerDeviceService::RequestPairing);
		pollerMsg->SetData(pairingRequestPoller);

		SendTransactionMessageToSCF( pollerMsg, QUEUE_PRIORITY_NORMAL );

#ifdef _DEBUG
		LogString(POLLER_LOG, "Internals: RequestPairing() -> INT_MSG_TRANSACTION_MC_PAIRING_REQUEST_EVENT, type = %s.",
			ToPlayerDeviceService::pRequestPairing);
#endif

		// Set pairing information
		m_pairingInformation.SetWaitingForPairing(true);
		m_pairingInformation.SetPairingRequestTime(CUtilities::GetCurrentTime());
	}
	else
	{
		LogString(POLLER_LOG, "Session in-progress, sending pairing.status to floor protocol");
		SendMCPairingStatusToPollerProtocol(false, SESSION_IN_PROGRESS);
	}
}

void CInternalsFramework::SendMCPairingStatusToPollerProtocol(bool isSuccess, std::string status)
{
	if (m_config.MobileConnectCommunicationEnabled())
	{
		LogString(POLLER_LOG, "Sending pairing.status to Poller protocol");

		CPairingStatusEvent *pPairingStatusEventPoller = new CPairingStatusEvent(isSuccess, status);
		CInternalMessage *pollerMsg = PrepareTransactionMessage(INT_MSG_TRANSACTION_MOBILE_CONNECT_PROXIMITY, IsPlayerVirtual());
		pollerMsg->SetData((WORD) ToPlayerDeviceService::EventPairingStatus);
		pollerMsg->SetData(pPairingStatusEventPoller);

		SendTransactionMessageToSCF(pollerMsg, QUEUE_PRIORITY_NORMAL);
	}
}

void CInternalsFramework::HandleMobilePairingTimeOut()
{
	m_pairingInformation.SetWaitingForPairing(false);
	m_notificationBox.SetMobileState(MobileState::ErrorOccured, TIMEOUT);
	SendSubscriptionUpdateIfNecessary();

	LogString(POLLER_LOG, "Timeout in receiving event.pairing, sending pairing.status to floor protocol");
	SendMCPairingStatusToPollerProtocol(false, TIMEOUT);
}

void CInternalsFramework::HandleMobileRequestCardChangeTimeOut()
{
	m_pairingInformation.SetWaitingForRequestCardChange(false);
	m_notificationBox.SetMobileState(MobileState::ErrorOccured, TIMEOUT);
	
	// Set Idle ID, incase request.cardChange is not received
	if (!m_pairingInformation.GetPairingType().compare(BEACON) && m_beaconManager->GetBeacon() && m_beaconManager->GetBeacon()->IsConnected())
	{
        if (m_beaconManager->SetEddystoneFrameData(m_pairingInformation.GetIdlePairingId(), m_pairingInformation.GetIdlePairingChecksum(), m_pairingInformation.GetMeasuredPower()))
			SendMCPairingStatusToPollerProtocol(true, BEACON_IDLE_SET);
		else
			SendMCPairingStatusToPollerProtocol(false, BEACON_IDLE_NOT_SET);
	}
							
	LogString(POLLER_LOG, "Timeout in receiving request.cardChange, sending pairing.status to floor protocol");
	SendMCPairingStatusToPollerProtocol(false, TIMEOUT);
	SendSubscriptionUpdateIfNecessary();
}

void CInternalsFramework::CheckBeaconConnectionStatus()
{
	if (!m_pairingInformation.GetPairingType().compare(BEACON))
	{
		if (m_beaconManager->CheckBeaconConnected())
		{ // status changed
			if (nullptr != m_beaconManager->GetBeacon())
			{ //beacon connected
                if (m_beaconManager->SetEddystoneFrameData(m_pairingInformation.GetIdlePairingId(), m_pairingInformation.GetIdlePairingChecksum(), m_pairingInformation.GetMeasuredPower()))
					SendMCPairingStatusToPollerProtocol(true, BEACON_IDLE_SET);
				else
					SendMCPairingStatusToPollerProtocol(false, BEACON_IDLE_NOT_SET);
			}
			else
			{// beacon disconnected
				SendMCPairingStatusToPollerProtocol(false,BEACON_DISCONNECTED);
			}
		}
		LogString(ERROR_LOG, "%s %d: Invalid message", __FUNCTION__, __LINE__);
	}
}

void CInternalsFramework::ProcessCallProximityRequestBeaconScanList()
{
#ifdef _DEBUG
		LogString(PROXIMITY_LOG, "%s -> INT_MSG_TRANSACTION_PROXIMITY_REQUEST_BEACON_SCAN_LIST.", __FUNCTION__);
#endif
	CInternalMessage *transMsg = PrepareTransactionMessage(INT_MSG_TRANSACTION_PROXIMITY_REQUEST_BEACON_SCAN_LIST, IsPlayerVirtual());

	SendTransactionMessageToSCF(transMsg, QUEUE_PRIORITY_NORMAL);
}

void CInternalsFramework::ProcessCallProximitySettingsRequest()
{
#ifdef _DEBUG
		LogString(POLLER_LOG, "%s -> INT_MSG_CALL_MCP_PROXIMITY_SETTINGS_REQUEST.", __FUNCTION__);
#endif
	CInternalMessage *transMsg = PrepareTransactionMessage(INT_MSG_TRANSACTION_MOBILE_CONNECT_PROXIMITY, IsPlayerVirtual());
	transMsg->SetData((WORD)ToPlayerDeviceService::RequestProximitySettings);

	SendTransactionMessageToSCF(transMsg, QUEUE_PRIORITY_NORMAL);
}

void CInternalsFramework::ProcessProximitySettingsEventData(CInternalMessage *msg)
{
	LogString(POLLER_LOG, "%s %d", __FUNCTION__, __LINE__);
	std::string status;
	bool success = false;
	bool active = false;

	if (msg != nullptr)
	{
		CProximitySettingsEvent* proximitySettingsEvent(msg->GetProximitySettingsEventData());
		if (proximitySettingsEvent != nullptr)
		{
			std::string UUID = proximitySettingsEvent->GetUUID();
			active = proximitySettingsEvent->GetActive();

			// remove '-' character from UUID string
			UUID.erase(std::remove(UUID.begin(), UUID.end(), '-'), UUID.end());

			LogString(POLLER_LOG, "deviceID: %s, deviceType: %s, UUID: %s, beaconId: %u, pointOfInterest: %u, transmitPower: %u, transmitFrequency: %u, active: %u",
					  proximitySettingsEvent->GetDeviceId().c_str(),
					  proximitySettingsEvent->GetDeviceType().c_str(),
					  UUID.c_str(),
					  proximitySettingsEvent->GetBeaconId(),
					  proximitySettingsEvent->GetPointOfInterest(),
					  proximitySettingsEvent->GetTransmitPower(),
					  proximitySettingsEvent->GetTransmitFrequency(),
					  active);

			// emit iBeacon frame if beacon state is active
			if (active)
			{
				success = m_beaconManager->InitializeBeaconFrames(EDDYSTONE_FRAME | IBEACON_FRAME);
			}
			else
			{
				success = m_beaconManager->InitializeBeaconFrames(EDDYSTONE_FRAME);
			}

			// program beacon with proximity settings
			if (success)
			{
				success = m_beaconManager->SetIBeaconFrameData(UUID,
															   proximitySettingsEvent->GetPointOfInterest(),
															   proximitySettingsEvent->GetBeaconId(),
															   proximitySettingsEvent->GetTransmitPower());
			}

			status = success ? (active ? PROXIMITY_ID_SET : PROXIMITY_ID_SET_INACTIVE) : BEACON_ERROR;
		}
		else
		{
			LogString(ERROR_LOG, "%s %d: Failed to retrieve CProximitySettingsEvent data from message", __FUNCTION__, __LINE__);
			status = BEACON_ERROR;
		}
		SendProximityStatusToPollerProtocol(success, status);
	}
}

void CInternalsFramework::SendProximityStatusToPollerProtocol(bool isSuccess, std::string status)
{
	// This is a proximity message and does not need the MC flag check
	LogString(POLLER_LOG, "Sending proximity.status to Poller protocol");

	CProximityStatusEvent *pProximityStatusEventPoller = new CProximityStatusEvent(isSuccess, status);
	CInternalMessage *pollerMsg = PrepareTransactionMessage(INT_MSG_TRANSACTION_MOBILE_CONNECT_PROXIMITY, IsPlayerVirtual());
	pollerMsg->SetData((WORD)ToPlayerDeviceService::EventProximityStatus);
	pollerMsg->SetData(pProximityStatusEventPoller);

	SendTransactionMessageToSCF(pollerMsg, QUEUE_PRIORITY_NORMAL);
}

void CInternalsFramework::ProcessProximityBeaconScanListResponse(CInternalMessage *msg)
{
	LogString(PROXIMITY_LOG, "Setting proximity beacon scan list.");
}