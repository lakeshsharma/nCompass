#include "stdafx.h"
#include "MemoryStorage.h"

// Sentinel includes.
#ifdef LOGGING
#include "Logging/LogString.h"
#endif

#include "Diagnostics.h"
#include "EEPROMinator.h"
#include "NVRAMinator.h"
#include "PersonalBanker.h"
#include "PersonalBankerTransfer.h"
#include "ProgressiveProtocol/StandardProgressiveWins.h"
#include "ProgressiveProtocol/ProgressiveTransactionResendList.h"
#include "../PlayerInfo.h"
#include "Games.h"
#include "DirectoryManager.h"

// Comment this out once the NVRAM reset issue has been found
#define VERIFY_NVRAM_BLOCKS_ON_WRITE 

// NOTE: The following is no longer used. Keep here just for data conversion.
typedef enum
{
	changeStateNone,
	changeStateOnline,
	changeStateOffline,
	changeStateBeginMove,
	changeStateCompleteMove,
	
} EGMChangeStateInProgress;

// Resolve the singleton eeprom object
CEEPROMinator CMemoryStorage::m_eeprom;
CNVRAMinator CMemoryStorage::m_nvram;
DWORD CMemoryStorage::PreviousNVRAMVersion = 0;

CMemoryStorage::CMemoryStorage(void) :
	m_dwOffset(NVRAM_DEFAULT_OFFSET),
	m_type(DEFAULT_MEMORY_TYPE),
	m_dwAllowedSize(NVRAM_DEFAULT_SIZE)
{
	m_bOffsetIsSet = false;
}

CMemoryStorage::~CMemoryStorage(void)
{    
}

bool CMemoryStorage::IntegrityCheck(std::vector<BYTE> *sprinkles)
{
	// Set up the array of sprinkle offsets.
	DWORD sprinkleOffset[NUM_SPRINKLE_VALUES] = {SPRINKLE1_OFFSET, 
		SPRINKLE2_OFFSET, 
		SPRINKLE3_OFFSET,
		SPRINKLE4_OFFSET,
		SPRINKLE5_OFFSET,
		SPRINKLE6_OFFSET,
		SPRINKLE7_OFFSET,
		SPRINKLE8_OFFSET,
		SPRINKLE9_OFFSET,
		SPRINKLE10_OFFSET,
		SPRINKLE11_OFFSET,
		SPRINKLE12_OFFSET,
		SPRINKLE13_OFFSET,
		SPRINKLE14_OFFSET,
		SPRINKLE15_OFFSET,
		SPRINKLE16_OFFSET };

	// Check the value at each sprinkle offset and make sure each is good.
	bool sprinkleOkay = true;
	for (int sprinkleIndex = 0; sprinkleIndex < NUM_SPRINKLE_VALUES; sprinkleIndex++ )
	{
        BYTE sprinkleValue;

		// Get the sprinkle value from NVRAM.
		m_nvram.GetData(sprinkleOffset[sprinkleIndex], &sprinkleValue, sizeof(BYTE));

        if (sprinkles != nullptr) 
        {
            sprinkles->push_back(sprinkleValue);
        }
        
		// The old and current sprinkle values are both valid
		if ( sprinkleValue != SPRINKLE_VALUE && sprinkleValue != OLD_SPRINKLE_VALUE )
		{
			sprinkleOkay = false;
		}
	}

	// If the CLEAR_MEMORY file exists in the sdboot directory, set flag to clear memory
	if (CUtilities::CheckIfNvramNeedsToBeCleared())
	{
		LogString(NVRAM_LOG, "CLEAR_MEMORY exists, Setting to clear memory");
		sprinkleOkay = false;
	}

	return sprinkleOkay;
}

// This returns true if the first sprinkle is set to zero value
bool CMemoryStorage::IsNVRAMCleared()
{
	bool bmemoryCleared = false;
	BYTE sprinkleValue;
	// Get the sprinkle value of SPRINKLE1_OFFSET from NVRAM.
	m_nvram.GetData(SPRINKLE1_OFFSET, &sprinkleValue, sizeof(BYTE));
	if (sprinkleValue == ZERO_SPRINKLE_VALUE || CUtilities::CheckIfNvramNeedsToBeCleared())
	{
		LogString(NVRAM_LOG,"%s %d", __FUNCTION__, __LINE__);
		bmemoryCleared = true;
	}
	return bmemoryCleared;
}

