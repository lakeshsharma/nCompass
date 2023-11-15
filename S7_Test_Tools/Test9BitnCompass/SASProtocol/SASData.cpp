#include "stdafx.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "SASData.h"

CSASData::CSASData(void)
{
	m_lpDataBuffer = NULL;
	m_nDataLength = 0;
}

CSASData::~CSASData(void)
{
}

void CSASData::SetData (BYTE* lpDataBuffer, UINT &nDataLength)
{
	m_lpDataBuffer = lpDataBuffer;
	m_nDataLength = nDataLength;
}

BYTE *CSASData::GetData()
{
	return m_lpDataBuffer;
}



unsigned short CSASData::CRC(unsigned char *s, int len, unsigned short crcval)
{
	unsigned short c,q;

	for (; len; len--)
	{
		c = *s++;
		q = (crcval ^ c) & 017;
		crcval = (crcval >> 4) ^ (q * 010201);
		q = (crcval ^ (c >> 4)) & 017;
		crcval = (crcval >> 4) ^ (q * 010201);
	}
	return (crcval);
}