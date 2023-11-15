#include "stdafx.h"
#include "CumulativeProgressiveManager.h"

const DWORD CumulativeProgressiveManager::NVRAM_ACTIVE_OFFSET = NVRAM_CUMULATIVE_PROGRESSIVE_CHECKBYTE_OFFSET;
const BYTE CumulativeProgressiveManager::NVRAM_ACTIVE_VALUE = NVRAM_CUMULATIVE_PROGRESSIVE_CHECKBYTE_VALUE;
const DWORD CumulativeProgressiveManager::NVRAM_DATA_OFFSET = NVRAM_CUMULATIVE_PROGRESSIVE_DATA_OFFSET;
const DWORD CumulativeProgressiveManager::NVRAM_DATA_SIZE = NVRAM_CUMULATIVE_PROGRESSIVE_DATA_SIZE;

CumulativeProgressiveManager::CumulativeProgressiveManager(MEMORY_TYPE memoryType)
	:
	CMemoryStorageEx(memoryType, NVRAM_DATA_OFFSET, NVRAM_DATA_SIZE, NVRAM_ACTIVE_OFFSET, NVRAM_ACTIVE_VALUE)
{
	m_currentCumulativeMeterValue = 0;
	m_hasCumulativeMeterAlready = false;

	BuildYourself();
}

void CumulativeProgressiveManager::SetFromNvramBuffer(const BYTE *buffer, UINT bufferSize)
{
	size_t offset(0);
	bool ok(true);

	CUtilities::MemCopyZero_FromOffset(&ok, &m_currentCumulativeMeterValue, sizeof(m_currentCumulativeMeterValue), buffer, bufferSize, &offset);
	CUtilities::MemCopyZero_FromOffset(&ok, &m_hasCumulativeMeterAlready, sizeof(m_hasCumulativeMeterAlready), buffer, bufferSize, &offset);
}

// Persist m_hasCumulativeMeterAlready, instead of using the active flag, so we can add persisted variables that are independent.
UINT CumulativeProgressiveManager::GetNvramBuffer(BYTE *buffer, UINT bufferSize, bool& active) const
{
	size_t offset(0);
	bool ok(true);

	active = true;
	CUtilities::MemCopySafe_ToOffset(&ok, buffer, bufferSize, &offset, &m_currentCumulativeMeterValue, sizeof(m_currentCumulativeMeterValue));
	CUtilities::MemCopySafe_ToOffset(&ok, buffer, bufferSize, &offset, &m_hasCumulativeMeterAlready, sizeof(m_hasCumulativeMeterAlready));

	return (UINT)offset;
}

bool CumulativeProgressiveManager::HasCumulativeMeterAlready()
{
	return m_hasCumulativeMeterAlready;
}

void CumulativeProgressiveManager::ClearHasCumulativeMeterStatus()
{
	m_hasCumulativeMeterAlready = false;
	m_currentCumulativeMeterValue = 0;

	StoreYourself();
}

__int64 CumulativeProgressiveManager::GetCumulativeDelta(__int64 amount)
{
	__int64 delta = 0;

	if (false == m_hasCumulativeMeterAlready)
	{
		LogString(
			PROGRESSIVE,
			_T("CumulativeProgressiveManager::GetCumulativeDelta did not have cumulative meter already - amount:%lld"),
			amount);

		m_hasCumulativeMeterAlready = true;
	}
	else
	{
		if (amount < m_currentCumulativeMeterValue)
		{
			LogString(
				PROGRESSIVE,
				_T("CumulativeProgressiveManager::GetCumulativeDelta cumulative meter sent up is lower than current amount- amount:%lld - Currentamount:%lld"),
				amount,
				m_currentCumulativeMeterValue);
		}
		else
		{
			delta = amount - m_currentCumulativeMeterValue;			
            LogString( PROGRESSIVE, _T("CumulativeProgressiveManager::GetCumulativeDelta previous cumulative %lld. New cumulative %lld. Cumulative delta %lld"), m_currentCumulativeMeterValue, amount, delta);
		}
	}

	m_currentCumulativeMeterValue = amount;
	StoreYourself();
	return delta;
}

