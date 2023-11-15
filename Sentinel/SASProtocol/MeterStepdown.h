#pragma once
#include "binsem.h"

#define METER_RETRY_COUNT 3

class CMeterStepdownItem
{
public:
	CMeterStepdownItem(void);
	CMeterStepdownItem(byte meter, byte primaryLongPoll, byte secondaryLongPoll, byte fallback, byte secondFallback );
	~CMeterStepdownItem(void);

	byte m_meter;
	short m_primaryCount;
	byte m_primaryLongPoll;
	short m_secondaryCount;
	byte m_secondaryLongPoll;
	short m_fallbackCount;
	byte m_fallbackLongPoll;
	short m_secondFallbackCount;
	byte m_secondFallbackLongPoll;
	bool m_AtLeastOneSuccess;
};

//typedef CMap<short,short,CMeterStepdownItem, CMeterStepdownItem&> StepdownMap;
//typedef CMap<byte,byte,byte,byte> MeterCodeMap;
typedef std::map<short,CMeterStepdownItem> StepdownMap;
typedef std::map<byte,byte> MeterCodeMap;

class CMeterStepdown 
{
public:
	CMeterStepdown(void);
public:
	~CMeterStepdown(void);

	void AddMeterItem(byte oasisMeter, byte primaryLongPoll, byte secondaryLongPoll, byte fallback, byte seconfFallback);

	/// <summary>
	/// Gets the long poll value for the meter. INVALID_LONG_POLL is returned if no long poll available.
	/// </summary>
	/// <param name="meter">IN - The meter number.</param>
	/// <returns>The long poll value for the meter. INVALID_LONG_POLL is returned if no long poll available.</returns>
	byte GetMeterLongPoll(byte meter);

	byte GetOasisMeter(byte meterCode);
	void MeterLongPollFail(byte meter);
	void MeterLongPollSuccessful(byte meter);
	void DeactivateForSAS5x();
	void DeactivateForSAS3x();
	void DeactivateForSAS4x();
	void ResetAllStepdowns();

	WORD GetMeterCount( void )
	{
		return meterMap.size();
	}

private:
	binsem m_cs;
	StepdownMap meterMap;
	MeterCodeMap meterCodeMap;
	CMeterStepdownItem *BuildMeterStepdownItem(byte meter, byte primaryLongPoll, byte secondaryLongPoll, byte fallback, byte secondFallback);
};

