#include "stdafx.h"
#include "SpeedMediaCacheListMock.h"


CSpeedMediaCacheListMock::CSpeedMediaCacheListMock(string certificateFile) : CSpeedMediaCacheList(certificateFile)
{
    m_readCount = 0;
}


CSpeedMediaCacheListMock::~CSpeedMediaCacheListMock()
{
}

bool CSpeedMediaCacheListMock::OpenStaticCacheFileSuccess(FILE *cacheIPFile)
{
    m_readCount = 0;
    return true;
}

bool CSpeedMediaCacheListMock::OpenStaticCacheFileFailure(FILE *cacheIPFile)
{
    return false;
}

bool CSpeedMediaCacheListMock::ReadNextStaticCacheIPMockGood1(FILE *cacheIPFile, string &staticCacheIP)
{
    bool retVal = false;
    if (m_readCount == 0)
    {
        staticCacheIP = "10.10.10.1";
        retVal = true;
        m_readCount++;
    }

    return retVal;
}

bool CSpeedMediaCacheListMock::ReadNextStaticCacheIPMockGood2(FILE *cacheIPFile, string &staticCacheIP)
{
    bool retVal = false;
    if (m_readCount == 0)
    {
        staticCacheIP = "10.10.10.1";
        retVal = true;
        m_readCount++;
    }
    else if (m_readCount == 1)
    {
        staticCacheIP = "10.10.10.2";
        retVal = true;
        m_readCount++;
    }

    return retVal;
}

bool CSpeedMediaCacheListMock::ReadNextStaticCacheIPMockGoodPadded(FILE *cacheIPFile, string &staticCacheIP)
{
    bool retVal = false;
    if (m_readCount == 0)
    {
        staticCacheIP = " 10.10.10.3   ";
        retVal = true;
        m_readCount++;
    }
    return retVal;
}

bool CSpeedMediaCacheListMock::ReadNextStaticCacheIPMockBad(FILE *cacheIPFile, string &staticCacheIP)
{
    bool retVal = false;
    if (m_readCount == 0)
    {
        staticCacheIP = "ThisIsNotAnIP";
        retVal = true;
        m_readCount++;
    }
    return retVal;
}

