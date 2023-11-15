#include "stdafx.h"

#include "SubgameSession.h"
#include "Hardware/MemoryStorage.h"
#include "Utilities.h"
#include "Logging/LogString.h"
#include "metersdef.h"

const DWORD FIRST_GAME_START(1);

CSubgameSession::CSubgameSession(const SubgameSessionParameters& subgameSessionParameters) :
m_status(SubgameSessionStatus::ActiveWaitingForParameters),
m_subgameSessionParameters(subgameSessionParameters),
m_sessionGameStartCount(0)
{
	// Initialize meter delta map
	m_subgameSessionMeterDeltas[gCI] = 0;
	m_subgameSessionMeterDeltas[gCO] = 0;
	m_subgameSessionMeterDeltas[gJP] = 0;
	m_subgameSessionMeterDeltas[mBonusMach] = 0;
	m_subgameSessionMeterDeltas[mPrgPayAtt] = 0;
	m_subgameSessionMeterDeltas[mPrgPayMac] = 0;
	m_subgameSessionMeterDeltas[mRstrPlayd] = 0;
}

CSubgameSession::~CSubgameSession()
{
}

bool CSubgameSession::ApplySubgameSessionParameters(const SubgameSessionParameters& subgameSessionParameters)
{
	LogString(SESSION_LOG, "%s:%d := Multiplier: %u, Divider: %u, m_gameSessionSequenceNumber: %u, m_gameNumber: %u, m_denom : %u", __FUNCTION__, __LINE__, 
		subgameSessionParameters.m_multiplier, subgameSessionParameters.m_divider, subgameSessionParameters.m_gameSessionSequenceNumber, subgameSessionParameters.m_gameNumber, subgameSessionParameters.m_denom);

	bool success(false);

	if (m_subgameSessionParameters.m_gameSessionSequenceNumber == subgameSessionParameters.m_gameSessionSequenceNumber)
	{
		//Only set it to active if status is ActiveWaitingForParameters
		if (SubgameSessionStatus::ActiveWaitingForParameters == m_status)
		{
			m_status = SubgameSessionStatus::Active;
		}
		else if (SubgameSessionStatus::ClosedWaitingForParameters == m_status)
		{
			m_status = SubgameSessionStatus::Closed;
		}
		m_subgameSessionParameters = subgameSessionParameters;
		success = true;
	}

	return success;
}

