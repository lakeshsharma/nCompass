#pragma once
#include "GlobalDefs.h"
#include "metersdef.h"
#include "TiltsDef.h"
#include "PollerGamesListMessage.h"
#include "IPollerProtocolInterface.h"
#include "PollerProtocol.h"
#include "../InternalMessage.h"
#include <random>

// Declare friend class(es) for unit test access.
class PollerGameListInfoUnitTests;
class CPollerProtocol;
class CPollerGameListInfo
{
public:
	CPollerGameListInfo(CPollerProtocol* pollerprotocol);
	virtual ~CPollerGameListInfo(void)
    {
        delete m_parseGameList;
    }
	/// <summary>
	/// Process all games from internals.
	/// </summary>
	/// <param name="msg">message received from internals</param> 
	/// <returns>None.</returns>
	void ProcessAllGameListFromInternals(CInternalMessage *msg);

	/// <summary>
	/// Process denom games from internals.
	/// </summary>
	/// <param name="msg">message received from internals</param> 
	/// <returns>None.</returns>
	void ProcessDenomGamesFromInternalsAndSendMessageToPoller(CInternalMessage *msg);

	void ProcessEnabledGamesFromInternals(CInternalMessage *msg);

	/// <summary>
	/// Get Parse List.
	/// </summary>
	CParseGameList* GetParseList();

	/// <summary>
	/// Process Global MGMD flag from internals.
	/// </summary>
	/// <param name="msg">message received from internals</param> 
	/// <returns>None.</returns>
	void ProcessGlobalMGMDFlag(CInternalMessage* msg);

	/// <summary>
	/// Process Poller Online from internals.
	/// </summary>
	/// <param name="msg">message received from internals</param> 
	/// <returns>None.</returns>
	void ProcessandPollerOnlineFlag(CInternalMessage* msg);

	/// <summary>
	/// Clears member data.
	/// </summary>
	/// <returns>None.</returns>
	void ClearData();

	/// <summary>
	/// Sets member variable m_GameOptionChanged.
	/// </summary>
	/// <returns>None.</returns>
	void ProcessGameOptionChange();

	/// <summary>
	/// Get Total Subgames available.
	/// </summary>
	/// <returns>Total Subgames available.</returns>
	WORD GetTotalSubgames();
	
	/// <summary>
	/// Get Subgames Sent till now.
	/// </summary>
	/// <returns>Subgames Sent till now.</returns>
	WORD GetSubgamesSent();
	
	/// <summary>
	/// Update Subgames Sent till now.
	/// </summary>
	/// <param name="currentSubgamesSent">Update Subgames Sent till now</param> 
	/// <returns>None.</returns>
	void UpdateSubgamesSent(WORD currentSubgamesSent);
	
	/// <summary>
	/// Get Last Subgame Index In Current Chunk. Useful to find out how many games to send.
	/// </summary>
	/// <returns>Last Subgame Index In Current Chunk.</returns>
	WORD GetLastSubgameIndexInCurrentChunk();

	/// <summary>
	/// Get how many subgames are in current message. If more than chunk size then divide them accordingly.
	/// </summary>
	/// <returns>Number of subgames in current message.</returns>
	WORD GetGamesInCurrentMessage();
	
	/// <summary>
	/// Gets how many number of chunks.
	/// </summary>
	/// <returns>Number of chunks.</returns>
	WORD GetNumberOfChunks();
	
	/// <summary>
	/// Get Next Chunk Index from GameList.
	/// </summary>
	/// <returns>Next Chunk Index to send from GameList.</returns>
	WORD GetNextGameListChunkIndex();

	/// <summary>
	/// Set Next Chunk Index from GameList.
	/// </summary>
	/// <param name="nextGameListChunkIndex">Next Chunk Index in 
	/// GameList</param> 
	/// <returns>None.</returns>
	void SetNextGameListChunkIndex(WORD nextGameListChunkIndex);
	
	/// <summary>
	/// Is Game Chunk Ready.
	/// </summary>
	/// <returns>Is Game Chunk Ready.</returns>
	WORD GetIsGameChunkReady();
	
	/// <summary>
	/// Get Current Time to be added in each chunk of subgame list message.
	/// </summary>
	/// <returns>Current Time to be added in each chunk of subgame list message.</returns>
	time_t GetCurrentTime();
	
	/// <summary>
	/// Set Current Time to be added in each chunk of subgame list message..
	/// </summary>
	/// <returns>None.</returns>
	void SetCurrentTime();

	/// <summary>
	/// To know whether Current time is set for chunks
	/// </summary>
	/// <returns>whether Current time is set for chunks.</returns>
	bool GetIsCurrentTimeSet();
	
	/// <summary>
	/// Reset Game Chunks Data.
	/// </summary>
	/// <returns>None.</returns>
	void ResetGameChunksData();
	
	/// <summary>
	/// Send Total Number of Game Chunks To Internals.
	/// </summary>
	/// <returns>None.</returns>
	void SendTotalGameChunksToInternals();

	/// <summary>
	/// Update game list chunks information when respective feature flag is received.
	/// </summary>
	/// <returns>None.</returns>
	void ProcessConfigFeatureFlagsRecvd();

private:
	CPollerProtocolInterface* m_pollerprotocol;
	CParseGameList *m_parseGameList;
	WORD m_implementedGamesCount;	
	WORD m_oldImplementedGamesCount;
	bool m_DenomGamesReceived;
	bool m_GlobalMGMD;
	bool m_PollerConnected;
	bool m_AllowGameListbeSenttoPoller;

	WORD m_gameCountsFetched;
	WORD m_gameListChunkSize;
	WORD m_indexSubgameToSend;
	WORD m_numberOfChunks;
	WORD m_nextGameListChunkIndex;
	WORD m_subgamesSent;
	bool m_isChunkedGamesListFlagEnabled;
	bool m_configFeatureFlagsRecvd;
	bool m_updateNumberOfChunks;
	
	bool m_isGameChunkReady;
	bool m_isCurrentTimeSet;
	time_t m_currentTime;

	CSystemProtocol* m_systemProtocol;

	//Variables to retrieve random number
	std::random_device random_number_engine;
	std::mt19937 rand_generator;
	uniform_int_distribution<int> rand_number_distributor;
	
	void CheckandSendGameListtoPoller();

	FRIEND_TEST(PollerGameListInfoUnitTests, ProcessConfigFeatureFlagsRecvdTest);
	FRIEND_TEST(PollerGameListInfoUnitTests, ProcessAllGameListFromInternalsTest);	
};
