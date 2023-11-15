#include "stdafx.h"
#include "PollerConfigRespMessage.h"

CPollerConfigRespMessage::CPollerConfigRespMessage(const PollerMessageHeader &hdr, const byte *dataPtr)
{
	MessageHeader = hdr;
	m_bufferSize = hdr.DataLength;
	m_lpBuffer = new byte[m_bufferSize];
	memcpy_s(m_lpBuffer, m_bufferSize, dataPtr, hdr.DataLength);
}

CPollerConfigRespMessage::~CPollerConfigRespMessage(void)
{
}

CConfigFields * CPollerConfigRespMessage::GetFields()
{
	bool haveSenOptions1 = false; // Did we receive a "full" configuration message?

	// Did we receive valid fields for new options?
	bool haveFeatureFlags = false;
	bool haveNclEnabledCardTrack = false;
	bool haveShowPointsVsAccountBalance = false;
	bool haveSessionPointsMessage = false;
	bool havePointsOrCashLabel = false;
	bool haveBalanceRetrievalMessage = false;
	bool haveDailyPointsDisplaySuffix = false;
	bool haveTierPointsLabel = false;

	int offset = 0;
	FieldTypes type;
	int fieldLength(0);
	bool bFail = false;

	CConfigFields * lpConfigFields = new CConfigFields();

	// If Poller (packet version) does not support configuration fields then set the fields to default values.
	// Add default field(s) to lpConfigFields first, in case the Poller packet version is incorrect and a non-default field is later
	// added to the list, so that if lpConfigFields is processed in order then the non-default values will be processed last.
	if (!IsNclEnabledCardTrackFieldSupported())
	{
		// Create a default field object and add it to lpConfigFields.
		CConfigField* field = new CNclEnabledCardTrackField();
		if (field != NULL)
		{
			lpConfigFields->AddField(field);
		}
	}

	//get total number of fields sent
	int CountOfFields = m_lpBuffer[offset];
	offset++;

	for (int i = 0; (i < CountOfFields) && !bFail; i++)
	{
		fieldLength = 0;

		//for each field get its type
		type = (FieldTypes)m_lpBuffer[offset];
		offset++;

		//only read the length if it is not EXTENSIBLE_SYSTEM_SETTINGS
		//because it not used by EXTENSIBLE_SYSTEM_SETTINGS
		if (EXTENSIBLE_SYSTEM_SETTINGS != type)
		{			
			fieldLength = m_lpBuffer[offset];
			offset++;
		}

		//turn it into a field object and add it to the configs array
		CConfigField* field = GetField(type, &(m_lpBuffer[offset]), fieldLength);
		if (field != NULL)
		{
			lpConfigFields->AddField(field);

			// Record which fields of interest we have received.
			switch (type)
			{
			case SEN_OPTIONS1:
				haveSenOptions1 = true;
				break;
			case FEATURE_FLAGS:
				haveFeatureFlags = true;
				break;
			case NCL_ENABLED_CARD_TRACK:
				haveNclEnabledCardTrack = true;
				break;
			case SHOW_POINTS_VS_ACCOUNT_BALANCE:
				haveShowPointsVsAccountBalance = true;
				break;
			case SESSION_POINTS_TEXT:
				haveSessionPointsMessage = true;
				break;
			case POINTS_OR_CASH_LABEL:
				havePointsOrCashLabel = true;
				break;
			case BALANCE_RETRIEVAL_TEXT:
				haveBalanceRetrievalMessage = true;
				break;
			case DAILY_POINTS_DISPLAY_SUFFIX:
				haveDailyPointsDisplaySuffix = true;
				break;
			case TIER_POINTS_LABEL:
				haveTierPointsLabel = true;
				break;
			default:
				break;
			}
		}

		offset+= fieldLength;

		if (offset >= m_bufferSize)
		{
			bFail = true;
		}
	}

	// A "full" configuration message always includes SEN_OPTIONS1.
	// If we received a "full" configuration message and it did not include valid fields for new options
	// then add default fields for the new options.
	// /So nCompass will use default values for new options when communicating with older Pollers.
	if (haveSenOptions1)
	{
		if (!haveFeatureFlags)
		{
			lpConfigFields->AddField(new CConfigFeatureFlagField);
		}

		if (!haveNclEnabledCardTrack)
		{
			lpConfigFields->AddField(new CNclEnabledCardTrackField);
		}

		if (!haveShowPointsVsAccountBalance)
		{
			lpConfigFields->AddField(new CConfigShowPointsVsAccountBalanceField);
		}

		if (!haveSessionPointsMessage)
	{
			lpConfigFields->AddField(new CConfigSessionPointsTextField);
		}

		if (!havePointsOrCashLabel)
		{
			lpConfigFields->AddField(new CConfigPointsOrCashLabelField);
		}

		if (!haveBalanceRetrievalMessage)
		{
			lpConfigFields->AddField(new CConfigBalanceRetrievalTextField);
		}
		
		if (!haveDailyPointsDisplaySuffix)
		{
			lpConfigFields->AddField(new CConfigDailyPointsDisplaySuffixField);
		}
				
		if (!haveTierPointsLabel)
		{
			lpConfigFields->AddField(new CConfigTierPointsLabelField);
		}
	}

	return lpConfigFields;
}

