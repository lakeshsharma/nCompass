#include "stdafx.h"

#include "Logging/LogString.h"

#include "EEPROMConfig.h"
#include "../Hardware/IOCTLOffsets.h"
#include "../CheckSum.h"

using namespace std;

BYTE CEEPROMConfig::m_eepromShadow[EEPROM_OFFSET_END];
std::recursive_mutex CEEPROMConfig::m_cs;

CEEPROMConfig::CEEPROMConfig(void)
{
	lock_guard<recursive_mutex> lock(m_cs);
	try 
	{
		m_type = MEMORY_EEPROM;
		
		// create shadow EEPROM and checksum
		bool bValidEeprom = false;
		m_memoryReset = false;
		int i = 0;
		WORD tmpCS = 0;
		// Don't do the checksum verification if existing checksum is 0.  This way, we don't bring down the whole floor the first time we run.
		m_eeprom.GetData(EEPROM_CHECKSUM_OFFSET, (BYTE *)&tmpCS, sizeof(tmpCS));
		if (tmpCS != 0)
		{
			while (!bValidEeprom && i++ < 3)
			{
				m_eeprom.GetData(EEPROM_OFFSET_START, (BYTE *)m_eepromShadow, EEPROM_OFFSET_END);
				tmpCS = CalculateChecksum();
				m_eeprom.GetData(EEPROM_CHECKSUM_OFFSET, (BYTE *)&m_eepromShadowChecksum, sizeof(m_eepromShadowChecksum));
				if (tmpCS == m_eepromShadowChecksum)
					bValidEeprom = true;
			}
			if (!bValidEeprom)
			{
				LogString(EEPROM_LOG, "Checksum bad at startup." );
				LogString(EEPROM_LOG, "Expected Checksum: %d", m_eepromShadowChecksum);
				LogString(EEPROM_LOG, "Actual Checksum: %d", tmpCS );
				LogData(EEPROMDATA_LOG, DATA_READ, m_eepromShadow, sizeof(m_eepromShadow));
				//m_eeprom.ResetMemory(); //We decided not to reset and just leave the values as is but send up the memory bad transaction.
				m_memoryReset = true;
			}
		}
		else
		{
			// checksum 0 means probably first time running after upgrade, just initialize shadow eeprom.
			m_eeprom.GetData(EEPROM_OFFSET_START, (BYTE *)m_eepromShadow, EEPROM_OFFSET_END);
		}
	}
	catch (...)
	{
	}
}

// defined so that normal constructor isn't called
CEEPROMConfig::CEEPROMConfig( CEEPROMConfig &eRHS ) 
{
    UNREFERENCED_PARAMETER(&eRHS); 
}

CEEPROMConfig::~CEEPROMConfig(void)
{
}

bool CEEPROMConfig::Set(DWORD offset, BYTE *data, UINT len)
{
	bool retValue = false;

	m_bOffsetIsSet = TRUE;
	m_dwAllowedSize = len;

	if(ValidateOffset(offset, false))
	{
	retValue = Write(offset, data, len);
	}

	return retValue;
}

bool CEEPROMConfig::Get(DWORD offset, BYTE *data, UINT len)
{

	bool retValue = false;

	m_dwAllowedSize = len;

	
	if(ValidateOffset(offset, true))
	{
	retValue = Read(offset, data, len);
	}

	return retValue;
}

