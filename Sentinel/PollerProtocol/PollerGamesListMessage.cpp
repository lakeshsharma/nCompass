#include "PollerGamesListMessage.h"
#include "Utilities.h"
#include "Logging/LogString.h"

CPollerGamesListMessage::CPollerGamesListMessage(CParseGameList *games, time_t& currentTime, WORD packetVersion, WORD totalSubgames, WORD chunkIndex, WORD gameListChunkoffset, WORD numberOfChunks, WORD gameListChunkSize):
	m_pollerPacketVersion(packetVersion),
	m_gameListChunkIndex(chunkIndex),  
	m_numberOfGameListChunks(numberOfChunks),
	m_gameListChunkSize(gameListChunkSize)
{
	if (games)
	{
		games->GetGamesToSend(m_gamesToSend, gameListChunkoffset, m_gameListChunkSize);
	}	
	m_denoms			= games->GetDenoms();
	m_gameDenoms		= games->GetGameDenoms();
	m_gameCount 		= totalSubgames;
    m_currentTime 		= currentTime;
    m_denominationCount = m_denoms.size();
	
    m_bufferSize 		= SIZE_OF_MESSAGE_HEADER +								/* Message header */
	                     POLLER_MFG_ID_LEN * sizeof(byte)						/* Mfg Id */ 
	                    + sizeof(m_denominationCount)							/* m_denominationCount */
	                    + m_denominationCount*sizeof(WORD)						/* Total EGM denominations */
	                    + sizeof(WORD)                                          /* m_gameCount */
	                    + GetSizeofSubGameBlock()								/* Game block */
						+ sizeof(WORD)											/* m_gameCount */
						+ sizeof(WORD)											/* m_numberOfGameListChunks */
						+ sizeof(WORD)											/* m_gameListChunkIndex */
	                    + sizeof(SYSTEMTIME);									/* Start/Current Time */
						
    m_lpBuffer 			= new byte[m_bufferSize];
}

CPollerGamesListMessage::~CPollerGamesListMessage()
{
	for_each(m_gamesToSend.begin(), m_gamesToSend.end(), [](CGame *game) { delete game; });
	m_gamesToSend.clear();
}

byte * CPollerGamesListMessage::GetReadWriteBuffer(DWORD &bufferSize)
{
    bufferSize = m_bufferSize;
	SetReadWriteBuffer();
	return m_lpBuffer;
}

