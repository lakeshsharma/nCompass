#include "stdafx.h"
#include "EEPROMinator.h"

// Sentinel includes.
#include "DirectoryManager.h"
#include "Hardware/WatchDog.h"
#include "Logging/LogString.h"
#include "IOCTLOffsets.h"
#include "NonVolatileMemoryMaps.h"
#include "Utilities.h"
#include "../CheckSum.h"
#include "Diagnostics.h"

using namespace std;

CEEPROMinator::CEEPROMinator(void)
	:
	m_fileChecked(false),
	m_isEEPROMReset(false)
{
	// Functionality from old HANDLE CEEPROMinator::CDevice::GetHandle(void).
	if (!IntegrityCheck())
	{
		// Failing integrity check will cause debug files to be created before timezone offset can be read from NVRAM resulting in debug filenames with UTC time.
		ResetMemory();
	}
}

CEEPROMinator::~CEEPROMinator(void)
{
}

void CEEPROMinator::KillWatchdog( void )
{
	CDiagnostics::Instance().PersistReboot(tltRebootMemoryError, ERROR_LOG, DIAGNOSTICS_PERSIST_NVRAM_ONLY);
#ifdef WATCHDOG
	CWatchDog::Instance().BeginShutdown(CWebDiagnosticsStatusInfo::TiltCodeToString(tltRebootMemoryError));
#endif
}

bool CEEPROMinator::IntegrityCheck( void )
{
	bool retVal = false;

	byte checkValue = 0;
	if ( GetData( EEPROM_CHECKBYTE_OFFSET, &checkValue, sizeof(byte) ) )
	{
		if ( checkValue == EEPROM_CHECKBYTE_VALUE )
		{
			retVal = true;
		}
	}

	else
	{
		// If we can't even read the check byte, we need to reset.
		KillWatchdog();
	}

	return retVal;
}