//This returns the requested config field
// CPollerConfigRespMessage::GetField()
//     IN type - indicates the field type identifier for the field data.
//     IN buffer - reference MUST point to a readable location at least "length" bytes long.
//     IN length - indicates the length of the buffer that is used to specify the field value.
// Process (raw) message data from a Sentinel Configuration field and put it into a CConfigFile instance.
CConfigField * CPollerConfigRespMessage::GetField(FieldTypes type, byte * buffer, int length)
{
	CConfigField *field = NULL;

	switch (type)
	{
	case DENOMINATION:	field = new CConfigDenomField();
		((CConfigDenomField *)field)->SetDenom(buffer, length);
		break;
	case CASINO_NUMBER: field = new CConfigCasinoNumField();
		((CConfigCasinoNumField *)field)->SetCasinoNumber(buffer[0]);
		break;
	case COMM_ID:		field = new CConfigCommIDField();
		((CConfigCommIDField *)field)->SetCommID(buffer[0]);
		break;
	case SEN_OPTIONS1:	field = new CConfigSenOption1Field();
		((CConfigSenOption1Field *)field)->SetSenOptions(buffer, length);
		break;						  
	case SEN_OPTIONS2:	field = new CConfigSenOption2Field();
		((CConfigSenOption2Field *)field)->SetSenOptions(buffer, length);
		break;			
	case SLOT_NUMBER:	field = new CConfigSlotNumberField();
		((CConfigSlotNumberField *)field)->SetSlotNumber(buffer, length);
		break;
	case PLAYER_PREFIXES: field = new CConfigPlayerPrefixField();
		((CConfigPlayerPrefixField *)field)->SetPlayerPrefixes(buffer, length);
		break;
	case OTHER_PREFIXES: field = new CConfigOtherPrefixesField();
		((CConfigOtherPrefixesField *)field)->SetPrefixes(buffer, length);
		break;
	case CASINO_NAME:	field = new CConfigCasinoNameField();
		((CConfigCasinoNameField *)field)->SetCasinoName(buffer, length);
		break;
	case POINTS_PER_DOLLAR:	field = new CConfigPointsPerDollarField();
		((CConfigPointsPerDollarField *)field)->SetPointsPerDollar(buffer, length);
		break;
	case LOSS_LIMIT:	field = new CConfigLossLimitField();
		((CConfigLossLimitField *)field)->SetLossLimitOptions(buffer, length);
		break;
	case TICKETING:		field = new CConfigTicketingField();
		((CConfigTicketingField *)field)->SetTicketingOptions(buffer, length);
		break;
	case PLAYER_TRACKING: field = new CConfigPlayerTrackingField();
		((CConfigPlayerTrackingField *)field)->SetPlayerTrackingOptions(buffer, length);
		break;
	case VIRTUAL_SESSIONS: field = new CConfigVirtualSessionField();
		((CConfigVirtualSessionField *)field)->SetVirtualSessionOptions(buffer, length);
		break;
	case CARD_TIMEOUT:	field = new CConfigCardTimeoutField();
		((CConfigCardTimeoutField *)field)->SetCardTimeout(buffer, length);
		break;
	case FLOOR_LOCK:	field = new CConfigFloorLockField();
		((CConfigFloorLockField *)field)->SetFloorLock(buffer[0] == 1);
		break;
	case ALLOW_FLOOR_LOCKOUT:
		field = new CConfigAllowFloorLockoutField();
		// As it comes from the poller, this is actually DISAllowFloorLockout in order to keep things backwards compatable.
		((CConfigAllowFloorLockoutField*)field)->SetAllowFloorLockout(buffer[0] == 0);
		break;
	case PBT:			
		field = new CConfigPBTField();
		((CConfigPBTField *)field)->SetPBTOptions(buffer, length);
		break;
	case CASH_TRANSFER_TRACKING:
		field = new CConfigCashTransferTrackingField();
		((CConfigCashTransferTrackingField*)field)->SetCashTransferTracking(buffer[0] == 1);
		break;
	case TIMEZONE:
		field = new CConfigTimezoneField();
		((CConfigTimezoneField*)field)->SetTimezoneOptions(buffer, length);
		break;
	case SLOTMAST_REVISION:
		field = new CConfigSlotMastRevisionField();
		((CConfigSlotMastRevisionField*)field)->SetSlotMastRevision(buffer, length);
		break;
	case POINTS_FOR_PROMO:
		field = new CConfigPointsForPromoField();
		((CConfigPointsForPromoField*)field)->SetAccruePointsForPromo(buffer, length);
		break;
	case SEND_FULL_METER_UPDATE:
		field = new CConfigUpdateMMTField();
		((CConfigUpdateMMTField*)field)->SetUpdateMMT(buffer[0] == 1);
		break;
	case UNIVERSE_ENABLED:
		field = new CUniverseEnabledField();
		((CUniverseEnabledField*)field)->SetUniverseFields(buffer, length);
		break;
	case UNIVERSE_PLAYER_PREFIXES:
		field = new CConfigUniverseCardPrefixField();
		((CConfigUniverseCardPrefixField *)field)->SetUniverseCardPrefixes(buffer, length);
		break;
	case UNIVERSE_PROP_CODES:
		field = new CConfigUniversePropCodesField();
		((CConfigUniversePropCodesField *)field)->SetUniversePropCodes(buffer, length);
		break;
	case FEATURE_FLAGS:
		field = new CConfigFeatureFlagField();
		((CConfigFeatureFlagField*)field)->SetFeatureFlags(buffer, length);
		break;
	case NCL_ENABLED_CARD_TRACK:
		field = new CNclEnabledCardTrackField();
		((CNclEnabledCardTrackField *)field)->SetField(buffer, length);
		break;
	case SHOW_POINTS_VS_ACCOUNT_BALANCE:
		field = new CConfigShowPointsVsAccountBalanceField();
		((CConfigShowPointsVsAccountBalanceField*)field)->SetShowPointsVsAccountBalance(buffer[0] == 1);
		break;
	case SESSION_POINTS_TEXT:
		field = new CConfigSessionPointsTextField();
		((CConfigSessionPointsTextField*)field)->SetSessionPointsText(buffer, length);
		break;
	case POINTS_OR_CASH_LABEL:
		field = new CConfigPointsOrCashLabelField();
		((CConfigPointsOrCashLabelField*)field)->SetPointsOrCashLabel(buffer, length);
		break;
	case BALANCE_RETRIEVAL_TEXT:
		field = new CConfigBalanceRetrievalTextField();
		((CConfigBalanceRetrievalTextField*)field)->SetBalanceRetrievalText(buffer, length);
		break;		
	case DAILY_POINTS_DISPLAY_SUFFIX:
		field = new CConfigDailyPointsDisplaySuffixField();
		((CConfigDailyPointsDisplaySuffixField*)field)->SetDailyPointsDisplaySuffixText(buffer, length);
		break;
	case TIER_POINTS_LABEL:
		field = new CConfigTierPointsLabelField();
		((CConfigTierPointsLabelField*)field)->SetTierPointsLabel(buffer, length);
		break;
	case PLAYER_PROFILE_BUTTON:
		field = new CConfigPlayerProfileButtonField();
		((CConfigPlayerProfileButtonField*)field)->SetDisplayPlayerProfileButton(buffer[0] == 1);
		break;
	case GLOBAL_MGMD_ENABLED:
		field = new CConfigMGMDField();
		((CConfigMGMDField*)field)->SetGlobalMGMD(buffer[0] == 1);
		break;
	case EXCESSIVE_VOUCHER_OUT_ENABLED:
		field = new CConfigExcessiveVoucherOutEnabledField();
		((CConfigExcessiveVoucherOutEnabledField*)field)->SetEnabledFlag(buffer[0] == 1);
		break;
	case EXCESSIVE_VOUCHER_OUT_THRESHOLD:
	case TOTAL_BILL_IN_THRESHOLD:
	case COIN_IN_THRESHOLD:
	case COIN_OUT_THRESHOLD:
		field = new CConfigExcessiveVoucherOutThresholdField(type);
		((CConfigExcessiveVoucherOutThresholdField *)field)->SetThreshold(buffer, length);
		break;
	case PARTIAL_WAT_OUT_ENABLED:
		field = new CConfigPartialWATOutEnabledField();
		((CConfigPartialWATOutEnabledField*)field)->SetEnabledFlag(buffer[0] == 1);
		break;
	case EXTENSIBLE_SYSTEM_SETTINGS:
		field = new CConfigExtensibleSystemSettingsField(buffer, length);
		break;
	default:
		break;
	}

	return field;
}

bool CPollerConfigRespMessage::IsNclEnabledCardTrackFieldSupported()
{
	static const WORD NCL_ENABLED_CARD_TRACK_MINIMUM_PACKET_VERSION = 5;

	bool returnValue = MessageHeader.PacketVersion >= NCL_ENABLED_CARD_TRACK_MINIMUM_PACKET_VERSION;
	return returnValue;
}
