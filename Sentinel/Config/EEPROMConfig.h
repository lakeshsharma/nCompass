#pragma once

#include "../Hardware/MemoryStorage.h"

class CEEPROMConfig :
	public CMemoryStorage
{
public:
	CEEPROMConfig(void);
	CEEPROMConfig( CEEPROMConfig &eRHS ); // defined so that normal constructor isn't called
public:
	virtual ~CEEPROMConfig(void);
protected:
	bool Write(DWORD offset1, BYTE * data1, UINT len1,
			   DWORD offset2 = -1, BYTE * data2 = NULL, UINT len2 = 0,
			   DWORD offset3 = -1, BYTE * data3 = NULL, UINT len3 = 0,
			   DWORD offset4 = -1, BYTE * data4 = NULL, UINT len4 = 0);
	bool Read(DWORD offset, BYTE * data, DWORD len);

	static std::recursive_mutex m_cs;
	bool m_memoryReset;

	//------------------------
	// EEPROM Shadow Area
	//------------------------
	static BYTE m_eepromShadow[EEPROM_OFFSET_END];
	WORD m_eepromShadowChecksum;
	WORD CalculateChecksum();

private:
	bool ValidateOffset(DWORD offset1, bool read);

public:
	bool Set(DWORD offset, BYTE * data, UINT len);
	bool Get(DWORD offset, BYTE * data, UINT len);
	BYTE *GetEEPROMShadow( );

	virtual void BuildYourself()
	{
	}

	bool GetMemoryResetStatus(bool bClear = false);
};
