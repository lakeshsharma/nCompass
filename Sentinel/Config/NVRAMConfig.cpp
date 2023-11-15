#include "stdafx.h"
#include "NVRAMConfig.h"
#include "../Hardware/MemoryStorage.h"

CNVRAMConfig::CNVRAMConfig(void)
{
	m_type = MEMORY_NVRAM;
	m_dwAllowedSize = NVRAM_CONFIG_SIZE;
}

CNVRAMConfig::~CNVRAMConfig(void)
{
}


bool CNVRAMConfig::Set(DWORD offset1, BYTE * data1, UINT len1,
			   DWORD offset2, BYTE * data2, UINT len2,
			   DWORD offset3, BYTE * data3, UINT len3,
			   DWORD offset4, BYTE * data4, UINT len4)
{
	m_bOffsetIsSet = true;

    bool retValue = Write(offset1, data1, len1,
		offset2, data2, len2,
		offset3, data3, len3,
		offset4, data4, len4);

	return retValue;
}
	
bool CNVRAMConfig::Get(DWORD offset, BYTE * data, UINT len)
{
	bool retValue = Read(offset, data, len);

	return retValue;
}