bool CEEPROMConfig::ValidateOffset(DWORD offset1, bool read)
{
	bool retVal = true;

	if(offset1 != EEPROM_CHECKBYTE_OFFSET && offset1 != EEPROM_SLOTMAST_ID_OFFSET &&
		offset1 != EEPROM_CASINO_NUMBER_OFFSET && offset1 != EEPROM_COMM_ID_OFFSET &&
		offset1 != EEPROM_MACHINE_TYPE_OFFSET && offset1 != EEPROM_SLOT_NUMBER_OFFSET &&
		offset1 != EEPROM_HARD_PIN_TYPES_OFFSET && offset1 != EEPROM_HARD_PIN_ACTIVE_STATE_OFFSET &&
		offset1 != EEPROM_SERIAL_PROTOCOL_OFFSET && offset1 != EEPROM_GAME_DENOM_OFFSET &&
		offset1 != EEPROM_SCREEN_BRIGHTNESS_OFFSET && offset1 != EEPROM_LEFT_SPEAKER_VOLUME_OFFSET &&
		offset1 != EEPROM_RIGHT_SPEAKER_VOLUME_OFFSET && offset1 != EEPROM_GLOBAL_CARD_PASS_OFFSET &&
		offset1 != EEPROM_UNUSED1_OFFSET && offset1 != EEPROM_UNUSED2_OFFSET &&
		offset1 != EEPROM_DHCPAUTO_OFFSET && offset1 != EEPROM_HOST_ADDRESS_OFFSET &&
		offset1 != EEPROM_HOST_PORT_OFFSET && offset1 != EEPROM_MY_IP_OFFSET &&
		offset1 != EEPROM_DEFAULT_GATEWAY_OFFSET && offset1 != EEPROM_SUBNET_MASK_OFFSET &&
		offset1 != EEPROM_PREFERRED_DNS_OFFSET &&
		offset1 != EEPROM_EGM_TYPE_OFFSET && offset1 != EEPROM_ALT_DNS_OFFSET &&
		offset1 != EEPROM_EGM_POLLING_SPEED_OFFSET && offset1 != EEPROM_EGM_PROTOCOL_OFFSET &&
		offset1 != EEPROM_UNUSED3_OFFSET && offset1 != EEPROM_SCREEN_CALIBRATED_FLAG_OFFSET &&
		offset1 != EEPROM_EGM_PROTOCOL_STATE_OFFSET && offset1 != EEPROM_SENTINEL_ONLINE_STATE_OFFSET &&
		offset1 != EEPROM_BATTERY_STATUS_OFFSET && offset1 != EEPROM_BATTERY_STATUS_TIME_OFFSET &&
		offset1 != EEPROM_SERVICE_WINDOW_LOCATION_OFFSET && offset1 != EEPROM_SERVICE_WINDOW_SIZE_OFFSET &&
		offset1 != EEPROM_TSPROTO_TYPE_OFFSET && offset1 != EEPROM_TSPROTO_VENDOR_ID_OFFSET &&
		offset1 != EEPROM_TSPROTO_BAUD_OFFSET &&
		offset1 != EEPROM_CHECKSUM_OFFSET && 
		offset1 != EEPROM_TASKBAR_SIZE_OFFSET && offset1 != EEPROM_MIXER_REBOOT_COUNT_OFFSET &&
		offset1 != EEPROM_VIDEO_INPUT_AUTO_DETECT_OFFSET && offset1 != EEPROM_VIDEO_OUTPUT_AUTO_CONFIG_OFFSET &&
		offset1 != EEPROM_DESKTOP_ORIENTATION_OFFSET &&
		offset1 != EEPROM_MW_EGM_ASPECT_RATIO_OFFSET &&
		offset1 != EEPROM_MW_CARD_IN_PLAYER_STATE_OFFSET &&
		offset1 != EEPROM_MW_CARD_OUT_PLAYER_STATE_OFFSET &&
		offset1 != EEPROM_MW_AUTO_OPEN_OFFSET &&
		offset1 != EEPROM_MW_AUTO_CLOSE_OFFSET &&
		offset1 != EEPROM_MW_AUTO_MINUTES_OFFSET &&
		offset1 != EEPROM_MW_FIRST_PLAY_PLAYER_STATE_OFFSET &&
		offset1 != EEPROM_MW_LOCAL_CHANGE_PENDING_MAP_OFFSET &&
		offset1 != EEPROM_REBOOT_DIAGNOSTIC_TILTCODE_OFFSET &&
		offset1 != EEPROM_MW_BONUS_EVENT_PLAYER_STATE_OFFSET)
	{
		LogString(ERROR_LOG, "EEPROM %s offset unknown: %d.\n", read ? "read" : "write", offset1 );
		retVal = false;
	}

	return retVal;
}