// This returns true if the first sprinkle is set to the old sprinkle value
bool CMemoryStorage::HasOldSprinkleValue()
{	
	bool hasOldSprinkleValue = false;

	BYTE sprinkleValue;
	m_nvram.GetData(SPRINKLE1_OFFSET, &sprinkleValue, sizeof(BYTE));
	if ( sprinkleValue == OLD_SPRINKLE_VALUE )
	{
		hasOldSprinkleValue = true;
	}

	return hasOldSprinkleValue;
}

void CMemoryStorage::SetSprinkles()
{
	//This sets the known values throughout NVRAM for later checking.
	BYTE value = SPRINKLE_VALUE;

	m_nvram.SetData( SPRINKLE1_OFFSET, &value, sizeof(BYTE),
		   SPRINKLE2_OFFSET, &value, sizeof(BYTE),
		   SPRINKLE3_OFFSET, &value, sizeof(BYTE),
		   SPRINKLE4_OFFSET, &value, sizeof(BYTE));

	m_nvram.SetData( SPRINKLE5_OFFSET, &value, sizeof(BYTE),
		   SPRINKLE6_OFFSET, &value, sizeof(BYTE),
		   SPRINKLE7_OFFSET, &value, sizeof(BYTE),
		   SPRINKLE8_OFFSET, &value, sizeof(BYTE));

	m_nvram.SetData( SPRINKLE9_OFFSET, &value, sizeof(BYTE),
		   SPRINKLE10_OFFSET, &value, sizeof(BYTE),
		   SPRINKLE11_OFFSET, &value, sizeof(BYTE),
		   SPRINKLE12_OFFSET, &value, sizeof(BYTE));

	m_nvram.SetData( SPRINKLE13_OFFSET, &value, sizeof(BYTE),
		   SPRINKLE14_OFFSET, &value, sizeof(BYTE),
		   SPRINKLE15_OFFSET, &value, sizeof(BYTE),
		   SPRINKLE16_OFFSET, &value, sizeof(BYTE));

    // Once the sprinkle values have been set, set to verify blocks upon write.
#ifdef VERIFY_NVRAM_BLOCKS_ON_WRITE
    m_nvram.SetToVerifyBlocksUponWrite();
#endif

	// Now that memory is reset, remove the file indicating to clear memory
	if (CUtilities::CheckIfNvramNeedsToBeCleared())
	{
		LogString(NVRAM_LOG, "Delete CLEAR_MEMORY");
		CUtilities::DeleteFile(CDirectoryManager::CLEAR_MEMORY);
		CProgressiveTransactionResendList::DeleteAllResendListFiles();
	}

	// Set the version to the current number
	m_nvram.SetData(NVRAM_MAP_VERSION_OFFSET, (BYTE *)&NVRAM_MAP_VERSION_VALUE, sizeof(NVRAM_MAP_VERSION_VALUE));
}

