#pragma once
#include "SASQueueList.h"
#include "SASData.h"
#include "../binsem.h"
#include "../QueueItem.h"
#include "../metersdef.h"

// Declare friend class(es) for unit test access.
namespace SentinelNativeUnitTests { class SASPollMessage; }

const byte INVALID_LONG_POLL = 0;

const byte INVALID_SAS_METER_CODE = 0xff;
const int SASPOLL_RETRY_COUNT = 2;
const int SASPOLL_INCOMPLETE_DATA = 1;
const int SASPOLL_NACK = 2;
const int SASPOLL_SUCCESS = 0;
const int SASPOLL_DATA_TOO_LARGE = 3;
const int LPCOMMANDLOC = 1;
const int GAMEADDRLOC = 0;
const WORD MAX_DAYS_TO_EXPIRE_TICKETS = 9999; // Maximum usable value.
const WORD NO_EXPIRATION_7D = 9999;
const byte NO_EXPIRATION_7B = 0x00;
const byte MAX_EXPIRATION_7B = 0xFF;

typedef enum {
	smcTotalCreditsFromCoinAcceptor = 0x08,
	smcTotalCreditsPaidFromHopper = 0x09,
	smcTotalCreditsFromCoinsToDrop = 0x0a,
	smcTotalSasCashableTicketIn_IncludingNonrestricted = 0x0d,
	smcTotalSasRestrictedTicketOut = 0x10,
} SASMeterCode;

typedef struct _meterCodeMapping
{
	int oasisMeterNumber;
	byte sasMeterCode;
	byte sasMeterCodeSecondary;
} meterCodeMapping;

const int NUM_MAIN_GAME_METER_CODE_MAPPINGS = 57;
const meterCodeMapping mainGameMeterCodeMap[NUM_MAIN_GAME_METER_CODE_MAPPINGS] = 
{
	{mCD,		smcTotalCreditsFromCoinsToDrop, INVALID_SAS_METER_CODE},
	{mD1,		0x40, INVALID_SAS_METER_CODE},	//{Ones}
	{mD2,		0x41, INVALID_SAS_METER_CODE},	//{Twos}
	{mD5,		0x42, INVALID_SAS_METER_CODE},	//{Fives}
	{mD10,		0x43, INVALID_SAS_METER_CODE},	//{Tens}
	{mD20,		0x44, INVALID_SAS_METER_CODE},	//{Twenties}
	{mD50,		0x46, INVALID_SAS_METER_CODE},	//{Fifties}
	{mD100,		0x47, INVALID_SAS_METER_CODE},	//{Hundreds}
	{mCrd,		0x0c, INVALID_SAS_METER_CODE},
	{mTCi,		smcTotalCreditsFromCoinAcceptor, INVALID_SAS_METER_CODE},
	{mTCo,		smcTotalCreditsPaidFromHopper, INVALID_SAS_METER_CODE},
	{mBD,		0x0b, INVALID_SAS_METER_CODE},
	{mTHP,		0x23, INVALID_SAS_METER_CODE},
	{mActualCD, smcTotalCreditsFromCoinsToDrop, INVALID_SAS_METER_CODE},
	{gCI,		0x00, INVALID_SAS_METER_CODE},
	{gCO,		0x1C, 0x01},
	{gGS,		0x05, INVALID_SAS_METER_CODE},
	{gGW,		0x06, INVALID_SAS_METER_CODE},
	{gCC,		0x04, INVALID_SAS_METER_CODE},
	{gJP,		0x1F, 0x02},
	{mHCC,		0x03, INVALID_SAS_METER_CODE},
	{mD200,		0x48, INVALID_SAS_METER_CODE}, //{Two Hundreds}
	{mD500,		0x4A, INVALID_SAS_METER_CODE}, //{Five Hundreds}
	{mD1000,	0x4B, INVALID_SAS_METER_CODE}, //{One Thousands}
	{mTValueCI, smcTotalCreditsFromCoinAcceptor, INVALID_SAS_METER_CODE},
	{mTValueCO, smcTotalCreditsPaidFromHopper, INVALID_SAS_METER_CODE},
	{mGameWTPP, 0x7f, INVALID_SAS_METER_CODE},
	{mTotalDrop,0x24, INVALID_SAS_METER_CODE},
	{mTVal        ,0x2c, 0x0e},
	{mTO          ,0x12, INVALID_SAS_METER_CODE},
	{mCPromoOutVal,0x2d, 0x10},
	{mCPromoOutCnt,0x14, INVALID_SAS_METER_CODE},
	{mHPCshRcptVal,0x8c, INVALID_SAS_METER_CODE},
	{mHPCshRcptCnt,0x8d, INVALID_SAS_METER_CODE},
	{mHPWinRcptCnt,0x8f, INVALID_SAS_METER_CODE},
	{mHPWinRcptVal,0x8e, INVALID_SAS_METER_CODE},
	{mCoupV       ,0x28, 0x0d},
	{mVchrTktDrop ,0x29, INVALID_SAS_METER_CODE},
	{mCoup        ,0x11, INVALID_SAS_METER_CODE},
	{mCTktPV      ,0x2a, 0x0f},
	{mCTktPN      ,0x13, INVALID_SAS_METER_CODE},
	{mCTktCV      ,0x2b, 0x84},
	{mCTktCN      ,0x85, INVALID_SAS_METER_CODE},
	{mPbAR2G      ,0x2f, INVALID_SAS_METER_CODE},
	{mPbAN2G      ,0x30, INVALID_SAS_METER_CODE},
	{mPbAC2H      ,0x32, INVALID_SAS_METER_CODE},
	{mPbAC2G      ,0x2e, INVALID_SAS_METER_CODE},
	{mPbAR2H      ,0x33, INVALID_SAS_METER_CODE},
	{mPbAN2H      ,0x34, INVALID_SAS_METER_CODE},
	{mRstrPlayd      ,0x19, INVALID_SAS_METER_CODE},
	{mURstrPlayd      ,0x1a, INVALID_SAS_METER_CODE},
	{mPbAD2G      ,0x31, INVALID_SAS_METER_CODE},
	{mBonusMach      ,0x1e, INVALID_SAS_METER_CODE},
	{mBonusAtt      ,0x21, INVALID_SAS_METER_CODE},
	{mPrgPayAtt      ,0x20, INVALID_SAS_METER_CODE},
	{mPrgPayMac      ,0x1d, INVALID_SAS_METER_CODE},
	{mNcepCredits      ,0x1b, INVALID_SAS_METER_CODE},
};

