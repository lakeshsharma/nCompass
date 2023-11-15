#pragma once

#include <stdafx.h>
#define NVRAM_NULL_VALUE -1
#define NVRAM_SAFE_WRITE_SIZE 256

class CNVRAMinator
{
public:
	CNVRAMinator(void);
public:
	~CNVRAMinator(void);

public:
	bool SetData(DWORD offset1, BYTE * data1, UINT len1,
				 DWORD offset2 = NVRAM_NULL_VALUE, BYTE * data2 = NULL, UINT len2 = 0,
			 	 DWORD offset3 = NVRAM_NULL_VALUE, BYTE * data3 = NULL, UINT len3 = 0,
				 DWORD offset4 = NVRAM_NULL_VALUE, BYTE * data4 = NULL, UINT len4 = 0);

	bool GetData(DWORD offset, BYTE * data, UINT len);

    void ReleaseShadowBuffer();

    void SetToVerifyBlocksUponWrite();

private:
	static std::recursive_mutex m_cs;

	// This will be an entire buffer of all of NVRAM
	byte *m_shadowBuffer;

    bool m_verifyBlocks;

	void InitializeShadowBuffer();

	void AddToOffsetArray( std::vector<DWORD> &wordOffsetArray, DWORD byteOffset, UINT byteLen );

	void ClearRamAccessBlocks( S7LITE_SRAMACCESS &ramAccess );

	bool WriteToNVRam( S7LITE_SRAMACCESS &NVRWrite );

    void VerifyBlocks(S7LITE_SRAMACCESS &blocks);

#ifdef NVRAM_FILE
	void KillWatchdog( void );
#endif // NVRAM_FILE.
    
};
