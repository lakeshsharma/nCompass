#include "stdafx.h"
#include "Hardware/MemoryStorageEx.h"
#include "Logging/LogString.h"

CMemoryStorageEx::CMemoryStorageEx() :
	m_activeOffset(NVRAM_DEFAULT_OFFSET),
	m_activeValue(0),
	m_activeOffsetIsSet(false)
{
}

CMemoryStorageEx::CMemoryStorageEx(MEMORY_TYPE memoryType,
	DWORD dataOffset,
	DWORD dataSize,
	DWORD activeOffset,
	BYTE activeValue) :
	m_activeOffset(activeOffset),
	m_activeValue(activeValue),
	m_activeOffsetIsSet(true)
{
	m_type = memoryType;
	m_dwOffset = dataOffset;
	m_dwAllowedSize = dataSize;
	m_bOffsetIsSet = true;
}

CMemoryStorageEx::~CMemoryStorageEx()
{
}

void CMemoryStorageEx::SetMemoryStorageParameters(MEMORY_TYPE memoryType,
	DWORD dataOffset,
	DWORD dataSize,
	DWORD activeOffset,
	BYTE activeValue)
{
	m_type = memoryType;
	m_dwOffset = dataOffset;
	m_dwAllowedSize = dataSize;
	m_bOffsetIsSet = true;

	m_activeOffset = activeOffset;
	m_activeValue = activeValue;
	m_activeOffsetIsSet = true;
}

void CMemoryStorageEx::ClearMemoryStorageParameters()
{
	m_type = MEMORY_NONE;
	m_dwOffset = NVRAM_DEFAULT_OFFSET;
	m_dwAllowedSize = NVRAM_DEFAULT_SIZE;
	m_bOffsetIsSet = false;

	m_activeOffset = NVRAM_DEFAULT_OFFSET;
	m_activeValue = NVRAM_DEFAULT_ACTIVE_VALUE;
	m_activeOffsetIsSet = false;
}

DWORD CMemoryStorageEx::GetNvramActiveOffset()
{
	return m_activeOffset;
}

void CMemoryStorageEx::BuildYourself()
{
	BuildYourself(false);
}

void CMemoryStorageEx::BuildYourself(bool buildInactive)
{
	if (m_type == MEMORY_NVRAM && m_bOffsetIsSet)
	{
		if (CMemoryStorage::IntegrityCheck())
		{
			bool active(IsActiveLocation(m_activeOffset, m_activeValue));
			if (active || buildInactive)
			{
				std::vector<BYTE> nvramBuffer(m_dwAllowedSize);
				if (!Read(m_dwOffset, nvramBuffer.data(), m_dwAllowedSize))
				{
					// Report the offset to indicate which derived class failed.
					LogString(ERROR_LOG, _T("Could not read memory storage data from NVRAM offset=%u size=%u"),
						(unsigned)m_dwOffset,
						(unsigned)m_dwAllowedSize);
				}
				else
				{
					SetFromNvramBuffer(nvramBuffer.data(), m_dwAllowedSize, active);
				}
			}
		}
		else
		{
			SetActiveLocation(false);
		}
	}
}

void CMemoryStorageEx::StoreYourself()
{
	if (m_type == MEMORY_NVRAM && m_bOffsetIsSet)
	{
		bool active(false);
		std::vector<BYTE> nvramBuffer(m_dwAllowedSize);
		UINT bytesInBuffer = GetNvramBuffer(nvramBuffer.data(), m_dwAllowedSize, active);
		if (bytesInBuffer > 0)
		{
			if (!Write(m_dwOffset, nvramBuffer.data(), bytesInBuffer))
			{
				// Report the offset to indicate which derived class failed.
				LogString(ERROR_LOG, _T("Could not write memory storage data to NVRAM offset=%u size=%u"),
					(unsigned)m_dwOffset,
					(unsigned)m_dwAllowedSize);
				active = false;
			}
		}

		SetActiveLocation(active);
	}
}

void CMemoryStorageEx::SetFromNvramBuffer(const BYTE *buffer, UINT bufferSize)
{
	UNREFERENCED_PARAMETER(buffer);
	UNREFERENCED_PARAMETER(bufferSize);
}

void CMemoryStorageEx::SetFromNvramBuffer(const BYTE *buffer, UINT bufferSize, bool active)
{
	if (active)
	{
		SetFromNvramBuffer(buffer, bufferSize);
	}
}

void CMemoryStorageEx::SetActiveLocation(bool active)
{
	if (m_type == MEMORY_NVRAM && m_activeOffsetIsSet)
	{
		BYTE value(active ? m_activeValue : (BYTE)~m_activeValue);
		CMemoryStorage::SetActiveLocation(m_activeOffset, value);
	}
}