bool CSubgameSession::ApplyMeterChange(CMeters *meters, CMeters *origMeters)
{
    bool bApplyMeterChange = false;

	if (SubgameSessionStatus::Active == m_status || SubgameSessionStatus::ActiveWaitingForParameters == m_status)
	{
	
		// It is possible that the coinIn meter will come in following a "game end" and get processed before the high priority meters get processed.
		// If this happens, the new coinIn will get processed before all of the other meters that go into point calculations.
		// When the High Priority meters do show up, the new coinIn will have already been processed and won't look like it has changed causing the
		// points that should be awarded to be missed.	Need to keep a flag of an unprocessed coinIn change and 'mRstrPlayd' meter change specially if
		// going to calculate for ACCOUNTING_CASHABLE_PLAYED and ACCOUNTING_CASHABLE_THEORETICAL.
		std::map<DWORD, bool> metersReceived;
		metersReceived[gCI] = false;
		metersReceived[mRstrPlayd] = false;
		
		if (meters)
		{			
			if (meters->GetMeterByNumber(gCI))
			{
				metersReceived[gCI] = true;
			}
			
			if (meters->GetMeterByNumber(mRstrPlayd))
			{
				metersReceived[mRstrPlayd] = true;
			}
		}

		int meterCounts = meters->GetMeterCount();
		
		for (int mi = 0; mi < meterCounts; ++mi)
		{
			CMeter *meter = meters->GetMeterByIndex(mi);
			byte meterNumber = meter->GetMeterNumber();
			CMeter *origMeter = origMeters->GetMeterByNumber(meterNumber);
			if (meter->GetValue() != INVALID_METER_VALUE && origMeter != nullptr && origMeter->GetValue() != INVALID_METER_VALUE && ( meterNumber == gCI || meterNumber == gCO || meterNumber == gJP  || meterNumber == mBonusMach || meterNumber == mPrgPayAtt || meterNumber == mPrgPayMac || meterNumber == mRstrPlayd ))
			{			 
				LogString(SESSION_LOG, "%s %d meterCounts: %d meter number: %d curMeter: %lld, origMeter: %lld, AccType: %d", 
					__FUNCTION__, __LINE__, meterCounts, meterNumber, meter->GetValue(), origMeter->GetValue(), 
					m_subgameSessionParameters.m_accountingType);
				// Add meterDelta to proper bucket
				uint64_t meterDelta = origMeters->CompareMeter(meter);
				
				if (meterDelta > 0)
				{
					m_subgameSessionMeterDeltas[meterNumber] += meterDelta;
					switch (m_subgameSessionParameters.m_accountingType)
					{
					case ACCOUNTING_COINS_IN:
						if ( meterNumber == gCI )
						{
							bApplyMeterChange = true;
						}
						break;
					case ACCOUNTING_COINS_OUT:
						if ( meterNumber == gCO )
						{
							bApplyMeterChange = true;
						}
						break;
					case ACCOUNTING_COINS_WON:
						if ( meterNumber == gCO || meterNumber == gJP  || meterNumber == mBonusMach || meterNumber == mPrgPayAtt || meterNumber == mPrgPayMac )
						{
							bApplyMeterChange = true;
						}
						break;
					case ACCOUNTING_THEORETICAL:
						if ( meterNumber == gCI )
						{
							// Adjust meterChange and multiply by PAR.
							// TBD. PAR is not currently set in the config.
							bApplyMeterChange = true;
						}
						break;
					case ACCOUNTING_CASHABLE_PLAYED:
					case ACCOUNTING_CASHABLE_THEORETICAL:	
						LogString(SESSION_LOG, "%s %d gCI: %d mRstrPlayd: %d", __FUNCTION__, __LINE__, metersReceived[gCI], metersReceived[mRstrPlayd]);
						if (metersReceived[gCI] && metersReceived[mRstrPlayd])
							bApplyMeterChange = true;
						break;

					default:
						bApplyMeterChange = false;
					}
				}
			}
		}
	}

	return bApplyMeterChange;
}

bool  CSubgameSession::CloseSession(DWORD gameSessionSequenceNumber)
{
	bool success(false);

	if (m_subgameSessionParameters.m_gameSessionSequenceNumber == gameSessionSequenceNumber)
	{
		if (SubgameSessionStatus::ActiveWaitingForParameters == m_status)
		{
			m_status = SubgameSessionStatus::ClosedWaitingForParameters;
		}
		else if (SubgameSessionStatus::ClosedWaitingForParameters != m_status)
		{
			m_status = SubgameSessionStatus::Closed;
		}
		success = true;
	}

	return success;
}

bool CSubgameSession::IncrementGameStart()
{
	bool firstGameStart(false);

	if (SubgameSessionStatus::Active == m_status || SubgameSessionStatus::ActiveWaitingForParameters == m_status)
	{
		firstGameStart = ++m_sessionGameStartCount == FIRST_GAME_START;
	}

	return firstGameStart;
}

