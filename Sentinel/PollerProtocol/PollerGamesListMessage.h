#pragma once
#include "PollerMessage.h"
#include "../Games.h"
#include "../Game.h"

#include "ParseGameList.h"

const int GAMELIST_PAY_TABLE_ID_LEN = 20;

class CPollerGamesListMessage :public CPollerMessage
{
 public:
	 /// <summary>
	 /// Object construction.
	 /// </summary>
	 /// <param name="games">Games List to parse and send</param> 
	 /// <param name="currentTime">Current Time send</param> 
	 /// <param name="packetVersion">Packet Version to send</param> 
	 /// <param name="chunkIndex">Chunk Index</param> 
     /// <param name="gameListChunkoffset">Game List Chunk offset 
     /// from where to fetch the games from the games list</param> 
     /// <param name="numberOfChunks">Number Of Chunks message 
     /// field</param> 
	 /// <param name="gameListChunkSize">Game List Chunk Size</param>
	 /// <returns>None.</returns>
	CPollerGamesListMessage(CParseGameList *games, time_t& currentTime, WORD packetVersion, WORD totalSubgames, WORD chunkIndex, WORD gameListChunkoffset, WORD numberOfChunks, WORD gameListChunkSize);

	/// <summary>
	/// Object Destruction.
	/// </summary>
	~CPollerGamesListMessage();

	 /// <summary>
	 /// Get complete message buffer.
	 /// </summary>
     /// <param name="buffer Size">OUT: Total buffer size used to 
     /// fill the message</param> 
	 /// <returns>None.</returns>
	byte * GetReadWriteBuffer( DWORD &bufferSize );

	 /// <summary>
	 /// Create game list message and store in a buffer.
	 /// </summary>
	 /// <returns>None.</returns>
    void SetReadWriteBuffer();

 private:
	std::vector<CGame*> m_gamesToSend;
	std::set<WORD> m_denoms;  // set of denoms
	std::map<DWORD, std::set<WORD> > m_gameDenoms;	//<GameNumber, list of supported denoms>
	WORD m_pollerPacketVersion;
    byte m_denominationCount;
    WORD m_gameCount;
	WORD  m_subGameTotalDenomCount;	
	WORD  m_gameListChunkIndex;
	WORD  m_numberOfGameListChunks;
	WORD  m_gameListChunkSize;
	time_t m_currentTime;
	
	WORD GetSizeofSubGameBlock();
};