void CMemoryStorage::UpgradeNVRAM()
{
	if ( IntegrityCheck() )
	{
		// Check to see if the old sprinkle values are currenly used.
		// If yes, this is an indication that the NVRAM version is still the original version number.
		// This is done because the NVRAM version number was not originally set. 
		// Therefore the sprinkle value had to be used to indicate the prior version.
		if ( HasOldSprinkleValue() )
		{
			PreviousNVRAMVersion = NVRAM_ORIGINAL_VERSION;
		}

		// If the sprinkle is not the old value, get the version number that is in NVRAM 
		else
		{
			m_nvram.GetData(NVRAM_MAP_VERSION_OFFSET, (BYTE *)&PreviousNVRAMVersion, sizeof PreviousNVRAMVersion);
		}

		LogString(NVRAM_LOG, "[%s]: Existing NVRAM version: %u, Sentinel NVRAM version: %u", __FUNCTION__, 
			PreviousNVRAMVersion, NVRAM_MAP_VERSION_VALUE);

		printf("\n[%s] Existing NVRAM version: %u, Sentinel NVRAM version: %u\n", __FUNCTION__,
			PreviousNVRAMVersion, NVRAM_MAP_VERSION_VALUE);

		if ( PreviousNVRAMVersion < NVRAM_UNIVERSAL_VERSION )
		{
			// NOTE: Upgrade to NVRAM_UNIVERSAL_VERSION is handled from within the CConfig class.
		}

		if ( PreviousNVRAMVersion < NVRAM_125_COMPATIBLE_VERSION )
		{
			// Is CPersonalBankerTransfer active?
			byte active = 0;
			m_nvram.GetData( NVRAM_PB_TRANSFER_CHECKBYTE_OFFSET, &active, NVRAM_PB_TRANSFER_CHECKBYTE_SIZE );
			
			if ( active == NVRAM_PB_CHECKBYTE_VALUE )
			{
				// Read CPersonalBankerTransfer::m_state located at NVRAM_PB_TRANSFER_UPDATED_OFFSET
				PersonalBankerState pbtState = PBStateUnknown;
				m_nvram.GetData( NVRAM_PB_TRANSFER_UPDATED_OFFSET, (byte *) &pbtState, CPersonalBanker::NVRAM_PB_STATE_SIZE );
				
				// Save value to CPersonalBanker nvram area as well as the checkbyte
				m_nvram.SetData( CPersonalBanker::NVRAM_PB_STATE_OFFSET, (byte *) &pbtState, CPersonalBanker::NVRAM_PB_STATE_SIZE,
								NVRAM_PB_CHECKBYTE_OFFSET, &active, NVRAM_PB_CHECKBYTE_SIZE );

			}
			else
			{
				// Need to clear CPersonalBanker Active location
				active = ~NVRAM_PB_CHECKBYTE_VALUE;
				m_nvram.SetData( NVRAM_PB_CHECKBYTE_OFFSET, &active, NVRAM_PB_CHECKBYTE_SIZE );
			}
		}

		if (PreviousNVRAMVersion < NVRAM_PLAYER_RANK_VERSION)
		{
			// Copy the portion of the old NVRAM location that actually used to the new location, setting new values to default values.
			// Then set the old NVRAM location to default values.

			// Get a CPlayerInfo NVRAM buffer with default values.
			BYTE defaultBuffer[NVRAM_PLAYER_INFO_SIZE];
			CPlayerInfo playerInfo;
			playerInfo.GetNvramBuffer(defaultBuffer, sizeof(defaultBuffer));

			// Get a CPlayerInfo NVRAM buffer with the old NVRAM values that were actually used and the new values set to default values.
			BYTE newBuffer[NVRAM_PLAYER_INFO_SIZE];
			memcpy_s(newBuffer, sizeof(newBuffer), defaultBuffer, sizeof(defaultBuffer));
			m_nvram.GetData(NVRAM_OLD_PLAYER_INFO_OFFSET, newBuffer, CPlayerInfo::NVRAM_OLD_PLAYER_INFO_SIZE_USED);

			// Set the new NVRAM location to the new player information.
			m_nvram.SetData(NVRAM_PLAYER_INFO_OFFSET, newBuffer, NVRAM_PLAYER_INFO_SIZE);

			// After setting the new information, set the old NVRAM location to the default player information.
			m_nvram.SetData(NVRAM_OLD_PLAYER_INFO_OFFSET, defaultBuffer, CPlayerInfo::NVRAM_OLD_PLAYER_INFO_SIZE_USED);
		}

		if (PreviousNVRAMVersion < NVRAM_EGM_AWARD_VERSION)
		{
			BYTE active(~NVRAM_EGM_AWARD_CHECKBYTE_VALUE);
			m_nvram.SetData(NVRAM_EGM_AWARD_CHECKBYTE_OFFSET, &active, NVRAM_EGM_AWARD_CHECKBYTE_SIZE);
		}

		if (PreviousNVRAMVersion < NVRAM_EGM_AWARD_STANDARD_VERSION)
		{
			StandardProgressiveWins::SetInactiveLocation();
		}

		if (PreviousNVRAMVersion < NVRAM_CUMULATIVE_PROGRESSIVE_VERSION)
		{
			BYTE active(~NVRAM_CUMULATIVE_PROGRESSIVE_CHECKBYTE_VALUE);
			m_nvram.SetData(NVRAM_CUMULATIVE_PROGRESSIVE_CHECKBYTE_OFFSET, &active, sizeof(active));
		}

		//When clearing NVRAM via CLEAR_MEMORY method,
		//we found that NVRAM version doesn't get saved
		//on first boot.  We need to prevent this upgrade from running
		//to avoid messing up the stored number of enabled games (CGames).
		//TODO: Need to make CLEAR_MEMORY perform like the UI method
		if (PreviousNVRAMVersion < NVRAM_MGMD_VERSION && PreviousNVRAMVersion > 0)
		{
			DWORD defaultDWORD = 0;
			bool defaultBool = false;

			// Initialize 4 items to 0s, (playSessionID, lastGameNumber, lastDenom, gameSessionID)
			// Add 6 more ints and 2 bools to player session data
			DWORD offset = NVRAM_PLAYER_SESSION2_DATA_OFFSET;
			m_nvram.SetData(offset, (byte *) &defaultDWORD, sizeof(DWORD));
			offset += sizeof(DWORD);
			m_nvram.SetData(offset, (byte *) &defaultDWORD, sizeof(DWORD));
			offset += sizeof(DWORD);
			m_nvram.SetData(offset, (byte *) &defaultDWORD, sizeof(DWORD));
			offset += sizeof(DWORD);
			m_nvram.SetData(offset, (byte *) &defaultDWORD, sizeof(DWORD));
			offset += sizeof(DWORD);
			m_nvram.SetData(offset, (byte *)&defaultDWORD, sizeof(DWORD));
			offset += sizeof(DWORD);
			m_nvram.SetData(offset, (byte *)&defaultDWORD, sizeof(DWORD));
			offset += sizeof(DWORD);
			m_nvram.SetData(offset, (byte *)&defaultBool, sizeof(bool));
			offset += sizeof(bool);
			m_nvram.SetData(offset, (byte *)&defaultBool, sizeof(bool));
			offset += sizeof(bool);
			m_nvram.SetData(offset, (byte *)&defaultDWORD, sizeof(DWORD));
			offset += sizeof(DWORD);
			m_nvram.SetData(offset, (byte *)&defaultDWORD, sizeof(DWORD));
			offset += sizeof(DWORD);
			m_nvram.SetData(offset, (byte *)&defaultDWORD, sizeof(DWORD));
			offset += sizeof(DWORD);
			m_nvram.SetData(offset, (byte *)&defaultDWORD, sizeof(DWORD));

			UpgradeGamesForMGMD();
		}

		if (PreviousNVRAMVersion < NVRAM_GAME_LIST_CHUNKS_VERSION)
		{
			WORD gameListChunkSize = DEFAULT_GAME_LIST_CHUNK_SIZE;

			//Want this log specifically to Poller for easier debugging.
			LogString(POLLER_LOG, _T("%s: New NVRAM version detected. So writing default Game List Chunk Size"), __FUNCTION__);

			// Initialize to default value
			DWORD offset = NVRAM_GAME_LIST_CHUNKS_CONFIG_OFFSET;
			m_nvram.SetData(offset, (byte *) &gameListChunkSize, sizeof(gameListChunkSize));
		}
		
		if (PreviousNVRAMVersion < NVRAM_MOBILE_NCOMPASS_VERSION)
		{
			LogString(NVRAM_LOG, _T("NVRAM upgraded for Mobile nCompass"));
			bool defaultMobilePlayer = 0;
			DWORD defaultPrevPlayerState = 0;
			DWORD offset = NVRAM_PLAYER_VIRTUAL_OFFSET;
			byte OffsetToMobilePlayer = sizeof(DWORD) + sizeof(DWORD) + sizeof(DWORD) + sizeof(DWORD) + sizeof(bool) + sizeof(WORD) + sizeof(bool);
									/* currentCountdown + sessionPoints + sessionPointsFlushed + PARTIAL_BONUSING_REMAINDER_SIZE + m_bPointsAdded + m_lossLimitSessionBalance + m_hasPlayed */
			m_nvram.SetData( offset+OffsetToMobilePlayer,(byte *) &defaultMobilePlayer, sizeof(defaultMobilePlayer));
				
			offset = NVRAM_PLAYER_SESSION2_DATA_OFFSET;
			byte OffsetToPrevPlayerState = 6 * sizeof(DWORD) + 2 * sizeof(bool) + 4 * sizeof(DWORD);
			m_nvram.SetData( offset+OffsetToPrevPlayerState,(byte *) &defaultPrevPlayerState, sizeof(defaultPrevPlayerState));
			WORD gameListChunkSize = DEFAULT_GAME_LIST_CHUNK_SIZE;

			//Want this log specifically to Poller for easier debugging.
			LogString(POLLER_LOG, _T("%s: New NVRAM version detected. So writing default Game List Chunk Size"), __FUNCTION__);

			// Initialize to default value
			offset = NVRAM_GAME_LIST_CHUNKS_CONFIG_OFFSET;
			m_nvram.SetData(offset, (byte *) &gameListChunkSize, sizeof(gameListChunkSize));
		}
		
		if (PreviousNVRAMVersion == NVRAM_EXCESSIVE_VOUCHER_OUT_VERSION)
		{
			// Changed the way subgames are stored in NVRAM. This one version has nonenabled games stored in NVRAM.
			// Clear out all subgames during this upgrade
			DWORD dwCurrentOffset = NVRAM_GAMES_OFFSET + NVRAM_LOCATION_IS_ACTIVE;
			DWORD NVRAMGameCount = 1;
			BYTE inactive = 0;

			//Get the number of games
			dwCurrentOffset += sizeof(NVRAMGameCount);

			// skip first game, if it is active it can stay active. This is the main EGM_NUMBER
			dwCurrentOffset += NVRAM_EGM_GAME_SIZE;

			// cycle through remaining subgame locations and mark inactive
			while (dwCurrentOffset < NVRAM_GAMES_SIZE + NVRAM_GAMES_OFFSET)
			{
				if (IsActiveLocation(dwCurrentOffset))
				{
					LogString(NVRAM_LOG, "NVRAM Subgame upgrade, removing subgame at offset %d", dwCurrentOffset);
					m_nvram.SetData(dwCurrentOffset, &inactive, sizeof(inactive));
				}

				// move offset pointer to next subgame
				dwCurrentOffset += NVRAM_SUBGAME_SIZE;
			}


			// Reset game count to 1 if the main egm game is present
			if (IsActiveLocation(NVRAM_GAMES_OFFSET))
			{
				LogString(NVRAM_LOG, "NVRAM Subgame upgrade, setting gamecount to 1");
				dwCurrentOffset = NVRAM_GAMES_OFFSET + NVRAM_LOCATION_IS_ACTIVE;

				m_nvram.SetData(dwCurrentOffset, (BYTE *)&NVRAMGameCount, sizeof(NVRAMGameCount));
			}
		}

		if (PreviousNVRAMVersion < NVRAM_PARTIAL_WAT_OUT_VERSION)
		{
			// Initialize a new WORD field in the Updated fields section of CPersonalBankerTransfer
			// object which constitutes personal banker transfer flags
			WORD defaultWord = 0;
			DWORD offset = NVRAM_PB_TRANSFER_UPDATED_OFFSET + NVRAM_PB_TRANSFER_UPDATED_SIZE - sizeof(WORD);

			m_nvram.SetData(offset, (byte *)&defaultWord, sizeof(WORD));

			//Want this log specifically to Poller section for easier debugging.
			LogString(POLLER_LOG, _T("%s: New NVRAM version detected. So writing default personal banker "
				"transfer flags to CPersonalBankerTransfer object's NVRAM location."), __FUNCTION__);

			// NOTE: Updates to config section is handled from within the CConfig class.
		}

		if (PreviousNVRAMVersion < NVRAM_UJP_TAX_WITHHOLDING_VERSION)
		{
			DWORD defaultfederalTaxWithholding(0);
			DWORD defaultstateTaxWithholding(0);
			bool defaultfederalTaxOverridden(false);
			bool defaultstateTaxOverridden(false);
			DWORD offset = NVRAM_JACKPOT_OFFSET + NVRAM_JACKPOT_CHECKBYTE_SIZE + 37;
			m_nvram.SetData(offset, (byte*)&defaultfederalTaxWithholding, sizeof(defaultfederalTaxWithholding));
			offset += sizeof(defaultfederalTaxWithholding);
			m_nvram.SetData(offset, (byte*)&defaultstateTaxWithholding, sizeof(defaultstateTaxWithholding));
			offset += sizeof(defaultstateTaxWithholding);
			m_nvram.SetData(offset, (byte*)&defaultfederalTaxOverridden, sizeof(defaultfederalTaxOverridden));
			offset += sizeof(defaultfederalTaxOverridden);
			m_nvram.SetData(offset, (byte*)&defaultstateTaxOverridden, sizeof(defaultstateTaxOverridden));
			offset += sizeof(defaultstateTaxOverridden);

			//Want this log specifically to Poller section for easier debugging.
			LogString(POLLER_LOG, _T("%s: New NVRAM version detected. So writing default tax withholding values"
				" to CJackpot object's NVRAM location."), __FUNCTION__);

			// NOTE: Updates to config section is handled from within the CConfig class.
		}
		
		// Add future upgrades here.

	}

	// Set the version to the current number
	m_nvram.SetData( NVRAM_MAP_VERSION_OFFSET, (BYTE *)&NVRAM_MAP_VERSION_VALUE, sizeof(NVRAM_MAP_VERSION_VALUE));
}

