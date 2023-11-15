#pragma once
#include "nonvolatilememorymaps.h"
#include "EEPROMinator.h"
#include "NVRAMinator.h"

#define ACTIVE_LOCATION			0xFF
#define INACTIVE_LOCATION		0x00
#define OUT_OF_SPACE_FAILURE	((DWORD)-1)

typedef	enum
{
	MEMORY_NONE,
	MEMORY_EEPROM,
	MEMORY_NVRAM
} MEMORY_TYPE;


class CMemoryStorage
{
public:
	CMemoryStorage(void);
public:
	virtual ~CMemoryStorage(void);

public:
	static bool	IntegrityCheck();
	static void	SetSprinkles();
	static void UpgradeNVRAM();

	//the first byte of each NVRAM item tells if that location is active or not
	static bool IsActiveLocation(DWORD offset, BYTE value = ACTIVE_LOCATION);
	static bool SetInactiveLocation(DWORD offset, BYTE value = INACTIVE_LOCATION);
	
	//these two should be used publicly only when absolutely necessary.
	static CEEPROMinator m_eeprom;
	static CNVRAMinator m_nvram;

	MEMORY_TYPE GetMemoryType( void )
	{
		return m_type;
	}

protected:
	DWORD		m_dwOffset;
	MEMORY_TYPE	m_type;
	DWORD		m_dwAllowedSize;
	bool		m_bOffsetIsSet;



	bool Write(DWORD offset1, BYTE * data1, UINT len1,
			   DWORD offset2 = -1, BYTE * data2 = NULL, UINT len2 = 0,
			   DWORD offset3 = -1, BYTE * data3 = NULL, UINT len3 = 0,
			   DWORD offset4 = -1, BYTE * data4 = NULL, UINT len4 = 0);
	bool Read(DWORD offset, BYTE * data, DWORD len);
	virtual void BuildYourself() = 0;
	bool SetActiveLocation(DWORD offset, BYTE value = ACTIVE_LOCATION);




};