DWORD CSubgameSession::GetMeterDelta(bool &tilted, bool coinInOverride) const
{
	if (coinInOverride)
	{
		return m_subgameSessionMeterDeltas.at(gCI);
	}

	// Return meterdelta based on subgame session pointcalc method.
	DWORD subgameSessionMeterDelta = 0;

	switch (m_subgameSessionParameters.m_accountingType)
	{
	case ACCOUNTING_COINS_IN:
		subgameSessionMeterDelta = m_subgameSessionMeterDeltas.at(gCI);
		break;
	case ACCOUNTING_COINS_OUT:
		subgameSessionMeterDelta = m_subgameSessionMeterDeltas.at(gCO);
		break;
	case ACCOUNTING_COINS_WON:
		subgameSessionMeterDelta = m_subgameSessionMeterDeltas.at(gCO) + m_subgameSessionMeterDeltas.at(gJP) + m_subgameSessionMeterDeltas.at(mBonusMach) +
			m_subgameSessionMeterDeltas.at(mPrgPayAtt) + m_subgameSessionMeterDeltas.at(mPrgPayMac);
		break;
	case ACCOUNTING_THEORETICAL:
		subgameSessionMeterDelta = m_subgameSessionMeterDeltas.at(gCI);
		break;
	case ACCOUNTING_CASHABLE_PLAYED:
		LogString(SESSION_LOG, "%s %d m_subgameSessionMeterDeltas[gCI]: %d	m_subgameSessionMeterDeltas[mRstrPlayd]: %d", __FUNCTION__, __LINE__, m_subgameSessionMeterDeltas.at(gCI),  m_subgameSessionMeterDeltas.at(mRstrPlayd));
		if (m_subgameSessionMeterDeltas.at(mRstrPlayd) <= m_subgameSessionMeterDeltas.at(gCI))
		{
			subgameSessionMeterDelta = m_subgameSessionMeterDeltas.at(gCI) - m_subgameSessionMeterDeltas.at(mRstrPlayd);
		}
		else
		{
			tilted = true;
			subgameSessionMeterDelta = 0;
		}
		break;
	case ACCOUNTING_CASHABLE_THEORETICAL:
		LogString(SESSION_LOG, "%s %d m_subgameSessionMeterDeltas[gCI]: %d	m_subgameSessionMeterDeltas[mRstrPlayd]: %d", __FUNCTION__, __LINE__, m_subgameSessionMeterDeltas.at(gCI),  m_subgameSessionMeterDeltas.at(mRstrPlayd));
		if (m_subgameSessionMeterDeltas.at(gCI) + m_subgameSessionMeterDeltas.at(gJP) + m_subgameSessionMeterDeltas.at(mBonusMach) +
			m_subgameSessionMeterDeltas.at(mPrgPayAtt) + m_subgameSessionMeterDeltas.at(mPrgPayMac) >= m_subgameSessionMeterDeltas.at(mRstrPlayd))
		{
			subgameSessionMeterDelta = m_subgameSessionMeterDeltas.at(gCI) + m_subgameSessionMeterDeltas.at(gJP) + m_subgameSessionMeterDeltas.at(mBonusMach) +
				m_subgameSessionMeterDeltas.at(mPrgPayAtt) + m_subgameSessionMeterDeltas.at(mPrgPayMac) - m_subgameSessionMeterDeltas.at(mRstrPlayd);
		}
		else
		{
			tilted = true;
			subgameSessionMeterDelta = 0;
		}
		break;
	default:
		subgameSessionMeterDelta = 0;
		break;
	}
	
	return subgameSessionMeterDelta;
}

const SubgameSessionParameters & CSubgameSession::GetSubgameSessionParameters() 
{
	return m_subgameSessionParameters;
}

SubgameSessionStatus CSubgameSession::GetStatus() const
{
	return m_status;
}

int CSubgameSession::GetGameStartCount() const
{
	return m_sessionGameStartCount;
}

bool CSubgameSession::IsSameGameNumberAndDenom(CSubgameSession *sess) const
{
	return sess->GetSubgameSessionParameters().m_gameNumber == m_subgameSessionParameters.m_gameNumber && sess->GetSubgameSessionParameters().m_denom == m_subgameSessionParameters.m_denom;
}

void CSubgameSession::SetStatus(SubgameSessionStatus status)
{
	m_status = status;
}