void CMemoryStorage::CheckSprinkles(bool before, std::string args) const
{
	if (!CUtilities::CheckIfNvramNeedsToBeCleared())
	{
		std::stringstream ss = std::stringstream();
		bool errorsFound = false;

		ss <<  "NVRAM invalid sprinkle(s) found " << (before ? " before" : " after") << " write. Args:(" << args.c_str() << ")";

		// Set up the array of sprinkle offsets.
		DWORD sprinkleOffset[NUM_SPRINKLE_VALUES] = {
			SPRINKLE1_OFFSET,
			SPRINKLE2_OFFSET,
			SPRINKLE3_OFFSET,
			SPRINKLE4_OFFSET,
			SPRINKLE5_OFFSET,
			SPRINKLE6_OFFSET,
			SPRINKLE7_OFFSET,
			SPRINKLE8_OFFSET,
			SPRINKLE9_OFFSET,
			SPRINKLE10_OFFSET,
			SPRINKLE11_OFFSET,
			SPRINKLE12_OFFSET,
			SPRINKLE13_OFFSET,
			SPRINKLE14_OFFSET,
			SPRINKLE15_OFFSET,
			SPRINKLE16_OFFSET };

		// Check the value at each sprinkle offset and make sure each is good.
		for (int sprinkleIndex = 0; sprinkleIndex < NUM_SPRINKLE_VALUES; sprinkleIndex++ )
		{
			BYTE sprinkleValue;

			// Get the sprinkle value from NVRAM.
			m_nvram.GetData(sprinkleOffset[sprinkleIndex], &sprinkleValue, sizeof(BYTE));

			// The old and current sprinkle values are both valid
			if ( sprinkleValue != SPRINKLE_VALUE && sprinkleValue != OLD_SPRINKLE_VALUE )
			{
				errorsFound = true;
				ss << " Index=" << sprinkleIndex << " offset=" << sprinkleOffset[sprinkleIndex] << " value=" << std::hex << (int)sprinkleValue << std::dec;
			}
		}

		if (errorsFound)
		{
			ss << " -- " << DumpCorruptNVRAMArea();
			ReportSingleCallstack(ss.str());
		}
	}
}

