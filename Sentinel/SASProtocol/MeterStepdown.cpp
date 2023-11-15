#include "stdafx.h"
#include "MeterStepdown.h"
#include "../metersdef.h"
#include "SASPollMessage.h"
#include "../Sentinel.h"
#include "Logging/LogString.h"

using namespace std;

CMeterStepdown::CMeterStepdown(void)
{

	return;
}

CMeterStepdown::~CMeterStepdown(void)
{
	meterMap.erase(meterMap.begin(), meterMap.end());
	meterCodeMap.erase(meterCodeMap.begin(), meterCodeMap.end());
}

void CMeterStepdown::AddMeterItem(byte oasisMeter, byte primaryLongPoll, byte secondaryLongPoll, byte fallback, byte secondFallback)
{
	m_cs.lock();
	CMeterStepdownItem *sdMeter;
	sdMeter = BuildMeterStepdownItem(oasisMeter, primaryLongPoll, secondaryLongPoll, fallback, secondFallback);
	meterMap[oasisMeter] = *sdMeter;
	delete sdMeter;
	m_cs.unlock();
	return;
}

CMeterStepdownItem* CMeterStepdown::BuildMeterStepdownItem(byte meter, byte primaryLongPoll, byte secondaryLongPoll, byte fallback, byte secondFallback)
{
	CMeterStepdownItem *sdMeter = new CMeterStepdownItem(meter, primaryLongPoll, secondaryLongPoll, fallback, secondFallback);
	return sdMeter;
}

CMeterStepdownItem::CMeterStepdownItem(byte meter, byte primaryLongPoll, byte secondaryLongPoll, byte fallback, byte secondFallback)
{
	m_meter = meter;
	m_primaryLongPoll = primaryLongPoll;
	m_secondaryLongPoll = secondaryLongPoll;
	m_fallbackLongPoll = fallback;
	m_secondFallbackLongPoll = secondFallback;
	m_primaryCount = METER_RETRY_COUNT;
	m_secondaryCount = METER_RETRY_COUNT;
	m_fallbackCount = METER_RETRY_COUNT;
	m_secondFallbackCount = METER_RETRY_COUNT;
	m_AtLeastOneSuccess = false;
	return;
}

CMeterStepdownItem::CMeterStepdownItem(void)
{
}

CMeterStepdownItem::~CMeterStepdownItem(void)
{
}

byte CMeterStepdown::GetMeterLongPoll(byte internalMeterNumber)
{
	byte retPoll = INVALID_LONG_POLL;
	m_cs.lock();

	auto meterItem = meterMap.find(internalMeterNumber);
	if (meterItem != meterMap.end())
	{
        // found meter item.
		if (meterItem->second.m_primaryCount > 0)
		{
            retPoll = meterItem->second.m_primaryLongPoll;
		}
		else if (meterItem->second.m_secondaryCount > 0)
		{
			retPoll = meterItem->second.m_secondaryLongPoll;
		}
		else if (meterItem->second.m_fallbackCount > 0)
		{
			retPoll = meterItem->second.m_fallbackLongPoll;
		}
		else if (meterItem->second.m_secondFallbackCount > 0)
		{
			retPoll = meterItem->second.m_secondFallbackLongPoll;
		}
	}
//CString logStr;
//logStr.Format(L"Get Meter stepdown %X", retPoll);
//((CSentinelApp *)AfxGetApp())->m_eLog.LogString("SASGENERAL", logStr);
	m_cs.unlock();
	return retPoll;
}

byte CMeterStepdown::GetOasisMeter(byte meterCode)
{
	byte oasisMeter;
	auto oasisMeterIt = meterCodeMap.find(meterCode);
	m_cs.lock();
	if (oasisMeterIt != meterCodeMap.end())
	{
		oasisMeter = oasisMeterIt->second;
	}
	else
	{
		oasisMeter = mNone;
	}
	m_cs.unlock();
	return oasisMeter;
}

