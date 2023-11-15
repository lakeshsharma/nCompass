#include "stdafx.h"
#include "RepairLog.h"
#include "../Hardware/MemoryStorage.h"
#include "../Utilities.h"

CRepairLog::CRepairLog(DWORD code)
{
	m_dateTime = CUtilities::GetCurrentTime();
	m_code = code;
}

CRepairLog::CRepairLog(byte *nvramBuffer)
{
	byte *bufferPos = nvramBuffer;

	m_dateTime = CUtilities::MakeTimeFromNvramBuffer(bufferPos, NVRAM_TIME_SIZE);
	bufferPos += NVRAM_TIME_SIZE;

	memcpy(&m_code, bufferPos, sizeof(m_code));
}

CRepairLog::CRepairLog(CRepairLog &rRHS)
{
	*this = rRHS;
}

CRepairLog::~CRepairLog()
{
}

CRepairLog & CRepairLog::operator=(CRepairLog &rRHS)
{
	m_dateTime = rRHS.GetDateTime();
	m_code = rRHS.GetRepairCode();

	return *this;
}

int CRepairLog::GetNvramBuffer(byte *nvramBuffer, int nvramBufferLength)
{
	byte *bufferPos = nvramBuffer;

	if (nvramBufferLength >= (int)NVRAM_REPAIR_CODES_LOG_DATA_SIZE)
	{
		CUtilities::GetNvramBufferFromTime(m_dateTime, bufferPos, NVRAM_TIME_SIZE);
		bufferPos += NVRAM_TIME_SIZE;

		memcpy(bufferPos, &m_code, sizeof(m_code));
		bufferPos += sizeof(m_code);
	}

	return (bufferPos - nvramBuffer);
}