// only for corruption testing
BYTE CMemoryStorage::PokeNVRam(DWORD offset, BYTE value) const
{
    BYTE retVal;
    m_nvram.GetData(offset, &retVal, sizeof(BYTE));
    m_nvram.SetData(offset, &value, sizeof(BYTE));
    return retVal;
}

string CMemoryStorage::DumpCorruptNVRAMArea() const
{       
    // Set up the array of sprinkle offsets.
	DWORD sprinkleOffset[NUM_SPRINKLE_VALUES] = {
        SPRINKLE1_OFFSET, 
		SPRINKLE2_OFFSET, 
		SPRINKLE3_OFFSET,
		SPRINKLE4_OFFSET,
		SPRINKLE5_OFFSET,
		SPRINKLE6_OFFSET,
		SPRINKLE7_OFFSET,
		SPRINKLE8_OFFSET,
		SPRINKLE9_OFFSET,
		SPRINKLE10_OFFSET,
		SPRINKLE11_OFFSET,
		SPRINKLE12_OFFSET,
		SPRINKLE13_OFFSET,
		SPRINKLE14_OFFSET,
		SPRINKLE15_OFFSET,
		SPRINKLE16_OFFSET };

    int firstBadIndex = 0;

    for (int sprinkleIndex = 0; sprinkleIndex < NUM_SPRINKLE_VALUES; sprinkleIndex++ )
    {
        BYTE sprinkleValue;

        DWORD offset = sprinkleOffset[sprinkleIndex];

        // Get the sprinkle value from NVRAM.
        m_nvram.GetData(offset, &sprinkleValue, sizeof(BYTE));

        // The old and current sprinkle values are both valid
        if ( sprinkleValue != SPRINKLE_VALUE && sprinkleValue != OLD_SPRINKLE_VALUE )
            break;

        firstBadIndex = sprinkleIndex;
    }

    int lastBadIndex = NUM_SPRINKLE_VALUES - 1;

    for (int sprinkleIndex = NUM_SPRINKLE_VALUES - 1; sprinkleIndex >= 0; sprinkleIndex-- )
    {
        BYTE sprinkleValue;

        DWORD offset = sprinkleOffset[sprinkleIndex];

        // Get the sprinkle value from NVRAM.
        m_nvram.GetData(offset, &sprinkleValue, sizeof(BYTE));

        // The old and current sprinkle values are both valid
        if ( sprinkleValue != SPRINKLE_VALUE && sprinkleValue != OLD_SPRINKLE_VALUE )
            break;

        lastBadIndex = sprinkleIndex;
    }

    DWORD firstBadOffset = sprinkleOffset[firstBadIndex];
    DWORD lastBadOffset = sprinkleOffset[lastBadIndex];

    if (firstBadOffset < lastBadOffset) 
    {
        DWORD badLength = lastBadOffset - firstBadOffset;

        BYTE buffer[badLength];

        m_nvram.GetData(firstBadOffset, buffer, badLength);

        std::stringstream ss = std::stringstream();

        ss << "Bad NVRAM dump follows. Index ";
        ss << firstBadIndex << " to " << lastBadIndex;
        ss << " Offset " << firstBadOffset << " to " << lastBadOffset;
        ss << CUtilities::ToHexString(buffer, badLength);

        return ss.str();
    }

    return "";
}

