#pragma once
#include "GlobalDefs.h"
#include "Config/CardConfig.h"

typedef enum 
{
	CARD_UNKNOWN = 0,
	CARD_PLAYER,
	CARD_MECHANIC,
	CARD_GLOBAL,
	CARD_FLOOR,
	CARD_CURRENCY,		// A.K.A. Soft Drop Card
	CARD_METER,
	CARD_TEMP
} CardType;

static const DWORD NVRAM_CARD_SIZE = 128;

static const DWORD MAX_CARD_NUMBER_LENGTH = 20;

namespace SentinelNativeUnitTests { class CardTest; }

class CCard 
{
public:
	static const int CARD_ID_TYPE_POS = 0;

	// Declare constructor(s) with one parameter as "explicit" to prevent unwanted implicit parameter conversions.
	CCard(time_t const cardInTime, time_t const utcCardInTime );
	explicit CCard( const CCard &cardRHS );
	CCard( const byte *nvramBuffer );
public:
	~CCard(void);

	CCard &operator=( const CCard &cardRHS ); 

	int GetNvramBuffer( byte *nvramBuffer, int nvramBufferLength ) const;

	/// <summary>
	/// Sets the card identifier.
	/// </summary>
	/// <param name="cardIDStr">IN - The card identifier string.</param>
	/// <param name="config">IN - The card configuration.</param>
	/// <remarks>
	/// m_validRead is set to true.
	/// If "config" != NULL then "config" is used to set m_siteNumber and m_isUniversalCard
	/// else m_siteNumber is set to 1, and m_isUniversalCard is unchanged.
	/// </remarks>
	void SetCardID( std::string cardIDStr, const CCardConfig *config = NULL );

	void SetCardInfo( CardType cardType, const byte *cardID, int cardIDLen );
	void SetCardInfoFromRawData(const byte *rawData, int rawDataLen, const CCardConfig &config);


	// CCard::SetAsInvalidRead
	// Set m_validRead=false and clear instance to initial values.
	void SetAsInvalidRead(void);
	
	/// <summary>
	/// Set the card as a valid read and as a player card type.
	/// </summary>
	void SetAsValidPlayer(void);

	bool IsValidRead(void)const;

	bool IsGlobalGlobal() const;

	byte GetCardTypeNumber(void) const;

	/// <summary>
	/// Gets whether this instance is a valid employee card.
	/// </summary>
	/// <returns>true is the card is a valid employee card, false otherwise.</returns>
	bool IsEmployee() const;

	CardType GetCardType(void) const;

	void SetCardType(CardType type);

	/// <summary>
	/// Gets the card identifier.
	/// </summary>
	/// <param name="cardIDLen">OUT - The length of the card identifier.</param>
	/// <returns>A pointer to a const card indentifier.</returns>
	const byte *GetCardID(int &cardIDLen) const;

	void SetCardInTime(time_t const cardInTime, time_t const utcCardInTime);

	void SetCardOutTime(time_t const cardOutTime);

	time_t GetCardInTime(void)const;

	time_t GetUTCCardInTime(void)const;

	time_t GetCardOutTime(void)const;

	bool GetIsUniversalCard()const;

	CardType DetermineFloorCardType( void );

	/// <summary>
	/// Compare the provided card number with this card.
	/// Returns true if the first 10 hexadecimal digits of the provided card number are the same as the card number for this card, false otherwise.
	/// </summary>
	/// <param name="cardToCompare">IN - The card number to compare to; typically 1 hexadecimal digit per character.</param>
	/// <returns>true if the first 10 hexadecimal digits of the provided card number are the same as the card number for this card, false otherwise.</returns>
	/// <remarks>
	/// "cardToCompare" MUST be (at least) 2*"bufferLength" hexadecimal characters.
	/// Otherwise, the value of "buffer" not defined.
	/// </remarks>
	bool Compare ( LPCTSTR cardToCompare ) const;

	/// <summary>
	/// Compare the provided card with this card.
	/// Returns true if the card numbers are the same, false otherwise.
	/// </summary>
	/// <param name="cardToCompare">IN - The card to compare to.</param>
	/// <returns></returns>
	bool Compare( const CCard *cardToCompare ) const;

	/// <summary>
	/// Compare the provided card number with this card.
	/// Returns true if the card numbers are the same, false otherwise.
	/// </summary>
	/// <param name="cardID">IN - the card number to compare to; 2 hexadecimal digits per byte.</param>
	/// <param name="cardIDLen">IN - The length of cardID in bytes.</param>
	/// <returns>true if the card numbers are the same, false otherwise.</returns>
	bool Compare( const byte *cardID, int cardIDLen ) const;

	/// <summary>
	/// Checks if the specified card commID is valid for the card.
	/// </summary>
	/// <param name="commId">IN - The specified card commID.</param>
	/// <returns>true if the specified card commID is valid for the card, false otherwise.</returns>
	/// <remarks>
	/// Tracker #21732.
	/// </remarks>
	bool CheckCommID(byte commId) const;

	/// <summary>
	/// Checks if last card read has to be cleared at startup.
	/// If the last card read is an employee card, it will be cleared.
	/// </summary>
	/// <param name="config">IN/OUT - The Configuration object.</param>
	void ClearLastCardReadIfNecessary(CConfig& config);

	/// <summary>
	/// Gets a std::string card number for a card pointer.
	/// </summary>
	/// <param name="card">IN - The card instance. MAY be NULL.</param>
	/// <returns>A std::string card number for the card pointer.</returns>
	/// <remarks>
	/// If card is NULL then an empty string is returned.
	/// </remarks>
	static std::string MakeCStringCardNumber(const CCard *card);

private:
// #pragma region private constants
	static const DWORD DEFAULT_SITE_NUMBER = 1; // Default value when not clear.
// #pragma endregion private constants

	CardType m_cardType;
	byte m_cardTypeNumber;
	byte m_siteNumber;
	int m_cardIDLen;
	byte *m_cardID;
	time_t m_cardInTime;
	time_t m_utcCardInTime;
	time_t m_cardOutTime;
	bool m_validRead;
	int m_rawDataLength; // Unused.
	bool m_isUniversalCard;
	WORD m_universalPropCode;

	CCard(); // Disable default constructor.
	void Init();
	void Clear();
	bool SetCardInfoFromRawNclData(const byte *rawData, int rawDataLen, byte siteNumber);
	bool SetCardInfoFromRawRcclData(const byte *rawData, int rawDataLen, byte rcclCardType, byte siteNumber, bool bEmployeeOnly);
	void SetCardInfoFromRawUniverseData(const byte *rawData, int rawDataLen, const std::vector<WORD>&propCodes, const std::vector<BYTE>&universePrefixes);
	void SetCardInfoFromRawSdsData(const byte* rawData, int rawDataLen);
	void SetCardInfoFromRawAtiData(const byte *rawData, int rawDataLen);
	static const byte *GetNextField(const byte *rawDataPtr, int &rawDataLen, int fieldLen, DWORD &value);
	static const byte *GetNextField( const byte *rawDataPtr, int &rawDataLen, int fieldLen, byte &value );
	static const byte* GetNextField(const byte* rawDataPtr, int& rawDataLen, int fieldLen, uint16_t& value);
	static DWORD CharToDWord( const char *charData, int charLen );

	friend class SentinelNativeUnitTests::CardTest;
};
