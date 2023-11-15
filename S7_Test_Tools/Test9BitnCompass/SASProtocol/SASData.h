#pragma once

class CSASData
{
public:
	CSASData(void);
public:
	~CSASData(void);
private:
	BYTE*	m_lpDataBuffer;
	UINT	m_nDataLength;

public:
	void SetData (BYTE* lpDataBuffer, UINT &nDataLength);
	BYTE *GetData();
private:
	unsigned short CRC(unsigned char *s, int len, unsigned short crcval = 0);
};
