#pragma once

#include "../Hardware/MemoryStorage.h"

class CNVRAMConfig :
	public CMemoryStorage
{
public:
	CNVRAMConfig(void);
public:
	~CNVRAMConfig(void);

	bool Set(DWORD offset1, BYTE * data1, UINT len1,
			   DWORD offset2 = -1, BYTE * data2 = NULL, UINT len2 = 0,
			   DWORD offset3 = -1, BYTE * data3 = NULL, UINT len3 = 0,
			   DWORD offset4 = -1, BYTE * data4 = NULL, UINT len4 = 0);
	bool Get(DWORD offset, BYTE * data, UINT len);
	
	virtual void BuildYourself()
	{
	}
};


