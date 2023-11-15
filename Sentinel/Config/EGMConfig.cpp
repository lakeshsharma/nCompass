#include "stdafx.h"
#include "EGMConfig.h"
#include "RapidJsonHelper.h"
#include "UI/ScreenDefs.h"
#include "Logging/LogString.h"
#include "../PollerProtocol/PollerMGMDData.h"

using namespace std;

string SentinelOnlineStateStr[] =
{
	"sentinelOffline",
	"sentinelOfflineMove",
	"sentinelComingOnline",
	"sentinelComingOnlineMove",
	"sentinelOnline",
	"sentinelGoingOffline",
	"sentinelGoingOfflineMove",
	"sentinelGoingOfflineDownload"
};

CEGMConfig::CEGMConfig( MEMORY_TYPE memoryType ) : CDataSubscriptionProvider("EgmConfiguration"), CEEPROMConfig ()
{
	m_type = memoryType;

	m_dwAllowedSize = EEPROM_MAX_READ;
	m_dwOffset = EEPROM_OFFSET_START;
	m_bOffsetIsSet = true;

	m_egmType = (EGMType)EEPROM_EGM_TYPE_DEFAULT;
	m_egmProtocol = (EGMProtocol)EEPROM_EGM_PROTOCOL_DEFAULT;
	m_egmPollingSpeed = (EGMPollingSpeed)EEPROM_EGM_POLLING_SPEED_DEFAULT;
	m_egmProtocolState = (EGMProtocolState)EEPROM_EGM_PROTOCOL_STATE_DEFAULT;
	m_sentinelOnlineState = (SentinelOnlineState)EEPROM_SENTINEL_ONLINE_STATE_DEFAULT;
}

CEGMConfig::CEGMConfig( CEGMConfig &eRHS ) : 
CDataSubscriptionProvider("EgmConfiguration"),
CEEPROMConfig ( (CEEPROMConfig &)eRHS)
{
	m_type = MEMORY_NONE;

	*this = eRHS;
}

CEGMConfig::~CEGMConfig(void)
{
}

CEGMConfig & CEGMConfig::operator=( CEGMConfig &eRHS )
{
	m_egmType = eRHS.GetEGMType( );
	m_egmProtocol = eRHS.GetEGMProtocol( );
	m_egmPollingSpeed = eRHS.GetEGMPollingSpeed( );
	m_egmProtocolState = eRHS.GetEGMProtocolState( );
	m_sentinelOnlineState = eRHS.GetSentinelOnlineState( );
	memcpy(m_eepromShadow, eRHS.GetEEPROMShadow( ), EEPROM_OFFSET_END);

	StoreDataToEEPROM();

	return *this;
}

string CEGMConfig::GetSubscriptionDataResponse(const std::string& topic, const std::string& whereId)
{
    UNREFERENCED_PARAMETER(&topic); 
    UNREFERENCED_PARAMETER(&whereId); 

	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);

	writer.StartObject();
	writer.Key("EgmType");					writer.Int(m_egmType);
	writer.Key("EgmProtocol");				writer.Int(m_egmProtocol);
	writer.Key("EgmPollingSpeed");			writer.Int(m_egmPollingSpeed);
	writer.Key("EgmProtocolState");			writer.Int(m_egmProtocolState);
	writer.Key("CanPerformMachineOnline");	writer.Bool(CanPerformMachineOnline());
	writer.Key("CanPerformMachineOffline");	writer.Bool(CanPerformMachineOffline());
	writer.Key("CanBeginMachineMove");		writer.Bool(CanBeginMachineMove());
	writer.Key("CanCompleteMachineMove");	writer.Bool(CanCompleteMachineMove());
	writer.Key("SentinelOnlineState");		writer.String(SentinelOnlineStateStr[m_sentinelOnlineState]);
	writer.Key("SlotMastIdResponse");		writer.String(m_slotMastIdResponse);
	writer.EndObject();

	return writeStream.m_str;
}

void CEGMConfig::SetEGMType( EGMType egmType )
{
	m_egmType = egmType;

	if( m_egmType == NO_EGM )
		m_egmProtocol = NO_EGM_PROTOCOL;

	StoreDataToEEPROM();
}

void CEGMConfig::BuildYourself()
{
	if ( m_type == MEMORY_EEPROM )
	{
		byte tempByte;

		if ( Read(EEPROM_EGM_TYPE_OFFSET,		 (byte *)&tempByte,		EEPROM_EGM_TYPE_SIZE) )
			m_egmType = (EGMType)tempByte;
		else
			LogString(ERROR_LOG, "Could not read egm type from EEPROM" );

		if ( Read(EEPROM_EGM_POLLING_SPEED_OFFSET,(BYTE *)&tempByte,		EEPROM_EGM_POLLING_SPEED_SIZE) )
		{
			// NOTE: we used to store the speed, not the enum. So translate into enum if necessary.
			if ( tempByte == 40 )
				m_egmPollingSpeed = MS40;
			else if ( tempByte == 200 )
				m_egmPollingSpeed = MS200;
			else
				m_egmPollingSpeed = (EGMPollingSpeed)tempByte;
			CPollerMGMDData::Instance().SetMGMDPollingSpeed(byte(m_egmPollingSpeed));
		}
		else
			LogString(ERROR_LOG, "Could not read egm polling speed from EEPROM" );

		if ( Read(EEPROM_EGM_PROTOCOL_OFFSET,	 (BYTE *)&tempByte,		EEPROM_EGM_PROTOCOL_SIZE) )
			m_egmProtocol = (EGMProtocol)tempByte;
		else
			LogString(ERROR_LOG, "Could not read egm protocol from EEPROM" );

		if ( Read(EEPROM_EGM_PROTOCOL_STATE_OFFSET,	 (BYTE *)&tempByte,	EEPROM_EGM_PROTOCOL_STATE_SIZE) )
			m_egmProtocolState = (EGMProtocolState)tempByte;
		else
			LogString(ERROR_LOG, "Could not read egm protocol state from EEPROM" );

		if ( Read(EEPROM_SENTINEL_ONLINE_STATE_OFFSET,	 (BYTE *)&tempByte,	EEPROM_SENTINEL_ONLINE_STATE_SIZE) )
			m_sentinelOnlineState = (SentinelOnlineState)tempByte;
		else
			LogString(ERROR_LOG, "Could not read egm change in progress from EEPROM" );
	}
}

