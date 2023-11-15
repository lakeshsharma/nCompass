#include "stdafx.h"
#include "PollerMGMDData.h"
#include "Logging/LogString.h"

CPollerMGMDData *CPollerMGMDData::m_instance = NULL;
std::recursive_mutex CPollerMGMDData::m_instanceCS;

CPollerMGMDData & CPollerMGMDData::Instance()
{
	lock_guard<recursive_mutex> lock(m_instanceCS);

	if (m_instance == NULL)
	{
		m_instance = new CPollerMGMDData();
	}
	return *m_instance;
}

CPollerMGMDData::CPollerMGMDData()
{
	m_mgmdAvailable = 0;
	m_mgmdGameStart = 0;
	m_mgmdGameEnd = 0;
	m_mgmdPollingSpeed = BYTE_MAX;
	m_mgmdGlobalOption = 0;
	m_mgmdSenOption = 0;
	m_gameListChunkSizeOption = DEFAULT_GAME_LIST_CHUNK_SIZE;

	m_chunkedGamesListFlag = true;  //Always, assuming to send games in chunks, but real value will be set when config is recvd
	m_configFeatureFlagsRecvd = false;
}

CPollerMGMDData::~CPollerMGMDData(void)
{
}

byte CPollerMGMDData::GetMGMDAvailable()
{
	return m_mgmdAvailable;
}

byte CPollerMGMDData::GetMGMDGlobalOption()
{
	return m_mgmdGlobalOption;
}

byte CPollerMGMDData::GetMGMDGameStart()
{
	return m_mgmdGameStart;
}

byte CPollerMGMDData::GetMGMDGameEnd()
{
	return m_mgmdGameEnd;
}

byte CPollerMGMDData::GetMGMDPollingSpeed()
{
	return m_mgmdPollingSpeed;
}

byte CPollerMGMDData::GetMGMDSenOption()
{
	return m_mgmdSenOption;
}

void CPollerMGMDData::SetMGMDGameStart(byte mgmdGameStart)
{
	m_mgmdGameStart = mgmdGameStart;
	CheckandSetMGMDAvailable();
}

void CPollerMGMDData::SetMGMDGameEnd(byte mgmdGameEnd)
{
	m_mgmdGameEnd = mgmdGameEnd;
	CheckandSetMGMDAvailable();
}

void CPollerMGMDData::SetMGMDPollingSpeed(byte mgmdPollingSpeed)
{
	m_mgmdPollingSpeed = mgmdPollingSpeed;
	CheckandSetMGMDAvailable();
}

void CPollerMGMDData::SetMGMDSenOption(byte mgmdSenOption)
{
	m_mgmdSenOption = mgmdSenOption;
	CheckandSetMGMDAvailable();
}

void CPollerMGMDData::SetMGMDGlobalOption(byte mgmdGlobalOption)
{
	m_mgmdGlobalOption = mgmdGlobalOption;
}

void CPollerMGMDData::CheckandSetMGMDAvailable()
{
	if (m_mgmdSenOption == 1 && m_mgmdPollingSpeed == MS40 && m_mgmdGameEnd == 1 && m_mgmdGameStart == 1)
	{
		m_mgmdAvailable = 1;
	}
	else
	{
		m_mgmdAvailable = 0;
	}
}

WORD CPollerMGMDData::GetGameListChunkSizeOption()
{
	LogString(POLLER_LOG, _T("%s: %d gameListChunkSize - %u"), __TFUNCTION__, __LINE__, m_gameListChunkSizeOption);
	return m_gameListChunkSizeOption;
}

void CPollerMGMDData::SetGameListChunkSizeOption(WORD		gameListChunkSize)
{
	m_gameListChunkSizeOption = gameListChunkSize;
}

bool CPollerMGMDData::IsChunkedGamesListFlagEnabled()
{
	return m_chunkedGamesListFlag;
}

void CPollerMGMDData::SetChunkedGamesListFlag(WORD	chunkedGamesListFlag)
{
	m_chunkedGamesListFlag = chunkedGamesListFlag;
	m_configFeatureFlagsRecvd = true;
	LogString(POLLER_LOG, _T("Chunked Games List Flag: %u"), m_chunkedGamesListFlag);
}

bool CPollerMGMDData::IsConfigFeatureFlagsRecvd()
{
	return m_configFeatureFlagsRecvd;
}

