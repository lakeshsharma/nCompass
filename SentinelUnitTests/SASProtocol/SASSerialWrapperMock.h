#pragma once

#include <gmock/gmock.h>
#include "SASProtocol/SASSerialWrapper.h"

using namespace std;

class CSASSerialWrapperMock :
	public CSASSerialWrapper
{
public:
	MOCK_METHOD0(OpenSAS, void());
	MOCK_METHOD0(CloseSAS, void());
	MOCK_METHOD3(ReadSAS, void(byte * buffer, DWORD bufferLen, DWORD &numberBytesRead));
	MOCK_METHOD3(WriteSAS, void(const byte * buffer, DWORD bufferLen, bool mark));
};