void CEEPROMinator::ResetMemory( void )
{
	m_isEEPROMReset = true;
	// Clear all usable EEPROM locations before setting defaults.
	// The Hardware guarantees a power-fail-safe write of up to 32 bytes
	// of course more can be written.
	BYTE buffer[EEPROM_SAFE_WRITE_SIZE];
	ZeroMemory(buffer, sizeof buffer);
	for (DWORD offset = EEPROM_OFFSET_START; offset <= EEPROM_OFFSET_END; /* Variable increment. */)
	{
		DWORD len = sizeof buffer;
		len = min(len, EEPROM_MAX_WRITE); // Limit writes to maximum write length.
		len = min(len, (EEPROM_OFFSET_END + 1) - offset); // Limit normal writes to usable EEPROM locations.
		len = min(len, EEPROM_PAGE_SIZE - (offset % EEPROM_PAGE_SIZE)); // Limit writes to same page.
		SetData(offset, buffer, len);
		offset += len;
	}

	// Clearing checksum MUST be done in a separate write, per SetData requirements.  2011-05-04
	SetData(EEPROM_CHECKSUM_OFFSET, (BYTE *)&buffer, EEPROM_CHECKSUM_SIZE);

	// Set EEPROM defaults.
	SetData(EEPROM_CHECKBYTE_OFFSET,	(BYTE *)&EEPROM_CHECKBYTE_VALUE,		EEPROM_CHECKBYTE_SIZE,
			EEPROM_SLOTMAST_ID_OFFSET,	(BYTE *)&EEPROM_SLOTMAST_ID_DEFAULT,	EEPROM_SLOTMAST_ID_SIZE,
			EEPROM_CASINO_NUMBER_OFFSET,(BYTE *)&EEPROM_CASINO_NUMBER_DEFAULT,	EEPROM_CASINO_NUMBER_SIZE,
			EEPROM_COMM_ID_OFFSET,		(BYTE *)&EEPROM_COMM_ID_DEFAULT,		EEPROM_COMM_ID_SIZE );

	SetData(EEPROM_MACHINE_TYPE_OFFSET,	(BYTE *)&EEPROM_MACHINE_TYPE_DEFAULT,	EEPROM_MACHINE_TYPE_SIZE,
			EEPROM_SLOT_NUMBER_OFFSET,	(BYTE *)&EEPROM_SLOT_NUMBER_DEFAULT,	EEPROM_SLOT_NUMBER_SIZE );

	byte hardPinTypesDefault[EEPROM_HARD_PIN_TYPES_SIZE];
	for ( int ti = 0; ti < (int)EEPROM_HARD_PIN_TYPES_SIZE; ti++ )
	{
		hardPinTypesDefault[ti] = EEPROM_HARD_PIN_TYPES_DEFAULT;
	}
	SetData(EEPROM_HARD_PIN_TYPES_OFFSET,		(BYTE *)hardPinTypesDefault,					EEPROM_HARD_PIN_TYPES_SIZE );

	SetData(EEPROM_HARD_PIN_ACTIVE_STATE_OFFSET,(BYTE *)&EEPROM_HARD_PIN_ACTIVE_STATE_DEFAULT,	EEPROM_HARD_PIN_ACTIVE_STATE_SIZE,
			EEPROM_SERIAL_PROTOCOL_OFFSET,		(BYTE *)&EEPROM_SERIAL_PROTOCOL_DEFAULT,		EEPROM_SERIAL_PROTOCOL_SIZE );

	SetData(EEPROM_GAME_DENOM_OFFSET, (BYTE *)&EEPROM_GAME_DENOM_DEFAULT, EEPROM_GAME_DENOM_SIZE);

	SetData(EEPROM_SCREEN_BRIGHTNESS_OFFSET,	(BYTE *)&EEPROM_SCREEN_BRIGHTNESS_DEFAULT,		EEPROM_SCREEN_BRIGHTNESS_SIZE,
			EEPROM_LEFT_SPEAKER_VOLUME_OFFSET,	(BYTE *)&EEPROM_LEFT_SPEAKER_VOLUME_DEFAULT,	EEPROM_LEFT_SPEAKER_VOLUME_SIZE,
			EEPROM_RIGHT_SPEAKER_VOLUME_OFFSET,	(BYTE *)&EEPROM_RIGHT_SPEAKER_VOLUME_DEFAULT,	EEPROM_RIGHT_SPEAKER_VOLUME_SIZE );

	char globalCardPasswordDefault[EEPROM_GLOBAL_CARD_PASS_SIZE];
	for ( int ti = 0; ti < (int)EEPROM_GLOBAL_CARD_PASS_SIZE; ti++ )
	{
		globalCardPasswordDefault[ti] = EEPROM_GLOBAL_CARD_PASS_DEFAULT;
	}
	SetData(EEPROM_GLOBAL_CARD_PASS_OFFSET,		(BYTE *)globalCardPasswordDefault,	EEPROM_GLOBAL_CARD_PASS_SIZE);

	SetData(EEPROM_DHCPAUTO_OFFSET,				(BYTE *)&EEPROM_DHCPAUTO_DEFAULT,	EEPROM_DHCPAUTO_SIZE,
			EEPROM_HOST_PORT_OFFSET,			(BYTE *)&EEPROM_HOST_PORT_DEFAULT,	EEPROM_HOST_PORT_SIZE);

	//the host name crosses the 128 page bound of the EEPROM
	//so we need to break it into 2 blocks at the 128 page bound
	char hostAddressDefaultFirstBlock[EEPROM_HOST_ADDRESS_FIRST_BLOCK_SIZE];
	char hostAddressDefaultSecondBlock[EEPROM_HOST_ADDRESS_SECOND_BLOCK_SIZE];
	for ( int ti = 0; ti < (int)EEPROM_HOST_ADDRESS_FIRST_BLOCK_SIZE; ti++ )
	{
		hostAddressDefaultFirstBlock[ti] = EEPROM_HOST_ADDRESS_DEFAULT;
	}
	for ( int ti = 0; ti < (int)EEPROM_HOST_ADDRESS_SECOND_BLOCK_SIZE; ti++ )
	{
		hostAddressDefaultSecondBlock[ti] = EEPROM_HOST_ADDRESS_DEFAULT;
	}
	SetData(EEPROM_HOST_ADDRESS_OFFSET,	(BYTE *)hostAddressDefaultFirstBlock, EEPROM_HOST_ADDRESS_FIRST_BLOCK_SIZE,
		EEPROM_HOST_ADDRESS_OFFSET + EEPROM_HOST_ADDRESS_FIRST_BLOCK_SIZE, (BYTE *)hostAddressDefaultSecondBlock, EEPROM_HOST_ADDRESS_SECOND_BLOCK_SIZE);

	byte ipDefault[IP_ADDRESS_SIZE];
	for ( int ti = 0; ti < IP_ADDRESS_SIZE; ti++ )
	{
		ipDefault[ti] = EEPROM_MY_IP_DEFAULT;
	}
	SetData(EEPROM_MY_IP_OFFSET,				(BYTE *)ipDefault,					EEPROM_MY_IP_SIZE);

	for ( int ti = 0; ti < IP_ADDRESS_SIZE; ti++ )
	{
		ipDefault[ti] = EEPROM_DEFAULT_GATEWAY_DEFAULT;
	}
	SetData(EEPROM_DEFAULT_GATEWAY_OFFSET,		(BYTE *)ipDefault,					EEPROM_DEFAULT_GATEWAY_SIZE);

	for ( int ti = 0; ti < IP_ADDRESS_SIZE; ti++ )
	{
		ipDefault[ti] = EEPROM_SUBNET_MASK_DEFAULT;
	}
	SetData(EEPROM_SUBNET_MASK_OFFSET,			(BYTE *)ipDefault,					EEPROM_SUBNET_MASK_SIZE);

	for ( int ti = 0; ti < IP_ADDRESS_SIZE; ti++ )
	{
		ipDefault[ti] = EEPROM_PREFERRED_DNS_DEFAULT;
	}
	SetData(EEPROM_PREFERRED_DNS_OFFSET,		(BYTE *)ipDefault,					EEPROM_PREFERRED_DNS_SIZE);

	for ( int ti = 0; ti < IP_ADDRESS_SIZE; ti++ )
	{
		ipDefault[ti] = EEPROM_ALT_DNS_DEFAULT;
	}
	SetData(EEPROM_ALT_DNS_OFFSET,				(BYTE *)ipDefault,					EEPROM_ALT_DNS_SIZE);

	SetData(EEPROM_EGM_TYPE_OFFSET,				(BYTE *)&EEPROM_EGM_TYPE_DEFAULT,			EEPROM_EGM_TYPE_SIZE,
			EEPROM_EGM_POLLING_SPEED_OFFSET,	(BYTE *)&EEPROM_EGM_POLLING_SPEED_DEFAULT,	EEPROM_EGM_POLLING_SPEED_SIZE,
			EEPROM_EGM_PROTOCOL_OFFSET,			(BYTE *)&EEPROM_EGM_PROTOCOL_DEFAULT,		EEPROM_EGM_PROTOCOL_SIZE );

	SetData(EEPROM_SCREEN_CALIBRATED_FLAG_OFFSET, (BYTE *)&EEPROM_SCREEN_CALIBRATED_DEFAULT, EEPROM_SCREEN_CALIBRATED_FLAG_SIZE);

	SetData(EEPROM_EGM_PROTOCOL_STATE_OFFSET,		(BYTE *)&EEPROM_EGM_PROTOCOL_STATE_DEFAULT,		EEPROM_EGM_PROTOCOL_STATE_SIZE,
			EEPROM_SENTINEL_ONLINE_STATE_OFFSET,	(BYTE *)&EEPROM_SENTINEL_ONLINE_STATE_DEFAULT,	EEPROM_SENTINEL_ONLINE_STATE_SIZE,
			EEPROM_BATTERY_STATUS_OFFSET,			(BYTE *)&EEPROM_BATTERY_STATUS_DEFAULT,			EEPROM_BATTERY_STATUS_SIZE );
	SetData(EEPROM_SERVICE_WINDOW_LOCATION_OFFSET, (BYTE *)&EEPROM_SERVICE_WINDOW_LOCATION_DEFAULT, EEPROM_SERVICE_WINDOW_LOCATION_SIZE,
			EEPROM_SERVICE_WINDOW_SIZE_OFFSET, (BYTE *)&EEPROM_SERVICE_WINDOW_SIZE_DEFAULT, EEPROM_SERVICE_WINDOW_SIZE_SIZE,
			EEPROM_TASKBAR_SIZE_OFFSET, (BYTE *)&EEPROM_TASKBAR_SIZE_DEFAULT, EEPROM_TASKBAR_SIZE);

	SetData(EEPROM_TSPROTO_TYPE_OFFSET, (BYTE *)&EEPROM_TSPROTO_TYPE_RESET, EEPROM_TSPROTO_TYPE_SIZE,
		EEPROM_TSPROTO_VENDOR_ID_OFFSET, (BYTE *)&EEPROM_TSPROTO_VENDOR_ID_DEFAULT, EEPROM_TSPROTO_VENDOR_ID_SIZE,
		EEPROM_TSPROTO_BAUD_OFFSET, (BYTE *)&EEPROM_TSPROTO_BAUD_DEFAULT, EEPROM_TSPROTO_BAUD_SIZE);

	SetData(EEPROM_MIXER_REBOOT_COUNT_OFFSET, (BYTE *)&EEPROM_MIXER_REBOOT_COUNT_DEFAULT, EEPROM_MIXER_REBOOT_COUNT_SIZE);

	SetData(EEPROM_VIDEO_INPUT_AUTO_DETECT_OFFSET, (BYTE *)&EEPROM_VIDEO_INPUT_AUTO_DETECT_DEFAULT, EEPROM_VIDEO_INPUT_AUTO_DETECT_SIZE,
		EEPROM_VIDEO_OUTPUT_AUTO_CONFIG_OFFSET, (BYTE *)&EEPROM_VIDEO_OUTPUT_AUTO_CONFIG_DEFAULT, EEPROM_VIDEO_OUTPUT_AUTO_CONFIG_SIZE);

	SetData(EEPROM_DESKTOP_ORIENTATION_OFFSET, (BYTE *)&EEPROM_DESKTOP_ORIENTATION_DEFAULT, EEPROM_DESKTOP_ORIENTATION_SIZE);

	SetData(EEPROM_MW_EGM_ASPECT_RATIO_OFFSET,		(BYTE *)&EEPROM_MW_EGM_ASPECT_DEFAULT,				EEPROM_MW_EGM_ASPECT_RATIO_SIZE);
	SetData(EEPROM_MW_CARD_IN_PLAYER_STATE_OFFSET,	(BYTE *)&EEPROM_MW_CARD_IN_PLAYER_STATE_DEFAULT,	EEPROM_MW_CARD_IN_PLAYER_STATE_SIZE);
	SetData(EEPROM_MW_CARD_OUT_PLAYER_STATE_OFFSET,	(BYTE *)&EEPROM_MW_CARD_OUT_PLAYER_STATE_DEFAULT,	EEPROM_MW_CARD_OUT_PLAYER_STATE_SIZE);
	SetData(EEPROM_MW_AUTO_OPEN_OFFSET,				(BYTE *)&EEPROM_MW_AUTO_OPEN_DEFAULT,				EEPROM_MW_AUTO_OPEN_SIZE);
	SetData(EEPROM_MW_AUTO_CLOSE_OFFSET,			(BYTE *)&EEPROM_MW_AUTO_CLOSE_DEFAULT,				EEPROM_MW_AUTO_CLOSE_SIZE);
	SetData(EEPROM_MW_AUTO_MINUTES_OFFSET,			(BYTE *)&EEPROM_MW_AUTO_MINUTES_DEFAULT,			EEPROM_MW_AUTO_MINUTES_SIZE);

	SetData(EEPROM_REBOOT_DIAGNOSTIC_TILTCODE_OFFSET, (BYTE *)&EEPROM_REBOOT_DIAGNOSTIC_TILTCODE_DEFAULT, EEPROM_REBOOT_DIAGNOSTIC_TILTCODE_SIZE);

	SetData(EEPROM_MW_BONUS_EVENT_PLAYER_STATE_OFFSET, (BYTE *)&EEPROM_MW_BONUS_EVENT_PLAYER_STATE_DEFAULT, EEPROM_MW_BONUS_EVENT_PLAYER_STATE_SIZE);

	// Set the battery check time to a time in the past to insure it gets checked
	byte timeBuffer[NVRAM_TIME_SIZE];
	time_t oldTime(CUtilities::GetTimeFromFields(2000, 1, 1, 0, 0, 0));
	CUtilities::GetNvramBufferFromTime( oldTime, timeBuffer, NVRAM_TIME_SIZE );
	SetData(EEPROM_BATTERY_STATUS_TIME_OFFSET,	(BYTE *)timeBuffer,					EEPROM_BATTERY_STATUS_TIME_SIZE);

	// Calculate new checksum
	CCheckSum cs(true);
	BYTE tmpEeprom[EEPROM_OFFSET_END];
	GetData(EEPROM_OFFSET_START, tmpEeprom, EEPROM_OFFSET_END);

	cs.Add((LPBYTE)tmpEeprom, EEPROM_OFFSET_END);
	// Write checksum
	WORD eepromCS = (WORD)cs.Get();
	SetData(EEPROM_CHECKSUM_OFFSET, (BYTE *)&eepromCS, sizeof(WORD));

}

