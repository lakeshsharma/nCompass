#include "stdafx.h"
#include "PollerGameListInfo.h"
#include "InternalMessage.h"
#include "Logging/LogString.h"
#include "PollerMGMDData.h"

#include <functional>
#include <chrono>
#include <future>
#include <random>

static const int GAME_LIST_MESSAGE_START_TIME_IN_SECS = 0;
static const int GAME_LIST_MESSAGE_END_TIME_IN_SECS = 120;

CPollerGameListInfo::CPollerGameListInfo(CPollerProtocol* pollerprotocol)
	:rand_generator(random_number_engine()),
	rand_number_distributor(GAME_LIST_MESSAGE_START_TIME_IN_SECS, GAME_LIST_MESSAGE_END_TIME_IN_SECS)
{
	m_parseGameList = new CParseGameList();
	m_implementedGamesCount = 0;
	m_DenomGamesReceived = false;
	m_GlobalMGMD = false;
	m_PollerConnected = false;
	m_AllowGameListbeSenttoPoller = true;
	m_configFeatureFlagsRecvd = false;
	ResetGameChunksData();

	m_pollerprotocol = dynamic_cast <CPollerProtocolInterface*>(pollerprotocol);
	m_systemProtocol = dynamic_cast <CSystemProtocol*>(pollerprotocol);
}

void CPollerGameListInfo::ProcessDenomGamesFromInternalsAndSendMessageToPoller(CInternalMessage *msg)
{
	LogString(GAMES, "Received Denom Games in CPollerGameListInfo");
	m_parseGameList->ParseDenomGames(msg->GetGamesData());
	m_DenomGamesReceived = true;

	ProcessConfigFeatureFlagsRecvd();
	
	CheckandSendGameListtoPoller();
}

void CPollerGameListInfo::ProcessEnabledGamesFromInternals(CInternalMessage *msg)
{
	vector<DWORD>* vectorOfEnabledGames(msg->GetVectorOfWords());
	if (vectorOfEnabledGames)
	{
		LogString(GAMES, "ProcessEnabledGamesFromInternals %u enabled games", vectorOfEnabledGames->size());
		m_parseGameList->ParseEnabledGames(vectorOfEnabledGames);
	}
	else
	{
		LogString(GAMES, "No enabled games");
	}	
}

void CPollerGameListInfo::ProcessAllGameListFromInternals(CInternalMessage *msg)
{
	CGames* games(msg->GetGamesData());
	WORD newGameCountsFetched(games->GetGameCountsFetched());
	WORD numberOfImplementedGames(msg->GetWordData());
	if (numberOfImplementedGames > 0)
	{
		LogString(GAMES, "ProcessAllGameListFromInternals-ClearData numberOfImplementedGames=%u", numberOfImplementedGames);
		ClearData();
		m_AllowGameListbeSenttoPoller = true;
		m_implementedGamesCount = numberOfImplementedGames;
	}

	//Update number of chunks information if total games changed
	if (m_oldImplementedGamesCount != m_implementedGamesCount)
	{
		m_oldImplementedGamesCount = m_implementedGamesCount;
		m_updateNumberOfChunks = true; //signal to update number of chunks if total games changed
	}

	ProcessConfigFeatureFlagsRecvd();

	//It might be that message is sent with the same game counts. So ignore that case
	if(games->HaveAllTheEnabledGamesBeenFetched())
	{
		LogString(GAMES, "AllTheEnabledGamesBeenFetched");
		LogString(GAMES, "%s: %d: m_gameCountsFetched = %u newGameCountsFetched: %u m_gameListChunkSize = %u", __FUNCTION__, __LINE__,  m_gameCountsFetched, newGameCountsFetched, m_gameListChunkSize);
		
		if ( ((m_gameListChunkSize > 0) && (newGameCountsFetched % m_gameListChunkSize  == 0)) || (newGameCountsFetched == m_implementedGamesCount))
		{
			m_indexSubgameToSend += (newGameCountsFetched - m_gameCountsFetched);
			LogString(GAMES, "%s: %d: implementedGamesCount = %d m_indexSubgameToSend: %u", __FUNCTION__, __LINE__,  m_implementedGamesCount, m_indexSubgameToSend);

			m_parseGameList->ParseGamesRange(msg->GetGamesData(), m_gameCountsFetched, m_indexSubgameToSend);

			m_gameCountsFetched = newGameCountsFetched;

			CheckandSendGameListtoPoller();
		}
	}
}

