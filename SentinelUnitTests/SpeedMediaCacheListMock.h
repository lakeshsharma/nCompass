#pragma once

#include "gmock/gmock.h"
#include "SpeedMedia/SpeedMediaCacheList.h"

using namespace std;

class CSpeedMediaCacheListMock : public CSpeedMediaCacheList
{
public:
	CSpeedMediaCacheListMock(string certificateFile = "");
	~CSpeedMediaCacheListMock();

	MOCK_METHOD1(OpenStaticCacheFile, bool(FILE *& cacheIPFile));
	MOCK_METHOD2(ReadNextStaticCacheIP, bool(FILE *cacheIPFile, string &staticCacheIP));
	MOCK_METHOD1(CloseStaticCacheFile, void(FILE *cacheIPFile));
	MOCK_METHOD0(GetTick, DWORD());

	bool OpenStaticCacheFileSuccess(FILE *cacheIPFile);
	bool OpenStaticCacheFileFailure(FILE *cacheIPFile);
	bool ReadNextStaticCacheIPMockGood1(FILE *cacheIPFile, string &staticCacheIP);
	bool ReadNextStaticCacheIPMockGood2(FILE *cacheIPFile, string &staticCacheIP);
	bool ReadNextStaticCacheIPMockGoodPadded(FILE *cacheIPFile, string &staticCacheIP);
	bool ReadNextStaticCacheIPMockBad(FILE *cacheIPFile, string &staticCacheIP);

private:
	int m_readCount;
};