void CMeterStepdown::MeterLongPollFail(byte meter)
{
	m_cs.lock();
	auto meterIt = meterMap.find(meter);
	CMeterStepdownItem meterItem;
	if (meterIt != meterMap.end())
	{
		meterItem = meterIt->second;
		if (!meterItem.m_AtLeastOneSuccess) //TR# 28195 once we have a successful meter read, don't fall back again.
		{

			// found meter item.
			if (meterItem.m_primaryCount > 0)
			{
				meterItem.m_primaryCount--;
				LogString("SASGENERAL", "Fail LongPoll %X, primaryCount %d, Meter %d", meterItem.m_primaryLongPoll, meterItem.m_primaryCount, meterItem.m_meter);
			}
			else if (meterItem.m_secondaryCount > 0)
			{
				meterItem.m_secondaryCount--;
				LogString("SASGENERAL", "Fail LongPoll %X, secondaryCount %d, Meter %d", meterItem.m_secondFallbackLongPoll, meterItem.m_secondaryCount, meterItem.m_meter);
			}
			else if (meterItem.m_fallbackCount > 0)
			{
				meterItem.m_fallbackCount--;
				LogString("SASGENERAL", "Fail LongPoll %X, fallbackCount %d, Meter %d", meterItem.m_fallbackLongPoll, meterItem.m_fallbackCount, meterItem.m_meter);
			}
			else if (meterItem.m_secondFallbackCount > 0)
			{
				meterItem.m_secondFallbackCount--;
				LogString("SASGENERAL", "Fail LongPoll %X, secondaryFallbackCount %d, Meter %d", meterItem.m_secondFallbackLongPoll, meterItem.m_secondFallbackCount, meterItem.m_meter);
			}
			meterMap[meter] = meterItem;
		}
	}
	m_cs.unlock();
}




void CMeterStepdown::MeterLongPollSuccessful(byte meter)
{
	m_cs.lock();
	auto meterIt = meterMap.find(meter);
	CMeterStepdownItem meterItem;
	if (meterIt != meterMap.end())
	{
		meterItem = meterIt->second;
		meterItem.m_AtLeastOneSuccess = true;
		meterMap[meter] = meterItem;
	}
	m_cs.unlock();
}

void CMeterStepdown::DeactivateForSAS5x()
{

	m_cs.lock();
	using value_type = std::map<const short, CMeterStepdownItem>::value_type;
	
	// deactivate all 6F's because it is not supported on this EGM.
	// it takes too long to step down all 6F calls, watchdog might fire.
	for_each(meterMap.begin(), meterMap.end(), []( value_type &item)
    {
        if (item.second.m_primaryLongPoll == 0x6f)
		{
			item.second.m_primaryCount = 0;
        }
	});
	
	// deactivate 2F's with meter codes > 0x28 becaue they are not supported in SAS 5.x
	for_each(meterMap.begin(), meterMap.end(), [](value_type &item)
	{
		if (item.second.m_secondaryLongPoll == 0x2f)
		{
			byte primaryMeterCode, secondaryMeterCode;
			byte meterCode = CSASPollMessage::GetMeterCodeForMeterNumber(item.second.m_meter, 0, primaryMeterCode, secondaryMeterCode);

			// Tracker# 018314:
			// Don't deactivate 2f poll if metercode <= 0x28.
			if (meterCode > 0x28 && (secondaryMeterCode == INVALID_SAS_METER_CODE || secondaryMeterCode > 0x28))
				item.second.m_secondaryCount = 0;  // this meter code is not supported
		}
	});
	
	m_cs.unlock();
}

void CMeterStepdown::DeactivateForSAS3x()
{
	DeactivateForSAS4x();

	using value_type = std::map<const short, CMeterStepdownItem>::value_type;

	// Deactivate additional meters beyond those deactivated for SAS 4
	m_cs.lock();
	for_each(meterMap.begin(), meterMap.end(), [](value_type& item)
		{
			if (item.second.m_primaryLongPoll == 0x2A) // Send true coin in
				item.second.m_primaryCount = 0;
			else if (item.second.m_primaryLongPoll == 0x2B) // Send true coin out
				item.second.m_primaryCount = 0;
		});

	m_cs.unlock();
}

void CMeterStepdown::DeactivateForSAS4x()
{

	DeactivateForSAS5x();
	using value_type = std::map<const short, CMeterStepdownItem>::value_type;

	// deactivate all 2F's because it is not supported on this EGM.
	// it takes too long to step down all 6F calls, watchdog might fire.

	m_cs.lock();
	for_each(meterMap.begin(), meterMap.end(), [](value_type& item)
		{
			if (item.second.m_secondaryLongPoll == 0x2f)
				item.second.m_secondaryCount = 0;
		});

	m_cs.unlock();
}

void CMeterStepdown::ResetAllStepdowns()
{
	using value_type = std::map<const short, CMeterStepdownItem>::value_type;

	m_cs.lock();
	for_each (meterMap.begin(), meterMap.end(), [](value_type &item)
	{
		item.second.m_primaryCount = METER_RETRY_COUNT;
		item.second.m_secondaryCount = METER_RETRY_COUNT;
		item.second.m_fallbackCount = METER_RETRY_COUNT;
		item.second.m_secondFallbackCount = METER_RETRY_COUNT;
		item.second.m_AtLeastOneSuccess = false;
	});

	m_cs.unlock();
}


