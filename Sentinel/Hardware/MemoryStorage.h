#pragma once

#include "NonVolatileMemoryMaps.h"
#include "EEPROMinator.h"
#include "NVRAMinator.h"

using namespace std;

#define ACTIVE_LOCATION			0xFF
#define INACTIVE_LOCATION		0x00
#define OUT_OF_SPACE_FAILURE	((DWORD)-1)

typedef	enum
{
	MEMORY_NONE,
	MEMORY_EEPROM,
	MEMORY_NVRAM,
	MEMORY_SDCARD
} MEMORY_TYPE;

// Declare friend class(es) for unit test access.
namespace SentinelNativeUnitTests { class EGMAward; }
namespace SentinelNativeUnitTests { class EGMAwardManager; }
namespace SentinelNativeUnitTests { class MemoryStorageEx; }
namespace SentinelNativeUnitTests { class CumulativeProgressiveManagerTests; }

class CMemoryStorage
{
public:
	static const MEMORY_TYPE DEFAULT_MEMORY_TYPE = MEMORY_NONE;
	static const DWORD NVRAM_DEFAULT_OFFSET = 0xcccccccc; // Cannot write to this location.
	static const DWORD NVRAM_DEFAULT_SIZE = 0;

	CMemoryStorage(void);
public:
	virtual ~CMemoryStorage(void);

public:
 
	static bool	IntegrityCheck(std::vector<BYTE> *sprinkles = nullptr);
	static bool	IsNVRAMCleared();
	static void	SetSprinkles();

	/// <summary>
	/// Check if NVRAM needs to be upgraded and upgrade it as needed.
	/// </summary>
	static void UpgradeNVRAM();

	static bool HasOldSprinkleValue();

	//the first byte of each NVRAM item tells if that location is active or not
	static bool IsActiveLocation(DWORD offset, BYTE value = ACTIVE_LOCATION);
	static bool SetInactiveLocation(DWORD offset, BYTE value = INACTIVE_LOCATION);
	
	//these two should be used publicly only when absolutely necessary.
	static CEEPROMinator m_eeprom;
	static CNVRAMinator m_nvram;

	MEMORY_TYPE GetMemoryType( void ) const
	{
		return m_type;
	}

	static DWORD PreviousNVRAMVersion;

protected:
	DWORD		m_dwOffset;
	MEMORY_TYPE	m_type;
	DWORD		m_dwAllowedSize;
	bool		m_bOffsetIsSet;

	virtual bool Write(DWORD offset1, BYTE * data1, UINT len1,
		DWORD offset2 = NVRAM_NULL_VALUE, BYTE * data2 = NULL, UINT len2 = 0,
		DWORD offset3 = NVRAM_NULL_VALUE, BYTE * data3 = NULL, UINT len3 = 0,
		DWORD offset4 = NVRAM_NULL_VALUE, BYTE * data4 = NULL, UINT len4 = 0) const;

	virtual bool Read(DWORD offset, BYTE * data, DWORD len);

	// virtual wrappers of static functions for UTs
	virtual bool IsActiveLocation_wrapper(DWORD offset, BYTE value = ACTIVE_LOCATION);
	virtual bool SetInactiveLocation_wrapper(DWORD offset, BYTE value = INACTIVE_LOCATION);

	virtual void BuildYourself() = 0;
	bool SetActiveLocation(DWORD offset, BYTE value = ACTIVE_LOCATION);
	static void UpgradeGamesForMGMD();

    void ReleaseShadowBuffer();

    void CheckSprinkles(bool before, std::string args) const;
    std::string DumpCorruptNVRAMArea() const;
    BYTE PokeNVRam(DWORD offset, BYTE value) const;


	// Grant "friend" access for testing.
	friend class SentinelNativeUnitTests::EGMAward;
	friend class SentinelNativeUnitTests::EGMAwardManager;
	friend class SentinelNativeUnitTests::MemoryStorageEx;
    friend class SentinelNativeUnitTests::CumulativeProgressiveManagerTests;

 };