/// <summary>
/// Ensure that the (persistent) file for EEPROM storage exists and is at least the size we need.
/// </summary>
void CEEPROMinator::CheckFile()
{
	// Only check the file once.
	if (!m_fileChecked)
	{
		m_fileChecked = true;
		static const string eepromFilename(CDirectoryManager::Instance().EepromBinFilePath());
		
		// Try to open (or create) the file for appending.
		ofstream eepromStream(eepromFilename, ios_base::out | ios_base::app | ofstream::binary);
		if (!eepromStream)
		{
			string errorString(CUtilities::GetLastErrorCodeString());
			LogString(ERROR_LOG, "Error: could not open \"%s\" EEPROM file for file check - rdstate=%d %s",
				static_cast<LPCSTR>(eepromFilename.c_str()),
				(int)eepromStream.rdstate(), static_cast<LPCSTR>(errorString.c_str()));
		}
		else
		{
			if (eepromStream.tellp() < EEPROM_TOTAL_SIZE)
			{
				LogString(EEPROM_LOG, "Info: Increasing size of \"%s\" EEPROM file from %lld bytes to %lld bytes",
					static_cast<LPCSTR>(eepromFilename.c_str()), (long long)eepromStream.tellp(), (long long)EEPROM_TOTAL_SIZE);
			}

			while (eepromStream && eepromStream.tellp() < EEPROM_TOTAL_SIZE)
			{
				eepromStream << (char)0;
				if (!eepromStream)
				{
					string errorString(CUtilities::GetLastErrorCodeString());
					LogString(ERROR_LOG, "Error: could not write to \"%s\" EEPROM file offset=%lld for file check - rdstate=%d %s",
						static_cast<LPCSTR>(eepromFilename.c_str()), (long long)eepromStream.tellp(),
						(int)eepromStream.rdstate(), static_cast<LPCSTR>(errorString.c_str()));
				}
			}
		}
	}
}