void CPollerGamesListMessage::SetReadWriteBuffer()
{
	MessageHeader.MessageID = POLLER_MSG_ALL_GAME_LIST;
	MessageHeader.DataLength = m_bufferSize - SIZE_OF_MESSAGE_HEADER;

	int offset = 0; 
   
	//Header (piece by piece to avoid missaligned data error)
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(MessageHeader.PacketVersion), sizeof(MessageHeader.PacketVersion));
	offset += sizeof(MessageHeader.PacketVersion);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(MessageHeader.MessageID), sizeof(MessageHeader.MessageID));
	offset += sizeof(MessageHeader.MessageID);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(MessageHeader.DataLength), sizeof(MessageHeader.DataLength));
	offset += sizeof(MessageHeader.DataLength);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(MessageHeader.MacAddress), sizeof(MessageHeader.MacAddress));
	offset += sizeof(MessageHeader.MacAddress);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(MessageHeader.SequenceNumber), sizeof(MessageHeader.SequenceNumber));
	offset += sizeof(MessageHeader.SequenceNumber);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(MessageHeader.SlotMastID), sizeof(MessageHeader.SlotMastID));
	offset += sizeof(MessageHeader.SlotMastID);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(MessageHeader.SlotmastRevision), sizeof(MessageHeader.SlotmastRevision));
	offset += sizeof(MessageHeader.SlotmastRevision);

	//MfgID from end. In few cases, value is not retrieved in beginning.
	auto gameItr = m_gamesToSend.rbegin();
	CGame* game = *gameItr;
	char* mfgID = new char[MFG_ID_LEN];
	memset(mfgID, 0, MFG_ID_LEN);
	CUtilities::ConvertCharsToChars(game->GetMfgID(), mfgID, MFG_ID_LEN);

	memcpy(m_lpBuffer + offset, mfgID, POLLER_MFG_ID_LEN);
	offset+= POLLER_MFG_ID_LEN;

	delete[] mfgID;
	
	memcpy(m_lpBuffer + offset, &m_denominationCount, sizeof(m_denominationCount));
	offset += sizeof(m_denominationCount);

	for (auto denom : m_denoms)
	{
		memcpy(m_lpBuffer + offset, &denom, sizeof(denom));
		offset += sizeof(denom);
	}

	//From here message fragmentation will start
	//Number of Games in this message
	memcpy(m_lpBuffer + offset, &m_gameListChunkSize, sizeof(m_gameListChunkSize));
	offset += sizeof(m_gameListChunkSize);

	for (auto game : m_gamesToSend)
	{
		WORD gameNumber = game->GetGameNumber();
		std::map<DWORD, std::set<WORD>>::iterator itr = m_gameDenoms.find(gameNumber);
		if (itr != m_gameDenoms.end())
		{
			LogString(GAMES, "%s %d: Fetching Info for GameNumber: %u enabled=%d", __FUNCTION__, __LINE__, game->GetGameNumber(), game->IsEnabled());

			WORD theoreticalPayback = game->GetTheoreticalPayback();
			byte gameStatus = game->IsEnabled();
			byte subgameDenomCount = (byte)itr->second.size();

			memcpy(m_lpBuffer + offset, &gameNumber, sizeof(gameNumber));
			offset += sizeof(gameNumber);

			byte gameNameLength = game->GetGamenameLength();
			memcpy(m_lpBuffer + offset, &gameNameLength, sizeof(gameNameLength));
			offset += sizeof(gameNameLength);

			char* gameName = new char[gameNameLength + 1];
			CUtilities::ConvertCharsToChars(game->GetGameName(), gameName, gameNameLength + 1);
			memcpy(m_lpBuffer + offset, gameName, gameNameLength);
			offset += gameNameLength;
			delete gameName;

			byte payTableIDlength = game->GetPaytableIDLength();
			memcpy(m_lpBuffer + offset, &payTableIDlength, sizeof(payTableIDlength));
			offset += sizeof(payTableIDlength);

			char* payTableID = new char[payTableIDlength + 1];
			CUtilities::ConvertCharsToChars(game->GetPaytableID(), payTableID, payTableIDlength + 1);
			memcpy(m_lpBuffer + offset, payTableID, payTableIDlength);
			offset += payTableIDlength;
			delete payTableID;

			char* additionalId = new char[ADDITIONAL_ID_LEN];
			CUtilities::ConvertCharsToChars(game->GetAdditionalID(), additionalId, ADDITIONAL_ID_LEN);
			memcpy(m_lpBuffer + offset, additionalId, ADDITIONAL_ID_LEN - 1);
			offset += (ADDITIONAL_ID_LEN - 1);
			delete additionalId;

			memcpy(m_lpBuffer + offset, &theoreticalPayback, sizeof(theoreticalPayback));
			offset += sizeof(theoreticalPayback);

			memcpy(m_lpBuffer + offset, &gameStatus, sizeof(gameStatus));
			offset += sizeof(gameStatus);

			memcpy(m_lpBuffer + offset, &subgameDenomCount, sizeof(subgameDenomCount));
			offset += sizeof(subgameDenomCount);

			std::set<WORD> tempV;
			std::map<DWORD, set<WORD>>::iterator itr = m_gameDenoms.find(gameNumber);
			if (itr != m_gameDenoms.end())
			{
				tempV = itr->second;
			}

			for (auto gameDenom : tempV)
			{
				memcpy(m_lpBuffer + offset, &gameDenom, sizeof(gameDenom));
				offset += sizeof(gameDenom);
			}
		}
	}
	
	memcpy(m_lpBuffer + offset, &m_gameCount, sizeof(m_gameCount));
	offset += sizeof(m_gameCount);

	memcpy(m_lpBuffer + offset, &m_numberOfGameListChunks, sizeof(m_numberOfGameListChunks));
	offset += sizeof(m_numberOfGameListChunks);

	WORD gameListChunkNumber = m_gameListChunkIndex;
	memcpy(m_lpBuffer + offset, &gameListChunkNumber, sizeof(gameListChunkNumber));
	offset += sizeof(gameListChunkNumber);

	// Transaction time
	SYSTEMTIME transTime = CUtilities::GetUTCSystemTime(m_currentTime);
	offset = AppendUTCTimeStamp(offset, transTime);

	LogString(GAMES, "%s: %d: offset: %u\n", __FUNCTION__, __LINE__,	offset);
}

WORD CPollerGamesListMessage::GetSizeofSubGameBlock()
{
	WORD subgameBlockSize = 0;

	WORD currentSubameIndex = 0; // counter to increment till the game counts which are already sent, means subgamesCountToSent

	for (auto game : m_gamesToSend)
	{
		DWORD gameNumber = game->GetGameNumber();

		std::map<DWORD, std::set<WORD>>::iterator itr = m_gameDenoms.find(gameNumber);  // m_gameNumber[j] == denom.first

		if (itr != m_gameDenoms.end())
		{
			subgameBlockSize += (sizeof(WORD)										/*m_gameNumber*/
				+ sizeof(byte)										/*game name length*/
				+ (game->GetGamenameLength() * sizeof(byte))		/*game name*/
				+ sizeof(byte)										/*paytable length*/
				+ (game->GetPaytableIDLength() * sizeof(byte))		/* payTableId*/
				+ (ADDITIONAL_INFO_LENGTH * sizeof(byte))			/*additional info*/
				+ sizeof(WORD)										/*m_theoriticalPayback*/
				+ sizeof(byte)										/*m_gameStatus*/
				+ sizeof(byte));									/*m_subgameDenomCount*/

			std::set<WORD> tempV;
			std::map<DWORD, set<WORD>>::iterator itr = m_gameDenoms.find(gameNumber);
			if (itr != m_gameDenoms.end())
			{
				tempV = itr->second;
			}

			subgameBlockSize += (tempV.size() * sizeof(WORD));
		}
	}

	LogString(GAMES, "%s: %d: subgameBlockSize: %u", __FUNCTION__, __LINE__, subgameBlockSize);

	return subgameBlockSize;
}
