#include "stdafx.h"
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

void CSASData::GenerateChecksum()
{
	//WARNING this places the checksum in the last two bytes of the databuffer
	// we are not using this right now.
	//if ((m_lpDataBuffer != NULL) && (m_nDataLength > 2))
	//{
	//	WORD crcCalc = CRC((unsigned char *)m_lpDataBuffer, m_nDataLength - 2);
	//	*((WORD*)m_lpDataBuffer[m_nDataLength - 2]) = crcCalc;
	//}
}

bool CSASData::VerifyChecksum()
{
	// bool bReturnValue;
	// we aren't using this right now
	//if ((m_lpDataBuffer != NULL) && (m_nDataLength > 2))
	//{
	//	WORD crcCalc = CRC((unsigned char *)m_lpDataBuffer, m_nDataLength - 2);
	//	bReturnValue = *((WORD*)m_lpDataBuffer[m_nDataLength - 2]) == crcCalc;
	//}
	//else
	//{
	//	bReturnValue = false;
	//}

	//return bReturnValue;
	return true;
}

/*
unsigned short CSASData::CRC(unsigned char *s, int len, unsigned short crcval)
{
	unsigned c,q;

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
*/