CParseGameList* CPollerGameListInfo::GetParseList()
{
	return m_parseGameList;
}

void CPollerGameListInfo::CheckandSendGameListtoPoller()
{
	SendTotalGameChunksToInternals();
	
	//Games will be sent to PollerProtocol in multiple of chunks for sake of simplicity
	if (m_configFeatureFlagsRecvd && (m_subgamesSent != m_indexSubgameToSend) && ((m_indexSubgameToSend % m_gameListChunkSize  == 0) || (m_indexSubgameToSend == m_implementedGamesCount)))
		m_isGameChunkReady = true;

	LogString(GAMES, "%s: m_isGameChunkReady: %u, implementedGamesCount = %d, m_subgamesSent = %d, m_indexSubgameToSend = %d, (m_DenomGamesReceived = %d) && (m_GlobalMGMD = %d) && (m_PollerConnected = %d) && (m_AllowGameListbeSenttoPoller = %d)", 
		__FUNCTION__, m_isGameChunkReady, m_implementedGamesCount, m_subgamesSent, m_indexSubgameToSend, (m_DenomGamesReceived) , (m_GlobalMGMD) , (m_PollerConnected) , (m_AllowGameListbeSenttoPoller));

	if ((m_isGameChunkReady) && (m_DenomGamesReceived) && (m_GlobalMGMD) && (m_PollerConnected) && (m_AllowGameListbeSenttoPoller))
	{		
		SetCurrentTime();
		m_isGameChunkReady = false;

		if (m_pollerprotocol)
			m_pollerprotocol->SendGameListToPoller();
	}
}

void CPollerGameListInfo::ProcessGlobalMGMDFlag(CInternalMessage* msg)
{
	m_GlobalMGMD = (bool)msg->GetWordData();
	LogString(GAMES, "%s: Received GlobalMGMD as %d in ProcessGlobalMGMDFlag", __FUNCTION__, m_GlobalMGMD);
	CheckandSendGameListtoPoller();
}

void CPollerGameListInfo::ProcessandPollerOnlineFlag(CInternalMessage* msg)
{
	m_PollerConnected = (bool)msg->GetWordData();
	LogString(GAMES, "Received PollerOnlineFlag as %d in ProcessandPollerOnlineFlag", m_PollerConnected);
	ProcessConfigFeatureFlagsRecvd();
	CheckandSendGameListtoPoller();
}

void CPollerGameListInfo::ProcessConfigFeatureFlagsRecvd()
{
	// retrieve feature flag if config is received for first time since nCompass came up.
	if (!m_configFeatureFlagsRecvd && CPollerMGMDData::Instance().IsConfigFeatureFlagsRecvd())
	{
		m_configFeatureFlagsRecvd = true; // signifies config feature flag received from poller.
		m_isChunkedGamesListFlagEnabled = CPollerMGMDData::Instance().IsChunkedGamesListFlagEnabled(); //fetch feature flag
		LogString(GAMES, _T("%s: Received ChunkedGamesListFlag as %u"), __TFUNCTION__, m_isChunkedGamesListFlagEnabled);

		// if ChunkedGamesListFlag is not enabled, then send all games at once. 
		// Thus, sentting chunk size to be equal to total games.
		if (!m_isChunkedGamesListFlagEnabled && m_implementedGamesCount > 0)
		{
			m_gameListChunkSize = m_implementedGamesCount;
			CPollerMGMDData::Instance().SetGameListChunkSizeOption(m_gameListChunkSize);
		}
		LogString(GAMES, _T("%s: %d gameListChunkSize - %u"), __TFUNCTION__, __LINE__, m_gameListChunkSize);

		m_updateNumberOfChunks = true;  //depending on feature flag, update number of chunks through SendTotalGameChunksToInternals()
	}
}

void CPollerGameListInfo::ClearData()
{
	LogString(GAMES, "%s:%d", __FUNCTION__, __LINE__);
	m_parseGameList->ClearGameList();
	m_DenomGamesReceived = 0;
	m_implementedGamesCount = 0;
	m_AllowGameListbeSenttoPoller = false;
	ResetGameChunksData();
}

void CPollerGameListInfo::ProcessGameOptionChange()
{
	LogString(GAMES, "%s:%d: GPE_OPTIONS_CHANGED received from Internals", __FUNCTION__, __LINE__);
	m_AllowGameListbeSenttoPoller = true;
}

