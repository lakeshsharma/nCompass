#pragma once
#include "s7lite.h"

class CSASSerialWrapper
{
public:
	CSASSerialWrapper(void);
	~CSASSerialWrapper(void);

	S7_Result OpenSAS(void);
	void CloseSAS(void);

	S7_Result Write(byte * buffer, DWORD bufferLen, bool mark, DWORD * numberBytesWritten);
	S7_Result Read(byte * buffer, DWORD bufferLen, DWORD * numberBytesRead);

public:
	byte * m_LastWrittenBufer;
	byte * m_LastReadBufer;
};

union Ser9BitData {
	USHORT sp;
	WORD wp[1];
	BYTE cp[2];
};