const int NUM_SUB_GAME_METER_CODE_MAPPINGS = 10;
const meterCodeMapping subGameMeterCodeMap[NUM_SUB_GAME_METER_CODE_MAPPINGS] = 
{
	{SUBGAME_METER_GAMES_PLAYED,		0x05, INVALID_SAS_METER_CODE}, 
	{SUBGAME_METER_COIN_IN,				0x00, INVALID_SAS_METER_CODE},
	{SUBGAME_METER_COIN_OUT,			0x1c, INVALID_SAS_METER_CODE},
	{SUBGAME_METER_ATT_PAID_JACKPOT,	0x1f, INVALID_SAS_METER_CODE},
	{SUBGAME_METER_GAMES_WON,			0x06, INVALID_SAS_METER_CODE},
	{SUBGAME_METER_ATT_PAID_PROG,		0x20, INVALID_SAS_METER_CODE},
	{SUBGAME_METER_ATT_PAID_EXT_BONUS,	0x21, INVALID_SAS_METER_CODE},
	{SUBGAME_METER_MACH_PAID_PROG,		0x1d, INVALID_SAS_METER_CODE},
	{SUBGAME_METER_MACH_PAID_EXT_BONUS,	0x1e, INVALID_SAS_METER_CODE},
	{SUBGAME_METER_WTPP,				0x7f, INVALID_SAS_METER_CODE},
};

#define CRCSIZE 2

class CSASPollMessage : public CQueueItem
{
public:
	static const BYTE GAME_ADDRESS_NACK_OR_VALUE = 0x80; // General polls cannot be NACKed.

