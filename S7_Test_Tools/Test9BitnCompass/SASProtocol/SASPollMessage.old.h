#pragma once
#include "SASData.h"
//#include "../QueueItem.h"
#include "../metersdef.h"
#include "types.h"

const byte INVALID_LONG_POLL = 0;

const byte INVALID_SAS_METER_CODE = 0xff;
const int SASPOLL_RETRY_COUNT = 2;
const int SASPOLL_INCOMPLETE_DATA = 1;
const int SASPOLL_NACK = 2;
const int SASPOLL_SUCCESS = 0;
const int SASPOLL_DATA_TOO_LARGE = 3;
const int LPCOMMANDLOC = 1;
const int GAMEADDRLOC = 0;

typedef struct _meterCodeMapping
{
	int oasisMeterNumber;
	byte sasMeterCode;
	byte sasMeterCodeSecondary;
} meterCodeMapping;

const int NUM_MAIN_GAME_METER_CODE_MAPPINGS = 57;
const meterCodeMapping mainGameMeterCodeMap[NUM_MAIN_GAME_METER_CODE_MAPPINGS] = 
{
	{mCD,		0x0A, INVALID_SAS_METER_CODE},
	{mD1,		0x40, INVALID_SAS_METER_CODE},	//{Ones}
	{mD2,		0x41, INVALID_SAS_METER_CODE},	//{Twos}
	{mD5,		0x42, INVALID_SAS_METER_CODE},	//{Fives}
	{mD10,		0x43, INVALID_SAS_METER_CODE},	//{Tens}
	{mD20,		0x44, INVALID_SAS_METER_CODE},	//{Twenties}
	{mD50,		0x46, INVALID_SAS_METER_CODE},	//{Fifties}
	{mD100,		0x47, INVALID_SAS_METER_CODE},	//{Hundreds}
	{mCrd,		0x0c, INVALID_SAS_METER_CODE},
	{mTCi,		0x08, INVALID_SAS_METER_CODE},
	{mTCo,		0x09, INVALID_SAS_METER_CODE},
	{mBD,		0x0b, INVALID_SAS_METER_CODE},
	{mTHP,		0x23, INVALID_SAS_METER_CODE},
	{mActualCD, 0x0a, INVALID_SAS_METER_CODE},
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
	{mTValueCI, 0x08, INVALID_SAS_METER_CODE},
	{mTValueCO, 0x09, INVALID_SAS_METER_CODE},
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

class CSASPollMessage //: public CQueueItem
{
public:
	CSASPollMessage(byte gameAddress);
public:
	static DWORD Bcd2Dword(BYTE *buffer, UINT bufferSize);
	static __int64 Bcd2Int64(BYTE *buffer, UINT bufferSize);
	static bool Dword2Bcd(DWORD dwVal, byte *bcdVal, int BcdSize);
	static unsigned short CRC(unsigned char *s, int len, unsigned short crcval = 0);

	virtual ~CSASPollMessage(void);

protected:


	//static bool Dword2Bcd(DWORD dwVal, byte *bcdVal, int BcdSize);
	//unsigned short CRC(unsigned char *s, int len, unsigned short crcval = 0);
	//int GetMeterNumberForMeterCode( byte meterCode, int gameNumber );

	//CSASData	m_sasPoll;
	//CSASData	m_sasReturn;
	BYTE * m_sendBuffer;
	UINT m_sendBufferSize;
	BYTE * m_responseBuffer;
	UINT m_responseBufferSize;

	
	void		*m_hResponseEventHandle;
	int			m_nReturnValue;
	void *		m_lpReturnBuffer;
	int			m_nReturnBufferLength;
	//int			m_nNumberOfRetries;
	//int			m_nCurrentRetryCount;
	byte		m_gameAddress;
	int			m_retryCount;

public:

	bool		m_bWaitForSASResponse;
	virtual void ResponseReceived();
	void WaitForResponse();
	bool VerifyResponseChecksum();
	//virtual void SetReturnValue(int returnValue);
	//virtual int GetReturnValue();

	bool		m_isVariableResponse;
	uint		m_variableResponseLenghLocation;
	uint		m_variableResponseLengthSize;
	uint		m_variableResponseSizeConstant; // how much to add to length to get total size.
	virtual void IncRetryCount();
	virtual int GetRetryCount();

	//virtual void SetGameNumber(int gameNumber);

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

//	HANDLE GetResponseEventHandle( void )
//	{
//		return m_hResponseEventHandle;
//	}

	// Get the poll value for this message (i.e. 6F, 2F)
	virtual BYTE GetPollValue() = 0;

	// If an alternative value can be done for this poll, swap it.
	// i.e. AF can be swapped for 6F.
	// Returns true if value can be swapped.
	virtual bool SwapPollValue() = 0;

	virtual void Fill() = 0;
	virtual void Fill(bool doNothing)
	{
		UNREFERENCED_PARAMETER(doNothing);
	}

};
