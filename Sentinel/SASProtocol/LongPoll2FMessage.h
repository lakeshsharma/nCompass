#pragma once
#include "MetersLongPoll.h"
#include "../Meters.h"

#define X2F_EXTRADATASIZE 7
#define X2F_RESPONSE_EXTRADATASIZE 5
#define X2F_CRCSIZE 5
#define X2F_LENGTHBYTE 2
#define X2F_LENGTHSIZE 1
#define X2F_DEFMETERSIZE 6
#define X2F_GAMENUMBERLOCATION 3
#define X2F_GAMENUMBERSIZE 2
#define X2F_METERLOCATION 5

const int X2F_MAX_METERS = 10;
const int LP2F = 0x2f;


// Declare friend class(es) for unit test access.
class LongPoll2FMessage;

class CLongPoll2FMessage :
	public CMetersLongPoll
{
public:
	static const byte POLL_VALUE = 0x2f;
	static const int DEFAULT_METER_VALUE_SIZE = 4;
	static const int LONG_METER_VALUE_SIZE = 5;

	// Tracker# 018314:
	// Added sas version so that unsupported metercodes can be ignored
	/// <summary>
	/// Standard constructor. Initializes a new instance of the <see cref="CLongPoll2FMessage"/> class.
	/// </summary>
	/// <param name="gameAddress">IN - The SAS game address.</param>
	/// <param name="meters">IN/OUT - The meters requested.</param>
	/// <remarks>
	/// The caller is responsible for the lifetime of "meters".
	/// The lifetime of "meters" should exceed the lifetime of the instance.
	/// </remarks>
	CLongPoll2FMessage(byte gameAddress,CMeters *meters);

	/// <summary>
	/// Destructor. Finalizes an instance of the <see cref="CLongPoll2FMessage"/> class.
	/// </summary>
	~CLongPoll2FMessage(void);

	
	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill(bool isIgtS2000);
	void Fill();
	void SetGameNumber(int gameNumber);
	void SetDenomMultiplier(WORD denom);

private:
	__int64 ConvertToCents(byte meterCode, __int64 meterValue );
	void MeterReceived(bool &ok, const byte *&response, const byte *responseEnd);

	// Tracker# 018314:
	//int m_version;
	int m_gameNumber;
	WORD m_denomMult;
	bool m_isIgtS2000;

	// Grant "friend" access for testing.
	friend class LongPoll2FMessage;
};