bool CEEPROMinator::SetData(DWORD offset1, BYTE * data1, UINT len1,
			 DWORD offset2, BYTE * data2, UINT len2,
			 DWORD offset3, BYTE * data3, UINT len3,
			 DWORD offset4, BYTE * data4, UINT len4)
{
	CheckFile();

	///Sentinel 5 Specifics:
	// Due to the way XP drivers work, the memory passed to the driver must be in one chunk.
	// immediately following the DBD.
	// The passed in memory will be copied to the appropriate spot.


	// The Hardware guarantees a power-fail-safe write of up to 32 bytes
	// of course more can be written.

	// Side Effect: if any offset is set to -1, subsequent blocks will not be written.

	int nDataBlocks = 4;
	bool bOutOfRange = false;
	BOOL ioResult = FALSE;

	// How much is being written and is it asking to write out of range?
	//---------------------------------------------------
	if ((int)offset4 == EEPROM_NULL_VALUE)
	{
		nDataBlocks = 3;
	}
	else if (offset4 + len4 > EEPROM_OFFSET_END + 1)
	{
		bOutOfRange = true;
	}

	if ((int)offset3 == EEPROM_NULL_VALUE)
	{
		nDataBlocks = 2;
	}
	else if (offset3 + len3 > EEPROM_OFFSET_END + 1)
	{
		bOutOfRange = true;
	}

	if ((int)offset2 == EEPROM_NULL_VALUE)
	{
		nDataBlocks = 1;
	}
	else if (offset2 + len2 > EEPROM_OFFSET_END + 1)
	{
		bOutOfRange = true;
	}

	if ((int)offset1 == EEPROM_NULL_VALUE)
	{
		nDataBlocks = 0;
	}
	// Adjust else if for case of checksum write
	else if ((offset1 + len1 > EEPROM_OFFSET_END + 1) && (offset1 != EEPROM_CHECKSUM_OFFSET || len1 != 2))
	{
		bOutOfRange = true;
	}
	//---------------------------------------------------

	// if everything's ok, then write to EEPROM
	if (nDataBlocks && !bOutOfRange)
	{
		DWORD sizeofDBD = sizeof(TEPRDataBlockDescriptor) * nDataBlocks;
		DWORD dataSize = sizeofDBD + len1 + len2 + len3 + len4;
		// allocate enough data for all dbd's and all data passed in
		BYTE *data = new BYTE[ dataSize ];
		TEPRDataBlockDescriptor *EPRWrite = (TEPRDataBlockDescriptor *)data;

		// fill the data buffer
		if (nDataBlocks > 0)
		{
			EPRWrite[0].dwOffset = offset1;
			// fixup first offset with number of blocks
			EPRWrite[0].dwOffset |= (nDataBlocks << 24);
			EPRWrite[0].dataPtr = data + sizeofDBD;   //data1;
			memcpy(EPRWrite[0].dataPtr, data1, len1);
			EPRWrite[0].dataLen = len1;

			if (nDataBlocks > 1)
			{
				EPRWrite[1].dwOffset = offset2;
				EPRWrite[1].dataPtr = EPRWrite[0].dataPtr + len1; //data2;
				memcpy(EPRWrite[1].dataPtr, data2, len2);
				EPRWrite[1].dataLen = len2;

				if (nDataBlocks > 2)
				{
					EPRWrite[2].dwOffset = offset3;
					EPRWrite[2].dataPtr = EPRWrite[1].dataPtr + len2; //data3;
					memcpy(EPRWrite[2].dataPtr, data3, len3);
					EPRWrite[2].dataLen = len3;
	
					if (nDataBlocks > 3)
					{
						EPRWrite[3].dwOffset = offset4;
						EPRWrite[3].dataPtr = EPRWrite[2].dataPtr + len3; //data4;
						memcpy(EPRWrite[3].dataPtr, data4, len4);
						EPRWrite[3].dataLen = len4;
					}
				}
			}
		}

		{
			// Try to write the data to the file.
			string eepromFilename(CDirectoryManager::Instance().EepromBinFilePath());

			ioResult = TRUE;
			// Open the file for both input and output to avoid truncation.
			fstream eepromStream(eepromFilename, ios_base::in | ios_base::out | ofstream::binary);
			if (!eepromStream)
			{
				string errorString(CUtilities::GetLastErrorCodeString());
				ioResult = FALSE;
				LogString(ERROR_LOG, "Error: could not open \"%s\" EEPROM file for writing - rdstate=%d %s",
					static_cast<LPCSTR>(eepromFilename.c_str()),
					(int)eepromStream.rdstate(), static_cast<LPCSTR>(errorString.c_str()));
			}
			else
			{
				// Undo fixup first offset with number of blocks.
				EPRWrite[0].dwOffset &= (1 << 24) - 1;

				for (int blockIndex(0); blockIndex < nDataBlocks; ++blockIndex)
				{
					if (EPRWrite[blockIndex].dwOffset + (QWORD)EPRWrite[blockIndex].dataLen > EEPROM_TOTAL_SIZE)
					{
						LogString(ERROR_LOG, "Error: dwOffset=%u + dataLen=%u >EEPROM_TOTAL_SIZE=%llu for EEPROM write blockIndex=%d", (unsigned)EPRWrite[blockIndex].dwOffset, (unsigned)EPRWrite[blockIndex].dataLen, (unsigned long long)EEPROM_TOTAL_SIZE, (int)blockIndex);
					}
					else
					{
						eepromStream.seekp(EPRWrite[blockIndex].dwOffset);
						if (!eepromStream)
						{
							string errorString(CUtilities::GetLastErrorCodeString());
							ioResult = FALSE;
							LogString(ERROR_LOG, "Error: could not seek to %u on \"%s\" EEPROM file for writing - rdstate=%d %s",
								(unsigned)EPRWrite[blockIndex].dwOffset, static_cast<LPCSTR>(eepromFilename.c_str()),
								(int)eepromStream.rdstate(), static_cast<LPCSTR>(errorString.c_str()));
						}
						else
						{
							eepromStream.write((const char *)EPRWrite[blockIndex].dataPtr, EPRWrite[blockIndex].dataLen);
							if (!eepromStream)
							{
								string errorString(CUtilities::GetLastErrorCodeString());
								ioResult = FALSE;
								LogString(ERROR_LOG, "Error: could not write %u bytes at %u on \"%s\" EEPROM file for writing - rdstate=%d %s",
									(unsigned)EPRWrite[blockIndex].dataLen, (unsigned)EPRWrite[blockIndex].dwOffset, static_cast<LPCSTR>(eepromFilename.c_str()),
									(int)eepromStream.rdstate(), static_cast<LPCSTR>(errorString.c_str()));
							}
						}
					}
				}
			}
		}

		delete [] data;
	}

	if (!ioResult)
	{
		if ( bOutOfRange )
		{
#ifdef LOGGING
			LogString(ERROR_LOG, "Error - attempting out of range write to EEPROM" );
#endif	
		}
		if ( nDataBlocks < 0 )
		{
#ifdef LOGGING
			LogString(ERROR_LOG, "Error - no data blocks to write to EEPROM" );
#endif
		}
		else
		{
#ifdef LOGGING
			LogString(ERROR_LOG, "Error writing to EEPROM: Code: %d", CUtilities::GetLastError());
#endif
		}
	}

	return (bool)ioResult;
}