WORD CPollerGameListInfo::GetTotalSubgames()
{
	return m_implementedGamesCount;
}

WORD CPollerGameListInfo::GetSubgamesSent()
{
	return m_subgamesSent;
}

void CPollerGameListInfo::UpdateSubgamesSent(WORD currentSubgamesSent)
{
	m_subgamesSent += currentSubgamesSent;
}

WORD CPollerGameListInfo::GetLastSubgameIndexInCurrentChunk()
{
	return m_indexSubgameToSend;
}

WORD CPollerGameListInfo::GetGamesInCurrentMessage()
{
	return (m_indexSubgameToSend - m_subgamesSent); 
}

void CPollerGameListInfo::SendTotalGameChunksToInternals()
{
	//Don't flood internals if total games remain unchanged or all games were already sent
	if ((0 != m_AllowGameListbeSenttoPoller) && m_updateNumberOfChunks && m_systemProtocol)
	{
		m_updateNumberOfChunks = false;
		
		WORD chunkIndex = 0;

		// If no game was sent till now or it's resending game list after some change, then reset the chunk-index
		if (0 == m_subgamesSent)
		{
			chunkIndex = 0; // This is same as initializer value. But just sake of readability.
		}
		else  //else dont try to change chunkindex which was communicate by PollerProtocol::PrepareAndSendGameListChunksMessage
		{
			//Max WORD, so as to ignore by internals. Needed only to update total number of chunks in case there is change.
			chunkIndex = UINT16_MAX; 
		}

		//Calculate number of chunks
		DWORD totalSubgames = m_implementedGamesCount;
		m_numberOfChunks	= ceil((float)totalSubgames / m_gameListChunkSize);

		//Send to internals to update on diagnostic web page		
		CInternalMessage *msg(new CInternalMessage(INT_MSG_GAME_LIST_CHUNKS_SENT, chunkIndex));
		msg->SetData((DWORD)m_numberOfChunks);

		LogString(GAMES, "%s: %d chunkIndex - %u NumberOfChunks - %u", __FUNCTION__, __LINE__, chunkIndex, m_numberOfChunks);
		m_systemProtocol->SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);
	}
}

WORD CPollerGameListInfo::GetNumberOfChunks()
{
	return m_numberOfChunks;
}

WORD CPollerGameListInfo::GetNextGameListChunkIndex()
{
	return m_nextGameListChunkIndex;
}

void CPollerGameListInfo::SetNextGameListChunkIndex(WORD nextGameListChunkIndex)
{
	m_nextGameListChunkIndex = nextGameListChunkIndex;
}

WORD CPollerGameListInfo::GetIsGameChunkReady()
{
		return m_isGameChunkReady;
}

time_t CPollerGameListInfo::GetCurrentTime()
{
	return m_currentTime;
}

void CPollerGameListInfo::SetCurrentTime()
{
	if(!m_isCurrentTimeSet)
	{
		m_currentTime = CUtilities::GetCurrentTime();
		m_isCurrentTimeSet = true;
	}
}

bool CPollerGameListInfo::GetIsCurrentTimeSet()
{
	return m_isCurrentTimeSet;
}
		
void CPollerGameListInfo::ResetGameChunksData()
{
	m_indexSubgameToSend 		= 0;
	m_numberOfChunks 			= 0;
	m_nextGameListChunkIndex 	= 1;
	m_subgamesSent				= 0;
	m_gameCountsFetched			= 0;
	m_implementedGamesCount		= 0;
	m_oldImplementedGamesCount	= 0;
	m_isGameChunkReady 			= false;
	m_isCurrentTimeSet			= false;

	//Resetting with current/updated value of chunk size instead of any zeros for next iteration.
	m_gameListChunkSize 			= CPollerMGMDData::Instance().GetGameListChunkSizeOption(); 
	m_isChunkedGamesListFlagEnabled = CPollerMGMDData::Instance().IsChunkedGamesListFlagEnabled();
	m_updateNumberOfChunks 			= false;
	
	LogString(GAMES, _T("%s: %d isChunkedGamesListFlagEnabled - %u, gameListChunkSize - %u"), __TFUNCTION__, __LINE__, m_isChunkedGamesListFlagEnabled, m_gameListChunkSize);
}