bool CEEPROMConfig::Write(DWORD offset1, BYTE * data1, UINT len1,
						   DWORD offset2, BYTE * data2, UINT len2,
						   DWORD offset3, BYTE * data3, UINT len3,
						   DWORD offset4, BYTE * data4, UINT len4)
{
    UNREFERENCED_PARAMETER(&offset2); 
    UNREFERENCED_PARAMETER(&data2); 
    UNREFERENCED_PARAMETER(&len2); 
    UNREFERENCED_PARAMETER(&offset3); 
    UNREFERENCED_PARAMETER(&data3); 
    UNREFERENCED_PARAMETER(&len3); 
    UNREFERENCED_PARAMETER(&offset4); 
    UNREFERENCED_PARAMETER(&data4); 
    UNREFERENCED_PARAMETER(&len4); 

	bool bReturnValue = false;

	if (m_type == MEMORY_NONE)
	{
		bReturnValue = true;
	}
	else if (len1 <= m_dwAllowedSize)
	{
		lock_guard<recursive_mutex> lock(m_cs);
		try
		{
			switch (m_type)
			{
			case MEMORY_EEPROM:
				{
					// Write EEPROM data to shadow eeprom and calculate new checksum.
					memcpy(&m_eepromShadow[offset1], data1, len1);
					m_eepromShadowChecksum = CalculateChecksum();
					LogString(EEPROM_LOG, "Writting to Offset = %d.\n", offset1 );


					// make 3 attempts to write data to eeprom and checksum
					bReturnValue = false;
					for (int i = 0; i < 3 && !bReturnValue; i++)
					{
						//need to account for the host name crossing the 128 page EEPROM boundry
						if(offset1 == EEPROM_HOST_ADDRESS_OFFSET)
						{
							//the host name crosses the 128 page bound of the EEPROM
							//so we need to break it into 2 blocks at the 128 page bound
							char hostAddressDefaultFirstBlock[EEPROM_HOST_ADDRESS_FIRST_BLOCK_SIZE] = "\0";
							char hostAddressDefaultSecondBlock[EEPROM_HOST_ADDRESS_SECOND_BLOCK_SIZE] = "\0";
							int index = 0;
							for ( int ti = 0; ti < (int)EEPROM_HOST_ADDRESS_FIRST_BLOCK_SIZE; ti++ )
							{
								hostAddressDefaultFirstBlock[ti] = data1[index++];
							}
							for ( int ti = 0; ti < (int)EEPROM_HOST_ADDRESS_SECOND_BLOCK_SIZE; ti++ )
							{
								hostAddressDefaultSecondBlock[ti] = data1[index++];
							}
							bReturnValue = m_eeprom.SetData(EEPROM_HOST_ADDRESS_OFFSET, (BYTE *)hostAddressDefaultFirstBlock, EEPROM_HOST_ADDRESS_FIRST_BLOCK_SIZE,
												EEPROM_HOST_ADDRESS_OFFSET + EEPROM_HOST_ADDRESS_FIRST_BLOCK_SIZE, (BYTE *)hostAddressDefaultSecondBlock, EEPROM_HOST_ADDRESS_SECOND_BLOCK_SIZE);
						}
						else
						{
						bReturnValue = m_eeprom.SetData(offset1, data1, len1);
						}

						bool bCSReturnValue = m_eeprom.SetData(EEPROM_CHECKSUM_OFFSET,(BYTE *) &m_eepromShadowChecksum, sizeof(m_eepromShadowChecksum));
						
						if (bReturnValue && bCSReturnValue)
						{
							// wrote data and checksum, read these values and compare against original
							WORD tmpChecksum = 0;
							BYTE tmpData[EEPROM_OFFSET_END];
							memset(tmpData, 0, EEPROM_OFFSET_END);
							m_eeprom.GetData(offset1, tmpData, len1);
                            m_eeprom.GetData(EEPROM_CHECKSUM_OFFSET, (BYTE *)&tmpChecksum, sizeof(tmpChecksum));
							if (!memcmp(tmpData, data1, len1) && tmpChecksum == m_eepromShadowChecksum)
							{
								// values read in equal what should have been written
								bReturnValue = true;
							}
							else
							{
								bReturnValue = false;
								LogString(EEPROM_LOG, "After write checksum or data does not match.\n" );
								LogString(EEPROM_LOG, "Offset = %d.\n", offset1 );
							}
						}
						else
						{
							LogString(EEPROM_LOG, "Failed to write %s to EEPROM.\n", bReturnValue == true ? "checksum" : "data" );
							bReturnValue = false;
						}
					}

				}	
				break;
			default:
				bReturnValue = false;
			}
		}
		catch(...)
		{
		}
	}
	else
	{
		#ifdef LOGGING
		LogString(ERROR_LOG, "Error - size of memory storage write too large," );
		#endif
	}

	return bReturnValue;
}

bool CEEPROMConfig::Read(DWORD offset, BYTE * data, DWORD len)
{
	bool bReturnValue = false;

	if (len <= m_dwAllowedSize)
	{
		switch (m_type)
		{
			case MEMORY_NONE:
				bReturnValue = true;
				break;
			case MEMORY_EEPROM:
				{
					//bReturnValue = m_eeprom.GetData(offset, data, len);
					memcpy(data, &m_eepromShadow[offset], len);
					bReturnValue = true;
				}	
				break;
			case MEMORY_NVRAM:
				{
					bReturnValue = m_nvram.GetData(offset, data, len);
				}
				break;
			default:
				bReturnValue = false;
		}
	}
	return bReturnValue;
}

WORD CEEPROMConfig::CalculateChecksum()
{
	CCheckSum cs(true);
	cs.Add(m_eepromShadow, EEPROM_OFFSET_END);
	return (WORD)cs.Get();
}

BYTE *CEEPROMConfig::GetEEPROMShadow( )
{
	return m_eepromShadow;
}

bool CEEPROMConfig::GetMemoryResetStatus(bool bClear)
{
	bool retVal = m_memoryReset;
	if (m_memoryReset && bClear)
		m_memoryReset = false;
	
	return retVal;
}