bool CEEPROMinator::GetData(DWORD offset, BYTE * data, UINT len)
{	
	CheckFile();

	bool bOutOfRange = false;
	BOOL ioResult = FALSE;
	TEPRDataBlockDescriptor EPRRead;
	DWORD dwBytesReturned = 0;

	if ((offset + len > EEPROM_OFFSET_END) && (offset != EEPROM_CHECKSUM_OFFSET || len != 2))
	{
		bOutOfRange = true;
	}

	if (!bOutOfRange)
	{
		EPRRead.dataPtr = data;
		EPRRead.dataLen = len;
		EPRRead.dwOffset = offset;

		{
			// Try to read the data from the file.
			static const string eepromFilename(CDirectoryManager::Instance().EepromBinFilePath());

			ifstream eepromStream(eepromFilename, ios_base::in | ofstream::binary);
			
			if (!eepromStream)
			{
				string errorString(CUtilities::GetLastErrorCodeString());
				LogString(ERROR_LOG, "Error: could not open \"%s\" EEPROM file for reading - rdstate=%d %s",
					static_cast<LPCSTR>(eepromFilename.c_str()),
					(int)eepromStream.rdstate(), static_cast<LPCSTR>(errorString.c_str()));
			}
			else
			{

				if (EPRRead.dwOffset + (QWORD)EPRRead.dataLen > EEPROM_TOTAL_SIZE)
				{
					LogString(ERROR_LOG, "Error: dwOffset=%u + dataLen=%u >EEPROM_TOTAL_SIZE=%llu for EEPROM read", (unsigned)EPRRead.dwOffset, (unsigned)EPRRead.dataLen, (unsigned long long)EEPROM_TOTAL_SIZE);
				}
				else
				{
					eepromStream.seekg(EPRRead.dwOffset);
					if (!eepromStream)
					{
						string errorString(CUtilities::GetLastErrorCodeString());
						LogString(ERROR_LOG, "Error: could not seek to %u on \"%s\" file for EEPROM reading - rdstate=%d %s",
							(unsigned)EPRRead.dwOffset, static_cast<LPCSTR>(eepromFilename.c_str()),
							(int)eepromStream.rdstate(), static_cast<LPCSTR>(errorString.c_str()));
					}
					else
					{
						eepromStream.read((char *)EPRRead.dataPtr, EPRRead.dataLen);
						if (!eepromStream)
						{
							string errorString(CUtilities::GetLastErrorCodeString());
							LogString(ERROR_LOG, "Error: could not read %u bytes at %u on \"%s\" EEPROM file for reading - rdstate=%d %s",
								(unsigned)EPRRead.dataLen, (unsigned)EPRRead.dwOffset, static_cast<LPCSTR>(eepromFilename.c_str()),
								(int)eepromStream.rdstate(), static_cast<LPCSTR>(errorString.c_str()));
						}
						else
						{
							ioResult = true;
							dwBytesReturned = EPRRead.dataLen;
						}
					}
				}
			}
		}

		//A short read is a failed read
		if (dwBytesReturned != len)
		{
			ioResult = FALSE;
		}
	}

	if (!ioResult )
	{
		if ( bOutOfRange )
		{
#ifdef LOGGING
			LogString(ERROR_LOG, "Error - attmpting out of range read from EEPROM" );
#endif
		}
		if ( dwBytesReturned == 0 )
		{
#ifdef LOGGING
			LogString(ERROR_LOG, "Error - no data read from EEPROM" );
#endif
		}
		else
		{
#ifdef LOGGING
			LogString(ERROR_LOG, "Error reading from EEPROM: Code: %d", CUtilities::GetLastError());
#endif
		}
	}

	return (bool)ioResult;
}
