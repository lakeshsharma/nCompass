#pragma once
#include "SASPollMessage.h"

class CLongPollB5Message : public CSASPollMessage
{
public:
	CLongPollB5Message(byte gameAddress);
	~CLongPollB5Message();

	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();
	void SetGameNumber(int gameNumber);
	__int64 GetMaxBet() const;

	/// <summary>
	/// Gets the progressive group.
	/// </summary>
	/// <returns>The progressive group.</returns>
	byte GetProgressiveGroup() const;

	/// <summary>
	/// Gets the Game Name for given game number.
	/// </summary>
	/// <param name="gameName">OUT: Game Name for given game number</param>
	/// <returns>The length of the Game Name.</returns>
	WORD GetGameName (char** gameName);
	
	/// <summary>
	/// Gets the Paytable Name for given game number.
	/// </summary>
	/// <param name="paytableName">OUT: Paytable Name for given game number</param>
	/// <returns>The length of the Paytable Name.</returns>
	WORD GetPaytableName (char** paytableName);

	/// <summary>
	/// Gets the progressive level IDs for this game as 32 bit bitmask.
	/// </summary>
	/// <returns>Progressive level IDs bitmask</returns>
	DWORD GetLevelIds() const;

private:
   // 6.02 addendum adds player denoms resulting in up to 33 extra bytes
   static const byte XB5_MAX_RESPONSE_SIZE = 92;
	static const byte XB5_LENGTHBYTE = 2;
	static const byte XB5_LENGTHSIZE = 1;
	static const byte XB5_MAXBETLOCATION = 5;
	static const byte XB5_MAXBETSIZE = 2; // BYTES
	static const byte XB5_PROGRESSIVEGROUPLOCATION = 7;
	static const byte XB5_PROGRESSIVEGROUPSIZE = 1; // BYTES
	static const byte XB5_POLLSIZE = 6;
	static const byte XB5_GAMENUMBERLOCATION = 2;
	static const byte XB5_GAMENUMBERSIZE = 2;
	static const byte XB5_RESPONSE_EXTRADATASIZE = 5;
	static const byte XB5_RESPONSE_GAMENUMBERLOCATION = 3;
	static const byte XB5_RESPONSE_GAMENAME_LENGTH = 12;
	static const byte XB5_RESPONSE_GAMENAME_LENGTH_SIZE = 1;
	static const byte XB5_PROGRESSIVELEVELSIDLOCATION = 8;
	static const byte XB5_NON_SAS_PROGRESSIVE_GROUP = 0; // Progressive Group (ID) 0 is reserved for non-sas progressives.
	static const byte XB5_DEFAULT_PROGRESSIVE_GROUP = XB5_NON_SAS_PROGRESSIVE_GROUP;

	static const UINT GAME_ADDRESS_LOCATION = 0;
	static const UINT POLL_VALUE_LOCATION = 1;
	static const UINT POLL_VALUE = 0xB5;

	__int64 m_maxBet;
	byte m_progressiveGroup;
	int m_gameNumber;
	
	BYTE m_gameNameLength;
	char *m_gameName;
	BYTE m_paytableNameLength;
	char *m_paytableName;
	DWORD m_levelIds;

	//File the game info from LPB5 response received
	bool FillGameName(const BYTE **responseBuffer, const BYTE *responseEnd); 

	//File the Paytable info from LPB5 response received
	bool FillPaytableName(const BYTE **responseBuffer, const BYTE *responseEnd);
};