EGMProtocol CEGMConfig::GetEGMProtocol(void) const
{
	return m_egmProtocol;
}

EGMProtocol CEGMConfig::GetEGMProtocolType() const
{
	EGMProtocol protocolType = NO_EGM_PROTOCOL;

	if (protocolStateEnabled == m_egmProtocolState)
	{
		protocolType = m_egmProtocol;
	}

	return protocolType;
}

bool CEGMConfig::GetProgressivesSupportedByEGM() const
{
	bool supportedByEGM(false);

	switch (GetEGMProtocolType())
	{
	case SAS5xx:
	case SAS6xx:
		supportedByEGM = true;
		break;
	default:
		break;
	}

	return supportedByEGM;
}

void CEGMConfig::SetEGMPollingSpeed(EGMPollingSpeed pollingSpeed)
{
	m_egmPollingSpeed = pollingSpeed;
	CPollerMGMDData::Instance().SetMGMDPollingSpeed(byte(m_egmPollingSpeed));
	StoreDataToEEPROM();
}

EGMPollingSpeed CEGMConfig::GetEGMPollingSpeed() const
{
	return m_egmPollingSpeed;
}

void CEGMConfig::SetEGMProtocolState(EGMProtocolState protocolState)
{
	m_egmProtocolState = protocolState;
	StoreDataToEEPROM();
}

EGMProtocolState CEGMConfig::GetEGMProtocolState() const
{
	return m_egmProtocolState;
}

string CEGMConfig::GetSentinelOnlineStateStr() const
{
	string onlineStateStr;

	switch (m_sentinelOnlineState)
	{
	case sentinelOffline:
		onlineStateStr = "Sentinel is offline";
		break;
	case sentinelOfflineMove:
		onlineStateStr = "Sentinel is offline because of a machine move";
		break;
	case sentinelComingOnline:
		onlineStateStr = "Sentinel is in the process of coming online";
		break;
	case sentinelComingOnlineMove:
		onlineStateStr = "Sentinel is in the process of completing a move";
		break;
	case sentinelOnline:
		onlineStateStr = "Sentinel is currently online";
		break;
	case sentinelGoingOffline:
		onlineStateStr = "Sentinel is in the process of going offline";
		break;
	case sentinelGoingOfflineMove:
		onlineStateStr = "Sentinel is in the process of going offline because of a machine move";
		break;
	case sentinelGoingOfflineDownload:
		onlineStateStr = "Sentinel is in the process of going offline because of a download";
		break;
	default:
		break;
	}

	return onlineStateStr;
}

void CEGMConfig::StoreDataToEEPROM( void )
{
	if ( m_type == MEMORY_EEPROM )
	{
		byte tempByte;

		tempByte = (byte)m_egmType;
		if ( !Write( EEPROM_EGM_TYPE_OFFSET, &tempByte, EEPROM_EGM_TYPE_SIZE ) )
			LogString(ERROR_LOG, "Could not write egm type to EEPROM" );

		tempByte = (byte)m_egmPollingSpeed;
		if ( !Write( EEPROM_EGM_POLLING_SPEED_OFFSET, &tempByte, EEPROM_EGM_POLLING_SPEED_SIZE ) )
			LogString(ERROR_LOG, "Could not write egm polling speed to EEPROM" );

		tempByte = (byte)m_egmProtocol;
		if ( !Write( EEPROM_EGM_PROTOCOL_OFFSET, &tempByte, EEPROM_EGM_PROTOCOL_SIZE ) )
			LogString(ERROR_LOG, "Could not write egm protocol to EEPROM" );

		tempByte = (byte)m_egmProtocolState;
		if ( !Write( EEPROM_EGM_PROTOCOL_STATE_OFFSET, &tempByte, EEPROM_EGM_PROTOCOL_STATE_SIZE ) )
			LogString(ERROR_LOG, "Could not write egm protocol state to EEPROM" );

		tempByte = (byte)m_sentinelOnlineState;
		if ( !Write( EEPROM_SENTINEL_ONLINE_STATE_OFFSET, &tempByte, EEPROM_SENTINEL_ONLINE_STATE_SIZE ) )
			LogString(ERROR_LOG, "Could not write egm change in progress to EEPROM" );
	}
}

void CEGMConfig::SetSlotmastIDResponse(SlotmastIDResponse response)
{
	switch (response)
	{
	case CHANGE_SUCCESS:
		m_slotMastIdResponse = "COMPLETE";
		break;
	case NO_SLOTMAST_ID:
		m_slotMastIdResponse = "NO_SLOTMAST_ID";
		break;
	case WRONG_OASIS_ID:
		m_slotMastIdResponse = "WRONG_OASIS_ID";
		break;
	case RESPONSE_MISMATCH:
		m_slotMastIdResponse = "NON_MATCHING_ID";
		break;
	case PMT_DENIED:
		m_slotMastIdResponse = "PMT_DENIED";
		break;
	case TIME_OUT:
		m_slotMastIdResponse = "RESPONSE_TIMED_OUT";
		break;
	default:
		m_slotMastIdResponse = FormatString("0x%02X", response);
		break;
	}
}