	CSASPollMessage(byte gameAddress);
public:
	virtual ~CSASPollMessage(void);

protected:
	// Long poll field offsets.
	static const size_t GAME_ADDRESS_OFFSET = 0;
	static const size_t GAME_ADDRESS_SIZE = sizeof(BYTE);
	static const size_t POLL_VALUE_OFFSET = GAME_ADDRESS_OFFSET + GAME_ADDRESS_SIZE;
	static const size_t POLL_VALUE_SIZE = sizeof(BYTE);
	static const size_t OPTIONAL_LENGTH_OFFSET = POLL_VALUE_OFFSET + POLL_VALUE_SIZE; // Offset of Type S long poll optional length field, when present.
	static const size_t OPTIONAL_LENGTH_SIZE = sizeof(BYTE); // Size for Type S long poll optional length field, when present.
	static const size_t OPTIONAL_DATA_OFFSET = OPTIONAL_LENGTH_OFFSET + OPTIONAL_LENGTH_SIZE; // Offset of Type S long poll optional data, when present.
	// For variable-length messages, m_sendBufferSize = NON_DATA_LENGTH + OPTIONAL_LENGTH.
	static const UINT NON_DATA_LENGTH = GAME_ADDRESS_SIZE + POLL_VALUE_SIZE + OPTIONAL_LENGTH_SIZE + CRCSIZE;

	/// <summary>
	/// Sets the size, the optional length field, and the CRC.
	/// </summary>
	/// <param name="size">IN - The new size.</param>
	/// <remarks>
	/// This method cannot be used to increase the send buffer size.
	/// Nothing is done if the new size is greater than the current size or if the new size is less than NON_DATA_LENGTH.
	/// </remarks>
	void SetSizeLengthAndCRC(UINT size);

	/// <summary>
	/// Generates the checksum for the send buffer and stores it at the end of the send buffer.
	/// </summary>
	/// <remarks>
	/// Nothing is done if the send buffer size is less than CRCSIZE + 1.
	/// </remarks>
	void GenerateSendChecksum();

	// Block the execution until message is processed by SAS
	void WaitForSASProcessingDone();

	unsigned short CRC(unsigned char *s, int len, unsigned short crcval = 0);
	int GetMeterNumberForMeterCode( byte meterCode, int gameNumber );

	//CSASData	m_sasPoll;
	//CSASData	m_sasReturn;
	BYTE * m_sendBuffer;
	UINT m_sendBufferSize;
	BYTE * m_responseBuffer;
	UINT m_responseBufferSize;

	binsem m_hResponseSem; // semiphore
	int			m_nReturnValue;
	void *		m_lpReturnBuffer;
	int			m_nReturnBufferLength;
	int			m_nNumberOfRetries;
	int			m_nCurrentRetryCount;
	byte		m_gameAddress;
	int			m_retryCount;
	bool		m_incRetryOnNoResponse;
	CSASQueueList *m_ownerQueue;
	int		m_priority;
	bool	m_responseReceived;

public:

	bool		m_bWaitForSASResponse;
	virtual void ResponseReceived();
	void ResetResponse();
	void WaitForResponse();
	bool VerifyResponseChecksum();
	bool VerifyResponsePollValue();
	virtual void SetReturnValue(int returnValue);
	virtual int GetReturnValue();

	static byte GetMeterCodeForMeterNumber( int meterNumber, int gameNumber, byte &primaryMeterCode, byte &secondaryMeterCode );
	bool		m_isVariableResponse;
	uint		m_variableResponseLenghLocation;
	uint		m_variableResponseLengthSize;
	uint		m_variableResponseSizeConstant; // how much to add to length to get total size.
	virtual void IncRetryCount();
	virtual int GetRetryCount();

	virtual void SetGameNumber(int gameNumber);

	bool IsWaitingForSASResponse(void)
	{
		return m_bWaitForSASResponse;
	}

	// Provide access to send/recv data
	BYTE *GetSendBuffer();
	BYTE *GetResponseBuffer();
	UINT GetSendBufferSize();
	UINT GetResponseBufferSize();
	
	void SetSendBuffer(BYTE *buffer, UINT size);
	void SetResponseBuffer(BYTE *buffer, UINT size);

	// Get the poll value for this message (i.e. 6F, 2F)
	virtual BYTE GetPollValue() = 0;

	/// <summary>
	/// If an alternative poll value is supported for this message, swap the poll value.
	/// i.e. AF can be swapped for 6F.
	/// </summary>
	/// <returns>	true if the message poll value was swapped, false otherwise</returns>
	virtual bool SwapPollValue();