bool CMemoryStorage::Write(DWORD offset1, BYTE * data1, UINT len1,
						   DWORD offset2, BYTE * data2, UINT len2,
						   DWORD offset3, BYTE * data3, UINT len3,
						   DWORD offset4, BYTE * data4, UINT len4) const
{
	bool bReturnValue = false;

	if (m_type == MEMORY_NONE)
	{
		bReturnValue = true;
	}
	else if ((len1 + len2 + len3 + len4) <= m_dwAllowedSize)
	{
		switch (m_type)
		{
		case MEMORY_EEPROM:
			{
				bReturnValue = m_eeprom.SetData(offset1, data1, len1);

			}	
			break;
		case MEMORY_NVRAM:
			{
                //BYTE orgByte1 = PokeNVRam(SPRINKLE6_OFFSET, 0);

                #ifndef _UNIT_TEST
                std::stringstream ss = std::stringstream();
                ss << offset1 << " 0x" << len1 
                    << " " << offset2 << " " << len2 
                    << " " << offset3 << " " << len3 
                    << " " << offset4 << " " << len4 << " ";
                CheckSprinkles(true, ss.str());
                #endif

				bReturnValue = m_nvram.SetData(offset1, data1, len1,
					offset2, data2, len2,
					offset3, data3, len3,
					offset4, data4, len4);

                #ifndef _UNIT_TEST
                CheckSprinkles(false, ss.str());
                #endif

                //PokeNVRam(SPRINKLE6_OFFSET, orgByte1);
			}
			break;
        default:
			bReturnValue = false;
			break;
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

bool CMemoryStorage::Read(DWORD offset, BYTE * data, DWORD len)
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
					bReturnValue = m_eeprom.GetData(offset, data, len);
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

bool CMemoryStorage::IsActiveLocation_wrapper(DWORD offset, BYTE value)
{
	return IsActiveLocation(offset, value);
}

bool CMemoryStorage::SetInactiveLocation_wrapper(DWORD offset, BYTE value)
{
	return SetInactiveLocation(offset, value);
}

bool CMemoryStorage::SetInactiveLocation(DWORD offset, BYTE value)
{	
	BYTE data = value;

	bool bReturnValue = m_nvram.SetData(offset, &data, 1);
	if (!bReturnValue)
	{
		// Report the offset to indicate which derived class failed.
		LogString(ERROR_LOG, _T("Could not write inactive memory location to NVRAM offset=%u value=%u"),
			(unsigned)offset,
			(unsigned)value);
	}

	return bReturnValue;
}

bool CMemoryStorage::SetActiveLocation(DWORD offset, BYTE value)
{
	BYTE data = value;

	bool bReturnValue = m_nvram.SetData(offset, &data, 1);
	if (!bReturnValue)
	{
		// Report the offset to indicate which derived class failed.
		LogString(ERROR_LOG, _T("Could not write active memory location to NVRAM offset=%u value=%u"),
			(unsigned)offset,
			(unsigned)value);
	}

	return bReturnValue;
}

// Returns true if the byte at NVRAM offset is has the value indicated.
// Returns false otherwise
bool CMemoryStorage::IsActiveLocation(DWORD offset, BYTE value)
{
	bool bReturnValue;
	BYTE data;

	bReturnValue = m_nvram.GetData(offset, &data, 1);

	if (bReturnValue)
	{
		bReturnValue = (data == value);
	}
	return bReturnValue;
}

void CMemoryStorage::ReleaseShadowBuffer()
{
    m_nvram.ReleaseShadowBuffer();
}

void CMemoryStorage::UpgradeGamesForMGMD()
{
	// Upgrade m_games and associated game | meters collections. CGame::m_lpPaytableID length increased to 21 from 9 WCHARs.
	// 1. Read in m_games collection using shorter paytableID length.
	// 2. Copy m_games to MEMORY_NONE collection.
	// 3. Reset m_games.
	// 4. Copy EGM game back to m_games to get data saved properly.
	// 5. Add subgames from copy to m_games.

	LogString(NVRAM_LOG, "[CMemoryStorage::UpgradeGamesForMGMD]");
	CGames *games = new CGames(true);
	LogString(NVRAM_LOG, "Creating copy of pre-MGMD CGames object...");
	CGames *copy = new CGames(*games);
	LogString(NVRAM_LOG, "Calling ResetGames...");
	games->ResetGames();
	LogString(NVRAM_LOG, "Creating copy of EGM game object from non-NVRAM CGames object...");
	CGame *egmCopy = copy->GetEGM();
	LogString(NVRAM_LOG, "Creating copy of EGM game object from NVRAM CGames object...");
	CGame *egm = games->GetEGM();

	// Copy egm to games which will already have a default egm game.
	if (egm != nullptr && egmCopy != nullptr)
	{
		egm->SetGameData(*egmCopy);
		LogString(NVRAM_LOG, "Calling SetGameMeters on EGM game object of NVRAM CGames object...");
		egm->SetGameMeters(*egmCopy->GetMeters());
	}

	// Add any remaining subgames
	for (int i = 0; i < copy->GetGameCount(); i++)
	{
		CGame *game = copy->GetGameByIndex(i);
		if (!game->IsEGM())
		{
			CGame *newGame = new CGame(*game);
			games->AddGame(newGame);
		}
	}

	delete copy;
	delete games;
}
