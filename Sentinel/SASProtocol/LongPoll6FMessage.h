#pragma once
#include "MetersLongPoll.h"
#include "../Meters.h"

#define X6F_EXTRADATASIZE 7
#define X6F_RESPONSE_EXTRADATASIZE 5
#define X6F_LENGTHBYTE 2
#define X6F_LENGTHSIZE 1
#define X6F_DEFMETERSIZE 12
#define X6F_GAMENUMBERLOCATION 3
#define X6F_GAMENUMBERSIZE 2
#define X6F_METERLOCATION 5

const int X6F_MAX_METERS = 12;
const int LP6F = 0x6f;


// Declare friend class(es) for unit test access.
namespace SentinelUnitTests { class LongPoll6FMessage; }

class CLongPoll6FMessage :
	public CMetersLongPoll
{
public:
	CLongPoll6FMessage(byte gameAddress, CMeters *meters);
	~CLongPoll6FMessage(void);


	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	virtual void Fill();
	virtual void Fill(bool isIgtS2000);

	CMeters *GetResult( void );
	void ResponseReceived();
	void SetGameNumber(int gameNumber);
	void SetDenomMultiplier(WORD denom);

private:
	BYTE m_pollValue;
	int m_gameNumber;
	WORD m_denomMult;
	bool m_isIgtS2000;

	__int64 ConvertToCents(byte meterCode, __int64 meterValue);
	void MeterReceived(bool &ok, const BYTE *&response, const BYTE *responseEnd);

	// Grant "friend" access for testing.
	friend class SentinelUnitTests::LongPoll6FMessage;
};