	/// <summary>
	/// Indicates if Fill(DWORD tickCount) needs to be called (immediately) before sending the message.
	/// Indicates if the message contains time-sensitive information that needs to be discarded if stale or expired.
	/// </summary>
	/// <returns>true if Fill(DWORD tickCount) needs to be called (immediately) before sending the message, false otherwise.</returns>
	/// <remarks>
	/// After calling Fill(DWORD tickCount), if there is nothing to send then GetSendBufferSize() will return 0.
	/// </remarks>
	virtual bool FillWithTickCountBeforeSending();

	/// <summary>
	/// Fills the message after checking if stale or expired time-sensitive information needs to be discarded.
	/// </summary>
	/// <param name="tickCount">IN - The current tick count.</param>
	/// <remarks>
	/// After calling this method, if there is nothing to send then GetSendBufferSize() will return 0.
	/// </remarks>
	virtual void Fill(DWORD tickCount);

	virtual void Fill() = 0;
	virtual void Fill(bool doNothing)
	{
		UNREFERENCED_PARAMETER(&doNothing);
	}

	virtual bool IncRetryOnNoResponse();
	virtual void SetIncRetryOnNoResponse(bool incRetryOnNoResponse);

	/// <summary>
	/// Returns true if this instance sends general polls, false otherwise.
	/// </summary>
	/// <returns>true if this instance sends general polls, false otherwise.</returns>
	virtual bool IsGeneralPoll();

	/// <summary>
	/// Determines whether the response buffer contains a NACK.
	/// </summary>
	/// <param name="numberBytesRead">IN - The number bytes actually read so far.</param>
	/// <returns>true if the response buffer contains a NACK, false otherwise.</returns>
	/// <remarks>
	/// Use numberBytesRead instead of GetResponseBufferSize(), so caller can update the response buffer size independently.
	/// </remarks>
	bool IsNACK(DWORD numberBytesRead);

	void SetQueue(CSASQueueList *queueList, int priority);

	/// <summary>
	/// Converts a BCD buffer to a DWORD value.
	/// </summary>
	/// <param name="buffer">IN - The buffer.</param>
	/// <param name="bufferSize">IN - Size of the buffer.</param>
	/// <returns>The DWORD value.</returns>
	/// <remarks>
	/// Does not check for DWORD value overflow.
	/// Does not check for invalid BCD (BYTE) values -- for example: 0x9a or 0xa9.
	/// Returns 0 if buffer is nullptr.
	/// Returns 0 if bufferSize is 0.
	/// </remarks>
	static DWORD Bcd2Dword(const BYTE *buffer, UINT bufferSize);

	/// <summary>
	/// Converts a BCD buffer to an __int64 value.
	/// </summary>
	/// <param name="buffer">IN - The buffer.</param>
	/// <param name="bufferSize">IN - Size of the buffer.</param>
	/// <returns>The __int64 value.</returns>
	/// <remarks>
	/// Does not check for __int64 value overflow.
	/// Does not check for invalid BCD (BYTE) values -- for example: 0x9a or 0xa9.
	/// Returns 0 if buffer is nullptr.
	/// Returns 0 if bufferSize is 0.
	/// </remarks>
	static __int64 Bcd2Int64(const BYTE *buffer, UINT bufferSize);

	static bool Dword2Bcd(DWORD dwVal, byte *bcdVal, int BcdSize);

	/// <summary>
	/// Converts an __int64 value to a BCD value using the buffer provided.
	/// If the __int64 value is out of range then the BCD field is set to 0 and false is returned.
	/// </summary>
	/// <param name="value">IN - The __int64 value.</param>
	/// <param name="bcdVal">OUT - The output buffer for the BCD value.</param>
	/// <param name="BcdSize">IN - the size of the output buffer for the BCD value.</param>
	/// <returns>true if the value was successfully stored, false otherwise.</returns>
	static bool Int64ToBcd(__int64 value, byte *bcdVal, int BcdSize);

	const static unsigned SASPOLL_POLL_VALUE_OFFSET = 1; // Into a 0-based array.

	// Grant "friend" access for testing.
	friend class SentinelNativeUnitTests::SASPollMessage;
};
