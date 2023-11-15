#pragma once
#include "PollerMessage.h"
#include "Config/Config.h"

typedef struct _extendedBucketsCountdownData
{
	DWORD		  id;
	std::string   displayName;
	std::string   bucketCurrency;
	DWORD		  countdown;
	DWORD		  multiplier;
	DWORD		  divider;
}ExtendedBucketsCountdownData;

typedef struct _playerResp
{
	byte card_id[CARD_ID_LEN];
	WORD multiplier;
	WORD divider;
	bool birthdayIndicator;
	bool bannedFromPlayIndicator;
	bool anniversaryIndicator;	
	DWORD totalPoints;
	WORD countdown;
	DWORD comps; // in cents
	byte nameLength; // Length does not include a trailing NUL character.
	byte *name; // Has a trailing NUL character, if not NULL.
	byte respLength; // Length does not include a trailing NUL character.
	byte *response; // Has a trailing NUL character, if not NULL.
	WORD lossLimitBalance;
	BYTE lossLimitStatus;
	DWORD dailyPoints; //Tracker 026925
	byte maxCardLimitIndicator; //Tracker 026984
	DWORD pointsTillNextTier; //Tracker 026928
	DWORD tierMultiplier; //SPS 7/14/2010 TR #26584
	DWORD m_rankId;
	byte m_rankNameLength; // Length does not include a trailing NUL character.
	byte *m_rankName; // Has a trailing NUL character, if not NULL.
	DWORD m_playerId;
	DWORD m_tierPoints;
	DWORD countdownHighPrecision;
	AccountingType accountingType;
	bool ebIndicator;
	DWORD ebPointsId;
	DWORD ebCompsId;
	DWORD ebPromosId;
	DWORD ebTierPointsId;
	std::vector<ExtendedBucketsCountdownData*> ebCountdowns;
	std::string ebJSONData;
	bool enrolledInWallet_2_0;
} PlayerResponse;




class CPollerPlayerResponseMessage :
	public CPollerMessage
{
public:
	static const DWORD INVALID_RANK_ID = (DWORD)-1;
	static const DWORD DEFAULT_RANK_ID = INVALID_RANK_ID;
	static const DWORD DEFAULT_PLAYER_ID = (DWORD)-1;
	static const DWORD DEFAULT_TIER_POINTS = (DWORD)-1;
	static const DWORD DEFAULT_COUNTDOWN_HIGH_PRECISION = (DWORD)-1;

	/// <summary>
	/// Standard constructor. Initializes a new instance of the <see cref="CPollerPlayerResponseMessage"/> class.
	/// </summary>
	/// <param name="response">IN - A pointer to message data.</param>
	/// <param name="packetVersion">IN - The packet version for the message.</param>
	/// <param name="dataLength">IN - The length of the message data.</param>
	CPollerPlayerResponseMessage(const byte *response,WORD packetVersion, WORD dataLength);

	/// <summary>
	/// Gets a shallow copy of the player response.  I.e., the (original) pointers are managed by the CPollerPlayerResponseMessage instance.
	/// </summary>
	/// <returns>A shallow copy of the player response.</returns>
	PlayerResponse GetPlayerResponse();

	/// <summary>
	/// Desctructor. Finalizes an instance of the <see cref="CPollerPlayerResponseMessage"/> class.
	/// </summary>
	~CPollerPlayerResponseMessage(void);

private:

	PlayerResponse m_playerResponse;
};
