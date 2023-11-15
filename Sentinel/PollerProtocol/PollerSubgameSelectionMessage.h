#pragma once
#include "PollerMessage.h"

class CPollerSubgameSelectionMessage :
	public CPollerMessage
{
public:
	/// <summary>
	/// Constructs a poller subgame selection message
	/// </summary>
	/// <returns></returns>
	CPollerSubgameSelectionMessage(DWORD gameSessionSequence,
									bool cardedPlayerIndicator,
									const byte *cardID,
									int cardNumberLen,
									DWORD playerId,
									int playerSessionId,
									WORD gameNumber,
									WORD denom,
								   bool mgmdAvailable);
public:
	~CPollerSubgameSelectionMessage(void);

	/// <summary>
	/// Initializes read/write buffer
	/// </summary>
	/// <param name="bufferSize"></param>
	/// <returns>Poller message memory buffer</returns>
	byte * GetReadWriteBuffer( long &bufferSize );

private:
	void SetReadWriteBuffer();

	DWORD m_gameSessionSequence;
	bool m_cardedPlayerIndicator;
	byte *m_cardID;
	int m_cardNumberLen;
	DWORD m_playerId;
	int m_playerSessionId;
	WORD m_gameNumber;
	WORD m_denom;
	bool m_mgmdAvailable;
};

