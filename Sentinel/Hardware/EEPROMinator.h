#pragma once

#define EEPROM_NULL_VALUE -1
#define EEPROM_SAFE_WRITE_SIZE 32

class CEEPROMinator
{
public:
	CEEPROMinator(void);
public:
	~CEEPROMinator(void);

public:
	bool SetData(DWORD offset1, BYTE * data1, UINT len1,
				 DWORD offset2 = EEPROM_NULL_VALUE, BYTE * data2 = NULL, UINT len2 = 0,
			 	 DWORD offset3 = EEPROM_NULL_VALUE, BYTE * data3 = NULL, UINT len3 = 0,
				 DWORD offset4 = EEPROM_NULL_VALUE, BYTE * data4 = NULL, UINT len4 = 0);
	bool GetData(DWORD offset, BYTE * data, UINT len);
	void ResetMemory( void ); // made public so that eeprom can be reset from eepromconfig if eepromconfig detects invalid checksum
	bool GetEEPROMResetStatus() const
	{
		return m_isEEPROMReset;
	}

private:

	void KillWatchdog( void );
	bool IntegrityCheck( void );
	void CheckFile();

	bool m_fileChecked; // Check file once.
	bool m_isEEPROMReset;
};
